#include "md1.h"
#if (avs3_min(W, H) == 64)
#define ME_SIZE 64
#elif (avs3_min(W, H) == 32)
#define ME_SIZE 32
#elif (avs3_min(W, H) == 16)
#define ME_SIZE 16
#elif (avs3_min(W, H) == 8)
#define ME_SIZE 8
#elif (avs3_min(W, H) == 4)
#define ME_SIZE 4
#endif


static int FUNC_NAME(get_md_rdo_num, W, H)(int qt_depth)
{
	int max_rdo_num = 5;

	if (qt_depth == 2)
	{
#if (W == 16 && H == 16)
			max_rdo_num = 2;
#else
			max_rdo_num = 3;
#endif
	}

	return max_rdo_num;
}

void FUNC_NAME(analyze_uni_pred, W, H)(RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	int& inter_mode, s64* cost_best_temp,
	MD_INPUT* md_input_ptr, u8 num_refp_cnt, u8& curr_mvr, int cu_width, int cu_height, s16 mvp_all[2][5][2],
	s16 mv_all[2][5][2], u32 mecost_all[2][5], int best_mv_uni_inner[2][4][2], strFetch_ref_window* Fetch_Refwin_ptr,
	MD_COM_MODE* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, COM_PART_INFO* mod_info_curr_tb_info, s64 dist_pred[1], pel* org_y, int stride_org, int bit_cnt, int refi_cur, strFetch_Orig_Lcu* Orig_Lcu_ptr,
	u8& mode_type, int mode_idx, SKIP_RDO skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO merge_rdo,
	int cu_width_log2, int cu_height_log2, s16 resi_t[W * H], u8 tree_status, u8 ctx_cons_pred_mode, U1 valid_flag[1], U1 valid_mode[17],  S14 sub_pel_mv[MV_D], pel pred_cache[W * H], s16 coef_y_pb_part[W * H])
{
	U1 allowed[1] = { 1 };
	*cost_best_temp = (s64)MAX_S64;
	dist_pred[0] = 0;
	valid_flag[0] = valid_mode[mode_idx];

	if (valid_flag[0] == 1) {
		if (mode_idx < 9)
		{
			mode_type = 0;
			inter_mode = ((int)mode_type) - 1;
			curr_mvr = 0;
		}
		else if ((mode_idx >= 9) && (mode_idx <= 13))
		{
			mode_type = 1;
			inter_mode = ((int)mode_type) - 1;
			curr_mvr = mode_idx - 9;
		}
		else if ((mode_idx >= 14) && (mode_idx <= 16))
		{
			mode_type = 2;
			inter_mode = ((int)mode_type) - 1;
			curr_mvr = mode_idx - 14;
		}

		int ppb_h_uni = md_fw_ptr->pic_height >> 2;
		int ppb_w_uni = md_fw_ptr->pic_width >> 2;
		int pic_height = md_fw_ptr->pic_height_in_scu << 2;
		int pic_width = md_fw_ptr->pic_width_in_scu << 2;
		if (((int)inter_mode) == 0) {
			BOOL res = FUNC_NAME(analyze_me_uni_pred, W, H)(model_cnt,md_fw_ptr, mod_info_curr_pb_info, md_input_ptr, num_refp_cnt, curr_mvr, cu_width, cu_height,
				mvp_all, mv_all, mecost_all, best_mv_uni_inner, cost_best_temp, Fetch_Refwin_ptr, mod_info_curr_ptr, dist_pred,
				org_y, stride_org, bit_cnt, refi_cur, resi_t, tree_status, ctx_cons_pred_mode, sub_pel_mv, pred_cache, coef_y_pb_part);
			valid_flag[0] = res;
		}
		else if (((int)inter_mode) == 1)
		{
			/*for (int i = 0; i < NEB_NUMS; ++i){
				me_line_map_ptr->mv_nebs[i][0] = me_line_map_ptr->mv_nebs[i][1] = 0;
			}*/
			FUNC_NAME(analyze_affine_uni_pred, W, H)(model_cnt,md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, mod_info_curr_ptr, mod_info_curr_pb_info, mod_info_curr_tb_info, md_input_ptr, Orig_Lcu_ptr,
				Fetch_Refwin_ptr, curr_mvr, best_mv_uni_inner, cost_best_temp,
				dist_pred, allowed, resi_t, tree_status, ctx_cons_pred_mode, pred_cache, coef_y_pb_part);
			valid_flag[0] = allowed[0];
			//printf("pos(%d %d) size=(%d %d) ", mod_info_curr->x_pos, mod_info_curr->y_pos, cu_width, cu_height);
			//if (valid_flag[0]){
			//	printf("mv=(");
			//	for (int i = 0; i < mod_info_curr_ptr->affine_flag + 1; ++i){
			//		printf("(%d %d)", mod_info_curr_ptr->affine_mv[REFP_0][i][MV_X], mod_info_curr_ptr->affine_mv[REFP_0][i][MV_Y]);
			//	}
			//	printf(")");
			//}
			//printf("ime=(");
			//printf("(%d %d) ", best_mv_uni_inner[0][0][0], best_mv_uni_inner[0][0][1]);
			//printf(") neb=");
			//for (int i = 0; i < NEB_NUMS; ++i){
			//	printf("(%d %d) ", me_line_map_ptr->mv_nebs[i][0], me_line_map_ptr->mv_nebs[i][1]);
			//}
			//printf(")\n");
		}
		else if (((int)mode_type) == 0)
		{
			if (!FUNC_NAME(make_skip_resi, W, H)(mode_idx, &skip_rdo, skip_rdo_mode, &merge_rdo, mod_info_curr_ptr, mod_info_curr_pb_info, Fetch_Refwin_ptr, pic_width, pic_height,
				cu_width_log2, cu_height_log2, org_y, 64, dist_pred, cu_width, cu_height, resi_t, pred_cache)) {
				valid_flag[0] = 0;
			}
		}
	}
	else return;
}

