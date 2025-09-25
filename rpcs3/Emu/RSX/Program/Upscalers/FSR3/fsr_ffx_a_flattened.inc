////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
// FSR 3.0 FFX_A FLATTENED IMPLEMENTATION
//------------------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

R"--RPCS3--(
#define A_2PI 6.28318530718
#if defined(A_GLSL) && defined(A_GPU)
#ifndef A_SKIP_EXT
#ifdef A_HALF
#extension GL_EXT_shader_16bit_storage:require
#extension GL_EXT_shader_explicit_arithmetic_types:require
#endif
#ifdef A_LONG
#extension GL_ARB_gpu_shader_int64:require
#extension GL_NV_shader_atomic_int64:require
#endif
#ifdef A_WAVE
#extension GL_KHR_shader_subgroup_arithmetic:require
#extension GL_KHR_shader_subgroup_ballot:require
#extension GL_KHR_shader_subgroup_quad:require
#extension GL_KHR_shader_subgroup_shuffle:require
#endif
#endif
#define AP1 bool
#define AP2 bvec2
#define AP3 bvec3
#define AP4 bvec4
#define AF1 float
#define AF2 vec2
#define AF3 vec3
#define AF4 vec4
#define AU1 uint
#define AU2 uvec2
#define AU3 uvec3
#define AU4 uvec4
#define ASU1 int
#define ASU2 ivec2
#define ASU3 ivec3
#define ASU4 ivec4
#define AF1_AU1(x) uintBitsToFloat(AU1(x))
#define AF2_AU2(x) uintBitsToFloat(AU2(x))
#define AF3_AU3(x) uintBitsToFloat(AU3(x))
#define AF4_AU4(x) uintBitsToFloat(AU4(x))
#define AU1_AF1(x) floatBitsToUint(AF1(x))
#define AU2_AF2(x) floatBitsToUint(AF2(x))
#define AU3_AF3(x) floatBitsToUint(AF3(x))
#define AU4_AF4(x) floatBitsToUint(AF4(x))
AU1 AU1_AH1_AF1_x(AF1 a){return packHalf2x16(AF2(a,0.0));}
#define AU1_AH1_AF1(a) AU1_AH1_AF1_x(AF1(a))
#define AU1_AH2_AF2 packHalf2x16
#define AU1_AW2Unorm_AF2 packUnorm2x16
#define AU1_AB4Unorm_AF4 packUnorm4x8
#define AF2_AH2_AU1 unpackHalf2x16
#define AF2_AW2Unorm_AU1 unpackUnorm2x16
#define AF4_AB4Unorm_AU1 unpackUnorm4x8
AF1 AF1_x(AF1 a){return AF1(a);}
AF2 AF2_x(AF1 a){return AF2(a,a);}
AF3 AF3_x(AF1 a){return AF3(a,a,a);}
AF4 AF4_x(AF1 a){return AF4(a,a,a,a);}
#define AF1_(a) AF1_x(AF1(a))
#define AF2_(a) AF2_x(AF1(a))
#define AF3_(a) AF3_x(AF1(a))
#define AF4_(a) AF4_x(AF1(a))
AU1 AU1_x(AU1 a){return AU1(a);}
AU2 AU2_x(AU1 a){return AU2(a,a);}
AU3 AU3_x(AU1 a){return AU3(a,a,a);}
AU4 AU4_x(AU1 a){return AU4(a,a,a,a);}
#define AU1_(a) AU1_x(AU1(a))
#define AU2_(a) AU2_x(AU1(a))
#define AU3_(a) AU3_x(AU1(a))
#define AU4_(a) AU4_x(AU1(a))
AU1 AAbsSU1(AU1 a){return AU1(abs(ASU1(a)));}
AU2 AAbsSU2(AU2 a){return AU2(abs(ASU2(a)));}
AU3 AAbsSU3(AU3 a){return AU3(abs(ASU3(a)));}
AU4 AAbsSU4(AU4 a){return AU4(abs(ASU4(a)));}
AU1 ABfe(AU1 src,AU1 off,AU1 bits){return bitfieldExtract(src,ASU1(off),ASU1(bits));}
AU1 ABfi(AU1 src,AU1 ins,AU1 mask){return (ins&mask)|(src&(~mask));}
AU1 ABfiM(AU1 src,AU1 ins,AU1 bits){return bitfieldInsert(src,ins,0,ASU1(bits));}
AF1 AClampF1(AF1 x,AF1 n,AF1 m){return clamp(x,n,m);}
AF2 AClampF2(AF2 x,AF2 n,AF2 m){return clamp(x,n,m);}
AF3 AClampF3(AF3 x,AF3 n,AF3 m){return clamp(x,n,m);}
AF4 AClampF4(AF4 x,AF4 n,AF4 m){return clamp(x,n,m);}
AF1 AFractF1(AF1 x){return fract(x);}
AF2 AFractF2(AF2 x){return fract(x);}
AF3 AFractF3(AF3 x){return fract(x);}
AF4 AFractF4(AF4 x){return fract(x);}
AF1 ALerpF1(AF1 x,AF1 y,AF1 a){return mix(x,y,a);}
AF2 ALerpF2(AF2 x,AF2 y,AF2 a){return mix(x,y,a);}
AF3 ALerpF3(AF3 x,AF3 y,AF3 a){return mix(x,y,a);}
AF4 ALerpF4(AF4 x,AF4 y,AF4 a){return mix(x,y,a);}
AF1 AMax3F1(AF1 x,AF1 y,AF1 z){return max(x,max(y,z));}
AF2 AMax3F2(AF2 x,AF2 y,AF2 z){return max(x,max(y,z));}
AF3 AMax3F3(AF3 x,AF3 y,AF3 z){return max(x,max(y,z));}
AF4 AMax3F4(AF4 x,AF4 y,AF4 z){return max(x,max(y,z));}
AU1 AMax3SU1(AU1 x,AU1 y,AU1 z){return AU1(max(ASU1(x),max(ASU1(y),ASU1(z))));}
AU2 AMax3SU2(AU2 x,AU2 y,AU2 z){return AU2(max(ASU2(x),max(ASU2(y),ASU2(z))));}
AU3 AMax3SU3(AU3 x,AU3 y,AU3 z){return AU3(max(ASU3(x),max(ASU3(y),ASU3(z))));}
AU4 AMax3SU4(AU4 x,AU4 y,AU4 z){return AU4(max(ASU4(x),max(ASU4(y),ASU4(z))));}
AU1 AMax3U1(AU1 x,AU1 y,AU1 z){return max(x,max(y,z));}
AU2 AMax3U2(AU2 x,AU2 y,AU2 z){return max(x,max(y,z));}
AU3 AMax3U3(AU3 x,AU3 y,AU3 z){return max(x,max(y,z));}
AU4 AMax3U4(AU4 x,AU4 y,AU4 z){return max(x,max(y,z));}
AU1 AMaxSU1(AU1 a,AU1 b){return AU1(max(ASU1(a),ASU1(b)));}
AU2 AMaxSU2(AU2 a,AU2 b){return AU2(max(ASU2(a),ASU2(b)));}
AU3 AMaxSU3(AU3 a,AU3 b){return AU3(max(ASU3(a),ASU3(b)));}
AU4 AMaxSU4(AU4 a,AU4 b){return AU4(max(ASU4(a),ASU4(b)));}
AF1 AMed3F1(AF1 x,AF1 y,AF1 z){return max(min(x,y),min(max(x,y),z));}
AF2 AMed3F2(AF2 x,AF2 y,AF2 z){return max(min(x,y),min(max(x,y),z));}
AF3 AMed3F3(AF3 x,AF3 y,AF3 z){return max(min(x,y),min(max(x,y),z));}
AF4 AMed3F4(AF4 x,AF4 y,AF4 z){return max(min(x,y),min(max(x,y),z));}
AF1 AMin3F1(AF1 x,AF1 y,AF1 z){return min(x,min(y,z));}
AF2 AMin3F2(AF2 x,AF2 y,AF2 z){return min(x,min(y,z));}
AF3 AMin3F3(AF3 x,AF3 y,AF3 z){return min(x,min(y,z));}
AF4 AMin3F4(AF4 x,AF4 y,AF4 z){return min(x,min(y,z));}
AU1 AMin3SU1(AU1 x,AU1 y,AU1 z){return AU1(min(ASU1(x),min(ASU1(y),ASU1(z))));}
AU2 AMin3SU2(AU2 x,AU2 y,AU2 z){return AU2(min(ASU2(x),min(ASU2(y),ASU2(z))));}
AU3 AMin3SU3(AU3 x,AU3 y,AU3 z){return AU3(min(ASU3(x),min(ASU3(y),ASU3(z))));}
AU4 AMin3SU4(AU4 x,AU4 y,AU4 z){return AU4(min(ASU4(x),min(ASU4(y),ASU4(z))));}
AU1 AMin3U1(AU1 x,AU1 y,AU1 z){return min(x,min(y,z));}
AU2 AMin3U2(AU2 x,AU2 y,AU2 z){return min(x,min(y,z));}
AU3 AMin3U3(AU3 x,AU3 y,AU3 z){return min(x,min(y,z));}
AU4 AMin3U4(AU4 x,AU4 y,AU4 z){return min(x,min(y,z));}
AU1 AMinSU1(AU1 a,AU1 b){return AU1(min(ASU1(a),ASU1(b)));}
AU2 AMinSU2(AU2 a,AU2 b){return AU2(min(ASU2(a),ASU2(b)));}
AU3 AMinSU3(AU3 a,AU3 b){return AU3(min(ASU3(a),ASU3(b)));}
AU4 AMinSU4(AU4 a,AU4 b){return AU4(min(ASU4(a),ASU4(b)));}
AF1 ANCosF1(AF1 x){return cos(x*AF1_(A_2PI));}
AF2 ANCosF2(AF2 x){return cos(x*AF2_(A_2PI));}
AF3 ANCosF3(AF3 x){return cos(x*AF3_(A_2PI));}
AF4 ANCosF4(AF4 x){return cos(x*AF4_(A_2PI));}
AF1 ANSinF1(AF1 x){return sin(x*AF1_(A_2PI));}
AF2 ANSinF2(AF2 x){return sin(x*AF2_(A_2PI));}
AF3 ANSinF3(AF3 x){return sin(x*AF3_(A_2PI));}
AF4 ANSinF4(AF4 x){return sin(x*AF4_(A_2PI));}
AF1 ARcpF1(AF1 x){return AF1_(1.0)/x;}
AF2 ARcpF2(AF2 x){return AF2_(1.0)/x;}
AF3 ARcpF3(AF3 x){return AF3_(1.0)/x;}
AF4 ARcpF4(AF4 x){return AF4_(1.0)/x;}
AF1 ARsqF1(AF1 x){return AF1_(1.0)/sqrt(x);}
AF2 ARsqF2(AF2 x){return AF2_(1.0)/sqrt(x);}
AF3 ARsqF3(AF3 x){return AF3_(1.0)/sqrt(x);}
AF4 ARsqF4(AF4 x){return AF4_(1.0)/sqrt(x);}
AF1 ASatF1(AF1 x){return clamp(x,AF1_(0.0),AF1_(1.0));}
AF2 ASatF2(AF2 x){return clamp(x,AF2_(0.0),AF2_(1.0));}
AF3 ASatF3(AF3 x){return clamp(x,AF3_(0.0),AF3_(1.0));}
AF4 ASatF4(AF4 x){return clamp(x,AF4_(0.0),AF4_(1.0));}
AU1 AShrSU1(AU1 a,AU1 b){return AU1(ASU1(a)>>ASU1(b));}
AU2 AShrSU2(AU2 a,AU2 b){return AU2(ASU2(a)>>ASU2(b));}
AU3 AShrSU3(AU3 a,AU3 b){return AU3(ASU3(a)>>ASU3(b));}
AU4 AShrSU4(AU4 a,AU4 b){return AU4(ASU4(a)>>ASU4(b));}

