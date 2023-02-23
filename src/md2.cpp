#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md2.h"
#include "md1.h"
#include "md_kernel_8.h"
#include "md_kernel_16.h"
#include "md_kernel_32.h"
#include "md_kernel_64.h"
#include "md_dummy.h"

#include "iime.h"
#pragma warning(disable:6262)

#if Debug_TWO_PASS
extern FILE* md_two_pass;
#endif // Debug_TWO_PASS

static const s8 com_tbl_log2[257] = {
	/* 0, 1 */
	-1, -1,
	/* 2, 3 */
	1, -1,
	/* 4 ~ 7 */
	2, -1, -1, -1,
	/* 8 ~ 15 */
	3, -1, -1, -1, -1, -1, -1, -1,
	/* 16 ~ 31 */
	4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 31 ~ 63 */
	5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 64 ~ 127 */
	6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 128 ~ 255 */
	7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 256 */
	8
};


extern void pinter_residue_rdo_chroma(MD_FW* md_fw_ptr, strFetch_ref_window* Fetch_Ref_window_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_CU_DATA_ARRAY* cu_data_temp_ptr, u8 tree_status, MD_COM_MODE* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info,
	MD_COM_MODE* mod_info_best_ptr, MD_INPUT* md_input_ptr, strFetch_Orig_Lcu* Fetch_Orig_Lcu, s8 coef_rec_write_flag[1]);


void md2_com_check_split_mode_top(U14 x0, U14 y0, U8 cu_width, U8 cu_height, U13 pic_width, U13 pic_height, U2 split_mode, U1* do_split) {
	U1 boundary = !(x0 + cu_width <= pic_width && y0 + cu_height <= pic_height);
	U1 boundary_r = boundary && x0 + cu_width > pic_width && !(y0 + cu_height > pic_height);
	U1 boundary_b = boundary && y0 + cu_height > pic_height && !(x0 + cu_width > pic_width);

	if (boundary) {
		if (split_mode == NO_SPLIT)
			*do_split = 0;
		else if (split_mode == SPLIT_BI_VER)
			*do_split = boundary_r ? 1 : 0;
		else if (split_mode == SPLIT_BI_HOR)
			*do_split = boundary_b ? 1 : 0;
	}
}

#if USE_SPEED_LEVEL
int get_rmd_result_new(const RMD_OUTPUT* rmd_output, int cu_width_log2, int cu_heght_log2, int x, int y, u8 ipred_list[], U8 speed_level)
#else
int get_rmd_result_new(const RMD_OUTPUT* rmd_output, int cu_width_log2, int cu_heght_log2, int x, int y, u8 ipred_list[])
#endif
{
	int i;
	int w = cu_width_log2 - 2;
	int h = cu_heght_log2 - 2;
	int x0 = (x % 64) >> 2;
	int y0 = (y % 64) >> 2;
	int idx;
	//int cnt = ((((cu_width_log2 - cu_heght_log2) >= 2) || ((cu_heght_log2 - cu_width_log2) >= 2)) ? (IPD_RDO_CNT - 1) : IPD_RDO_CNT);

#if USE_SPEED_LEVEL
	int cnt = IPD_RDO_CNT;
	if (speed_level == 1)
		cnt = IPD_RDO_CNT; //5
	if (speed_level == 2)
		cnt = EXTRA_RMD_MODE;//10
	else
		cnt = IPD_RDO_CNT + EXTRA_RMD_MODE;//15
#else
	int cnt = IPD_RDO_CNT;
#endif

	/*if (w == 1 && h == 0) {
		idx = x0*8 + y0;
		for (i = 0; i < IPD_RDO_CNT; i++) {
			ipred_list[i] = ((int)rmd_output->modes_8x4[idx][i]);
		}
	}
	else if (w == 2 && h == 0) {
		idx = x0*4 + y0;
		for (i = 0; i < IPD_RDO_CNT; i++) {
			ipred_list[i] = ((int)rmd_output->modes_16x4[idx][i]);
		}
	}
	else if (w == 3 && h == 0) {
		idx = x0*2 + y0;
		for (i = 0; i < IPD_RDO_CNT; i++) {
			ipred_list[i] = ((int)rmd_output->modes_32x4[idx][i]);
		}
	}
	else if (w == 0 && h == 1) {
		idx = x0*8 + (y0 >> 1);
		for (i = 0; i < IPD_RDO_CNT; i++) {
			ipred_list[i] = ((int)rmd_output->modes_4x8[idx][i]);
		}
	}*/
	//else 
	if (w == 1 && h == 1) {
		idx = x0 * 4 + (y0 >> 1);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_8x8[idx][i]);
		}
	}
	else if (w == 2 && h == 1) {
		idx = x0 * 2 + (y0 >> 1);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_16x8[idx][i]);
		}
	}
	else if (w == 3 && h == 1) {
		idx = x0 + (y0 >> 1);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_32x8[idx][i]);
		}
	}
	else if (w == 4 && h == 1) {
		idx = y0 >> 1;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_64x8[idx][i]);
		}
	}
	//else if (w == 0 && h == 2) {
	//	idx = x0*4 + (y0 >> 2);
	//	for (i = 0; i < IPD_RDO_CNT; i++) {
	//		ipred_list[i] = ((int)rmd_output->modes_4x16[idx][i]);
	//	}
	//}
	else if (w == 1 && h == 2) {
		idx = x0 * 2 + (y0 >> 2);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_8x16[idx][i]);
		}
	}
	else if (w == 2 && h == 2) {
		idx = x0 + (y0 >> 2);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_16x16[idx][i]);
		}
	}
	else if (w == 3 && h == 2) {
		idx = (x0 >> 1) + (y0 >> 2);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_32x16[idx][i]);
		}
	}
	else if (w == 4 && h == 2) {
		idx = y0 >> 2;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_64x16[idx][i]);
		}
	}
	//else if (w == 0 && h == 3) {
	//	idx = x0*2 + (y0 >> 3);
	//	for (i = 0; i < IPD_RDO_CNT; i++) {
	//		ipred_list[i] = ((int)rmd_output->modes_4x32[idx][i]);
	//	}
	//}
	else if (w == 1 && h == 3) {
		idx = x0 + (y0 >> 3);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_8x32[idx][i]);
		}
	}
	else if (w == 2 && h == 3) {
		idx = (x0 >> 1) + (y0 >> 3);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_16x32[idx][i]);
		}
	}
	else if (w == 3 && h == 3) {
		idx = (x0 >> 2) + (y0 >> 3);
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_32x32[idx][i]);
		}
	}
	else if (w == 4 && h == 3) {
		idx = y0 >> 3;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_64x32[idx][i]);
		}
	}
	else if (w == 1 && h == 4) {
		idx = x0 >> 1;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_8x64[idx][i]);
		}
	}
	else if (w == 2 && h == 4) {
		idx = x0 >> 2;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_16x64[idx][i]);
		}
	}
	else if (w == 3 && h == 4) {
		idx = x0 >> 3;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_32x64[idx][i]);
		}
	}
	else if (w == 4 && h == 4) {
		idx = 0;
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = ((int)rmd_output->modes_64x64[idx][i]);
		}
	}
	else {
		for (i = 0; i < cnt; i++) {
#pragma HLS pipeline
			ipred_list[i] = 0;
		}
	}
	return cnt;
}

#if MD32C_TB
static FilePrinter md32cPrinter("E:/working/avs3ip/vivadoBackBox/chroma32TB/data", -1, -1);
static FileReader md32cReader("E:/working/avs3ip/vivadoBackBox/chroma32TB/data", -1);
U8 compareDataArray(ENC_CU_DATA_ARRAY* golden, ENC_CU_DATA_ARRAY* result)
{
	U8 res = 1;
	res = res && (golden->x_chroma == result->x_chroma);
	res = res && (golden->y_chroma == result->y_chroma);
	res = res && (golden->x_last == result->x_last);
	res = res && (golden->y_last == result->y_last);
	res = res && (golden->last_child == result->last_child);

	for (int i = 0; i < MAX_CU_DEPTH; i++)
	{
		for (int j = 0; j < NUM_BLOCK_SHAPE; j++)
		{
			for (int k = 0; k < MAX_CU_CNT_IN_LCU; k++)
			{
				res = res && (golden->split_mode[i][j][k] == result->split_mode[i][j][k]);
			}
		}
	}
	for (int i = 0; i < MAX_CU_CNT_IN_LCU; i++)
	{
		for (int j = 0; j < REFP_NUM; j++)
		{
			for (int k = 0; k < MV_D; k++)
			{
				res = res && (golden->mv[i][j][k] == result->mv[i][j][k]);
				res = res && (golden->mvd[i][j][k] == result->mvd[i][j][k]);
			}
		}
	}
	for (int i = 0; i < (1 << (MAX_CU_DEPTH - 1)) * (1 << (MAX_CU_DEPTH - 1)); i++)
	{
		res = res && (golden->pb_part[i] == result->pb_part[i]);
		res = res && (golden->tb_part[i] == result->tb_part[i]);
		res = res && (golden->pred_mode[i] == result->mvr_idx[i]);
		res = res && (golden->umve_flag[i] == result->umve_flag[i]);
		res = res && (golden->skip_idx[i] == result->skip_idx[i]);
		res = res && (golden->map_scu[i] == result->map_scu[i]);
		res = res && (golden->affine_flag[i] == result->affine_flag[i]);
		res = res && (golden->smvd_flag[i] == result->smvd_flag[i]);
		res = res && (golden->map_cu_mode[i] == result->map_cu_mode[i]);
		res = res && (golden->map_pb_tb_part[i] == result->map_pb_tb_part[i]);
		res = res && (golden->depth[i] == result->depth[i]);
		res = res && (golden->ipf_flag[i] == result->ipf_flag[i]);
		res = res && (golden->mvp_from_hmvp_flag[i] == result->mvp_from_hmvp_flag[i]);
		res = res && (golden->mpm[0][i] == result->mpm[0][i]);
		res = res && (golden->ipm[0][i] == result->ipm[0][i]);
		res = res && (golden->mpm[1][i] == result->mpm[1][i]);
		res = res && (golden->ipm[1][i] == result->ipm[1][i]);
		res = res && (golden->refi[i][0] == result->refi[i][0]);
		res = res && (golden->refi[i][1] == result->refi[i][1]);
		res = res && (golden->num_nz_coef[0][i] == result->num_nz_coef[0][i]);
		res = res && (golden->num_nz_coef[1][i] == result->num_nz_coef[1][i]);
		res = res && (golden->num_nz_coef[2][i] == result->num_nz_coef[2][i]);
		for (int j = 0; j < 8; j++)
		{
			res = res && (golden->mpm_ext[j][i] == result->mpm_ext[j][i]);
		}
	}

	for (int i = 0; i < (CU_SIZE << 4) >> 2; i++)
	{
		res = res && (golden->coef_u[i] == result->coef_u[i]);
		res = res && (golden->coef_v[i] == result->coef_v[i]);
		res = res && (golden->reco_u[i] == result->reco_u[i]);
		res = res && (golden->reco_u[i] == result->reco_u[i]);
	}
	return res;
}
static int tbCnt = 0;
#endif


s64 circuit_64x8_8x64(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father)
{
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U3 num_split_to_try = split_struct.num_split[part_num];;

	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);

	s64 cost[1] = { MAX_S64 };
	U2 tree_status_child[2] = { TREE_LC };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;
	U3 split_mode[2];
	split_mode[0] = NO_SPLIT;
	//split_mode[1] = (cu_height == 8) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	//U5 rmd_index = rmd_idx_tbl[cu_width_log2 - 2][cu_height_log2 - 2];

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	//decide allowed split modes for the current node
	/***************************** Step 1: decide normatively allowed split modes ********************************/
	static IP_ARRAY_SIZE<8, 64>			IP_buffer_8x64;
	static IP_ARRAY_SIZE<64, 8>			IP_buffer_64x8;
	static IP_MAP_SCU_SIZE<8, 64>		IP_map_scu_8x64;
	static IP_MAP_SCU_SIZE<64, 8>		IP_map_scu_64x8;
	static MPM_ARRAY_SIZE<8, 64>		MPM_buffer_8x64;
	static MPM_ARRAY_SIZE<64, 8>		MPM_buffer_64x8;
	static ENC_ME_NEB_INFO				up_nebs[1 + (64 >> 2) + 1];
	static ENC_ME_NEB_INFO				left_nebs[(64 >> 2)];
	static ENC_ME_NEB_INFO				nebs_line_ver[16];
	static ENC_ME_NEB_INFO				nebs_line_hor[16];
	static ENC_ME_LINE_MAP_SIZE<64>		me_line_map_64;
	static ENC_CU_DATA_ARRAY_SIZE<64>	cu_data_temp[1];
	static ENC_CU_DATA_ARRAY_SIZE<64>	curr_cu_data_best;

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3

#pragma HLS ARRAY_PARTITION variable=cu_data_temp->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->coef_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->reco_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mpm_ext cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->split_mode complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->split_mode complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mpm cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->refi cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mv cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mvd cyclic factor=2 dim=4

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif

	//cu_data_chroma_pos<64>(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, cu_data_temp, prev_cu_data_ptr, qt_depth + bet_depth);

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;

	switch (split_mode_father)
	{
	case SPLIT_BI_HOR:
		assert(cu_width == 64 && cu_height == 8);
		IP_buffer_64x8.fetch_bi_hor(part_num, IP_buffer_ptr_64x16);
		MPM_buffer_64x8.fetch_bi_hor(part_num, MPM_buffer_ptr_64x16);
		IP_map_scu_64x8.fetch_bi_hor(part_num, IP_map_scu_ptr_64x16);
		fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
		me_line_map_64.fetch_bi_hor(part_num, me_line_map_ptr_64, cu_width, cu_height);
		break;
	case SPLIT_BI_VER:
		assert(cu_width == 8 && cu_height == 64);
		IP_buffer_8x64.fetch_bi_ver(part_num, IP_buffer_ptr_16x64);
		MPM_buffer_8x64.fetch_bi_ver(part_num, MPM_buffer_ptr_16x64);
		IP_map_scu_8x64.fetch_bi_ver(part_num, IP_map_scu_ptr_16x64);
		fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
		me_line_map_64.fetch_bi_ver(part_num, me_line_map_ptr_64, cu_width, cu_height);
		break;
	default:
		assert(0);
	}

	STATIC_C IP_ARRAY_SIZE<64, 64> null_IP_buffer_ptr_64x64;
	STATIC_C IP_ARRAY_SIZE<32, 64> null_IP_buffer_ptr_32x64;
	STATIC_C IP_ARRAY_SIZE<64, 32> null_IP_buffer_ptr_64x32;
	STATIC_C IP_ARRAY_SIZE<16, 64> null_IP_buffer_ptr_16x64;
	STATIC_C IP_ARRAY_SIZE<64, 16> null_IP_buffer_ptr_64x16;
	STATIC_C MPM_ARRAY_SIZE<64, 64> null_MPM_buffer_ptr_64x64;
	STATIC_C MPM_ARRAY_SIZE<32, 64> null_MPM_buffer_ptr_32x64;
	STATIC_C MPM_ARRAY_SIZE<64, 32> null_MPM_buffer_ptr_64x32;
	STATIC_C MPM_ARRAY_SIZE<16, 64> null_MPM_buffer_ptr_16x64;
	STATIC_C MPM_ARRAY_SIZE<64, 16> null_MPM_buffer_ptr_64x16;
	STATIC_C IP_MAP_SCU_SIZE<64, 64> null_IP_map_scu_ptr_64x64;
	STATIC_C IP_MAP_SCU_SIZE<32, 64> null_IP_map_scu_ptr_32x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 32> null_IP_map_scu_ptr_64x32;
	STATIC_C IP_MAP_SCU_SIZE<16, 64> null_IP_map_scu_ptr_16x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 16> null_IP_map_scu_ptr_64x16;

	cost[0] = md_kernel_64(cu_width, cu_height, md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_in, boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
		cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&cu_data_temp[0],
		fme_mv_info,
		&null_IP_buffer_ptr_64x64,
		&null_IP_buffer_ptr_32x64,
		&null_IP_buffer_ptr_64x32,
		&null_IP_buffer_ptr_16x64,
		&null_IP_buffer_ptr_64x16,
		&IP_buffer_8x64, &IP_buffer_64x8,
		&null_MPM_buffer_ptr_64x64,
		&null_MPM_buffer_ptr_32x64,
		&null_MPM_buffer_ptr_64x32,
		&null_MPM_buffer_ptr_16x64,
		&null_MPM_buffer_ptr_64x16,
		&MPM_buffer_8x64, &MPM_buffer_64x8,
		&null_IP_map_scu_ptr_64x64,
		&null_IP_map_scu_ptr_32x64,
		&null_IP_map_scu_ptr_64x32,
		&null_IP_map_scu_ptr_16x64,
		&null_IP_map_scu_ptr_64x16,
		&IP_map_scu_8x64, &IP_map_scu_64x8,
		ref,
		Fetch_Ref_window_ptr);

	//cost_compare_fixed_point_one_time<64>(md_fw_ptr, &cost_best, cost, cu_data_temp, &curr_cu_data_best, cu_width_log2, cu_height_log2, cud, tree_status,
	//	split_mode, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child, tree_status_child, &best_tree_status_child, &tree_status_temp, 1);
	
	cost_best = cost[0];
	best_split_mode = split_mode[0];
	best_cons_pred_mode = cons_pred_mode_child[0];

#if MD32C_TB
	if (tbCnt < MD32C_TB_MAXCNT)
	{
		md32cPrinter.printAtribute("in_wlog2_" + std::to_string(tbCnt), sizeof(U3), &cu_width_log2, -1);
		md32cPrinter.printAtribute("in_hlog2_" + std::to_string(tbCnt), sizeof(U3), &cu_height_log2, -1);
		md32cPrinter.printAtribute("in_rdoq_model_" + std::to_string(tbCnt), sizeof(RDOQ_MODEL), rdoq_model, -1);
		md32cPrinter.printAtribute("in_model_cnt_" + std::to_string(tbCnt), sizeof(RDOQ_MODEL), model_cnt, -1);
		md32cPrinter.printAtribute("in_md_fw_ptr_" + std::to_string(tbCnt), sizeof(MD_FW), md_fw_ptr, -1);
		md32cPrinter.printAtribute("in_me_line_map_ptr_" + std::to_string(tbCnt), sizeof(ENC_ME_LINE_MAP), me_line_map_ptr, -1);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < (((1 << (7)) >> 1) >> 2); j++)
				md32cPrinter.printAtribute("in_inner_nebs_" + std::to_string(tbCnt), ((((1 << (7)) >> 1) >> 2)) * sizeof(ENC_ME_NEB_INFO), &inner_nebs[i][j][0], -1);
		md32cPrinter.printAtribute("in_curr_cu_data_temp_array_ptr_" + std::to_string(tbCnt), sizeof(ENC_CU_DATA_ARRAY), &curr_cu_data_best, -1);
		md32cPrinter.printAtribute("in_x_" + std::to_string(tbCnt), sizeof(U14), &x0, -1);
		md32cPrinter.printAtribute("in_y_" + std::to_string(tbCnt), sizeof(U14), &y0, -1);
		md32cPrinter.printAtribute("in_tree_status_" + std::to_string(tbCnt), sizeof(U2), &tree_status_temp[0], -1);
		md32cPrinter.printAtribute("in_cud_" + std::to_string(tbCnt), sizeof(U4), &cud, -1);
		md32cPrinter.printAtribute("in_md_input_ptr_" + std::to_string(tbCnt), sizeof(MD_INPUT), md_input_ptr, -1);
		md32cPrinter.printAtribute("in_p_fenc_LCU_UV_" + std::to_string(tbCnt), (MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2) * sizeof(pel), &p_fenc_LCU_UV[0], -1);
		md32cPrinter.printAtribute("in_RefWindowLCU_UP_" + std::to_string(tbCnt), ((SWH * SWW) >> 2) * sizeof(pel), &RefWindowLCU_UP[0], -1);
		md32cPrinter.printAtribute("in_RefWindowLCU_VP_" + std::to_string(tbCnt), ((SWH * SWW) >> 2) * sizeof(pel), &RefWindowLCU_VP[0], -1);
		for (int i = 0; i < RANGE_NUM; i++)
			for (int j = 0; j < DIR_DIM; j++)
				md32cPrinter.printAtribute("in_CtrPos_" + std::to_string(tbCnt), MAP_DIMS * sizeof(S14), &CtrPos[i][j][0], -1);
		U1 need_dochroma = ((best_tree_status_child[0] == TREE_L && tree_status == TREE_LC) || best_split_mode[0] == NO_SPLIT);
		md32cPrinter.printAtribute("in_need_dochroma_" + std::to_string(tbCnt), sizeof(U1), &need_dochroma, -1);
		md32cPrinter.printAtribute("in_cup_" + std::to_string(tbCnt), sizeof(U12), &cup, -1);
		md32cPrinter.printAtribute("in_ctx_cons_pred_mode_" + std::to_string(tbCnt), sizeof(U2), &cons_pred_mode, -1);
		md32cPrinter.printAtribute("in_IP_ARRAY_" + std::to_string(tbCnt), sizeof(IP_ARRAY), IP_buffer_ptr, -1);
		md32cPrinter.printAtribute("in_MPM_ARRAY_" + std::to_string(tbCnt), sizeof(MPM_ARRAY), MPM_buffer_ptr, -1);
		md32cPrinter.printAtribute("in_IP_MAP_SCU_" + std::to_string(tbCnt), sizeof(IP_MAP_SCU), MAP_SCU_IP_ptr, -1);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 4; j++)
				md32cPrinter.printAtribute("in_CtrPos_" + std::to_string(tbCnt), 2 * sizeof(S14), &best_affine_mv[i][j][0], -1);
	}

#endif

#if MD32C_TB
	if (tbCnt < MD32C_TB_MAXCNT)
	{
		md32cPrinter.printAtribute("out_curr_cu_data_temp_array_ptr_" + std::to_string(tbCnt), sizeof(ENC_CU_DATA_ARRAY), &curr_cu_data_best, -1);
#if MD32C_TB_LOOPTEST
		U8 wLoop; U8 hLoop; static RDOQ_MODEL rdoq_modelLoop; static RDOQ_MODEL model_cntLoop;
		static MD_FW md_fw_ptrLoop; static ENC_ME_LINE_MAP me_line_map_ptrLoop;
		static ENC_ME_NEB_INFO inner_nebsLoop[2][(((1 << (7)) >> 1) >> 2)][(((1 << (7)) >> 1) >> 2)];
		static ENC_CU_DATA_ARRAY curr_cu_data_temp_array_ptrLoop; U14 xLoop; U14 yLoop; U3 cu_width_log2Loop;
		U3 cu_height_log2Loop; U2 tree_statusLoop; U4 cudLoop; static MD_INPUT md_input_ptrLoop; static pel p_fenc_LCU_UVLoop[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2];
		static U8 RefWindowLCU_UPLoop[(SWH * SWW) >> 2]; static U8 RefWindowLCU_VPLoop[(SWH * SWW) >> 2]; S14 CtrPosLoop[RANGE_NUM][DIR_DIM][MAP_DIMS]; U1 need_dochromaLoop; U12 cupLoop; U2 ctx_cons_pred_modeLoop;
		static IP_ARRAY IP_buffer_ptrLoop; static MPM_ARRAY MPM_buffer_ptrLoop; static IP_MAP_SCU MAP_SCU_IP_ptrLoop; S14 best_affine_mvLoop[2][4][2];
		static ENC_CU_DATA_ARRAY curr_cu_data_temp_array_ptrOut;

		md32cReader.readAtribute("in_wlog2_" + std::to_string(tbCnt), sizeof(U3), &cu_width_log2Loop);
		md32cReader.readAtribute("in_hlog2_" + std::to_string(tbCnt), sizeof(U3), &cu_height_log2Loop);
		md32cReader.readAtribute("in_rdoq_model_" + std::to_string(tbCnt), sizeof(RDOQ_MODEL), &rdoq_modelLoop);
		md32cReader.readAtribute("in_model_cnt_" + std::to_string(tbCnt), sizeof(RDOQ_MODEL), &model_cntLoop);
		md32cReader.readAtribute("in_md_fw_ptr_" + std::to_string(tbCnt), sizeof(MD_FW), &md_fw_ptrLoop);
		md32cReader.readAtribute("in_me_line_map_ptr_" + std::to_string(tbCnt), sizeof(ENC_ME_LINE_MAP), &md_fw_ptrLoop);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < (((1 << (7)) >> 1) >> 2); j++)
				md32cReader.readAtribute("in_inner_nebs_" + std::to_string(tbCnt), ((((1 << (7)) >> 1) >> 2)) * sizeof(ENC_ME_NEB_INFO), &inner_nebsLoop[i][j][0]);
		md32cReader.readAtribute("in_curr_cu_data_temp_array_ptr_" + std::to_string(tbCnt), sizeof(ENC_CU_DATA_ARRAY), &curr_cu_data_temp_array_ptrLoop);
		md32cReader.readAtribute("in_x_" + std::to_string(tbCnt), sizeof(U14), &xLoop);
		md32cReader.readAtribute("in_y_" + std::to_string(tbCnt), sizeof(U14), &yLoop);
		md32cReader.readAtribute("in_tree_status_" + std::to_string(tbCnt), sizeof(U2), &tree_statusLoop);
		md32cReader.readAtribute("in_cud_" + std::to_string(tbCnt), sizeof(U4), &cudLoop);
		md32cReader.readAtribute("in_md_input_ptr_" + std::to_string(tbCnt), sizeof(MD_INPUT), &md_input_ptrLoop);
		md32cReader.readAtribute("in_p_fenc_LCU_UV_" + std::to_string(tbCnt), (MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2) * sizeof(pel), &p_fenc_LCU_UVLoop[0]);
		md32cReader.readAtribute("in_RefWindowLCU_UP_" + std::to_string(tbCnt), ((SWH * SWW) >> 2) * sizeof(pel), &RefWindowLCU_UPLoop[0]);
		md32cReader.readAtribute("in_RefWindowLCU_VP_" + std::to_string(tbCnt), ((SWH * SWW) >> 2) * sizeof(pel), &RefWindowLCU_VPLoop[0]);
		for (int i = 0; i < RANGE_NUM; i++)
			for (int j = 0; j < DIR_DIM; j++)
				md32cReader.readAtribute("in_CtrPos_" + std::to_string(tbCnt), MAP_DIMS * sizeof(S14), &CtrPosLoop[i][j][0]);
		md32cReader.readAtribute("in_need_dochroma_" + std::to_string(tbCnt), sizeof(U1), &need_dochromaLoop);
		md32cReader.readAtribute("in_cup_" + std::to_string(tbCnt), sizeof(U12), &cup);
		md32cReader.readAtribute("in_IP_ARRAY_" + std::to_string(tbCnt), sizeof(IP_ARRAY), &IP_buffer_ptrLoop);
		md32cReader.readAtribute("in_MPM_ARRAY_" + std::to_string(tbCnt), sizeof(MPM_ARRAY), &MPM_buffer_ptrLoop);
		md32cReader.readAtribute("in_IP_MAP_SCU_" + std::to_string(tbCnt), sizeof(IP_MAP_SCU), &MAP_SCU_IP_ptrLoop);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 4; j++)
				md32cReader.readAtribute("in_CtrPos_" + std::to_string(tbCnt), 2 * sizeof(S14), &best_affine_mvLoop[i][j][0]);

		md32cReader.readAtribute("out_curr_cu_data_temp_array_ptr_" + std::to_string(tbCnt), sizeof(ENC_CU_DATA_ARRAY), &curr_cu_data_temp_array_ptrOut);

		U8 ifSucc = compareDataArray(&curr_cu_data_temp_array_ptrOut, &curr_cu_data_best);
		if (!ifSucc)
			printf("Wrong at LoopTest: %d\n", tbCnt);
		tbCnt++;
#endif
	}