static void FUNC_NAME(limit_inter_rdo_num, W, H)(RDOQ_MODEL* model_cnt, int max_rdo_num, int all_inter_mode_num, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	int& inter_mode, s64 cost_best_temp[17 + 1],
	MD_INPUT* md_input_ptr, u8 num_refp_cnt, u8& curr_mvr, int cu_width, int cu_height, s16 mvp_all[2][5][2],
	s16 mv_all[2][5][2], u32 mecost_all[2][5], int best_mv_uni_inner[2][4][2], strFetch_ref_window* Fetch_Refwin_ptr,
	MD_COM_MODE mod_info_curr_ptr[17 + 1], COM_PART_INFO mod_info_curr_pb_info[17 + 1], COM_PART_INFO mod_info_curr_tb_info[17 + 1], s64 dist_pred[17 + 1], pel* org_y, int stride_org, int bit_cnt, int refi_cur, strFetch_Orig_Lcu* Orig_Lcu_ptr,
	u8& mode_type, SKIP_RDO skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO merge_rdo,
	int cu_width_log2, int cu_height_log2, s16 resi_t[17 + 1][W * H], u8 tree_status, u8 ctx_cons_pred_mode, U1 valid_flag[1], U1 valid_mode[17], S14 sub_pel_mv[MV_D], pel pred_cache[17 + 1][W * H], s16 coef_y_pb_part[W * H])
{
	s64 dist_pred_all[30] = { 0 };
	for (int mode_idx = 0; mode_idx < all_inter_mode_num; mode_idx++) {
#pragma HLS LOOP_TRIPCOUNT max=17
		FUNC_NAME(analyze_uni_pred, W, H)(model_cnt, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, inter_mode, &cost_best_temp[1 + mode_idx], md_input_ptr, num_refp_cnt, curr_mvr,
			cu_width, cu_height, mvp_all, mv_all, mecost_all, best_mv_uni_inner, Fetch_Refwin_ptr, &mod_info_curr_ptr[0], &mod_info_curr_pb_info[0], &mod_info_curr_tb_info[0],
			&dist_pred[1 + mode_idx], org_y, stride_org, bit_cnt, (int)refi_cur, Orig_Lcu_ptr,
			mode_type, mode_idx, skip_rdo, skip_rdo_mode, merge_rdo, cu_width_log2, cu_height_log2, resi_t[1 + mode_idx], tree_status, ctx_cons_pred_mode, valid_flag, valid_mode, sub_pel_mv, pred_cache[1 + mode_idx], coef_y_pb_part);

		if (valid_flag[0])
		{
			copy_md_com_mode_info(&mod_info_curr_ptr[0], &mod_info_curr_ptr[1 + mode_idx]);
			copy_md_com_part_info(&mod_info_curr_pb_info[0], &mod_info_curr_pb_info[1 + mode_idx]);
			copy_md_com_part_info(&mod_info_curr_tb_info[0], &mod_info_curr_tb_info[1 + mode_idx]);
		}

		if (valid_flag[0] == 1)
		{
			dist_pred_all[mode_idx] = dist_pred[0];
			valid_mode[mode_idx] = 1;

		}
		else
		{
			dist_pred_all[mode_idx] = MAX_S64;
			valid_mode[mode_idx] = 0;
		}
	}
	//printf("valid_mode=(");
	//for (int i = 0; i < all_inter_mode_num; ++i){
	//	printf("%d ", valid_mode[i]);
	//}
	//printf(")\n");
	//printf("dist_pred_all=(");
	//for (int i = 0; i < all_inter_mode_num; ++i){
	//	print_s64(dist_pred_all[i]);
	//	printf(" ");
	//}
	//printf(")\n");

	choose_inter_rdo_mode(max_rdo_num, all_inter_mode_num, dist_pred_all, valid_mode);
}

