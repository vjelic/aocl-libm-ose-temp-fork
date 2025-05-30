/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#include <fenv.h>
#include "almstruct.h"
#include <libm_util_amd.h>
/*
 * Test cases to check for exceptions for the sinhf() routine.
 * These test cases are not exhaustive
 * These values as as per GLIBC output
 */
static libm_test_special_data_f32
test_sinhf_conformance_data[] = {
    // special accuracy tests
    {0x38800000, 0x38800000,  0},  //min= 0.00006103515625, small enough that sinh(x) = 1
    {0x387FFFFF, 0x38800000,  FE_UNDERFLOW}, //min - 1 bit
    {0x38800001, 0x38800001,  0}, //min + 1 bit
    {0xF149F2C9, 0xff800000,  FE_OVERFLOW}, //lambda + x = 1, x = -9.9999994e+29
    {0xF149F2C8, 0xff800000,  FE_OVERFLOW}, //lambda + x < 1
    {0xF149F2CA, 0xff800000,  FE_OVERFLOW}, //lambda + x > 1
    {0x42B2D4FC, 0x7f7fffec,  0}, //max arg, x = 89.41598629223294,max sinhf arg
    {0x42B2D4FB, 0x7f7fff6c,  0}, //max arg - 1 bit
    {0x42B2D4FD, 0x7f800000,  FE_OVERFLOW}, //max arg + 1 bit
    {0x42B2D4FF, 0x7f800000,  FE_OVERFLOW}, // > max
    {0x42B2D400, 0x7f7f820b,  0}, // < max
    {0x41A00000, 0x4d675844,  0}, //small_threshold = 20
    {0x41A80000, 0x4e1d3710,  0}, //small_threshold+1 = 21
    {0x41980000, 0x4caa36c8,  0}, //small_threshold - 1 = 19

    //sinh special exception checks
    {POS_ZERO_F32, POS_ZERO_F32, 0 },  //0
    {NEG_ZERO_F32, NEG_ZERO_F32, 0 },  //0
    {POS_INF_F32,  POS_INF_F32,  0 },
    {NEG_INF_F32,  NEG_INF_F32,  0 },
    {POS_SNAN_F32, POS_SNAN_F32, FE_INVALID },  //
    {NEG_SNAN_F32, NEG_SNAN_F32, FE_INVALID },  //
    {POS_QNAN_F32, POS_QNAN_F32, 0 },  //
    {NEG_QNAN_F32, NEG_QNAN_F32, 0 },  //
    {0x42BE0000,   POS_INF_F32,  FE_OVERFLOW },  //95
    {0xc2ce0000,   NEG_INF_F32,  FE_OVERFLOW},  //-103
    //{F32_POS_LDENORM,F32_POS_LDENORM,FE_UNDERFLOW},
    //{F32_NEG_LDENORM,F32_NEG_LDENORM,FE_UNDERFLOW},
    //{F32_POS_HDENORM,F32_POS_HDENORM,FE_UNDERFLOW},
    //{F32_NEG_HDENORM,F32_NEG_HDENORM,FE_UNDERFLOW},

    //test case reported
    {0xc9f5087f, NEG_INF_F32, FE_OVERFLOW},
    {0xca20fa65, NEG_INF_F32, FE_OVERFLOW},
    {0xca8272da, NEG_INF_F32, FE_OVERFLOW},
    {0xc935c441, NEG_INF_F32, FE_OVERFLOW},
    {0xc6daf501, NEG_INF_F32, FE_OVERFLOW},
    {0xc7b775d0, NEG_INF_F32, FE_OVERFLOW},

    {0x00000001, 0x00000001,  FE_UNDERFLOW},  // denormal min
    {0x0005fde6, 0x0005fde6,  FE_UNDERFLOW},  // denormal intermediate
    {0x007fffff, 0x007fffff,  FE_UNDERFLOW},  // denormal max
    {0x80000001, 0x80000001,  FE_UNDERFLOW},  // -denormal min
    {0x805def12, 0x805def12,  FE_UNDERFLOW},  // -denormal intermediate
    {0x807FFFFF, 0x807fffff,  FE_UNDERFLOW},  // -denormal max
    {0x00800000, 0x00800000,  FE_UNDERFLOW},  // normal min
    {0x43b3c4ea, 0x7f800000,  FE_OVERFLOW},  // normal intermediate
    {0x7f7fffff, 0x7f800000,  FE_OVERFLOW},  // normal max
    {0x80800000, 0x80800000,  FE_UNDERFLOW},  // -normal min
    {0xc5812e71, 0xff800000,  FE_OVERFLOW},  // -normal intermediate
    {0xFF7FFFFF, 0xff800000,  FE_OVERFLOW},  // -normal max
    {0x7F800000, 0x7f800000,  0},  // inf
    {0xfF800000, 0xff800000,  0},  // -inf
    {0x7Fc00000, 0x7fc00000,  0},  // qnan min
    {0x7Fe1a570, 0x7fe1a570,  0},  // qnan intermediate
    {0x7FFFFFFF, 0x7fffffff,  0},  // qnan max
    {0xfFc00000, 0xffc00000,  0},  // indeterninate
    {0xFFC00001, 0xffc00001,  0},  // -qnan min
    {0xFFd2ba31, 0xffd2ba31,  0},  // -qnan intermediate
    {0xFFFFFFFF, 0xffffffff,  0},  // -qnan max
    {0x7F800001, 0x7fc00001,  FE_INVALID},  // snan min
    {0x7Fa0bd90, 0x7fe0bd90,  FE_INVALID},  // snan intermediate
    {0x7FBFFFFF, 0x7fffffff,  FE_INVALID},  // snan max
    {0xFF800001, 0xffc00001,  FE_INVALID},  // -snan min
    {0xfF95fffa, 0xffd5fffa,  FE_INVALID},  // -snan intermediate
    {0xFFBFFFFF, 0xffffffff,  FE_INVALID},  // -snan max
    {0x3FC90FDB, 0x4013487c,  0},  // pi/2
    {0x40490FDB, 0x4138c7a4,  0},  // pi
    {0x40C90FDB, 0x4385df5a,  0},  // 2pi
    {0x402DF853, 0x40f1698d,  0},  // e --
    {0x402DF854, 0x40f16991,  0},  // e
    {0x402DF855, 0x40f16995,  0},  // e ++
    {0x00000000, 0x00000000,  0},  // 0
    {0x37C0F01F, 0x37c0f01f,  FE_UNDERFLOW},  // 0.000023
    {0x3EFFFEB0, 0x3f0565c3,  0},  // 0.49999
    {0x3F0000C9, 0x3f056763,  0},  // 0.500012
    {0x80000000, 0x80000000,  0},  // -0
    {0xb7C0F01F, 0xb7c0f01f,  FE_UNDERFLOW},  // -0.000023
    {0xbEFFFEB0, 0xbf0565c3,  0},  // -0.49999
    {0xbF0000C9, 0xbf056763,  0},  // -0.500012
    {0x3f800000, 0x3f966cfe,  0},  // 1
    {0x3f700001, 0x3f8a5df2,  0},  // 0.93750006
    {0x3F87FFFE, 0x3fa31281,  0},  // 1.0624998
    {0x3FBFFFAC, 0x400845bb,  0},  // 1.49999
    {0x3FC00064, 0x40084693,  0},  // 1.500012
    {0xbf800000, 0xbf966cfe,  0},  // -1
    {0xbf700001, 0xbf8a5df2,  0},  // -0.93750006
    {0xbF87FFFE, 0xbfa31281,  0},  // -1.0624998
    {0xbFBFFFAC, 0xc00845bb,  0},  // -1.49999
    {0xbFC00064, 0xc0084693,  0},  // -1.500012
    {0x40000000, 0x40681e7b,  0},  // 2
    {0xc0000000, 0xc0681e7b,  0},  // -2
    {0x41200000, 0x462c14ee,  0},  // 10
    {0xc1200000, 0xc62c14ee,  0},  // -10
    {0x447A0000, 0x7f800000,  FE_OVERFLOW},  // 1000
    {0xc47A0000, 0xff800000,  FE_OVERFLOW},  // -1000
    {0x4286CCCC, 0x6f96eb6f,  0},  // 67.4
    {0xc286CCCC, 0xef96eb6f,  0},  // -67.4
    {0x44F7F333, 0x7f800000,  FE_OVERFLOW},  // 1983.6
    {0xc4F7F333, 0xff800000,  FE_OVERFLOW},  // -1983.6
    {0x42AF0000, 0x7e16bab3,  0},  // 87.5
    {0xc2AF0000, 0xfe16bab3,  0},  // -87.5
    {0x48015E40, 0x7f800000,  FE_OVERFLOW},  // 132473
    {0xc8015E40, 0xff800000,  FE_OVERFLOW},  // -132473
    {0x4B000000, 0x7f800000,  FE_OVERFLOW},  // 2^23
    {0x4B000001, 0x7f800000,  FE_OVERFLOW},  // 2^23 + 1
    {0x4AFFFFFF, 0x7f800000,  FE_OVERFLOW},  // 2^23 -1 + 0.5
    {0xcB000000, 0xff800000,  FE_OVERFLOW},  // -2^23
    {0xcB000001, 0xff800000,  FE_OVERFLOW},  // -(2^23 + 1)
    {0xcAFFFFFF, 0xff800000,  FE_OVERFLOW},  // -(2^23 -1 + 0.5)

    {0x80000000, 0x80000000,  0},
    {0x00000000, 0x00000000,  0},
    {0x7f800000, 0x7f800000,  0},
    {0xff800000, 0xff800000,  0},

    {0x7f800001, 0x7fc00001,  0},
    {0xff800001, 0xffc00001,  0},
    {0x7fc00000, 0x7fc00000,  0},
    {0xffc00000, 0xffc00000,  0},

    //answer from NAG test tool  
    {0x38000000, 0x38000000,  0}, // 2^(-15), < 2 ^(-14), x
    {0xb8000000, 0xb8000000,  0}, //-2^(-15), < 2 ^(-14), x
    {0x42b40000, 0x7f800000,  0}, // 90, > max_sinh_arg, +inf
    {0xc2b40000, 0xff800000,  0}, //-90, > max_sinh_arg, -inf
    {0x41f00000, 0x549b8238,  0}, // 30, > small_threshold
    {0xc1f00000, 0xd49b8238,  0}, //-30, > small_threshold
    {0x3f800000, 0x3f966cfe,  0}, // 1
    {0xbf800000, 0xbf966cfe,  0}, //-1
    {0x3dcccccd, 0x3dcd243a,  0}, // 0.1
    {0xb951b717, 0xb951b717,  0}, //-0.0002


};

