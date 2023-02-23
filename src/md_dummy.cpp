#include "md1.h"
#include "md_kernel_32.h"
#include "md_dummy.h"

#if MD_KERNEL_SET_DUMMY_VALUE
#define DUMMY_OUT(dummy) (zero ? (dummy) : 0)
#else
#define DUMMY_OUT(dummy) (dummy)
#endif //MD_KERNEL_SET_DUMMY_VALUE

#if LUMA_8_PIPELINE_DUMMY
void rdo_luma_8_pipeline(u8 all_rdo_num, U8 w, U8 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, U2 curr_cons_pred_mode, U4 bit_depth, u16 avail_tb,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr, ENC_CU_DATA_ARRAY_INNER* cu_data_temp_ptr,
	u8 rdo_list[MD_MAX_RDO_NUM], U1 valid_mode[17], u8 ipred_list[IPRED_LIST_NUM], pel src_le_temp[2 * 8 + 3], pel src_up_temp[2 * 8 + 3], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	pel orgY[8 * 8], 
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif
	s64 flag_cost[2], s32 dist,
	s64* cost_best, 
	pel fme2mdrefbuf[2560][8][2][16])
{
#pragma HLS INLINE OFF

	s64 dummy = MAX_S64 >> 4;
 // ::u8 all_rdo_num; 
    dummy+=all_rdo_num;

 // ::U8 w; 
    dummy+=w;

 // ::U8 h; 
    dummy+=h;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U2 curr_cons_pred_mode; 
    dummy+=curr_cons_pred_mode;

 // ::U4 bit_depth; 
    dummy+=bit_depth;

 // ::u16 avail_tb; 
    dummy+=avail_tb;

#if !AZB_ENABLE0	
 // ::RDOQ_ARRAY* rdoq_array; 
    dummy+=rdoq_array[0].rdoq_est_run[0][0];
    dummy+=rdoq_array[0].rdoq_est_run[0][1];
    dummy+=rdoq_array[0].rdoq_est_run[1][0];
    dummy+=rdoq_array[0].rdoq_est_run[1][1];
    dummy+=rdoq_array[0].rdoq_est_run[2][0];
    dummy+=rdoq_array[0].rdoq_est_run[2][1];
    dummy+=rdoq_array[0].rdoq_est_run[3][0];
    dummy+=rdoq_array[0].rdoq_est_run[3][1];
    dummy+=rdoq_array[0].rdoq_est_run[4][0];
    dummy+=rdoq_array[0].rdoq_est_run[4][1];
    dummy+=rdoq_array[0].rdoq_est_run[5][0];
    dummy+=rdoq_array[0].rdoq_est_run[5][1];
    dummy+=rdoq_array[0].rdoq_est_run[6][0];
    dummy+=rdoq_array[0].rdoq_est_run[6][1];
    dummy+=rdoq_array[0].rdoq_est_run[7][0];
    dummy+=rdoq_array[0].rdoq_est_run[7][1];
    dummy+=rdoq_array[0].rdoq_est_run[8][0];
    dummy+=rdoq_array[0].rdoq_est_run[8][1];
    dummy+=rdoq_array[0].rdoq_est_run[9][0];
    dummy+=rdoq_array[0].rdoq_est_run[9][1];
    dummy+=rdoq_array[0].rdoq_est_run[10][0];
    dummy+=rdoq_array[0].rdoq_est_run[10][1];
    dummy+=rdoq_array[0].rdoq_est_run[11][0];
    dummy+=rdoq_array[0].rdoq_est_run[11][1];
    dummy+=rdoq_array[0].rdoq_est_run[12][0];
    dummy+=rdoq_array[0].rdoq_est_run[12][1];
    dummy+=rdoq_array[0].rdoq_est_run[13][0];
    dummy+=rdoq_array[0].rdoq_est_run[13][1];
    dummy+=rdoq_array[0].rdoq_est_run[14][0];
    dummy+=rdoq_array[0].rdoq_est_run[14][1];
    dummy+=rdoq_array[0].rdoq_est_run[15][0];
    dummy+=rdoq_array[0].rdoq_est_run[15][1];
    dummy+=rdoq_array[0].rdoq_est_run[16][0];
    dummy+=rdoq_array[0].rdoq_est_run[16][1];
    dummy+=rdoq_array[0].rdoq_est_run[17][0];
    dummy+=rdoq_array[0].rdoq_est_run[17][1];
    dummy+=rdoq_array[0].rdoq_est_run[18][0];
    dummy+=rdoq_array[0].rdoq_est_run[18][1];
    dummy+=rdoq_array[0].rdoq_est_run[19][0];
    dummy+=rdoq_array[0].rdoq_est_run[19][1];
    dummy+=rdoq_array[0].rdoq_est_run[20][0];
    dummy+=rdoq_array[0].rdoq_est_run[20][1];
    dummy+=rdoq_array[0].rdoq_est_run[21][0];
    dummy+=rdoq_array[0].rdoq_est_run[21][1];
    dummy+=rdoq_array[0].rdoq_est_run[22][0];
    dummy+=rdoq_array[0].rdoq_est_run[22][1];
    dummy+=rdoq_array[0].rdoq_est_run[23][0];
    dummy+=rdoq_array[0].rdoq_est_run[23][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][1];
    dummy+=rdoq_array[0].rdoq_est_level[0][0];
    dummy+=rdoq_array[0].rdoq_est_level[0][1];
    dummy+=rdoq_array[0].rdoq_est_level[1][0];
    dummy+=rdoq_array[0].rdoq_est_level[1][1];
    dummy+=rdoq_array[0].rdoq_est_level[2][0];
    dummy+=rdoq_array[0].rdoq_est_level[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[3][0];
    dummy+=rdoq_array[0].rdoq_est_level[3][1];
    dummy+=rdoq_array[0].rdoq_est_level[4][0];
    dummy+=rdoq_array[0].rdoq_est_level[4][1];
    dummy+=rdoq_array[0].rdoq_est_level[5][0];
    dummy+=rdoq_array[0].rdoq_est_level[5][1];
    dummy+=rdoq_array[0].rdoq_est_level[6][0];
    dummy+=rdoq_array[0].rdoq_est_level[6][1];
    dummy+=rdoq_array[0].rdoq_est_level[7][0];
    dummy+=rdoq_array[0].rdoq_est_level[7][1];
    dummy+=rdoq_array[0].rdoq_est_level[8][0];
    dummy+=rdoq_array[0].rdoq_est_level[8][1];
    dummy+=rdoq_array[0].rdoq_est_level[9][0];
    dummy+=rdoq_array[0].rdoq_est_level[9][1];
    dummy+=rdoq_array[0].rdoq_est_level[10][0];
    dummy+=rdoq_array[0].rdoq_est_level[10][1];
    dummy+=rdoq_array[0].rdoq_est_level[11][0];
    dummy+=rdoq_array[0].rdoq_est_level[11][1];
    dummy+=rdoq_array[0].rdoq_est_level[12][0];
    dummy+=rdoq_array[0].rdoq_est_level[12][1];
    dummy+=rdoq_array[0].rdoq_est_level[13][0];
    dummy+=rdoq_array[0].rdoq_est_level[13][1];
    dummy+=rdoq_array[0].rdoq_est_level[14][0];
    dummy+=rdoq_array[0].rdoq_est_level[14][1];
    dummy+=rdoq_array[0].rdoq_est_level[15][0];
    dummy+=rdoq_array[0].rdoq_est_level[15][1];
    dummy+=rdoq_array[0].rdoq_est_level[16][0];
    dummy+=rdoq_array[0].rdoq_est_level[16][1];
    dummy+=rdoq_array[0].rdoq_est_level[17][0];
    dummy+=rdoq_array[0].rdoq_est_level[17][1];
    dummy+=rdoq_array[0].rdoq_est_level[18][0];
    dummy+=rdoq_array[0].rdoq_est_level[18][1];
    dummy+=rdoq_array[0].rdoq_est_level[19][0];
    dummy+=rdoq_array[0].rdoq_est_level[19][1];
    dummy+=rdoq_array[0].rdoq_est_level[20][0];
    dummy+=rdoq_array[0].rdoq_est_level[20][1];
    dummy+=rdoq_array[0].rdoq_est_level[21][0];
    dummy+=rdoq_array[0].rdoq_est_level[21][1];
    dummy+=rdoq_array[0].rdoq_est_level[22][0];
    dummy+=rdoq_array[0].rdoq_est_level[22][1];
    dummy+=rdoq_array[0].rdoq_est_level[23][0];
    dummy+=rdoq_array[0].rdoq_est_level[23][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][0][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][1][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][2][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][3][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][4][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[0][5][11][1];
#endif
 // ::MD_FW* md_fw_ptr; 
    // md_fw_ptr[0] . pic_width is ignored ;
    // md_fw_ptr[0] . pic_height is ignored ;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    // md_fw_ptr[0] . pic_width_in_scu is ignored ;
    // md_fw_ptr[0] . pic_height_in_scu is ignored ;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . refp_ptr[0][0] is ignored ;
    // md_fw_ptr[0] . num_refp is ignored ;
    // md_fw_ptr[0] . cur_ptr is ignored ;

 // ::MD_INPUT* md_input_ptr; 
    // md_input_ptr[0] . pix_x is ignored ;
    // md_input_ptr[0] . pix_y is ignored ;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
#if !AZB_ENABLE0	
    dummy+=md_input_ptr[0] . qp_y;
#endif
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPosFME[0][0][0] is ignored ;

 // ::SKIP_RDO* skip_rdo; 
    // skip_rdo[0] . pmv_cands[0][0][0] is ignored ;
    // skip_rdo[0] . refi_cands[0][0] is ignored ;
    // skip_rdo[0] . mode_list[0] is ignored ;
    // skip_rdo[0] . num_rdo is ignored ;

 // ::MERGE_RDO merge_rdo; 
    // merge_rdo . mrg_list_refi[0][0] is ignored ;
    // merge_rdo . mrg_list_cp_mv[0][0][0][0] is ignored ;
    // merge_rdo . mrg_list_cp_num[0] is ignored ;
    // merge_rdo . num_cands is ignored ;
    // merge_rdo . valid[0] is ignored ;

 // ::SKIP_ELEMENT[12] skip_mode; 
    // skip_mode[0] . valid is ignored ;
    // skip_mode[0] . skip_flag is ignored ;
    // skip_mode[0] . skip_idx is ignored ;
    // skip_mode[0] . umve_flag is ignored ;
    // skip_mode[0] . umve_idx is ignored ;
    // skip_mode[0] . affine_flag is ignored ;
    // skip_mode[0] . mvp[0] is ignored ;
    // skip_mode[0] . refi is ignored ;

 // ::SKIP_ELEMENT[12] skip_rdo_mode; 
    // skip_rdo_mode[0] . valid is ignored ;
    // skip_rdo_mode[0] . skip_flag is ignored ;
    // skip_rdo_mode[0] . skip_idx is ignored ;
    // skip_rdo_mode[0] . umve_flag is ignored ;
    // skip_rdo_mode[0] . umve_idx is ignored ;
    // skip_rdo_mode[0] . affine_flag is ignored ;
    // skip_rdo_mode[0] . mvp[0] is ignored ;
    // skip_rdo_mode[0] . refi is ignored ;

 // ::MD_COM_MODE* mod_info_curr_ptr; 
    // mod_info_curr_ptr[0] . x_scu is ignored ;
    // mod_info_curr_ptr[0] . y_scu is ignored ;
    // mod_info_curr_ptr[0] . cud is ignored ;
    // mod_info_curr_ptr[0] . cu_width is ignored ;
    // mod_info_curr_ptr[0] . cu_height is ignored ;
    // mod_info_curr_ptr[0] . cu_width_log2 is ignored ;
    // mod_info_curr_ptr[0] . cu_height_log2 is ignored ;
    // mod_info_curr_ptr[0] . x_pos is ignored ;
    // mod_info_curr_ptr[0] . y_pos is ignored ;
    // mod_info_curr_ptr[0] . scup is ignored ;
    (mod_info_curr_ptr[0] . cu_mode)++;
    dummy+=mod_info_curr_ptr[0] . pb_part;
    (mod_info_curr_ptr[0] . tb_part)++;
    (mod_info_curr_ptr[0].num_nz[0][0])++;
    (mod_info_curr_ptr[0].num_nz[0][1])++;
    (mod_info_curr_ptr[0].num_nz[0][2])++;
    (mod_info_curr_ptr[0].num_nz[1][0])++;
    (mod_info_curr_ptr[0].num_nz[1][1])++;
    (mod_info_curr_ptr[0].num_nz[1][2])++;
    (mod_info_curr_ptr[0].num_nz[2][0])++;
    (mod_info_curr_ptr[0].num_nz[2][1])++;
    (mod_info_curr_ptr[0].num_nz[2][2])++;
    (mod_info_curr_ptr[0].num_nz[3][0])++;
    (mod_info_curr_ptr[0].num_nz[3][1])++;
    (mod_info_curr_ptr[0].num_nz[3][2])++;
    // mod_info_curr_ptr[0] . refi[0] is ignored ;
    // mod_info_curr_ptr[0] . mvr_idx is ignored ;
    dummy+=mod_info_curr_ptr[0] . umve_flag;
    dummy+=mod_info_curr_ptr[0] . umve_idx;
    dummy+=mod_info_curr_ptr[0] . skip_idx;
    dummy+=mod_info_curr_ptr[0] . mvp_from_hmvp_flag;
    // mod_info_curr_ptr[0] . mvd[0][0] is ignored ;
    // mod_info_curr_ptr[0] . mv[0][0] is ignored ;
    dummy+=mod_info_curr_ptr[0] . affine_flag;
    // mod_info_curr_ptr[0] . affine_mv[0][0][0] is ignored ;
    // mod_info_curr_ptr[0] . affine_mvd[0][0][0] is ignored ;
    dummy+=mod_info_curr_ptr[0] . smvd_flag;
    dummy+=mod_info_curr_ptr[0].mpm[0][0];
    dummy+=mod_info_curr_ptr[0].mpm[0][1];
    dummy+=mod_info_curr_ptr[0].mpm[1][0];
    dummy+=mod_info_curr_ptr[0].mpm[1][1];
    dummy+=mod_info_curr_ptr[0].mpm[2][0];
    dummy+=mod_info_curr_ptr[0].mpm[2][1];
    dummy+=mod_info_curr_ptr[0].mpm[3][0];
    dummy+=mod_info_curr_ptr[0].mpm[3][1];
    // mod_info_curr_ptr[0] . ipm[0][0] is ignored ;
    // mod_info_curr_ptr[0] . ipf_flag is ignored ;
    // mod_info_curr_ptr[0] . chroma_motion is ignored ;
    // mod_info_curr_ptr[0] . x_luma is ignored ;
    // mod_info_curr_ptr[0] . y_luma is ignored ;
    // mod_info_curr_ptr[0] . x_chroma is ignored ;
    // mod_info_curr_ptr[0] . y_chroma is ignored ;
    // mod_info_curr_ptr[0] . last_child is ignored ;

 // ::MD_COM_MODE_BEST* best_info_ptr; 
    // best_info_ptr[0] . x_scu is ignored ;
    // best_info_ptr[0] . y_scu is ignored ;
    // best_info_ptr[0] . cud is ignored ;
    // best_info_ptr[0] . cu_width is ignored ;
    // best_info_ptr[0] . cu_height is ignored ;
    // best_info_ptr[0] . cu_width_log2 is ignored ;
    // best_info_ptr[0] . cu_height_log2 is ignored ;
    // best_info_ptr[0] . x_pos is ignored ;
    // best_info_ptr[0] . y_pos is ignored ;
    // best_info_ptr[0] . scup is ignored ;
    best_info_ptr[0] . cu_mode=dummy;
    best_info_ptr[0] . pb_part=dummy;
    best_info_ptr[0] . tb_part=dummy;
    // best_info_ptr[0] . rec_y[0] is ignored ;
    // best_info_ptr[0] . rec_u[0] is ignored ;
    // best_info_ptr[0] . rec_v[0] is ignored ;
    // best_info_ptr[0] . coef_y[0] is ignored ;
    // best_info_ptr[0] . coef_u[0] is ignored ;
    // best_info_ptr[0] . coef_v[0] is ignored ;
    // best_info_ptr[0] . pred_y[0] is ignored ;
    // best_info_ptr[0] . pred_u[0] is ignored ;
    // best_info_ptr[0] . pred_v[0] is ignored ;
    best_info_ptr[0].num_nz[0][0]=dummy;
    best_info_ptr[0].num_nz[0][1]=dummy;
    best_info_ptr[0].num_nz[0][2]=dummy;
    best_info_ptr[0].num_nz[1][0]=dummy;
    best_info_ptr[0].num_nz[1][1]=dummy;
    best_info_ptr[0].num_nz[1][2]=dummy;
    best_info_ptr[0].num_nz[2][0]=dummy;
    best_info_ptr[0].num_nz[2][1]=dummy;
    best_info_ptr[0].num_nz[2][2]=dummy;
    best_info_ptr[0].num_nz[3][0]=dummy;
    best_info_ptr[0].num_nz[3][1]=dummy;
    best_info_ptr[0].num_nz[3][2]=dummy;
    // best_info_ptr[0] . refi[0] is ignored ;
    best_info_ptr[0] . mvr_idx=dummy;
    best_info_ptr[0] . umve_flag=dummy;
    best_info_ptr[0] . umve_idx=dummy;
    best_info_ptr[0] . skip_idx=dummy;
    best_info_ptr[0] . mvp_from_hmvp_flag=dummy;
    // best_info_ptr[0] . mvd[0][0] is ignored ;
    // best_info_ptr[0] . mv[0][0] is ignored ;
    best_info_ptr[0] . affine_flag=dummy;
    // best_info_ptr[0] . affine_mv[0][0][0] is ignored ;
    // best_info_ptr[0] . affine_mvd[0][0][0] is ignored ;
    best_info_ptr[0] . smvd_flag=dummy;
    best_info_ptr[0].mpm[0][0]=dummy;
    best_info_ptr[0].mpm[0][1]=dummy;
    best_info_ptr[0].mpm[1][0]=dummy;
    best_info_ptr[0].mpm[1][1]=dummy;
    best_info_ptr[0].mpm[2][0]=dummy;
    best_info_ptr[0].mpm[2][1]=dummy;
    best_info_ptr[0].mpm[3][0]=dummy;
    best_info_ptr[0].mpm[3][1]=dummy;
    best_info_ptr[0].ipm[0][0]=dummy;
    best_info_ptr[0].ipm[0][1]=dummy;
    // best_info_ptr[0] . ipf_flag is ignored ;
    // best_info_ptr[0] . chroma_motion is ignored ;
    // best_info_ptr[0] . x_luma is ignored ;
    // best_info_ptr[0] . y_luma is ignored ;
    // best_info_ptr[0] . x_chroma is ignored ;
    // best_info_ptr[0] . y_chroma is ignored ;
    // best_info_ptr[0] . last_child is ignored ;

 // ::ENC_CU_DATA_ARRAY* cu_data_temp_ptr; 
    // cu_data_temp_ptr[0] . split_mode[0][0][0] is ignored ;
    // cu_data_temp_ptr[0] . mv[0][0][0] is ignored ;
    // cu_data_temp_ptr[0] . mvd[0][0][0] is ignored ;
    // cu_data_temp_ptr[0] . pb_part[0] is ignored ;
    // cu_data_temp_ptr[0] . tb_part[0] is ignored ;
    // cu_data_temp_ptr[0] . pred_mode[0] is ignored ;
    // cu_data_temp_ptr[0] . mvr_idx[0] is ignored ;
    // cu_data_temp_ptr[0] . umve_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . umve_idx[0] is ignored ;
    // cu_data_temp_ptr[0] . skip_idx[0] is ignored ;
    // cu_data_temp_ptr[0] . map_scu[0] is ignored ;
    // cu_data_temp_ptr[0] . affine_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . smvd_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . map_cu_mode[0] is ignored ;
    // cu_data_temp_ptr[0] . map_pb_tb_part[0] is ignored ;
    // cu_data_temp_ptr[0] . depth[0] is ignored ;
    // cu_data_temp_ptr[0] . ipf_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . mvp_from_hmvp_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . mpm[0][0] is ignored ;
    // cu_data_temp_ptr[0] . mpm_ext[0][0] is ignored ;
    // cu_data_temp_ptr[0] . ipm[0][0] is ignored ;
    // cu_data_temp_ptr[0] . refi[0][0] is ignored ;
    // cu_data_temp_ptr[0] . num_nz_coef[0][0] is ignored ;
    cu_data_temp_ptr[0].coef_y[0]=dummy;
    cu_data_temp_ptr[0].coef_y[1]=dummy;
    cu_data_temp_ptr[0].coef_y[2]=dummy;
    cu_data_temp_ptr[0].coef_y[3]=dummy;
    cu_data_temp_ptr[0].coef_y[4]=dummy;
    cu_data_temp_ptr[0].coef_y[5]=dummy;
    cu_data_temp_ptr[0].coef_y[6]=dummy;
    cu_data_temp_ptr[0].coef_y[7]=dummy;
    cu_data_temp_ptr[0].coef_y[8]=dummy;
    cu_data_temp_ptr[0].coef_y[9]=dummy;
    cu_data_temp_ptr[0].coef_y[10]=dummy;
    cu_data_temp_ptr[0].coef_y[11]=dummy;
    cu_data_temp_ptr[0].coef_y[12]=dummy;
    cu_data_temp_ptr[0].coef_y[13]=dummy;
    cu_data_temp_ptr[0].coef_y[14]=dummy;
    cu_data_temp_ptr[0].coef_y[15]=dummy;
    cu_data_temp_ptr[0].coef_y[16]=dummy;
    cu_data_temp_ptr[0].coef_y[17]=dummy;
    cu_data_temp_ptr[0].coef_y[18]=dummy;
    cu_data_temp_ptr[0].coef_y[19]=dummy;
    cu_data_temp_ptr[0].coef_y[20]=dummy;
    cu_data_temp_ptr[0].coef_y[21]=dummy;
    cu_data_temp_ptr[0].coef_y[22]=dummy;
    cu_data_temp_ptr[0].coef_y[23]=dummy;
    cu_data_temp_ptr[0].coef_y[24]=dummy;
    cu_data_temp_ptr[0].coef_y[25]=dummy;
    cu_data_temp_ptr[0].coef_y[26]=dummy;
    cu_data_temp_ptr[0].coef_y[27]=dummy;
    cu_data_temp_ptr[0].coef_y[28]=dummy;
    cu_data_temp_ptr[0].coef_y[29]=dummy;
    cu_data_temp_ptr[0].coef_y[30]=dummy;
    cu_data_temp_ptr[0].coef_y[31]=dummy;
    cu_data_temp_ptr[0].coef_y[32]=dummy;
    cu_data_temp_ptr[0].coef_y[33]=dummy;
    cu_data_temp_ptr[0].coef_y[34]=dummy;
    cu_data_temp_ptr[0].coef_y[35]=dummy;
    cu_data_temp_ptr[0].coef_y[36]=dummy;
    cu_data_temp_ptr[0].coef_y[37]=dummy;
    cu_data_temp_ptr[0].coef_y[38]=dummy;
    cu_data_temp_ptr[0].coef_y[39]=dummy;
    cu_data_temp_ptr[0].coef_y[40]=dummy;
    cu_data_temp_ptr[0].coef_y[41]=dummy;
    cu_data_temp_ptr[0].coef_y[42]=dummy;
    cu_data_temp_ptr[0].coef_y[43]=dummy;
    cu_data_temp_ptr[0].coef_y[44]=dummy;
    cu_data_temp_ptr[0].coef_y[45]=dummy;
    cu_data_temp_ptr[0].coef_y[46]=dummy;
    cu_data_temp_ptr[0].coef_y[47]=dummy;
    cu_data_temp_ptr[0].coef_y[48]=dummy;
    cu_data_temp_ptr[0].coef_y[49]=dummy;
    cu_data_temp_ptr[0].coef_y[50]=dummy;
    cu_data_temp_ptr[0].coef_y[51]=dummy;
    cu_data_temp_ptr[0].coef_y[52]=dummy;
    cu_data_temp_ptr[0].coef_y[53]=dummy;
    cu_data_temp_ptr[0].coef_y[54]=dummy;
    cu_data_temp_ptr[0].coef_y[55]=dummy;
    cu_data_temp_ptr[0].coef_y[56]=dummy;
    cu_data_temp_ptr[0].coef_y[57]=dummy;
    cu_data_temp_ptr[0].coef_y[58]=dummy;
    cu_data_temp_ptr[0].coef_y[59]=dummy;
    cu_data_temp_ptr[0].coef_y[60]=dummy;
    cu_data_temp_ptr[0].coef_y[61]=dummy;
    cu_data_temp_ptr[0].coef_y[62]=dummy;
    cu_data_temp_ptr[0].coef_y[63]=dummy;
    // cu_data_temp_ptr[0] . coef_u[0] is ignored ;
    // cu_data_temp_ptr[0] . coef_v[0] is ignored ;
    cu_data_temp_ptr[0].reco_y[0]=dummy;
    cu_data_temp_ptr[0].reco_y[1]=dummy;
    cu_data_temp_ptr[0].reco_y[2]=dummy;
    cu_data_temp_ptr[0].reco_y[3]=dummy;
    cu_data_temp_ptr[0].reco_y[4]=dummy;
    cu_data_temp_ptr[0].reco_y[5]=dummy;
    cu_data_temp_ptr[0].reco_y[6]=dummy;
    cu_data_temp_ptr[0].reco_y[7]=dummy;
    cu_data_temp_ptr[0].reco_y[8]=dummy;
    cu_data_temp_ptr[0].reco_y[9]=dummy;
    cu_data_temp_ptr[0].reco_y[10]=dummy;
    cu_data_temp_ptr[0].reco_y[11]=dummy;
    cu_data_temp_ptr[0].reco_y[12]=dummy;
    cu_data_temp_ptr[0].reco_y[13]=dummy;
    cu_data_temp_ptr[0].reco_y[14]=dummy;
    cu_data_temp_ptr[0].reco_y[15]=dummy;
    cu_data_temp_ptr[0].reco_y[16]=dummy;
    cu_data_temp_ptr[0].reco_y[17]=dummy;
    cu_data_temp_ptr[0].reco_y[18]=dummy;
    cu_data_temp_ptr[0].reco_y[19]=dummy;
    cu_data_temp_ptr[0].reco_y[20]=dummy;
    cu_data_temp_ptr[0].reco_y[21]=dummy;
    cu_data_temp_ptr[0].reco_y[22]=dummy;
    cu_data_temp_ptr[0].reco_y[23]=dummy;
    cu_data_temp_ptr[0].reco_y[24]=dummy;
    cu_data_temp_ptr[0].reco_y[25]=dummy;
    cu_data_temp_ptr[0].reco_y[26]=dummy;
    cu_data_temp_ptr[0].reco_y[27]=dummy;
    cu_data_temp_ptr[0].reco_y[28]=dummy;
    cu_data_temp_ptr[0].reco_y[29]=dummy;
    cu_data_temp_ptr[0].reco_y[30]=dummy;
    cu_data_temp_ptr[0].reco_y[31]=dummy;
    cu_data_temp_ptr[0].reco_y[32]=dummy;
    cu_data_temp_ptr[0].reco_y[33]=dummy;
    cu_data_temp_ptr[0].reco_y[34]=dummy;
    cu_data_temp_ptr[0].reco_y[35]=dummy;
    cu_data_temp_ptr[0].reco_y[36]=dummy;
    cu_data_temp_ptr[0].reco_y[37]=dummy;
    cu_data_temp_ptr[0].reco_y[38]=dummy;
    cu_data_temp_ptr[0].reco_y[39]=dummy;
    cu_data_temp_ptr[0].reco_y[40]=dummy;
    cu_data_temp_ptr[0].reco_y[41]=dummy;
    cu_data_temp_ptr[0].reco_y[42]=dummy;
    cu_data_temp_ptr[0].reco_y[43]=dummy;
    cu_data_temp_ptr[0].reco_y[44]=dummy;
    cu_data_temp_ptr[0].reco_y[45]=dummy;
    cu_data_temp_ptr[0].reco_y[46]=dummy;
    cu_data_temp_ptr[0].reco_y[47]=dummy;
    cu_data_temp_ptr[0].reco_y[48]=dummy;
    cu_data_temp_ptr[0].reco_y[49]=dummy;
    cu_data_temp_ptr[0].reco_y[50]=dummy;
    cu_data_temp_ptr[0].reco_y[51]=dummy;
    cu_data_temp_ptr[0].reco_y[52]=dummy;
    cu_data_temp_ptr[0].reco_y[53]=dummy;
    cu_data_temp_ptr[0].reco_y[54]=dummy;
    cu_data_temp_ptr[0].reco_y[55]=dummy;
    cu_data_temp_ptr[0].reco_y[56]=dummy;
    cu_data_temp_ptr[0].reco_y[57]=dummy;
    cu_data_temp_ptr[0].reco_y[58]=dummy;
    cu_data_temp_ptr[0].reco_y[59]=dummy;
    cu_data_temp_ptr[0].reco_y[60]=dummy;
    cu_data_temp_ptr[0].reco_y[61]=dummy;
    cu_data_temp_ptr[0].reco_y[62]=dummy;
    cu_data_temp_ptr[0].reco_y[63]=dummy;
    // cu_data_temp_ptr[0] . reco_u[0] is ignored ;
    // cu_data_temp_ptr[0] . reco_v[0] is ignored ;
    // cu_data_temp_ptr[0] . x_chroma is ignored ;
    // cu_data_temp_ptr[0] . y_chroma is ignored ;
    // cu_data_temp_ptr[0] . x_last is ignored ;
    // cu_data_temp_ptr[0] . y_last is ignored ;
    // cu_data_temp_ptr[0] . last_child is ignored ;

 // ::u8[5] rdo_list; 
    dummy+=rdo_list[0];

 // ::U1[17] valid_mode; 
    // valid_mode[0] is ignored ;

 // ::u8[5] ipred_list; 
    dummy+=ipred_list[0];

 // ::pel[19] src_le_temp; 
    dummy+=src_le_temp[0];
    dummy+=src_le_temp[1];
    dummy+=src_le_temp[2];
    dummy+=src_le_temp[3];
    dummy+=src_le_temp[4];
    dummy+=src_le_temp[5];
    dummy+=src_le_temp[6];
    dummy+=src_le_temp[7];
    dummy+=src_le_temp[8];
    dummy+=src_le_temp[9];
    dummy+=src_le_temp[10];
    dummy+=src_le_temp[11];
    dummy+=src_le_temp[12];
    dummy+=src_le_temp[13];
    dummy+=src_le_temp[14];
    dummy+=src_le_temp[15];
    dummy+=src_le_temp[16];
    dummy+=src_le_temp[17];
    dummy+=src_le_temp[18];

 // ::pel[19] src_up_temp; 
    dummy+=src_up_temp[0];
    dummy+=src_up_temp[1];
    dummy+=src_up_temp[2];
    dummy+=src_up_temp[3];
    dummy+=src_up_temp[4];
    dummy+=src_up_temp[5];
    dummy+=src_up_temp[6];
    dummy+=src_up_temp[7];
    dummy+=src_up_temp[8];
    dummy+=src_up_temp[9];
    dummy+=src_up_temp[10];
    dummy+=src_up_temp[11];
    dummy+=src_up_temp[12];
    dummy+=src_up_temp[13];
    dummy+=src_up_temp[14];
    dummy+=src_up_temp[15];
    dummy+=src_up_temp[16];
    dummy+=src_up_temp[17];
    dummy+=src_up_temp[18];

 // ::S14[2] sub_pel_mv; 
    // sub_pel_mv[0] is ignored ;

 // ::s16[2][5][2] mvp_all; 
    // mvp_all[0][0][0] is ignored ;

 // ::pel[64] orgY; 
    dummy+=orgY[0];
    dummy+=orgY[1];
    dummy+=orgY[2];
    dummy+=orgY[3];
    dummy+=orgY[4];
    dummy+=orgY[5];
    dummy+=orgY[6];
    dummy+=orgY[7];
    dummy+=orgY[8];
    dummy+=orgY[9];
    dummy+=orgY[10];
    dummy+=orgY[11];
    dummy+=orgY[12];
    dummy+=orgY[13];
    dummy+=orgY[14];
    dummy+=orgY[15];
    dummy+=orgY[16];
    dummy+=orgY[17];
    dummy+=orgY[18];
    dummy+=orgY[19];
    dummy+=orgY[20];
    dummy+=orgY[21];
    dummy+=orgY[22];
    dummy+=orgY[23];
    dummy+=orgY[24];
    dummy+=orgY[25];
    dummy+=orgY[26];
    dummy+=orgY[27];
    dummy+=orgY[28];
    dummy+=orgY[29];
    dummy+=orgY[30];
    dummy+=orgY[31];
    dummy+=orgY[32];
    dummy+=orgY[33];
    dummy+=orgY[34];
    dummy+=orgY[35];
    dummy+=orgY[36];
    dummy+=orgY[37];
    dummy+=orgY[38];
    dummy+=orgY[39];
    dummy+=orgY[40];
    dummy+=orgY[41];
    dummy+=orgY[42];
    dummy+=orgY[43];
    dummy+=orgY[44];
    dummy+=orgY[45];
    dummy+=orgY[46];
    dummy+=orgY[47];
    dummy+=orgY[48];
    dummy+=orgY[49];
    dummy+=orgY[50];
    dummy+=orgY[51];
    dummy+=orgY[52];
    dummy+=orgY[53];
    dummy+=orgY[54];
    dummy+=orgY[55];
    dummy+=orgY[56];
    dummy+=orgY[57];
    dummy+=orgY[58];
    dummy+=orgY[59];
    dummy+=orgY[60];
    dummy+=orgY[61];
    dummy+=orgY[62];
    dummy+=orgY[63];

 // ::pel[49152] RefWindowLCU; 
    // RefWindowLCU[0] is ignored ;

 // ::S14[2][2][2] CtrPos; 
    // CtrPos[0][0][0] is ignored ;

 // ::s64[2] flag_cost; 
    (flag_cost[0])++;

 // ::s32 dist; 
    // dist is ignored ;


 // ::s64* cost_best; 
    (cost_best[0])++;
	(cost_best[0]) += dummy;
 // ::pel[88][88] refwin_md; 
    // refwin_md[0][0] is ignored ;

 // ::S14[2][2][2] CtrPosFME; 
    // CtrPosFME[0][0][0] is ignored ;

#pragma HLS resource variable=best_info_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].ipm core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].mpm core=RAM_1P_BRAM 
#pragma HLS resource variable=best_info_ptr[0].num_nz core=RAM_1P_BRAM 
#pragma HLS resource variable=cost_best core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=flag_cost core=RAM_1P_BRAM 
#pragma HLS resource variable=ipred_list core=RAM_1P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].mpm core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_curr_ptr[0].num_nz core=RAM_1P_BRAM 
// #pragma HLS resource variable=model_cnt[0].cbf core=RAM_1P_BRAM 
// #pragma HLS resource variable=model_cnt[0].last1 core=RAM_1P_BRAM 
// #pragma HLS resource variable=model_cnt[0].last2 core=RAM_1P_BRAM 
// #pragma HLS resource variable=model_cnt[0].level core=RAM_1P_BRAM 
// #pragma HLS resource variable=model_cnt[0].run_rdoq core=RAM_1P_BRAM 
#pragma HLS resource variable=rdo_list core=RAM_1P_BRAM 
#pragma HLS array_partition variable=best_info_ptr[0].ipm cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].ipm cyclic factor=2 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].mpm cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mpm cyclic factor=2 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].num_nz cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].num_nz cyclic factor=3 dim=2
#pragma HLS array_partition variable=cu_data_temp_ptr[0].coef_y cyclic factor=64 dim=1
#pragma HLS array_partition variable=cu_data_temp_ptr[0].reco_y cyclic factor=64 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].mpm cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].mpm cyclic factor=2 dim=2
#pragma HLS array_partition variable=mod_info_curr_ptr[0].num_nz cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].num_nz cyclic factor=3 dim=2
// #pragma HLS array_partition variable=model_cnt[0].cbf cyclic factor=3 dim=1
// #pragma HLS array_partition variable=model_cnt[0].last1 cyclic factor=12 dim=1
// #pragma HLS array_partition variable=model_cnt[0].last2 cyclic factor=22 dim=1
// #pragma HLS array_partition variable=model_cnt[0].level cyclic factor=24 dim=1
// #pragma HLS array_partition variable=model_cnt[0].run_rdoq cyclic factor=24 dim=1
#pragma HLS array_partition variable=orgY complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=3
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=4
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=2
#pragma HLS array_partition variable=src_le_temp complete dim=1
#pragma HLS array_partition variable=src_up_temp complete dim=1
}
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
	MD_KERNEL_INPUT* md_kernel_input, u8 mpm[MAX_NUM_PB][2], U7 cu_width, int refp_ptr0, int refp_ptr1, U2 ch_type, s64 lambda, s64 cost_skip, s64 cost_best[1], U6 qp
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
#pragma HLS INLINE OFF
	 int dummy = 0;

 // ::u8 all_rdo_num; 
    // all_rdo_num is ignored ;

 // ::U8 w; 
    // w is ignored ;

 // ::U8 h; 
    // h is ignored ;

 // ::pel[256] orgY; 
    (orgY[0])++;
    (orgY[1])++;
    (orgY[2])++;
    (orgY[3])++;
    (orgY[4])++;
    (orgY[5])++;
    (orgY[6])++;
    (orgY[7])++;
    (orgY[8])++;
    (orgY[9])++;
    (orgY[10])++;
    (orgY[11])++;
    (orgY[12])++;
    (orgY[13])++;
    (orgY[14])++;
    (orgY[15])++;

 // ::pel[16][16] orgY_nz; 
    // orgY_nz[0][0] is ignored ;

 // ::pel[16][16] org_diff; 
    // org_diff[0][0] is ignored ;

 // ::U1[17] valid_mode; 
    // valid_mode[0] is ignored ;

 // ::u8[5] ipred_list; 
    (ipred_list[0])=(ipred_list[1]/*TODO:*/)=(ipred_list[0])+(ipred_list[1]/*TODO:*/);

 // ::pel[35] src_le_temp; 
    // src_le_temp[0] is ignored ;

 // ::pel[35] src_up_temp; 
    // src_up_temp[0] is ignored ;

 // ::u16 avail_tb; 
    // avail_tb is ignored ;

 // ::u8[5] rdo_list; 
    (rdo_list[0])++;

 // ::u8 inter_rdo_num; 
    // inter_rdo_num is ignored ;

 // ::RDOQ_ARRAY* rdoq_array; 
    // rdoq_array[0] . rdoq_est_run[0][0] is ignored ;
    // rdoq_array[0] . rdoq_est_cbf[0][0] is ignored ;
    // rdoq_array[0] . rdoq_est_level[0][0] is ignored ;
    // rdoq_array[0] . rdoq_est_last[0][0][0][0] is ignored ;

 // ::MD_FW* md_fw_ptr; 
    // md_fw_ptr[0] . pic_width is ignored ;
    // md_fw_ptr[0] . pic_height is ignored ;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . slice_qp is ignored ;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    // md_fw_ptr[0] . pic_width_in_scu is ignored ;
    // md_fw_ptr[0] . pic_height_in_scu is ignored ;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    // md_fw_ptr[0] . mipf_enable_flag is ignored ;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    // md_fw_ptr[0] . refp_ptr[0][0] is ignored ;
    // md_fw_ptr[0] . num_refp[0] is ignored ;
    // md_fw_ptr[0] . cur_ptr is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cr is ignored ;

 // ::U2 tree_status; 
    // tree_status is ignored ;

 // struct ::MD_COM_MODE_BEST_16{U11 x_scu;U11 y_scu;U4 cud;U7 cu_width;U7 cu_height;U3 cu_width_log2;U3 cu_height_log2;U13 x_pos;U13 y_pos;U23 scup;U2 cu_mode;int pb_part;int tb_part;int num_nz_y[4];S3 refi[2];U8 mvr_idx;U8 umve_flag;U8 umve_idx;U8 skip_idx;U8 mvp_from_hmvp_flag;s16 mvd[2][2];S14 mv[2][2];U8 affine_flag;CPMV affine_mv[2][4][2];s16 affine_mvd[2][4][2];U8 smvd_flag;u8 mpm[4][2];S8 ipm[4][2];U8 ipf_flag;U1 chroma_motion;U13 x_luma;U13 y_luma;U12 x_chroma;U12 y_chroma;BOOL last_child;}* best_info_ptr_16; 
    // best_info_ptr_16[0] . x_scu is ignored ;
    // best_info_ptr_16[0] . y_scu is ignored ;
    // best_info_ptr_16[0] . cud is ignored ;
    // best_info_ptr_16[0] . cu_width is ignored ;
    // best_info_ptr_16[0] . cu_height is ignored ;
    // best_info_ptr_16[0] . cu_width_log2 is ignored ;
    // best_info_ptr_16[0] . cu_height_log2 is ignored ;
    // best_info_ptr_16[0] . x_pos is ignored ;
    // best_info_ptr_16[0] . y_pos is ignored ;
    // best_info_ptr_16[0] . scup is ignored ;
    best_info_ptr_16[0] . cu_mode=dummy;
    // best_info_ptr_16[0] . pb_part is ignored ;
    // best_info_ptr_16[0] . tb_part is ignored ;
    best_info_ptr_16[0].num_nz_y[0]=dummy;
    best_info_ptr_16[0].num_nz_y[1]=dummy;
    best_info_ptr_16[0].num_nz_y[2]=dummy;
    best_info_ptr_16[0].num_nz_y[3]=dummy;
    // best_info_ptr_16[0] . refi[0] is ignored ;
    // best_info_ptr_16[0] . mvr_idx is ignored ;
    // best_info_ptr_16[0] . umve_flag is ignored ;
    // best_info_ptr_16[0] . umve_idx is ignored ;
    // best_info_ptr_16[0] . skip_idx is ignored ;
    // best_info_ptr_16[0] . mvp_from_hmvp_flag is ignored ;
    // best_info_ptr_16[0] . mvd[0][0] is ignored ;
    // best_info_ptr_16[0] . mv[0][0] is ignored ;
    best_info_ptr_16[0] . affine_flag=dummy;
    // best_info_ptr_16[0] . affine_mv[0][0][0] is ignored ;
    // best_info_ptr_16[0] . affine_mvd[0][0][0] is ignored ;
    // best_info_ptr_16[0] . smvd_flag is ignored ;
    (best_info_ptr_16[0] . mpm[0][0])=(best_info_ptr_16[0] . mpm[0][1]/*TODO:*/)=(best_info_ptr_16[0] . mpm[0][0])+(best_info_ptr_16[0] . mpm[0][1]/*TODO:*/);
    (best_info_ptr_16[0] . ipm[0][0])=(best_info_ptr_16[0] . ipm[0][1]/*TODO:*/)=(best_info_ptr_16[0] . ipm[0][0])+(best_info_ptr_16[0] . ipm[0][1]/*TODO:*/);
    // best_info_ptr_16[0] . ipf_flag is ignored ;
    // best_info_ptr_16[0] . chroma_motion is ignored ;
    // best_info_ptr_16[0] . x_luma is ignored ;
    // best_info_ptr_16[0] . y_luma is ignored ;
    // best_info_ptr_16[0] . x_chroma is ignored ;
    // best_info_ptr_16[0] . y_chroma is ignored ;
    // best_info_ptr_16[0] . last_child is ignored ;

 // ::MD_INPUT* md_input_ptr; 
    // md_input_ptr[0] . pix_x is ignored ;
    // md_input_ptr[0] . pix_y is ignored ;
    // md_input_ptr[0] . lambda_mv is ignored ;
    // md_input_ptr[0] . lambda[0] is ignored ;
    // md_input_ptr[0] . lambda_y is ignored ;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    // md_input_ptr[0] . qp_y is ignored ;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0][0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;
    // md_input_ptr[0] . cu_dqp_enable is ignored ;
    // md_input_ptr[0] . cu_qp_group_area_size is ignored ;
    // md_input_ptr[0] . cu_qp_group_pred_qp is ignored ;
    // md_input_ptr[0] . cu_luma_qp[0] is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cr is ignored ;
    // md_input_ptr[0] . md_slice_qp is ignored ;
    // md_input_ptr[0] . CuQPMap[0] is ignored ;

 // ::MV_DIR* mv_dir; 
    // mv_dir[0] . lidx is ignored ;
    // mv_dir[0] . cur_refi is ignored ;

 // ::strFetch_ref_window_16* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . code_stru is ignored ;
    // Fetch_Ref_window_ptr[0] . lidx[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . cur_refi[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0] is ignored ;

 // ::SKIP_ELEMENT[12] skip_mode; 
    // skip_mode[0] . valid is ignored ;
    // skip_mode[0] . skip_flag is ignored ;
    // skip_mode[0] . skip_idx is ignored ;
    // skip_mode[0] . umve_flag is ignored ;
    // skip_mode[0] . umve_idx is ignored ;
    // skip_mode[0] . affine_flag is ignored ;
    // skip_mode[0] . mvp[0][0] is ignored ;
    // skip_mode[0] . refi[0] is ignored ;

 // ::MERGE_RDO* merge_rdo; 
    // merge_rdo[0] . mrg_list_refi[0][0] is ignored ;
    // merge_rdo[0] . mrg_list_cp_mv[0][0][0][0] is ignored ;
    // merge_rdo[0] . mrg_list_cp_num[0] is ignored ;
    // merge_rdo[0] . num_cands is ignored ;
    // merge_rdo[0] . valid[0] is ignored ;

 // ::SKIP_RDO* skip_rdo; 
    // skip_rdo[0] . pmv_cands[0][0][0] is ignored ;
    // skip_rdo[0] . refi_cands[0][0] is ignored ;
    // skip_rdo[0] . mode_list[0] is ignored ;
    // skip_rdo[0] . num_rdo is ignored ;

 // ::SKIP_ELEMENT[12] skip_rdo_mode; 
    // skip_rdo_mode[0] . valid is ignored ;
    // skip_rdo_mode[0] . skip_flag is ignored ;
    // skip_rdo_mode[0] . skip_idx is ignored ;
    // skip_rdo_mode[0] . umve_flag is ignored ;
    // skip_rdo_mode[0] . umve_idx is ignored ;
    // skip_rdo_mode[0] . affine_flag is ignored ;
    // skip_rdo_mode[0] . mvp[0][0] is ignored ;
    // skip_rdo_mode[0] . refi[0] is ignored ;

 // ::S14[2] sub_pel_mv; 
    // sub_pel_mv[0] is ignored ;

 // ::s16[2][5][2] mvp_all; 
    // mvp_all[0][0][0] is ignored ;

 // ::U3 cu_width_log2; 
    // cu_width_log2 is ignored ;

 // ::U3 cu_height_log2; 
    // cu_height_log2 is ignored ;

 // struct ::ENC_CU_DATA_ARRAY_SIZE < 16 > {U8 split_mode[5][7][256];S14 mv[16][2][2];S16 mvd[16][2][2];int pb_part[16];int tb_part[16];U3 pred_mode[16];U8 mvr_idx[16];U8 umve_flag[16];U8 umve_idx[16];U8 skip_idx[16];u32 map_scu[16];U8 affine_flag[16];U8 smvd_flag[16];U32 map_cu_mode[16];U32 map_pb_tb_part[16];U4 depth[16];U8 ipf_flag[16];U8 mvp_from_hmvp_flag[16];U8 inter_filter_flag[16];U8 mpm[2][16];U8 mpm_ext[8][16];S8 ipm[2][16];S3 refi[16][2];int num_nz_coef[3][16];S16 coef_y[16][16];pel reco_y[16][16];U12 x_chroma;U12 y_chroma;U12 x_last;U12 y_last;BOOL last_child;int cu_qp_group_pred_qp;U8 cu_luma_qp[16];}* cu_data_temp_ptr_16; 
    // cu_data_temp_ptr_16[0] . split_mode[0][0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . mv[0][0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . mvd[0][0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . pb_part[0] is ignored ;
    // cu_data_temp_ptr_16[0] . tb_part[0] is ignored ;
    // cu_data_temp_ptr_16[0] . pred_mode[0] is ignored ;
    // cu_data_temp_ptr_16[0] . mvr_idx[0] is ignored ;
    // cu_data_temp_ptr_16[0] . umve_flag[0] is ignored ;
    // cu_data_temp_ptr_16[0] . umve_idx[0] is ignored ;
    // cu_data_temp_ptr_16[0] . skip_idx[0] is ignored ;
    // cu_data_temp_ptr_16[0] . map_scu[0] is ignored ;
    // cu_data_temp_ptr_16[0] . affine_flag[0] is ignored ;
    // cu_data_temp_ptr_16[0] . smvd_flag[0] is ignored ;
    // cu_data_temp_ptr_16[0] . map_cu_mode[0] is ignored ;
    // cu_data_temp_ptr_16[0] . map_pb_tb_part[0] is ignored ;
    // cu_data_temp_ptr_16[0] . depth[0] is ignored ;
    // cu_data_temp_ptr_16[0] . ipf_flag[0] is ignored ;
    // cu_data_temp_ptr_16[0] . mvp_from_hmvp_flag[0] is ignored ;
    // cu_data_temp_ptr_16[0] . inter_filter_flag[0] is ignored ;
    // cu_data_temp_ptr_16[0] . mpm[0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . mpm_ext[0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . ipm[0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . refi[0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . num_nz_coef[0][0] is ignored ;
    (cu_data_temp_ptr_16[0] . coef_y[0][0])=(cu_data_temp_ptr_16[0] . coef_y[0][1]/*TODO:*/)=(cu_data_temp_ptr_16[0] . coef_y[0][0])+(cu_data_temp_ptr_16[0] . coef_y[0][1]/*TODO:*/);
    (cu_data_temp_ptr_16[0] . reco_y[0][0])=(cu_data_temp_ptr_16[0] . reco_y[0][1]/*TODO:*/)=(cu_data_temp_ptr_16[0] . reco_y[0][0])+(cu_data_temp_ptr_16[0] . reco_y[0][1]/*TODO:*/);
    // cu_data_temp_ptr_16[0] . x_chroma is ignored ;
    // cu_data_temp_ptr_16[0] . y_chroma is ignored ;
    // cu_data_temp_ptr_16[0] . x_last is ignored ;
    // cu_data_temp_ptr_16[0] . y_last is ignored ;
    // cu_data_temp_ptr_16[0] . last_child is ignored ;
    // cu_data_temp_ptr_16[0] . cu_qp_group_pred_qp is ignored ;
    // cu_data_temp_ptr_16[0] . cu_luma_qp[0] is ignored ;

 // ::U2 curr_cons_pred_mode; 
    // curr_cons_pred_mode is ignored ;

 // ::S14[2][2] mv_copy; 
    // mv_copy[0][0] is ignored ;

 // ::s16[2][2] mvd_copy; 
    // mvd_copy[0][0] is ignored ;

 // ::S3[2] refi_copy; 
    // refi_copy[0] is ignored ;

 // ::pel[16][16] pred_y_inter1; 
    // pred_y_inter1[0][0] is ignored ;

 // ::pel[2][16][16] pred_y_inter2; 
    // pred_y_inter2[0][0][0] is ignored ;

 // ::pel[5][16][16] pred_y_intra; 
    (pred_y_intra[0][0][0])++;
    (pred_y_intra[0][0][1])++;
    (pred_y_intra[0][0][2])++;
    (pred_y_intra[0][0][3])++;
    (pred_y_intra[0][0][4])++;
    (pred_y_intra[0][0][5])++;
    (pred_y_intra[0][0][6])++;
    (pred_y_intra[0][0][7])++;
    (pred_y_intra[0][0][8])++;
    (pred_y_intra[0][0][9])++;
    (pred_y_intra[0][0][10])++;
    (pred_y_intra[0][0][11])++;
    (pred_y_intra[0][0][12])++;
    (pred_y_intra[0][0][13])++;
    (pred_y_intra[0][0][14])++;
    (pred_y_intra[0][0][15])++;

 // struct ::MD_KERNEL_INPUT{U13 x_pos;U13 y_pos;U12 x_chroma;U12 y_chroma;U1 chroma_motion;U23 scup;U7 cu_width;U7 cu_height;U3 cu_width_log2;U3 cu_height_log2;}* md_kernel_input; 
    // md_kernel_input[0] . x_pos is ignored ;
    // md_kernel_input[0] . y_pos is ignored ;
    // md_kernel_input[0] . x_chroma is ignored ;
    // md_kernel_input[0] . y_chroma is ignored ;
    // md_kernel_input[0] . chroma_motion is ignored ;
    // md_kernel_input[0] . scup is ignored ;
    // md_kernel_input[0] . cu_width is ignored ;
    // md_kernel_input[0] . cu_height is ignored ;
    // md_kernel_input[0] . cu_width_log2 is ignored ;
    // md_kernel_input[0] . cu_height_log2 is ignored ;

 // ::u8[4][2] mpm; 
    dummy+=mpm[0][0];
    dummy+=mpm[0][1];
    dummy+=mpm[1][0];
    dummy+=mpm[1][1];
    dummy+=mpm[2][0];
    dummy+=mpm[2][1];
    dummy+=mpm[3][0];
    dummy+=mpm[3][1];

 // ::U7 cu_width; 
    // cu_width is ignored ;

 // int refp_ptr0; 
    // refp_ptr0 is ignored ;

 // int refp_ptr1; 
    // refp_ptr1 is ignored ;

 // ::U2 ch_type; 
    dummy+=ch_type;

 // ::s64 lambda; 
    // lambda is ignored ;

 // ::s64 cost_skip; 
    dummy+=cost_skip;


 // ::s64[1] cost_best; 
    (cost_best[0])++;

 // ::U6 qp; 
    dummy+=qp;

 // ::CORE_SBTMVP* core_sbtmvp; 
    // core_sbtmvp[0] . sbTmvp_flag is ignored ;
    // core_sbtmvp[0] . best_sbTmvp_flag is ignored ;
    // core_sbtmvp[0] . sbTmvp[0] . mv[0][0] is ignored ;
    // core_sbtmvp[0] . sbTmvp[0] . ref_idx[0] is ignored ;
    // core_sbtmvp[0] . best_sbTmvp[0] . mv[0][0] is ignored ;
    // core_sbtmvp[0] . best_sbTmvp[0] . ref_idx[0] is ignored ;
    // core_sbtmvp[0] . isValid is ignored ;

#pragma HLS resource variable=best_info_ptr_16[0].ipm core=RAM_T2P_BRAM 
#pragma HLS resource variable=best_info_ptr_16[0].mpm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cost_best core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].coef_y core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].reco_y core=RAM_T2P_BRAM 
#pragma HLS resource variable=ipred_list core=RAM_T2P_BRAM 
#pragma HLS resource variable=orgY core=RAM_1P_BRAM 
#pragma HLS resource variable=pred_y_intra core=RAM_1P_BRAM 
#pragma HLS resource variable=rdo_list core=RAM_1P_BRAM 
#pragma HLS array_partition variable=best_info_ptr_16[0].num_nz_y complete dim=1
#pragma HLS array_partition variable=mpm complete dim=1
#pragma HLS array_partition variable=mpm complete dim=2
#pragma HLS array_partition variable=orgY cyclic factor=16 dim=1
#pragma HLS array_partition variable=pred_y_intra cyclic factor=16 dim=3

}
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
)
{
    s64 dummy = 0;
	// dummy_template.cpp
}
#endif // LUMA_16x8_PIPELINE_DUMMY

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
)
{
    s64 dummy = 0;
	// dummy_template.cpp
}
#endif // LUMA_8x16_PIPELINE_DUMMY

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
	s64 *cost_best)
{
	//	paste code begin

	int dummy = 0;


	// ::U8 w; 
	dummy += w;

	// ::U8 h; 
	dummy += h;

	// ::RDOQ_MODEL[128] model_cnt; 
	 //  (model_cnt[0] . cbf[0])=(model_cnt[0] . cbf[0])+(model_cnt[0] . cbf[1]/*TODO:*/);
	   // model_cnt[0] . run_rdoq[0] is ignored ;
	   // model_cnt[0] . level[0] is ignored ;
	   // model_cnt[0] . last1[0] is ignored ;
	   // model_cnt[0] . last2[0] is ignored ;

	// ::pel[1024] orgY;
	int loop = dummy;
	for (int i = 0; i < loop; i++) {
		dummy += orgY_32[i][0] + orgY_32[i][1];
	}
	//dummy+=(orgY[0])+(orgY[1]/*TODO:*/);
	//dummy+=(orgY[1])+(orgY[1]);
	//dummy+=(orgY[2])+(orgY[2]);
	//dummy+=(orgY[3])+(orgY[3]);
	//dummy+=(orgY[4])+(orgY[4]);
	//dummy+=(orgY[5])+(orgY[5]);
	//dummy+=(orgY[6])+(orgY[6]);
	//dummy+=(orgY[7])+(orgY[7]);
	//dummy+=(orgY[8])+(orgY[8]);
	//dummy+=(orgY[9])+(orgY[9]);
	//dummy+=(orgY[10])+(orgY[10]);
	//dummy+=(orgY[11])+(orgY[11]);
	//dummy+=(orgY[12])+(orgY[12]);
	//dummy+=(orgY[13])+(orgY[13]);
	//dummy+=(orgY[14])+(orgY[14]);
	//dummy+=(orgY[15])+(orgY[15]);
	//dummy+=(orgY[16])+(orgY[16]);
	//dummy+=(orgY[17])+(orgY[17]);
	//dummy+=(orgY[18])+(orgY[18]);
	//dummy+=(orgY[19])+(orgY[19]);
	//dummy+=(orgY[20])+(orgY[20]);
	//dummy+=(orgY[21])+(orgY[21]);
	//dummy+=(orgY[22])+(orgY[22]);
	//dummy+=(orgY[23])+(orgY[23]);
	//dummy+=(orgY[24])+(orgY[24]);
	//dummy+=(orgY[25])+(orgY[25]);
	//dummy+=(orgY[26])+(orgY[26]);
	//dummy+=(orgY[27])+(orgY[27]);
	//dummy+=(orgY[28])+(orgY[28]);
	//dummy+=(orgY[29])+(orgY[29]);
	//dummy+=(orgY[30])+(orgY[30]);
	//dummy+=(orgY[31])+(orgY[31]);
	//dummy+=(orgY[32])+(orgY[32]);
	//dummy+=(orgY[33])+(orgY[33]);
	//dummy+=(orgY[34])+(orgY[34]);
	//dummy+=(orgY[35])+(orgY[35]);
	//dummy+=(orgY[36])+(orgY[36]);
	//dummy+=(orgY[37])+(orgY[37]);
	//dummy+=(orgY[38])+(orgY[38]);
	//dummy+=(orgY[39])+(orgY[39]);
	//dummy+=(orgY[40])+(orgY[40]);
	//dummy+=(orgY[41])+(orgY[41]);
	//dummy+=(orgY[42])+(orgY[42]);
	//dummy+=(orgY[43])+(orgY[43]);
	//dummy+=(orgY[44])+(orgY[44]);
	//dummy+=(orgY[45])+(orgY[45]);
	//dummy+=(orgY[46])+(orgY[46]);
	//dummy+=(orgY[47])+(orgY[47]);
	//dummy+=(orgY[48])+(orgY[48]);
	//dummy+=(orgY[49])+(orgY[49]);
	//dummy+=(orgY[50])+(orgY[50]);
	//dummy+=(orgY[51])+(orgY[51]);
	//dummy+=(orgY[52])+(orgY[52]);
	//dummy+=(orgY[53])+(orgY[53]);
	//dummy+=(orgY[54])+(orgY[54]);
	//dummy+=(orgY[55])+(orgY[55]);
	//dummy+=(orgY[56])+(orgY[56]);
	//dummy+=(orgY[57])+(orgY[57]);
	//dummy+=(orgY[58])+(orgY[58]);
	//dummy+=(orgY[59])+(orgY[59]);
	//dummy+=(orgY[60])+(orgY[60]);
	//dummy+=(orgY[61])+(orgY[61]);
	//dummy+=(orgY[62])+(orgY[62]);
	//dummy+=(orgY[63])+(orgY[63]);
	//dummy+=(orgY[64])+(orgY[64]);
	//dummy+=(orgY[65])+(orgY[65]);
	//dummy+=(orgY[66])+(orgY[66]);
	//dummy+=(orgY[67])+(orgY[67]);
	//dummy+=(orgY[68])+(orgY[68]);
	//dummy+=(orgY[69])+(orgY[69]);
	//dummy+=(orgY[70])+(orgY[70]);
	//dummy+=(orgY[71])+(orgY[71]);
	//dummy+=(orgY[72])+(orgY[72]);
	//dummy+=(orgY[73])+(orgY[73]);
	//dummy+=(orgY[74])+(orgY[74]);
	//dummy+=(orgY[75])+(orgY[75]);
	//dummy+=(orgY[76])+(orgY[76]);
	//dummy+=(orgY[77])+(orgY[77]);
	//dummy+=(orgY[78])+(orgY[78]);
	//dummy+=(orgY[79])+(orgY[79]);
	//dummy+=(orgY[80])+(orgY[80]);
	//dummy+=(orgY[81])+(orgY[81]);
	//dummy+=(orgY[82])+(orgY[82]);
	//dummy+=(orgY[83])+(orgY[83]);
	//dummy+=(orgY[84])+(orgY[84]);
	//dummy+=(orgY[85])+(orgY[85]);
	//dummy+=(orgY[86])+(orgY[86]);
	//dummy+=(orgY[87])+(orgY[87]);
	//dummy+=(orgY[88])+(orgY[88]);
	//dummy+=(orgY[89])+(orgY[89]);
	//dummy+=(orgY[90])+(orgY[90]);
	//dummy+=(orgY[91])+(orgY[91]);
	//dummy+=(orgY[92])+(orgY[92]);
	//dummy+=(orgY[93])+(orgY[93]);
	//dummy+=(orgY[94])+(orgY[94]);
	//dummy+=(orgY[95])+(orgY[95]);
	//dummy+=(orgY[96])+(orgY[96]);
	//dummy+=(orgY[97])+(orgY[97]);
	//dummy+=(orgY[98])+(orgY[98]);
	//dummy+=(orgY[99])+(orgY[99]);
	//dummy+=(orgY[100])+(orgY[100]);
	//dummy+=(orgY[101])+(orgY[101]);
	//dummy+=(orgY[102])+(orgY[102]);
	//dummy+=(orgY[103])+(orgY[103]);
	//dummy+=(orgY[104])+(orgY[104]);
	//dummy+=(orgY[105])+(orgY[105]);
	//dummy+=(orgY[106])+(orgY[106]);
	//dummy+=(orgY[107])+(orgY[107]);
	//dummy+=(orgY[108])+(orgY[108]);
	//dummy+=(orgY[109])+(orgY[109]);
	//dummy+=(orgY[110])+(orgY[110]);
	//dummy+=(orgY[111])+(orgY[111]);
	//dummy+=(orgY[112])+(orgY[112]);
	//dummy+=(orgY[113])+(orgY[113]);
	//dummy+=(orgY[114])+(orgY[114]);
	//dummy+=(orgY[115])+(orgY[115]);
	//dummy+=(orgY[116])+(orgY[116]);
	//dummy+=(orgY[117])+(orgY[117]);
	//dummy+=(orgY[118])+(orgY[118]);
	//dummy+=(orgY[119])+(orgY[119]);
	//dummy+=(orgY[120])+(orgY[120]);
	//dummy+=(orgY[121])+(orgY[121]);
	//dummy+=(orgY[122])+(orgY[122]);
	//dummy+=(orgY[123])+(orgY[123]);
	//dummy+=(orgY[124])+(orgY[124]);
	//dummy+=(orgY[125])+(orgY[125]);
	//dummy+=(orgY[126])+(orgY[126]);
	//dummy+=(orgY[127])+(orgY[127]);
	//dummy+=(orgY[128])+(orgY[128]);
	//dummy+=(orgY[129])+(orgY[129]);
	//dummy+=(orgY[130])+(orgY[130]);
	//dummy+=(orgY[131])+(orgY[131]);
	//dummy+=(orgY[132])+(orgY[132]);
	//dummy+=(orgY[133])+(orgY[133]);
	//dummy+=(orgY[134])+(orgY[134]);
	//dummy+=(orgY[135])+(orgY[135]);
	//dummy+=(orgY[136])+(orgY[136]);
	//dummy+=(orgY[137])+(orgY[137]);
	//dummy+=(orgY[138])+(orgY[138]);
	//dummy+=(orgY[139])+(orgY[139]);
	//dummy+=(orgY[140])+(orgY[140]);
	//dummy+=(orgY[141])+(orgY[141]);
	//dummy+=(orgY[142])+(orgY[142]);
	//dummy+=(orgY[143])+(orgY[143]);
	//dummy+=(orgY[144])+(orgY[144]);
	//dummy+=(orgY[145])+(orgY[145]);
	//dummy+=(orgY[146])+(orgY[146]);
	//dummy+=(orgY[147])+(orgY[147]);
	//dummy+=(orgY[148])+(orgY[148]);
	//dummy+=(orgY[149])+(orgY[149]);
	//dummy+=(orgY[150])+(orgY[150]);
	//dummy+=(orgY[151])+(orgY[151]);
	//dummy+=(orgY[152])+(orgY[152]);
	//dummy+=(orgY[153])+(orgY[153]);
	//dummy+=(orgY[154])+(orgY[154]);
	//dummy+=(orgY[155])+(orgY[155]);
	//dummy+=(orgY[156])+(orgY[156]);
	//dummy+=(orgY[157])+(orgY[157]);
	//dummy+=(orgY[158])+(orgY[158]);
	//dummy+=(orgY[159])+(orgY[159]);
	//dummy+=(orgY[160])+(orgY[160]);
	//dummy+=(orgY[161])+(orgY[161]);
	//dummy+=(orgY[162])+(orgY[162]);
	//dummy+=(orgY[163])+(orgY[163]);
	//dummy+=(orgY[164])+(orgY[164]);
	//dummy+=(orgY[165])+(orgY[165]);
	//dummy+=(orgY[166])+(orgY[166]);
	//dummy+=(orgY[167])+(orgY[167]);
	//dummy+=(orgY[168])+(orgY[168]);
	//dummy+=(orgY[169])+(orgY[169]);
	//dummy+=(orgY[170])+(orgY[170]);
	//dummy+=(orgY[171])+(orgY[171]);
	//dummy+=(orgY[172])+(orgY[172]);
	//dummy+=(orgY[173])+(orgY[173]);
	//dummy+=(orgY[174])+(orgY[174]);
	//dummy+=(orgY[175])+(orgY[175]);
	//dummy+=(orgY[176])+(orgY[176]);
	//dummy+=(orgY[177])+(orgY[177]);
	//dummy+=(orgY[178])+(orgY[178]);
	//dummy+=(orgY[179])+(orgY[179]);
	//dummy+=(orgY[180])+(orgY[180]);
	//dummy+=(orgY[181])+(orgY[181]);
	//dummy+=(orgY[182])+(orgY[182]);
	//dummy+=(orgY[183])+(orgY[183]);
	//dummy+=(orgY[184])+(orgY[184]);
	//dummy+=(orgY[185])+(orgY[185]);
	//dummy+=(orgY[186])+(orgY[186]);
	//dummy+=(orgY[187])+(orgY[187]);
	//dummy+=(orgY[188])+(orgY[188]);
	//dummy+=(orgY[189])+(orgY[189]);
	//dummy+=(orgY[190])+(orgY[190]);
	//dummy+=(orgY[191])+(orgY[191]);
	//dummy+=(orgY[192])+(orgY[192]);
	//dummy+=(orgY[193])+(orgY[193]);
	//dummy+=(orgY[194])+(orgY[194]);
	//dummy+=(orgY[195])+(orgY[195]);
	//dummy+=(orgY[196])+(orgY[196]);
	//dummy+=(orgY[197])+(orgY[197]);
	//dummy+=(orgY[198])+(orgY[198]);
	//dummy+=(orgY[199])+(orgY[199]);
	//dummy+=(orgY[200])+(orgY[200]);
	//dummy+=(orgY[201])+(orgY[201]);
	//dummy+=(orgY[202])+(orgY[202]);
	//dummy+=(orgY[203])+(orgY[203]);
	//dummy+=(orgY[204])+(orgY[204]);
	//dummy+=(orgY[205])+(orgY[205]);
	//dummy+=(orgY[206])+(orgY[206]);
	//dummy+=(orgY[207])+(orgY[207]);
	//dummy+=(orgY[208])+(orgY[208]);
	//dummy+=(orgY[209])+(orgY[209]);
	//dummy+=(orgY[210])+(orgY[210]);
	//dummy+=(orgY[211])+(orgY[211]);
	//dummy+=(orgY[212])+(orgY[212]);
	//dummy+=(orgY[213])+(orgY[213]);
	//dummy+=(orgY[214])+(orgY[214]);
	//dummy+=(orgY[215])+(orgY[215]);
	//dummy+=(orgY[216])+(orgY[216]);
	//dummy+=(orgY[217])+(orgY[217]);
	//dummy+=(orgY[218])+(orgY[218]);
	//dummy+=(orgY[219])+(orgY[219]);
	//dummy+=(orgY[220])+(orgY[220]);
	//dummy+=(orgY[221])+(orgY[221]);
	//dummy+=(orgY[222])+(orgY[222]);
	//dummy+=(orgY[223])+(orgY[223]);
	//dummy+=(orgY[224])+(orgY[224]);
	//dummy+=(orgY[225])+(orgY[225]);
	//dummy+=(orgY[226])+(orgY[226]);
	//dummy+=(orgY[227])+(orgY[227]);
	//dummy+=(orgY[228])+(orgY[228]);
	//dummy+=(orgY[229])+(orgY[229]);
	//dummy+=(orgY[230])+(orgY[230]);
	//dummy+=(orgY[231])+(orgY[231]);
	//dummy+=(orgY[232])+(orgY[232]);
	//dummy+=(orgY[233])+(orgY[233]);
	//dummy+=(orgY[234])+(orgY[234]);
	//dummy+=(orgY[235])+(orgY[235]);
	//dummy+=(orgY[236])+(orgY[236]);
	//dummy+=(orgY[237])+(orgY[237]);
	//dummy+=(orgY[238])+(orgY[238]);
	//dummy+=(orgY[239])+(orgY[239]);
	//dummy+=(orgY[240])+(orgY[240]);
	//dummy+=(orgY[241])+(orgY[241]);
	//dummy+=(orgY[242])+(orgY[242]);
	//dummy+=(orgY[243])+(orgY[243]);
	//dummy+=(orgY[244])+(orgY[244]);
	//dummy+=(orgY[245])+(orgY[245]);
	//dummy+=(orgY[246])+(orgY[246]);
	//dummy+=(orgY[247])+(orgY[247]);
	//dummy+=(orgY[248])+(orgY[248]);
	//dummy+=(orgY[249])+(orgY[249]);
	//dummy+=(orgY[250])+(orgY[250]);
	//dummy+=(orgY[251])+(orgY[251]);
	//dummy+=(orgY[252])+(orgY[252]);
	//dummy+=(orgY[253])+(orgY[253]);
	//dummy+=(orgY[254])+(orgY[254]);
	//dummy+=(orgY[255])+(orgY[255]);

 // ::U1[17] valid_mode; 
	dummy += valid_mode[0];

	// ::u8[5] ipred_list; 
	   // ipred_list[0] is ignored ;

	// ::pel[67] src_le_temp; 
	dummy += (src_le_temp[0]) + (src_le_temp[1]/*TODO:*/);

	// ::pel[67] src_up_temp; 
	dummy += (src_up_temp[0]) + (src_up_temp[1]/*TODO:*/);

	// ::u16 avail_tb; 
	dummy += avail_tb;

	// ::u8[5] rdo_list; 
	dummy += rdo_list[0];

	// ::u8 inter_rdo_num; 
	   // inter_rdo_num is ignored ;

	// ::MD_COM_MODE_BEST[1] best_info_ptr; 
	   // best_info_ptr[0] . x_scu is ignored ;
	   // best_info_ptr[0] . y_scu is ignored ;
	   // best_info_ptr[0] . cud is ignored ;
	   // best_info_ptr[0] . cu_width is ignored ;
	   // best_info_ptr[0] . cu_height is ignored ;
	   // best_info_ptr[0] . cu_width_log2 is ignored ;
	   // best_info_ptr[0] . cu_height_log2 is ignored ;
	   // best_info_ptr[0] . x_pos is ignored ;
	   // best_info_ptr[0] . y_pos is ignored ;
	   // best_info_ptr[0] . scup is ignored ;
	best_info_ptr[0].cu_mode = dummy;
	best_info_ptr[0].pb_part = dummy;
	best_info_ptr[0].tb_part = dummy;
	// best_info_ptr[0] . rec_y[0] is ignored ;
	// best_info_ptr[0] . rec_u[0] is ignored ;
	// best_info_ptr[0] . rec_v[0] is ignored ;
	// best_info_ptr[0] . coef_y[0] is ignored ;
	// best_info_ptr[0] . coef_u[0] is ignored ;
	// best_info_ptr[0] . coef_v[0] is ignored ;
	// best_info_ptr[0] . pred_y[0] is ignored ;
	// best_info_ptr[0] . pred_u[0] is ignored ;
	// best_info_ptr[0] . pred_v[0] is ignored ;
	best_info_ptr[0].num_nz[0][0] = dummy;
	best_info_ptr[0].num_nz[0][1] = dummy;
	best_info_ptr[0].num_nz[0][2] = dummy;
	best_info_ptr[0].num_nz[1][0] = dummy;
	best_info_ptr[0].num_nz[1][1] = dummy;
	best_info_ptr[0].num_nz[1][2] = dummy;
	best_info_ptr[0].num_nz[2][0] = dummy;
	best_info_ptr[0].num_nz[2][1] = dummy;
	best_info_ptr[0].num_nz[2][2] = dummy;
	best_info_ptr[0].num_nz[3][0] = dummy;
	best_info_ptr[0].num_nz[3][1] = dummy;
	best_info_ptr[0].num_nz[3][2] = dummy;
	(best_info_ptr[0].refi[0]) = (best_info_ptr[0].refi[1]/*TODO:*/) = dummy;
	best_info_ptr[0].mvr_idx = dummy;
	best_info_ptr[0].umve_flag = dummy;
	best_info_ptr[0].umve_idx = dummy;
	best_info_ptr[0].skip_idx = dummy;
	best_info_ptr[0].mvp_from_hmvp_flag = dummy;
	best_info_ptr[0].mvd[0][0] = dummy;
	best_info_ptr[0].mvd[0][1] = dummy;
	best_info_ptr[0].mvd[1][0] = dummy;
	best_info_ptr[0].mvd[1][1] = dummy;
	best_info_ptr[0].mv[0][0] = dummy;
	best_info_ptr[0].mv[0][1] = dummy;
	best_info_ptr[0].mv[1][0] = dummy;
	best_info_ptr[0].mv[1][1] = dummy;
	best_info_ptr[0].affine_flag = dummy;
	best_info_ptr[0].affine_mv[0][0][0] = dummy;
	best_info_ptr[0].affine_mv[0][0][1] = dummy;
	best_info_ptr[0].affine_mv[0][1][0] = dummy;
	best_info_ptr[0].affine_mv[0][1][1] = dummy;
	best_info_ptr[0].affine_mv[0][2][0] = dummy;
	best_info_ptr[0].affine_mv[0][2][1] = dummy;
	best_info_ptr[0].affine_mv[0][3][0] = dummy;
	best_info_ptr[0].affine_mv[0][3][1] = dummy;
	best_info_ptr[0].affine_mv[1][0][0] = dummy;
	best_info_ptr[0].affine_mv[1][0][1] = dummy;
	best_info_ptr[0].affine_mv[1][1][0] = dummy;
	best_info_ptr[0].affine_mv[1][1][1] = dummy;
	best_info_ptr[0].affine_mv[1][2][0] = dummy;
	best_info_ptr[0].affine_mv[1][2][1] = dummy;
	best_info_ptr[0].affine_mv[1][3][0] = dummy;
	best_info_ptr[0].affine_mv[1][3][1] = dummy;
	best_info_ptr[0].affine_mvd[0][0][0] = dummy;
	best_info_ptr[0].affine_mvd[0][0][1] = dummy;
	best_info_ptr[0].affine_mvd[0][1][0] = dummy;
	best_info_ptr[0].affine_mvd[0][1][1] = dummy;
	best_info_ptr[0].affine_mvd[0][2][0] = dummy;
	best_info_ptr[0].affine_mvd[0][2][1] = dummy;
	best_info_ptr[0].affine_mvd[0][3][0] = dummy;
	best_info_ptr[0].affine_mvd[0][3][1] = dummy;
	best_info_ptr[0].affine_mvd[1][0][0] = dummy;
	best_info_ptr[0].affine_mvd[1][0][1] = dummy;
	best_info_ptr[0].affine_mvd[1][1][0] = dummy;
	best_info_ptr[0].affine_mvd[1][1][1] = dummy;
	best_info_ptr[0].affine_mvd[1][2][0] = dummy;
	best_info_ptr[0].affine_mvd[1][2][1] = dummy;
	best_info_ptr[0].affine_mvd[1][3][0] = dummy;
	best_info_ptr[0].affine_mvd[1][3][1] = dummy;
	best_info_ptr[0].smvd_flag = dummy;
	best_info_ptr[0].mpm[0][0] = dummy;
	best_info_ptr[0].mpm[0][1] = dummy;
	best_info_ptr[0].mpm[1][0] = dummy;
	best_info_ptr[0].mpm[1][1] = dummy;
	best_info_ptr[0].mpm[2][0] = dummy;
	best_info_ptr[0].mpm[2][1] = dummy;
	best_info_ptr[0].mpm[3][0] = dummy;
	best_info_ptr[0].mpm[3][1] = dummy;
	best_info_ptr[0].ipm[0][0] = dummy;
	best_info_ptr[0].ipm[0][1] = dummy;
	best_info_ptr[0].ipm[1][0] = dummy;
	best_info_ptr[0].ipm[1][1] = dummy;
	best_info_ptr[0].ipm[2][0] = dummy;
	best_info_ptr[0].ipm[2][1] = dummy;
	best_info_ptr[0].ipm[3][0] = dummy;
	best_info_ptr[0].ipm[3][1] = dummy;
	// best_info_ptr[0] . ipf_flag is ignored ;
	// best_info_ptr[0] . chroma_motion is ignored ;
	// best_info_ptr[0] . x_luma is ignored ;
	// best_info_ptr[0] . y_luma is ignored ;
	// best_info_ptr[0] . x_chroma is ignored ;
	// best_info_ptr[0] . y_chroma is ignored ;
	// best_info_ptr[0] . last_child is ignored ;

 // ::MD_INPUT* md_input_ptr; 
	// md_input_ptr[0] . pix_x is ignored ;
	// md_input_ptr[0] . pix_y is ignored ;
	// md_input_ptr[0] . lambda_mv is ignored ;
	dummy += md_input_ptr[0].lambda[0];
#if !AZB_ENABLE0
	dummy += md_input_ptr[0].qp_y;
#endif
	// md_input_ptr[0] . qp_u is ignored ;
	// md_input_ptr[0] . qp_v is ignored ;
	// md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
	// md_input_ptr[0] . x_last_ctu is ignored ;
	// md_input_ptr[0] . y_last_ctu is ignored ;
	// md_input_ptr[0] . CtrPosFME[0][0][0] is ignored ;

 // ::pel[49152] RefWindowLCU; 
	dummy += Fetch_Ref_window_ptr->RefWindowLCU[0];

	// ::S14[2][2][2] CtrPos; 
	dummy += Fetch_Ref_window_ptr->CtrPos[0][0][0];
	dummy += Fetch_Ref_window_ptr->CtrPos[0][1][0];
	dummy += Fetch_Ref_window_ptr->CtrPos[1][0][0];
	dummy += Fetch_Ref_window_ptr->CtrPos[1][1][0];

	// ::MD_COM_MODE* mod_info_curr_ptr; 
	   // mod_info_curr_ptr[0] . x_scu is ignored ;
	   // mod_info_curr_ptr[0] . y_scu is ignored ;
	   // mod_info_curr_ptr[0] . cud is ignored ;
	   // mod_info_curr_ptr[0] . cu_width is ignored ;
	   // mod_info_curr_ptr[0] . cu_height is ignored ;
	   // mod_info_curr_ptr[0] . cu_width_log2 is ignored ;
	   // mod_info_curr_ptr[0] . cu_height_log2 is ignored ;
	   // mod_info_curr_ptr[0] . x_pos is ignored ;
	   // mod_info_curr_ptr[0] . y_pos is ignored ;
	   // mod_info_curr_ptr[0] . scup is ignored ;
	   // mod_info_curr_ptr[0] . cu_mode is ignored ;
	   // mod_info_curr_ptr[0] . pb_part is ignored ;
	   // mod_info_curr_ptr[0] . tb_part is ignored ;
	   // mod_info_curr_ptr[0] . num_nz[0][0] is ignored ;
	   // mod_info_curr_ptr[0] . refi[0] is ignored ;
	   // mod_info_curr_ptr[0] . mvr_idx is ignored ;
	   // mod_info_curr_ptr[0] . umve_flag is ignored ;
	   // mod_info_curr_ptr[0] . umve_idx is ignored ;
	   // mod_info_curr_ptr[0] . skip_idx is ignored ;
	   // mod_info_curr_ptr[0] . mvp_from_hmvp_flag is ignored ;
	   // mod_info_curr_ptr[0] . mvd[0][0] is ignored ;
	   // mod_info_curr_ptr[0] . mv[0][0] is ignored ;
	   // mod_info_curr_ptr[0] . affine_flag is ignored ;
	   // mod_info_curr_ptr[0] . affine_mv[0][0][0] is ignored ;
	   // mod_info_curr_ptr[0] . affine_mvd[0][0][0] is ignored ;
	   // mod_info_curr_ptr[0] . smvd_flag is ignored ;
	dummy += mod_info_curr_ptr[0].mpm[0][0];
	// mod_info_curr_ptr[0] . ipm[0][0] is ignored ;
	// mod_info_curr_ptr[0] . ipf_flag is ignored ;
	// mod_info_curr_ptr[0] . chroma_motion is ignored ;
	// mod_info_curr_ptr[0] . x_luma is ignored ;
	// mod_info_curr_ptr[0] . y_luma is ignored ;
	// mod_info_curr_ptr[0] . x_chroma is ignored ;
	// mod_info_curr_ptr[0] . y_chroma is ignored ;
	// mod_info_curr_ptr[0] . last_child is ignored ;

 // ::ENC_CU_DATA_ARRAY* cu_data_temp_ptr; 
	// cu_data_temp_ptr[0] . split_mode[0][0][0] is ignored ;
	// cu_data_temp_ptr[0] . mv[0][0][0] is ignored ;
	// cu_data_temp_ptr[0] . mvd[0][0][0] is ignored ;
	// cu_data_temp_ptr[0] . pb_part[0] is ignored ;
	// cu_data_temp_ptr[0] . tb_part[0] is ignored ;
	// cu_data_temp_ptr[0] . pred_mode[0] is ignored ;
	// cu_data_temp_ptr[0] . mvr_idx[0] is ignored ;
	// cu_data_temp_ptr[0] . umve_flag[0] is ignored ;
	// cu_data_temp_ptr[0] . umve_idx[0] is ignored ;
	// cu_data_temp_ptr[0] . skip_idx[0] is ignored ;
	// cu_data_temp_ptr[0] . map_scu[0] is ignored ;
	// cu_data_temp_ptr[0] . affine_flag[0] is ignored ;
	// cu_data_temp_ptr[0] . smvd_flag[0] is ignored ;
	// cu_data_temp_ptr[0] . map_cu_mode[0] is ignored ;
	// cu_data_temp_ptr[0] . map_pb_tb_part[0] is ignored ;
	// cu_data_temp_ptr[0] . depth[0] is ignored ;
	// cu_data_temp_ptr[0] . ipf_flag[0] is ignored ;
	// cu_data_temp_ptr[0] . mvp_from_hmvp_flag[0] is ignored ;
	// cu_data_temp_ptr[0] . mpm[0][0] is ignored ;
	// cu_data_temp_ptr[0] . mpm_ext[0][0] is ignored ;
	// cu_data_temp_ptr[0] . ipm[0][0] is ignored ;
	// cu_data_temp_ptr[0] . refi[0][0] is ignored ;
	// cu_data_temp_ptr[0] . num_nz_coef[0][0] is ignored ;
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			cu_data_temp_ptr[0].coef_y[(i << 6) + j] = dummy;
		}
	}
	// cu_data_temp_ptr[0] . coef_u[0] is ignored ;
	// cu_data_temp_ptr[0] . coef_v[0] is ignored ;
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			cu_data_temp_ptr[0].reco_y[(i << 6) + j] = dummy;
		}
	}
	// cu_data_temp_ptr[0] . reco_u[0] is ignored ;
	// cu_data_temp_ptr[0] . reco_v[0] is ignored ;
	// cu_data_temp_ptr[0] . x_chroma is ignored ;
	// cu_data_temp_ptr[0] . y_chroma is ignored ;
	// cu_data_temp_ptr[0] . x_last is ignored ;
	// cu_data_temp_ptr[0] . y_last is ignored ;
	// cu_data_temp_ptr[0] . last_child is ignored ;

 // ::SKIP_ELEMENT[10] skip_mode; 
	// skip_mode[0] . valid is ignored ;
	// skip_mode[0] . skip_flag is ignored ;
	// skip_mode[0] . skip_idx is ignored ;
	// skip_mode[0] . umve_flag is ignored ;
	// skip_mode[0] . umve_idx is ignored ;
	// skip_mode[0] . affine_flag is ignored ;
	// skip_mode[0] . mvp[0] is ignored ;
	// skip_mode[0] . refi is ignored ;

 // ::SKIP_RDO* skip_rdo; 
	// skip_rdo[0] . pmv_cands[0][0][0] is ignored ;
	// skip_rdo[0] . refi_cands[0][0] is ignored ;
	// skip_rdo[0] . mode_list[0] is ignored ;
	dummy += skip_rdo[0].num_rdo;

	// ::SKIP_ELEMENT[12] skip_rdo_mode; 
	   // skip_rdo_mode[0] . valid is ignored ;
	   // skip_rdo_mode[0] . skip_flag is ignored ;
	dummy += skip_rdo_mode[0].skip_idx;
	dummy += skip_rdo_mode[0].umve_flag;
	dummy += skip_rdo_mode[0].umve_idx;
	dummy += skip_rdo_mode[0].affine_flag;
	dummy += (skip_rdo_mode[0].mvp[0][0]) + (skip_rdo_mode[0].mvp[0][1]/*TODO:*/);
	dummy += skip_rdo_mode[0].refi[0];

	// ::S14[2] sub_pel_mv; 
	dummy += sub_pel_mv[0];
	dummy += sub_pel_mv[1];

	// ::MERGE_RDO merge_rdo; 
	   // merge_rdo . mrg_list_refi[0][0] is ignored ;
	   // merge_rdo . mrg_list_cp_mv[0][0][0][0] is ignored ;
	   // merge_rdo . mrg_list_cp_num[0] is ignored ;
	   // merge_rdo . num_cands is ignored ;
	   // merge_rdo . valid[0] is ignored ;

	// ::U3 cu_width_log2; 
	dummy += mod_info_curr_ptr->cu_width_log2;

	// ::U3 cu_height_log2; 
	dummy += mod_info_curr_ptr->cu_height_log2;

	// ::U2 tree_status; 
	dummy += tree_status;

	// ::U2 curr_cons_pred_mode; 
	dummy += curr_cons_pred_mode;

	// ::s16[2][5][2] mvp_all; 
	dummy += mvp_all[0][0][0];
	dummy += mvp_all[0][0][1];
	dummy += mvp_all[0][1][0];
	dummy += mvp_all[0][1][1];
	dummy += mvp_all[0][2][0];
	dummy += mvp_all[0][2][1];
	dummy += mvp_all[0][3][0];
	dummy += mvp_all[0][3][1];
	dummy += mvp_all[0][4][0];
	dummy += mvp_all[0][4][1];

	// ::U13 pic_width; 
	dummy += (md_fw_ptr->pic_height_in_scu << 2);
	// ::U13 pic_height; 
	dummy += (md_fw_ptr->pic_width_in_scu << 2);

	// struct ::MD_KERNEL_INPUT_32{U13 x_pos;U13 y_pos;U12 x_chroma;U12 y_chroma;U1 chroma_motion;U23 scup;U7 cu_width;U7 cu_height;U3 cu_width_log2;U3 cu_height_log2;}* md_kernel_input_32; 
	dummy += md_kernel_input_32[0].x_pos;
	dummy += md_kernel_input_32[0].y_pos;
	dummy += md_kernel_input_32[0].x_chroma;
	dummy += md_kernel_input_32[0].y_chroma;
	dummy += md_kernel_input_32[0].chroma_motion;
	// md_kernel_input_32[0] . scup is ignored ;
	dummy += md_kernel_input_32[0].cu_width;
	dummy += md_kernel_input_32[0].cu_height;
	dummy += md_kernel_input_32[0].cu_width_log2;
	dummy += md_kernel_input_32[0].cu_height_log2;

	// ::pel[88][88] refwin_md; 
	dummy += Fetch_Ref_window_ptr->RefWindowLCU[0];

	// ::S14[2][2][2] CtrPosFME; 
	dummy += (Fetch_Ref_window_ptr->CtrPos[0][0][0]) + (Fetch_Ref_window_ptr->CtrPos[0][0][1]/*TODO:*/);

	// ::U2 slice_type; 
	dummy += md_fw_ptr->slice_type;

	// ::U4 bit_depth; 
	dummy += md_fw_ptr->bit_depth_internal;

	// ::s64 lambda_y; 
	dummy += md_input_ptr->lambda_y;


	// ::s64 cost_best; 
	*cost_best += dummy;

	// class ::hls::stream < s64 > {protected: string _name;class deque< s64 , allocator< s64 > > _data;}& cost_best_out; 
	   // cost_best_out . _name . npos is ignored ;
	   // cost_best_out . _name . _M_dataplus . _M_p is ignored ;
	   // cost_best_out . _name . _M_string_length is ignored ;

   //add manual
   //1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].num_nz core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].num_nz cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].num_nz cyclic factor=3 dim=2
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mv core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mv cyclic factor=2 dim=2
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mvd core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].mvd cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mvd cyclic factor=2 dim=2
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].affine_mv core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].affine_mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].affine_mv cyclic factor=4 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].affine_mv cyclic factor=2 dim=3
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].affine_mvd core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].affine_mvd cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].affine_mvd cyclic factor=4 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].affine_mvd cyclic factor=2 dim=3
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].ipm core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].ipm cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].ipm cyclic factor=2 dim=2
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mpm core=RAM_1P_BRAM
#pragma HLS array_partition variable=best_info_ptr[0].mpm cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mpm cyclic factor=2 dim=2
//add end

//#pragma HLS resource variable=CtrPosFME core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=Fetch_Ref_window_ptr core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].affine_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].cu_mode core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mvr_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].pb_part core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].refi core=RAM_T2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].skip_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].smvd_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].tb_part core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].umve_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].umve_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=cu_data_temp_ptr[0].coef_y core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=cu_data_temp_ptr[0].reco_y core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].mpm core=RAM_1P_BRAM 
//#pragma HLS resource variable=model_cnt[0].cbf core=RAM_T2P_BRAM 
//#pragma HLS resource variable=orgY core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=rdo_list core=RAM_1P_BRAM 
//#pragma HLS resource variable=refwin_md core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_rdo_mode[0].mvp core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].refi core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].skip_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].umve_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=src_le_temp core=RAM_2P_BRAM 
#pragma HLS resource variable=src_up_temp core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=valid_mode core=RAM_1P_BRAM 
#pragma HLS array_partition variable=Fetch_Ref_window_ptr->CtrPos complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr->CtrPos complete dim=2
#pragma HLS array_partition variable=Fetch_Ref_window_ptr->CtrPos complete dim=3

#pragma HLS array_partition variable=cu_data_temp_ptr[0].coef_y cyclic factor=32 dim=1
#pragma HLS array_partition variable=cu_data_temp_ptr[0].reco_y cyclic factor=32 dim=1
#pragma HLS array_partition variable=mvp_all complete dim=1
#pragma HLS array_partition variable=mvp_all complete dim=2
#pragma HLS array_partition variable=mvp_all complete dim=3
#pragma HLS array_partition variable=orgY_32 complete dim=2
#pragma HLS array_partition variable=sub_pel_mv complete dim=1
// paste code end
}
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
	)
{
	// * cost_best = MAX_S64 >> 4;
	static S64 dummy = 0;
#pragma HLS reset variable=dummy

// ::u8 all_rdo_num; 
    dummy+=all_rdo_num;

 // ::U8 w; 
    dummy+=w;

 // ::U8 h; 
    dummy+=h;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U2 curr_cons_pred_mode; 
    dummy+=curr_cons_pred_mode;

 // ::U4 bit_depth; 
    dummy+=bit_depth;

 // ::u16 avail_tb; 
    dummy+=avail_tb;

 // ::RDOQ_MODEL* model_cnt; 
    //(model_cnt[0] . cbf[0])++;
    // model_cnt[0] . run_rdoq[0] is ignored ;
    // model_cnt[0] . level[0] is ignored ;
    // model_cnt[0] . last1[0] is ignored ;
    // model_cnt[0] . last2[0] is ignored ;

 // ::RDOQ_ARRAY* rdoq_array; 
    // rdoq_array[0] . rdoq_est_run[0][0] is ignored ;
    // rdoq_array[0] . rdoq_est_cbf[0][0] is ignored ;
    // rdoq_array[0] . rdoq_est_level[0][0] is ignored ;
    // rdoq_array[0] . rdoq_est_last[0][0][0][0] is ignored ;

 // ::MD_FW* md_fw_ptr; 
    // md_fw_ptr[0] . pic_width is ignored ;
    // md_fw_ptr[0] . pic_height is ignored ;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    dummy+=md_fw_ptr[0] . affine_enable_flag;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    dummy+=(md_fw_ptr[0] . refp_ptr[0][0])+(md_fw_ptr[0] . refp_ptr[0][1]/*TODO:*/);
#if ENABLE_BFRAME
	dummy += md_fw_ptr[0].num_refp[0];
#else
    dummy+=md_fw_ptr[0] . num_refp;
#endif
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    // md_input_ptr[0] . pix_x is ignored ;
    // md_input_ptr[0] . pix_y is ignored ;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPosFME[0][0][0] is ignored ;

 // ::SKIP_RDO* skip_rdo; 
    // skip_rdo[0] . pmv_cands[0][0][0] is ignored ;
    // skip_rdo[0] . refi_cands[0][0] is ignored ;
    // skip_rdo[0] . mode_list[0] is ignored ;
    dummy+=skip_rdo[0] . num_rdo;

 // ::MERGE_RDO merge_rdo; 
    // merge_rdo . mrg_list_refi[0][0] is ignored ;
    // merge_rdo . mrg_list_cp_mv[0][0][0][0] is ignored ;
    // merge_rdo . mrg_list_cp_num[0] is ignored ;
    // merge_rdo . num_cands is ignored ;
    // merge_rdo . valid[0] is ignored ;

 // ::SKIP_ELEMENT[12] skip_mode; 
    // skip_mode[0] . valid is ignored ;
    // skip_mode[0] . skip_flag is ignored ;
    // skip_mode[0] . skip_idx is ignored ;
    // skip_mode[0] . umve_flag is ignored ;
    // skip_mode[0] . umve_idx is ignored ;
    // skip_mode[0] . affine_flag is ignored ;
    // skip_mode[0] . mvp[0] is ignored ;
    // skip_mode[0] . refi is ignored ;

 // ::SKIP_ELEMENT[12] skip_rdo_mode; 
    // skip_rdo_mode[0] . valid is ignored ;
    // skip_rdo_mode[0] . skip_flag is ignored ;
    dummy+=skip_rdo_mode[0] . skip_idx;
    dummy+=skip_rdo_mode[0] . umve_flag;
    dummy+=skip_rdo_mode[0] . umve_idx;
    dummy+=skip_rdo_mode[0] . affine_flag;
    dummy+=(skip_rdo_mode[0] . mvp[0][0])+(skip_rdo_mode[0] . mvp[0][1]/*TODO:*/);
    dummy+=skip_rdo_mode[0] . refi[0];

 // ::MD_COM_MODE[128] mod_info_curr_ptr; 
    // mod_info_curr_ptr[0] . x_scu is ignored ;
    // mod_info_curr_ptr[0] . y_scu is ignored ;
    // mod_info_curr_ptr[0] . cud is ignored ;
    dummy+=mod_info_curr_ptr[0] . cu_width;
    dummy+=mod_info_curr_ptr[0] . cu_height;
    dummy+=mod_info_curr_ptr[0] . cu_width_log2;
    dummy+=mod_info_curr_ptr[0] . cu_height_log2;
    dummy+=mod_info_curr_ptr[0] . x_pos;
    dummy+=mod_info_curr_ptr[0] . y_pos;
    // mod_info_curr_ptr[0] . scup is ignored ;
    (mod_info_curr_ptr[0] . cu_mode)++;
    dummy+=mod_info_curr_ptr[0] . pb_part;
    (mod_info_curr_ptr[0] . tb_part)++;
    (mod_info_curr_ptr[0].num_nz[0][0])++;
    (mod_info_curr_ptr[0].num_nz[0][1])++;
    (mod_info_curr_ptr[0].num_nz[0][2])++;
    (mod_info_curr_ptr[0].num_nz[1][0])++;
    (mod_info_curr_ptr[0].num_nz[1][1])++;
    (mod_info_curr_ptr[0].num_nz[1][2])++;
    (mod_info_curr_ptr[0].num_nz[2][0])++;
    (mod_info_curr_ptr[0].num_nz[2][1])++;
    (mod_info_curr_ptr[0].num_nz[2][2])++;
    (mod_info_curr_ptr[0].num_nz[3][0])++;
    (mod_info_curr_ptr[0].num_nz[3][1])++;
    (mod_info_curr_ptr[0].num_nz[3][2])++;
    // mod_info_curr_ptr[0] . refi[0] is ignored ;
    // mod_info_curr_ptr[0] . mvr_idx is ignored ;
    (mod_info_curr_ptr[0] . umve_flag)++;
    (mod_info_curr_ptr[0] . umve_idx)++;
    (mod_info_curr_ptr[0] . skip_idx)++;
    dummy+=mod_info_curr_ptr[0] . mvp_from_hmvp_flag;
    // mod_info_curr_ptr[0] . mvd[0][0] is ignored ;
    // mod_info_curr_ptr[0] . mv[0][0] is ignored ;
    (mod_info_curr_ptr[0] . affine_flag)++;
    (mod_info_curr_ptr[0].affine_mv[0][0][0])++;
    (mod_info_curr_ptr[0].affine_mv[0][0][1])++;
    (mod_info_curr_ptr[0].affine_mv[0][1][0])++;
    (mod_info_curr_ptr[0].affine_mv[0][1][1])++;
    (mod_info_curr_ptr[0].affine_mv[0][2][0])++;
    (mod_info_curr_ptr[0].affine_mv[0][2][1])++;
    (mod_info_curr_ptr[0].affine_mv[0][3][0])++;
    (mod_info_curr_ptr[0].affine_mv[0][3][1])++;
    (mod_info_curr_ptr[0].affine_mv[1][0][0])++;
    (mod_info_curr_ptr[0].affine_mv[1][0][1])++;
    (mod_info_curr_ptr[0].affine_mv[1][1][0])++;
    (mod_info_curr_ptr[0].affine_mv[1][1][1])++;
    (mod_info_curr_ptr[0].affine_mv[1][2][0])++;
    (mod_info_curr_ptr[0].affine_mv[1][2][1])++;
    (mod_info_curr_ptr[0].affine_mv[1][3][0])++;
    (mod_info_curr_ptr[0].affine_mv[1][3][1])++;
    (mod_info_curr_ptr[0].affine_mvd[0][0][0])++;
    (mod_info_curr_ptr[0].affine_mvd[0][0][1])++;
    (mod_info_curr_ptr[0].affine_mvd[0][1][0])++;
    (mod_info_curr_ptr[0].affine_mvd[0][1][1])++;
    (mod_info_curr_ptr[0].affine_mvd[0][2][0])++;
    (mod_info_curr_ptr[0].affine_mvd[0][2][1])++;
    (mod_info_curr_ptr[0].affine_mvd[0][3][0])++;
    (mod_info_curr_ptr[0].affine_mvd[0][3][1])++;
    (mod_info_curr_ptr[0].affine_mvd[1][0][0])++;
    (mod_info_curr_ptr[0].affine_mvd[1][0][1])++;
    (mod_info_curr_ptr[0].affine_mvd[1][1][0])++;
    (mod_info_curr_ptr[0].affine_mvd[1][1][1])++;
    (mod_info_curr_ptr[0].affine_mvd[1][2][0])++;
    (mod_info_curr_ptr[0].affine_mvd[1][2][1])++;
    (mod_info_curr_ptr[0].affine_mvd[1][3][0])++;
    (mod_info_curr_ptr[0].affine_mvd[1][3][1])++;
    (mod_info_curr_ptr[0] . smvd_flag)++;
    dummy+=mod_info_curr_ptr[0].mpm[0][0];
    dummy+=mod_info_curr_ptr[0].mpm[0][1];
    dummy+=mod_info_curr_ptr[0].mpm[1][0];
    dummy+=mod_info_curr_ptr[0].mpm[1][1];
    dummy+=mod_info_curr_ptr[0].mpm[2][0];
    dummy+=mod_info_curr_ptr[0].mpm[2][1];
    dummy+=mod_info_curr_ptr[0].mpm[3][0];
    dummy+=mod_info_curr_ptr[0].mpm[3][1];
    // mod_info_curr_ptr[0] . ipm[0][0] is ignored ;
    // mod_info_curr_ptr[0] . ipf_flag is ignored ;
    dummy+=mod_info_curr_ptr[0] . chroma_motion;
    // mod_info_curr_ptr[0] . x_luma is ignored ;
    // mod_info_curr_ptr[0] . y_luma is ignored ;
    // mod_info_curr_ptr[0] . x_chroma is ignored ;
    // mod_info_curr_ptr[0] . y_chroma is ignored ;
    // mod_info_curr_ptr[0] . last_child is ignored ;

 // ::MD_COM_MODE_BEST[128] best_info_ptr; 
    // best_info_ptr[0] . x_scu is ignored ;
    // best_info_ptr[0] . y_scu is ignored ;
    // best_info_ptr[0] . cud is ignored ;
    // best_info_ptr[0] . cu_width is ignored ;
    // best_info_ptr[0] . cu_height is ignored ;
    // best_info_ptr[0] . cu_width_log2 is ignored ;
    // best_info_ptr[0] . cu_height_log2 is ignored ;
    // best_info_ptr[0] . x_pos is ignored ;
    // best_info_ptr[0] . y_pos is ignored ;
    // best_info_ptr[0] . scup is ignored ;
    best_info_ptr[0] . cu_mode=dummy;
    best_info_ptr[0] . pb_part=dummy;
    best_info_ptr[0] . tb_part=dummy;
    // best_info_ptr[0] . rec_y[0] is ignored ;
    // best_info_ptr[0] . rec_u[0] is ignored ;
    // best_info_ptr[0] . rec_v[0] is ignored ;
    // best_info_ptr[0] . coef_y[0] is ignored ;
    // best_info_ptr[0] . coef_u[0] is ignored ;
    // best_info_ptr[0] . coef_v[0] is ignored ;
    // best_info_ptr[0] . pred_y[0] is ignored ;
    // best_info_ptr[0] . pred_u[0] is ignored ;
    // best_info_ptr[0] . pred_v[0] is ignored ;
    best_info_ptr[0].num_nz[0][0]=dummy;
    best_info_ptr[0].num_nz[0][1]=dummy;
    best_info_ptr[0].num_nz[0][2]=dummy;
    best_info_ptr[0].num_nz[1][0]=dummy;
    best_info_ptr[0].num_nz[1][1]=dummy;
    best_info_ptr[0].num_nz[1][2]=dummy;
    best_info_ptr[0].num_nz[2][0]=dummy;
    best_info_ptr[0].num_nz[2][1]=dummy;
    best_info_ptr[0].num_nz[2][2]=dummy;
    best_info_ptr[0].num_nz[3][0]=dummy;
    best_info_ptr[0].num_nz[3][1]=dummy;
    best_info_ptr[0].num_nz[3][2]=dummy;
    (best_info_ptr[0] . refi[0])=(best_info_ptr[0] . refi[1]/*TODO:*/)=dummy;
    best_info_ptr[0] . mvr_idx=dummy;
    best_info_ptr[0] . umve_flag=dummy;
    best_info_ptr[0] . umve_idx=dummy;
    best_info_ptr[0] . skip_idx=dummy;
    best_info_ptr[0] . mvp_from_hmvp_flag=dummy;
    best_info_ptr[0].mvd[0][0]=dummy;
    best_info_ptr[0].mvd[0][1]=dummy;
    best_info_ptr[0].mvd[1][0]=dummy;
    best_info_ptr[0].mvd[1][1]=dummy;
    best_info_ptr[0].mv[0][0]=dummy;
    best_info_ptr[0].mv[0][1]=dummy;
    best_info_ptr[0].mv[1][0]=dummy;
    best_info_ptr[0].mv[1][1]=dummy;
    best_info_ptr[0] . affine_flag=dummy;
    best_info_ptr[0].affine_mv[0][0][0]=dummy;
    best_info_ptr[0].affine_mv[0][0][1]=dummy;
    best_info_ptr[0].affine_mv[0][1][0]=dummy;
    best_info_ptr[0].affine_mv[0][1][1]=dummy;
    best_info_ptr[0].affine_mv[0][2][0]=dummy;
    best_info_ptr[0].affine_mv[0][2][1]=dummy;
    best_info_ptr[0].affine_mv[0][3][0]=dummy;
    best_info_ptr[0].affine_mv[0][3][1]=dummy;
    best_info_ptr[0].affine_mv[1][0][0]=dummy;
    best_info_ptr[0].affine_mv[1][0][1]=dummy;
    best_info_ptr[0].affine_mv[1][1][0]=dummy;
    best_info_ptr[0].affine_mv[1][1][1]=dummy;
    best_info_ptr[0].affine_mv[1][2][0]=dummy;
    best_info_ptr[0].affine_mv[1][2][1]=dummy;
    best_info_ptr[0].affine_mv[1][3][0]=dummy;
    best_info_ptr[0].affine_mv[1][3][1]=dummy;
    best_info_ptr[0].affine_mvd[0][0][0]=dummy;
    best_info_ptr[0].affine_mvd[0][0][1]=dummy;
    best_info_ptr[0].affine_mvd[0][1][0]=dummy;
    best_info_ptr[0].affine_mvd[0][1][1]=dummy;
    best_info_ptr[0].affine_mvd[0][2][0]=dummy;
    best_info_ptr[0].affine_mvd[0][2][1]=dummy;
    best_info_ptr[0].affine_mvd[0][3][0]=dummy;
    best_info_ptr[0].affine_mvd[0][3][1]=dummy;
    best_info_ptr[0].affine_mvd[1][0][0]=dummy;
    best_info_ptr[0].affine_mvd[1][0][1]=dummy;
    best_info_ptr[0].affine_mvd[1][1][0]=dummy;
    best_info_ptr[0].affine_mvd[1][1][1]=dummy;
    best_info_ptr[0].affine_mvd[1][2][0]=dummy;
    best_info_ptr[0].affine_mvd[1][2][1]=dummy;
    best_info_ptr[0].affine_mvd[1][3][0]=dummy;
    best_info_ptr[0].affine_mvd[1][3][1]=dummy;
    best_info_ptr[0] . smvd_flag=dummy;
    best_info_ptr[0].mpm[0][0]=dummy;
    best_info_ptr[0].mpm[0][1]=dummy;
    best_info_ptr[0].mpm[1][0]=dummy;
    best_info_ptr[0].mpm[1][1]=dummy;
    best_info_ptr[0].mpm[2][0]=dummy;
    best_info_ptr[0].mpm[2][1]=dummy;
    best_info_ptr[0].mpm[3][0]=dummy;
    best_info_ptr[0].mpm[3][1]=dummy;
    best_info_ptr[0].ipm[0][0]=dummy;
    best_info_ptr[0].ipm[0][1]=dummy;
    best_info_ptr[0].ipm[1][0]=dummy;
    best_info_ptr[0].ipm[1][1]=dummy;
    best_info_ptr[0].ipm[2][0]=dummy;
    best_info_ptr[0].ipm[2][1]=dummy;
    best_info_ptr[0].ipm[3][0]=dummy;
    best_info_ptr[0].ipm[3][1]=dummy;
    // best_info_ptr[0] . ipf_flag is ignored ;
    // best_info_ptr[0] . chroma_motion is ignored ;
    // best_info_ptr[0] . x_luma is ignored ;
    // best_info_ptr[0] . y_luma is ignored ;
    // best_info_ptr[0] . x_chroma is ignored ;
    // best_info_ptr[0] . y_chroma is ignored ;
    // best_info_ptr[0] . last_child is ignored ;

 // ::ENC_CU_DATA_ARRAY* cu_data_temp_ptr; 
    // cu_data_temp_ptr[0] . split_mode[0][0][0] is ignored ;
    // cu_data_temp_ptr[0] . mv[0][0][0] is ignored ;
    // cu_data_temp_ptr[0] . mvd[0][0][0] is ignored ;
    // cu_data_temp_ptr[0] . pb_part[0] is ignored ;
    // cu_data_temp_ptr[0] . tb_part[0] is ignored ;
    // cu_data_temp_ptr[0] . pred_mode[0] is ignored ;
    // cu_data_temp_ptr[0] . mvr_idx[0] is ignored ;
    // cu_data_temp_ptr[0] . umve_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . umve_idx[0] is ignored ;
    // cu_data_temp_ptr[0] . skip_idx[0] is ignored ;
    // cu_data_temp_ptr[0] . map_scu[0] is ignored ;
    // cu_data_temp_ptr[0] . affine_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . smvd_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . map_cu_mode[0] is ignored ;
    // cu_data_temp_ptr[0] . map_pb_tb_part[0] is ignored ;
    // cu_data_temp_ptr[0] . depth[0] is ignored ;
    // cu_data_temp_ptr[0] . ipf_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . mvp_from_hmvp_flag[0] is ignored ;
    // cu_data_temp_ptr[0] . mpm[0][0] is ignored ;
    // cu_data_temp_ptr[0] . mpm_ext[0][0] is ignored ;
    // cu_data_temp_ptr[0] . ipm[0][0] is ignored ;
    // cu_data_temp_ptr[0] . refi[0][0] is ignored ;
    // cu_data_temp_ptr[0] . num_nz_coef[0][0] is ignored ;
    cu_data_temp_ptr[0].coef_y[0]=dummy;
    // cu_data_temp_ptr[0] . coef_u[0] is ignored ;
    // cu_data_temp_ptr[0] . coef_v[0] is ignored ;
    cu_data_temp_ptr[0].reco_y[0]=dummy;
    // cu_data_temp_ptr[0] . reco_u[0] is ignored ;
    // cu_data_temp_ptr[0] . reco_v[0] is ignored ;
    // cu_data_temp_ptr[0] . x_chroma is ignored ;
    // cu_data_temp_ptr[0] . y_chroma is ignored ;
    // cu_data_temp_ptr[0] . x_last is ignored ;
    // cu_data_temp_ptr[0] . y_last is ignored ;
    // cu_data_temp_ptr[0] . last_child is ignored ;

 // ::u8[5] rdo_list; 
    dummy+=rdo_list[0];

 // ::U1[17] valid_mode; 
    dummy+=valid_mode[0];

 // ::u8[5] ipred_list; 
    // ipred_list[0] is ignored ;

 // ::pel[131] src_le_temp; 
    dummy+=(src_le_temp[0])+(src_le_temp[1]/*TODO:*/);

 // ::pel[131] src_up_temp; 
    dummy+=(src_up_temp[0])+(src_up_temp[1]/*TODO:*/);

 // ::S14[2] sub_pel_mv; 
    dummy+=sub_pel_mv[0];
    dummy+=sub_pel_mv[1];

 // ::s16[2][5][2] mvp_all; 
    dummy+=mvp_all[0][0][0];
    dummy+=mvp_all[0][0][1];
    dummy+=mvp_all[0][1][0];
    dummy+=mvp_all[0][1][1];
    dummy+=mvp_all[0][2][0];
    dummy+=mvp_all[0][2][1];
    dummy+=mvp_all[0][3][0];
    dummy+=mvp_all[0][3][1];
    dummy+=mvp_all[0][4][0];
    dummy+=mvp_all[0][4][1];

 // ::pel[4096] orgY; 
 for(int i=0;i<64;i++){
    dummy+=(orgY[i])+(orgY[i+64]/*TODO:*/);
 }
   // dummy+=(orgY[0])+(orgY[1]/*TODO:*/);
   // dummy+=(orgY[1])+(orgY[1]);
   // dummy+=(orgY[2])+(orgY[2]);
   // dummy+=(orgY[3])+(orgY[3]);
   // dummy+=(orgY[4])+(orgY[4]);
   // dummy+=(orgY[5])+(orgY[5]);
   // dummy+=(orgY[6])+(orgY[6]);
   // dummy+=(orgY[7])+(orgY[7]);
   // dummy+=(orgY[8])+(orgY[8]);
   // dummy+=(orgY[9])+(orgY[9]);
   // dummy+=(orgY[10])+(orgY[10]);
   // dummy+=(orgY[11])+(orgY[11]);
   // dummy+=(orgY[12])+(orgY[12]);
   // dummy+=(orgY[13])+(orgY[13]);
   // dummy+=(orgY[14])+(orgY[14]);
   // dummy+=(orgY[15])+(orgY[15]);
   // dummy+=(orgY[16])+(orgY[16]);
   // dummy+=(orgY[17])+(orgY[17]);
   // dummy+=(orgY[18])+(orgY[18]);
   // dummy+=(orgY[19])+(orgY[19]);
   // dummy+=(orgY[20])+(orgY[20]);
   // dummy+=(orgY[21])+(orgY[21]);
   // dummy+=(orgY[22])+(orgY[22]);
   // dummy+=(orgY[23])+(orgY[23]);
   // dummy+=(orgY[24])+(orgY[24]);
   // dummy+=(orgY[25])+(orgY[25]);
   // dummy+=(orgY[26])+(orgY[26]);
   // dummy+=(orgY[27])+(orgY[27]);
   // dummy+=(orgY[28])+(orgY[28]);
   // dummy+=(orgY[29])+(orgY[29]);
   // dummy+=(orgY[30])+(orgY[30]);
   // dummy+=(orgY[31])+(orgY[31]);
   // dummy+=(orgY[32])+(orgY[32]);
   // dummy+=(orgY[33])+(orgY[33]);
   // dummy+=(orgY[34])+(orgY[34]);
   // dummy+=(orgY[35])+(orgY[35]);
   // dummy+=(orgY[36])+(orgY[36]);
   // dummy+=(orgY[37])+(orgY[37]);
   // dummy+=(orgY[38])+(orgY[38]);
   // dummy+=(orgY[39])+(orgY[39]);
   // dummy+=(orgY[40])+(orgY[40]);
   // dummy+=(orgY[41])+(orgY[41]);
   // dummy+=(orgY[42])+(orgY[42]);
   // dummy+=(orgY[43])+(orgY[43]);
   // dummy+=(orgY[44])+(orgY[44]);
   // dummy+=(orgY[45])+(orgY[45]);
   // dummy+=(orgY[46])+(orgY[46]);
   // dummy+=(orgY[47])+(orgY[47]);
   // dummy+=(orgY[48])+(orgY[48]);
   // dummy+=(orgY[49])+(orgY[49]);
   // dummy+=(orgY[50])+(orgY[50]);
   // dummy+=(orgY[51])+(orgY[51]);
   // dummy+=(orgY[52])+(orgY[52]);
   // dummy+=(orgY[53])+(orgY[53]);
   // dummy+=(orgY[54])+(orgY[54]);
   // dummy+=(orgY[55])+(orgY[55]);
   // dummy+=(orgY[56])+(orgY[56]);
   // dummy+=(orgY[57])+(orgY[57]);
   // dummy+=(orgY[58])+(orgY[58]);
   // dummy+=(orgY[59])+(orgY[59]);
   // dummy+=(orgY[60])+(orgY[60]);
   // dummy+=(orgY[61])+(orgY[61]);
   // dummy+=(orgY[62])+(orgY[62]);
   // dummy+=(orgY[63])+(orgY[63]);

 // ::pel[49152] RefWindowLCU; 

 // ::S14[2][2][2] CtrPos; 

 // ::s64[128][2] flag_cost; 
    (flag_cost[0])++;

 // ::s32 dist; 
    // dist is ignored ;


 // ::s64[5] cost_best; 
    (cost_best[0]) +=  dummy;

 // ::pel[88][88] refwin_md; 

 // ::S14[2][2][2] CtrPosFME; 

//#pragma HLS resource variable=RefWindowLCU core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].affine_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].cu_mode core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mvr_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].pb_part core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].refi core=RAM_T2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].skip_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].smvd_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].tb_part core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].umve_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].umve_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].affine_mv core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].affine_mvd core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].ipm core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mpm core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mv core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].mvd core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=best_info_ptr[0].num_nz core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=cost_best core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=cu_data_temp_ptr[0].coef_y core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=cu_data_temp_ptr[0].reco_y core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=flag_cost core=RAM_1P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].affine_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].chroma_motion core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].cu_height core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].cu_height_log2 core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].cu_mode core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].cu_width core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].cu_width_log2 core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].pb_part core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].skip_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].smvd_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].tb_part core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].umve_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].umve_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].x_pos core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].y_pos core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].affine_mv core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].affine_mvd core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].mpm core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=mod_info_curr_ptr[0].num_nz core=RAM_1P_BRAM 
//1PBRAM_disable //#pragma HLS resource variable=model_cnt[0].cbf core=RAM_1P_BRAM 
#pragma HLS resource variable=orgY core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=rdo_list core=RAM_1P_BRAM 
//#pragma HLS resource variable=refwin_md core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_rdo_mode[0].mvp core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].refi core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].skip_idx core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].umve_flag core=RAM_1P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=skip_rdo_mode[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=src_le_temp core=RAM_2P_BRAM 
#pragma HLS resource variable=src_up_temp core=RAM_2P_BRAM 
//1PBRAM_disable #pragma HLS resource variable=valid_mode core=RAM_1P_BRAM 
#pragma HLS array_partition variable=best_info_ptr[0].affine_mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].affine_mv cyclic factor=4 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].affine_mv cyclic factor=2 dim=3
#pragma HLS array_partition variable=best_info_ptr[0].affine_mvd cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].affine_mvd cyclic factor=4 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].affine_mvd cyclic factor=2 dim=3
#pragma HLS array_partition variable=best_info_ptr[0].ipm cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].ipm cyclic factor=2 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].mpm cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mpm cyclic factor=2 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mv cyclic factor=2 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].mvd cyclic factor=2 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].mvd cyclic factor=2 dim=2
#pragma HLS array_partition variable=best_info_ptr[0].num_nz cyclic factor=4 dim=1
#pragma HLS array_partition variable=best_info_ptr[0].num_nz cyclic factor=3 dim=2
//#pragma HLS array_partition variable=cu_data_temp_ptr[0].coef_y cyclic factor=256 dim=1
//#pragma HLS array_partition variable=cu_data_temp_ptr[0].reco_y cyclic factor=256 dim=1
//#pragma HLS array_partition variable=flag_cost complete dim=2
#pragma HLS array_partition variable=mod_info_curr_ptr[0].affine_mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].affine_mv cyclic factor=4 dim=2
#pragma HLS array_partition variable=mod_info_curr_ptr[0].affine_mv cyclic factor=2 dim=3
#pragma HLS array_partition variable=mod_info_curr_ptr[0].affine_mvd cyclic factor=2 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].affine_mvd cyclic factor=4 dim=2
#pragma HLS array_partition variable=mod_info_curr_ptr[0].affine_mvd cyclic factor=2 dim=3
#pragma HLS array_partition variable=mod_info_curr_ptr[0].mpm cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].mpm cyclic factor=2 dim=2
#pragma HLS array_partition variable=mod_info_curr_ptr[0].num_nz cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_curr_ptr[0].num_nz cyclic factor=3 dim=2
#pragma HLS array_partition variable=mvp_all complete dim=1
#pragma HLS array_partition variable=mvp_all complete dim=2
#pragma HLS array_partition variable=mvp_all complete dim=3
#pragma HLS array_partition variable=orgY cyclic factor=64 dim=1
#pragma HLS array_partition variable=sub_pel_mv complete dim=1
}
#endif //LUMA_64_PIPELINE_DUMMY

#if MD_KERNEL_8_DUMMY
#if USE_FACADE_MD_8
s64 md_kernel_8(U8 width, U8 height, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPD_RDO_CNT],
	pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_ref1[SWW * SWH], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, pel refwin_md_ref1[88][88], S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status, ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, 
	pel refwin_md[88][88], S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window* Fetch_Ref_window_ptr)
#else
s64 md_kernel_8(U8 width, U8 height, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPD_RDO_CNT],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4],
	ENC_ME_NEB_INFO left_nebs[2],
	SKIP_MERGE_INPUT* skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status, ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8, 
	IP_MAP_SCU_SIZE<8, 8>*	IP_map_scu_ptr_8x8, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
#endif
{
	s64 dummy = MAX_S64 >> 4;
	U1 zero = (md_input_ptr->pix_x == 0x1fff);
 // ::U8 width; 
    dummy+=width;

 // ::U8 height; 
    dummy+=height;

 // ::MD_FW* md_fw_ptr; 
    // md_fw_ptr[0] . pic_width is ignored ;
    // md_fw_ptr[0] . pic_height is ignored ;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    dummy+=md_fw_ptr[0] . max_part_ratio;
    dummy+=md_fw_ptr[0] . min_cu_size;
    dummy+=md_fw_ptr[0] . max_split_times;
    dummy+=md_fw_ptr[0] . min_qt_size;
    dummy+=md_fw_ptr[0] . max_bt_size;
    dummy+=md_fw_ptr[0] . max_eqt_size;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    // md_fw_ptr[0] . mipf_enable_flag is ignored ;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    dummy+=md_fw_ptr[0] . refp_ptr[0][0];
    dummy+=(md_fw_ptr[0] . num_refp[0])+(md_fw_ptr[0] . num_refp[1]/*TODO:*/);
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    dummy+=md_input_ptr[0] . pix_x;
    dummy+=md_input_ptr[0] . pix_y;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . lambda_y;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;

 // ::u8[5] rmd_mode; 
    dummy+=rmd_mode[0];

 // ::ME_MV_DIR* me_mv_dir; 
    // me_mv_dir[0] . block64x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block64x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block64x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x64MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block8x8MvDir[0][0] . lidx;
    // me_mv_dir[0] . block8x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block64x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block8x4MvDir[0][0] . lidx;
    // me_mv_dir[0] . block8x4MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block4x8MvDir[0][0] . lidx;
    // me_mv_dir[0] . block4x8MvDir[0][0] . cur_refi is ignored ;

 // ::pel[4096] p_fenc_LCU_Y; 
    dummy+=p_fenc_LCU_Y[0][0];

 // struct ::ENC_ME_LINE_MAP_SIZE < 8UL > {U32 map_scu_up_line[4];U32 map_cu_mode_up_line[4];U32 map_scu_left_line[2];U32 map_cu_mode_left_line[2];U11 min_scu_x;U11 min_scu_y;U1 above_line_idx;U1 curr_ctu_idx;U32 map_scu_line_hor[16];U32 map_cu_mode_line_hor[16];U32 map_scu_line_ver[8];U32 map_cu_mode_line_ver[8];}* me_line_map_ptr_8x8; 
    dummy+=(me_line_map_ptr_8x8[0] . map_scu_up_line[0])+(me_line_map_ptr_8x8[0] . map_scu_up_line[1]/*TODO:*/);
    dummy+=me_line_map_ptr_8x8[0] . map_cu_mode_up_line[0];
    dummy+=me_line_map_ptr_8x8[0] . map_scu_left_line[0];
    dummy+=me_line_map_ptr_8x8[0] . map_cu_mode_left_line[0];
    dummy+=me_line_map_ptr_8x8[0] . min_scu_x;
    dummy+=me_line_map_ptr_8x8[0] . min_scu_y;
    // me_line_map_ptr_8x8[0] . above_line_idx is ignored ;
    // me_line_map_ptr_8x8[0] . curr_ctu_idx is ignored ;
    // me_line_map_ptr_8x8[0] . map_scu_line_hor[0] is ignored ;
    // me_line_map_ptr_8x8[0] . map_cu_mode_line_hor[0] is ignored ;
    // me_line_map_ptr_8x8[0] . map_scu_line_ver[0] is ignored ;
    // me_line_map_ptr_8x8[0] . map_cu_mode_line_ver[0] is ignored ;

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[4] up_nebs; 
    dummy+=(up_nebs[0] . mv[0][0])+(up_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(up_nebs[0] . refi[0])+(up_nebs[0] . refi[1]/*TODO:*/);

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[2] left_nebs; 
    dummy+=(left_nebs[0] . mv[0][0])+(left_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(left_nebs[0] . refi[0])+(left_nebs[0] . refi[1]/*TODO:*/);

 // ::SKIP_MERGE_INPUT* skip_in; 
    dummy+=skip_in[0] . slice_type;
    dummy+=skip_in[0] . ptr;
    dummy+=skip_in[0] . umve_enable_flag;
    // skip_in[0] . refp_map_mv[0][0][0] is ignored ;
    // skip_in[0] . refp_map_refi[0][0] is ignored ;
    // skip_in[0] . pix_x is ignored ;
    dummy+=(skip_in[0] . refp_map_mv1[0][0][0][0])+(skip_in[0] . refp_map_mv1[0][0][0][1]/*TODO:*/);
    dummy+=skip_in[0] . refp_map_refi1[0][0][0][0];
    dummy+=skip_in[0] . refp_list_ptr1[0][0];
    dummy+=(skip_in[0] . refp_ptr1[0][0])+(skip_in[0] . refp_ptr1[0][1]/*TODO:*/);

 // ::U1 boundary; 
    dummy+=boundary;

 // ::U1 isNS_allow; 
    dummy+=isNS_allow;

 // ::U2[1] NS_luma_pred_mode;
    NS_luma_pred_mode[0]= DUMMY_OUT(dummy);
 
 // ::U13 x0; 
    dummy+=x0;

 // ::U13 y0; 
    dummy+=y0;

 // ::U12 cup; 
    // cup is ignored ;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U4 cud; 
    dummy+=cud;

 // ::U3 qt_depth; 
    dummy+=qt_depth;

 // ::U3 bet_depth; 
    dummy+=bet_depth;

 // ::U2 cons_pred_mode; 
    dummy+=cons_pred_mode;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // ::ENC_CU_DATA_ARRAY_INNER* cu_data_ptr; 
 #if MD_KERNEL_SET_DUMMY_VALUE
	for (int i = 0; i < (8 >> 2); i++) {
		for (int j = 0; j < (8 >> 2); j++) {
			for (int k = 0; k < REFP_NUM; k++) {
				for (int m = 0; m < MV_D; m++) {
					cu_data_ptr[0] . mv[i * (8 >> 2) + j][k][m]= (zero ? dummy : 0);
					cu_data_ptr[0] . mvd[i * (8 >> 2) + j][k][m]= (zero ? dummy : 0);
				}
			}
			cu_data_ptr[0] . pb_part[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . tb_part[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . pred_mode[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . umve_flag[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . mvr_idx[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . skip_idx[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . map_scu[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . affine_flag[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . smvd_flag[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . map_cu_mode[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . map_pb_tb_part[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . depth[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . ipf_flag[i * (8 >> 2) + j]= (zero ? dummy : 0);
			cu_data_ptr[0] . mvp_from_hmvp_flag[i * (8 >> 2) + j]= (zero ? dummy : 0);
			for (int k = 0; k < 2; k++) {
				cu_data_ptr[0] . mpm[k][i * (8 >> 2) + j]= (zero ? dummy : 0);
				cu_data_ptr[0] . ipm[k][i * (8 >> 2) + j]= (zero ? dummy : 0);
			}
			for (int k = 0; k < REFP_NUM; k++) {
				cu_data_ptr[0] . refi[i * (8 >> 2) + j][k]= (zero ? dummy : 0);
			}
			cu_data_ptr[0] . num_nz_coef[0][i * (8 >> 2) + j]= (zero ? dummy : 0);
		}
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			cu_data_ptr[0] . coef_y[i][j]= (zero ? dummy : 0);
			cu_data_ptr[0] . reco_y[i][j]= (zero ? dummy : 0);
		}
	}
#endif //MD_KERNEL_SET_DUMMY_VALUE
    // cu_data_ptr[0] . reco_u[0] is ignored ;
    // cu_data_ptr[0] . reco_v[0] is ignored ;
    // cu_data_ptr[0] . x_chroma is ignored ;
    // cu_data_ptr[0] . y_chroma is ignored ;
    // cu_data_ptr[0] . x_last is ignored ;
    // cu_data_ptr[0] . y_last is ignored ;
    // cu_data_ptr[0] . last_child is ignored ;

 // ::ENC_FME_INFO_ALL* fme_mv_info; 
    // fme_mv_info[0] . sadTree64x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x32Fmv[0][0][0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 8UL , 8UL > {pel up_line[16];pel left_line[16];pel top_left;pel rec_line_hor[16];pel rec_line_ver[16];}* IP_buffer_ptr_8x8; 
    dummy+=IP_buffer_ptr_8x8[0] . up_line[0];
    dummy+=IP_buffer_ptr_8x8[0] . left_line[0];
    dummy+=IP_buffer_ptr_8x8[0] . top_left;
    // IP_buffer_ptr_8x8[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_8x8[0] . rec_line_ver[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 8UL , 8UL > {pel up;pel left;pel up_line[2];pel left_line[2];pel ipm_line_ver[2];pel ipm_line_hor[4];}* MPM_buffer_ptr_8x8; 
    dummy+=MPM_buffer_ptr_8x8[0] . up;
    dummy+=MPM_buffer_ptr_8x8[0] . left;
    // MPM_buffer_ptr_8x8[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_8x8[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_8x8[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_8x8[0] . ipm_line_hor[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 8UL , 8UL > {U32 map_scu_above_line[4];U32 map_scu_top_left;U32 map_scu_up_line[4];U32 map_scu_left_line_new[4];U32 map_scu_top_left_new;U32 cur_map_scu[2][2];}* IP_map_scu_ptr_8x8; 
    // IP_map_scu_ptr_8x8[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_8x8[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_8x8[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_8x8[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_8x8[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_8x8[0] . cur_map_scu[0][0] is ignored ;

 // ::pel[2560][8][2][16] fme2mdrefbuf; 
    // fme2mdrefbuf[0][0][0][0] is ignored ;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0] . lidx[0];
    dummy+=Fetch_Ref_window_ptr[0] . cur_refi[0];
    // Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0] is ignored ;
	return DUMMY_OUT(dummy);

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x8[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x8[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x8[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x8[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=NS_luma_pred_mode core=RAM_1P_BRAM 
#if 0
#pragma HLS resource variable=cu_data_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].depth core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].ipm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].map_pb_tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mpm core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mv core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mvd core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].refi core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].reco_y core=RAM_1P_BRAM 
#endif
#pragma HLS resource variable=left_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].refi core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].num_refp core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_1P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_8x8[0].map_cu_mode_left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_8x8[0].map_cu_mode_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_8x8[0].map_scu_left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_8x8[0].map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block4x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x4MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=p_fenc_LCU_Y core=RAM_1P_BRAM 
#pragma HLS resource variable=rmd_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_list_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_mv1 core=RAM_2P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_refi1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_ptr1 core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs[0].refi core=RAM_2P_BRAM 
// #pragma HLS array_partition variable=cu_data_ptr[0].coef_y cyclic factor=64 dim=1
// #pragma HLS array_partition variable=cu_data_ptr[0].reco_y cyclic factor=64 dim=1
}
#endif

#if MD_KERNEL_16_DUMMY
#if USE_FACADE_MD_16
s64 md_kernel_16(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPD_RDO_CNT],
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status, ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, 
	pel fme2mdrefbuf[2560][8][2][16],
	strFetch_ref_window* Fetch_Ref_window_ptr)
#else
s64 md_kernel_16(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[16][16],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16* skip_in_16,
	U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, 
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_temp_ptr_16,
	ENC_FME_INFO_ALL_16* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, 
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16, MPM_ARRAY_SIZE<8, 16>* MPM_buffer_ptr_8x16, MPM_ARRAY_SIZE<16, 8>* MPM_buffer_ptr_16x8, 
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16, IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8,
	Arbitor& ref,
	strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	,SKIP_MERGE_INPUT* skip_in
#endif
#endif
)
{
	s64 dummy = MAX_S64 >> 4;
	U1 zero = (md_input_ptr->pix_x == 0x1fff);
 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    dummy+=md_fw_ptr[0] . affine_enable_flag;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    dummy+=md_fw_ptr[0] . max_part_ratio;
    dummy+=md_fw_ptr[0] . min_cu_size;
    dummy+=md_fw_ptr[0] . max_split_times;
    dummy+=md_fw_ptr[0] . min_qt_size;
    dummy+=md_fw_ptr[0] . max_bt_size;
    dummy+=md_fw_ptr[0] . max_eqt_size;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    // md_fw_ptr[0] . mipf_enable_flag is ignored ;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    dummy+=(md_fw_ptr[0] . refp_ptr[0][0])+(md_fw_ptr[0] . refp_ptr[0][1]/*TODO:*/);
    dummy+=(md_fw_ptr[0] . num_refp[0])+(md_fw_ptr[0] . num_refp[1]/*TODO:*/);
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    dummy+=md_input_ptr[0] . pix_x;
    dummy+=md_input_ptr[0] . pix_y;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . lambda_y;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    dummy+=(md_input_ptr[0] . CtrPos2MD[0][0][0])+(md_input_ptr[0] . CtrPos2MD[0][0][1]/*TODO:*/);
    dummy+=(md_input_ptr[0] . CtrPos2MD_ref1[0][0][0])+(md_input_ptr[0] . CtrPos2MD_ref1[0][0][1]/*TODO:*/);
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;

 // ::u8[5] rmd_mode; 
    // rmd_mode[0] is ignored ;

 // ::ME_MV_DIR_16* me_mv_dir; 
    dummy+=me_mv_dir[0] . block16x16MvDir[0][0] . lidx;
    // me_mv_dir[0] . block16x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x8MvDir[0][0] . lidx;
    // me_mv_dir[0] . block16x8MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block8x16MvDir[0][0] . lidx;
    // me_mv_dir[0] . block8x16MvDir[0][0] . cur_refi is ignored ;

 // ::pel[16][16] p_fenc_LCU_Y; 
    dummy+=p_fenc_LCU_Y[0][0];

 // struct ::ENC_ME_LINE_MAP_SIZE < 16 > {U32 map_scu_up_line[6];U32 map_cu_mode_up_line[6];U32 map_scu_left_line[4];U32 map_cu_mode_left_line[4];U11 min_scu_x;U11 min_scu_y;U1 above_line_idx;U1 curr_ctu_idx;U32 map_scu_line_hor[32];U32 map_cu_mode_line_hor[32];U32 map_scu_line_ver[16];U32 map_cu_mode_line_ver[16];}* me_line_map_ptr_16; 
    dummy+=(me_line_map_ptr_16[0] . map_scu_up_line[0])+(me_line_map_ptr_16[0] . map_scu_up_line[1]/*TODO:*/);
    // me_line_map_ptr_16[0] . map_cu_mode_up_line[0] is ignored ;
    dummy+=(me_line_map_ptr_16[0] . map_scu_left_line[0])+(me_line_map_ptr_16[0] . map_scu_left_line[1]/*TODO:*/);
    // me_line_map_ptr_16[0] . map_cu_mode_left_line[0] is ignored ;
    dummy+=me_line_map_ptr_16[0] . min_scu_x;
    dummy+=me_line_map_ptr_16[0] . min_scu_y;
    // me_line_map_ptr_16[0] . above_line_idx is ignored ;
    // me_line_map_ptr_16[0] . curr_ctu_idx is ignored ;
    // me_line_map_ptr_16[0] . map_scu_line_hor[0] is ignored ;
    // me_line_map_ptr_16[0] . map_cu_mode_line_hor[0] is ignored ;
    // me_line_map_ptr_16[0] . map_scu_line_ver[0] is ignored ;
    // me_line_map_ptr_16[0] . map_cu_mode_line_ver[0] is ignored ;

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[6] up_nebs; 
    dummy+=(up_nebs[0] . mv[0][0])+(up_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(up_nebs[0] . refi[0])+(up_nebs[0] . refi[1]/*TODO:*/);

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[4] left_nebs; 
    dummy+=(left_nebs[0] . mv[0][0])+(left_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(left_nebs[0] . refi[0])+(left_nebs[0] . refi[1]/*TODO:*/);

 // struct ::SKIP_MERGE_INPUT_16{u8 slice_type;int ptr;u8 umve_enable_flag;U13 pix_x;s16 refp_mv[2];S2 refp_refi;s16 refp_map_mv[2][2];S2 refp_map_refi[2][2];int refp_list_ptr1[2][17];int refp_ptr1[17][2];}* skip_in_16; 
    // skip_in_16[0] . slice_type is ignored ;
    // skip_in_16[0] . ptr is ignored ;
    // skip_in_16[0] . umve_enable_flag is ignored ;
    // skip_in_16[0] . pix_x is ignored ;
    // skip_in_16[0] . refp_mv[0] is ignored ;
    // skip_in_16[0] . refp_refi is ignored ;
    // skip_in_16[0] . refp_map_mv[0][0] is ignored ;
    // skip_in_16[0] . refp_map_refi[0][0] is ignored ;
    // skip_in_16[0] . refp_list_ptr1[0][0] is ignored ;
    // skip_in_16[0] . refp_ptr1[0][0] is ignored ;

 // ::U1 boundary; 
    dummy+=boundary;

 // ::U1 isNS_allow; 
    dummy+=isNS_allow;

 // ::U2[1] NS_luma_pred_mode; 
    NS_luma_pred_mode[0]=DUMMY_OUT(dummy);

 // ::U13 x0; 
    dummy+=x0;

 // ::U13 y0; 
    dummy+=y0;

 // ::U12 cup; 
    // cup is ignored ;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U4 cud; 
    dummy+=cud;

 // ::U3 qt_depth; 
    dummy+=qt_depth;

 // ::U3 bet_depth; 
    dummy+=bet_depth;

 // ::U2 cons_pred_mode; 
    dummy+=cons_pred_mode;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // struct ::ENC_CU_DATA_ARRAY_INNER_16{S14 mv[16][2][2];S16 mvd[16][2][2];int pb_part[16];int tb_part[16];U3 pred_mode[16];U8 mvr_idx[16];U8 umve_flag[16];U8 umve_idx[16];U8 skip_idx[16];u32 map_scu[16];U8 affine_flag[16];U8 smvd_flag[16];U32 map_cu_mode[16];U32 map_pb_tb_part[16];U4 depth[16];U8 ipf_flag[16];U8 mvp_from_hmvp_flag[16];U8 mpm[2][16];U8 mpm_ext[8][16];S8 ipm[2][16];S3 refi[16][2];int num_nz_coef[3][16];S16 coef_y[16][16];S16 coef_u[8][8];S16 coef_v[8][8];pel reco_y[16][16];pel reco_u[8][8];pel reco_v[8][8];U12 x_chroma;U12 y_chroma;U12 x_last;U12 y_last;BOOL last_child;}* cu_data_temp_ptr_16; 
#if MD_KERNEL_SET_DUMMY_VALUE
	for (int i = 0; i < (16 >> 2); i++) {
		for (int j = 0; j < (16 >> 2); j++) {
			for (int k = 0; k < REFP_NUM; k++) {
				for (int m = 0; m < MV_D; m++) {
					cu_data_temp_ptr_16[0] . mv[i * (16 >> 2) + j][k][m]= (zero ? dummy : 0);
					cu_data_temp_ptr_16[0] . mvd[i * (16 >> 2) + j][k][m]= (zero ? dummy : 0);
				}
			}
			cu_data_temp_ptr_16[0] . pb_part[i * (16 >> 2) + j]= (zero ? dummy : 0);
			cu_data_temp_ptr_16[0] . tb_part[i * (16 >> 2) + j]= (zero ? dummy : 0);
	    	cu_data_temp_ptr_16[0] . pred_mode[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . mvr_idx[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . umve_flag[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . umve_idx[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . skip_idx[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . map_scu[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . affine_flag[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . smvd_flag[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . map_cu_mode[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . map_pb_tb_part[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . depth[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . ipf_flag[i * (16 >> 2) + j]=(zero? dummy : 0);
	    	cu_data_temp_ptr_16[0] . mvp_from_hmvp_flag[i * (16 >> 2) + j]=(zero? dummy : 0);
			for (int k = 0; k < 2; k++) {
				cu_data_temp_ptr_16[0] . mpm[k][i * (16 >> 2) + j]= (zero ? dummy : 0);
				cu_data_temp_ptr_16[0] . ipm[k][i * (16 >> 2) + j]= (zero ? dummy : 0);
			}
			for (int k = 0; k < REFP_NUM; k++) {
				cu_data_temp_ptr_16[0] . refi[i * (16 >> 2) + j][k]= (zero ? dummy : 0);
			}
    		cu_data_temp_ptr_16[0] . num_nz_coef[0][i * (16 >> 2) + j]=(zero ? dummy : 0);
		}
	}
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
    		cu_data_temp_ptr_16[0] . coef_y[i][j]=(zero ? dummy : 0);
    		cu_data_temp_ptr_16[0] . reco_y[i][j]=(zero ? dummy : 0);
		}
	}
#else
    (cu_data_temp_ptr_16[0] . mv[0][0][0])=(cu_data_temp_ptr_16[0] . mv[0][0][1]/*TODO:*/)=dummy;
    (cu_data_temp_ptr_16[0] . mvd[0][0][0])=(cu_data_temp_ptr_16[0] . mvd[0][0][1]/*TODO:*/)=dummy;
    cu_data_temp_ptr_16[0] . pb_part[0]=dummy;
    cu_data_temp_ptr_16[0] . tb_part[0]=dummy;
    (cu_data_temp_ptr_16[0] . pred_mode[0])++;
    cu_data_temp_ptr_16[0] . mvr_idx[0]=dummy;
    cu_data_temp_ptr_16[0] . umve_flag[0]=dummy;
    cu_data_temp_ptr_16[0] . umve_idx[0]=dummy;
    cu_data_temp_ptr_16[0] . skip_idx[0]=dummy;
    (cu_data_temp_ptr_16[0] . map_scu[0])++;
    cu_data_temp_ptr_16[0] . affine_flag[0]=dummy;
    cu_data_temp_ptr_16[0] . smvd_flag[0]=dummy;
    cu_data_temp_ptr_16[0] . map_cu_mode[0]=dummy;
    (cu_data_temp_ptr_16[0] . map_pb_tb_part[0])++;
    cu_data_temp_ptr_16[0] . depth[0]=dummy;
    cu_data_temp_ptr_16[0] . ipf_flag[0]=dummy;
    cu_data_temp_ptr_16[0] . mvp_from_hmvp_flag[0]=dummy;
    (cu_data_temp_ptr_16[0] . mpm[0][0])=(cu_data_temp_ptr_16[0] . mpm[0][1]/*TODO:*/)=dummy;
    // cu_data_temp_ptr_16[0] . mpm_ext[0][0] is ignored ;
    (cu_data_temp_ptr_16[0] . ipm[0][0])=(cu_data_temp_ptr_16[0] . ipm[0][1]/*TODO:*/)=dummy;
    (cu_data_temp_ptr_16[0] . refi[0][0])=(cu_data_temp_ptr_16[0] . refi[0][1]/*TODO:*/)=dummy;
    cu_data_temp_ptr_16[0] . num_nz_coef[0][0]=dummy;
    (cu_data_temp_ptr_16[0] . coef_y[0][0])++;
    // cu_data_temp_ptr_16[0] . coef_u[0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . coef_v[0][0] is ignored ;
    cu_data_temp_ptr_16[0] . reco_y[0][0]=dummy;
#endif //MD_KERNEL_SET_DUMMY_VALUE
    // cu_data_temp_ptr_16[0] . reco_u[0][0] is ignored ;
    // cu_data_temp_ptr_16[0] . reco_v[0][0] is ignored ;
    dummy+=cu_data_temp_ptr_16[0] . x_chroma;
    dummy+=cu_data_temp_ptr_16[0] . y_chroma;
    // cu_data_temp_ptr_16[0] . x_last is ignored ;
    // cu_data_temp_ptr_16[0] . y_last is ignored ;
    dummy+=cu_data_temp_ptr_16[0] . last_child;

 // ::ENC_FME_INFO_ALL_16* fme_mv_info; 
    dummy+=(fme_mv_info[0] . sadTree16x16Fmv[0][0][0])+(fme_mv_info[0] . sadTree16x16Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree16x8Fmv[0][0][0])+(fme_mv_info[0] . sadTree16x8Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree8x16Fmv[0][0][0])+(fme_mv_info[0] . sadTree8x16Fmv[0][0][1]/*TODO:*/);

 // struct ::IP_ARRAY_SIZE < 16 , 16 > {pel up_line[32];pel left_line[32];pel top_left;pel rec_line_hor[32];pel rec_line_ver[32];}* IP_buffer_ptr_16x16; 
    dummy+=IP_buffer_ptr_16x16[0] . up_line[0];
    dummy+=IP_buffer_ptr_16x16[0] . left_line[0];
    // IP_buffer_ptr_16x16[0] . top_left is ignored ;
    // IP_buffer_ptr_16x16[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_16x16[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 8UL , 16 > {pel up_line[16];pel left_line[32];pel top_left;pel rec_line_hor[16];pel rec_line_ver[32];}* IP_buffer_ptr_8x16; 
    dummy+=IP_buffer_ptr_8x16[0] . up_line[0];
    dummy+=IP_buffer_ptr_8x16[0] . left_line[0];
    // IP_buffer_ptr_8x16[0] . top_left is ignored ;
    // IP_buffer_ptr_8x16[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_8x16[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 16 , 8UL > {pel up_line[32];pel left_line[16];pel top_left;pel rec_line_hor[32];pel rec_line_ver[16];}* IP_buffer_ptr_16x8; 
    dummy+=IP_buffer_ptr_16x8[0] . up_line[0];
    dummy+=IP_buffer_ptr_16x8[0] . left_line[0];
    // IP_buffer_ptr_16x8[0] . top_left is ignored ;
    // IP_buffer_ptr_16x8[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_16x8[0] . rec_line_ver[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 16 , 16 > {pel up;pel left;pel up_line[4];pel left_line[4];pel ipm_line_ver[4];pel ipm_line_hor[8];}* MPM_buffer_ptr_16x16; 
    dummy+=MPM_buffer_ptr_16x16[0] . up;
    dummy+=MPM_buffer_ptr_16x16[0] . left;
    // MPM_buffer_ptr_16x16[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_16x16[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_16x16[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_16x16[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 8UL , 16 > {pel up;pel left;pel up_line[2];pel left_line[4];pel ipm_line_ver[4];pel ipm_line_hor[4];}* MPM_buffer_ptr_8x16; 
    dummy+=MPM_buffer_ptr_8x16[0] . up;
    dummy+=MPM_buffer_ptr_8x16[0] . left;
    // MPM_buffer_ptr_8x16[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_8x16[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_8x16[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_8x16[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 16 , 8UL > {pel up;pel left;pel up_line[4];pel left_line[2];pel ipm_line_ver[2];pel ipm_line_hor[8];}* MPM_buffer_ptr_16x8; 
    dummy+=MPM_buffer_ptr_16x8[0] . up;
    dummy+=MPM_buffer_ptr_16x8[0] . left;
    // MPM_buffer_ptr_16x8[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_16x8[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_16x8[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_16x8[0] . ipm_line_hor[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 16 , 16 > {U32 map_scu_above_line[8];U32 map_scu_top_left;U32 map_scu_up_line[8];U32 map_scu_left_line_new[8];U32 map_scu_top_left_new;U32 cur_map_scu[4][4];}* IP_map_scu_ptr_16x16; 
    // IP_map_scu_ptr_16x16[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_16x16[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_16x16[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x16[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x16[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_16x16[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 8UL , 16 > {U32 map_scu_above_line[4];U32 map_scu_top_left;U32 map_scu_up_line[4];U32 map_scu_left_line_new[8];U32 map_scu_top_left_new;U32 cur_map_scu[2][4];}* IP_map_scu_ptr_8x16; 
    // IP_map_scu_ptr_8x16[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_8x16[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_8x16[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_8x16[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_8x16[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_8x16[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 16 , 8UL > {U32 map_scu_above_line[8];U32 map_scu_top_left;U32 map_scu_up_line[8];U32 map_scu_left_line_new[4];U32 map_scu_top_left_new;U32 cur_map_scu[4][2];}* IP_map_scu_ptr_16x8; 
    // IP_map_scu_ptr_16x8[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_16x8[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_16x8[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x8[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x8[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_16x8[0] . cur_map_scu[0][0] is ignored ;

 // ::pel[2560][8][2][16] fme2mdrefbuf; 
    //dummy+=fme2mdrefbuf[0][0][0][0];

 // ::strFetch_ref_window_16* Fetch_Ref_window_ptr; 
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0] . lidx[0];
    dummy+=Fetch_Ref_window_ptr[0] . cur_refi[0];
    dummy+=(Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0])+(Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][1]/*TODO:*/);
    dummy+=(Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0])+(Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][1]/*TODO:*/);

 // ::SKIP_MERGE_INPUT* skip_in; 
    dummy+=skip_in[0] . slice_type;
    dummy+=skip_in[0] . ptr;
    dummy+=skip_in[0] . umve_enable_flag;
    // skip_in[0] . refp_map_mv[0][0][0] is ignored ;
    // skip_in[0] . refp_map_refi[0][0] is ignored ;
    // skip_in[0] . pix_x is ignored ;
    dummy+=(skip_in[0] . refp_map_mv1[0][0][0][0])+(skip_in[0] . refp_map_mv1[0][0][0][1]/*TODO:*/);
    dummy+=skip_in[0] . refp_map_refi1[0][0][0][0];
    dummy+=skip_in[0] . refp_list_ptr1[0][0];
    dummy+=(skip_in[0] . refp_ptr1[0][0])+(skip_in[0] . refp_ptr1[0][1]/*TODO:*/);

	return DUMMY_OUT(dummy);

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].refp_is_library_picture core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].refp_pic_ptr core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x16[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x16[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x8[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x8[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x16[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x16[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x16[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x16[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x8[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x8[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x16[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x16[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=NS_luma_pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].depth core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].ipm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].map_pb_tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].mpm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].mv core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].mvd core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].refi core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr_16[0].umve_idx core=RAM_1P_BRAM 
//#pragma HLS resource variable=fme2mdrefbuf core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x16Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x8Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree8x16Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].refi core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].num_refp core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].CtrPos2MD core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].CtrPos2MD_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_16[0].map_scu_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_16[0].map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=p_fenc_LCU_Y core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_list_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_mv1 core=RAM_2P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_refi1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_ptr1 core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs[0].refi core=RAM_2P_BRAM 
}
#endif

#if MD_KERNEL_32_DUMMY
s64 md_kernel_32_lite(U8 width, U8 height, 
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
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
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS INLINE off
    int dummy = 0;
 // preproc: signal'NS_luma_pred_mode_ap_vld' is removed.
 // ::U8 width; 
    dummy+=width;

 // ::U8 height; 
    dummy+=height;

 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    dummy+=md_fw_ptr[0] . max_part_ratio;
    dummy+=md_fw_ptr[0] . min_cu_size;
    dummy+=md_fw_ptr[0] . max_split_times;
    dummy+=md_fw_ptr[0] . min_qt_size;
    dummy+=md_fw_ptr[0] . max_bt_size;
    dummy+=md_fw_ptr[0] . max_eqt_size;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . eipm_enable_flag;
    // md_fw_ptr[0] . mipf_enable_flag is ignored ;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    dummy+=md_fw_ptr[0] . refp_ptr[0][0];
    dummy+=(md_fw_ptr[0] . num_refp[0])+(md_fw_ptr[0] . num_refp[1]/*TODO:*/);
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    dummy+=md_input_ptr[0] . pix_x;
    dummy+=md_input_ptr[0] . pix_y;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . lambda_y;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;

 // ::u8[5] rmd_mode; 
    dummy+=rmd_mode[0];

 // ::ME_MV_DIR* me_mv_dir; 
    // me_mv_dir[0] . block64x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x64MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block32x32MvDir[0][0] . lidx;
    // me_mv_dir[0] . block32x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block64x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block32x16MvDir[0][0] . lidx;
    // me_mv_dir[0] . block32x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x32MvDir[0][0] . lidx;
    // me_mv_dir[0] . block16x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block64x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block64x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block64x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x8MvDir[0][0] . cur_refi is ignored ;

 // ::pel[4096] p_fenc_LCU_Y; 
    dummy+=p_fenc_LCU_Y[0][0];

 // ::ENC_ME_LINE_MAP* me_line_map_ptr; 
    dummy+= me_line_map_ptr_32[0] .map_scu_up_line[0];
    dummy+= me_line_map_ptr_32[0] .map_cu_mode_up_line[0];
    dummy+= me_line_map_ptr_32[0] . min_scu_x;
    dummy+= me_line_map_ptr_32[0] . min_scu_y;
    // me_line_map_ptr[0] . stride_in_map is ignored ;
    // me_line_map_ptr[0] . stride_in_line is ignored ;
    // me_line_map_ptr[0] . pic_width_in_scu is ignored ;
    // me_line_map_ptr[0] . pic_height_in_scu is ignored ;
    // me_line_map_ptr[0] . above_line_idx is ignored ;
    dummy+= me_line_map_ptr_32[0] . curr_ctu_idx;
    // me_line_map_ptr[0] . x_ctu is ignored ;
    // me_line_map_ptr[0] . y_ctu is ignored ;
    dummy+=(me_line_map_ptr_32[0] .map_scu_line_hor[0])+(me_line_map_ptr_32[0] .map_scu_line_hor[1]/*TODO:*/);
    dummy+= me_line_map_ptr_32[0] .map_cu_mode_line_hor[0];
    dummy += (me_line_map_ptr_32[0].map_scu_line_ver[0]) + (me_line_map_ptr_32[0].map_scu_line_ver[1]/*TODO:*/);
    dummy += me_line_map_ptr_32[0].map_cu_mode_line_ver[0];
    // me_line_map_ptr[0] . co_left_mv[0][0] is ignored ;
    // me_line_map_ptr[0] . co_left_refi[0] is ignored ;
    // me_line_map_ptr[0] . co_left_ptr_ref[0] is ignored ;
 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[2][16][16] inner_nebs; 
    dummy+=(left_nebs[0] . mv[0][0])+(left_nebs[0]. mv[0][1]/*TODO:*/);
    dummy+=(left_nebs[0]. refi[0])+(left_nebs[0] . refi[1]/*TODO:*/);

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[48] above_nebs_line; 
    dummy+=(up_nebs[0] . mv[0][0])+(up_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(up_nebs[0] . refi[0])+(up_nebs[0] . refi[1]/*TODO:*/);

 // ::SKIP_MERGE_INPUT* skip_in; 
    dummy+=skip_in[0] . slice_type;
    dummy+=skip_in[0] . ptr;
    dummy+=skip_in[0] . umve_enable_flag;
    // skip_in[0] . refp_map_mv[0][0][0] is ignored ;
    // skip_in[0] . refp_map_refi[0][0] is ignored ;
    dummy+=skip_in[0] . pix_x;
    dummy+=(skip_in[0] . refp_map_mv1[0][0][0][0])+(skip_in[0] . refp_map_mv1[0][0][0][1]/*TODO:*/);
    dummy+=skip_in[0] . refp_map_refi1[0][0][0][0];
    dummy+=skip_in[0] . refp_list_ptr1[0][0];
    dummy+=(skip_in[0] . refp_ptr1[0][0])+(skip_in[0] . refp_ptr1[0][1]/*TODO:*/);

 // ::U1 boundary; 
    dummy+=boundary;

 // ::U1 isNS_allow; 
    dummy+=isNS_allow;

 // ::U2* NS_luma_pred_mode; 
    NS_luma_pred_mode[0]=dummy;

 // ::U13 x0; 
    dummy+=x0;

 // ::U13 y0; 
    dummy+=y0;

 // ::U12 cup; 
    // cup is ignored ;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U4 cud; 
    dummy+=cud;

 // ::U3 qt_depth; 
    dummy+=qt_depth;

 // ::U3 bet_depth; 
    dummy+=bet_depth;

 // ::U2 cons_pred_mode; 
    dummy+=cons_pred_mode;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // ::ENC_CU_DATA_ARRAY* cu_data_temp_ptr; 
    (cu_data_ptr[0] . split_mode[0][0][0])++;
    (cu_data_ptr[0] . mv[0][0][0])=(cu_data_ptr[0] . mv[0][0][1]/*TODO:*/)=dummy+(cu_data_ptr[0] . mv[0][0][0]);
    (cu_data_ptr[0] . mvd[0][0][0])=(cu_data_ptr[0] . mvd[0][0][1]/*TODO:*/)=dummy;
    cu_data_ptr[0] . pb_part[0]=dummy;
    cu_data_ptr[0] . tb_part[0]=dummy;
    (cu_data_ptr[0] . pred_mode[0])++;
    cu_data_ptr[0] . mvr_idx[0]=dummy;
    cu_data_ptr[0] . umve_flag[0]=dummy;
    cu_data_ptr[0] . umve_idx[0]=dummy;
    cu_data_ptr[0] . skip_idx[0]=dummy;
    (cu_data_ptr[0] . map_scu[0])++;
    (cu_data_ptr[0] . affine_flag[0])++;
    cu_data_ptr[0] . smvd_flag[0]=dummy;
    cu_data_ptr[0] . map_cu_mode[0]=dummy;
    (cu_data_ptr[0] . map_pb_tb_part[0])++;
    cu_data_ptr[0] . depth[0]=dummy;
    cu_data_ptr[0] . ipf_flag[0]=dummy;
    cu_data_ptr[0] . mvp_from_hmvp_flag[0]=dummy;
    // cu_data_ptr[0] . inter_filter_flag[0] is ignored ;
    (cu_data_ptr[0] . mpm[0][0])=(cu_data_ptr[0] . mpm[0][1]/*TODO:*/)=dummy;
    // cu_data_ptr[0] . mpm_ext[0][0] is ignored ;
    (cu_data_ptr[0] . ipm[0][0])=(cu_data_ptr[0] . ipm[0][1]/*TODO:*/)=dummy+(cu_data_ptr[0] . ipm[0][0]);
    (cu_data_ptr[0] . refi[0][0])=(cu_data_ptr[0] . refi[0][1]/*TODO:*/)=dummy+(cu_data_ptr[0] . refi[0][0]);
    cu_data_ptr[0] . num_nz_coef[0][0]=dummy;
    (cu_data_ptr[0].coef_y[0][0])++;
    (cu_data_ptr[0].coef_y[1][0])++;
    (cu_data_ptr[0].coef_y[2][0])++;
    (cu_data_ptr[0].coef_y[3][0])++;
    (cu_data_ptr[0].coef_y[4][0])++;
    (cu_data_ptr[0].coef_y[5][0])++;
    (cu_data_ptr[0].coef_y[6][0])++;
    (cu_data_ptr[0].coef_y[7][0])++;
    (cu_data_ptr[0].coef_y[8][0])++;
    (cu_data_ptr[0].coef_y[9][0])++;
    (cu_data_ptr[0].coef_y[10][0])++;
    (cu_data_ptr[0].coef_y[11][0])++;
    (cu_data_ptr[0].coef_y[12][0])++;
    (cu_data_ptr[0].coef_y[13][0])++;
    (cu_data_ptr[0].coef_y[14][0])++;
    (cu_data_ptr[0].coef_y[15][0])++;
    (cu_data_ptr[0].coef_y[16][0])++;
    (cu_data_ptr[0].coef_y[17][0])++;
    (cu_data_ptr[0].coef_y[18][0])++;
    (cu_data_ptr[0].coef_y[19][0])++;
    (cu_data_ptr[0].coef_y[20][0])++;
    (cu_data_ptr[0].coef_y[21][0])++;
    (cu_data_ptr[0].coef_y[22][0])++;
    (cu_data_ptr[0].coef_y[23][0])++;
    (cu_data_ptr[0].coef_y[24][0])++;
    (cu_data_ptr[0].coef_y[25][0])++;
    (cu_data_ptr[0].coef_y[26][0])++;
    (cu_data_ptr[0].coef_y[27][0])++;
    (cu_data_ptr[0].coef_y[28][0])++;
    (cu_data_ptr[0].coef_y[29][0])++;
    (cu_data_ptr[0].coef_y[30][0])++;
    (cu_data_ptr[0].coef_y[31][0])++;
    // cu_data_ptr[0] . coef_u[0] is ignored ;
    // cu_data_ptr[0] . coef_v[0] is ignored ;
    cu_data_ptr[0].reco_y[0][0]=dummy;
    cu_data_ptr[0].reco_y[1][0]=dummy;
    cu_data_ptr[0].reco_y[2][0]=dummy;
    cu_data_ptr[0].reco_y[3][0]=dummy;
    cu_data_ptr[0].reco_y[4][0]=dummy;
    cu_data_ptr[0].reco_y[5][0]=dummy;
    cu_data_ptr[0].reco_y[6][0]=dummy;
    cu_data_ptr[0].reco_y[7][0]=dummy;
    cu_data_ptr[0].reco_y[8][0]=dummy;
    cu_data_ptr[0].reco_y[9][0]=dummy;
    cu_data_ptr[0].reco_y[10][0]=dummy;
    cu_data_ptr[0].reco_y[11][0]=dummy;
    cu_data_ptr[0].reco_y[12][0]=dummy;
    cu_data_ptr[0].reco_y[13][0]=dummy;
    cu_data_ptr[0].reco_y[14][0]=dummy;
    cu_data_ptr[0].reco_y[15][0]=dummy;
    cu_data_ptr[0].reco_y[16][0]=dummy;
    cu_data_ptr[0].reco_y[17][0]=dummy;
    cu_data_ptr[0].reco_y[18][0]=dummy;
    cu_data_ptr[0].reco_y[19][0]=dummy;
    cu_data_ptr[0].reco_y[20][0]=dummy;
    cu_data_ptr[0].reco_y[21][0]=dummy;
    cu_data_ptr[0].reco_y[22][0]=dummy;
    cu_data_ptr[0].reco_y[23][0]=dummy;
    cu_data_ptr[0].reco_y[24][0]=dummy;
    cu_data_ptr[0].reco_y[25][0]=dummy;
    cu_data_ptr[0].reco_y[26][0]=dummy;
    cu_data_ptr[0].reco_y[27][0]=dummy;
    cu_data_ptr[0].reco_y[28][0]=dummy;
    cu_data_ptr[0].reco_y[29][0]=dummy;
    cu_data_ptr[0].reco_y[30][0]=dummy;
    cu_data_ptr[0].reco_y[31][0]=dummy;
    // cu_data_ptr[0] . reco_u[0] is ignored ;
    // cu_data_ptr[0] . reco_v[0] is ignored ;
    dummy+=cu_data_ptr[0] . x_chroma;
    dummy+=cu_data_ptr[0] . y_chroma;
    // cu_data_ptr[0] . x_last is ignored ;
    // cu_data_ptr[0] . y_last is ignored ;
    dummy+=cu_data_ptr[0] . last_child;

 // ::ENC_FME_INFO_ALL* fme_mv_info; 
    // fme_mv_info[0] . sadTree64x64Fmv[0][0][0] is ignored ;
    dummy+=(fme_mv_info[0] . sadTree32x32Fmv[0][0][0])+(fme_mv_info[0] . sadTree32x32Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree64x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x16Fmv[0][0][0] is ignored ;
    dummy+=(fme_mv_info[0] . sadTree32x16Fmv[0][0][0])+(fme_mv_info[0] . sadTree32x16Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree16x32Fmv[0][0][0])+(fme_mv_info[0] . sadTree16x32Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree64x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x32Fmv[0][0][0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 32 , 32 > {pel up_line[64];pel left_line[64];pel top_left;pel rec_line_hor[64];pel rec_line_ver[64];}* IP_buffer_ptr_32x32; 
    dummy+=IP_buffer_ptr_32x32[0] . up_line[0];
    dummy+=IP_buffer_ptr_32x32[0] . left_line[0];
    dummy+=IP_buffer_ptr_32x32[0] . top_left;
    // IP_buffer_ptr_32x32[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_32x32[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 16 , 32 > {pel up_line[32];pel left_line[64];pel top_left;pel rec_line_hor[32];pel rec_line_ver[64];}* IP_buffer_ptr_16x32; 
    dummy+=IP_buffer_ptr_16x32[0] . up_line[0];
    dummy+=IP_buffer_ptr_16x32[0] . left_line[0];
    dummy+=IP_buffer_ptr_16x32[0] . top_left;
    // IP_buffer_ptr_16x32[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_16x32[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 32 , 16 > {pel up_line[64];pel left_line[32];pel top_left;pel rec_line_hor[64];pel rec_line_ver[32];}* IP_buffer_ptr_32x16; 
    dummy+=IP_buffer_ptr_32x16[0] . up_line[0];
    dummy+=IP_buffer_ptr_32x16[0] . left_line[0];
    dummy+=IP_buffer_ptr_32x16[0] . top_left;
    // IP_buffer_ptr_32x16[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_32x16[0] . rec_line_ver[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 32 , 32 > {pel up;pel left;pel up_line[8];pel left_line[8];pel ipm_line_ver[8];pel ipm_line_hor[16];}* MPM_buffer_ptr_32x32; 
    dummy+=MPM_buffer_ptr_32x32[0] . up;
    dummy+=MPM_buffer_ptr_32x32[0] . left;
    // MPM_buffer_ptr_32x32[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_32x32[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_32x32[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_32x32[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 16 , 32 > {pel up;pel left;pel up_line[4];pel left_line[8];pel ipm_line_ver[8];pel ipm_line_hor[8];}* MPM_buffer_ptr_16x32; 
    dummy+=MPM_buffer_ptr_16x32[0] . up;
    dummy+=MPM_buffer_ptr_16x32[0] . left;
    // MPM_buffer_ptr_16x32[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_16x32[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_16x32[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_16x32[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 32 , 16 > {pel up;pel left;pel up_line[8];pel left_line[4];pel ipm_line_ver[4];pel ipm_line_hor[16];}* MPM_buffer_ptr_32x16; 
    dummy+=MPM_buffer_ptr_32x16[0] . up;
    dummy+=MPM_buffer_ptr_32x16[0] . left;
    // MPM_buffer_ptr_32x16[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_32x16[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_32x16[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_32x16[0] . ipm_line_hor[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 32 , 32 > {U32 map_scu_above_line[16];U32 map_scu_top_left;U32 map_scu_up_line[16];U32 map_scu_left_line_new[16];U32 map_scu_top_left_new;U32 cur_map_scu[8][8];}* IP_map_scu_ptr_32x32; 
    // IP_map_scu_ptr_32x32[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_32x32[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_32x32[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_32x32[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_32x32[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_32x32[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 16 , 32 > {U32 map_scu_above_line[8];U32 map_scu_top_left;U32 map_scu_up_line[8];U32 map_scu_left_line_new[16];U32 map_scu_top_left_new;U32 cur_map_scu[4][8];}* IP_map_scu_ptr_16x32; 
    // IP_map_scu_ptr_16x32[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_16x32[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_16x32[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x32[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x32[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_16x32[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 32 , 16 > {U32 map_scu_above_line[16];U32 map_scu_top_left;U32 map_scu_up_line[16];U32 map_scu_left_line_new[8];U32 map_scu_top_left_new;U32 cur_map_scu[8][4];}* IP_map_scu_ptr_32x16; 
    // IP_map_scu_ptr_32x16[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_32x16[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_32x16[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_32x16[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_32x16[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_32x16[0] . cur_map_scu[0][0] is ignored ;

 // ::pel[2560][8][2][16] fme2mdrefbuf; 
    // fme2mdrefbuf[0][0][0][0] is ignored ;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    dummy+=Fetch_Ref_window_ptr[0] . RefWindowLCU[0];
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[0][0][0];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[0][0][1];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[0][1][0];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[1][0][0];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[1][1][0];
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0] . lidx[0];
    dummy+=Fetch_Ref_window_ptr[0] . cur_refi[0];
    // Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0] is ignored ;
	return dummy;

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x32[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x32[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x16[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x16[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x32[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x32[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x32[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x32[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x16[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x16[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x32[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x32[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=up_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs[0].refi core=RAM_2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].depth core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].ipm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].map_pb_tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mpm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mv core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mvd core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].refi core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].split_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_ptr[0].reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x32Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree32x16Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree32x32Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].refi core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].num_refp core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_1P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 

#pragma HLS resource variable=me_line_map_ptr_32[0].map_cu_mode_line_hor core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_32[0].map_scu_line_hor core=RAM_2P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_32[0].map_cu_mode_line_ver core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_32[0].map_scu_line_ver core=RAM_2P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_32[0].map_cu_mode_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_32[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block32x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block32x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=p_fenc_LCU_Y core=RAM_1P_BRAM 
#pragma HLS resource variable=rmd_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_list_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_mv1 core=RAM_2P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_refi1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_ptr1 core=RAM_2P_BRAM 
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].CtrPos complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].CtrPos complete dim=2
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].CtrPos complete dim=3
#pragma HLS array_partition variable=cu_data_ptr[0].coef_y cyclic factor=32 dim=1
#pragma HLS array_partition variable=cu_data_ptr[0].reco_y cyclic factor=32 dim=1
}
#endif //MD_KERNEL_32_DUMMY

#if MD_KERNEL_64_DUMMY
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
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS INLINE off
	int dummy = 0;
 // ::U7 w; 
    dummy+=w;

 // ::U7 h; 
    dummy+=h;

 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    dummy+=md_fw_ptr[0] . affine_enable_flag;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    dummy+=md_fw_ptr[0] . max_part_ratio;
    dummy+=md_fw_ptr[0] . min_cu_size;
    dummy+=md_fw_ptr[0] . max_split_times;
    dummy+=md_fw_ptr[0] . min_qt_size;
    dummy+=md_fw_ptr[0] . max_bt_size;
    dummy+=md_fw_ptr[0] . max_eqt_size;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . eipm_enable_flag;
    // md_fw_ptr[0] . mipf_enable_flag is ignored ;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    dummy+=(md_fw_ptr[0] . refp_ptr[0][0])+(md_fw_ptr[0] . refp_ptr[0][1]/*TODO:*/);
    dummy+=(md_fw_ptr[0] . num_refp[0])+(md_fw_ptr[0] . num_refp[1]/*TODO:*/);
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    dummy+=md_input_ptr[0] . pix_x;
    dummy+=md_input_ptr[0] . pix_y;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . lambda_y;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;

 // ::u8[5] rmd_mode; 
    dummy+=rmd_mode[0];

 // ::ME_MV_DIR* me_mv_dir; 
    dummy+=me_mv_dir[0] . block64x64MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x32MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block64x32MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x32MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block64x16MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x64MvDir[0][0] . lidx;
    // me_mv_dir[0] . block16x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block64x8MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x8MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block8x64MvDir[0][0] . lidx;
    // me_mv_dir[0] . block8x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x8MvDir[0][0] . cur_refi is ignored ;

 // ::pel[4096] p_fenc_LCU_Y; 
    dummy+=p_fenc_LCU_Y[0][0];

 // ::ENC_ME_LINE_MAP* me_line_map_ptr; 
    dummy+=me_line_map_ptr_64[0] .map_scu_up_line[0];
    dummy+=me_line_map_ptr_64[0] .map_cu_mode_up_line[0];
    dummy+=me_line_map_ptr_64[0] . min_scu_x;
    dummy+=me_line_map_ptr_64[0] . min_scu_y;
    // me_line_map_ptr_64[0] . stride_in_map is ignored ;
    // me_line_map_ptr_64[0] . stride_in_line is ignored ;
    // me_line_map_ptr_64[0] . pic_width_in_scu is ignored ;
    // me_line_map_ptr_64[0] . pic_height_in_scu is ignored ;
    // me_line_map_ptr_64[0] . above_line_idx is ignored ;
    dummy+=me_line_map_ptr_64[0] . curr_ctu_idx;
    // me_line_map_ptr_64[0] . x_ctu is ignored ;
    // me_line_map_ptr_64[0] . y_ctu is ignored ;
    dummy+=(me_line_map_ptr_64[0] .map_scu_left_line[0])+(me_line_map_ptr_64[0] .map_scu_left_line[0]/*TODO:*/);
    dummy+=me_line_map_ptr_64[0] .map_cu_mode_left_line[0];
    // me_line_map_ptr_64[0] . co_left_mv[0][0] is ignored ;
    // me_line_map_ptr_64[0] . co_left_refi[0] is ignored ;
    // me_line_map_ptr_64[0] . co_left_ptr_ref[0] is ignored ;

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[2][16][16] inner_nebs; 
    dummy+=(left_nebs[0]. mv[0][0])+(left_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(left_nebs[0]. refi[0])+(left_nebs[0] . refi[1]/*TODO:*/);

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[48] above_nebs_line; 
    dummy+=(up_nebs[0] . mv[0][0])+(up_nebs[0] . mv[0][1]/*TODO:*/);
    dummy+=(up_nebs[0] . refi[0])+(up_nebs[0] . refi[1]/*TODO:*/);

 // ::SKIP_MERGE_INPUT* skip_in; 
    dummy+=skip_in[0] . slice_type;
    dummy+=skip_in[0] . ptr;
    dummy+=skip_in[0] . umve_enable_flag;
    // skip_in[0] . refp_map_mv[0][0][0] is ignored ;
    // skip_in[0] . refp_map_refi[0][0] is ignored ;
    dummy+=skip_in[0] . pix_x;
    dummy+=(skip_in[0] . refp_map_mv1[0][0][0][0])+(skip_in[0] . refp_map_mv1[0][0][0][1]/*TODO:*/);
    dummy+=skip_in[0] . refp_map_refi1[0][0][0][0];
    dummy+=skip_in[0] . refp_list_ptr1[0][0];
    dummy+=(skip_in[0] . refp_ptr1[0][0])+(skip_in[0] . refp_ptr1[0][1]/*TODO:*/);

 // ::U1 boundary; 
    dummy+=boundary;

 // ::U1 isNS_allow; 
    dummy+=isNS_allow;

 // ::U2[1] NS_luma_pred_mode; 
    NS_luma_pred_mode[0]=dummy;

 // ::U13 x0; 
    dummy+=x0;

 // ::U13 y0; 
    dummy+=y0;

 // ::U12 cup; 
    // cup is ignored ;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U4 cud; 
    dummy+=cud;

 // ::U3 qt_depth; 
    dummy+=qt_depth;

 // ::U3 bet_depth; 
    dummy+=bet_depth;

 // ::U2 cons_pred_mode; 
    dummy+=cons_pred_mode;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // ::ENC_CU_DATA_ARRAY* cu_data_temp_ptr; 
    (cu_data_temp_ptr[0] . split_mode[0][0][0])++;
    (cu_data_temp_ptr[0] . mv[0][0][0])=(cu_data_temp_ptr[0] . mv[0][0][1]/*TODO:*/)=dummy+(cu_data_temp_ptr[0] . mv[0][0][0]);
    (cu_data_temp_ptr[0] . mvd[0][0][0])=(cu_data_temp_ptr[0] . mvd[0][0][1]/*TODO:*/)=dummy;
    cu_data_temp_ptr[0] . pb_part[0]=dummy;
    cu_data_temp_ptr[0] . tb_part[0]=dummy;
    (cu_data_temp_ptr[0] . pred_mode[0])++;
    cu_data_temp_ptr[0] . mvr_idx[0]=dummy;
    cu_data_temp_ptr[0] . umve_flag[0]=dummy;
    cu_data_temp_ptr[0] . umve_idx[0]=dummy;
    cu_data_temp_ptr[0] . skip_idx[0]=dummy;
    (cu_data_temp_ptr[0] . map_scu[0])++;
    (cu_data_temp_ptr[0] . affine_flag[0])++;
    cu_data_temp_ptr[0] . smvd_flag[0]=dummy;
    cu_data_temp_ptr[0] . map_cu_mode[0]=dummy;
    (cu_data_temp_ptr[0] . map_pb_tb_part[0])++;
    cu_data_temp_ptr[0] . depth[0]=dummy;
    cu_data_temp_ptr[0] . ipf_flag[0]=dummy;
    cu_data_temp_ptr[0] . mvp_from_hmvp_flag[0]=dummy;
    // cu_data_temp_ptr[0] . inter_filter_flag[0] is ignored ;
    (cu_data_temp_ptr[0] . mpm[0][0])=(cu_data_temp_ptr[0] . mpm[0][1]/*TODO:*/)=dummy;
    // cu_data_temp_ptr[0] . mpm_ext[0][0] is ignored ;
    (cu_data_temp_ptr[0] . ipm[0][0])=(cu_data_temp_ptr[0] . ipm[0][1]/*TODO:*/)=dummy+(cu_data_temp_ptr[0] . ipm[0][0]);
    (cu_data_temp_ptr[0] . refi[0][0])=(cu_data_temp_ptr[0] . refi[0][1]/*TODO:*/)=dummy+(cu_data_temp_ptr[0] . refi[0][0]);
    cu_data_temp_ptr[0] . num_nz_coef[0][0]=dummy;
    (cu_data_temp_ptr[0] . coef_y[0][0])++;
    // cu_data_temp_ptr[0] . coef_u[0] is ignored ;
    // cu_data_temp_ptr[0] . coef_v[0] is ignored ;
    cu_data_temp_ptr[0] . reco_y[0][0]=dummy;
    // cu_data_temp_ptr[0] . reco_u[0] is ignored ;
    // cu_data_temp_ptr[0] . reco_v[0] is ignored ;
    // cu_data_temp_ptr[0] . x_chroma is ignored ;
    // cu_data_temp_ptr[0] . y_chroma is ignored ;
    // cu_data_temp_ptr[0] . x_last is ignored ;
    // cu_data_temp_ptr[0] . y_last is ignored ;
    dummy+=cu_data_temp_ptr[0] . last_child;

 // ::ENC_FME_INFO_ALL* fme_mv_info; 
    dummy+=(fme_mv_info[0] . sadTree64x64Fmv[0][0][0])+(fme_mv_info[0] . sadTree64x64Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree32x32Fmv[0][0][0] is ignored ;
    dummy+=(fme_mv_info[0] . sadTree64x32Fmv[0][0][0])+(fme_mv_info[0] . sadTree64x32Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree32x64Fmv[0][0][0])+(fme_mv_info[0] . sadTree32x64Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree16x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x32Fmv[0][0][0] is ignored ;
    dummy+=(fme_mv_info[0] . sadTree64x16Fmv[0][0][0])+(fme_mv_info[0] . sadTree64x16Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree16x64Fmv[0][0][0])+(fme_mv_info[0] . sadTree16x64Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree8x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x32Fmv[0][0][0] is ignored ;
    dummy+=(fme_mv_info[0] . sadTree64x8Fmv[0][0][0])+(fme_mv_info[0] . sadTree64x8Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree8x64Fmv[0][0][0])+(fme_mv_info[0] . sadTree8x64Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree4x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x32Fmv[0][0][0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 64 , 64 > {pel up_line[128];pel left_line[64];pel top_left;}* IP_buffer_ptr_64x64; 
    dummy+=IP_buffer_ptr_64x64[0] . up_line[0];
    dummy+=IP_buffer_ptr_64x64[0] . left_line[0];
    dummy+=IP_buffer_ptr_64x64[0] . top_left;

 // struct ::IP_ARRAY_SIZE < 32 , 64 > {pel up_line[64];pel left_line[128];pel top_left;pel rec_line_hor[64];pel rec_line_ver[128];}* IP_buffer_ptr_32x64; 
    dummy+=IP_buffer_ptr_32x64[0] . up_line[0];
    dummy+=IP_buffer_ptr_32x64[0] . left_line[0];
    dummy+=IP_buffer_ptr_32x64[0] . top_left;
    // IP_buffer_ptr_32x64[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_32x64[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 64 , 32 > {pel up_line[128];pel left_line[64];pel top_left;pel rec_line_hor[128];pel rec_line_ver[64];}* IP_buffer_ptr_64x32; 
    dummy+=IP_buffer_ptr_64x32[0] . up_line[0];
    dummy+=IP_buffer_ptr_64x32[0] . left_line[0];
    dummy+=IP_buffer_ptr_64x32[0] . top_left;
    // IP_buffer_ptr_64x32[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_64x32[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 16 , 64 > {pel up_line[32];pel left_line[128];pel top_left;pel rec_line_hor[32];pel rec_line_ver[128];}* IP_buffer_ptr_16x64; 
    dummy+=IP_buffer_ptr_16x64[0] . up_line[0];
    dummy+=IP_buffer_ptr_16x64[0] . left_line[0];
    dummy+=IP_buffer_ptr_16x64[0] . top_left;
    // IP_buffer_ptr_16x64[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_16x64[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 64 , 16 > {pel up_line[128];pel left_line[32];pel top_left;pel rec_line_hor[128];pel rec_line_ver[32];}* IP_buffer_ptr_64x16; 
    dummy+=IP_buffer_ptr_64x16[0] . up_line[0];
    dummy+=IP_buffer_ptr_64x16[0] . left_line[0];
    dummy+=IP_buffer_ptr_64x16[0] . top_left;
    // IP_buffer_ptr_64x16[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_64x16[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 8UL , 64 > {pel up_line[16];pel left_line[128];pel top_left;pel rec_line_hor[16];pel rec_line_ver[128];}* IP_buffer_ptr_8x64; 
    dummy+=IP_buffer_ptr_8x64[0] . up_line[0];
    dummy+=IP_buffer_ptr_8x64[0] . left_line[0];
    dummy+=IP_buffer_ptr_8x64[0] . top_left;
    // IP_buffer_ptr_8x64[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_8x64[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 64 , 8UL > {pel up_line[128];pel left_line[16];pel top_left;pel rec_line_hor[128];pel rec_line_ver[16];}* IP_buffer_ptr_64x8; 
    dummy+=IP_buffer_ptr_64x8[0] . up_line[0];
    dummy+=IP_buffer_ptr_64x8[0] . left_line[0];
    dummy+=IP_buffer_ptr_64x8[0] . top_left;
    // IP_buffer_ptr_64x8[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_64x8[0] . rec_line_ver[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 64 , 64 > {pel up;pel left;pel up_line[16];pel left_line[16];pel ipm_line_ver[16];pel ipm_line_hor[32];}* MPM_buffer_ptr_64x64; 
    dummy+=MPM_buffer_ptr_64x64[0] . up;
    dummy+=MPM_buffer_ptr_64x64[0] . left;
    // MPM_buffer_ptr_64x64[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_64x64[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_64x64[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_64x64[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 32 , 64 > {pel up;pel left;pel up_line[8];pel left_line[16];pel ipm_line_ver[16];pel ipm_line_hor[16];}* MPM_buffer_ptr_32x64; 
    dummy+=MPM_buffer_ptr_32x64[0] . up;
    dummy+=MPM_buffer_ptr_32x64[0] . left;
    // MPM_buffer_ptr_32x64[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_32x64[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_32x64[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_32x64[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 64 , 32 > {pel up;pel left;pel up_line[16];pel left_line[8];pel ipm_line_ver[8];pel ipm_line_hor[32];}* MPM_buffer_ptr_64x32; 
    dummy+=MPM_buffer_ptr_64x32[0] . up;
    dummy+=MPM_buffer_ptr_64x32[0] . left;
    // MPM_buffer_ptr_64x32[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_64x32[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_64x32[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_64x32[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 16 , 64 > {pel up;pel left;pel up_line[4];pel left_line[16];pel ipm_line_ver[16];pel ipm_line_hor[8];}* MPM_buffer_ptr_16x64; 
    dummy+=MPM_buffer_ptr_16x64[0] . up;
    dummy+=MPM_buffer_ptr_16x64[0] . left;
    // MPM_buffer_ptr_16x64[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_16x64[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_16x64[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_16x64[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 64 , 16 > {pel up;pel left;pel up_line[16];pel left_line[4];pel ipm_line_ver[4];pel ipm_line_hor[32];}* MPM_buffer_ptr_64x16; 
    dummy+=MPM_buffer_ptr_64x16[0] . up;
    dummy+=MPM_buffer_ptr_64x16[0] . left;
    // MPM_buffer_ptr_64x16[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_64x16[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_64x16[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_64x16[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 8UL , 64 > {pel up;pel left;pel up_line[2];pel left_line[16];pel ipm_line_ver[16];pel ipm_line_hor[4];}* MPM_buffer_ptr_8x64; 
    dummy+=MPM_buffer_ptr_8x64[0] . up;
    dummy+=MPM_buffer_ptr_8x64[0] . left;
    // MPM_buffer_ptr_8x64[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_8x64[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_8x64[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_8x64[0] . ipm_line_hor[0] is ignored ;

 // struct ::MPM_ARRAY_SIZE < 64 , 8UL > {pel up;pel left;pel up_line[16];pel left_line[2];pel ipm_line_ver[2];pel ipm_line_hor[32];}* MPM_buffer_ptr_64x8; 
    dummy+=MPM_buffer_ptr_64x8[0] . up;
    dummy+=MPM_buffer_ptr_64x8[0] . left;
    // MPM_buffer_ptr_64x8[0] . up_line[0] is ignored ;
    // MPM_buffer_ptr_64x8[0] . left_line[0] is ignored ;
    // MPM_buffer_ptr_64x8[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_64x8[0] . ipm_line_hor[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 64 , 64 > {U32 map_scu_above_line[32];U32 map_scu_top_left;U32 map_scu_up_line[32];U32 map_scu_left_line_new[32];U32 map_scu_top_left_new;U32 cur_map_scu[16][16];}* IP_map_scu_ptr_64x64; 
    // IP_map_scu_ptr_64x64[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_64x64[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 32 , 64 > {U32 map_scu_above_line[16];U32 map_scu_top_left;U32 map_scu_up_line[16];U32 map_scu_left_line_new[32];U32 map_scu_top_left_new;U32 cur_map_scu[8][16];}* IP_map_scu_ptr_32x64; 
    // IP_map_scu_ptr_32x64[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_32x64[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_32x64[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_32x64[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_32x64[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_32x64[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 64 , 32 > {U32 map_scu_above_line[32];U32 map_scu_top_left;U32 map_scu_up_line[32];U32 map_scu_left_line_new[16];U32 map_scu_top_left_new;U32 cur_map_scu[16][8];}* IP_map_scu_ptr_64x32; 
    // IP_map_scu_ptr_64x32[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_64x32[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_64x32[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_64x32[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_64x32[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_64x32[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 16 , 64 > {U32 map_scu_above_line[8];U32 map_scu_top_left;U32 map_scu_up_line[8];U32 map_scu_left_line_new[32];U32 map_scu_top_left_new;U32 cur_map_scu[4][16];}* IP_map_scu_ptr_16x64; 
    // IP_map_scu_ptr_16x64[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_16x64[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_16x64[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x64[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x64[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_16x64[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 64 , 16 > {U32 map_scu_above_line[32];U32 map_scu_top_left;U32 map_scu_up_line[32];U32 map_scu_left_line_new[8];U32 map_scu_top_left_new;U32 cur_map_scu[16][4];}* IP_map_scu_ptr_64x16; 
    // IP_map_scu_ptr_64x16[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_64x16[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_64x16[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_64x16[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_64x16[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_64x16[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 8UL , 64 > {U32 map_scu_above_line[4];U32 map_scu_top_left;U32 map_scu_up_line[4];U32 map_scu_left_line_new[32];U32 map_scu_top_left_new;U32 cur_map_scu[2][16];}* IP_map_scu_ptr_8x64; 
    // IP_map_scu_ptr_8x64[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_8x64[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_8x64[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_8x64[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_8x64[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_8x64[0] . cur_map_scu[0][0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 64 , 8UL > {U32 map_scu_above_line[32];U32 map_scu_top_left;U32 map_scu_up_line[32];U32 map_scu_left_line_new[4];U32 map_scu_top_left_new;U32 cur_map_scu[16][2];}* IP_map_scu_ptr_64x8; 
    // IP_map_scu_ptr_64x8[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_64x8[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_64x8[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_64x8[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_64x8[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_64x8[0] . cur_map_scu[0][0] is ignored ;

 // ::pel[2560][8][2][16] fme2mdrefbuf; 
    // fme2mdrefbuf[0][0][0][0] is ignored ;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0] . lidx[0];
    dummy+=Fetch_Ref_window_ptr[0] . cur_refi[0];
    // Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0] is ignored ;
	return dummy;
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x16[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x16[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x32[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x32[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x8[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x8[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x64[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x64[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x16[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x16[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x32[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x32[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x8[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x8[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x64[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=NS_luma_pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=up_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs[0].refi core=RAM_2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].depth core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].ipm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].map_pb_tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].mpm core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].mv core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].mvd core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].refi core=RAM_T2P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].split_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=cu_data_temp_ptr[0].umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x64Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree32x64Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree64x16Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree64x32Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree64x64Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree64x8Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree8x64Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].mv core=RAM_2P_BRAM 
#pragma HLS resource variable=left_nebs[0].refi core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].num_refp core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_cu_mode_left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_scu_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_cu_mode_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=p_fenc_LCU_Y core=RAM_1P_BRAM 
#pragma HLS resource variable=rmd_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_list_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_mv1 core=RAM_2P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_refi1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_ptr1 core=RAM_2P_BRAM 
}
#endif //MD_KERNEL_64_DUMMY

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
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father)
{
	s64 dummy = 0;
	// dummy_template.cpp
 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    dummy+=md_fw_ptr[0] . affine_enable_flag;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    dummy+=md_fw_ptr[0] . log2_culine;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    dummy+=md_fw_ptr[0] . max_part_ratio;
    dummy+=md_fw_ptr[0] . min_cu_size;
    dummy+=md_fw_ptr[0] . max_split_times;
    dummy+=md_fw_ptr[0] . min_qt_size;
    dummy+=md_fw_ptr[0] . max_bt_size;
    dummy+=md_fw_ptr[0] . max_eqt_size;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    dummy+=(md_fw_ptr[0] . refp_ptr[0][0])+(md_fw_ptr[0] . refp_ptr[0][1]/*TODO:*/);
#if ENABLE_BFRAME
    dummy+=md_fw_ptr[0] . num_refp[0];
#endif
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    dummy+=md_input_ptr[0] . pix_x;
    dummy+=md_input_ptr[0] . pix_y;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . lambda_y;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    dummy+=(md_input_ptr[0] . CtrPos2MD[0][0][0])+(md_input_ptr[0] . CtrPos2MD[0][0][1]/*TODO:*/);
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;

 // ::RMD_OUTPUT* g_rmd_output; 
    dummy+=g_rmd_output[0] . modes_8x8[0][0];
    dummy+=g_rmd_output[0] . modes_16x8[0][0];
    dummy+=g_rmd_output[0] . modes_32x8[0][0];
    dummy+=g_rmd_output[0] . modes_64x8[0][0];
    dummy+=g_rmd_output[0] . modes_8x16[0][0];
    dummy+=g_rmd_output[0] . modes_16x16[0][0];
    dummy+=g_rmd_output[0] . modes_32x16[0][0];
    dummy+=g_rmd_output[0] . modes_64x16[0][0];
    dummy+=g_rmd_output[0] . modes_8x32[0][0];
    dummy+=g_rmd_output[0] . modes_16x32[0][0];
    dummy+=g_rmd_output[0] . modes_32x32[0][0];
    dummy+=g_rmd_output[0] . modes_64x32[0][0];
    dummy+=g_rmd_output[0] . modes_8x64[0][0];
    dummy+=g_rmd_output[0] . modes_16x64[0][0];
    dummy+=g_rmd_output[0] . modes_32x64[0][0];
    dummy+=g_rmd_output[0] . modes_64x64[0][0];

 // ::pel[4096] p_fenc_LCU_Y; 
    dummy+=p_fenc_LCU_Y[0];

 // ::pel[2048] p_fenc_LCU_UV; 
    // p_fenc_LCU_UV[0] is ignored ;

 // ::SKIP_MERGE_INPUT* skip_in; 
    // skip_in[0] . slice_type is ignored ;
    // skip_in[0] . ptr is ignored ;
    // skip_in[0] . umve_enable_flag is ignored ;
    dummy+=(skip_in[0] . refp_map_mv[0][0][0])+(skip_in[0] . refp_map_mv[0][0][1]/*TODO:*/);
    dummy+=skip_in[0] . refp_map_refi[0][0];
    dummy+=skip_in[0] . pix_x;
    // skip_in[0] . refp_map_mv1[0][0][0][0] is ignored ;
    // skip_in[0] . refp_map_refi1[0][0][0][0] is ignored ;
    // skip_in[0] . refp_list_ptr1[0][0] is ignored ;
    // skip_in[0] . refp_ptr1[0][0] is ignored ;

 // ::ENC_CU_DATA* prev_cu_data_ptr; 
    (prev_cu_data_ptr[0] . split_mode[0][0][0])=(prev_cu_data_ptr[0] . split_mode[0][0][1]/*TODO:*/)=dummy;
    prev_cu_data_ptr[0] . mv[0][0][0]=dummy;
    prev_cu_data_ptr[0] . mvd[0][0][0]=dummy;
    prev_cu_data_ptr[0] . pb_part[0]=dummy;
    prev_cu_data_ptr[0] . tb_part[0]=dummy;
    prev_cu_data_ptr[0] . pred_mode[0]=dummy;
    prev_cu_data_ptr[0] . mvr_idx[0]=dummy;
    prev_cu_data_ptr[0] . umve_flag[0]=dummy;
    prev_cu_data_ptr[0] . umve_idx[0]=dummy;
    prev_cu_data_ptr[0] . skip_idx[0]=dummy;
    prev_cu_data_ptr[0] . map_scu[0]=dummy;
    prev_cu_data_ptr[0] . affine_flag[0]=dummy;
    prev_cu_data_ptr[0] . smvd_flag[0]=dummy;
    prev_cu_data_ptr[0] . map_cu_mode[0]=dummy;
    prev_cu_data_ptr[0] . map_pb_tb_part[0]=dummy;
    prev_cu_data_ptr[0] . depth[0]=dummy;
    prev_cu_data_ptr[0] . ipf_flag[0]=dummy;
    prev_cu_data_ptr[0] . mvp_from_hmvp_flag[0]=dummy;
    (prev_cu_data_ptr[0] . mpm[0][0])=(prev_cu_data_ptr[0] . mpm[0][1]/*TODO:*/)=dummy;
    prev_cu_data_ptr[0] . mpm_ext[0][0]=dummy;
    prev_cu_data_ptr[0] . ipm[0][0]=dummy;
    prev_cu_data_ptr[0] . refi[0][0]=dummy;
    prev_cu_data_ptr[0] . num_nz_coef[0][0]=dummy;
    prev_cu_data_ptr[0] . coef_y[0][0]=dummy;
    // prev_cu_data_ptr[0] . coef_u[0] is ignored ;
    // prev_cu_data_ptr[0] . coef_v[0] is ignored ;
    prev_cu_data_ptr[0] . reco_y[0][0]=dummy;
    // prev_cu_data_ptr[0] . reco_u[0] is ignored ;
    // prev_cu_data_ptr[0] . reco_v[0] is ignored ;
    // prev_cu_data_ptr[0] . x_chroma is ignored ;
    // prev_cu_data_ptr[0] . y_chroma is ignored ;
    // prev_cu_data_ptr[0] . x_last is ignored ;
    // prev_cu_data_ptr[0] . y_last is ignored ;
    // prev_cu_data_ptr[0] . last_child is ignored ;

 // ::U3 qt_depth; 
    dummy+=qt_depth;

 // ::U3 bet_depth; 
    dummy+=bet_depth;

 // ::U2 cons_pred_mode; 
    dummy+=cons_pred_mode;

 // ::U2 tree_status; 
    // tree_status is ignored ;

 // ::U3 part_num; 
    dummy+=part_num;

 // ::COM_SPLIT_STRUCT split_struct; 
    // split_struct . part_count is ignored ;
    dummy+=split_struct.cud;
    // split_struct . width[0] is ignored ;
    // split_struct . height[0] is ignored ;
    dummy+=split_struct.log_cuw[3];
    dummy+=split_struct.log_cuh[3];
    dummy+=split_struct.x_pos[3];
    dummy+=split_struct.y_pos[3];
    dummy+=split_struct.cup[3];
    //dummy+=split_struct.split_allow[3];
    // split_struct . num_split[0] is ignored ;

 // ::U3 prev_up_log2_sub_cuw; 
    dummy+=prev_up_log2_sub_cuw;

 // ::ENC_FME_INFO_ALL* fme_mv_info; 
    // fme_mv_info[0] . sadTree64x64Fmv[0][0][0] is ignored ;
    dummy+=(fme_mv_info[0] . sadTree32x32Fmv[0][0][0])+(fme_mv_info[0] . sadTree32x32Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree64x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x64Fmv[0][0][0] is ignored ;
    dummy+=fme_mv_info[0] . sadTree16x16Fmv[0][0][0];
    dummy+=(fme_mv_info[0] . sadTree32x16Fmv[0][0][0])+(fme_mv_info[0] . sadTree32x16Fmv[0][0][1]/*TODO:*/);
    dummy+=(fme_mv_info[0] . sadTree16x32Fmv[0][0][0])+(fme_mv_info[0] . sadTree16x32Fmv[0][0][1]/*TODO:*/);
    // fme_mv_info[0] . sadTree64x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x8Fmv[0][0][0] is ignored ;
    dummy+=fme_mv_info[0] . sadTree16x8Fmv[0][0][0];
    dummy+=fme_mv_info[0] . sadTree8x16Fmv[0][0][0];
    // fme_mv_info[0] . sadTree32x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x32Fmv[0][0][0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 64 , 64 > {pel up_line[128];pel left_line[64];pel top_left;}* IP_buffer_ptr_64x64; 
    dummy+=IP_buffer_ptr_64x64[0] . up_line[0];
    dummy+=IP_buffer_ptr_64x64[0] . left_line[0];
    dummy+=IP_buffer_ptr_64x64[0] . top_left;

 // struct ::IP_ARRAY{pel temp_buffer_chroma[2][32][32];pel up_chroma[2][1920];pel left_chroma[2][32];pel top_left_chroma[2];}* IP_buffer_ptr; 
    // IP_buffer_ptr[0] . temp_buffer_chroma[0][0][0] is ignored ;
    // IP_buffer_ptr[0] . up_chroma[0][0] is ignored ;
    // IP_buffer_ptr[0] . left_chroma[0][0] is ignored ;
    // IP_buffer_ptr[0] . top_left_chroma[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 64 , 64 > {U32 map_scu_above_line[32];U32 map_scu_top_left;U32 map_scu_left_line[32];U32 map_scu_inner_up[32];U32 map_scu_inner_left[32];U32 map_scu_up_line[32];U32 map_scu_left_line_new[32];U32 map_scu_top_left_new;U32 cur_map_scu[16][16];}* IP_map_scu_ptr_64x64; 
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_above_line[0];
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_top_left;
    // IP_map_scu_ptr_64x64[0] . map_scu_left_line[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_inner_up[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_inner_left[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_up_line[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_left_line_new[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_top_left_new is ignored ;
    // IP_map_scu_ptr_64x64[0] . cur_map_scu[0][0] is ignored ;

 // ::pel[2560][8][2][16] fme2mdrefbuf; 
    // fme2mdrefbuf[0][0][0][0] is ignored ;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    dummy+=Fetch_Ref_window_ptr[0] . RefWindowLCU[0];
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[0][0][0];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[0][0][1];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[0][1][0];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[1][0][0];
    dummy+=Fetch_Ref_window_ptr[0].CtrPos[1][1][0];
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . code_stru is ignored ;
    // Fetch_Ref_window_ptr[0] . lidx[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . cur_refi[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0] is ignored ;

 // int split_mode_father; 
    // split_mode_father is ignored ;

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64[0].map_scu_above_line core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x16Fmv core=RAM_1P_BRAM 
//#pragma HLS resource variable=fme_mv_info[0].sadTree16x32Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x8Fmv core=RAM_1P_BRAM 
//#pragma HLS resource variable=fme_mv_info[0].sadTree32x16Fmv core=RAM_2P_BRAM 
//#pragma HLS resource variable=fme_mv_info[0].sadTree32x32Fmv core=RAM_2P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree8x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].CtrPos2MD core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
#pragma HLS resource variable=p_fenc_LCU_Y core=RAM_1P_BRAM 
#if 0
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].depth core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].ipm core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].map_pb_tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].mpm core=RAM_T2P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].mpm_ext core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].mv core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].mvd core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].refi core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].split_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr[0].umve_idx core=RAM_1P_BRAM 
#endif
//#pragma HLS resource variable=skip_in[0].refp_map_mv core=RAM_2P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=split_struct.cup core=RAM_1P_BRAM 
#pragma HLS resource variable=split_struct.log_cuh core=RAM_1P_BRAM 
#pragma HLS resource variable=split_struct.log_cuw core=RAM_1P_BRAM 
#pragma HLS resource variable=split_struct.split_allow core=RAM_1P_BRAM 
#pragma HLS resource variable=split_struct.x_pos core=RAM_1P_BRAM 
#pragma HLS resource variable=split_struct.y_pos core=RAM_1P_BRAM 
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].CtrPos complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].CtrPos complete dim=2
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].CtrPos complete dim=3
//#pragma HLS array_partition variable=split_struct.cud complete dim=1
#pragma HLS array_partition variable=split_struct.cup cyclic factor=4 dim=1
#pragma HLS array_partition variable=split_struct.log_cuh cyclic factor=4 dim=1
#pragma HLS array_partition variable=split_struct.log_cuw cyclic factor=4 dim=1
#pragma HLS array_partition variable=split_struct.split_allow cyclic factor=4 dim=1
#pragma HLS array_partition variable=split_struct.x_pos cyclic factor=4 dim=1
#pragma HLS array_partition variable=split_struct.y_pos cyclic factor=4 dim=1
	s64 cost_best = dummy;
	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}
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
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	//md2_best_result_ptr->cost_best = MAX_S64;
	s64 dummy = 0;
	// dummy_template.cpp
 // preproc: signal 'md2_best_result_ptr_cost_best_i' is renamed into 'md2_best_result_ptr_cost_best' for R.
 // preproc: signal 'md2_best_result_ptr_cost_best_o' is merged into 'md2_best_result_ptr_cost_best' for W
 // preproc: signal'md2_best_result_ptr_cost_best_o_o_ap_vld' is removed.
 // preproc: signal'md2_best_result_ptr_cost_best_o_ap_vld' is removed.
 // preproc: signal'md2_best_result_ptr_best_split_mode_ap_vld' is removed.
 // preproc: signal'md2_best_result_ptr_best_cons_pred_mode_ap_vld' is removed.
 // ::U2 split_mode; 
    dummy+=split_mode;

 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    dummy+=md_fw_ptr[0] . affine_enable_flag;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    dummy+=md_fw_ptr[0] . log2_max_cuwh;
    dummy+=md_fw_ptr[0] . log2_culine;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    dummy+=md_fw_ptr[0] . max_part_ratio;
    dummy+=md_fw_ptr[0] . min_cu_size;
    dummy+=md_fw_ptr[0] . max_split_times;
    dummy+=md_fw_ptr[0] . min_qt_size;
    dummy+=md_fw_ptr[0] . max_bt_size;
    dummy+=md_fw_ptr[0] . max_eqt_size;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . eipm_enable_flag;
    // md_fw_ptr[0] . mipf_enable_flag is ignored ;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    dummy+=(md_fw_ptr[0] . refp_ptr[0][0])+(md_fw_ptr[0] . refp_ptr[0][1]/*TODO:*/);
    dummy+=(md_fw_ptr[0] . num_refp[0])+(md_fw_ptr[0] . num_refp[1]/*TODO:*/);
    dummy+=md_fw_ptr[0] . cur_ptr;

 // ::MD_INPUT* md_input_ptr; 
    dummy+=md_input_ptr[0] . pix_x;
    dummy+=md_input_ptr[0] . pix_y;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0] . lambda[0];
    dummy+=md_input_ptr[0] . lambda_y;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    dummy+=md_input_ptr[0] . qp_y;
    // md_input_ptr[0] . qp_u is ignored ;
    // md_input_ptr[0] . qp_v is ignored ;
    // md_input_ptr[0] . dist_chroma_weight[0] is ignored ;
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    dummy+=(md_input_ptr[0] . CtrPos2MD[0][0][0])+(md_input_ptr[0] . CtrPos2MD[0][0][1]/*TODO:*/);
    dummy+=(md_input_ptr[0] . CtrPos2MD_ref1[0][0][0])+(md_input_ptr[0] . CtrPos2MD_ref1[0][0][1]/*TODO:*/);
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;

 // ::RMD_OUTPUT* g_rmd_output; 
    dummy+=g_rmd_output[0] . modes_8x8[0][0];
    dummy+=g_rmd_output[0] . modes_16x8[0][0];
    dummy+=g_rmd_output[0] . modes_32x8[0][0];
    dummy+=g_rmd_output[0] . modes_64x8[0][0];
    dummy+=g_rmd_output[0] . modes_8x16[0][0];
    dummy+=g_rmd_output[0] . modes_16x16[0][0];
    dummy+=g_rmd_output[0] . modes_32x16[0][0];
    dummy+=g_rmd_output[0] . modes_64x16[0][0];
    dummy+=g_rmd_output[0] . modes_8x32[0][0];
    dummy+=g_rmd_output[0] . modes_16x32[0][0];
    dummy+=g_rmd_output[0] . modes_32x32[0][0];
    dummy+=g_rmd_output[0] . modes_64x32[0][0];
    dummy+=g_rmd_output[0] . modes_8x64[0][0];
    dummy+=g_rmd_output[0] . modes_16x64[0][0];
    dummy+=g_rmd_output[0] . modes_32x64[0][0];
    dummy+=g_rmd_output[0] . modes_64x64[0][0];

 // ::pel[12288] RefWindowLCU_UP; 
    // RefWindowLCU_UP[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP; 
    // RefWindowLCU_VP[0] is ignored ;

 // ::S14[2][2][2] CtrPos; 
    // CtrPos[0][0][0] is ignored ;

 // ::pel[12288] RefWindowLCU_UP_ref1; 
    // RefWindowLCU_UP_ref1[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP_ref1; 
    // RefWindowLCU_VP_ref1[0] is ignored ;

 // ::S14[2][2][2] CtrPos_ref1; 
    // CtrPos_ref1[0][0][0] is ignored ;

 // ::ME_MV_DIR* me_mv_dir; 
    dummy+=me_mv_dir[0] . block64x64MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x64MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block32x32MvDir[0][0] . lidx;
    // me_mv_dir[0] . block32x32MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block64x32MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x64MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x16MvDir[0][0] . lidx;
    dummy+=me_mv_dir[0] . block16x16MvDir[0][0] . cur_refi;
    dummy+=me_mv_dir[0] . block32x16MvDir[0][0] . lidx;
    // me_mv_dir[0] . block32x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x32MvDir[0][0] . lidx;
    // me_mv_dir[0] . block16x32MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block64x16MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x16MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x64MvDir[0][0] . lidx;
    // me_mv_dir[0] . block16x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x8MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block16x8MvDir[0][0] . lidx;
    dummy+=me_mv_dir[0] . block16x8MvDir[0][0] . cur_refi;
    dummy+=me_mv_dir[0] . block8x16MvDir[0][0] . lidx;
    dummy+=me_mv_dir[0] . block8x16MvDir[0][0] . cur_refi;
    // me_mv_dir[0] . block32x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x8MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x32MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block64x8MvDir[0][0] . lidx;
    // me_mv_dir[0] . block64x8MvDir[0][0] . cur_refi is ignored ;
    dummy+=me_mv_dir[0] . block8x64MvDir[0][0] . lidx;
    // me_mv_dir[0] . block8x64MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block32x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x32MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block16x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x16MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block8x4MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block8x4MvDir[0][0] . cur_refi is ignored ;
    // me_mv_dir[0] . block4x8MvDir[0][0] . lidx is ignored ;
    // me_mv_dir[0] . block4x8MvDir[0][0] . cur_refi is ignored ;

 // ::pel[4096] p_fenc_LCU_Y; 
    dummy+=p_fenc_LCU_Y[0];

 // struct ::ENC_ME_LINE_MAP_SIZE < 64 > {U32 map_scu_up_line[18];U32 map_cu_mode_up_line[18];U32 map_scu_left_line[16];U32 map_cu_mode_left_line[16];U11 min_scu_x;U11 min_scu_y;U1 above_line_idx;U1 curr_ctu_idx;U32 map_scu_line_hor[128];U32 map_cu_mode_line_hor[128];U32 map_scu_line_ver[64];U32 map_cu_mode_line_ver[64];}* me_line_map_ptr_64; 
    dummy+=me_line_map_ptr_64[0] . map_scu_up_line[0];
    dummy+=me_line_map_ptr_64[0] . map_cu_mode_up_line[0];
    dummy+=me_line_map_ptr_64[0] . map_scu_left_line[0];
    dummy+=me_line_map_ptr_64[0] . map_cu_mode_left_line[0];
    dummy+=me_line_map_ptr_64[0] . min_scu_x;
    dummy+=me_line_map_ptr_64[0] . min_scu_y;
    dummy+=me_line_map_ptr_64[0] . above_line_idx;
    dummy+=me_line_map_ptr_64[0] . curr_ctu_idx;
    // me_line_map_ptr_64[0] . map_scu_line_hor[0] is ignored ;
    // me_line_map_ptr_64[0] . map_cu_mode_line_hor[0] is ignored ;
    // me_line_map_ptr_64[0] . map_scu_line_ver[0] is ignored ;
    // me_line_map_ptr_64[0] . map_cu_mode_line_ver[0] is ignored ;

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[18] up_nebs_father; 
    dummy+=up_nebs_father[0] . mv[0][0];
    dummy+=up_nebs_father[0] . refi[0];

 // struct ::ENC_ME_NEB_INFO{S14 mv[2][2];S2 refi[2];}[16] left_nebs_father; 
    dummy+=left_nebs_father[0] . mv[0][0];
    dummy+=left_nebs_father[0] . refi[0];

 // ::SKIP_MERGE_INPUT* skip_in; 
    dummy+=skip_in[0] . slice_type;
    dummy+=skip_in[0] . ptr;
    dummy+=skip_in[0] . umve_enable_flag;
    // skip_in[0] . refp_map_mv[0][0][0] is ignored ;
    // skip_in[0] . refp_map_refi[0][0] is ignored ;
    dummy+=skip_in[0] . pix_x;
    dummy+=(skip_in[0] . refp_map_mv1[0][0][0][0])+(skip_in[0] . refp_map_mv1[0][0][0][1]/*TODO:*/);
    dummy+=skip_in[0] . refp_map_refi1[0][0][0][0];
    dummy+=skip_in[0] . refp_list_ptr1[0][0];
    dummy+=(skip_in[0] . refp_ptr1[0][0])+(skip_in[0] . refp_ptr1[0][1]/*TODO:*/);

 // ::MD_RESULT* md2_best_result_ptr; 
    (md2_best_result_ptr[0] . cost_best)++;
    md2_best_result_ptr[0] . best_split_mode=dummy;
    md2_best_result_ptr[0] . best_cons_pred_mode=dummy;

 // ::ENC_FME_INFO_ALL* fme_mv_info; 
    // fme_mv_info[0] . sadTree64x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x64Fmv[0][0][0] is ignored ;
    dummy+=fme_mv_info[0] . sadTree16x16Fmv[0][0][0];
    // fme_mv_info[0] . sadTree32x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x8Fmv[0][0][0] is ignored ;
    dummy+=fme_mv_info[0] . sadTree16x8Fmv[0][0][0];
    dummy+=fme_mv_info[0] . sadTree8x16Fmv[0][0][0];
    // fme_mv_info[0] . sadTree32x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x32Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree64x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x64Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x8Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree8x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree16x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x16Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree32x4Fmv[0][0][0] is ignored ;
    // fme_mv_info[0] . sadTree4x32Fmv[0][0][0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 64 , 64 > {pel up_line[128];pel left_line[64];pel top_left;}* IP_buffer_ptr_64x64; 
    dummy+=IP_buffer_ptr_64x64[0] . up_line[0];
    dummy+=IP_buffer_ptr_64x64[0] . left_line[0];
    // IP_buffer_ptr_64x64[0] . top_left is ignored ;

 // struct ::MPM_ARRAY_SIZE < 64 , 64 > {pel up;pel left;pel up_line[16];pel left_line[16];pel ipm_line_ver[16];pel ipm_line_hor[32];}* MPM_buffer_ptr_64x64; 
    // MPM_buffer_ptr_64x64[0] . up is ignored ;
    // MPM_buffer_ptr_64x64[0] . left is ignored ;
    dummy+=MPM_buffer_ptr_64x64[0] . up_line[0];
    dummy+=MPM_buffer_ptr_64x64[0] . left_line[0];
    // MPM_buffer_ptr_64x64[0] . ipm_line_ver[0] is ignored ;
    // MPM_buffer_ptr_64x64[0] . ipm_line_hor[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 64 , 64 > {U32 map_scu_above_line[32];U32 map_scu_top_left;U32 map_scu_up_line[32];U32 map_scu_left_line_new[32];U32 map_scu_top_left_new;U32 cur_map_scu[16][16];}* IP_map_scu_ptr_64x64; 
    // IP_map_scu_ptr_64x64[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_64x64[0] . map_scu_top_left is ignored ;
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_64x64[0] . map_scu_top_left_new;
    (IP_map_scu_ptr_64x64[0] . cur_map_scu[0][0])++;

 // ::ENC_CU_DATA* md2_best_ctu_output_ptr; 
    md2_best_ctu_output_ptr[0] . split_mode[0][0][0]=dummy;
    md2_best_ctu_output_ptr[0] . pb_part[0]=dummy;
    md2_best_ctu_output_ptr[0] . tb_part[0]=dummy;
    md2_best_ctu_output_ptr[0] . pred_mode[0]=dummy;
    (md2_best_ctu_output_ptr[0] . mpm[0][0])=(md2_best_ctu_output_ptr[0] . mpm[0][1]/*TODO:*/)=dummy;
    md2_best_ctu_output_ptr[0] . mpm_ext[0][0]=dummy;
    md2_best_ctu_output_ptr[0] . ipm[0][0]=dummy;
    md2_best_ctu_output_ptr[0] . refi[0][0]=dummy;
    md2_best_ctu_output_ptr[0] . mvr_idx[0]=dummy;
    md2_best_ctu_output_ptr[0] . umve_flag[0]=dummy;
    md2_best_ctu_output_ptr[0] . umve_idx[0]=dummy;
    md2_best_ctu_output_ptr[0] . skip_idx[0]=dummy;
    md2_best_ctu_output_ptr[0] . mvp_from_hmvp_flag[0]=dummy;
    md2_best_ctu_output_ptr[0] . mv[0][0][0]=dummy;
    md2_best_ctu_output_ptr[0] . mvd[0][0][0]=dummy;
    md2_best_ctu_output_ptr[0] . num_nz_coef[0][0]=dummy;
    md2_best_ctu_output_ptr[0] . map_scu[0]=dummy;
    md2_best_ctu_output_ptr[0] . affine_flag[0]=dummy;
    md2_best_ctu_output_ptr[0] . smvd_flag[0]=dummy;
    md2_best_ctu_output_ptr[0] . map_cu_mode[0]=dummy;
    md2_best_ctu_output_ptr[0] . map_pb_tb_part[0]=dummy;
    md2_best_ctu_output_ptr[0] . depth[0]=dummy;
    md2_best_ctu_output_ptr[0] . ipf_flag[0]=dummy;
    // md2_best_ctu_output_ptr[0] . coef[0][0] is ignored ;
    md2_best_ctu_output_ptr[0] . coef_y[0][0]=dummy;
    // md2_best_ctu_output_ptr[0] . coef_u[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . coef_v[0] is ignored ;
    md2_best_ctu_output_ptr[0] . reco_y[0][0]=dummy;
    // md2_best_ctu_output_ptr[0] . reco_u[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . reco_v[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sawp_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sawp_idx0[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sawp_idx1[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_mvr_flag0[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_mvr_idx0[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_mvr_flag1[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_mvr_idx1[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . bvr_idx[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . inter_filter_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . ipc_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . bgc_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . bgc_idx[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . ibc_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . cbvp_idx[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . cnt_hbvp_cands[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . map_usp[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sub_string_no[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . is_matched is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . length is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . offset_x is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . offset_y is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . match_dict[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . pixel[0][0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . n_recent_flag is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_strInfo[0] . n_recent_idx is ignored ;
    // md2_best_ctu_output_ptr[0] . sp_copy_direction[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . is_sp_pix_completed[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . cs2_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_copy_direction[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_sub_string_no[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . is_matched is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . length is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . pixel[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . match_type is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . srb_index is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . pv_type is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . pvflag is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . srb_bits is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . srb_dist is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . esc_flag is ignored ;
    // md2_best_ctu_output_ptr[0] . evs_str_copy_info[0] . pos is ignored ;
    // md2_best_ctu_output_ptr[0] . unpred_pix_num[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . unpred_pix_info[0] . Y is ignored ;
    // md2_best_ctu_output_ptr[0] . unpred_pix_info[0] . U is ignored ;
    // md2_best_ctu_output_ptr[0] . unpred_pix_info[0] . V is ignored ;
    // md2_best_ctu_output_ptr[0] . equal_val_str_present_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . unpredictable_pix_present_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pvbuf_size[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pvbuf_size_prev[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . p_SRB[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pvbuf_reused_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . p_SRB_prev[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . all_comp_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . all_comp_pre_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . m_dpb_idx[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . m_dpb_idx_prev[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . m_dpb_reYonly[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . m_dpb_reYonly_prev[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . cuS_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . cuS_pre_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pv_x[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pv_x_prev[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pv_y[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . pv_y_prev[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . srb_u[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . srb_v[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . affine_umve_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . affine_umve_idx[0][0] is ignored ;
    // md2_best_ctu_output_ptr[0] . etmvp_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . mvap_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sub_tmvp_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_idx0[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . awp_idx1[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . st_chroma_tu_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . iip_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . ist_tu_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . est_tu_flag[0] is ignored ;
    // md2_best_ctu_output_ptr[0] . sawp_mpm[0][0] is ignored ;
    // md2_best_ctu_output_ptr[0] . y_org_sao[0][0] is ignored ;
    // md2_best_ctu_output_ptr[0] . u_org_sao[0][0] is ignored ;
    // md2_best_ctu_output_ptr[0] . v_org_sao[0][0] is ignored ;

 // Arbitor& ref; 

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0] . lidx[0];
    dummy+=Fetch_Ref_window_ptr[0] . cur_refi[0];
    dummy+=Fetch_Ref_window_ptr[0] . refp_pic_ptr[0][0];
    dummy+=Fetch_Ref_window_ptr[0] . refp_is_library_picture[0][0];
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0] is ignored ;

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].refp_is_library_picture core=RAM_1P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].refp_pic_ptr core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64[0].cur_map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64[0].map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=MPM_buffer_ptr_64x64[0].left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=MPM_buffer_ptr_64x64[0].up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree16x8Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=fme_mv_info[0].sadTree8x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_16x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_32x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_64x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=g_rmd_output[0].modes_8x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=left_nebs_father[0].mv core=RAM_1P_BRAM 
#pragma HLS resource variable=left_nebs_father[0].refi core=RAM_1P_BRAM 
#if 0
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].depth core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].ipm core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].map_pb_tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].mpm core=RAM_T2P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].mpm_ext core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].mv core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].mvd core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].refi core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].smvd_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].split_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md2_best_ctu_output_ptr[0].umve_idx core=RAM_1P_BRAM 
#endif
#pragma HLS resource variable=md_fw_ptr[0].num_refp core=RAM_2P_BRAM 
#pragma HLS resource variable=md_fw_ptr[0].refp_ptr core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].CtrPos2MD core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].CtrPos2MD_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=md_input_ptr[0].lambda core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_cu_mode_left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_cu_mode_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_scu_left_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_line_map_ptr_64[0].map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x8MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block16x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block32x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block32x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block64x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=me_mv_dir[0].block8x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=p_fenc_LCU_Y core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_list_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_mv1 core=RAM_2P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_map_refi1 core=RAM_1P_BRAM 
#pragma HLS resource variable=skip_in[0].refp_ptr1 core=RAM_2P_BRAM 
#pragma HLS resource variable=up_nebs_father[0].mv core=RAM_1P_BRAM 
#pragma HLS resource variable=up_nebs_father[0].refi core=RAM_1P_BRAM 
	md2_best_result_ptr->cost_best = dummy;
}
#endif //CIRCUIT64_DUMMY

#if LOAD_CORE_DUMMY
bool load_core(InstructionRegister<Instruction>& ir1, InstructionRegister<Instruction>& ir2, Instruction mem_load[PR_SIZE],
	Instruction mem_pu[PR_SIZE], PRMan& pu_man, hls::stream<U32>& status, Load2BpChn& bp_chn_in_buf, Load2OutChn& out_chn_in_buf,
	Load2PU pu_in_buf[PU_NUM], Channel& input, ShareMemory& pool, LCUConfig& config, LCUInput& buffer, stream_type<ap_uint<1> > push_data[PU_NUM])
{
   return true;
}
#endif //LOAD_CORE_DUMMY

void copy_cu_data_array_src_cur_size_16ul_s(ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2)
{
	copy_cu_data_array_src<16>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
		cu_width_log2, cu_height_log2, cu_width_log2);
}

void copy_cu_data_array_src_cur_size_32ul_s(ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2)
{
	copy_cu_data_array_src<32>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
		cu_width_log2, cu_height_log2, cu_width_log2);
}

void copy_cu_data_array_src_cur_size_64ul_s(ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2)
{
	copy_cu_data_array_src<64>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
		cu_width_log2, cu_height_log2, cu_width_log2);
}

void copy_cu_data_array_src_cur_size_16ul_l(ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2)
   {
	copy_cu_data_array_src<16>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
		cu_width_log2, cu_height_log2, cu_width_log2);
   }


void copy_cu_data_array_src_cur_size_32ul_l(ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2)
   {
	copy_cu_data_array_src<32>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
		cu_width_log2, cu_height_log2, cu_width_log2);
   }

void copy_cu_data_array_src_cur_size_64ul_l(ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2)
   {
	copy_cu_data_array_src<64>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
		cu_width_log2, cu_height_log2, cu_width_log2);
   }

s64 store_enc_info_best_16_top_array_size_16(
	MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_SIZE<16>*	curr_cu_data_best,   ENC_CU_DATA_ARRAY_SIZE<32>*  prev_cu_data_ptr,
	U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw
	)
{
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];

	//allowed split by normative and non-normative selection
	U3 num_split_to_try = split_struct.num_split[part_num];


	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;


	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	assert(cu_width == 16 && cu_height == 16);


	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1


	store_enc_info_best_16_top(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode,
		prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

void cost_compare_fixed_point_one_time_32_not_use(MD_FW* md_fw_ptr,
    s64* cost_best, s64 cost[1],
    ENC_CU_DATA_ARRAY_SIZE<32> curr_cu_data_ptr[1], ENC_CU_DATA_ARRAY_SIZE<32> curr_cu_data_best_ptr[1],
    U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode[1], U3* best_split_mode,
    U2* best_cons_pred_mode, U2 cons_pred_mode_child[1], U2 tree_status_child[1], U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
    cost_compare_fixed_point_one_time<32>(md_fw_ptr,
        cost_best, cost,
        curr_cu_data_ptr, curr_cu_data_best_ptr,
        cu_width_log2, cu_height_log2, cud, tree_status, split_mode, best_split_mode,
        best_cons_pred_mode, cons_pred_mode_child, tree_status_child, best_child_status, tree_status_temp, split_mode_num);
}

void cost_compare_fixed_point_one_time_64_not_use(MD_FW* md_fw_ptr,
    s64* cost_best, s64 cost[1],
    ENC_CU_DATA_ARRAY_SIZE<64> curr_cu_data_ptr[1], ENC_CU_DATA_ARRAY_SIZE<64> curr_cu_data_best_ptr[1],
    U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode[1], U3* best_split_mode,
    U2* best_cons_pred_mode, U2 cons_pred_mode_child[1], U2 tree_status_child[1], U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
    cost_compare_fixed_point_one_time<64>(md_fw_ptr,
        cost_best, cost,
        curr_cu_data_ptr, curr_cu_data_best_ptr,
        cu_width_log2, cu_height_log2, cud, tree_status, split_mode, best_split_mode,
        best_cons_pred_mode, cons_pred_mode_child, tree_status_child, best_child_status, tree_status_temp, split_mode_num);
}
void cost_compare_fixed_point_one_time_32_0_not_use(MD_FW* md_fw_ptr,
    s64* cost_best, s64 cost[1],
    ENC_CU_DATA_ARRAY_SIZE<32> curr_cu_data_ptr[1], ENC_CU_DATA_ARRAY curr_cu_data_best_ptr[1],
    U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode[1], U3* best_split_mode,
    U2* best_cons_pred_mode, U2 cons_pred_mode_child[1], U2 tree_status_child[1], U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
    cost_compare_fixed_point_one_time<32>(md_fw_ptr,
        cost_best, cost,
        curr_cu_data_ptr, curr_cu_data_best_ptr,
        cu_width_log2, cu_height_log2, cud, tree_status, split_mode, best_split_mode,
        best_cons_pred_mode, cons_pred_mode_child, tree_status_child, best_child_status, tree_status_temp, split_mode_num);
}
void cost_compare_fixed_point_one_time_64_0_not_use(MD_FW* md_fw_ptr,
    s64* cost_best, s64 cost[1],
    ENC_CU_DATA_ARRAY_SIZE<64> curr_cu_data_ptr[1], ENC_CU_DATA_ARRAY curr_cu_data_best_ptr[1],
    U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode[1], U3* best_split_mode,
    U2* best_cons_pred_mode, U2 cons_pred_mode_child[1], U2 tree_status_child[1], U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
    cost_compare_fixed_point_one_time<64>(md_fw_ptr,
        cost_best, cost,
        curr_cu_data_ptr, curr_cu_data_best_ptr,
        cu_width_log2, cu_height_log2, cud, tree_status, split_mode, best_split_mode,
        best_cons_pred_mode, cons_pred_mode_child, tree_status_child, best_child_status, tree_status_temp, split_mode_num);
}
void compare_store_top_not_use(MD_FW* md_fw_ptr,
    s64& cost_best, s64 cost,
    ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best_ptr,
    U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3* best_split_mode,
    U2* best_cons_pred_mode, U2 cons_pred_mode_child, U2 tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
     compare_store_top(md_fw_ptr,
        cost_best, cost,
        curr_cu_data_ptr, NULL, curr_cu_data_best_ptr, NULL, NULL,
        cu_width_log2, cu_height_log2, cud, tree_status, split_mode, best_split_mode,
        best_cons_pred_mode, cons_pred_mode_child, tree_status_child, best_child_status, tree_status_temp, split_mode_num);
}
void get_cu_data_from_curblk_16_16_not_use(ENC_CU_DATA_ARRAY_SIZE<16>* prev_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
{
	prev_cu_data_ptr->get_cu_data_from_curblk(src, x_cur_cu, y_cur_cu, cu_width_log2, cu_height_log2, log2_prev_cuw);
}
void get_cu_data_from_curblk_32_32_not_use(ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
{
	prev_cu_data_ptr->get_cu_data_from_curblk(src, x_cur_cu, y_cur_cu, cu_width_log2, cu_height_log2, log2_prev_cuw);
}
void get_cu_data_from_curblk_64_64_not_use(ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<64>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
{
	prev_cu_data_ptr->get_cu_data_from_curblk(src, x_cur_cu, y_cur_cu, cu_width_log2, cu_height_log2, log2_prev_cuw);
}
void get_cu_data_from_curblk_16_8_not_use(ENC_CU_DATA_ARRAY_SIZE<16>* prev_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<8>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
{
	prev_cu_data_ptr->get_cu_data_from_curblk(src, x_cur_cu, y_cur_cu, cu_width_log2, cu_height_log2, log2_prev_cuw);
}
void get_cu_data_from_curblk_32_16_not_use(ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
{
	prev_cu_data_ptr->get_cu_data_from_curblk(src, x_cur_cu, y_cur_cu, cu_width_log2, cu_height_log2, log2_prev_cuw);
}
void get_cu_data_from_curblk_64_32_not_use(ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* src, int x_cur_cu, int y_cur_cu, int cu_width_log2, int cu_height_log2, int log2_prev_cuw)
{
	prev_cu_data_ptr->get_cu_data_from_curblk(src, x_cur_cu, y_cur_cu, cu_width_log2, cu_height_log2, log2_prev_cuw);
}

s64 store_enc_info_best_for_synth(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
   COM_SPLIT_STRUCT split_struct;
	s64 cost_best = MAX_S64;

	store_enc_info_best(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, best_split_mode, best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);
   
	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

s64 store_enc_info_best_size_64_for_synth(MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<64>* curr_cu_data_best, 
	U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, 
	ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
   COM_SPLIT_STRUCT split_struct;
	s64 cost_best=  MAX_S64;

	store_enc_info_best<64>(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, best_split_mode, best_cons_pred_mode, num_split_to_try,
		prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}


s64 store_enc_info_best_size_32_for_synth(MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_best, 
	U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, 
	ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
   COM_SPLIT_STRUCT split_struct;
	s64 cost_best=  MAX_S64;

	store_enc_info_best<32>(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, best_split_mode, best_cons_pred_mode, num_split_to_try,
		prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

s64 store_enc_info_best_sizex2_32_for_synth(MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_best, 
	U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, 
	ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
   COM_SPLIT_STRUCT split_struct;
	s64 cost_best=  MAX_S64;

	store_enc_info_best<32>(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, best_split_mode, best_cons_pred_mode, num_split_to_try,
		prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

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
	pel mod_info_best_coef_v[4][4])
{
#pragma HLS INLINE off

   s64 dummy = 0;
   // dummy_template.cpp
    // int all_rdo_mode; 
    dummy+=all_rdo_mode;

 // ::RDOQ_ARRAY* rdoq_array; 
    dummy+=rdoq_array[0].rdoq_est_run[0][0];
    dummy+=rdoq_array[0].rdoq_est_run[0][1];
    dummy+=rdoq_array[0].rdoq_est_run[1][0];
    dummy+=rdoq_array[0].rdoq_est_run[1][1];
    dummy+=rdoq_array[0].rdoq_est_run[2][0];
    dummy+=rdoq_array[0].rdoq_est_run[2][1];
    dummy+=rdoq_array[0].rdoq_est_run[3][0];
    dummy+=rdoq_array[0].rdoq_est_run[3][1];
    dummy+=rdoq_array[0].rdoq_est_run[4][0];
    dummy+=rdoq_array[0].rdoq_est_run[4][1];
    dummy+=rdoq_array[0].rdoq_est_run[5][0];
    dummy+=rdoq_array[0].rdoq_est_run[5][1];
    dummy+=rdoq_array[0].rdoq_est_run[6][0];
    dummy+=rdoq_array[0].rdoq_est_run[6][1];
    dummy+=rdoq_array[0].rdoq_est_run[7][0];
    dummy+=rdoq_array[0].rdoq_est_run[7][1];
    dummy+=rdoq_array[0].rdoq_est_run[8][0];
    dummy+=rdoq_array[0].rdoq_est_run[8][1];
    dummy+=rdoq_array[0].rdoq_est_run[9][0];
    dummy+=rdoq_array[0].rdoq_est_run[9][1];
    dummy+=rdoq_array[0].rdoq_est_run[10][0];
    dummy+=rdoq_array[0].rdoq_est_run[10][1];
    dummy+=rdoq_array[0].rdoq_est_run[11][0];
    dummy+=rdoq_array[0].rdoq_est_run[11][1];
    dummy+=rdoq_array[0].rdoq_est_run[12][0];
    dummy+=rdoq_array[0].rdoq_est_run[12][1];
    dummy+=rdoq_array[0].rdoq_est_run[13][0];
    dummy+=rdoq_array[0].rdoq_est_run[13][1];
    dummy+=rdoq_array[0].rdoq_est_run[14][0];
    dummy+=rdoq_array[0].rdoq_est_run[14][1];
    dummy+=rdoq_array[0].rdoq_est_run[15][0];
    dummy+=rdoq_array[0].rdoq_est_run[15][1];
    dummy+=rdoq_array[0].rdoq_est_run[16][0];
    dummy+=rdoq_array[0].rdoq_est_run[16][1];
    dummy+=rdoq_array[0].rdoq_est_run[17][0];
    dummy+=rdoq_array[0].rdoq_est_run[17][1];
    dummy+=rdoq_array[0].rdoq_est_run[18][0];
    dummy+=rdoq_array[0].rdoq_est_run[18][1];
    dummy+=rdoq_array[0].rdoq_est_run[19][0];
    dummy+=rdoq_array[0].rdoq_est_run[19][1];
    dummy+=rdoq_array[0].rdoq_est_run[20][0];
    dummy+=rdoq_array[0].rdoq_est_run[20][1];
    dummy+=rdoq_array[0].rdoq_est_run[21][0];
    dummy+=rdoq_array[0].rdoq_est_run[21][1];
    dummy+=rdoq_array[0].rdoq_est_run[22][0];
    dummy+=rdoq_array[0].rdoq_est_run[22][1];
    dummy+=rdoq_array[0].rdoq_est_run[23][0];
    dummy+=rdoq_array[0].rdoq_est_run[23][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[1][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[1][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[2][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[0][0];
    dummy+=rdoq_array[0].rdoq_est_level[0][1];
    dummy+=rdoq_array[0].rdoq_est_level[1][0];
    dummy+=rdoq_array[0].rdoq_est_level[1][1];
    dummy+=rdoq_array[0].rdoq_est_level[2][0];
    dummy+=rdoq_array[0].rdoq_est_level[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[3][0];
    dummy+=rdoq_array[0].rdoq_est_level[3][1];
    dummy+=rdoq_array[0].rdoq_est_level[4][0];
    dummy+=rdoq_array[0].rdoq_est_level[4][1];
    dummy+=rdoq_array[0].rdoq_est_level[5][0];
    dummy+=rdoq_array[0].rdoq_est_level[5][1];
    dummy+=rdoq_array[0].rdoq_est_level[6][0];
    dummy+=rdoq_array[0].rdoq_est_level[6][1];
    dummy+=rdoq_array[0].rdoq_est_level[7][0];
    dummy+=rdoq_array[0].rdoq_est_level[7][1];
    dummy+=rdoq_array[0].rdoq_est_level[8][0];
    dummy+=rdoq_array[0].rdoq_est_level[8][1];
    dummy+=rdoq_array[0].rdoq_est_level[9][0];
    dummy+=rdoq_array[0].rdoq_est_level[9][1];
    dummy+=rdoq_array[0].rdoq_est_level[10][0];
    dummy+=rdoq_array[0].rdoq_est_level[10][1];
    dummy+=rdoq_array[0].rdoq_est_level[11][0];
    dummy+=rdoq_array[0].rdoq_est_level[11][1];
    dummy+=rdoq_array[0].rdoq_est_level[12][0];
    dummy+=rdoq_array[0].rdoq_est_level[12][1];
    dummy+=rdoq_array[0].rdoq_est_level[13][0];
    dummy+=rdoq_array[0].rdoq_est_level[13][1];
    dummy+=rdoq_array[0].rdoq_est_level[14][0];
    dummy+=rdoq_array[0].rdoq_est_level[14][1];
    dummy+=rdoq_array[0].rdoq_est_level[15][0];
    dummy+=rdoq_array[0].rdoq_est_level[15][1];
    dummy+=rdoq_array[0].rdoq_est_level[16][0];
    dummy+=rdoq_array[0].rdoq_est_level[16][1];
    dummy+=rdoq_array[0].rdoq_est_level[17][0];
    dummy+=rdoq_array[0].rdoq_est_level[17][1];
    dummy+=rdoq_array[0].rdoq_est_level[18][0];
    dummy+=rdoq_array[0].rdoq_est_level[18][1];
    dummy+=rdoq_array[0].rdoq_est_level[19][0];
    dummy+=rdoq_array[0].rdoq_est_level[19][1];
    dummy+=rdoq_array[0].rdoq_est_level[20][0];
    dummy+=rdoq_array[0].rdoq_est_level[20][1];
    dummy+=rdoq_array[0].rdoq_est_level[21][0];
    dummy+=rdoq_array[0].rdoq_est_level[21][1];
    dummy+=rdoq_array[0].rdoq_est_level[22][0];
    dummy+=rdoq_array[0].rdoq_est_level[22][1];
    dummy+=rdoq_array[0].rdoq_est_level[23][0];
    dummy+=rdoq_array[0].rdoq_est_level[23][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][4][1];

 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    dummy+=md_fw_ptr[0] . slice_type;
    // md_fw_ptr[0] . slice_qp is ignored ;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . mipf_enable_flag;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    // md_fw_ptr[0] . refp_ptr[0][0] is ignored ;
    // md_fw_ptr[0] . num_refp[0] is ignored ;
    // md_fw_ptr[0] . cur_ptr is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cr is ignored ;

 // ::U2 tree_status; 
    dummy+=tree_status;

 // ::MD_COM_MODE_BEST* mod_info_best; 
    dummy+=mod_info_best[0] . x_scu;
    dummy+=mod_info_best[0] . y_scu;
    // mod_info_best[0] . cud is ignored ;
    dummy+=mod_info_best[0] . cu_width;
    dummy+=mod_info_best[0] . cu_height;
    dummy+=mod_info_best[0] . cu_width_log2;
    dummy+=mod_info_best[0] . cu_height_log2;
    dummy+=mod_info_best[0] . x_pos;
    dummy+=mod_info_best[0] . y_pos;
    // mod_info_best[0] . scup is ignored ;
    dummy+=mod_info_best[0] . cu_mode;
    // mod_info_best[0] . pb_part is ignored ;
    // mod_info_best[0] . tb_part is ignored ;
    // mod_info_best[0] . rec_y[0] is ignored ;
    /*
    mod_info_best[0].rec_u[0]=dummy;
    mod_info_best[0].rec_u[1]=dummy;
    mod_info_best[0].rec_u[2]=dummy;
    mod_info_best[0].rec_u[3]=dummy;
    mod_info_best[0].rec_v[0]=dummy;
    mod_info_best[0].rec_v[1]=dummy;
    mod_info_best[0].rec_v[2]=dummy;
    mod_info_best[0].rec_v[3]=dummy;
    // mod_info_best[0] . coef_y[0] is ignored ;
    mod_info_best[0].coef_u[0]=dummy;
    mod_info_best[0].coef_u[1]=dummy;
    mod_info_best[0].coef_u[2]=dummy;
    mod_info_best[0].coef_u[3]=dummy;
    mod_info_best[0].coef_v[0]=dummy;
    mod_info_best[0].coef_v[1]=dummy;
    mod_info_best[0].coef_v[2]=dummy;
    mod_info_best[0].coef_v[3]=dummy;
    */
    // mod_info_best[0] . pred_y[0][0] is ignored ;
    // mod_info_best[0] . pred_u[0] is ignored ;
    // mod_info_best[0] . pred_v[0] is ignored ;
    mod_info_best[0].num_nz[0][1]=dummy;
    mod_info_best[0].num_nz[0][2]=dummy;
    dummy+=mod_info_best[0].refi[0];
    dummy+=mod_info_best[0].refi[1];
    // mod_info_best[0] . mvr_idx is ignored ;
    // mod_info_best[0] . umve_flag is ignored ;
    // mod_info_best[0] . umve_idx is ignored ;
    // mod_info_best[0] . skip_idx is ignored ;
    // mod_info_best[0] . mvp_from_hmvp_flag is ignored ;
    // mod_info_best[0] . inter_filter_flag is ignored ;
    // mod_info_best[0] . mvd[0][0] is ignored ;
    dummy+=mod_info_best[0].mv[0][0];
    dummy+=mod_info_best[0].mv[0][1];
    dummy+=mod_info_best[0].mv[1][0];
    dummy+=mod_info_best[0].mv[1][1];
    // mod_info_best[0] . affine_flag is ignored ;
    // mod_info_best[0] . affine_mv[0][0][0] is ignored ;
    // mod_info_best[0] . affine_mvd[0][0][0] is ignored ;
    // mod_info_best[0] . smvd_flag is ignored ;
    // mod_info_best[0] . mpm[0][0] is ignored ;
    dummy+=mod_info_best[0].ipm[0][0];
    mod_info_best[0].ipm[0][1]=dummy;
    // mod_info_best[0] . ipf_flag is ignored ;
    // mod_info_best[0] . chroma_motion is ignored ;
    // mod_info_best[0] . x_luma is ignored ;
    // mod_info_best[0] . y_luma is ignored ;
    // mod_info_best[0] . x_chroma is ignored ;
    // mod_info_best[0] . y_chroma is ignored ;
    // mod_info_best[0] . last_child is ignored ;

 // ::COM_PART_INFO* mod_info_curr_pb_info; 
    // mod_info_curr_pb_info[0] . num_sub_part is ignored ;
    // mod_info_curr_pb_info[0] . sub_x[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_y[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_w[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_h[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_scup[0] is ignored ;

 // ::MD_INPUT* md_input_ptr; 
    // md_input_ptr[0] . pix_x is ignored ;
    // md_input_ptr[0] . pix_y is ignored ;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0].lambda[0];
    dummy+=md_input_ptr[0].lambda[1];
    // md_input_ptr[0] . lambda_y is ignored ;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    // md_input_ptr[0] . qp_y is ignored ;
    dummy+=md_input_ptr[0] . qp_u;
    // md_input_ptr[0] . qp_v is ignored ;
    dummy+=md_input_ptr[0].dist_chroma_weight[0];
    dummy+=md_input_ptr[0].dist_chroma_weight[1];
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0][0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;
    // md_input_ptr[0] . cu_dqp_enable is ignored ;
    // md_input_ptr[0] . cu_qp_group_area_size is ignored ;
    // md_input_ptr[0] . cu_qp_group_pred_qp is ignored ;
    // md_input_ptr[0] . cu_luma_qp[0] is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cr is ignored ;
    // md_input_ptr[0] . md_slice_qp is ignored ;
    // md_input_ptr[0] . CuQPMap[0] is ignored ;

 // ::pel[2][16] orgU; 
    dummy+=orgU[0][0];
    dummy+=orgU[0][1];
    dummy+=orgU[0][2];
    dummy+=orgU[0][3];
    dummy+=orgU[0][4];
    dummy+=orgU[0][5];
    dummy+=orgU[0][6];
    dummy+=orgU[0][7];
    dummy+=orgU[0][8];
    dummy+=orgU[0][9];
    dummy+=orgU[0][10];
    dummy+=orgU[0][11];
    dummy+=orgU[0][12];
    dummy+=orgU[0][13];
    dummy+=orgU[0][14];
    dummy+=orgU[0][15];
    dummy+=orgU[1][0];
    dummy+=orgU[1][1];
    dummy+=orgU[1][2];
    dummy+=orgU[1][3];
    dummy+=orgU[1][4];
    dummy+=orgU[1][5];
    dummy+=orgU[1][6];
    dummy+=orgU[1][7];
    dummy+=orgU[1][8];
    dummy+=orgU[1][9];
    dummy+=orgU[1][10];
    dummy+=orgU[1][11];
    dummy+=orgU[1][12];
    dummy+=orgU[1][13];
    dummy+=orgU[1][14];
    dummy+=orgU[1][15];

 // struct ::IP_ARRAY_SIZE < 4UL , 4UL > {pel up_line[8];pel left_line[8];pel top_left;pel rec_line_hor[8];pel rec_line_ver[8];}* IP_buffer_ptr_U_4x4; 
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[0];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[1];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[2];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[3];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[4];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[5];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[6];
    dummy+=IP_buffer_ptr_U_4x4[0].up_line[7];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[0];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[1];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[2];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[3];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[4];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[5];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[6];
    dummy+=IP_buffer_ptr_U_4x4[0].left_line[7];
    dummy+=IP_buffer_ptr_U_4x4[0] . top_left;
    // IP_buffer_ptr_U_4x4[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_U_4x4[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_ARRAY_SIZE < 4UL , 4UL > {pel up_line[8];pel left_line[8];pel top_left;pel rec_line_hor[8];pel rec_line_ver[8];}* IP_buffer_ptr_V_4x4; 
    dummy+=(IP_buffer_ptr_V_4x4[0] . up_line[0])+(IP_buffer_ptr_V_4x4[0] . up_line[1]/*TODO:*/);
    dummy+=(IP_buffer_ptr_V_4x4[0] . left_line[0])+(IP_buffer_ptr_V_4x4[0] . left_line[1]/*TODO:*/);
    dummy+=IP_buffer_ptr_V_4x4[0] . top_left;
    // IP_buffer_ptr_V_4x4[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_V_4x4[0] . rec_line_ver[0] is ignored ;

 // ::pel[12288] RefWindowLCU_UP; 
    // RefWindowLCU_UP[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP; 
    // RefWindowLCU_VP[0] is ignored ;

 // ::S14[2][2][2] CtrPos; 
    // CtrPos[0][0][0] is ignored ;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[128]/*TODO:*/);
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[128]/*TODO:*/);
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0].lidx[0];
    dummy+=Fetch_Ref_window_ptr[0].lidx[1];
    dummy+=Fetch_Ref_window_ptr[0].cur_refi[0];
    dummy+=Fetch_Ref_window_ptr[0].cur_refi[1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[0][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[0][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[1][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[1][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[2][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[2][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[3][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[3][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[4][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[4][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[5][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[5][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[6][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[6][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[7][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[7][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[8][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[8][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[9][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[9][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[10][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[10][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[11][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[11][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[12][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[12][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[13][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[13][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[14][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[14][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[15][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[15][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[16][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[16][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[0][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[0][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[1][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[1][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[2][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[2][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[3][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[3][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[4][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[4][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[5][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[5][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[6][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[6][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[7][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[7][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[8][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[8][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[9][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[9][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[10][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[10][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[11][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[11][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[12][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[12][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[13][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[13][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[14][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[14][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[15][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[15][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[16][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[16][1];
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[128]/*TODO:*/);
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[128]/*TODO:*/);

 // ::pel[8][8] piRecoY; 
    dummy+=piRecoY[0][0];
    dummy+=piRecoY[0][1];
    dummy+=piRecoY[0][2];
    dummy+=piRecoY[0][3];
    dummy+=piRecoY[0][4];
    dummy+=piRecoY[0][5];
    dummy+=piRecoY[0][6];
    dummy+=piRecoY[0][7];
    dummy+=piRecoY[1][0];
    dummy+=piRecoY[1][1];
    dummy+=piRecoY[1][2];
    dummy+=piRecoY[1][3];
    dummy+=piRecoY[1][4];
    dummy+=piRecoY[1][5];
    dummy+=piRecoY[1][6];
    dummy+=piRecoY[1][7];
    dummy+=piRecoY[2][0];
    dummy+=piRecoY[2][1];
    dummy+=piRecoY[2][2];
    dummy+=piRecoY[2][3];
    dummy+=piRecoY[2][4];
    dummy+=piRecoY[2][5];
    dummy+=piRecoY[2][6];
    dummy+=piRecoY[2][7];
    dummy+=piRecoY[3][0];
    dummy+=piRecoY[3][1];
    dummy+=piRecoY[3][2];
    dummy+=piRecoY[3][3];
    dummy+=piRecoY[3][4];
    dummy+=piRecoY[3][5];
    dummy+=piRecoY[3][6];
    dummy+=piRecoY[3][7];
    dummy+=piRecoY[4][0];
    dummy+=piRecoY[4][1];
    dummy+=piRecoY[4][2];
    dummy+=piRecoY[4][3];
    dummy+=piRecoY[4][4];
    dummy+=piRecoY[4][5];
    dummy+=piRecoY[4][6];
    dummy+=piRecoY[4][7];
    dummy+=piRecoY[5][0];
    dummy+=piRecoY[5][1];
    dummy+=piRecoY[5][2];
    dummy+=piRecoY[5][3];
    dummy+=piRecoY[5][4];
    dummy+=piRecoY[5][5];
    dummy+=piRecoY[5][6];
    dummy+=piRecoY[5][7];
    dummy+=piRecoY[6][0];
    dummy+=piRecoY[6][1];
    dummy+=piRecoY[6][2];
    dummy+=piRecoY[6][3];
    dummy+=piRecoY[6][4];
    dummy+=piRecoY[6][5];
    dummy+=piRecoY[6][6];
    dummy+=piRecoY[6][7];
    dummy+=piRecoY[7][0];
    dummy+=piRecoY[7][1];
    dummy+=piRecoY[7][2];
    dummy+=piRecoY[7][3];
    dummy+=piRecoY[7][4];
    dummy+=piRecoY[7][5];
    dummy+=piRecoY[7][6];
    dummy+=piRecoY[7][7];

 // struct ::IP_ARRAY_SIZE < 8UL , 8UL > {pel up_line[16];pel left_line[16];pel top_left;pel rec_line_hor[16];pel rec_line_ver[16];}* IP_buffer_ptr_8x8; 
    dummy+=IP_buffer_ptr_8x8[0].up_line[0];
    dummy+=IP_buffer_ptr_8x8[0].up_line[1];
    dummy+=IP_buffer_ptr_8x8[0].up_line[2];
    dummy+=IP_buffer_ptr_8x8[0].up_line[3];
    dummy+=IP_buffer_ptr_8x8[0].up_line[4];
    dummy+=IP_buffer_ptr_8x8[0].up_line[5];
    dummy+=IP_buffer_ptr_8x8[0].up_line[6];
    dummy+=IP_buffer_ptr_8x8[0].up_line[7];
    dummy+=IP_buffer_ptr_8x8[0].up_line[8];
    dummy+=IP_buffer_ptr_8x8[0].up_line[9];
    dummy+=IP_buffer_ptr_8x8[0].up_line[10];
    dummy+=IP_buffer_ptr_8x8[0].up_line[11];
    dummy+=IP_buffer_ptr_8x8[0].up_line[12];
    dummy+=IP_buffer_ptr_8x8[0].up_line[13];
    dummy+=IP_buffer_ptr_8x8[0].up_line[14];
    dummy+=IP_buffer_ptr_8x8[0].up_line[15];
    dummy+=IP_buffer_ptr_8x8[0].left_line[0];
    dummy+=IP_buffer_ptr_8x8[0].left_line[1];
    dummy+=IP_buffer_ptr_8x8[0].left_line[2];
    dummy+=IP_buffer_ptr_8x8[0].left_line[3];
    dummy+=IP_buffer_ptr_8x8[0].left_line[4];
    dummy+=IP_buffer_ptr_8x8[0].left_line[5];
    dummy+=IP_buffer_ptr_8x8[0].left_line[6];
    dummy+=IP_buffer_ptr_8x8[0].left_line[7];
    dummy+=IP_buffer_ptr_8x8[0].left_line[8];
    dummy+=IP_buffer_ptr_8x8[0].left_line[9];
    dummy+=IP_buffer_ptr_8x8[0].left_line[10];
    dummy+=IP_buffer_ptr_8x8[0].left_line[11];
    dummy+=IP_buffer_ptr_8x8[0].left_line[12];
    dummy+=IP_buffer_ptr_8x8[0].left_line[13];
    dummy+=IP_buffer_ptr_8x8[0].left_line[14];
    dummy+=IP_buffer_ptr_8x8[0].left_line[15];
    dummy+=IP_buffer_ptr_8x8[0] . top_left;
    // IP_buffer_ptr_8x8[0] . rec_line_hor[0] is ignored ;
    // IP_buffer_ptr_8x8[0] . rec_line_ver[0] is ignored ;

 // struct ::IP_MAP_SCU_SIZE < 8UL , 8UL > {U32 map_scu_above_line[4];U32 map_scu_top_left;U32 map_scu_up_line[4];U32 map_scu_left_line_new[4];U32 map_scu_top_left_new;U32 cur_map_scu[2][2];}* IP_map_scu_ptr_8x8; 
    // IP_map_scu_ptr_8x8[0] . map_scu_above_line[0] is ignored ;
    // IP_map_scu_ptr_8x8[0] . map_scu_top_left is ignored ;
    dummy+=(IP_map_scu_ptr_8x8[0] . map_scu_up_line[0])+(IP_map_scu_ptr_8x8[0] . map_scu_up_line[1]/*TODO:*/);
    dummy+=(IP_map_scu_ptr_8x8[0] . map_scu_left_line_new[0])+(IP_map_scu_ptr_8x8[0] . map_scu_left_line_new[1]/*TODO:*/);
    dummy+=IP_map_scu_ptr_8x8[0] . map_scu_top_left_new;
    // IP_map_scu_ptr_8x8[0] . cur_map_scu[0][0] is ignored ;

 // ::pel[4][4] mod_info_best_rec_u; 
    mod_info_best_rec_u[0][0]=dummy;
    mod_info_best_rec_u[0][1]=dummy;
    mod_info_best_rec_u[0][2]=dummy;
    mod_info_best_rec_u[0][3]=dummy;
    mod_info_best_rec_u[1][0]=dummy;
    mod_info_best_rec_u[1][1]=dummy;
    mod_info_best_rec_u[1][2]=dummy;
    mod_info_best_rec_u[1][3]=dummy;
    mod_info_best_rec_u[2][0]=dummy;
    mod_info_best_rec_u[2][1]=dummy;
    mod_info_best_rec_u[2][2]=dummy;
    mod_info_best_rec_u[2][3]=dummy;
    mod_info_best_rec_u[3][0]=dummy;
    mod_info_best_rec_u[3][1]=dummy;
    mod_info_best_rec_u[3][2]=dummy;
    mod_info_best_rec_u[3][3]=dummy;

 // ::pel[4][4] mod_info_best_rec_v; 
    mod_info_best_rec_v[0][0]=dummy;
    mod_info_best_rec_v[0][1]=dummy;
    mod_info_best_rec_v[0][2]=dummy;
    mod_info_best_rec_v[0][3]=dummy;
    mod_info_best_rec_v[1][0]=dummy;
    mod_info_best_rec_v[1][1]=dummy;
    mod_info_best_rec_v[1][2]=dummy;
    mod_info_best_rec_v[1][3]=dummy;
    mod_info_best_rec_v[2][0]=dummy;
    mod_info_best_rec_v[2][1]=dummy;
    mod_info_best_rec_v[2][2]=dummy;
    mod_info_best_rec_v[2][3]=dummy;
    mod_info_best_rec_v[3][0]=dummy;
    mod_info_best_rec_v[3][1]=dummy;
    mod_info_best_rec_v[3][2]=dummy;
    mod_info_best_rec_v[3][3]=dummy;

 // ::pel[4][4] mod_info_best_coef_u; 
    mod_info_best_coef_u[0][0]=dummy;
    mod_info_best_coef_u[0][1]=dummy;
    mod_info_best_coef_u[0][2]=dummy;
    mod_info_best_coef_u[0][3]=dummy;
    mod_info_best_coef_u[1][0]=dummy;
    mod_info_best_coef_u[1][1]=dummy;
    mod_info_best_coef_u[1][2]=dummy;
    mod_info_best_coef_u[1][3]=dummy;
    mod_info_best_coef_u[2][0]=dummy;
    mod_info_best_coef_u[2][1]=dummy;
    mod_info_best_coef_u[2][2]=dummy;
    mod_info_best_coef_u[2][3]=dummy;
    mod_info_best_coef_u[3][0]=dummy;
    mod_info_best_coef_u[3][1]=dummy;
    mod_info_best_coef_u[3][2]=dummy;
    mod_info_best_coef_u[3][3]=dummy;

 // ::pel[4][4] mod_info_best_coef_v; 
    mod_info_best_coef_v[0][0]=dummy;
    mod_info_best_coef_v[0][1]=dummy;
    mod_info_best_coef_v[0][2]=dummy;
    mod_info_best_coef_v[0][3]=dummy;
    mod_info_best_coef_v[1][0]=dummy;
    mod_info_best_coef_v[1][1]=dummy;
    mod_info_best_coef_v[1][2]=dummy;
    mod_info_best_coef_v[1][3]=dummy;
    mod_info_best_coef_v[2][0]=dummy;
    mod_info_best_coef_v[2][1]=dummy;
    mod_info_best_coef_v[2][2]=dummy;
    mod_info_best_coef_v[2][3]=dummy;
    mod_info_best_coef_v[3][0]=dummy;
    mod_info_best_coef_v[3][1]=dummy;
    mod_info_best_coef_v[3][2]=dummy;
    mod_info_best_coef_v[3][3]=dummy;

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_4x4[0].left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_4x4[0].up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x8[0].map_scu_left_line_new core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x8[0].map_scu_up_line core=RAM_2P_BRAM 

#pragma HLS resource variable=mod_info_best_coef_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_v core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_v core=RAM_1P_BRAM 

#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].cur_refi complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].lidx complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=2
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=2
#pragma HLS array_partition variable=IP_buffer_ptr_8x8[0].left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_8x8[0].up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_4x4[0].left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_4x4[0].up_line complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].dist_chroma_weight complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].lambda complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].num_nz complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].num_nz complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].refi complete dim=1
#pragma HLS array_partition variable=mod_info_best_coef_u cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_best_coef_v cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_u cyclic factor=4 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_v cyclic factor=4 dim=1

#pragma HLS array_partition variable=orgU complete dim=1
#pragma HLS array_partition variable=orgU complete dim=2
#pragma HLS array_partition variable=piRecoY complete dim=1
#pragma HLS array_partition variable=piRecoY complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=3
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=4
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=2

#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_UP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_VP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1 cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1 cyclic factor=128 dim=1

}
#endif

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
	pel IP_buffer_ptr_U_8x8_up_line[2 * 8],  pel IP_buffer_ptr_U_8x8_left_line[2 * 8],  pel IP_buffer_ptr_U_8x8_top_left,
	pel IP_buffer_ptr_V_8x8_up_line[2 * 8],  pel IP_buffer_ptr_V_8x8_left_line[2 * 8],  pel IP_buffer_ptr_V_8x8_top_left,
	pel IP_buffer_ptr_U_4x8_up_line[2 * 4],  pel IP_buffer_ptr_U_4x8_left_line[2 * 8],  pel IP_buffer_ptr_U_4x8_top_left,
	pel IP_buffer_ptr_V_4x8_up_line[2 * 4],  pel IP_buffer_ptr_V_4x8_left_line[2 * 8],  pel IP_buffer_ptr_V_4x8_top_left,
	pel IP_buffer_ptr_U_8x4_up_line[2 * 8],  pel IP_buffer_ptr_U_8x4_left_line[2 * 4],  pel IP_buffer_ptr_U_8x4_top_left,
	pel IP_buffer_ptr_V_8x4_up_line[2 * 8],  pel IP_buffer_ptr_V_8x4_left_line[2 * 4],  pel IP_buffer_ptr_V_8x4_top_left,
	pel IP_buffer_ptr_8x16_up_line [2 * 8],  pel IP_buffer_ptr_8x16_left_line [2 * 16], pel IP_buffer_ptr_8x16_top_left,
	pel IP_buffer_ptr_16x8_up_line [2 * 16], pel IP_buffer_ptr_16x8_left_line [2 * 8],  pel IP_buffer_ptr_16x8_top_left,
	pel IP_buffer_ptr_16x16_up_line[2 * 16], pel IP_buffer_ptr_16x16_left_line[2 * 16], pel IP_buffer_ptr_16x16_top_left,
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new)
{
#pragma HLS INLINE off

   s64 dummy = 0;
   int i = 0;
    int j = 0;
    int k = 0;
   // dummy_template.cpp
 // int all_rdo_mode; 
    dummy+=all_rdo_mode;

 // ::RDOQ_ARRAY* rdoq_array; 
    dummy+=rdoq_array[0].rdoq_est_run[0][0];
    dummy+=rdoq_array[0].rdoq_est_run[0][1];
    dummy+=rdoq_array[0].rdoq_est_run[1][0];
    dummy+=rdoq_array[0].rdoq_est_run[1][1];
    dummy+=rdoq_array[0].rdoq_est_run[2][0];
    dummy+=rdoq_array[0].rdoq_est_run[2][1];
    dummy+=rdoq_array[0].rdoq_est_run[3][0];
    dummy+=rdoq_array[0].rdoq_est_run[3][1];
    dummy+=rdoq_array[0].rdoq_est_run[4][0];
    dummy+=rdoq_array[0].rdoq_est_run[4][1];
    dummy+=rdoq_array[0].rdoq_est_run[5][0];
    dummy+=rdoq_array[0].rdoq_est_run[5][1];
    dummy+=rdoq_array[0].rdoq_est_run[6][0];
    dummy+=rdoq_array[0].rdoq_est_run[6][1];
    dummy+=rdoq_array[0].rdoq_est_run[7][0];
    dummy+=rdoq_array[0].rdoq_est_run[7][1];
    dummy+=rdoq_array[0].rdoq_est_run[8][0];
    dummy+=rdoq_array[0].rdoq_est_run[8][1];
    dummy+=rdoq_array[0].rdoq_est_run[9][0];
    dummy+=rdoq_array[0].rdoq_est_run[9][1];
    dummy+=rdoq_array[0].rdoq_est_run[10][0];
    dummy+=rdoq_array[0].rdoq_est_run[10][1];
    dummy+=rdoq_array[0].rdoq_est_run[11][0];
    dummy+=rdoq_array[0].rdoq_est_run[11][1];
    dummy+=rdoq_array[0].rdoq_est_run[12][0];
    dummy+=rdoq_array[0].rdoq_est_run[12][1];
    dummy+=rdoq_array[0].rdoq_est_run[13][0];
    dummy+=rdoq_array[0].rdoq_est_run[13][1];
    dummy+=rdoq_array[0].rdoq_est_run[14][0];
    dummy+=rdoq_array[0].rdoq_est_run[14][1];
    dummy+=rdoq_array[0].rdoq_est_run[15][0];
    dummy+=rdoq_array[0].rdoq_est_run[15][1];
    dummy+=rdoq_array[0].rdoq_est_run[16][0];
    dummy+=rdoq_array[0].rdoq_est_run[16][1];
    dummy+=rdoq_array[0].rdoq_est_run[17][0];
    dummy+=rdoq_array[0].rdoq_est_run[17][1];
    dummy+=rdoq_array[0].rdoq_est_run[18][0];
    dummy+=rdoq_array[0].rdoq_est_run[18][1];
    dummy+=rdoq_array[0].rdoq_est_run[19][0];
    dummy+=rdoq_array[0].rdoq_est_run[19][1];
    dummy+=rdoq_array[0].rdoq_est_run[20][0];
    dummy+=rdoq_array[0].rdoq_est_run[20][1];
    dummy+=rdoq_array[0].rdoq_est_run[21][0];
    dummy+=rdoq_array[0].rdoq_est_run[21][1];
    dummy+=rdoq_array[0].rdoq_est_run[22][0];
    dummy+=rdoq_array[0].rdoq_est_run[22][1];
    dummy+=rdoq_array[0].rdoq_est_run[23][0];
    dummy+=rdoq_array[0].rdoq_est_run[23][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[1][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[1][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[2][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[0][0];
    dummy+=rdoq_array[0].rdoq_est_level[0][1];
    dummy+=rdoq_array[0].rdoq_est_level[1][0];
    dummy+=rdoq_array[0].rdoq_est_level[1][1];
    dummy+=rdoq_array[0].rdoq_est_level[2][0];
    dummy+=rdoq_array[0].rdoq_est_level[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[3][0];
    dummy+=rdoq_array[0].rdoq_est_level[3][1];
    dummy+=rdoq_array[0].rdoq_est_level[4][0];
    dummy+=rdoq_array[0].rdoq_est_level[4][1];
    dummy+=rdoq_array[0].rdoq_est_level[5][0];
    dummy+=rdoq_array[0].rdoq_est_level[5][1];
    dummy+=rdoq_array[0].rdoq_est_level[6][0];
    dummy+=rdoq_array[0].rdoq_est_level[6][1];
    dummy+=rdoq_array[0].rdoq_est_level[7][0];
    dummy+=rdoq_array[0].rdoq_est_level[7][1];
    dummy+=rdoq_array[0].rdoq_est_level[8][0];
    dummy+=rdoq_array[0].rdoq_est_level[8][1];
    dummy+=rdoq_array[0].rdoq_est_level[9][0];
    dummy+=rdoq_array[0].rdoq_est_level[9][1];
    dummy+=rdoq_array[0].rdoq_est_level[10][0];
    dummy+=rdoq_array[0].rdoq_est_level[10][1];
    dummy+=rdoq_array[0].rdoq_est_level[11][0];
    dummy+=rdoq_array[0].rdoq_est_level[11][1];
    dummy+=rdoq_array[0].rdoq_est_level[12][0];
    dummy+=rdoq_array[0].rdoq_est_level[12][1];
    dummy+=rdoq_array[0].rdoq_est_level[13][0];
    dummy+=rdoq_array[0].rdoq_est_level[13][1];
    dummy+=rdoq_array[0].rdoq_est_level[14][0];
    dummy+=rdoq_array[0].rdoq_est_level[14][1];
    dummy+=rdoq_array[0].rdoq_est_level[15][0];
    dummy+=rdoq_array[0].rdoq_est_level[15][1];
    dummy+=rdoq_array[0].rdoq_est_level[16][0];
    dummy+=rdoq_array[0].rdoq_est_level[16][1];
    dummy+=rdoq_array[0].rdoq_est_level[17][0];
    dummy+=rdoq_array[0].rdoq_est_level[17][1];
    dummy+=rdoq_array[0].rdoq_est_level[18][0];
    dummy+=rdoq_array[0].rdoq_est_level[18][1];
    dummy+=rdoq_array[0].rdoq_est_level[19][0];
    dummy+=rdoq_array[0].rdoq_est_level[19][1];
    dummy+=rdoq_array[0].rdoq_est_level[20][0];
    dummy+=rdoq_array[0].rdoq_est_level[20][1];
    dummy+=rdoq_array[0].rdoq_est_level[21][0];
    dummy+=rdoq_array[0].rdoq_est_level[21][1];
    dummy+=rdoq_array[0].rdoq_est_level[22][0];
    dummy+=rdoq_array[0].rdoq_est_level[22][1];
    dummy+=rdoq_array[0].rdoq_est_level[23][0];
    dummy+=rdoq_array[0].rdoq_est_level[23][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][9][1];

 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    // md_fw_ptr[0] . slice_type is ignored ;
    // md_fw_ptr[0] . slice_qp is ignored ;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . mipf_enable_flag;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    // md_fw_ptr[0] . refp_ptr[0][0] is ignored ;
    // md_fw_ptr[0] . num_refp[0] is ignored ;
    // md_fw_ptr[0] . cur_ptr is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cr is ignored ;

 // ::U2 tree_status; 
    // tree_status is ignored ;

 // ::MD_COM_MODE_BEST* mod_info_best; 
    dummy+=mod_info_best[0] . x_scu;
    dummy+=mod_info_best[0] . y_scu;
    // mod_info_best[0] . cud is ignored ;
    dummy+=mod_info_best[0] . cu_width;
    dummy+=mod_info_best[0] . cu_height;
    // mod_info_best[0] . cu_width_log2 is ignored ;
    // mod_info_best[0] . cu_height_log2 is ignored ;
    dummy+=mod_info_best[0] . x_pos;
    dummy+=mod_info_best[0] . y_pos;
    // mod_info_best[0] . scup is ignored ;
    dummy+=mod_info_best[0] . cu_mode;
    // mod_info_best[0] . pb_part is ignored ;
    // mod_info_best[0] . tb_part is ignored ;
    // mod_info_best[0] . rec_y[0] is ignored ;
    /*mod_info_best[0].rec_u[0]=dummy;
    mod_info_best[0].rec_u[1]=dummy;
    mod_info_best[0].rec_u[2]=dummy;
    mod_info_best[0].rec_u[3]=dummy;
    mod_info_best[0].rec_u[4]=dummy;
    mod_info_best[0].rec_u[5]=dummy;
    mod_info_best[0].rec_u[6]=dummy;
    mod_info_best[0].rec_u[7]=dummy;
    mod_info_best[0].rec_v[0]=dummy;
    mod_info_best[0].rec_v[1]=dummy;
    mod_info_best[0].rec_v[2]=dummy;
    mod_info_best[0].rec_v[3]=dummy;
    mod_info_best[0].rec_v[4]=dummy;
    mod_info_best[0].rec_v[5]=dummy;
    mod_info_best[0].rec_v[6]=dummy;
    mod_info_best[0].rec_v[7]=dummy;
    // mod_info_best[0] . coef_y[0] is ignored ;
    mod_info_best[0].coef_u[0]=dummy;
    mod_info_best[0].coef_u[1]=dummy;
    mod_info_best[0].coef_u[2]=dummy;
    mod_info_best[0].coef_u[3]=dummy;
    mod_info_best[0].coef_u[4]=dummy;
    mod_info_best[0].coef_u[5]=dummy;
    mod_info_best[0].coef_u[6]=dummy;
    mod_info_best[0].coef_u[7]=dummy;
    mod_info_best[0].coef_v[0]=dummy;
    mod_info_best[0].coef_v[1]=dummy;
    mod_info_best[0].coef_v[2]=dummy;
    mod_info_best[0].coef_v[3]=dummy;
    mod_info_best[0].coef_v[4]=dummy;
    mod_info_best[0].coef_v[5]=dummy;
    mod_info_best[0].coef_v[6]=dummy;
    mod_info_best[0].coef_v[7]=dummy;*/
    // mod_info_best[0] . pred_y[0][0] is ignored ;
    // mod_info_best[0] . pred_u[0] is ignored ;
    // mod_info_best[0] . pred_v[0] is ignored ;
    // mod_info_best[0] . num_nz[0][0] is ignored ;
    dummy+=mod_info_best[0].num_nz[0][0];
    mod_info_best[0].num_nz[0][1] = dummy;        // liaozhx
    mod_info_best[0].num_nz[0][2] = dummy;        // liaozhx
    dummy+=mod_info_best[0].refi[0];
    dummy+=mod_info_best[0].refi[1];
    // mod_info_best[0] . mvr_idx is ignored ;
    // mod_info_best[0] . umve_flag is ignored ;
    // mod_info_best[0] . umve_idx is ignored ;
    dummy+=mod_info_best[0] . skip_idx;
    // mod_info_best[0] . mvp_from_hmvp_flag is ignored ;
    // mod_info_best[0] . inter_filter_flag is ignored ;
    // mod_info_best[0] . mvd[0][0] is ignored ;
    dummy+=mod_info_best[0].mv[0][0];
    dummy+=mod_info_best[0].mv[0][1];
    dummy+=mod_info_best[0].mv[1][0];
    dummy+=mod_info_best[0].mv[1][1];
    // mod_info_best[0] . affine_flag is ignored ;
    // mod_info_best[0] . affine_mv[0][0][0] is ignored ;
    // mod_info_best[0] . affine_mvd[0][0][0] is ignored ;
    // mod_info_best[0] . smvd_flag is ignored ;
    // mod_info_best[0] . mpm[0][0] is ignored ;
    dummy+=mod_info_best[0].ipm[0][0];
    mod_info_best[0].ipm[0][1] = dummy;          // liaozhx
    // mod_info_best[0] . ipf_flag is ignored ;
    // mod_info_best[0] . chroma_motion is ignored ;
    // mod_info_best[0] . x_luma is ignored ;
    // mod_info_best[0] . y_luma is ignored ;
    // mod_info_best[0] . x_chroma is ignored ;
    // mod_info_best[0] . y_chroma is ignored ;
    // mod_info_best[0] . last_child is ignored ;

 // ::COM_PART_INFO* mod_info_curr_pb_info; 
    // mod_info_curr_pb_info[0] . num_sub_part is ignored ;
    // mod_info_curr_pb_info[0] . sub_x[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_y[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_w[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_h[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_scup[0] is ignored ;

 // ::MD_INPUT* md_input_ptr; 
    // md_input_ptr[0] . pix_x is ignored ;
    // md_input_ptr[0] . pix_y is ignored ;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0].lambda[0];
    dummy+=md_input_ptr[0].lambda[1];
    // md_input_ptr[0] . lambda_y is ignored ;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    // md_input_ptr[0] . qp_y is ignored ;
    dummy+=md_input_ptr[0] . qp_u;
    // md_input_ptr[0] . qp_v is ignored ;
    dummy+=md_input_ptr[0].dist_chroma_weight[0];
    dummy+=md_input_ptr[0].dist_chroma_weight[1];
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0][0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;
    // md_input_ptr[0] . cu_dqp_enable is ignored ;
    // md_input_ptr[0] . cu_qp_group_area_size is ignored ;
    // md_input_ptr[0] . cu_qp_group_pred_qp is ignored ;
    // md_input_ptr[0] . cu_luma_qp[0] is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cr is ignored ;
    // md_input_ptr[0] . md_slice_qp is ignored ;
    // md_input_ptr[0] . CuQPMap[0] is ignored ;

 // ::pel[2][8][8] orgU; 
/* dummy += orgU[0][0];
    dummy += orgU[0][1];
    dummy += orgU[0][2];
    dummy += orgU[0][3];
    dummy += orgU[0][4];
    dummy += orgU[0][5];
    dummy += orgU[0][6];
    dummy += orgU[0][7];
    dummy += orgU[1][0];
    dummy += orgU[1][1];
    dummy += orgU[1][2];
    dummy += orgU[1][3];
    dummy += orgU[1][4];
    dummy += orgU[1][5];
    dummy += orgU[1][6];
    dummy += orgU[1][7];*/
    for (i = 0; i < 2;i = i + 1) {
        for (j = 0; j < 8; j = j + 1) {
            for (k = 0; k < 8; k = k + 1) {
                dummy += orgU[i][j][k];
            }
        }

    }
 // ::pel[12288] RefWindowLCU_UP; 
    // RefWindowLCU_UP[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP; 
    // RefWindowLCU_VP[0] is ignored ;

 // ::S14[2][2][2] CtrPos; 
    // CtrPos[0][0][0] is ignored ;

 // ::pel[12288] RefWindowLCU_UP_ref1; 
    // RefWindowLCU_UP_ref1[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP_ref1; 
    // RefWindowLCU_VP_ref1[0] is ignored ;

 // ::S14[2][2][2] CtrPos_ref1; 
    // CtrPos_ref1[0][0][0] is ignored ;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U7 cu_width; 
    dummy+=cu_width;

 // ::U7 cu_height; 
    dummy+=cu_height;

 // ::U13 pb_x; 
    dummy+=pb_x;

 // ::U13 pb_y; 
    dummy+=pb_y;

 // ::S12 size_tmp; 
    // size_tmp is ignored ;

 // ::s64 lambda_y; 
    // lambda_y is ignored ;

 // ::U4 bit_depth; 
    dummy+=bit_depth;

 // ::s16[2][8][8] coef_uv_pb_part0; 
    // coef_uv_pb_part0[0][0][0] is ignored ;

 // ::pel[2][8][8] rec_tmp0; 
    // rec_tmp0[0][0][0] is ignored ;

 // ::pel[2][8][8] pred; 
    // pred[0][0][0] is ignored ;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0];
    dummy+=Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0];
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0].lidx[0];
    dummy+=Fetch_Ref_window_ptr[0].lidx[1];
    dummy+=Fetch_Ref_window_ptr[0].cur_refi[0];
    dummy+=Fetch_Ref_window_ptr[0].cur_refi[1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[0][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[0][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[1][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[1][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[2][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[2][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[3][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[3][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[4][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[4][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[5][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[5][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[6][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[6][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[7][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[7][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[8][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[8][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[9][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[9][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[10][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[10][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[11][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[11][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[12][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[12][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[13][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[13][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[14][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[14][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[15][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[15][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[16][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[16][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[0][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[0][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[1][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[1][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[2][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[2][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[3][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[3][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[4][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[4][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[5][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[5][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[6][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[6][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[7][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[7][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[8][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[8][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[9][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[9][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[10][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[10][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[11][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[11][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[12][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[12][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[13][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[13][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[14][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[14][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[15][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[15][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[16][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[16][1];
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    dummy+=Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0];
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[128]/*TODO:*/);

 // ::pel[16][16] piRecoY; 
    dummy+=piRecoY[0][0];
    dummy+=piRecoY[0][1];
    dummy+=piRecoY[0][2];
    dummy+=(piRecoY[0][3])+(piRecoY[2][3]);  // liaozhx
    dummy+=piRecoY[0][4];
    dummy+=(piRecoY[0][5])+(piRecoY[2][5]);
    dummy+=piRecoY[0][6];
    dummy+=(piRecoY[0][7])+(piRecoY[2][7]);
    dummy+=piRecoY[0][8];
    dummy+=(piRecoY[0][9])+(piRecoY[2][9]);
    dummy+=piRecoY[0][10];
    dummy+=(piRecoY[0][11])+(piRecoY[2][11]);
    dummy+=piRecoY[0][12];
    dummy+=(piRecoY[0][13])+(piRecoY[2][13]);
    dummy+=piRecoY[0][14];
    dummy+=piRecoY[0][15];
    dummy+=piRecoY[1][0];
    dummy+=piRecoY[1][1];
    dummy+=piRecoY[1][2];
    dummy+=(piRecoY[1][3])+(piRecoY[3][3]);
    dummy+=piRecoY[1][4];
    dummy+=(piRecoY[1][5])+(piRecoY[3][5]);
    dummy+=piRecoY[1][6];
    dummy+=(piRecoY[1][7])+(piRecoY[3][7]);
    dummy+=piRecoY[1][8];
    dummy+=(piRecoY[1][9])+(piRecoY[3][9]);
    dummy+=piRecoY[1][10];
    dummy+=(piRecoY[1][11])+(piRecoY[3][11]);
    dummy+=piRecoY[1][12];
    dummy+=(piRecoY[1][13])+(piRecoY[3][13]);
    dummy+=piRecoY[1][14];
    dummy+=piRecoY[1][15];

 // ::CORE_SBTMVP* core_sbtmvp; 
    dummy+=core_sbtmvp[0] . sbTmvp_flag;
    // core_sbtmvp[0] . best_sbTmvp_flag is ignored ;
    dummy+=core_sbtmvp[0].sbTmvp[0].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[0].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[1].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[1].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[2].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[2].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[3].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[3].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[0].ref_idx[0];
    dummy+=core_sbtmvp[0].sbTmvp[1].ref_idx[0];
    dummy+=core_sbtmvp[0].sbTmvp[2].ref_idx[0];
    dummy+=core_sbtmvp[0].sbTmvp[3].ref_idx[0];
    // core_sbtmvp[0] . best_sbTmvp[0] . mv[0][0] is ignored ;
    // core_sbtmvp[0] . best_sbTmvp[0] . ref_idx[0] is ignored ;
    // core_sbtmvp[0] . isValid is ignored ;

 // ::pel[16] IP_buffer_ptr_U_8x8_up_line; 
    dummy+=IP_buffer_ptr_U_8x8_up_line[0];
    dummy+=IP_buffer_ptr_U_8x8_up_line[1];
    dummy+=IP_buffer_ptr_U_8x8_up_line[2];
    dummy+=IP_buffer_ptr_U_8x8_up_line[3];
    dummy+=IP_buffer_ptr_U_8x8_up_line[4];
    dummy+=IP_buffer_ptr_U_8x8_up_line[5];
    dummy+=IP_buffer_ptr_U_8x8_up_line[6];
    dummy+=IP_buffer_ptr_U_8x8_up_line[7];
    dummy+=IP_buffer_ptr_U_8x8_up_line[8];
    dummy+=IP_buffer_ptr_U_8x8_up_line[9];
    dummy+=IP_buffer_ptr_U_8x8_up_line[10];
    dummy+=IP_buffer_ptr_U_8x8_up_line[11];
    dummy+=IP_buffer_ptr_U_8x8_up_line[12];
    dummy+=IP_buffer_ptr_U_8x8_up_line[13];
    dummy+=IP_buffer_ptr_U_8x8_up_line[14];
    dummy+=IP_buffer_ptr_U_8x8_up_line[15];

 // ::pel[16] IP_buffer_ptr_U_8x8_left_line; 
    dummy+=IP_buffer_ptr_U_8x8_left_line[0];
    dummy+=IP_buffer_ptr_U_8x8_left_line[1];
    dummy+=IP_buffer_ptr_U_8x8_left_line[2];
    dummy+=IP_buffer_ptr_U_8x8_left_line[3];
    dummy+=IP_buffer_ptr_U_8x8_left_line[4];
    dummy+=IP_buffer_ptr_U_8x8_left_line[5];
    dummy+=IP_buffer_ptr_U_8x8_left_line[6];
    dummy+=IP_buffer_ptr_U_8x8_left_line[7];
    dummy+=IP_buffer_ptr_U_8x8_left_line[8];
    dummy+=IP_buffer_ptr_U_8x8_left_line[9];
    dummy+=IP_buffer_ptr_U_8x8_left_line[10];
    dummy+=IP_buffer_ptr_U_8x8_left_line[11];
    dummy+=IP_buffer_ptr_U_8x8_left_line[12];
    dummy+=IP_buffer_ptr_U_8x8_left_line[13];
    dummy+=IP_buffer_ptr_U_8x8_left_line[14];
    dummy+=IP_buffer_ptr_U_8x8_left_line[15];

 // ::pel IP_buffer_ptr_U_8x8_top_left; 
    dummy+=IP_buffer_ptr_U_8x8_top_left;

 // ::pel[16] IP_buffer_ptr_V_8x8_up_line; 
    dummy+=IP_buffer_ptr_V_8x8_up_line[0];
    dummy+=IP_buffer_ptr_V_8x8_up_line[1];
    dummy+=IP_buffer_ptr_V_8x8_up_line[2];
    dummy+=IP_buffer_ptr_V_8x8_up_line[3];
    dummy+=IP_buffer_ptr_V_8x8_up_line[4];
    dummy+=IP_buffer_ptr_V_8x8_up_line[5];
    dummy+=IP_buffer_ptr_V_8x8_up_line[6];
    dummy+=IP_buffer_ptr_V_8x8_up_line[7];
    dummy+=IP_buffer_ptr_V_8x8_up_line[8];
    dummy+=IP_buffer_ptr_V_8x8_up_line[9];
    dummy+=IP_buffer_ptr_V_8x8_up_line[10];
    dummy+=IP_buffer_ptr_V_8x8_up_line[11];
    dummy+=IP_buffer_ptr_V_8x8_up_line[12];
    dummy+=IP_buffer_ptr_V_8x8_up_line[13];
    dummy+=IP_buffer_ptr_V_8x8_up_line[14];
    dummy+=IP_buffer_ptr_V_8x8_up_line[15];

 // ::pel[16] IP_buffer_ptr_V_8x8_left_line; 
    dummy+=IP_buffer_ptr_V_8x8_left_line[0];
    dummy+=IP_buffer_ptr_V_8x8_left_line[1];
    dummy+=IP_buffer_ptr_V_8x8_left_line[2];
    dummy+=IP_buffer_ptr_V_8x8_left_line[3];
    dummy+=IP_buffer_ptr_V_8x8_left_line[4];
    dummy+=IP_buffer_ptr_V_8x8_left_line[5];
    dummy+=IP_buffer_ptr_V_8x8_left_line[6];
    dummy+=IP_buffer_ptr_V_8x8_left_line[7];
    dummy+=IP_buffer_ptr_V_8x8_left_line[8];
    dummy+=IP_buffer_ptr_V_8x8_left_line[9];
    dummy+=IP_buffer_ptr_V_8x8_left_line[10];
    dummy+=IP_buffer_ptr_V_8x8_left_line[11];
    dummy+=IP_buffer_ptr_V_8x8_left_line[12];
    dummy+=IP_buffer_ptr_V_8x8_left_line[13];
    dummy+=IP_buffer_ptr_V_8x8_left_line[14];
    dummy+=IP_buffer_ptr_V_8x8_left_line[15];

 // ::pel IP_buffer_ptr_V_8x8_top_left; 
    dummy+=IP_buffer_ptr_V_8x8_top_left;

 // ::pel[8] IP_buffer_ptr_U_4x8_up_line; 
    dummy+=IP_buffer_ptr_U_4x8_up_line[0];
    dummy+=IP_buffer_ptr_U_4x8_up_line[1];
    dummy+=IP_buffer_ptr_U_4x8_up_line[2];
    dummy+=IP_buffer_ptr_U_4x8_up_line[3];
    dummy+=IP_buffer_ptr_U_4x8_up_line[4];
    dummy+=IP_buffer_ptr_U_4x8_up_line[5];
    dummy+=IP_buffer_ptr_U_4x8_up_line[6];
    dummy+=IP_buffer_ptr_U_4x8_up_line[7];

 // ::pel[16] IP_buffer_ptr_U_4x8_left_line; 
    dummy+=IP_buffer_ptr_U_4x8_left_line[0];
    dummy+=IP_buffer_ptr_U_4x8_left_line[1];
    dummy+=IP_buffer_ptr_U_4x8_left_line[2];
    dummy+=IP_buffer_ptr_U_4x8_left_line[3];
    dummy+=IP_buffer_ptr_U_4x8_left_line[4];
    dummy+=IP_buffer_ptr_U_4x8_left_line[5];
    dummy+=IP_buffer_ptr_U_4x8_left_line[6];
    dummy+=IP_buffer_ptr_U_4x8_left_line[7];
    dummy+=IP_buffer_ptr_U_4x8_left_line[8];
    dummy+=IP_buffer_ptr_U_4x8_left_line[9];
    dummy+=IP_buffer_ptr_U_4x8_left_line[10];
    dummy+=IP_buffer_ptr_U_4x8_left_line[11];
    dummy+=IP_buffer_ptr_U_4x8_left_line[12];
    dummy+=IP_buffer_ptr_U_4x8_left_line[13];
    dummy+=IP_buffer_ptr_U_4x8_left_line[14];
    dummy+=IP_buffer_ptr_U_4x8_left_line[15];

 // ::pel IP_buffer_ptr_U_4x8_top_left; 
    dummy+=IP_buffer_ptr_U_4x8_top_left;

 // ::pel[8] IP_buffer_ptr_V_4x8_up_line; 
    dummy+=IP_buffer_ptr_V_4x8_up_line[0];
    dummy+=IP_buffer_ptr_V_4x8_up_line[1];
    dummy+=IP_buffer_ptr_V_4x8_up_line[2];
    dummy+=IP_buffer_ptr_V_4x8_up_line[3];
    dummy+=IP_buffer_ptr_V_4x8_up_line[4];
    dummy+=IP_buffer_ptr_V_4x8_up_line[5];
    dummy+=IP_buffer_ptr_V_4x8_up_line[6];
    dummy+=IP_buffer_ptr_V_4x8_up_line[7];

 // ::pel[16] IP_buffer_ptr_V_4x8_left_line; 
    dummy+=IP_buffer_ptr_V_4x8_left_line[0];
    dummy+=IP_buffer_ptr_V_4x8_left_line[1];
    dummy+=IP_buffer_ptr_V_4x8_left_line[2];
    dummy+=IP_buffer_ptr_V_4x8_left_line[3];
    dummy+=IP_buffer_ptr_V_4x8_left_line[4];
    dummy+=IP_buffer_ptr_V_4x8_left_line[5];
    dummy+=IP_buffer_ptr_V_4x8_left_line[6];
    dummy+=IP_buffer_ptr_V_4x8_left_line[7];
    dummy+=IP_buffer_ptr_V_4x8_left_line[8];
    dummy+=IP_buffer_ptr_V_4x8_left_line[9];
    dummy+=IP_buffer_ptr_V_4x8_left_line[10];
    dummy+=IP_buffer_ptr_V_4x8_left_line[11];
    dummy+=IP_buffer_ptr_V_4x8_left_line[12];
    dummy+=IP_buffer_ptr_V_4x8_left_line[13];
    dummy+=IP_buffer_ptr_V_4x8_left_line[14];
    dummy+=IP_buffer_ptr_V_4x8_left_line[15];

 // ::pel IP_buffer_ptr_V_4x8_top_left; 
    dummy+=IP_buffer_ptr_V_4x8_top_left;
	
 // ::pel[8][8] mod_info_best_rec_u; 
   /* mod_info_best_rec_u[0] = dummy;
    mod_info_best_rec_u[1] = dummy;
    mod_info_best_rec_u[2] = dummy;
    mod_info_best_rec_u[3] = dummy;
    mod_info_best_rec_u[4] = dummy;
    mod_info_best_rec_u[5] = dummy;
    mod_info_best_rec_u[6] = dummy;
    mod_info_best_rec_u[7] = dummy;

    // ::pel[8][8] mod_info_best_rec_v; 
    mod_info_best_rec_v[0] = dummy;
    mod_info_best_rec_v[1] = dummy;
    mod_info_best_rec_v[2] = dummy;
    mod_info_best_rec_v[3] = dummy;
    mod_info_best_rec_v[4] = dummy;
    mod_info_best_rec_v[5] = dummy;
    mod_info_best_rec_v[6] = dummy;
    mod_info_best_rec_v[7] = dummy;

    // ::pel[8][8] mod_info_best_coef_u; 
    mod_info_best_coef_u[0] = dummy;
    mod_info_best_coef_u[1] = dummy;
    mod_info_best_coef_u[2] = dummy;
    mod_info_best_coef_u[3] = dummy;
    mod_info_best_coef_u[4] = dummy;
    mod_info_best_coef_u[5] = dummy;
    mod_info_best_coef_u[6] = dummy;
    mod_info_best_coef_u[7] = dummy;

    // ::pel[8][8] mod_info_best_coef_v; 
    mod_info_best_coef_v[0] = dummy;
    mod_info_best_coef_v[1] = dummy;
    mod_info_best_coef_v[2] = dummy;
    mod_info_best_coef_v[3] = dummy;
    mod_info_best_coef_v[4] = dummy;
    mod_info_best_coef_v[5] = dummy;
    mod_info_best_coef_v[6] = dummy;
    mod_info_best_coef_v[7] = dummy;*/
    
    

 // ::pel[16] IP_buffer_ptr_U_8x4_up_line; 
    dummy+=IP_buffer_ptr_U_8x4_up_line[0];
    dummy+=IP_buffer_ptr_U_8x4_up_line[1];
    dummy+=IP_buffer_ptr_U_8x4_up_line[2];
    dummy+=IP_buffer_ptr_U_8x4_up_line[3];
    dummy+=IP_buffer_ptr_U_8x4_up_line[4];
    dummy+=IP_buffer_ptr_U_8x4_up_line[5];
    dummy+=IP_buffer_ptr_U_8x4_up_line[6];
    dummy+=IP_buffer_ptr_U_8x4_up_line[7];
    dummy+=IP_buffer_ptr_U_8x4_up_line[8];
    dummy+=IP_buffer_ptr_U_8x4_up_line[9];
    dummy+=IP_buffer_ptr_U_8x4_up_line[10];
    dummy+=IP_buffer_ptr_U_8x4_up_line[11];
    dummy+=IP_buffer_ptr_U_8x4_up_line[12];
    dummy+=IP_buffer_ptr_U_8x4_up_line[13];
    dummy+=IP_buffer_ptr_U_8x4_up_line[14];
    dummy+=IP_buffer_ptr_U_8x4_up_line[15];

 // ::pel[8] IP_buffer_ptr_U_8x4_left_line; 
    dummy+=IP_buffer_ptr_U_8x4_left_line[0];
    dummy+=IP_buffer_ptr_U_8x4_left_line[1];
    dummy+=IP_buffer_ptr_U_8x4_left_line[2];
    dummy+=IP_buffer_ptr_U_8x4_left_line[3];
    dummy+=IP_buffer_ptr_U_8x4_left_line[4];
    dummy+=IP_buffer_ptr_U_8x4_left_line[5];
    dummy+=IP_buffer_ptr_U_8x4_left_line[6];
    dummy+=IP_buffer_ptr_U_8x4_left_line[7];

 // ::pel IP_buffer_ptr_U_8x4_top_left; 
    dummy+=IP_buffer_ptr_U_8x4_top_left;

 // ::pel[16] IP_buffer_ptr_V_8x4_up_line; 
    dummy+=IP_buffer_ptr_V_8x4_up_line[0];
    dummy+=IP_buffer_ptr_V_8x4_up_line[1];
    dummy+=IP_buffer_ptr_V_8x4_up_line[2];
    dummy+=IP_buffer_ptr_V_8x4_up_line[3];
    dummy+=IP_buffer_ptr_V_8x4_up_line[4];
    dummy+=IP_buffer_ptr_V_8x4_up_line[5];
    dummy+=IP_buffer_ptr_V_8x4_up_line[6];
    dummy+=IP_buffer_ptr_V_8x4_up_line[7];
    dummy+=IP_buffer_ptr_V_8x4_up_line[8];
    dummy+=IP_buffer_ptr_V_8x4_up_line[9];
    dummy+=IP_buffer_ptr_V_8x4_up_line[10];
    dummy+=IP_buffer_ptr_V_8x4_up_line[11];
    dummy+=IP_buffer_ptr_V_8x4_up_line[12];
    dummy+=IP_buffer_ptr_V_8x4_up_line[13];
    dummy+=IP_buffer_ptr_V_8x4_up_line[14];
    dummy+=IP_buffer_ptr_V_8x4_up_line[15];

 // ::pel[8] IP_buffer_ptr_V_8x4_left_line; 
    dummy+=IP_buffer_ptr_V_8x4_left_line[0];
    dummy+=IP_buffer_ptr_V_8x4_left_line[1];
    dummy+=IP_buffer_ptr_V_8x4_left_line[2];
    dummy+=IP_buffer_ptr_V_8x4_left_line[3];
    dummy+=IP_buffer_ptr_V_8x4_left_line[4];
    dummy+=IP_buffer_ptr_V_8x4_left_line[5];
    dummy+=IP_buffer_ptr_V_8x4_left_line[6];
    dummy+=IP_buffer_ptr_V_8x4_left_line[7];

 // ::pel IP_buffer_ptr_V_8x4_top_left; 
    dummy+=IP_buffer_ptr_V_8x4_top_left;

 // ::pel[16] IP_buffer_ptr_8x16_up_line; 
    dummy+=IP_buffer_ptr_8x16_up_line[0];
    dummy+=IP_buffer_ptr_8x16_up_line[1];
    dummy+=IP_buffer_ptr_8x16_up_line[2];
    dummy+=IP_buffer_ptr_8x16_up_line[3];
    dummy+=IP_buffer_ptr_8x16_up_line[4];
    dummy+=IP_buffer_ptr_8x16_up_line[5];
    dummy+=IP_buffer_ptr_8x16_up_line[6];
    dummy+=IP_buffer_ptr_8x16_up_line[7];
    dummy+=IP_buffer_ptr_8x16_up_line[8];
    dummy+=IP_buffer_ptr_8x16_up_line[9];
    dummy+=IP_buffer_ptr_8x16_up_line[10];
    dummy+=IP_buffer_ptr_8x16_up_line[11];
    dummy+=IP_buffer_ptr_8x16_up_line[12];
    dummy+=IP_buffer_ptr_8x16_up_line[13];
    dummy+=IP_buffer_ptr_8x16_up_line[14];
    dummy+=IP_buffer_ptr_8x16_up_line[15];

 // ::pel[32] IP_buffer_ptr_8x16_left_line; 
    dummy+=IP_buffer_ptr_8x16_left_line[0];
    dummy+=IP_buffer_ptr_8x16_left_line[1];
    dummy+=IP_buffer_ptr_8x16_left_line[2];
    dummy+=IP_buffer_ptr_8x16_left_line[3];
    dummy+=IP_buffer_ptr_8x16_left_line[4];
    dummy+=IP_buffer_ptr_8x16_left_line[5];
    dummy+=IP_buffer_ptr_8x16_left_line[6];
    dummy+=IP_buffer_ptr_8x16_left_line[7];
    dummy+=IP_buffer_ptr_8x16_left_line[8];
    dummy+=IP_buffer_ptr_8x16_left_line[9];
    dummy+=IP_buffer_ptr_8x16_left_line[10];
    dummy+=IP_buffer_ptr_8x16_left_line[11];
    dummy+=IP_buffer_ptr_8x16_left_line[12];
    dummy+=IP_buffer_ptr_8x16_left_line[13];
    dummy+=IP_buffer_ptr_8x16_left_line[14];
    dummy+=IP_buffer_ptr_8x16_left_line[15];
    dummy+=IP_buffer_ptr_8x16_left_line[16];
    dummy+=IP_buffer_ptr_8x16_left_line[17];
    dummy+=IP_buffer_ptr_8x16_left_line[18];
    dummy+=IP_buffer_ptr_8x16_left_line[19];
    dummy+=IP_buffer_ptr_8x16_left_line[20];
    dummy+=IP_buffer_ptr_8x16_left_line[21];
    dummy+=IP_buffer_ptr_8x16_left_line[22];
    dummy+=IP_buffer_ptr_8x16_left_line[23];
    dummy+=IP_buffer_ptr_8x16_left_line[24];
    dummy+=IP_buffer_ptr_8x16_left_line[25];
    dummy+=IP_buffer_ptr_8x16_left_line[26];
    dummy+=IP_buffer_ptr_8x16_left_line[27];
    dummy+=IP_buffer_ptr_8x16_left_line[28];
    dummy+=IP_buffer_ptr_8x16_left_line[29];
    dummy+=IP_buffer_ptr_8x16_left_line[30];
    dummy+=IP_buffer_ptr_8x16_left_line[31];

 // ::pel IP_buffer_ptr_8x16_top_left; 
    dummy+=IP_buffer_ptr_8x16_top_left;

 // ::pel[32] IP_buffer_ptr_16x8_up_line; 
    dummy+=IP_buffer_ptr_16x8_up_line[0];
    dummy+=IP_buffer_ptr_16x8_up_line[1];
    dummy+=IP_buffer_ptr_16x8_up_line[2];
    dummy+=IP_buffer_ptr_16x8_up_line[3];
    dummy+=IP_buffer_ptr_16x8_up_line[4];
    dummy+=IP_buffer_ptr_16x8_up_line[5];
    dummy+=IP_buffer_ptr_16x8_up_line[6];
    dummy+=IP_buffer_ptr_16x8_up_line[7];
    dummy+=IP_buffer_ptr_16x8_up_line[8];
    dummy+=IP_buffer_ptr_16x8_up_line[9];
    dummy+=IP_buffer_ptr_16x8_up_line[10];
    dummy+=IP_buffer_ptr_16x8_up_line[11];
    dummy+=IP_buffer_ptr_16x8_up_line[12];
    dummy+=IP_buffer_ptr_16x8_up_line[13];
    dummy+=IP_buffer_ptr_16x8_up_line[14];
    dummy+=IP_buffer_ptr_16x8_up_line[15];
    dummy+=IP_buffer_ptr_16x8_up_line[16];
    dummy+=IP_buffer_ptr_16x8_up_line[17];
    dummy+=IP_buffer_ptr_16x8_up_line[18];
    dummy+=IP_buffer_ptr_16x8_up_line[19];
    dummy+=IP_buffer_ptr_16x8_up_line[20];
    dummy+=IP_buffer_ptr_16x8_up_line[21];
    dummy+=IP_buffer_ptr_16x8_up_line[22];
    dummy+=IP_buffer_ptr_16x8_up_line[23];
    dummy+=IP_buffer_ptr_16x8_up_line[24];
    dummy+=IP_buffer_ptr_16x8_up_line[25];
    dummy+=IP_buffer_ptr_16x8_up_line[26];
    dummy+=IP_buffer_ptr_16x8_up_line[27];
    dummy+=IP_buffer_ptr_16x8_up_line[28];
    dummy+=IP_buffer_ptr_16x8_up_line[29];
    dummy+=IP_buffer_ptr_16x8_up_line[30];
    dummy+=IP_buffer_ptr_16x8_up_line[31];

 // ::pel[16] IP_buffer_ptr_16x8_left_line; 
    dummy+=IP_buffer_ptr_16x8_left_line[0];
    dummy+=IP_buffer_ptr_16x8_left_line[1];
    dummy+=IP_buffer_ptr_16x8_left_line[2];
    dummy+=IP_buffer_ptr_16x8_left_line[3];
    dummy+=IP_buffer_ptr_16x8_left_line[4];
    dummy+=IP_buffer_ptr_16x8_left_line[5];
    dummy+=IP_buffer_ptr_16x8_left_line[6];
    dummy+=IP_buffer_ptr_16x8_left_line[7];
    dummy+=IP_buffer_ptr_16x8_left_line[8];
    dummy+=IP_buffer_ptr_16x8_left_line[9];
    dummy+=IP_buffer_ptr_16x8_left_line[10];
    dummy+=IP_buffer_ptr_16x8_left_line[11];
    dummy+=IP_buffer_ptr_16x8_left_line[12];
    dummy+=IP_buffer_ptr_16x8_left_line[13];
    dummy+=IP_buffer_ptr_16x8_left_line[14];
    dummy+=IP_buffer_ptr_16x8_left_line[15];

 // ::pel IP_buffer_ptr_16x8_top_left; 
    dummy+=IP_buffer_ptr_16x8_top_left;

 // ::pel[32] IP_buffer_ptr_16x16_up_line; 
    dummy+=IP_buffer_ptr_16x16_up_line[0];
    dummy+=IP_buffer_ptr_16x16_up_line[1];
    dummy+=IP_buffer_ptr_16x16_up_line[2];
    dummy+=IP_buffer_ptr_16x16_up_line[3];
    dummy+=IP_buffer_ptr_16x16_up_line[4];
    dummy+=IP_buffer_ptr_16x16_up_line[5];
    dummy+=IP_buffer_ptr_16x16_up_line[6];
    dummy+=IP_buffer_ptr_16x16_up_line[7];
    dummy+=IP_buffer_ptr_16x16_up_line[8];
    dummy+=IP_buffer_ptr_16x16_up_line[9];
    dummy+=IP_buffer_ptr_16x16_up_line[10];
    dummy+=IP_buffer_ptr_16x16_up_line[11];
    dummy+=IP_buffer_ptr_16x16_up_line[12];
    dummy+=IP_buffer_ptr_16x16_up_line[13];
    dummy+=IP_buffer_ptr_16x16_up_line[14];
    dummy+=IP_buffer_ptr_16x16_up_line[15];
    dummy+=IP_buffer_ptr_16x16_up_line[16];
    dummy+=IP_buffer_ptr_16x16_up_line[17];
    dummy+=IP_buffer_ptr_16x16_up_line[18];
    dummy+=IP_buffer_ptr_16x16_up_line[19];
    dummy+=IP_buffer_ptr_16x16_up_line[20];
    dummy+=IP_buffer_ptr_16x16_up_line[21];
    dummy+=IP_buffer_ptr_16x16_up_line[22];
    dummy+=IP_buffer_ptr_16x16_up_line[23];
    dummy+=IP_buffer_ptr_16x16_up_line[24];
    dummy+=IP_buffer_ptr_16x16_up_line[25];
    dummy+=IP_buffer_ptr_16x16_up_line[26];
    dummy+=IP_buffer_ptr_16x16_up_line[27];
    dummy+=IP_buffer_ptr_16x16_up_line[28];
    dummy+=IP_buffer_ptr_16x16_up_line[29];
    dummy+=IP_buffer_ptr_16x16_up_line[30];
    dummy+=IP_buffer_ptr_16x16_up_line[31];

 // ::pel[32] IP_buffer_ptr_16x16_left_line; 
    dummy+=IP_buffer_ptr_16x16_left_line[0];
    dummy+=IP_buffer_ptr_16x16_left_line[1];
    dummy+=IP_buffer_ptr_16x16_left_line[2];
    dummy+=IP_buffer_ptr_16x16_left_line[3];
    dummy+=IP_buffer_ptr_16x16_left_line[4];
    dummy+=IP_buffer_ptr_16x16_left_line[5];
    dummy+=IP_buffer_ptr_16x16_left_line[6];
    dummy+=IP_buffer_ptr_16x16_left_line[7];
    dummy+=IP_buffer_ptr_16x16_left_line[8];
    dummy+=IP_buffer_ptr_16x16_left_line[9];
    dummy+=IP_buffer_ptr_16x16_left_line[10];
    dummy+=IP_buffer_ptr_16x16_left_line[11];
    dummy+=IP_buffer_ptr_16x16_left_line[12];
    dummy+=IP_buffer_ptr_16x16_left_line[13];
    dummy+=IP_buffer_ptr_16x16_left_line[14];
    dummy+=IP_buffer_ptr_16x16_left_line[15];
    dummy+=IP_buffer_ptr_16x16_left_line[16];
    dummy+=IP_buffer_ptr_16x16_left_line[17];
    dummy+=IP_buffer_ptr_16x16_left_line[18];
    dummy+=IP_buffer_ptr_16x16_left_line[19];
    dummy+=IP_buffer_ptr_16x16_left_line[20];
    dummy+=IP_buffer_ptr_16x16_left_line[21];
    dummy+=IP_buffer_ptr_16x16_left_line[22];
    dummy+=IP_buffer_ptr_16x16_left_line[23];
    dummy+=IP_buffer_ptr_16x16_left_line[24];
    dummy+=IP_buffer_ptr_16x16_left_line[25];
    dummy+=IP_buffer_ptr_16x16_left_line[26];
    dummy+=IP_buffer_ptr_16x16_left_line[27];
    dummy+=IP_buffer_ptr_16x16_left_line[28];
    dummy+=IP_buffer_ptr_16x16_left_line[29];
    dummy+=IP_buffer_ptr_16x16_left_line[30];
    dummy+=IP_buffer_ptr_16x16_left_line[31];

 // ::pel IP_buffer_ptr_16x16_top_left; 
    dummy+=IP_buffer_ptr_16x16_top_left;

 // ::U32[4] IP_map_scu_ptr_8x16_map_scu_up_line; 
    dummy+=IP_map_scu_ptr_8x16_map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_8x16_map_scu_up_line[2];
    dummy+=IP_map_scu_ptr_8x16_map_scu_up_line[3];

 // ::U32[8] IP_map_scu_ptr_8x16_map_scu_left_line_new; 
    dummy+=IP_map_scu_ptr_8x16_map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_8x16_map_scu_left_line_new[4];
    dummy+=IP_map_scu_ptr_8x16_map_scu_left_line_new[5];
    dummy+=IP_map_scu_ptr_8x16_map_scu_left_line_new[6];
    dummy+=IP_map_scu_ptr_8x16_map_scu_left_line_new[7];

 // ::U32 IP_map_scu_ptr_8x16_map_scu_top_left_new; 
    dummy+=IP_map_scu_ptr_8x16_map_scu_top_left_new;

 // ::U32[8] IP_map_scu_ptr_16x8_map_scu_up_line; 
    dummy+=IP_map_scu_ptr_16x8_map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x8_map_scu_up_line[4];
    dummy+=IP_map_scu_ptr_16x8_map_scu_up_line[5];
    dummy+=IP_map_scu_ptr_16x8_map_scu_up_line[6];
    dummy+=IP_map_scu_ptr_16x8_map_scu_up_line[7];

 // ::U32[4] IP_map_scu_ptr_16x8_map_scu_left_line_new; 
    dummy+=IP_map_scu_ptr_16x8_map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x8_map_scu_left_line_new[2];
    dummy+=IP_map_scu_ptr_16x8_map_scu_left_line_new[3];

 // ::U32 IP_map_scu_ptr_16x8_map_scu_top_left_new; 
    dummy+=IP_map_scu_ptr_16x8_map_scu_top_left_new;

 // ::U32[8] IP_map_scu_ptr_16x16_map_scu_up_line; 
    dummy+=IP_map_scu_ptr_16x16_map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x16_map_scu_up_line[4];
    dummy+=IP_map_scu_ptr_16x16_map_scu_up_line[5];
    dummy+=IP_map_scu_ptr_16x16_map_scu_up_line[6];
    dummy+=IP_map_scu_ptr_16x16_map_scu_up_line[7];

 // ::U32[8] IP_map_scu_ptr_16x16_map_scu_left_line_new; 
    dummy+=IP_map_scu_ptr_16x16_map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x16_map_scu_left_line_new[4];
    dummy+=IP_map_scu_ptr_16x16_map_scu_left_line_new[5];
    dummy+=IP_map_scu_ptr_16x16_map_scu_left_line_new[6];
    dummy+=IP_map_scu_ptr_16x16_map_scu_left_line_new[7];

 // ::U32 IP_map_scu_ptr_16x16_map_scu_top_left_new; 
    dummy+=IP_map_scu_ptr_16x16_map_scu_top_left_new;
    
    for (i = 0; i < 8; i = i + 1) {
        for (j = 0; j < 8; j = j + 1) {
            mod_info_best_rec_u[i][j] = dummy;
        }
    }
    for (i = 0; i < 8; i = i + 1) {
        for (j = 0; j < 8; j = j + 1) {
            mod_info_best_rec_v[i][j] = dummy;
        }
    }
    for (i = 0; i < 8; i = i + 1) {
        for (j = 0; j < 8; j = j + 1) {
            mod_info_best_coef_u[i][j] = dummy;
        }
    }
    for (i = 0; i < 8; i = i + 1) {
        for (j = 0; j < 8; j = j + 1) {
            mod_info_best_coef_v[i][j] = dummy;
        }
    }
#pragma HLS resource variable=mod_info_best[0].ipm core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best[0].num_nz core=RAM_1P_BRAM 

#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_UP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_VP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1 cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1 cyclic factor=128 dim=1

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_v core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_v core=RAM_1P_BRAM 
#pragma HLS resource variable=orgU core=RAM_1P_BRAM 
#pragma HLS resource variable=piRecoY core=RAM_2P_BRAM   // liaozx
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].cur_refi complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].lidx complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=2
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=2
#pragma HLS array_partition variable=IP_buffer_ptr_16x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_16x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_16x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_16x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_8x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_8x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_4x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_4x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_8x4_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_8x4_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_8x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_8x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_4x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_4x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_8x4_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_8x4_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_8x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_8x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_16x16_map_scu_left_line_new complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_16x16_map_scu_up_line complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_16x8_map_scu_left_line_new complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_16x8_map_scu_up_line complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_8x16_map_scu_left_line_new complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_8x16_map_scu_up_line complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].mv complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].mv complete dim=2
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].ref_idx complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].dist_chroma_weight complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].lambda complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].refi complete dim=1
#pragma HLS array_partition variable=mod_info_best_coef_u cyclic factor=8 dim=1
#pragma HLS array_partition variable=mod_info_best_coef_v cyclic factor=8 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_u cyclic factor=8 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_v cyclic factor=8 dim=1
#pragma HLS array_partition variable=orgU cyclic factor=2 dim=1
#pragma HLS array_partition variable=orgU cyclic factor=8 dim=2
#pragma HLS array_partition variable=piRecoY cyclic factor=2 dim=1
#pragma HLS array_partition variable=piRecoY cyclic factor=16 dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=3
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=4
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=2
}
#endif // CHROMA_8_PIPELINE_DUMMY

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
	U32 IP_map_scu_ptr_32x32_map_scu_top_left_new)
{
   s64 dummy = 0;
   // dummy_template.cpp
 // ::u8 all_rdo_num; 
    dummy+=all_rdo_num;

 // ::MD_COM_MODE_BEST* mod_info_best; 
    dummy+=mod_info_best[0] . x_scu;
    dummy+=mod_info_best[0] . y_scu;
    // mod_info_best[0] . cud is ignored ;
    dummy+=mod_info_best[0] . cu_width;
    dummy+=mod_info_best[0] . cu_height;
    // mod_info_best[0] . cu_width_log2 is ignored ;
    // mod_info_best[0] . cu_height_log2 is ignored ;
    dummy+=mod_info_best[0] . x_pos;
    dummy+=mod_info_best[0] . y_pos;
    // mod_info_best[0] . scup is ignored ;
    dummy+=mod_info_best[0] . cu_mode;
    // mod_info_best[0] . pb_part is ignored ;
    // mod_info_best[0] . tb_part is ignored ;
    // mod_info_best[0] . rec_y[0] is ignored ;
    mod_info_best[0].rec_u[0]=dummy;
    mod_info_best[0].rec_u[1]=dummy;
    mod_info_best[0].rec_u[2]=dummy;
    mod_info_best[0].rec_u[3]=dummy;
    mod_info_best[0].rec_u[4]=dummy;
    mod_info_best[0].rec_u[5]=dummy;
    mod_info_best[0].rec_u[6]=dummy;
    mod_info_best[0].rec_u[7]=dummy;
    mod_info_best[0].rec_u[8]=dummy;
    mod_info_best[0].rec_u[9]=dummy;
    mod_info_best[0].rec_u[10]=dummy;
    mod_info_best[0].rec_u[11]=dummy;
    mod_info_best[0].rec_u[12]=dummy;
    mod_info_best[0].rec_u[13]=dummy;
    mod_info_best[0].rec_u[14]=dummy;
    mod_info_best[0].rec_u[15]=dummy;
    mod_info_best[0].rec_v[0]=dummy;
    mod_info_best[0].rec_v[1]=dummy;
    mod_info_best[0].rec_v[2]=dummy;
    mod_info_best[0].rec_v[3]=dummy;
    mod_info_best[0].rec_v[4]=dummy;
    mod_info_best[0].rec_v[5]=dummy;
    mod_info_best[0].rec_v[6]=dummy;
    mod_info_best[0].rec_v[7]=dummy;
    mod_info_best[0].rec_v[8]=dummy;
    mod_info_best[0].rec_v[9]=dummy;
    mod_info_best[0].rec_v[10]=dummy;
    mod_info_best[0].rec_v[11]=dummy;
    mod_info_best[0].rec_v[12]=dummy;
    mod_info_best[0].rec_v[13]=dummy;
    mod_info_best[0].rec_v[14]=dummy;
    mod_info_best[0].rec_v[15]=dummy;
    // mod_info_best[0] . coef_y[0] is ignored ;
    mod_info_best[0].coef_u[0]=dummy;
    mod_info_best[0].coef_u[1]=dummy;
    mod_info_best[0].coef_u[2]=dummy;
    mod_info_best[0].coef_u[3]=dummy;
    mod_info_best[0].coef_u[4]=dummy;
    mod_info_best[0].coef_u[5]=dummy;
    mod_info_best[0].coef_u[6]=dummy;
    mod_info_best[0].coef_u[7]=dummy;
    mod_info_best[0].coef_u[8]=dummy;
    mod_info_best[0].coef_u[9]=dummy;
    mod_info_best[0].coef_u[10]=dummy;
    mod_info_best[0].coef_u[11]=dummy;
    mod_info_best[0].coef_u[12]=dummy;
    mod_info_best[0].coef_u[13]=dummy;
    mod_info_best[0].coef_u[14]=dummy;
    (mod_info_best[0].coef_u[15])=(mod_info_best[0].coef_u[15])=dummy;
    mod_info_best[0].coef_v[0]=dummy;
    mod_info_best[0].coef_v[1]=dummy;
    mod_info_best[0].coef_v[2]=dummy;
    mod_info_best[0].coef_v[3]=dummy;
    mod_info_best[0].coef_v[4]=dummy;
    mod_info_best[0].coef_v[5]=dummy;
    mod_info_best[0].coef_v[6]=dummy;
    mod_info_best[0].coef_v[7]=dummy;
    mod_info_best[0].coef_v[8]=dummy;
    mod_info_best[0].coef_v[9]=dummy;
    mod_info_best[0].coef_v[10]=dummy;
    mod_info_best[0].coef_v[11]=dummy;
    mod_info_best[0].coef_v[12]=dummy;
    mod_info_best[0].coef_v[13]=dummy;
    mod_info_best[0].coef_v[14]=dummy;
    (mod_info_best[0].coef_v[15])=(mod_info_best[0].coef_v[15])=dummy;
    // mod_info_best[0] . pred_y[0][0] is ignored ;
    // mod_info_best[0] . pred_u[0] is ignored ;
    // mod_info_best[0] . pred_v[0] is ignored ;
    // mod_info_best[0] . num_nz[0][0] is ignored ;
    dummy+=mod_info_best[0].refi[0];
    dummy+=mod_info_best[0].refi[1];
    // mod_info_best[0] . mvr_idx is ignored ;
    // mod_info_best[0] . umve_flag is ignored ;
    // mod_info_best[0] . umve_idx is ignored ;
    dummy+=mod_info_best[0] . skip_idx;
    // mod_info_best[0] . mvp_from_hmvp_flag is ignored ;
    // mod_info_best[0] . inter_filter_flag is ignored ;
    // mod_info_best[0] . mvd[0][0] is ignored ;
    dummy+=mod_info_best[0].mv[0][0];
    dummy+=mod_info_best[0].mv[0][1];
    dummy+=mod_info_best[0].mv[1][0];
    dummy+=mod_info_best[0].mv[1][1];
    // mod_info_best[0] . affine_flag is ignored ;
    // mod_info_best[0] . affine_mv[0][0][0] is ignored ;
    // mod_info_best[0] . affine_mvd[0][0][0] is ignored ;
    // mod_info_best[0] . smvd_flag is ignored ;
    // mod_info_best[0] . mpm[0][0] is ignored ;
    dummy+=mod_info_best[0].ipm[0][0];
    // mod_info_best[0] . ipf_flag is ignored ;
    // mod_info_best[0] . chroma_motion is ignored ;
    // mod_info_best[0] . x_luma is ignored ;
    // mod_info_best[0] . y_luma is ignored ;
    // mod_info_best[0] . x_chroma is ignored ;
    // mod_info_best[0] . y_chroma is ignored ;
    // mod_info_best[0] . last_child is ignored ;

 // ::RDOQ_ARRAY* rdoq_array; 
    dummy+=rdoq_array[0].rdoq_est_run[0][0];
    dummy+=rdoq_array[0].rdoq_est_run[0][1];
    dummy+=rdoq_array[0].rdoq_est_run[1][0];
    dummy+=rdoq_array[0].rdoq_est_run[1][1];
    dummy+=rdoq_array[0].rdoq_est_run[2][0];
    dummy+=rdoq_array[0].rdoq_est_run[2][1];
    dummy+=rdoq_array[0].rdoq_est_run[3][0];
    dummy+=rdoq_array[0].rdoq_est_run[3][1];
    dummy+=rdoq_array[0].rdoq_est_run[4][0];
    dummy+=rdoq_array[0].rdoq_est_run[4][1];
    dummy+=rdoq_array[0].rdoq_est_run[5][0];
    dummy+=rdoq_array[0].rdoq_est_run[5][1];
    dummy+=rdoq_array[0].rdoq_est_run[6][0];
    dummy+=rdoq_array[0].rdoq_est_run[6][1];
    dummy+=rdoq_array[0].rdoq_est_run[7][0];
    dummy+=rdoq_array[0].rdoq_est_run[7][1];
    dummy+=rdoq_array[0].rdoq_est_run[8][0];
    dummy+=rdoq_array[0].rdoq_est_run[8][1];
    dummy+=rdoq_array[0].rdoq_est_run[9][0];
    dummy+=rdoq_array[0].rdoq_est_run[9][1];
    dummy+=rdoq_array[0].rdoq_est_run[10][0];
    dummy+=rdoq_array[0].rdoq_est_run[10][1];
    dummy+=rdoq_array[0].rdoq_est_run[11][0];
    dummy+=rdoq_array[0].rdoq_est_run[11][1];
    dummy+=rdoq_array[0].rdoq_est_run[12][0];
    dummy+=rdoq_array[0].rdoq_est_run[12][1];
    dummy+=rdoq_array[0].rdoq_est_run[13][0];
    dummy+=rdoq_array[0].rdoq_est_run[13][1];
    dummy+=rdoq_array[0].rdoq_est_run[14][0];
    dummy+=rdoq_array[0].rdoq_est_run[14][1];
    dummy+=rdoq_array[0].rdoq_est_run[15][0];
    dummy+=rdoq_array[0].rdoq_est_run[15][1];
    dummy+=rdoq_array[0].rdoq_est_run[16][0];
    dummy+=rdoq_array[0].rdoq_est_run[16][1];
    dummy+=rdoq_array[0].rdoq_est_run[17][0];
    dummy+=rdoq_array[0].rdoq_est_run[17][1];
    dummy+=rdoq_array[0].rdoq_est_run[18][0];
    dummy+=rdoq_array[0].rdoq_est_run[18][1];
    dummy+=rdoq_array[0].rdoq_est_run[19][0];
    dummy+=rdoq_array[0].rdoq_est_run[19][1];
    dummy+=rdoq_array[0].rdoq_est_run[20][0];
    dummy+=rdoq_array[0].rdoq_est_run[20][1];
    dummy+=rdoq_array[0].rdoq_est_run[21][0];
    dummy+=rdoq_array[0].rdoq_est_run[21][1];
    dummy+=rdoq_array[0].rdoq_est_run[22][0];
    dummy+=rdoq_array[0].rdoq_est_run[22][1];
    dummy+=rdoq_array[0].rdoq_est_run[23][0];
    dummy+=rdoq_array[0].rdoq_est_run[23][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[0][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[1][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[1][1];
    dummy+=rdoq_array[0].rdoq_est_cbf[2][0];
    dummy+=rdoq_array[0].rdoq_est_cbf[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[0][0];
    dummy+=rdoq_array[0].rdoq_est_level[0][1];
    dummy+=rdoq_array[0].rdoq_est_level[1][0];
    dummy+=rdoq_array[0].rdoq_est_level[1][1];
    dummy+=rdoq_array[0].rdoq_est_level[2][0];
    dummy+=rdoq_array[0].rdoq_est_level[2][1];
    dummy+=rdoq_array[0].rdoq_est_level[3][0];
    dummy+=rdoq_array[0].rdoq_est_level[3][1];
    dummy+=rdoq_array[0].rdoq_est_level[4][0];
    dummy+=rdoq_array[0].rdoq_est_level[4][1];
    dummy+=rdoq_array[0].rdoq_est_level[5][0];
    dummy+=rdoq_array[0].rdoq_est_level[5][1];
    dummy+=rdoq_array[0].rdoq_est_level[6][0];
    dummy+=rdoq_array[0].rdoq_est_level[6][1];
    dummy+=rdoq_array[0].rdoq_est_level[7][0];
    dummy+=rdoq_array[0].rdoq_est_level[7][1];
    dummy+=rdoq_array[0].rdoq_est_level[8][0];
    dummy+=rdoq_array[0].rdoq_est_level[8][1];
    dummy+=rdoq_array[0].rdoq_est_level[9][0];
    dummy+=rdoq_array[0].rdoq_est_level[9][1];
    dummy+=rdoq_array[0].rdoq_est_level[10][0];
    dummy+=rdoq_array[0].rdoq_est_level[10][1];
    dummy+=rdoq_array[0].rdoq_est_level[11][0];
    dummy+=rdoq_array[0].rdoq_est_level[11][1];
    dummy+=rdoq_array[0].rdoq_est_level[12][0];
    dummy+=rdoq_array[0].rdoq_est_level[12][1];
    dummy+=rdoq_array[0].rdoq_est_level[13][0];
    dummy+=rdoq_array[0].rdoq_est_level[13][1];
    dummy+=rdoq_array[0].rdoq_est_level[14][0];
    dummy+=rdoq_array[0].rdoq_est_level[14][1];
    dummy+=rdoq_array[0].rdoq_est_level[15][0];
    dummy+=rdoq_array[0].rdoq_est_level[15][1];
    dummy+=rdoq_array[0].rdoq_est_level[16][0];
    dummy+=rdoq_array[0].rdoq_est_level[16][1];
    dummy+=rdoq_array[0].rdoq_est_level[17][0];
    dummy+=rdoq_array[0].rdoq_est_level[17][1];
    dummy+=rdoq_array[0].rdoq_est_level[18][0];
    dummy+=rdoq_array[0].rdoq_est_level[18][1];
    dummy+=rdoq_array[0].rdoq_est_level[19][0];
    dummy+=rdoq_array[0].rdoq_est_level[19][1];
    dummy+=rdoq_array[0].rdoq_est_level[20][0];
    dummy+=rdoq_array[0].rdoq_est_level[20][1];
    dummy+=rdoq_array[0].rdoq_est_level[21][0];
    dummy+=rdoq_array[0].rdoq_est_level[21][1];
    dummy+=rdoq_array[0].rdoq_est_level[22][0];
    dummy+=rdoq_array[0].rdoq_est_level[22][1];
    dummy+=rdoq_array[0].rdoq_est_level[23][0];
    dummy+=rdoq_array[0].rdoq_est_level[23][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][0][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][1][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][2][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][3][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][4][11][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][0][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][0][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][1][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][1][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][2][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][2][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][3][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][3][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][4][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][4][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][5][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][5][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][6][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][6][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][7][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][7][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][8][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][8][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][9][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][9][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][10][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][10][1];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][11][0];
    dummy+=rdoq_array[0].rdoq_est_last[1][5][11][1];

 // ::MD_FW* md_fw_ptr; 
    dummy+=md_fw_ptr[0] . pic_width;
    dummy+=md_fw_ptr[0] . pic_height;
    // md_fw_ptr[0] . slice_type is ignored ;
    // md_fw_ptr[0] . slice_qp is ignored ;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . pic_width_in_scu;
    dummy+=md_fw_ptr[0] . pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy+=md_fw_ptr[0] . bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    dummy+=md_fw_ptr[0] . mipf_enable_flag;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    // md_fw_ptr[0] . refp_ptr[0][0] is ignored ;
    // md_fw_ptr[0] . num_refp[0] is ignored ;
    // md_fw_ptr[0] . cur_ptr is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cr is ignored ;

 // ::U2 tree_status; 
    // tree_status is ignored ;

 // ::pel[12288] RefWindowLCU_UP; 
    // RefWindowLCU_UP[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP; 
    // RefWindowLCU_VP[0] is ignored ;

 // ::S14[2][2][2] CtrPos; 
    // CtrPos[0][0][0] is ignored ;

 // ::pel[12288] RefWindowLCU_UP_ref1; 
    // RefWindowLCU_UP_ref1[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP_ref1; 
    // RefWindowLCU_VP_ref1[0] is ignored ;

 // ::S14[2][2][2] CtrPos_ref1; 
    // CtrPos_ref1[0][0][0] is ignored ;

 // ::MD_INPUT* md_input_ptr; 
    // md_input_ptr[0] . pix_x is ignored ;
    // md_input_ptr[0] . pix_y is ignored ;
    // md_input_ptr[0] . lambda_mv is ignored ;
    dummy+=md_input_ptr[0].lambda[0];
    dummy+=md_input_ptr[0].lambda[1];
    // md_input_ptr[0] . lambda_y is ignored ;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    // md_input_ptr[0] . qp_y is ignored ;
    dummy+=md_input_ptr[0] . qp_u;
    // md_input_ptr[0] . qp_v is ignored ;
    dummy+=md_input_ptr[0].dist_chroma_weight[0];
    dummy+=md_input_ptr[0].dist_chroma_weight[1];
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0][0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;
    // md_input_ptr[0] . cu_dqp_enable is ignored ;
    // md_input_ptr[0] . cu_qp_group_area_size is ignored ;
    // md_input_ptr[0] . cu_qp_group_pred_qp is ignored ;
    // md_input_ptr[0] . cu_luma_qp[0] is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cr is ignored ;
    // md_input_ptr[0] . md_slice_qp is ignored ;
    // md_input_ptr[0] . CuQPMap[0] is ignored ;

 // ::pel[2][16][16] orgU; 
    dummy+=orgU[0][ 0][0];
    dummy+=orgU[0][ 1][0];
    dummy+=orgU[0][ 2][0];
    dummy+=orgU[0][ 3][0];
    dummy+=orgU[0][ 4][0];
    dummy+=orgU[0][ 5][0];
    dummy+=orgU[0][ 6][0];
    dummy+=orgU[0][ 7][0];
    dummy+=orgU[0][ 8][0];
    dummy+=orgU[0][ 9][0];
    dummy+=orgU[0][10][0];
    dummy+=orgU[0][11][0];
    dummy+=orgU[0][12][0];
    dummy+=orgU[0][13][0];
    dummy+=orgU[0][14][0];
    dummy+=orgU[0][15][0];
    dummy+=orgU[1][ 0][0];
    dummy+=orgU[1][ 1][0];
    dummy+=orgU[1][ 2][0];
    dummy+=orgU[1][ 3][0];
    dummy+=orgU[1][ 4][0];
    dummy+=orgU[1][ 5][0];
    dummy+=orgU[1][ 6][0];
    dummy+=orgU[1][ 7][0];
    dummy+=orgU[1][ 8][0];
    dummy+=orgU[1][ 9][0];
    dummy+=orgU[1][10][0];
    dummy+=orgU[1][11][0];
    dummy+=orgU[1][12][0];
    dummy+=orgU[1][13][0];
    dummy+=orgU[1][14][0];
    dummy+=orgU[1][15][0];

 // ::S8 ipm; 
    dummy+=ipm;

 // ::U7 cu_width; 
    dummy+=cu_width;

 // ::U7 cu_height; 
    dummy+=cu_height;

 // ::U3 cu_width_log2; 
    dummy+=cu_width_log2;

 // ::U3 cu_height_log2; 
    dummy+=cu_height_log2;

 // ::U6 qp; 
    // qp is ignored ;

 // ::s64 lambda; 
    // lambda is ignored ;

 // ::U1 is_intra_quant; 
    // is_intra_quant is ignored ;

 // int mod_info_nz_tb_part; 
    // mod_info_nz_tb_part is ignored ;

 // ::s64 lambda_y; 
    // lambda_y is ignored ;

 // ::U4 bit_depth; 
    dummy+=bit_depth;

 // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
    // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP[1]/*TODO:*/);
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP[1]/*TODO:*/);
    dummy+=Fetch_Ref_window_ptr[0] . code_stru;
    dummy+=Fetch_Ref_window_ptr[0].lidx[0];
    dummy+=Fetch_Ref_window_ptr[0].lidx[1];
    dummy+=Fetch_Ref_window_ptr[0].cur_refi[0];
    dummy+=Fetch_Ref_window_ptr[0].cur_refi[1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[0][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[0][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[1][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[1][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[2][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[2][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[3][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[3][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[4][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[4][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[5][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[5][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[6][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[6][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[7][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[7][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[8][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[8][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[9][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[9][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[10][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[10][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[11][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[11][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[12][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[12][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[13][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[13][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[14][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[14][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[15][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[15][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[16][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_pic_ptr[16][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[0][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[0][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[1][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[1][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[2][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[2][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[3][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[3][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[4][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[4][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[5][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[5][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[6][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[6][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[7][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[7][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[8][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[8][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[9][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[9][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[10][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[10][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[11][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[11][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[12][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[12][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[13][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[13][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[14][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[14][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[15][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[15][1];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[16][0];
    dummy+=Fetch_Ref_window_ptr[0].refp_is_library_picture[16][1];
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_UP_ref1[1]/*TODO:*/);
    dummy+=(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[0])+(Fetch_Ref_window_ptr[0] . RefWindowLCU_VP_ref1[1]/*TODO:*/);

 // ::pel[32][32] piRecoY; 
    dummy+=piRecoY[0][0];
    dummy+=piRecoY[0][1];
    dummy+=piRecoY[0][2];
    dummy+=(piRecoY[0][3])+(piRecoY[0][3]);
    dummy+=piRecoY[0][4];
    dummy+=(piRecoY[0][5])+(piRecoY[0][5]);
    dummy+=piRecoY[0][6];
    dummy+=(piRecoY[0][7])+(piRecoY[0][7]);
    dummy+=piRecoY[0][8];
    dummy+=(piRecoY[0][9])+(piRecoY[0][9]);
    dummy+=piRecoY[0][10];
    dummy+=(piRecoY[0][11])+(piRecoY[0][11]);
    dummy+=piRecoY[0][12];
    dummy+=(piRecoY[0][13])+(piRecoY[0][13]);
    dummy+=piRecoY[0][14];
    dummy+=(piRecoY[0][15])+(piRecoY[0][15]);
    dummy+=piRecoY[0][16];
    dummy+=(piRecoY[0][17])+(piRecoY[0][17]);
    dummy+=piRecoY[0][18];
    dummy+=(piRecoY[0][19])+(piRecoY[0][19]);
    dummy+=piRecoY[0][20];
    dummy+=(piRecoY[0][21])+(piRecoY[0][21]);
    dummy+=piRecoY[0][22];
    dummy+=(piRecoY[0][23])+(piRecoY[0][23]);
    dummy+=piRecoY[0][24];
    dummy+=(piRecoY[0][25])+(piRecoY[0][25]);
    dummy+=piRecoY[0][26];
    dummy+=(piRecoY[0][27])+(piRecoY[0][27]);
    dummy+=piRecoY[0][28];
    dummy+=(piRecoY[0][29])+(piRecoY[0][29]);
    dummy+=piRecoY[0][30];
    dummy+=piRecoY[0][31];
    dummy+=piRecoY[1][0];
    dummy+=piRecoY[1][1];
    dummy+=piRecoY[1][2];
    dummy+=(piRecoY[1][3])+(piRecoY[1][3]);
    dummy+=piRecoY[1][4];
    dummy+=(piRecoY[1][5])+(piRecoY[1][5]);
    dummy+=piRecoY[1][6];
    dummy+=(piRecoY[1][7])+(piRecoY[1][7]);
    dummy+=piRecoY[1][8];
    dummy+=(piRecoY[1][9])+(piRecoY[1][9]);
    dummy+=piRecoY[1][10];
    dummy+=(piRecoY[1][11])+(piRecoY[1][11]);
    dummy+=piRecoY[1][12];
    dummy+=(piRecoY[1][13])+(piRecoY[1][13]);
    dummy+=piRecoY[1][14];
    dummy+=(piRecoY[1][15])+(piRecoY[1][15]);
    dummy+=piRecoY[1][16];
    dummy+=(piRecoY[1][17])+(piRecoY[1][17]);
    dummy+=piRecoY[1][18];
    dummy+=(piRecoY[1][19])+(piRecoY[1][19]);
    dummy+=piRecoY[1][20];
    dummy+=(piRecoY[1][21])+(piRecoY[1][21]);
    dummy+=piRecoY[1][22];
    dummy+=(piRecoY[1][23])+(piRecoY[1][23]);
    dummy+=piRecoY[1][24];
    dummy+=(piRecoY[1][25])+(piRecoY[1][25]);
    dummy+=piRecoY[1][26];
    dummy+=(piRecoY[1][27])+(piRecoY[1][27]);
    dummy+=piRecoY[1][28];
    dummy+=(piRecoY[1][29])+(piRecoY[1][29]);
    dummy+=piRecoY[1][30];
    dummy+=piRecoY[1][31];

 // ::CORE_SBTMVP* core_sbtmvp; 
    dummy+=core_sbtmvp[0] . sbTmvp_flag;
    // core_sbtmvp[0] . best_sbTmvp_flag is ignored ;
    dummy+=core_sbtmvp[0].sbTmvp[0].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[0].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[1].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[1].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[2].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[2].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[3].mv[0][0];
    dummy+=core_sbtmvp[0].sbTmvp[3].mv[0][1];
    dummy+=core_sbtmvp[0].sbTmvp[0].ref_idx[0];
    dummy+=core_sbtmvp[0].sbTmvp[1].ref_idx[0];
    dummy+=core_sbtmvp[0].sbTmvp[2].ref_idx[0];
    dummy+=core_sbtmvp[0].sbTmvp[3].ref_idx[0];
    // core_sbtmvp[0] . best_sbTmvp[0] . mv[0][0] is ignored ;
    // core_sbtmvp[0] . best_sbTmvp[0] . ref_idx[0] is ignored ;
    // core_sbtmvp[0] . isValid is ignored ;

 // ::pel[16][16] mod_info_best_rec_u; 
    mod_info_best_rec_u[ 0][0]=dummy;
    mod_info_best_rec_u[ 1][0]=dummy;
    mod_info_best_rec_u[ 2][0]=dummy;
    mod_info_best_rec_u[ 3][0]=dummy;
    mod_info_best_rec_u[ 4][0]=dummy;
    mod_info_best_rec_u[ 5][0]=dummy;
    mod_info_best_rec_u[ 6][0]=dummy;
    mod_info_best_rec_u[ 7][0]=dummy;
    mod_info_best_rec_u[ 8][0]=dummy;
    mod_info_best_rec_u[ 9][0]=dummy;
    mod_info_best_rec_u[10][0]=dummy;
    mod_info_best_rec_u[11][0]=dummy;
    mod_info_best_rec_u[12][0]=dummy;
    mod_info_best_rec_u[13][0]=dummy;
    mod_info_best_rec_u[14][0]=dummy;
    mod_info_best_rec_u[15][0]=dummy;

 // ::pel[16][16] mod_info_best_rec_v; 
    mod_info_best_rec_v[ 0][0]=dummy;
    mod_info_best_rec_v[ 1][0]=dummy;
    mod_info_best_rec_v[ 2][0]=dummy;
    mod_info_best_rec_v[ 3][0]=dummy;
    mod_info_best_rec_v[ 4][0]=dummy;
    mod_info_best_rec_v[ 5][0]=dummy;
    mod_info_best_rec_v[ 6][0]=dummy;
    mod_info_best_rec_v[ 7][0]=dummy;
    mod_info_best_rec_v[ 8][0]=dummy;
    mod_info_best_rec_v[ 9][0]=dummy;
    mod_info_best_rec_v[10][0]=dummy;
    mod_info_best_rec_v[11][0]=dummy;
    mod_info_best_rec_v[12][0]=dummy;
    mod_info_best_rec_v[13][0]=dummy;
    mod_info_best_rec_v[14][0]=dummy;
    mod_info_best_rec_v[15][0]=dummy;

 // ::pel[16][16] mod_info_best_coef_u; 
    mod_info_best_coef_u[ 0][0]=dummy;
    mod_info_best_coef_u[ 1][0]=dummy;
    mod_info_best_coef_u[ 2][0]=dummy;
    mod_info_best_coef_u[ 3][0]=dummy;
    mod_info_best_coef_u[ 4][0]=dummy;
    mod_info_best_coef_u[ 5][0]=dummy;
    mod_info_best_coef_u[ 6][0]=dummy;
    mod_info_best_coef_u[ 7][0]=dummy;
    mod_info_best_coef_u[ 8][0]=dummy;
    mod_info_best_coef_u[ 9][0]=dummy;
    mod_info_best_coef_u[10][0]=dummy;
    mod_info_best_coef_u[11][0]=dummy;
    mod_info_best_coef_u[12][0]=dummy;
    mod_info_best_coef_u[13][0]=dummy;
    mod_info_best_coef_u[14][0]=dummy;
    mod_info_best_coef_u[15][0]=dummy;

 // ::pel[16][16] mod_info_best_coef_v; 
    mod_info_best_coef_v[ 0][0]=dummy;
    mod_info_best_coef_v[ 1][0]=dummy;
    mod_info_best_coef_v[ 2][0]=dummy;
    mod_info_best_coef_v[ 3][0]=dummy;
    mod_info_best_coef_v[ 4][0]=dummy;
    mod_info_best_coef_v[ 5][0]=dummy;
    mod_info_best_coef_v[ 6][0]=dummy;
    mod_info_best_coef_v[ 7][0]=dummy;
    mod_info_best_coef_v[ 8][0]=dummy;
    mod_info_best_coef_v[ 9][0]=dummy;
    mod_info_best_coef_v[10][0]=dummy;
    mod_info_best_coef_v[11][0]=dummy;
    mod_info_best_coef_v[12][0]=dummy;
    mod_info_best_coef_v[13][0]=dummy;
    mod_info_best_coef_v[14][0]=dummy;
    mod_info_best_coef_v[15][0]=dummy;

 // ::pel[32] IP_buffer_ptr_U_16x16_up_line; 
    dummy+=IP_buffer_ptr_U_16x16_up_line[0];
    dummy+=IP_buffer_ptr_U_16x16_up_line[1];
    dummy+=IP_buffer_ptr_U_16x16_up_line[2];
    dummy+=IP_buffer_ptr_U_16x16_up_line[3];
    dummy+=IP_buffer_ptr_U_16x16_up_line[4];
    dummy+=IP_buffer_ptr_U_16x16_up_line[5];
    dummy+=IP_buffer_ptr_U_16x16_up_line[6];
    dummy+=IP_buffer_ptr_U_16x16_up_line[7];
    dummy+=IP_buffer_ptr_U_16x16_up_line[8];
    dummy+=IP_buffer_ptr_U_16x16_up_line[9];
    dummy+=IP_buffer_ptr_U_16x16_up_line[10];
    dummy+=IP_buffer_ptr_U_16x16_up_line[11];
    dummy+=IP_buffer_ptr_U_16x16_up_line[12];
    dummy+=IP_buffer_ptr_U_16x16_up_line[13];
    dummy+=IP_buffer_ptr_U_16x16_up_line[14];
    dummy+=IP_buffer_ptr_U_16x16_up_line[15];
    dummy+=IP_buffer_ptr_U_16x16_up_line[16];
    dummy+=IP_buffer_ptr_U_16x16_up_line[17];
    dummy+=IP_buffer_ptr_U_16x16_up_line[18];
    dummy+=IP_buffer_ptr_U_16x16_up_line[19];
    dummy+=IP_buffer_ptr_U_16x16_up_line[20];
    dummy+=IP_buffer_ptr_U_16x16_up_line[21];
    dummy+=IP_buffer_ptr_U_16x16_up_line[22];
    dummy+=IP_buffer_ptr_U_16x16_up_line[23];
    dummy+=IP_buffer_ptr_U_16x16_up_line[24];
    dummy+=IP_buffer_ptr_U_16x16_up_line[25];
    dummy+=IP_buffer_ptr_U_16x16_up_line[26];
    dummy+=IP_buffer_ptr_U_16x16_up_line[27];
    dummy+=IP_buffer_ptr_U_16x16_up_line[28];
    dummy+=IP_buffer_ptr_U_16x16_up_line[29];
    dummy+=IP_buffer_ptr_U_16x16_up_line[30];
    dummy+=IP_buffer_ptr_U_16x16_up_line[31];

 // ::pel[32] IP_buffer_ptr_U_16x16_left_line; 
    dummy+=IP_buffer_ptr_U_16x16_left_line[0];
    dummy+=IP_buffer_ptr_U_16x16_left_line[1];
    dummy+=IP_buffer_ptr_U_16x16_left_line[2];
    dummy+=IP_buffer_ptr_U_16x16_left_line[3];
    dummy+=IP_buffer_ptr_U_16x16_left_line[4];
    dummy+=IP_buffer_ptr_U_16x16_left_line[5];
    dummy+=IP_buffer_ptr_U_16x16_left_line[6];
    dummy+=IP_buffer_ptr_U_16x16_left_line[7];
    dummy+=IP_buffer_ptr_U_16x16_left_line[8];
    dummy+=IP_buffer_ptr_U_16x16_left_line[9];
    dummy+=IP_buffer_ptr_U_16x16_left_line[10];
    dummy+=IP_buffer_ptr_U_16x16_left_line[11];
    dummy+=IP_buffer_ptr_U_16x16_left_line[12];
    dummy+=IP_buffer_ptr_U_16x16_left_line[13];
    dummy+=IP_buffer_ptr_U_16x16_left_line[14];
    dummy+=IP_buffer_ptr_U_16x16_left_line[15];
    dummy+=IP_buffer_ptr_U_16x16_left_line[16];
    dummy+=IP_buffer_ptr_U_16x16_left_line[17];
    dummy+=IP_buffer_ptr_U_16x16_left_line[18];
    dummy+=IP_buffer_ptr_U_16x16_left_line[19];
    dummy+=IP_buffer_ptr_U_16x16_left_line[20];
    dummy+=IP_buffer_ptr_U_16x16_left_line[21];
    dummy+=IP_buffer_ptr_U_16x16_left_line[22];
    dummy+=IP_buffer_ptr_U_16x16_left_line[23];
    dummy+=IP_buffer_ptr_U_16x16_left_line[24];
    dummy+=IP_buffer_ptr_U_16x16_left_line[25];
    dummy+=IP_buffer_ptr_U_16x16_left_line[26];
    dummy+=IP_buffer_ptr_U_16x16_left_line[27];
    dummy+=IP_buffer_ptr_U_16x16_left_line[28];
    dummy+=IP_buffer_ptr_U_16x16_left_line[29];
    dummy+=IP_buffer_ptr_U_16x16_left_line[30];
    dummy+=IP_buffer_ptr_U_16x16_left_line[31];

 // ::pel IP_buffer_ptr_U_16x16_top_left; 
    dummy+=IP_buffer_ptr_U_16x16_top_left;

 // ::pel[32] IP_buffer_ptr_V_16x16_up_line; 
    dummy+=IP_buffer_ptr_V_16x16_up_line[0];
    dummy+=IP_buffer_ptr_V_16x16_up_line[1];
    dummy+=IP_buffer_ptr_V_16x16_up_line[2];
    dummy+=IP_buffer_ptr_V_16x16_up_line[3];
    dummy+=IP_buffer_ptr_V_16x16_up_line[4];
    dummy+=IP_buffer_ptr_V_16x16_up_line[5];
    dummy+=IP_buffer_ptr_V_16x16_up_line[6];
    dummy+=IP_buffer_ptr_V_16x16_up_line[7];
    dummy+=IP_buffer_ptr_V_16x16_up_line[8];
    dummy+=IP_buffer_ptr_V_16x16_up_line[9];
    dummy+=IP_buffer_ptr_V_16x16_up_line[10];
    dummy+=IP_buffer_ptr_V_16x16_up_line[11];
    dummy+=IP_buffer_ptr_V_16x16_up_line[12];
    dummy+=IP_buffer_ptr_V_16x16_up_line[13];
    dummy+=IP_buffer_ptr_V_16x16_up_line[14];
    dummy+=IP_buffer_ptr_V_16x16_up_line[15];
    dummy+=IP_buffer_ptr_V_16x16_up_line[16];
    dummy+=IP_buffer_ptr_V_16x16_up_line[17];
    dummy+=IP_buffer_ptr_V_16x16_up_line[18];
    dummy+=IP_buffer_ptr_V_16x16_up_line[19];
    dummy+=IP_buffer_ptr_V_16x16_up_line[20];
    dummy+=IP_buffer_ptr_V_16x16_up_line[21];
    dummy+=IP_buffer_ptr_V_16x16_up_line[22];
    dummy+=IP_buffer_ptr_V_16x16_up_line[23];
    dummy+=IP_buffer_ptr_V_16x16_up_line[24];
    dummy+=IP_buffer_ptr_V_16x16_up_line[25];
    dummy+=IP_buffer_ptr_V_16x16_up_line[26];
    dummy+=IP_buffer_ptr_V_16x16_up_line[27];
    dummy+=IP_buffer_ptr_V_16x16_up_line[28];
    dummy+=IP_buffer_ptr_V_16x16_up_line[29];
    dummy+=IP_buffer_ptr_V_16x16_up_line[30];
    dummy+=IP_buffer_ptr_V_16x16_up_line[31];

 // ::pel[32] IP_buffer_ptr_V_16x16_left_line; 
    dummy+=IP_buffer_ptr_V_16x16_left_line[0];
    dummy+=IP_buffer_ptr_V_16x16_left_line[1];
    dummy+=IP_buffer_ptr_V_16x16_left_line[2];
    dummy+=IP_buffer_ptr_V_16x16_left_line[3];
    dummy+=IP_buffer_ptr_V_16x16_left_line[4];
    dummy+=IP_buffer_ptr_V_16x16_left_line[5];
    dummy+=IP_buffer_ptr_V_16x16_left_line[6];
    dummy+=IP_buffer_ptr_V_16x16_left_line[7];
    dummy+=IP_buffer_ptr_V_16x16_left_line[8];
    dummy+=IP_buffer_ptr_V_16x16_left_line[9];
    dummy+=IP_buffer_ptr_V_16x16_left_line[10];
    dummy+=IP_buffer_ptr_V_16x16_left_line[11];
    dummy+=IP_buffer_ptr_V_16x16_left_line[12];
    dummy+=IP_buffer_ptr_V_16x16_left_line[13];
    dummy+=IP_buffer_ptr_V_16x16_left_line[14];
    dummy+=IP_buffer_ptr_V_16x16_left_line[15];
    dummy+=IP_buffer_ptr_V_16x16_left_line[16];
    dummy+=IP_buffer_ptr_V_16x16_left_line[17];
    dummy+=IP_buffer_ptr_V_16x16_left_line[18];
    dummy+=IP_buffer_ptr_V_16x16_left_line[19];
    dummy+=IP_buffer_ptr_V_16x16_left_line[20];
    dummy+=IP_buffer_ptr_V_16x16_left_line[21];
    dummy+=IP_buffer_ptr_V_16x16_left_line[22];
    dummy+=IP_buffer_ptr_V_16x16_left_line[23];
    dummy+=IP_buffer_ptr_V_16x16_left_line[24];
    dummy+=IP_buffer_ptr_V_16x16_left_line[25];
    dummy+=IP_buffer_ptr_V_16x16_left_line[26];
    dummy+=IP_buffer_ptr_V_16x16_left_line[27];
    dummy+=IP_buffer_ptr_V_16x16_left_line[28];
    dummy+=IP_buffer_ptr_V_16x16_left_line[29];
    dummy+=IP_buffer_ptr_V_16x16_left_line[30];
    dummy+=IP_buffer_ptr_V_16x16_left_line[31];

 // ::pel IP_buffer_ptr_V_16x16_top_left; 
    dummy+=IP_buffer_ptr_V_16x16_top_left;

 // ::pel[16] IP_buffer_ptr_U_8x16_up_line; 
    dummy+=IP_buffer_ptr_U_8x16_up_line[0];
    dummy+=IP_buffer_ptr_U_8x16_up_line[1];
    dummy+=IP_buffer_ptr_U_8x16_up_line[2];
    dummy+=IP_buffer_ptr_U_8x16_up_line[3];
    dummy+=IP_buffer_ptr_U_8x16_up_line[4];
    dummy+=IP_buffer_ptr_U_8x16_up_line[5];
    dummy+=IP_buffer_ptr_U_8x16_up_line[6];
    dummy+=IP_buffer_ptr_U_8x16_up_line[7];
    dummy+=IP_buffer_ptr_U_8x16_up_line[8];
    dummy+=IP_buffer_ptr_U_8x16_up_line[9];
    dummy+=IP_buffer_ptr_U_8x16_up_line[10];
    dummy+=IP_buffer_ptr_U_8x16_up_line[11];
    dummy+=IP_buffer_ptr_U_8x16_up_line[12];
    dummy+=IP_buffer_ptr_U_8x16_up_line[13];
    dummy+=IP_buffer_ptr_U_8x16_up_line[14];
    dummy+=IP_buffer_ptr_U_8x16_up_line[15];

 // ::pel[32] IP_buffer_ptr_U_8x16_left_line; 
    dummy+=IP_buffer_ptr_U_8x16_left_line[0];
    dummy+=IP_buffer_ptr_U_8x16_left_line[1];
    dummy+=IP_buffer_ptr_U_8x16_left_line[2];
    dummy+=IP_buffer_ptr_U_8x16_left_line[3];
    dummy+=IP_buffer_ptr_U_8x16_left_line[4];
    dummy+=IP_buffer_ptr_U_8x16_left_line[5];
    dummy+=IP_buffer_ptr_U_8x16_left_line[6];
    dummy+=IP_buffer_ptr_U_8x16_left_line[7];
    dummy+=IP_buffer_ptr_U_8x16_left_line[8];
    dummy+=IP_buffer_ptr_U_8x16_left_line[9];
    dummy+=IP_buffer_ptr_U_8x16_left_line[10];
    dummy+=IP_buffer_ptr_U_8x16_left_line[11];
    dummy+=IP_buffer_ptr_U_8x16_left_line[12];
    dummy+=IP_buffer_ptr_U_8x16_left_line[13];
    dummy+=IP_buffer_ptr_U_8x16_left_line[14];
    dummy+=IP_buffer_ptr_U_8x16_left_line[15];
    dummy+=IP_buffer_ptr_U_8x16_left_line[16];
    dummy+=IP_buffer_ptr_U_8x16_left_line[17];
    dummy+=IP_buffer_ptr_U_8x16_left_line[18];
    dummy+=IP_buffer_ptr_U_8x16_left_line[19];
    dummy+=IP_buffer_ptr_U_8x16_left_line[20];
    dummy+=IP_buffer_ptr_U_8x16_left_line[21];
    dummy+=IP_buffer_ptr_U_8x16_left_line[22];
    dummy+=IP_buffer_ptr_U_8x16_left_line[23];
    dummy+=IP_buffer_ptr_U_8x16_left_line[24];
    dummy+=IP_buffer_ptr_U_8x16_left_line[25];
    dummy+=IP_buffer_ptr_U_8x16_left_line[26];
    dummy+=IP_buffer_ptr_U_8x16_left_line[27];
    dummy+=IP_buffer_ptr_U_8x16_left_line[28];
    dummy+=IP_buffer_ptr_U_8x16_left_line[29];
    dummy+=IP_buffer_ptr_U_8x16_left_line[30];
    dummy+=IP_buffer_ptr_U_8x16_left_line[31];

 // ::pel IP_buffer_ptr_U_8x16_top_left; 
    dummy+=IP_buffer_ptr_U_8x16_top_left;

 // ::pel[16] IP_buffer_ptr_V_8x16_up_line; 
    dummy+=IP_buffer_ptr_V_8x16_up_line[0];
    dummy+=IP_buffer_ptr_V_8x16_up_line[1];
    dummy+=IP_buffer_ptr_V_8x16_up_line[2];
    dummy+=IP_buffer_ptr_V_8x16_up_line[3];
    dummy+=IP_buffer_ptr_V_8x16_up_line[4];
    dummy+=IP_buffer_ptr_V_8x16_up_line[5];
    dummy+=IP_buffer_ptr_V_8x16_up_line[6];
    dummy+=IP_buffer_ptr_V_8x16_up_line[7];
    dummy+=IP_buffer_ptr_V_8x16_up_line[8];
    dummy+=IP_buffer_ptr_V_8x16_up_line[9];
    dummy+=IP_buffer_ptr_V_8x16_up_line[10];
    dummy+=IP_buffer_ptr_V_8x16_up_line[11];
    dummy+=IP_buffer_ptr_V_8x16_up_line[12];
    dummy+=IP_buffer_ptr_V_8x16_up_line[13];
    dummy+=IP_buffer_ptr_V_8x16_up_line[14];
    dummy+=IP_buffer_ptr_V_8x16_up_line[15];

 // ::pel[32] IP_buffer_ptr_V_8x16_left_line; 
    dummy+=IP_buffer_ptr_V_8x16_left_line[0];
    dummy+=IP_buffer_ptr_V_8x16_left_line[1];
    dummy+=IP_buffer_ptr_V_8x16_left_line[2];
    dummy+=IP_buffer_ptr_V_8x16_left_line[3];
    dummy+=IP_buffer_ptr_V_8x16_left_line[4];
    dummy+=IP_buffer_ptr_V_8x16_left_line[5];
    dummy+=IP_buffer_ptr_V_8x16_left_line[6];
    dummy+=IP_buffer_ptr_V_8x16_left_line[7];
    dummy+=IP_buffer_ptr_V_8x16_left_line[8];
    dummy+=IP_buffer_ptr_V_8x16_left_line[9];
    dummy+=IP_buffer_ptr_V_8x16_left_line[10];
    dummy+=IP_buffer_ptr_V_8x16_left_line[11];
    dummy+=IP_buffer_ptr_V_8x16_left_line[12];
    dummy+=IP_buffer_ptr_V_8x16_left_line[13];
    dummy+=IP_buffer_ptr_V_8x16_left_line[14];
    dummy+=IP_buffer_ptr_V_8x16_left_line[15];
    dummy+=IP_buffer_ptr_V_8x16_left_line[16];
    dummy+=IP_buffer_ptr_V_8x16_left_line[17];
    dummy+=IP_buffer_ptr_V_8x16_left_line[18];
    dummy+=IP_buffer_ptr_V_8x16_left_line[19];
    dummy+=IP_buffer_ptr_V_8x16_left_line[20];
    dummy+=IP_buffer_ptr_V_8x16_left_line[21];
    dummy+=IP_buffer_ptr_V_8x16_left_line[22];
    dummy+=IP_buffer_ptr_V_8x16_left_line[23];
    dummy+=IP_buffer_ptr_V_8x16_left_line[24];
    dummy+=IP_buffer_ptr_V_8x16_left_line[25];
    dummy+=IP_buffer_ptr_V_8x16_left_line[26];
    dummy+=IP_buffer_ptr_V_8x16_left_line[27];
    dummy+=IP_buffer_ptr_V_8x16_left_line[28];
    dummy+=IP_buffer_ptr_V_8x16_left_line[29];
    dummy+=IP_buffer_ptr_V_8x16_left_line[30];
    dummy+=IP_buffer_ptr_V_8x16_left_line[31];

 // ::pel IP_buffer_ptr_V_8x16_top_left; 
    dummy+=IP_buffer_ptr_V_8x16_top_left;

 // ::pel[32] IP_buffer_ptr_U_16x8_up_line; 
    dummy+=IP_buffer_ptr_U_16x8_up_line[0];
    dummy+=IP_buffer_ptr_U_16x8_up_line[1];
    dummy+=IP_buffer_ptr_U_16x8_up_line[2];
    dummy+=IP_buffer_ptr_U_16x8_up_line[3];
    dummy+=IP_buffer_ptr_U_16x8_up_line[4];
    dummy+=IP_buffer_ptr_U_16x8_up_line[5];
    dummy+=IP_buffer_ptr_U_16x8_up_line[6];
    dummy+=IP_buffer_ptr_U_16x8_up_line[7];
    dummy+=IP_buffer_ptr_U_16x8_up_line[8];
    dummy+=IP_buffer_ptr_U_16x8_up_line[9];
    dummy+=IP_buffer_ptr_U_16x8_up_line[10];
    dummy+=IP_buffer_ptr_U_16x8_up_line[11];
    dummy+=IP_buffer_ptr_U_16x8_up_line[12];
    dummy+=IP_buffer_ptr_U_16x8_up_line[13];
    dummy+=IP_buffer_ptr_U_16x8_up_line[14];
    dummy+=IP_buffer_ptr_U_16x8_up_line[15];
    dummy+=IP_buffer_ptr_U_16x8_up_line[16];
    dummy+=IP_buffer_ptr_U_16x8_up_line[17];
    dummy+=IP_buffer_ptr_U_16x8_up_line[18];
    dummy+=IP_buffer_ptr_U_16x8_up_line[19];
    dummy+=IP_buffer_ptr_U_16x8_up_line[20];
    dummy+=IP_buffer_ptr_U_16x8_up_line[21];
    dummy+=IP_buffer_ptr_U_16x8_up_line[22];
    dummy+=IP_buffer_ptr_U_16x8_up_line[23];
    dummy+=IP_buffer_ptr_U_16x8_up_line[24];
    dummy+=IP_buffer_ptr_U_16x8_up_line[25];
    dummy+=IP_buffer_ptr_U_16x8_up_line[26];
    dummy+=IP_buffer_ptr_U_16x8_up_line[27];
    dummy+=IP_buffer_ptr_U_16x8_up_line[28];
    dummy+=IP_buffer_ptr_U_16x8_up_line[29];
    dummy+=IP_buffer_ptr_U_16x8_up_line[30];
    dummy+=IP_buffer_ptr_U_16x8_up_line[31];

 // ::pel[16] IP_buffer_ptr_U_16x8_left_line; 
    dummy+=IP_buffer_ptr_U_16x8_left_line[0];
    dummy+=IP_buffer_ptr_U_16x8_left_line[1];
    dummy+=IP_buffer_ptr_U_16x8_left_line[2];
    dummy+=IP_buffer_ptr_U_16x8_left_line[3];
    dummy+=IP_buffer_ptr_U_16x8_left_line[4];
    dummy+=IP_buffer_ptr_U_16x8_left_line[5];
    dummy+=IP_buffer_ptr_U_16x8_left_line[6];
    dummy+=IP_buffer_ptr_U_16x8_left_line[7];
    dummy+=IP_buffer_ptr_U_16x8_left_line[8];
    dummy+=IP_buffer_ptr_U_16x8_left_line[9];
    dummy+=IP_buffer_ptr_U_16x8_left_line[10];
    dummy+=IP_buffer_ptr_U_16x8_left_line[11];
    dummy+=IP_buffer_ptr_U_16x8_left_line[12];
    dummy+=IP_buffer_ptr_U_16x8_left_line[13];
    dummy+=IP_buffer_ptr_U_16x8_left_line[14];
    dummy+=IP_buffer_ptr_U_16x8_left_line[15];

 // ::pel IP_buffer_ptr_U_16x8_top_left; 
    dummy+=IP_buffer_ptr_U_16x8_top_left;

 // ::pel[32] IP_buffer_ptr_V_16x8_up_line; 
    dummy+=IP_buffer_ptr_V_16x8_up_line[0];
    dummy+=IP_buffer_ptr_V_16x8_up_line[1];
    dummy+=IP_buffer_ptr_V_16x8_up_line[2];
    dummy+=IP_buffer_ptr_V_16x8_up_line[3];
    dummy+=IP_buffer_ptr_V_16x8_up_line[4];
    dummy+=IP_buffer_ptr_V_16x8_up_line[5];
    dummy+=IP_buffer_ptr_V_16x8_up_line[6];
    dummy+=IP_buffer_ptr_V_16x8_up_line[7];
    dummy+=IP_buffer_ptr_V_16x8_up_line[8];
    dummy+=IP_buffer_ptr_V_16x8_up_line[9];
    dummy+=IP_buffer_ptr_V_16x8_up_line[10];
    dummy+=IP_buffer_ptr_V_16x8_up_line[11];
    dummy+=IP_buffer_ptr_V_16x8_up_line[12];
    dummy+=IP_buffer_ptr_V_16x8_up_line[13];
    dummy+=IP_buffer_ptr_V_16x8_up_line[14];
    dummy+=IP_buffer_ptr_V_16x8_up_line[15];
    dummy+=IP_buffer_ptr_V_16x8_up_line[16];
    dummy+=IP_buffer_ptr_V_16x8_up_line[17];
    dummy+=IP_buffer_ptr_V_16x8_up_line[18];
    dummy+=IP_buffer_ptr_V_16x8_up_line[19];
    dummy+=IP_buffer_ptr_V_16x8_up_line[20];
    dummy+=IP_buffer_ptr_V_16x8_up_line[21];
    dummy+=IP_buffer_ptr_V_16x8_up_line[22];
    dummy+=IP_buffer_ptr_V_16x8_up_line[23];
    dummy+=IP_buffer_ptr_V_16x8_up_line[24];
    dummy+=IP_buffer_ptr_V_16x8_up_line[25];
    dummy+=IP_buffer_ptr_V_16x8_up_line[26];
    dummy+=IP_buffer_ptr_V_16x8_up_line[27];
    dummy+=IP_buffer_ptr_V_16x8_up_line[28];
    dummy+=IP_buffer_ptr_V_16x8_up_line[29];
    dummy+=IP_buffer_ptr_V_16x8_up_line[30];
    dummy+=IP_buffer_ptr_V_16x8_up_line[31];

 // ::pel[16] IP_buffer_ptr_V_16x8_left_line; 
    dummy+=IP_buffer_ptr_V_16x8_left_line[0];
    dummy+=IP_buffer_ptr_V_16x8_left_line[1];
    dummy+=IP_buffer_ptr_V_16x8_left_line[2];
    dummy+=IP_buffer_ptr_V_16x8_left_line[3];
    dummy+=IP_buffer_ptr_V_16x8_left_line[4];
    dummy+=IP_buffer_ptr_V_16x8_left_line[5];
    dummy+=IP_buffer_ptr_V_16x8_left_line[6];
    dummy+=IP_buffer_ptr_V_16x8_left_line[7];
    dummy+=IP_buffer_ptr_V_16x8_left_line[8];
    dummy+=IP_buffer_ptr_V_16x8_left_line[9];
    dummy+=IP_buffer_ptr_V_16x8_left_line[10];
    dummy+=IP_buffer_ptr_V_16x8_left_line[11];
    dummy+=IP_buffer_ptr_V_16x8_left_line[12];
    dummy+=IP_buffer_ptr_V_16x8_left_line[13];
    dummy+=IP_buffer_ptr_V_16x8_left_line[14];
    dummy+=IP_buffer_ptr_V_16x8_left_line[15];

 // ::pel IP_buffer_ptr_V_16x8_top_left; 
    dummy+=IP_buffer_ptr_V_16x8_top_left;

 // ::pel[32] IP_buffer_ptr_16x32_up_line; 
    dummy+=IP_buffer_ptr_16x32_up_line[0];
    dummy+=IP_buffer_ptr_16x32_up_line[1];
    dummy+=IP_buffer_ptr_16x32_up_line[2];
    dummy+=IP_buffer_ptr_16x32_up_line[3];
    dummy+=IP_buffer_ptr_16x32_up_line[4];
    dummy+=IP_buffer_ptr_16x32_up_line[5];
    dummy+=IP_buffer_ptr_16x32_up_line[6];
    dummy+=IP_buffer_ptr_16x32_up_line[7];
    dummy+=IP_buffer_ptr_16x32_up_line[8];
    dummy+=IP_buffer_ptr_16x32_up_line[9];
    dummy+=IP_buffer_ptr_16x32_up_line[10];
    dummy+=IP_buffer_ptr_16x32_up_line[11];
    dummy+=IP_buffer_ptr_16x32_up_line[12];
    dummy+=IP_buffer_ptr_16x32_up_line[13];
    dummy+=IP_buffer_ptr_16x32_up_line[14];
    dummy+=IP_buffer_ptr_16x32_up_line[15];
    dummy+=IP_buffer_ptr_16x32_up_line[16];
    dummy+=IP_buffer_ptr_16x32_up_line[17];
    dummy+=IP_buffer_ptr_16x32_up_line[18];
    dummy+=IP_buffer_ptr_16x32_up_line[19];
    dummy+=IP_buffer_ptr_16x32_up_line[20];
    dummy+=IP_buffer_ptr_16x32_up_line[21];
    dummy+=IP_buffer_ptr_16x32_up_line[22];
    dummy+=IP_buffer_ptr_16x32_up_line[23];
    dummy+=IP_buffer_ptr_16x32_up_line[24];
    dummy+=IP_buffer_ptr_16x32_up_line[25];
    dummy+=IP_buffer_ptr_16x32_up_line[26];
    dummy+=IP_buffer_ptr_16x32_up_line[27];
    dummy+=IP_buffer_ptr_16x32_up_line[28];
    dummy+=IP_buffer_ptr_16x32_up_line[29];
    dummy+=IP_buffer_ptr_16x32_up_line[30];
    dummy+=IP_buffer_ptr_16x32_up_line[31];

 // ::pel[64] IP_buffer_ptr_16x32_left_line; 
    dummy+=IP_buffer_ptr_16x32_left_line[0];
    dummy+=IP_buffer_ptr_16x32_left_line[1];
    dummy+=IP_buffer_ptr_16x32_left_line[2];
    dummy+=IP_buffer_ptr_16x32_left_line[3];
    dummy+=IP_buffer_ptr_16x32_left_line[4];
    dummy+=IP_buffer_ptr_16x32_left_line[5];
    dummy+=IP_buffer_ptr_16x32_left_line[6];
    dummy+=IP_buffer_ptr_16x32_left_line[7];
    dummy+=IP_buffer_ptr_16x32_left_line[8];
    dummy+=IP_buffer_ptr_16x32_left_line[9];
    dummy+=IP_buffer_ptr_16x32_left_line[10];
    dummy+=IP_buffer_ptr_16x32_left_line[11];
    dummy+=IP_buffer_ptr_16x32_left_line[12];
    dummy+=IP_buffer_ptr_16x32_left_line[13];
    dummy+=IP_buffer_ptr_16x32_left_line[14];
    dummy+=IP_buffer_ptr_16x32_left_line[15];
    dummy+=IP_buffer_ptr_16x32_left_line[16];
    dummy+=IP_buffer_ptr_16x32_left_line[17];
    dummy+=IP_buffer_ptr_16x32_left_line[18];
    dummy+=IP_buffer_ptr_16x32_left_line[19];
    dummy+=IP_buffer_ptr_16x32_left_line[20];
    dummy+=IP_buffer_ptr_16x32_left_line[21];
    dummy+=IP_buffer_ptr_16x32_left_line[22];
    dummy+=IP_buffer_ptr_16x32_left_line[23];
    dummy+=IP_buffer_ptr_16x32_left_line[24];
    dummy+=IP_buffer_ptr_16x32_left_line[25];
    dummy+=IP_buffer_ptr_16x32_left_line[26];
    dummy+=IP_buffer_ptr_16x32_left_line[27];
    dummy+=IP_buffer_ptr_16x32_left_line[28];
    dummy+=IP_buffer_ptr_16x32_left_line[29];
    dummy+=IP_buffer_ptr_16x32_left_line[30];
    dummy+=IP_buffer_ptr_16x32_left_line[31];
    dummy+=IP_buffer_ptr_16x32_left_line[32];
    dummy+=IP_buffer_ptr_16x32_left_line[33];
    dummy+=IP_buffer_ptr_16x32_left_line[34];
    dummy+=IP_buffer_ptr_16x32_left_line[35];
    dummy+=IP_buffer_ptr_16x32_left_line[36];
    dummy+=IP_buffer_ptr_16x32_left_line[37];
    dummy+=IP_buffer_ptr_16x32_left_line[38];
    dummy+=IP_buffer_ptr_16x32_left_line[39];
    dummy+=IP_buffer_ptr_16x32_left_line[40];
    dummy+=IP_buffer_ptr_16x32_left_line[41];
    dummy+=IP_buffer_ptr_16x32_left_line[42];
    dummy+=IP_buffer_ptr_16x32_left_line[43];
    dummy+=IP_buffer_ptr_16x32_left_line[44];
    dummy+=IP_buffer_ptr_16x32_left_line[45];
    dummy+=IP_buffer_ptr_16x32_left_line[46];
    dummy+=IP_buffer_ptr_16x32_left_line[47];
    dummy+=IP_buffer_ptr_16x32_left_line[48];
    dummy+=IP_buffer_ptr_16x32_left_line[49];
    dummy+=IP_buffer_ptr_16x32_left_line[50];
    dummy+=IP_buffer_ptr_16x32_left_line[51];
    dummy+=IP_buffer_ptr_16x32_left_line[52];
    dummy+=IP_buffer_ptr_16x32_left_line[53];
    dummy+=IP_buffer_ptr_16x32_left_line[54];
    dummy+=IP_buffer_ptr_16x32_left_line[55];
    dummy+=IP_buffer_ptr_16x32_left_line[56];
    dummy+=IP_buffer_ptr_16x32_left_line[57];
    dummy+=IP_buffer_ptr_16x32_left_line[58];
    dummy+=IP_buffer_ptr_16x32_left_line[59];
    dummy+=IP_buffer_ptr_16x32_left_line[60];
    dummy+=IP_buffer_ptr_16x32_left_line[61];
    dummy+=IP_buffer_ptr_16x32_left_line[62];
    dummy+=IP_buffer_ptr_16x32_left_line[63];

 // ::pel IP_buffer_ptr_16x32_top_left; 
    dummy+=IP_buffer_ptr_16x32_top_left;

 // ::pel[64] IP_buffer_ptr_32x16_up_line; 
    dummy+=IP_buffer_ptr_32x16_up_line[0];
    dummy+=IP_buffer_ptr_32x16_up_line[1];
    dummy+=IP_buffer_ptr_32x16_up_line[2];
    dummy+=IP_buffer_ptr_32x16_up_line[3];
    dummy+=IP_buffer_ptr_32x16_up_line[4];
    dummy+=IP_buffer_ptr_32x16_up_line[5];
    dummy+=IP_buffer_ptr_32x16_up_line[6];
    dummy+=IP_buffer_ptr_32x16_up_line[7];
    dummy+=IP_buffer_ptr_32x16_up_line[8];
    dummy+=IP_buffer_ptr_32x16_up_line[9];
    dummy+=IP_buffer_ptr_32x16_up_line[10];
    dummy+=IP_buffer_ptr_32x16_up_line[11];
    dummy+=IP_buffer_ptr_32x16_up_line[12];
    dummy+=IP_buffer_ptr_32x16_up_line[13];
    dummy+=IP_buffer_ptr_32x16_up_line[14];
    dummy+=IP_buffer_ptr_32x16_up_line[15];
    dummy+=IP_buffer_ptr_32x16_up_line[16];
    dummy+=IP_buffer_ptr_32x16_up_line[17];
    dummy+=IP_buffer_ptr_32x16_up_line[18];
    dummy+=IP_buffer_ptr_32x16_up_line[19];
    dummy+=IP_buffer_ptr_32x16_up_line[20];
    dummy+=IP_buffer_ptr_32x16_up_line[21];
    dummy+=IP_buffer_ptr_32x16_up_line[22];
    dummy+=IP_buffer_ptr_32x16_up_line[23];
    dummy+=IP_buffer_ptr_32x16_up_line[24];
    dummy+=IP_buffer_ptr_32x16_up_line[25];
    dummy+=IP_buffer_ptr_32x16_up_line[26];
    dummy+=IP_buffer_ptr_32x16_up_line[27];
    dummy+=IP_buffer_ptr_32x16_up_line[28];
    dummy+=IP_buffer_ptr_32x16_up_line[29];
    dummy+=IP_buffer_ptr_32x16_up_line[30];
    dummy+=IP_buffer_ptr_32x16_up_line[31];
    dummy+=IP_buffer_ptr_32x16_up_line[32];
    dummy+=IP_buffer_ptr_32x16_up_line[33];
    dummy+=IP_buffer_ptr_32x16_up_line[34];
    dummy+=IP_buffer_ptr_32x16_up_line[35];
    dummy+=IP_buffer_ptr_32x16_up_line[36];
    dummy+=IP_buffer_ptr_32x16_up_line[37];
    dummy+=IP_buffer_ptr_32x16_up_line[38];
    dummy+=IP_buffer_ptr_32x16_up_line[39];
    dummy+=IP_buffer_ptr_32x16_up_line[40];
    dummy+=IP_buffer_ptr_32x16_up_line[41];
    dummy+=IP_buffer_ptr_32x16_up_line[42];
    dummy+=IP_buffer_ptr_32x16_up_line[43];
    dummy+=IP_buffer_ptr_32x16_up_line[44];
    dummy+=IP_buffer_ptr_32x16_up_line[45];
    dummy+=IP_buffer_ptr_32x16_up_line[46];
    dummy+=IP_buffer_ptr_32x16_up_line[47];
    dummy+=IP_buffer_ptr_32x16_up_line[48];
    dummy+=IP_buffer_ptr_32x16_up_line[49];
    dummy+=IP_buffer_ptr_32x16_up_line[50];
    dummy+=IP_buffer_ptr_32x16_up_line[51];
    dummy+=IP_buffer_ptr_32x16_up_line[52];
    dummy+=IP_buffer_ptr_32x16_up_line[53];
    dummy+=IP_buffer_ptr_32x16_up_line[54];
    dummy+=IP_buffer_ptr_32x16_up_line[55];
    dummy+=IP_buffer_ptr_32x16_up_line[56];
    dummy+=IP_buffer_ptr_32x16_up_line[57];
    dummy+=IP_buffer_ptr_32x16_up_line[58];
    dummy+=IP_buffer_ptr_32x16_up_line[59];
    dummy+=IP_buffer_ptr_32x16_up_line[60];
    dummy+=IP_buffer_ptr_32x16_up_line[61];
    dummy+=IP_buffer_ptr_32x16_up_line[62];
    dummy+=IP_buffer_ptr_32x16_up_line[63];

 // ::pel[32] IP_buffer_ptr_32x16_left_line; 
    dummy+=IP_buffer_ptr_32x16_left_line[0];
    dummy+=IP_buffer_ptr_32x16_left_line[1];
    dummy+=IP_buffer_ptr_32x16_left_line[2];
    dummy+=IP_buffer_ptr_32x16_left_line[3];
    dummy+=IP_buffer_ptr_32x16_left_line[4];
    dummy+=IP_buffer_ptr_32x16_left_line[5];
    dummy+=IP_buffer_ptr_32x16_left_line[6];
    dummy+=IP_buffer_ptr_32x16_left_line[7];
    dummy+=IP_buffer_ptr_32x16_left_line[8];
    dummy+=IP_buffer_ptr_32x16_left_line[9];
    dummy+=IP_buffer_ptr_32x16_left_line[10];
    dummy+=IP_buffer_ptr_32x16_left_line[11];
    dummy+=IP_buffer_ptr_32x16_left_line[12];
    dummy+=IP_buffer_ptr_32x16_left_line[13];
    dummy+=IP_buffer_ptr_32x16_left_line[14];
    dummy+=IP_buffer_ptr_32x16_left_line[15];
    dummy+=IP_buffer_ptr_32x16_left_line[16];
    dummy+=IP_buffer_ptr_32x16_left_line[17];
    dummy+=IP_buffer_ptr_32x16_left_line[18];
    dummy+=IP_buffer_ptr_32x16_left_line[19];
    dummy+=IP_buffer_ptr_32x16_left_line[20];
    dummy+=IP_buffer_ptr_32x16_left_line[21];
    dummy+=IP_buffer_ptr_32x16_left_line[22];
    dummy+=IP_buffer_ptr_32x16_left_line[23];
    dummy+=IP_buffer_ptr_32x16_left_line[24];
    dummy+=IP_buffer_ptr_32x16_left_line[25];
    dummy+=IP_buffer_ptr_32x16_left_line[26];
    dummy+=IP_buffer_ptr_32x16_left_line[27];
    dummy+=IP_buffer_ptr_32x16_left_line[28];
    dummy+=IP_buffer_ptr_32x16_left_line[29];
    dummy+=IP_buffer_ptr_32x16_left_line[30];
    dummy+=IP_buffer_ptr_32x16_left_line[31];

 // ::pel IP_buffer_ptr_32x16_top_left; 
    dummy+=IP_buffer_ptr_32x16_top_left;

 // ::pel[64] IP_buffer_ptr_32x32_up_line; 
    dummy+=IP_buffer_ptr_32x32_up_line[0];
    dummy+=IP_buffer_ptr_32x32_up_line[1];
    dummy+=IP_buffer_ptr_32x32_up_line[2];
    dummy+=IP_buffer_ptr_32x32_up_line[3];
    dummy+=IP_buffer_ptr_32x32_up_line[4];
    dummy+=IP_buffer_ptr_32x32_up_line[5];
    dummy+=IP_buffer_ptr_32x32_up_line[6];
    dummy+=IP_buffer_ptr_32x32_up_line[7];
    dummy+=IP_buffer_ptr_32x32_up_line[8];
    dummy+=IP_buffer_ptr_32x32_up_line[9];
    dummy+=IP_buffer_ptr_32x32_up_line[10];
    dummy+=IP_buffer_ptr_32x32_up_line[11];
    dummy+=IP_buffer_ptr_32x32_up_line[12];
    dummy+=IP_buffer_ptr_32x32_up_line[13];
    dummy+=IP_buffer_ptr_32x32_up_line[14];
    dummy+=IP_buffer_ptr_32x32_up_line[15];
    dummy+=IP_buffer_ptr_32x32_up_line[16];
    dummy+=IP_buffer_ptr_32x32_up_line[17];
    dummy+=IP_buffer_ptr_32x32_up_line[18];
    dummy+=IP_buffer_ptr_32x32_up_line[19];
    dummy+=IP_buffer_ptr_32x32_up_line[20];
    dummy+=IP_buffer_ptr_32x32_up_line[21];
    dummy+=IP_buffer_ptr_32x32_up_line[22];
    dummy+=IP_buffer_ptr_32x32_up_line[23];
    dummy+=IP_buffer_ptr_32x32_up_line[24];
    dummy+=IP_buffer_ptr_32x32_up_line[25];
    dummy+=IP_buffer_ptr_32x32_up_line[26];
    dummy+=IP_buffer_ptr_32x32_up_line[27];
    dummy+=IP_buffer_ptr_32x32_up_line[28];
    dummy+=IP_buffer_ptr_32x32_up_line[29];
    dummy+=IP_buffer_ptr_32x32_up_line[30];
    dummy+=IP_buffer_ptr_32x32_up_line[31];
    dummy+=IP_buffer_ptr_32x32_up_line[32];
    dummy+=IP_buffer_ptr_32x32_up_line[33];
    dummy+=IP_buffer_ptr_32x32_up_line[34];
    dummy+=IP_buffer_ptr_32x32_up_line[35];
    dummy+=IP_buffer_ptr_32x32_up_line[36];
    dummy+=IP_buffer_ptr_32x32_up_line[37];
    dummy+=IP_buffer_ptr_32x32_up_line[38];
    dummy+=IP_buffer_ptr_32x32_up_line[39];
    dummy+=IP_buffer_ptr_32x32_up_line[40];
    dummy+=IP_buffer_ptr_32x32_up_line[41];
    dummy+=IP_buffer_ptr_32x32_up_line[42];
    dummy+=IP_buffer_ptr_32x32_up_line[43];
    dummy+=IP_buffer_ptr_32x32_up_line[44];
    dummy+=IP_buffer_ptr_32x32_up_line[45];
    dummy+=IP_buffer_ptr_32x32_up_line[46];
    dummy+=IP_buffer_ptr_32x32_up_line[47];
    dummy+=IP_buffer_ptr_32x32_up_line[48];
    dummy+=IP_buffer_ptr_32x32_up_line[49];
    dummy+=IP_buffer_ptr_32x32_up_line[50];
    dummy+=IP_buffer_ptr_32x32_up_line[51];
    dummy+=IP_buffer_ptr_32x32_up_line[52];
    dummy+=IP_buffer_ptr_32x32_up_line[53];
    dummy+=IP_buffer_ptr_32x32_up_line[54];
    dummy+=IP_buffer_ptr_32x32_up_line[55];
    dummy+=IP_buffer_ptr_32x32_up_line[56];
    dummy+=IP_buffer_ptr_32x32_up_line[57];
    dummy+=IP_buffer_ptr_32x32_up_line[58];
    dummy+=IP_buffer_ptr_32x32_up_line[59];
    dummy+=IP_buffer_ptr_32x32_up_line[60];
    dummy+=IP_buffer_ptr_32x32_up_line[61];
    dummy+=IP_buffer_ptr_32x32_up_line[62];
    dummy+=IP_buffer_ptr_32x32_up_line[63];

 // ::pel[64] IP_buffer_ptr_32x32_left_line; 
    dummy+=IP_buffer_ptr_32x32_left_line[0];
    dummy+=IP_buffer_ptr_32x32_left_line[1];
    dummy+=IP_buffer_ptr_32x32_left_line[2];
    dummy+=IP_buffer_ptr_32x32_left_line[3];
    dummy+=IP_buffer_ptr_32x32_left_line[4];
    dummy+=IP_buffer_ptr_32x32_left_line[5];
    dummy+=IP_buffer_ptr_32x32_left_line[6];
    dummy+=IP_buffer_ptr_32x32_left_line[7];
    dummy+=IP_buffer_ptr_32x32_left_line[8];
    dummy+=IP_buffer_ptr_32x32_left_line[9];
    dummy+=IP_buffer_ptr_32x32_left_line[10];
    dummy+=IP_buffer_ptr_32x32_left_line[11];
    dummy+=IP_buffer_ptr_32x32_left_line[12];
    dummy+=IP_buffer_ptr_32x32_left_line[13];
    dummy+=IP_buffer_ptr_32x32_left_line[14];
    dummy+=IP_buffer_ptr_32x32_left_line[15];
    dummy+=IP_buffer_ptr_32x32_left_line[16];
    dummy+=IP_buffer_ptr_32x32_left_line[17];
    dummy+=IP_buffer_ptr_32x32_left_line[18];
    dummy+=IP_buffer_ptr_32x32_left_line[19];
    dummy+=IP_buffer_ptr_32x32_left_line[20];
    dummy+=IP_buffer_ptr_32x32_left_line[21];
    dummy+=IP_buffer_ptr_32x32_left_line[22];
    dummy+=IP_buffer_ptr_32x32_left_line[23];
    dummy+=IP_buffer_ptr_32x32_left_line[24];
    dummy+=IP_buffer_ptr_32x32_left_line[25];
    dummy+=IP_buffer_ptr_32x32_left_line[26];
    dummy+=IP_buffer_ptr_32x32_left_line[27];
    dummy+=IP_buffer_ptr_32x32_left_line[28];
    dummy+=IP_buffer_ptr_32x32_left_line[29];
    dummy+=IP_buffer_ptr_32x32_left_line[30];
    dummy+=IP_buffer_ptr_32x32_left_line[31];
    dummy+=IP_buffer_ptr_32x32_left_line[32];
    dummy+=IP_buffer_ptr_32x32_left_line[33];
    dummy+=IP_buffer_ptr_32x32_left_line[34];
    dummy+=IP_buffer_ptr_32x32_left_line[35];
    dummy+=IP_buffer_ptr_32x32_left_line[36];
    dummy+=IP_buffer_ptr_32x32_left_line[37];
    dummy+=IP_buffer_ptr_32x32_left_line[38];
    dummy+=IP_buffer_ptr_32x32_left_line[39];
    dummy+=IP_buffer_ptr_32x32_left_line[40];
    dummy+=IP_buffer_ptr_32x32_left_line[41];
    dummy+=IP_buffer_ptr_32x32_left_line[42];
    dummy+=IP_buffer_ptr_32x32_left_line[43];
    dummy+=IP_buffer_ptr_32x32_left_line[44];
    dummy+=IP_buffer_ptr_32x32_left_line[45];
    dummy+=IP_buffer_ptr_32x32_left_line[46];
    dummy+=IP_buffer_ptr_32x32_left_line[47];
    dummy+=IP_buffer_ptr_32x32_left_line[48];
    dummy+=IP_buffer_ptr_32x32_left_line[49];
    dummy+=IP_buffer_ptr_32x32_left_line[50];
    dummy+=IP_buffer_ptr_32x32_left_line[51];
    dummy+=IP_buffer_ptr_32x32_left_line[52];
    dummy+=IP_buffer_ptr_32x32_left_line[53];
    dummy+=IP_buffer_ptr_32x32_left_line[54];
    dummy+=IP_buffer_ptr_32x32_left_line[55];
    dummy+=IP_buffer_ptr_32x32_left_line[56];
    dummy+=IP_buffer_ptr_32x32_left_line[57];
    dummy+=IP_buffer_ptr_32x32_left_line[58];
    dummy+=IP_buffer_ptr_32x32_left_line[59];
    dummy+=IP_buffer_ptr_32x32_left_line[60];
    dummy+=IP_buffer_ptr_32x32_left_line[61];
    dummy+=IP_buffer_ptr_32x32_left_line[62];
    dummy+=IP_buffer_ptr_32x32_left_line[63];

 // ::pel IP_buffer_ptr_32x32_top_left; 
    dummy+=IP_buffer_ptr_32x32_top_left;

 // ::U32[8] IP_map_scu_ptr_16x32_map_scu_up_line; 
    dummy+=IP_map_scu_ptr_16x32_map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_16x32_map_scu_up_line[4];
    dummy+=IP_map_scu_ptr_16x32_map_scu_up_line[5];
    dummy+=IP_map_scu_ptr_16x32_map_scu_up_line[6];
    dummy+=IP_map_scu_ptr_16x32_map_scu_up_line[7];

 // ::U32[16] IP_map_scu_ptr_16x32_map_scu_left_line_new; 
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[8];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[9];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[10];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[11];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[12];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[13];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[14];
    dummy+=IP_map_scu_ptr_16x32_map_scu_left_line_new[15];

 // ::U32 IP_map_scu_ptr_16x32_map_scu_top_left_new; 
    dummy+=IP_map_scu_ptr_16x32_map_scu_top_left_new;

 // ::U32[16] IP_map_scu_ptr_32x16_map_scu_up_line; 
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[8];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[9];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[10];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[11];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[12];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[13];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[14];
    dummy+=IP_map_scu_ptr_32x16_map_scu_up_line[15];

 // ::U32[8] IP_map_scu_ptr_32x16_map_scu_left_line_new; 
    dummy+=IP_map_scu_ptr_32x16_map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_32x16_map_scu_left_line_new[4];
    dummy+=IP_map_scu_ptr_32x16_map_scu_left_line_new[5];
    dummy+=IP_map_scu_ptr_32x16_map_scu_left_line_new[6];
    dummy+=IP_map_scu_ptr_32x16_map_scu_left_line_new[7];

 // ::U32 IP_map_scu_ptr_32x16_map_scu_top_left_new; 
    dummy+=IP_map_scu_ptr_32x16_map_scu_top_left_new;

 // ::U32[16] IP_map_scu_ptr_32x32_map_scu_up_line; 
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[0];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[8];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[9];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[10];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[11];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[12];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[13];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[14];
    dummy+=IP_map_scu_ptr_32x32_map_scu_up_line[15];

 // ::U32[16] IP_map_scu_ptr_32x32_map_scu_left_line_new; 
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[0];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[8];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[9];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[10];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[11];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[12];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[13];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[14];
    dummy+=IP_map_scu_ptr_32x32_map_scu_left_line_new[15];

 // ::U32 IP_map_scu_ptr_32x32_map_scu_top_left_new; 
    dummy+=IP_map_scu_ptr_32x32_map_scu_top_left_new;

// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'piRecoY': RAM_1P_BRAM vs RAM_2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'mod_info_best_coef_u': RAM_1P_BRAM vs RAM_T2P_BRAM
// ERROR: port numbers mismatched on partitioned array 'mod_info_best_coef_v': RAM_1P_BRAM vs RAM_T2P_BRAM
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_v core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_v core=RAM_1P_BRAM 
#pragma HLS resource variable=orgU core=RAM_1P_BRAM 
#pragma HLS resource variable=piRecoY core=RAM_1P_BRAM 
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].cur_refi complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].lidx complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=2
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=2
#pragma HLS array_partition variable=IP_buffer_ptr_16x32_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_16x32_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_32x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_32x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_32x32_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_32x32_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_16x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_16x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_16x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_16x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_8x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_U_8x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_16x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_16x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_16x8_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_16x8_up_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_8x16_left_line complete dim=1
#pragma HLS array_partition variable=IP_buffer_ptr_V_8x16_up_line complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_16x32_map_scu_left_line_new complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_16x32_map_scu_up_line complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_32x16_map_scu_left_line_new complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_32x16_map_scu_up_line complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_32x32_map_scu_left_line_new complete dim=1
#pragma HLS array_partition variable=IP_map_scu_ptr_32x32_map_scu_up_line complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].mv complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].mv complete dim=2
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].ref_idx complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].dist_chroma_weight complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].lambda complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].refi complete dim=1
#pragma HLS array_partition variable=mod_info_best_coef_u cyclic factor=16 dim=1
#pragma HLS array_partition variable=mod_info_best_coef_v cyclic factor=16 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_u cyclic factor=16 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_v cyclic factor=16 dim=1
#pragma HLS array_partition variable=orgU cyclic factor=2 dim=1
#pragma HLS array_partition variable=orgU cyclic factor=16 dim=2
#pragma HLS array_partition variable=piRecoY cyclic factor=2 dim=1
#pragma HLS array_partition variable=piRecoY cyclic factor=32 dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=3
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=4
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=2
}
#endif

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
    U32 IP_map_scu_ptr_64x8_map_scu_top_left_new
)
{
    s64 dummy = 0;
    // int all_rdo_num; 
    dummy += all_rdo_num;
    int i = 0;
    int j = 0;
    int k = 0;
    // ::RDOQ_ARRAY* rdoq_array; 
    dummy += rdoq_array[0].rdoq_est_run[0][0];
    dummy += rdoq_array[0].rdoq_est_run[0][1];
    dummy += rdoq_array[0].rdoq_est_run[1][0];
    dummy += rdoq_array[0].rdoq_est_run[1][1];
    dummy += rdoq_array[0].rdoq_est_run[2][0];
    dummy += rdoq_array[0].rdoq_est_run[2][1];
    dummy += rdoq_array[0].rdoq_est_run[3][0];
    dummy += rdoq_array[0].rdoq_est_run[3][1];
    dummy += rdoq_array[0].rdoq_est_run[4][0];
    dummy += rdoq_array[0].rdoq_est_run[4][1];
    dummy += rdoq_array[0].rdoq_est_run[5][0];
    dummy += rdoq_array[0].rdoq_est_run[5][1];
    dummy += rdoq_array[0].rdoq_est_run[6][0];
    dummy += rdoq_array[0].rdoq_est_run[6][1];
    dummy += rdoq_array[0].rdoq_est_run[7][0];
    dummy += rdoq_array[0].rdoq_est_run[7][1];
    dummy += rdoq_array[0].rdoq_est_run[8][0];
    dummy += rdoq_array[0].rdoq_est_run[8][1];
    dummy += rdoq_array[0].rdoq_est_run[9][0];
    dummy += rdoq_array[0].rdoq_est_run[9][1];
    dummy += rdoq_array[0].rdoq_est_run[10][0];
    dummy += rdoq_array[0].rdoq_est_run[10][1];
    dummy += rdoq_array[0].rdoq_est_run[11][0];
    dummy += rdoq_array[0].rdoq_est_run[11][1];
    dummy += rdoq_array[0].rdoq_est_run[12][0];
    dummy += rdoq_array[0].rdoq_est_run[12][1];
    dummy += rdoq_array[0].rdoq_est_run[13][0];
    dummy += rdoq_array[0].rdoq_est_run[13][1];
    dummy += rdoq_array[0].rdoq_est_run[14][0];
    dummy += rdoq_array[0].rdoq_est_run[14][1];
    dummy += rdoq_array[0].rdoq_est_run[15][0];
    dummy += rdoq_array[0].rdoq_est_run[15][1];
    dummy += rdoq_array[0].rdoq_est_run[16][0];
    dummy += rdoq_array[0].rdoq_est_run[16][1];
    dummy += rdoq_array[0].rdoq_est_run[17][0];
    dummy += rdoq_array[0].rdoq_est_run[17][1];
    dummy += rdoq_array[0].rdoq_est_run[18][0];
    dummy += rdoq_array[0].rdoq_est_run[18][1];
    dummy += rdoq_array[0].rdoq_est_run[19][0];
    dummy += rdoq_array[0].rdoq_est_run[19][1];
    dummy += rdoq_array[0].rdoq_est_run[20][0];
    dummy += rdoq_array[0].rdoq_est_run[20][1];
    dummy += rdoq_array[0].rdoq_est_run[21][0];
    dummy += rdoq_array[0].rdoq_est_run[21][1];
    dummy += rdoq_array[0].rdoq_est_run[22][0];
    dummy += rdoq_array[0].rdoq_est_run[22][1];
    dummy += rdoq_array[0].rdoq_est_run[23][0];
    dummy += rdoq_array[0].rdoq_est_run[23][1];
    dummy += rdoq_array[0].rdoq_est_cbf[0][0];
    dummy += rdoq_array[0].rdoq_est_cbf[0][1];
    dummy += rdoq_array[0].rdoq_est_cbf[1][0];
    dummy += rdoq_array[0].rdoq_est_cbf[1][1];
    dummy += rdoq_array[0].rdoq_est_cbf[2][0];
    dummy += rdoq_array[0].rdoq_est_cbf[2][1];
    dummy += rdoq_array[0].rdoq_est_level[0][0];
    dummy += rdoq_array[0].rdoq_est_level[0][1];
    dummy += rdoq_array[0].rdoq_est_level[1][0];
    dummy += rdoq_array[0].rdoq_est_level[1][1];
    dummy += rdoq_array[0].rdoq_est_level[2][0];
    dummy += rdoq_array[0].rdoq_est_level[2][1];
    dummy += rdoq_array[0].rdoq_est_level[3][0];
    dummy += rdoq_array[0].rdoq_est_level[3][1];
    dummy += rdoq_array[0].rdoq_est_level[4][0];
    dummy += rdoq_array[0].rdoq_est_level[4][1];
    dummy += rdoq_array[0].rdoq_est_level[5][0];
    dummy += rdoq_array[0].rdoq_est_level[5][1];
    dummy += rdoq_array[0].rdoq_est_level[6][0];
    dummy += rdoq_array[0].rdoq_est_level[6][1];
    dummy += rdoq_array[0].rdoq_est_level[7][0];
    dummy += rdoq_array[0].rdoq_est_level[7][1];
    dummy += rdoq_array[0].rdoq_est_level[8][0];
    dummy += rdoq_array[0].rdoq_est_level[8][1];
    dummy += rdoq_array[0].rdoq_est_level[9][0];
    dummy += rdoq_array[0].rdoq_est_level[9][1];
    dummy += rdoq_array[0].rdoq_est_level[10][0];
    dummy += rdoq_array[0].rdoq_est_level[10][1];
    dummy += rdoq_array[0].rdoq_est_level[11][0];
    dummy += rdoq_array[0].rdoq_est_level[11][1];
    dummy += rdoq_array[0].rdoq_est_level[12][0];
    dummy += rdoq_array[0].rdoq_est_level[12][1];
    dummy += rdoq_array[0].rdoq_est_level[13][0];
    dummy += rdoq_array[0].rdoq_est_level[13][1];
    dummy += rdoq_array[0].rdoq_est_level[14][0];
    dummy += rdoq_array[0].rdoq_est_level[14][1];
    dummy += rdoq_array[0].rdoq_est_level[15][0];
    dummy += rdoq_array[0].rdoq_est_level[15][1];
    dummy += rdoq_array[0].rdoq_est_level[16][0];
    dummy += rdoq_array[0].rdoq_est_level[16][1];
    dummy += rdoq_array[0].rdoq_est_level[17][0];
    dummy += rdoq_array[0].rdoq_est_level[17][1];
    dummy += rdoq_array[0].rdoq_est_level[18][0];
    dummy += rdoq_array[0].rdoq_est_level[18][1];
    dummy += rdoq_array[0].rdoq_est_level[19][0];
    dummy += rdoq_array[0].rdoq_est_level[19][1];
    dummy += rdoq_array[0].rdoq_est_level[20][0];
    dummy += rdoq_array[0].rdoq_est_level[20][1];
    dummy += rdoq_array[0].rdoq_est_level[21][0];
    dummy += rdoq_array[0].rdoq_est_level[21][1];
    dummy += rdoq_array[0].rdoq_est_level[22][0];
    dummy += rdoq_array[0].rdoq_est_level[22][1];
    dummy += rdoq_array[0].rdoq_est_level[23][0];
    dummy += rdoq_array[0].rdoq_est_level[23][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][0][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][0][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][1][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][1][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][2][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][2][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][3][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][3][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][4][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][4][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][5][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][5][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][6][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][6][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][7][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][7][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][8][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][8][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][9][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][9][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][10][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][10][1];
    dummy += rdoq_array[0].rdoq_est_last[1][0][11][0];
    dummy += rdoq_array[0].rdoq_est_last[1][0][11][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][0][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][0][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][1][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][1][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][2][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][2][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][3][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][3][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][4][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][4][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][5][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][5][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][6][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][6][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][7][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][7][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][8][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][8][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][9][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][9][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][10][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][10][1];
    dummy += rdoq_array[0].rdoq_est_last[1][1][11][0];
    dummy += rdoq_array[0].rdoq_est_last[1][1][11][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][0][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][0][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][1][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][1][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][2][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][2][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][3][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][3][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][4][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][4][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][5][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][5][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][6][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][6][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][7][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][7][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][8][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][8][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][9][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][9][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][10][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][10][1];
    dummy += rdoq_array[0].rdoq_est_last[1][2][11][0];
    dummy += rdoq_array[0].rdoq_est_last[1][2][11][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][0][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][0][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][1][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][1][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][2][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][2][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][3][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][3][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][4][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][4][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][5][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][5][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][6][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][6][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][7][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][7][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][8][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][8][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][9][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][9][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][10][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][10][1];
    dummy += rdoq_array[0].rdoq_est_last[1][3][11][0];
    dummy += rdoq_array[0].rdoq_est_last[1][3][11][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][0][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][0][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][1][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][1][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][2][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][2][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][3][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][3][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][4][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][4][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][5][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][5][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][6][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][6][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][7][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][7][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][8][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][8][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][9][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][9][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][10][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][10][1];
    dummy += rdoq_array[0].rdoq_est_last[1][4][11][0];
    dummy += rdoq_array[0].rdoq_est_last[1][4][11][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][0][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][0][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][1][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][1][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][2][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][2][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][3][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][3][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][4][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][4][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][5][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][5][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][6][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][6][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][7][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][7][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][8][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][8][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][9][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][9][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][10][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][10][1];
    dummy += rdoq_array[0].rdoq_est_last[1][5][11][0];
    dummy += rdoq_array[0].rdoq_est_last[1][5][11][1];

    // ::MD_INPUT* md_input_ptr; 
       // md_input_ptr[0] . pix_x is ignored ;
       // md_input_ptr[0] . pix_y is ignored ;
       // md_input_ptr[0] . lambda_mv is ignored ;
    dummy += md_input_ptr[0].lambda[0];
    dummy += md_input_ptr[0].lambda[1];
    // md_input_ptr[0] . lambda_y is ignored ;
    // md_input_ptr[0] . lambda_u is ignored ;
    // md_input_ptr[0] . lambda_v is ignored ;
    // md_input_ptr[0] . qp_y is ignored ;
    dummy += md_input_ptr[0].qp_u;
    // md_input_ptr[0] . qp_v is ignored ;
    dummy += md_input_ptr[0].dist_chroma_weight[0];
    dummy += md_input_ptr[0].dist_chroma_weight[1];
    // md_input_ptr[0] . x_last_ctu is ignored ;
    // md_input_ptr[0] . y_last_ctu is ignored ;
    // md_input_ptr[0] . CtrPos2MD[0][0][0] is ignored ;
    // md_input_ptr[0] . CtrPos2MD_ref1[0][0][0] is ignored ;
    // md_input_ptr[0] . split_mode[0][0][0] is ignored ;
    // md_input_ptr[0] . do_chroma is ignored ;
    // md_input_ptr[0] . pred_mode[0] is ignored ;
    // md_input_ptr[0] . ipm[0][0] is ignored ;
    // md_input_ptr[0] . affine_flag[0] is ignored ;
    // md_input_ptr[0] . mv[0][0][0] is ignored ;
    // md_input_ptr[0] . refi[0][0] is ignored ;
    // md_input_ptr[0] . num_nz_coef[0][0] is ignored ;
    // md_input_ptr[0] . map_scu[0] is ignored ;
    // md_input_ptr[0] . map_cu_mode[0] is ignored ;
    // md_input_ptr[0] . reco_y[0][0] is ignored ;
    // md_input_ptr[0] . umve_flag[0] is ignored ;
    // md_input_ptr[0] . skip_idx[0] is ignored ;
    // md_input_ptr[0] . inter_filter_flag[0] is ignored ;
    // md_input_ptr[0] . cu_dqp_enable is ignored ;
    // md_input_ptr[0] . cu_qp_group_area_size is ignored ;
    // md_input_ptr[0] . cu_qp_group_pred_qp is ignored ;
    // md_input_ptr[0] . cu_luma_qp[0] is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_input_ptr[0] . chroma_quant_param_delta_cr is ignored ;
    // md_input_ptr[0] . md_slice_qp is ignored ;
    // md_input_ptr[0] . CuQPMap[0] is ignored ;

 // ::U13 pb_x; 
    dummy += pb_x;

    // ::U13 pb_y; 
    dummy += pb_y;

    // ::MD_FW* md_fw_ptr; 
    dummy += md_fw_ptr[0].pic_width;
    dummy += md_fw_ptr[0].pic_height;
    // md_fw_ptr[0] . slice_type is ignored ;
    // md_fw_ptr[0] . slice_qp is ignored ;
    // md_fw_ptr[0] . max_dt_size is ignored ;
    // md_fw_ptr[0] . affine_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_enable_flag is ignored ;
    // md_fw_ptr[0] . dt_intra_enable_flag is ignored ;
    // md_fw_ptr[0] . num_of_hmvp_cand is ignored ;
    // md_fw_ptr[0] . ipf_enable_flag is ignored ;
    dummy += md_fw_ptr[0].pic_width_in_scu;
    dummy += md_fw_ptr[0].pic_height_in_scu;
    // md_fw_ptr[0] . log2_max_cuwh is ignored ;
    // md_fw_ptr[0] . log2_culine is ignored ;
    // md_fw_ptr[0] . max_cuwh is ignored ;
    // md_fw_ptr[0] . f_scu is ignored ;
    // md_fw_ptr[0] . gop_size is ignored ;
    dummy += md_fw_ptr[0].bit_depth_internal;
    // md_fw_ptr[0] . max_part_ratio is ignored ;
    // md_fw_ptr[0] . min_cu_size is ignored ;
    // md_fw_ptr[0] . max_split_times is ignored ;
    // md_fw_ptr[0] . min_qt_size is ignored ;
    // md_fw_ptr[0] . max_bt_size is ignored ;
    // md_fw_ptr[0] . max_eqt_size is ignored ;
    // md_fw_ptr[0] . affine_subblock_size_idx is ignored ;
    // md_fw_ptr[0] . ipcm_enable_flag is ignored ;
    // md_fw_ptr[0] . position_based_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . secondary_transform_enable_flag is ignored ;
    // md_fw_ptr[0] . tscpm_enable_flag is ignored ;
    // md_fw_ptr[0] . amvr_enable_flag is ignored ;
    // md_fw_ptr[0] . emvr_enable_flag is ignored ;
    // md_fw_ptr[0] . eipm_enable_flag is ignored ;
    dummy += md_fw_ptr[0].mipf_enable_flag;
    // md_fw_ptr[0] . interpf_enable_flag is ignored ;
    // md_fw_ptr[0] . umve_set_flag is ignored ;
    // md_fw_ptr[0] . refp_ptr[0][0] is ignored ;
    // md_fw_ptr[0] . num_refp[0] is ignored ;
    // md_fw_ptr[0] . cur_ptr is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cb is ignored ;
    // md_fw_ptr[0] . chroma_quant_param_delta_cr is ignored ;

 // ::pel[12288] RefWindowLCU_UP; 
    // RefWindowLCU_UP[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP; 
    // RefWindowLCU_VP[0] is ignored ;

 // ::S14[2][2][2] CtrPos; 
    // CtrPos[0][0][0] is ignored ;

 // ::pel[12288] RefWindowLCU_UP_ref1; 
    // RefWindowLCU_UP_ref1[0] is ignored ;

 // ::pel[12288] RefWindowLCU_VP_ref1; 
    // RefWindowLCU_VP_ref1[0] is ignored ;

 // ::S14[2][2][2] CtrPos_ref1; 
    // CtrPos_ref1[0][0][0] is ignored ;

 // ::U2 tree_status; 
    // tree_status is ignored ;

 // ::MD_COM_MODE_BEST* mod_info_best; 
    dummy += mod_info_best[0].x_scu;
    dummy += mod_info_best[0].y_scu;
    // mod_info_best[0] . cud is ignored ;
    dummy += mod_info_best[0].cu_width;
    dummy += mod_info_best[0].cu_height;
    // mod_info_best[0] . cu_width_log2 is ignored ;
    // mod_info_best[0] . cu_height_log2 is ignored ;
    dummy += mod_info_best[0].x_pos;
    dummy += mod_info_best[0].y_pos;
    // mod_info_best[0] . scup is ignored ;
    dummy += mod_info_best[0].cu_mode;
    // mod_info_best[0] . pb_part is ignored ;
    // mod_info_best[0] . tb_part is ignored ;
    // mod_info_best[0] . rec_y[0] is ignored ;
    mod_info_best[0].rec_u[0] = dummy;
    mod_info_best[0].rec_u[1] = dummy;
    mod_info_best[0].rec_u[2] = dummy;
    mod_info_best[0].rec_u[3] = dummy;
    mod_info_best[0].rec_u[4] = dummy;
    mod_info_best[0].rec_u[5] = dummy;
    mod_info_best[0].rec_u[6] = dummy;
    mod_info_best[0].rec_u[7] = dummy;
    mod_info_best[0].rec_u[8] = dummy;
    mod_info_best[0].rec_u[9] = dummy;
    mod_info_best[0].rec_u[10] = dummy;
    mod_info_best[0].rec_u[11] = dummy;
    mod_info_best[0].rec_u[12] = dummy;
    mod_info_best[0].rec_u[13] = dummy;
    mod_info_best[0].rec_u[14] = dummy;
    mod_info_best[0].rec_u[15] = dummy;
    mod_info_best[0].rec_u[16] = dummy;
    mod_info_best[0].rec_u[17] = dummy;
    mod_info_best[0].rec_u[18] = dummy;
    mod_info_best[0].rec_u[19] = dummy;
    mod_info_best[0].rec_u[20] = dummy;
    mod_info_best[0].rec_u[21] = dummy;
    mod_info_best[0].rec_u[22] = dummy;
    mod_info_best[0].rec_u[23] = dummy;
    mod_info_best[0].rec_u[24] = dummy;
    mod_info_best[0].rec_u[25] = dummy;
    mod_info_best[0].rec_u[26] = dummy;
    mod_info_best[0].rec_u[27] = dummy;
    mod_info_best[0].rec_u[28] = dummy;
    mod_info_best[0].rec_u[29] = dummy;
    mod_info_best[0].rec_u[30] = dummy;
    mod_info_best[0].rec_u[31] = dummy;
    mod_info_best[0].rec_v[0] = dummy;
    mod_info_best[0].rec_v[1] = dummy;
    mod_info_best[0].rec_v[2] = dummy;
    mod_info_best[0].rec_v[3] = dummy;
    mod_info_best[0].rec_v[4] = dummy;
    mod_info_best[0].rec_v[5] = dummy;
    mod_info_best[0].rec_v[6] = dummy;
    mod_info_best[0].rec_v[7] = dummy;
    mod_info_best[0].rec_v[8] = dummy;
    mod_info_best[0].rec_v[9] = dummy;
    mod_info_best[0].rec_v[10] = dummy;
    mod_info_best[0].rec_v[11] = dummy;
    mod_info_best[0].rec_v[12] = dummy;
    mod_info_best[0].rec_v[13] = dummy;
    mod_info_best[0].rec_v[14] = dummy;
    mod_info_best[0].rec_v[15] = dummy;
    mod_info_best[0].rec_v[16] = dummy;
    mod_info_best[0].rec_v[17] = dummy;
    mod_info_best[0].rec_v[18] = dummy;
    mod_info_best[0].rec_v[19] = dummy;
    mod_info_best[0].rec_v[20] = dummy;
    mod_info_best[0].rec_v[21] = dummy;
    mod_info_best[0].rec_v[22] = dummy;
    mod_info_best[0].rec_v[23] = dummy;
    mod_info_best[0].rec_v[24] = dummy;
    mod_info_best[0].rec_v[25] = dummy;
    mod_info_best[0].rec_v[26] = dummy;
    mod_info_best[0].rec_v[27] = dummy;
    mod_info_best[0].rec_v[28] = dummy;
    mod_info_best[0].rec_v[29] = dummy;
    mod_info_best[0].rec_v[30] = dummy;
    mod_info_best[0].rec_v[31] = dummy;
    // mod_info_best[0] . coef_y[0] is ignored ;
    (mod_info_best[0].coef_u[0]) = (mod_info_best[0].coef_u[1]/*TODO:*/) = dummy;
    (mod_info_best[0].coef_u[1]) = (mod_info_best[0].coef_u[1]) = dummy;
    (mod_info_best[0].coef_u[2]) = (mod_info_best[0].coef_u[2]) = dummy;
    (mod_info_best[0].coef_u[3]) = (mod_info_best[0].coef_u[3]) = dummy;
    (mod_info_best[0].coef_u[4]) = (mod_info_best[0].coef_u[4]) = dummy;
    (mod_info_best[0].coef_u[5]) = (mod_info_best[0].coef_u[5]) = dummy;
    (mod_info_best[0].coef_u[6]) = (mod_info_best[0].coef_u[6]) = dummy;
    (mod_info_best[0].coef_u[7]) = (mod_info_best[0].coef_u[7]) = dummy;
    (mod_info_best[0].coef_u[8]) = (mod_info_best[0].coef_u[8]) = dummy;
    (mod_info_best[0].coef_u[9]) = (mod_info_best[0].coef_u[9]) = dummy;
    (mod_info_best[0].coef_u[10]) = (mod_info_best[0].coef_u[10]) = dummy;
    (mod_info_best[0].coef_u[11]) = (mod_info_best[0].coef_u[11]) = dummy;
    (mod_info_best[0].coef_u[12]) = (mod_info_best[0].coef_u[12]) = dummy;
    (mod_info_best[0].coef_u[13]) = (mod_info_best[0].coef_u[13]) = dummy;
    (mod_info_best[0].coef_u[14]) = (mod_info_best[0].coef_u[14]) = dummy;
    (mod_info_best[0].coef_u[15]) = (mod_info_best[0].coef_u[15]) = dummy;
    (mod_info_best[0].coef_u[16]) = (mod_info_best[0].coef_u[16]) = dummy;
    (mod_info_best[0].coef_u[17]) = (mod_info_best[0].coef_u[17]) = dummy;
    (mod_info_best[0].coef_u[18]) = (mod_info_best[0].coef_u[18]) = dummy;
    (mod_info_best[0].coef_u[19]) = (mod_info_best[0].coef_u[19]) = dummy;
    (mod_info_best[0].coef_u[20]) = (mod_info_best[0].coef_u[20]) = dummy;
    (mod_info_best[0].coef_u[21]) = (mod_info_best[0].coef_u[21]) = dummy;
    (mod_info_best[0].coef_u[22]) = (mod_info_best[0].coef_u[22]) = dummy;
    (mod_info_best[0].coef_u[23]) = (mod_info_best[0].coef_u[23]) = dummy;
    (mod_info_best[0].coef_u[24]) = (mod_info_best[0].coef_u[24]) = dummy;
    (mod_info_best[0].coef_u[25]) = (mod_info_best[0].coef_u[25]) = dummy;
    (mod_info_best[0].coef_u[26]) = (mod_info_best[0].coef_u[26]) = dummy;
    (mod_info_best[0].coef_u[27]) = (mod_info_best[0].coef_u[27]) = dummy;
    (mod_info_best[0].coef_u[28]) = (mod_info_best[0].coef_u[28]) = dummy;
    (mod_info_best[0].coef_u[29]) = (mod_info_best[0].coef_u[29]) = dummy;
    (mod_info_best[0].coef_u[30]) = (mod_info_best[0].coef_u[30]) = dummy;
    (mod_info_best[0].coef_u[31]) = (mod_info_best[0].coef_u[31]) = dummy;
    (mod_info_best[0].coef_v[0]) = (mod_info_best[0].coef_v[1]/*TODO:*/) = dummy;
    (mod_info_best[0].coef_v[1]) = (mod_info_best[0].coef_v[1]) = dummy;
    (mod_info_best[0].coef_v[2]) = (mod_info_best[0].coef_v[2]) = dummy;
    (mod_info_best[0].coef_v[3]) = (mod_info_best[0].coef_v[3]) = dummy;
    (mod_info_best[0].coef_v[4]) = (mod_info_best[0].coef_v[4]) = dummy;
    (mod_info_best[0].coef_v[5]) = (mod_info_best[0].coef_v[5]) = dummy;
    (mod_info_best[0].coef_v[6]) = (mod_info_best[0].coef_v[6]) = dummy;
    (mod_info_best[0].coef_v[7]) = (mod_info_best[0].coef_v[7]) = dummy;
    (mod_info_best[0].coef_v[8]) = (mod_info_best[0].coef_v[8]) = dummy;
    (mod_info_best[0].coef_v[9]) = (mod_info_best[0].coef_v[9]) = dummy;
    (mod_info_best[0].coef_v[10]) = (mod_info_best[0].coef_v[10]) = dummy;
    (mod_info_best[0].coef_v[11]) = (mod_info_best[0].coef_v[11]) = dummy;
    (mod_info_best[0].coef_v[12]) = (mod_info_best[0].coef_v[12]) = dummy;
    (mod_info_best[0].coef_v[13]) = (mod_info_best[0].coef_v[13]) = dummy;
    (mod_info_best[0].coef_v[14]) = (mod_info_best[0].coef_v[14]) = dummy;
    (mod_info_best[0].coef_v[15]) = (mod_info_best[0].coef_v[15]) = dummy;
    (mod_info_best[0].coef_v[16]) = (mod_info_best[0].coef_v[16]) = dummy;
    (mod_info_best[0].coef_v[17]) = (mod_info_best[0].coef_v[17]) = dummy;
    (mod_info_best[0].coef_v[18]) = (mod_info_best[0].coef_v[18]) = dummy;
    (mod_info_best[0].coef_v[19]) = (mod_info_best[0].coef_v[19]) = dummy;
    (mod_info_best[0].coef_v[20]) = (mod_info_best[0].coef_v[20]) = dummy;
    (mod_info_best[0].coef_v[21]) = (mod_info_best[0].coef_v[21]) = dummy;
    (mod_info_best[0].coef_v[22]) = (mod_info_best[0].coef_v[22]) = dummy;
    (mod_info_best[0].coef_v[23]) = (mod_info_best[0].coef_v[23]) = dummy;
    (mod_info_best[0].coef_v[24]) = (mod_info_best[0].coef_v[24]) = dummy;
    (mod_info_best[0].coef_v[25]) = (mod_info_best[0].coef_v[25]) = dummy;
    (mod_info_best[0].coef_v[26]) = (mod_info_best[0].coef_v[26]) = dummy;
    (mod_info_best[0].coef_v[27]) = (mod_info_best[0].coef_v[27]) = dummy;
    (mod_info_best[0].coef_v[28]) = (mod_info_best[0].coef_v[28]) = dummy;
    (mod_info_best[0].coef_v[29]) = (mod_info_best[0].coef_v[29]) = dummy;
    (mod_info_best[0].coef_v[30]) = (mod_info_best[0].coef_v[30]) = dummy;
    (mod_info_best[0].coef_v[31]) = (mod_info_best[0].coef_v[31]) = dummy;
    // mod_info_best[0] . pred_y[0][0] is ignored ;
    // mod_info_best[0] . pred_u[0] is ignored ;
    // mod_info_best[0] . pred_v[0] is ignored ;
    (mod_info_best[0].num_nz[0][0]) = (mod_info_best[0].num_nz[0][1]/*TODO:*/) = (mod_info_best[0].num_nz[0][0]) + (mod_info_best[0].num_nz[0][1]/*TODO:*/);
    dummy += mod_info_best[0].refi[0];
    dummy += mod_info_best[0].refi[1];
    // mod_info_best[0] . mvr_idx is ignored ;
    // mod_info_best[0] . umve_flag is ignored ;
    // mod_info_best[0] . umve_idx is ignored ;
    dummy += mod_info_best[0].skip_idx;
    // mod_info_best[0] . mvp_from_hmvp_flag is ignored ;
    // mod_info_best[0] . inter_filter_flag is ignored ;
    // mod_info_best[0] . mvd[0][0] is ignored ;
    dummy += mod_info_best[0].mv[0][0];
    dummy += mod_info_best[0].mv[0][1];
    dummy += mod_info_best[0].mv[1][0];
    dummy += mod_info_best[0].mv[1][1];
    // mod_info_best[0] . affine_flag is ignored ;
    // mod_info_best[0] . affine_mv[0][0][0] is ignored ;
    // mod_info_best[0] . affine_mvd[0][0][0] is ignored ;
    // mod_info_best[0] . smvd_flag is ignored ;
    // mod_info_best[0] . mpm[0][0] is ignored ;
    dummy += mod_info_best[0].ipm[0][0];
    // mod_info_best[0] . ipf_flag is ignored ;
    // mod_info_best[0] . chroma_motion is ignored ;
    // mod_info_best[0] . x_luma is ignored ;
    // mod_info_best[0] . y_luma is ignored ;
    // mod_info_best[0] . x_chroma is ignored ;
    // mod_info_best[0] . y_chroma is ignored ;
    // mod_info_best[0] . last_child is ignored ;

 // ::COM_PART_INFO* mod_info_curr_pb_info; 
    // mod_info_curr_pb_info[0] . num_sub_part is ignored ;
    // mod_info_curr_pb_info[0] . sub_x[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_y[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_w[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_h[0] is ignored ;
    // mod_info_curr_pb_info[0] . sub_scup[0] is ignored ;

 // ::pel[2][32][32] orgU; 
   /* dummy += orgU[0];
    dummy += orgU[1];
    dummy += orgU[2];
    dummy += orgU[3];
    dummy += orgU[4];
    dummy += orgU[5];
    dummy += orgU[6];
    dummy += orgU[7];
    dummy += orgU[8];
    dummy += orgU[9];
    dummy += orgU[10];
    dummy += orgU[11];
    dummy += orgU[12];
    dummy += orgU[13];
    dummy += orgU[14];
    dummy += orgU[15];
    dummy += orgU[16];
    dummy += orgU[17];
    dummy += orgU[18];
    dummy += orgU[19];
    dummy += orgU[20];
    dummy += orgU[21];
    dummy += orgU[22];
    dummy += orgU[23];
    dummy += orgU[24];
    dummy += orgU[25];
    dummy += orgU[26];
    dummy += orgU[27];
    dummy += orgU[28];
    dummy += orgU[29];
    dummy += orgU[30];
    dummy += orgU[31];*/
    
    for (i = 0; i < 2;i = i + 1) {
        for (j = 0; j < 2; j = j + 1) {
            for (k = 0; k < 2; k = k + 1) {
                dummy += orgU[i][j][k];
            }
        }

    }
    // int cu_width_log2_chroma; 
    dummy += cu_width_log2_chroma;

    // int cu_width_chroma; 
    dummy += cu_width_chroma;

    // int cu_height_log2_chroma; 
    dummy += cu_height_log2_chroma;

    // int cu_height_chroma; 
    dummy += cu_height_chroma;

    // int bit_depth; 
    dummy += bit_depth;

    // ::U2 cu_mode; 
       // cu_mode is ignored ;

    // ::U2 slice_type; 
       // slice_type is ignored ;

    // ::U14[2] scale; 
       // scale[0] is ignored ;

    // ::U6[2] qpUV; 
       // qpUV[0] is ignored ;

    // int tscpm_enable_flag; 
    dummy += tscpm_enable_flag;

    // ::s64 lambda_y; 
       // lambda_y is ignored ;

    // ::S12 size_tmp; 
       // size_tmp is ignored ;

    // ::strFetch_ref_window* Fetch_Ref_window_ptr; 
       // Fetch_Ref_window_ptr[0] . RefWindowLCU[0] is ignored ;
       // Fetch_Ref_window_ptr[0] . swOutLv0[0][0] is ignored ;
       // Fetch_Ref_window_ptr[0] . CtrPos[0][0][0] is ignored ;
    dummy += Fetch_Ref_window_ptr[0].RefWindowLCU_UP[0];
    dummy += Fetch_Ref_window_ptr[0].RefWindowLCU_VP[0];
    dummy += Fetch_Ref_window_ptr[0].code_stru;
    dummy += Fetch_Ref_window_ptr[0].lidx[0];
    dummy += Fetch_Ref_window_ptr[0].lidx[1];
    dummy += Fetch_Ref_window_ptr[0].cur_refi[0];
    dummy += Fetch_Ref_window_ptr[0].cur_refi[1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[0][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[0][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[1][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[1][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[2][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[2][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[3][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[3][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[4][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[4][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[5][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[5][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[6][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[6][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[7][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[7][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[8][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[8][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[9][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[9][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[10][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[10][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[11][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[11][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[12][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[12][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[13][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[13][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[14][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[14][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[15][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[15][1];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[16][0];
    dummy += Fetch_Ref_window_ptr[0].refp_pic_ptr[16][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[0][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[0][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[1][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[1][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[2][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[2][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[3][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[3][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[4][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[4][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[5][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[5][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[6][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[6][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[7][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[7][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[8][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[8][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[9][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[9][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[10][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[10][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[11][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[11][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[12][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[12][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[13][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[13][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[14][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[14][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[15][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[15][1];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[16][0];
    dummy += Fetch_Ref_window_ptr[0].refp_is_library_picture[16][1];
    // Fetch_Ref_window_ptr[0] . CtrPos_ref1[0][0][0] is ignored ;
    // Fetch_Ref_window_ptr[0] . RefWindowLCU_ref1[0] is ignored ;
    // Fetch_Ref_window_ptr[0] . swOutLv0_ref1[0][0] is ignored ;
    dummy += Fetch_Ref_window_ptr[0].RefWindowLCU_UP_ref1[0];
    dummy += (Fetch_Ref_window_ptr[0].RefWindowLCU_VP_ref1[0]) + (Fetch_Ref_window_ptr[0].RefWindowLCU_VP_ref1[1]/*TODO:*/);

    // ::pel[64][64] piRecoY; 
    dummy += piRecoY[0][0];
    dummy += piRecoY[0][1];
    dummy += piRecoY[0][2];
    dummy += (piRecoY[0][3]) + (piRecoY[0][3]);
    dummy += piRecoY[0][4];
    dummy += (piRecoY[0][5]) + (piRecoY[0][5]);
    dummy += piRecoY[0][6];
    dummy += (piRecoY[0][7]) + (piRecoY[0][7]);
    dummy += piRecoY[0][8];
    dummy += (piRecoY[0][9]) + (piRecoY[0][9]);
    dummy += piRecoY[0][10];
    dummy += (piRecoY[0][11]) + (piRecoY[0][11]);
    dummy += piRecoY[0][12];
    dummy += (piRecoY[0][13]) + (piRecoY[0][13]);
    dummy += piRecoY[0][14];
    dummy += (piRecoY[0][15]) + (piRecoY[0][15]);
    dummy += piRecoY[0][16];
    dummy += (piRecoY[0][17]) + (piRecoY[0][17]);
    dummy += piRecoY[0][18];
    dummy += (piRecoY[0][19]) + (piRecoY[0][19]);
    dummy += piRecoY[0][20];
    dummy += (piRecoY[0][21]) + (piRecoY[0][21]);
    dummy += piRecoY[0][22];
    dummy += (piRecoY[0][23]) + (piRecoY[0][23]);
    dummy += piRecoY[0][24];
    dummy += (piRecoY[0][25]) + (piRecoY[0][25]);
    dummy += piRecoY[0][26];
    dummy += (piRecoY[0][27]) + (piRecoY[0][27]);
    dummy += piRecoY[0][28];
    dummy += (piRecoY[0][29]) + (piRecoY[0][29]);
    dummy += piRecoY[0][30];
    dummy += (piRecoY[0][31]) + (piRecoY[0][31]);
    dummy += piRecoY[0][32];
    dummy += (piRecoY[0][33]) + (piRecoY[0][33]);
    dummy += piRecoY[0][34];
    dummy += (piRecoY[0][35]) + (piRecoY[0][35]);
    dummy += piRecoY[0][36];
    dummy += (piRecoY[0][37]) + (piRecoY[0][37]);
    dummy += piRecoY[0][38];
    dummy += (piRecoY[0][39]) + (piRecoY[0][39]);
    dummy += piRecoY[0][40];
    dummy += (piRecoY[0][41]) + (piRecoY[0][41]);
    dummy += piRecoY[0][42];
    dummy += (piRecoY[0][43]) + (piRecoY[0][43]);
    dummy += piRecoY[0][44];
    dummy += (piRecoY[0][45]) + (piRecoY[0][45]);
    dummy += piRecoY[0][46];
    dummy += (piRecoY[0][47]) + (piRecoY[0][47]);
    dummy += piRecoY[0][48];
    dummy += (piRecoY[0][49]) + (piRecoY[0][49]);
    dummy += piRecoY[0][50];
    dummy += (piRecoY[0][51]) + (piRecoY[0][51]);
    dummy += piRecoY[0][52];
    dummy += (piRecoY[0][53]) + (piRecoY[0][53]);
    dummy += piRecoY[0][54];
    dummy += (piRecoY[0][55]) + (piRecoY[0][55]);
    dummy += piRecoY[0][56];
    dummy += (piRecoY[0][57]) + (piRecoY[0][57]);
    dummy += piRecoY[0][58];
    dummy += (piRecoY[0][59]) + (piRecoY[0][59]);
    dummy += piRecoY[0][60];
    dummy += (piRecoY[0][61]) + (piRecoY[0][61]);
    dummy += piRecoY[0][62];
    dummy += piRecoY[0][63];
    dummy += piRecoY[1][0];
    dummy += piRecoY[1][1];
    dummy += piRecoY[1][2];
    dummy += (piRecoY[1][3]) + (piRecoY[1][3]);
    dummy += piRecoY[1][4];
    dummy += (piRecoY[1][5]) + (piRecoY[1][5]);
    dummy += piRecoY[1][6];
    dummy += (piRecoY[1][7]) + (piRecoY[1][7]);
    dummy += piRecoY[1][8];
    dummy += (piRecoY[1][9]) + (piRecoY[1][9]);
    dummy += piRecoY[1][10];
    dummy += (piRecoY[1][11]) + (piRecoY[1][11]);
    dummy += piRecoY[1][12];
    dummy += (piRecoY[1][13]) + (piRecoY[1][13]);
    dummy += piRecoY[1][14];
    dummy += (piRecoY[1][15]) + (piRecoY[1][15]);
    dummy += piRecoY[1][16];
    dummy += (piRecoY[1][17]) + (piRecoY[1][17]);
    dummy += piRecoY[1][18];
    dummy += (piRecoY[1][19]) + (piRecoY[1][19]);
    dummy += piRecoY[1][20];
    dummy += (piRecoY[1][21]) + (piRecoY[1][21]);
    dummy += piRecoY[1][22];
    dummy += (piRecoY[1][23]) + (piRecoY[1][23]);
    dummy += piRecoY[1][24];
    dummy += (piRecoY[1][25]) + (piRecoY[1][25]);
    dummy += piRecoY[1][26];
    dummy += (piRecoY[1][27]) + (piRecoY[1][27]);
    dummy += piRecoY[1][28];
    dummy += (piRecoY[1][29]) + (piRecoY[1][29]);
    dummy += piRecoY[1][30];
    dummy += (piRecoY[1][31]) + (piRecoY[1][31]);
    dummy += piRecoY[1][32];
    dummy += (piRecoY[1][33]) + (piRecoY[1][33]);
    dummy += piRecoY[1][34];
    dummy += (piRecoY[1][35]) + (piRecoY[1][35]);
    dummy += piRecoY[1][36];
    dummy += (piRecoY[1][37]) + (piRecoY[1][37]);
    dummy += piRecoY[1][38];
    dummy += (piRecoY[1][39]) + (piRecoY[1][39]);
    dummy += piRecoY[1][40];
    dummy += (piRecoY[1][41]) + (piRecoY[1][41]);
    dummy += piRecoY[1][42];
    dummy += (piRecoY[1][43]) + (piRecoY[1][43]);
    dummy += piRecoY[1][44];
    dummy += (piRecoY[1][45]) + (piRecoY[1][45]);
    dummy += piRecoY[1][46];
    dummy += (piRecoY[1][47]) + (piRecoY[1][47]);
    dummy += piRecoY[1][48];
    dummy += (piRecoY[1][49]) + (piRecoY[1][49]);
    dummy += piRecoY[1][50];
    dummy += (piRecoY[1][51]) + (piRecoY[1][51]);
    dummy += piRecoY[1][52];
    dummy += (piRecoY[1][53]) + (piRecoY[1][53]);
    dummy += piRecoY[1][54];
    dummy += (piRecoY[1][55]) + (piRecoY[1][55]);
    dummy += piRecoY[1][56];
    dummy += (piRecoY[1][57]) + (piRecoY[1][57]);
    dummy += piRecoY[1][58];
    dummy += (piRecoY[1][59]) + (piRecoY[1][59]);
    dummy += piRecoY[1][60];
    dummy += (piRecoY[1][61]) + (piRecoY[1][61]);
    dummy += piRecoY[1][62];
    dummy += piRecoY[1][63];

    // ::CORE_SBTMVP* core_sbtmvp; 
    dummy += core_sbtmvp[0].sbTmvp_flag;
    // core_sbtmvp[0] . best_sbTmvp_flag is ignored ;
    dummy += core_sbtmvp[0].sbTmvp[0].mv[0][0];
    dummy += core_sbtmvp[0].sbTmvp[0].mv[0][1];
    dummy += core_sbtmvp[0].sbTmvp[1].mv[0][0];
    dummy += core_sbtmvp[0].sbTmvp[1].mv[0][1];
    dummy += core_sbtmvp[0].sbTmvp[2].mv[0][0];
    dummy += core_sbtmvp[0].sbTmvp[2].mv[0][1];
    dummy += core_sbtmvp[0].sbTmvp[3].mv[0][0];
    dummy += core_sbtmvp[0].sbTmvp[3].mv[0][1];
    dummy += core_sbtmvp[0].sbTmvp[0].ref_idx[0];
    dummy += core_sbtmvp[0].sbTmvp[1].ref_idx[0];
    dummy += core_sbtmvp[0].sbTmvp[2].ref_idx[0];
    dummy += core_sbtmvp[0].sbTmvp[3].ref_idx[0];
    // core_sbtmvp[0] . best_sbTmvp[0] . mv[0][0] is ignored ;
    // core_sbtmvp[0] . best_sbTmvp[0] . ref_idx[0] is ignored ;
    // core_sbtmvp[0] . isValid is ignored ;

 // ::pel[32][32] mod_info_best_rec_u; 
    /*mod_info_best_rec_u[0] = dummy;
    mod_info_best_rec_u[1] = dummy;
    mod_info_best_rec_u[2] = dummy;
    mod_info_best_rec_u[3] = dummy;
    mod_info_best_rec_u[4] = dummy;
    mod_info_best_rec_u[5] = dummy;
    mod_info_best_rec_u[6] = dummy;
    mod_info_best_rec_u[7] = dummy;
    mod_info_best_rec_u[8] = dummy;
    mod_info_best_rec_u[9] = dummy;
    mod_info_best_rec_u[10] = dummy;
    mod_info_best_rec_u[11] = dummy;
    mod_info_best_rec_u[12] = dummy;
    mod_info_best_rec_u[13] = dummy;
    mod_info_best_rec_u[14] = dummy;
    mod_info_best_rec_u[15] = dummy;
    mod_info_best_rec_u[16] = dummy;
    mod_info_best_rec_u[17] = dummy;
    mod_info_best_rec_u[18] = dummy;
    mod_info_best_rec_u[19] = dummy;
    mod_info_best_rec_u[20] = dummy;
    mod_info_best_rec_u[21] = dummy;
    mod_info_best_rec_u[22] = dummy;
    mod_info_best_rec_u[23] = dummy;
    mod_info_best_rec_u[24] = dummy;
    mod_info_best_rec_u[25] = dummy;
    mod_info_best_rec_u[26] = dummy;
    mod_info_best_rec_u[27] = dummy;
    mod_info_best_rec_u[28] = dummy;
    mod_info_best_rec_u[29] = dummy;
    mod_info_best_rec_u[30] = dummy;
    mod_info_best_rec_u[31] = dummy;*/

    
    for (i = 0; i < 32; i = i + 1) {
        for (j = 0; j < 32; j = j + 1) {
            mod_info_best_rec_u[i][j] = dummy;
        }
    }
    // ::pel[32][32] mod_info_best_rec_v; 
    /*mod_info_best_rec_v[0] = dummy;
    mod_info_best_rec_v[1] = dummy;
    mod_info_best_rec_v[2] = dummy;
    mod_info_best_rec_v[3] = dummy;
    mod_info_best_rec_v[4] = dummy;
    mod_info_best_rec_v[5] = dummy;
    mod_info_best_rec_v[6] = dummy;
    mod_info_best_rec_v[7] = dummy;
    mod_info_best_rec_v[8] = dummy;
    mod_info_best_rec_v[9] = dummy;
    mod_info_best_rec_v[10] = dummy;
    mod_info_best_rec_v[11] = dummy;
    mod_info_best_rec_v[12] = dummy;
    mod_info_best_rec_v[13] = dummy;
    mod_info_best_rec_v[14] = dummy;
    mod_info_best_rec_v[15] = dummy;
    mod_info_best_rec_v[16] = dummy;
    mod_info_best_rec_v[17] = dummy;
    mod_info_best_rec_v[18] = dummy;
    mod_info_best_rec_v[19] = dummy;
    mod_info_best_rec_v[20] = dummy;
    mod_info_best_rec_v[21] = dummy;
    mod_info_best_rec_v[22] = dummy;
    mod_info_best_rec_v[23] = dummy;
    mod_info_best_rec_v[24] = dummy;
    mod_info_best_rec_v[25] = dummy;
    mod_info_best_rec_v[26] = dummy;
    mod_info_best_rec_v[27] = dummy;
    mod_info_best_rec_v[28] = dummy;
    mod_info_best_rec_v[29] = dummy;
    mod_info_best_rec_v[30] = dummy;
    mod_info_best_rec_v[31] = dummy;*/
    for (i = 0; i < 32; i = i + 1) {
        for (j = 0; j < 32; j = j + 1) {
            mod_info_best_rec_v[i][j] = dummy;
        }
    }
    // ::pel[32][32] mod_info_best_coef_u; 
    /*(mod_info_best_coef_u[0]) = (mod_info_best_coef_u[1]) = dummy;
    (mod_info_best_coef_u[1]) = (mod_info_best_coef_u[1]) = dummy;
    (mod_info_best_coef_u[2]) = (mod_info_best_coef_u[2]) = dummy;
    (mod_info_best_coef_u[3]) = (mod_info_best_coef_u[3]) = dummy;
    (mod_info_best_coef_u[4]) = (mod_info_best_coef_u[4]) = dummy;
    (mod_info_best_coef_u[5]) = (mod_info_best_coef_u[5]) = dummy;
    (mod_info_best_coef_u[6]) = (mod_info_best_coef_u[6]) = dummy;
    (mod_info_best_coef_u[7]) = (mod_info_best_coef_u[7]) = dummy;
    (mod_info_best_coef_u[8]) = (mod_info_best_coef_u[8]) = dummy;
    (mod_info_best_coef_u[9]) = (mod_info_best_coef_u[9]) = dummy;
    (mod_info_best_coef_u[10]) = (mod_info_best_coef_u[10]) = dummy;
    (mod_info_best_coef_u[11]) = (mod_info_best_coef_u[11]) = dummy;
    (mod_info_best_coef_u[12]) = (mod_info_best_coef_u[12]) = dummy;
    (mod_info_best_coef_u[13]) = (mod_info_best_coef_u[13]) = dummy;
    (mod_info_best_coef_u[14]) = (mod_info_best_coef_u[14]) = dummy;
    (mod_info_best_coef_u[15]) = (mod_info_best_coef_u[15]) = dummy;
    (mod_info_best_coef_u[16]) = (mod_info_best_coef_u[16]) = dummy;
    (mod_info_best_coef_u[17]) = (mod_info_best_coef_u[17]) = dummy;
    (mod_info_best_coef_u[18]) = (mod_info_best_coef_u[18]) = dummy;
    (mod_info_best_coef_u[19]) = (mod_info_best_coef_u[19]) = dummy;
    (mod_info_best_coef_u[20]) = (mod_info_best_coef_u[20]) = dummy;
    (mod_info_best_coef_u[21]) = (mod_info_best_coef_u[21]) = dummy;
    (mod_info_best_coef_u[22]) = (mod_info_best_coef_u[22]) = dummy;
    (mod_info_best_coef_u[23]) = (mod_info_best_coef_u[23]) = dummy;
    (mod_info_best_coef_u[24]) = (mod_info_best_coef_u[24]) = dummy;
    (mod_info_best_coef_u[25]) = (mod_info_best_coef_u[25]) = dummy;
    (mod_info_best_coef_u[26]) = (mod_info_best_coef_u[26]) = dummy;
    (mod_info_best_coef_u[27]) = (mod_info_best_coef_u[27]) = dummy;
    (mod_info_best_coef_u[28]) = (mod_info_best_coef_u[28]) = dummy;
    (mod_info_best_coef_u[29]) = (mod_info_best_coef_u[29]) = dummy;
    (mod_info_best_coef_u[30]) = (mod_info_best_coef_u[30]) = dummy;
    (mod_info_best_coef_u[31]) = (mod_info_best_coef_u[31]) = dummy;*/
    for (i = 0; i < 32; i = i + 1) {
        for (j = 0; j < 32; j = j + 1) {
            mod_info_best_coef_u[i][j] = dummy;
        }
    }
    // ::pel[32][32] mod_info_best_coef_v; 
    /*(mod_info_best_coef_v[0]) = (mod_info_best_coef_v[1]) = dummy;
    (mod_info_best_coef_v[1]) = (mod_info_best_coef_v[1]) = dummy;
    (mod_info_best_coef_v[2]) = (mod_info_best_coef_v[2]) = dummy;
    (mod_info_best_coef_v[3]) = (mod_info_best_coef_v[3]) = dummy;
    (mod_info_best_coef_v[4]) = (mod_info_best_coef_v[4]) = dummy;
    (mod_info_best_coef_v[5]) = (mod_info_best_coef_v[5]) = dummy;
    (mod_info_best_coef_v[6]) = (mod_info_best_coef_v[6]) = dummy;
    (mod_info_best_coef_v[7]) = (mod_info_best_coef_v[7]) = dummy;
    (mod_info_best_coef_v[8]) = (mod_info_best_coef_v[8]) = dummy;
    (mod_info_best_coef_v[9]) = (mod_info_best_coef_v[9]) = dummy;
    (mod_info_best_coef_v[10]) = (mod_info_best_coef_v[10]) = dummy;
    (mod_info_best_coef_v[11]) = (mod_info_best_coef_v[11]) = dummy;
    (mod_info_best_coef_v[12]) = (mod_info_best_coef_v[12]) = dummy;
    (mod_info_best_coef_v[13]) = (mod_info_best_coef_v[13]) = dummy;
    (mod_info_best_coef_v[14]) = (mod_info_best_coef_v[14]) = dummy;
    (mod_info_best_coef_v[15]) = (mod_info_best_coef_v[15]) = dummy;
    (mod_info_best_coef_v[16]) = (mod_info_best_coef_v[16]) = dummy;
    (mod_info_best_coef_v[17]) = (mod_info_best_coef_v[17]) = dummy;
    (mod_info_best_coef_v[18]) = (mod_info_best_coef_v[18]) = dummy;
    (mod_info_best_coef_v[19]) = (mod_info_best_coef_v[19]) = dummy;
    (mod_info_best_coef_v[20]) = (mod_info_best_coef_v[20]) = dummy;
    (mod_info_best_coef_v[21]) = (mod_info_best_coef_v[21]) = dummy;
    (mod_info_best_coef_v[22]) = (mod_info_best_coef_v[22]) = dummy;
    (mod_info_best_coef_v[23]) = (mod_info_best_coef_v[23]) = dummy;
    (mod_info_best_coef_v[24]) = (mod_info_best_coef_v[24]) = dummy;
    (mod_info_best_coef_v[25]) = (mod_info_best_coef_v[25]) = dummy;
    (mod_info_best_coef_v[26]) = (mod_info_best_coef_v[26]) = dummy;
    (mod_info_best_coef_v[27]) = (mod_info_best_coef_v[27]) = dummy;
    (mod_info_best_coef_v[28]) = (mod_info_best_coef_v[28]) = dummy;
    (mod_info_best_coef_v[29]) = (mod_info_best_coef_v[29]) = dummy;
    (mod_info_best_coef_v[30]) = (mod_info_best_coef_v[30]) = dummy;
    (mod_info_best_coef_v[31]) = (mod_info_best_coef_v[31]) = dummy;*/
    for (i = 0; i < 32; i = i + 1) {
        for (j = 0; j < 32; j = j + 1) {
            mod_info_best_coef_v[i][j] = dummy;
        }
    }
    // ::pel[64] IP_buffer_ptr_U_32x32_up_line; 
    dummy += (IP_buffer_ptr_U_32x32_up_line[0]) + (IP_buffer_ptr_U_32x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_U_32x32_left_line; 
    dummy += (IP_buffer_ptr_U_32x32_left_line[0]) + (IP_buffer_ptr_U_32x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_32x32_top_left; 
    dummy += IP_buffer_ptr_U_32x32_top_left;

    // ::pel[64] IP_buffer_ptr_V_32x32_up_line; 
    dummy += (IP_buffer_ptr_V_32x32_up_line[0]) + (IP_buffer_ptr_V_32x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_V_32x32_left_line; 
    dummy += (IP_buffer_ptr_V_32x32_left_line[0]) + (IP_buffer_ptr_V_32x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_32x32_top_left; 
    dummy += IP_buffer_ptr_V_32x32_top_left;

    // ::pel[32] IP_buffer_ptr_U_16x32_up_line; 
    dummy += (IP_buffer_ptr_U_16x32_up_line[0]) + (IP_buffer_ptr_U_16x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_U_16x32_left_line; 
    dummy += (IP_buffer_ptr_U_16x32_left_line[0]) + (IP_buffer_ptr_U_16x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_16x32_top_left; 
    dummy += IP_buffer_ptr_U_16x32_top_left;

    // ::pel[32] IP_buffer_ptr_V_16x32_up_line; 
    dummy += (IP_buffer_ptr_V_16x32_up_line[0]) + (IP_buffer_ptr_V_16x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_V_16x32_left_line; 
    dummy += (IP_buffer_ptr_V_16x32_left_line[0]) + (IP_buffer_ptr_V_16x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_16x32_top_left; 
    dummy += IP_buffer_ptr_V_16x32_top_left;

    // ::pel[64] IP_buffer_ptr_U_32x16_up_line; 
    dummy += (IP_buffer_ptr_U_32x16_up_line[0]) + (IP_buffer_ptr_U_32x16_up_line[1]/*TODO:*/);

    // ::pel[32] IP_buffer_ptr_U_32x16_left_line; 
    dummy += (IP_buffer_ptr_U_32x16_left_line[0]) + (IP_buffer_ptr_U_32x16_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_32x16_top_left; 
    dummy += IP_buffer_ptr_U_32x16_top_left;

    // ::pel[64] IP_buffer_ptr_V_32x16_up_line; 
    dummy += (IP_buffer_ptr_V_32x16_up_line[0]) + (IP_buffer_ptr_V_32x16_up_line[1]/*TODO:*/);

    // ::pel[32] IP_buffer_ptr_V_32x16_left_line; 
    dummy += (IP_buffer_ptr_V_32x16_left_line[0]) + (IP_buffer_ptr_V_32x16_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_32x16_top_left; 
    dummy += IP_buffer_ptr_V_32x16_top_left;

    // ::pel[16] IP_buffer_ptr_U_8x32_up_line; 
    dummy += (IP_buffer_ptr_U_8x32_up_line[0]) + (IP_buffer_ptr_U_8x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_U_8x32_left_line; 
    dummy += (IP_buffer_ptr_U_8x32_left_line[0]) + (IP_buffer_ptr_U_8x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_8x32_top_left; 
    dummy += IP_buffer_ptr_U_8x32_top_left;

    // ::pel[16] IP_buffer_ptr_V_8x32_up_line; 
    dummy += (IP_buffer_ptr_V_8x32_up_line[0]) + (IP_buffer_ptr_V_8x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_V_8x32_left_line; 
    dummy += (IP_buffer_ptr_V_8x32_left_line[0]) + (IP_buffer_ptr_V_8x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_8x32_top_left; 
    dummy += IP_buffer_ptr_V_8x32_top_left;

    // ::pel[64] IP_buffer_ptr_U_32x8_up_line; 
    dummy += (IP_buffer_ptr_U_32x8_up_line[0]) + (IP_buffer_ptr_U_32x8_up_line[1]/*TODO:*/);

    // ::pel[16] IP_buffer_ptr_U_32x8_left_line; 
    dummy += (IP_buffer_ptr_U_32x8_left_line[0]) + (IP_buffer_ptr_U_32x8_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_32x8_top_left; 
    dummy += IP_buffer_ptr_U_32x8_top_left;

    // ::pel[64] IP_buffer_ptr_V_32x8_up_line; 
    dummy += (IP_buffer_ptr_V_32x8_up_line[0]) + (IP_buffer_ptr_V_32x8_up_line[1]/*TODO:*/);

    // ::pel[16] IP_buffer_ptr_V_32x8_left_line; 
    dummy += (IP_buffer_ptr_V_32x8_left_line[0]) + (IP_buffer_ptr_V_32x8_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_32x8_top_left; 
    dummy += IP_buffer_ptr_V_32x8_top_left;

    // ::pel[8] IP_buffer_ptr_U_4x32_up_line; 
    dummy += (IP_buffer_ptr_U_4x32_up_line[0]) + (IP_buffer_ptr_U_4x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_U_4x32_left_line; 
    dummy += (IP_buffer_ptr_U_4x32_left_line[0]) + (IP_buffer_ptr_U_4x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_4x32_top_left; 
    dummy += IP_buffer_ptr_U_4x32_top_left;

    // ::pel[8] IP_buffer_ptr_V_4x32_up_line; 
    dummy += (IP_buffer_ptr_V_4x32_up_line[0]) + (IP_buffer_ptr_V_4x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_V_4x32_left_line; 
    dummy += (IP_buffer_ptr_V_4x32_left_line[0]) + (IP_buffer_ptr_V_4x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_4x32_top_left; 
    dummy += IP_buffer_ptr_V_4x32_top_left;

    // ::pel[64] IP_buffer_ptr_U_32x4_up_line; 
    dummy += (IP_buffer_ptr_U_32x4_up_line[0]) + (IP_buffer_ptr_U_32x4_up_line[1]/*TODO:*/);

    // ::pel[8] IP_buffer_ptr_U_32x4_left_line; 
    dummy += (IP_buffer_ptr_U_32x4_left_line[0]) + (IP_buffer_ptr_U_32x4_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_U_32x4_top_left; 
    dummy += IP_buffer_ptr_U_32x4_top_left;

    // ::pel[64] IP_buffer_ptr_V_32x4_up_line; 
    dummy += (IP_buffer_ptr_V_32x4_up_line[0]) + (IP_buffer_ptr_V_32x4_up_line[1]/*TODO:*/);

    // ::pel[8] IP_buffer_ptr_V_32x4_left_line; 
    dummy += (IP_buffer_ptr_V_32x4_left_line[0]) + (IP_buffer_ptr_V_32x4_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_V_32x4_top_left; 
    dummy += IP_buffer_ptr_V_32x4_top_left;

    // ::pel[128] IP_buffer_ptr_64x64_up_line; 
    dummy += (IP_buffer_ptr_64x64_up_line[0]) + (IP_buffer_ptr_64x64_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_64x64_left_line; 
    dummy += (IP_buffer_ptr_64x64_left_line[0]) + (IP_buffer_ptr_64x64_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_64x64_top_left; 
    dummy += IP_buffer_ptr_64x64_top_left;

    // ::pel[64] IP_buffer_ptr_32x64_up_line; 
    dummy += (IP_buffer_ptr_32x64_up_line[0]) + (IP_buffer_ptr_32x64_up_line[1]/*TODO:*/);

    // ::pel[128] IP_buffer_ptr_32x64_left_line; 
    dummy += (IP_buffer_ptr_32x64_left_line[0]) + (IP_buffer_ptr_32x64_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_32x64_top_left; 
    dummy += IP_buffer_ptr_32x64_top_left;

    // ::pel[128] IP_buffer_ptr_64x32_up_line; 
    dummy += (IP_buffer_ptr_64x32_up_line[0]) + (IP_buffer_ptr_64x32_up_line[1]/*TODO:*/);

    // ::pel[64] IP_buffer_ptr_64x32_left_line; 
    dummy += (IP_buffer_ptr_64x32_left_line[0]) + (IP_buffer_ptr_64x32_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_64x32_top_left; 
    dummy += IP_buffer_ptr_64x32_top_left;

    // ::pel[32] IP_buffer_ptr_16x64_up_line; 
    dummy += (IP_buffer_ptr_16x64_up_line[0]) + (IP_buffer_ptr_16x64_up_line[1]/*TODO:*/);

    // ::pel[128] IP_buffer_ptr_16x64_left_line; 
    dummy += (IP_buffer_ptr_16x64_left_line[0]) + (IP_buffer_ptr_16x64_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_16x64_top_left; 
    dummy += IP_buffer_ptr_16x64_top_left;

    // ::pel[128] IP_buffer_ptr_64x16_up_line; 
    dummy += (IP_buffer_ptr_64x16_up_line[0]) + (IP_buffer_ptr_64x16_up_line[1]/*TODO:*/);

    // ::pel[32] IP_buffer_ptr_64x16_left_line; 
    dummy += (IP_buffer_ptr_64x16_left_line[0]) + (IP_buffer_ptr_64x16_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_64x16_top_left; 
    dummy += IP_buffer_ptr_64x16_top_left;

    // ::pel[16] IP_buffer_ptr_8x64_up_line; 
    dummy += (IP_buffer_ptr_8x64_up_line[0]) + (IP_buffer_ptr_8x64_up_line[1]/*TODO:*/);

    // ::pel[128] IP_buffer_ptr_8x64_left_line; 
    dummy += (IP_buffer_ptr_8x64_left_line[0]) + (IP_buffer_ptr_8x64_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_8x64_top_left; 
    dummy += IP_buffer_ptr_8x64_top_left;

    // ::pel[128] IP_buffer_ptr_64x8_up_line; 
    dummy += (IP_buffer_ptr_64x8_up_line[0]) + (IP_buffer_ptr_64x8_up_line[1]/*TODO:*/);

    // ::pel[16] IP_buffer_ptr_64x8_left_line; 
    dummy += (IP_buffer_ptr_64x8_left_line[0]) + (IP_buffer_ptr_64x8_left_line[1]/*TODO:*/);

    // ::pel IP_buffer_ptr_64x8_top_left; 
    dummy += IP_buffer_ptr_64x8_top_left;

    // ::U32[32] IP_map_scu_ptr_64x64_map_scu_up_line; 
    dummy += IP_map_scu_ptr_64x64_map_scu_up_line[0];

    // ::U32[32] IP_map_scu_ptr_64x64_map_scu_left_line_new; 
    dummy += IP_map_scu_ptr_64x64_map_scu_left_line_new[0];

    // ::U32 IP_map_scu_ptr_64x64_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_64x64_map_scu_top_left_new;

    // ::U32[16] IP_map_scu_ptr_32x64_map_scu_up_line; 
    dummy += (IP_map_scu_ptr_32x64_map_scu_up_line[0]) + (IP_map_scu_ptr_32x64_map_scu_up_line[1]/*TODO:*/);

    // ::U32[32] IP_map_scu_ptr_32x64_map_scu_left_line_new; 
    dummy += IP_map_scu_ptr_32x64_map_scu_left_line_new[0];

    // ::U32 IP_map_scu_ptr_32x64_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_32x64_map_scu_top_left_new;

    // ::U32[32] IP_map_scu_ptr_64x32_map_scu_up_line; 
    dummy += (IP_map_scu_ptr_64x32_map_scu_up_line[0]) + (IP_map_scu_ptr_64x32_map_scu_up_line[1]/*TODO:*/);

    // ::U32[16] IP_map_scu_ptr_64x32_map_scu_left_line_new; 
    dummy += (IP_map_scu_ptr_64x32_map_scu_left_line_new[0]) + (IP_map_scu_ptr_64x32_map_scu_left_line_new[1]/*TODO:*/);

    // ::U32 IP_map_scu_ptr_64x32_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_64x32_map_scu_top_left_new;

    // ::U32[8] IP_map_scu_ptr_16x64_map_scu_up_line; 
    dummy += (IP_map_scu_ptr_16x64_map_scu_up_line[0]) + (IP_map_scu_ptr_16x64_map_scu_up_line[1]/*TODO:*/);

    // ::U32[32] IP_map_scu_ptr_16x64_map_scu_left_line_new; 
    dummy += IP_map_scu_ptr_16x64_map_scu_left_line_new[0];

    // ::U32 IP_map_scu_ptr_16x64_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_16x64_map_scu_top_left_new;

    // ::U32[32] IP_map_scu_ptr_64x16_map_scu_up_line; 
    dummy += (IP_map_scu_ptr_64x16_map_scu_up_line[0]) + (IP_map_scu_ptr_64x16_map_scu_up_line[1]/*TODO:*/);

    // ::U32[8] IP_map_scu_ptr_64x16_map_scu_left_line_new; 
    dummy += (IP_map_scu_ptr_64x16_map_scu_left_line_new[0]) + (IP_map_scu_ptr_64x16_map_scu_left_line_new[1]/*TODO:*/);

    // ::U32 IP_map_scu_ptr_64x16_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_64x16_map_scu_top_left_new;

    // ::U32[4] IP_map_scu_ptr_8x64_map_scu_up_line; 
    dummy += (IP_map_scu_ptr_8x64_map_scu_up_line[0]) + (IP_map_scu_ptr_8x64_map_scu_up_line[1]/*TODO:*/);

    // ::U32[32] IP_map_scu_ptr_8x64_map_scu_left_line_new; 
    dummy += IP_map_scu_ptr_8x64_map_scu_left_line_new[0];

    // ::U32 IP_map_scu_ptr_8x64_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_8x64_map_scu_top_left_new;

    // ::U32[32] IP_map_scu_ptr_64x8_map_scu_up_line; 
    dummy += (IP_map_scu_ptr_64x8_map_scu_up_line[0]) + (IP_map_scu_ptr_64x8_map_scu_up_line[1]/*TODO:*/);

    // ::U32[4] IP_map_scu_ptr_64x8_map_scu_left_line_new; 
    dummy += (IP_map_scu_ptr_64x8_map_scu_left_line_new[0]) + (IP_map_scu_ptr_64x8_map_scu_left_line_new[1]/*TODO:*/);

    // ::U32 IP_map_scu_ptr_64x8_map_scu_top_left_new; 
    dummy += IP_map_scu_ptr_64x8_map_scu_top_left_new;

#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_UP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP core=RAM_2P_BRAM 
#pragma HLS resource variable=Fetch_Ref_window_ptr[0].RefWindowLCU_VP_ref1 core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x64_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_16x64_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x64_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_32x64_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x16_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x16_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x64_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x8_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_64x8_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x64_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_8x64_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_16x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_16x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x16_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x16_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x4_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x4_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x8_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_32x8_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_4x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_4x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_8x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_U_8x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_16x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_16x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x16_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x16_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x4_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x4_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x8_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_32x8_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_4x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_4x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_8x32_left_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_buffer_ptr_V_8x32_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x64_map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_16x64_map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x64_map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_32x64_map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x16_map_scu_left_line_new core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x16_map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x32_map_scu_left_line_new core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x32_map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64_map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x64_map_scu_up_line core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x8_map_scu_left_line_new core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_64x8_map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x64_map_scu_left_line_new core=RAM_1P_BRAM 
#pragma HLS resource variable=IP_map_scu_ptr_8x64_map_scu_up_line core=RAM_2P_BRAM 
#pragma HLS resource variable=mod_info_best[0].num_nz core=RAM_T2P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_u core=RAM_T2P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_v core=RAM_T2P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_v core=RAM_1P_BRAM 
#pragma HLS resource variable=orgU core=RAM_1P_BRAM 
#pragma HLS resource variable=piRecoY core=RAM_1P_BRAM 
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].cur_refi complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].lidx complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_is_library_picture complete dim=2
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=1
//#pragma HLS array_partition variable=Fetch_Ref_window_ptr[0].refp_pic_ptr complete dim=2
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].mv complete dim=1
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].mv complete dim=2
#pragma HLS array_partition variable=core_sbtmvp[0].sbTmvp[0].ref_idx complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].dist_chroma_weight complete dim=1
#pragma HLS array_partition variable=md_input_ptr[0].lambda complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].ipm complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=1
#pragma HLS array_partition variable=mod_info_best[0].mv complete dim=2
#pragma HLS array_partition variable=mod_info_best[0].refi complete dim=1
#pragma HLS array_partition variable=mod_info_best_coef_u cyclic factor=32 dim=1
#pragma HLS array_partition variable=mod_info_best_coef_v cyclic factor=32 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_u cyclic factor=32 dim=1
#pragma HLS array_partition variable=mod_info_best_rec_v cyclic factor=32 dim=1
#pragma HLS array_partition variable=orgU cyclic factor=32 dim=3	// liaozhx
#pragma HLS array_partition variable=piRecoY cyclic factor=2 dim=1
#pragma HLS array_partition variable=piRecoY cyclic factor=64 dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_cbf complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=3
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_last complete dim=4
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_level complete dim=2
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=1
#pragma HLS array_partition variable=rdoq_array[0].rdoq_est_run complete dim=2
}
#endif//CHROMA_32_PIPELINE_DUMMY
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
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father)
{
	return 1;
}
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
	strFetch_ref_window* Fetch_Ref_window_ptr, ENC_CU_DATA* md_output_luma)
{
}
#endif





#if VMOD_DUMMY
void vmod(LCUConfig& config, LCUInput& buffer, Arbitor& ref
#if MD_CHANNEL_DEBUG
	, Channel& input, Channel& output
#endif
	, Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], Instruction mem_store[PR_SIZE], ENC_CU_DATA& md_output)
{
#pragma HLS INLINE off

   s64 dummy = 0;

   dummy+=config . md_fw_ptr . pic_width;
   dummy+=config . md_fw_ptr . pic_height;
   dummy+=config . md_fw_ptr . slice_type;
   dummy+=config . md_fw_ptr . slice_qp;
   dummy+=config . md_fw_ptr . max_dt_size;
   dummy+=config . md_fw_ptr . affine_enable_flag;
   dummy+=config . md_fw_ptr . umve_enable_flag;
   dummy+=config . md_fw_ptr . dt_intra_enable_flag;
   dummy+=config . md_fw_ptr . num_of_hmvp_cand;
   dummy+=config . md_fw_ptr . ipf_enable_flag;
   dummy+=config . md_fw_ptr . pic_width_in_scu;
   dummy+=config . md_fw_ptr . pic_height_in_scu;
   dummy+=config . md_fw_ptr . log2_max_cuwh;
   dummy+=config . md_fw_ptr . log2_culine;
   dummy+=config . md_fw_ptr . max_cuwh;
   dummy+=config . md_fw_ptr . f_scu;
   dummy+=config . md_fw_ptr . gop_size;
   dummy+=config . md_fw_ptr . bit_depth_internal;
   dummy+=config . md_fw_ptr . max_part_ratio;
   dummy+=config . md_fw_ptr . min_cu_size;
   dummy+=config . md_fw_ptr . max_split_times;
   dummy+=config . md_fw_ptr . min_qt_size;
   dummy+=config . md_fw_ptr . max_bt_size;
   dummy+=config . md_fw_ptr . max_eqt_size;
   dummy+=config . md_fw_ptr . affine_subblock_size_idx;
   dummy+=config . md_fw_ptr . ipcm_enable_flag;
   dummy+=config . md_fw_ptr . position_based_transform_enable_flag;
   dummy+=config . md_fw_ptr . secondary_transform_enable_flag;
   dummy+=config . md_fw_ptr . tscpm_enable_flag;
   dummy+=config . md_fw_ptr . amvr_enable_flag;
   dummy+=config . md_fw_ptr . emvr_enable_flag;
   dummy+=config . md_fw_ptr . eipm_enable_flag;
   dummy+=config . md_fw_ptr . mipf_enable_flag;
   dummy+=config . md_fw_ptr . interpf_enable_flag;
   dummy+=config . md_fw_ptr . umve_set_flag;
   // config . md_fw_ptr . refp_ptr[0][0] is ignored ;
   // config . md_fw_ptr . num_refp[0] is ignored ;
   dummy+=config . md_fw_ptr . cur_ptr;
   dummy+=config . md_fw_ptr . chroma_quant_param_delta_cb;
   dummy+=config . md_fw_ptr . chroma_quant_param_delta_cr;
   dummy+=config . md_input_ptr . pix_x;
   dummy+=config . md_input_ptr . pix_y;
   dummy+=config . md_input_ptr . lambda_mv;
   // config . md_input_ptr . lambda[0] is ignored ;
   dummy+=config . md_input_ptr . lambda_y;
   // config . md_input_ptr . lambda_u is ignored ;
   // config . md_input_ptr . lambda_v is ignored ;
   dummy+=config . md_input_ptr . qp_y;
   // config . md_input_ptr . qp_u is ignored ;
   // config . md_input_ptr . qp_v is ignored ;
   // config . md_input_ptr . dist_chroma_weight[0] is ignored ;
   dummy+=config . md_input_ptr . x_last_ctu;
   dummy+=config . md_input_ptr . y_last_ctu;
   // config . md_input_ptr . CtrPos2MD[0][0][0] is ignored ;
   // config . md_input_ptr . CtrPos2MD_ref1[0][0][0] is ignored ;
   // config . md_input_ptr . split_mode[0][0][0] is ignored ;
   // config . md_input_ptr . do_chroma is ignored ;
   // config . md_input_ptr . pred_mode[0] is ignored ;
   // config . md_input_ptr . ipm[0][0] is ignored ;
   // config . md_input_ptr . affine_flag[0] is ignored ;
   // config . md_input_ptr . mv[0][0][0] is ignored ;
   // config . md_input_ptr . refi[0][0] is ignored ;
   // config . md_input_ptr . num_nz_coef[0][0] is ignored ;
   // config . md_input_ptr . map_scu[0] is ignored ;
   // config . md_input_ptr . map_cu_mode[0] is ignored ;
   // config . md_input_ptr . reco_y[0][0] is ignored ;
   // config . md_input_ptr . umve_flag[0] is ignored ;
   // config . md_input_ptr . skip_idx[0] is ignored ;
   // config . md_input_ptr . inter_filter_flag[0] is ignored ;
   dummy+=config . md_input_ptr . cu_dqp_enable;
   // config . md_input_ptr . cu_qp_group_area_size is ignored ;
   // config . md_input_ptr . cu_qp_group_pred_qp is ignored ;
   // config . md_input_ptr . cu_luma_qp[0] is ignored ;
   // config . md_input_ptr . chroma_quant_param_delta_cb is ignored ;
   // config . md_input_ptr . chroma_quant_param_delta_cr is ignored ;
   // config . md_input_ptr . md_slice_qp is ignored ;
   // config . md_input_ptr . CuQPMap[0] is ignored ;
   dummy+=config . min_scu_x;
   dummy+=config . min_scu_y;

   // struct ::LCUInput: public DeleteOperator{struct RMD_OUTPUT g_rmd_output;pel RefWindowLCU_UP[12288];pel RefWindowLCU_VP[12288];S14 CtrPos[2][2][2];pel RefWindowLCU_UP_ref1[12288];pel RefWindowLCU_VP_ref1[12288];S14 CtrPos_ref1[2][2][2];ME_MV_DIR me_mv_dir;pel p_fenc_LCU_Y[64][64];SKIP_MERGE_INPUT skip_in;ENC_FME_INFO_ALL fme_mv_info;strFetch_ref_window Fetch_Ref_window_ptr;friend inline void copy(int w,int h,struct LCUInput &_Left,struct LCUInput &_Right){copy(_Left . g_rmd_output,_Right . g_rmd_output);copy(_Left . me_mv_dir,_Right . me_mv_dir);copy(_Left . fme_mv_info,_Right . fme_mv_info);for(int i = 0;i < 2;++i) {for(int j = 0;j < 2;++j) {for(int k = 0;k < 2;++k) {_Left . CtrPos[i][j][k] = _Right . CtrPos[i][j][k];_Left . CtrPos_ref1[i][j][k] = _Right . CtrPos_ref1[i][j][k];}}}for(int i = 0;i < 64;++i) {for(int j = 0;j < 64;++j) {_Left . p_fenc_LCU_Y[i][j] = _Right . p_fenc_LCU_Y[i][j];}}copy(_Left . skip_in,_Right . skip_in);copy_idx(_Left . Fetch_Ref_window_ptr,_Right . Fetch_Ref_window_ptr);}}& buffer; 
   dummy+=buffer . g_rmd_output . modes_8x8[0][0];
   dummy+=buffer . g_rmd_output . modes_16x8[0][0];
   dummy+=buffer . g_rmd_output . modes_32x8[0][0];
   dummy+=buffer . g_rmd_output . modes_64x8[0][0];
   dummy+=buffer . g_rmd_output . modes_8x16[0][0];
   dummy+=buffer . g_rmd_output . modes_16x16[0][0];
   dummy+=buffer . g_rmd_output . modes_32x16[0][0];
   dummy+=buffer . g_rmd_output . modes_64x16[0][0];
   dummy+=buffer . g_rmd_output . modes_8x32[0][0];
   dummy+=buffer . g_rmd_output . modes_16x32[0][0];
   dummy+=buffer . g_rmd_output . modes_32x32[0][0];
   dummy+=buffer . g_rmd_output . modes_64x32[0][0];
   dummy+=buffer . g_rmd_output . modes_8x64[0][0];
   dummy+=buffer . g_rmd_output . modes_16x64[0][0];
   dummy+=buffer . g_rmd_output . modes_32x64[0][0];
   dummy+=buffer . g_rmd_output . modes_64x64[0][0];
   // buffer . RefWindowLCU_UP[0] is ignored ;
   // buffer . RefWindowLCU_VP[0] is ignored ;
   dummy+=buffer . CtrPos[0][0][0];
   // buffer . RefWindowLCU_UP_ref1[0] is ignored ;
   // buffer . RefWindowLCU_VP_ref1[0] is ignored ;
   dummy+=buffer . CtrPos_ref1[0][0][0];
   dummy+=buffer . me_mv_dir . block64x64MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block64x64MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block32x32MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block32x32MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block64x32MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block64x32MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block32x64MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block32x64MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block16x16MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block16x16MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block32x16MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block32x16MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block16x32MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block16x32MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block64x16MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block64x16MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block16x64MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block16x64MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block8x8MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block8x8MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block16x8MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block16x8MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block8x16MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block8x16MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block32x8MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block32x8MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block8x32MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block8x32MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block64x8MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block64x8MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block8x64MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block8x64MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block32x4MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block32x4MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block4x32MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block4x32MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block16x4MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block16x4MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block4x16MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block4x16MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block8x4MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block8x4MvDir[0][0] . cur_refi;
   dummy+=buffer . me_mv_dir . block4x8MvDir[0][0] . lidx;
   dummy+=buffer . me_mv_dir . block4x8MvDir[0][0] . cur_refi;
   dummy+=buffer . p_fenc_LCU_Y[0][0];
   dummy+=buffer . skip_in . slice_type;
   dummy+=buffer . skip_in . ptr;
   dummy+=buffer . skip_in . umve_enable_flag;
   dummy+=buffer . skip_in . refp_map_mv[0][0][0];
   dummy+=buffer . skip_in . refp_map_refi[0][0];
   dummy+=buffer . skip_in . pix_x;
   dummy+=buffer . skip_in . refp_map_mv1[0][0][0][0];
   dummy+=buffer . skip_in . refp_map_refi1[0][0][0][0];
   dummy+=buffer . skip_in . refp_list_ptr1[0][0];
   dummy+=buffer . skip_in . refp_ptr1[0][0];
   dummy+=buffer . fme_mv_info . sadTree64x64Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree32x32Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree64x32Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree32x64Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree16x16Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree32x16Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree16x32Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree64x16Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree16x64Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree8x8Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree16x8Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree8x16Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree32x8Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree8x32Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree64x8Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree8x64Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree4x8Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree8x4Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree16x4Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree4x16Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree32x4Fmv[0][0][0];
   dummy+=buffer . fme_mv_info . sadTree4x32Fmv[0][0][0];
   // buffer . Fetch_Ref_window_ptr . RefWindowLCU[0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . swOutLv0[0][0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . CtrPos[0][0][0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . RefWindowLCU_UP[0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . RefWindowLCU_VP[0] is ignored ;
   dummy+=buffer.Fetch_Ref_window_ptr.code_stru;
   dummy+=buffer . Fetch_Ref_window_ptr . lidx[0];
   dummy+=buffer . Fetch_Ref_window_ptr . cur_refi[0];
   // buffer . Fetch_Ref_window_ptr . refp_pic_ptr[0][0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . refp_is_library_picture[0][0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . CtrPos_ref1[0][0][0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . RefWindowLCU_ref1[0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . swOutLv0_ref1[0][0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . RefWindowLCU_UP_ref1[0] is ignored ;
   // buffer . Fetch_Ref_window_ptr . RefWindowLCU_VP_ref1[0] is ignored ;

   // struct ::Arbitor{ref_pel ref_0_0[2560][16];ref_pel ref_0_1[2560][16];ref_pel ref_1_0[2560][16];ref_pel ref_1_1[2560][16];ref_pel ref_2_0[2560][16];ref_pel ref_2_1[2560][16];ref_pel ref_3_0[2560][16];ref_pel ref_3_1[2560][16];ref_pel ref_4_0[2560][16];ref_pel ref_4_1[2560][16];ref_pel ref_5_0[2560][16];ref_pel ref_5_1[2560][16];ref_pel ref_6_0[2560][16];ref_pel ref_6_1[2560][16];ref_pel ref_7_0[2560][16];ref_pel ref_7_1[2560][16];inline Arbitor &operator=(pel refwin_mc[2560][8][2][16]){for(int i = 0;i < 2560;++i) {for(int j = 0;j < 16;++j) {(this) -> ref_0_0[i][j] = refwin_mc[i][0][0][j];(this) -> ref_0_1[i][j] = refwin_mc[i][0][1][j];(this) -> ref_1_0[i][j] = refwin_mc[i][1][0][j];(this) -> ref_1_1[i][j] = refwin_mc[i][1][1][j];(this) -> ref_2_0[i][j] = refwin_mc[i][2][0][j];(this) -> ref_2_1[i][j] = refwin_mc[i][2][1][j];(this) -> ref_3_0[i][j] = refwin_mc[i][3][0][j];(this) -> ref_3_1[i][j] = refwin_mc[i][3][1][j];(this) -> ref_4_0[i][j] = refwin_mc[i][4][0][j];(this) -> ref_4_1[i][j] = refwin_mc[i][4][1][j];(this) -> ref_5_0[i][j] = refwin_mc[i][5][0][j];(this) -> ref_5_1[i][j] = refwin_mc[i][5][1][j];(this) -> ref_6_0[i][j] = refwin_mc[i][6][0][j];(this) -> ref_6_1[i][j] = refwin_mc[i][6][1][j];(this) -> ref_7_0[i][j] = refwin_mc[i][7][0][j];(this) -> ref_7_1[i][j] = refwin_mc[i][7][1][j];}}return *(this);}}& ref; 
   dummy+=ref . ref_0_0[0][0];
   dummy+=ref . ref_0_1[0][0];
   dummy+=ref . ref_1_0[0][0];
   dummy+=ref . ref_1_1[0][0];
   dummy+=ref . ref_2_0[0][0];
   dummy+=ref . ref_2_1[0][0];
   dummy+=ref . ref_3_0[0][0];
   dummy+=ref . ref_3_1[0][0];
   dummy+=ref . ref_4_0[0][0];
   dummy+=ref . ref_4_1[0][0];
   dummy+=ref . ref_5_0[0][0];
   dummy+=ref . ref_5_1[0][0];
   dummy+=ref . ref_6_0[0][0];
   dummy+=ref . ref_6_1[0][0];
   dummy+=ref . ref_7_0[0][0];
   dummy+=ref . ref_7_1[0][0];

   // struct ::Instruction{class ap_uint< 256 > buf;typedef struct ap_range_ref< 256 , false > InsnRef;inline Instruction(){(this) -> buf = 0;}inline InsnRef x0() const{return((this) -> buf(12,0));}inline InsnRef y0() const{return((this) -> buf(26,14));}inline InsnRef cu_width_log2() const{return((this) -> buf(30,28));}inline InsnRef cu_height_log2() const{return((this) -> buf(33,31));}inline InsnRef cup() const{return((this) -> buf(45,34));}inline InsnRef cud() const{return((this) -> buf(61,58));}inline InsnRef qtd() const{return((this) -> buf(64,62));}inline InsnRef btd() const{return((this) -> buf(68,66));}inline InsnRef tree_status() const{return((this) -> buf(71,70));}inline InsnRef pred_mode() const{return((this) -> buf(73,72));}inline InsnRef split_mode() const{return((this) -> buf(76,74));}inline InsnRef part_num() const{return((this) -> buf(79,77));}inline InsnRef prev_up_log2_sub_cuw() const{return((this) -> buf(82,80));}inline InsnRef x_pos() const{return((this) -> buf(96,83));}inline InsnRef y_pos() const{return((this) -> buf(110,97));}inline InsnRef flag_pu() const{return((this) -> buf(159,128));}inline InsnRef flag_chn() const{return((this) -> buf(191,160));}inline InsnRef tag_pu() const{return((this) -> buf(199,192));}inline InsnRef tag_chn() const{return((this) -> buf(215,208));}inline InsnRef mem_ptr() const{return((this) -> buf(223,216));}inline InsnRef version() const{return((this) -> buf(247,244));}inline InsnRef opcode() const{return((this) -> buf(255,248));}inline InsnRef flag_pu(U8 pu_idx) const{return((this) -> buf((128 + pu_idx),(128 + pu_idx)));}inline InsnRef flag_chn(U8 chn_idx) const{return((this) -> buf((160 + chn_idx),(160 + chn_idx)));}inline Instruction &operator=(const struct Instruction &_Right){(this) -> buf = _Right . buf;return *(this);}inline Instruction &operator=(const class ap_uint< 256 > &_Right){(this) -> buf = _Right;return *(this);}}[256] mem_load_buf; 
   // mem_load_buf[0] . buf . dummy[0] is ignored ;

   dummy+=mem_load[0] . buf;  // liaozhx

   // struct ::Instruction{class ap_uint< 256 > buf;typedef struct ap_range_ref< 256 , false > InsnRef;inline Instruction(){(this) -> buf = 0;}inline InsnRef x0() const{return((this) -> buf(12,0));}inline InsnRef y0() const{return((this) -> buf(26,14));}inline InsnRef cu_width_log2() const{return((this) -> buf(30,28));}inline InsnRef cu_height_log2() const{return((this) -> buf(33,31));}inline InsnRef cup() const{return((this) -> buf(45,34));}inline InsnRef cud() const{return((this) -> buf(61,58));}inline InsnRef qtd() const{return((this) -> buf(64,62));}inline InsnRef btd() const{return((this) -> buf(68,66));}inline InsnRef tree_status() const{return((this) -> buf(71,70));}inline InsnRef pred_mode() const{return((this) -> buf(73,72));}inline InsnRef split_mode() const{return((this) -> buf(76,74));}inline InsnRef part_num() const{return((this) -> buf(79,77));}inline InsnRef prev_up_log2_sub_cuw() const{return((this) -> buf(82,80));}inline InsnRef x_pos() const{return((this) -> buf(96,83));}inline InsnRef y_pos() const{return((this) -> buf(110,97));}inline InsnRef flag_pu() const{return((this) -> buf(159,128));}inline InsnRef flag_chn() const{return((this) -> buf(191,160));}inline InsnRef tag_pu() const{return((this) -> buf(199,192));}inline InsnRef tag_chn() const{return((this) -> buf(215,208));}inline InsnRef mem_ptr() const{return((this) -> buf(223,216));}inline InsnRef version() const{return((this) -> buf(247,244));}inline InsnRef opcode() const{return((this) -> buf(255,248));}inline InsnRef flag_pu(U8 pu_idx) const{return((this) -> buf((128 + pu_idx),(128 + pu_idx)));}inline InsnRef flag_chn(U8 chn_idx) const{return((this) -> buf((160 + chn_idx),(160 + chn_idx)));}inline Instruction &operator=(const struct Instruction &_Right){(this) -> buf = _Right . buf;return *(this);}inline Instruction &operator=(const class ap_uint< 256 > &_Right){(this) -> buf = _Right;return *(this);}}[256] mem_pu_buf; 
   // mem_pu_buf[0] . buf . dummy[0] is ignored ;

   dummy+=mem_pu[0] . buf;  // liaozhx

   // struct ::Instruction{class ap_uint< 256 > buf;typedef struct ap_range_ref< 256 , false > InsnRef;inline Instruction(){(this) -> buf = 0;}inline InsnRef x0() const{return((this) -> buf(12,0));}inline InsnRef y0() const{return((this) -> buf(26,14));}inline InsnRef cu_width_log2() const{return((this) -> buf(30,28));}inline InsnRef cu_height_log2() const{return((this) -> buf(33,31));}inline InsnRef cup() const{return((this) -> buf(45,34));}inline InsnRef cud() const{return((this) -> buf(61,58));}inline InsnRef qtd() const{return((this) -> buf(64,62));}inline InsnRef btd() const{return((this) -> buf(68,66));}inline InsnRef tree_status() const{return((this) -> buf(71,70));}inline InsnRef pred_mode() const{return((this) -> buf(73,72));}inline InsnRef split_mode() const{return((this) -> buf(76,74));}inline InsnRef part_num() const{return((this) -> buf(79,77));}inline InsnRef prev_up_log2_sub_cuw() const{return((this) -> buf(82,80));}inline InsnRef x_pos() const{return((this) -> buf(96,83));}inline InsnRef y_pos() const{return((this) -> buf(110,97));}inline InsnRef flag_pu() const{return((this) -> buf(159,128));}inline InsnRef flag_chn() const{return((this) -> buf(191,160));}inline InsnRef tag_pu() const{return((this) -> buf(199,192));}inline InsnRef tag_chn() const{return((this) -> buf(215,208));}inline InsnRef mem_ptr() const{return((this) -> buf(223,216));}inline InsnRef version() const{return((this) -> buf(247,244));}inline InsnRef opcode() const{return((this) -> buf(255,248));}inline InsnRef flag_pu(U8 pu_idx) const{return((this) -> buf((128 + pu_idx),(128 + pu_idx)));}inline InsnRef flag_chn(U8 chn_idx) const{return((this) -> buf((160 + chn_idx),(160 + chn_idx)));}inline Instruction &operator=(const struct Instruction &_Right){(this) -> buf = _Right . buf;return *(this);}inline Instruction &operator=(const class ap_uint< 256 > &_Right){(this) -> buf = _Right;return *(this);}}[256] mem_store_buf; 
   // mem_store_buf[0] . buf . dummy[0] is ignored ;

   dummy+=mem_store[0] . buf;  // liaozhx

   // ::ENC_CU_DATA& md_output; 
   md_output . split_mode[0][0][0]=dummy;
   // md_output . pb_part[0]=dummy;
   md_output . tb_part[0]=dummy;
   md_output . pred_mode[0]=dummy;
   md_output . mpm[0][0]=dummy;
   // md_output . mpm_ext[0][0] is ignored ;
   md_output . ipm[0][0]=dummy;
   md_output . refi[0][0]=dummy;
   md_output . mvr_idx[0]=dummy;
   md_output . umve_flag[0]=dummy;
   md_output . umve_idx[0]=dummy;
   md_output . skip_idx[0]=dummy;
   md_output . mvp_from_hmvp_flag[0]=dummy;
   md_output . mv[0][0][0]=dummy;
   md_output . mvd[0][0][0]=dummy;
   md_output . num_nz_coef[0][0]=dummy;
   md_output . map_scu[0]=dummy;
   md_output . affine_flag[0]=dummy;
   // md_output . smvd_flag[0] is ignored ;
   md_output . map_cu_mode[0]=dummy;
   // md_output . map_pb_tb_part[0] is ignored ;
   // md_output . depth[0] is ignored ;
   md_output . ipf_flag[0]=dummy;
   // md_output . coef[0][0]=dummy;
   md_output . coef_y[0][0]=dummy;
   // md_output . coef_u[0] is ignored ;
   // md_output . coef_v[0] is ignored ;
   md_output . reco_y[0][0]=dummy;
   // md_output . reco_u[0] is ignored ;
   // md_output . reco_v[0] is ignored ;
   // md_output . sawp_flag[0] is ignored ;
   // md_output . sawp_idx0[0] is ignored ;
   // md_output . sawp_idx1[0] is ignored ;
   // md_output . awp_mvr_flag0[0] is ignored ;
   // md_output . awp_mvr_idx0[0] is ignored ;
   // md_output . awp_mvr_flag1[0] is ignored ;
   // md_output . awp_mvr_idx1[0] is ignored ;
   // md_output . bvr_idx[0] is ignored ;
   // md_output . inter_filter_flag[0] is ignored ;
   // md_output . ipc_flag[0] is ignored ;
   // md_output . bgc_flag[0] is ignored ;
   // md_output . bgc_idx[0] is ignored ;
   // md_output . ibc_flag[0] is ignored ;
   // md_output . cbvp_idx[0] is ignored ;
   // md_output . cnt_hbvp_cands[0] is ignored ;
   // md_output . map_usp[0] is ignored ;
   // md_output . sp_flag[0] is ignored ;
   // md_output . sub_string_no[0] is ignored ;
   // md_output . sp_strInfo[0] . is_matched is ignored ;
   // md_output . sp_strInfo[0] . length is ignored ;
   // md_output . sp_strInfo[0] . offset_x is ignored ;
   // md_output . sp_strInfo[0] . offset_y is ignored ;
   // md_output . sp_strInfo[0] . match_dict[0] is ignored ;
   // md_output . sp_strInfo[0] . pixel[0][0] is ignored ;
   // md_output . sp_strInfo[0] . n_recent_flag is ignored ;
   // md_output . sp_strInfo[0] . n_recent_idx is ignored ;
   // md_output . sp_copy_direction[0] is ignored ;
   // md_output . is_sp_pix_completed[0] is ignored ;
   // md_output . cs2_flag[0] is ignored ;
   // md_output . evs_copy_direction[0] is ignored ;
   // md_output . evs_sub_string_no[0] is ignored ;
   // md_output . evs_str_copy_info[0] . is_matched is ignored ;
   // md_output . evs_str_copy_info[0] . length is ignored ;
   // md_output . evs_str_copy_info[0] . pixel[0] is ignored ;
   // md_output . evs_str_copy_info[0] . match_type is ignored ;
   // md_output . evs_str_copy_info[0] . srb_index is ignored ;
   // md_output . evs_str_copy_info[0] . pv_type is ignored ;
   // md_output . evs_str_copy_info[0] . pvflag is ignored ;
   // md_output . evs_str_copy_info[0] . srb_bits is ignored ;
   // md_output . evs_str_copy_info[0] . srb_dist is ignored ;
   // md_output . evs_str_copy_info[0] . esc_flag is ignored ;
   // md_output . evs_str_copy_info[0] . pos is ignored ;
   // md_output . unpred_pix_num[0] is ignored ;
   // md_output . unpred_pix_info[0] . Y is ignored ;
   // md_output . unpred_pix_info[0] . U is ignored ;
   // md_output . unpred_pix_info[0] . V is ignored ;
   // md_output . equal_val_str_present_flag[0] is ignored ;
   // md_output . unpredictable_pix_present_flag[0] is ignored ;
   // md_output . pvbuf_size[0] is ignored ;
   // md_output . pvbuf_size_prev[0] is ignored ;
   // md_output . p_SRB[0] is ignored ;
   // md_output . pvbuf_reused_flag[0] is ignored ;
   // md_output . p_SRB_prev[0] is ignored ;
   // md_output . all_comp_flag[0] is ignored ;
   // md_output . all_comp_pre_flag[0] is ignored ;
   // md_output . m_dpb_idx[0] is ignored ;
   // md_output . m_dpb_idx_prev[0] is ignored ;
   // md_output . m_dpb_reYonly[0] is ignored ;
   // md_output . m_dpb_reYonly_prev[0] is ignored ;
   // md_output . cuS_flag[0] is ignored ;
   // md_output . cuS_pre_flag[0] is ignored ;
   // md_output . pv_x[0] is ignored ;
   // md_output . pv_x_prev[0] is ignored ;
   // md_output . pv_y[0] is ignored ;
   // md_output . pv_y_prev[0] is ignored ;
   // md_output . srb_u[0] is ignored ;
   // md_output . srb_v[0] is ignored ;
   // md_output . affine_umve_flag[0] is ignored ;
   // md_output . affine_umve_idx[0][0] is ignored ;
   // md_output . etmvp_flag[0] is ignored ;
   // md_output . mvap_flag[0] is ignored ;
   // md_output . sub_tmvp_flag[0] is ignored ;
   // md_output . awp_flag[0] is ignored ;
   // md_output . awp_idx0[0] is ignored ;
   // md_output . awp_idx1[0] is ignored ;
   // md_output . st_chroma_tu_flag[0] is ignored ;
   // md_output . iip_flag[0] is ignored ;
   // md_output . ist_tu_flag[0] is ignored ;
   // md_output . est_tu_flag[0] is ignored ;
   // md_output . sawp_mpm[0][0] is ignored ;
   // md_output . y_org_sao[0][0] is ignored ;
   // md_output . u_org_sao[0][0] is ignored ;
   // md_output . v_org_sao[0][0] is ignored ;
   // md_output . cu_luma_qp[0] is ignored ;

#pragma HLS ARRAY_RESHAPE variable = ref.ref_0_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_0_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_1_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_1_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_2_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_2_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_3_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_3_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_4_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_4_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_5_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_5_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_6_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_6_1   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_7_0   dim=2 complete
#pragma HLS ARRAY_RESHAPE variable = ref.ref_7_1   dim=2 complete

// #pragma HLS resource variable= core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.CtrPos core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.CtrPos_ref1 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.Fetch_Ref_window_ptr.cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.Fetch_Ref_window_ptr.lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree16x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree16x32Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree16x4Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree16x64Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree16x8Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree32x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree32x32Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree32x4Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree32x64Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree32x8Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree4x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree4x32Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree4x8Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree64x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree64x32Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree64x64Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree64x8Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree8x16Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree8x32Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree8x4Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree8x64Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.fme_mv_info.sadTree8x8Fmv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_16x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_16x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_16x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_16x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_32x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_32x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_32x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_32x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_64x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_64x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_64x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_64x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_8x16 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_8x32 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_8x64 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.g_rmd_output.modes_8x8 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x32MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x4MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x4MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x64MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x8MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block16x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x32MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x4MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x4MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x64MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x8MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block32x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block4x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block4x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block4x32MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block4x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block4x8MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block4x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x32MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x64MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x8MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block64x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x16MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x16MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x32MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x32MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x4MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x4MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x64MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x64MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x8MvDir[0][0].cur_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.me_mv_dir.block8x8MvDir[0][0].lidx core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.p_fenc_LCU_Y core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.skip_in.refp_list_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.skip_in.refp_map_mv core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.skip_in.refp_map_mv1 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.skip_in.refp_map_refi core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.skip_in.refp_map_refi1 core=RAM_1P_BRAM 
#pragma HLS resource variable=buffer.skip_in.refp_ptr1 core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.affine_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.coef core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.coef_y core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.ipf_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.ipm core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.map_cu_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.map_scu core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.mpm core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.mv core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.mvd core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.mvp_from_hmvp_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.mvr_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.num_nz_coef core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.pb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.pred_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.reco_y core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.refi core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.skip_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.split_mode core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.tb_part core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.umve_flag core=RAM_1P_BRAM 
#pragma HLS resource variable=md_output.umve_idx core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_0_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_0_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_1_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_1_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_2_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_2_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_3_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_3_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_4_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_4_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_5_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_5_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_6_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_6_1 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_7_0 core=RAM_1P_BRAM 
#pragma HLS resource variable=ref.ref_7_1 core=RAM_1P_BRAM 
}
#endif //if VMOD_DUMMY

#if PU_KERNEL_16x16_DUMMY
void pu_KERNEL_16X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_16x16& axis_in)
{

}
#endif // PU_KERNEL_16x16_DUMMY
