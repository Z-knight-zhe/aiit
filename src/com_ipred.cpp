/**************************************************************************************
 * Copyright (c) 2018-2020 ["Peking University Shenzhen Graduate School",
 *   "Peng Cheng Laboratory", and "Guangdong Bohua UHD Innovation Corporation"]
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes the software uAVS3d developed by
 *    Peking University Shenzhen Graduate School, Peng Cheng Laboratory
 *    and Guangdong Bohua UHD Innovation Corporation.
 * 4. Neither the name of the organizations (Peking University Shenzhen Graduate School,
 *    Peng Cheng Laboratory and Guangdong Bohua UHD Innovation Corporation) nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information, contact us at rgwang@pkusz.edu.cn.
 **************************************************************************************/

#include "com_def.h"
#include "com_define.h"

static signed  char com_tbl_log2[257] = {
    /* 0, 1 */
    -1, 0,
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
#define CONV_LOG2(v)                   (com_tbl_log2[v])
extern funs_handle_t avs3_funs_handle;

void eipm_part_cal(int filter_idx, int offset, pel src_pn_n1, pel src_p, pel src_pn, pel src_pn_p2, pel* tmp1, int bit_depth) {
#pragma HLS INLINE  
    static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
    {
        {0, 0},{0, 0},{0, 0},                                 //0~2
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
        {744, 1408},{512, 2048},                              //7~8
        {372, 2816},                                          //9
        {256, 4096},                                          //10
        {128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
        {372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
        {1408, 744},{2048, 512},{2816, 372},                  //19~21
        {4096, 256},                                          //22
        {8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
        {744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
        {1640, 640},                                          //35
        {1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
        {320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
        {32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
        {448, 2340},                                          //48
        {640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
        {2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
        {32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
        {3276, 320},{2340, 448},                              //61~62
        {1640, 640},{1168, 898},{898, 1168}                   //63~65
    };
#if MIPF

    static const s16 com_tbl_ipred_adi_bilinear[32][4] =
    {
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
        { 9, 41, 55, 23 },
        { 8, 40, 56, 24 },
        { 7, 39, 57, 25 },
        { 6, 38, 58, 26 },
        { 5, 37, 59, 27 },
        { 4, 36, 60, 28 },
        { 3, 35, 61, 29 },
        { 2, 34, 62, 30 },
        { 1, 33, 63, 31 },
    };

    static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
    {
        /* best for CfP extra smooth */
        { 23, 82, 21, 2 },
        { 21, 82, 23, 2 },
        { 19, 82, 25, 2 },
        { 18, 81, 27, 2 },
        { 16, 80, 30, 2 },
        { 15, 79, 32, 2 },
        { 13, 78, 35, 2 },
        { 12, 77, 37, 2 },
        { 11, 75, 40, 2 },
        { 10, 74, 42, 2 },
        { 9, 72, 45, 2 },
        { 8, 70, 47, 3 },
        { 7, 68, 50, 3 },
        { 6, 67, 52, 3 },
        { 6, 64, 54, 4 },
        { 5, 62, 57, 4 },
        { 4, 60, 60, 4 },
        { 4, 57, 62, 5 },
        { 4, 54, 64, 6 },
        { 3, 52, 67, 6 },
        { 3, 50, 68, 7 },
        { 3, 47, 70, 8 },
        { 2, 45, 72, 9 },
        { 2, 42, 74, 10 },
        { 2, 40, 75, 11 },
        { 2, 37, 77, 12 },
        { 2, 35, 78, 13 },
        { 2, 32, 79, 15 },
        { 2, 30, 80, 16 },
        { 2, 27, 81, 18 },
        { 2, 25, 82, 19 },
        { 2, 23, 82, 21 },
    };

    static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
    {
        // semi-sharp
        { 11, 106, 12, -1 },
        { 9, 105, 15, -1 },
        { 7, 105, 18, -2 },
        { 6, 103, 21, -2 },
        { 4, 102, 25, -3 },
        { 2, 100, 29, -3 },
        { 1, 99, 32, -4 },
        { 0, 96, 36, -4 },
        { -1, 94, 39, -4 },
        { -2, 92, 43, -5 },
        { -3, 89, 47, -5 },
        { -4, 86, 51, -5 },
        { -4, 83, 55, -6 },
        { -5, 80, 59, -6 },
        { -5, 77, 62, -6 },
        { -6, 74, 66, -6 },
        { -6, 70, 70, -6 },
        { -6, 66, 74, -6 },
        { -6, 62, 77, -5 },
        { -6, 59, 80, -5 },
        { -6, 55, 83, -4 },
        { -5, 51, 86, -4 },
        { -5, 47, 89, -3 },
        { -5, 43, 92, -2 },
        { -4, 39, 94, -1 },
        { -4, 36, 96, 0 },
        { -4, 32, 99, 1 },
        { -3, 29, 100, 2 },
        { -3, 25, 102, 4 },
        { -2, 21, 103, 6 },
        { -2, 18, 105, 7 },
        { -1, 15, 105, 9 },
    };

    static const s16 tbl_mc_c_coeff_hp[32][4] =
    {
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
#else
    static const s16 com_tbl_ipred_adi[32][4] = { { (32), (64), (32), 0 }, { (31), (63), (33), (1) }, { (30), (62), (34), (2) }, { (29), (61), (35), (3) }, { (28), (60), (36), (4) }, { (27), (59), (37), (5) }, { (26), (58), (38), (6) }, { (25), (57), (39), (7) }, { (24), (56), (40), (8) }, { (23), (55), (41), (9) }, { (22), (54), (42), (10) }, { (21), (53), (43), (11) }, { (20), (52), (44), (12) }, { (19), (51), (45), (13) }, { (18), (50), (46), (14) }, { (17), (49), (47), (15) }, { (16), (48), (48), (16) }, { (15), (47), (49), (17) }, { (14), (46), (50), (18) }, { (13), (45), (51), (19) }, { (12), (44), (52), (20) }, { (11), (43), (53), (21) }, { (10), (42), (54), (22) }, { (9), (41), (55), (23) }, { (8), (40), (56), (24) }, { (7), (39), (57), (25) }, { (6), (38), (58), (26) }, { (5), (37), (59), (27) }, { (4), (36), (60), (28) }, { (3), (35), (61), (29) }, { (2), (34), (62), (30) }, { (1), (33), (63), (31) } };
#endif
    const int filter_bits_list[4] = { 7, 7, 6, 7 };
    const int filter_offset_list[4] = { 64, 64 ,32, 64 };

    S16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
    S16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
    S16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
    S16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
    S16 data5 = filter_offset_list[filter_idx];
    S16 data6 = filter_bits_list[filter_idx];
    S32 buffer1 = src_pn_n1 * data1;
    S32 buffer2 = src_p * data2;
    S32 buffer3 = src_pn * data3;
    S32 buffer4 = src_pn_p2 * data4;
    pel tmp = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
    *tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
}

void ipred_ang_eipm_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], int w, int h, int ipm
#if MIPF
    , int is_luma, int mipf_enable_flag, U4 bit_depth
#endif
)
{
#pragma HLS INLINE 
    pel tmp, tmp1;
    static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
    {
        {0, 0},{0, 0},{0, 0},                                 //0~2
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
        {744, 1408},{512, 2048},                              //7~8
        {372, 2816},                                          //9
        {256, 4096},                                          //10
        {128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
        {372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
        {1408, 744},{2048, 512},{2816, 372},                  //19~21
        {4096, 256},                                          //22
        {8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
        {744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
        {1640, 640},                                          //35
        {1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
        {320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
        {32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
        {448, 2340},                                          //48
        {640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
        {2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
        {32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
        {3276, 320},{2340, 448},                              //61~62
        {1640, 640},{1168, 898},{898, 1168}                   //63~65
    };
#if MIPF

    const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
    const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
    int filter_idx;
#else
    const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
    const int filter_offset = ADI_4T_FILTER_OFFSET;
    const int filter_bits = ADI_4T_FILTER_BITS;
#endif

    const int* mt = com_tbl_ipred_dxdy[ipm];

    //const pel* src_ch = NULL;
    //const s16* filter;

    int offset_x[MAX_CU_SIZE], offset_y[MAX_CU_SIZE];
    int t_dx[MAX_CU_SIZE], t_dy[MAX_CU_SIZE];
    int i, j;
    int offset = 0;
    int pos_max = w + h - 1;
    int p, pn, pn_n1, pn_p2;
    int src_p, src_pn, src_pn_n1, src_pn_p2;
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
#pragma HLS ARRAY_PARTITION variable=t_dx complete dim=1
#pragma HLS ARRAY_PARTITION variable=t_dy complete dim=1
#pragma HLS ARRAY_PARTITION variable=offset_x complete dim=1
#pragma HLS ARRAY_PARTITION variable=offset_y complete dim=1

    if (((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
    {
        if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
            pos_max = w * 2 - 1;
        else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
            pos_max = h * 2 - 1;

        for (j = 0; j < 4; j++)
        {
#pragma HLS PIPELINE
            int dx;
            for (i = 0; i < 4; i++)
            {
#pragma HLS UNROLL
                int x;
             //   int src_p, src_pn, src_pn_n1, src_pn_p2;

                if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) {
                    GET_REF_POS(mt[0], j + 1, dx, offset);
                    filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;

                    x = i + dx;

                    pn_n1 = x - 1;
                    p = x;
                    pn = x + 1;
                    pn_p2 = x + 2;

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

                    src_pn_n1 = src_up[pn_n1];
                    src_p = src_up[p];
                    src_pn = src_up[pn];
                    src_pn_p2 = src_up[pn_p2];
                }
                else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
                {
                    GET_REF_POS(mt[1], i + 1, dx, offset);
                    filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

                    x = j + dx;

                    pn_n1 = x - 1;
                    p = x;
                    pn = x + 1;
                    pn_p2 = x + 2;

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

                    src_pn_n1 = src_le[pn_n1];
                    src_p = src_le[p];
                    src_pn = src_le[pn];
                    src_pn_p2 = src_le[pn_p2];

                }

          
                eipm_part_cal(filter_idx, offset, src_pn_n1, src_p, src_pn, src_pn_p2, &tmp1, bit_depth);
                //tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                dst[j * 4 + i] = tmp1;

            }
          //  dst += w;
        }
    }
    else
    {
#pragma HLS PIPELINE
        for (i = 0; i < 4; i++)
        {
#pragma HLS UNROLL
            GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
            t_dy[i] = -t_dy[i];
        }
        for (j = 0; j < 4; j++)
        {
#pragma HLS UNROLL
            GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
            t_dx[j] = -t_dx[j];
        }

        {
            for (j = 0; j < 4; j++)
            {
#pragma HLS UNROLL
                for (i = 0; i < 4; i++)
                {
#pragma HLS UNROLL
                    int x = i + t_dx[j];
                    int y = j + t_dy[i];
                    int src_p, src_pn, src_pn_n1, src_pn_p2;

                    if (y <= -1)
                    {
                        offset = offset_x[j];
                        pos_max = w * 2 - 1;

                        pn_n1 = x + 1;
                        p = x;
                        pn = x - 1;
                        pn_p2 = x - 2;

                        pn_n1 = COM_MIN(pn_n1, pos_max);
                        p = COM_MIN(p, pos_max);
                        pn = COM_MIN(pn, pos_max);
                        pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1 = src_up[pn_n1];
                        src_p = src_up[p];
                        src_pn = src_up[pn];
                        src_pn_p2 = src_up[pn_p2];

                    }
                    else
                    {
                        offset = offset_y[i];
                        pos_max = h * 2 - 1;

                        pn_n1 = y + 1;
                        p = y;
                        pn = y - 1;
                        pn_p2 = y - 2;

                        pn_n1 = COM_MIN(pn_n1, pos_max);
                        p = COM_MIN(p, pos_max);
                        pn = COM_MIN(pn, pos_max);
                        pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1 = src_le[pn_n1];
                        src_p = src_le[p];
                        src_pn = src_le[pn];
                        src_pn_p2 = src_le[pn_p2];

                    }

                    if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT)) {
                        filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
                    }
                    else
                    {
                        filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
                    }
                   
                    eipm_part_cal(filter_idx, offset, src_pn_n1, src_p, src_pn, src_pn_p2, &tmp1, bit_depth);
                    dst[j * 4+ i] = tmp1;

                }
             //   dst += w;
            }
        }
    }
}

void ipred_ang_eipm_64(pel src_le[131], pel src_up[131], int w, int h, int ipm
#if MIPF
    , int is_luma, int mipf_enable_flag, U4 bit_depth, s16 diff[64][64], pel pred_for_diff[64][64], pel* orgY
#endif
)
{
    pel tmp, tmp1;
    static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
    {
        {0, 0},{0, 0},{0, 0},                                 //0~2
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
        {744, 1408},{512, 2048},                              //7~8
        {372, 2816},                                          //9
        {256, 4096},                                          //10
        {128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
        {372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
        {1408, 744},{2048, 512},{2816, 372},                  //19~21
        {4096, 256},                                          //22
        {8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
        {744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
        {1640, 640},                                          //35
        {1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
        {320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
        {32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
        {448, 2340},                                          //48
        {640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
        {2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
        {32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
        {3276, 320},{2340, 448},                              //61~62
        {1640, 640},{1168, 898},{898, 1168}                   //63~65
    };
#if MIPF

    static const s16 com_tbl_ipred_adi_bilinear[32][4] =
    {
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
        { 9, 41, 55, 23 },
        { 8, 40, 56, 24 },
        { 7, 39, 57, 25 },
        { 6, 38, 58, 26 },
        { 5, 37, 59, 27 },
        { 4, 36, 60, 28 },
        { 3, 35, 61, 29 },
        { 2, 34, 62, 30 },
        { 1, 33, 63, 31 },
    };

    static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
    {
        /* best for CfP extra smooth */
        { 23, 82, 21, 2 },
        { 21, 82, 23, 2 },
        { 19, 82, 25, 2 },
        { 18, 81, 27, 2 },
        { 16, 80, 30, 2 },
        { 15, 79, 32, 2 },
        { 13, 78, 35, 2 },
        { 12, 77, 37, 2 },
        { 11, 75, 40, 2 },
        { 10, 74, 42, 2 },
        { 9, 72, 45, 2 },
        { 8, 70, 47, 3 },
        { 7, 68, 50, 3 },
        { 6, 67, 52, 3 },
        { 6, 64, 54, 4 },
        { 5, 62, 57, 4 },
        { 4, 60, 60, 4 },
        { 4, 57, 62, 5 },
        { 4, 54, 64, 6 },
        { 3, 52, 67, 6 },
        { 3, 50, 68, 7 },
        { 3, 47, 70, 8 },
        { 2, 45, 72, 9 },
        { 2, 42, 74, 10 },
        { 2, 40, 75, 11 },
        { 2, 37, 77, 12 },
        { 2, 35, 78, 13 },
        { 2, 32, 79, 15 },
        { 2, 30, 80, 16 },
        { 2, 27, 81, 18 },
        { 2, 25, 82, 19 },
        { 2, 23, 82, 21 },
    };

    static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
    {
        // semi-sharp
        { 11, 106, 12, -1 },
        { 9, 105, 15, -1 },
        { 7, 105, 18, -2 },
        { 6, 103, 21, -2 },
        { 4, 102, 25, -3 },
        { 2, 100, 29, -3 },
        { 1, 99, 32, -4 },
        { 0, 96, 36, -4 },
        { -1, 94, 39, -4 },
        { -2, 92, 43, -5 },
        { -3, 89, 47, -5 },
        { -4, 86, 51, -5 },
        { -4, 83, 55, -6 },
        { -5, 80, 59, -6 },
        { -5, 77, 62, -6 },
        { -6, 74, 66, -6 },
        { -6, 70, 70, -6 },
        { -6, 66, 74, -6 },
        { -6, 62, 77, -5 },
        { -6, 59, 80, -5 },
        { -6, 55, 83, -4 },
        { -5, 51, 86, -4 },
        { -5, 47, 89, -3 },
        { -5, 43, 92, -2 },
        { -4, 39, 94, -1 },
        { -4, 36, 96, 0 },
        { -4, 32, 99, 1 },
        { -3, 29, 100, 2 },
        { -3, 25, 102, 4 },
        { -2, 21, 103, 6 },
        { -2, 18, 105, 7 },
        { -1, 15, 105, 9 },
    };

    static const s16 tbl_mc_c_coeff_hp[32][4] =
    {
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
#else
    static const s16 com_tbl_ipred_adi[32][4] = { { (32), (64), (32), 0 }, { (31), (63), (33), (1) }, { (30), (62), (34), (2) }, { (29), (61), (35), (3) }, { (28), (60), (36), (4) }, { (27), (59), (37), (5) }, { (26), (58), (38), (6) }, { (25), (57), (39), (7) }, { (24), (56), (40), (8) }, { (23), (55), (41), (9) }, { (22), (54), (42), (10) }, { (21), (53), (43), (11) }, { (20), (52), (44), (12) }, { (19), (51), (45), (13) }, { (18), (50), (46), (14) }, { (17), (49), (47), (15) }, { (16), (48), (48), (16) }, { (15), (47), (49), (17) }, { (14), (46), (50), (18) }, { (13), (45), (51), (19) }, { (12), (44), (52), (20) }, { (11), (43), (53), (21) }, { (10), (42), (54), (22) }, { (9), (41), (55), (23) }, { (8), (40), (56), (24) }, { (7), (39), (57), (25) }, { (6), (38), (58), (26) }, { (5), (37), (59), (27) }, { (4), (36), (60), (28) }, { (3), (35), (61), (29) }, { (2), (34), (62), (30) }, { (1), (33), (63), (31) } };
#endif

#if MIPF
    //const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
    const int filter_bits_list[4] = { 7, 7, 6, 7 };
    const int filter_offset_list[4] = { 64, 64 ,32, 64 };
    const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
    const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
    int filter_idx;
#else
    const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
    const int filter_offset = ADI_4T_FILTER_OFFSET;
    const int filter_bits = ADI_4T_FILTER_BITS;
#endif

    const int* mt = com_tbl_ipred_dxdy[ipm];

    //const pel* src_ch = NULL;
    //const s16* filter;

    int offset_x[MAX_CU_SIZE], offset_y[MAX_CU_SIZE];
    int t_dx[MAX_CU_SIZE], t_dy[MAX_CU_SIZE];
    int i, j;
    int offset;
    int pos_max = w + h - 1;
    int p, pn, pn_n1, pn_p2;

#if EIPM
    if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
#else
    if (ipm < IPD_VER)
#endif
    {
        //src_ch = src_up;
        pos_max = w * 2 - 1;

        for (j = 0; j < h; j++)
        {
            int dx;
            GET_REF_POS(mt[0], j + 1, dx, offset);
#if MIPF
            filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
            //filter = (tbl_filt_list[filter_idx] + offset)[0];
#else
            filter = (tbl_filt + offset)[0];
#endif
            for (i = 0; i < w; i++)
            {
                int x = i + dx;
                pn_n1 = x - 1;
                p = x;
                pn = x + 1;
                pn_p2 = x + 2;

                pn_n1 = COM_MIN(pn_n1, pos_max);
                p = COM_MIN(p, pos_max);
                pn = COM_MIN(pn, pos_max);
                pn_p2 = COM_MIN(pn_p2, pos_max);

#if MIPF
                /*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                    src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                    filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/

                if (filter_idx == 0)
                    tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
                        src_up[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 1)
                    tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
                        src_up[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 2)
                    tmp = (pel)((src_up[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_up[p] * tbl_mc_c_coeff_hp[offset][1] +
                        src_up[pn] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 3)
                    tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p] * com_tbl_ipred_adi_bilinear[offset][1] +
                        src_up[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                diff[j][i] = orgY[(j * 64) + i] - tmp1;
                pred_for_diff[j][i] = tmp1;
#else
                dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                    src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                    filter_offset) >> filter_bits);
#endif
            }
            //dst += w;
        }
    }
#if EIPM
    else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
#else
    else if (ipm > IPD_HOR)
#endif
    {
        //src_ch = src_le;
        pos_max = h * 2 - 1;

        for (i = 0; i < w; i++)
        {
            GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
        }

        for (j = 0; j < h; j++)
        {
            for (i = 0; i < w; i++)
            {
                int y = j + t_dy[i];
                pn_n1 = y - 1;
                p = y;
                pn = y + 1;
                pn_p2 = y + 2;

#if  MIPF
                filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
                //filter = (tbl_filt_list[filter_idx] + offset_y[i])[0];
#else
                filter = (tbl_filt + offset_y[i])[0];
#endif

                pn_n1 = COM_MIN(pn_n1, pos_max);
                p = COM_MIN(p, pos_max);
                pn = COM_MIN(pn, pos_max);
                pn_p2 = COM_MIN(pn_p2, pos_max);

#if MIPF

                /*  dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                      src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                      filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/

                int offset_y_new = offset_y[i];
                if (filter_idx == 0)
                    tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset_y_new][0] + src_le[p] * com_tbl_ipred_adi_extra_smooth[offset_y_new][1] +
                        src_le[pn] * com_tbl_ipred_adi_extra_smooth[offset_y_new][2] + src_le[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset_y_new][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 1)
                    tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset_y_new][0] + src_le[p] * com_tbl_ipred_adi_semi_sharp[offset_y_new][1] +
                        src_le[pn] * com_tbl_ipred_adi_semi_sharp[offset_y_new][2] + src_le[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset_y_new][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 2)
                    tmp = (pel)((src_le[pn_n1] * tbl_mc_c_coeff_hp[offset_y_new][0] + src_le[p] * tbl_mc_c_coeff_hp[offset_y_new][1] +
                        src_le[pn] * tbl_mc_c_coeff_hp[offset_y_new][2] + src_le[pn_p2] * tbl_mc_c_coeff_hp[offset_y_new][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 3)
                    tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_bilinear[offset_y_new][0] + src_le[p] * com_tbl_ipred_adi_bilinear[offset_y_new][1] +
                        src_le[pn] * com_tbl_ipred_adi_bilinear[offset_y_new][2] + src_le[pn_p2] * com_tbl_ipred_adi_bilinear[offset_y_new][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                diff[j][i] = orgY[(j * 64) + i] - tmp1;
                pred_for_diff[j][i] = tmp1;

#else
                dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                    src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                    filter_offset) >> filter_bits);
#endif
            }
            //dst += w;
        }
    }
    else
    {
        for (i = 0; i < w; i++)
        {
            GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
            t_dy[i] = -t_dy[i];
        }
        for (j = 0; j < h; j++)
        {
            GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
            t_dx[j] = -t_dx[j];
        }
#if MIPF
#if EIPM
        if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT))
#else
        if (ipm < IPD_DIA_R)
#endif
        {
#endif
            for (j = 0; j < h; j++)
            {
#if  MIPF
                filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
#endif
                for (i = 0; i < w; i++)
                {
                    int x = i + t_dx[j];
                    int y = j + t_dy[i];

                    if (y <= -1)
                    {
                        //src_ch = src_up;
                        offset = offset_x[j];
                        pos_max = w * 2 - 1;

                        pn_n1 = x + 1;
                        p = x;
                        pn = x - 1;
                        pn_p2 = x - 2;
                    }
                    else
                    {
                        //src_ch = src_le;
                        offset = offset_y[i];
                        pos_max = h * 2 - 1;

                        pn_n1 = y + 1;
                        p = y;
                        pn = y - 1;
                        pn_p2 = y - 2;
                    }

#if  MIPF
                    //filter = (tbl_filt_list[filter_idx] + offset)[0];
#else
                    filter = (tbl_filt + offset)[0];
#endif

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

#if MIPF

                    /*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                        src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/
                    if (y <= -1) {
                        if (filter_idx == 0)
                            tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
                                src_up[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 1)
                            tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
                                src_up[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 2)
                            tmp = (pel)((src_up[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_up[p] * tbl_mc_c_coeff_hp[offset][1] +
                                src_up[pn] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 3)
                            tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p] * com_tbl_ipred_adi_bilinear[offset][1] +
                                src_up[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    else {
                        if (filter_idx == 0)
                            tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_le[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
                                src_le[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_le[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 1)
                            tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_le[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
                                src_le[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_le[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 2)
                            tmp = (pel)((src_le[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_le[p] * tbl_mc_c_coeff_hp[offset][1] +
                                src_le[pn] * tbl_mc_c_coeff_hp[offset][2] + src_le[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 3)
                            tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_le[p] * com_tbl_ipred_adi_bilinear[offset][1] +
                                src_le[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_le[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                    diff[j][i] = orgY[(j * 64) + i] - tmp1;
                    pred_for_diff[j][i] = tmp1;
#else
                    dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                        src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                        filter_offset) >> filter_bits);
#endif
                }
                //dst += w;	com_assert(NULL != dst);
            }
#if MIPF
        }
        else
        {
            for (j = 0; j < h; j++)
            {
                for (i = 0; i < w; i++)
                {
                    int x = i + t_dx[j];
                    int y = j + t_dy[i];

                    if (y <= -1)
                    {
                        //src_ch = src_up;
                        offset = offset_x[j];
                        pos_max = w * 2 - 1;

                        pn_n1 = x + 1;
                        p = x;
                        pn = x - 1;
                        pn_p2 = x - 2;
                    }
                    else
                    {
                        //src_ch = src_le;
                        offset = offset_y[i];
                        pos_max = h * 2 - 1;

                        pn_n1 = y + 1;
                        p = y;
                        pn = y - 1;
                        pn_p2 = y - 2;
                    }

#if  MIPF
                    filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

                    //filter = (tbl_filt_list[filter_idx] + offset)[0];

#else
                    filter = (tbl_filt + offset)[0];
#endif

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);
#if MIPF

                    /*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                        src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/
                    if (y <= -1) {
                        if (filter_idx == 0)
                            tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
                                src_up[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 1)
                            tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
                                src_up[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 2)
                            tmp = (pel)((src_up[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_up[p] * tbl_mc_c_coeff_hp[offset][1] +
                                src_up[pn] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 3)
                            tmp = (pel)((src_up[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p] * com_tbl_ipred_adi_bilinear[offset][1] +
                                src_up[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    else {
                        if (filter_idx == 0)
                            tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_le[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
                                src_le[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_le[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 1)
                            tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_le[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
                                src_le[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_le[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 2)
                            tmp = (pel)((src_le[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_le[p] * tbl_mc_c_coeff_hp[offset][1] +
                                src_le[pn] * tbl_mc_c_coeff_hp[offset][2] + src_le[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                        else if (filter_idx == 3)
                            tmp = (pel)((src_le[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_le[p] * com_tbl_ipred_adi_bilinear[offset][1] +
                                src_le[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_le[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
                                filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                    diff[j][i] = orgY[(j * 64) + i] - tmp1;
                    pred_for_diff[j][i] = tmp1;
#else
                    dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
                        src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
                        filter_offset) >> filter_bits);
#endif
                }
                //dst += w;
            }
        }
#endif
    }
}

void ipred_ang_eipm_32(pel src_le[67], pel src_up[67], int w, int h, int ipm
#if MIPF
    , int is_luma, int mipf_enable_flag, U4 bit_depth, s16 diff[32][32]
#endif
)
{
    pel tmp, tmp1;
    static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
    {
        {0, 0},{0, 0},{0, 0},                                 //0~2
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
        {744, 1408},{512, 2048},                              //7~8
        {372, 2816},                                          //9
        {256, 4096},                                          //10
        {128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
        {372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
        {1408, 744},{2048, 512},{2816, 372},                  //19~21
        {4096, 256},                                          //22
        {8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
        {744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
        {1640, 640},                                          //35
        {1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
        {320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
        {32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
        {448, 2340},                                          //48
        {640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
        {2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
        {32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
        {3276, 320},{2340, 448},                              //61~62
        {1640, 640},{1168, 898},{898, 1168}                   //63~65
    };
#if MIPF

    static const s16 com_tbl_ipred_adi_bilinear[32][4] =
    {
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
        { 9, 41, 55, 23 },
        { 8, 40, 56, 24 },
        { 7, 39, 57, 25 },
        { 6, 38, 58, 26 },
        { 5, 37, 59, 27 },
        { 4, 36, 60, 28 },
        { 3, 35, 61, 29 },
        { 2, 34, 62, 30 },
        { 1, 33, 63, 31 },
    };

    static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
    {
        /* best for CfP extra smooth */
        { 23, 82, 21, 2 },
        { 21, 82, 23, 2 },
        { 19, 82, 25, 2 },
        { 18, 81, 27, 2 },
        { 16, 80, 30, 2 },
        { 15, 79, 32, 2 },
        { 13, 78, 35, 2 },
        { 12, 77, 37, 2 },
        { 11, 75, 40, 2 },
        { 10, 74, 42, 2 },
        { 9, 72, 45, 2 },
        { 8, 70, 47, 3 },
        { 7, 68, 50, 3 },
        { 6, 67, 52, 3 },
        { 6, 64, 54, 4 },
        { 5, 62, 57, 4 },
        { 4, 60, 60, 4 },
        { 4, 57, 62, 5 },
        { 4, 54, 64, 6 },
        { 3, 52, 67, 6 },
        { 3, 50, 68, 7 },
        { 3, 47, 70, 8 },
        { 2, 45, 72, 9 },
        { 2, 42, 74, 10 },
        { 2, 40, 75, 11 },
        { 2, 37, 77, 12 },
        { 2, 35, 78, 13 },
        { 2, 32, 79, 15 },
        { 2, 30, 80, 16 },
        { 2, 27, 81, 18 },
        { 2, 25, 82, 19 },
        { 2, 23, 82, 21 },
    };

    static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
    {
        // semi-sharp
        { 11, 106, 12, -1 },
        { 9, 105, 15, -1 },
        { 7, 105, 18, -2 },
        { 6, 103, 21, -2 },
        { 4, 102, 25, -3 },
        { 2, 100, 29, -3 },
        { 1, 99, 32, -4 },
        { 0, 96, 36, -4 },
        { -1, 94, 39, -4 },
        { -2, 92, 43, -5 },
        { -3, 89, 47, -5 },
        { -4, 86, 51, -5 },
        { -4, 83, 55, -6 },
        { -5, 80, 59, -6 },
        { -5, 77, 62, -6 },
        { -6, 74, 66, -6 },
        { -6, 70, 70, -6 },
        { -6, 66, 74, -6 },
        { -6, 62, 77, -5 },
        { -6, 59, 80, -5 },
        { -6, 55, 83, -4 },
        { -5, 51, 86, -4 },
        { -5, 47, 89, -3 },
        { -5, 43, 92, -2 },
        { -4, 39, 94, -1 },
        { -4, 36, 96, 0 },
        { -4, 32, 99, 1 },
        { -3, 29, 100, 2 },
        { -3, 25, 102, 4 },
        { -2, 21, 103, 6 },
        { -2, 18, 105, 7 },
        { -1, 15, 105, 9 },
    };

    static const s16 tbl_mc_c_coeff_hp[32][4] =
    {
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
#else
    static const s16 com_tbl_ipred_adi[32][4] = { { (32), (64), (32), 0 }, { (31), (63), (33), (1) }, { (30), (62), (34), (2) }, { (29), (61), (35), (3) }, { (28), (60), (36), (4) }, { (27), (59), (37), (5) }, { (26), (58), (38), (6) }, { (25), (57), (39), (7) }, { (24), (56), (40), (8) }, { (23), (55), (41), (9) }, { (22), (54), (42), (10) }, { (21), (53), (43), (11) }, { (20), (52), (44), (12) }, { (19), (51), (45), (13) }, { (18), (50), (46), (14) }, { (17), (49), (47), (15) }, { (16), (48), (48), (16) }, { (15), (47), (49), (17) }, { (14), (46), (50), (18) }, { (13), (45), (51), (19) }, { (12), (44), (52), (20) }, { (11), (43), (53), (21) }, { (10), (42), (54), (22) }, { (9), (41), (55), (23) }, { (8), (40), (56), (24) }, { (7), (39), (57), (25) }, { (6), (38), (58), (26) }, { (5), (37), (59), (27) }, { (4), (36), (60), (28) }, { (3), (35), (61), (29) }, { (2), (34), (62), (30) }, { (1), (33), (63), (31) } };
#endif

#if MIPF
    //const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
    const int filter_bits_list[4] = { 7, 7, 6, 7 };
    const int filter_offset_list[4] = { 64, 64 ,32, 64 };
    const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
    const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
    int filter_idx;
#else
    const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
    const int filter_offset = ADI_4T_FILTER_OFFSET;
    const int filter_bits = ADI_4T_FILTER_BITS;
#endif

    const int* mt = com_tbl_ipred_dxdy[ipm];

    int offset_x[MAX_CU_SIZE], offset_y[MAX_CU_SIZE];
    int t_dx[MAX_CU_SIZE], t_dy[MAX_CU_SIZE];
    int i, j;
    int offset = 0;
    int pos_max = w + h - 1;
    int p, pn, pn_n1, pn_p2;

    if (((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
    {
        if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
            pos_max = w * 2 - 1;
        else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
            pos_max = h * 2 - 1;

        for (j = 0; j < h; j++)
        {
            int dx;

            for (i = 0; i < w; i++)
            {
                int x;
                int src_p, src_pn, src_pn_n1, src_pn_p2;

                if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) {
                    GET_REF_POS(mt[0], j + 1, dx, offset);
                    filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;

                    x = i + dx;

                    pn_n1 = x - 1;
                    p = x;
                    pn = x + 1;
                    pn_p2 = x + 2;

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

                    src_pn_n1 = src_up[pn_n1];
                    src_p = src_up[p];
                    src_pn = src_up[pn];
                    src_pn_p2 = src_up[pn_p2];
                }
                else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
                {
                    GET_REF_POS(mt[1], i + 1, dx, offset);
                    filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

                    x = j + dx;

                    pn_n1 = x - 1;
                    p = x;
                    pn = x + 1;
                    pn_p2 = x + 2;

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

                    src_pn_n1 = src_le[pn_n1];
                    src_p = src_le[p];
                    src_pn = src_le[pn];
                    src_pn_p2 = src_le[pn_p2];

                }

                if (filter_idx == 0)
                    tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
                        src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 1)
                    tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
                        src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 2)
                    tmp = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
                        src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                else if (filter_idx == 3)
                    tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
                        src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
                        filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                diff[j][i] = tmp1;

            }
        }
    }
    else
    {
        for (i = 0; i < w; i++)
        {
            GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
            t_dy[i] = -t_dy[i];
        }
        for (j = 0; j < h; j++)
        {
            GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
            t_dx[j] = -t_dx[j];
        }

        {
            for (j = 0; j < h; j++)
            {
                for (i = 0; i < w; i++)
                {
                    int x = i + t_dx[j];
                    int y = j + t_dy[i];
                    int src_p, src_pn, src_pn_n1, src_pn_p2;

                    if (y <= -1)
                    {
                        offset = offset_x[j];
                        pos_max = w * 2 - 1;

                        pn_n1 = x + 1;
                        p = x;
                        pn = x - 1;
                        pn_p2 = x - 2;

                        pn_n1 = COM_MIN(pn_n1, pos_max);
                        p = COM_MIN(p, pos_max);
                        pn = COM_MIN(pn, pos_max);
                        pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1 = src_up[pn_n1];
                        src_p = src_up[p];
                        src_pn = src_up[pn];
                        src_pn_p2 = src_up[pn_p2];

                    }
                    else
                    {
                        offset = offset_y[i];
                        pos_max = h * 2 - 1;

                        pn_n1 = y + 1;
                        p = y;
                        pn = y - 1;
                        pn_p2 = y - 2;

                        pn_n1 = COM_MIN(pn_n1, pos_max);
                        p = COM_MIN(p, pos_max);
                        pn = COM_MIN(pn, pos_max);
                        pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1 = src_le[pn_n1];
                        src_p = src_le[p];
                        src_pn = src_le[pn];
                        src_pn_p2 = src_le[pn_p2];

                    }

                    if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT)) {
                        filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
                    }
                    else
                    {
                        filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
                    }

                    if (filter_idx == 0)
                        tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
                            src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    else if (filter_idx == 1)
                        tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
                            src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    else if (filter_idx == 2)
                        tmp = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
                            src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    else if (filter_idx == 3)
                        tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
                            src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
                    diff[j][i] = tmp1;

                }
            }
        }
    }
}

void ipred_ang_eipm_Chroma16(pel src_le[35], pel src_up[35], int w, int h, int ipm
#if MIPF
    , int is_luma, int mipf_enable_flag, U4 bit_depth, pel pred_for_diff[16][16], pel pred[16][16]
#endif
)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=diff complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_for_diff complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
    pel tmp[32], tmp1;
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=0
    static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
    {
        {0, 0},{0, 0},{0, 0},                                 //0~2
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
        {744, 1408},{512, 2048},                              //7~8
        {372, 2816},                                          //9
        {256, 4096},                                          //10
        {128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
        {372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
        {1408, 744},{2048, 512},{2816, 372},                  //19~21
        {4096, 256},                                          //22
        {8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
        {744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
        {1640, 640},                                          //35
        {1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
        {320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
        {32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
        {448, 2340},                                          //48
        {640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
        {2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
        {32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
        {3276, 320},{2340, 448},                              //61~62
        {1640, 640},{1168, 898},{898, 1168}                   //63~65
    };
#pragma HLS ARRAY_PARTITION variable = com_tbl_ipred_dxdy complete dim = 2
#if MIPF

    static const s16 com_tbl_ipred_adi_bilinear[32][4] =
    {
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
        { 9, 41, 55, 23 },
        { 8, 40, 56, 24 },
        { 7, 39, 57, 25 },
        { 6, 38, 58, 26 },
        { 5, 37, 59, 27 },
        { 4, 36, 60, 28 },
        { 3, 35, 61, 29 },
        { 2, 34, 62, 30 },
        { 1, 33, 63, 31 },
    };

    static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
    {
        /* best for CfP extra smooth */
        { 23, 82, 21, 2 },
        { 21, 82, 23, 2 },
        { 19, 82, 25, 2 },
        { 18, 81, 27, 2 },
        { 16, 80, 30, 2 },
        { 15, 79, 32, 2 },
        { 13, 78, 35, 2 },
        { 12, 77, 37, 2 },
        { 11, 75, 40, 2 },
        { 10, 74, 42, 2 },
        { 9, 72, 45, 2 },
        { 8, 70, 47, 3 },
        { 7, 68, 50, 3 },
        { 6, 67, 52, 3 },
        { 6, 64, 54, 4 },
        { 5, 62, 57, 4 },
        { 4, 60, 60, 4 },
        { 4, 57, 62, 5 },
        { 4, 54, 64, 6 },
        { 3, 52, 67, 6 },
        { 3, 50, 68, 7 },
        { 3, 47, 70, 8 },
        { 2, 45, 72, 9 },
        { 2, 42, 74, 10 },
        { 2, 40, 75, 11 },
        { 2, 37, 77, 12 },
        { 2, 35, 78, 13 },
        { 2, 32, 79, 15 },
        { 2, 30, 80, 16 },
        { 2, 27, 81, 18 },
        { 2, 25, 82, 19 },
        { 2, 23, 82, 21 },
    };

    static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
    {
        // semi-sharp
        { 11, 106, 12, -1 },
        { 9, 105, 15, -1 },
        { 7, 105, 18, -2 },
        { 6, 103, 21, -2 },
        { 4, 102, 25, -3 },
        { 2, 100, 29, -3 },
        { 1, 99, 32, -4 },
        { 0, 96, 36, -4 },
        { -1, 94, 39, -4 },
        { -2, 92, 43, -5 },
        { -3, 89, 47, -5 },
        { -4, 86, 51, -5 },
        { -4, 83, 55, -6 },
        { -5, 80, 59, -6 },
        { -5, 77, 62, -6 },
        { -6, 74, 66, -6 },
        { -6, 70, 70, -6 },
        { -6, 66, 74, -6 },
        { -6, 62, 77, -5 },
        { -6, 59, 80, -5 },
        { -6, 55, 83, -4 },
        { -5, 51, 86, -4 },
        { -5, 47, 89, -3 },
        { -5, 43, 92, -2 },
        { -4, 39, 94, -1 },
        { -4, 36, 96, 0 },
        { -4, 32, 99, 1 },
        { -3, 29, 100, 2 },
        { -3, 25, 102, 4 },
        { -2, 21, 103, 6 },
        { -2, 18, 105, 7 },
        { -1, 15, 105, 9 },
    };

    static const s16 tbl_mc_c_coeff_hp[32][4] =
    {
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
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_bilinear complete dim=0
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_extra_smooth complete dim=0
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_semi_sharp complete dim=0
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_hp complete dim=0
#else
    static const s16 com_tbl_ipred_adi[32][4] = { { (32), (64), (32), 0 }, { (31), (63), (33), (1) }, { (30), (62), (34), (2) }, { (29), (61), (35), (3) }, { (28), (60), (36), (4) }, { (27), (59), (37), (5) }, { (26), (58), (38), (6) }, { (25), (57), (39), (7) }, { (24), (56), (40), (8) }, { (23), (55), (41), (9) }, { (22), (54), (42), (10) }, { (21), (53), (43), (11) }, { (20), (52), (44), (12) }, { (19), (51), (45), (13) }, { (18), (50), (46), (14) }, { (17), (49), (47), (15) }, { (16), (48), (48), (16) }, { (15), (47), (49), (17) }, { (14), (46), (50), (18) }, { (13), (45), (51), (19) }, { (12), (44), (52), (20) }, { (11), (43), (53), (21) }, { (10), (42), (54), (22) }, { (9), (41), (55), (23) }, { (8), (40), (56), (24) }, { (7), (39), (57), (25) }, { (6), (38), (58), (26) }, { (5), (37), (59), (27) }, { (4), (36), (60), (28) }, { (3), (35), (61), (29) }, { (2), (34), (62), (30) }, { (1), (33), (63), (31) } };
#endif

#if MIPF
    //const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
    const int filter_bits_list[4] = { 7, 7, 6, 7 };
    const int filter_offset_list[4] = { 64, 64 ,32, 64 };
#pragma HLS ARRAY_PARTITION variable=filter_bits_list complete dim=0
#pragma HLS ARRAY_PARTITION variable=filter_offset_list complete dim=0
    const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
    const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
    int filter_idx;
#else
    const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
    const int filter_offset = ADI_4T_FILTER_OFFSET;
    const int filter_bits = ADI_4T_FILTER_BITS;
#endif

    const int* mt = com_tbl_ipred_dxdy[ipm];

    //const pel* src_ch = NULL;
    //const s16* filter;

    int offset_x[16], offset_y[16];
    int t_dx[16], t_dy[16];
    int i, j;
    int offset = 0;
    int pos_max = w + h - 1;
    int p, pn, pn_n1, pn_p2;

    pel src_p_tmp[32], src_pn_tmp[32], src_pn_n1_tmp[32], src_pn_p2_tmp[32];
    int filter_idx_tmp[32];
    int offset_tmp[32];
    s16 data1 ;
    s16 data2;
    s16 data3;
    s16 data4;
    
#pragma HLS ARRAY_PARTITION variable=offset_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=offset_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=t_dx complete dim=0
#pragma HLS ARRAY_PARTITION variable=t_dy complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_p_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_pn_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_pn_n1_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_pn_p2_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=filter_idx_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=offset_tmp complete dim=0
    if ( ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
    {

        if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
            pos_max = w * 2 - 1;
        else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
            pos_max = h * 2 - 1;
    }
    else {
        for (i = 0; i < 16; i++)
        {
#pragma HLS UNROLL
            if (i < w) {
                GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
                t_dy[i] = -t_dy[i];
            }
        }
        for (j = 0; j < 16; j++)
        {
#pragma HLS UNROLL
            if (j < h) {
                GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
                t_dx[j] = -t_dx[j];
            }
        }
    }
        for (j = 0; j < h; j++)
        {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=16 max=16 avg=16
        for (i = 0; i < 16; i++)
            {
            if (i < w) {
                if (((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
                {
                    int dx;
                int x;

                if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) {
                        GET_REF_POS(mt[0], j + 1, dx, offset_tmp[i]);
                        filter_idx_tmp[i] = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
                    x = i + dx;

                    pn_n1 = x - 1;
                    p = x;
                    pn = x + 1;
                    pn_p2 = x + 2;

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1_tmp[i] = src_up[pn_n1];
                        src_p_tmp[i] = src_up[p];
                        src_pn_tmp[i] = src_up[pn];
                        src_pn_p2_tmp[i] = src_up[pn_p2];
                }
                else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
                {
                        GET_REF_POS(mt[1], i + 1, dx, offset_tmp[i]);
                        filter_idx_tmp[i] = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

                    x = j + dx;

                    pn_n1 = x - 1;
                    p = x;
                    pn = x + 1;
                    pn_p2 = x + 2;

                    pn_n1 = COM_MIN(pn_n1, pos_max);
                    p = COM_MIN(p, pos_max);
                    pn = COM_MIN(pn, pos_max);
                    pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1_tmp[i] = src_le[pn_n1];
                        src_p_tmp[i] = src_le[p];
                        src_pn_tmp[i] = src_le[pn];
                        src_pn_p2_tmp[i] = src_le[pn_p2];
                }

                //diff[j][i] = tmp1;
            }

                else {


                    int x = i + t_dx[j];
                    int y = j + t_dy[i];
                    int src_p, src_pn, src_pn_n1, src_pn_p2;

                    if (y <= -1)
                    {

                        offset_tmp[i] = offset_x[j];
                        pos_max = w * 2 - 1;

                        pn_n1 = x + 1;
                        p = x;
                        pn = x - 1;
                        pn_p2 = x - 2;

                        pn_n1 = COM_MIN(pn_n1, pos_max);
                        p = COM_MIN(p, pos_max);
                        pn = COM_MIN(pn, pos_max);
                        pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1_tmp[i] = src_up[pn_n1];
                        src_p_tmp[i] = src_up[p];
                        src_pn_tmp[i] = src_up[pn];
                        src_pn_p2_tmp[i] = src_up[pn_p2];
                    }
                    else
                    {

                        offset_tmp[i] = offset_y[i];
                        pos_max = h * 2 - 1;

                        pn_n1 = y + 1;
                        p = y;
                        pn = y - 1;
                        pn_p2 = y - 2;

                        pn_n1 = COM_MIN(pn_n1, pos_max);
                        p = COM_MIN(p, pos_max);
                        pn = COM_MIN(pn, pos_max);
                        pn_p2 = COM_MIN(pn_p2, pos_max);

                        src_pn_n1_tmp[i] = src_le[pn_n1];
                        src_p_tmp[i] = src_le[p];
                        src_pn_tmp[i] = src_le[pn];
                        src_pn_p2_tmp[i] = src_le[pn_p2];
                    }
                    if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT)) {
                        filter_idx_tmp[i] = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
                    }
                    else
                    {
                        filter_idx_tmp[i] = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
                    }
                }
            }
        }
        for (i = 0; i < 16; i++)
        {
            if (i < w) {
                int src_p, src_pn, src_pn_n1, src_pn_p2;
                filter_idx = filter_idx_tmp[i];
                offset = offset_tmp[i];
                src_pn_n1 = src_pn_n1_tmp[i];
                src_p = src_p_tmp[i];
                src_pn = src_pn_tmp[i];
                src_pn_p2 = src_pn_p2_tmp[i];
                    if (filter_idx == 0)
                    {
                      data1 = com_tbl_ipred_adi_extra_smooth[offset][0];
                      data2 = com_tbl_ipred_adi_extra_smooth[offset][1];
                      data3 = com_tbl_ipred_adi_extra_smooth[offset][2];
                      data4 = com_tbl_ipred_adi_extra_smooth[offset][3];
                    // tmp[i] = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
                            // src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
                            // filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    else if (filter_idx == 1)
                    {
                      data1 = com_tbl_ipred_adi_semi_sharp[offset][0];
                      data2 = com_tbl_ipred_adi_semi_sharp[offset][1];
                      data3 = com_tbl_ipred_adi_semi_sharp[offset][2];
                      data4 = com_tbl_ipred_adi_semi_sharp[offset][3];
                    // tmp[i] = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
                            // src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
                            // filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    else if (filter_idx == 2)
                    {
                      data1 = tbl_mc_c_coeff_hp[offset][0];
                      data2 = tbl_mc_c_coeff_hp[offset][1];
                      data3 = tbl_mc_c_coeff_hp[offset][2];
                      data4 = tbl_mc_c_coeff_hp[offset][3];
                    // tmp[i] = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
                            // src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
                            // filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    else if (filter_idx == 3)
                    {
                      data1 = com_tbl_ipred_adi_bilinear[offset][0];
                      data2 = com_tbl_ipred_adi_bilinear[offset][1];
                      data3 = com_tbl_ipred_adi_bilinear[offset][2];
                      data4 = com_tbl_ipred_adi_bilinear[offset][3];
                    // tmp[i] = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
                            // src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
                            // filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    }
                    tmp[i] = (pel)((src_pn_n1 * data1 + src_p * data2 + src_pn * data3 + src_pn_p2 * data4 + filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

            }
        }
        for (i = 0; i < 16; i++)
        {
            if (i < w) {
                tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp[i]);
                    //diff[j][i] = tmp1;
                //diff[j][i] = orgY[j][i] - tmp1;
                    pred_for_diff[j][i] = tmp1;
                    pred[j][i] = tmp1;
                }

            }
        }

}


void ipred_ang_eipm_Chroma8(pel src_le[19], pel src_up[19], int w, int h, int ipm
#if MIPF
    , int is_luma, int mipf_enable_flag, U4 bit_depth, s16 pred_1[8][8], pel pred_for_diff[8][8]
#endif
)
{
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_for_diff complete dim=2

    pel tmp[8], tmp1;
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=0

    static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
    {
        {0, 0},{0, 0},{0, 0},                                 //0~2
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
        {744, 1408},{512, 2048},                              //7~8
        {372, 2816},                                          //9
        {256, 4096},                                          //10
        {128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
        {372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
        {1408, 744},{2048, 512},{2816, 372},                  //19~21
        {4096, 256},                                          //22
        {8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
        {2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
        {744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
        {1640, 640},                                          //35
        {1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
        {320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
        {32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
        {448, 2340},                                          //48
        {640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
        {2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
        {32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
        {3276, 320},{2340, 448},                              //61~62
        {1640, 640},{1168, 898},{898, 1168}                   //63~65
    };
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_dxdy complete dim=2
#if MIPF

    static const s16 com_tbl_ipred_adi_bilinear[32][4] =
    {
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
        { 9, 41, 55, 23 },
        { 8, 40, 56, 24 },
        { 7, 39, 57, 25 },
        { 6, 38, 58, 26 },
        { 5, 37, 59, 27 },
        { 4, 36, 60, 28 },
        { 3, 35, 61, 29 },
        { 2, 34, 62, 30 },
        { 1, 33, 63, 31 },
    };

    static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
    {
        /* best for CfP extra smooth */
        { 23, 82, 21, 2 },
        { 21, 82, 23, 2 },
        { 19, 82, 25, 2 },
        { 18, 81, 27, 2 },
        { 16, 80, 30, 2 },
        { 15, 79, 32, 2 },
        { 13, 78, 35, 2 },
        { 12, 77, 37, 2 },
        { 11, 75, 40, 2 },
        { 10, 74, 42, 2 },
        { 9, 72, 45, 2 },
        { 8, 70, 47, 3 },
        { 7, 68, 50, 3 },
        { 6, 67, 52, 3 },
        { 6, 64, 54, 4 },
        { 5, 62, 57, 4 },
        { 4, 60, 60, 4 },
        { 4, 57, 62, 5 },
        { 4, 54, 64, 6 },
        { 3, 52, 67, 6 },
        { 3, 50, 68, 7 },
        { 3, 47, 70, 8 },
        { 2, 45, 72, 9 },
        { 2, 42, 74, 10 },
        { 2, 40, 75, 11 },
        { 2, 37, 77, 12 },
        { 2, 35, 78, 13 },
        { 2, 32, 79, 15 },
        { 2, 30, 80, 16 },
        { 2, 27, 81, 18 },
        { 2, 25, 82, 19 },
        { 2, 23, 82, 21 },
    };

    static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
    {
        // semi-sharp
        { 11, 106, 12, -1 },
        { 9, 105, 15, -1 },
        { 7, 105, 18, -2 },
        { 6, 103, 21, -2 },
        { 4, 102, 25, -3 },
        { 2, 100, 29, -3 },
        { 1, 99, 32, -4 },
        { 0, 96, 36, -4 },
        { -1, 94, 39, -4 },
        { -2, 92, 43, -5 },
        { -3, 89, 47, -5 },
        { -4, 86, 51, -5 },
        { -4, 83, 55, -6 },
        { -5, 80, 59, -6 },
        { -5, 77, 62, -6 },
        { -6, 74, 66, -6 },
        { -6, 70, 70, -6 },
        { -6, 66, 74, -6 },
        { -6, 62, 77, -5 },
        { -6, 59, 80, -5 },
        { -6, 55, 83, -4 },
        { -5, 51, 86, -4 },
        { -5, 47, 89, -3 },
        { -5, 43, 92, -2 },
        { -4, 39, 94, -1 },
        { -4, 36, 96, 0 },
        { -4, 32, 99, 1 },
        { -3, 29, 100, 2 },
        { -3, 25, 102, 4 },
        { -2, 21, 103, 6 },
        { -2, 18, 105, 7 },
        { -1, 15, 105, 9 },
    };

    static const s16 tbl_mc_c_coeff_hp[32][4] =
    {
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
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_bilinear complete dim=0
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_extra_smooth complete dim=0
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_semi_sharp complete dim=0
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_hp complete dim=0

#else
    static const s16 com_tbl_ipred_adi[32][4] = { { (32), (64), (32), 0 }, { (31), (63), (33), (1) }, { (30), (62), (34), (2) }, { (29), (61), (35), (3) }, { (28), (60), (36), (4) }, { (27), (59), (37), (5) }, { (26), (58), (38), (6) }, { (25), (57), (39), (7) }, { (24), (56), (40), (8) }, { (23), (55), (41), (9) }, { (22), (54), (42), (10) }, { (21), (53), (43), (11) }, { (20), (52), (44), (12) }, { (19), (51), (45), (13) }, { (18), (50), (46), (14) }, { (17), (49), (47), (15) }, { (16), (48), (48), (16) }, { (15), (47), (49), (17) }, { (14), (46), (50), (18) }, { (13), (45), (51), (19) }, { (12), (44), (52), (20) }, { (11), (43), (53), (21) }, { (10), (42), (54), (22) }, { (9), (41), (55), (23) }, { (8), (40), (56), (24) }, { (7), (39), (57), (25) }, { (6), (38), (58), (26) }, { (5), (37), (59), (27) }, { (4), (36), (60), (28) }, { (3), (35), (61), (29) }, { (2), (34), (62), (30) }, { (1), (33), (63), (31) } };
#endif

#if MIPF
    //const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
    const int filter_bits_list[4] = { 7, 7, 6, 7 };
    const int filter_offset_list[4] = { 64, 64 ,32, 64 };
#pragma HLS ARRAY_PARTITION variable=filter_bits_list complete dim=0
#pragma HLS ARRAY_PARTITION variable=filter_offset_list complete dim=0
    const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
    const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
    int filter_idx;
#else
    const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
    const int filter_offset = ADI_4T_FILTER_OFFSET;
    const int filter_bits = ADI_4T_FILTER_BITS;
#endif

    const int* mt = com_tbl_ipred_dxdy[ipm];

    int offset_x[8], offset_y[8];
    int t_dx[8], t_dy[8];
    int i, j;
    int offset = 0;
    int pos_max = w + h - 1;
    int p, pn, pn_n1, pn_p2;
    int src_p, src_pn, src_pn_n1, src_pn_p2;
    int src_p_tmp[8], src_pn_tmp[8], src_pn_n1_tmp[8], src_pn_p2_tmp[8];
    int filter_idx_tmp[8];
    int offset_tmp[8];

#pragma HLS ARRAY_PARTITION variable=offset_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=offset_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=t_dx complete dim=0
#pragma HLS ARRAY_PARTITION variable=t_dy complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_p_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_pn_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_pn_n1_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_pn_p2_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=filter_idx_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=offset_tmp complete dim=0

    if (((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
    {
        if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
            pos_max = w * 2 - 1;
        else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
            pos_max = h * 2 - 1;
    }
    else {
        for (i = 0; i < 8; i++)
        {
#pragma HLS UNROLL
            if (i < w) {
                GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
                t_dy[i] = -t_dy[i];
            }
        }
        for (j = 0; j < 8; j++)
        {
#pragma HLS UNROLL
            if (j < h) {
                GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
                t_dx[j] = -t_dx[j];
            }
        }
    }
        for (j = 0; j < h; j++)
        {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
            for (i = 0; i < 8; i++)
            {
                if (i < w) {
                    if (((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
                    {
                        int dx;
                        int x;
                        if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) {
                            GET_REF_POS(mt[0], j + 1, dx, offset_tmp[i]);
                            filter_idx_tmp[i] = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
                            x = i + dx;

                            pn_n1 = x - 1;
                            p = x;
                            pn = x + 1;
                            pn_p2 = x + 2;

                            pn_n1 = COM_MIN(pn_n1, pos_max);
                            p = COM_MIN(p, pos_max);
                            pn = COM_MIN(pn, pos_max);
                            pn_p2 = COM_MIN(pn_p2, pos_max);

                            src_pn_n1_tmp[i] = src_up[pn_n1];
                            src_p_tmp[i] = src_up[p];
                            src_pn_tmp[i] = src_up[pn];
                            src_pn_p2_tmp[i] = src_up[pn_p2];
                        }
                        else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
                        {
                            GET_REF_POS(mt[1], i + 1, dx, offset_tmp[i]);
                            filter_idx_tmp[i] = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

                            x = j + dx;

                            pn_n1 = x - 1;
                            p = x;
                            pn = x + 1;
                            pn_p2 = x + 2;

                            pn_n1 = COM_MIN(pn_n1, pos_max);
                            p = COM_MIN(p, pos_max);
                            pn = COM_MIN(pn, pos_max);
                            pn_p2 = COM_MIN(pn_p2, pos_max);

                            src_pn_n1_tmp[i] = src_le[pn_n1];
                            src_p_tmp[i] = src_le[p];
                            src_pn_tmp[i] = src_le[pn];
                            src_pn_p2_tmp[i] = src_le[pn_p2];
                        }
                    }
                    else {
                        int x = i + t_dx[j];
                        int y = j + t_dy[i];


                        if (y <= -1)
                        {

                            offset_tmp[i] = offset_x[j];
                            pos_max = w * 2 - 1;

                            pn_n1 = x + 1;
                            p = x;
                            pn = x - 1;
                            pn_p2 = x - 2;

                            pn_n1 = COM_MIN(pn_n1, pos_max);
                            p = COM_MIN(p, pos_max);
                            pn = COM_MIN(pn, pos_max);
                            pn_p2 = COM_MIN(pn_p2, pos_max);

                            src_pn_n1_tmp[i] = src_up[pn_n1];
                            src_p_tmp[i] = src_up[p];
                            src_pn_tmp[i] = src_up[pn];
                            src_pn_p2_tmp[i] = src_up[pn_p2];
                        }
                        else
                        {

                            offset_tmp[i] = offset_y[i];
                            pos_max = h * 2 - 1;

                            pn_n1 = y + 1;
                            p = y;
                            pn = y - 1;
                            pn_p2 = y - 2;

                            pn_n1 = COM_MIN(pn_n1, pos_max);
                            p = COM_MIN(p, pos_max);
                            pn = COM_MIN(pn, pos_max);
                            pn_p2 = COM_MIN(pn_p2, pos_max);

                            src_pn_n1_tmp[i] = src_le[pn_n1];
                            src_p_tmp[i] = src_le[p];
                            src_pn_tmp[i] = src_le[pn];
                            src_pn_p2_tmp[i] = src_le[pn_p2];
                        }
                        if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT)) {
                            filter_idx_tmp[i] = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
                        }
                        else
                        {
                            filter_idx_tmp[i] = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
                        }
                    }
                }
            }
            for (i = 0; i < 8; i++)
            {
                if (i < w) {
                    filter_idx = filter_idx_tmp[i];
                    offset = offset_tmp[i];
                    src_pn_n1 = src_pn_n1_tmp[i];
                    src_p = src_p_tmp[i];
                    src_pn = src_pn_tmp[i];
                    src_pn_p2 = src_pn_p2_tmp[i];

#if 0
                    if (filter_idx == 0)
                        tmp[i] = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
                            src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    else if (filter_idx == 1)
                        tmp[i] = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
                            src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    else if (filter_idx == 2)
                        tmp[i] = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
                            src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
                    else if (filter_idx == 3)
                        tmp[i] = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
                            src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
                            filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

            
            for (i = 0; i < 8; i++)
            {
                if (i < w) {
                    tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp[i]);
                    pred_1[j][i] = tmp1;
                    pred_for_diff[j][i] = tmp1;
                }
 }
#else
                    s16 adi_tmp[4];
                    int filter_offset;
                    int filter_bits;
                    pel tmp0;
                    if (filter_idx == 0) {
                        for (int idx = 0; idx < 4; idx++)
                            adi_tmp[idx] = com_tbl_ipred_adi_extra_smooth[offset][idx];
                        filter_offset = filter_offset_list[0];
                        filter_bits = filter_bits_list[0];
                    }
                    else if (filter_idx == 1) {
                        for (int idx = 0; idx < 4; idx++)
                            adi_tmp[idx] = com_tbl_ipred_adi_semi_sharp[offset][idx];
                        filter_offset = filter_offset_list[1];
                        filter_bits = filter_bits_list[1];
                    }
                    else if (filter_idx == 2) {
                        for (int idx = 0; idx < 4; idx++)
                            adi_tmp[idx] = tbl_mc_c_coeff_hp[offset][idx];
                        filter_offset = filter_offset_list[2];
                        filter_bits = filter_bits_list[2];
                    }
                    else if (filter_idx == 3) {
                        for (int idx = 0; idx < 4; idx++)
                            adi_tmp[idx] = com_tbl_ipred_adi_bilinear[offset][idx];
                        filter_offset = filter_offset_list[2];
                        filter_bits = filter_bits_list[2];
                    }
                    tmp0 = (pel)((src_pn_n1 * adi_tmp[0] + src_p * adi_tmp[1] + src_pn * adi_tmp[2] + src_pn_p2 * adi_tmp[3] + filter_offset) >> filter_bits);

                        tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp0);
                        pred_1[j][i] = tmp1;
                        pred_for_diff[j][i] = tmp1;

#endif
                }
            }


        }
    
    
    
}

static void ipred_hor(pel* src, pel* dst, int i_dst, int w, int h)
{
    for (int i = 0; i < h; i++) {
        memset(dst, src[-i], w);
        dst += i_dst;
    }
}


static void ipred_vert(pel* src, pel* dst, int i_dst, int w, int h)
{
    while (h--) {
        memcpy(dst, src, w * sizeof(pel));
        dst += i_dst;
    }
}

static void ipred_dc_4(pel* src, pel* dst, int i_dst, int w, int h, u16 avail_cu, int bit_depth)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    int dc = 0;
    int i, j;
    if (IS_AVAIL(avail_cu, AVAIL_LE)) {
        for (i = 0; i < h; i++) {
            dc += src[-i - 1];
        }
        if (IS_AVAIL(avail_cu, AVAIL_UP)) {
            for (j = 0; j < w; j++) {
                dc += src[j + 1];
            }
            dc = (dc + ((w + h) >> 1)) * (4096 / (w + h)) >> 12;
        }
        else {
            dc = (dc + (h >> 1)) >> com_tbl_log2[h];
        }
    }
    else if (IS_AVAIL(avail_cu, AVAIL_UP)) {
        for (j = 0; j < w; j++) {
            dc += src[j + 1];
        }
        dc = (dc + (w >> 1)) >> com_tbl_log2[w];
    }
    else {
        dc = 1 << (bit_depth - 1);
    }

    while (h--) {
        memset(dst, dc, w);
        dst += i_dst;
    }
}

static void ipred_plane_4(pel* src, pel* dst, int i_dst, int w, int h, int bit_depth)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    pel* rsrc;
    int  coef_h = 0, coef_v = 0;
    int  a, b, c, x, y;
    int  w2 = w >> 1;
    int  h2 = h >> 1;
    int  ib_mult[5] = { 13, 17, 5, 11, 23 };
    int  ib_shift[5] = { 7, 10, 11, 15, 19 };
    int  idx_w = com_tbl_log2[w] - 2;
    int  idx_h = com_tbl_log2[h] - 2;
    int  im_h, is_h, im_v, is_v, temp, temp2;
    int  max_pel = (1 << bit_depth) - 1;
    int  val;

    im_h = ib_mult[idx_w];
    is_h = ib_shift[idx_w];
    im_v = ib_mult[idx_h];
    is_v = ib_shift[idx_h];
    rsrc = src + w2;

    for (x = 1; x < w2 + 1; x++) {
        coef_h += x * (rsrc[x] - rsrc[-x]);
    }
    rsrc = src - h2;
    for (y = 1; y < h2 + 1; y++) {
        coef_v += y * (rsrc[-y] - rsrc[y]);
    }
    a = (src[-h] + src[w]) << 4;
    b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
    temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

    for (y = 0; y < h; y++) {
        temp2 = temp;
        for (x = 0; x < w; x++) {
            val = temp2 >> 5;
            dst[x] = (pel)COM_CLIP3(0, max_pel, val);
            temp2 += b;
        }
        temp += c;
        dst += i_dst;
    }
}

static void ipred_plane_ipf(pel* src, s16* dst, int w, int h)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    pel* rsrc;
    int  coef_h = 0, coef_v = 0;
    int  a, b, c, x, y;
    int  w2 = w >> 1;
    int  h2 = h >> 1;
    int  ib_mult[5] = { 13, 17, 5, 11, 23 };
    int  ib_shift[5] = { 7, 10, 11, 15, 19 };
    int  idx_w = com_tbl_log2[w] - 2;
    int  idx_h = com_tbl_log2[h] - 2;
    int  im_h, is_h, im_v, is_v, temp, temp2;
    im_h = ib_mult[idx_w];
    is_h = ib_shift[idx_w];
    im_v = ib_mult[idx_h];
    is_v = ib_shift[idx_h];
    rsrc = src + w2;

    for (x = 1; x < w2 + 1; x++) {
        coef_h += x * (rsrc[x] - rsrc[-x]);
    }
    rsrc = src - h2;
    for (y = 1; y < h2 + 1; y++) {
        coef_v += y * (rsrc[-y] - rsrc[y]);
    }
    a = (src[-h] + src[w]) << 4;
    b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
    c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
    temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

    for (y = 0; y < h; y++) {
        temp2 = temp;
        for (x = 0; x < w; x++) {
            dst[x] = (s16)(temp2 >> 5);
            temp2 += b;
        }
        temp += c;
        dst += w;
    }
}

static void ipred_bi_4(pel* src, pel* dst, int i_dst, int w, int h, int bit_depth)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    int x, y;
    int ishift_x = com_tbl_log2[w];
    int ishift_y = com_tbl_log2[h];
    int ishift = COM_MIN(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int a, b, c, wt, wxy, tmp;
    int predx;
    int ref_up[MAX_CU_SIZE], ref_le[MAX_CU_SIZE], up[MAX_CU_SIZE], le[MAX_CU_SIZE], wy[MAX_CU_SIZE];
    int wc, tbl_wc[6] = { -1, 21, 13, 7, 4, 2 };
    int max_pel = (1 << bit_depth) - 1;
    int val;

    wc = ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x;
    com_assert(wc <= 5);

    wc = tbl_wc[wc];
    for (x = 0; x < w; x++) {
        ref_up[x] = src[x + 1];
    }
    for (y = 0; y < h; y++) {
        ref_le[y] = src[-y - 1];
    }

    a = src[w];
    b = src[-h];
    c = (w == h) ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6);
    wt = (c << 1) - a - b;

    for (x = 0; x < w; x++) {
        up[x] = b - ref_up[x];
        ref_up[x] <<= ishift_y;
    }
    tmp = 0;
    for (y = 0; y < h; y++) {
        le[y] = a - ref_le[y];
        ref_le[y] <<= ishift_x;
        wy[y] = tmp;
        tmp += wt;
    }
    for (y = 0; y < h; y++) {
        predx = ref_le[y];
        wxy = 0;
        for (x = 0; x < w; x++) {
            predx += le[y];
            ref_up[x] += up[x];
            val = ((predx << ishift_y) + (ref_up[x] << ishift_x) + wxy + offset) >> ishift_xy;
            dst[x] = (pel)COM_CLIP3(0, max_pel, val);
            wxy += wy[y];
        }
        dst += i_dst;
    }
}

static void ipred_bi_ipf(pel* src, s16* dst, int w, int h)
{
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    int x, y;
    int ishift_x = com_tbl_log2[w];
    int ishift_y = com_tbl_log2[h];
    int ishift = COM_MIN(ishift_x, ishift_y);
    int ishift_xy = ishift_x + ishift_y + 1;
    int offset = 1 << (ishift_x + ishift_y);
    int a, b, c, wt, wxy, tmp;
    int predx;
    int ref_up[MAX_CU_SIZE], ref_le[MAX_CU_SIZE], up[MAX_CU_SIZE], le[MAX_CU_SIZE], wy[MAX_CU_SIZE];
    int wc, tbl_wc[6] = { -1, 21, 13, 7, 4, 2 };
    wc = ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x;
    com_assert(wc <= 5);

    wc = tbl_wc[wc];
    for (x = 0; x < w; x++) {
        ref_up[x] = src[x + 1];
    }
    for (y = 0; y < h; y++) {
        ref_le[y] = src[-y - 1];
    }

    a = src[w];
    b = src[-h];
    c = (w == h) ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6);
    wt = (c << 1) - a - b;

    for (x = 0; x < w; x++) {
        up[x] = b - ref_up[x];
        ref_up[x] <<= ishift_y;
    }
    tmp = 0;
    for (y = 0; y < h; y++) {
        le[y] = a - ref_le[y];
        ref_le[y] <<= ishift_x;
        wy[y] = tmp;
        tmp += wt;
    }
    for (y = 0; y < h; y++) {
        predx = ref_le[y];
        wxy = 0;
        for (x = 0; x < w; x++) {
            predx += le[y];
            ref_up[x] += up[x];
            dst[x] = (s16)(((predx << ishift_y) + (ref_up[x] << ishift_x) + wxy + offset) >> ishift_xy);
            wxy += wy[y];
        }
        dst += w;
    }
}

static void tspcm_down_sample_4(pel pSrc_y[8][8], int uiCWidth, int uiCHeight, int bitDept, int uiSrcStride, pel pDst[4 * 4], int a, int iShift, int b)
{
#pragma HLS INLINE 
    int maxResult = (1 << bitDept) - 1;
    int tempValue;
    int i, j;
    int m = 0 ;
#pragma HLS ARRAY_PARTITION variable=pSrc_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=pSrc_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=pDst complete dim=1
    for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
        for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
            int src, src1, src2, src3, src4, src5;

            src = (((s64)a * pSrc_y[m][2 * i]) >> (iShift >= 0 ? iShift : 0)) + b;
            src = COM_CLIP3(0, maxResult, src);

            src1 = (((s64)a * pSrc_y[m + 1][2 * i]) >> (iShift >= 0 ? iShift : 0)) + b;
            src1 = COM_CLIP3(0, maxResult, src1);

            if (i == 0) {
                tempValue = (src + src1 + 1) >> 1;
            }
            else {
                src2 = (((s64)a * pSrc_y[m][2 * i + 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src2 = COM_CLIP3(0, maxResult, src2);

                src3 = (((s64)a * pSrc_y[m][2 * i - 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src3 = COM_CLIP3(0, maxResult, src3);

                src4 = (((s64)a * pSrc_y[m + 1][2 * i + 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src4 = COM_CLIP3(0, maxResult, src4);

                src5 = (((s64)a * pSrc_y[m + 1][2 * i - 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src5 = COM_CLIP3(0, maxResult, src5);

                tempValue = (src * 2 + src2 + src3
                    + src1 * 2
                    + src4
                    + src5
                    + 4) >> 3;
            }

            if (tempValue > maxResult || tempValue < 0) {
                printf("\n TSCPM clip error");
            }
            pDst[i] = tempValue;
        }
        m = m + 2 ;
        pDst += uiCWidth;

    }
}

static pel  tspcm_get_luma_border(int uiIdx, int bAbovePixel, int uiCWidth, int uiCHeight, int bAboveAvail, int bLeftAvail, pel* src_left_luma, pel* src_up_luma)
{
#pragma HLS INLINE 
    pel* pSrc = NULL;
    pel dstPoint = -1;
#if 0
    if (bAbovePixel) {
        if (bAboveAvail) {
            //pSrc = luma + 1;
            pSrc = src_up_luma + 3;
            int i = uiIdx << 1;
            if (i < (uiCWidth)) {
                if (i == 0 && !bLeftAvail) {
                    dstPoint = (3 * pSrc[i] + pSrc[i + 1] + 2) >> 2;
                }
                else {
                    dstPoint = (2 * pSrc[i] + pSrc[i - 1] + pSrc[i + 1] + 2) >> 2;
                }
            }
        }
    }
    else {
        if (bLeftAvail) {
            //pSrc = luma - 1;
            pSrc = src_left_luma + 3;
            int j = uiIdx << 1;
            if (j < (uiCHeight)) {
                dstPoint = (pSrc[-j] + pSrc[-j - 1] + 1) >> 1;
            }
        }
    }
    com_assert(dstPoint >= 0);
    return dstPoint;
#else
    if (bAbovePixel)
    {
        if (bAboveAvail)
        {
            pSrc = src_up_luma + 3;
            int i = uiIdx;
            if (i < uiCWidth)
            {
                if (i == 0 && !bLeftAvail)
                {
                    dstPoint = (3 * pSrc[2 * i] + pSrc[2 * i + 1] + 2) >> 2;
                }
                else
                {
                    dstPoint = (2 * pSrc[2 * i] + pSrc[2 * i - 1] + pSrc[2 * i + 1] + 2) >> 2;
                }
            }
        }
    }
    else
    {
        if (bLeftAvail)
        {
            pSrc = src_left_luma + 3;
            int j = uiIdx;
            if (j < uiCHeight)
            {
                dstPoint = (pSrc[2 * j] + pSrc[2 * j + 1] + 1) >> 1;
            }
        }
    }

    if (dstPoint < 0)
    {
        printf("\n Error get dstPoint in xGetLumaBorderPixel");
    }
    return dstPoint;
#endif

}

#define xGetSrcPixel(idx, bAbovePixel)  tspcm_get_luma_border((idx), (bAbovePixel), uiCWidth, uiCHeight, bAboveAvaillable, bLeftAvaillable, src_left_luma,src_up_luma)
#define xExchange(a, b, type) {type exTmp; exTmp = (a); (a) = (b); (b) = exTmp;}

static int com_tbl_ai_tscpm_div[64] = {
    65536, 32768, 21845, 16384, 13107, 10922, 9362, 8192, 7281, 6553, 5957, 5461, 5041, 4681, 4369, 4096, 3855, 3640,
    3449,  3276,  3120,  2978,  2849,  2730,  2621, 2520, 2427, 2340, 2259, 2184, 2114, 2048, 1985, 1927, 1872, 1820,
    1771,  1724,  1680,  1638,  1598,  1560,  1524, 1489, 1456, 1424, 1394, 1365, 1337, 1310, 1285, 1260, 1236, 1213,
    1191,  1170,  1149,  1129,  1110,  1092,  1074, 1057, 1040, 1024,
};

static void tspcm_get_param_4(int* a, int* b, int* iShift, int bAbove, int bLeft, int uiCWidth, int uiCHeight, int bitDept, pel src_left[11],
    pel src_up[11], pel src_left_luma[19], pel src_up_luma[19])
{
#pragma HLS INLINE 
    int bAboveAvaillable = bAbove;
    int bLeftAvaillable = bLeft;
    //pel *pCur = NULL;

    unsigned uiInternalBitDepth = bitDept;

    int minDim = bLeftAvaillable && bAboveAvaillable ? COM_MIN(uiCHeight, uiCWidth) : (bLeftAvaillable ? uiCHeight : uiCWidth);
    int numSteps = minDim;
    int yMax = 0;
    int xMax = -MAX_INT;
    int yMin = 0;
    int xMin = MAX_INT;

    // four points start
    int iRefPointLuma[4] = { -1, -1, -1, -1 };
    int iRefPointChroma[4] = { -1, -1, -1, -1 };
    int yMax_sec = 0;
    int xMax_sec = -MAX_INT;
    int yMin_sec = 0;
    int xMin_sec = MAX_INT;
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_left complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_left_luma complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up_luma complete dim=1
#pragma HLS ARRAY_PARTITION variable=iRefPointLuma complete dim=0
#pragma HLS ARRAY_PARTITION variable=iRefPointChroma complete dim=0

    int idx1 = 3;  //idx1 =((numSteps - 1) * uiCWidth) / minDim;
    int idx2 = 3;  //idx2 =((numSteps - 1) * uiCWidth) / minDim;
    iRefPointLuma[0] = bAboveAvaillable ? xGetSrcPixel(0, 1) : -1;
    iRefPointChroma[0] = bAboveAvaillable ? src_up[0] :-1 ;
    iRefPointLuma[1] = bAboveAvaillable ? xGetSrcPixel(idx1, 1) :-1; // pSrc[idx];
    iRefPointChroma[1] = bAboveAvaillable ? src_up[idx1] : -1;

    iRefPointLuma[2] = bLeftAvaillable ? xGetSrcPixel(0, 0) : -1; // pSrc[0];
    iRefPointChroma[2] = bLeftAvaillable ? src_left[0] : -1;
    iRefPointLuma[3] = bLeftAvaillable ? xGetSrcPixel(idx2, 0) : -1; // pSrc[idx * iSrcStride];
    iRefPointChroma[3] = bLeftAvaillable ? src_left[idx2] : -1;

    if (bAboveAvaillable && !bLeftAvaillable && uiCWidth >= 4){
        int uiStep = uiCWidth >> 2;
        for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
            iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 1); // pSrc[i * uiStep];
            iRefPointChroma[i] = src_up[i * uiStep];
        }
    }

    else if (bLeftAvaillable && !bAboveAvaillable && uiCHeight >= 4) {
        int uiStep = uiCHeight >> 2;
        for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
            iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 0); // pSrc[i * uiStep * iSrcStride];
            iRefPointChroma[i] = src_left[i * uiStep];
        }
    
    }

    if ((bAboveAvaillable && bLeftAvaillable)
        || (bAboveAvaillable && !bLeftAvaillable && uiCWidth >= 4)
        || (bLeftAvaillable && !bAboveAvaillable && uiCHeight >= 4)) {
        int minGrpIdx[2] = { 0, 2 };
        int maxGrpIdx[2] = { 1, 3 };
        int* tmpMinGrp = minGrpIdx;
        int* tmpMaxGrp = maxGrpIdx;
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMinGrp[1]]) {
            xExchange(tmpMinGrp[0], tmpMinGrp[1], int);
        }
        if (iRefPointLuma[tmpMaxGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMaxGrp[0], tmpMaxGrp[1], int);
        }
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMinGrp, tmpMaxGrp, int*);
        }
        if (iRefPointLuma[tmpMinGrp[1]] > iRefPointLuma[tmpMaxGrp[0]]) {
            xExchange(tmpMinGrp[1], tmpMaxGrp[0], int);
        }

        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[1]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[1]]);

        xMin = (iRefPointLuma[tmpMinGrp[0]] + iRefPointLuma[tmpMinGrp[1]] + 1) >> 1;
        yMin = (iRefPointChroma[tmpMinGrp[0]] + iRefPointChroma[tmpMinGrp[1]] + 1) >> 1;

        xMax = (iRefPointLuma[tmpMaxGrp[0]] + iRefPointLuma[tmpMaxGrp[1]] + 1) >> 1;
        yMax = (iRefPointChroma[tmpMaxGrp[0]] + iRefPointChroma[tmpMaxGrp[1]] + 1) >> 1;
    }
    else if (bAboveAvaillable) {
        for (int k = 0; k < 2; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    else if (bLeftAvaillable) {
        for (int k = 2; k < 4; k++) {
#pragma HLS UNROLL    
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    // four points end

    if (bLeftAvaillable || bAboveAvaillable) {
        *a = 0;
        *iShift = 16;
        int diff = xMax - xMin;
        int add = 0;
        int shift = 0;
        if (diff > 0 && diff <= 64) {
            *a = ((yMax - yMin) * com_tbl_ai_tscpm_div[diff - 1] + add) >> shift;
        }
        else if (diff > 64) {
            shift = (uiInternalBitDepth > 8) ? uiInternalBitDepth - 6 : 2;
            add = shift ? 1 << (shift - 1) : 0;
            diff = (diff + add) >> shift;
            *a = ((yMax - yMin) * com_tbl_ai_tscpm_div[diff - 1] + add) >> shift;
            if (uiInternalBitDepth == 10) {
                assert(shift == 4 && add == 8); // for default 10bit
            }       
        }
        *b = yMin - (((s64)(*a) * xMin) >> (*iShift));
    }
    if (!bLeftAvaillable && !bAboveAvaillable) {
        *a = 0;
        *b = 1 << (uiInternalBitDepth - 1);
        *iShift = 0;
        return;
    }
}

static void tspcm_get_param_8(int* a, int* b, int* iShift, int bAbove, int bLeft, int uiCWidth, int uiCHeight, int bitDept, pel src_left[19],
    pel src_up[19], pel src_left_luma[35], pel src_up_luma[35])
{
#pragma HLS INLINE
    int bAboveAvaillable = bAbove;
    int bLeftAvaillable = bLeft;
    //pel* pCur = NULL;

    unsigned uiInternalBitDepth = bitDept;

    int minDim = bLeftAvaillable && bAboveAvaillable ? COM_MIN(uiCHeight, uiCWidth) : (bLeftAvaillable ? uiCHeight : uiCWidth);
    int numSteps = minDim;
    int yMax = 0;
    int xMax = -MAX_INT;
    int yMin = 0;
    int xMin = MAX_INT;

    // four points start
    int iRefPointLuma[4] = { -1, -1, -1, -1 };
    int iRefPointChroma[4] = { -1, -1, -1, -1 };
    int yMax_sec = 0;
    int xMax_sec = -MAX_INT;
    int yMin_sec = 0;
    int xMin_sec = MAX_INT;
#pragma HLS ARRAY_PARTITION variable=iRefPointLuma complete dim=0
#pragma HLS ARRAY_PARTITION variable=iRefPointChroma complete dim=0

    if (bAboveAvaillable) {
        int idx;
        if (minDim == 4) {
            idx = ((numSteps - 1) * uiCWidth) / minDim;
        }
        else if (minDim == 8) {
            idx = ((numSteps - 1) * uiCWidth) / minDim;
        }
      
        iRefPointLuma[0] = xGetSrcPixel(0, 1); // pSrc[0];
        iRefPointChroma[0] = src_up[0];
        iRefPointLuma[1] = xGetSrcPixel(idx, 1); // pSrc[idx];
        iRefPointChroma[1] = src_up[idx];
        // using 4 points when only one border
        if (!bLeftAvaillable && uiCWidth >= 4) {
            int uiStep = uiCWidth >> 2;
            for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 1); // pSrc[i * uiStep];
                iRefPointChroma[i] = src_up[i * uiStep];
            }
        }
    }
    if (bLeftAvaillable) {
        int idx;
        if (minDim == 4) {
            idx = ((numSteps - 1) * uiCHeight) / minDim;
        }
        else if (minDim == 8) {
            idx = ((numSteps - 1) * uiCHeight) / minDim;
        }

        iRefPointLuma[2] = xGetSrcPixel(0, 0); // pSrc[0];
        iRefPointChroma[2] = src_left[0];
        iRefPointLuma[3] = xGetSrcPixel(idx, 0); // pSrc[idx * iSrcStride];
        iRefPointChroma[3] = src_left[idx];
        // using 4 points when only one border
        if (!bAboveAvaillable && uiCHeight >= 4) {
            int uiStep = uiCHeight >> 2;
            for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 0); // pSrc[i * uiStep * iSrcStride];
                iRefPointChroma[i] = src_left[i * uiStep];
            }
        }
    }

    if ((bAboveAvaillable && bLeftAvaillable)
        || (bAboveAvaillable && !bLeftAvaillable && uiCWidth >= 4)
        || (bLeftAvaillable && !bAboveAvaillable && uiCHeight >= 4)) {
        int minGrpIdx[2] = { 0, 2 };
        int maxGrpIdx[2] = { 1, 3 };
        int* tmpMinGrp = minGrpIdx;
        int* tmpMaxGrp = maxGrpIdx;
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMinGrp[1]]) {
            xExchange(tmpMinGrp[0], tmpMinGrp[1], int);
        }
        if (iRefPointLuma[tmpMaxGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMaxGrp[0], tmpMaxGrp[1], int);
        }
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMinGrp, tmpMaxGrp, int*);
        }
        if (iRefPointLuma[tmpMinGrp[1]] > iRefPointLuma[tmpMaxGrp[0]]) {
            xExchange(tmpMinGrp[1], tmpMaxGrp[0], int);
        }

        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[1]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[1]]);

        xMin = (iRefPointLuma[tmpMinGrp[0]] + iRefPointLuma[tmpMinGrp[1]] + 1) >> 1;
        yMin = (iRefPointChroma[tmpMinGrp[0]] + iRefPointChroma[tmpMinGrp[1]] + 1) >> 1;

        xMax = (iRefPointLuma[tmpMaxGrp[0]] + iRefPointLuma[tmpMaxGrp[1]] + 1) >> 1;
        yMax = (iRefPointChroma[tmpMaxGrp[0]] + iRefPointChroma[tmpMaxGrp[1]] + 1) >> 1;
    }
    else if (bAboveAvaillable) {
        for (int k = 0; k < 2; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    else if (bLeftAvaillable) {
        for (int k = 2; k < 4; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    // four points end

    if (bLeftAvaillable || bAboveAvaillable) {
        *a = 0;
        *iShift = 16;
        int diff = xMax - xMin;
        int add = 0;
        int shift = 0;
        if (diff > 64) {
            shift = (uiInternalBitDepth > 8) ? uiInternalBitDepth - 6 : 2;
            add = shift ? 1 << (shift - 1) : 0;
            diff = (diff + add) >> shift;

            if (uiInternalBitDepth == 10) {
                assert(shift == 4 && add == 8); // for default 10bit
            }
        }

        if (diff > 0) {
            *a = ((yMax - yMin) * com_tbl_ai_tscpm_div[diff - 1] + add) >> shift;
        }
        *b = yMin - (((s64)(*a) * xMin) >> (*iShift));
    }
    if (!bLeftAvaillable && !bAboveAvaillable) {
        *a = 0;
        *b = 1 << (uiInternalBitDepth - 1);
        *iShift = 0;
        return;
    }
}


static void tspcm_get_param_32(int* a, int* b, int* iShift, int bAbove, int bLeft, int uiCWidth, int uiCHeight, int bitDept, pel src_left[67],
    pel src_up[67], pel src_left_luma[131], pel src_up_luma[131])
{
#pragma HLS INLINE
    int bAboveAvaillable = bAbove;
    int bLeftAvaillable = bLeft;
    //pel* pCur = NULL;

    unsigned uiInternalBitDepth = bitDept;

    int minDim = bLeftAvaillable && bAboveAvaillable ? COM_MIN(uiCHeight, uiCWidth) : (bLeftAvaillable ? uiCHeight : uiCWidth);
    int numSteps = minDim;
    int yMax = 0;
    int xMax = -MAX_INT;
    int yMin = 0;
    int xMin = MAX_INT;

    // four points start
    int iRefPointLuma[4] = { -1, -1, -1, -1 };
    int iRefPointChroma[4] = { -1, -1, -1, -1 };
    int yMax_sec = 0;
    int xMax_sec = -MAX_INT;
    int yMin_sec = 0;
    int xMin_sec = MAX_INT;

#pragma HLS ARRAY_PARTITION variable=iRefPointLuma complete dim=0
#pragma HLS ARRAY_PARTITION variable=iRefPointChroma complete dim=0
    if (bAboveAvaillable) {
        //pCur = src_up;
    	int idx;
    	if(minDim == 4){
    		idx = ((numSteps - 1) * uiCWidth) / minDim;
    	}else if(minDim == 8){
    		idx = ((numSteps - 1) * uiCWidth) / minDim;
    	}else if(minDim == 16){
    		idx = ((numSteps - 1) * uiCWidth) / minDim;
    	}else if(minDim == 32){
    		idx = ((numSteps - 1) * uiCWidth) / minDim;
    	}
        iRefPointLuma[0] = xGetSrcPixel(0, 1); // pSrc[0];
        iRefPointChroma[0] = src_up[0];
        iRefPointLuma[1] = xGetSrcPixel(idx, 1); // pSrc[idx];
        iRefPointChroma[1] = src_up[idx];
        // using 4 points when only one border
        if (!bLeftAvaillable && uiCWidth >= 4) {
            int uiStep = uiCWidth >> 2;
            for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 1); // pSrc[i * uiStep];
                iRefPointChroma[i] = src_up[i * uiStep];
            }
        }
    }
    if (bLeftAvaillable) {
        //pCur = src_left;
    	int idx;
    	if(minDim == 4){
    		idx = ((numSteps - 1) * uiCHeight) / minDim;
    	}else if( minDim == 8){
    		idx = ((numSteps - 1) * uiCHeight) / minDim;
    	}else if(minDim == 16 ){
    		idx = ((numSteps - 1) * uiCHeight) / minDim;
    	}else if(minDim == 32){
    		idx = ((numSteps - 1) * uiCHeight) / minDim;
    	}
        iRefPointLuma[2] = xGetSrcPixel(0, 0); // pSrc[0];
        iRefPointChroma[2] = src_left[0];
        iRefPointLuma[3] = xGetSrcPixel(idx, 0); // pSrc[idx * iSrcStride];
        iRefPointChroma[3] = src_left[idx];
        // using 4 points when only one border
        if (!bAboveAvaillable && uiCHeight >= 4) {
            int uiStep = uiCHeight >> 2;
            for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 0); // pSrc[i * uiStep * iSrcStride];
                iRefPointChroma[i] = src_left[i * uiStep];
            }
        }
    }

    if ((bAboveAvaillable && bLeftAvaillable)
        || (bAboveAvaillable && !bLeftAvaillable && uiCWidth >= 4)
        || (bLeftAvaillable && !bAboveAvaillable && uiCHeight >= 4)) {
        int minGrpIdx[2] = { 0, 2 };
        int maxGrpIdx[2] = { 1, 3 };
        int* tmpMinGrp = minGrpIdx;
        int* tmpMaxGrp = maxGrpIdx;
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMinGrp[1]]) {
            xExchange(tmpMinGrp[0], tmpMinGrp[1], int);
        }
        if (iRefPointLuma[tmpMaxGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMaxGrp[0], tmpMaxGrp[1], int);
        }
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMinGrp, tmpMaxGrp, int*);
        }
        if (iRefPointLuma[tmpMinGrp[1]] > iRefPointLuma[tmpMaxGrp[0]]) {
            xExchange(tmpMinGrp[1], tmpMaxGrp[0], int);
        }

        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[1]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[1]]);

        xMin = (iRefPointLuma[tmpMinGrp[0]] + iRefPointLuma[tmpMinGrp[1]] + 1) >> 1;
        yMin = (iRefPointChroma[tmpMinGrp[0]] + iRefPointChroma[tmpMinGrp[1]] + 1) >> 1;

        xMax = (iRefPointLuma[tmpMaxGrp[0]] + iRefPointLuma[tmpMaxGrp[1]] + 1) >> 1;
        yMax = (iRefPointChroma[tmpMaxGrp[0]] + iRefPointChroma[tmpMaxGrp[1]] + 1) >> 1;
    }
    else if (bAboveAvaillable) {
        for (int k = 0; k < 2; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    else if (bLeftAvaillable) {
        for (int k = 2; k < 4; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    // four points end

    if (bLeftAvaillable || bAboveAvaillable) {
        *a = 0;
        *iShift = 16;
        int diff = xMax - xMin;
        int add = 0;
        int shift = 0;
        if (diff > 64) {
            shift = (uiInternalBitDepth > 8) ? uiInternalBitDepth - 6 : 2;
            add = shift ? 1 << (shift - 1) : 0;
            diff = (diff + add) >> shift;

            if (uiInternalBitDepth == 10) {
                assert(shift == 4 && add == 8); // for default 10bit
            }
        }

        if (diff > 0) {
            *a = ((yMax - yMin) * com_tbl_ai_tscpm_div[diff - 1] + add) >> shift;
        }
        *b = yMin - (((s64)(*a) * xMin) >> (*iShift));
    }
    if (!bLeftAvaillable && !bAboveAvaillable) {
        *a = 0;
        *b = 1 << (uiInternalBitDepth - 1);
        *iShift = 0;
        return;
    }
}

static void tspcm_get_param_16(int* a, int* b, int* iShift, int bAbove, int bLeft, int uiCWidth, int uiCHeight, int bitDept, pel src_left[35],
    pel src_up[35], pel src_left_luma[67], pel src_up_luma[67])
{
    int bAboveAvaillable = bAbove;
    int bLeftAvaillable = bLeft;
    //pel* pCur = NULL;

    unsigned uiInternalBitDepth = bitDept;

    int minDim = bLeftAvaillable && bAboveAvaillable ? COM_MIN(uiCHeight, uiCWidth) : (bLeftAvaillable ? uiCHeight : uiCWidth);
    int numSteps = minDim;
    int yMax = 0;
    int xMax = -MAX_INT;
    int yMin = 0;
    int xMin = MAX_INT;

    // four points start
    int iRefPointLuma[4] = { -1, -1, -1, -1 };
    int iRefPointChroma[4] = { -1, -1, -1, -1 };
    int yMax_sec = 0;
    int xMax_sec = -MAX_INT;
    int yMin_sec = 0;
    int xMin_sec = MAX_INT;
#pragma HLS ARRAY_PARTITION variable=iRefPointLuma complete dim=0
#pragma HLS ARRAY_PARTITION variable=iRefPointChroma complete dim=0
    if (bAboveAvaillable) {
        //pCur = src_up;
        int idx;// = ((numSteps - 1) * uiCWidth) / minDim;
        if (minDim == 4) {
            idx = ((numSteps - 1) * uiCWidth) / minDim;
        }
        else if (minDim == 8) {
            idx = ((numSteps - 1) * uiCWidth) / minDim;
        }
        else if (minDim == 16) {
            idx = ((numSteps - 1) * uiCWidth) / minDim;
        }
        iRefPointLuma[0] = xGetSrcPixel(0, 1); // pSrc[0];
        iRefPointChroma[0] = src_up[0];
        iRefPointLuma[1] = xGetSrcPixel(idx, 1); // pSrc[idx];
        iRefPointChroma[1] = src_up[idx];
        // using 4 points when only one border
        if (!bLeftAvaillable && uiCWidth >= 4) {
            int uiStep = uiCWidth >> 2;
            for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 1); // pSrc[i * uiStep];
                iRefPointChroma[i] = src_up[i * uiStep];
            }
        }
    }
    if (bLeftAvaillable) {
        //pCur = src_left;
        int idx;// = ((numSteps - 1) * uiCHeight) / minDim;
        if (minDim == 4) {
            idx = ((numSteps - 1) * uiCHeight) / minDim;
        }
        else if (minDim == 8) {
            idx = ((numSteps - 1) * uiCHeight) / minDim;
        }
        else if (minDim == 16) {
            idx = ((numSteps - 1) * uiCHeight) / minDim;
        }
        iRefPointLuma[2] = xGetSrcPixel(0, 0); // pSrc[0];
        iRefPointChroma[2] = src_left[0];
        iRefPointLuma[3] = xGetSrcPixel(idx, 0); // pSrc[idx * iSrcStride];
        iRefPointChroma[3] = src_left[idx];
        // using 4 points when only one border
        if (!bAboveAvaillable && uiCHeight >= 4) {
            int uiStep = uiCHeight >> 2;
            for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
                iRefPointLuma[i] = xGetSrcPixel(i * uiStep, 0); // pSrc[i * uiStep * iSrcStride];
                iRefPointChroma[i] = src_left[i * uiStep];
            }
        }
    }

    if ((bAboveAvaillable && bLeftAvaillable)
        || (bAboveAvaillable && !bLeftAvaillable && uiCWidth >= 4)
        || (bLeftAvaillable && !bAboveAvaillable && uiCHeight >= 4)) {
        int minGrpIdx[2] = { 0, 2 };
        int maxGrpIdx[2] = { 1, 3 };
        int* tmpMinGrp = minGrpIdx;
        int* tmpMaxGrp = maxGrpIdx;
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMinGrp[1]]) {
            xExchange(tmpMinGrp[0], tmpMinGrp[1], int);
        }
        if (iRefPointLuma[tmpMaxGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMaxGrp[0], tmpMaxGrp[1], int);
        }
        if (iRefPointLuma[tmpMinGrp[0]] > iRefPointLuma[tmpMaxGrp[1]]) {
            xExchange(tmpMinGrp, tmpMaxGrp, int*);
        }
        if (iRefPointLuma[tmpMinGrp[1]] > iRefPointLuma[tmpMaxGrp[0]]) {
            xExchange(tmpMinGrp[1], tmpMaxGrp[0], int);
        }

        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[0]] >= iRefPointLuma[tmpMinGrp[1]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[0]]);
        assert(iRefPointLuma[tmpMaxGrp[1]] >= iRefPointLuma[tmpMinGrp[1]]);

        xMin = (iRefPointLuma[tmpMinGrp[0]] + iRefPointLuma[tmpMinGrp[1]] + 1) >> 1;
        yMin = (iRefPointChroma[tmpMinGrp[0]] + iRefPointChroma[tmpMinGrp[1]] + 1) >> 1;

        xMax = (iRefPointLuma[tmpMaxGrp[0]] + iRefPointLuma[tmpMaxGrp[1]] + 1) >> 1;
        yMax = (iRefPointChroma[tmpMaxGrp[0]] + iRefPointChroma[tmpMaxGrp[1]] + 1) >> 1;
    }
    else if (bAboveAvaillable) {
        for (int k = 0; k < 2; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    else if (bLeftAvaillable) {
        for (int k = 2; k < 4; k++) {
#pragma HLS UNROLL
            if (iRefPointLuma[k] > xMax) {
                xMax = iRefPointLuma[k];
                yMax = iRefPointChroma[k];
            }
            if (iRefPointLuma[k] < xMin) {
                xMin = iRefPointLuma[k];
                yMin = iRefPointChroma[k];
            }
        }
    }
    // four points end

    if (bLeftAvaillable || bAboveAvaillable) {
        *a = 0;
        *iShift = 16;
        int diff = xMax - xMin;
        int add = 0;
        int shift = 0;
        if (diff > 64) {
            shift = (uiInternalBitDepth > 8) ? uiInternalBitDepth - 6 : 2;
            add = shift ? 1 << (shift - 1) : 0;
            diff = (diff + add) >> shift;

            if (uiInternalBitDepth == 10) {
                assert(shift == 4 && add == 8); // for default 10bit
            }
        }

        if (diff > 0) {
            *a = ((yMax - yMin) * com_tbl_ai_tscpm_div[diff - 1] + add) >> shift;
        }
        *b = yMin - (((s64)(*a) * xMin) >> (*iShift));
    }
    if (!bLeftAvaillable && !bAboveAvaillable) {
        *a = 0;
        *b = 1 << (uiInternalBitDepth - 1);
        *iShift = 0;
        return;
    }
}

static void tspcm_linear_transform_4(pel pSrc[8][8], int iSrcStride, pel* pDst, int iDstStride, int a, int iShift, int b, int uiWidth, int uiHeight, int bitDept)
{
    int maxResult = (1 << bitDept) - 1;

    for (int j = 0; j < uiHeight; j++) {
        for (int i = 0; i < uiWidth; i++) {
            int tempValue = (((s64)a * pSrc[j][i]) >> (iShift >= 0 ? iShift : 0)) + b;
            pDst[i] = COM_CLIP3(0, maxResult, tempValue);
        }
        pDst += iDstStride;
        //pSrc += iSrcStride;
    }
}
static void tspcm_linear_transform_8(pel pSrc[16][16], int iSrcStride, pel* pDst, int iDstStride, int a, int iShift, int b, int uiWidth, int uiHeight, int bitDept)
{
    int maxResult = (1 << bitDept) - 1;

    for (int j = 0; j < uiHeight; j++) {
        for (int i = 0; i < uiWidth; i++) {
            int tempValue = (((s64)a * pSrc[j][i]) >> (iShift >= 0 ? iShift : 0)) + b;
            pDst[i] = COM_CLIP3(0, maxResult, tempValue);
        }
        pDst += iDstStride;
    }
}
static void tspcm_linear_transform_16(pel pSrc[32][32], int iSrcStride, pel* pDst, int iDstStride, int a, int iShift, int b, int uiWidth, int uiHeight, int bitDept)
{
    int maxResult = (1 << bitDept) - 1;
    for (int j = 0; j < uiHeight; j++) {
        for (int i = 0; i < uiWidth; i++) {
            int tempValue = (((s64)a * pSrc[j][i]) >> (iShift >= 0 ? iShift : 0)) + b;
            pDst[i] = COM_CLIP3(0, maxResult, tempValue);
        }
        pDst += iDstStride;
    }
}
static void tspcm_linear_transform_32(pel pSrc[64][64], int iSrcStride, pel* pDst, int iDstStride, int a, int iShift, int b, int uiWidth, int uiHeight, int bitDept)
{
    int maxResult = (1 << bitDept) - 1;
    for (int j = 0; j < uiHeight; j++) {
        for (int i = 0; i < uiWidth; i++) {
            int tempValue = (((s64)a * pSrc[j][i]) >> (iShift >= 0 ? iShift : 0)) + b;
            pDst[i] = COM_CLIP3(0, maxResult, tempValue);
        }
        pDst += iDstStride;
    }
}
static void tspcm_down_sample_8(pel pSrc_y[16][16], int uiCWidth, int uiCHeight, int bitDept, int uiSrcStride, pel pDst[8][8], pel pDst1[8][8], int a, int iShift, int b)
{
#pragma HLS INLINE 
    int maxResult = (1 << bitDept) - 1;
    int tempValue;
    int i, j, m;

    for (j = 0, m = 0; j < uiCHeight; j++, m = m + 2) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
        for (i = 0; i < 8; i++) {
            if (i < uiCWidth) {
                int src, src1, src2, src3, src4, src5;

                src = (((s64)a * pSrc_y[m][2 * i]) >> (iShift >= 0 ? iShift : 0)) + b;
                src = COM_CLIP3(0, maxResult, src);

                src1 = (((s64)a * pSrc_y[m+1][2 * i]) >> (iShift >= 0 ? iShift : 0)) + b;
                src1 = COM_CLIP3(0, maxResult, src1);

                if (i == 0) {
                    tempValue = (src + src1 + 1) >> 1;
                }
                else {
                    src2 = (((s64)a * pSrc_y[m][2 * i + 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                    src2 = COM_CLIP3(0, maxResult, src2);

                    src3 = (((s64)a * pSrc_y[m][2 * i - 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                    src3 = COM_CLIP3(0, maxResult, src3);

                    src4 = (((s64)a * pSrc_y[m+1][2 * i + 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                    src4 = COM_CLIP3(0, maxResult, src4);

                    src5 = (((s64)a * pSrc_y[m+1][2 * i - 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                    src5 = COM_CLIP3(0, maxResult, src5);

                    tempValue = (src * 2 + src2 + src3
                        + src1 * 2
                        + src4
                        + src5
                        + 4) >> 3;
                }

                if (tempValue > maxResult || tempValue < 0) {
                    printf("\n TSCPM clip error");
                }
                pDst[j][i] = tempValue;
                pDst1[j][i] = tempValue;
            }
        }
    }
}

static void tspcm_down_sample_16(pel pSrc_y[32][32], int uiCWidth, int uiCHeight, int bitDept, int uiSrcStride, pel pDst[16][16], pel pred[16][16], int a, int iShift, int b)
{
    int maxResult = (1 << bitDept) - 1;
    int tempValue;
    int i, j, m;
    int shift = iShift >= 0 ? iShift : 0;
    for (j = 0, m = 0; j < uiCHeight; j++, m = m + 2) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=16 max=16 avg=16
        for (i = 0; i < 16; i++) {
            if (i < uiCWidth) {
            int src, src1, src2, src3, src4, src5;

            src = (((s64)a * pSrc_y[m][2 * i]) >> (shift)) + b;
            src = COM_CLIP3(0, maxResult, src);

                src1 = (((s64)a * pSrc_y[m + 1][2 * i ]) >> (shift)) + b;
            src1 = COM_CLIP3(0, maxResult, src1);

            if (i == 0) {
                tempValue = (src + src1 + 1) >> 1;
            }
            else {
                src2 = (((s64)a * pSrc_y[m][2 * i + 1]) >> (shift)) + b;
                src2 = COM_CLIP3(0, maxResult, src2);

                src3 = (((s64)a * pSrc_y[m][2 * i - 1]) >> (shift)) + b;
                src3 = COM_CLIP3(0, maxResult, src3);

                    src4 = (((s64)a * pSrc_y[m + 1][2 * i + 1]) >> (shift)) + b;
                src4 = COM_CLIP3(0, maxResult, src4);

                    src5 = (((s64)a * pSrc_y[m + 1][2 * i - 1]) >> (shift)) + b;
                src5 = COM_CLIP3(0, maxResult, src5);

                tempValue = (src * 2 + src2 + src3
                    + src1 * 2
                    + src4
                    + src5
                    + 4) >> 3;
            }

            if (tempValue > maxResult || tempValue < 0) {
                printf("\n TSCPM clip error");
            }
            pDst[j][i] = tempValue;
            pred[j][i] = tempValue;
                //resi[j][i] = org[j][i] - tempValue;
            }
        }
    }
}

static void tspcm_down_sample_32(pel pSrc_y[64][64], int uiCWidth, int uiCHeight, int bitDept, int uiSrcStride, pel pDst[32][32], pel pDst0[32][32], int a, int iShift, int b)
{
    int maxResult = (1 << bitDept) - 1;
    int tempValue;
    int i, j, m;

    for (j = 0, m = 0; j < uiCHeight; j++, m = m + 2) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=32 max=32 avg=32
        for (i = 0; i < 32; i++) {
            if (i < uiCWidth) {
            int src, src1, src2, src3, src4, src5;

            src = (((s64)a * pSrc_y[m][2 * i]) >> (iShift >= 0 ? iShift : 0)) + b;
            src = COM_CLIP3(0, maxResult, src);

                src1 = (((s64)a * pSrc_y[m+1][2 * i]) >> (iShift >= 0 ? iShift : 0)) + b;
            src1 = COM_CLIP3(0, maxResult, src1);

            if (i == 0) {
                tempValue = (src + src1 + 1) >> 1;
            }
            else {
                src2 = (((s64)a * pSrc_y[m][2 * i + 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src2 = COM_CLIP3(0, maxResult, src2);

                src3 = (((s64)a * pSrc_y[m][2 * i - 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src3 = COM_CLIP3(0, maxResult, src3);

                    src4 = (((s64)a * pSrc_y[m+1][2 * i + 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src4 = COM_CLIP3(0, maxResult, src4);

                    src5 = (((s64)a * pSrc_y[m+1][2 * i - 1]) >> (iShift >= 0 ? iShift : 0)) + b;
                src5 = COM_CLIP3(0, maxResult, src5);

                tempValue = (src * 2 + src2 + src3
                    + src1 * 2
                    + src4
                    + src5
                    + 4) >> 3;
            }

            if (tempValue > maxResult || tempValue < 0) {
                printf("\n TSCPM clip error");
            }
            pDst[j][i] = tempValue;
                pDst0[j][i] = tempValue;
            }
        }
    }
}


void ipred_tscpm_Chroma32(pel piPred[32][32], pel piPred0[32][32], pel piRecoY[64][64], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept,
    pel src_left[67], pel src_up[67], pel src_left_luma[131], pel src_up_luma[131])
{
#pragma HLS ARRAY_PARTITION variable=piPred complete dim=2
#pragma HLS ARRAY_PARTITION variable=piPred0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=piRecoY complete dim=2
#pragma HLS ARRAY_PARTITION variable=piRecoY cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=src_left complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_left_luma complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up_luma complete dim=0
    int a, b, shift;   // parameters of Linear Model : a, b, shift
    tspcm_get_param_32(&a, &b, &shift, bAbove, bLeft, uiWidth, uiHeight, bitDept, src_left, src_up, src_left_luma, src_up_luma);

    //pel* lumaRec = piRecoY;
    int lumaRecStride = uiStrideY;
    //pel upCbCrRec[MAX_CU_SIZE * MAX_CU_SIZE];
    int upCbCrStride = MAX_CU_SIZE2;
    //tspcm_linear_transform_32(piRecoY, lumaRecStride, upCbCrRec, upCbCrStride,
        //a, shift, b, (uiWidth << 1), (uiHeight << 1), bitDept);

    //int uiStride = uiWidth;
    tspcm_down_sample_32(piRecoY, uiWidth, uiHeight, bitDept, upCbCrStride, piPred, piPred0, a, shift, b);
}

void ipred_tscpm_Chroma16(pel piPred[16][16], pel pred[16][16], pel piRecoY[32][32], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept,
    pel src_left[35], pel src_up[35], pel src_left_luma[67], pel src_up_luma[67])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=piPred complete dim=2
#pragma HLS ARRAY_PARTITION variable=piRecoY complete dim=2
#pragma HLS ARRAY_PARTITION variable=piRecoY cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=src_left complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_left_luma complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up_luma complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
//#pragma HLS ARRAY_PARTITION variable=resi complete dim=2
    int a, b, shift;   // parameters of Linear Model : a, b, shift
    tspcm_get_param_16(&a, &b, &shift, bAbove, bLeft, uiWidth, uiHeight, bitDept, src_left, src_up, src_left_luma, src_up_luma);

    //pel* lumaRec = piRecoY;
    int lumaRecStride = uiStrideY;
   // pel upCbCrRec[MAX_CU_SIZE * MAX_CU_SIZE];
    int upCbCrStride = MAX_CU_SIZE2>>1;
    //tspcm_linear_transform_16(piRecoY, lumaRecStride, upCbCrRec, upCbCrStride,
        //a, shift, b, (uiWidth << 1), (uiHeight << 1), bitDept);

    //int uiStride = uiWidth;
    tspcm_down_sample_16(piRecoY, uiWidth, uiHeight, bitDept, upCbCrStride, piPred, pred, a, shift, b );
}

void ipred_tscpm_Chroma8(pel piPred[8][8], pel piPred1[8][8], pel piRecoY[16][16], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept,
    pel src_left[19], pel src_up[19], pel src_left_luma[35], pel src_up_luma[35])
{
#pragma HLS INLINE OFF
#pragma HLS ARRAY_PARTITION variable=piPred complete dim=2
#pragma HLS ARRAY_PARTITION variable=piPred1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=piRecoY complete dim=2
#pragma HLS ARRAY_PARTITION variable=piRecoY cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=src_left complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_left_luma complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up_luma complete dim=0

    int a, b, shift;   
    tspcm_get_param_8(&a, &b, &shift, bAbove, bLeft, uiWidth, uiHeight, bitDept, src_left, src_up, src_left_luma, src_up_luma);

    int lumaRecStride = uiStrideY;
    int upCbCrStride = MAX_CU_SIZE2>>2;
    tspcm_down_sample_8(piRecoY, uiWidth, uiHeight, bitDept, upCbCrStride, piPred, piPred1, a, shift, b);
}

void ipred_tscpm_4(pel piPred[4 * 4], pel piRecoY[8][8], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept,
    pel src_left[11], pel src_up[11], pel src_left_luma[19], pel src_up_luma[19])
{
#pragma HLS INLINE 
    pel Pred[16];

#pragma HLS ARRAY_PARTITION variable=piPred complete dim=1
#pragma HLS ARRAY_PARTITION variable=Pred complete dim=1
    int a, b, shift;   // parameters of Linear Model : a, b, shift
    tspcm_get_param_4(&a, &b, &shift, bAbove, bLeft, uiWidth, uiHeight, bitDept, src_left, src_up, src_left_luma, src_up_luma);

     
    int lumaRecStride = uiStrideY;
    //pel upCbCrRec[MAX_CU_SIZE * MAX_CU_SIZE];
    int upCbCrStride = MAX_CU_SIZE2>>3;
    //tspcm_linear_transform_4(piRecoY, lumaRecStride, upCbCrRec, upCbCrStride,
       // a, shift, b, (uiWidth << 1), (uiHeight << 1), bitDept);

    //int uiStride = uiWidth;
    tspcm_down_sample_4(piRecoY, uiWidth, uiHeight, bitDept, upCbCrStride, Pred, a, shift, b);
    for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
        piPred[i] = Pred[i];

    }


}

static char tbl_ipf_pred_param[5][10] = {
    { 24,  6,  2,  0,  0,  0,  0,  0,  0,  0 }, //4x4, 24, 0.5
    { 44, 25, 14,  8,  4,  2,  1,  1,  0,  0 }, //8x8, 44-1.2
    { 40, 27, 19, 13,  9,  6,  4,  3,  2,  1 }, //16x16, 40-1.8
    { 36, 27, 21, 16, 12,  9,  7,  5,  4,  3 }, //32x32, 36-2.5
    { 52, 44, 37, 31, 26, 22, 18, 15, 13, 11 }, //64x64
};

static void ipf_core(pel* src, pel* dst, int i_dst, int ipm, int w, int h, int bit_depth)
{
    s32 filter_idx_hor = (s32)com_tbl_log2[w] - 2; //Block Size
    s32 filter_idx_ver = (s32)com_tbl_log2[h] - 2; //Block Size
    s32 ver_filter_range = COM_MIN(h, 10);
    s32 hor_filter_range = COM_MIN(w, 10);

    // TODO: tbl_ipf_pred_param doesn't support 128
    if (filter_idx_hor > 4) {
        filter_idx_hor = 4;
        hor_filter_range = 0; // don't use IPF at horizontal direction
    }
    if (filter_idx_ver > 4) {
        filter_idx_ver = 4;
        ver_filter_range = 0; // don't use IPF at vertical direction
    }

    char* filter_hori_param = tbl_ipf_pred_param[filter_idx_hor];
    char* filter_vert_param = tbl_ipf_pred_param[filter_idx_ver];

    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) { // vertical mode use left reference pixels, don't use top reference
        ver_filter_range = 0;
    }
    if (IPD_DIA_R < ipm) { // horizontal mode use top reference pixels, don't use left reference
        hor_filter_range = 0;
    }

    pel* p_top = src + 1;
    int row;

    for (row = 0; row < ver_filter_range; row++) {
        s32 coeff_top = filter_vert_param[row];
        int col;
        for (col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left - coeff_top;
            dst[col] = (coeff_left * src[-row - 1] + coeff_top * p_top[col] + coeff_cur * dst[col] + 32) >> 6;
        }
        for (; col < w; col++) {
            s32 coeff_cur = 64 - coeff_top;
            dst[col] = (coeff_top * p_top[col] + coeff_cur * dst[col] + 32) >> 6;
        }
        dst += i_dst;
    }
    for (; row < h; row++) {
        for (s32 col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left;
            dst[col] = (coeff_left * src[-row - 1] + coeff_cur * dst[col] + 32) >> 6;
        }
        dst += i_dst;
    }
}

static void ipf_core_s16(pel* src, pel* dst, int i_dst, s16* pred, int ipm, int w, int h, int bit_depth)
{
    s32 filter_idx_hor = (s32)com_tbl_log2[w] - 2; //Block Size
    s32 filter_idx_ver = (s32)com_tbl_log2[h] - 2; //Block Size
    s32 ver_filter_range = COM_MIN(h, 10);
    s32 hor_filter_range = COM_MIN(w, 10);

    int max_val = (1 << bit_depth) - 1;

    // TODO: tbl_ipf_pred_param doesn't support 128
    if (filter_idx_hor > 4) {
        filter_idx_hor = 4;
        hor_filter_range = 0; // don't use IPF at horizontal direction
    }
    if (filter_idx_ver > 4) {
        filter_idx_ver = 4;
        ver_filter_range = 0; // don't use IPF at vertical direction
    }

    char* filter_hori_param = tbl_ipf_pred_param[filter_idx_hor];
    char* filter_vert_param = tbl_ipf_pred_param[filter_idx_ver];

    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) { // vertical mode use left reference pixels, don't use top reference
        ver_filter_range = 0;
    }
    if (IPD_DIA_R < ipm) { // horizontal mode use top reference pixels, don't use left reference
        hor_filter_range = 0;
    }
    pel* p_top = src + 1;
    int row;

    for (row = 0; row < ver_filter_range; ++row) {
        s32 coeff_top = filter_vert_param[row];
        int col;
        for (col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left - coeff_top;
            s32 sample_val = (coeff_left * src[-row - 1] + coeff_top * p_top[col] + coeff_cur * pred[col] + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, sample_val);
        }
        for (; col < w; col++) {
            s32 coeff_cur = 64 - coeff_top;
            s32 sample_val = (coeff_top * p_top[col] + coeff_cur * pred[col] + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, sample_val);
        }
        dst += i_dst;
        pred += w;
    }
    for (; row < h; ++row) {
        int col;
        for (col = 0; col < hor_filter_range; col++) {
            s32 coeff_left = filter_hori_param[col];
            s32 coeff_cur = 64 - coeff_left;
            s32 sample_val = (coeff_left * src[-row - 1] + coeff_cur * pred[col] + 32) >> 6;
            dst[col] = COM_CLIP3(0, max_val, sample_val);
        }
        for (; col < w; col++) {
            dst[col] = COM_CLIP3(0, max_val, pred[col]);
        }
        dst += i_dst;
        pred += w;
    }
}

void com_intra_pred(pel* src, pel* dst, int ipm, int w, int h, int bit_depth, u16 avail_cu, u8 ipf_flag)
{
    assert(w <= 64 && h <= 64);

    if (ipm != IPD_PLN && ipm != IPD_BI) {
        switch (ipm) {
        case IPD_VER:
            avs3_funs_handle.intra_pred_ver(src + 1, dst, w, w, h);
            break;
        case IPD_HOR:
            avs3_funs_handle.intra_pred_hor(src - 1, dst, w, w, h);
            break;
        case IPD_DC:
            avs3_funs_handle.intra_pred_dc(src, dst, w, w, h, avail_cu, bit_depth);
            break;
        default:
            avs3_funs_handle.intra_pred_ang[ipm](src, dst, w, ipm, w, h);
            break;
        }
        if (ipf_flag) {
            avs3_funs_handle.intra_pred_ipf_core(src, dst, w, ipm, w, h, bit_depth);
        }
    }
    else {
        if (ipf_flag) {
            ALIGNED_32(short tmp_buf[MAX_CU_DIM]);

            switch (ipm) {
            case IPD_PLN:
                avs3_funs_handle.intra_pred_plane_ipf(src, (s16*)tmp_buf, w, h);
                break;
            case IPD_BI:
                avs3_funs_handle.intra_pred_bi_ipf(src, (s16*)tmp_buf, w, h);
                break;
            }
            avs3_funs_handle.intra_pred_ipf_core_s16(src, dst, w, (s16*)tmp_buf, ipm, w, h, bit_depth);
        }
        else {
            switch (ipm) {
            case IPD_PLN:
                avs3_funs_handle.intra_pred_plane(src, dst, w, w, h, bit_depth);
                break;
            case IPD_BI:
                avs3_funs_handle.intra_pred_bi(src, dst, w, w, h, bit_depth);
                break;
            }
        }
    }
}

void com_intra_pred_chroma(pel* dst, int ipm_c, int ipm, int w, int h, int bit_depth, u16 avail_cu, int chType, pel* piRecoY, int uiStrideY, pel nb[N_C][INTRA_NEIB_SIZE])
{
    int bAbove = IS_AVAIL(avail_cu, AVAIL_UP);
    int bLeft = IS_AVAIL(avail_cu, AVAIL_LE);
    pel* src = nb[chType] + INTRA_NEIB_MID;

    if (ipm_c == IPD_DM_C && COM_IPRED_CHK_CONV(ipm)) {
        ipm_c = COM_IPRED_CONV_L2C(ipm);
    }
    switch (ipm_c) {
    case IPD_DM_C:
        switch (ipm) {
        case IPD_PLN:
            avs3_funs_handle.intra_pred_plane(src, dst, w, w, h, bit_depth);
            break;
        default:
            avs3_funs_handle.intra_pred_ang[ipm](src, dst, w, ipm, w, h);
            break;
        }
        break;
    case IPD_DC_C:
        avs3_funs_handle.intra_pred_dc(src, dst, w, w, h, avail_cu, bit_depth);
        break;
    case IPD_HOR_C:
        avs3_funs_handle.intra_pred_hor(src - 1, dst, w, w, h);
        break;
    case IPD_VER_C:
        avs3_funs_handle.intra_pred_ver(src + 1, dst, w, w, h);
        break;
    case IPD_BI_C:
        avs3_funs_handle.intra_pred_bi(src, dst, w, w, h, bit_depth);
        break;
    case IPD_TSCPM_C: {
        //ipred_tscpm(chType, dst, piRecoY, uiStrideY, w, h, bAbove, bLeft, bit_depth, src, nb[Y_C] + INTRA_NEIB_MID);
        break;
    }
    default:
        printf("\n illegal chroma intra prediction mode\n");
        break;
    }
}

static int  getContextPixel(int uiDirMode, int uiXYflag, int iTempD, int* offset)
{
    int imult = tab_auc_dir_dxdy[uiXYflag][uiDirMode][0];
    int ishift = tab_auc_dir_dxdy[uiXYflag][uiDirMode][1];

    int iTempDn = iTempD * imult >> ishift;
    *offset = ((iTempD * imult * 32) >> ishift) - iTempDn * 32;
    return iTempDn;
}

static void ipred_ang_x(pel* src, pel* dst, int i_dst, int mode, int width, int height)
{
    int i, j;
    int offset;
    int width2 = width << 1;

    for (j = 0; j < height; j++) {
        int c1, c2, c3, c4;
        int idx = getContextPixel(mode, 0, j + 1, &offset);
        pel* p = src + idx;
        int pred_width = COM_MIN(width, width2 - idx + 1);

        c1 = 32 - offset;
        c2 = 64 - offset;
        c3 = 32 + offset;
        c4 = offset;

        for (i = 0; i < pred_width; i++, p++) {
            dst[i] = (p[0] * c1 + p[1] * c2 + p[2] * c3 + p[3] * c4 + 64) >> 7;
        }
        if (pred_width <= 0) {
            dst[0] = (src[width2] * c1 + src[width2 + 1] * c2 + src[width2 + 2] * c3 + src[width2 + 3] * c4 + 64) >> 7;
            pred_width = 1;
        }
        for (; i < width; i++) {
            dst[i] = dst[pred_width - 1];
        }
        dst += i_dst;
    }
}

static void ipred_ang_x_4(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = COM_MIN(line_size, iWidth * 2 - 1);
    int iHeight2 = iHeight * 2;
    int i;

    pSrc += 3;

    for (i = 0; i < real_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    // padding
    for (; i < line_size; i++) {
        first_line[i] = first_line[real_size - 1];
    }

    for (i = 0; i < iHeight2; i += 2) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void ipred_ang_x_6(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int real_size = COM_MIN(line_size, iWidth * 2);
    int i;

    pSrc += 2;

    for (i = 0; i < real_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    // padding
    for (; i < line_size; i++) {
        first_line[i] = first_line[real_size - 1];
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void ipred_ang_x_8(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 * (64 + 32)]);
    int line_size = iWidth + iHeight / 2 - 1;
    int real_size = COM_MIN(line_size, iWidth * 2 + 1);
    int i;
    int pad1, pad2;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel* pfirst[2] = { first_line, first_line + aligned_line_size };

    for (i = 0; i < real_size; i++, pSrc++) {
        pfirst[0][i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
        pfirst[1][i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
    }

    // padding
    if (real_size < line_size) {
        pfirst[1][real_size - 1] = pfirst[1][real_size - 2];

        pad1 = pfirst[0][real_size - 1];
        pad2 = pfirst[1][real_size - 1];
        for (; i < line_size; i++) {
            pfirst[0][i] = pad1;
            pfirst[1][i] = pad2;
        }
    }

    iHeight /= 2;

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel));
        memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel));
        dst += i_dst * 2;
    }
}

static void ipred_ang_x_10(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;
    pel* dst1 = dst;
    pel* dst2 = dst1 + i_dst;
    pel* dst3 = dst2 + i_dst;
    pel* dst4 = dst3 + i_dst;

    if (iHeight != 4) {
        ALIGNED_16(pel first_line[4 * (64 + 16)]);
        int line_size = iWidth + iHeight / 4 - 1;
        int aligned_line_size = ((line_size + 15) >> 4) << 4;
        pel* pfirst[4] = { first_line, first_line + aligned_line_size, first_line + aligned_line_size * 2, first_line + aligned_line_size * 3 };

        for (i = 0; i < line_size; i++, pSrc++) {
            pfirst[0][i] = (pSrc[0] * 3 + pSrc[1] * 7 + pSrc[2] * 5 + pSrc[3] + 8) >> 4;
            pfirst[1][i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
            pfirst[2][i] = (pSrc[0] + pSrc[1] * 5 + pSrc[2] * 7 + pSrc[3] * 3 + 8) >> 4;
            pfirst[3][i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
        }

        iHeight /= 4;

        for (i = 0; i < iHeight; i++) {
            memcpy(dst1, pfirst[0] + i, iWidth * sizeof(pel));
            memcpy(dst2, pfirst[1] + i, iWidth * sizeof(pel));
            memcpy(dst3, pfirst[2] + i, iWidth * sizeof(pel));
            memcpy(dst4, pfirst[3] + i, iWidth * sizeof(pel));
            dst1 += i_dst * 4;
            dst2 += i_dst * 4;
            dst3 += i_dst * 4;
            dst4 += i_dst * 4;
        }
    }
    else {
        for (i = 0; i < iWidth; i++, pSrc++) {
            dst1[i] = (pSrc[0] * 3 + pSrc[1] * 7 + pSrc[2] * 5 + pSrc[3] + 8) >> 4;
            dst2[i] = (pSrc[0] + (pSrc[1] + pSrc[2]) * 3 + pSrc[3] + 4) >> 3;
            dst3[i] = (pSrc[0] + pSrc[1] * 5 + pSrc[2] * 7 + pSrc[3] * 3 + 8) >> 4;
            dst4[i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
        }
    }
}

static void xPredIntraAngAdi_Y(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offset;
    int offsets[64];
    int xsteps[64];
    int iHeight2 = iHeight << 1;

    for (i = 0; i < iWidth; i++) {
        xsteps[i] = getContextPixel(uiDirMode, 1, i + 1, &offsets[i]);
    }

    for (j = 0; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            int idx = -j - xsteps[i];
            idx = COM_MAX(-iHeight2, idx);

            offset = offsets[i];
            dst[i] = (pSrc[idx] * (32 - offset) + pSrc[idx - 1] * (64 - offset) + pSrc[idx - 2] * (32 + offset) + pSrc[idx - 3] * offset + 64) >> 7;
        }
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_Y_26(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;

    if (iWidth != 4) {
        ALIGNED_16(pel first_line[64 + 256]);
        int line_size = iWidth + (iHeight - 1) * 4;
        int iHeight4 = iHeight << 2;
        for (i = 0; i < line_size; i += 4, pSrc--) {
            first_line[i] = (pSrc[0] * 3 + pSrc[-1] * 7 + pSrc[-2] * 5 + pSrc[-3] + 8) >> 4;
            first_line[i + 1] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
            first_line[i + 2] = (pSrc[0] + pSrc[-1] * 5 + pSrc[-2] * 7 + pSrc[-3] * 3 + 8) >> 4;
            first_line[i + 3] = (pSrc[-1] + pSrc[-2] * 2 + pSrc[-3] + 2) >> 2;
        }

        for (i = 0; i < iHeight4; i += 4) {
            memcpy(dst, first_line + i, iWidth * sizeof(pel));
            dst += i_dst;
        }
    }
    else {
        for (i = 0; i < iHeight; i++, pSrc--) {
            dst[0] = (pSrc[0] * 3 + pSrc[-1] * 7 + pSrc[-2] * 5 + pSrc[-3] + 8) >> 4;
            dst[1] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
            dst[2] = (pSrc[0] + pSrc[-1] * 5 + pSrc[-2] * 7 + pSrc[-3] * 3 + 8) >> 4;
            dst[3] = (pSrc[-1] + pSrc[-2] * 2 + pSrc[-3] + 2) >> 2;
            dst += i_dst;
        }
    }
}

static void xPredIntraAngAdi_Y_28(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int line_size = iWidth + (iHeight - 1) * 2;
    int real_size = COM_MIN(line_size, iHeight * 4 + 1);
    int i;
    int iHeight2 = iHeight << 1;
    int pad1, pad2;

    for (i = 0; i < real_size; i += 2, pSrc--) {
        first_line[i] = (pSrc[0] + (pSrc[-1] + pSrc[-2]) * 3 + pSrc[-3] + 4) >> 3;
        first_line[i + 1] = (pSrc[-1] + pSrc[-2] * 2 + pSrc[-3] + 2) >> 2;
    }

    // padding
    if (real_size < line_size) {
        first_line[i - 1] = first_line[i - 3];

        pad1 = first_line[i - 2];
        pad2 = first_line[i - 1];

        for (; i < line_size; i += 2) {
            first_line[i] = pad1;
            first_line[i + 1] = pad2;
        }
    }

    for (i = 0; i < iHeight2; i += 2) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_Y_30(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int real_size = COM_MIN(line_size, iHeight * 2);
    int i;

    pSrc -= 2;

    for (i = 0; i < real_size; i++, pSrc--) {
        first_line[i] = (pSrc[1] + pSrc[0] * 2 + pSrc[-1] + 2) >> 2;
    }

    // padding
    for (; i < line_size; i++) {
        first_line[i] = first_line[real_size - 1];
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, first_line + i, iWidth * sizeof(pel));
        dst += i_dst;
    }
}

static void xPredIntraAngAdi_Y_32(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 * (32 + 64)]);
    int line_size = iHeight / 2 + iWidth - 1;
    int real_size = COM_MIN(line_size, iHeight);
    int i;
    pel pad_val;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel* pfirst[2] = { first_line, first_line + aligned_line_size };

    pSrc -= 3;

    for (i = 0; i < real_size; i++, pSrc -= 2) {
        pfirst[0][i] = (pSrc[1] + pSrc[0] * 2 + pSrc[-1] + 2) >> 2;
        pfirst[1][i] = (pSrc[0] + pSrc[-1] * 2 + pSrc[-2] + 2) >> 2;
    }

    // padding
    pad_val = pfirst[1][i - 1];
    for (; i < line_size; i++) {
        pfirst[0][i] = pad_val;
        pfirst[1][i] = pad_val;
    }

    iHeight /= 2;
    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst[0] + i, iWidth * sizeof(pel));
        memcpy(dst + i_dst, pfirst[1] + i, iWidth * sizeof(pel));
        dst += i_dst * 2;
    }
}

static void ipred_ang_xy(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    int offsetx, offsety;
    pel* rpSrc = pSrc;
    int xoffsets[64];
    int xsteps[64];

    for (i = 0; i < iWidth; i++) {
        xsteps[i] = getContextPixel(uiDirMode, 1, i + 1, &xoffsets[i]);
    }

    for (j = 0; j < iHeight; j++) {
        pel* px = pSrc - getContextPixel(uiDirMode, 0, j + 1, &offsetx);

        for (i = 0; i < iWidth; i++, px++) {
            int iYy = j - xsteps[i];
            if (iYy <= -1) {
                dst[i] = (px[2] * (32 - offsetx) + px[1] * (64 - offsetx) + px[0] * (32 + offsetx) + px[-1] * offsetx + 64) >> 7;
            }
            else {
                offsety = xoffsets[i];
                dst[i] = (rpSrc[-iYy - 2] * (32 - offsety) + rpSrc[-iYy - 1] * (64 - offsety) + rpSrc[-iYy] * (32 + offsety) + rpSrc[-iYy + 1] * offsety + 64) >> 7;
            }
        }
        dst += i_dst;
    }
}

static void ipred_ang_xy_13(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel* rpSrc = pSrc;
    pel* dst_base = dst - 8 * i_dst - 1;
    int step1_height = COM_MIN(iHeight, 7);

    for (j = 0; j < step1_height; j++) {
        int iTempD = j + 1;
        pel* p = pSrc - (iTempD >> 3);
        int offsetx = (iTempD << 2) & 0x1f;
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        for (i = 0; i < iWidth; i++, p++) {
            dst[i] = (p[2] * a + p[1] * b + p[0] * c + p[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        int iTempD = j + 1;
        int step1_width = (int)(((j + 1) / 8.0 + 0.9999)) - 1;
        int offsetx = (iTempD << 2) & 0x1f;
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        pel* px = pSrc + step1_width - (iTempD >> 3);

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel* py = rpSrc - j + ((i + 1) << 3);
            dst[i] = (py[-2] + (py[-1] << 1) + py[0] + 2) >> 2;
        }
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_14(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;

    if (iHeight != 4) {
        ALIGNED_16(pel first_line[4 * (64 + 16)]);
        int line_size = iWidth + iHeight / 4 - 1;
        int left_size = line_size - iWidth;
        int aligned_line_size = ((line_size + 15) >> 4) << 4;
        pel* pfirst[4] = { first_line, first_line + aligned_line_size, first_line + aligned_line_size * 2, first_line + aligned_line_size * 3 };

        pSrc -= iHeight - 4;
        for (i = 0; i < left_size; i++, pSrc += 4) {
            pfirst[0][i] = (pSrc[2] + pSrc[3] * 2 + pSrc[4] + 2) >> 2;
            pfirst[1][i] = (pSrc[1] + pSrc[2] * 2 + pSrc[3] + 2) >> 2;
            pfirst[2][i] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
            pfirst[3][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }

        for (; i < line_size; i++, pSrc++) {
            pfirst[0][i] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            pfirst[1][i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            pfirst[2][i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            pfirst[3][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }

        pfirst[0] += left_size;
        pfirst[1] += left_size;
        pfirst[2] += left_size;
        pfirst[3] += left_size;

        iHeight /= 4;

        for (i = 0; i < iHeight; i++) {
            memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel));
            dst += i_dst;
            memcpy(dst, pfirst[1] - i, iWidth * sizeof(pel));
            dst += i_dst;
            memcpy(dst, pfirst[2] - i, iWidth * sizeof(pel));
            dst += i_dst;
            memcpy(dst, pfirst[3] - i, iWidth * sizeof(pel));
            dst += i_dst;
        }
    }
    else {
        pel* dst1 = dst;
        pel* dst2 = dst1 + i_dst;
        pel* dst3 = dst2 + i_dst;
        pel* dst4 = dst3 + i_dst;

        for (i = 0; i < iWidth; i++, pSrc++) {
            dst1[i] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            dst2[i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            dst3[i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            dst4[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }
    }
}

static void ipred_ang_xy_15(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel* rpSrc = pSrc;
    pel* dst_base = dst - 11 * i_dst - 4;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 11;

    for (i = 0; i < iWidth; i++, iTempD += 11) {
        xoffsets[i] = (iTempD << 3) & 0x1f;
        xsteps[i] = (iTempD >> 2);
    }

    iTempD = 93;

    for (j = 0; j < 2; j++, iTempD += 93) {
        int offsetx = (iTempD >> 3) & 0x1f;
        pel* px = pSrc - (iTempD >> 8);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        for (i = 0; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++, iTempD += 93) {
        int setp1_width = (int)(((j + 1) << 2) / 11.0 + 0.9999) - 1;
        int offsetx = (iTempD >> 3) & 0x1f;
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        pel* px = pSrc - (iTempD >> 8);

        setp1_width = COM_MIN(setp1_width, iWidth);

        for (i = 0; i < setp1_width; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }

        px += i;

        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_16(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[2 * (64 + 32)]);
    int line_size = iWidth + iHeight / 2 - 1;
    int left_size = line_size - iWidth;
    int aligned_line_size = ((line_size + 15) >> 4) << 4;
    pel* pfirst[2] = { first_line, first_line + aligned_line_size };

    int i;

    pSrc -= iHeight - 2;

    for (i = 0; i < left_size; i++, pSrc += 2) {
        pfirst[0][i] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
        pfirst[1][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (; i < line_size; i++, pSrc++) {
        pfirst[0][i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
        pfirst[1][i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    pfirst[0] += left_size;
    pfirst[1] += left_size;

    iHeight /= 2;

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst[0] - i, iWidth * sizeof(pel));
        memcpy(dst + i_dst, pfirst[1] - i, iWidth * sizeof(pel));
        dst += 2 * i_dst;
    }
}

static void ipred_ang_xy_17(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel* rpSrc = pSrc;
    pel* dst_base = dst - 11 * i_dst - 8;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 11;

    for (i = 0; i < iWidth; i++, iTempD += 11) {
        xoffsets[i] = (iTempD << 2) & 0x1f;
        xsteps[i] = iTempD >> 3;
    }

    iTempD = 93;

    {
        int offsetx = (iTempD >> 2) & 0x1f;
        pel* px = pSrc - (iTempD >> 7);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;
        for (i = 0; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
        iTempD += 93;
    }

    for (j = 1; j < iHeight; j++, iTempD += 93) {
        int setp1_width = (int)(((j + 1) << 3) / 11.0 + 0.9999) - 1;
        int offsetx = (iTempD >> 2) & 0x1f;
        pel* px = pSrc - (iTempD >> 7);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        setp1_width = COM_MIN(setp1_width, iWidth);

        for (i = 0; i < setp1_width; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        px += i;
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_18(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 64]);
    int line_size = iWidth + iHeight - 1;
    int i;
    pel* pfirst = first_line + iHeight - 1;

    pSrc -= iHeight - 1;

    for (i = 0; i < line_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst, iWidth * sizeof(pel));
        pfirst--;
        dst += i_dst;
    }
}

static void ipred_ang_xy_19(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel* rpSrc = pSrc;
    pel* dst_base = dst - 8 * i_dst - 11;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 93;
    int step2_height = ((93 * iWidth) >> 7);

    for (i = 0; i < iWidth; i++, iTempD += 93) {
        xoffsets[i] = (iTempD >> 2) & 0x1f;
        xsteps[i] = iTempD >> 7;
    }

    iTempD = 11;

    step2_height = COM_MIN(step2_height, iHeight);

    for (j = 0; j < step2_height; j++, iTempD += 11) {
        int step1_width = (int)(((j + 1) << 7) / 93.0 + 0.9999) - 1;
        int offsetx = (iTempD << 2) & 0x1f;
        pel* px = pSrc - (iTempD >> 3);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        px += i;

        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_20(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    ALIGNED_16(pel first_line[64 + 128]);
    int left_size = (iHeight - 1) * 2 + 1;
    int top_size = iWidth - 1;
    int line_size = left_size + top_size;
    int i;
    pel* pfirst = first_line + left_size - 1;

    pSrc -= iHeight;

    for (i = 0; i < left_size; i += 2, pSrc++) {
        first_line[i] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
        first_line[i + 1] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
    }
    i--;

    for (; i < line_size; i++, pSrc++) {
        first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
    }

    for (i = 0; i < iHeight; i++) {
        memcpy(dst, pfirst, iWidth * sizeof(pel));
        pfirst -= 2;
        dst += i_dst;
    }
}

static void ipred_ang_xy_21(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel* rpSrc = pSrc;
    pel* dst_base = dst - 4 * i_dst - 11;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 93;
    int step2_height = (93 * iWidth) >> 8;

    for (i = 0; i < iWidth; i++, iTempD += 93) {
        xoffsets[i] = (iTempD >> 3) & 0x1f;
        xsteps[i] = iTempD >> 8;
    }

    iTempD = 11;
    step2_height = COM_MIN(step2_height, iHeight);

    for (j = 0; j < step2_height; j++, iTempD += 11) {
        int step1_width = (int)(((j + 1) << 8) / 93.0 + 0.9999) - 1;
        int offsetx = (iTempD << 3) & 0x1f;
        pel* px = pSrc - (iTempD >> 2);
        int a = 32 - offsetx, b = 64 - offsetx, c = 32 + offsetx;

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }

        px += i;

        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] * a + px[1] * b + px[0] * c + px[-1] * offsetx + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

static void ipred_ang_xy_22(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i;
    pSrc -= iHeight;

    if (iWidth != 4) {
        ALIGNED_16(pel first_line[64 + 256]);
        int left_size = (iHeight - 1) * 4 + 3;
        int top_size = iWidth - 3;
        int line_size = left_size + top_size;
        pel* pfirst = first_line + left_size - 3;
        for (i = 0; i < left_size; i += 4, pSrc++) {
            first_line[i] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            first_line[i + 1] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            first_line[i + 2] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            first_line[i + 3] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
        }
        i--;

        for (; i < line_size; i++, pSrc++) {
            first_line[i] = (pSrc[-1] + pSrc[0] * 2 + pSrc[1] + 2) >> 2;
        }

        for (i = 0; i < iHeight; i++) {
            memcpy(dst, pfirst, iWidth * sizeof(pel));
            dst += i_dst;
            pfirst -= 4;
        }
    }
    else {
        dst += (iHeight - 1) * i_dst;
        for (i = 0; i < iHeight; i++, pSrc++) {
            dst[0] = (pSrc[-1] * 3 + pSrc[0] * 7 + pSrc[1] * 5 + pSrc[2] + 8) >> 4;
            dst[1] = (pSrc[-1] + (pSrc[0] + pSrc[1]) * 3 + pSrc[2] + 4) >> 3;
            dst[2] = (pSrc[-1] + pSrc[0] * 5 + pSrc[1] * 7 + pSrc[2] * 3 + 8) >> 4;
            dst[3] = (pSrc[0] + pSrc[1] * 2 + pSrc[2] + 2) >> 2;
            dst -= i_dst;
        }
    }
}

static void ipred_ang_xy_23(pel* pSrc, pel* dst, int i_dst, int uiDirMode, int iWidth, int iHeight)
{
    int i, j;
    pel* rpSrc = pSrc;
    pel* dst_base = dst - i_dst - 8;
    int xoffsets[64];
    int xsteps[64];
    int iTempD = 1;
    int step2_height = (iWidth >> 3);

    for (i = 0; i < iWidth; i++, iTempD++) {
        xoffsets[i] = (iTempD << 2) & 0x1f;
        xsteps[i] = iTempD >> 3;
    }

    iTempD = 8;

    for (j = 0; j < step2_height; j++, iTempD += 8) {
        int step1_width = ((j + 1) << 3) - 1;
        pel* px = pSrc - iTempD;

        step1_width = COM_MIN(step1_width, iWidth);

        for (i = 0; i < step1_width; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        px += i;
        for (; i < iWidth; i++, px++) {
            dst[i] = (px[2] + (px[1] << 1) + px[0] + 2) >> 2;
        }
        dst_base += i_dst;
        dst += i_dst;
    }

    for (; j < iHeight; j++) {
        for (i = 0; i < iWidth; i++) {
            pel* py = rpSrc - j + xsteps[i];
            int offsety = xoffsets[i];
            dst[i] = (py[-2] * (32 - offsety) + py[-1] * (64 - offsety) + py[0] * (32 + offsety) + py[1] * offsety + 64) >> 7;
        }
        dst_base += i_dst;
        dst += i_dst;
    }
}

void uavs3e_funs_init_intra_pred_c()
{
    int i;

    for (i = IPD_BI + 1; i < IPD_VER; i++) {
        avs3_funs_handle.intra_pred_ang[i] = ipred_ang_x;
    }
    for (i = IPD_VER; i < IPD_HOR; i++) {
        avs3_funs_handle.intra_pred_ang[i] = ipred_ang_xy;
    }
    for (i = IPD_HOR + 1; i < IPD_CNT; i++) {
        avs3_funs_handle.intra_pred_ang[i] = xPredIntraAngAdi_Y;
    }

    avs3_funs_handle.intra_pred_ang[4] = ipred_ang_x_4;
    avs3_funs_handle.intra_pred_ang[6] = ipred_ang_x_6;
    avs3_funs_handle.intra_pred_ang[8] = ipred_ang_x_8;
    avs3_funs_handle.intra_pred_ang[10] = ipred_ang_x_10;

    avs3_funs_handle.intra_pred_ang[13] = ipred_ang_xy_13;
    avs3_funs_handle.intra_pred_ang[15] = ipred_ang_xy_15;
    avs3_funs_handle.intra_pred_ang[17] = ipred_ang_xy_17;
    avs3_funs_handle.intra_pred_ang[19] = ipred_ang_xy_19;
    avs3_funs_handle.intra_pred_ang[21] = ipred_ang_xy_21;
    avs3_funs_handle.intra_pred_ang[23] = ipred_ang_xy_23;

    avs3_funs_handle.intra_pred_ang[14] = ipred_ang_xy_14;
    avs3_funs_handle.intra_pred_ang[16] = ipred_ang_xy_16;
    avs3_funs_handle.intra_pred_ang[18] = ipred_ang_xy_18;
    avs3_funs_handle.intra_pred_ang[20] = ipred_ang_xy_20;
    avs3_funs_handle.intra_pred_ang[22] = ipred_ang_xy_22;

    avs3_funs_handle.intra_pred_ang[26] = xPredIntraAngAdi_Y_26;
    avs3_funs_handle.intra_pred_ang[28] = xPredIntraAngAdi_Y_28;
    avs3_funs_handle.intra_pred_ang[30] = xPredIntraAngAdi_Y_30;
    avs3_funs_handle.intra_pred_ang[32] = xPredIntraAngAdi_Y_32;

    //avs3_funs_handle.intra_pred_dc        = ipred_dc;
    //avs3_funs_handle.intra_pred_bi        = ipred_bi;
    //avs3_funs_handle.intra_pred_plane     = ipred_plane;
    avs3_funs_handle.intra_pred_hor = ipred_hor;
    avs3_funs_handle.intra_pred_ver = ipred_vert;
    avs3_funs_handle.intra_pred_bi_ipf = ipred_bi_ipf;
    avs3_funs_handle.intra_pred_plane_ipf = ipred_plane_ipf;
    avs3_funs_handle.intra_pred_ipf_core = ipf_core;
    avs3_funs_handle.intra_pred_ipf_core_s16 = ipf_core_s16;
}

#if INTERPF && 0

static const s32 g_ipf_pred_param[5][10] =
{
    { 24,  6,  2,  0,  0,  0,  0,  0,  0,  0 }, //4x4, 24, 0.5
    { 44, 25, 14,  8,  4,  2,  1,  1,  0,  0 }, //8x8, 44-1.2
    { 40, 27, 19, 13,  9,  6,  4,  3,  2,  1 }, //16x16, 40-1.8
    { 36, 27, 21, 16, 12,  9,  7,  5,  4,  3 }, //32x32, 36-2.5
    { 52, 44, 37, 31, 26, 22, 18, 15, 13, 11 }, //64x64
};

void ipf_core_forInterPF(pel* src_le, pel* src_up, pel* dst, int ipm, int w, int h)
{
    com_assert((MIN_CU_SIZE <= w) && (MIN_CU_SIZE <= h));
    com_assert(ipm < IPD_CNT);
    assert(com_tbl_log2[w] >= 2);
    assert(com_tbl_log2[h] >= 2);

    s32 filter_idx_hor = (s32)com_tbl_log2[w] - 2; //Block Size
    s32 filter_idx_ver = (s32)com_tbl_log2[h] - 2; //Block Size
    const s32 filter_range = 10;
    s32 ver_filter_range = filter_range;
    s32 hor_filter_range = filter_range;

    // TODO: g_ipf_pred_param doesn't support 128
    if (filter_idx_hor > 4)
    {
        filter_idx_hor = 4;
        hor_filter_range = 0; // don't use IPF at horizontal direction
    }
    if (filter_idx_ver > 4)
    {
        filter_idx_ver = 4;
        ver_filter_range = 0; // don't use IPF at vertical direction
    }

    const s32* filter_hori_param = g_ipf_pred_param[filter_idx_hor];
    const s32* filter_vert_param = g_ipf_pred_param[filter_idx_ver];
    const s32 par_shift = 6; //normalization factor
    const s32 par_scale = 1 << par_shift;
    const s32 par_offset = 1 << (par_shift - 1);

#if EIPM
    if ((IPD_DIA_L <= ipm && ipm <= IPD_DIA_R) || (34 <= ipm && ipm <= 50))
#else
    if (IPD_DIA_L <= ipm && ipm <= IPD_DIA_R)
#endif
    {
        // vertical mode use left reference pixels, don't use top reference
        ver_filter_range = 0;
    }
#if EIPM
    if ((ipm > IPD_DIA_R && ipm < IPD_IPCM) || (ipm > IPD_DIA_R_EXT && ipm < IPD_CNT))
#else
    if (IPD_DIA_R < ipm)
#endif
    {
        // horizontal mode use top reference pixels, don't use left reference
        hor_filter_range = 0;
    }

    s32 p_ref_lenth = w + h;
    s32* p_ref_vector = (s32*)com_malloc(p_ref_lenth * sizeof(s32));
    com_mset(p_ref_vector, 0, (w + h) * sizeof(s32));
    s32* p_ref_vector_h = p_ref_vector + h;
    for (s32 i = 0; i < w; ++i)
    {
        p_ref_vector_h[i] = src_up[i];
    }
    for (s32 i = 1; i <= h; ++i)
    {
        p_ref_vector_h[-i] = src_le[i - 1];
    }

    for (s32 row = 0; row < h; ++row)
    {
        s32 pos = row * w;
        s32 coeff_top = (row < ver_filter_range) ? filter_vert_param[row] : 0;
        for (s32 col = 0; col < w; col++, pos++)
        {
            s32 coeff_left = (col < hor_filter_range) ? filter_hori_param[col] : 0;
            s32 coeff_cur = par_scale - coeff_left - coeff_top;
            s32 sample_val = (coeff_left * p_ref_vector_h[-row - 1] + coeff_top * p_ref_vector_h[col] + coeff_cur * dst[pos] + par_offset) >> par_shift;
            dst[pos] = sample_val;
        }
    }

    // Release memory
    com_mfree(p_ref_vector);
}

void clip_pred_forInterPF(pel* dst, const int w, const int h, int bit_depth, int stride)
{
    com_assert(NULL != dst);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            dst[i * stride + j] = COM_CLIP3(0, ((1 << bit_depth) - 1), dst[i * stride + j]);
        }
    }
}

void com_inter_filter(pel* src_le, pel* src_up, pel* dst, int w, int h, int bit_depth, u16 avail_cu, int pfIdx, int stride)
{
    int coef_h = 0, coef_v = 0;
    int x, y;
    int wintra = 3;
    int winter = 5;
    int log2_w = com_tbl_log2[w];
    int log2_h = com_tbl_log2[h];

#if FIX_335
    pel* dst_start = dst;
#endif
    if (pfIdx == 2)
    {
        ipf_core_forInterPF(src_le, src_up, dst, IPD_BI, w, h);/*  component=Y/U_C/V_C  */
    }
    else
    {
        for (y = 0; y < h; y++)
        {
            for (x = 0; x < w; x++)
            {
                int predV = ((h - 1 - y) * src_up[x] + (y + 1) * src_le[h] + (h >> 1)) >> log2_h;
                int predH = ((w - 1 - x) * src_le[y] + (x + 1) * src_up[w] + (w >> 1)) >> log2_w;
                int predP1 = (predV + predH + 1) >> 1;

                dst[x] = (pel)((dst[x] * winter + predP1 * wintra + 4) >> 3);
            }
            dst += stride;
        }
    }
#if FIX_335
    clip_pred_forInterPF(dst_start, w, h, bit_depth, stride);
#else
    clip_pred(dst, w, h, bit_depth);
#endif
}




static void com_get_nbr_forInterPF(int x, int y, int width, int height, pel* src, int s_src, u16 avail_cu, pel nb[N_C][N_REF][MAX_CU_SIZE * 3], int scup, u32* map_scu, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int ch_type)
{
    int  i;
    int  width_in_scu = (ch_type == Y_C) ? (width >> MIN_CU_LOG2) : (width >> (MIN_CU_LOG2 - 1));
    int  height_in_scu = (ch_type == Y_C) ? (height >> MIN_CU_LOG2) : (height >> (MIN_CU_LOG2 - 1));
    int  unit_size = (ch_type == Y_C) ? MIN_CU_SIZE : (MIN_CU_SIZE >> 1);
    int  x_scu = PEL2SCU(ch_type == Y_C ? x : x << 1);
    int  y_scu = PEL2SCU(ch_type == Y_C ? y : y << 1);
    pel* const src_bak = src;
    pel* left = nb[ch_type][0] + STNUM;
    pel* up = nb[ch_type][1] + STNUM;
    int pad_le = height;  //number of padding pixel in the left column
    int pad_up = width;   //number of padding pixel in the upper row
    int pad_le_in_scu = height_in_scu;
    int pad_up_in_scu = width_in_scu;

    com_mset_16b(left - STNUM, 1 << (bit_depth - 1), height + pad_le + STNUM);
    com_mset_16b(up - STNUM, 1 << (bit_depth - 1), width + pad_up + STNUM);
    if (IS_AVAIL(avail_cu, AVAIL_UP))
    {
        com_mcpy(up, src - s_src, width * sizeof(pel));
        for (i = 0; i < pad_up_in_scu; i++)
        {
            if (x_scu + width_in_scu + i < pic_width_in_scu && MCU_GET_CODED_FLAG(map_scu[scup - pic_width_in_scu + width_in_scu + i]))
            {
                com_mcpy(up + width + i * unit_size, src - s_src + width + i * unit_size, unit_size * sizeof(pel));
            }
            else
            {
                com_mset_16b(up + width + i * unit_size, up[width + i * unit_size - 1], unit_size);
            }
        }
    }

    if (IS_AVAIL(avail_cu, AVAIL_LE))
    {
        src--;
        for (i = 0; i < height; ++i)
        {
            left[i] = *src;
            src += s_src;
        }
        for (i = 0; i < pad_le_in_scu; i++)
        {
            if (y_scu + height_in_scu + i < pic_height_in_scu && MCU_GET_CODED_FLAG(map_scu[scup - 1 + (height_in_scu + i) * pic_width_in_scu]))
            {
                int j;
                for (j = 0; j < unit_size; ++j)
                {
                    left[height + i * unit_size + j] = *src;
                    src += s_src;
                }
            }
            else
            {
                com_mset_16b(left + height + i * unit_size, left[height + i * unit_size - 1], unit_size);
                src += (s_src * unit_size);
            }
        }
    }

    if (IS_AVAIL(avail_cu, AVAIL_UP_LE))
    {
        up[-1] = left[-1] = src_bak[-s_src - 1];
    }
    else if (IS_AVAIL(avail_cu, AVAIL_UP))
    {
        up[-1] = left[-1] = up[0];
    }
    else if (IS_AVAIL(avail_cu, AVAIL_LE))
    {
        up[-1] = left[-1] = left[0];
    }

    up[-2] = left[0];
    left[-2] = up[0];
    up[-3] = left[1];
    left[-3] = up[1];
#if IIP
    if (STNUM > 3)
    {
        up[-4] = left[2];
        left[-4] = up[2];
        up[-5] = left[3];
        left[-5] = up[3];
    }
#endif
}

u16 com_get_avail_intra_forInterPF(IP_MAP_SCU* MAP_SCU_IP_ptr, U11 x_scu, U11 y_scu)
{

#pragma HLS INLINE off
    u16 avail = (u16)0;
    U11 pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
    U11 pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
    if (x_scu == MAP_SCU_IP_ptr->min_scu_x) {
        if (x_scu > 0
            && ((MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu] >> 31 & (1)))) {
            avail |= 1 << 1;
        }
    }
    else {
        if (x_scu > 0
            && ((MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu] >> 31 & (1)))) {
            avail |= 1 << 1;
        }
    }
    if (y_scu > 0) {
        if (y_scu == MAP_SCU_IP_ptr->min_scu_y) {
            if ((MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu] >> 31 & (1))) {
                avail |= 1 << 0;
            }
        }
        else {
            if ((MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1] >> 31 & (1))) {
                avail |= 1 << 0;
            }
        }
    }
    if (y_scu == MAP_SCU_IP_ptr->min_scu_y) {
        if (x_scu > 0)
            if (x_scu > 0
                && ((MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu - 1] >> 31 & (1)))) {
                avail |= 1 << 2;
            }
    }
    else if (x_scu == MAP_SCU_IP_ptr->min_scu_x) {
        if (x_scu > 0
            && ((MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu - 1] >> 31 & (1)))) {
            avail |= 1 << 2;
        }
    }
    else {
        if (x_scu > 0
            && ((MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu - 1] >> 31 & (1)))) {
            avail |= 1 << 2;
        }
    }
    return avail;
}

static S7 get_ip_array_size_idx_forIPF(U8 cu_width, U8 cu_height) {
    static S7 ip_array_size_idx[16][16] = {
        { 0, 4, -1, 9, -1, -1, -1, 14, -1, -1, -1, -1, -1, -1, -1 },
        { 1, 5, -1, 10, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, 19 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 2, 6, -1, 11, -1, -1, -1, 16, -1, -1, -1, -1, -1, -1, 20 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { 3, 7, -1, 12, -1, -1, -1, 17, -1, -1, -1, -1, -1, -1, 21 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
        { -1, 8, -1, 13, -1, -1, -1, 18, -1, -1, -1, -1, -1, -1, 22 }
    };
    return ip_array_size_idx[(cu_width >> 2) - 1][(cu_height >> 2) - 1];
}

void get_IP_pixel_forIPF(U7 w, U7 h, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
    U4 bit_depth, U2 ch_type, pel left[MAX_CU_SIZE * 3], pel up[MAX_CU_SIZE * 3], U14 x, U14 y, ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
    int i;
    int j;
    int width_in_scu = ch_type == 0 ? w >> 2 : w >> (2 - 1);
    int height_in_scu = ch_type == 0 ? h >> 2 : h >> (2 - 1);
    //4
    U4 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
    U2 unit_size_bit = ch_type == 0 ? 2 : 1;
    //x/4
    int x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
    //y/4
    int y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
    //number of padding pixel in the left column   64
    int pad_le = h;
    //number of padding pixel in the upper row     64
    int pad_up = w;
    //16
    int pad_le_in_scu = height_in_scu;
    //16
    int pad_up_in_scu = width_in_scu;
    int pic_x = x % 64;
    int pic_y = y % 64;
    int pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
    int pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
    int flag;

    S7 ip_array_size_idx = get_ip_array_size_idx_forIPF(w, h);
#if MD_PRINT
    assert(ip_array_size_idx != -1);
#endif

    if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
        if (pic_y == 0) {
            for (i = 0; i < w; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                up[i + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + i];

            }
            for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (x_scu + width_in_scu + i < pic_width_in_scu) {
                    flag = ((int)(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu + width_in_scu + i] >> 31 & ((
                        unsigned int)1)));
                }
                else {
                    flag = 0;
                }
                if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + w + (i << unit_size_bit) + j];
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
        else {
            for (i = 0; i < w; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                up[i + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + i];
            }
            for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_x_scu + width_in_scu + i < 16) {
                    flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1]
                        >> 31 & ((unsigned int)1)));
                }
                else {
                    flag = 0;
                }
                if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + w + (i << unit_size_bit) + j];
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
    }
    if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
        if (pic_x == 0) {
            for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                left[i + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + i];
            }
            for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_y_scu + height_in_scu + i < 16) {
                    flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31
                        & ((unsigned int)1)));
                }
                else {
                    flag = 0;
                }
                if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + h + (i << unit_size_bit) + j];

                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
        else {
            for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                left[i + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][pic_y + i][(pic_x >> 2) - 1];

            }
            for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_y_scu + height_in_scu + i < 16) {
                    flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i]
                        >> 31 & ((unsigned int)1)));
                }
                else {
                    flag = 0;
                }
                if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][h + (i << unit_size_bit) + j + pic_y][(pic_x >> 2) - 1];
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
    }
    if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
        if (pic_y == 0 && pic_x == 0) {
            up[2] = left[2] = ((pel)(IP_buffer_ptr->top_left));
        }
        else if (pic_y == 0) {
            up[2] = left[2] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x - 1];
        }
        else if (pic_x == 0) {
            up[2] = left[2] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y - 1];
        }
        else {
            up[2] = left[2] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x - 1];

        }
    }
    else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
        up[2] = left[2] = up[3];
    }
    else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
        up[2] = left[2] = left[3];
    }
    up[1] = left[3];
    left[1] = up[3];
    up[0] = left[4];
    left[0] = up[4];
}

static __inline void com_mset_8b(pel* dst, pel v, int cnt)
{
#pragma HLS INLINE off
    int i;
    for (i = 0; i < cnt; i++)
    {
#pragma HLS LOOP_TRIPCOUNT max=67 
        dst[i] = v;
    }
}

void get_IP_pixel_chroma_forIPF(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu, U4 bit_depth, U2 ch_type, pel* left, pel* up, U12 x, U12 y, ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr)
{
    int i;
    int j;
    U6 width_in_scu = ch_type == 0 ? width >> 2 : width >> (2 - 1);
    U6 height_in_scu = ch_type == 0 ? height >> 2 : height >> (2 - 1);
    //4
    U3 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
    U2 unit_size_bit = ch_type == 0 ? 2 : 1;
    //x/4
    U11 x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
    //y/4
    U11 y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
    //number of padding pixel in the left column   32
    U7 pad_le = height;
    //number of padding pixel in the upper row     32
    U7 pad_up = width;
    //16
    U6 pad_le_in_scu = height_in_scu;
    //16
    U6 pad_up_in_scu = width_in_scu;
    U6 pic_x = x % 32;
    U6 pic_y = y % 32;
    com_mset_8b(left, (1 << (bit_depth - 1)), height + pad_le + 3);
    com_mset_8b(up, (1 << (bit_depth - 1)), width + pad_up + 3);
    U11 pic_x_scu = x_scu - me_line_ptr->min_scu_x;
    U11 pic_y_scu = y_scu - me_line_ptr->min_scu_y;
    U1 flag = 0;
    if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
        if (pic_y == 0) {
            for (i = 0; i < width; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                up[i + 3] = (IP_buffer_ptr->up_chroma[ch_type - 1][x + i]);
            }
            for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                //				assert(MCU_GET_CODED_FLAG(map_scu[scup - pic_width_in_scu + width_in_scu + i]) == 1);
                if (x_scu + width_in_scu + i < pic_width_in_scu) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->up_chroma[ch_type - 1][x + width + (i << unit_size_bit) + j]);
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
        else {
            for (i = 0; i < width; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                up[i + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + i]);
            }
            for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_x_scu + width_in_scu + i < 16) {
                    flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31
                        & (1)));
                }
                else {
                    flag = 0;
                }
                if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + width + i *
                            unit_size + j]);
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
    }
    if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
        if (pic_x == 0) {
            for (i = 0; i < height; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                left[i + 3] = (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + i]);
            }
            for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_y_scu + height_in_scu + i < 16) {
                    flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
                }
                else {
                    flag = 0;
                }
                if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + height + (i << unit_size_bit) +
                            j]);
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
        else {
            for (i = 0; i < height; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                left[i + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y + i][pic_x - 1]);
            }
            for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_y_scu + height_in_scu + i < 16) {
                    flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31
                        & (1)));
                }
                else {
                    flag = 0;
                }
                if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][height + (i << unit_size_bit) + j
                            + pic_y][pic_x - 1]);
                    }
                }
                else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
    }
    if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
        if (pic_y == 0 && pic_x == 0) {
            up[2] = left[2] = (IP_buffer_ptr->top_left_chroma[ch_type - 1]);
        }
        else if (pic_y == 0) {
            up[2] = left[2] = (IP_buffer_ptr->up_chroma[ch_type - 1][x - 1]);
        }
        else if (pic_x == 0) {
            up[2] = left[2] = (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y - 1]);
        }
        else {
            up[2] = left[2] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x - 1]);
        }
    }
    else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
        up[2] = left[2] = up[3];
    }
    else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
        up[2] = left[2] = left[3];
    }
    up[1] = left[3];
    left[1] = up[3];
    up[0] = left[4];
    left[0] = up[4];
}
void pred_inter_filter(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int component, int pfIdx, pel* pred_y, pel* pred_u, pel* pred_v,
    ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride)
{
    u16 avail_cu = com_get_avail_intra_forInterPF(MAP_SCU_IP_ptr, x >> MIN_CU_LOG2, y >> MIN_CU_LOG2);
    static pel nb[N_C][N_REF][MAX_CU_SIZE * 3] = { 0 };

    if ((component == 0) || (component == 1))
    {
        /* Y */
        get_IP_pixel_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, Y_C, nb[Y_C][0], nb[Y_C][1], x, y, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr);
        com_inter_filter(nb[Y_C][0] + 3, nb[Y_C][1] + 3, pred_y, cu_width, cu_height, bit_depth, avail_cu, pfIdx, stride);
    }

    if ((component == 0) || (component == 2))
    {
        get_IP_pixel_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, U_C, nb[U_C][0], nb[U_C][1], x, y, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr);
        get_IP_pixel_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, V_C, nb[V_C][0], nb[V_C][1], x, y, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr);
        com_inter_filter(nb[U_C][0] + 3, nb[U_C][1] + 3, pred_u, cu_width >> 1, cu_height >> 1, bit_depth, avail_cu, pfIdx, stride);
        com_inter_filter(nb[V_C][0] + 3, nb[V_C][1] + 3, pred_v, cu_width >> 1, cu_height >> 1, bit_depth, avail_cu, pfIdx, stride);
    }
}

void pred_inter_filter_luma_core(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, pel* pred_y,
    ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride)
{
    u16 avail_cu = com_get_avail_intra_forInterPF(MAP_SCU_IP_ptr, x >> MIN_CU_LOG2, y >> MIN_CU_LOG2);
    static pel nb[N_REF][MAX_CU_SIZE * 3] = { 0 };
    pel padding_number = (1 << (bit_depth - 1));
    for (int i = 0; i < N_REF; i++)
    {
        for (int j = 0; j < MAX_CU_SIZE * 3; j++)
        {
            nb[i][j] = padding_number;
        }
    }
    get_IP_pixel_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, Y_C, nb[0], nb[1], x, y, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr);
    com_inter_filter(nb[0] + 3, nb[1] + 3, pred_y, cu_width, cu_height, bit_depth, avail_cu, pfIdx, stride);
}


void pred_inter_filter_chroma_core(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, pel* pred_u, pel* pred_v,
    ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride)
{
    u16 avail_cu = com_get_avail_intra_forInterPF(MAP_SCU_IP_ptr, x >> MIN_CU_LOG2, y >> MIN_CU_LOG2);
    static pel nb[2][N_REF][MAX_CU_SIZE * 3] = { 0 };
    pel padding_number = (1 << (bit_depth - 1));
    for (int k = 0; k < 2; k++)
    {
        for (int i = 0; i < N_REF; i++)
        {
            for (int j = 0; j < MAX_CU_SIZE * 3; j++)
            {
                nb[k][i][j] = padding_number;
            }
        }
    }
    get_IP_pixel_chroma_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, U_C, nb[U_C - 1][0], nb[U_C - 1][1], x >> 1, y >> 1, me_line_ptr, IP_buffer_ptr);
    get_IP_pixel_chroma_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, V_C, nb[V_C - 1][0], nb[V_C - 1][1], x >> 1, y >> 1, me_line_ptr, IP_buffer_ptr);
    com_inter_filter(nb[U_C - 1][0] + 3, nb[U_C - 1][1] + 3, pred_u, cu_width, cu_height, bit_depth, avail_cu, pfIdx, stride);
    com_inter_filter(nb[V_C - 1][0] + 3, nb[V_C - 1][1] + 3, pred_v, cu_width, cu_height, bit_depth, avail_cu, pfIdx, stride);
}

void pred_inter_filter_chroma_single_core(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, int component, pel* pred,
    ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride)
{
    u16 avail_cu = com_get_avail_intra_forInterPF(MAP_SCU_IP_ptr, x >> MIN_CU_LOG2, y >> MIN_CU_LOG2);
    static pel nb[N_REF][MAX_CU_SIZE * 3] = { 0 };
    pel padding_number = (1 << (bit_depth - 1));

    for (int i = 0; i < N_REF; i++)
    {
        for (int j = 0; j < MAX_CU_SIZE * 3; j++)
        {
            nb[i][j] = padding_number;
        }
    }
    get_IP_pixel_chroma_forIPF(cu_width, cu_height, avail_cu, pic_width_in_scu, pic_height_in_scu, bit_depth, component, nb[0], nb[1], x >> 1, y >> 1, me_line_ptr, IP_buffer_ptr);
    com_inter_filter(nb[0] + 3, nb[1] + 3, pred, cu_width, cu_height, bit_depth, avail_cu, pfIdx, stride);
}

u8 is_apply_interpf(int interpf_enable_flag, int w, int h)
{
    if (interpf_enable_flag && (w * h >= 64) && (w <= 64) && (h <= 64) && (w > 4) && (h > 4))
        return 1;
    else
        return 0;
}
#endif
