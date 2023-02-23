/* ====================================================================================================================

  The copyright in this software is being made available under the License included below.
  This software may be subject to other third party and contributor rights, including patent rights, and no such
  rights are granted under this license.

  Copyright (c) 2018, HUAWEI TECHNOLOGIES CO., LTD. All rights reserved.
  Copyright (c) 2018, SAMSUNG ELECTRONICS CO., LTD. All rights reserved.
  Copyright (c) 2018, PEKING UNIVERSITY SHENZHEN GRADUATE SCHOOL. All rights reserved.
  Copyright (c) 2018, PENGCHENG LABORATORY. All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted only for
  the purpose of developing standards within Audio and Video Coding Standard Workgroup of China (AVS) and for testing and
  promoting such standards. The following conditions are required to be met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
      the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
      the following disclaimer in the documentation and/or other materials provided with the distribution.
    * The name of HUAWEI TECHNOLOGIES CO., LTD. or SAMSUNG ELECTRONICS CO., LTD. may not be used to endorse or promote products derived from
      this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

* ====================================================================================================================
*/

#include "com_def.h"
#include <math.h>

int  g_table_t0[17]={0, 8192, 4096, 2730, 2048, 1638, 1365, 1170, 1024, 910, 819, 744, 682, 630, 585, 546, 512, };
#if TSCPM
int g_aiTscpmDivTable64[64] = {
    65536, 32768, 21845, 16384, 13107, 10922, 9362, 8192, 7281, 6553, 5957, 5461, 5041, 4681, 4369, 4096, 3855, 3640,
    3449,  3276,  3120,  2978,  2849,  2730,  2621, 2520, 2427, 2340, 2259, 2184, 2114, 2048, 1985, 1927, 1872, 1820,
    1771,  1724,  1680,  1638,  1598,  1560,  1524, 1489, 1456, 1424, 1394, 1365, 1337, 1310, 1285, 1260, 1236, 1213,
    1191,  1170,  1149,  1129,  1110,  1092,  1074, 1057, 1040, 1024,
};
#endif

#if FS_SAME_SIZE_PER_X_CTU
const int tbl_target_size_list[27][2] = {
    {  4,   4 },
    {  4,   8 },
    {  4,  16 },
    {  4,  32 },
    {  8,   4 },
    {  8,   8 },
    {  8,  16 },
    {  8,  32 },
    {  8,  64 },
    { 16,   4 },
    { 16,   8 },
    { 16,  16 },
    { 16,  32 },
    { 16,  64 },
    { 32,   4 },
    { 32,   8 },
    { 32,  16 },
    { 32,  32 },
    { 32,  64 },
    { 64,   8 },
    { 64,  16 },
    { 64,  32 },
    { 64,  64 },
    { 64, 128 },
    {128,  64 },
    {128, 128 },
    { -1,  -1 }, //end of all possible CU sizes
};
#endif

int com_tbl_subset_inter[2] = { DCT8, DST7 };
//int com_tbl_subset_inter[2] = { DCT2, DCT2 };

s8 get_com_tbl_log2(int a)
{
    s8 com_tbl_log2[257] = {
        /* 0, 1 */
        -1, -1,
            /* 2, 3 */
            1, -1,
            /* 4 ~ 7 */
            2, -1, -1, -1,
            /* 8 ~ 15 */
            3, -1, -1, -1, -1, -1, -1, -1,
            /* 16 ~ 31 */
            4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            /* 31 ~ 63 */
            5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            /* 64 ~ 127 */
            6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            /* 128 ~ 255 */
            7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            /* 256 */
            8
        };
    return com_tbl_log2[a];
}

//s8 com_tbl_wrap[6][NUM_TRANS_TYPE][64][64];
//s8 com_tbl_tm2[NUM_TRANS_TYPE][2][2];
//s8 com_tbl_tm4[NUM_TRANS_TYPE][4][4];
//s8 com_tbl_tm8[NUM_TRANS_TYPE][8][8];
//s8 com_tbl_tm16[NUM_TRANS_TYPE][16][16];
//s8 com_tbl_tm32[NUM_TRANS_TYPE][32][32];
//s8 com_tbl_tm64[NUM_TRANS_TYPE][64][64];