#endif

	switch (split_mode_father)
	{
	case SPLIT_BI_HOR:
		assert(cu_width == 64 && cu_height == 8);
		IP_buffer_64x8.update_bi_hor(part_num, cu_data_temp[0].reco_y);
		IP_map_scu_64x8.update_bi_hor(part_num, IP_map_scu_ptr_64x16->cur_map_scu, cu_data_temp[0].map_scu);
		MPM_buffer_64x8.update_bi_hor(part_num, cu_data_temp[0].ipm[0]);
		update_nebs_bi_hor(nebs_line_hor, cu_data_temp[0].refi, cu_data_temp[0].mv, cu_width, cu_height, part_num);
		me_line_map_64.update_bi_hor(part_num, cu_data_temp[0].map_scu, cu_data_temp[0].map_cu_mode, cu_width, cu_height);
		break;
	case SPLIT_BI_VER:
		assert(cu_width == 8 && cu_height == 64);
		IP_buffer_8x64.update_bi_ver(part_num, cu_data_temp[0].reco_y);
		IP_map_scu_8x64.update_bi_ver(part_num, IP_map_scu_ptr_16x64->cur_map_scu, cu_data_temp[0].map_scu);
		MPM_buffer_8x64.update_bi_ver(part_num, cu_data_temp[0].ipm[0]);
		update_nebs_bi_ver(nebs_line_ver, cu_data_temp[0].refi, cu_data_temp[0].mv, cu_width, cu_height, part_num);
		me_line_map_64.update_bi_ver(part_num, cu_data_temp[0].map_scu, cu_data_temp[0].map_cu_mode, cu_width, cu_height);
		break;
	default:
		assert(0);
	}

	store_enc_info_best<64>(md_fw_ptr, cu_data_temp, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try,
		prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST;
#endif

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

#if !ONLY_QT_QT
s64 md_64x16_16x64_loop(U1 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in, U1 isBT_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag, ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int ns_64x64)
{
#pragma HLS INLINE off
	s64 bt_cost = (s64)MAX_S64;
	if (isBT_allow) {
		COM_SPLIT_STRUCT split_struct;
		U3 part_num = 0;

		bt_cost = prepare_cu_info_COM_combine(md_fw_ptr, md_input_ptr->lambda_y, cu_data_temp_ptr, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS loop_tripcount max=2
			bt_cost += circuit_64x16_16x64(
				md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_64,
				up_nebs_father, left_nebs_father,
				skip_in,
				cu_data_temp_ptr, qt_depth, split_mode == NO_SPLIT ? bet_depth : (U3)(bet_depth + 1), cons_pred_mode_BTH[0],
				tree_status, part_num, split_struct, cu_width_log2, cu_height_log2,
				fme_mv_info,
				IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32,
				MPM_buffer_ptr_64x64, MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32,
				IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32,
				ref,
				Fetch_Ref_window_ptr, split_mode, ns_64x64);
		}
//#if KERNEL_SIZE_CLOSE(64, 64)
//		if (cu_width == 64 && cu_height == 64 && ns_64x64)
//			bt_cost = MAX_S64;
//#endif // KERNEL_SIZE_CLOSE(64, 64)
//#if KERNEL_SIZE_CLOSE(32, 64)
//		if (cu_width == 32 && cu_height == 64 && split_mode == NO_SPLIT)
//			bt_cost = MAX_S64;
//#endif // KERNEL_SIZE_CLOSE(32, 64)
//#if KERNEL_SIZE_CLOSE(64, 32)
//		if (cu_width == 64 && cu_height == 32 && split_mode == NO_SPLIT)
//			bt_cost = MAX_S64;
//#endif // KERNEL_SIZE_CLOSE(64, 32)
	}
	return bt_cost;
}
#endif

void load_64x64_64x32_32x64(Load2PU& in_buf, InstructionPU& insn_pu, Channel& channel, InstructionLoad& insn_load)
{
	switch (insn_pu.split_mode)
	{
	case NO_SPLIT:
		for (int i = 0; i < (insn_pu.cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
			cpy_1d_2_1d<S3, REFP_NUM>(channel.up_nebs_64[i].refi, channel.up_nebs_32x32[i].refi);
			cpy_2d_2_2d<S14, REFP_NUM, MV_D>(channel.up_nebs_64[i].mv, channel.up_nebs_32x32[i].mv);
		}
		for (int i = 0; i < (insn_pu.cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			cpy_1d_2_1d<S3, REFP_NUM>(channel.left_nebs_64[i].refi, channel.left_nebs_32x32[i].refi);
			cpy_2d_2_2d<S14, REFP_NUM, MV_D>(channel.left_nebs_64[i].mv, channel.left_nebs_32x32[i].mv);
		}

		cpy_1d_2_1d<U32, 18>(channel.me_line_map_ptr_64.map_scu_up_line, in_buf.me_line_map_64.map_scu_up_line);
		cpy_1d_2_1d<U32, 18>(channel.me_line_map_ptr_64.map_cu_mode_up_line, in_buf.me_line_map_64.map_cu_mode_up_line);
		cpy_1d_2_1d<U32, 16>(channel.me_line_map_ptr_64.map_scu_left_line, in_buf.me_line_map_64.map_scu_left_line);
		cpy_1d_2_1d<U32, 16>(channel.me_line_map_ptr_64.map_cu_mode_left_line, in_buf.me_line_map_64.map_cu_mode_left_line);
		in_buf.me_line_map_64.min_scu_x = channel.me_line_map_ptr_64.min_scu_x;
		in_buf.me_line_map_64.min_scu_y = channel.me_line_map_ptr_64.min_scu_y;
		in_buf.me_line_map_64.above_line_idx = channel.me_line_map_ptr_64.above_line_idx;
		in_buf.me_line_map_64.curr_ctu_idx = channel.me_line_map_ptr_64.curr_ctu_idx;
		break;
	case SPLIT_BI_VER:
		assert(insn_pu.cu_width == 32 && insn_pu.cu_height == 64);
		switch (insn_load.part_num)
		{
		case 0:
			for (int i = 0; i < (32 << 1); i++)
			{
				in_buf.IP_buffer_32x64.up_line[i] = channel.IP_buffer_ptr_64x64.up_line[i];
			}
			for (int i = 0; i < 64; i++)
			{
				in_buf.IP_buffer_32x64.left_line[i] = channel.IP_buffer_ptr_64x64.left_line[i];
			}
			in_buf.IP_buffer_32x64.top_left = channel.IP_buffer_ptr_64x64.top_left;
			break;
		case 1:
			for (int i = 0; i < (32 << 1); i++)
			{
				in_buf.IP_buffer_32x64.up_line[i] = channel.IP_buffer_ptr_64x64.up_line[i + 32];
			}
			for (int i = 0; i < 64; i++)
			{
				in_buf.IP_buffer_32x64.left_line[i] = in_buf.IP_buffer_32x64.rec_line_ver[i];
			}
			in_buf.IP_buffer_32x64.top_left = channel.IP_buffer_ptr_64x64.up_line[31];
			break;
		default:assert(0);
		}
		in_buf.MPM_buffer_32x64.fetch_bi_ver(insn_load.part_num, &channel.MPM_buffer_ptr_64x64);
		in_buf.IP_map_scu_32x64.fetch_bi_ver(insn_load.part_num, &channel.IP_map_scu_ptr_64x64);
		fetch_nebs_bi_ver(channel.up_nebs_32x32, channel.left_nebs_32x32, channel.up_nebs_64, channel.left_nebs_64, channel.nebs_line_ver_64, insn_pu.cu_width, insn_pu.cu_height, insn_load.part_num);
		in_buf.me_line_map_64.fetch_bi_ver(insn_load.part_num, &channel.me_line_map_ptr_64, insn_pu.cu_width, insn_pu.cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(insn_pu.cu_width == 64 && insn_pu.cu_height == 32);
		switch (insn_load.part_num)
		{
		case 0:
			for (int i = 0; i < (64 << 1); i++)
			{
				in_buf.IP_buffer_64x32.up_line[i] = channel.IP_buffer_ptr_64x64.up_line[i];
			}
			for (int i = 0; i < (32 << 1); i++)
			{
				in_buf.IP_buffer_64x32.left_line[i] = channel.IP_buffer_ptr_64x64.left_line[i];
			}
			in_buf.IP_buffer_64x32.top_left = channel.IP_buffer_ptr_64x64.top_left;
			break;
		case 1:
			for (int i = 0; i < 64; i++)
			{
				in_buf.IP_buffer_64x32.up_line[i] = in_buf.IP_buffer_64x32.rec_line_hor[i];
			}
			for (int i = 0; i < 32; i++)
			{
				in_buf.IP_buffer_64x32.left_line[i] = channel.IP_buffer_ptr_64x64.left_line[i + 32];
			}
			in_buf.IP_buffer_64x32.top_left = channel.IP_buffer_ptr_64x64.left_line[31];
			break;
		default:
			assert(0);
		}
		in_buf.MPM_buffer_64x32.fetch_bi_hor(insn_load.part_num, &channel.MPM_buffer_ptr_64x64);
		in_buf.IP_map_scu_64x32.fetch_bi_hor(insn_load.part_num, &channel.IP_map_scu_ptr_64x64);
		fetch_nebs_bi_hor(channel.up_nebs_32x32, channel.left_nebs_32x32, channel.up_nebs_64, channel.left_nebs_64, channel.nebs_line_hor_64, insn_pu.cu_width, insn_pu.cu_height, insn_load.part_num);
		in_buf.me_line_map_64.fetch_bi_hor(insn_load.part_num, &channel.me_line_map_ptr_64, insn_pu.cu_width, insn_pu.cu_height);
		break;
	default:assert(0);
	}
}

s64 store_64x64_64x32_32x64(InstructionStore& insn_store, Channel& channel, PU2Store pu_out_buf[PU_NUM], LCUConfig& config)
{
	U2 tree_status_temp = insn_store.tree_status;
	ENC_CU_DATA_ARRAY curr_cu_data_temp[2];
	U2 cons_pred_mode_child[2];
	U3 split_mode[2];
	U2 tree_status_child[2];
	s64 cost[2];

	for (size_t i = 0; i < 2; i++)
	{
		split_mode[i] = insn_store.split_mode[i];
		cost[i] = insn_store.cost_temp[i];
		tree_status_child[i] = insn_store.tree_status_child[i];
		cons_pred_mode_child[i] = pu_out_buf[split_mode[i]].cons_pred_mode_child;
		curr_cu_data_temp[i] = pu_out_buf[split_mode[i]].curr_cu_data_temp_64x64;
	}
	cost_compare_fixed_point_one_time(&config.md_fw_ptr, &channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2], cost, curr_cu_data_temp, &channel.curr_cu_data_best, insn_store.cu_width_log2, insn_store.cu_height_log2, insn_store.cud, insn_store.tree_status,
		split_mode, &insn_store.best_split_mode, &insn_store.best_cons_pred_mode, cons_pred_mode_child, tree_status_child, &insn_store.best_tree_status_child, &tree_status_temp, 2);

	switch (channel.split_mode_father)
	{
	case NO_SPLIT:
		break;
	case SPLIT_BI_VER:
		assert(insn_store.cu_width == 32 && insn_store.cu_height == 64);
		switch (insn_store.part_num)
		{
		case 0:
			for (int i = 0; i < 64; i++)
			{
				channel.IP_buffer_32x64.rec_line_ver[i] = channel.curr_cu_data_best.reco_y[i][insn_store.cu_width - 1];
			}
			break;
		case 1:
			break;
		default:
			assert(0);
		}
		channel.IP_map_scu_32x64.update_bi_ver(insn_store.part_num, channel.IP_map_scu_ptr_64x64.cur_map_scu, channel.curr_cu_data_best.map_scu);
		channel.MPM_buffer_32x64.update_bi_ver(insn_store.part_num, channel.curr_cu_data_best.ipm[0]);
		update_nebs_bi_ver(channel.nebs_line_ver_64, channel.curr_cu_data_best.refi, channel.curr_cu_data_best.mv, insn_store.cu_width, insn_store.cu_height, insn_store.part_num);
		channel.me_line_map_64.update_bi_ver(insn_store.part_num, channel.curr_cu_data_best.map_scu, channel.curr_cu_data_best.map_cu_mode, insn_store.cu_width, insn_store.cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(insn_store.cu_width == 64 && insn_store.cu_height == 32);
		switch (insn_store.part_num)
		{
		case 0:
			for (int i = 0; i < 64; i++)
			{
				channel.IP_buffer_64x32.rec_line_hor[i] = channel.curr_cu_data_best.reco_y[31][i];
			}
			break;
		case 1:
			break;
		default:
			assert(0); 
		}
		channel.IP_map_scu_64x32.update_bi_hor(insn_store.part_num, channel.IP_map_scu_ptr_64x64.cur_map_scu, channel.curr_cu_data_best.map_scu);
		channel.MPM_buffer_64x32.update_bi_hor(insn_store.part_num, channel.curr_cu_data_best.ipm[0]);
		update_nebs_bi_hor(channel.nebs_line_hor_64, channel.curr_cu_data_best.refi, channel.curr_cu_data_best.mv, insn_store.cu_width, insn_store.cu_height, insn_store.part_num);
		channel.me_line_map_64.update_bi_hor(insn_store.part_num, channel.curr_cu_data_best.map_scu, channel.curr_cu_data_best.map_cu_mode, insn_store.cu_width, insn_store.cu_height);
		break;
	default: assert(0);
	}

	store_enc_info_best(&config.md_fw_ptr, &channel.curr_cu_data_best, insn_store.x0, insn_store.y0, insn_store.cup, insn_store.cu_width_log2, insn_store.cu_height_log2, insn_store.cud, \
		insn_store.cons_pred_mode, insn_store.cu_width, insn_store.cu_height, insn_store.tree_status, \
		&insn_store.best_split_mode, &insn_store.best_cons_pred_mode, insn_store.num_split_to_try, &channel.prev_up_cu_data_temp_array_ptr,\
		config.md_input_ptr.pix_x, config.md_input_ptr.pix_y, insn_store.prev_up_log2_sub_cuw);

	return channel.cost_best[(insn_store.y0-insn_store.y_pos)>>MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2];
}

#if !ONLY_QT_QT
void pu_64x64_64x32_32x64_loop(Load2PU& pu_in_buf,  LCUInput& buffer, PU2Store& pu_out_buf, Channel& channel, s64& cost_temp, Arbitor& ref)
{
	U2 NS_luma_pred_mode = ONLY_INTRA;
	
	cost_temp = md_64x16_16x64_loop(pu_in_buf.insn.split_mode,
		&pu_in_buf.config.md_fw_ptr, &pu_in_buf.config.md_input_ptr, &buffer.g_rmd_output, buffer.RefWindowLCU_UP, buffer.RefWindowLCU_VP, buffer.CtrPos,
#if ENABLE_BFRAME
		buffer.RefWindowLCU_UP_ref1, buffer.RefWindowLCU_VP_ref1, buffer.CtrPos_ref1, &buffer.me_mv_dir,
#endif
		buffer.p_fenc_LCU_Y,
		&pu_in_buf.me_line_map_64,
		channel.up_nebs_32x32, channel.left_nebs_32x32,
		&pu_in_buf.buffer.skip_in,true, pu_in_buf.insn.x0, pu_in_buf.insn.y0, pu_in_buf.insn.cup, pu_in_buf.insn.cu_width_log2, pu_in_buf.insn.cu_height_log2, 0,
		pu_in_buf.insn.qt_depth, pu_in_buf.insn.bet_depth, pu_in_buf.insn.cons_pred_mode,
		pu_in_buf.insn.cu_width, pu_in_buf.insn.cu_height, NS_luma_pred_mode, &pu_out_buf.curr_cu_data_temp_64x64, &pu_out_buf.cons_pred_mode_child, pu_in_buf.insn.tree_status,
		&pu_in_buf.buffer.fme_mv_info,
		&channel.IP_buffer_ptr_64x64, &pu_in_buf.IP_buffer_32x64, &pu_in_buf.IP_buffer_64x32,
		&channel.MPM_buffer_ptr_64x64, &pu_in_buf.MPM_buffer_32x64, &pu_in_buf.MPM_buffer_64x32,
		&channel.IP_map_scu_ptr_64x64, &pu_in_buf.IP_map_scu_32x64, &pu_in_buf.IP_map_scu_64x32,
		ref, 
		&pu_in_buf.buffer.Fetch_Ref_window_ptr, channel.split_mode_father == NO_SPLIT ? 1 : 0);
}

//64x32
s64 circuit_64x64_64x32_32x64(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
	const U3 parent_split,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father)
{
#pragma HLS INLINE off
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U3 num_split_to_try = split_struct.num_split[part_num];

	static ENC_CU_DATA_ARRAY curr_cu_data_best;
	static ENC_CU_DATA_ARRAY curr_cu_data_temp[2];
	s64 cost[2] = { MAX_S64, MAX_S64 };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS, NO_MODE_CONS };
	U2 tree_status_child[2] = { TREE_LC, TREE_LC };

	U2 best_tree_status_child = TREE_LC;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;
	U3 split_mode[2];
	split_mode[0] = NO_SPLIT;
	split_mode[1] = (cu_height == 32) ? SPLIT_BI_HOR : SPLIT_BI_VER;
	//#pragma HLS resource variable=curr_cu_data_best->split_mode core=RAM_2P_BRAM
	//#pragma HLS resource variable=curr_cu_data_temp->split_mode core=RAM_2P_BRAM
	//#pragma HLS resource variable=prev_up_cu_data_temp_array_ptr->split_mode core=RAM_2P_BRAM

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->coef_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->reco_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->split_mode cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->split_mode cyclic factor=7 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mpm_ext cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->refi cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mv cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mvd cyclic factor=2 dim=4
#if CIRCUIT_64x64_64x32_32x64
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mvd cyclic factor=2 dim=3
#endif
	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	//for (int i = 0; i < 2; i++)
	//{
	//	cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &curr_cu_data_temp[i], prev_up_cu_data_temp_array_ptr, qt_depth + bet_depth);
	//}

	static IP_ARRAY_SIZE<32, 64>	IP_buffer_32x64;
	static IP_ARRAY_SIZE<64, 32>	IP_buffer_64x32;
	static IP_MAP_SCU_SIZE<32, 64>	IP_map_scu_32x64;
	static IP_MAP_SCU_SIZE<64, 32>	IP_map_scu_64x32;
	static MPM_ARRAY_SIZE<32, 64>	MPM_buffer_32x64;
	static MPM_ARRAY_SIZE<64, 32>	MPM_buffer_64x32;
	static ENC_ME_NEB_INFO			up_nebs[1 + 16 + 1];
	static ENC_ME_NEB_INFO			left_nebs[16];
	static ENC_ME_NEB_INFO			nebs_line_ver[16];
	static ENC_ME_NEB_INFO			nebs_line_hor[16];
	static ENC_ME_LINE_MAP_SIZE<64>	me_line_map_64;
	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;
	S7 ip_array_size_idx;

	STATIC_C InstructionPU insn_pu;
	STATIC_C Load2PU in_buf;
	STATIC_C Channel channel;
	STATIC_C ShareMemory pool;
	STATIC_C LCUConfig config;
	STATIC_C LCUInput buffer;
	STATIC_C InstructionLoad insn_load;
	STATIC_C InstructionStore insn_store;
	static Load2PU pu_in_buf[PU_NUM];
	static PU2Store pu_out_buf[PU_NUM];

	insn_pu.cu_width_log2 = cu_width_log2;
	insn_pu.cu_height_log2 = cu_height_log2;
	insn_pu.x0 = x0;
	insn_pu.y0 = y0;
	insn_pu.split_mode = split_mode_father;
	insn_pu.cu_width = cu_width;
	insn_pu.cu_height = cu_height;
	insn_load.part_num = part_num;

	channel.IP_buffer_ptr_64x64 = *IP_buffer_ptr_64x64;
	channel.IP_map_scu_ptr_64x64 = *IP_map_scu_ptr_64x64;
	channel.MPM_buffer_ptr_64x64 = *MPM_buffer_ptr_64x64;
	channel.me_line_map_ptr_64 = *me_line_map_ptr_64;
	channel.split_mode_father = split_mode_father;

	for (size_t i = 0; i < 18; i++)
	{
		channel.up_nebs_64[i] = up_nebs_father[i];
	}

	for (size_t i = 0; i < 16; i++)
	{
		channel.left_nebs_64[i] = left_nebs_father[i];
	}
	buffer.g_rmd_output = *g_rmd_output;
	buffer.me_mv_dir = *me_mv_dir;
	buffer.skip_in = *skip_in;
	buffer.fme_mv_info = *fme_mv_info;
	buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;

	for (size_t i = 0; i < MAX_CU_SIZE_FETCH; i++)
	{
		for (size_t j = 0; j < MAX_CU_SIZE_FETCH; j++)
		{
			buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
		}
	}

	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			for (size_t k = 0; k < 2; k++) {
				buffer.CtrPos[i][j][k] = CtrPos[i][j][k];
			}
		}
	}

	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			for (size_t k = 0; k < 2; k++) {
				buffer.CtrPos_ref1[i][j][k] = CtrPos_ref1[i][j][k];
			}
		}
	}

	for (size_t i = 0; i < MAX_CU_SIZE_FETCH; i++)
	{
		for (size_t j = 0; j < MAX_CU_SIZE_FETCH; j++) {
			buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
		}
	}

	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_UP[i] = RefWindowLCU_UP[i];
	}

	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_UP_ref1[i] = RefWindowLCU_UP_ref1[i];
	}

	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_VP[i] = RefWindowLCU_VP[i];
	}

	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_VP_ref1[i] = RefWindowLCU_VP_ref1[i];
	}

	in_buf.IP_buffer_32x64 = IP_buffer_32x64;
	in_buf.IP_buffer_64x32 = IP_buffer_64x32;
	in_buf.IP_map_scu_32x64 = IP_map_scu_32x64;
	in_buf.IP_map_scu_64x32 = IP_map_scu_64x32;
	in_buf.MPM_buffer_32x64 = MPM_buffer_32x64;
	in_buf.MPM_buffer_64x32 = MPM_buffer_64x32;
	in_buf.me_line_map_64 = me_line_map_64;

	load_64x64_64x32_32x64(in_buf, insn_pu, channel, insn_load);

	for (int i = 0; i < 18; i++)
	{
		up_nebs[i] = channel.up_nebs_32x32[i];
	}
	for (int i = 0; i < 16; i++)
	{
		left_nebs[i] = channel.left_nebs_32x32[i];
	}

	IP_buffer_32x64 = in_buf.IP_buffer_32x64;
	IP_buffer_64x32 = in_buf.IP_buffer_64x32;
	IP_map_scu_32x64 = in_buf.IP_map_scu_32x64;
	IP_map_scu_64x32 = in_buf.IP_map_scu_64x32;
	MPM_buffer_32x64 = in_buf.MPM_buffer_32x64;
	MPM_buffer_64x32 = in_buf.MPM_buffer_64x32;
	me_line_map_64 = in_buf.me_line_map_64;

	// pu<KERNEL_32X64>
	// pu<KERNEL_64X32>
#if !KERNEL_SIZE_CLOSE(64, 32) || !KERNEL_SIZE_CLOSE(32, 64) || !KERNEL_SIZE_CLOSE(64, 64)
	if (split_struct.split_allow[part_num][split_mode[0]])
	{
			pu_in_buf[split_mode[0]].config.md_fw_ptr = *md_fw_ptr;
			pu_in_buf[split_mode[0]].config.md_input_ptr = *md_input_ptr;
			pu_in_buf[split_mode[0]].buffer.me_mv_dir = *me_mv_dir;
			pu_in_buf[split_mode[0]].buffer.skip_in = *skip_in;
			pu_in_buf[split_mode[0]].buffer.fme_mv_info = *fme_mv_info;
			pu_in_buf[split_mode[0]].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;

			pu_in_buf[split_mode[0]].me_line_map_64 = me_line_map_64;

			for (size_t i = 0; i < 18; i++) {
				pu_in_buf[split_mode[0]].up_nebs_64[i] = channel.up_nebs_32x32[i];
			}

			for (size_t i = 0; i < 16; i++) {
				pu_in_buf[split_mode[0]].left_nebs_64[i] = channel.left_nebs_32x32[i];
			}

			pu_in_buf[split_mode[0]].insn.cud = cud;
			pu_in_buf[split_mode[0]].insn.split_mode = split_mode[0];
			pu_in_buf[split_mode[0]].insn.cup = cup;
			pu_in_buf[split_mode[0]].insn.cu_height_log2 = cu_height_log2;
			pu_in_buf[split_mode[0]].insn.cu_width_log2 = cu_width_log2;
			pu_in_buf[split_mode[0]].insn.cu_width = cu_width;
			pu_in_buf[split_mode[0]].insn.cu_height = cu_height;
			pu_in_buf[split_mode[0]].insn.qt_depth = qt_depth;
			pu_in_buf[split_mode[0]].insn.x0 = x0;
			pu_in_buf[split_mode[0]].insn.y0 = y0;
			pu_in_buf[split_mode[0]].insn.qt_depth = qt_depth;
			pu_in_buf[split_mode[0]].insn.bet_depth = bet_depth;
			pu_in_buf[split_mode[0]].insn.cons_pred_mode = cons_pred_mode;
			pu_in_buf[split_mode[0]].insn.tree_status = tree_status;

			pu_in_buf[split_mode[0]].IP_buffer_32x64 = IP_buffer_32x64;
			pu_in_buf[split_mode[0]].IP_buffer_64x32 = IP_buffer_64x32;
			pu_in_buf[split_mode[0]].MPM_buffer_32x64 = MPM_buffer_32x64;
			pu_in_buf[split_mode[0]].MPM_buffer_64x32 = MPM_buffer_64x32;
			pu_in_buf[split_mode[0]].IP_map_scu_32x64 = IP_map_scu_32x64;
			pu_in_buf[split_mode[0]].IP_map_scu_64x32 = IP_map_scu_64x32;

			pu_64x64_64x32_32x64_loop(pu_in_buf[split_mode[0]], buffer, pu_out_buf[split_mode[0]], channel, cost[0], ref);
	}
#endif // !KERNEL_SIZE_CLOSE(64, 32) || !KERNEL_SIZE_CLOSE(32, 64)
#if KERNEL_SIZE_CLOSE(64, 32)
	if (cu_width == 64 && cu_height == 32)
		cost[0] = MAX_S64;
#endif // KERNEL_SIZE_CLOSE(64, 32)
#if KERNEL_SIZE_CLOSE(32, 64)
	if (cu_width == 32 && cu_height == 64)
		cost[0] = MAX_S64;
#endif // KERNEL_SIZE_CLOSE(32, 64)

	STATIC_C IP_ARRAY_SIZE<64, 64> null_IP_buffer_ptr_64x64;
	STATIC_C MPM_ARRAY_SIZE<64, 64> null_MPM_buffer_ptr_64x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 64> null_IP_map_scu_ptr_64x64;
	if (split_struct.split_allow[part_num][split_mode[1]])
	{
		pu_in_buf[split_mode[1]].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[split_mode[1]].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[split_mode[1]].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[split_mode[1]].buffer.skip_in = *skip_in;
		pu_in_buf[split_mode[1]].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[split_mode[1]].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
		pu_in_buf[split_mode[1]].me_line_map_64 = me_line_map_64;

		for (size_t i = 0; i < 18; i++) {
			pu_in_buf[split_mode[1]].up_nebs_64[i] = channel.up_nebs_32x32[i];
		}

		for (size_t i = 0; i < 16; i++) {
			pu_in_buf[split_mode[1]].left_nebs_64[i] = channel.left_nebs_32x32[i];
		}

		pu_in_buf[split_mode[1]].insn.cud = cud;
		pu_in_buf[split_mode[1]].insn.split_mode = split_mode[1];
		pu_in_buf[split_mode[1]].insn.cup = cup;
		pu_in_buf[split_mode[1]].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[split_mode[1]].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[split_mode[1]].insn.cu_width = cu_width;
		pu_in_buf[split_mode[1]].insn.cu_height = cu_height;
		pu_in_buf[split_mode[1]].insn.qt_depth = qt_depth;
		pu_in_buf[split_mode[1]].insn.x0 = x0;
		pu_in_buf[split_mode[1]].insn.y0 = y0;
		pu_in_buf[split_mode[1]].insn.qt_depth = qt_depth;
		pu_in_buf[split_mode[1]].insn.bet_depth = bet_depth;
		pu_in_buf[split_mode[1]].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[split_mode[1]].insn.tree_status = tree_status;

		pu_in_buf[split_mode[1]].IP_buffer_32x64 = IP_buffer_32x64;
		pu_in_buf[split_mode[1]].IP_buffer_64x32 = IP_buffer_64x32;
		pu_in_buf[split_mode[1]].MPM_buffer_32x64 = MPM_buffer_32x64;
		pu_in_buf[split_mode[1]].MPM_buffer_64x32 = MPM_buffer_64x32;
		pu_in_buf[split_mode[1]].IP_map_scu_32x64 = IP_map_scu_32x64;
		pu_in_buf[split_mode[1]].IP_map_scu_64x32 = IP_map_scu_64x32;

		pu_64x64_64x32_32x64_loop(pu_in_buf[split_mode[1]], buffer, pu_out_buf[split_mode[1]], channel, cost[1], ref);
	}

	insn_store.cu_width_log2 = cu_width_log2;
	insn_store.cu_height_log2 = cu_height_log2;
	insn_store.cud = cud;
	insn_store.tree_status = tree_status;
	insn_store.num_split_to_try = num_split_to_try;
	insn_store.part_num = part_num;
	insn_store.cu_width = cu_width;
	insn_store.cu_height = cu_height;
	insn_store.x0 = x0;
	insn_store.y0 = y0;
	insn_store.cup = cup;
	insn_store.x_pos = split_struct.x_pos[0];
	insn_store.best_split_mode = NO_SPLIT;
	insn_store.best_cons_pred_mode = NO_MODE_CONS;
	insn_store.best_tree_status_child = TREE_LC;
	insn_store.y_pos = split_struct.y_pos[0];
	insn_store.prev_up_log2_sub_cuw = prev_up_log2_sub_cuw;
	channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2] = MAX_S64;
	channel.curr_cu_data_best = curr_cu_data_best;
	config.md_fw_ptr = *md_fw_ptr;
	config.md_input_ptr = *md_input_ptr;

	for (size_t i = 0; i < 2; i++) {
		insn_store.tree_status_child[i] = TREE_LC;
		insn_store.split_mode[i] = split_mode[i];
		insn_store.cost_temp[i] = cost[i];
	}

	cost_best = store_64x64_64x32_32x64(insn_store, channel, pu_out_buf, config);

	IP_buffer_32x64 = channel.IP_buffer_32x64;
	IP_buffer_64x32 = channel.IP_buffer_64x32;
	IP_map_scu_32x64 = channel.IP_map_scu_32x64;
	IP_map_scu_64x32 = channel.IP_map_scu_64x32;
	MPM_buffer_32x64 = channel.MPM_buffer_32x64;
	MPM_buffer_64x32 = channel.MPM_buffer_64x32;
	me_line_map_64 = channel.me_line_map_64;

	*prev_up_cu_data_temp_array_ptr = channel.prev_up_cu_data_temp_array_ptr;
	*IP_map_scu_ptr_64x64 = channel.IP_map_scu_ptr_64x64;

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}
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
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS INLINE off
	s64 bt_cost = MAX_S64;
	COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	U1 split_mode = (cu_width == 64 && cu_height == 16) ? SPLIT_BI_HOR : SPLIT_BI_VER;

	if (isBTH_allow) {
		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);

		bt_cost = prepare_cu_info_COM_combine_only_bt<64>(md_fw_ptr, md_input_ptr->lambda_y, cu_data_temp_ptr, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS loop_tripcount max=2
			bt_cost += circuit_64x8_8x64(
				md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_64,
				up_nebs, left_nebs,
				skip_in,
				cu_data_temp_ptr, qt_depth, bet_depth + 1, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2, fme_mv_info,
				IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
				MPM_buffer_ptr_16x64, MPM_buffer_ptr_64x16,
				IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
				ref,
				Fetch_Ref_window_ptr, split_mode);
		}
		
#if KERNEL_SIZE_CLOSE(64, 8)
		if (cu_width == 64 && cu_height == 16)
			bt_cost = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(64, 8)
#if KERNEL_SIZE_CLOSE(8, 64)
		if (cu_width == 16 && cu_height == 64)
			bt_cost = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(8, 64)
	}
	return bt_cost;
}

s64 circuit_16x16_16x8_8x16_md2(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4],
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA_ARRAY_SIZE<16>* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode)
{
#pragma HLS INLINE off
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U3 num_split_to_try = split_struct.num_split[part_num];

	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);

	/*************************for parallel**********************/
	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };
	U2 tree_status_child[1] = { TREE_LC };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_temp;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U3 split_mode_order[3];
	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;

	split_mode_order[0] = NO_SPLIT;
	split_mode_order[1] = (cu_height == 8) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	split_mode_order[2] = (cu_height == 8) ? SPLIT_BI_HOR : SPLIT_BI_VER;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	static IP_ARRAY_SIZE<8, 16>			IP_buffer_8x16;
	static IP_ARRAY_SIZE<16, 8>			IP_buffer_16x8;
	static IP_MAP_SCU_SIZE<8, 16>		IP_map_scu_8x16;
	static IP_MAP_SCU_SIZE<16, 8>		IP_map_scu_16x8;
	static MPM_ARRAY_SIZE<8, 16>		MPM_buffer_8x16;
	static MPM_ARRAY_SIZE<16, 8>		MPM_buffer_16x8;
	static ENC_ME_NEB_INFO				up_nebs[6];
	static ENC_ME_NEB_INFO				left_nebs[4];
	static ENC_ME_NEB_INFO				nebs_line_hor[4];
	static ENC_ME_NEB_INFO				nebs_line_ver[4];
	static ENC_ME_LINE_MAP_SIZE<16>		me_line_map_16;
	static ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data;
	static ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data_best;

#pragma HLS ARRAY_PARTITION variable=curr_cu_data->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mpm_ext cyclic factor=8 dim=1	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mvd cyclic factor=2 dim=3		

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3		

#if CIRCUIT_16x16_16x8_8x16_MD2_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd cyclic factor=2 dim=3	
#endif


	//if (tree_status == TREE_LC) {
	//	curr_cu_data.x_chroma = x0;
	//	curr_cu_data.y_chroma = y0;
	//	curr_cu_data.x_last = x0 + cu_width - 4;
	//	curr_cu_data.y_last = y0 + cu_height - 4;
	//	curr_cu_data.last_child = FALSE;
	//}
	//else {
	//	curr_cu_data.x_chroma = prev_cu_data_ptr->x_chroma;
	//	curr_cu_data.y_chroma = prev_cu_data_ptr->y_chroma;
	//	curr_cu_data.x_last = prev_cu_data_ptr->x_last;
	//	curr_cu_data.y_last = prev_cu_data_ptr->y_last;
	//	curr_cu_data.last_child = x0 + cu_width == prev_cu_data_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_ptr->y_last + 4;
	//}

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;
	switch (split_mode)
	{
	case SPLIT_BI_VER:
		assert(cu_width == 8 && cu_height == 16);
		IP_buffer_8x16.fetch_bi_ver(part_num, IP_buffer_ptr_16x16);
		IP_map_scu_8x16.fetch_bi_ver(part_num, IP_map_scu_ptr_16x16);
		MPM_buffer_8x16.fetch_bi_ver(part_num, MPM_buffer_ptr_16x16);
		fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
		me_line_map_16.fetch_bi_ver(part_num, me_line_map_ptr_16, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 16 && cu_height == 8);
		IP_buffer_16x8.fetch_bi_hor(part_num, IP_buffer_ptr_16x16);
		IP_map_scu_16x8.fetch_bi_hor(part_num, IP_map_scu_ptr_16x16);
		MPM_buffer_16x8.fetch_bi_hor(part_num, MPM_buffer_ptr_16x16);
		fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
		me_line_map_16.fetch_bi_hor(part_num, me_line_map_ptr_16, cu_width, cu_height);
		break;
	default: assert(0);
	}

#if ENABLE_BFRAME
	ME_MV_DIR_16 me_mv_dir_16;
#endif // ENABLE_BFRAME
	pel org_y_16[16][16];
	ENC_FME_INFO_ALL_16 fme_mv_info_16;
	strFetch_ref_window_16 Fetch_Ref_window_16;
	SKIP_MERGE_INPUT_16 skip_in_16;

	fetch_port_16_md2_tst(
#if ENABLE_BFRAME
		me_mv_dir,
#endif // ENABLE_BFRAME
		p_fenc_LCU_Y,
		//me_line_map_ptr,
		skip_in,
		fme_mv_info,
		Fetch_Ref_window_ptr,
		x0, y0, cu_width, cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height,
#if ENABLE_BFRAME
		& me_mv_dir_16,
#endif // ENABLE_BFRAME
		org_y_16,
		//&me_line_map_16,
		&skip_in_16,
		&fme_mv_info_16,
		&Fetch_Ref_window_16);

	STATIC_C IP_ARRAY_SIZE<16, 16> null_IP_buffer_ptr_16x16;
	STATIC_C MPM_ARRAY_SIZE<16, 16> null_MPM_buffer_ptr_16x16;
	STATIC_C IP_MAP_SCU_SIZE<16, 16> null_IP_map_scu_ptr_16x16;
	cost_temp = md_kernel_16(md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		& me_mv_dir_16,
#endif
		org_y_16,
		&me_line_map_16, up_nebs,
		left_nebs,
		&skip_in_16,
		boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
		cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&curr_cu_data,
		&fme_mv_info_16,
		&null_IP_buffer_ptr_16x16, &IP_buffer_8x16, &IP_buffer_16x8,
		&null_MPM_buffer_ptr_16x16, &MPM_buffer_8x16, &MPM_buffer_16x8,
		&null_IP_map_scu_ptr_16x16, &IP_map_scu_8x16, &IP_map_scu_16x8,
		ref,
		&Fetch_Ref_window_16
#if SUB_TMVP
		, skip_in
#endif
	);

	//compare_store_top(md_fw_ptr,
	//	cost_best, cost_temp,
	//	&curr_cu_data, NULL, &curr_cu_data_best, NULL, NULL, cu_width_log2, cu_height_log2, cud, tree_status,
	//	NO_SPLIT, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child[0], tree_status_child[0], &best_tree_status_child, &tree_status_temp, 3);
	cost_best = cost_temp;
	best_split_mode = NO_SPLIT;
	best_cons_pred_mode = cons_pred_mode_child[0];

	switch (split_mode)
	{
	case SPLIT_BI_VER:
		assert(cu_width == 8 && cu_height == 16);
		IP_buffer_8x16.update_bi_ver(part_num, curr_cu_data.reco_y);
		IP_map_scu_8x16.update_bi_ver(part_num, IP_map_scu_ptr_16x16->cur_map_scu, curr_cu_data.map_scu);
		MPM_buffer_8x16.update_bi_ver(part_num, curr_cu_data.ipm[0]);
		update_nebs_bi_ver(nebs_line_ver, curr_cu_data.refi, curr_cu_data.mv, cu_width, cu_height, part_num);
		me_line_map_16.update_bi_ver(part_num, curr_cu_data.map_scu, curr_cu_data.map_cu_mode, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 16 && cu_height == 8);
		IP_buffer_16x8.update_bi_hor(part_num, curr_cu_data.reco_y);
		IP_map_scu_16x8.update_bi_hor(part_num, IP_map_scu_ptr_16x16->cur_map_scu, curr_cu_data.map_scu);
		MPM_buffer_16x8.update_bi_hor(part_num, curr_cu_data.ipm[0]);
		update_nebs_bi_hor(nebs_line_hor, curr_cu_data.refi, curr_cu_data.mv, cu_width, cu_height, part_num);
		me_line_map_16.update_bi_hor(part_num, curr_cu_data.map_scu, curr_cu_data.map_cu_mode, cu_width, cu_height);
		break;
	default: assert(0);
	}

	prev_cu_data_ptr->get_cu_data_from_curblk(&curr_cu_data, (x0 - split_struct.x_pos[0]), (y0 - split_struct.y_pos[0]), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);

	//store_enc_info_best_16_top2(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
	//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}



static s64 md_16x16_16x8_8x16_loop_md2(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS INLINE off
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (isBTH_allow) {

		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);

		bth_cost = prepare_cu_info_16(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud, qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			bth_cost += circuit_16x16_16x8_8x16_md2(
				md_fw_ptr, md_input_ptr, g_rmd_output,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_16,
				up_nebs_father, left_nebs_father,
				skip_in,
				cu_data_ptr,
				qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0], child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				fme_mv_info,
				IP_buffer_ptr_16x16,
				MPM_buffer_ptr_16x16,
				IP_map_scu_ptr_16x16,

				ref,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return bth_cost;
}
void load_md2(u8 ip_list[IPD_RDO_CNT], RMD_OUTPUT* g_rmd_output, U3 cu_width_log2, U3 cu_height_log2, U14 x0, U14 y0, int split_mode_father, IP_ARRAY_SIZE<16, 16>& IP_buffer_16x16,
	U3 part_num, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_16x16, IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32,
	MPM_ARRAY_SIZE<16, 16>& MPM_buffer_16x16, MPM_ARRAY_SIZE<16, 32>* MPM_buffer_ptr_16x32, ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	ENC_ME_NEB_INFO up_nebs_father[1 + (32 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(32 >> 2)], ENC_ME_NEB_INFO	nebs_line_hor[4], U8 cu_width, U8 cu_height,
	ENC_ME_LINE_MAP_SIZE<16>& me_line_map_16, ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	MPM_ARRAY_SIZE<32, 16>* MPM_buffer_ptr_32x16, ENC_ME_NEB_INFO nebs_line_ver[4], ME_MV_DIR* me_mv_dir, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], SKIP_MERGE_INPUT* skip_in,
	ENC_FME_INFO_ALL* fme_mv_info, strFetch_ref_window* Fetch_Ref_window_ptr, MD_FW* md_fw_ptr, ME_MV_DIR_16& me_mv_dir_16, pel org_y_16[16][16], ENC_FME_INFO_ALL_16& fme_mv_info_16,
	strFetch_ref_window_16& Fetch_Ref_window_16, SKIP_MERGE_INPUT_16& skip_in_16)
{

#if USE_SPEED_LEVEL
	//static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	//static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;
	S7 ip_array_size_idx;

	switch (split_mode_father)
	{
	case SPLIT_BI_HOR:
		IP_buffer_16x16.fetch_bi_hor(part_num, IP_buffer_ptr_16x32);
		IP_map_scu_16x16.fetch_bi_hor(part_num, IP_map_scu_ptr_16x32);
		MPM_buffer_16x16.fetch_bi_hor(part_num, MPM_buffer_ptr_16x32);
		fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
		me_line_map_16.fetch_bi_hor(part_num, me_line_map_ptr_32, cu_width, cu_height);
		break;
	case SPLIT_BI_VER:
		IP_buffer_16x16.fetch_bi_ver(part_num, IP_buffer_ptr_32x16);
		IP_map_scu_16x16.fetch_bi_ver(part_num, IP_map_scu_ptr_32x16);
		MPM_buffer_16x16.fetch_bi_ver(part_num, MPM_buffer_ptr_32x16);
		fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
		me_line_map_16.fetch_bi_ver(part_num, me_line_map_ptr_32, cu_width, cu_height);
		break;
	default: assert(0);
	}


	fetch_port_16_md2_tst(
#if ENABLE_BFRAME
		me_mv_dir,
#endif
		p_fenc_LCU_Y,
		//me_line_map_ptr,
		skip_in,
		fme_mv_info,
		Fetch_Ref_window_ptr,
		x0, y0, cu_width, cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height,
#if ENABLE_BFRAME
		& me_mv_dir_16,
#endif
		org_y_16,
		//&me_line_map_16,
		&skip_in_16,
		&fme_mv_info_16,
		&Fetch_Ref_window_16);
};


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
)
{
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_best_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY null_curr_cu_data_best_ptr2;
	compare_store_top(md_fw_ptr, cost_best, cost0, &curr_cu_data_ptr0, &null_curr_cu_data_ptr_32, &curr_cu_data_best_ptr, &null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2,
		cu_width_log2, cu_height_log2, cud, tree_status,
		NO_SPLIT, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child0, tree_status_child0, &best_child_status, &tree_status_temp, 3);

	compare_store_top(md_fw_ptr, cost_best, cost1, &curr_cu_data_ptr1, &null_curr_cu_data_ptr_32, &curr_cu_data_best_ptr, &null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2,
		cu_width_log2, cu_height_log2, cud, tree_status,
		SPLIT_BI_VER, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child1, tree_status_child1, &best_child_status, &tree_status_temp, 3);

	compare_store_top(md_fw_ptr, cost_best, cost2, &curr_cu_data_ptr2, &null_curr_cu_data_ptr_32, &curr_cu_data_best_ptr, &null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2,
		cu_width_log2, cu_height_log2, cud, tree_status,
		SPLIT_BI_HOR, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child2, tree_status_child2, &best_child_status, &tree_status_temp, 3);

	switch (split_mode_father)
	{
	case SPLIT_BI_HOR:
		IP_buffer_16x16.update_bi_hor(part_num, curr_cu_data_best_ptr.reco_y);
		IP_map_scu_16x16.update_bi_hor(part_num, IP_map_scu_ptr_16x32->cur_map_scu, curr_cu_data_best_ptr.map_scu);
		MPM_buffer_16x16.update_bi_hor(part_num, curr_cu_data_best_ptr.ipm[0]);
		update_nebs_bi_hor(nebs_line_hor, curr_cu_data_best_ptr.refi, curr_cu_data_best_ptr.mv, cu_width, cu_height, part_num);
		me_line_map_16.update_bi_hor(part_num, curr_cu_data_best_ptr.map_scu, curr_cu_data_best_ptr.map_cu_mode, cu_width, cu_height);
		break;
	case SPLIT_BI_VER:
		IP_buffer_16x16.update_bi_ver(part_num, curr_cu_data_best_ptr.reco_y);
		IP_map_scu_16x16.update_bi_ver(part_num, IP_map_scu_ptr_32x16->cur_map_scu, curr_cu_data_best_ptr.map_scu);
		MPM_buffer_16x16.update_bi_ver(part_num, curr_cu_data_best_ptr.ipm[0]);
		update_nebs_bi_ver(nebs_line_ver, curr_cu_data_best_ptr.refi, curr_cu_data_best_ptr.mv, cu_width, cu_height, part_num);
		me_line_map_16.update_bi_ver(part_num, curr_cu_data_best_ptr.map_scu, curr_cu_data_best_ptr.map_cu_mode, cu_width, cu_height);
		break;
	default: assert(0);
	}



	U8 split_mode[5][7][256];
#pragma HLS ARRAY_PARTITION variable=split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=split_mode cyclic factor=16 dim=3	

	for (int i = 0; i < 5; i++)
	{
#pragma HLS PIPELINE II=1
		for (int j = 0; j < 7; j++)
		{
			for (int k = 0; k < 256; k++)
			{
				split_mode[i][j][k] = NO_SPLIT;
			}
		}
	}

	com_set_split_mode(*best_split_mode0, cud, 0, cu_width, cu_height, cu_width, split_mode);
	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[cu_width] - 2)) + (cu_width >> 1 >> 2));
	U3 shape = SQUARE + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, *best_split_mode0, md_fw_ptr->slice_type))
	{
		com_set_cons_pred_mode(best_cons_pred_mode, cud, 0, cu_width, cu_height, cu_width, split_mode);
	}

	int x_cur_cu_scu = (x0 - x_pos) >> 2;
	int y_cur_cu_scu = (y0 - y_pos) >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);
