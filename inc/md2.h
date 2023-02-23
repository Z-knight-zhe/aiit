#include "com_def.h"
#include "enc_def.h"
#include "iime.h"
#include "md_cmod.h"
#ifndef __MERLIN_MD2_H__

#define __MERLIN_MD2_H__

typedef struct _SKIP_MERGE_INPUT SKIP_MERGE_INPUT;
typedef s16 pel;
typedef int CPMV;
typedef u32 SBAC_CTX_MODEL;
typedef long long COM_MTIME;
typedef struct _COM_IMGB COM_IMGB;
typedef struct _PICBUF_ALLOCATOR PICBUF_ALLOCATOR;
typedef struct _ENC_PINTER ENC_PINTER;
typedef struct SAOstatdata SAOStatData;
typedef int BOOL;
typedef struct _SKIP_RDO SKIP_RDO;
typedef struct _MERGE_RDO MERGE_RDO;


typedef struct _MD2_OUTPUT {
	s64 cost_best[1];
	s8 best_split_mode[1];
	u8 best_cons_pred_mode[1];
	COM_MOTION motion_cands_curr[8];
	COM_MOTION motion_cands_last[8];
	s8 cnt_hmvp_cands_curr[1];
	s8 cnt_hmvp_cands_last[1];

	ENC_CU_DATA cu_data_best_top;/////////////////////////////
	s8* ctx_map_map_ipm;
	s8(*ctx_map_map_refi)[REFP_NUM];
	s16(*ctx_map_mv)[REFP_NUM][MV_D];
	s8(*ctx_map_map_split)[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU];
	u32* ctx_map_map_scu;
	u32* core_cu_data_best_top_map_scu;
	u32* ctx_map_map_pb_tb_part;
	s8* core_cu_data_best_top_ipm;
	u32* core_cu_data_best_top_map_pb_tb_part;
	s16 core_cu_data_best_top_mv[MAX_CU_CNT_IN_LCU][REFP_NUM][MV_D];
	s8** core_cu_data_best_top_refi;
	s8* ctx_map_depth;
	s8 core_cu_data_best_top_depth;
	u32* ctx_map_cu_mode;
	u32* core_cu_data_best_top_map_cu_mode;
	pel ctx_best_rec_y[64][64];
	pel ctx_best_rec_u[64][64];
	pel ctx_best_rec_v[64][64];
}MD2_OUTPUT;
typedef struct _MD_OUT_PARA_SET
{
	/////Frame-level paramters
	U13 pic_width;
	U13 pic_height;

	U13 pix_x;
	U13 pix_y;

}MD_OUT_PARA_SET;

#if !ONLY_QT_QT
s64 md_32x32_32x16_16x32_loop_md2(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	U1 isBTH_allow, U13 x0, U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);
#endif

s64 md_64x8_8x64_loop(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);

#if ONLY_QT_QT
// void md_run(Instruction* mem_load, Instruction* mem_pu, Instruction* mem_store,
// 	pel fme2mdrefbuf[2560][8][2][16],			// FETCH
// #if ENABLE_BFRAME
// 	ME_MV_DIR& me_mv_dir_fme2md,				// FME, no scalar.
// #endif
// 	ENC_ME_NEB_INFO cur_ctu_mi[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], // 2CHROMA
// 	SKIP_MERGE_INPUT& skip_input_ptr,			// FETCH
// 	RMD_OUTPUT& rmd_output,						// RMD, No scalar.
// 	ENC_FME_INFO_ALL& fme_mv_info,				// FME, No scalar.
// 	pel dbk_y[71][71],							// 2DBK
// 	FME_OUT_PARA_SET& fme_out_para_set, FME_OUT_PARA_SET_AXIS& fme_out_para_set_axis, FME_OUT_PARA_SET& fme_out_para_set_md2chroma,	// FME, 2CHROMA
// 	ENC_CU_DATA& md_output_luma,				// 2CHROMA, no scalar.
// 	ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_LINE_MAP_AXIS* me_line_map_ptr_axis, ENC_ME_LINE_MAP* me_line_map_ptr_luma,			// FETCH, 2CHROMA
// 	strFetch_ref_window& Fetch_Ref_Window, strFetch_ref_window& Fetch_Ref_Window_md2chroma);										// FETCH, 2CHROMA
void md_run(
	// FIXME ZP: 20221022 cosim SEGV
	Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], Instruction mem_store[PR_SIZE], Register& reg,
	//Instruction* mem_load, Instruction* mem_pu, Instruction* mem_store,
	LCUInput & buffer, Arbitor & ref, int axis_Fetch_Ref_window_ptr_code_stru,
	//LCUInput buffer, Arbitor ref, int axis_Fetch_Ref_window_ptr_code_stru,
	ENC_ME_NEB_INFO cur_ctu_mi[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], 														// 2CHROMA
	pel dbk_y[71][71],																												// 2DBK
	FME_OUT_PARA_SET & fme_out_para_set, FME_OUT_PARA_SET_AXIS & axis_fme_out_para_set,												// FME, 2CHROMA
	ENC_CU_DATA & md_output_luma,																									// 2CHROMA, no scalar.
	ENC_ME_LINE_MAP * me_line_map_ptr,   ENC_ME_LINE_MAP_AXIS * axis_me_line_map_ptr,   ENC_ME_LINE_MAP * me_line_map_ptr_luma,
	const pel ram_in_buffer_p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const SKIP_MERGE_INPUT_AXIS_IN &skip_in_axis_in);	// FETCH, 2CHROMA