// FSR3-specific extensions for temporal processing
AF1 ATemporal3F1(AF1 current, AF1 previous, AF1 weight){return mix(previous, current, weight);}
AF2 ATemporal3F2(AF2 current, AF2 previous, AF2 weight){return mix(previous, current, weight);}
AF3 ATemporal3F3(AF3 current, AF3 previous, AF3 weight){return mix(previous, current, weight);}
AF4 ATemporal3F4(AF4 current, AF4 previous, AF4 weight){return mix(previous, current, weight);}

// Motion vector utilities for FSR3
AF2 AMotionVector3(AF2 current_pos, AF2 previous_pos){return current_pos - previous_pos;}
AF1 AMotionMagnitude3(AF2 motion){return length(motion);}
AF1 AMotionWeight3(AF1 magnitude, AF1 threshold){return clamp(1.0 - magnitude / threshold, 0.0, 1.0);}

// Depth comparison utilities for FSR3
AF1 ADepthWeight3(AF1 current_depth, AF1 previous_depth, AF1 threshold)
{
    AF1 depth_diff = abs(current_depth - previous_depth);
    return clamp(1.0 - depth_diff / threshold, 0.0, 1.0);
}

// Reactive mask utilities for FSR3
AF1 AReactiveMask3(AF1 luminance_diff, AF1 threshold)
{
    return clamp(luminance_diff / threshold, 0.0, 1.0);
}

