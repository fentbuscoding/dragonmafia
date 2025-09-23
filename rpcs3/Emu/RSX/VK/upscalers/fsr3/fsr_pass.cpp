#include "../../vkutils/barriers.h"
#include "../../vkutils/image_helpers.h"
#include "../../VKHelpers.h"
#include "../../VKResourceManager.h"
#include "Emu/RSX/RSXTexture.h"

#include "../fsr3_pass.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-qualifiers"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#define A_CPU 1
#include "3rdparty/GPUOpen/include/ffx_a.h"
#include "3rdparty/GPUOpen/include/ffx_fsr3.h"
#undef A_CPU

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

namespace vk
{
	namespace FidelityFX
	{
		fsr3_pass::fsr3_pass(const std::string& config_definitions, u32 push_constants_size_)
		{
			// Use AMD-provided source with FSR3 modifications
			const char* shader_core =
				#include "Emu/RSX/Program/Upscalers/FSR3/fsr_ubershader.glsl"
			;

			// Replacements
			const char* ffx_a_contents =
				#include "Emu/RSX/Program/Upscalers/FSR1/fsr_ffx_a_flattened.inc"
			;

			const char* ffx_fsr_contents =
				#include "Emu/RSX/Program/Upscalers/FSR3/fsr_ffx_fsr3_flattened.inc"
			;

			const std::pair<std::string_view, std::string> replacement_table[] =
			{
				{ "%FFX_DEFINITIONS%", config_definitions },
				{ "%FFX_A_IMPORT%", ffx_a_contents },
				{ "%FFX_FSR_IMPORT%", ffx_fsr_contents },
				{ "%push_block%", "push_constant" }
			};

			m_src = shader_core;
			m_src = fmt::replace_all(m_src, replacement_table);

			// Fill with 0 to avoid sending incomplete/unused variables to the GPU
			memset(m_constants_buf, 0, sizeof(m_constants_buf));

			// No ssbo usage
			ssbo_count = 0;

			// Enable push constants
			use_push_constants = true;
			push_constants_size = push_constants_size_;

			create();
		}

		std::vector<glsl::program_input> fsr3_pass::get_inputs()
		{
			std::vector<glsl::program_input> inputs = 
			{
				glsl::program_input::make(
					::glsl::program_domain::glsl_compute_program,
					"InputTexture",
					vk::glsl::input_type_texture,
					0,
					0
				),

				glsl::program_input::make(
					::glsl::program_domain::glsl_compute_program,
					"OutputTexture", 
					vk::glsl::input_type_storage_texture,
					0,
					1
				),
			};

			auto result = compute_task::get_inputs();
			result.insert(result.end(), inputs.begin(), inputs.end());
			return result;
		}

		void fsr3_pass::bind_resources(const vk::command_buffer& /*cmd*/)
		{
			// Create sampler if needed
			if (!m_sampler)
			{
				const auto pdev = vk::get_current_renderer();
				m_sampler = std::make_unique<vk::sampler>(*pdev,
					VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					VK_FALSE, 0.f, 1.f, 0.f, 0.f, VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK);
			}

			m_program->bind_uniform({ m_sampler->value, m_input_image->value, m_input_image->image()->current_layout }, 0, 0);
			m_program->bind_uniform({ VK_NULL_HANDLE, m_output_image->value, m_output_image->image()->current_layout }, 0, 1);
		}

		void fsr3_pass::run(const vk::command_buffer& cmd, vk::viewable_image* src, vk::viewable_image* dst, const size2u& input_size, const size2u& output_size)
		{
			m_input_image = src->get_view(rsx::default_remap_vector.with_encoding(VK_REMAP_IDENTITY));
			m_output_image = dst->get_view(rsx::default_remap_vector.with_encoding(VK_REMAP_IDENTITY));
			m_input_size = input_size;
			m_output_size = output_size;

			configure(cmd);

			compute_task::run(cmd, (output_size.width + 7) / 8, (output_size.height + 7) / 8, 1);
		}

		fsr3_upscale_pass::fsr3_upscale_pass()
			: fsr3_pass(
				"#define SAMPLE_FSR3_UPSCALE 1\n"
				"#define SAMPLE_FSR3_TEMPORAL 0\n",
				80 // 5*VEC4
			)
		{}

		void fsr3_upscale_pass::configure(const vk::command_buffer& cmd)
		{
			auto src_image = m_input_image->image();

			// Configuration vectors for FSR3
			auto con0 = &m_constants_buf[0];
			auto con1 = &m_constants_buf[4];
			auto con2 = &m_constants_buf[8];
			auto con3 = &m_constants_buf[12];
			auto con4 = &m_constants_buf[16];

			Fsr3UpscaleCon(con0, con1, con2, con3, con4,
				static_cast<f32>(m_input_size.width), static_cast<f32>(m_input_size.height),     // Incoming viewport size to upscale (actual size)
				static_cast<f32>(src_image->width()), static_cast<f32>(src_image->height()),     // Size of the raw image to upscale (in case viewport does not cover it all)
				static_cast<f32>(m_output_size.width), static_cast<f32>(m_output_size.height));  // Size of output viewport (target size)

			load_program(cmd);
		}

		fsr3_temporal_pass::fsr3_temporal_pass()
			: fsr3_pass(
				"#define SAMPLE_FSR3_UPSCALE 0\n"
				"#define SAMPLE_FSR3_TEMPORAL 1\n",
				80 // 5*VEC4
			)
		{}

