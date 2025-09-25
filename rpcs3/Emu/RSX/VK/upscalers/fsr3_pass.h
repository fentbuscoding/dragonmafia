#pragma once

#include "../vkutils/sampler.h"
#include "../VKCompute.h"

#include "upscaling.h"

namespace vk
{
	namespace FidelityFX
	{
		class fsr3_pass : public compute_task
		{
		protected:
			std::unique_ptr<vk::sampler> m_sampler;
			const vk::image_view* m_input_image = nullptr;
			const vk::image_view* m_output_image = nullptr;
			size2u m_input_size;
			size2u m_output_size;
			u32 m_constants_buf[20];

			std::vector<glsl::program_input> get_inputs() override;
			void bind_resources(const vk::command_buffer&) override;

			virtual void configure(const vk::command_buffer& cmd) = 0;

		public:
			fsr3_pass(const std::string& config_definitions, u32 push_constants_size_);
			void run(const vk::command_buffer& cmd, vk::viewable_image* src, vk::viewable_image* dst, const size2u& input_size, const size2u& output_size);
		};

		class fsr3_upscale_pass : public fsr3_pass
		{
			void configure(const vk::command_buffer& cmd) override;

		public:
			fsr3_upscale_pass();
		};

		class fsr3_temporal_pass : public fsr3_pass
		{
			void configure(const vk::command_buffer& cmd) override;

		public:
			fsr3_temporal_pass();
		};
	}

	class fsr3_upscale_pass : public upscaler
	{
		// Output images
		std::unique_ptr<vk::viewable_image> m_output_left;
		std::unique_ptr<vk::viewable_image> m_output_right;
		std::unique_ptr<vk::viewable_image> m_intermediate_data;
		
		// FSR3 temporal data
		std::unique_ptr<vk::viewable_image> m_motion_vectors;
		std::unique_ptr<vk::viewable_image> m_depth_buffer;
		std::unique_ptr<vk::viewable_image> m_reactive_mask;
		std::unique_ptr<vk::viewable_image> m_transparency_mask;
		std::unique_ptr<vk::viewable_image> m_previous_color;
		
		// FSR3 passes
		std::unique_ptr<FidelityFX::fsr3_upscale_pass> m_upscale_pass;
		std::unique_ptr<FidelityFX::fsr3_temporal_pass> m_temporal_pass;
		
		// Frame tracking
		u32 m_frame_count;
		struct { f32 x, y; } m_jitter_offset;

		void dispose_images();
		void initialize_image(u32 input_w, u32 input_h, u32 output_w, u32 output_h, rsx::flags32_t mode);
		void clear_auxiliary_buffers();
		void update_motion_vectors(const vk::command_buffer& cmd);
		void update_reactive_mask(const vk::command_buffer& cmd, vk::viewable_image* src);
		float get_quality_scale_factor() const;

	public:
		fsr3_upscale_pass();
		~fsr3_upscale_pass();

		vk::viewable_image* scale_output(
			const vk::command_buffer& cmd,          // CB
			vk::viewable_image* src,                // Source input
			VkImage present_surface,                // Present target. May be VK_NULL_HANDLE for some passes
			VkImageLayout present_surface_layout,   // Present surface layout, or VK_IMAGE_LAYOUT_UNDEFINED if no present target is provided
			const VkImageBlit& request,             // Scaling request information
			rsx::flags32_t mode                     // Mode
		) override;
	};
}