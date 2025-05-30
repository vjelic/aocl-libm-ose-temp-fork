/*
 * Copyright (C) 2025 Advanced Micro Devices, Inc. All rights reserved.
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


#include <cmath>
#include "libm_tests.h"
#include <libm_macros.h>

#define AMD_LIBM_VEC_EXPERIMENTAL

#include <libm_amd.h>
#include <libm/amd_funcs_internal.h>
#include <fmaintrin.h>
#include <immintrin.h>
#include "callback.h"
#include "../libs/mparith/alm_mp_funcs.h"

void LIBM_FUNC(sincosf)(float, float*, float*);
void LIBM_FUNC(sincos) (double, double*, double*);

static uint32_t ipargs = 1;
bool specialcase = false;

uint32_t GetnIpArgs( void )
{
  return ipargs;
}

bool getSpecialCase(void)
{
  return specialcase;
}

void ConfSetupf32(SpecParams *specp) {
  specp->data32 = nullptr;
  specp->countf = 0;
}

void ConfSetupf64(SpecParams *specp) {
  specp->data64 = nullptr;
  specp->countd = 0;
}

void getExpected(float *data, double *op) {
  alm_mp_sincosf(data[0], &op[0], &op[1]);
}

void getExpected(double *data, long double *op) {
  alm_mp_sincos(data[0], &op[0], &op[1]);
}

/**********************
*FUNCTIONS*
**********************/
int test_s1s(test_data *data, int idx)  {
  float *ip   = (float*)data->ip;
  float *ops  = (float*)data->op;
  float *opc  = (float*)data->sc;
  LIBM_FUNC(sincosf)(ip[idx], &ops[0], &opc[0]);
  return 0;
}

