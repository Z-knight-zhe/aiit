#ifndef _MD_DUMMY_H_
#define _MD_DUMMY_H_
#include "md1.h"
#include "md_kernel_32.h"

#if LUMA_8_PIPELINE_DUMMY
void rdo_luma_8_pipeline(u8 all_rdo_num, U8 w, U8 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, U2 curr_cons_pred_mode, U4 bit_depth, u16 avail_tb,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	u8 rdo_list[MAX_RDO_NUM_SIZE(8, 8)], U1 valid_mode[17], u8 ipred_list[IPRED_LIST_NUM], pel src_le_temp[2 * 8 + 3], pel src_up_temp[2 * 8 + 3], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	pel orgY[8 * 8], 
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif
	/*s64 flag_cost[2], */s32 dist_value,
	s64 cost_best[1], 
	//pel fme2mdrefbuf[2560][8][2][16],
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[8][8], pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 8) - 1][8][8], pel pred_y_intra[MAX_RDO_NUM_SIZE(8, 8)][8][8]
);
#endif //LUMA_8_PIPELINE_DUMMY

#if LUMA_16x16_PIPELINE_DUMMY
void rdo_luma_16_16_pipeline(u8 all_rdo_num, U8 w, U8 h, pel orgY[16 * 16], pel orgY_nz[16][16], pel org_diff[16][16], U1 valid_mode[17], u8 ipred_list[5], pel src_le_temp[2 * 16 + 3], pel src_up_temp[2 * 16 + 3], u16 avail_tb, u8 rdo_list[5], u8 inter_rdo_num,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST_16* best_info_ptr_16, MD_INPUT* md_input_ptr,
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window_16* Fetch_Ref_window_ptr,
#endif
	SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], MERGE_RDO* merge_rdo, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], U3 cu_width_log2, U3 cu_height_log2,
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_temp_ptr_16,
	U2 curr_cons_pred_mode,
	//pel fme2mdrefbuf[2560][8][2][16],
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[16][16], pel pred_y_inter2[2][16][16], pel pred_y_intra[5][16][16],
	MD_KERNEL_INPUT* md_kernel_input, u8 mpm[MAX_NUM_PB][2], U7 cu_width, int refp_ptr0, int refp_ptr1, U2 ch_type, s64 lambda, s64 cost_skip, s64* cost_best, U6 qp
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);
#endif //LUMA_16x16_PIPELINE_DUMMY

#if LUMA_16x8_PIPELINE_DUMMY
void rdo_luma_16_8_pipeline(u8 all_rdo_num, U8 w, U8 h, pel orgY[16 * 16], pel orgY_nz[16][16], pel org_diff[16][16], U1 valid_mode[17], u8 ipred_list[5], pel src_le_temp[2 * 16 + 3], pel src_up_temp[2 * 16 + 3], u16 avail_tb, u8 rdo_list[MAX_RDO_NUM_SIZE(16, 8)], u8 inter_rdo_num,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST_16* best_info_ptr_16, MD_INPUT* md_input_ptr,
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window_16* Fetch_Ref_window_ptr,
#endif
	SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], MERGE_RDO* merge_rdo, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], U3 cu_width_log2, U3 cu_height_log2,
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_temp_ptr_16,
	U2 curr_cons_pred_mode,
	//pel fme2mdrefbuf[2560][8][2][16],
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[16][16], pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(16, 8) - 1][16][16], pel pred_y_intra[MAX_RDO_NUM_SIZE(16, 8)][16][16],
	MD_KERNEL_INPUT* md_kernel_input, u8 mpm[MAX_NUM_PB][2], U7 cu_width, int refp_ptr0, int refp_ptr1, U2 ch_type, s64 lambda, s64 cost_skip, s64* cost_best, U6 qp
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);
#endif //LUMA_16x8_PIPELINE_DUMMY