		void fsr3_temporal_pass::configure(const vk::command_buffer& cmd)
		{
			// Similar to upscale pass but with temporal configuration
			auto src_image = m_input_image->image();

			auto con0 = &m_constants_buf[0];
			auto con1 = &m_constants_buf[4];
			auto con2 = &m_constants_buf[8];
			auto con3 = &m_constants_buf[12];
			auto con4 = &m_constants_buf[16];

			Fsr3UpscaleCon(con0, con1, con2, con3, con4,
				static_cast<f32>(m_input_size.width), static_cast<f32>(m_input_size.height),     // Incoming viewport size to upscale (actual size)
				static_cast<f32>(src_image->width()), static_cast<f32>(src_image->height()),     // Size of the raw image to upscale (in case viewport does not cover it all)
				static_cast<f32>(m_output_size.width), static_cast<f32>(m_output_size.height));  // Size of output viewport (target size)

			load_program(cmd);
		}
	}

	// Main FSR3 upscale pass implementation
	fsr3_upscale_pass::~fsr3_upscale_pass()
	{
		dispose_images();
	}

	void fsr3_upscale_pass::dispose_images()
	{
		m_output_left.reset();
		m_output_right.reset();
		m_intermediate_data.reset();
	}

	void fsr3_upscale_pass::initialize_image(u32 output_w, u32 output_h, rsx::flags32_t mode)
	{
		dispose_images();

		const VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
		const VkImageUsageFlags usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		if (mode & vk::UPSCALE_LEFT_VIEW)
		{
			m_output_left = std::make_unique<vk::viewable_image>(
				*vk::get_current_renderer(), 
				vk::get_current_renderer()->get_memory_mapping().device_local, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				VK_IMAGE_TYPE_2D,
				format, 
				output_w, 
				output_h, 
				1, 1, 1, 
				VK_SAMPLE_COUNT_1_BIT, 
				VK_IMAGE_LAYOUT_GENERAL,
				VK_IMAGE_TILING_OPTIMAL, 
				usage, 
				0,
				vmm_allocation_pool::VMM_ALLOCATION_POOL_TEXTURE_CACHE,
				rsx::format_class::RSX_FORMAT_CLASS_COLOR
			);
		}

		if (mode & vk::UPSCALE_RIGHT_VIEW)
		{
			m_output_right = std::make_unique<vk::viewable_image>(
				*vk::get_current_renderer(), 
				vk::get_current_renderer()->get_memory_mapping().device_local, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				VK_IMAGE_TYPE_2D,
				format, 
				output_w, 
				output_h, 
				1, 1, 1, 
				VK_SAMPLE_COUNT_1_BIT, 
				VK_IMAGE_LAYOUT_GENERAL,
				VK_IMAGE_TILING_OPTIMAL, 
				usage, 
				0,
				vmm_allocation_pool::VMM_ALLOCATION_POOL_TEXTURE_CACHE,
				rsx::format_class::RSX_FORMAT_CLASS_COLOR
			);
		}
	}

	vk::viewable_image* fsr3_upscale_pass::scale_output(
		const vk::command_buffer& cmd,
		vk::viewable_image* src,
		VkImage present_surface,
		VkImageLayout present_surface_layout,
		const VkImageBlit& request,
		rsx::flags32_t mode)
	{
		const auto input_w = static_cast<u32>(request.srcOffsets[1].x - request.srcOffsets[0].x);
		const auto input_h = static_cast<u32>(request.srcOffsets[1].y - request.srcOffsets[0].y);
		const auto output_w = static_cast<u32>(request.dstOffsets[1].x - request.dstOffsets[0].x);
		const auto output_h = static_cast<u32>(request.dstOffsets[1].y - request.dstOffsets[0].y);

		// Initialize output image if needed
		if (!m_output_left && !m_output_right)
		{
			initialize_image(output_w, output_h, mode);
		}

		vk::viewable_image* output_image = nullptr;
		if (mode & vk::UPSCALE_LEFT_VIEW)
		{
			output_image = m_output_left.get();
		}
		else if (mode & vk::UPSCALE_RIGHT_VIEW)
		{
			output_image = m_output_right.get();
		}

		if (!output_image)
		{
			rsx_log.warning("FSR3 is enabled, but the system is out of memory. Will fall back to bilinear upscaling.");
			return src;
		}

		// Run FSR3 upscaling pass
		static FidelityFX::fsr3_upscale_pass upscale_pass;
		upscale_pass.run(cmd, src, output_image, { input_w, input_h }, { output_w, output_h });

		// Commit result if requested
		if (mode & vk::UPSCALE_AND_COMMIT)
		{
			VkImageBlit blit_request = request;
			blit_request.srcOffsets[0] = { 0, 0, 0 };
			blit_request.srcOffsets[1] = { static_cast<s32>(output_w), static_cast<s32>(output_h), 1 };

			VkImageSubresourceRange range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			vk::change_image_layout(cmd, output_image->value, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, range);
			vk::change_image_layout(cmd, present_surface, present_surface_layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, range);

			vkCmdBlitImage(cmd, output_image->value, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, present_surface, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit_request, VK_FILTER_LINEAR);

			vk::change_image_layout(cmd, output_image->value, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, range);
			vk::change_image_layout(cmd, present_surface, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, present_surface_layout, range);
		}

		return output_image;
	}
}