int test_s1d(test_data *data, int idx)  {
  double *ip  = (double*)data->ip;
  double *ops  = (double*)data->op;
  double *opc  = (double*)data->sc;
  LIBM_FUNC(sincos)(ip[idx], &ops[0], &opc[0]);
  return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/* GLIBC prototype declarations */
#if (LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
#define _ZGVdN2v_sincos _ZGVbN2vvv_sincos
#define _ZGVdN4v_sincos _ZGVdN4vvv_sincos
#define _ZGVsN4v_sincosf _ZGVbN4vvv_sincosf
#define _ZGVsN8v_sincosf _ZGVdN8vvv_sincosf

#define _ZGVsN16v_sincosf _ZGVeN16vvv_sincosf
#define _ZGVdN8v_sincos _ZGVeN8vvv_sincos
#endif

/*vector routines*/
#if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  void LIBM_FUNC_VEC(d, 2, sincos)(__m128d, __m128d*, __m128d*);
  void LIBM_FUNC_VEC(d, 4, sincos)(__m256d, __m256d*, __m256d*);
  void LIBM_FUNC_VEC(s, 4, sincosf)(__m128, __m128*, __m128*);
  void LIBM_FUNC_VEC(s, 8, sincosf)(__m256, __m256*, __m256*);

  #if defined(__AVX512__)
    void LIBM_FUNC_VEC(d, 8, sincos)(__m512d, __m512d*, __m512d*);
    void LIBM_FUNC_VEC(s, 16, sincosf)(__m512, __m512*, __m512*);
  #endif
#endif

int test_v2d(test_data *data, int idx) {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  double *ip  = (double*)data->ip;
  double *ops  = (double*)data->op;
  double *opc  = (double*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  __m128d ip2 = _mm_set_pd(ip[idx+1], ip[idx]);
  __m128d sin = {0};
  __m128d cos = {0};
  LIBM_FUNC_VEC(d, 2, sincos)(ip2, &sin, &cos);
  _mm_storeu_pd(ops, sin);
  _mm_storeu_pd(opc, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  double ip2a[2] = {ip[idx], ip[idx+1]};
  vdSinCos(2, ip2a , ops, opc);
#endif
  #endif
  return 0;
}

int test_v4s(test_data *data, int idx) {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  float *ip  = (float*)data->ip;
  float *ops  = (float*)data->op;
  float *opc  = (float*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  __m128 ip4 = _mm_set_ps(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m128 sin = {0};
  __m128 cos = {0};
  LIBM_FUNC_VEC(s, 4, sincosf)(ip4, &sin, &cos);
  _mm_storeu_ps(ops, sin);
  _mm_storeu_ps(opc, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  float ip4a[4] = {ip[idx], ip[idx+1], ip[idx+2], ip[idx+3]};
  vsSinCos(4, ip4a , ops, opc);
#endif
  #endif
  return 0;
}

int test_v4d(test_data *data, int idx) {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  double *ip  = (double*)data->ip;
  double *ops  = (double*)data->op;
  double *opc  = (double*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  __m256d ip4 = _mm256_set_pd(ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256d sin = {0};
  __m256d cos = {0};
  LIBM_FUNC_VEC(d, 4, sincos)(ip4, &sin, &cos);
  _mm256_storeu_pd(ops, sin);
  _mm256_storeu_pd(opc, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  double ip4a[4] = {ip[idx], ip[idx+1], ip[idx+2], ip[idx+3]};
  vdSinCos(4, ip4a , ops, opc);
#endif
  #endif
  return 0;
}

int test_v8s(test_data *data, int idx) {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  float *ip  = (float*)data->ip;
  float *ops  = (float*)data->op;
  float *opc  = (float*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  __m256 ip8 = _mm256_set_ps(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                              ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m256 sin = {0};
  __m256 cos = {0};
  LIBM_FUNC_VEC(s, 8, sincosf)(ip8, &sin, &cos);
  _mm256_storeu_ps(ops, sin);
  _mm256_storeu_ps(opc, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  float ip8a[8] = {ip[idx], ip[idx+1], ip[idx+2], ip[idx+3],
                   ip[idx+4], ip[idx+5], ip[idx+6], ip[idx+7]};
  vsSinCos(8, ip8a , ops, opc);
#endif
  #endif
  return 0;
}

int test_v8d(test_data *data, int idx) {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  #if defined(__AVX512__)
  double *ip  = (double*)data->ip;
  double *ops  = (double*)data->op;
  double *opc  = (double*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  __m512d ip8 = _mm512_set_pd(ip[idx+7], ip[idx+6], ip[idx+5], ip[idx+4],
                              ip[idx+3], ip[idx+2], ip[idx+1], ip[idx]);
  __m512d sin = {0};
  __m512d cos = {0};
  LIBM_FUNC_VEC(d, 8, sincos)(ip8,  &sin, &cos);
  _mm512_storeu_pd(ops, sin);
  _mm512_storeu_pd(opc, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  double ip8a[8] = {ip[idx], ip[idx+1], ip[idx+2], ip[idx+3],
                    ip[idx+4], ip[idx+5], ip[idx+6], ip[idx+7]};
  vdSinCos(8, ip8a , ops, opc);
#endif
  #endif
  #endif
  return 0;
}

int test_v16s(test_data *data, int idx) {
  #if (LIBM_PROTOTYPE != PROTOTYPE_MSVC)
  #if defined(__AVX512__)
  float *ip  = (float*)data->ip;
  float *ops  = (float*)data->op;
  float *opc  = (float*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL || LIBM_PROTOTYPE == PROTOTYPE_GLIBC)
  __m512 ip16 = _mm512_set_ps(ip[idx+15], ip[idx+14], ip[idx+13], ip[idx+12],
                              ip[idx+11], ip[idx+10], ip[idx+9],  ip[idx+8],
                              ip[idx+7],  ip[idx+6],  ip[idx+5],  ip[idx+4],
                              ip[idx+3],  ip[idx+2],  ip[idx+1],  ip[idx]);
  __m512 sin = {0};
  __m512 cos = {0};
  LIBM_FUNC_VEC(s, 16, sincosf)(ip16, &sin, &cos);
  _mm512_storeu_ps(ops, sin);
  _mm512_storeu_ps(opc, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  float ip16a[16] = {ip[idx], ip[idx+1], ip[idx+2], ip[idx+3],
                     ip[idx+4], ip[idx+5], ip[idx+6], ip[idx+7],
                     ip[idx+8], ip[idx+9], ip[idx+10], ip[idx+11],
                     ip[idx+12], ip[idx+13], ip[idx+14], ip[idx+15]};
  vsSinCos(16, ip16a , ops, opc);
#endif
  #endif
  #endif
  return 0;
}

int test_vad(test_data *data, int count)  {
#if (LIBM_PROTOTYPE != PROTOTYPE_GLIBC)
  double *ip  = (double*)data->ip;
  double *sin  = (double*)data->op;
  double *cos  = (double*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrda_sincos(count, ip , sin, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vdSinCos(count, ip , sin, cos);
#endif
#endif
 return 0;
}

int test_vas(test_data *data, int count)  {
#if (LIBM_PROTOTYPE != PROTOTYPE_GLIBC)
  float *ip   = (float*)data->ip;
  float *sin  = (float*)data->op;
  float *cos  = (float*)data->sc;
#if (LIBM_PROTOTYPE == PROTOTYPE_AOCL)
  amd_vrsa_sincosf(count, ip , sin, cos);
#elif (LIBM_PROTOTYPE == PROTOTYPE_SVML)
  vsSinCos(count, ip , sin, cos);
#endif
#endif
  return 0;
}

#ifdef __cplusplus
}
#endif