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

#define IP_BUFFER_NULL 0

#include "iime.h"
extern int md_slice_qp;
static signed  char com_tbl_log2[257] = {
	/* 0, 1 */
	-1, 0,
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

#if Debug_TWO_PASS
extern FILE* md_two_pass;
#endif // Debug_TWO_PASS

void modify_md_enc_split_allow(U4 cud, U14 cup, U8 cu_width, U8 cu_height, MD_INPUT* md_input_ptr, U1 split_allow[6], U3* best_split_mode_luma)
{
	U3 split_mode = 0;
	com_get_split_mode(&split_mode, cud, cup, cu_width, cu_height, 64, md_input_ptr->split_mode);

	for (int i = 0; i < 6; i++)
	{
		if (i == split_mode)
		{
			if (split_allow[i] == 0)
				printf("2222\n");
#if !ONLY_QT_QT
			assert(split_allow[i] != 0);
#endif
			split_allow[i] = 1;
		}
		else
		{
			split_allow[i] = 0;
		}
	}
	*best_split_mode_luma = split_mode;
}

static void store_cu_data_pixel_inner_chroma(ENC_CU_DATA_ARRAY *curr_cu_data_best_ptr, IP_ARRAY *IP_buffer_ptr, U13 x, U13 y, U7 w, U7 h, U13 pic_width,
	U13 pic_height, U2 tree_status)
{
	int i;
	int j;
	U7 pic_x = x & 63;
	U7 pic_y = y & 63;
	U6 pic_x_chroma = pic_x >> 1;
	U6 pic_y_chroma = pic_y >> 1;
	U7 w_temp = x + w < pic_width ? w : (U7)(pic_width - x);
	U7 h_temp = y + h < pic_height ? h : (U7)(pic_height - y);
	U7 stride = w;
	s8 log2_w;
	s8 log2_h;
	ENC_CU_DATA_ARRAY *cu_data;
	pel *src;
	log2_w = (com_tbl_log2[w]);
	log2_h = (com_tbl_log2[h]);

	cu_data = curr_cu_data_best_ptr;
	//src = cu_data->reco_y;

	if ((tree_status) != (TREE_L)) {
		src = cu_data->reco_u;
		for (j = 0; j < h_temp >> 1; j++) {
			for (i = 0; i < w_temp >> 1; i++) {
				IP_buffer_ptr->temp_buffer_chroma[0][pic_y_chroma + j][pic_x_chroma + i] = (src[(j << (log2_w - 1)) + i]);
			}
		}
		src = cu_data->reco_v;
		for (j = 0; j < h_temp >> 1; j++) {
			for (i = 0; i < w_temp >> 1; i++) {
				IP_buffer_ptr->temp_buffer_chroma[1][pic_y_chroma + j][pic_x_chroma + i] = (src[(j << (log2_w - 1)) + i]);
			}
		}
	}
}

int copy_cu_data_array_dst_chroma(ENC_CU_DATA_ARRAY *dst, ENC_CU_DATA_ARRAY *src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
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
	U14 cx;
	U14 cy;
	U22 idx_dst;
	U22 idx_src;
	cx = (x >> 2);
	cy = (y >> 2);
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);


	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);

		for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->ipm[1] + idx_dst)[i] = (src->ipm[1] + idx_src)[i];
		}

		for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->map_scu + idx_dst)[i] = (src->map_scu + idx_src)[i];

		}

		for (k = 1; k < 3; k++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->num_nz_coef[k] + idx_dst)[i1] = (src->num_nz_coef[k] + idx_src)[i1];
			}
		}
	}

	for (j = 0; j < cu_height >> 1; j++) {
#pragma HLS LOOP_TRIPCOUNT max=32
		idx_dst = ((U22)((y >> 1) + j) << (log2_cus - 1)) + (x >> 1);
		idx_src = (U22)j << (cu_width_log2 - 1);

		for (i = 0; i < (cu_width >> 1); i++) {
#pragma HLS LOOP_TRIPCOUNT max=32
			(dst->coef_u + idx_dst)[i] = (src->coef_u + idx_src)[i];
			(dst->coef_v + idx_dst)[i] = (src->coef_v + idx_src)[i];
			(dst->reco_u + idx_dst)[i] = (src->reco_u + idx_src)[i];
			(dst->reco_v + idx_dst)[i] = (src->reco_v + idx_src)[i];
		}

	}
	return 0;
}

void update_map_scu_chroma(MD_FW *md_fw_ptr, ENC_ME_LINE_MAP *me_line_map_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, U13 x, U13 y, U8 src_cuw, U8 src_cuh,
	U2 tree_status)
{
	//s8 *map_ipm = 0;
	S8 *src_map_ipm = 0;
	//int size_depth;
	U6 w;
	U6 h;
	u16 i;

	s8 log2_src_cuw;
	s8 log2_src_cuh;
	U11 scu_x;
	U11 scu_y;
	U5 mpm_x;
	U5 mpm_y;
	u8 ii = 0;
	U11 x_scu_line;
	U11 y_scu_line;
	u8 j = 0;
	scu_x = x >> 2;
	scu_y = y >> 2;
	log2_src_cuw = (com_tbl_log2[src_cuw]);
	log2_src_cuh = (com_tbl_log2[src_cuh]);

	src_map_ipm = curr_cu_data_best_ptr->ipm[0];

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
#pragma HLS LOOP_TRIPCOUNT max=16
		src_map_ipm += src_cuw >> 2;
	}
	x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	y_scu_line = scu_y - me_line_map_ptr->min_scu_y;
	j = 0;
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
				= curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
			me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i] = curr_cu_data_best_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
		}
	}
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i] =
				curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
		}
	}
}

void store_enc_info_best_chroma(MD_FW*md_fw_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3 *best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY *prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw, ENC_ME_LINE_MAP *me_line_map_ptr, 
	IP_MAP_SCU *MAP_SCU_IP_ptr)
{
	//store_cu_data_pixel_inner_chroma(curr_cu_data_best, IP_buffer_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height,
	//	tree_status);

	copy_cu_data_array_dst_chroma(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
		cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status);

	update_map_scu_chroma(md_fw_ptr, me_line_map_ptr, curr_cu_data_best, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, tree_status);
}
int mode_cu_init_chroma_4_8_chroma(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,
	MD_COM_MODE_BEST* bst_info_ptr, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status, U2* cons_pred_mode, 
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr, 
	MD_INPUT* md_input_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	MD_COM_MODE_BEST* bst_info = bst_info_ptr;

	bst_info->x_pos = x;
	bst_info->y_pos = y;
	bst_info->cu_width = (1 << cu_width_log2);
	bst_info->cu_height = (1 << cu_height_log2);
	bst_info->cu_width_log2 = cu_width_log2;
	bst_info->cu_height_log2 = cu_height_log2;
	bst_info->x_scu = x >> 2;
	bst_info->y_scu = y >> 2;
	bst_info->scup = bst_info->y_scu * pic_width_in_scu + bst_info->x_scu;///////////////////////////////
	bst_info->cud = cud;
	bst_info->chroma_motion = /*0*/cu_data_temp_ptr->last_child && tree_status == TREE_L;

	U5 cuw_scu = 1 << (cu_width_log2 - 2);
	U5 cuh_scu = 1 << (cu_height_log2 - 2);
	cu_nz_cln(bst_info->num_nz);
	int i;
	for (i = 0; i < (1 << 7 >> 1) * (1 << 7 >> 1); i++) {
		bst_info->coef_y[i] = 0;
	}
	for (i = 0; i < ((1 << 7 >> 1) * (1 << 7 >> 1)) >> 2; i++) {
		bst_info->coef_u[i] = 0;
		bst_info->coef_v[i] = 0;
	}
	bst_info->cu_mode = 0;
	//init the best cu info
	bst_info->pb_part = 0;
	bst_info->tb_part = 0;
	//init_pb_part(bst_info);
	//init_tb_part(bst_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->pb_part, bst_info_pb_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->tb_part, bst_info_tb_info);

	int j;

	for (j = 0; j < 2; j++) {
		bst_info->refi[j] = 0;
	}

	bst_info->ipf_flag = 0;

	bst_info->affine_flag = 0;
	int k;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 2; k++) {
				bst_info->affine_mv[i][j][k] = 0;
				bst_info->affine_mv[i][j][k] = 0;
			}
		}
	}

	bst_info->smvd_flag = 0;
	enc_rdoq_bit_est(rdoq_model, model_cnt, rdoq_array);
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U3 luma_pred_mode;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	int luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (tree_status == TREE_C) {

		// bottom-right
		U13 luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;
		luma_pred_mode = md_input_ptr->pred_mode[luma_cup];

		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
#if SUB_TMVP
		core_sbtmvp->sbTmvp_flag = 0;
		if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE
			&& md_input_ptr->skip_idx[luma_cup] == 0 && md_input_ptr->pred_mode[luma_cup] >= MODE_SKIP && md_input_ptr->umve_flag[luma_cup] == 0) {
			for (i = 0; i < SBTMVP_NUM; i++) {
				//int blk = ((i >= (cu_width >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? 1 : 0) + ((j >= (cu_height >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? SBTMVP_NUM_1D : 0);
				int sbtmvp_pos;// = cu_w_scu * (cu_h_scu / SBTMVP_NUM_1D - 1) * (i / 2) + (cu_w_scu / SBTMVP_NUM_1D - 1) * (i % 2);
				sbtmvp_pos = (y_scu_in_LCU + ((cu_h_scu / SBTMVP_NUM_1D) * ((i / 2) + 1) - 1)) * scu_stride + (x_scu_in_LCU + ((cu_w_scu / SBTMVP_NUM_1D) * ((i % 2) + 1) - 1));
				core_sbtmvp->sbTmvp[i].ref_idx[0] = md_input_ptr->refi[sbtmvp_pos][0];
				core_sbtmvp->sbTmvp[i].ref_idx[1] = md_input_ptr->refi[sbtmvp_pos][1];

				core_sbtmvp->sbTmvp[i].mv[0][0] = md_input_ptr->mv[sbtmvp_pos][0][0];
				core_sbtmvp->sbTmvp[i].mv[0][1] = md_input_ptr->mv[sbtmvp_pos][0][1];
				core_sbtmvp->sbTmvp[i].mv[1][0] = md_input_ptr->mv[sbtmvp_pos][1][0];
				core_sbtmvp->sbTmvp[i].mv[1][1] = md_input_ptr->mv[sbtmvp_pos][1][1];
			}
			core_sbtmvp->sbTmvp_flag = 1;
		}
#endif
		bst_info->cu_mode = luma_pred_mode;
		bst_info->ipm[0][0] = md_input_ptr->ipm[0][luma_cup];
		bst_info->ipm[0][1] = 0;
		bst_info->affine_flag = md_input_ptr->affine_flag[luma_cup];
#if USE_ENH
		bst_info->umve_flag = md_input_ptr->umve_flag[luma_cup];
		bst_info->skip_idx = md_input_ptr->skip_idx[luma_cup];
#if INTERPF
		bst_info->inter_filter_flag = md_input_ptr->inter_filter_flag[luma_cup];
#endif
#endif
		for (i = 0; i < 2; i++) {
			bst_info->refi[i] = md_input_ptr->refi[luma_cup][i];
			bst_info->mv[i][0] = md_input_ptr->mv[luma_cup][i][0];
			bst_info->mv[i][1] = md_input_ptr->mv[luma_cup][i][1];
		}
	}

	bst_info->x_chroma = cu_data_temp_ptr->x_chroma;
	bst_info->y_chroma = cu_data_temp_ptr->y_chroma;
	int coef_nz_y = md_input_ptr->num_nz_coef[0][luma_cup];
	return coef_nz_y;
}

template<size_t KERNEL_SIZE_C>
int mode_cu_init_chroma_4_8(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,
	MD_COM_MODE_BEST* bst_info_ptr, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status, U2* cons_pred_mode,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<KERNEL_SIZE_C * 2>* cu_data_temp_c_ptr,
	MD_INPUT* md_input_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	MD_COM_MODE_BEST* bst_info = bst_info_ptr;

	bst_info->x_pos = x;
	bst_info->y_pos = y;
	bst_info->cu_width = (1 << cu_width_log2);
	bst_info->cu_height = (1 << cu_height_log2);
	bst_info->cu_width_log2 = cu_width_log2;
	bst_info->cu_height_log2 = cu_height_log2;
	bst_info->x_scu = x >> 2;
	bst_info->y_scu = y >> 2;
	bst_info->scup = bst_info->y_scu * pic_width_in_scu + bst_info->x_scu;///////////////////////////////
	bst_info->cud = cud;
	bst_info->chroma_motion = /*0*/cu_data_temp_c_ptr->last_child && tree_status == TREE_L;

	U5 cuw_scu = 1 << (cu_width_log2 - 2);
	U5 cuh_scu = 1 << (cu_height_log2 - 2);
	cu_nz_cln(bst_info->num_nz);
	int i;
#if !SIMPLE_MD
	for (i = 0; i < (1 << 7 >> 1) * (1 << 7 >> 1); i++) {
#pragma HLS UNROLL factor=64
#pragma HLS PIPELINE
		bst_info->coef_y[i] = 0;
	}
	for (i = 0; i < ((1 << 7 >> 1) * (1 << 7 >> 1)) >> 2; i++) {
#pragma HLS UNROLL factor=32
#pragma HLS PIPELINE
		bst_info->coef_u[i] = 0;
		bst_info->coef_v[i] = 0;
	}
#endif //!SIMPLE_MD
	bst_info->cu_mode = 0;
	//init the best cu info
	bst_info->pb_part = 0;
	bst_info->tb_part = 0;
	//init_pb_part(bst_info);
	//init_tb_part(bst_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->pb_part, bst_info_pb_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->tb_part, bst_info_tb_info);

	int j;

	for (j = 0; j < 2; j++) {
		bst_info->refi[j] = 0;
	}

	bst_info->ipf_flag = 0;

	bst_info->affine_flag = 0;
	int k;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 2; k++) {
#pragma HLS PIPELINE
				bst_info->affine_mv[i][j][k] = 0;
				bst_info->affine_mv[i][j][k] = 0;
			}
		}
	}

	bst_info->smvd_flag = 0;
#if !SIMPLE_MD
	enc_rdoq_bit_est(rdoq_model, model_cnt, rdoq_array);
#endif
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U3 luma_pred_mode;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	int luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (tree_status == TREE_C) {

		// bottom-right
		U13 luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;
		luma_pred_mode = md_input_ptr->pred_mode[luma_cup];

		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
#if SUB_TMVP && !SIMPLE_MD
		core_sbtmvp->sbTmvp_flag = 0;
		if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE
			&& md_input_ptr->skip_idx[luma_cup] == 0 && md_input_ptr->pred_mode[luma_cup] >= MODE_SKIP && md_input_ptr->umve_flag[luma_cup] == 0) {
			for (i = 0; i < SBTMVP_NUM; i++) {
				//int blk = ((i >= (cu_width >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? 1 : 0) + ((j >= (cu_height >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? SBTMVP_NUM_1D : 0);
				int sbtmvp_pos;// = cu_w_scu * (cu_h_scu / SBTMVP_NUM_1D - 1) * (i / 2) + (cu_w_scu / SBTMVP_NUM_1D - 1) * (i % 2);
				sbtmvp_pos = (y_scu_in_LCU + ((cu_h_scu / SBTMVP_NUM_1D) * ((i / 2) + 1) - 1)) * scu_stride + (x_scu_in_LCU + ((cu_w_scu / SBTMVP_NUM_1D) * ((i % 2) + 1) - 1));
				core_sbtmvp->sbTmvp[i].ref_idx[0] = md_input_ptr->refi[sbtmvp_pos][0];
				core_sbtmvp->sbTmvp[i].ref_idx[1] = md_input_ptr->refi[sbtmvp_pos][1];

				core_sbtmvp->sbTmvp[i].mv[0][0] = md_input_ptr->mv[sbtmvp_pos][0][0];
				core_sbtmvp->sbTmvp[i].mv[0][1] = md_input_ptr->mv[sbtmvp_pos][0][1];
				core_sbtmvp->sbTmvp[i].mv[1][0] = md_input_ptr->mv[sbtmvp_pos][1][0];
				core_sbtmvp->sbTmvp[i].mv[1][1] = md_input_ptr->mv[sbtmvp_pos][1][1];
			}
			core_sbtmvp->sbTmvp_flag = 1;
		}
#endif
		bst_info->cu_mode = luma_pred_mode;
		bst_info->ipm[0][0] = md_input_ptr->ipm[0][luma_cup];
		bst_info->ipm[0][1] = 0;
		bst_info->affine_flag = md_input_ptr->affine_flag[luma_cup];
#if USE_ENH
		bst_info->umve_flag = md_input_ptr->umve_flag[luma_cup];
		bst_info->skip_idx = md_input_ptr->skip_idx[luma_cup];
#if INTERPF
		bst_info->inter_filter_flag = md_input_ptr->inter_filter_flag[luma_cup];
#endif
#endif
		for (i = 0; i < 2; i++) {
			bst_info->refi[i] = md_input_ptr->refi[luma_cup][i];
			bst_info->mv[i][0] = md_input_ptr->mv[luma_cup][i][0];
			bst_info->mv[i][1] = md_input_ptr->mv[luma_cup][i][1];
		}
	}

	bst_info->x_chroma = cu_data_temp_c_ptr->x_chroma;
	bst_info->y_chroma = cu_data_temp_c_ptr->y_chroma;
	int coef_nz_y = md_input_ptr->num_nz_coef[0][luma_cup];
	return coef_nz_y;
}
template<size_t KERNEL_SIZE_C>
int mode_cu_init_chroma(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,
	MD_COM_MODE_BEST* bst_info_ptr, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status, U2* cons_pred_mode, ENC_CU_DATA_ARRAY_CHROMA_SIZE<KERNEL_SIZE_C * 2>* cu_data_temp_ptr, MD_INPUT* md_input_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	MD_COM_MODE_BEST* bst_info = bst_info_ptr;

	bst_info->x_pos = x;
	bst_info->y_pos = y;
	bst_info->cu_width = (1 << cu_width_log2);
	bst_info->cu_height = (1 << cu_height_log2);
	bst_info->cu_width_log2 = cu_width_log2;
	bst_info->cu_height_log2 = cu_height_log2;
	bst_info->x_scu = x >> 2;
	bst_info->y_scu = y >> 2;
	bst_info->scup = bst_info->y_scu * pic_width_in_scu + bst_info->x_scu;///////////////////////////////
	bst_info->cud = cud;
	bst_info->chroma_motion = /*0*/cu_data_temp_ptr->last_child && tree_status == TREE_L;

	U5 cuw_scu = 1 << (cu_width_log2 - 2);
	U5 cuh_scu = 1 << (cu_height_log2 - 2);
	cu_nz_cln(bst_info->num_nz);
	int i;

	for (i = 0; i < ((1 << 7 >> 1) * (1 << 7 >> 1)) >> 2; i++) {
		bst_info->coef_u[i] = 0;
		bst_info->coef_v[i] = 0;
	}
	bst_info->cu_mode = 0;
	//init the best cu info
	bst_info->pb_part = 0;
	bst_info->tb_part = 0;
	//init_pb_part(bst_info);
	//init_tb_part(bst_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->pb_part, bst_info_pb_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->tb_part, bst_info_tb_info);

	int j;

	for (j = 0; j < 2; j++) {
		bst_info->refi[j] = 0;
	}

	bst_info->ipf_flag = 0;

	bst_info->affine_flag = 0;
	int k;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 2; k++) {
				bst_info->affine_mv[i][j][k] = 0;
				bst_info->affine_mv[i][j][k] = 0;
			}
		}
	}

	bst_info->smvd_flag = 0;
	enc_rdoq_bit_est(rdoq_model, model_cnt, rdoq_array);
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U3 luma_pred_mode;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	int luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (tree_status == TREE_C) {
		// bottom-right
		U13 luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;

		luma_pred_mode = md_input_ptr->pred_mode[luma_cup];

		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
#if SUB_TMVP
		core_sbtmvp->sbTmvp_flag = 0;
		if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE
			&& md_input_ptr->skip_idx[luma_cup] == 0 && md_input_ptr->pred_mode[luma_cup] >= MODE_SKIP && md_input_ptr->umve_flag[luma_cup] == 0) {
			for (i = 0; i < SBTMVP_NUM; i++) {
				//int blk = ((i >= (cu_width >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? 1 : 0) + ((j >= (cu_height >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? SBTMVP_NUM_1D : 0);
				int sbtmvp_pos;// = cu_w_scu * (cu_h_scu / SBTMVP_NUM_1D - 1) * (i / 2) + (cu_w_scu / SBTMVP_NUM_1D - 1) * (i % 2);
				sbtmvp_pos = (y_scu_in_LCU + ((cu_h_scu / SBTMVP_NUM_1D) * ((i / 2) + 1) - 1)) * scu_stride + (x_scu_in_LCU + ((cu_w_scu / SBTMVP_NUM_1D) * ((i % 2) + 1) - 1));
				core_sbtmvp->sbTmvp[i].ref_idx[0] = md_input_ptr->refi[sbtmvp_pos][0];
				core_sbtmvp->sbTmvp[i].ref_idx[1] = md_input_ptr->refi[sbtmvp_pos][1];

				core_sbtmvp->sbTmvp[i].mv[0][0] = md_input_ptr->mv[sbtmvp_pos][0][0];
				core_sbtmvp->sbTmvp[i].mv[0][1] = md_input_ptr->mv[sbtmvp_pos][0][1];
				core_sbtmvp->sbTmvp[i].mv[1][0] = md_input_ptr->mv[sbtmvp_pos][1][0];
				core_sbtmvp->sbTmvp[i].mv[1][1] = md_input_ptr->mv[sbtmvp_pos][1][1];
				core_sbtmvp->sbTmvp_flag = 1;
			}
		}
#endif
		bst_info->cu_mode = luma_pred_mode;
		bst_info->ipm[0][0] = md_input_ptr->ipm[0][luma_cup];
		bst_info->ipm[0][1] = 0;
		bst_info->affine_flag = md_input_ptr->affine_flag[luma_cup];

		for (i = 0; i < 2; i++) {
			bst_info->refi[i] = md_input_ptr->refi[luma_cup][i];
			bst_info->mv[i][0] = md_input_ptr->mv[luma_cup][i][0];
			bst_info->mv[i][1] = md_input_ptr->mv[luma_cup][i][1];
		}
	}

	bst_info->x_chroma = cu_data_temp_ptr->x_chroma;
	bst_info->y_chroma = cu_data_temp_ptr->y_chroma;

	int coef_nz_y = md_input_ptr->num_nz_coef[0][luma_cup];
	return coef_nz_y;
}

int mode_cu_init_chroma(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,
	MD_COM_MODE_BEST* bst_info_ptr, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status, U2* cons_pred_mode, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_INPUT* md_input_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	MD_COM_MODE_BEST* bst_info = bst_info_ptr;

	bst_info->x_pos = x;
	bst_info->y_pos = y;
	bst_info->cu_width = (1 << cu_width_log2);
	bst_info->cu_height = (1 << cu_height_log2);
	bst_info->cu_width_log2 = cu_width_log2;
	bst_info->cu_height_log2 = cu_height_log2;
	bst_info->x_scu = x >> 2;
	bst_info->y_scu = y >> 2;
	bst_info->scup = bst_info->y_scu * pic_width_in_scu + bst_info->x_scu;///////////////////////////////
	bst_info->cud = cud;
	bst_info->chroma_motion = /*0*/cu_data_temp_ptr->last_child && tree_status == TREE_L;

	U5 cuw_scu = 1 << (cu_width_log2 - 2);
	U5 cuh_scu = 1 << (cu_height_log2 - 2);
	cu_nz_cln(bst_info->num_nz);
	int i;

	for (i = 0; i < ((1 << 7 >> 1) * (1 << 7 >> 1)) >> 2; i++) {
		bst_info->coef_u[i] = 0;
		bst_info->coef_v[i] = 0;
	}
	bst_info->cu_mode = 0;
	//init the best cu info
	bst_info->pb_part = 0;
	bst_info->tb_part = 0;
	//init_pb_part(bst_info);
	//init_tb_part(bst_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->pb_part, bst_info_pb_info);
	get_part_info(pic_width_in_scu, bst_info->x_pos, bst_info->y_pos, bst_info->cu_width,
		bst_info->cu_height, bst_info->tb_part, bst_info_tb_info);

	int j;

	for (j = 0; j < 2; j++) {
		bst_info->refi[j] = 0;
	}

	bst_info->ipf_flag = 0;

	bst_info->affine_flag = 0;
	int k;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			for (k = 0; k < 2; k++) {
				bst_info->affine_mv[i][j][k] = 0;
				bst_info->affine_mv[i][j][k] = 0;
			}
		}
	}

	bst_info->smvd_flag = 0;
	enc_rdoq_bit_est(rdoq_model, model_cnt, rdoq_array);
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	U3 luma_pred_mode;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	int luma_cup = (y_scu_in_LCU + (cu_h_scu - 1))* scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (tree_status == TREE_C) {
		// bottom-right
		U13 luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;

		luma_pred_mode = md_input_ptr->pred_mode[luma_cup];

		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
#if SUB_TMVP
		core_sbtmvp->sbTmvp_flag = 0;
		if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE
			&& md_input_ptr->skip_idx[luma_cup] == 0 && md_input_ptr->pred_mode[luma_cup] >= MODE_SKIP && md_input_ptr->umve_flag[luma_cup]==0) {
			for (i = 0; i < SBTMVP_NUM; i++) {
				//int blk = ((i >= (cu_width >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? 1 : 0) + ((j >= (cu_height >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? SBTMVP_NUM_1D : 0);
				int sbtmvp_pos;// = cu_w_scu * (cu_h_scu / SBTMVP_NUM_1D - 1) * (i / 2) + (cu_w_scu / SBTMVP_NUM_1D - 1) * (i % 2);
				sbtmvp_pos = (y_scu_in_LCU + ((cu_h_scu / SBTMVP_NUM_1D) * ((i / 2) + 1) - 1)) * scu_stride + (x_scu_in_LCU + ((cu_w_scu / SBTMVP_NUM_1D) * ((i % 2) + 1) - 1));
				core_sbtmvp->sbTmvp[i].ref_idx[0] = md_input_ptr->refi[sbtmvp_pos][0];
				core_sbtmvp->sbTmvp[i].ref_idx[1] = md_input_ptr->refi[sbtmvp_pos][1];

				core_sbtmvp->sbTmvp[i].mv[0][0] = md_input_ptr->mv[sbtmvp_pos][0][0];
				core_sbtmvp->sbTmvp[i].mv[0][1] = md_input_ptr->mv[sbtmvp_pos][0][1];
				core_sbtmvp->sbTmvp[i].mv[1][0] = md_input_ptr->mv[sbtmvp_pos][1][0];
				core_sbtmvp->sbTmvp[i].mv[1][1] = md_input_ptr->mv[sbtmvp_pos][1][1];
				core_sbtmvp->sbTmvp_flag = 1;
			}
		}
#endif
		bst_info->cu_mode = luma_pred_mode;
		bst_info->ipm[0][0] = md_input_ptr->ipm[0][luma_cup];
		bst_info->ipm[0][1] = 0;
		bst_info->affine_flag = md_input_ptr->affine_flag[luma_cup];

		for (i = 0; i < 2; i++) {
			bst_info->refi[i] = md_input_ptr->refi[luma_cup][i];
			bst_info->mv[i][0] = md_input_ptr->mv[luma_cup][i][0];
			bst_info->mv[i][1] = md_input_ptr->mv[luma_cup][i][1];
		}
	}

	bst_info->x_chroma = cu_data_temp_ptr->x_chroma;
	bst_info->y_chroma = cu_data_temp_ptr->y_chroma;

	int coef_nz_y = md_input_ptr->num_nz_coef[0][luma_cup];
	return coef_nz_y;
}

static void store_cu_data_pixel_inner_8_chroma(ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr, IP_ARRAY* IP_buffer_ptr, U13 x, U13 y, U7 w, U7 h, U13 pic_width,
	U13 pic_height, U2 tree_status, U3 part_num)
{
	int i;
	int j;
	U7 pic_x = x & 63;
	U7 pic_y = y & 63;
	U6 pic_x_chroma = pic_x >> 1;
	U6 pic_y_chroma = pic_y >> 1;
	U7 w_temp = x + w < pic_width ? w : (U7)(pic_width - x);
	U7 h_temp = y + h < pic_height ? h : (U7)(pic_height - y);
	U7 stride = w;
	s8 log2_w;
	s8 log2_h;
	ENC_CU_DATA_ARRAY* cu_data;
	pel* src;
	log2_w = (com_tbl_log2[w]);
	log2_h = (com_tbl_log2[h]);

	cu_data = curr_cu_data_best_ptr;
	//src = cu_data->reco_y;
	/* luma */

	if ((tree_status) != (TREE_L)) {
		src = cu_data->reco_u;
		for (j = 0; j < h_temp >> 1; j++) {
			for (i = 0; i < w_temp >> 1; i++) {
				IP_buffer_ptr->temp_buffer_chroma[0][pic_y_chroma + j][pic_x_chroma + i] = (src[(j << (log2_w - 1)) + i]);
			}
		}
		src = cu_data->reco_v;
		for (j = 0; j < h_temp >> 1; j++) {
			for (i = 0; i < w_temp >> 1; i++) {
				IP_buffer_ptr->temp_buffer_chroma[1][pic_y_chroma + j][pic_x_chroma + i] = (src[(j << (log2_w - 1)) + i]);
			}
		}
	}
}

