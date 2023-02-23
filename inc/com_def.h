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
extern int pic_cnt;
extern int isProblem;
#define Debug422 0
#ifndef _COM_DEF_H_
#define _COM_DEF_H_

#define DISABLE_CU 0
#define DISABLE_CU_64 DISABLE_CU&&1
#define DISABLE_CU_32 DISABLE_CU&&1
#define DISABLE_CU_16 DISABLE_CU&&1
#define DISABLE_CU_8 DISABLE_CU&&1

#define CDEBUG 0
#define CDEBUG_MAP_SCU	0
#define CDEBUG_CU_DATA	1

//#define PRINT_IP_BUFFER		1
//#define PRINT_ME_LINE_MAP	1
//#define PRINT_MPM_BUFFER	1
//#define PRINT_IP_MAP_SCU	1
//#define PRINT_CU_DATA		1

//#define PRINT_KERNEL_16		1

//#define MD_TV_ACTIVE

//#define TEST_VECTOR 0

//#include "ap_int.h"
#include "com_typedef.h"
#include "com_port.h"
#include "type_def.h"

#define Debug_TWO_PASS     0
#define Debug_RMD_MODE     0
#define Debug_MD_COST      1
#define USE_INTRA_REFRESH  0

#define USE_SPEED_LEVEL    0

#define USE_ROI_QP_CONFIG  0

#define USE_ROI_MODE_CONFIG  0

#define USE_TRACE_DUMP     0

#define SIMPLE_MD          1
#if SIMPLE_MD
#define ONLY_NS_16X16      1
#endif // SIMPLE_MD

#define SAO_YUV			   1	//1: sao copy yuv, canot hls; 0: sao not copy yuv, for hls

#if USE_SPEED_LEVEL
#define EXTRA_RMD_MODE     10
#define EXTRA_RDO_MODE     10
#else
#define EXTRA_RMD_MODE     0
#define EXTRA_RDO_MODE     0
#endif


#define size_8x8_close		0
#define size_8x16_close		0
#define size_16x8_close		0
#define size_16x16_close	0
#define size_16x32_close	0
#define size_32x16_close	0
#define size_32x32_close	0
#define size_8x64_close		0
#define	size_64x8_close		0
#define	size_16x64_close	0
#define size_64x16_close	0
#define size_32x64_close	0
#define size_64x32_close	0
#define size_64x64_close	0
#define KERNEL_SIZE_CLOSE(m, n)	size_##m##x##n##_close

#define size_8x16_md2_close				size_8x16_close || 0
#define size_16x8_md2_close				size_16x8_close || 0
#define size_8x16_md1_top_bt_close		size_8x16_close || 0
#define size_16x8_md1_top_bt_close		size_16x8_close || 0

// only 64x64->32x32->16x16->...
#ifndef ONLY_QT_QT
#define ONLY_QT_QT 1
#endif // ONLY_QT_QT
// input channel and output channel is only used to debug channel data
// this would be removed in final mdtop rtl
#ifndef MD_CHANNEL_DEBUG
#define MD_CHANNEL_DEBUG 0
#endif // MD_CHANNEL_DEBUG

// for AZB based MD integration. all zero block. 
// nazb for intra frame, and azb for p/b frame.
#define AZB_ENABLE 0  
#define sao_print 0
//#define avs3_min(a,b) a<b?a:b
//#define avs3_max(a,b) a>b?a:b

#define USE_ZXZ_HGOP	1

#define ENABLE_RFC_FETCH      1

#define	MD_BASE_PROFILE		1	
#define MD_MC_LUMA_REMOVE	1	

//#define MD_CMOD
#define MD_CMOD_8X8          0
#define MD_CMOD_16X16        1

#define RMD_TEST_VECTOR_NEW 0
#define CMOD_LUMA_16_LITE	0
#ifdef MD_CMOD
#define RDOQ_OPEN  0
#else
#if SIMPLE_MD
#define RDOQ_OPEN  0
#else
#define RDOQ_OPEN  1
#endif
#endif

#if RDOQ_OPEN
#define RATE_EST_SWITCH 1
#else
#define RATE_EST_SWITCH 0
#endif

#define PATCH_DEBUG 0
#define PATCH_TEST 0

#if __SYNTHESIS__
#define STATIC_FOR_STACK
#else
#define STATIC_FOR_STACK static
#endif

#define SKIP_FRAME 0
#define REC_GOLDEN "test_rec.yuv"
#define MAP_GOLDEN "test_map.dat"

#define EXTEND_ARGS(args) args
#define _FUNC_NAME(name, w, h) name##_##w##x##h
#define FUNC_NAME(name, w, h) EXTEND_ARGS(_FUNC_NAME(name, w, h))
#define _ME_NAME(name, w) name##w
#define ME_NAME(name, w) EXTEND_ARGS(_ME_NAME(name, w))
#define _FME_NAME(name0, name1, w, h) name0##w##x##h##name1
#define FME_NAME(name0, name1, w, h) EXTEND_ARGS(_FME_NAME(name0, name1, w, h))

#define DECL_FUNC(func) \
	func(64, 64, 64); \
	func(64, 32, 32); \
	func(64, 16, 16); \
	func(64, 8, 8); \
	func(8, 64, 8); \
	func(16, 64, 16); \
	func(32, 64, 32); \
	func(32, 32, 32); \
	func(32, 16, 16); \
	func(32, 8, 8); \
	func(32, 4, 4); \
	func(4, 32, 4); \
	func(8, 32, 8); \
	func(16, 32, 16); \
	func(16, 16, 16); \
	func(16, 8, 8); \
	func(16, 4, 4); \
	func(4, 16, 4); \
	func(8, 16, 8); \
	func(8, 8, 8); \
	func(8, 4, 4); \
	func(4, 8, 4); \
	func(4, 4, 4);

#define MAX_NUM_SMB                       2040//32400    //zhaof
#define MAX_NUM_LCU_ROW                   60

#define INTER_MERGE 1

#define MIN_CU_BIT		MIN_CU_LOG2
#define MAX_CU_BIT		(MIN_CU_LOG2 + MAX_CU_DEPTH - 1)
#define CU_DEPTH_BIT	(MAX_CU_DEPTH - 1)
#define MAX_CU			MAX_CU_SIZE2
#define MAX_CU_IN_SCU	(1 << CU_DEPTH_BIT)

#define PATCH_DIVIDE_ENABLE					1

#define REFP_SURD_OFF_W	(SRW + FENC_PADDING)
#define REFP_SURD_OFF_H (SRH + FENC_PADDING)

//#if PATCH_DIVIDE_ENABLE
//#define REFP_SURD_OFF_W_PATCH (SRW/2 - 1)//0
//#define REFP_SURD_OFF_H_PATCH (SRH/2 - 1)//0
//#else
//#define REFP_SURD_OFF_W	(SRW + FENC_PADDING)
//#define REFP_SURD_OFF_H (SRH + FENC_PADDING)
//#endif

#define REFP_CU_SIZE	(2 * REFP_SURD_OFF + MAX_CU)

#define SKIP_HMVP_NUM					   0
#define SKIP_UMVE_BASE_NUM				   2
#define SKIP_UMVE_REFINE_NUM			   1
#define SKIP_UMVE_DIRECTION_NUM			   4
#define SKIP_UMVE_NUM					   SKIP_UMVE_BASE_NUM * SKIP_UMVE_REFINE_NUM * SKIP_UMVE_DIRECTION_NUM
#define SKIP_MODE_NUM					  (4 + SKIP_HMVP_NUM + SKIP_UMVE_NUM)
#define SKIP_RDO_NUM						4

#define SKIP_MC_NUM 			2
#define SKIP_MC_HOR_PAD			8
#define SKIP_MC_VER_PAD			2
#define SKIP_MC_HOR_NUM			(2 * SKIP_MC_HOR_PAD + 1)					// 17
#define SKIP_MC_VER_NUM			(2 * SKIP_MC_VER_PAD + 1)					// 5
#define SKIP_MC_OUT_WIDTH		(SKIP_MC_HOR_NUM + W - 1)			// W + 16
#define SKIP_MC_OUT_HEIGHT		(SKIP_MC_VER_NUM + H - 1)			// H + 4
#define SKIP_MC_IN_WIDTH		(SKIP_MC_OUT_WIDTH + 7)						// W + 23
#define SKIP_MC_IN_HEIGHT		(SKIP_MC_OUT_HEIGHT + 7)					// H + 11

#define SKIP_MC_OUT_WIDTH_16		(SKIP_MC_HOR_NUM + 16 - 1)			// W + 16
#define SKIP_MC_OUT_HEIGHT_16		(SKIP_MC_VER_NUM + 16 - 1)			// H + 4
#define SKIP_MC_IN_WIDTH_16		(SKIP_MC_OUT_WIDTH_16 + 7)						// W + 23
#define SKIP_MC_IN_HEIGHT_16		(SKIP_MC_OUT_HEIGHT_16 + 7)					// H + 11


#define SKIP_MC_OUT_WIDTH_64		(SKIP_MC_HOR_NUM + 64 - 1)
#define SKIP_MC_OUT_HEIGHT_64		(SKIP_MC_VER_NUM + 64 - 1)
#define SKIP_MC_IN_WIDTH_64		(SKIP_MC_OUT_WIDTH_64 + 7)	
#define SKIP_MC_IN_HEIGHT_64		(SKIP_MC_OUT_HEIGHT_64 + 7)

#if SUB_TMVP
#define SBTMVP_MIN_SIZE                    16
#define SBTMVP_NUM                         4
#define SBTMVP_NUM_1D                      2
#endif

#define aec_max(x, y) (((x) > (y)) ? (x) : (y))
#define aec_min(x, y) (((x) < (y)) ? (x) : (y))

#define __assert_HLS(a,b,c,d)   assert(0)
//printf(a)
#pragma warning(disable:4554 4244 4819)
#define CLOSE_CU_SPLIT_EARLY_TERMINATION              1 //xgq
#define USE_CTU64_ARRARY                              1 //xgq
#define EQT_OFF                                       0 //xgq
#define SET_SIGN_SYNTAX                               1 //xgq
#define CLOSE_CU_WH_1_8_ROOT                          0 //xgq
#define CLOSE_CU_WH_1_4_ROOT                          0 //xgq
#define CLOSE_AEC_MCU_FLAG                            1 //xgq
#define CLOSE_CHROMA_COST                             1 //xgq//xingxing
#define TEST_CLOSE_I                                  0 //xgq

//only for HLS
#define VIVTIS_HLS_4X4                                0 //xgq
#define INTRA_HLS_8X8                                 0     
#define INTRA_8x8                                     0

//#define SAO_easy_stat                                 1 //zhaof
//#define SAO_easy_rdo                                  1 //zhaof
#define DBK2SAO                                       1

#define ATAUS_rdo_ppl                                 0 //zhaof
#define ATAUS_ppl                                     1

#define use_pic_rec                                   0 //zhaof

#define DBK_LCU										  1//LCU-level DBK //huangj
#define IP_Linebuffer								  1 //liuky
#define Use_MPM										  1 //liuky
#define max_buffer_width							  3840//liuky
#define tscpm_enable								  0//liuky
//#define IP_map_scu									  1//liuky
#define IPRED_HLS									  1//liuky
#define IPRED_8x8_HLS								  1//liuky
#define RMD_ARRAY									  1//liuky
#define RMD_Linebuffer								  0 //chenglei
#define FIXED_POINT								      1 //chencx

#define FIXED_POINT_SAO								  1//liuxy
#define FIXED_POINT_NUM                               1000//liuxy
#define MAX_S64                                       9223372036854775807 //liuxy avs3_max. value of signed 64-bit integer
#define MAX_S56										  (0xFFFFFFF)
#define MAX_S32                                       2147483647

#define FIXED_LAMBDA_NUM                              1024//ludd
#define FIXED_SQRT_LAMBDA_NUM                         65535//ludd
#define LAMBDA_SCALE                                  10//xgq

#if TSCPM
#define MAX_INT                     2147483647  ///< avs3_max. value of signed 32-bit integer
#endif

/* profile & level */
#if USE_ENH
#if PHASE_2_PROFILE
#define PROFILE_ID 0x32
#else
#define PROFILE_ID 0x22
#endif
#else
#define PROFILE_ID 0x22
#endif
#define LEVEL_ID 0x6A

/* MCABAC (START) */
#define PROB_BITS                         11 // LPS_PROB(10-bit) + MPS(1-bit)
#define PROB_MASK                         ((1 << PROB_BITS) - 1) // mask for LPS_PROB + MPS
#define MAX_PROB                          ((1 << PROB_BITS) - 1) // equal to PROB_LPS + PROB_MPS, 0x7FF
#define HALF_PROB                         (MAX_PROB >> 1)
#define QUAR_HALF_PROB                    (1 << (PROB_BITS-3))
#define LG_PMPS_SHIFTNO                   2

#if CABAC_MULTI_PROB
#define PROB_INIT                         (HALF_PROB << 1) + (HALF_PROB << PROB_BITS) 
#else
#define PROB_INIT                         (HALF_PROB << 1) /* 1/2 of initialization = (HALF_PROB << 1)+ MPS(0) */
#endif

#define READ_BITS_INIT                    16
#define DEC_RANGE_SHIFT                   (READ_BITS_INIT-(PROB_BITS-2))

#define COM_BSR_IS_BYTE_ALIGN(bs)         !((bs)->leftbits & 0x7)

#if CABAC_MULTI_PROB
#define MAX_WINSIZE                        9
#define MIN_WINSIZE                        2
#define MCABAC_SHIFT_I                     5
#define MCABAC_SHIFT_B                     5
#define MCABAC_SHIFT_P                     5
#define CYCNO_SHIFT_BITS                   21   // PROB_BITS
#define MCABAC_PROB_BITS                   10
#define MCABAC_PROB_MASK                   ((1 << MCABAC_PROB_BITS) - 1)
#define COUNTER_THR_I                      8
#define COUNTER_THR_B                      16
#define COUNTER_THR_P                      16
#endif
/* MCABAC (END) */

/* Multiple Reference (START) */
#define MAX_NUM_ACTIVE_REF_FRAME_B         2  /* Maximum number of active reference frames for RA condition */
#define MAX_NUM_ACTIVE_REF_FRAME_LDB       4  /* Maximum number of active reference frames for LDB condition */
#define MV_SCALE_PREC                      14 /* Scaling precision for motion vector prediction (2^MVP_SCALING_PRECISION) */
/* Multiple Reference (END) */

/* Max. and avs3_min. Quantization parameter */
#define MIN_QUANT                          0
#define MAX_QUANT_BASE                     63


/* AMVR (START) */
#define MAX_NUM_MVR                        5  /* 0 (1/4-pel) ~ 4 (4-pel) */
#if BD_AFFINE_AMVR
#define MAX_NUM_AFFINE_MVR                 3
#endif
#define FAST_MVR_IDX                       2
#define SKIP_MVR_IDX                       1
/* AMVR (END)  */

/* DB_AVS2 (START) */
#define EDGE_TYPE_LUMA                     1
#define EDGE_TYPE_ALL                      2
#define LOOPFILTER_DIR_TYPE                2
#define LOOPFILTER_SIZE_IN_BIT             2
#define LOOPFILTER_SIZE                    (1 << LOOPFILTER_SIZE_IN_BIT)
#define LOOPFILTER_GRID                    8
#define DB_CROSS_SLICE                     1
#define PIC_MAX_WIDTH                    1920   //added by huangju, used by zhaof as well 
#define PIC_MAX_HEIGHT                   1080   //added by huangju
#define DBK_UNRECUR                        1    //added by huangju
/* DB_AVS2 (END) */

/* SAO_AVS2 (START) */
#define NUM_BO_OFFSET                      32
#define MAX_NUM_SAO_CLASSES                32
#define NUM_SAO_BO_CLASSES_LOG2            5
#define NUM_SAO_BO_CLASSES_IN_BIT          5
#define NUM_SAO_EO_TYPES_LOG2              2
#define NUM_SAO_BO_CLASSES                 (1<<NUM_SAO_BO_CLASSES_LOG2)
#define SAO_SHIFT_PIX_NUM                  4
/* SAO_AVS2 (END) */
/* ESAO_AVS3 (START) */
#if ESAO
#define ESAO_LUMA_TYPES                    2  // 1 : only 17 classes; 2: both 17 and 9 classes are applied
#define NUM_ESAO_LUMA_TYPE0                17   //
#define NUM_ESAO_LUMA_TYPE1                9

#if ESAO_ENH
#define ESAO_SET_NUM                       2
#define ESAO_SET_NUM_BIT                   1
#endif
//Y
#define ESAO_LABEL_NUM_Y                   16   //
#define ESAO_LABEL_NUM_IN_BIT_Y            4 //bits
#define ESAO_LABEL_CLASSES_Y               272  //16*17
//U
#define ESAO_LABEL_NUM_U                   96
#define ESAO_LABEL_NUM_IN_BIT_U            7
#define ESAO_LABEL_CLASSES_U               272
//V
#define ESAO_LABEL_NUM_V                   96
#define ESAO_LABEL_NUM_IN_BIT_V            7 
#define ESAO_LABEL_CLASSES_V               272 

//maximum value among  Y, U and V
#define ESAO_LABEL_NUM_MAX                 96
#define ESAO_LABEL_NUM_IN_BIT_MAX          7 
#define ESAO_LABEL_CLASSES_MAX             272
#endif
/* ESAO_AVS3 (END) */

#if CCSAO
/* CCSAO_AVS3 (START) */
#if CCSAO_ENHANCEMENT
#define CCSAO_SET_NUM                      4
#define CCSAO_SET_NUM_BIT                  2
#endif

#define CCSAO_TYPE_NUM                     9
#define CCSAO_TYPE_NUM_BIT                 4

#define CCSAO_BAND_NUM                     16
#define CCSAO_BAND_NUM_BIT                 4

#if CCSAO_ENHANCEMENT
#define CCSAO_BAND_NUM_C                   2
#define CCSAO_BAND_NUM_BIT_C               1
#define CCSAO_CLASS_NUM                    (CCSAO_BAND_NUM * CCSAO_BAND_NUM_C)
#else
#define CCSAO_CLASS_NUM                    CCSAO_BAND_NUM
#endif

#if ECCSAO
#define CCSAO_EDGE_BAND_NUM_Y                  2
#define CCSAO_EDGE_BAND_NUM_C                  2

#define CCSAO_EDGE_COMPARE_VALUE               2
#define CCSAO_EDGE_BAND_NUM_BIT                2 

#define CCSAO_EDGE_TYPE                        4
#define CCSAO_EDGE_TYPE_BIT                    2

#define CCSAO_EDGE_NUM                         16 

#define CCSAO_QUAN_NUM                         8 
#define CCSAO_QUAN_NUM_BIT                     3
#endif

#if CCSAO_LINE_BUFFER
#define CCSAO_PAD_ROWS                         3
#endif
/* CCSAO_AVS3 (END) */
#endif

/* SAWP (START) */
#if SAWP
#define SAWP_MAX_SIZE_LOG2                 5
#define SAWP_MIN_SIZE_LOG2                 3
#define SAWP_MAX_SIZE                      (1 << SAWP_MAX_SIZE_LOG2)
#define SAWP_MIN_SIZE                      (1 << SAWP_MIN_SIZE_LOG2)
#define SAWP_MIN_PIXEL                     64

#if SAWP_MPM_SIMP
#define SAWP_MPM_NUM                       2
#else // SAWP_MPM_SIMP
#define SAWP_MPM_NUM                       4
#endif // SAWP_MPM_SIMP
#define SAWP_CPP                           10  // candidate per partition
#define SAWP_RDO_NUM                       10

	/*fast algorithm*/
#define SAWP_COST_FAST                     1
#define SAWP_SAVE_LOAD                     1
#endif // SAWP
/* SAWP (END) */


/* ALF_AVS2 (START) */
#define ALF_LOW_LANTENCY_ENC               0
#define ALF_MAX_NUM_COEF                   9
#define NO_VAR_BINS                        16
#define ALF_REDESIGN_ITERATION             3
#define LOG2_VAR_SIZE_H                    2
#define LOG2_VAR_SIZE_W                    2
#define ALF_FOOTPRINT_SIZE                 7
#define DF_CHANGED_SIZE                    3
#define ALF_NUM_BIT_SHIFT                  6
/* ALF_AVS2 (END) */
#if USE_IBC
#define MODE_IBC                           4
#endif
/* Common stuff (START) */
#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#define FORCE_INLINE __forceinline
//#define INLINE __inline
#else
#if defined(__GNUC__)
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#define FORCE_INLINE __attribute__((always_inline))
//#define INLINE __inline__
#endif
#endif

#define avs3_max(x, y) (((x) > (y)) ? (x) : (y))
#define avs3_min(x, y) (((x) < (y)) ? (x) : (y))
#define Clip3(x, y, z) (((z) < (x)) ? (x) : (((z) > (y)) ? (y) : (z)))
typedef int BOOL;
#define TRUE  1
#define FALSE 0

/* AFFINE (START) */
#define VER_NUM                             4
#define AFFINE_MAX_NUM_LT                   3 ///< avs3_max number of motion candidates in top-left corner
#define AFFINE_MAX_NUM_RT                   2 ///< avs3_max number of motion candidates in top-right corner
#define AFFINE_MAX_NUM_LB                   2 ///< avs3_max number of motion candidates in left-bottom corner
#define AFFINE_MAX_NUM_RB                   1 ///< avs3_max number of motion candidates in right-bottom corner
#define AFFINE_MIN_BLOCK_SIZE               4 ///< Minimum affine MC block size

#define AFF_MAX_NUM_MRG                     5 // maximum affine merge candidates
#define AFF_MODEL_CAND                      2 // maximum affine model based candidate

#define MAX_MEMORY_ACCESS_BI                ((8 + 7) * (8 + 7) / 64)
#define MAX_MEMORY_ACCESS_UNI               ((8 + 7) * (4 + 7) / 32)
#define MVF_RANGE_DIM 2
#define MVF_RANGE_MIN 0
#define MVF_RANGE_MAX 1
#define DIR_NUMS 2
#define DIR_HOR 0
#define DIR_VER 1

// AFFINE ME configuration (non-normative)
#define AF_ITER_UNI                         7 // uni search iteration time
#define AF_ITER_BI                          5 // bi search iteration time
#define AFFINE_BI_ITER                      1

#define AFF_SIZE                            16
/* AFFINE (END) */

/* IPF (START) */
#define NUM_IPF_CTX                         1 ///< number of context models for MPI Idx coding
#if DT_PARTITION
#define DT_INTRA_BOUNDARY_FILTER_OFF        1 ///< turn off boundary filter if intra DT is ON
#endif
/* IPF (END) */
/* MIPF (START) */
#if MIPF
#define MIPF_TH_SIZE                       64
#define MIPF_TH_SIZE_CHROMA                32
#define MIPF_TH_DIST                       1
#define MIPF_TH_DIST_CHROMA                2
#endif
/* MIPF (END) */

#define DEBUG_CU 0
/* For debugging (START) */
#define ENC_DEC_TRACE                       0
#if ENC_DEC_TRACE
#define MVF_TRACE                           0 ///< use for tracing MVF
#define TRACE_REC                           0 ///< trace reconstructed pixels
#define TRACE_RDO                           0 //!< Trace only encode stream (0), only RDO (1) or all of them (2)
#define TRACE_BIN                           1 //!< trace each bin
#if TRACE_RDO
#define TRACE_RDO_EXCLUDE_I                 0 //!< Exclude I frames
#endif
#define PRINT_TOSTOUT 0
#define DST_PTR 0
extern FILE* fp_trace;
extern int fp_trace_print;
extern int fp_trace_counter;
extern int aec_is_this_line;
#if TRACE_RDO == 1
#define COM_TRACE_SET(A) fp_trace_print=!A
#elif TRACE_RDO == 2
#define COM_TRACE_SET(A)
#else
#define COM_TRACE_SET(A) fp_trace_print=A
#endif
#if PRINT_TOSTOUT
#define COM_TRACE_FUNC if(fp_trace_print) { printf("[%s]", __FUNCTION__); fflush(fp_trace); }
#define COM_TRACE_STR(STR) if(fp_trace_print) { printf(STR); fflush(stdout); }
#define COM_TRACE_INT(INT) if(fp_trace_print) { printf("%d ", INT); fflush(stdout); }
#define COM_TRACE_COUNTER  COM_TRACE_INT(fp_trace_counter++); COM_TRACE_STR("\t")
#define COM_TRACE_MV(X, Y) if(fp_trace_print) { printf("(%d, %d) ", X, Y); fflush(stdout); }
#define COM_TRACE_FLUSH    if(fp_trace_print) fflush(stdout)
#if TRACE_BIN
#define COM_TRACE_DOUBLE(D) if(fp_trace_print) { printf("%e ", D); fflush(stdout); }
#endif
#else
#define COM_TRACE_FUNC if(fp_trace_print) { fprintf(fp_trace, __FUNCTION__); fflush(fp_trace); }
#define COM_TRACE_STR(STR) if(fp_trace_print) { fprintf(fp_trace, STR); fflush(fp_trace); }
#define COM_TRACE_INT(INT) if(fp_trace_print) { fprintf(fp_trace, "%d ", INT); fflush(fp_trace); }
#define COM_TRACE_COUNTER  COM_TRACE_INT(fp_trace_counter++); COM_TRACE_STR("\t")
#define COM_TRACE_MV(X, Y) if(fp_trace_print) { fprintf(fp_trace, "(%d, %d) ", X, Y); fflush(fp_trace); }
#define COM_TRACE_FLUSH    if(fp_trace_print) fflush(fp_trace)
#if TRACE_BIN
#define COM_TRACE_DOUBLE(D) if(fp_trace_print) { fprintf(fp_trace, "%e ", D); fflush(fp_trace); }
#endif
#endif
#else
#define COM_TRACE_SET(A)
#define COM_TRACE_STR(str)
#define COM_TRACE_INT(INT)
#define COM_TRACE_COUNTER
#define COM_TRACE_MV(X, Y)
#define COM_TRACE_FLUSH
#endif
/* For debugging (END) */

#define C_P_S                              1 //chroma prediction switch 2to1 & 6to1
#define STRIDE_IMGB2PIC(s_imgb)          ((s_imgb)>>1)

#define Y_C                                0  /* Y luma */
#define U_C                                1  /* Cb Chroma */
#define V_C                                2  /* Cr Chroma */
#define N_C                                3  /* number of color component */

#define REFP_0                             0
#define REFP_1                             1
#define REFP_NUM                           2

/* X direction motion vector indicator */
#define MV_X                               0
/* Y direction motion vector indicator */
#define MV_Y                               1
/* Maximum count (dimension) of motion */
#define MV_D                               2

#define N_REF                              2  /* left, up, right */

#define NUM_NEIB                           1  //since SUCO is not implemented

#define MINI_SIZE                          8
#define MAX_CU_LOG2                        6
#define MIN_CU_LOG2                        2
#define MAX_CU_SIZE                       (1 << (7))
#define MIN_CU_SIZE                       (1 << MIN_CU_LOG2)
#define MIN_CU_DIM                        (MIN_CU_SIZE * MIN_CU_SIZE)

#define MAX_CU_SIZE2                      (MAX_CU_SIZE >> 1) //addby xgq
#define MAX_CU_DEPTH                      5  /* 64x64 ~ 4x4 */
#define MAX_CU_DIM                        (MAX_CU_SIZE2 * MAX_CU_SIZE2)
#define MAX_CU_DIM_C                     (MAX_CU_SIZE2 * MAX_CU_SIZE2)>>2

#define MAX_TR_LOG2                        6  /* 64x64 */
#define MIN_TR_LOG2                        1  /* 2x2 */
#define MAX_TR_SIZE                        (1 << MAX_TR_LOG2)
#define MIN_TR_SIZE                        (1 << MIN_TR_LOG2)
#define MAX_TR_DIM                         (MAX_TR_SIZE * MAX_TR_SIZE)
#define MIN_TR_DIM                         (MIN_TR_SIZE * MIN_TR_SIZE)

#if TB_SPLIT_EXT
#define MAX_NUM_PB                         4
#define MAX_NUM_TB                         4
#define PB0                                0  // default PB idx
#define TB0                                0  // default TB idx
#define TBUV0                              0  // default TB idx for chroma

#define NUM_SL_INTER                       10
#define NUM_SL_INTRA                       8
#endif