#if STORE_ENC_INFO_BEST_16_TOP
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2	
#endif
	for (int i = 0; i < cuh_scu; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		int idx_dst = ((y_cur_cu_scu + i) << (prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (cu_width_log2 - 2);
		for (int j = 0; j < cuw_scu; j++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16	
#pragma HLS PIPELINE II=1	
			for (int m = 0; m < 5; m++)
			{
#pragma HLS LOOP_TRIPCOUNT max=5
				if (m >= cud)
				{
					for (int n = 0; n < (NUM_BLOCK_SHAPE); n++)
					{
						(prev_cu_data_ptr->split_mode[m][n] + idx_dst)[j] = (split_mode[m][n] + idx_src)[j];
					}
				}
			}
		}
	}

	prev_cu_data_ptr->get_cu_data_from_curblk(curr_cu_data_best, (x0 - x_pos), (y0 - y_pos), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);

	//copy_cu_data_array_dst_16_top(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
	//	cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status, *best_split_mode, shape,pos, split_mode);

	//update_map_scu_16_top(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);

};
#include<md_cmod.h>

void pu_test_0(s64& cost_temp, U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR_16& me_mv_dir,
#endif
	pel p_fenc_LCU_Y[16][16],
	ENC_ME_LINE_MAP_SIZE<16>& me_line_map_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16& skip_in_16,
	U1 boundary, U1 isNS_allow, U2& NS_luma_pred_mode, U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	ENC_CU_DATA_ARRAY_SIZE<16>& cu_data_temp_ptr_16,
	ENC_FME_INFO_ALL_16& fme_mv_info,
	IP_ARRAY_SIZE<16, 16>& IP_buffer_ptr_16x16, IP_ARRAY_SIZE<8, 16>& IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>& IP_buffer_ptr_16x8,
	MPM_ARRAY_SIZE<16, 16>& MPM_buffer_ptr_16x16, MPM_ARRAY_SIZE<8, 16>& MPM_buffer_ptr_8x16, MPM_ARRAY_SIZE<16, 8>& MPM_buffer_ptr_16x8,
	IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_ptr_16x16, IP_MAP_SCU_SIZE<8, 16>& IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>& IP_map_scu_ptr_16x8,
	Arbitor& ref,
	strFetch_ref_window_16& Fetch_Ref_window_ptr
#if SUB_TMVP
	, SKIP_MERGE_INPUT* skip_in
#endif


)
{
	if (split_mode == NO_SPLIT)
	{
		cost_temp = md_kernel_16(md_fw_ptr, md_input_ptr, rmd_mode,
#if ENABLE_BFRAME
			& me_mv_dir,
#endif
			p_fenc_LCU_Y,
			&me_line_map_16, up_nebs,
			left_nebs,
			&skip_in_16,
			boundary, isNS_allow, &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
			cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
			&cu_data_temp_ptr_16,
			&fme_mv_info,
			&IP_buffer_ptr_16x16, &IP_buffer_ptr_8x16, &IP_buffer_ptr_16x8,
			&MPM_buffer_ptr_16x16, &MPM_buffer_ptr_8x16, &MPM_buffer_ptr_16x8,
			&IP_map_scu_ptr_16x16, &IP_map_scu_ptr_8x16, &IP_map_scu_ptr_16x8,
			ref,
			&Fetch_Ref_window_ptr
#if SUB_TMVP
			, skip_in
#endif
		);
	}

	if (split_mode == SPLIT_BI_VER)
	{
		cost_temp = md_kernel_16(md_fw_ptr, md_input_ptr, rmd_mode,
#if ENABLE_BFRAME
			& me_mv_dir,
#endif
			p_fenc_LCU_Y,
			&me_line_map_16, up_nebs,
			left_nebs,
			&skip_in_16,
			boundary, isNS_allow, &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
			cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
			&cu_data_temp_ptr_16,
			&fme_mv_info,
			&IP_buffer_ptr_16x16, &IP_buffer_ptr_8x16, &IP_buffer_ptr_16x8,
			&MPM_buffer_ptr_16x16, &MPM_buffer_ptr_8x16, &MPM_buffer_ptr_16x8,
			&IP_map_scu_ptr_16x16, &IP_map_scu_ptr_8x16, &IP_map_scu_ptr_16x8,
			ref,
			&Fetch_Ref_window_ptr
#if SUB_TMVP
			, skip_in
#endif
		);
	}
	if (split_mode == SPLIT_BI_HOR)
	{
		cost_temp = md_kernel_16(md_fw_ptr, md_input_ptr, rmd_mode,
#if ENABLE_BFRAME
			& me_mv_dir,
#endif
			p_fenc_LCU_Y,
			&me_line_map_16, up_nebs,
			left_nebs,
			&skip_in_16,
			boundary, isNS_allow, &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
			cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
			&cu_data_temp_ptr_16,
			&fme_mv_info,
			&IP_buffer_ptr_16x16, &IP_buffer_ptr_8x16, &IP_buffer_ptr_16x8,
			&MPM_buffer_ptr_16x16, &MPM_buffer_ptr_8x16, &MPM_buffer_ptr_16x8,
			&IP_map_scu_ptr_16x16, &IP_map_scu_ptr_8x16, &IP_map_scu_ptr_16x8,
			ref,
			&Fetch_Ref_window_ptr
#if SUB_TMVP
			, skip_in
#endif
		);
	}

	if (split_mode == SPLIT_QUAD)
	{
		cost_temp = md_kernel_16(md_fw_ptr, md_input_ptr, rmd_mode,
#if ENABLE_BFRAME
			& me_mv_dir,
#endif
			p_fenc_LCU_Y,
			&me_line_map_16, up_nebs,
			left_nebs,
			&skip_in_16,
			boundary, isNS_allow, &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
			cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
			&cu_data_temp_ptr_16,
			&fme_mv_info,
			&IP_buffer_ptr_16x16, &IP_buffer_ptr_8x16, &IP_buffer_ptr_16x8,
			&MPM_buffer_ptr_16x16, &MPM_buffer_ptr_8x16, &MPM_buffer_ptr_16x8,
			&IP_map_scu_ptr_16x16, &IP_map_scu_ptr_8x16, &IP_map_scu_ptr_16x8,
			ref,
			&Fetch_Ref_window_ptr
#if SUB_TMVP
			, skip_in
#endif
		);
	}
};
void pu_test_1(s64& cost_temp, U3 split_mode,
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
	IP_ARRAY_SIZE<16, 16>& IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>& MPM_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<16, 16>& IP_map_scu_ptr_16x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	cost_temp = md_16x16_16x8_8x16_loop_md2(split_mode,
		md_fw_ptr, md_input_ptr, g_rmd_output,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_16,
		up_nebs, left_nebs,
		skip_in,
		isBTH_allow, x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, cu_width, cu_height, cons_flag,
		&cu_data_ptr,
		&cons_pred_mode_BTH, tree_status, &child_tree_status,
		fme_mv_info,
		&IP_buffer_ptr_16x16,
		&MPM_buffer_ptr_16x16,
		&IP_map_scu_ptr_16x16,
		ref,
		Fetch_Ref_window_ptr);
};
void pu_1(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref,
	COM_SPLIT_STRUCT split_struct, U3 part_num, ENC_ME_NEB_INFO up_nebs[6], ENC_ME_NEB_INFO left_nebs[6], U3 split_mode)
{
	//split_struct.split_allow[part_num][SPLIT_BI_VER]
	if (split_mode == NO_SPLIT)
	{
		out_buf.cost_temp = md_16x16_16x8_8x16_loop_md2(NO_SPLIT,
			&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output,
			in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
			in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
			in_buf.buffer.p_fenc_LCU_Y,
			&in_buf.me_line_map_16,
			up_nebs, left_nebs,
			&in_buf.buffer.skip_in,

			1,
			in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth,
			in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
			&out_buf.curr_cu_data_temp,
			&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child,
			&in_buf.buffer.fme_mv_info,
			&in_buf.IP_buffer_16x16,
			&in_buf.MPM_buffer_16x16,
			&in_buf.IP_map_scu_16x16,
			ref,
			&in_buf.buffer.Fetch_Ref_window_ptr);
	}

	if (split_mode == SPLIT_BI_VER)
	{
		out_buf.cost_temp = md_16x16_16x8_8x16_loop_md2(SPLIT_BI_VER,
			&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output,
			in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
			in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
			in_buf.buffer.p_fenc_LCU_Y,
			&in_buf.me_line_map_16,
			up_nebs, left_nebs,
			&in_buf.buffer.skip_in,

			1,
			in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth,
			in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
			&out_buf.curr_cu_data_temp,
			&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child,
			&in_buf.buffer.fme_mv_info,
			&in_buf.IP_buffer_16x16,
			&in_buf.MPM_buffer_16x16,
			&in_buf.IP_map_scu_16x16,
			ref,
			&in_buf.buffer.Fetch_Ref_window_ptr);
	}

	if (split_mode == SPLIT_BI_HOR)
	{
		out_buf.cost_temp = md_16x16_16x8_8x16_loop_md2(SPLIT_BI_HOR,
			&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output,
			in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
			in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
			in_buf.buffer.p_fenc_LCU_Y,
			&in_buf.me_line_map_16,
			up_nebs, left_nebs,
			&in_buf.buffer.skip_in,

			1,
			in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth,
			in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
			&out_buf.curr_cu_data_temp,
			&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child,
			&in_buf.buffer.fme_mv_info,
			&in_buf.IP_buffer_16x16,
			&in_buf.MPM_buffer_16x16,
			&in_buf.IP_map_scu_16x16,
			ref,
			&in_buf.buffer.Fetch_Ref_window_ptr);
	}

	if (split_mode == SPLIT_QUAD)
	{
		out_buf.cost_temp = md_16x16_16x8_8x16_loop_md2(SPLIT_QUAD,
			&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output,
			in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
			in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
			in_buf.buffer.p_fenc_LCU_Y,
			&in_buf.me_line_map_16,
			up_nebs, left_nebs,
			&in_buf.buffer.skip_in,

			1,
			in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth,
			in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
			&out_buf.curr_cu_data_temp,
			&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child,
			&in_buf.buffer.fme_mv_info,
			&in_buf.IP_buffer_16x16,
			&in_buf.MPM_buffer_16x16,
			&in_buf.IP_map_scu_16x16,
			ref,
			&in_buf.buffer.Fetch_Ref_window_ptr);
	}


};

#if !ONLY_QT_QT
s64 circuit_16x16_top_md2(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO up_nebs_father[1 + (32 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(32 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	MPM_ARRAY_SIZE<16, 32>* MPM_buffer_ptr_16x32, MPM_ARRAY_SIZE<32, 16>* MPM_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father)
{
#pragma HLS INLINE off
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
	//allowed split by normative and non-normative selection

	s64 cost_best = MAX_S64;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	assert(cu_width == 16 && cu_height == 16);

   	STATIC_C Channel channel;
   	STATIC_C InstructionLoad insn;
	STATIC_C Load2PU pu_in_buf[PU_NUM];

   	channel.split_mode_father = split_mode_father;
	channel.IP_buffer_ptr_16x32 = *IP_buffer_ptr_16x32;
	channel.IP_map_scu_ptr_16x32 = *IP_map_scu_ptr_16x32;
	channel.MPM_buffer_ptr_16x32 = *MPM_buffer_ptr_16x32;
	//memcpy(channel.up_nebs_32, up_nebs_father, sizeof(channel.up_nebs_32));
	//memcpy(channel.left_nebs_32, left_nebs_father, sizeof(channel.left_nebs_32));
	for (size_t i = 0; i < 10; i++) {
		channel.up_nebs_32[i] = up_nebs_father[i];
	}
	for (size_t i = 0; i < 8; i++) {
		channel.left_nebs_32[i] = left_nebs_father[i];
	}
	
	channel.me_line_map_ptr_32 = *me_line_map_ptr_32;
	channel.IP_buffer_ptr_32x16 = *IP_buffer_ptr_32x16;
	channel.IP_map_scu_ptr_32x16 = *IP_map_scu_ptr_32x16;
	channel.MPM_buffer_ptr_32x16 = *MPM_buffer_ptr_32x16;
	channel.IP_buffer_ptr_16x32 = *IP_buffer_ptr_16x32;
	channel.IP_map_scu_ptr_16x32 = *IP_map_scu_ptr_16x32;
	channel.MPM_buffer_ptr_16x32 = *MPM_buffer_ptr_16x32;
	channel.boundary = boundary;
	insn.part_num = part_num;

	for (int i = 0; i < 3; i++) {
		pu_in_buf[i].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[i].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[i].buffer.g_rmd_output = *g_rmd_output;
		pu_in_buf[i].buffer.me_mv_dir = *me_mv_dir;
		//memcpy(pu_in_buf[i].buffer.p_fenc_LCU_Y, p_fenc_LCU_Y, sizeof(pu_in_buf[i].buffer.p_fenc_LCU_Y));
		for (size_t j = 0; j < MAX_CU_SIZE_FETCH; j++) {
			for (size_t k = 0; k < MAX_CU_SIZE_FETCH; k++) {
				pu_in_buf[i].buffer.p_fenc_LCU_Y[j][k] = p_fenc_LCU_Y[j][k];
			}
		}
		
		pu_in_buf[i].buffer.skip_in = *skip_in;
		pu_in_buf[i].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[i].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
   		pu_in_buf[i].insn.cu_width_log2 = cu_width_log2;
   		pu_in_buf[i].insn.cu_height_log2 = cu_height_log2;
   		pu_in_buf[i].insn.x0 = x0;
   		pu_in_buf[i].insn.y0 = y0;
		pu_in_buf[i].insn.cu_width = cu_width;
		pu_in_buf[i].insn.cu_height = cu_height;
		pu_in_buf[i].boundary = channel.boundary;
		pu_in_buf[i].insn.cup = cup;
		pu_in_buf[i].insn.cud = cud;
		pu_in_buf[i].insn.qt_depth = qt_depth;
		pu_in_buf[i].insn.bet_depth = bet_depth;
		pu_in_buf[i].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[i].insn.tree_status = tree_status;
		load_line_buffer_16x16_md2(pu_in_buf[i].insn, pu_in_buf[i], channel, insn, pu_in_buf[i].config, pu_in_buf[i].buffer);

		//memcpy(pu_in_buf[i].buffer.RefWindowLCU_UP, RefWindowLCU_UP, sizeof(pu_in_buf[i].buffer.RefWindowLCU_UP));
		//memcpy(pu_in_buf[i].buffer.RefWindowLCU_VP, RefWindowLCU_VP, sizeof(pu_in_buf[i].buffer.RefWindowLCU_VP));
		//memcpy(pu_in_buf[i].buffer.RefWindowLCU_UP_ref1, RefWindowLCU_UP_ref1, sizeof(pu_in_buf[i].buffer.RefWindowLCU_UP_ref1));
		//memcpy(pu_in_buf[i].buffer.RefWindowLCU_VP_ref1, RefWindowLCU_VP_ref1, sizeof(pu_in_buf[i].buffer.RefWindowLCU_VP_ref1));
		for (size_t j = 0; j < (SWH * SWW) >> 2; j++) {
			pu_in_buf[i].buffer.RefWindowLCU_UP[j] = RefWindowLCU_UP[j];
			pu_in_buf[i].buffer.RefWindowLCU_VP[j] = RefWindowLCU_VP[j];
			pu_in_buf[i].buffer.RefWindowLCU_UP_ref1[j] = RefWindowLCU_UP_ref1[j];
			pu_in_buf[i].buffer.RefWindowLCU_VP_ref1[j] = RefWindowLCU_VP_ref1[j];
		}

		//memcpy(pu_in_buf[i].buffer.CtrPos, CtrPos, sizeof(pu_in_buf[i].buffer.CtrPos));
		//memcpy(pu_in_buf[i].buffer.CtrPos_ref1, CtrPos_ref1, sizeof(pu_in_buf[i].buffer.CtrPos_ref1));
		for (size_t j = 0; j < MAX_CU_SIZE_FETCH; j++) {
			for (size_t k = 0; k < MAX_CU_SIZE_FETCH; k++) {
				for (size_t l = 0; l < MAX_CU_SIZE_FETCH; l++) {
					pu_in_buf[i].buffer.CtrPos[j][k][l] = CtrPos[j][k][l];
					pu_in_buf[i].buffer.CtrPos_ref1[j][k][l] = CtrPos_ref1[j][k][l];
				}
			}
		}
	}

	STATIC_C PU2Store pu_out_buf[PU_NUM];

	// pu<KERNEL_16X16>
	pu_out_buf[0].cost_temp = MAX_S64;
#if !KERNEL_SIZE_CLOSE(16, 16)
	if (split_struct.split_allow[part_num][NO_SPLIT])
	{
		 pu_16x16_md2(pu_in_buf[0], pu_out_buf[0], ref);
	}
#endif // !KERNEL_SIZE_CLOSE(16, 16)
		//test

		// pu<KERNEL_8X16>
	pu_out_buf[1].cost_temp = MAX_S64;
	if (split_struct.split_allow[part_num][SPLIT_BI_VER])
	{
#if !size_8x16_md2_close
	    pu_16x16_8x16_md2(pu_in_buf[1], pu_out_buf[1], ref);
#endif // size_8x16_md2_close

	}

	pu_out_buf[2].cost_temp = MAX_S64;
	if (split_struct.split_allow[part_num][SPLIT_BI_HOR])
	{
#if !size_16x8_md2_close
	    pu_16x16_16x8_md2(pu_in_buf[2], pu_out_buf[2], ref);
#endif // !size_16x8_md2_close
	}

	STATIC_C InstructionStore insn_load;
	insn_load.cu_width_log2 = cu_width_log2;
   	insn_load.cu_height_log2 = cu_height_log2;
	insn_load.cud = cud;
   	insn_load.tree_status = tree_status;
   	insn_load.part_num = part_num;
  	//SWITCH
	insn_load.cu_width = cu_width;
   	insn_load.cu_height = cu_height;
	insn_load.x0 = x0;
   	insn_load.y0 = y0;
   	insn_load.cup = cup;
   	insn_load.cons_pred_mode = cons_pred_mode;
	insn_load.x_pos = split_struct.x_pos[0];
   	insn_load.y_pos = split_struct.y_pos[0];
   	insn_load.prev_up_log2_sub_cuw = prev_up_log2_sub_cuw;

	store_n_1_16x16_md2(pu_in_buf[0].config, pu_out_buf, insn_load, channel);

   	cost_best = channel.cost_best[(insn_load.y0-insn_load.y_pos)>>MIN_CU_LOG2][(insn_load.x0-insn_load.x_pos) >> MIN_CU_LOG2];
	*IP_map_scu_ptr_16x32 = channel.IP_map_scu_ptr_16x32;
	*IP_map_scu_ptr_32x16 = channel.IP_map_scu_ptr_32x16;
	*prev_cu_data_ptr = channel.prev_cu_data_ptr;

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif
	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

s64 md_16x16_loop_md2(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO up_nebs_father[1 + (32 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(32 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	U1 isBTH_allow, U13 x0, U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	MPM_ARRAY_SIZE<16, 32>* MPM_buffer_ptr_16x32, MPM_ARRAY_SIZE<32, 16>* MPM_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	U3 split_mode = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	if (isBTH_allow) {

		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);
		bth_cost = prepare_cu_info_COM_combine_only_bt<32>(md_fw_ptr, md_input_ptr->lambda_y, cu_data_temp_ptr, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode, cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2

			bth_cost += circuit_16x16_top_md2(
				md_fw_ptr, md_input_ptr, g_rmd_output,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_32,
				up_nebs_father, left_nebs_father,
				skip_in,
				cu_data_temp_ptr, qt_depth, bet_depth + 1, cons_pred_mode_BTH[0], child_tree_status[0], part_num, split_struct,
				cu_width_log2, cu_height_log2, fme_mv_info,
				IP_buffer_ptr_16x32, IP_buffer_ptr_32x16,
				MPM_buffer_ptr_16x32, MPM_buffer_ptr_32x16,
				IP_map_scu_ptr_16x32, IP_map_scu_ptr_32x16,
				ref,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return bth_cost;
}
#endif

void load_64x32_32x64_64x16_16x64(
	U14 x0,
	U14 y0,
	U8 cu_width,
	U8 cu_height,
	U3 part_num,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32,
	IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	int split_mode_father,
	int ns_64x64,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_16x64,
	IP_ARRAY_SIZE<64, 16>* IP_buffer_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_16x64,
	IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_64x16,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_16x64,
	MPM_ARRAY_SIZE<64, 16>* MPM_buffer_64x16,
	ENC_ME_NEB_INFO				up_nebs[1 + (64 >> 2) + 1],
	ENC_ME_NEB_INFO				left_nebs[64 >> 2],
	ENC_ME_NEB_INFO				nebs_line_ver[16],
	ENC_ME_NEB_INFO				nebs_line_hor[16],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_64)
{

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;

	switch (ns_64x64)
	{
	case 0:
		switch (split_mode_father)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 16 && cu_height == 64);
			IP_buffer_16x64->fetch_bi_ver(part_num, IP_buffer_ptr_32x64);
			MPM_buffer_16x64->fetch_bi_ver(part_num, MPM_buffer_ptr_32x64);
			IP_map_scu_16x64->fetch_bi_ver(part_num, IP_map_scu_ptr_32x64);
			fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
			me_line_map_64->fetch_bi_ver(part_num, me_line_map_ptr_64, cu_width, cu_height);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 64 && cu_height == 16);
			IP_buffer_64x16->fetch_bi_hor(part_num, IP_buffer_ptr_64x32);
			MPM_buffer_64x16->fetch_bi_hor(part_num, MPM_buffer_ptr_64x32);
			IP_map_scu_64x16->fetch_bi_hor(part_num, IP_map_scu_ptr_64x32);
			fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
			me_line_map_64->fetch_bi_hor(part_num, me_line_map_ptr_64, cu_width, cu_height);
			break;
		default:
			assert(0);
		}
		break;
	case 1:
		assert(cu_width == 64 && cu_height == 64);
		break;
	default:
		assert(0);
	}
	if (ns_64x64 || split_mode_father == NO_SPLIT)
	{
		for (int i = 0; i < (cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
			cpy_1d_2_1d<S2, REFP_NUM>(up_nebs_father[i].refi, up_nebs[i].refi);
			cpy_2d_2_2d<S14, REFP_NUM, MV_D>(up_nebs_father[i].mv, up_nebs[i].mv);
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			cpy_1d_2_1d<S2, REFP_NUM>(left_nebs_father[i].refi, left_nebs[i].refi);
			cpy_2d_2_2d<S14, REFP_NUM, MV_D>(left_nebs_father[i].mv, left_nebs[i].mv);
		}

		cpy_1d_2_1d<U32, 18>(me_line_map_ptr_64->map_scu_up_line, me_line_map_64->map_scu_up_line);
		cpy_1d_2_1d<U32, 18>(me_line_map_ptr_64->map_cu_mode_up_line, me_line_map_64->map_cu_mode_up_line);
		cpy_1d_2_1d<U32, 16>(me_line_map_ptr_64->map_scu_left_line, me_line_map_64->map_scu_left_line);
		cpy_1d_2_1d<U32, 16>(me_line_map_ptr_64->map_cu_mode_left_line, me_line_map_64->map_cu_mode_left_line);
		me_line_map_64->min_scu_x = me_line_map_ptr_64->min_scu_x;
		me_line_map_64->min_scu_y = me_line_map_ptr_64->min_scu_y;
		me_line_map_64->above_line_idx = me_line_map_ptr_64->above_line_idx;
		me_line_map_64->curr_ctu_idx = me_line_map_ptr_64->curr_ctu_idx;
	}
}

s64 store_64x32_32x64_64x16_16x64(
	MD_FW* md_fw_ptr,
	ENC_CU_DATA_ARRAY* prev_cu_data_ptr,
	U2 cons_pred_mode, U2 tree_status, U3 part_num,
	U3 prev_up_log2_sub_cuw,
	int split_mode_father,
	int ns_64x64,
	U3 cu_width_log2,
	U3 cu_height_log2,
	U8 cu_width,
	U8 cu_height,
	U12 cup,
	U4 cud,
	U14 x0,
	U14 y0,
	U3 num_split_to_try,
	s64 cost[3],
	U2 tree_status_child[2],
	U2 cons_pred_mode_child[2],
	U3 split_mode[3],
	U14      split_struct_x_pos_0,
	U14      split_struct_y_pos_0,
	ENC_CU_DATA_ARRAY_SIZE<64>	curr_cu_data_temp[2],
	U3& best_split_mode,
	U2& best_cons_pred_mode,
	s64& cost_best,
	U2& best_tree_status_child,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_16x64,
	IP_ARRAY_SIZE<64, 16>* IP_buffer_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_16x64,
	IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_64x16,
	IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64,
	IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_16x64,
	MPM_ARRAY_SIZE<64, 16>* MPM_buffer_64x16,
	ENC_ME_NEB_INFO				nebs_line_ver[16],
	ENC_ME_NEB_INFO				nebs_line_hor[16],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_64,
	ENC_CU_DATA_ARRAY* curr_cu_data_best)
{
	U2 tree_status_temp = tree_status;

	cost_compare_fixed_point_one_time<64>(md_fw_ptr, &cost_best, cost, curr_cu_data_temp, curr_cu_data_best, cu_width_log2, cu_height_log2, cud, tree_status,
		split_mode, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child, tree_status_child, &best_tree_status_child, &tree_status_temp, 3);

	switch (ns_64x64)
	{
	case 0:
		switch (split_mode_father)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 16 && cu_height == 64);
			IP_buffer_16x64->update_bi_ver(part_num, curr_cu_data_best->reco_y);
			IP_map_scu_16x64->update_bi_ver(part_num, IP_map_scu_ptr_32x64->cur_map_scu, curr_cu_data_best->map_scu);
			MPM_buffer_16x64->update_bi_ver(part_num, curr_cu_data_best->ipm[0]);
			update_nebs_bi_ver(nebs_line_ver, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
			me_line_map_64->update_bi_ver(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 64 && cu_height == 16);
			IP_buffer_64x16->update_bi_hor(part_num, curr_cu_data_best->reco_y);
			IP_map_scu_64x16->update_bi_hor(part_num, IP_map_scu_ptr_64x32->cur_map_scu, curr_cu_data_best->map_scu);
			MPM_buffer_64x16->update_bi_hor(part_num, curr_cu_data_best->ipm[0]);
			update_nebs_bi_hor(nebs_line_hor, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
			me_line_map_64->update_bi_hor(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
			break;
		default:
			assert(0);
		}
		break;
	case 1:
		break;
	default:
		assert(0);
	}

	store_enc_info_best(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_cu_data_ptr,
		split_struct_x_pos_0, split_struct_y_pos_0, prev_up_log2_sub_cuw);

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

void load_64x16_16x64_32x16_16x32_md2(
	U14 x0,
	U14 y0,
	U8 cu_width,
	U8 cu_height,
	U3 part_num,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	int split_mode_father,
	IP_ARRAY_SIZE<16, 32>*		IP_buffer_16x32,
	IP_ARRAY_SIZE<32, 16>*		IP_buffer_32x16,
	IP_MAP_SCU_SIZE<16, 32>*	IP_map_scu_16x32,
	IP_MAP_SCU_SIZE<32, 16>*	IP_map_scu_32x16,
	MPM_ARRAY_SIZE<16, 32>*		MPM_buffer_16x32,
	MPM_ARRAY_SIZE<32, 16>*		MPM_buffer_32x16,
	ENC_ME_NEB_INFO				up_nebs[1 + 8 + 1],
	ENC_ME_NEB_INFO				left_nebs[8],
	ENC_ME_NEB_INFO				nebs_line_ver[8],
	ENC_ME_NEB_INFO				nebs_line_hor[8],
	ENC_ME_LINE_MAP_SIZE<32>*		me_line_map_32)
{

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;
	S7 ip_array_size_idx;

	switch (split_mode_father)
	{
	case SPLIT_BI_VER:
		assert(cu_width == 32 && cu_height == 16);
		IP_buffer_32x16->fetch_bi_ver(part_num, IP_buffer_ptr_64x16);
		MPM_buffer_32x16->fetch_bi_ver(part_num, MPM_buffer_ptr_64x16);
		IP_map_scu_32x16->fetch_bi_ver(part_num, IP_map_scu_ptr_64x16);
		fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
		me_line_map_32->fetch_bi_ver(part_num, me_line_map_ptr_64, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 16 && cu_height == 32);
		IP_buffer_16x32->fetch_bi_hor(part_num, IP_buffer_ptr_16x64);
		MPM_buffer_16x32->fetch_bi_hor(part_num, MPM_buffer_ptr_16x64);
		IP_map_scu_16x32->fetch_bi_hor(part_num, IP_map_scu_ptr_16x64);
		fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
		me_line_map_32->fetch_bi_hor(part_num, me_line_map_ptr_64, cu_width, cu_height);
		break;
	default:
		assert(0);
	}
}

s64 store_64x16_16x64_32x16_16x32_md2(
	MD_FW* md_fw_ptr,
	ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr,
    U2 cons_pred_mode, U2 tree_status, U3 part_num,
	U3 prev_up_log2_sub_cuw,
	int split_mode_father,
	U3 cu_width_log2,
	U3 cu_height_log2,
	U8 cu_width,
	U8 cu_height,
	U12 cup,
	U4 cud,
	U14 x0,
	U14 y0,
	U3 num_split_to_try,
	s64 cost[3],
	U2 tree_status_child[2],
	U2 cons_pred_mode_child[2],
	U3 split_mode[3],
    U14      split_struct_x_pos_0,
    U14      split_struct_y_pos_0,
	ENC_CU_DATA_ARRAY_SIZE<32>	curr_cu_data_temp[2],
	U3 &best_split_mode,
	U2 &best_cons_pred_mode,
	s64 &cost_best,
	U2 &best_tree_status_child,
	IP_ARRAY_SIZE<16, 32>*		IP_buffer_16x32,
	IP_ARRAY_SIZE<32, 16>*		IP_buffer_32x16,
	IP_MAP_SCU_SIZE<16, 32>*	IP_map_scu_16x32,
	IP_MAP_SCU_SIZE<32, 16>*	IP_map_scu_32x16,
	IP_MAP_SCU_SIZE<16, 64>*    IP_map_scu_ptr_16x64, 
	IP_MAP_SCU_SIZE<64, 16>*    IP_map_scu_ptr_64x16,
	MPM_ARRAY_SIZE<16, 32>*		MPM_buffer_16x32,
	MPM_ARRAY_SIZE<32, 16>*		MPM_buffer_32x16,
	ENC_ME_NEB_INFO				nebs_line_ver[8],
	ENC_ME_NEB_INFO				nebs_line_hor[8],
	ENC_ME_LINE_MAP_SIZE<32>*		me_line_map_32,
	ENC_CU_DATA_ARRAY_SIZE<32>*	curr_cu_data_best)
{
	//store
	U2 tree_status_temp = tree_status;
	cost_compare_fixed_point_one_time<32>(md_fw_ptr, &cost_best, cost, curr_cu_data_temp, curr_cu_data_best, cu_width_log2, cu_height_log2, cud, tree_status,
		split_mode, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child, tree_status_child, &best_tree_status_child, &tree_status_temp, 2);

	switch (split_mode_father)
	{
	case SPLIT_BI_VER:
		assert(cu_width == 32 && cu_height == 16);
		IP_buffer_32x16->update_bi_ver(part_num, curr_cu_data_best->reco_y);
		IP_map_scu_32x16->update_bi_ver(part_num, IP_map_scu_ptr_64x16->cur_map_scu, curr_cu_data_best->map_scu);
		MPM_buffer_32x16->update_bi_ver(part_num, curr_cu_data_best->ipm[0]);
		update_nebs_bi_ver(nebs_line_ver, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
		me_line_map_32->update_bi_ver(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 16 && cu_height == 32);
		IP_buffer_16x32->update_bi_hor(part_num, curr_cu_data_best->reco_y);
		IP_map_scu_16x32->update_bi_hor(part_num, IP_map_scu_ptr_16x64->cur_map_scu, curr_cu_data_best->map_scu);
		MPM_buffer_16x32->update_bi_hor(part_num, curr_cu_data_best->ipm[0]);
		update_nebs_bi_hor(nebs_line_hor, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
		me_line_map_32->update_bi_hor(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
		break;
	default:
		assert(0);
	}

	store_enc_info_best<32>(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try,
		prev_cu_data_ptr,
		split_struct_x_pos_0, split_struct_y_pos_0, prev_up_log2_sub_cuw);

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

#if !ONLY_QT_QT
s64 circuit_32x32_32x16_16x32_md2(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA_ARRAY_SIZE<64>* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father)
{
#pragma HLS INLINE off
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
	U3 num_split_to_try = split_struct.num_split[part_num];

	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[2] = { TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;

	U3 split_mode[3];
	split_mode[0] = NO_SPLIT;
	split_mode[1] = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return  0;
	}

	//load_data
	static IP_ARRAY_SIZE<16, 32>		IP_buffer_16x32;
	static IP_ARRAY_SIZE<32, 16>		IP_buffer_32x16;
	static IP_MAP_SCU_SIZE<16, 32>		IP_map_scu_16x32;
	static IP_MAP_SCU_SIZE<32, 16>		IP_map_scu_32x16;
	static MPM_ARRAY_SIZE<16, 32>		MPM_buffer_16x32;
	static MPM_ARRAY_SIZE<32, 16>		MPM_buffer_32x16;
	static ENC_ME_NEB_INFO				up_nebs[1 + 8 + 1];
	static ENC_ME_NEB_INFO				left_nebs[8];
	static ENC_ME_NEB_INFO				nebs_line_ver[8];
	static ENC_ME_NEB_INFO				nebs_line_hor[8];
	static ENC_ME_LINE_MAP_SIZE<32>		me_line_map_32;

    load_64x16_16x64_32x16_16x32_md2(
			x0,
			y0,
			cu_width,
			cu_height,
			part_num,
			me_line_map_ptr_64,
			up_nebs_father, left_nebs_father,
			IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
			MPM_buffer_ptr_16x64, MPM_buffer_ptr_64x16,
			IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
			split_mode_father,
			&IP_buffer_16x32,
			&IP_buffer_32x16,
			&IP_map_scu_16x32,
			&IP_map_scu_32x16,
			&MPM_buffer_16x32,
			&MPM_buffer_32x16,
			up_nebs,
			left_nebs,
			nebs_line_ver,
			nebs_line_hor,
			&me_line_map_32);

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif

	static ENC_CU_DATA_ARRAY_SIZE<32>	curr_cu_data_temp[2];
	static ENC_CU_DATA_ARRAY_SIZE<32>	curr_cu_data_best;
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->split_mode cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->split_mode cyclic factor=7 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->ipm complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mpm complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mv complete dim=4
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mvd complete dim=4
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->refi complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mpm_ext complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->num_nz_coef complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->coef_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->reco_y cyclic factor=4 dim=3

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best.reco_y cyclic factor=4 dim=2

	//for (int i = 0; i < 2; i++)
	//{
	//	cu_data_chroma_pos<32>(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &curr_cu_data_temp[i], prev_cu_data_ptr, qt_depth + bet_depth);
	//}

	STATIC_C IP_ARRAY_SIZE<32, 32> null_IP_buffer_ptr_32x32;
	STATIC_C MPM_ARRAY_SIZE<32, 32> null_MPM_buffer_ptr_32x32;
	STATIC_C IP_MAP_SCU_SIZE<32, 32> null_IP_map_scu_ptr_32x32;

	// pu<KERNEL_16X32>
	// pu<KERNEL_32X16>
#if !KERNEL_SIZE_CLOSE(16, 32) || !KERNEL_SIZE_CLOSE(32, 16)
	cost[0] = md_kernel_32_lite(cu_width, cu_height, md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_32,
		up_nebs, left_nebs,
		skip_in,
		boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
		cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&curr_cu_data_temp[0],
		fme_mv_info,
		&null_IP_buffer_ptr_32x32, &IP_buffer_16x32, &IP_buffer_32x16,
		&null_MPM_buffer_ptr_32x32, &MPM_buffer_16x32, &MPM_buffer_32x16,
		&null_IP_map_scu_ptr_32x32, &IP_map_scu_16x32, &IP_map_scu_32x16,
		ref,
		Fetch_Ref_window_ptr);
#endif // !KERNEL_SIZE_CLOSE(16, 32) || !KERNEL_SIZE_CLOSE(32, 16)
#if KERNEL_SIZE_CLOSE(16, 32)
	if (cu_width == 16 && cu_height == 32)
		cost[0] = MAX_S64;
#endif // KERNEL_SIZE_CLOSE(16, 32)
#if KERNEL_SIZE_CLOSE(32, 16)
	if (cu_width == 32 && cu_height == 16)
		cost[0] = MAX_S64;
#endif // KERNEL_SIZE_CLOSE(32, 16)
	//cu_data_cpy<32>(&curr_cu_data[0], &curr_cu_data_temp[0], cu_width, cu_height, 0);

	cost[1] = md_16x16_loop_md2(
		md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_32,
		up_nebs, left_nebs,
		skip_in,
		split_struct.split_allow[part_num][split_mode[1]], x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, cu_width, cu_height, NS_luma_pred_mode,
		&curr_cu_data_temp[1],
		&cons_pred_mode_child[1], tree_status, &tree_status_child[1],
		fme_mv_info,
		&IP_buffer_16x32, &IP_buffer_32x16,
		&MPM_buffer_16x32, &MPM_buffer_32x16,
		&IP_map_scu_16x32, &IP_map_scu_32x16,
		ref,
		Fetch_Ref_window_ptr);

	s64 cost_best_temp = store_64x16_16x64_32x16_16x32_md2(
				md_fw_ptr,
				prev_cu_data_ptr,
    			cons_pred_mode, tree_status, part_num,
				prev_up_log2_sub_cuw,
				split_mode_father,
				cu_width_log2,
				cu_height_log2,
				cu_width,
				cu_height,
				cup,
				cud,
				x0,
				y0,
				num_split_to_try,
				cost,
				tree_status_child,
				cons_pred_mode_child,
				split_mode,
    			split_struct.x_pos[0],
    			split_struct.y_pos[0],
				curr_cu_data_temp,
				best_split_mode,
				best_cons_pred_mode,
				cost_best,
				best_tree_status_child,
				&IP_buffer_16x32,
				&IP_buffer_32x16,
				&IP_map_scu_16x32,
				&IP_map_scu_32x16,
				IP_map_scu_ptr_16x64, 
				IP_map_scu_ptr_64x16,
				&MPM_buffer_16x32,
				&MPM_buffer_32x16,
				nebs_line_ver,
				nebs_line_hor,
				&me_line_map_32,
				&curr_cu_data_best);

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST_C(cost_best_temp);
#endif
	return cost_best_temp;
}

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
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS INLINE off
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (isBTH_allow) {
		child_tree_status[0] = TREE_LC;
		bth_cost = prepare_cu_info_COM_combine<64>(md_fw_ptr, md_input_ptr->lambda_y, cu_data_temp_ptr, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode, cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			bth_cost += circuit_32x32_32x16_16x32_md2(
				md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_64,
				up_nebs, left_nebs,
				skip_in,
				cu_data_temp_ptr, qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2, fme_mv_info,
				IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
				MPM_buffer_ptr_16x64, MPM_buffer_ptr_64x16,
				IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
				ref,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return bth_cost;
}
#endif

void pu_64x16_16x64(s64& cost_temp, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 ip_list[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>& me_line_map_64,
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	U1 boundary, U2& NS_luma_pred_mode, U13 x0, U13 y0, U12 cup, U3 cu_width, U3 cu_height, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	COM_SPLIT_STRUCT& split_struct, U3 part_num,
	ENC_CU_DATA_ARRAY_SIZE<64> cu_data_temp[3],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>& IP_buffer_16x64, IP_ARRAY_SIZE<64, 16>& IP_buffer_64x16,
	IP_ARRAY_SIZE<8, 64>& null_IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>& null_IP_buffer_ptr_64x8,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<16, 64>& MPM_buffer_16x64, MPM_ARRAY_SIZE<64, 16>& MPM_buffer_64x16,
	MPM_ARRAY_SIZE<8, 64>& null_MPM_buffer_ptr_8x64, MPM_ARRAY_SIZE<64, 8>& null_MPM_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>& IP_map_scu_16x64, IP_MAP_SCU_SIZE<64, 16>& IP_map_scu_64x16,
	IP_MAP_SCU_SIZE<8, 64>& null_IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>& null_IP_map_scu_ptr_64x8,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	// pu<KERNEL_16X64>
	// pu<KERNEL_64X16>
#if !KERNEL_SIZE_CLOSE(64, 64) || !KERNEL_SIZE_CLOSE(64, 32) || !KERNEL_SIZE_CLOSE(32, 64) || !KERNEL_SIZE_CLOSE(16, 64) || !KERNEL_SIZE_CLOSE(64, 16)
	cost_temp = md_kernel_64(cu_width, cu_height, md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_in, boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
		cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&cu_data_temp[0],
		fme_mv_info,
		IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32, &IP_buffer_16x64, &IP_buffer_64x16,
		&null_IP_buffer_ptr_8x64,
		&null_IP_buffer_ptr_64x8,
		MPM_buffer_ptr_64x64, MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32, &MPM_buffer_16x64, &MPM_buffer_64x16,
		&null_MPM_buffer_ptr_8x64,
		&null_MPM_buffer_ptr_64x8,
		IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32, &IP_map_scu_16x64, &IP_map_scu_64x16,
		&null_IP_map_scu_ptr_8x64,
		&null_IP_map_scu_ptr_64x8,
		ref,
		Fetch_Ref_window_ptr);
#endif // !KERNEL_SIZE_CLOSE(64, 64) || !KERNEL_SIZE_CLOSE(64, 32) || !KERNEL_SIZE_CLOSE(32, 64) || !KERNEL_SIZE_CLOSE(16, 64) || !KERNEL_SIZE_CLOSE(64, 16)
#if KERNEL_SIZE_CLOSE(16, 64)
	if (cu_width == 16 && cu_height == 64)
		cost_temp = MAX_S64;
#endif // KERNEL_SIZE_CLOSE(16, 64)
#if KERNEL_SIZE_CLOSE(64, 16)
	if (cu_width == 64 && cu_height == 16)
		cost_temp = MAX_S64;
#endif // KERNEL_SIZE_CLOSE(64, 16)
};

#if !ONLY_QT_QT
void pu_32x32_32x16_16x32(s64& cost_temp, U3 split_mode_order[3], MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], ENC_ME_LINE_MAP_SIZE<64>& me_line_map_64,
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	U2& NS_luma_pred_mode, U13 x0, U13 y0, U12 cup, U3 cu_width, U3 cu_height, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	COM_SPLIT_STRUCT split_struct, U3 part_num,
	ENC_CU_DATA_ARRAY_SIZE<64> cu_data_temp[3], U2 cons_pred_mode_child[3], U2 tree_status_child[3],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 64>& IP_buffer_16x64, IP_ARRAY_SIZE<64, 16>& IP_buffer_64x16,
	MPM_ARRAY_SIZE<16, 64>& MPM_buffer_16x64, MPM_ARRAY_SIZE<64, 16>& MPM_buffer_64x16,
	IP_MAP_SCU_SIZE<16, 64>& IP_map_scu_16x64, IP_MAP_SCU_SIZE<64, 16>& IP_map_scu_64x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	cost_temp = md_32x32_32x16_16x32_loop_md2(split_mode_order[1],
		md_fw_ptr, md_input_ptr, g_rmd_output,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_in,
		split_struct.split_allow[part_num][split_mode_order[1]], x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, cu_width, cu_height, NS_luma_pred_mode, /*(int)cons_pred_mode,*/
		&cu_data_temp[1],
		&cons_pred_mode_child[1], &tree_status_child[1],
		fme_mv_info,
		&IP_buffer_16x64, &IP_buffer_64x16,
		&MPM_buffer_16x64, &MPM_buffer_64x16,
		&IP_map_scu_16x64, &IP_map_scu_64x16,
		ref,
		Fetch_Ref_window_ptr);
};
#endif

void pu_64x8_8x64(s64& cost_temp, U3 split_mode_order[3], MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], ENC_ME_LINE_MAP_SIZE<64>& me_line_map_64,
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in,
	U2& NS_luma_pred_mode, U13 x0, U13 y0, U12 cup, U3 cu_width, U3 cu_height, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status,
	COM_SPLIT_STRUCT split_struct, U3 part_num,
	ENC_CU_DATA_ARRAY_SIZE<64> cu_data_temp[3], U2 cons_pred_mode_child[3], U2 tree_status_child[3],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 64>& IP_buffer_16x64, IP_ARRAY_SIZE<64, 16>& IP_buffer_64x16,
	MPM_ARRAY_SIZE<16, 64>& MPM_buffer_16x64, MPM_ARRAY_SIZE<64, 16>& MPM_buffer_64x16,
	IP_MAP_SCU_SIZE<16, 64>& IP_map_scu_16x64, IP_MAP_SCU_SIZE<64, 16>& IP_map_scu_64x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	// pu<KERNEL_8X64>
	// pu<KERNEL_64X8>
	cost_temp = md_64x8_8x64_loop(
		md_fw_ptr, md_input_ptr, g_rmd_output,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_in, split_struct.split_allow[part_num][split_mode_order[2]], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
		qt_depth, bet_depth, cons_pred_mode,
		cu_width, cu_height, NS_luma_pred_mode,
		&cu_data_temp[2],
		&cons_pred_mode_child[2], tree_status, &tree_status_child[2],
		fme_mv_info,
		&IP_buffer_16x64, &IP_buffer_64x16,
		&MPM_buffer_16x64, &MPM_buffer_64x16,
		&IP_map_scu_16x64, &IP_map_scu_64x16,
		ref,
		Fetch_Ref_window_ptr);
};

#if !ONLY_QT_QT
void circuit_64x16_16x64_new(LCUConfig& config, LCUInput& buffer, Arbitor& ref, Channel& input, Channel& output, Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], Instruction mem_store[PR_SIZE])
{
	// stream port unused may cause core dump
#if vmod_interface
//#pragma HLS INTERFACE axis port=config.md_fw_ptr
//#pragma HLS INTERFACE axis port=config.md_input_ptr
#endif

#pragma HLS RESOURCE variable=ref->ref_0_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_0_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_1_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_1_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_2_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_2_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_3_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_3_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_4_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_4_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_5_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_5_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_6_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_6_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_7_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_7_1 core=RAM_1P_BRAM

	static Load2Bp bp_in_buf[BP_NUM];
	static Load2BpChn bp_chn_in_buf;
	static Load2OutChn out_chn_in_buf;
	static Load2PU pu_in_buf[PU_NUM];
	static BpChn2Store bp_chn_out_buf;
	static PU2Store pu_out_buf[PU_NUM];
	static ShareMemory pool;
	static CuDataPool cu_data;
	static Load2Output out_in_buf;
	static Output2Store out_out_buf;

	InstructionRegister<Instruction> ir1, ir2, ir;
	PRMan pu_man;
	hls::stream<U32> status;
	stream_type<ap_uint<1> > push_data_load[PU_NUM];
	reverse_stream_type<ap_uint<1> > push_data_store[PU_NUM];
#pragma HLS STREAM variable=status depth=2
	ir1.init();
	ir2.init();
	ir.init();
	pu_man.init();

	bool load_done = false, store_done = false;
	while (!load_done || !store_done)
	{
		if (!load_done)
		{
			load_done = load_core(ir1, ir2, mem_load, mem_pu, pu_man, status, bp_chn_in_buf, out_chn_in_buf, pu_in_buf, input, pool, config, buffer, push_data_load, bp_in_buf, cu_data, out_in_buf);
		}
		if (!push_data_load[BYPASS_CHANNEL].empty() && !push_data_store[BYPASS_CHANNEL].sbuf.full())
		{
			push_data_load[BYPASS_CHANNEL].read();
			bypass_channel(bp_chn_out_buf, bp_chn_in_buf);
			push_data_store[BYPASS_CHANNEL].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X64].empty() && !push_data_store[KERNEL_16X64].sbuf.full())
		{
			push_data_load[KERNEL_16X64].read();
			pu<KERNEL_16X64>(pu_in_buf[KERNEL_16X64], pu_out_buf[KERNEL_16X64], ref);
			push_data_store[KERNEL_16X64].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_64X16].empty() && !push_data_store[KERNEL_64X16].sbuf.full())
		{
			push_data_load[KERNEL_64X16].read();
			pu<KERNEL_64X16>(pu_in_buf[KERNEL_64X16], pu_out_buf[KERNEL_64X16], ref);
			push_data_store[KERNEL_64X16].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_8X64].empty() && !push_data_store[KERNEL_8X64].sbuf.full())
		{
			push_data_load[KERNEL_8X64].read();
			pu<KERNEL_8X64>(pu_in_buf[KERNEL_8X64], pu_out_buf[KERNEL_8X64], ref);
			push_data_store[KERNEL_8X64].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_64X8].empty() && !push_data_store[KERNEL_64X8].sbuf.full())
		{
			push_data_load[KERNEL_64X8].read();
			pu<KERNEL_64X8>(pu_in_buf[KERNEL_64X8], pu_out_buf[KERNEL_64X8], ref);
			push_data_store[KERNEL_64X8].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X32].empty() && !push_data_store[KERNEL_16X32].sbuf.full())
		{
			push_data_load[KERNEL_16X32].read();
			pu<KERNEL_16X32>(pu_in_buf[KERNEL_16X32], pu_out_buf[KERNEL_16X32], ref);
			push_data_store[KERNEL_16X32].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_32X16].empty() && !push_data_store[KERNEL_32X16].sbuf.full())
		{
			push_data_load[KERNEL_32X16].read();
			pu<KERNEL_32X16>(pu_in_buf[KERNEL_32X16], pu_out_buf[KERNEL_32X16], ref);
			push_data_store[KERNEL_32X16].sbuf.write(1);
		}
		if (!push_data_load[OUTPUT_CHANNEL].empty() && !push_data_store[OUTPUT_CHANNEL].sbuf.full())
		{
			push_data_load[OUTPUT_CHANNEL].read();
			output_channel(out_chn_in_buf, output);
			//push_data_store[OUTPUT_CHANNEL].sbuf.write(1);
		}
		if (!store_done)
		{
			store_done = store_core(ir, mem_store, status, push_data_store, config, bp_chn_out_buf, pu_out_buf, pool, cu_data, out_out_buf);
		}
	}
}

s64 circuit_64x16_16x64_wrap(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father, int ns_64x64)
{
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U3 num_split_to_try = split_struct.num_split[part_num];

	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	STATIC_C LCUConfig config;
	STATIC_C LCUInput buffer;
	STATIC_C Channel channel;

	STATIC_C Instruction mem_load[PR_SIZE] = {};
	STATIC_C Instruction mem_pu[PR_SIZE] = {};
	STATIC_C Instruction mem_store[PR_SIZE] = {};

	mem_load[0].opcode() = LOAD_CHANNEL;
	mem_load[0].cu_width_log2() = 6;
	mem_load[0].cu_height_log2() = 6;
	mem_load[0].flag_pu(BYPASS_CHANNEL) = 1;
	mem_load[0].tag_chn() = 0;

	mem_load[1].opcode() = LOAD_1_N_16x64_64x16;
	mem_load[1].part_num() = part_num;
	mem_load[1].flag_chn(0) = 1;
	mem_load[1].flag_pu(KERNEL_16X64) = ((cu_height_log2 == 4) && (cu_width_log2 == 6) && split_struct.split_allow[part_num][NO_SPLIT]) ? 1 : 0;
	mem_load[1].flag_pu(KERNEL_8X64) = ((cu_height_log2 == 4) && (cu_width_log2 == 6) && split_struct.split_allow[part_num][SPLIT_BI_VER]) ? 1 : 0;
	mem_load[1].flag_pu(KERNEL_16X32) = ((cu_height_log2 == 4) && (cu_width_log2 == 6) && split_struct.split_allow[part_num][SPLIT_BI_HOR]) ? 1 : 0;
	mem_load[1].flag_pu(KERNEL_64X16) = ((cu_height_log2 == 6) && (cu_width_log2 == 4) && split_struct.split_allow[part_num][NO_SPLIT]) ? 1 : 0;
	mem_load[1].flag_pu(KERNEL_32X16) = ((cu_height_log2 == 6) && (cu_width_log2 == 4) && split_struct.split_allow[part_num][SPLIT_BI_VER]) ? 1 : 0;
	mem_load[1].flag_pu(KERNEL_64X8) = ((cu_height_log2 == 6) && (cu_width_log2 == 4) && split_struct.split_allow[part_num][SPLIT_BI_HOR]) ? 1 : 0;
	mem_load[1].tag_chn() = 1;

	mem_load[2].opcode() = LOAD_1_OUT_CHANNEL;
	mem_load[2].cu_width_log2() = 6;
	mem_load[2].cu_height_log2() = 6;
	mem_load[2].flag_chn(0) = 1;
	mem_load[2].tag_chn() = 2;
	mem_load[2].flag_pu(OUTPUT_CHANNEL) = 1;

	mem_load[3].opcode() = DONE;

	mem_pu[1].opcode() = BYPASS_CHANNEL;
	mem_pu[1].cu_width_log2() = 6;
	mem_pu[1].cu_height_log2() = 6;
	mem_pu[1].tag_pu() = 1;

	int load_cnt = 1;
	if ((cu_height_log2 == 4) && (cu_width_log2 == 6))	//16x64
	{
		if (split_struct.split_allow[part_num][NO_SPLIT])
		{
			mem_pu[load_cnt].opcode() = KERNEL_16X64;
			mem_pu[load_cnt].split_mode() = NO_SPLIT;
			mem_pu[load_cnt].btd() = bet_depth;
			mem_pu[load_cnt].pred_mode() = cons_pred_mode;
			mem_pu[load_cnt].cud() = cud;
			mem_pu[load_cnt].cup() = cup;
			mem_pu[load_cnt].cu_height_log2() = cu_height_log2;
			mem_pu[load_cnt].cu_width_log2() = cu_width_log2;
			mem_pu[load_cnt].qtd() = qt_depth;
			mem_pu[load_cnt].tree_status() = tree_status;
			mem_pu[load_cnt].x0() = x0;
			mem_pu[load_cnt].y0() = y0;
			mem_pu[load_cnt].tag_pu() = 1;
			++load_cnt;
		}
		
		if (split_struct.split_allow[part_num][SPLIT_BI_VER])
		{
			mem_pu[load_cnt].opcode() = KERNEL_8X64;
			mem_pu[load_cnt].split_mode() = SPLIT_BI_VER;
			mem_pu[load_cnt].btd() = bet_depth;
			mem_pu[load_cnt].pred_mode() = cons_pred_mode;
			mem_pu[load_cnt].cud() = cud;
			mem_pu[load_cnt].cup() = cup;
			mem_pu[load_cnt].cu_height_log2() = cu_height_log2;
			mem_pu[load_cnt].cu_width_log2() = cu_width_log2;
			mem_pu[load_cnt].qtd() = qt_depth;
			mem_pu[load_cnt].tree_status() = tree_status;
			mem_pu[load_cnt].x0() = x0;
			mem_pu[load_cnt].y0() = y0;
			mem_pu[load_cnt].tag_pu() = 1;
			++load_cnt;
		}

		if (split_struct.split_allow[part_num][SPLIT_BI_HOR])
		{
			mem_pu[load_cnt].opcode() = KERNEL_16X32;
			mem_pu[load_cnt].split_mode() = SPLIT_BI_HOR;
			mem_pu[load_cnt].btd() = bet_depth;
			mem_pu[load_cnt].pred_mode() = cons_pred_mode;
			mem_pu[load_cnt].cud() = cud;
			mem_pu[load_cnt].cup() = cup;
			mem_pu[load_cnt].cu_height_log2() = cu_height_log2;
			mem_pu[load_cnt].cu_width_log2() = cu_width_log2;
			mem_pu[load_cnt].qtd() = qt_depth;
			mem_pu[load_cnt].tree_status() = tree_status;
			mem_pu[load_cnt].x0() = x0;
			mem_pu[load_cnt].y0() = y0;
			mem_pu[load_cnt].tag_pu() = 1;
			++load_cnt;
		}
	}
	else if ((cu_height_log2 == 6) && (cu_width_log2 == 4))	//64x16
	{
		if (split_struct.split_allow[part_num][NO_SPLIT])
		{
			mem_pu[load_cnt].opcode() = KERNEL_64X16;
			mem_pu[load_cnt].split_mode() = NO_SPLIT;
			mem_pu[load_cnt].btd() = bet_depth;
			mem_pu[load_cnt].pred_mode() = cons_pred_mode;
			mem_pu[load_cnt].cud() = cud;
			mem_pu[load_cnt].cup() = cup;
			mem_pu[load_cnt].cu_height_log2() = cu_height_log2;
			mem_pu[load_cnt].cu_width_log2() = cu_width_log2;
			mem_pu[load_cnt].qtd() = qt_depth;
			mem_pu[load_cnt].tree_status() = tree_status;
			mem_pu[load_cnt].x0() = x0;
			mem_pu[load_cnt].y0() = y0;
			mem_pu[load_cnt].tag_pu() = 1;
			++load_cnt;
		}

		if (split_struct.split_allow[part_num][SPLIT_BI_VER])
		{
			mem_pu[load_cnt].opcode() = KERNEL_32X16;
			mem_pu[load_cnt].split_mode() = SPLIT_BI_VER;
			mem_pu[load_cnt].btd() = bet_depth;
			mem_pu[load_cnt].pred_mode() = cons_pred_mode;
			mem_pu[load_cnt].cud() = cud;
			mem_pu[load_cnt].cup() = cup;
			mem_pu[load_cnt].cu_height_log2() = cu_height_log2;
			mem_pu[load_cnt].cu_width_log2() = cu_width_log2;
			mem_pu[load_cnt].qtd() = qt_depth;
			mem_pu[load_cnt].tree_status() = tree_status;
			mem_pu[load_cnt].x0() = x0;
			mem_pu[load_cnt].y0() = y0;
			mem_pu[load_cnt].tag_pu() = 1;
			++load_cnt;
		}

		if (split_struct.split_allow[part_num][SPLIT_BI_HOR])
		{
			mem_pu[load_cnt].opcode() = KERNEL_64X8;
			mem_pu[load_cnt].split_mode() = SPLIT_BI_HOR;
			mem_pu[load_cnt].btd() = bet_depth;
			mem_pu[load_cnt].pred_mode() = cons_pred_mode;
			mem_pu[load_cnt].cud() = cud;
			mem_pu[load_cnt].cup() = cup;
			mem_pu[load_cnt].cu_height_log2() = cu_height_log2;
			mem_pu[load_cnt].cu_width_log2() = cu_width_log2;
			mem_pu[load_cnt].qtd() = qt_depth;
			mem_pu[load_cnt].tree_status() = tree_status;
			mem_pu[load_cnt].x0() = x0;
			mem_pu[load_cnt].y0() = y0;
			mem_pu[load_cnt].tag_pu() = 1;
			++load_cnt;
		}
	}

	mem_pu[load_cnt].opcode() = OUTPUT_CHANNEL;
	mem_pu[load_cnt].cu_width_log2() = 6;
	mem_pu[load_cnt].cu_height_log2() = 6;
	mem_pu[load_cnt].tag_pu() = 1;

	mem_store[0].opcode() = STORE_CHN_1;
	mem_store[0].cu_width_log2() = 6;
	mem_store[0].cu_height_log2() = 6;
	mem_store[0].flag_pu(BYPASS_CHANNEL) = 1;
	mem_store[0].flag_chn(0) = 1;
	mem_store[0].tag_pu() = 1;
	mem_store[0].tag_chn() = 1;

	mem_store[1].opcode() = STORE_N_1_16x64_64x16;
	mem_store[1].cu_width_log2() = cu_width_log2;
	mem_store[1].cu_height_log2() = cu_height_log2;
	mem_store[1].cud() = cud;
	mem_store[1].tree_status() = tree_status;
	mem_store[1].x0() = x0;
	mem_store[1].y0() = y0;
	mem_store[1].part_num() = part_num;
	mem_store[1].cup() = cup;
	mem_store[1].pred_mode() = cons_pred_mode;
	mem_store[1].x_pos() = split_struct.x_pos[0];
	mem_store[1].y_pos() = split_struct.y_pos[0];
	mem_store[1].prev_up_log2_sub_cuw() = prev_up_log2_sub_cuw;
	mem_store[1].flag_chn(0) = 1;
	mem_store[1].flag_pu(KERNEL_16X64) = ((cu_height_log2 == 4) && (cu_width_log2 == 6) && split_struct.split_allow[part_num][NO_SPLIT]) ? 1 : 0;
	mem_store[1].flag_pu(KERNEL_8X64) = ((cu_height_log2 == 4) && (cu_width_log2 == 6) && split_struct.split_allow[part_num][SPLIT_BI_VER]) ? 1 : 0;
	mem_store[1].flag_pu(KERNEL_16X32) = ((cu_height_log2 == 4) && (cu_width_log2 == 6) && split_struct.split_allow[part_num][SPLIT_BI_HOR]) ? 1 : 0;
	mem_store[1].flag_pu(KERNEL_64X16) = ((cu_height_log2 == 6) && (cu_width_log2 == 4) && split_struct.split_allow[part_num][NO_SPLIT]) ? 1 : 0;
	mem_store[1].flag_pu(KERNEL_32X16) = ((cu_height_log2 == 6) && (cu_width_log2 == 4) && split_struct.split_allow[part_num][SPLIT_BI_VER]) ? 1 : 0;
	mem_store[1].flag_pu(KERNEL_64X8) = ((cu_height_log2 == 6) && (cu_width_log2 == 4) && split_struct.split_allow[part_num][SPLIT_BI_HOR]) ? 1 : 0;
	mem_store[1].tag_pu() = 1;
	mem_store[1].tag_chn() = 2;

	mem_store[2].opcode() = DONE;

	config.md_fw_ptr = *md_fw_ptr;
	config.md_input_ptr = *md_input_ptr;

	//memcpy(buffer.CtrPos, CtrPos, sizeof(buffer.CtrPos));
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			for (size_t k = 0; k < 2; k++) {
				buffer.CtrPos[i][j][k] = CtrPos[i][j][k];
			}
		}
	}
	//memcpy(buffer.CtrPos_ref1, CtrPos_ref1, sizeof(buffer.CtrPos_ref1));
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			for (size_t k = 0; k < 2; k++) {
				buffer.CtrPos_ref1[i][j][k] = CtrPos_ref1[i][j][k];
			}
		}
	}
	buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
	buffer.fme_mv_info = *fme_mv_info;
	buffer.g_rmd_output = *g_rmd_output;
	buffer.me_mv_dir = *me_mv_dir;
	//memcpy(buffer.p_fenc_LCU_Y, p_fenc_LCU_Y, sizeof(buffer.p_fenc_LCU_Y));
	/*for (size_t i = 0; i < MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH; i++) {
		buffer.p_fenc_LCU_Y[i] = p_fenc_LCU_Y[i];
	}*/
	for (size_t i = 0; i < MAX_CU_SIZE_FETCH; i++)
	{
		for (size_t j = 0; j < MAX_CU_SIZE_FETCH; j++) {
			buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
		}
	}
	//memcpy(buffer.RefWindowLCU_UP, RefWindowLCU_UP, sizeof(buffer.RefWindowLCU_UP));
	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_UP[i] = RefWindowLCU_UP[i];
	}
	//memcpy(buffer.RefWindowLCU_UP_ref1, RefWindowLCU_UP_ref1, sizeof(buffer.RefWindowLCU_UP_ref1));
	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_UP_ref1[i] = RefWindowLCU_UP_ref1[i];
	}
	//memcpy(buffer.RefWindowLCU_VP, RefWindowLCU_VP, sizeof(buffer.RefWindowLCU_VP));
	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_VP[i] = RefWindowLCU_VP[i];
	}
	//memcpy(buffer.RefWindowLCU_VP_ref1, RefWindowLCU_VP_ref1, sizeof(buffer.RefWindowLCU_VP_ref1));
	for (size_t i = 0; i < (SWH * SWW) >> 2; i++) {
		buffer.RefWindowLCU_VP_ref1[i] = RefWindowLCU_VP_ref1[i];
	}
	buffer.skip_in = *skip_in;

	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; ++i)
	{
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j)
		{
			channel.cost_best[i][j] = MAX_S64;
		}
	}
	channel.IP_buffer_ptr_64x64 = *IP_buffer_ptr_64x64;
	channel.IP_map_scu_ptr_64x64 = *IP_map_scu_ptr_64x64;
	//memcpy(channel.left_nebs_32, left_nebs_father, sizeof(channel.left_nebs_32));
	for (size_t i = 0; i < (64 >> 2); i++) {
		channel.left_nebs_64[i] = left_nebs_father[i];
	}
	channel.me_line_map_ptr_64 = *me_line_map_ptr_64;
	channel.MPM_buffer_ptr_64x64 = *MPM_buffer_ptr_64x64;
	//memcpy(channel.up_nebs_32, up_nebs_father, sizeof(channel.up_nebs_32));
	for (size_t i = 0; i < 10; i++) {
		channel.up_nebs_32[i] = up_nebs_father[i];
	}

	circuit_64x16_16x64_new(config, buffer, ref, channel, channel, mem_load, mem_pu, mem_store);

	*prev_up_cu_data_temp_array_ptr = channel.prev_up_cu_data_temp_array_ptr;
	*IP_map_scu_ptr_64x64 = channel.IP_map_scu_ptr_64x64;
	return channel.cost_best[(y0 - split_struct.y_pos[0]) >> MIN_CU_LOG2][(x0 - split_struct.x_pos[0]) >> MIN_CU_LOG2];
}
#endif