int copy_cu_data_array_src_chroma(ENC_CU_DATA_ARRAY *dst, ENC_CU_DATA_ARRAY *src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
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
	U13 cx;
	U13 cy;
	U22 idx_dst;
	U22 idx_src;
	cx = (x >> 2);
	cy = (y >> 2);
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);

	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);

		for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];

		}

		for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
		}


		for (k = 1; k < 3; k++) {
			for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->num_nz_coef[k] + idx_dst)[i] = (src->num_nz_coef[k] + idx_src)[i];
			}
		}
	}


	for (j = 0; j < cu_height >> 1; j++) {
#pragma HLS LOOP_TRIPCOUNT max=32
		idx_dst = ((U22)((y >> 1) + j) << (log2_cus - 1)) + (x >> 1);
		idx_src = (U22)j << (cu_width_log2 - 1);

		for (i = 0; i < (cu_width >> 1); i++) {
#pragma HLS LOOP_TRIPCOUNT max=32
			(dst->coef_u + idx_dst)[i] = (src->coef_u + idx_src)[i];
			(dst->coef_v + idx_dst)[i] = (src->coef_v + idx_src)[i];
			(dst->reco_u + idx_dst)[i] = (src->reco_u + idx_src)[i];
			(dst->reco_v + idx_dst)[i] = (src->reco_v + idx_src)[i];
		}

	}
	return 0;
}


void copy_to_cu_data_chroma8_temp_chroma(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1], int coef_nz_y, u32 luma_map_scu)
{
	int i;
	int j;
	int k;
	int idx;
	int size;
	U3 cu_width_log2;
	U3 cu_height_log2;
	//int num_coef_y = 0;
	U12 num_coef_u = 0;
	U12 num_coef_v = 0;
	//int num_nnz_sum = 0;
	U1 cu_cbf_flag = 0;
	//int pb_idx_u = 0;
	U2 tb_idx_u = 0;
	U2 tb_idx_y;
	U14 num_coef_y = 0;

	cu_width_log2 = mod_info_best->cu_width_log2;
	cu_height_log2 = mod_info_best->cu_height_log2;

	if ((tree_status) != (TREE_L)) {
		/* copy coef */
		size = ((((mod_info_best->cu_width * mod_info_best->cu_height)) * sizeof(s16) >> 2));
		//if (coef_rec_write_flag[0] == 0) {////for Intra_CU
		for (i = 0; i < ((mod_info_best->cu_width * mod_info_best->cu_height) >> 2); i++) {
			cu_data->coef_u[i] = mod_info_best->coef_u[i];
			cu_data->coef_v[i] = mod_info_best->coef_v[i];
			cu_data->reco_u[i] = mod_info_best->rec_u[i];
			cu_data->reco_v[i] = mod_info_best->rec_v[i];  /* copy reco */

		}

		//}

		for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height / 4; i++) {
			num_coef_u += ((cu_data->coef_u[i]) != 0 ? 1 : 0);
			num_coef_v += ((cu_data->coef_v[i]) != 0 ? 1 : 0);
		}
	}

	num_coef_y = coef_nz_y;

	if (tree_status == TREE_LC) {
		cu_cbf_flag = ((num_coef_y == 0 ? 0 : 1)) | ((num_coef_u == 0 ? 0 : 1)) | ((num_coef_v == 0 ? 0 : 1));
	}
	else if ((tree_status) == (TREE_L)) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	/* copy mode info */
	if ((tree_status) == (TREE_C)) {
		idx = 0;
		for (j = 0; j < mod_info_best->cu_height >> 2; j++) {
			for (i = 0; i < mod_info_best->cu_width >> 2; i++) {
				//intra chroma mode
				if (mod_info_best->cu_mode == 0) {
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
				// residual and partition
				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < mod_info_best->cu_height >> 2; j++) {
			for (i = 0; i < mod_info_best->cu_width >> 2; i++) {

				//			tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, mod_info_best->cu_width,mod_info_best->cu_height);
				tb_idx_y = get_part_idx(SIZE_2Nx2N, i << 2, j << 2, mod_info_best->cu_width,
					mod_info_best->cu_height);
				//pb_idx_u = 0;
				tb_idx_u = 0;

				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
				}

				if (cu_cbf_flag) {
					//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((1 << 24));
					cu_data->map_scu[idx + i] = luma_map_scu | ((1 << 24));
				}
				else {
					//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((~(1 << 24)));
					cu_data->map_scu[idx + i] = luma_map_scu & ((~(1 << 24)));
				}
				if (mod_info_best->cu_mode == 0)
				{
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}

	}
}

template<size_t KERNEL_SIZE_C>
void copy_to_cu_data_chroma(U6 qp_y, ENC_CU_DATA_ARRAY_CHROMA_SIZE<KERNEL_SIZE_C * 2>* cu_data,
	MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1], int coef_nz_y, u32 luma_map_scu
#if CUDQP_QP_MAP
	, U6 cu_qp_group_pred_qp, int cu_dqp_enable
#endif
)
{
	int i;
	int j;
	int k;
	int idx;
	int size;
	U3 cu_width_log2;
	U3 cu_height_log2;
	//int num_coef_y = 0;
	U12 num_coef_u = 0;
	U12 num_coef_v = 0;
	//int num_nnz_sum = 0;
	U1 cu_cbf_flag = 0;
	//int pb_idx_u = 0;
	U2 tb_idx_u = 0;
	U2 tb_idx_y;
	U14 num_coef_y = 0;

	cu_width_log2 = mod_info_best->cu_width_log2;
	cu_height_log2 = mod_info_best->cu_height_log2;
/*#pragma HLS ARRAY_PARTITION variable=cu_data->coef_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data->coef_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data->reco_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data->reco_v complete dim=2*/
#pragma HLS ARRAY_PARTITION variable=mod_info_best->coef_u cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best->coef_v cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best->reco_u cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best->reco_v cyclic factor=32 dim=1

	if ((tree_status) != (TREE_L)) {
		/* copy coef */
		size = ((((mod_info_best->cu_width * mod_info_best->cu_height)) * sizeof(s16) >> 2));
		for (i = 0; i < KERNEL_SIZE_C; i++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C
			for (int j = 0; j < KERNEL_SIZE_C; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C
				if (i < mod_info_best->cu_height >> 1 && j < mod_info_best->cu_width >> 1)
				{
					cu_data->coef_u[i][j] = mod_info_best->coef_u[i * (mod_info_best->cu_width >> 1) + j];
					cu_data->coef_v[i][j] = mod_info_best->coef_v[i * (mod_info_best->cu_width >> 1) + j];
					cu_data->reco_u[i][j] = mod_info_best->rec_u[i * (mod_info_best->cu_width >> 1) + j];
					cu_data->reco_v[i][j] = mod_info_best->rec_v[i * (mod_info_best->cu_width >> 1) + j];  /* copy reco */
				}
				else
				{
					break;
				}
			}
		}

		for (i = 0; i < KERNEL_SIZE_C; i++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C
			for (int j = 0; j < KERNEL_SIZE_C; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C
				if (i < mod_info_best->cu_height >> 1 && j < mod_info_best->cu_width >> 1)
				{
					num_coef_u += ((cu_data->coef_u[i][j]) != 0 ? 1 : 0);
					num_coef_v += ((cu_data->coef_v[i][j]) != 0 ? 1 : 0);
				}
				else
				{
					break;
				}
			}
		}
	}
	//if (tree_status != TREE_C)
	//{
	//	for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height; i++) {
	//		num_coef_y += ((cu_data->coef_y[i]) != 0 ? 1 : 0);
	//	}
	//}

	num_coef_y = coef_nz_y;

	if (tree_status == TREE_LC) {
		cu_cbf_flag = ((num_coef_y == 0 ? 0 : 1)) | ((num_coef_u == 0 ? 0 : 1)) | ((num_coef_v == 0 ? 0 : 1));
	}
	else if ((tree_status) == (TREE_L)) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	/* copy mode info */
	if ((tree_status) == (TREE_C)) {
		idx = 0;
		for (j = 0; j < KERNEL_SIZE_C/2; j++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C/2
			for (i = 0; i < KERNEL_SIZE_C/2; i++) {
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C/2
				if (i < mod_info_best->cu_height >> 2 && j < mod_info_best->cu_width >> 2)
				{
					//intra chroma mode
					if (mod_info_best->cu_mode == 0)
					{
						cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
					}
					// residual and partition
					for (k = 1; k < 3; k++)
					{
#pragma HLS UNROLL
						cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
					}
				}
				else
				{
					break;
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < KERNEL_SIZE_C/2; j++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C/2
			for (i = 0; i < KERNEL_SIZE_C/2; i++) {
#pragma HLS LOOP_TRIPCOUNT max=KERNEL_SIZE_C/2
				if (i < mod_info_best->cu_height >> 2 && j < mod_info_best->cu_width >> 2)
				{
					//			tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, mod_info_best->cu_width,mod_info_best->cu_height);
					tb_idx_y = get_part_idx(SIZE_2Nx2N, i << 2, j << 2, mod_info_best->cu_width,
						mod_info_best->cu_height);
					//pb_idx_u = 0;
					tb_idx_u = 0;

					for (k = 1; k < 3; k++)
					{
#pragma HLS UNROLL
						cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
					}

					if (cu_cbf_flag) {
						//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((1 << 24));
						cu_data->map_scu[idx + i] = luma_map_scu | ((1 << 24));
					}
					else {
						//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((~(1 << 24)));
						cu_data->map_scu[idx + i] = luma_map_scu & ((~(1 << 24)));
					}
#if CUDQP_QP_MAP
					if ((cu_dqp_enable && !cu_cbf_flag))
					{
						//change qp to pred_qp if no residual
						MCU_SET_QP(cu_data->map_scu[idx + i], cu_qp_group_pred_qp);
						//assert(ctx->cu_qp_group.pred_qp >= 0 && ctx->cu_qp_group.pred_qp <= MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
					}
					//fprintf(md_cudqp, "%d %d %d %d %d %d %u\n", mod_info_best->x_pos, mod_info_best->y_pos, qp_y, cu_qp_group_pred_qp, idx, i, cu_data->map_scu[idx + i]);
#endif
					if (mod_info_best->cu_mode == 0)
					{
						cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
					}
				}
				else
				{
					break;
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}
	}
}

s64 md_chroma_8(U8 w, U8 h, MD_FW* md_fw_ptr,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* cu_data_temp_ptr_c,
	U14 x, U14 y, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status, U4 cud,
	MD_INPUT* md_input_ptr,
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U1 need_dochroma, U12 cup, U2 ctx_cons_pred_mode,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_U_4x8,
	IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_V_4x8,
	IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_U_8x4,
	IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_V_8x4,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8, IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16
)
{
#pragma HLS INLINE off

	if (!need_dochroma)
	{
		return 0;
	}

	STATIC_C MD_COM_MODE_BEST mod_info_best;
	static COM_PART_INFO bst_info_pb_info;
	static COM_PART_INFO bst_info_tb_info;
 
#pragma HLS ARRAY_PARTITION variable=mod_info_best.coef_y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best.coef_u cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best.coef_v cyclic factor=32 dim=1

	U2 cons_pred_mode = ctx_cons_pred_mode;
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	//pel* orgU;
	static RDOQ_ARRAY rdoq_array;
	static RDOQ_MODEL rdoq_model_t;
	static RDOQ_MODEL model_cnt_t;
#if 1 //for clean_test pass
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	IP_buffer_ptr_16x8->top_left = IP_buffer_ptr_16x8->top_left + zero;
	IP_buffer_ptr_8x16->top_left = IP_buffer_ptr_8x16->top_left + zero;
	IP_buffer_ptr_U_4x8->top_left += zero;
	IP_buffer_ptr_U_8x4->top_left += zero;
	IP_buffer_ptr_V_4x8->top_left += zero;
	IP_buffer_ptr_V_8x4->top_left += zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_ptr_U_4x8->top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_U_8x4->top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_4x8->top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_8x4->top_left +=  (zero_1b << (16 - 1));	
	IP_map_scu_ptr_16x8->map_scu_top_left_new = IP_map_scu_ptr_16x8->map_scu_top_left_new + zero;
	IP_map_scu_ptr_8x16->map_scu_top_left_new = IP_map_scu_ptr_8x16->map_scu_top_left_new + zero;
	IP_buffer_ptr_16x16->top_left = IP_buffer_ptr_16x16->top_left + zero;
	IP_map_scu_ptr_16x16->map_scu_top_left_new = IP_map_scu_ptr_16x16->map_scu_top_left_new + zero;
#endif
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_run complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_last complete dim=0
#if !SIMPLE_MD
	for (int i = 0; i < 24; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 2; j++)
		{
#pragma HLS UNROLL
			rdoq_array.rdoq_est_run[i][j] = 0;
		}
	}
	for (int i = 0; i < 3; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 2; j++)
		{
#pragma HLS UNROLL
			rdoq_array.rdoq_est_cbf[i][j] = 0;
		}
	}
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_level[i][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 6; j++)
		{
#pragma HLS UNROLL
			for (int k = 0; k < 12; k++)
			{
#pragma HLS UNROLL
				for (int l = 0; l < 2; l++)
				{
#pragma HLS UNROLL
					rdoq_array.rdoq_est_last[i][j][k][l] = 0;
				}
			}
		}
	}
	int i;

#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.last1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.last2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.run_rdoq complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.last1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.last2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.run_rdoq complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.level complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.cbf complete dim=0

	for (i = 0; i < 12; i++) {
#pragma HLS UNROLL
		rdoq_model_t.last1[i] = PROB_INIT;
		model_cnt_t.last1[i] = 0;
	}
	for (i = 0; i < 22; i++) {
#pragma HLS UNROLL
		rdoq_model_t.last2[i] = PROB_INIT;
		model_cnt_t.last2[i] = 0;
	}
	for (i = 0; i < 24; i++) {
#pragma HLS UNROLL
		rdoq_model_t.run_rdoq[i] = PROB_INIT;
		model_cnt_t.run_rdoq[i] = 0;
		rdoq_model_t.level[i] = PROB_INIT;
		model_cnt_t.level[i] = 0;
	}
	for (i = 0; i < 3; i++) {
#pragma HLS UNROLL
		rdoq_model_t.cbf[i] = PROB_INIT;
		model_cnt_t.cbf[i] = 0;
	}
#endif
	U1 coef_rec_write_flag[1] = { 0 };
	int coef_nz_y;
#if SUB_TMVP
	static CORE_SBTMVP core_sbtmvp;
	CORE_SBTMVP* core_sbtmvp_ptr = &core_sbtmvp;
	core_sbtmvp.sbTmvp_flag = 0;
	core_sbtmvp.best_sbTmvp_flag = 0;

#pragma HLS ARRAY_PARTITION variable=core_sbtmvp.sbTmvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=core_sbtmvp.best_sbTmvp complete dim=0

	for (int k = 0; k < SBTMVP_NUM; k++)
	{
#pragma HLS UNROLL
		for (int i = 0; i < REFP_NUM; i++)
		{
#pragma HLS UNROLL
			core_sbtmvp.sbTmvp[k].ref_idx[i] = -1;
			core_sbtmvp.best_sbTmvp[k].ref_idx[i] = -1;
			for (int j = 0; j < MV_D; j++)
			{
#pragma HLS UNROLL
				core_sbtmvp.sbTmvp[k].mv[i][j] = 0;
				core_sbtmvp.best_sbTmvp[k].mv[i][j] = 0;
			}
		}
	}
#endif
	coef_nz_y = mode_cu_init_chroma_4_8<8>(&rdoq_model_t, &model_cnt_t, &rdoq_array, &mod_info_best, &bst_info_pb_info, &bst_info_tb_info, x, y, cu_width_log2, cu_height_log2, cud,
		md_fw_ptr->pic_width_in_scu, TREE_C, &cons_pred_mode, cu_data_temp_ptr_c, md_input_ptr
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
		);

	static pel orgU[2][8][8];
#pragma HLS ARRAY_PARTITION variable=orgU cyclic factor=8 dim=3
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE
			for (int j = 0; j < 8; j++) {
				if (k == 0) {
					orgU[k][i][j] = p_fenc_LCU_U[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
				else {
					orgU[k][i][j] = p_fenc_LCU_V[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
			}
		}
	}

#if USE_TSCPM
	pel piRecoY[16][16];// = md_input_ptr->reco_y + (mod_info_best.y_pos - md_input_ptr->pix_y) * 64 + (mod_info_best.x_pos - md_input_ptr->pix_x);
#pragma HLS ARRAY_PARTITION variable=piRecoY cyclic factor=16 dim=2
	for (int i = 0; i < 16; i++) {
#pragma HLS PIPELINE
		for (int j = 0; j < 16; j++) {
			piRecoY[i][j] = md_input_ptr->reco_y[(mod_info_best.y_pos - md_input_ptr->pix_y + i)][mod_info_best.x_pos - md_input_ptr->pix_x + j];
		}
	}
#endif
#if CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;

	const int com_tbl_qp_chroma_adjust[64] =
	{
		//this table aligned with spec
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
		46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
		50, 50, 50, 51
	};

		luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
		if (md_input_ptr->cu_dqp_enable) {
		int qp_offset_bit_depth = (8 * (md_fw_ptr->bit_depth_internal - 8));

		md_input_ptr->qp_u = md_input_ptr->cu_luma_qp[luma_cup];
		md_input_ptr->qp_v = md_input_ptr->cu_luma_qp[luma_cup];

		md_input_ptr->qp_u += md_input_ptr->chroma_quant_param_delta_cb - qp_offset_bit_depth;
		md_input_ptr->qp_v += md_input_ptr->chroma_quant_param_delta_cr - qp_offset_bit_depth;

		md_input_ptr->qp_u = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_u)];
		md_input_ptr->qp_v = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_v)];

		md_input_ptr->qp_u = COM_CLIP(md_input_ptr->qp_u + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
		md_input_ptr->qp_v = COM_CLIP(md_input_ptr->qp_v + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
	}
#endif
	rdo_chroma_8(&rdoq_array, md_fw_ptr, tree_status, &mod_info_best, &bst_info_pb_info, md_input_ptr, orgU,
		IP_buffer_ptr_U_8x8,
		IP_buffer_ptr_V_8x8,
		IP_buffer_ptr_U_4x8,
		IP_buffer_ptr_V_4x8,
		IP_buffer_ptr_U_8x4,
		IP_buffer_ptr_V_8x4,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, 
#endif
		coef_rec_write_flag, Fetch_Ref_window_ptr,
		piRecoY, 
		IP_buffer_ptr_8x16, IP_buffer_ptr_16x8, IP_buffer_ptr_16x16, 
		IP_map_scu_ptr_8x16, IP_map_scu_ptr_16x8, IP_map_scu_ptr_16x16
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
	);



	if (mod_info_best.cu_mode == 0)
	{
		mod_info_best.cu_mode = 0;
		mod_info_best.affine_flag = 0;
	}
	else {

		coef_rec_write_flag[0] = 1;
	}
#if !CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
#endif
	luma_map_scu = md_input_ptr->map_scu[luma_cup];

	copy_to_cu_data_chroma<8>(md_input_ptr->qp_y, cu_data_temp_ptr_c, &mod_info_best, tree_status, coef_rec_write_flag, coef_nz_y, luma_map_scu
#if CUDQP_QP_MAP
		, md_input_ptr->cu_qp_group_pred_qp, md_input_ptr->cu_dqp_enable
#endif
		);
#if CUDQP_QP_MAP
	int cu_cbf_flag = MCU_GET_CBF(cu_data_temp_ptr_c->map_scu[0]);
	int qpy = MCU_GET_QP(cu_data_temp_ptr_c->map_scu[0]);
	if (md_input_ptr->cu_dqp_enable && tree_status != TREE_C && cu_cbf_flag)
	{
		md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->cu_luma_qp[luma_cup];
	}
#endif
	return 0;

}
static s64 circuit_16x16_16x8_8x16_md1_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode)
{

#pragma HLS ARRAY_PARTITION variable=p_fenc_LCU_U complete dim=2
#pragma HLS ARRAY_PARTITION variable=p_fenc_LCU_V complete dim=2
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->reco_y cyclic factor=16 dim=2

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
	static s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	//static ENC_CU_DATA_ARRAY curr_cu_data_best;
	//static ENC_CU_DATA_ARRAY cu_data_temp[3];
	U2 cons_pred_mode_child[3] = { NO_MODE_CONS,NO_MODE_CONS,NO_MODE_CONS };
	U2 tree_status_child[3] = { TREE_LC, TREE_LC, TREE_LC };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
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

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma]) {

		static IP_ARRAY_SIZE<8, 16>					IP_buffer_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_16x8;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_16x16;
		static IP_ARRAY_SIZE<4, 8>					IP_buffer_U_4x8;
		static IP_ARRAY_SIZE<4, 8>					IP_buffer_V_4x8;
		static IP_ARRAY_SIZE<8, 4>					IP_buffer_U_8x4;
		static IP_ARRAY_SIZE<8, 4>					IP_buffer_V_8x4;
		static IP_MAP_SCU_SIZE<8, 16>				IP_map_scu_8x16;
		static IP_MAP_SCU_SIZE<16, 8>				IP_map_scu_16x8;
		static IP_MAP_SCU_SIZE<16, 16>				IP_map_scu_16x16;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_temp_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_4x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_4x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_4x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_4x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_4x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_4x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x4.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x4.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x4.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x4.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x4.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x4.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (split_mode)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 8 && cu_height == 16);
			IP_buffer_8x16.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_16x16, md_input_ptr, x_pos_lcu, y_pos_lcu);

			IP_buffer_U_4x8.fetch_bi_ver(part_num, IP_buffer_ptr_U_8x8);
			IP_buffer_V_4x8.fetch_bi_ver(part_num, IP_buffer_ptr_V_8x8);

			IP_map_scu_8x16.fetch_bi_ver(part_num, IP_map_scu_ptr_16x16);
			//MPM_buffer_ptr->get_mpm_up(&MPM_buffer_8x16.up, MPM_buffer_ptr, x0, y0);
			//MPM_buffer_ptr->get_mpm_left(&MPM_buffer_8x16.left, MPM_buffer_ptr, x0, y0);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 16 && cu_height == 8);
			IP_buffer_16x8.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_16x16, md_input_ptr, x_pos_lcu, y_pos_lcu);

			IP_buffer_U_8x4.fetch_bi_hor(part_num, IP_buffer_ptr_U_8x8);
			IP_buffer_V_8x4.fetch_bi_hor(part_num, IP_buffer_ptr_V_8x8);

			IP_map_scu_16x8.fetch_bi_hor(part_num, IP_map_scu_ptr_16x16);
			//MPM_buffer_ptr->get_mpm_up(&MPM_buffer_16x8.up, MPM_buffer_ptr, x0, y0);
			//MPM_buffer_ptr->get_mpm_left(&MPM_buffer_16x8.left, MPM_buffer_ptr, x0, y0);
			break;
		default: assert(0);
		}

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_8x16.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_8x16.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_8x16.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);

		if (tree_status == TREE_LC) {
			cu_data_temp_c.x_chroma = x0;
			cu_data_temp_c.y_chroma = y0;
			cu_data_temp_c.x_last = x0 + cu_width - 4;
			cu_data_temp_c.y_last = y0 + cu_height - 4;
			cu_data_temp_c.last_child = FALSE;
		}
		else {
			cu_data_temp_c.x_chroma = prev_cu_data_temp_ptr->x_chroma;
			cu_data_temp_c.y_chroma = prev_cu_data_temp_ptr->y_chroma;
			cu_data_temp_c.x_last = prev_cu_data_temp_ptr->x_last;
			cu_data_temp_c.y_last = prev_cu_data_temp_ptr->y_last;
			cu_data_temp_c.last_child = x0 + cu_width == prev_cu_data_temp_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_temp_ptr->y_last + 4;
		}
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_ptr_16x16->map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 4);
			int flag2 = (x0 > 0 && ((IP_map_scu_16x8.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 3);
			int flag3 = (x0 > 0 && ((IP_map_scu_8x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 3) && (cu_height_log2 == 4);

			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_ptr_16x16->map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x8.map_scu_left_line_new[0]) >> 16) & 0x7F;

			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_8x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (cu_width == 8)
		{
#if 1 //for clean_test pass		
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	IP_buffer_U_4x8.top_left += zero;
	IP_buffer_U_8x4.top_left += zero;
	IP_buffer_V_4x8.top_left += zero;
	IP_buffer_V_8x4.top_left += zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_U_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_U_8x4.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_V_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_V_8x4.top_left +=  (zero_1b << (16 - 1));		
#endif		
			md_chroma_8(4, 8, md_fw_ptr,
				&cu_data_temp_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V,RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
#if IP_BUFFER_NULL
				NULL, NULL,
#else
				IP_buffer_ptr_U_8x8, IP_buffer_ptr_V_8x8, //NULL, NULL,
#endif //IP_BUFFER_NULL
				&IP_buffer_U_4x8, &IP_buffer_V_4x8,
				&IP_buffer_U_8x4, &IP_buffer_V_8x4,
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16,
				&IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}
		else if (cu_height == 8)
		{
#if 1 //for clean_test pass		
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	IP_buffer_U_4x8.top_left += zero;
	IP_buffer_U_8x4.top_left += zero;
	IP_buffer_V_4x8.top_left += zero;
	IP_buffer_V_8x4.top_left += zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_U_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_U_8x4.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_V_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_V_8x4.top_left +=  (zero_1b << (16 - 1));	
#endif			
			md_chroma_8(8, 4, md_fw_ptr,
				&cu_data_temp_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
#if IP_BUFFER_NULL
				NULL, NULL,
#else
				IP_buffer_ptr_U_8x8, IP_buffer_ptr_V_8x8, //NULL, NULL, 
#endif //IP_BUFFER_NULL
				&IP_buffer_U_4x8, &IP_buffer_V_4x8,
				&IP_buffer_U_8x4, &IP_buffer_V_8x4,
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16,
				&IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}

		switch (split_mode)
		{
		case SPLIT_BI_HOR:
			IP_buffer_U_8x4.update_bi_hor(part_num, cu_data_temp_c.reco_u);
			IP_buffer_V_8x4.update_bi_hor(part_num, cu_data_temp_c.reco_v);

			IP_map_scu_16x8.update_bi_hor(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_temp_c.map_scu);
			break;
		case SPLIT_BI_VER:
			IP_buffer_U_4x8.update_bi_ver(part_num, cu_data_temp_c.reco_u);
			IP_buffer_V_4x8.update_bi_ver(part_num, cu_data_temp_c.reco_v);

			IP_map_scu_8x16.update_bi_ver(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_temp_c.map_scu);
			break;
		default: assert(0);
		}


#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.coef_u cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.coef_v cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.reco_u cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.reco_v cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.ipm cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.map_scu cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_c.num_nz_coef cyclic factor=4 dim=2
		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_temp_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &cu_data_inner, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}

	return 1;
}

static void clear_map_scu_chroma(ENC_ME_LINE_MAP* me_line_map_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, U13 x, U13 y, U7 cu_width, U7 cu_height, U13 pic_width,
	U13 pic_height)
{
	U5 w;
	U5 h;
	U5 i;
	U5 m;
	if (x + cu_width > pic_width) {
		cu_width = pic_width - x;
	}
	if (y + cu_height > pic_height) {
		cu_height = pic_height - y;
	}
	w = cu_width >> 2;
	h = cu_height >> 2;
	U11 x_scu_line = (x >> 2) - me_line_map_ptr->min_scu_x;
	U11 y_scu_line = (y >> 2) - me_line_map_ptr->min_scu_y;
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (m = 0; m < w; m++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + m][y_scu_line + i]
				= 0;
			me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + m][y_scu_line +
				i] = 0;
		}
	}
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (m = 0; m < w; m++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + m][y_scu_line + i] = 0;
		}
	}
}

s64 prepare_cu_info_16_chroma(MD_FW* md_fw_ptr, s64 lambdaY, U13 x0, U13 y0,
	U12 cup, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct)
{

	md_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], qt_depth, split_mode == NO_SPLIT ? bet_depth : (U3)(bet_depth + 1), &split_struct->num_split[i], split_mode);
	}

	if (split_mode == NO_SPLIT)
		return 0;

	U2 num_cons_pred_mode_loop[1];
	num_cons_pred_mode_loop[0] = (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) ? 2 : 1;
	s64 cost_temp = 0;
	U3 bit_cnt;
	//U3 part_num = 0;
	U3 rate_bit = 0;
	if (cons_pred_mode == NO_MODE_CONS && ((md_fw_ptr->slice_type)) != 1) {
		if (num_cons_pred_mode_loop[0] == 1) {
			cons_pred_mode_child[0] = NO_MODE_CONS;
		}
		else {
			cons_pred_mode_child[0] = (((cons_flag != 0 ? ONLY_INTER : ONLY_INTRA)));
		}
	}
	else {
		cons_pred_mode_child[0] = cons_pred_mode;
	}
	//clear_map_scu_chroma(me_line_map_ptr, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width,
	//	md_fw_ptr->pic_height);


	return cost_temp;
}