#if LUMA_8x16_PIPELINE_DUMMY
void rdo_luma_8_16_pipeline(u8 all_rdo_num, U8 w, U8 h, pel orgY[16 * 16], pel orgY_nz[16][16], pel org_diff[16][16], U1 valid_mode[17], u8 ipred_list[5], pel src_le_temp[2 * 16 + 3], pel src_up_temp[2 * 16 + 3], u16 avail_tb, u8 rdo_list[MAX_RDO_NUM_SIZE(8, 16)], u8 inter_rdo_num, 
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST_16* best_info_ptr_16, MD_INPUT* md_input_ptr,
	#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window_16* Fetch_Ref_window_ptr,
#endif
	SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], MERGE_RDO* merge_rdo, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], U3 cu_width_log2, U3 cu_height_log2, 
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_temp_ptr_16,
	U2 curr_cons_pred_mode,
	//pel fme2mdrefbuf[2560][8][2][16],
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[16][16], pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 16) - 1][16][16], pel pred_y_intra[MAX_RDO_NUM_SIZE(8, 16)][16][16],
	MD_KERNEL_INPUT* md_kernel_input, u8 mpm[MAX_NUM_PB][2], U7 cu_width, int refp_ptr0, int refp_ptr1, U2 ch_type, s64 lambda, s64 cost_skip, s64* cost_best, U6 qp
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);
#endif //LUMA_8x16_PIPELINE_DUMMY

#if LUMA_32_PIPELINE_DUMMY
void rdo_luma_32_pipeline(U8 w, U8 h, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs, SKIP_MERGE_INPUT* skip_in,
	U2 tree_status, MD_COM_MODE_BEST* best_info_ptr, MD_INPUT* md_input_ptr, pel orgY_32[32][32],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif
	MD_COM_MODE* mod_info_curr_ptr, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U2 curr_cons_pred_mode, U3 qt_depth,
	ENC_FME_INFO_ALL* fme_mv_info,
	u8 rmd_mode[IPD_RDO_CNT],
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32, IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	MD_KERNEL_INPUT_32* md_kernel_input_32,
	//pel fme2mdrefbuf[2560][8][2][16],
	pel pred_y_intr[5][32][32],
	strFetch_ref_window* Fetch_Ref_window_ptr,
#if SUB_TMVP
	CORE_SBTMVP* core_sbtmvp,
#endif
	//add new paras
	U1 valid_mode[17],
	u8 ipred_list[5],
	pel src_le_temp[2 * 32 + 3],
	pel src_up_temp[2 * 32 + 3],
	u16 avail_tb,
	u8 rdo_list[5],
	u8 inter_rdo_num,
	SKIP_RDO* skip_rdo,
	SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	MERGE_RDO* merge_rdo,
	S14 sub_pel_mv[MV_D],
	s16 mvp_all[2][5][2],
#if ENABLE_BFRAME
	MV_DIR* mv_dir,
#endif
	s64 *cost_best);
#endif //LUMA_32_PIPELINE_DUMMY
#if LUMA_64_PIPELINE_DUMMY
void rdo_luma_64_pipeline(u8 all_rdo_num, U8 w, U8 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, U2 curr_cons_pred_mode, U4 bit_depth, u16 avail_tb,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr, ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
	u8 rdo_list[MD_MAX_RDO_NUM], U1 valid_mode[17], u8 ipred_list[5], pel src_le_temp[2 * 64 + 3], pel src_up_temp[2 * 64 + 3], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	pel orgY[64 * 64], 
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif
	s64 flag_cost[2], s32 dist,
	s64* cost_best,
	pel fme2mdrefbuf[2560][8][2][16]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
	);
#endif //LUMA_64_PIPELINE_DUMMY

#if CIRCUIT32_DUMMY
s64 circuit_32x32_top(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in, 
	ENC_CU_DATA* prev_cu_data_ptr, 
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, 
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, 
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father);
#endif //CIRCUIT32_DUMMY

#if CIRCUIT64_DUMMY
void circuit_64x64_64x32_32x64_branch_top(U2 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], 
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in, MD_RESULT* md2_best_result_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, 
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, 
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, 
	ENC_CU_DATA* md2_best_ctu_output_ptr,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);
#endif //CIRCUIT64_DUMMY

#if LOAD_CORE_DUMMY
bool load_core(InstructionRegister<Instruction>& ir1, InstructionRegister<Instruction>& ir2, Instruction mem_load[PR_SIZE],
	Instruction mem_pu[PR_SIZE], PRMan& pu_man, hls::stream<U32>& status, Load2PU pu_in_buf[PU_NUM], Channel& input, ShareMemory& pool,
	LCUConfig& config, LCUInput& buffer, stream_type<ap_uint<1> > push_data[PU_NUM]);
#endif //LOAD_CORE_DUMMY

void copy_cu_data_array_src_cur_size_16ul_s(ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2);


void copy_cu_data_array_src_cur_size_32ul_s(ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2);

void copy_cu_data_array_src_cur_size_64ul_s(ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2);

void copy_cu_data_array_src_cur_size_16ul_l(ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2);


void copy_cu_data_array_src_cur_size_32ul_l(ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2);

