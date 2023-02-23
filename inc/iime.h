#ifndef _IIME_H_
#define _IIME_H_

#define IME_VIVADO_HLS 1
#define IME_PRAGMA_ON_INTERFACE 1
#define IME_PRAGMA_ON IME_VIVADO_HLS&&1		// turn off
#define REFP_PTR_FEBUG	1
#define BW_DEBUG					0//1	
#include "type_def.h"
#define FME_SW_SIZE		        88
#define FME_SWW_SIZE		        164//128//88
#define FME_SWH_SIZE		        132//96//88

#define OFFSET_LV1_LV0_ON	1//1
#define OFFSET_LV1		(FENC_PADDING)//((FENC_PADDING >>1)>>1)
#define OFFSET_LV0		(FENC_PADDING)//((FENC_PADDING >>1)>>1)
#define OFFSET_FME_SWW		((FME_SWW_SIZE-FME_SW_SIZE)>>1)	
#define OFFSET_FME_SWH		((FME_SWH_SIZE-FME_SW_SIZE)>>1)

#define MMEA_LEVEL1_SR_UNI 32
#define PRINT_IME_MV 0//1//0
#define PRINT_sadScuUp_1	0
#define DEBUG_IME	0
#define IME_DEBUG_calcSadScuLv0TestCaseMore							0//1	// 1: cycle 3222, area 145% ; 0: cycle 4582, area 65%
#define SEARCH_FULLLV1	1
#define SEARCH_FULLLV1_TURN_ON	1//0
#define ZJJ_OP_OFF				1

#define MAX_CU_H             64    /* maxx CU size */
#define MAX_CU_W             64    /* maxx CU size */

#define VIVADO_HLS_IME				1//0//1
#define VIVADO_HLS_IME_OP_1			1//0		// 0:turn on; 1: turn off
#define IME_PRAGMA_ON_OP			1//0	// 1:turn on; 0:turn off
#define ORR_T						0//1	//control fme org transposition

#define TWO_STAGE_1_SR_W				16	
#define TWO_STAGE_1_SR_H				16	

#define TWO_STAGE_2_SR_W				16	
#define TWO_STAGE_2_SR_H				16

#define LV1_SERIAL						1//0//1
#define LV2_SERIAL						1//0//1//1
#define LV0_MV_PMV_PARTTION_ON			1//0//1
#define SCALE_LAMBDA_MV_16BIT			1
//#define PATCH_DIVIDE					0//1

#define PRINT_MV 0
#define PRINT_PMV 0

#if 0//VIVADO_HLS_IME
#define BIT_10_ON						0
#else
#define BIT_10_ON						1
#endif

#if 0

/*	Basic typedef	*/
typedef signed char                s8;
typedef unsigned char              u8;
typedef signed short               s16;
typedef unsigned short             u16;
typedef signed int                 s32;
typedef unsigned int               u32;
#if defined(X86_64) && !defined(_MSC_VER) /* for 64bit-Linux */
typedef signed long                s64;
typedef unsigned long              u64;
#else
typedef signed long long           s64;
typedef unsigned long long         u64;
typedef s16                        pel; /* pixel type */

#endif
/*	Macro def	*/
#define MV_D 2
#define MAX_CU_SIZE                       (1 << (7))
#define MAX_CU_SIZE2                      (MAX_CU_SIZE >> 1) //addby xgq
#define MAX_CU_DIM                        (MAX_CU_SIZE2 * MAX_CU_SIZE2)
#define MAX_CU_DEPTH                      5  /* 64x64 ~ 4x4 */
#define CU_SIZE_NUM 1600
#define MAP_CU_UNVISITED (CU_SIZE_NUM-1)
#define MAP_CU_UNAVAILABLE (-1)
#define SCU_IN_LCU_NUMS_LEN (MAX_CU_SIZE2>>MIN_CU_LOG2)
#define SCU_IN_LCU_NUMS (SCU_IN_LCU_NUMS_LEN*SCU_IN_LCU_NUMS_LEN)
#define CU_SIZE_NUM 1600
#define MAX_WIDTH 3840
#define MAX_CU_LOG2                        6
#define MIN_CU_LOG2                        2
#define MAX_CU_SIZE_FETCH             64    /* maxx CU size */
#define FENC_STRIDE    (MAX_CU_SIZE_FETCH         )
#define MIN_CU_SIZE_IN_BIT           3
#define SRW				    68 * 4							
#define SRH                 68 * 4
#define FENC_PADDING        4
#define SWW              (MAX_CU_SIZE_FETCH+2*SRW+2*FENC_PADDING)  //576+8=584
#define SWH              (MAX_CU_SIZE_FETCH+2*SRH+2*FENC_PADDING)  //576+8=584
#define RANGE_NUM 2
#define RANGE_MIN 0
#define RANGE_MAX 1
#define DIR_DIM 2
#define MAP_DIMS 2
#define PIC_DIM 0
#define RW_DIM 1
#define MMEA_LEVEL0_SR 8
#define MMEA_LEVEL1_SR 28
#define MMEA_LEVEL2_SR 128
#define SLICE_I                            1
#define NUM_AVS2_SPATIAL_MV                3
#define NUM_SKIP_SPATIAL_MV                6
#define MVPRED_L                           0
#define MVPRED_U                           1
#define MVPRED_UR                          2
#define MVPRED_xy_MIN                      3
#define COM_INT16_MAX           ((s16)0x7FFF)
#define COM_INT16_MIN           ((s16)0x8000)
#define REFP_NUM                           2
#define CTX_CONTROL_LUKE 1
#define  CTU_CONTROL_LUKE 1
/*	Macro function	*/
#define avs3_max(x, y) (((x) > (y)) ? (x) : (y))
#define avs3_min(x, y) (((x) < (y)) ? (x) : (y))
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))
#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define COM_ABS16(a)                   (((a)^((a)>>15)) - ((a)>>15))
#define COM_ABS32(a)                   (((a)^((a)>>31)) - ((a)>>31))
#define MCU_GET_INTRA_FLAG(m)           (int)(((m)>>15) & 1)
#define MCU_GET_CODED_FLAG(m)           (int)(((m)>>31) & 1)
#define REFI_IS_VALID(refi)               ((refi) >= 0)
#define assert(a) 
#define max_ime(x, y, type) ((((type)(x)) > ((type)(y))) ? ((type)(x)) : ((type)(y)))
#define min_ime(x, y, type) ((((type)(x)) < ((type)(y))) ? ((type)(x)) : ((type)(y)))
#define COM_CLIP3_IME(min_x, max_x, value, type)   max_ime((min_x), min_ime((max_x), (value),type),type)
#define DEBUG 0
#define STAT 0
#define CU_SIZE_NUM_32 12
#define CU_SIZE_NUM_16 80
#define CU_SIZE_NUM_8 400
#define CU_SIZE_NUM_4 1056
#define SPLIT_MAP_STRIDE *MAX_CU_DEPTH
#define SW_LEN_LV0 (256)+MAX_CU_SIZE2

#define CU_SIZE_NUM_P1 266
#define CU_SIZE_NUM_P2 266
#define CU_SIZE_NUM_P3 266
#define CU_SIZE_NUM_P4 810

