#include "../fsr3_pass.h"
#include "util/logs.hpp"

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

namespace gl
{
	namespace FidelityFX
	{
		fsr3_pass::fsr3_pass(const std::string& config_definitions, u32 push_constants_size)
		{
			// Load shader source
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
				{ "layout(set=0,", "layout(" },
				{ "%push_block%", fmt::format("binding=%d, std140", GL_COMPUTE_BUFFER_SLOT(0)) }
			};

			m_src = shader_core;
			m_src = fmt::replace_all(m_src, replacement_table);

			// Fill with 0 to avoid sending incomplete/unused variables to the GPU
			m_constants_buf.resize(utils::rounded_div(push_constants_size, 4), 0);

			create();

			m_sampler.create();
			m_sampler.apply_defaults(GL_LINEAR);
		}

		fsr3_pass::~fsr3_pass()
		{
			m_ubo.remove();
			m_sampler.remove();
		}

		void fsr3_pass::bind_resources()
		{
			m_sampler.bind(0);
		}

		void fsr3_pass::run(gl::command_context& cmd, gl::texture* src, gl::texture* dst, const size2u& input_size, const size2u& output_size)
		{
			m_input_image = src;
			m_output_image = dst;
			m_input_size = input_size;
			m_output_size = output_size;

			configure();

			// Bind input texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, src->id());

			// Bind output image
			glBindImageTexture(1, dst->id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

			// Upload constants
			m_ubo.reserve_storage_on_heap(m_constants_buf.size() * sizeof(u32));
			auto mapped = m_ubo.map(gl::buffer::access::write);
			std::memcpy(mapped, m_constants_buf.data(), m_constants_buf.size() * sizeof(u32));
			m_ubo.unmap();

			bind_resources();

			// Dispatch compute shader
			const auto wg_size = 8;
			const auto invocations_x = utils::aligned_div(output_size.width, wg_size);
			const auto invocations_y = utils::aligned_div(output_size.height, wg_size);

			ensure(invocations_x == (output_size.width + (wg_size - 1)) / wg_size);
			ensure(invocations_y == (output_size.height + (wg_size - 1)) / wg_size);
			compute_task::run(cmd, invocations_x, invocations_y);
		}

		fsr3_upscale_pass::fsr3_upscale_pass()
			: fsr3_pass(
				"#define SAMPLE_FSR3_UPSCALE 1\n"
				"#define SAMPLE_FSR3_TEMPORAL 0\n",
				80 // 5*VEC4
			)
		{}

		void fsr3_upscale_pass::configure()
		{
			// Configure FSR3 upscaling constants based on actual input/output sizes
			const f32 scale_x = static_cast<f32>(m_output_size.width) / static_cast<f32>(m_input_size.width);
			const f32 scale_y = static_cast<f32>(m_output_size.height) / static_cast<f32>(m_input_size.height);
			
			// con0: scale factors
			m_constants_buf[0] = std::bit_cast<u32>(scale_x);
			m_constants_buf[1] = std::bit_cast<u32>(scale_y);
			m_constants_buf[2] = std::bit_cast<u32>(0.5f * scale_x - 0.5f);
			m_constants_buf[3] = std::bit_cast<u32>(0.5f * scale_y - 0.5f);

			// con1: reciprocal dimensions
			const f32 rcpw = 1.0f / static_cast<f32>(m_input_size.width);
			const f32 rcph = 1.0f / static_cast<f32>(m_input_size.height);
			m_constants_buf[4] = std::bit_cast<u32>(rcpw);
			m_constants_buf[5] = std::bit_cast<u32>(rcph);
			m_constants_buf[6] = std::bit_cast<u32>(0.5f * rcpw);
			m_constants_buf[7] = std::bit_cast<u32>(0.5f * rcph);

			// con2-con4: additional configuration for temporal data
			for (int i = 8; i < 20; ++i)
			{
				m_constants_buf[i] = 0;
			}
		}

		fsr3_temporal_pass::fsr3_temporal_pass()
			: fsr3_pass(
				"#define SAMPLE_FSR3_UPSCALE 0\n"
				"#define SAMPLE_FSR3_TEMPORAL 1\n",
				80 // 5*VEC4
			)
		{}

		void fsr3_temporal_pass::configure()
		{
			// Configure FSR3 temporal constants
			const f32 scale_x = static_cast<f32>(m_output_size.width) / static_cast<f32>(m_input_size.width);
			const f32 scale_y = static_cast<f32>(m_output_size.height) / static_cast<f32>(m_input_size.height);
			
			// con0: scale factors  
			m_constants_buf[0] = std::bit_cast<u32>(scale_x);
			m_constants_buf[1] = std::bit_cast<u32>(scale_y);
			m_constants_buf[2] = std::bit_cast<u32>(0.5f * scale_x - 0.5f);
			m_constants_buf[3] = std::bit_cast<u32>(0.5f * scale_y - 0.5f);

			// con1: reciprocal dimensions
			const f32 rcpw = 1.0f / static_cast<f32>(m_input_size.width);
			const f32 rcph = 1.0f / static_cast<f32>(m_input_size.height);
			m_constants_buf[4] = std::bit_cast<u32>(rcpw);
			m_constants_buf[5] = std::bit_cast<u32>(rcph);
			m_constants_buf[6] = std::bit_cast<u32>(0.5f * rcpw);
			m_constants_buf[7] = std::bit_cast<u32>(0.5f * rcph);

			// con2-con4: temporal-specific configuration
			for (int i = 8; i < 20; ++i)
			{
				m_constants_buf[i] = 0;
			}
		}
	}

	// Main GL FSR3 upscaler implementation
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

		const GLenum ifmt = GL_RGBA8;
		const GLenum fmt = GL_RGBA;
		const GLenum type = GL_UNSIGNED_BYTE;

		if (mode & gl::UPSCALE_LEFT_VIEW)
		{
			m_output_left = std::make_unique<gl::viewable_image>(output_w, output_h, GL_TEXTURE_2D, ifmt, fmt, type);
		}

		if (mode & gl::UPSCALE_RIGHT_VIEW) 
		{
			m_output_right = std::make_unique<gl::viewable_image>(output_w, output_h, GL_TEXTURE_2D, ifmt, fmt, type);
		}
	}

	gl::texture* fsr3_upscale_pass::scale_output(
		gl::command_context& cmd,
		gl::texture* src,
		const areai& src_region,
		const areai& dst_region,
		gl::flags32_t mode)
	{
		const auto input_w = src_region.width();
		const auto input_h = src_region.height();
		const auto output_w = dst_region.width();
		const auto output_h = dst_region.height();

		// Initialize output image if needed
		if (!m_output_left && !m_output_right)
		{
			initialize_image(output_w, output_h, mode);
		}

		gl::viewable_image* output_image = nullptr;
		if (mode & gl::UPSCALE_LEFT_VIEW)
		{
			output_image = m_output_left.get();
		}
		else if (mode & gl::UPSCALE_RIGHT_VIEW)
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

		return output_image;
	}
}