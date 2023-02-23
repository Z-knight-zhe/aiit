#include "md1.h"
#include "md_cmod.h"

void make_skip_cand(S7 mode_list[SKIP_RDO_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const S32 mode_sad[SKIP_MODE_NUM]);
void mc_center(
	SKIP_MODE_INTER mode[SKIP_MODE_NUM],
	S12 mc_pos[SKIP_MC_NUM][MV_D],
	const U12 x_pos,
	const U12 y_pos,
	const U7 cu_width,
	const U7 cu_height,
	const U12 pic_w,
	const U12 pic_h
	);
void derive_mvp_line_all(MD_FW *md_fw_ptr, ENC_ME_LINE_MAP *me_line_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], MD_COM_MODE *mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2]);
void derive_neb_cu(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], const U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], const U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[MAX_CU_IN_SCU + 2], U32 map_scu_above_line[MAX_CU_IN_SCU + 2], U32 map_cu_mode_above_line[MAX_CU_IN_SCU + 2], const U1 curr_ctu_idx);
void derive_inter_cands(SKIP_MERGE_INPUT * input, U6 *num_cands_all, U4 *num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window* Fetch_Ref_window_ptr);
void output_skip_mode(SKIP_RDO * skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], SKIP_ELEMENT * mode, S7 mode_list[MAX_INTER_SKIP_RDO]);
void shifting(S15 *reg, const S15 in_data, const U4 depth);
BOOL check_mv_in_refwin(const int top, const int left, const int x_pos, const int y_pos, const int cu_width, const int cu_height, const int x_chroma, const int y_chroma, const BOOL chroma_motion);
S21 mc_8tag(const u8 idx, const S15 data[8]);

int encode_mvd_est(S16 mvd[2]);
int sbac_write_truncate_unary_sym_est(U8 sym, U8 max_num);
int encode_affine_flag_est(int flag, u8 affine_enable_flag, int  cu_width_log2, int cu_height_log2);
int encode_skip_idx_cu_est(u8  skip_idx, u8 num_hmvp_cands, u8 slice_type);
int encode_umve_idx_est(int umve_idx);
int encode_intra_dir_est(u8 ipm, u8 mpm[2]);

int enc_eco_run_est_3(U10 run);
int enc_eco_level_est_3(U10 sym);

void cu_plane_nz_cln(int dst[4][3], int plane);

void cal_base_cost(s64 d64_base_cost[64], s64 mat_coded_cost[64], s64 mat_uncoded_cost[64], s64 d64_cost_last_zero[64], U24 lev_opt[64]);
int compare_top(s64 data[64], s64* best_data);

s64 md_kernel_8(U8 width, U8 height, MD_FW*md_fw_ptr, MD_INPUT *md_input_ptr,
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE],
#else
	u8 rmd_mode[IPD_RDO_CNT],
#endif 
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4],
	ENC_ME_NEB_INFO left_nebs[2], 
	SKIP_MERGE_INPUT *skip_in, 
	U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0, U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	ENC_FME_INFO_ALL *fme_mv_info,
	IP_ARRAY_SIZE<8, 8> *IP_buffer_ptr_8x8, 
	MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8, 
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);