static void FUNC_NAME(get_IP_pixel, W, H)(u16 avail_cu, int pic_width_in_scu, int pic_height_in_scu,
	int bit_depth, int ch_type, U8 left[2 * H + 3], U8 up[2 * W + 3], int x, int y, ENC_ME_LINE_MAP *me_line_ptr, IP_ARRAY *IP_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr)
{
	int i;
	int j;
	int width_in_scu = ch_type == 0 ? W >> 2 : W >> (2 - 1);
	int height_in_scu = ch_type == 0 ? H >> 2 : H >> (2 - 1);
	//4
	int unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
	//x/4
	int x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
	//y/4
	int y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
	//number of padding pixel in the left column   64
	int pad_le = H;
	//number of padding pixel in the upper row     64
	int pad_up = W;
	//16
	int pad_le_in_scu = height_in_scu;
	//16
	int pad_up_in_scu = width_in_scu;
	int pic_x = x % 64;
	int pic_y = y % 64;
	int pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
	int pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
	int flag;
	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		if (pic_y == 0) {
			for (i = 0; i < W; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				up[i + 3] = FUNC_NAME(IP_buffer_ptr->up_temp, W, H)[pic_x + i];
			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (x_scu + width_in_scu + i < pic_width_in_scu) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu + width_in_scu + i] >> 31 & ((
						unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[W + (i << unit_size_bit) + j + 3] = FUNC_NAME(IP_buffer_ptr->up_temp, W, H)[pic_x + W + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[W + (i << unit_size_bit) + j + 3] = up[W + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < W; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				up[i + 3] = FUNC_NAME(IP_buffer_ptr->up_innerLB, W, H)[(pic_y >> 2) - 1][pic_x + i];
			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_x_scu + width_in_scu + i < 16) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1]
						>> 31 & ((unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[W + (i << unit_size_bit) + j + 3] = FUNC_NAME(IP_buffer_ptr->up_innerLB, W, H)[(pic_y >> 2) - 1][pic_x + W + (i << unit_size_bit) +
							j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						up[W + (i << unit_size_bit) + j + 3] = up[W + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0) {
			for (i = 0; i < H; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = FUNC_NAME(IP_buffer_ptr->left, W, H)[pic_y + i];
			}
			for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31
						& ((unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[H + (i << unit_size_bit) + j + 3] = FUNC_NAME(IP_buffer_ptr->left, W, H)[pic_y + H + (i << unit_size_bit) + j];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[H + (i << unit_size_bit) + j + 3] = left[H + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < H; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = FUNC_NAME(IP_buffer_ptr->left_innerLB, W, H)[pic_y + i][(pic_x >> 2) - 1];
			}
			for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16) {
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i]
						>> 31 & ((unsigned int)1)));
				}
				else {
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[H + (i << unit_size_bit) + j + 3] = FUNC_NAME(IP_buffer_ptr->left_innerLB, W, H)[H + (i << unit_size_bit) + j + pic_y][(pic_x >>
							2) - 1];
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						left[H + (i << unit_size_bit) + j + 3] = left[H + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
		if (pic_y == 0 && pic_x == 0) {
			up[2] = left[2] = ((pel)(IP_buffer_ptr->top_left));
		}
		else if (pic_y == 0) {
			up[2] = left[2] = FUNC_NAME(IP_buffer_ptr->up_temp, W, H)[pic_x - 1];
		}
		else if (pic_x == 0) {
			up[2] = left[2] = FUNC_NAME(IP_buffer_ptr->left, W, H)[pic_y - 1];
		}
		else {
			up[2] = left[2] = FUNC_NAME(IP_buffer_ptr->up_innerLB, W, H)[(pic_y >> 2) - 1][pic_x - 1];
		}
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
	}
	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}


void FUNC_NAME(prepare_intra_info, W, H)(MD_FW*md_fw_ptr, ENC_ME_LINE_MAP *me_line_map_ptr, MD_COM_MODE* mod_info_curr_ptr, IP_ARRAY *IP_buffer_ptr, MPM_ARRAY *MPM_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, int cu_width_log2, int cu_height_log2, int pix_x, int pix_y, int sub_x, int sub_y, u8 ipred_list[33], int *pred_cnt, U8 src_le_temp[2 * H + 3], U8 src_up_temp[2 * W + 3], u16 *avail_tb, RMD_OUTPUT *g_rmd_output)
{
	int pb_x_scu = mod_info_curr_ptr->x_pos >> 2;
	int pb_y_scu = mod_info_curr_ptr->y_pos >> 2;
	int tb_x_scu;
	int tb_y_scu;
	int i;
	com_get_mpm(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr, me_line_map_ptr);
	*pred_cnt = get_rmd_result(cu_width_log2, cu_height_log2, pix_x, pix_y, ipred_list, g_rmd_output);

	
	tb_x_scu = sub_x >> 2;
	tb_y_scu = sub_y >> 2;
	*avail_tb = com_get_avail_intra_ip(MAP_SCU_IP_ptr, tb_x_scu, tb_y_scu);
	
	for (i = 0; i < (2 * H + 3); i++) {
		src_le_temp[i] = (1 << (8 - 1));
	}
	for (i = 0; i < (2 * W + 3); i++) {
		src_up_temp[i] = (1 << (8 - 1));
	}
	FUNC_NAME(get_IP_pixel, W, H)( *avail_tb, md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu, 8, 0, src_le_temp, src_up_temp, sub_x, sub_y, me_line_map_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr);
}

void FUNC_NAME(rdo_prepare, W, H)(RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, MD_COM_MODE mod_info_curr_ptr[17 + 1],
	SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_INPUT* md_input_ptr,
	u32 lambda_mv, s8 refi_cur, u8 num_refp_cnt, U9 ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2], s16 mv_all[2][5][2],
	u32 mecost_all[2][5], U1 valid_mode[17], U1 amvr_enable_flag, 
	ENC_FME_INFO_ALL *fme_mv_info,
	IP_ARRAY *IP_buffer_ptr, MPM_ARRAY *MPM_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, U3 cu_width_log2, U3 cu_height_log2,
	u8 ipred_list[33], int *pred_cnt, U8 src_le_temp[2 * H + 3], U8 src_up_temp[2 * W + 3], u16 *avail_tb, RMD_OUTPUT *g_rmd_output,
	U3 qt_depth, int all_inter_mode_num,
	int &inter_mode, s64 cost_best_temp[17 + 1],
	u8 &curr_mvr, U7 cu_width, U7 cu_height,
	int best_mv_uni_inner[2][4][2], strFetch_ref_window *Fetch_Refwin_ptr,
	COM_PART_INFO mod_info_curr_pb_info[17 + 1], COM_PART_INFO mod_info_curr_tb_info[17 + 1], s64 dist_pred[17 + 1], pel *org_y, U7 stride_org, int bit_cnt, strFetch_Orig_Lcu *Orig_Lcu_ptr,
	u8 &mode_type,
	s16 resi_t[17 + 1][W * H], u8 tree_status, u8 curr_cons_pred_mode, U1 valid_flag[1],
	S14 sub_pel_mv[MV_D], pel pred_cache[17 + 1][W * H], s16 coef_y_pb_part[W * H],
	u8 inter_num[1], u8 intra_num[1], u8 *rdo_list)
{
	int max_rdo_num = FUNC_NAME(get_md_rdo_num, W, H)(qt_depth);
	int max_inter = 0;

	U14 sub_x = mod_info_curr_pb_info[0].sub_x[0];
	U14 sub_y = mod_info_curr_pb_info[0].sub_y[0];

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;

	int num_amvr;
	if (amvr_enable_flag) {
		num_amvr = 5;
	}
	else {
		num_amvr = 1;
	}

	inter_num[0] = 0;
	intra_num[0] = 0;
	//printf("pos=(%d %d) size=(%d %d) slice=%d cons_pred_mode=%d\n", mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, cu_width, cu_height, md_fw_ptr->slice_type, curr_cons_pred_mode);
	if (!(md_fw_ptr->slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA)) {
		max_inter = (curr_cons_pred_mode == ONLY_INTER) ? max_rdo_num : (max_rdo_num + 1) / 2;
		FUNC_NAME(prepare_luma_inter, W, H)(md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, skip_in, &mod_info_curr_ptr[0],
			skip_rdo, skip_rdo_mode, merge_rdo, Fetch_Refwin_ptr->CtrPos, md_input_ptr,
			num_amvr, md_input_ptr->lambda_mv, refi_cur, num_refp_cnt, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mvp_all, mv_all, mecost_all, valid_mode, amvr_enable_flag, fme_mv_info, Fetch_Refwin_ptr, Orig_Lcu_ptr);

		FUNC_NAME(limit_inter_rdo_num, W, H)(model_cnt, max_inter, all_inter_mode_num, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, inter_mode, cost_best_temp, md_input_ptr, num_refp_cnt, curr_mvr,
			cu_width, cu_height, mvp_all, mv_all, mecost_all, best_mv_uni_inner, Fetch_Refwin_ptr, mod_info_curr_ptr, mod_info_curr_pb_info, mod_info_curr_tb_info,
			dist_pred, org_y, stride_org, bit_cnt, refi_cur, Orig_Lcu_ptr,
			mode_type, *skip_rdo, skip_rdo_mode, *merge_rdo, cu_width_log2, cu_height_log2, resi_t, tree_status, curr_cons_pred_mode, valid_flag, valid_mode,  sub_pel_mv, pred_cache, coef_y_pb_part);

		for (int mode_index = 0; mode_index < all_inter_mode_num; mode_index++)
		{
#pragma HLS loop_tripcount max = 17
			if (valid_mode[mode_index] == 1)
			{
				rdo_list[inter_num[0]] = mode_index;
				inter_num[0]++;
			}
		}
	}


	if (!(md_fw_ptr->slice_type == 2 && curr_cons_pred_mode == ONLY_INTER))
	{
		FUNC_NAME(prepare_intra_info, W, H)(md_fw_ptr, me_line_map_ptr, &mod_info_curr_ptr[0], IP_buffer_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr, cu_width_log2, cu_height_log2, pix_x, pix_y, sub_x, sub_y, ipred_list, pred_cnt, src_le_temp, src_up_temp, avail_tb, g_rmd_output);
		intra_num[0] = avs3_min(*pred_cnt, max_rdo_num - inter_num[0]);
		for (int intra_index = 0; intra_index < intra_num[0]; intra_index++)
		{
			rdo_list[inter_num[0] + intra_index] = all_inter_mode_num + intra_index;
		}
	}
}

s64 FUNC_NAME(rdo_luma, W, H)(U1 isNS_allow, U8 w, U8 h, RDOQ_MODEL* model_cnt, int rdoq_est_cbf[NUM_QT_CBF_CTX][2], s32 rdoq_est_level[NUM_SBAC_CTX_LEVEL][2],
	s32 rdoq_est_last[2][NUM_SBAC_CTX_LAST1][NUM_SBAC_CTX_LAST2][2], s32 rdoq_est_run[24][2], MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], SKIP_MERGE_INPUT* skip_in,
	U2 tree_status, MD_COM_MODE* best_info_ptr, MD_INPUT* md_input_ptr, strFetch_Orig_Lcu* Orig_Lcu_ptr, strFetch_ref_window* Fetch_Refwin_ptr,
	MD_COM_MODE mod_info_curr_ptr[17 + 1], COM_PART_INFO mod_info_curr_pb_info[17 + 1], COM_PART_INFO mod_info_curr_tb_info[17 + 1],
	COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U2 curr_cons_pred_mode, U9 cup, U3 qt_depth,
	s16 me_info_addr_map_split[MAX_CU_DEPTH * MAX_CU_DEPTH][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_FME_INFO_ALL* fme_mv_info, s8 coef_rec_write_flag[1],
	RMD_OUTPUT* g_rmd_output, IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, S14 best_affine_mv[2][4][2])
{
	if (!isNS_allow)
		return 0;

	U4 bit_depth = 8;
	s64 cost_best = MAX_S64;
	U3 cu_width_log2 = mod_info_curr_ptr[0].cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr[0].cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	U1 emvr_enable_flag = 1;
	U1 amvr_enable_flag = 1;
	U1 affine_enable_flag = 1;
	U1 run_direct_skip = 1;

	int num_iter_mvp = 2;
	int num_hmvp_inter = 5;

	U14 sub_x = mod_info_curr_pb_info[0].sub_x[0];
	U14 sub_y = mod_info_curr_pb_info[0].sub_y[0];

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;

#if W * H >= 64
	run_direct_skip = 1;
#else
	run_direct_skip = 0;
#endif

	u8 mode_type = 0;
	int inter_mode = 0;
	u8 curr_mvr = 0;

	static s16 coef_tmp[17 + 1][W * H];
	static s16 coef_tmp_h[W * H];
	static s16 coef_tmp_v[W * H];
	static pel rec_y_pb_part[W * H];
	static pel pred_cache[17 + 1][W * H];
	static s16 coef_y_pb_part[W * H];
	int best_mv_uni_inner[2][4][2];
	u32 mecost_all[2][5];
	s16 mvp_all[2][5][2];
	s16 mv_all[2][5][2];
	s8 refi_cur = 0;

	//int i = 0;
	int j = 0;
	int k = 0;
	U1 valid_mode[17];   //{SKIP:0,1,2,3;Skip_Merge:4,5,6,7,8;Mode1:9,10,11,12,13;Mode2:14,15,16};    //{Mode 0:(skip mode:fix 4, skip_merger:0~5), Mode1:(ME:AMVR:FIX 1/5); Mode 2(AFF_ME::FIX 1/3)
	U1 valid_flag[1] = { 0 };
	int bit_cnt = 0;
	int stride_org = 64;
	s32 dist = 0;
	u8 num_refp_cnt = md_fw_ptr->num_refp;//(u8 )ctx->rpm.num_refp[0];
	SKIP_RDO skip_rdo;
	SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO];
	MERGE_RDO merge_rdo;
	pel* orgY = Orig_Lcu_ptr->p_fenc_LCU_Y + (mod_info_curr_ptr[0].x_pos - md_input_ptr->pix_x) +
		(mod_info_curr_ptr[0].y_pos - md_input_ptr->pix_y) * 64;
	s64 cost_best_temp[17 + 1] = { MAX_S64 };
	s64 dist_pred[17 + 1] = { 0 };
	s64 lambda_y = md_input_ptr->lambda[0];

	int best_ipd_c[1] = { (-1) };

	s64 best_dist_c[1] = { 0 };
	u8 ipred_list[33];
	int pred_cnt = 33;
	int best_ipd_pb_part[4] = { (-1), (-1), (-1), (-1) };
	s32 best_dist_y_pb_part[4] = { 0, 0, 0, 0 };

	/*int num_nz_y_pb_part[1] = { 0 };*/
	int num_nz_uv_pb_part[2] = { 0 };
	int coef_offset_tb = 0;
	U8 src_le_temp[2 * H + 3];
	U8 src_up_temp[2 * W + 3];
	//pel src_le_temp[128 + 3];
	//pel src_up_temp[128 + 3];
	u16 avail_tb;
	s64 cost_pb_best = MAX_S64;

	// idx for sub pel mv.
	U13 x_pos = mod_info_curr_ptr[0].x_pos;
	U13 y_pos = mod_info_curr_ptr[0].y_pos;
	//int info_idx;
//	int info_idx_spl;
	U11 x0_scu;
	U11 y0_scu;
	x0_scu = PEL2SCU(x_pos - md_input_ptr->pix_x);
	y0_scu = PEL2SCU(y_pos - md_input_ptr->pix_y);

	//info_idx_spl = me_info_addr_map_split[(cu_width_log2 - 2) + (MAX_CU_DEPTH * (cu_height_log2 - 2))][x0_scu][y0_scu];


#define sub_pel_mv fme_mv_info->FME_NAME(sadTree, Fmv, W , H)[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2]


	u8 rdo_list[22] = {}, inter_rdo_num[1] = { 0 }, intra_rdo_num[1] = { 0 };
	FUNC_NAME(rdo_prepare, W, H)( model_cnt, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, skip_in, mod_info_curr_ptr,
		&skip_rdo, skip_rdo_mode, &merge_rdo, Fetch_Refwin_ptr->CtrPos, md_input_ptr,
		md_input_ptr->lambda_mv, refi_cur, num_refp_cnt, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mvp_all, mv_all, mecost_all, valid_mode, amvr_enable_flag, me_info_addr_map_split,fme_mv_info,
		IP_buffer_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr, cu_width_log2, cu_height_log2, ipred_list, &pred_cnt, src_le_temp, src_up_temp, &avail_tb, g_rmd_output,
		qt_depth, 17, inter_mode, cost_best_temp, curr_mvr,
		cu_width, cu_height, best_mv_uni_inner, Fetch_Refwin_ptr, mod_info_curr_pb_info, mod_info_curr_tb_info,
		dist_pred, orgY, stride_org, bit_cnt, Orig_Lcu_ptr,
		mode_type, coef_tmp, tree_status, curr_cons_pred_mode, valid_flag, sub_pel_mv, pred_cache, coef_y_pb_part,
		inter_rdo_num, intra_rdo_num, rdo_list);


	u8 all_rdo_num = inter_rdo_num[0] + intra_rdo_num[0];

	s8 inter_mode_idx, intra_mode_idx;
	u8 rdo_idx;
	U1 is_intra;

	for (rdo_idx = 0; rdo_idx < all_rdo_num; rdo_idx++) {
#pragma HLS LOOP_TRIPCOUNT max=5

		FUNC_NAME(intra_pred, W, H)(curr_mvr, &mod_info_curr_ptr[0], orgY, mode_type, inter_mode_idx, intra_mode_idx, coef_tmp[0], valid_flag, valid_mode, pred_cache[0],
			is_intra, ipred_list, src_le_temp, src_up_temp, avail_tb, rdo_list, rdo_idx, inter_rdo_num[0], coef_rec_write_flag);

		FUNC_NAME(transform_hor, W, H)(8, mod_info_curr_ptr[1 + inter_mode_idx].num_nz, coef_tmp[1 + inter_mode_idx], coef_tmp_h, 1 << cu_width_log2, 1 << cu_height_log2, md_fw_ptr->slice_type, 0, tree_status);
		FUNC_NAME(transform_ver, W, H)(md_fw_ptr->slice_type, 8, md_input_ptr->qp_y, mod_info_curr_ptr[1 + inter_mode_idx].num_nz, md_input_ptr->lambda, coef_tmp_h, coef_tmp_v, coef_y_pb_part, 1 << cu_width_log2, 1 << cu_height_log2, 0, is_intra, tree_status);
		mod_info_curr_ptr[1 + inter_mode_idx].num_nz[0][0] = FUNC_NAME(quant_nnz, W, H)(model_cnt, rdoq_est_cbf, rdoq_est_level, rdoq_est_last, rdoq_est_run, bit_depth, md_input_ptr->qp_y, md_input_ptr->lambda[0], is_intra, coef_tmp_v, cu_width_log2, cu_height_log2, 0, md_fw_ptr->slice_type, coef_y_pb_part, mod_info_curr_ptr[0].coef_u, mod_info_curr_ptr[0].coef_v);
		FUNC_NAME(inv_transform_ver, W, H)(8, md_input_ptr->qp_y, &mod_info_curr_ptr[1 + inter_mode_idx], coef_tmp_v, cu_width_log2, cu_height_log2, 1 << cu_width_log2, 1 << cu_height_log2, 0);
		FUNC_NAME(inv_transform_hor, W, H)(md_fw_ptr, 8, &mod_info_curr_ptr[1 + inter_mode_idx], &mod_info_curr_pb_info[1 + inter_mode_idx], cu_width_log2, cu_height_log2, pred_cache[1 + inter_mode_idx], &dist, rec_y_pb_part, sub_x, sub_y, 1 << cu_width_log2, 1 << cu_height_log2, 0, coef_tmp_v, 0, 0, orgY);
		FUNC_NAME(update_rdo_luma, W, H)(model_cnt, md_fw_ptr, cu_data_temp_ptr, &mod_info_curr_ptr[1 + inter_mode_idx], mode_type, &dist_pred[1 + inter_mode_idx], &cost_best_temp[1 + inter_mode_idx], best_info_ptr, &mod_info_curr_pb_info[1 + inter_mode_idx],
			&mod_info_curr_tb_info[1 + inter_mode_idx], bst_info_pb_info, bst_info_tb_info, md_input_ptr, &cost_best, curr_mvr, tree_status, curr_cons_pred_mode, coef_rec_write_flag,
			rec_y_pb_part, pred_cache[1 + inter_mode_idx], coef_y_pb_part, &dist, lambda_y, is_intra, me_line_map_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr, 1 << cu_width_log2, 1 << cu_height_log2,
			&cost_pb_best);
	}

	if (best_info_ptr->cu_mode != MODE_INTRA)
	{
		coef_rec_write_flag[0] = 1;
		if (tree_status == TREE_LC){
			for (int ver = 0; ver < 4; ver++) {
				best_affine_mv[0][ver][0] = best_info_ptr->affine_mv[0][ver][0];
				best_affine_mv[0][ver][1] = best_info_ptr->affine_mv[0][ver][1];
				best_affine_mv[1][ver][0] = best_info_ptr->affine_mv[1][ver][0];
				best_affine_mv[1][ver][1] = best_info_ptr->affine_mv[1][ver][1];
			}
		}
	}

	//cost_best_intra[0] = cost_best;
#undef sub_pel_mv

	return cost_best;
}

void FUNC_NAME(copy_to_cu_data_luma, W, H)(U1 isNS_allow, U6 qp_y, ENC_CU_DATA_ARRAY *cu_data, MD_COM_MODE *mod_info_best, U2 tree_status,
	int slice_num, int affine_subblock_size_idx, s8 coef_rec_write_flag[1], U2 NS_luma_pred_mode[1])
{
	if (!isNS_allow)
	{
		NS_luma_pred_mode[0] = ONLY_INTRA;
		return;
	}
	int i;
	int j;
	int k;
	int idx;
	U3 cu_width_log2;
	U3 cu_height_log2;
	int num_coef_y = 0;
	int num_coef_u = 0;
	int num_coef_v = 0;
	int num_nnz_sum = 0;
	int num_luma_tb;
	int cu_cbf_flag = 0;
	int pb_idx_u = 0;
	int tb_idx_u = 0;
	int pb_idx_y;
	int tb_idx_y;
	cu_width_log2 = mod_info_best->cu_width_log2;
	cu_height_log2 = mod_info_best->cu_height_log2;
	if (tree_status != TREE_C){
		if (coef_rec_write_flag[0] == 0){
			for (i = 0; i < (W * H); i++){
				cu_data->coef_y[i] = mod_info_best->coef_y[i];
				cu_data->reco_y[i] = mod_info_best->rec_y[i];
			}
		}
		num_luma_tb = get_part_num((PART_SIZE)(mod_info_best->tb_part));
		for (i = 0; i < W * H; i++) {
			num_coef_y += (((int)cu_data->coef_y[i]) != 0 ? 1 : 0);
		}
		for (i = 0; i < num_luma_tb; i++) {
			num_nnz_sum += mod_info_best->num_nz[i][0];
		}
	}
	if (tree_status == TREE_LC) {
		cu_cbf_flag = ((num_coef_y == 0 ? 0 : 1)) | ((num_coef_u == 0 ? 0 : 1)) | ((num_coef_v == 0 ? 0 : 1));
	}
	else if (((int)tree_status) == ((int)TREE_L)) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	if (((int)tree_status) == ((int)TREE_C)) {
		idx = 0;
		for (j = 0; j < H >> 2; j++) {
			for (i = 0; i < W >> 2; i++) {
				if (mod_info_best->cu_mode == 0) {
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
				}
			}
			idx += W >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < H >> 2; j++) {
			for (i = 0; i < W >> 2; i++) {
				pb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->pb_part), i << 2, j << 2, W, H);
				tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, W, H);
				pb_idx_u = 0;
				tb_idx_u = 0;
				cu_data->pred_mode[idx + i] = ((u8)(mod_info_best->cu_mode));
				cu_data->umve_flag[idx + i] = mod_info_best->umve_flag;
				cu_data->umve_idx[idx + i] = mod_info_best->umve_idx;
				cu_data->pb_part[idx + i] = mod_info_best->pb_part;
				cu_data->tb_part[idx + i] = mod_info_best->tb_part;
				for (k = 0; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
				}
				cu_data->map_scu[idx + i] = (cu_data->map_scu[idx + i] & 0xFF807F80) | ((unsigned int)(slice_num & 0x7F)) | ((
					unsigned int)(qp_y << 16)) | ((unsigned int)((mod_info_best->cu_mode == 0) << 15)) | ((unsigned int)(1 << 31));
				if (mod_info_best->cu_mode == 2) {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((unsigned int)(1 << 23));
				}
				else {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((unsigned int)(~(1 << 23)));
				}
				cu_data->depth[idx + i] = ((s8)(mod_info_best->cud));
				cu_data->affine_flag[idx + i] = mod_info_best->affine_flag;
				if (mod_info_best->affine_flag) {
					cu_data->map_scu[idx + i] = (cu_data->map_scu[idx + i] & 0xFFFFFCFF) | ((unsigned int)((((int)(
						mod_info_best->affine_flag)) & 0x03) << 8));
				}
				else {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & 0xFFFFFCFF;
				}
				if (cu_cbf_flag) {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((unsigned int)(1 << 24));
				}
				else {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((unsigned int)(~(1 << 24)));
				}
				cu_data->map_cu_mode[idx + i] = (cu_data->map_cu_mode[idx + i] & 0xFFFF00FF) | ((unsigned int)((i & 0xFF) << 8));
				cu_data->map_cu_mode[idx + i] = (cu_data->map_cu_mode[idx + i] & 0xFF00FFFF) | ((unsigned int)((j & 0xFF) << 16));
				cu_data->map_cu_mode[idx + i] = (cu_data->map_cu_mode[idx + i] & 0xF0FFFFFF) | ((unsigned int)((
					cu_width_log2 & 0x0F) << 24));
				cu_data->map_cu_mode[idx + i] = (cu_data->map_cu_mode[idx + i] & 0x0FFFFFFF) | ((unsigned int)((
					cu_height_log2 & 0x0F) << 28));
				cu_data->map_pb_tb_part[idx + i] = (cu_data->map_pb_tb_part[idx + i] & 0xFFFFFF00) | ((unsigned int)((
					mod_info_best->tb_part & 0xFF) << 0));
				if (mod_info_best->cu_mode == 0) {
					cu_data->mpm[0][idx + i] = mod_info_best->mpm[pb_idx_y][0];
					cu_data->mpm[1][idx + i] = mod_info_best->mpm[pb_idx_y][1];
					cu_data->ipm[0][idx + i] = mod_info_best->ipm[pb_idx_y][0];
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[pb_idx_u][1];
					cu_data->ipf_flag[idx + i] = mod_info_best->ipf_flag;
					cu_data->mv[idx + i][0][0] = ((s16)0);
					cu_data->mv[idx + i][0][1] = ((s16)0);
					cu_data->mv[idx + i][1][0] = ((s16)0);
					cu_data->mv[idx + i][1][1] = ((s16)0);
					cu_data->refi[idx + i][0] = ((s8)(-1));
					cu_data->refi[idx + i][1] = ((s8)(-1));
				}
				else {
					cu_data->refi[idx + i][0] = mod_info_best->refi[0];
					cu_data->refi[idx + i][1] = mod_info_best->refi[1];
					cu_data->mvr_idx[idx + i] = mod_info_best->mvr_idx;
					cu_data->mvp_from_hmvp_flag[idx + i] = mod_info_best->mvp_from_hmvp_flag;
					cu_data->smvd_flag[idx + i] = mod_info_best->smvd_flag;
					cu_data->skip_idx[idx + i] = mod_info_best->skip_idx;
					cu_data->mv[idx + i][0][0] = mod_info_best->mv[0][0];
					cu_data->mv[idx + i][0][1] = mod_info_best->mv[0][1];
					cu_data->mv[idx + i][1][0] = mod_info_best->mv[1][0];
					cu_data->mv[idx + i][1][1] = mod_info_best->mv[1][1];
					cu_data->mvd[idx + i][0][0] = mod_info_best->mvd[0][0];
					cu_data->mvd[idx + i][0][1] = mod_info_best->mvd[0][1];
					cu_data->mvd[idx + i][1][0] = mod_info_best->mvd[1][0];
					cu_data->mvd[idx + i][1][1] = mod_info_best->mvd[1][1];
				}
			}
			idx += W >> 2;
		}
		if (mod_info_best->affine_flag) {
			enc_set_affine_mvf(affine_subblock_size_idx, cu_data, mod_info_best);
		}
	}

	NS_luma_pred_mode[0] = cu_data->pred_mode[0];
}

s64 FUNC_NAME(md_kernel, W, H)(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW*md_fw_ptr, MD_INPUT *md_input_ptr, RMD_OUTPUT *g_rmd_output, strFetch_ref_window *Fetch_Ref_window_ptr, strFetch_Orig_Lcu *Fetch_Orig_Lcu_ptr, ENC_ME_LINE_MAP *me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT *skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status, ENC_CU_DATA_ARRAY *cu_data_temp_ptr, 
	ENC_FME_INFO_ALL *fme_mv_info,
	IP_ARRAY *IP_buffer_ptr, MPM_ARRAY *MPM_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, S14 best_affine_mv[2][4][2])
{
	s64 ns_cost = (s64)MAX_S64;

	MD_COM_MODE mod_info_curr_temp[18];
	MD_COM_MODE mod_info_best;
	COM_PART_INFO mod_info_curr_pb_info[18];
	COM_PART_INFO mod_info_curr_tb_info[18];
	COM_PART_INFO bst_info_pb_info;
	COM_PART_INFO bst_info_tb_info;

	s8 coef_rec_write_flag[1] = { 0 };

	s32 rdoq_est_run[NUM_SBAC_CTX_RUN][2] = { 0 };
	int rdoq_est_cbf[NUM_QT_CBF_CTX][2] = { 0 };
	s32 rdoq_est_level[NUM_SBAC_CTX_LEVEL][2] = { 0 };
	s32 rdoq_est_last[2][NUM_SBAC_CTX_LAST1][NUM_SBAC_CTX_LAST2][2] = { 0 };

	if (!boundary) {
		ns_cost = init_basic_leaf(isNS_allow, rdoq_model, model_cnt, rdoq_est_cbf, rdoq_est_level, rdoq_est_last, rdoq_est_run, md_fw_ptr, md_input_ptr->lambda[0], cu_width_log2, cu_height_log2, cud, &mod_info_best, qt_depth, bet_depth, x0, y0, cup, tree_status, &cons_pred_mode,
			&mod_info_curr_temp[0], &mod_info_curr_pb_info[0], &mod_info_curr_tb_info[0], &bst_info_pb_info, &bst_info_tb_info, cu_data_temp_ptr, Fetch_Ref_window_ptr);
		ns_cost += FUNC_NAME(rdo_luma, W, H)(isNS_allow, W, H, model_cnt, rdoq_est_cbf, rdoq_est_level, rdoq_est_last, rdoq_est_run, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, skip_in, tree_status, &mod_info_best, md_input_ptr, Fetch_Orig_Lcu_ptr,
			Fetch_Ref_window_ptr, mod_info_curr_temp, mod_info_curr_pb_info, mod_info_curr_tb_info, &bst_info_pb_info, &bst_info_tb_info, cu_data_temp_ptr, cons_pred_mode, cup, qt_depth, fme_mv_info, coef_rec_write_flag,
			g_rmd_output, IP_buffer_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr, best_affine_mv);
		FUNC_NAME(copy_to_cu_data_luma, W, H)(isNS_allow, md_input_ptr->qp_y, cu_data_temp_ptr, &mod_info_best, tree_status, 0, 0, coef_rec_write_flag, NS_luma_pred_mode);
	}
	//printf("pos=(%d %d) size=(%d %d) cost=%lld\n", x0, y0, 1 << cu_width_log2, 1 << cu_height_log2, ns_cost);
	return ns_cost;
}


#undef W
#undef H
#undef ME_SIZE