#else
void md_run(
	pel fme2mdrefbuf[2560][8][2][16],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, 
#endif
	//ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	//ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_input_ptr,
	RMD_OUTPUT* rmd_output,
	ENC_FME_INFO_ALL* fme_mv_info,
	pel dbk_y[71][71],
	FME_OUT_PARA_SET* fme_out_para_set,
	ENC_CU_DATA* md2_output, ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_input_luma,
	strFetch_ref_window* Fetch_Ref_window_ptr);
#endif

void md_run_chroma(
	//ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	//ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO cur_ctu_mi[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16],
	ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN], ENC_ME_NEB_INFO new_above_nebs_line_DBK[ABOVE_LINE_LEN],
	/*ENC_FME_INFO_ALL* fme_mv_info,*/
	pel dbk_u[39][39], pel dbk_v[39][39],
	FME_OUT_PARA_SET* fme_out_para_set_md2chroma,
	ENC_CU_DATA* ctu_map_cu_data, ENC_CU_DATA* ctu_map_cu_data_DBK, MD2SAO_* md2sao, ENC_ME_LINE_MAP* me_line_map_ptr_luma,
	strFetch_ref_window* Fetch_Ref_window_md2chroma,
	ENC_CU_DATA* md_output_luma, ENC_ME_LINE_MAP* me_line_map_ptr_DBK, ENC_ME_LINE_MAP* me_line_map_ptr_AEC, ENC_ME_LINE_MAP* me_line_map_ptr
	, U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN]
	#if CHROMA_AXIS_ADAPTER
	,FME_OUT_PARA_SET_COPY* axis_fme_out_para_set, ENC_ME_LINE_MAP_COPY* axis_me_line_map_ptr_luma, ENC_ME_LINE_MAP_COPY* axis_me_line_map_ptr_DBK, ENC_ME_LINE_MAP_COPY* axis_me_line_map_ptr_AEC, 
	ENC_ME_LINE_MAP_COPY* axis_me_line_map_ptr, ENC_CU_DATA_COPY* axis_ctu_map_cu_data, ENC_CU_DATA_COPY* axis_md_output_luma, strFetch_ref_window_COPY* axis_Fetch_Ref_window_ptr
	#endif
	);

void load_md2(u8 ip_list[IPD_RDO_CNT], RMD_OUTPUT* g_rmd_output, U3 cu_width_log2, U3 cu_height_log2, U14 x0, U14 y0, int split_mode_father, IP_ARRAY_SIZE<16, 16>& IP_buffer_16x16,
	U3 part_num, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_16x16, IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32,
	MPM_ARRAY_SIZE<16, 16>& MPM_buffer_16x16, MPM_ARRAY_SIZE<16, 32>* MPM_buffer_ptr_16x32, ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	ENC_ME_NEB_INFO up_nebs_father[1 + (32 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(32 >> 2)], ENC_ME_NEB_INFO	nebs_line_hor[4], U8 cu_width, U8 cu_height,
	ENC_ME_LINE_MAP_SIZE<16>& me_line_map_16, ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	MPM_ARRAY_SIZE<32, 16>* MPM_buffer_ptr_32x16, ENC_ME_NEB_INFO nebs_line_ver[4], ME_MV_DIR* me_mv_dir, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], SKIP_MERGE_INPUT* skip_in,
	ENC_FME_INFO_ALL* fme_mv_info, strFetch_ref_window* Fetch_Ref_window_ptr, MD_FW* md_fw_ptr, ME_MV_DIR_16& me_mv_dir_16, pel org_y_16[16][16], ENC_FME_INFO_ALL_16& fme_mv_info_16,
	strFetch_ref_window_16& Fetch_Ref_window_16, SKIP_MERGE_INPUT_16& skip_in_16);