static void md_16x16_16x8_8x16_loop_md1_chroma(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (isBTH_allow) {

		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);

		prepare_cu_info_16_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode, cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_16x16_16x8_8x16_md1_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_16x16,
				IP_buffer_ptr_U_8x8,
				IP_buffer_ptr_V_8x8,
				IP_map_scu_ptr_16x16,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	//return bth_cost;
}

s64 circuit_16x16_top_md1_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* prev_cu_data_temp_ptr,
	U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_U_8x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_V_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_U_16x8, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_V_16x8,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode)
{
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
	U3 num_split_to_try = split_struct.num_split[part_num];

	U2 tree_status_child[1] = { TREE_LC };
	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_temp;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma]) {
		assert(cu_width == 16 && cu_height == 16);
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_16x8;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_16x16;
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_U_8x8;
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_V_8x8;
		static IP_MAP_SCU_SIZE<8, 16>				IP_map_scu_8x16;
		static IP_MAP_SCU_SIZE<16, 8>				IP_map_scu_16x8;
		static IP_MAP_SCU_SIZE<16, 16>				IP_map_scu_16x16;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_temp_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		if (tree_status == TREE_LC) {
			cu_data_temp_c.x_chroma = x0;
			cu_data_temp_c.y_chroma = y0;
			cu_data_temp_c.x_last = x0 + cu_width - 4;
			cu_data_temp_c.y_last = y0 + cu_height - 4;
			cu_data_temp_c.last_child = FALSE;
		}
		else {
			cu_data_temp_c.x_chroma = prev_cu_data_temp_ptr->x_chroma;
			cu_data_temp_c.y_chroma = prev_cu_data_temp_ptr->y_chroma;
			cu_data_temp_c.x_last = prev_cu_data_temp_ptr->x_last;
			cu_data_temp_c.y_last = prev_cu_data_temp_ptr->y_last;
			cu_data_temp_c.last_child = x0 + cu_width == prev_cu_data_temp_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_temp_ptr->y_last + 4;
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (split_mode)
		{
		case SPLIT_BI_HOR:
			IP_buffer_16x16.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_16x32, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x8.fetch_bi_hor(part_num, IP_buffer_ptr_U_8x16);
			IP_buffer_V_8x8.fetch_bi_hor(part_num, IP_buffer_ptr_V_8x16);

			IP_map_scu_16x16.fetch_bi_hor(part_num, IP_map_scu_ptr_16x32);
			break;
		case SPLIT_BI_VER:
			IP_buffer_16x16.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_32x16, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x8.fetch_bi_ver(part_num, IP_buffer_ptr_U_16x8);
			IP_buffer_V_8x8.fetch_bi_ver(part_num, IP_buffer_ptr_V_16x8);

			IP_map_scu_16x16.fetch_bi_ver(part_num, IP_map_scu_ptr_32x16);
			break;
		default: assert(0);
		}

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_16x16.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_16x16.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_16x16.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_16x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 4);
			int flag2 = (x0 > 0 && ((IP_map_scu_16x8.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 3);
			int flag3 = (x0 > 0 && ((IP_map_scu_8x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 3) && (cu_height_log2 == 4);
			//if (flag1 || flag2 || flag3)
				//md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x8.map_scu_left_line_new[0]) >> 16) & 0x7F;

			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_8x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma == NO_SPLIT)
		{
			best_split_mode = best_split_mode_luma;

			best_cons_pred_mode = cons_pred_mode_child[0];
			best_tree_status_child = tree_status_child[0];

			/*if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}*/

#if !IP_BUFFER_NULL
			IP_ARRAY_SIZE<4, 8> IP_buffer_ptr_U_4x8;	// liaozhx, HLS. NULL pointer is not allowed
			IP_ARRAY_SIZE<4, 8> IP_buffer_ptr_V_4x8;
			IP_ARRAY_SIZE<8, 4> IP_buffer_ptr_U_8x4;
			IP_ARRAY_SIZE<8, 4> IP_buffer_ptr_V_8x4;
#endif // !IP_BUFFER_NULL
#if 0 //for clean_test pass
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	//IP_buffer_ptr_U_4x8.top_left = zero;
	//IP_buffer_ptr_U_8x4.top_left = zero;
	//IP_buffer_ptr_V_4x8.top_left = zero;
	//IP_buffer_ptr_V_8x4.top_left = zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_ptr_U_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_U_8x4.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_8x4.top_left +=  (zero_1b << (16 - 1));	
#endif
			md_chroma_8(8, 8, md_fw_ptr,
				&cu_data_best_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
				&IP_buffer_U_8x8, &IP_buffer_V_8x8,
#if IP_BUFFER_NULL
				NULL, NULL,
				NULL, NULL,
#else
				&IP_buffer_ptr_U_4x8, &IP_buffer_ptr_V_4x8, //NULL, NULL,
				&IP_buffer_ptr_U_8x4, &IP_buffer_ptr_V_8x4, //NULL, NULL,
#endif // IP_BUFFER_NULL
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16,
				&IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}
		else
		{
			md_16x16_16x8_8x16_loop_md1_chroma(best_split_mode_luma,
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c,
				&cons_pred_mode_child[0], tree_status, &tree_status_child[0],
				&IP_buffer_16x16,
				&IP_buffer_U_8x8,
				&IP_buffer_V_8x8,
				&IP_map_scu_16x16,
				Fetch_Ref_window_ptr);

			best_split_mode = best_split_mode_luma;

			copy_cu_data_array_src_chroma<8>(&cu_data_best_c, &cu_data_temp_c, 0, 0,
				cu_width_log2, cu_height_log2, cu_width_log2);

			best_cons_pred_mode = cons_pred_mode_child[0];
			best_tree_status_child = tree_status_child[0];

			if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}
		}

		switch (split_mode)
		{
		case SPLIT_BI_HOR:
			IP_buffer_U_8x8.update_bi_hor(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_8x8.update_bi_hor(part_num, cu_data_best_c.reco_v);
			IP_map_scu_16x16.update_bi_hor(part_num, IP_map_scu_ptr_16x32->cur_map_scu, cu_data_best_c.map_scu);
			break;
		case SPLIT_BI_VER:
			IP_buffer_U_8x8.update_bi_ver(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_8x8.update_bi_ver(part_num, cu_data_best_c.reco_v);
			IP_map_scu_16x16.update_bi_ver(part_num, IP_map_scu_ptr_32x16->cur_map_scu, cu_data_best_c.map_scu);
			break;
		default: assert(0);
		}
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.coef_u cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.coef_v cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.reco_u cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.reco_v cyclic factor=8 dim=2
//#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.ipm cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.map_scu cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.num_nz_coef cyclic factor=4 dim=2
		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}

s64 prepare_cu_info_COM_combine_only_bt_chroma(MD_FW* md_fw_ptr, s64 lambdaY,
	U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct)
{
	com_split_get_part_structure_only_bt(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], qt_depth, bet_depth + 1, &split_struct->num_split[i], split_mode);
	}

	U2 num_cons_pred_mode_loop[1];
	num_cons_pred_mode_loop[0] = (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) ? 2 : 1;
	s64 cost_temp = 0;
	U3 bit_cnt;
	//U3 part_num = 0;
	U3 rate_bit = 0;
	if (cons_pred_mode == NO_MODE_CONS && ((md_fw_ptr->slice_type)) != 1) {
		if (num_cons_pred_mode_loop[0] == 1) {
			cons_pred_mode_child[0] = NO_MODE_CONS;
		}
		else {
			cons_pred_mode_child[0] = (((cons_flag != 0 ? ONLY_INTER : ONLY_INTRA)));
		}
	}
	else {
		cons_pred_mode_child[0] = cons_pred_mode;
	}
	//clear_map_scu_chroma(me_line_map_ptr, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width,
	//	md_fw_ptr->pic_height);

	//if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height) {
	//	/* consider CU split flag */
	//	com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);

	//	rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
	//	if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) {
	//		rate_bit += 1;
	//	}
	//	bit_cnt = rate_bit;

	//	cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	//}
	return cost_temp;
}


static s64 md_16x16_loop_md1_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_U_8x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_V_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_U_16x8, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_V_16x8,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	U3 split_mode = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	if (isBTH_allow) {

		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);
		prepare_cu_info_COM_combine_only_bt_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_16x16_top_md1_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, bet_depth + 1, cons_pred_mode_BTH[0], child_tree_status[0], part_num, split_struct,
				cu_width_log2,
				IP_buffer_ptr_16x32, IP_buffer_ptr_32x16,
				IP_buffer_ptr_U_8x16, IP_buffer_ptr_V_8x16,
				IP_buffer_ptr_U_16x8, IP_buffer_ptr_V_16x8,
				IP_map_scu_ptr_16x32, IP_map_scu_ptr_32x16,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return 1;
}

void copy_to_cu_data_chroma16_chroma(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1], int coef_nz_y, u32 luma_map_scu)
{
	int i;
	int j;
	int k;
	int idx;
	int size;
	U3 cu_width_log2;
	U3 cu_height_log2;
	//int num_coef_y = 0;
	U12 num_coef_u = 0;
	U12 num_coef_v = 0;
	//int num_nnz_sum = 0;
	U1 cu_cbf_flag = 0;
	//int pb_idx_u = 0;
	U2 tb_idx_u = 0;
	U2 tb_idx_y;
	U14 num_coef_y = 0;

	cu_width_log2 = mod_info_best->cu_width_log2;
	cu_height_log2 = mod_info_best->cu_height_log2;

	if ((tree_status) != (TREE_L)) {
		/* copy coef */
		size = ((((mod_info_best->cu_width * mod_info_best->cu_height)) * sizeof(s16) >> 2));
		//if (coef_rec_write_flag[0] == 0) {////for Intra_CU
		for (i = 0; i < ((mod_info_best->cu_width * mod_info_best->cu_height) >> 2); i++) {
			cu_data->coef_u[i] = mod_info_best->coef_u[i];
			cu_data->coef_v[i] = mod_info_best->coef_v[i];
			cu_data->reco_u[i] = mod_info_best->rec_u[i];
			cu_data->reco_v[i] = mod_info_best->rec_v[i];  /* copy reco */

		}

		//}

		for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height / 4; i++) {
			num_coef_u += ((cu_data->coef_u[i]) != 0 ? 1 : 0);
			num_coef_v += ((cu_data->coef_v[i]) != 0 ? 1 : 0);
		}
	}

	num_coef_y = coef_nz_y;

	if (tree_status == TREE_LC) {
		cu_cbf_flag = ((num_coef_y == 0 ? 0 : 1)) | ((num_coef_u == 0 ? 0 : 1)) | ((num_coef_v == 0 ? 0 : 1));
	}
	else if ((tree_status) == (TREE_L)) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	/* copy mode info */
	if ((tree_status) == (TREE_C)) {
		idx = 0;
		for (j = 0; j < mod_info_best->cu_height >> 2; j++) {
			for (i = 0; i < mod_info_best->cu_width >> 2; i++) {
				//intra chroma mode
				if (mod_info_best->cu_mode == 0) {
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
				// residual and partition
				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < mod_info_best->cu_height >> 2; j++) {
			for (i = 0; i < mod_info_best->cu_width >> 2; i++) {

				//			tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, mod_info_best->cu_width,mod_info_best->cu_height);
				tb_idx_y = get_part_idx(SIZE_2Nx2N, i << 2, j << 2, mod_info_best->cu_width,
					mod_info_best->cu_height);
				//pb_idx_u = 0;
				tb_idx_u = 0;

				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
				}

				if (cu_cbf_flag) {
					//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((1 << 24));
					cu_data->map_scu[idx + i] = luma_map_scu | ((1 << 24));
				}
				else {
					//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((~(1 << 24)));
					cu_data->map_scu[idx + i] = luma_map_scu & ((~(1 << 24)));
				}
				if (mod_info_best->cu_mode == 0)
				{
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}

	}
}


#if !ONLY_QT_QT
s64 md_chroma_16_chroma(MD_FW* md_fw_ptr,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* cu_data_temp_ptr_c,
	U14 x, U14 y, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status, U4 cud,
	MD_INPUT* md_input_ptr, 
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U1 need_dochroma, U2 ctx_cons_pred_mode,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_U_16x16, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_V_16x16,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_U_8x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_V_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_U_16x8, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_V_16x8,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32, IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16
)
{
	if (!need_dochroma)
	{
		return 0;
	}

	static MD_COM_MODE_BEST mod_info_best;
	static COM_PART_INFO bst_info_pb_info;
	static COM_PART_INFO bst_info_tb_info;

	U2 cons_pred_mode = ctx_cons_pred_mode;
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);

	//pel* orgU;
	static RDOQ_ARRAY rdoq_array;
	static RDOQ_MODEL rdoq_model_t;
	static RDOQ_MODEL model_cnt_t;
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_run[i][j] = 0;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_cbf[i][j] = 0;
		}
	}
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_level[i][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < 12; k++)
			{
				for (int l = 0; l < 2; l++)
				{
					rdoq_array.rdoq_est_last[i][j][k][l] = 0;
				}
			}
		}
	}
	int i;
	for (i = 0; i < 12; i++) {
		rdoq_model_t.last1[i] = PROB_INIT;
		model_cnt_t.last1[i] = 0;
	}
	for (i = 0; i < 22; i++) {
		rdoq_model_t.last2[i] = PROB_INIT;
		model_cnt_t.last2[i] = 0;
	}
	for (i = 0; i < 24; i++) {
		rdoq_model_t.run_rdoq[i] = PROB_INIT;
		model_cnt_t.run_rdoq[i] = 0;
		rdoq_model_t.level[i] = PROB_INIT;
		model_cnt_t.level[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		rdoq_model_t.cbf[i] = PROB_INIT;
		model_cnt_t.cbf[i] = 0;
	}
	U1 coef_rec_write_flag[1] = { 0 };
	int coef_nz_y;
#if SUB_TMVP
	static CORE_SBTMVP core_sbtmvp;
	CORE_SBTMVP* core_sbtmvp_ptr = &core_sbtmvp;
	core_sbtmvp.sbTmvp_flag = 0;
	core_sbtmvp.best_sbTmvp_flag = 0;
	for (int k = 0; k < SBTMVP_NUM; k++)
	{
		for (int i = 0; i < REFP_NUM; i++)
		{
			core_sbtmvp.sbTmvp[k].ref_idx[i] = -1;
			core_sbtmvp.best_sbTmvp[k].ref_idx[i] = -1;
			for (int j = 0; j < MV_D; j++)
			{
				core_sbtmvp.sbTmvp[k].mv[i][j] = 0;
				core_sbtmvp.best_sbTmvp[k].mv[i][j] = 0;
			}
		}
	}
#endif

	coef_nz_y = mode_cu_init_chroma<16>(&rdoq_model_t, &model_cnt_t, &rdoq_array, &mod_info_best, &bst_info_pb_info, &bst_info_tb_info, x, y, cu_width_log2, cu_height_log2, cud,
		md_fw_ptr->pic_width_in_scu, TREE_C, &cons_pred_mode, cu_data_temp_ptr_c, md_input_ptr
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
	);


	static pel orgU[2][16][16];
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 16; i++) {
			for (int j = 0; j < 16; j++) {
				if (k == 0) {
					orgU[k][i][j] = p_fenc_LCU_U[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
				else {
					orgU[k][i][j] = p_fenc_LCU_V[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
			}
		}
	}

#if USE_TSCPM
	pel piRecoY[32][32];// = md_input_ptr->reco_y + (mod_info_best.y_pos - md_input_ptr->pix_y) * 64 + (mod_info_best.x_pos - md_input_ptr->pix_x);
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < 32; j++) {
			piRecoY[i][j] = md_input_ptr->reco_y[(mod_info_best.y_pos - md_input_ptr->pix_y + i) ][(mod_info_best.x_pos - md_input_ptr->pix_x + j)];
		}
	}
#endif
#if CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;

	const int com_tbl_qp_chroma_adjust[64] =
	{
		//this table aligned with spec
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
		46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
		50, 50, 50, 51
	};
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (md_input_ptr->cu_dqp_enable) {
		int qp_offset_bit_depth = (8 * (md_fw_ptr->bit_depth_internal - 8));

		md_input_ptr->qp_u = md_input_ptr->cu_luma_qp[luma_cup];
		md_input_ptr->qp_v = md_input_ptr->cu_luma_qp[luma_cup];

		md_input_ptr->qp_u += md_input_ptr->chroma_quant_param_delta_cb - qp_offset_bit_depth;
		md_input_ptr->qp_v += md_input_ptr->chroma_quant_param_delta_cr - qp_offset_bit_depth;

		md_input_ptr->qp_u = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_u)];
		md_input_ptr->qp_v = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_v)];

		md_input_ptr->qp_u = COM_CLIP(md_input_ptr->qp_u + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
		md_input_ptr->qp_v = COM_CLIP(md_input_ptr->qp_v + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
	}
#endif
	rdo_chroma_16(&rdoq_array, md_fw_ptr, tree_status, &mod_info_best, md_input_ptr, orgU,
		IP_buffer_ptr_U_16x16, IP_buffer_ptr_V_16x16,
		IP_buffer_ptr_U_8x16, IP_buffer_ptr_V_8x16,
		IP_buffer_ptr_U_16x8, IP_buffer_ptr_V_16x8,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
		coef_rec_write_flag, Fetch_Ref_window_ptr,
		piRecoY, 
		IP_buffer_ptr_32x32, IP_buffer_ptr_16x32, IP_buffer_ptr_32x16,
		IP_map_scu_ptr_32x32, IP_map_scu_ptr_16x32, IP_map_scu_ptr_32x16
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
	);

	if (mod_info_best.cu_mode == MODE_INTRA)
	{
		mod_info_best.cu_mode = 0;
		mod_info_best.affine_flag = 0;
	}
	else {

		coef_rec_write_flag[0] = 1;
	}
#if !CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
#endif
	luma_map_scu = md_input_ptr->map_scu[luma_cup];

	//copy_to_cu_data_chroma16_chroma(md_input_ptr->qp_y, curr_cu_data_temp_array_ptr, &mod_info_best, tree_status, coef_rec_write_flag, coef_nz_y, luma_map_scu);
	copy_to_cu_data_chroma<16>(md_input_ptr->qp_y, cu_data_temp_ptr_c, &mod_info_best, tree_status, coef_rec_write_flag, coef_nz_y, luma_map_scu
#if CUDQP_QP_MAP
		, md_input_ptr->cu_qp_group_pred_qp, md_input_ptr->cu_dqp_enable
#endif
		);
#if CUDQP_QP_MAP
	int cu_cbf_flag = MCU_GET_CBF(cu_data_temp_ptr_c->map_scu[0]);
	int qpy = MCU_GET_QP(cu_data_temp_ptr_c->map_scu[0]);
	if (md_input_ptr->cu_dqp_enable && tree_status != TREE_C && cu_cbf_flag)
	{

		md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->cu_luma_qp[luma_cup];

	}

#endif
	return 0;
}

static s64 circuit_32x32_32x16_16x32_md1_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_U_16x16,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_V_16x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	strFetch_ref_window* Fetch_Ref_window_ptr, int prev_split_mode)
{
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
	U3 num_split_to_try = split_struct.num_split[part_num];

	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[2] = { TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;

	U3 split_mode[3];
	U2 best_index;

	split_mode[0] = NO_SPLIT;
	split_mode[1] = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return  0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma])
	{
		if (best_split_mode_luma == NO_SPLIT)
			best_index = 0;
		else
			best_index = 1;

		static IP_ARRAY_SIZE<16, 32>				IP_buffer_16x32;
		static IP_ARRAY_SIZE<32, 16>				IP_buffer_32x16;
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_U_8x16;
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_V_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_U_16x8;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_V_16x8;
		static IP_MAP_SCU_SIZE<16, 32>				IP_map_scu_16x32;
		static IP_MAP_SCU_SIZE<32, 16>				IP_map_scu_32x16;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>	cu_data_temp_c[2];
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;

		for (int i = 0; i < 2; i++)
		{
			cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_temp_c[i], prev_cu_data_temp_ptr, qt_depth + bet_depth);
		}

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_16x32.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_16x32.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_16x32.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);
		//
		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_32x16.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_32x16.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_32x16.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);

		switch (prev_split_mode)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 16 && cu_height == 32);
			IP_buffer_16x32.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_32x32, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x16.fetch_bi_ver(part_num, IP_buffer_ptr_U_16x16);
			IP_buffer_V_8x16.fetch_bi_ver(part_num, IP_buffer_ptr_V_16x16);
			IP_map_scu_16x32.fetch_bi_ver(part_num, IP_map_scu_ptr_32x32);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 32 && cu_height == 16);
			IP_buffer_32x16.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_32x32, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_16x8.fetch_bi_hor(part_num, IP_buffer_ptr_U_16x16);
			IP_buffer_V_16x8.fetch_bi_hor(part_num, IP_buffer_ptr_V_16x16);
			IP_map_scu_32x16.fetch_bi_hor(part_num, IP_map_scu_ptr_32x32);
			break;
		default:assert(0);
		}
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_ptr_32x32->map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 5) && (cu_height_log2 == 5);
			int flag2 = (x0 > 0 && ((IP_map_scu_16x32.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 5);
			int flag3 = (x0 > 0 && ((IP_map_scu_32x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 5) && (cu_height_log2 == 4);

			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_ptr_32x32->map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x32.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_32x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma == NO_SPLIT) {

			best_split_mode = best_split_mode_luma;
			best_cons_pred_mode = cons_pred_mode_child[best_index];
			best_tree_status_child = tree_status_child[best_index];

			/*if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}*/

			if (cu_height == 16 || cu_width == 16)
			{
				md_chroma_16_chroma(md_fw_ptr,
					&cu_data_best_c,
					x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
					md_input_ptr,
					p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
					RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
					((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cons_pred_mode,
					NULL, NULL,
					&IP_buffer_U_8x16, &IP_buffer_V_8x16,
					&IP_buffer_U_16x8, &IP_buffer_V_16x8,
					Fetch_Ref_window_ptr,
					IP_buffer_ptr_32x32, &IP_buffer_16x32, &IP_buffer_32x16,
					IP_map_scu_ptr_32x32, &IP_map_scu_16x32, &IP_map_scu_32x16
				);
			}

		}
		else
		{
			md_16x16_loop_md1_chroma(
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c[best_index],
				&cons_pred_mode_child[best_index], tree_status, &tree_status_child[best_index],
				&IP_buffer_16x32, &IP_buffer_32x16,
				&IP_buffer_U_8x16, &IP_buffer_V_8x16,
				&IP_buffer_U_16x8, &IP_buffer_V_16x8,
				&IP_map_scu_16x32, &IP_map_scu_32x16,
				Fetch_Ref_window_ptr);

			copy_cu_data_array_src_chroma<16>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}

		switch (prev_split_mode)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 16 && cu_height == 32);
			IP_buffer_U_8x16.update_bi_ver(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_8x16.update_bi_ver(part_num, cu_data_best_c.reco_v);
			IP_map_scu_16x32.update_bi_ver(part_num, IP_map_scu_ptr_32x32->cur_map_scu, cu_data_best_c.map_scu);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 32 && cu_height == 16);
			IP_buffer_U_16x8.update_bi_hor(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_16x8.update_bi_hor(part_num, cu_data_best_c.reco_v);
			IP_map_scu_32x16.update_bi_hor(part_num, IP_map_scu_ptr_32x32->cur_map_scu, cu_data_best_c.map_scu);
			break;
		default:assert(0);
		}

		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}
#endif //!ONLY_QT_QT


s64 prepare_cu_info_COM_combine_chroma(MD_FW* md_fw_ptr, s64 lambdaY,
	U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct)
{

	md_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], qt_depth, split_mode == NO_SPLIT ? bet_depth : (U3)(bet_depth + 1), &split_struct->num_split[i], split_mode);
	}

	if (split_mode == NO_SPLIT)
		return 0;

	U2 num_cons_pred_mode_loop[1];
	num_cons_pred_mode_loop[0] = (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) ? 2 : 1;
	s64 cost_temp = 0;
	U3 bit_cnt;
	//U3 part_num = 0;
	U3 rate_bit = 0;
	if (cons_pred_mode == NO_MODE_CONS && ((md_fw_ptr->slice_type)) != 1) {
		if (num_cons_pred_mode_loop[0] == 1) {
			cons_pred_mode_child[0] = NO_MODE_CONS;
		}
		else {
			cons_pred_mode_child[0] = (((cons_flag != 0 ? ONLY_INTER : ONLY_INTRA)));
		}
	}
	else {
		cons_pred_mode_child[0] = cons_pred_mode;
	}
	//clear_map_scu_chroma(me_line_map_ptr, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width,
	//	md_fw_ptr->pic_height);

	return cost_temp;
}