/* SP (START) */
#if USE_SP
#define SP_STRING_INFO_NO                  1024//  1 << ( MAX_CU_LOG2 + MAX_CU_LOG2 - 4)//64*64 / 4 = 1024 //number of max sp string_copy_info in a CU
#define SP_MAX_SPS_CANDS                   256
#define SP_RECENT_CANDS                    12
#define SP_EXG_ORDER                       5
#define MAX_CU_SIZE_IN_BIT                 6
#define MAX_SRB_SIZE                       15
#define MAX_SSRB_SIZE                      17
#define MAX_SRB_PRED_SIZE                  28
#define UV_WEIGHT                          0.25
#define EVS_PV_MAX                         10
#define EVS_PV_8                           5
#define EVS_PV_4                           2
#define GET_TRAV_X(trav_idx, cu_width)                  ((trav_idx) & ((cu_width)-1))
#define GET_TRAV_Y(trav_idx, cu_width_log2)             ((trav_idx) >> (cu_width_log2))
#define IS_VALID_SP_CU_SIZE(cu_width, cu_height)        (((cu_width) < 4 || (cu_height) < 4 || (cu_width) > 32 || (cu_height) > 32) ? 0 : 1)
#define IS_VALID_CS2_CU_SIZE(cu_width, cu_height)       (((cu_width) < 4 || (cu_height) < 4 || (cu_width) > 32 || (cu_height) > 32) ? 0 : 1)
#endif
/* SP (END) */

/* ETMVP (START) */
#if ETMVP
#define MIN_ETMVP_SIZE                 8
#define MIN_ETMVP_MC_SIZE              8
#define MAX_ETMVP_NUM                  5
#endif
/* ETMVP (END) */

#if PMC || EPMC
#define IS_RIGHT_CBF_U(cbf_u)               ((cbf_u) > 0)
#define V_QP_OFFSET                         1   
#endif

#if AWP || SAWP
#define AWP_ANGLE_NUM                     (8)
#define AWP_RWFERENCE_SET_NUM             (7)
#define AWP_MODE_NUM                      (AWP_ANGLE_NUM * AWP_RWFERENCE_SET_NUM)
#define MIN_AWP_SIZE_LOG2                  3 /*  8 */
#define MAX_AWP_SIZE_LOG2                  6 /* 64 */
#define MIN_AWP_SIZE                      (1 << MIN_AWP_SIZE_LOG2)
#define MAX_AWP_SIZE                      (1 << MAX_AWP_SIZE_LOG2)
#define MAX_AWP_DIM                       (MAX_AWP_SIZE * MAX_AWP_SIZE)
#endif // AWP || SAWP

/* SBT (START) */
#if SBT
#define get_sbt_idx(s)                     (s & 0xf)
#define get_sbt_pos(s)                     ((s>>4) & 0xf)
#define get_sbt_info(idx, pos)             (idx + (pos << 4))
#define is_sbt_horizontal(idx)             (idx == 2 || idx == 4)
#define is_sbt_quad_size(idx)              (idx == 3 || idx == 4)
//encoder only
#define SBT_FAST                           1 // early skip fast algorithm
#define SBT_SAVELOAD                       1 // save & load
#endif
/* SBT (END) */

/* IC_AVS3 (START) */
#if IPC
#define NUM_RDO_WITH_IPC                    6        // number of additional RDO
#define IPC_MIN_BLK                         64                   
#define IPC_MAX_WD                          128       // this number is included
#define IPC_MAX_HT                          128       // this number is included
#define IPC_POS_MAX_LUMA                    16
#define IPC_POS_MAX_CHR                     8
#endif
/* IC_AVS3 (END) */

/* BVP (START) */
#if IBC_BVP
#define ALLOWED_HBVP_NUM                   12
#define MAX_NUM_BVP                        7
#define CBVP_TH_SIZE                       32
#define CBVP_TH_CNT                        2
#endif
/* BVP (END) */

/* ABVR (START) */
#if IBC_ABVR
#define MAX_NUM_BVR                        2 /* 1/4-pel */
#endif
/* ABVR (END) */


/* IBC (START) */
#if USE_IBC
#define IBC_SEARCH_RANGE                     64
#define IBC_NUM_CANDIDATES                   64
#define IBC_FAST_METHOD_BUFFERBV             0X01
#define IBC_FAST_METHOD_ADAPTIVE_SEARCHRANGE 0X02
#define IBC_BITSTREAM_FLAG_RESTRIC_LOG2      6 // restrict coded flag size
#define IBC_MAX_CU_LOG2                      4 /* max block size for ibc search in unit of log2 */
#define IBC_MAX_CAND_SIZE                    (1 << IBC_MAX_CU_LOG2) 
#endif
/* IBC (END) */
#if SBT
//set
#define MCU_SET_SBT_INFO(m,v)           (m)=((m & 0xFFFF00FF)|((v)&0xFF)<<8 )
//get
#define MCU_GET_SBT_INFO(m)             (int)(((m)>>8 )&0xFF)
#endif

#if USE_IBC
/* set ibc mode flag */
#define MCU_SET_IBC(m)                  (m)=((m)|(1<<25))
/* get ibc mode flag */
#define MCU_GET_IBC(m)                  (int)(((m)>>25) & 1)
/* clear ibc mode flag */
#define MCU_CLR_IBC(m)                  (m)=((m) & (~(1<<25)))
#endif
#if AWP
#define AWP_MV_LIST_LENGTH                 5
// encoder para
#define AWP_RDO_NUM                        7
#define CADIDATES_PER_PARTITION            2
#endif
#if AFFINE_UMVE
#define AFFINE_UMVE_BASE_NUM               AFF_MAX_NUM_MRG
#define AFFINE_UMVE_REFINE_STEP            5
#define AFFINE_UMVE_DIR                    4
#define AFFINE_UMVE_MAX_REFINE_NUM         (AFFINE_UMVE_REFINE_STEP * AFFINE_UMVE_DIR)
#endif

#if AWP_MVR
#define AWP_MVR_REFINE_STEP                5
#define AWP_MVR_DIR                        4
#define AWP_MVR_MAX_REFINE_NUM             (AWP_MVR_REFINE_STEP * AWP_MVR_DIR)
#endif
#if USE_SP
#define MSP_SET_SP_INFO(m)              (m)=(m | 0x80)
#define MSP_GET_SP_INFO(m)              (int)(((m)>>7) & 1)
#define MSP_CLR_SP_INFO(m)              (m)=(m & 0x7F)
#define MSP_SET_CS2_INFO(m)             (m)=((m)|0x40)
#define MSP_GET_CS2_INFO(m)             (int)(((m)>>6) & 1)
#define MSP_CLR_CS2_INFO(m)             (m)=(m & 0xBF)
#endif

#if ENHANCE_LT_MODE
#if PMC
#define IPD_MCPM_C                         9
#define IPD_MCPM_LT_C                      10
#define IPD_MCPM_L_C                       11
#define IPD_MCPM_T_C                       12
#endif
#if PMC && EPMC
#define IPD_EMCPM_C                        13
#define IPD_EMCPM_LT_C                     14
#define IPD_EMCPM_L_C                      15
#define IPD_EMCPM_T_C                      16
#define IPD_EMCPM2_C                       17
#define IPD_EMCPM2_LT_C                    18
#define IPD_EMCPM2_L_C                     19
#define IPD_EMCPM2_T_C                     20
#elif EPMC
#define IPD_EMCPM_C                        9
#define IPD_EMCPM_LT_C                     10
#define IPD_EMCPM_L_C                      11
#define IPD_EMCPM_T_C                      12
#define IPD_EMCPM2_C                       13
#define IPD_EMCPM2_LT_C                    14
#define IPD_EMCPM2_L_C                     15
#define IPD_EMCPM2_T_C                     16
#endif
#else
#if PMC
#define IPD_MCPM_C                         8
#define IPD_MCPM_L_C                       9
#define IPD_MCPM_T_C                       10
#endif
#if PMC && EPMC
#define IPD_EMCPM_C                         11
#define IPD_EMCPM_L_C                       12
#define IPD_EMCPM_T_C                       13 
#define IPD_EMCPM2_C                        14
#define IPD_EMCPM2_L_C                      15
#define IPD_EMCPM2_T_C                      16 
#elif EPMC
#define IPD_EMCPM_C                         8
#define IPD_EMCPM_L_C                       9
#define IPD_EMCPM_T_C                       10 
#define IPD_EMCPM2_C                        11
#define IPD_EMCPM2_L_C                      12
#define IPD_EMCPM2_T_C                      13 
#endif
#endif

#if USE_IBC || USE_SP
#define BVD_EXG_ORDER                        2
#endif

#if IIP
#define MAX_IIP_BLK                       4096 //the number is inlcuded
#define MIN_IIP_BLK                       64 //the number is included
#define STNUM                             5 // anchor is 3
#else
#define STNUM                             3 // anchor is 3 
#endif
#if SRCC
/* Scan-Region based coefficient coding (SRCC) (START) */
#define LOG2_CG_SIZE                       4

#define NUM_PREV_0VAL                      5
#define NUM_PREV_12VAL                     5

#define SCAN_REGION_GROUP                  14

#define NUM_CTX_SCANR_LUMA                 25
#define NUM_CTX_SCANR_CHROMA               3
#define NUM_CTX_SCANR                      (NUM_CTX_SCANR_LUMA + NUM_CTX_SCANR_CHROMA)

#define NUM_CTX_GT0_LUMA                   51
#define NUM_CTX_GT0_LUMA_TU                17

#define NUM_CTX_GT0_CHROMA                 9   /* number of context models for chroma gt0 flag */
#define NUM_CTX_GT0                        (NUM_CTX_GT0_LUMA + NUM_CTX_GT0_CHROMA)  /* number of context models for gt0 flag */

#define NUM_CTX_GT1_LUMA                   17
#define NUM_CTX_GT1_CHROMA                 5     
#define NUM_CTX_GT1                        (NUM_CTX_GT1_LUMA + NUM_CTX_GT1_CHROMA)  /* number of context models for gt1/2 flag */

/* Scan-Region based coefficient coding (SRCC) (END) */
#endif
#if UMVE_ENH
#define UMVE_REFINE_STEP_SEC_SET           8
#define UMVE_MAX_REFINE_NUM_SEC_SET        (UMVE_REFINE_STEP_SEC_SET * 4)
#endif

/* maximum CB count in a LCB */
#define MAX_CU_CNT_IN_LCU                  (MAX_CU_DIM/MIN_CU_DIM)
/* pixel position to SCB position */
#define PEL2SCU(pel)                       ((pel) >> MIN_CU_LOG2)

#define PIC_PAD_SIZE_L                     (MAX_CU_SIZE + 16)
#define PIC_PAD_SIZE_C                     (PIC_PAD_SIZE_L >> 1)

#define NUM_AVS2_SPATIAL_MV                3
#define NUM_SKIP_SPATIAL_MV                6
#define MVPRED_L                           0
#define MVPRED_U                           1
#define MVPRED_UR                          2
#define MVPRED_xy_MIN                      3

/* for GOP 16 test, increase to 32 */
/* maximum reference picture count. Originally, Max. 16 */
/* for GOP 16 test, increase to 32 */
#define MAX_NUM_REF_PICS                   17

#define MAX_NUM_ACTIVE_REF_FRAME           4

/* DPB Extra size */
#define EXTRA_FRAME                        MAX_NUM_REF_PICS

/* maximum picture buffer size */
#define MAX_PB_SIZE                       (MAX_NUM_REF_PICS + EXTRA_FRAME)

/* Neighboring block availability flag bits */
#define AVAIL_BIT_UP                       0
#define AVAIL_BIT_LE                       1
#define AVAIL_BIT_UP_LE                    2


/* Neighboring block availability flags */
#define AVAIL_UP                          (1 << AVAIL_BIT_UP)
#define AVAIL_LE                          (1 << AVAIL_BIT_LE)
#define AVAIL_UP_LE                       (1 << AVAIL_BIT_UP_LE)


/* MB availability check macro */
#define IS_AVAIL(avail, pos)            (((avail)&(pos)) == (pos))
/* MB availability set macro */
#define SET_AVAIL(avail, pos)             (avail) |= (pos)
/* MB availability remove macro */
#define REM_AVAIL(avail, pos)             (avail) &= (~(pos))
/* MB availability into bit flag */
#define GET_AVAIL_FLAG(avail, bit)      (((avail)>>(bit)) & 0x1)

#if BGC
/*
- [10:11] : bgc flag and bgc idx
*/
#define MCU_SET_BGC_FLAG(m)             (m)=((m)|(1<<10))
#define MCU_GET_BGC_FLAG(m)             (int)(((m)>>10) & 1)
#define MCU_CLR_BGC_FLAG(m)             (m)=((m) & (~(1<<10)))
#define MCU_SET_BGC_IDX(m)              (m)=((m)|(1<<11))
#define MCU_GET_BGC_IDX(m)              (int)(((m)>>11) & 1)
#define MCU_CLR_BGC_IDX(m)              (m)=((m) & (~(1<<11)))
#endif

/*****************************************************************************
 * slice type
 *****************************************************************************/
#define SLICE_I                            COM_ST_I
#define SLICE_P                            COM_ST_P
#define SLICE_B                            COM_ST_B

#define IS_INTRA_SLICE(slice_type)       ((slice_type) == SLICE_I))
#define IS_INTER_SLICE(slice_type)      (((slice_type) == SLICE_P) || ((slice_type) == SLICE_B))

/*****************************************************************************
 * prediction mode
 *****************************************************************************/
#define MODE_INTRA                         0
#define MODE_INTER                         1
#define MODE_SKIP                          2
#define MODE_DIR                           3

/*****************************************************************************
 * prediction direction
 *****************************************************************************/
enum PredDirection
{
    PRED_L0 = 0,
    PRED_L1 = 1,
    PRED_BI = 2,
    PRED_DIR_NUM = 3,
};

#if BD_AFFINE_AMVR
#define Tab_Affine_AMVR(x)                 ((x==0) ? 2 : ((x==1) ? 4 : 0) )
#endif

/*****************************************************************************
 * skip / direct mode
 *****************************************************************************/
#define TRADITIONAL_SKIP_NUM               (PRED_DIR_NUM + 1)
#define ALLOWED_HMVP_NUM                   8
#define MAX_SKIP_NUM                       (TRADITIONAL_SKIP_NUM + ALLOWED_HMVP_NUM)//12

#define UMVE_BASE_NUM                      2
#define UMVE_REFINE_STEP                   5
#define UMVE_MAX_REFINE_NUM                (UMVE_REFINE_STEP * 4)
#if UMVE_ENH
#define UMVE_REFINE_STEP_SEC_SET           8
#define UMVE_MAX_REFINE_NUM_SEC_SET        (UMVE_REFINE_STEP_SEC_SET * 4)
#endif

#define MAX_INTER_SKIP_RDO                 MAX_SKIP_NUM

#if EXT_AMVR_HMVP
#define SKIP_SMVD                          1
#define THRESHOLD_MVPS_CHECK               1.1
#endif

/*****************************************************************************
 * intra prediction direction
 *****************************************************************************/
#define IPD_DC                             0
#define IPD_PLN                            1  /* Luma, Planar */
#define IPD_BI                             2  /* Luma, Bilinear */

#define IPD_DM_C                           0  /* Chroma, DM*/
#define IPD_DC_C                           1  /* Chroma, DC */
#define IPD_HOR_C                          2  /* Chroma, Horizontal*/
#define IPD_VER_C                          3  /* Chroma, Vertical */
#define IPD_BI_C                           4  /* Chroma, Bilinear */
#if TSCPM
#define IPD_TSCPM_C                        5
#if ENHANCE_TSPCM || PMC || EPMC
#if ENHANCE_LT_MODE
#define IPD_TSCPM_LT_C                     6
#define IPD_TSCPM_L_C                      7
#define IPD_TSCPM_T_C                      8
#else
#define IPD_TSCPM_L_C                      6
#define IPD_TSCPM_T_C                      7
#endif
#endif
#endif

#define USE_EIPM_RMD                       1
#define USE_TSCPM                          1
#define SET_MD_64_MAX_COST                 0
#define SET_MD_32_MAX_COST                 0
#define SET_MD_16_MAX_COST                 0

#define IPD_CHROMA_CNT                     5

#define IPD_INVALID                       (-1)

#define IPD_RDO_CNT                        5

#define IPD_HOR                            24 /* Luma, Horizontal */
#define IPD_VER                            12 /* Luma, Vertical */

#define GET_REF_POS(mt,d_in,d_out,offset) \
    (d_out) = ((d_in) * (mt)) >> 10;\
    (offset) = ((((d_in) * (mt)) << 5) >> 10) - ((d_out) << 5);
#define ADI_4T_FILTER_BITS                 7
#define ADI_4T_FILTER_OFFSET              (1<<(ADI_4T_FILTER_BITS-1))
#if EIPM
#define IPRED_LIST_NUM_PREV                     5
#define STNUM_PREV                              3
#define IPD_CNT_PREV                            33
#define IPD_CNT                            66
#define IPD_HOR_EXT                        58
#define IPD_VER_EXT                        43
#define IPD_DIA_R_EXT                      50
#define IPD_DIA_L_EXT                      34
#define STNUM                              5

#if USE_EIPM_RMD
#if USE_SPEED_LEVEL
#define IPRED_LIST_NUM                     (IPD_RDO_CNT + EXTRA_RDO_MODE)
#else
#define IPRED_LIST_NUM                     5
#endif

#else
#define IPRED_LIST_NUM                     7
#endif


void ipred_ang_eipm_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], int w, int h, int ipm
#if MIPF
	, int is_luma, int mipf_enable_flag, U4 bit_depth
#endif
);

void ipred_ang_eipm_64(pel* src_le, pel* src_up, int w, int h, int ipm
#if MIPF
	, int is_luma, int mipf_enable_flag, U4 bit_depth, s16 diff[64][64], pel pred_for_diff[64][64], pel* orgY
#endif
);
void ipred_ang_eipm_32(pel src_le[67], pel src_up[67], int w, int h, int ipm
#if MIPF
	, int is_luma, int mipf_enable_flag, U4 bit_depth, s16 diff[32][32]
#endif
);
void ipred_ang_eipm_Chroma16(pel src_le[35], pel src_up[35], int w, int h, int ipm
#if MIPF
	, int is_luma, int mipf_enable_flag, U4 bit_depth, s16 diff[16][16], pel pred[16][16]
#endif
);
void ipred_ang_eipm_Chroma8(pel src_le[19], pel src_up[19], int w, int h, int ipm
#if MIPF
	, int is_luma, int mipf_enable_flag, U4 bit_depth, s16 pred_1[8][8], pel pred_for_diff[8][8]
#endif
);
#else
#define IPRED_LIST_NUM                     5
#define STNUM                              3
#define IPD_CNT                            33
#define IPRED_LIST_NUM_PREV                     IPRED_LIST_NUM
#define STNUM_PREV                              STNUM
#define IPD_CNT_PREV                            IPD_CNT
#endif
#define IPD_IPCM                           33

#define IPD_DIA_R                         18 /* Luma, Right diagonal */
#define IPD_DIA_L                         3  /* Luma, Left diagonal */
#define IPD_DIA_U                         32 /* Luma, up diagonal */

/*****************************************************************************
* Transform
*****************************************************************************/


/*****************************************************************************
 * reference index
 *****************************************************************************/
#define REFI_INVALID                      (-1)
#define REFI_IS_VALID(refi)               ((refi) >= 0)
#define SET_REFI(refi, idx0, idx1)        (refi)[REFP_0] = (idx0); (refi)[REFP_1] = (idx1)

/*****************************************************************************
 * macros for CU map

 - [ 0: 6] : slice number (0 ~ 128)
 - [ 7:14] : reserved
 - [15:15] : 1 -> intra CU, 0 -> inter CU
 - [16:22] : QP
 - [23:23] : skip mode flag
 - [24:24] : luma cbf
 - [25:30] : reserved
 - [31:31] : 0 -> no encoded/decoded CU, 1 -> encoded/decoded CU
 *****************************************************************************/
/* set intra CU flag to map */
#define MCU_SET_INTRA_FLAG(m)           (m)=((m)|(1<<15))
/* get intra CU flag from map */
#define MCU_GET_INTRA_FLAG(m)           (int)(((m)>>15) & 1)
/* clear intra CU flag in map */
#define MCU_CLEAR_INTRA_FLAG(m)         (m)=((m) & 0xFFFF7FFF)

/* set QP to map */
#if CUDQP_QP_MAP
#define MCU_SET_QP(m, qp)               (m)=((m&0xFF80FFFF)|((qp)&0x7F)<<16)
#else
#define MCU_SET_QP(m, qp)               (m)=((m)|((qp)&0x7F)<<16)
#endif
/* get QP from map */
#define MCU_GET_QP(m)                   (int)(((m)>>16)&0x7F)

/* set skip mode flag */
#define MCU_SET_SF(m)                   (m)=((m)|(1<<23))
/* get skip mode flag */
#define MCU_GET_SF(m)                   (int)(((m)>>23) & 1)
/* clear skip mode flag */
#define MCU_CLR_SF(m)                   (m)=((m) & (~(1<<23)))

/* set cu cbf flag */
#define MCU_SET_CBF(m)                  (m)=((m)|(1<<24))
/* get cu cbf flag */
#define MCU_GET_CBF(m)                  (int)(((m)>>24) & 1)
/* clear cu cbf flag */
#define MCU_CLR_CBF(m)                  (m)=((m) & (~(1<<24)))

/* set encoded/decoded flag of CU to map */
#define MCU_SET_CODED_FLAG(m)           (m)=((m)|(1<<31))
/* get encoded/decoded flag of CU from map */
#define MCU_GET_CODED_FLAG(m)           (int)(((m)>>31) & 1)
/* clear encoded/decoded CU flag to map */
#define MCU_CLR_CODED_FLAG(m)           (m)=((m) & 0x7FFFFFFF)

/* multi bit setting: intra flag, encoded/decoded flag, slice number */
#define MCU_SET_IF_COD_SN_QP(m, i, sn, qp) \
    (m) = (((m)&0xFF807F80)|((sn)&0x7F)|((qp)<<16)|((i)<<15)|(1<<31))

#define MCU_IS_COD_NIF(m)               ((((m)>>15) & 0x10001) == 0x10000)

#define MCU_DEFAULT_MAP_SCU(qp) ((qp)<<16)

/*
- [8:9] : affine vertex number, 00: 1(trans); 01: 2(affine); 10: 3(affine); 11: 4(affine)
*/

/* set affine CU mode to map */
#define MCU_SET_AFF(m, v)               (m)=((m & 0xFFFFFCFF)|((v)&0x03)<<8)
/* get affine CU mode from map */
#define MCU_GET_AFF(m)                  (int)(((m)>>8)&0x03)
/* clear affine CU mode to map */
#define MCU_CLR_AFF(m)                  (m)=((m) & 0xFFFFFCFF)

/* set x scu offset & y scu offset relative to top-left scu of CU to map */
#define MCU_SET_X_SCU_OFF(m, v)         (m)=((m & 0xFFFF00FF)|((v)&0xFF)<<8)
#define MCU_SET_Y_SCU_OFF(m, v)         (m)=((m & 0xFF00FFFF)|((v)&0xFF)<<16)
/* get x scu offset & y scu offset relative to top-left scu of CU in map */
#define MCU_GET_X_SCU_OFF(m)            (int)(((m)>>8)&0xFF)
#define MCU_GET_Y_SCU_OFF(m)            (int)(((m)>>16)&0xFF)

/* set cu_width_log2 & cu_height_log2 to map */
#define MCU_SET_LOGW(m, v)              (m)=((m & 0xF0FFFFFF)|((v)&0x0F)<<24)
#define MCU_SET_LOGH(m, v)              (m)=((m & 0x0FFFFFFF)|((v)&0x0F)<<28)
/* get cu_width_log2 & cu_height_log2 to map */
#define MCU_GET_LOGW(m)                 (int)(((m)>>24)&0x0F)
#define MCU_GET_LOGH(m)                 (int)(((m)>>28)&0x0F)
#if SAWP_MPM_SIMP
#define SAWP_MPM_NUM                       2
#else // SAWP_MPM_SIMP
#define SAWP_MPM_NUM                       4
#endif // SAWP_MPM_SIMP
#if TB_SPLIT_EXT
//set
#define MCU_SET_TB_PART_LUMA(m,v)       (m)=((m & 0xFFFFFF00)|((v)&0xFF)<<0 )
//get
#define MCU_GET_TB_PART_LUMA(m)         (int)(((m)>>0 )&0xFF)
#endif

typedef u32 SBAC_CTX_MODEL;
#define NUM_POSITION_CTX                   2
#define NUM_SBAC_CTX_AFFINE_MVD_FLAG       2

#if SEP_CONTEXT
#define NUM_SBAC_CTX_SKIP_FLAG             4
#else
#define NUM_SBAC_CTX_SKIP_FLAG             3
#endif
#define NUM_SBAC_CTX_SKIP_IDX              (MAX_SKIP_NUM - 1)//11
#if SEP_CONTEXT
#define NUM_SBAC_CTX_DIRECT_FLAG           2
#else
#define NUM_SBAC_CTX_DIRECT_FLAG           1
#endif

#define NUM_SBAC_CTX_UMVE_BASE_IDX         1
#define NUM_SBAC_CTX_UMVE_STEP_IDX         1
#define NUM_SBAC_CTX_UMVE_DIR_IDX          2

#if SEP_CONTEXT
#define NUM_SBAC_CTX_SPLIT_FLAG            4
#else
#define NUM_SBAC_CTX_SPLIT_FLAG            3
#endif
#define NUM_SBAC_CTX_SPLIT_MODE            3
#define NUM_SBAC_CTX_BT_SPLIT_FLAG         9
#if SEP_CONTEXT
#define NUM_SBAC_CTX_SPLIT_DIR             5
#else
#define NUM_SBAC_CTX_SPLIT_DIR             3
#endif
#define NUM_QT_CBF_CTX                     3       /* number of context models for QT CBF */
#if SEP_CONTEXT
#define NUM_CTP_ZERO_FLAG_CTX              2       /* number of context models for ctp_zero_flag */
#define NUM_PRED_MODE_CTX                  6       /* number of context models for prediction mode */
#else
#define NUM_CTP_ZERO_FLAG_CTX              1       /* number of context models for ctp_zero_flag */
#define NUM_PRED_MODE_CTX                  5       /* number of context models for prediction mode */
#endif
#if MODE_CONS
#define NUM_CONS_MODE_CTX                  1       /* number of context models for constrained prediction mode */
#endif
#define NUM_TB_SPLIT_CTX                   1

#define NUM_SBAC_CTX_DELTA_QP              4

#if SEP_CONTEXT
#define NUM_INTER_DIR_CTX                  3       /* number of context models for inter prediction direction */
#else
#define NUM_INTER_DIR_CTX                  2       /* number of context models for inter prediction direction */
#endif
#define NUM_REFI_CTX                       3

#define NUM_MVR_IDX_CTX                   (MAX_NUM_MVR - 1)//4
#if BD_AFFINE_AMVR
#define NUM_AFFINE_MVR_IDX_CTX            (MAX_NUM_AFFINE_MVR - 1)//2
#endif

#if EXT_AMVR_HMVP
#define NUM_EXTEND_AMVR_FLAG               1
#endif
#define NUM_MV_RES_CTX                     3       /* number of context models for motion vector difference */

#if PMC && EPMC 
#define NUM_INTRA_DIR_CTX                  14
#elif PMC || EPMC
#define NUM_INTRA_DIR_CTX                  13
#else
#if EIPM
#if TSCPM
#if ENHANCE_TSPCM
#define NUM_INTRA_DIR_CTX                  12
#else
#define NUM_INTRA_DIR_CTX                  11
#endif
#else
#define NUM_INTRA_DIR_CTX                  10
#endif
#else
#if TSCPM
#if ENHANCE_TSPCM
#define NUM_INTRA_DIR_CTX                  11
#else
#define NUM_INTRA_DIR_CTX                  10
#endif
#else
#define NUM_INTRA_DIR_CTX                  9
#endif
#endif
#endif

#define NUM_SBAC_CTX_AFFINE_FLAG           1
#define NUM_SBAC_CTX_AFFINE_MRG            (AFF_MAX_NUM_MRG - 1)//4

#if SMVD
#define NUM_SBAC_CTX_SMVD_FLAG             1
#endif

#if DT_SYNTAX
#define NUM_SBAC_CTX_PART_SIZE             6
#endif

#define NUM_SAO_MERGE_FLAG_CTX             3
#define NUM_SAO_MODE_CTX                   1
#define NUM_SAO_OFFSET_CTX                 1

#define NUM_ALF_COEFF_CTX                  1
#define NUM_ALF_LCU_CTX                    1