#ifdef A_HALF
#define AH1 float16_t
#define AH2 f16vec2
#define AH3 f16vec3
#define AH4 f16vec4
#define AW1 uint16_t
#define AW2 u16vec2
#define AW3 u16vec3
#define AW4 u16vec4
#define ASW1 int16_t
#define ASW2 i16vec2
#define ASW3 i16vec3
#define ASW4 i16vec4
#define AH2_AU1(x) unpackFloat2x16(AU1(x))
AH4 AH4_AU2_x(AU2 x){return AH4(unpackFloat2x16(x.x),unpackFloat2x16(x.y));}
#define AH4_AU2(x) AH4_AU2_x(AU2(x))
#define AW2_AU1(x) unpackUint2x16(AU1(x))
#define AW4_AU2(x) unpackUint4x16(pack64(AU2(x)))
#define AU1_AH2(x) packFloat2x16(AH2(x))
AU2 AU2_AH4_x(AH4 x){return AU2(packFloat2x16(x.xy),packFloat2x16(x.zw));}
#define AU2_AH4(x) AU2_AH4_x(AH4(x))
#define AU1_AW2(x) packUint2x16(AW2(x))
#define AU2_AW4(x) unpack32(packUint4x16(AW4(x)))
#define AW1_AH1(x) halfBitsToUint16(AH1(x))
#define AW2_AH2(x) halfBitsToUint16(AH2(x))
#define AW3_AH3(x) halfBitsToUint16(AH3(x))
#define AW4_AH4(x) halfBitsToUint16(AH4(x))
#define AH1_AW1(x) uint16BitsToHalf(AW1(x))
#define AH2_AW2(x) uint16BitsToHalf(AW2(x))
#define AH3_AW3(x) uint16BitsToHalf(AW3(x))
#define AH4_AW4(x) uint16BitsToHalf(AW4(x))
AH1 AH1_x(AH1 a){return AH1(a);}
AH2 AH2_x(AH1 a){return AH2(a,a);}
AH3 AH3_x(AH1 a){return AH3(a,a,a);}
AH4 AH4_x(AH1 a){return AH4(a,a,a,a);}
#define AH1_(a) AH1_x(AH1(a))
#define AH2_(a) AH2_x(AH1(a))
#define AH3_(a) AH3_x(AH1(a))
#define AH4_(a) AH4_x(AH1(a))
AW1 AW1_x(AW1 a){return AW1(a);}
AW2 AW2_x(AW1 a){return AW2(a,a);}
AW3 AW3_x(AW1 a){return AW3(a,a,a);}
AW4 AW4_x(AW1 a){return AW4(a,a,a,a);}
#define AW1_(a) AW1_x(AW1(a))
#define AW2_(a) AW2_x(AW1(a))
#define AW3_(a) AW3_x(AW1(a))
#define AW4_(a) AW4_x(AW1(a))
AW1 AAbsSW1(AW1 a){return AW1(abs(ASW1(a)));}
AW2 AAbsSW2(AW2 a){return AW2(abs(ASW2(a)));}
AW3 AAbsSW3(AW3 a){return AW3(abs(ASW3(a)));}
AW4 AAbsSW4(AW4 a){return AW4(abs(ASW4(a)));}
AH1 AClampH1(AH1 x,AH1 n,AH1 m){return clamp(x,n,m);}
AH2 AClampH2(AH2 x,AH2 n,AH2 m){return clamp(x,n,m);}
AH3 AClampH3(AH3 x,AH3 n,AH3 m){return clamp(x,n,m);}
AH4 AClampH4(AH4 x,AH4 n,AH4 m){return clamp(x,n,m);}
AH1 AFractH1(AH1 x){return fract(x);}
AH2 AFractH2(AH2 x){return fract(x);}
AH3 AFractH3(AH3 x){return fract(x);}
AH4 AFractH4(AH4 x){return fract(x);}
AH1 ALerpH1(AH1 x,AH1 y,AH1 a){return mix(x,y,a);}
AH2 ALerpH2(AH2 x,AH2 y,AH2 a){return mix(x,y,a);}
AH3 ALerpH3(AH3 x,AH3 y,AH3 a){return mix(x,y,a);}
AH4 ALerpH4(AH4 x,AH4 y,AH4 a){return mix(x,y,a);}
AH1 AMax3H1(AH1 x,AH1 y,AH1 z){return max(x,max(y,z));}
AH2 AMax3H2(AH2 x,AH2 y,AH2 z){return max(x,max(y,z));}
AH3 AMax3H3(AH3 x,AH3 y,AH3 z){return max(x,max(y,z));}
AH4 AMax3H4(AH4 x,AH4 y,AH4 z){return max(x,max(y,z));}
AW1 AMaxSW1(AW1 a,AW1 b){return AW1(max(ASU1(a),ASU1(b)));}
AW2 AMaxSW2(AW2 a,AW2 b){return AW2(max(ASU2(a),ASU2(b)));}
AW3 AMaxSW3(AW3 a,AW3 b){return AW3(max(ASU3(a),ASU3(b)));}
AW4 AMaxSW4(AW4 a,AW4 b){return AW4(max(ASU4(a),ASU4(b)));}
AH1 AMin3H1(AH1 x,AH1 y,AH1 z){return min(x,min(y,z));}
AH2 AMin3H2(AH2 x,AH2 y,AH2 z){return min(x,min(y,z));}
AH3 AMin3H3(AH3 x,AH3 y,AH3 z){return min(x,min(y,z));}
AH4 AMin3H4(AH4 x,AH4 y,AH4 z){return min(x,min(y,z));}
AW1 AMinSW1(AW1 a,AW1 b){return AW1(min(ASU1(a),ASU1(b)));}
AW2 AMinSW2(AW2 a,AW2 b){return AW2(min(ASU2(a),ASU2(b)));}
AW3 AMinSW3(AW3 a,AW3 b){return AW3(min(ASU3(a),ASU3(b)));}
AW4 AMinSW4(AW4 a,AW4 b){return AW4(min(ASU4(a),ASU4(b)));}
AH1 ARcpH1(AH1 x){return AH1_(1.0)/x;}
AH2 ARcpH2(AH2 x){return AH2_(1.0)/x;}
AH3 ARcpH3(AH3 x){return AH3_(1.0)/x;}
AH4 ARcpH4(AH4 x){return AH4_(1.0)/x;}
AH1 ARsqH1(AH1 x){return AH1_(1.0)/sqrt(x);}
AH2 ARsqH2(AH2 x){return AH2_(1.0)/sqrt(x);}
AH3 ARsqH3(AH3 x){return AH3_(1.0)/sqrt(x);}
AH4 ARsqH4(AH4 x){return AH4_(1.0)/sqrt(x);}
AH1 ASatH1(AH1 x){return clamp(x,AH1_(0.0),AH1_(1.0));}
AH2 ASatH2(AH2 x){return clamp(x,AH2_(0.0),AH2_(1.0));}
AH3 ASatH3(AH3 x){return clamp(x,AH3_(0.0),AH3_(1.0));}
AH4 ASatH4(AH4 x){return clamp(x,AH4_(0.0),AH4_(1.0));}
AW1 AShrSW1(AW1 a,AW1 b){return AW1(ASW1(a)>>ASW1(b));}
AW2 AShrSW2(AW2 a,AW2 b){return AW2(ASW2(a)>>ASW2(b));}
AW3 AShrSW3(AW3 a,AW3 b){return AW3(ASW3(a)>>ASW3(b));}
AW4 AShrSW4(AW4 a,AW4 b){return AW4(ASW4(a)>>ASW4(b));}