const s16 tab_c4_trans[4][4] = {
    { 34,    58,    72,     81, },
    { 77,    69,    -7,    -75, },
    { 79,   -33,   -75,     58, },
    { 55,   -84,    73,    -28, }
};

const s16 tab_c8_trans[4][4] = {
    { 123,   -35,    -8,    -3, },
    { -32,  -120,    30,    10, },
    { 14,    25,   123,   -22, },
    { 8,    13,    19,   126, },
};


u16 *com_scan_tbl[COEF_SCAN_TYPE_NUM][MAX_CU_LOG2][MAX_CU_LOG2];
//int com_scan_sr[MAX_TR_SIZE * MAX_TR_SIZE];



const s16 tbl_mc_l_coeff_hp[16][8] = {
    {  0, 0,   0, 64,  0,   0,  0,  0 },
    {  0, 1,  -3, 63,  4,  -2,  1,  0 },
    { -1, 2,  -5, 62,  8,  -3,  1,  0 },
    { -1, 3,  -8, 60, 13,  -4,  1,  0 },
    { -1, 4, -10, 58, 17,  -5,  1,  0 },
    { -1, 4, -11, 52, 26,  -8,  3, -1 },
    { -1, 3,  -9, 47, 31, -10,  4, -1 },
    { -1, 4, -11, 45, 34, -10,  4, -1 },
    { -1, 4, -11, 40, 40, -11,  4, -1 },
    { -1, 4, -10, 34, 45, -11,  4, -1 },
    { -1, 4, -10, 31, 47,  -9,  3, -1 },
    { -1, 3,  -8, 26, 52, -11,  4, -1 },
    {  0, 1,  -5, 17, 58, -10,  4, -1 },
    {  0, 1,  -4, 13, 60,  -8,  3, -1 },
    {  0, 1,  -3,  8, 62,  -5,  2, -1 },
    {  0, 1,  -2,  4, 63,  -3,  1,  0 }
};

const s16 tbl_mc_l_coeff_all[2][16][8] = {
    {
        { 0, 0, 0, 64, 0, 0, 0, 0 },
        { -1, 4, -10, 57, 19, -7, 3, -1 },
        { -1, 4, -11, 40, 40, -11, 4, -1 },
        { -1, 3, -7, 19, 57, -10, 4, -1 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0 }
    },
    {
        { 0, 0, 0, 64, 0, 0, 0, 0 },
        { 0, 1, -3, 63, 4, -2, 1, 0 },
        { -1, 2, -5, 62, 8, -3, 1, 0 },
        { -1, 3, -8, 60, 13, -4, 1, 0 },
        { -1, 4, -10, 58, 17, -5, 1, 0 },
        { -1, 4, -11, 52, 26, -8, 3, -1 },
        { -1, 3, -9, 47, 31, -10, 4, -1 },
        { -1, 4, -11, 45, 34, -10, 4, -1 },
        { -1, 4, -11, 40, 40, -11, 4, -1 },
        { -1, 4, -10, 34, 45, -11, 4, -1 },
        { -1, 4, -10, 31, 47, -9, 3, -1 },
        { -1, 3, -8, 26, 52, -11, 4, -1 },
        { 0, 1, -5, 17, 58, -10, 4, -1 },
        { 0, 1, -4, 13, 60, -8, 3, -1 },
        { 0, 1, -3, 8, 62, -5, 2, -1 },
        { 0, 1, -2, 4, 63, -3, 1, 0 }
    }
};

s16 get_tbl_mc_l_coeff_hp(int a, int b)
{
    s16 tbl_mc_l_coeff_hp[16][8] = {
        { 0, 0,   0, 64,  0,   0,  0,  0 },
        { 0, 1,  -3, 63,  4,  -2,  1,  0 },
        { -1, 2,  -5, 62,  8,  -3,  1,  0 },
        { -1, 3,  -8, 60, 13,  -4,  1,  0 },
        { -1, 4, -10, 58, 17,  -5,  1,  0 },
        { -1, 4, -11, 52, 26,  -8,  3, -1 },
        { -1, 3,  -9, 47, 31, -10,  4, -1 },
        { -1, 4, -11, 45, 34, -10,  4, -1 },
        { -1, 4, -11, 40, 40, -11,  4, -1 },
        { -1, 4, -10, 34, 45, -11,  4, -1 },
        { -1, 4, -10, 31, 47,  -9,  3, -1 },
        { -1, 3,  -8, 26, 52, -11,  4, -1 },
        { 0, 1,  -5, 17, 58, -10,  4, -1 },
        { 0, 1,  -4, 13, 60,  -8,  3, -1 },
        { 0, 1,  -3,  8, 62,  -5,  2, -1 },
        { 0, 1,  -2,  4, 63,  -3,  1,  0 }
    };
    return tbl_mc_l_coeff_hp[a][b];
}