#define RANK_NUM 6
#define NUM_SBAC_CTX_LEVEL                 (RANK_NUM * 4)//24
#define NUM_SBAC_CTX_RUN                   (RANK_NUM * 4)

#define NUM_SBAC_CTX_RUN_RDOQ              (RANK_NUM * 4)


#define NUM_SBAC_CTX_LAST1                 RANK_NUM//6
#define NUM_SBAC_CTX_LAST2                 12

#define NUM_SBAC_CTX_LAST                  2


#if IIP
#define NUM_IIP_CTX                        1
#endif

#if INTERPF
#define NUM_SBAC_CTX_INTERPF               2
#endif
#if IPC
#define NUM_SBAC_CTX_IPC                   3
#endif
#if USE_IBC
#define NUM_SBAC_CTX_IBC_FLAG              3
#endif
#if USE_SP
#define NUM_SOIF_CTX                       1 ///< number of context models for SP or IBC flag coding
#define NUM_GSF_CTX                        1 ///< number of context models for SP flag coding
#define NUM_SP_CDF_CTX                     1 ///< number of context models for SP copy direction flag
#define NUM_SP_IMF_CTX                     1 ///< number of context models for SP is matched flag 
#define NUM_SP_STRLEN_CTX                  3 ///< number of context models for SP matched length
#define NUM_SP_ABO_CTX                     1 ///< number of context models for SP is above offset flag
#define NUM_SP_OYZ_CTX                     1 ///< number of context models for SP is offset Y Zero flag
#define NUM_SP_OXZ_CTX                     1 ///< number of context models for SP is offset X Zero flag
#define NUM_SP_NRF_CTX                     1
#define NUM_SP_NID_CTX                     3
#define NUM_SP_PIMF_CTX                    1 ///< number of context models for SP pixel is matched flag 
#define NUM_CS2F_CTX                       1 ///< number of context models for CS2 flag coding
#define NUM_SP_MODE_CTX                    1
#define NUM_SP_STRING_TYPE_CTX             1
#define NUM_SP_STRING_SCAN_MODE_CTX        1
#define NUM_SP_LOREF_MAX_LENGTH_CTX        4
#define NUM_SP_SRB_FLAG_CTX                1
#define NUM_SP_SRB_LOREFFLAG_CTX           1
#define NUM_SP_SRB_TOPRUN_CTX              3
#define NUM_SP_SRB_LEFTRUN_CTX             5
#endif

#if IBC_ABVR
#define NUM_BVR_IDX_CTX                   (MAX_NUM_BVR - 1)
#endif
#if IBC_BVP
#define NUM_BVP_IDX_CTX                   (MAX_NUM_BVP - 1)
#endif
#if USE_IBC
#define NUM_BV_RES_CTX                     8       /* number of context models for block vector difference */
#endif
#if USE_SP
#define NUM_SV_RES_CTX                     7       /* number of context models for string vector difference */
#endif
#if CUDQP
#define NUM_SBAC_CTX_CU_QP_DELTA_ABS       4
#endif
#if ETS
#define NUM_ETS_IDX_CTX                    1
#endif

#if IBC_TS
#define NUM_TS_IDX_CTX                     1
#endif

#if EST
#define NUM_EST_IDX_CTX                    1
#endif

#if ST_CHROMA
#define NUM_ST_CHROMA_IDX_CTX              1
#endif
#if SBT
#define NUM_SBAC_CTX_SBT_INFO              7
#endif
#if ESAO
#define NUM_ESAO_LCU_CTX                   1  
#define NUM_ESAO_OFFSET_CTX                1 
#define NUM_ESAO_UV_COUNT_CTX              1 
#endif

#if CCSAO
#define NUM_CCSAO_LCU_CTX                  1
#define NUM_CCSAO_OFFSET_CTX               1 
#endif

#if NN_FILTER
#define NUM_NNLF_LCU_CTX                   3
#define NUM_NNLF_SET_CTX                   9
#endif
#if ETMVP
#define NUM_SBAC_CTX_ETMVP_IDX             (MAX_ETMVP_NUM - 1)
#define NUM_SBAC_CTX_ETMVP_FLAG            1
#endif
#if AWP
#define NUM_SBAC_CTX_AWP_FLAG              1
#if FIX_316
#define NUM_SBAC_CTX_AWP_IDX               3
#else
#define NUM_SBAC_CTX_AWP_IDX               2
#endif
#endif

#if SAWP
#define NUM_SBAC_CTX_SAWP_FLAG              1
#endif // SAWP
/* context models for arithemetic coding */
typedef struct _COM_SBAC_CTX
{
	SBAC_CTX_MODEL   skip_flag[NUM_SBAC_CTX_SKIP_FLAG];//4: 0~3
#if USE_IBC
	SBAC_CTX_MODEL   ibc_flag[NUM_SBAC_CTX_IBC_FLAG];//3: 4~6
#endif
#if USE_SP
	SBAC_CTX_MODEL   sp_or_ibc_flag[NUM_SOIF_CTX];//1: 7
	SBAC_CTX_MODEL   sp_flag[NUM_GSF_CTX];//1: 8
	SBAC_CTX_MODEL   sp_copy_direct_flag[NUM_SP_CDF_CTX];//1: 9
	SBAC_CTX_MODEL   sp_is_matched_flag[NUM_SP_IMF_CTX];//1: 10
	SBAC_CTX_MODEL   sp_string_length[NUM_SP_STRLEN_CTX];//3: 11~13
	SBAC_CTX_MODEL   sp_above_offset[NUM_SP_ABO_CTX];//1: 14
	SBAC_CTX_MODEL   sp_offset_y_zero[NUM_SP_OYZ_CTX];//1: 15
	SBAC_CTX_MODEL   sp_offset_x_zero[NUM_SP_OXZ_CTX];//1: 16
	SBAC_CTX_MODEL   sp_n_recent_flag[NUM_SP_NRF_CTX];//1: 17
	SBAC_CTX_MODEL   sp_n_index[NUM_SP_NID_CTX];//3: 18~20
	SBAC_CTX_MODEL   sp_pixel_is_matched_flag[NUM_SP_PIMF_CTX];//1: 21
	SBAC_CTX_MODEL   sp_cs2_flag[NUM_CS2F_CTX];//1: 22
	SBAC_CTX_MODEL   sp_mode_context[NUM_SP_MODE_CTX];//1: 23
	SBAC_CTX_MODEL   sp_str_type_context[NUM_SP_STRING_TYPE_CTX];//1: 24
	SBAC_CTX_MODEL   sp_str_scanmode_context[NUM_SP_STRING_SCAN_MODE_CTX];//1: 25
	SBAC_CTX_MODEL   sp_lo_ref_maxlength_context[NUM_SP_LOREF_MAX_LENGTH_CTX];//4: 26~29
	//For SRB
	SBAC_CTX_MODEL   sp_evs_present_flag_context[NUM_SP_SRB_FLAG_CTX];//1: 30
	SBAC_CTX_MODEL   sp_SRB_lo_ref_color_flag_context[NUM_SP_SRB_LOREFFLAG_CTX];//1: 31
	SBAC_CTX_MODEL   sp_SRB_copy_toprun_context[NUM_SP_SRB_TOPRUN_CTX];//3: 32~34
#endif
	SBAC_CTX_MODEL   skip_idx_ctx[NUM_SBAC_CTX_SKIP_IDX];//11: 35~45
	SBAC_CTX_MODEL   direct_flag[NUM_SBAC_CTX_DIRECT_FLAG];//2: 46~47
#if INTERPF
	SBAC_CTX_MODEL   inter_filter_flag[NUM_SBAC_CTX_INTERPF];//2: 48~49
#endif
#if IPC
	SBAC_CTX_MODEL   ipc_flag[NUM_SBAC_CTX_IPC];//3: 50~52
#endif
#if BGC
	SBAC_CTX_MODEL   bgc_flag;//1: 53
	SBAC_CTX_MODEL   bgc_idx;//1: 54
#endif
#if AWP
	SBAC_CTX_MODEL   umve_awp_flag;//1: 55
#else
	SBAC_CTX_MODEL   umve_flag;
#endif
	SBAC_CTX_MODEL   umve_base_idx[NUM_SBAC_CTX_UMVE_BASE_IDX];//1: 56
	SBAC_CTX_MODEL   umve_step_idx[NUM_SBAC_CTX_UMVE_STEP_IDX];//1: 57
	SBAC_CTX_MODEL   umve_dir_idx[NUM_SBAC_CTX_UMVE_DIR_IDX];//2: 58~59
#if AFFINE_UMVE
	SBAC_CTX_MODEL   affine_umve_flag;//1: 60
	SBAC_CTX_MODEL   affine_umve_step_idx[NUM_SBAC_CTX_UMVE_STEP_IDX];//1: 61
	SBAC_CTX_MODEL   affine_umve_dir_idx[NUM_SBAC_CTX_UMVE_DIR_IDX];//2: 62~63
#endif
	SBAC_CTX_MODEL   inter_dir[NUM_INTER_DIR_CTX];//3: 64~66
	SBAC_CTX_MODEL   intra_dir[NUM_INTRA_DIR_CTX];//14: 67~80
	SBAC_CTX_MODEL   pred_mode[NUM_PRED_MODE_CTX];//6: 81~86
#if MODE_CONS
	SBAC_CTX_MODEL   cons_mode[NUM_CONS_MODE_CTX];//1: 87
#endif
#if IIP
	SBAC_CTX_MODEL   iip_flag[NUM_IIP_CTX];//1: 88
#endif
	SBAC_CTX_MODEL   ipf_flag[NUM_IPF_CTX];//1: 89
	SBAC_CTX_MODEL   refi[NUM_REFI_CTX];//3: 90~92
	SBAC_CTX_MODEL   mvr_idx[NUM_MVR_IDX_CTX];//4: 93~96
#if IBC_ABVR
	SBAC_CTX_MODEL   bvr_idx[NUM_BVR_IDX_CTX];//1: 97
#endif
#if BD_AFFINE_AMVR
	SBAC_CTX_MODEL   affine_mvr_idx[NUM_AFFINE_MVR_IDX_CTX];//2: 98~99
#endif
#if EXT_AMVR_HMVP
	SBAC_CTX_MODEL   mvp_from_hmvp_flag[NUM_EXTEND_AMVR_FLAG];//1: 100
#endif
#if IBC_BVP
	SBAC_CTX_MODEL   cbvp_idx[NUM_BVP_IDX_CTX];//6: 101~106
#endif
#if USE_IBC
	SBAC_CTX_MODEL   bvd[2][NUM_BV_RES_CTX];//16: 107~122
#endif
#if USE_SP
	SBAC_CTX_MODEL   svd[2][NUM_SV_RES_CTX];//14: 123~136
#endif
	SBAC_CTX_MODEL   mvd[2][NUM_MV_RES_CTX];//6: 137~142
	SBAC_CTX_MODEL   ctp_zero_flag[NUM_CTP_ZERO_FLAG_CTX];//2: 143~144
	SBAC_CTX_MODEL   cbf[NUM_QT_CBF_CTX];//3: 145~147
#if CUDQP
	SBAC_CTX_MODEL   cu_qp_delta_abs[NUM_SBAC_CTX_CU_QP_DELTA_ABS];//4: 148~151
#endif
	SBAC_CTX_MODEL   tb_split[NUM_TB_SPLIT_CTX];//1: 152
#if SBT
	SBAC_CTX_MODEL   sbt_info[NUM_SBAC_CTX_SBT_INFO];//7: 153~159
#endif
#if SRCC
	SBAC_CTX_MODEL   cc_gt0[NUM_CTX_GT0];//60: 160~219
	SBAC_CTX_MODEL   cc_gt1[NUM_CTX_GT1];//22: 220~241
	SBAC_CTX_MODEL   cc_scanr_x[NUM_CTX_SCANR];//28: 242~269
	SBAC_CTX_MODEL   cc_scanr_y[NUM_CTX_SCANR];//28: 270~297
#endif
	SBAC_CTX_MODEL   run[NUM_SBAC_CTX_RUN];//24: 298~321
	SBAC_CTX_MODEL   run_rdoq[NUM_SBAC_CTX_RUN];//24: 322~345
	SBAC_CTX_MODEL   last1[NUM_SBAC_CTX_LAST1 * 2];//12: 346~357
	SBAC_CTX_MODEL   last2[NUM_SBAC_CTX_LAST2 * 2 - 2];//22: 358~379
	SBAC_CTX_MODEL   level[NUM_SBAC_CTX_LEVEL];//24: 380~403
	SBAC_CTX_MODEL   split_flag[NUM_SBAC_CTX_SPLIT_FLAG];//4: 404~407
	SBAC_CTX_MODEL   bt_split_flag[NUM_SBAC_CTX_BT_SPLIT_FLAG];//9: 408~416
	SBAC_CTX_MODEL   split_dir[NUM_SBAC_CTX_SPLIT_DIR];//5: 417~421
	SBAC_CTX_MODEL   split_mode[NUM_SBAC_CTX_SPLIT_MODE];//3: 422~424
	SBAC_CTX_MODEL   affine_flag[NUM_SBAC_CTX_AFFINE_FLAG];//1: 425
	SBAC_CTX_MODEL   affine_mrg_idx[NUM_SBAC_CTX_AFFINE_MRG];//4: 426~429
#if ETMVP
	SBAC_CTX_MODEL   etmvp_flag[NUM_SBAC_CTX_ETMVP_FLAG];//1: 430
	SBAC_CTX_MODEL   etmvp_idx[NUM_SBAC_CTX_ETMVP_IDX];//4: 431~434
#endif
#if AWP
	SBAC_CTX_MODEL   awp_flag[NUM_SBAC_CTX_AWP_FLAG];//1: 435
	SBAC_CTX_MODEL   awp_idx[NUM_SBAC_CTX_AWP_IDX];//3: 436~438
#endif
#if SAWP
	SBAC_CTX_MODEL   sawp_flag[NUM_SBAC_CTX_SAWP_FLAG];//1: 439
#endif // SAWP

#if AWP_MVR
	SBAC_CTX_MODEL   awp_mvr_flag;//1: 440
	SBAC_CTX_MODEL   awp_mvr_step_idx[NUM_SBAC_CTX_UMVE_STEP_IDX];//1: 441
	SBAC_CTX_MODEL   awp_mvr_dir_idx[NUM_SBAC_CTX_UMVE_DIR_IDX];//2: 442~443
#endif
#if SMVD
	SBAC_CTX_MODEL   smvd_flag[NUM_SBAC_CTX_SMVD_FLAG];//1: 444
#endif
#if DT_SYNTAX
	SBAC_CTX_MODEL   part_size[NUM_SBAC_CTX_PART_SIZE];//6: 445~450
#endif
#if ETS
	SBAC_CTX_MODEL   ets_flag[NUM_ETS_IDX_CTX];//1: 451
#endif
#if IBC_TS
	SBAC_CTX_MODEL   ts_flag[NUM_TS_IDX_CTX];//1: 452
#endif
#if EST
	SBAC_CTX_MODEL   est_flag[NUM_EST_IDX_CTX];//1: 453
#endif
#if ST_CHROMA
	SBAC_CTX_MODEL   st_chroma_flag[NUM_ST_CHROMA_IDX_CTX];//1: 454
#endif

	SBAC_CTX_MODEL   sao_merge_flag[NUM_SAO_MERGE_FLAG_CTX];//3: 455~457
	SBAC_CTX_MODEL   sao_mode[NUM_SAO_MODE_CTX];//1: 458
	SBAC_CTX_MODEL   sao_offset[NUM_SAO_OFFSET_CTX];//1: 459
	SBAC_CTX_MODEL   alf_lcu_enable[NUM_ALF_LCU_CTX];//1: 460
	SBAC_CTX_MODEL   delta_qp[NUM_SBAC_CTX_DELTA_QP];//4: 461~464
#if ESAO
	SBAC_CTX_MODEL   esao_lcu_enable[NUM_ESAO_LCU_CTX];//1: 465
	SBAC_CTX_MODEL   esao_offset[NUM_ESAO_OFFSET_CTX];//1: 466
	SBAC_CTX_MODEL   esao_chroma_mode_flag[NUM_ESAO_UV_COUNT_CTX];//1: 467
#endif 
#if CCSAO
	SBAC_CTX_MODEL   ccsao_lcu_flag[NUM_CCSAO_LCU_CTX];//1: 468
	SBAC_CTX_MODEL   ccsao_offset[NUM_CCSAO_OFFSET_CTX];//1: 469
#endif
#if NN_FILTER
	SBAC_CTX_MODEL   nnlf_lcu_enable[NUM_NNLF_LCU_CTX];//3: 470~472
	SBAC_CTX_MODEL   nnlf_lcu_set[NUM_NNLF_SET_CTX];//9: 473~481
#endif

} COM_SBAC_CTX;

typedef struct _COM_SBAC_CTX_SKIP
{
	SBAC_CTX_MODEL   skip_flag[NUM_SBAC_CTX_SKIP_FLAG];//4
	SBAC_CTX_MODEL   skip_idx_ctx[NUM_SBAC_CTX_SKIP_IDX];//10
	SBAC_CTX_MODEL   umve_flag;
	SBAC_CTX_MODEL   umve_base_idx[NUM_SBAC_CTX_UMVE_BASE_IDX];//1
	SBAC_CTX_MODEL   umve_step_idx[NUM_SBAC_CTX_UMVE_STEP_IDX];//1
	SBAC_CTX_MODEL   umve_dir_idx[NUM_SBAC_CTX_UMVE_DIR_IDX];//2
	SBAC_CTX_MODEL   affine_flag[NUM_SBAC_CTX_AFFINE_FLAG];//1
	SBAC_CTX_MODEL   affine_mrg_idx[NUM_SBAC_CTX_AFFINE_MRG];//4
} COM_SBAC_CTX_SKIP;

#define COEF_SCAN_ZIGZAG                    0
#define COEF_SCAN_TYPE_NUM                  1

/* Maximum transform dynamic range (excluding sign bit) */
#define MAX_TX_DYNAMIC_RANGE               15

#define QUANT_SHIFT                        14

typedef struct _CU_POS{
	int scup;
	int x_pos;
	int y_pos;
	int cu_width;
	int cu_height;
} CU_POS;
typedef struct _SQH_SKIP{
	u8 affine_enable_flag;
	int num_of_hmvp_cand;
	int umve_enable_flag;
} SQH_SKIP;
typedef struct _PIC_INFO_SKIP{
	int pic_width;
	int pic_height;
} PIC_INFO_SKIP;
typedef struct _SKIP_ELEMENT{
	BOOL valid;
	u8 skip_flag;
	u8 skip_idx;
	u8 umve_flag;
	u8 umve_idx;
	u8 affine_flag;
	s32 mvp[REFP_NUM][MV_D];
	s32 refi[REFP_NUM];
} SKIP_ELEMENT;

typedef struct
{
	BOOL valid;
	u8 skip_flag;
	u8 skip_idx;
	u8 umve_flag;
	u8 umve_idx;
	u8 affine_flag;
	s16 mvp[MV_D];
	s8 refi;
	u8 mv_idx;
	s16 mvd[MV_D];
	int x, y;
	u8 dx, dy;
} SKIP_MODE_INTER;

/* neighbor CUs
   neighbor position:

   D     B     C

   A     X,<G>

   E          <F>
*/
#define MAX_NEB                            5
#define NEB_A                              0  /* left */
#define NEB_B                              1  /* up */
#define NEB_C                              2  /* up-right */
#define NEB_D                              3  /* up-left */
#define NEB_E                              4  /* low-left */

#define NEB_F                              5  /* co-located of low-right */
#define NEB_G                              6  /* co-located of X */
#define NEB_X                              7  /* center (current block) */
#define NEB_H                              8  /* right */
#define NEB_I                              9  /* low-right */
#define MAX_NEB2                           10

/* SAO_AVS2(START) */
enum SAOMode   //mode
{
    SAO_MODE_OFF = 0,
    SAO_MODE_MERGE,
    SAO_MODE_NEW,
    NUM_SAO_MODES
};

enum SAOModeMergeTypes
{
    SAO_MERGE_LEFT = 0,
    SAO_MERGE_ABOVE,
    NUM_SAO_MERGE_TYPES
};

enum SAOModeNewTypes   //NEW: types
{
    SAO_TYPE_EO_0,
    SAO_TYPE_EO_90,
    SAO_TYPE_EO_135,
    SAO_TYPE_EO_45,
    SAO_TYPE_BO,
    NUM_SAO_NEW_TYPES
};

enum SAOEOClasses   // EO Groups, the assignments depended on how you implement the edgeType calculation
{
    SAO_CLASS_EO_FULL_VALLEY = 0,
    SAO_CLASS_EO_HALF_VALLEY = 1,
    SAO_CLASS_EO_PLAIN = 2,
    SAO_CLASS_EO_HALF_PEAK = 3,
    SAO_CLASS_EO_FULL_PEAK = 4,
    SAO_CLASS_BO = 5,
    NUM_SAO_EO_CLASSES = SAO_CLASS_BO,
    NUM_SAO_OFFSET
};

struct SAOstatdata
{
    long int diff[MAX_NUM_SAO_CLASSES];        // liaoax, 2021-01-29: long long -> long.   (pixed_diff(256)* 64*64) =avs3_max= 2^20
    int count[MAX_NUM_SAO_CLASSES];
};

typedef struct SAOstatdata SAOStatData;
typedef struct
{
    int modeIdc; //NEW, MERGE, OFF
    int typeIdc; //NEW: EO_0, EO_90, EO_135, EO_45, BO. MERGE: left, above
    int startBand; //BO: starting band index
    int startBand2;
    int deltaband;
    int offset[MAX_NUM_SAO_CLASSES];
} SAOBlkParam;
/* SAO_AVS2(END) */
#if DBR
typedef struct _DBR_PARAM
{
	int dbr_horizontal_enabled;
	int thresh_horizontal_index;
#if EDBR
	int horizontal_offsets[10];
#else
	int horizontal_offsets[6];
#endif
	int dbr_vertical_enabled;
	int thresh_vertical_index;
#if EDBR
	int dbr_fs0_horizontal_enabled;
	int dbr_fs0_vertical_enabled;
	int vertical_offsets[10];
#else
	int vertical_offsets[6];
#endif
} DBR_PARAM;
#endif

#if ESAO
/* ESAO_AVS3(START) */

#if ESAO_ENH
typedef struct
{
	long long diff[ESAO_SET_NUM][ESAO_LABEL_CLASSES_MAX];
	int       count[ESAO_SET_NUM][ESAO_LABEL_CLASSES_MAX];
}ESAO_REFINE_DATA;

typedef struct
{
	int       lcu_pos;
	double    lcu_cost;
}ESAO_LCU_COST;

typedef struct
{
	BOOL      en;
	int       set;
	int       cnt;
}ESAO_SET_CNT;
#endif

typedef struct _ESAO_STAT_DATA
{
	long long diff[ESAO_LABEL_CLASSES_MAX];
	int count[ESAO_LABEL_CLASSES_MAX];
} ESAO_STAT_DATA;

typedef struct _ESAO_BLK_PARAM
{
#if ESAO_ENH
	int set_num;
	int offset[ESAO_SET_NUM][ESAO_LABEL_CLASSES_MAX];
	int* lcu_flag;
	int mode[ESAO_SET_NUM];
	int luma_type[ESAO_SET_NUM];
	int chroma_band_length[ESAO_SET_NUM];
	int chroma_start_band[ESAO_SET_NUM];
	int chroma_band_flag[ESAO_SET_NUM];
#else
	int  offset[ESAO_LABEL_CLASSES_MAX];
	int* lcu_flag;
#endif
}ESAO_BLK_PARAM;

typedef struct _ESAO_FUNC_POINTER
{
	void (*esao_on_block_for_luma)(pel* p_dst, int i_dst, pel* p_src, int i_src, int i_block_w, int i_block_h, int bit_depth, int bo_value, const int* esao_offset, int lcu_available_left, int lcu_available_right,
		int lcu_available_up, int lcu_available_down, int lcu_available_upleft, int lcu_available_upright, int lcu_available_leftdown, int lcu_available_rightdwon, int luma_type);
	void (*esao_on_block_for_chroma)(pel* p_dst, int i_dst, pel* p_src, int i_src, int i_block_w, int i_block_h, int bo_value, int shift_value, int bit_depth, const int* esao_offset);
}ESAO_FUNC_POINTER;
/* ESAO_AVS3(END) */
#endif

#if CCSAO
/* CCSAO_AVS3(START) */
typedef struct
{
	long long diff[CCSAO_CLASS_NUM];
	int       count[CCSAO_CLASS_NUM];
}CCSAO_STAT_DATA;

#if CCSAO_ENHANCEMENT
typedef struct
{
	long long diff[CCSAO_SET_NUM][CCSAO_CLASS_NUM];
	int       count[CCSAO_SET_NUM][CCSAO_CLASS_NUM];
}CCSAO_REFINE_DATA;

typedef struct
{
	int       lcu_pos;
	double    lcu_cost;
}CCSAO_LCU_COST;

typedef struct
{
	BOOL      en;
	int       set;
	int       cnt;
}CCSAO_SET_CNT;

typedef struct
{
	int       set_num;
	int       type[CCSAO_SET_NUM];
	int       mode[CCSAO_SET_NUM];
	int       mode_c[CCSAO_SET_NUM];
	int       offset[CCSAO_SET_NUM][CCSAO_CLASS_NUM];
	int* lcu_flag;
#if ECCSAO
	int       class_type[CCSAO_SET_NUM];
#endif
}CCSAO_BLK_PARAM;
#else
typedef struct
{
	int       type;
	int       mode;
	int       offset[CCSAO_CLASS_NUM];
	int* lcu_flag;
}CCSAO_BLK_PARAM;
#endif

typedef struct
{
	void(*ccsao_on_block_for_chroma)(pel* p_dst, int i_dst, pel* p_src, int i_src,
#if CCSAO_ENHANCEMENT
		pel* p_src2, int i_src2,
#endif
		int lcu_width_c, int lcu_height_c, int bit_depth, int type, int band_num,
#if CCSAO_ENHANCEMENT
		int band_num_c,
#endif
		const int* ccsao_offset,
		int lcu_available_left, int lcu_available_right, int lcu_available_up, int lcu_available_down, int lcu_available_upleft, int lcu_available_upright, int lcu_available_leftdown, int lcu_available_rightdwon);
}CCSAO_FUNC_POINTER;
/* CCSAO_AVS3(END) */
#endif
/* ALF_AVS2(START) */
typedef struct ALFParam
{
    int alf_flag;
    int num_coeff;
    int filters_per_group;
    int componentID;
    int *filterPattern;
    int **coeffmulti;
} ALFParam;

typedef struct ALF_APS
{
    int usedflag;
    int cur_number;
    int max_number;
    ALFParam alf_par[N_C];
} ALF_APS;

typedef struct _AlfCorrData
{
    double ** *ECorr; //!< auto-correlation matrix
    double  **yCorr; //!< cross-correlation
    double   *pixAcc;
    int componentID;
} AlfCorrData;
/* ALF_AVS2(END) */

#if USE_ENH
typedef struct _ALF_PARAM
{
	int alf_flag;
	int num_coeff;
	int filters_per_group;
#if ALF_IMP
	int dir_index;
	int max_filter_num;
#endif
	int component_id;
	int* filter_pattern;
	int** coeff_multi;
} ALF_PARAM;

typedef struct _ALF_CORR_DATA
{
	double*** E_corr; //!< auto-correlation matrix
	double** y_corr; //!< cross-correlation
	double* pix_acc;
	int       component_id;
} ALF_CORR_DATA;
#endif
/* ALF_AVS2(END) */

typedef struct uavs3e_com_subpel_t {
	int          b_used;
	COM_IMGB* imgs[4][4];
} com_subpel_t;

typedef struct _COM_POS
{
	u8 a;    // 1/4 position
	u8 b;    // 2/4 position
	u8 c;    // 3/4 position
} COM_POS;
typedef struct _COM_POS_INFO
{
	COM_POS x;   // x dimension
	COM_POS y;   // y dimension
} COM_POS_INFO;


