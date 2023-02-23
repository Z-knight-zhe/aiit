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

#include "rmd.h"
#include "enc_def.h"
#include <math.h>



#if COLLECT_RMD_DATA
FILE* open_file(char* filename, char* mode)
{
	FILE* f;
	f = fopen(filename, mode);
	if (!f)
	{
		perror(filename);
		exit(0);
	}
	return f;
}

FILE* rmd_input_pix_x;
FILE* rmd_input_pix_y;
FILE* rmd_input_LCU_Y;
FILE* rmd_output_modes;
static int rmd_cu_cnt = 0;

void rmd_save_indata(U12* pix_x, U12* pix_y, pel Orig_Lcu_ptr_p_fenc_lcu_Y_rmd[MAX_CU_SIZE2 * MAX_CU_SIZE2])
{
	fwrite(pix_x, sizeof(U12), 1, rmd_input_pix_x);
	if (*pix_x == 0) {
		fwrite(pix_y, sizeof(U12), 1, rmd_input_pix_y);
	}
	fwrite(Orig_Lcu_ptr_p_fenc_lcu_Y_rmd, sizeof(pel), MAX_CU_SIZE2 * MAX_CU_SIZE2, rmd_input_LCU_Y);
}
void rmd_save_outdata(RMD_OUTPUT* rmd_output)
{
	fwrite(rmd_output, sizeof(RMD_OUTPUT), 1, rmd_output_modes);
}
#endif


#if PCNL_RMD

//#define USE_LOG 

#ifdef TEST_RMD_ALG
#include "md1.h"
#endif // TEST_RMD_ALG

#ifdef _MSC_VER
#define COM_INLINE __inline
#else
#define COM_INLINE static
#endif

#define ADI_4T_FILTER_BITS1                 7
#define ADI_4T_FILTER_OFFSET1              (1<<(ADI_4T_FILTER_BITS1-1))
#define GET_REF_POS1(mt,d_in,d_out,offset) \
    (d_out) = ((d_in) * (mt)) >> 10;\
    (offset) = ((((d_in) * (mt)) << 5) >> 10) - ((d_out) << 5);
#define _RMD_SIZE_NAME(name, m, n) name##_##m##x##n
#define RMD_SIZE_NAME(name, m, n) _RMD_SIZE_NAME(name, m, n)

#ifdef __SYNTHESIS__
#define _HLS_DIRECTIVE(cmd) cmd
#else
#define _HLS_DIRECTIVE(cmd) (void)"xx_hls_cmd "#cmd;
#endif

#define HLS_DIRECTIVE(cmd) _HLS_DIRECTIVE(cmd)

#ifdef USE_LOG
#define COM_LOG(fmt, ...) printf("[%s,%d]" fmt "\n",  __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define COM_LOG(...)
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

//extern RMD_BUF g_rmd_buf;
#if PATCH_DEBUG
extern PATCH_INFO* patch;
#endif

typedef struct {
	//U2 en_4x8;
	//U2 en_4x16;
	//U2 en_4x32;
	//U2 en_8x4;
	U2 en_8x8;
	U2 en_8x16;
	U2 en_8x32;
	//U2 en_16x4;
	U2 en_16x8;
	U2 en_16x16;
	U2 en_16x32;
	//U2 en_32x4;
	U2 en_32x8;
	U2 en_32x16;
	U2 en_32x32;
	U2 en_64x8;
	U2 en_64x16;
	U2 en_64x32;
}EN_CONTROL;

typedef struct {
	u32 data_reg_8x4[8];
	u32 data_reg_4x8;
	u32 data_reg_4x16;
	u32 data_reg_16x4[8];
	u32 data_reg_8x8[4];
	u32 data_reg_4x32;
	u32 data_reg_32x4[8];
	u32 data_reg_8x16[2];
	u32 data_reg_16x8;
	u32 data_reg_8x32;
	u32 data_reg_32x8;
	u32 data_reg_16x16[2];
	u32 data_reg_64x8[4];
	u32 data_reg_16x32;
	u32 data_reg_32x16;
	u32 data_reg_64x16;
	u32 data_reg_32x32;
	u32 data_reg_64x32;
}COST_REG;