#if !ONLY_QT_QT
s64 md_32x32_32x16_16x32_loop_md1_chroma(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 child_tree_status[1],
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_U_16x16,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_V_16x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;

	if (isBTH_allow) {
		child_tree_status[0] = TREE_LC;
		prepare_cu_info_COM_combine_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_32x32_32x16_16x32_md1_chroma(md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2,
				IP_buffer_ptr_32x32,
				IP_buffer_ptr_U_16x16,
				IP_buffer_ptr_V_16x16,
				IP_map_scu_ptr_32x32,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return 1;
}
#endif //!ONLY_QT_QT


void com_check_split_mode_only_qt_chroma(U14 x0, U14 y0, MD_FW* md_fw_ptr, U1* split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3 bet_depth, U3* num_split)
{
	for (U3 i = 0; i <= (SPLIT_QUAD); i++) {
		split_allow[i] = 0;
	}

	//constraints parameters
	const U4 min_qt_size = (md_fw_ptr->min_qt_size);

	U7 cu_w = 1 << cu_width_log2;
	U7 cu_h = 1 << cu_height_log2;
	U1 boundary = !(x0 + cu_w <= md_fw_ptr->pic_width && y0 + cu_h <= md_fw_ptr->pic_height);
	U1 boundary_r = boundary && x0 + cu_w > md_fw_ptr->pic_width && !(y0 + cu_h > md_fw_ptr->pic_height);
	U1 boundary_b = boundary && y0 + cu_h > md_fw_ptr->pic_height && !(x0 + cu_w > md_fw_ptr->pic_width);

	U3 num_split_to_try = 0;

	if (boundary) {
		if (!boundary_r && !boundary_b) {
			split_allow[SPLIT_QUAD] = 1;
		}
		else if (boundary_r) {
			split_allow[SPLIT_BI_VER] = 1;
		}
		else if (boundary_b) {
			split_allow[SPLIT_BI_HOR] = 1;
		}
	}
	else {
		split_allow[NO_SPLIT] = 1;
		split_allow[SPLIT_QUAD] = cu_w > min_qt_size && bet_depth == 0 ? 1 : 0;
		split_allow[SPLIT_BI_HOR] = 1;
		split_allow[SPLIT_BI_VER] = 1;
	}
	for (int i = 0; i < 6; i++) {
		num_split_to_try += split_allow[i];
	}

	if (num_split_to_try == 0)
	{
		split_allow[NO_SPLIT] = 1;
	}

	*num_split = num_split_to_try;
}


s64 prepare_cu_info_COM_combine_only_qt_8_chroma(MD_FW* md_fw_ptr, s64 lambdaY, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct)
{
	com_split_get_part_structure_only_qt(x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode_only_qt_chroma(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], bet_depth, &split_struct->num_split[i]);
	}

	s64 cost_temp = 0;
	U3 rate_bit = 0;

	//clear_map_scu_chroma(me_line_map_ptr, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width,
	//	md_fw_ptr->pic_height);

	//if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height) {
	//	/* consider CU split flag */
	//	//com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);
	//	rate_bit++;
	//	cost_temp += (rate_bit * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	//}
	return cost_temp;
}

void cu_data_chroma_pos_8_chroma(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
void cu_data_chroma_pos_8_chroma(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_CHROMA_SIZE<8>* dst, ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}

void update_chroma_4_chroma(MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef_y_pb_part[2][16], U1 i, pel pred[2][4 * 4], pel rec[2][4 * 4], int num_nz[4][3],
	U12 size_tmp1, int rdo_idx, s64 cost, s64* cost_best1, int* num_nz1, int* num_nz2, int* best_ipd1, U4 bit_depth)

{
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	U7 cu_width = mod_info_curr_ptr->cu_width;
	U7 cu_height = mod_info_curr_ptr->cu_height;
	U12 size_tmp = cu_width * cu_height >> 2;
	U2 inter_mode = mod_info_curr_ptr->cu_mode;
	int j;

	if (inter_mode != 0) {
		mod_info_curr_ptr->num_nz[0][1] = num_nz[0][1];
		mod_info_curr_ptr->num_nz[0][2] = num_nz[0][2];

		if (inter_mode == 2) {
			num_nz[0][i + 1] = 0;
		}
		if (num_nz[0][i + 1] == 0) {
			for (j = 0; j < size_tmp; j++) {
#pragma HLS LOOP_TRIPCOUNT max=1024
				if (i == 0)mod_info_curr_ptr->coef_u[j] = 0;
				else if (i == 1)mod_info_curr_ptr->coef_v[j] = 0;
				if (i == 0) {
					mod_info_curr_ptr->rec_u[j] = mod_info_curr_ptr->pred_u[j] = (((0 > (((1 << bit_depth) - 1 < (pred[i][j]) ?
						(1 << bit_depth) - 1 : (pred[i][j]))) ? 0 : (((1 << bit_depth) - 1 < (pred[i][j]) ? (1 << bit_depth) - 1 : (pred[i][j]))))));
				}
				else if (i == 1) {
					mod_info_curr_ptr->rec_v[j] = mod_info_curr_ptr->pred_v[j] = (((0 > (((1 << bit_depth) - 1 < (pred[i][j]) ?
						(1 << bit_depth) - 1 : (pred[i][j]))) ? 0 : (((1 << bit_depth) - 1 < (pred[i][j]) ? (1 << bit_depth) - 1 : (pred[i][j]))))));
				}
			}
		}
		else {
			for (j = 0; j < size_tmp; j++) {

#pragma HLS LOOP_TRIPCOUNT max=1024
				if (i == 0) {
					mod_info_curr_ptr->coef_u[j] = coef_y_pb_part[0][j];
					mod_info_curr_ptr->rec_u[j] = ((pel)((0 > (((1 << bit_depth) - 1 < (rec[i][j]) ?
						(1 << bit_depth) - 1 : (rec[i][j]))) ? 0 : (((1 << bit_depth) - 1 < (rec[i][j]) ? (1 << bit_depth) - 1 : (rec[i][j]))))));

				}
				else if (i == 1) {
					mod_info_curr_ptr->coef_v[j] = coef_y_pb_part[1][j];
					mod_info_curr_ptr->rec_v[j] = ((pel)((0 > (((1 << bit_depth) - 1 < (rec[i][j]) ?
						(1 << bit_depth) - 1 : (rec[i][j]))) ? 0 : (((1 << bit_depth) - 1 < (rec[i][j]) ? (1 << bit_depth) - 1 : (rec[i][j]))))));
				}


			}
			//coef_rec_write_flag[0] = 1;
		}
	}
	else {
		if (i == 1 && cost < *cost_best1)
		{
			*cost_best1 = cost;
			*best_ipd1 = rdo_idx / 2;
			memcpy_rewrintra(mod_info_curr_ptr->rec_u, rec[0], size_tmp1);
			memcpy_rewrintra(mod_info_curr_ptr->rec_v, rec[1], size_tmp1);


			memcpy_rewrintra(mod_info_curr_ptr->coef_u, coef_y_pb_part[0], size_tmp1);
			memcpy_rewrintra(mod_info_curr_ptr->coef_v, coef_y_pb_part[1], size_tmp1);

			*num_nz1 = num_nz[0][1];
			*num_nz2 = num_nz[0][2];


		}
		if (rdo_idx == 9) {
			mod_info_curr_ptr->ipm[0][1] = *best_ipd1;
			mod_info_curr_ptr->num_nz[0][1] = *num_nz1;
			mod_info_curr_ptr->num_nz[0][2] = *num_nz2;
		}
	}

}

void rdo_chroma_dataflow_4(int all_rdo_mode, MD_COM_MODE_BEST* mod_info_best, U7 cu_width, U7 cu_height, U3 cu_width_log2_chroma, U3 cu_height_log2_chroma,
	pel orgU[2][16], pel pred[12][16], pel pred0[12][16], U4 bit_depth,  int num_nz_out[12],int num_nz_out1[12],
	RDOQ_ARRAY* rdoq_array, MD_INPUT* md_input_ptr, MD_FW* md_fw_ptr, s16 coef_uv_pb_part[12][16], 
	pel rec_tmp[12][4 * 4],	U2 tree_status) 
{
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_run complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_last complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->lambda complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->dist_chroma_weight complete dim=0
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred complete dim=0	
#pragma HLS ARRAY_PARTITION variable=pred0 complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=2	
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1

	int num_nz_q;
	int num_nz[4][3];
	int num_nz1[4][3];
	int num_nz2[4][3];
	for (int rdo_idx = 0; rdo_idx < all_rdo_mode; rdo_idx++)
	{
		
		s16 diff[12][4 * 4];
		s16 coef_t_h[2][4 * 4];
		s16 coef_t_v[2][4 * 4];
		s16 coef_it_v[2][4 * 4];
		s16 coef_dq[4 * 4];
		pel resi[4 * 4];
		s16 coefVer_q[12][16];

#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=12 max=12 avg=12
		U1 uv = rdo_idx & 1;
		calcDiffChroma4(mod_info_best->cu_mode, cu_width, cu_height, orgU[uv], pred[rdo_idx], diff[rdo_idx]);

		transform_hor_4x4(bit_depth, diff[rdo_idx], coef_t_h[uv], 1 << cu_width_log2_chroma, 1 << cu_height_log2_chroma, md_fw_ptr->slice_type, uv + 1, tree_status);

		transform_ver_4x4(md_fw_ptr->slice_type, bit_depth, md_input_ptr->qp_u, mod_info_best->num_nz, md_input_ptr->lambda, coef_t_h[uv], coef_t_v[uv], 1 << cu_width_log2_chroma, 1 << cu_height_log2_chroma, uv + 1, 0, tree_status);

		quantChroma4(&num_nz_q, rdoq_array, bit_depth, md_input_ptr->qp_u, md_input_ptr->lambda[1], 1, coef_t_v[uv], cu_width_log2_chroma,
			cu_height_log2_chroma, uv + 1, md_fw_ptr->slice_type, coef_uv_pb_part[rdo_idx], coefVer_q[rdo_idx]);

		dquantChroma4(bit_depth, md_input_ptr->qp_u, num_nz, coefVer_q[rdo_idx], coef_dq, uv + 1, num_nz_q, num_nz1, num_nz2);

		invTransVerChroma4(bit_depth, md_input_ptr->qp_u, num_nz, coef_dq, coef_it_v[uv], uv + 1);

		invTransHorChroma4(bit_depth, num_nz1, coef_it_v[uv], uv + 1, resi);

		reconChroma4(mod_info_best->x_pos, mod_info_best->y_pos, cu_width_log2_chroma, cu_height_log2_chroma, bit_depth,
			num_nz2, mod_info_best->tb_part, pred0[rdo_idx], rec_tmp[rdo_idx], uv + 1, resi, num_nz_out, num_nz_out1, rdo_idx, mod_info_best->cu_mode);
	}

}

#if !CHROMA_4_PIPELINE_DUMMY
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

	pel info_best_rec_u[4][4];
	pel info_best_rec_v[4][4];
	pel info_best_coef_u[4][4];
	pel info_best_coef_v[4][4];

	static s16 coef_t_h[2][4 * 4];
	static s16 coef_t_v[2][4 * 4];
//	static s16 coef_q[2][16];
	static s16 coef_dq[4 * 4];
	static s16 coef_it_v[2][4 * 4];
	
	
	U4 bit_depth = md_fw_ptr->bit_depth_internal;
	U3 cu_width_log2 = mod_info_best->cu_width_log2;
	U3 cu_height_log2 = mod_info_best->cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	U13 pb_x = mod_info_best->x_pos;
	U13 pb_y = mod_info_best->y_pos;
	
	pel pred_inter[2][4][4];
	U12 size_tmp = (1 << (cu_width_log2 - 1)) * (1 << (cu_height_log2 - 1));


	U1 uv;
	static pel nb[3][2][4 * 2 + 3];
#if USE_TSCPM
	pel nb_y[2][8 * 2 + 3];
#endif
	static int num_nz[4][3];
	static pel resi[4 * 4];

	s64 cost;
	s64 cost_best1 = 9223372036854775807;
	int tscpm_enable_flag = 1;
	int best_ipd1 = 0;
	int num_nz1 = 0;
	int num_nz2 = 0;
	pel rec_tb = 0;
	U7 cu_width_c = cu_width >> 1;
	U7 cu_height_c = cu_height >> 1;
	U3 cu_width_log2_chroma = cu_width_log2 - 1;
	U3 cu_height_log2_chroma = cu_height_log2 - 1;
#if USE_TSCPM
	RDOQ_MODEL model_cnt_inner[12];
	int sum_run[12];
	U32 last1_tmp[12][6];
	U32 last2_tmp[12][6];
#else
	RDOQ_MODEL model_cnt_inner[10];
	int sum_run[10];
	U32 last1_tmp[10][6];
	U32 last2_tmp[10][6];
#endif

	u16 avail_cu_output = 0;

	int num_nz_out[12] ;
	int num_nz_out1[12] ;
	pel pred[12][4 * 4];
	s16 coef_uv_pb_part[12][16];
	s16 coef_q[12][4 * 4];
	static pel rec_tmp[12][4 * 4];
	static pel rec_tmp1[12][4][4];
	pel pred0[12][4 * 4];
	pel pred1[12][4][4];
	static s16 diff[12][4 * 4];
#pragma HLS ARRAY_PARTITION variable=nb complete dim=3
#pragma HLS ARRAY_PARTITION variable=nb_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=1
#pragma HLS ARRAY_PARTITION variable=pred0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred1 complete dim=3
#pragma HLS ARRAY_PARTITION variable=pred1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=diff complete dim=2
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=rec_tmp1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=rec_tmp1 complete dim=3
//#pragma HLS ARRAY_PARTITION variable=rec_tmp1 cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=pred_inter complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_inter complete dim=3
//修改冲突
#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_u complete  dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_v complete  dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_v complete dim=1
//约束
#pragma HLS resource variable=mod_info_best_coef_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_coef_v core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_u core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best_rec_v core=RAM_1P_BRAM 

#pragma HLS ARRAY_PARTITION variable=info_best_rec_u complete  dim=0
#pragma HLS ARRAY_PARTITION variable=info_best_rec_v complete  dim=0
#pragma HLS ARRAY_PARTITION variable=info_best_coef_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=info_best_coef_v complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=num_nz_out1 complete dim=1

	for (int i = 0; i < 2 * 4 + 3; i++) {
#pragma HLS UNROLL
		nb[1][0][i] = (1 << bit_depth - 1);
		nb[1][1][i] = (1 << bit_depth - 1);
		nb[2][0][i] = (1 << bit_depth - 1);
		nb[2][1][i] = (1 << bit_depth - 1);
	}

	for (int i = 0; i < 2 * 8 + 3; i++) {
#pragma HLS UNROLL
		nb_y[0][i] = (1 << bit_depth - 1);
		nb_y[1][i] = (1 << bit_depth - 1);
	}


	if (mod_info_best->cu_mode != 0 )
	{
		com_mc_cu_uv4(mod_info_best, md_fw_ptr->pic_width, md_fw_ptr->pic_height, mod_info_best->refi, mod_info_best->mv,
			pred_inter[0], pred_inter[1], bit_depth, Fetch_Ref_window_ptr);

		for (int j = 0; j < 4; j++)
		{
#pragma HLS UNROLL
			for (int i = 0; i < 4; i++)
			{
#pragma HLS UNROLL			
				pred[0][j * 4 + i] = pred_inter[0][j][i];
				pred[1][j * 4 + i] = pred_inter[1][j][i];
			}
		}
	}

	else {
		ipred_c_prepare(md_fw_ptr, cu_width_log2 - 1, cu_height_log2 - 1, tree_status, 0, pb_x, pb_y,
			mod_info_best, diff,  nb_y, nb[0 + 1], nb[1 + 1],
			IP_buffer_ptr_U_4x4,
			IP_buffer_ptr_V_4x4,
			bit_depth, piRecoY,
			IP_buffer_ptr_8x8, IP_map_scu_ptr_8x8, &avail_cu_output);
		
		for (int rdo_idx = 0; rdo_idx < all_rdo_mode; rdo_idx++)
		{
#pragma HLS LOOP_TRIPCOUNT max=12
#pragma HLS PIPELINE 
			uv = rdo_idx & 1;
			ipred_c_4(md_fw_ptr, cu_width_log2 - 1, cu_height_log2 - 1, tree_status, uv, pb_x, pb_y, pred[rdo_idx],
				mod_info_best, diff[rdo_idx], orgU[uv], nb_y, nb[(rdo_idx & 1) + 1][0], nb[(rdo_idx & 1) + 1][1], rdo_idx,
				IP_buffer_ptr_U_4x4,
				IP_buffer_ptr_V_4x4,
				bit_depth, piRecoY,
				IP_buffer_ptr_8x8, IP_map_scu_ptr_8x8, &avail_cu_output
			);
		}
	
	}


	for (int k = 0; k < 12; k++)
	{
#pragma HLS UNROLL
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL	
			pred0[k][i] = pred[k][i];
		}
	}

	rdo_chroma_dataflow_4(all_rdo_mode, mod_info_best, cu_width, cu_height, cu_width_log2_chroma, cu_height_log2_chroma,
		orgU, pred, pred0, bit_depth, num_nz_out,num_nz_out1, rdoq_array, md_input_ptr, md_fw_ptr, coef_uv_pb_part, rec_tmp, tree_status);

	for (int i = 0; i < 12; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++) 
		{
#pragma HLS UNROLL
			for (int k = 0; k < 4; k++)
			{
#pragma HLS UNROLL
				pred1[i][j][k] = pred[i][j * 4 + k];
				rec_tmp1[i][j][k] = rec_tmp[i][j * 4 + k];
			}
		}
	}

	//if (mod_info_best->cu_mode == 2)
	//{
	//	num_nz_out1[0] = 0;
	//	num_nz_out1[1] = 0;
	//}

	for (int rdo_idx = 0; rdo_idx < all_rdo_mode; rdo_idx++)
	{
#pragma HLS LOOP_TRIPCOUNT max=12
#pragma HLS PIPELINE 
		uv = rdo_idx & 1;
		deriveRDCostChroma4(mod_info_best->cu_mode, md_input_ptr->dist_chroma_weight[uv], mod_info_best, cu_width_log2_chroma,
			cu_height_log2_chroma, cu_width, cu_height, rec_tmp[rdo_idx], orgU[uv], md_input_ptr->lambda[uv],
			coef_uv_pb_part, tscpm_enable_flag, &cost, bit_depth, uv, num_nz, num_nz_out, rdo_idx);

		update_chroma_4(mod_info_best, coef_uv_pb_part, uv, pred1[rdo_idx], rec_tmp1[rdo_idx], rdo_idx >= 1 ? rec_tmp1[rdo_idx-1]: rec_tmp1[rdo_idx], num_nz, size_tmp, rdo_idx, cost,
			&cost_best1, &num_nz1, &num_nz2, &best_ipd1, bit_depth, num_nz_out1,
			info_best_rec_u,
		    info_best_rec_v,
			info_best_coef_u,
			info_best_coef_v);
		for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
			for (int j = 0; j < 4; j++)
			{
#pragma HLS UNROLL
				mod_info_best_rec_u[i][j] = info_best_rec_u[i][j];
				mod_info_best_rec_v[i][j] = info_best_rec_v[i][j];
				mod_info_best_coef_u[i][j] =info_best_coef_u[i][j];
				mod_info_best_coef_v[i][j] =info_best_coef_v[i][j];

			}
		}
		
	
	}


}
#endif

int rdo_chroma_4_chroma(RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best,
	COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, pel orgU[2][4 * 4], 
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
	strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[8][8], 
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
{


	U4 bit_depth = md_fw_ptr->bit_depth_internal;
	U3 cu_width_log2 = mod_info_best->cu_width_log2;
	U3 cu_height_log2 = mod_info_best->cu_height_log2;

	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);

	U7 cu_width_chroma = cu_width >> 1;
	U7 cu_height_chroma = cu_height >> 1;

	U3 cu_width_log2_chroma = cu_width_log2 - 1;
	U3 cu_height_log2_chroma = cu_height_log2 - 1;
	int all_rdo_mode = 0;
	pel mod_info_best_rec_u[4][4];
	pel mod_info_best_rec_v[4][4];
	pel mod_info_best_coef_u[4][4];
	pel mod_info_best_coef_v[4][4];

	if (tree_status != TREE_LC && tree_status != TREE_C)
	{
		return 0;
	}


	if (mod_info_best->cu_mode != 0)
	{
		all_rdo_mode = 2;
	}
	else
	{
#if USE_TSCPM
		all_rdo_mode = 12;
		//all_rdo_mode = 10;
#else
		all_rdo_mode = 10;
#endif
	}

   U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
   U1 zero_1b = zero & 1;

    tree_status = tree_status + (zero_1b<<(8-1));
    all_rdo_mode = all_rdo_mode +(zero_1b<<(32-1));
    mod_info_best->x_scu = mod_info_best->x_scu + (zero_1b<<(8-1));
    mod_info_best->y_scu = mod_info_best->y_scu + (zero_1b<<(8-1));
	
	mod_info_best->cu_height_log2 = mod_info_best->cu_height_log2 + (zero_1b<<(8-1));
	mod_info_best->cu_width_log2  = mod_info_best->cu_width_log2  + (zero_1b<<(8-1));

   /*
   Fetch_Ref_window_ptr->RefWindowLCU_UP[0] = Fetch_Ref_window_ptr->RefWindowLCU_UP[0] + zero_1b;
   Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1[0] = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1[0] + zero_1b;
   Fetch_Ref_window_ptr->RefWindowLCU_VP[0] = Fetch_Ref_window_ptr->RefWindowLCU_VP[0] + zero_1b;
   Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1[0] = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1[0] + zero_1b;
   */


	rdo_chroma_4_pipeline_chroma(all_rdo_mode, rdoq_array, md_fw_ptr, tree_status, mod_info_best,
		mod_info_curr_pb_info, md_input_ptr, orgU, 
		IP_buffer_ptr_U_4x4,
		IP_buffer_ptr_V_4x4,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos, Fetch_Ref_window_ptr, piRecoY, 
		IP_buffer_ptr_8x8,
		IP_map_scu_ptr_8x8,
		mod_info_best_rec_u,
		mod_info_best_rec_v,
		mod_info_best_coef_u,
		mod_info_best_coef_v);

	for (int k = 0; k < 4; ++k) {
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++) {
#pragma HLS UNROLL	
			if ((k < cu_height_chroma) && (j < cu_width_chroma)) {
				mod_info_best->rec_u[k * cu_width_chroma + j] = mod_info_best_rec_u[k][j];
				mod_info_best->rec_v[k * cu_width_chroma + j] = mod_info_best_rec_v[k][j];
				mod_info_best->coef_u[k * cu_width_chroma + j] = mod_info_best_coef_u[k][j];
				mod_info_best->coef_v[k * cu_width_chroma + j] = mod_info_best_coef_v[k][j];
			}
		}
	}
	return 0;
}

s64 md_chroma_4x4_chroma(MD_FW* md_fw_ptr,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<8>* cu_data_temp_c_ptr,
	U14 x, U14 y, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status, U4 cud,
	MD_INPUT* md_input_ptr,
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U1 need_dochroma, U12 cup, U2 ctx_cons_pred_mode,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
{
	if (!need_dochroma)
	{
		return 0;
	}

	static MD_COM_MODE_BEST mod_info_best;
	static COM_PART_INFO bst_info_pb_info;
	static COM_PART_INFO bst_info_tb_info;

	U2 cons_pred_mode = ctx_cons_pred_mode;
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	//pel* orgU;
	static RDOQ_ARRAY rdoq_array;
	static RDOQ_MODEL rdoq_model_t;
	static RDOQ_MODEL model_cnt_t;
 
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_last complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array.rdoq_est_run complete dim=0

#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.run_rdoq complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.last2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_model_t.last1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.last1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.last2 complete dim=0 
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.run_rdoq complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.level complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt_t.cbf complete dim=0 


	for (int i = 0; i < 24; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 2; j++)
		{
#pragma HLS UNROLL
			rdoq_array.rdoq_est_run[i][j] = 0;
		}
	}
	for (int i = 0; i < 3; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 2; j++)
		{
#pragma HLS UNROLL
			rdoq_array.rdoq_est_cbf[i][j] = 0;
		}
	}
	for (int i = 0; i < 24; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 2; j++)
		{
#pragma HLS UNROLL
			rdoq_array.rdoq_est_level[i][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 6; j++)
		{
#pragma HLS UNROLL
			for (int k = 0; k < 12; k++)
			{
#pragma HLS UNROLL
				for (int l = 0; l < 2; l++)
				{
#pragma HLS UNROLL
					rdoq_array.rdoq_est_last[i][j][k][l] = 0;
				}
			}
		}
	}
	int i;
	for (i = 0; i < 12; i++) {
#pragma HLS UNROLL
		rdoq_model_t.last1[i] = PROB_INIT;
		model_cnt_t.last1[i] = 0;
	}
	for (i = 0; i < 22; i++) {
#pragma HLS UNROLL
		rdoq_model_t.last2[i] = PROB_INIT;
		model_cnt_t.last2[i] = 0;
	}
	for (i = 0; i < 24; i++) {
#pragma HLS UNROLL
		rdoq_model_t.run_rdoq[i] = PROB_INIT;
		model_cnt_t.run_rdoq[i] = 0;
		rdoq_model_t.level[i] = PROB_INIT;
		model_cnt_t.level[i] = 0;
	}
	for (i = 0; i < 3; i++) {
#pragma HLS UNROLL
		rdoq_model_t.cbf[i] = PROB_INIT;
		model_cnt_t.cbf[i] = 0;
	}

	U1 coef_rec_write_flag[1] = { 0 };
	int coef_nz_y;
#if SUB_TMVP
	static CORE_SBTMVP core_sbtmvp;
	CORE_SBTMVP* core_sbtmvp_ptr = &core_sbtmvp;
	core_sbtmvp.sbTmvp_flag = 0;
	core_sbtmvp.best_sbTmvp_flag = 0;
#pragma HLS ARRAY_PARTITION variable=core_sbtmvp.sbTmvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=core_sbtmvp.best_sbTmvp complete dim=0

	for (int k = 0; k < SBTMVP_NUM; k++)
	{
#pragma HLS UNROLL
		for (int i = 0; i < REFP_NUM; i++)
		{
#pragma HLS UNROLL
			core_sbtmvp.sbTmvp[k].ref_idx[i] = -1;
			core_sbtmvp.best_sbTmvp[k].ref_idx[i] = -1;
			for (int j = 0; j < MV_D; j++)
			{
#pragma HLS UNROLL
				core_sbtmvp.sbTmvp[k].mv[i][j] = 0;
				core_sbtmvp.best_sbTmvp[k].mv[i][j] = 0;
			}
		}
	}
#endif


#pragma HLS ARRAY_PARTITION variable=mod_info_best.coef_y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best.coef_u cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=mod_info_best.coef_v cyclic factor=32 dim=1

	coef_nz_y = mode_cu_init_chroma_4_8<4>(&rdoq_model_t, &model_cnt_t, &rdoq_array, &mod_info_best, &bst_info_pb_info, &bst_info_tb_info, x, y, cu_width_log2, cu_height_log2, cud,
		md_fw_ptr->pic_width_in_scu, TREE_C, &cons_pred_mode,
		cu_data_temp_c_ptr,
		md_input_ptr
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
		);

	static pel orgU[2][4 * 4];
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=0
	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 4; i++) {
#pragma HLS PIPELINE
			for (int j = 0; j < 4; j++) {
				if (k == 0) {
					orgU[k][(i << 2) + j] = p_fenc_LCU_U[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
				else {
					orgU[k][(i << 2) + j] = p_fenc_LCU_V[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
			}
		}
	}

	//orgU = p_fenc_LCU_UV + ((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + 64 * ((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1);

#if USE_TSCPM
	pel piRecoY[8][8];// = md_input_ptr->reco_y + (mod_info_best.y_pos - md_input_ptr->pix_y) * 64 + (mod_info_best.x_pos - md_input_ptr->pix_x);
#pragma HLS ARRAY_PARTITION variable=piRecoY complete dim=0
	for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE
		for (int j = 0; j < 8; j++) {
			piRecoY[i][j] = md_input_ptr->reco_y[(mod_info_best.y_pos - md_input_ptr->pix_y + i)][(mod_info_best.x_pos - md_input_ptr->pix_x + j)];
		}
	}
#endif
#if CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;

	const int com_tbl_qp_chroma_adjust[64] =
	{
		//this table aligned with spec
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
		46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
		50, 50, 50, 51
	};
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (md_input_ptr->cu_dqp_enable) {
		int qp_offset_bit_depth = (8 * (md_fw_ptr->bit_depth_internal - 8));

		md_input_ptr->qp_u = md_input_ptr->cu_luma_qp[luma_cup];
		md_input_ptr->qp_v = md_input_ptr->cu_luma_qp[luma_cup];

		md_input_ptr->qp_u += md_input_ptr->chroma_quant_param_delta_cb - qp_offset_bit_depth;
		md_input_ptr->qp_v += md_input_ptr->chroma_quant_param_delta_cr - qp_offset_bit_depth;

		md_input_ptr->qp_u = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_u)];
		md_input_ptr->qp_v = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_v)];

		md_input_ptr->qp_u = COM_CLIP(md_input_ptr->qp_u + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
		md_input_ptr->qp_v = COM_CLIP(md_input_ptr->qp_v + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
	}
#endif
	rdo_chroma_4_chroma(&rdoq_array, md_fw_ptr, tree_status, &mod_info_best, &bst_info_pb_info, md_input_ptr, orgU,
		IP_buffer_ptr_U_4x4,
		IP_buffer_ptr_V_4x4,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos, Fetch_Ref_window_ptr, piRecoY,
		IP_buffer_ptr_8x8,
		IP_map_scu_ptr_8x8);

	if (mod_info_best.cu_mode == 0)
	{
		mod_info_best.cu_mode = 0;
		mod_info_best.affine_flag = 0;
	}
	else {

		coef_rec_write_flag[0] = 1;
	}

#if !CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
#endif
	luma_map_scu = md_input_ptr->map_scu[luma_cup];

	copy_to_cu_data_chroma<4>(md_input_ptr->qp_y, cu_data_temp_c_ptr, &mod_info_best, tree_status, coef_rec_write_flag, coef_nz_y, luma_map_scu
#if CUDQP_QP_MAP
		, md_input_ptr->cu_qp_group_pred_qp, md_input_ptr->cu_dqp_enable
#endif
		);
#if CUDQP_QP_MAP
	int cu_cbf_flag = MCU_GET_CBF(cu_data_temp_c_ptr->map_scu[0]);
	int qpy = MCU_GET_QP(cu_data_temp_c_ptr->map_scu[0]);
	if (md_input_ptr->cu_dqp_enable && tree_status != TREE_C && cu_cbf_flag)
	{
		md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->cu_luma_qp[luma_cup];

	}
#endif
	return 0;

}