#if !ONLY_QT_QT
s64 circuit_64x16_16x64(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father, int ns_64x64)
{
#pragma HLS INLINE off
	U3 cu_width_log2 = split_struct.log_cuw[part_num];
	U3 cu_height_log2 = split_struct.log_cuh[part_num];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U12 cup = split_struct.cup[part_num];
	U4 cud = split_struct.cud;
	U14 x0 = split_struct.x_pos[part_num];
	U14 y0 = split_struct.y_pos[part_num];
	U3 num_split_to_try = split_struct.num_split[part_num];

	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);

	static ENC_CU_DATA_ARRAY curr_cu_data_best;
	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[3] = { TREE_LC, TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[3] = { NO_MODE_CONS, NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;
	U3 split_mode_order[3];

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3
#if CIRCUIT_64x16_16x64
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_up_cu_data_temp_array_ptr->mvd cyclic factor=2 dim=3
#endif
	split_mode_order[0] = NO_SPLIT;
	split_mode_order[1] = (cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	split_mode_order[2] = (cu_height == 16) ? SPLIT_BI_HOR : SPLIT_BI_VER;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	static IP_ARRAY_SIZE<16, 64>		IP_buffer_16x64;
	static IP_ARRAY_SIZE<64, 16>		IP_buffer_64x16;
	static IP_MAP_SCU_SIZE<16, 64>		IP_map_scu_16x64;
	static IP_MAP_SCU_SIZE<64, 16>		IP_map_scu_64x16;
	static MPM_ARRAY_SIZE<16, 64>		MPM_buffer_16x64;
	static MPM_ARRAY_SIZE<64, 16>		MPM_buffer_64x16;
	static ENC_ME_NEB_INFO				up_nebs[1 + (64 >> 2) + 1];
	static ENC_ME_NEB_INFO				left_nebs[(64 >> 2)];
	static ENC_ME_NEB_INFO				nebs_line_ver[16];
	static ENC_ME_NEB_INFO				nebs_line_hor[16];
	static ENC_ME_LINE_MAP_SIZE<64>		me_line_map_64;
	static ENC_CU_DATA_ARRAY_SIZE<64>	cu_data_temp[3];

#pragma HLS ARRAY_PARTITION variable=cu_data_temp->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->coef_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->reco_y cyclic factor=4 dim=3

#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mpm_ext cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->split_mode cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->split_mode cyclic factor=7 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mpm cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mv cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mvd cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->refi cyclic factor=2 dim=3

	//for (int i = 0; i < 3; i++)
	//{
	//	cu_data_chroma_pos<64>(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_temp[i], prev_up_cu_data_temp_array_ptr, qt_depth + bet_depth);
	//}

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif

	load_64x32_32x64_64x16_16x64(
		x0,
		y0,
		cu_width,
		cu_height,
		part_num,
		me_line_map_ptr_64,
		up_nebs_father, left_nebs_father,
		IP_buffer_ptr_32x64, IP_buffer_ptr_64x32,
		MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32,
		IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32,
		split_mode_father,
		ns_64x64,
		&IP_buffer_16x64,
		&IP_buffer_64x16,
		&IP_map_scu_16x64,
		&IP_map_scu_64x16,
		&MPM_buffer_16x64,
		&MPM_buffer_64x16,
		up_nebs,
		left_nebs,
		nebs_line_ver,
		nebs_line_hor,
		&me_line_map_64);

	STATIC_C IP_ARRAY_SIZE<8, 64> null_IP_buffer_ptr_8x64;
	STATIC_C IP_ARRAY_SIZE<64, 8> null_IP_buffer_ptr_64x8;
	STATIC_C MPM_ARRAY_SIZE<8, 64> null_MPM_buffer_ptr_8x64;
	STATIC_C MPM_ARRAY_SIZE<64, 8> null_MPM_buffer_ptr_64x8;
	STATIC_C IP_MAP_SCU_SIZE<8, 64> null_IP_map_scu_ptr_8x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 8> null_IP_map_scu_ptr_64x8;

	pu_64x16_16x64(cost[0], md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		me_mv_dir,
#endif
		p_fenc_LCU_Y, me_line_map_64, up_nebs, left_nebs, skip_in, boundary, NS_luma_pred_mode, 
		x0, y0, cup, cu_width, cu_height, cu_width_log2, cu_height_log2,
		cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		split_struct, part_num, cu_data_temp, fme_mv_info,
		IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32, IP_buffer_16x64, IP_buffer_64x16,
		null_IP_buffer_ptr_8x64, null_IP_buffer_ptr_64x8,
		MPM_buffer_ptr_64x64, MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32, MPM_buffer_16x64, MPM_buffer_64x16,
		null_MPM_buffer_ptr_8x64, null_MPM_buffer_ptr_64x8,
		IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32, IP_map_scu_16x64, IP_map_scu_64x16,
		null_IP_map_scu_ptr_8x64, null_IP_map_scu_ptr_64x8,
		ref, Fetch_Ref_window_ptr);

	pu_32x32_32x16_16x32(cost[1], split_mode_order,
		md_fw_ptr, md_input_ptr, g_rmd_output,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
		p_fenc_LCU_Y,
		me_line_map_64,
		up_nebs, left_nebs,
		skip_in,
		NS_luma_pred_mode, x0, y0, cup, cu_width, cu_height, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		split_struct, part_num,
		cu_data_temp,
		cons_pred_mode_child, tree_status_child,
		fme_mv_info,
		IP_buffer_16x64, IP_buffer_64x16,
		MPM_buffer_16x64, MPM_buffer_64x16,
		IP_map_scu_16x64, IP_map_scu_64x16,
		ref,
		Fetch_Ref_window_ptr);

	pu_64x8_8x64(cost[2], split_mode_order,
		md_fw_ptr, md_input_ptr, g_rmd_output,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
		p_fenc_LCU_Y,
		me_line_map_64,
		up_nebs, left_nebs,
		skip_in,
		NS_luma_pred_mode, x0, y0, cup, cu_width, cu_height, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		split_struct, part_num,
		cu_data_temp,
		cons_pred_mode_child, tree_status_child,
		fme_mv_info,
		IP_buffer_16x64, IP_buffer_64x16,
		MPM_buffer_16x64, MPM_buffer_64x16,
		IP_map_scu_16x64, IP_map_scu_64x16,
		ref,
		Fetch_Ref_window_ptr);

    s64 cost_best_temp = store_64x32_32x64_64x16_16x64(
		md_fw_ptr,
		prev_up_cu_data_temp_array_ptr,
		cons_pred_mode, tree_status, part_num,
		prev_up_log2_sub_cuw,
		split_mode_father,
		ns_64x64,
		cu_width_log2,
		cu_height_log2,
		cu_width,
		cu_height,
		cup,
		cud,
		x0,
		y0,
		num_split_to_try,
		cost,
		tree_status_child,
		cons_pred_mode_child,
		split_mode_order,
		split_struct.x_pos[0],
		split_struct.y_pos[0],
		cu_data_temp,
		best_split_mode,
		best_cons_pred_mode,
		cost_best,
		best_tree_status_child,
		&IP_buffer_16x64,
		&IP_buffer_64x16,
		&IP_map_scu_16x64,
		&IP_map_scu_64x16,
		IP_map_scu_ptr_32x64,
		IP_map_scu_ptr_64x32,
		&MPM_buffer_16x64,
		&MPM_buffer_64x16,
		nebs_line_ver,
		nebs_line_hor,
		&me_line_map_64,
		&curr_cu_data_best);


#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST_C(cost_best_temp);
#endif

	return cost_best_temp;
}
#endif

void update_map_scu_top_luma(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, U13 x, U13 y,
	U2 tree_status, ENC_CU_DATA* md1_best_ctu_output_ptr)
{
#pragma HLS INLINE off
#if UPDATE_MAP_SCU_TOP_LUMA
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_cu_mode_inner complete dim=1
#endif
	U7 src_cuw = md_fw_ptr->max_cuwh;
	U7 src_cuh = md_fw_ptr->max_cuwh;
	S8* src_map_ipm;
	U7 w;
	U7 h;
	u16 i;
	U3 log2_src_cuw;
	U3 log2_src_cuh;
	U13 scu_x;
	U13 scu_y;


	U5 mpm_x;
	U5 mpm_y;
	U16 ii = 0;
	U13 x_scu_line;
	U13 y_scu_line;
	U16 j = 0;

	scu_x = x >> 2;
	scu_y = y >> 2;
	log2_src_cuw = (com_tbl_log2[src_cuw]);
	log2_src_cuh = (com_tbl_log2[src_cuh]);

	src_map_ipm = md1_best_ctu_output_ptr->ipm[0];

	if (x >= md_fw_ptr->pic_width || y >= md_fw_ptr->pic_height)
		return;

	if (x + src_cuw > md_fw_ptr->pic_width) {
		w = (md_fw_ptr->pic_width - x) >> 2;
	}
	else {
		w = src_cuw >> 2;
	}
	if (y + src_cuh > md_fw_ptr->pic_height) {
		h = (md_fw_ptr->pic_height - y) >> 2;
	}
	else {
		h = src_cuh >> 2;
	}

	mpm_x = scu_x & 15;
	mpm_y = scu_y & 15;

	//for (i = 0; i < h; i++) {
	//	for (ii = 0; ii < w; ii++) {
	//		MPM_buffer_ptr->buffer[i + mpm_y][ii + mpm_x] = (src_map_ipm[(i << (log2_src_cuw - 2)) + ii]);
	//	}
	//}
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		src_map_ipm += src_cuw >> 2;
	}
	x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	y_scu_line = scu_y - me_line_map_ptr->min_scu_y;

	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
		for (j = 0; j < 16; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			if (j < w) {
				me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
					= md1_best_ctu_output_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
				me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i] = md1_best_ctu_output_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
			}
		}
	}
}
static void update_map_scu_top(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U13 x, U13 y,
	U2 tree_status, ENC_CU_DATA* md1_best_ctu_output_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
	U7 src_cuw = md_fw_ptr->max_cuwh;
	U7 src_cuh = md_fw_ptr->max_cuwh;
	S8* src_map_ipm;
	U7 w;
	U7 h;
	u16 i;
	U3 log2_src_cuw;
	U3 log2_src_cuh;
	U13 scu_x;
	U13 scu_y;


	U5 mpm_x;
	U5 mpm_y;
	U16 ii = 0;
	U13 x_scu_line;
	U13 y_scu_line;
	U16 j = 0;

	scu_x = x >> 2;
	scu_y = y >> 2;
	log2_src_cuw = (com_tbl_log2[src_cuw]);
	log2_src_cuh = (com_tbl_log2[src_cuh]);

	src_map_ipm = md1_best_ctu_output_ptr->ipm[0];

	if (x >= md_fw_ptr->pic_width || y >= md_fw_ptr->pic_height)
		return;

	if (x + src_cuw > md_fw_ptr->pic_width) {
		w = (md_fw_ptr->pic_width - x) >> 2;
	}
	else {
		w = src_cuw >> 2;
	}
	if (y + src_cuh > md_fw_ptr->pic_height) {
		h = (md_fw_ptr->pic_height - y) >> 2;
	}
	else {
		h = src_cuh >> 2;
	}

	mpm_x = scu_x & 15;
	mpm_y = scu_y & 15;

	for (i = 0; i < h; i++) {
		for (ii = 0; ii < w; ii++) {
			MPM_buffer_ptr->buffer[i + mpm_y][ii + mpm_x] = (src_map_ipm[(i << (log2_src_cuw - 2)) + ii]);
		}
	}
	for (i = 0; i < h; i++) {
		src_map_ipm += src_cuw >> 2;
	}
	x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	y_scu_line = scu_y - me_line_map_ptr->min_scu_y;

	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
				= md1_best_ctu_output_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
			me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i] = md1_best_ctu_output_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].refi[0] = md1_best_ctu_output_ptr->refi[j + (i << (log2_src_cuw - 2))][0];
			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].refi[1] = md1_best_ctu_output_ptr->refi[j + (i << (log2_src_cuw - 2))][1];
			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].mv[0][0] = md1_best_ctu_output_ptr->mv[j + (i << (log2_src_cuw - 2))][0][0];
			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].mv[0][1] = md1_best_ctu_output_ptr->mv[j + (i << (log2_src_cuw - 2))][0][1];
			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].mv[1][0] = md1_best_ctu_output_ptr->mv[j + (i << (log2_src_cuw - 2))][1][0];
			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].mv[1][1] = md1_best_ctu_output_ptr->mv[j + (i << (log2_src_cuw - 2))][1][1];
			/*inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].pb_tb_part = md1_best_ctu_output_ptr->map_pb_tb_part[j + (i << (log2_src_cuw - 2))];*/
		}
	}
	//for (i = 0; i < h; i++) {
	//	for (j = 0; j < w; j++) {
	//		MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i] =
	//			md1_best_ctu_output_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
	//	}
	//}
}

