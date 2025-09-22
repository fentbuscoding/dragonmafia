////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
// FSR 3.0 UBERSHADER
//------------------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

R"(
#version 450

// Include FidelityFX framework
%FFX_A_IMPORT%

// Include FSR3 implementation
%FFX_FSR_IMPORT%

// Configuration definitions
%FFX_DEFINITIONS%

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Input texture
layout(set = 0, binding = 0) uniform sampler2D InputTexture;

// Output image
layout(set = 0, binding = 1, rgba8) uniform writeonly image2D OutputTexture;

// Push constants for FSR3 configuration
layout(%push_block%) uniform PushConstants
{
    vec4 con0;
    vec4 con1;
    vec4 con2;
    vec4 con3;
    vec4 con4;
} pc;

// Main compute shader entry point
void main()
{
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    ivec2 outputSize = imageSize(OutputTexture);
    
    // Early exit if out of bounds
    if (id.x >= outputSize.x || id.y >= outputSize.y)
        return;
    
    vec3 outputColor = vec3(0.0);
    
#if defined(SAMPLE_FSR3_UPSCALE)
    // FSR3 upscaling pass
    Fsr3Upscale(outputColor, id, pc.con0, pc.con1, pc.con2, pc.con3, pc.con4);
#elif defined(SAMPLE_FSR3_TEMPORAL)
    // FSR3 temporal upscaling pass
    Fsr3TemporalUpscale(outputColor, id, pc.con0, pc.con1, pc.con2, pc.con3, pc.con4);
#else
    // Fallback to bilinear sampling
    vec2 uv = (vec2(id) + 0.5) / vec2(outputSize);
    outputColor = texture(InputTexture, uv).rgb;
#endif
    
    // Write output
    imageStore(OutputTexture, id, vec4(outputColor, 1.0));
}
)"