const s16 tbl_mc_l_coeff[4][8] = {
    { 0,  0,  0, 64,  0,  0,  0,  0},
    { -1, 4, -10, 57, 19,  -7, 3, -1},
    { -1, 4, -11, 40, 40, -11, 4, -1},
    { -1, 3,  -7, 19, 57, -10, 4, -1}
};

s16 get_tbl_mc_l_coeff(int a, int b)
{
    s16 tbl_mc_l_coeff[4][8] = {
        { 0,  0,  0, 64,  0,  0,  0,  0 },
        { -1, 4, -10, 57, 19,  -7, 3, -1 },
        { -1, 4, -11, 40, 40, -11, 4, -1 },
        { -1, 3,  -7, 19, 57, -10, 4, -1 }
    };
    return tbl_mc_l_coeff[a][b];
}

const s16 tbl_mc_c_coeff_all[2][32][4] = {
    {
        { 0, 64, 0, 0 },
        { -4, 62, 6, 0 },
        { -6, 56, 15, -1 },
        { -5, 47, 25, -3 },
        { -4, 36, 36, -4 },
        { -3, 25, 47, -5 },
        { -1, 15, 56, -6 },
        { 0, 6, 62, -4 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
    },
    {
        { 0, 64, 0, 0 },
        { -1, 63, 2, 0 },
        { -2, 62, 4, 0 },
        { -2, 60, 7, -1 },
        { -2, 58, 10, -2 },
        { -3, 57, 12, -2 },
        { -4, 56, 14, -2 },
        { -4, 55, 15, -2 },
        { -4, 54, 16, -2 },
        { -5, 53, 18, -2 },
        { -6, 52, 20, -2 },
        { -6, 49, 24, -3 },
        { -6, 46, 28, -4 },
        { -5, 44, 29, -4 },
        { -4, 42, 30, -4 },
        { -4, 39, 33, -4 },
        { -4, 36, 36, -4 },
        { -4, 33, 39, -4 },
        { -4, 30, 42, -4 },
        { -4, 29, 44, -5 },
        { -4, 28, 46, -6 },
        { -3, 24, 49, -6 },
        { -2, 20, 52, -6 },
        { -2, 18, 53, -5 },
        { -2, 16, 54, -4 },
        { -2, 15, 55, -4 },
        { -2, 14, 56, -4 },
        { -2, 12, 57, -3 },
        { -2, 10, 58, -2 },
        { -1, 7, 60, -2 },
        { 0, 4, 62, -2 },
        { 0, 2, 63, -1 },
    }
};

const s16 tbl_mc_c_coeff_hp[32][4] = {
    { 0, 64, 0, 0 },
    { -1, 63,  2,  0 },
    { -2, 62,  4,  0 },
    { -2, 60,  7,  -1 },
    { -2, 58, 10, -2 },
    { -3, 57, 12, -2 },
    { -4, 56, 14, -2 },
    { -4, 55, 15, -2 },
    { -4, 54, 16, -2 },
    { -5, 53, 18, -2 },
    { -6, 52, 20, -2 },
    { -6, 49, 24, -3 },
    { -6, 46, 28, -4 },
    { -5, 44, 29, -4 },
    { -4, 42, 30, -4 },
    { -4, 39, 33, -4 },
    { -4, 36, 36, -4 },
    { -4, 33, 39, -4 },
    { -4, 30, 42, -4 },
    { -4, 29, 44, -5 },
    { -4, 28, 46, -6 },
    { -3, 24, 49, -6 },
    { -2, 20, 52, -6 },
    { -2, 18, 53, -5 },
    { -2, 16, 54, -4 },
    { -2, 15, 55, -4 },
    { -2, 14, 56, -4 },
    { -2, 12, 57, -3 },
    { -2, 10, 58, -2 },
    { -1,  7, 60, -2 },
    { 0,  4, 62, -2 },
    { 0,  2, 63, -1 },
};

const s16 tbl_mc_c_coeff[8][4] = {
    { 0, 64, 0, 0 },
    { -4, 62, 6, 0 },
    { -6, 56, 15, -1 },
    { -5, 47, 25, -3 },
    { -4, 36, 36, -4 },
    { -3, 25, 47, -5 },
    { -1, 15, 56, -6 },
    { 0, 6, 62, -4 }
};

const s16 com_tbl_ipred_adi[32][4] = {
    /* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
    { 32, 64, 32,  0 },
    { 31, 63, 33,  1 },
    { 30, 62, 34,  2 },
    { 29, 61, 35,  3 },
    { 28, 60, 36,  4 },
    { 27, 59, 37,  5 },
    { 26, 58, 38,  6 },
    { 25, 57, 39,  7 },
    { 24, 56, 40,  8 },
    { 23, 55, 41,  9 },
    { 22, 54, 42, 10 },
    { 21, 53, 43, 11 },
    { 20, 52, 44, 12 },
    { 19, 51, 45, 13 },
    { 18, 50, 46, 14 },
    { 17, 49, 47, 15 },
    { 16, 48, 48, 16 },
    { 15, 47, 49, 17 },
    { 14, 46, 50, 18 },
    { 13, 45, 51, 19 },
    { 12, 44, 52, 20 },
    { 11, 43, 53, 21 },
    { 10, 42, 54, 22 },
    {  9, 41, 55, 23 },
    {  8, 40, 56, 24 },
    {  7, 39, 57, 25 },
    {  6, 38, 58, 26 },
    {  5, 37, 59, 27 },
    {  4, 36, 60, 28 },
    {  3, 35, 61, 29 },
    {  2, 34, 62, 30 },
    {  1, 33, 63, 31 },
};

const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
{
    { 0, 0 },
    { 0, 0 }, { 0, 0 }, { 2816, 372 }, { 2048, 512 }, { 1408, 744 },
    { 1024, 1024 }, { 744, 1408 }, { 512, 2048 }, { 372, 2816 }, { 256, 4096 },
    { 128, 8192 }, { 0, 0 }, { 128, 8192 }, { 256, 4096 }, { 372, 2816 },
    { 512, 2048 }, { 744, 1408 }, { 1024, 1024 }, { 1408, 744 }, { 2048, 512 },
    { 2816, 372 }, { 4096, 256 }, { 8192, 128 }, { 0, 0 }, { 8192, 128 },
    { 4096, 256 }, { 2816, 372 }, { 2048, 512 }, { 1408, 744 }, { 1024, 1024 },
    { 744, 1408 }, { 512, 2048 },
};

const u8 com_tbl_df_st[4][52] = {
    /* intra */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 8, 9, 10, 11, 12, 12, 12, 12, 12,
    },
    /* non-zero coefficient(s) for luma */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 7, 8,  9, 10, 11, 11, 11, 11, 11,
    },
    /* no non-zero coefficient & mvd >= 4 */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 4, 4, 5, 6, 7,  8,  9, 10, 10, 10, 10, 10,
    },
    /* no deblock */
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,
    },
};

