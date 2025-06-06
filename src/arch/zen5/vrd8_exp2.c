/*
 * Copyright (C) 2024 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * C implementation of exp2 double precision 512-bit vector version (v8d)
 *
 * Signature:
 *    v_f64x8_t amd_vrd8_exp2(v_f64x8_t x)
 *
 * Maximum ULP is 2.30127
 *
 * Implementation Notes
 * ----------------------
 * 0. Choosing N = 64
 *
 * 1. Argument Reduction:
 *      2^(x)   =   2^(x*N/N)                --- (1)
 *
 *      Let x = n + f                        --- (2)
 *      | n is integer and |f| <= 0.5
 *
 * 2. Polynomial Evaluation
 *   From (2),
 *     f  = x - n
 *
 *   Taylor series only exist for e^x
 *        2^x = e^(x*ln2)
 *     => 2^r = e^(r*ln2)
 *
 * 3. Reconstruction
 *      Thus,
 *        2^x = 2^m * 2^r
 */

#include <libm_macros.h>
#include <libm/compiler.h>
#include <libm_util_amd.h>
#include <libm/types.h>
#include <libm/typehelper-vec.h>
#include <libm/amd_funcs_internal.h>
#include <libm/poly-vec.h>

#include <libm/arch/zen5.h>

static struct {
    v_u64x8_t   mask;
    v_u64x8_t   arg_max;
    v_u64x8_t   all_ones;
    v_f64x8_t   huge;
    v_f64x8_t   ln2;
    v_f64x8_t   poly[12];
    } exp2_data = {
    .mask           = _MM512_SET1_U64x8(0x7FFFFFFFFFFFFFFFUL),
    .arg_max        = _MM512_SET1_U64x8(0x408FF00000000000UL),
    .all_ones       = _MM512_SET1_U64x8(0xFFFFFFFFFFFFFFFFUL),
    .huge           = _MM512_SET1_PD8(0x1.8p+52),
    .ln2            = _MM512_SET1_PD8(0x1.62e42fefa39efp-1),
    .poly           = {
        _MM512_SET1_PD8(0x1p0),
        _MM512_SET1_PD8(0x1.000000000001p-1),
        _MM512_SET1_PD8(0x1.55555555554a2p-3),
        _MM512_SET1_PD8(0x1.555555554f37p-5),
        _MM512_SET1_PD8(0x1.1111111130dd6p-7),
        _MM512_SET1_PD8(0x1.6c16c1878111dp-10),
        _MM512_SET1_PD8(0x1.a01a011057479p-13),
        _MM512_SET1_PD8(0x1.a01992d0fe581p-16),
        _MM512_SET1_PD8(0x1.71df4520705a4p-19),
        _MM512_SET1_PD8(0x1.28b311c80e499p-22),
        _MM512_SET1_PD8(0x1.ad661ce7af3e3p-26),
    },
};

#define ALM_V8_EXP2_HUGE             exp2_data.huge
#define ALM_V8_EXP2_LN2              exp2_data.ln2
#define ALM_V8_EXP2_ARG_MAX          exp2_data.arg_max
#define ALM_V8_EXP2_MASK             exp2_data.mask
#define ALM_V8_EXP2_ALL_ONES         exp2_data.all_ones

#define C1  exp2_data.poly[0]
#define C3  exp2_data.poly[1]
#define C4  exp2_data.poly[2]
#define C5  exp2_data.poly[3]
#define C6  exp2_data.poly[4]
#define C7  exp2_data.poly[5]
#define C8  exp2_data.poly[6]
#define C9  exp2_data.poly[7]
#define C10 exp2_data.poly[8]
#define C11 exp2_data.poly[9]
#define C12 exp2_data.poly[10]

#define SCALAR_EXP2  ALM_PROTO(exp2)

v_f64x8_t
ALM_PROTO_ARCH_ZN5(vrd8_exp2)(v_f64x8_t input)
{
    v_f64x8_t   r, dn;
    v_u64x8_t   vx, n, m, cond;

    /* Get absolute value */
    vx = as_v8_u64_f64(input) & ALM_V8_EXP2_MASK;

    /* Check if -1023 < vx < 1023 */
    cond = ((vx) > ALM_V8_EXP2_ARG_MAX);

    dn = input + ALM_V8_EXP2_HUGE;

    n = as_v8_u64_f64(dn);

    /* dn = double(n) */
    dn = dn - ALM_V8_EXP2_HUGE;

    r = input - dn;
    r *= ALM_V8_EXP2_LN2;

    /* Calculate m = 2^m  */
    m = n << 52;

    /* 
     * poly = C1 + C2*r + C3*r^2 + C4*r^3 + C5*r^4 + C6*r^5 +
     *        C7*r^6 + C8*r^7 + C9*r^8 + C10*r^9 + C11*r^10 + C12*r^11
     */

    v_f64x8_t poly = POLY_EVAL_11(r, C1, C1, C3, C4, C5, C6,
                                     C7, C8, C9, C10, C11, C12);

    /* result = poly * 2^m */
    v_f64x8_t ret = as_v8_f64_u64(as_v8_u64_f64(poly) + m);

    if(unlikely(_mm512_test_epi64_mask(cond, ALM_V8_EXP2_ALL_ONES))) {
        ret = (v_f64x8_t) {
            (cond[0]) ? SCALAR_EXP2(input[0]) : ret[0],
            (cond[1]) ? SCALAR_EXP2(input[1]) : ret[1],
            (cond[2]) ? SCALAR_EXP2(input[2]) : ret[2],
            (cond[3]) ? SCALAR_EXP2(input[3]) : ret[3],
            (cond[4]) ? SCALAR_EXP2(input[4]) : ret[4],
            (cond[5]) ? SCALAR_EXP2(input[5]) : ret[5],
            (cond[6]) ? SCALAR_EXP2(input[6]) : ret[6],
            (cond[7]) ? SCALAR_EXP2(input[7]) : ret[7],
        };
    }
    return ret;
}
