#include "md1.h"
#include "com_def.h"

s64 md_kernel_64(U7 w, U7 h, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], 
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	SKIP_MERGE_INPUT* skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status, 
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 64>* IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>* IP_buffer_ptr_64x8,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<8, 64>* MPM_buffer_ptr_8x64, MPM_ARRAY_SIZE<64, 8>* MPM_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	IP_MAP_SCU_SIZE<8, 64>* IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>* IP_map_scu_ptr_64x8,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);