u16 com_split_tbl[6][SPLIT_CHECK_NUM][2];


int saoclip[NUM_SAO_OFFSET][3] = {
    //EO
    { -1, 6, 7 }, //low bound, upper bound, threshold
    { 0, 1, 1 },
    { 0, 0, 0 },
    { -1, 0, 1 },
    { -6, 1, 7 },
    { -7, 7, 7 } //BO
};

int EO_OFFSET_MAP[8] = { 3, 1, 0, 2, 4, 5, 6, 7 };
int EO_OFFSET_MAP_DULPI1[8] = { 3, 1, 0, 2, 4, 5, 6, 7 };
int EO_OFFSET_MAP_DULPI2[8] = { 3, 1, 0, 2, 4, 5, 6, 7 };
int EO_OFFSET_MAP_DULPI3[8] = { 3, 1, 0, 2, 4, 5, 6, 7 };
int deltaband_cost[17] = { -1, -1, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 16 };


/*************************init transform matrix ***********************************/
/*void init_dct_coef()
{
    int i, c = 2;
    int c_wrap = 64;
    const double PI = 3.14159265358979323846;

    memset(com_tbl_wrap, 0, sizeof(com_tbl_wrap));
    for (i = 0; i < 6; i++) {
        s8 *iT = NULL;
        const double s = sqrt((double)c) * 32;
        s8 *iT_wrap = com_tbl_wrap[i][0][0];
        switch (i) {
            case 0:
                iT = com_tbl_tm2[0][0];
                break;
            case 1:
                iT = com_tbl_tm4[0][0];
                break;
            case 2:
                iT = com_tbl_tm8[0][0];
                break;
            case 3:
                iT = com_tbl_tm16[0][0];
                break;
            case 4:
                iT = com_tbl_tm32[0][0];
                break;
            case 5:
                iT = com_tbl_tm64[0][0];
                break;
            default:
                exit(0);
                break;
        }

        for (int k = 0; k < c; k++) {
            for (int n = 0; n < c; n++) {
                double w0, v;

                // DCT-II
                w0 = k == 0 ? sqrt(0.5) : 1;
                v = cos(PI * (n + 0.5) * k / c) * w0 * sqrt(2.0 / c);
                iT[DCT2 * c * c + k * c + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));
                iT_wrap[DCT2 * c_wrap * c_wrap + k * c_wrap + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));

                // DCT-VIII
                v = cos(PI * (k + 0.5) * (n + 0.5) / (c + 0.5)) * sqrt(2.0 / (c + 0.5));
                iT[DCT8 * c * c + k * c + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));
                iT_wrap[DCT8 * c_wrap * c_wrap + k * c_wrap + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));

                // DST-VII
                v = sin(PI * (k + 0.5) * (n + 1) / (c + 0.5)) * sqrt(2.0 / (c + 0.5));
                iT[DST7 * c * c + k * c + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));
                iT_wrap[DST7 * c_wrap * c_wrap + k * c_wrap + n] = (s8)(s * v + (v > 0 ? 0.5 : -0.5));

            }
        }
        c <<= 1;
    }
	
#if PRINT_TRANSFORM_TABLE // PRINT transform matrix
    for ( int trType = 0; trType < 3; trType++ ) {
        printf( "\n Transform %s", trType == 0 ? "DCT2" : (trType == 1 ? "DST7" : "DCT8") );
        int sizeTH = 16;
        int minSizeTrType = (trType == 0) ?  2 :  4;
        int maxSizeTrType = (trType == 0) ? 64 : 16;
        for ( int c = 2; c <= maxSizeTrType; c *= 2 ) {
            int partN = c > sizeTH ? c / sizeTH : 1;
            int s = c > sizeTH ? sizeTH : c;
            s8 *iT = NULL;

            switch ( c ) {
                case 2:
                    iT = &com_tbl_tm2[trType][0][0];
                    break;
                case 4:
                    iT = &com_tbl_tm4[trType][0][0];
                    break;
                case 8:
                    iT = &com_tbl_tm8[trType][0][0];
                    break;
                case 16:
                    iT = &com_tbl_tm16[trType][0][0];
                    break;
                case 32:
                    iT = &com_tbl_tm32[trType][0][0];
                    break;
                case 64:
                    iT = &com_tbl_tm64[trType][0][0];
                    break;
                default:
                    exit( 0 );
                    break;
            }

            for ( int yPart = 0; yPart < partN; yPart++ ) {
                for ( int xPart = 0; xPart < partN; xPart++ ) {
                    printf( "\n Size %d partX %d partY %d", c, xPart, yPart );
                    for ( int y = 0; y < s; y++ ) {
                        printf( "\n " );
                        for ( int x = 0; x < s; x++ ) {
                            printf( "%3d  ", *(iT + (y + yPart * sizeTH)*c + x + xPart * sizeTH) );
                        }
                    }
                }
            }
        }

    }
    printf( "\nFinish printf matrices, and terminate" );
    exit( 0 );
#endif
}*/