void copy_cu_data_array_src_cur_size_64ul_l(ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2);

s64 store_enc_info_best_16_top_array_size_16(
	MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_SIZE<16>*	curr_cu_data_best,   ENC_CU_DATA_ARRAY_SIZE<32>*  prev_cu_data_ptr,
	U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw
	);

#if CHROMA_4_PIPELINE_DUMMY
void rdo_chroma_4_pipeline_chroma(int all_rdo_mode, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, 
	U2 tree_status, 
	MD_COM_MODE_BEST* mod_info_best,
	COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, pel orgU[2][4 * 4],
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
	strFetch_ref_window* Fetch_Ref_window_ptr, pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8,
	pel mod_info_best_rec_u[4][4],
	pel mod_info_best_rec_v[4][4],
	pel mod_info_best_coef_u[4][4],
	pel mod_info_best_coef_v[4][4]);
#endif //CHROMA_4_PIPELINE_DUMMY

#if CHROMA_8_PIPELINE_DUMMY
void rdo_chroma_8_pipeline(int all_rdo_mode, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best,
	COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, pel orgU[2][8][8],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U3 cu_width_log2, U3 cu_height_log2, U7 cu_width, U7 cu_height, U13 pb_x, U13 pb_y, S12 size_tmp, s64 lambda_y,
	U4 bit_depth, s16 coef_uv_pb_part0[2][8][8], pel rec_tmp0[2][8][8], pel pred[2][8][8], strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[16][16]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
	, pel mod_info_best_rec_u[8][8],
	pel mod_info_best_rec_v[8][8],
	pel mod_info_best_coef_u[8][8],
	pel mod_info_best_coef_v[8][8],
	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8], pel IP_buffer_ptr_U_8x8_left_line[2 * 8], pel IP_buffer_ptr_U_8x8_top_left,
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8], pel IP_buffer_ptr_V_8x8_left_line[2 * 8], pel IP_buffer_ptr_V_8x8_top_left,
	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4], pel IP_buffer_ptr_U_4x8_left_line[2 * 8], pel IP_buffer_ptr_U_4x8_top_left,
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4], pel IP_buffer_ptr_V_4x8_left_line[2 * 8], pel IP_buffer_ptr_V_4x8_top_left,
	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8], pel IP_buffer_ptr_U_8x4_left_line[2 * 4], pel IP_buffer_ptr_U_8x4_top_left,
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8], pel IP_buffer_ptr_V_8x4_left_line[2 * 4], pel IP_buffer_ptr_V_8x4_top_left,
	pel	IP_buffer_ptr_8x16_up_line[2 * 8], pel IP_buffer_ptr_8x16_left_line[2 * 16], pel IP_buffer_ptr_8x16_top_left,
	pel	IP_buffer_ptr_16x8_up_line[2 * 16], pel IP_buffer_ptr_16x8_left_line[2 * 8], pel IP_buffer_ptr_16x8_top_left,
	pel	IP_buffer_ptr_16x16_up_line[2 * 16], pel IP_buffer_ptr_16x16_left_line[2 * 16], pel IP_buffer_ptr_16x16_top_left,
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new);
#endif //CHROMA_8_PIPELINE_DUMMY