void update_to_ctx_map_top_luma(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_CU_DATA* md1_best_ctu_output_ptr)
{
#pragma HLS INLINE off
	ENC_CU_DATA* cu_data;
	U8 cu_width;
	U8 cu_height;
	U6 i;
	U6 j;
	U6 w;
	U6 h;
	U13 x;
	U13 y;
	U18 core_idx;
	// s8 (*map_refi)[2];
	// s16 (*map_mv)[2][2];
	U13 x_scu_line;
	U13 y_scu_line;
#if UPDATE_TO_CTX_MAP_TOP_LUMA_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_cu_mode_inner complete dim=1

#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].refi complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data->mv complete dim=3
#endif
	//s8 (*map_split)[5][NUM_BLOCK_SHAPE][4096 / 16];
	cu_data = md1_best_ctu_output_ptr;
	cu_width = md_fw_ptr->max_cuwh;
	cu_height = md_fw_ptr->max_cuwh;
	x = md_input_ptr->pix_x;
	y = md_input_ptr->pix_y;
	if (x + cu_width > md_fw_ptr->pic_width) {
		cu_width = md_fw_ptr->pic_width - x;
	}
	if (y + cu_height > md_fw_ptr->pic_height) {
		cu_height = md_fw_ptr->pic_height - y;
	}
	w = cu_width >> 2;
	h = cu_height >> 2;
	/* copy mode info */
	x_scu_line = (x >> 2) - me_line_map_ptr->min_scu_x;
	y_scu_line = (y >> 2) - me_line_map_ptr->min_scu_y;
	core_idx = 0;
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (j = 0; j < 16; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			if (j < w) {
				if ((cu_data->pred_mode[core_idx + j]) == 0) {
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].refi[0] = -1;
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].refi[1] = -1;
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[0][0] = 0;
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[0][1] = 0;
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[1][0] = 0;
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[1][1] = 0;
				}
				else {
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].refi[0] = cu_data->refi[core_idx + j][0];
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].refi[1] = cu_data->refi[core_idx + j][1];
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[0][0] = cu_data->mv[core_idx + j][0][0];
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[0][1] = cu_data->mv[core_idx + j][0][1];
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[1][0] = cu_data->mv[core_idx + j][1][0];
					cur_ctu_mi[x_scu_line + j][y_scu_line +
						i].mv[1][1] = cu_data->mv[core_idx + j][1][1];
				}
			}
		}
		core_idx += md_fw_ptr->max_cuwh >> 2;
	}
	//for (U4 k1 = 0; k1 < 5; k1++){
	//	for (U4 k2 = 0; k2 < (NUM_BLOCK_SHAPE); k2++){
	//		for (U9 k3 = 0; k3 < ((1 << 7 >> 1) << (7 - 1)) >> 4; k3++) {
	//			me_line_map_ptr->line_map_split[k1][k2][k3] = cu_data->split_mode[k1][k2][k3];
	//		}
	//	}
	//}
	update_map_scu_top_luma(md_fw_ptr, me_line_map_ptr, md_input_ptr->pix_x, md_input_ptr->pix_y,
		TREE_LC, md1_best_ctu_output_ptr);
}

void update_to_ctx_map_top(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, ENC_CU_DATA* md1_best_ctu_output_ptr)
{
	ENC_CU_DATA* cu_data;
	U8 cu_width;
	U8 cu_height;
	U6 i;
	U6 j;
	U6 w;
	U6 h;
	U13 x;
	U13 y;
	U18 core_idx;
	// s8 (*map_refi)[2];
	// s16 (*map_mv)[2][2];
	U13 x_scu_line;
	U13 y_scu_line;


	//s8 (*map_split)[5][NUM_BLOCK_SHAPE][4096 / 16];
	cu_data = md1_best_ctu_output_ptr;
	cu_width = md_fw_ptr->max_cuwh;
	cu_height = md_fw_ptr->max_cuwh;
	x = md_input_ptr->pix_x;
	y = md_input_ptr->pix_y;
	if (x + cu_width > md_fw_ptr->pic_width) {
		cu_width = md_fw_ptr->pic_width - x;
	}
	if (y + cu_height > md_fw_ptr->pic_height) {
		cu_height = md_fw_ptr->pic_height - y;
	}
	w = cu_width >> 2;
	h = cu_height >> 2;
	/* copy mode info */
	x_scu_line = (x >> 2) - me_line_map_ptr->min_scu_x;
	y_scu_line = (y >> 2) - me_line_map_ptr->min_scu_y;
	core_idx = 0;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			if ((cu_data->pred_mode[core_idx + j]) == 0) {
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].refi[0] = -1;
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].refi[1] = -1;
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[0][0] = 0;
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[0][1] = 0;
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[1][0] = 0;
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[1][1] = 0;
			}
			else {
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].refi[0] = cu_data->refi[core_idx + j][0];
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].refi[1] = cu_data->refi[core_idx + j][1];
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[0][0] = cu_data->mv[core_idx + j][0][0];
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[0][1] = cu_data->mv[core_idx + j][0][1];
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[1][0] = cu_data->mv[core_idx + j][1][0];
				inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i].mv[1][1] = cu_data->mv[core_idx + j][1][1];
			}
		}
		core_idx += md_fw_ptr->max_cuwh >> 2;
	}
	//for (U4 k1 = 0; k1 < 5; k1++){
	//	for (U4 k2 = 0; k2 < (NUM_BLOCK_SHAPE); k2++){
	//		for (U9 k3 = 0; k3 < ((1 << 7 >> 1) << (7 - 1)) >> 4; k3++) {
	//			me_line_map_ptr->line_map_split[k1][k2][k3] = cu_data->split_mode[k1][k2][k3];
	//		}
	//	}
	//}
	update_map_scu_top(md_fw_ptr, me_line_map_ptr, inner_nebs, md_input_ptr->pix_x, md_input_ptr->pix_y,
		TREE_LC, md1_best_ctu_output_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr);
}

int copy_cu_data(ENC_CU_DATA* dst, ENC_CU_DATA* src, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH])
{
	U8 i, ii, i1, i2;
	U8 j;
	U8 k;
	U8 cu_width;
	U8 cu_height;
	U8 cus;
	U8 cuw_scu;
	U8 cuh_scu;
	U8 cus_scu;
	//U14 cx;
	//U14 cy;
	U22 idx_dst;
	U22 idx_src;
	//cx = x >> 2;
	//cy = y >> 2;
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);

#if COPY_CU_DATA_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=dst->num_nz_coef cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1	
#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3		
#pragma HLS ARRAY_PARTITION variable=src->num_nz_coef cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2
#endif

	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = (U22)((j) << (log2_cus - 2));
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			for (k = 0; k < 5; k++) {
#pragma HLS LOOP_TRIPCOUNT max=5
				if (k >= cud) {
					for (i = 0; i < (NUM_BLOCK_SHAPE); i++) {
						(dst->split_mode[k][i] + idx_dst)[ii] = (src->split_mode[k][i] + idx_src)[ii];
					}
				}
			}
		}
	}

	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = (U22)((j) << (log2_cus - 2));
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (ii = 0; ii < 16; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			if (ii < cuw_scu) {
				(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
				(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
				(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
				(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
				(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
				(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];

				for (i = 0; i < 8; i++) {
					(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];
				}

				(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
				(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
				(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
				(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
				(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
				(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];

				for (i = 0; i < 2; i++) {
					(dst->refi + idx_dst)[ii][i] = (src->refi + idx_src)[ii][i];
				}

				(dst->umve_flag + idx_dst)[ii] = (src->umve_flag + idx_src)[ii];
				(dst->umve_idx + idx_dst)[ii] = (src->umve_idx + idx_src)[ii];
				(dst->skip_idx + idx_dst)[ii] = (src->skip_idx + idx_src)[ii];
				(dst->mvr_idx + idx_dst)[ii] = (src->mvr_idx + idx_src)[ii];
				(dst->mvp_from_hmvp_flag + idx_dst)[ii] = (src->mvp_from_hmvp_flag + idx_src)[ii];
				(dst->ipf_flag + idx_dst)[ii] = (src->ipf_flag + idx_src)[ii];
				for (i1 = 0; i1 < 2; i1++) {
					for (i2 = 0; i2 < 2; i2++) {
						(dst->mv + idx_dst)[ii][i1][i2] = (src->mv + idx_src)[ii][i1][i2];
						(dst->mvd + idx_dst)[ii][i1][i2] = (src->mvd + idx_src)[ii][i1][i2];
					}
				}

				for (k = 0; k < 3; k++) {
					(dst->num_nz_coef[k] + idx_dst)[ii] = (src->num_nz_coef[k] + idx_src)[ii];
				}

				(dst->pb_part + idx_dst)[ii] = (src->pb_part + idx_src)[ii];
				(dst->tb_part + idx_dst)[ii] = (src->tb_part + idx_src)[ii];

				for (int k = 0; k < 4; k++) {
					for (int l = 0; l < 4; l++) {
						dst->coef_y[y + j * 4 + k][x + ii * 4 + l] = src->coef_y[j * 4 + k][ii * 4 + l];
						dst->reco_y[y + j * 4 + k][x + ii * 4 + l] = src->reco_y[j * 4 + k][ii * 4 + l];
					}
				}
			}
		}
	}
#if 0
	for (j = 0; j < cu_height; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
		idx_dst = ((U22)(y + j) << log2_cus) + x;
		idx_src = (U22)j << cu_width_log2;
#pragma HLS PIPELINE
		for (i = 0; i < 64; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
			if (i < cu_width) {
				dst->coef_y[y + j][x + i] = src->coef_y[j][i];
				dst->reco_y[y + j][x + i] = src->reco_y[j][i];
			}
		}
	}
#endif
	return 0;
}

void Copy_LCU_To_Top_linebuffer(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, pel reco_y[CU_SIZE >> 2][CU_SIZE >> 2], pel dbk_y[71][71])//wangwei modify
{
#pragma HLS INLINE off
#if COPY_LCU_TO_TOP_LINEBUFFER_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=dbk_y complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=dbk_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=reco_y cyclic factor=4 dim=2
#endif
	//COM_INFO *info = &ctx->info;
	U13 lcu_x = md_input_ptr->pix_x;
	U13 lcu_y = md_input_ptr->pix_y;
	U8 lcuw = 1 << md_fw_ptr->log2_max_cuwh < md_fw_ptr->pic_width - lcu_x ? (U8)(1 << md_fw_ptr->log2_max_cuwh) : (U8)(md_fw_ptr->pic_width - lcu_x);
	U8 lcuh = 1 << md_fw_ptr->log2_max_cuwh < md_fw_ptr->pic_height - lcu_y ? (U8)(1 << md_fw_ptr->log2_max_cuwh) : (U8)(md_fw_ptr->pic_height - lcu_y);
	U8 j;

	for (int j = 0; j < 71; j++) {
		for (int i = 0; i < 71; i++) {
#pragma HLS UNROLL
			dbk_y[j][i] = 0;
		}
	}

	//dst_dbk = dbk_y[6];
	for (j = 0; j < lcuh; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
#pragma HLS PIPELINE
		for (U8 i = 0; i < 64; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
			if (i < lcuw) {
				dbk_y[j + 6][i + 6] = reco_y[j][i];
			}
		}
	}
}

void write_back_inner_line_map(ENC_ME_LINE_MAP* dst, ENC_ME_LINE_MAP* src)
{
	int i, j;

	for (i = 0; i < SCU_IN_LCU_NUMS_LEN; i++) {
#pragma HLS PIPELINE
		for (j = 0; j < SCU_IN_LCU_NUMS_LEN; j++) {
			dst->map_scu_inner[src->curr_ctu_idx][i][j] = src->map_scu_inner[src->curr_ctu_idx][i][j];
			dst->map_scu_inner[1 - src->curr_ctu_idx][i][j] = src->map_scu_inner[1 - src->curr_ctu_idx][i][j];

			dst->map_cu_mode_inner[src->curr_ctu_idx][i][j] = src->map_cu_mode_inner[src->curr_ctu_idx][i][j];
			dst->map_cu_mode_inner[1 - src->curr_ctu_idx][i][j] = src->map_cu_mode_inner[1 - src->curr_ctu_idx][i][j];
		}
	}
}



void write_back_inner_nebs(ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
	int i, j;
	for (i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
		for (j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			cur_ctu_mi[i][j].mv[0][0] = inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j].mv[0][0];
			cur_ctu_mi[i][j].mv[1][0] = inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j].mv[1][0];
			cur_ctu_mi[i][j].mv[0][1] = inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j].mv[0][1];
			cur_ctu_mi[i][j].mv[1][1] = inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j].mv[1][1];
			cur_ctu_mi[i][j].refi[0] = inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j].refi[0];
			cur_ctu_mi[i][j].refi[1] = inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j].refi[1];

		}
	}
}

void init_dbk_1(pel dbk_y[71][71])
{
#pragma HLS inline
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			for (int l = 0; l < 4; l++)
			{
				for (int k = 0; k < 4; k++)
				{
					dbk_y[j * 4 + l][i * 4 + k] = 0;
				}
			}
		}
	}
}

void copy_dbk_1(ENC_CU_DATA md_best_ctu_output[2], ENC_CU_DATA* md2_output, pel dbk_y[71][71], U1 best_index)
{
#pragma HLS inline
	U8 i, j, k, l;
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
#pragma HLS PIPELINE
			for (l = 0; l < 4; l++)
			{
				for (k = 0; k < 4; k++)
				{
					md2_output->coef_y[i * 4 + k][j * 4 + l] = md_best_ctu_output[best_index].coef_y[i * 4 + k][j * 4 + l];
					md2_output->reco_y[i * 4 + k][j * 4 + l] = md_best_ctu_output[best_index].reco_y[i * 4 + k][j * 4 + l];
					dbk_y[6 + j * 4 + l][6 + i * 4 + k] = md_best_ctu_output[best_index].reco_y[j * 4 + l][i * 4 + k];
				}
			}
		}
	}
}

void store_enc_info_top_0(ENC_CU_DATA md_best_ctu_output[2], ENC_CU_DATA* md2_output, pel dbk_y[71][71], U1 best_index)
{
#pragma HLS inline
	U8 i, j, k, l;

	init_dbk_1(dbk_y);
	copy_dbk_1(md_best_ctu_output, md2_output, dbk_y, best_index);
}

void store_me_line(int w, int h, ENC_CU_DATA* src, ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output, int x_scu_line, int y_scu_line, int log2_src_cuw, MD_FW* md_fw_ptr)
{
#pragma HLS inline
	U18 core_idx = 0;

STORE_TOP12:
	for (int i = 0; i < h; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		for (int j = 0; j < 16; j++)
		{
#pragma HLS PIPELINE
			if (j < w)
			{
				if ((src->pred_mode[core_idx + j]) == 0)
				{
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].refi[0] = -1;
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].refi[1] = -1;
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[0][0] = 0;
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[0][1] = 0;
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[1][0] = 0;
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[1][1] = 0;
				}
				else
				{
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].refi[0] = src->refi[core_idx + j][0];
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].refi[1] = src->refi[core_idx + j][1];
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[0][0] = src->mv[core_idx + j][0][0];
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[0][1] = src->mv[core_idx + j][0][1];
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[1][0] = src->mv[core_idx + j][1][0];
					cur_ctu_mi[x_scu_line + j][y_scu_line + i].mv[1][1] = src->mv[core_idx + j][1][1];
				}

				//me_line_map_ptr_input->map_scu_inner     [me_line_map_ptr_input->curr_ctu_idx][x_scu_line + j][y_scu_line + i] = src->map_scu    [j + (i << (log2_src_cuw - 2))];
				//me_line_map_ptr_input->map_cu_mode_inner [me_line_map_ptr_input->curr_ctu_idx][x_scu_line + j][y_scu_line + i] = src->map_cu_mode[j + (i << (log2_src_cuw - 2))];
				me_line_map_ptr_output->map_scu_inner    [me_line_map_ptr_input->curr_ctu_idx][x_scu_line + j][y_scu_line + i] = src->map_scu    [j + (i << (log2_src_cuw - 2))];
				me_line_map_ptr_output->map_cu_mode_inner[me_line_map_ptr_input->curr_ctu_idx][x_scu_line + j][y_scu_line + i] = src->map_cu_mode[j + (i << (log2_src_cuw - 2))];
			}
		}
		core_idx += md_fw_ptr->max_cuwh >> 2;
	}
}

void store_cu_data(ENC_CU_DATA* src, ENC_CU_DATA* dst)
{
#pragma HLS inline
STORE_TOP13:
	for (int j = 0; j < 16; j++)
	{
		int idx_dst = (U22)((j) << (6 - 2));
		int idx_src = (U22)((j) << (6 - 2));
		for (int ii = 0; ii < 16; ii++)
		{
#pragma HLS PIPELINE
			for (int k = 0; k < 5; k++)
			{
				for (int i = 0; i < (NUM_BLOCK_SHAPE); i++)
				{
					(dst->split_mode[k][i] + idx_dst)[ii] = (src->split_mode[k][i] + idx_src)[ii];
				}
			}

			(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
			(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
			(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
			(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
			(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];

			for (int i = 0; i < 8; i++)
			{
				(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];
			}

			(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
			(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
			(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
			(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
			(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];

			for (int i = 0; i < 2; i++)
			{
				(dst->refi + idx_dst)[ii][i] = (src->refi + idx_src)[ii][i];
			}

			(dst->umve_flag + idx_dst)[ii] = (src->umve_flag + idx_src)[ii];
			(dst->umve_idx + idx_dst)[ii] = (src->umve_idx + idx_src)[ii];
			(dst->skip_idx + idx_dst)[ii] = (src->skip_idx + idx_src)[ii];
			(dst->mvr_idx + idx_dst)[ii] = (src->mvr_idx + idx_src)[ii];
			(dst->mvp_from_hmvp_flag + idx_dst)[ii] = (src->mvp_from_hmvp_flag + idx_src)[ii];
			(dst->ipf_flag + idx_dst)[ii] = (src->ipf_flag + idx_src)[ii];
			for (int i1 = 0; i1 < 2; i1++)
			{
				for (int i2 = 0; i2 < 2; i2++)
				{
					(dst->mv + idx_dst)[ii][i1][i2] = (src->mv + idx_src)[ii][i1][i2];
					(dst->mvd + idx_dst)[ii][i1][i2] = (src->mvd + idx_src)[ii][i1][i2];
				}
			}

			for (int k = 0; k < 3; k++)
			{
				(dst->num_nz_coef[k] + idx_dst)[ii] = (src->num_nz_coef[k] + idx_src)[ii];
			}

			(dst->pb_part + idx_dst)[ii] = (src->pb_part + idx_src)[ii];
			(dst->tb_part + idx_dst)[ii] = (src->tb_part + idx_src)[ii];

		}
	}
}

void store_enc_info_top_1(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, 
	U12 cup,
	U4 cud, U2 cons_pred_mode, U7 cu_width, U7 cu_height, U2 tree_status, MD_RESULT md_best_result[2],
	ENC_CU_DATA md_best_ctu_output[2],
	ENC_CU_DATA* md2_output, pel dbk_y[71][71], 
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], U1 best_index)
{
#pragma HLS inline
	U8 cu_width1;
	U8 cu_height1;
	U8 i, ii, i1, i2;
	U8 j;
	U8 k;

	ENC_CU_DATA* dst = md2_output;
	ENC_CU_DATA* src = &md_best_ctu_output[best_index];
	S8* src_map_ipm = src->ipm[0];
	U18 core_idx = 0;

	U13 x = 0;
	U13 y = 0;
	U22 idx_dst;
	U22 idx_src;

	U7 src_cuw = md_fw_ptr->max_cuwh;
	U7 src_cuh = md_fw_ptr->max_cuwh;
	U3 log2_src_cuw = (com_tbl_log2[src_cuw]);
	U3 log2_src_cuh = (com_tbl_log2[src_cuh]);
	U13 x_scu_line;
	U13 y_scu_line;
	U6 w;
	U6 h;

	cu_width1 = md_fw_ptr->max_cuwh;
	cu_height1 = md_fw_ptr->max_cuwh;
	x = md_input_ptr->pix_x;
	y = md_input_ptr->pix_y;
	if (x >= md_fw_ptr->pic_width || y >= md_fw_ptr->pic_height)
		return;

	if (x + cu_width1 > md_fw_ptr->pic_width) {
		cu_width1 = md_fw_ptr->pic_width - x;
	}
	if (y + cu_height1 > md_fw_ptr->pic_height) {
		cu_height1 = md_fw_ptr->pic_height - y;
	}
	w = cu_width1 >> 2;
	h = cu_height1 >> 2;
	x_scu_line = (md_input_ptr->pix_x >> 2) - me_line_map_ptr_input->min_scu_x;
	y_scu_line = (md_input_ptr->pix_y >> 2) - me_line_map_ptr_input->min_scu_y;
#if 0
	STORE_TOP11:
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		src_map_ipm += src_cuw >> 2;
	}
#endif

	store_me_line(w, h, src, cur_ctu_mi, me_line_map_ptr_input, me_line_map_ptr_output, x_scu_line, y_scu_line, log2_src_cuw, md_fw_ptr);
	store_cu_data(src, dst);
}

void store_enc_info_top_paral(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, 
	U12 cup,
	U4 cud, U2 cons_pred_mode, U7 cu_width, U7 cu_height, U2 tree_status, MD_RESULT md_best_result[2],
	ENC_CU_DATA md_best_ctu_output[2],
	ENC_CU_DATA* md2_output, pel dbk_y[71][71], 
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], U1 best_index)
{
#pragma HLS inline
	store_enc_info_top_0(md_best_ctu_output, md2_output, dbk_y, best_index);
	store_enc_info_top_1(md_fw_ptr, md_input_ptr, 
		cup,
		cud, cons_pred_mode, cu_width, cu_height, tree_status, md_best_result,
		md_best_ctu_output,
		md2_output, dbk_y, 
		cur_ctu_mi,
		me_line_map_ptr_input, me_line_map_ptr_output,
		p_fenc_LCU_Y, best_index);
}

void store_enc_info_top(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, 
	U12 cup,
	U4 cud, U2 cons_pred_mode, U7 cu_width, U7 cu_height, U2 tree_status, MD_RESULT md_best_result[2],
	ENC_CU_DATA md_best_ctu_output[2],
	ENC_CU_DATA* md2_output, pel dbk_y[71][71], 
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr_input,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH])
{
#pragma HLS INLINE off
#if STORE_ENC_INFO_TOP_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_tmp->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_tmp->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_input->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_input->map_cu_mode_inner complete dim=1

#pragma HLS ARRAY_PARTITION variable=md2_output->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=md2_output->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md2_output->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=md2_output->num_nz_coef cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].mpm_ext cyclic factor=8 dim=1	
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].mvd cyclic factor=2 dim=3		
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].num_nz_coef cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_output[0].reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=dbk_y complete dim=2

#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].refi complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=2
#endif

	U1 best_index = md_best_result[0].cost_best < md_best_result[1].cost_best ? 0 : 1;

	com_set_split_mode(md_best_result[best_index].best_split_mode, cud, 0, cu_width, cu_height, cu_width, md_best_ctu_output[best_index].split_mode);

#if 0
	update_to_ctx_map_top_luma(md_fw_ptr, md_input_ptr, me_line_map_ptr, cur_ctu_mi, &md_best_ctu_output[best_index]);
	copy_cu_data(md2_output, &md_best_ctu_output[best_index], 0, 0, 6, 6, 6, 0, TREE_LC, p_fenc_LCU_Y);
	Copy_LCU_To_Top_linebuffer(md_fw_ptr, md_input_ptr, md_best_ctu_output[best_index].reco_y, dbk_y);
	write_back_inner_line_map(me_line_map_ptr_input, me_line_map_ptr_tmp);
#else
	store_enc_info_top_paral(md_fw_ptr, md_input_ptr, 
		cup,
		cud, cons_pred_mode, cu_width, cu_height, tree_status, md_best_result,
		md_best_ctu_output,
		md2_output, dbk_y, 
		cur_ctu_mi,
		me_line_map_ptr_input, me_line_map_ptr_input,
		p_fenc_LCU_Y, best_index);
#endif
}

#if !CIRCUIT64_DUMMY
#if !ONLY_QT_QT
void circuit_64x64_64x32_32x64_branch_top(U2 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs_father[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs_father[(64 >> 2)],
	SKIP_MERGE_INPUT* skip_in, MD_RESULT* md2_best_result_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	ENC_CU_DATA* md2_best_ctu_output_ptr,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	U13 x0 = md_input_ptr->pix_x;
	U13 y0 = md_input_ptr->pix_y;
	U3 cu_width_log2 = md_fw_ptr->log2_max_cuwh;
	U3 cu_height_log2 = md_fw_ptr->log2_max_cuwh;
	U8 cu_width = 1 << cu_width_log2;
	U8 cu_height = 1 << cu_height_log2;
	s64 cost_temp = MAX_S64;
	//U1 split_allow[5 + 1];
	static ENC_CU_DATA_ARRAY curr_cu_data_temp_array;
	U3 part_num = 0;
	//U3 num_split_to_try = 0;
	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };
	U1 do_split = 1;

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->num_nz_coef cyclic factor=3 dim=1

#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->split_mode cyclic factor=7 dim=2	
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=md2_best_ctu_output_ptr->num_nz_coef cyclic factor=3 dim=1

	md2_com_check_split_mode_top(x0, y0, cu_width, cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height, split_mode, &do_split);

	curr_cu_data_temp_array.last_child = TRUE;
	curr_cu_data_temp_array.x_chroma = x0;
	curr_cu_data_temp_array.y_chroma = y0;
	print_cu_split(x0, y0, cu_width, cu_height, TREE_LC, part_num, 1, curr_cu_data_temp_array.x_chroma, curr_cu_data_temp_array.y_chroma, curr_cu_data_temp_array.last_child, 0);
	//printf("\n");

	if (do_split) {
		COM_SPLIT_STRUCT split_struct;
		cost_temp = md2_prepare_cu_info_COM_combine_top(md_fw_ptr, md_input_ptr->lambda_y, &curr_cu_data_temp_array, x0, y0, 0, 0,
			0, 0, cu_width, cu_height, split_mode, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS loop_tripcount max=2
			cost_temp += circuit_64x64_64x32_32x64(
				md_fw_ptr, md_input_ptr, g_rmd_output,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_64,
				up_nebs_father, left_nebs_father,
				skip_in,
				&curr_cu_data_temp_array, split_mode,
				0, split_mode != NO_SPLIT ? 1 : 0, cons_pred_mode_child[0], TREE_LC, part_num, split_struct, cu_width_log2,
				cu_height_log2, fme_mv_info,
				IP_buffer_ptr_64x64,
				MPM_buffer_ptr_64x64,
				IP_map_scu_ptr_64x64,
				ref,
				Fetch_Ref_window_ptr, split_mode);

		}

		cost_compare_fixed_point_top_new(cost_temp, &curr_cu_data_temp_array, cu_width_log2, cu_height_log2, 0, TREE_LC,
			split_mode, md2_best_result_ptr, cons_pred_mode_child[0], md2_best_ctu_output_ptr);
	}
}
#endif
#endif

#if PATCH_DEBUG
extern PATCH_INFO* patch;
#endif

void copy_IP_buffer(U13 x0, IP_ARRAY* IP_buffer_ptr) {
	int i;
#if PATCH_DEBUG
	if (patch->patch_switch)
	{
		for (i = 0; i < 128; i++)
		{
			IP_buffer_ptr->up[x0 + i] = 128;
		}
		if (x0 == patch->left_pel)
		{
			for (int i = 0; i < 64; i++)
			{
				IP_buffer_ptr->left[i] = 128;
			}
		}
	}
#endif
	//for (i = 0; i < 128; i++) {
	   // IP_buffer_ptr->up_temp[i] = IP_buffer_ptr->up[x0 + i];
	//}
	///*copy up_temp*/
	//for (i = 0; i < 128; i++) {
	   // for (int j = 0; j < 23; j++)
	   //	 IP_buffer_ptr->up_temp_size[j][i] = IP_buffer_ptr->up_temp[i];
	//}
	///*copy left*/
	//for (i = 0; i < 64; i++) {
	   // for (int j = 0; j < 23; j++)
	   //	 IP_buffer_ptr->left_size[j][i] = IP_buffer_ptr->left[i];
	//}

}
void write_in_para_set_md(FME_OUT_PARA_SET* fme_out_para_set, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr) {
	int iter;

	/////Frame-level paramters
	md_fw_ptr->pic_width          = fme_out_para_set->pic_width;
	md_fw_ptr->pic_height         = fme_out_para_set->pic_height;
	md_fw_ptr->slice_type         = fme_out_para_set->slice_type;
	md_fw_ptr->pic_width_in_scu   = fme_out_para_set->pic_width_in_scu;
	md_fw_ptr->pic_height_in_scu  = fme_out_para_set->pic_height_in_scu;
	md_fw_ptr->log2_max_cuwh      = fme_out_para_set->log2_max_cuwh;
	md_fw_ptr->log2_culine        = fme_out_para_set->log2_culine;
	md_fw_ptr->max_cuwh           = fme_out_para_set->max_cuwh;
	md_fw_ptr->bit_depth_internal = fme_out_para_set->bit_depth_internal;
	//md_fw_ptr->f_scu = fme_out_para_set->f_scu;
	/* gop size */
	//md_fw_ptr->gop_size = fme_out_para_set->gop_size;
#if MIPF
	md_fw_ptr->mipf_enable_flag = fme_out_para_set->mipf_enable_flag;
#endif
#if EIPM
	md_fw_ptr->eipm_enable_flag = fme_out_para_set->eipm_enable_flag;
#endif
#if INTERPF
	md_fw_ptr->interpf_enable_flag = fme_out_para_set->interpf_enable_flag;
#endif
#if UMVE_ENH
	md_fw_ptr->umve_set_flag = fme_out_para_set->umve_set_flag;
#endif
#if CTX_CONTROL_LUKE
	//md_fw_ptr->bit_depth_internal = fme_out_para_set->bit_depth_internal;
	md_fw_ptr->max_part_ratio  = fme_out_para_set->max_part_ratio;
	md_fw_ptr->min_cu_size     = fme_out_para_set->min_cu_size;
	md_fw_ptr->max_split_times = fme_out_para_set->max_split_times;
	md_fw_ptr->min_qt_size     = fme_out_para_set->min_qt_size;
	md_fw_ptr->max_bt_size     = fme_out_para_set->max_bt_size;
	md_fw_ptr->max_eqt_size    = fme_out_para_set->max_eqt_size;
	//md_fw_ptr->affine_subblock_size_idx = fme_out_para_set->affine_subblock_size_idx;
	//md_fw_ptr->ipcm_enable_flag = fme_out_para_set->ipcm_enable_flag;
	//md_fw_ptr->position_based_transform_enable_flag = fme_out_para_set->position_based_transform_enable_flag;
	//md_fw_ptr->secondary_transform_enable_flag = fme_out_para_set->secondary_transform_enable_flag;
	//md_fw_ptr->tscpm_enable_flag = fme_out_para_set->tscpm_enable_flag;
	//md_fw_ptr->amvr_enable_flag = fme_out_para_set->amvr_enable_flag;
	//md_fw_ptr->emvr_enable_flag = fme_out_para_set->emvr_enable_flag;
#endif
	for (iter = 0; iter < 17; iter++)
	{
		md_fw_ptr->refp_ptr[iter][0] = fme_out_para_set->refp_ptr[iter][0];
		md_fw_ptr->refp_ptr[iter][1] = fme_out_para_set->refp_ptr[iter][1];
	}
#if ENABLE_BFRAME
	md_fw_ptr->num_refp[0] = fme_out_para_set->num_refp[0];
	md_fw_ptr->num_refp[1] = fme_out_para_set->num_refp[1];
#else
	md_fw_ptr->num_refp = fme_out_para_set->num_refp;
#endif

	md_fw_ptr->cur_ptr = fme_out_para_set->cur_ptr; // ref pic

	md_input_ptr->dist_chroma_weight[0] = fme_out_para_set->dist_chroma_weight[0];
	md_input_ptr->dist_chroma_weight[1] = fme_out_para_set->dist_chroma_weight[1];

	md_input_ptr->pix_x = fme_out_para_set->pix_x;
	md_input_ptr->pix_y = fme_out_para_set->pix_y;
	for (int i = 0; i < 3; i++)
		md_input_ptr->lambda[i] = fme_out_para_set->lambda[i];

	md_input_ptr->lambda_y = fme_out_para_set->lambda[0];
	md_input_ptr->lambda_u = fme_out_para_set->lambda[1];
	md_input_ptr->lambda_v = fme_out_para_set->lambda[2];

	md_input_ptr->qp_y = fme_out_para_set->qp_y;
	md_input_ptr->qp_u = fme_out_para_set->qp_u;
	//md_input_ptr->qp_v = fme_out_para_set->qp_v;
	//md_input_ptr->dist_chroma_weight[0] = fme_out_para_set->dist_chroma_weight[0];
	//md_input_ptr->dist_chroma_weight[1] = fme_out_para_set->dist_chroma_weight[1];
	md_input_ptr->x_last_ctu = fme_out_para_set->x_last_ctu;
	md_input_ptr->y_last_ctu = fme_out_para_set->y_last_ctu;

	md_input_ptr->CtrPos2MD[0][0][0] = fme_out_para_set->CtrPos2MD[0][0][0];
	md_input_ptr->CtrPos2MD[0][0][1] = fme_out_para_set->CtrPos2MD[0][0][1];
	md_input_ptr->CtrPos2MD[0][1][0] = fme_out_para_set->CtrPos2MD[0][1][0];
	md_input_ptr->CtrPos2MD[0][1][1] = fme_out_para_set->CtrPos2MD[0][1][1];
	md_input_ptr->CtrPos2MD[1][0][0] = fme_out_para_set->CtrPos2MD[1][0][0];
	md_input_ptr->CtrPos2MD[1][0][1] = fme_out_para_set->CtrPos2MD[1][0][1];
	md_input_ptr->CtrPos2MD[1][1][0] = fme_out_para_set->CtrPos2MD[1][1][0];
	md_input_ptr->CtrPos2MD[1][1][1] = fme_out_para_set->CtrPos2MD[1][1][1];

#if ENABLE_BFRAME
	md_input_ptr->CtrPos2MD_ref1[0][0][0] = fme_out_para_set->CtrPos2MD_ref1[0][0][0];
	md_input_ptr->CtrPos2MD_ref1[0][0][1] = fme_out_para_set->CtrPos2MD_ref1[0][0][1];
	md_input_ptr->CtrPos2MD_ref1[0][1][0] = fme_out_para_set->CtrPos2MD_ref1[0][1][0];
	md_input_ptr->CtrPos2MD_ref1[0][1][1] = fme_out_para_set->CtrPos2MD_ref1[0][1][1];
	md_input_ptr->CtrPos2MD_ref1[1][0][0] = fme_out_para_set->CtrPos2MD_ref1[1][0][0];
	md_input_ptr->CtrPos2MD_ref1[1][0][1] = fme_out_para_set->CtrPos2MD_ref1[1][0][1];
	md_input_ptr->CtrPos2MD_ref1[1][1][0] = fme_out_para_set->CtrPos2MD_ref1[1][1][0];
	md_input_ptr->CtrPos2MD_ref1[1][1][1] = fme_out_para_set->CtrPos2MD_ref1[1][1][1];
#endif // ENABLE_BFRAME
#if USE_INTRA_REFRESH
	md_input_ptr->EncRefresh  = fme_out_para_set->EncRefresh;
	md_input_ptr->RefreshMode = fme_out_para_set->RefreshMode;
	md_input_ptr->RefreshSize = fme_out_para_set->RefreshSize;
	md_input_ptr->pirStartRow = fme_out_para_set->pirStartRow;
	md_input_ptr->pirEndRow   = fme_out_para_set->pirEndRow;
	md_input_ptr->pirStartCol = fme_out_para_set->pirStartCol;
	md_input_ptr->pirEndCol   = fme_out_para_set->pirEndCol;
#endif
#if USE_SPEED_LEVEL
	md_input_ptr->speed_level = fme_out_para_set->speed_level;
#endif
#if USE_ROI_MODE_CONFIG
	md_input_ptr->valid_mode_config[0] = fme_out_para_set->valid_mode[0];
	md_input_ptr->valid_mode_config[1] = fme_out_para_set->valid_mode[1];
	md_input_ptr->valid_mode_config[2] = fme_out_para_set->valid_mode[2];
#endif
#if CUDQP_QP_MAP
	md_input_ptr->cu_dqp_enable               = fme_out_para_set->cu_dqp_enable;
	md_input_ptr->cu_qp_group_pred_qp         = fme_out_para_set->cu_qp_group_pred_qp;
	md_input_ptr->cu_qp_group_area_size       = fme_out_para_set->cu_qp_group_area_size;
	md_input_ptr->chroma_quant_param_delta_cb = fme_out_para_set->chroma_quant_param_delta_cb;
	md_input_ptr->chroma_quant_param_delta_cr = fme_out_para_set->chroma_quant_param_delta_cr;
	md_input_ptr->md_slice_qp                 = fme_out_para_set->qp_y;
	for (int i = 0; i < 256; i++)
	{
		md_input_ptr->CuQPMap[i] = fme_out_para_set->CuQPMap[i];
	}
#endif

}
void write_out_para_set_md(FME_OUT_PARA_SET* in_para, MD_OUT_PARA_SET* out_para) {

	/////Frame-level paramters
	out_para->pic_width = in_para->pic_width;
	out_para->pic_height = in_para->pic_height;

	out_para->pix_x = in_para->pix_x;
	out_para->pix_y = in_para->pix_y;
}


//#define MD_TB1
#ifdef MD_TB1
#include "md_testbench.h"
FILE* fp_md_run_output_golden = NULL;
FILE* fp_md_run_input_golden = NULL;
#endif

#ifdef MD_TB1
static void md_run_get_golden_data(bool b_input, int f_cu, int ptr, strFetch_ref_window1* Fetch_Ref_window_ptr, strFetch_Orig_Lcu1* Fetch_Orig_Lcu_ptr, ENC_ME_LINE_MAP1* me_line_map_ptr,
	ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U32 left_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 left_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U32 cur_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], SKIP_MERGE_INPUT* skip_in, RMD_OUTPUT* rmd_output,
	ENC_FME_INFO_ALL* fme_mv_info, ENC_CU_DATA1* md2_output, pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39], FME_OUT_PARA_SET* fme_out_para_set,
	MD_OUT_PARA_SET* md_out_para_set, s8  cu_data_split_mode[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU], ENC_ME_LINE_MAP2aec* fetch_output_me_line_map_ptr)
{
	static const int ptr_max = 5, f_cu_max = 10;
	if (b_input)
	{
		/******************************************************tb_data_input********************************************/
		if (fp_md_run_input_golden == NULL) {
			fp_md_run_input_golden = fopen("md_run_input_golden.dat", "wb");
			assert(fp_md_run_input_golden);
		}

		if (ptr < ptr_max && f_cu < f_cu_max) {

			printf("ptr:%d, f_cu:%d\n", ptr, f_cu);
			MD_PORT* md = new MD_PORT(Fetch_Ref_window_ptr, Fetch_Orig_Lcu_ptr, me_line_map_ptr, left_ctu_mi, cur_ctu_mi, left_ctu_scu, left_ctu_cu_mode, cur_ctu_scu, cur_ctu_cu_mode, inner_nebs,
				above_nebs_line, skip_in, rmd_output, fme_mv_info, md2_output, dbk_y, dbk_u, dbk_v, fme_out_para_set, md_out_para_set, cu_data_split_mode, fetch_output_me_line_map_ptr);

			size_t cnt = md->write_bin(fp_md_run_input_golden);
			printf("md_input_golden=%d\n", cnt);
			if (!cnt) {
				printf("Write input error!\n");
			}

			fflush(stdout);
			delete md;
		}

		if (ptr >= ptr_max && fp_md_run_input_golden != NULL) { fclose(fp_md_run_input_golden); }
		/******************************************************tb_data_input********************************************/
	}
	else
	{
		/******************************************************tb_data_output********************************************/


		if (fp_md_run_output_golden == NULL) {
			fp_md_run_output_golden = fopen("md_run_output_golden.dat", "wb");
			assert(fp_md_run_output_golden);
		}

		if (ptr < ptr_max && f_cu < f_cu_max) {

			MD_PORT* md = new MD_PORT(Fetch_Ref_window_ptr, Fetch_Orig_Lcu_ptr, me_line_map_ptr, left_ctu_mi, cur_ctu_mi, left_ctu_scu, left_ctu_cu_mode, cur_ctu_scu, cur_ctu_cu_mode,
				inner_nebs, above_nebs_line, skip_in, rmd_output, fme_mv_info, md2_output, dbk_y, dbk_u, dbk_v,
				fme_out_para_set, md_out_para_set, cu_data_split_mode, fetch_output_me_line_map_ptr);

			size_t cnt = md->write_bin(fp_md_run_output_golden);
			printf("md_output_golden=%d\n", cnt);
			if (!cnt) {
				printf("Write output error!\n");
			}

			fflush(stdout);

			delete md;
		}

		if (ptr >= ptr_max && fp_md_run_output_golden != NULL)
		{
			fclose(fp_md_run_output_golden);
		}

		/******************************************************tb_data_output********************************************/
	}

}
#endif

