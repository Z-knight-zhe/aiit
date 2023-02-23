#include "com_def.h"
#include "enc_def.h"
#include "type_def.h"
//#define COLLECT_DBK_DATA

//#define DBK_TEST_VECTOR 0
//#if DBK_TEST_VECTOR
//#include <cstdarg>
//#include <sstream>
//#endif
//#include<map>
//#include<vector>
//#include <fstream>
//#include <iostream>

#ifndef __MERLIN_TYPE_DEFINE_H__

#define __MERLIN_TYPE_DEFINE_H__
typedef signed short s16;
typedef s16 pel;
typedef unsigned int u32;
//typedef signed char s8;
typedef unsigned char u8;
//typedef struct _ENC_ME_NEB_INFO {int scu;s16 mv[2][2];s8 refi[2];u8 is_valid;u32 pb_tb_part;}ENC_ME_NEB_INFO;
//typedef enum _BLOCK_SHAPE {NON_SQUARE_18=0,NON_SQUARE_14=1,NON_SQUARE_12=2,SQUARE=3,NON_SQUARE_21=4,NON_SQUARE_41=5,NON_SQUARE_81=6,NUM_BLOCK_SHAPE=7}BLOCK_SHAPE;
//typedef enum _SPLIT_MODE {NO_SPLIT=0,SPLIT_BI_VER=1,SPLIT_BI_HOR=2,SPLIT_EQT_VER=3,SPLIT_EQT_HOR=4,SPLIT_QUAD=5,NUM_SPLIT_MODE=6}SPLIT_MODE;
//typedef enum _SPLIT_DIR {SPLIT_VER=0,SPLIT_HOR=1,SPLIT_QT=2}SPLIT_DIR;
//typedef struct _COM_SPLIT_STRUCT {int part_count;int cud;int width[4];int height[4];int log_cuw[4];int log_cuh[4];int x_pos[4];int y_pos[4];int cup[4];}COM_SPLIT_STRUCT;
typedef struct FILTER_SPLIT_MODE {int x;int y;int cuw;int cuh;int cud;int cup;U3 split_mode;int lcu_idx;int next;}FILTER_SPLIT_MODE;
typedef int BOOL;
typedef s16 dbk_ushort;
//typedef struct _ENC_ME_LINE_MAP {int min_scu;int min_scu_x;int min_scu_y;int stride_in_map;int stride_in_line;int pic_width_in_scu;int pic_height_in_scu;int above_line_idx;int curr_ctu_idx;int x_ctu;int y_ctu;s8 line_map_split[5][7][256];ENC_ME_NEB_INFO inner_nebs[2][16][16];u32 map_scu_inner[2][16][16];u32 map_cu_mode_inner[2][16][16];ENC_ME_NEB_INFO above_nebs_line[2][960];u32 map_scu_above_line[2][960];u32 map_cu_mode_above_line[2][960];s16 co_left_mv[16][2];s8 co_left_refi[16];s8 refi_nebs[5];u32 scu_nebs[5];s16 mv_nebs[5][2];int if_init;}ENC_ME_LINE_MAP;
#endif

// struct pack_array {
// 	u8 h[8];
// 	u8 v[8];
// };

// struct pack_array_uv {
// 	u8 h[6];
// 	u8 v[6];
// };

typedef enum _N_ZIGZAG_CHOICE_dbk
{
	N_LEFT_TOWARDS_dbk = 0,
	N_RIGHT_UP_dbk = 1,
	N_GO_UP_dbk = 2,
	N_LEFT_DOWN_dbk = 3,
} N_ZIGZAG_CHOICE_dbk;