static s64 circuit_8x8_top_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num,
	COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode)
{
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

	/*************************for parallel**********************/
	s64 ns_cost = MAX_S64;
	U2 tree_status_child = TREE_LC;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = 0;
	U2 tree_status_temp = tree_status;
	U3 best_split_mode_luma = NO_SPLIT;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma]) {
		/***************************** Step 3: reduce split modes by fast algorithm ********************************/
		split_struct.split_allow[part_num][SPLIT_BI_VER] = 0;
		split_struct.split_allow[part_num][SPLIT_BI_HOR] = 0;

		assert(cu_width == 8 && cu_height == 8 && split_mode == SPLIT_QUAD);
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_ptr_8x8;
		static IP_ARRAY_SIZE<4, 4>					IP_buffer_ptr_U_4x4;
		static IP_ARRAY_SIZE<4, 4>					IP_buffer_ptr_V_4x4;
		static IP_MAP_SCU_SIZE<8, 8>				IP_map_scu_ptr_8x8;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<8>		cu_data_best_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		cu_data_chroma_pos_8_chroma(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_best_c, prev_cu_data_temp_ptr, qt_depth + bet_depth);

		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_ptr_8x8.up_line[i] = IP_buffer_ptr_16x16->up_line[i];
				IP_buffer_ptr_8x8.left_line[i] = IP_buffer_ptr_16x16->left_line[i];
			}
			IP_buffer_ptr_8x8.top_left = IP_buffer_ptr_16x16->top_left;

			/// <chroma ip buf>
			for (int i = 0; i < (16 >> 1); i++)
			{
#pragma HLS PIPELINE
				IP_buffer_ptr_U_4x4.up_line[i] = IP_buffer_ptr_U_8x8->up_line[i];
				IP_buffer_ptr_V_4x4.up_line[i] = IP_buffer_ptr_V_8x8->up_line[i];
				IP_buffer_ptr_U_4x4.left_line[i] = IP_buffer_ptr_U_8x8->left_line[i];
				IP_buffer_ptr_V_4x4.left_line[i] = IP_buffer_ptr_V_8x8->left_line[i];
			}
			IP_buffer_ptr_U_4x4.top_left = IP_buffer_ptr_U_8x8->top_left;
			IP_buffer_ptr_V_4x4.top_left = IP_buffer_ptr_V_8x8->top_left;
			/// </chroma ip buf>
			break;
		case 1:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_ptr_8x8.up_line[i] = IP_buffer_ptr_16x16->up_line[i + 8];
				//IP_buffer_8x8.left_line[i] = IP_buffer_8x8.rec_line_ver[i];
				IP_buffer_ptr_8x8.left_line[i] = md_input_ptr->reco_y[ i + y_pos_lcu][x_pos_lcu - 1];
			}
			IP_buffer_ptr_8x8.top_left = IP_buffer_ptr_16x16->up_line[7];

			/// <chroma ip buf>
			for (int i = 0; i < (16 >> 1); ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_ptr_U_4x4.up_line[i] = IP_buffer_ptr_U_8x8->up_line[i + (8 >> 1)];
				IP_buffer_ptr_V_4x4.up_line[i] = IP_buffer_ptr_V_8x8->up_line[i + (8 >> 1)];
				IP_buffer_ptr_U_4x4.left_line[i] = IP_buffer_ptr_U_4x4.rec_line_ver[i];
				IP_buffer_ptr_V_4x4.left_line[i] = IP_buffer_ptr_V_4x4.rec_line_ver[i];
			}
			IP_buffer_ptr_U_4x4.top_left = IP_buffer_ptr_U_8x8->up_line[3];
			IP_buffer_ptr_V_4x4.top_left = IP_buffer_ptr_V_8x8->up_line[3];
			/// </chroma ip buf>
			break;
		case 2:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_8x8.up_line[i] = IP_buffer_8x8.rec_line_hor[i];
				IP_buffer_ptr_8x8.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu] ;
				IP_buffer_ptr_8x8.left_line[i] = IP_buffer_ptr_16x16->left_line[i + 8];
			}
			IP_buffer_ptr_8x8.top_left = IP_buffer_ptr_16x16->left_line[7];

			/// <chroma ip buf>
			for (int i = 0; i < (16 >> 1); ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_ptr_U_4x4.up_line[i] = IP_buffer_ptr_U_4x4.rec_line_hor[i];
				IP_buffer_ptr_V_4x4.up_line[i] = IP_buffer_ptr_V_4x4.rec_line_hor[i];
				IP_buffer_ptr_U_4x4.left_line[i] = IP_buffer_ptr_U_8x8->left_line[i + 4];
				IP_buffer_ptr_V_4x4.left_line[i] = IP_buffer_ptr_V_8x8->left_line[i + 4];
			}
			IP_buffer_ptr_U_4x4.top_left = IP_buffer_ptr_U_8x8->left_line[3];
			IP_buffer_ptr_V_4x4.top_left = IP_buffer_ptr_V_8x8->left_line[3];
			/// </chroma ip buf>
			break;
		case 3:
			for (int i = 0; i < 8; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_8x8.up_line[i] = IP_buffer_8x8.rec_line_hor[i + 8];
				IP_buffer_ptr_8x8.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu ];
				//IP_buffer_8x8.left_line[i] = IP_buffer_8x8.rec_line_ver[i + 8];				
				IP_buffer_ptr_8x8.left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
			}
			//IP_buffer_8x8.top_left = IP_buffer_8x8.rec_line_hor[7];
			IP_buffer_ptr_8x8.top_left = md_input_ptr->reco_y[y_pos_lcu - 1][x_pos_lcu - 1];

			/// <chroma ip buf>
			for (int i = 0; i < 4; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_ptr_U_4x4.up_line[i] = IP_buffer_ptr_U_4x4.rec_line_hor[i + 4];
				IP_buffer_ptr_V_4x4.up_line[i] = IP_buffer_ptr_V_4x4.rec_line_hor[i + 4];
				IP_buffer_ptr_U_4x4.left_line[i] = IP_buffer_ptr_U_4x4.rec_line_ver[i + 4];
				IP_buffer_ptr_V_4x4.left_line[i] = IP_buffer_ptr_V_4x4.rec_line_ver[i + 4];

			}
			IP_buffer_ptr_U_4x4.top_left = IP_buffer_ptr_U_4x4.rec_line_hor[3];
			IP_buffer_ptr_V_4x4.top_left = IP_buffer_ptr_V_4x4.rec_line_hor[3];
			/// </chroma ip buf>
			break;
		default:
			assert(0);
		}
		IP_map_scu_ptr_8x8.fetch_map_scu_qt(part_num, IP_map_scu_ptr_16x16);

		md_chroma_4x4_chroma(md_fw_ptr,
			&cu_data_best_c,
			x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
			md_input_ptr,
			p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
			RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
			1, cup, NO_MODE_CONS,
			&IP_buffer_ptr_U_4x4,
			&IP_buffer_ptr_V_4x4,
			Fetch_Ref_window_ptr,
			&IP_buffer_ptr_8x8,
			&IP_map_scu_ptr_8x8
		);

		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 4; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_ptr_U_4x4.rec_line_hor[i] = cu_data_best_c.reco_u/*[i + 12]*/[3][i];
				IP_buffer_ptr_V_4x4.rec_line_hor[i] = cu_data_best_c.reco_v/*[i + 12]*/[3][i];
				IP_buffer_ptr_U_4x4.rec_line_ver[i] = cu_data_best_c.reco_u/*[i * 4 + 3]*/[i][3];
				IP_buffer_ptr_V_4x4.rec_line_ver[i] = cu_data_best_c.reco_v/*[i * 4 + 3]*/[i][3];
			}
			break;
		case 1:
			for (int i = 0; i < 4; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_ptr_U_4x4.rec_line_hor[i + 4] = cu_data_best_c.reco_u[3][i];
				IP_buffer_ptr_V_4x4.rec_line_hor[i + 4] = cu_data_best_c.reco_v[3][i];
			}
			break;
		case 2:
			for (int i = 0; i < 4; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_ptr_U_4x4.rec_line_ver[i + 4] = cu_data_best_c.reco_u[i][3];
				IP_buffer_ptr_V_4x4.rec_line_ver[i + 4] = cu_data_best_c.reco_v[i][3];
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
		IP_map_scu_ptr_8x8.update_map_scu_qt(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_best_c.map_scu);
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.coef_u cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.coef_v cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.reco_u cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.reco_v cyclic factor=4 dim=2
//#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.ipm cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.map_scu cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.num_nz_coef cyclic factor=4 dim=2
		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);

		//store_enc_info_best_chroma(md_fw_ptr, &cu_data_temp_NS, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}

static s64 md_8x8_loop_chroma(U1 btv_allowed,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U3 split_mode, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTV[1], U2 tree_status, U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (btv_allowed) {
		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);
		prepare_cu_info_COM_combine_only_qt_8_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cu_width, cu_height, SPLIT_QUAD, &split_struct);

		for (int i = 0; i < (cu_height >> 2); i++)
			for (int j = 0; j < (cu_width >> 2); j++)
				IP_map_scu_ptr_16x16->cur_map_scu[j][i] = 0;

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_8x8_top_chroma(
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth + 1, bet_depth, cons_pred_mode_BTV[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_16x16,
				IP_buffer_ptr_U_8x8,
				IP_buffer_ptr_V_8x8,
				IP_map_scu_ptr_16x16,
				Fetch_Ref_window_ptr, SPLIT_QUAD);
		}
	}
	return 1;
}

s64 circuit_16x16_top_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* prev_cu_data_temp_ptr,
	U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_U_16x16,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_V_16x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
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
	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
	//allowed split by normative and non-normative selection
	U3 num_split_to_try = split_struct.num_split[part_num];

	s64 cost[4] = { MAX_S64, MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[4] = { TREE_LC, TREE_LC, TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode[1] = { NO_SPLIT };
	U2 best_cons_pred_mode[1] = { NO_MODE_CONS };
	s64 cost_temp;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child[1] = { TREE_LC };

	U2 NS_luma_pred_mode[1] = { ONLY_INTRA };
	U2 tree_status_temp[1] = { tree_status };
	U3 split_mode[4] = { NO_SPLIT, SPLIT_BI_VER, SPLIT_BI_HOR, SPLIT_QUAD };

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma])
	{
		assert(cu_width == 16 && cu_height == 16 && split_mode_father == SPLIT_QUAD);
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_16x8;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_16x16;
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_U_8x8;
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_V_8x8;
		static IP_MAP_SCU_SIZE<8, 16>				IP_map_scu_8x16;
		static IP_MAP_SCU_SIZE<16, 8>				IP_map_scu_16x8;
		static IP_MAP_SCU_SIZE<16, 16>				IP_map_scu_16x16;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_temp_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		if (tree_status == TREE_LC) {
			cu_data_temp_c.x_chroma = x0;
			cu_data_temp_c.y_chroma = y0;
			cu_data_temp_c.x_last = x0 + cu_width - 4;
			cu_data_temp_c.y_last = y0 + cu_height - 4;
			cu_data_temp_c.last_child = FALSE;
		}
		else {
			cu_data_temp_c.x_chroma = prev_cu_data_temp_ptr->x_chroma;
			cu_data_temp_c.y_chroma = prev_cu_data_temp_ptr->y_chroma;
			cu_data_temp_c.x_last = prev_cu_data_temp_ptr->x_last;
			cu_data_temp_c.y_last = prev_cu_data_temp_ptr->y_last;
			cu_data_temp_c.last_child = x0 + cu_width == prev_cu_data_temp_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_temp_ptr->y_last + 4;
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_16x16.up_line[i] = IP_buffer_ptr_32x32->up_line[i];
				IP_buffer_16x16.left_line[i] = IP_buffer_ptr_32x32->left_line[i];
			}
			IP_buffer_16x16.top_left = IP_buffer_ptr_32x32->top_left;
#if CDEBUG
			IP_buffer_ptr->get_left_line(IP_buffer_16x16.left_line, IP_buffer_ptr, x_pos_lcu, y_pos_lcu, cu_height, ip_array_size_idx, 1);
			IP_buffer_ptr->get_top_left(&IP_buffer_16x16.top_left, IP_buffer_ptr, x_pos_lcu, y_pos_lcu, ip_array_size_idx, 1);
#endif // CDEBUG

			for (int i = 0; i < (32 >> 1); i++)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.up_line[i] = IP_buffer_ptr_U_16x16->up_line[i];
				IP_buffer_V_8x8.up_line[i] = IP_buffer_ptr_V_16x16->up_line[i];
				IP_buffer_U_8x8.left_line[i] = IP_buffer_ptr_U_16x16->left_line[i];
				IP_buffer_V_8x8.left_line[i] = IP_buffer_ptr_V_16x16->left_line[i];
			}
			IP_buffer_U_8x8.top_left = IP_buffer_ptr_U_16x16->top_left;
			IP_buffer_V_8x8.top_left = IP_buffer_ptr_V_16x16->top_left;

			break;
		case 1:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_16x16.up_line[i] = IP_buffer_ptr_32x32->up_line[i + 16];
				//IP_buffer_16x16.left_line[i] = IP_buffer_16x16.rec_line_ver[i];
				IP_buffer_16x16.left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
			}
			IP_buffer_16x16.top_left = IP_buffer_ptr_32x32->up_line[15];

			for (int i = 0; i < (32 >> 1); ++i)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.up_line[i] = IP_buffer_ptr_U_16x16->up_line[i + (16 >> 1)];
				IP_buffer_V_8x8.up_line[i] = IP_buffer_ptr_V_16x16->up_line[i + (16 >> 1)];
				IP_buffer_U_8x8.left_line[i] = IP_buffer_U_8x8.rec_line_ver[i];
				IP_buffer_V_8x8.left_line[i] = IP_buffer_V_8x8.rec_line_ver[i];
			}
			IP_buffer_U_8x8.top_left = IP_buffer_ptr_U_16x16->up_line[7];
			IP_buffer_V_8x8.top_left = IP_buffer_ptr_V_16x16->up_line[7];

			break;
		case 2:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_16x16.up_line[i] = IP_buffer_16x16.rec_line_hor[i];
				IP_buffer_16x16.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu];
			}
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_16x16.left_line[i] = IP_buffer_ptr_32x32->left_line[i + 16];
			}
			IP_buffer_16x16.top_left = IP_buffer_ptr_32x32->left_line[15];
#if CDEBUG
			IP_buffer_ptr->get_left_line(IP_buffer_16x16.left_line, IP_buffer_ptr, x_pos_lcu, y_pos_lcu, cu_height, ip_array_size_idx, 1);
			IP_buffer_ptr->get_top_left(&IP_buffer_16x16.top_left, IP_buffer_ptr, x_pos_lcu, y_pos_lcu, ip_array_size_idx, 1);
#endif // CDEBUG

			for (int i = 0; i < (32 >> 1); ++i)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.up_line[i] = IP_buffer_U_8x8.rec_line_hor[i];
				IP_buffer_V_8x8.up_line[i] = IP_buffer_V_8x8.rec_line_hor[i];
				IP_buffer_U_8x8.left_line[i] = IP_buffer_ptr_U_16x16->left_line[i + 8];
				IP_buffer_V_8x8.left_line[i] = IP_buffer_ptr_V_16x16->left_line[i + 8];
			}
			IP_buffer_U_8x8.top_left = IP_buffer_ptr_U_16x16->left_line[7];
			IP_buffer_V_8x8.top_left = IP_buffer_ptr_V_16x16->left_line[7];

			break;
		case 3:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_16x16.up_line[i] = IP_buffer_16x16.rec_line_hor[i + 16];
				IP_buffer_16x16.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu];
				//IP_buffer_16x16.left_line[i] = IP_buffer_16x16.rec_line_ver[i + 16];
				IP_buffer_16x16.left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
			}
			IP_buffer_16x16.top_left = md_input_ptr->reco_y[y_pos_lcu - 1][x_pos_lcu - 1];
			//IP_buffer_16x16.top_left = IP_buffer_16x16.rec_line_hor[15];

			for (int i = 0; i < 8; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.up_line[i] = IP_buffer_U_8x8.rec_line_hor[i + 8];
				IP_buffer_V_8x8.up_line[i] = IP_buffer_V_8x8.rec_line_hor[i + 8];
				IP_buffer_U_8x8.left_line[i] = IP_buffer_U_8x8.rec_line_ver[i + 8];
				IP_buffer_V_8x8.left_line[i] = IP_buffer_V_8x8.rec_line_ver[i + 8];
			}
			IP_buffer_U_8x8.top_left = IP_buffer_U_8x8.rec_line_hor[7];
			IP_buffer_V_8x8.top_left = IP_buffer_V_8x8.rec_line_hor[7];

			break;
		default:
			assert(0);
		}
		IP_map_scu_16x16.fetch_map_scu_qt(part_num, IP_map_scu_ptr_32x32);
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_16x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 4);
			int flag2 = (x0 > 0 && ((IP_map_scu_16x8.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 3);
			int flag3 = (x0 > 0 && ((IP_map_scu_8x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 3) && (cu_height_log2 == 4);
			//if (flag1 || flag2 || flag3)
				//md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x8.map_scu_left_line_new[0]) >> 16) & 0x7F;

			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_8x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma != SPLIT_QUAD && best_split_mode_luma != NO_SPLIT)
		{
#if !ONLY_NS_16X16
			md_16x16_16x8_8x16_loop_md1_chroma(best_split_mode_luma,
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode[0],
				&cu_data_temp_c,
				&cons_pred_mode_child[0], tree_status, &tree_status_child[0],
				&IP_buffer_16x16,
				&IP_buffer_U_8x8, &IP_buffer_V_8x8,
				&IP_map_scu_16x16,
				Fetch_Ref_window_ptr);

			best_split_mode[0] = best_split_mode_luma;

			copy_cu_data_array_src_chroma<8>(&cu_data_best_c, &cu_data_temp_c, 0, 0,
				cu_width_log2, cu_height_log2, cu_width_log2);

			best_cons_pred_mode[0] = cons_pred_mode_child[0];
			best_tree_status_child[0] = tree_status_child[0];

			if (*best_split_mode != NO_SPLIT)
			{
				*tree_status_temp = TREE_C;
			}
#endif
		}
		else if (best_split_mode_luma == SPLIT_QUAD)
		{
#if !ONLY_NS_16X16
			md_8x8_loop_chroma(split_struct.split_allow[part_num][SPLIT_QUAD],
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				best_split_mode_luma, x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode[0],
				&cu_data_temp_c,
				&cons_pred_mode_child[0], tree_status, &tree_status_child[0],
				&IP_buffer_16x16,
				&IP_buffer_U_8x8, &IP_buffer_V_8x8,
				&IP_map_scu_16x16,
				Fetch_Ref_window_ptr);

			best_split_mode[0] = best_split_mode_luma;

			copy_cu_data_array_src_chroma<8>(&cu_data_best_c, &cu_data_temp_c, 0, 0,
				cu_width_log2, cu_height_log2, cu_width_log2);

			best_cons_pred_mode[0] = cons_pred_mode_child[0];
			best_tree_status_child[0] = tree_status_child[0];

			if (*best_split_mode != NO_SPLIT)
			{
				*tree_status_temp = TREE_C;
			}
#endif
		}
		else
		{
#if !IP_BUFFER_NULL
			IP_ARRAY_SIZE<4, 8> IP_buffer_ptr_U_4x8;	// liaozhx, HLS. NULL pointer is not allowed
			IP_ARRAY_SIZE<4, 8> IP_buffer_ptr_V_4x8;
			IP_ARRAY_SIZE<8, 4> IP_buffer_ptr_U_8x4;
			IP_ARRAY_SIZE<8, 4> IP_buffer_ptr_V_8x4;
#endif // !IP_BUFFER_NULL
#if 0 //for clean_test pass
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	//IP_buffer_ptr_U_4x8.top_left = zero;
	//IP_buffer_ptr_U_8x4.top_left = zero;
	//IP_buffer_ptr_V_4x8.top_left = zero;
	//IP_buffer_ptr_V_8x4.top_left = zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_ptr_U_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_U_8x4.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_8x4.top_left +=  (zero_1b << (16 - 1));	
#endif
			md_chroma_8(8, 8, md_fw_ptr,
				&cu_data_best_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp[0], cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child[0] == TREE_L && tree_status == TREE_LC) || best_split_mode[0] == NO_SPLIT), cup, cons_pred_mode,
				&IP_buffer_U_8x8, &IP_buffer_V_8x8,
#if IP_BUFFER_NULL
				NULL, NULL,
				NULL, NULL,
#else
				&IP_buffer_ptr_U_4x8, &IP_buffer_ptr_V_4x8, //NULL, NULL,
				&IP_buffer_ptr_U_8x4, &IP_buffer_ptr_V_8x4, //NULL, NULL,
#endif // IP_BUFFER_NULL
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16,
				&IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}

		int scu_h = (y0 + 16 > md_fw_ptr->pic_height ? ((int)(md_fw_ptr->pic_height - y0) >> 2) : (16 >> 2));
		int scu_w = (x0 + 16 > md_fw_ptr->pic_width ? ((int)(md_fw_ptr->pic_width - x0) >> 2) : (16 >> 2));
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_16x16.rec_line_hor[i] = md_input_ptr->reco_y[i + 240];
				//IP_buffer_16x16.rec_line_ver[i] = md_input_ptr->reco_y[i * 16 + 15];
				//if (i + x_pos_lcu + y_pos_lcu * 64 - 64 >= 0)
				IP_buffer_16x16.rec_line_hor[i] = md_input_ptr->reco_y[y_pos_lcu + (cu_height - 1)][i + x_pos_lcu];
				//if (64 * i + x_pos_lcu + y_pos_lcu * 64 - 1 >= 0)
				IP_buffer_16x16.rec_line_ver[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu + cu_width - 1];
			}
			for (int i = 0; i < 8; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.rec_line_hor[i] = cu_data_best_c.reco_u[7][i];
				IP_buffer_V_8x8.rec_line_hor[i] = cu_data_best_c.reco_v[7][i];
				IP_buffer_U_8x8.rec_line_ver[i] = cu_data_best_c.reco_u[i][7];
				IP_buffer_V_8x8.rec_line_ver[i] = cu_data_best_c.reco_v[i][7];
			}
			break;
		case 1:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_16x16.rec_line_hor[i + 16] = md_input_ptr->reco_y[i + 240];
				IP_buffer_16x16.rec_line_hor[i + 16] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu ];
			}
			for (int i = 0; i < 8; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.rec_line_hor[i + 8] = cu_data_best_c.reco_u[7][i];
				IP_buffer_V_8x8.rec_line_hor[i + 8] = cu_data_best_c.reco_v[7][i];
			}
			break;
		case 2:
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS UNROLL
				//IP_buffer_16x16.rec_line_ver[i + 16] = md_input_ptr->reco_y[i * 16 + 15];
				//if (64 * i + x_pos_lcu + y_pos_lcu * 64 - 1 >= 0)
				IP_buffer_16x16.rec_line_ver[i + 16] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu + cu_width - 1];
			}
			for (int i = 0; i < 8; ++i)
			{
#pragma HLS UNROLL
				IP_buffer_U_8x8.rec_line_ver[i + 8] = cu_data_best_c.reco_u[i][7];
				IP_buffer_V_8x8.rec_line_ver[i + 8] = cu_data_best_c.reco_v[i][7];
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
		IP_map_scu_16x16.update_map_scu_qt(part_num, IP_map_scu_ptr_32x32->cur_map_scu, cu_data_best_c.map_scu);
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.coef_u cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.coef_v cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.reco_u cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.reco_v cyclic factor=8 dim=2
//#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.ipm cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.map_scu cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_best_c.num_nz_coef cyclic factor=4 dim=2
		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, best_split_mode, best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}

s64 prepare_cu_info_COM_combine_only_qt_chroma(MD_FW* md_fw_ptr, s64 lambdaY,
	U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct)
{
	com_split_get_part_structure_only_qt(x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode_only_qt(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], bet_depth, &split_struct->num_split[i]);
	}

	//clear_map_scu_chroma(me_line_map_ptr, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width,
	//	md_fw_ptr->pic_height);

	return 0;
}


s64 md_16x16_loop_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_QU[1], U2 child_tree_status[1],
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_U_16x16,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_V_16x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bqu_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (isBTH_allow) {
		child_tree_status[0] = TREE_LC;
		prepare_cu_info_COM_combine_only_qt_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth,
			cu_width, cu_height, SPLIT_QUAD, &split_struct);

		for (int i = 0; i < (cu_height >> 2); i++)
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			for (int j = 0; j < (cu_width >> 2); j++)
#pragma HLS LOOP_TRIPCOUNT max=16
				IP_map_scu_ptr_32x32->cur_map_scu[j][i] = 0;

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=4
			circuit_16x16_top_chroma(md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth + 1, bet_depth, cons_pred_mode_QU[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_32x32,
				IP_buffer_ptr_U_16x16,
				IP_buffer_ptr_V_16x16,
				IP_map_scu_ptr_32x32,
				Fetch_Ref_window_ptr, SPLIT_QUAD);
		}
	}
	return 1;
}

#if !CIRCUIT32_CHROMA_DUMMY
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

	static ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>	cu_data_best_c;
	s64 cost[4] = { MAX_S64, MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[4] = { TREE_LC, TREE_LC, TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[4] = { NO_MODE_CONS, NO_MODE_CONS, NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;
	U3 split_mode[4] = { NO_SPLIT, SPLIT_BI_VER, SPLIT_BI_HOR, SPLIT_QUAD };
	U2 best_index;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma]) {

		if (best_split_mode_luma == NO_SPLIT)
			best_index = 0;

		if (best_split_mode_luma == SPLIT_BI_VER)
			best_index = 1;

		if (best_split_mode_luma == SPLIT_BI_HOR)
			best_index = 2;

		if (best_split_mode_luma == SPLIT_QUAD)
			best_index = 3;

		assert(cu_width == 32 && cu_height == 32 && split_mode_father == SPLIT_QUAD);
		static IP_ARRAY_SIZE<16, 32>				IP_buffer_16x32;
		static IP_ARRAY_SIZE<32, 16>				IP_buffer_32x16;
		static IP_MAP_SCU_SIZE<16, 32>				IP_map_scu_16x32;
		static IP_MAP_SCU_SIZE<32, 16>				IP_map_scu_32x16;
		static IP_ARRAY_SIZE<32, 32>				IP_buffer_32x32;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_U_16x16;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_V_16x16;
		static IP_MAP_SCU_SIZE<32, 32>				IP_map_scu_32x32;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>	cu_data_temp_c[4];
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		for (int i = 0; i < 4; i++)
		{
			cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_temp_c[i], prev_cu_data_temp_ptr, qt_depth + bet_depth);
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_16x16.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_16x16.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_16x16.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_16x16.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_16x16.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_16x16.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 64; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_32x32.up_line[i] = IP_buffer_ptr_64x64->up_line[i];
				IP_buffer_32x32.left_line[i] = IP_buffer_ptr_64x64->left_line[i];
			}
			IP_buffer_32x32.top_left = IP_buffer_ptr_64x64->top_left;

			for (int i = 0; i < (64 >> 1); i++)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.up_line[i] = IP_buffer_ptr_U_32x32->up_line[i];
				IP_buffer_V_16x16.up_line[i] = IP_buffer_ptr_V_32x32->up_line[i];
				IP_buffer_U_16x16.left_line[i] = IP_buffer_ptr_U_32x32->left_line[i];
				IP_buffer_V_16x16.left_line[i] = IP_buffer_ptr_V_32x32->left_line[i];
			}
			IP_buffer_U_16x16.top_left = IP_buffer_ptr_U_32x32->top_left;
			IP_buffer_V_16x16.top_left = IP_buffer_ptr_V_32x32->top_left;
			break;
		case 1:
			for (int i = 0; i < 64; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_32x32.up_line[i] = IP_buffer_ptr_64x64->up_line[i + 32];
			}
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS PIPELINE
				//IP_buffer_32x32.left_line[i] = IP_buffer_32x32.rec_line_ver[i];
				IP_buffer_32x32.left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
			}
			//IP_buffer_ptr->get_top_left(&IP_buffer_32x32.top_left, IP_buffer_ptr, x_pos_lcu, y_pos_lcu, ip_array_size_idx);
			IP_buffer_32x32.top_left = IP_buffer_ptr_64x64->up_line[31];

			for (int i = 0; i < (64 >> 1); ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.up_line[i] = IP_buffer_ptr_U_32x32->up_line[i + (32 >> 1)];
				IP_buffer_V_16x16.up_line[i] = IP_buffer_ptr_V_32x32->up_line[i + (32 >> 1)];
				IP_buffer_U_16x16.left_line[i] = IP_buffer_U_16x16.rec_line_ver[i];
				IP_buffer_V_16x16.left_line[i] = IP_buffer_V_16x16.rec_line_ver[i];
			}
			IP_buffer_U_16x16.top_left = IP_buffer_ptr_U_32x32->up_line[15];
			IP_buffer_V_16x16.top_left = IP_buffer_ptr_V_32x32->up_line[15];

			break;
		case 2:
			for (int i = 0; i < 64; ++i)
			{
#pragma HLS PIPELINE
				//IP_buffer_32x32.up_line[i] = IP_buffer_32x32.rec_line_hor[i];
				IP_buffer_32x32.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu ];
			}
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_32x32.left_line[i] = IP_buffer_ptr_64x64->left_line[i + 32];
			}
			IP_buffer_32x32.top_left = IP_buffer_ptr_64x64->left_line[31];

			for (int i = 0; i < (64 >> 1); ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.up_line[i] = IP_buffer_U_16x16.rec_line_hor[i];
				IP_buffer_V_16x16.up_line[i] = IP_buffer_V_16x16.rec_line_hor[i];
				IP_buffer_U_16x16.left_line[i] = IP_buffer_ptr_U_32x32->left_line[i + 16];
				IP_buffer_V_16x16.left_line[i] = IP_buffer_ptr_V_32x32->left_line[i + 16];
			}
			IP_buffer_U_16x16.top_left = IP_buffer_ptr_U_32x32->left_line[15];
			IP_buffer_V_16x16.top_left = IP_buffer_ptr_V_32x32->left_line[15];
			break;
		case 3:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS PIPELINE
				//IP_buffer_32x32.up_line[i] = IP_buffer_32x32.rec_line_hor[i + 32];
				//IP_buffer_32x32.left_line[i] = IP_buffer_32x32.rec_line_ver[i + 32];
				IP_buffer_32x32.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu];
				IP_buffer_32x32.left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
			}
			IP_buffer_32x32.top_left = IP_buffer_32x32.rec_line_ver[31];

			for (int i = 0; i < 16; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.up_line[i] = IP_buffer_U_16x16.rec_line_hor[i + 16];
				IP_buffer_V_16x16.up_line[i] = IP_buffer_V_16x16.rec_line_hor[i + 16];
				IP_buffer_U_16x16.left_line[i] = IP_buffer_U_16x16.rec_line_ver[i + 16];
				IP_buffer_V_16x16.left_line[i] = IP_buffer_V_16x16.rec_line_ver[i + 16];

			}
			IP_buffer_U_16x16.top_left = IP_buffer_U_16x16.rec_line_hor[15];
			IP_buffer_V_16x16.top_left = IP_buffer_V_16x16.rec_line_hor[15];
			break;
		default:
			assert(0);
		}
		IP_map_scu_32x32.fetch_map_scu_qt(part_num, IP_map_scu_ptr_64x64);
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_32x32.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 5) && (cu_height_log2 == 5);

			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_32x32.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma == NO_SPLIT)
		{
			best_split_mode = best_split_mode_luma;
			best_cons_pred_mode = cons_pred_mode_child[best_index];
			best_tree_status_child = tree_status_child[best_index];
#if !ONLY_QT_QT
#ifndef MD_CMOD
			md_chroma_16_chroma(md_fw_ptr,
				&cu_data_best_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cons_pred_mode,
				&IP_buffer_U_16x16, &IP_buffer_V_16x16,
				NULL, NULL,
				NULL, NULL,
				Fetch_Ref_window_ptr,
				&IP_buffer_32x32, &IP_buffer_16x32, &IP_buffer_32x16,
				&IP_map_scu_32x32, &IP_map_scu_16x32, &IP_map_scu_32x16);
#endif
#endif
		}
		else if (best_split_mode_luma != SPLIT_QUAD)
		{
#if !ONLY_QT_QT
			md_32x32_32x16_16x32_loop_md1_chroma(best_split_mode_luma,
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c[best_index],
				&cons_pred_mode_child[best_index], &tree_status_child[best_index],
				&IP_buffer_32x32,
				&IP_buffer_U_16x16,
				&IP_buffer_V_16x16,
				&IP_map_scu_32x32,
				Fetch_Ref_window_ptr);
#endif

			copy_cu_data_array_src_chroma<16>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}
		else {
			md_16x16_loop_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c[best_index],
				&cons_pred_mode_child[best_index], &tree_status_child[best_index],
				&IP_buffer_32x32,
				&IP_buffer_U_16x16,
				&IP_buffer_V_16x16,
				&IP_map_scu_32x32,
				Fetch_Ref_window_ptr);

			copy_cu_data_array_src_chroma<16>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}

		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS PIPELINE
				//IP_buffer_32x32.rec_line_ver[i] = md_input_ptr->reco_y[i * 32 + 31];
				//IP_buffer_32x32.rec_line_hor[i] = md_input_ptr->reco_y[992 + i];
				//if (64 * i + x_pos_lcu + y_pos_lcu * 64 - 1 >= 0)
				IP_buffer_32x32.rec_line_ver[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu + cu_width - 1];
				//if (i + x_pos_lcu + y_pos_lcu * 64 - 64 >= 0)
				IP_buffer_32x32.rec_line_hor[i] = md_input_ptr->reco_y[y_pos_lcu + (cu_height - 1)][i + x_pos_lcu];
			}
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.rec_line_hor[i] = cu_data_best_c.reco_u[15][i];
				IP_buffer_V_16x16.rec_line_hor[i] = cu_data_best_c.reco_v[15][i];
				IP_buffer_U_16x16.rec_line_ver[i] = cu_data_best_c.reco_u[i][15];
				IP_buffer_V_16x16.rec_line_ver[i] = cu_data_best_c.reco_v[i][15];
			}
			break;
		case 1:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS PIPELINE
				//IP_buffer_32x32.rec_line_hor[i + 32] = md_input_ptr->reco_y[992 + i];
				//if (i + x_pos_lcu + y_pos_lcu * 64 - 64 >= 0)
				IP_buffer_32x32.rec_line_hor[i + 32] = md_input_ptr->reco_y[y_pos_lcu + (cu_height - 1)][i + x_pos_lcu ];
			}
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.rec_line_hor[i + 16] = cu_data_best_c.reco_u[15][i];
				IP_buffer_V_16x16.rec_line_hor[i + 16] = cu_data_best_c.reco_v[15][i];
			}
			break;
		case 2:
			for (int i = 0; i < 32; ++i)
			{
#pragma HLS PIPELINE
				//IP_buffer_32x32.rec_line_ver[i + 32] = md_input_ptr->reco_y[i * 32 + 31];
				IP_buffer_32x32.rec_line_ver[i + 32] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
			}
			for (int i = 0; i < 16; ++i)
			{
#pragma HLS PIPELINE
				IP_buffer_U_16x16.rec_line_ver[i + 16] = cu_data_best_c.reco_u[i][15];
				IP_buffer_V_16x16.rec_line_ver[i + 16] = cu_data_best_c.reco_v[i][15];
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
		IP_map_scu_32x32.update_map_scu_qt(part_num, IP_map_scu_ptr_64x64->cur_map_scu, cu_data_best_c.map_scu);
	}

	prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
	//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
	//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);

	return 1;
}
#endif