static void md1_line(MD_INPUT* md_input_ptr, ENC_ME_LINE_MAP* me_line_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr) {
	int i = 0;
	int j = 0;
	U11 x_scu = me_line_ptr->min_scu_x - 1;
	U11 y_scu = me_line_ptr->min_scu_y;
	U2 scan_type = 0;
	U1 bu_idx = 0;
	U1 curr_ctu_idx = 0;
	U16 x_last_ctu = md_input_ptr->x_last_ctu;
	U16 y_last_ctu = md_input_ptr->y_last_ctu;
	U13 x_cur_ctu = md_input_ptr->pix_x;
	U13 y_cur_ctu = md_input_ptr->pix_y;
	if (x_cur_ctu == 0 && y_cur_ctu == 0) {
		scan_type = 0;
	}
	else if (x_cur_ctu > x_last_ctu && y_cur_ctu == y_last_ctu) {
		scan_type = 1;
	}
	else if (x_cur_ctu <= x_last_ctu && y_cur_ctu > y_last_ctu) {
		scan_type = 2;
	}
	else {
		scan_type = 3;
	}

#if PATCH_DEBUG
	if (patch->patch_switch && patch->pat == 0)
	{
		scan_type = 0;
	}
#endif

	if (((int)scan_type) == 0) {
		for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
			for (int j = 0; j < 1 << 7 >> 1 >> 2; j++) {
				MAP_SCU_IP_ptr->map_scu_inner[0][i][j] = 0;
				MAP_SCU_IP_ptr->map_scu_inner[1][i][j] = 0;
			}
		}
		for (i = 0; i < me_line_ptr->pic_width_in_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=960
			MAP_SCU_IP_ptr->map_scu_above_line[0][i] = 0;
			MAP_SCU_IP_ptr->map_scu_above_line[1][i] = 0;
		}
		MAP_SCU_IP_ptr->above_line_idx = 0;
		MAP_SCU_IP_ptr->curr_ctu_idx = 0;
	}
	else if (scan_type == 1) {
		x_scu = x_last_ctu >> 2;
		y_scu = y_last_ctu >> 2;
		bu_idx = 1 - me_line_ptr->above_line_idx;
		curr_ctu_idx = 1 - me_line_ptr->curr_ctu_idx;
		for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
			MAP_SCU_IP_ptr->map_scu_above_line[bu_idx][x_scu + i] = MAP_SCU_IP_ptr->map_scu_inner[curr_ctu_idx][i][(1 << 7 >> 1 >>
				2) - 1];
		}
		MAP_SCU_IP_ptr->curr_ctu_idx = 1 - curr_ctu_idx;
		for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
			for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {
				MAP_SCU_IP_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][i][j] = ((u32)0);
			}
		}
	}
	else if (scan_type == 2) {
		x_scu = x_last_ctu >> 2;
		y_scu = y_last_ctu >> 2;
		bu_idx = me_line_ptr->above_line_idx;
		curr_ctu_idx = 1 - me_line_ptr->curr_ctu_idx;
		for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
			if (x_scu + i < me_line_ptr->pic_width_in_scu) {
				MAP_SCU_IP_ptr->map_scu_above_line[bu_idx][x_scu + i] = MAP_SCU_IP_ptr->map_scu_inner[curr_ctu_idx][i][(1 << 7 >> 1 >>
					2) - 1];

			}
			else {
				break;
			}
		}
		MAP_SCU_IP_ptr->curr_ctu_idx = 1 - curr_ctu_idx;
		for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
			for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {
				MAP_SCU_IP_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][i][j] = ((u32)0);
				MAP_SCU_IP_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][i][j] = ((u32)0);

			}
		}
		for (i = 0; i < me_line_ptr->pic_width_in_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=960
			MAP_SCU_IP_ptr->map_scu_above_line[1 - me_line_ptr->above_line_idx][i] = ((u32)0);
		}
		MAP_SCU_IP_ptr->above_line_idx = 1 - MAP_SCU_IP_ptr->above_line_idx;
	}
	MAP_SCU_IP_ptr->min_scu_x = x_cur_ctu >> 2;
	MAP_SCU_IP_ptr->min_scu_y = y_cur_ctu >> 2;
}


void rdoq_model_init(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt)
{
	int i;
	for (i = 0; i < 12; i++) {
		rdoq_model->last1[i] = PROB_INIT;
		model_cnt->last1[i] = 0;
	}
	for (i = 0; i < 22; i++) {
		rdoq_model->last2[i] = PROB_INIT;
		model_cnt->last2[i] = 0;
	}
	for (i = 0; i < 24; i++) {
		rdoq_model->run_rdoq[i] = PROB_INIT;
		model_cnt->run_rdoq[i] = 0;
		rdoq_model->level[i] = PROB_INIT;
		model_cnt->level[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		rdoq_model->cbf[i] = PROB_INIT;
		model_cnt->cbf[i] = 0;
	}
}

static void construct_inner_line_map(MD_INPUT* md_input, ENC_ME_LINE_MAP* dst, ENC_ME_LINE_MAP* src, U32 left_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 left_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
	int i, j;
	bool first_lcu = (md_input->pix_x == 0 && md_input->pix_y == 0);
	dst->min_scu_x = src->min_scu_x;
	dst->min_scu_y = src->min_scu_y;
	//dst->stride_in_map = src->stride_in_map;
	//dst->stride_in_line = src->stride_in_line;
	dst->pic_width_in_scu = src->pic_width_in_scu;
	//dst->pic_height_in_scu = src->pic_height_in_scu;
	dst->above_line_idx = src->above_line_idx;
	dst->curr_ctu_idx = src->curr_ctu_idx;
	//dst->x_ctu = src->x_ctu;
	//dst->y_ctu = src->y_ctu;

	for (i = 0; i < ABOVE_LINE_LEN; i++) {
#pragma HLS PIPELINE
		dst->new_map_scu_above_line[i] = src->new_map_scu_above_line[i];
		dst->new_map_cu_mode_above_line[i] = src->new_map_cu_mode_above_line[i];
	}

	for (i = 0; i < SCU_IN_LCU_NUMS_LEN; i++) {
#pragma HLS PIPELINE
		for (j = 0; j < SCU_IN_LCU_NUMS_LEN; j++) {
			if (first_lcu) {
				dst->map_scu_inner[1 - dst->curr_ctu_idx][i][j] = 0;
				dst->map_cu_mode_inner[1 - dst->curr_ctu_idx][i][j] = 0;
			}
			else {
				dst->map_scu_inner[1 - dst->curr_ctu_idx][i][j] = left_ctu_scu[i][j];
				dst->map_cu_mode_inner[1 - dst->curr_ctu_idx][i][j] = left_ctu_cu_mode[i][j];
			}

			dst->map_scu_inner[dst->curr_ctu_idx][i][j] = 0;
			dst->map_cu_mode_inner[dst->curr_ctu_idx][i][j] = 0;
		}
	}
}

static void construct_inner_line_map_chroma(MD_INPUT* md_input, ENC_ME_LINE_MAP* dst, ENC_ME_LINE_MAP* src, U32 left_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 left_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
	int i, j;
	bool first_lcu = (md_input->pix_x == 0 && md_input->pix_y == 0);
	dst->min_scu_x = src->min_scu_x;
	dst->min_scu_y = src->min_scu_y;
	//dst->stride_in_map = src->stride_in_map;
	//dst->stride_in_line = src->stride_in_line;
	dst->pic_width_in_scu = src->pic_width_in_scu;
	//dst->pic_height_in_scu = src->pic_height_in_scu;
	dst->above_line_idx = src->above_line_idx;
	dst->curr_ctu_idx = src->curr_ctu_idx;
	//dst->x_ctu = src->x_ctu;
	//dst->y_ctu = src->y_ctu;

	for (i = 0; i < ABOVE_LINE_LEN; i++)
	{
		dst->new_map_scu_above_line[i] = src->new_map_scu_above_line[i];
		dst->new_map_cu_mode_above_line[i] = src->new_map_cu_mode_above_line[i];
	}


	for (i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
		for (j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			if (first_lcu)
			{
				dst->map_scu_inner[1 - dst->curr_ctu_idx][i][j] = 0;
				dst->map_cu_mode_inner[1 - dst->curr_ctu_idx][i][j] = 0;
			}
			else
			{
				dst->map_scu_inner[1 - dst->curr_ctu_idx][i][j] = left_ctu_scu[i][j];
				dst->map_cu_mode_inner[1 - dst->curr_ctu_idx][i][j] = left_ctu_cu_mode[i][j];
			}

			dst->map_scu_inner[dst->curr_ctu_idx][i][j] = 0;
			dst->map_cu_mode_inner[dst->curr_ctu_idx][i][j] = 0;
		}
	}

}


static void init_neb_info(ENC_ME_NEB_INFO* neb)
{
	int i = 0;
	//neb->scu = scu;
	for (i = 0; i < 2; i++) {

#pragma HLS loop_tripcount max = 2
		neb->mv[i][0] = ((s16)0);
		neb->mv[i][1] = ((s16)0);
		neb->refi[i] = ((s8)(-1));
	}
	//neb->is_valid = ((u8 )0);
}


static void construct_inner_nebs(MD_INPUT* md_input, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
	int i, j;
	bool first_lcu = (md_input->pix_x == 0 && md_input->pix_y == 0);
	for (i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
		for (j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			if (first_lcu)
			{
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[0][0] = 0;
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[1][0] = 0;
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[0][1] = 0;
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[1][1] = 0;
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].refi[0] = 0;
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].refi[1] = 0;
			}
			else
			{
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[0][0] = left_ctu_mi[i][j].mv[0][0];
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[1][0] = left_ctu_mi[i][j].mv[1][0];
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[0][1] = left_ctu_mi[i][j].mv[0][1];
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].mv[1][1] = left_ctu_mi[i][j].mv[1][1];
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].refi[0] = left_ctu_mi[i][j].refi[0];
				inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][i][j].refi[1] = left_ctu_mi[i][j].refi[1];
			}

			init_neb_info(&inner_nebs[me_line_map_ptr->curr_ctu_idx][i][j]);
		}
	}

}


static void md_run_top_prepare(FME_OUT_PARA_SET* fme_out_para_set, MD_FW* md_fw, MD_INPUT* md_input,
	MD_INPUT* md_input_ptr, ENC_ME_LINE_MAP* me_line_map_ptr_input,
	U32 left_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 left_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U13& x0, U13& y0, U3& cu_width_log2, U3& cu_height_log2, U8& cu_width, U8& cu_height,
	U1* do_split, ENC_CU_DATA_ARRAY curr_cu_data_temp_array)
{

	write_in_para_set_md(fme_out_para_set, md_fw, md_input);

	if (md_input_ptr->pix_x == 0 && md_input_ptr->pix_y == 0)
	{
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{
				for (int k = 0; k < 16; ++k)
				{
					me_line_map_ptr_input->map_scu_inner[i][j][k] = 0;
					me_line_map_ptr_input->map_cu_mode_inner[i][j][k] = 0;
				}
			}
		}
	}

	//copy_IP_buffer(fme_out_para_set->pix_x, IP_buffer_ptr);

	//construct_inner_line_map(md_input_ptr, me_line_map_ptr_tmp, me_line_map_ptr_input, left_ctu_scu, left_ctu_cu_mode);

	//construct_inner_nebs(md_input_ptr, me_line_map_ptr_tmp, inner_nebs, left_ctu_mi);

	//md1_line(md_input_ptr, me_line_map_ptr_tmp, MAP_SCU_IP_ptr);

	x0 = md_input_ptr->pix_x;
	y0 = md_input_ptr->pix_y;
	cu_width_log2 = md_fw->log2_max_cuwh;
	cu_height_log2 = md_fw->log2_max_cuwh;
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);

	md1_com_check_split_mode_top(x0, y0, cu_width, cu_height, md_fw->pic_width, md_fw->pic_height, SPLIT_QUAD, do_split);

	curr_cu_data_temp_array.last_child = TRUE;
	curr_cu_data_temp_array.x_chroma = x0;
	curr_cu_data_temp_array.y_chroma = y0;
}


static void md_run_top_prepare_chroma(
	FME_OUT_PARA_SET* fme_out_para_set, MD_FW* md_fw, MD_INPUT* md_input,
	IP_ARRAY* IP_buffer_ptr, MD_INPUT* md_input_ptr, ENC_ME_LINE_MAP* me_line_map_ptr_tmp, ENC_ME_LINE_MAP* me_line_map_ptr_input,
	U32 left_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 left_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	IP_MAP_SCU* MAP_SCU_IP_ptr, U13& x0, U13& y0, U3& cu_width_log2, U3& cu_height_log2, U8& cu_width, U8& cu_height,
	U1* do_split, ENC_CU_DATA_ARRAY curr_cu_data_temp_array)
{
	/*rdoq_model_init(rdoq_model, model_cnt);
	rdoq_model_init(rdoq_model_c, model_cnt_c);*/

	write_in_para_set_md(fme_out_para_set, md_fw, md_input);

	copy_IP_buffer(fme_out_para_set->pix_x, IP_buffer_ptr);

	construct_inner_line_map(md_input_ptr, me_line_map_ptr_tmp, me_line_map_ptr_input, left_ctu_scu, left_ctu_cu_mode);

	md1_line(md_input_ptr, me_line_map_ptr_tmp, MAP_SCU_IP_ptr);

	x0 = md_input_ptr->pix_x;
	y0 = md_input_ptr->pix_y;
	cu_width_log2 = md_fw->log2_max_cuwh;
	cu_height_log2 = md_fw->log2_max_cuwh;
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);

	md1_com_check_split_mode_top(x0, y0, cu_width, cu_height, md_fw->pic_width, md_fw->pic_height, SPLIT_QUAD, do_split);

	curr_cu_data_temp_array.last_child = TRUE;
	curr_cu_data_temp_array.x_chroma = x0;
	curr_cu_data_temp_array.y_chroma = y0;
}


#if !ONLY_QT_QT
// MD TOP
void md_run(
	pel fme2mdrefbuf[2560][8][2][16],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,						// No scalar in.
#endif
	//ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], //out for dbk-sao
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],// out  for dbk-sao
	//ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_input_ptr,			// cped before use in circuit_32x32_top_qt()
	RMD_OUTPUT* rmd_output,						// No scalar in.
	ENC_FME_INFO_ALL* fme_mv_info,				// No scalar in.
	pel dbk_y[71][71],// out  for dbk-sao
	FME_OUT_PARA_SET* fme_out_para_set,			// cped before use in md_run_top_prepare().
	ENC_CU_DATA* md2_output,// out  for dbk-sao	// No scalar in.
	ENC_ME_LINE_MAP* me_line_map_ptr_input,		// cp here.
	ENC_ME_LINE_MAP* me_line_map_ptr_input_luma,
	strFetch_ref_window* Fetch_Ref_window_ptr)	// cped before use in circuit_32x32_top_qt().
{
#pragma HLS INLINE off

// =================================================================================================
// INTERFACE CONSTRAINTS
// =================================================================================================
#pragma HLS RESOURCE variable=fme2mdrefbuf core=RAM_1P_BRAM

#if 1
#pragma HLS RESOURCE variable=me_mv_dir->block64x64MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block32x32MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block64x32MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block32x64MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block16x16MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block32x16MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block16x32MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block64x16MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block16x64MvDir core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block8x8MvDir   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block16x8MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block8x16MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block32x8MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block8x32MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block64x8MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block8x64MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block32x4MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block4x32MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block16x4MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block4x16MvDir  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block8x4MvDir   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_dir->block4x8MvDir   core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=cur_ctu_mi core=RAM_1P_BRAM

#if 0
#pragma HLS INTERFACE axis port=skip_input_ptr->slice_type
#pragma HLS INTERFACE axis port=skip_input_ptr->ptr
#pragma HLS INTERFACE axis port=skip_input_ptr->umve_enable_flag
#pragma HLS INTERFACE axis port=skip_input_ptr->pix_x
#endif
#if 1
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_mv    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_refi  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_mv1   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_map_refi1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_list_ptr1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=skip_input_ptr->refp_ptr1      core=RAM_1P_BRAM
#endif

#if 1
#pragma HLS RESOURCE variable=rmd_output->modes_8x8   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_8x16  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x8  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_8x32  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x8  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x8  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_8x64  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_16x64 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_32x64 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=rmd_output->modes_64x64 core=RAM_1P_BRAM
#endif 

#if 1
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x64Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x32Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x64Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x16Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x64Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x8Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x64Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x4Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree4x32Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x32Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x16Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x16Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x32Fmv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x8Fmv   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x8Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x16Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x8Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x32Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree4x8Fmv   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x4Fmv   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x4Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree4x16Fmv  core=RAM_1P_BRAM
#endif 

#pragma HLS RESOURCE variable=dbk_y core=RAM_1P_BRAM

#if 1
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_width
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_height
#pragma HLS INTERFACE axis port=fme_out_para_set->slice_type
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_width_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_height_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set->log2_max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set->log2_culine
#pragma HLS INTERFACE axis port=fme_out_para_set->max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set->bit_depth_internal
#pragma HLS INTERFACE axis port=fme_out_para_set->max_part_ratio
#pragma HLS INTERFACE axis port=fme_out_para_set->min_cu_size
#pragma HLS INTERFACE axis port=fme_out_para_set->max_split_times
#pragma HLS INTERFACE axis port=fme_out_para_set->min_qt_size
#pragma HLS INTERFACE axis port=fme_out_para_set->max_bt_size
#pragma HLS INTERFACE axis port=fme_out_para_set->max_eqt_size
#pragma HLS INTERFACE axis port=fme_out_para_set->cur_ptr
#pragma HLS INTERFACE axis port=fme_out_para_set->pix_x
#pragma HLS INTERFACE axis port=fme_out_para_set->pix_y
#pragma HLS INTERFACE axis port=fme_out_para_set->eipm_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->mipf_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->interpf_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_y
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_u
#pragma HLS INTERFACE axis port=fme_out_para_set->x_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set->y_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set->umve_set_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->cu_dqp_enable
#pragma HLS INTERFACE axis port=fme_out_para_set->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=fme_out_para_set->cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=fme_out_para_set->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=fme_out_para_set->chroma_quant_param_delta_cr
#endif
#if 0
#pragma HLS INTERFACE axis port=fme_out_para_set->affine_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_v
#pragma HLS INTERFACE axis port=fme_out_para_set->md_slice_qp
#endif
#if 1
#pragma HLS RESOURCE variable=fme_out_para_set->refp_ptr            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->num_refp            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPosFME_ref1      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPos2MD_ref1      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->lambda              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->dist_chroma_weight  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPosFME           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPos2MD           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->p_fenc_LCU_Y        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->p_fenc_LCU_U        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->p_fenc_LCU_V        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->CuQPMap             core=RAM_1P_BRAM
#endif

#if 1
#pragma HLS RESOURCE variable=md2_output->split_mode                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pb_part                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->tb_part                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pred_mode                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mpm                            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mpm_ext                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->ipm                            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->refi                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mvr_idx                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->umve_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->umve_idx                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->skip_idx                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mvp_from_hmvp_flag             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mv                             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mvd                            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->num_nz_coef                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->map_scu                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->affine_flag                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->smvd_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->map_cu_mode                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->map_pb_tb_part                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->depth                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->ipf_flag                       core=RAM_1P_BRAM
// #pragma HLS RESOURCE variable=md2_output->coef                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->coef_y                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->coef_u                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->coef_v                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->reco_y                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->reco_u                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->reco_v                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sawp_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sawp_idx0                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sawp_idx1                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_mvr_flag0                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_mvr_idx0                   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_mvr_flag1                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_mvr_idx1                   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->bvr_idx                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->inter_filter_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->ipc_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->bgc_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->bgc_idx                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->ibc_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->cbvp_idx                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->cnt_hbvp_cands                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->map_usp                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sp_flag                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sub_string_no                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sp_strInfo                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sp_copy_direction              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->is_sp_pix_completed            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->cs2_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->evs_copy_direction             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->evs_sub_string_no              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->evs_str_copy_info              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->unpred_pix_num                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->unpred_pix_info                core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->equal_val_str_present_flag     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->unpredictable_pix_present_flag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pvbuf_size                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pvbuf_size_prev                core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->p_SRB                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pvbuf_reused_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->p_SRB_prev                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->all_comp_flag                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->all_comp_pre_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->m_dpb_idx                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->m_dpb_idx_prev                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->m_dpb_reYonly                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->m_dpb_reYonly_prev             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->cuS_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->cuS_pre_flag                   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pv_x                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pv_x_prev                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pv_y                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->pv_y_prev                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->srb_u                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->srb_v                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->affine_umve_flag               core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->affine_umve_idx                core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->etmvp_flag                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->mvap_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sub_tmvp_flag                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_idx0                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->awp_idx1                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->st_chroma_tu_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->iip_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->ist_tu_flag                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->est_tu_flag                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->sawp_mpm                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->y_org_sao                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->u_org_sao                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->v_org_sao                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2_output->cu_luma_qp                     core=RAM_1P_BRAM
#endif

#if 1
#pragma HLS RESOURCE variable=me_line_map_ptr_input->new_map_scu_above_line     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_input->new_map_cu_mode_above_line core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_input->map_scu_inner              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_input->map_cu_mode_inner          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_input->co_left_mv                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_input->co_left_refi               core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_input->co_left_ptr_ref            core=RAM_1P_BRAM
#endif 
#if 1
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->min_scu_x
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->min_scu_y
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->curr_ctu_idx
#endif
#if 0
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->stride_in_map
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->stride_in_line
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->pic_width_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->pic_height_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->above_line_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->x_ctu
#pragma HLS INTERFACE axis port=me_line_map_ptr_input->y_ctu
#endif

#if 0
#pragma HLS INTERFACE axis port=Fetch_Ref_window_ptr->code_stru
#endif
#if 1
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->RefWindowLCU
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->swOutLv0
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->CtrPos
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->RefWindowLCU_UP
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->RefWindowLCU_VP
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->lidx
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->cur_refi
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->refp_pic_ptr
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->refp_is_library_picture
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->CtrPos_ref1
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->RefWindowLCU_ref1
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->swOutLv0_ref1
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1
#pragma HLS RESOURCE variable=Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1
#endif
// =================================================================================================

// =================================================================================================
// PARTITION OF INTERFACE
// =================================================================================================
#if 1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=2

#pragma HLS ARRAY_PARTITION variable=md2_output->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md2_output->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md2_output->num_nz_coef cyclic factor=3 dim=1

#pragma HLS ARRAY_PARTITION variable=dbk_y complete dim=2

// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_tmp->map_scu_inner     complete dim=1
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_tmp->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_input->map_scu_inner     complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_input->map_cu_mode_inner complete dim=1
#endif 
// =================================================================================================

// =================================================================================================
// Latch STRUCTs (especially the scalar in structs)
// =================================================================================================
	SKIP_MERGE_INPUT * skip_input_local_ptr;
#if 1
	skip_input_local_ptr = skip_input_ptr;
#else
	SKIP_MERGE_INPUT skip_input_local;
	skip_input_local_ptr = &skip_input_local;
	skip_input_local.cp(skip_input_local_ptr, skip_input_ptr);
#endif

#if 0
	ENC_ME_LINE_MAP me_line_map_local;
	ENC_ME_LINE_MAP * me_line_map_local_ptr = &me_line_map_local;
	me_line_map_local.cp(me_line_map_local_ptr, me_line_map_ptr_input);
#endif

//  fme_out_para_set     will be copied before use in md_run_top_prepare().
//  skip_input_ptr       will be copied before use in circuit_32x32_top_qt().
//	Fetch_Ref_window_ptr will be copied before use in circuit_32x32_top_qt().
// =================================================================================================

	STATIC_C Arbitor ref;
	ref = fme2mdrefbuf;

	static ENC_CU_DATA md_best_ctu_data[2];
	static MD_RESULT md_best_result[2];
	md_best_result[0].cost_best = MAX_S64;
	md_best_result[1].cost_best = MAX_S64;

	static MD_FW md_fw;
	static MD_INPUT md_input;
	MD_FW* md_fw_ptr = &md_fw;
	MD_INPUT* md_input_ptr = &md_input;

	ENC_ME_LINE_MAP me_line_map_ptr_input_temp;

	U13 x0;
	U13 y0;
	U3 cu_width_log2;
	U3 cu_height_log2;
	U8 cu_width;
	U8 cu_height;
	s64 cost_temp = MAX_S64;
	static ENC_CU_DATA_ARRAY curr_cu_data_temp_array;
	U3 part_num = 0;
	COM_SPLIT_STRUCT split_struct;
	U2 cons_pred_mode_child;

	U1 do_split = 1;
	
// =================================================================================================
#if 1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp_array->num_nz_coef cyclic factor=3 dim=1

#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->coef_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->reco_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->split_mode cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->split_mode cyclic factor=7 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->mpm_ext cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->mpm cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->refi cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->mv cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->mvd cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=md_best_ctu_data->num_nz_coef cyclic factor=3 dim=2
#endif
// =================================================================================================

	MD_RUN_1:
	for (int i = 0; i < 256; i++) {
#pragma HLS PIPELINE
		for (int j = 0; j < 7; j++) {
			for (int k = 0; k < 5; k++) {
				md_best_ctu_data[0].split_mode[k][j][i] = NO_SPLIT;
				md_best_ctu_data[1].split_mode[k][j][i] = NO_SPLIT;
			}
		}
	}

// #if EIPM																	// Liaozhx: Move into md_run_top_prepare().
// 	md_fw_ptr->eipm_enable_flag = fme_out_para_set->eipm_enable_flag;
// #endif
// #if INTERPF
// 	md_fw_ptr->interpf_enable_flag = fme_out_para_set->interpf_enable_flag;
// #endif

	md_run_top_prepare(fme_out_para_set, md_fw_ptr, &md_input, md_input_ptr, 
		me_line_map_ptr_input, me_line_map_ptr_input->map_scu_inner[1 - me_line_map_ptr_input->curr_ctu_idx], me_line_map_ptr_input->map_cu_mode_inner[1 - me_line_map_ptr_input->curr_ctu_idx],
		//me_line_map_local_ptr, me_line_map_local_ptr->map_scu_inner[1 - me_line_map_local_ptr->curr_ctu_idx], me_line_map_local_ptr->map_cu_mode_inner[1 - me_line_map_local_ptr->curr_ctu_idx],
		x0, y0, cu_width_log2, cu_height_log2, cu_width, cu_height, &do_split, curr_cu_data_temp_array);

	ENC_ME_LINE_MAP_SIZE<64>	me_line_map_64;
	get_me_line_map_up  (me_line_map_64.map_scu_up_line,   me_line_map_64.map_cu_mode_up_line,   me_line_map_ptr_input, cu_width, cu_height, x0, y0);
	for (int i = 0; i < 17; ++i)
	{
		if (x0 + (i*4) >= md_fw.pic_width)
		{
			me_line_map_64.map_scu_up_line[i + 1] = 0;
			me_line_map_64.map_cu_mode_up_line[i + 1] = 0;
		}
	}
	if (x0 == 0)
	{
		for (int i = 0; i < 16; ++i)
		{
			me_line_map_64.map_scu_left_line[i] = 0;
			me_line_map_64.map_cu_mode_left_line[i] = 0;
		}
	}
	else
	{
		get_me_line_map_left(me_line_map_64.map_scu_left_line, me_line_map_64.map_cu_mode_left_line, me_line_map_ptr_input, cu_width, cu_height, x0, y0);
	}
	
	me_line_map_64.min_scu_x      = /*me_line_map_local_ptr*/ me_line_map_ptr_input->min_scu_x;
	me_line_map_64.min_scu_y      = /*me_line_map_local_ptr*/ me_line_map_ptr_input->min_scu_y;
	me_line_map_64.above_line_idx = /*me_line_map_local_ptr*/ me_line_map_ptr_input->above_line_idx;
	me_line_map_64.curr_ctu_idx   = /*me_line_map_local_ptr*/ me_line_map_ptr_input->curr_ctu_idx;

	ENC_ME_NEB_INFO			up_nebs[1 + 16 + 1];
	ENC_ME_NEB_INFO			left_nebs[16];
	static ENC_ME_NEB_INFO		nebs_line_hor[960];
	static ENC_ME_NEB_INFO		nebs_line_ver[16];
	static ENC_ME_NEB_INFO		nebs_bottom_right;
	cpy_1d_2_1d<S2, REFP_NUM>(nebs_bottom_right.refi, up_nebs[0].refi);
	cpy_2d_2_2d<S14, REFP_NUM, MV_D>(nebs_bottom_right.mv, up_nebs[0].mv);
MD_RUN_2:
	for (int i = 0; i < COM_MIN(16 + 1, (md_fw_ptr->pic_width - x0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		cpy_1d_2_1d<S2, REFP_NUM>(nebs_line_hor[(x0 >> 2) + i].refi, up_nebs[i + 1].refi);
		cpy_2d_2_2d<S14, REFP_NUM, MV_D>(nebs_line_hor[(x0 >> 2) + i].mv, up_nebs[i + 1].mv);
	}
MD_RUN_3:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		cpy_1d_2_1d<S2, REFP_NUM>(nebs_line_ver[i].refi, left_nebs[i].refi);
		cpy_2d_2_2d<S14, REFP_NUM, MV_D>(nebs_line_ver[i].mv, left_nebs[i].mv);
	}
	//get_up_nebs_line(up_nebs, new_above_nebs_line, inner_nebs, me_line_map_ptr_tmp, cu_width, cu_height, x0, y0);
	//get_left_nebs_line(left_nebs, inner_nebs, me_line_map_ptr_tmp, cu_width, cu_height, x0, y0);

	IP_MAP_SCU_SIZE<64, 64>	IP_map_scu_64x64 = {};
	static u32					map_scu_line_hor[960];
	static u32					map_scu_line_ver[16];
	static u32					map_scu_bottom_right;
MD_RUN_4:
	for (int i = 0; i < COM_MIN(32, (md_fw_ptr->pic_width - x0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=32
		IP_map_scu_64x64.map_scu_up_line[i] = y0 == 0 ? 0 : map_scu_line_hor[(x0 >> 2) + i];
	}
MD_RUN_5:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		IP_map_scu_64x64.map_scu_left_line_new[i] = x0 == 0 ? 0 : map_scu_line_ver[i];
	}
	IP_map_scu_64x64.map_scu_top_left_new = (y0 == 0 || x0 == 0) ? 0 : map_scu_bottom_right;

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;
	assert(y_pos_lcu == 0);
#if CDEBUG_MAP_SCU
	MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_64x64.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width, 1);
	MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_64x64.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height, 1);
	MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_64x64.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0, 1);