#define MAX_CU_SIZE_SPEL		(1 << 6)
#define PRED_WINDOW_STRIDE		((MAX_CU_SIZE_SPEL << 2) + 1)
#define SUB_PEL_ME_NUM          2  // 1/4pel & 1/2pel
#define ABOVE_LINE_LEN SCU_IN_LCU_NUMS_LEN*3
#define MCU_GET_INTRA_FLAG_IME(m)           (S32)(((m)>>15) & 1)
#define MCU_GET_CODED_FLAG_IME(m)           (S32)(((m)>>31) & 1)
#define MV_X 0
#define MV_Y 1
#ifdef VIVADO_HLS
#define COM_ABS14(a)                   (((a)^((a)>>13)) - ((a)>>13))
#else
#define COM_ABS14(a)                   COM_ABS16(a)
#endif
#define MV_SCALE_PREC                      14 /* Scaling precision for motion vector prediction (2^MVP_SCALING_PRECISION) */

typedef struct {
	U12 pix_x;
	U12 pix_y;
#if CTU_CONTROL_LUKE
	//int  LCU_nr;
	/* total count of remained LCU for encoding one picture. if a picture is
	encoded properly, this value should reach to zero */
	int  lcu_cnt;
	//int  LCU_qp;
	//int  Frame_qp;
#endif
	int  smb_num_width;
	int  smb_num_height;
#if CTU_CONTROL_LUKE
	//int  up_lcu_avail;
	//int  down_lcu_avail;
	//int  left_lcu_avail;
	//int  right_lcu_avail;
	//int  slice_start_flag;
	//int  slice_end_flag;
	/* QP for luma of current encoding CU */
	int             qp_y;
	/* QP for chroma of current encoding CU */
	int             qp_u;
	int             qp_v;
	u32             lambda_mv;
	s64             lambda[3];
	s64             sqrt_lambda[3];
	s64          dist_chroma_weight[2];
	/* current frame numbser */
	int             ptr;
	int				x_last_ctu;
	int				y_last_ctu;
#endif
	//	ENC_ME_LINE_MAP *me_line_map_ptr;
} CTU_Level_Parameters;

typedef struct _FETCH2MD_FW {
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	U7 max_dt_size;
	U1 affine_enable_flag;
	U1 dt_intra_enable_flag;//
	U4 num_of_hmvp_cand;
	U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	U6 pic_width_in_lcu;
	U6 pic_height_in_lcu;
	U3               chroma_quant_param_delta_cb;
	U3               chroma_quant_param_delta_cr;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;
	U21 f_scu;
	/* gop size */
	U6             gop_size;

#if CTX_CONTROL_LUKE
	//int				 width;
	//int				 height;
	U4              bit_depth_internal;
	U4              max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	U1				 affine_subblock_size_idx;
	U1				 ipcm_enable_flag;
	U1			 position_based_transform_enable_flag;
	U1              secondary_transform_enable_flag;
	U1              tscpm_enable_flag;
	U1				 amvr_enable_flag;
	U1				 emvr_enable_flag;
	//int				 affine_enable_flag;
#endif

	S16 refp_ptr[17][2];
	U5 num_refp;
	U9 cur_ptr; // ref pic

}FETCH2MD_FW;

typedef struct _ENC_ME_INFO_ALL
{
	s16 mv[MV_D];
	u8 refi;
	u32 cost;
	u8 visited;
	s16 sq_parent;
	u16 x0_scu; //10
	u16 y0_scu; //10
	u8 otherSizeLog2Minus; //3
	u8 sideFlag;//1
}ENC_ME_INFO_ALL;

typedef struct _ENC_ME_NEB_INFO
{
	//U28 scu;
	S14 mv[REFP_NUM][MV_D];
	S2 refi[REFP_NUM];
	//	U1 is_valid;
	//#if TB_SPLIT_EXT
	//	U32                    pb_tb_part;
	//#endif

}ENC_ME_NEB_INFO;

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
	s16 sadTree4x8Mv[16][8][2];
	s16 sadTree8x4Mv[8][16][2];
	s16 sadTree16x4Mv[4][16][2];
	s16 sadTree4x16Mv[16][4][2];
	s16 sadTree32x4Mv[2][16][2];
	s16 sadTree4x32Mv[16][2][2];
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
	s16 sadTree4x8PMv[16][8][2];
	s16 sadTree8x4PMv[8][16][2];
	s16 sadTree16x4PMv[4][16][2];
	s16 sadTree4x16PMv[16][4][2];
	s16 sadTree32x4PMv[2][16][2];
	s16 sadTree4x32PMv[16][2][2];
}ME_PMV_INFO;

#else

#if 0//VIVADO_HLS_IME



//#include "enc_def.h"
//#include "com_def.h"
#include <math.h>
#include <string>
#include<vector>
#include<map>
#include <fstream>
#include <iostream>

#include "/home/ubuntu/software/Vivado/2019.2/include/gmp.h"	// 197 sever
//#include "/home/ubuntu/software/Vivado/2019.2/include/gmp.h"

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
#define SKIP_RESTRI			1

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

#define RANGE_NUM 2
#define RANGE_MIN 0
#define RANGE_MAX 1
#define DIR_DIM 2
#define MAP_DIMS 2
#define PIC_DIM 0
#define RW_DIM 1

#define REFP_0                             0
#define REFP_1                             1
#define REFP_NUM                           2
//**********************************************************************************************
//**********************************************************************************************
#define MAX_CU_DEPTH                      5  /* 64x64 ~ 4x4 */

#define MAX_CU_SIZE                       (1 << (7))
#define MAX_CU_SIZE2                      (MAX_CU_SIZE >> 1) //addby xgq

#define REFP_NUM                           2
#define MV_D                               2
#define MIN_CU_LOG2                        2
#define SCU_IN_LCU_NUMS_LEN (MAX_CU_SIZE2>>MIN_CU_LOG2)
#define MAX_WIDTH 3840
#define NEB_NUMS 5 /*(A-B-C-D-G)*/

#define ENABLE_BFRAME                    1//0 // enable B frame encoding

#define ABOVE_LINE_LEN SCU_IN_LCU_NUMS_LEN*3

#define COM_ABS16(a)                   (((a)^((a)>>15)) - ((a)>>15))
#define COM_ABS32(a)                   (((a)^((a)>>31)) - ((a)>>31))

#define MV_X 0
#define MV_Y 1

#define MV_SCALE_PREC                      14 /* Scaling precision for motion vector prediction (2^MVP_SCALING_PRECISION) */
#define MMEA_LEVEL0_SR 8
#define REFI_IS_VALID(refi)               ((refi) >= 0)

#define COM_INT16_MIN           ((s16)0x8000)
#define COM_INT16_MAX           ((s16)0x7FFF)
#define CTX_CONTROL_LUKE 1

#define COM_ST_UNKNOWN                  (0)
#define COM_ST_I                        (1)
#define COM_ST_P                        (2)
#define COM_ST_B                        (3)
#define SLICE_I                            COM_ST_I
#define SLICE_P                            COM_ST_P
#define SLICE_B                            COM_ST_B