static s64 circuit_16x16_16x8_8x16_md2_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* prev_cu_data_temp_ptr,
	U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8, IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode)
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

	/*************************for parallel**********************/
	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };
	U2 tree_status_child[1] = { TREE_LC };

	U3 best_split_mode_luma = NO_SPLIT;
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

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma]) {
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_16x8;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_16x16;
		static IP_ARRAY_SIZE<4, 8>					IP_buffer_U_4x8;
		static IP_ARRAY_SIZE<4, 8>					IP_buffer_V_4x8;
		static IP_ARRAY_SIZE<8, 4>					IP_buffer_U_8x4;
		static IP_ARRAY_SIZE<8, 4>					IP_buffer_V_8x4;
		static IP_MAP_SCU_SIZE<8, 16>				IP_map_scu_8x16;
		static IP_MAP_SCU_SIZE<16, 8>				IP_map_scu_16x8;
		static IP_MAP_SCU_SIZE<16, 16>				IP_map_scu_16x16;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_temp_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		if (tree_status == TREE_LC) {
			cu_data_temp_c.x_chroma = x0;
			cu_data_temp_c.y_chroma = y0;
			cu_data_temp_c.x_last = x0 + cu_width - 4;
			cu_data_temp_c.y_last = y0 + cu_height - 4;
			cu_data_temp_c.last_child = FALSE;
		}
		else {
			cu_data_temp_c.x_chroma = prev_cu_data_temp_ptr->x_chroma;
			cu_data_temp_c.y_chroma = prev_cu_data_temp_ptr->y_chroma;
			cu_data_temp_c.x_last = prev_cu_data_temp_ptr->x_last;
			cu_data_temp_c.y_last = prev_cu_data_temp_ptr->y_last;
			cu_data_temp_c.last_child = x0 + cu_width == prev_cu_data_temp_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_temp_ptr->y_last + 4;
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_4x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_4x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_4x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_4x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_4x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_4x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x4.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x4.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x4.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x4.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x4.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x4.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (split_mode)
		{
		case SPLIT_BI_VER:
			assert(cu_width == 8 && cu_height == 16);
			IP_buffer_8x16.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_16x16, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_4x8.fetch_bi_ver(part_num, IP_buffer_ptr_U_8x8);
			IP_buffer_V_4x8.fetch_bi_ver(part_num, IP_buffer_ptr_V_8x8);
			IP_map_scu_8x16.fetch_bi_ver(part_num, IP_map_scu_ptr_16x16);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 16 && cu_height == 8);
			IP_buffer_16x8.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_16x16, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x4.fetch_bi_hor(part_num, IP_buffer_ptr_U_8x8);
			IP_buffer_V_8x4.fetch_bi_hor(part_num, IP_buffer_ptr_V_8x8);
			IP_map_scu_16x8.fetch_bi_hor(part_num, IP_map_scu_ptr_16x16);
			break;
		default: assert(0);
		}

		best_split_mode = best_split_mode_luma;


		best_cons_pred_mode = cons_pred_mode_child[0];
		best_tree_status_child = tree_status_child[0];

		if (best_split_mode != NO_SPLIT)
		{
			tree_status_temp = TREE_C;
		}
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_ptr_16x16->map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 4);
			int flag2 = (x0 > 0 && ((IP_map_scu_16x8.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 3);
			int flag3 = (x0 > 0 && ((IP_map_scu_8x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 3) && (cu_height_log2 == 4);
			//if (flag1 || flag2 || flag3)
				//md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_ptr_16x16->map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x8.map_scu_left_line_new[0]) >> 16) & 0x7F;

			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_8x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
#if 1 //for clean_test pass
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	IP_buffer_U_4x8.top_left += zero;
	IP_buffer_U_8x4.top_left += zero;
	IP_buffer_V_4x8.top_left += zero;
	IP_buffer_V_8x4.top_left += zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_U_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_U_8x4.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_V_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_V_8x4.top_left +=  (zero_1b << (16 - 1));	
#endif
		if (cu_width == 8)
		{
			md_chroma_8(4, 8, md_fw_ptr,
				&cu_data_temp_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
#if IP_BUFFER_NULL
				NULL, NULL,
#else
				IP_buffer_ptr_U_8x8, IP_buffer_ptr_V_8x8, //NULL, NULL,
#endif 
				&IP_buffer_U_4x8, &IP_buffer_V_4x8,
				&IP_buffer_U_8x4, &IP_buffer_V_8x4,
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16,
				&IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}
		else if (cu_height == 8)
		{
			md_chroma_8(8, 4, md_fw_ptr,
				&cu_data_temp_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
#if IP_BUFFER_NULL
				NULL, NULL,
#else
				IP_buffer_ptr_U_8x8, IP_buffer_ptr_V_8x8, //NULL, NULL,
#endif
				&IP_buffer_U_4x8, &IP_buffer_V_4x8,
				&IP_buffer_U_8x4, &IP_buffer_V_8x4,
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16, &IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}

		switch (split_mode)
		{
		case SPLIT_BI_VER:
			assert(cu_width == 8 && cu_height == 16);
			IP_buffer_U_4x8.update_bi_ver(part_num, cu_data_temp_c.reco_u);
			IP_buffer_V_4x8.update_bi_ver(part_num, cu_data_temp_c.reco_v);
			IP_map_scu_8x16.update_bi_ver(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_temp_c.map_scu);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 16 && cu_height == 8);
			IP_buffer_U_8x4.update_bi_hor(part_num, cu_data_temp_c.reco_u);
			IP_buffer_V_8x4.update_bi_hor(part_num, cu_data_temp_c.reco_v);
			IP_map_scu_16x8.update_bi_hor(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_temp_c.map_scu);
			break;
		default: assert(0);
		}

		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_temp_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}

static s64 md_16x16_16x8_8x16_loop_md2_chroma(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8, IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (isBTH_allow) {

		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);

		prepare_cu_info_16_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud, qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_16x16_16x8_8x16_md2_chroma(
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_16x16,
				IP_buffer_ptr_U_8x8, IP_buffer_ptr_V_8x8,
				IP_map_scu_ptr_16x16,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return 1;
}

s64 circuit_16x16_top_md2_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num,
	COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_U_8x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_V_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_U_16x8, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_V_16x8,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
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
	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
	//allowed split by normative and non-normative selection
	U3 num_split_to_try = split_struct.num_split[part_num];

	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[3] = { TREE_LC };
	U2 cons_pred_mode_child[3] = { NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_temp;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;
	U3 split_mode[3] = { NO_SPLIT, SPLIT_BI_VER, SPLIT_BI_HOR };

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}


	if (split_struct.split_allow[part_num][best_split_mode_luma])
	{
		assert(cu_width == 16 && cu_height == 16);
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_16x8;
		static IP_ARRAY_SIZE<16, 16>				IP_buffer_16x16;
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_U_8x8;
		static IP_ARRAY_SIZE<8, 8>					IP_buffer_V_8x8;
		static IP_MAP_SCU_SIZE<8, 16>				IP_map_scu_8x16;
		static IP_MAP_SCU_SIZE<16, 8>				IP_map_scu_16x8;
		static IP_MAP_SCU_SIZE<16, 16>				IP_map_scu_16x16;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<16>	cu_data_temp_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		if (tree_status == TREE_LC) {
			cu_data_temp_c.x_chroma = x0;
			cu_data_temp_c.y_chroma = y0;
			cu_data_temp_c.x_last = x0 + cu_width - 4;
			cu_data_temp_c.y_last = y0 + cu_height - 4;
			cu_data_temp_c.last_child = FALSE;
		}
		else {
			cu_data_temp_c.x_chroma = prev_cu_data_temp_ptr->x_chroma;
			cu_data_temp_c.y_chroma = prev_cu_data_temp_ptr->y_chroma;
			cu_data_temp_c.x_last = prev_cu_data_temp_ptr->x_last;
			cu_data_temp_c.y_last = prev_cu_data_temp_ptr->y_last;
			cu_data_temp_c.last_child = x0 + cu_width == prev_cu_data_temp_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_temp_ptr->y_last + 4;
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (split_mode_father)
		{
		case SPLIT_BI_HOR:
			IP_buffer_16x16.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_16x32, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x8.fetch_bi_hor(part_num, IP_buffer_ptr_U_8x16);
			IP_buffer_V_8x8.fetch_bi_hor(part_num, IP_buffer_ptr_V_8x16);
			IP_map_scu_16x16.fetch_bi_hor(part_num, IP_map_scu_ptr_16x32);
			break;
		case SPLIT_BI_VER:
			IP_buffer_16x16.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_32x16, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x8.fetch_bi_ver(part_num, IP_buffer_ptr_U_16x8);
			IP_buffer_V_8x8.fetch_bi_ver(part_num, IP_buffer_ptr_V_16x8);
			IP_map_scu_16x16.fetch_bi_ver(part_num, IP_map_scu_ptr_32x16);
			break;
		default: assert(0);
		}
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_16x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 4);
			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma == NO_SPLIT)
		{
			best_split_mode = best_split_mode_luma;

			best_cons_pred_mode = cons_pred_mode_child[0];
			best_tree_status_child = tree_status_child[0];

			if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}
#if !IP_BUFFER_NULL
			IP_ARRAY_SIZE<4, 8> IP_buffer_ptr_U_4x8;	// liaozhx, HLS. NULL pointer is not allowed
			IP_ARRAY_SIZE<4, 8> IP_buffer_ptr_V_4x8;
			IP_ARRAY_SIZE<8, 4> IP_buffer_ptr_U_8x4;
			IP_ARRAY_SIZE<8, 4> IP_buffer_ptr_V_8x4;
#endif
#if 0 //for clean_test pass
    U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	//IP_buffer_ptr_U_4x8.top_left = zero;
	//IP_buffer_ptr_U_8x4.top_left = zero;
	//IP_buffer_ptr_V_4x8.top_left = zero;
	//IP_buffer_ptr_V_8x4.top_left = zero;	
	U1 zero_1b = zero & 1;
	IP_buffer_ptr_U_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_U_8x4.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_4x8.top_left +=  (zero_1b << (16 - 1));
	IP_buffer_ptr_V_8x4.top_left +=  (zero_1b << (16 - 1));	
#endif
			md_chroma_8(8, 8, md_fw_ptr,
				&cu_data_best_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
				&IP_buffer_U_8x8, &IP_buffer_V_8x8,
#if IP_BUFFER_NULL
				NULL, NULL,
				NULL, NULL,
#else
				&IP_buffer_ptr_U_4x8, &IP_buffer_ptr_V_4x8, //NULL, NULL,
				&IP_buffer_ptr_U_8x4, &IP_buffer_ptr_V_8x4, //NULL, NULL,
#endif
				Fetch_Ref_window_ptr,
				&IP_buffer_8x16, &IP_buffer_16x8, &IP_buffer_16x16,
				&IP_map_scu_8x16, &IP_map_scu_16x8, &IP_map_scu_16x16);
		}
		else
		{
			md_16x16_16x8_8x16_loop_md2_chroma(best_split_mode_luma,
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c,
				&cons_pred_mode_child[0], tree_status, &tree_status_child[0],
				&IP_buffer_16x16,
				&IP_buffer_U_8x8, &IP_buffer_V_8x8,
				&IP_map_scu_16x16,
				Fetch_Ref_window_ptr);

			best_split_mode = best_split_mode_luma;

			copy_cu_data_array_src_chroma<8>(&cu_data_best_c, &cu_data_temp_c, 0, 0,
				cu_width_log2, cu_height_log2, cu_width_log2);

			best_cons_pred_mode = cons_pred_mode_child[0];
			best_tree_status_child = tree_status_child[0];

			if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}
		}

		switch (split_mode_father)
		{
		case SPLIT_BI_HOR:
			IP_buffer_U_8x8.update_bi_hor(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_8x8.update_bi_hor(part_num, cu_data_best_c.reco_v);
			IP_map_scu_16x16.update_bi_hor(part_num, IP_map_scu_ptr_16x32->cur_map_scu, cu_data_best_c.map_scu);
			break;
		case SPLIT_BI_VER:
			IP_buffer_U_8x8.update_bi_ver(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_8x8.update_bi_ver(part_num, cu_data_best_c.reco_v);
			IP_map_scu_16x16.update_bi_ver(part_num, IP_map_scu_ptr_32x16->cur_map_scu, cu_data_best_c.map_scu);
			break;
		default: assert(0);
		}

		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}


static s64 md_16x16_loop_md2_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_U_8x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_V_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_U_16x8, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_V_16x8,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	U3 split_mode = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	if (isBTH_allow) {

		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);
		prepare_cu_info_COM_combine_only_bt_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_16x16_top_md2_chroma(
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, bet_depth + 1, cons_pred_mode_BTH[0], child_tree_status[0], part_num, split_struct,
				cu_width_log2, cu_height_log2,
				IP_buffer_ptr_16x32, IP_buffer_ptr_32x16,
				IP_buffer_ptr_U_8x16, IP_buffer_ptr_V_8x16,
				IP_buffer_ptr_U_16x8, IP_buffer_ptr_V_16x8,
				IP_map_scu_ptr_16x32, IP_map_scu_ptr_32x16,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return 1;
}

#if !ONLY_QT_QT
static s64 circuit_32x32_32x16_16x32_md2_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_U_8x32, IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_V_8x32,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_U_32x8, IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_V_32x8,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
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
	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
	U3 num_split_to_try = split_struct.num_split[part_num];

	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[2] = { TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;

	U3 split_mode[3];
	U2 best_index;
	split_mode[0] = NO_SPLIT;
	split_mode[1] = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return  0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}


	if (split_struct.split_allow[part_num][best_split_mode_luma]) {

		if (best_split_mode_luma == NO_SPLIT)
			best_index = 0;
		else
			best_index = 1;

		static IP_ARRAY_SIZE<16, 32>				IP_buffer_16x32;
		static IP_ARRAY_SIZE<32, 16>				IP_buffer_32x16;
		static IP_ARRAY_SIZE<32, 32>				IP_buffer_32x32;
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_U_8x16;
		static IP_ARRAY_SIZE<8, 16>					IP_buffer_V_8x16;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_U_16x8;
		static IP_ARRAY_SIZE<16, 8>					IP_buffer_V_16x8;
		static IP_MAP_SCU_SIZE<16, 32>				IP_map_scu_16x32;
		static IP_MAP_SCU_SIZE<32, 16>				IP_map_scu_32x16;
		static IP_MAP_SCU_SIZE<32, 32>				IP_map_scu_32x32;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<32>	cu_data_temp_c[2];
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		S7 ip_array_size_idx;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		for (int i = 0; i < 2; i++)
		{
			cu_data_chroma_pos<32>(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_temp_c[i], prev_cu_data_temp_ptr, qt_depth + bet_depth);
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x16.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x16.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x16.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x16.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x16.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x16.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_16x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_16x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_16x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_16x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_16x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_16x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (split_mode_father)
		{
		case SPLIT_BI_VER:
			assert(cu_width == 32 && cu_height == 16);
			IP_buffer_32x16.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_64x16, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_16x8.fetch_bi_ver(part_num, IP_buffer_ptr_U_32x8);
			IP_buffer_V_16x8.fetch_bi_ver(part_num, IP_buffer_ptr_V_32x8);
			IP_map_scu_32x16.fetch_bi_ver(part_num, IP_map_scu_ptr_64x16);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 16 && cu_height == 32);
			IP_buffer_16x32.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_16x64, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_8x16.fetch_bi_hor(part_num, IP_buffer_ptr_U_8x32);
			IP_buffer_V_8x16.fetch_bi_hor(part_num, IP_buffer_ptr_V_8x32);
			IP_map_scu_16x32.fetch_bi_hor(part_num, IP_map_scu_ptr_16x64);
			break;
		default:
			assert(0);
		}

#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_32x32.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 5) && (cu_height_log2 == 5);
			int flag2 = (x0 > 0 && ((IP_map_scu_16x32.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 5);
			int flag3 = (x0 > 0 && ((IP_map_scu_32x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 5) && (cu_height_log2 == 4);

			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_32x32.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x32.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_32x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma == NO_SPLIT) {
			best_split_mode = best_split_mode_luma;
			best_cons_pred_mode = cons_pred_mode_child[best_index];
			best_tree_status_child = tree_status_child[best_index];

			/*if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}*/
			if (cu_height == 16 || cu_width == 16)
			{
				md_chroma_16_chroma(md_fw_ptr,
					&cu_data_best_c,
					x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
					md_input_ptr,
					p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
					RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
					((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cons_pred_mode,
					NULL, NULL,
					&IP_buffer_U_8x16, &IP_buffer_V_8x16,
					&IP_buffer_U_16x8, &IP_buffer_V_16x8,
					Fetch_Ref_window_ptr,
					&IP_buffer_32x32, &IP_buffer_16x32, &IP_buffer_32x16,
					&IP_map_scu_32x32, &IP_map_scu_16x32, &IP_map_scu_32x16);
			}
		}
		else {
			md_16x16_loop_md2_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V, split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c[1],
				&cons_pred_mode_child[1], tree_status, &tree_status_child[1],
				&IP_buffer_16x32, &IP_buffer_32x16,
				&IP_buffer_U_8x16, &IP_buffer_V_8x16,
				&IP_buffer_U_16x8, &IP_buffer_V_16x8,
				&IP_map_scu_16x32, &IP_map_scu_32x16,
				Fetch_Ref_window_ptr);

			copy_cu_data_array_src_chroma<16>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}

		switch (split_mode_father)
		{
		case SPLIT_BI_VER:
			assert(cu_width == 32 && cu_height == 16);
			IP_buffer_U_16x8.update_bi_ver(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_16x8.update_bi_ver(part_num, cu_data_best_c.reco_v);
			IP_map_scu_32x16.update_bi_ver(part_num, IP_map_scu_ptr_64x16->cur_map_scu, cu_data_best_c.map_scu);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 16 && cu_height == 32);
			IP_buffer_U_8x16.update_bi_hor(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_8x16.update_bi_hor(part_num, cu_data_best_c.reco_v);
			IP_map_scu_16x32.update_bi_hor(part_num, IP_map_scu_ptr_16x64->cur_map_scu, cu_data_best_c.map_scu);
			break;
		default:
			assert(0);
		}

		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}

static s64 md_32x32_32x16_16x32_loop_md2_chroma(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_U_8x32, IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_V_8x32,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_U_32x8, IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_V_32x8,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (isBTH_allow) {
		child_tree_status[0] = TREE_LC;
		prepare_cu_info_COM_combine_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			circuit_32x32_32x16_16x32_md2_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
				IP_buffer_ptr_U_8x32, IP_buffer_ptr_V_8x32,
				IP_buffer_ptr_U_32x8, IP_buffer_ptr_V_32x8,
				IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return 1;
}
#endif //!ONLY_QT_QT

void copy_to_cu_data_chroma32_chroma(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1], int coef_nz_y, u32 luma_map_scu)
{
	int i;
	int j;
	int k;
	int idx;
	int size;
	U3 cu_width_log2;
	U3 cu_height_log2;
	//int num_coef_y = 0;
	U12 num_coef_u = 0;
	U12 num_coef_v = 0;
	//int num_nnz_sum = 0;
	U1 cu_cbf_flag = 0;
	//int pb_idx_u = 0;
	U2 tb_idx_u = 0;
	U2 tb_idx_y;
	U14 num_coef_y = 0;

	cu_width_log2 = mod_info_best->cu_width_log2;
	cu_height_log2 = mod_info_best->cu_height_log2;

	if ((tree_status) != (TREE_L)) {
		/* copy coef */
		size = ((((mod_info_best->cu_width * mod_info_best->cu_height)) * sizeof(s16) >> 2));
		//if (coef_rec_write_flag[0] == 0) {////for Intra_CU
		for (i = 0; i < ((mod_info_best->cu_width * mod_info_best->cu_height) >> 2); i++) {
			cu_data->coef_u[i] = mod_info_best->coef_u[i];
			cu_data->coef_v[i] = mod_info_best->coef_v[i];
			cu_data->reco_u[i] = mod_info_best->rec_u[i];
			cu_data->reco_v[i] = mod_info_best->rec_v[i];  /* copy reco */

		}

		//}

		for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height / 4; i++) {
			num_coef_u += ((cu_data->coef_u[i]) != 0 ? 1 : 0);
			num_coef_v += ((cu_data->coef_v[i]) != 0 ? 1 : 0);
		}
	}
	//if (tree_status != TREE_C)
	//{
	//	for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height; i++) {
	//		num_coef_y += ((cu_data->coef_y[i]) != 0 ? 1 : 0);
	//	}
	//}

	num_coef_y = coef_nz_y;

	if (tree_status == TREE_LC) {
		cu_cbf_flag = ((num_coef_y == 0 ? 0 : 1)) | ((num_coef_u == 0 ? 0 : 1)) | ((num_coef_v == 0 ? 0 : 1));
	}
	else if ((tree_status) == (TREE_L)) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	/* copy mode info */
	if ((tree_status) == (TREE_C)) {
		idx = 0;
		for (j = 0; j < mod_info_best->cu_height >> 2; j++) {
			for (i = 0; i < mod_info_best->cu_width >> 2; i++) {
				//intra chroma mode
				if (mod_info_best->cu_mode == 0) {
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
				// residual and partition
#if SRCC
				cu_data->num_nz_coef[1][idx + i] = num_coef_u;
				cu_data->num_nz_coef[2][idx + i] = num_coef_v;
#elif
				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
				}
#endif
			}
			idx += mod_info_best->cu_width >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < mod_info_best->cu_height >> 2; j++) {
			for (i = 0; i < mod_info_best->cu_width >> 2; i++) {

				//			tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, mod_info_best->cu_width,mod_info_best->cu_height);
				tb_idx_y = get_part_idx(SIZE_2Nx2N, i << 2, j << 2, mod_info_best->cu_width,
					mod_info_best->cu_height);
				//pb_idx_u = 0;
				tb_idx_u = 0;
#if SRCC
				cu_data->num_nz_coef[1][idx + i] = num_coef_u;
				cu_data->num_nz_coef[2][idx + i] = num_coef_v;
				cu_data->num_nz_coef[0][idx + i] = mod_info_best->num_nz[tb_idx_y][0];
#elif
				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
				}
#endif
				

				if (cu_cbf_flag) {
					//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((1 << 24));
					cu_data->map_scu[idx + i] = luma_map_scu | ((1 << 24));
				}
				else {
					//cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((~(1 << 24)));
					cu_data->map_scu[idx + i] = luma_map_scu & ((~(1 << 24)));
				}
				if (mod_info_best->cu_mode == 0)
				{
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
			}
			idx += mod_info_best->cu_width >> 2;
		}

	}
}


s64 md_chroma_32_chroma(
	U8 w, U8 h, MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* curr_cu_data_temp_array_ptr, 
	U16 x, U16 y, U8 cu_width_log2,
	U8 cu_height_log2, U8 tree_status, U8 cud, MD_INPUT* md_input_ptr, pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],  
#endif
	U8 need_dochroma, U16 cup, U8 ctx_cons_pred_mode,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32, IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_U_16x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_V_16x32,
	IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_U_32x16, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_V_32x16,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_U_8x32, IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_V_8x32,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_U_32x8, IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_V_32x8,
	IP_ARRAY_SIZE<4, 32>* IP_buffer_ptr_U_4x32, IP_ARRAY_SIZE<4, 32>* IP_buffer_ptr_V_4x32,
	IP_ARRAY_SIZE<32, 4>* IP_buffer_ptr_U_32x4, IP_ARRAY_SIZE<32, 4>* IP_buffer_ptr_V_32x4,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 64>* IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>* IP_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	IP_MAP_SCU_SIZE<8, 64>* IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>* IP_map_scu_ptr_64x8)
{
	if (!need_dochroma)
	{
		return 0;
	}

	static MD_COM_MODE_BEST mod_info_best;
	static COM_PART_INFO bst_info_pb_info;
	static COM_PART_INFO bst_info_tb_info;

	U2 cons_pred_mode = ctx_cons_pred_mode;
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	//pel* orgU;
	static RDOQ_ARRAY rdoq_array;
	static RDOQ_MODEL rdoq_model_t;
	static RDOQ_MODEL model_cnt_t;
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_run[i][j] = 0;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_cbf[i][j] = 0;
		}
	}
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			rdoq_array.rdoq_est_level[i][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			for (int k = 0; k < 12; k++)
			{
				for (int l = 0; l < 2; l++)
				{
					rdoq_array.rdoq_est_last[i][j][k][l] = 0;
				}
			}
		}
	}
	int i;
	for (i = 0; i < 12; i++) {
		rdoq_model_t.last1[i] = PROB_INIT;
		model_cnt_t.last1[i] = 0;
	}
	for (i = 0; i < 22; i++) {
		rdoq_model_t.last2[i] = PROB_INIT;
		model_cnt_t.last2[i] = 0;
	}
	for (i = 0; i < 24; i++) {
		rdoq_model_t.run_rdoq[i] = PROB_INIT;
		model_cnt_t.run_rdoq[i] = 0;
		rdoq_model_t.level[i] = PROB_INIT;
		model_cnt_t.level[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		rdoq_model_t.cbf[i] = PROB_INIT;
		model_cnt_t.cbf[i] = 0;
	}
	U1 coef_rec_write_flag[1] = { 0 };
	int coef_nz_y;
#if SUB_TMVP
	static CORE_SBTMVP core_sbtmvp;
	CORE_SBTMVP* core_sbtmvp_ptr = &core_sbtmvp;
	core_sbtmvp.sbTmvp_flag = 0;
	core_sbtmvp.best_sbTmvp_flag = 0;
	for (int k = 0; k < SBTMVP_NUM; k++)
	{
		for (int i = 0; i < REFP_NUM; i++)
		{
			core_sbtmvp.sbTmvp[k].ref_idx[i] = -1;
			core_sbtmvp.best_sbTmvp[k].ref_idx[i] = -1;
			for (int j = 0; j < MV_D; j++)
			{
				core_sbtmvp.sbTmvp[k].mv[i][j] = 0;
				core_sbtmvp.best_sbTmvp[k].mv[i][j] = 0;
			}
		}
	}
#endif
	coef_nz_y = mode_cu_init_chroma<32>(&rdoq_model_t, &model_cnt_t, &rdoq_array, &mod_info_best, &bst_info_pb_info, &bst_info_tb_info, x, y, cu_width_log2, cu_height_log2, cud,
		md_fw_ptr->pic_width_in_scu, TREE_C, &cons_pred_mode, curr_cu_data_temp_array_ptr, md_input_ptr
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
	);


	//static pel orgU[2][32 * 32];
	static pel orgU[2][32][32];

	for (int k = 0; k < 2; k++) {
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				//orgU[k][(i << 5) + j] = p_fenc_LCU_UV[64 * (((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i) + (((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j + (k << 5))];
				if (k == 0) {
					orgU[k][i][j] = p_fenc_LCU_U[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
				else {
					orgU[k][i][j] = p_fenc_LCU_V[((mod_info_best.y_pos - md_input_ptr->pix_y) >> 1) + i][((mod_info_best.x_pos - md_input_ptr->pix_x) >> 1) + j];
				}
			}
		}
	}

#if USE_TSCPM
	pel piRecoY[64][64];// = md_input_ptr->reco_y + (mod_info_best.y_pos - md_input_ptr->pix_y) * 64 + (mod_info_best.x_pos - md_input_ptr->pix_x);
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			piRecoY[i][j] = md_input_ptr->reco_y[(mod_info_best.y_pos - md_input_ptr->pix_y + i) ][(mod_info_best.x_pos - md_input_ptr->pix_x + j)];
		}
	}
#endif
#if CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;

	const int com_tbl_qp_chroma_adjust[64] =
	{
		//this table aligned with spec
		0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 42, 43, 43, 44, 44, 45, 45,
		46, 46, 47, 47, 48, 48, 48, 49, 49, 49,
		50, 50, 50, 51
	};
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
	if (md_input_ptr->cu_dqp_enable) {
		int qp_offset_bit_depth = (8 * (md_fw_ptr->bit_depth_internal - 8));

		md_input_ptr->qp_u = md_input_ptr->cu_luma_qp[luma_cup];
		md_input_ptr->qp_v = md_input_ptr->cu_luma_qp[luma_cup];

		md_input_ptr->qp_u += md_input_ptr->chroma_quant_param_delta_cb - qp_offset_bit_depth;
		md_input_ptr->qp_v += md_input_ptr->chroma_quant_param_delta_cr - qp_offset_bit_depth;

		md_input_ptr->qp_u = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_u)];
		md_input_ptr->qp_v = com_tbl_qp_chroma_adjust[COM_MIN(MAX_QUANT_BASE, md_input_ptr->qp_v)];

		md_input_ptr->qp_u = COM_CLIP(md_input_ptr->qp_u + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
		md_input_ptr->qp_v = COM_CLIP(md_input_ptr->qp_v + qp_offset_bit_depth, MIN_QUANT, MAX_QUANT_BASE + qp_offset_bit_depth);
	}
#endif
	RDOChroma32(&rdoq_array, md_fw_ptr, tree_status, &mod_info_best, &bst_info_pb_info, md_input_ptr, orgU, 
		IP_buffer_ptr_U_32x32, IP_buffer_ptr_V_32x32,
		IP_buffer_ptr_U_16x32, IP_buffer_ptr_V_16x32,
		IP_buffer_ptr_U_32x16, IP_buffer_ptr_V_32x16,
		IP_buffer_ptr_U_8x32, IP_buffer_ptr_V_8x32,
		IP_buffer_ptr_U_32x8, IP_buffer_ptr_V_32x8,
		IP_buffer_ptr_U_4x32, IP_buffer_ptr_V_4x32,
		IP_buffer_ptr_U_32x4, IP_buffer_ptr_V_32x4,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, 
#endif
		coef_rec_write_flag, Fetch_Ref_window_ptr, piRecoY,
		IP_buffer_ptr_64x64,
		IP_buffer_ptr_32x64, IP_buffer_ptr_64x32,
		IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
		IP_buffer_ptr_8x64, IP_buffer_ptr_64x8,
		IP_map_scu_ptr_64x64,
		IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32,
		IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
		IP_map_scu_ptr_8x64, IP_map_scu_ptr_64x8
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
	);


	if (mod_info_best.cu_mode == 0)
	{
		mod_info_best.cu_mode = 0;
		mod_info_best.affine_flag = 0;
	}
	else {

		coef_rec_write_flag[0] = 1;
	}
#if !CUDQP_QP_MAP
	u32 luma_map_scu;
	int scu_stride = 16;
	int x_scu_in_LCU = (x % 64) >> 2;
	int y_scu_in_LCU = (y % 64) >> 2;
	int luma_cup;
	U6 cu_w_scu = cu_width >> 2;
	U6 cu_h_scu = cu_height >> 2;
	luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));
#endif
	luma_map_scu = md_input_ptr->map_scu[luma_cup];


	copy_to_cu_data_chroma<32>(md_input_ptr->qp_y, curr_cu_data_temp_array_ptr, &mod_info_best, tree_status, coef_rec_write_flag, coef_nz_y, luma_map_scu
#if CUDQP_QP_MAP
		, md_input_ptr->cu_qp_group_pred_qp, md_input_ptr->cu_dqp_enable
#endif
		);

#if CUDQP_QP_MAP
	int cu_cbf_flag = MCU_GET_CBF(curr_cu_data_temp_array_ptr->map_scu[0]);
	int qpy= MCU_GET_QP(curr_cu_data_temp_array_ptr->map_scu[0]);
	if (md_input_ptr->cu_dqp_enable && tree_status != TREE_C && cu_cbf_flag)
	{
		md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->cu_luma_qp[luma_cup];

	}
#endif

	return 0;
}


