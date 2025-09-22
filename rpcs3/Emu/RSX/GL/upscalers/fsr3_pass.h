#pragma once

#include "../glutils/buffer_object.h"
#include "../glutils/state_tracker.hpp"

#include "../GLHelpers.h"
#include "../GLCompute.h"
#include "upscaling.h"

namespace gl
{
	namespace FidelityFX
	{
		class fsr3_pass : public compute_task
		{
		protected:
			gl::texture* m_input_image = nullptr;
			gl::texture* m_output_image = nullptr;
			size2u m_input_size;
			size2u m_output_size;

			std::vector<u32> m_constants_buf;
			gl::sampler_state m_sampler;
			gl::buffer m_ubo;

			void bind_resources() override;
			virtual void configure() = 0;

		public:
			fsr3_pass(const std::string& config_definitions, u32 push_constants_size);
			virtual ~fsr3_pass();

			void run(gl::command_context& cmd, gl::texture* src, gl::texture* dst, const size2u& input_size, const size2u& output_size);
		};

		class fsr3_upscale_pass : public fsr3_pass
		{
			void configure() override;

		public:
			fsr3_upscale_pass();
		};

		class fsr3_temporal_pass : public fsr3_pass
		{
			void configure() override;

		public:
			fsr3_temporal_pass();
		};
	}

	class fsr3_upscale_pass : public upscaler
	{
	public:
		fsr3_upscale_pass() = default;
		~fsr3_upscale_pass();

		gl::texture* scale_output(
			gl::command_context& cmd,               // State
			gl::texture* src,                       // Source input
			const areai& src_region,                // Scaling request information
			const areai& dst_region,                // Ditto
			gl::flags32_t mode                      // Mode
		) override;

	private:
		std::unique_ptr<gl::viewable_image> m_output_left;
		std::unique_ptr<gl::viewable_image> m_output_right;
		std::unique_ptr<gl::viewable_image> m_intermediate_data;

		gl::fbo m_flip_fbo;

		void dispose_images();
		void initialize_image(u32 output_w, u32 output_h, rsx::flags32_t mode);
	};
}