/* picture store structure */
typedef struct _COM_PIC
{
    /* Start address of Y component (except padding) */
    pel             *y;
    /* Start address of U component (except padding)  */
    pel             *u;
    /* Start address of V component (except padding)  */
    pel             *v;
	/*Start address of RFC Bin and Prm*/
	pel*y_rfcBin;
	pel*u_rfcBin;
	pel*v_rfcBin;
	u16             *y_rfcPrm;
	u16             *u_rfcPrm;	
	u16             *v_rfcPrm;
    /* Stride of luma picture */
    int              stride_luma;
    /* Stride of chroma picture */
    int              stride_chroma;
    /* Width of luma picture */
    int              width_luma;
    /* Height of luma picture */
    int              height_luma;
    /* Width of chroma picture */
    int              width_chroma;
    /* Height of chroma picture */
    int              height_chroma;
    /* padding size of luma */
    int              padsize_luma;
    /* padding size of chroma */
    int              padsize_chroma;
    /* image buffer */
    COM_IMGB        *imgb;
    COM_IMGB        *img;
    /* decoding temporal reference of this picture */
    int              dtr;
    /* presentation temporal reference of this picture */
    int              ptr;
    int              picture_output_delay;
    /* 0: not used for reference buffer, reference picture type */
    u8               is_ref;
    /* needed for output? */
    u8               need_for_out;
    /* scalable layer id */
    u8               temporal_id;
	s32	(*map_mv)[REFP_NUM][MV_D];//s16            (*map_mv)[REFP_NUM][MV_D];
	s32	(*map_refi)[REFP_NUM];//s8             (*map_refi)[REFP_NUM];
    u32              list_ptr[MAX_NUM_REF_PICS];
    double       picture_satd;                   /* [org] */
    int          picture_qp;                     /* [org] */
    double       picture_qp_real;                /* [rec] - real qp in bit depth of 8 */
    int          picture_bits;                   /* [rec] */
    com_subpel_t  subpel;
    int lay_id;
} COM_PIC;

/*****************************************************************************
* for binary and triple tree structure
*****************************************************************************/
typedef enum _SPLIT_MODE
{
	NO_SPLIT = 0,
	SPLIT_BI_VER = 1,
	SPLIT_BI_HOR = 2,
#if EQT
	SPLIT_EQT_VER = 3,
	SPLIT_EQT_HOR = 4,
	SPLIT_QUAD = 5,
#else
	SPLIT_QUAD = 3,
#endif
	NUM_SPLIT_MODE
} SPLIT_MODE;

typedef enum _SPLIT_DIR
{
	SPLIT_VER = 0,
	SPLIT_HOR = 1,
	SPLIT_QT = 2,
} SPLIT_DIR;

#if MODE_CONS
typedef enum _CONS_PRED_MODE
{
	NO_MODE_CONS = 0,
	ONLY_INTER = 1,
	ONLY_INTRA = 2,
} CONS_PRED_MODE;
#endif

#if CHROMA_NOT_SPLIT
typedef enum _TREE_STATUS
{
	TREE_LC = 0,
	TREE_L = 1,
	TREE_C = 2,
} TREE_STATUS;
#endif

typedef enum _CHANNEL_TYPE
{
	CHANNEL_LC = 0,
	CHANNEL_L = 1,
	CHANNEL_C = 2,
} CHANNEL_TYPE;

#if TB_SPLIT_EXT
typedef enum _PART_SIZE
{
	SIZE_2Nx2N,           ///< symmetric partition,  2Nx2N
	SIZE_2NxnU,           ///< asymmetric partition, 2Nx( N/2) + 2Nx(3N/2)
	SIZE_2NxnD,           ///< asymmetric partition, 2Nx(3N/2) + 2Nx( N/2)
	SIZE_nLx2N,           ///< asymmetric partition, ( N/2)x2N + (3N/2)x2N
	SIZE_nRx2N,           ///< asymmetric partition, (3N/2)x2N + ( N/2)x2N
	SIZE_NxN,             ///< symmetric partition, NxN
	SIZE_2NxhN,           ///< symmetric partition, 2N x 0.5N
	SIZE_hNx2N,           ///< symmetric partition, 0.5N x 2N
	NUM_PART_SIZE
} PART_SIZE;
#endif
#define MAX_SCU_SIZE (MAX_CU_SIZE2 >> 2)
# if Use_MPM
struct MPM_ARRAY {
	pel buffer[16][16];//64/4
	pel up[max_buffer_width / 4];//1920/4
	pel left_buffer[16];
	int top_buffer_stride1;
	int left_buffer_stride1;
	pel left[MAX_SCU_SIZE];

	void get_mpm_up(pel* up, MPM_ARRAY* MPM_buffer_ptr, U14 x_pos_pic, U14 y_pos_pic, int cdebug = 0)
	{
		int x_scu = x_pos_pic >> 2;
		int y_scu = y_pos_pic >> 2;
		int x_scu_pos_lcu = (x_pos_pic >> 2) & 15;
		int y_scu_pos_lcu = (y_pos_pic >> 2) & 15;

		if (cdebug)
		{
			assert(*up == (y_scu_pos_lcu == 0 ?
				MPM_buffer_ptr->up[x_scu] :
				MPM_buffer_ptr->buffer[y_scu_pos_lcu - 1][x_scu_pos_lcu]));
		}
		else
		{
			*up = (y_scu_pos_lcu == 0 ?
				MPM_buffer_ptr->up[x_scu] :
				MPM_buffer_ptr->buffer[y_scu_pos_lcu - 1][x_scu_pos_lcu]);
		}
	}
	void get_mpm_left(pel* left, MPM_ARRAY* MPM_buffer_ptr, U14 x_pos_pic, U14 y_pos_pic, int cdebug = 0)
	{
		int x_scu = x_pos_pic >> 2;
		int y_scu = y_pos_pic >> 2;
		int x_scu_pos_lcu = (x_pos_pic >> 2) & 15;
		int y_scu_pos_lcu = (y_pos_pic >> 2) & 15;
		
		if (cdebug)
		{
			assert(*left == (x_scu_pos_lcu == 0 ?
				MPM_buffer_ptr->left_buffer[y_scu_pos_lcu] :
				MPM_buffer_ptr->buffer[y_scu_pos_lcu][x_scu_pos_lcu - 1]));
		}
		else
		{
			*left = (x_scu_pos_lcu == 0 ?
				MPM_buffer_ptr->left_buffer[y_scu_pos_lcu] :
				MPM_buffer_ptr->buffer[y_scu_pos_lcu][x_scu_pos_lcu - 1]);
		}
	}
};

template<size_t W, size_t H>
struct MPM_ARRAY_SIZE {
	pel up;
	pel left;

	pel up_line[W >> 2];
	pel left_line[H >> 2];

	pel ipm_line_ver[H >> 2];
	pel ipm_line_hor[(W >> 2) << 1];

	void print_line()
	{
#if PRINT_MPM_BUFFER
		printf("mpm_array:\n");
		for (int i = 0; i < (W >> 2); ++i)
		{
			::printf("%d ", up_line[i]);
		}
		printf("\n");
		for (int i = 0; i < (H >> 2); ++i)
		{
			::printf("%d ", left_line[i]);
		}
		printf("\n");
		printf("%d %d\n", up, left);
#endif
	}

	void print_buffer()
	{
		for (int i = 0; i < (H >> 2); ++i)
		{
			::printf("%d ", ipm_line_ver[i]);
		}
		printf("\n");
		for (int i = 0; i < ((W >> 2) << 1); ++i)
		{
			::printf("%d ", ipm_line_hor[i]);
		}
		printf("\n");
	}

	void fetch_bi_ver(int part_num, MPM_ARRAY_SIZE<2 * W, H>* father)
	{
#pragma HLS INLINE off
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (W >> 2); i++)
			{
				up_line[i] = father->up_line[i];
			}
			for (int i = 0; i < (H >> 2); i++)
			{
				left_line[i] = father->left_line[i];
			}
			up = up_line[0];
			left = left_line[0];
			break;
		case 1:
			for (int i = 0; i < (W >> 2); i++)
			{
				up_line[i] = father->up_line[i + (W >> 2)];
			}
			for (int i = 0; i < (H >> 2); i++)
			{
				left_line[i] = ipm_line_ver[i];
			}
			up = up_line[0];
			left = left_line[0];
			break;
		default:
			assert(0);
		}
	}
	void fetch_bi_hor(int part_num, MPM_ARRAY_SIZE<W, 2 * H>* father)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (W >> 2); i++)
			{
				up_line[i] = father->up_line[i];
			}
			for (int i = 0; i < (H >> 2); i++)
			{
				left_line[i] = father->left_line[i];
			}
			up = up_line[0];
			left = left_line[0];
			break;
		case 1:
			for (int i = 0; i < (W >> 2); i++)
			{
				up_line[i] = ipm_line_hor[i];
			}
			for (int i = 0; i < (H >> 2); i++)
			{
				left_line[i] = father->left_line[i + (H >> 2)];
			}
			up = up_line[0];
			left = left_line[0];
			break;
		default:
			assert(0);
		}
	}
	void update_bi_ver(int part_num, S8* ipm)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (H >> 2); i++)
			{
#pragma HLS pipeline
				ipm_line_ver[i] = ipm[((W >> 2) - 1) + i * (W >> 2)];
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}
	void update_bi_hor(int part_num, S8* ipm)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (W >> 2); i++)
			{
#pragma HLS pipeline
				ipm_line_hor[i] = ipm[i + ((H >> 2) - 1) * (W >> 2)];
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}
};
#endif

struct frame_buf_luma {
	pel up[max_buffer_width];
	pel left[MAX_CU_SIZE2];
	pel top_left;
};

struct frame_buf_chroma {
	pel up_chroma[max_buffer_width / 2];
	pel left_chroma[MAX_CU_SIZE2 / 2];
	pel top_left_chroma;
};

# if IP_Linebuffer
struct FRAME_MSG {
	frame_buf_luma luma;
	frame_buf_chroma chroma[2];
};

struct IP_ARRAY {
	//pel temp_buffer[64][64];//64/4
	//pel up_innerLB[16][64];//64/4
	//pel up_temp[128];
	//pel left[64];

	//pel temp_buffer_size[23][64][64];
	//
	//pel up_innerLB_size[23][16][64];
	//pel left_innerLB_size[23][64][16];
	//pel up_temp_size[23][128];
	//pel left_size[23][64];

	//pel top_left;
	//pel up[max_buffer_width];
	pel temp_buffer_chroma[2][32][32];
	pel up_chroma[2][max_buffer_width / 2];
	pel left_chroma[2][32];
	pel top_left_chroma[2];

	// TODO
	//void get_up_line(pel* up_line, IP_ARRAY* IP_buffer_ptr, int x_pos_lcu, int y_pos_lcu, int cu_width, int ip_array_size_idx, int cdebug = 0)
	//{
	//	for (int i = 0; i < cu_width; ++i)
	//	{
	//		if (cdebug)
	//		{
	//			assert(up_line[i] == 
	//				(y_pos_lcu == 0 ? 
	//					IP_buffer_ptr->up_temp_size[ip_array_size_idx][x_pos_lcu + i] : 
	//					IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(y_pos_lcu >> 2) - 1][x_pos_lcu + i]));
	//		}
	//		else
	//		{
	//			up_line[i] =
	//				y_pos_lcu == 0 ?
	//				IP_buffer_ptr->up_temp_size[ip_array_size_idx][x_pos_lcu + i] :
	//				IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(y_pos_lcu >> 2) - 1][x_pos_lcu + i];
	//		}
	//	}
	//	for (int i = cu_width; i < 2 * cu_width; ++i)
	//	{
	//		if (cdebug)
	//		{
	//			assert(up_line[i] == (y_pos_lcu == 0 ? IP_buffer_ptr->up_temp_size[ip_array_size_idx][x_pos_lcu + i] : IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(y_pos_lcu >> 2) - 1][x_pos_lcu + i]));
	//		}
	//		else
	//		{
	//			up_line[i] =
	//				y_pos_lcu == 0 ?
	//				IP_buffer_ptr->up_temp_size[ip_array_size_idx][x_pos_lcu + i] :
	//				IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(y_pos_lcu >> 2) - 1][x_pos_lcu + i];
	//		}
	//	}
	//}
	//void get_left_line(pel* left_line, IP_ARRAY* IP_buffer_ptr, int x_pos_lcu, int y_pos_lcu, int cu_height, int ip_array_size_idx, int cdebug = 0)
	//{
	//	for (int i = 0; i < cu_height; ++i)
	//	{
	//		if (cdebug)
	//		{
	//			assert(left_line[i] == (x_pos_lcu == 0 ? IP_buffer_ptr->left_size[ip_array_size_idx][y_pos_lcu + i] : IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][y_pos_lcu + i][(x_pos_lcu >> 2) - 1]));
	//		}
	//		else
	//		{
	//			left_line[i] =
	//				x_pos_lcu == 0 ?
	//				IP_buffer_ptr->left_size[ip_array_size_idx][y_pos_lcu + i] :
	//				IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][y_pos_lcu + i][(x_pos_lcu >> 2) - 1];
	//		}
	//	}
	//	for (int i = cu_height; i < 2 * cu_height; ++i)
	//	{
	//		if (cdebug)
	//		{
	//			assert(left_line[i] == (y_pos_lcu + cu_height < 64 ? ( x_pos_lcu == 0 ? IP_buffer_ptr->left_size[ip_array_size_idx][y_pos_lcu + i] : IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][y_pos_lcu + i][(x_pos_lcu >> 2) - 1] ) : -1));
	//		}
	//		else
	//		{
	//			left_line[i] =
	//				y_pos_lcu + cu_height < 64 ? (
	//					x_pos_lcu == 0 ?
	//					IP_buffer_ptr->left_size[ip_array_size_idx][y_pos_lcu + i] :
	//					IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][y_pos_lcu + i][(x_pos_lcu >> 2) - 1]
	//					) :
	//				-1;
	//		}
	//	}
	//}
	//void get_top_left(pel* top_left, IP_ARRAY* IP_buffer_ptr, int x_pos_lcu, int y_pos_lcu, int ip_array_size_idx, int cdebug = 0)
	//{
	//	if(cdebug){
	//		assert(*top_left == (y_pos_lcu == 0 ? ( x_pos_lcu == 0 ? IP_buffer_ptr->top_left : IP_buffer_ptr->up_temp_size[ip_array_size_idx][x_pos_lcu - 1] ) : (x_pos_lcu == 0 ? IP_buffer_ptr->left_size[ip_array_size_idx][y_pos_lcu - 1] : IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(y_pos_lcu >> 2) - 1][x_pos_lcu - 1])));
	//	}
	//	else
	//	{
	//		*top_left =
	//			y_pos_lcu == 0 ? (
	//				x_pos_lcu == 0 ?
	//				IP_buffer_ptr->top_left :
	//				IP_buffer_ptr->up_temp_size[ip_array_size_idx][x_pos_lcu - 1]
	//				) : (
	//					x_pos_lcu == 0 ?
	//					IP_buffer_ptr->left_size[ip_array_size_idx][y_pos_lcu - 1] :
	//					IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(y_pos_lcu >> 2) - 1][x_pos_lcu - 1]
	//					);
	//	}
	//}

	void get_up_line_chroma(pel* up_line, IP_ARRAY* IP_buffer_ptr, int x_pos_pic, int y_pos_lcu, int cu_width, int ip_array_size_idx, int cdebug = 0)
	{
		int x_pos_lcu = x_pos_pic & 31;
		for (int i = 0; i < cu_width; ++i)
		{
			if (cdebug)
			{
				assert(up_line[i] ==
					(y_pos_lcu == 0 ?
						IP_buffer_ptr->up_chroma[ip_array_size_idx][x_pos_pic + i] :
						IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu - 1][x_pos_lcu + i]));
			}
			else
			{
				up_line[i] =
					y_pos_lcu == 0 ?
					IP_buffer_ptr->up_chroma[ip_array_size_idx][x_pos_pic + i] :
					IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu - 1][x_pos_lcu + i];
			}
		}

		for (int i = cu_width; i < 2 * cu_width; ++i)
		{
			if (cdebug)
			{
				assert(up_line[i] ==
					(y_pos_lcu == 0 ?
						IP_buffer_ptr->up_chroma[ip_array_size_idx][x_pos_pic + i] :
						IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu - 1][x_pos_lcu + i]));
			}
			else
			{
				up_line[i] =
					y_pos_lcu == 0 ?
					IP_buffer_ptr->up_chroma[ip_array_size_idx][x_pos_pic + i] :
					IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu - 1][x_pos_lcu + i];
			}
		}
	}
	void get_left_line_chroma(pel* left_line, IP_ARRAY* IP_buffer_ptr, int x_pos_lcu, int y_pos_lcu, int cu_height, int ip_array_size_idx, int cdebug = 0)
	{
		for (int i = 0; i < cu_height; ++i)
		{
			if (cdebug)
			{
				assert(left_line[i] == 
					(x_pos_lcu == 0 ? 
						IP_buffer_ptr->left_chroma[ip_array_size_idx][y_pos_lcu + i] :
						IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu + i][x_pos_lcu - 1]));
			}
			else
			{
				left_line[i] =
					x_pos_lcu == 0 ?
					IP_buffer_ptr->left_chroma[ip_array_size_idx][y_pos_lcu + i] :
					IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu + i][x_pos_lcu - 1];
			}
		}

		for (int i = cu_height; i < 2 * cu_height; ++i)
		{
			if (cdebug)
			{
				assert(left_line[i] ==
					(y_pos_lcu + cu_height < 32 ?
						(x_pos_lcu == 0 ?
							IP_buffer_ptr->left_chroma[ip_array_size_idx][y_pos_lcu + i] :
							IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu + i][x_pos_lcu - 1]) :
						-1));
			}
			else
			{
				left_line[i] =
					y_pos_lcu + cu_height < 32 ? (
						x_pos_lcu == 0 ?
						IP_buffer_ptr->left_chroma[ip_array_size_idx][y_pos_lcu + i] :
						IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu + i][x_pos_lcu - 1]
						) :
					-1;
			}
		}
	}
	void get_top_left_chroma(pel* top_left, IP_ARRAY* IP_buffer_ptr, int x_pos_pic, int y_pos_lcu, int ip_array_size_idx, int cdebug = 0)
	{
		int x_pos_lcu = x_pos_pic & 31;
		if (cdebug) {
			assert(*top_left ==
				(y_pos_lcu == 0 ?
					(x_pos_lcu == 0 ?
						IP_buffer_ptr->top_left_chroma[ip_array_size_idx] :
						IP_buffer_ptr->up_chroma[ip_array_size_idx][x_pos_pic - 1]
						) :
					(x_pos_lcu == 0 ?
						IP_buffer_ptr->left_chroma[ip_array_size_idx][y_pos_lcu - 1] :
						IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu - 1][x_pos_lcu - 1])));
		}
		else
		{
			*top_left =
				y_pos_lcu == 0 ? (
					x_pos_lcu == 0 ?
					IP_buffer_ptr->top_left_chroma[ip_array_size_idx] :
					IP_buffer_ptr->up_chroma[ip_array_size_idx][x_pos_pic - 1]
					) : 
				(x_pos_lcu == 0 ?
					IP_buffer_ptr->left_chroma[ip_array_size_idx][y_pos_lcu - 1] :
					IP_buffer_ptr->temp_buffer_chroma[ip_array_size_idx][y_pos_lcu - 1][x_pos_lcu - 1]
					);
		}
	}
};

#define SCU_IN_LCU_NUMS_LEN (MAX_CU_SIZE2>>MIN_CU_LOG2)

struct IP_MAP_SCU
{
	U10 min_scu_x;
	U10 min_scu_y;
	U1 curr_ctu_idx;
	U1 above_line_idx;
	U32 map_scu_inner[2][16][16];
	U32 map_scu_above_line[2][max_buffer_width >> MIN_CU_LOG2];

	void get_map_scu_up_line(U32* map_scu_up_line, IP_MAP_SCU* MAP_SCU_IP_ptr, int x_pos_pic, int y_pos_pic, int cu_width, int cdebug = 0)
	{
		int x_pos_lcu = x_pos_pic & 63;
		int y_pos_lcu = y_pos_pic & 63;
		for (int i = 0; i < ((cu_width >> 2) << 1); i++)
		{
			if (y_pos_lcu == 0)
			{
				if (cdebug)
				{
					assert(map_scu_up_line[i] == MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][(x_pos_pic >> 2) + i]);
				}
				else
				{
					map_scu_up_line[i] = MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][(x_pos_pic >> 2) + i];
				}
			}
			else
			{
				if (cdebug)
				{
					if ((x_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_x + i < 16)
						assert(map_scu_up_line[i] == MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][(x_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_x + i][(y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y - 1]);
				}
				else
				{
					if ((x_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_x + i < 16)
						map_scu_up_line[i] = MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][(x_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_x + i][(y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y - 1];
				}
			}
		}
	}
	void get_map_scu_left_line_new(U32* map_scu_left_line, IP_MAP_SCU* MAP_SCU_IP_ptr, int x_pos_pic, int y_pos_pic, int cu_height, int cdebug = 0)
	{
		int x_pos_lcu = x_pos_pic & 63;
		int y_pos_lcu = y_pos_pic & 63;

		for (int i = 0; i < ((cu_height >> 2) << 1); i++)
		{
			if (x_pos_lcu == 0)
			{
				if (cdebug)
				{
					if ((y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i < 16)
						assert(map_scu_left_line[i] == MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][(y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i]);
				}
				else
				{
					if ((y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i < 16)
						map_scu_left_line[i] = MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][(y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i];
				}
			}
			else
			{
				if (cdebug)
				{
					if ((y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i < 16)
					{
						assert(map_scu_left_line[i] == MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][(x_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_x - 1][(y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i]);
					}
				}
				else
				{
					if ((y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i < 16)
						map_scu_left_line[i] = MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][(x_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_x - 1][(y_pos_pic >> 2) - MAP_SCU_IP_ptr->min_scu_y + i];
				}
			}
		}
	}
	void get_map_scu_top_left_new(U32* map_scu_top_left, IP_MAP_SCU* MAP_SCU_IP_ptr, int x_pos_pic, int y_pos_pic, int cdebug = 0)
	{
		int x_pos_lcu = x_pos_pic & 63;
		int y_pos_lcu = y_pos_pic & 63;
		if (cdebug)
		{
			if (x_pos_pic > 0)
			{
				assert(*map_scu_top_left == (y_pos_lcu == 0 ?
					MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][(x_pos_pic >> 2) - 1] :
					(x_pos_lcu == 0 ?
						MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][(y_pos_lcu >> 2) - 1] :
						MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][(x_pos_lcu >> 2) - 1][(y_pos_lcu >> 2) - 1])
					));
			}
		}
		else
		{
			if (x_pos_pic > 0)
				*map_scu_top_left = (y_pos_lcu == 0 ? 
					MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][(x_pos_pic >> 2) - 1] :
					(x_pos_lcu == 0 ? 
						MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][(y_pos_lcu >> 2) - 1] :
						MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][(x_pos_lcu >> 2) - 1][(y_pos_lcu >> 2) - 1])
					);
			else
			{
				if (y_pos_lcu > 0)
				{
					*map_scu_top_left = MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][(y_pos_lcu >> 2) - 1];
				}
			}
		}
	}
};

void clear_map_scu_line(U32* line, U8 line_num);

template <size_t W, size_t H>
struct IP_MAP_SCU_SIZE
{
	U32 map_scu_above_line[(W >> 2) << 1];
	U32 map_scu_top_left;
	
	U32 map_scu_up_line[(W >> 2) << 1];
	U32 map_scu_left_line_new[(H >> 2) << 1];
	U32 map_scu_top_left_new;

	U32 cur_map_scu[W >> 2][H >> 2];

	void print_line() const
	{
#if PRINT_IP_MAP_SCU
		printf("ip_map_scu\n");
		for (int j = 0; j < (W >> 1); ++j)
		{
			printf("%x ", map_scu_up_line[j]);
		}
		printf("\n");
		for (int i = 0; i < (H >> 1); ++i)
		{
			printf("%x ", map_scu_left_line_new[i]);
		}
		printf("\n");
		printf("%x\n", map_scu_top_left_new);
#endif
	}


	void print_buffer() const
	{
		for (int i = 0; i < (W >> 2); ++i)
		{
			for (int j = 0; j < (H >> 2); ++j)
			{
				::printf("%d ", cur_map_scu[i][j]);
			}
			printf("\n");
		}
	}
	
	void fetch_map_scu_qt(int part_num, IP_MAP_SCU_SIZE<2 * W, 2 * H>* father)
	{
		assert(W == H);
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < W >> 2 << 1; i++)
			{
#pragma HLS PIPELINE
				map_scu_up_line[i] = father->map_scu_up_line[i];
				map_scu_left_line_new[i] = father->map_scu_left_line_new[i];
			}
			map_scu_top_left_new = father->map_scu_top_left_new;
			break;
		case 1:
			for (int i = 0; i < W >> 2 << 1; i++)
			{
#pragma HLS PIPELINE
				map_scu_up_line[i] = father->map_scu_up_line[i + (W >> 2)];
				map_scu_left_line_new[i] = father->cur_map_scu[(W >> 2) - 1][i];
			}
			map_scu_top_left_new = father->map_scu_up_line[(W >> 2) - 1];
			break;
		case 2:
			for (int i = 0; i < W >> 2 << 1; i++)
			{
#pragma HLS PIPELINE
				map_scu_up_line[i] = father->cur_map_scu[i][(H >> 2) - 1];
				map_scu_left_line_new[i] = father->map_scu_left_line_new[i + (H >> 2)];
			}
			map_scu_top_left_new = father->map_scu_left_line_new[(H >> 2) - 1];
			break;
		case 3:
			clear_map_scu_line(map_scu_up_line, (W >> 2) << 1);
			clear_map_scu_line(map_scu_left_line_new, (H >> 2) << 1);
			for (int i = 0; i < W >> 2; i++)
			{
#pragma HLS PIPELINE
				map_scu_up_line[i] = father->cur_map_scu[(W >> 2) + i][(W >> 2) - 1];
				map_scu_left_line_new[i] = father->cur_map_scu[(H >> 2) - 1][(H >> 2) + i];
			}
			map_scu_top_left_new = father->cur_map_scu[(W >> 2) - 1][(H >> 2) - 1];
			break;
		default:
			assert(0);
		}
	}
	void update_map_scu_qt(int part_num, U32 map_scu_father[(2 * W) >> 2][(2 * H) >> 2], u32* map_scu)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < H >> 2; i++)
			{
#pragma HLS PIPELINE
				for (int j = 0; j < W >> 2; j++)
				{
					map_scu_father[j][i] = map_scu[j + (i * (W >> 2))];
				}
			}
			break;
		case 1:
			for (int i = 0; i < H >> 2; i++)
			{
#pragma HLS PIPELINE
				for (int j = 0; j < W >> 2; j++)
				{
					map_scu_father[(W >> 2) + j][i] = map_scu[j + (i * (W >> 2))];
				}
			}
			break;
		case 2:
			for (int i = 0; i < H >> 2; i++)
			{
#pragma HLS PIPELINE
				for (int j = 0; j < W >> 2; j++)
				{
					map_scu_father[j][i + (H >> 2)] = map_scu[j + (i * (W >> 2))];
				}
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
	}
	void fetch_bi_ver(int part_num, IP_MAP_SCU_SIZE<2 * W, H>* father)
	{
#pragma HLS INLINE off
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < ((W >> 2) << 1); i++)
			{
				map_scu_up_line[i] = father->map_scu_up_line[i];
			}
			for (int i = 0; i < ((H >> 2) << 1); i++)
			{
				map_scu_left_line_new[i] = father->map_scu_left_line_new[i];
			}
			map_scu_top_left_new = father->map_scu_top_left_new;
			break;
		case 1:
			clear_map_scu_line(map_scu_left_line_new, (H >> 2) << 1);
			for (int i = 0; i < ((W >> 2) << 1); i++)
			{
				map_scu_up_line[i] = father->map_scu_up_line[i + (W >> 2)];
			}
			for (int i = 0; i < (H >> 2); i++)
			{
				map_scu_left_line_new[i] = father->cur_map_scu[(W >> 2) - 1][i];
			}
			map_scu_top_left_new = father->map_scu_up_line[(W >> 2) - 1];
			break;
		default:
			assert(0);
		}
	}
	void fetch_bi_hor(int part_num, IP_MAP_SCU_SIZE<W, 2 * H>* father)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < ((W >> 2) << 1); i++)
			{
				map_scu_up_line[i] = father->map_scu_up_line[i];
			}
			for (int i = 0; i < ((H >> 2) << 1); i++)
			{
				map_scu_left_line_new[i] = father->map_scu_left_line_new[i];
			}
			map_scu_top_left_new = father->map_scu_top_left_new;
			break;
		case 1:
			clear_map_scu_line(map_scu_up_line, (W >> 2) << 1);
			for (int i = 0; i < (W >> 2); i++)
			{
				map_scu_up_line[i] = father->cur_map_scu[i][(H >> 2) - 1];
			}
			for (int i = 0; i < ((H >> 2) << 1); i++)
			{
				map_scu_left_line_new[i] = father->map_scu_left_line_new[i + (H >> 2)];
			}
			map_scu_top_left_new = father->map_scu_left_line_new[(H >> 2) - 1];
			break;
		default:
			assert(0);
		}
	}
	void update_bi_ver(int part_num, U32 map_scu_father[(2 * W) >> 2][H >> 2], u32* map_scu)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (H >> 2); i++)
			{
#pragma HLS pipeline
				for (int j = 0; j < (W >> 2); j++)
				{
					map_scu_father[j][i] = map_scu[j + (i * (W >> 2))];
				}
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}
	void update_bi_hor(int part_num, U32 map_scu_father[W >> 2][(2 * H) >> 2], u32* map_scu)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (H >> 2); i++)
			{
#pragma HLS pipeline
				for (int j = 0; j < (W >> 2); j++)
				{
					map_scu_father[j][i] = map_scu[j + (i * (W >> 2))];
				}
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}

	void fetch_map_scu_btv(int part_num, IP_MAP_SCU_SIZE<2 * W, H>* father)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < ((W >> 2) << 1); i++)
			{
				map_scu_above_line[i] = father->map_scu_above_line[i];
			}
			map_scu_top_left = father->map_scu_top_left;
			break;
		case 1:
			for (int i = 0; i < ((W >> 2) << 1); i++)
			{
				map_scu_above_line[i] = father->map_scu_above_line[i + (W >> 2)];
			}
			map_scu_top_left = father->map_scu_above_line[(W >> 2) - 1];
			break;
		default:
			assert(0);
		}
	}
	void fetch_map_scu_bth(int part_num, IP_MAP_SCU_SIZE<W, 2 * H>* father)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < ((W >> 2) << 1); i++)
			{
				map_scu_above_line[i] = father->map_scu_above_line[i];
			}
			map_scu_top_left = father->map_scu_top_left;
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}
};