typedef struct {

#if USE_SPEED_LEVEL
	//u32 cost_8x4[64][5];
//u32 cost_4x8[64][5];
//u32 cost_4x16[32][5];
//u32 cost_16x4[32][5];
	u32 cost_8x8[32][IPD_RDO_CNT + EXTRA_RMD_MODE];
	//u32 cost_4x32[16][5];
	//u32 cost_32x4[16][5];
	u32 cost_8x16[16][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_16x8[16][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_8x32[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_32x8[9][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_16x16[32][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_64x8[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_16x32[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_32x16[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_64x16[2][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_32x32[2][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_64x32[1][IPD_RDO_CNT + EXTRA_RMD_MODE];
#else
	//u32 cost_8x4[64][5];
//u32 cost_4x8[64][5];
//u32 cost_4x16[32][5];
//u32 cost_16x4[32][5];
	u32 cost_8x8[32][5];
	//u32 cost_4x32[16][5];
	//u32 cost_32x4[16][5];
	u32 cost_8x16[16][5];
	u32 cost_16x8[16][5];
	u32 cost_8x32[8][5];
	u32 cost_32x8[9][5];
	u32 cost_16x16[32][5];
	u32 cost_64x8[4][5];
	u32 cost_16x32[4][5];
	u32 cost_32x16[4][5];
	u32 cost_64x16[2][5];
	u32 cost_32x32[2][5];
	u32 cost_64x32[1][5];
#endif

}COST_MEM;

typedef struct {
#if USE_SPEED_LEVEL
	//u8 mode[RMD_CU_SIZE_NUM][IPD_RDO_CNT];
	//u8 modes_8x4[64][IPD_RDO_CNT];
	//u8 modes_16x4[32][IPD_RDO_CNT];
	//u8 modes_32x4[16][IPD_RDO_CNT];
	//u8 modes_4x8[64][IPD_RDO_CNT];
	u8 modes_8x8[32][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_16x8[16][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_32x8[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_64x8[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	//u8 modes_4x16[32][IPD_RDO_CNT];
	u8 modes_8x16[16][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_16x16[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_32x16[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_64x16[2][IPD_RDO_CNT + EXTRA_RMD_MODE];
	//u8 modes_4x32[16][IPD_RDO_CNT];
	u8 modes_8x32[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_16x32[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_32x32[2][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 modes_64x32[1][IPD_RDO_CNT + EXTRA_RMD_MODE];;
#else
	//u8 mode[RMD_CU_SIZE_NUM][IPD_RDO_CNT];
//u8 modes_8x4[64][IPD_RDO_CNT];
//u8 modes_16x4[32][IPD_RDO_CNT];
//u8 modes_32x4[16][IPD_RDO_CNT];
//u8 modes_4x8[64][IPD_RDO_CNT];
	u8 modes_8x8[32][IPD_RDO_CNT];
	u8 modes_16x8[16][IPD_RDO_CNT];
	u8 modes_32x8[8][IPD_RDO_CNT];
	u8 modes_64x8[4][IPD_RDO_CNT];
	//u8 modes_4x16[32][IPD_RDO_CNT];
	u8 modes_8x16[16][IPD_RDO_CNT];
	u8 modes_16x16[8][IPD_RDO_CNT];
	u8 modes_32x16[4][IPD_RDO_CNT];
	u8 modes_64x16[2][IPD_RDO_CNT];
	//u8 modes_4x32[16][IPD_RDO_CNT];
	u8 modes_8x32[8][IPD_RDO_CNT];
	u8 modes_16x32[4][IPD_RDO_CNT];
	u8 modes_32x32[2][IPD_RDO_CNT];
	u8 modes_64x32[1][IPD_RDO_CNT];;

	u8 out_modes_8x8[32][IPD_RDO_CNT];
	u8 out_modes_16x8[16][IPD_RDO_CNT];
	u8 out_modes_32x8[8][IPD_RDO_CNT];
	u8 out_modes_64x8[4][IPD_RDO_CNT];
	u8 out_modes_8x16[16][IPD_RDO_CNT];
	u8 out_modes_16x16[8][IPD_RDO_CNT];
	u8 out_modes_32x16[4][IPD_RDO_CNT];
	u8 out_modes_64x16[2][IPD_RDO_CNT];
	u8 out_modes_8x32[8][IPD_RDO_CNT];
	u8 out_modes_16x32[4][IPD_RDO_CNT];
	u8 out_modes_32x32[2][IPD_RDO_CNT];
	u8 out_modes_64x32[1][IPD_RDO_CNT];;
#endif

} RMD_OUT_TMP;

typedef struct {
	pel up_org[4];
	pel upright_org[4];
	pel left_org[4];
	pel leftdown_org[4];
}PIX_ORG;

static const int com_tbl_ipred_dxdy_phase2[IPD_CNT][2] = /* {dx/dy, dy/dx} */
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

static const int com_tbl_ipred_dxdy[IPD_CNT_PREV][2] = /* {dx/dy, dy/dx} */{
	{ 0,0 },
	{ 0,0 },{ 0,0 },{ 2816,372 },{ 2048,512 },{ 1408,744 },
	{ 1024,1024 },{ 744,1408 },{ 512,2048 },{ 372,2816 },{ 256,4096 },
	{ 128,8192 },{ 0,0 },{ 128,8192 },{ 256,4096 },{ 372,2816 },
	{ 512,2048 },{ 744,1408 },{ 1024,1024 },{ 1408,744 },{ 2048,512 },
	{ 2816,372 },{ 4096,256 },{ 8192,128 },{ 0,0 },{ 8192,128 },
	{ 4096,256 },{ 2816,372 },{ 2048,512 },{ 1408,744 },{ 1024,1024 },
	{ 744,1408 },{ 512,2048 },
};
static const u8 offs_tbl[11][16] = {
{  4,  4,  4,  4,  8,  8,  8,  8, 12, 12, 12, 12, 16, 16, 16, 16 },
{  8,  8,  8,  8, 16, 16, 16, 16, 24, 24, 24, 24,  0,  0,  0,  0 },
{ 11, 11, 11, 11, 23, 23, 23, 23, 24,  2,  2,  2, 24, 14, 14, 14 },
{ 16, 16, 16, 16,  0,  0,  0,  0,  0, 16, 16, 16,  0,  0,  0,  0 },
{ 23, 23, 23, 23, 12, 14, 14, 14, 12, 24,  5,  5, 12, 24, 29, 29 },
{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
{ 23, 12, 12, 12, 23, 14, 24, 24, 23, 14,  5, 29, 23, 14,  5, 29 },
{ 16,  0,  0,  0, 16,  0, 16,  0, 16,  0, 16,  0, 16,  0, 16,  0 },
{ 11, 23, 24, 24, 11, 23,  2, 14, 11, 23,  2, 14, 11, 23,  2, 14 },
{ 8, 16, 24,  0,  8, 16, 24,  0,  8, 16, 24,  0,  8, 16, 24,  0 },
{ 4,  8, 12, 16,  4,  8, 12, 16,  4,  8, 12, 16,  4,  8, 12, 16 },
};
static const u8 pn_n1_tbl[11][16] = {
{  4,  5,  6,  7,  4,  5,  6,  7,  4,  5,  6,  7,  4,  5,  6,  7 },
{  4,  5,  6,  7,  4,  5,  6,  7,  4,  5,  6,  7,  3,  4,  5,  6 },
{  4,  5,  6,  7,  4,  5,  6,  7,  4,  4,  5,  6,  5,  4,  5,  6 },
{  4,  5,  6,  7,  3,  4,  5,  6,  4,  4,  5,  6,  5,  3,  4,  5 },
{  4,  5,  6,  7,  4,  4,  5,  6,  5,  4,  4,  5,  6,  5,  4,  5 },
{  3,  4,  5,  6,  4,  3,  4,  5,  5,  4,  3,  4,  6,  5,  4,  3},
{  4,  4,  5,  6,  5,  4,  4,  5,  6,  5,  4,  4,  7,  6,  5,  5 },
{  4,  3,  4,  5,  5,  4,  4,  3,  6,  5,  5,  4,  7,  6,  6,  5 },
{  4,  4,  4,  5,  5,  5,  4,  4,  6,  6,  5,  5,  7,  7,  6,  6 },
{  4,  4,  4,  3,  5,  5,  5,  4,  6,  6,  6,  5,  7,  7,  7,  6},
{  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7},
};
static const u8 p_tbl[11][16] = {
{ 3, 4, 5, 6, 3, 4, 5, 6, 3, 4, 5, 6, 3, 4, 5, 6 },
{ 3, 4, 5, 6, 3, 4, 5, 6, 3, 4, 5, 6, 2, 3, 4, 5 },
{ 3, 4, 5, 6, 3, 4, 5, 6, 3, 3, 4, 5, 4, 3, 4, 5 },
{ 3, 4, 5, 6, 2, 3, 4, 5, 3, 3, 4, 5, 4, 2, 3, 4 },
{ 3, 4, 5, 6, 3, 3, 4, 5, 4, 3, 3, 4, 5, 4, 3, 4 },
{ 2, 3, 4, 5, 3, 2, 3, 4, 4, 3, 2, 3, 5, 4, 3, 2},
{ 3, 3, 4, 5, 4, 3, 3, 4, 5, 4, 3, 3, 6, 5, 4, 4 },
{ 3, 2, 3, 4, 4, 3, 3, 2, 5, 4, 4, 3, 6, 5, 5, 4 },
{ 3, 3, 3, 4, 4, 4, 3, 3, 5, 5, 4, 4, 6, 6, 5, 5 },
{ 3, 3, 3, 2, 4, 4, 4, 3, 5, 5, 5, 4, 6, 6, 6, 5},
{ 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6},
};
static const u8 pn_tbl[11][16] = {
{ 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5 },
{ 2, 3, 4, 5, 2, 3, 4, 5, 2, 3, 4, 5, 1, 2, 3, 4 },
{ 2, 3, 4, 5, 2, 3, 4, 5, 2, 2, 3, 4, 3, 2, 3, 4 },
{ 2, 3, 4, 5, 1, 2, 3, 4, 2, 2, 3, 4, 3, 1, 2, 3 },
{ 2, 3, 4, 5, 2, 2, 3, 4, 3, 2, 2, 3, 4, 3, 2, 3 },
{ 1, 2, 3, 4, 2, 1, 2, 3, 3, 2, 1, 2, 4, 3, 2, 1},
{ 2, 2, 3, 4, 3, 2, 2, 3, 4, 3, 2, 2, 5, 4, 3, 3 },
{ 2, 1, 2, 3, 3, 2, 2, 1, 4, 3, 3, 2, 5, 4, 4, 3 },
{ 2, 2, 2, 3, 3, 3, 2, 2, 4, 4, 3, 3, 5, 5, 4, 4 },
{ 2, 2, 2, 1, 3, 3, 3, 2, 4, 4, 4, 3, 5, 5, 5, 4},
{ 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5},
};
static const u8 pn_p2_tbl[11][16] = {
{ 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4 },
{ 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 0, 1, 2, 3 },
{ 1, 2, 3, 4, 1, 2, 3, 4, 1, 1, 2, 3, 2, 1, 2, 3 },
{ 1, 2, 3, 4, 0, 1, 2, 3, 1, 1, 2, 3, 2, 0, 1, 2 },
{ 1, 2, 3, 4, 1, 1, 2, 3, 2, 1, 1, 2, 3, 2, 1, 2 },
{ 0, 1, 2, 3, 1, 0, 1, 2, 2, 1, 0, 1, 3, 2, 1, 0},
{ 1, 1, 2, 3, 2, 1, 1, 2, 3, 2, 1, 1, 4, 3, 2, 2 },
{ 1, 0, 1, 2, 2, 1, 1, 0, 3, 2, 2, 1, 4, 3, 3, 2 },
{ 1, 1, 1, 2, 2, 2, 1, 1, 3, 3, 2, 2, 4, 4, 3, 3 },
{ 1, 1, 1, 0, 2, 2, 2, 1, 3, 3, 3, 2, 4, 4, 4, 3},
{ 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4},
};
static const s8 x_tbl[11][16] = {
{ 0,  1,  2,  3,  0,  1,  2,  3,  0,  1,  2,  3,  0,  1,  2,  3 },
{ 0,  1,  2,  3,  0,  1,  2,  3,  0,  1,  2,  3, -1,  0,  1,  2 },
{ 0,  1,  2,  3,  0,  1,  2,  3, -1,  0,  1,  2, -1,  0,  1,  2 },
{ 0,  1,  2,  3, -1,  0,  1,  2, -1,  0,  1,  2, -1, -1,  0,  1 },
{ 0,  1,  2,  3, -1,  0,  1,  2, -1, -1,  0,  1, -1, -1,  0,  1 },
{ -1,  0,  1,  2, -1, -1,  0,  1, -1, -1, -1,  0, -1, -1, -1, -1},
{ -1,  0,  1,  2, -1, -1,  0,  1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ -1, -1,  0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ -1, -1,  0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};
static const s8 y_tbl[11][16] = {
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
{ -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1,  1, -1, -1, -1 },
{ -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1,  1, -1, -1, -1 },
{ -1, -1, -1, -1,  0, -1, -1, -1,  1,  0, -1, -1,  2,  1, -1, -1 },
{ -1, -1, -1, -1,  0, -1, -1, -1,  1,  0, -1, -1,  2,  1,  0, -1},
{ 0, -1, -1, -1,  1,  0, -1, -1,  2,  1,  0,  0,  3,  2,  1,  1 },
{ 0, -1, -1, -1,  1,  0,  0, -1,  2,  1,  1,  0,  3,  2,  2,  1 },
{ 0,  0, -1, -1,  1,  1,  0,  0,  2,  2,  1,  1,  3,  3,  2,  2 },
{ 0,  0,  0, -1,  1,  1,  1,  0,  2,  2,  2,  1,  3,  3,  3,  2},
{ 0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3},
};
static const s16 com_tbl_ipred_adi[32][4] = {
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

#define IDX(y, x, stride) ((y)*(stride) + (x))
u32 rmd_satd_4x4(pel org[4 * 4], pel cur[4 * 4])
{
	HLS_DIRECTIVE(#pragma HLS inline off)
		HLS_DIRECTIVE(#pragma HLS array_partition variable = org complete dim = 0)
		HLS_DIRECTIVE(#pragma HLS array_partition variable = cur complete dim = 0)
		s8 h, w, k;
	u32 satd = 0;
	s16 diff[16], m[16], d[16];
	HLS_DIRECTIVE(#pragma HLS array_partition variable = diff complete dim = 0)
		HLS_DIRECTIVE(#pragma HLS array_partition variable = m complete dim = 0)
		HLS_DIRECTIVE(#pragma HLS array_partition variable = d complete dim = 0)
		for (h = 0; h < 4; h++)
		{
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				for (w = 0; w < 4; w++)
				{
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						diff[w + h * 4] = org[IDX(h, w, 4)] - cur[IDX(h, w, 4)];
				}
		}
	m[0] = diff[0] + diff[12];
	m[1] = diff[1] + diff[13];
	m[2] = diff[2] + diff[14];
	m[3] = diff[3] + diff[15];
	m[4] = diff[4] + diff[8];
	m[5] = diff[5] + diff[9];
	m[6] = diff[6] + diff[10];
	m[7] = diff[7] + diff[11];
	m[8] = diff[4] - diff[8];
	m[9] = diff[5] - diff[9];
	m[10] = diff[6] - diff[10];
	m[11] = diff[7] - diff[11];
	m[12] = diff[0] - diff[12];
	m[13] = diff[1] - diff[13];
	m[14] = diff[2] - diff[14];
	m[15] = diff[3] - diff[15];
	d[0] = m[0] + m[4];
	d[1] = m[1] + m[5];
	d[2] = m[2] + m[6];
	d[3] = m[3] + m[7];
	d[4] = m[8] + m[12];
	d[5] = m[9] + m[13];
	d[6] = m[10] + m[14];
	d[7] = m[11] + m[15];
	d[8] = m[0] - m[4];
	d[9] = m[1] - m[5];
	d[10] = m[2] - m[6];
	d[11] = m[3] - m[7];
	d[12] = m[12] - m[8];
	d[13] = m[13] - m[9];
	d[14] = m[14] - m[10];
	d[15] = m[15] - m[11];
	m[0] = d[0] + d[3];
	m[1] = d[1] + d[2];
	m[2] = d[1] - d[2];
	m[3] = d[0] - d[3];
	m[4] = d[4] + d[7];
	m[5] = d[5] + d[6];
	m[6] = d[5] - d[6];
	m[7] = d[4] - d[7];
	m[8] = d[8] + d[11];
	m[9] = d[9] + d[10];
	m[10] = d[9] - d[10];
	m[11] = d[8] - d[11];
	m[12] = d[12] + d[15];
	m[13] = d[13] + d[14];
	m[14] = d[13] - d[14];
	m[15] = d[12] - d[15];
	d[0] = m[0] + m[1];
	d[1] = m[0] - m[1];
	d[2] = m[2] + m[3];
	d[3] = m[3] - m[2];
	d[4] = m[4] + m[5];
	d[5] = m[4] - m[5];
	d[6] = m[6] + m[7];
	d[7] = m[7] - m[6];
	d[8] = m[8] + m[9];
	d[9] = m[8] - m[9];
	d[10] = m[10] + m[11];
	d[11] = m[11] - m[10];
	d[12] = m[12] + m[13];
	d[13] = m[12] - m[13];
	d[14] = m[14] + m[15];
	d[15] = m[15] - m[14];
	for (k = 0; k < 16; k++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			satd += COM_ABS(d[k]);
	}
	satd = ((satd + 1) >> 1);
	return satd;
}

COM_INLINE u16 get_avail_intra(int x, int y)
{
#pragma HLS inline
	u16 avail = 0;
	if (x > 0) {
#if PATCH_DEBUG
		if (x > patch->left_pel)
#endif
		{
			SET_AVAIL(avail, AVAIL_LE);
		}
	}
	if (y > 0) {
#if PATCH_DEBUG
		if (y > patch->up_pel)
#endif
		{
			SET_AVAIL(avail, AVAIL_UP);
			if (x > 0) {
#if PATCH_DEBUG
				if (x > patch->left_pel)
#endif
				{
					SET_AVAIL(avail, AVAIL_UP_LE);
				}
			}
		}
	}
	return avail;
}

COM_INLINE int to_idx(int y, int x)
{
	return (y << 6) + x;
}



#ifndef TEST_RMD_ALG



void rmd_ipred_hor_4x4(pel src_le[4], pel dst[4 * 4])
{

	int i, j;
	{
		for (i = 0; i < 4; i++)
		{
			HLS_DIRECTIVE(#pragma HLS UNROLL)

				for (j = 0; j < 4; j++)
				{
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						dst[j] = src_le[0];
				}
			dst += 4;
			src_le++;
		}
	}
}

void rmd_ipred_vert_4x4(pel src_up[4], pel dst[4 * 4])
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			for (j = 0; j < 4; j++)
			{
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					dst[j] = src_up[j];
			}
		dst += 4;
	}
}

void rmd_ipred_dc_4x4(pel src_le[4], pel src_up[4], pel dst[4 * 4], int bit_depth, u16 avail_cu)
{
	int dc = 0;
	int i, j;
	if (IS_AVAIL(avail_cu, AVAIL_LE))
	{
		for (i = 0; i < 4; i++)
		{
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				dc += src_le[i];
		}
		if (IS_AVAIL(avail_cu, AVAIL_UP))
		{
			for (j = 0; j < 4; j++)
			{
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					dc += src_up[j];
			}
			dc = (dc + ((4 + 4) >> 1)) * (4096 / (4 + 4)) >> 12;
		}
		else
		{
			dc = (dc + (4 >> 1)) >> 2;
		}
	}
	else if (IS_AVAIL(avail_cu, AVAIL_UP))
	{
		for (j = 0; j < 4; j++)
		{
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				dc += src_up[j];
		}
		dc = (dc + (4 >> 1)) >> 2;
	}
	else
	{
		dc = 1 << (bit_depth - 1);
	}
	for (i = 0; i < 16; i++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			dst[i] = (pel)dc;
	}
}

void rmd_ipred_plane_4x4(pel src_le[2 * 4 + 1], pel src_up[2 * 4 + 1], pel dst[16], U4 bit_depth)
{
#pragma HLS LATENCY min=2 max=2
	pel* rsrc;
	int  coef_h = 0, coef_v = 0;
	int  a, b, c, x, y;
	int  w2 = 4 >> 1;
	int  h2 = 4 >> 1;
	int  ib_mult[5] = { 13, 17, 5, 11, 23 };
	int  ib_shift[5] = { 7, 10, 11, 15, 19 };
	int  idx_w = 0;
	int  idx_h = 0;
	int  im_h, is_h, im_v, is_v, temp, temp2;
	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];
	rsrc = src_up;
	for (x = 1; x < (4 >> 1) + 1; x++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			coef_h += x * (rsrc[w2 + x] - rsrc[w2 - x]);
	}
	rsrc = src_le;
	for (y = 1; y < (4 >> 1) + 1; y++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			coef_v += y * (rsrc[h2 + y] - rsrc[h2 - y]);
	}
	a = (src_le[4] + src_up[4]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;
	pel dst_v;
	for (y = 0; y < 4; y++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			temp2 = temp;
		for (x = 0; x < 4; x++)
		{
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				dst_v = (pel)(temp2 >> 5);
			dst[x] = COM_CLIP3(0, ((1 << bit_depth) - 1), dst_v);
			temp2 += b;
		}
		temp += c;
		dst += 4;
	}
}

void rmd_ipred_bi_4x4(pel src_le[4], pel src_up[4], pel dst[16], U4 bit_depth)
{
	int x, y;
	int ishift_x = 2;
	int ishift_y = 2;
	//    int ishift = COM_MIN(ishift_x, ishift_y);
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a, b, c, wt, wxy, tmp;
	int predx;
	int ref_up[4], ref_le[4], up[4], le[4], wy[16];
	int wc, tbl_wc[6] = { -1, 21, 13, 7, 4, 2 };
	HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = ref_up complete dim = 1)
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = ref_le complete dim = 1)
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = up complete dim = 1)
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = le complete dim = 1)
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = wy cyclic factor = 4 dim = 1)
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = tbl_wc complete dim = 1)
		wc = ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x;
	wc = tbl_wc[wc];
	for (x = 0; x < 4; x++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			ref_up[x] = src_up[x];
	}
	for (y = 0; y < 4; y++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			ref_le[y] = src_le[y];
	}
	a = src_up[4 - 1];
	b = src_le[4 - 1];
	c = (a + b + 1) >> 1;
	wt = (c << 1) - a - b;
	for (x = 0; x < 4; x++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			up[x] = b - ref_up[x];
		ref_up[x] <<= ishift_y;
	}
	tmp = 0;
	for (y = 0; y < 4; y++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			le[y] = a - ref_le[y];
		ref_le[y] <<= ishift_x;
		wy[y] = tmp;
		tmp += wt;
	}
	int dst_v;
	for (y = 0; y < 4; y++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			predx = ref_le[y];
		wxy = 0;
		for (x = 0; x < 4; x++)
		{
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				predx += le[y];
			ref_up[x] += up[x];
			dst_v = (pel)(((predx << ishift_y) + (ref_up[x] << ishift_x) + wxy + offset) >> ishift_xy);
			dst[x] = COM_CLIP3(0, ((1 << bit_depth) - 1), dst_v);
			wxy += wy[y];
		}
		dst += 4;
	}
}

pel rmd_ipred_ang_val4x4(pel src_up[2 * 4 + 3], pel src_le[2 * 4 + 3], int ipm, int i, int j)
{
	HLS_DIRECTIVE(#pragma HLS INLINE)
		int offset;
	int t_dx, xx, yy;
	int x, xn, yn, xn_n1, yn_n1, xn_p2, yn_p2;
	const s16* filter;
	const s16(*tbl_filt)[4];
	int filter_offset, filter_bits;
	//const int* mt = com_tbl_ipred_dxdy[ipm];
	const int* mt = com_tbl_ipred_dxdy_phase2[ipm];
	int num_selections = 0;
	int p, pn, pn_n1, pn_p2;
	int pos_max = 4 + 4 - 1;
	pel temp_pel = 0;
	x = INT_MAX;
	xn = INT_MAX;
	xn_n1 = INT_MAX;
	xn_p2 = INT_MAX;
	yn = INT_MAX;
	yn_n1 = INT_MAX;
	yn_p2 = INT_MAX;
	xx = INT_MAX;
	yy = INT_MAX;
	filter_offset = ADI_4T_FILTER_OFFSET1;
	filter_bits = ADI_4T_FILTER_BITS1;
	tbl_filt = com_tbl_ipred_adi;
	GET_REF_POS1(mt[0], j + 1, t_dx, offset);
	x = i + t_dx;
	xn_n1 = x - 1;
	xn = x + 1;
	xn_p2 = x + 2;
	++num_selections;
	pos_max = 4 * 2 - 1;
	pn_n1 = xn_n1;
	p = x;
	pn = xn;
	pn_p2 = xn_p2;
	pn_n1 = COM_MIN(pn_n1, pos_max);
	p = COM_MIN(p, pos_max);
	pn = COM_MIN(pn, pos_max);
	pn_p2 = COM_MIN(pn_p2, pos_max);
	filter = (tbl_filt + offset)[0];
	pn_n1 += 3;
	p += 3;
	pn += 3;
	pn_p2 += 3;
	temp_pel = ((pel)((((int)src_up[pn_n1]) * ((int)filter[0]) + ((int)src_up[p]) * ((int)filter[1]) + ((int)src_up[pn]) * ((int)filter[2]) + ((int)src_up[pn_p2]) * ((int)filter[3]) + filter_offset) >> filter_bits));
	return temp_pel;
}

void rmd_ipred_ang4x4(pel src_le[2 * 4 + 3], pel src_up[2 * 4 + 3], pel dst[4 * 4], int ipm)
{
	int i, j;
	for (j = 0; j < 4; j++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			for (i = 0; i < 4; i++)
			{
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					dst[i] = rmd_ipred_ang_val4x4(src_up, src_le, ipm, i, j);
			}
		dst += 4;
	}
}

pel rmd_ipred1_ang_val4x4(pel src_up[2 * 4 + 3], pel src_le[2 * 4 + 3], int ipm, int i, int j)
{
	HLS_DIRECTIVE(#pragma HLS INLINE)
		int offset;
	const s16* filter;
	const s16(*tbl_filt)[4];
	int filter_offset, filter_bits;
	int p, pn, pn_n1, pn_p2;
	pel temp_pel = 0;

	filter_offset = ADI_4T_FILTER_OFFSET1;
	filter_bits = ADI_4T_FILTER_BITS1;
	tbl_filt = com_tbl_ipred_adi;
	offset = offs_tbl[ipm - 13][j * 4 + i];
	pn_n1 = pn_n1_tbl[ipm - 13][j * 4 + i];
	p = p_tbl[ipm - 13][j * 4 + i];
	pn = pn_tbl[ipm - 13][j * 4 + i];
	pn_p2 = pn_p2_tbl[ipm - 13][j * 4 + i];
	filter = (tbl_filt + offset)[0];
	pel src_tmp[4];
	HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = src_tmp complete dim = 1)
		src_tmp[0] = (y_tbl[ipm - 13][j * 4 + i] == -1) ? src_up[pn_n1] : (x_tbl[ipm - 13][j * 4 + i] == -1) ? src_le[pn_n1] : 0;
	src_tmp[1] = (y_tbl[ipm - 13][j * 4 + i] == -1) ? src_up[p] : (x_tbl[ipm - 13][j * 4 + i] == -1) ? src_le[p] : 0;
	src_tmp[2] = (y_tbl[ipm - 13][j * 4 + i] == -1) ? src_up[pn] : (x_tbl[ipm - 13][j * 4 + i] == -1) ? src_le[pn] : 0;
	src_tmp[3] = (y_tbl[ipm - 13][j * 4 + i] == -1) ? src_up[pn_p2] : (x_tbl[ipm - 13][j * 4 + i] == -1) ? src_le[pn_p2] : 0;

	temp_pel = ((pel)((((int)src_tmp[0]) * ((int)filter[0]) + ((int)src_tmp[1]) * ((int)filter[1]) + ((int)src_tmp[2]) * ((int)filter[2]) + ((int)src_tmp[3]) * ((int)filter[3]) + filter_offset) >> filter_bits));

	return temp_pel;
}

pel rmd_ipred1_ang_val4x4_phase2(pel src_up[2 * 4 + 3], pel src_le[2 * 4 + 3], int ipm, int i, int j)
{
	HLS_DIRECTIVE(#pragma HLS INLINE)
		int offset;
	const s16* filter;
	const s16(*tbl_filt)[4];
	int filter_offset, filter_bits;
	int p, pn, pn_n1, pn_p2;
	pel temp_pel = 0;


	static const u8 offs_tbl_phase2[14][16] = {
{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4},
{2, 2, 2, 2, 4, 4, 4, 4, 6, 6, 6, 6, 8, 8, 8, 8},
{6, 6, 6, 6, 12, 12, 12, 12, 18, 18, 18, 18, 24, 24, 24, 24},
{10, 10, 10, 10, 20, 20, 20, 20, 30, 30, 30, 30, 6, 8, 8, 8},
{14, 14, 14, 14, 28, 28, 28, 28, 9, 10, 10, 10, 9, 24, 24, 24},
{20, 20, 20, 20, 19, 8, 8, 8, 19, 28, 28, 28, 19, 6, 16, 16},
{28, 28, 28, 28, 4, 24, 24, 24, 4, 9, 20, 20, 4, 9, 13, 16},
{28, 4, 4, 4, 28, 24, 9, 9, 28, 24, 20, 13, 28, 24, 20, 16},
{20, 19, 19, 19, 20, 8, 28, 6, 20, 8, 28, 16, 20, 8, 28, 16},
{14, 28, 9, 9, 14, 28, 10, 24, 14, 28, 10, 24, 14, 28, 10, 24},
{10, 20, 30, 6, 10, 20, 30, 8, 10, 20, 30, 8, 10, 20, 30, 8},
{6, 12, 18, 24, 6, 12, 18, 24, 6, 12, 18, 24, 6, 12, 18, 24},
{2, 4, 6, 8, 2, 4, 6, 8, 2, 4, 6, 8, 2, 4, 6, 8},
{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4},
	};
	static const u8 pn_n1_tbl_phase2[14][16] = {
{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4},
{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4},
{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4},
{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 1, 2, 3},
{1, 2, 3, 4, 1, 2, 3, 4, 1, 1, 2, 3, 2, 1, 2, 3},
{1, 2, 3, 4, 1, 1, 2, 3, 2, 1, 2, 3, 3, 1, 1, 2},
{1, 2, 3, 4, 1, 1, 2, 3, 2, 1, 1, 2, 3, 2, 1, 1},
{1, 1, 2, 3, 2, 1, 1, 2, 3, 2, 1, 1, 4, 3, 2, 1},
{1, 1, 2, 3, 2, 1, 1, 1, 3, 2, 2, 1, 4, 3, 3, 2},
{1, 1, 1, 2, 2, 2, 1, 1, 3, 3, 2, 2, 4, 4, 3, 3},
{1, 1, 1, 1, 2, 2, 2, 1, 3, 3, 3, 2, 4, 4, 4, 3},
{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4},
{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4},
{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4},
	};
	static const u8 p_tbl_phase2[14][16] = {
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 0, 1, 2},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 0, 1, 2, 1, 0, 1, 2},
{0, 1, 2, 3, 0, 0, 1, 2, 1, 0, 1, 2, 2, 0, 0, 1},
{0, 1, 2, 3, 0, 0, 1, 2, 1, 0, 0, 1, 2, 1, 0, 0},
{0, 0, 1, 2, 1, 0, 0, 1, 2, 1, 0, 0, 3, 2, 1, 0},
{0, 0, 1, 2, 1, 0, 0, 0, 2, 1, 1, 0, 3, 2, 2, 1},
{0, 0, 0, 1, 1, 1, 0, 0, 2, 2, 1, 1, 3, 3, 2, 2},
{0, 0, 0, 0, 1, 1, 1, 0, 2, 2, 2, 1, 3, 3, 3, 2},
{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
	};
	static const s8 pn_tbl_phase2[14][16] = {
{-1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2},
{-1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2},
{-1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2},
{-1, 0, 1, 2, -1, 0, 1, 2, -1, 0, 1, 2, -1, -1, 0, 1},
{-1, 0, 1, 2, -1, 0, 1, 2, -1, -1, 0, 1, 0, -1, 0, 1},
{-1, 0, 1, 2, -1, -1, 0, 1, 0, -1, 0, 1, 1, -1, -1, 0},
{-1, 0, 1, 2, -1, -1, 0, 1, 0, -1, -1, 0, 1, 0, -1, -1},
{-1, -1, 0, 1, 0, -1, -1, 0, 1, 0, -1, -1, 2, 1, 0, -1},
{-1, -1, 0, 1, 0, -1, -1, -1, 1, 0, 0, -1, 2, 1, 1, 0},
{-1, -1, -1, 0, 0, 0, -1, -1, 1, 1, 0, 0, 2, 2, 1, 1},
{-1, -1, -1, -1, 0, 0, 0, -1, 1, 1, 1, 0, 2, 2, 2, 1},
{-1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2},
{-1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2},
{-1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2},
	};
	static const s8 pn_p2_tbl_phase2[14][16] = {
{-2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1},
{-2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1},
{-2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1},
{-2, -1, 0, 1, -2, -1, 0, 1, -2, -1, 0, 1, -2, -2, -1, 0},
{-2, -1, 0, 1, -2, -1, 0, 1, -2, -2, -1, 0, -1, -2, -1, 0},
{-2, -1, 0, 1, -2, -2, -1, 0, -1, -2, -1, 0, 0, -2, -2, -1},
{-2, -1, 0, 1, -2, -2, -1, 0, -1, -2, -2, -1, 0, -1, -2, -2},
{-2, -2, -1, 0, -1, -2, -2, -1, 0, -1, -2, -2, 1, 0, -1, -2},
{-2, -2, -1, 0, -1, -2, -2, -2, 0, -1, -1, -2, 1, 0, 0, -1},
{-2, -2, -2, -1, -1, -1, -2, -2, 0, 0, -1, -1, 1, 1, 0, 0},
{-2, -2, -2, -2, -1, -1, -1, -2, 0, 0, 0, -1, 1, 1, 1, 0},
{-2, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1},
{-2, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1},
{-2, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1},
	};
	static const s8 x_tbl_phase2[14][16] = {
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3},
{0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, -1, 0, 1, 2},
{0, 1, 2, 3, 0, 1, 2, 3, -1, 0, 1, 2, -1, 0, 1, 2},
{0, 1, 2, 3, -1, 0, 1, 2, -1, 0, 1, 2, -2, -1, 0, 1},
{0, 1, 2, 3, -1, 0, 1, 2, -2, -1, 0, 1, -3, -2, -1, 0},
{-1, 0, 1, 2, -2, -1, 0, 1, -3, -2, -1, 0, -4, -3, -2, -1},
{-1, 0, 1, 2, -3, -2, -1, 0, -4, -3, -2, -1, -6, -5, -4, -3},
{-2, -1, 0, 1, -4, -3, -2, -1, -6, -5, -4, -3, -9, -8, -7, -6},
{-3, -2, -1, 0, -6, -5, -4, -3, -9, -8, -7, -6, -12, -11, -10, -9},
{-5, -4, -3, -2, -10, -9, -8, -7, -15, -14, -13, -12, -21, -20, -19, -18},
{-16, -15, -14, -13, -32, -31, -30, -29, -48, -47, -46, -45, -64, -63, -62, -61},
{-32, -31, -30, -29, -64, -63, -62, -61, -96, -95, -94, -93, -128, -127, -126, -125},

	};
	static const s8 y_tbl_phase2[14][16] = {
{-32, -64, -96, -128, -31, -63, -95, -127, -30, -62, -94, -126, -29, -61, -93, -125},
{-16, -32, -48, -64, -15, -31, -47, -63, -14, -30, -46, -62, -13, -29, -45, -61},
{-5, -10, -15, -21, -4, -9, -14, -20, -3, -8, -13, -19, -2, -7, -12, -18},
{-3, -6, -9, -12, -2, -5, -8, -11, -1, -4, -7, -10, 0, -3, -6, -9},
{-2, -4, -6, -9, -1, -3, -5, -8, 0, -2, -4, -7, 1, -1, -3, -6},
{-1, -3, -4, -6, 0, -2, -3, -5, 1, -1, -2, -4, 2, 0, -1, -3},
{-1, -2, -3, -4, 0, -1, -2, -3, 1, 0, -1, -2, 2, 1, 0, -1},
{0, -1, -2, -3, 1, 0, -1, -2, 2, 1, 0, -1, 3, 2, 1, 0},
{0, -1, -1, -2, 1, 0, 0, -1, 2, 1, 1, 0, 3, 2, 2, 1},
{0, 0, -1, -1, 1, 1, 0, 0, 2, 2, 1, 1, 3, 3, 2, 2},
{0, 0, 0, -1, 1, 1, 1, 0, 2, 2, 2, 1, 3, 3, 3, 2},
{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3},
	};

	filter_offset = ADI_4T_FILTER_OFFSET1;
	filter_bits = ADI_4T_FILTER_BITS1;
	tbl_filt = com_tbl_ipred_adi;
	offset = offs_tbl_phase2[ipm - 44][j * 4 + i];
	pn_n1 = pn_n1_tbl_phase2[ipm - 44][j * 4 + i];
	p = p_tbl_phase2[ipm - 44][j * 4 + i];
	pn = pn_tbl_phase2[ipm - 44][j * 4 + i];
	pn_p2 = pn_p2_tbl_phase2[ipm - 44][j * 4 + i];

	pn_n1 += 3;
	p += 3;
	pn += 3;
	pn_p2 += 3;




	filter = (tbl_filt + offset)[0];
	pel src_tmp[4];
	HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = src_tmp complete dim = 1)
		src_tmp[0] = (y_tbl_phase2[ipm - 44][j * 4 + i] <= -1) ? src_up[pn_n1] : src_le[pn_n1];
	src_tmp[1] = (y_tbl_phase2[ipm - 44][j * 4 + i] <= -1) ? src_up[p] : src_le[p];
	src_tmp[2] = (y_tbl_phase2[ipm - 44][j * 4 + i] <= -1) ? src_up[pn] : src_le[pn];
	src_tmp[3] = (y_tbl_phase2[ipm - 44][j * 4 + i] <= -1) ? src_up[pn_p2] : src_le[pn_p2];

	temp_pel = ((pel)((((int)src_tmp[0]) * ((int)filter[0]) + ((int)src_tmp[1]) * ((int)filter[1]) + ((int)src_tmp[2]) * ((int)filter[2]) + ((int)src_tmp[3]) * ((int)filter[3]) + filter_offset) >> filter_bits));

	return temp_pel;
}

void rmd_ipred1_ang4x4_phase2(pel src_le[2 * 4 + 3], pel src_up[2 * 4 + 3], pel dst[4 * 4], int ipm)
{
	int i, j;
	for (j = 0; j < 4; j++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			for (i = 0; i < 4; i++)
			{
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					dst[i] = rmd_ipred1_ang_val4x4_phase2(src_up, src_le, ipm, i, j);
			}
		dst += 4;
	}
}

void rmd_ipred1_ang4x4(pel src_le[2 * 4 + 3], pel src_up[2 * 4 + 3], pel dst[4 * 4], int ipm)
{
	int i, j;
	for (j = 0; j < 4; j++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			for (i = 0; i < 4; i++)
			{
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					dst[i] = rmd_ipred1_ang_val4x4(src_up, src_le, ipm, i, j);
			}
		dst += 4;
	}
}

pel rmd_ipred2_ang_val4x4(pel src_up[2 * 4 + 3], pel src_le[2 * 4 + 3], int ipm, int i, int j)
{
	HLS_DIRECTIVE(#pragma HLS INLINE)
		int offset;
	int t_dy, xx, yy;
	int x, y, xn, yn, xn_n1, yn_n1, xn_p2, yn_p2;
	const s16* filter;
	const s16(*tbl_filt)[4];
	int filter_offset, filter_bits;
	//const int* mt = com_tbl_ipred_dxdy[ipm];
	const int* mt = com_tbl_ipred_dxdy_phase2[ipm];
	int num_selections = 0;
	int use_x = -1;
	int p, pn, pn_n1, pn_p2;
	int pos_max = 4 + 4 - 1;
	pel temp_pel = 0;
	x = INT_MAX;
	xn = INT_MAX;
	xn_n1 = INT_MAX;
	xn_p2 = INT_MAX;
	y = INT_MAX;
	yn = INT_MAX;
	yn_n1 = INT_MAX;
	yn_p2 = INT_MAX;
	xx = INT_MAX;
	yy = INT_MAX;
	filter_offset = ADI_4T_FILTER_OFFSET1;
	filter_bits = ADI_4T_FILTER_BITS1;
	tbl_filt = com_tbl_ipred_adi;
	GET_REF_POS1(mt[1], i + 1, t_dy, offset);
	x = -1;
	y = j + t_dy;
	yn_n1 = y - 1;
	yn = y + 1;
	yn_p2 = y + 2;
	use_x = 0;
	++num_selections;
	pos_max = 4 * 2 - 1;
	pn_n1 = yn_n1;
	p = y;
	pn = yn;
	pn_p2 = yn_p2;
	pn_n1 = COM_MIN(pn_n1, pos_max);
	p = COM_MIN(p, pos_max);
	pn = COM_MIN(pn, pos_max);
	pn_p2 = COM_MIN(pn_p2, pos_max);
	filter = (tbl_filt + offset)[0];
	pn_n1 += 3;
	p += 3;
	pn += 3;
	pn_p2 += 3;
	temp_pel = ((pel)((((int)src_le[pn_n1]) * ((int)filter[0]) + ((int)src_le[p]) * ((int)filter[1]) + ((int)src_le[pn]) * ((int)filter[2]) + ((int)src_le[pn_p2]) * ((int)filter[3]) + filter_offset) >> filter_bits));
	return temp_pel;
}

void rmd_ipred2_ang4x4(pel src_le[2 * 4 + 3], pel src_up[2 * 4 + 3], pel dst[4 * 4], int ipm)
{
	int i, j;
	for (j = 0; j < 4; j++)
	{
		HLS_DIRECTIVE(#pragma HLS UNROLL)
			for (i = 0; i < 4; i++)
			{
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					dst[i] = rmd_ipred2_ang_val4x4(src_up, src_le, ipm, i, j);
			}
		dst += 4;
	}
}

void copy_org(pel src[MAX_CU_SIZE2 * MAX_CU_SIZE2], pel dest[2][8][16][4 * 4], pel dest1[2][32][16])
{
	#pragma HLS inline
#pragma HLS ARRAY_PARTITION variable=src cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=dest complete dim=4
#pragma HLS ARRAY_PARTITION variable=dest complete dim=3
#pragma HLS ARRAY_PARTITION variable=dest1 complete dim=3
	for (int i = 0; i < (MAX_CU_SIZE2/2); i++) {
#pragma HLS PIPELINE
		dest[0][i / 4][0][((i & 3) << 2) + 0] = src[(i << 6) + 0];
		dest[0][i / 4][0][((i & 3) << 2) + 1] = src[(i << 6) + 1];
		dest[0][i / 4][0][((i & 3) << 2) + 2] = src[(i << 6) + 2];
		dest[0][i / 4][0][((i & 3) << 2) + 3] = src[(i << 6) + 3];
		dest[0][i / 4][1][((i & 3) << 2) + 0] = src[(i << 6) + 4];
		dest[0][i / 4][1][((i & 3) << 2) + 1] = src[(i << 6) + 5];
		dest[0][i / 4][1][((i & 3) << 2) + 2] = src[(i << 6) + 6];
		dest[0][i / 4][1][((i & 3) << 2) + 3] = src[(i << 6) + 7];
		dest[0][i / 4][2][((i & 3) << 2) + 0] = src[(i << 6) + 8];
		dest[0][i / 4][2][((i & 3) << 2) + 1] = src[(i << 6) + 9];
		dest[0][i / 4][2][((i & 3) << 2) + 2] = src[(i << 6) + 10];
		dest[0][i / 4][2][((i & 3) << 2) + 3] = src[(i << 6) + 11];
		dest[0][i / 4][3][((i & 3) << 2) + 0] = src[(i << 6) + 12];
		dest[0][i / 4][3][((i & 3) << 2) + 1] = src[(i << 6) + 13];
		dest[0][i / 4][3][((i & 3) << 2) + 2] = src[(i << 6) + 14];
		dest[0][i / 4][3][((i & 3) << 2) + 3] = src[(i << 6) + 15];
		dest[0][i / 4][4][((i & 3) << 2) + 0] = src[(i << 6) + 16];
		dest[0][i / 4][4][((i & 3) << 2) + 1] = src[(i << 6) + 17];
		dest[0][i / 4][4][((i & 3) << 2) + 2] = src[(i << 6) + 18];
		dest[0][i / 4][4][((i & 3) << 2) + 3] = src[(i << 6) + 19];
		dest[0][i / 4][5][((i & 3) << 2) + 0] = src[(i << 6) + 20];
		dest[0][i / 4][5][((i & 3) << 2) + 1] = src[(i << 6) + 21];
		dest[0][i / 4][5][((i & 3) << 2) + 2] = src[(i << 6) + 22];
		dest[0][i / 4][5][((i & 3) << 2) + 3] = src[(i << 6) + 23];
		dest[0][i / 4][6][((i & 3) << 2) + 0] = src[(i << 6) + 24];
		dest[0][i / 4][6][((i & 3) << 2) + 1] = src[(i << 6) + 25];
		dest[0][i / 4][6][((i & 3) << 2) + 2] = src[(i << 6) + 26];
		dest[0][i / 4][6][((i & 3) << 2) + 3] = src[(i << 6) + 27];
		dest[0][i / 4][7][((i & 3) << 2) + 0] = src[(i << 6) + 28];
		dest[0][i / 4][7][((i & 3) << 2) + 1] = src[(i << 6) + 29];
		dest[0][i / 4][7][((i & 3) << 2) + 2] = src[(i << 6) + 30];
		dest[0][i / 4][7][((i & 3) << 2) + 3] = src[(i << 6) + 31];
		dest[0][i / 4][8][((i & 3) << 2) + 0] = src[(i << 6) + 32];
		dest[0][i / 4][8][((i & 3) << 2) + 1] = src[(i << 6) + 33];
		dest[0][i / 4][8][((i & 3) << 2) + 2] = src[(i << 6) + 34];
		dest[0][i / 4][8][((i & 3) << 2) + 3] = src[(i << 6) + 35];
		dest[0][i / 4][9][((i & 3) << 2) + 0] = src[(i << 6) + 36];
		dest[0][i / 4][9][((i & 3) << 2) + 1] = src[(i << 6) + 37];
		dest[0][i / 4][9][((i & 3) << 2) + 2] = src[(i << 6) + 38];
		dest[0][i / 4][9][((i & 3) << 2) + 3] = src[(i << 6) + 39];
		dest[0][i / 4][10][((i & 3) << 2) + 0] = src[(i << 6) + 40];
		dest[0][i / 4][10][((i & 3) << 2) + 1] = src[(i << 6) + 41];
		dest[0][i / 4][10][((i & 3) << 2) + 2] = src[(i << 6) + 42];
		dest[0][i / 4][10][((i & 3) << 2) + 3] = src[(i << 6) + 43];
		dest[0][i / 4][11][((i & 3) << 2) + 0] = src[(i << 6) + 44];
		dest[0][i / 4][11][((i & 3) << 2) + 1] = src[(i << 6) + 45];
		dest[0][i / 4][11][((i & 3) << 2) + 2] = src[(i << 6) + 46];
		dest[0][i / 4][11][((i & 3) << 2) + 3] = src[(i << 6) + 47];
		dest[0][i / 4][12][((i & 3) << 2) + 0] = src[(i << 6) + 48];
		dest[0][i / 4][12][((i & 3) << 2) + 1] = src[(i << 6) + 49];
		dest[0][i / 4][12][((i & 3) << 2) + 2] = src[(i << 6) + 50];
		dest[0][i / 4][12][((i & 3) << 2) + 3] = src[(i << 6) + 51];
		dest[0][i / 4][13][((i & 3) << 2) + 0] = src[(i << 6) + 52];
		dest[0][i / 4][13][((i & 3) << 2) + 1] = src[(i << 6) + 53];
		dest[0][i / 4][13][((i & 3) << 2) + 2] = src[(i << 6) + 54];
		dest[0][i / 4][13][((i & 3) << 2) + 3] = src[(i << 6) + 55];
		dest[0][i / 4][14][((i & 3) << 2) + 0] = src[(i << 6) + 56];
		dest[0][i / 4][14][((i & 3) << 2) + 1] = src[(i << 6) + 57];
		dest[0][i / 4][14][((i & 3) << 2) + 2] = src[(i << 6) + 58];
		dest[0][i / 4][14][((i & 3) << 2) + 3] = src[(i << 6) + 59];
		dest[0][i / 4][15][((i & 3) << 2) + 0] = src[(i << 6) + 60];
		dest[0][i / 4][15][((i & 3) << 2) + 1] = src[(i << 6) + 61];
		dest[0][i / 4][15][((i & 3) << 2) + 2] = src[(i << 6) + 62];
		dest[0][i / 4][15][((i & 3) << 2) + 3] = src[(i << 6) + 63];
		dest1[0][i][0] = src[(i << 6) + 3];
		dest1[0][i][1] = src[(i << 6) + 7];
		dest1[0][i][2] = src[(i << 6) + 11];
		dest1[0][i][3] = src[(i << 6) + 15];
		dest1[0][i][4] = src[(i << 6) + 19];
		dest1[0][i][5] = src[(i << 6) + 23];
		dest1[0][i][6] = src[(i << 6) + 27];
		dest1[0][i][7] = src[(i << 6) + 31];
		dest1[0][i][8] = src[(i << 6) + 35];
		dest1[0][i][9] = src[(i << 6) + 39];
		dest1[0][i][10] = src[(i << 6) + 43];
		dest1[0][i][11] = src[(i << 6) + 47];
		dest1[0][i][12] = src[(i << 6) + 51];
		dest1[0][i][13] = src[(i << 6) + 55];
		dest1[0][i][14] = src[(i << 6) + 59];
		dest1[0][i][15] = src[(i << 6) + 63];
		dest[1][i / 4][0][((i & 3) << 2) + 0] = src[((i+32) << 6) + 0];
		dest[1][i / 4][0][((i & 3) << 2) + 1] = src[((i+32) << 6) + 1];
		dest[1][i / 4][0][((i & 3) << 2) + 2] = src[((i+32) << 6) + 2];
		dest[1][i / 4][0][((i & 3) << 2) + 3] = src[((i+32) << 6) + 3];
		dest[1][i / 4][1][((i & 3) << 2) + 0] = src[((i+32) << 6) + 4];
		dest[1][i / 4][1][((i & 3) << 2) + 1] = src[((i+32) << 6) + 5];
		dest[1][i / 4][1][((i & 3) << 2) + 2] = src[((i+32) << 6) + 6];
		dest[1][i / 4][1][((i & 3) << 2) + 3] = src[((i+32) << 6) + 7];
		dest[1][i / 4][2][((i & 3) << 2) + 0] = src[((i+32) << 6) + 8];
		dest[1][i / 4][2][((i & 3) << 2) + 1] = src[((i+32) << 6) + 9];
		dest[1][i / 4][2][((i & 3) << 2) + 2] = src[((i+32) << 6) + 10];
		dest[1][i / 4][2][((i & 3) << 2) + 3] = src[((i+32) << 6) + 11];
		dest[1][i / 4][3][((i & 3) << 2) + 0] = src[((i+32) << 6) + 12];
		dest[1][i / 4][3][((i & 3) << 2) + 1] = src[((i+32) << 6) + 13];
		dest[1][i / 4][3][((i & 3) << 2) + 2] = src[((i+32) << 6) + 14];
		dest[1][i / 4][3][((i & 3) << 2) + 3] = src[((i+32) << 6) + 15];
		dest[1][i / 4][4][((i & 3) << 2) + 0] = src[((i+32) << 6) + 16];
		dest[1][i / 4][4][((i & 3) << 2) + 1] = src[((i+32) << 6) + 17];
		dest[1][i / 4][4][((i & 3) << 2) + 2] = src[((i+32) << 6) + 18];
		dest[1][i / 4][4][((i & 3) << 2) + 3] = src[((i+32) << 6) + 19];
		dest[1][i / 4][5][((i & 3) << 2) + 0] = src[((i+32) << 6) + 20];
		dest[1][i / 4][5][((i & 3) << 2) + 1] = src[((i+32) << 6) + 21];
		dest[1][i / 4][5][((i & 3) << 2) + 2] = src[((i+32) << 6) + 22];
		dest[1][i / 4][5][((i & 3) << 2) + 3] = src[((i+32) << 6) + 23];
		dest[1][i / 4][6][((i & 3) << 2) + 0] = src[((i+32) << 6) + 24];
		dest[1][i / 4][6][((i & 3) << 2) + 1] = src[((i+32) << 6) + 25];
		dest[1][i / 4][6][((i & 3) << 2) + 2] = src[((i+32) << 6) + 26];
		dest[1][i / 4][6][((i & 3) << 2) + 3] = src[((i+32) << 6) + 27];
		dest[1][i / 4][7][((i & 3) << 2) + 0] = src[((i+32) << 6) + 28];
		dest[1][i / 4][7][((i & 3) << 2) + 1] = src[((i+32) << 6) + 29];
		dest[1][i / 4][7][((i & 3) << 2) + 2] = src[((i+32) << 6) + 30];
		dest[1][i / 4][7][((i & 3) << 2) + 3] = src[((i+32) << 6) + 31];
		dest[1][i / 4][8][((i & 3) << 2) + 0] = src[((i+32) << 6) + 32];
		dest[1][i / 4][8][((i & 3) << 2) + 1] = src[((i+32) << 6) + 33];
		dest[1][i / 4][8][((i & 3) << 2) + 2] = src[((i+32) << 6) + 34];
		dest[1][i / 4][8][((i & 3) << 2) + 3] = src[((i+32) << 6) + 35];
		dest[1][i / 4][9][((i & 3) << 2) + 0] = src[((i+32) << 6) + 36];
		dest[1][i / 4][9][((i & 3) << 2) + 1] = src[((i+32) << 6) + 37];
		dest[1][i / 4][9][((i & 3) << 2) + 2] = src[((i+32) << 6) + 38];
		dest[1][i / 4][9][((i & 3) << 2) + 3] = src[((i+32) << 6) + 39];
		dest[1][i / 4][10][((i & 3) << 2) + 0] = src[((i+32) << 6) + 40];
		dest[1][i / 4][10][((i & 3) << 2) + 1] = src[((i+32) << 6) + 41];
		dest[1][i / 4][10][((i & 3) << 2) + 2] = src[((i+32) << 6) + 42];
		dest[1][i / 4][10][((i & 3) << 2) + 3] = src[((i+32) << 6) + 43];
		dest[1][i / 4][11][((i & 3) << 2) + 0] = src[((i+32) << 6) + 44];
		dest[1][i / 4][11][((i & 3) << 2) + 1] = src[((i+32) << 6) + 45];
		dest[1][i / 4][11][((i & 3) << 2) + 2] = src[((i+32) << 6) + 46];
		dest[1][i / 4][11][((i & 3) << 2) + 3] = src[((i+32) << 6) + 47];
		dest[1][i / 4][12][((i & 3) << 2) + 0] = src[((i+32) << 6) + 48];
		dest[1][i / 4][12][((i & 3) << 2) + 1] = src[((i+32) << 6) + 49];
		dest[1][i / 4][12][((i & 3) << 2) + 2] = src[((i+32) << 6) + 50];
		dest[1][i / 4][12][((i & 3) << 2) + 3] = src[((i+32) << 6) + 51];
		dest[1][i / 4][13][((i & 3) << 2) + 0] = src[((i+32) << 6) + 52];
		dest[1][i / 4][13][((i & 3) << 2) + 1] = src[((i+32) << 6) + 53];
		dest[1][i / 4][13][((i & 3) << 2) + 2] = src[((i+32) << 6) + 54];
		dest[1][i / 4][13][((i & 3) << 2) + 3] = src[((i+32) << 6) + 55];
		dest[1][i / 4][14][((i & 3) << 2) + 0] = src[((i+32) << 6) + 56];
		dest[1][i / 4][14][((i & 3) << 2) + 1] = src[((i+32) << 6) + 57];
		dest[1][i / 4][14][((i & 3) << 2) + 2] = src[((i+32) << 6) + 58];
		dest[1][i / 4][14][((i & 3) << 2) + 3] = src[((i+32) << 6) + 59];
		dest[1][i / 4][15][((i & 3) << 2) + 0] = src[((i+32) << 6) + 60];
		dest[1][i / 4][15][((i & 3) << 2) + 1] = src[((i+32) << 6) + 61];
		dest[1][i / 4][15][((i & 3) << 2) + 2] = src[((i+32) << 6) + 62];
		dest[1][i / 4][15][((i & 3) << 2) + 3] = src[((i+32) << 6) + 63];
		dest1[1][i][0] = src[((i+32) << 6) + 3];
		dest1[1][i][1] = src[((i+32) << 6) + 7];
		dest1[1][i][2] = src[((i+32) << 6) + 11];
		dest1[1][i][3] = src[((i+32) << 6) + 15];
		dest1[1][i][4] = src[((i+32) << 6) + 19];
		dest1[1][i][5] = src[((i+32) << 6) + 23];
		dest1[1][i][6] = src[((i+32) << 6) + 27];
		dest1[1][i][7] = src[((i+32) << 6) + 31];
		dest1[1][i][8] = src[((i+32) << 6) + 35];
		dest1[1][i][9] = src[((i+32) << 6) + 39];
		dest1[1][i][10] = src[((i+32) << 6) + 43];
		dest1[1][i][11] = src[((i+32) << 6) + 47];
		dest1[1][i][12] = src[((i+32) << 6) + 51];
		dest1[1][i][13] = src[((i+32) << 6) + 55];
		dest1[1][i][14] = src[((i+32) << 6) + 59];
		dest1[1][i][15] = src[((i+32) << 6) + 63];
	}
}

void copy_up_org(pel src[MAX_CU_SIZE2 * MAX_CU_SIZE2], pel dest[2][8][64]) {
	#pragma HLS inline
#pragma HLS ARRAY_PARTITION variable=src cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=dest complete dim=3
	for (int i = 0; i < 16; i++) {
#pragma HLS PIPELINE
		/*for (int j = 0; j < 64; j++){
			dest[0][i][j] = src[(((i << 2) + 3) << 6) + j];
			dest[1][i][j] = src[((((i+8) << 2) + 3) << 6) + j];
		}*/
		 if (i < 8) {
			 for (int j = 0; j < 64; j++)
				 dest[0][i][j] = src[(((i << 2) + 3) << 6) + j];
		 }
		 else {
			 for (int j = 0; j < 64; j++)
				 dest[1][i - 8][j] = src[(((i << 2) + 3) << 6) + j];
		 }
	}
}


void split_up_ref(const pel save_boder_up[max_buffer_width], U12 pix_x, pel up_ref[68])
{
#pragma HLS ARRAY_PARTITION variable=up_ref complete dim=1
#pragma HLS ARRAY_PARTITION variable=save_boder_up cyclic factor=64 dim=1//16
	HLS_DIRECTIVE(#pragma HLS inline off)
		for (int m = 0; m < 68; m++) {
#pragma HLS PIPELINE
			if (pix_x + m < max_buffer_width) {
				up_ref[m] = save_boder_up[pix_x + m];
			}
		}
}

void get_rmd_pixel_4x4(u16 avail_cu, int bit_depth, pel left[4 * 2 + 3], pel up[4 * 2 + 3], int x, int y,
	const pel save_boder_left[MAX_CU_SIZE2], pel top_left, pel up_ref[68], PIX_ORG* pix_org, pel up_left)
{
#pragma HLS ARRAY_PARTITION variable=pix_org->leftdown_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->left_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->upright_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->up_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=up_ref complete dim=1
#pragma HLS ARRAY_PARTITION variable=save_boder_left complete dim=1
#pragma HLS ARRAY_PARTITION variable=up complete dim=1
#pragma HLS ARRAY_PARTITION variable=left complete dim=1
	HLS_DIRECTIVE(#pragma HLS inline off)
		HLS_DIRECTIVE(#pragma HLS PIPELINE)
		int pic_x = x & (MAX_CU_SIZE2 - 1);
	int pic_y = y & (MAX_CU_SIZE2 - 1);
	if (IS_AVAIL(avail_cu, AVAIL_UP))
	{
		if (pic_y == 0) {
			if (x + 8 <= max_buffer_width) {//should less than pic_width,Not max_buffer_width
				for (int i = 0; i < 2 * 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						up[3 + i] = up_ref[pic_x + i];
				}
			}
			else {
				for (int i = 0; i < 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						up[3 + i] = up_ref[pic_x + i];
				}
				for (int i = 4; i < 2 * 4; i++) { //W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						up[3 + i] = up_ref[MAX_CU_SIZE2 - 1];
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {///W
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					up[3 + i] = pix_org->up_org[i];
			}
			if (pic_x + 8 <= MAX_CU_SIZE2) {
				for (int i = 4; i < 2 * 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						up[3 + i] = pix_org->upright_org[i - 4];
				}
			}
			else {
				for (int i = 4; i < 2 * 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						up[3 + i] = pix_org->up_org[3];
				}
			}
		}
	}
	else {
		for (s16 i = 0; i < 2 * 4; i++) {///
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				up[3 + i] = 1 << (bit_depth - 1);
		}
	}
	if (IS_AVAIL(avail_cu, AVAIL_LE))
	{
		if (pic_x == 0) {
			if (pic_y + 8 <= MAX_CU_SIZE2) {
				for (int i = 0; i < 2 * 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						left[3 + i] = save_boder_left[pic_y + i];
				}
			}
			else {
				for (int i = 0; i < 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						left[3 + i] = save_boder_left[pic_y + i];
				}
				for (int i = 4; i < 2 * 4; i++) { //W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						left[3 + i] = save_boder_left[MAX_CU_SIZE2 - 1];
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {///W
				HLS_DIRECTIVE(#pragma HLS UNROLL)
					left[3 + i] = pix_org->left_org[i];
			}
			if (pic_y + 8 <= MAX_CU_SIZE2) {
				for (int i = 4; i < 2 * 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						left[3 + i] = pix_org->leftdown_org[i - 4];
				}
			}
			else {
				for (int i = 4; i < 2 * 4; i++) {///W
					HLS_DIRECTIVE(#pragma HLS UNROLL)
						left[3 + i] = pix_org->left_org[3];
				}
			}
		}
	}
	else {
		for (s16 i = 0; i < 2 * 4; i++) {///
			HLS_DIRECTIVE(#pragma HLS UNROLL)
				left[3 + i] = 1 << (bit_depth - 1);
		}
	}
	if (IS_AVAIL(avail_cu, AVAIL_UP_LE))
	{
		if (pic_y == 0 && pic_x == 0) {
			up[2] = left[2] = top_left;
		}
		else if (pic_y == 0) {
			up[2] = left[2] = up_ref[pic_x - 1];
		}
		else if (pic_x == 0) {
			up[2] = left[2] = save_boder_left[pic_y - 1];
		}
		else {
			up[2] = left[2] = up_left;
		}
	}
	else if (IS_AVAIL(avail_cu, AVAIL_UP))
	{
		up[2] = left[2] = up[3];
	}
	else if (IS_AVAIL(avail_cu, AVAIL_LE))
	{
		up[2] = left[2] = left[3];
	}
	else {
		up[2] = left[2] = (1 << (bit_depth - 1));
	}
	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}
#if USE_SPEED_LEVEL
void sort_modes(u32 satd, u8 rmd_output_mode[IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u32 cost_reg[IPD_RDO_CNT + EXTRA_RMD_MODE], U2 en, U8 speed_level)
#else
void sort_modes(u32 satd, u8 rmd_output_mode[IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u32 cost_reg[IPD_RDO_CNT + EXTRA_RMD_MODE], U2 en)
#endif
{
#pragma HLS ARRAY_PARTITION variable=cost_reg complete
#pragma HLS ARRAY_PARTITION variable=rmd_output_mode complete
	static u32 tmp0, tmp1;
	static int tmp_ip0, tmp_ip1;
#if USE_SPEED_LEVEL
	int max_rmd_mode_cnt = 5;
	if (speed_level == 1)
		max_rmd_mode_cnt = IPD_RDO_CNT; //5
	if (speed_level == 2)
		max_rmd_mode_cnt = EXTRA_RMD_MODE;//10
	else
		max_rmd_mode_cnt = IPD_RDO_CNT + EXTRA_RMD_MODE;//15
#else
	int max_rmd_mode_cnt = 5;
#endif
	if (en) {
		if (mode == 0 || mode == 34) {
			cost_reg[0] = satd;
			rmd_output_mode[0] = mode == 0 ? 0 : 34;
			for (int k = 1; k < max_rmd_mode_cnt; k++) {
#pragma HLS PIPELINE
				cost_reg[k] = 0xffffffff;
			}
		}
		else {
			tmp0 = satd;
			tmp_ip0 = mode;
			int shift = 0;
			for (int i = 0; i < max_rmd_mode_cnt; i++)
			{
				if (satd < cost_reg[max_rmd_mode_cnt - i - 1]) {
					shift++;
				}
			}
			if (shift != 0) {
				for (int j = 0; j < max_rmd_mode_cnt; j++) {
					if (j >= (max_rmd_mode_cnt - shift)) {
						tmp1 = tmp0;
						tmp0 = cost_reg[j];
						cost_reg[j] = tmp1;
						tmp_ip1 = tmp_ip0;
						tmp_ip0 = rmd_output_mode[j];
						rmd_output_mode[j] = tmp_ip1;
					}
				}
			}
		}
	}
}

void sort_modes_for64(u32 satd, u8 rmd_output_mode[IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u32 cost_reg[IPD_RDO_CNT + EXTRA_RMD_MODE], U2 en)
{
#pragma HLS ARRAY_PARTITION variable=cost_reg complete
#pragma HLS ARRAY_PARTITION variable=rmd_output_mode complete
	static u32 tmp0, tmp1;
	static int tmp_ip0, tmp_ip1;
#if USE_SPEED_LEVEL
	int max_rmd_mode_cnt = 5;
	if (speed_level == 1)
		max_rmd_mode_cnt = IPD_RDO_CNT; //5
	if (speed_level == 2)
		max_rmd_mode_cnt = EXTRA_RMD_MODE;//10
	else
		max_rmd_mode_cnt = IPD_RDO_CNT + EXTRA_RMD_MODE;//15
#else
	int max_rmd_mode_cnt = 5;
#endif
	if (en) {
		if (mode == 0/* || mode == 34*/) {
			cost_reg[0] = satd;
			rmd_output_mode[0] = mode == 0 ? 0 : 34;
			for (int k = 1; k < max_rmd_mode_cnt; k++) {
#pragma HLS PIPELINE
				cost_reg[k] = 0xffffffff;
			}
		}
		else {
			tmp0 = satd;
			tmp_ip0 = mode;
			int shift = 0;
			for (int i = 0; i < max_rmd_mode_cnt; i++)
			{
				if (satd < cost_reg[max_rmd_mode_cnt - i - 1]) {
					shift++;
				}
			}
			if (shift != 0) {
				for (int j = 0; j < max_rmd_mode_cnt; j++) {
					if (j >= (max_rmd_mode_cnt - shift)) {
						tmp1 = tmp0;
						tmp0 = cost_reg[j];
						cost_reg[j] = tmp1;
						tmp_ip1 = tmp_ip0;
						tmp_ip0 = rmd_output_mode[j];
						rmd_output_mode[j] = tmp_ip1;
					}
				}
			}
		}
	}
}



#if USE_SPEED_LEVEL
void load_tmp(u32 src[IPD_RDO_CNT + EXTRA_RMD_MODE], u32 dest[IPD_RDO_CNT + EXTRA_RMD_MODE], u8 src1[IPD_RDO_CNT + EXTRA_RMD_MODE], u8 dest1[IPD_RDO_CNT + EXTRA_RMD_MODE], U8 speed_level)
#else
void load_tmp(u32 src[IPD_RDO_CNT + EXTRA_RMD_MODE], u32 dest[IPD_RDO_CNT + EXTRA_RMD_MODE], u8 src1[IPD_RDO_CNT + EXTRA_RMD_MODE], u8 dest1[IPD_RDO_CNT + EXTRA_RMD_MODE])
#endif
{
#if USE_SPEED_LEVEL
	int max_rmd_mode_cnt = 5;
	if (speed_level == 1)
		max_rmd_mode_cnt = IPD_RDO_CNT; //5
	if (speed_level == 2)
		max_rmd_mode_cnt = EXTRA_RMD_MODE;//10
	else
		max_rmd_mode_cnt = IPD_RDO_CNT + EXTRA_RMD_MODE;//15
#else
	int max_rmd_mode_cnt = 5;
#endif
	for (int i = 0; i < max_rmd_mode_cnt; i++) {
#pragma HLS UNROLL
		dest[i] = src[i];
		dest1[i] = src1[i];
	}
}

void get_enable(int x, int y, EN_CONTROL* en) {
#pragma HLS inline off
	int col = y >> 2;
	int row = x >> 2;
	/*if(col % 2 == 1)
		en->en_4x8 = 1;
	else
		en->en_4x8 = 0;

	if((col + 1) % 4 == 0)
		en->en_4x16 = 1;
	else
		en->en_4x16 = 0;

	if((col + 1) % 8 == 0)
		en->en_4x32 = 1;
	else
		en->en_4x32 = 0;

	if(row % 2 == 1)
		en->en_8x4 = 1;
	else
		en->en_8x4 = 0;*/

	if ((row % 2 == 1) && (col % 2 == 1))
		en->en_8x8 = 1;
	else
		en->en_8x8 = 0;

	if ((row % 2 == 1) && ((col + 1) % 4 == 0))
		en->en_8x16 = 1;
	else
		en->en_8x16 = 0;

	if ((row % 2 == 1) && ((col + 1) % 8 == 0))
		en->en_8x32 = 1;
	else
		en->en_8x32 = 0;

	/*if((row + 1) % 4 == 0)
		en->en_16x4 = 1;
	else
		en->en_16x4 = 0;*/

	if (((row + 1) % 4 == 0) && (col % 2 == 1))
		en->en_16x8 = 1;
	else
		en->en_16x8 = 0;

	if (((row + 1) % 4 == 0) && ((col + 1) % 4 == 0))
		en->en_16x16 = 1;
	else
		en->en_16x16 = 0;

	if (((row + 1) % 4 == 0) && ((col + 1) % 8 == 0))
		en->en_16x32 = 1;
	else
		en->en_16x32 = 0;

	//if((row + 1) % 8 == 0)
	//	en->en_32x4 = 1;
	//else
	//	en->en_32x4 = 0;

	if (((row + 1) % 8 == 0) && ((col + 1) % 2 == 0))
		en->en_32x8 = 1;
	else
		en->en_32x8 = 0;

	if (((row + 1) % 8 == 0) && ((col + 1) % 4 == 0))
		en->en_32x16 = 1;
	else
		en->en_32x16 = 0;

	if (((row + 1) % 8 == 0) && ((col + 1) % 8 == 0))
		en->en_32x32 = 1;
	else
		en->en_32x32 = 0;

	if (((row + 1) % 16 == 0) && ((col + 1) % 2 == 0))
		en->en_64x8 = 1;
	else
		en->en_64x8 = 0;

	if (((row + 1) % 16 == 0) && ((col + 1) % 4 == 0))
		en->en_64x16 = 1;
	else
		en->en_64x16 = 0;

	if (((row + 1) % 16 == 0) && ((col + 1) % 8 == 0))
		en->en_64x32 = 1;
	else
		en->en_64x32 = 0;

}

void cal_cost(int x, int y, u32 satd, COST_REG* cost, u32* cost_8x32) {
#pragma HLS inline off
	int col = y >> 2;
	int row = x >> 2;
	if (col % 2 == 1)
		cost->data_reg_4x8 += satd;
	else
		cost->data_reg_4x8 = satd;

	if ((col + 1) % 4 == 0)
		cost->data_reg_4x16 += cost->data_reg_4x8;
	else if (col % 2 == 1)
		cost->data_reg_4x16 = cost->data_reg_4x8;

	if ((col + 1) % 8 == 0)
		cost->data_reg_4x32 += cost->data_reg_4x16;
	else if ((col + 1) % 4 == 0)
		cost->data_reg_4x32 = cost->data_reg_4x16;

	if (row % 2 == 1)
		cost->data_reg_8x4[col] += satd;
	else
		cost->data_reg_8x4[col] = satd;

	if ((row % 2 == 1) && (col % 2 == 1))
		cost->data_reg_8x8[col / 2] += cost->data_reg_4x8;
	else if (col % 2 == 1)
		cost->data_reg_8x8[col / 2] = cost->data_reg_4x8;

	if ((row % 2 == 1) && ((col + 1) % 4 == 0))
		cost->data_reg_8x16[col / 4] += cost->data_reg_4x16;
	else if ((col + 1) % 4 == 0)
		cost->data_reg_8x16[col / 4] = cost->data_reg_4x16;

	if ((row % 2 == 1) && ((col + 1) % 8 == 0)) {
		cost->data_reg_8x32 += cost->data_reg_4x32;
		*cost_8x32 = cost->data_reg_8x32;
	}
	else if ((col + 1) % 8 == 0)
		cost->data_reg_8x32 = cost->data_reg_4x32;


	if ((row + 1) % 4 == 0)
		cost->data_reg_16x4[col] += cost->data_reg_8x4[col];
	else if (row % 2 == 1)
		cost->data_reg_16x4[col] = cost->data_reg_8x4[col];

	if (((row + 1) % 4 == 0) && (col % 2 == 1))
		cost->data_reg_16x8 += cost->data_reg_16x4[col];
	else if ((row + 1) % 4 == 0)
		cost->data_reg_16x8 = cost->data_reg_16x4[col];

	if (((row + 1) % 4 == 0) && ((col + 1) % 4 == 0))
		cost->data_reg_16x16[col / 4] += cost->data_reg_8x16[col / 4];
	else if ((row % 2 == 1) && ((col + 1) % 4 == 0))
		cost->data_reg_16x16[col / 4] = cost->data_reg_8x16[col / 4];

	if (((row + 1) % 4 == 0) && ((col + 1) % 8 == 0))
		cost->data_reg_16x32 += cost->data_reg_8x32;
	else if ((row % 2 == 1) && ((col + 1) % 8 == 0))
		cost->data_reg_16x32 = cost->data_reg_8x32;

	if ((row + 1) % 8 == 0)
		cost->data_reg_32x4[col] += cost->data_reg_16x4[col];
	else if ((row + 1) % 4 == 0)
		cost->data_reg_32x4[col] = cost->data_reg_16x4[col];

	if (((row + 1) % 8 == 0) && ((col + 1) % 2 == 0))
		cost->data_reg_32x8 += cost->data_reg_32x4[col];
	else if ((row + 1) % 8 == 0)
		cost->data_reg_32x8 = cost->data_reg_32x4[col];

	if (((row + 1) % 8 == 0) && ((col + 1) % 4 == 0))
		cost->data_reg_32x16 += cost->data_reg_32x8;
	else if (((row + 1) % 8 == 0) && ((col + 1) % 2 == 0))
		cost->data_reg_32x16 = cost->data_reg_32x8;

	if (((row + 1) % 8 == 0) && ((col + 1) % 8 == 0))
		cost->data_reg_32x32 += cost->data_reg_16x32;
	else if (((row + 1) % 4 == 0) && ((col + 1) % 8 == 0))
		cost->data_reg_32x32 = cost->data_reg_16x32;

	if (((row + 1) % 16 == 0) && ((col + 1) % 2 == 0))
		cost->data_reg_64x8[col / 2] += cost->data_reg_32x8;
	else if (((row + 1) % 8 == 0) && ((col + 1) % 2 == 0))
		cost->data_reg_64x8[col / 2] = cost->data_reg_32x8;

	if (((row + 1) % 16 == 0) && ((col + 1) % 4 == 0))
		cost->data_reg_64x16 += cost->data_reg_64x8[col / 2];
	else if (((row + 1) % 16 == 0) && ((col + 1) % 2 == 0))
		cost->data_reg_64x16 = cost->data_reg_64x8[col / 2];

	if (((row + 1) % 16 == 0) && ((col + 1) % 8 == 0))
		cost->data_reg_64x32 += cost->data_reg_64x16;
	else if (((row + 1) % 16 == 0) && ((col + 1) % 4 == 0))
		cost->data_reg_64x32 = cost->data_reg_64x16;
}

#if USE_SPEED_LEVEL
void load_regs(int x, int y, COST_MEM* cost_mem, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], RMD_OUT_TMP* rmd_output, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], U8 speed_level)
#else
void load_regs(int x, int y, COST_MEM* cost_mem, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], RMD_OUT_TMP* rmd_output, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE])
#endif
{
#pragma HLS inline off
	int row = x >> 2;
	int col = y >> 2;
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_4x8 cyclic factor=5 dim=2
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x4 cyclic factor=5 dim=2
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_4x16 cyclic factor=5 dim=2
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x4 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x8 cyclic factor=5 dim=2
//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_4x32 cyclic factor=5 dim=2
//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x4 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x8 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x32 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x8 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_64x8 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x32 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_64x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x32 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_64x32 cyclic factor=5 dim=2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x4 cyclic factor=5 dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_4x8 cyclic factor=5 dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_4x16 cyclic factor=5 dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x4 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x8 cyclic factor=5 dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_4x32 cyclic factor=5 dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x4 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x16 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x8 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x32 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x8 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x16 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x8 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x32 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x16 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x16 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x32 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x32 cyclic factor=5 dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=1
		//load_tmp(cost_mem->cost_4x8[((row)<<2)+(col>>1)],cost_reg[0],rmd_output->modes_4x8[((row)<<2)+(col>>1)],ip_reg[0]);

		//load_tmp(cost_mem->cost_4x16[((row)<<1)+(col>>2)],cost_reg[1],rmd_output->modes_4x16[((row)<<1)+(col>>2)],ip_reg[1]);

		//load_tmp(cost_mem->cost_4x32[row+(col>>3)],cost_reg[2],rmd_output->modes_4x32[(row)+(col>>3)],ip_reg[2]);

		//load_tmp(cost_mem->cost_8x4[((row>>1)<<3)+col],cost_reg[3],rmd_output->modes_8x4[((row>>1)<<3)+col],ip_reg[3]);
#if USE_SPEED_LEVEL
	load_tmp(cost_mem->cost_8x8[((row >> 1) << 2) + (col >> 1)], cost_reg[4], rmd_output->modes_8x8[((row >> 1) << 2) + (col >> 1)], ip_reg[4], speed_level);

	load_tmp(cost_mem->cost_8x16[((row >> 1) << 1) + (col >> 2)], cost_reg[5], rmd_output->modes_8x16[((row >> 1) << 1) + (col >> 2)], ip_reg[5], speed_level);

	load_tmp(cost_mem->cost_8x32[(row >> 1) + (col >> 3)], cost_reg[6], rmd_output->modes_8x32[(row >> 1) + (col >> 3)], ip_reg[6], speed_level);

	//load_tmp(cost_mem->cost_16x4[((row>>2)<<3)+col],cost_reg[8],rmd_output->modes_16x4[((row>>2)<<3)+col],ip_reg[8]);

	load_tmp(cost_mem->cost_16x8[((row >> 2) << 2) + (col >> 1)], cost_reg[9], rmd_output->modes_16x8[((row >> 2) << 2) + (col >> 1)], ip_reg[9], speed_level);

	load_tmp(cost_mem->cost_16x16[((row >> 2) << 1) + (col >> 2)], cost_reg[10], rmd_output->modes_16x16[((row >> 2) << 1) + (col >> 2)], ip_reg[10], speed_level);

	load_tmp(cost_mem->cost_16x32[(row >> 2) + (col >> 3)], cost_reg[11], rmd_output->modes_16x32[(row >> 2) + (col >> 3)], ip_reg[11], speed_level);

	//load_tmp(cost_mem->cost_32x4[((row>>3)<<3)+ col],cost_reg[13],rmd_output->modes_32x4[((row>>3)<<3)+ col],ip_reg[13]);

	load_tmp(cost_mem->cost_32x8[((row >> 3) << 2) + (col >> 1)], cost_reg[14], rmd_output->modes_32x8[((row >> 3) << 2) + (col >> 1)], ip_reg[14], speed_level);

	load_tmp(cost_mem->cost_32x16[((row >> 3) << 1) + (col >> 2)], cost_reg[15], rmd_output->modes_32x16[((row >> 3) << 1) + (col >> 2)], ip_reg[15], speed_level);

	load_tmp(cost_mem->cost_32x32[(row >> 3) + (col >> 3)], cost_reg[16], rmd_output->modes_32x32[(row >> 3) + (col >> 3)], ip_reg[16], speed_level);

	load_tmp(cost_mem->cost_64x8[col / 2], cost_reg[18], rmd_output->modes_64x8[col / 2], ip_reg[18], speed_level);

	load_tmp(cost_mem->cost_64x16[col / 4], cost_reg[19], rmd_output->modes_64x16[col / 4], ip_reg[19], speed_level);

	load_tmp(cost_mem->cost_64x32[col / 8], cost_reg[20], rmd_output->modes_64x32[col / 8], ip_reg[20], speed_level);
#else
	load_tmp(cost_mem->cost_8x8[((row >> 1) << 2) + (col >> 1)], cost_reg[4], rmd_output->modes_8x8[((row >> 1) << 2) + (col >> 1)], ip_reg[4]);

	load_tmp(cost_mem->cost_8x16[((row >> 1) << 1) + (col >> 2)], cost_reg[5], rmd_output->modes_8x16[((row >> 1) << 1) + (col >> 2)], ip_reg[5]);

	load_tmp(cost_mem->cost_8x32[(row >> 1) + (col >> 3)], cost_reg[6], rmd_output->modes_8x32[(row >> 1) + (col >> 3)], ip_reg[6]);

	//load_tmp(cost_mem->cost_16x4[((row>>2)<<3)+col],cost_reg[8],rmd_output->modes_16x4[((row>>2)<<3)+col],ip_reg[8]);

	load_tmp(cost_mem->cost_16x8[((row >> 2) << 2) + (col >> 1)], cost_reg[9], rmd_output->modes_16x8[((row >> 2) << 2) + (col >> 1)], ip_reg[9]);

	load_tmp(cost_mem->cost_16x16[((row >> 2) << 1) + (col >> 2)], cost_reg[10], rmd_output->modes_16x16[((row >> 2) << 1) + (col >> 2)], ip_reg[10]);

	load_tmp(cost_mem->cost_16x32[(row >> 2) + (col >> 3)], cost_reg[11], rmd_output->modes_16x32[(row >> 2) + (col >> 3)], ip_reg[11]);

	//load_tmp(cost_mem->cost_32x4[((row>>3)<<3)+ col],cost_reg[13],rmd_output->modes_32x4[((row>>3)<<3)+ col],ip_reg[13]);

	load_tmp(cost_mem->cost_32x8[((row >> 3) << 2) + (col >> 1)], cost_reg[14], rmd_output->modes_32x8[((row >> 3) << 2) + (col >> 1)], ip_reg[14]);

	load_tmp(cost_mem->cost_32x16[((row >> 3) << 1) + (col >> 2)], cost_reg[15], rmd_output->modes_32x16[((row >> 3) << 1) + (col >> 2)], ip_reg[15]);

	load_tmp(cost_mem->cost_32x32[(row >> 3) + (col >> 3)], cost_reg[16], rmd_output->modes_32x32[(row >> 3) + (col >> 3)], ip_reg[16]);

	load_tmp(cost_mem->cost_64x8[col / 2], cost_reg[18], rmd_output->modes_64x8[col / 2], ip_reg[18]);

	load_tmp(cost_mem->cost_64x16[col / 4], cost_reg[19], rmd_output->modes_64x16[col / 4], ip_reg[19]);

	load_tmp(cost_mem->cost_64x32[col / 8], cost_reg[20], rmd_output->modes_64x32[col / 8], ip_reg[20]);

#endif
}

#if USE_SPEED_LEVEL
void stor_regs(int x, int y, COST_MEM* cost_mem, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], RMD_OUT_TMP* rmd_output, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], U8 speed_level) {
#else
void stor_regs(int x, int y, COST_MEM * cost_mem, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], RMD_OUT_TMP * rmd_output, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE]) {
#endif
	int row = x >> 2;
	int col = y >> 2;
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_4x8 cyclic factor=5 dim=2
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x4 cyclic factor=5 dim=2
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_4x16 cyclic factor=5 dim=2
	//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x4 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x8 cyclic factor=5 dim=2
//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_4x32 cyclic factor=5 dim=2
//#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x4 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x8 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_8x32 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x8 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_64x8 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_16x32 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_64x16 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_32x32 cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable = cost_mem->cost_64x32 cyclic factor=5 dim=2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x4 complete dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_4x8 complete dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_4x16 complete dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x4 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x8 complete dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_4x32 complete dim = 2
//#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x4 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=1

		//load_tmp(cost_reg[0],cost_mem->cost_4x8[((row)<<2)+(col>>1)],ip_reg[0],rmd_output->modes_4x8[((row)<<2)+(col>>1)]);

		//load_tmp(cost_reg[1],cost_mem->cost_4x16[((row)<<1)+(col>>2)],ip_reg[1],rmd_output->modes_4x16[((row)<<1)+(col>>2)]);

		//load_tmp(cost_reg[2],cost_mem->cost_4x32[row+(col>>3)],ip_reg[2],rmd_output->modes_4x32[row+(col>>3)]);

		//load_tmp(cost_reg[3],cost_mem->cost_8x4[((row>>1)<<3)+col],ip_reg[3],rmd_output->modes_8x4[((row>>1)<<3)+col]);
#if USE_SPEED_LEVEL
	load_tmp(cost_reg[4], cost_mem->cost_8x8[((row >> 1) << 2) + (col >> 1)], ip_reg[4], rmd_output->modes_8x8[((row >> 1) << 2) + (col >> 1)], speed_level);

	load_tmp(cost_reg[5], cost_mem->cost_8x16[((row >> 1) << 1) + (col >> 2)], ip_reg[5], rmd_output->modes_8x16[((row >> 1) << 1) + (col >> 2)], speed_level);

	load_tmp(cost_reg[6], cost_mem->cost_8x32[(row >> 1) + (col >> 3)], ip_reg[6], rmd_output->modes_8x32[(row >> 1) + (col >> 3)], speed_level);

	//load_tmp(cost_reg[8],cost_mem->cost_16x4[((row>>2)<<3)+col],ip_reg[8],rmd_output->modes_16x4[((row>>2)<<3)+col]);

	load_tmp(cost_reg[9], cost_mem->cost_16x8[((row >> 2) << 2) + (col >> 1)], ip_reg[9], rmd_output->modes_16x8[((row >> 2) << 2) + (col >> 1)], speed_level);

	load_tmp(cost_reg[10], cost_mem->cost_16x16[((row >> 2) << 1) + (col >> 2)], ip_reg[10], rmd_output->modes_16x16[((row >> 2) << 1) + (col >> 2)], speed_level);

	load_tmp(cost_reg[11], cost_mem->cost_16x32[(row >> 2) + (col >> 3)], ip_reg[11], rmd_output->modes_16x32[(row >> 2) + (col >> 3)], speed_level);

	//load_tmp(cost_reg[13],cost_mem->cost_32x4[((row>>3)<<3)+ col],ip_reg[13],rmd_output->modes_32x4[((row>>3)<<3)+ col]);

	load_tmp(cost_reg[14], cost_mem->cost_32x8[((row >> 3) << 2) + (col >> 1)], ip_reg[14], rmd_output->modes_32x8[((row >> 3) << 2) + (col >> 1)], speed_level);

	load_tmp(cost_reg[15], cost_mem->cost_32x16[((row >> 3) << 1) + (col >> 2)], ip_reg[15], rmd_output->modes_32x16[((row >> 3) << 1) + (col >> 2)], speed_level);

	load_tmp(cost_reg[16], cost_mem->cost_32x32[(row >> 3) + (col >> 3)], ip_reg[16], rmd_output->modes_32x32[(row >> 3) + (col >> 3)], speed_level);

	load_tmp(cost_reg[18], cost_mem->cost_64x8[col / 2], ip_reg[18], rmd_output->modes_64x8[col / 2], speed_level);

	load_tmp(cost_reg[19], cost_mem->cost_64x16[col / 4], ip_reg[19], rmd_output->modes_64x16[col / 4], speed_level);

	load_tmp(cost_reg[20], cost_mem->cost_64x32[col / 8], ip_reg[20], rmd_output->modes_64x32[col / 8], speed_level);
#else
	load_tmp(cost_reg[4], cost_mem->cost_8x8[((row >> 1) << 2) + (col >> 1)], ip_reg[4], rmd_output->modes_8x8[((row >> 1) << 2) + (col >> 1)]);

	load_tmp(cost_reg[5], cost_mem->cost_8x16[((row >> 1) << 1) + (col >> 2)], ip_reg[5], rmd_output->modes_8x16[((row >> 1) << 1) + (col >> 2)]);

	load_tmp(cost_reg[6], cost_mem->cost_8x32[(row >> 1) + (col >> 3)], ip_reg[6], rmd_output->modes_8x32[(row >> 1) + (col >> 3)]);

	//load_tmp(cost_reg[8],cost_mem->cost_16x4[((row>>2)<<3)+col],ip_reg[8],rmd_output->modes_16x4[((row>>2)<<3)+col]);

	load_tmp(cost_reg[9], cost_mem->cost_16x8[((row >> 2) << 2) + (col >> 1)], ip_reg[9], rmd_output->modes_16x8[((row >> 2) << 2) + (col >> 1)]);

	load_tmp(cost_reg[10], cost_mem->cost_16x16[((row >> 2) << 1) + (col >> 2)], ip_reg[10], rmd_output->modes_16x16[((row >> 2) << 1) + (col >> 2)]);

	load_tmp(cost_reg[11], cost_mem->cost_16x32[(row >> 2) + (col >> 3)], ip_reg[11], rmd_output->modes_16x32[(row >> 2) + (col >> 3)]);

	//load_tmp(cost_reg[13],cost_mem->cost_32x4[((row>>3)<<3)+ col],ip_reg[13],rmd_output->modes_32x4[((row>>3)<<3)+ col]);

	load_tmp(cost_reg[14], cost_mem->cost_32x8[((row >> 3) << 2) + (col >> 1)], ip_reg[14], rmd_output->modes_32x8[((row >> 3) << 2) + (col >> 1)]);

	load_tmp(cost_reg[15], cost_mem->cost_32x16[((row >> 3) << 1) + (col >> 2)], ip_reg[15], rmd_output->modes_32x16[((row >> 3) << 1) + (col >> 2)]);

	load_tmp(cost_reg[16], cost_mem->cost_32x32[(row >> 3) + (col >> 3)], ip_reg[16], rmd_output->modes_32x32[(row >> 3) + (col >> 3)]);

	load_tmp(cost_reg[18], cost_mem->cost_64x8[col / 2], ip_reg[18], rmd_output->modes_64x8[col / 2]);

	load_tmp(cost_reg[19], cost_mem->cost_64x16[col / 4], ip_reg[19], rmd_output->modes_64x16[col / 4]);

	load_tmp(cost_reg[20], cost_mem->cost_64x32[col / 8], ip_reg[20], rmd_output->modes_64x32[col / 8]);
#endif

}

#if USE_SPEED_LEVEL
void sorts(int x, int y, COST_REG * cost_tmp_reg, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], EN_CONTROL * en, U8 speed_level) {
#else
void sorts1(int x, int y, COST_REG * cost_tmp_reg, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], EN_CONTROL * en) {
#endif
	int row = x >> 2;
	int col = y >> 2;
#if USE_SPEED_LEVEL
	/*sort_modes(cost_tmp_reg->data_reg_4x8, ip_reg[0], mode,cost_reg[0],en->en_4x8);
sort_modes(cost_tmp_reg->data_reg_4x16, ip_reg[1], mode,cost_reg[1],en->en_4x16);
sort_modes(cost_tmp_reg->data_reg_4x32, ip_reg[2], mode,cost_reg[2],en->en_4x32);
sort_modes(cost_tmp_reg->data_reg_8x4[col], ip_reg[3], mode,cost_reg[3],en->en_8x4);*/
	sort_modes(cost_tmp_reg->data_reg_8x8[col / 2], ip_reg[4], mode, cost_reg[4], en->en_8x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_8x16[col / 4], ip_reg[5], mode, cost_reg[5], en->en_8x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_8x32, ip_reg[6], mode, cost_reg[6], en->en_8x32, speed_level);
	//sort_modes(cost_tmp_reg->data_reg_16x4[col], ip_reg[8], mode,cost_reg[8],en->en_16x4);
	sort_modes(cost_tmp_reg->data_reg_16x8, ip_reg[9], mode, cost_reg[9], en->en_16x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_16x16[col / 4], ip_reg[10], mode, cost_reg[10], en->en_16x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_16x32, ip_reg[11], mode, cost_reg[11], en->en_16x32, speed_level);
	//sort_modes(cost_tmp_reg->data_reg_32x4[col], ip_reg[13], mode,cost_reg[13],en->en_32x4);
	sort_modes(cost_tmp_reg->data_reg_32x8, ip_reg[14], mode, cost_reg[14], en->en_32x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_32x16, ip_reg[15], mode, cost_reg[15], en->en_32x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_32x32, ip_reg[16], mode, cost_reg[16], en->en_32x32, speed_level);
	sort_modes(cost_tmp_reg->data_reg_64x8[col / 2], ip_reg[18], mode, cost_reg[18], en->en_64x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_64x16, ip_reg[19], mode, cost_reg[19], en->en_64x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_64x32, ip_reg[20], mode, cost_reg[20], en->en_64x32, speed_level);
#else
	/*sort_modes(cost_tmp_reg->data_reg_4x8, ip_reg[0], mode,cost_reg[0],en->en_4x8);
sort_modes(cost_tmp_reg->data_reg_4x16, ip_reg[1], mode,cost_reg[1],en->en_4x16);
sort_modes(cost_tmp_reg->data_reg_4x32, ip_reg[2], mode,cost_reg[2],en->en_4x32);
sort_modes(cost_tmp_reg->data_reg_8x4[col], ip_reg[3], mode,cost_reg[3],en->en_8x4);*/
	sort_modes(cost_tmp_reg->data_reg_8x8[col / 2], ip_reg[4], mode, cost_reg[4], en->en_8x8);
	sort_modes(cost_tmp_reg->data_reg_8x16[col / 4], ip_reg[5], mode, cost_reg[5], en->en_8x16);
	sort_modes(cost_tmp_reg->data_reg_8x32, ip_reg[6], mode, cost_reg[6], en->en_8x32);
	//sort_modes(cost_tmp_reg->data_reg_16x4[col], ip_reg[8], mode,cost_reg[8],en->en_16x4);
	sort_modes(cost_tmp_reg->data_reg_16x8, ip_reg[9], mode, cost_reg[9], en->en_16x8);
	sort_modes(cost_tmp_reg->data_reg_16x16[col / 4], ip_reg[10], mode, cost_reg[10], en->en_16x16);
	sort_modes(cost_tmp_reg->data_reg_16x32, ip_reg[11], mode, cost_reg[11], en->en_16x32);
	//sort_modes(cost_tmp_reg->data_reg_32x4[col], ip_reg[13], mode,cost_reg[13],en->en_32x4);
	sort_modes(cost_tmp_reg->data_reg_32x8, ip_reg[14], mode, cost_reg[14], en->en_32x8);
	sort_modes(cost_tmp_reg->data_reg_32x16, ip_reg[15], mode, cost_reg[15], en->en_32x16);
	sort_modes(cost_tmp_reg->data_reg_32x32, ip_reg[16], mode, cost_reg[16], en->en_32x32);
	sort_modes(cost_tmp_reg->data_reg_64x8[col / 2], ip_reg[18], mode, cost_reg[18], en->en_64x8);
	sort_modes(cost_tmp_reg->data_reg_64x16, ip_reg[19], mode, cost_reg[19], en->en_64x16);
	sort_modes(cost_tmp_reg->data_reg_64x32, ip_reg[20], mode, cost_reg[20], en->en_64x32);
#endif

}

#if USE_SPEED_LEVEL
void sorts(int x, int y, COST_REG * cost_tmp_reg, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], EN_CONTROL * en, U8 speed_level) {
#else
void sorts2(int x, int y, COST_REG * cost_tmp_reg, u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], int mode, u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE], EN_CONTROL * en) {
#endif
	int row = x >> 2;
	int col = y >> 2;
#if USE_SPEED_LEVEL
	/*sort_modes(cost_tmp_reg->data_reg_4x8, ip_reg[0], mode,cost_reg[0],en->en_4x8);
sort_modes(cost_tmp_reg->data_reg_4x16, ip_reg[1], mode,cost_reg[1],en->en_4x16);
sort_modes(cost_tmp_reg->data_reg_4x32, ip_reg[2], mode,cost_reg[2],en->en_4x32);
sort_modes(cost_tmp_reg->data_reg_8x4[col], ip_reg[3], mode,cost_reg[3],en->en_8x4);*/
	sort_modes(cost_tmp_reg->data_reg_8x8[col / 2], ip_reg[4], mode, cost_reg[4], en->en_8x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_8x16[col / 4], ip_reg[5], mode, cost_reg[5], en->en_8x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_8x32, ip_reg[6], mode, cost_reg[6], en->en_8x32, speed_level);
	//sort_modes(cost_tmp_reg->data_reg_16x4[col], ip_reg[8], mode,cost_reg[8],en->en_16x4);
	sort_modes(cost_tmp_reg->data_reg_16x8, ip_reg[9], mode, cost_reg[9], en->en_16x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_16x16[col / 4], ip_reg[10], mode, cost_reg[10], en->en_16x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_16x32, ip_reg[11], mode, cost_reg[11], en->en_16x32, speed_level);
	//sort_modes(cost_tmp_reg->data_reg_32x4[col], ip_reg[13], mode,cost_reg[13],en->en_32x4);
	sort_modes(cost_tmp_reg->data_reg_32x8, ip_reg[14], mode, cost_reg[14], en->en_32x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_32x16, ip_reg[15], mode, cost_reg[15], en->en_32x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_32x32, ip_reg[16], mode, cost_reg[16], en->en_32x32, speed_level);
	sort_modes(cost_tmp_reg->data_reg_64x8[col / 2], ip_reg[18], mode, cost_reg[18], en->en_64x8, speed_level);
	sort_modes(cost_tmp_reg->data_reg_64x16, ip_reg[19], mode, cost_reg[19], en->en_64x16, speed_level);
	sort_modes(cost_tmp_reg->data_reg_64x32, ip_reg[20], mode, cost_reg[20], en->en_64x32, speed_level);
#else
	/*sort_modes(cost_tmp_reg->data_reg_4x8, ip_reg[0], mode,cost_reg[0],en->en_4x8);
sort_modes(cost_tmp_reg->data_reg_4x16, ip_reg[1], mode,cost_reg[1],en->en_4x16);
sort_modes(cost_tmp_reg->data_reg_4x32, ip_reg[2], mode,cost_reg[2],en->en_4x32);
sort_modes(cost_tmp_reg->data_reg_8x4[col], ip_reg[3], mode,cost_reg[3],en->en_8x4);*/
	sort_modes(cost_tmp_reg->data_reg_8x8[col / 2], ip_reg[4], mode, cost_reg[4], en->en_8x8);
	sort_modes(cost_tmp_reg->data_reg_8x16[col / 4], ip_reg[5], mode, cost_reg[5], en->en_8x16);
	sort_modes(cost_tmp_reg->data_reg_8x32, ip_reg[6], mode, cost_reg[6], en->en_8x32);
	//sort_modes(cost_tmp_reg->data_reg_16x4[col], ip_reg[8], mode,cost_reg[8],en->en_16x4);
	sort_modes(cost_tmp_reg->data_reg_16x8, ip_reg[9], mode, cost_reg[9], en->en_16x8);
	sort_modes(cost_tmp_reg->data_reg_16x16[col / 4], ip_reg[10], mode, cost_reg[10], en->en_16x16);
	sort_modes(cost_tmp_reg->data_reg_16x32, ip_reg[11], mode, cost_reg[11], en->en_16x32);
	//sort_modes(cost_tmp_reg->data_reg_32x4[col], ip_reg[13], mode,cost_reg[13],en->en_32x4);
	sort_modes(cost_tmp_reg->data_reg_32x8, ip_reg[14], mode, cost_reg[14], en->en_32x8);
	sort_modes(cost_tmp_reg->data_reg_32x16, ip_reg[15], mode, cost_reg[15], en->en_32x16);
	sort_modes(cost_tmp_reg->data_reg_32x32, ip_reg[16], mode, cost_reg[16], en->en_32x32);
	sort_modes(cost_tmp_reg->data_reg_64x8[col / 2], ip_reg[18], mode, cost_reg[18], en->en_64x8);
	sort_modes(cost_tmp_reg->data_reg_64x16, ip_reg[19], mode, cost_reg[19], en->en_64x16);
	sort_modes(cost_tmp_reg->data_reg_64x32, ip_reg[20], mode, cost_reg[20], en->en_64x32);
#endif

}



#if USE_EIPM_RMD
#if EIPM
void ipred_ang_eipm_rmd(pel * src_le, pel * src_up, pel * dst, int w, int h, int ipm
#if MIPF1
	, int is_luma, int mipf_enable_flag
#endif
)
{
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
#if MIPF1
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

#if MIPF1
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

	const pel* src_ch = NULL;
	const s16* filter;

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
		src_ch = src_up;
		pos_max = w * 2 - 1;

		for (j = 0; j < h; j++)
		{
			int dx;
			GET_REF_POS(mt[0], j + 1, dx, offset);
#if MIPF1
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

#if MIPF1
				/*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
					src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
					filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/

				if (filter_idx == 0)
					dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_ch[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_ch[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 1)
					dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_ch[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_ch[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 2)
					dst[i] = (pel)((src_ch[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_ch[p] * tbl_mc_c_coeff_hp[offset][1] +
						src_ch[pn] * tbl_mc_c_coeff_hp[offset][2] + src_ch[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 3)
					dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_ch[p] * com_tbl_ipred_adi_bilinear[offset][1] +
						src_ch[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

#else
				dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
					src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
					filter_offset) >> filter_bits);
#endif
			}
			dst += w;
		}
	}
#if EIPM
	else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
#else
	else if (ipm > IPD_HOR)
#endif
	{
		src_ch = src_le;
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

#if  MIPF1
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
				//filter = (tbl_filt_list[filter_idx] + offset_y[i])[0];
#else
				filter = (tbl_filt + offset_y[i])[0];
#endif

				pn_n1 = COM_MIN(pn_n1, pos_max);
				p = COM_MIN(p, pos_max);
				pn = COM_MIN(pn, pos_max);
				pn_p2 = COM_MIN(pn_p2, pos_max);

#if MIPF1

				/*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
					src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
					filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/

				int offset_y_new = offset_y[i];
				if (filter_idx == 0)
					dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset_y_new][0] + src_ch[p] * com_tbl_ipred_adi_extra_smooth[offset_y_new][1] +
						src_ch[pn] * com_tbl_ipred_adi_extra_smooth[offset_y_new][2] + src_ch[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset_y_new][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 1)
					dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset_y_new][0] + src_ch[p] * com_tbl_ipred_adi_semi_sharp[offset_y_new][1] +
						src_ch[pn] * com_tbl_ipred_adi_semi_sharp[offset_y_new][2] + src_ch[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset_y_new][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 2)
					dst[i] = (pel)((src_ch[pn_n1] * tbl_mc_c_coeff_hp[offset_y_new][0] + src_ch[p] * tbl_mc_c_coeff_hp[offset_y_new][1] +
						src_ch[pn] * tbl_mc_c_coeff_hp[offset_y_new][2] + src_ch[pn_p2] * tbl_mc_c_coeff_hp[offset_y_new][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 3)
					dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_bilinear[offset_y_new][0] + src_ch[p] * com_tbl_ipred_adi_bilinear[offset_y_new][1] +
						src_ch[pn] * com_tbl_ipred_adi_bilinear[offset_y_new][2] + src_ch[pn_p2] * com_tbl_ipred_adi_bilinear[offset_y_new][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

#else
				dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
					src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
					filter_offset) >> filter_bits);
#endif
			}
			dst += w;
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
#if MIPF1
#if EIPM
		if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT))
#else
		if (ipm < IPD_DIA_R)
#endif
		{
#endif
			for (j = 0; j < h; j++)
			{
#if  MIPF1
				filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
#endif
				for (i = 0; i < w; i++)
				{
					int x = i + t_dx[j];
					int y = j + t_dy[i];

					if (y <= -1)
					{
						src_ch = src_up;
						offset = offset_x[j];
						pos_max = w * 2 - 1;

						pn_n1 = x + 1;
						p = x;
						pn = x - 1;
						pn_p2 = x - 2;
					}
					else
					{
						src_ch = src_le;
						offset = offset_y[i];
						pos_max = h * 2 - 1;

						pn_n1 = y + 1;
						p = y;
						pn = y - 1;
						pn_p2 = y - 2;
					}

#if  MIPF1
					//filter = (tbl_filt_list[filter_idx] + offset)[0];
#else
					filter = (tbl_filt + offset)[0];
#endif

					pn_n1 = COM_MIN(pn_n1, pos_max);
					p = COM_MIN(p, pos_max);
					pn = COM_MIN(pn, pos_max);
					pn_p2 = COM_MIN(pn_p2, pos_max);

#if MIPF1

					/*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
						src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/

					if (filter_idx == 0)
						dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_ch[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
							src_ch[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 1)
						dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_ch[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
							src_ch[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 2)
						dst[i] = (pel)((src_ch[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_ch[p] * tbl_mc_c_coeff_hp[offset][1] +
							src_ch[pn] * tbl_mc_c_coeff_hp[offset][2] + src_ch[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 3)
						dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_ch[p] * com_tbl_ipred_adi_bilinear[offset][1] +
							src_ch[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

#else
					dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
						src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
						filter_offset) >> filter_bits);
#endif
				}
				dst += w;
			}
#if MIPF1
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
						src_ch = src_up;
						offset = offset_x[j];
						pos_max = w * 2 - 1;

						pn_n1 = x + 1;
						p = x;
						pn = x - 1;
						pn_p2 = x - 2;
					}
					else
					{
						src_ch = src_le;
						offset = offset_y[i];
						pos_max = h * 2 - 1;

						pn_n1 = y + 1;
						p = y;
						pn = y - 1;
						pn_p2 = y - 2;
					}

#if  MIPF1
					filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

					//filter = (tbl_filt_list[filter_idx] + offset)[0];

#else
					filter = (tbl_filt + offset)[0];
#endif

					pn_n1 = COM_MIN(pn_n1, pos_max);
					p = COM_MIN(p, pos_max);
					pn = COM_MIN(pn, pos_max);
					pn_p2 = COM_MIN(pn_p2, pos_max);
#if MIPF1

					/*dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
						src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);*/

					if (filter_idx == 0)
						dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_ch[p] * com_tbl_ipred_adi_extra_smooth[offset][1] +
							src_ch[pn] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_extra_smooth[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 1)
						dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_ch[p] * com_tbl_ipred_adi_semi_sharp[offset][1] +
							src_ch[pn] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_semi_sharp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 2)
						dst[i] = (pel)((src_ch[pn_n1] * tbl_mc_c_coeff_hp[offset][0] + src_ch[p] * tbl_mc_c_coeff_hp[offset][1] +
							src_ch[pn] * tbl_mc_c_coeff_hp[offset][2] + src_ch[pn_p2] * tbl_mc_c_coeff_hp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 3)
						dst[i] = (pel)((src_ch[pn_n1] * com_tbl_ipred_adi_bilinear[offset][0] + src_ch[p] * com_tbl_ipred_adi_bilinear[offset][1] +
							src_ch[pn] * com_tbl_ipred_adi_bilinear[offset][2] + src_ch[pn_p2] * com_tbl_ipred_adi_bilinear[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

#else
					dst[i] = (pel)((src_ch[pn_n1] * filter[0] + src_ch[p] * filter[1] +
						src_ch[pn] * filter[2] + src_ch[pn_p2] * filter[3] +
						filter_offset) >> filter_bits);
#endif
				}
				dst += w;
			}
		}
#endif
	}
}
#endif
#endif

#if Debug_RMD_MODE
extern FILE* rmd_mode;
#endif

void clip_pred_rmd(pel * dst, const int w, const int h, int bit_depth)
{
	com_assert(NULL != dst);
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			dst[i * w + j] = COM_CLIP3(0, ((1 << bit_depth) - 1), dst[i * w + j]);
		}
	}
}

#if USE_SPEED_LEVEL
void calc_part1(int x0, int y0, int mode, U12 pix_x, U12 pix_y, pel cu_org[8][16][4 * 4], pel up_org[8][64], pel left_org[32][16], pel up_ref[68], const RMD_BUF * rmd_buf,
	RMD_OUT_TMP * rmd_output, RMD_OUT_TMP * rmd_out_tmp, COST_MEM * cost_mem, COST_MEM * cost_mem1, u32 * cost_8x32, u32 lambda, U8 speed_level) {
#else
void calc_part1(int x0, int y0, int mode, U12 pix_x, U12 pix_y, pel cu_org[8][16][4 * 4], pel up_org[8][64], pel left_org[32][16], pel up_ref[68], const RMD_BUF * rmd_buf,
	RMD_OUT_TMP * rmd_output, RMD_OUT_TMP * rmd_out_tmp, COST_MEM * cost_mem, COST_MEM * cost_mem1, u32 * cost_8x32, u32 lambda) {
#endif

#pragma HLS inline
	static COST_REG cost_tmp_reg;
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_32x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_64x8 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x8 complete
	int R;
	int x = pix_x + x0;

	EN_CONTROL en_all;
	pel pred_cache[4 * 4];
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
	u32 cost_satd;
	pel src_le[4 * 2 + 3];
	pel src_up[4 * 2 + 3];
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
	pel org[4 * 4];
	PIX_ORG pix_org;
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->leftdown_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->left_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->upright_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->up_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=org complete dim=1
	u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=1
	u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=1

	const int bit_depth = 8;
	int y = pix_y + y0;
	u16 avail_cu = get_avail_intra(x, y);

	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		org[i] = cu_org[(y0 >> 2)][x0 >> 2][i];
	}

	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		if (y0 != 0)
			pix_org.up_org[i] = up_org[(y0 >> 2) - 1][x0 + i];
		if (y0 != 0 && x0 != MAX_CU_SIZE2 - 4)
			pix_org.upright_org[i] = up_org[(y0 >> 2) - 1][x0 + 4 + i];
		if (x0 != 0)
			pix_org.left_org[i] = left_org[y0 + i][((x0 >> 2) - 1)];
		//if (x0 != 0 && y0 != MAX_CU_SIZE2 - 4)
		//	pix_org.leftdown_org[i] = left_org[y0 + 4 + i][(x0 >> 2) - 1];
	}
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		if (x0 != 0 && y0 < MAX_CU_SIZE2 / 2 - 4)
			pix_org.leftdown_org[i] = left_org[y0 + 4 + i][(x0 >> 2) - 1];
		if (x0 != 0 && y0 >= MAX_CU_SIZE2 / 2 - 4)
			pix_org.leftdown_org[i] = pix_org.left_org[3];
	}

	get_rmd_pixel_4x4(avail_cu, bit_depth, src_le, src_up,
		x, y, rmd_buf->left, rmd_buf->top_left, up_ref, &pix_org, (y0 != 0 && x0 != 0) ? (cu_org[(y0 >> 2) - 1][(x0 >> 2) - 1][15]) : (1 << (bit_depth - 1)));
	get_enable(x0, y0, &en_all);
	if (mode == 0) {
		rmd_ipred_dc_4x4(src_le + 3, src_up + 3, pred_cache, bit_depth, avail_cu);
	}
	else if (mode == 1) {
		rmd_ipred_plane_4x4(src_le + 2, src_up + 2, pred_cache, bit_depth);
	}
	else if (mode == 2) {
		rmd_ipred_bi_4x4(src_le + 3, src_up + 3, pred_cache, bit_depth);
	}
	else if (mode == 12) {
		rmd_ipred_vert_4x4(src_up + 3, pred_cache);
	}
	else if (mode == 24) {
		rmd_ipred_hor_4x4(src_le + 3, pred_cache);
	}
	else if ((mode >= 3 && mode < 12)) {
		rmd_ipred_ang4x4(src_le, src_up, pred_cache, mode);
	}
	else if ((mode >= 13 && mode < 24)) {
		rmd_ipred1_ang4x4(src_le, src_up, pred_cache, mode);
	}
	else if ((mode >= 25 && mode < IPD_CNT_PREV)) {
		rmd_ipred2_ang4x4(src_le, src_up, pred_cache, mode);
	}
	//#if USE_EIPM_RMD
	//	else
	//	{
	//		ipred_ang_eipm_rmd(src_le+3, src_up+3, pred_cache, 4, 4, mode/*, 1, 1*/);
	//		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	//	}
	//#endif

	cost_satd = rmd_satd_4x4(org, pred_cache);
#if Debug_RMD_MODE
	fprintf(rmd_mode, "cal1:mode=%d cost_satd=%d x=%d y=%d x0=%d y0=%d\n", mode, cost_satd, x, y, x0, y0);
#endif
	//if (mode < 2) {
	//    R = 1;
	//}
	//else if (mode < 4) {
	//    R = 2;
	//}
	//else if (mode < 8) {
	//    R = 3;
	//}
	//else if (mode < 16) {
	//    R = 4;
	//}
	//else if (mode < 32) {
	//    R = 5;
	//}
	//else {
	//    R = 6;
	//}
	if (mode == 2 || mode == 0)
		R = 2;
	else
		R = 6;
	cost_satd += (lambda * R) >> 10;
	cal_cost(x0, y0, cost_satd, &cost_tmp_reg, cost_8x32);
#if USE_SPEED_LEVEL
	load_regs(x0, y0, cost_mem, cost_reg, rmd_output, ip_reg, speed_level);
	sorts(x0, y0, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all, speed_level);
	stor_regs(x0, y0, cost_mem1, cost_reg, rmd_out_tmp, ip_reg, speed_level);
#else
	load_regs(x0, y0, cost_mem, cost_reg, rmd_output, ip_reg);
	sorts1(x0, y0, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all);
	stor_regs(x0, y0, cost_mem1, cost_reg, rmd_out_tmp, ip_reg);
#endif


}

void calc_part1_2(int x0, int y0, int mode, U12 pix_x, U12 pix_y, pel cu_org[8][16][4 * 4], pel up_org[8][64], pel left_org[32][16], pel up_ref[68], const RMD_BUF * rmd_buf,
	RMD_OUT_TMP * rmd_output, RMD_OUT_TMP * rmd_out_tmp, COST_MEM * cost_mem, COST_MEM * cost_mem1, u32 * cost_8x32, u32 lambda) {


#pragma HLS inline
	static COST_REG cost_tmp_reg;
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_32x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_64x8 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x8 complete
	int R;
	int x = pix_x + x0;

	EN_CONTROL en_all;
	pel pred_cache[4 * 4];
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
	u32 cost_satd;
	pel src_le[4 * 2 + 3];
	pel src_up[4 * 2 + 3];
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
	pel org[4 * 4];
	PIX_ORG pix_org;
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->leftdown_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->left_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->upright_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->up_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=org complete dim=1
	u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=1
	u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=1

	const int bit_depth = 8;
	int y = pix_y + y0;
	u16 avail_cu = get_avail_intra(x, y);

	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		org[i] = cu_org[(y0 >> 2)][x0 >> 2][i];
	}

	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		if (y0 != 0)
			pix_org.up_org[i] = up_org[(y0 >> 2) - 1][x0 + i];
		if (y0 != 0 && x0 != MAX_CU_SIZE2 - 4)
			pix_org.upright_org[i] = up_org[(y0 >> 2) - 1][x0 + 4 + i];
		if (x0 != 0)
			pix_org.left_org[i] = left_org[y0 + i][((x0 >> 2) - 1)];
		//if (x0 != 0 && y0 != MAX_CU_SIZE2 - 4)
		//	pix_org.leftdown_org[i] = left_org[y0 + 4 + i][(x0 >> 2) - 1];
	}
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		if (x0 != 0 && y0 < MAX_CU_SIZE2 / 2 - 4)
			pix_org.leftdown_org[i] = left_org[y0 + 4 + i][(x0 >> 2) - 1];
		if (x0 != 0 && y0 >= MAX_CU_SIZE2 / 2 - 4)
			pix_org.leftdown_org[i] = pix_org.left_org[3];
	}

	get_rmd_pixel_4x4(avail_cu, bit_depth, src_le, src_up,
		x, y, rmd_buf->left, rmd_buf->top_left, up_ref, &pix_org, (y0 != 0 && x0 != 0) ? (cu_org[(y0 >> 2) - 1][(x0 >> 2) - 1][15]) : (1 << (bit_depth - 1)));
	get_enable(x0, y0, &en_all);

	if (mode >= 34 && mode <= 43) {
		rmd_ipred_ang4x4(src_le, src_up, pred_cache, mode);
		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	}
	else if ((mode >= 44 && mode <= 57)) {
		rmd_ipred1_ang4x4_phase2(src_le, src_up, pred_cache, mode);
		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	}
	else if ((mode >= 58 && mode < 66)) {
		rmd_ipred2_ang4x4(src_le, src_up, pred_cache, mode);
		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	}

	cost_satd = rmd_satd_4x4(org, pred_cache);
#if Debug_RMD_MODE
	fprintf(rmd_mode, "cal1_2:mode=%d cost_satd=%d x=%d y=%d x0=%d y0=%d\n", mode, cost_satd, x, y, x0, y0);
#endif

	if (mode == 2 || mode == 0)
		R = 2;
	else
		R = 6;
	cost_satd += (lambda * R) >> 10;
	cal_cost(x0, y0, cost_satd, &cost_tmp_reg, cost_8x32);
#if USE_SPEED_LEVEL
	load_regs(x0, y0, cost_mem, cost_reg, rmd_output, ip_reg, speed_level);
	sorts(x0, y0, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all, speed_level);
	stor_regs(x0, y0, cost_mem1, cost_reg, rmd_out_tmp, ip_reg, speed_level);
#else
	load_regs(x0, y0, cost_mem, cost_reg, rmd_output, ip_reg);
	sorts1(x0, y0, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all);
	stor_regs(x0, y0, cost_mem1, cost_reg, rmd_out_tmp, ip_reg);
#endif


}


#if USE_SPEED_LEVEL
void calc_part2(int x0, int y0, int mode, U12 pix_x, U12 pix_y, pel cu_org[8][16][4 * 4], pel up_org[8][64], pel left_org[32][16], pel up_ref[68], const RMD_BUF * rmd_buf,
	RMD_OUT_TMP * rmd_output, RMD_OUT_TMP * rmd_out_tmp, COST_MEM * cost_mem, COST_MEM * cost_mem1, u32 * cost_8x32, u32 lambda, U8 speed_level) {
#else
void calc_part2(int x0, int y0, int mode, U12 pix_x, U12 pix_y, pel cu_org[8][16][4 * 4], pel up_org[8][64], pel left_org[32][16], pel up_ref[68], const RMD_BUF * rmd_buf,
	RMD_OUT_TMP * rmd_output, RMD_OUT_TMP * rmd_out_tmp, COST_MEM * cost_mem, COST_MEM * cost_mem1, u32 * cost_8x32, u32 lambda, pel up_org_lastline[64]) {
#endif
#pragma HLS inline
	static COST_REG cost_tmp_reg;
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_32x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_64x8 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x8 complete
	int x = x0 + pix_x;
	EN_CONTROL en_all;
	pel pred_cache[4 * 4];
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
	u32 cost_satd;
	pel src_le[4 * 2 + 3];
	pel src_up[4 * 2 + 3];
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
	pel org[4 * 4];
	PIX_ORG pix_org;
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->leftdown_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->left_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->upright_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->up_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=org complete dim=1

	u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=1
	u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=1

	int R;
	const int bit_depth = 8;
	int y = pix_y + y0;
	u16 avail_cu = get_avail_intra(x, y);

	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		org[i] = cu_org[((y0 - 32) >> 2)][x0 >> 2][i];
	}

	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		if (y0 != 32) {
			pix_org.up_org[i] = up_org[((y0 - 32) >> 2) - 1][x0 + i];
		}
		if (y0 == 32)
		{
			pix_org.up_org[i] = up_org_lastline[x0 + i];
		}
		if (y0 != 32 && x0 != MAX_CU_SIZE2 - 4) {
			pix_org.upright_org[i] = up_org[((y0 - 32) >> 2) - 1][x0 + 4 + i];
		}
		if (y0 == 32 && x0 != MAX_CU_SIZE2 - 4)
		{
			pix_org.upright_org[i] = up_org_lastline[x0 + 4 + i];
		}
		if (x0 != 0)
			pix_org.left_org[i] = left_org[(y0 - 32) + i][((x0 >> 2) - 1)];
		if (x0 != 0 && y0 != MAX_CU_SIZE2 - 4)
			pix_org.leftdown_org[i] = left_org[(y0 - 32) + 4 + i][(x0 >> 2) - 1];
	}

	get_rmd_pixel_4x4(avail_cu, bit_depth, src_le, src_up,
		x, y, rmd_buf->left, rmd_buf->top_left, up_ref, &pix_org, (y0 != 32 && x0 != 0) ? (cu_org[((y0 - 32) >> 2) - 1][(x0 >> 2) - 1][15]) : up_org_lastline[x0 - 1]);
	get_enable(x0, y0 - 32, &en_all);
	if (mode == 0) {
		rmd_ipred_dc_4x4(src_le + 3, src_up + 3, pred_cache, bit_depth, avail_cu);
	}
	else if (mode == 1) {
		rmd_ipred_plane_4x4(src_le + 2, src_up + 2, pred_cache, bit_depth);
	}
	else if (mode == 2) {
		rmd_ipred_bi_4x4(src_le + 3, src_up + 3, pred_cache, bit_depth);
	}
	else if (mode == 12) {
		rmd_ipred_vert_4x4(src_up + 3, pred_cache);
	}
	else if (mode == 24) {
		rmd_ipred_hor_4x4(src_le + 3, pred_cache);
	}
	else if ((mode >= 3 && mode < 12)) {
		rmd_ipred_ang4x4(src_le, src_up, pred_cache, mode);
	}
	else if ((mode >= 13 && mode < 24)) {
		rmd_ipred1_ang4x4(src_le, src_up, pred_cache, mode);
	}
	else if ((mode >= 25 && mode < IPD_CNT_PREV)) {
		rmd_ipred2_ang4x4(src_le, src_up, pred_cache, mode);
	}
	//#if USE_EIPM_RMD
	//	else
	//	{
	//		ipred_ang_eipm_rmd(src_le+3, src_up+3, pred_cache, 4, 4, mode/*, 1, 1*/);
	//		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	//	}
	//#endif
		//clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	cost_satd = rmd_satd_4x4(org, pred_cache);
#if Debug_RMD_MODE
	fprintf(rmd_mode, "cal2:mode=%d cost_satd=%d\n", mode, cost_satd);
#endif
	//if (mode < 2) {
	//    R = 1;
	//}
	//else if (mode < 4) {
	//    R = 2;
	//}
	//else if (mode < 8) {
	//    R = 3;
	//}
	//else if (mode < 16) {
	//    R = 4;
	//}
	//else if (mode < 32) {
	//    R = 5;
	//}
	//else {
	//    R = 6;
	//}
	if (mode == 2 || mode == 0)
		R = 2;
	else
		R = 6;
	cost_satd += (lambda * R) >> 10;
	cal_cost(x0, y0 - 32, cost_satd, &cost_tmp_reg, cost_8x32);
#if USE_SPEED_LEVEL
	load_regs(x0, y0 - 32, cost_mem, cost_reg, rmd_output, ip_reg, speed_level);
	sorts(x0, y0 - 32, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all, speed_level);
	stor_regs(x0, y0 - 32, cost_mem1, cost_reg, rmd_out_tmp, ip_reg, speed_level);
#else
	load_regs(x0, y0 - 32, cost_mem, cost_reg, rmd_output, ip_reg);
	sorts2(x0, y0 - 32, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all);
	stor_regs(x0, y0 - 32, cost_mem1, cost_reg, rmd_out_tmp, ip_reg);
#endif
	//R = 6;
}

void calc_part2_2(int x0, int y0, int mode, U12 pix_x, U12 pix_y, pel cu_org[8][16][4 * 4], pel up_org[8][64], pel left_org[32][16], pel up_ref[68], const RMD_BUF * rmd_buf,
	RMD_OUT_TMP * rmd_output, RMD_OUT_TMP * rmd_out_tmp, COST_MEM * cost_mem, COST_MEM * cost_mem1, u32 * cost_8x32, u32 lambda, pel up_org_lastline[64]) {
#pragma HLS inline
	static COST_REG cost_tmp_reg;
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x16 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_16x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_32x4 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_64x8 complete
#pragma HLS ARRAY_PARTITION variable = cost_tmp_reg.data_reg_8x8 complete
	int x = x0 + pix_x;
	EN_CONTROL en_all;
	pel pred_cache[4 * 4];
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
	u32 cost_satd;
	pel src_le[4 * 2 + 3];
	pel src_up[4 * 2 + 3];
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
	pel org[4 * 4];
	PIX_ORG pix_org;
#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->leftdown_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->left_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->upright_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=pix_org->up_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=org complete dim=1

	u32 cost_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_reg complete dim=1
	u8 ip_reg[22][IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_reg complete dim=1

	int R;
	const int bit_depth = 8;
	int y = pix_y + y0;
	u16 avail_cu = get_avail_intra(x, y);

	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		org[i] = cu_org[((y0 - 32) >> 2)][x0 >> 2][i];
	}

	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		if (y0 != 32) {
			pix_org.up_org[i] = up_org[((y0 - 32) >> 2) - 1][x0 + i];
		}
		if (y0 == 32)
		{
			pix_org.up_org[i] = up_org_lastline[x0 + i];
		}
		if (y0 != 32 && x0 != MAX_CU_SIZE2 - 4) {
			pix_org.upright_org[i] = up_org[((y0 - 32) >> 2) - 1][x0 + 4 + i];
		}
		if (y0 == 32 && x0 != MAX_CU_SIZE2 - 4)
		{
			pix_org.upright_org[i] = up_org_lastline[x0 + 4 + i];
		}
		if (x0 != 0)
			pix_org.left_org[i] = left_org[(y0 - 32) + i][((x0 >> 2) - 1)];
		if (x0 != 0 && y0 != MAX_CU_SIZE2 - 4)
			pix_org.leftdown_org[i] = left_org[(y0 - 32) + 4 + i][(x0 >> 2) - 1];
	}

	get_rmd_pixel_4x4(avail_cu, bit_depth, src_le, src_up,
		x, y, rmd_buf->left, rmd_buf->top_left, up_ref, &pix_org, (y0 != 32 && x0 != 0) ? (cu_org[((y0 - 32) >> 2) - 1][(x0 >> 2) - 1][15]) : up_org_lastline[x0 - 1]);
	get_enable(x0, y0 - 32, &en_all);

	if (mode >= 34 && mode <= 43) {
		rmd_ipred_ang4x4(src_le, src_up, pred_cache, mode);
		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	}
	else if ((mode >= 44 && mode <= 57)) {
		rmd_ipred1_ang4x4_phase2(src_le, src_up, pred_cache, mode);
		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	}
	else if ((mode >= 58 && mode < 66)) {
		rmd_ipred2_ang4x4(src_le, src_up, pred_cache, mode);
		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	}
	//#if USE_EIPM_RMD
	//	else
	//	{
	//		ipred_ang_eipm_rmd(src_le+3, src_up+3, pred_cache, 4, 4, mode/*, 1, 1*/);
	//		clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	//	}
	//#endif
		//clip_pred_rmd(pred_cache, 4, 4, bit_depth);
	cost_satd = rmd_satd_4x4(org, pred_cache);
#if Debug_RMD_MODE
	fprintf(rmd_mode, "cal2_2:mode=%d cost_satd=%d\n", mode, cost_satd);
#endif
	//if (mode < 2) {
	//    R = 1;
	//}
	//else if (mode < 4) {
	//    R = 2;
	//}
	//else if (mode < 8) {
	//    R = 3;
	//}
	//else if (mode < 16) {
	//    R = 4;
	//}
	//else if (mode < 32) {
	//    R = 5;
	//}
	//else {
	//    R = 6;
	//}
	if (mode == 2 || mode == 0)
		R = 2;
	else
		R = 6;
	cost_satd += (lambda * R) >> 10;
	cal_cost(x0, y0 - 32, cost_satd, &cost_tmp_reg, cost_8x32);
#if USE_SPEED_LEVEL
	load_regs(x0, y0 - 32, cost_mem, cost_reg, rmd_output, ip_reg, speed_level);
	sorts(x0, y0 - 32, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all, speed_level);
	stor_regs(x0, y0 - 32, cost_mem1, cost_reg, rmd_out_tmp, ip_reg, speed_level);
#else
	load_regs(x0, y0 - 32, cost_mem, cost_reg, rmd_output, ip_reg);
	sorts2(x0, y0 - 32, &cost_tmp_reg, cost_reg, mode, ip_reg, &en_all);
	stor_regs(x0, y0 - 32, cost_mem1, cost_reg, rmd_out_tmp, ip_reg);
#endif
}

void sort_final(u32 cost[5], u8 mode[5], u32 cost2[5], u8 mode2[5], u8 mode_out[5])
{
	u32 cost_all[10];
	u8 mode_all[10];



	for (int i = 0; i < 5; i++)
	{
#pragma HLS UNROLL
		cost_all[i] = cost[i];
		cost_all[i + 5] = cost2[i];
		mode_all[i] = mode[i];
		mode_all[i + 5] = mode2[i];
	}

	u32 cost_min[5];
	u8  mode_min[5];
#pragma HLS ARRAY_PARTITION variable=cost_min complete
#pragma HLS ARRAY_PARTITION variable=mode_min complete
	for (int i =0; i < 5; i++){
		#pragma HLS unroll
		cost_min[i] = 0xFFFFFFFF;
		//mode_min[i] = 255;
	}
	for (int i = 0; i < 5; i++){
		#pragma HLS unroll
		mode_min[i] = 0xFF;
	}
	for(int i = 0; i < 10; i++){
		#pragma HLS pipeline
		if(cost_min[0] > cost_all[i]){
			cost_min[4] = cost_min[3];
			cost_min[3] = cost_min[2];
			cost_min[2] = cost_min[1];
			cost_min[1] = cost_min[0];
			cost_min[0] = cost_all[i];
			mode_min[4] = mode_min[3];
			mode_min[3] = mode_min[2];
			mode_min[2] = mode_min[1];
			mode_min[1] = mode_min[0];
			mode_min[0] = mode_all[i];
		} else if (cost_min[1] > cost_all[i]){
			cost_min[4] = cost_min[3];
			cost_min[3] = cost_min[2];
			cost_min[2] = cost_min[1];
			cost_min[1] = cost_all[i];
			mode_min[4] = mode_min[3];
			mode_min[3] = mode_min[2];
			mode_min[2] = mode_min[1];
			mode_min[1] = mode_all[i];
		} else if (cost_min[2] > cost_all[i]){
			cost_min[4] = cost_min[3];
			cost_min[3] = cost_min[2];
			cost_min[2] = cost_all[i];
			mode_min[4] = mode_min[3];
			mode_min[3] = mode_min[2];
			mode_min[2] = mode_all[i];
		} else if (cost_min[3] > cost_all[i]){
			cost_min[4] = cost_min[3];
			cost_min[3] = cost_all[i];
			mode_min[4] = mode_min[3];
			mode_min[3] = mode_all[i];
		} else if (cost_min[4] > cost_all[i]){
			cost_min[4] = cost_all[i];
			mode_min[4] = mode_all[i];
		}
	}

	for (int i = 0; i < 5; i++)
	{
#pragma HLS UNROLL
		mode_out[i] = mode_min[i];
		//cost[i] = cost_all[i];
	}
 


}

void mode_part1(U12 pix_x, U12 pix_y, pel cu_org[2][8][16][4 * 4], pel up_org[2][8][64], pel left_org[2][32][16], pel up_ref[68], const RMD_BUF* rmd_buf,
	RMD_OUT_TMP* rmd_out_tmp00, RMD_OUT_TMP* rmd_out_tmp01, RMD_OUT_TMP* rmd_out_tmp10, RMD_OUT_TMP* rmd_out_tmp11,
	COST_MEM* cost_mem00, COST_MEM* cost_mem01, COST_MEM* cost_mem10, COST_MEM* cost_mem11,
	u32 cost_8x32[2][66][8], u32 lambda)
{
	for (u8 mode = 0; mode < 33; mode++) {

		if (mode % 2 == 1) {
			for (int x0 = 0; x0 < MAX_CU_SIZE2; x0 += 4) {
				for (int y0 = 0; y0 < MAX_CU_SIZE2 / 2; y0 += 4) {
#pragma HLS PIPELINE
					calc_part1(x0, y0, mode, pix_x, pix_y, cu_org[0], up_org[0], left_org[0], up_ref, rmd_buf,
						rmd_out_tmp00, rmd_out_tmp10, cost_mem00, cost_mem10, &cost_8x32[0][mode][x0 >> 3], lambda);
					calc_part2(x0, y0 + 32, mode, pix_x, pix_y, cu_org[1], up_org[1], left_org[1], up_ref, rmd_buf,
						rmd_out_tmp01, rmd_out_tmp11, cost_mem01, cost_mem11, &cost_8x32[1][mode][x0 >> 3], lambda,up_org[0][7]);
				}
			}
		}
		else {
			for (int x0 = 0; x0 < MAX_CU_SIZE2; x0 += 4) {
				for (int y0 = 0; y0 < MAX_CU_SIZE2 / 2; y0 += 4) {
#pragma HLS PIPELINE
					calc_part1(x0, y0, mode, pix_x, pix_y, cu_org[0], up_org[0], left_org[0], up_ref, rmd_buf,
						rmd_out_tmp10, rmd_out_tmp00, cost_mem10, cost_mem00, &cost_8x32[0][mode][x0 >> 3], lambda);
					calc_part2(x0, y0 + 32, mode, pix_x, pix_y, cu_org[1], up_org[1], left_org[1], up_ref, rmd_buf,
						rmd_out_tmp11, rmd_out_tmp01, cost_mem11, cost_mem01, &cost_8x32[1][mode][x0 >> 3], lambda, up_org[0][7]);
				}
			}
		}
	}
}

void mode_part2(U12 pix_x, U12 pix_y, pel cu_org[2][8][16][4 * 4], pel up_org[2][8][64], pel left_org[2][32][16], pel up_ref[68], const RMD_BUF* rmd_buf,
	RMD_OUT_TMP* rmd_out_tmp00, RMD_OUT_TMP* rmd_out_tmp01, RMD_OUT_TMP* rmd_out_tmp10, RMD_OUT_TMP* rmd_out_tmp11,
	COST_MEM* cost_mem00, COST_MEM* cost_mem01, COST_MEM* cost_mem10, COST_MEM* cost_mem11,
	u32 cost_8x32[2][66][8], u32 lambda)
{
	for (u8 mode = 0; mode < 32; mode++) {

		if ((mode + 34) % 2 == 1) {
			for (int x0 = 0; x0 < MAX_CU_SIZE2; x0 += 4) {
				for (int y0 = 0; y0 < MAX_CU_SIZE2 / 2; y0 += 4) {
#pragma HLS PIPELINE
					calc_part1_2(x0, y0, mode + 34, pix_x, pix_y, cu_org[0], up_org[0], left_org[0], up_ref, rmd_buf,
						rmd_out_tmp00, rmd_out_tmp10, cost_mem00, cost_mem10, &cost_8x32[0][mode + 34][x0 >> 3], lambda);
					calc_part2_2(x0, y0 + 32, mode + 34, pix_x, pix_y, cu_org[1], up_org[1], left_org[1], up_ref, rmd_buf,
						rmd_out_tmp01, rmd_out_tmp11, cost_mem01, cost_mem11, &cost_8x32[1][mode + 34][x0 >> 3], lambda, up_org[0][7]);
				}
			}
		}
		else {
			for (int x0 = 0; x0 < MAX_CU_SIZE2; x0 += 4) {
				for (int y0 = 0; y0 < MAX_CU_SIZE2 / 2; y0 += 4) {
#pragma HLS PIPELINE
					calc_part1_2(x0, y0, mode + 34, pix_x, pix_y, cu_org[0], up_org[0], left_org[0], up_ref, rmd_buf,
						rmd_out_tmp10, rmd_out_tmp00, cost_mem10, cost_mem00, &cost_8x32[0][mode + 34][x0 >> 3], lambda);
					calc_part2_2(x0, y0 + 32, mode + 34, pix_x, pix_y, cu_org[1], up_org[1], left_org[1], up_ref, rmd_buf,
						rmd_out_tmp11, rmd_out_tmp01, cost_mem11, cost_mem01, &cost_8x32[1][mode + 34][x0 >> 3], lambda, up_org[0][7]);
				}
			}
		}
	}
}

void copy_org_data(pel fenc_lcu_y[MAX_CU_SIZE2 * MAX_CU_SIZE2],
	pel cu_org[2][8][16][4 * 4], pel up_org[2][8][64], pel left_org[2][32][16],
	pel cu_org_2[2][8][16][4 * 4], pel up_org_2[2][8][64], pel left_org_2[2][32][16])
{
	copy_org(fenc_lcu_y, cu_org, left_org);
	copy_up_org(fenc_lcu_y, up_org);

	copy_org(fenc_lcu_y, cu_org_2, left_org_2);
	copy_up_org(fenc_lcu_y, up_org_2);
}

#if USE_SPEED_LEVEL
void calc_scu_cost(U12 pix_x, U12 pix_y, pel fenc_lcu_y[MAX_CU_SIZE2 * MAX_CU_SIZE2], const RMD_BUF * rmd_buf, RMD_OUTPUT * rmd_output, u32 lambda, U8 speed_level)
#else
void calc_scu_cost(U12 pix_x, U12 pix_y, pel fenc_lcu_y[MAX_CU_SIZE2 * MAX_CU_SIZE2], const RMD_BUF * rmd_buf, const RMD_BUF * rmd_buf2, RMD_OUTPUT * rmd_output, u32 lambda)
#endif
{
	static pel cu_org[2][8][16][4 * 4];
	static pel up_org[2][8][64];
	static pel left_org[2][32][16];

	static pel cu_org_2[2][8][16][4 * 4];
	static pel up_org_2[2][8][64];
	static pel left_org_2[2][32][16];
#if 1
#pragma HLS ARRAY_PARTITION variable=cu_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_org complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_org complete dim=4
#pragma HLS ARRAY_PARTITION variable=up_org complete dim=1
#pragma HLS ARRAY_PARTITION variable=up_org complete dim=3
#pragma HLS ARRAY_PARTITION variable=left_org complete dim=3
#pragma HLS ARRAY_PARTITION variable=left_org cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=left_org complete dim=1

#pragma HLS ARRAY_PARTITION variable=cu_org_2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_org_2 complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_org_2 complete dim=4
#pragma HLS ARRAY_PARTITION variable=up_org_2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=up_org_2 complete dim=3
#pragma HLS ARRAY_PARTITION variable=left_org_2 complete dim=3
#pragma HLS ARRAY_PARTITION variable=left_org_2 cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=left_org_2 complete dim=1

#pragma HLS RESOURCE variable=rmd_output->modes_8x8 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_8x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x8 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_8x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x8 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_8x64 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x8 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x64 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x64 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x64 core=RAM_1P_BRAM

#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_8x64 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_16x64 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_32x64 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_output->modes_64x64 complete dim = 2
#endif
	static RMD_OUT_TMP rmd_out_tmp00;
#if 1
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.modes_64x32 complete dim = 2
#endif
	static RMD_OUT_TMP rmd_out_tmp01;
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.modes_64x32 complete dim = 2
	static RMD_OUT_TMP rmd_out_tmp10;
#if 1
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.modes_64x32 complete dim = 2
#endif
	static RMD_OUT_TMP rmd_out_tmp11;
#if 1
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.modes_64x32 complete dim = 2
#endif


	u32 data_reg_8x64[8];
	u32 data_reg_16x64[4];
	u32 data_reg_32x64[2];
	u32 data_reg_64x64;

#pragma HLS ARRAY_PARTITION variable = data_reg_8x64 complete
#pragma HLS ARRAY_PARTITION variable = data_reg_16x64 complete
#pragma HLS ARRAY_PARTITION variable = data_reg_32x64 complete
#if USE_EIPM_RMD
	static u32 cost_8x32[2][66][8];
	static u32 cost_8x32_2[2][66][8];
#else
	static u32 cost_8x32[2][33][8];
#endif

#if 1
#pragma HLS ARRAY_PARTITION variable = cost_8x32 complete dim=3
#pragma HLS ARRAY_PARTITION variable = cost_8x32 complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_8x32 complete dim=1

#pragma HLS ARRAY_PARTITION variable = cost_8x32_2 complete dim=3
#pragma HLS ARRAY_PARTITION variable = cost_8x32_2 complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_8x32_2 complete dim=1
#endif
	u8 ip_8x64[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 ip_16x64[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 ip_32x64[2][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u8 ip_64x64[IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = ip_8x64 complete dim=1
#pragma HLS ARRAY_PARTITION variable = ip_16x64 complete dim=1
#pragma HLS ARRAY_PARTITION variable = ip_32x64 complete dim=1
#pragma HLS ARRAY_PARTITION variable = ip_8x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_16x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_32x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable = ip_64x64 complete dim=1
	u32 cost_8x64[8][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_16x64[4][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_32x64[2][IPD_RDO_CNT + EXTRA_RMD_MODE];
	u32 cost_64x64[IPD_RDO_CNT + EXTRA_RMD_MODE];
#pragma HLS ARRAY_PARTITION variable = cost_8x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_16x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_32x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable = cost_8x64 complete dim=1
#pragma HLS ARRAY_PARTITION variable = cost_16x64 complete dim=1
#pragma HLS ARRAY_PARTITION variable = cost_32x64 complete dim=1
#pragma HLS ARRAY_PARTITION variable = cost_64x64 complete dim=1

	copy_org_data(fenc_lcu_y, cu_org, up_org, left_org, cu_org_2, up_org_2, left_org_2);

	pel up_ref[68];
	pel up_ref_2[68];
#pragma HLS ARRAY_PARTITION variable=up_ref complete
	split_up_ref(rmd_buf->up, pix_x, up_ref);
	split_up_ref(rmd_buf2->up, pix_x, up_ref_2);

	static COST_MEM cost_mem00;
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00.cost_64x32 complete dim = 2
	static COST_MEM cost_mem01;
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01.cost_64x32 complete dim = 2
	static COST_MEM cost_mem10;
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10.cost_64x32 complete dim = 2
	static COST_MEM cost_mem11;
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11.cost_64x32 complete dim = 2

	static COST_MEM cost_mem00_2, cost_mem10_2, cost_mem11_2, cost_mem01_2;
	static RMD_OUT_TMP rmd_out_tmp00_2, rmd_out_tmp10_2, rmd_out_tmp11_2, rmd_out_tmp01_2;

#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem00_2.cost_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem01_2.cost_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem10_2.cost_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = cost_mem11_2.cost_64x32 complete dim = 2

	//*******************************
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00_2.modes_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01_2.modes_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10_2.modes_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11_2.modes_64x32 complete dim = 2

//************************
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp00.out_modes_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp01.out_modes_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp10.out_modes_64x32 complete dim = 2

#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_8x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_8x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_16x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_8x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_32x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_16x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_64x8 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_16x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_32x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_64x16 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_32x32 complete dim = 2
#pragma HLS ARRAY_PARTITION variable = rmd_out_tmp11.out_modes_64x32 complete dim = 2
//************************
	U12 pix_x2 = pix_x;
	U12 pix_y2 = pix_y;
	u32 lambda2 = lambda;
	mode_part1(pix_x, pix_y, cu_org, up_org, left_org, up_ref, rmd_buf, &rmd_out_tmp00, &rmd_out_tmp01, &rmd_out_tmp10, &rmd_out_tmp11,
		&cost_mem00, &cost_mem01, &cost_mem10, &cost_mem11, cost_8x32, lambda);

	mode_part2(pix_x2, pix_y2, cu_org_2, up_org_2, left_org_2, up_ref_2, rmd_buf2, &rmd_out_tmp00_2, &rmd_out_tmp01_2, &rmd_out_tmp10_2, &rmd_out_tmp11_2,
		&cost_mem00_2, &cost_mem01_2, &cost_mem10_2, &cost_mem11_2, cost_8x32_2, lambda2);

#if 1
	sort_final(cost_mem00.cost_64x32[0], rmd_out_tmp00.modes_64x32[0], cost_mem10_2.cost_64x32[0], rmd_out_tmp10_2.modes_64x32[0], rmd_out_tmp00.out_modes_64x32[0]);
	sort_final(cost_mem01.cost_64x32[0], rmd_out_tmp01.modes_64x32[0], cost_mem11_2.cost_64x32[0], rmd_out_tmp11_2.modes_64x32[0], rmd_out_tmp01.out_modes_64x32[0]);

	//32x32 64x16
	for (int cu_cnt = 0; cu_cnt < 2; cu_cnt++) {
#pragma HLS UNROLL
		sort_final(cost_mem00.cost_32x32[cu_cnt], rmd_out_tmp00.modes_32x32[cu_cnt], cost_mem10_2.cost_32x32[cu_cnt], rmd_out_tmp10_2.modes_32x32[cu_cnt], rmd_out_tmp00.out_modes_32x32[cu_cnt]);
		sort_final(cost_mem01.cost_32x32[cu_cnt], rmd_out_tmp01.modes_32x32[cu_cnt], cost_mem11_2.cost_32x32[cu_cnt], rmd_out_tmp11_2.modes_32x32[cu_cnt], rmd_out_tmp01.out_modes_32x32[cu_cnt]);

		sort_final(cost_mem00.cost_64x16[cu_cnt], rmd_out_tmp00.modes_64x16[cu_cnt], cost_mem10_2.cost_64x16[cu_cnt], rmd_out_tmp10_2.modes_64x16[cu_cnt], rmd_out_tmp00.out_modes_64x16[cu_cnt]);
		sort_final(cost_mem01.cost_64x16[cu_cnt], rmd_out_tmp01.modes_64x16[cu_cnt], cost_mem11_2.cost_64x16[cu_cnt], rmd_out_tmp11_2.modes_64x16[cu_cnt], rmd_out_tmp01.out_modes_64x16[cu_cnt]);
	}

#if 1	
	//16x32 32x16 64x8
	for (int cu_cnt = 0; cu_cnt < 4; cu_cnt++) {
#pragma HLS UNROLL
		sort_final(cost_mem00.cost_16x32[cu_cnt], rmd_out_tmp00.modes_16x32[cu_cnt], cost_mem10_2.cost_16x32[cu_cnt], rmd_out_tmp10_2.modes_16x32[cu_cnt], rmd_out_tmp00.out_modes_16x32[cu_cnt]);
		sort_final(cost_mem01.cost_16x32[cu_cnt], rmd_out_tmp01.modes_16x32[cu_cnt], cost_mem11_2.cost_16x32[cu_cnt], rmd_out_tmp11_2.modes_16x32[cu_cnt], rmd_out_tmp01.out_modes_16x32[cu_cnt]);

		sort_final(cost_mem00.cost_32x16[cu_cnt], rmd_out_tmp00.modes_32x16[cu_cnt], cost_mem10_2.cost_32x16[cu_cnt], rmd_out_tmp10_2.modes_32x16[cu_cnt], rmd_out_tmp00.out_modes_32x16[cu_cnt]);
		sort_final(cost_mem01.cost_32x16[cu_cnt], rmd_out_tmp01.modes_32x16[cu_cnt], cost_mem11_2.cost_32x16[cu_cnt], rmd_out_tmp11_2.modes_32x16[cu_cnt], rmd_out_tmp01.out_modes_32x16[cu_cnt]);

		sort_final(cost_mem00.cost_64x8[cu_cnt], rmd_out_tmp00.modes_64x8[cu_cnt], cost_mem10_2.cost_64x8[cu_cnt], rmd_out_tmp10_2.modes_64x8[cu_cnt], rmd_out_tmp00.out_modes_64x8[cu_cnt]);
		sort_final(cost_mem01.cost_64x8[cu_cnt], rmd_out_tmp01.modes_64x8[cu_cnt], cost_mem11_2.cost_64x8[cu_cnt], rmd_out_tmp11_2.modes_64x8[cu_cnt], rmd_out_tmp01.out_modes_64x8[cu_cnt]);
	}


	//8x32 32x8 16x16
	for (int cu_cnt = 0; cu_cnt < 8; cu_cnt++) {
#pragma HLS UNROLL
		sort_final(cost_mem00.cost_8x32[cu_cnt], rmd_out_tmp00.modes_8x32[cu_cnt], cost_mem10_2.cost_8x32[cu_cnt], rmd_out_tmp10_2.modes_8x32[cu_cnt], rmd_out_tmp00.out_modes_8x32[cu_cnt]);
		sort_final(cost_mem01.cost_8x32[cu_cnt], rmd_out_tmp01.modes_8x32[cu_cnt], cost_mem11_2.cost_8x32[cu_cnt], rmd_out_tmp11_2.modes_8x32[cu_cnt], rmd_out_tmp01.out_modes_8x32[cu_cnt]);

		sort_final(cost_mem00.cost_32x8[cu_cnt], rmd_out_tmp00.modes_32x8[cu_cnt], cost_mem10_2.cost_32x8[cu_cnt], rmd_out_tmp10_2.modes_32x8[cu_cnt], rmd_out_tmp00.out_modes_32x8[cu_cnt]);
		sort_final(cost_mem01.cost_32x8[cu_cnt], rmd_out_tmp01.modes_32x8[cu_cnt], cost_mem11_2.cost_32x8[cu_cnt], rmd_out_tmp11_2.modes_32x8[cu_cnt], rmd_out_tmp01.out_modes_32x8[cu_cnt]);

		sort_final(cost_mem00.cost_16x16[cu_cnt], rmd_out_tmp00.modes_16x16[cu_cnt], cost_mem10_2.cost_16x16[cu_cnt], rmd_out_tmp10_2.modes_16x16[cu_cnt], rmd_out_tmp00.out_modes_16x16[cu_cnt]);
		sort_final(cost_mem01.cost_16x16[cu_cnt], rmd_out_tmp01.modes_16x16[cu_cnt], cost_mem11_2.cost_16x16[cu_cnt], rmd_out_tmp11_2.modes_16x16[cu_cnt], rmd_out_tmp01.out_modes_16x16[cu_cnt]);
	}
	//16x8 8x16
	for (int cu_cnt = 0; cu_cnt < 16; cu_cnt++) {
#pragma HLS UNROLL
		sort_final(cost_mem00.cost_16x8[cu_cnt], rmd_out_tmp00.modes_16x8[cu_cnt], cost_mem10_2.cost_16x8[cu_cnt], rmd_out_tmp10_2.modes_16x8[cu_cnt], rmd_out_tmp00.out_modes_16x8[cu_cnt]);
		sort_final(cost_mem01.cost_16x8[cu_cnt], rmd_out_tmp01.modes_16x8[cu_cnt], cost_mem11_2.cost_16x8[cu_cnt], rmd_out_tmp11_2.modes_16x8[cu_cnt], rmd_out_tmp01.out_modes_16x8[cu_cnt]);

		sort_final(cost_mem00.cost_8x16[cu_cnt], rmd_out_tmp00.modes_8x16[cu_cnt], cost_mem10_2.cost_8x16[cu_cnt], rmd_out_tmp10_2.modes_8x16[cu_cnt], rmd_out_tmp00.out_modes_8x16[cu_cnt]);
		sort_final(cost_mem01.cost_8x16[cu_cnt], rmd_out_tmp01.modes_8x16[cu_cnt], cost_mem11_2.cost_8x16[cu_cnt], rmd_out_tmp11_2.modes_8x16[cu_cnt], rmd_out_tmp01.out_modes_8x16[cu_cnt]);
	}

	//8x8
	for (int cu_cnt = 0; cu_cnt < 32; cu_cnt++) {
#pragma HLS UNROLL
		sort_final(cost_mem00.cost_8x8[cu_cnt], rmd_out_tmp00.modes_8x8[cu_cnt], cost_mem10_2.cost_8x8[cu_cnt], rmd_out_tmp10_2.modes_8x8[cu_cnt], rmd_out_tmp00.out_modes_8x8[cu_cnt]);
		sort_final(cost_mem01.cost_8x8[cu_cnt], rmd_out_tmp01.modes_8x8[cu_cnt], cost_mem11_2.cost_8x8[cu_cnt], rmd_out_tmp11_2.modes_8x8[cu_cnt], rmd_out_tmp01.out_modes_8x8[cu_cnt]);
	}

#endif	

	for (u8 mode = 0; mode < 32; mode++)
	{
#pragma HLS UNROLL
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			cost_8x32[0][mode + 34][i] = cost_8x32_2[0][mode + 34][i];
			cost_8x32[1][mode + 34][i] = cost_8x32_2[1][mode + 34][i];
		}
	}
#endif

#if Debug_RMD_MODE

	for (int j = 0; j < 32; j++)
	{
		for (int i = 0; i < 5; i++)
		{
			fprintf(rmd_mode, "j=%d i=%d cost00 = %d mode00 = %d\n",j, i, cost_mem00.cost_8x8[j][i], rmd_out_tmp00.out_modes_8x8[j][i]);
			fprintf(rmd_mode, "j=%d i=%d cost01 = %d mode01 = %d\n",j, i, cost_mem01.cost_8x8[j][i], rmd_out_tmp01.out_modes_8x8[j][i]);
		}
	}

	for (int j = 0; j < 66; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			fprintf(rmd_mode, "cost_8x32_0=%d\n", cost_8x32[0][j][i]);
			fprintf(rmd_mode, "cost_8x32_1=%d\n", cost_8x32[1][j][i]);
		}
	}
#endif

#if 1
#if USE_EIPM_RMD
	for (int mode = 0; mode < 66; mode++) {
		if (mode != 33) {
#else
	for (int mode = 0; mode < 33; mode++) {
#endif
#pragma HLS PIPELINE
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			data_reg_8x64[i] = cost_8x32[0][mode][i] + cost_8x32[1][mode][i];
#if USE_SPEED_LEVEL
			sort_modes_for64(data_reg_8x64[i], ip_8x64[i], mode, cost_8x64[i], 1, speed_level);
#else
			sort_modes_for64(data_reg_8x64[i], ip_8x64[i], mode, cost_8x64[i], 1);
#endif

		}
		for (int i = 0; i < 4; i++) {
			data_reg_16x64[i] = data_reg_8x64[2 * i] + data_reg_8x64[2 * i + 1];
#if USE_SPEED_LEVEL
			sort_modes_for64(data_reg_16x64[i], ip_16x64[i], mode, cost_16x64[i], 1, speed_level);
#else
			sort_modes_for64(data_reg_16x64[i], ip_16x64[i], mode, cost_16x64[i], 1);
#endif

		}
		for (int i = 0; i < 2; i++)
		{
			data_reg_32x64[i] = data_reg_16x64[2 * i] + data_reg_16x64[2 * i + 1];
#if USE_SPEED_LEVEL
			sort_modes_for64(data_reg_32x64[i], ip_32x64[i], mode, cost_32x64[i], 1, speed_level);
#else
			sort_modes_for64(data_reg_32x64[i], ip_32x64[i], mode, cost_32x64[i], 1);
#endif

		}
		data_reg_64x64 = data_reg_32x64[0] + data_reg_32x64[1];
#if USE_SPEED_LEVEL
		sort_modes_for64(data_reg_64x64, ip_64x64, mode, cost_64x64, 1, speed_level);
#else
		sort_modes_for64(data_reg_64x64, ip_64x64, mode, cost_64x64, 1);
#endif

	}//1320
#if USE_EIPM_RMD
		}
#endif
#if USE_SPEED_LEVEL
int max_rmd_mode_cnt = 5;
if (speed_level == 1)
max_rmd_mode_cnt = IPD_RDO_CNT; //5
if (speed_level == 2)
max_rmd_mode_cnt = EXTRA_RMD_MODE;//10
else
max_rmd_mode_cnt = IPD_RDO_CNT + EXTRA_RMD_MODE;//15
#else
int max_rmd_mode_cnt = 5;
#endif
for (int i = 0; i < 16; i++) {//����
	for (int k = 0; k < 4; k++) {
#pragma HLS PIPELINE
		for (int j = 0; j < max_rmd_mode_cnt; j++) {
#pragma HLS UNROLL
			//if(k<4){
		 //   rmd_output->modes_4x8[8 * i + k][j] = rmd_out_tmp00.modes_4x8[i * 4 + k][j];
		 //   rmd_output->modes_4x8[8 * i + k + 4][j] = rmd_out_tmp01.modes_4x8[i * 4 + k][j];}
		 //   if(k<2){
		 //   rmd_output->modes_4x16[4 * i + k][j] = rmd_out_tmp00.modes_4x16[i * 2 + k][j];
		 //   rmd_output->modes_4x16[4 * i + k + 2][j] = rmd_out_tmp01.modes_4x16[i * 2 + k][j];}
		 //   if(k==0){
		 //   rmd_output->modes_4x32[2 * i][j] = rmd_out_tmp00.modes_4x32[i][j];
		 //   rmd_output->modes_4x32[2 * i + 1][j] = rmd_out_tmp01.modes_4x32[i][j];}
			if (i < 8) {
				//if(k<8){
				//	 rmd_output->modes_8x4[16 * i + k][j] = rmd_out_tmp00.modes_8x4[i * 8 + k][j];
				//	 rmd_output->modes_8x4[16 * i + k + 8][j] = rmd_out_tmp01.modes_8x4[8 * i + k][j];
				//}
				if (k < 4) {
					rmd_output->modes_8x8[8 * i + k][j] = rmd_out_tmp00.out_modes_8x8[i * 4 + k][j];
					rmd_output->modes_8x8[8 * i + k + 4][j] = rmd_out_tmp01.out_modes_8x8[i * 4 + k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_8x8=%d %d\n", rmd_output->modes_8x8[8 * i + k][j], rmd_output->modes_8x8[8 * i + k + 4][j]);
#endif
				}
				if (k < 2) {
					rmd_output->modes_8x16[4 * i + k][j] = rmd_out_tmp00.out_modes_8x16[i * 2 + k][j];
					rmd_output->modes_8x16[4 * i + k + 2][j] = rmd_out_tmp01.out_modes_8x16[i * 2 + k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_8x16=%d %d\n", rmd_output->modes_8x16[4 * i + k][j], rmd_output->modes_8x16[4 * i + k + 2][j]);
#endif
				}
				if (k == 0) {
					rmd_output->modes_8x32[2 * i][j] = rmd_out_tmp00.out_modes_8x32[i][j];
					rmd_output->modes_8x32[2 * i + 1][j] = rmd_out_tmp01.out_modes_8x32[i][j];
					rmd_output->modes_8x64[i][j] = ip_8x64[i][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_8x32=%d %d\n", rmd_output->modes_8x32[2 * i][j], rmd_output->modes_8x32[2 * i + 1][j]);
					fprintf(rmd_mode, "modes_8x64=%d\n", rmd_output->modes_8x64[i][j]);
#endif
				}
			}
			if (i < 4) {
				//if(k<8){
				//	 rmd_output->modes_16x4[16 * i + k][j] = rmd_out_tmp00.modes_16x4[i * 8 + k][j];
				//	 rmd_output->modes_16x4[16 * i + k + 8][j] = rmd_out_tmp01.modes_16x4[i * 8 + k][j];
				//}
				if (k < 4) {
					rmd_output->modes_16x8[8 * i + k][j] = rmd_out_tmp00.out_modes_16x8[4 * i + k][j];
					rmd_output->modes_16x8[8 * i + k + 4][j] = rmd_out_tmp01.out_modes_16x8[4 * i + k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_16x8=%d %d\n", rmd_output->modes_16x8[8 * i + k][j], rmd_output->modes_16x8[8 * i + k + 4][j]);
#endif
				}
				if (k < 2) {
					rmd_output->modes_16x16[4 * i + k][j] = rmd_out_tmp00.out_modes_16x16[2 * i + k][j];
					rmd_output->modes_16x16[4 * i + k + 2][j] = rmd_out_tmp01.out_modes_16x16[2 * i + k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_16x16=%d %d\n", rmd_output->modes_16x16[4 * i + k][j], rmd_output->modes_16x16[4 * i + k + 2][j]);
#endif
				}
				if (k == 0) {
					rmd_output->modes_16x32[2 * i][j] = rmd_out_tmp00.out_modes_16x32[i][j];
					rmd_output->modes_16x32[2 * i + 1][j] = rmd_out_tmp01.out_modes_16x32[i][j];
					rmd_output->modes_16x64[i][j] = ip_16x64[i][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_16x32=%d %d\n", rmd_output->modes_16x32[2 * i][j], rmd_output->modes_16x32[2 * i + 1][j]);
					fprintf(rmd_mode, "modes_16x64=%d\n", rmd_output->modes_16x64[i][j]);
#endif
				}
			}
			if (i < 2) {
				//if(k<8){
				//	rmd_output->modes_32x4[16 * i + k][j] = rmd_out_tmp00.modes_32x4[i * 8 + k][j];
				//	rmd_output->modes_32x4[16 * i + k + 8][j] = rmd_out_tmp01.modes_32x4[i * 8 + k][j];
				//}
				if (k < 4) {
					rmd_output->modes_32x8[8 * i + k][j] = rmd_out_tmp00.out_modes_32x8[4 * i + k][j];
					rmd_output->modes_32x8[8 * i + k + 4][j] = rmd_out_tmp01.out_modes_32x8[4 * i + k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_32x8=%d %d\n", rmd_output->modes_32x8[8 * i + k][j], rmd_output->modes_32x8[8 * i + k + 4][j]);
#endif
				}
				if (k < 2) {
					rmd_output->modes_32x16[4 * i + k][j] = rmd_out_tmp00.out_modes_32x16[2 * i + k][j];
					rmd_output->modes_32x16[4 * i + k + 2][j] = rmd_out_tmp01.out_modes_32x16[2 * i + k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_32x16=%d %d\n", rmd_output->modes_32x16[4 * i + k][j], rmd_output->modes_32x16[4 * i + k + 2][j]);
#endif
				}
				if (k == 0) {
					rmd_output->modes_32x32[2 * i][j] = rmd_out_tmp00.out_modes_32x32[i][j];
					rmd_output->modes_32x32[2 * i + 1][j] = rmd_out_tmp01.out_modes_32x32[i][j];
					rmd_output->modes_32x64[i][j] = ip_32x64[i][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_32x32=%d %d\n", rmd_output->modes_32x32[2 * i][j], rmd_output->modes_32x32[2 * i + 1][j]);
					fprintf(rmd_mode, "modes_32x64=%d\n", rmd_output->modes_32x64[i][j]);
#endif
				}
			}
			if (i == 0) {
				if (k < 4) {
					rmd_output->modes_64x8[k][j] = rmd_out_tmp00.out_modes_64x8[k][j];
					rmd_output->modes_64x8[k + 4][j] = rmd_out_tmp01.out_modes_64x8[k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_64x8=%d %d\n", rmd_output->modes_64x8[k][j], rmd_output->modes_64x8[k + 4][j]);
#endif
				}
				if (k < 2) {
					rmd_output->modes_64x16[k][j] = rmd_out_tmp00.out_modes_64x16[k][j];
					rmd_output->modes_64x16[k + 2][j] = rmd_out_tmp01.out_modes_64x16[k][j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_64x16=%d %d\n", rmd_output->modes_64x16[k][j], rmd_output->modes_64x16[k + 2][j]);
#endif
				}
				if (k == 0) {
					rmd_output->modes_64x32[0][j] = rmd_out_tmp00.out_modes_64x32[0][j];
					rmd_output->modes_64x32[1][j] = rmd_out_tmp01.out_modes_64x32[0][j];
					rmd_output->modes_64x64[i][j] = ip_64x64[j];
#if Debug_RMD_MODE
					fprintf(rmd_mode, "modes_64x32=%d %d\n", rmd_output->modes_64x32[0][j], rmd_output->modes_64x32[1][j]);
					fprintf(rmd_mode, "modes_64x64=%d\n", rmd_output->modes_64x64[i][j]);
#endif
				}
			}

		}
	}
}//1264
#endif	
	}


	static void save_rmd_buf(U12 pix_x, const pel fenc_lcu_y[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_BUF * rmd_buf, RMD_BUF * rmd_buf2)
	{
#pragma HLS ARRAY_PARTITION variable=fenc_lcu_y cyclic factor=64 dim=1
#ifndef __SYNTHESIS__
		assert((u32)(pix_x + MAX_CU_SIZE2 - 1) < ARRAY_SIZE(rmd_buf->up));
#endif
		for (int i = 0; i < (MAX_CU_SIZE2 - 1); i++) {
#pragma HLS PIPELINE
			rmd_buf->up[pix_x + i] = fenc_lcu_y[to_idx(MAX_CU_SIZE2 - 1, i)];
			rmd_buf->left[i] = fenc_lcu_y[to_idx(i, MAX_CU_SIZE2 - 1)];

			rmd_buf2->up[pix_x + i] = fenc_lcu_y[to_idx(MAX_CU_SIZE2 - 1, i)];
			rmd_buf2->left[i] = fenc_lcu_y[to_idx(i, MAX_CU_SIZE2 - 1)];
		}
		pel v = fenc_lcu_y[to_idx(MAX_CU_SIZE2 - 1, MAX_CU_SIZE2 - 1)];
		rmd_buf->up[pix_x + MAX_CU_SIZE2 - 1] = v;
		rmd_buf->left[MAX_CU_SIZE2 - 1] = v;
		rmd_buf->top_left = v;

		rmd_buf2->up[pix_x + MAX_CU_SIZE2 - 1] = v;
		rmd_buf2->left[MAX_CU_SIZE2 - 1] = v;
		rmd_buf2->top_left = v;
	}

#if USE_SPEED_LEVEL
	void rmd_run(U6 real_lcu_width, U6 real_lcu_height, U12 * pix_x, U12 * pix_y, u32 lambda, pel Orig_Lcu_ptr_p_fenc_LCU_Y_rmd[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_OUTPUT * rmd_output, U8 speed_level)
#else
	void rmd_run(U7 real_lcu_width, U7 real_lcu_height, U12 * fetch_output_rmd_pix_x, U12 * fetch_output_rmd_pix_y, u32 fetch_output_rmd_lambda_mv, pel Fetch_Orig_Lcu_p_fenc_LCU_Y_rmd[MAX_CU_SIZE2 * MAX_CU_SIZE2], RMD_OUTPUT * rmd_output)
#endif
	{
		pel Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner[MAX_CU_SIZE2 * MAX_CU_SIZE2];
#if COLLECT_RMD_DATA

		if (rmd_cu_cnt == 0) {
			rmd_input_pix_x = open_file(RMD_INPUT_pix_x, "wb");
			rmd_input_pix_y = open_file(RMD_INPUT_pix_y, "wb");
			rmd_input_LCU_Y = open_file(RMD_INPUT_LCU_Y, "wb");
			rmd_output_modes = open_file(RMD_OUTPUT_modes, "wb");
		}
		if (rmd_cu_cnt < RMD_CU_NUM_TV) {
			rmd_save_indata(pix_x, pix_y, Orig_Lcu_ptr_p_fenc_LCU_Y_rmd);
			printf("RMD: LCU %d  INPUT DATA SAVED. \n", rmd_cu_cnt);
		}

#endif

#pragma HLS RESOURCE variable=Fetch_Orig_Lcu_p_fenc_LCU_Y_rmd core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=fetch_output_rmd_pix_x
#pragma HLS INTERFACE axis port=fetch_output_rmd_pix_y

#pragma HLS INTERFACE axis port=real_lcu_width
#pragma HLS INTERFACE axis port=real_lcu_height
#pragma HLS INTERFACE axis port=fetch_output_rmd_lambda_mv


#pragma HLS ARRAY_PARTITION variable=Fetch_Orig_Lcu_p_fenc_LCU_Y_rmd cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner cyclic factor=64 dim=1

		U12 pix_x_tmp = *fetch_output_rmd_pix_x;
		U12 pix_y_tmp = *fetch_output_rmd_pix_y;

		static RMD_BUF g_rmd_buf;
		static RMD_BUF g_rmd_buf2;
#pragma HLS ARRAY_PARTITION variable=g_rmd_buf.left complete dim=1
#pragma HLS ARRAY_PARTITION variable=g_rmd_buf.up  cyclic factor=64 dim=1//16

#pragma HLS ARRAY_PARTITION variable=g_rmd_buf2.left complete dim=1
#pragma HLS ARRAY_PARTITION variable=g_rmd_buf2.up  cyclic factor=64 dim=1//16
		if (pix_x_tmp == 0 && pix_y_tmp == 0) {
			g_rmd_buf2.top_left = 128;
			g_rmd_buf2.top_left = 128;
			for (int i = 0; i < 64; i++) {
#pragma HLS unroll
				g_rmd_buf2.left[i] = 128;
			}
			for (int j = 0; j < 60; j++) {
#pragma HLS pipeline
				for (int i = 0; i < 64; i++) {
#pragma HLS unroll
					g_rmd_buf2.up[i + j * 64] = 128;
				}
			}

		}

		for (int j = 0; j < 64; j++) {
#pragma HLS pipeline
			for (int i = 0; i < 64; i++) {
#pragma HLS unroll
				Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner[i + j * 64] = 128;
			}
		}

		//        for (int j = 0; j < real_lcu_height; j++) {
		//#pragma HLS pipeline
		//            for (int i = 0; i < real_lcu_width; i++) {
		//#pragma HLS unroll
		//                Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner[i + j * 64] = Orig_Lcu_ptr_p_fenc_LCU_Y_rmd[i + j * 64];
		//            }
		//        }
		for (int j = 0; j < 64; j++) {
#pragma HLS pipeline
			for (int i = 0; i < 64; i++) {
#pragma HLS unroll
				if (i < real_lcu_width && j < real_lcu_height)
					Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner[i + j * 64] = Fetch_Orig_Lcu_p_fenc_LCU_Y_rmd[i + j * 64];
			}
		}

#if USE_SPEED_LEVEL
		calc_scu_cost(pix_x_tmp, pix_y_tmp, Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner, &g_rmd_buf, rmd_output, lambda, speed_level);
#else
		calc_scu_cost(pix_x_tmp, pix_y_tmp, Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner, &g_rmd_buf, &g_rmd_buf2, rmd_output, fetch_output_rmd_lambda_mv);
#endif

		save_rmd_buf(pix_x_tmp, Orig_Lcu_ptr_p_fenc_LCU_Y_rmd_inner, &g_rmd_buf, &g_rmd_buf2);


#if COLLECT_RMD_DATA

		if (rmd_cu_cnt < RMD_CU_NUM_TV) {
			rmd_save_outdata(rmd_output);
			printf("RMD: LCU %d OUTPUT DATA SAVED. \n", rmd_cu_cnt);
			printf("------------------------------\n", rmd_cu_cnt);
			rmd_cu_cnt += 1;
		} -modes_16x16	0x044ce660 {0x044ce660 "", 0x044ce665 "", 0x044ce66a "", 0x044ce66f "\x1b\x1c\x2\x1a", 0x044ce674 "", ...}	unsigned char[16][5]

		else if (rmd_cu_cnt == RMD_CU_NUM_TV) {
			fclose(rmd_input_pix_x);
			fclose(rmd_input_pix_y);
			fclose(rmd_input_LCU_Y);
			fclose(rmd_output_modes);
			printf("Files closed. \n");
			rmd_cu_cnt += 1;
		}

#endif

	}
#endif // TEST_RMD_ALG

#endif