const int tab_wq_param_default[2][6] = {
    { 67, 71, 71, 80, 80, 106 },
    { 64, 49, 53, 58, 58,  64 }
};

const u8 tab_WqMDefault4x4[16] = {
    64, 64, 64, 68,
    64, 64, 68, 72,
    64, 68, 76, 80,
    72, 76, 84, 96
};

const u8 tab_WqMDefault8x8[64] = {
    64,  64,  64,  64,  68,  68,  72,  76,
    64,  64,  64,  68,  72,  76,  84,  92,
    64,  64,  68,  72,  76,  80,  88, 100,
    64,  68,  72,  80,  84,  92, 100, 112,
    68,  72,  80,  84,  92, 104, 112, 128,
    76,  80,  84,  92, 104, 116, 132, 152,
    96, 100, 104, 116, 124, 140, 164, 188,
    104, 108, 116, 128, 152, 172, 192, 216
};

const int tab_WeightQuantModel[4][64] = {
    //   l a b c d h
    //   0 1 2 3 4 5
    {
        // Mode 0
        0, 0, 0, 4, 4, 4, 5, 5,
        0, 0, 3, 3, 3, 3, 5, 5,
        0, 3, 2, 2, 1, 1, 5, 5,
        4, 3, 2, 2, 1, 5, 5, 5,
        4, 3, 1, 1, 5, 5, 5, 5,
        4, 3, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }, {
        // Mode 1
        0, 0, 0, 4, 4, 4, 5, 5,
        0, 0, 4, 4, 4, 4, 5, 5,
        0, 3, 2, 2, 2, 1, 5, 5,
        3, 3, 2, 2, 1, 5, 5, 5,
        3, 3, 2, 1, 5, 5, 5, 5,
        3, 3, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }, {
        // Mode 2
        0, 0, 0, 4, 4, 3, 5, 5,
        0, 0, 4, 4, 3, 2, 5, 5,
        0, 4, 4, 3, 2, 1, 5, 5,
        4, 4, 3, 2, 1, 5, 5, 5,
        4, 3, 2, 1, 5, 5, 5, 5,
        3, 2, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }, {
        // Mode 3
        0, 0, 0, 3, 2, 1, 5, 5,
        0, 0, 4, 3, 2, 1, 5, 5,
        0, 4, 4, 3, 2, 1, 5, 5,
        3, 3, 3, 3, 2, 5, 5, 5,
        2, 2, 2, 2, 5, 5, 5, 5,
        1, 1, 1, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5
    }
};