#define  CTU_CONTROL_LUKE 1


#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))

#define NUM_AVS2_SPATIAL_MV                3
#define NUM_SKIP_SPATIAL_MV                6
#define MVPRED_L                           0
#define MVPRED_U                           1
#define MVPRED_UR                          2
#define MVPRED_xy_MIN                      3
#define MAX_NUM_REF_PICS                   17

//**********************************************************************************************
typedef unsigned int               u32;
typedef signed short               s16;
typedef s16                        pel; /* pixel type */
typedef bool                       BOOL;
typedef signed int                 s32;

/*	Basic typedef	*/
typedef signed char                s8;
typedef unsigned char              u8;
typedef signed short               s16;
typedef unsigned short             u16;
typedef signed int                 s32;
typedef unsigned int               u32;
#if defined(X86_64) && !defined(_MSC_VER) /* for 64bit-Linux */
typedef signed long                s64;
typedef unsigned long              u64;
#else
typedef signed long long           s64;
typedef unsigned long long         u64;
typedef s16                        pel; /* pixel type */

#endif

//**********************************************************************************************

#define DEBUG 0
#define STAT 0

#define SPLIT_MAP_STRIDE *MAX_CU_DEPTH
#define SW_LEN_LV0 (256)+MAX_CU_SIZE2

#define SW_LEN_LV0_V (SWW)//((REFP_SURD_OFF_W * 2) + MAX_CU_SIZE2)
#define SW_LEN_LV0_H (SWH)//((REFP_SURD_OFF_H * 2) + MAX_CU_SIZE2)

#define CU_SIZE_NUM_P1 266
#define CU_SIZE_NUM_P2 266
#define CU_SIZE_NUM_P3 266
#define CU_SIZE_NUM_P4 810

#define MAX_CU_SIZE_SPEL		(1 << 6)
#define PRED_WINDOW_STRIDE		((MAX_CU_SIZE_SPEL << 2) + 1)
#define SUB_PEL_ME_NUM          2  // 1/4pel & 1/2pel
#define ENABLE_EQT_ME 0

#define max_ime(x, y, type) ((((type)(x)) > ((type)(y))) ? ((type)(x)) : ((type)(y)))
#define min_ime(x, y, type) ((((type)(x)) < ((type)(y))) ? ((type)(x)) : ((type)(y)))
#define COM_CLIP3_IME(min_x, max_x, value, type)   max_ime((min_x), min_ime((max_x), (value),type),type)
#ifdef VIVADO_HLS
#define COM_ABS14(a)                   (((a)^((a)>>13)) - ((a)>>13))
#else
#define COM_ABS14(a)                   COM_ABS16(a)
#endif
#define MCU_GET_INTRA_FLAG_IME(m)           (S32)(((m)>>15) & 1)
#define MCU_GET_CODED_FLAG_IME(m)           (S32)(((m)>>31) & 1)

typedef struct _ENC_ME_NEB_INFO_BW
{
	S32 scu;
	S14 mv[REFP_NUM][MV_D];
	S2 refi[REFP_NUM];
	U1 is_valid;
#if TB_SPLIT_EXT
	U32                    pb_tb_part;
#endif
}ENC_ME_NEB_INFO_BW;

typedef struct _ENC_ME_LINE_MAP_BW
{
	//int min_scu;
	U10 min_scu_x;
	U10 min_scu_y;
	S32 stride_in_map;
	S32 stride_in_line;
	U10 pic_width_in_scu;
	U10 pic_height_in_scu;
	U1 above_line_idx;
	U1 curr_ctu_idx;
	U10 x_ctu;
	U10 y_ctu;
	/* LCU split information */
	//S8                     line_map_split[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];

	//#if  !USE_IME_LINE_COMP
	//ENC_ME_NEB_INFO above_nebs[SCU_IN_LCU_NUMS_LEN + 2];
	//u32 map_scu_above[SCU_IN_LCU_NUMS_LEN + 2];
	//ENC_ME_NEB_INFO left_nebs[SCU_IN_LCU_NUMS_LEN];
	//u32 map_scu_left[SCU_IN_LCU_NUMS_LEN];
	//#endif

	//ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];

	//ENC_ME_NEB_INFO above_nebs_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	U32 map_scu_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	U32 map_cu_mode_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	S14 co_left_mv[SCU_IN_LCU_NUMS_LEN][MV_D];
	S2 co_left_refi[SCU_IN_LCU_NUMS_LEN];
	//ENC_ME_NEB_INFO left_nebs_ctu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	//u32 map_scu_left_ctu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];

	/* buffer */
	S2 refi_nebs[NEB_NUMS];
	u32 scu_nebs[NEB_NUMS];
	S14 mv_nebs[NEB_NUMS][MV_D];
	U1 if_init;
}ENC_ME_LINE_MAP_BW;

typedef struct {
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2];

	pel y_org[64][64];
	pel u_org[32][32];
	pel v_org[32][32];
	pel y_orgT[64][64];
	pel y_org_lv1_2d[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	pel y_org_lv2_2d[MAX_CU_SIZE_FETCH >> 2][MAX_CU_SIZE_FETCH >> 2];
} strFetch_Orig_Lcu_BW;

typedef struct {
	S12 ref_left;
	S12 ref_right;
	S12 ref_up;
	S12 ref_down;
	pel RefWindowLCU[SWW * SWH];
	pel RefWindowLCU_U[SWH / 2][SWW / 2];
	pel RefWindowLCU_V[SWH / 2][SWW / 2];
	S12 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP[(SWH * SWW) >> 2];
	pel swOutLv0[320][320];
	pel swOutLv1[160][160];
	pel swOutLv2[80][80];
	U12 x_chroma;
	U12 y_chroma;
	BOOL last_child;
} strFetch_ref_window_BW;

typedef struct _ENC_ME_CU_BW
{

	S12 info_addr_map[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN][MAX_CU_DEPTH][MAX_CU_DEPTH];
	S12 info_addr_map_split[MAX_CU_DEPTH * MAX_CU_DEPTH][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 me_cnt;
	U1 has_partitioned;
	U11 info_idx;
	U2 refp_num;
	U11 pic_width_in_scu;
	U11 pic_height_in_scu;
	U11 info_idx32;
	U11 info_idx16;
	U11 info_idx8;
	U11 info_idx4;
	S14 swSubLv0TopLeft[MV_D];
}ENC_ME_CU_BW;

//**********************************************************************************************
typedef struct _ENC_ME_LINE_MAP_IME
{
	U32 new_map_scu_above_line[ABOVE_LINE_LEN];

	U11 min_scu_x;
	//U11 min_scu_y;

	S14 co_left_mv[SCU_IN_LCU_NUMS_LEN][MV_D];
	S2 co_left_refi[SCU_IN_LCU_NUMS_LEN];
	S16 co_left_ptr_ref[SCU_IN_LCU_NUMS_LEN];


}ENC_ME_LINE_MAP_IME;

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

typedef struct _ENC_ME_NEB_INFO
{
	//U28 scu;
	S14 mv[REFP_NUM][MV_D];
	S2 refi[REFP_NUM];
	//	U1 is_valid;
	//#if TB_SPLIT_EXT
	//	U32                    pb_tb_part;
	//#endif

}ENC_ME_NEB_INFO;

typedef struct __strFetch_ref_window {
	/*U13 ref_left;
	U13 ref_right;
	U13 ref_up;
	U13 ref_down;*/
	pel RefWindowLCU[SWW * SWH];
	//U8 RefWindowLCU_2D[SWH][SWW];
	//U8 RefWindowLCU_U[SWH / 2][SWW / 2];
	//U8 RefWindowLCU_V[SWH / 2][SWW / 2];
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//S14 CtrPos_IME[RANGE_NUM][DIR_DIM][MAP_DIMS];
	pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP[(SWH * SWW) >> 2];
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
	pel RefWindowLCU_ref1[SWW * SWH];
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2];
#endif
} strFetch_ref_window;

