/*
 * Copyright (C) 2024, Advanced Micro Devices. All rights reserved.
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

#include <libm_util_amd.h>
#include <libm/alm_special.h>

#include <libm_macros.h>
#include <libm/amd_funcs_internal.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/typehelper-vec.h>
#include <libm/compiler.h>

#define AMD_LIBM_FMA_USABLE 1           /* needed for poly-vec.h */
#include <libm/poly-vec.h>

typedef struct {
    double head;
    double tail;
} lookup_data;

extern lookup_data log_f_256[];
extern lookup_data log_Finv[];
extern double two_to_jby1024_double[];

#define VECTOR_LENGTH 4
#define N 10
#define TAB_F_INV log_Finv
#define TAB_LOG   log_f_256
#define TWO_POWER_J_BY_N  two_to_jby1024_double
#define MANT_MASK_N  (0x000FFC0000000000ULL)
#define DOUBLE_PRECISION_BIAS 1023
#define DOUBLE_PRECISION_MANTISSA 0x000fffffffffffffULL
#define ONE_BY_TWO 0x3fe0000000000000ULL
#define SIGN_MASK   0x7FFFFFFFFFFFFFFFUL

static struct {
    v_i64x4_t float_bias;
    v_u64x4_t mantissa_bits, one_by_two, mant_8_bits;
    v_f64x4_t poly[8];
    v_f64x4_t ln2_head, ln2_tail;
} v_log_data = {
    .float_bias =  _MM_SET1_I64(DOUBLE_PRECISION_BIAS),
    .mantissa_bits = _MM_SET1_I64(DOUBLE_PRECISION_MANTISSA),
    .mant_8_bits = _MM_SET1_I64(MANT_MASK_N),
    .one_by_two = _MM_SET1_I64(ONE_BY_TWO),
    .ln2_head = _MM_SET1_PD4(0x1.62e42e0000000p-1), /* ln(2) head*/
    .ln2_tail = _MM_SET1_PD4(0x1.efa39ef35793cp-25), /* ln(2) tail*/
    /*
    * Polynomial constants, 1/x! (reciprocal x)
    */
    .poly = { /* skip for 0/1 and 1/1 */
        _MM_SET1_PD4(0x1.0000000000000p-1),    /* 1/2 */
        _MM_SET1_PD4(0x1.5555555555555p-2),    /* 1/3 */
        _MM_SET1_PD4(0x1.0000000000000p-2),    /* 1/4 */
        _MM_SET1_PD4(0x1.999999999999ap-3),    /* 1/5 */
        _MM_SET1_PD4(0x1.5555555555555p-3),    /* 1/6 */
        _MM_SET1_PD4(0x1.2492492492492p-3),    /* 1/7 */
        _MM_SET1_PD4(0x1.0000000000000p-3),    /* 1/8 */
        _MM_SET1_PD4(0x1.c71c71c71c71cp-4),    /* 1/9 */
    },
};

static struct {
    v_f64x4_t tblsz_byln2;
    v_f64x4_t Huge;
    v_f64x4_t ln2_by_n_head, ln2_by_n_tail;
    v_i64x4_t one;
    v_f64x4_t poly[4];
} v_exp_data  = {
    //.exp_max = _MM_SET1_I64x2(0x4086200000000000),
    .tblsz_byln2 = _MM_SET1_PD4(0x1.71547652b82fep10),
    .ln2_by_n_head = _MM_SET1_PD4(0x1.62e42f0000000p-11),
    .ln2_by_n_tail = _MM_SET1_PD4(0x1.DF473DE6AF279p-36),
    .one =  _MM_SET1_I64(0x3ff0000000000000ULL),
    .Huge = _MM_SET1_PD4(0x1.8000000000000p+52),
    .poly = {
        _MM_SET1_PD4(0x1.0000000000000p-1),
        _MM_SET1_PD4(0x1.5555555555555p-3),
        _MM_SET1_PD4(0x1.5555555555555p-5),
        _MM_SET1_PD4(0x1.1111111111111p-7),
    },
};

#define MANTISSA_N_BITS v_log_data.mant_8_bits
#define MANTISSA_BITS   v_log_data.mantissa_bits
#define DP64_BIAS       v_log_data.float_bias
#define DP_HALF         v_log_data.one_by_two
#define LN2_HEAD        v_log_data.ln2_head
#define LN2_TAIL        v_log_data.ln2_tail

#define LN2_BY_N_HEAD   v_exp_data.ln2_by_n_head
#define LN2_BY_N_TAIL   v_exp_data.ln2_by_n_tail
#define INVLN2          v_exp_data.tblsz_byln2
#define EXP_HUGE        v_exp_data.Huge
#define ONE             v_exp_data.one

/*
 * Short names for polynomial coefficients
 */

#define A1  v_log_data.poly[0]
#define A2  v_log_data.poly[1]
#define A3  v_log_data.poly[2]
#define A4  v_log_data.poly[3]

#define B1  v_exp_data.poly[0]
#define B2  v_exp_data.poly[1]
#define B3  v_exp_data.poly[2]
#define B4  v_exp_data.poly[3]