typedef struct _DBK_OUT_PARA_SET
{
	U13 pic_width;
	U13 pic_height;
	//U6 pic_width_in_lcu;
	//U6 pic_height_in_lcu;
	//s8 sample_adaptive_offset_enable_flag;
	U13 x_pel;
	U13 y_pel;
	U10 pic_width_in_scu;
	/*pel dbkout_y;
	pel dbkout_u;
	pel dbkout_v;*/
	U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U11 min_scu_x;
	U11 min_scu_y;
	//U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U1 curr_ctu_idx;
}DBK_OUT_PARA_SET;
typedef struct _SRCC_input
{
	s8  cu_data_split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
//	//int cu_data_pb_part[CU_SIZE];
//	//int cu_data_tb_part[CU_SIZE];
//	u8  cu_data_pred_mode[CU_SIZE];
//	//u8  cu_data_ipf_flag[CU_SIZE];
//	u8  cu_data_mpm[2][CU_SIZE];
//	s8  cu_data_ipm[2][CU_SIZE];
//	int cu_data_num_nz_coef[N_C][CU_SIZE];
//
//	U8  cu_data_affine_flag[CU_SIZE];
//	U8  cu_data_umve_flag[CU_SIZE];
//	U8  cu_data_umve_idx[CU_SIZE];
//	U8  cu_data_skip_idx[CU_SIZE];
//	U8  cu_data_mvr_idx[CU_SIZE];
//#if EXT_AMVR_HMVP
//	// u8  *mvp_from_hmvp_flag;
//	U8  cu_data_mvp_from_hmvp_flag[CU_SIZE];
//#endif
//	S3  cu_data_refi[CU_SIZE][REFP_NUM];
//
//	s16      coef[N_C][MAX_CU_DIM];//input

	int x_pel;//self
	int y_pel;//self
	int x_lcu_idx;//self
	int y_lcu_idx;//self
	//int lcu_num;//input
	int lcu_idx;//self
	/*u8 log2_culine;//input
	*/
	//int modeIdc[N_C];//input
	//int typeIdc[N_C];//input
	//int startBand[N_C];//input
	//int startBand2[N_C];//input
	//int deltaband[N_C];//input
	//u8 slice_sao_enable[N_C];//input
	//int offset[N_C][MAX_NUM_SAO_CLASSES];//input

	/*int MergeLeftAvail;
	int MergeUpAvail;*/

	int fetch_output_me_line_map_ptr_min_scu_x;
	int fetch_output_me_line_map_ptr_min_scu_y;


	/*U3  rpm_num_refp[REFP_NUM];
	s16 mvd[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];*/
} SRCC_input;
typedef struct _SRCC_FW_input
{
//#if DT_PARTITION
//	u8   dt_intra_enable_flag;
//#endif
//	u8   position_based_transform_enable_flag;
//#if TSCPM
//	u8   tscpm_enable_flag;
//#endif	
//	int  ipf_enable_flag;
//	u8   sample_adaptive_offset_enable_flag;
	int  pic_width_in_scu;
	int  max_cuwh;
	int  pic_width;
	int  pic_height;
	u8   slice_type;
	u8   log2_culine;
	int lcu_cnt;

	/*U4   sqh_num_of_hmvp_cand;
	U1  sqh_umve_enable_flag;
	U1   sqh_affine_enable_flag;
	U1   sqh_amvr_enable_flag;
#if EXT_AMVR_HMVP
	U1  sqh_emvr_enable_flag;
#endif	
	U3 rpm_num_refp[REFP_NUM];

	int qp_y;*/
}SRCC_FW_input;


void dbk_run(
	U13 fetch2dbk_fw_ptr_pic_width, U12 fetch2dbk_fw_ptr_pic_height, U7 fetch2dbk_fw_ptr_pic_width_in_lcu, U6 fetch2dbk_fw_ptr_pic_height_in_lcu,
	S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39],
	U12 fetch_output_dbk_pix_x, U12 fetch_output_dbk_pix_y,
	U11 me_line_map_ptr_DBK_min_scu_x, U11 me_line_map_ptr_DBK_min_scu_y,
	//U6 me_line_map_ptr_line_map_split[5][7][256],
	U32 me_line_map_ptr_DBK_map_scu_inner[2][16][16],
	ENC_ME_NEB_INFO new_above_nebs_line_DBK[48], U32 me_line_map_ptr_DBK_new_map_scu_above_line[48],
	ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16], ENC_ME_NEB_INFO inner_nebs_sao[2][16][16],
	DBK_OUT_PARA_SET* dbk_out_para_set, U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U4 fetch2dbk_fw_ptr_bit_depth, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth, 
 U2 use_deblock
#if ENABLE_BFRAME
	//, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM]
  , int fetch2dbk_fw_ptr_ref_ptr_dbk[MAX_NUM_REF_PICS][REFP_NUM]
#endif
	, ENC_CU_DATA* ctu_map_cu_data_DBK, AEC_FW* fetch2dbk_firmware, int fetch_output_dbk_lcu_num, SRCC_FOR_AEC* srcc_for_aec
);


void dbk_run_facade(
	S32 fetch2dbk_fw_ptr_pic_width, S32 fetch2dbk_fw_ptr_pic_height, S32 fetch2dbk_fw_ptr_pic_width_in_lcu, S32 fetch2dbk_fw_ptr_pic_height_in_lcu,
	S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39],
	U12 fetch_output_dbk_pix_x, U12 fetch_output_dbk_pix_y,
	U11 me_line_map_ptr_DBK_min_scu_x, U11 me_line_map_ptr_DBK_min_scu_y,
	//U6 me_line_map_ptr_line_map_split[5][7][256],
	U32 me_line_map_ptr_DBK_map_scu_inner[2][16][16],
	ENC_ME_NEB_INFO new_above_nebs_line_DBK[48], U32 me_line_map_ptr_DBK_new_map_scu_above_line[48],
	ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16], ENC_ME_NEB_INFO inner_nebs_sao[2][16][16],
	DBK_OUT_PARA_SET* dbk_out_para_set, U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U4 fetch2dbk_fw_ptr_bit_depth, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth
#if ENABLE_BFRAME
	, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM]
#endif
);