typedef struct __strFetch_Orig_Lcu {
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_Y_rmd[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2];

	//u8 p_fenc_LCU_Y_u8[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	//u8 p_fenc_LCU_UV_u8[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2];

	pel y_org[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel u_org[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
	pel v_org[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
	pel y_org_sao[64][64];
	pel u_org_sao[32][32];
	pel v_org_sao[32][32];
	//U8 y_orgT[64][64];
	pel y_orgT_fme[64][64];
} strFetch_Orig_Lcu;

typedef struct _FETCH2MD_FW {
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	//U7 max_dt_size;
	U1 affine_enable_flag;
	//U1 dt_intra_enable_flag;//
	//U4 num_of_hmvp_cand;
	//U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	//U6 pic_width_in_lcu;
	//U6 pic_height_in_lcu;
	//U3               chroma_quant_param_delta_cb;
	//U3               chroma_quant_param_delta_cr;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;
	//U21 f_scu;
	/* gop size */
	//U6             gop_size;

#if CTX_CONTROL_LUKE
	//int				 width;
	//int				 height;
	U4              bit_depth_internal;
	U4              max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	//U1				 affine_subblock_size_idx;
	//U1				 ipcm_enable_flag;
	//U1			 position_based_transform_enable_flag;
	//U1              secondary_transform_enable_flag;
	//U1              tscpm_enable_flag;
	//U1				 amvr_enable_flag;
	//U1				 emvr_enable_flag;
	//int				 affine_enable_flag;
#endif

	S16 refp_ptr[17][2];
#if ENABLE_BFRAME
	U5 num_refp[REFP_NUM];
#else
	U5 num_refp[REFP_NUM];
#endif

	U9 cur_ptr; // ref pic

}FETCH2MD_FW;

typedef struct {
	U12 pix_x;
	U12 pix_y;
#if 1//CTU_CONTROL_LUKE
	//int  LCU_nr;
	/* total count of remained LCU for encoding one picture. if a picture is
	encoded properly, this value should reach to zero */
	int  lcu_cnt;
	int  lcu_num;
#endif
	//int  smb_num_width;
	//int  smb_num_height;
#if 1//CTU_CONTROL_LUKE

	/* QP for luma of current encoding CU */
	int             qp_y;
	/* QP for chroma of current encoding CU */
	int             qp_u;
	int             qp_v;
	u32             lambda_mv;
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
	//	ENC_ME_LINE_MAP *me_line_map_ptr;
} CTU_Level_Parameters;

#if ENABLE_BFRAME
typedef struct _MV_DIR
{
	U1 lidx;
	S8 cur_refi;
} MV_DIR;

typedef struct _ME_MV_DIR_BW
{
	MV_DIR block64x64MvDir[1][1];
	MV_DIR block32x32MvDir[2][2];
	MV_DIR block64x32MvDir[1][2];
	MV_DIR block32x64MvDir[2][1];
	MV_DIR block16x16MvDir[4][4];
	MV_DIR block32x16MvDir[2][4];
	MV_DIR block16x32MvDir[4][2];
	MV_DIR block64x16MvDir[1][4];
	MV_DIR block16x64MvDir[4][1];
	MV_DIR block8x8MvDir[8][8];
	MV_DIR block16x8MvDir[4][8];
	MV_DIR block8x16MvDir[8][4];
	MV_DIR block32x8MvDir[2][8];
	MV_DIR block8x32MvDir[8][2];
	MV_DIR block64x8MvDir[1][8];
	MV_DIR block8x64MvDir[8][1];
	MV_DIR block32x4MvDir[2][16];
	MV_DIR block4x32MvDir[16][2];
	MV_DIR block16x4MvDir[4][16];
	MV_DIR block4x16MvDir[16][4];
	MV_DIR block8x4MvDir[8][16];
	MV_DIR block4x8MvDir[16][8];
}ME_MV_DIR;

typedef struct _ME_MV_DIR_BW_16
{
	MV_DIR block16x16MvDir[4][4];
	MV_DIR block16x8MvDir[4][8];
	MV_DIR block8x16MvDir[8][4];
}ME_MV_DIR_16;
#endif


//**********************************************************************************************



#else

#include "enc_def.h"
#include "com_def.h"
#include <math.h>
#include <string>
#include<vector>
#include<map>
#include <fstream>
#include <iostream>

#define DEBUG 0
#define STAT 0

#define SPLIT_MAP_STRIDE *MAX_CU_DEPTH
#define SW_LEN_LV0 (256)+MAX_CU_SIZE2

#define SW_LEN_LV0_V (SWW)//((REFP_SURD_OFF_W * 2) + MAX_CU_SIZE2)
#define SW_LEN_LV0_H (SWH)//((REFP_SURD_OFF_H * 2) + MAX_CU_SIZE2)

#define CU_SIZE_NUM_P1 266
#define CU_SIZE_NUM_P2 266
#define CU_SIZE_NUM_P3 266
#define CU_SIZE_NUM_P4 810

#define MAX_CU_SIZE_SPEL		(1 << 6)
#define PRED_WINDOW_STRIDE		((MAX_CU_SIZE_SPEL << 2) + 1)
#define SUB_PEL_ME_NUM          2  // 1/4pel & 1/2pel
#define ENABLE_EQT_ME 0

#define max_ime(x, y, type) ((((type)(x)) > ((type)(y))) ? ((type)(x)) : ((type)(y)))
#define min_ime(x, y, type) ((((type)(x)) < ((type)(y))) ? ((type)(x)) : ((type)(y)))
#define COM_CLIP3_IME(min_x, max_x, value, type)   max_ime((min_x), min_ime((max_x), (value),type),type)
#ifdef VIVADO_HLS
#define COM_ABS14(a)                   (((a)^((a)>>13)) - ((a)>>13))
#else
#define COM_ABS14(a)                   COM_ABS16(a)
#endif
#define MCU_GET_INTRA_FLAG_IME(m)           (S32)(((m)>>15) & 1)
#define MCU_GET_CODED_FLAG_IME(m)           (S32)(((m)>>31) & 1)

typedef struct _ENC_ME_NEB_INFO_BW
{
	S32 scu;
	S14 mv[REFP_NUM][MV_D];
	S2 refi[REFP_NUM];
	U1 is_valid;
#if TB_SPLIT_EXT
	U32                    pb_tb_part;
#endif
}ENC_ME_NEB_INFO_BW;

typedef struct _ENC_ME_LINE_MAP_BW
{
	//int min_scu;
	U10 min_scu_x;
	U10 min_scu_y;
	S32 stride_in_map;
	S32 stride_in_line;
	U10 pic_width_in_scu;
	U10 pic_height_in_scu;
	U1 above_line_idx;
	U1 curr_ctu_idx;
	U10 x_ctu;
	U10 y_ctu;
	/* LCU split information */
	//S8                     line_map_split[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];

	//#if  !USE_IME_LINE_COMP
	//ENC_ME_NEB_INFO above_nebs[SCU_IN_LCU_NUMS_LEN + 2];
	//u32 map_scu_above[SCU_IN_LCU_NUMS_LEN + 2];
	//ENC_ME_NEB_INFO left_nebs[SCU_IN_LCU_NUMS_LEN];
	//u32 map_scu_left[SCU_IN_LCU_NUMS_LEN];
	//#endif

	//ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];

	//ENC_ME_NEB_INFO above_nebs_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	U32 map_scu_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	U32 map_cu_mode_above_line[2][MAX_WIDTH >> MIN_CU_LOG2];
	S14 co_left_mv[SCU_IN_LCU_NUMS_LEN][MV_D];
	S2 co_left_refi[SCU_IN_LCU_NUMS_LEN];
	//ENC_ME_NEB_INFO left_nebs_ctu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	//u32 map_scu_left_ctu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];

	/* buffer */
	S2 refi_nebs[NEB_NUMS];
	u32 scu_nebs[NEB_NUMS];
	S14 mv_nebs[NEB_NUMS][MV_D];
	U1 if_init;
}ENC_ME_LINE_MAP_BW;

typedef struct {
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2];

	pel y_org[64][64];
	pel u_org[32][32];
	pel v_org[32][32];

	pel y_org_lv1_2d[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	pel y_org_lv2_2d[MAX_CU_SIZE_FETCH >> 2][MAX_CU_SIZE_FETCH >> 2];
} strFetch_Orig_Lcu_BW;

typedef struct {
	S12 ref_left;
	S12 ref_right;
	S12 ref_up;
	S12 ref_down;
	pel RefWindowLCU[SWW * SWH];
	pel RefWindowLCU_U[SWH / 2][SWW / 2];
	pel RefWindowLCU_V[SWH / 2][SWW / 2];
	S12 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP[(SWH * SWW) >> 2];
	pel swOutLv0[320][320];
	pel swOutLv1[160][160];
	pel swOutLv2[80][80];
	U12 x_chroma;
	U12 y_chroma;
	BOOL last_child;
} strFetch_ref_window_BW;

typedef struct _ENC_ME_CU_BW
{

	S12 info_addr_map[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN][MAX_CU_DEPTH][MAX_CU_DEPTH];
	S12 info_addr_map_split[MAX_CU_DEPTH * MAX_CU_DEPTH][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U32 me_cnt;
	U1 has_partitioned;
	U11 info_idx;
	U2 refp_num;
	U11 pic_width_in_scu;
	U11 pic_height_in_scu;
	U11 info_idx32;
	U11 info_idx16;
	U11 info_idx8;
	U11 info_idx4;
	S14 swSubLv0TopLeft[MV_D];
}ENC_ME_CU_BW;



#endif	// #if VIVADO_HLS_IME

#endif

typedef struct __FETCH_REF_WINDOW_IME {
#if BW_DEBUG
	U10 swOutLv0[SWH][SWW];
#else
	pel swOutLv0[SWH][SWW];
#endif
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	//pel RefWindowLCU_VP[(SWH * SWW) >> 2];

#if ENABLE_BFRAME
	//int code_stru;
	U1 lidx[REFP_NUM];
	S8 cur_refi[REFP_NUM];
	//int refp_pic_ptr[MAX_NUM_REF_PICS][REFP_NUM];
	//int refp_is_library_picture[MAX_NUM_REF_PICS][REFP_NUM];

	S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
#if BW_DEBUG
	U10 swOutLv0_ref1[SWH][SWW];
#else
	pel swOutLv0_ref1[SWH][SWW];
#endif
	//pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2];
	//pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2];
#endif
} FETCH_REF_WINDOW_IME;

#if BW_DEBUG
typedef struct __FETCH_ORGI_LCU_IME {
	U10 y_org[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	U10 u_org[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	U10 v_org[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
} FETCH_ORGI_LCU_IME;
#else
typedef struct __FETCH_ORGI_LCU_IME {
	pel y_org[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel u_org[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	pel v_org[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
} FETCH_ORGI_LCU_IME;
#endif

typedef struct _ME_MV_INFO_BW
{
	S14 sadTree64x64Mv[1][1][2];
	S14 sadTree32x32Mv[2][2][2];
	S14 sadTree64x32Mv[1][2][2];
	S14 sadTree32x64Mv[2][1][2];
	S14 sadTree16x16Mv[4][4][2];
	S14 sadTree32x16Mv[2][4][2];
	S14 sadTree16x32Mv[4][2][2];
	S14 sadTree64x16Mv[1][4][2];
	S14 sadTree16x64Mv[4][1][2];
	S14 sadTree8x8Mv  [8][8][2];
	S14 sadTree16x8Mv [4][8][2];
	S14 sadTree8x16Mv [8][4][2];
	S14 sadTree32x8Mv [2][8][2];
	S14 sadTree8x32Mv [8][2][2];
	S14 sadTree64x8Mv [1][8][2];
	S14 sadTree8x64Mv [8][1][2];
	//S14 sadTree4x8Mv[16][8][2];
	//S14 sadTree8x4Mv[8][16][2];
	//S14 sadTree16x4Mv[4][16][2];
	//S14 sadTree4x16Mv[16][4][2];
	//S14 sadTree32x4Mv[2][16][2];
	//S14 sadTree4x32Mv[16][2][2];

#if PRINT_MV
	template<unsigned H,unsigned V>
	void printMVArray(S14 mvArray[H][V][2])
	{
		for (int i=0;i<H;i++)
		{
			for (int j=0;j<V;j++)
			{
				printf("[%d,%d],", mvArray[i][j][MV_X], mvArray[i][j][MV_Y]);
			}
			printf("\n");
		}
	}
	void printMV()
	{	
		printf("64x64Mv\n"); printMVArray<  1, 1 >(sadTree64x64Mv);
		printf("32x32Mv\n"); printMVArray < 2, 2 >(sadTree32x32Mv);
		printf("64x32Mv\n"); printMVArray < 1, 2 >(sadTree64x32Mv);
		printf("32x64Mv\n"); printMVArray < 2, 1 >(sadTree32x64Mv);
		printf("16x16Mv\n"); printMVArray < 4, 4 >(sadTree16x16Mv);
		printf("32x16Mv\n"); printMVArray < 2, 4 >(sadTree32x16Mv);
		printf("16x32Mv\n"); printMVArray < 4, 2 >(sadTree16x32Mv);
		printf("64x16Mv\n"); printMVArray < 1, 4 >(sadTree64x16Mv);
		printf("16x64Mv\n"); printMVArray < 4, 1 >(sadTree16x64Mv);
		printf("8x8Mv\n"); printMVArray < 8, 8 >(sadTree8x8Mv);
		printf("16x8Mv\n"); printMVArray < 4, 8 >(sadTree16x8Mv);
		printf("8x16Mv\n"); printMVArray < 8, 4 >(sadTree8x16Mv);
		printf("32x8Mv\n"); printMVArray < 2, 8 >(sadTree32x8Mv);
		printf("8x32Mv\n"); printMVArray < 8, 2 >(sadTree8x32Mv);
		printf("64x8Mv\n"); printMVArray < 1, 8 >(sadTree64x8Mv);
		printf("8x64Mv\n");printMVArray < 8,1 >(sadTree8x64Mv );	
	}
#endif

}ME_MV_INFO_BW;

typedef struct _ME_PMV_INFO_BW
{
	S14 sadTree64x64PMv[1][1][2];
	S14 sadTree32x32PMv[2][2][2];
	S14 sadTree64x32PMv[1][2][2];
	S14 sadTree32x64PMv[2][1][2];
	S14 sadTree16x16PMv[4][4][2];
	S14 sadTree32x16PMv[2][4][2];
	S14 sadTree16x32PMv[4][2][2];
	S14 sadTree64x16PMv[1][4][2];
	S14 sadTree16x64PMv[4][1][2];
	S14 sadTree8x8PMv[8][8][2];
	S14 sadTree16x8PMv[4][8][2];
	S14 sadTree8x16PMv[8][4][2];
	S14 sadTree32x8PMv[2][8][2];
	S14 sadTree8x32PMv[8][2][2];
	S14 sadTree64x8PMv[1][8][2];
	S14 sadTree8x64PMv[8][1][2];
	//S14 sadTree4x8PMv[16][8][2];
	//S14 sadTree8x4PMv[8][16][2];
	//S14 sadTree16x4PMv[4][16][2];
	//S14 sadTree4x16PMv[16][4][2];
	//S14 sadTree32x4PMv[2][16][2];
	//S14 sadTree4x32PMv[16][2][2];

#if PRINT_MV
	template<unsigned H, unsigned V>
	void printMVArray(S14 mvArray[H][V][2])
	{
		for (int i = 0; i < H; i++)
		{
			for (int j = 0; j < V; j++)
			{
				printf("[%d,%d],", mvArray[i][j][MV_X], mvArray[i][j][MV_Y]);
			}
			printf("\n");
		}
	}
	void printMV()
	{
		printf("64x64Mv\n"); printMVArray<  1, 1 >(sadTree64x64PMv);
		printf("32x32Mv\n"); printMVArray < 2, 2 >(sadTree32x32PMv);
		printf("64x32Mv\n"); printMVArray < 1, 2 >(sadTree64x32PMv);
		printf("32x64Mv\n"); printMVArray < 2, 1 >(sadTree32x64PMv);
		printf("16x16Mv\n"); printMVArray < 4, 4 >(sadTree16x16PMv);
		printf("32x16Mv\n"); printMVArray < 2, 4 >(sadTree32x16PMv);
		printf("16x32Mv\n"); printMVArray < 4, 2 >(sadTree16x32PMv);
		printf("64x16Mv\n"); printMVArray < 1, 4 >(sadTree64x16PMv);
		printf("16x64Mv\n"); printMVArray < 4, 1 >(sadTree16x64PMv);
		printf("8x8Mv\n"); printMVArray < 8, 8 >(sadTree8x8PMv);
		printf("16x8Mv\n"); printMVArray < 4, 8 >(sadTree16x8PMv);
		printf("8x16Mv\n"); printMVArray < 8, 4 >(sadTree8x16PMv);
		printf("32x8Mv\n"); printMVArray < 2, 8 >(sadTree32x8PMv);
		printf("8x32Mv\n"); printMVArray < 8, 2 >(sadTree8x32PMv);
		printf("64x8Mv\n"); printMVArray < 1, 8 >(sadTree64x8PMv);
		printf("8x64Mv\n"); printMVArray < 8, 1 >(sadTree8x64PMv);
	}
#endif

}ME_PMV_INFO_BW;


typedef struct _IME_OUT_PARA_SET
{
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	//U7 max_dt_size;
	U1 affine_enable_flag;
	//U1 dt_intra_enable_flag;//
	//U4 num_of_hmvp_cand;
	//U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;
	//U21 f_scu;
	/* gop size */
	//U6             gop_size;
#if EIPM
	int              eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
#if CTX_CONTROL_LUKE
	//int				 width;
	//int				 height;
	U4              bit_depth_internal;
	U4               max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	//U1				 affine_subblock_size_idx;
	//U1				 ipcm_enable_flag;
	//U1				 position_based_transform_enable_flag;
	//U1               secondary_transform_enable_flag;
	//U1               tscpm_enable_flag;
	//U1		 		 amvr_enable_flag;
	//U1	  	   	     emvr_enable_flag;
	//int				 affine_enable_flag;
#endif

	int refp_ptr[17][2];
#if ENABLE_BFRAME
	U5 num_refp[REFP_NUM];
	S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 swSubLv0TopLeft[REFP_NUM][MV_D];
#else
	U5 num_refp[REFP_NUM];
	S14 swSubLv0TopLeft[MV_D];
#endif
	U9 cur_ptr; // ref pic

	U13 pix_x;
	U13 pix_y;
	//#if CTU_CONTROL_LUKE
	//	int  lcu_cnt;
	//#endif
	//	int  smb_num_width;
	//	int  smb_num_height;
#if CTU_CONTROL_LUKE
	/* QP for luma of current encoding CU */
	U6             qp_y;
	/* QP for chroma of current encoding CU */
	U6             qp_u;
	U6             qp_v;
	U32             lambda_mv;							//// need to give fme
	S64             lambda[3];
	//S64             sqrt_lambda[3];
	S64             dist_chroma_weight[2];
	/* current frame numbser */
	//int             ptr;
	U16				x_last_ctu;
	U16				y_last_ctu;
#endif
#if UMVE_ENH
	u8               umve_set_flag;
#endif
	/* strFetch_ref_window */

	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];			//// need to give fme
	S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//U12 x_chroma;
	//U12 y_chroma;
	//U1 last_child;

	pel y_org[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel u_org[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	pel v_org[MAX_CU_SIZE_FETCH >> 1][MAX_CU_SIZE_FETCH >> 1];
	//pel RefWindowLCU[SWW * SWH];
	//pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	//pel RefWindowLCU_VP[(SWH * SWW) >> 2];

#if ENABLE_BFRAME
	S14 offset_fme_sww_h[MV_D];
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 offset_fme_sww_h_ref1[MV_D];
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
#else
	S14 offset_fme_sww_h[MV_D];
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];
#endif
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
#if USE_ROI_MODE_CONFIG
	U1 valid_mode[3];//valid_mode[0] skip/merge/direct;valid_mode[1] inter;valid_mode[2] intra
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	int md_slice_qp;
	int CuQPMap[256];

#endif
}IME_OUT_PARA_SET;

typedef struct _FME_OUT_PARA_SET_COPY
{
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	//U7 max_dt_size;
	U1 affine_enable_flag;
	//U1 dt_intra_enable_flag;//
	//U4 num_of_hmvp_cand;
	//U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;
	//U21 f_scu;
	/* gop size */
	//U6             gop_size;

#if CTX_CONTROL_LUKE
	//int				 width;
	//int				 height;
	U4              bit_depth_internal;
	U4               max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	//U1				 affine_subblock_size_idx;
	//U1				 ipcm_enable_flag;
	//U1				 position_based_transform_enable_flag;
	//U1               secondary_transform_enable_flag;
	//U1               tscpm_enable_flag;
	//U1		 		 amvr_enable_flag;
	//U1	  	   	     emvr_enable_flag;
	//int				 affine_enable_flag;
#endif

	int refp_ptr[17][2];
#if ENABLE_BFRAME
	U5 num_refp[REFP_NUM];
	S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
#else
	U5 num_refp[REFP_NUM];
#endif
	U9 cur_ptr; // ref pic

	U13 pix_x;
	U13 pix_y;
	//#if CTU_CONTROL_LUKE
	//	int  lcu_cnt;
	//#endif
	//	int  smb_num_width;
	//	int  smb_num_height;
#if EIPM
	int              eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
#if CTU_CONTROL_LUKE
	/* QP for luma of current encoding CU */
	U6             qp_y;
	/* QP for chroma of current encoding CU */
	U6             qp_u;
	U6             qp_v;
	//U32             lambda_mv;
	S64             lambda[3];
	//S64             sqrt_lambda[3];
	S64             dist_chroma_weight[2];
	/* current frame numbser */
	//int             ptr;
	U16				x_last_ctu;
	U16				y_last_ctu;
#endif
#if UMVE_ENH
	u8               umve_set_flag;
#endif
	/* strFetch_ref_window */

	//S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//U12 x_chroma;
	//U12 y_chroma;
	//U1 last_child;
	S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];

	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
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
#if USE_ROI_MODE_CONFIG
	U1 valid_mode[3];//valid_mode[0] skip/merge/direct;valid_mode[1] inter;valid_mode[2] intra
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	int md_slice_qp;
	int CuQPMap[256];
#endif
}FME_OUT_PARA_SET_COPY;

typedef struct _FME_OUT_PARA_SET
{
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	//U7 max_dt_size;
	U1 affine_enable_flag;
	//U1 dt_intra_enable_flag;//
	//U4 num_of_hmvp_cand;
	//U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;
	//U21 f_scu;
	/* gop size */
	//U6             gop_size;

#if CTX_CONTROL_LUKE
	//int				 width;
	//int				 height;
	U4              bit_depth_internal;
	U4               max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	//U1				 affine_subblock_size_idx;
	//U1				 ipcm_enable_flag;
	//U1				 position_based_transform_enable_flag;
	//U1               secondary_transform_enable_flag;
	//U1               tscpm_enable_flag;
	//U1		 		 amvr_enable_flag;
	//U1	  	   	     emvr_enable_flag;
	//int				 affine_enable_flag;
#endif

	int refp_ptr[17][2];
#if ENABLE_BFRAME
	U5 num_refp[REFP_NUM];
	S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
#else
	U5 num_refp[REFP_NUM];
#endif
	U9 cur_ptr; // ref pic

	U13 pix_x;
	U13 pix_y;
	//#if CTU_CONTROL_LUKE
	//	int  lcu_cnt;
	//#endif
	//	int  smb_num_width;
	//	int  smb_num_height;
#if EIPM
	int              eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
#if CTU_CONTROL_LUKE
	/* QP for luma of current encoding CU */
	U6             qp_y;
	/* QP for chroma of current encoding CU */
	U6             qp_u;
	U6             qp_v;
	//U32             lambda_mv;
	S64             lambda[3];
	//S64             sqrt_lambda[3];
	S64             dist_chroma_weight[2];
	/* current frame numbser */
	//int             ptr;
	U16				x_last_ctu;
	U16				y_last_ctu;
#endif
#if UMVE_ENH
	u8               umve_set_flag;
#endif
	/* strFetch_ref_window */

	//S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//U12 x_chroma;
	//U12 y_chroma;
	//U1 last_child;
	S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];

	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
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
#if USE_ROI_MODE_CONFIG
	U1 valid_mode[3];//valid_mode[0] skip/merge/direct;valid_mode[1] inter;valid_mode[2] intra
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	int md_slice_qp;
	int CuQPMap[256];
#endif
}FME_OUT_PARA_SET;

typedef struct _FME_OUT_PARA_SET_CHROMA
{
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	//U7 max_dt_size;
	U1 affine_enable_flag;
	//U1 dt_intra_enable_flag;//
	//U4 num_of_hmvp_cand;
	//U1  ipf_enable_flag;
	U11  pic_width_in_scu;
	U11  pic_height_in_scu;
	U3  log2_max_cuwh;
	U3  log2_culine;
	U7 max_cuwh;
	//U21 f_scu;
	/* gop size */
	//U6             gop_size;

#if CTX_CONTROL_LUKE
	//int				 width;
	//int				 height;
	U4              bit_depth_internal;
	U4               max_part_ratio;
	U3               min_cu_size;
	U3               max_split_times;
	U4               min_qt_size;
	U7               max_bt_size;
	U7               max_eqt_size;
	//U1				 affine_subblock_size_idx;
	//U1				 ipcm_enable_flag;
	//U1				 position_based_transform_enable_flag;
	//U1               secondary_transform_enable_flag;
	//U1               tscpm_enable_flag;
	//U1		 		 amvr_enable_flag;
	//U1	  	   	     emvr_enable_flag;
	//int				 affine_enable_flag;
#endif

	int refp_ptr[17][2];
#if ENABLE_BFRAME
	U5 num_refp[REFP_NUM];
	S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
#else
	U5 num_refp[REFP_NUM];
#endif
	U9 cur_ptr; // ref pic

	U13 pix_x;
	U13 pix_y;
	//#if CTU_CONTROL_LUKE
	//	int  lcu_cnt;
	//#endif
	//	int  smb_num_width;
	//	int  smb_num_height;
#if EIPM
	int              eipm_enable_flag;
#endif
#if MIPF
	int              mipf_enable_flag;
#endif
#if INTERPF
	int				 interpf_enable_flag;
#endif
#if CTU_CONTROL_LUKE
	/* QP for luma of current encoding CU */
	U6             qp_y;
	/* QP for chroma of current encoding CU */
	U6             qp_u;
	U6             qp_v;
	//U32             lambda_mv;
	S64             lambda[3];
	//S64             sqrt_lambda[3];
	S64             dist_chroma_weight[2];
	/* current frame numbser */
	//int             ptr;
	U16				x_last_ctu;
	U16				y_last_ctu;
#endif
#if UMVE_ENH
	u8               umve_set_flag;
#endif
	/* strFetch_ref_window */

	//S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
	//U12 x_chroma;
	//U12 y_chroma;
	//U1 last_child;
	S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS];
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS];

	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2];
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
#if USE_ROI_MODE_CONFIG
	U1 valid_mode[3];//valid_mode[0] skip/merge/direct;valid_mode[1] inter;valid_mode[2] intra
#endif
#if CUDQP_QP_MAP
	int cu_dqp_enable;
	int cu_qp_group_area_size;
	int cu_qp_group_pred_qp;
	s8 chroma_quant_param_delta_cb;
	s8 chroma_quant_param_delta_cr;
	int md_slice_qp;
	int CuQPMap[256];
#endif
}FME_OUT_PARA_SET_CHROMA;

typedef struct _FME_OUT_PARA_SET_AXIS
{
	U13 pic_width;
	U13 pic_height;
	U2  slice_type;
	U7  max_cuwh;
	U11 pic_width_in_scu;
	U11 pic_height_in_scu;
	U3  log2_max_cuwh;
	U3  log2_culine;
#if CTX_CONTROL_LUKE
	U4  bit_depth_internal;
	U4  max_part_ratio;
	U3  min_cu_size;
	U3  max_split_times;
	U4  min_qt_size;
	U7  max_bt_size;
	U7  max_eqt_size;
#endif
	U9  cur_ptr;
	U13 pix_x;
	U13 pix_y;
#if EIPM
	int  eipm_enable_flag;
#endif
#if MIPF
	int  mipf_enable_flag;
#endif
#if INTERPF
	int  interpf_enable_flag;
#endif
#if CTU_CONTROL_LUKE
	U6  qp_y;
	U16 x_last_ctu;
	U16 y_last_ctu;
#endif
#if UMVE_ENH
	u8  umve_set_flag;
#endif
#if CUDQP_QP_MAP
	int cu_qp_group_pred_qp;
#endif
}FME_OUT_PARA_SET_AXIS;

typedef struct _ME_MV_COST_BW
{
	U32 sadTree64x64Best[1][1];
	U32 sadTree32x32Best[2][2];
	U32 sadTree64x32Best[1][2];
	U32 sadTree32x64Best[2][1];
	U32 sadTree16x16Best[4][4];
	U32 sadTree32x16Best[2][4];
	U32 sadTree16x32Best[4][2];
	U32 sadTree64x16Best[1][4];
	U32 sadTree16x64Best[4][1];
	U32 sadTree64x8Best[1][8];
	U32 sadTree8x64Best[8][1];
	U16 sadTree8x8Best[8][8];
	U16 sadTree16x8Best[4][8];
	U16 sadTree8x16Best[8][4];
	U16 sadTree32x8Best[2][8];
	U16 sadTree8x32Best[8][2];
}ME_MV_COST_BW;


class FilePrinter
{
private:
	std::string baseDirName;
	int elementNum;
	int maxElementNum;
	int elementNumPerFrame;
	int maxElementPerFrame;
	int lastFrameNumber;
public:
	FilePrinter(std::string baseDirName, int maxElementNum, int maxElementPerFrame);
	bool printAtribute(std::string atriName, size_t size, void* ptr, int frameNumber);
	bool continueWriting(int frameNumber);
	void increaseElement();
};

class FileReader
{
private:
	std::string baseDirName;
	int elementNum;
	int maxElementNum;
	std::map <std::string, size_t> offsetMapper;

public:
	FileReader(std::string baseDirName, int maxElementNum);
	bool readAtribute(std::string atriName, size_t size, void* ptr);
	bool continueReading();
	/* compare */
	bool comparePMv(ME_PMV_INFO* pmvRead, ME_PMV_INFO* pmvWrite);
	bool compareMv(ME_MV_INFO* mvRead, ME_MV_INFO* mvWrite);
	bool comparePMv(ME_PMV_INFO_BW* pmvRead, ME_PMV_INFO_BW* pmvWrite);
	bool compareMv(ME_MV_INFO_BW* mvRead, ME_MV_INFO_BW* mvWrite);
	void increaseElement();
	int getElementCnt() { return elementNum; }
};


void ime_run(
	ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
	ENC_ME_NEB_INFO new_above_nebs_line_IME[ABOVE_LINE_LEN],
	FETCH_REF_WINDOW_IME* Fetch_Ref_Window_IME,
	FETCH_ORGI_LCU_IME* Fetch_Orig_Lcu_IME,
	FETCH2MD_FW* fetch2md_fw_ptr,
//#if REFP_PTR_FEBUG
//	S16 refp_ptr[17][2],
//#endif
	CTU_Level_Parameters* fetch_output,
	ME_MV_INFO_BW* me_mv_info,
	ME_PMV_INFO_BW* me_pmv_info,
#if BW_DEBUG
	U10 RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE],
#else
	pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE],
#endif
	IME_OUT_PARA_SET* ime_out_para_set,
	U32 ime_64x64_best_cost[2]
#if ENABLE_BFRAME
	, ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME_1,
	ENC_ME_NEB_INFO new_above_nebs_line_IME_1[ABOVE_LINE_LEN]
#if BW_DEBUG
	, U10 RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE],
#else
	, pel RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE],
#endif
	ME_MV_DIR* me_mv_dir
#endif
);

#if PRINT_IME_MV
void printMv(ME_MV_INFO_BW* me_mv_info, int x/*, int y*/);
#endif


#if PRINT_sadScuUp_1

typedef struct _ME_MV_SAD_UP_COST_BW
{
	U16 sadScuUp0[8][8];
	U16 sadScuUp1[8][8];
	U16 sadScuUp2[8][8];
	U16 sadScuUp3[8][8];
	U16 sadScuUp4[8][8];
	U16 sadScuUp5[8][8];
	U16 sadScuUp6[8][8];
	U16 sadScuUp7[8][8];
	U16 sadScuUp8[8][8];
	U16 sadScuUp9[8][8];
	U16 sadScuUp10[8][8];
	U16 sadScuUp11[8][8];
	U16 sadScuUp12[8][8];
	U16 sadScuUp13[8][8];
	U16 sadScuUp14[8][8];
	U16 sadScuUp15[8][8];

} ME_MV_SAD_UP_COST_BW;
void print_sadScuUp_1(ME_MV_SAD_UP_COST_BW* me_mvsad_up_cost, int y);

void Copy_SadUpCost(
	ME_MV_SAD_UP_COST_BW* me_mvsad_up_cos,
	U16 sadScuUp0[8][8], U16 sadScuUp1[8][8], U16 sadScuUp2[8][8], U16 sadScuUp3[8][8],
	U16 sadScuUp4[8][8], U16 sadScuUp5[8][8], U16 sadScuUp6[8][8], U16 sadScuUp7[8][8],
	U16 sadScuUp8[8][8], U16 sadScuUp9[8][8], U16 sadScuUp10[8][8], U16 sadScuUp11[8][8],
	U16 sadScuUp12[8][8], U16 sadScuUp13[8][8], U16 sadScuUp14[8][8], U16 sadScuUp15[8][8]);

#endif


#endif /* _IME_H_ */