#define EXP_MAX   0x4086200000000000
#define SCALAR_POW ALM_PROTO(pow)

/*
 *   __m256d ALM_PROTO_OPT(vrd4_powx)(__m256d, double);
 *
 * Spec:
 *   - A variation of power function
 *   - Computes vector raised to a scalar power
 *
 *
 * Implementation Notes:
 * pow(x,y) = e^(y * log(x))
 *
 *  1. Calculation of log(x) proceeds using a vectorized version of the scalar log algorithm
 *     which returns both head and tail portions for increased accuracy.
 *
 *  2. Computation of e^(y * log(x)) then proceeds similarly
 *
 */

 __m256d
ALM_PROTO_OPT(vrd4_powx)(__m256d _x, double _y)
{
     __m256d result;

     v_u64x4_t ux = as_v4_u64_f64(_x);

     /* This portion of the code is a vectorized version of the scalar log.c, with some checks removed */

    v_u64x4_t int_exponent = (ux >> 52) - DP64_BIAS ;

    v_u64x4_t mant  = ((ux & MANTISSA_BITS) | DP_HALF);

    v_u64x4_t index = ux & MANTISSA_N_BITS;

    v_f64x4_t index_times_half = as_v4_f64_u64(index | DP_HALF);

    index =  index >> (52 - N);

    v_f64x4_t y1  = as_v4_f64_u64(mant);

    v_f64x4_t f = index_times_half - y1;

    v_f64x4_t F_INV_HEAD, F_INV_TAIL;

    v_f64x4_t LOG_256_HEAD, LOG_256_TAIL;

    v_f64x4_t exponent, r, r1, u;

    /* Avoiding the use of vgatherpd instruction for performance reasons */

    for(int i = 0; i < VECTOR_LENGTH; i++) {

        int32_t j = (int32_t)index[i];

        exponent[i] = (double)int_exponent[i];

        F_INV_HEAD[i] = TAB_F_INV[j].head;

        F_INV_TAIL[i] = TAB_F_INV[j].tail;

        LOG_256_HEAD[i] = TAB_LOG[j].head;

        LOG_256_TAIL[i] = TAB_LOG[j].tail;

    }

    r = f * F_INV_TAIL;

    r1 = f * F_INV_HEAD;

    u = r + r1;

    v_f64x4_t z =  r1 - u;

    /* Use Estrin's polynomial to compute log(u) */

    v_f64x4_t u2 = u * u;

    v_f64x4_t a1 = A1  + u * A2;

    v_f64x4_t a2 = A3  + u * A4;

    v_f64x4_t u4 = u2 * u2;

    v_f64x4_t b1 = u2 * a1;

    v_f64x4_t  q1 = b1 + u4 * a2;

    v_f64x4_t poly = (z + r) + q1;

    /* m*log(2) + log(G) - poly */

    v_f64x4_t temp = (LN2_TAIL * exponent - poly) + LOG_256_TAIL;

    v_f64x4_t resT  = temp - u;

    v_f64x4_t resH = LN2_HEAD * exponent + LOG_256_HEAD;

    v_f64x4_t logx_h = resH + resT;

    v_f64x4_t logx_t = resH - logx_h + resT;

    v_f64x4_t vector_y = _MM_SET1_PD4(_y);

    v_f64x4_t ylogx_h = logx_h * vector_y;

    v_f64x4_t ylogx_t = logx_t * vector_y + _mm256_fmadd_pd(logx_h, vector_y, -ylogx_h);

    /* Calculate exp */

    v_u64x4_t v = as_v4_u64_f64(ylogx_h) & SIGN_MASK;

    z = ylogx_h * INVLN2;

    v_f64x4_t dn = z + EXP_HUGE;

    v_u64x4_t n = as_v4_u64_f64(dn);

    dn = dn - EXP_HUGE;

    index = n & DP64_BIAS;

    r = ylogx_h - (dn * LN2_BY_N_HEAD);

    v_i64x4_t m = (v_i64x4_t)(((n - index) << (52 - N)) + ONE);

    r = (r - (LN2_BY_N_TAIL * dn)) + ylogx_t;

    /* poly = r + r^2*D1 + r^3*(D2) + r^4*D3 */

    v_f64x4_t r2 = r * r;

    poly = POLY_EVAL_1(r, B1, B2) + r2 * r2  * B3;

    v_f64x4_t j_by_N;

    for (int i = 0; i < VECTOR_LENGTH; i++) {

        int32_t j = (int32_t)index[i];

        j_by_N[i] = TWO_POWER_J_BY_N[j];

    }

    z = poly * j_by_N + j_by_N;

    result = z * as_v4_f64_i64(m);

    /* Check for special cases */
    /* If y*log(x) is outside valid range, call scalar pow(value) */
    /* Otherwise, return the above computed result */
    for(int i = 0; i < VECTOR_LENGTH; i++) {
        if(unlikely(v[i] >= EXP_MAX)){
            result[i] = ALM_PROTO(pow)(_x[i], _y);
         }
    }

    return result;

}