#if !ONLY_QT_QT
static s64 circuit_64x8_8x64_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* prev_cu_data_array,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_U_8x32,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_V_8x32,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_U_32x8,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_V_32x8,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
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

	s64 cost = MAX_S64;
	U2 tree_status_child[2] = { TREE_LC };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;
	U3 split_mode[2];
	U3 best_split_mode_luma = NO_SPLIT;
	split_mode[0] = NO_SPLIT;
	//split_mode[1] = (cu_height == 8) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	//U5 rmd_index = rmd_idx_tbl[cu_width_log2 - 2][cu_height_log2 - 2];

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	//decide allowed split modes for the current node
	/***************************** Step 1: decide normatively allowed split modes ********************************/
	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma])
	{
		static IP_ARRAY_SIZE<8, 64>					IP_buffer_8x64;
		static IP_ARRAY_SIZE<64, 8>					IP_buffer_64x8;
		static IP_ARRAY_SIZE<64, 64>				IP_buffer_64x64;
		static IP_ARRAY_SIZE<32, 64>				IP_buffer_32x64;
		static IP_ARRAY_SIZE<64, 32>				IP_buffer_64x32;
		static IP_ARRAY_SIZE<4, 32>					IP_buffer_U_4x32;
		static IP_ARRAY_SIZE<4, 32>					IP_buffer_V_4x32;
		static IP_ARRAY_SIZE<32, 4>					IP_buffer_U_32x4;
		static IP_ARRAY_SIZE<32, 4>					IP_buffer_V_32x4;
		static IP_MAP_SCU_SIZE<8, 64>				IP_map_scu_8x64;
		static IP_MAP_SCU_SIZE<64, 8>				IP_map_scu_64x8;
		static IP_MAP_SCU_SIZE<64, 64>				IP_map_scu_64x64;
		static IP_MAP_SCU_SIZE<32, 64>				IP_map_scu_32x64;
		static IP_MAP_SCU_SIZE<64, 32>				IP_map_scu_64x32;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	cu_data_best_c;
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_best_c, prev_cu_data_array, qt_depth + bet_depth);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_4x32.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_4x32.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_4x32.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_4x32.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_4x32.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_4x32.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_32x4.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_32x4.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_32x4.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_32x4.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_32x4.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_32x4.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_8x64.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_8x64.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_8x64.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_64x8.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_64x8.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_64x8.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);

		switch (split_mode_father)
		{
		case SPLIT_BI_HOR:
			assert(cu_width == 64 && cu_height == 8);
			IP_buffer_64x8.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_64x16, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_32x4.fetch_bi_hor(part_num, IP_buffer_ptr_U_32x8);
			IP_buffer_V_32x4.fetch_bi_hor(part_num, IP_buffer_ptr_V_32x8);;
			IP_map_scu_64x8.fetch_bi_hor(part_num, IP_map_scu_ptr_64x16);
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 8 && cu_height == 64);
			IP_buffer_8x64.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_16x64, md_input_ptr, x_pos_lcu, y_pos_lcu);
			IP_buffer_U_4x32.fetch_bi_ver(part_num, IP_buffer_ptr_U_8x32);
			IP_buffer_V_4x32.fetch_bi_ver(part_num, IP_buffer_ptr_V_8x32);;
			IP_map_scu_8x64.fetch_bi_ver(part_num, IP_map_scu_ptr_16x64);
			break;
		default:
			assert(0);
		}

		best_split_mode = best_split_mode_luma;
		best_cons_pred_mode = cons_pred_mode_child[0];
		best_tree_status_child = tree_status_child[0];

		if (best_split_mode != NO_SPLIT)
		{
			tree_status_temp = TREE_C;
		}
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
#if ENABLE_MD32C_BB
		md_chroma_32_wrapper(cu_width >> 1, cu_height >> 1, rdoq_model_c, model_cnt_c, md_fw_ptr, me_line_map_ptr, inner_nebs, &curr_cu_data_best, x0, y0, cu_width_log2, cu_height_log2, tree_status_temp[0], cud, md_input_ptr,
			p_fenc_LCU_UV, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos, ((best_tree_status_child[0] == TREE_L && tree_status == TREE_LC) || best_split_mode[0] == NO_SPLIT), cup, cons_pred_mode, IP_buffer_ptr, MPM_buffer_ptr, MAP_SCU_IP_ptr);

#else
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag6 = (x0 > 0 && ((IP_map_scu_8x64.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 3) && (cu_height_log2 == 6);
			int flag7 = (x0 > 0 && ((IP_map_scu_64x8.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 6) && (cu_height_log2 == 3);

			if (flag6)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_8x64.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag7)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_64x8.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}
		}
#endif
		md_chroma_32_chroma(cu_width >> 1, cu_height >> 1, md_fw_ptr,
			&cu_data_best_c,
			x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
			md_input_ptr,
			p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
			RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
			((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			NULL, NULL,
			&IP_buffer_U_4x32, &IP_buffer_V_4x32,
			&IP_buffer_U_32x4, &IP_buffer_V_32x4,
			Fetch_Ref_window_ptr,
			&IP_buffer_64x64,
			&IP_buffer_32x64, &IP_buffer_64x32,
			IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
			&IP_buffer_8x64, &IP_buffer_64x8,
			&IP_map_scu_64x64,
			&IP_map_scu_32x64, &IP_map_scu_64x32,
			IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
			&IP_map_scu_8x64, &IP_map_scu_64x8
		);
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
			IP_buffer_U_32x4.update_bi_hor(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_32x4.update_bi_hor(part_num, cu_data_best_c.reco_v);;
			IP_map_scu_64x8.update_bi_hor(part_num, IP_map_scu_ptr_64x16->cur_map_scu, cu_data_best_c.map_scu);
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 8 && cu_height == 64);
			IP_buffer_U_4x32.update_bi_ver(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_4x32.update_bi_ver(part_num, cu_data_best_c.reco_v);;
			IP_map_scu_8x64.update_bi_ver(part_num, IP_map_scu_ptr_16x64->cur_map_scu, cu_data_best_c.map_scu);
			break;
		default:
			assert(0);
		}

		prev_cu_data_array->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr,
		//	MAP_SCU_IP_ptr);
	}
	return 1;
}

static s64 md_64x8_8x64_loop_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_U_8x32,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_V_8x32,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_U_32x8,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_V_32x8,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bt_cost = MAX_S64;
	COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	U3 split_mode = (cu_width == 64 && cu_height == 16) ? SPLIT_BI_HOR : SPLIT_BI_VER;

	if (isBTH_allow) {
		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);

		prepare_cu_info_COM_combine_only_bt_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS loop_tripcount max=2
			circuit_64x8_8x64_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, bet_depth + 1, cons_pred_mode_BTH[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_16x64, IP_buffer_ptr_64x16,
				IP_buffer_ptr_U_8x32,
				IP_buffer_ptr_V_8x32,
				IP_buffer_ptr_U_32x8,
				IP_buffer_ptr_V_32x8,
				IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return 1;
}


s64 circuit_64x16_16x64_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_U_16x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_V_16x32,
	IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_U_32x16, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_V_32x16,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father, int ns_64x64, ENC_CU_DATA* md_output_luma)
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

	s64 cost[3] = { MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[3] = { TREE_LC, TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[3] = { NO_MODE_CONS, NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;
	U3 split_mode_order[3];
	U2 best_index;

	split_mode_order[0] = NO_SPLIT;
	split_mode_order[1] = (cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;
	split_mode_order[2] = (cu_height == 16) ? SPLIT_BI_HOR : SPLIT_BI_VER;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma && (cu_width == 16 || cu_height == 16))
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma]) {

		if (best_split_mode_luma == NO_SPLIT)
			best_index = 0;

		if (((best_split_mode_luma == SPLIT_BI_VER) && (cu_height == 16)) || ((best_split_mode_luma == SPLIT_BI_HOR) && (cu_height != 16)))
			best_index = 1;

		if (((best_split_mode_luma == SPLIT_BI_HOR) && (cu_height == 16)) || ((best_split_mode_luma == SPLIT_BI_VER) && (cu_height != 16)))
			best_index = 2;

		static IP_ARRAY_SIZE<16, 64>				IP_buffer_16x64;
		static IP_ARRAY_SIZE<64, 16>				IP_buffer_64x16;
		static IP_ARRAY_SIZE<8, 64>					IP_buffer_8x64;
		static IP_ARRAY_SIZE<64, 8>					IP_buffer_64x8;
		static IP_ARRAY_SIZE<8, 32>					IP_buffer_U_8x32;
		static IP_ARRAY_SIZE<8, 32>					IP_buffer_V_8x32;
		static IP_ARRAY_SIZE<32, 8>					IP_buffer_U_32x8;
		static IP_ARRAY_SIZE<32, 8>					IP_buffer_V_32x8;
		static IP_MAP_SCU_SIZE<16, 64>				IP_map_scu_16x64;
		static IP_MAP_SCU_SIZE<64, 16>				IP_map_scu_64x16;
		static IP_MAP_SCU_SIZE<8, 64>				IP_map_scu_8x64;
		static IP_MAP_SCU_SIZE<64, 8>				IP_map_scu_64x8;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	cu_data_temp_c[3];
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		for (int i = 0; i < 3; i++)
		{
			cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_temp_c[i], prev_cu_data_temp_ptr, qt_depth + bet_depth);
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_8x32.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_8x32.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_8x32.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_8x32.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_8x32.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_8x32.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_32x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_32x8.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_32x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_32x8.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_32x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_32x8.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_16x64.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_16x64.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_16x64.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);

		//MAP_SCU_IP_ptr->get_map_scu_up_line(IP_map_scu_64x16.map_scu_up_line, MAP_SCU_IP_ptr, x0, y0, cu_width);
		//MAP_SCU_IP_ptr->get_map_scu_left_line_new(IP_map_scu_64x16.map_scu_left_line_new, MAP_SCU_IP_ptr, x0, y0, cu_height);
		//MAP_SCU_IP_ptr->get_map_scu_top_left_new(&IP_map_scu_64x16.map_scu_top_left_new, MAP_SCU_IP_ptr, x0, y0);

		switch (ns_64x64)
		{
		case 0:
			switch (split_mode_father)
			{
			case NO_SPLIT:
				break;
			case SPLIT_BI_VER:
				assert(cu_width == 16 && cu_height == 64);
				IP_buffer_16x64.fetch_bi_ver_chroma(part_num, IP_buffer_ptr_32x64, md_input_ptr, x_pos_lcu, y_pos_lcu);
				IP_buffer_U_8x32.fetch_bi_ver(part_num, IP_buffer_ptr_U_16x32);
				IP_buffer_V_8x32.fetch_bi_ver(part_num, IP_buffer_ptr_V_16x32);
				IP_map_scu_16x64.fetch_bi_ver(part_num, IP_map_scu_ptr_32x64);
				break;
			case SPLIT_BI_HOR:
				assert(cu_width == 64 && cu_height == 16);
				IP_buffer_64x16.fetch_bi_hor_chroma(part_num, IP_buffer_ptr_64x32, md_input_ptr, x_pos_lcu, y_pos_lcu);
				IP_buffer_U_32x8.fetch_bi_hor(part_num, IP_buffer_ptr_U_32x16);
				IP_buffer_V_32x8.fetch_bi_hor(part_num, IP_buffer_ptr_V_32x16);
				IP_map_scu_64x16.fetch_bi_hor(part_num, IP_map_scu_ptr_64x32);
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
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag4 = (x0 > 0 && ((IP_map_scu_16x64.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 4) && (cu_height_log2 == 6);
			int flag5 = (x0 > 0 && ((IP_map_scu_64x16.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 6) && (cu_height_log2 == 4);
			//if (flag1 || flag2 || flag3 || flag4 || flag5 || flag6 || flag7)
			//	md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
			if (flag4)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_16x64.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag5)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_64x16.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma == NO_SPLIT)
		{
			best_split_mode = best_split_mode_luma;
			best_cons_pred_mode = cons_pred_mode_child[best_index];
			best_tree_status_child = tree_status_child[best_index];

			/*if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}*/

			if (cu_height == 16 || cu_width == 16)
			{

				md_chroma_32_chroma(cu_width >> 1, cu_height >> 1, md_fw_ptr,
					&cu_data_best_c,
					x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
					md_input_ptr,
					p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
					RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
					((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
					NULL, NULL,
					NULL, NULL,
					NULL, NULL,
					&IP_buffer_U_8x32, &IP_buffer_V_8x32,
					&IP_buffer_U_32x8, &IP_buffer_V_32x8,
					NULL, NULL,
					NULL, NULL,
					Fetch_Ref_window_ptr,
					IP_buffer_ptr_64x64,
					IP_buffer_ptr_32x64, IP_buffer_ptr_64x32,
					&IP_buffer_16x64, &IP_buffer_64x16,
					&IP_buffer_8x64, &IP_buffer_64x8,
					IP_map_scu_ptr_64x64,
					IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32,
					&IP_map_scu_16x64, &IP_map_scu_64x16,
					&IP_map_scu_8x64, &IP_map_scu_64x8
				);
			}
		}
		else if (((best_split_mode_luma == SPLIT_BI_VER) && (cu_height == 16)) || ((best_split_mode_luma == SPLIT_BI_HOR) && (cu_height != 16)))
		{
			md_32x32_32x16_16x32_loop_md2_chroma(split_mode_order[1],
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][split_mode_order[1]], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode, /*(int)cons_pred_mode,*/
				&cu_data_temp_c[1],
				&cons_pred_mode_child[1], &tree_status_child[1],
				&IP_buffer_16x64, &IP_buffer_64x16,
				&IP_buffer_U_8x32, &IP_buffer_V_8x32,
				&IP_buffer_U_32x8, &IP_buffer_V_32x8,
				&IP_map_scu_16x64, &IP_map_scu_64x16,
				Fetch_Ref_window_ptr);

			copy_cu_data_array_src_chroma<32>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}
		else if (((best_split_mode_luma == SPLIT_BI_HOR) && (cu_height == 16)) || ((best_split_mode_luma == SPLIT_BI_VER) && (cu_height != 16)))
		{
			md_64x8_8x64_loop_chroma(
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][split_mode_order[2]], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c[2],
				&cons_pred_mode_child[2], tree_status, &tree_status_child[2],
				&IP_buffer_16x64, &IP_buffer_64x16,
				&IP_buffer_U_8x32,
				&IP_buffer_V_8x32,
				&IP_buffer_U_32x8,
				&IP_buffer_V_32x8,
				&IP_map_scu_16x64, &IP_map_scu_64x16,
				Fetch_Ref_window_ptr);

			copy_cu_data_array_src_chroma<32>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}


		switch (ns_64x64)
		{
		case 0:
			switch (split_mode_father)
			{
			case NO_SPLIT:
				break;
			case SPLIT_BI_VER:
				assert(cu_width == 16 && cu_height == 64);
				IP_buffer_U_8x32.update_bi_ver(part_num, cu_data_best_c.reco_u);
				IP_buffer_V_8x32.update_bi_ver(part_num, cu_data_best_c.reco_v);
				IP_map_scu_16x64.update_bi_ver(part_num, IP_map_scu_ptr_32x64->cur_map_scu, cu_data_best_c.map_scu);
				break;
			case SPLIT_BI_HOR:
				assert(cu_width == 64 && cu_height == 16);
				IP_buffer_U_32x8.update_bi_hor(part_num, cu_data_best_c.reco_u);
				IP_buffer_V_32x8.update_bi_hor(part_num, cu_data_best_c.reco_v);
				IP_map_scu_64x16.update_bi_hor(part_num, IP_map_scu_ptr_64x32->cur_map_scu, cu_data_best_c.map_scu);
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

		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}

static U1 md_64x16_16x64_loop_chroma(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	U1 isBT_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_U_16x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_V_16x32,
	IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_U_32x16, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_V_32x16,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	strFetch_ref_window* Fetch_Ref_window_ptr, int ns_64x64, ENC_CU_DATA* md_output_luma)
{
	s64 bt_cost = (s64)MAX_S64;
	if (isBT_allow) {
		COM_SPLIT_STRUCT split_struct;
		U3 part_num = 0;

		prepare_cu_info_COM_combine_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS loop_tripcount max=2
			circuit_64x16_16x64_chroma(
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				cu_data_temp_ptr, qt_depth, split_mode == NO_SPLIT ? bet_depth : (U3)(bet_depth + 1), cons_pred_mode_BTH[0],
				tree_status, part_num, split_struct, cu_width_log2, cu_height_log2,
				IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32,
				IP_buffer_ptr_U_16x32, IP_buffer_ptr_V_16x32,
				IP_buffer_ptr_U_32x16, IP_buffer_ptr_V_32x16,
				IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32,
				Fetch_Ref_window_ptr, split_mode, ns_64x64, md_output_luma);
		}
	}
	return 1;
}

static U1 circuit_64x64_64x32_32x64_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* prev_cu_data_temp_ptr,
	const U3 parent_split,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father, ENC_CU_DATA* md_output_luma)
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

	s64 cost[2] = { MAX_S64, MAX_S64 };
	U2 cons_pred_mode_child[2] = { NO_MODE_CONS, NO_MODE_CONS };
	U2 tree_status_child[2] = { TREE_LC, TREE_LC };

	U2 best_tree_status_child = TREE_LC;
	U3 best_split_mode_luma = NO_SPLIT;
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;
	U3 split_mode[2];
	U2 best_index;
	split_mode[0] = NO_SPLIT;
	split_mode[1] = (cu_height == 32) ? SPLIT_BI_HOR : SPLIT_BI_VER;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	if (md_input_ptr->do_chroma)
	{
		modify_md_enc_split_allow(cud, cup, cu_width, cu_height, md_input_ptr, split_struct.split_allow[part_num], &best_split_mode_luma);
	}

	if (split_struct.split_allow[part_num][best_split_mode_luma])
	{

		if (best_split_mode_luma == NO_SPLIT)
			best_index = 0;
		else
			best_index = 1;

		static IP_ARRAY_SIZE<32, 64>				IP_buffer_32x64;
		static IP_ARRAY_SIZE<64, 32>				IP_buffer_64x32;
		static IP_ARRAY_SIZE<8, 64>					IP_buffer_8x64;
		static IP_ARRAY_SIZE<64, 8>					IP_buffer_64x8;
		static IP_ARRAY_SIZE<16, 64>				IP_buffer_16x64;
		static IP_ARRAY_SIZE<64, 16>				IP_buffer_64x16;
		static IP_ARRAY_SIZE<16, 32>				IP_buffer_U_16x32;
		static IP_ARRAY_SIZE<16, 32>				IP_buffer_V_16x32;
		static IP_ARRAY_SIZE<32, 16>				IP_buffer_U_32x16;
		static IP_ARRAY_SIZE<32, 16>				IP_buffer_V_32x16;
		static IP_MAP_SCU_SIZE<16, 64>				IP_map_scu_16x64;
		static IP_MAP_SCU_SIZE<64, 16>				IP_map_scu_64x16;
		static IP_MAP_SCU_SIZE<32, 64>				IP_map_scu_32x64;
		static IP_MAP_SCU_SIZE<64, 32>				IP_map_scu_64x32;
		static IP_MAP_SCU_SIZE<8, 64>				IP_map_scu_8x64;
		static IP_MAP_SCU_SIZE<64, 8>				IP_map_scu_64x8;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	cu_data_best_c;
		static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	cu_data_temp_c[2];
		int x_pos_lcu = x0 & 63;
		int y_pos_lcu = y0 & 63;
		int x_pos_lcu_c = x_pos_lcu >> 1;
		int y_pos_lcu_c = y_pos_lcu >> 1;
		int cu_width_c = cu_width >> 1;
		int cu_height_c = cu_height >> 1;

		for (int i = 0; i < 2; i++)
		{
			cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_temp_c[i], prev_cu_data_temp_ptr, qt_depth + bet_depth);
		}

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_16x32.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_16x32.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_16x32.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_16x32.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_16x32.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_16x32.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_U_32x16.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 0, 0);
		//IP_buffer_ptr->get_up_line_chroma(IP_buffer_V_32x16.up_line, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, cu_width_c, 1, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_U_32x16.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 0, 0);
		//IP_buffer_ptr->get_left_line_chroma(IP_buffer_V_32x16.left_line, IP_buffer_ptr, x_pos_lcu_c, y_pos_lcu_c, cu_height_c, 1, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_U_32x16.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 0, 0);
		//IP_buffer_ptr->get_top_left_chroma(&IP_buffer_V_32x16.top_left, IP_buffer_ptr, x0 >> 1, y_pos_lcu_c, 1, 0);

		switch (split_mode_father)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 32 && cu_height == 64);
			switch (part_num)
			{
			case 0:
				for (int i = 0; i < (32 << 1); i++)
				{
					IP_buffer_32x64.up_line[i] = IP_buffer_ptr_64x64->up_line[i];
				}
				for (int i = 0; i < 64; i++)
				{
					IP_buffer_32x64.left_line[i] = IP_buffer_ptr_64x64->left_line[i];
				}
				IP_buffer_32x64.top_left = IP_buffer_ptr_64x64->top_left;
				break;
			case 1:
				for (int i = 0; i < (32 << 1); i++)
				{
					IP_buffer_32x64.up_line[i] = IP_buffer_ptr_64x64->up_line[i + 32];
				}
				for (int i = 0; i < 64; i++)
				{
					//IP_buffer_32x64.left_line[i] = IP_buffer_32x64.rec_line_ver[i];
					IP_buffer_32x64.left_line[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu - 1];
				}
				IP_buffer_32x64.top_left = IP_buffer_ptr_64x64->up_line[31];
				break;
			default:assert(0);
			}
			IP_buffer_U_16x32.fetch_bi_ver(part_num, IP_buffer_ptr_U_32x32);
			IP_buffer_V_16x32.fetch_bi_ver(part_num, IP_buffer_ptr_V_32x32);
			IP_map_scu_32x64.fetch_bi_ver(part_num, IP_map_scu_ptr_64x64);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 64 && cu_height == 32);
			switch (part_num)
			{
			case 0:
				for (int i = 0; i < (64 << 1); i++)
				{
					IP_buffer_64x32.up_line[i] = IP_buffer_ptr_64x64->up_line[i];
				}
				for (int i = 0; i < (32 << 1); i++)
				{
					IP_buffer_64x32.left_line[i] = IP_buffer_ptr_64x64->left_line[i];
				}
				IP_buffer_64x32.top_left = IP_buffer_ptr_64x64->top_left;

				if (y_pos_lcu == 0) {
					for (int i = 0; i < ((64 >> 2) << 1); i++)
					{
						IP_map_scu_64x32.map_scu_above_line[i] = IP_map_scu_ptr_64x64->map_scu_above_line[i];
					}
					IP_map_scu_64x32.map_scu_top_left = IP_map_scu_ptr_64x64->map_scu_top_left;
#if CDEBUG_MAP_SCU
					MAP_SCU_IP_ptr->get_map_scu_above_line(IP_map_scu_64x32.map_scu_above_line, MAP_SCU_IP_ptr, x0, y_pos_lcu, cu_width, 1);
					MAP_SCU_IP_ptr->get_map_scu_top_left(&IP_map_scu_64x32.map_scu_top_left, MAP_SCU_IP_ptr, x0, y_pos_lcu, cu_width, 1);
#endif // CDEBUG_MAP_SCU
				}
				break;
			case 1:
				for (int i = 0; i < 64; i++)
				{
					//IP_buffer_64x32.up_line[i] = IP_buffer_64x32.rec_line_hor[i];
					IP_buffer_64x32.up_line[i] = md_input_ptr->reco_y[y_pos_lcu - 1][i + x_pos_lcu];
				}
				for (int i = 0; i < 32; i++)
				{
					IP_buffer_64x32.left_line[i] = IP_buffer_ptr_64x64->left_line[i + 32];
				}
				IP_buffer_64x32.top_left = IP_buffer_ptr_64x64->left_line[31];
				break;
			default:
				assert(0);
			}
			IP_buffer_U_32x16.fetch_bi_hor(part_num, IP_buffer_ptr_U_32x32);
			IP_buffer_V_32x16.fetch_bi_hor(part_num, IP_buffer_ptr_V_32x32);
			IP_map_scu_64x32.fetch_bi_hor(part_num, IP_map_scu_ptr_64x64);
			break;
		default:assert(0);
		}