// FSR3-specific half precision temporal functions
AH1 ATemporal3H1(AH1 current, AH1 previous, AH1 weight){return mix(previous, current, weight);}
AH2 ATemporal3H2(AH2 current, AH2 previous, AH2 weight){return mix(previous, current, weight);}
AH3 ATemporal3H3(AH3 current, AH3 previous, AH3 weight){return mix(previous, current, weight);}
AH4 ATemporal3H4(AH4 current, AH4 previous, AH4 weight){return mix(previous, current, weight);}
#endif

#ifdef A_WAVE
AF1 AWaveXorF1(AF1 v,AU1 x){return subgroupShuffleXor(v,x);}
AF2 AWaveXorF2(AF2 v,AU1 x){return subgroupShuffleXor(v,x);}
AF3 AWaveXorF3(AF3 v,AU1 x){return subgroupShuffleXor(v,x);}
AF4 AWaveXorF4(AF4 v,AU1 x){return subgroupShuffleXor(v,x);}
AU1 AWaveXorU1(AU1 v,AU1 x){return subgroupShuffleXor(v,x);}
AU2 AWaveXorU2(AU2 v,AU1 x){return subgroupShuffleXor(v,x);}
AU3 AWaveXorU3(AU3 v,AU1 x){return subgroupShuffleXor(v,x);}
AU4 AWaveXorU4(AU4 v,AU1 x){return subgroupShuffleXor(v,x);}
#ifdef A_HALF
AH2 AWaveXorH2(AH2 v,AU1 x){return AH2_AU1(subgroupShuffleXor(AU1_AH2(v),x));}
AH4 AWaveXorH4(AH4 v,AU1 x){return AH4_AU2(subgroupShuffleXor(AU2_AH4(v),x));}
AW2 AWaveXorW2(AW2 v,AU1 x){return AW2_AU1(subgroupShuffleXor(AU1_AW2(v),x));}
AW4 AWaveXorW4(AW4 v,AU1 x){return AW4_AU2(subgroupShuffleXor(AU2_AW4(v),x));}
#endif
#endif
#endif
)--RPCS3--"