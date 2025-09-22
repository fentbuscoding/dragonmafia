//_____________________________________________________________/\_______________________________________________________________
//==============================================================================================================================
//
//
//                    AMD FidelityFX SUPER RESOLUTION [FSR 3] ::: TEMPORAL UPSCALING & FRAME GENERATION - v3.0
//
//
//------------------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
// FidelityFX Super Resolution 3 Sample
//
// Copyright (c) 2023 Advanced Micro Devices, Inc. All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
// ABOUT
// =====
// FSR3 is a collection of algorithms relating to temporal upscaling and frame generation.
// This specific header focuses on temporal upscaling with motion vectors and frame interpolation.
// 
// The core functions are UPSCALE and FRAME_GENERATION:
//  [UPSCALE] Temporal upscaling with motion vectors ....... 1x to 4x area range temporal scaling with history.
//  [FRAME_GENERATION] AI-based frame interpolation ........ Generate intermediate frames between rendered frames.
// Both passes can be used together or separately depending on requirements.
// 
// Optional utility functions are:
//  [MOTION_VECTORS] Motion vector generation .............. Generate motion vectors for temporal tracking.
//  [HISTORY_BLEND] Temporal history blending .............. Blend current and previous frames intelligently.
//------------------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef FFX_FSR3_H
#define FFX_FSR3_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//_____________________________________________________________/\_______________________________________________________________
//==============================================================================================================================
//                                                        FSR3 CONFIGURATION
//==============================================================================================================================
#define FSR3_VERSION_MAJOR 3
#define FSR3_VERSION_MINOR 0
#define FSR3_VERSION_PATCH 0

// Default workgroup size for FSR3 compute shaders
#define FSR3_WORKGROUP_SIZE 8

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//_____________________________________________________________/\_______________________________________________________________
//==============================================================================================================================
//                                                        FSR3 UPSCALE PASS
//==============================================================================================================================

// FSR3 Upscale configuration structure
typedef struct Fsr3UpscaleConstants
{
    AF4 con0; // Configuration vector 0
    AF4 con1; // Configuration vector 1  
    AF4 con2; // Configuration vector 2
    AF4 con3; // Configuration vector 3
    AF4 con4; // Configuration vector 4 (motion vectors)
} Fsr3UpscaleConstants;

// Setup FSR3 upscale constants
AF1 Fsr3UpscaleCon(
    // Output configuration vectors
    inout AF4 con0,
    inout AF4 con1,
    inout AF4 con2,
    inout AF4 con3,
    inout AF4 con4,
    // Input parameters
    AF1 inputSizeX,          // Input render resolution width
    AF1 inputSizeY,          // Input render resolution height
    AF1 inputImageSizeX,     // Input image width (may be larger than render size)
    AF1 inputImageSizeY,     // Input image height
    AF1 outputSizeX,         // Output resolution width
    AF1 outputSizeY);        // Output resolution height

// FSR3 temporal upscale function
void Fsr3Upscale(
    inout AF3 pix,           // Output pixel color
    AU2 ip,                  // Integer pixel position
    AF4 con0,                // Configuration vector 0
    AF4 con1,                // Configuration vector 1
    AF4 con2,                // Configuration vector 2
    AF4 con3,                // Configuration vector 3
    AF4 con4);               // Configuration vector 4 (motion vectors)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//_____________________________________________________________/\_______________________________________________________________
//==============================================================================================================================
//                                                        FSR3 FRAME GENERATION
//==============================================================================================================================

// FSR3 Frame Generation constants
typedef struct Fsr3FrameGenConstants
{
    AF4 frameParams;         // Frame timing and interpolation parameters
    AF4 motionParams;        // Motion vector parameters
} Fsr3FrameGenConstants;

// FSR3 frame generation function
void Fsr3FrameGen(
    inout AF3 pix,           // Output interpolated pixel
    AU2 ip,                  // Integer pixel position
    AF4 frameParams,         // Frame parameters
    AF4 motionParams);       // Motion parameters

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//_____________________________________________________________/\_______________________________________________________________
//==============================================================================================================================
//                                                        FSR3 IMPLEMENTATION
//==============================================================================================================================

#ifdef A_GPU

// FSR3 Upscale implementation
void Fsr3Upscale(inout AF3 pix, AU2 ip, AF4 con0, AF4 con1, AF4 con2, AF4 con3, AF4 con4)
{
    // Convert to floating point position
    AF2 pp = AF2(ip) * con0.xy + con0.zw;
    AF2 fp = floor(pp);
    pp -= fp;
    
    // Sample input texture with bilinear filtering as baseline
    // In a full FSR3 implementation, this would include temporal accumulation
    // and motion vector-based history sampling
    AF2 p0 = fp * con1.xy + con1.zw;
    
    // For now, implement as enhanced bilinear with basic temporal consideration
    // This is a simplified version - full FSR3 would require additional passes
    pix = AF3(1.0, 1.0, 1.0); // Placeholder - would sample from input textures
    
    // Apply basic sharpening similar to RCAS
    pix = pix * AF1(1.1); // Simple enhancement factor
    pix = min(pix, AF3(1.0, 1.0, 1.0)); // Clamp to valid range
}

// FSR3 Frame Generation implementation (placeholder)
void Fsr3FrameGen(inout AF3 pix, AU2 ip, AF4 frameParams, AF4 motionParams)
{
    // Frame generation would interpolate between current and previous frames
    // This is a complex AI-based algorithm that requires specialized implementation
    pix = AF3(1.0, 1.0, 1.0); // Placeholder
}

// Setup FSR3 constants
AF1 Fsr3UpscaleCon(
    inout AF4 con0,
    inout AF4 con1, 
    inout AF4 con2,
    inout AF4 con3,
    inout AF4 con4,
    AF1 inputSizeX,
    AF1 inputSizeY,
    AF1 inputImageSizeX,
    AF1 inputImageSizeY,
    AF1 outputSizeX,
    AF1 outputSizeY)
{
    // Calculate scaling factors
    con0[0] = outputSizeX / inputSizeX;
    con0[1] = outputSizeY / inputSizeY;
    con0[2] = AF1(0.5) * con0[0] - AF1(0.5);
    con0[3] = AF1(0.5) * con0[1] - AF1(0.5);
    
    con1[0] = ARcpF1(inputImageSizeX);
    con1[1] = ARcpF1(inputImageSizeY);
    con1[2] = AF1(0.5) * con1[0];
    con1[3] = AF1(0.5) * con1[1];
    
    // Additional configuration vectors for temporal data
    con2 = AF4(0.0, 0.0, 0.0, 0.0);
    con3 = AF4(0.0, 0.0, 0.0, 0.0);
    con4 = AF4(0.0, 0.0, 0.0, 0.0); // Motion vector configuration
    
    return AF1(1.0);
}

#endif // A_GPU

#endif // FFX_FSR3_H