#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable && (1 << cu_width_log2) * (1 << cu_height_log2) >= md_input_ptr->cu_qp_group_area_size)
		{
			int flag1 = (x0 > 0 && ((IP_map_scu_ptr_64x64->map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 6) && (cu_height_log2 == 6);
			int flag2 = (x0 > 0 && ((IP_map_scu_32x64.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 5) && (cu_height_log2 == 6);
			int flag3 = (x0 > 0 && ((IP_map_scu_64x32.map_scu_left_line_new[0] >> 31 & (1)))) && (cu_width_log2 == 6) && (cu_height_log2 == 5);
			//if (flag1 || flag2 || flag3 || flag4 || flag5 || flag6 || flag7)
			//	md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
			if (flag1)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_ptr_64x64->map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag2)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_32x64.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else if (flag3)
			{
				md_input_ptr->cu_qp_group_pred_qp = ((IP_map_scu_64x32.map_scu_left_line_new[0]) >> 16) & 0x7F;
			}
			else
			{
				md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->md_slice_qp;//for debug
			}

		}
#endif
		if (best_split_mode_luma != NO_SPLIT) {
			md_64x16_16x64_loop_chroma(best_split_mode_luma,
				md_fw_ptr, md_input_ptr, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				split_struct.split_allow[part_num][best_split_mode_luma], x0, y0, cup, cu_width_log2, cu_height_log2, 0,
				qt_depth, bet_depth, cons_pred_mode,
				cu_width, cu_height, NS_luma_pred_mode,
				&cu_data_temp_c[best_index],
				&cons_pred_mode_child[best_index], tree_status,
				IP_buffer_ptr_64x64, &IP_buffer_32x64, &IP_buffer_64x32,
				&IP_buffer_U_16x32, &IP_buffer_V_16x32,
				&IP_buffer_U_32x16, &IP_buffer_V_32x16,
				IP_map_scu_ptr_64x64, &IP_map_scu_32x64, &IP_map_scu_64x32,
				Fetch_Ref_window_ptr, split_mode_father == NO_SPLIT ? 1 : 0, md_output_luma);

			copy_cu_data_array_src_chroma<32>(&cu_data_best_c,
				&cu_data_temp_c[best_index], 0, 0, cu_width_log2, cu_height_log2,
				cu_width_log2);
		}
		else {

			best_split_mode = best_split_mode_luma;
			best_cons_pred_mode = cons_pred_mode_child[best_index];
			best_tree_status_child = tree_status_child[best_index];

			if (best_split_mode != NO_SPLIT)
			{
				tree_status_temp = TREE_C;
			}

			md_chroma_32_chroma(cu_width >> 1, cu_height >> 1, md_fw_ptr,
				&cu_data_best_c,
				x0, y0, cu_width_log2, cu_height_log2, tree_status_temp, cud,
				md_input_ptr,
				p_fenc_LCU_U, p_fenc_LCU_V, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				((best_tree_status_child == TREE_L && tree_status == TREE_LC) || best_split_mode == NO_SPLIT), cup, cons_pred_mode,
				IP_buffer_ptr_U_32x32, IP_buffer_ptr_V_32x32,
				&IP_buffer_U_16x32, &IP_buffer_V_16x32,
				&IP_buffer_U_32x16, &IP_buffer_V_32x16,
				NULL, NULL,
				NULL, NULL,
				NULL, NULL,
				NULL, NULL,
				Fetch_Ref_window_ptr,
				IP_buffer_ptr_64x64,
				&IP_buffer_32x64, &IP_buffer_64x32,
				&IP_buffer_16x64, &IP_buffer_64x16,
				&IP_buffer_8x64, &IP_buffer_64x8,
				IP_map_scu_ptr_64x64,
				&IP_map_scu_32x64, &IP_map_scu_64x32,
				&IP_map_scu_16x64, &IP_map_scu_64x16,
				&IP_map_scu_8x64, &IP_map_scu_64x8
			);
		}

		switch (split_mode_father)
		{
		case NO_SPLIT:
			break;
		case SPLIT_BI_VER:
			assert(cu_width == 32 && cu_height == 64);
			switch (part_num)
			{
			case 0:
				for (int i = 0; i < 64; i++)
				{
					//IP_buffer_32x64.rec_line_ver[i] = md_input_ptr->reco_y[i * cu_width + (cu_width - 1)];
					//if (64 * i + x_pos_lcu + y_pos_lcu * 64 - 1 >= 0)
					IP_buffer_32x64.rec_line_ver[i] = md_input_ptr->reco_y[i + y_pos_lcu][x_pos_lcu + cu_width - 1];
				}
				break;
			case 1:
				break;
			default:
				assert(0);
			}
			IP_buffer_U_16x32.update_bi_ver(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_16x32.update_bi_ver(part_num, cu_data_best_c.reco_v);
			IP_map_scu_32x64.update_bi_ver(part_num, IP_map_scu_ptr_64x64->cur_map_scu, cu_data_best_c.map_scu);
			break;
		case SPLIT_BI_HOR:
			assert(cu_width == 64 && cu_height == 32);
			switch (part_num)
			{
			case 0:
				for (int i = 0; i < 64; i++)
				{
					//IP_buffer_64x32.rec_line_hor[i] = md_input_ptr->reco_y[cu_width * 31 + i];
					//if (i + x_pos_lcu + y_pos_lcu * 64 - 64 >= 0)
					IP_buffer_64x32.rec_line_hor[i] = md_input_ptr->reco_y[y_pos_lcu + (cu_height - 1)][i + x_pos_lcu];
				}
				break;
			case 1:
				break;
			default:
				assert(0);
			}
			IP_buffer_U_32x16.update_bi_hor(part_num, cu_data_best_c.reco_u);
			IP_buffer_V_32x16.update_bi_hor(part_num, cu_data_best_c.reco_v);
			IP_map_scu_64x32.update_bi_hor(part_num, IP_map_scu_ptr_64x64->cur_map_scu, cu_data_best_c.map_scu);
			break;
		default: assert(0);
		}

		prev_cu_data_temp_ptr->get_cu_data_from_curblk_c(&cu_data_best_c, x0 - split_struct.x_pos[0], y0 - split_struct.y_pos[0], cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
		//store_enc_info_best_chroma(md_fw_ptr, &curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
		//	md_input_ptr->pix_x, md_input_ptr->pix_y, prev_up_log2_sub_cuw, me_line_map_ptr, MAP_SCU_IP_ptr);
	}
	return 1;
}
#endif //!ONLY_QT_QT


int copy_cu_data_array_src_top_chroma(ENC_CU_DATA* dst, ENC_CU_DATA_ARRAY* src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
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
	U14 cx;
	U14 cy;
	U22 idx_dst;
	U22 idx_src;
	cx = (x >> 2);
	cy = (y >> 2);
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);


	for (j = 0; j < 16; j++) {
#pragma HLS PIPELINE
		idx_dst = (U22)((cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);

		for (i = 0; i < 16; i++)
		{
			if ((j < cuh_scu) && (i < cuw_scu))
			{
				(dst->ipm[1] + idx_dst)[i] = (src->ipm[1] + idx_src)[i];
				(dst->map_scu + idx_dst)[i] = (src->map_scu + idx_src)[i];
				(dst->num_nz_coef[1] + idx_dst)[i] = (src->num_nz_coef[1] + idx_src)[i];
				(dst->num_nz_coef[2] + idx_dst)[i] = (src->num_nz_coef[2] + idx_src)[i];
			}
			else
			{
				break;
			}
		}
	}

	for (j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		idx_dst = (U22)(((y >> 1) + j) << (log2_cus - 1)) + (x >> 1);
		idx_src = (U22)j << (cu_width_log2 - 1);
		for (i = 0; i < 32; i++)
		{
			if ((j < (cu_height >> 1)) && (i < (cu_width >> 1)))
			{
				(dst->coef_u + idx_dst)[i] = (src->coef_u + idx_src)[i];
				(dst->coef_v + idx_dst)[i] = (src->coef_v + idx_src)[i];
				(dst->reco_u + idx_dst)[i] = (src->reco_u + idx_src)[i];
				(dst->reco_v + idx_dst)[i] = (src->reco_v + idx_src)[i];
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}


void cost_compare_fixed_point_top_new_chroma(s64 cost_temp, ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, MD_RESULT* md_best_result_ptr, U2 cons_pred_mode_child, ENC_CU_DATA* md_best_ctu_output_ptr)
{
		copy_cu_data_array_src_top_chroma(md_best_ctu_output_ptr,
			curr_cu_data_temp_array_ptr, 0, 0, cu_width_log2, cu_height_log2,
			cu_width_log2, cud, tree_status);
		md_best_result_ptr->cost_best = cost_temp;
		md_best_result_ptr->best_split_mode = split_mode;
		md_best_result_ptr->best_cons_pred_mode = cons_pred_mode_child;
}

s64 md2_prepare_cu_info_COM_combine_top_chroma(MD_FW* md_fw_ptr, s64 lambdaY,
	U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width, U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct)
{
	md_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		md2_com_check_split_mode(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], &split_struct->num_split[i], split_mode);
	}

	if (split_mode == NO_SPLIT)
		return 0;

	s64 cost_temp = 0;
	//U3 rate_bit = 0;

	//clear_map_scu_chroma(me_line_map_ptr, MAP_SCU_IP_ptr, x0, y0, cu_width, cu_height, md_fw_ptr->pic_width,
	//	md_fw_ptr->pic_height);
	return cost_temp;
}

#if !CIRCUIT64_CHROMA_DUMMY
#if !ONLY_QT_QT
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
#pragma HLS INLINE off

	U13 x0 = md_input_ptr->pix_x;
	U13 y0 = md_input_ptr->pix_y;
	U3 cu_width_log2 = md_fw_ptr->log2_max_cuwh;
	U3 cu_height_log2 = md_fw_ptr->log2_max_cuwh;
	U8 cu_width = 1 << cu_width_log2;
	U8 cu_height = 1 << cu_height_log2;
	s64 cost_temp = MAX_S64;
	//U1 split_allow[5 + 1];
	static ENC_CU_DATA_ARRAY curr_cu_data_temp_array;
	static ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>	md2_cu_data_best_c;
	U3 part_num = 0;
	//U3 num_split_to_try = 0;
	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };
	U1 do_split = 1;

	md2_com_check_split_mode_top(x0, y0, cu_width, cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height, split_mode, &do_split);

	curr_cu_data_temp_array.last_child = md2_cu_data_best_c.last_child = TRUE;
	curr_cu_data_temp_array.x_chroma = md2_cu_data_best_c.x_chroma = x0;
	curr_cu_data_temp_array.y_chroma = md2_cu_data_best_c.y_chroma = y0;
	print_cu_split(x0, y0, cu_width, cu_height, TREE_LC, part_num, 1, curr_cu_data_temp_array.x_chroma, curr_cu_data_temp_array.y_chroma, curr_cu_data_temp_array.last_child, 0);
	//printf("\n");

	if (do_split) {
		COM_SPLIT_STRUCT split_struct;
		md2_prepare_cu_info_COM_combine_top_chroma(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, 0, 0,
			0, 0, cu_width, cu_height, split_mode, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS loop_tripcount max=2
			circuit_64x64_64x32_32x64_chroma(
				md_fw_ptr, md_input_ptr,
				RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
				p_fenc_LCU_U, p_fenc_LCU_V,
				&md2_cu_data_best_c,
				split_mode,
				0, split_mode != NO_SPLIT ? 1 : 0, cons_pred_mode_child[0], TREE_LC, part_num, split_struct, cu_width_log2,
				cu_height_log2,
				IP_buffer_ptr_64x64,
				IP_buffer_ptr_U_32x32,
				IP_buffer_ptr_V_32x32,
				IP_map_scu_ptr_64x64,
				Fetch_Ref_window_ptr, split_mode, md_output_luma);
		}
		cost_temp = 1;
		cu_data_cpy_c<32>(&curr_cu_data_temp_array, &md2_cu_data_best_c, cu_width, cu_height, 0);
		cost_compare_fixed_point_top_new_chroma(cost_temp, &curr_cu_data_temp_array, cu_width_log2, cu_height_log2, 0, TREE_LC,
			split_mode, md2_best_result_ptr, cons_pred_mode_child[0], md2_best_ctu_output_ptr);
	}
}
#endif //!ONLY_QT_QT
#endif //!CIRCUIT64_CHROMA_DUMMY

static int copy_cu_data_chroma(ENC_CU_DATA *dst, ENC_CU_DATA *src, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2], ENC_CU_DATA *src_luma)
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

	for (j = 0; j < cuh_scu; j++) {
#pragma HLS PIPELINE
		idx_dst = (U22)((j) << (log2_cus - 2));
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (k = cud; k < 5; k++) {
			for (i = 0; i < (NUM_BLOCK_SHAPE); i++) {
				for (ii = 0; ii < cuw_scu; ii++)(dst->split_mode[k][i] + idx_dst)[ii] = (src_luma->split_mode[k][i] + idx_src)[ii];
			}
		}
		for (ii = 0; ii < cuw_scu; ii++) {
			(dst->pred_mode + idx_dst)[ii] = (src_luma->pred_mode + idx_src)[ii];
			(dst->mpm[0] + idx_dst)[ii] = (src_luma->mpm[0] + idx_src)[ii];
			(dst->mpm[1] + idx_dst)[ii] = (src_luma->mpm[1] + idx_src)[ii];
			(dst->ipm[0] + idx_dst)[ii] = (src_luma->ipm[0] + idx_src)[ii];
			(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];
		}
		for (i = 0; i < 8; i++) {
			for (ii = 0; ii < cuw_scu; ii++)(dst->mpm_ext[i] + idx_dst)[ii] = (src_luma->mpm_ext[i] + idx_src)[ii];
		}
		for (ii = 0; ii < cuw_scu; ii++) {
			(dst->affine_flag + idx_dst)[ii] = (src_luma->affine_flag + idx_src)[ii];
			(dst->smvd_flag + idx_dst)[ii] = (src_luma->smvd_flag + idx_src)[ii];
			(dst->depth + idx_dst)[ii] = (src_luma->depth + idx_src)[ii];
			(dst->map_pb_tb_part + idx_dst)[ii] = (src_luma->map_pb_tb_part + idx_src)[ii];
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
			(dst->map_cu_mode + idx_dst)[ii] = (src_luma->map_cu_mode + idx_src)[ii];
		}
		for (ii = 0; ii < cuw_scu; ii++) {
			for (i = 0; i < 2; i++) {
				(dst->refi + idx_dst)[ii][i] = (src_luma->refi + idx_src)[ii][i];
			}
		}
#if !SIMPLE_MD
		for (ii = 0; ii < cuw_scu; ii++) {

			(dst->umve_flag + idx_dst)[ii] = (src_luma->umve_flag + idx_src)[ii];
			(dst->umve_idx + idx_dst)[ii] = (src_luma->umve_idx + idx_src)[ii];
			(dst->skip_idx + idx_dst)[ii] = (src_luma->skip_idx + idx_src)[ii];
			(dst->mvr_idx + idx_dst)[ii] = (src_luma->mvr_idx + idx_src)[ii];
			(dst->mvp_from_hmvp_flag + idx_dst)[ii] = (src_luma->mvp_from_hmvp_flag + idx_src)[ii];
			(dst->ipf_flag + idx_dst)[ii] = (src_luma->ipf_flag + idx_src)[ii];

			for (i1 = 0; i1 < 2; i1++) {
				for (i2 = 0; i2 < 2; i2++) {
					(dst->mv + idx_dst)[ii][i1][i2] = (src_luma->mv + idx_src)[ii][i1][i2];
					(dst->mvd + idx_dst)[ii][i1][i2] = (src_luma->mvd + idx_src)[ii][i1][i2];
				}

			}

		}
#endif
		for (i = 0; i < cuw_scu; i++)(dst->num_nz_coef[0] + idx_dst)[i] = (src_luma->num_nz_coef[0] + idx_src)[i];
		for (k = 1; k < 3; k++) {
			for (i = 0; i < cuw_scu; i++)(dst->num_nz_coef[k] + idx_dst)[i] = (src->num_nz_coef[k] + idx_src)[i];
		}

		for (i = 0; i < cuw_scu; i++) {
			(dst->pb_part + idx_dst)[i] = (src_luma->pb_part + idx_src)[i];
			(dst->tb_part + idx_dst)[i] = (src_luma->tb_part + idx_src)[i];
		}
	}
	for (j = 0; j < cu_height; j++) {
#pragma HLS UNROLL factor=4
#pragma HLS PIPELINE II=16
		idx_dst = ((U22)(y + j) << log2_cus) + x;
		idx_src = (U22)j << cu_width_log2;
		for (i = 0; i < cu_width; i++) {
			dst->coef_y[y + j][x + i] = src_luma->coef_y[j][i];
			dst->reco_y[y + j][x + i] = src_luma->reco_y[j][i];
		}
	}
	for (j = 0; j < (cu_height >> 1); j++) {
#pragma HLS PIPELINE
		idx_dst = (U22)(((y >> 1) + j) << (log2_cus - 1)) + (x >> 1);
		idx_src = (U22)j << (cu_width_log2 - 1);
		for (i = 0; i < (cu_width >> 1); i++) {
			(dst->coef_u + idx_dst)[i] = (src->coef_u + idx_src)[i];
			(dst->coef_v + idx_dst)[i] = (src->coef_v + idx_src)[i];
			(dst->reco_u + idx_dst)[i] = (src->reco_u + idx_src)[i];
			(dst->reco_v + idx_dst)[i] = (src->reco_v + idx_src)[i];
		}
	}
#if !SIMPLE_MD
	//add by xgq
	for (j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
#pragma HLS UNROLL
			dst->y_org_sao[j][i] = p_fenc_LCU_Y[j][i];
		}
	}

	for (j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		for (i = 0; i < 32; i++)
		{
#pragma HLS UNROLL
			dst->u_org_sao[j][i] = p_fenc_LCU_U[j][i];
			dst->v_org_sao[j][i] = p_fenc_LCU_V[j][i];
		}
	}
#endif
	return 0;
}


void write_back_inner_line_map_chroma(ENC_ME_LINE_MAP *dst, ENC_ME_LINE_MAP *src, U32 cur_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_LINE_MAP* me_line_map_ptr_input_luma, U32 cur_ctu_cu_mode_luma[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
	int i, j;

	for (i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			cur_ctu_scu[i][j] = src->map_scu_inner[src->curr_ctu_idx][i][j];
			//cur_ctu_cu_mode[i][j] = src->map_cu_mode_inner[src->curr_ctu_idx][i][j];
			cur_ctu_cu_mode[i][j] = cur_ctu_cu_mode_luma[i][j];

			//cur_ctu_scu[i][j] = me_line_map_ptr_input_luma->map_scu_inner[me_line_map_ptr_input_luma->curr_ctu_idx][i][j];
			//cur_ctu_cu_mode[i][j] = me_line_map_ptr_input_luma->map_cu_mode_inner[me_line_map_ptr_input_luma->curr_ctu_idx][i][j];

			/*dst->map_scu_inner[src->curr_ctu_idx][i][j] = src->map_scu_inner[src->curr_ctu_idx][i][j];
			dst->map_scu_inner[1 - src->curr_ctu_idx][i][j] = src->map_scu_inner[1 - src->curr_ctu_idx][i][j];*/

			//dst->map_cu_mode_inner[src->curr_ctu_idx][i][j] = me_line_map_ptr_input_luma->map_cu_mode_inner[src->curr_ctu_idx][i][j];
			//dst->map_cu_mode_inner[1 - src->curr_ctu_idx][i][j] = me_line_map_ptr_input_luma->map_cu_mode_inner[1 - src->curr_ctu_idx][i][j];
			/*dst->map_cu_mode_inner[0][i][j] = me_line_map_ptr_input_luma->map_cu_mode_inner[0][i][j];
			dst->map_cu_mode_inner[1][i][j] = me_line_map_ptr_input_luma->map_cu_mode_inner[1][i][j];*/
		}
	}
}

void Copy_LCU_To_Top_linebuffer_chroma(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, pel reco_u[(CU_SIZE << 4) >> 2], pel reco_v[(CU_SIZE << 4) >> 2],
	 pel dbk_u[39][39], pel dbk_v[39][39])
{
	//COM_INFO *info = &ctx->info;
	U13 lcu_x = md_input_ptr->pix_x;
	U13 lcu_y = md_input_ptr->pix_y;
	U8 lcuw = 1 << md_fw_ptr->log2_max_cuwh < md_fw_ptr->pic_width - lcu_x ? (U8)(1 << md_fw_ptr->log2_max_cuwh) : (U8)(md_fw_ptr->pic_width - lcu_x);
	U8 lcuh = 1 << md_fw_ptr->log2_max_cuwh < md_fw_ptr->pic_height - lcu_y ? (U8)(1 << md_fw_ptr->log2_max_cuwh) : (U8)(md_fw_ptr->pic_height - lcu_y);
	U8 j;


	for (int j = 0; j < 39; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 39; i++)
		{
			dbk_u[j][i] = 0;
			dbk_v[j][i] = 0;
		}
	}


	for (j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		for (U8 i = 0; i < 32; i++)
		{
			if(i < (lcuw >> 1) && j < (lcuh >> 1)){
				dbk_u[j + 6][i + 6] = reco_u[j * 32 + i];
  				dbk_v[j + 6][i + 6] = reco_v[j * 32 + i];
			}
		}   
	}
}

void store_cu_data_pixel_top_chroma(pel* reco_u, pel* reco_v, U13 x, U13 y, U7 w, U7 h, U13 pic_width,
	U13 pic_height, U2 tree_status, IP_ARRAY* IP_buffer_ptr)
{
	int i;
	int j;

	U7 w_temp = (x + w < pic_width ? w : (U7)(pic_width - x));
	U7 h_temp = (y + h < pic_height ? h : (U7)(pic_height - y));
	U7 stride = w;
	s8 log2_w;
	s8 log2_h;
	log2_w = (com_tbl_log2[w]);
	log2_h = (com_tbl_log2[h]);

	/* chroma */
	if (tree_status != TREE_L) {
		for (j = 0; j < (h_temp >> 1); j++) {
#pragma HLS LOOP_TRIPCOUNT max=32
			for (i = 0; i < (w_temp >> 1); i++) {
#pragma HLS LOOP_TRIPCOUNT max=32
#pragma HLS UNROLL factor=32
#pragma HLS PIPELINE
				IP_buffer_ptr->temp_buffer_chroma[0][j][i] = (reco_u[(j << (log2_w - 1)) + i]);
				IP_buffer_ptr->temp_buffer_chroma[1][j][i] = (reco_v[(j << (log2_w - 1)) + i]);
			}
		}
	}
}

static void update_map_scu_top_chroma(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, U13 x, U13 y,
	U2 tree_status, ENC_CU_DATA* md1_best_ctu_output_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
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

	x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	y_scu_line = scu_y - me_line_map_ptr->min_scu_y;


	for (i = 0; i < 16; i++)
	{
#pragma HLS PIPELINE
		src_map_ipm += src_cuw >> 2;
		for (j = 0; j < 16; j++)
		{
			if(i < h && j < w)
			{
				me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
					= md1_best_ctu_output_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
				me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
					i] = md1_best_ctu_output_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
			}
		}
	}
}

void update_to_ctx_map_top_chroma(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, 
	IP_MAP_SCU* MAP_SCU_IP_ptr, ENC_CU_DATA* md1_best_ctu_output_ptr)
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
	update_map_scu_top_chroma(md_fw_ptr, me_line_map_ptr, md_input_ptr->pix_x, md_input_ptr->pix_y,
		TREE_LC, md1_best_ctu_output_ptr, MAP_SCU_IP_ptr);
}

void store_enc_info_top_chroma(MD_FW *md_fw_ptr, MD_INPUT *md_input_ptr, ENC_ME_LINE_MAP *me_line_map_ptr,
	U12 cup,
	U4 cud, U2 cons_pred_mode, U7 cu_width, U7 cu_height, U2 tree_status, MD_RESULT md_best_result[2],
	ENC_CU_DATA md_best_ctu_output[2], IP_ARRAY *IP_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr,
	ENC_CU_DATA *md2_output, ENC_CU_DATA *ctu_map_cu_data_DBK, pel dbk_u[39][39], pel dbk_v[39][39], ENC_ME_LINE_MAP* me_line_map_ptr_tmp,
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_LINE_MAP* me_line_map_ptr_input,
	U32 cur_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_ME_LINE_MAP* me_line_map_ptr_input_luma, ENC_CU_DATA *src_luma, U32 cur_ctu_cu_mode_luma[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
	U13 x0 = md_input_ptr->pix_x;
	U13 y0 = md_input_ptr->pix_y;

	U1 best_index = md_best_result[0].cost_best < md_best_result[1].cost_best ? 0 : 1;

	store_cu_data_pixel_top_chroma(md_best_ctu_output[best_index].reco_u, md_best_ctu_output[best_index].reco_v,
		x0, y0, 64, 64, md_fw_ptr->pic_width, md_fw_ptr->pic_height, tree_status, IP_buffer_ptr);
	com_set_split_mode(md_best_result[best_index].best_split_mode, cud, 0, cu_width, cu_height, cu_width, md_best_ctu_output[best_index].split_mode);

	update_to_ctx_map_top_chroma(md_fw_ptr, md_input_ptr, me_line_map_ptr, MAP_SCU_IP_ptr, &md_best_ctu_output[best_index]);

	copy_cu_data_chroma(md2_output, &md_best_ctu_output[best_index], 0, 0, 6, 6, 6, 0, TREE_LC, p_fenc_LCU_Y, p_fenc_LCU_U, p_fenc_LCU_V, src_luma);
	copy_cu_data_chroma(ctu_map_cu_data_DBK, &md_best_ctu_output[best_index], 0, 0, 6, 6, 6, 0, TREE_LC, p_fenc_LCU_Y, p_fenc_LCU_U, p_fenc_LCU_V, src_luma);
	Copy_LCU_To_Top_linebuffer_chroma(md_fw_ptr, md_input_ptr, md2_output->reco_u, md2_output->reco_v, dbk_u, dbk_v);


	/* chroma */
	int chroma_x = x0 >> 1;
	for (int j = 0; j < 2; j++) {
		IP_buffer_ptr->top_left_chroma[j] = IP_buffer_ptr->up_chroma[j][chroma_x + 31];
		for (int i = 0; i < 32; i++) {
			IP_buffer_ptr->up_chroma[j][chroma_x + i] = IP_buffer_ptr->temp_buffer_chroma[j][31][i];
		}
		for (int i = 0; i < 32; i++) {
			IP_buffer_ptr->left_chroma[j][i] = IP_buffer_ptr->temp_buffer_chroma[j][i][31];
		}
	}

	//write_back_inner_nebs(me_line_map_ptr_tmp, inner_nebs, cur_ctu_mi);
	write_back_inner_line_map_chroma(me_line_map_ptr_input, me_line_map_ptr_tmp, cur_ctu_scu, cur_ctu_cu_mode, me_line_map_ptr_input_luma, cur_ctu_cu_mode_luma);

	//memcpy(me_line_map_ptr_input, me_line_map_ptr_input_luma, sizeof(ENC_ME_LINE_MAP));
}

