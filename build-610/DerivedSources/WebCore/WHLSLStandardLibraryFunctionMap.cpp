
/*
 * Copyright (C) 2019 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WHLSLStandardLibraryFunctionMap.h"

#if ENABLE(WEBGPU)

namespace WebCore {

namespace WHLSL {

HashMap<String, SubstringLocation> computeStandardLibraryFunctionMap()
{
    HashMap<String, SubstringLocation> result;
    result.add("reversebits"_str, SubstringLocation { 5494, 7375 });
    result.add("isordered"_str, SubstringLocation { 7375, 8060 });
    result.add("transpose"_str, SubstringLocation { 8060, 11080 });
    result.add("InterlockedAdd"_str, SubstringLocation { 11080, 11978 });
    result.add("saturate"_str, SubstringLocation { 11978, 16301 });
    result.add("mad"_str, SubstringLocation { 16301, 21961 });
    result.add("modf"_str, SubstringLocation { 21961, 44365 });
    result.add("InterlockedMin"_str, SubstringLocation { 44365, 45263 });
    result.add("length"_str, SubstringLocation { 45263, 45408 });
    result.add("operator--"_str, SubstringLocation { 45408, 45604 });
    result.add("operator<<"_str, SubstringLocation { 45604, 45708 });
    result.add("exp2"_str, SubstringLocation { 45708, 49623 });
    result.add("isunordered"_str, SubstringLocation { 49623, 50337 });
    result.add("atan"_str, SubstringLocation { 50337, 54219 });
    result.add("log"_str, SubstringLocation { 54219, 57997 });
    result.add("Load"_str, SubstringLocation { 57997, 61342 });
    result.add("sin"_str, SubstringLocation { 61342, 65120 });
    result.add("sincos"_str, SubstringLocation { 65120, 159177 });
    result.add("InterlockedCompareExchange"_str, SubstringLocation { 159177, 160297 });
    result.add("smoothstep"_str, SubstringLocation { 160297, 166759 });
    result.add("exp"_str, SubstringLocation { 166759, 170537 });
    result.add("all"_str, SubstringLocation { 170537, 179717 });
    result.add("firstbitlow"_str, SubstringLocation { 179717, 182568 });
    result.add("operator<="_str, SubstringLocation { 182568, 183484 });
    result.add("ddx_fine"_str, SubstringLocation { 183484, 187799 });
    result.add("operator cast"_str, SubstringLocation { 187799, 192211 });
    result.add("load"_str, SubstringLocation { 192211, 192401 });
    result.add("count_bits"_str, SubstringLocation { 192401, 192435 });
    result.add("rsqrt"_str, SubstringLocation { 192435, 196443 });
    result.add("sqrt"_str, SubstringLocation { 196443, 200347 });
    result.add("acos"_str, SubstringLocation { 200347, 204229 });
    result.add("operator>"_str, SubstringLocation { 204229, 205123 });
    result.add("operator&"_str, SubstringLocation { 205123, 205277 });
    result.add("tanh"_str, SubstringLocation { 205277, 209159 });
    result.add("operator*"_str, SubstringLocation { 209159, 211520 });
    result.add("cos"_str, SubstringLocation { 211520, 215298 });
    result.add("operator=="_str, SubstringLocation { 215298, 216772 });
    result.add("operator>>"_str, SubstringLocation { 216772, 216876 });
    result.add("reflect"_str, SubstringLocation { 216876, 217197 });
    result.add("lit"_str, SubstringLocation { 217197, 217534 });
    result.add("log2"_str, SubstringLocation { 217534, 221449 });
    result.add("normalize"_str, SubstringLocation { 221449, 221709 });
    result.add("ldexp"_str, SubstringLocation { 221709, 226649 });
    result.add("abs"_str, SubstringLocation { 226649, 231413 });
    result.add("round"_str, SubstringLocation { 231413, 235399 });
    result.add("mul"_str, SubstringLocation { 235399, 288946 });
    result.add("radians"_str, SubstringLocation { 288946, 293168 });
    result.add("ceil"_str, SubstringLocation { 293168, 297050 });
    result.add("asin"_str, SubstringLocation { 297050, 300932 });
    result.add("asuint"_str, SubstringLocation { 300932, 302108 });
    result.add("pow"_str, SubstringLocation { 302108, 306814 });
    result.add("tan"_str, SubstringLocation { 306814, 310592 });
    result.add("fma"_str, SubstringLocation { 310592, 316250 });
    result.add("determinant"_str, SubstringLocation { 316250, 317964 });
    result.add("floor"_str, SubstringLocation { 317964, 321950 });
    result.add("cross"_str, SubstringLocation { 321950, 322168 });
    result.add("isfinite"_str, SubstringLocation { 322168, 322715 });
    result.add("InterlockedOr"_str, SubstringLocation { 322715, 323600 });
    result.add("atan2"_str, SubstringLocation { 323600, 328514 });
    result.add("DeviceMemoryBarrierWithGroupSync"_str, SubstringLocation { 328514, 328618 });
    result.add("fmod"_str, SubstringLocation { 328618, 333523 });
    result.add("operator-"_str, SubstringLocation { 333523, 336461 });
    result.add("min"_str, SubstringLocation { 336461, 342296 });
    result.add("InterlockedXor"_str, SubstringLocation { 342296, 343194 });
    result.add("GroupMemoryBarrierWithGroupSync"_str, SubstringLocation { 343194, 343296 });
    result.add("trunc"_str, SubstringLocation { 343296, 347282 });
    result.add("max"_str, SubstringLocation { 347282, 353123 });
    result.add("asint"_str, SubstringLocation { 353123, 354249 });
    result.add("clamp"_str, SubstringLocation { 354249, 355235 });
    result.add("operator<"_str, SubstringLocation { 355235, 356129 });
    result.add("isnan"_str, SubstringLocation { 356129, 356635 });
    result.add("AllMemoryBarrierWithGroupSync"_str, SubstringLocation { 356635, 356733 });
    result.add("operator^"_str, SubstringLocation { 356733, 356912 });
    result.add("isinf"_str, SubstringLocation { 356912, 357417 });
    result.add("operator|"_str, SubstringLocation { 357417, 357571 });
    result.add("degrees"_str, SubstringLocation { 357571, 361793 });
    result.add("Sample"_str, SubstringLocation { 361793, 369837 });
    result.add("SampleLevel"_str, SubstringLocation { 369837, 375626 });
    result.add("SampleBias"_str, SubstringLocation { 375626, 381414 });
    result.add("SampleGrad"_str, SubstringLocation { 381414, 387982 });
    result.add("sinh"_str, SubstringLocation { 387982, 391864 });
    result.add("ddy"_str, SubstringLocation { 391864, 395642 });
    result.add("operator>="_str, SubstringLocation { 395642, 396558 });
    result.add("dst"_str, SubstringLocation { 396558, 397099 });
    result.add("dot"_str, SubstringLocation { 397099, 398851 });
    result.add("asfloat"_str, SubstringLocation { 398851, 400481 });
    result.add("isnormal"_str, SubstringLocation { 400481, 401030 });
    result.add("InterlockedExchange"_str, SubstringLocation { 401030, 401993 });
    result.add("GetDimensions"_str, SubstringLocation { 401993, 861729 });
    result.add("operator+"_str, SubstringLocation { 861729, 864702 });
    result.add("frac"_str, SubstringLocation { 864702, 868607 });
    result.add("store"_str, SubstringLocation { 868607, 868826 });
    result.add("refract"_str, SubstringLocation { 868826, 869733 });
    result.add("log10"_str, SubstringLocation { 869733, 873753 });
    result.add("operator/"_str, SubstringLocation { 873753, 876114 });
    result.add("ddy_fine"_str, SubstringLocation { 876114, 880429 });
    result.add("any"_str, SubstringLocation { 880429, 889638 });
    result.add("ddy_coarse"_str, SubstringLocation { 889638, 894161 });
    result.add("operator++"_str, SubstringLocation { 894161, 894357 });
    result.add("InterlockedAnd"_str, SubstringLocation { 894357, 895255 });
    result.add("operator~"_str, SubstringLocation { 895255, 895394 });
    result.add("ddx"_str, SubstringLocation { 895394, 899172 });
    result.add("InterlockedMax"_str, SubstringLocation { 899172, 900070 });
    result.add("cosh"_str, SubstringLocation { 900070, 903952 });
    result.add("ddx_coarse"_str, SubstringLocation { 903952, 908475 });
    result.add("firstbithigh"_str, SubstringLocation { 908475, 911354 });
    result.add("rcp"_str, SubstringLocation { 911354, 915148 });
    result.add("fwidth"_str, SubstringLocation { 915148, 919275 });
    result.add("lerp"_str, SubstringLocation { 919275, 925047 });
    result.add("distance"_str, SubstringLocation { 925047, 925336 });

    return result;
}

unsigned firstFunctionOffsetInStandardLibrary()
{
    return 5494;

}

}

}

#endif