static libm_test_special_data_f64
test_sinh_conformance_data[] = {
    // special accuracy tests
    {0x3e30000000000000LL, 0x3e30000000000000LL, 0},  //min, small enough that sinh(x) = 1 //
    {0x3E2FFFFFFFFFFFFFLL, 0x3e2fffffffffffffLL, FE_UNDERFLOW}, //min - 1 bit
    {0x3e30000000000001LL, 0x3e30000000000001LL, 0}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0xfff0000000000000LL, FE_OVERFLOW}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0xfff0000000000000LL, FE_OVERFLOW}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0xfff0000000000000LL, FE_OVERFLOW}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg, x = 89.41598629223294,max sinhf arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, 0}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, FE_OVERFLOW}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // > max
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, 0}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, 0}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, 0}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e1LL, 0}, //small_threshold - 1 = 19

    //special exception tests
    {POS_ZERO_F64, POS_ZERO_F64,0 },  //0
    {NEG_ZERO_F64, NEG_ZERO_F64,0 },  //0
    {POS_INF_F64,  POS_INF_F64,0 },
    {NEG_INF_F64,  NEG_INF_F64,0 },
    {POS_SNAN_F64, POS_SNAN_F64,FE_INVALID },  //
    {NEG_SNAN_F64, NEG_SNAN_F64,FE_INVALID },  //
    {POS_QNAN_F64, POS_QNAN_F64,0 },  //
    {NEG_QNAN_F64, NEG_QNAN_F64,0 },  //
    {0x43E0000020000000LL,POS_INF_F64,FE_OVERFLOW},  //
    {0x408633CED916872BLL,POS_INF_F64,FE_OVERFLOW}, //7.104760e+002
    {0xC0FF5D35B020C49CLL,NEG_INF_F64,FE_OVERFLOW},  // -708.3964185322642 largest denormal result
    {0xC08F400000000000LL,NEG_INF_F64,FE_OVERFLOW}, // -1000
    //{F64_POS_LDENORM,F64_POS_LDENORM,FE_UNDERFLOW},
    //{F64_NEG_LDENORM,F64_NEG_LDENORM,FE_UNDERFLOW},

    {0x0000000000000001LL, 0x0000000000000001LL, FE_UNDERFLOW}, // denormal min
    {0x0005fde623545abcLL, 0x0005fde623545abcLL, FE_UNDERFLOW}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0x000fffffffffffffLL, FE_UNDERFLOW}, // denormal max
    {0x8000000000000001LL, 0x8000000000000001LL, FE_UNDERFLOW}, // -denormal min
    {0x8002344ade5def12LL, 0x8002344ade5def12LL, FE_UNDERFLOW}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0x800fffffffffffffLL, FE_UNDERFLOW}, // -denormal max
    {0x0010000000000000LL, 0x0010000000000000LL, FE_UNDERFLOW}, // normal min
    {0x43b3c4eafedcab02LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // normal max
    {0x8010000000000000LL, 0x8010000000000000LL, FE_UNDERFLOW}, // -normal min
    {0xc5812e71245acfdbLL, 0xfff0000000000000LL, FE_OVERFLOW}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xfff0000000000000LL, FE_OVERFLOW}, // -normal max
    {0x7FF0000000000000LL, 0x7ff0000000000000LL, 0}, // inf
    {0xFFF0000000000000LL, 0xfff0000000000000LL, 0}, // -inf
    {0x7FF8000000000000LL, 0x7ff8000000000000LL, 0}, // qnan min
    {0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL, 0}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL, 0}, // qnan max
    {0xFFF8000000000000LL, 0xfff8000000000000LL, 0}, // indeterninate
    {0xFFF8000000000001LL, 0xfff8000000000001LL, 0}, // -qnan min
    {0xFFF9123425dcba31LL, 0xfff9123425dcba31LL, 0}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFFLL, 0xffffffffffffffffLL, 0}, // -qnan max
    {0x7FF0000000000001LL, 0x7ff8000000000001LL, FE_INVALID}, // snan min
    {0x7FF5344752a0bd90LL, 0x7ffd344752a0bd90LL, FE_INVALID}, // snan intermediate
    {0x7FF7FFFFFFFFFFFFLL, 0x7fffffffffffffffLL, FE_INVALID}, // snan max
    {0xFFF0000000000001LL, 0xfff8000000000001LL, FE_INVALID}, // -snan min
    {0xfFF432438995fffaLL, 0xfffc32438995fffaLL, FE_INVALID}, // -snan intermediate
    {0xFFF7FFFFFFFFFFFFLL, 0xffffffffffffffffLL, FE_INVALID}, // -snan max
    {0x3FF921FB54442D18LL, 0x4002690f661dd820LL, 0}, // pi/2
    {0x400921FB54442D18LL, 0x402718f45d72e671LL, 0}, // pi
    {0x401921FB54442D18LL, 0x4070bbeb1603926aLL, 0}, // 2pi
    {0x3FFB7E151628AED3LL, 0x4005950fa1f030c5LL, 0}, // e --
    {0x4005BF0A8B145769LL, 0x401e2d3246d48e6fLL, 0}, // e
    {0x400DBF0A8B145769LL, 0x403495aaf7d6b9d9LL, 0}, // e ++
    {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, FE_UNDERFLOW}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3fe0accdb2828938LL, 0}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3fe0acd00fe63c11LL, 0}, // 0.500000000000012
    {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, FE_UNDERFLOW}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0xbfe0accdb2828938LL, 0}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0xbfe0acd00fe63c11LL, 0}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x3ff2cd9fc44eb982LL, 0}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x3ff2cd9f967c1586LL, 0}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3ff2cd9fc44ebb34LL, 0}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x400108c26f0145f0LL, 0}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x400108c3aabd6a62LL, 0}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0xbff2cd9fc44eb982LL, 0}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0xbff2cd9f967c1586LL, 0}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xbff2cd9fc44ebb34LL, 0}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xc00108c26f0145f0LL, 0}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xc00108c3aabd6a62LL, 0}, // -1.50000000000000012
    {0x4000000000000000LL, 0x400d03cf63b6e19fLL, 0}, // 2
    {0xC000000000000000LL, 0xc00d03cf63b6e19fLL, 0}, // -2
    {0x4024000000000000LL, 0x40c5829dced69992LL, 0}, // 10
    {0xC024000000000000LL, 0xc0c5829dced69992LL, 0}, // -10
    {0x408F400000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 1000
    {0xC08F400000000000LL, 0xfff0000000000000LL, FE_OVERFLOW}, // -1000
    {0x4050D9999999999ALL, 0x45f2dd7567cd83eeLL, 0}, // 67.4
    {0xC050D9999999999ALL, 0xc5f2dd7567cd83eeLL, 0}, // -67.4
    {0x409EFE6666666666LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 1983.6
    {0xC09EFE6666666666LL, 0xfff0000000000000LL, FE_OVERFLOW}, // -1983.6

    {0x4055E00000000000LL, 0x47c2d7566d26536bLL, 0}, // 87.5
    {0xC055E00000000000LL, 0xc7c2d7566d26536bLL, 0}, // -87.5
    {0x41002BC800000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 132473
    {0xC1002BC800000000LL, 0xfff0000000000000LL, FE_OVERFLOW}, // -132473
    {0x4330000000000000LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 2^52
    {0x4330000000000001LL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 2^52 + 1
    {0x432FFFFFFFFFFFFFLL, 0x7ff0000000000000LL, FE_OVERFLOW}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xfff0000000000000LL, FE_OVERFLOW}, // -2^52
    {0xC330000000000001LL, 0xfff0000000000000LL, FE_OVERFLOW}, // -(2^52 + 1)
    {0xC32FFFFFFFFFFFFFLL, 0xfff0000000000000LL, FE_OVERFLOW}, // -(2^52 -1 + 0.5)

};