#define RANGE_NUM 2
#define DIR_DIM 2
#define MAP_DIMS 2
#define CU_SIZE (1 << (MAX_CU_DEPTH - 1))* (1 << (MAX_CU_DEPTH - 1))

typedef enum _BLOCK_SHAPE
{
	NON_SQUARE_18,
	NON_SQUARE_14,
	NON_SQUARE_12,
	SQUARE,
	NON_SQUARE_21,
	NON_SQUARE_41,
	NON_SQUARE_81,
	NUM_BLOCK_SHAPE,
} BLOCK_SHAPE;

typedef struct _MD_INPUT {
	U13 pix_x;
	U13 pix_y;
	u32             lambda_mv;
	s64          lambda[3];
	s64             lambda_y;
	s64             lambda_u;
	s64             lambda_v;
	U6             qp_y;
	U6             qp_u;
	U6             qp_v;
	S64				dist_chroma_weight[2];
	U16				x_last_ctu;
	U16				y_last_ctu;
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
#endif // ENABLE_BFRAME
	//**************for md chroma****************************
	U8  split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
	U1 do_chroma;
	U3  pred_mode[CU_SIZE];
	S8  ipm[2][CU_SIZE];
	U8  affine_flag[CU_SIZE];
	S14 mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	S3  refi[CU_SIZE][REFP_NUM];
	int num_nz_coef[N_C][CU_SIZE];
	u32 map_scu[CU_SIZE];
	u32 map_cu_mode[CU_SIZE];
#if USE_TSCPM
	pel reco_y[CU_SIZE << 2][CU_SIZE << 2];
#endif
#if USE_ENH
	U8  umve_flag[CU_SIZE];
	U8  skip_idx[CU_SIZE];
#if INTERPF
	u8 inter_filter_flag[CU_SIZE];
#endif
#endif
#if USE_INTRA_REFRESH
	U1 EncRefresh;
	U1 RefreshMode;
	U8 RefreshSize;
	U8 pirStartRow;
	U8 pirEndRow;
	U8 pirStartCol;
	U8 pirEndCol;
#endif
#if USE_SPEED_LEVEL
	U8 speed_level;
#endif
#if USE_ROI_MODE_CONFIG
	U1 valid_mode_config[3];//valid_mode[0] skip;valid_mode[1] inter;valid_mode[2] intra
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	U6 cu_luma_qp[CU_SIZE];
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	int md_slice_qp;
	int CuQPMap[256];
#endif

	friend void copy_luma(_MD_INPUT& _Left, const _MD_INPUT& _Right)
	{
		_Left.pix_x = _Right.pix_x;
		_Left.pix_y = _Right.pix_y;
		_Left.lambda_mv = _Right.lambda_mv;
		for (int i = 0; i < 3; ++i)
		{
			_Left.lambda[i] = _Right.lambda[i];
		}
		_Left.lambda_y = _Right.lambda_y;
		_Left.qp_y = _Right.qp_y;
		for (int i = 0; i < 2; ++i)
		{
			_Left.dist_chroma_weight[i] = _Right.dist_chroma_weight[i];
		}
		_Left.x_last_ctu = _Right.x_last_ctu;
		_Left.y_last_ctu = _Right.y_last_ctu;
		for (int i = 0; i < RANGE_NUM; ++i)
		{
			for (int j = 0; j < DIR_DIM; ++j)
			{
				for (int k = 0; k < MAP_DIMS; ++k)
				{
					_Left.CtrPos2MD[i][j][k] = _Right.CtrPos2MD[i][j][k];
#if ENABLE_BFRAME
					_Left.CtrPos2MD_ref1[i][j][k] = _Right.CtrPos2MD_ref1[i][j][k];
#endif // ENABLE_BFRAME
				}
			}
		}
		_Left.cu_dqp_enable       = _Right.cu_dqp_enable;
		_Left.cu_qp_group_pred_qp = _Right.cu_qp_group_pred_qp;
		for (int i = 0; i < 256; ++i) {
#pragma HLS UNROLL
			_Left.CuQPMap[i] = _Right.CuQPMap[i];
		}
	}
}MD_INPUT;

template <size_t W, size_t H>
struct IP_ARRAY_SIZE
{
	pel	up_line[2 * W];
	pel left_line[2 * H];
	pel top_left;

	pel rec_line_hor[2 * W];
	pel rec_line_ver[2 * H];

	void print_line()
	{
#if PRINT_IP_BUFFER
		printf("ip_array:\n");
		for (int i = 0; i < 2 * W; ++i)
		{
			::printf("%d ", up_line[i]);
		}
		printf("\n");
		for (int i = 0; i < 2 * H; ++i)
		{
			::printf("%d ", left_line[i]);
		}
		printf("\n");
		printf("%d\n", top_left);
#endif
	}

	void print_buffer()
	{
		for (int i = 0; i < 2 * W; ++i)
		{
			::printf("%d ", rec_line_hor[i]);
		}
		printf("\n");
		for (int i = 0; i < 2 * H; ++i)
		{
			::printf("%d ", rec_line_ver[i]);
		}
		printf("\n");
	}

	void fetch_bi_ver(int part_num, IP_ARRAY_SIZE<2 * W, H>* father)
	{
#pragma HLS INLINE off
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 2 * W; ++i)
			{
#pragma HLS pipeline
				up_line[i] = father->up_line[i];
			}
			for (int i = 0; i < (H == 64 ? H : 2 * H); ++i)
			{
#pragma HLS pipeline
				left_line[i] = father->left_line[i];
			}
			top_left = father->top_left;
			break;
		case 1:
			for (int i = 0; i < 2 * W; ++i)
			{
#pragma HLS pipeline
				up_line[i] = father->up_line[i + W];
			}
			for (int i = 0; i < H; ++i)
			{
#pragma HLS pipeline
				left_line[i] = rec_line_ver[i];
			}
			top_left = father->up_line[W - 1];
			break;
		default: assert(0);
		}
	}

#if USE_TSCPM
	void fetch_bi_ver_chroma(int part_num, IP_ARRAY_SIZE<2 * W, H>* father, MD_INPUT* md_input_ptr,int x_pos_lcu, int y_pos_lcu)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 2 * W; ++i)
			{
				up_line[i] = father->up_line[i];
			}
			for (int i = 0; i < (H == 64 ? H : 2 * H); ++i)
			{
				left_line[i] = father->left_line[i];
			}
			top_left = father->top_left;
			break;
		case 1:
			for (int i = 0; i < 2 * W; ++i)
			{
				up_line[i] = father->up_line[i + W];
			}
			for (int i = 0; i < H; ++i)
			{
				left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];;
			}
			top_left = father->up_line[W - 1];
			break;
		default: assert(0);
		}
	}

	void fetch_bi_hor_chroma(int part_num, IP_ARRAY_SIZE<W, 2 * H>* father, MD_INPUT* md_input_ptr, int x_pos_lcu, int y_pos_lcu)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 2 * W; ++i)
			{
				up_line[i] = father->up_line[i];
			}
			for (int i = 0; i < 2 * H; ++i)
			{
				left_line[i] = father->left_line[i];
			}
			top_left = father->top_left;
			break;
		case 1:
			for (int i = 0; i < W; ++i)
			{
				up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu ];;
			}
			for (int i = 0; i < (H == 32 ? H : 2 * H); ++i)
			{
				left_line[i] = father->left_line[i + H];
			}
			top_left = father->left_line[H - 1];
			break;
		default: assert(0);
		}
	}
#endif

	void update_bi_ver(int part_num, pel* rec)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < H; ++i)
			{
				rec_line_ver[i] = rec[i * W + W - 1];
			}
			break;
		case 1:
			break;
		default: assert(0);
		}
	}

	void fetch_bi_hor(int part_num, IP_ARRAY_SIZE<W, 2 * H>* father)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 2 * W; ++i)
			{
#pragma HLS pipeline
				up_line[i] = father->up_line[i];
			}
			for (int i = 0; i < 2 * H; ++i)
			{
#pragma HLS pipeline
				left_line[i] = father->left_line[i];
			}
			top_left = father->top_left;
			break;
		case 1:
			for (int i = 0; i < W; ++i)
			{
#pragma HLS pipeline
				up_line[i] = rec_line_hor[i];
			}
			for (int i = 0; i < (H == 32 ? H : 2 * H); ++i)
			{
#pragma HLS pipeline
				left_line[i] = father->left_line[i + H];
			}
			top_left = father->left_line[H - 1];
			break;
		default: assert(0);
		}
	}

	void update_bi_hor(int part_num, pel* rec)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < W; ++i)
			{
				rec_line_hor[i] = rec[W * (H - 1) + i];
			}
			break;
		case 1:
			break;
		default: assert(0);
		}
	}

	void const_up(pel* up, IP_MAP_SCU_SIZE<W, H>* IP_map_scu_ptr, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu)
	{
		if ((((int)avail_cu) & 1 << 0) == 1 << 0)
		{
			int flag;
			for (int i = 0; i < W; ++i)
			{
#pragma HLS UNROLL
				up[i + 3] = up_line[i];
			}
			for (int i = 0; i < (W >> 2); i++)
			{
#pragma HLS UNROLL
				if (pic_y == 0 ? x_scu + (W >> 2) + i < pic_width_in_scu : pic_x_scu + (W >> 2) + i < 16)
				{
					flag = IP_map_scu_ptr->map_scu_up_line[(W >> 2) + i] >> 31 & 1;
				}
				else
				{
					flag = 0;
				}
				if (x_scu + (W >> 2) + i < pic_width_in_scu && flag)
				{
					for (int j = 0; j < 4; j++)
					{
#pragma HLS UNROLL
						up[W + (i << 2) + j + 3] = up_line[W + (i << 2) + j];
					}
				}
				else
				{
					for (int j = 0; j < 4; j++)
					{
#pragma HLS UNROLL
						up[W + (i << 2) + j + 3] = up[W + (i << 2) + 2];
					}
				}
			}
		}
	}

	void const_left(pel* left, IP_MAP_SCU_SIZE<W, H>* IP_map_scu_ptr, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu)
	{
		if ((avail_cu & 1 << 1) == 1 << 1)
		{
			int flag;
			for (int i = 0; i < H; ++i)
			{
#pragma HLS UNROLL
				left[i + 3] = left_line[i];
			}
			for (int i = 0; i < (H >> 2); i++)
			{
#pragma HLS UNROLL
				if (pic_y_scu + (H >> 2) + i < 16)
				{
					flag = IP_map_scu_ptr->map_scu_left_line_new[(H >> 2) + i] >> 31 & 1;
				}
				else
				{
					flag = 0;
				}
				if (y_scu + (H >> 2) + i < pic_height_in_scu && flag)
				{
					for (int j = 0; j < 4; j++)
					{
#pragma HLS UNROLL
						left[H + (i << 2) + j + 3] = left_line[H + (i << 2) + j];
					}
				}
				else
				{
					for (int j = 0; j < 4; j++)
					{
#pragma HLS UNROLL
						left[H + (i << 2) + j + 3] = left[H + (i << 2) + 2];
					}
				}
			}
		}
	}

	void const_top_left(pel* up, pel* left, int avail_cu)
	{
		if ((avail_cu & 1 << 2) == 1 << 2)
		{
			up[2] = left[2] = top_left;
		}
		else if ((avail_cu & 1 << 0) == 1 << 0)
		{
			up[2] = left[2] = up[3];
		}
		else if ((avail_cu & 1 << 1) == 1 << 1)
		{
			up[2] = left[2] = left[3];
		}
	}

#define CUR_REC_SIZE (W > H ? W : H)
	void update_bi_ver(int part_num, pel rec[CUR_REC_SIZE][CUR_REC_SIZE])
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < H; ++i)
			{
#pragma HLS pipeline
				rec_line_ver[i] = rec/*[i * W + W - 1]*/[i][W - 1];
			}
			break;
		case 1:
			break;
		default: assert(0);
		}
	}
	void update_bi_hor(int part_num, pel rec[CUR_REC_SIZE][CUR_REC_SIZE])
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < W; ++i)
			{
#pragma HLS pipeline
				rec_line_hor[i] = rec/*[W * (H - 1) + i]*/[H - 1][i];
			}
			break;
		case 1:
			break;
		default: assert(0);
		}
	}
};

template <>
struct IP_ARRAY_SIZE<64, 64>
{
	pel	up_line[128];
	pel left_line[64];
	pel top_left;
};
#endif



typedef enum _CTX_NEV_IDX
{
	CNID_SKIP_FLAG,
	CNID_PRED_MODE,
	CNID_AFFN_FLAG,
	NUM_CNID,
} CTX_NEV_IDX;


//IME_LINE_BUFFER
#define USE_IME_LINE 1
#define NEB_NUMS 5 /*(A-B-C-D-G)*/
#define SCU_IN_LCU_NUMS_LEN (MAX_CU_SIZE2>>MIN_CU_LOG2)
#define SCU_IN_LCU_NUMS (SCU_IN_LCU_NUMS_LEN*SCU_IN_LCU_NUMS_LEN)
#define IME_LINE_ALL USE_IME_LINE && 1
#if USE_IME_LINE
#define USE_INNER_BEST_CU 1
#define IME_LINE_NEW IME_LINE_ALL&&0
#define IME_LINE_INNER IME_LINE_ALL&&1
#define USE_IME_LINE_COMP 1
#define USE_ABOVE_FRAOM_MAP 1
#define ABOVE_LINE_LEN SCU_IN_LCU_NUMS_LEN*3
#define SCUX_OFFSET SCU_IN_LCU_NUMS_LEN

struct ENC_ME_NEB_INFO
{
	S14 mv[REFP_NUM][MV_D];
	S2 refi[REFP_NUM];

	void print() const
	{
		::printf("%d %d %d %d %d %d\n", mv[0][0], mv[0][1], mv[1][0], mv[1][0], refi[0], refi[1]);
	}
};

template <size_t _Width, size_t _Height>
void print(ENC_ME_NEB_INFO up_nebs[(_Width >> 2) + 2], ENC_ME_NEB_INFO left_nebs[_Height >> 2])
{
#if PRINT_NEB_LINE
	printf("up_nebs\n");
	for (int i = 0; i < (_Width >> 2) + 2; ++i)
	{
		up_nebs[i].print();
	}
	printf("left_nebs\n");
	for (int i = 0; i < (_Height >> 2); ++i)
	{
		left_nebs[i].print();
	}
#endif
}

#define MAX_WIDTH 3840

typedef struct _ENC_ME_LINE_MAP_COPY
{
	U11 min_scu_x;
	U11 min_scu_y;
	U11 stride_in_map;
	U5 stride_in_line;
	U11 pic_width_in_scu;
	U11 pic_height_in_scu;
	U1 above_line_idx;
	U1 curr_ctu_idx;
	U13 x_ctu;
	U13 y_ctu;
}ENC_ME_LINE_MAP_COPY;

typedef struct _ENC_ME_LINE_MAP
{
	U32 new_map_scu_above_line[ABOVE_LINE_LEN];
	U32 new_map_cu_mode_above_line[ABOVE_LINE_LEN];
	//U32* new_map_scu_above_line;

	//int min_scu;
	U11 min_scu_x;
	U11 min_scu_y;
	U11 stride_in_map;
	U5 stride_in_line;
	U11 pic_width_in_scu;
	U11 pic_height_in_scu;
	U1 above_line_idx;
	U1 curr_ctu_idx;
	U13 x_ctu;
	U13 y_ctu;
	/* LCU split information */
	//U6  line_map_split[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];

	U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	//U32 map_scu_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	//U32 map_cu_mode_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	S14 co_left_mv[SCU_IN_LCU_NUMS_LEN][MV_D];
	S2 co_left_refi[SCU_IN_LCU_NUMS_LEN];
	S16 co_left_ptr_ref[SCU_IN_LCU_NUMS_LEN];
	/* buffer */
	//S2 refi_nebs[NEB_NUMS];
	//u32 scu_nebs[NEB_NUMS];
	//S14 mv_nebs[NEB_NUMS][MV_D];
	//U1 if_init;

	void cp(_ENC_ME_LINE_MAP * dst, _ENC_ME_LINE_MAP * src)
	{
		for (int i=0; i<ABOVE_LINE_LEN; i++) {
			dst->new_map_scu_above_line    [i] = src->new_map_scu_above_line    [i];
			dst->new_map_cu_mode_above_line[i] = src->new_map_cu_mode_above_line[i];
		}
		dst->min_scu_x         = src->min_scu_x;
	    dst->min_scu_y         = src->min_scu_y;
	    dst->stride_in_map     = src->stride_in_map;
	    dst->stride_in_line    = src->stride_in_line;
	    dst->pic_width_in_scu  = src->pic_width_in_scu;
	    dst->pic_height_in_scu = src->pic_height_in_scu;
	    dst->above_line_idx    = src->above_line_idx;
	    dst->curr_ctu_idx      = src->curr_ctu_idx;
	    dst->x_ctu             = src->x_ctu;
	    dst->y_ctu             = src->y_ctu;
		for (int i=0; i<2; i++) {
			for (int j=0; j<SCU_IN_LCU_NUMS_LEN; j++) {
				for (int k=0; k<SCU_IN_LCU_NUMS_LEN; k++) {
					dst->map_scu_inner    [i][j][k] = src->map_scu_inner    [i][j][k];
					dst->map_cu_mode_inner[i][j][k] = src->map_cu_mode_inner[i][j][k];
				}
			}
		}
		for (int i=0; i<SCU_IN_LCU_NUMS_LEN; i++) {
			for (int j=0; j<MV_D; j++) {
				dst->co_left_mv[i][j] = src->co_left_mv[i][j];
			}
			dst->co_left_refi   [i] = src->co_left_refi   [i];
			dst->co_left_ptr_ref[i] = src->co_left_ptr_ref[i];
		}
	}
}ENC_ME_LINE_MAP;

typedef struct _ENC_ME_LINE_MAP_AXIS
{
	U11 min_scu_x;
	U11 min_scu_y;
	U11 stride_in_map;
	U5  stride_in_line;
	U11 pic_width_in_scu;
	U11 pic_height_in_scu;
	U1  above_line_idx;
	U1  curr_ctu_idx;
	U13 x_ctu;
	U13 y_ctu;
}ENC_ME_LINE_MAP_AXIS;

template<size_t SIZE>
struct ENC_ME_LINE_MAP_SIZE
{
	U32		map_scu_up_line[1 + (SIZE >> 2) + 1];
	U32		map_cu_mode_up_line[1 + (SIZE >> 2) + 1];

	U32		map_scu_left_line[(SIZE >> 2)];
	U32		map_cu_mode_left_line[(SIZE >> 2)];

	U11		min_scu_x;
	U11		min_scu_y;
	U1		above_line_idx;
	U1		curr_ctu_idx;

	U32		map_scu_line_hor[SIZE << 1];
	U32		map_cu_mode_line_hor[SIZE << 1];
	U32		map_scu_line_ver[SIZE];
	U32		map_cu_mode_line_ver[SIZE];

	void print_line() const
	{
#if PRINT_ME_LINE_MAP
		printf("me_line_map:\n");
		for (int i = 0; i < (1 + (SIZE >> 2) + 1); ++i)
		{
			printf("%x ", map_scu_up_line[i]);
		}
		printf("\n");
		for (int i = 0; i < (1 + (SIZE >> 2) + 1); ++i)
		{
			printf("%x ", map_cu_mode_up_line[i]);
		}
		printf("\n");
		for (int i = 0; i < (SIZE >> 2); ++i)
		{
			printf("%x ", map_scu_left_line[i]);
		}
		printf("\n");
		for (int i = 0; i < (SIZE >> 2); ++i)
		{
			printf("%x ", map_cu_mode_left_line[i]);
		}
		printf("\n");
		printf("%d %d\n", min_scu_x, min_scu_y);
#endif
	}

	void fetch_me_line_qt(U3 part_num, ENC_ME_LINE_MAP_SIZE<SIZE * 2>* father, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < ((cu_width >> 2) + 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
				map_scu_up_line[i] = father->map_scu_up_line[i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = father->map_scu_left_line[i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[i];
			}
			break;
		case 1:
			for (int i = 0; i < ((cu_width >> 2) + 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
				map_scu_up_line[i] = father->map_scu_up_line[(cu_width >> 2) + i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[(cu_width >> 2) + i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = map_scu_line_ver[i];
				map_cu_mode_left_line[i] = map_cu_mode_line_ver[i];
			}
			break;
		case 2:
			map_scu_up_line[0] = father->map_scu_left_line[(cu_height >> 2) - 1];
			map_cu_mode_up_line[0] = father->map_cu_mode_left_line[(cu_height >> 2) - 1];
			for (int i = 0; i < (cu_width >> 2) + 1; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=17
#pragma HLS pipeline
				map_scu_up_line[i + 1] = map_scu_line_hor[i];
				map_cu_mode_up_line[i + 1] = map_cu_mode_line_hor[i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = father->map_scu_left_line[(cu_height >> 2) + i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[(cu_height >> 2) + i];
			}
			break;
		case 3:
			for (int i = 0; i < (cu_width >> 2) + 1; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=17
#pragma HLS pipeline
				map_scu_up_line[i] = map_scu_line_hor[(cu_width >> 2) - 1 + i];
				map_cu_mode_up_line[i] = map_cu_mode_line_hor[(cu_width >> 2) - 1 + i];
			}
			map_scu_up_line[(cu_width >> 2) + 1] = 0;
			map_cu_mode_up_line[(cu_width >> 2) + 1] = 0;
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = map_scu_line_ver[i];
				map_cu_mode_left_line[i] = map_cu_mode_line_ver[i];
			}
			break;
		default:
			assert(0);
		}

		min_scu_x = father->min_scu_x;
		min_scu_y = father->min_scu_y;
		above_line_idx = father->above_line_idx;
		curr_ctu_idx = father->curr_ctu_idx;
	}
	void update_me_line_qt(int part_num, u32* map_scu, U32* map_cu_mode, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_width >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_line_hor[i] = map_scu[i + (((cu_height >> 2) - 1) * (cu_width >> 2))];
				map_cu_mode_line_hor[i] = map_cu_mode[i + (((cu_height >> 2) - 1) * (cu_width >> 2))];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_line_ver[i] = map_scu[((cu_width >> 2) - 1) + (i * (cu_width >> 2))];
				map_cu_mode_line_ver[i] = map_cu_mode[((cu_width >> 2) - 1) + (i * (cu_width >> 2))];
			}
			break;
		case 1:
			for (int i = 0; i < (cu_width >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_line_hor[i + (cu_width >> 2)] = map_scu[i + (((cu_height >> 2) - 1) * (cu_width >> 2))];
				map_cu_mode_line_hor[i + (cu_width >> 2)] = map_cu_mode[i + (((cu_height >> 2) - 1) * (cu_width >> 2))];
			}
			break;
		case 2:
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_line_ver[i] = map_scu[((cu_width >> 2) - 1) + (i * (cu_width >> 2))];
				map_cu_mode_line_ver[i] = map_cu_mode[((cu_width >> 2) - 1) + (i * (cu_width >> 2))];
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
	}
	void fetch_bi_hor(U3 part_num, ENC_ME_LINE_MAP_SIZE<SIZE * 2>* father, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_width >> 2) + 2; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
				map_scu_up_line[i] = father->map_scu_up_line[i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
				map_scu_left_line[i] = father->map_scu_left_line[i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[i];
			}
			break;
		case 1:
			map_scu_up_line[0] = father->map_scu_left_line[(cu_height >> 2) - 1];
			map_cu_mode_up_line[0] = father->map_cu_mode_left_line[(cu_height >> 2) - 1];
			for (int i = 0; i < (cu_width >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				map_scu_up_line[i + 1] = map_scu_line_hor[i];
				map_cu_mode_up_line[i + 1] = map_cu_mode_line_hor[i];
			}
			map_scu_up_line[(cu_width >> 2) + 1] = 0;
			map_cu_mode_up_line[(cu_width >> 2) + 1] = 0;
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				map_scu_left_line[i] = father->map_scu_left_line[(cu_height >> 2) + i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[(cu_height >> 2) + i];
			}
			break;
		default:
			assert(0);
		}
		min_scu_x = father->min_scu_x;
		min_scu_y = father->min_scu_y;
		above_line_idx = father->above_line_idx;
		curr_ctu_idx = father->curr_ctu_idx;
	}
	void fetch_bi_ver(U3 part_num, ENC_ME_LINE_MAP_SIZE<SIZE * 2>* father, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_width >> 2) + 2; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
				map_scu_up_line[i] = father->map_scu_up_line[i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				map_scu_left_line[i] = father->map_scu_left_line[i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[i];
			}
			break;
		case 1:
			for (int i = 0; i < (cu_width >> 2) + 2; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
				map_scu_up_line[i] = father->map_scu_up_line[(cu_width >> 2) + i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[(cu_width >> 2) + i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				map_scu_left_line[i] = map_scu_line_ver[i];
				map_cu_mode_left_line[i] = map_cu_mode_line_ver[i];
			}
			break;
		default:
			assert(0);
		}
		min_scu_x = father->min_scu_x;
		min_scu_y = father->min_scu_y;
		above_line_idx = father->above_line_idx;
		curr_ctu_idx = father->curr_ctu_idx;
	}
	void fetch_bi_hor(U3 part_num, ENC_ME_LINE_MAP_SIZE<SIZE>* father, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_width >> 2) + 2; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
				map_scu_up_line[i] = father->map_scu_up_line[i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = father->map_scu_left_line[i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[i];
			}
			break;
		case 1:
			map_scu_up_line[0] = father->map_scu_left_line[(cu_height >> 2) - 1];
			map_cu_mode_up_line[0] = father->map_cu_mode_left_line[(cu_height >> 2) - 1];
			for (int i = 0; i < (cu_width >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_up_line[i + 1] = map_scu_line_hor[i];
				map_cu_mode_up_line[i + 1] = map_cu_mode_line_hor[i];
			}
			map_scu_up_line[(cu_width >> 2) + 1] = 0;
			map_cu_mode_up_line[(cu_width >> 2) + 1] = 0;
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = father->map_scu_left_line[(cu_height >> 2) + i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[(cu_height >> 2) + i];
			}
			break;
		default:
			assert(0);
		}
		min_scu_x = father->min_scu_x;
		min_scu_y = father->min_scu_y;
		above_line_idx = father->above_line_idx;
		curr_ctu_idx = father->curr_ctu_idx;
	}
	void fetch_bi_ver(U3 part_num, ENC_ME_LINE_MAP_SIZE<SIZE>* father, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_width >> 2) + 2; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
				map_scu_up_line[i] = father->map_scu_up_line[i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = father->map_scu_left_line[i];
				map_cu_mode_left_line[i] = father->map_cu_mode_left_line[i];
			}
			break;
		case 1:
			for (int i = 0; i < (cu_width >> 2) + 2; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
				map_scu_up_line[i] = father->map_scu_up_line[(cu_width >> 2) + i];
				map_cu_mode_up_line[i] = father->map_cu_mode_up_line[(cu_width >> 2) + i];
			}
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
				map_scu_left_line[i] = map_scu_line_ver[i];
				map_cu_mode_left_line[i] = map_cu_mode_line_ver[i];
			}
			break;
		default:
			assert(0);
		}
		min_scu_x = father->min_scu_x;
		min_scu_y = father->min_scu_y;
		above_line_idx = father->above_line_idx;
		curr_ctu_idx = father->curr_ctu_idx;
	}
	void update_bi_hor(int part_num, u32* map_scu, U32* map_cu_mode, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_width >> 2); i++)
			{
#pragma HLS pipeline 
#pragma HLS LOOP_TRIPCOUNT max=16
				map_scu_line_hor[i] = map_scu[i + (((cu_height >> 2) - 1) * (cu_width >> 2))];
				map_cu_mode_line_hor[i] = map_cu_mode[i + (((cu_height >> 2) - 1) * (cu_width >> 2))];
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}
	void update_bi_ver(int part_num, u32* map_scu, U32* map_cu_mode, int cu_width, int cu_height)
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < (cu_height >> 2); i++)
			{
#pragma HLS pipeline 
#pragma HLS LOOP_TRIPCOUNT max=16
				map_scu_line_ver[i] = map_scu[((cu_width >> 2) - 1) + (i * (cu_width >> 2))];
				map_cu_mode_line_ver[i] = map_cu_mode[((cu_width >> 2) - 1) + (i * (cu_width >> 2))];
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
	}
};

typedef struct _ENC_ME_LINE_MAP_IME
{
	U32 new_map_scu_above_line[ABOVE_LINE_LEN];
	
	U11 min_scu_x;
	//U11 min_scu_y;
	
	S14 co_left_mv[SCU_IN_LCU_NUMS_LEN][MV_D];
	S2 co_left_refi[SCU_IN_LCU_NUMS_LEN];
	S16 co_left_ptr_ref[SCU_IN_LCU_NUMS_LEN];	
	

}ENC_ME_LINE_MAP_IME;
typedef struct _ENC_ME_LINE_MAP_DBK
{
	U32 new_map_scu_above_line[ABOVE_LINE_LEN];

	U11 min_scu_x;
	U11 min_scu_y;


}ENC_ME_LINE_MAP_DBK;

typedef struct _ENC_ME_LINE_MAP1
{
	U32 new_map_scu_above_line[ABOVE_LINE_LEN];
	U32 new_map_cu_mode_above_line[ABOVE_LINE_LEN];
	//U32* new_map_scu_above_line;

	//int min_scu;
	U11 min_scu_x;
	U11 min_scu_y;
	//U11 stride_in_map;
	//U5 stride_in_line;
	U11 pic_width_in_scu;
	//U11 pic_height_in_scu;
	U1 above_line_idx;
	U1 curr_ctu_idx;
	//U13 x_ctu;
	//U13 y_ctu;
	/* LCU split information */
	S8  line_map_split[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];

	U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	//U32 map_scu_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	//U32 map_cu_mode_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	//S14 co_left_mv[SCU_IN_LCU_NUMS_LEN][MV_D];
	//S2 co_left_refi[SCU_IN_LCU_NUMS_LEN];
	//S16 co_left_ptr_ref[SCU_IN_LCU_NUMS_LEN];
	/* buffer */
	//S2 refi_nebs[NEB_NUMS];
	//u32 scu_nebs[NEB_NUMS];
	//S14 mv_nebs[NEB_NUMS][MV_D];
	//U1 if_init;

}ENC_ME_LINE_MAP1;

typedef struct _ENC_ME_LINE_MAP2aec
{
	U32 min_scu_x;
	U32 min_scu_y;
	U32 above_line_idx;
	U32 curr_ctu_idx;
	U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_scu_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	U32 map_cu_mode_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];

}ENC_ME_LINE_MAP2aec;


