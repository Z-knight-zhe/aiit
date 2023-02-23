#include "md1.h"

#define SIZE_16 (16 * 16)

s64 md_kernel_16(
	MD_FW* md_fw_ptr,
	MD_INPUT* md_input_ptr, 
	u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[16][16],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16* skip_in_16,
	U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0, U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_temp_ptr_16,
	ENC_FME_INFO_ALL_16* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, 
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16, MPM_ARRAY_SIZE<8, 16>* MPM_buffer_ptr_8x16, MPM_ARRAY_SIZE<16, 8>* MPM_buffer_ptr_16x8, 
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16, IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8,
	Arbitor& ref,
	strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, SKIP_MERGE_INPUT* skip_in
#endif
);