void pu_test_0(s64& cost_temp, U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR_16& me_mv_dir,
#endif
	pel p_fenc_LCU_Y[16][16],
	ENC_ME_LINE_MAP_SIZE<16>& me_line_map_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16& skip_in_16,
	U1 boundary, U1 isNS_allow, U2 &NS_luma_pred_mode, U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	ENC_CU_DATA_ARRAY_SIZE<16>&cu_data_temp_ptr_16,
	ENC_FME_INFO_ALL_16&fme_mv_info,
	IP_ARRAY_SIZE<16, 16>& IP_buffer_ptr_16x16, IP_ARRAY_SIZE<8, 16>& IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>& IP_buffer_ptr_16x8,
	MPM_ARRAY_SIZE<16, 16>& MPM_buffer_ptr_16x16, MPM_ARRAY_SIZE<8, 16>& MPM_buffer_ptr_8x16, MPM_ARRAY_SIZE<16, 8>& MPM_buffer_ptr_16x8,
	IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_ptr_16x16, IP_MAP_SCU_SIZE<8, 16>& IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>& IP_map_scu_ptr_16x8,
	Arbitor& ref,
	strFetch_ref_window_16&Fetch_Ref_window_ptr
#if SUB_TMVP
	, SKIP_MERGE_INPUT* skip_in
#endif

);

void pu_1(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref,
	COM_SPLIT_STRUCT split_struct, U3 part_num, ENC_ME_NEB_INFO up_nebs[6], ENC_ME_NEB_INFO left_nebs[6], U3 split_mode);

void pu_test_1(s64 &cost_temp,U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<16>& me_line_map_16,
	ENC_ME_NEB_INFO up_nebs[6], ENC_ME_NEB_INFO left_nebs[4],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<16>& cu_data_ptr,
	U2& cons_pred_mode_BTH, U2 tree_status, U2& child_tree_status,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>&IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>& MPM_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_ptr_16x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);

void store_md2(
	MD_FW* md_fw_ptr, s64& cost_best, s64 cost0, s64 cost1, s64 cost2,
	ENC_CU_DATA_ARRAY_SIZE<16>& curr_cu_data_ptr0,
	ENC_CU_DATA_ARRAY_SIZE<16>& curr_cu_data_ptr1,
	ENC_CU_DATA_ARRAY_SIZE<16>& curr_cu_data_ptr2,
	ENC_CU_DATA_ARRAY_SIZE<16>& curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status,
	U3& best_split_mode, U2& best_cons_pred_mode, U2 cons_pred_mode_child0,
	U2 cons_pred_mode_child1, U2 cons_pred_mode_child2, U2 tree_status_child0,
	U2 tree_status_child1, U2 tree_status_child2, U2& best_child_status,
	U2& tree_status_temp,

	int split_mode_father, IP_ARRAY_SIZE<16, 16>& IP_buffer_16x16,
	IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_16x16, MPM_ARRAY_SIZE<16, 16>& MPM_buffer_16x16,
	ENC_ME_NEB_INFO	nebs_line_ver[4], ENC_ME_NEB_INFO nebs_line_hor[4],
	ENC_ME_LINE_MAP_SIZE<16>& me_line_map_16, U3 part_num, U8 cu_width, U8 cu_height,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32,
	IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,

	U14 x0, U14 y0, U12 cup,
	u8 cons_pred_mode, U3 num_split_to_try,
	ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw, U3* best_split_mode0, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best
);

#define MD_CHROMA(w, h) s64 FUNC_NAME(md_chroma, w, h)(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],\
ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr, U14 x, U14 y, U3 cu_width_log2,\
U3 cu_height_log2, U2 tree_status, U4 cud, MD_INPUT* md_input_ptr, strFetch_Orig_Lcu* Orig_Lcu_ptr,\
strFetch_ref_window* Fetch_Refwin_ptr, u8 need_dochroma, U12 cup, u8 ctx_cons_pred_mode,\
IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr);\

#endif

void write_in_para_set_md(FME_OUT_PARA_SET* fme_out_para_set, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr);
void store_enc_info_top_paral(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	U12 cup,
	U4 cud, U2 cons_pred_mode, U7 cu_width, U7 cu_height, U2 tree_status, MD_RESULT md_best_result[2],
	ENC_CU_DATA md_best_ctu_output[2],
	ENC_CU_DATA* md2_output, pel dbk_y[71][71],
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], U1 best_index);