#if CHROMA_16_PIPELINE_DUMMY
void rdo_chroma_16_pipeline(u8 all_rdo_num, MD_COM_MODE_BEST * mod_info_best, RDOQ_ARRAY * rdoq_array, MD_FW * md_fw_ptr,
	U2 tree_status,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	MD_INPUT * md_input_ptr, pel orgU[2][16][16], 
	S8 ipm, U7 cu_width, U7 cu_height, U3 cu_width_log2, U3 cu_height_log2, U6 qp, s64 lambda, U1 is_intra_quant, int mod_info_nz_tb_part, s64 lambda_y, U4 bit_depth, strFetch_ref_window * Fetch_Ref_window_ptr,
	pel piRecoY[32][32], 
#if SUB_TMVP
	CORE_SBTMVP * core_sbtmvp,
#endif
	pel mod_info_best_rec_u[16][16], pel mod_info_best_rec_v[16][16], pel mod_info_best_coef_u[16][16], pel mod_info_best_coef_v[16][16],
	pel	IP_buffer_ptr_U_16x16_up_line[2 * 16], pel IP_buffer_ptr_U_16x16_left_line[2 * 16], pel IP_buffer_ptr_U_16x16_top_left,
	pel	IP_buffer_ptr_V_16x16_up_line[2 * 16], pel IP_buffer_ptr_V_16x16_left_line[2 * 16], pel IP_buffer_ptr_V_16x16_top_left,
	pel	IP_buffer_ptr_U_8x16_up_line[2 * 8], pel IP_buffer_ptr_U_8x16_left_line[2 * 16], pel IP_buffer_ptr_U_8x16_top_left,
	pel	IP_buffer_ptr_V_8x16_up_line[2 * 8], pel IP_buffer_ptr_V_8x16_left_line[2 * 16], pel IP_buffer_ptr_V_8x16_top_left,
	pel	IP_buffer_ptr_U_16x8_up_line[2 * 16], pel IP_buffer_ptr_U_16x8_left_line[2 * 8], pel IP_buffer_ptr_U_16x8_top_left,
	pel	IP_buffer_ptr_V_16x8_up_line[2 * 16], pel IP_buffer_ptr_V_16x8_left_line[2 * 8], pel IP_buffer_ptr_V_16x8_top_left,
	pel	IP_buffer_ptr_16x32_up_line[2 * 16], pel IP_buffer_ptr_16x32_left_line[2 * 32], pel IP_buffer_ptr_16x32_top_left,
	pel	IP_buffer_ptr_32x16_up_line[2 * 32], pel IP_buffer_ptr_32x16_left_line[2 * 16], pel IP_buffer_ptr_32x16_top_left,
	pel	IP_buffer_ptr_32x32_up_line[2 * 32], pel IP_buffer_ptr_32x32_left_line[2 * 32], pel IP_buffer_ptr_32x32_top_left,
	U32 IP_map_scu_ptr_16x32_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x32_map_scu_left_line_new[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_16x32_map_scu_top_left_new,
	U32 IP_map_scu_ptr_32x16_map_scu_up_line[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_32x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_32x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_32x32_map_scu_up_line[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_32x32_map_scu_left_line_new[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_32x32_map_scu_top_left_new);
#endif //CHROMA_16_PIPELINE_DUMMY

#if CHROMA_32_PIPELINE_DUMMY
void rdo_chroma_pipeline_32(int all_rdo_num, RDOQ_ARRAY* rdoq_array, MD_INPUT* md_input_ptr, U13 pb_x, U13 pb_y,
	MD_FW* md_fw_ptr, pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U2 tree_status,
	MD_COM_MODE_BEST* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info,
	pel orgU[2][32][32],
	int cu_width_log2_chroma, int cu_width_chroma, int cu_height_log2_chroma, int cu_height_chroma, int bit_depth,
	U2 cu_mode, U2 slice_type, U14 scale[2], U6 qpUV[2], int tscpm_enable_flag, s64 lambda_y,
	S12 size_tmp, strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
	, pel mod_info_best_rec_u[32][32],
	pel mod_info_best_rec_v[32][32],
	pel mod_info_best_coef_u[32][32],
	pel mod_info_best_coef_v[32][32],
	pel IP_buffer_ptr_U_32x32_up_line[2 * 32], pel IP_buffer_ptr_U_32x32_left_line[2 * 32], pel IP_buffer_ptr_U_32x32_top_left,
	pel IP_buffer_ptr_V_32x32_up_line[2 * 32], pel IP_buffer_ptr_V_32x32_left_line[2 * 32], pel IP_buffer_ptr_V_32x32_top_left,
	pel IP_buffer_ptr_U_16x32_up_line[2 * 16], pel IP_buffer_ptr_U_16x32_left_line[2 * 32], pel IP_buffer_ptr_U_16x32_top_left,
	pel IP_buffer_ptr_V_16x32_up_line[2 * 16], pel IP_buffer_ptr_V_16x32_left_line[2 * 32], pel IP_buffer_ptr_V_16x32_top_left,
	pel IP_buffer_ptr_U_32x16_up_line[2 * 32], pel IP_buffer_ptr_U_32x16_left_line[2 * 16], pel IP_buffer_ptr_U_32x16_top_left,
	pel IP_buffer_ptr_V_32x16_up_line[2 * 32], pel IP_buffer_ptr_V_32x16_left_line[2 * 16], pel IP_buffer_ptr_V_32x16_top_left,
	pel IP_buffer_ptr_U_8x32_up_line[2 * 8], pel IP_buffer_ptr_U_8x32_left_line[2 * 32], pel IP_buffer_ptr_U_8x32_top_left,
	pel IP_buffer_ptr_V_8x32_up_line[2 * 8], pel IP_buffer_ptr_V_8x32_left_line[2 * 32], pel IP_buffer_ptr_V_8x32_top_left,
	pel IP_buffer_ptr_U_32x8_up_line[2 * 32], pel IP_buffer_ptr_U_32x8_left_line[2 * 8], pel IP_buffer_ptr_U_32x8_top_left,
	pel IP_buffer_ptr_V_32x8_up_line[2 * 32], pel IP_buffer_ptr_V_32x8_left_line[2 * 8], pel IP_buffer_ptr_V_32x8_top_left,
	pel IP_buffer_ptr_U_4x32_up_line[2 * 4], pel IP_buffer_ptr_U_4x32_left_line[2 * 32], pel IP_buffer_ptr_U_4x32_top_left,
	pel IP_buffer_ptr_V_4x32_up_line[2 * 4], pel IP_buffer_ptr_V_4x32_left_line[2 * 32], pel IP_buffer_ptr_V_4x32_top_left,
	pel IP_buffer_ptr_U_32x4_up_line[2 * 32], pel IP_buffer_ptr_U_32x4_left_line[2 * 4], pel IP_buffer_ptr_U_32x4_top_left,
	pel IP_buffer_ptr_V_32x4_up_line[2 * 32], pel IP_buffer_ptr_V_32x4_left_line[2 * 4], pel IP_buffer_ptr_V_32x4_top_left,
	pel IP_buffer_ptr_64x64_up_line[128], pel IP_buffer_ptr_64x64_left_line[64], pel IP_buffer_ptr_64x64_top_left,
	pel IP_buffer_ptr_32x64_up_line[2 * 32], pel IP_buffer_ptr_32x64_left_line[2 * 64], pel IP_buffer_ptr_32x64_top_left,
	pel IP_buffer_ptr_64x32_up_line[2 * 64], pel IP_buffer_ptr_64x32_left_line[2 * 32], pel IP_buffer_ptr_64x32_top_left,
	pel IP_buffer_ptr_16x64_up_line[2 * 16], pel IP_buffer_ptr_16x64_left_line[2 * 64], pel IP_buffer_ptr_16x64_top_left,
	pel IP_buffer_ptr_64x16_up_line[2 * 64], pel IP_buffer_ptr_64x16_left_line[2 * 16], pel IP_buffer_ptr_64x16_top_left,
	pel IP_buffer_ptr_8x64_up_line[2 * 8], pel IP_buffer_ptr_8x64_left_line[2 * 64], pel IP_buffer_ptr_8x64_top_left,
	pel IP_buffer_ptr_64x8_up_line[2 * 64], pel IP_buffer_ptr_64x8_left_line[2 * 8], pel IP_buffer_ptr_64x8_top_left,
	U32 IP_map_scu_ptr_64x64_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_32x64_map_scu_up_line[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_32x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_32x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_64x32_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x32_map_scu_left_line_new[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_64x32_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x64_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_16x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_64x16_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_64x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_8x64_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_8x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_64x8_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_64x8_map_scu_top_left_new);
#endif //CHROMA_32_PIPELINE_DUMMY

#if CHROMA_32_PIPELINE_DUMMY
void rdo_chroma_pipeline_32_pred(int all_rdo_num, U13 pb_x, U13 pb_y,
	int cu_width_log2_chroma, int cu_width_chroma, int cu_height_log2_chroma, int cu_height_chroma, int bit_depth, MD_COM_MODE_BEST* mod_info_best, MD_FW* md_fw_ptr,

	pel pred[12][32][32], pel pred0[12][32][32], strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
	,
	pel IP_buffer_ptr_U_32x32_up_line[2 * 32], pel IP_buffer_ptr_U_32x32_left_line[2 * 32], pel IP_buffer_ptr_U_32x32_top_left,
	pel IP_buffer_ptr_V_32x32_up_line[2 * 32], pel IP_buffer_ptr_V_32x32_left_line[2 * 32], pel IP_buffer_ptr_V_32x32_top_left,
	pel IP_buffer_ptr_U_16x32_up_line[2 * 16], pel IP_buffer_ptr_U_16x32_left_line[2 * 32], pel IP_buffer_ptr_U_16x32_top_left,
	pel IP_buffer_ptr_V_16x32_up_line[2 * 16], pel IP_buffer_ptr_V_16x32_left_line[2 * 32], pel IP_buffer_ptr_V_16x32_top_left,
	pel IP_buffer_ptr_U_32x16_up_line[2 * 32], pel IP_buffer_ptr_U_32x16_left_line[2 * 16], pel IP_buffer_ptr_U_32x16_top_left,
	pel IP_buffer_ptr_V_32x16_up_line[2 * 32], pel IP_buffer_ptr_V_32x16_left_line[2 * 16], pel IP_buffer_ptr_V_32x16_top_left,
	pel IP_buffer_ptr_U_8x32_up_line[2 * 8], pel IP_buffer_ptr_U_8x32_left_line[2 * 32], pel IP_buffer_ptr_U_8x32_top_left,
	pel IP_buffer_ptr_V_8x32_up_line[2 * 8], pel IP_buffer_ptr_V_8x32_left_line[2 * 32], pel IP_buffer_ptr_V_8x32_top_left,
	pel IP_buffer_ptr_U_32x8_up_line[2 * 32], pel IP_buffer_ptr_U_32x8_left_line[2 * 8], pel IP_buffer_ptr_U_32x8_top_left,
	pel IP_buffer_ptr_V_32x8_up_line[2 * 32], pel IP_buffer_ptr_V_32x8_left_line[2 * 8], pel IP_buffer_ptr_V_32x8_top_left,
	pel IP_buffer_ptr_U_4x32_up_line[2 * 4], pel IP_buffer_ptr_U_4x32_left_line[2 * 32], pel IP_buffer_ptr_U_4x32_top_left,
	pel IP_buffer_ptr_V_4x32_up_line[2 * 4], pel IP_buffer_ptr_V_4x32_left_line[2 * 32], pel IP_buffer_ptr_V_4x32_top_left,
	pel IP_buffer_ptr_U_32x4_up_line[2 * 32], pel IP_buffer_ptr_U_32x4_left_line[2 * 4], pel IP_buffer_ptr_U_32x4_top_left,
	pel IP_buffer_ptr_V_32x4_up_line[2 * 32], pel IP_buffer_ptr_V_32x4_left_line[2 * 4], pel IP_buffer_ptr_V_32x4_top_left,
	pel IP_buffer_ptr_64x64_up_line[128], pel IP_buffer_ptr_64x64_left_line[64], pel IP_buffer_ptr_64x64_top_left,
	pel IP_buffer_ptr_32x64_up_line[2 * 32], pel IP_buffer_ptr_32x64_left_line[2 * 64], pel IP_buffer_ptr_32x64_top_left,
	pel IP_buffer_ptr_64x32_up_line[2 * 64], pel IP_buffer_ptr_64x32_left_line[2 * 32], pel IP_buffer_ptr_64x32_top_left,
	pel IP_buffer_ptr_16x64_up_line[2 * 16], pel IP_buffer_ptr_16x64_left_line[2 * 64], pel IP_buffer_ptr_16x64_top_left,
	pel IP_buffer_ptr_64x16_up_line[2 * 64], pel IP_buffer_ptr_64x16_left_line[2 * 16], pel IP_buffer_ptr_64x16_top_left,
	pel IP_buffer_ptr_8x64_up_line[2 * 8], pel IP_buffer_ptr_8x64_left_line[2 * 64], pel IP_buffer_ptr_8x64_top_left,
	pel IP_buffer_ptr_64x8_up_line[2 * 64], pel IP_buffer_ptr_64x8_left_line[2 * 8], pel IP_buffer_ptr_64x8_top_left,
	U32 IP_map_scu_ptr_64x64_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_32x64_map_scu_up_line[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_32x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_32x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_64x32_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x32_map_scu_left_line_new[(32 >> 2) << 1],
	U32 IP_map_scu_ptr_64x32_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x64_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_16x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_64x16_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_64x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_8x64_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x64_map_scu_left_line_new[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_8x64_map_scu_top_left_new,
	U32 IP_map_scu_ptr_64x8_map_scu_up_line[(64 >> 2) << 1],
	U32 IP_map_scu_ptr_64x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_64x8_map_scu_top_left_new
);
#endif

#if CIRCUIT32_CHROMA_DUMMY
s64 circuit_32x32_top_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num,
	COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father);
#endif

#if CIRCUIT64_CHROMA_DUMMY
void circuit_64x64_64x32_32x64_branch_top_chroma(U2 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	MD_RESULT* md2_best_result_ptr,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	ENC_CU_DATA* md2_best_ctu_output_ptr,
	strFetch_ref_window* Fetch_Ref_window_ptr, ENC_CU_DATA* md_output_luma);
#endif

#endif //_MD_DUMMY_H_