const int tab_WeightQuantModel4x4[4][16] = {
    //   l a b c d h
    //   0 1 2 3 4 5
    {
        // Mode 0
        0, 4, 3, 5,
        4, 2, 1, 5,
        3, 1, 1, 5,
        5, 5, 5, 5
    }, {
        // Mode 1
        0, 4, 4, 5,
        3, 2, 2, 5,
        3, 2, 1, 5,
        5, 5, 5, 5
    }, {

        // Mode 2
        0, 4, 3, 5,
        4, 3, 2, 5,
        3, 2, 1, 5,
        5, 5, 5, 5
    }, {
        // Mode 3
        0, 3, 1, 5,
        3, 4, 2, 5,
        1, 2, 2, 5,
        5, 5, 5, 5
    }
};
#if CABAC_MULTI_PROB
u16 mCabac_ws;
u16 mCabac_offset;
u16 cwr2LGS[10] = { 427, 427, 427, 197, 95, 46, 23, 12, 6, 3 };
u8 g_compatible_back;
u8 counter_thr1;
u8 counter_thr2;
#endif
void set_pic_wq_matrix_by_param(int *param_vector, int mode, u8 *pic_wq_matrix4x4, u8 *pic_wq_matrix8x8)
{
    int i;
    for (i = 0; i < 16; i++) {
        pic_wq_matrix4x4[i] = param_vector[tab_WeightQuantModel4x4[mode][i]];
    }
    for (i = 0; i < 64; i++) {
        pic_wq_matrix8x8[i] = param_vector[tab_WeightQuantModel[mode][i]];
    }
}

void init_pic_wq_matrix(u8 *pic_wq_matrix4x4, u8 *pic_wq_matrix8x8)
{
    int i;
    for (i = 0; i < 16; i++) {
        pic_wq_matrix4x4[i] = 64;
    }
    for (i = 0; i < 64; i++) {
        pic_wq_matrix8x8[i] = 64;
    }
}