#endif // CDEBUG_MAP_SCU

	static MPM_ARRAY_SIZE<64, 64>	MPM_buffer_64x64 = {};
	static pel						mpm_line_hor[960];
	static pel						mpm_line_ver[16];
MD_RUN_6:
	if (y0 == 0)
	{
		for (int i = 0; i < 16; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			MPM_buffer_64x64.up_line[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_width - x0) >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			MPM_buffer_64x64.up_line[i] = mpm_line_hor[(x0 >> 2) + i];
		}
	}
MD_RUN_7:
	if (x0 == 0)
	{
		for (int i = 0; i < 16; ++i)
		{
			MPM_buffer_64x64.left_line[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			MPM_buffer_64x64.left_line[i] = mpm_line_ver[i];
		}
	}
	MPM_buffer_64x64.up = MPM_buffer_64x64.up_line[0];
	MPM_buffer_64x64.left = MPM_buffer_64x64.left_line[0];

	//MPM_buffer_ptr->get_mpm_up(&MPM_buffer_64x64.up, MPM_buffer_ptr, x0, y0, 1);
	//MPM_buffer_ptr->get_mpm_left(&MPM_buffer_64x64.left, MPM_buffer_ptr, x0, y0, 1);
	//MPM_buffer_ptr->get_mpm_up_line(MPM_buffer_64x64.up_line, MPM_buffer_ptr, x0, y0, cu_width, 1);
	//MPM_buffer_ptr->get_mpm_left_line(MPM_buffer_64x64.left_line, MPM_buffer_ptr, x0, y0, cu_height, 1);

	static IP_ARRAY_SIZE<64, 64>	IP_buffer_64x64 = {};
	static pel						rec_line_hor[3840];
	static pel						rec_line_ver[64];
	static pel						bottom_right;
MD_RUN_8:
	if (y0 == 0)
	{
		for (int i = 0; i < COM_MIN(128, md_fw_ptr->pic_width - x0); ++i)
		{
#pragma HLS LOOP_TRIPCOUNT max=128
			IP_buffer_64x64.up_line[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < COM_MIN(128, md_fw_ptr->pic_width - x0); ++i)
		{
#pragma HLS LOOP_TRIPCOUNT max=128
			IP_buffer_64x64.up_line[i] = rec_line_hor[x0 + i];
		}
		for (int i = COM_MIN(128, md_fw_ptr->pic_width - x0); i < 128; ++i)
		{
#pragma HLS LOOP_TRIPCOUNT max=128
			IP_buffer_64x64.up_line[i] = 0;
		}
	}
MD_RUN_9:
	if (x0 == 0)
	{
		for (int i = 0; i < 64; ++i)
		{
			IP_buffer_64x64.left_line[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < 64; ++i)
		{
			IP_buffer_64x64.left_line[i] = rec_line_ver[i];
		}
	}
	if (x0 == 0 || y0 == 0)
	{
		IP_buffer_64x64.top_left = 0;
	}
	else
	{
		IP_buffer_64x64.top_left = bottom_right;
	}

	if (do_split) {
		cons_pred_mode_child = NO_MODE_CONS;
		cost_temp = prepare_cu_info_COM_combine_only_qt(md_fw_ptr, md_input_ptr->lambda_y, &curr_cu_data_temp_array, x0, y0, 0, 0,
			0, 0, cu_width, cu_height, SPLIT_QUAD, &split_struct);

	MD_RUN_10:
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				IP_map_scu_64x64.cur_map_scu[i][j] = 0;
			}
		}

	MD_RUN_11:
		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=4
//#if !ONLY_QT_QT
#if 1
			cost_temp += circuit_32x32_top(md_fw_ptr, md_input_ptr, rmd_output,
				Fetch_Ref_window_ptr->RefWindowLCU_UP, Fetch_Ref_window_ptr->RefWindowLCU_VP, Fetch_Ref_window_ptr->CtrPos,
#if ENABLE_BFRAME
				Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1, Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1, Fetch_Ref_window_ptr->CtrPos_ref1, me_mv_dir,
#endif
				fme_out_para_set->p_fenc_LCU_Y,
				&me_line_map_64,
				up_nebs, left_nebs,
				skip_input_local_ptr, // skip_input_ptr,
				&md_best_ctu_data[0],
				1, 0, cons_pred_mode_child, TREE_LC, part_num, split_struct, cu_width_log2,
				fme_mv_info,
				&IP_buffer_64x64,
				&MPM_buffer_64x64,
				&IP_map_scu_64x64,
				ref,
				Fetch_Ref_window_ptr, SPLIT_QUAD);
#else
			Instruction mem_load[PR_SIZE];
				Instruction mem_pu[PR_SIZE];
				Instruction mem_store[PR_SIZE];
			cost_temp += circuit_32x32_top_qt(md_fw_ptr, md_input_ptr, rmd_output,
				Fetch_Ref_window_ptr->RefWindowLCU_UP, Fetch_Ref_window_ptr->RefWindowLCU_VP, Fetch_Ref_window_ptr->CtrPos,
#if ENABLE_BFRAME
				Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1, Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1, Fetch_Ref_window_ptr->CtrPos_ref1, me_mv_dir,
#endif
				fme_out_para_set->p_fenc_LCU_Y,
				&me_line_map_64,
				up_nebs, left_nebs,
				skip_input_local_ptr, // skip_input_ptr,
				&md_best_ctu_data[0],
				1, 0, cons_pred_mode_child, TREE_LC, part_num, split_struct, cu_width_log2,
				fme_mv_info,
				&IP_buffer_64x64,
				&MPM_buffer_64x64,
				&IP_map_scu_64x64,
				ref,
				Fetch_Ref_window_ptr, SPLIT_QUAD, mem_load, mem_pu, mem_store);
#endif
		}

		md_best_result[0].cost_best = cost_temp;
		md_best_result[0].best_split_mode = SPLIT_QUAD;
		md_best_result[0].best_cons_pred_mode = cons_pred_mode_child;

		//cost_compare_fixed_point_top_new(cost_temp, &curr_cu_data_temp_array, cu_width_log2, cu_height_log2, 0, TREE_LC, SPLIT_QUAD,
		//	&md_best_result[0], cons_pred_mode_child, &md_best_ctu_data[0]);
	}

#if !ONLY_QT_QT
	// pu<KERNEL_64X64>
#if !KERNEL_SIZE_CLOSE(64, 64)
	circuit_64x64_64x32_32x64_branch_top(NO_SPLIT, md_fw_ptr, md_input_ptr, rmd_output,
		Fetch_Ref_window_ptr->RefWindowLCU_UP, Fetch_Ref_window_ptr->RefWindowLCU_VP, Fetch_Ref_window_ptr->CtrPos,
#if ENABLE_BFRAME
		Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1, Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1, Fetch_Ref_window_ptr->CtrPos_ref1, me_mv_dir,
#endif
		fme_out_para_set->p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_input_local_ptr, // skip_input_ptr, 
		&md_best_result[1], fme_mv_info,
		&IP_buffer_64x64,
		&MPM_buffer_64x64,
		&IP_map_scu_64x64,
		&md_best_ctu_data[1],
		ref,
		Fetch_Ref_window_ptr);
#endif // !KERNEL_SIZE_CLOSE(64, 64)
	circuit_64x64_64x32_32x64_branch_top(SPLIT_BI_VER, md_fw_ptr, md_input_ptr, rmd_output,
		Fetch_Ref_window_ptr->RefWindowLCU_UP, Fetch_Ref_window_ptr->RefWindowLCU_VP, Fetch_Ref_window_ptr->CtrPos,
#if ENABLE_BFRAME
		Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1, Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1, Fetch_Ref_window_ptr->CtrPos_ref1, me_mv_dir,
#endif
		fme_out_para_set->p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_input_local_ptr, // skip_input_ptr, 
		&md_best_result[1], fme_mv_info,
		&IP_buffer_64x64,
		&MPM_buffer_64x64,
		&IP_map_scu_64x64,
		&md_best_ctu_data[1],
		ref,
		Fetch_Ref_window_ptr);
	circuit_64x64_64x32_32x64_branch_top(SPLIT_BI_HOR, md_fw_ptr, md_input_ptr, rmd_output,
		Fetch_Ref_window_ptr->RefWindowLCU_UP, Fetch_Ref_window_ptr->RefWindowLCU_VP, Fetch_Ref_window_ptr->CtrPos,
#if ENABLE_BFRAME
		Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1, Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1, Fetch_Ref_window_ptr->CtrPos_ref1, me_mv_dir,
#endif
		fme_out_para_set->p_fenc_LCU_Y,
		&me_line_map_64,
		up_nebs, left_nebs,
		skip_input_local_ptr, // skip_input_ptr, 
		&md_best_result[1], fme_mv_info,
		&IP_buffer_64x64,
		&MPM_buffer_64x64,
		&IP_map_scu_64x64,
		&md_best_ctu_data[1],
		ref,
		Fetch_Ref_window_ptr);
#endif
#if ONLY_QT_QT
	md_best_result[1].cost_best = MAX_S64;
	//assert(md_best_result[0].cost_best != MAX_S64);
#endif

	//*****************************************************************************************************
	// store_64x64
	copy_enc_me_line_map(me_line_map_ptr_input, &me_line_map_ptr_input_temp);
	store_enc_info_top(&md_fw, &md_input, 0, 0, NO_MODE_CONS, 64,
		64, TREE_LC, md_best_result, md_best_ctu_data, md2_output, dbk_y,
		cur_ctu_mi,
		&me_line_map_ptr_input_temp/*_luma*/,
		fme_out_para_set->p_fenc_LCU_Y);

	copy_enc_me_line_map(&me_line_map_ptr_input_temp, me_line_map_ptr_input_luma);

	bottom_right = rec_line_hor[x0 + 63];
MD_RUN_12:
	for (int i = 0; i < COM_MIN(64, md_fw_ptr->pic_width - x0); ++i) {
#pragma HLS LOOP_TRIPCOUNT max=64
		rec_line_hor[x0 + i] = md2_output->reco_y[63][i];
	}
MD_RUN_13:
	for (int i = 0; i < 64; ++i) {
		rec_line_ver[i] = md2_output->reco_y[i][63];
	}

	map_scu_bottom_right = map_scu_line_hor[(x0 >> 2) + 15];
MD_RUN_14:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_width - x0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		map_scu_line_hor[(x0 >> 2) + i] = md2_output->map_scu[240 + i];
	}
MD_RUN_15:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		map_scu_line_ver[i] = md2_output->map_scu[16 * i + 15];
	}

MD_RUN_16:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_width - x0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		mpm_line_hor[(x0 >> 2) + i] = md2_output->ipm[0][240 + i];
	}
MD_RUN_17:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		mpm_line_ver[i] = md2_output->ipm[0][16 * i + 15];
	}

	cpy_1d_2_1d<S2, REFP_NUM>(nebs_line_hor[(x0 >> 2) + 15].refi, nebs_bottom_right.refi);
	cpy_2d_2_2d<S14, REFP_NUM, MV_D>(nebs_line_hor[(x0 >> 2) + 15].mv, nebs_bottom_right.mv);
MD_RUN_18:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_width - x0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		cpy_1d_2_1d<S2, REFP_NUM>(cur_ctu_mi[i][15].refi, nebs_line_hor[(x0 >> 2) + i].refi);
		cpy_2d_2_2d<S14, REFP_NUM, MV_D>(cur_ctu_mi[i][15].mv, nebs_line_hor[(x0 >> 2) + i].mv);
	}
MD_RUN_19:
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		cpy_1d_2_1d<S2, REFP_NUM>(cur_ctu_mi[15][i].refi, nebs_line_ver[i].refi);
		cpy_2d_2_2d<S14, REFP_NUM, MV_D>(cur_ctu_mi[15][i].mv, nebs_line_ver[i].mv);
	}

}
#endif

