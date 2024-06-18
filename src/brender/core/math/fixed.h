#ifndef REC2_FIXED_H
#define REC2_FIXED_H

#include "c2_hooks.h"

#include "brender/br_types.h"

//#define BR_ONE_LS ((br_fixed_ls)0x00010000)
#define BR_ONE_LU ((br_fixed_lu)0x00010000)
#define BR_ONE_LSF ((br_fixed_ls)0x00008000)
#define BR_ONE_LUF ((br_fixed_lu)0x00010000)
#define BR_ONE_SS ((br_fixed_ss)0x0100)
#define BR_ONE_SU ((br_fixed_ss)0x0100)
#define BR_ONE_SSF ((br_fixed_ss)0x0080)
#define BR_ONE_SUF ((br_fixed_ss)0x0100)
#define BrIntToFixed(i) ((i)<<16)
#define BrFloatToFixed(f) ((br_fixed_ls)((f)*65536.f))
#define BrFixedToInt(i) ((i)>>16)
#define BrFixedToFloat(i) ((float)((i)*(1.f/65536.f)))

#define BrFloatToFixedFraction(f) ((br_fixed_lsf)((f)*32768.f))
#define BrFixedFractionToFloat(f) ((float)(((br_fixed_lsf)(f))*(1.f/32768.f)))

#define BrFloatToFixedUFraction(f) ((br_fixed_luf)((f)*65536.f))
#define BrFixedUFractionToFloat(f) ((float)(((br_fixed_luf)(f))*(1.f/65536.f)))

#define BrFixedNeg(f) ((f) ^ 0x80000000)

#if 1
#define BrFloatToScalar(f) f
#define BrScalarToFlat(f) f
//#define BrFixedToScalar BrFixedToFloat
//#define BrScalarToFixed BrFloatToFixed

#define BrScalarToFraction(f) f
#define BrFractionToScalar(f) f
#endif

br_fixed_ls C2_HOOK_CDECL BrFixedAbs(br_fixed_ls a);

br_fixed_ls C2_HOOK_FASTCALL BrFixedMul(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedMac2(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d);

br_fixed_ls C2_HOOK_CDECL BrFixedMac3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f);

br_fixed_ls C2_HOOK_CDECL BrFixedMac4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f, br_fixed_ls g, br_fixed_ls h);

br_fixed_ls C2_HOOK_CDECL BrFixedLength2(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedLength3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c);

br_fixed_ls C2_HOOK_CDECL BrFixedLength4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d);

br_fixed_ls C2_HOOK_CDECL BrFixedRLength2(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedRLength3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c);

br_fixed_ls C2_HOOK_CDECL BrFixedRLength4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d);

br_fixed_ls C2_HOOK_CDECL BrFixedDiv(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedDivR(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedDivF(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedMulDiv(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c);

br_fixed_ls C2_HOOK_CDECL BrFixedMac2Div(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e);

br_fixed_ls C2_HOOK_CDECL BrFixedMac3Div(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f, br_fixed_ls g);

br_fixed_ls C2_HOOK_CDECL BrFixedMac4Div(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d, br_fixed_ls e, br_fixed_ls f, br_fixed_ls g, br_fixed_ls h, br_fixed_ls i);

br_fixed_ls C2_HOOK_CDECL BrFixedFMac2(br_fixed_lsf a, br_fixed_ls b, br_fixed_lsf c, br_fixed_ls d);

br_fixed_ls C2_HOOK_CDECL BrFixedFMac3(br_fixed_lsf a, br_fixed_ls b, br_fixed_lsf c, br_fixed_ls d, br_fixed_lsf e, br_fixed_ls f);

br_fixed_ls C2_HOOK_CDECL BrFixedFMac4(br_fixed_lsf a, br_fixed_ls b, br_fixed_lsf c, br_fixed_ls d, br_fixed_lsf e, br_fixed_ls f, br_fixed_lsf g, br_fixed_ls h);

br_fixed_ls C2_HOOK_CDECL BrFixedRcp(br_fixed_ls a);

br_fixed_ls C2_HOOK_CDECL BrFixedSqr(br_fixed_ls a);

br_fixed_ls C2_HOOK_CDECL BrFixedSqr2(br_fixed_ls a, br_fixed_ls b);

br_fixed_ls C2_HOOK_CDECL BrFixedSqr3(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c);

br_fixed_ls C2_HOOK_CDECL BrFixedSqr4(br_fixed_ls a, br_fixed_ls b, br_fixed_ls c, br_fixed_ls d);

br_fixed_ls C2_HOOK_CDECL BrFixedSin(br_angle a);

br_fixed_ls C2_HOOK_CDECL BrFixedCos(br_angle a);

br_angle C2_HOOK_CDECL BrFixedASin(br_fixed_ls a);

br_angle C2_HOOK_CDECL BrFixedACos(br_fixed_ls a);

br_angle C2_HOOK_CDECL BrFixedATan2(br_fixed_ls x, br_fixed_ls y);

br_angle C2_HOOK_CDECL BrFixedATan2Fast(br_fixed_ls x, br_fixed_ls y);

br_uint_16 C2_HOOK_CDECL _BrISqrt32(br_uint_32 a);

br_uint_32 C2_HOOK_FASTCALL _BrISqrt64(uint64_t a);

br_uint_16 C2_HOOK_CDECL _BrFastSqrt32(br_uint_32 v);

br_uint_32 C2_HOOK_CDECL _BrFastRSqrt32(br_uint_32 v);

br_uint_32 C2_HOOK_FASTCALL _BrFastRSqrt64(uint64_t v);

#endif
