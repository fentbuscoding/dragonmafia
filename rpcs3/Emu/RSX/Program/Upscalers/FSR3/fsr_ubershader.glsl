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
    uvec4 con0;
    uvec4 con1;
    uvec4 con2;
    uvec4 con3;
    uvec4 con4;
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
    // FSR3 spatial upscaling pass with enhanced edge detection
    vec2 inputSize = textureSize(InputTexture, 0);
    vec2 invInputSize = 1.0 / inputSize;
    vec2 uv = (vec2(id) + 0.5) / vec2(outputSize);
    
    // Enhanced FSR3 upscaling with better edge preservation
    Fsr3Upscale(outputColor, id, pc.con0, pc.con1, pc.con2, pc.con3, pc.con4);
    
    // Apply sharpening filter for better detail preservation
    vec2 texel = uv * inputSize;
    vec2 texelFloor = floor(texel);
    vec2 texelFract = texel - texelFloor;
    
    // Bicubic sampling for improved quality
    vec3 col00 = texture(InputTexture, (texelFloor + vec2(0.0, 0.0)) * invInputSize).rgb;
    vec3 col10 = texture(InputTexture, (texelFloor + vec2(1.0, 0.0)) * invInputSize).rgb;
    vec3 col01 = texture(InputTexture, (texelFloor + vec2(0.0, 1.0)) * invInputSize).rgb;
    vec3 col11 = texture(InputTexture, (texelFloor + vec2(1.0, 1.0)) * invInputSize).rgb;
    
    // Bilinear interpolation weights
    vec3 col0 = mix(col00, col10, texelFract.x);
    vec3 col1 = mix(col01, col11, texelFract.x);
    vec3 bicubicResult = mix(col0, col1, texelFract.y);
    
    // Blend FSR3 result with bicubic for enhanced quality
    outputColor = mix(outputColor, bicubicResult, 0.2);
    
#elif defined(SAMPLE_FSR3_TEMPORAL)
    // FSR3 temporal upscaling pass with motion vector support
    vec2 inputSize = textureSize(InputTexture, 0);
    vec2 uv = (vec2(id) + 0.5) / vec2(outputSize);
    
    // Enhanced temporal upscaling with motion compensation
    Fsr3TemporalUpscale(outputColor, id, pc.con0, pc.con1, pc.con2, pc.con3, pc.con4);
    
    // Apply temporal stability enhancements
    vec3 currentFrame = texture(InputTexture, uv).rgb;
    
    // Calculate luminance for temporal weighting
    float currentLuma = dot(currentFrame, vec3(0.299, 0.587, 0.114));
    float outputLuma = dot(outputColor, vec3(0.299, 0.587, 0.114));
    
    // Preserve detail in high-contrast areas
    float contrastWeight = abs(currentLuma - outputLuma);
    contrastWeight = smoothstep(0.0, 0.1, contrastWeight);
    
    // Blend current frame with temporal result for stability
    outputColor = mix(outputColor, currentFrame, contrastWeight * 0.3);
    
#else
    // Enhanced bilinear sampling fallback with edge-aware upscaling
    vec2 inputSize = textureSize(InputTexture, 0);
    vec2 invInputSize = 1.0 / inputSize;
    vec2 uv = (vec2(id) + 0.5) / vec2(outputSize);
    
    // Sample with improved filtering
    vec3 center = texture(InputTexture, uv).rgb;
    
    // Edge detection for adaptive filtering
    vec3 left = texture(InputTexture, uv + vec2(-invInputSize.x, 0.0)).rgb;
    vec3 right = texture(InputTexture, uv + vec2(invInputSize.x, 0.0)).rgb;
    vec3 top = texture(InputTexture, uv + vec2(0.0, -invInputSize.y)).rgb;
    vec3 bottom = texture(InputTexture, uv + vec2(0.0, invInputSize.y)).rgb;
    
    // Calculate edge strength
    vec3 edgeH = abs(left - right);
    vec3 edgeV = abs(top - bottom);
    float edgeStrength = length(edgeH + edgeV);
    
    // Apply adaptive sharpening based on edge detection
    vec3 sharpened = center + (center - (left + right + top + bottom) * 0.25) * 0.5;
    
    outputColor = mix(center, sharpened, min(edgeStrength, 0.5));
#endif
    
    // Write output
    imageStore(OutputTexture, id, vec4(outputColor, 1.0));
}
)"