extern int md_slice_qp;
void md_run_chroma(
	//ENC_ME_NEB_INFO left_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], //out for dbk-sao
	//ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],  //out for dbk-sao
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
	)
{
#define MD_RUN_CHROMA_AXIS 1

#if MD_RUN_CHROMA_AXIS
#if CHROMA_AXIS_ADAPTER
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->pic_width
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->pic_height
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->slice_type
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->log2_culine
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->max_cuwh
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->bit_depth_internal
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->max_split_times
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->min_qt_size
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->pix_x
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->pix_y
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->qp_y
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->qp_u
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->x_last_ctu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->y_last_ctu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->cu_dqp_enable
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_fme_out_para_set->md_slice_qp

#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->min_scu_x
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->min_scu_y
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->stride_in_map
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->stride_in_line
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->above_line_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->curr_ctu_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->x_ctu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_luma->y_ctu

#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->min_scu_x
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->min_scu_y
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->stride_in_map
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->stride_in_line
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->above_line_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->curr_ctu_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->x_ctu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_DBK->y_ctu

#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->min_scu_x
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->min_scu_y
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->stride_in_map
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->stride_in_line
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->above_line_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->curr_ctu_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->x_ctu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr_AEC->y_ctu
#else
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->pic_width
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->pic_height
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->slice_type
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->pic_width_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->pic_height_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->log2_max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->log2_culine
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->max_cuwh
#if CTX_CONTROL_LUKE
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->bit_depth_internal
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->max_split_times
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->min_qt_size
#endif
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->pix_x
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->pix_y
#if MIPF
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->mipf_enable_flag
#endif
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->qp_y
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->qp_u
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->x_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->y_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->cu_dqp_enable
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=fme_out_para_set_md2chroma->md_slice_qp

#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->min_scu_x
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->min_scu_y
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->stride_in_map
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->stride_in_line
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->pic_width_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->pic_height_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->above_line_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->curr_ctu_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->x_ctu
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->y_ctu

#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->min_scu_x
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->min_scu_y
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->stride_in_map
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->stride_in_line
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->pic_width_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->pic_height_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->above_line_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->curr_ctu_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->x_ctu
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK->y_ctu

#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->min_scu_x
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->min_scu_y
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->stride_in_map
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->stride_in_line
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->pic_width_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->pic_height_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->above_line_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->curr_ctu_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->x_ctu
#pragma HLS INTERFACE axis port=me_line_map_ptr_AEC->y_ctu
#endif

// #if CHROMA_AXIS_ADAPTER
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->sp_strInfo->is_matched
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->sp_strInfo->length
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->sp_strInfo->offset_x
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->sp_strInfo->offset_y
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->sp_strInfo->n_recent_flag
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->sp_strInfo->n_recent_idx
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->is_matched
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->length
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->match_type
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->srb_index
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->pv_type
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->pvflag
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->srb_bits
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->srb_dist
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->esc_flag
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->evs_str_copy_info->pos
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->unpred_pix_info->Y
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->unpred_pix_info->U
// #pragma HLS INTERFACE axis port=axis_ctu_map_cu_data->unpred_pix_info->V
// #else
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->sp_strInfo->is_matched
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->sp_strInfo->length
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->sp_strInfo->offset_x
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->sp_strInfo->offset_y
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->sp_strInfo->n_recent_flag
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->sp_strInfo->n_recent_idx
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->is_matched
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->length	
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->match_type
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->srb_index
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->pv_type
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->pvflag
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->srb_bits
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->srb_dist
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->esc_flag
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->evs_str_copy_info->pos	
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->unpred_pix_info->Y
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->unpred_pix_info->U
// #pragma HLS INTERFACE axis port=ctu_map_cu_data->unpred_pix_info->V	
// #endif
// #if CHROMA_AXIS_ADAPTER
// #pragma HLS INTERFACE axis port=axis_md_output_luma->sp_strInfo->is_matched
// #pragma HLS INTERFACE axis port=axis_md_output_luma->sp_strInfo->length
// #pragma HLS INTERFACE axis port=axis_md_output_luma->sp_strInfo->offset_x
// #pragma HLS INTERFACE axis port=axis_md_output_luma->sp_strInfo->offset_y
// #pragma HLS INTERFACE axis port=axis_md_output_luma->sp_strInfo->n_recent_flag
// #pragma HLS INTERFACE axis port=axis_md_output_luma->sp_strInfo->n_recent_idx
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->is_matched
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->length	
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->match_type
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->srb_index
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->pv_type
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->pvflag
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->srb_bits
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->srb_dist
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->esc_flag
// #pragma HLS INTERFACE axis port=axis_md_output_luma->evs_str_copy_info->pos	
// #pragma HLS INTERFACE axis port=axis_md_output_luma->unpred_pix_info->Y
// #pragma HLS INTERFACE axis port=axis_md_output_luma->unpred_pix_info->U
// #pragma HLS INTERFACE axis port=axis_md_output_luma->unpred_pix_info->V
// #else
// #pragma HLS INTERFACE axis port=md_output_luma->sp_strInfo->is_matched
// #pragma HLS INTERFACE axis port=md_output_luma->sp_strInfo->length
// #pragma HLS INTERFACE axis port=md_output_luma->sp_strInfo->offset_x
// #pragma HLS INTERFACE axis port=md_output_luma->sp_strInfo->offset_y
// #pragma HLS INTERFACE axis port=md_output_luma->sp_strInfo->n_recent_flag
// #pragma HLS INTERFACE axis port=md_output_luma->sp_strInfo->n_recent_idx
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->is_matched
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->length	
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->match_type
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->srb_index
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->pv_type
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->pvflag
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->srb_bits
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->srb_dist
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->esc_flag
// #pragma HLS INTERFACE axis port=md_output_luma->evs_str_copy_info->pos	
// #pragma HLS INTERFACE axis port=md_output_luma->unpred_pix_info->Y
// #pragma HLS INTERFACE axis port=md_output_luma->unpred_pix_info->U
// #pragma HLS INTERFACE axis port=md_output_luma->unpred_pix_info->V
// #endif

// #if CHROMA_AXIS_ADAPTER
// #pragma HLS INTERFACE axis port=axis_Fetch_Ref_window_ptr->code_stru
// #else
// #pragma HLS INTERFACE axis port=Fetch_Ref_window_md2chroma->code_stru
// #endif

// #if CHROMA_AXIS_ADAPTER
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->min_scu_x
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->min_scu_y
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->stride_in_map
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->stride_in_line
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->pic_width_in_scu
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->pic_height_in_scu
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->above_line_idx
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->curr_ctu_idx
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->x_ctu
// #pragma HLS INTERFACE axis port=axis_me_line_map_ptr->y_ctu
// #else
// #pragma HLS INTERFACE axis port=me_line_map_ptr->min_scu_x
// #pragma HLS INTERFACE axis port=me_line_map_ptr->min_scu_y
// #pragma HLS INTERFACE axis port=me_line_map_ptr->stride_in_map
// #pragma HLS INTERFACE axis port=me_line_map_ptr->stride_in_line
// #pragma HLS INTERFACE axis port=me_line_map_ptr->pic_width_in_scu
// #pragma HLS INTERFACE axis port=me_line_map_ptr->pic_height_in_scu
// #pragma HLS INTERFACE axis port=me_line_map_ptr->above_line_idx
// #pragma HLS INTERFACE axis port=me_line_map_ptr->curr_ctu_idx
// #pragma HLS INTERFACE axis port=me_line_map_ptr->x_ctu
// #pragma HLS INTERFACE axis port=me_line_map_ptr->y_ctu
// #endif
#endif // MD_RUN_CHROMA_AXIS



//#pragma HLS RESOURCE variable=left_ctu_mi 										core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=cur_ctu_mi 										core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=inner_nebs_md_chroma 									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=new_above_nebs_line 								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=new_above_nebs_line_DBK 							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbk_u												core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbk_v												core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->refp_ptr						core=RAM_1P_BRAM
#if ENABLE_BFRAME
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->num_refp					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->CtrPosFME_ref1				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->CtrPos2MD_ref1				core=RAM_1P_BRAM
#else
#endif

#if CTU_CONTROL_LUKE
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->lambda						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->dist_chroma_weight			core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->CtrPosFME						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->CtrPos2MD						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->p_fenc_LCU_Y					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->p_fenc_LCU_U					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->p_fenc_LCU_V					core=RAM_1P_BRAM

#if USE_ROI_MODE_CONFIG
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->valid_mode					core=RAM_1P_BRAM
#endif
#if CUDQP_QP_MAP
#pragma HLS RESOURCE variable=fme_out_para_set_md2chroma->CuQPMap						core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=ctu_map_cu_data->split_mode							core=RAM_1P_BRAM
#if TB_SPLIT_EXT
#pragma HLS RESOURCE variable=ctu_map_cu_data->pb_part							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->tb_part							core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=ctu_map_cu_data->pred_mode								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->mpm									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->mpm_ext								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->ipm									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->refi									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->mvr_idx								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->umve_flag								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->umve_idx								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->skip_idx								core=RAM_1P_BRAM

#if EXT_AMVR_HMVP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->mvp_from_hmvp_flag				core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK->num_nz_coef						core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=ctu_map_cu_data->mv									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->mvd									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->num_nz_coef							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->map_scu								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->affine_flag							core=RAM_1P_BRAM
#if SMVD
	#pragma HLS RESOURCE variable=ctu_map_cu_data->smvd_flag							core=RAM_1P_BRAM
#endif
	#pragma HLS RESOURCE variable=ctu_map_cu_data->map_cu_mode						core=RAM_1P_BRAM
#if TB_SPLIT_EXT
	#pragma HLS RESOURCE variable=ctu_map_cu_data->map_pb_tb_part					core=RAM_1P_BRAM
#endif
#pragma HLS RESOURCE variable=ctu_map_cu_data->depth									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->ipf_flag								core=RAM_1P_BRAM
// #pragma HLS RESOURCE variable=ctu_map_cu_data->coef									core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->coef_y								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->coef_u								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->coef_v								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->reco_y								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->reco_u								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data->reco_v								core=RAM_1P_BRAM
#if SAWP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sawp_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sawp_idx0							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sawp_idx1							core=RAM_1P_BRAM
#endif

#if AWP_MVR
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_mvr_flag0						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_mvr_idx0						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_mvr_flag1						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_mvr_idx1						core=RAM_1P_BRAM
#endif

#if IBC_ABVR
	#pragma HLS RESOURCE variable=ctu_map_cu_data->bvr_idx							core=RAM_1P_BRAM
#endif
#if INTERPF
	#pragma HLS RESOURCE variable=ctu_map_cu_data->inter_filter_flag					core=RAM_1P_BRAM
#endif
#if IPC
	#pragma HLS RESOURCE variable=ctu_map_cu_data->ipc_flag							core=RAM_1P_BRAM
#endif
#if BGC
	#pragma HLS RESOURCE variable=ctu_map_cu_data->bgc_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->bgc_idx							core=RAM_1P_BRAM
#endif

#if USE_IBC
	#pragma HLS RESOURCE variable=ctu_map_cu_data->ibc_flag							core=RAM_1P_BRAM
#endif
#if IBC_BVP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->cbvp_idx							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->cnt_hbvp_cands					core=RAM_1P_BRAM
#endif
#if USE_SP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->map_usp							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sp_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sub_string_no						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sp_strInfo->match_dict			core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sp_strInfo->pixel					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sp_copy_direction					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->is_sp_pix_completed				core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->cs2_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->evs_copy_direction				core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->evs_sub_string_no					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->evs_str_copy_info->pixel			core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->unpred_pix_num					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->equal_val_str_present_flag		core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->unpredictable_pix_present_flag	core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pvbuf_size						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pvbuf_size_prev					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->p_SRB								core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pvbuf_reused_flag					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->p_SRB_prev						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->all_comp_flag						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->all_comp_pre_flag					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->m_dpb_idx							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->m_dpb_idx_prev					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->m_dpb_reYonly						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->m_dpb_reYonly_prev				core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->cuS_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->cuS_pre_flag						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pv_x								core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pv_x_prev							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pv_y								core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->pv_y_prev							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->srb_u								core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->srb_v								core=RAM_1P_BRAM
#endif

#if AFFINE_UMVE
	#pragma HLS RESOURCE variable=ctu_map_cu_data->affine_umve_flag					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->affine_umve_idx					core=RAM_1P_BRAM
#endif
#if ETMVP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->etmvp_flag						core=RAM_1P_BRAM
#endif
#if UNIFIED_HMVP_1
	#pragma HLS RESOURCE variable=ctu_map_cu_data->mvap_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sub_tmvp_flag						core=RAM_1P_BRAM
#endif
#if AWP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_flag							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_idx0							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->awp_idx1							core=RAM_1P_BRAM
#endif
#if ST_CHROMA
	#pragma HLS RESOURCE variable=ctu_map_cu_data->st_chroma_tu_flag					core=RAM_1P_BRAM
#endif
#if IIP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->iip_flag							core=RAM_1P_BRAM
#endif
#if IST
	#pragma HLS RESOURCE variable=ctu_map_cu_data->ist_tu_flag						core=RAM_1P_BRAM
#endif
#if EST
	#pragma HLS RESOURCE variable=ctu_map_cu_data->est_tu_flag						core=RAM_1P_BRAM
#endif
#if SAWP
	//u8** sawp_mpm;
	#pragma HLS RESOURCE variable=ctu_map_cu_data->sawp_mpm							core=RAM_1P_BRAM
#endif // SAWP

	#pragma HLS RESOURCE variable=ctu_map_cu_data->y_org_sao							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->u_org_sao							core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=ctu_map_cu_data->v_org_sao							core=RAM_1P_BRAM
#if CUDQP_QP_MAP
	#pragma HLS RESOURCE variable=ctu_map_cu_data->cu_luma_qp						core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=md2sao->y_org_sao 								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2sao->u_org_sao 								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2sao->v_org_sao 								core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr_luma->new_map_scu_above_line		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->new_map_cu_mode_above_line	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->map_scu_inner				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->map_cu_mode_inner			core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->co_left_mv					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->co_left_refi				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->co_left_ptr_ref				core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->RefWindowLCU			core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->swOutLv0				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->CtrPos				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_UP		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_VP		core=RAM_1P_BRAM

#if ENABLE_BFRAME
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->lidx						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->cur_refi					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->refp_pic_ptr				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->refp_is_library_picture	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->CtrPos_ref1				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_ref1			core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->swOutLv0_ref1				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1		core=RAM_2P_BRAM
#pragma HLS RESOURCE variable=Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1		core=RAM_2P_BRAM
#endif

#pragma HLS RESOURCE variable=md_output_luma->split_mode						core=RAM_1P_BRAM
#if TB_SPLIT_EXT

	#pragma HLS RESOURCE variable=md_output_luma->pb_part						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->tb_part						core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=md_output_luma->pred_mode							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->mpm								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->mpm_ext							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->ipm								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->refi								core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=md_output_luma->mvr_idx							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->umve_flag							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->umve_idx							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->skip_idx							core=RAM_1P_BRAM

#if EXT_AMVR_HMVP
	#pragma HLS RESOURCE variable=md_output_luma->mvp_from_hmvp_flag			core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=md_output_luma->mv								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->mvd								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->num_nz_coef						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->map_scu							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->affine_flag						core=RAM_1P_BRAM

#if SMVD
	#pragma HLS RESOURCE variable=md_output_luma->smvd_flag						core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=md_output_luma->map_cu_mode						core=RAM_1P_BRAM
#if TB_SPLIT_EXT
	#pragma HLS RESOURCE variable=md_output_luma->map_pb_tb_part				core=RAM_1P_BRAM
#endif
#pragma HLS RESOURCE variable=md_output_luma->depth								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->ipf_flag							core=RAM_1P_BRAM
// #pragma HLS RESOURCE variable=md_output_luma->coef								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->coef_y							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->coef_u							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->coef_v							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->reco_y							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->reco_u							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->reco_v							core=RAM_1P_BRAM
#if SAWP
	#pragma HLS RESOURCE variable=md_output_luma->sawp_flag						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->sawp_idx0						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->sawp_idx1						core=RAM_1P_BRAM
#endif

#if AWP_MVR
	#pragma HLS RESOURCE variable=md_output_luma->awp_mvr_flag0					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->awp_mvr_idx0					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->awp_mvr_flag1					core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->awp_mvr_idx1					core=RAM_1P_BRAM
#endif

#if IBC_ABVR
	#pragma HLS RESOURCE variable=md_output_luma->bvr_idx						core=RAM_1P_BRAM
#endif
#if INTERPF
	#pragma HLS RESOURCE variable=md_output_luma->inter_filter_flag				core=RAM_1P_BRAM
#endif
#if IPC
	#pragma HLS RESOURCE variable=md_output_luma->ipc_flag						core=RAM_1P_BRAM
#endif
#if BGC
	#pragma HLS RESOURCE variable=md_output_luma->bgc_flag						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->bgc_idx						core=RAM_1P_BRAM
#endif

#if USE_IBC
	#pragma HLS RESOURCE variable=md_output_luma->ibc_flag						core=RAM_1P_BRAM
#endif
#if IBC_BVP
	#pragma HLS RESOURCE variable=md_output_luma->cbvp_idx						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->cnt_hbvp_cands				core=RAM_1P_BRAM
#endif

#if USE_SP
	#pragma HLS RESOURCE variable=md_output_luma->map_usp						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->sp_flag						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->sub_string_no					core=RAM_1P_BRAM



#pragma HLS RESOURCE variable=md_output_luma->sp_strInfo->match_dict			core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->sp_strInfo->pixel					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->sp_copy_direction					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->is_sp_pix_completed				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->cs2_flag							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->evs_copy_direction				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->evs_sub_string_no					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->evs_str_copy_info->pixel			core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->unpred_pix_num					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->equal_val_str_present_flag		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->unpredictable_pix_present_flag	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pvbuf_size						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pvbuf_size_prev					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->p_SRB								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pvbuf_reused_flag					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->p_SRB_prev						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->all_comp_flag						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->all_comp_pre_flag					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->m_dpb_idx							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->m_dpb_idx_prev					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->m_dpb_reYonly						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->m_dpb_reYonly_prev				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->cuS_flag							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->cuS_pre_flag						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pv_x								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pv_x_prev							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pv_y								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->pv_y_prev							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->srb_u								core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->srb_v								core=RAM_1P_BRAM
#endif

#if AFFINE_UMVE
	#pragma HLS RESOURCE variable=md_output_luma->affine_umve_flag				core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->affine_umve_idx				core=RAM_1P_BRAM
#endif
#if ETMVP
	#pragma HLS RESOURCE variable=md_output_luma->etmvp_flag					core=RAM_1P_BRAM
#endif
#if UNIFIED_HMVP_1
	#pragma HLS RESOURCE variable=md_output_luma->mvap_flag						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->sub_tmvp_flag					core=RAM_1P_BRAM
#endif
#if AWP
	#pragma HLS RESOURCE variable=md_output_luma->awp_flag						core=RAM_1P_BRAM

	#pragma HLS RESOURCE variable=md_output_luma->awp_idx0						core=RAM_1P_BRAM
	#pragma HLS RESOURCE variable=md_output_luma->awp_idx1						core=RAM_1P_BRAM
#endif
#if ST_CHROMA
	#pragma HLS RESOURCE variable=md_output_luma->st_chroma_tu_flag				core=RAM_1P_BRAM
#endif
#if IIP
	#pragma HLS RESOURCE variable=md_output_luma->iip_flag						core=RAM_1P_BRAM
#endif
#if IST
	#pragma HLS RESOURCE variable=md_output_luma->ist_tu_flag					core=RAM_1P_BRAM
#endif
#if EST
	#pragma HLS RESOURCE variable=md_output_luma->est_tu_flag					core=RAM_1P_BRAM
#endif
#if SAWP
	#pragma HLS RESOURCE variable=md_output_luma->sawp_mpm						core=RAM_1P_BRAM
#endif // SAWP
#pragma HLS RESOURCE variable=md_output_luma->y_org_sao							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->u_org_sao							core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma->v_org_sao							core=RAM_1P_BRAM
#if CUDQP_QP_MAP
	#pragma HLS RESOURCE variable=md_output_luma->cu_luma_qp					core=RAM_1P_BRAM
#endif

#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->new_map_scu_above_line		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->new_map_cu_mode_above_line	core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->map_scu_inner				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->map_cu_mode_inner			core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->co_left_mv					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->co_left_refi					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_DBK->co_left_ptr_ref				core=RAM_1P_BRAM


#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->new_map_scu_above_line		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->new_map_cu_mode_above_line	core=RAM_1P_BRAM


#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->map_scu_inner				core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->map_cu_mode_inner			core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->co_left_mv					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->co_left_refi					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_AEC->co_left_ptr_ref				core=RAM_1P_BRAM

#if 1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0][0].refi complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0][0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0][0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=inner_nebs_md_chroma complete dim=3
#pragma HLS ARRAY_PARTITION variable=new_above_nebs_line_DBK complete dim=0
#pragma HLS ARRAY_PARTITION variable=dbk_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_v complete dim=2

#pragma HLS ARRAY_PARTITION variable=ctu_map_cu_data->split_mode cyclic factor=16  dim=3
#pragma HLS ARRAY_PARTITION variable=ctu_map_cu_data->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=ctu_map_cu_data_DBK->split_mode cyclic factor=16  dim=3
#pragma HLS ARRAY_PARTITION variable=fme_out_para_set_md2chroma->p_fenc_LCU_Y cyclic factor=64 dim=1 
#pragma HLS ARRAY_PARTITION variable=fme_out_para_set_md2chroma->p_fenc_LCU_U cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=fme_out_para_set_md2chroma->p_fenc_LCU_V cyclic factor=32 dim=1  

#pragma HLS array_partition variable=md2sao->y_org_sao complete dim=2
#pragma HLS array_partition variable=md2sao->u_org_sao complete dim=2
#pragma HLS array_partition variable=md2sao->v_org_sao complete dim=2
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_luma->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_luma->map_cu_mode_inner complete dim=1

#pragma HLS ARRAY_PARTITION variable=md_output_luma->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md_output_luma->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md_output_luma->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=md_output_luma->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=md_output_luma->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=md_output_luma->ipm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=md_output_luma->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md_output_luma->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md_output_luma->num_nz_coef cyclic factor=3 dim=1

#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK->new_map_scu_above_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK->new_map_cu_mode_above_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK->co_left_mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK->co_left_refi complete dim=1

#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->new_map_scu_above_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->new_map_cu_mode_above_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->map_scu_inner complete dim=3
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->map_cu_mode_inner complete dim=3
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->co_left_mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_AEC->co_left_refi complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->co_left_mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->co_left_refi complete dim=1
#endif

#if CHROMA_AXIS_ADAPTER
	fme_out_para_set_md2chroma->pic_width 					= axis_fme_out_para_set->pic_width;
	fme_out_para_set_md2chroma->pic_height 					= axis_fme_out_para_set->pic_height;
	fme_out_para_set_md2chroma->slice_type					= axis_fme_out_para_set->slice_type;
	fme_out_para_set_md2chroma->pic_width_in_scu			= axis_fme_out_para_set->pic_width_in_scu;
	fme_out_para_set_md2chroma->pic_height_in_scu			= axis_fme_out_para_set->pic_height_in_scu;
	fme_out_para_set_md2chroma->log2_max_cuwh				= axis_fme_out_para_set->log2_max_cuwh;
	fme_out_para_set_md2chroma->log2_culine					= axis_fme_out_para_set->log2_culine;
	fme_out_para_set_md2chroma->max_cuwh					= axis_fme_out_para_set->max_cuwh;
	fme_out_para_set_md2chroma->bit_depth_internal			= axis_fme_out_para_set->bit_depth_internal;
	fme_out_para_set_md2chroma->max_split_times				= axis_fme_out_para_set->max_split_times;
	fme_out_para_set_md2chroma->min_qt_size					= axis_fme_out_para_set->min_qt_size;
	fme_out_para_set_md2chroma->pix_x						= axis_fme_out_para_set->pix_x;
	fme_out_para_set_md2chroma->pix_y						= axis_fme_out_para_set->pix_y;
	fme_out_para_set_md2chroma->mipf_enable_flag			= axis_fme_out_para_set->mipf_enable_flag;												
	fme_out_para_set_md2chroma->qp_y						= axis_fme_out_para_set->qp_y;
	fme_out_para_set_md2chroma->qp_u						= axis_fme_out_para_set->qp_u;
	fme_out_para_set_md2chroma->x_last_ctu					= axis_fme_out_para_set->x_last_ctu;
	fme_out_para_set_md2chroma->y_last_ctu					= axis_fme_out_para_set->y_last_ctu;												
	fme_out_para_set_md2chroma->cu_dqp_enable				= axis_fme_out_para_set->cu_dqp_enable;
	fme_out_para_set_md2chroma->cu_qp_group_area_size		= axis_fme_out_para_set->cu_qp_group_area_size;
	fme_out_para_set_md2chroma->cu_qp_group_pred_qp			= axis_fme_out_para_set->cu_qp_group_pred_qp;
	fme_out_para_set_md2chroma->chroma_quant_param_delta_cb	= axis_fme_out_para_set->chroma_quant_param_delta_cb;
	fme_out_para_set_md2chroma->md_slice_qp					= axis_fme_out_para_set->md_slice_qp;

	me_line_map_ptr_luma->min_scu_x							= axis_me_line_map_ptr_luma->min_scu_x;
	me_line_map_ptr_luma->min_scu_y							= axis_me_line_map_ptr_luma->min_scu_y;
	me_line_map_ptr_luma->stride_in_map						= axis_me_line_map_ptr_luma->stride_in_map;
	me_line_map_ptr_luma->stride_in_line					= axis_me_line_map_ptr_luma->stride_in_line;
	me_line_map_ptr_luma->pic_width_in_scu					= axis_me_line_map_ptr_luma->pic_width_in_scu;
	me_line_map_ptr_luma->pic_height_in_scu					= axis_me_line_map_ptr_luma->pic_height_in_scu;
	me_line_map_ptr_luma->above_line_idx					= axis_me_line_map_ptr_luma->above_line_idx;
	me_line_map_ptr_luma->curr_ctu_idx						= axis_me_line_map_ptr_luma->curr_ctu_idx;
	me_line_map_ptr_luma->x_ctu								= axis_me_line_map_ptr_luma->x_ctu;
	me_line_map_ptr_luma->y_ctu								= axis_me_line_map_ptr_luma->y_ctu;

	// ENC_CU_DATA_COPY ctu_map_cu_data_local; // Latch locally
	// ctu_map_cu_data_local->sp_strInfo->is_matched			= axis_ctu_map_cu_data->sp_strInfo->is_matched;
	// ctu_map_cu_data_local->sp_strInfo->length				= axis_ctu_map_cu_data->sp_strInfo->length;
	// ctu_map_cu_data_local->sp_strInfo->offset_x				= axis_ctu_map_cu_data->sp_strInfo->offset_x;
	// ctu_map_cu_data_local->sp_strInfo->offset_y				= axis_ctu_map_cu_data->sp_strInfo->offset_y;
	// ctu_map_cu_data_local->sp_strInfo->n_recent_flag		= axis_ctu_map_cu_data->sp_strInfo->n_recent_flag;
	// ctu_map_cu_data_local->sp_strInfo->n_recent_idx			= axis_ctu_map_cu_data->sp_strInfo->n_recent_idx;
	// ctu_map_cu_data_local->evs_str_copy_info->is_matched	= axis_ctu_map_cu_data->evs_str_copy_info->is_matched;
	// ctu_map_cu_data_local->evs_str_copy_info->length		= axis_ctu_map_cu_data->evs_str_copy_info->length;
	// ctu_map_cu_data_local->evs_str_copy_info->match_type	= axis_ctu_map_cu_data->evs_str_copy_info->match_type;
	// ctu_map_cu_data_local->evs_str_copy_info->srb_index		= axis_ctu_map_cu_data->evs_str_copy_info->srb_index;
	// ctu_map_cu_data_local->evs_str_copy_info->pv_type		= axis_ctu_map_cu_data->evs_str_copy_info->pv_type;
	// ctu_map_cu_data_local->evs_str_copy_info->pvflag		= axis_ctu_map_cu_data->evs_str_copy_info->pvflag;
	// ctu_map_cu_data_local->evs_str_copy_info->srb_bits		= axis_ctu_map_cu_data->evs_str_copy_info->srb_bits;
	// ctu_map_cu_data_local->evs_str_copy_info->srb_dist		= axis_ctu_map_cu_data->evs_str_copy_info->srb_dist;
	// ctu_map_cu_data_local->evs_str_copy_info->esc_flag		= axis_ctu_map_cu_data->evs_str_copy_info->esc_flag;
	// ctu_map_cu_data_local->evs_str_copy_info->pos			= axis_ctu_map_cu_data->evs_str_copy_info->pos;
	// ctu_map_cu_data_local->unpred_pix_info->Y				= axis_ctu_map_cu_data->unpred_pix_info->Y;
	// ctu_map_cu_data_local->unpred_pix_info->U				= axis_ctu_map_cu_data->unpred_pix_info->U;
	// ctu_map_cu_data_local->unpred_pix_info->V				= axis_ctu_map_cu_data->unpred_pix_info->V;
	// // 2 AEC																																		
	// ctu_map_cu_data->sp_strInfo->is_matched					= ctu_map_cu_data_local->sp_strInfo->is_matched;
	// ctu_map_cu_data->sp_strInfo->length						= ctu_map_cu_data_local->sp_strInfo->length;
	// ctu_map_cu_data->sp_strInfo->offset_x					= ctu_map_cu_data_local->sp_strInfo->offset_x;
	// ctu_map_cu_data->sp_strInfo->offset_y					= ctu_map_cu_data_local->sp_strInfo->offset_y;
	// ctu_map_cu_data->sp_strInfo->n_recent_flag				= ctu_map_cu_data_local->sp_strInfo->n_recent_flag;
	// ctu_map_cu_data->sp_strInfo->n_recent_idx				= ctu_map_cu_data_local->sp_strInfo->n_recent_idx;
	// ctu_map_cu_data->evs_str_copy_info->is_matched			= ctu_map_cu_data_local->evs_str_copy_info->is_matched;
	// ctu_map_cu_data->evs_str_copy_info->length				= ctu_map_cu_data_local->evs_str_copy_info->length;
	// ctu_map_cu_data->evs_str_copy_info->match_type			= ctu_map_cu_data_local->evs_str_copy_info->match_type;
	// ctu_map_cu_data->evs_str_copy_info->srb_index			= ctu_map_cu_data_local->evs_str_copy_info->srb_index;
	// ctu_map_cu_data->evs_str_copy_info->pv_type				= ctu_map_cu_data_local->evs_str_copy_info->pv_type;
	// ctu_map_cu_data->evs_str_copy_info->pvflag				= ctu_map_cu_data_local->evs_str_copy_info->pvflag;
	// ctu_map_cu_data->evs_str_copy_info->srb_bits			= ctu_map_cu_data_local->evs_str_copy_info->srb_bits;
	// ctu_map_cu_data->evs_str_copy_info->srb_dist			= ctu_map_cu_data_local->evs_str_copy_info->srb_dist;
	// ctu_map_cu_data->evs_str_copy_info->esc_flag			= ctu_map_cu_data_local->evs_str_copy_info->esc_flag;
	// ctu_map_cu_data->evs_str_copy_info->pos					= ctu_map_cu_data_local->evs_str_copy_info->pos;
	// ctu_map_cu_data->unpred_pix_info->Y						= ctu_map_cu_data_local->unpred_pix_info->Y;
	// ctu_map_cu_data->unpred_pix_info->U						= ctu_map_cu_data_local->unpred_pix_info->U;
	// ctu_map_cu_data->unpred_pix_info->V						= ctu_map_cu_data_local->unpred_pix_info->V;
	// // 2 DBK
	// ctu_map_cu_data_DBK->sp_strInfo->is_matched				= ctu_map_cu_data_local->sp_strInfo->is_matched;
	// ctu_map_cu_data_DBK->sp_strInfo->length					= ctu_map_cu_data_local->sp_strInfo->length;
	// ctu_map_cu_data_DBK->sp_strInfo->offset_x				= ctu_map_cu_data_local->sp_strInfo->offset_x;
	// ctu_map_cu_data_DBK->sp_strInfo->offset_y				= ctu_map_cu_data_local->sp_strInfo->offset_y;
	// ctu_map_cu_data_DBK->sp_strInfo->n_recent_flag			= ctu_map_cu_data_local->sp_strInfo->n_recent_flag;
	// ctu_map_cu_data_DBK->sp_strInfo->n_recent_idx			= ctu_map_cu_data_local->sp_strInfo->n_recent_idx;
	// ctu_map_cu_data_DBK->evs_str_copy_info->is_matched		= ctu_map_cu_data_local->evs_str_copy_info->is_matched;
	// ctu_map_cu_data_DBK->evs_str_copy_info->length			= ctu_map_cu_data_local->evs_str_copy_info->length;
	// ctu_map_cu_data_DBK->evs_str_copy_info->match_type		= ctu_map_cu_data_local->evs_str_copy_info->match_type;
	// ctu_map_cu_data_DBK->evs_str_copy_info->srb_index		= ctu_map_cu_data_local->evs_str_copy_info->srb_index;
	// ctu_map_cu_data_DBK->evs_str_copy_info->pv_type			= ctu_map_cu_data_local->evs_str_copy_info->pv_type;
	// ctu_map_cu_data_DBK->evs_str_copy_info->pvflag			= ctu_map_cu_data_local->evs_str_copy_info->pvflag;
	// ctu_map_cu_data_DBK->evs_str_copy_info->srb_bits		= ctu_map_cu_data_local->evs_str_copy_info->srb_bits;
	// ctu_map_cu_data_DBK->evs_str_copy_info->srb_dist		= ctu_map_cu_data_local->evs_str_copy_info->srb_dist;
	// ctu_map_cu_data_DBK->evs_str_copy_info->esc_flag		= ctu_map_cu_data_local->evs_str_copy_info->esc_flag;
	// ctu_map_cu_data_DBK->evs_str_copy_info->pos				= ctu_map_cu_data_local->evs_str_copy_info->pos;
	// ctu_map_cu_data_DBK->unpred_pix_info->Y					= ctu_map_cu_data_local->unpred_pix_info->Y;
	// ctu_map_cu_data_DBK->unpred_pix_info->U					= ctu_map_cu_data_local->unpred_pix_info->U;
	// ctu_map_cu_data_DBK->unpred_pix_info->V					= ctu_map_cu_data_local->unpred_pix_info->V;																																																		

	// Fetch_Ref_window_md2chroma->code_stru					= axis_Fetch_Ref_window_ptr->code_stru;																						

	// md_output_luma->sp_strInfo->is_matched					= axis_md_output_luma->sp_strInfo->is_matched;
	// md_output_luma->sp_strInfo->length						= axis_md_output_luma->sp_strInfo->length;
	// md_output_luma->sp_strInfo->offset_x					= axis_md_output_luma->sp_strInfo->offset_x;
	// md_output_luma->sp_strInfo->offset_y					= axis_md_output_luma->sp_strInfo->offset_y;
	// md_output_luma->sp_strInfo->n_recent_flag				= axis_md_output_luma->sp_strInfo->n_recent_flag;
	// md_output_luma->sp_strInfo->n_recent_idx				= axis_md_output_luma->sp_strInfo->n_recent_idx;
	// md_output_luma->evs_str_copy_info->is_matched			= axis_md_output_luma->evs_str_copy_info->is_matched;
	// md_output_luma->evs_str_copy_info->length				= axis_md_output_luma->evs_str_copy_info->length;
	// md_output_luma->evs_str_copy_info->match_type			= axis_md_output_luma->evs_str_copy_info->match_type;
	// md_output_luma->evs_str_copy_info->srb_index			= axis_md_output_luma->evs_str_copy_info->srb_index;
	// md_output_luma->evs_str_copy_info->pv_type				= axis_md_output_luma->evs_str_copy_info->pv_type;
	// md_output_luma->evs_str_copy_info->pvflag				= axis_md_output_luma->evs_str_copy_info->pvflag;
	// md_output_luma->evs_str_copy_info->srb_bits				= axis_md_output_luma->evs_str_copy_info->srb_bits;
	// md_output_luma->evs_str_copy_info->srb_dist				= axis_md_output_luma->evs_str_copy_info->srb_dist;
	// md_output_luma->evs_str_copy_info->esc_flag				= axis_md_output_luma->evs_str_copy_info->esc_flag;
	// md_output_luma->evs_str_copy_info->pos					= axis_md_output_luma->evs_str_copy_info->pos;
	// md_output_luma->unpred_pix_info->Y						= axis_md_output_luma->unpred_pix_info->Y;
	// md_output_luma->unpred_pix_info->U						= axis_md_output_luma->unpred_pix_info->U;
	// md_output_luma->unpred_pix_info->V						= axis_md_output_luma->unpred_pix_info->V;																								
#endif


	STATIC_C strFetch_ref_window regi_Fetch_Ref_window_ptr;
#pragma HLS array_partition variable=Fetch_Ref_window_md2chroma->cur_refi complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_md2chroma->lidx complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_md2chroma->refp_is_library_picture complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_md2chroma->refp_is_library_picture complete dim=2
#pragma HLS array_partition variable=Fetch_Ref_window_md2chroma->refp_pic_ptr complete dim=1
#pragma HLS array_partition variable=Fetch_Ref_window_md2chroma->refp_pic_ptr complete dim=2
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_md2chroma->RefWindowLCU_UP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_md2chroma->RefWindowLCU_VP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1 cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1 cyclic factor=128 dim=1

#if ENABLE_BFRAME

	for (int i = 0; i < (SWH * SWW) >> 2; i++) {
#pragma HLS PIPELINE
		regi_Fetch_Ref_window_ptr.RefWindowLCU_UP     [i] = Fetch_Ref_window_md2chroma->RefWindowLCU_UP[i];
		regi_Fetch_Ref_window_ptr.RefWindowLCU_VP     [i] = Fetch_Ref_window_md2chroma->RefWindowLCU_VP[i];
		regi_Fetch_Ref_window_ptr.RefWindowLCU_UP_ref1[i] = Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1[i];
		regi_Fetch_Ref_window_ptr.RefWindowLCU_VP_ref1[i] = Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1[i];
	}

	#if CHROMA_AXIS_ADAPTER
	regi_Fetch_Ref_window_ptr.code_stru							= axis_Fetch_Ref_window_ptr->code_stru;
	#endif
	for(int i = 0; i < REFP_NUM; i++)
	{
#pragma HLS PIPELINE
		regi_Fetch_Ref_window_ptr.lidx[i]							= Fetch_Ref_window_md2chroma->lidx[i];
		regi_Fetch_Ref_window_ptr.cur_refi[i]						= Fetch_Ref_window_md2chroma->cur_refi[i];
	}
	for(int i = 0; i < MAX_NUM_REF_PICS; i++)
	{
#pragma HLS PIPELINE
		for(int j = 0; j < REFP_NUM; j++)
		{
			regi_Fetch_Ref_window_ptr.refp_pic_ptr[i][j]				= Fetch_Ref_window_md2chroma->refp_pic_ptr[i][j];
			regi_Fetch_Ref_window_ptr.refp_is_library_picture[i][j]	= Fetch_Ref_window_md2chroma->refp_is_library_picture[i][j];
		}
	}

#endif


	static ENC_CU_DATA md_best_ctu_data[2];

	static MD_RESULT md_best_result[2];
	md_best_result[0].cost_best = MAX_S64;
	md_best_result[1].cost_best = MAX_S64;

	static IP_ARRAY IP_buffer;
	static IP_MAP_SCU MAP_SCU_IP;
	static MD_FW md_fw;
	static MD_INPUT md_input;
	IP_ARRAY* IP_buffer_ptr = &IP_buffer;
	IP_MAP_SCU* MAP_SCU_IP_ptr = &MAP_SCU_IP;

	static ENC_ME_LINE_MAP me_line_map_inner;
	ENC_ME_LINE_MAP* me_line_map_ptr_tmp = &me_line_map_inner;
	MD_FW* md_fw_ptr = &md_fw;
	MD_INPUT* md_input_ptr = &md_input;

	U13 x0;
	U13 y0;
	U3 cu_width_log2;
	U3 cu_height_log2;
	U8 cu_width;
	U8 cu_height;
	s64 cost_temp = MAX_S64;
	static ENC_CU_DATA_ARRAY curr_cu_data_temp_array;
	U3 part_num = 0;
	COM_SPLIT_STRUCT split_struct;
	U2 cons_pred_mode_child;

	U1 do_split = 1;
	ENC_ME_NEB_INFO left_ctu_mi_luma[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	ENC_ME_NEB_INFO cur_ctu_mi_luma[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	ENC_ME_NEB_INFO new_above_nebs_line_luma[ABOVE_LINE_LEN];
	for (int j = 0; j < 16; j++) {
#pragma HLS PIPELINE
		for (int i = 0; i < 16; i++)
		{
			//memcpy(&left_ctu_mi_luma[j][i], &left_ctu_mi[j][i], sizeof(ENC_ME_NEB_INFO));
			//memcpy(&cur_ctu_mi_luma[j][i], &cur_ctu_mi[j][i], sizeof(ENC_ME_NEB_INFO));
			// left_ctu_mi_luma[j][i] = left_ctu_mi[j][i];
			// cur_ctu_mi_luma[j][i] = cur_ctu_mi[j][i];
			left_ctu_mi_luma[j][i] = cur_ctu_mi[1 - me_line_map_ptr_luma->curr_ctu_idx][j][i];
			cur_ctu_mi_luma[j][i] = cur_ctu_mi[me_line_map_ptr_luma->curr_ctu_idx][j][i];
		}
	}
	for (int i = 0; i < ABOVE_LINE_LEN; i++) {
		//memcpy(&new_above_nebs_line_luma[i], &new_above_nebs_line[i], sizeof(ENC_ME_NEB_INFO));
		new_above_nebs_line_luma[i] = new_above_nebs_line[i];
	}
	for (int k = 0; k < 5; k++) {
		for (int j = 0; j < 7; j++) {
			for (int i = 0; i < 256; i++)
			{
#pragma HLS UNROLL factor=16
#pragma HLS PIPELINE
				md_best_ctu_data[0].split_mode[k][j][i] = NO_SPLIT;
				md_best_ctu_data[1].split_mode[k][j][i] = NO_SPLIT;
			}
		}
	}

	md_run_top_prepare_chroma(fme_out_para_set_md2chroma, &md_fw, &md_input, IP_buffer_ptr, md_input_ptr, me_line_map_ptr_tmp,
		me_line_map_ptr_luma, me_line_map_ptr_luma->map_scu_inner[1 - me_line_map_ptr_luma->curr_ctu_idx],
		me_line_map_ptr_luma->map_cu_mode_inner[1 - me_line_map_ptr_luma->curr_ctu_idx],
		cur_ctu_mi[1 - me_line_map_ptr_luma->curr_ctu_idx]/*left_ctu_mi*/, MAP_SCU_IP_ptr, x0, y0, cu_width_log2, cu_height_log2, cu_width, cu_height, &do_split, curr_cu_data_temp_array);

	IP_MAP_SCU_SIZE<64, 64>	IP_map_scu_64x64;
	static u32					map_scu_line_hor[960];
	static u32					map_scu_line_ver[16];
	static u32					map_scu_bottom_right;
	for (int i = 0; i < COM_MIN(32, (md_fw_ptr->pic_width - x0) >> 2); i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=32
		IP_map_scu_64x64.map_scu_up_line[i] = y0 == 0 ? 0 : map_scu_line_hor[(x0 >> 2) + i];
	}
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		IP_map_scu_64x64.map_scu_left_line_new[i] = x0 == 0 ? 0 : map_scu_line_ver[i];
	}
	IP_map_scu_64x64.map_scu_top_left_new = y0 == 0 ? 0 : map_scu_bottom_right;

	static IP_ARRAY_SIZE<64, 64> IP_buffer_64x64 = {};
	static pel rec_line_hor[3840];
	static pel rec_line_ver[64];
	static pel bottom_right;
	for (int i = 0; i < COM_MIN(128, md_fw_ptr->pic_width - x0); ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=128
		IP_buffer_64x64.up_line[i] = rec_line_hor[x0 + i];
	}
	for (int i = 0; i < 64; ++i)
	{
		IP_buffer_64x64.left_line[i] = rec_line_ver[i];
	}
	IP_buffer_64x64.top_left = bottom_right;

	static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	md1_cu_data_best_c;

	///////////////////////////////////////////////////////////////////////////
	/// <chroma line buffer>
	static IP_ARRAY_SIZE<32, 32> IP_buffer_U_32x32 = {};
	static IP_ARRAY_SIZE<32, 32> IP_buffer_V_32x32 = {};
	static pel rec_line_hor_U[1920];
	static pel rec_line_hor_V[1920];
	static pel rec_line_ver_U[32];
	static pel rec_line_ver_V[32];
	static pel bottom_right_U;
	static pel bottom_right_V;
	for (int i = 0; i < COM_MIN(64, (md_fw_ptr->pic_width - x0) >> 1); i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=64
		IP_buffer_U_32x32.up_line[i] = rec_line_hor_U[(x0 >> 1) + i];
		IP_buffer_V_32x32.up_line[i] = rec_line_hor_V[(x0 >> 1) + i];
	}
	for (int i = 0; i < 32; i++)
	{
		IP_buffer_U_32x32.left_line[i] = rec_line_ver_U[i];
		IP_buffer_V_32x32.left_line[i] = rec_line_ver_V[i];
	}
	IP_buffer_U_32x32.top_left = bottom_right_U;
	IP_buffer_V_32x32.top_left = bottom_right_V;
	/// </chroma line buffer>

	//memcpy(md2_output, md_output_luma, sizeof(ENC_CU_DATA));

	md_input_ptr->do_chroma = 1;
	U3 split_mode = 0;
	for (int k = 0; k < 5; k++)
	{
		for (int j = 0; j < 7; j++)
			for (int i = 0; i < 256; i++)
			{
#pragma HLS UNROLL factor=16
#pragma HLS PIPELINE
				md_input_ptr->split_mode[k][j][i] = md_output_luma->split_mode[k][j][i];
			}
	}

	for (int k = 0; k < CU_SIZE; k++)
	{
		md_input_ptr->pred_mode[k] = md_output_luma->pred_mode[k];
#if CUDQP_QP_MAP
		md_input_ptr->cu_dqp_enable = fme_out_para_set_md2chroma->cu_dqp_enable;
		md_input_ptr->cu_luma_qp[k] = md_output_luma->cu_luma_qp[k];
		md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;
		md_input_ptr->md_slice_qp = fme_out_para_set_md2chroma->md_slice_qp;
#endif
		md_input_ptr->ipm[0][k] = md_output_luma->ipm[0][k];
		md_input_ptr->ipm[1][k] = md_output_luma->ipm[1][k];
		md_input_ptr->affine_flag[k] = md_output_luma->affine_flag[k];
		md_input_ptr->refi[k][0] = md_output_luma->refi[k][0];
		md_input_ptr->refi[k][1] = md_output_luma->refi[k][1];
		md_input_ptr->map_scu[k] = md_output_luma->map_scu[k];
		md_input_ptr->map_cu_mode[k] = md_output_luma->map_cu_mode[k];
#if USE_ENH
		md_input_ptr->umve_flag[k] = md_output_luma->umve_flag[k];
		md_input_ptr->skip_idx[k] = md_output_luma->skip_idx[k];
#if INTERPF
		md_input_ptr->inter_filter_flag[k] = md_output_luma->inter_filter_flag[k];
#endif
#endif
	}

	for (int k = 0; k < MAX_CU_CNT_IN_LCU; k++)
	{
#pragma HLS PIPELINE II=16
		for (int j = 0; j < REFP_NUM; j++)
			for (int i = 0; i < MV_D; i++)
			{

				md_input_ptr->mv[k][j][i] = md_output_luma->mv[k][j][i];
			}
	}

	for (int j = 0; j < N_C; j++)
		for (int i = 0; i < CU_SIZE; i++)
		{
#pragma HLS UNROLL factor=16
#pragma HLS PIPELINE
			md_input_ptr->num_nz_coef[j][i] = md_output_luma->num_nz_coef[j][i];
		}

#if USE_TSCPM
	for (int i = 0; i < 64; i++) {
#pragma HLS PIPELINE
		for (int j = 0; j < 64; j++) {
			md_input_ptr->reco_y[i][j] = md_output_luma->reco_y[i][j];
		}
	}
#endif

	//memcpy(me_line_map_ptr_input_luma, me_line_map_ptr_input, sizeof(ENC_ME_LINE_MAP));
	//*me_line_map_ptr_input_luma = *me_line_map_ptr_input;
	com_get_split_mode(&split_mode, 0, 0, 64, 64, 64, md_output_luma->split_mode);
	if (split_mode != SPLIT_QUAD)
		do_split = 0;

	if (do_split) {
		cons_pred_mode_child = NO_MODE_CONS;
		prepare_cu_info_COM_combine_only_qt_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, 0, 0,
			0, 0, cu_width, cu_height, SPLIT_QUAD, &split_struct);


		for (int i = 0; i < (cu_width >> 2); i++)
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int j = 0; j < (cu_height >> 2); j++)
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS UNROLL factor=16
#pragma HLS PIPELINE
				IP_map_scu_64x64.cur_map_scu[i][j] = 0;

		for (part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS LOOP_TRIPCOUNT max=4
			cost_temp += circuit_32x32_top_chroma(md_fw_ptr, md_input_ptr,
				Fetch_Ref_window_md2chroma->RefWindowLCU_UP, Fetch_Ref_window_md2chroma->RefWindowLCU_VP, Fetch_Ref_window_md2chroma->CtrPos,
#if ENABLE_BFRAME
				Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1, Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1, Fetch_Ref_window_md2chroma->CtrPos_ref1,
#endif
				fme_out_para_set_md2chroma->p_fenc_LCU_U,
				fme_out_para_set_md2chroma->p_fenc_LCU_V,
				&md1_cu_data_best_c,
				1, 0, cons_pred_mode_child, TREE_LC, part_num, split_struct, cu_width_log2,
				&IP_buffer_64x64,
				&IP_buffer_U_32x32, &IP_buffer_V_32x32,
				&IP_map_scu_64x64,
				/*Fetch_Ref_window_md2chroma*/&regi_Fetch_Ref_window_ptr, SPLIT_QUAD);
		}
		cu_data_cpy_c<32>(&curr_cu_data_temp_array, &md1_cu_data_best_c, cu_width, cu_height, 0);

		cost_compare_fixed_point_top_new_chroma(cost_temp, &curr_cu_data_temp_array, cu_width_log2, cu_height_log2, 0, TREE_LC, SPLIT_QUAD,
			&md_best_result[0], cons_pred_mode_child, &md_best_ctu_data[0]);
	}
	else
	{
#if !ONLY_QT_QT
		circuit_64x64_64x32_32x64_branch_top_chroma(split_mode, md_fw_ptr, md_input_ptr,
			Fetch_Ref_window_md2chroma->RefWindowLCU_UP, Fetch_Ref_window_md2chroma->RefWindowLCU_VP, Fetch_Ref_window_md2chroma->CtrPos,
#if ENABLE_BFRAME
			Fetch_Ref_window_md2chroma->RefWindowLCU_UP_ref1, Fetch_Ref_window_md2chroma->RefWindowLCU_VP_ref1, Fetch_Ref_window_md2chroma->CtrPos_ref1,
#endif
			fme_out_para_set_md2chroma->p_fenc_LCU_U,
			fme_out_para_set_md2chroma->p_fenc_LCU_V,
			&md_best_result[1],
			&IP_buffer_64x64,
			&IP_buffer_U_32x32, &IP_buffer_V_32x32,
			&IP_map_scu_64x64,
			&md_best_ctu_data[1],
			/*Fetch_Ref_window_md2chroma*/&regi_Fetch_Ref_window_ptr, md_output_luma);
		#endif
	}

	store_enc_info_top_chroma(&md_fw, &md_input, me_line_map_ptr_tmp, 0, 0, NO_MODE_CONS, 64,
		64, TREE_LC, md_best_result, md_best_ctu_data, IP_buffer_ptr, MAP_SCU_IP_ptr, ctu_map_cu_data, ctu_map_cu_data_DBK, dbk_u, dbk_v,
		me_line_map_ptr_tmp, cur_ctu_mi[me_line_map_ptr_luma->curr_ctu_idx], me_line_map_ptr_luma, me_line_map_ptr_luma->map_scu_inner[me_line_map_ptr_luma->curr_ctu_idx],
		/*me_line_map_ptr_luma->map_cu_mode_inner[me_line_map_ptr_luma->curr_ctu_idx]*/cur_ctu_cu_mode, fme_out_para_set_md2chroma->p_fenc_LCU_Y, fme_out_para_set_md2chroma->p_fenc_LCU_U,
		fme_out_para_set_md2chroma->p_fenc_LCU_V,
		me_line_map_ptr_luma, md_output_luma, me_line_map_ptr_luma->map_cu_mode_inner[me_line_map_ptr_luma->curr_ctu_idx]);

#if 0
	for (int i = 0; i < MAX_CU_DEPTH; ++i) {
		for (int j = 0; j < NUM_BLOCK_SHAPE; ++j) {
			for (int k = 0; k < MAX_CU_CNT_IN_LCU; ++k) {
				//ctu_map_cu_data_DBK->split_mode[i][j][k] = md_output_luma->split_mode[i][j][k];
			}
		}
	}
	for(int i = 0; i < CU_SIZE; i++) {
		for(int j = 0; j < N_C; j++) {
			//ctu_map_cu_data_DBK->num_nz_coef[j][i] = ctu_map_cu_data->num_nz_coef[j][i];
		}
	#if INTERPF
		//ctu_map_cu_data_DBK->inter_filter_flag[i] = md_output_luma->inter_filter_flag[i];
	#endif
	#if AWP
		//ctu_map_cu_data_DBK->awp_flag[i] = md_output_luma->awp_flag[i];
	#endif
	}

	for (int j = 0; j < (CU_SIZE >> 2); j++) {
		for (int i = 0; i < (CU_SIZE >> 2); i++) {
		//ctu_map_cu_data_DBK->coef_y[j][i] = ctu_map_cu_data->coef_y[j][i];
		}
	}
    for (int j = 0; j < ((CU_SIZE << 4) >> 2); j++) {
        //ctu_map_cu_data_DBK->coef_u[j] = ctu_map_cu_data->coef_u[j];		
    }
    for (int j = 0; j < ((CU_SIZE << 4) >> 2); j++) {
        //ctu_map_cu_data_DBK->coef_v[j] = ctu_map_cu_data->coef_v[j];
    }
#endif

	// int i, j;

	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++) {
#pragma HLS PIPELINE
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; j++) {
			me_line_map_ptr->map_scu_inner[me_line_map_ptr_tmp->curr_ctu_idx][i][j] = me_line_map_ptr_tmp->map_scu_inner[me_line_map_ptr_tmp->curr_ctu_idx][i][j];
			me_line_map_ptr->map_scu_inner[1 - me_line_map_ptr_tmp->curr_ctu_idx][i][j] = me_line_map_ptr_tmp->map_scu_inner[1 - me_line_map_ptr_tmp->curr_ctu_idx][i][j];

		}
	}

	copy_enc_me_line_map_DBK(me_line_map_ptr_luma, me_line_map_ptr_DBK);
	copy_enc_me_line_map_AEC(me_line_map_ptr_luma, me_line_map_ptr_AEC);
	//copy_enc_me_line_map(me_line_map_ptr_luma, me_line_map_ptr);
#if CHROMA_AXIS_ADAPTER
	axis_me_line_map_ptr_DBK->min_scu_x							= me_line_map_ptr_luma->min_scu_x;
	axis_me_line_map_ptr_DBK->min_scu_y							= me_line_map_ptr_luma->min_scu_y;
	axis_me_line_map_ptr_DBK->stride_in_map						= me_line_map_ptr_luma->stride_in_map;
	axis_me_line_map_ptr_DBK->stride_in_line					= me_line_map_ptr_luma->stride_in_line;
	axis_me_line_map_ptr_DBK->pic_width_in_scu					= me_line_map_ptr_luma->pic_width_in_scu;
	axis_me_line_map_ptr_DBK->pic_height_in_scu					= me_line_map_ptr_luma->pic_height_in_scu;
	axis_me_line_map_ptr_DBK->above_line_idx					= me_line_map_ptr_luma->above_line_idx;
	axis_me_line_map_ptr_DBK->curr_ctu_idx						= me_line_map_ptr_luma->curr_ctu_idx;
	axis_me_line_map_ptr_DBK->x_ctu								= me_line_map_ptr_luma->x_ctu;
	axis_me_line_map_ptr_DBK->y_ctu								= me_line_map_ptr_luma->y_ctu;

	axis_me_line_map_ptr_AEC->min_scu_x							= me_line_map_ptr_luma->min_scu_x;
	axis_me_line_map_ptr_AEC->min_scu_y							= me_line_map_ptr_luma->min_scu_y;
	axis_me_line_map_ptr_AEC->stride_in_map						= me_line_map_ptr_luma->stride_in_map;
	axis_me_line_map_ptr_AEC->stride_in_line					= me_line_map_ptr_luma->stride_in_line;
	axis_me_line_map_ptr_AEC->pic_width_in_scu					= me_line_map_ptr_luma->pic_width_in_scu;
	axis_me_line_map_ptr_AEC->pic_height_in_scu					= me_line_map_ptr_luma->pic_height_in_scu;
	axis_me_line_map_ptr_AEC->above_line_idx					= me_line_map_ptr_luma->above_line_idx;
	axis_me_line_map_ptr_AEC->curr_ctu_idx						= me_line_map_ptr_luma->curr_ctu_idx;
	axis_me_line_map_ptr_AEC->x_ctu								= me_line_map_ptr_luma->x_ctu;
	axis_me_line_map_ptr_AEC->y_ctu								= me_line_map_ptr_luma->y_ctu;

	// axis_me_line_map_ptr->min_scu_x								= me_line_map_ptr_luma->min_scu_x;
	// axis_me_line_map_ptr->min_scu_y								= me_line_map_ptr_luma->min_scu_y;
	// axis_me_line_map_ptr->stride_in_map							= me_line_map_ptr_luma->stride_in_map;
	// axis_me_line_map_ptr->stride_in_line						= me_line_map_ptr_luma->stride_in_line;
	// axis_me_line_map_ptr->pic_width_in_scu						= me_line_map_ptr_luma->pic_width_in_scu;
	// axis_me_line_map_ptr->pic_height_in_scu						= me_line_map_ptr_luma->pic_height_in_scu;
	// axis_me_line_map_ptr->above_line_idx						= me_line_map_ptr_luma->above_line_idx;
	// axis_me_line_map_ptr->curr_ctu_idx							= me_line_map_ptr_luma->curr_ctu_idx;
	// axis_me_line_map_ptr->x_ctu									= me_line_map_ptr_luma->x_ctu;
	// axis_me_line_map_ptr->y_ctu									= me_line_map_ptr_luma->y_ctu;	
#endif

#if !SIMPLE_MD
	for(int i=0;i<64;++i)
	{
#pragma HLS PIPELINE
		for(int j=0;j<64;++j)
		{
			md2sao->y_org_sao[i][j]=ctu_map_cu_data->y_org_sao[i][j];
		}
	}
	for(int i=0;i<32;++i)
	{
#pragma HLS PIPELINE
		for(int j=0;j<32;++j)
		{
			md2sao->u_org_sao[i][j]=ctu_map_cu_data->u_org_sao[i][j];
			md2sao->v_org_sao[i][j]=ctu_map_cu_data->v_org_sao[i][j];
		}
	}
#endif

	for (int j = 0; j < 16; j++) {
#pragma HLS PIPELINE
		for (int i = 0; i < 16; i++) {
			inner_nebs_md_chroma[1 - me_line_map_ptr_luma->curr_ctu_idx][j][i] = cur_ctu_mi[1 - me_line_map_ptr_luma->curr_ctu_idx][j][i]; //left_ctu_mi[j][i];
			inner_nebs_md_chroma[me_line_map_ptr_luma->curr_ctu_idx][j][i] = cur_ctu_mi[me_line_map_ptr_luma->curr_ctu_idx][j][i]; //cur_ctu_mi[j][i];
		}
	}


	for (int j = 0; j < 16; j++) {
#pragma HLS PIPELINE
		for (int i = 0; i < 16; i++) {
			//memcpy(&left_ctu_mi[j][i], &left_ctu_mi_luma[j][i], sizeof(ENC_ME_NEB_INFO));
			//memcpy(&cur_ctu_mi[j][i], &cur_ctu_mi_luma[j][i], sizeof(ENC_ME_NEB_INFO));
			//left_ctu_mi[j][i] = left_ctu_mi_luma[j][i];
			//cur_ctu_mi[j][i] = cur_ctu_mi_luma[j][i];
			cur_ctu_mi[1 - me_line_map_ptr_luma->curr_ctu_idx][j][i] = left_ctu_mi_luma[j][i];
			cur_ctu_mi[me_line_map_ptr_luma->curr_ctu_idx][j][i] = cur_ctu_mi_luma[j][i];
		}
	}

	for (int i = 0; i < 48; i++) {
		//memcpy(&new_above_nebs_line[i], &new_above_nebs_line_luma[i], sizeof(ENC_ME_NEB_INFO));
		//new_above_nebs_line[i] = new_above_nebs_line_luma[i];
		new_above_nebs_line_DBK[i] = new_above_nebs_line_luma[i];
	}

	bottom_right = rec_line_hor[x0 + 63];
	for (int i = 0; i < COM_MIN(64, md_fw_ptr->pic_width - x0); ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=64
		rec_line_hor[x0 + i] = md_output_luma->reco_y[63][i];
	}
	for (int i = 0; i < 64; ++i)
	{
		rec_line_ver[i] = md_output_luma->reco_y[i][63];
	}

	bottom_right_U = rec_line_hor_U[(x0 >> 1) + 31];
	bottom_right_V = rec_line_hor_V[(x0 >> 1) + 31];
	for (int i = 0; i < COM_MIN(32, (md_fw_ptr->pic_width - x0) >> 1); ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=32
		rec_line_hor_U[(x0 >> 1) + i] = ctu_map_cu_data_DBK->reco_u[992 + i];
		rec_line_hor_V[(x0 >> 1) + i] = ctu_map_cu_data_DBK->reco_v[992 + i];
	}
	for (int i = 0; i < 32; ++i)
	{
		rec_line_ver_U[i] = ctu_map_cu_data_DBK->reco_u[32 * i + 31];
		rec_line_ver_V[i] = ctu_map_cu_data_DBK->reco_v[32 * i + 31];
	}

	map_scu_bottom_right = map_scu_line_hor[(x0 >> 2) + 15];
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_width - x0) >> 2); i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		map_scu_line_hor[(x0 >> 2) + i] = ctu_map_cu_data_DBK->map_scu[240 + i];
	}
	for (int i = 0; i < COM_MIN(16, (md_fw_ptr->pic_height - y0) >> 2); i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		map_scu_line_ver[i] = ctu_map_cu_data_DBK->map_scu[16 * i + 15];
	}
}