typedef struct _ENC_LINE_MAP
{
	int min_scu_x;
	int min_scu_y;
	int curr_ctu_idx;
	bool is_coded[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	bool is_coded_above_line[MAX_WIDTH >> MIN_CU_LOG2];
}ENC_LINE_MAP;

struct ENC_MAP_DATA {
	u32 map_scu_inner[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	u32 map_cu_mode_inner[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
};

typedef struct _ME_MV_INFO
{
	s16 sadTree64x64Mv[1][1][2];
	s16 sadTree32x32Mv[2][2][2]; 
	s16 sadTree64x32Mv[1][2][2]; 
	s16 sadTree32x64Mv[2][1][2];
	s16 sadTree16x16Mv[4][4][2]; 
	s16 sadTree32x16Mv[2][4][2]; 
	s16 sadTree16x32Mv[4][2][2]; 
	s16 sadTree64x16Mv[1][4][2]; 
	s16 sadTree16x64Mv[4][1][2];
	s16 sadTree8x8Mv[8][8][2]; 
	s16 sadTree16x8Mv[4][8][2]; 
	s16 sadTree8x16Mv[8][4][2]; 
	s16 sadTree32x8Mv[2][8][2]; 
	s16 sadTree8x32Mv[8][2][2]; 
	s16 sadTree64x8Mv[1][8][2]; 
	s16 sadTree8x64Mv[8][1][2];
	//s16 sadTree4x8Mv[16][8][2]; 
	//s16 sadTree8x4Mv[8][16][2]; 
	//s16 sadTree16x4Mv[4][16][2]; 
	//s16 sadTree4x16Mv[16][4][2]; 
	//s16 sadTree32x4Mv[2][16][2]; 
	//s16 sadTree4x32Mv[16][2][2];
}ME_MV_INFO;

typedef struct _ME_PMV_INFO
{
	s16 sadTree64x64PMv[1][1][2];
	s16 sadTree32x32PMv[2][2][2]; 
	s16 sadTree64x32PMv[1][2][2]; 
	s16 sadTree32x64PMv[2][1][2];
	s16 sadTree16x16PMv[4][4][2]; 
	s16 sadTree32x16PMv[2][4][2]; 
	s16 sadTree16x32PMv[4][2][2]; 
	s16 sadTree64x16PMv[1][4][2]; 
	s16 sadTree16x64PMv[4][1][2];
	s16 sadTree8x8PMv[8][8][2]; 
	s16 sadTree16x8PMv[4][8][2]; 
	s16 sadTree8x16PMv[8][4][2]; 
	s16 sadTree32x8PMv[2][8][2]; 
	s16 sadTree8x32PMv[8][2][2]; 
	s16 sadTree64x8PMv[1][8][2]; 
	s16 sadTree8x64PMv[8][1][2];
	//s16 sadTree4x8PMv[16][8][2]; 
	//s16 sadTree8x4PMv[8][16][2]; 
	//s16 sadTree16x4PMv[4][16][2]; 
	//s16 sadTree4x16PMv[16][4][2]; 
	//s16 sadTree32x4PMv[2][16][2]; 
	//s16 sadTree4x32PMv[16][2][2];
}ME_PMV_INFO;

typedef struct _SKIP_REFP{
	int ptr;
	s16 map_mv[MAX_CU_IN_SCU][MAX_CU_IN_SCU][MV_D];
	s8  map_refi[MAX_CU_IN_SCU][MAX_CU_IN_SCU];
	u32 list_ptr;
} SKIP_REFP;

typedef struct _SKIP_MERGE_MAP_SCU{
	s32 map_mv[REFP_NUM][MV_D];
	s32 map_refi[REFP_NUM];
	u32 map_scu;
	u32 map_cu_mode;
} SKIP_MERGE_MAP_SCU;

struct _SKIP_MERGE_INPUT{
	u8 slice_type;
	int ptr;
	u8 umve_enable_flag;
	s16 refp_map_mv[MAX_CU_IN_SCU][MAX_CU_IN_SCU][MV_D];
	S2  refp_map_refi[MAX_CU_IN_SCU][MAX_CU_IN_SCU];
	U13 pix_x;
	//S64 s64_sqrt_lambda_y;
#if ENABLE_BFRAME
	s32 refp_map_mv1[REFP_NUM][MAX_CU_IN_SCU][MAX_CU_IN_SCU][MV_D];
	S32  refp_map_refi1[REFP_NUM][MAX_CU_IN_SCU][MAX_CU_IN_SCU][REFP_NUM];
	int refp_list_ptr1[REFP_NUM][MAX_NUM_REF_PICS];
	int refp_ptr1[MAX_NUM_REF_PICS][REFP_NUM];
#endif

	void cp(_SKIP_MERGE_INPUT * dst, _SKIP_MERGE_INPUT * src)
	{
		dst->slice_type       = src->slice_type;
		dst->ptr              = src->ptr;
		dst->umve_enable_flag = src->umve_enable_flag;
		dst->pix_x            = src->pix_x;
		for (int i=0; i<MAX_CU_IN_SCU; i++) {
			for (int j=0; j<MAX_CU_IN_SCU; j++) {
				for (int k=0; k<MV_D; k++) {
					dst->refp_map_mv[i][j][k] = src->refp_map_mv[i][j][k];
				}
				dst->refp_map_refi[i][j] = src->refp_map_refi[i][j];
			}
		}
		for (int m=0; m<REFP_NUM; m++) {
			for (int i=0; i<MAX_CU_IN_SCU; i++) {
				for (int j=0; j<MAX_CU_IN_SCU; j++) {
					for (int k=0; k<MV_D; k++) {
						dst->refp_map_mv1[m][i][j][k] = src->refp_map_mv1[m][i][j][k];
					}
					for (int k=0; k<REFP_NUM; k++) {
						dst->refp_map_refi1[m][i][j][k] = src->refp_map_refi1[m][i][j][k];
					}
				}
			}
			for (int i=0; i<MAX_NUM_REF_PICS; i++) {
				dst->refp_list_ptr1[m][i] = src->refp_list_ptr1[m][i];
				dst->refp_ptr1     [i][m] = src->refp_ptr1     [i][m];
			}
		}
	}

	friend void copy(_SKIP_MERGE_INPUT& _Left, _SKIP_MERGE_INPUT& _Right)
	{
		_Left.cp(&_Left, &_Right);
	}
};
typedef struct _SKIP_MERGE_INPUT SKIP_MERGE_INPUT;

struct SKIP_MERGE_INPUT_AXIS_IN{
    u8 buffer_skip_in_slice_type;
    int buffer_skip_in_ptr;
    u8 buffer_skip_in_umve_enable_flag;
    U13 buffer_skip_in_pix_x;
};

struct SKIP_MERGE_INPUT_16
{
	u8		slice_type;
	int		ptr;
	u8		umve_enable_flag;
	U13		pix_x;
	s16		refp_mv[MV_D];
	S2		refp_refi;
#if ENABLE_BFRAME
	s16		refp_map_mv[REFP_NUM][MV_D];
	S2		refp_map_refi[REFP_NUM][REFP_NUM];
	int		refp_list_ptr1[REFP_NUM][MAX_NUM_REF_PICS];
	int		refp_ptr1[MAX_NUM_REF_PICS][REFP_NUM];
#endif
};

#define M5422   0  //RL low_delay
#define FETCH_LUKE 1
#define CTU_CONTROL_LUKE 1
#define CTX_CONTROL_LUKE 1

#if FETCH_LUKE
//Fetch && IME
//**********************************************************************************************
#define MAX_CU_SIZE_FETCH             64    /* maxx CU size */
#define FENC_STRIDE    (MAX_CU_SIZE_FETCH         )
#define MIN_CU_SIZE_IN_BIT           3
#define SRW				    92//124//							
#define SRH                 60//124//
#define FENC_PADDING        4
#define SWW              (MAX_CU_SIZE_FETCH+2*SRW+2*FENC_PADDING)  //256
#define SWH              (MAX_CU_SIZE_FETCH+2*SRH+2*FENC_PADDING)  //192

#define	SKIP_COARSE_SEARCH			1	//LV2 OK! && LV1 OK!
#define	COARSE_SEARCH_DOUBLE_SPEED	0//1	//LV2 OK!

#define SCALE_QUARTER_PIXEL_LEVEL			2
#define SCALE_DOWNSAMPLE_LEVEL2				2
#define SCALE_DOWNSAMPLE_LEVEL1				1

#define MMEA_LEVEL2_SR_H	(SRH+FENC_PADDING)	//64
#define MMEA_LEVEL2_SR_V	(SRW+FENC_PADDING)	//92
#define MMEA_LEVEL1_SR_H	(MMEA_LEVEL2_SR_H>>1)	//32
#define MMEA_LEVEL1_SR_V	(MMEA_LEVEL2_SR_V>>1)	//48

//turn off skip affine
//#define SKIP_AFFINE_OFF		0	//1:turn on; 0:turn off
#ifndef SKIP_RESTRI
#define SKIP_RESTRI			1
#endif //SKIP_RESTRI

//#define REFW_SKIP_LEFT ((SWW-SWW)/2)
//#define REFW_SKIP_RIGHT (SWW-REFW_SKIP_LEFT)
//#define REFW_SKIP_TOP ((SWH-SWH)/2)
//#define REFW_SKIP_BOTTOM (SWW-REFW_SKIP_TOP)

#define SWW_SKIP 128//(SWW)//112//128//96//88
#define SWH_SKIP 96//(SWH)//32//88//96//88

#define REFW_SKIP_LEFT ((SWW-SWW_SKIP)/2)
#define REFW_SKIP_RIGHT (SWW-REFW_SKIP_LEFT)
#define REFW_SKIP_TOP ((SWH-SWH_SKIP)/2)
#define REFW_SKIP_BOTTOM (SWW-REFW_SKIP_TOP)


#define RANGE_MIN 0
#define RANGE_MAX 1


#define PIC_DIM 0
#define RW_DIM 1
//**********************************************************************************************

typedef struct __strFetch_Orig_Lcu {
	pel p_fenc_LCU_Y_rmd[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];//rmd

	//ime
	pel y_org[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel u_org[MAX_CU_SIZE_FETCH/2][MAX_CU_SIZE_FETCH/2];
	pel v_org[MAX_CU_SIZE_FETCH/2][MAX_CU_SIZE_FETCH/2];
} strFetch_Orig_Lcu;

typedef struct __strFetch_Orig_Lcu1 {
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2];
} strFetch_Orig_Lcu1;

typedef struct {

	pel y_org[64][64];
	pel u_org[32][32];
	pel v_org[32][32];
	/*pel y_orgT[64][64];

	pel y_org_lv1_2d[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	pel y_org_lv2_2d[MAX_CU_SIZE_FETCH >> 2][MAX_CU_SIZE_FETCH >> 2];
	*/

} strFetch_Orig_Lcu0;

typedef struct __strFetch_ref_window_COPY {

#if ENABLE_BFRAME
	int code_stru;
	U1 lidx[REFP_NUM];
	S8 cur_refi[REFP_NUM];
	int refp_pic_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	int refp_is_library_picture[MAX_NUM_REF_PICS][REFP_NUM];
#endif


} strFetch_ref_window_COPY;

typedef struct __strFetch_ref_window {
	/*U13 ref_left;
	U13 ref_right;
	U13 ref_up;
	U13 ref_down;*/
	pel RefWindowLCU[SWW*SWH];
	pel swOutLv0[SWH][SWW];
	//U8 RefWindowLCU_2D[SWH][SWW];
	//U8 RefWindowLCU_U[SWH / 2][SWW / 2];
	//U8 RefWindowLCU_V[SWH / 2][SWW / 2];
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//S14 CtrPos_IME[RANGE_NUM][DIR_DIM][MAP_DIMS];
	pel RefWindowLCU_UP[(SWH * SWW)>> 2];
	pel RefWindowLCU_VP[(SWH * SWW) >>2];
	//U8 swOutLv0[320][320];
	//U8 swOutLv1[160][160];
	//U8 swOutLv2[80][80];
	//U12 x_chroma;
	//U12 y_chroma;
	//U1 last_child;

#if ENABLE_BFRAME
	int code_stru;
	U1 lidx[REFP_NUM];
	S8 cur_refi[REFP_NUM];
	int refp_pic_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	int refp_is_library_picture[MAX_NUM_REF_PICS][REFP_NUM];

	S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	pel RefWindowLCU_ref1[SWW*SWH];
	pel swOutLv0_ref1[SWH][SWW];
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2];
#endif

	friend void copy_idx(__strFetch_ref_window& _Left, const  __strFetch_ref_window& _Right)
	{
		for (int i = 0; i < REFP_NUM; ++i)
		{
			_Left.lidx[i] = _Right.lidx[i];
			_Left.cur_refi[i] = _Right.cur_refi[i];
			for (int j = 0; j < MAX_NUM_REF_PICS; ++j) {
				_Left.refp_pic_ptr[j][i] = _Right.refp_pic_ptr[j][i];
			}
		}
		_Left.code_stru = _Right.code_stru;
	}
} strFetch_ref_window;
typedef struct __strFetch_ref_window_16 {
#if ENABLE_BFRAME
	int code_stru;
	U1 lidx[REFP_NUM];
	S8 cur_refi[REFP_NUM];
	int refp_pic_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	int refp_is_library_picture[MAX_NUM_REF_PICS][REFP_NUM];
#endif
} strFetch_ref_window_16;


typedef struct __strFetch_ref_window1 {
	/*U13 ref_left;
	U13 ref_right;
	U13 ref_up;
	U13 ref_down;*/
	pel RefWindowLCU[SWW * SWH];
	pel refwin_md[88][88];
	//	U8 RefWindowLCU_2D[SWH][SWW];
	//	U8 RefWindowLCU_U[SWH / 2][SWW / 2];
	//	U8 RefWindowLCU_V[SWH / 2][SWW / 2];
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP[(SWH * SWW) >> 2];
	//	U8 swOutLv0[320][320];
	//	U8 swOutLv0_fme[320][320];
	//	U8 swOutLv1[160][160];
	//	U8 swOutLv2[80][80];
	//	U12 x_chroma;
	//	U12 y_chroma;
		//U1 last_child;


} strFetch_ref_window1;

typedef struct
{
	/* Start address of Y component (except padding) */
	pel imgY_org[3840 * 2160];
	pel imgU_org[1920 * 1080];
	pel imgV_org[1920 * 1080];
	pel RefbufY[(3840 + 144 * 2) * (2160 + 144 * 2)];
	pel RefbufU[(1920 + 72 * 2) * (1080 + 72 * 2)];
	pel RefbufV[(1920 + 72 * 2) * (1080 + 72 * 2)];
#if ENABLE_BFRAME
	pel RefbufY1[(3840 + 144 * 2) * (2160 + 144 * 2)];
	pel RefbufU1[(1920 + 72 * 2) * (1080 + 72 * 2)];
	pel RefbufV1[(1920 + 72 * 2) * (1080 + 72 * 2)];
#endif
} strFetch_input_video_U8;

typedef struct
{
	/* Start address of Y component (except padding) */

	pel* imgY_org;
	pel* imgU_org;
	pel* imgV_org;
	pel* RefbufY;
	pel* RefbufU;
	pel* RefbufV;
	pel*  y_rfcBin;
	u16* y_rfcPrm;
	pel*  u_rfcBin;
	u16* u_rfcPrm;
	pel*  v_rfcBin;
	u16* v_rfcPrm;

#if ENABLE_BFRAME
	int code_stru;
	pel* y_rfcBin1;
	u16* y_rfcPrm1;
	pel* u_rfcBin1;
	u16* u_rfcPrm1;
	pel* v_rfcBin1;
	u16* v_rfcPrm1;

	pel* RefbufY1;
	pel* RefbufU1;
	pel* RefbufV1;
#endif
} strFetch_input_video_U8_ptr;
typedef struct
{
	/* Start address of Y component (except padding) */
	pel* imgY_org;
	pel* imgU_org;
	pel* imgV_org;
	pel* RefbufY;
	pel* RefbufU;
	pel* RefbufV;
	
	
	//pel             imgY_org[3840 * 2160];
	//pel				imgY_org_64x64[(3840 / 64) * 34][64][64];
	/* Start address of U component (except padding)  */
	//pel             imgU_org[1920 * 1080];
	//pel				imgU_org_32x32[(1920 / 64) * 17][32][32];
	/* Start address of V component (except padding)  */
	//pel             imgV_org[1920 * 1080];
	//pel				imgV_org_32x32[(1920 / 64) * 17][32][32];

	//pel RefbufY[(3840 + 144 * 2) * (2160 + 144 * 2)];//3840+144*2//2160+144*2////////(REFP_SURD_OFF_H)*(width + ((1 << 7) + 16) * 2) - (REFP_SURD_OFF_W)   (stride+1)*padding_size
	//pel* RefbufY1;
	//pel RefbufU[(1920 + 72 * 2) * (1080 + 72 * 2)];//
	//pel RefbufV[(1920 + 72 * 2) * (1080 + 72 * 2)];

} strFetch_input_video;

/*ctu_ctrl to hardware modules*/
typedef struct {
	U12 pix_x;
	U12 pix_y;

	U12 rmd_pix_x;
	U12 rmd_pix_y;

	U12 dbk_pix_x;
	U12 dbk_pix_y;

	U12 sao_pix_x;
	U12 sao_pix_y;
#if CTU_CONTROL_LUKE
	//int  LCU_nr;
	/* total count of remained LCU for encoding one picture. if a picture is
	encoded properly, this value should reach to zero */
	int  lcu_cnt;
	int  lcu_num;

	int  aec_lcu_cnt;
	int  aec_lcu_num;
	int  dbk_lcu_num;
#endif
	//int  smb_num_width;
	//int  smb_num_height;
#if CTU_CONTROL_LUKE
	
	/* QP for luma of current encoding CU */
	int             qp_y;
	/* QP for chroma of current encoding CU */
	int             qp_u;
	int             qp_v;
	u32             lambda_mv;
	u32             rmd_lambda_mv;
	s64             lambda[3];
	s64             lambda_y;
	s64             lambda_u;
	s64             lambda_v;
	//s64             sqrt_lambda[3];
	s64          dist_chroma_weight[2];
	/* current frame numbser */
	//int             ptr;
	int				x_last_ctu;
	int				y_last_ctu;
#endif
	int delta_qp;
	int aec_delta_qp;
//	ENC_ME_LINE_MAP *me_line_map_ptr;
#if USE_INTRA_REFRESH
	U1 EncRefresh;
	U1 RefreshMode;
	U8 RefreshSize;
	U8 pirStartRow;
	U8 pirEndRow;
	U8 pirStartCol;
	U8 pirEndCol;
	U9 numCuInWidth;
	U9 numCuInHeight;
#endif
#if USE_SPEED_LEVEL
	U8 speed_level;
#endif
#if USE_ROI_QP_CONFIG
	int use_ROI_qp1;
	int ROI_qp_Start_x1;
	int ROI_qp_Start_y1;
	int ROI_qp_End_x1;
	int ROI_qp_End_y1;
	int ROI_QP1;

	int use_ROI_qp2;
	int ROI_qp_Start_x2;
	int ROI_qp_Start_y2;
	int ROI_qp_End_x2;
	int ROI_qp_End_y2;
	int ROI_QP2;

	int use_ROI_qp3;
	int ROI_qp_Start_x3;
	int ROI_qp_Start_y3;
	int ROI_qp_End_x3;
	int ROI_qp_End_y3;
	int ROI_QP3;

	int use_ROI_qp4;
	int ROI_qp_Start_x4;
	int ROI_qp_Start_y4;
	int ROI_qp_End_x4;
	int ROI_qp_End_y4;
	int ROI_QP4;

	int use_ROI_qp5;
	int ROI_qp_Start_x5;
	int ROI_qp_Start_y5;
	int ROI_qp_End_x5;
	int ROI_qp_End_y5;
	int ROI_QP5;

	int use_ROI_qp6;
	int ROI_qp_Start_x6;
	int ROI_qp_Start_y6;
	int ROI_qp_End_x6;
	int ROI_qp_End_y6;
	int ROI_QP6;

	int use_ROI_qp7;
	int ROI_qp_Start_x7;
	int ROI_qp_Start_y7;
	int ROI_qp_End_x7;
	int ROI_qp_End_y7;
	int ROI_QP7;

	int use_ROI_qp8;
	int ROI_qp_Start_x8;
	int ROI_qp_Start_y8;
	int ROI_qp_End_x8;
	int ROI_qp_End_y8;
	int ROI_QP8;

#endif

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	int useQPMAP;
	int useModeMAP;
	int useQPMAPCon;
	int useModeMAPCon;
#endif

#if USE_ROI_MODE_CONFIG
	U1  use_ROI_Mode1;
	U12 ROI_Mode_Start_x1;
	U12 ROI_Mode_Start_y1;
	U12 ROI_Mode_End_x1;
	U12 ROI_Mode_End_y1;
	U3  ROI_Mode1;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode2;
	U12 ROI_Mode_Start_x2;
	U12 ROI_Mode_Start_y2;
	U12 ROI_Mode_End_x2;
	U12 ROI_Mode_End_y2;
	U3  ROI_Mode2;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode3;
	U12 ROI_Mode_Start_x3;
	U12 ROI_Mode_Start_y3;
	U12 ROI_Mode_End_x3;
	U12 ROI_Mode_End_y3;
	U3  ROI_Mode3;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode4;
	U12 ROI_Mode_Start_x4;
	U12 ROI_Mode_Start_y4;
	U12 ROI_Mode_End_x4;
	U12 ROI_Mode_End_y4;
	U3  ROI_Mode4;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode5;
	U12 ROI_Mode_Start_x5;
	U12 ROI_Mode_Start_y5;
	U12 ROI_Mode_End_x5;
	U12 ROI_Mode_End_y5;
	U3  ROI_Mode5;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode6;
	U12 ROI_Mode_Start_x6;
	U12 ROI_Mode_Start_y6;
	U12 ROI_Mode_End_x6;
	U12 ROI_Mode_End_y6;
	U3  ROI_Mode6;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode7;
	U12 ROI_Mode_Start_x7;
	U12 ROI_Mode_Start_y7;
	U12 ROI_Mode_End_x7;
	U12 ROI_Mode_End_y7;
	U3  ROI_Mode7;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode8;
	U12 ROI_Mode_Start_x8;
	U12 ROI_Mode_Start_y8;
	U12 ROI_Mode_End_x8;
	U12 ROI_Mode_End_y8;
	U3  ROI_Mode8;//MSB~LSB  intra inter skip  default value: 3'b111

	U1 valid_mode[3];//valid_mode[0] skip/merge/direct;valid_mode[1] inter;valid_mode[2] intra
#endif

#if USE_TRACE_DUMP
	int useTraceDump;
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int md_slice_qp;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	int useCuQPMAP;
	int CuQPMap[256];
#endif
} CTU_Level_Parameters;
#endif

#if CTU_CONTROL_LUKE
/*firmware to ctu_ctrl*/
typedef struct {
	int width;
	int height;
	//unsigned char   chroma_format;
	//unsigned char   slice_lcu_y;
	//unsigned char   slice_lcu_num;
	unsigned int   f_lcu;
	unsigned char   slice_enable;
	//unsigned char end_of_picture;
	/* QP for luma of current encoding CU */
	int             qp_y;
	/* QP for chroma of current encoding CU */
	int             qp_u;
	int             qp_v;
	u32             lambda_mv;
	U32             lambda[3];
	U32             sqrt_lambda[3];
	U32             dist_chroma_weight[2][2];

#if EIPM
	int              eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
	/*U32             lambda[3][2];
	U32             sqrt_lambda[3][2];
	U32             dist_chroma_weight[2][2];*/

	/* current frame numbser */
	int             ptr;
	/* gop size */
	int             gop_size;
#if CTX_CONTROL_LUKE
	u8               slice_type;
	u8               num_of_hmvp_cand;
	int              bit_depth_internal;
	int              ipf_enable_flag;
	u8               max_part_ratio;
	int              pic_width_in_scu;
	int              pic_height_in_scu;
	int              f_scu;
	int              log2_max_cuwh;
	u8               min_cu_size;
	u8               max_split_times;
	u8               min_qt_size;
	u8               max_bt_size;
	u8               max_eqt_size;
	int				 affine_subblock_size_idx;
	int				 ipcm_enable_flag;
	//u8				 position_based_transform_enable_flag;
	//int              secondary_transform_enable_flag;
	u8               tscpm_enable_flag;
	int				 amvr_enable_flag;
	int				 emvr_enable_flag;
	int				 affine_enable_flag;
#endif
	//#if CTU_RATE_CONTROL
	int fixed_slice_qp_flag;
	int last_lcu_qp;
	int delta_qp;
	int depth;
	int slice_qp;
#if USE_INTRA_REFRESH
	U1 EncRefresh;
	U1 RefreshMode;
	U8 RefreshSize;
	U8 pirStartRow;
	U8 pirEndRow;
	U8 pirStartCol;
	U8 pirEndCol;
#endif
#if USE_SPEED_LEVEL
	U8 speed_level;
#endif
#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	int useQPMAP;
	int useModeMAP;
	int useQPMAPCon;
	int useModeMAPCon;
#endif
#if USE_TRACE_DUMP
	int useTraceDump;
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	int md_slice_qp;
	int useCuQPMAP;
	int CuQPMap[256];
	unsigned int CuQPMap_Frame[1920*1080];
#endif
//#endif
} Slice_Level_Parameters;
#endif

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
typedef struct {
	U5 reserved_bits;
	U1 use_ROI_qp;
	U6 ROI_qp;
	U1 use_ROI_Mode;
	U3 ROI_Mode;
} MapLcu;

typedef struct {
	U1 useModeMAP;
	U3 ModeMAP;
} ModeMAP_param;
#endif

#if LIBVC_ON
#define MAX_NUM_COMPONENT                   3
#define MAX_NUM_LIBPIC                      16
#define MAX_CANDIDATE_PIC                   256
#define MAX_DIFFERENCE_OF_RLPIC_AND_LIBPIC  1.7e+308; ///< avs3_max. value of Double-type value

typedef struct _PIC_HIST_FEATURE
{
    int num_component;
    int num_of_hist_interval;
    int length_of_interval;
    int * list_hist_feature[MAX_NUM_COMPONENT];
} PIC_HIST_FEATURE;

// ====================================================================================================================
// Information of libpic
// ====================================================================================================================

typedef struct _LibVCData
{
    int num_candidate_pic;
    int list_poc_of_candidate_pic[MAX_CANDIDATE_PIC]; // for now, the candidate set contain only pictures from input sequence.
    COM_IMGB * list_candidate_pic[MAX_CANDIDATE_PIC];
    PIC_HIST_FEATURE list_hist_feature_of_candidate_pic[MAX_CANDIDATE_PIC];

    int num_lib_pic;
    int list_poc_of_libpic[MAX_NUM_LIBPIC];

    COM_PIC * list_libpic_outside[MAX_NUM_LIBPIC];
    int num_libpic_outside;
    int list_library_index_outside[MAX_NUM_LIBPIC];

    int num_RLpic;
    int list_poc_of_RLpic[MAX_CANDIDATE_PIC];
    int list_libidx_for_RLpic[MAX_CANDIDATE_PIC]; //the idx of library picture instead of poc of library picture in origin sequence is set as new poc.

    double bits_dependencyFile;
    double bits_libpic;

#if M5422
	long long  sum_intra_var;
	long long  sum_inter_var;
	int end_of_intra_period;
	int last_libpic_index;
	int i_period;
#endif

    int library_picture_enable_flag;
    int is_libpic_processing;
    int is_libpic_prepared;

} LibVCData;
#endif

/*****************************************************************************
 * picture buffer allocator
 *****************************************************************************/
typedef struct _PICBUF_ALLOCATOR PICBUF_ALLOCATOR;
struct _PICBUF_ALLOCATOR
{
    /* width */
    int              width;
    /* height */
    int              height;
    /* pad size for luma */
    int              pad_l;
    /* pad size for chroma */
    int              pad_c;
    /* arbitrary data, if needs */
    int              ndata[4];
    /* arbitrary address, if needs */
    int            *pdata[4];
};

/*****************************************************************************
 * picture manager for DPB in decoder and RPB in encoder
 *****************************************************************************/
typedef struct _COM_PM
{
    /* picture store (including reference and non-reference) */
    COM_PIC          *pic[MAX_PB_SIZE];
    /* address of reference pictures */
    COM_PIC          *pic_ref[MAX_NUM_REF_PICS];
    /* maximum reference picture count */
    int               max_num_ref_pics;
    /* current count of available reference pictures in PB */
    int               cur_num_ref_pics;
    /* number of reference pictures */
    int               num_refp[REFP_NUM];
    /* next output POC */
    int               ptr_next_output;
    /* POC increment */
    int               ptr_increase;
    /* avs3_max number of picture buffer */
    int               max_pb_size;
    /* current picture buffer size */
    int               cur_pb_size;
    /* address of leased picture for current decoding/encoding buffer */
    COM_PIC          *pic_lease;
    /* picture buffer allocator */
    PICBUF_ALLOCATOR  pa;
#if LIBVC_ON
    /* information for LibVC */
    LibVCData        *libvc_data;
    int               is_library_buffer_empty;
    COM_PIC          *pb_libpic; //buffer for libpic
    int               cur_libpb_size;
    int               pb_libpic_library_index;
#endif
} COM_PM;

/* reference picture structure */
typedef struct _COM_REFP
{
    /* address of reference picture */
    COM_PIC         *pic;
    /* PTR of reference picture */
    int              ptr;
	s32	(*map_mv)[REFP_NUM][MV_D];//s16            (*map_mv)[REFP_NUM][MV_D];
	s32	(*map_refi)[REFP_NUM];//s8             (*map_refi)[REFP_NUM];
    u32             *list_ptr;
#if LIBVC_ON
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
    u32             *list_is_library_pic;
#endif
    int              is_library_picture;
#endif
} COM_REFP;

/*****************************************************************************
 * chunk header
 *****************************************************************************/
typedef struct _COM_CNKH
{
    /* version: 3bit */
    int              ver;
    /* chunk type: 4bit */
    int              ctype;
    /* broken link flag: 1bit(should be zero) */
    int              broken;
} COM_CNKH;

/*****************************************************************************
 * sequence header
 *****************************************************************************/
typedef struct _COM_SQH
{
	u8               profile_id;                 /* 8 bits */
	u8               level_id;                   /* 8 bits */
	u8               progressive_sequence;       /* 1 bit  */
	u8               field_coded_sequence;       /* 1 bit  */
#if LIBVC_ON
	u8               library_stream_flag;     /* 1 bit  */
	u8               library_picture_enable_flag;     /* 1 bit  */
	u8               duplicate_sequence_header_flag;     /* 1 bit  */
#endif
	u8               chroma_format;              /* 2 bits */
	u8               encoding_precision;         /* 3 bits */
	u8               output_reorder_delay;       /* 5 bits */
	u8               sample_precision;           /* 3 bits */
	u8               aspect_ratio;               /* 4 bits */
	u8               frame_rate_code;            /* 4 bits */
	u32              bit_rate_lower;             /* 18 bits */
	u32              bit_rate_upper;             /* 18 bits */
	u8               low_delay;                  /* 1 bit */
	u8               temporal_id_enable_flag;    /* 1 bit */
	u32              bbv_buffer_size;            /* 18 bits */
	u8               max_dpb_size;               /* 4 bits */
	int              horizontal_size;            /* 14 bits */
	int              vertical_size;              /* 14 bits */
	u8               log2_max_cu_width_height;   /* 3 bits */
	u8               min_cu_size;
	u8               max_part_ratio;
	u8               max_split_times;
	u8               min_qt_size;
	u8               max_bt_size;
	u8               max_eqt_size;
	u8               max_dt_size;
#if HLS_RPL
	int              rpl1_index_exist_flag;
	int              rpl1_same_as_rpl0_flag;
	COM_RPL          rpls_l0[MAX_NUM_RPLS];
	COM_RPL          rpls_l1[MAX_NUM_RPLS];
	int              rpls_l0_num;
	int              rpls_l1_num;
	int              num_ref_default_active_minus1[2];
#endif
#if IPCM
	int              ipcm_enable_flag;
#endif
	u8               amvr_enable_flag;
#if IBC_ABVR
	u8               abvr_enable_flag;
#endif
#if USE_SP
	int              isc_enable_flag;
#endif
	int              umve_enable_flag;
#if UMVE_ENH
	int              umve_enh_enable_flag;
#endif
#if OBMC
	int              obmc_enable_flag;
#endif
#if AWP
	int              awp_enable_flag;
#endif
#if AWP_MVR
	int              awp_mvr_enable_flag;
#endif
#if ETMVP
	int              etmvp_enable_flag;
#endif
#if SUB_TMVP
	int              sbtmvp_enable_flag;
#endif
#if SH_UNIFY
#if ETMVP || MVAP
	int              emvp_enable_flag;
#endif
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if DBK_SCC
	int              loop_filter_type_enable_flag;
#endif
#if DBR
	int              dbr_enable_flag;
#endif
	int              ipf_enable_flag;
#if EXT_AMVR_HMVP
	int              emvr_enable_flag;
#endif
	u8               affine_enable_flag;
#if ASP
	u8               asp_enable_flag;
#endif
#if AFFINE_UMVE
	u8               affine_umve_enable_flag;
#endif
#if SMVD
	u8               smvd_enable_flag;
#endif
#if BIO
	u8               bio_enable_flag;
#endif
#if BGC
	u8               bgc_enable_flag;
#endif
#if DMVR
	u8               dmvr_enable_flag;
#endif
#if INTERPF
	u8               interpf_enable_flag;
#endif
#if IPC
	u8               ipc_enable_flag;
#endif
#if MVAP
	u8               mvap_enable_flag;
	u8               num_of_mvap_cand;
#endif
#if DT_PARTITION
	u8               dt_intra_enable_flag;
#endif
	u8               num_of_hmvp_cand;
#if IBC_BVP
	u8               num_of_hbvp_cand;
#endif
#if TSCPM
	u8               tscpm_enable_flag;
#if ENHANCE_TSPCM
	u8               enhance_tscpm_enable_flag;
#endif
#if PMC
	u8               pmc_enable_flag;
#endif
#endif
#if IPF_CHROMA
	u8               chroma_ipf_enable_flag;
#endif
#if IIP
	u8               iip_enable_flag;
#endif
#if SAWP
	u8               sawp_enable_flag;
#endif // SAWP
#if FIMC
	u8               fimc_enable_flag;
#endif
#if IST
	u8               ist_enable_flag;
#endif
#if ISTS
	u8               ists_enable_flag;
#endif
#if TS_INTER
	u8               ts_inter_enable_flag;
#endif
#if EST
	u8               est_enable_flag;
#endif
#if ST_CHROMA
	u8               st_chroma_enable_flag;
#endif
#if SRCC
	u8               srcc_enable_flag;
#endif
#if CABAC_MULTI_PROB
	u8               mcabac_enable_flag;
#endif
	u8               eipm_enable_flag;
	u8               sample_adaptive_offset_enable_flag;
#if ESAO
	u8               esao_enable_flag;
#endif 
#if CCSAO
	u8               ccsao_enable_flag;
#endif 
#if ALF_SHAPE || ALF_IMP || ALF_SHIFT
	u8               adaptive_leveling_filter_enhance_flag;
#endif
	u8               adaptive_leveling_filter_enable_flag;
	u8               secondary_transform_enable_flag;
	u8               position_based_transform_enable_flag;
#if SBT
	u8               sbt_enable_flag;
#endif

	u8               wq_enable;
	u8               seq_wq_mode;
	u8               wq_4x4_matrix[16];
	u8               wq_8x8_matrix[64];
#if PATCH
	u8               patch_stable;
	u8               cross_patch_loop_filter;
	u8               patch_ref_colocated;
	u8               patch_uniform;
	u8               patch_width_minus1;
	u8               patch_height_minus1;
	u8               patch_columns;
	u8               patch_rows;
	int              column_width[64];
	int              row_height[32];
#endif
#if USE_IBC
	u8               ibc_flag;
#endif
#if ESAO
	u8              pic_esao_on[3];
	u8              esao_adaptive_param[3];
	u8              esao_lcu_enable[3];
#endif 
#if NN_HOOK
	u8              nn_tools_set_hook;
#if NN_FILTER
	u8              nnlf_enable_flag;
	u8              num_of_nnlf;
#endif
#endif
} COM_SQH;


/*****************************************************************************
 * picture header
 *****************************************************************************/
 typedef struct _COM_PIC_HEADER
 {
	 /* info from sqh */
	 u8  low_delay;
	 /* decoding temporal reference */
#if HLS_RPL
	 s32 poc;
	 int              rpl_l0_idx;         //-1 means this slice does not use RPL candidate in SPS for RPL0
	 int              rpl_l1_idx;         //-1 means this slice does not use RPL candidate in SPS for RPL1
	 COM_RPL          rpl_l0;
	 COM_RPL          rpl_l1;
	 u8               num_ref_idx_active_override_flag;
	 u8               ref_pic_list_sps_flag[2];
#endif
	 u32              dtr;
	 u8               slice_type;
	 u8               temporal_id;
	 u8               loop_filter_disable_flag;
	 u32              bbv_delay;
	 u16              bbv_check_times;

#if DBK_SCC
	 u8               loop_fitler_type;
#endif
	 u8               loop_filter_parameter_flag;
	 int              alpha_c_offset;
	 int              beta_offset;

	 u8               chroma_quant_param_disable_flag;
	 s8               chroma_quant_param_delta_cb;
	 s8               chroma_quant_param_delta_cr;

	 /*Flag and coeff for ALF*/
	 int              tool_alf_on;
#if ALF_SHAPE || ALF_IMP || ALF_SHIFT
	 int              tool_alf_enhance_on;
#endif
	 //int* pic_alf_on;
#if USE_ENH
	 //ALF_PARAM** alf_picture_param;
#endif
#if DBR
	 DBR_PARAM        ph_dbr_param;
#endif

	 int              fixed_picture_qp_flag;
#if CUDQP
	 u8               cu_delta_qp_flag;
	 u8               cu_qp_group_size;
	 int              cu_qp_group_area_size;
#endif
	 int              random_access_decodable_flag;
	 int              time_code_flag;
	 int              time_code;
	 int              decode_order_index;
	 int              picture_output_delay;
	 int              progressive_frame;
	 int              picture_structure;
	 int              top_field_first;
	 int              repeat_first_field;
	 int              top_field_picture_flag;
	 int              picture_qp;
	 int              affine_subblock_size_idx;
#if LIBVC_ON
	 int              is_RLpic_flag;  // only reference libpic
	 int              library_picture_index;
#endif
#if UMVE_ENH
	 u8               umve_set_flag;
#endif

	 int              pic_wq_enable;
	 int              pic_wq_data_idx;
	 int              wq_param;
	 int              wq_model;
	 int              wq_param_vector[6];
	 u8               wq_4x4_matrix[16];
	 u8               wq_8x8_matrix[64];
#if USE_IBC
	 u8               ibc_flag;
#endif
#if USE_SP
	 u8               sp_pic_flag;
	 u8               evs_ubvs_pic_flag;
#endif
#if FIMC
	 u8               fimc_pic_flag;
#endif
#if ISTS
	 u8               ph_ists_enable_flag;
#endif
#if TS_INTER
	 u8               ph_ts_inter_enable_flag;
#endif
#if AWP
	 u8               ph_awp_refine_flag;
#endif
#if AWP || FAST_LD
	 int              ph_poc[REFP_NUM][MAX_NUM_REF_PICS];
#endif
#if ENC_ME_IMP
	 BOOL             is_lowdelay;
#endif
#if FAST_LD
	 int              l1idx_to_l0idx[MAX_NUM_REF_PICS];
#endif

#if OBMC
	 u8               obmc_blending_flag;
#endif
#if EPMC
	 u8               ph_epmc_model_flag;
#endif
#if IPC
	 u8               ph_ipc_flag;
#endif
#if ESAO
	 u8               pic_esao_on[N_C];
#if ESAO_ENH
	 u8               esao_set_num[N_C];
	 u8               esao_adaptive_param[N_C][ESAO_SET_NUM];
	 u8               esao_luma_type[ESAO_SET_NUM];            //0:17 1 :9
	 u8               esao_lcu_enable[N_C];      //lcu mode control flag
	 int              esao_chroma_band_length[N_C - 1][ESAO_SET_NUM];
	 int              esao_chroma_start_band[N_C - 1][ESAO_SET_NUM];
	 int              esao_chroma_band_flag[N_C - 1][ESAO_SET_NUM];
#else
	 u8               esao_adaptive_param[N_C];
	 u8               esao_luma_type;
	 u8               esao_lcu_enable[N_C];      //lcu mode control flag
	 int              esao_chroma_band_length[N_C - 1];
	 int              esao_chroma_start_band[N_C - 1];
	 int              esao_chroma_band_flag[N_C - 1];
#endif
#if ESAO_PH_SYNTAX
	 //ESAO_BLK_PARAM* pic_esao_params;
#endif
#endif 
#if CCSAO
	 u8               pic_ccsao_on[N_C - 1];
	 u8               ccsao_lcu_ctrl[N_C - 1];
#if CCSAO_ENHANCEMENT
	 u8               ccsao_set_num[N_C - 1];
	 u8               ccsao_type[N_C - 1][CCSAO_SET_NUM];
	 u8               ccsao_band_num[N_C - 1][CCSAO_SET_NUM];
	 u8               ccsao_band_num_c[N_C - 1][CCSAO_SET_NUM];
#if ECCSAO
	 u8               ccsao_class_type[N_C - 1][CCSAO_SET_NUM];
#endif
#else
	 u8               ccsao_type[N_C - 1];
	 u8               ccsao_band_num[N_C - 1];
#endif
#if CCSAO_PH_SYNTAX
	 //CCSAO_BLK_PARAM* pic_ccsao_params;
#endif
#endif
#if NN_FILTER
	 u8               ph_nnlf_enable_flag[N_C];
	 u8               ph_nnlf_adaptive_flag[N_C];
	 u8               ph_nnlf_set_index[N_C];
	 //u8* nnlf_lcu_enable_flag[N_C];
	 //u8* nnlf_lcu_set_index[N_C];
#endif
 } COM_PIC_HEADER;

typedef struct _COM_SH_EXT
{
    u8               slice_sao_enable[N_C];
    u8               fixed_slice_qp_flag;
    u8               slice_qp;
} COM_SH_EXT;

#if TB_SPLIT_EXT
typedef struct _COM_PART_INFO
{
    u8 num_sub_part;
    U14 sub_x[MAX_NUM_PB]; //sub part x, y, w and h
	U14 sub_y[MAX_NUM_PB];
    U7 sub_w[MAX_NUM_PB];
    U7 sub_h[MAX_NUM_PB];
    U23 sub_scup[MAX_NUM_PB];
} COM_PART_INFO;
#endif

/*****************************************************************************
 * user data types
 *****************************************************************************/
#define COM_UD_PIC_SIGNATURE              0x10
#define COM_UD_END                        0xFF


/*****************************************************************************
* mode decision structure
*****************************************************************************/
typedef struct _COM_MODE
{
	/* CU position X in a frame in SCU unit */
	int            x_scu;
	/* CU position Y in a frame in SCU unit */
	int            y_scu;

	/* depth of current CU */
	int            cud;

	/* width of current CU */
	int            cu_width;
	/* height of current CU */
	int            cu_height;
	/* log2 of cu_width */
	int             cu_width_log2;
	/* log2 of cu_height */
	int             cu_height_log2;
	/* position of CU */
	int            x_pos;
	int            y_pos;
	/* CU position in current frame in SCU unit */
	int            scup;

	int  cu_mode;
#if TB_SPLIT_EXT
	//note: DT can apply to intra CU and only use normal amvp for inter CU (no skip, direct, amvr, affine, hmvp)
	int  pb_part;
	int  tb_part;
	//COM_PART_INFO  pb_info;
	//COM_PART_INFO  tb_info;
#endif

	//pel  rec[N_C][MAX_CU_DIM];
	pel  rec_y[MAX_CU_DIM];
	pel  rec_u[MAX_CU_DIM >> 2];
	pel  rec_v[MAX_CU_DIM >> 2];
	//s16  coef[N_C][MAX_CU_DIM];
	s16  coef_y[MAX_CU_DIM];
	s16  coef_u[MAX_CU_DIM >> 2];
	s16  coef_v[MAX_CU_DIM >> 2];

	// pel  pred[N_C][MAX_CU_DIM];
	pel  pred_y[MAX_CU_DIM];
	pel  pred_u[MAX_CU_DIM >> 2];
	pel  pred_v[MAX_CU_DIM >> 2];
#if TB_SPLIT_EXT
	int  num_nz[MAX_NUM_TB][N_C];
#else
	int  num_nz[N_C];
#endif

	/* reference indices */
	s8   refi[REFP_NUM];

	/* MVR indices */
	u8   mvr_idx;
	u8   umve_flag;
	u8   umve_idx;
	u8   skip_idx;
#if EXT_AMVR_HMVP
	u8   mvp_from_hmvp_flag;
#endif

	/* mv difference */
	s16  mvd[REFP_NUM][MV_D];
	/* mv */
	s16  mv[REFP_NUM][MV_D];

	u8   affine_flag;
	CPMV affine_mv[REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd[REFP_NUM][VER_NUM][MV_D];
#if SMVD
	u8   smvd_flag;
#endif

	/* intra prediction mode */
#if TB_SPLIT_EXT
	u8   mpm[MAX_NUM_PB][2];
	s8   ipm[MAX_NUM_PB][2];
#else
	u8   mpm[2];
	s8   ipm[2];
#endif
	u8   ipf_flag;

#if IPCM
	int              ipcm_enable_flag;
#endif
	u8               amvr_enable_flag;
#if IBC_ABVR
	u8               abvr_enable_flag;
#endif
#if USE_SP
	int              isc_enable_flag;
#endif
	int              umve_enable_flag;
#if UMVE_ENH
	int              umve_enh_enable_flag;
#endif
#if OBMC
	int              obmc_enable_flag;
#endif
#if AWP
	int              awp_enable_flag;
#endif
#if AWP_MVR
	int              awp_mvr_enable_flag;
#endif
#if ETMVP
	int              etmvp_enable_flag;
#endif
#if SUB_TMVP
	int              sbtmvp_enable_flag;
#endif
#if SH_UNIFY
#if ETMVP || MVAP
	int              emvp_enable_flag;
#endif
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if DBK_SCC
	int              loop_filter_type_enable_flag;
#endif
#if DBR
	int              dbr_enable_flag;
#endif
	int              ipf_enable_flag;
#if EXT_AMVR_HMVP
	int              emvr_enable_flag;
#endif
	u8               affine_enable_flag;
#if ASP
	u8               asp_enable_flag;
#endif
#if AFFINE_UMVE
	u8               affine_umve_enable_flag;
#endif
#if SMVD
	u8               smvd_enable_flag;
#endif
#if BIO
	u8               bio_enable_flag;
#endif
#if BGC
	u8               bgc_enable_flag;
#endif
#if DMVR
	u8               dmvr_enable_flag;
#endif
#if INTERPF
	u8               interpf_enable_flag;
#endif
#if IPC
	u8               ipc_enable_flag;
#endif
#if MVAP
	u8               mvap_enable_flag;
	u8               num_of_mvap_cand;
#endif
#if DT_PARTITION
	u8               dt_intra_enable_flag;
#endif
	u8               num_of_hmvp_cand;
#if IBC_BVP
	u8               num_of_hbvp_cand;
#endif
#if TSCPM
	u8               tscpm_enable_flag;
#if ENHANCE_TSPCM
	u8               enhance_tscpm_enable_flag;
#endif
#if PMC
	u8               pmc_enable_flag;
#endif
#endif
#if IPF_CHROMA
	u8               chroma_ipf_enable_flag;
#endif
#if IIP
	u8               iip_enable_flag;
#endif
#if SAWP
	u8               sawp_enable_flag;
#endif // SAWP
#if FIMC
	u8               fimc_enable_flag;
#endif
#if IST
	u8               ist_enable_flag;
#endif
#if ISTS
	u8               ists_enable_flag;
#endif
#if TS_INTER
	u8               ts_inter_enable_flag;
#endif
#if EST
	u8               est_enable_flag;
#endif
#if ST_CHROMA
	u8               st_chroma_enable_flag;
#endif
#if SRCC
	u8               srcc_enable_flag;
#endif
#if CABAC_MULTI_PROB
	u8               mcabac_enable_flag;
#endif
#if EIPM
	u8               eipm_enable_flag;
#endif
	u8               sample_adaptive_offset_enable_flag;
#if ESAO
	u8               esao_enable_flag;
#endif 
#if CCSAO
	u8               ccsao_enable_flag;
#endif 

#if INTERPF
	u8   inter_filter_flag;
#endif
#if IPC
	u8   ipc_flag;
#endif
#if AWP
	/* awp flag*/
	u8   awp_flag;
	/* awp cand idx*/
	u8   awp_idx0;
	u8   awp_idx1;
	/* awp mv & refidx*/
	s16  awp_mv0[REFP_NUM][MV_D];
	s16  awp_mv1[REFP_NUM][MV_D];
	s8   awp_refi0[REFP_NUM];
	s8   awp_refi1[REFP_NUM];
#endif
#if SAWP
	/* awp flag*/
	u8   sawp_flag;
	/* awp cand idx*/
	u8   sawp_idx0;
	u8   sawp_idx1;

	u8   sawp_mpm[SAWP_MPM_NUM];
#endif // SAWP

#if AWP_MVR
	u8   awp_mvr_flag0;
	u8   awp_mvr_idx0;
	u8   awp_mvr_flag1;
	u8   awp_mvr_idx1;
#endif
#if USE_IBC
	// ibc flag for MODE_IBC
	u8   ibc_flag;
#endif
#if IBC_ABVR
	u8   bvr_idx;
#endif
#if IIP
	u8   iip_flag;
#endif
#if IST
	u8   ist_tu_flag;
#endif
#if ISTS
	u8   ph_ists_enable_flag;
#endif
#if TS_INTER
	u8   ph_ts_inter_enable_flag;
#endif
#if AWP
	u8   ph_awp_refine_flag;
#endif
#if EST
	u8   est_flag;
#endif
#if ST_CHROMA
	u8   st_chroma_flag;
#endif
#if IPC
	u8                   ph_ipc_flag;
#endif
#if AFFINE_UMVE
	u8   affine_umve_flag;
	s8   affine_umve_idx[VER_NUM];
	s8   best_affine_merge_index;
#endif
#if ETMVP
	u8   etmvp_flag;
#endif
#if UNIFIED_HMVP_1
	u8   mvap_flag;
	u8   sub_tmvp_flag;
#endif
#if USE_SP
	u8   sp_flag;
	u8   sp_copy_direction;
	u16  sub_string_no;
	//COM_SP_INFO string_copy_info[SP_STRING_INFO_NO];
	double cur_bst_rdcost;
	u8   is_sp_pix_completed;
	//u8* PixelType;
	//u16* PvAddr;
	//u16* upIdx;
	//u8* up_all_comp_flag;
	u8                   cs2_flag;
	u8                   evs_copy_direction;
	int                  evs_sub_string_no;
	//COM_SP_EVS_INFO* evs_str_copy_info; //evs_str_copy_info[SP_STRING_INFO_NO];
	int                  unpred_pix_num;
	//COM_SP_PIX* unpred_pix_info;   //unpred_pix_info[SP_STRING_INFO_NO];
	u8                   equal_val_str_present_flag;
	u8                   unpredictable_pix_present_flag;
	//s16* p_SRB[N_C];
	u8                   pvbuf_size;
	//u8* pvbuf_reused_flag;
	u8                   all_comp_flag[MAX_SRB_SIZE];//1:LUMA AND CHROMA 0: ONLY LUMA
	u8                   all_comp_pre_flag[MAX_SRB_PRED_SIZE];
	u8                   cuS_flag[MAX_SRB_SIZE];
	u8                   cuS_pre_flag[MAX_SRB_PRED_SIZE];
	s16                  pv_x[MAX_SRB_SIZE];
	s16                  pv_x_prev[MAX_SRB_PRED_SIZE];
	s16                  pv_y[MAX_SRB_SIZE];
	s16                  pv_y_prev[MAX_SRB_PRED_SIZE];
	//s16* p_SRB_prev[N_C];
	u8                   pvbuf_size_prev;
	s16                  m_pvbuf_prev[2][MAX_SRB_PRED_SIZE];
	u16                  m_pvbuf[2][MAX_SRB_SIZE];
	u8                   m_dpb_idx[MAX_SRB_SIZE];
	u8                   m_dpb_idx_prev[MAX_SRB_PRED_SIZE];
	u8                   m_pv_flag[MAX_CU_DIM];
	u8                   m_dpb_reYonly[MAX_SRB_SIZE];
	u8                   m_dpb_reYonly_prev[MAX_SRB_PRED_SIZE];
	int                  LcuRx0;
	int                  LcuRy0;
#endif
#if SBT
	u8   sbt_info;
	int  sbt_hor_trans;
	int  sbt_ver_trans;
#endif

#if TB_SPLIT_EXT
	//note: DT can apply to intra CU and only use normal amvp for inter CU (no skip, direct, amvr, affine, hmvp)
	//int  pb_part;
	//int  tb_part;
	COM_PART_INFO  pb_info;
	COM_PART_INFO  tb_info;
#endif
} COM_MODE;

typedef struct _MD_COM_MODE
{
	/* CU position X in a frame in SCU unit */
	U11            x_scu;
	/* CU position Y in a frame in SCU unit */
	U11            y_scu;

	/* depth of current CU */
	U4            cud;

	/* width of current CU */
	U7            cu_width;
	/* height of current CU */
	U7            cu_height;
	/* log2 of cu_width */
	U3             cu_width_log2;
	/* log2 of cu_height */
	U3             cu_height_log2;
	/* position of CU */
	U13            x_pos;
	U13            y_pos;
	/* CU position in current frame in SCU unit */
	U23            scup;
	U2  cu_mode;
#if TB_SPLIT_EXT
	//note: DT can apply to intra CU and only use normal amvp for inter CU (no skip, direct, amvr, affine, hmvp)
	int  pb_part;
	int  tb_part;
#endif
#if TB_SPLIT_EXT
	int  num_nz[MAX_NUM_TB][N_C];
#else
	int  num_nz[N_C];
#endif

	/* reference indices */
	S3   refi[REFP_NUM];

	/* MVR indices */
	U8   mvr_idx;
	U8   umve_flag;
	U8   umve_idx;
	U8   skip_idx;
#if EXT_AMVR_HMVP
	U8   mvp_from_hmvp_flag;
#endif
#if INTERPF
	u8   inter_filter_flag;
#endif
	/* mv difference */
	s16  mvd[REFP_NUM][MV_D];
	/* mv */
	s16  mv[REFP_NUM][MV_D];

	U8   affine_flag;
	CPMV affine_mv[REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd[REFP_NUM][VER_NUM][MV_D];
#if SMVD
	U8   smvd_flag;
#endif

	/* intra prediction mode */
#if TB_SPLIT_EXT
	u8   mpm[MAX_NUM_PB][2];
	S8   ipm[MAX_NUM_PB][2];
#else
	U8   mpm[2];
	S8   ipm[2];
#endif
	U8   ipf_flag;

	U1 chroma_motion;
    U13 x_luma;
	U13 y_luma;

	static U12 x_chroma;
	static U12 y_chroma;
	static BOOL last_child;
} MD_COM_MODE;

struct MD_KERNEL_INPUT
{
	U13            x_pos;
	U13            y_pos;
	U12 x_chroma;
	U12 y_chroma;
	U1 chroma_motion;
	U23            scup;
	U7            cu_width;
	U7            cu_height;
	U3             cu_width_log2;
	U3             cu_height_log2;
};

struct MD_KERNEL_MODE
{
	U2  cu_mode;
	U8   affine_flag;
	s16  mvd[REFP_NUM][MV_D];
	s16  mv[REFP_NUM][MV_D];
	U8   skip_idx;
	U8   smvd_flag;
	CPMV affine_mv[REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd[REFP_NUM][VER_NUM][MV_D];
	S3   refi[REFP_NUM];
	U8   umve_flag;
	U8   umve_idx;
	S8   ipm[MAX_NUM_PB][2];
	int  tb_part;
	int  pb_part;
	u8   mpm[MAX_NUM_PB][2];
#if INTERPF
	u8   inter_filter_flag;
#endif
};

typedef struct _MD_COM_MODE_BEST
{
	/* CU position X in a frame in SCU unit */
	U11            x_scu;
	/* CU position Y in a frame in SCU unit */
	U11            y_scu;
	/* depth of current CU */
	U4            cud;
	/* width of current CU */
	U7            cu_width;
	/* height of current CU */
	U7            cu_height;
	/* log2 of cu_width */
	U3             cu_width_log2;
	/* log2 of cu_height */
	U3             cu_height_log2;
	/* position of CU */
	U13            x_pos;
	U13            y_pos;
	/* CU position in current frame in SCU unit */
	U23            scup;

	U2  cu_mode;
#if TB_SPLIT_EXT
	//note: DT can apply to intra CU and only use normal amvp for inter CU (no skip, direct, amvr, affine, hmvp)
	int  pb_part;
	int  tb_part;
	//COM_PART_INFO  pb_info;
	//COM_PART_INFO  tb_info;
#endif

	pel  rec_y[MAX_CU_DIM];
	pel  rec_u[MAX_CU_DIM >> 2];
	pel  rec_v[MAX_CU_DIM >> 2];
	s16  coef_y[MAX_CU_DIM];
	s16  coef_u[MAX_CU_DIM >> 2];
	s16  coef_v[MAX_CU_DIM >> 2];
	pel  pred_y[MAX_CU_SIZE2][MAX_CU_SIZE2];
	/*struct pred {
		int cu_width;
		int pred_y[64][cu_width];
	};*/
	pel  pred_u[MAX_CU_DIM >> 2];
	pel  pred_v[MAX_CU_DIM >> 2];

#if TB_SPLIT_EXT
	int  num_nz[MAX_NUM_TB][N_C];
#else
	int  num_nz[N_C];
#endif

	/* reference indices */
	S3   refi[REFP_NUM];

	/* MVR indices */
	U8   mvr_idx;
	U8   umve_flag;
	U8   umve_idx;
	U8   skip_idx;
#if EXT_AMVR_HMVP
	U8   mvp_from_hmvp_flag;
#endif
#if INTERPF
	u8   inter_filter_flag;
#endif
	/* mv difference */
	s16  mvd[REFP_NUM][MV_D];
	/* mv */
	s16  mv[REFP_NUM][MV_D];

	U8   affine_flag;
	CPMV affine_mv[REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd[REFP_NUM][VER_NUM][MV_D];
#if SMVD
	U8   smvd_flag;
#endif

	/* intra prediction mode */
#if TB_SPLIT_EXT
	u8   mpm[MAX_NUM_PB][2];
	S8   ipm[MAX_NUM_PB][2];
#else
	U8   mpm[2];
	S8   ipm[2];
#endif
	U8   ipf_flag;

	U1 chroma_motion;
	U13 x_luma;
	U13 y_luma;

	U12 x_chroma;
	U12 y_chroma;
	BOOL last_child;
} MD_COM_MODE_BEST;

struct MD_COM_MODE_BEST_16
{
	U11		x_scu;
	U11		y_scu;
	U4		cud;
	U7		cu_width;
	U7		cu_height;
	U3		cu_width_log2;
	U3		cu_height_log2;
	U13		x_pos;
	U13		y_pos;
	U23		scup;
	U2		cu_mode;
#if TB_SPLIT_EXT
	//note: DT can apply to intra CU and only use normal amvp for inter CU (no skip, direct, amvr, affine, hmvp)
	int		pb_part;
	int		tb_part;
#endif
#if TB_SPLIT_EXT
	int		num_nz_y[MAX_NUM_TB];
#else
	int		num_nz;
#endif
	S3		refi[REFP_NUM];
	U8		mvr_idx;
	U8		umve_flag;
	U8		umve_idx;
	U8		skip_idx;
#if EXT_AMVR_HMVP
	U8		mvp_from_hmvp_flag;
#endif
	s16		mvd[REFP_NUM][MV_D];
	S14		mv[REFP_NUM][MV_D];
	U8		affine_flag;
	CPMV	affine_mv[REFP_NUM][VER_NUM][MV_D];
	s16		affine_mvd[REFP_NUM][VER_NUM][MV_D];
#if SMVD
	U8		smvd_flag;
#endif
#if TB_SPLIT_EXT
	u8		mpm[MAX_NUM_PB][2];
	S8		ipm[MAX_NUM_PB][2];
#else
	U8		mpm[2];
	S8		ipm[2];
#endif
	U8		ipf_flag;
	U1		chroma_motion;
	U13		x_luma;
	U13		y_luma;
	U12		x_chroma;
	U12		y_chroma;
	BOOL	last_child;
};
/*****************************************************************************
* map structure
*****************************************************************************/
typedef struct _COM_MAP
{
    /* SCU map for CU information */
    u32                    *map_scu;
    /* LCU split information */
    s8                     (*map_split)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
    /* decoded motion vector for every blocks */
	s32(*map_mv)[REFP_NUM][MV_D];//s16                    (*map_mv)[REFP_NUM][MV_D];
    /* reference frame indices */
	s32(*map_refi)[REFP_NUM];//s8                     (*map_refi)[REFP_NUM];
    /* intra prediction modes */
    s8                     *map_ipm;
    u32                    *map_cu_mode;
#if TB_SPLIT_EXT
    u32                    *map_pb_tb_part;
#endif
    s8                     *map_depth;
    s8                     *map_delta_qp;
    s8                     *map_patch_idx;
#if USE_SP
	u8* map_usp;
#endif
} COM_MAP;

/*****************************************************************************
* common info
*****************************************************************************/
typedef struct _COM_INFO
{
    /* current chunk header */
    COM_CNKH                cnkh;

    /* current picture header */
    COM_PIC_HEADER          pic_header;

    /* current slice header */
    COM_SH_EXT              shext;

    /* sequence header */
    COM_SQH                 sqh;

    /* decoding picture width */
    int                     pic_width;
    /* decoding picture height */
    int                     pic_height;
    /* maximum CU width and height */
    int                     max_cuwh;
    /* log2 of maximum CU width and height */
    int                     log2_max_cuwh;

    /* picture width in LCU unit */
    int                     pic_width_in_lcu;
    /* picture height in LCU unit */
    int                     pic_height_in_lcu;

    /* picture size in LCU unit (= w_lcu * h_lcu) */
    int                     f_lcu;
    /* picture width in SCU unit */
    int                     pic_width_in_scu;
    /* picture height in SCU unit */
    int                     pic_height_in_scu;
    /* picture size in SCU unit (= pic_width_in_scu * h_scu) */
    int                     f_scu;

    int                     bit_depth_internal;
    int                     bit_depth_input;
    int                     qp_offset_bit_depth;
    u8  *tab_mvbits;
    int gop_size;
#if USE_INTRA_REFRESH
	U1 EncRefresh;
	U1 RefreshMode;
	U8 RefreshSize;
	U8 pirStartRow;
	U8 pirEndRow;
	U8 pirStartCol;
	U8 pirEndCol;
	int framesSinceLastPir;
#endif
#if USE_SPEED_LEVEL
	U8 speed_level;
#endif
#if USE_ROI_QP_CONFIG
	int use_ROI_qp1;
	int ROI_qp_Start_x1;
	int ROI_qp_Start_y1;
	int ROI_qp_End_x1;
	int ROI_qp_End_y1;
	int ROI_QP1;

	int use_ROI_qp2;
	int ROI_qp_Start_x2;
	int ROI_qp_Start_y2;
	int ROI_qp_End_x2;
	int ROI_qp_End_y2;
	int ROI_QP2;

	int use_ROI_qp3;
	int ROI_qp_Start_x3;
	int ROI_qp_Start_y3;
	int ROI_qp_End_x3;
	int ROI_qp_End_y3;
	int ROI_QP3;

	int use_ROI_qp4;
	int ROI_qp_Start_x4;
	int ROI_qp_Start_y4;
	int ROI_qp_End_x4;
	int ROI_qp_End_y4;
	int ROI_QP4;

	int use_ROI_qp5;
	int ROI_qp_Start_x5;
	int ROI_qp_Start_y5;
	int ROI_qp_End_x5;
	int ROI_qp_End_y5;
	int ROI_QP5;

	int use_ROI_qp6;
	int ROI_qp_Start_x6;
	int ROI_qp_Start_y6;
	int ROI_qp_End_x6;
	int ROI_qp_End_y6;
	int ROI_QP6;

	int use_ROI_qp7;
	int ROI_qp_Start_x7;
	int ROI_qp_Start_y7;
	int ROI_qp_End_x7;
	int ROI_qp_End_y7;
	int ROI_QP7;

	int use_ROI_qp8;
	int ROI_qp_Start_x8;
	int ROI_qp_Start_y8;
	int ROI_qp_End_x8;
	int ROI_qp_End_y8;
	int ROI_QP8;
#endif

#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	int useQPMAP;
	int useModeMAP;
	int useQPMAPCon;
	int useModeMAPCon;
#endif

#if USE_ROI_MODE_CONFIG
	U1  use_ROI_Mode1;
	U12 ROI_Mode_Start_x1;
	U12 ROI_Mode_Start_y1;
	U12 ROI_Mode_End_x1;
	U12 ROI_Mode_End_y1;
	U3  ROI_Mode1;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode2;
	U12 ROI_Mode_Start_x2;
	U12 ROI_Mode_Start_y2;
	U12 ROI_Mode_End_x2;
	U12 ROI_Mode_End_y2;
	U3  ROI_Mode2;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode3;
	U12 ROI_Mode_Start_x3;
	U12 ROI_Mode_Start_y3;
	U12 ROI_Mode_End_x3;
	U12 ROI_Mode_End_y3;
	U3  ROI_Mode3;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode4;
	U12 ROI_Mode_Start_x4;
	U12 ROI_Mode_Start_y4;
	U12 ROI_Mode_End_x4;
	U12 ROI_Mode_End_y4;
	U3  ROI_Mode4;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode5;
	U12 ROI_Mode_Start_x5;
	U12 ROI_Mode_Start_y5;
	U12 ROI_Mode_End_x5;
	U12 ROI_Mode_End_y5;
	U3  ROI_Mode5;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode6;
	U12 ROI_Mode_Start_x6;
	U12 ROI_Mode_Start_y6;
	U12 ROI_Mode_End_x6;
	U12 ROI_Mode_End_y6;
	U3  ROI_Mode6;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode7;
	U12 ROI_Mode_Start_x7;
	U12 ROI_Mode_Start_y7;
	U12 ROI_Mode_End_x7;
	U12 ROI_Mode_End_y7;
	U3  ROI_Mode7;//MSB~LSB  intra inter skip  default value: 3'b111

	U1  use_ROI_Mode8;
	U12 ROI_Mode_Start_x8;
	U12 ROI_Mode_Start_y8;
	U12 ROI_Mode_End_x8;
	U12 ROI_Mode_End_y8;
	U3  ROI_Mode8;//MSB~LSB  intra inter skip  default value: 3'b111
#endif
#if CUDQP_QP_MAP
	int useCuQPMAP;
#endif
#if USE_TRACE_DUMP
	int useTraceDump;
#endif
} COM_INFO;

typedef struct _COM_INFO_HLS
{
    /* current picture header */
    COM_PIC_HEADER          pic_header;

    /* current slice header */
    COM_SH_EXT              shext;//slice_qp, fixed_slice_qp_flag

    /* sequence header */
    COM_SQH                 sqh;

    /* decoding picture width */
    int                     pic_width;
    /* decoding picture height */
    int                     pic_height;
    /* maximum CU width and height */
    int                     max_cuwh;
    /* log2 of maximum CU width and height */
    int                     log2_max_cuwh;

    /* picture width in LCU unit */
    int                     pic_width_in_lcu;
    /* picture height in LCU unit */
    int                     pic_height_in_lcu;

    /* picture width in SCU unit */
    int                     pic_width_in_scu;
    /* picture height in SCU unit */
    int                     pic_height_in_scu;

    int                     bit_depth_internal;
    int                     bit_depth_input;
    int                     qp_offset_bit_depth;


} COM_INFO_HLS;

typedef enum _MSL_IDX
{
    MSL_SKIP,  //skip
    MSL_MERG,  //merge or direct
    MSL_LIS0,  //list 0
    MSL_LIS1,  //list 1
    MSL_BI,    //bi pred
    NUM_MODE_SL,
} MSL_IDX;
/*****************************************************************************
* patch info
*****************************************************************************/
#if PATCH
typedef struct _PATCH_INFO
{
    int                     stable;
    int                     cross_patch_loop_filter;
    int                     ref_colocated;
    int                     uniform;
    int                     height;
    int                     width;
    int                     columns;
    int                     rows;
    int                     idx;
    /*patch's location(up left) in pixel*/
    int                     x_pel;
    int                     y_pel;
    /*all the cu in a patch are skip_mode*/
    int                     skip_patch_flag;
    /*pointer the width of each patch*/
    int                    *width_in_lcu;
    int                    *height_in_lcu;
    /*count as the patch*/
    int                     x_pat;
    int                     y_pat;
    /*define the boundary of a patch*/
    int                     left_pel;
    int                     right_pel;
    int                     up_pel;
    int                     down_pel;
    /*patch_sao_enable_flag info of all patches*/
    s8                     *patch_sao_enable;
	int					   patch_switch;
	int                    pat;
	int                    patch_end;
	int                    len;
} PATCH_INFO;
#endif

/*****************************************************************************
 *  Substitude of the Microsoft SSE structure __m128i   by zhuxz
*****************************************************************************/

typedef union _sim_128i {
	u64	ep64u[2];
	s64	ep64s[2];
	u32 ep32u[4];
	s32 ep32s[4];
	u16 ep16u[8];
	s16 ep16s[8];
	u8 ep8u[16];
	s8 ep8s[16];
} sim_128i;

enum TRANS_TYPE
{
    DCT2,
    DCT8,
    DST7,
    NUM_TRANS_TYPE
};


/*****************************************************************************
 *  fetch extraction by wenyb // revised by xgq
*****************************************************************************/
typedef struct _CTX_FETCH
{	
	U32 ctx_pinter_refp_ptr[17][2];
	U32 ctx_ptr;
#if ENABLE_BFRAME
	U8 ctx_rpm_num_refp[REFP_NUM];
	U12  ctx_pinter_refp_pic_stride_chroma_all[17][2];
	U13  ctx_pinter_refp_pic_stride_luma_all[17][2];
	U11  ctx_pinter_refp_pic_width_chroma_all[17][2];
	U12  ctx_pinter_refp_pic_width_luma_all[17][2];
#else
	U4 ctx_rpm_num_refp;
#endif
	
	/////////////////////ctx.pinter.refp.pic
	U11  ctx_pinter_refp_pic_height_chroma;
	U12  ctx_pinter_refp_pic_height_luma;
	U12  ctx_pinter_refp_pic_stride_chroma;
	U13  ctx_pinter_refp_pic_stride_luma;
	U11  ctx_pinter_refp_pic_width_chroma;
	U12  ctx_pinter_refp_pic_width_luma;
	
	U12  ctx_info_pic_width;
	U12  ctx_info_pic_height;
	U6  ctx_info_pic_width_in_lcu;
	U6  ctx_info_pic_height_in_lcu;
	U10  ctx_info_pic_width_in_scu;
	U10  ctx_info_pic_height_in_scu;
	
	s8  ctx_info_pic_header_chroma_quant_param_delta_cb;
	s8  ctx_info_pic_header_chroma_quant_param_delta_cr;
	U2  ctx_info_pic_header_slice_type;
	
	u8        sample_adaptive_offset_enable_flag;
	int       stride_luma;
	int       stride_chroma;
	U12 lcu_cnt;
#if USE_ROI_QP_CONFIG || USE_ROI_MODE_CONFIG
	FILE *lcumap_in;
	int slice_qp;
#endif
	//dbk
	int ref_ptr_dbk[MAX_NUM_REF_PICS][REFP_NUM];
	U1 flag_valid_map_mv;
	U1 flag_valid_map_refi;
	U1 flag_valid_list_ptr;
	U1 flag_valid_map_mv_1;
	U1 flag_valid_map_refi_1;
	U1 flag_valid_list_ptr_1;
	U32 refp_ptr_test[MAX_NUM_REF_PICS][REFP_NUM];
	U1 is_library_picture[MAX_NUM_REF_PICS][REFP_NUM];
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int use_deblock;
 	int f_scu;
	int lcu_num;// core_lcu_num; ////
	u8 skip_flag;// core_skip_flag;
	int log2_culine; //ctx_log2_culine;
	int max_cuwh;// ctx_info_max_cuwh;
	//int pic_width_in_lcu;// ctx_info_pic_width_in_lcu;
	//int pic_width_in_scu;// ctx_info_pic_width_in_scu;
	u8 slice_sao_enable[3]; //ctx_info_shext_slice_sao_enable[3];
	//u8 pic_header_slice_type; //ctx_info_pic_header_slice_type;
	u8 affine_enable_flag; //ctx_info_sqh_addine_enable_flag;
	//int pic_width; //ctx_info_pic_width;
	//int pic_height; //ctx_info_pic_height;

	int umve_enable_flag;//    ctx_info_sqh_umve_enable_flag = 1
	u8 num_of_hmvp_cand;////ctx->info.sqh.num_of_hmvp_cand
	u8 amvr_enable_flag;// ctx_info_sqh_amvr_enable_flag = 1
	int emvr_enable_flag;//    ctx_info_sqh_emvr_enable_flag = 1
	u8 dt_intra_enable_flag;// ctx_info_sqh_dt_intra_enable_flag = 0
	u8 position_based_transform_enable_flag;//ctx_info_sqh_position_based_transform_enable_flag = 0
	u8 tscpm_enable_flag;// ctx_info_sqh_tscpm_enable_flag = 0
	int ipf_enable_flag;
	//u8 sample_adaptive_offset_enable_flag;

	//u8 slice_type; //ctx_info_pic_header_slice_type;
	int lcu_total_cnt;

	//U3  rpm_num_refp[REFP_NUM];//ctx_rpm_num_refp[REFP_NUM]
	int fixed_slice_qp_flag;
    u8 cu_delta_qp_flag;
	u8 ph_ists_enable_flag;
	u8 ph_epmc_model_flag;
	u8 ibc_flag;
	u8 sp_pic_flag;
	u8 evs_ubvs_pic_flag;
	u8 umve_set_flag;
	u8 ph_ipc_flag;
	int cu_qp_group_area_size;
	//s8 chroma_quant_param_delta_cb;//ctx_info_pic_header_chroma_quant_param_delta_cb
	//s8 chroma_quant_param_delta_cr;//ctx_info_pic_header_chroma_quant_param_delta_cr
	u8 slice_qp;
	u8 min_cu_size;
	u8 max_part_ratio;
	u8 max_split_times;
	u8 min_qt_size;
	u8 max_bt_size;
	u8 max_eqt_size;
	u8 max_dt_size;
	u8 abvr_enable_flag;
	int umve_enh_enable_flag;
	int awp_enable_flag;
	int awp_mvr_enable_flag;
	int etmvp_enable_flag;
	u8 affine_umve_enable_flag;
	u8 smvd_enable_flag;
	u8 bgc_enable_flag;
	u8 interpf_enable_flag;
	u8 ipc_enable_flag;
	u8 num_of_mvap_cand;
	u8 num_of_hbvp_cand;
	u8 enhance_tscpm_enable_flag;
	u8 pmc_enable_flag;
	u8 iip_enable_flag;
	u8 sawp_enable_flag;
	u8 sbt_enable_flag;
	u8 ists_enable_flag;
	u8 srcc_enable_flag;
	u8 eipm_enable_flag;
	u8 st_chroma_enable_flag;
	u8 est_enable_flag;
	int log2_max_cuwh;
	int bit_depth_input;
    int qp_offset_bit_depth;
	//int ptr;//ctx_ptr
	u8 temporal_id;
	int refp_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	//int num_refp[REFP_NUM];//rpm_num_refp
	BOOL dataCol;
	u32 umveOffsetPicCount[UMVE_REFINE_STEP_SEC_SET];
	int cons_pred_mode;
	u8 tree_status;
	int cu_delta_qp_lcu_map[256];
	int num_delta_qp;
	int pred_qp;
	int cu_qp_group_x;
	int cu_qp_group_y;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} CTX_FETCH;

/*****************************************************************************
 *  fme extraction by jiangych
*****************************************************************************/
#define MAX_CU_SIZE_SPEL		(1 << 6)
#define PRED_WINDOW_STRIDE		((MAX_CU_SIZE_SPEL << 2) + 1)
#define SUB_PEL_ME_NUM          2  // 1/4pel & 1/2pel



/* Line buffer of motion esitmation */
//ENC_ME_LINE_MAP* me_line_map_ptr;
#endif 

#include "com_tbl.h"
#include "com_util.h"
//#include "com_recon.h"
//#include "com_ipred.h"
#include "com_tbl.h"
//#include "com_itdq.h"
#include "com_picman.h"
//#include "com_mc.h"
#include "com_img.h"

#define USE_IME_ALL_CU 1
#define USE_FME_ALL_CU 1&&USE_IME_ALL_CU
#define USE_FME_MVP    0&&USE_IME_ALL_CU
#define USE_ORG_QT_ONLY 0
/* most split cu number */
#define CU_SIZE_NUM_0 1600
#define MAP_CU_UNVISITED (CU_SIZE_NUM_0-1)
#define MAP_CU_UNAVAILABLE (-1)


#define UNIT_SIZE_LOG2 4
#define UNIT_SIZE (1 << UNIT_SIZE_LOG2)
#define UNIT_WIDX (UNIT_SIZE_LOG2 - MIN_CU_LOG2)

#define UNITC_SIZE_LOG2 (UNIT_SIZE_LOG2 - 1)
#define UNITC_SIZE (1 << UNITC_SIZE_LOG2)
#define UNITC_WIDX (UNITC_SIZE_LOG2 - MIN_CU_LOG2)

#define HISBLOCK_SIZE 8

#define INTRA_NEIB_SIZE (MAX_CU_SIZE / 2 * 9)
#define INTRA_NEIB_MID (INTRA_NEIB_SIZE / 2)
#define MAX_FIRST_SEARCH_STEP              32
#define MAX_REFINE_SEARCH_STEP             8
#define RASTER_SEARCH_THD                  5
#define REFINE_SEARCH_THD                  0

struct glb_reg_tv {
	u8 SLICE_GLB_CTRL_aec_slice_start;
	u8 SLICE_GLB_CTRL_sao_slice_start;
	u8 SLICE_GLB_CTRL_dbk_slice_start;
	u8 SLICE_GLB_CTRL_md_slice_start;
	u8 SLICE_GLB_CTRL_rmd_slice_start;
	u8 SLICE_GLB_CTRL_fetch_slice_start;
	u8 SLICE_INFO_slice_type;
	u16 PIC_SIZE_PEL_pic_height;
	u16 PIC_SIZE_PEL_pic_width;
	u16 PIC_SIZE_SCU_pic_height_in_scu;
	u16 PIC_SIZE_SCU_pic_width_in_scu;
	u16 PIC_SIZE_LCU_pic_height_in_lcu;
	u16 PIC_SIZE_LCU_pic_width_in_lcu;
	u8 LCU_CNT_lcu_col;
	u8 LCU_CNT_lcu_row;
	u16 LCU_CNT_lcu_total_cnt;
	u8 CU_SIZE_scu_wh;
	u8 CU_SIZE_lcu_wh;
	u8 CU_SIZE_log2_scu_wh;
	u8 CU_SIZE_log2_lcu_wh;
	u8 PIX_INFO_internal_bit_width;
	u32 ORG_PIC_Y_ADDR_org_pic_y_addr;
	u32 ORG_PIC_U_ADDR_org_pic_u_addr;
	u32 ORG_PIC_V_ADDR_org_pic_v_addr;
	u32 REC_PIC_Y_ADDR_rec_pic_y_addr;
	u32 REC_PIC_U_ADDR_rec_pic_u_addr;
	u32 REC_PIC_V_ADDR_rec_pic_v_addr;
	u32 BS_START_ADDR_bs_start_addr;
	u32 MD_LAMBA0_lambda0;
	u32 MD_LAMBA1_lambda1;
	u32 MD_LAMBA2_lambda2;
	u8 MD_QP_qp_y;
	u8 MD_QP_qp_u;
	u8 MD_QP_qp_v;
	u32 CHROMA_W0_chroma_weight0;
	u32 CHROMA_W1_chroma_weight1;
	u8 MAX_SPLIT_TIMES_max_split_times;
	u8 MAX_SPLIT_TIMES_max_part_ratio;
	u8 MAX_SPLIT_TIMES_min_qt_size;
	u8 MAX_SPLIT_TIMES_max_bt_size;
	u8 MAX_EQT_SIZE_max_eqt_size;
	u32 PIC_PITCH_pic_pitch;
	u32 ENC_TEST_test_rw;
};

struct md_info_tv {
	u16 pix_x;
	u16 pix_y;
};


#endif /* _COM_DEF_H_ */

#define PCNL_FETCH 1
#define PCNL_RMD 1

#ifdef __SYNTHESIS__
#define PATCH_MODE() 0
#else
#define PATCH_MODE() op_patch_mode_on
#endif //__SYNTHESIS__

