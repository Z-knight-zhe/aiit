#ifndef _MD_KERNEL_32_H_
#define _MD_KERNEL_32_H_
#include "md1.h"

#define _DATA_CP_FOR_DATAFLOW_ 1
#define SIZE_32 (32 * 32)

//s64 md_kernel_32(U8 width, U8 height, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, U6 rmd_mode[RMD_CU_SIZE_NUM][IPD_RDO_CNT], 
//	U8 RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH],
//	ENC_ME_LINE_MAP* me_line_map_ptr,
//	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
//	SKIP_MERGE_INPUT* skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
//	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
//	U2 tree_status, ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
//	ENC_FME_INFO_ALL* fme_mv_info,
//	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr);


struct MD_KERNEL_INPUT_32
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

#ifndef _DATA_CP_FOR_DATAFLOW_
struct MD_KERNEL_MODE_32
{
	U2  cu_mode;
	U8   affine_flag;
	s16  mvd[REFP_NUM][MV_D];
	S14  mv[REFP_NUM][MV_D];
	U8   skip_idx;
	U8   smvd_flag;
	CPMV affine_mv[REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd[REFP_NUM][VER_NUM][MV_D];
	S3   refi[REFP_NUM];
	U8   umve_flag;
	U8   umve_idx;
	S8   ipm[MAX_NUM_PB][2];
	u8   mpm[MAX_NUM_PB][2];
	int  tb_part;
	int  pb_part;
};
#else
struct MD_KERNEL_MODE_32
{
	U2   cu_mode;
	U8   affine_flag;
	s16  mvd   [REFP_NUM][MV_D];
	s16  mvd_cp[REFP_NUM][MV_D];					// mem cp for DATAFLOW
	s16  mv    [REFP_NUM][MV_D];
	U8   skip_idx;
	U8   smvd_flag;
	CPMV affine_mv    [REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd   [REFP_NUM][VER_NUM][MV_D];
	s16  affine_mvd_cp[REFP_NUM][VER_NUM][MV_D];	// mem cp for DATAFLOW
	S3   refi   [REFP_NUM];
	S3   refi_cp[REFP_NUM];							// mem cp for DATAFLOW
	U8   umve_flag;
	U8   umve_idx;
	S8   ipm   [MAX_NUM_PB][2];
	S8   ipm_cp[MAX_NUM_PB][2];						// mem cp for DATAFLOW
	u8   mpm[MAX_NUM_PB][2];
	u8   mpm_cp[MAX_NUM_PB][2];						// mem cp for DATAFLOW
	int  tb_part;
	int  pb_part;
};
#endif //_DATA_CP_FOR_DATAFLOW_

void diff_ssd_bitcnt_32(RDOQ_MODEL* model_cnt, MD_INPUT* md_input_ptr,
	u8 mode_type, MD_KERNEL_MODE_32* mod_info_nz_32, pel pred_cache[32][32],
	pel org_y[32 * 32], s16 resi_t[32][32], s64* dist_pred, s64* cost_best,
	U2 tree_status, u8 cons_pred_mode, int mode_idx, U1 valid_mode[17], MD_KERNEL_INPUT_32* md_kernel_input, U4 bit_depth);

//only 32x32 32x16_16x32
s64 md_kernel_32_lite(U8 width, U8 height,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH ][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	SKIP_MERGE_INPUT* skip_in,
	U1 boundary, U1 isNS_allow, U2* NS_luma_pred_mode, U13 x0, U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	ENC_CU_DATA_ARRAY_SIZE<32>* cu_data_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	MPM_ARRAY_SIZE<32, 32>* MPM_buffer_ptr_32x32, MPM_ARRAY_SIZE<16, 32>* MPM_buffer_ptr_16x32, MPM_ARRAY_SIZE<32, 16>* MPM_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32, IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);

#if ENABLE_MD32C_BB
S64 md_chroma_32_wrapper(
	U8 w,U8 h,RDOQ_MODEL* rdoq_model,RDOQ_MODEL* model_cnt,MD_FW* md_fw_ptr,ENC_ME_LINE_MAP* me_line_map_ptr,ENC_ME_NEB_INFO inner_nebs[2][16][16],
	ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr,U16 x,U16 y,U8 cu_width_log2,
	U8 cu_height_log2,U8 tree_status,U8 cud,MD_INPUT* md_input_ptr,pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	U8 RefWindowLCU_UP[25600],U8 RefWindowLCU_VP[25600],S14 CtrPos[2][2][2],U8 need_dochroma,U16 cup,U8 ctx_cons_pred_mode,
	IP_ARRAY* IP_buffer_ptr,MPM_ARRAY* MPM_buffer_ptr,IP_MAP_SCU* MAP_SCU_IP_ptr,S14 best_affine_mv[2][4][2]);
#endif

#endif // _MD_KERNEL_32_H_