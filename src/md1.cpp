#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "md1.h"
#include "md_kernel_8.h"
#include "md_kernel_16.h"
#include "md_kernel_32.h"

U12 MD_COM_MODE::x_chroma;
U12 MD_COM_MODE::y_chroma;
BOOL MD_COM_MODE::last_child;
#define GET_I_COST(rate, lamba)  (rate*lamba)

#ifdef DEBUG_PRINT_MD_COST
int g_cost_idx = 0;
#endif

#if CUDQP_QP_MAP
int md_com_is_cu_dqp(COM_INFO* info)
{
	return !info->shext.fixed_slice_qp_flag && info->pic_header.cu_delta_qp_flag;
}
#endif

static U4 get_colocal_scu_len(const U10 cur, const U10 pic_size_in_scu)
{
#pragma HLS INLINE
	const U10 mask = (-1) ^ 3;
	const U10 col = (cur & mask) + 2;
	const U10 result = col >= pic_size_in_scu ? (U10)((cur & mask) + pic_size_in_scu) >> 1 : col;
	return result & (MAX_CU_IN_SCU - 1);
}
void get_skip_in(SKIP_MERGE_INPUT_16* dst, SKIP_MERGE_INPUT* src,
	int x_pos_pic, int y_pos_pic, int pic_width, int pic_height)
{
	const U4 co_x_scu_of_ctu = get_colocal_scu_len(x_pos_pic >> 2, pic_width >> 2);
	const U4 co_y_scu_of_ctu = get_colocal_scu_len(y_pos_pic >> 2, pic_height >> 2);

	dst->slice_type = src->slice_type;
	dst->ptr = src->ptr;
	dst->umve_enable_flag = src->umve_enable_flag;
	dst->pix_x = src->pix_x;
#if ENABLE_BFRAME
	for (int i = 0; i < REFP_NUM; i++)
	{
#pragma HLS pipeline
		for (int j = 0; j < MV_D; j++)
		{
			dst->refp_map_mv[i][j] = src->refp_map_mv1[i][co_y_scu_of_ctu][co_x_scu_of_ctu][j];
		}
		for (int k = 0; k < REFP_NUM; k++)
		{
			dst->refp_map_refi[i][k] = src->refp_map_refi1[i][co_y_scu_of_ctu][co_x_scu_of_ctu][k];

		}
	}
	cpy_2d_2_2d<int, REFP_NUM, MAX_NUM_REF_PICS>(src->refp_list_ptr1, dst->refp_list_ptr1);
	cpy_2d_2_2d<int, MAX_NUM_REF_PICS, REFP_NUM>(src->refp_ptr1, dst->refp_ptr1);

#else
	for (int j = 0; j < MV_D; j++)
	{
		dst->refp_mv[j] = src->refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][j];
	}
	dst->refp_refi = src->refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu];
#endif // ENABLE_BFRAME
}

void compare_store_top(MD_FW* md_fw_ptr,
	s64& cost_best, s64 cost,
	ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_ptr_32, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_best_ptr_32,
	ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr2,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2 cons_pred_mode_child, U2 tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
	if ((5 == cu_width_log2) && (5 == cu_height_log2))
	{
		copy_cu_data_array_src<32>(curr_cu_data_best_ptr2, curr_cu_data_ptr_32, 0, 0,
			cu_width_log2, cu_height_log2, cu_width_log2);
		*best_split_mode = split_mode;
		*best_cons_pred_mode = cons_pred_mode_child;
		*best_child_status = tree_status_child;
	}
	else
	{
		if (cost_best > cost)
		{
			cost_best = cost;

			copy_cu_data_array_src<16>(curr_cu_data_best_ptr, curr_cu_data_ptr, 0, 0,
				cu_width_log2, cu_height_log2, cu_width_log2);

			*best_split_mode = split_mode;
			*best_cons_pred_mode = cons_pred_mode_child;
			*best_child_status = tree_status_child;
		}
	}

	if (*best_split_mode != NO_SPLIT)
	{
		*tree_status_temp = TREE_C;
	}
}


void fetch_port_16_md2(
	// input
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif // ENABLE_BFRAME
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	SKIP_MERGE_INPUT* skip_in,
	ENC_FME_INFO_ALL* fme_mv_info,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	// Supplementary information
	int x_pos_pic, int y_pos_pic, int cu_width, int cu_height, int pic_width, int pic_height,
	// output
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir_16,
#endif // ENABLE_BFRAME
	pel org_y_16[16][16],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	SKIP_MERGE_INPUT_16* skip_in_16,
	ENC_FME_INFO_ALL_16* fme_mv_info_16,
	strFetch_ref_window_16* Fetch_Ref_window_ptr_16
)
{
#if ENABLE_BFRAME
	cpy_2d_2_2d<MV_DIR, 4, 4>(me_mv_dir->block16x16MvDir, me_mv_dir_16->block16x16MvDir);
	cpy_2d_2_2d<MV_DIR, 4, 8>(me_mv_dir->block16x8MvDir, me_mv_dir_16->block16x8MvDir);
	cpy_2d_2_2d<MV_DIR, 8, 4>(me_mv_dir->block8x16MvDir, me_mv_dir_16->block8x16MvDir);
#endif // ENABLE_BFRAME

    for (int i = 0; i < cu_height; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
#pragma HLS pipeline
        for (int j = 0; j < cu_width; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
            org_y_16[i][j] = p_fenc_LCU_Y[((y_pos_pic & 63) + i) * 64 + ((x_pos_pic & 63) + j)];
        }
    }

	get_me_line_map_up(me_line_map_ptr_16->map_scu_up_line, me_line_map_ptr_16->map_cu_mode_up_line, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);
	get_me_line_map_left(me_line_map_ptr_16->map_scu_left_line, me_line_map_ptr_16->map_cu_mode_left_line, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);
	me_line_map_ptr_16->min_scu_x = me_line_map_ptr->min_scu_x;
	me_line_map_ptr_16->min_scu_y = me_line_map_ptr->min_scu_y;
	me_line_map_ptr_16->above_line_idx = me_line_map_ptr->above_line_idx;
	me_line_map_ptr_16->curr_ctu_idx = me_line_map_ptr->curr_ctu_idx;

	//get_up_nebs_line(up_nebs, above_nebs_line, inner_nebs, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);
	//get_left_nebs_line(left_nebs, inner_nebs, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);

	get_skip_in(skip_in_16, skip_in, x_pos_pic, y_pos_pic, pic_width, pic_height);

	cpy_3d_2_3d<S14, 4, 4, 2>(fme_mv_info->sadTree16x16Fmv, fme_mv_info_16->sadTree16x16Fmv);
	cpy_3d_2_3d<S14, 4, 8, 2>(fme_mv_info->sadTree16x8Fmv, fme_mv_info_16->sadTree16x8Fmv);
	cpy_3d_2_3d<S14, 8, 4, 2>(fme_mv_info->sadTree8x16Fmv, fme_mv_info_16->sadTree8x16Fmv);

#if ENABLE_BFRAME
	Fetch_Ref_window_ptr_16->code_stru = Fetch_Ref_window_ptr->code_stru;
	cpy_1d_2_1d<U1, REFP_NUM>(Fetch_Ref_window_ptr->lidx, Fetch_Ref_window_ptr_16->lidx);
	cpy_1d_2_1d<S8, REFP_NUM>(Fetch_Ref_window_ptr->cur_refi, Fetch_Ref_window_ptr_16->cur_refi);
	cpy_2d_2_2d<int, MAX_NUM_REF_PICS, REFP_NUM>(Fetch_Ref_window_ptr->refp_pic_ptr, Fetch_Ref_window_ptr_16->refp_pic_ptr);
	cpy_2d_2_2d<int, MAX_NUM_REF_PICS, REFP_NUM>(Fetch_Ref_window_ptr->refp_is_library_picture, Fetch_Ref_window_ptr_16->refp_is_library_picture);
#endif // ENABLE_BFRAME
}

void copy_org_y_16(
	// input
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	// Supplementary information
	int x_pos_pic, int y_pos_pic, int cu_width, int cu_height,
	// output
	pel org_y_16[16][16]
)
{
	#pragma HLS ARRAY_PARTITION variable=org_y_16 complete dim=1 
  
	#pragma HLS ARRAY_PARTITION variable=p_fenc_LCU_Y complete dim=1 
  
    for (int j = 0; j < 16; j++) {
#pragma HLS pipeline
        for (int i = 0; i < 16; i++) {
            org_y_16[i][j] = p_fenc_LCU_Y[((y_pos_pic & 63) + i)][ ((x_pos_pic & 63) + j)];
        }
    }
}
void set_code_stru(
	// input
	strFetch_ref_window* Fetch_Ref_window_ptr,
	// output
	strFetch_ref_window_16* Fetch_Ref_window_ptr_16
)
{
#if ENABLE_BFRAME
#pragma HLS INLINE off
	Fetch_Ref_window_ptr_16->code_stru = Fetch_Ref_window_ptr->code_stru;
#endif // ENABLE_BFRAME
}
void fetch_port_16_md2_tst(
	// input
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif // ENABLE_BFRAME
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	SKIP_MERGE_INPUT* skip_in,
	ENC_FME_INFO_ALL* fme_mv_info,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	// Supplementary information
	int x_pos_pic, int y_pos_pic, int cu_width, int cu_height, int pic_width, int pic_height,
	// output
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir_16,
#endif // ENABLE_BFRAME
	pel org_y_16[16][16],
	SKIP_MERGE_INPUT_16* skip_in_16,
	ENC_FME_INFO_ALL_16* fme_mv_info_16,
	strFetch_ref_window_16* Fetch_Ref_window_ptr_16
)
{
//#pragma HLS ARRAY_PARTITION variable=org_y_16 complete dim=0

#pragma HLS ARRAY_PARTITION variable=org_y_16 complete dim=1 
#pragma HLS ARRAY_PARTITION variable=p_fenc_LCU_Y complete dim=1 

//#pragma HLS ARRAY_PARTITION variable=fme_mv_info->sadTree16x16Fmv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info->sadTree16x16Fmv complete dim=3
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info_16->sadTree16x16Fmv complete dim=0
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info->sadTree16x8Fmv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info->sadTree16x8Fmv complete dim=3
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info_16->sadTree16x8Fmv complete dim=0
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info->sadTree8x16Fmv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info->sadTree8x16Fmv complete dim=3
//#pragma HLS ARRAY_PARTITION variable=fme_mv_info_16->sadTree8x16Fmv complete dim=0
   /* for (int i = 0; i < 16; i++) {
#pragma HLS pipeline
        for (int j = 0; j < 16; j++) {
            org_y_16[i][j] = p_fenc_LCU_Y[((y_pos_pic & 63) + i)][ ((x_pos_pic & 63) + j)];
        }
		if (i < 4) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 2; k++) {
					fme_mv_info_16->sadTree16x16Fmv[i][j][k] = fme_mv_info->sadTree16x16Fmv[i][j][k];
				}
			}
			for (int j = 0; j < 8; j++) {
				for (int k = 0; k < 2; k++) {
					fme_mv_info_16->sadTree16x8Fmv[i][j][k] = fme_mv_info->sadTree16x8Fmv[i][j][k];
				}
			}
			
		}
		if (i < 8) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 2; k++) {
					fme_mv_info_16->sadTree8x16Fmv[i][j][k] = fme_mv_info->sadTree8x16Fmv[i][j][k];
				}
			}
		}
    }*/
	//get_me_line_map_up(me_line_map_ptr_16->map_scu_up_line, me_line_map_ptr_16->map_cu_mode_up_line, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);
	//get_me_line_map_left(me_line_map_ptr_16->map_scu_left_line, me_line_map_ptr_16->map_cu_mode_left_line, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);
	//me_line_map_ptr_16->min_scu_x = me_line_map_ptr->min_scu_x;
	//me_line_map_ptr_16->min_scu_y = me_line_map_ptr->min_scu_y;
	//me_line_map_ptr_16->above_line_idx = me_line_map_ptr->above_line_idx;
	//me_line_map_ptr_16->curr_ctu_idx = me_line_map_ptr->curr_ctu_idx;

	//get_up_nebs_line(up_nebs, above_nebs_line, inner_nebs, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);
	//get_left_nebs_line(left_nebs, inner_nebs, me_line_map_ptr, cu_width, cu_height, x_pos_pic, y_pos_pic);


	const U4 co_x_scu_of_ctu = get_colocal_scu_len(x_pos_pic >> 2, pic_width >> 2);
	const U4 co_y_scu_of_ctu = get_colocal_scu_len(y_pos_pic >> 2, pic_height >> 2);

	skip_in_16->slice_type = skip_in->slice_type;
	skip_in_16->ptr = skip_in->ptr;
	skip_in_16->umve_enable_flag = skip_in->umve_enable_flag;
	skip_in_16->pix_x = skip_in->pix_x;

#pragma HLS ARRAY_PARTITION variable=skip_in->refp_list_ptr1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=skip_in_16->refp_list_ptr1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=skip_in->refp_ptr1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=skip_in_16->refp_ptr1 complete dim=0

// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->refp_pic_ptr complete dim=2
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr_16->refp_pic_ptr complete dim=0
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->refp_is_library_picture complete dim=2
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr_16->refp_is_library_picture complete dim=0
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr_16->lidx complete dim=0
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->lidx complete dim=0
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr_16->cur_refi complete dim=0
// #pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->cur_refi complete dim=0

#pragma HLS ARRAY_PARTITION variable=me_mv_dir->block16x16MvDir complete dim=2
#pragma HLS ARRAY_PARTITION variable=me_mv_dir_16->block16x16MvDir complete dim=0
#pragma HLS ARRAY_PARTITION variable=me_mv_dir->block16x8MvDir complete dim=2
#pragma HLS ARRAY_PARTITION variable=me_mv_dir_16->block16x8MvDir complete dim=0
#pragma HLS ARRAY_PARTITION variable=me_mv_dir->block8x16MvDir complete dim=2
#pragma HLS ARRAY_PARTITION variable=me_mv_dir_16->block8x16MvDir complete dim=0

#pragma HLS ARRAY_PARTITION variable=skip_in->refp_map_mv1 complete dim=4
#pragma HLS ARRAY_PARTITION variable=skip_in->refp_map_refi1 complete dim=4
#pragma HLS ARRAY_PARTITION variable=skip_in->refp_ptr1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=skip_in_16->refp_ptr1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=skip_in->refp_ptr1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=skip_in_16->refp_ptr1 complete dim=1
	for (int i = 0; i < 17; i++) {
#pragma HLS pipeline
		if (i < 16) {
			for (int j = 0; j < 16; j++) {
				org_y_16[j][i] = p_fenc_LCU_Y[((y_pos_pic & 63) + j)][((x_pos_pic & 63) + i)];
			}
			if (i < 4) {
				for (int j = 0; j < 4; j++) {
					for (int k = 0; k < 2; k++) {
						fme_mv_info_16->sadTree16x16Fmv[i][j][k] = fme_mv_info->sadTree16x16Fmv[i][j][k];
					}
				}
				for (int j = 0; j < 8; j++) {
					for (int k = 0; k < 2; k++) {
						fme_mv_info_16->sadTree16x8Fmv[i][j][k] = fme_mv_info->sadTree16x8Fmv[i][j][k];
					}
				}

			}
			if (i < 8) {
				for (int j = 0; j < 4; j++) {
					for (int k = 0; k < 2; k++) {
						fme_mv_info_16->sadTree8x16Fmv[i][j][k] = fme_mv_info->sadTree8x16Fmv[i][j][k];
					}
				}
			}
		}
#if !ENABLE_BFRAME
	}
#endif
#if ENABLE_BFRAME
		for (int j = 0; j < 2; j++) {
			skip_in_16->refp_ptr1[i][j] = skip_in->refp_ptr1[i][j];
			Fetch_Ref_window_ptr_16->refp_pic_ptr[i][j] = Fetch_Ref_window_ptr->refp_pic_ptr[i][j];
			Fetch_Ref_window_ptr_16->refp_is_library_picture[i][j] = Fetch_Ref_window_ptr->refp_is_library_picture[i][j];
		}
		if (i < 2) {
			for (int j = 0; j < 17; j++) {
				skip_in_16->refp_list_ptr1[i][j] = skip_in->refp_list_ptr1[i][j];
			}
			Fetch_Ref_window_ptr_16->lidx[i] = Fetch_Ref_window_ptr->lidx[i];
			Fetch_Ref_window_ptr_16->cur_refi[i] = Fetch_Ref_window_ptr->cur_refi[i];
			
			for (int j = 0; j < MV_D; j++)
			{
				skip_in_16->refp_map_mv[i][j] = skip_in->refp_map_mv1[i][co_y_scu_of_ctu][co_x_scu_of_ctu][j];
			}
			for (int k = 0; k < REFP_NUM; k++)
			{
				skip_in_16->refp_map_refi[i][k] = skip_in->refp_map_refi1[i][co_y_scu_of_ctu][co_x_scu_of_ctu][k];

			}
			
		}
		if (i < 8) {
			for (int j = 0; j < 4; j++) {
				me_mv_dir_16->block8x16MvDir[i][j] = me_mv_dir->block8x16MvDir[i][j];
			}
			if (i < 4) {
				for (int j = 0; j < 4; j++) {
					me_mv_dir_16->block16x16MvDir[i][j] = me_mv_dir->block16x16MvDir[i][j];
				}
				for (int j = 0; j < 8; j++) {
					me_mv_dir_16->block16x8MvDir[i][j] = me_mv_dir->block16x8MvDir[i][j];
				}
			}
		}
	}
	set_code_stru(
		Fetch_Ref_window_ptr,
		Fetch_Ref_window_ptr_16
	);
	
#else
	for (int j = 0; j < MV_D; j++)
	{
		skip_in_16->refp_mv[j] = skip_in->refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][j];
	}
	skip_in_16->refp_refi = skip_in->refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu];


   
#endif // ENABLE_BFRAME
}

//void cu_data_cpy_16(ENC_CU_DATA_ARRAY_INNER* src, ENC_CU_DATA_ARRAY_INNER_16* dst, int cu_width, int cu_height, int cdebug)
//{
//#pragma HLS INLINE off
//	int w_scu = cu_width >> 2;
//	int h_scu = cu_height >> 2;
//	if (cdebug)
//	{
//		for (int i = 0; i < cu_height; i++)
//		{
//			for (int j = 0; j < cu_width; j++)
//			{
//				assert(src->coef_y[i * cu_width + j] == dst->coef_y[i][j]);
//				assert(src->reco_y[i * cu_width + j] == dst->reco_y[i][j]);
//			}
//		}
//		for (int i = 0; i < h_scu; i++)
//		{
//			for (int j = 0; j < w_scu; j++)
//			{
//				for (int m = 0; m < 2; m++)
//				{
//					assert(src->refi[i * w_scu + j][m] == dst->refi[i * w_scu + j][m]);
//					for (int n = 0; n < 2; n++)
//					{
//						assert(src->mv[i * w_scu + j][m][n] == dst->mv[i * w_scu + j][m][n]);
//						assert(src->mvd[i * w_scu + j][m][n] == dst->mvd[i * w_scu + j][m][n]);
//					}
//				}
//				assert(src->pb_part[i * w_scu + j] == dst->pb_part[i * w_scu + j]);
//				assert(src->tb_part[i * w_scu + j] == dst->tb_part[i * w_scu + j]);
//				assert(src->pred_mode[i * w_scu + j] == dst->pred_mode[i * w_scu + j]);
//				assert(src->mvr_idx[i * w_scu + j] == dst->mvr_idx[i * w_scu + j]);
//				assert(src->umve_flag[i * w_scu + j] == dst->umve_flag[i * w_scu + j]);
//				assert(src->umve_idx[i * w_scu + j] == dst->umve_idx[i * w_scu + j]);
//				assert(src->skip_idx[i * w_scu + j] == dst->skip_idx[i * w_scu + j]);
//				assert(src->map_scu[i * w_scu + j] == dst->map_scu[i * w_scu + j]);
//				assert(src->affine_flag[i * w_scu + j] == dst->affine_flag[i * w_scu + j]);
//				assert(src->smvd_flag[i * w_scu + j] == dst->smvd_flag[i * w_scu + j]);
//				assert(src->map_cu_mode[i * w_scu + j] == dst->map_cu_mode[i * w_scu + j]);
//				assert(src->map_pb_tb_part[i * w_scu + j] == dst->map_pb_tb_part[i * w_scu + j]);
//				assert(src->depth[i * w_scu + j] == dst->depth[i * w_scu + j]);
//				assert(src->ipf_flag[i * w_scu + j] == dst->ipf_flag[i * w_scu + j]);
//				assert(src->mvp_from_hmvp_flag[i * w_scu + j] == dst->mvp_from_hmvp_flag[i * w_scu + j]);
//			}
//		}
//		for (int k = 0; k < 2; k++)
//		{
//			for (int i = 0; i < h_scu; i++)
//			{
//				for (int j = 0; j < w_scu; j++)
//				{
//					/*assert(src->mpm[k][i * w_scu + j] == dst->mpm[k][i * w_scu + j]);
//					assert(src->ipm[k][i * w_scu + j] == dst->ipm[k][i * w_scu + j]);*/
//				}
//			}
//		}
//		for (int k = 0; k < 8; k++)
//		{
//			for (int i = 0; i < h_scu; i++)
//			{
//				for (int j = 0; j < w_scu; j++)
//				{
//					assert(src->mpm_ext[k][i * w_scu + j] == dst->mpm_ext[k][i * w_scu + j]);
//				}
//			}
//		}
//		for (int k = 0; k < 1; k++)
//		{
//			for (int i = 0; i < h_scu; i++)
//			{
//				for (int j = 0; j < w_scu; j++)
//				{
//					assert(src->num_nz_coef[k][i * w_scu + j] == dst->num_nz_coef[k][i * w_scu + j]);
//				}
//			}
//		}
//		assert(src->x_chroma == dst->x_chroma);
//		assert(src->y_chroma == dst->y_chroma);
//		assert(src->x_last == dst->x_last);
//		assert(src->y_last == dst->y_last);
//		assert(src->last_child == dst->last_child);
//		for (int i = 0; i < cu_height; i++)
//		{
//			for (int j = 0; j < cu_width; j++)
//			{
//				assert(src->coef_y[i * cu_width + j] == dst->coef_y[i][j]);
//				assert(src->reco_y[i * cu_width + j] == dst->reco_y[i][j]);
//			}
//		}
//		//for (int i = 0; i < (cu_height >> 1); i++)
//		//{
//		//	for (int j = 0; j < (cu_width >> 1); j++)
//		//	{
//		//		assert(src->coef_u[i * (cu_width >> 1) + j] == dst->coef_u[i][j]);
//		//		assert(src->coef_v[i * (cu_width >> 1) + j] == dst->coef_v[i][j]);
//		//		assert(src->reco_u[i * (cu_width >> 1) + j] == dst->reco_u[i][j]);
//		//		assert(src->reco_v[i * (cu_width >> 1) + j] == dst->reco_v[i][j]);
//		//	}
//		//}
//	}
//	else
//	{
//		for (int i = 0; i < h_scu; i++)
//		{
//			for (int j = 0; j < w_scu; j++)
//			{
//				for (int m = 0; m < 2; m++)
//				{
//					src->refi[i * w_scu + j][m] = dst->refi[i * w_scu + j][m];
//					for (int n = 0; n < 2; n++)
//					{
//						src->mv[i * w_scu + j][m][n] = dst->mv[i * w_scu + j][m][n];
//						src->mvd[i * w_scu + j][m][n] = dst->mvd[i * w_scu + j][m][n];
//					}
//				}
//				src->pb_part[i * w_scu + j] = dst->pb_part[i * w_scu + j];
//				src->tb_part[i * w_scu + j] = dst->tb_part[i * w_scu + j];
//				src->pred_mode[i * w_scu + j] = dst->pred_mode[i * w_scu + j];
//				src->mvr_idx[i * w_scu + j] = dst->mvr_idx[i * w_scu + j];
//				src->umve_flag[i * w_scu + j] = dst->umve_flag[i * w_scu + j];
//				src->umve_idx[i * w_scu + j] = dst->umve_idx[i * w_scu + j];
//				src->skip_idx[i * w_scu + j] = dst->skip_idx[i * w_scu + j];
//				src->map_scu[i * w_scu + j] = dst->map_scu[i * w_scu + j];
//				src->affine_flag[i * w_scu + j] = dst->affine_flag[i * w_scu + j];
//				src->smvd_flag[i * w_scu + j] = dst->smvd_flag[i * w_scu + j];
//				src->map_cu_mode[i * w_scu + j] = dst->map_cu_mode[i * w_scu + j];
//				src->map_pb_tb_part[i * w_scu + j] = dst->map_pb_tb_part[i * w_scu + j];
//				src->depth[i * w_scu + j] = dst->depth[i * w_scu + j];
//				src->ipf_flag[i * w_scu + j] = dst->ipf_flag[i * w_scu + j];
//				src->mvp_from_hmvp_flag[i * w_scu + j] = dst->mvp_from_hmvp_flag[i * w_scu + j];
//			}
//		}
//		for (int k = 0; k < 2; k++)
//		{
//			for (int i = 0; i < h_scu; i++)
//			{
//				for (int j = 0; j < w_scu; j++)
//				{
//					src->mpm[k][i * w_scu + j] = dst->mpm[k][i * w_scu + j];
//					src->ipm[k][i * w_scu + j] = dst->ipm[k][i * w_scu + j];
//				}
//			}
//		}
//		for (int k = 0; k < 8; k++)
//		{
//			for (int i = 0; i < h_scu; i++)
//			{
//				for (int j = 0; j < w_scu; j++)
//				{
//					src->mpm_ext[k][i * w_scu + j] = dst->mpm_ext[k][i * w_scu + j];
//				}
//			}
//		}
//		for (int k = 0; k < 1; k++)
//		{
//			for (int i = 0; i < h_scu; i++)
//			{
//				for (int j = 0; j < w_scu; j++)
//				{
//					src->num_nz_coef[k][i * w_scu + j] = dst->num_nz_coef[k][i * w_scu + j];
//				}
//			}
//		}
//		src->x_chroma = dst->x_chroma;
//		src->y_chroma = dst->y_chroma;
//		src->x_last = dst->x_last;
//		src->y_last = dst->y_last;
//		src->last_child = dst->last_child;
//		for (int i = 0; i < cu_height; i++)
//		{
//			for (int j = 0; j < cu_width; j++)
//			{
//				src->coef_y[i * cu_width + j] = dst->coef_y[i][j];
//				src->reco_y[i * cu_width + j] = dst->reco_y[i][j];
//			}
//		}
//		//for (int i = 0; i < (cu_height >> 1); i++)
//		//{
//		//	for (int j = 0; j < (cu_width >> 1); j++)
//		//	{
//		//		src->coef_u[i * (cu_width >> 1) + j] = dst->coef_u[i][j];
//		//		src->coef_v[i * (cu_width >> 1) + j] = dst->coef_v[i][j];
//		//		src->reco_u[i * (cu_width >> 1) + j] = dst->reco_u[i][j];
//		//		src->reco_v[i * (cu_width >> 1) + j] = dst->reco_v[i][j];
//		//	}
//		//}
//	}
//}

void clear_map_scu_line(U32* line, U8 line_num)
{
	for (int i = 0; i < line_num; i++)
		line[i] = 0;
}
template<u8 W_scu, u8 H_scu>
void clear_intermedia_map_scu(U32 cur_map_scu[W_scu][H_scu])
{
	for (int i = 0; i < H_scu; i++)
#pragma HLS pipeline
		for (int j = 0; j < W_scu; j++)
			cur_map_scu[j][i] = 0;
}

void print_cu_split(int x, int y, int w, int h, int tree, int num, int part, int xc, int yc, int last, int depth)
{
	return;
	while (depth--){
		printf("\t");
	}
	//printf("%d: ", depth);
	printf("cu=(%d %d %d %d), tree=%s num=%d/%d, chroma=(%d %d), use=%s",
		x, y, w, h,
		tree == 0 ? "LC" : (tree == 1 ? "L" : "C"), num, part,
		xc, yc,
		last ? "true" : "false");
}

void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY* src, int depth)
{
	//for (int i = 0; i < size; ++i){
		if (tree == TREE_LC){
			dst->x_chroma = x;
			dst->y_chroma = y;
			dst->x_last = x + w - 4;
			dst->y_last = y + h - 4;
			dst->last_child = FALSE;
		}
		else{
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


S7 get_ip_array_size_idx(U8 cu_width, U8 cu_height){
	S7 ip_array_size_idx[16][16] = {
		{ 0, 4, -1, 9, -1, -1, -1, 14, -1, -1, -1, -1, -1, -1, -1 },	
		{ 1, 5, -1, 10, -1, -1, -1, 15, -1, -1, -1, -1, -1, -1, 19 },	
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 2, 6, -1, 11, -1, -1, -1, 16, -1, -1, -1, -1, -1, -1, 20 },	
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ 3, 7, -1, 12, -1, -1, -1, 17, -1, -1, -1, -1, -1, -1, 21 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
		{ -1, 8, -1, 13, -1, -1, -1, 18, -1, -1, -1, -1, -1, -1, 22 }
	};
	return ip_array_size_idx[(cu_width >> 2) - 1][(cu_height >> 2) - 1];
}

void print_s64(const s64& val)
{
#ifdef LINUX
	printf("%ld", val);
#elif defined(WIN32)
	printf("%lld", val);
#else
	assert(0);
#endif
}

void copy_md_fw(MD_FW *tmp, MD_FW *dst){
	dst->pic_width = tmp->pic_width;
	dst->pic_height = tmp->pic_height;
	dst->slice_type = tmp->slice_type;
	dst->max_dt_size = tmp->max_dt_size;
	dst->dt_intra_enable_flag = tmp->dt_intra_enable_flag;//
	dst->num_of_hmvp_cand = tmp->num_of_hmvp_cand;
	dst->ipf_enable_flag = tmp->ipf_enable_flag;
	dst->pic_width_in_scu = tmp->pic_width_in_scu;
	dst->pic_height_in_scu = tmp->pic_height_in_scu;
	dst->log2_max_cuwh = tmp->log2_max_cuwh;
	dst->log2_culine = tmp->log2_culine;
	dst->max_cuwh = tmp->max_cuwh;
	dst->f_scu = tmp->f_scu;
	/* gop size */
	dst->gop_size = tmp->gop_size;
#if CTX_CONTROL_LUKE
	dst->bit_depth_internal = tmp->bit_depth_internal;
	dst->max_part_ratio = tmp->max_part_ratio;
	dst->min_cu_size = tmp->min_cu_size;
	dst->max_split_times = tmp->max_split_times;
	dst->min_qt_size = tmp->min_qt_size;
	dst->max_bt_size = tmp->max_bt_size;
	dst->max_eqt_size = tmp->max_eqt_size;
	dst->affine_subblock_size_idx = tmp->affine_subblock_size_idx;
	dst->ipcm_enable_flag = tmp->ipcm_enable_flag;
	dst->position_based_transform_enable_flag = tmp->position_based_transform_enable_flag;
	dst->secondary_transform_enable_flag = tmp->secondary_transform_enable_flag;
	dst->tscpm_enable_flag = tmp->tscpm_enable_flag;
	dst->amvr_enable_flag = tmp->amvr_enable_flag;
	dst->emvr_enable_flag = tmp->emvr_enable_flag;
#endif // CTX_CONTROL_LUKE
	int i, j;
	for (i = 0; i < 17; i++){
		for (j = 0; j < 2; j++){
			dst->refp_ptr[i][j] = tmp->refp_ptr[i][j];
		}
	}
#if ENABLE_BFRAME
	dst->num_refp[0] = tmp->num_refp[0];
	dst->num_refp[0] = tmp->num_refp[0];
	dst->num_refp[1] = tmp->num_refp[1];
	dst->num_refp[1] = tmp->num_refp[1];
#else
	dst->num_refp = tmp->num_refp;
#endif
	
	dst->cur_ptr = tmp->cur_ptr; // ref pic
}

void copy_md_input(MD_INPUT *tmp, MD_INPUT *dst){
	dst->pix_x = tmp->pix_x;
	dst->pix_y = tmp->pix_y;
	dst->lambda_mv = tmp->lambda_mv;
	int i;
	for (i = 0; i < 3; i++){
		dst->lambda[i] = tmp->lambda[i];
	}
	dst->qp_y = tmp->qp_y;
	dst->qp_u = tmp->qp_u;
	dst->qp_v = tmp->qp_v;
	for (i = 0; i < 2; i++){
		dst->dist_chroma_weight[i] = tmp->dist_chroma_weight[i];
	}

	dst->x_last_ctu = tmp->x_last_ctu;
	dst->y_last_ctu = tmp->y_last_ctu;
}

void copy_rdoq(RDOQ_MODEL *tmp, RDOQ_MODEL *dst)
{
	int i;
	for (i = 0; i < 3; i++)
	{
		dst->cbf[i] = tmp->cbf[i];
	}
	for (i = 0; i < 24; i++){
		dst->run_rdoq[i] = tmp->run_rdoq[i];
		dst->level[i] = tmp->level[i];
	}
	for (i = 0; i < 12; i++){
		dst->last1[i] = tmp->last1[i];
	}
	for (i = 0; i < 22; i++){
		dst->last2[i] = tmp->last2[i];
	}
}

//	dst->ref_down = tmp->ref_down;*/
//
//	for (int i = 0; i < SWW * SWH; i++)
//	{
//		dst->RefWindowLCU[i] = tmp->RefWindowLCU[i];
//	}
//
//	for (int i = 0; i < SWH; i++)
//	{
//		for (int j = 0; j < SWH; j++)
//		{
//			dst->RefWindowLCU_2D[i][j] = tmp->RefWindowLCU_2D[i][j];
//		}
//	}
//
//	for (int i = 0; i < SWH / 2; i++)
//	{
//		for (int j = 0; j < SWH / 2; j++)
//		{
//			dst->RefWindowLCU_U[i][j] = tmp->RefWindowLCU_U[i][j];
//			dst->RefWindowLCU_V[i][j] = tmp->RefWindowLCU_V[i][j];
//		}
//	}
//
//	for (int i = 0; i < 2; i++)
//	{
//		for (int j = 0; j < 2; j++)
//		{
//			for (int k = 0; k < 2; k++)
//			{
//				dst->CtrPos[i][j][k] = tmp->CtrPos[i][j][k];
//			}
//		}
//	}
//
//	for (int i = 0; i < (SWH * SWW) >> 2; i++)
//	{
//		dst->RefWindowLCU_UP[i] = tmp->RefWindowLCU_UP[i];
//		dst->RefWindowLCU_VP[i] = tmp->RefWindowLCU_VP[i];
//	}
//
//	//for (int i = 0; i < SWH; i++)
//	//{
//	//	for (int j = 0; j < SWW; j++)
//	//	{
//	//		dst->swOutLv0[i][j] = tmp->swOutLv0[i][j];
//	//	}
//	//}
//
//	//for (int i = 0; i < 160; i++)
//	//{
//	//	for (int j = 0; j < 160; j++)
//	//	{
//	//		dst->swOutLv1[i][j] = tmp->swOutLv1[i][j];
//	//	}
//	//}
//
//	//for (int i = 0; i < 80; i++)
//	//{
//	//	for (int j = 0; j < 80; j++)
//	//	{
//	//		dst->swOutLv2[i][j] = tmp->swOutLv2[i][j];
//	//	}
//	//}
	//		//dst->y_orgT[i][j] = tmp->y_orgT[i][j];
	//	}
	//}

	//for (int i = 0; i < 32; i++)
	//{
	//	for (int j = 0; j < 32; j++)
	//	{
	//		//dst->u_org[i][j] = tmp->u_org[i][j];
	//		//dst->v_org[i][j] = tmp->v_org[i][j];
	//		//dst->y_org_lv1_2d[i][j] = tmp->y_org_lv1_2d[i][j];
	//	}
	//}

	//for (int i = 0; i < 16; i++)

void copy_enc_me_line_map(ENC_ME_LINE_MAP *tmp, ENC_ME_LINE_MAP *dst)
{
	dst->min_scu_x = tmp->min_scu_x;
	dst->min_scu_y = tmp->min_scu_y;
	dst->stride_in_map = tmp->stride_in_map;
	dst->stride_in_line = tmp->stride_in_line;
	dst->pic_width_in_scu = tmp->pic_width_in_scu;
	dst->pic_height_in_scu = tmp->pic_height_in_scu;
	dst->above_line_idx = tmp->above_line_idx;
	dst->curr_ctu_idx = tmp->curr_ctu_idx;
	dst->x_ctu = tmp->x_ctu;
	dst->y_ctu = tmp->y_ctu;

	//for (int i = 0; i < MAX_CU_DEPTH; i++)
	//{
	//	for (int j = 0; j < NUM_BLOCK_SHAPE; j++)
	//	{
	//		for (int k = 0; k < MAX_CU_CNT_IN_LCU; k++)
	//		{
	//			dst->line_map_split[i][j][k] = tmp->line_map_split[i][j][k];
	//		}
	//	}
	//}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			for (int k = 0; k < SCU_IN_LCU_NUMS_LEN; k++)
			{
				dst->map_scu_inner[i][j][k] = tmp->map_scu_inner[i][j][k];
				dst->map_cu_mode_inner[i][j][k] = tmp->map_cu_mode_inner[i][j][k];
			}
		}
	}

	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
		for (int j = 0; j < MV_D; j++)
		{
			dst->co_left_mv[i][j] = tmp->co_left_mv[i][j];
		}
	}

	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
		dst->co_left_refi[i] = tmp->co_left_refi[i];
	}

	//for (int i = 0; i < NEB_NUMS; i++)
	//{
	//	//dst->refi_nebs[i] = tmp->refi_nebs[i];
	//	//dst->scu_nebs[i] = tmp->scu_nebs[i];
	//}

	//for (int i = 0; i < NEB_NUMS; i++)
	//{
	//	for (int j = 0; j < MV_D; j++)
	//	{
	//		dst->mv_nebs[i][j] = tmp->mv_nebs[i][j];
	//	}
	//}
	for (int i = 0; i < ABOVE_LINE_LEN; i++)
	{
		dst->new_map_scu_above_line[i] = tmp->new_map_scu_above_line[i];
		dst->new_map_cu_mode_above_line[i] = tmp->new_map_cu_mode_above_line[i];
	}

	//dst->if_init = tmp->if_init;
}

void copy_enc_me_line_map_DBK(ENC_ME_LINE_MAP *tmp, ENC_ME_LINE_MAP *dst)
{
	dst->min_scu_x = tmp->min_scu_x;
	dst->min_scu_y = tmp->min_scu_y;
	dst->stride_in_map = tmp->stride_in_map;
	dst->stride_in_line = tmp->stride_in_line;
	dst->pic_width_in_scu = tmp->pic_width_in_scu;
	dst->pic_height_in_scu = tmp->pic_height_in_scu;
	dst->above_line_idx = tmp->above_line_idx;
	dst->curr_ctu_idx = tmp->curr_ctu_idx;
	dst->x_ctu = tmp->x_ctu;
	dst->y_ctu = tmp->y_ctu;

	//for (int i = 0; i < MAX_CU_DEPTH; i++)
	//{
	//	for (int j = 0; j < NUM_BLOCK_SHAPE; j++)
	//	{
	//		for (int k = 0; k < MAX_CU_CNT_IN_LCU; k++)
	//		{
	//			dst->line_map_split[i][j][k] = tmp->line_map_split[i][j][k];
	//		}
	//	}
	//}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
#pragma HLS PIPELINE
			for (int k = 0; k < SCU_IN_LCU_NUMS_LEN; k++)
			{
				dst->map_scu_inner[i][j][k] = tmp->map_scu_inner[i][j][k];
				//dst->map_cu_mode_inner[i][j][k] = tmp->map_cu_mode_inner[i][j][k];
			}
		}
	}

	//for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	//{
		//for (int j = 0; j < MV_D; j++)
		//{
			//dst->co_left_mv[i][j] = tmp->co_left_mv[i][j];
		//}
	//}

	//for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	//{
		//dst->co_left_refi[i] = tmp->co_left_refi[i];
	//}

	//for (int i = 0; i < NEB_NUMS; i++)
	//{
	//	//dst->refi_nebs[i] = tmp->refi_nebs[i];
	//	//dst->scu_nebs[i] = tmp->scu_nebs[i];
	//}

	//for (int i = 0; i < NEB_NUMS; i++)
	//{
	//	for (int j = 0; j < MV_D; j++)
	//	{
	//		dst->mv_nebs[i][j] = tmp->mv_nebs[i][j];
	//	}
	//}
	//for (int i = 0; i < ABOVE_LINE_LEN; i++)
	//{
		//dst->new_map_scu_above_line[i] = tmp->new_map_scu_above_line[i];
		//dst->new_map_cu_mode_above_line[i] = tmp->new_map_cu_mode_above_line[i];
	//}

	//dst->if_init = tmp->if_init;
}

void copy_enc_me_line_map_AEC(ENC_ME_LINE_MAP *tmp, ENC_ME_LINE_MAP *dst)
{
	dst->min_scu_x = tmp->min_scu_x;
	dst->min_scu_y = tmp->min_scu_y;
	dst->stride_in_map = tmp->stride_in_map;
	dst->stride_in_line = tmp->stride_in_line;
	dst->pic_width_in_scu = tmp->pic_width_in_scu;
	dst->pic_height_in_scu = tmp->pic_height_in_scu;
	dst->above_line_idx = tmp->above_line_idx;
	dst->curr_ctu_idx = tmp->curr_ctu_idx;
	dst->x_ctu = tmp->x_ctu;
	dst->y_ctu = tmp->y_ctu;

	//for (int i = 0; i < MAX_CU_DEPTH; i++)
	//{
	//	for (int j = 0; j < NUM_BLOCK_SHAPE; j++)
	//	{
	//		for (int k = 0; k < MAX_CU_CNT_IN_LCU; k++)
	//		{
	//			dst->line_map_split[i][j][k] = tmp->line_map_split[i][j][k];
	//		}
	//	}
	//}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
#pragma HLS PIPELINE
			for (int k = 0; k < SCU_IN_LCU_NUMS_LEN; k++)
			{
				dst->map_scu_inner[i][j][k] = tmp->map_scu_inner[i][j][k];
				dst->map_cu_mode_inner[i][j][k] = tmp->map_cu_mode_inner[i][j][k];
			}
		}
	}

	//for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	//{
		//for (int j = 0; j < MV_D; j++)
		//{
			//dst->co_left_mv[i][j] = tmp->co_left_mv[i][j];
		//}
	//}

	//for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	//{
		//dst->co_left_refi[i] = tmp->co_left_refi[i];
	//}

	//for (int i = 0; i < NEB_NUMS; i++)
	//{
	//	//dst->refi_nebs[i] = tmp->refi_nebs[i];
	//	//dst->scu_nebs[i] = tmp->scu_nebs[i];
	//}

	//for (int i = 0; i < NEB_NUMS; i++)
	//{
	//	for (int j = 0; j < MV_D; j++)
	//	{
	//		dst->mv_nebs[i][j] = tmp->mv_nebs[i][j];
	//	}
	//}
	//for (int i = 0; i < ABOVE_LINE_LEN; i++)
	//{
		//dst->new_map_scu_above_line[i] = tmp->new_map_scu_above_line[i];
		//dst->new_map_cu_mode_above_line[i] = tmp->new_map_cu_mode_above_line[i];
	//}

	//dst->if_init = tmp->if_init;
}

void copy_skip_merge(SKIP_MERGE_INPUT *tmp, SKIP_MERGE_INPUT *dst)
{
	dst->pix_x = tmp->pix_x;
	//dst->s64_sqrt_lambda_y = tmp->s64_sqrt_lambda_y;

	for (int i = 0; i < MAX_CU_IN_SCU; i++)
	{
		for (int j = 0; j < MAX_CU_IN_SCU; j++)
		{
			for (int k = 0; k < MV_D; k++)
			{
				dst->refp_map_mv[i][j][k] = tmp->refp_map_mv[i][j][k];
			}
			dst->refp_map_refi[i][j] = tmp->refp_map_refi[i][j];
		}
	}
}

void copy_fme_info(ENC_FME_INFO_ALL *tmp, ENC_FME_INFO_ALL *dst)
{
	for (int i = 0; i < 1; i++)//i = 1
	{
		for (int j = 0; j < 1; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree64x64Fmv[i][j][k] = tmp->sadTree64x64Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree64x32Fmv[i][j][k] = tmp->sadTree64x32Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree64x16Fmv[i][j][k] = tmp->sadTree64x16Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree64x8Fmv[i][j][k] = tmp->sadTree64x8Fmv[i][j][k];
			}
		}
	}

	for (int i = 0; i < 2; i++)//i = 2
	{
		for (int j = 0; j < 1; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree32x64Fmv[i][j][k] = tmp->sadTree32x64Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree32x32Fmv[i][j][k] = tmp->sadTree32x32Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree32x16Fmv[i][j][k] = tmp->sadTree32x16Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree32x8Fmv[i][j][k] = tmp->sadTree32x8Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 16; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree32x4Fmv[i][j][k] = tmp->sadTree32x4Fmv[i][j][k];
			}
		}
	}

	for (int i = 0; i < 4; i++)//i = 4
	{
		for (int j = 0; j < 1; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree16x64Fmv[i][j][k] = tmp->sadTree16x64Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree16x32Fmv[i][j][k] = tmp->sadTree16x32Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree16x16Fmv[i][j][k] = tmp->sadTree16x16Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree16x8Fmv[i][j][k] = tmp->sadTree16x8Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 16; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree16x4Fmv[i][j][k] = tmp->sadTree16x4Fmv[i][j][k];
			}
		}
	}

	for (int i = 0; i < 8; i++)//i = 8
	{
		for (int j = 0; j < 1; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree8x64Fmv[i][j][k] = tmp->sadTree8x64Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree8x32Fmv[i][j][k] = tmp->sadTree8x32Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree8x16Fmv[i][j][k] = tmp->sadTree8x16Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree8x8Fmv[i][j][k] = tmp->sadTree8x8Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 16; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree8x4Fmv[i][j][k] = tmp->sadTree8x4Fmv[i][j][k];
			}
		}
	}

	for (int i = 0; i < 16; i++)//i = 16
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree4x32Fmv[i][j][k] = tmp->sadTree4x32Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree4x16Fmv[i][j][k] = tmp->sadTree4x16Fmv[i][j][k];
			}
		}

		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				dst->sadTree4x8Fmv[i][j][k] = tmp->sadTree4x8Fmv[i][j][k];
			}
		}
	}
}

//void copy_ip_array(IP_ARRAY *tmp, IP_ARRAY *dst)
//{
//	for (int i = 0; i < 64; i++)
//	{
//		for (int j = 0; j < 64; j++)
//		{
//			dst->temp_buffer[i][j] = tmp->temp_buffer[i][j];
//			for (int k = 0; k < 23; k++)
//				dst->temp_buffer_size[k][i][j] = tmp->temp_buffer_size[k][i][j];
//		}
//	}
//
//	for (int i = 0; i < 16; i++)
//	{
//		for (int j = 0; j < 64; j++)
//		{
//			dst->up_innerLB[i][j] = tmp->up_innerLB[i][j];
//			for (int k = 0; k < 23; k++)
//				dst->up_innerLB_size[k][i][j] = tmp->up_innerLB_size[k][i][j];
//
//		}
//	}
//
//	for (int i = 0; i < 64; i++)
//	{
//		for (int j = 0; j < 16; j++)
//		{
//			for (int k = 0; k < 23; k++)
//				dst->left_innerLB_size[k][i][j] = tmp->left_innerLB_size[k][i][j];
//
//		}
//	}
//
//	for (int i = 0; i < 128; i++)
//	{
//		dst->up_temp[i] = tmp->up_temp[i];
//		for (int k = 0; k < 23; k++)
//			dst->up_temp_size[k][i] = tmp->up_temp_size[k][i];
//
//	}
//
//	for (int i = 0; i < 64; i++)
//	{
//		dst->left[i] = tmp->left[i];
//		for (int k = 0; k < 23; k++)
//			dst->left_size[k][i] = tmp->left_size[k][i];
//	}
//
//	dst->top_left = tmp->top_left;
//	for (int i = 0; i < max_buffer_width; i++)
//	{
//		dst->up[i] = tmp->up[i];
//	}
//	for (int i = 0; i < 2; i++)
//	{
//		for (int j = 0; j < 32; j++)
//		{
//			for (int k = 0; k < 32; k++)
//			{
//				dst->temp_buffer_chroma[i][j][k] = tmp->temp_buffer_chroma[i][j][k];
//			}
//		}
//	}
//
//	for (int i = 0; i < 2; i++)
//	{
//		for (int j = 0; j < max_buffer_width / 2; j++)
//		{
//			dst->up_chroma[i][j] = tmp->up_chroma[i][j];
//		}
//	}
//
//	for (int i = 0; i < 2; i++)
//	{
//		for (int j = 0; j < 32; j++)
//		{
//			dst->left_chroma[i][j] = tmp->left_chroma[i][j];
//		}
//	}
//
//	for (int i = 0; i < 2; i++)
//	{
//		dst->top_left_chroma[i] = tmp->top_left_chroma[i];
//	}
//}

void copy_mpm_array(MPM_ARRAY *tmp, MPM_ARRAY *dst)
{
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			dst->buffer[i][j] = tmp->buffer[i][j];
		}

		dst->left_buffer[i] = tmp->left_buffer[i];
	}

	for (int i = 0; i < max_buffer_width / 4; i++)
	{
		dst->up[i] = tmp->up[i];
	}

	dst->top_buffer_stride1 = tmp->top_buffer_stride1;
	dst->left_buffer_stride1 = tmp->left_buffer_stride1;
}

void copy_ip_map_scu(IP_MAP_SCU *tmp, IP_MAP_SCU *dst)
{
	dst->min_scu_x = tmp->min_scu_x;
	dst->min_scu_y = tmp->min_scu_y;
	dst->curr_ctu_idx = tmp->curr_ctu_idx;
	dst->above_line_idx = tmp->above_line_idx;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			for (int k = 0; k < SCU_IN_LCU_NUMS_LEN; k++)
			{
				dst->map_scu_inner[i][j][k] = tmp->map_scu_inner[i][j][k];
			}
		}

		for (int j = 0; j < (max_buffer_width >> MIN_CU_LOG2); j++)
		{
			dst->map_scu_above_line[i][j] = tmp->map_scu_above_line[i][j];
		}
	}
}

#if ENABLE_BFRAME
S8 get_lidx_by_refi(S3 refi0, S3 refi1)
{
	if (refi0 >= 0 && refi1 < 0)
		return 0;
	else if (refi1 >= 0 && refi0 < 0)
		return 1;
	else
		return -1;
}
#endif

s16 get_g_map_idx(int w, int h, int x0, int y0)
{
	static const s16 g_map_idx[5][5][16][16] = { { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, (1), (2), (3), (4), (5), (6), (7), (8), (9), (10), (11), (12), (13), (14), 0 }, { (15), (16), (17), (18), (19), (20), (21), (22), (23), (24), (25), (26), (27), (28), (29), 0 }, { (30), (31), (32), (33), (34), (35), (36), (37), (38), (39), (40), (41), (42), (43), (44), 0 }, { (45), (46), (47), (48), (49), (50), (51), (52), (53), (54), (55), (56), (57), (58), (59), 0 }, { (60), (61), (62), (63), (64), (65), (66), (67), (68), (69), (70), (71), (72), (73), (74), 0 }, { (75), (76), (77), (78), (79), (80), (81), (82), (83), (84), (85), (86), (87), (88), (89), 0 }, { (90), (91), (92), (93), (94), (95), (96), (97), (98), (99), (100), (101), (102), (103), (104), 0 }, { (105), (106), (107), (108), (109), (110), (111), (112), (113), (114), (115), (116), (117), (118), (119), 0 }, { (120), (121), (122), (123), (124), (125), (126), (127), (128), (129), (130), (131), (132), (133), (134), 0 }, { (135), (136), (137), (138), (139), (140), (141), (142), (143), (144), (145), (146), (147), (148), (149), 0 }, { (150), (151), (152), (153), (154), (155), (156), (157), (158), (159), (160), (161), (162), (163), (164), 0 }, { (165), (166), (167), (168), (169), (170), (171), (172), (173), (174), (175), (176), (177), (178), (179), 0 }, { (180), (181), (182), (183), (184), (185), (186), (187), (188), (189), (190), (191), (192), (193), (194), 0 }, { (195), (196), (197), (198), (199), (200), (201), (202), (203), (204), (205), (206), (207), (208), (209), 0 }, { (210), (211), (212), (213), (214), (215), (216), (217), (218), (219), (220), (221), (222), (223), (224), 0 }, { (225), (226), (227), (228), (229), (230), (231), (232), (233), (234), (235), (236), (237), (238), (239), 0 } }, { { (240), 0, (241), 0, (242), 0, (243), 0, (244), 0, (245), 0, (246), 0, 0, 0 }, { (247), 0, (248), 0, (249), 0, (250), 0, (251), 0, (252), 0, (253), 0, 0, 0 }, { (254), 0, (255), 0, (256), 0, (257), 0, (258), 0, (259), 0, (260), 0, 0, 0 }, { (261), 0, (262), 0, (263), 0, (264), 0, (265), 0, (266), 0, (267), 0, 0, 0 }, { (268), 0, (269), 0, (270), 0, (271), 0, (272), 0, (273), 0, (274), 0, 0, 0 }, { (275), 0, (276), 0, (277), 0, (278), 0, (279), 0, (280), 0, (281), 0, 0, 0 }, { (282), 0, (283), 0, (284), 0, (285), 0, (286), 0, (287), 0, (288), 0, 0, 0 }, { (289), 0, (290), 0, (291), 0, (292), 0, (293), 0, (294), 0, (295), 0, 0, 0 }, { (296), 0, (297), 0, (298), 0, (299), 0, (300), 0, (301), 0, (302), 0, 0, 0 }, { (303), 0, (304), 0, (305), 0, (306), 0, (307), 0, (308), 0, (309), 0, 0, 0 }, {
		(310), 0, (311), 0, (312), 0, (313), 0, (314), 0, (315), 0, (316), 0, 0



		, 0
	}, { (317), 0, (318), 0, (319), 0, (320), 0, (321), 0, (322), 0, (323), 0, 0, 0 }, { (324), 0, (325), 0, (326), 0, (327), 0, (328), 0, (329), 0, (330), 0, 0, 0 }, { (331), 0, (332), 0, (333), 0, (334), 0, (335), 0, (336), 0, (337), 0, 0, 0 }, { (338), 0, (339), 0, (340), 0, (341), 0, (342), 0, (343), 0, (344), 0, 0, 0 }, { (345), 0, (346), 0, (347), 0, (348), 0, (349), 0, (350), 0, (351), 0, 0, 0 }
	}, { { (352), 0, 0, 0, (353), 0, 0, 0, (354), 0, 0, 0, 0, 0, 0, 0 }, { (355), 0, 0, 0, (356), 0, 0, 0, (357), 0, 0, 0, 0, 0, 0, 0 }, { (358), 0, 0, 0, (359), 0, 0, 0, (360), 0, 0, 0, 0, 0, 0, 0 }, { (361), 0, 0, 0, (362), 0, 0, 0, (363), 0, 0, 0, 0, 0, 0, 0 }, { (364), 0, 0, 0, (365), 0, 0, 0, (366), 0, 0, 0, 0, 0, 0, 0 }, { (367), 0, 0, 0, (368), 0, 0, 0, (369), 0, 0, 0, 0, 0, 0, 0 }, { (370), 0, 0, 0, (371), 0, 0, 0, (372), 0, 0, 0, 0, 0, 0, 0 }, { (373), 0, 0, 0, (374), 0, 0, 0, (375), 0, 0, 0, 0, 0, 0, 0 }, { (376), 0, 0, 0, (377), 0, 0, 0, (378), 0, 0, 0, 0, 0, 0, 0 }, { (379), 0, 0, 0, (380), 0, 0, 0, (381), 0, 0, 0, 0, 0, 0, 0 }, { (382), 0, 0, 0, (383), 0, 0, 0, (384), 0, 0, 0, 0, 0, 0, 0 }, { (385), 0, 0, 0, (386), 0, 0, 0, (387), 0, 0, 0, 0, 0, 0, 0 }, { (388), 0, 0, 0, (389), 0, 0, 0, (390), 0, 0, 0, 0, 0, 0, 0 }, { (391), 0, 0, 0, (392), 0, 0, 0, (393), 0, 0, 0, 0, 0, 0, 0 }, { (394), 0, 0, 0, (395), 0, 0, 0, (396), 0, 0, 0, 0, 0, 0, 0 }, { (397), 0, 0, 0, (398), 0, 0, 0, (399), 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
		}, { { { (400), (401), (402), (403), (404), (405), (406), (407), (408), (409), (410), (411), (412), (413), (414), (415) }, { (416), (417), (418), (419), (420), (421), (422), (423), (424), (425), (426), (427), (428), (429), (430), (431) }, { (432), (433), (434), (435), (436), (437), (438), (439), (440), (441), (442), (443), (444), (445), (446), (447) }, { (448), (449), (450), (451), (452), (453), (454), (455), (456), (457), (458), (459), (460), (461), (462), (463) }, { (464), (465), (466), (467), (468), (469), (470), (471), (472), (473), (474), (475), (476), (477), (478), (479) }, { (480), (481), (482), (483), (484), (485), (486), (487), (488), (489), (490), (491), (492), (493), (494), (495) }, { (496), (497), (498), (499), (500), (501), (502), (503), (504), (505), (506), (507), (508), (509), (510), (511) }, {
			(512), (513), (514), (515), (516), (517), (518), (519), (520), (521), (522),

			(523), (524), (525), (526), (527)
		}, { (528), (529), (530), (531), (532), (533), (534), (535), (536), (537), (538), (539), (540), (541), (542), (543) }, { (544), (545), (546), (547), (548), (549), (550), (551), (552), (553), (554), (555), (556), (557), (558), (559) }, { (560), (561), (562), (563), (564), (565), (566), (567), (568), (569), (570), (571), (572), (573), (574), (575) }, { (576), (577), (578), (579), (580), (581), (582), (583), (584), (585), (586), (587), (588), (589), (590), (591) }, { (592), (593), (594), (595), (596), (597), (598), (599), (600), (601), (602), (603), (604), (605), (606), (607) }, { (608), (609), (610), (611), (612), (613), (614), (615), (616), (617), (618), (619), (620), (621), (622), (623) }, { (624), (625), (626), (627), (628), (629), (630), (631), (632), (633), (634), (635), (636), (637), (638), (639) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}, { { (640), (641), (642), (643), (644), (645), (646), (647), (648), (649), (650), (651), (652), (653), (654), 0 }, { (655), 0, (656), 0, (657), 0, (658), 0, (659), 0, (660), 0, (661), 0, (662), 0 }, { (663), (664), (665), (666), (667), (668), (669), (670), (671), (672), (673), (674), (675), (676), (677), 0 }, { (678), 0, (679), 0, (680), 0, (681), 0, (682), 0, (683), 0, (684), 0, (685), 0 }, { (686), (687), (688), (689), (690), (691), (692), (693), (694), (695), (696), (697), (698), (699), (700), 0 }, { (701), 0, (702), 0, (703), 0, (704), 0, (705), 0, (706), 0, (707), 0, (708), 0 }, { (709), (710), (711), (712), (713), (714), (715), (716), (717), (718), (719), (720), (721), (722), (723), 0 }, { (724), 0, (725), 0, (726), 0, (727), 0, (728), 0, (729), 0, (730), 0, (731), 0 }, { (732), (733), (734), (735), (736), (737), (738), (739), (740), (741), (742), (743), (744), (745), (746), 0 }, { (747), 0, (748), 0, (749), 0, (750), 0, (751), 0, (752), 0, (753), 0, (754), 0 }, { (755), (756), (757), (758), (759), (760), (761), (762), (763), (764), (765), (766), (767), (768), (769), 0 }, { (770), 0, (771), 0, (772), 0, (773), 0, (774), 0, (775), 0, (776), 0, (777), 0 }, { (778), (779), (780), (781), (782), (783), (784), (785), (786), (787), (788), (789), (790), (791), (792), 0 }, { (793), 0, (794), 0, (795), 0, (796), 0, (797), 0, (798), 0, (799), 0, (800), 0 }, { (801), (802), (803), (804), (805), (806), (807), (808), (809), (810), (811), (812), (813), (814), (815), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (816), 0, (817), 0, (818), 0, (819), 0, (820), 0, (821), 0, (822), 0, 0, 0 }, { (823), 0, 0, 0, (824), 0, 0, 0, (825), 0, 0, 0, (826), 0, 0, 0 }, { (827), 0, (828), 0, (829), 0, (830), 0, (831), 0, (832), 0, (833), 0, 0, 0 }, { (834), 0, 0, 0, (835), 0, 0, 0, (836), 0, 0, 0, (837), 0, 0, 0 }, { (838), 0, (839), 0, (840), 0, (841), 0, (842), 0, (843), 0, (844), 0, 0, 0 }, { (845), 0, 0, 0, (846), 0, 0, 0, (847), 0, 0, 0, (848), 0, 0, 0 }, { (849), 0, (850), 0, (851), 0, (852), 0, (853), 0, (854), 0, (855), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (856), 0, (857), 0, (858), 0, (859), 0, (860), 0, (861), 0, (862), 0, 0, 0 }, { (863), 0, 0, 0, (864), 0, 0, 0, (865), 0, 0, 0, (866), 0, 0, 0 }, { (867), 0, (868), 0, (869), 0, (870), 0, (871), 0, (872), 0, (873), 0, 0, 0 }, { (874), 0, 0, 0, (875), 0, 0, 0, (876), 0, 0, 0, (877), 0, 0, 0 }, { (878), 0, (879), 0, (880), 0, (881), 0, (882), 0, (883), 0, (884), 0, 0, 0 }, { (885), 0, 0, 0, (886), 0, 0, 0, (887), 0, 0, 0, (888), 0, 0, 0 }, { (889), 0, (890), 0, (891), 0, (892), 0, (893), 0, (894), 0, (895), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { {
					(896), 0, 0, 0, (897), 0,



					0, 0, (898), 0, 0, 0, 0, 0, 0, 0
				}, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (899), 0, 0, 0, (900), 0, 0, 0, (901), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (902), 0, 0, 0, (903), 0, 0, 0, (904), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (905), 0, 0, 0, (906), 0, 0, 0, (907), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (908), 0, 0, 0, (909), 0, 0, 0, (910), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (911), 0, 0, 0, (912), 0, 0, 0, (913), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (914), 0, 0, 0, (915), 0, 0, 0, (916), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (917), 0, 0, 0, (918), 0, 0, 0, (919), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}, { { (920), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (921), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (922), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (923), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (924), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (925), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (926), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (927), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
		}, { { { (928), (929), (930), (931), (932), (933), (934), (935), (936), (937), (938), (939), (940), (941), (942), (943) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (944), (945), (946), (947), (948), (949), (950), (951), (952), (953), (954), (955), (956), (957), (958), (959) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (960), (961), (962), (963), (964), (965), (966), (967), (968), (969), (970), (971), (972), (973), (974), (975) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (976), (977), (978), (979), (980), (981), (982), (983), (984), (985), (986), (987), (988), (989), (990), (991) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (992), (993), (994), (995), (996), (997), (998), (999), (1000), (1001), (1002), (1003), (1004), (1005), (1006), (1007) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1008), (1009), (1010), (1011), (1012), (1013), (1014), (1015), (1016), (1017), (1018), (1019), (1020), (1021), (1022), (1023) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1024), (1025), (1026), (1027), (1028), (1029), (1030), (1031), (1032), (1033), (1034), (1035), (1036), (1037), (1038), (1039) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
			0, 0, 0, 0,



			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}, { { (1040), (1041), (1042), (1043), (1044), (1045), (1046), 0, (1047), (1048), (1049), (1050), (1051), (1052), (1053), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1054), 0, (1055), 0, (1056), 0, (1057), 0, (1058), 0, (1059), 0, (1060), 0, (1061), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1062), (1063), (1064), (1065), (1066), (1067), (1068), 0, (1069), (1070), (1071), (1072), (1073), (1074), (1075), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1076), 0, (1077), 0, (1078), 0, (1079), 0, (1080), 0, (1081), 0, (1082), 0, (1083), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1084), (1085), (1086), (1087), (1088), (1089), (1090), 0, (1091), (1092), (1093), (1094), (1095), (1096), (1097), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1098), 0, (1099), 0, (1100), 0, (1101), 0, (1102), 0, (1103), 0, (1104), 0, (1105), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1106), (1107), (1108), (1109), (1110), (1111), (1112), 0, (1113), (1114), (1115), (1116), (1117), (1118), (1119), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1120), 0, (1121), 0, (1122), 0, (1123), 0, (1124), 0, (1125), 0, (1126), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1127), 0, 0, 0, (1128), 0, 0, 0, (1129), 0, 0, 0, (1130), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1131), 0, (1132), 0, (1133), 0, (1134), 0, (1135), 0, (1136), 0, (1137), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1138), 0, 0, 0, (1139), 0, 0, 0, (1140), 0, 0, 0, (1141), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1142), 0, (1143), 0, (1144), 0, (1145), 0, (1146), 0, (1147), 0, (1148), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1149), 0, 0, 0, (1150), 0, 0, 0, (1151), 0, 0, 0, (1152), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1153), 0, (1154), 0, (1155), 0, (1156), 0, (1157), 0, (1158), 0, (1159), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1160), 0, 0, 0, (1161), 0, 0, 0, (1162), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1163), 0, 0, 0, 0, 0, 0, 0, (1164), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1165), 0, 0, 0, (1166), 0, 0, 0, (1167), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1168), 0, 0, 0, (1169), 0, 0, 0, (1170), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1171), 0, 0, 0, 0, 0, 0, 0, (1172), 0, 0, 0, 0, 0, 0, 0 }, {
				0, 0, 0, (



				0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			}, { (1173), 0, 0, 0, (1174), 0, 0, 0, (1175), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}, { { (1176), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1177), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1178), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1179), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
		}, { { { (1180), (1181), (1182), (1183), (1184), (1185), (1186), (1187), (1188), (1189), (1190), (1191), (1192), (1193), (1194), (1195) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1196), (1197), (1198), (1199), (1200), (1201), (1202), (1203), (1204), (1205), (1206), (1207), (1208), (1209), (1210), (1211) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1212), (1213), (1214), (1215), (1216), (1217), (1218), (1219), (1220), (1221), (1222), (1223), (1224), (1225), (1226), (1227) }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1228), 0, (1229), 0, (1230), 0, (1231), 0, (1232), 0, (1233), 0, (1234), 0, (1235), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1236), 0, (1237), 0, (1238), 0, (1239), 0, (1240), 0, (1241), 0, (1242), 0, (1243), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1244), 0, (1245), 0, (1246), 0, (1247), 0, (1248), 0, (1249), 0, (1250), 0, (1251), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
			0, 0,



			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		}, { { (1252), 0, (1253), 0, (1254), 0, 0, 0, (1255), 0, (1256), 0, (1257), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1258), 0, 0, 0, (1259), 0, 0, 0, (1260), 0, 0, 0, (1261), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1262), 0, (1263), 0, (1264), 0, 0, 0, (1265), 0, (1266), 0, (1267), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1268), 0, 0, 0, (1269), 0, 0, 0, (1270), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1271), 0, 0, 0, 0, 0, 0, 0, (1272), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1273), 0, 0, 0, (1274), 0, 0, 0, (1275), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1276), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { (1277), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		}
		}, { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1278), 0, (1279), 0, (1280), 0, (1281), 0, (1282), 0, (1283), 0, (1284), 0, (1285), 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1286), 0, 0, 0, (1287), 0, 0, 0, (1288), 0, 0, 0, (1289), 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
			0, 0, 0, 0, 0, 0,



			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		}, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		}, { { (1290), 0, 0, 0, 0, 0, 0, 0, (1291), 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { { (1292), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
		}
	};

	return g_map_idx[w][h][x0][y0];
}

s64 init_basic_leaf(U1 isNS_allow, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,  MD_FW *md_fw_ptr, s64 lambdaY,U3 cu_width_log2, 
U3 cu_height_log2, U4 cud, MD_COM_MODE_BEST *bst_info_ptr,
	U3 qt_depth, U3 bet_depth, U13 x0, U13 y0, U9 cup, U2 tree_status,
	U2 *cons_pred_mode, MD_COM_MODE *mod_info_curr_temp_ptr,/* COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info,
	COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,*/
	ENC_CU_DATA_ARRAY *cu_data_temp_ptr)
{
	if (!isNS_allow)
		return MAX_S64;

	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	int bit_cnt;
	s64 cost_temp = 0;
	int pos = 0 + ((cu_height >> 1 >> 2) * (cu_width >> 2) + (cu_width >> 1 >> 2));
	int shape = (SQUARE) + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	int bit_rate = 0;
	cu_data_temp_ptr->split_mode[cud][shape][pos] = NO_SPLIT;
	bit_rate += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
	bit_cnt = bit_rate;
	cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	mode_cu_init(rdoq_model, model_cnt, rdoq_array, mod_info_curr_temp_ptr, bst_info_ptr, x0, y0, cu_width_log2, cu_height_log2, cud, md_fw_ptr->pic_width_in_scu,
		tree_status, cons_pred_mode, cu_data_temp_ptr);
	return cost_temp;
}

void init_md_kernel_input(MD_KERNEL_INPUT* md_kernel_input, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U13 x0, U13 y0, U3 cu_width_log2, U3 cu_height_log2, U11 pic_width_in_scu, U2 tree_status)
{
	md_kernel_input->x_chroma = cu_data_temp_ptr->x_chroma;
	md_kernel_input->y_chroma = cu_data_temp_ptr->y_chroma;
	md_kernel_input->x_pos = x0;
	md_kernel_input->y_pos = y0;
	md_kernel_input->chroma_motion = (cu_data_temp_ptr->last_child && tree_status == TREE_L);
	md_kernel_input->scup = (y0 >> 2) * pic_width_in_scu + (x0 >> 2);
	md_kernel_input->cu_width = (1 << cu_width_log2);
	md_kernel_input->cu_height = (1 << cu_height_log2);
	md_kernel_input->cu_width_log2 = cu_width_log2;
	md_kernel_input->cu_height_log2 = cu_height_log2;
}

void update_skip_candidates(COM_MOTION motion_cands[8], s8 *num_cands, const int max_hmvp_num, s32 mv_new[2][2], s32 refi_new[2])
{
	int i;
	int equal_idx = -1;
	COM_MOTION motion_curr;
	if (!((refi_new[0]) >= 0) && !((refi_new[1]) >= 0)) {
		return;
	}
	create_motion(&motion_curr, mv_new, refi_new);
	for (i = ((*num_cands)) - 1; i >= 0; i--) {
		if (same_motion(motion_cands[i], motion_curr)) {
			equal_idx = i;
			break;
		}
	}
	if (equal_idx == -1) {
		if (((*num_cands)) < max_hmvp_num) {
			(*num_cands)++;
		}
		else {
			int valid_pos = max_hmvp_num - 1 < ((*num_cands)) ? max_hmvp_num - 1 : ((*num_cands));
			for (i = 1; i <= valid_pos; i++) {
				copy_motion(&motion_cands[i - 1], motion_cands[i]);
			}
		}
		copy_motion(&motion_cands[((*num_cands)) - 1], motion_curr);
	}
	else {
		for (i = equal_idx; i < ((*num_cands)) - 1; i++) {
			copy_motion(&motion_cands[i], motion_cands[i + 1]);
		}
		copy_motion(&motion_cands[((*num_cands)) - 1], motion_curr);
	}
}


void md1_com_check_split_mode_top(U14 x0, U14 y0, U8 cu_width, U8 cu_height, U13 pic_width, U13 pic_height, U2 split_mode, U1 *do_split){
	U1 boundary = !(x0 + cu_width <= pic_width && y0 + cu_height <= pic_height);
	U1 boundary_r = boundary && x0 + cu_width > pic_width && !(y0 + cu_height > pic_height);
	U1 boundary_b = boundary && y0 + cu_height > pic_height && !(x0 + cu_width > pic_width);

	*do_split = boundary ? (U1)(!boundary_r && !boundary_b ? 1 : 0) : *do_split;

	return;
}
void md2_com_check_split_mode(U14 x0, U14 y0, MD_FW *md_fw_ptr, U1 *split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3 *num_split, U3 split_mode)
{
	U7 cu_w = 1 << cu_width_log2;
	U7 cu_h = 1 << cu_height_log2;
	int i;
	U1 boundary = !(x0 + cu_w <= md_fw_ptr->pic_width && y0 + cu_h <= md_fw_ptr->pic_height);
	U1 boundary_r = boundary && x0 + cu_w > md_fw_ptr->pic_width && !(y0 + cu_h > md_fw_ptr->pic_height);
	U1 boundary_b = boundary && y0 + cu_h > md_fw_ptr->pic_height && !(x0 + cu_w > md_fw_ptr->pic_width);

	U3 num_split_to_try = 0;
	for (i = 0; i <= (SPLIT_QUAD); i++) {
		split_allow[i] = 0;
	}

	if (split_mode == NO_SPLIT){
		split_allow[NO_SPLIT] = 1;
		*num_split = num_split_to_try = 1;
		return;
	}

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
		split_allow[SPLIT_BI_HOR] = 1;
		split_allow[SPLIT_BI_VER] = 1;
	}
	if (cu_w == 64 && cu_h == 32)
	{
		split_allow[SPLIT_BI_VER] = 0;
	}

	if (cu_w == 32 && cu_h == 64)
	{
		split_allow[SPLIT_BI_HOR] = 0;
	}

	for (int i = 0; i < 6; i++) {
		num_split_to_try += split_allow[i];
	}

	*num_split = num_split_to_try;
}
void com_check_split_mode_only_qt(U14 x0, U14 y0, MD_FW *md_fw_ptr, U1 *split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3 bet_depth, U3 *num_split)
{
	for (U3 i = 0; i <= (SPLIT_QUAD); i++) {
#pragma HLS PIPELINE
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
#pragma HLS PIPELINE
		num_split_to_try += split_allow[i];
	}

	if (num_split_to_try == 0)
	{
		split_allow[NO_SPLIT] = 1;
	}

	*num_split = num_split_to_try;
}
void com_check_split_mode(U14 x0, U14 y0, MD_FW *md_fw_ptr, U1 *split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3 qt_depth, U3 bet_depth, U3 *num_split, U3 split_mode)
{
	for (U3 i = 0; i <= (SPLIT_QUAD); i++) {
#pragma HLS pipeline
		split_allow[i] = 0;
	}

	if (split_mode == NO_SPLIT){
		split_allow[NO_SPLIT] = 1;
		*num_split = 1;
		return;
	}

	//constraints parameters
	const U3 min_bt_size = (md_fw_ptr->min_cu_size);
	//const U3 min_eqt_size = (md_fw_ptr->min_cu_size);
	const U3 max_split_depth = (md_fw_ptr->max_split_times);
	const U4 max_aspect_ratio = (md_fw_ptr->max_part_ratio);
	const U4 min_qt_size = (md_fw_ptr->min_qt_size);
	const U7 max_bt_size = (md_fw_ptr->max_bt_size);
	//const U7 max_eqt_size = (md_fw_ptr->max_eqt_size);

	U3 max_aspect_ratio_eqt = max_aspect_ratio >> 1;
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
        if (qt_depth + bet_depth >= max_split_depth) {
//no further split allowed
            split_allow[NO_SPLIT] = 1;
        } 
		else {
			split_allow[NO_SPLIT] = 1;
			split_allow[SPLIT_BI_HOR] = 1;
			split_allow[SPLIT_BI_VER] = 1;
////not split
//            if (cu_w <= cu_h * max_aspect_ratio && cu_h <= cu_w * max_aspect_ratio) {
//                split_allow[NO_SPLIT] = 1;
//            }
////qt
//            if (cu_w > min_qt_size && bet_depth == 0) {
//				split_allow[SPLIT_QUAD] = 1;
//			}
//			//hbt
//			if (cu_w <= max_bt_size && cu_h <= max_bt_size && cu_h > min_bt_size && cu_w < cu_h * max_aspect_ratio) {
//				split_allow[SPLIT_BI_HOR] = 1;
//				//4X4 is not allow
//				if (cu_w == 4 && cu_h == 8) {
//					split_allow[SPLIT_BI_HOR] = 0;
//				}
//			}
//			//vbt
//			if (cu_w <= max_bt_size && cu_h <= max_bt_size && cu_w > min_bt_size && cu_h < cu_w * max_aspect_ratio) {
//				split_allow[SPLIT_BI_VER] = 1;
//				//4X4 is not allow
//				if (cu_w == 8 && cu_h == 4) {
//					split_allow[SPLIT_BI_VER] = 0;
//				}
//			}
			////heqt
			//if (cu_w <= max_eqt_size && cu_h <= max_eqt_size && cu_h > min_eqt_size * 2 && cu_w > min_eqt_size
			//	&& cu_w < cu_h * max_aspect_ratio_eqt) {
			//	//if (!(cu_w == 16 && cu_h == 16) && !(cu_w == 32 && cu_h == 32) && !(cu_w == 64 && cu_h == 64))//16x16 32x32 64x64not do eqt
			//	if ((cu_w == 32 && cu_h == 64) || (cu_w == 32 && cu_h == 16) || (cu_w == 16 && cu_h == 32))//only 64x32 32x16 do eqt
			//		split_allow[SPLIT_EQT_HOR] = 1;
			//}
			////veqt
			//if (cu_w <= max_eqt_size && cu_h <= max_eqt_size && cu_w > min_eqt_size * 2 && cu_h > min_eqt_size
			//	&& cu_h < cu_w * max_aspect_ratio_eqt) {
			//		if ((cu_w == 64 && cu_h == 32) || (cu_w == 32 && cu_h == 16) || (cu_w == 16 && cu_h == 32))//only 64x32 32x16 do eqt
			//		split_allow[SPLIT_EQT_VER] = 1;
			//}
		}
	}
	if (cu_w == 64 && cu_h == 8)
	{
		split_allow[SPLIT_BI_VER] = 0;
	}

	if (cu_w == 8 && cu_h == 64)
	{
		split_allow[SPLIT_BI_HOR] = 0;
	}

	if (cu_w == 32 && cu_h == 16)
	{
		split_allow[SPLIT_BI_HOR] = 0;
	}

	if (cu_w == 16 && cu_h == 32)
	{
		split_allow[SPLIT_BI_VER] = 0;
	}

#if MD_BASE_PROFILE
	// remove all 4 sizes
	if (cu_w == 16 && cu_h == 8)
	{
		split_allow[SPLIT_BI_HOR] = 0;
	}

	if (cu_w == 8 && cu_h == 16)
	{
		split_allow[SPLIT_BI_VER] = 0;
	}
#endif // MD_BASE_PROFILE

	for (int i = 0; i < 6; i++) {
#pragma HLS pipeline
		num_split_to_try += split_allow[i];
	}

	if (num_split_to_try == 0)
	{
		split_allow[NO_SPLIT] = 1;
		num_split_to_try++;
	}


	*num_split = num_split_to_try;
}


void copy_motion_table(COM_MOTION *motion_dst, s8 *cnt_cands_dst, const COM_MOTION *motion_src, const s8 cnt_cands_src)
{
	int i;
	*cnt_cands_dst = cnt_cands_src;
	for (i = 0; i < (cnt_cands_src); i++) {

#pragma HLS loop_tripcount max=8
		motion_dst[i].mv[0][0] = motion_src[i].mv[0][0];
		motion_dst[i].mv[0][1] = motion_src[i].mv[0][1];
		motion_dst[i].mv[1][0] = motion_src[i].mv[1][0];
		motion_dst[i].mv[1][1] = motion_src[i].mv[1][1];
		motion_dst[i].ref_idx[0] = motion_src[i].ref_idx[0];
		motion_dst[i].ref_idx[1] = motion_src[i].ref_idx[1];
	}
}

//static int com_split_part_count(int split_mode)
//{
//	switch (split_mode) {
//	case (int)SPLIT_BI_VER:
//		;
//	case (int)SPLIT_BI_HOR:
//		return 2;
//	case (int)SPLIT_EQT_VER:
//		;
//	case (int)SPLIT_EQT_HOR:
//		return 4;
//	case (int)SPLIT_QUAD:
//		return 4;
//	default:
//		// NO_SPLIT
//		return 0;
//	}
//}

static SPLIT_DIR com_split_get_direction(U3 mode)
{
	switch (mode) {
	case SPLIT_BI_HOR:
		;
	case SPLIT_EQT_HOR:
		return SPLIT_HOR;
	case SPLIT_BI_VER:
		;
	case SPLIT_EQT_VER:
		return SPLIT_VER;
	default:
		return SPLIT_QT;
	}
}

static int com_split_is_vertical(U3 mode)
{
	//return ((com_split_get_direction(mode))) == ((unsigned int)SPLIT_VER) ? 1 : 0;
	return ((com_split_get_direction(mode))) == (0) ? 1 : 0;
}

//static int com_split_get_part_size(int split_mode, int part_num, int length)
//{
//	int ans = length;
//	switch (split_mode) {
//	case (int)SPLIT_QUAD:
//		;
//	case (int)SPLIT_BI_HOR:
//		;
//	case (int)SPLIT_BI_VER:
//		ans = length >> 1;
//            break;
//        case (int )SPLIT_EQT_HOR:
//            ;
//        case (int )SPLIT_EQT_VER:
//            if (part_num == 1 || part_num == 2) {
//                ans = length >> 1;
//            } else {
//                ans = length >> 2;
//            }
//            break;
//    }
//    return ans;
//}

//extern int cbf_0;
//extern int cbf_1;

static void clear_map_scu(ENC_ME_LINE_MAP *me_line_map_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, U13 x, U13 y, U7 cu_width, U7 cu_height, U13 pic_width, U13 pic_height)
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
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (m = 0; m < w; m++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + m][y_scu_line + i] = 0;
//		}
//	}
}

void com_set_split_mode(U3 split_mode, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s,
	U8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))])
{
	if (cu_width < 8 && cu_height < 8) {
		return;
	}
	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[lcu_s] - 2)) + (cu_width >> 1 >> 2));
	U3 shape = SQUARE + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);

	split_mode_buf[cud][shape][pos] = split_mode;
	return;
}

void com_get_split_mode(U3 *split_mode, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s,
	U8(*split_mode_buf)[NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))])
{
	if (cu_width < 8 && cu_height < 8) {
		*split_mode = NO_SPLIT;
		return;
	}
	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[lcu_s] - 2)) + (cu_width >> 1 >> 2));
	int shape = SQUARE + (com_tbl_log2[cu_width] -com_tbl_log2[cu_height]);

	*split_mode = split_mode_buf[cud][shape][pos] & 0x07;

	return;
}

void com_check_split_mode_ECO(MD_FW *md_fw_ptr, U1 *split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U1 boundary, U1 boundary_b, U1 boundary_r, U3 qt_depth, U3 bet_depth)
{
	//constraints parameters
	const U3 min_bt_size = (md_fw_ptr->min_cu_size);
	const U3 min_eqt_size = (md_fw_ptr->min_cu_size);
	const U3 max_split_depth = (md_fw_ptr->max_split_times);
	const U4 max_aspect_ratio = (md_fw_ptr->max_part_ratio);
	const U4 min_qt_size = (md_fw_ptr->min_qt_size);
	const U7 max_bt_size = (md_fw_ptr->max_bt_size);
	const U7 max_eqt_size = (md_fw_ptr->max_eqt_size);
	U3 max_aspect_ratio_eqt = max_aspect_ratio >> 1;
	U7 cu_w = 1 << cu_width_log2;
	U7 cu_h = 1 << cu_height_log2;
	int i;
    for (i = (NO_SPLIT); i <= (SPLIT_QUAD); i++) {
#pragma HLS unroll
        split_allow[i] = 0;
    }
    if (boundary) {
        if (!boundary_r && !boundary_b) {
            split_allow[SPLIT_QUAD] = 1;
        } else if (boundary_r) {
            split_allow[SPLIT_BI_VER] = 1;
        } else if (boundary_b) {
            split_allow[SPLIT_BI_HOR] = 1;
        }
    } else {
        if (qt_depth + bet_depth >= max_split_depth) {
//no further split allowed
            split_allow[NO_SPLIT] = 1;
        } else {
//not split
            if (cu_w <= cu_h * max_aspect_ratio && cu_h <= cu_w * max_aspect_ratio) {
                split_allow[NO_SPLIT] = 1;
            }
//qt
            if (cu_w > min_qt_size && bet_depth == 0) {
				split_allow[SPLIT_QUAD] = 1;
			}
			//hbt
			if (cu_w <= max_bt_size && cu_h <= max_bt_size && cu_h > min_bt_size && cu_w < cu_h * max_aspect_ratio) {
				split_allow[SPLIT_BI_HOR] = 1;
			}
			//vbt
			if (cu_w <= max_bt_size && cu_h <= max_bt_size && cu_w > min_bt_size && cu_h < cu_w * max_aspect_ratio) {
				split_allow[SPLIT_BI_VER] = 1;
			}
			//heqt
			if (cu_w <= max_eqt_size && cu_h <= max_eqt_size && cu_h > min_eqt_size << 1 && cu_w > min_eqt_size
				&& cu_w < cu_h * max_aspect_ratio_eqt) {
				split_allow[SPLIT_EQT_HOR] = 1;
			}
			//veqt
			if (cu_w <= max_eqt_size && cu_h <= max_eqt_size && cu_w > min_eqt_size << 1 && cu_h > min_eqt_size
				&& cu_h < cu_w * max_aspect_ratio_eqt) {
				split_allow[SPLIT_EQT_VER] = 1;
			}
		}
	}
}

U3 enc_eco_split_mode_est(MD_FW* md_fw_ptr, 
	U8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))],
	U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s, U3 qt_depth, U3 bet_depth, U13 x, U13 y)
{
	U13 pic_width = md_fw_ptr->pic_width;
	U13 pic_height = md_fw_ptr->pic_height;
	//ENC_SBAC *sbac;
	//int ret = 0;
	U3 split_mode;
	//int ctx = 0;
	U3 enc_est = 0;
	U1 split_allow[6];
	//int i;
	U3 non_QT_split_mode_num;
	U1 boundary = 0;
	U1 boundary_b = 0;
	U1 boundary_r = 0;
	if (cu_width == 1 << 2 && cu_height == 1 << 2) {
		return enc_est;
	}

	com_get_split_mode(&split_mode, cud, cup, cu_width, cu_height, lcu_s, split_mode_buf);
	boundary = !(x + cu_width <= pic_width && y + cu_height <= pic_height);
	boundary_b = boundary && y + cu_height > pic_height && !(x + cu_width > pic_width);
	boundary_r = boundary && x + cu_width > pic_width && !(y + cu_height > pic_height);
	com_check_split_mode_ECO(md_fw_ptr, split_allow, com_tbl_log2[cu_width], com_tbl_log2[cu_height],
		boundary, boundary_b, boundary_r, qt_depth, bet_depth);
	non_QT_split_mode_num = 0;
	non_QT_split_mode_num = split_allow[1] + split_allow[2] + split_allow[3] + split_allow[4];

	//only QT is allowed
	if (split_allow[SPLIT_QUAD] && !(non_QT_split_mode_num || split_allow[NO_SPLIT])) {
		return enc_est;
	}
	else if (split_allow[SPLIT_QUAD]) {
		enc_est++;
		if (split_mode == SPLIT_QUAD) {
			return enc_est;
		}
	}
	if (non_QT_split_mode_num) {

		//split flag

		if (split_allow[NO_SPLIT]) {
			enc_est++;
		}

		if (split_mode != NO_SPLIT) {
			int HBT = split_allow[SPLIT_BI_HOR];
			int VBT = split_allow[SPLIT_BI_VER];
			int EnableBT = HBT || VBT;
			int HEQT = split_allow[SPLIT_EQT_HOR];
			int VEQT = split_allow[SPLIT_EQT_VER];
			int EnableEQT = HEQT || VEQT;

			u8 split_typ = (split_mode == SPLIT_EQT_HOR || split_mode == SPLIT_EQT_VER);
			if (EnableEQT && EnableBT) {
				enc_est++;
			}
			if ((split_typ) == 0) {
				if (HBT && VBT) {
					enc_est++;
				}
			}
			if ((split_typ) == 1) {
				if (HEQT && VEQT) {
					enc_est++;
				}
			}
		}
	}
	return enc_est;
}

U3 enc_eco_split_mode_est(MD_FW *md_fw_ptr, ENC_CU_DATA_ARRAY *cu_data_temp_ptr, U4 cud,
	U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s, U3 qt_depth, U3 bet_depth, U13 x, U13 y)
{
	U13 pic_width = md_fw_ptr->pic_width;
	U13 pic_height = md_fw_ptr->pic_height;
	//ENC_SBAC *sbac;
	//int ret = 0;
	U3 split_mode;
	//int ctx = 0;
	U3 enc_est = 0;
	U1 split_allow[6];
	//int i;
	U3 non_QT_split_mode_num;
	U1 boundary = 0;
	U1 boundary_b = 0;
	U1 boundary_r = 0;
	if (cu_width == 1 << 2 && cu_height == 1 << 2) {
		return enc_est;
	}

	com_get_split_mode(&split_mode, cud, cup, cu_width, cu_height, lcu_s, cu_data_temp_ptr->split_mode);
	boundary = !(x + cu_width <= pic_width && y + cu_height <= pic_height);
	boundary_b = boundary && y + cu_height > pic_height && !(x + cu_width > pic_width);
	boundary_r = boundary && x + cu_width > pic_width && !(y + cu_height > pic_height);
	com_check_split_mode_ECO(md_fw_ptr, split_allow, com_tbl_log2[cu_width], com_tbl_log2[cu_height],
		boundary, boundary_b, boundary_r, qt_depth, bet_depth);
	non_QT_split_mode_num = 0;
	non_QT_split_mode_num = split_allow[1] + split_allow[2] + split_allow[3] + split_allow[4];

//only QT is allowed
    if (split_allow[SPLIT_QUAD] && !(non_QT_split_mode_num || split_allow[NO_SPLIT])) {
        return enc_est;
    } else if (split_allow[SPLIT_QUAD]) {
        enc_est++;
        if (split_mode==SPLIT_QUAD) {
            return enc_est;
        }
    }
    if (non_QT_split_mode_num) {

//split flag

        if (split_allow[NO_SPLIT]) {
            enc_est++;
        } 
 
        if (split_mode!= NO_SPLIT) {
            int HBT = split_allow[SPLIT_BI_HOR];
			int VBT = split_allow[SPLIT_BI_VER];
			int EnableBT = HBT || VBT;
			int HEQT = split_allow[SPLIT_EQT_HOR];
			int VEQT = split_allow[SPLIT_EQT_VER];
			int EnableEQT = HEQT || VEQT;

			u8 split_typ = (split_mode== SPLIT_EQT_HOR || split_mode==SPLIT_EQT_VER);
			if (EnableEQT && EnableBT) {
				enc_est++;
			}
			if ((split_typ) == 0) {
				if (HBT && VBT) {
					enc_est++;
				}
			}
			if ((split_typ) == 1) {
				if (HEQT && VEQT) {
					enc_est++;
				}
			}
		}
	}
	return enc_est;
}

int com_split_is_EQT(U3 mode)
{
	return mode==SPLIT_EQT_HOR
		|| mode ==SPLIT_EQT_VER ? 1 : 0;
}

static int com_split_is_BT(U3 mode)
{
	return mode==SPLIT_BI_HOR
		|| mode ==SPLIT_BI_VER ? 1 : 0;
}

u8 com_constrain_pred_mode(U7 w, U7 h, U3 split, U2 slice_type)
{
    if ((slice_type) == 1) {
        return 0;
    } else {
        U16 s = (U16)w << com_tbl_log2[h];
        if ((com_split_is_EQT(split) && s == 128) ||
			((com_split_is_BT(split) || split==SPLIT_QUAD) && s == 64)) {
            return 1;
        } else {
            return 0;
        }
    }
}

static U1 enc_eco_cons_pred_mode_child(U2 cons_pred_mode_child)
{
	U1 enc_est = 0;

	U1 flag = cons_pred_mode_child == ONLY_INTRA;

	enc_est++;
	return enc_est;
}

void cu_nz_cln(int dst[4][3])
{
#pragma HLS ARRAY_PARTITION variable=dst complete dim=0
	U3 i, j;
#pragma HLS INLINE off
	for (i = 0; i < 4; i++) {

#pragma HLS UNROLL
		for (j= 0; j < 3; j++) {

#pragma HLS UNROLL
			dst[i][j] = 0;
		}
	}
}

//void init_pb_part(MD_COM_MODE *mod_info_curr)
//{
//
//#pragma HLS INLINE off
////	mod_info_curr->pb_part = ((int)SIZE_2Nx2N);
//	mod_info_curr->pb_part = 0;
//	//cu_nz_cln(mod_info_curr->num_nz);
//}
//
//void init_tb_part(MD_COM_MODE *mod_info_curr)
//{
//
//#pragma HLS INLINE off
////	mod_info_curr->tb_part = ((int)SIZE_2Nx2N);
//	mod_info_curr->tb_part = 0;
//}

void copy_md_com_part_info(COM_PART_INFO* src, COM_PART_INFO* dst)
{
	dst->num_sub_part = src->num_sub_part;
	for (int i = 0; i < MAX_NUM_PB; i++)
	{
		dst->sub_x[i] = src->sub_x[i];
		dst->sub_y[i] = src->sub_y[i];
		dst->sub_w[i] = src->sub_w[i];
		dst->sub_h[i] = src->sub_h[i];
		dst->sub_scup[i] = src->sub_scup[i];
	}
}

void copy_md_com_mode_info(MD_COM_MODE* src, MD_COM_MODE* dst)
{
	dst->x_scu = src->x_scu;
	dst->y_scu = src->y_scu;
	dst->cud = src->cud;
	dst->cu_width = src->cu_width;
	dst->cu_height = src->cu_height;
	dst->cu_width_log2 = src->cu_width_log2;
	dst->cu_height_log2 = src->cu_height_log2;
	dst->x_pos = src->x_pos;
	dst->y_pos = src->y_pos;
	dst->scup = src->scup;

	dst->cu_mode = src->cu_mode;
#if TB_SPLIT_EXT
	dst->pb_part = src->pb_part;
	dst->tb_part = src->tb_part;
#endif

	//for (int i = 0; i < MAX_CU_DIM; i++)
	//{
	//	dst->rec_y[i] = src->rec_y[i];
	//	dst->coef_y[i] = src->coef_y[i];
	//	dst->pred_y[i] = src->pred_y[i];
	//}

	//for (int i = 0; i < MAX_CU_DIM >> 2; i++)
	//{
	//	dst->rec_u[i] = src->rec_u[i];
	//	dst->rec_v[i] = src->rec_v[i];

	//	dst->coef_u[i] = src->coef_u[i];
	//	dst->coef_v[i] = src->coef_v[i];

	//	dst->pred_u[i] = src->pred_u[i];
	//	dst->pred_v[i] = src->pred_v[i];
	//}


#if TB_SPLIT_EXT
	for (int i = 0; i < MAX_NUM_TB; i++)
	{
		for (int j = 0; j < N_C; j++)
		{
			dst->num_nz[i][j] = src->num_nz[i][j];
		}
	}
#else
	for (int i = 0; i < N_C; i++)
		dst->num_nz[i] = src->num_nz[i];
#endif

		/* reference indices */
	for (int i = 0; i < REFP_NUM; i++)
	{
		dst->refi[i] = src->refi[i];
	}

		/* MVR indices */
	dst->mvr_idx = src->mvr_idx;
	dst->umve_flag = src->umve_flag;
	dst->umve_idx = src->umve_idx;
	dst->skip_idx = src->skip_idx;
#if EXT_AMVR_HMVP
	dst->mvp_from_hmvp_flag = src->mvp_from_hmvp_flag;
#endif

	for (int i = 0; i < REFP_NUM; i++)
	{
		for (int j = 0; j < MV_D; j++)
		{
			dst->mvd[i][j] = src->mvd[i][j];
			dst->mv[i][j] = src->mv[i][j];
		}
	}

	for (int i = 0; i < REFP_NUM; i++)
	{
		for (int j = 0; j < VER_NUM; j++)
		{
			for (int k = 0; k < MV_D; k++)
			{
				dst->affine_mv[i][j][k] = src->affine_mv[i][j][k];
				dst->affine_mvd[i][j][k] = src->affine_mvd[i][j][k];
			}
		}

	}

		dst->affine_flag = src->affine_flag;
#if SMVD
		dst->smvd_flag = src->smvd_flag;
#endif

		/* intra prediction mode */
#if TB_SPLIT_EXT
		for (int i = 0; i < MAX_NUM_PB; i++)
		{
			dst->mpm[i][0] = src->mpm[i][0];
			dst->mpm[i][1] = src->mpm[i][1];

			dst->ipm[i][0] = src->ipm[i][0];
			dst->ipm[i][1] = src->ipm[i][1];
		}
#else
		dst->mpm[0] = src->mpm[0];
		dst->mpm[1] = src->mpm[1];
		dst->ipm[0] = src->ipm[0];
		dst->ipm[1] = src->ipm[1];
#endif

		dst->ipf_flag = src->ipf_flag;
		dst->chroma_motion = src->chroma_motion;
		dst->x_luma = src->x_luma;
		dst->y_luma = src->y_luma;
}


int mode_cu_init(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt,  RDOQ_ARRAY* rdoq_array,  MD_COM_MODE *mod_info_curr_ptr, MD_COM_MODE_BEST *bst_info_ptr,
  /*COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info, COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,*/ 
  U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status,
	U2 *cons_pred_mode, ENC_CU_DATA_ARRAY *cu_data_temp_ptr)
{
	MD_COM_MODE *mod_info_curr = mod_info_curr_ptr;
	MD_COM_MODE_BEST *bst_info = bst_info_ptr;
	bst_info->x_luma = mod_info_curr->x_luma = x;
	bst_info->y_luma = mod_info_curr->y_luma = y;
	bst_info->x_pos = mod_info_curr->x_pos = x;
	bst_info->y_pos = mod_info_curr->y_pos = y;
	bst_info->cu_width = mod_info_curr->cu_width = (1 << cu_width_log2);
	bst_info->cu_height = mod_info_curr->cu_height = (1 << cu_height_log2);
	bst_info->cu_width_log2 = mod_info_curr->cu_width_log2 = cu_width_log2;
	bst_info->cu_height_log2 = mod_info_curr->cu_height_log2 = cu_height_log2;
	bst_info->x_scu = mod_info_curr->x_scu = x >> 2;
	bst_info->y_scu = mod_info_curr->y_scu = y >> 2;
	bst_info->scup = mod_info_curr->scup = mod_info_curr->y_scu * pic_width_in_scu + mod_info_curr->x_scu;///////////////////////////////
	bst_info->cud = mod_info_curr->cud = cud;
	bst_info->chroma_motion = 0;
	mod_info_curr->chroma_motion = /*0*/ mod_info_curr->last_child && tree_status == TREE_L;
	U5 cuw_scu = 1 << (cu_width_log2 - 2);
	U5 cuh_scu = 1 << (cu_height_log2 - 2);
	int m;
	cu_nz_cln(mod_info_curr->num_nz);
	cu_nz_cln(bst_info->num_nz);
	
	if (tree_status!=TREE_C) {
		for (m = 0; m < cuw_scu * cuh_scu; m++) {
#pragma HLS LOOP_TRIPCOUNT max=256
			cu_data_temp_ptr->ipm[0][m] = 0;
			cu_data_temp_ptr->ipm[1][m] = 0;
		}
	}

	int i;
	bst_info->cu_mode = 0;
	mod_info_curr->cu_mode = 0;
	mod_info_curr->pb_part = 0;
	mod_info_curr->tb_part = 0;
	bst_info->pb_part = 0;
	bst_info->tb_part = 0;
	bst_info->ipf_flag = 0;
	mod_info_curr->ipf_flag = 0;

	bst_info->mvr_idx = 0;
	bst_info->skip_idx = 0;
	bst_info->umve_flag = 0;
	bst_info->umve_idx = ((-1));
	bst_info->mvp_from_hmvp_flag = 0;
	bst_info->affine_flag = 0;
	mod_info_curr->mvr_idx = 0;
	mod_info_curr->skip_idx = 0;
	mod_info_curr->umve_flag = 0;
	mod_info_curr->umve_idx = ((-1));
	mod_info_curr->mvp_from_hmvp_flag = 0;
	mod_info_curr->affine_flag = 0;

	mod_info_curr->smvd_flag = 0;
	bst_info->smvd_flag = 0;
	
	enc_rdoq_bit_est(rdoq_model, model_cnt, rdoq_array);
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	u8 luma_pred_mode;
	
	if (tree_status==TREE_C) {
		int cu_w_scu = cu_width >> 2;
		int cu_h_scu = cu_height >> 2;
		// bottom-right
		int luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;
		luma_pred_mode = cu_data_temp_ptr->pred_mode[luma_pos_scu];
		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
		bst_info->cu_mode =  mod_info_curr->cu_mode = luma_pred_mode;
        bst_info->ipm[0][0] = mod_info_curr->ipm[0][0]= cu_data_temp_ptr->ipm[0][luma_pos_scu];
        bst_info->ipm[0][1] = mod_info_curr->ipm[0][1] = 0;
		bst_info->affine_flag = mod_info_curr->affine_flag = cu_data_temp_ptr->affine_flag[luma_pos_scu];

        for (i = 0; i < 2; i++) {
			bst_info->refi[i] =  mod_info_curr->refi[i] = cu_data_temp_ptr->refi[luma_pos_scu][i];
			bst_info->mv[i][0] = mod_info_curr->mv[i][0] = cu_data_temp_ptr->mv[luma_pos_scu][i][0];
			bst_info->mv[i][0] = mod_info_curr->mv[i][1] = cu_data_temp_ptr->mv[luma_pos_scu][i][1];
        } 
	}
	return 0;
}


static void diff_16b(U7 w, U7 h, s16 src1[16], s16 src2[4 * 4], s16 diff[16])
{

#pragma HLS INLINE
	U15 offset_s1 = 0;
	U15 offset_s2 = 0;
	U15 offset_diff = 0;
	U7 i;
	U7 j;
#pragma HLS ARRAY_PARTITION variable=src2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=diff complete dim=1
	for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
			diff[j + i * 4] = src1[j + i * 4 ] - src2[j + i * 4];
		}

	}
}

void enc_diff_16b(U3 log2w, U3 log2h, s16 src1[16], s16 src2[4 * 4], U7 s_src1, U7 s_src2, U7 s_diff, s16 diff[16])
{

#pragma HLS INLINE off

	U7 w = 1 << log2w;
	U7 h = 1 << log2h;
	{
		diff_16b(w, h, src1, src2,  diff);
	}
}

s64 enc_ssd_16b_1(U3 w_log2, U3 h_log2, s16 *src1, s16 *src2, U8 s_src1, U7 s_src2, U4 bit_depth)
{

#pragma HLS INLINE off
	U7 w = 1 << w_log2;
	U7 h = 1 << h_log2;
	const U2 shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	s16 *s1 = (s16 *)src1;
	s16 *s2 = (s16 *)src2;
	U7 i, j;
	int diff;

	for (i = 0; i < h; i++) {
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < w; j++) {
#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT max=64
			diff = s1[j] - s2[j];
			ssd += ((diff * diff >> shift));
		}

		s1 += s_src1;
		s2 += s_src2;
	}
	return ssd;
	//add by xujch
}

//void cu_plane_nz_cln(int dst[4][3], int plane)
//{
//
//#pragma HLS INLINE off
//	int i;
//	for (i = 0; i < 4; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//		dst[i][plane] = 0;
//	}
//}


static void init_scan(u16 scan[16], int size_x, int size_y, int scan_type)
{

//#pragma HLS INLINE
	int x;
	int y;
	int l;
	int pos;
	int num_line;
	pos = 0;
	num_line = size_x + size_y - 1;
	if (scan_type == 0) {
		/* starting point */
		scan[pos] = ((u16)0);
		pos++;
		/* loop */
		for (l = 1; l < num_line; l++) {

#pragma HLS LOOP_TRIPCOUNT max=127
			/* decreasing loop */
			if (l % 2) {
				x = (l < size_x - 1 ? l : size_x - 1);
				y = (0 > l - (size_x - 1) ? 0 : l - (size_x - 1));
				while (x >= 0 && y < size_y) {

#pragma HLS LOOP_TRIPCOUNT max=64
					scan[pos] = ((y * size_x + x));
					pos++;
					x--;
					y++;
				}
			}
			else
				/* increasing loop */
			{
				y = (l < size_y - 1 ? l : size_y - 1);
				x = (0 > l - (size_y - 1) ? 0 : l - (size_y - 1));
				while (y >= 0 && x < size_x) {

#pragma HLS LOOP_TRIPCOUNT max=64
					scan[pos] = ((y * size_x + x));
					pos++;
					x++;
					y--;
				}
			}
		}
	}
}

void memcpy_rewrintra(s16 *des, s16 *src, int n)
{

#pragma HLS INLINE off
	int z;
	for (z = 0; z < n; z++) {

#pragma HLS LOOP_TRIPCOUNT max=64
		des[z] = src[z];
	}
}

static int is_cu_plane_nz(int nz[4][3], int plane)
{
	int cu_nz = 0;
	int i;
	for (i = 0; i < 4; i++) {

#pragma HLS loop_tripcount max = 4
		cu_nz |= nz[i][plane];
	}
	return cu_nz ? 1 : 0;
}


static void update_chroma_md(ENC_CU_DATA_ARRAY *cu_data_temp_ptr, MD_COM_MODE_BEST *mod_info_curr_ptr, s16 coef_y_pb_part[(1 << 7 >> 1) * (1 << 7 >> 1)], int i,
	MD_COM_MODE_BEST *mod_info_best_ptr, s8 coef_rec_write_flag[1],U4 bit_depth)
{
	int cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	int cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	int cu_width = mod_info_curr_ptr->cu_width;
	int cu_height = mod_info_curr_ptr->cu_height;
	int size_tmp = cu_width * cu_height >> 2;
	int j;

	if (mod_info_best_ptr->cu_mode == 2) {
		mod_info_curr_ptr->num_nz[0][i + 1] = 0;
	}
	if (mod_info_curr_ptr->num_nz[0][i + 1] == 0) {
		mod_info_best_ptr->num_nz[0][i + 1] = 0;
		for (j = 0; j < size_tmp; j++) {

#pragma HLS LOOP_TRIPCOUNT max=1024
            //mod_info_best_ptr->coef[i + 1][j] = ((s16 )0);
			if (i == 0)cu_data_temp_ptr->coef_u[j] = 0;
			else if (i == 1)cu_data_temp_ptr->coef_v[j] = 0;

			if (i == 0){
				cu_data_temp_ptr->reco_u[j] = (((0 > (((1 << bit_depth) - 1 < ( mod_info_curr_ptr->pred_u[j]) ?
					(1 << bit_depth) - 1 : ( mod_info_curr_ptr->pred_u[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->pred_u[j]) ? (1 << bit_depth) - 1 : ( mod_info_curr_ptr->pred_u[j]))))));
			}
			else if (i == 1){
				cu_data_temp_ptr->reco_v[j] = (((0 > (((1 << bit_depth) - 1 < ( mod_info_curr_ptr->pred_v[j]) ?
					(1 << bit_depth) - 1 : ( mod_info_curr_ptr->pred_v[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->pred_v[j]) ? (1 << bit_depth) - 1 : ( mod_info_curr_ptr->pred_v[j]))))));
			}
        }
		coef_rec_write_flag[0] = 1;
    } else {
        mod_info_best_ptr->num_nz[0][i + 1] = mod_info_curr_ptr->num_nz[0][i + 1];
        for (j = 0; j < size_tmp; j++) {

#pragma HLS LOOP_TRIPCOUNT max=1024
			if (i == 0){
				cu_data_temp_ptr->coef_u[j] = coef_y_pb_part[j];
				cu_data_temp_ptr->reco_u[j] = ((pel)((0 >(((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_u[j]) ?
					(1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_u[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_u[j]) ? (1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_u[j]))))));

			}
			else if (i == 1){
				cu_data_temp_ptr->coef_v[j] = coef_y_pb_part[j];
				cu_data_temp_ptr->reco_v[j] = ((pel)((0 >(((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_v[j]) ?
					(1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_v[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_v[j]) ? (1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_v[j]))))));
			}
		     
		}
		coef_rec_write_flag[0] = 1;
	}
}

static void update_chroma_md1(ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef_y_pb_part[(1 << 7 >> 1) * (1 << 7 >> 1)], U1 i, U1 coef_rec_write_flag[1],U2 inter_mode, U4 bit_depth)
{
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	U7 cu_width = mod_info_curr_ptr->cu_width;
	U7 cu_height = mod_info_curr_ptr->cu_height;
	U12 size_tmp = cu_width * cu_height >> 2;
	int j;

	if (inter_mode == 2) {
		mod_info_curr_ptr->num_nz[0][i + 1] = 0;
		//mod_info_best_ptr->num_nz[0][i + 1] = 0;
	}
	if (mod_info_curr_ptr->num_nz[0][i + 1] == 0) {
		//mod_info_best_ptr->num_nz[0][i + 1] = 0;
		for (j = 0; j < size_tmp; j++) {

#pragma HLS LOOP_TRIPCOUNT max=1024
			if (i == 0)cu_data_temp_ptr->coef_u[j] = 0;
			else if (i == 1)cu_data_temp_ptr->coef_v[j] = 0;
			if (i == 0) {
				cu_data_temp_ptr->reco_u[j] = (((0 > (((1 << bit_depth) - 1 < (mod_info_curr_ptr->pred_u[j]) ?
					(1 << bit_depth) - 1 : (mod_info_curr_ptr->pred_u[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->pred_u[j]) ? (1 << bit_depth) - 1 : (mod_info_curr_ptr->pred_u[j]))))));
			}
			else if (i == 1) {
				cu_data_temp_ptr->reco_v[j] = (((0 > (((1 << bit_depth) - 1 < (mod_info_curr_ptr->pred_v[j]) ?
					(1 << bit_depth) - 1 : (mod_info_curr_ptr->pred_v[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->pred_v[j]) ? (1 << bit_depth) - 1 : (mod_info_curr_ptr->pred_v[j]))))));
			}
		}
		coef_rec_write_flag[0] = 1;
	}
	else {
		//mod_info_curr_ptr->num_nz[0][i + 1] = mod_info_best_ptr->num_nz[0][i + 1];
		for (j = 0; j < size_tmp; j++) {

#pragma HLS LOOP_TRIPCOUNT max=1024
			if (i == 0) {
				cu_data_temp_ptr->coef_u[j] = coef_y_pb_part[j];
				cu_data_temp_ptr->reco_u[j] = ((pel)((0 > (((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_u[j]) ?
					(1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_u[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_u[j]) ? (1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_u[j]))))));

			}
			else if (i == 1) {
				cu_data_temp_ptr->coef_v[j] = coef_y_pb_part[j];
				cu_data_temp_ptr->reco_v[j] = ((pel)((0 > (((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_v[j]) ?
					(1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_v[j]))) ? 0 : (((1 << bit_depth) - 1 < (mod_info_curr_ptr->rec_v[j]) ? (1 << bit_depth) - 1 : (mod_info_curr_ptr->rec_v[j]))))));
			}

		}
		coef_rec_write_flag[0] = 1;
	}
}


void copy_to_cu_data_chroma(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1])
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
		if (coef_rec_write_flag[0] == 0) {////for Intra_CU
			for (i = 0; i < ((mod_info_best->cu_width * mod_info_best->cu_height) >> 2); i++) {
				cu_data->coef_u[i] = mod_info_best->coef_u[i];
				cu_data->coef_v[i] = mod_info_best->coef_v[i];
				cu_data->reco_u[i] = mod_info_best->rec_u[i];
				cu_data->reco_v[i] = mod_info_best->rec_v[i];  /* copy reco */

			}

		}

		for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height / 4; i++) {
			num_coef_u += ((cu_data->coef_u[i]) != 0 ? 1 : 0);
			num_coef_v += ((cu_data->coef_v[i]) != 0 ? 1 : 0);
		}
	}
	if (tree_status != TREE_C)
	{
		for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height; i++) {
			num_coef_y += ((cu_data->coef_y[i]) != 0 ? 1 : 0);
		}
	}
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
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((1 << 24));
				}
				else {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((~(1 << 24)));
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


void print_chroma(MD_COM_MODE_BEST* mod_info, int cnt, pel pred[2][16][16], pel resi[2][16][16])
{
	printf("Num %d:\n", cnt);
	int w = mod_info->cu_width >> 1, h = mod_info->cu_height >> 1;
	//printf("pred_u:\n");
	//for (int i = 0; i < h; ++i){
	//	for (int j = 0; j < w; ++j){
	//		printf("%d ", pred[0][i][j]);
	//	}
	//	printf("\n");
	//}
	//printf("pred_v:\n");
	//for (int i = 0; i < h; ++i){
	//	for (int j = 0; j < w; ++j){
	//		printf("%d ", pred[1][i][j]);
	//	}
	//	printf("\n");
	//}
	//printf("resi_u:\n");
	//for (int i = 0; i < h; ++i){
	//	for (int j = 0; j < w; ++j){
	//		printf("%d ", resi[0][i][j]);
	//	}
	//	printf("\n");
	//}
	//printf("resi_v:\n");
	//for (int i = 0; i < h; ++i){
	//	for (int j = 0; j < w; ++j){
	//		printf("%d ", resi[1][i][j]);
	//	}
	//	printf("\n");
	//}
	printf("coef_u:\n");
	for (int i = 0; i < h; ++i){
		for (int j = 0; j < w; ++j){
			printf("%d ", mod_info->coef_u[i * w + j]);
		}
		printf("\n");
	}
	printf("coef_v:\n");
	for (int i = 0; i < h; ++i){
		for (int j = 0; j < w; ++j){
			printf("%d ", mod_info->coef_v[i * w + j]);
		}
		printf("\n");
	}
	printf("rec_u:\n");
	for (int i = 0; i < h; ++i){
		for (int j = 0; j < w; ++j){
			printf("%d ", mod_info->rec_u[i * w + j]);
		}
		printf("\n");
	}
	printf("rec_v:\n");
	for (int i = 0; i < h; ++i){
		for (int j = 0; j < w; ++j){
			printf("%d ", mod_info->rec_v[i * w + j]);
		}
		printf("\n");
	}
	printf("ipm: %d\n", mod_info->ipm[0][1]);
	printf("num_nz:\n");
	for (int i = 1; i < 3; ++i){
		printf("%d ", mod_info->num_nz[0][i]);
	}
	printf("\n");
}

static void sort_inter_mode(s64 arr[17], u8 index[17], U1 mode_vaild[17], u8 valid_num, int all_mode_num)
{
	s64 min_arr, temp_arr;
	u8 min_index, temp_index;
	int start = 0;

	for (start = 0; start < valid_num; start++)
	{
#pragma HLS LOOP_TRIPCOUNT max=17
		min_arr = MAX_S64;
		min_index = start;
		for (int i = start; i < all_mode_num; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=17
			if (mode_vaild[index[i]] && (arr[i] < min_arr))
			{
				min_arr = arr[i];
				min_index = i;
			}
		}

		if (min_index != start)
		{
			temp_arr = arr[start];
			arr[start] = arr[min_index];
			arr[min_index] = temp_arr;

			temp_index = index[start];
			index[start] = index[min_index];
			index[min_index] = temp_index;
		}
	}
}

static int check_same_mode(int all_mode_num, int max_rdo_mode_num, u8 valid_num[1], s64 dist_all[17], U1 mode_vaild[17], int start, int end)
{
	int ref = start;
	int mode_num = 1;

	for (int i = start + 1; i <= end; i++)
	{
#pragma HLS loop_tripcount max=8
		if (mode_vaild[i])
		{
			mode_num++;
			if (dist_all[i] == dist_all[ref])
			{
				if (valid_num[0] > max_rdo_mode_num)
				{
					mode_vaild[i] = 0;
					valid_num[0]--;
					mode_num--;
				}
			}
			else
				ref = i;
		}
	}

	return mode_num;
}

int choose_inter_rdo_mode(int max_rdo_num, int all_mode_num, s64 dist_all[17], U1 mode_vaild[17])
{
#pragma hls inline off
	int skip_merge_num = 0, affine_me_num = 0;
	u8 valid_num[1] = { 0 };
	u8 mode_list_all[17];

	for (u8 mode_idx = 0; mode_idx < all_mode_num; mode_idx++)
	{
#pragma HLS LOOP_TRIPCOUNT max=17
		mode_list_all[mode_idx] = mode_idx;
		if (mode_vaild[mode_idx])
		{
			valid_num[0]++;
		}
	}


	skip_merge_num = check_same_mode(all_mode_num, max_rdo_num, valid_num, dist_all, mode_vaild, 0, 9 - 1);

	affine_me_num = check_same_mode(all_mode_num, max_rdo_num, valid_num, dist_all, mode_vaild, 9, all_mode_num - 1);

	if (skip_merge_num + affine_me_num > max_rdo_num)
	{
		if (skip_merge_num >= max_rdo_num)
		{
			if (affine_me_num == 0)
				skip_merge_num = max_rdo_num;
			else
			{
				affine_me_num = 1;
				skip_merge_num = max_rdo_num - 1;
			}
		}
		else
		{
			affine_me_num = max_rdo_num - skip_merge_num;
		}
	}

	sort_inter_mode(dist_all, mode_list_all, mode_vaild, valid_num[0], all_mode_num);

	for (int i = 0; i < all_mode_num; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=17
		u8 mode_idx = mode_list_all[i];
		if (mode_vaild[mode_idx])
		{
			if (mode_idx < 9)
			{
				if (skip_merge_num > 0)
					skip_merge_num--;
				else
					mode_vaild[mode_idx] = 0;
			}
			else
			{
				if (affine_me_num > 0)
					affine_me_num--;
				else
					mode_vaild[mode_idx] = 0;
			}
		}
	}

	return avs3_min(valid_num[0], max_rdo_num);
}

//****************************analyze_inter_intra_cu for all sizes*****************************


//***************************************************************************************
void com_get_mpm(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY *MPM_buffer_ptr, ENC_ME_LINE_MAP *me_line_ptr)
{
#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_ptr->min_scu_x;
	int pic_y_scu = y_scu - me_line_ptr->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_ptr->min_scu_x) {
        if (x_scu > 0
                && ((me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu] >> 15 & (1)))
                && ((me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu] >> 31 & (1)))) {
            if (mpm_x == 0) {
                ipm_l = MPM_buffer_ptr->left_buffer[mpm_y];
            } else {
                ipm_l = MPM_buffer_ptr->buffer[mpm_y][mpm_x - 1];
            }
            valid_l = 1;
        }
    } else {
        if (x_scu > 0
                && ((me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu] >> 31 & (1)))) {
            if (mpm_x == 0) {
                ipm_l = MPM_buffer_ptr->left_buffer[mpm_y];
            } else {
                ipm_l = MPM_buffer_ptr->buffer[mpm_y][mpm_x - 1];
            }
            valid_l = 1;
        }
    }
    if (y_scu == me_line_ptr->min_scu_y) {
        if (y_scu > 0
			&& ((me_line_ptr->new_map_scu_above_line[x_scu - me_line_ptr->min_scu_x + SCUX_OFFSET] >> 15 & (1)))
			&& ((me_line_ptr->new_map_scu_above_line[x_scu - me_line_ptr->min_scu_x + SCUX_OFFSET] >> 31 & (1)))) {
            if (mpm_y == 0) {
                ipm_u = (MPM_buffer_ptr->up[x_scu]);
            } else {
                ipm_u = (MPM_buffer_ptr->buffer[mpm_y - 1][mpm_x]);
            }
            valid_u = 1;
        }
    } else {
        if (y_scu > 0
                && ((me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1] >> 15 & (1)))
                && ((me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1] >> 31 & (1)))) {
            if (mpm_y == 0) {
                ipm_u = (MPM_buffer_ptr->up[x_scu]);
            } else {
                ipm_u = (MPM_buffer_ptr->buffer[mpm_y - 1][mpm_x]);
            }
            valid_u = 1;
        }
    }
    mpm[0] = (ipm_l< ipm_u?ipm_l:ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0]==mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}

u16 com_get_avail_intra_ip(IP_MAP_SCU *MAP_SCU_IP_ptr, U11 x_scu, U11 y_scu)
{

#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu - MAP_SCU_IP_ptr->min_scu_x;
	U11 pic_y_scu = y_scu - MAP_SCU_IP_ptr->min_scu_y;
	if (x_scu == MAP_SCU_IP_ptr->min_scu_x) {
        if (x_scu > 0
                && ((MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu] >> 31 & (1)))) {
            avail |= 1 << 1;
        }
    } else {
        if (x_scu > 0
                && ((MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu] >> 31 & (1)))) {
            avail |= 1 << 1;
        }
	}
    if (y_scu > 0) {
        if (y_scu == MAP_SCU_IP_ptr->min_scu_y) {
            if ((MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu] >> 31 & (1))) {
                avail |= 1 << 0;
            }
        } else {
            if ((MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu][pic_y_scu - 1] >> 31 & (1))) {
                avail |= 1 << 0;
            }
        }
	}
	if (y_scu == MAP_SCU_IP_ptr->min_scu_y) {
        if (x_scu > 0)
            if (x_scu > 0
                    && ((MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu - 1] >> 31 & (1)))) {
                avail |= 1 << 2;
            }
    } else if (x_scu == MAP_SCU_IP_ptr->min_scu_x) {
        if (x_scu > 0
                && ((MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu - 1] >> 31 & (1)))) {
            avail |= 1 << 2;
        }
    } else {
        if (x_scu > 0
                && ((MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu - 1] >> 31 & (1)))) {
            avail |= 1 << 2;
        }
	}
	return avail;
}

static int com_dt_allow(int cu_w, int cu_h, int pred_mode, int max_dt_size)
{

#pragma HLS INLINE off
//only allow intra DT
    if (pred_mode != 0) {
		return 0;
	}
	int max_size = max_dt_size;
	int min_size = 16;
	int hori_allow = cu_h >= min_size && (cu_w <= max_size && cu_h <= max_size) && cu_w < cu_h * 4;
	int vert_allow = cu_w >= min_size && (cu_w <= max_size && cu_h <= max_size) && cu_h < cu_w * 4;
	return hori_allow + (vert_allow << 1);
}

//void update_intra_info_map_scu(ENC_ME_LINE_MAP *me_line_map_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, int tb_x, int tb_y, int tb_w, int tb_h,
//	int pic_width_in_scu, int ipm)
//{
//	int i, j;
//#pragma HLS INLINE off
//	int scu_x = tb_x >> 2;
//	int scu_y = tb_y >> 2;
//	int scu_w = tb_w >> 2;
//	int scu_h = tb_h >> 2;
//	int scu_x_line = scu_x - me_line_map_ptr->min_scu_x;
//	int scu_y_line = scu_y - me_line_map_ptr->min_scu_y;
//	int mpm_x = scu_x % 16;
//	int mpm_y = scu_y % 16;
//	//   map_scu = map_scu + scu_y * pic_width_in_scu + scu_x;
//	//   map_ipm = map_ipm + scu_y * pic_width_in_scu + scu_x;
//	for (j= 0; j < scu_h; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=3
//		for (i = 0; i < scu_w; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=3
//			//     MCU_SET_CODED_FLAG(map_scu[i]);
//			//     MCU_SET_INTRA_FLAG(map_scu[i]);
//			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] =
//				me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] | ((
//				1 << 31));
//			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] =
//				me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] | ((
//				1 << 15));
//			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] =
//				MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] | ((
//				1 << 31));
//			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] =
//				MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][scu_x_line + i][scu_y_line + j] | ((
//				1 << 15));
//			//			MPM_buffer_ptr->buffer[j + mpm_y][i + mpm_x] = ipm;
//		}
//		//    map_scu += pic_width_in_scu;
//		//   map_ipm += pic_width_in_scu;
//	}
//}

static __inline void com_mset_8b(pel * dst, pel v, int cnt)
{
#pragma HLS INLINE off
	int i;
	for (i = 0; i < cnt; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=67 
		dst[i] = v;
	}
}

void get_IP_pixel_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel *left, pel *up, U12 x, U12 y,
	ENC_ME_LINE_MAP *me_line_ptr, IP_ARRAY *IP_buffer_ptr)
{
	int i;
	int j;
	U6 width_in_scu = ch_type == 0 ? width >> 2 : width >> (2 - 1);
	U6 height_in_scu = ch_type == 0 ? height >> 2 : height >> (2 - 1);
	//4
	U3 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
	U2 unit_size_bit = ch_type == 0 ? 2 : 1;
	//x/4
	U11 x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
	//y/4
	U11 y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
	//number of padding pixel in the left column   32
	U7 pad_le = height;
	//number of padding pixel in the upper row     32
	U7 pad_up = width;
	//16
	U6 pad_le_in_scu = height_in_scu;
	//16
	U6 pad_up_in_scu = width_in_scu;
	U6 pic_x = x % 32;
	U6 pic_y = y % 32;
	com_mset_8b(left, (1 << (bit_depth - 1)), height + pad_le + 3);
	com_mset_8b(up, (1 << (bit_depth - 1)), width + pad_up + 3);
	U11 pic_x_scu = x_scu - me_line_ptr->min_scu_x;
	U11 pic_y_scu = y_scu - me_line_ptr->min_scu_y;
	U1 flag = 0;
	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		if (pic_y == 0) {
			for (i = 0; i < width; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				up[i + 3] = (IP_buffer_ptr->up_chroma[ch_type - 1][x + i]);
			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				//				assert(MCU_GET_CODED_FLAG(map_scu[scup - pic_width_in_scu + width_in_scu + i]) == 1);
                if (x_scu + width_in_scu + i < pic_width_in_scu) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->up_chroma[ch_type - 1][x + width + (i << unit_size_bit) + j]);
                    }
                } else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
                    }
                }
            }
        } else {
            for (i = 0; i < width; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                up[i + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + i]);
            }
            for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_x_scu + width_in_scu + i < 16) {
                    flag = ((int )(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31
                                   & (1)));
                } else {
                    flag = 0;
                }
                if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + width + i *
                                                             unit_size + j]);
                    }
                } else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
                    }
                }
			}
		}
	}
	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0) {
			for (i = 0; i < height; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + i]);
			}
            for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_y_scu + height_in_scu + i < 16) {
                    flag = ((int )(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
                } else {
                    flag = 0;
                }
                if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + height + (i << unit_size_bit) +
                                                                j]);
                    }
                } else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
                    }
                }
            }
        } else {
            for (i = 0; i < height; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
                left[i + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y + i][pic_x - 1]);
            }
            for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
                if (pic_y_scu + height_in_scu + i < 16) {
                    flag = ((int )(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31
                                   & (1)));
                } else {
                    flag = 0;
                }
                if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][height + (i << unit_size_bit) + j
                                                                + pic_y][pic_x - 1]);
                    }
                } else {
                    for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
                        left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
                    }
                }
            }
        }
    }
    if ((((int )avail_cu) & 1 << 2) == 1 << 2) {
        if (pic_y == 0 && pic_x == 0) {
            up[2] = left[2] = (IP_buffer_ptr->top_left_chroma[ch_type - 1]);
        } else if (pic_y == 0) {
            up[2] = left[2] = (IP_buffer_ptr->up_chroma[ch_type - 1][x - 1]);
        } else if (pic_x == 0) {
            up[2] = left[2] = (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y - 1]);
        } else {
            up[2] = left[2] = (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x - 1]);
        }
    } else if ((((int )avail_cu) & 1 << 0) == 1 << 0) {
        up[2] = left[2] = up[3];
    } else if ((((int )avail_cu) & 1 << 1) == 1 << 1) {
        up[2] = left[2] = left[3];
    }
    up[1] = left[3];
    left[1] = up[3];
    up[0] = left[4];
    left[0] = up[4];
}

void clip_pred(pel dst[4 * 4], const int w, const int h, int bit_depth)
{
#pragma HLS INLINE
	com_assert(NULL != dst);
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
	for (int i = 0; i < 4; i++)
	{
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++)
		{
#pragma HLS UNROLL
			dst[i * w + j] = COM_CLIP3(0, ((1 << bit_depth) - 1), dst[i * w + j]);
		}
	}
}

void com_ipred_uv_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], S8 ipm_c, S8 ipm, U7 w, U7 h, U4 bit_depth,
	u16 avail_cu, U2 chType, pel piRecoY[8][8], pel nb_y[2][2 * 8 + 3]
#if MIPF
	, int mipf_enable_flag
#endif
)
{
#pragma HLS INLINE
#if USE_TSCPM
	int bAbove = IS_AVAIL(avail_cu, AVAIL_UP);
	int bLeft = IS_AVAIL(avail_cu, AVAIL_LE);
#endif
	pel dst1[16];
	pel dst2[16];
	pel dst3[16];
	pel dst4[16];
	pel dst5[16];
	pel dst6[16];
	pel dst7[16];
	pel dst8[16];
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst3 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst4 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst5 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst6 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst7 complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst8 complete dim=1
	if (ipm_c == 0 && (ipm == 12 || ipm == 24 || ipm == 0 || ipm == 2)) {
		ipm_c = (ipm == 12 ? 3 : ((ipm == 24 ? 2 : ((ipm == 0 ? 1 : 4)))));
	}
	switch (ipm_c) {
	case 0:
		switch (ipm) {
		case 1:
			ipred_plane_4(src_le + 2, src_up + 2, dst1, bit_depth, w, h);
			for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
				dst[i] = dst1[i];
			}
			break;
		default:
#if EIPM
			if (ipm > 33) {
				ipred_ang_eipm_4(src_le + 3, src_up + 3, dst2, w, h, ipm
#if MIPF
					, 0, mipf_enable_flag , bit_depth
#endif
				);
				clip_pred(dst2, w, h, bit_depth);
				for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
					dst[i] = dst2[i];
				}
			}			
			else {
#endif
				ipred_ang_4(src_le, src_up, dst3, bit_depth, w, h, ipm, 1
#if MIPF
					, 0, mipf_enable_flag
#endif
				);
				for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
					dst[i] = dst3[i];
				}
			}
			break;
		}
		break;
	case 1:
		ipred_dc_4(src_le + 3, src_up + 3, dst4, w, h, bit_depth, avail_cu);
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			dst[i] = dst4[i];
		}
		break;
	case 2:
		ipred_hor_4(src_le + 3, dst5, w, h);
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			dst[i] = dst5[i];
		}
		break;
	case 3:
		ipred_vert_4(src_up + 3, dst6, w, h);
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			dst[i] = dst6[i];
		}
		break;
	case 4:
		ipred_bi_4(src_le + 3, src_up + 3, dst7, bit_depth, w, h);
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			dst[i] = dst7[i];
		}
		break;
#if USE_TSCPM
	case 5:
		ipred_tscpm_4(dst8, piRecoY, 64, w, h, bAbove, bLeft, bit_depth, src_le + 3, src_up + 3, nb_y[0], nb_y[1]);
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			dst[i] = dst8[i];
		}
		break;
#endif
	default:
#if MD_PRINT
		printf("\n illegal chroma intra prediction mode\n");
#endif
		break;
	}
}

//void get_IP_pixel_4_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
//	U4 bit_depth, U2 ch_type, pel left[11], pel up[11], U12 x, U12 y,
//	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
//	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
//	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
//{
//	int i;
//	int j;
//	U6 width_in_scu = ch_type == 0 ? width >> 2 : width >> (2 - 1);
//	U6 height_in_scu = ch_type == 0 ? height >> 2 : height >> (2 - 1);
//	//4
//	U3 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
//	U2 unit_size_bit = ch_type == 0 ? 2 : 1;
//	//x/4
//	U11 x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
//	//y/4
//	U11 y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
//	//number of padding pixel in the left column   32
//	U7 pad_le = height;
//	//number of padding pixel in the upper row     32
//	U7 pad_up = width;
//	//16
//	U6 pad_le_in_scu = height_in_scu;
//	//16
//	U6 pad_up_in_scu = width_in_scu;
//	U6 pic_x = x % 32;
//	U6 pic_y = y % 32;
////	com_mset_8b(left , (1 << (bit_depth - 1)), height + pad_le + 3);
//	for (int i = 0; i < 11; i++)
//	{
//#pragma HLS UNROLL
//		left[i] = 1 << (bit_depth - 1);
//	}
////	com_mset_8b(up, (1 << (bit_depth - 1)), width + pad_up + 3);
//	for (int i = 0; i < 11; i++)
//	{
//#pragma HLS UNROLL 
//		up[i] = 1 << (bit_depth - 1);
//	}
//	U11 pic_x_scu = x_scu & 15;
//	U11 pic_y_scu = y_scu & 15;
//	U1 flag = 0;
//	if (((avail_cu) & 1 << 0) == 1 << 0) {
//		for (i = 0; i < width; i++)
//		{
//#pragma HLS LOOP_TRIPCOUNT max=8
//			up[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_4x4->up_line[i]) : (IP_buffer_ptr_V_4x4->up_line[i]);
//		}
//		for (i = 0; i < pad_up_in_scu; i++)
//		{
//#pragma HLS LOOP_TRIPCOUNT max=2
//			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
//			{
//				//flag = pic_y == 0 ?
//				//	1 :
//				//	((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31 & (1)));
//				flag = pic_y == 0 ?
//					1 :
//					((int)(IP_map_scu_ptr_8x8->map_scu_up_line[width_in_scu + i] >> 31 & (1)));
//			}
//			else
//				flag = 0;
//
//			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
//			{
//				for (j = 0; j < unit_size; j++)
//				{
//#pragma HLS LOOP_TRIPCOUNT max=4
//					up[width + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
//						(IP_buffer_ptr_U_4x4->up_line[width + (i << unit_size_bit) + j]) :
//						(IP_buffer_ptr_V_4x4->up_line[width + (i << unit_size_bit) + j]);
//				}
//			}
//			else
//			{
//				for (j = 0; j < unit_size; j++)
//				{
//#pragma HLS LOOP_TRIPCOUNT max=4
//					up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
//				}
//			}
//		}
//	}
//	if (((avail_cu) & 1 << 1) == 1 << 1) {
//		if (pic_x == 0) 
//		{
//			for (i = 0; i < height; i++) 
//			{
//#pragma HLS LOOP_TRIPCOUNT max=8
//				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_4x4->left_line[i]) : (IP_buffer_ptr_V_4x4->left_line[i]);
//			}
//			for (i = 0; i < pad_le_in_scu; i++) 
//			{
//#pragma HLS LOOP_TRIPCOUNT max=2
//				if (pic_y_scu + height_in_scu + i < 16) 
//				{
//					//flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
//					flag = ((int)(IP_map_scu_ptr_8x8->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
//				}
//				else 
//				{
//					flag = 0;
//				}
//				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) 
//				{
//					for (j = 0; j < unit_size; j++) 
//					{
//#pragma HLS LOOP_TRIPCOUNT max=4
//						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ? 
//							(IP_buffer_ptr_U_4x4->left_line[height + (i << unit_size_bit) + j]) :
//							(IP_buffer_ptr_V_4x4->left_line[height + (i << unit_size_bit) + j]);
//					}
//				}
//				else 
//				{
//					for (j = 0; j < unit_size; j++)
//					{
//#pragma HLS LOOP_TRIPCOUNT max=4
//						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
//					}
//				}
//			}
//		}
//		else {
//			for (i = 0; i < height; i++) 
//			{
//#pragma HLS LOOP_TRIPCOUNT max=8
//				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_4x4->left_line[i]) : (IP_buffer_ptr_V_4x4->left_line[i]);
//			}
//			for (i = 0; i < pad_le_in_scu; i++) 
//			{
//#pragma HLS LOOP_TRIPCOUNT max=2
//				if (pic_y_scu + height_in_scu + i < 16) 
//				{
//					//flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31 & (1)));
//					flag = ((int)(IP_map_scu_ptr_8x8->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
//				}
//				else 
//				{
//					flag = 0;
//				}
//				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) 
//				{
//					for (j = 0; j < unit_size; j++) 
//					{
//#pragma HLS LOOP_TRIPCOUNT max=4
//						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ? 
//							(IP_buffer_ptr_U_4x4->left_line[height + (i << unit_size_bit) + j]) :
//							(IP_buffer_ptr_V_4x4->left_line[height + (i << unit_size_bit) + j]);
//					}
//				}
//				else 
//				{
//					for (j = 0; j < unit_size; j++) 
//					{
//#pragma HLS LOOP_TRIPCOUNT max=4
//						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
//					}
//				}
//			}
//		}
//	}
//	if (((avail_cu) & 1 << 2) == 1 << 2) {
//		up[2] = left[2] = ch_type == 1 ? (IP_buffer_ptr_U_4x4->top_left) : (IP_buffer_ptr_V_4x4->top_left);
//	}
//	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
//		up[2] = left[2] = up[3];
//	}
//	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
//		up[2] = left[2] = left[3];
//	}
//
//#if 0
//	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
//		if (pic_y == 0) {
//			for (i = 0; i < width; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=8
//				assert(up[i + 3] == (IP_buffer_ptr->up_chroma[ch_type - 1][x + i]));
//			}
//			for (i = 0; i < pad_up_in_scu; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=2
//				//				assert(MCU_GET_CODED_FLAG(map_scu[scup - pic_width_in_scu + width_in_scu + i]) == 1);
//				if (x_scu + width_in_scu + i < pic_width_in_scu) {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(up[width + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->up_chroma[ch_type - 1][x + width + (i << unit_size_bit) + j]));
//					}
//				}
//				else {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(up[width + (i << unit_size_bit) + j + 3] == up[width + (i << unit_size_bit) + 2]);
//					}
//				}
//			}
//		}
//		else {
//			for (i = 0; i < width; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=8
//				assert(up[i + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + i]));
//			}
//			for (i = 0; i < pad_up_in_scu; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=2
//				if (pic_x_scu + width_in_scu + i < 16) {
//					flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31
//						& (1)));
//				}
//				else {
//					flag = 0;
//				}
//				if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(up[width + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + width + i *
//							unit_size + j]));
//					}
//				}
//				else {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(up[width + (i << unit_size_bit) + j + 3] == up[width + (i << unit_size_bit) + 2]);
//					}
//				}
//			}
//		}
//	}
//	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
//		if (pic_x == 0) {
//			for (i = 0; i < height; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=8
//				assert(left[i + 3] == (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + i]));
//			}
//			for (i = 0; i < pad_le_in_scu; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=2
//				if (pic_y_scu + height_in_scu + i < 16) {
//					flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
//				}
//				else {
//					flag = 0;
//				}
//				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(left[height + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + height + (i << unit_size_bit) +
//							j]));
//					}
//				}
//				else {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(left[height + (i << unit_size_bit) + j + 3] == left[height + (i << unit_size_bit) + 2]);
//					}
//				}
//			}
//		}
//		else {
//			for (i = 0; i < height; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=8
//				assert(left[i + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y + i][pic_x - 1]));
//			}
//			for (i = 0; i < pad_le_in_scu; i++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=2
//				if (pic_y_scu + height_in_scu + i < 16) {
//					flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31
//						& (1)));
//				}
//				else {
//					flag = 0;
//				}
//				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(left[height + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][height + (i << unit_size_bit) + j
//							+ pic_y][pic_x - 1]));
//					}
//				}
//				else {
//					for (j = 0; j < unit_size; j++) {
//
//#pragma HLS LOOP_TRIPCOUNT max=4
//						assert(left[height + (i << unit_size_bit) + j + 3] == left[height + (i << unit_size_bit) + 2]);
//					}
//				}
//			}
//		}
//	}
//	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
//		if (pic_y == 0 && pic_x == 0) {
//			assert(up[2] == (IP_buffer_ptr->top_left_chroma[ch_type - 1]));
//		}
//		else if (pic_y == 0) {
//			assert(up[2] == (IP_buffer_ptr->up_chroma[ch_type - 1][x - 1]));
//		}
//		else if (pic_x == 0) {
//			assert(up[2] == (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y - 1]));
//		}
//		else {
//			assert(up[2] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x - 1]));
//		}
//	}
//	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
//		up[2] = left[2] = up[3];
//	}
//	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
//		up[2] = left[2] = left[3];
//	}
//
//#endif // 1
//
//
//	up[1] = left[3];
//	left[1] = up[3];
//	up[0] = left[4];
//	left[0] = up[4];
//}

void get_IP_pixel_4_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, pel left_u[11], pel up_u[11], pel left_v[11], pel up_v[11], U12 x, U12 y,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
{
#pragma HLS INLINE
	int i;
	int j;
	U6 width_in_scu = width >> (2 - 1);
	U6 height_in_scu = height >> (2 - 1);
	//4
	U3 unit_size = 1 << 2 >> 1;
	U2 unit_size_bit = 1;
	//x/4
	U11 x_scu = (( x << 1)) >> 2;
	//y/4
	U11 y_scu = ((y << 1)) >> 2;
	//number of padding pixel in the left column   32
	U7 pad_le = height;
	//number of padding pixel in the upper row     32
	U7 pad_up = width;
	//16
	U6 pad_le_in_scu = height_in_scu;
	//16
	U6 pad_up_in_scu = width_in_scu;
	U6 pic_x = x % 32;
	U6 pic_y = y % 32;
#pragma HLS ARRAY_PARTITION variable=left_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=up_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=left_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=up_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_4x4->left_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_4x4->up_line complete dim=1
	//	com_mset_8b(left , (1 << (bit_depth - 1)), height + pad_le + 3);
	for (int i = 0; i < 11; i++)
	{
#pragma HLS UNROLL
		left_u[i] = 1 << (bit_depth - 1);
		left_v[i] = 1 << (bit_depth - 1);
		up_u[i] = 1 << (bit_depth - 1);
		up_v[i] = 1 << (bit_depth - 1);
	}
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < 4; i++)
		{
#pragma HLS UNROLL
			up_u[i + 3] = IP_buffer_ptr_U_4x4->up_line[i];
			up_v[i + 3] = IP_buffer_ptr_V_4x4->up_line[i];
		}
		for (i = 0; i < 2; i++)
		{
#pragma HLS UNROLL
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				//flag = pic_y == 0 ?
				//	1 :
				//	((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31 & (1)));
				flag = pic_y == 0 ?
					1 :
					((int)(IP_map_scu_ptr_8x8->map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else
				flag = 0;

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < 2; j++)
				{
#pragma HLS UNROLL
					up_u[4 + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_U_4x4->up_line[4 + (i << unit_size_bit) + j];
			
					up_v[4 + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_V_4x4->up_line[4 + (i << unit_size_bit) + j];
				}
			}
			else
			{
				for (j = 0; j < 2; j++)
				{
#pragma HLS UNROLL
					up_u[4 + (i << unit_size_bit) + j + 3] = up_u[4 + (i << unit_size_bit) + 2];
					up_v[4 + (i << unit_size_bit) + j + 3] = up_v[4 + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0)
		{
			for (i = 0; i < 4; i++)
			{
#pragma HLS UNROLL
				left_u[i + 3] = IP_buffer_ptr_U_4x4->left_line[i];
				left_v[i + 3] = IP_buffer_ptr_V_4x4->left_line[i];
			}
			for (i = 0; i < 2; i++)
			{
#pragma HLS UNROLL
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_8x8->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
				}
				else
				{
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
				{
					for (j = 0; j < 2; j++)
					{
#pragma HLS UNROLL
						left_u[4 + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_U_4x4->left_line[4 + (i << unit_size_bit) + j];
						left_v[4 + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_V_4x4->left_line[4 + (i << unit_size_bit) + j];
					}
				}
				else
				{
					for (j = 0; j < 2; j++)
					{
#pragma HLS UNROLL
						left_u[4 + (i << unit_size_bit) + j + 3] = left_u[4 + (i << unit_size_bit) + 2];
						left_v[4 + (i << unit_size_bit) + j + 3] = left_v[4 + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < 4; i++)
			{
#pragma HLS UNROLL
				left_u[i + 3] = IP_buffer_ptr_U_4x4->left_line[i];
				left_v[i + 3] = IP_buffer_ptr_V_4x4->left_line[i];
			}
			for (i = 0; i < 2; i++)
			{
#pragma HLS UNROLL
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_8x8->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
				}
				else
				{
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
				{
					for (j = 0; j < 2; j++)
					{
#pragma HLS UNROLL
						left_u[height + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_U_4x4->left_line[height + (i << unit_size_bit) + j];
						left_v[height + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_V_4x4->left_line[height + (i << unit_size_bit) + j];
					}
				}
				else
				{
					for (j = 0; j < 2; j++)
					{
#pragma HLS UNROLL
						left_u[height + (i << unit_size_bit) + j + 3] = left_u[height + (i << unit_size_bit) + 2];
						left_v[height + (i << unit_size_bit) + j + 3] = left_v[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up_u[2] = left_u[2] = IP_buffer_ptr_U_4x4->top_left;
		up_v[2] = left_v[2] = IP_buffer_ptr_V_4x4->top_left;
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up_u[2] = left_u[2] = up_u[3];
		up_v[2] = left_v[2] = up_v[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up_u[2] = left_u[2] = left_u[3];
		up_v[2] = left_v[2] = left_v[3];
	}

#if 0
	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		if (pic_y == 0) {
			for (i = 0; i < width; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(up[i + 3] == (IP_buffer_ptr->up_chroma[ch_type - 1][x + i]));
			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				//				assert(MCU_GET_CODED_FLAG(map_scu[scup - pic_width_in_scu + width_in_scu + i]) == 1);
				if (x_scu + width_in_scu + i < pic_width_in_scu) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(up[width + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->up_chroma[ch_type - 1][x + width + (i << unit_size_bit) + j]));
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(up[width + (i << unit_size_bit) + j + 3] == up[width + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
		else {
			for (i = 0; i < width; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(up[i + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + i]));
			}
			for (i = 0; i < pad_up_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_x_scu + width_in_scu + i < 16) {
					flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31
						& (1)));
				}
				else {
					flag = 0;
				}
				if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(up[width + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x + width + i *
							unit_size + j]));
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(up[width + (i << unit_size_bit) + j + 3] == up[width + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0) {
			for (i = 0; i < height; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(left[i + 3] == (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + i]));
			}
			for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16) {
					flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
				}
				else {
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[height + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y + height + (i << unit_size_bit) +
							j]));
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[height + (i << unit_size_bit) + j + 3] == left[height + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
		else {
			for (i = 0; i < height; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(left[i + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y + i][pic_x - 1]));
			}
			for (i = 0; i < pad_le_in_scu; i++) {

#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16) {
					flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31
						& (1)));
				}
				else {
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag) {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[height + (i << unit_size_bit) + j + 3] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][height + (i << unit_size_bit) + j
							+ pic_y][pic_x - 1]));
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[height + (i << unit_size_bit) + j + 3] == left[height + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
		if (pic_y == 0 && pic_x == 0) {
			assert(up[2] == (IP_buffer_ptr->top_left_chroma[ch_type - 1]));
		}
		else if (pic_y == 0) {
			assert(up[2] == (IP_buffer_ptr->up_chroma[ch_type - 1][x - 1]));
		}
		else if (pic_x == 0) {
			assert(up[2] == (IP_buffer_ptr->left_chroma[ch_type - 1][pic_y - 1]));
		}
		else {
			assert(up[2] == (IP_buffer_ptr->temp_buffer_chroma[ch_type - 1][pic_y - 1][pic_x - 1]));
		}
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
	}

#endif // 1


	up_u[1] = left_u[3];
	up_v[1] = left_v[3];
	left_u[1] = up_u[3];
	left_v[1] = up_v[3];
	up_u[0] = left_u[4];
	up_v[0] = left_v[4];
	left_u[0] = up_u[4];
	left_v[0] = up_v[4];
}




#if USE_TSCPM
static void get_IP_pixel_8_for_chroma(U8 w, U8 h, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel left[2 * 8 + 3], pel up[2 * 8 + 3], U14 x, U14 y,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
{
#pragma HLS INLINE
	int i;
	int j;
	//	int width_in_scu = ch_type == 0 ? w >> 2 : w >> (2 - 1);
	int width_in_scu = 2;
	//	int height_in_scu = ch_type == 0 ? h >> 2 : h >> (2 - 1);
	int height_in_scu = 2;
	//4
	int unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
	int unit_size_bit = ch_type == 0 ? 2 : 1;
	//x/4
	int x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
	//y/4
	int y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
	//number of padding pixel in the left column   64
	int pad_le = h;
	//number of padding pixel in the upper row     64
	int pad_up = w;
	//16
	int pad_le_in_scu = height_in_scu;
	//16
	int pad_up_in_scu = width_in_scu;
	int pic_x = x & 63;//x%64
	int pic_y = y & 63;//y%64
	int pic_x_scu = x_scu & 15;
	int pic_y_scu = y_scu & 15;
	int flag;
#pragma HLS ARRAY_PARTITION variable=left complete dim=1
#pragma HLS ARRAY_PARTITION variable=up complete dim=1
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_8x8->up_line complete dim=1
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_8x8->left_line complete dim=1

	S7 ip_array_size_idx = get_ip_array_size_idx(w, h);
#if MD_PRINT
	assert(ip_array_size_idx != -1);
#endif

	if ((avail_cu & 1 << 0) == 1 << 0)
	{
		for (i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			up[i + 3] = IP_buffer_ptr_8x8->up_line[i];
		}
		for (i = 0; i < 2; i++)
		{
#pragma HLS UNROLL
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
#if CDEBUG_MAP_SCU
				if (pic_y == 0)
					assert(IP_map_scu_ptr_8x8->map_scu_up_line[width_in_scu + i] == MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu + width_in_scu + i]);
				else
					assert(IP_map_scu_ptr_8x8->map_scu_up_line[width_in_scu + i] == MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1]);
#endif // CDEBUG_MAP_SCU
				//flag = (pic_y == 0 ? ((int)(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu + width_in_scu + i] >> 31 & ((unsigned int)1))) :
				//	((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31 & ((unsigned int)1))));
				//if (pic_y == 0)
				//	flag = (IP_map_scu_ptr_8x8->map_scu_above_line[width_in_scu + i] >> 31 & 1); 
				//else
				//{
				//	assert(IP_map_scu_ptr_8x8->map_scu_inner_up[width_in_scu + i] == MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1]);
				//	flag = (IP_map_scu_ptr_8x8->map_scu_inner_up[width_in_scu + i] >> 31 & 1);
				//}

				flag = (IP_map_scu_ptr_8x8->map_scu_up_line[width_in_scu + i] >> 31 & 1);
			}
			else
				flag = 0;

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < 4; j++)
				{
#pragma HLS UNROLL
					up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_8x8->up_line[w + (i << unit_size_bit) + j];
				}
			}
			else
			{
				for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
					up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if ((avail_cu & (1 << 1)) == 1 << 1)
	{
		for (i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			left[i + 3] = IP_buffer_ptr_8x8->left_line[i];
		}
		for (i = 0; i < 2; i++)
		{
#pragma HLS UNROLL
			if (pic_y_scu + 2 + i < 16)
			{
				//if (pic_x == 0)
				//{
				//	assert(IP_map_scu_ptr_8x8->map_scu_left_line[2 + i] == MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + 2 + i]);
				//	//flag = MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + 2 + i] >> 31 & 1;
				//	flag = IP_map_scu_ptr_8x8->map_scu_left_line[2 + i] >> 31 & 1;
				//}
				//else
				//{
				//	assert(IP_map_scu_ptr_8x8->map_scu_inner_left[2 + i] == MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + 2 + i]);
				//	//flag = MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + 2 + i] >> 31 & 1;
				//	flag = IP_map_scu_ptr_8x8->map_scu_inner_left[2 + i] >> 31 & 1;
				//}
				flag = IP_map_scu_ptr_8x8->map_scu_left_line_new[2 + i] >> 31 & 1;
			}
			else
			{
				flag = 0;
			}
			if (y_scu + 2 + i < pic_height_in_scu && flag)
			{
				for (j = 0; j < 4; j++)
				{
#pragma HLS UNROLL
					left[8 + (i << 2) + j + 3] = IP_buffer_ptr_8x8->left_line[8 + (i << 2) + j];
				}
			}
			else
			{
				for (j = 0; j < 4; j++)
				{
#pragma HLS UNROLL
					left[8 + (i << 2) + j + 3] = left[8 + (i << 2) + 2];
				}
			}
		}
	}
#if CDEBUG
	if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		if (pic_y == 0) {
			for (i = 0; i < w; i++) {
#pragma HLS LOOP_TRIPCOUNT max=8
				assert(up[i + 3] == IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + i]);
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
						assert(up[w + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + w + (i << unit_size_bit) + j]);
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(up[w + (i << unit_size_bit) + j + 3] == up[w + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
		else {
			for (i = 0; i < w; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(up[i + 3] == IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + i]);
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
						assert(up[w + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + w + (i << unit_size_bit) +
							j]);
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(up[w + (i << unit_size_bit) + j + 3] == up[w + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
	}


	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0) {
			for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(left[i + 3] == IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + i]);
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
						assert(left[h + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + h + (i << unit_size_bit) + j]);
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[h + (i << unit_size_bit) + j + 3] == left[h + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
		else {
			for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=8
				assert(left[i + 3] == IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][pic_y + i][(pic_x >> 2) - 1]);
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
						assert(left[h + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][h + (i << unit_size_bit) + j + pic_y][(pic_x >>
							2) - 1]);
					}
				}
				else {
					for (j = 0; j < unit_size; j++) {

#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[h + (i << unit_size_bit) + j + 3] == left[h + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
	}
#endif
	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
		up[2] = left[2] = IP_buffer_ptr_8x8->top_left;
	}
#if CDEBUG
	if ((((int)avail_cu) & 1 << 2) == 1 << 2) {
		if (pic_y == 0 && pic_x == 0) {
			assert(up[2] == ((pel)(IP_buffer_ptr->top_left)));
		}
		else if (pic_y == 0) {
			assert(up[2] == IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x - 1]);
		}
		else if (pic_x == 0) {
			assert(up[2] == IP_buffer_ptr->left_size[ip_array_size_idx][pic_y - 1]);
		}
		else {
			assert(up[2] == IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x - 1]);
		}
	}
#endif

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
#endif

u16 com_get_avail_intra_ip_chroma_4(U11 x_scu, U11 y_scu, IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
{

#pragma HLS INLINE 
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x8->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x8->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_8x8->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_8x8->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_8x8->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x8->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x8->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}

void ipred_c_prepare(MD_FW* md_fw_ptr, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status,
	U1 uv, U13 pb_x, U13 pb_y,  MD_COM_MODE_BEST* mod_info_curr_ptr,
	s16 coef_tmp[12][4 * 4], pel nb_y[2][2 * 8 + 3],
	pel nb_u[2][2 * 4 + 3], pel nb_v[2][2 * 4 + 3],
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	U4 bit_depth, pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output)
{
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=nb_u complete dim=1
#pragma HLS ARRAY_PARTITION variable=nb_v complete dim=1
#pragma HLS ARRAY_PARTITION variable=nb_y complete dim=1
	u16 avail_cu = com_get_avail_intra_ip_chroma_4(mod_info_curr_ptr->x_scu, mod_info_curr_ptr->y_scu, IP_map_scu_ptr_8x8);

	get_IP_pixel_4_chroma(1 << cu_width_log2, 1 << cu_height_log2,
		avail_cu,
		md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu,
		bit_depth, 
		nb_u[0], nb_u[1], nb_v[0], nb_v[1], pb_x >> 1, pb_y >> 1,
		IP_buffer_ptr_U_4x4,
		IP_buffer_ptr_V_4x4,
		IP_map_scu_ptr_8x8);

#if USE_TSCPM
	get_IP_pixel_8_for_chroma(1 << (cu_width_log2 + 1), 1 << (cu_height_log2 + 1), avail_cu, md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu, bit_depth, 0,
		nb_y[0], nb_y[1], pb_x, pb_y,
		IP_buffer_ptr_8x8,
		IP_map_scu_ptr_8x8);
#endif

	*avail_cu_output = avail_cu;
}

 void ipred_c_4(MD_FW* md_fw_ptr, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status,
	U1 uv, U13 pb_x, U13 pb_y, pel pred[4 * 4], MD_COM_MODE_BEST* mod_info_curr_ptr,
	s16 coef_tmp[16], pel orgU[16], pel nb_y[2][2 * 8 + 3],
	pel nb1[2 * 4 + 3],pel nb2[2 * 4 + 3],S8 rdo_idx,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	U4 bit_depth, pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output)
{
#pragma HLS INLINE

	u16 avail_cu = *avail_cu_output;

	com_ipred_uv_4(nb1, nb2, pred, rdo_idx/2, mod_info_curr_ptr->ipm[0][0],
		1 << cu_width_log2, 1 << cu_height_log2, bit_depth, avail_cu, uv + 1, piRecoY, nb_y
#if MIPF
		, md_fw_ptr->mipf_enable_flag
#endif
	);
//	enc_diff_16b(cu_width_log2, cu_height_log2, orgU, pred, 4, 1 << cu_width_log2, 1 << cu_width_log2, coef_tmp);
}


static int sbac_write_truncate_unary_sym_est(u32 sym, u32 max_num)
{

#pragma HLS INLINE
	int enc_est = 0;
	u32 ctx_idx = 0;
	u32 num;
	num = (max_num - ((unsigned int)1) > sym + ((unsigned int)1) ? sym + ((unsigned int)1) : max_num - ((
		unsigned int)1));
	//int enc_est_1;
	enc_est = ((int)(((unsigned int)enc_est) + num));
	/*do{
	enc_est++;
	ctx_idx++;
	} while (ctx_idx < max_num - 1 && sym--);*/
	//printf("%d  ", enc_est);
	//printf("%d  ", enc_est_1);
	return enc_est;
}
static int enc_eco_run_est(u32 sym)
{

#pragma HLS INLINE
	int exp_golomb_order = 0;
	int enc_est = 0;
	if (sym < (16)) {
		enc_est += sbac_write_truncate_unary_sym_est(sym, (u32)17);
	}
	else {
		sym -= (16);
		enc_est += sbac_write_truncate_unary_sym_est((u32)16, (u32)17);
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 1;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 3;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 5;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 7;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 9;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 11;
		}
		enc_est += 2 * exp_golomb_order;
		/*sbac_write_unary_sym_ep_est(exp_golomb_order);
		sbac_encode_bins_ep_msb_est(exp_golomb_order);*/
	}
	return enc_est;
}




static int enc_eco_level_est(u32 sym)
{

#pragma HLS INLINE
	int exp_golomb_order = 0;
	int enc_est = 0;
	if (sym < ((unsigned int)8)) {
		enc_est += sbac_write_truncate_unary_sym_est(sym, (u32)9);
	}
	else {
		sym -= ((unsigned int)8);
		enc_est += sbac_write_truncate_unary_sym_est((u32)8, (u32)9);
		//		while ((int)sym >= (1 << exp_golomb_order))
		//		{
		//#pragma HLS LOOP_TRIPCOUNT max=11
		//			sym = sym - (1 << exp_golomb_order);
		//			exp_golomb_order++;
		//		}
		if (sym == ((unsigned int)0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= ((unsigned int)1) && sym < ((unsigned int)3)) {
			exp_golomb_order = 1;
		}
		else if (sym >= ((unsigned int)3) && sym < ((unsigned int)7)) {
			exp_golomb_order = 2;
		}
		else if (sym >= ((unsigned int)7) && sym < ((unsigned int)15)) {
			exp_golomb_order = 3;
		}
		else if (sym >= ((unsigned int)15) && sym < ((unsigned int)31)) {
			exp_golomb_order = 4;
		}
		else if (sym >= ((unsigned int)31) && sym < ((unsigned int)63)) {
			exp_golomb_order = 5;
		}
		else if (sym >= ((unsigned int)63) && sym < ((unsigned int)127)) {
			exp_golomb_order = 6;
		}
		else if (sym >= ((unsigned int)127) && sym < ((unsigned int)255)) {
			exp_golomb_order = 7;
		}
		else if (sym >= ((unsigned int)255) && sym < ((unsigned int)511)) {
			exp_golomb_order = 8;
		}
		else if (sym >= ((unsigned int)512) && sym < ((unsigned int)1023)) {
			exp_golomb_order = 9;
		}
		else if (sym >= ((unsigned int)1023) && sym < ((unsigned int)2047)) {
			exp_golomb_order = 10;
		}
		else if (sym >= ((unsigned int)2047) && sym < ((unsigned int)4095)) {
			exp_golomb_order = 11;
		}
		enc_est += 2 * exp_golomb_order;
		//sbac_write_unary_sym_ep_est(exp_golomb_order);
		//sbac_encode_bins_ep_msb_est(exp_golomb_order);
	}
	return enc_est;
}
static int enc_eco_run_length_cc_est_2_8x8(s16 coef[64], int log2_w, int log2_h, int num_sig, int ch_type)
{
	int enc_est = 0;
	u32 num_coeff;
	u32 scan_pos;
	u32 level;
	u32 prev_level;
	u32 run;
	u32 last_flag;
	s16 coef_cur;
	num_coeff = ((u32)64);
	u16 scanp[64] = { ((u16)0), ((u16)1), ((u16)8), ((u16)16), ((u16)9), ((u16)2), ((u16)3), ((u16)10), ((u16)17), ((u16)24), ((u16)32), ((u16)25), ((u16)18), ((u16)11), ((u16)4), ((u16)5), ((u16)12), ((u16)19), ((u16)26), ((u16)33), ((u16)40), ((u16)48), ((u16)41), ((u16)34), ((u16)27), ((u16)20), ((u16)13), ((u16)6), ((u16)7), ((u16)14), ((u16)21), ((u16)28), ((u16)35), ((u16)42), ((u16)49), ((u16)56), ((u16)57), ((u16)50), ((u16)43), ((u16)36), ((u16)29), ((u16)22), ((u16)15), ((u16)23), ((u16)30), ((u16)37), ((u16)44), ((u16)51), ((u16)58), ((u16)59), ((u16)52), ((u16)45), ((u16)38), ((u16)31), ((u16)39), ((u16)46), ((u16)53), ((u16)60), ((u16)61), ((u16)54), ((u16)47), ((u16)55), ((u16)62), ((u16)63) };
	run = 0;
	prev_level = ((u32)6);
	for (scan_pos = 0; scan_pos < num_coeff; scan_pos++) {

#pragma HLS UNROLL

#pragma HLS loop_tripcount max = 64
		coef_cur = coef[scanp[scan_pos]];
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est += enc_eco_run_est(run);
			enc_est += enc_eco_level_est(level - ((unsigned int)1));
			enc_est++;
			if (scan_pos == num_coeff - ((unsigned int)1)) {
				break;
			}
			run = 0;
			num_sig--;
			/* Last flag coding */
			last_flag = ((u32)((num_sig == 0 ? 1 : 0)));
			enc_est++;
			prev_level = level;
			if (last_flag) {
				break;
			}
		}
		else {
			run++;
		}
	}
	return enc_est;
}


static int enc_eco_run_length_cc_est_2(s16 coef[16], int log2_w, int log2_h, int num_sig, int ch_type)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1

	int enc_est = 0;
	u32 num_coeff;
	u32 scan_pos;
	u32 level;
	u32 prev_level;
	u32 run;
	u32 last_flag;
	num_coeff = 16; //((u32)(1 << (log2_w + log2_h)));

//	static u16 scanp[1 << 12];
//	init_scan(scanp, 1 << log2_w, 1 << log2_h, 0);
	run = 0;
	prev_level = ((u32)6);
	int run_1 = 0;
	U15 enc_est1 = 0;
	U15 enc_est2 = 0;
	U15 enc_est3 = 0;
	U15 enc_est4 = 0;

	U10 run1 = 0;
	U10 run2 = 0;
	U10 run3 = 0;
	U10 run4 = 0;

	U8 runflag1 = 0;
	U8 runflag2 = 0;
	U8 runflag3 = 0;
	U8 runflag4 = 0;

	s16 coef_cur;
	static s16 run_num[16];
	static int num_sig_num[16];
	int scan_pos_1;
	int i;

	s16 scanp_zag[16] = { 0,1,4,8,5,2,3,6,9,12,13,10,7,11,14,15 };
	s16 coef_zig_zag[16];
#pragma HLS ARRAY_PARTITION variable=scanp_zag complete dim=1

	for (scan_pos_1 = 0; scan_pos_1 < 4; scan_pos_1++) {
#pragma HLS UNROLL	
		u8 blk_pos = scanp_zag[scan_pos_1];
		coef_cur = coef[blk_pos];
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));

			enc_est1 += enc_eco_run_est(run1);
			enc_est1 += enc_eco_level_est(level - ((unsigned int)1));
			enc_est1++;
			run1 = 0;
			enc_est1++;
		}
		else {
			run1++;
		}
		if (scan_pos_1 == 3) {
			runflag1 = run1;
		}
	}
	for (i = 4; i < 8; i++) {
#pragma HLS UNROLL
		s16 coef_cur;
		u8 blk_pos = scanp_zag[i];
		coef_cur = coef[blk_pos];
		if (coef_cur) {
			run2 = 0;
		}
		else {
			run2++;
		}
		if (i == 7) {
			runflag2 = run2;
		}
	}
	for (i = 8; i < 12; i++) {
#pragma HLS UNROLL
		s16 coef_cur;
		u8 blk_pos = scanp_zag[i];
		coef_cur = coef[blk_pos];
		if (coef_cur) {
			run3 = 0;
		}
		else {
			run3++;
		}
		if (i == 11) {
			runflag3 = run3;
		}
	}
	for (scan_pos_1 = 4; scan_pos_1 < 8; scan_pos_1++) {
#pragma HLS UNROLL	
		u8 blk_pos = scanp_zag[scan_pos_1];
		coef_cur = coef[blk_pos];
		if (scan_pos_1 == 4) {
			run = runflag1;
		}
		
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est2 += enc_eco_run_est(run);
			enc_est2 += enc_eco_level_est(level - ((unsigned int)1));
			enc_est2++;
			run = 0;
			enc_est2++;
		}
		else {
			run++;
		}
	}
	for (scan_pos_1 = 8; scan_pos_1 < 12; scan_pos_1++) {
#pragma HLS UNROLL	
		u8 blk_pos = scanp_zag[scan_pos_1];
		coef_cur = coef[blk_pos];
		if (scan_pos_1 == 8) {
			if (runflag2 == 4) {
				run = runflag1 + runflag2;
			}
			else {
				run = runflag2;
			}
		}
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est3 += enc_eco_run_est(run);
			enc_est3 += enc_eco_level_est(level - ((unsigned int)1));
			enc_est3++;
			run = 0;
			enc_est3++;
		}
		else {
			run++;
		}
	}
	for (scan_pos_1 = 12; scan_pos_1 < 16; scan_pos_1++) {
#pragma HLS UNROLL	
		u8 blk_pos = scanp_zag[scan_pos_1];
		coef_cur = coef[blk_pos];
		if (scan_pos_1 == 12) {
			if (runflag3 == 8) {
				if (runflag2 == 4) {
					run = runflag1 + runflag2 + runflag3;
				}
				else {
					run = runflag2 + runflag3;
				}
			}
		}
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est4 += enc_eco_run_est(run);
			enc_est4 += enc_eco_level_est(level - ((unsigned int)1));
			enc_est4++;
			run = 0;
			enc_est4++;
		}
		else {
			run++;
		}
	}
	enc_est = enc_est1 + enc_est2 + enc_est3 + enc_est4;

/*
	for (scan_pos_1 = 0; scan_pos_1 < 16; scan_pos_1++) {
#pragma HLS UNROLL
		coef_cur_1 = coef[scanp_zag[scan_pos_1]];
		if (coef_cur_1) {
			run_num[scan_pos_1] =  ((s16)run_1);
			if (scan_pos_1 == num_coeff - ((unsigned int)1)) {
				break;
			}
			run_1 = 0;
			num_sig_num[scan_pos_1] = --num_sig;
			if (!num_sig) {
				break;
			}
		}
		else {
			run_1++;
			run_num[scan_pos_1] = 0;
		}
	}
	for (scan_pos = 0; scan_pos < 16; scan_pos++) {
#pragma HLS UNROLL
		coef_cur = coef[scanp_zag[scan_pos]];
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est += enc_eco_run_est((u32)run_num[scan_pos]);
			enc_est += enc_eco_level_est(level - ((unsigned int)1));
			enc_est++;
			if (scan_pos == num_coeff - ((unsigned int)1)) {
				break;
			}
			last_flag = ((u32)((num_sig_num[scan_pos] == 0 ? 1 : 0)));
			enc_est++;
			if (last_flag) {
				break;
			}
		}
	}
*/
	return enc_est;
}
static int enc_eco_run_length_cc_est_2_64x64(s16* coef, int log2_w, int log2_h, int num_sig, int ch_type)
{
	int enc_est = 0;
	u32 num_coeff;
	u32 scan_pos;
	u32 level;
	u32 prev_level;
	u32 run;
	u32 last_flag;
	s16 coef_cur;
	num_coeff = ((u32)(1 << (log2_w + log2_h)));
	static u16 scanp[1 << 12];
	init_scan(scanp, 1 << log2_w, 1 << log2_h, 0);
	run = 0;
	prev_level = ((u32)6);
	int run_1 = 0;
	s16 coef_cur_1;
	static s16 run_num[1 << 12];
	static int num_sig_num[1 << 12];
	u32 scan_pos_1;
	for (scan_pos_1 = 0; scan_pos_1 < num_coeff; scan_pos_1++) {

#pragma HLS loop_tripcount max=4096
		coef_cur_1 = coef[scanp[scan_pos_1]];
		if (coef_cur_1) {
			run_num[scan_pos_1] = ((s16)run_1);
			if (scan_pos_1 == num_coeff - ((unsigned int)1)) {
				break;
			}
			run_1 = 0;
			num_sig_num[scan_pos_1] = --num_sig;
			if (!num_sig) {
				break;
			}
		}
		else {
			run_1++;
			run_num[scan_pos_1] = 0;
		}
	}
	for (scan_pos = 0; scan_pos < num_coeff; scan_pos++) {

#pragma HLS loop_tripcount max=4096
		coef_cur = coef[scanp[scan_pos]];
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est += enc_eco_run_est((u32)run_num[scan_pos]);
			enc_est += enc_eco_level_est(level - ((unsigned int)1));
			enc_est++;
			if (scan_pos == num_coeff - ((unsigned int)1)) {
				break;
			}
			last_flag = ((u32)((num_sig_num[scan_pos] == 0 ? 1 : 0)));
			enc_est++;
			if (last_flag) {
				break;
			}
		}
	}
	return enc_est;
}
static int enc_eco_run_length_cc_est_2_32x32(s16* coef, int log2_w, int log2_h, int num_sig, int ch_type)
{
	int enc_est = 0;
	u32 num_coeff;
	u32 scan_pos;
	u32 level;
	u32 prev_level;
	u32 run;
	u32 last_flag;
	s16 coef_cur;
	num_coeff = ((u32)(1 << (log2_w + log2_h)));
	static u16 scanp[1 << 12];
	init_scan(scanp, 1 << log2_w, 1 << log2_h, 0);
	run = 0;
	prev_level = ((u32)6);
	int run_1 = 0;
	s16 coef_cur_1;
	static s16 run_num[1 << 12];
	static int num_sig_num[1 << 12];
	u32 scan_pos_1;
	for (scan_pos_1 = 0; scan_pos_1 < num_coeff; scan_pos_1++) {

#pragma HLS loop_tripcount max=1024
		coef_cur_1 = coef[scanp[scan_pos_1]];
		if (coef_cur_1) {
			run_num[scan_pos_1] = ((s16)run_1);
			if (scan_pos_1 == num_coeff - ((unsigned int)1)) {
				break;
			}
			run_1 = 0;
			num_sig_num[scan_pos_1] = --num_sig;
			if (!num_sig) {
				break;
			}
		}
		else {
			run_1++;
			run_num[scan_pos_1] = 0;
		}
	}
	for (scan_pos = 0; scan_pos < num_coeff; scan_pos++) {

#pragma HLS loop_tripcount max=1024
		coef_cur = coef[scanp[scan_pos]];
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			enc_est += enc_eco_run_est((u32)run_num[scan_pos]);
			enc_est += enc_eco_level_est(level - ((unsigned int)1));
			enc_est++;
			if (scan_pos == num_coeff - ((unsigned int)1)) {
				break;
			}
			last_flag = ((u32)((num_sig_num[scan_pos] == 0 ? 1 : 0)));
			enc_est++;
			if (last_flag) {
				break;
			}
		}
	}
	return enc_est;
}

static int enc_eco_run_length_cc_est_2_16x16(s16* coef, int log2_w, int log2_h, int num_sig, int ch_type)
{
	int enc_est = 0;
	u32 num_coeff;
	u32 scan_pos;
	u32 level;
	u32 prev_level;
	u32 run;
	u32 last_flag;
	s16 coef_cur;
	int run_1 = 0;
	s16 coef_cur_1;
	static s16 run_num[1 << 12];
	static int num_sig_num[1 << 12];
	u32 scan_pos_1;
	num_coeff = ((u32)(1 << (log2_w + log2_h)));
	static u16 scanp[256] = { ((u16)0), ((u16)1), ((u16)16), ((u16)32), ((u16)17), ((u16)2), ((u16)3), ((u16)18), ((u16)33), ((u16)48), ((u16)64), ((u16)49), ((u16)34), ((u16)19), ((u16)4), ((u16)5), ((u16)20), ((u16)35), ((u16)50), ((u16)65), ((u16)80), ((u16)96), ((u16)81), ((u16)66), ((u16)51), ((u16)36), ((u16)21), ((u16)6), ((u16)7), ((u16)22), ((u16)37), ((u16)52), ((u16)67), ((u16)82), ((u16)97), ((u16)112), ((u16)128), ((u16)113), ((u16)98), ((u16)83), ((u16)68), ((u16)53), ((u16)38), ((u16)23), ((u16)8), ((u16)9), ((u16)24), ((u16)39), ((u16)54), ((u16)69), ((u16)84), ((u16)99), ((u16)114), ((u16)129), ((u16)144), ((u16)160), ((u16)145), ((u16)130), ((u16)115), ((u16)100), ((u16)85), ((u16)70), ((u16)55), ((u16)40), ((u16)25), ((u16)10), ((u16)11), ((u16)26), ((u16)41), ((u16)56), ((u16)71), ((u16)86), ((u16)101), ((u16)116), ((u16)131), ((u16)146), ((u16)161), ((u16)176), ((u16)192), ((u16)177), ((u16)162), ((u16)147), ((u16)132), ((u16)117), ((u16)102), ((u16)87), ((u16)72), ((u16)57), ((u16)42), ((u16)27), ((u16)12), ((u16)13), ((u16)28), ((u16)43), ((u16)58), ((u16)73), ((u16)88), ((u16)103), ((u16)118), ((u16)133), ((u16)148), ((u16)163), ((u16)178), ((u16)193), ((u16)208), ((u16)224), ((u16)209), ((u16)194), ((u16)179), ((u16)164), ((u16)149), ((u16)134), ((u16)119), ((u16)104), ((u16)89), ((u16)74), ((u16)59), ((u16)44), ((u16)29), ((u16)14), ((u16)15), ((u16)30), ((u16)45), ((u16)60), ((u16)75), ((u16)90), ((u16)105), ((u16)120), ((u16)135), ((u16)150), ((u16)165), ((u16)180), ((u16)195), ((u16)210), ((u16)225), ((u16)240), ((u16)241), ((u16)226), ((u16)211), ((u16)196), ((u16)181), ((u16)166), ((u16)151), ((u16)136), ((u16)121), ((u16)106), ((u16)91), ((u16)76), ((u16)61), ((u16)46), ((u16)31), ((u16)47), ((u16)62), ((u16)77), ((u16)92), ((u16)107), ((u16)122), ((u16)137), ((u16)152), ((u16)167), ((u16)182), ((u16)197), ((u16)212), ((u16)227), ((u16)242), ((u16)243), ((u16)228), ((u16)213), ((u16)198), ((u16)183), ((u16)168), ((u16)153), ((u16)138), ((u16)123), ((u16)108), ((u16)93), ((u16)78), ((u16)63), ((u16)79), ((u16)94), ((u16)109), ((u16)124), ((u16)139), ((u16)154), ((u16)169), ((u16)184), ((u16)199), ((u16)214), ((u16)229), ((u16)244), ((u16)245), ((u16)230), ((u16)215), ((u16)200), ((u16)185), ((u16)170), ((u16)155), ((u16)140), ((u16)125), ((u16)110), ((u16)95), ((u16)111), ((u16)126), ((u16)141), ((u16)156), ((u16)171), ((u16)186), ((u16)201), ((u16)216), ((u16)231), ((u16)246), ((u16)247), ((u16)232), ((u16)217), ((u16)202), ((u16)187), ((u16)172), ((u16)157), ((u16)142), ((u16)127), ((u16)143), ((u16)158), ((u16)173), ((u16)188), ((u16)203), ((u16)218), ((u16)233), ((u16)248), ((u16)249), ((u16)234), ((u16)219), ((u16)204), ((u16)189), ((u16)174), ((u16)159), ((u16)175), ((u16)190), ((u16)205), ((u16)220), ((u16)235), ((u16)250), ((u16)251), ((u16)236), ((u16)221), ((u16)206), ((u16)191), ((u16)207), ((u16)222), ((u16)237), ((u16)252), ((u16)253), ((u16)238), ((u16)223), ((u16)239), ((u16)254), ((u16)255) };
	run = 0;
	prev_level = ((u32)6);

	for (scan_pos_1 = 0; scan_pos_1 < num_coeff; scan_pos_1++) {

#pragma HLS loop_tripcount max=256
		coef_cur_1 = coef[scanp[scan_pos_1]];
		if (coef_cur_1) {
			run_num[scan_pos_1] = ((s16)run_1);
			if (scan_pos_1 == num_coeff - ((unsigned int)1)) {
				break;
			}
			run_1 = 0;
			num_sig_num[scan_pos_1] = --num_sig;
			if (!num_sig) {
				break;
			}
		}
		else {
			run_1++;
			run_num[scan_pos_1] = 0;
		}
	}
	for (scan_pos = 0; scan_pos < num_coeff; scan_pos++) {

#pragma HLS loop_tripcount max=256
		coef_cur = coef[scanp[scan_pos]];
		if (coef_cur) {
			level = ((u32)((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15)));
			//sign = (coef_cur > 0) ? 0 : 1;
			/* Run coding */
			enc_est += enc_eco_run_est((u32)run_num[scan_pos]);
			/* Level coding */
			enc_est += enc_eco_level_est(level - ((unsigned int)1));
			/* Sign coding */
			enc_est++;
			if (scan_pos == num_coeff - ((unsigned int)1)) {
				break;
			}
			//	run = 0;
			//		num_sig--;
			/* Last flag coding */
			last_flag = ((u32)((num_sig_num[scan_pos] == 0 ? 1 : 0)));
			enc_est++;
			if (last_flag) {
				break;
			}
		}
	}
	return enc_est;
}

static int enc_eco_xcoef_est(s16 coef[4096], int log2_w, int log2_h, int num_sig, int ch_type)
{

#pragma HLS INLINE off
	int enc_est;

	if (log2_h == 3 && log2_w == 3) {
		enc_est = enc_eco_run_length_cc_est_2_8x8(coef, log2_w, log2_h, num_sig, (ch_type == 0 ? 0 : 1));
	}
	else if (log2_h == 4 && log2_w == 4) {
		enc_est = enc_eco_run_length_cc_est_2_16x16(coef, log2_w, log2_h, num_sig, (ch_type == 0 ? 0 : 1));
	}
	else if (log2_h == 5 && log2_w == 5) {
		enc_est = enc_eco_run_length_cc_est_2_32x32(coef, log2_w, log2_h, num_sig, (ch_type == 0 ? 0 : 1));
	}
	else if (log2_h == 6 && log2_w == 6) {
		enc_est = enc_eco_run_length_cc_est_2_64x64(coef, log2_w, log2_h, num_sig, (ch_type == 0 ? 0 : 1));
	}
	else {
		enc_est = enc_eco_run_length_cc_est_2(coef, log2_w, log2_h, num_sig, (ch_type == 0 ? 0 : 1));
	}
	return enc_est;
}

static int enc_eco_run_est_2_only(U10 sym)
{
#pragma HLS INLINE 
	U5 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (sym == 65) {
		enc_est = 1;
	}
	else if (sym == 0) {
		enc_est = 0;
	}
	else if (sym < (16)) {
		enc_est += sym + 1;
	}
	else {
		sym -= (16);
		enc_est += 16;
		if (sym == (0)) {
			exp_golomb_order = 1;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 3;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 5;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 7;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 9;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 11;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 13;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 15;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 17;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 19;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 21;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 23;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}
static int enc_eco_level_est_2_only(u16 sym)
{
#pragma HLS INLINE
	U5 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (sym == 0)
		enc_est = 0;
	else if (sym < (9)) {
		enc_est = sym;
		enc_est += 2;
	}
	else {
		sym -= (9);
		//enc_est += sbac_write_truncate_unary_sym_est(8, 9);
		enc_est = 10;
		if (sym == (0)) {
			exp_golomb_order = 1;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 3;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 5;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 7;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 9;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 11;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 13;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 15;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 17;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 19;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 21;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 23;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}
static int rate_coef_level_est_32(s16 coef[32], u8 w)
{
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 32; i++)
	{
		if (i < w)
		{
			u16  level;
			level = abs(coef[i]);
			//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
			//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
			enc_est += enc_eco_level_est_2_only(level);

		}
	}
	return enc_est;
}
static int rate_coef_level_est_16(s16 coef[16], u8 w)
{
#pragma HLS INLINE
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 16; i++)
	{
		if (i < w)
		{
			u16  level;
			level = abs(coef[i]);
			enc_est += enc_eco_level_est_2_only(level);
		}
	}
	return enc_est;
}
static int rate_coef_level_est_8(s16 coef[8], u8 w)
{
#pragma HLS INLINE
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 8; i++)
	{
		if (i < w)
		{
			u16  level;
			level = abs(coef[i]);
			enc_est += enc_eco_level_est_2_only(level);
		}
	}
	return enc_est;
}

static int enc_eco_coef_32_runlevel_chroma(U8 cu_width, U8 cu_height, s16 coef[32][32]) {


	int final_rate = 0;
	int last_x[32];  // the last non-zero position of scan-line.
	int last_y[32];

	int first_x[32]; // the first non-zero position of scan-line.
	int first_y[32];

	int sumRate[32];// = { 0, };
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0

	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	//memset(last_x, -1, sizeof(int) * 32);
	//memset(last_y, -1, sizeof(int) * 32);

	//memset(first_x, -1, sizeof(int) * 32);
	//memset(first_y, -1, sizeof(int) * 32);
	int shift_t;
	if (cu_width == 4 || cu_height == 4) {
		shift_t = 4;
	}
	else if (cu_width == 8 || cu_height == 8) {
		shift_t = 8;
	}
	else if (cu_width == 16 || cu_height == 16) {
		shift_t = 16;
	}
	else {
		shift_t = 32;
	}
	s16 coef_tmp[32];
	s16 coef_out[32];
	int max_num = 0;
	U15 enc_est = 0;
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	for (int j = 0; j < cu_width; j++) // vertical
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=32 max=32 avg=32
		//int zzz = 0;
		for (int i = 0; i < 32; i++) // horizontal
		{
			coef_tmp[i] = coef[i][j];
		}
		if (cu_height == 32) {
			max_num = cu_width - j;
		}
		else {
			max_num = (j < cu_height ? cu_height : cu_height - j);
		}
		for (int i = 0; i < 32; i++)
		{
			if ((i < max_num)&&(i < cu_height)) {
				coef_out[i] = coef_tmp[i];
			}
		}
		enc_est += rate_coef_level_est_32(coef_out, max_num);
		for (int i = 0; i < 32; i++) // horizontal
		{
			if (i < cu_height) {
			if (j == 0) // first input row data.
			{
					if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else if (i == shift_t - 1) {
				last_x[i] = -1;
				last_y[i] = -1;

				first_x[i] = -1;
				first_y[i] = -1;

					if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else
			{
				if ((i + j) % 2 == 0) // above-right-scan
				{
						if (first_x[i] == -1 && coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
					}

						if (coef_tmp[i])
					{
						if (last_x[i] != -1)
						{
							int offset = last_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}
						last_x[i] = i;
						last_y[i] = j;
					}
				}
				else // above - top scan for (i+j)%2 is even case
				{
						if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
					{
						last_x[i] = i;
						last_y[i] = j;
					}

						if (coef_tmp[i] && first_x[i] != -1)
					{
						int offset = first_x[i] - i;
						if (offset == 1) // previou is non-zero coeff. 
						{
							sumRate[i] += 1;
						}
						else
						{
							// should change here.
							sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
						}
					}

						if (coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
						}
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			if (j < shift_t) {
				cur_zig_zag_num = j + 1;
			}
			else {
				cur_zig_zag_num = shift_t;
			}
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[0];
			}
			else
			{
				if (j < shift_t) {
					cur_zig_zag_num = j - first_x[0];
				}
				else {
					cur_zig_zag_num = shift_t - 1 - first_x[0];
				}
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				if (j < shift_t) {
					final_zig_zag = j - last_x[0]; // update
				}
				else {
					final_zig_zag = shift_t - 1 - last_x[0];
				}
			}
			else
			{
				final_zig_zag = j - last_y[0]; // update
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 32; i++)
		{
			if (i < shift_t) {
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
			}
		}
	}
	return (final_rate + enc_est);
}

//static int  enc_eco_coef_32_level_chroma(U8 cu_width, U8 cu_height, s16 coef[32][32])
//{
//	U15 enc_est = 0;
//	static s16 coef_out[32];
//	int max_num = 0;
//	for (U7 z = 0; z < 32; z++) {
//		coef_out[z] = 0;
//	}
//	for (int j = 0; j < cu_width; j++)
//	{
//		if (cu_height == 32) {
//			max_num = cu_width - j;
//		}
//		else {
//			max_num = (j < cu_height ? cu_height : cu_height - j);
//		}
//		for (int i = 0; i < cu_height; i++)
//		{
//			if (i < max_num) {
//				coef_out[i] = coef[i][(i + j) & 31];
//			}
//		}
//		enc_est += rate_coef_level_est_32(coef_out, max_num);
//	}

//	return enc_est;


//}

void enc_eco_coef_16_runlevel_chroma_mux(int j, s16 coef_tmp[16], s16 coef[16][16])
{
#pragma HLS INLINE 
	if (j == 0) {
		for (int i = 0; i < 16; i++)  {coef_tmp[i] = coef[(i + j) & 15][j];}
	}
	else if (j == 1) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 2) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 3) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 4) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 5) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 6) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 7) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 8) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 9) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 10) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 11) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 12) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 13) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 14) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
	else if (j == 15) {
		for (int i = 0; i < 16; i++) { coef_tmp[i] = coef[(i + j) & 15][j]; }
	}
}
static int enc_eco_coef_16_runlevel_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16]) {
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	int final_rate = 0;
	int last_x[16];  // the last non-zero position of scan-line.
	int last_y[16];

	int first_x[16]; // the first non-zero position of scan-line.
	int first_y[16];

	int sumRate[16];// = { 0, };
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0
	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	int shift_t;
	if (cu_width == 8 || cu_height == 8) {
		shift_t = 8;
	}
	else {
		shift_t = 16;
	}
	s16 coef_tmp[16];
	s16 coef_out[16];
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	U15 enc_est = 0;
	int max_num = 0;
	//printf("%d %d\n", cu_width, cu_height);
	/*if (cu_width == 16 && cu_height == 8) {
		int zzz = 0;
	}*/
	for (int j = 0; j < cu_width; j++) // vertical
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=16 max=16 avg=16
		//if (j < cu_width) {

			//{
			//	coef_tmp[i] = coef[(i + j) & 15][j];
			//}
			enc_eco_coef_16_runlevel_chroma_mux( j,  coef_tmp,  coef);
			if (cu_height == 16) {
				max_num = 16 - j;
			}
			else {
				max_num = (j < cu_height ? cu_height : cu_height - j);
			}
			for (int i = 0; i < 16; i++)
			{
				if (i < max_num) {
					coef_out[i] = coef_tmp[i];
				}
			}
			enc_est += rate_coef_level_est_16(coef_out, max_num);
			int zzz = 0;
			for (int i = 0; i < 16; i++) // horizontal
			{
				if (i < cu_height) {
					if (j == 0) // first input row data.
					{
						if (coef_tmp[i])
						{
							last_x[i] = i;
							last_y[i] = j;

							first_x[i] = i;
							first_y[i] = j;
						}
					}
					else if (i == shift_t - 1) {
						last_x[i] = -1;
						last_y[i] = -1;

						first_x[i] = -1;
						first_y[i] = -1;

						if (coef_tmp[i])
						{
							last_x[i] = i;
							last_y[i] = j;

							first_x[i] = i;
							first_y[i] = j;
						}
					}
					else
					{
						if ((i + j) % 2 == 0) // above-right-scan
						{
							if (first_x[i] == -1 && coef_tmp[i])
							{
								first_x[i] = i;
								first_y[i] = j;
							}

							if (coef_tmp[i])
							{
								if (last_x[i] != -1)
								{
									int offset = last_x[i] - i;
									if (offset == 1) // previou is non-zero coeff. 
									{
										sumRate[i] += 1;
									}
									else
									{
										sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
									}
								}
								last_x[i] = i;
								last_y[i] = j;
							}
						}
						else // above - top scan for (i+j)%2 is even case
						{
							if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
							{
								last_x[i] = i;
								last_y[i] = j;
							}

							if (coef_tmp[i] && first_x[i] != -1)
							{
								int offset = first_x[i] - i;
								if (offset == 1) // previou is non-zero coeff. 
								{
									sumRate[i] += 1;
								}
								else
								{
									// should change here.
									sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
								}
							}

							if (coef_tmp[i])
							{
								first_x[i] = i;
								first_y[i] = j;
							}
						}
					}
				}
			}
			// calculate the rate of the last coeff.
			int cur_zig_zag_num;

			if (last_x[0] == -1) // all zeros for current scan-line.
			{
				if (j < shift_t) {
					cur_zig_zag_num = j + 1;
				}
				else {
					cur_zig_zag_num = shift_t;
				}
				final_zig_zag += cur_zig_zag_num;
			}
			else
			{
				if (j % 2 == 1) // bottom -left scan
				{
					cur_zig_zag_num = first_x[0];
				}
				else
				{
					if (j < shift_t) {
						cur_zig_zag_num = j - first_x[0];
					}
					else {
						cur_zig_zag_num = shift_t - 1 - first_x[0];
					}
				}

				final_zig_zag += cur_zig_zag_num;

				// rate _cal:
				// get(final_zig_zag); 
				if (final_zig_zag == 0) // only one non-zero coeff.
				{
					final_rate += 1;
				}
				else
				{
					final_rate += enc_eco_run_est_2_only(final_zig_zag);
				}

				if (j % 2 == 1) // bottom -left scan
				{
					if (j < shift_t) {
						final_zig_zag = j - last_x[0]; // update
					}
					else {
						final_zig_zag = shift_t - 1 - last_x[0];
					}
				}
				else
				{
					final_zig_zag = j - last_y[0]; // update
				}
				/*if (cu_width == 16 && cu_height == 8)
					printf("%d\n", final_zig_zag);*/
				final_rate += sumRate[0];
			}


			// shift register.
			for (int i = 1; i < 16; i++)
			{
				if (i < shift_t) {
					last_x[i - 1] = last_x[i];
					last_y[i - 1] = last_y[i];

					first_x[i - 1] = first_x[i];
					first_y[i - 1] = first_y[i];

					sumRate[i - 1] = sumRate[i];
				}
			}
		//}
	}

	if (cu_width == 8 && cu_height == 16) {
//		int cur_zig_zag_temp[8];
//		int final_zig_zag_temp[8];
//		int final_zig_zag_num[8];
//		for (int i = 0; i < 8; i++)
//		{
//			final_zig_zag_num[i] = 0;
//			final_zig_zag_temp[i] = 0;
//			cur_zig_zag_temp[i] = 0;
//		}
//		for (int i = 0; i < 8; i++)
//		{
//			if (last_x[i] == -1) // all zeros for current scan-line.
//			{
//				cur_zig_zag_temp[i] = 0;
//			}
//			else
//			{
//				if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
//				{
//					cur_zig_zag_temp[i] = first_x[i] - i - 1;
//				}
//				else
//				{
//
//					cur_zig_zag_temp[i] = first_y[i];
//				}
//
//			}
//		}
//		for (int i = 0; i < 8; i++)
//		{
//			if (last_x[i] == -1) 
//			{
//				final_zig_zag_temp[i] = 8;
//			}
//			else
//			{
//				if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
//				{
//
//					final_zig_zag_temp[i] = last_y[i]; // update
//
//				}
//				else
//				{
//					final_zig_zag_temp[i] = last_x[i] - i - 1; // update
//				}
//
//			}
//		}
//
//		if (last_x[0] != -1) {
//			final_zig_zag_num[0] = cur_zig_zag_temp[0] + final_zig_zag;
//		}
//	
//
//		for (int i = 1; i < 8; i++)
//		{
//			if (last_x[i] != -1) {
//				final_zig_zag_num[i] += cur_zig_zag_temp[i];
//				for (int j = i - 1; j >= 0; j--)
//				{
//					if (last_x[j] == -1) {
//						if (j == 0) {
//							final_zig_zag_num[i] += final_zig_zag;
//						}
//						final_zig_zag_num[i] += 8;
//					}
//					else {
//						final_zig_zag_num[i] += final_zig_zag_temp[j];
//						break;
//					}
//				}
//			}
//			
//		}
//
//
//		for (int i = 0; i < 8; i++)
//		{
//#pragma HLS UNROLL
//			if (last_x[i] != -1) {
//				if (final_zig_zag_num[i] == 0) // only one non-zero coeff.
//				{
//					final_rate += 1;
//				}
//				else
//				{
//					final_rate += enc_eco_run_est_2_only(final_zig_zag_num[i]);
//				}
//				final_rate += sumRate[i];
//			}
//		}
		for (int i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			int cur_zig_zag_num;
			if (last_x[i] == -1) // all zeros for current scan-line.
			{
				cur_zig_zag_num = 8;
				final_zig_zag += cur_zig_zag_num;
			}
			else
			{
				if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
				{
					cur_zig_zag_num = first_x[i] - i - 1;
				}
				else
				{

					cur_zig_zag_num = first_y[i];
				}

				final_zig_zag += cur_zig_zag_num;

				// rate _cal:
				// get(final_zig_zag); 
				if (final_zig_zag == 0) // only one non-zero coeff.
				{
					final_rate += 1;
				}
				else
				{
					final_rate += enc_eco_run_est_2_only(final_zig_zag);
				}

				if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
				{

					final_zig_zag = last_y[i]; // update

				}
				else
				{
					final_zig_zag = last_x[i] - i - 1; // update
				}
				final_rate += sumRate[i];
			}
		}
	}
	return (final_rate + enc_est);
}


static int enc_eco_coef_16_16_run_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16]) {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	int final_rate = 0;
	int last_x[16];  // the last non-zero position of scan-line.
	int last_y[16];

	int first_x[16]; // the first non-zero position of scan-line.
	int first_y[16];

	int sumRate[16];// = { 0, };
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0
	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	//memset(last_x, -1, sizeof(int) * 16);
	//memset(last_y, -1, sizeof(int) * 16);

	//memset(first_x, -1, sizeof(int) * 16);
	//memset(first_y, -1, sizeof(int) * 16);
	s16 coef_tmp[16];
	s16 coef_out[16];
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	U15 enc_est = 0;
	for (int j = 0; j < 11; j++) // vertical
	{
#pragma HLS PIPELINE II=1
		for (int i = 0; i < 16; i++) // horizontal
		{
			coef_tmp[i] = coef[(i+j)&15][j];
		}
		int max_num = 0;
		if (j < 9) {
			max_num = 11 - j;
		}
		else {
			max_num = 10 - j;
		}
		for (int i = 0; i < 11; i++)
		{
			if (i < max_num) {
				coef_out[i] = coef_tmp[i];
			}
		}
		enc_est += rate_coef_level_est_16(coef_out, max_num);
		int zzz = 0;
		for (int i = 0; i < 11; i++) // horizontal
		{
			if (j == 0) // first input row data.
			{
				if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}

			else
			{
				if ((i == 0 && j == 10) || (i == 1 && j == 9)) {
					int zzz = 0;
					break;
				}
				else {

					if ((i + j) % 2 == 0) // above-right-scan
					{
						if (first_x[i] == -1 && coef_tmp[i])
						{
							first_x[i] = i;
							first_y[i] = j;
						}

						if (coef_tmp[i])
						{
							if (last_x[i] != -1)
							{
								int offset = last_x[i] - i;
								if (offset == 1) // previou is non-zero coeff. 
								{
									sumRate[i] += 1;
								}
								else
								{
									sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
								}
							}
							last_x[i] = i;
							last_y[i] = j;
						}
					}
					else // above - top scan for (i+j)%2 is even case
					{
						if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
						{
							last_x[i] = i;
							last_y[i] = j;
						}

						if (coef_tmp[i] && first_x[i] != -1)
						{
							int offset = first_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								// should change here.
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}

						if (coef_tmp[i])
						{
							first_x[i] = i;
							first_y[i] = j;
						}
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			cur_zig_zag_num = j + 1;
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[0];
			}
			else
			{
				cur_zig_zag_num = j - first_x[0];
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				final_zig_zag = j - last_x[0]; // update
			}
			else
			{
				final_zig_zag = j - last_y[0]; // update
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 16; i++)
		{
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
		}
	}
	return (final_rate+ enc_est);
}

static int enc_eco_coef_16_8_run_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16]) {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	int final_rate = 0;
	int last_x[16];  // the last non-zero position of scan-line.
	int last_y[16];

	int first_x[16]; // the first non-zero position of scan-line.
	int first_y[16];

	int sumRate[16] = { 0, };
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0
	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	//memset(last_x, -1, sizeof(int) * 16);
	//memset(last_y, -1, sizeof(int) * 16);

	//memset(first_x, -1, sizeof(int) * 16);
	//memset(first_y, -1, sizeof(int) * 16);
	s16 coef_tmp[16];
	s16 coef_out[16];
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	U15 enc_est = 0;
	for (int j = 0; j < 12; j++) // vertical
	{
#pragma HLS PIPELINE II=1
		for (int i = 0; i < 16; i++) // horizontal
		{
			coef_tmp[i] = coef[(i+j)&15][j];
		}
		int max_num;
		if (j < 4) {
			max_num = 8;
		}
		else if (j < 8) {
			max_num = 11 - j;
		}
		else {
			max_num = 12 - j;
		}
		for (int i = 0; i < 8; i++)
		{
			if (i < max_num) {
				coef_out[i] = coef_tmp[i];
			}
		}
		enc_est += rate_coef_level_est_16(coef_out, max_num);
		int zzz = 0;
		for (int i = 0; i < 8; i++) // horizontal
		{
			if (j == 0) // first input row data.
			{
				if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else if (i == 7) {
				last_x[i] = -1;
				last_y[i] = -1;

				first_x[i] = -1;
				first_y[i] = -1;

				if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else
			{
				if ((i == 7 && j == 4) || (i == 6 && j == 5) || (i == 5 && j == 6) || (i == 4 && j == 7)) {
					int zzz = 0;
					break;
				}
				else {

					if ((i + j) % 2 == 0) // above-right-scan
					{
						if (first_x[i] == -1 && coef_tmp[i])
						{
							first_x[i] = i;
							first_y[i] = j;
						}

						if (coef_tmp[i])
						{
							if (last_x[i] != -1)
							{
								int offset = last_x[i] - i;
								if (offset == 1) // previou is non-zero coeff. 
								{
									sumRate[i] += 1;
								}
								else
								{
									sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
								}
							}
							last_x[i] = i;
							last_y[i] = j;
						}
					}
					else // above - top scan for (i+j)%2 is even case
					{
						if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
						{
							last_x[i] = i;
							last_y[i] = j;
						}

						if (coef_tmp[i] && first_x[i] != -1)
						{
							int offset = first_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								// should change here.
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}

						if (coef_tmp[i])
						{
							first_x[i] = i;
							first_y[i] = j;
						}
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			if (j < 8) {
				cur_zig_zag_num = j + 1;
			}
			else if (j < 11) {
				cur_zig_zag_num = 8;
			}
			else {
				cur_zig_zag_num = 4;
			}
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[0];
			}
			else
			{
				if (j < 8) {
					cur_zig_zag_num = j - first_x[0];
				}
				else if (j < 11) {
					cur_zig_zag_num = 8 - 1 - first_x[0];
				}
				else {
					cur_zig_zag_num = 4 - 1 - first_x[0];
				}
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				if (j < 8) {
					final_zig_zag = j - last_x[0]; // update
				}
				else if (j < 11) {
					final_zig_zag = 8 - 1 - last_x[0];
				}
				else {
					final_zig_zag = 4 - 1 - last_x[0];
				}
			}
			else
			{
				final_zig_zag = j - last_y[0]; // update
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 8; i++)
		{
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
		}
	}
	return (final_rate + enc_est);
}

static int enc_eco_coef_8_16_run_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16]) {
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	int final_rate = 0;
	int last_x[16];  // the last non-zero position of scan-line.
	int last_y[16];

	int first_x[16]; // the first non-zero position of scan-line.
	int first_y[16];

	int sumRate[16] = { 0, };
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0
	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	//memset(last_x, -1, sizeof(int) * 16);
	//memset(last_y, -1, sizeof(int) * 16);

	//memset(first_x, -1, sizeof(int) * 16);
	//memset(first_y, -1, sizeof(int) * 16);
	s16 coef_tmp[16];
	s16 coef_out[16];
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	U15 enc_est = 0;
	int max_num = 0;
	for (int j = 0; j < 12; j++) // vertical
	{
#pragma HLS PIPELINE II=1
		for (int i = 0; i < 16; i++) // horizontal
		{
			coef_tmp[i] = coef[(i+j)&15][j];
		}
		if (j < 8) {
			int max_num_tmp;
			if (j < 4) {
				max_num_tmp = 11 - j;
			}
			else {
				max_num_tmp = 12 - j;
			}
			for (int i = 0; i < 12; i++)
			{
				if (i < max_num_tmp) {
					coef_out[i] = coef_tmp[i];
				}
			}
			enc_est += rate_coef_level_est_16(coef_out, max_num_tmp);
		}
		int zzz = 0;
		if (j < 4) {
			max_num = 11 - j;
		}
		else if (j < 8) {
			max_num = 12 - j;
		}
		else {
			max_num = 0;
		}
		for (int i = 0; i < 11; i++) // horizontal
		{
			if (i < max_num) {
			if (j == 0) // first input row data.
			{
				if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else
			{

				if ((i + j) % 2 == 0) // above-right-scan
				{
					if (first_x[i] == -1 && coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
					}

					if (coef_tmp[i])
					{
						if (last_x[i] != -1)
						{
							int offset = last_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}
						last_x[i] = i;
						last_y[i] = j;
					}
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
					{
						last_x[i] = i;
						last_y[i] = j;
					}

					if (coef_tmp[i] && first_x[i] != -1)
					{
						int offset = first_x[i] - i;
						if (offset == 1) // previou is non-zero coeff. 
						{
							sumRate[i] += 1;
						}
						else
						{
							// should change here.
							sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
						}
					}

					if (coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
						}
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			if (j < 8) {
				cur_zig_zag_num = j + 1;
			}
			else if (j < 11) {
				cur_zig_zag_num = 8;
			}
			else {
				cur_zig_zag_num = 4;
			}
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				if (j < 8) {
					cur_zig_zag_num = first_x[0];
				}
				else {
					cur_zig_zag_num = first_x[0] - j + 7;
				}
			}
			else
			{
				cur_zig_zag_num = j - first_x[0];
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				final_zig_zag = j - last_x[0]; // update
			}
			else
			{
				if (j < 8) {
					final_zig_zag = j - last_y[0]; // update
				}
				else {
					final_zig_zag = 7 - last_y[0];
				}
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 16; i++)
		{
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
		}
	}
	return (final_rate + enc_est);
}

static int  enc_eco_coef_16_16_level_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16])
{
	U15 enc_est = 0;
	static s16 coef_out[16];
	int max_num = 0;
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	for (U7 z = 0; z < 16; z++) {
#pragma HLS UNROLL
		coef_out[z] = 0;
	}
	for (int j = 0; j < 11; j++)
	{
		if (j < 9) {
			max_num = 11 - j;
		}
		else {
			max_num = 10 - j;
		}
		for (int i = 0; i < 11; i++)
		{
			if (i < max_num) {
				coef_out[i] = coef[i][j];
			}
		}
		enc_est += rate_coef_level_est_16(coef_out, max_num);
	}

	return enc_est;


}

static int  enc_eco_coef_16_8_level_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16])
{
	U15 enc_est = 0;
	static s16 coef_out[16];
	int max_num = 0;
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	for (U7 z = 0; z < 16; z++) {
#pragma HLS UNROLL
		coef_out[z] = 0;
	}
	for (int j = 0; j < 12; j++)
	{
		if (j < 4) {
			max_num = 8;
		}
		else if (j < 8) {
			max_num = 11 - j;
		}
		else {
			max_num = 12 - j;
		}
		for (int i = 0; i < 8; i++)
		{
			if (i < max_num) {
				coef_out[i] = coef[i][j];
			}
		}
		enc_est += rate_coef_level_est_16(coef_out, max_num);
	}

	return enc_est;


}

static int  enc_eco_coef_8_16_level_chroma(U8 cu_width, U8 cu_height, s16 coef[16][16])
{
	U15 enc_est = 0;
	static s16 coef_out[16];
	int max_num = 0;
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1

#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	for (U7 z = 0; z < 16; z++) {
#pragma HLS UNROLL
		coef_out[z] = 0;
	}
	for (int j = 0; j < 8; j++)
	{
		if (j < 4) {
			max_num = 11 - j;
		}
		else {
			max_num = 12 - j;
		}
		for (int i = 0; i < 12; i++)
		{
			if (i < max_num) {
				coef_out[i] = coef[i][j];
			}
		}
		enc_est += rate_coef_level_est_16(coef_out, max_num);
	}

	return enc_est;


}

static int enc_eco_coef_8_run_chroma(U8 cu_width, U8 cu_height, s16 coef[8][8]) {
//#pragma HLS INLINE OFF
#pragma HLS INLINE 

	int last_x[8];  // the last non-zero position of scan-line.
	int last_y[8];

	int first_x[8]; // the first non-zero position of scan-line.
	int first_y[8];

	int sumRate[8];// = { 0, };
#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0

	//memset(last_x, -1, sizeof(int) * 8);
	//memset(last_y, -1, sizeof(int) * 8);

	//memset(first_x, -1, sizeof(int) * 8);
	//memset(first_y, -1, sizeof(int) * 8);

	s16 coef_tmp[8];
	s16 coef_out[8];
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	U15 enc_est = 0;
	int final_last_x = -1;
	int final_last_y = -1;
	int final_zig_zag = 0;
	int final_rate = 0;
	int shift_t;
	if (cu_height == 4) {
		shift_t = 4;
	}
	else {
		shift_t = 8;
	}
	for (int j = 0; j < cu_width; j++) // vertical
	{
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (int i = 0; i < 8; i++) // horizontal
		{
			coef_tmp[i] = coef[i][j];
		}
		int max_num = cu_height;
		for (int i = 0; i < 8; i++)
		{
			if (i < cu_height) {
				coef_out[i] = coef_tmp[i];
			}
		}
		enc_est += rate_coef_level_est_8(coef_out, max_num);
		//int zzz = 0;
		for (int i = 0; i < 8; i++) // horizontal
		{
			if (i < cu_height) {
			if (j == 0) // first input row data.
			{
					if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else if (i == shift_t - 1) {
				last_x[i] = -1;
				last_y[i] = -1;

				first_x[i] = -1;
				first_y[i] = -1;

					if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else
			{
				if ((i + j) % 2 == 0) // above-right-scan
				{
						if (first_x[i] == -1 && coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
					}

						if (coef_tmp[i])
					{
						if (last_x[i] != -1)
						{
							int offset = last_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}
						last_x[i] = i;
						last_y[i] = j;
					}
				}
				else // above - top scan for (i+j)%2 is even case
				{
						if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
					{
						last_x[i] = i;
						last_y[i] = j;
					}

						if (coef_tmp[i] && first_x[i] != -1)
					{
						int offset = first_x[i] - i;
						if (offset == 1) // previou is non-zero coeff. 
						{
							sumRate[i] += 1;
						}
						else
						{
							// should change here.
							sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
						}
					}

						if (coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
						}
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			if (j < shift_t) {
				cur_zig_zag_num = j + 1;
			}
			else {
				cur_zig_zag_num = shift_t;
			}
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[0];
			}
			else
			{
				if (j < shift_t) {
					cur_zig_zag_num = j - first_x[0];
				}
				else {
					cur_zig_zag_num = shift_t - 1 - first_x[0];
				}
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				if (j < shift_t) {
					final_zig_zag = j - last_x[0]; // update
				}
				else {
					final_zig_zag = shift_t - 1 - last_x[0];
				}
			}
			else
			{
				final_zig_zag = j - last_y[0]; // update
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 8; i++)
		{
			if (i < shift_t) {
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
			}
		}
	}
	//remaining 
	for (int i = 0; i < 8 - 1; i++)
	{
#pragma HLS UNROLL
		if (i < cu_height - 1) {
		int cur_zig_zag_num;
		if (last_x[i] == -1) // all zeros for current scan-line.
		{
			cur_zig_zag_num = cu_height - i - 1;
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[i] - i - 1;
			}
			else
			{

				cur_zig_zag_num = cu_height - 1 - first_x[i];
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
			{

				final_zig_zag = cu_height - last_x[i] - 1; // update

			}
			else
			{
				final_zig_zag = last_x[i] - i - 1; // update
			}
			final_rate += sumRate[i];
		}
	}
	}

	return (final_rate + enc_est);
}
static int enc_eco_coef_4_8_run_chroma(U8 cu_width, U8 cu_height, s16 coef[8][8]) {
#pragma HLS INLINE
//#pragma HLS INLINE OFF
	int final_rate = 0;
	int last_x[8];  // the last non-zero position of scan-line.
	int last_y[8];

	int first_x[8]; // the first non-zero position of scan-line.
	int first_y[8];
	int sumRate[8];// = { 0, };

#pragma HLS ARRAY_PARTITION variable=last_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=0

	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	//memset(last_x, -1, sizeof(int) * 8);
	//memset(last_y, -1, sizeof(int) * 8);

	//memset(first_x, -1, sizeof(int) * 8);
	//memset(first_y, -1, sizeof(int) * 8);
	int shift_t = 8;
	s16 coef_tmp[8];
	s16 coef_out[8];
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=0
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		last_x[i] = -1;
		last_y[i] = -1;
		first_x[i] = -1;
		first_y[i] = -1;
		sumRate[i] = 0;
		coef_out[i] = 0;
	}
	U15 enc_est = 0;
	for (int j = 0; j < 4; j++) // vertical
	{
#pragma HLS PIPELINE II=1
		for (int i = 0; i < 8; i++) // horizontal
		{
			coef_tmp[i] = coef[i][j];
		}
		int max_num = 8;
		for (int i = 0; i < 8; i++)
		{
			coef_out[i] = coef_tmp[i];
		}
		enc_est += rate_coef_level_est_8(coef_out, max_num);

		for (int i = 0; i < 8; i++) // horizontal
		{
			if (j == 0) // first input row data.
			{
				if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else if (i == 7) {
				last_x[i] = -1;
				last_y[i] = -1;

				first_x[i] = -1;
				first_y[i] = -1;

				if (coef_tmp[i])
				{
					last_x[i] = i;
					last_y[i] = j;

					first_x[i] = i;
					first_y[i] = j;
				}
			}
			else
			{
				if ((i + j) % 2 == 0) // above-right-scan
				{
					if (first_x[i] == -1 && coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
					}

					if (coef_tmp[i])
					{
						if (last_x[i] != -1)
						{
							int offset = last_x[i] - i;
							if (offset == 1) // previou is non-zero coeff. 
							{
								sumRate[i] += 1;
							}
							else
							{
								sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
							}
						}
						last_x[i] = i;
						last_y[i] = j;
					}
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (coef_tmp[i] && last_x[i] == -1) // first non-zero position
					{
						last_x[i] = i;
						last_y[i] = j;
					}

					if (coef_tmp[i] && first_x[i] != -1)
					{
						int offset = first_x[i] - i;
						if (offset == 1) // previou is non-zero coeff. 
						{
							sumRate[i] += 1;
						}
						else
						{
							// should change here.
							sumRate[i] += enc_eco_run_est_2_only(offset - 1); // rate_coef_run_est_32(offset, cu_width);
						}
					}

					if (coef_tmp[i])
					{
						first_x[i] = i;
						first_y[i] = j;
					}
				}
			}
		}

		// calculate the rate of the last coeff.
		int cur_zig_zag_num;
		if (last_x[0] == -1) // all zeros for current scan-line.
		{
			cur_zig_zag_num = j + 1;

			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if (j % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = first_x[0];
			}
			else
			{
				cur_zig_zag_num = j - first_x[0];

			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if (j % 2 == 1) // bottom -left scan
			{
				final_zig_zag = j - last_x[0]; // update
			}
			else
			{
				final_zig_zag = j - last_y[0]; // update
			}
			final_rate += sumRate[0];
		}


		// shift register.
		for (int i = 1; i < 8; i++)
		{
			if (i < shift_t) {
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];

			first_x[i - 1] = first_x[i];
			first_y[i - 1] = first_y[i];

			sumRate[i - 1] = sumRate[i];
			}
		}
	}



	//remaining
	for (int i = 0; i < 8 - 1; i++)
	{
#pragma HLS UNROLL
		int cur_zig_zag_num;
		if (last_x[i] == -1) // all zeros for current scan-line.
		{
			if (i < 4) {
				cur_zig_zag_num = 4;
			}
			else {
				cur_zig_zag_num = 7 - i;
			}
			final_zig_zag += cur_zig_zag_num;
		}
		else
		{
			if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
			{
				cur_zig_zag_num = cu_width - 1 - first_y[i];

			}
			else
			{
				if (i < 4) {
					cur_zig_zag_num = first_y[i];
				}
				else {
					cur_zig_zag_num = first_y[i] - i + 3;
				}
			}

			final_zig_zag += cur_zig_zag_num;

			// rate _cal:
			// get(final_zig_zag); 
			if (final_zig_zag == 0) // only one non-zero coeff.
			{
				final_rate += 1;
			}
			else
			{
				final_rate += enc_eco_run_est_2_only(final_zig_zag);
			}

			if ((last_x[i] + last_y[i]) % 2 == 1) // bottom -left scan
			{
				if (i < 4) {
					final_zig_zag = last_y[i]; // update
				}
				else {
					final_zig_zag = last_y[i] - i + 3;
				}
			}
			else
			{

				final_zig_zag = cu_width - last_y[i] - 1; // update
			}
			final_rate += sumRate[i];
		}
	}

	return (final_rate+ enc_est);
}

static int  enc_eco_coef_8_level_chroma(U8 cu_width, U8 cu_height, s16 coef[8][8])
{
	U15 enc_est = 0;
	static s16 coef_out[8];
	int max_num = cu_height;

	for (U7 z = 0; z < 8; z++) {
		coef_out[z] = 0;
	}
	for (int j = 0; j < cu_width; j++)
	{
		for (int i = 0; i < cu_height; i++)
		{
			coef_out[i] = coef[i][j];
		}
		enc_est += rate_coef_level_est_8(coef_out, max_num);
	}

	return enc_est;


}


 int enc_eco_xcoef_est_Chroma8(s16 coef[8][8], int cu_width_c, int cu_height_c,int num_nz)
{

#pragma HLS INLINE off
	int enc_est ;
	if (num_nz > 0)
	{
#if !SIMPLE_MD
		if (cu_width_c == 4 && cu_height_c == 8)
		{
			enc_est = enc_eco_coef_4_8_run_chroma(cu_width_c, cu_height_c, coef);
		}
		else
		{
#endif //!SIMPLE_MD
			enc_est = enc_eco_coef_8_run_chroma(cu_width_c, cu_height_c, coef);//8x8 8x4
#if !SIMPLE_MD
		}
#endif //!SIMPLE_MD
		//enc_est += enc_eco_coef_8_level_chroma(cu_width_c, cu_height_c, coef);
	}
	else
	{
		enc_est = 0;
	}

	return enc_est;
}


 int enc_eco_xcoef_est_16(s16 coef[16][16], int log2_w, int log2_h, int num_nz)
{

#pragma HLS INLINE off
	int enc_est = 0;
	U8 cu_width = 1 << log2_w;
	U8 cu_height = 1 << log2_h;
	if (num_nz > 0) {
	//if (cu_width == 16 && cu_height == 16) {
	//	enc_est += enc_eco_coef_16_16_run_chroma(cu_width, cu_height, coef);
	//		//enc_est += enc_eco_coef_16_16_level_chroma(cu_width, cu_height, coef);
	//}
	//else if (cu_width == 16 && cu_height == 8) {
	//	enc_est += enc_eco_coef_16_8_run_chroma(cu_width, cu_height, coef);
	//		//enc_est += enc_eco_coef_16_8_level_chroma(cu_width, cu_height, coef);
	//}
	//else {
	//	enc_est += enc_eco_coef_8_16_run_chroma(cu_width, cu_height, coef);
	//		//enc_est += enc_eco_coef_8_16_level_chroma(cu_width, cu_height, coef);
	//	}

	enc_est += enc_eco_coef_16_runlevel_chroma(cu_width, cu_height, coef);
	}


	return enc_est;
}



 int enc_eco_xcoef_est_32(s16 coef[32][32], int cu_width, int cu_height, int num_nz)
{

#pragma HLS INLINE off
	int enc_est = 0;
	if (num_nz >0) {
		enc_est += enc_eco_coef_32_runlevel_chroma(cu_width, cu_height, coef);
		//enc_est += enc_eco_coef_32_level_chroma(cu_width, cu_height, coef);
	}
	return enc_est;
	
}


int encode_intra_dir_c_est(u8 ipm, u8 ipm_l, u8 tscpm_enable_flag)
{
#pragma HLS INLINE off
	int enc_est = 0;
	//  ENC_SBAC *sbac;
	u8 chk_bypass;
	//	sbac = &core_inner->s_temp_run;
	COM_IPRED_CONV_L2C_CHK(ipm_l, chk_bypass);
	enc_est++;
	//enc_sbac_encode_bin(!ipm, sbac, sbac->ctx.intra_dir + 7, bs);

	if (ipm)
	{
#if TSCPM
		if (tscpm_enable_flag)
		{
			if (ipm == IPD_TSCPM_C)
			{
				//enc_sbac_encode_bin(1, sbac, sbac->ctx.intra_dir + 9, bs);
				enc_est++;
				return enc_est;
			}
			else
			{
				//enc_sbac_encode_bin(0, sbac, sbac->ctx.intra_dir + 9, bs);
				enc_est++;
			}
		}
#endif
		u8 symbol = (chk_bypass && ipm > ipm_l) ? ipm - 2 : ipm - 1;

		//sbac_write_truncate_unary_sym(symbol, 1, IPD_CHROMA_CNT - 1, sbac, sbac->ctx.intra_dir + 8, bs);
		enc_est += sbac_write_truncate_unary_sym_est(symbol, IPD_CHROMA_CNT - 1);

	}

	return enc_est;
}
int enc_bit_est_intra_chroma(MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef[4096]/*[N_C - 1][MAX_CU_DIM_C]*/, 
u8 tscpm_enable_flag)
{
#pragma HLS INLINE off
	//COM_MODE *mod_info_curr = &core->mod_info_curr;

	//ENC_SBAC *sbac = &core_inner->s_temp_run;
	int enc_est = 0;
	int cbf_u, cbf_v;
	int cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	int cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	int* num_nz_coef = mod_info_curr_ptr->num_nz[TB0];


	enc_est += encode_intra_dir_c_est(mod_info_curr_ptr->ipm[PB0][1], mod_info_curr_ptr->ipm[PB0][0], tscpm_enable_flag);

	cbf_u = (num_nz_coef[U_C] > 0) ? 1 : 0;
	cbf_v = (num_nz_coef[V_C] > 0) ? 1 : 0;

	//enc_sbac_encode_bin(cbf_u, sbac, sbac->ctx.cbf + 1, &core_inner->bs_temp);
	//enc_sbac_encode_bin(cbf_v, sbac, sbac->ctx.cbf + 2, &core_inner->bs_temp);
	enc_est += 2;


	cu_width_log2--;
	cu_height_log2--;

	if (cbf_u)
	{
		//enc_eco_xcoef(&core_inner->bs_temp, sbac, coef[U_C - 1], cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
		enc_est += enc_eco_xcoef_est(coef, cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
	}
	if (cbf_v)
	{
		//enc_eco_xcoef(&core_inner->bs_temp, sbac, coef[V_C - 1], cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
		enc_est += enc_eco_xcoef_est(coef, cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
	}
	return enc_est;

}




s64 get_bit_est_intra_chroma8( int cu_width, int cu_height,int est_bit_intra)
{
#pragma HLS INLINE off
	if (cu_height == 16 && cu_width == 16) {
		//bit_cnt = ((int)(0.707 * ((double)est_bit_intra) + 4.119));
		return (9378 * est_bit_intra - 5116) >> 14;
	}

	if (cu_height == 16 && cu_width == 8) {
		//bit_cnt = ((int)(0.7958 * ((double)est_bit_intra) + 2.264));
		return (9682 * est_bit_intra - 7467) >> 14;
	}

	if (cu_height == 8 && cu_width == 16) {
		//bit_cnt = ((int)(0.7837 * ((double)est_bit_intra) + 2.333));
		return (9667 * est_bit_intra - 7965) >> 14;
	}

	return 1;
}
s64 get_bit_est_intra_chroma(MD_COM_MODE_BEST* mod_info_curr_ptr, int cu_width, int cu_height,
	s16* coef_tmp/*[N_C - 1][1024]*/, u8 tscpm_enable_flag)
{
	int est_bit_intra;
	s64 cost = 0;
	int bit_cnt = 0;
	if (cu_height == 64 && cu_width == 64) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7641 * ((double)est_bit_intra) + 7.17));
		return (7819 * est_bit_intra + 120464) >> 14;
	}
	if (cu_height == 64 && cu_width == 32) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7764 * ((double)est_bit_intra) + 15.45));
		return (7995 * est_bit_intra + 60760) >> 14;
	}

	if (cu_height == 64 && cu_width == 16) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7223 * ((double)est_bit_intra) + 6.819));
		return (8283 * est_bit_intra + 24825) >> 14;
	}

	if (cu_height == 32 && cu_width == 64) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7916 * ((double)est_bit_intra) + 6.326));
		return (8145 * est_bit_intra + 56343) >> 14;
	}

	if (cu_height == 16 && cu_width == 64) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.8061 * ((double)est_bit_intra) + 3.832));
		return (8578 * est_bit_intra + 19128) >> 14;
	}

	if (cu_height == 64 && cu_width == 8) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7011 * ((double)est_bit_intra) + 4.841));
		return (9061 * est_bit_intra + 4071) >> 14;
	}

	if (cu_height == 8 && cu_width == 64) {
		est_bit_intra = enc_bit_est_intra_chroma(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.8292 * ((double)est_bit_intra) + 1.931));
		return (9303 * est_bit_intra + 2478) >> 14;
	}

	return bit_cnt;
}

s64 get_bit_est_intra_chroma_32( int cu_width_chroma, int cu_height_chroma,int est_bit_intra)
{

	int cu_width = cu_width_chroma << 1;
	int cu_height = cu_height_chroma << 1;
	if (cu_height == 64 && cu_width == 64) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7641 * ((double)est_bit_intra) + 7.17));
		return (7819 * est_bit_intra + 120464) >> 14;
	}
	if (cu_height == 64 && cu_width == 32) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7764 * ((double)est_bit_intra) + 15.45));
		return (7995 * est_bit_intra + 60760) >> 14;
	}

	if (cu_height == 64 && cu_width == 16) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7223 * ((double)est_bit_intra) + 6.819));
		return (8283 * est_bit_intra + 24825) >> 14;
	}

	if (cu_height == 32 && cu_width == 64) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7916 * ((double)est_bit_intra) + 6.326));
		return (8145 * est_bit_intra + 56343) >> 14;
	}

	if (cu_height == 16 && cu_width == 64) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.8061 * ((double)est_bit_intra) + 3.832));
		return (8578 * est_bit_intra + 19128) >> 14;
	}

	if (cu_height == 64 && cu_width == 8) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.7011 * ((double)est_bit_intra) + 4.841));
		return (9061 * est_bit_intra + 4071) >> 14;
	}

	if (cu_height == 8 && cu_width == 64) {
		//est_bit_intra = enc_bit_est_intra_chroma_32(mod_info_curr_ptr, coef_q_temp, coef_tmp, tscpm_enable_flag);
		//bit_cnt = ((int)(0.8292 * ((double)est_bit_intra) + 1.931));
		return (9303 * est_bit_intra + 2478) >> 14;
	}

	return 0;
}


//****************************analyze_intra_cu for all sizes*****************************



int copy_cu_data_array_src(ENC_CU_DATA_ARRAY *dst, ENC_CU_DATA_ARRAY *src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
{
	U8 i, ii, i1, i2;
	U8 j;
	U8 k;
	U8 l;
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
	cy =(y >> 2);
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);
	dst->x_chroma = src->x_chroma;
	dst->y_chroma = src->y_chroma;
	dst->last_child = src->last_child;
#if COPY_CU_DATA_ARRAY_SRC
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=7 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	

	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=7 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1	
	#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3
#endif

	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (ii = 0; ii < cuw_scu; ii++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline II=1
			for (k = 0; k < 5; k++) {
				if(k >= cud)
				{
					for (i = 0; i < (NUM_BLOCK_SHAPE); i++) {
						(dst->split_mode[k][i] + idx_dst)[ii] = (src->split_mode[k][i] + idx_src)[ii];
					}
				}
			}
			(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
			(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
			(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
			(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
			//(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];
			for (i = 0; i < 8; i++) {
				(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];
			}
			(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
			(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
			(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
			(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
			(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];
			for (i = 0; i < 2; i++){
				(dst->refi + idx_dst)[ii][i] = (src->refi + idx_src)[ii][i];
			}
			(dst->umve_flag + idx_dst)[ii] = (src->umve_flag + idx_src)[ii];
			(dst->umve_idx + idx_dst)[ii] = (src->umve_idx + idx_src)[ii];
			(dst->skip_idx + idx_dst)[ii] = (src->skip_idx + idx_src)[ii];
			(dst->mvr_idx + idx_dst)[ii] = (src->mvr_idx + idx_src)[ii];
			(dst->mvp_from_hmvp_flag + idx_dst)[ii] = (src->mvp_from_hmvp_flag + idx_src)[ii];
			(dst->ipf_flag + idx_dst)[ii] = (src->ipf_flag + idx_src)[ii];
			for (i1 = 0; i1 < 2; i1++){
				for ( i2 = 0; i2 < 2; i2++){
					(dst->mv + idx_dst)[ii][i1][i2] = (src->mv + idx_src)[ii][i1][i2];
					(dst->mvd + idx_dst)[ii][i1][i2] = (src->mvd + idx_src)[ii][i1][i2];
				}

			}
	 		for (k = 0; k < 1; k++) {
			 (dst->num_nz_coef[k] + idx_dst)[ii] = (src->num_nz_coef[k] + idx_src)[ii];
		 	}			
			(dst->pb_part + idx_dst)[ii] = (src->pb_part + idx_src)[ii];
			(dst->tb_part + idx_dst)[ii] = (src->tb_part + idx_src)[ii];
			for(l = 0; l < 4; l++)
			{
				for(k = 0; k < 4; k++)
				{
					dst->coef_y[y + j*4+l][x + ii*4+k] = src->coef_y[j*4+l][ii*4+k];
					dst->reco_y[y + j*4+l][x + ii*4+k] = src->reco_y[j*4+l][ii*4+k];
				}
			}
		}
	}

	return 0;
}


u8 com_tree_split(int w, int h, s8 split, u8 slice_type)
{
    if (split==SPLIT_QUAD) {
        if (w == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if ((split) == (SPLIT_EQT_HOR)) {
        if (h == 16 || w == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if ((split) == (SPLIT_EQT_VER)) {
        if (w == 16 || h == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if ((split) == (SPLIT_BI_HOR)) {
        if (h == 8) {
            return 1;
        } else {
            return 0;
        }
    } else if ((split) == (SPLIT_BI_VER)) {
        if (w == 8) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

s64 md2_prepare_cu_info_COM_combine_top(MD_FW*md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY *cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width, U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT *split_struct)
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
	U3 rate_bit = 0;

	if (x0 + cu_width <= md_fw_ptr->pic_width  && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);
		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		cost_temp += (rate_bit * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}

s64 prepare_cu_info_COM_combine_only_qt(MD_FW*md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY *cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT *split_struct)
{
	com_split_get_part_structure_only_qt(x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode_only_qt(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], bet_depth, &split_struct->num_split[i]);
	}

	s64 cost_temp = 0;
	U3 rate_bit = 0;

	if (x0 + cu_width <= md_fw_ptr->pic_width  && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);
		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		cost_temp += (rate_bit * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}
s64 prepare_cu_info_COM_combine_only_bt(MD_FW*md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY *cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT *split_struct)
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

	if (x0 + cu_width <= md_fw_ptr->pic_width  && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);

		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) {
			rate_bit += 1;
		}
		bit_cnt = rate_bit;

		cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}
s64 prepare_cu_info_COM_combine(MD_FW*md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY *cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT *split_struct)
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

	if (x0 + cu_width <= md_fw_ptr->pic_width  && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);

		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) {
			rate_bit += 1;
		}
		bit_cnt = rate_bit;

		cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}


void com_set_cons_pred_mode(U2 cons_pred_mode, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s,
	U8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))])
{
	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[lcu_s] - 2)) + (cu_width >> 1 >> 2));
	U3 shape = (SQUARE)+(com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	split_mode_buf[cud][shape][pos] = ((((split_mode_buf[cud][shape][pos]) & 0x67) + ((U8)(cons_pred_mode) << 3)));
}


int copy_cu_data_array_dst(ENC_CU_DATA_ARRAY *dst, ENC_CU_DATA_ARRAY *src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
{
	U8 i, ii, i1, i2;
	U8 j;
	U8 k;
	U8 l;
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
	cy =( y >> 2);
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);
#if COPY_CU_DATA_ARRAY_DST
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=7 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	

	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=7 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3
#endif
	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (ii = 0; ii < cuw_scu; ii++){
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline II=1
			for (k = 0; k < 5; k++) {
				if(k >= cud)
				{
					for (i = 0; i < (NUM_BLOCK_SHAPE); i++) {
						(dst->split_mode[k][i] + idx_dst)[ii] = (src->split_mode[k][i] + idx_src)[ii];
					}
				}
			}
			(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
			(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
			(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
			(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
			//(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];
			for (i = 0; i < 8; i++) {
				(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];
			}
			(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
			(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
			(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
			(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
			(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];

			for (i1 = 0; i1 < 2; i1++)
				(dst->refi + idx_dst)[ii][i1] = (src->refi + idx_src)[ii][i1];

			(dst->umve_flag + idx_dst)[ii] = (src->umve_flag + idx_src)[ii];
			(dst->umve_idx + idx_dst)[ii] = (src->umve_idx + idx_src)[ii];
			(dst->skip_idx + idx_dst)[ii] = (src->skip_idx + idx_src)[ii];
			(dst->mvr_idx + idx_dst)[ii] = (src->mvr_idx + idx_src)[ii];
			(dst->mvp_from_hmvp_flag + idx_dst)[ii] = (src->mvp_from_hmvp_flag + idx_src)[ii];
			(dst->ipf_flag + idx_dst)[ii] = (src->ipf_flag + idx_src)[ii];
			for (i1 = 0; i1 < 2; i1++){
				for (i2 = 0; i2 < 2; i2++){
					(dst->mv + idx_dst)[ii][i1][i2] = (src->mv + idx_src)[ii][i1][i2];
					(dst->mvd + idx_dst)[ii][i1][i2] = (src->mvd + idx_src)[ii][i1][i2];
				}
			}
			for (k = 0; k < 1; k++) {
				(dst->num_nz_coef[k] + idx_dst)[ii] = (src->num_nz_coef[k] + idx_src)[ii];
			}
			(dst->pb_part + idx_dst)[ii] = (src->pb_part + idx_src)[ii];
			(dst->tb_part + idx_dst)[ii] = (src->tb_part + idx_src)[ii];
			for(l = 0; l < 4; l++)
			{
				for(k = 0; k < 4; k++)
				{
					dst->coef_y[y + j*4+l][x + ii*4+k] = src->coef_y[j*4+l][ii*4+k];
					dst->reco_y[y + j*4+l][x + ii*4+k] = src->reco_y[j*4+l][ii*4+k];
				}
			}
		}
	}

	return 0;
}
int copy_cu_data_array_dst(ENC_CU_DATA* dst, ENC_CU_DATA_ARRAY* src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
{
	U8 i, ii, i1, i2;
	U8 j;
	U8 k;
	U8 l;
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
#if COPY_CU_DATA_ARRAY_DST
#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	

#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2

#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3
#endif
	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline II=1
			for (k = 0; k < 5; k++) {
				if (k >= cud)
				{
					for (i = 0; i < (NUM_BLOCK_SHAPE); i++) {
						(dst->split_mode[k][i] + idx_dst)[ii] = (src->split_mode[k][i] + idx_src)[ii];
					}
				}
			}
			(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
			(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
			(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
			(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
			//(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];
			for (i = 0; i < 8; i++) {
				(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];
			}
			(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
			(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
			(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
			(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
			(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];

			for (i1 = 0; i1 < 2; i1++)
				(dst->refi + idx_dst)[ii][i1] = (src->refi + idx_src)[ii][i1];

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
			for (k = 0; k < 1; k++) {
				(dst->num_nz_coef[k] + idx_dst)[ii] = (src->num_nz_coef[k] + idx_src)[ii];
			}
			(dst->pb_part + idx_dst)[ii] = (src->pb_part + idx_src)[ii];
			(dst->tb_part + idx_dst)[ii] = (src->tb_part + idx_src)[ii];
			for (l = 0; l < 4; l++)
			{
				for (k = 0; k < 4; k++)
				{
					dst->coef_y[y + j * 4 + l][x + ii * 4 + k] = src->coef_y[j * 4 + l][ii * 4 + k];
					dst->reco_y[y + j * 4 + l][x + ii * 4 + k] = src->reco_y[j * 4 + l][ii * 4 + k];
				}
			}
		}
	}

	return 0;
}


void update_map_scu(MD_FW *md_fw_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best_ptr, U13 x, U13 y, U8 src_cuw, U8 src_cuh,
	U2 tree_status)
{
	//s8 *map_ipm = 0;
	S8 *src_map_ipm = 0;
	//int size_depth;
	U6 w;
	U6 h;
	u16 i;
	/*int size;
	int size_ipm;
	int size_mv;
	int size_refi;*/
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

    if (x + src_cuw > md_fw_ptr->pic_width ) {
        w = (md_fw_ptr->pic_width  - x )>> 2;
    } else {
        w = src_cuw >> 2;
    }
    if (y + src_cuh > md_fw_ptr->pic_height ) {
        h = (md_fw_ptr->pic_height  - y) >> 2;
    } else {
        h = src_cuh >> 2;
    }
	/*size = (sizeof(u32) * w);
	size_ipm = (sizeof(u8) * w);
	size_mv = (sizeof(s16) * w * 2 * 2);
	size_refi = (sizeof(s8) * w* 2);
	size_depth = (sizeof(s8)* w);*/
	mpm_x = scu_x & 15;
	mpm_y = scu_y & 15;
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (ii = 0; ii < w; ii++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MPM_buffer_ptr->buffer[i + mpm_y][ii + mpm_x] = (src_map_ipm[(i << (log2_src_cuw - 2)) + ii]);
//		}
//	}
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		src_map_ipm += src_cuw >> 2;
	}
	//x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	//y_scu_line = scu_y - me_line_map_ptr->min_scu_y;
	j = 0;
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (j = 0; j < w; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
//				= curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
//			me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i] = curr_cu_data_best_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].refi[0] = curr_cu_data_best_ptr->refi[j + (i << (log2_src_cuw - 2))][0];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].refi[1] = curr_cu_data_best_ptr->refi[j + (i << (log2_src_cuw - 2))][1];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[0][0] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][0][0];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[0][1] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][0][1];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[1][0] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][1][0];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[1][1] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][1][1];
//			/*inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].pb_tb_part = curr_cu_data_best_ptr->map_pb_tb_part[j + (i << (log2_src_cuw - 2))];*/
//		}
//	}
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (j = 0; j < w; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i] =
//				curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
//		}
//	}
}

static s64 enc_ssd_16b_chroma_4x4(U3 w_log2, U3 h_log2, s16 src1[(4 * 4)], s16 src2[4 * 4], U4 bit_depth)
{
#pragma HLS INLINE 
	U7 w = 1 << w_log2;
	U7 h = 1 << h_log2;
	const U2 shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	U7 i, j;
	int diff;
#pragma HLS ARRAY_PARTITION variable=src1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src2 complete dim=1

	for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
			diff = src1[i * 4 + j] - src2[i * 4 + j];
			ssd += ((diff * diff >> shift));
		}
	}
	return ssd;

}

void update_chroma_4(MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef_y_pb_part[12][16], U1 i, pel pred[4][4], pel rec[4][4], pel rec_1[4][4],  int num_nz[4][3],
	U12 size_tmp1, S8 rdo_idx, s64 cost, s64* cost_best1, int* num_nz1, int* num_nz2, int* best_ipd1, U4 bit_depth, int num_nz_out[12],
	pel mod_info_best_rec_u[4][4],
	pel mod_info_best_rec_v[4][4],
	pel mod_info_best_coef_u[4][4],
	pel mod_info_best_coef_v[4][4])

{

#pragma HLS INLINE
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	U7 cu_width = mod_info_curr_ptr->cu_width ;
	U7 cu_height = mod_info_curr_ptr->cu_height ;
	U7 cu_width_c = cu_width >> 1;
	U7 cu_height_c = cu_height >> 1;
	U12 size_tmp = cu_width * cu_height >> 2;
	U2 inter_mode = mod_info_curr_ptr->cu_mode;
	int j;
	u32 run_rdoq[24];
	u32 level[24];
	u32 last1[12];
	u32 last2[22];
	pel pred_tmp;
#pragma HLS ARRAY_PARTITION variable=rec complete dim=0
	
#pragma HLS ARRAY_PARTITION variable=coef_y_pb_part complete dim=2	
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1	
#pragma HLS ARRAY_PARTITION variable=pred complete dim=0

#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_u complete  dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_v complete  dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_v complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr_ptr->num_nz complete dim=0
	if (rdo_idx == 0) {
		*cost_best1 = 9223372036854775807;
		*num_nz1 = 0;
		*num_nz2 = 0;
		*best_ipd1 = 0;
	}
	if (inter_mode != 0) 
	{

		mod_info_curr_ptr->num_nz[0][1] = num_nz_out[rdo_idx - 1];
		mod_info_curr_ptr->num_nz[0][2] = num_nz_out[rdo_idx];
		//if (inter_mode == 2) {

		//	num_nz_out[i] = 0;
		//}

		if (num_nz_out[i] == 0)
			{
			for (int k = 0; k < 4; k++)
			{
#pragma HLS UNROLL
				for (j = 0; j < 4; j++) {
#pragma HLS UNROLL				
					if (i == 0) {
						mod_info_best_coef_u[k][j] = 0;
						mod_info_best_rec_u[k][j] = (((0 > (((1 << bit_depth) - 1 < (pred[k][j]) ?
							(1 << bit_depth) - 1 : (pred[k][j]))) ? 0 : (((1 << bit_depth) - 1 < (pred[k][j]) ? (1 << bit_depth) - 1 : (pred[k][j]))))));
					}
					else if (i == 1) {
						mod_info_best_coef_v[k][j] = 0;
						mod_info_best_rec_v[k][j] = (((0 > (((1 << bit_depth) - 1 < (pred[k][j]) ?
							(1 << bit_depth) - 1 : (pred[k][j]))) ? 0 : (((1 << bit_depth) - 1 < (pred[k][j]) ? (1 << bit_depth) - 1 : (pred[k][j]))))));
					}
				}
			}



		}

		
		else {
			for (int k = 0; k < 4; k++)
			{
#pragma HLS UNROLL
				for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
					if (i == 0) {
						mod_info_best_coef_u[k][j] = coef_y_pb_part[rdo_idx][k * cu_width_c + j];
						mod_info_best_rec_u[k][j] = ((pel)((0 > (((1 << bit_depth) - 1 < (rec[k][j]) ?
							(1 << bit_depth) - 1 : (rec[k][j]))) ? 0 : (((1 << bit_depth) - 1 < (rec[k][j]) ? (1 << bit_depth) - 1 : (rec[k][j]))))));

					}
					else if (i == 1) {
						mod_info_best_coef_v[k][j] = coef_y_pb_part[rdo_idx][k * cu_width_c + j];
						mod_info_best_rec_v[k][j] = ((pel)((0 > (((1 << bit_depth) - 1 < (rec[k][j]) ?
							(1 << bit_depth) - 1 : (rec[k][j]))) ? 0 : (((1 << bit_depth) - 1 < (rec[k][j]) ? (1 << bit_depth) - 1 : (rec[k][j]))))));
					}


				}
				//coef_rec_write_flag[0] = 1;
			}
		}
	}
	else 
	{
		if (i == 1 && cost < *cost_best1)
		{
			*cost_best1 = cost;
			*best_ipd1 = rdo_idx / 2;		

			for (int k = 0; k < 4; k++)
			{
#pragma HLS UNROLL
				for (int j = 0; j < 4; j++) {
#pragma HLS UNROLL
					mod_info_best_rec_u[k][j] = rec_1[k][j];
					mod_info_best_rec_v[k][j] = rec[k][j];
					mod_info_best_coef_u[k][j] = coef_y_pb_part[rdo_idx - 1][k * 4 + j];
					mod_info_best_coef_v[k][j] = coef_y_pb_part[rdo_idx][k * 4 + j];
				}
			}


			//*num_nz1 = num_nz[0][1];
			//*num_nz2 = num_nz[0][2];
			*num_nz1 = num_nz_out[rdo_idx - 1];
			*num_nz2 = num_nz_out[rdo_idx];

		}
#if USE_TSCPM
		if (rdo_idx == 11) {
			mod_info_curr_ptr->ipm[0][1] = *best_ipd1;
			mod_info_curr_ptr->num_nz[0][1] = *num_nz1;
			mod_info_curr_ptr->num_nz[0][2] = *num_nz2;
		}
#else
		if (rdo_idx == 9) {
			mod_info_curr_ptr->ipm[0][1] = *best_ipd1;
			mod_info_curr_ptr->num_nz[0][1] = *num_nz1;
			mod_info_curr_ptr->num_nz[0][2] = *num_nz2;
		}
#endif

	}

}

void calcDiffChroma4(U2 cu_mode, U3 cu_width, U3 cu_height, s16 orgU[16], s16 pred[4 * 4], s16 coef_tmp[4 * 4])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=pred complete dim=1
//	if (cu_mode != 0) {
		diff_16b(cu_width / 2, cu_height / 2, orgU, pred, coef_tmp);
//	}
}

static void model_update_4_4_chroma(U1 mat_run[16], U24 mat_prev_level[16], U14 max_num, int* sum_run, U32 last1_tmp[6], U32 last2_tmp[6])
{
	int ace_log2 = 0;
	U7 scan_pos = 0;
	int sum_run_temp = 0;
	U32 last1_tmp_temp[6];
	U32 last2_tmp_temp[6];
	for (int i = 0; i < 6; i++)
	{
		last1_tmp_temp[i] = 0;
		last2_tmp_temp[i] = 0;
	}
	for (scan_pos = 0; scan_pos < 16; scan_pos++)
	{
		if (scan_pos + 1 < 2) {
			ace_log2 = 0;
		}
		else if (scan_pos + 1 < 4) {
			ace_log2 = 1;
		}
		else if (scan_pos + 1 < 8) {
			ace_log2 = 2;
		}
		else if (scan_pos + 1 < 16) {
			ace_log2 = 3;
		}
		else if (scan_pos + 1 < 32) {
			ace_log2 = 4;
		}

		else {
			ace_log2 = 4;
		}
		U4 last1_pos = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5)))) + 6;
		U5 last2_pos = ace_log2 + 12;
		sum_run_temp += mat_run[scan_pos];
		switch (last1_pos)
		{
		case  6: last1_tmp_temp[0]++; break;
		case  7: last1_tmp_temp[1]++; break;
		case  8: last1_tmp_temp[2]++; break;
		case  9: last1_tmp_temp[3]++; break;
		case  10: last1_tmp_temp[4]++; break;
		case  11: last1_tmp_temp[5]++; break;
		default: break;
		}
		switch (last2_pos)
		{
		case  12: last2_tmp_temp[0]++; break;
		case  13: last2_tmp_temp[1]++; break;
		case  14: last2_tmp_temp[2]++; break;
		case  15: last2_tmp_temp[3]++; break;
		case  16: last2_tmp_temp[4]++; break;
		case  17: last2_tmp_temp[5]++; break;
		default: break;
		}
	}
	*sum_run = sum_run_temp;
	for (int i = 0; i < 6; i++)
	{
		last1_tmp[i] = last1_tmp_temp[i];
		last2_tmp[i] = last2_tmp_temp[i];
	}
}


int rdoq_top_4_4_chroma(int qp, s64 d_lambda, int is_intra, s16 src_coef[16], s16 dst_tmp[16], int cu_width_log2,
	int cu_height_log2, int ch_type, int bit_depth, RDOQ_ARRAY* rdoq_array)
{
//#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst_tmp complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = rdoq_est_run_local complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = rdoq_est_level_local complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = rdoq_est_last_local complete dim = 0
	U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

	const U14 scale = quant_scale[qp];
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);

	const int max_num_coef = 16;

	u16 scan[16] = { 0,1,4,8,5,2,3,6,9,12,13,10,7,11,14,15 };
	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	const int q_bits = QUANT_SHIFT + tr_shift;
	int num_nz_coef = 0;
	int scan_pos;
	s32 ctx_qt_cbf;
	int best_last_idx_p1 = 0;
	s16 tmp_coef[16] = { 0 };
	U36 tmp_level_double[16] = { 0 };
	s16 tmp_dst_coef[16] = { 0 };
	const s64 lambda = (d_lambda * (1 << 15) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	//const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
	U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	s64 d64_best_cost = 0;
	s64 d64_base_cost[16] = { 0 };
	s64 d64_coded_cost = 0;
	s64 d64_uncoded_cost = 0;

#pragma HLS ARRAY_PARTITION variable=scan complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_level_double complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=d64_base_cost complete dim=0

	/* ===== quantization ===== */
	for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
	{
#pragma HLS UNROLL
		u32 blk_pos = scan[scan_pos];
		U36 level_double = src_coef[blk_pos];
		U24 max_abs_level;
		U1 lower_int;
		U36 temp_level;
		temp_level = ((s64)COM_ABS(src_coef[blk_pos]) * (s64)q_value);
		level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - (s64)(1 << (q_bits - 1)));
		tmp_level_double[blk_pos] = level_double;
		max_abs_level = (u32)(level_double >> q_bits);
		lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < (s64)(1 << (q_bits - 1))) ? 1 : 0;
		if (!lower_int)
		{
			max_abs_level++;
		}
		tmp_coef[blk_pos] = src_coef[blk_pos] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
	}

	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
		{
#pragma HLS UNROLL
			d64_base_cost[scan_pos] = 0;
		}
	}
	else
	{
		ctx_qt_cbf = ch_type;
		d64_best_cost = GET_I_COST(rdoq_array->rdoq_est_cbf[ctx_qt_cbf][0], lambda);
		//d64_best_cost = d64_best_cost >> 24;
		for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
		{
#pragma HLS UNROLL
			d64_base_cost[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_cbf[ctx_qt_cbf][1], lambda);
			//d64_base_cost[scan_pos] = d64_base_cost[scan_pos] >> 24;
		}
	}

	U24 mat_prev_level[16] = { 0 };
	U1 mat_run[16] = { 0 };
	mat_prev_level[0] = 6;
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=0

	for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++)
	{
#pragma HLS UNROLL
		u32 blk_pos = scan[scan_pos - 1];

		if (COM_ABS(tmp_coef[blk_pos]))
		{
			mat_prev_level[scan_pos] = COM_ABS(tmp_coef[blk_pos]);
			mat_run[scan_pos] = 0;
		}
		else
		{
			mat_prev_level[scan_pos] = mat_prev_level[scan_pos - 1];
			mat_run[scan_pos] = 1;
		}
	}

	s64 mat_uncoded_cost[16] = { 0 };
	s64 mat_coded_cost[16] = { 0 };
	U24 lev_opt[16] = { 0 };
#pragma HLS ARRAY_PARTITION variable=mat_uncoded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_coded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=lev_opt complete dim=0

	for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
	{
#pragma HLS UNROLL
		u32 blk_pos = scan[scan_pos];
		U24 level;
		U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) << 1) + (ch_type == Y_C ? 0 : 12);

		int iflast = (scan_pos == max_num_coef - 1);
		level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[blk_pos], COM_ABS(tmp_coef[blk_pos]),
			mat_run[scan_pos], (u16)ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, iflast, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level);
		tmp_dst_coef[blk_pos] = (s16)(tmp_coef[blk_pos] < 0 ? -(s16)(level) : (s16)level);

		mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
		mat_coded_cost[scan_pos] = d64_coded_cost;
		lev_opt[scan_pos] = level;
	}

	s64 d64_cost_last_zero[16] = { 0 };
	d64_cost_last_zero[0] = 0;
	int ace_log2 = 0;
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=0

	for (scan_pos = 1; scan_pos < max_num_coef; scan_pos++)
	{
#pragma HLS UNROLL
		u32 blk_pos = scan[scan_pos];
		if ((scan_pos) < 2)
		{
			ace_log2 = 0;
		}
		else if ((scan_pos) < 4)
		{
			ace_log2 = 1;
		}
		else if ((scan_pos) < 8)
		{
			ace_log2 = 2;
		}
		else if ((scan_pos) < 16)
		{
			ace_log2 = 3;
		}
		else if ((scan_pos) < 32)
		{
			ace_log2 = 4;
		}
		else
		{
			ace_log2 = 4;
		}
		d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos - 1] - 1), (U24)(5))][ace_log2][0], lambda);
	}
	for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
	{
#pragma HLS UNROLL
		for (int i = 0; i <= scan_pos; i++)
		{
#pragma HLS UNROLL
			d64_base_cost[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * !!(lev_opt[i]);
		}
	}
	for (scan_pos = 0; scan_pos < max_num_coef; scan_pos++)
	{
#pragma HLS UNROLL
		u32 blk_pos = scan[scan_pos];
		if ((scan_pos + 1) < 2)
		{
			ace_log2 = 0;
		}
		else if ((scan_pos + 1) < 4)
		{
			ace_log2 = 1;
		}
		else if ((scan_pos + 1) < 8)
		{
			ace_log2 = 2;
		}
		else if ((scan_pos + 1) < 16)
		{
			ace_log2 = 3;
		}
		else if ((scan_pos + 1) < 32)
		{
			ace_log2 = 4;
		}
		else {
			ace_log2 = 4;
		}
		U24 prev_level = mat_prev_level[scan_pos];
		s64 d64_cost_last_one = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(prev_level - 1), (U24)(5))][ace_log2][1], lambda);
		//d64_cost_last_one = d64_cost_last_one >> 24;
		if (COM_ABS(tmp_dst_coef[blk_pos]))
		{
			s64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

			if (d64_cur_is_last_cost < d64_best_cost)
			{
				d64_best_cost = d64_cur_is_last_cost;
				best_last_idx_p1 = scan_pos + 1;
			}
		}
	}
	//model_update_4_4_chroma(mat_run, mat_prev_level, max_num_coef, sum_run, last1_tmp, last2_tmp);

	int mask[16] = { 0 };
#pragma HLS ARRAY_PARTITION variable=mask complete dim=0

	for (scan_pos = 0; scan_pos < 16; scan_pos++)
	{
#pragma HLS UNROLL
		u32 blk_pos = scan[scan_pos];
		if (scan_pos < max_num_coef)
		{
			if (scan_pos < best_last_idx_p1)
				mask[blk_pos] = 1;
			else
				mask[blk_pos] = 0;
		}
		else
		{
			mask[blk_pos] = 0;
		}
		dst_tmp[blk_pos] = tmp_dst_coef[blk_pos] * mask[blk_pos];
		num_nz_coef += !!(dst_tmp[blk_pos]);
	}
	return num_nz_coef;
}
static int quant_4(U7 w, U7 h, U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth, s16 coef[4][4], U2 slice_type, U14 scale)
{
	int offset;
	int i, j;
	int shift;
	int sign;
	s16 lev;
	int tr_shift;
	int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	int num_nz_coef = 0;
	int scale_x_ns_scale = scale * ns_scale;

	tr_shift = get_transform_shift(bit_depth, log2_size - ns_shift);
	shift = QUANT_SHIFT + tr_shift;
	offset = (s64)((slice_type == SLICE_I) ? 171 : 85) << (s64)(shift - 9);

	for (i = 0; i < h; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < w; j++)
		{
#pragma HLS LOOP_TRIPCOUNT max=64
			sign = COM_SIGN_GET(coef[i][j]);
			lev = (((u32)(COM_ABS(coef[i][j]) * scale_x_ns_scale + offset)) >> shift);
			coef[i][j] = COM_SIGN_SET(lev, sign);

			//bug fix for rate estimation
			//if ((i + j) >= w)
			//{
			//	coef[i][j] = 0;
			//}

			num_nz_coef += !!(coef[i][j]);
		}
	}
	return num_nz_coef;
}

int quant_nnz_4_4_chroma(RDOQ_ARRAY* rdoq_array, int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16],
	int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[16],  s16 coefVer_q[16])
{
	int i;
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_y_pb_part dim=1
#pragma HLS ARRAY_PARTITION variable=coefVer_q dim=1


	int num_nz_coef = 0;
	s16 coef_out[16] = { 0 };
#pragma HLS ARRAY_PARTITION variable=coef_out dim=1

	U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

	U14 scale = quant_scale[qp];
	/*for (i = 0; i < 16; i++) {
		coef_y_pb_part[i] = 0;
	}*/

#if RDOQ_OPEN
	num_nz_coef = rdoq_top_4_4_chroma(qp, lambda, is_intra, coef, coef_out, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_array);
#else 
	s16 coef_tmp[4][4] = { 0 };
#pragma HLS ARRAY_PARTITION variable=coef_tmp dim=0
	for (U3 i = 0; i < 4; i++) {
#pragma HLS UNROLL
		for (U3 j = 0; j < 4; j++) {
#pragma HLS UNROLL
			coef_tmp[i][j] = coef[i * 4 + j];
		}
	}
	num_nz_coef = quant_4(4, 4, cu_width_log2, cu_height_log2, bit_depth, coef_tmp, slice_type, scale);
	/*for (U3 i = 0; i < 4; i++) {
#pragma HLS UNROLL
		for (U3 j = 0; j < 4; j++) {
#pragma HLS UNROLL
			coef[i * 4 + j] = coef_tmp[i][j];
		}
	}*/
	for (U3 i = 0; i < 4; i++) {
#pragma HLS UNROLL
		for (U3 j = 0; j < 4; j++) {
#pragma HLS UNROLL
			coef_out[i * 4 + j] = coef_tmp[i][j];
		}
	}
#endif 
	for (U8 z = 0; z < 16; z++) {
#pragma HLS UNROLL
		coef_y_pb_part[z] = coef_out[z];

	}
	for (U8 z = 0; z < 16; z++) {
#pragma HLS UNROLL
		coefVer_q[z] = coef_out[z];

	}

	return num_nz_coef;
}


void quantChroma4(int* num_nz, RDOQ_ARRAY* rdoq_array, int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16], 
	int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[16], s16 coefVer_q[16] )
{
#pragma HLS INLINE
	//*num_nz = quant_nnz_4x4(model_cnt, rdoq_array, bit_depth, qp, lambda, 1, coef, cu_width_log2, cu_height_log2, ch_type, slice_type, coef_y_pb_part, coef_u[0], coef_u[1]);
	*num_nz = quant_nnz_4_4_chroma(rdoq_array, bit_depth, qp, lambda, 1, coef, cu_width_log2, cu_height_log2, 
		ch_type, slice_type, coef_y_pb_part, coefVer_q);

}

void dquant_4(U6 qp, s16 coef[16], s16 coef_out[4 * 4], U3 log2_w, U3 log2_h, U4 bit_depth, int* num_nz_ith, int num_nz_q, int* num_nz_ith1, int* num_nz_ith2)
{

#pragma HLS INLINE
	int i;
	int j;
	int w = 1 << log2_w;
	int h = 1 << log2_h;
	U4 log2_size = ((U4)log2_w + log2_h) >> 1;
	int refix = (log2_w + log2_h) & 1;
	const int com_tbl_dq_scale[80] = { // [64 + 16]
	32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
	32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933,
	65535, 35734, 38968, 42577, 46341, 50617, 55027, 60097,
	32809, 35734, 38968, 42454, 46382, 50576, 55109, 60056,
	65535, 35734, 38968, 42495, 46320, 50515, 55109, 60076,
	65535, 35744, 38968, 42495, 46341, 50535, 55099, 60087,
	65535, 35734, 38973, 42500, 46341, 50535, 55109, 60097,
	32771, 35734, 38965, 42497, 46341, 50535, 55109, 60099,
	32768, 36061, 38968, 42495, 46341, 50535, 55437, 60424,
	32932, 35734, 38968, 42495, 46177, 50535, 55109, 59933
	};
	const int com_tbl_dq_shift[80] = { // [64 + 16]
	14, 14, 14, 14, 14, 14, 14, 14,  //15, 15, 15, 15, 15, 15, 15, 15,
	13, 13, 13, 13, 13, 13, 13, 13,  //14, 14, 14, 14, 14, 14, 14, 14,
	13, 12, 12, 12, 12, 12, 12, 12,  //14, 13, 13, 13, 13, 13, 13, 13,
	11, 11, 11, 11, 11, 11, 11, 11,  //12, 12, 12, 12, 12, 12, 12, 12,
	11, 10, 10, 10, 10, 10, 10, 10,  //12, 11, 11, 11, 11, 11, 11, 11,
	10,  9,  9,  9,  9,  9,  9,  9,  //11, 10, 10, 10, 10, 10, 10, 10,
	9,  8,  8,  8,  8,  8,  8,  8,  //10,  9,  9,  9,  9,  9,  9,  9,
	7,  7,  7,  7,  7,  7,  7,  7,  // 8,  8,  8,  8,  8,  8,  8,  8,
	6,  6,  6,  6,  6,  6,  6,  6,  // 7,  7,  7,  7,  7,  7,  7,  7,
	5,  5,  5,  5,  5,  5,  5,  5,  // 6,  6,  6,  6,  6,  6,  6,  6
	};
	int scale = com_tbl_dq_scale[qp];
	// +1 is used to compensate for the mismatching of shifts in quantization and inverse quantization
	int shift = com_tbl_dq_shift[qp] - get_transform_shift(bit_depth, log2_size) + 1;
	int offset = shift == 0 ? 0 : 1 << (shift - 1);
	if (log2_w > 6 || log2_h > 6) {
		0 ? ((void)0) : __assert_HLS("0", "../separate/enc_pintra.c", (unsigned int)2617, __FUNCTION__);
	}
	int sr_x = 0;
	int sr_y = 0;
	for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
			if (coef[j]) {
				sr_y = i > sr_y ? i : sr_y;
				sr_x = j > sr_x ? j : sr_x;
			}
			int weight = (i | j) & 0xE0 ? 0 : 64;
			int lev = (((coef[j] * weight >> 2) * scale >> 4) + offset) >> shift;
			lev = (lev > 32767 ? 32767 : ((lev < -32768 ? -32768 : lev)));
			if (refix) {
				lev = (lev * 181 + 128) >> 8;
			}
			coef_out[j] = (lev);
		}
		coef_out += 4;
		coef += 4;
	}
	*num_nz_ith = (sr_y << 20) + (sr_x << 12) + num_nz_q;
	*num_nz_ith1 = (sr_y << 20) + (sr_x << 12) + num_nz_q;
	*num_nz_ith2 = (sr_y << 20) + (sr_x << 12) + num_nz_q;
}

void dquantChroma4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[16], s16 coef_dq[4 * 4], U3 type, int num_nz_q, int num_nz1[4][3], int num_nz2[4][3] )
{
#pragma HLS INLINE
	U3 tb_width_log2 = com_tbl_log2[4];
	U3 tb_height_log2 = com_tbl_log2[4];
	if (num_nz_q) {
		dquant_4(qp_y, coef_tmp, coef_dq, tb_width_log2, tb_height_log2, bit_depth, &num_nz[0][type], num_nz_q, &num_nz1[0][type], &num_nz2[0][type]);
	}
	else {
		num_nz[0][type] = 0;
		num_nz1[0][type] = 0;
		num_nz2[0][type] = 0;
	}

}

void inv_dct_col_4(s16 coeff[4][4], s16 block[4][4], U4 shift)
{
#pragma HLS ARRAY_PARTITION variable=coeff complete dim=0
#pragma HLS ARRAY_PARTITION variable=block complete dim=0

	int E[2], O[2];
	int rnd_factor = 1 << (shift - 1);

	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		O[0] = (42) * coeff[1][i] + (17) * coeff[3][i];
		O[1] = (17) * coeff[1][i] + (-42) * coeff[3][i];
		E[0] = (32) * coeff[0][i] + (32) * coeff[2][i];
		E[1] = (32) * coeff[0][i] + (-32) * coeff[2][i];
		block[i][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
		block[i][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
		block[i][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
		block[i][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
	}
}

void itrans_v_4(s16 coef_dq[4 * 4], s16 coef_temp[4 * 4], U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth)
{

#pragma HLS INLINE
	U4 shift1 = com_get_inverse_trans_shift(cu_width_log2, 0, bit_depth);
	static s16 coeff1[4][4];
	static s16 block1[4][4];

	U7 size = 4;
	U8 size_b = size - 1;
#pragma HLS ARRAY_PARTITION variable=coef_dq complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_temp complete dim=1
#pragma HLS ARRAY_PARTITION variable=coeff1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=block1 complete dim=0
	for (int i = 0; i < 4; i++)
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++) {
#pragma HLS UNROLL
			coeff1[i][j] = coef_dq[i * 4 + j];
		}

	inv_dct_col_4(coeff1, block1, shift1);

	for (int i = 0; i < 4; i++)
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++) {
#pragma HLS UNROLL
			coef_temp[i * 4 + j] = block1[i][j];
		}
}

void invTransVerChroma4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_dq[4 * 4], s16 coef_tmp[4 * 4], U3 type)
{
#pragma HLS INLINE
	U3 tb_width_log2 = com_tbl_log2[4];
	U3 tb_height_log2 = com_tbl_log2[4];
	if (num_nz[0][type]) {
		itrans_v_4(coef_dq, coef_tmp, tb_width_log2, tb_height_log2, bit_depth);
	}
}

void inv_dct_row_4(s16 coeff[4][4], s16 block[4][4], U4 shift)
{
#pragma HLS ARRAY_PARTITION variable=coeff complete dim=0
#pragma HLS ARRAY_PARTITION variable=block complete dim=0

	int E[2], O[2];
	int rnd_factor = 1 << (shift - 1);
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		O[0] = (42) * coeff[1][i] + (17) * coeff[3][i];
		O[1] = (17) * coeff[1][i] + (-42) * coeff[3][i];
		E[0] = (32) * coeff[0][i] + (32) * coeff[2][i];
		E[1] = (32) * coeff[0][i] + (-32) * coeff[2][i];
		block[i][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
		block[i][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
		block[i][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
		block[i][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
	}
}

void itrans_h_4(s16 resi[4 * 4], s16 coef_temp[4 * 4], U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth)
{
	U4 shift2;
	static s16 block1[4][4];
	static s16 rec1[4][4];
	U7 size = 4;
	U8 size_b = size - 1;

#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=resi complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_temp complete dim=1
#pragma HLS ARRAY_PARTITION variable=block1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec1 complete dim=0

	//s16 coef_temp[MAX_TR_DIM];
	if (cu_width_log2 > 6 || cu_height_log2 > 6) {
		0 ? ((void)0) : __assert_HLS("0", "../separate/enc_pintra.c", (unsigned int)2554, __FUNCTION__);
	}
	else {
		shift2 = com_get_inverse_trans_shift(cu_height_log2, 1, bit_depth);


		for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
			for (int j = 0; j < 4; j++) {
#pragma HLS UNROLL
				block1[i][j] = coef_temp[i * 4 + j];
			}
		}

		inv_dct_row_4(block1, rec1, shift2);

		for (int i = 0; i < 4; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 4; j++) {
#pragma HLS UNROLL
				resi[i * 4 + j] = rec1[i][j];
			}
	}
}

void invTransHorChroma4(U4 bit_depth, int num_nz[4][3], s16 coef_temp[4 * 4], U3 type, pel resi[4 * 4])
{
#pragma HLS INLINE
	U3 tb_width_log2 = com_tbl_log2[4];
	U3 tb_height_log2 = com_tbl_log2[4];

	if (num_nz[0][type]) {
		itrans_h_4(resi, coef_temp, tb_width_log2, tb_height_log2, bit_depth);
	}

}

void recon_4(s16 resi[16], pel pred[16], int num_nz[4][3], int plane, U7 cu_width, U7 cu_height, int s_rec, pel rec[16], U4 bit_depth)
{
#pragma HLS INLINE
	int i;
	int j;
	s16 t0;
	//get_part_num(part);
#pragma HLS ARRAY_PARTITION variable=rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=pred complete dim=1
#pragma HLS ARRAY_PARTITION variable=resi complete dim=1

//#pragma HLS RESOURCE variable=rec core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=pred core=RAM_1P_BRAM

	if (num_nz[0][plane] == 0) {
		for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				rec[i * 4 + j] = (((0 > (((1 << bit_depth) - 1 < (pred[i * 4 + j]) ? (1 << bit_depth) - 1 : (pred[i * 4 + j]))) ? 0 : (((
				1 << bit_depth) - 1 < (pred[i * 4 + j]) ? (1 << bit_depth) - 1 : (pred[i * 4 + j]))))));
			}
		}
	}
	else 
	{
		for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				t0 = (((resi[i * 4 + j]) + (pred[i * 4 + j])));
				rec[i * 4 + j] = (((0 > (((1 << bit_depth) - 1 < (t0) ? (1 << bit_depth) - 1 : (t0))) ? 0 : (((
					1 << bit_depth) - 1 < (t0) ? (1 << bit_depth) - 1 : (t0))))));
			}
		}
	}
}


void reconChroma4(int x_pos, int y_pos, U13 tb_x, U13 tb_y, U4 bit_depth, int num_nz[4][3], int tb_part, pel pred_cache[4 * 4],
	pel rec[4 * 4], U3 type, pel resi[4 * 4],int num_nz_out[12], int num_nz_out1[12], S8 rdo_idx, U2 cu_mode)
{

#pragma HLS ARRAY_PARTITION variable=num_nz complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=num_nz_out1 complete dim=1
	recon_4(resi, pred_cache, num_nz, type, 4, 4, 4, rec, bit_depth);

	num_nz_out[rdo_idx] = num_nz[0][type];
	num_nz_out1[rdo_idx] = num_nz[0][type];
	if (cu_mode == 2)
	{
		num_nz_out1[0] = 0;
		num_nz_out1[1] = 0;
	}
}


int enc_bit_est_intra_chroma4(MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef[12][16]/*[N_C - 1][MAX_CU_DIM_C]*/,
	u8 tscpm_enable_flag, int num_nz[12],S8 rdo_idx,S8 ipm01)
{
#pragma HLS INLINE
	//COM_MODE *mod_info_curr = &core->mod_info_curr;

	//ENC_SBAC *sbac = &core_inner->s_temp_run;
	int enc_est = 0;
	int cbf_u, cbf_v;
	int cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	int cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	int num_nz_coef[3] = { 0 } /*= num_nz*/;
#pragma HLS ARRAY_PARTITION variable=coef complete dim=2
#pragma HLS ARRAY_PARTITION variable=num_nz_coef complete dim=1

	num_nz_coef[U_C] = num_nz[rdo_idx - 1];
	num_nz_coef[V_C] = num_nz[rdo_idx];

	//enc_est += encode_intra_dir_c_est(mod_info_curr_ptr->ipm[PB0][1], mod_info_curr_ptr->ipm[PB0][0], tscpm_enable_flag);
	enc_est += encode_intra_dir_c_est(ipm01, mod_info_curr_ptr->ipm[PB0][0], tscpm_enable_flag);

	cbf_u = (num_nz_coef[U_C] > 0) ? 1 : 0;
	cbf_v = (num_nz_coef[V_C] > 0) ? 1 : 0;

	//enc_sbac_encode_bin(cbf_u, sbac, sbac->ctx.cbf + 1, &core_inner->bs_temp);
	//enc_sbac_encode_bin(cbf_v, sbac, sbac->ctx.cbf + 2, &core_inner->bs_temp);
	enc_est += 2;


	cu_width_log2--;
	cu_height_log2--;

	if (cbf_u)
	{
		//enc_eco_xcoef(&core_inner->bs_temp, sbac, coef[U_C - 1], cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
		//enc_est += enc_eco_xcoef_est(coef[rdo_idx - 1], cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
		enc_est += enc_eco_run_length_cc_est_2(coef[rdo_idx - 1], cu_width_log2, cu_height_log2, num_nz_coef[U_C], U_C);
	}
	if (cbf_v)
	{
		//enc_eco_xcoef(&core_inner->bs_temp, sbac, coef[V_C - 1], cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
		//enc_est += enc_eco_xcoef_est(coef[rdo_idx], cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
		enc_est += enc_eco_run_length_cc_est_2(coef[rdo_idx], cu_width_log2, cu_height_log2, num_nz_coef[V_C], V_C);
	}
	return enc_est;

}

s64 get_bit_est_intra_chroma4(MD_COM_MODE_BEST* mod_info_curr_ptr, int cu_width, int cu_height,
	s16 coef_tmp[12][16]/*[N_C - 1][1024]*/, u8 tscpm_enable_flag, int num_nz[12], S8 rdo_idx, S8 ipm01)
{
#pragma HLS INLINE 
	int est_bit_intra;
	s64 cost = 0;
	int bit_cnt = 0;
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=num_nz complete dim=1

	if (cu_height == 8 && cu_width == 8) {
		est_bit_intra = enc_bit_est_intra_chroma4(mod_info_curr_ptr, coef_tmp, tscpm_enable_flag, num_nz, rdo_idx,ipm01);
		//bit_cnt = ((int)(0.8356 * ((double)est_bit_intra) + 3.124));
		return (9592 * est_bit_intra - 5506) >> 14;
	}
	return bit_cnt;
}

void deriveRDCostChroma4(U2 cu_mode, U32 dist_chroma_weight, MD_COM_MODE_BEST* mod_info_best, U3 cu_width_log2_chroma,
	U3 cu_height_log2_chroma, int cu_width, int cu_height, s16 rec_tmp[4 * 4], s16 orgU[16], s64 lambda_y,
	s16 coef_tmp[12][16], u8 tscpm_enable_flag, s64* cost, U4 bit_depth, int uv, int num_nz[4][3],int num_nz_out[12],S8 rdo_idx)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1


	s64 ssd_uv;
	static s64 ssd_u;
	static s64 ssd_v;
	int est_bit_intra_uv_tmp;
	static int est_bit_intra_u;
	static int est_bit_intra_v;

	int bin_cnt_chroma;

	if (cu_mode == 0) {
		s8 ipm01 = (S8)rdo_idx / 2;
		if (uv == 0)
			*cost = 0;
		*cost += (dist_chroma_weight * enc_ssd_16b_chroma_4x4(cu_width_log2_chroma, cu_height_log2_chroma, rec_tmp, orgU, 
		bit_depth) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	
		if (uv == 1) {
			bin_cnt_chroma = get_bit_est_intra_chroma4(mod_info_best, cu_width, cu_height, coef_tmp, tscpm_enable_flag, num_nz_out, rdo_idx, ipm01);
			*cost += (bin_cnt_chroma * lambda_y + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
		}
	}
}




//void predictChromaByMode4_intra_prepare(MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info,
//	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
//	int rdo_idx, s64* cost,
//	U3 cu_width_log2, U3 cu_height_log2, U13 pb_x, U13 pb_y, s16 coef_tmp[12][4 * 4], pel orgU[2][4 * 4],
//	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
//	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
//	pel nb_y[2][8 * 2 + 3], pel nb[3][2][4 * 2 + 3], U1* uv, U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
//	pel piRecoY[8][8],
//	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
//	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output)
//{
//	//*uv = rdo_idx & 1;
//	
//		/*if (*uv == 0)
//			*cost = 0;
//		mod_info_best->ipm[0][1] = (s8)rdo_idx / 2;*/
//
//		ipred_c_prepare(md_fw_ptr, cu_width_log2 - 1, cu_height_log2 - 1, tree_status, 0, pb_x, pb_y, 
//			mod_info_best, coef_tmp, orgU[0], nb_y, nb[0 + 1],
//			IP_buffer_ptr_U_4x4,
//			IP_buffer_ptr_V_4x4,
//			bit_depth, piRecoY,
//			IP_buffer_ptr_8x8, IP_map_scu_ptr_8x8, avail_cu_output
//		);
//
//		ipred_c_prepare(md_fw_ptr, cu_width_log2 - 1, cu_height_log2 - 1, tree_status, 1, pb_x, pb_y, 
//			mod_info_best, coef_tmp, orgU[1], nb_y, nb[1 + 1],
//			IP_buffer_ptr_U_4x4,
//			IP_buffer_ptr_V_4x4,
//			bit_depth, piRecoY,
//			IP_buffer_ptr_8x8, IP_map_scu_ptr_8x8, avail_cu_output
//		);
//
//	
//}

/*


void predictChromaByMode4_intra(MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
	pel pred[4 * 4], S8 rdo_idx, s64* cost,
	U3 cu_width_log2, U3 cu_height_log2, U13 pb_x, U13 pb_y, s16 coef_tmp[12][4 * 4], pel orgU[2][4 * 4], 
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	pel nb_y[2][8 * 2 + 3], pel nb[3][2][4 * 2 + 3], U1* uv, U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[8][8], 
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output)
{
	*uv = rdo_idx & 1;
	if (mod_info_best->cu_mode == 0)
	{
		if (*uv == 0)
			*cost = 0;
		mod_info_best->ipm[0][1] = (s8)rdo_idx / 2;
#if USE_TSCPM
		ipred_c_4(md_fw_ptr, cu_width_log2 - 1, cu_height_log2 - 1, tree_status, *uv, pb_x, pb_y, pred,
			mod_info_best, coef_tmp[rdo_idx], orgU[*uv], nb_y, nb[(rdo_idx & 1) + 1][0], nb[(rdo_idx & 1) + 1][1], rdo_idx,
			IP_buffer_ptr_U_4x4,
			IP_buffer_ptr_V_4x4,
			bit_depth, piRecoY,
			IP_buffer_ptr_8x8, IP_map_scu_ptr_8x8, avail_cu_output
		);
	}
#else
		ipred_c(md_fw_ptr, me_line_map_ptr, cu_width_log2 - 1, cu_height_log2 - 1, tree_status, *uv, pb_x, pb_y, pred,
			mod_info_best, coef_tmp[rdo_idx], orgU[*uv], nb[*uv + 1], IP_buffer_ptr, MAP_SCU_IP_ptr, bit_depth, piRecoY);
#endif
	
}
*/
int mode_cu_init_chroma_4_8(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,
	MD_COM_MODE_BEST* bst_info_ptr, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status, U2* cons_pred_mode, ENC_CU_DATA_ARRAY_INNER* cu_data_temp_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	MD_COM_MODE_BEST* bst_info = bst_info_ptr;

	//bst_info->x_luma = bst_info->x_pos;
	//bst_info->y_luma = bst_info->y_pos;
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
	if (tree_status == TREE_C) {
		U6 cu_w_scu = cu_width >> 2;
		U6 cu_h_scu = cu_height >> 2;
		// bottom-right
		U13 luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;
		luma_pred_mode = cu_data_temp_ptr->pred_mode[luma_pos_scu];
		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
#if SUB_TMVP
		if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE
			&& cu_data_temp_ptr->skip_idx[luma_pos_scu] == 0 && cu_data_temp_ptr->pred_mode[luma_pos_scu] >= MODE_SKIP) {
			for (i = 0; i < SBTMVP_NUM; i++) {
				//int blk = ((i >= (cu_width >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? 1 : 0) + ((j >= (cu_height >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? SBTMVP_NUM_1D : 0);
				int sbtmvp_pos;// = cu_w_scu * (cu_h_scu / SBTMVP_NUM_1D - 1) * (i / 2) + (cu_w_scu / SBTMVP_NUM_1D - 1) * (i % 2);
				sbtmvp_pos = ((cu_h_scu / SBTMVP_NUM_1D) * ((i / 2) + 1) - 1) * cu_w_scu + ((cu_w_scu / SBTMVP_NUM_1D) * ((i % 2) + 1) - 1);
				core_sbtmvp->sbTmvp[i].ref_idx[0] = cu_data_temp_ptr->refi[sbtmvp_pos][0];
				core_sbtmvp->sbTmvp[i].ref_idx[1] = cu_data_temp_ptr->refi[sbtmvp_pos][1];

				core_sbtmvp->sbTmvp[i].mv[0][0] = cu_data_temp_ptr->mv[sbtmvp_pos][0][0];
				core_sbtmvp->sbTmvp[i].mv[0][1] = cu_data_temp_ptr->mv[sbtmvp_pos][0][1];
				core_sbtmvp->sbTmvp[i].mv[1][0] = cu_data_temp_ptr->mv[sbtmvp_pos][1][0];
				core_sbtmvp->sbTmvp[i].mv[1][1] = cu_data_temp_ptr->mv[sbtmvp_pos][1][1];
			}
		}
#endif
		bst_info->cu_mode = luma_pred_mode;
		bst_info->ipm[0][0] = cu_data_temp_ptr->ipm[0][luma_pos_scu];
		bst_info->ipm[0][1] = 0;
		bst_info->affine_flag = cu_data_temp_ptr->affine_flag[luma_pos_scu];

		for (i = 0; i < 2; i++) {
			bst_info->refi[i] = cu_data_temp_ptr->refi[luma_pos_scu][i];
			bst_info->mv[i][0] = cu_data_temp_ptr->mv[luma_pos_scu][i][0];
			bst_info->mv[i][1] = cu_data_temp_ptr->mv[luma_pos_scu][i][1];
		}
	}

	bst_info->x_chroma = cu_data_temp_ptr->x_chroma;
	bst_info->y_chroma = cu_data_temp_ptr->y_chroma;
	return 0;
}


int copy_cu_data_array_src_8(ENC_CU_DATA_ARRAY_INNER* dst, ENC_CU_DATA_ARRAY_INNER* src, U14 x, U14 y, U3 cu_width_log2,
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
	dst->x_chroma = src->x_chroma;
	dst->y_chroma = src->y_chroma;
	dst->last_child = src->last_child;

		for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
			idx_src = (U22)j << (cu_width_log2 - 2);

			for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
				(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
				(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
				(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
				(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
				//(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];

			}

			for (i = 0; i < 8; i++) {
				for (ii = 0; ii < cuw_scu; ii++)
#pragma HLS LOOP_TRIPCOUNT max=16
					(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];

			}
			for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
				(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
				(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
				(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
				(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
				(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];

			}

			for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				for (i = 0; i < 2; i++) {
					(dst->refi + idx_dst)[ii][i] = (src->refi + idx_src)[ii][i];
				}

			}

			for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
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

			}


			for (k = 0; k < 1; k++) {
				for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
					(dst->num_nz_coef[k] + idx_dst)[i] = (src->num_nz_coef[k] + idx_src)[i];
				}
			}

			for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->pb_part + idx_dst)[i] = (src->pb_part + idx_src)[i];
				(dst->tb_part + idx_dst)[i] = (src->tb_part + idx_src)[i];
			}

		}



		for (j = 0; j < cu_height; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
			idx_dst = ((U22)(y + j) << log2_cus) + x;
			idx_src = (U22)j << cu_width_log2;

			for (i = 0; i < cu_width; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
				dst->coef_y[y + j][x + i] = src->coef_y[j][i];
				dst->reco_y[y + j][x + i] = src->reco_y[j][i];
			}
		}

	return 0;
}

void cost_compare_fixed_point_8(MD_FW* md_fw_ptr, s64 cost_best[1], s64 cost_temp, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_temp_array_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3 best_split_mode[1],
	U2 best_cons_pred_mode[1], U2 cons_pred_mode_child, U2 tree_status_child, U2 best_child_status[1], U2 tree_status_temp[1])
{
	if (cost_best[0] > cost_temp) {
		cost_best[0] = cost_temp;
		best_split_mode[0] = split_mode;
		best_cons_pred_mode[0] = cons_pred_mode_child;
		best_child_status[0] = tree_status_child;
	}

	if (best_split_mode[0] != NO_SPLIT)
	{
		tree_status_temp[0] = TREE_C;
	}
}

void cu_data_chroma_pos_8(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_INNER* dst, ENC_CU_DATA_ARRAY_INNER* src, int depth)
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
void cu_data_chroma_pos_8(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_SIZE<8>* dst, ENC_CU_DATA_ARRAY_INNER* src, int depth)
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
void cu_data_chroma_pos_8(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_SIZE<8>* dst, ENC_CU_DATA_ARRAY_SIZE<16>* src, int depth)
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


int copy_cu_data_array_dst_8(ENC_CU_DATA_ARRAY_INNER* dst, ENC_CU_DATA_ARRAY_INNER* src, U14 x, U14 y, U3 cu_width_log2,
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
			(dst->pred_mode + idx_dst)[i] = (src->pred_mode + idx_src)[i];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[i] = (src->cu_luma_qp + idx_src)[i];
#endif
			(dst->mpm[0] + idx_dst)[i] = (src->mpm[0] + idx_src)[i];
			(dst->mpm[1] + idx_dst)[i] = (src->mpm[1] + idx_src)[i];
			(dst->ipm[0] + idx_dst)[i] = (src->ipm[0] + idx_src)[i];
			//(dst->ipm[1] + idx_dst)[i] = (src->ipm[1] + idx_src)[i];
		}

		for (i = 0; i < 8; i++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->mpm_ext[i] + idx_dst)[i1] = (src->mpm_ext[i] + idx_src)[i1];
			}

		}
		for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->affine_flag + idx_dst)[i] = (src->affine_flag + idx_src)[i];
			(dst->smvd_flag + idx_dst)[i] = (src->smvd_flag + idx_src)[i];
			(dst->depth + idx_dst)[i] = (src->depth + idx_src)[i];
			(dst->map_scu + idx_dst)[i] = (src->map_scu + idx_src)[i];
			(dst->map_cu_mode + idx_dst)[i] = (src->map_cu_mode + idx_src)[i];
			(dst->map_pb_tb_part + idx_dst)[i] = (src->map_pb_tb_part + idx_src)[i];

			for (i1 = 0; i1 < 2; i1++)
				(dst->refi + idx_dst)[i][i1] = (src->refi + idx_src)[i][i1];

			(dst->umve_flag + idx_dst)[i] = (src->umve_flag + idx_src)[i];
			(dst->umve_idx + idx_dst)[i] = (src->umve_idx + idx_src)[i];
			(dst->skip_idx + idx_dst)[i] = (src->skip_idx + idx_src)[i];
			(dst->mvr_idx + idx_dst)[i] = (src->mvr_idx + idx_src)[i];
			(dst->mvp_from_hmvp_flag + idx_dst)[i] = (src->mvp_from_hmvp_flag + idx_src)[i];
			(dst->ipf_flag + idx_dst)[i] = (src->ipf_flag + idx_src)[i];
			for (i1 = 0; i1 < 2; i1++) {
				for (i2 = 0; i2 < 2; i2++) {
					(dst->mv + idx_dst)[i][i1][i2] = (src->mv + idx_src)[i][i1][i2];
					(dst->mvd + idx_dst)[i][i1][i2] = (src->mvd + idx_src)[i][i1][i2];
				}
			}

		}

		for (k = 0; k < 1; k++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->num_nz_coef[k] + idx_dst)[i1] = (src->num_nz_coef[k] + idx_src)[i1];
			}
		}

		for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->pb_part + idx_dst)[i1] = (src->pb_part + idx_src)[i1];
			(dst->tb_part + idx_dst)[i1] = (src->tb_part + idx_src)[i1];
		}

	}


	for (j = 0; j < cu_height; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
		idx_dst = ((U22)(y + j) << log2_cus) + x;
		idx_src = (U22)j << cu_width_log2;

		for (i = 0; i < cu_width; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
			dst->coef_y[y + j][x + i] = src->coef_y[j][i];
			dst->reco_y[y + j][x + i] = src->reco_y[j][i];
		}
	}

	return 0;
}

void update_map_scu_8(MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best_ptr, 
	U13 x, U13 y, U8 src_cuw, U8 src_cuh,
	U2 tree_status)
{
	//s8 *map_ipm = 0;
	S8* src_map_ipm = 0;
	//int size_depth;
	U6 w;
	U6 h;
	u16 i;
	/*int size;
	int size_ipm;
	int size_mv;
	int size_refi;*/
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
	/*size = (sizeof(u32) * w);
	size_ipm = (sizeof(u8) * w);
	size_mv = (sizeof(s16) * w * 2 * 2);
	size_refi = (sizeof(s8) * w* 2);
	size_depth = (sizeof(s8)* w);*/
	mpm_x = scu_x & 15;
	mpm_y = scu_y & 15;
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (ii = 0; ii < w; ii++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MPM_buffer_ptr->buffer[i + mpm_y][ii + mpm_x] = (src_map_ipm[(i << (log2_src_cuw - 2)) + ii]);
//		}
//	}
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		src_map_ipm += src_cuw >> 2;
	}
	//x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	//y_scu_line = scu_y - me_line_map_ptr->min_scu_y;
	j = 0;
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			//me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
			//	= curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
			//me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i] = curr_cu_data_best_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
			//inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i].refi[0] = curr_cu_data_best_ptr->refi[j + (i << (log2_src_cuw - 2))][0];
			//inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i].refi[1] = curr_cu_data_best_ptr->refi[j + (i << (log2_src_cuw - 2))][1];
			//inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i].mv[0][0] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][0][0];
			//inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i].mv[0][1] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][0][1];
			//inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i].mv[1][0] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][1][0];
			//inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
			//	i].mv[1][1] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][1][1];
			/*inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
				i].pb_tb_part = curr_cu_data_best_ptr->map_pb_tb_part[j + (i << (log2_src_cuw - 2))];*/
		}
	}
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (j = 0; j < w; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i] =
//				curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
//		}
//	}
}

void store_enc_info_best_8_16(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best, 
	U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY_INNER* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw, 
	U3 part_num)
{

	copy_cu_data_array_dst_8(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
		cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status);

	update_map_scu_8(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}


static s64 circuit_8x8_top(
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

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	/***************************** Step 3: reduce split modes by fast algorithm ********************************/
	split_struct.split_allow[part_num][SPLIT_BI_VER] = 0;
	split_struct.split_allow[part_num][SPLIT_BI_HOR] = 0;


	assert(cu_width == 8 && cu_height == 8 && split_mode == SPLIT_QUAD);
	static IP_ARRAY_SIZE<8, 8>			IP_buffer_8x8;
	static MPM_ARRAY_SIZE<8, 8>			MPM_buffer_8x8;
	static IP_MAP_SCU_SIZE<8, 8>		IP_map_scu_8x8;
	static ENC_ME_NEB_INFO				up_nebs[1 + (8 >> 2) + 1];
	static ENC_ME_NEB_INFO				left_nebs[(8 >> 2)];
	static ENC_ME_NEB_INFO				nebs_line_hor[2 + 2];
	static ENC_ME_NEB_INFO				nebs_line_ver[2];
	static ENC_ME_LINE_MAP_SIZE<8>		me_line_map_8x8;
	static ENC_CU_DATA_ARRAY_SIZE<8>	cu_data_ptr;

	//cu_data_chroma_pos_8(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &cu_data_ptr, prev_cu_data_ptr, qt_depth + bet_depth);

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];

	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif


	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;
	S7 ip_array_size_idx = get_ip_array_size_idx(16, 16);

	me_line_map_8x8.fetch_me_line_qt(part_num, me_line_map_ptr_16, cu_width, cu_height);
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < 16; ++i)
		{
			IP_buffer_8x8.up_line[i] = IP_buffer_ptr_16x16->up_line[i];
			IP_buffer_8x8.left_line[i] = IP_buffer_ptr_16x16->left_line[i];
		}
		IP_buffer_8x8.top_left = IP_buffer_ptr_16x16->top_left;

		MPM_buffer_8x8.up = MPM_buffer_ptr_16x16->up_line[0];
		MPM_buffer_8x8.left = MPM_buffer_ptr_16x16->left_line[0];

		for (int i = 0; i < 4; i++)
		{
			IP_map_scu_8x8.map_scu_up_line[i] = IP_map_scu_ptr_16x16->map_scu_up_line[i];
			IP_map_scu_8x8.map_scu_left_line_new[i] = IP_map_scu_ptr_16x16->map_scu_left_line_new[i];
		}
		IP_map_scu_8x8.map_scu_top_left_new = IP_map_scu_ptr_16x16->map_scu_top_left_new;

		for (int i = 0; i < 4; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[i].mv[m][n];
				}
			}
		}
		break;
	case 1:
		for (int i = 0; i < 16; ++i)
		{
			IP_buffer_8x8.up_line[i] = IP_buffer_ptr_16x16->up_line[i + 8];
			IP_buffer_8x8.left_line[i] = IP_buffer_8x8.rec_line_ver[i];
		}
		IP_buffer_8x8.top_left = IP_buffer_ptr_16x16->up_line[7];

		MPM_buffer_8x8.up = MPM_buffer_ptr_16x16->up_line[(8 >> 2)];
		MPM_buffer_8x8.left = MPM_buffer_8x8.ipm_line_ver[0];

		for (int i = 0; i < 4; i++)
		{
			IP_map_scu_8x8.map_scu_up_line[i] = IP_map_scu_ptr_16x16->map_scu_up_line[i + 2];
			IP_map_scu_8x8.map_scu_left_line_new[i] = IP_map_scu_ptr_16x16->cur_map_scu[1][i];
		}
		IP_map_scu_8x8.map_scu_top_left_new = IP_map_scu_ptr_16x16->map_scu_up_line[1];

		for (int i = 0; i < 4; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[(8 >> 2) + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[(8 >> 2) + i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = nebs_line_ver[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = nebs_line_ver[i].mv[m][n];
				}
			}
		}
		break;
	case 2:
		for (int i = 0; i < 16; ++i)
		{
			IP_buffer_8x8.up_line[i] = IP_buffer_8x8.rec_line_hor[i];
			IP_buffer_8x8.left_line[i] = IP_buffer_ptr_16x16->left_line[i + 8];
		}
		IP_buffer_8x8.top_left = IP_buffer_ptr_16x16->left_line[7];

		MPM_buffer_8x8.up = MPM_buffer_8x8.ipm_line_hor[0];
		MPM_buffer_8x8.left = MPM_buffer_ptr_16x16->left_line[(8 >> 2)];
		
		for (int i = 0; i < 4; i++)
		{
			IP_map_scu_8x8.map_scu_up_line[i] = IP_map_scu_ptr_16x16->cur_map_scu[i][1];
			IP_map_scu_8x8.map_scu_left_line_new[i] = IP_map_scu_ptr_16x16->map_scu_left_line_new[i + (cu_height >> 2)];
		}
		IP_map_scu_8x8.map_scu_top_left_new = IP_map_scu_ptr_16x16->map_scu_left_line_new[(cu_height >> 2) - 1];

		for (int m = 0; m < REFP_NUM; m++)
		{
			up_nebs[0].refi[m] = left_nebs_father[(8 >> 2) - 1].refi[m];
			for (int n = 0; n < MV_D; n++)
			{
				up_nebs[0].mv[m][n] = left_nebs_father[(8 >> 2) - 1].mv[m][n];
			}
		}
		for (int i = 0; i < 3; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i + 1].refi[m] = nebs_line_hor[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i + 1].mv[m][n] = nebs_line_hor[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[(8 >> 2) + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[(8 >> 2) + i].mv[m][n];
				}
			}
		}
		break;
	case 3:
		for (int i = 0; i < 8; ++i)
		{
			IP_buffer_8x8.up_line[i] = IP_buffer_8x8.rec_line_hor[i + 8];
			IP_buffer_8x8.left_line[i] = IP_buffer_8x8.rec_line_ver[i + 8];
		}
		IP_buffer_8x8.top_left = IP_buffer_8x8.rec_line_hor[7];

		MPM_buffer_8x8.up = MPM_buffer_8x8.ipm_line_hor[(8 >> 2)];
		MPM_buffer_8x8.left = MPM_buffer_8x8.ipm_line_ver[0];

		clear_map_scu_line(IP_map_scu_8x8.map_scu_up_line, (8 >> 2) << 1);
		clear_map_scu_line(IP_map_scu_8x8.map_scu_left_line_new, (8 >> 2) << 1);
		for (int i = 0; i < 2; i++)
		{
			IP_map_scu_8x8.map_scu_up_line[i] = IP_map_scu_ptr_16x16->cur_map_scu[2 + i][1];
			IP_map_scu_8x8.map_scu_left_line_new[i] = IP_map_scu_ptr_16x16->cur_map_scu[1][2 + i];
		}
		IP_map_scu_8x8.map_scu_top_left_new = IP_map_scu_ptr_16x16->cur_map_scu[1][1];

		for (int i = 0; i < 3; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = nebs_line_hor[1 + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = nebs_line_hor[1 + i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < 2; i++)
		{
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = nebs_line_ver[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = nebs_line_ver[i].mv[m][n];
				}
			}
		}
		break;
	default:
		assert(0);
	}

	cost_best = md_kernel_8(cu_width, cu_height,
		md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		me_mv_dir,
#endif
		p_fenc_LCU_Y,
		&me_line_map_8x8,
		up_nebs,
		left_nebs,
		skip_in,
		boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&cu_data_ptr,
		fme_mv_info,
		&IP_buffer_8x8,
		&MPM_buffer_8x8,
		&IP_map_scu_8x8,
		ref,
		Fetch_Ref_window_ptr);

	int scu_h = (y0 + 8 > md_fw_ptr->pic_height ? ((md_fw_ptr->pic_height - y0) >> 2) : (8 >> 2));
	int scu_w = (x0 + 8 > md_fw_ptr->pic_width ? ((md_fw_ptr->pic_width - x0) >> 2) : (8 >> 2));
	me_line_map_8x8.update_me_line_qt(part_num, cu_data_ptr.map_scu, cu_data_ptr.map_cu_mode, cu_width, cu_height);
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < 8; ++i)
		{
			IP_buffer_8x8.rec_line_hor[i] = cu_data_ptr.reco_y/*[i + 56]*/[7][i];
			IP_buffer_8x8.rec_line_ver[i] = cu_data_ptr.reco_y/*[i * 8 + 7]*/[i][7];
		}

		for (int i = 0; i < (8 >> 2); ++i)
		{
			MPM_buffer_8x8.ipm_line_hor[i] = cu_data_ptr.ipm[0][i + ((8 >> 2) - 1) << 1];
			MPM_buffer_8x8.ipm_line_ver[i] = cu_data_ptr.ipm[0][((8 >> 2) - 1) + (i << 1)];
		}

		for (int i = 0; i < scu_h; i++)
		{
			#pragma HLS loop_tripcount max=16
			for (int j = 0; j < scu_w; j++)
			{
				#pragma HLS loop_tripcount max=16
				IP_map_scu_ptr_16x16->cur_map_scu[j][i] = cu_data_ptr.map_scu[j + (i << (cu_width_log2 - 2))];
			}
		}

		for (int i = 0; i < 2; i++)
		{
			nebs_line_hor[i].refi[0] = cu_data_ptr.refi[i + (((8 >> 2) - 1) << 1)][0];
			nebs_line_hor[i].refi[1] = cu_data_ptr.refi[i + (((8 >> 2) - 1) << 1)][1];
			nebs_line_hor[i].mv[0][0] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][0][0];
			nebs_line_hor[i].mv[0][1] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][0][1];
			nebs_line_hor[i].mv[1][0] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][1][0];
			nebs_line_hor[i].mv[1][1] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][1][1];

			nebs_line_ver[i].refi[0] = cu_data_ptr.refi[((8 >> 2) - 1) + (i << 1)][0];
			nebs_line_ver[i].refi[1] = cu_data_ptr.refi[((8 >> 2) - 1) + (i << 1)][1];
			nebs_line_ver[i].mv[0][0] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][0][0];
			nebs_line_ver[i].mv[0][1] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][0][1];
			nebs_line_ver[i].mv[1][0] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][1][0];
			nebs_line_ver[i].mv[1][1] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][1][1];
		}
		break;
	case 1:
		for (int i = 0; i < 8; ++i)
		{
			IP_buffer_8x8.rec_line_hor[i + 8] = cu_data_ptr.reco_y/*[i + 56]*/[7][i];
		}

		for (int i = 0; i < (8 >> 2); ++i)
		{
			MPM_buffer_8x8.ipm_line_hor[i + (8 >> 2)] = cu_data_ptr.ipm[0][i + ((8 >> 2) - 1) << 1];
		}

		for (int i = 0; i < scu_h; i++)
		{
			#pragma HLS loop_tripcount max=16
			for (int j = 0; j < scu_w; j++)
			{
				#pragma HLS loop_tripcount max=16
				IP_map_scu_ptr_16x16->cur_map_scu[(cu_width >> 2) + j][i] = cu_data_ptr.map_scu[j + (i << (cu_width_log2 - 2))];
			}
		}

		for (int i = 0; i < 2; i++)
		{
			nebs_line_hor[i + 2].refi[0] = cu_data_ptr.refi[i + (((8 >> 2) - 1) << 1)][0];
			nebs_line_hor[i + 2].refi[1] = cu_data_ptr.refi[i + (((8 >> 2) - 1) << 1)][1];
			nebs_line_hor[i + 2].mv[0][0] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][0][0];
			nebs_line_hor[i + 2].mv[0][1] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][0][1];
			nebs_line_hor[i + 2].mv[1][0] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][1][0];
			nebs_line_hor[i + 2].mv[1][1] = cu_data_ptr.mv[i + (((8 >> 2) - 1) << 1)][1][1];
		}
		break;
	case 2:
		for (int i = 0; i < 8; ++i)
		{
			IP_buffer_8x8.rec_line_ver[i + 8] = cu_data_ptr.reco_y/*[i * 8 + 7]*/[i][7];
		}

		for (int i = 0; i < (8 >> 2); ++i)
		{
			MPM_buffer_8x8.ipm_line_ver[i] = cu_data_ptr.ipm[0][((8 >> 2) - 1) + (i << 1)];
		}

		for (int i = 0; i < scu_h; i++)
		{
			#pragma HLS loop_tripcount max=16
			for (int j = 0; j < scu_w; j++)
			{
				#pragma HLS loop_tripcount max=16
				IP_map_scu_ptr_16x16->cur_map_scu[j][(cu_height >> 2) + i] = cu_data_ptr.map_scu[j + (i << (cu_width_log2 - 2))];
			}
		}

		for (int i = 0; i < 2; i++)
		{
			nebs_line_ver[i].refi[0] = cu_data_ptr.refi[((8 >> 2) - 1) + (i << 1)][0];
			nebs_line_ver[i].refi[1] = cu_data_ptr.refi[((8 >> 2) - 1) + (i << 1)][1];
			nebs_line_ver[i].mv[0][0] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][0][0];
			nebs_line_ver[i].mv[0][1] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][0][1];
			nebs_line_ver[i].mv[1][0] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][1][0];
			nebs_line_ver[i].mv[1][1] = cu_data_ptr.mv[((8 >> 2) - 1) + (i << 1)][1][1];
		}
		break;
	case 3:
		break;
	default:
		assert(0);
	}

	prev_cu_data_ptr->get_cu_data_from_curblk(&cu_data_ptr, (x0 - split_struct.x_pos[0]), (y0 - split_struct.y_pos[0]), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);

	//store_enc_info_best_8_16(md_fw_ptr, &cu_data_temp_NS, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status,
	//	&best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array,
	//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, part_num);

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

void store_enc_info_best(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
#pragma HLS INLINE off
	/* restore best data */
	com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width,
		curr_cu_data_best->split_mode);
	if (cons_pred_mode == NO_MODE_CONS
		&& com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type)) {
		com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width,
			curr_cu_data_best->split_mode);
	}

	copy_cu_data_array_dst(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
		cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status);

	//update_map_scu(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}

void store_enc_info_best(MD_FW*md_fw_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3 *best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY *prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
#pragma HLS INLINE off
    /* restore best data */
    com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width,
		curr_cu_data_best->split_mode);
    if (cons_pred_mode==NO_MODE_CONS
            && com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type)) {
        com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width,
			curr_cu_data_best->split_mode);
    }

	copy_cu_data_array_dst(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
		cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status);

	//update_map_scu(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}

void cost_compare_fixed_point_one_time(MD_FW*md_fw_ptr, s64 *cost_best, s64 *cost, ENC_CU_DATA_ARRAY *curr_cu_data_temp_array_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 *split_mode, U3 *best_split_mode,
	U2 *best_cons_pred_mode, U2 *cons_pred_mode_child, U2 *tree_status_child, U2 *best_child_status, U2 *tree_status_temp, U3 split_mode_num)
{
	S4 best_index = -1;
	for (U3 i = 0; i < split_mode_num; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max= 4
		if (*cost_best > cost[i])
		{
			*cost_best = cost[i];
			best_index = i;
		}
	}

	if (best_index != -1)
	{
		copy_cu_data_array_src(curr_cu_data_best_ptr,
			&curr_cu_data_temp_array_ptr[best_index], 0, 0, cu_width_log2, cu_height_log2,
			cu_width_log2, cud, tree_status);

		*best_split_mode = split_mode[best_index];
		*best_cons_pred_mode = cons_pred_mode_child[best_index];
		*best_child_status = tree_status_child[best_index];
	}

	if (*best_split_mode != NO_SPLIT)
	{
		*tree_status_temp = TREE_C;
	}
}


void cost_compare_fixed_point(MD_FW*md_fw_ptr, s64 cost_best[1], s64 cost_temp, ENC_CU_DATA_ARRAY *curr_cu_data_temp_array_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best_ptr,
                              U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3 best_split_mode[1],
							  U2 best_cons_pred_mode[1], U2 cons_pred_mode_child, U2 tree_status_child, U2 best_child_status[1], U2 tree_status_temp[1])
{
    if (cost_best[0] > cost_temp) {
		copy_cu_data_array_src(curr_cu_data_best_ptr,
			curr_cu_data_temp_array_ptr, 0, 0, cu_width_log2, cu_height_log2,
                               cu_width_log2, cud, tree_status);
        cost_best[0] = cost_temp;
        best_split_mode[0] = split_mode;
        best_cons_pred_mode[0] = cons_pred_mode_child;
        best_child_status[0] = tree_status_child;
    }

	if (best_split_mode[0] != NO_SPLIT)
	{
		tree_status_temp[0] = TREE_C;
	}
}


s64 prepare_cu_info_COM_combine_only_qt_8(MD_FW* md_fw_ptr, s64 lambdaY, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
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

	s64 cost_temp = 0;
	U3 rate_bit = 0;

	if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		//com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);
		rate_bit ++;
		cost_temp += (rate_bit * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}


s64 md_8x8_loop(U1 btv_allowed, 
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], 
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4],
	SKIP_MERGE_INPUT* skip_in, U3 split_mode, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag, 
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_ptr,
	U2 cons_pred_mode_BTV[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, 
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16, 
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	if (btv_allowed) {
		child_tree_status[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);
		bth_cost = prepare_cu_info_COM_combine_only_qt_8(md_fw_ptr, md_input_ptr->lambda_y, x0, y0,cup, cud,
			qt_depth, bet_depth, cu_width, cu_height, SPLIT_QUAD, &split_struct);

		for (int i = 0; i < (cu_height >> 2); i++)
			for (int j = 0; j < (cu_width >> 2); j++)
				IP_map_scu_ptr_16x16->cur_map_scu[j][i] = 0;

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			bth_cost += circuit_8x8_top( 
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
				qt_depth + 1, bet_depth, cons_pred_mode_BTV[0],
				child_tree_status[0], part_num, split_struct, cu_width_log2, cu_height_log2, fme_mv_info, 
				IP_buffer_ptr_16x16, 
				MPM_buffer_ptr_16x16, 
				IP_map_scu_ptr_16x16, 
				ref,
				Fetch_Ref_window_ptr, SPLIT_QUAD);
		}
	}
	return bth_cost;
}


void copy_to_cu_data_chroma8(U6 qp_y, ENC_CU_DATA_ARRAY_INNER* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1])
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
	if (tree_status != TREE_C)
	{
		for (i = 0; i < mod_info_best->cu_width * mod_info_best->cu_height; i++) {
			num_coef_y += ((cu_data->coef_y[i]) != 0 ? 1 : 0);
		}
	}
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
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] | ((1 << 24));
				}
				else {
					cu_data->map_scu[idx + i] = cu_data->map_scu[idx + i] & ((~(1 << 24)));
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

void test_1(int split_mode, ENC_ME_NEB_INFO	up_nebs[6], ENC_ME_NEB_INFO	 left_nebs[4],
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4], 
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16, ENC_ME_LINE_MAP_SIZE<16> *me_line_map_16,
	U8 cu_width, U8 cu_height, U3 part_num, IP_ARRAY_SIZE<8, 16> *IP_buffer_8x16,
	IP_ARRAY_SIZE<16, 8> *IP_buffer_16x8, IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_8x16,
	IP_MAP_SCU_SIZE<16, 8>*IP_map_scu_16x8, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16,IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	MPM_ARRAY_SIZE<8, 16> * MPM_buffer_8x16, MPM_ARRAY_SIZE<16, 8> *MPM_buffer_16x8,
	ENC_ME_NEB_INFO nebs_line_hor[4],ENC_ME_NEB_INFO nebs_line_ver[4]
	)
{
	switch (split_mode)
	{
	case NO_SPLIT:
		for (int i = 0; i < 6; i++)
		{
#pragma HLS PIPELINE
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[i].mv[m][n];
				}
			}
			if (i < 4) {
				for (int m = 0; m < REFP_NUM; m++)
				{
					left_nebs[i].refi[m] = left_nebs_father[i].refi[m];
					for (int n = 0; n < MV_D; n++)
					{
						left_nebs[i].mv[m][n] = left_nebs_father[i].mv[m][n];
					}
				}
			}
		}
		cpy_1d_2_1d<U32, 6>(me_line_map_ptr_16->map_scu_up_line, me_line_map_16->map_scu_up_line);
		cpy_1d_2_1d<U32, 6>(me_line_map_ptr_16->map_cu_mode_up_line, me_line_map_16->map_cu_mode_up_line);
		cpy_1d_2_1d<U32, 4>(me_line_map_ptr_16->map_scu_left_line, me_line_map_16->map_scu_left_line);
		cpy_1d_2_1d<U32, 4>(me_line_map_ptr_16->map_cu_mode_left_line, me_line_map_16->map_cu_mode_left_line);
		me_line_map_16->min_scu_x = me_line_map_ptr_16->min_scu_x;
		me_line_map_16->min_scu_y = me_line_map_ptr_16->min_scu_y;
		me_line_map_16->above_line_idx = me_line_map_ptr_16->above_line_idx;
		me_line_map_16->curr_ctu_idx = me_line_map_ptr_16->curr_ctu_idx;
		break;
	case SPLIT_BI_VER:
		assert(cu_width == 8 && cu_height == 16);
		IP_buffer_8x16->fetch_bi_ver(part_num, IP_buffer_ptr_16x16);
		IP_map_scu_8x16->fetch_bi_ver(part_num, IP_map_scu_ptr_16x16);
		MPM_buffer_8x16->fetch_bi_ver(part_num, MPM_buffer_ptr_16x16);
		fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
		me_line_map_16->fetch_bi_ver(part_num, me_line_map_ptr_16, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 16 && cu_height == 8);
		IP_buffer_16x8->fetch_bi_hor(part_num, IP_buffer_ptr_16x16);
		IP_map_scu_16x8->fetch_bi_hor(part_num, IP_map_scu_ptr_16x16);
		MPM_buffer_16x8->fetch_bi_hor(part_num, MPM_buffer_ptr_16x16);
		fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
		me_line_map_16->fetch_bi_hor(part_num, me_line_map_ptr_16, cu_width, cu_height);
		break;
	default: assert(0);
	}
}

void test_2(int split_mode, ENC_ME_LINE_MAP_SIZE<16>* me_line_map_16,
	U8 cu_width, U8 cu_height, U3 part_num, IP_ARRAY_SIZE<8, 16>* IP_buffer_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_16x8, IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_8x16,
	IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_16x8, 
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	MPM_ARRAY_SIZE<8, 16>* MPM_buffer_8x16, MPM_ARRAY_SIZE<16, 8>* MPM_buffer_16x8,
	ENC_ME_NEB_INFO nebs_line_hor[4], ENC_ME_NEB_INFO nebs_line_ver[4], 
	ENC_CU_DATA_ARRAY_SIZE<16> cu_data_temp_ptr_16
)
{
	switch (split_mode)
	{
	case NO_SPLIT:
		break;
	case SPLIT_BI_VER:
		assert(cu_width == 8 && cu_height == 16);
		IP_buffer_8x16->update_bi_ver(part_num, cu_data_temp_ptr_16.reco_y);
		IP_map_scu_8x16->update_bi_ver(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_temp_ptr_16.map_scu);
		MPM_buffer_8x16->update_bi_ver(part_num, cu_data_temp_ptr_16.ipm[0]);
		update_nebs_bi_ver(nebs_line_ver, cu_data_temp_ptr_16.refi, cu_data_temp_ptr_16.mv, cu_width, cu_height, part_num);
		me_line_map_16->update_bi_ver(part_num, cu_data_temp_ptr_16.map_scu, cu_data_temp_ptr_16.map_cu_mode, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 16 && cu_height == 8);
		IP_buffer_16x8->update_bi_hor(part_num, cu_data_temp_ptr_16.reco_y);
		IP_map_scu_16x8->update_bi_hor(part_num, IP_map_scu_ptr_16x16->cur_map_scu, cu_data_temp_ptr_16.map_scu);
		MPM_buffer_16x8->update_bi_hor(part_num, cu_data_temp_ptr_16.ipm[0]);
		update_nebs_bi_hor(nebs_line_hor, cu_data_temp_ptr_16.refi, cu_data_temp_ptr_16.mv, cu_width, cu_height, part_num);
		me_line_map_16->update_bi_hor(part_num, cu_data_temp_ptr_16.map_scu, cu_data_temp_ptr_16.map_cu_mode, cu_width, cu_height);
		break;
	default: assert(0);
	}
}
s64 circuit_16x16_16x8_8x16_md1(
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
	U2 cons_pred_mode_child[3] = { NO_MODE_CONS,NO_MODE_CONS,NO_MODE_CONS };
	U2 tree_status_child[3] = { TREE_LC, TREE_LC, TREE_LC };

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

	static IP_ARRAY_SIZE<8, 16>			IP_buffer_8x16;
	static IP_ARRAY_SIZE<16, 8>			IP_buffer_16x8;
	static IP_MAP_SCU_SIZE<8, 16>		IP_map_scu_8x16;
	static IP_MAP_SCU_SIZE<16, 8>		IP_map_scu_16x8;
	static MPM_ARRAY_SIZE<8, 16>		MPM_buffer_8x16;
	static MPM_ARRAY_SIZE<16, 8>		MPM_buffer_16x8;
	static ENC_ME_NEB_INFO				up_nebs[1 + (16 >> 2) + 1];
	static ENC_ME_NEB_INFO				left_nebs[(16 >> 2)];
	static ENC_ME_NEB_INFO				nebs_line_hor[16 >> 2];
	static ENC_ME_NEB_INFO				nebs_line_ver[16 >> 2];
	static ENC_ME_LINE_MAP_SIZE<16>		me_line_map_16;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16>	cu_data_temp_ptr_16;

#if ENABLE_BFRAME
	ME_MV_DIR_16 me_mv_dir_16;
#endif
	pel org_y_16[16][16];
	ENC_FME_INFO_ALL_16 fme_mv_info_16;
	strFetch_ref_window_16 Fetch_Ref_window_16;
	SKIP_MERGE_INPUT_16 skip_in_16;
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->coef_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->coef_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->reco_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->reco_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr_16->mvd cyclic factor=2 dim=3

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list, md_input_ptr->speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, ip_list);
#endif

	int x_pos_lcu = x0 & 63;
	int y_pos_lcu = y0 & 63;

	test_1(split_mode, up_nebs, left_nebs, up_nebs_father, left_nebs_father, me_line_map_ptr_16, &me_line_map_16, cu_width, 
		cu_height, part_num, &IP_buffer_8x16,&IP_buffer_16x8, &IP_map_scu_8x16, &IP_map_scu_16x8, IP_buffer_ptr_16x16,
		MPM_buffer_ptr_16x16, IP_map_scu_ptr_16x16, &MPM_buffer_8x16, &MPM_buffer_16x8, nebs_line_hor, nebs_line_ver);


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

	cost_best = md_kernel_16(md_fw_ptr, md_input_ptr, ip_list,
#if ENABLE_BFRAME
		& me_mv_dir_16,
#endif
		org_y_16,
		&me_line_map_16,
		up_nebs,
		left_nebs,
		&skip_in_16,
		boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2, cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&cu_data_temp_ptr_16,
		&fme_mv_info_16,
		IP_buffer_ptr_16x16, &IP_buffer_8x16, &IP_buffer_16x8,
		MPM_buffer_ptr_16x16, &MPM_buffer_8x16, &MPM_buffer_16x8,
		IP_map_scu_ptr_16x16, &IP_map_scu_8x16, &IP_map_scu_16x8,
		ref,
		&Fetch_Ref_window_16
#if SUB_TMVP
		, skip_in
#endif
	);
	test_2(split_mode,&me_line_map_16,cu_width,cu_height,part_num,&IP_buffer_8x16,
		&IP_buffer_16x8,&IP_map_scu_8x16,
		&IP_map_scu_16x8,
		IP_map_scu_ptr_16x16,
		&MPM_buffer_8x16,&MPM_buffer_16x8,
		nebs_line_hor,nebs_line_ver,
		cu_data_temp_ptr_16);
	

	prev_cu_data_ptr->get_cu_data_from_curblk(&cu_data_temp_ptr_16, (x0 - split_struct.x_pos[0]), (y0 - split_struct.y_pos[0]), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);

	//store_enc_info_best_8_16(md_fw_ptr, &cu_data_inner,
	//	x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_up_cu_data_temp_array_ptr,
	//	split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, part_num);

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}


U3 enc_eco_split_mode_est_16(U3 split_mode, MD_FW *md_fw_ptr, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s, U3 qt_depth, U3 bet_depth, U13 x, U13 y)
{
	U13 pic_width = md_fw_ptr->pic_width;
	U13 pic_height = md_fw_ptr->pic_height;

	U3 enc_est = 0;
	U1 split_allow[6];
	//int i;
	U3 non_QT_split_mode_num;
	U1 boundary = 0;
	U1 boundary_b = 0;
	U1 boundary_r = 0;
	if (cu_width == 1 << 2 && cu_height == 1 << 2) {
		return enc_est;
	}

	//com_get_split_mode(&split_mode, cud, cup, cu_width, cu_height, lcu_s, cu_data_temp_ptr->split_mode);
	boundary = !(x + cu_width <= pic_width && y + cu_height <= pic_height);
	boundary_b = boundary && y + cu_height > pic_height && !(x + cu_width > pic_width);
	boundary_r = boundary && x + cu_width > pic_width && !(y + cu_height > pic_height);
	com_check_split_mode_ECO(md_fw_ptr, split_allow, com_tbl_log2[cu_width], com_tbl_log2[cu_height],
		boundary, boundary_b, boundary_r, qt_depth, bet_depth);
	non_QT_split_mode_num = 0;
	non_QT_split_mode_num = split_allow[1] + split_allow[2] + split_allow[3] + split_allow[4];

//only QT is allowed
    if (split_allow[SPLIT_QUAD] && !(non_QT_split_mode_num || split_allow[NO_SPLIT])) {
        return enc_est;
    } else if (split_allow[SPLIT_QUAD]) {
        enc_est++;
        if (split_mode==SPLIT_QUAD) {
            return enc_est;
        }
    }
    if (non_QT_split_mode_num) {

//split flag

        if (split_allow[NO_SPLIT]) {
            enc_est++;
        } 
 
        if (split_mode!= NO_SPLIT) {
            int HBT = split_allow[SPLIT_BI_HOR];
			int VBT = split_allow[SPLIT_BI_VER];
			int EnableBT = HBT || VBT;
			int HEQT = split_allow[SPLIT_EQT_HOR];
			int VEQT = split_allow[SPLIT_EQT_VER];
			int EnableEQT = HEQT || VEQT;

			u8 split_typ = (split_mode== SPLIT_EQT_HOR || split_mode==SPLIT_EQT_VER);
			if (EnableEQT && EnableBT) {
				enc_est++;
			}
			if ((split_typ) == 0) {
				if (HBT && VBT) {
					enc_est++;
				}
			}
			if ((split_typ) == 1) {
				if (HEQT && VEQT) {
					enc_est++;
				}
			}
		}
	}
	return enc_est;
}


s64 prepare_cu_info_16(MD_FW*md_fw_ptr, s64 lambdaY, U13 x0, U13 y0,
	U12 cup, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT *split_struct)
{
#pragma HLS ARRAY_PARTITION variable=split_struct->split_allow complete dim=1
	md_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS PIPELINE
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

	if (x0 + cu_width <= md_fw_ptr->pic_width  && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		rate_bit += enc_eco_split_mode_est_16(split_mode, md_fw_ptr, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) {
			rate_bit += 1;
		}
		bit_cnt = rate_bit;

		cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}


s64 md_16x16_16x8_8x16_loop_md1(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH] ,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag, 
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, 
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16, 
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;
	U2 cons_pred_mode_BTH_copy[1];
	U2 child_tree_status_copy[1];
	cons_pred_mode_BTH_copy[0] = 0;
	child_tree_status_copy[0] = 0;
	if (isBTH_allow) {

		child_tree_status_copy[0] = (tree_status == TREE_LC ? TREE_LC : tree_status);

		bth_cost = prepare_cu_info_16(md_fw_ptr, md_input_ptr->lambda_y, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode, cons_pred_mode_BTH_copy, cu_width, cu_height, split_mode, cons_flag, &split_struct);
		clear_intermedia_map_scu<4, 4>(IP_map_scu_ptr_16x16->cur_map_scu);
		cons_pred_mode_BTH[0] = cons_pred_mode_BTH_copy[0];

	split_struct.part_count = 1;

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			bth_cost += circuit_16x16_16x8_8x16_md1(
				md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,   
				me_line_map_ptr_16,
				up_nebs_father, left_nebs_father,
				skip_in,
				cu_data_ptr,
				qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH_copy[0],
				child_tree_status_copy[0], part_num, split_struct, cu_width_log2, cu_height_log2, fme_mv_info,
				IP_buffer_ptr_16x16, 
				MPM_buffer_ptr_16x16, 
				IP_map_scu_ptr_16x16, 	
				ref,
				Fetch_Ref_window_ptr, split_mode);
		}
		child_tree_status[0] = child_tree_status_copy[0];
	}
	return bth_cost;
}

void load_16x16_top_md1(
	LCUConfig& config,
	InstructionPU& insn_pu, 
	Load2PU& in_buf, 
	Channel& channel, 
	LCUInput& buffer, 
	InstructionLoad& insn)
{
	
#if USE_SPEED_LEVEL
	//static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(&buffer.g_rmd_output, insn_pu.cu_width_log2, insn_pu.cu_height_log2, insn_pu.x0, insn_pu.y0, in_buf.ip_list, md_input_ptr->speed_level);
#else
	//static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(&buffer.g_rmd_output, insn_pu.cu_width_log2, insn_pu.cu_height_log2, insn_pu.x0, insn_pu.y0, in_buf.ip_list);
#endif

	int x_pos_lcu = insn_pu.x0 & 63;
	int y_pos_lcu = insn_pu.y0 & 63;
	S7 ip_array_size_idx;

	switch (channel.split_mode_father)
	{
	case SPLIT_BI_HOR:
		in_buf.IP_buffer_16x16.fetch_bi_hor(insn.part_num, &channel.IP_buffer_ptr_16x32);
		in_buf.IP_map_scu_16x16.fetch_bi_hor(insn.part_num, &channel.IP_map_scu_ptr_16x32);
		in_buf.MPM_buffer_16x16.fetch_bi_hor(insn.part_num, &channel.MPM_buffer_ptr_16x32);
		fetch_nebs_bi_hor(in_buf.up_nebs, in_buf.left_nebs, channel.up_nebs_32, channel.left_nebs_32, channel.nebs_line_hor, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);
		in_buf.me_line_map_16.fetch_bi_hor(insn.part_num, &channel.me_line_map_ptr_32, insn_pu.cu_width, insn_pu.cu_height);
		break;
	case SPLIT_BI_VER:
		in_buf.IP_buffer_16x16.fetch_bi_ver(insn.part_num, &channel.IP_buffer_ptr_32x16);
		in_buf.IP_map_scu_16x16.fetch_bi_ver(insn.part_num, &channel.IP_map_scu_ptr_32x16);
		in_buf.MPM_buffer_16x16.fetch_bi_ver(insn.part_num, &channel.MPM_buffer_ptr_32x16);
		fetch_nebs_bi_ver(in_buf.up_nebs, in_buf.left_nebs, channel.up_nebs_32, channel.left_nebs_32, channel.nebs_line_ver, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);
		in_buf.me_line_map_16.fetch_bi_ver(insn.part_num, &channel.me_line_map_ptr_32, insn_pu.cu_width, insn_pu.cu_height);
		break;
	default: assert(0);
	}


	fetch_port_16_md2_tst(
#if ENABLE_BFRAME
		&buffer.me_mv_dir,
#endif
		buffer.p_fenc_LCU_Y,
		//me_line_map_ptr,
		&buffer.skip_in,
		&buffer.fme_mv_info,
		&buffer.Fetch_Ref_window_ptr,
		insn_pu.x0, insn_pu.y0, insn_pu.cu_width, insn_pu.cu_height, config.md_fw_ptr.pic_width, config.md_fw_ptr.pic_height,
#if ENABLE_BFRAME
		&in_buf.me_mv_dir_16,
#endif
		in_buf.org_y_16,
		//&me_line_map_16,
		&in_buf.skip_in_16,
		&in_buf.fme_mv_info_16,
		&in_buf.Fetch_Ref_window_16);
}

s64 store_16x16_top_md1(
	LCUConfig& config, 
	PU2Store pu_out_buf[PU_NUM], 
	InstructionStore& insn, 
	Channel& channel)
{
	s64 cost_best = MAX_S64;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data_best;
	U3 best_split_mode[1] = { NO_SPLIT };
	U2 best_cons_pred_mode[1] = { NO_MODE_CONS };
	U2 best_tree_status_child[1] = { TREE_LC };
	U2 tree_status_temp[1] = { insn.tree_status };

	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_best_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY null_curr_cu_data_best_ptr2;
	compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[NO_SPLIT].cost_temp, &pu_out_buf[NO_SPLIT].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best, &null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2,
		insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
		NO_SPLIT, best_split_mode, best_cons_pred_mode, pu_out_buf[NO_SPLIT].cons_pred_mode, pu_out_buf[NO_SPLIT].tree_status_child, best_tree_status_child, tree_status_temp, 3);

	compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[SPLIT_BI_VER].cost_temp, &pu_out_buf[SPLIT_BI_VER].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best, &null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2,
		insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
		SPLIT_BI_VER, best_split_mode, best_cons_pred_mode, pu_out_buf[SPLIT_BI_VER].cons_pred_mode, pu_out_buf[SPLIT_BI_VER].tree_status_child, best_tree_status_child, tree_status_temp, 3);

	compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[SPLIT_BI_HOR].cost_temp, &pu_out_buf[SPLIT_BI_HOR].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best, &null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2,
		insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
		SPLIT_BI_HOR, best_split_mode, best_cons_pred_mode, pu_out_buf[SPLIT_BI_HOR].cons_pred_mode, pu_out_buf[SPLIT_BI_HOR].tree_status_child, best_tree_status_child, tree_status_temp, 3);

	switch (channel.split_mode_father)
	{
	case SPLIT_BI_HOR:
		channel.IP_buffer_16x16.update_bi_hor(insn.part_num, curr_cu_data_best.reco_y);
		channel.IP_map_scu_16x16.update_bi_hor(insn.part_num, channel.IP_map_scu_ptr_16x32.cur_map_scu, curr_cu_data_best.map_scu);
		channel.MPM_buffer_16x16.update_bi_hor(insn.part_num, curr_cu_data_best.ipm[0]);
		update_nebs_bi_hor(channel.nebs_line_hor, curr_cu_data_best.refi, curr_cu_data_best.mv, insn.cu_width, insn.cu_height, insn.part_num);
		channel.me_line_map_16.update_bi_hor(insn.part_num, curr_cu_data_best.map_scu, curr_cu_data_best.map_cu_mode, insn.cu_width, insn.cu_height);
		break;
	case SPLIT_BI_VER:
		channel.IP_buffer_16x16.update_bi_ver(insn.part_num, curr_cu_data_best.reco_y);
		channel.IP_map_scu_16x16.update_bi_ver(insn.part_num, channel.IP_map_scu_ptr_32x16.cur_map_scu, curr_cu_data_best.map_scu);
		channel.MPM_buffer_16x16.update_bi_ver(insn.part_num, curr_cu_data_best.ipm[0]);
		update_nebs_bi_ver(channel.nebs_line_ver, curr_cu_data_best.refi, curr_cu_data_best.mv, insn.cu_width, insn.cu_height, insn.part_num);
		channel.me_line_map_16.update_bi_ver(insn.part_num, curr_cu_data_best.map_scu, curr_cu_data_best.map_cu_mode, insn.cu_width, insn.cu_height);
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

	com_set_split_mode(best_split_mode[0], insn.cud, 0, insn.cu_width, insn.cu_height, insn.cu_width, split_mode);
	U9 pos = insn.cup + (((U9)(insn.cu_height >> 1 >> 2) << (com_tbl_log2[insn.cu_width] - 2)) + (insn.cu_width >> 1 >> 2));
	U3 shape = SQUARE + (com_tbl_log2[insn.cu_width] - com_tbl_log2[insn.cu_height]);
	if (insn.cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(insn.cu_width, insn.cu_height, best_split_mode[0], config.md_fw_ptr.slice_type))
	{
		com_set_cons_pred_mode(best_cons_pred_mode[0], insn.cud, 0, insn.cu_width, insn.cu_height, insn.cu_width, split_mode);
	}

	int x_cur_cu_scu = (insn.x0 - insn.x_pos) >> 2;
	int y_cur_cu_scu = (insn.y0 - insn.y_pos) >> 2;
	int cuh_scu = 1 << (insn.cu_height_log2 - 2);
	int cuw_scu = 1 << (insn.cu_width_log2 - 2);
#if STORE_ENC_INFO_BEST_16_TOP
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2	
#endif
	for (int i = 0; i < cuh_scu; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		int idx_dst = ((y_cur_cu_scu + i) << (insn.prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (insn.cu_width_log2 - 2);
		for (int j = 0; j < cuw_scu; j++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16	
#pragma HLS PIPELINE II=1	
			for (int m = 0; m < 5; m++)
			{
#pragma HLS LOOP_TRIPCOUNT max=5
				if (m >= insn.cud)
				{
					for (int n = 0; n < (NUM_BLOCK_SHAPE); n++)
					{
						(channel.prev_cu_data_ptr.split_mode[m][n] + idx_dst)[j] = (split_mode[m][n] + idx_src)[j];
					}
				}
			}
		}
	}

	channel.prev_cu_data_ptr.get_cu_data_from_curblk(&curr_cu_data_best, (insn.x0 - insn.x_pos), (insn.y0 - insn.y_pos), insn.cu_width_log2, insn.cu_height_log2, insn.prev_up_log2_sub_cuw);

	//copy_cu_data_array_dst_16_top(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
	//	cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status, *best_split_mode, shape,pos, split_mode);

	//update_map_scu_16_top(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
	return cost_best;
}

#if !ONLY_QT_QT
s64 circuit_16x16_top_md1(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	MPM_ARRAY_SIZE<16, 32>* MPM_buffer_ptr_16x32, MPM_ARRAY_SIZE<32, 16>* MPM_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16,
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
	U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
	//allowed split by normative and non-normative selection
	U3 num_split_to_try = split_struct.num_split[part_num];

	s64 cost[3] = {MAX_S64, MAX_S64, MAX_S64};
	s64 cost_temp[3] = {MAX_S64, MAX_S64, MAX_S64};
	U2 tree_status_child[3] = { TREE_LC };
	U2 cons_pred_mode_child[3] = { NO_MODE_CONS };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	//s64 cost_temp;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 NS_luma_pred_mode = ONLY_INTRA;
	U2 tree_status_temp = tree_status;
	U3 split_mode[3] = { NO_SPLIT, SPLIT_BI_VER, SPLIT_BI_HOR };

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	assert(cu_width == 16 && cu_height == 16);

	static IP_ARRAY_SIZE<16, 16>		IP_buffer_16x16;
	static IP_MAP_SCU_SIZE<16, 16>		IP_map_scu_16x16;
	static MPM_ARRAY_SIZE<16, 16>		MPM_buffer_16x16;
	static ENC_ME_NEB_INFO				up_nebs[1 + (16 >> 2) + 1];
	static ENC_ME_NEB_INFO				left_nebs[(16 >> 2)];
	static ENC_ME_NEB_INFO				nebs_line_ver[4];
	static ENC_ME_NEB_INFO				nebs_line_hor[4];
	static ENC_ME_LINE_MAP_SIZE<16>		me_line_map_16;
	static ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data[3];
	STATIC_FOR_STACK ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data_best;

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->mvd cyclic factor=2 dim=3	
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data->refi cyclic factor=2 dim=2	

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3	
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2		


	//for(int i = 0; i < 3; i++)
	//{
	//	if (tree_status == TREE_LC) {
	//		curr_cu_data[i].x_chroma = x0;
	//		curr_cu_data[i].y_chroma = y0;
	//		curr_cu_data[i].x_last = x0 + cu_width - 4;
	//		curr_cu_data[i].y_last = y0 + cu_height - 4;
	//		curr_cu_data[i].last_child = FALSE;
	//	}
	//	else {
	//		curr_cu_data[i].x_chroma = prev_cu_data_ptr->x_chroma;
	//		curr_cu_data[i].y_chroma = prev_cu_data_ptr->y_chroma;
	//		curr_cu_data[i].x_last = prev_cu_data_ptr->x_last;
	//		curr_cu_data[i].y_last = prev_cu_data_ptr->y_last;
	//		curr_cu_data[i].last_child = x0 + cu_width == prev_cu_data_ptr->x_last + 4 && y0 + cu_height == prev_cu_data_ptr->y_last + 4;
	//	}

	//}

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
#else
	static u8 ip_list[IPD_RDO_CNT];
#endif


#if ENABLE_BFRAME
	STATIC_C ME_MV_DIR_16 me_mv_dir_16;
#endif // ENABLE_BFRAME
	pel org_y_16[16][16];
	STATIC_C ENC_FME_INFO_ALL_16 fme_mv_info_16;
	STATIC_C strFetch_ref_window_16 Fetch_Ref_window_16;
	STATIC_C SKIP_MERGE_INPUT_16 skip_in_16;

#if 0
	load(ip_list, g_rmd_output, cu_width_log2, cu_height_log2, x0, y0, split_mode_father, IP_buffer_16x16,
	part_num, IP_buffer_ptr_16x32, IP_map_scu_16x16, IP_map_scu_ptr_16x32,
	MPM_buffer_16x16, MPM_buffer_ptr_16x32, up_nebs, left_nebs,
	up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height,
	me_line_map_16, me_line_map_ptr_32, IP_buffer_ptr_32x16, IP_map_scu_ptr_32x16,
	MPM_buffer_ptr_32x16, nebs_line_ver, me_mv_dir, p_fenc_LCU_Y, skip_in,
	fme_mv_info, Fetch_Ref_window_ptr, md_fw_ptr, me_mv_dir_16, org_y_16, fme_mv_info_16,
	Fetch_Ref_window_16, skip_in_16);
#else
	STATIC_C InstructionPU insn_pu;
	STATIC_C Load2PU in_buf;
	STATIC_C Channel channel;
	STATIC_C ShareMemory pool;
	STATIC_C LCUConfig config;
	STATIC_C LCUInput buffer; 
	STATIC_C InstructionLoad insn_load;
	insn_pu.cu_width_log2 = cu_width_log2;
	insn_pu.cu_height_log2 = cu_height_log2;
	insn_pu.x0 = x0;
	insn_pu.y0 = y0;
	insn_pu.cu_width = cu_width;
	insn_pu.cu_height = cu_height;
	insn_load.part_num = part_num;

	channel.split_mode_father = split_mode_father;
	channel.IP_buffer_ptr_16x32 = *IP_buffer_ptr_16x32;
	channel.IP_buffer_ptr_32x16 = *IP_buffer_ptr_32x16;

	channel.IP_map_scu_ptr_16x32 = *IP_map_scu_ptr_16x32;
	channel.IP_map_scu_ptr_32x16 = *IP_map_scu_ptr_32x16;

	channel.MPM_buffer_ptr_16x32 = *MPM_buffer_ptr_16x32;
	channel.MPM_buffer_ptr_32x16 = *MPM_buffer_ptr_32x16;
	

	for(size_t i = 0; i < 10; i++)
	{
        channel.up_nebs_32[i] = up_nebs_father[i];
	}
	channel.me_line_map_ptr_32 = *me_line_map_ptr_32;
	for(size_t i = 0; i < 8; i++)
	{
	    channel.left_nebs_32[i] = left_nebs_father[i];
	}

	for(size_t i = 0; i < 4; i++)
	{
	    channel.nebs_line_hor[i] = nebs_line_hor[i];
	    channel.nebs_line_ver[i] = nebs_line_ver[i];		
	}
	buffer.g_rmd_output = *g_rmd_output;
	buffer.me_mv_dir = *me_mv_dir;
	for(size_t i = 0; i < MAX_CU_SIZE_FETCH; i++)
	{
		for(size_t j = 0; j < MAX_CU_SIZE_FETCH; j++)
		{
			buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
		}
	}

	buffer.skip_in = *skip_in;
	in_buf.IP_buffer_16x16 = IP_buffer_16x16;
	in_buf.IP_map_scu_16x16 = IP_map_scu_16x16;
	in_buf.MPM_buffer_16x16 = MPM_buffer_16x16;
	in_buf.me_line_map_16 = me_line_map_16;
	in_buf.me_mv_dir_16 = me_mv_dir_16;
	in_buf.fme_mv_info_16 = fme_mv_info_16;
	in_buf.Fetch_Ref_window_16 = Fetch_Ref_window_16;
	in_buf.skip_in_16 = skip_in_16;

#if USE_SPEED_LEVEL
	for(size_t i = 0; i < (IPD_RDO_CNT + EXTRA_RDO_MODE); i++)
#else
	for(size_t i = 0; i < IPD_RDO_CNT; i++)
#endif	
	{
		in_buf.ip_list[i] = ip_list[i];
	}

	for(size_t i = 0; i < 16; i++)
	{
		for(size_t j = 0; j < 16; j++)
		{
			in_buf.org_y_16[i][j] = org_y_16[i][j];
		}
	}


	buffer.fme_mv_info = *fme_mv_info;
	buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
	config.md_fw_ptr = *md_fw_ptr;

	load_16x16_top_md1(config, insn_pu, in_buf, channel, buffer, insn_load);

	IP_buffer_16x16 = in_buf.IP_buffer_16x16;
	IP_map_scu_16x16 = in_buf.IP_map_scu_16x16;
	MPM_buffer_16x16 = in_buf.MPM_buffer_16x16;
	me_line_map_16 = in_buf.me_line_map_16;
	me_mv_dir_16 = in_buf.me_mv_dir_16;

#if USE_SPEED_LEVEL
	for(size_t i = 0; i < (IPD_RDO_CNT + EXTRA_RDO_MODE); i++)
#else
	for(size_t i = 0; i < IPD_RDO_CNT; i++)
#endif	
	{
		ip_list[i] = in_buf.ip_list[i];
	}

	for(size_t i = 0; i < 16; i++)
	{
		for(size_t j = 0; j < 16; j++)
		{
			org_y_16[i][j] = in_buf.org_y_16[i][j];
		}
	}

	fme_mv_info_16 = in_buf.fme_mv_info_16;
	Fetch_Ref_window_16 = in_buf.Fetch_Ref_window_16;
	skip_in_16 = in_buf.skip_in_16;

	for(size_t i = 0; i < 6; i++)
	{
		up_nebs[i] = in_buf.up_nebs[i];
	}
	for(size_t i = 0; i < 4; i++)
	{
		left_nebs[i] = in_buf.left_nebs[i];
	}
#endif

	static Load2PU pu_in_buf[PU_NUM];
	static PU2Store pu_out_buf[PU_NUM];
#if !KERNEL_SIZE_CLOSE(16, 16)
	if (split_struct.split_allow[part_num][NO_SPLIT])
	{
		pu_in_buf[0].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[0].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[0].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[0].buffer.skip_in = *skip_in;
		pu_in_buf[0].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[0].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
		pu_in_buf[0].insn.cud = cud;
		pu_in_buf[0].insn.cup = cup;
		pu_in_buf[0].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[0].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[0].insn.cu_width = cu_width;
		pu_in_buf[0].insn.cu_height = cu_height;
		pu_in_buf[0].insn.qt_depth = qt_depth;
		pu_in_buf[0].insn.x0 = x0;
		pu_in_buf[0].insn.y0 = y0;
		pu_in_buf[0].insn.qt_depth = qt_depth;
		pu_in_buf[0].insn.bet_depth = bet_depth;
		pu_in_buf[0].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[0].insn.tree_status = tree_status;

		pu_in_buf[0].IP_buffer_16x16 = IP_buffer_16x16;
		pu_in_buf[0].MPM_buffer_16x16 = MPM_buffer_16x16;
		pu_in_buf[0].IP_map_scu_16x16 = IP_map_scu_16x16;

#if USE_SPEED_LEVEL
		for(size_t i = 0; i < (IPD_RDO_CNT + EXTRA_RDO_MODE); i++)
#else
		for(size_t i = 0; i < IPD_RDO_CNT; i++)
#endif	
		{
			pu_in_buf[0].ip_list[i] = ip_list[i];
		}
				
		pu_in_buf[0].me_mv_dir_16 = me_mv_dir_16;
		for(size_t i = 0; i < 16; i++)
		{
			for(size_t j = 0; j < 16; j++)
			{
				pu_in_buf[0].org_y_16[i][j] = org_y_16[i][j];
			}
		}		
		pu_in_buf[0].me_line_map_16 = me_line_map_16;
		for(size_t i = 0; i < 6; i++)
		{
			pu_in_buf[0].up_nebs[i] = up_nebs[i];
		}
		for(size_t i = 0; i < 4; i++)
		{
			pu_in_buf[0].left_nebs[i] = left_nebs[i];
		}
		pu_in_buf[0].skip_in_16 = skip_in_16;
		pu_in_buf[0].boundary = boundary;
		pu_in_buf[0].fme_mv_info_16 = fme_mv_info_16;
		pu_in_buf[0].Fetch_Ref_window_16 = Fetch_Ref_window_16;

		#if 0
		static IP_ARRAY_SIZE<8, 16>			IP_buffer_8x16;
		static IP_ARRAY_SIZE<16, 8>			IP_buffer_16x8;
		static IP_MAP_SCU_SIZE<8, 16>		IP_map_scu_8x16;
		static IP_MAP_SCU_SIZE<16, 8>		IP_map_scu_16x8;
		static MPM_ARRAY_SIZE<8, 16>		MPM_buffer_8x16;
		static MPM_ARRAY_SIZE<16, 8>		MPM_buffer_16x8;		
		pu_test_0(pu_out_buf[0].cost_temp, NO_SPLIT,
				&pu_in_buf[0].config.md_fw_ptr, &pu_in_buf[0].config.md_input_ptr, pu_in_buf[0].ip_list,
#if ENABLE_BFRAME
				pu_in_buf[0].me_mv_dir_16,
#endif
				pu_in_buf[0].org_y_16,
				pu_in_buf[0].me_line_map_16, pu_in_buf[0].up_nebs,
				pu_in_buf[0].left_nebs,
				pu_in_buf[0].skip_in_16,
				pu_in_buf[0].boundary, split_struct.split_allow[part_num][NO_SPLIT], NS_luma_pred_mode, pu_in_buf[0].insn.x0, pu_in_buf[0].insn.y0, 
				pu_in_buf[0].insn.cup, pu_in_buf[0].insn.cu_width_log2, pu_in_buf[0].insn.cu_height_log2,
				pu_in_buf[0].insn.cud, pu_in_buf[0].insn.qt_depth, pu_in_buf[0].insn.bet_depth, pu_in_buf[0].insn.cons_pred_mode, pu_in_buf[0].insn.tree_status,
				pu_out_buf[0].curr_cu_data_temp,
				pu_in_buf[0].fme_mv_info_16,
				pu_in_buf[0].IP_buffer_16x16, IP_buffer_8x16, IP_buffer_16x8,
				pu_in_buf[0].MPM_buffer_16x16, MPM_buffer_8x16, MPM_buffer_16x8,
				pu_in_buf[0].IP_map_scu_16x16, IP_map_scu_8x16, IP_map_scu_16x8,
				ref,
				pu_in_buf[0].Fetch_Ref_window_16
#if SUB_TMVP
				, &pu_in_buf[0].buffer.skip_in
#endif
				);
		#else
		pu_16x16_md2(pu_in_buf[0], pu_out_buf[0], ref);
		me_line_map_16 = pu_in_buf[0].me_line_map_16;
		for(size_t i = 0; i < 6; i++)
		{
			up_nebs[i] = pu_in_buf[0].up_nebs[i];
		}
		for(size_t i = 0; i < 4; i++)
		{
			left_nebs[i] = pu_in_buf[0].left_nebs[i];
		}		
		#endif
		cost_temp[0] = pu_out_buf[0].cost_temp;
		curr_cu_data[0] = pu_out_buf[0].curr_cu_data_temp;
	}
#else
	cost_temp[0] = MAX_S64;
#endif // !KERNEL_SIZE_CLOSE(16, 16)

	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_best_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY null_curr_cu_data_best_ptr2;

#if !size_8x16_md1_top_bt_close
	if (split_struct.split_allow[part_num][SPLIT_BI_VER])
	{
		pu_in_buf[1].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[1].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[1].buffer.g_rmd_output = *g_rmd_output;
		//pu_in_buf[1].buffer.RefWindowLCU_UP[(SWH * SWW) >> 2] = RefWindowLCU_UP[(SWH * SWW) >> 2];
		//pu_in_buf[1].buffer.RefWindowLCU_VP[(SWH * SWW) >> 2] = RefWindowLCU_VP[(SWH * SWW) >> 2];
		//pu_in_buf[1].buffer.CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS] = CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
		//pu_in_buf[1].buffer.RefWindowLCU_UP_ref1[(SWH * SWW) >> 2] = RefWindowLCU_UP_ref1[(SWH * SWW) >> 2];
		//pu_in_buf[1].buffer.RefWindowLCU_VP_ref1[(SWH * SWW) >> 2] = RefWindowLCU_VP_ref1[(SWH * SWW) >> 2];
		//pu_in_buf[1].buffer.CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS] = CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
		int i, j;
		for (i = 0; i < 64; i = i + 1)
		{
			for (j = 0; j < 64; j = j + 1)
			{
				pu_in_buf[1].buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
			}

		};

		pu_in_buf[1].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[1].buffer.skip_in = *skip_in;
		pu_in_buf[1].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[1].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
		pu_in_buf[1].me_line_map_16 = me_line_map_16;
		pu_in_buf[1].insn.cud = cud;
		pu_in_buf[1].insn.cup = cup;
		pu_in_buf[1].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[1].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[1].insn.cu_width = cu_width;
		pu_in_buf[1].insn.cu_height = cu_height;
		pu_in_buf[1].insn.qt_depth = qt_depth;
		pu_in_buf[1].insn.x0 = x0;
		pu_in_buf[1].insn.y0 = y0;
		pu_in_buf[1].insn.qt_depth = qt_depth;
		pu_in_buf[1].insn.bet_depth = bet_depth;
		pu_in_buf[1].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[1].insn.tree_status = tree_status;
		pu_in_buf[1].IP_buffer_16x16 = IP_buffer_16x16;
		pu_in_buf[1].MPM_buffer_16x16 = MPM_buffer_16x16;
		pu_in_buf[1].IP_map_scu_16x16 = IP_map_scu_16x16;

		#if 0
		pu_1(pu_in_buf[1], pu_out_buf[1], ref, split_struct, part_num, up_nebs, left_nebs, SPLIT_BI_VER);
		#else
		pu_in_buf[1].part_num = part_num;
		for(size_t i = 0; i < 6; i++)
		{
			pu_in_buf[1].up_nebs[i] = up_nebs[i];
		}
		for(size_t i = 0; i < 4; i++)
		{
			pu_in_buf[1].left_nebs[i] = left_nebs[i];
		}		
		pu_16x16_8x16_md2(pu_in_buf[1], pu_out_buf[1], ref);
		part_num = pu_in_buf[1].part_num;
		for(size_t i = 0; i < 6; i++)
		{
			up_nebs[i] = pu_in_buf[1].up_nebs[i];
		}
		for(size_t i = 0; i < 4; i++)
		{
			left_nebs[i] = pu_in_buf[1].left_nebs[i];
		}		
		#endif
		cost_temp[1] = pu_out_buf[1].cost_temp;
		curr_cu_data[1] = pu_out_buf[1].curr_cu_data_temp;
		cons_pred_mode_child[1] = pu_out_buf[1].cons_pred_mode;
		tree_status_child[1] = pu_out_buf[1].tree_status_child;
#if KERNEL_SIZE_CLOSE(8, 16)
		cost_temp[1] = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(8, 16)
		/*pu_test_1(pu_out_buf[1].cost_temp,SPLIT_BI_VER,
			md_fw_ptr, md_input_ptr, g_rmd_output,
			RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
			RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
			pu_in_buf[1].buffer.p_fenc_LCU_Y,
			me_line_map_16,
			up_nebs, left_nebs,
			skip_in, split_struct.split_allow[part_num][SPLIT_BI_VER], x0, y0, cup, cu_width_log2, cu_height_log2, cud,
			qt_depth, bet_depth, cons_pred_mode,
			cu_width, cu_height, NS_luma_pred_mode,
			pu_out_buf[1].curr_cu_data_temp,
			pu_out_buf[1].cons_pred_mode, tree_status, pu_out_buf[1].tree_status_child,
			fme_mv_info,
			IP_buffer_16x16,
			MPM_buffer_16x16,
			IP_map_scu_16x16,
			ref,
			Fetch_Ref_window_ptr);*/

	}
#else
	cost_temp[1] = MAX_S64;
#endif // !size_8x16_md1_top_bt_close

#if !size_16x8_md1_top_bt_close
	if (split_struct.split_allow[part_num][SPLIT_BI_HOR])
	{
		pu_in_buf[2].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[2].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[2].buffer.g_rmd_output = *g_rmd_output;
		//pu_in_buf[2].buffer.RefWindowLCU_UP[(SWH * SWW) >> 2] = RefWindowLCU_UP[(SWH * SWW) >> 2];
		//pu_in_buf[2].buffer.RefWindowLCU_VP[(SWH * SWW) >> 2] = RefWindowLCU_VP[(SWH * SWW) >> 2];
		//pu_in_buf[2].buffer.CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS] = CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
		//pu_in_buf[2].buffer.RefWindowLCU_UP_ref1[(SWH * SWW) >> 2] = RefWindowLCU_UP_ref1[(SWH * SWW) >> 2];
		//pu_in_buf[2].buffer.RefWindowLCU_VP_ref1[(SWH * SWW) >> 2] = RefWindowLCU_VP_ref1[(SWH * SWW) >> 2];
		//pu_in_buf[2].buffer.CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS] = CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
		int i, j;
		for (i = 0; i < 64; i = i + 1) 
		{	
			for (j = 0; j < 64; j = j + 1) 
			{
				pu_in_buf[2].buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
			}
			
		
		};
		pu_in_buf[2].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[2].buffer.skip_in = *skip_in;
		pu_in_buf[2].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[2].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;
		pu_in_buf[2].me_line_map_16 = me_line_map_16;
		pu_in_buf[2].insn.cud = cud;
		pu_in_buf[2].insn.cup = cup;
		pu_in_buf[2].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[2].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[2].insn.cu_width = cu_width;
		pu_in_buf[2].insn.cu_height = cu_height;
		pu_in_buf[2].insn.qt_depth = qt_depth;
		pu_in_buf[2].insn.x0 = x0;
		pu_in_buf[2].insn.y0 = y0;
		pu_in_buf[2].insn.qt_depth = qt_depth;
		pu_in_buf[2].insn.bet_depth = bet_depth;
		pu_in_buf[2].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[2].insn.tree_status = tree_status;
		pu_in_buf[2].IP_buffer_16x16 = IP_buffer_16x16;
		pu_in_buf[2].MPM_buffer_16x16 = MPM_buffer_16x16;
		pu_in_buf[2].IP_map_scu_16x16 = IP_map_scu_16x16;

		#if 0	
		pu_1(pu_in_buf[2], pu_out_buf[2], ref, split_struct, part_num, up_nebs, left_nebs, SPLIT_BI_HOR);
		#else
		pu_in_buf[2].part_num = part_num;
		for(size_t i = 0; i < 6; i++)
		{
			pu_in_buf[2].up_nebs[i] = up_nebs[i];
		}
		for(size_t i = 0; i < 4; i++)
		{
			pu_in_buf[2].left_nebs[i] = left_nebs[i];
		}			
		pu_16x16_16x8_md2(pu_in_buf[2], pu_out_buf[2], ref);
		part_num = pu_in_buf[2].part_num;
		for(size_t i = 0; i < 6; i++)
		{
			up_nebs[i] = pu_in_buf[2].up_nebs[i];
		}
		for(size_t i = 0; i < 4; i++)
		{
			left_nebs[i] = pu_in_buf[2].left_nebs[i];
		}			
		#endif
		cost_temp[2] = pu_out_buf[2].cost_temp;
		curr_cu_data[2] = pu_out_buf[2].curr_cu_data_temp;
		cons_pred_mode_child[2] = pu_out_buf[2].cons_pred_mode;
		tree_status_child[2] = pu_out_buf[2].tree_status_child;
#if KERNEL_SIZE_CLOSE(16, 8)
		cost_temp[2] = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(8, 16)
		/*pu_test_1(cost_temp[2], SPLIT_BI_HOR,
			md_fw_ptr, md_input_ptr, g_rmd_output,
			RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
			RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
			p_fenc_LCU_Y,
			me_line_map_16,
			up_nebs, left_nebs,
			skip_in, split_struct.split_allow[part_num][SPLIT_BI_HOR], x0, y0, cup, cu_width_log2, cu_height_log2, cud, 
			qt_depth, bet_depth, cons_pred_mode, 
			cu_width, cu_height, NS_luma_pred_mode,
			curr_cu_data[2],
			cons_pred_mode_child[2], tree_status, tree_status_child[2], 
			fme_mv_info,
			IP_buffer_16x16,
			MPM_buffer_16x16,
			IP_map_scu_16x16,
			ref,
			Fetch_Ref_window_ptr);*/

	}
#else
	cost_temp[2] = MAX_S64;
#endif // !size_16x8_md1_top_bt_close

#if 0
	store(md_fw_ptr, cost_best, cost_temp[0], cost_temp[1], cost_temp[2], curr_cu_data[0], curr_cu_data[1], curr_cu_data[2], curr_cu_data_best,
		cu_width_log2, cu_height_log2, cud, tree_status, best_split_mode, best_cons_pred_mode, cons_pred_mode_child[0], cons_pred_mode_child[1],
		cons_pred_mode_child[2], tree_status_child[0], tree_status_child[1], tree_status_child[2], best_tree_status_child, tree_status_temp,
		split_mode_father, IP_buffer_16x16, IP_map_scu_16x16, MPM_buffer_16x16, nebs_line_ver, nebs_line_hor, me_line_map_16, part_num,
		cu_width, cu_height, IP_map_scu_ptr_16x32, IP_map_scu_ptr_32x16,
		x0, y0, cup, cons_pred_mode, num_split_to_try, prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw, &best_split_mode, &curr_cu_data_best);
#else
	STATIC_C InstructionStore insn_store; 
	config.md_fw_ptr = *md_fw_ptr;
	
	pu_out_buf[NO_SPLIT].cost_temp = cost_temp[0];
	pu_out_buf[SPLIT_BI_VER].cost_temp = cost_temp[1];
	pu_out_buf[SPLIT_BI_HOR].cost_temp = cost_temp[2];

	pu_out_buf[NO_SPLIT].curr_cu_data_temp = curr_cu_data[0];
	pu_out_buf[SPLIT_BI_VER].curr_cu_data_temp = curr_cu_data[1];
	pu_out_buf[SPLIT_BI_HOR].curr_cu_data_temp = curr_cu_data[2];

	pu_out_buf[NO_SPLIT].cons_pred_mode = cons_pred_mode_child[0];
	pu_out_buf[SPLIT_BI_VER].cons_pred_mode = cons_pred_mode_child[1];
	pu_out_buf[SPLIT_BI_HOR].cons_pred_mode = cons_pred_mode_child[2];

	pu_out_buf[NO_SPLIT].tree_status_child = tree_status_child[0];
	pu_out_buf[SPLIT_BI_VER].tree_status_child = tree_status_child[1];
	pu_out_buf[SPLIT_BI_HOR].tree_status_child = tree_status_child[2];

	insn_store.cu_width_log2 = cu_width_log2;
	insn_store.cu_height_log2 = cu_height_log2;
	insn_store.cud = cud;
	insn_store.tree_status = tree_status;

	channel.IP_buffer_16x16 = IP_buffer_16x16;
	channel.IP_map_scu_16x16 = IP_map_scu_16x16;
	channel.MPM_buffer_16x16 = MPM_buffer_16x16;

	for(size_t i = 0; i < 4; i++)
	{
	    channel.nebs_line_hor[i] = nebs_line_hor[i];
	    channel.nebs_line_ver[i] = nebs_line_ver[i];		
	}

	channel.me_line_map_16 = me_line_map_16;
	channel.IP_map_scu_ptr_16x32 = *IP_map_scu_ptr_16x32;
	channel.IP_map_scu_ptr_32x16 = *IP_map_scu_ptr_32x16;
	channel.prev_cu_data_ptr = *prev_cu_data_ptr;
	channel.split_mode_father = split_mode_father;

	insn_store.part_num = part_num;
	insn_store.cu_width = cu_width;
	insn_store.cu_height = cu_height;
	insn_store.x0 = x0;
	insn_store.y0 = y0;
	insn_store.cup = cup;
	insn_store.x_pos = split_struct.x_pos[0];
	insn_store.y_pos = split_struct.y_pos[0];
	insn_store.prev_up_log2_sub_cuw = prev_up_log2_sub_cuw;
	insn_store.cons_pred_mode = cons_pred_mode;

	cost_best = store_16x16_top_md1(config, pu_out_buf, insn_store, channel);

	IP_buffer_16x16 = channel.IP_buffer_16x16;
	IP_map_scu_16x16 = channel.IP_map_scu_16x16;
	MPM_buffer_16x16 = channel.MPM_buffer_16x16;

	for(size_t i = 0; i < 4; i++)
	{
	    nebs_line_hor[i] = channel.nebs_line_hor[i];
	    nebs_line_ver[i] = channel.nebs_line_ver[i];		
	}

	me_line_map_16 = channel.me_line_map_16;
	*IP_map_scu_ptr_16x32 = channel.IP_map_scu_ptr_16x32;
	*IP_map_scu_ptr_32x16 = channel.IP_map_scu_ptr_32x16;
	*prev_cu_data_ptr = channel.prev_cu_data_ptr;
#endif

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif
	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}
#endif

int copy_cu_data_array_dst_16_top(ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY_INNER* src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status, U3 best_split_mode,
	U3 shape, U9 pos, U8 split_mode[5][7][256])
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
		for (k = cud; k < 5; k++) {
#pragma HLS LOOP_TRIPCOUNT max=5
			for (i = 0; i < (NUM_BLOCK_SHAPE); i++) {
				for (ii = 0; ii < cuw_scu; ii++) {
#pragma HLS LOOP_TRIPCOUNT max=16
					(dst->split_mode[k][i] + idx_dst)[ii] = (split_mode[k][i] + idx_src)[ii];
					}
			}
		}
	}

	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = ((U22)(cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);

		for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->pred_mode + idx_dst)[i] = (src->pred_mode + idx_src)[i];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
			(dst->mpm[0] + idx_dst)[i] = (src->mpm[0] + idx_src)[i];
			(dst->mpm[1] + idx_dst)[i] = (src->mpm[1] + idx_src)[i];
			(dst->ipm[0] + idx_dst)[i] = (src->ipm[0] + idx_src)[i];
			//(dst->ipm[1] + idx_dst)[i] = (src->ipm[1] + idx_src)[i];
		}

		for (i = 0; i < 8; i++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->mpm_ext[i] + idx_dst)[i1] = (src->mpm_ext[i] + idx_src)[i1];
			}

		}
		for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->affine_flag + idx_dst)[i] = (src->affine_flag + idx_src)[i];
			(dst->smvd_flag + idx_dst)[i] = (src->smvd_flag + idx_src)[i];
			(dst->depth + idx_dst)[i] = (src->depth + idx_src)[i];
			(dst->map_scu + idx_dst)[i] = (src->map_scu + idx_src)[i];
			(dst->map_cu_mode + idx_dst)[i] = (src->map_cu_mode + idx_src)[i];
			(dst->map_pb_tb_part + idx_dst)[i] = (src->map_pb_tb_part + idx_src)[i];

			for (i1 = 0; i1 < 2; i1++)
				(dst->refi + idx_dst)[i][i1] = (src->refi + idx_src)[i][i1];

			(dst->umve_flag + idx_dst)[i] = (src->umve_flag + idx_src)[i];
			(dst->umve_idx + idx_dst)[i] = (src->umve_idx + idx_src)[i];
			(dst->skip_idx + idx_dst)[i] = (src->skip_idx + idx_src)[i];
			(dst->mvr_idx + idx_dst)[i] = (src->mvr_idx + idx_src)[i];
			(dst->mvp_from_hmvp_flag + idx_dst)[i] = (src->mvp_from_hmvp_flag + idx_src)[i];
			(dst->ipf_flag + idx_dst)[i] = (src->ipf_flag + idx_src)[i];
			for (i1 = 0; i1 < 2; i1++) {
				for (i2 = 0; i2 < 2; i2++) {
					(dst->mv + idx_dst)[i][i1][i2] = (src->mv + idx_src)[i][i1][i2];
					(dst->mvd + idx_dst)[i][i1][i2] = (src->mvd + idx_src)[i][i1][i2];
				}
			}

		}

		for (k = 0; k < 1; k++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->num_nz_coef[k] + idx_dst)[i1] = (src->num_nz_coef[k] + idx_src)[i1];
			}
		}

		for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->pb_part + idx_dst)[i1] = (src->pb_part + idx_src)[i1];
			(dst->tb_part + idx_dst)[i1] = (src->tb_part + idx_src)[i1];
		}

	}


	for (j = 0; j < cu_height; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
		idx_dst = ((U22)(y + j) << log2_cus) + x;
		idx_src = (U22)j << cu_width_log2;

		for (i = 0; i < cu_width; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
			dst->coef_y[y + j][x + i] = src->coef_y[j][i];
			dst->reco_y[y + j][x + i] = src->reco_y[j][i];
		}
	}

	return 0;
}

int copy_cu_data_array_dst_16_top2(ENC_CU_DATA_ARRAY_INNER* dst, ENC_CU_DATA_ARRAY_INNER* src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status, U3 best_split_mode,
	U3 shape, U9 pos, U8 split_mode[5][7][256])
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
			(dst->pred_mode + idx_dst)[i] = (src->pred_mode + idx_src)[i];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[i] = (src->cu_luma_qp + idx_src)[i];
#endif
			(dst->mpm[0] + idx_dst)[i] = (src->mpm[0] + idx_src)[i];
			(dst->mpm[1] + idx_dst)[i] = (src->mpm[1] + idx_src)[i];
			(dst->ipm[0] + idx_dst)[i] = (src->ipm[0] + idx_src)[i];
		    //(dst->ipm[1] + idx_dst)[i] = (src->ipm[1] + idx_src)[i];
		}

		for (i = 0; i < 8; i++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->mpm_ext[i] + idx_dst)[i1] = (src->mpm_ext[i] + idx_src)[i1];
			}

		}
		for (i = 0; i < cuw_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->affine_flag + idx_dst)[i] = (src->affine_flag + idx_src)[i];
			(dst->smvd_flag + idx_dst)[i] = (src->smvd_flag + idx_src)[i];
			(dst->depth + idx_dst)[i] = (src->depth + idx_src)[i];
			(dst->map_scu + idx_dst)[i] = (src->map_scu + idx_src)[i];
			(dst->map_cu_mode + idx_dst)[i] = (src->map_cu_mode + idx_src)[i];
			(dst->map_pb_tb_part + idx_dst)[i] = (src->map_pb_tb_part + idx_src)[i];

			for (i1 = 0; i1 < 2; i1++)
				(dst->refi + idx_dst)[i][i1] = (src->refi + idx_src)[i][i1];

			(dst->umve_flag + idx_dst)[i] = (src->umve_flag + idx_src)[i];
			(dst->umve_idx + idx_dst)[i] = (src->umve_idx + idx_src)[i];
			(dst->skip_idx + idx_dst)[i] = (src->skip_idx + idx_src)[i];
			(dst->mvr_idx + idx_dst)[i] = (src->mvr_idx + idx_src)[i];
			(dst->mvp_from_hmvp_flag + idx_dst)[i] = (src->mvp_from_hmvp_flag + idx_src)[i];
			(dst->ipf_flag + idx_dst)[i] = (src->ipf_flag + idx_src)[i];
			for (i1 = 0; i1 < 2; i1++) {
				for (i2 = 0; i2 < 2; i2++) {
					(dst->mv + idx_dst)[i][i1][i2] = (src->mv + idx_src)[i][i1][i2];
					(dst->mvd + idx_dst)[i][i1][i2] = (src->mvd + idx_src)[i][i1][i2];
				}
			}

		}

		for (k = 0; k < 1; k++) {
			for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->num_nz_coef[k] + idx_dst)[i1] = (src->num_nz_coef[k] + idx_src)[i1];
			}
		}

		for (i1 = 0; i1 < cuw_scu; i1++) {
#pragma HLS LOOP_TRIPCOUNT max=16
			(dst->pb_part + idx_dst)[i1] = (src->pb_part + idx_src)[i1];
			(dst->tb_part + idx_dst)[i1] = (src->tb_part + idx_src)[i1];
		}

	}


	for (j = 0; j < cu_height; j++) {
#pragma HLS LOOP_TRIPCOUNT max=64
		idx_dst = ((U22)(y + j) << log2_cus) + x;
		idx_src = (U22)j << cu_width_log2;

		for (i = 0; i < cu_width; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
			dst->coef_y[y +j][x + i] = src->coef_y[j][i];
			dst->reco_y[y +j][x + i] = src->reco_y[j][i];
		}
	}

	return 0;
}


void update_map_scu_16_top(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best_ptr, U13 x, U13 y, U8 src_cuw, U8 src_cuh,
	U2 tree_status)
{
	//s8 *map_ipm = 0;
	S8* src_map_ipm = 0;
	//int size_depth;
	U6 w;
	U6 h;
	u16 i;
	/*int size;
	int size_ipm;
	int size_mv;
	int size_refi;*/
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
	/*size = (sizeof(u32) * w);
	size_ipm = (sizeof(u8) * w);
	size_mv = (sizeof(s16) * w * 2 * 2);
	size_refi = (sizeof(s8) * w* 2);
	size_depth = (sizeof(s8)* w);*/
	mpm_x = scu_x & 15;
	mpm_y = scu_y & 15;
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (ii = 0; ii < w; ii++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MPM_buffer_ptr->buffer[i + mpm_y][ii + mpm_x] = (src_map_ipm[(i << (log2_src_cuw - 2)) + ii]);
//		}
//	}
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		src_map_ipm += src_cuw >> 2;
	}
	//x_scu_line = scu_x - me_line_map_ptr->min_scu_x;
	//y_scu_line = scu_y - me_line_map_ptr->min_scu_y;
	j = 0;
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (j = 0; j < w; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i]
//				= curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
//			me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i] = curr_cu_data_best_ptr->map_cu_mode[j + (i << (log2_src_cuw - 2))];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].refi[0] = curr_cu_data_best_ptr->refi[j + (i << (log2_src_cuw - 2))][0];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].refi[1] = curr_cu_data_best_ptr->refi[j + (i << (log2_src_cuw - 2))][1];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[0][0] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][0][0];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[0][1] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][0][1];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[1][0] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][1][0];
//			inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].mv[1][1] = curr_cu_data_best_ptr->mv[j + (i << (log2_src_cuw - 2))][1][1];
//			/*inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line +
//				i].pb_tb_part = curr_cu_data_best_ptr->map_pb_tb_part[j + (i << (log2_src_cuw - 2))];*/
//		}
//	}
//	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//		for (j = 0; j < w; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
//			MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i] =
//				curr_cu_data_best_ptr->map_scu[j + (i << (log2_src_cuw - 2))];
//		}
//	}
}

void store_enc_info_best_16_top(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
	U8 split_mode[5][7][256];
	#pragma HLS ARRAY_PARTITION variable=split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=split_mode cyclic factor=7 dim=2
	#pragma HLS ARRAY_PARTITION variable=split_mode cyclic factor=16 dim=3	
	
	int x_cur_cu_scu = (x0 - x_pos) >> 2;
	int y_cur_cu_scu = (y0 - y_pos) >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);
#if STORE_ENC_INFO_BEST_16_TOP
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2

#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2
#endif
	com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width, split_mode);
	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[cu_width] - 2)) + (cu_width >> 1 >> 2));
	U3 shape = SQUARE + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type)) 
	{
		com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width, split_mode);
	}

	int idx_dst = ((y_cur_cu_scu + (U9)(cu_height >> 1 >> 2)) << (prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
	int idx_src = (int)(cu_height >> 1 >> 2) << (cu_width_log2 - 2);
	(prev_cu_data_ptr->split_mode[cud][shape] + idx_dst)[(cu_width >> 1 >> 2)] = (split_mode[cud][shape] + idx_src)[(cu_width >> 1 >> 2)];

	prev_cu_data_ptr->get_cu_data_from_curblk(curr_cu_data_best, (x0 - x_pos), (y0 - y_pos), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
	
	//copy_cu_data_array_dst_16_top(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
	//	cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status, *best_split_mode, shape,pos, split_mode);

	//update_map_scu_16_top(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}

void store_enc_info_best_16_top(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
	U8 split_mode[5][7][256] = { NO_SPLIT };
	com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width, split_mode);
	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[cu_width] - 2)) + (cu_width >> 1 >> 2));
	U3 shape = SQUARE + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type))
	{
		com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width, split_mode);
	}

	copy_cu_data_array_dst_16_top(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
		cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status, *best_split_mode, shape,pos, split_mode);

	update_map_scu_16_top(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}

void store_enc_info_best_16_top2(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY_INNER* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
#pragma HLS INLINE off
	U8 split_mode[5][7][256] = { NO_SPLIT };

	/* restore best data */
	com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width, split_mode);

	U9 pos = cup + (((U9)(cu_height >> 1 >> 2) << (com_tbl_log2[cu_width] - 2)) + (cu_width >> 1 >> 2));
	U3 shape = SQUARE + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);

	if (cons_pred_mode == NO_MODE_CONS
		&& com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type)) {
		com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width, split_mode);
	}

	copy_cu_data_array_dst_16_top2(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
		cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status, *best_split_mode, shape, pos, split_mode);

	update_map_scu_16_top(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}


void compare_store_top_16(MD_FW* md_fw_ptr, s64 &cost_best, s64 cost, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_temp_array_ptr_8, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2 cons_pred_mode_child, U2 tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
#pragma HLS INLINE off
	if (cost_best > cost)
	{
		cost_best = cost;

		*best_split_mode = split_mode;

		copy_cu_data_array_src_8(curr_cu_data_best_ptr, curr_cu_data_temp_array_ptr_8, 0, 0,
			cu_width_log2, cu_height_log2, cu_width_log2, cud, tree_status);

		*best_cons_pred_mode = cons_pred_mode_child;
		*best_child_status = tree_status_child;
	}

	if (*best_split_mode != NO_SPLIT)
	{
		*tree_status_temp = TREE_C;
	}

}

#if !ONLY_QT_QT
static s64 md_16x16_loop_md1(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U8 cu_width, U8 cu_height, U2 cons_flag,
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
			bth_cost += circuit_16x16_top_md1(
				md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_32,
				up_nebs_father, left_nebs_father,
				skip_in,
				cu_data_temp_ptr,
				qt_depth, bet_depth + 1, cons_pred_mode_BTH[0], child_tree_status[0], part_num, split_struct, cu_width_log2,
				fme_mv_info,
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

void load_32x32_32x16_16x32_md1(	
	U8 cu_width, 
	U8 cu_height, 
	U3 part_num, 
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32, 

	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father, 
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	MPM_ARRAY_SIZE<32, 32>* MPM_buffer_ptr_32x32,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	int split_mode_father, 
	IP_ARRAY_SIZE<16, 32>*		IP_buffer_16x32,
	IP_ARRAY_SIZE<32, 16>*		IP_buffer_32x16,
	IP_MAP_SCU_SIZE<16, 32>*	IP_map_scu_16x32,
	IP_MAP_SCU_SIZE<32, 16>*	IP_map_scu_32x16,
	MPM_ARRAY_SIZE<16, 32>*		MPM_buffer_16x32,
	MPM_ARRAY_SIZE<32, 16>*		MPM_buffer_32x16,

	ENC_ME_NEB_INFO up_nebs[1 + (32 >> 2) + 1], 
	ENC_ME_NEB_INFO left_nebs[(32 >> 2)], 
	ENC_ME_NEB_INFO				nebs_line_ver[8],
	ENC_ME_NEB_INFO				nebs_line_hor[8],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_32)
{

	switch (split_mode_father)
	{
	case NO_SPLIT:
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
		cpy_1d_2_1d<U32, 10>(me_line_map_ptr_32->map_scu_up_line, me_line_map_32->map_scu_up_line);
		cpy_1d_2_1d<U32, 10>(me_line_map_ptr_32->map_cu_mode_up_line, me_line_map_32->map_cu_mode_up_line);
		cpy_1d_2_1d<U32, 8>(me_line_map_ptr_32->map_scu_left_line, me_line_map_32->map_scu_left_line);
		cpy_1d_2_1d<U32, 8>(me_line_map_ptr_32->map_cu_mode_left_line, me_line_map_32->map_cu_mode_left_line);
		me_line_map_32->min_scu_x = me_line_map_ptr_32->min_scu_x;
		me_line_map_32->min_scu_y = me_line_map_ptr_32->min_scu_y;
		me_line_map_32->above_line_idx = me_line_map_ptr_32->above_line_idx;
		me_line_map_32->curr_ctu_idx = me_line_map_ptr_32->curr_ctu_idx;
		break;
	case SPLIT_BI_VER:
		assert(cu_width == 16 && cu_height == 32);
		IP_buffer_16x32->fetch_bi_ver(part_num, IP_buffer_ptr_32x32);
		MPM_buffer_16x32->fetch_bi_ver(part_num, MPM_buffer_ptr_32x32);
		IP_map_scu_16x32->fetch_bi_ver(part_num, IP_map_scu_ptr_32x32);
		fetch_nebs_bi_ver(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_ver, cu_width, cu_height, part_num);
		me_line_map_32->fetch_bi_ver(part_num, me_line_map_ptr_32, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 32 && cu_height == 16);
		IP_buffer_32x16->fetch_bi_hor(part_num, IP_buffer_ptr_32x32);
		MPM_buffer_32x16->fetch_bi_hor(part_num, MPM_buffer_ptr_32x32);
		IP_map_scu_32x16->fetch_bi_hor(part_num, IP_map_scu_ptr_32x32);
		fetch_nebs_bi_hor(up_nebs, left_nebs, up_nebs_father, left_nebs_father, nebs_line_hor, cu_width, cu_height, part_num);
		me_line_map_32->fetch_bi_hor(part_num, me_line_map_ptr_32, cu_width, cu_height);
		break;
	default:assert(0);
	}	
}

s64 store_32x32_32x16_16x32_md1(MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr,
	U2 cons_pred_mode, U2 tree_status, U3 part_num, 
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
	ENC_CU_DATA_ARRAY_SIZE<32> curr_cu_data_temp[2], 
	U3 &best_split_mode, 
	U2 &best_cons_pred_mode, 
	s64 &cost_best, 
	U2 &best_tree_status_child, 		
	U2 tree_status_temp,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32, 
	IP_ARRAY_SIZE<16, 32>*		IP_buffer_16x32,
	IP_ARRAY_SIZE<32, 16>*		IP_buffer_32x16,
	IP_MAP_SCU_SIZE<16, 32>*	IP_map_scu_16x32,
	IP_MAP_SCU_SIZE<32, 16>*	IP_map_scu_32x16,
	MPM_ARRAY_SIZE<16, 32>*		MPM_buffer_16x32,
	MPM_ARRAY_SIZE<32, 16>*		MPM_buffer_32x16,
	ENC_ME_NEB_INFO				nebs_line_ver[8],
	ENC_ME_NEB_INFO				nebs_line_hor[8],
	ENC_ME_LINE_MAP_SIZE<32>*	me_line_map_32,
	COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	ENC_CU_DATA_ARRAY_SIZE<32>*	curr_cu_data_best)
{
	cost_compare_fixed_point_one_time<32>(md_fw_ptr, &cost_best, cost, curr_cu_data_temp, curr_cu_data_best, cu_width_log2, cu_height_log2, cud, tree_status,
		split_mode, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child, tree_status_child, &best_tree_status_child, &tree_status_temp, 2);

	switch (split_mode_father)
	{
	case NO_SPLIT:
		break;
	case SPLIT_BI_VER:
		assert(cu_width == 16 && cu_height == 32);
		IP_buffer_16x32->update_bi_ver(part_num, curr_cu_data_best->reco_y);
		IP_map_scu_16x32->update_bi_ver(part_num, IP_map_scu_ptr_32x32->cur_map_scu, curr_cu_data_best->map_scu);
		MPM_buffer_16x32->update_bi_ver(part_num, curr_cu_data_best->ipm[0]);
		update_nebs_bi_ver(nebs_line_ver, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
		me_line_map_32->update_bi_ver(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
		break;
	case SPLIT_BI_HOR:
		assert(cu_width == 32 && cu_height == 16);
		IP_buffer_32x16->update_bi_hor(part_num, curr_cu_data_best->reco_y);
		IP_map_scu_32x16->update_bi_hor(part_num, IP_map_scu_ptr_32x32->cur_map_scu, curr_cu_data_best->map_scu);
		MPM_buffer_32x16->update_bi_hor(part_num, curr_cu_data_best->ipm[0]);
		update_nebs_bi_hor(nebs_line_hor, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
		me_line_map_32->update_bi_hor(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
		break;
	default:assert(0);
	}

	store_enc_info_best<32>(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try,
		prev_cu_data_ptr,
		split_struct.x_pos[0], split_struct.y_pos[0], prev_up_log2_sub_cuw);	
	return 0;
}

#if !ONLY_QT_QT
static s64 circuit_32x32_32x16_16x32_md1(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	MPM_ARRAY_SIZE<32, 32>* MPM_buffer_ptr_32x32,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	Arbitor& ref,
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

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;

	U3 split_mode[3];
	split_mode[0] = NO_SPLIT;
	split_mode[1] = (cu_width == 32 && cu_height == 16) ? SPLIT_BI_VER : SPLIT_BI_HOR;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return  0;
	}

	static IP_ARRAY_SIZE<16, 32>		IP_buffer_16x32;
	static IP_ARRAY_SIZE<32, 16>		IP_buffer_32x16;
	static IP_MAP_SCU_SIZE<16, 32>		IP_map_scu_16x32;
	static IP_MAP_SCU_SIZE<32, 16>		IP_map_scu_32x16;
	static MPM_ARRAY_SIZE<16, 32>		MPM_buffer_16x32;
	static MPM_ARRAY_SIZE<32, 16>		MPM_buffer_32x16;
	static ENC_ME_NEB_INFO				up_nebs[1 + (32 >> 2) + 1];
	static ENC_ME_NEB_INFO				left_nebs[(32 >> 2)];
	static ENC_ME_NEB_INFO				nebs_line_ver[8];
	static ENC_ME_NEB_INFO				nebs_line_hor[8];
	static ENC_ME_LINE_MAP_SIZE<32>		me_line_map_32;
	static ENC_CU_DATA_ARRAY_SIZE<32>	curr_cu_data_temp[2];
	STATIC_FOR_STACK ENC_CU_DATA_ARRAY_SIZE<32>	curr_cu_data_best;

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->coef_y cyclic factor=4 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->reco_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->reco_y cyclic factor=4 dim=3

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mpm_ext cyclic factor=8 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->split_mode cyclic factor=5 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->split_mode cyclic factor=7 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mpm cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mv cyclic factor=2 dim=4
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mvd cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->mvd cyclic factor=2 dim=4
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_temp->refi cyclic factor=2 dim=3

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode complete dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode complete dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2

	//for (int i = 0; i < 2; i++)
	//{
	//	cu_data_chroma_pos(x0, y0, cu_width, cu_height, tree_status, part_num, split_struct.part_count, &curr_cu_data_temp[i], prev_cu_data_ptr, qt_depth + bet_depth);
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

	load_32x32_32x16_16x32_md1(	
		cu_width, 
		cu_height, 
		part_num, 
		me_line_map_ptr_32, 
		up_nebs_father, left_nebs_father, 
		IP_buffer_ptr_32x32,
		MPM_buffer_ptr_32x32,
		IP_map_scu_ptr_32x32,
		prev_split_mode, 
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
		skip_in, boundary, split_struct.split_allow[part_num][NO_SPLIT], &NS_luma_pred_mode, x0, y0, cup, cu_width_log2, cu_height_log2,
		cud, qt_depth, bet_depth, cons_pred_mode, tree_status,
		&curr_cu_data_temp[0],
		fme_mv_info,
		IP_buffer_ptr_32x32, &IP_buffer_16x32, &IP_buffer_32x16,
		MPM_buffer_ptr_32x32, &MPM_buffer_16x32, &MPM_buffer_32x16,
		IP_map_scu_ptr_32x32, &IP_map_scu_16x32, &IP_map_scu_32x16,
		ref,
		Fetch_Ref_window_ptr);
#endif // !KERNEL_SIZE_CLOSE(16, 32) || !KERNEL_SIZE_CLOSE(32, 16)
#if KERNEL_SIZE_CLOSE(16, 32)
	if (cu_width == 16 && cu_height == 32)
		cost[0] = MAX_S64;
#endif
#if KERNEL_SIZE_CLOSE(32, 16)
	if (cu_width == 32 && cu_height == 16)
		cost[0] = MAX_S64;
#endif

	cost[1] = md_16x16_loop_md1(
		md_fw_ptr, md_input_ptr, g_rmd_output,
		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
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


	store_32x32_32x16_16x32_md1(md_fw_ptr, prev_cu_data_ptr, cons_pred_mode, tree_status, part_num, prev_split_mode, cu_width_log2, cu_height_log2,
	cu_width, cu_height, cup, cud, x0, y0, num_split_to_try, cost, tree_status_child, cons_pred_mode_child, split_mode, curr_cu_data_temp, best_split_mode, 
	best_cons_pred_mode, cost_best, best_tree_status_child, tree_status_temp,IP_map_scu_ptr_32x32, 
	&IP_buffer_16x32, &IP_buffer_32x16,
	&IP_map_scu_16x32, &IP_map_scu_32x16,
	&MPM_buffer_16x32, &MPM_buffer_32x16,
	nebs_line_ver, nebs_line_hor, &me_line_map_32, split_struct, prev_up_log2_sub_cuw, &curr_cu_data_best);

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif

	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}
#endif

#if !ONLY_QT_QT
s64 md_16x16_loop(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in, 
	U1 isBTH_allow, U13 x0_father, U13 y0_father, U12 cup_father, U3 cu_width_log2_father, U3 cu_height_log2_father, U4 cud_father, U3 qt_depth_father, U3 bet_depth_father, U2 cons_pred_mode_father, U8 cu_width_father, U8 cu_height_father, U2 cons_flag_father,
	ENC_CU_DATA_ARRAY_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_QU[1], U2 child_tree_status[1], 
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, 
	MPM_ARRAY_SIZE<32, 32>* MPM_buffer_ptr_32x32, 
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bqu_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;

	assert(cu_width_father == 32 && cu_height_father == 32);

	STATIC_C LCUConfig config;
	STATIC_C LCUInput buffer;
	STATIC_C Channel channel;

	Instruction mem_load[PR_SIZE] = {};
	Instruction mem_pu[PR_SIZE] = {};
	Instruction mem_store[PR_SIZE] = {};

	if (isBTH_allow) {
		child_tree_status[0] = TREE_LC;
		bqu_cost = 0;

		get_split_structure_COM_combine_only_qt<32>(md_fw_ptr, x0_father, y0_father, cup_father, cud_father,
			bet_depth_father, cu_width_father, cu_height_father, &split_struct);

		for (int i = 0; i < (cu_height_father >> 2); i++)
			for (int j = 0; j < (cu_width_father >> 2); j++)
				IP_map_scu_ptr_32x32->cur_map_scu[j][i] = 0;

		int load_idx = 0, pu_idx = 0, store_idx = 0;
		int tag_pu = 1, tag_chn = 0;

#if MD_CHANNEL_DEBUG
		mem_load[load_idx].opcode() = LOAD_CHANNEL;
		mem_load[load_idx].cu_width_log2() = 5;
		mem_load[load_idx].cu_height_log2() = 5;
		mem_load[load_idx].flag_pu(BYPASS_CHANNEL) = 1;
		mem_load[load_idx].tag_chn() = tag_chn;
		++load_idx;
		++tag_chn;

		mem_pu[pu_idx].opcode() = BYPASS_CHANNEL;
		mem_pu[pu_idx].cu_width_log2() = 5;
		mem_pu[pu_idx].cu_height_log2() = 5;
		mem_pu[pu_idx].tag_pu() = 1;
		++pu_idx;

		mem_store[store_idx].opcode() = STORE_CHN_1;
		mem_store[store_idx].cu_width_log2() = 5;
		mem_store[store_idx].cu_height_log2() = 5;
		mem_store[store_idx].flag_pu(BYPASS_CHANNEL) = 1;
		mem_store[store_idx].flag_chn(0) = 1;
		mem_store[store_idx].tag_pu() = 1;
		mem_store[store_idx].tag_chn() = tag_chn;
		++store_idx;
#endif

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=4

			U3 cu_width_log2 = split_struct.log_cuw[part_num];
			U3 cu_height_log2 = split_struct.log_cuh[part_num];
			U8 cu_width = (1 << cu_width_log2);
			U8 cu_height = (1 << cu_height_log2);
			U12 cup = split_struct.cup[part_num];
			U4 cud = split_struct.cud;
			U14 x0 = split_struct.x_pos[part_num];
			U14 y0 = split_struct.y_pos[part_num];
			U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
			U3 qt_depth = qt_depth_father + 1;
			U3 bet_depth = bet_depth_father;
			U2 cons_pred_mode = cons_pred_mode_QU[0];
			U2 tree_status = child_tree_status[0];

			if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height))
			{
				continue;
			}

			mem_load[load_idx].opcode() = LOAD_1_N;
			mem_load[load_idx].part_num() = part_num;
			mem_load[load_idx].flag_chn(0) = 1;
			mem_load[load_idx].flag_pu(KERNEL_16X16) = split_struct.split_allow[part_num][NO_SPLIT] ? 1 : 0;
			mem_load[load_idx].flag_pu(KERNEL_8X16) = split_struct.split_allow[part_num][SPLIT_BI_VER] ? 1 : 0;
			mem_load[load_idx].flag_pu(KERNEL_16X8) = split_struct.split_allow[part_num][SPLIT_BI_HOR] ? 1 : 0;
			mem_load[load_idx].flag_pu(KERNEL_8X8) = split_struct.split_allow[part_num][SPLIT_QUAD] ? 1 : 0;
			mem_load[load_idx].tag_chn() = tag_chn;
			++load_idx;
			++tag_chn;

			if (split_struct.split_allow[part_num][NO_SPLIT])
			{
				mem_pu[pu_idx].opcode() = KERNEL_16X16;
				mem_pu[pu_idx].split_mode() = NO_SPLIT;
				mem_pu[pu_idx].btd() = bet_depth;
				mem_pu[pu_idx].pred_mode() = cons_pred_mode;
				mem_pu[pu_idx].cud() = cud;
				mem_pu[pu_idx].cup() = cup;
				mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
				mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
				mem_pu[pu_idx].qtd() = qt_depth;
				mem_pu[pu_idx].tree_status() = tree_status;
				mem_pu[pu_idx].x0() = x0;
				mem_pu[pu_idx].y0() = y0;
				mem_pu[pu_idx].tag_pu() = tag_pu;
				++pu_idx;
			}

			if (split_struct.split_allow[part_num][SPLIT_BI_VER])
			{
				mem_pu[pu_idx].opcode() = KERNEL_8X16;
				mem_pu[pu_idx].split_mode() = SPLIT_BI_VER;
				mem_pu[pu_idx].btd() = bet_depth;
				mem_pu[pu_idx].pred_mode() = cons_pred_mode;
				mem_pu[pu_idx].cud() = cud;
				mem_pu[pu_idx].cup() = cup;
				mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
				mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
				mem_pu[pu_idx].qtd() = qt_depth;
				mem_pu[pu_idx].tree_status() = tree_status;
				mem_pu[pu_idx].x0() = x0;
				mem_pu[pu_idx].y0() = y0;
				mem_pu[pu_idx].tag_pu() = tag_pu;
				++pu_idx;
			}

			if (split_struct.split_allow[part_num][SPLIT_BI_HOR])
			{
				mem_pu[pu_idx].opcode() = KERNEL_16X8;
				mem_pu[pu_idx].split_mode() = SPLIT_BI_HOR;
				mem_pu[pu_idx].btd() = bet_depth;
				mem_pu[pu_idx].pred_mode() = cons_pred_mode;
				mem_pu[pu_idx].cud() = cud;
				mem_pu[pu_idx].cup() = cup;
				mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
				mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
				mem_pu[pu_idx].qtd() = qt_depth;
				mem_pu[pu_idx].tree_status() = tree_status;
				mem_pu[pu_idx].x0() = x0;
				mem_pu[pu_idx].y0() = y0;
				mem_pu[pu_idx].tag_pu() = tag_pu;
				++pu_idx;
			}

			if (split_struct.split_allow[part_num][SPLIT_QUAD])
			{
				mem_pu[pu_idx].opcode() = KERNEL_8X8;
				mem_pu[pu_idx].split_mode() = SPLIT_QUAD;
				mem_pu[pu_idx].btd() = bet_depth;
				mem_pu[pu_idx].pred_mode() = cons_pred_mode;
				mem_pu[pu_idx].cud() = cud;
				mem_pu[pu_idx].cup() = cup;
				mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
				mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
				mem_pu[pu_idx].qtd() = qt_depth;
				mem_pu[pu_idx].tree_status() = tree_status;
				mem_pu[pu_idx].x0() = x0;
				mem_pu[pu_idx].y0() = y0;
				mem_pu[pu_idx].tag_pu() = tag_pu;
				++pu_idx;
			}

			mem_store[store_idx].opcode() = STORE_N_1;
			mem_store[store_idx].cu_width_log2() = cu_width_log2;
			mem_store[store_idx].cu_height_log2() = cu_height_log2;
			mem_store[store_idx].cud() = cud;
			mem_store[store_idx].tree_status() = tree_status;
			mem_store[store_idx].x0() = x0;
			mem_store[store_idx].y0() = y0;
			mem_store[store_idx].part_num() = part_num;
			mem_store[store_idx].cup() = cup;
			mem_store[store_idx].pred_mode() = cons_pred_mode;
			mem_store[store_idx].x_pos() = split_struct.x_pos[0];
			mem_store[store_idx].y_pos() = split_struct.y_pos[0];
			mem_store[store_idx].prev_up_log2_sub_cuw() = cu_width_log2_father;
			mem_store[store_idx].flag_chn(0) = 1;
			mem_store[store_idx].flag_pu(KERNEL_16X16) = split_struct.split_allow[part_num][NO_SPLIT] ? 1 : 0;
			mem_store[store_idx].flag_pu(KERNEL_8X16) = split_struct.split_allow[part_num][SPLIT_BI_VER] ? 1 : 0;
			mem_store[store_idx].flag_pu(KERNEL_16X8) = split_struct.split_allow[part_num][SPLIT_BI_HOR] ? 1 : 0;
			mem_store[store_idx].flag_pu(KERNEL_8X8) = split_struct.split_allow[part_num][SPLIT_QUAD] ? 1 : 0;
			mem_store[store_idx].tag_pu() = tag_pu;
			mem_store[store_idx].tag_chn() = tag_chn;
			mem_store[store_idx].mem_ptr() = part_num;
			++store_idx;
			++tag_pu;
		}

		mem_load[load_idx].opcode() = LOAD_1_BYPASS;
		mem_load[load_idx].version() = 2;
		mem_load[load_idx].cu_width_log2() = 5;
		mem_load[load_idx].cu_height_log2() = 5;
		mem_load[load_idx].flag_chn(0) = 1;
		mem_load[load_idx].tag_chn() = tag_chn;
		mem_load[load_idx].flag_pu(BYPASS + 0) = 1;
		++load_idx;
		++tag_chn;

#if MD_CHANNEL_DEBUG
		mem_load[load_idx].opcode() = LOAD_1_OUT_CHANNEL;
		mem_load[load_idx].version() = 2;
		mem_load[load_idx].cu_width_log2() = 5;
		mem_load[load_idx].cu_height_log2() = 5;
		mem_load[load_idx].x0() = x0_father;
		mem_load[load_idx].y0() = y0_father;
		mem_load[load_idx].flag_chn(0) = 1;
		mem_load[load_idx].tag_chn() = tag_chn;
		mem_load[load_idx].flag_pu(OUTPUT_CHANNEL) = 1;
		++load_idx;
#endif

		mem_load[load_idx].opcode() = DONE;

		mem_pu[pu_idx].opcode() = BYPASS + 0;
		mem_pu[pu_idx].cu_width_log2() = 5;
		mem_pu[pu_idx].cu_height_log2() = 5;
		mem_pu[pu_idx].x0() = x0_father;
		mem_pu[pu_idx].y0() = y0_father;
		mem_pu[pu_idx].cud() = cud_father;
		mem_pu[pu_idx].qtd() = qt_depth_father;
		mem_pu[pu_idx].btd() = bet_depth_father;
		mem_pu[pu_idx].split_mode() = SPLIT_QUAD;
		mem_pu[pu_idx].tag_pu() = 1;
		++pu_idx;

#if MD_CHANNEL_DEBUG
		mem_pu[pu_idx].opcode() = OUTPUT_CHANNEL;
		mem_pu[pu_idx].cu_width_log2() = 5;
		mem_pu[pu_idx].cu_height_log2() = 5;
		mem_pu[pu_idx].tag_pu() = 1;
#endif

		mem_store[store_idx].opcode() = STORE_N_1;
		mem_store[store_idx].x0() = x0_father;
		mem_store[store_idx].y0() = y0_father;
		mem_store[store_idx].cu_width_log2() = 5;
		mem_store[store_idx].cu_height_log2() = 5;
		mem_store[store_idx].cud() = cud_father;
		mem_store[store_idx].flag_chn(0) = 1;
		mem_store[store_idx].flag_pu(BYPASS + 0) = 1;
		mem_store[store_idx].tag_pu() = 1;
		mem_store[store_idx].tag_chn() = tag_chn;
		mem_store[store_idx].mem_ptr() = 0;
		++store_idx;

		mem_store[store_idx].opcode() = DONE;

		config.md_fw_ptr = *md_fw_ptr;
		config.md_input_ptr = *md_input_ptr;

		//memcpy(buffer.CtrPos, CtrPos, sizeof(buffer.CtrPos));
		for (size_t i = 0; i < 2; i++)
		{
			for (size_t j = 0; j < 2; j++)
			{
				for (size_t k = 0; k < 2; k++)
				{
					buffer.CtrPos[i][j][k] = CtrPos[i][j][k];
				}
			}
		}
		//memcpy(buffer.CtrPos_ref1, CtrPos_ref1, sizeof(buffer.CtrPos_ref1));
		for (size_t i = 0; i < 2; i++)
		{
			for (size_t j = 0; j < 2; j++)
			{
				for (size_t k = 0; k < 2; k++)
				{
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
			for (size_t j = 0; j < MAX_CU_SIZE_FETCH; j++)
			{
				buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
			}
		}
		//memcpy(buffer.RefWindowLCU_UP, RefWindowLCU_UP, sizeof(buffer.RefWindowLCU_UP));
		for (size_t i = 0; i < (SWH * SWW) >> 2; i++)
		{
			buffer.RefWindowLCU_UP[i] = RefWindowLCU_UP[i];
		}
		//memcpy(buffer.RefWindowLCU_UP_ref1, RefWindowLCU_UP_ref1, sizeof(buffer.RefWindowLCU_UP_ref1));
		for (size_t i = 0; i < (SWH * SWW) >> 2; i++)
		{
			buffer.RefWindowLCU_UP_ref1[i] = RefWindowLCU_UP_ref1[i];
		}
		//memcpy(buffer.RefWindowLCU_VP, RefWindowLCU_VP, sizeof(buffer.RefWindowLCU_VP));
		for (size_t i = 0; i < (SWH * SWW) >> 2; i++)
		{
			buffer.RefWindowLCU_VP[i] = RefWindowLCU_VP[i];
		}
		//memcpy(buffer.RefWindowLCU_VP_ref1, RefWindowLCU_VP_ref1, sizeof(buffer.RefWindowLCU_VP_ref1));
		for (size_t i = 0; i < (SWH * SWW) >> 2; i++)
		{
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
		channel.IP_buffer_ptr_32x32 = *IP_buffer_ptr_32x32;
		channel.IP_map_scu_ptr_32x32 = *IP_map_scu_ptr_32x32;
		//memcpy(channel.left_nebs_32, left_nebs_father, sizeof(channel.left_nebs_32));
		for (size_t i = 0; i < 8; i++)
		{
			channel.left_nebs_32[i] = left_nebs_father[i];
		}
		channel.me_line_map_ptr_32 = *me_line_map_ptr_32;
		channel.MPM_buffer_ptr_32x32 = *MPM_buffer_ptr_32x32;
		//memcpy(channel.up_nebs_32, up_nebs_father, sizeof(channel.up_nebs_32));
		for (size_t i = 0; i < 10; i++)
		{
			channel.up_nebs_32[i] = up_nebs_father[i];
		}

		channel.line_buffer.init();
		if (x0_father != 0)
		{
			if ((x0_father & 63) == 0)
			{
				channel.line_buffer.y0 = (((y0_father & 63) + 64) >> 2);
			}
			else if ((x0_father & 31) == 0)
			{
				for (int i = 0; i < ((x0_father & 63) >> 2); ++i)
				{
					channel.line_buffer.y[i] = (((y0_father & 63) + 32) >> 2);
				}
				channel.line_buffer.y0 = (((y0_father & 63) + 32) >> 2);
			}
		}
		copy(x0_father, y0_father, channel.line_buffer, *IP_buffer_ptr_32x32);
		copy(x0_father, y0_father, channel.line_buffer, *MPM_buffer_ptr_32x32);
		copy(x0_father, y0_father, channel.line_buffer, *IP_map_scu_ptr_32x32);
		copy(x0_father, y0_father, channel.line_buffer, *me_line_map_ptr_32);
		config.min_scu_x = me_line_map_ptr_32->min_scu_x;
		config.min_scu_y = me_line_map_ptr_32->min_scu_y;
		copy<32, 32>(x0_father, y0_father, channel.line_buffer, up_nebs_father, left_nebs_father);

		ENC_CU_DATA cu_data_unuse;

		vmod(config, buffer, ref, channel, channel, mem_load, mem_pu, mem_store, cu_data_unuse/*ENC_CU_DATA()*/);

		//load_line_buffer(x0_father, y0_father, *IP_buffer_ptr_32x32, channel.line_buffer);
		*cu_data_temp_ptr = channel.prev_cu_data_ptr;
		channel.prev_cu_data_ptr.print(32, 32);
		//*IP_map_scu_ptr_32x32 = channel.IP_map_scu_ptr_32x32;
		bqu_cost = channel.cost_best[0][0];

		//printf("bqu_cost=%lld\n", bqu_cost);
	}
	return bqu_cost;
}

s64 md_32x32_32x16_16x32_loop_md1(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<32>* me_line_map_ptr_32,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<32>* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32,
	MPM_ARRAY_SIZE<32, 32>* MPM_buffer_ptr_32x32,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 bth_cost = MAX_S64;
	static COM_SPLIT_STRUCT split_struct;
	U3 part_num = 0;

	if (isBTH_allow) {
		child_tree_status[0] = TREE_LC;
		bth_cost = prepare_cu_info_COM_combine<32>(md_fw_ptr, md_input_ptr->lambda_y, cu_data_temp_ptr, x0, y0, cup, cud,
			qt_depth, bet_depth, cons_pred_mode, cons_pred_mode_BTH, cu_width, cu_height, split_mode, cons_flag, &split_struct);

		for (part_num = 0; part_num < split_struct.part_count; ++part_num) {
#pragma HLS LOOP_TRIPCOUNT max=2
			bth_cost += circuit_32x32_32x16_16x32_md1(md_fw_ptr, md_input_ptr, g_rmd_output, RefWindowLCU_UP, RefWindowLCU_VP, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1, me_mv_dir,
#endif
				p_fenc_LCU_Y,
				me_line_map_ptr_32,
				up_nebs_father, left_nebs_father,
				skip_in,
				cu_data_temp_ptr,
				qt_depth, split_mode != NO_SPLIT ? (U3)(bet_depth + 1) : bet_depth, cons_pred_mode_BTH[0], child_tree_status[0], part_num, split_struct, cu_width_log2,
				fme_mv_info,
				IP_buffer_ptr_32x32,
				MPM_buffer_ptr_32x32,
				IP_map_scu_ptr_32x32,
				ref,
				Fetch_Ref_window_ptr, split_mode);
		}
	}
	return bth_cost;
}
#endif

#if !CIRCUIT32_DUMMY
void load_32x32_top( InstructionPU& insn_pu, InstructionLoad& insn, Load2PU& in_buf, Channel& channel)
{
		S7 ip_array_size_idx = get_ip_array_size_idx(64, 64);
		STATIC_C IP_ARRAY_SIZE<16, 16> null_IP_buffer_dst_16x16;
		STATIC_C IP_MAP_SCU_SIZE<16, 16> null_IP_map_scu_dst_16x16;
		STATIC_C MPM_ARRAY_SIZE<16, 16> null_MPM_buffer_dst_16x16;
		STATIC_C IP_ARRAY_SIZE<16, 16> null_IP_buffer_pre_16x16;
		STATIC_C MPM_ARRAY_SIZE<16, 16> null_MPM_buffer_pre_16x16;
		STATIC_C IP_ARRAY_SIZE<32, 32> null_IP_buffer_src_32x32;
		STATIC_C IP_MAP_SCU_SIZE<32, 32> null_IP_map_scu_src_32x32;
		STATIC_C MPM_ARRAY_SIZE<32, 32> null_MPM_buffer_src_32x32;

		fetch_nebs_qt(channel.up_nebs, channel.left_nebs, channel.up_nebs_32x32, channel.left_nebs_32x32, channel.nebs_line_hor_32x32, \
			channel.nebs_line_ver_32x32, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);

		in_buf.me_line_map_32.fetch_me_line_qt(insn.part_num, &channel.me_line_map_ptr_64, insn_pu.cu_width, insn_pu.cu_height);

		fetch_line_buffer_qt(insn.part_num, insn_pu.cu_width, insn_pu.cu_height, \
			& null_IP_buffer_dst_16x16, &null_IP_map_scu_dst_16x16, &null_MPM_buffer_dst_16x16, \
			& null_IP_buffer_pre_16x16, &null_MPM_buffer_pre_16x16, &null_IP_buffer_src_32x32, \
			& null_IP_map_scu_src_32x32, &null_MPM_buffer_src_32x32, \
			& in_buf.IP_buffer_32x32, &in_buf.IP_map_scu_32x32, &in_buf.MPM_buffer_32x32, \
			& in_buf.IP_buffer_32x32, &in_buf.MPM_buffer_32x32, \
			& channel.IP_buffer_ptr_64x64, &channel.IP_map_scu_ptr_64x64, &channel.MPM_buffer_ptr_64x64);
}

s64 store_32x32_top(InstructionStore& insn_store, Channel& channel, PU2Store pu_out_buf[PU_NUM], LCUConfig& config)
{
	U2 tree_status_temp = insn_store.tree_status;
	S4 best_index = -1;

	ENC_CU_DATA_ARRAY curr_cu_data_best;
	ENC_CU_DATA_ARRAY_SIZE<32>	cu_data_temp[4];

	cu_data_temp[0] = pu_out_buf[NO_SPLIT].cu_data_temp;
	cu_data_temp[1] = pu_out_buf[SPLIT_BI_VER].cu_data_temp;
	cu_data_temp[2] = pu_out_buf[SPLIT_BI_HOR].cu_data_temp;
	cu_data_temp[3] = pu_out_buf[SPLIT_QUAD].cu_data_temp;

	for (U3 i = 0; i < 4; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max= 4
#pragma HLS PIPELINE
		if (channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2] > insn_store.cost_temp[i])
		{
			channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2] = insn_store.cost_temp[i];
			best_index = i;
		}
	}

	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16> null_curr_cu_data_ptr;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16> null_curr_cu_data_best_ptr;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_best_ptr_32;

	if (best_index != -1)
	{
		copy_split_mode<32>(cu_data_temp, &curr_cu_data_best,
			insn_store.cu_width_log2, insn_store.cu_height_log2, insn_store.cud, best_index);
		compare_store_top(&config.md_fw_ptr, channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2], insn_store.cost_temp[best_index], &null_curr_cu_data_ptr, &cu_data_temp[best_index], &null_curr_cu_data_best_ptr, &null_curr_cu_data_best_ptr_32, &curr_cu_data_best,
			insn_store.cu_width_log2, insn_store.cu_height_log2, insn_store.cud, insn_store.tree_status,
			insn_store.split_mode[best_index], &insn_store.best_split_mode, &insn_store.best_cons_pred_mode, pu_out_buf[best_index].cons_pred_mode_child, pu_out_buf[best_index].tree_status_child, &insn_store.best_tree_status_child, &tree_status_temp, 0);
	}

	int scu_h = (insn_store.y0 + 32 > config.md_fw_ptr.pic_height ? ((config.md_fw_ptr.pic_height - insn_store.y0) >> 2) : (32 >> 2));
	int scu_w = (insn_store.x0 + 32 > config.md_fw_ptr.pic_width ? ((config.md_fw_ptr.pic_width - insn_store.x0) >> 2) : (32 >> 2));

	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16> null_curr_cu_data_best_ptr_16;
	STATIC_C IP_ARRAY_SIZE<16, 16> null_IP_buffer_16x16;
	STATIC_C MPM_ARRAY_SIZE<16, 16> null_MPM_buffer_16x16;
	STATIC_C IP_MAP_SCU_SIZE<32, 32> null_IP_map_scu_32x32;

	update_nebs_qt(channel.nebs_line_hor_32x32, channel.nebs_line_ver_32x32, curr_cu_data_best.refi, curr_cu_data_best.mv, insn_store.cu_width, insn_store.cu_height, insn_store.part_num);

	channel.me_line_map_32.update_me_line_qt(insn_store.part_num, curr_cu_data_best.map_scu, curr_cu_data_best.map_cu_mode, insn_store.cu_width, insn_store.cu_height);

	update_line_buffer_qt(insn_store.part_num, insn_store.cu_width, insn_store.cu_height, scu_h, scu_w, insn_store.cu_width_log2, &null_curr_cu_data_best_ptr_16, \
		&null_IP_buffer_16x16, &null_MPM_buffer_16x16, &null_IP_map_scu_32x32, \
		&curr_cu_data_best, &channel.IP_buffer_32x32, &channel.MPM_buffer_32x32, &channel.IP_map_scu_ptr_64x64);

	store_enc_info_best(&config.md_fw_ptr, &curr_cu_data_best, insn_store.x0, insn_store.y0, insn_store.cup, insn_store.cu_width_log2, insn_store.cu_height_log2, \
		insn_store.cud, insn_store.cons_pred_mode, insn_store.cu_width, insn_store.cu_height, insn_store.tree_status, &insn_store.best_split_mode, &insn_store.best_cons_pred_mode, \
		insn_store.num_split_to_try, &channel.prev_cu_data_ptr_32x32, insn_store.x_pos, insn_store.y_pos, insn_store.prev_up_log2_sub_cuw);

	return channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2];
}

#if !ONLY_QT_QT
void pu_32x32_top_0(Load2PU& pu_in_buf, s64& cost_temp, LCUInput& buffer, PU2Store& pu_out_buf, Arbitor& ref)
{
	cost_temp = md_16x16_loop(&pu_in_buf.config.md_fw_ptr, &pu_in_buf.config.md_input_ptr, &buffer.g_rmd_output, buffer.RefWindowLCU_UP, buffer.RefWindowLCU_VP, buffer.CtrPos,
#if ENABLE_BFRAME
		buffer.RefWindowLCU_UP_ref1, buffer.RefWindowLCU_VP_ref1, buffer.CtrPos_ref1, & pu_in_buf.buffer.me_mv_dir,
#endif
		buffer.p_fenc_LCU_Y,
		&pu_in_buf.me_line_map_32,
		pu_in_buf.up_nebs_32, pu_in_buf.left_nebs_32,
		&pu_in_buf.buffer.skip_in,
		true, pu_in_buf.insn.x0, pu_in_buf.insn.y0,
		pu_in_buf.insn.cup, pu_in_buf.insn.cu_width_log2, pu_in_buf.insn.cu_height_log2, pu_in_buf.insn.cud, pu_in_buf.insn.qt_depth, pu_in_buf.insn.bet_depth,
		pu_in_buf.insn.cons_pred_mode, pu_in_buf.insn.cu_width, pu_in_buf.insn.cu_height, ONLY_INTRA,
		&pu_out_buf.cu_data_temp,
		&pu_out_buf.cons_pred_mode_child, &pu_out_buf.tree_status_child,
		&pu_in_buf.buffer.fme_mv_info,
		&pu_in_buf.IP_buffer_32x32,
		&pu_in_buf.MPM_buffer_32x32,
		&pu_in_buf.IP_map_scu_32x32,
		ref,
		&pu_in_buf.buffer.Fetch_Ref_window_ptr
	);
};

void pu_32x32_top_1(Load2PU& pu_in_buf, s64& cost_temp, LCUInput& buffer, PU2Store& pu_out_buf, Arbitor& ref)
{
	cost_temp = md_32x32_32x16_16x32_loop_md1(pu_in_buf.insn.split_mode,
		&pu_in_buf.config.md_fw_ptr, &pu_in_buf.config.md_input_ptr, &buffer.g_rmd_output, buffer.RefWindowLCU_UP, buffer.RefWindowLCU_VP, buffer.CtrPos,
#if ENABLE_BFRAME
		buffer.RefWindowLCU_UP_ref1, buffer.RefWindowLCU_VP_ref1, buffer.CtrPos_ref1, &pu_in_buf.buffer.me_mv_dir,
#endif
		buffer.p_fenc_LCU_Y,
		&pu_in_buf.me_line_map_32,
		pu_in_buf.up_nebs_32, pu_in_buf.left_nebs_32,
		&pu_in_buf.buffer.skip_in,
		true, pu_in_buf.insn.x0, pu_in_buf.insn.y0,
		pu_in_buf.insn.cup, pu_in_buf.insn.cu_width_log2, pu_in_buf.insn.cu_height_log2, pu_in_buf.insn.cud, pu_in_buf.insn.qt_depth, pu_in_buf.insn.bet_depth,
		pu_in_buf.insn.cons_pred_mode, pu_in_buf.insn.cu_width, pu_in_buf.insn.cu_height, ONLY_INTRA,
		&pu_out_buf.cu_data_temp,
		&pu_out_buf.cons_pred_mode_child, &pu_out_buf.tree_status_child,
		&pu_in_buf.buffer.fme_mv_info,
		&pu_in_buf.IP_buffer_32x32,
		&pu_in_buf.MPM_buffer_32x32,
		&pu_in_buf.IP_map_scu_32x32,
		ref,
		&pu_in_buf.buffer.Fetch_Ref_window_ptr
	);
};
#endif

s64 circuit_32x32_top_qt(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	//ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO up_nebs_father[18], ENC_ME_NEB_INFO left_nebs_father[16], //作为顶层函数综合时需要指定数组大小，否则报错
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA* prev_cu_data_ptr,
	U3 qt_depth_father, U3 bet_depth_father, U2 cons_pred_mode_father, U2 tree_status_father, U3 part_num_father, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw_father,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father,

	Instruction mem_load[PR_SIZE],
Instruction mem_pu[PR_SIZE],
Instruction mem_store[PR_SIZE])
{
	U3 cu_width_log2_father = split_struct.log_cuw[part_num_father];
	U3 cu_height_log2_father = split_struct.log_cuh[part_num_father];
	U8 cu_width_father = (1 << cu_width_log2_father);
	U8 cu_height_father = (1 << cu_height_log2_father);
	U12 cup_father = split_struct.cup[part_num_father];
	U4 cud_father = split_struct.cud;
	U14 x0_father = split_struct.x_pos[part_num_father];
	U14 y0_father = split_struct.y_pos[part_num_father];
	U3 num_split_to_try_father = split_struct.num_split[part_num_father];

	STATIC_C InstructionPU insn_pu;
	STATIC_C ShareMemory pool;
	STATIC_C InstructionLoad insn_load;
	static Load2PU pu_in_buf[PU_NUM];

	insn_pu.cu_width_log2 = cu_width_log2_father;
	insn_pu.cu_height_log2 = cu_height_log2_father;
	insn_pu.x0 = x0_father;
	insn_pu.y0 = y0_father;
	insn_pu.split_mode = split_mode_father;
	insn_pu.cu_width = cu_width_father;
	insn_pu.cu_height = cu_height_father;

	insn_load.part_num = part_num_father;

	U2 cons_pred_mode_child[1] = { NO_MODE_CONS };

	int load_idx = 0, pu_idx = 0, store_idx = 0;
	int tag_pu = 1, tag_chn = 0;

	if (split_struct.split_allow[part_num_father][SPLIT_QUAD])
	{
		pu_in_buf[3].insn.cud = cud_father;
		pu_in_buf[3].insn.cup = cup_father;
		pu_in_buf[3].insn.cu_height_log2 = cu_height_log2_father;
		pu_in_buf[3].insn.cu_width_log2 = cu_width_log2_father;
		pu_in_buf[3].insn.cu_width = cu_width_father;
		pu_in_buf[3].insn.cu_height = cu_height_father;
		pu_in_buf[3].insn.x0 = x0_father;
		pu_in_buf[3].insn.y0 = y0_father;
		pu_in_buf[3].insn.qt_depth = qt_depth_father;
		pu_in_buf[3].insn.bet_depth = bet_depth_father;
		pu_in_buf[3].insn.cons_pred_mode = cons_pred_mode_father;
		pu_in_buf[3].insn.tree_status = tree_status_father;

		//s64 bqu_cost = MAX_S64;
		static COM_SPLIT_STRUCT split_struct_child;
		U3 part_num_child = 0;

		assert(cu_width_father == 32 && cu_height_father == 32);

		//STATIC_C Channel channel_child;

		if (split_struct.split_allow[part_num_father][SPLIT_QUAD])
		{
			get_split_structure_COM_combine_only_qt<32>(md_fw_ptr, pu_in_buf[3].insn.x0, pu_in_buf[3].insn.y0, pu_in_buf[3].insn.cup, pu_in_buf[3].insn.cud,
				pu_in_buf[3].insn.bet_depth, pu_in_buf[3].insn.cu_width, pu_in_buf[3].insn.cu_height, &split_struct_child);

			for (int i = 0; i < (pu_in_buf[3].insn.cu_height >> 2); i++)
				for (int j = 0; j < (pu_in_buf[3].insn.cu_width >> 2); j++)
					pu_in_buf[3].IP_map_scu_32x32.cur_map_scu[j][i] = 0;

#if MD_CHANNEL_DEBUG
			mem_load[load_idx].opcode() = LOAD_CHANNEL;
			mem_load[load_idx].cu_width_log2() = 6;
			mem_load[load_idx].cu_height_log2() = 6;
			mem_load[load_idx].flag_pu(BYPASS_CHANNEL) = 1;
			mem_load[load_idx].tag_chn() = tag_chn;
			++load_idx;
			++tag_chn;

			mem_pu[pu_idx].opcode() = BYPASS_CHANNEL;
			mem_pu[pu_idx].cu_width_log2() = 6;
			mem_pu[pu_idx].cu_height_log2() = 6;
			mem_pu[pu_idx].tag_pu() = 1;
			++pu_idx;

			mem_store[store_idx].opcode() = STORE_CHN_1;
			mem_store[store_idx].cu_width_log2() = 6;
			mem_store[store_idx].cu_height_log2() = 6;
			mem_store[store_idx].flag_pu(BYPASS_CHANNEL) = 1;
			mem_store[store_idx].flag_chn(0) = 1;
			mem_store[store_idx].tag_pu() = 1;
			mem_store[store_idx].tag_chn() = tag_chn;
			++store_idx;
#endif

			for (part_num_child = 0; part_num_child < split_struct_child.part_count; ++part_num_child)
			{
#pragma HLS LOOP_TRIPCOUNT max=4

				U3 cu_width_log2 = split_struct_child.log_cuw[part_num_child];
				U3 cu_height_log2 = split_struct_child.log_cuh[part_num_child];
				U8 cu_width = (1 << cu_width_log2);
				U8 cu_height = (1 << cu_height_log2);
				U12 cup = split_struct_child.cup[part_num_child];
				U4 cud = split_struct_child.cud;
				U14 x0 = split_struct_child.x_pos[part_num_child];
				U14 y0 = split_struct_child.y_pos[part_num_child];
				U1 boundary = !(x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height);
				U3 qt_depth = pu_in_buf[3].insn.qt_depth + 1;
				U3 bet_depth = pu_in_buf[3].insn.bet_depth;
				U2 cons_pred_mode = pu_in_buf[3].insn.cons_pred_mode;
				U2 tree_status = TREE_LC;

				if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height))
				{
					continue;
				}

				mem_load[load_idx].opcode() = part_num_child == 0 ? LOAD_1_N : LOAD_1_N;
				mem_load[load_idx].part_num() = part_num_child == 0 ? part_num_father : part_num_child;
				mem_load[load_idx].flag_chn(0) = 1;
				mem_load[load_idx].flag_pu(KERNEL_16X16) = split_struct_child.split_allow[part_num_child][NO_SPLIT] ? 1 : 0;
				mem_load[load_idx].flag_pu(KERNEL_8X16) = split_struct_child.split_allow[part_num_child][SPLIT_BI_VER] ? 1 : 0;
				mem_load[load_idx].flag_pu(KERNEL_16X8) = split_struct_child.split_allow[part_num_child][SPLIT_BI_HOR] ? 1 : 0;
				mem_load[load_idx].flag_pu(KERNEL_8X8) = split_struct_child.split_allow[part_num_child][SPLIT_QUAD] ? 1 : 0;
				mem_load[load_idx].tag_chn() = tag_chn;
				++load_idx;
				++tag_chn;

				if (split_struct_child.split_allow[part_num_child][NO_SPLIT])
				{
					mem_pu[pu_idx].opcode() = KERNEL_16X16;
					mem_pu[pu_idx].split_mode() = NO_SPLIT;
					mem_pu[pu_idx].btd() = bet_depth;
					mem_pu[pu_idx].pred_mode() = cons_pred_mode;
					mem_pu[pu_idx].cud() = cud;
					mem_pu[pu_idx].cup() = cup;
					mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
					mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
					mem_pu[pu_idx].qtd() = qt_depth;
					mem_pu[pu_idx].tree_status() = tree_status;
					mem_pu[pu_idx].x0() = x0;
					mem_pu[pu_idx].y0() = y0;
					mem_pu[pu_idx].tag_pu() = tag_pu;
					++pu_idx;
				}

				if (split_struct_child.split_allow[part_num_child][SPLIT_BI_VER])
				{
					mem_pu[pu_idx].opcode() = KERNEL_8X16;
					mem_pu[pu_idx].split_mode() = SPLIT_BI_VER;
					mem_pu[pu_idx].btd() = bet_depth;
					mem_pu[pu_idx].pred_mode() = cons_pred_mode;
					mem_pu[pu_idx].cud() = cud;
					mem_pu[pu_idx].cup() = cup;
					mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
					mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
					mem_pu[pu_idx].qtd() = qt_depth;
					mem_pu[pu_idx].tree_status() = tree_status;
					mem_pu[pu_idx].x0() = x0;
					mem_pu[pu_idx].y0() = y0;
					mem_pu[pu_idx].tag_pu() = tag_pu;
					++pu_idx;
				}

				if (split_struct_child.split_allow[part_num_child][SPLIT_BI_HOR])
				{
					mem_pu[pu_idx].opcode() = KERNEL_16X8;
					mem_pu[pu_idx].split_mode() = SPLIT_BI_HOR;
					mem_pu[pu_idx].btd() = bet_depth;
					mem_pu[pu_idx].pred_mode() = cons_pred_mode;
					mem_pu[pu_idx].cud() = cud;
					mem_pu[pu_idx].cup() = cup;
					mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
					mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
					mem_pu[pu_idx].qtd() = qt_depth;
					mem_pu[pu_idx].tree_status() = tree_status;
					mem_pu[pu_idx].x0() = x0;
					mem_pu[pu_idx].y0() = y0;
					mem_pu[pu_idx].tag_pu() = tag_pu;
					++pu_idx;
				}

				if (split_struct_child.split_allow[part_num_child][SPLIT_QUAD])
				{
					mem_pu[pu_idx].opcode() = KERNEL_8X8;
					mem_pu[pu_idx].split_mode() = SPLIT_QUAD;
					mem_pu[pu_idx].btd() = bet_depth;
					mem_pu[pu_idx].pred_mode() = cons_pred_mode;
					mem_pu[pu_idx].cud() = cud;
					mem_pu[pu_idx].cup() = cup;
					mem_pu[pu_idx].cu_height_log2() = cu_height_log2;
					mem_pu[pu_idx].cu_width_log2() = cu_width_log2;
					mem_pu[pu_idx].qtd() = qt_depth;
					mem_pu[pu_idx].tree_status() = tree_status;
					mem_pu[pu_idx].x0() = x0;
					mem_pu[pu_idx].y0() = y0;
					mem_pu[pu_idx].tag_pu() = tag_pu;
					++pu_idx;
				}

				mem_store[store_idx].opcode() = STORE_N_1;
				mem_store[store_idx].cu_width_log2() = cu_width_log2;
				mem_store[store_idx].cu_height_log2() = cu_height_log2;
				mem_store[store_idx].cud() = cud;
				mem_store[store_idx].tree_status() = tree_status;
				mem_store[store_idx].x0() = x0;
				mem_store[store_idx].y0() = y0;
				mem_store[store_idx].part_num() = part_num_child;
				mem_store[store_idx].cup() = cup;
				mem_store[store_idx].pred_mode() = cons_pred_mode;
				mem_store[store_idx].x_pos() = split_struct_child.x_pos[0];
				mem_store[store_idx].y_pos() = split_struct_child.y_pos[0];
				mem_store[store_idx].prev_up_log2_sub_cuw() = pu_in_buf[3].insn.cu_width_log2;
				mem_store[store_idx].flag_chn(0) = 1;
				mem_store[store_idx].flag_pu(KERNEL_16X16) = split_struct_child.split_allow[part_num_child][NO_SPLIT] ? 1 : 0;
				mem_store[store_idx].flag_pu(KERNEL_8X16) = split_struct_child.split_allow[part_num_child][SPLIT_BI_VER] ? 1 : 0;
				mem_store[store_idx].flag_pu(KERNEL_16X8) = split_struct_child.split_allow[part_num_child][SPLIT_BI_HOR] ? 1 : 0;
				mem_store[store_idx].flag_pu(KERNEL_8X8) = split_struct_child.split_allow[part_num_child][SPLIT_QUAD] ? 1 : 0;
				mem_store[store_idx].tag_pu() = tag_pu;
				mem_store[store_idx].tag_chn() = tag_chn;
				++store_idx;
				++tag_pu;
			}

			mem_load[load_idx].opcode() = LOAD_1_BYPASS;
			mem_load[load_idx].cu_width_log2() = 5;
			mem_load[load_idx].cu_height_log2() = 5;
			mem_load[load_idx].flag_chn(0) = 1;
			mem_load[load_idx].tag_chn() = tag_chn;
			mem_load[load_idx].flag_pu(BYPASS + 0) = 1;
			++load_idx;
			++tag_chn;

#if MD_CHANNEL_DEBUG
			mem_load[load_idx].opcode() = LOAD_1_OUT_CHANNEL;
			mem_load[load_idx].cu_width_log2() = 5;
			mem_load[load_idx].cu_height_log2() = 5;
			mem_load[load_idx].flag_chn(0) = 1;
			mem_load[load_idx].tag_chn() = tag_chn;
			mem_load[load_idx].flag_pu(OUTPUT_CHANNEL) = 1;
			++load_idx;
#endif

			mem_pu[pu_idx].opcode() = BYPASS + 0;
			mem_pu[pu_idx].cu_width_log2() = 5;
			mem_pu[pu_idx].cu_height_log2() = 5;
			mem_pu[pu_idx].x0() = pu_in_buf[3].insn.x0;
			mem_pu[pu_idx].y0() = pu_in_buf[3].insn.y0;
			mem_pu[pu_idx].cud() = pu_in_buf[3].insn.cud;
			mem_pu[pu_idx].qtd() = pu_in_buf[3].insn.qt_depth;
			mem_pu[pu_idx].btd() = pu_in_buf[3].insn.bet_depth;
			mem_pu[pu_idx].split_mode() = SPLIT_QUAD;
			mem_pu[pu_idx].tag_pu() = 1;
			++pu_idx;

#if MD_CHANNEL_DEBUG
			mem_pu[pu_idx].opcode() = OUTPUT_CHANNEL;
			mem_pu[pu_idx].cu_width_log2() = 5;
			mem_pu[pu_idx].cu_height_log2() = 5;
			mem_pu[pu_idx].tag_pu() = 1;
#endif

			mem_store[store_idx].opcode() = STORE_N_1;
			mem_store[store_idx].cu_width_log2() = 5;
			mem_store[store_idx].cu_height_log2() = 5;
			mem_store[store_idx].flag_chn(0) = 1;
			mem_store[store_idx].flag_pu(BYPASS + 0) = 1;
			mem_store[store_idx].tag_pu() = 1;
			mem_store[store_idx].tag_chn() = tag_chn;
			mem_store[store_idx].cud() = cud_father;
			mem_store[store_idx].tree_status() = tree_status_father;
			mem_store[store_idx].part_num() = part_num_father;
			mem_store[store_idx].x0() = x0_father;
			mem_store[store_idx].y0() = y0_father;
			mem_store[store_idx].cup() = cup_father;
			mem_store[store_idx].x_pos() = split_struct.x_pos[0];
			mem_store[store_idx].y_pos() = split_struct.y_pos[0];
			mem_store[store_idx].prev_up_log2_sub_cuw() = prev_up_log2_sub_cuw_father;
			mem_store[store_idx].pred_mode() = cons_pred_mode_child[0];
			++store_idx;
		}
	}
	mem_load[load_idx].opcode() = DONE;
	mem_store[store_idx].opcode() = DONE;

	return 0;
}

#if !ONLY_QT_QT
s64 circuit_32x32_top(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	//ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO up_nebs_father[18], ENC_ME_NEB_INFO left_nebs_father[16], //作为顶层函数综合时需要指定数组大小，否则报错
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

	s64 cost[4] = { MAX_S64, MAX_S64, MAX_S64, MAX_S64 };
	U2 tree_status_child[4] = { TREE_LC, TREE_LC, TREE_LC, TREE_LC };
	U2 cons_pred_mode_child[4] = { NO_MODE_CONS, NO_MODE_CONS, NO_MODE_CONS, NO_MODE_CONS };

	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	s64 cost_best = MAX_S64;
	s64 cost_1 = MAX_S64;
	U2 best_tree_status_child = TREE_LC;

	U2 tree_status_temp = tree_status;
	U2 NS_luma_pred_mode = ONLY_INTRA;
	U3 split_mode[4] = { NO_SPLIT, SPLIT_BI_VER, SPLIT_BI_HOR, SPLIT_QUAD };

	S4 best_index = -1;

	if (!(x0 < md_fw_ptr->pic_width && y0 < md_fw_ptr->pic_height)) {
		return 0;
	}

	assert(cu_width == 32 && cu_height == 32 && split_mode_father == SPLIT_QUAD);
	static IP_ARRAY_SIZE<32, 32>		IP_buffer_32x32;
	static IP_MAP_SCU_SIZE<32, 32>		IP_map_scu_32x32;
	static MPM_ARRAY_SIZE<32, 32>		MPM_buffer_32x32;
	static ENC_ME_NEB_INFO				up_nebs[1 + 8 + 1];
	static ENC_ME_NEB_INFO				left_nebs[8];
	static ENC_ME_NEB_INFO				nebs_line_ver[8];
	static ENC_ME_NEB_INFO				nebs_line_hor[8 + 8];
	static ENC_ME_LINE_MAP_SIZE<32>		me_line_map_32;
	static ENC_CU_DATA_ARRAY_SIZE<32>	cu_data_temp[4];
	STATIC_FOR_STACK ENC_CU_DATA_ARRAY			curr_cu_data_best;

#pragma HLS ARRAY_PARTITION variable=cu_data_temp->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->coef_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->reco_y cyclic factor=4 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->split_mode cyclic factor=5 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->split_mode cyclic factor=7 dim=3	
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mpm_ext cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mpm cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->refi cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mv cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->mvd cyclic factor=2 dim=4
#pragma HLS ARRAY_PARTITION variable=cu_data_temp->num_nz_coef cyclic factor=3 dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->num_nz_coef cyclic factor=3 dim=1

#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->num_nz_coef cyclic factor=3 dim=1

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
	for (size_t i = 0; i < 18; i++)
	{
		channel.up_nebs_32x32[i] = up_nebs_father[i];
	}

	for (size_t i = 0; i < 16; i++)
	{
		channel.left_nebs_32x32[i] = left_nebs_father[i];
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

	in_buf.IP_buffer_32x32 = IP_buffer_32x32;
	in_buf.IP_map_scu_32x32 = IP_map_scu_32x32;
	in_buf.MPM_buffer_32x32 = MPM_buffer_32x32;
	in_buf.me_line_map_32 = me_line_map_32;

	load_32x32_top(insn_pu, insn_load, in_buf, channel);
	
	IP_buffer_32x32 = in_buf.IP_buffer_32x32;
	IP_map_scu_32x32 = in_buf.IP_map_scu_32x32;
	MPM_buffer_32x32 = in_buf.MPM_buffer_32x32;
	me_line_map_32 = in_buf.me_line_map_32;

#if !ONLY_QT_QT
#if !KERNEL_SIZE_CLOSE(32, 32)
	if (split_struct.split_allow[part_num][NO_SPLIT])
	{
		pu_in_buf[NO_SPLIT].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[NO_SPLIT].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[NO_SPLIT].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[NO_SPLIT].buffer.skip_in = *skip_in;
		pu_in_buf[NO_SPLIT].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[NO_SPLIT].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;

		pu_in_buf[NO_SPLIT].me_line_map_32 = me_line_map_32;
		for (size_t i = 0; i < 10; i++) {
			pu_in_buf[NO_SPLIT].up_nebs_32[i] = channel.up_nebs[i];
		}
		for (size_t i = 0; i < 8; i++) {
			pu_in_buf[NO_SPLIT].left_nebs_32[i] = channel.left_nebs[i];
		}
		pu_in_buf[NO_SPLIT].insn.cud = cud;
		pu_in_buf[NO_SPLIT].insn.split_mode= NO_SPLIT;
		pu_in_buf[NO_SPLIT].insn.cup = cup;
		pu_in_buf[NO_SPLIT].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[NO_SPLIT].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[NO_SPLIT].insn.cu_width = cu_width;
		pu_in_buf[NO_SPLIT].insn.cu_height = cu_height;
		pu_in_buf[NO_SPLIT].insn.qt_depth = qt_depth;
		pu_in_buf[NO_SPLIT].insn.x0 = x0;
		pu_in_buf[NO_SPLIT].insn.y0 = y0;
		pu_in_buf[NO_SPLIT].insn.qt_depth = qt_depth;
		pu_in_buf[NO_SPLIT].insn.bet_depth = bet_depth;
		pu_in_buf[NO_SPLIT].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[NO_SPLIT].insn.tree_status = tree_status;

		pu_in_buf[NO_SPLIT].IP_buffer_32x32 = IP_buffer_32x32;
		pu_in_buf[NO_SPLIT].MPM_buffer_32x32 = MPM_buffer_32x32;
		pu_in_buf[NO_SPLIT].IP_map_scu_32x32 = IP_map_scu_32x32;

		pu_32x32_top_1(pu_in_buf[NO_SPLIT], cost[0], buffer, pu_out_buf[NO_SPLIT], ref);
#endif // !KERNEL_SIZE_CLOSE(32, 32)
	}

	if (split_struct.split_allow[part_num][SPLIT_BI_VER])
	{
		pu_in_buf[SPLIT_BI_VER].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[SPLIT_BI_VER].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[SPLIT_BI_VER].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[SPLIT_BI_VER].buffer.skip_in = *skip_in;
		pu_in_buf[SPLIT_BI_VER].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[SPLIT_BI_VER].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;

		pu_in_buf[SPLIT_BI_VER].me_line_map_32 = me_line_map_32;
		for (size_t i = 0; i < 10; i++) {
			pu_in_buf[SPLIT_BI_VER].up_nebs_32[i] = channel.up_nebs[i];
		}
		for (size_t i = 0; i < 8; i++) {
			pu_in_buf[SPLIT_BI_VER].left_nebs_32[i] = channel.left_nebs[i];
		}
		pu_in_buf[SPLIT_BI_VER].insn.cud = cud;
		pu_in_buf[SPLIT_BI_VER].insn.split_mode = SPLIT_BI_VER;
		pu_in_buf[SPLIT_BI_VER].insn.cup = cup;
		pu_in_buf[SPLIT_BI_VER].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[SPLIT_BI_VER].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[SPLIT_BI_VER].insn.cu_width = cu_width;
		pu_in_buf[SPLIT_BI_VER].insn.cu_height = cu_height;
		pu_in_buf[SPLIT_BI_VER].insn.qt_depth = qt_depth;
		pu_in_buf[SPLIT_BI_VER].insn.x0 = x0;
		pu_in_buf[SPLIT_BI_VER].insn.y0 = y0;
		pu_in_buf[SPLIT_BI_VER].insn.qt_depth = qt_depth;
		pu_in_buf[SPLIT_BI_VER].insn.bet_depth = bet_depth;
		pu_in_buf[SPLIT_BI_VER].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[SPLIT_BI_VER].insn.tree_status = tree_status;

		pu_in_buf[SPLIT_BI_VER].IP_buffer_32x32 = IP_buffer_32x32;
		pu_in_buf[SPLIT_BI_VER].MPM_buffer_32x32 = MPM_buffer_32x32;
		pu_in_buf[SPLIT_BI_VER].IP_map_scu_32x32 = IP_map_scu_32x32;

		pu_32x32_top_1(pu_in_buf[SPLIT_BI_VER], cost[1], buffer, pu_out_buf[SPLIT_BI_VER], ref);
	}

	if (split_struct.split_allow[part_num][SPLIT_BI_HOR])
	{
		pu_in_buf[SPLIT_BI_HOR].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[SPLIT_BI_HOR].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[SPLIT_BI_HOR].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[SPLIT_BI_HOR].buffer.skip_in = *skip_in;
		pu_in_buf[SPLIT_BI_HOR].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[SPLIT_BI_HOR].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;

		pu_in_buf[SPLIT_BI_HOR].me_line_map_32 = me_line_map_32;
		for (size_t i = 0; i < 10; i++) {
			pu_in_buf[SPLIT_BI_HOR].up_nebs_32[i] = channel.up_nebs[i];
		}
		for (size_t i = 0; i < 8; i++) {
			pu_in_buf[SPLIT_BI_HOR].left_nebs_32[i] = channel.left_nebs[i];
		}
		pu_in_buf[SPLIT_BI_HOR].insn.cud = cud;
		pu_in_buf[SPLIT_BI_HOR].insn.split_mode = SPLIT_BI_HOR;
		pu_in_buf[SPLIT_BI_HOR].insn.cup = cup;
		pu_in_buf[SPLIT_BI_HOR].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[SPLIT_BI_HOR].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[SPLIT_BI_HOR].insn.cu_width = cu_width;
		pu_in_buf[SPLIT_BI_HOR].insn.cu_height = cu_height;
		pu_in_buf[SPLIT_BI_HOR].insn.qt_depth = qt_depth;
		pu_in_buf[SPLIT_BI_HOR].insn.x0 = x0;
		pu_in_buf[SPLIT_BI_HOR].insn.y0 = y0;
		pu_in_buf[SPLIT_BI_HOR].insn.qt_depth = qt_depth;
		pu_in_buf[SPLIT_BI_HOR].insn.bet_depth = bet_depth;
		pu_in_buf[SPLIT_BI_HOR].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[SPLIT_BI_HOR].insn.tree_status = tree_status;

		pu_in_buf[SPLIT_BI_HOR].IP_buffer_32x32 = IP_buffer_32x32;
		pu_in_buf[SPLIT_BI_HOR].MPM_buffer_32x32 = MPM_buffer_32x32;
		pu_in_buf[SPLIT_BI_HOR].IP_map_scu_32x32 = IP_map_scu_32x32;

		pu_32x32_top_1(pu_in_buf[SPLIT_BI_HOR], cost[2], buffer, pu_out_buf[SPLIT_BI_HOR], ref);
	}
#endif

	if (split_struct.split_allow[part_num][SPLIT_QUAD])
	{
		pu_in_buf[SPLIT_QUAD].config.md_fw_ptr = *md_fw_ptr;
		pu_in_buf[SPLIT_QUAD].config.md_input_ptr = *md_input_ptr;
		pu_in_buf[SPLIT_QUAD].buffer.me_mv_dir = *me_mv_dir;
		pu_in_buf[SPLIT_QUAD].buffer.skip_in = *skip_in;
		pu_in_buf[SPLIT_QUAD].buffer.fme_mv_info = *fme_mv_info;
		pu_in_buf[SPLIT_QUAD].buffer.Fetch_Ref_window_ptr = *Fetch_Ref_window_ptr;

		pu_in_buf[SPLIT_QUAD].me_line_map_32 = me_line_map_32;
		for (size_t i = 0; i < 10; i++) {
			pu_in_buf[SPLIT_QUAD].up_nebs_32[i] = channel.up_nebs[i];
		}
		for (size_t i = 0; i < 8; i++) {
			pu_in_buf[SPLIT_QUAD].left_nebs_32[i] = channel.left_nebs[i];
		}
		pu_in_buf[SPLIT_QUAD].insn.cud = cud;
		pu_in_buf[SPLIT_QUAD].insn.split_mode = SPLIT_QUAD;
		pu_in_buf[SPLIT_QUAD].insn.cup = cup;
		pu_in_buf[SPLIT_QUAD].insn.cu_height_log2 = cu_height_log2;
		pu_in_buf[SPLIT_QUAD].insn.cu_width_log2 = cu_width_log2;
		pu_in_buf[SPLIT_QUAD].insn.cu_width = cu_width;
		pu_in_buf[SPLIT_QUAD].insn.cu_height = cu_height;
		pu_in_buf[SPLIT_QUAD].insn.qt_depth = qt_depth;
		pu_in_buf[SPLIT_QUAD].insn.x0 = x0;
		pu_in_buf[SPLIT_QUAD].insn.y0 = y0;
		pu_in_buf[SPLIT_QUAD].insn.qt_depth = qt_depth;
		pu_in_buf[SPLIT_QUAD].insn.bet_depth = bet_depth;
		pu_in_buf[SPLIT_QUAD].insn.cons_pred_mode = cons_pred_mode;
		pu_in_buf[SPLIT_QUAD].insn.tree_status = tree_status;

		pu_in_buf[SPLIT_QUAD].IP_buffer_32x32 = IP_buffer_32x32;
		pu_in_buf[SPLIT_QUAD].MPM_buffer_32x32 = MPM_buffer_32x32;
		pu_in_buf[SPLIT_QUAD].IP_map_scu_32x32 = IP_map_scu_32x32;

		pu_32x32_top_0(pu_in_buf[SPLIT_QUAD], cost[3], buffer, pu_out_buf[SPLIT_QUAD], ref);
	}

	insn_store.split_mode[0] = NO_SPLIT;
	insn_store.split_mode[1] = SPLIT_BI_VER;
	insn_store.split_mode[2] = SPLIT_BI_HOR;
	insn_store.split_mode[3] = SPLIT_QUAD;
	insn_store.cost_temp[0] = cost[0];
	insn_store.cost_temp[1] = cost[1];
	insn_store.cost_temp[2] = cost[2];
	insn_store.cost_temp[3] = cost[3];
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

	config.md_fw_ptr = *md_fw_ptr;

	channel.IP_buffer_32x32 = IP_buffer_32x32;
	channel.IP_map_scu_32x32 = IP_map_scu_32x32;
	channel.MPM_buffer_32x32 = MPM_buffer_32x32;
	channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2] = MAX_S64;
	channel.me_line_map_32 = me_line_map_32;
	channel.IP_map_scu_ptr_64x64 = *IP_map_scu_ptr_64x64;
	channel.prev_cu_data_ptr_32x32 = *prev_cu_data_ptr;

	insn_store.cu_width_log2 = cu_width_log2;
	insn_store.cu_height_log2 = cu_height_log2;
	insn_store.cud = cud;
	insn_store.tree_status = tree_status;
	insn_store.num_split_to_try = num_split_to_try;

	for (size_t i = 0; i < 4; i++) {
		insn_store.cons_pred_mode_child[i] = NO_MODE_CONS;
		insn_store.tree_status_child[i] = TREE_LC;
	}

	cost_best = store_32x32_top(insn_store, channel, pu_out_buf, config);

	me_line_map_32 = channel.me_line_map_32;
	*prev_cu_data_ptr = channel.prev_cu_data_ptr_32x32;
	IP_buffer_32x32 = channel.IP_buffer_32x32;
	MPM_buffer_32x32 = channel.MPM_buffer_32x32;
	*IP_map_scu_ptr_64x64 = channel.IP_map_scu_ptr_64x64;

	for (size_t i = 0; i < 8; i++)
	{
		nebs_line_ver[i] = channel.nebs_line_ver_32x32[i];
	}
	for (size_t i = 0; i < 16; i++)
	{
		nebs_line_hor[i] = channel.nebs_line_hor_32x32[i];
	}

#ifdef DEBUG_PRINT_MD_COST
	PRINT_MD_COST
#endif
	return cost_best > MAX_S64 ? MAX_S64 : cost_best;
}
#endif
#endif

int copy_cu_data_array_src_top(ENC_CU_DATA *dst, ENC_CU_DATA_ARRAY *src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status)
{
	U8 i, ii, i1, i2;
	U8 j;
	U8 k;
	U8 l;
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
	cy =( y >> 2);
	cu_width = (1 << cu_width_log2);
	cu_height = (1 << cu_height_log2);
	cus = 1 << log2_cus;
	cuw_scu = 1 << (cu_width_log2 - 2);
	cuh_scu = 1 << (cu_height_log2 - 2);
	cus_scu = 1 << (log2_cus - 2);
#if COPY_CU_DATA_ARRAY_SRC_TOP
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->split_mode cyclic factor=7 dim=2	
	#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	

	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=5 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->split_mode cyclic factor=7 dim=2	
	#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1	
	#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3		
#endif
	for (j = 0; j < cuh_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		idx_dst = (U22)((cy + j) << (log2_cus - 2)) + cx;
		idx_src = (U22)j << (cu_width_log2 - 2);
		for (ii = 0; ii < cuw_scu; ii++){
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE II=1
			for (k = 0; k < 5; k++) {
#pragma HLS LOOP_TRIPCOUNT max=5
				if(k >= cud)
				{
					for (i = 0; i < NUM_BLOCK_SHAPE; i++) {
						(dst->split_mode[k][i] + idx_dst)[ii] = (src->split_mode[k][i] + idx_src)[ii];
					}
				}
			}
			(dst->pred_mode + idx_dst)[ii] = (src->pred_mode + idx_src)[ii];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[ii] = (src->cu_luma_qp + idx_src)[ii];
#endif
			(dst->mpm[0] + idx_dst)[ii] = (src->mpm[0] + idx_src)[ii];
			(dst->mpm[1] + idx_dst)[ii] = (src->mpm[1] + idx_src)[ii];
			(dst->ipm[0] + idx_dst)[ii] = (src->ipm[0] + idx_src)[ii];
			//(dst->ipm[1] + idx_dst)[ii] = (src->ipm[1] + idx_src)[ii];
			for (i = 0; i < 8; i++) {
				(dst->mpm_ext[i] + idx_dst)[ii] = (src->mpm_ext[i] + idx_src)[ii];
			}
			(dst->affine_flag + idx_dst)[ii] = (src->affine_flag + idx_src)[ii];
			(dst->smvd_flag + idx_dst)[ii] = (src->smvd_flag + idx_src)[ii];
			(dst->depth + idx_dst)[ii] = (src->depth + idx_src)[ii];
			(dst->map_scu + idx_dst)[ii] = (src->map_scu + idx_src)[ii];
			(dst->map_cu_mode + idx_dst)[ii] = (src->map_cu_mode + idx_src)[ii];
			(dst->map_pb_tb_part + idx_dst)[ii] = (src->map_pb_tb_part + idx_src)[ii];
			for (i = 0; i < 2; i++){
				(dst->refi + idx_dst)[ii][i] = (src->refi + idx_src)[ii][i];
			}
			(dst->umve_flag + idx_dst)[ii] = (src->umve_flag + idx_src)[ii];
			(dst->umve_idx + idx_dst)[ii] = (src->umve_idx + idx_src)[ii];
			(dst->skip_idx + idx_dst)[ii] = (src->skip_idx + idx_src)[ii];
			(dst->mvr_idx + idx_dst)[ii] = (src->mvr_idx + idx_src)[ii];
			(dst->mvp_from_hmvp_flag + idx_dst)[ii] = (src->mvp_from_hmvp_flag + idx_src)[ii];
			(dst->ipf_flag + idx_dst)[ii] = (src->ipf_flag + idx_src)[ii];
			for (i1 = 0; i1 < 2; i1++){
				for (i2 = 0; i2 < 2; i2++){
					(dst->mv + idx_dst)[ii][i1][i2] = (src->mv + idx_src)[ii][i1][i2];
					(dst->mvd + idx_dst)[ii][i1][i2] = (src->mvd + idx_src)[ii][i1][i2];
				}

			}
			for (k = 0; k < 1; k++) {
				(dst->num_nz_coef[k] + idx_dst)[ii] = (src->num_nz_coef[k] + idx_src)[ii];
			}
			(dst->pb_part + idx_dst)[ii] = (src->pb_part + idx_src)[ii];
			(dst->tb_part + idx_dst)[ii] = (src->tb_part + idx_src)[ii];
			for(l = 0; l < 4; l++)
			{
				for(k = 0; k < 4; k++)
				{
					dst->coef_y[y + j*4+l][x + ii*4+k] = src->coef_y[j*4+l][ii*4+k];
					dst->reco_y[y + j*4+l][x + ii*4+k] = src->reco_y[j*4+l][ii*4+k];
				}
			}
		}
	}

	return 0;
}



void cost_compare_fixed_point_top_new(s64 cost_temp, ENC_CU_DATA_ARRAY *curr_cu_data_temp_array_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, MD_RESULT*  md_best_result_ptr, U2 cons_pred_mode_child, ENC_CU_DATA *md_best_ctu_output_ptr)
{
	if (md_best_result_ptr->cost_best > cost_temp) {
		/* backup the current best data */
		copy_cu_data_array_src_top(md_best_ctu_output_ptr,
			curr_cu_data_temp_array_ptr, 0, 0, cu_width_log2, cu_height_log2,
			cu_width_log2, cud, tree_status);
		md_best_result_ptr->cost_best = cost_temp;
		md_best_result_ptr->best_split_mode = split_mode;
		md_best_result_ptr->best_cons_pred_mode = cons_pred_mode_child;
	}
}


