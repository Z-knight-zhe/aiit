#include "com_def.h"
#include <math.h>
#include <stdlib.h>
#include "md1.h"
#pragma warning(disable:4477)
extern int op_patch_mode_on;
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

static void skip_element_skip_idx(SKIP_ELEMENT * mode, const U1 valid, const U4 skip_idx)
{
#pragma HLS INLINE
	mode->valid = valid;
	mode->skip_flag = 1;
	mode->skip_idx = skip_idx;
	mode->umve_flag = 0;
	mode->umve_idx = 0;
	mode->affine_flag = 0;
}
static void skip_element_umve_idx(SKIP_ELEMENT * mode, const U1 valid, const U6 umve_idx)
{
#pragma HLS INLINE
	mode->valid = valid;
	mode->skip_flag = 1;
	mode->skip_idx = 0;
	mode->umve_flag = 1;
	mode->umve_idx = umve_idx;
	mode->affine_flag = 0;
}
void skip_element_copy(SKIP_ELEMENT * dst, const SKIP_ELEMENT * src)
{
#pragma HLS INLINE
/*
#pragma HLS   ARRAY_PARTITION variable=dst->mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=dst->refi complete dim=0
*/
	dst->valid = src->valid;
	dst->skip_flag = src->skip_flag;
	dst->skip_idx = src->skip_idx;
	dst->umve_flag = src->umve_flag;
	dst->umve_idx = src->umve_idx;
	dst->affine_flag = src->affine_flag;
	dst->mvp[REFP_0][MV_X] = src->mvp[REFP_0][MV_X];
	dst->mvp[REFP_0][MV_Y] = src->mvp[REFP_0][MV_Y];
	dst->refi[REFP_0] = src->refi[REFP_0];
	dst->mvp[REFP_1][MV_X] = src->mvp[REFP_1][MV_X];
	dst->mvp[REFP_1][MV_Y] = src->mvp[REFP_1][MV_Y];
	dst->refi[REFP_1] = src->refi[REFP_1];
}

U1 is_same_skip_motion(const s32 mvp1[REFP_NUM][MV_D], const s32 refi1[REFP_NUM], const s32 mvp2[REFP_NUM][MV_D], const s32 refi2[REFP_NUM])
{
#pragma HLS INLINE
	return REFI_IS_VALID(refi1[REFP_0])
		&& refi1[REFP_0] == refi2[REFP_0]
		&& mvp1[REFP_0][MV_X] == mvp2[REFP_0][MV_X]
		&& mvp1[REFP_0][MV_Y] == mvp2[REFP_0][MV_Y]
#if ENABLE_BFRAME
		//&& REFI_IS_VALID(refi1[REFP_1])
		&& refi1[REFP_1] == refi2[REFP_1]
		&& mvp1[REFP_1][MV_X] == mvp2[REFP_1][MV_X]
		&& mvp1[REFP_1][MV_Y] == mvp2[REFP_1][MV_Y]
#endif
		;
}
static BOOL is_hmvp_duplicated(const COM_MOTION * motion_cands, const SKIP_ELEMENT * tmvp, const SKIP_ELEMENT * spat)
{
#pragma HLS INLINE OFF
	return (tmvp->valid && REFI_IS_VALID(motion_cands->ref_idx[REFP_0]) && REFI_IS_VALID(tmvp->refi[REFP_0]) && motion_cands->mv[REFP_0][MV_X] == tmvp->mvp[REFP_0][MV_X] && motion_cands->mv[REFP_0][MV_Y] == tmvp->mvp[REFP_0][MV_Y])
		|| (spat->valid && REFI_IS_VALID(motion_cands->ref_idx[REFP_0]) && REFI_IS_VALID(spat->refi[REFP_0]) && motion_cands->mv[REFP_0][MV_X] == spat->mvp[REFP_0][MV_X] && motion_cands->mv[REFP_0][MV_Y] == spat->mvp[REFP_0][MV_Y]);

}
static S4 get_first_valid_neb(const U1 valid[NUM_SKIP_SPATIAL_MV], const S4 num)
{
#pragma HLS INLINE
#pragma HLS function_instantiate variable=num
#if MD_PRINT
	assert(NUM_SKIP_SPATIAL_MV == 6);
#endif
	return num == -1 ? (S4)(-1) :
		(num >= NUM_SKIP_SPATIAL_MV ? (S4)(-1) : (valid[num] ? (S4)num :
		(num + 1 >= NUM_SKIP_SPATIAL_MV ?(S4)(-1) : (valid[num + 1] ? (S4)(num + 1):
		(num + 2 >= NUM_SKIP_SPATIAL_MV ?(S4)(-1) : (valid[num + 2] ? (S4)(num + 2):
		(num + 3 >= NUM_SKIP_SPATIAL_MV ?(S4)(-1) : (valid[num + 3] ? (S4)(num + 3):
		(num + 4 >= NUM_SKIP_SPATIAL_MV ?(S4)(-1) : (valid[num + 4] ? (S4)(num + 4):
		(num + 5 >= NUM_SKIP_SPATIAL_MV ?(S4)(-1) : (valid[num + 5] ? (S4)(num + 5): (S4)(-1)))))))))))));
}
void derive_neb_cu(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height, 
	NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], 
	const ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], 
	const U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], 
	const U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], 
	ENC_ME_NEB_INFO above_nebs_line[MAX_CU_IN_SCU + 2], 
	U32 map_scu_above_line[MAX_CU_IN_SCU + 2], 
	U32 map_cu_mode_above_line[MAX_CU_IN_SCU + 2], 
	const U1 curr_ctu_idx)
{
#pragma HLS INLINE OFF

	U5 cu_width_in_scu = cu_width >> MIN_CU_LOG2;
	U5 cu_height_in_scu = cu_height >> MIN_CU_LOG2;
	
	U5 x_scu_of_ctu = x_scu & (MAX_CU_IN_SCU - 1);
	U5 y_scu_of_ctu = y_scu & (MAX_CU_IN_SCU - 1);

	neb[0].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[0].y_scu_of_ctu = y_scu_of_ctu + cu_height_in_scu - 1;
	neb[1].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu - 1;
	neb[1].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[2].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu;
	neb[2].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[3].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[3].y_scu_of_ctu = y_scu_of_ctu;
	neb[4].x_scu_of_ctu = x_scu_of_ctu;
	neb[4].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[5].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[5].y_scu_of_ctu = y_scu_of_ctu - 1;


	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; i++){
#pragma HLS UNROLL
		S6 x = neb[i].x_scu_of_ctu;
		S6 y = neb[i].y_scu_of_ctu;
#if MD_PRINT
		assert(x >= -1 && x <= MAX_CU_IN_SCU && y >= -1 && y < MAX_CU_IN_SCU);
#endif
		if (neb[i].y_scu_of_ctu == -1){ // top, top left, top right
			neb_map[i].map_mv[REFP_0][MV_X] = above_nebs_line[x + 1].mv[REFP_0][MV_X];
			neb_map[i].map_mv[REFP_0][MV_Y] = above_nebs_line[x + 1].mv[REFP_0][MV_Y];
			neb_map[i].map_refi[REFP_0] = above_nebs_line[x + 1].refi[REFP_0];
			neb_map[i].map_mv[REFP_1][MV_X] = above_nebs_line[x + 1].mv[REFP_1][MV_X];
			neb_map[i].map_mv[REFP_1][MV_Y] = above_nebs_line[x + 1].mv[REFP_1][MV_Y];
			neb_map[i].map_refi[REFP_1] = above_nebs_line[x + 1].refi[REFP_1];
			neb_map[i].map_scu = map_scu_above_line[x + 1];
			neb_map[i].map_cu_mode = map_cu_mode_above_line[x + 1];
		}
		else if (x == MAX_CU_IN_SCU){ // right
			neb_map[i].map_mv[REFP_0][MV_X] = 0;
			neb_map[i].map_mv[REFP_0][MV_Y] = 0;
			neb_map[i].map_refi[REFP_0] = REFI_INVALID;
			neb_map[i].map_mv[REFP_1][MV_X] = 0;
			neb_map[i].map_mv[REFP_1][MV_Y] = 0;
			neb_map[i].map_refi[REFP_1] = REFI_INVALID;
			neb_map[i].map_scu = 0;
			neb_map[i].map_cu_mode = 0;
		}
		else if (x == -1){ // left
			neb_map[i].map_mv[REFP_0][MV_X] = inner_nebs[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y].mv[REFP_0][MV_X];
			neb_map[i].map_mv[REFP_0][MV_Y] = inner_nebs[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y].mv[REFP_0][MV_Y];
			neb_map[i].map_refi[REFP_0] = inner_nebs[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y].refi[REFP_0];
			neb_map[i].map_mv[REFP_1][MV_X] = inner_nebs[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y].mv[REFP_1][MV_X];
			neb_map[i].map_mv[REFP_1][MV_Y] = inner_nebs[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y].mv[REFP_1][MV_Y];
			neb_map[i].map_refi[REFP_1] = inner_nebs[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y].refi[REFP_1];
			neb_map[i].map_scu = map_scu_inner[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y];
			neb_map[i].map_cu_mode = map_cu_mode_inner[1 - curr_ctu_idx][SCU_IN_LCU_NUMS_LEN - 1][y];
		}
		else{
			neb_map[i].map_mv[REFP_0][MV_X] = inner_nebs[curr_ctu_idx][x][y].mv[REFP_0][MV_X];
			neb_map[i].map_mv[REFP_0][MV_Y] = inner_nebs[curr_ctu_idx][x][y].mv[REFP_0][MV_Y];
			neb_map[i].map_refi[REFP_0] = inner_nebs[curr_ctu_idx][x][y].refi[REFP_0];
			neb_map[i].map_mv[REFP_1][MV_X] = inner_nebs[curr_ctu_idx][x][y].mv[REFP_1][MV_X];
			neb_map[i].map_mv[REFP_1][MV_Y] = inner_nebs[curr_ctu_idx][x][y].mv[REFP_1][MV_Y];
			neb_map[i].map_refi[REFP_1] = inner_nebs[curr_ctu_idx][x][y].refi[REFP_1];
			neb_map[i].map_scu = map_scu_inner[curr_ctu_idx][x][y];
			neb_map[i].map_cu_mode = map_cu_mode_inner[curr_ctu_idx][x][y];
		}
	}

	neb[0].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[0].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[0].map_scu);
	neb[1].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[1].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[1].map_scu);
	neb[2].valid_flag = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu && MCU_GET_CODED_FLAG(neb_map[2].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[2].map_scu);
	neb[3].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[3].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[3].map_scu);
	neb[4].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[4].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[4].map_scu);
	neb[5].valid_flag = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[5].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[5].map_scu);
}

void get_colocal_scu_coord(int bx, int by, int *xpos, int *ypos, int pic_width_in_scu, int pic_height_in_scu)
{
	const int mask = -1 ^ 3;
	*xpos = (bx & mask) + 2;
	*ypos = (by & mask) + 2;
	if (*ypos >= pic_height_in_scu) {
		*ypos = (by & mask) + pic_height_in_scu >> 1;
	}
	if (*xpos >= pic_width_in_scu) {
		*xpos = (bx & mask) + pic_width_in_scu >> 1;
	}
	*xpos &= (1 << 4) - 1;
	*ypos &= (1 << 4) - 1;
}

static U4 get_colocal_scu_len(const U10 cur, const U10 pic_size_in_scu)
{
#pragma HLS INLINE
	const U10 mask = (-1) ^ 3;
	const U10 col = (cur & mask) + 2;
	const U10 result = col >= pic_size_in_scu ? (U10)((cur & mask) + pic_size_in_scu) >> 1 : col;
	return result & (MAX_CU_IN_SCU - 1);
}
#if ENABLE_BFRAME
void get_col_mv_from_list0(s32 mvp[MV_D], const SKIP_MERGE_INPUT* input, U10 co_x_scu_of_ctu, U10 co_y_scu_of_ctu)
{
	s32 mvc[MV_D];
	mvc[MV_X] = input->refp_map_mv1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_X];
	mvc[MV_Y] = input->refp_map_mv1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_Y];
	int ptr_col = input->refp_ptr1[0][REFP_0];
	int ptr_col_ref = input->refp_list_ptr1[REFP_0][input->refp_map_refi1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0]];
#if !ENABLE_BFRAME
	assert(input->ptr - input->refp_ptr1[0][REFP_0] == 1);
	assert(ptr_col - ptr_col_ref == 1);
#endif
	scaling_mv1(input->ptr, input->refp_ptr1[0][REFP_0], ptr_col, ptr_col_ref, mvc, mvp);
}
void get_col_mv_from_list0_16(s32 mvp[MV_D], const SKIP_MERGE_INPUT_16* skip_in_16, U10 co_x_scu_of_ctu, U10 co_y_scu_of_ctu)
{
	s32 mvc[MV_D];
	mvc[MV_X] = skip_in_16->refp_map_mv[REFP_0][MV_X];
	mvc[MV_Y] = skip_in_16->refp_map_mv[REFP_0][MV_Y];
	int ptr_col = skip_in_16->refp_ptr1[0][REFP_0];
	int ptr_col_ref = skip_in_16->refp_list_ptr1[REFP_0][skip_in_16->refp_map_refi[REFP_0][REFP_0]];
#if !ENABLE_BFRAME
	assert(input->ptr - input->refp_ptr1[0][REFP_0] == 1);
	assert(ptr_col - ptr_col_ref == 1);
#endif

	scaling_mv1(skip_in_16->ptr, skip_in_16->refp_ptr1[0][REFP_0], ptr_col, ptr_col_ref, mvc, mvp);
}

void get_col_mv_from_list1(s32 mvp[REFP_NUM][MV_D], const SKIP_MERGE_INPUT* input, int co_x_scu_of_ctu, int co_y_scu_of_ctu)
{
	s32 mvc[MV_D];
	mvc[MV_X] = input->refp_map_mv1[REFP_1][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_X];
	mvc[MV_Y] = input->refp_map_mv1[REFP_1][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_Y];
	int ptr_col = input->refp_ptr1[0][REFP_1];
	int ptr_col_ref = input->refp_list_ptr1[REFP_1][input->refp_map_refi1[REFP_1][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0]];
	scaling_mv1(input->ptr, input->refp_ptr1[0][REFP_0], ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
	scaling_mv1(input->ptr, input->refp_ptr1[0][REFP_1], ptr_col, ptr_col_ref, mvc, mvp[REFP_1]);
}
void get_col_mv_from_list1_16(s32 mvp[REFP_NUM][MV_D], const SKIP_MERGE_INPUT_16* skip_in_16, int co_x_scu_of_ctu, int co_y_scu_of_ctu)
{
	s32 mvc[MV_D];
	mvc[MV_X] = skip_in_16->refp_map_mv[REFP_1][MV_X];
	mvc[MV_Y] = skip_in_16->refp_map_mv[REFP_1][MV_Y];
	int ptr_col = skip_in_16->refp_ptr1[0][REFP_1];
	int ptr_col_ref = skip_in_16->refp_list_ptr1[REFP_1][skip_in_16->refp_map_refi[REFP_1][REFP_0]];
	scaling_mv1(skip_in_16->ptr, skip_in_16->refp_ptr1[0][REFP_0], ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
	scaling_mv1(skip_in_16->ptr, skip_in_16->refp_ptr1[0][REFP_1], ptr_col, ptr_col_ref, mvc, mvp[REFP_1]);
}

void com_get_mvp_default_core(s32* refi, s32 mvp[MV_D], const SKIP_MERGE_INPUT* input, NEB_CU* neb, SKIP_MERGE_MAP_SCU* neb_map, int lidx)
{
	int ptr_cur_ref = input->refp_ptr1[0][lidx]; // 2?????
	if (neb->valid_flag && REFI_IS_VALID(neb_map->map_refi[lidx]))
	{
		*refi = neb_map->map_refi[lidx]; // ?¨¤¨¢¨²?¨¦2?????
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
		if (refp[refi[cnt]][lidx].is_library_picture || refp[cur_refi][lidx].is_library_picture)
		{
			refi[cnt] = REFI_INVALID;
			MVPs[cnt][MV_X] = 0;
			MVPs[cnt][MV_Y] = 0;
		}
		else
#endif
		{
			int ptr_neb_ref = input->refp_ptr1[*refi][lidx]; // ?¨¤¨¢¨²?¨¦2?????
			scaling_mv1(input->ptr, ptr_cur_ref, input->ptr, ptr_neb_ref, neb_map->map_mv[lidx], mvp);
		}
	}
	else
	{
		*refi = REFI_INVALID;
		mvp[MV_X] = 0;
		mvp[MV_Y] = 0;
	}
}
void com_get_mvp_default_core_16(s8* refi, s32 mvp[MV_D], const SKIP_MERGE_INPUT_16* skip_in_16, NEB_CU* neb, SKIP_MERGE_MAP_SCU* neb_map, int lidx)
{
	int ptr_cur_ref = skip_in_16->refp_ptr1[0][lidx]; // 2?????
	if (neb->valid_flag && REFI_IS_VALID(neb_map->map_refi[lidx]))
	{
		*refi = neb_map->map_refi[lidx]; // ?¨¤¨¢¨²?¨¦2?????
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
		if (refp[refi[cnt]][lidx].is_library_picture || refp[cur_refi][lidx].is_library_picture)
		{
			refi[cnt] = REFI_INVALID;
			MVPs[cnt][MV_X] = 0;
			MVPs[cnt][MV_Y] = 0;
		}
		else
#endif
		{
			int ptr_neb_ref = skip_in_16->refp_ptr1[*refi][lidx]; // ?¨¤¨¢¨²?¨¦2?????
			scaling_mv1(skip_in_16->ptr, ptr_cur_ref, skip_in_16->ptr, ptr_neb_ref, neb_map->map_mv[lidx], mvp);
		}
	}
	else
	{
		*refi = REFI_INVALID;
		mvp[MV_X] = 0;
		mvp[MV_Y] = 0;
	}
}

void com_get_mvp_default(s32 mvp[MV_D], const SKIP_MERGE_INPUT* input, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], u8 lidx)
{
	s32 refi[NUM_AVS2_SPATIAL_MV];
	s32 MVPs[NUM_AVS2_SPATIAL_MV][MV_D];
#pragma HLS PIPELINE
#pragma HLS   ARRAY_PARTITION variable=refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=MVPs complete dim=0
//#pragma HLS   ARRAY_PARTITION variable=input complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb_map complete dim=0
	com_get_mvp_default_core(refi, MVPs[0], input, neb + 3, neb_map + 3, lidx);
	com_get_mvp_default_core(refi + 1, MVPs[1], input, neb + 4, neb_map + 4, lidx);
	if (neb[2].valid_flag && REFI_IS_VALID(neb_map[2].map_refi[lidx]))
	{
		com_get_mvp_default_core(refi + 2, MVPs[2], input, neb + 2, neb_map + 2, lidx);
	}
	else
	{
		com_get_mvp_default_core(refi + 2, MVPs[2], input, neb + 5, neb_map + 5, lidx);
	}

	int rFrameL = refi[0];
	int rFrameU = refi[1];
	int rFrameUR = refi[2];
	int mvPredType = MVPRED_xy_MIN;
	if ((rFrameL != REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR == REFI_INVALID))
	{
		mvPredType = MVPRED_L;
	}
	else if ((rFrameL == REFI_INVALID) && (rFrameU != REFI_INVALID) && (rFrameUR == REFI_INVALID))
	{
		mvPredType = MVPRED_U;
	}
	else if ((rFrameL == REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR != REFI_INVALID))
	{
		mvPredType = MVPRED_UR;
	}

	for (int hv = 0; hv < MV_D; hv++)
	{
#pragma HLS	UNROLL
		s32 mva = (s32)MVPs[0][hv], mvb = (s32)MVPs[1][hv], mvc = (s32)MVPs[2][hv];
		switch (mvPredType)
		{
		case MVPRED_xy_MIN: // ?¨°¦Ì?2?¡Àe¡Á?D?¦Ì?2??mv¡Á¡Â?amvp
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0))
			{
				mvp[hv] = (s16)((mvb + mvc) / 2);
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0))
			{
				mvp[hv] = (s16)((mvc + mva) / 2);
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0))
			{
				mvp[hv] = (s16)((mva + mvb) / 2);
			}
			else
			{
				s32 mva_ext = abs(mva - mvb);
				s32 mvb_ext = abs(mvb - mvc);
				s32 mvc_ext = abs(mvc - mva);
				s32 pred_vec = COM_MIN(mva_ext, COM_MIN(mvb_ext, mvc_ext));
				if (pred_vec == mva_ext)
				{
					mvp[hv] = (s16)((mva + mvb) / 2);
				}
				else if (pred_vec == mvb_ext)
				{
					mvp[hv] = (s16)((mvb + mvc) / 2);
				}
				else
				{
					mvp[hv] = (s16)((mvc + mva) / 2);
				}
			}
			break;
		case MVPRED_L:
			mvp[hv] = (s16)mva;
			break;
		case MVPRED_U:
			mvp[hv] = (s16)mvb;
			break;
		case MVPRED_UR:
			mvp[hv] = (s16)mvc;
			break;
		default:
			assert(0);
			break;
		}
	}

	// clip MVP after rounding (rounding process might result in 32768)
	int mvp_x, mvp_y;
	com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, 0, 0);
	mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
	mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}
void com_get_mvp_default_16(s32 mvp[MV_D], const SKIP_MERGE_INPUT_16* skip_in_16, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], u8 lidx)
{
	s8 refi[NUM_AVS2_SPATIAL_MV];
	s32 MVPs[NUM_AVS2_SPATIAL_MV][MV_D];
#pragma HLS PIPELINE
#pragma HLS   ARRAY_PARTITION variable=refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=MVPs complete dim=0
	//#pragma HLS   ARRAY_PARTITION variable=input complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb_map complete dim=0
	com_get_mvp_default_core_16(refi, MVPs[0], skip_in_16, neb + 3, neb_map + 3, lidx);
	com_get_mvp_default_core_16(refi + 1, MVPs[1], skip_in_16, neb + 4, neb_map + 4, lidx);
	if (neb[2].valid_flag && REFI_IS_VALID(neb_map[2].map_refi[lidx]))
	{
		com_get_mvp_default_core_16(refi + 2, MVPs[2], skip_in_16, neb + 2, neb_map + 2, lidx);
	}
	else
	{
		com_get_mvp_default_core_16(refi + 2, MVPs[2], skip_in_16, neb + 5, neb_map + 5, lidx);
	}

	int rFrameL = refi[0];
	int rFrameU = refi[1];
	int rFrameUR = refi[2];
	int mvPredType = MVPRED_xy_MIN;
	if ((rFrameL != REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR == REFI_INVALID))
	{
		mvPredType = MVPRED_L;
	}
	else if ((rFrameL == REFI_INVALID) && (rFrameU != REFI_INVALID) && (rFrameUR == REFI_INVALID))
	{
		mvPredType = MVPRED_U;
	}
	else if ((rFrameL == REFI_INVALID) && (rFrameU == REFI_INVALID) && (rFrameUR != REFI_INVALID))
	{
		mvPredType = MVPRED_UR;
	}

	for (int hv = 0; hv < MV_D; hv++)
	{
#pragma HLS	UNROLL
		s32 mva = (s32)MVPs[0][hv], mvb = (s32)MVPs[1][hv], mvc = (s32)MVPs[2][hv];
		switch (mvPredType)
		{
		case MVPRED_xy_MIN: // ?¨°¦Ì?2?¡Àe¡Á?D?¦Ì?2??mv¡Á¡Â?amvp
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0))
			{
				mvp[hv] = (s16)((mvb + mvc) / 2);
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0))
			{
				mvp[hv] = (s16)((mvc + mva) / 2);
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0))
			{
				mvp[hv] = (s16)((mva + mvb) / 2);
			}
			else
			{
				s32 mva_ext = abs(mva - mvb);
				s32 mvb_ext = abs(mvb - mvc);
				s32 mvc_ext = abs(mvc - mva);
				s32 pred_vec = COM_MIN(mva_ext, COM_MIN(mvb_ext, mvc_ext));
				if (pred_vec == mva_ext)
				{
					mvp[hv] = (s16)((mva + mvb) / 2);
				}
				else if (pred_vec == mvb_ext)
				{
					mvp[hv] = (s16)((mvb + mvc) / 2);
				}
				else
				{
					mvp[hv] = (s16)((mvc + mva) / 2);
				}
			}
			break;
		case MVPRED_L:
			mvp[hv] = (s16)mva;
			break;
		case MVPRED_U:
			mvp[hv] = (s16)mvb;
			break;
		case MVPRED_UR:
			mvp[hv] = (s16)mvc;
			break;
		default:
			assert(0);
			break;
		}
	}

	// clip MVP after rounding (rounding process might result in 32768)
	int mvp_x, mvp_y;
	com_mv_rounding_s32((s32)mvp[MV_X], (s32)mvp[MV_Y], &mvp_x, &mvp_y, 0, 0);
	mvp[MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_x);
	mvp[MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mvp_y);
}
#endif

void derive_tmvp_mode(const SKIP_MERGE_INPUT * input, const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT * mode, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV])
{
//#pragma HLS INLINE OFF
#pragma HLS INLINE
	const U4 co_x_scu_of_ctu = get_colocal_scu_len(x_scu, pic_width_in_scu);
	const U4 co_y_scu_of_ctu = get_colocal_scu_len(y_scu, pic_height_in_scu);
	skip_element_skip_idx(mode, TRUE, 0);
#if !ENABLE_BFRAME
	assert(input->slice_type == SLICE_P);
#endif
#pragma HLS   ARRAY_PARTITION variable=mode->mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode->refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode->mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode->mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode->mvp complete dim=0
#if !ENABLE_BFRAME
	const U1 valid = REFI_IS_VALID(input->refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu]);
	mode->mvp[REFP_0][MV_X] = valid ? (S14)input->refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][MV_X] :(S14)0;
	mode->mvp[REFP_0][MV_Y] = valid ? (S14)input->refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][MV_Y] :(S14)0;
	mode->refi[REFP_0] = 0;
#else
	if (input->slice_type == SLICE_P)
	{
		if (REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0]))
		{
			get_col_mv_from_list0(mode->mvp[REFP_0], input, co_x_scu_of_ctu, co_y_scu_of_ctu);
		}
		else
		{
			mode->mvp[REFP_0][MV_X] = 0;
			mode->mvp[REFP_0][MV_Y] = 0;
		}
		mode->refi[REFP_0] = 0;
		mode->mvp[REFP_1][MV_X] = 0;
		mode->mvp[REFP_1][MV_Y] = 0;
		mode->refi[REFP_1] = REFI_INVALID;
	}
#if ENABLE_BFRAME
	else
	{ // SLICE_B
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
		if (!REFI_IS_VALID(pi->refp[0][REFP_1].map_refi[scup_co][REFP_0]) || pi->refp[0][REFP_1].list_is_library_pic[pi->refp[0][REFP_1].map_refi[scup_co][REFP_0]] || pi->refp[0][REFP_1].is_library_picture || pi->refp[0][REFP_0].is_library_picture)
#else
		if (!REFI_IS_VALID(input->refp_map_refi1[REFP_1][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0])) // REFP_1?T2?????¡ê???¨®¨°mvp
#endif
		{
			com_get_mvp_default(mode->mvp[REFP_0], input, neb, neb_map, REFP_0);
			com_get_mvp_default(mode->mvp[REFP_1], input, neb, neb_map, REFP_1);
		}
		else
		{
			get_col_mv_from_list1(mode->mvp, input, co_x_scu_of_ctu, co_y_scu_of_ctu);
		}
		SET_REFI(mode->refi, 0, 0);
	}
#endif
#endif
}
void derive_tmvp_mode_16(const SKIP_MERGE_INPUT_16* skip_in_16, const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT* mode, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV])
{
#pragma HLS INLINE OFF
	const U4 co_x_scu_of_ctu = get_colocal_scu_len(x_scu, pic_width_in_scu);
	const U4 co_y_scu_of_ctu = get_colocal_scu_len(y_scu, pic_height_in_scu);
	skip_element_skip_idx(mode, TRUE, 0);
#if !ENABLE_BFRAME
	assert(input->slice_type == SLICE_P);
#endif

#if !ENABLE_BFRAME
	const U1 valid = REFI_IS_VALID(skip_in_16->refp_refi);
	mode->mvp[REFP_0][MV_X] = valid ? (S14)skip_in_16->refp_mv[MV_X] : (S14)0;
	mode->mvp[REFP_0][MV_Y] = valid ? (S14)skip_in_16->refp_mv[MV_Y] : (S14)0;
	mode->refi[REFP_0] = 0;
#else
	if (skip_in_16->slice_type == SLICE_P)
	{
		if (REFI_IS_VALID(skip_in_16->refp_map_refi[REFP_0][REFP_0]))
		{
			get_col_mv_from_list0_16(mode->mvp[REFP_0], skip_in_16, co_x_scu_of_ctu, co_y_scu_of_ctu);
		}
		else
		{
			mode->mvp[REFP_0][MV_X] = 0;
			mode->mvp[REFP_0][MV_Y] = 0;
		}
		mode->refi[REFP_0] = 0;
		mode->mvp[REFP_1][MV_X] = 0;
		mode->mvp[REFP_1][MV_Y] = 0;
		mode->refi[REFP_1] = REFI_INVALID;
	}
#if ENABLE_BFRAME
	else
	{ // SLICE_B
#if !LIBVC_BLOCKDISTANCE_BY_LIBPTR
		if (!REFI_IS_VALID(pi->refp[0][REFP_1].map_refi[scup_co][REFP_0]) || pi->refp[0][REFP_1].list_is_library_pic[pi->refp[0][REFP_1].map_refi[scup_co][REFP_0]] || pi->refp[0][REFP_1].is_library_picture || pi->refp[0][REFP_0].is_library_picture)
#else
		if (!REFI_IS_VALID(skip_in_16->refp_map_refi[REFP_1][REFP_0])) // REFP_1?T2?????¡ê???¨®¨°mvp
#endif
		{
			com_get_mvp_default_16(mode->mvp[REFP_0], skip_in_16, neb, neb_map, REFP_0);
			com_get_mvp_default_16(mode->mvp[REFP_1], skip_in_16, neb, neb_map, REFP_1);
		}
		else
		{
			get_col_mv_from_list1_16(mode->mvp, skip_in_16, co_x_scu_of_ctu, co_y_scu_of_ctu);
		}
		SET_REFI(mode->refi, 0, 0);
	}
#endif
#endif
}

void get_col_mv_from_list0(s16 refp_map_mv[MAX_CU_IN_SCU][MAX_CU_IN_SCU][MV_D], S2 refp_map_refi[MAX_CU_IN_SCU][MAX_CU_IN_SCU], U10 co_x_scu_of_ctu, U10 co_y_scu_of_ctu, S14 mvp[2][2])
{
#if MD_PRINT
	assert(refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu] == 0);
#endif
	mvp[REFP_0][MV_X] = refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][0];
	mvp[REFP_0][MV_Y] = refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][1];
}

void derive_spatial_mode(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], SKIP_ELEMENT * mode)
{
//#pragma HLS INLINE OFF
#pragma HLS INLINE
	U1 valid[NUM_SKIP_SPATIAL_MV];
#pragma HLS array_partition variable=valid complete dim=0
#pragma HLS array_partition variable=mode complete dim=0
#pragma HLS array_partition variable=mode[2].refi complete dim=0
#pragma HLS array_partition variable=mode[1].refi complete dim=0
#pragma HLS array_partition variable=mode[0].refi complete dim=0
#pragma HLS array_partition variable=mode[2].mvp complete dim=0
#pragma HLS array_partition variable=mode[1].mvp complete dim=0
#pragma HLS array_partition variable=mode[0].mvp complete dim=0
#pragma HLS array_partition variable=valid complete dim=0

	for (int k = 0; k < PRED_DIR_NUM; k++)
	{
#pragma HLS	UNROLL
		mode[k].mvp[REFP_0][MV_X] = 0;
		mode[k].mvp[REFP_0][MV_Y] = 0;
		mode[k].mvp[REFP_1][MV_X] = 0;
		mode[k].mvp[REFP_1][MV_Y] = 0;
	}
	mode[2].refi[REFP_0] = 0;
	mode[2].refi[REFP_1] = REFI_INVALID;
	mode[1].refi[REFP_0] = REFI_INVALID;
	mode[1].refi[REFP_1] = 0;
	mode[0].refi[REFP_0] = 0;
	mode[0].refi[REFP_1] = 0;

	S4 neb_idx[3] = {};

#pragma HLS array_partition variable=valid complete dim=0

	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; ++i)
	{
#pragma HLS UNROLL
		valid[i] = neb[i].valid_flag && REFI_IS_VALID(neb_map[i].map_refi[REFP_0])
#if ENABLE_BFRAME
			&& !REFI_IS_VALID(neb_map[i].map_refi[REFP_1])
#endif
			;
	}
	neb_idx[2] = get_first_valid_neb(valid, 0);
	skip_element_skip_idx(mode + 2, TRUE, 3);
	mode[2].mvp[REFP_0][MV_X] = neb_idx[2] == -1 ? (s16)0 : neb_map[neb_idx[2]].map_mv[REFP_0][MV_X];
	mode[2].mvp[REFP_0][MV_Y] = neb_idx[2] == -1 ? (s16)0 : neb_map[neb_idx[2]].map_mv[REFP_0][MV_Y];
	mode[2].refi[REFP_0] = neb_idx[2] == -1 ? (s8)0 : neb_map[neb_idx[2]].map_refi[REFP_0];

#if ENABLE_BFRAME
	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; ++i)
	{
#pragma HLS	UNROLL
		valid[i] = neb[i].valid_flag && !REFI_IS_VALID(neb_map[i].map_refi[REFP_0]) && REFI_IS_VALID(neb_map[i].map_refi[REFP_1]);
	}
	neb_idx[1] = get_first_valid_neb(valid, 0);
	skip_element_skip_idx(mode + 1, TRUE, 2);
	mode[1].mvp[REFP_1][MV_X] = neb_idx[1] == -1 ? (s16)0 : neb_map[neb_idx[1]].map_mv[REFP_1][MV_X];
	mode[1].mvp[REFP_1][MV_Y] = neb_idx[1] == -1 ? (s16)0 : neb_map[neb_idx[1]].map_mv[REFP_1][MV_Y];
	mode[1].refi[REFP_1] = neb_idx[1] == -1 ? (s8)0 : neb_map[neb_idx[1]].map_refi[REFP_1];

	int last_bi_idx = 0, BI_motion_found = 0;
	skip_element_skip_idx(mode, TRUE, 1);
	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; ++i)
	{
#pragma HLS	UNROLL
		if (neb[i].valid_flag)
		{
			if (REFI_IS_VALID(neb_map[i].map_refi[REFP_0]) && REFI_IS_VALID(neb_map[i].map_refi[REFP_1]))
			{
				if (!BI_motion_found)
				{
					mode[0].mvp[REFP_0][MV_X] = neb_map[i].map_mv[REFP_0][MV_X];
					mode[0].mvp[REFP_0][MV_Y] = neb_map[i].map_mv[REFP_0][MV_Y];
					mode[0].refi[REFP_0] = neb_map[i].map_refi[REFP_0];
					mode[0].mvp[REFP_1][MV_X] = neb_map[i].map_mv[REFP_1][MV_X];
					mode[0].mvp[REFP_1][MV_Y] = neb_map[i].map_mv[REFP_1][MV_Y];
					mode[0].refi[REFP_1] = neb_map[i].map_refi[REFP_1];
				}
				BI_motion_found++;
				last_bi_idx = i; // save last BI index (first in order: D, B, A, C, G, F)
			}
		}
	}

	// combine L0+L1 to BI
	if (neb_idx[2] >= 0 && neb_idx[1] >= 0 && !BI_motion_found)
	{
		mode[0].mvp[REFP_0][MV_X] = mode[2].mvp[REFP_0][MV_X];
		mode[0].mvp[REFP_0][MV_Y] = mode[2].mvp[REFP_0][MV_Y];
		mode[0].refi[REFP_0] = mode[2].refi[REFP_0];
		mode[0].mvp[REFP_1][MV_X] = mode[1].mvp[REFP_1][MV_X];
		mode[0].mvp[REFP_1][MV_Y] = mode[1].mvp[REFP_1][MV_Y];
		mode[0].refi[REFP_1] = mode[1].refi[REFP_1];
	}
	// Separate last BI to L0
	if (neb_idx[2] == -1 && BI_motion_found)
	{
		mode[2].mvp[REFP_0][MV_X] = neb_map[last_bi_idx].map_mv[REFP_0][MV_X];
		mode[2].mvp[REFP_0][MV_Y] = neb_map[last_bi_idx].map_mv[REFP_0][MV_Y];
		mode[2].refi[REFP_0] = neb_map[last_bi_idx].map_refi[REFP_0];
		assert(REFI_IS_VALID(neb_map[last_bi_idx].map_refi[REFP_0]));
	}
	// Separate last BI to L1
	if (neb_idx[1] == -1 && BI_motion_found)
	{
		mode[1].mvp[REFP_1][MV_X] = neb_map[last_bi_idx].map_mv[REFP_1][MV_X];
		mode[1].mvp[REFP_1][MV_Y] = neb_map[last_bi_idx].map_mv[REFP_1][MV_Y];
		mode[1].refi[REFP_1] = neb_map[last_bi_idx].map_refi[REFP_1];
		assert(REFI_IS_VALID(neb_map[last_bi_idx].map_refi[REFP_1]));
	}
#else
	skip_element_skip_idx(mode + 1, FALSE, 2);
	skip_element_skip_idx(mode, FALSE, 1);
#endif
}

static void check_umve_motion_availability(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], U1 valid[NUM_SKIP_SPATIAL_MV])
{
#pragma HLS INLINE
	static const s32 default_mv[REFP_NUM][MV_D] = { { 0, 0 }, { 0, 0 } };
	static const s32 default_refi[REFP_NUM] = { REFI_INVALID, REFI_INVALID };
#pragma HLS array_partition variable=default_mv complete dim=0
	U1 valid0 = neb[3].valid_flag ?
		!is_same_skip_motion(neb_map[5].map_mv, neb_map[5].map_refi, neb_map[3].map_mv, neb_map[3].map_refi) :
		!is_same_skip_motion(neb_map[5].map_mv, neb_map[5].map_refi, default_mv, default_refi);
	U1 valid1 = neb[1].valid_flag ?
		!is_same_skip_motion(neb_map[5].map_mv, neb_map[5].map_refi, neb_map[1].map_mv, neb_map[1].map_refi) :
		!is_same_skip_motion(neb_map[5].map_mv, neb_map[5].map_refi, default_mv, default_refi);
	valid[0] = neb[0].valid_flag;
	valid[1] = neb[1].valid_flag && (!neb[0].valid_flag || !is_same_skip_motion(neb_map[1].map_mv, neb_map[1].map_refi, neb_map[0].map_mv, neb_map[0].map_refi));
	valid[2] = neb[2].valid_flag && (!neb[1].valid_flag || !is_same_skip_motion(neb_map[2].map_mv, neb_map[2].map_refi, neb_map[1].map_mv, neb_map[1].map_refi));
	valid[3] = neb[3].valid_flag && (!neb[0].valid_flag || !is_same_skip_motion(neb_map[3].map_mv, neb_map[3].map_refi, neb_map[0].map_mv, neb_map[0].map_refi));
	valid[4] = FALSE;
	valid[5] = neb[5].valid_flag && valid0 && valid1;
}

static void derive_umve_base_motions(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], S14 base_mv[UMVE_BASE_NUM][REFP_NUM][MV_D], S2 base_refi[UMVE_BASE_NUM][REFP_NUM], const SKIP_ELEMENT * tmvp)
{
#pragma HLS INLINE OFF

	U1 valid[NUM_SKIP_SPATIAL_MV] = {};
	S4 neb_idx[UMVE_BASE_NUM] = {};
#pragma HLS array_partition variable=valid complete dim=0
#pragma HLS array_partition variable=neb_idx complete dim=0
#pragma HLS array_partition variable=neb_map[neb_idx[0]].map_mv complete dim=0
#pragma HLS array_partition variable=neb_map[neb_idx[1]].map_mv complete dim=0
#pragma HLS array_partition variable=neb_map[neb_idx[0]].map_refi complete dim=0
#pragma HLS array_partition variable=neb_map[neb_idx[1]].map_refi complete dim=0
#pragma HLS array_partition variable=neb_map[neb_idx[1]].map_refi complete dim=0
#pragma HLS array_partition variable=tmvp->mvp complete dim=0
#pragma HLS array_partition variable=tmvp->refi complete dim=0
	check_umve_motion_availability(neb, neb_map, valid);

#if MD_PRINT
	assert(UMVE_BASE_NUM == 2);
#endif
	neb_idx[0] = get_first_valid_neb(valid, 0);
	neb_idx[1] = neb_idx[0] == -1 ? S4(-1) : get_first_valid_neb(valid, neb_idx[0] + 1);

	base_mv[0][REFP_0][MV_X] = neb_idx[0] == -1 ? tmvp->mvp[REFP_0][MV_X] : neb_map[neb_idx[0]].map_mv[REFP_0][MV_X];
	base_mv[0][REFP_0][MV_Y] = neb_idx[0] == -1 ? tmvp->mvp[REFP_0][MV_Y] : neb_map[neb_idx[0]].map_mv[REFP_0][MV_Y];
	base_refi[0][REFP_0] = neb_idx[0] == -1 ? tmvp->refi[REFP_0] : neb_map[neb_idx[0]].map_refi[REFP_0];

	base_mv[1][REFP_0][MV_X] = neb_idx[0] == -1 ? S14(0) : (S14)(neb_idx[1] == -1 ? tmvp->mvp[REFP_0][MV_X] : neb_map[neb_idx[1]].map_mv[REFP_0][MV_X]);
	base_mv[1][REFP_0][MV_Y] = neb_idx[0] == -1 ? S14(0) : (S14)(neb_idx[1] == -1 ? tmvp->mvp[REFP_0][MV_Y] : neb_map[neb_idx[1]].map_mv[REFP_0][MV_Y]);
	base_refi[1][REFP_0] = neb_idx[0] == -1 ? S2(0) : (S2)(neb_idx[1] == -1 ? tmvp->refi[REFP_0] : neb_map[neb_idx[1]].map_refi[REFP_0]);

	base_mv[0][REFP_1][MV_X] = neb_idx[0] == -1 ? tmvp->mvp[REFP_1][MV_X] : neb_map[neb_idx[0]].map_mv[REFP_1][MV_X];
	base_mv[0][REFP_1][MV_Y] = neb_idx[0] == -1 ? tmvp->mvp[REFP_1][MV_Y] : neb_map[neb_idx[0]].map_mv[REFP_1][MV_Y];
	base_refi[0][REFP_1] = neb_idx[0] == -1 ? tmvp->refi[REFP_1] : neb_map[neb_idx[0]].map_refi[REFP_1];

	base_mv[1][REFP_1][MV_X] = neb_idx[0] == -1 ? S14(0) : (S14)(neb_idx[1] == -1 ? tmvp->mvp[REFP_1][MV_X] : neb_map[neb_idx[1]].map_mv[REFP_1][MV_X]);
	base_mv[1][REFP_1][MV_Y] = neb_idx[0] == -1 ? S14(0) : (S14)(neb_idx[1] == -1 ? tmvp->mvp[REFP_1][MV_Y] : neb_map[neb_idx[1]].map_mv[REFP_1][MV_Y]);
	base_refi[1][REFP_1] = neb_idx[0] == -1 ? S2(REFI_INVALID) : (S2)(neb_idx[1] == -1 ? tmvp->refi[REFP_1] : neb_map[neb_idx[1]].map_refi[REFP_1]);

}
extern int  g_table_t0[17];
void derive_umve_final_motions(SKIP_ELEMENT* mode, const S14 base_mv[REFP_NUM][MV_D], const S2 base_refi[REFP_NUM], const U1 step, const U2 direction, const U6 umve_idx, const SKIP_MERGE_INPUT* input)
{
	//#pragma HLS INLINE OFF
	#pragma HLS INLINE
#pragma HLS function_instantiate variable=direction
	S14 mvd[MV_D] = {};
#pragma HLS array_partition variable=mvd complete dim=0

	mvd[MV_X] = 0;
	mvd[MV_Y] = 0;
	if (direction == 0)
	{
		mvd[MV_X] = step;
	}
	else if (direction == 1)
	{
		mvd[MV_X] = -step;
	}
	else if (direction == 2)
	{
		mvd[MV_Y] = step;
	}
	else if (direction == 3)
	{
		mvd[MV_Y] = -step;
	}
#if MD_PRINT
	else
		assert(FALSE);
#endif

	//switch (direction) {
	//case 0: mvd[MV_X] = step; mvd[MV_Y] = 0; break;
	//case 1: mvd[MV_X] = -step; mvd[MV_Y] = 0; break;
	//case 2: mvd[MV_X] = 0; mvd[MV_Y] = step; break;
	//case 3: mvd[MV_X] = 0; mvd[MV_Y] = -step; break;
	//default: assert(FALSE); break;
	//}

	skip_element_umve_idx(mode, TRUE, umve_idx);

#if ENABLE_BFRAME
	if (REFI_IS_VALID(base_refi[REFP_0]) && REFI_IS_VALID(base_refi[REFP_1])) // ¨ª???¨º???mv¨¦¨²3¨¦D?¦Ì?mvp
	{
		const int poc0 = input->refp_ptr1[base_refi[REFP_0]][REFP_0] * 2;
		const int poc1 = input->refp_ptr1[base_refi[REFP_1]][REFP_1] * 2;
		int list0_weight = 1 << MV_SCALE_PREC;
		
		int list1_weight = 1 << MV_SCALE_PREC;
		
		int list0_sign = 1;
		int list1_sign = 1;

		int cur_poc = input->ptr * 2;
		
		if (abs(poc1 - cur_poc) >= abs(poc0 - cur_poc))
		{
			
			list0_weight = g_table_t0[abs(poc1 - cur_poc)>>1] * abs(poc0 - cur_poc);
			
			if ((poc1 - cur_poc) * (poc0 - cur_poc) < 0)
			{
				list0_sign = -1;
			}
		}
		else
		{
			
			list1_weight = g_table_t0[abs(poc0 - cur_poc)>>1] * abs(poc1 - cur_poc);
			
			if ((poc1 - cur_poc) * (poc0 - cur_poc) < 0)
			{
				list1_sign = -1;
			}
		}

		int ref_mvd0 = (list0_weight * step + (1 << (MV_SCALE_PREC - 1))) >> MV_SCALE_PREC;
		int ref_mvd1 = (list1_weight * step + (1 << (MV_SCALE_PREC - 1))) >> MV_SCALE_PREC;

		ref_mvd0 = COM_CLIP3(-(1 << 15), (1 << 15) - 1, list0_sign * ref_mvd0);
		ref_mvd1 = COM_CLIP3(-(1 << 15), (1 << 15) - 1, list1_sign * ref_mvd1);

		s32 mv_offset[REFP_NUM][MV_D];
		if (direction == 0)
		{
			mv_offset[REFP_0][MV_X] = ref_mvd0;
			mv_offset[REFP_0][MV_Y] = 0;
			mv_offset[REFP_1][MV_X] = ref_mvd1;
			mv_offset[REFP_1][MV_Y] = 0;
		}
		else if (direction == 1)
		{
			mv_offset[REFP_0][MV_X] = -ref_mvd0;
			mv_offset[REFP_0][MV_Y] = 0;
			mv_offset[REFP_1][MV_X] = -ref_mvd1;
			mv_offset[REFP_1][MV_Y] = 0;
		}
		else if (direction == 2)
		{
			mv_offset[REFP_0][MV_X] = 0;
			mv_offset[REFP_0][MV_Y] = ref_mvd0;
			mv_offset[REFP_1][MV_X] = 0;
			mv_offset[REFP_1][MV_Y] = ref_mvd1;
		}
		else
		{
			mv_offset[REFP_0][MV_X] = 0;
			mv_offset[REFP_0][MV_Y] = -ref_mvd0;
			mv_offset[REFP_1][MV_X] = 0;
			mv_offset[REFP_1][MV_Y] = -ref_mvd1;
		}

		s32 mv_x = (s32)base_mv[REFP_0][MV_X] + mv_offset[REFP_0][MV_X];
		s32 mv_y = (s32)base_mv[REFP_0][MV_Y] + mv_offset[REFP_0][MV_Y];

		mode->mvp[REFP_0][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
		mode->mvp[REFP_0][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
		mode->refi[REFP_0] = base_refi[REFP_0];

		mv_x = (s32)base_mv[REFP_1][MV_X] + mv_offset[REFP_1][MV_X];
		mv_y = (s32)base_mv[REFP_1][MV_Y] + mv_offset[REFP_1][MV_Y];

		mode->mvp[REFP_1][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
		mode->mvp[REFP_1][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
		mode->refi[REFP_1] = base_refi[REFP_1];
	}
	else if (REFI_IS_VALID(base_refi[REFP_0]))
	{
#endif
		U1 valid = REFI_IS_VALID(base_refi[REFP_0]);
		mode->mvp[REFP_0][MV_X] = valid ? COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_0][MV_X] + mvd[MV_X]) : S15(0);
		mode->mvp[REFP_0][MV_Y] = valid ? COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_0][MV_Y] + mvd[MV_Y]) : S15(0);
		mode->refi[REFP_0] = valid ? base_refi[REFP_0] : S2(REFI_INVALID);
		mode->mvp[REFP_1][MV_X] = 0;
		mode->mvp[REFP_1][MV_Y] = 0;
		mode->refi[REFP_1] = REFI_INVALID;
#if ENABLE_BFRAME
	}
	else if (REFI_IS_VALID(base_refi[REFP_1]))
	{
		mode->mvp[REFP_0][MV_X] = 0;
		mode->mvp[REFP_0][MV_Y] = 0;
		mode->refi[REFP_0] = REFI_INVALID;
		mode->mvp[REFP_1][MV_X] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_1][MV_X] + mvd[MV_X]);
		mode->mvp[REFP_1][MV_Y] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_1][MV_Y] + mvd[MV_Y]);
		mode->refi[REFP_1] = base_refi[REFP_1];
	}
	else
	{
		mode->mvp[REFP_0][MV_X] = 0;
		mode->mvp[REFP_0][MV_Y] = 0;
		mode->refi[REFP_0] = REFI_INVALID;
		mode->mvp[REFP_1][MV_X] = 0;
		mode->mvp[REFP_1][MV_Y] = 0;
		mode->refi[REFP_1] = REFI_INVALID;
	}
#endif
}
void derive_umve_final_motions_16(SKIP_ELEMENT* mode, const S14 base_mv[REFP_NUM][MV_D], const S2 base_refi[REFP_NUM], const U1 step, const U2 direction, const U6 umve_idx, const SKIP_MERGE_INPUT_16* skip_in_16)
{
#pragma HLS INLINE OFF
#pragma HLS function_instantiate variable=direction
	S14 mvd[MV_D] = {};
#pragma HLS array_partition variable=mvd complete dim=0

	mvd[MV_X] = 0;
	mvd[MV_Y] = 0;
	if (direction == 0)
	{
		mvd[MV_X] = step;
	}
	else if (direction == 1)
	{
		mvd[MV_X] = -step;
	}
	else if (direction == 2)
	{
		mvd[MV_Y] = step;
	}
	else if (direction == 3)
	{
		mvd[MV_Y] = -step;
	}
#if MD_PRINT
	else
		assert(FALSE);
#endif

	//switch (direction) {
	//case 0: mvd[MV_X] = step; mvd[MV_Y] = 0; break;
	//case 1: mvd[MV_X] = -step; mvd[MV_Y] = 0; break;
	//case 2: mvd[MV_X] = 0; mvd[MV_Y] = step; break;
	//case 3: mvd[MV_X] = 0; mvd[MV_Y] = -step; break;
	//default: assert(FALSE); break;
	//}

	skip_element_umve_idx(mode, TRUE, umve_idx);

#if ENABLE_BFRAME
	if (REFI_IS_VALID(base_refi[REFP_0]) && REFI_IS_VALID(base_refi[REFP_1])) // ¨ª???¨º???mv¨¦¨²3¨¦D?¦Ì?mvp
	{
		const int poc0 = skip_in_16->refp_ptr1[base_refi[REFP_0]][REFP_0] * 2;
		const int poc1 = skip_in_16->refp_ptr1[base_refi[REFP_1]][REFP_1] * 2;
		int list0_weight = 1 << MV_SCALE_PREC;
		int list1_weight = 1 << MV_SCALE_PREC;
		
		int list0_sign = 1;
		int list1_sign = 1;

		int cur_poc = skip_in_16->ptr * 2;

		if (abs(poc1 - cur_poc) >= abs(poc0 - cur_poc))
		{
			
			list0_weight = g_table_t0[abs(poc1 - cur_poc)>>1] * abs(poc0 - cur_poc);
			
			if ((poc1 - cur_poc) * (poc0 - cur_poc) < 0)
			{
				list0_sign = -1;
			}
		}
		else
		{
			
			list1_weight = g_table_t0[abs(poc0 - cur_poc)>>1] * abs(poc1 - cur_poc);
			
			if ((poc1 - cur_poc) * (poc0 - cur_poc) < 0)
			{
				list1_sign = -1;
			}
		}

		int ref_mvd0 = (list0_weight * step + (1 << (MV_SCALE_PREC - 1))) >> MV_SCALE_PREC;
		int ref_mvd1 = (list1_weight * step + (1 << (MV_SCALE_PREC - 1))) >> MV_SCALE_PREC;

		ref_mvd0 = COM_CLIP3(-(1 << 15), (1 << 15) - 1, list0_sign * ref_mvd0);
		ref_mvd1 = COM_CLIP3(-(1 << 15), (1 << 15) - 1, list1_sign * ref_mvd1);

		s32 mv_offset[REFP_NUM][MV_D];
		if (direction == 0)
		{
			mv_offset[REFP_0][MV_X] = ref_mvd0;
			mv_offset[REFP_0][MV_Y] = 0;
			mv_offset[REFP_1][MV_X] = ref_mvd1;
			mv_offset[REFP_1][MV_Y] = 0;
		}
		else if (direction == 1)
		{
			mv_offset[REFP_0][MV_X] = -ref_mvd0;
			mv_offset[REFP_0][MV_Y] = 0;
			mv_offset[REFP_1][MV_X] = -ref_mvd1;
			mv_offset[REFP_1][MV_Y] = 0;
		}
		else if (direction == 2)
		{
			mv_offset[REFP_0][MV_X] = 0;
			mv_offset[REFP_0][MV_Y] = ref_mvd0;
			mv_offset[REFP_1][MV_X] = 0;
			mv_offset[REFP_1][MV_Y] = ref_mvd1;
		}
		else
		{
			mv_offset[REFP_0][MV_X] = 0;
			mv_offset[REFP_0][MV_Y] = -ref_mvd0;
			mv_offset[REFP_1][MV_X] = 0;
			mv_offset[REFP_1][MV_Y] = -ref_mvd1;
		}

		s32 mv_x = (s32)base_mv[REFP_0][MV_X] + mv_offset[REFP_0][MV_X];
		s32 mv_y = (s32)base_mv[REFP_0][MV_Y] + mv_offset[REFP_0][MV_Y];

		mode->mvp[REFP_0][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
		mode->mvp[REFP_0][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
		mode->refi[REFP_0] = base_refi[REFP_0];

		mv_x = (s32)base_mv[REFP_1][MV_X] + mv_offset[REFP_1][MV_X];
		mv_y = (s32)base_mv[REFP_1][MV_Y] + mv_offset[REFP_1][MV_Y];

		mode->mvp[REFP_1][MV_X] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_x);
		mode->mvp[REFP_1][MV_Y] = (s16)COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, mv_y);
		mode->refi[REFP_1] = base_refi[REFP_1];
	}
	else if (REFI_IS_VALID(base_refi[REFP_0]))
	{
#endif
		U1 valid = REFI_IS_VALID(base_refi[REFP_0]);
		mode->mvp[REFP_0][MV_X] = valid ? COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_0][MV_X] + mvd[MV_X]) : S15(0);
		mode->mvp[REFP_0][MV_Y] = valid ? COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_0][MV_Y] + mvd[MV_Y]) : S15(0);
		mode->refi[REFP_0] = valid ? base_refi[REFP_0] : S2(REFI_INVALID);
		mode->mvp[REFP_1][MV_X] = 0;
		mode->mvp[REFP_1][MV_Y] = 0;
		mode->refi[REFP_1] = REFI_INVALID;
#if ENABLE_BFRAME
	}
	else if (REFI_IS_VALID(base_refi[REFP_1]))
	{
		mode->mvp[REFP_0][MV_X] = 0;
		mode->mvp[REFP_0][MV_Y] = 0;
		mode->refi[REFP_0] = REFI_INVALID;
		mode->mvp[REFP_1][MV_X] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_1][MV_X] + mvd[MV_X]);
		mode->mvp[REFP_1][MV_Y] = COM_CLIP3(COM_INT16_MIN, COM_INT16_MAX, base_mv[REFP_1][MV_Y] + mvd[MV_Y]);
		mode->refi[REFP_1] = base_refi[REFP_1];
	}
	else
	{
		mode->mvp[REFP_0][MV_X] = 0;
		mode->mvp[REFP_0][MV_Y] = 0;
		mode->refi[REFP_0] = REFI_INVALID;
		mode->mvp[REFP_1][MV_X] = 0;
		mode->mvp[REFP_1][MV_Y] = 0;
		mode->refi[REFP_1] = REFI_INVALID;
	}
#endif
}

void derive_umve_mode(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], SKIP_ELEMENT mode[SKIP_UMVE_NUM], SKIP_ELEMENT * tmvp, const SKIP_MERGE_INPUT* input)
{
//#pragma HLS INLINE OFF
#pragma HLS INLINE
	S14 base_mv[UMVE_BASE_NUM][REFP_NUM][MV_D] = {};
	S2 base_refi[UMVE_BASE_NUM][REFP_NUM] = {};
	static const U5 ref_mvd_cands[5] = { 1, 2, 4, 8, 16 };
#pragma HLS array_partition variable=base_mv complete dim=0
#pragma HLS array_partition variable=base_refi complete dim=0
#pragma HLS array_partition variable=ref_mvd_cands complete dim=0
#pragma HLS array_partition variable=ref_mvd_cands complete dim=0
#pragma HLS   ARRAY_PARTITION variable=input->refp_ptr1 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0
#pragma HLS   ARRAY_PARTITION variable=tmvp->mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=tmvp->refi complete dim=0
	extern int  g_table_t0[17];
#pragma HLS   ARRAY_PARTITION variable=g_table_t0 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode complete dim=0
	derive_umve_base_motions(neb, neb_map, base_mv, base_refi, tmvp);

	
	for (int base = 0; base < SKIP_UMVE_BASE_NUM; ++base) {
#pragma HLS UNROLL
		for (int refine = 0; refine < SKIP_UMVE_REFINE_NUM; ++refine) {
#pragma HLS UNROLL
			for (int direction = 0; direction < SKIP_UMVE_DIRECTION_NUM; ++direction) {
#pragma HLS UNROLL
				U6 umve_idx = base * UMVE_MAX_REFINE_NUM + refine * 4 + direction;
				U4 mode_idx = base * SKIP_UMVE_REFINE_NUM * SKIP_UMVE_DIRECTION_NUM + refine * SKIP_UMVE_DIRECTION_NUM + direction;
				derive_umve_final_motions(mode + mode_idx, base_mv[base], base_refi[base], ref_mvd_cands[refine], direction, umve_idx, input);
			}
		}
	}
}
void derive_umve_mode_16(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], SKIP_ELEMENT mode[SKIP_UMVE_NUM], SKIP_ELEMENT* tmvp, const SKIP_MERGE_INPUT_16* skip_in_16)
{
#pragma HLS INLINE OFF

	S14 base_mv[UMVE_BASE_NUM][REFP_NUM][MV_D] = {};
	S2 base_refi[UMVE_BASE_NUM][REFP_NUM] = {};
	static const U5 ref_mvd_cands[5] = { 1, 2, 4, 8, 16 };
#pragma HLS array_partition variable=base_mv complete dim=0
#pragma HLS array_partition variable=base_refi complete dim=0
#pragma HLS array_partition variable=ref_mvd_cands complete dim=0

	derive_umve_base_motions(neb, neb_map, base_mv, base_refi, tmvp);


	for (int base = 0; base < SKIP_UMVE_BASE_NUM; ++base) {
#pragma HLS UNROLL
		for (int refine = 0; refine < SKIP_UMVE_REFINE_NUM; ++refine) {
#pragma HLS UNROLL
			for (int direction = 0; direction < SKIP_UMVE_DIRECTION_NUM; ++direction) {
#pragma HLS UNROLL
				U6 umve_idx = base * UMVE_MAX_REFINE_NUM + refine * 4 + direction;
				U4 mode_idx = base * SKIP_UMVE_REFINE_NUM * SKIP_UMVE_DIRECTION_NUM + refine * SKIP_UMVE_DIRECTION_NUM + direction;
				derive_umve_final_motions_16(mode + mode_idx, base_mv[base], base_refi[base], ref_mvd_cands[refine], direction, umve_idx, skip_in_16);
			}
		}
	}
}

static void print_inter_cands(SKIP_ELEMENT * mode)
{
#if MD_PRINT
	int i;
	static int cnt = 0;
	printf("%d: ", cnt);
	for (i = 0; i < SKIP_MODE_NUM; ++i){
		if (!mode[i].valid){ continue; }
		printf("(%s-%d(%d,%d)-%d)",
			mode[i].umve_flag ? "umve" : "skip",
			mode[i].umve_flag ? mode[i].umve_idx : mode[i].skip_idx,
			mode[i].mvp[MV_X],
			mode[i].mvp[MV_Y],
			mode[i].refi);
	}
	printf("\n");
	++cnt;
#endif
}
#if SUB_TMVP
int get_colocal_scup_sbtmvp(int scup, int pic_width_in_scu, int pic_height_in_scu)
{
	const int mask = (-1) ^ 3;
	int bx = scup % pic_width_in_scu;
	int by = scup / pic_width_in_scu;
	int xpos = (bx & mask) + 2;
	int ypos = (by & mask) + 2;

	if (ypos >= pic_height_in_scu)
	{
		ypos = ((by & mask) + pic_height_in_scu) >> 1;
	}
	if (xpos >= pic_width_in_scu)
	{
		xpos = ((bx & mask) + pic_width_in_scu) >> 1;
	}

	return ypos * pic_width_in_scu + xpos;


}void get_col_mv_from_list0_ext(const SKIP_MERGE_INPUT* input, U4 co_x_scu_of_ctu, U4 co_y_scu_of_ctu, s32 mvp[REFP_NUM][MV_D], s32 refi[REFP_NUM])
{
	//s16 mvc[MV_D];

	// ref ref ptr
	/*int ptr_col;
	int ptr_col_ref;
	ptr_col = input->ptr;*/

	//assert(REFI_IS_VALID(refp->map_refi[scup_co][REFP_0]) || REFI_IS_VALID(refp->map_refi[scup_co][REFP_1]));

//	if (REFI_IS_VALID(refp->map_refi[scup_co][REFP_0]))
#pragma HLS INLINE
//	if (REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0]))

//	{

		const U1 valid = REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0]);

		mvp[REFP_0][MV_X] = valid ? input->refp_map_mv1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_X] : (S14)0;
		mvp[REFP_0][MV_Y] = valid ? input->refp_map_mv1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_Y] : (S14)0;

		//ptr_col_ref = refp->list_ptr[REFP_0][refp->map_refi[scup_co][REFP_0]];
		//ptr_col_ref = ptr_col - 1;
		//scaling_mv1(1, 0, ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
		refi[REFP_0] = 0;
//	}
	//else if (REFI_IS_VALID(refp->map_refi[scup_co][REFP_1]))
	//{
	//	//refl1 backward shouldn't available
	//	mvc[MV_X] = refp->map_mv[scup_co][REFP_1][MV_X];
	//	mvc[MV_Y] = refp->map_mv[scup_co][REFP_1][MV_Y];

	//	//ptr_col_ref = refp->list_ptr[REFP_1][refp->map_refi[scup_co][REFP_1]];
	//	ptr_col_ref = ptr_col - 1;
	//	scaling_mv1(1, 0, ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
	//	refi[REFP_0] = 0;
	//}
}
#endif
void check_refi(s32 refi[REFP_NUM], strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS INLINE
#pragma HLS   ARRAY_PARTITION variable=Fetch_Ref_window_ptr->lidx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=Fetch_Ref_window_ptr->cur_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=refi complete dim=0
	int i_tmp = 0;
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		bool res[REFP_NUM] = { true, true };
#pragma HLS   ARRAY_PARTITION variable=res complete dim=0
		for (int i = 0; i < REFP_NUM; ++i)
		{
#pragma HLS	UNROLL
			if (REFI_IS_VALID(refi[i_tmp]))
			{
				res[i] = false;
				for (int j = 0; j < REFP_NUM; ++j)
				{
#pragma HLS	UNROLL
					if (i_tmp == Fetch_Ref_window_ptr->lidx[j] && refi[i] == Fetch_Ref_window_ptr->cur_refi[j])
					{
						res[i] = true;
						break;
					}
				}
			}
			i_tmp++;
		}
		if (!res[REFP_0] || !res[REFP_1])
		{
			//printf("deprecate refi:%d %d\n", refi[REFP_0], refi[REFP_1]);
			refi[REFP_0] = refi[REFP_1] = REFI_INVALID;
		}
	}
	else
	{
		for (int i = 0; i < REFP_NUM; ++i)
		{
#pragma HLS	UNROLL
			if (REFI_IS_VALID(refi[i]))
			{
				assert(refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_0] || refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_1]);
			}
		}
	}
#endif
}

void check_refi_8_16(s32 refi[REFP_NUM], strFetch_ref_window_16* Fetch_Ref_window_ptr)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		bool res[REFP_NUM] = { true, true };
		for (int i = 0; i < REFP_NUM; ++i)
		{
			if (REFI_IS_VALID(refi[i]))
			{
				res[i] = false;
				for (int j = 0; j < REFP_NUM; ++j)
				{
					if (i == Fetch_Ref_window_ptr->lidx[j] && refi[i] == Fetch_Ref_window_ptr->cur_refi[j])
					{
						res[i] = true;
						break;
					}
				}
			}
		}
		if (!res[REFP_0] || !res[REFP_1])
		{
			//printf("deprecate refi:%d %d\n", refi[REFP_0], refi[REFP_1]);
			refi[REFP_0] = refi[REFP_1] = REFI_INVALID;
		}
	}
	else
	{
		for (int i = 0; i < REFP_NUM; ++i)
		{
			if (REFI_IS_VALID(refi[i]))
			{
				assert(refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_0] || refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_1]);
			}
		}
	}
#endif
}
void check_refi_16_8(s32 refi[REFP_NUM], strFetch_ref_window_16* Fetch_Ref_window_ptr)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		bool res[REFP_NUM] = { true, true };
		for (int i = 0; i < REFP_NUM; ++i)
		{
			if (REFI_IS_VALID(refi[i]))
			{
				res[i] = false;
				for (int j = 0; j < REFP_NUM; ++j)
				{
					if (i == Fetch_Ref_window_ptr->lidx[j] && refi[i] == Fetch_Ref_window_ptr->cur_refi[j])
					{
						res[i] = true;
						break;
					}
				}
			}
		}
		if (!res[REFP_0] || !res[REFP_1])
		{
			//printf("deprecate refi:%d %d\n", refi[REFP_0], refi[REFP_1]);
			refi[REFP_0] = refi[REFP_1] = REFI_INVALID;
		}
	}
	else
	{
		for (int i = 0; i < REFP_NUM; ++i)
		{
			if (REFI_IS_VALID(refi[i]))
			{
				assert(refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_0] || refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_1]);
			}
		}
	}
#endif
}
void check_refi_16_16(s32 refi[REFP_NUM], strFetch_ref_window_16* Fetch_Ref_window_ptr)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		bool res[REFP_NUM] = { true, true };
		for (int i = 0; i < REFP_NUM; ++i)
		{
			if (REFI_IS_VALID(refi[i]))
			{
				res[i] = false;
				for (int j = 0; j < REFP_NUM; ++j)
				{
					if (i == Fetch_Ref_window_ptr->lidx[j] && refi[i] == Fetch_Ref_window_ptr->cur_refi[j])
					{
						res[i] = true;
						break;
					}
				}
			}
		}
		if (!res[REFP_0] || !res[REFP_1])
		{
			//printf("deprecate refi:%d %d\n", refi[REFP_0], refi[REFP_1]);
			refi[REFP_0] = refi[REFP_1] = REFI_INVALID;
		}
	}
	else
	{
		for (int i = 0; i < REFP_NUM; ++i)
		{
			if (REFI_IS_VALID(refi[i]))
			{
				assert(refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_0] || refi[i] == Fetch_Ref_window_ptr->cur_refi[REFP_1]);
			}
		}
	}
#endif
}

void derive_inter_cands(SKIP_MERGE_INPUT * input, U4 *num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window* Fetch_Ref_window_ptr
#if SUB_TMVP
	, int cu_width, int cu_height, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	if (!run_direct_skip){
		return;
	}

#if SUB_TMVP
	// scu position data
	static int sb_scup[SBTMVP_NUM][MV_D];
	static U4 co_x_scu_of_ctu[SBTMVP_NUM];
	static U4 co_y_scu_of_ctu[SBTMVP_NUM];
	//	int sb_scup_co[SBTMVP_NUM];
	int x_scup = x_scu;
	int y_scup = y_scu;
	u8 isCopy[SBTMVP_NUM] = { 0 };
#pragma HLS INLINE
#pragma HLS   ARRAY_PARTITION variable=sb_scup complete dim=0
#pragma HLS   ARRAY_PARTITION variable=co_x_scu_of_ctu complete dim=0
#pragma HLS   ARRAY_PARTITION variable=co_y_scu_of_ctu complete dim=0
#pragma HLS   ARRAY_PARTITION variable=isCopy complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode->mvp	 complete dim=0
////////////////////////////////////
#pragma HLS   ARRAY_PARTITION variable=	input->refp_map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	input->refp_map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	input->refp_list_ptr1 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	input->refp_ptr1 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[0].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[1].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[2].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[3].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[4].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[5].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[6].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[7].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[8].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[9].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[10].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[11].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[0].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[1].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[2].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[3].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[4].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[5].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[6].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[7].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[8].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[9].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[10].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[11].valid complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode complete dim=0
//#pragma HLS   ARRAY_PARTITION variable=	skip_rdo_mode complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[0].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[1].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[2].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[3].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[4].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[5].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[6].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[7].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[8].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[9].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[10].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[11].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb_map complete dim=0
#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	neb_map[0].map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[1].map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[2].map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[3].map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[4].map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[5].map_mv complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	neb_map[0].map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[1].map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[2].map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[3].map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[4].map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	neb_map[5].map_refi complete dim=0
	extern int  g_table_t0[17];
#pragma HLS   ARRAY_PARTITION variable=g_table_t0 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	Fetch_Ref_window_ptr->lidx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	Fetch_Ref_window_ptr->cur_refi complete dim=0
	//////////////////////////////////
	for (int p = 0; p < SBTMVP_NUM; p++)
	{
#pragma HLS UNROLL
		//sb_scup[p] = (x_scup + y_scup * pic_width_in_scu) + pic_width_in_scu * ((cu_height >> 2) - 1) * (p / 2) + ((cu_width >> 2) - 1) * (p % 2);

		sb_scup[p][MV_X] = x_scup + ((cu_width >> 2) - 1) * (p % 2);
		sb_scup[p][MV_Y] = y_scup + ((cu_height >> 2) - 1) * (p / 2);

		co_x_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_X], pic_width_in_scu);
		co_y_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_Y], pic_height_in_scu);

		//sb_scup_co[p] = get_colocal_scup_sbtmvp(sb_scup[p], pic_width_in_scu, pic_height_in_scu);
	}
#endif

	static SKIP_ELEMENT tmvp = {};
	static SKIP_ELEMENT spat[3] = {};

	// insert TMVP
	derive_tmvp_mode(input, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, &tmvp, neb, neb_map);
	skip_element_copy(mode, &tmvp);
//#pragma HLS   ARRAY_PARTITION variable=input->refp_map_refi1 complete dim=0
//#pragma HLS   ARRAY_PARTITION variable=input->refp_map_mv1 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=co_x_scu_of_ctu complete dim=0
#pragma HLS   ARRAY_PARTITION variable=co_y_scu_of_ctu complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[0].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[0].ref_idx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[1].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[1].ref_idx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[2].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[2].ref_idx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[3].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=core_sbtmvp->sbTmvp[3].ref_idx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode[0].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[1].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[2].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[3].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[4].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[5].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[6].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[7].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[8].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[9].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[10].mvp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	mode[11].mvp complete dim=0

#if SUB_TMVP
	if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
		if (input->slice_type == SLICE_P)
		{
			core_sbtmvp->isValid = TRUE;
			for (int p = 0; p < SBTMVP_NUM; p++)
			{
#pragma HLS UNROLL
				if (
					REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu[p]][co_x_scu_of_ctu[p]][REFP_0])
					)
				{
					//get_col_mv_from_list0(core_sbtmvp->sbTmvp[p].mv[REFP_0], input, co_y_scu_of_ctu[p], co_x_scu_of_ctu[p]);
					//core_sbtmvp->sbTmvp[p].ref_idx[REFP_0] = input->refp_map_refi[co_y_scu_of_ctu[p]][co_x_scu_of_ctu[p]];
					get_col_mv_from_list0_ext(input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p], core_sbtmvp->sbTmvp[p].mv, core_sbtmvp->sbTmvp[p].ref_idx);

				}
				else
				{
					copy_mv(core_sbtmvp->sbTmvp[p].mv[REFP_0], mode[0].mvp[REFP_0]);
					isCopy[p] = 1;
				}
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_X] = 0;
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_Y] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_0] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_1] = -1;
			}

#if DEBUG_CU
			if (isProblem)
			{
				printf("SbMVP\n");
				printf("TMVP REFP_0:(%d,%d) REFP_0:(%d,%d)\n,", mode[0].mvp[REFP_0][0], mode[0].mvp[REFP_0][1], mode[0].mvp[REFP_1][0], mode[0].mvp[REFP_1][1]);
				for (int p = 0; p < SBTMVP_NUM; p++)
				{
					printf("%d->%d (%d,%d) (%d,%d)\n", p, isCopy[p], core_sbtmvp->sbTmvp[p].mv[0][0], core_sbtmvp->sbTmvp[p].mv[0][1], co_x_scu_of_ctu[p], co_y_scu_of_ctu[p]);
				}
		}
#endif

		}
#if ENABLE_BFRAME
		else
		{
			core_sbtmvp->isValid = FALSE;
		}
#endif
	}
#endif

	// insert list_01, list_1, list_0
	derive_spatial_mode(neb, neb_map, spat);
	if (input->slice_type == SLICE_P)
	{
		spat[0].valid = spat[1].valid = FALSE;
	}
	skip_element_copy(mode + 1, spat);
	skip_element_copy(mode + 2, spat + 1);
	skip_element_copy(mode + 3, spat + 2);

	// insert UMVE
	*num_cands_woUMVE = PRED_DIR_NUM + 1;

#if !ENABLE_BFRAME
	assert(input->umve_enable_flag);
#endif
	if (input->umve_enable_flag)
	{
		derive_umve_mode(neb, neb_map, mode + 4 + SKIP_HMVP_NUM, &tmvp, input);
	}

#if ENABLE_BFRAME
	if (input->slice_type == SLICE_B)
	{
		for (int i = 0; i < SKIP_MODE_NUM; ++i)
		{
#pragma HLS UNROLL
			if (mode[i].valid)
			{
				check_refi(mode[i].refi, Fetch_Ref_window_ptr);
			}
		}
	}
#endif

	//	print_inter_cands(mode);
}

static BOOL com_derive_affine_model_mv0(U4 cur_x_scu_of_ctu, U4 cur_y_scu_of_ctu, S6 neb_x_scu_of_ctu,
	S6 neb_y_scu_of_ctu, U7 cu_width, U7 cu_height, U10 pic_width_in_scu,
	CPMV mvp[4][2], U2* vertex_num, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], U10 pix_x_scu, int lidx)
{
	S14 neb_mv[4][2] = { { 0 } };

	BOOL is_top_ctu_boundary = neb_y_scu_of_ctu < 0 ? 1 : 0;
	BOOL is_left_ctu_boundary = neb_x_scu_of_ctu < 0 ? 1 : 0;
	BOOL is_right_ctu_boundary = neb_x_scu_of_ctu >= 1 << (5 - 1) ? 1 : 0;
	U3 neb_log_w = 0;
	U3 neb_log_h = 0;
	if (is_top_ctu_boundary) {
		neb_log_w = me_line_map_ptr->new_map_cu_mode_above_line[neb_x_scu_of_ctu + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET] >> 24 & (0x0F);
		neb_log_h = me_line_map_ptr->new_map_cu_mode_above_line[neb_x_scu_of_ctu + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET] >> 28 & (0x0F);
	}
	else {
		neb_log_w = me_line_map_ptr->map_cu_mode_inner[is_left_ctu_boundary ? U1(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][neb_x_scu_of_ctu + (is_left_ctu_boundary ? (1 << (5 - 1)) : 0)][neb_y_scu_of_ctu] >> 24 & (0x0F);
		neb_log_h = me_line_map_ptr->map_cu_mode_inner[is_left_ctu_boundary ? U1(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][neb_x_scu_of_ctu + (is_left_ctu_boundary ? (1 << (5 - 1)) : 0)][neb_y_scu_of_ctu] >> 28 & (0x0F);
	}
	U7 neb_w = 1 << neb_log_w;
	U7 neb_h = 1 << neb_log_h;
	S6 x_scu_of_ctu[4];
	S6 y_scu_of_ctu[4];
	x_scu_of_ctu[0] = neb_x_scu_of_ctu;
	x_scu_of_ctu[1] = neb_x_scu_of_ctu + (neb_w >> 2) - 1;
	x_scu_of_ctu[2] = neb_x_scu_of_ctu;
	x_scu_of_ctu[3] = neb_x_scu_of_ctu + (neb_w >> 2) - 1;
	y_scu_of_ctu[0] = (is_top_ctu_boundary ? S6(neb_y_scu_of_ctu + (neb_h >> 2) - 1) : neb_y_scu_of_ctu);
	y_scu_of_ctu[1] = (is_top_ctu_boundary ? S6(neb_y_scu_of_ctu + (neb_h >> 2) - 1) : neb_y_scu_of_ctu);
	y_scu_of_ctu[2] = neb_y_scu_of_ctu + (neb_h >> 2) - 1;
	y_scu_of_ctu[3] = neb_y_scu_of_ctu + (neb_h >> 2) - 1;
	for (int i = 0; i < 4; i++) {

#pragma HLS loop_tripcount max = 4
		if (is_top_ctu_boundary) {
			neb_mv[i][0] = above_nebs_line[x_scu_of_ctu[i] + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][0];
			neb_mv[i][1] = above_nebs_line[x_scu_of_ctu[i] + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][1];
		}
		else {
			neb_mv[i][0] = inner_nebs[is_left_ctu_boundary ? (U1)(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][x_scu_of_ctu[i] + (is_left_ctu_boundary ? (1 << (5 - 1)) : 0)][y_scu_of_ctu[i]].mv[lidx][0];
			neb_mv[i][1] = inner_nebs[is_left_ctu_boundary ? (U1)(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][x_scu_of_ctu[i] + (is_left_ctu_boundary ? (1 << (5 - 1)) : 0)][y_scu_of_ctu[i]].mv[lidx][1];
		}
	}
	U6 cur_x = cur_x_scu_of_ctu << 2;
	U6 cur_y = cur_y_scu_of_ctu << 2;
	S8 neb_x = neb_x_scu_of_ctu << 2;
	S8 neb_y = is_top_ctu_boundary ? (S8)cur_y : (S8)neb_y_scu_of_ctu << 2;
	U3 diff_w = 7 - neb_log_w;
	U3 diff_h = 7 - neb_log_h;
	CPMV dmv_ver_x = 0;
	CPMV dmv_ver_y = 0;

	// deltaMvHor
	CPMV dmv_hor_x = ((neb_mv[1][0] - neb_mv[0][0])) << diff_w;
	CPMV dmv_hor_y = ((neb_mv[1][1] - neb_mv[0][1])) << diff_w;
	if (*vertex_num == 3 && !is_top_ctu_boundary) {
		// deltaMvVer
		dmv_ver_x = ((neb_mv[2][0] - neb_mv[0][0])) << diff_h;
		dmv_ver_y = ((neb_mv[2][1] - neb_mv[0][1])) << diff_h;
	}
	else {
		// deltaMvVer
		dmv_ver_x = -dmv_hor_y;
		dmv_ver_y = dmv_hor_x;
		*vertex_num = 2;
	}
	CPMV hor_base = neb_mv[0][0] << 7;
	CPMV ver_base = neb_mv[0][1] << 7;
	// derive CPMV 0
	CPMV tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y) + hor_base;
	CPMV tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y) + ver_base;
	com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
	tmp_hor <<= 2;
	tmp_ver <<= 2;
	mvp[0][0] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)))));
	mvp[0][1] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)))));
	// derive CPMV 1
	tmp_hor = dmv_hor_x * (cur_x - neb_x + cu_width) + dmv_ver_x * (cur_y - neb_y) + hor_base;
	tmp_ver = dmv_hor_y * (cur_x - neb_x + cu_width) + dmv_ver_y * (cur_y - neb_y) + ver_base;
	com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
	tmp_hor <<= 2;
	tmp_ver <<= 2;
	mvp[1][0] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)))));
	mvp[1][1] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)))));
	// derive CPMV 2
	if (*vertex_num == 3) {
		tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y + cu_height) + hor_base;
		tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y + cu_height) + ver_base;
		com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
		tmp_hor <<= 2;
		tmp_ver <<= 2;
		mvp[2][0] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_hor)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_hor)))));
		mvp[2][1] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_ver)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_ver)))));
	}
	for (int i = 0; i < *vertex_num; ++i) {

#pragma HLS loop_tripcount max = 3
		if (abs(mvp[i][0]) >> 4 > SRW) {
			return 0;
		}
		if (abs(mvp[i][1]) >> 4 > SRH) {
			return 0;
		}
	}
	return 1;
}

static U1 com_derive_affine_model_mv1(U4 cur_x_scu_of_ctu, U4 cur_y_scu_of_ctu, S6 neb_x_scu_of_ctu,
	S6 neb_y_scu_of_ctu, U7 cu_width, U7 cu_height, U10 pic_width_in_scu,
	CPMV mvp[4][2], U2* vertex_num, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], U10 pix_x_scu, int lidx)
{
	S14 neb_mv[4][2] = { { 0 } };
	BOOL is_top_ctu_boundary = neb_y_scu_of_ctu < 0 ? 1 : 0;
	BOOL is_left_ctu_boundary = neb_x_scu_of_ctu < 0 ? 1 : 0;
	BOOL is_right_ctu_boundary = neb_x_scu_of_ctu >= 1 << (5 - 1) ? 1 : 0;
	U3 neb_log_w = 0;
	U3 neb_log_h = 0;
	if (is_top_ctu_boundary) {
		neb_log_w = me_line_map_ptr->new_map_cu_mode_above_line[neb_x_scu_of_ctu + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET] >> 24 & (0x0F);
		neb_log_h = me_line_map_ptr->new_map_cu_mode_above_line[neb_x_scu_of_ctu + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET] >> 28 & (0x0F);
	}
	else {
		neb_log_w = me_line_map_ptr->map_cu_mode_inner[is_left_ctu_boundary ? U1(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][neb_x_scu_of_ctu + (is_left_ctu_boundary ? (1 << 5 - 1) : 0)][neb_y_scu_of_ctu] >> 24 & (0x0F);
		neb_log_h = me_line_map_ptr->map_cu_mode_inner[is_left_ctu_boundary ? U1(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][neb_x_scu_of_ctu + (is_left_ctu_boundary ? (1 << 5 - 1) : 0)][neb_y_scu_of_ctu] >> 28 & (0x0F);
	}
	U7 neb_w = 1 << neb_log_w;
	U7 neb_h = 1 << neb_log_h;
	S6 x_scu_of_ctu[4] = {};
	S6 y_scu_of_ctu[4] = {};
	x_scu_of_ctu[0] = neb_x_scu_of_ctu;
	x_scu_of_ctu[1] = neb_x_scu_of_ctu + (neb_w >> 2) - 1;
	x_scu_of_ctu[2] = neb_x_scu_of_ctu;
	x_scu_of_ctu[3] = neb_x_scu_of_ctu + (neb_w >> 2) - 1;
	y_scu_of_ctu[0] = ((is_top_ctu_boundary) ? S6(neb_y_scu_of_ctu + (neb_h >> 2) - 1) : neb_y_scu_of_ctu);
	y_scu_of_ctu[1] = ((is_top_ctu_boundary) ? S6(neb_y_scu_of_ctu + (neb_h >> 2) - 1) : neb_y_scu_of_ctu);
	y_scu_of_ctu[2] = neb_y_scu_of_ctu + (neb_h >> 2) - 1;
	y_scu_of_ctu[3] = neb_y_scu_of_ctu + (neb_h >> 2) - 1;
	for (int i = 0; i < 4; i++) {

#pragma HLS loop_tripcount max = 4
		if (is_top_ctu_boundary) {
			neb_mv[i][0] = above_nebs_line[x_scu_of_ctu[i] + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][0];
			neb_mv[i][1] = above_nebs_line[x_scu_of_ctu[i] + pix_x_scu - me_line_map_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][1];
		}
		else {
			neb_mv[i][0] = inner_nebs[is_left_ctu_boundary ? U1(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][x_scu_of_ctu[i] + (is_left_ctu_boundary ? (1 << 5 - 1) : 0)][y_scu_of_ctu[i]].mv[lidx][0];
			neb_mv[i][1] = inner_nebs[is_left_ctu_boundary ? U1(1 - me_line_map_ptr->curr_ctu_idx) : me_line_map_ptr->curr_ctu_idx][x_scu_of_ctu[i] + (is_left_ctu_boundary ? (1 << 5 - 1) : 0)][y_scu_of_ctu[i]].mv[lidx][1];
		}
	}
	U6 cur_x = cur_x_scu_of_ctu << 2;
	U6 cur_y = cur_y_scu_of_ctu << 2;
	S8 neb_x = neb_x_scu_of_ctu << 2;
	S8 neb_y = (is_top_ctu_boundary) ? S8(cur_y) : S8(neb_y_scu_of_ctu << 2);
	U3 diff_w = 7 - neb_log_w;
	U3 diff_h = 7 - neb_log_h;

	CPMV dmv_ver_x = 0;
	CPMV dmv_ver_y = 0;

	// deltaMvHor
	CPMV dmv_hor_x = ((neb_mv[1][0] - neb_mv[0][0])) << diff_w;
	CPMV dmv_hor_y = ((neb_mv[1][1] - neb_mv[0][1])) << diff_w;
	if (*vertex_num == 3 && !is_top_ctu_boundary) {
		// deltaMvVer
		dmv_ver_x = ((neb_mv[2][0] - neb_mv[0][0])) << diff_h;
		dmv_ver_y = ((neb_mv[2][1] - neb_mv[0][1])) << diff_h;
	}
	else {
		// deltaMvVer
		dmv_ver_x = -dmv_hor_y;
		dmv_ver_y = dmv_hor_x;
		*vertex_num = 2;
	}
	CPMV hor_base = (neb_mv[0][0]) << 7;
	CPMV ver_base = (neb_mv[0][1]) << 7;
	// derive CPMV 0
	CPMV tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y) + hor_base;
	CPMV tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y) + ver_base;
	com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
	tmp_hor <<= 2;
	tmp_ver <<= 2;
	mvp[0][0] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)))));
	mvp[0][1] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)))));
	// derive CPMV 1
	tmp_hor = dmv_hor_x * (cur_x - neb_x + cu_width) + dmv_ver_x * (cur_y - neb_y) + hor_base;
	tmp_ver = dmv_hor_y * (cur_x - neb_x + cu_width) + dmv_ver_y * (cur_y - neb_y) + ver_base;
	com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
	tmp_hor <<= 2;
	tmp_ver <<= 2;
	mvp[1][0] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_hor)))));
	mvp[1][1] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
		1 << (18 - 1)) - 1)) : tmp_ver)))));
	// derive CPMV 2
	if (*vertex_num == 3) {
		tmp_hor = dmv_hor_x * (cur_x - neb_x) + dmv_ver_x * (cur_y - neb_y + cu_height) + hor_base;
		tmp_ver = dmv_hor_y * (cur_x - neb_x) + dmv_ver_y * (cur_y - neb_y + cu_height) + ver_base;
		com_mv_rounding_s32(tmp_hor, tmp_ver, &tmp_hor, &tmp_ver, 7, 0);
		tmp_hor <<= 2;
		tmp_ver <<= 2;
		mvp[2][0] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_hor)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_hor ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_hor)))));
		mvp[2][1] = ((((CPMV(-(1 << (18 - 1)))) > (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_ver)) ? (CPMV(-(1 << (18 - 1)))) : (((CPMV((1 << (18 - 1)) - 1)) < tmp_ver ? (CPMV((
			1 << (18 - 1)) - 1)) : tmp_ver)))));
	}
	for (int i = 0; i < *vertex_num; ++i) {

#pragma HLS loop_tripcount max = 3
		if (abs(mvp[i][0]) >> 4 > SRW) {
			return 0;
		}
		if (abs(mvp[i][1]) >> 4 > SRH) {
			return 0;
		}
	}
	return 1;
}

static void com_mc_l_00(pel *ref, int gmv_x, int gmv_y, U10 s_ref, U7 s_pred, pel *pred, U4 w, U4 h, U1 bHpFilter)
{
	U4 i;
	U4 j;
	int offset_ref = 0;
	int offset_pred = 0;
	if (bHpFilter) {
		gmv_x >>= 4;
		gmv_y >>= 4;
	}
	else {
		gmv_x >>= 2;
		gmv_y >>= 2;
	}
	offset_ref += gmv_y * s_ref + gmv_x;
	{
		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				//printf("ref[%d]=%d\n", j + offset_ref, ref[j + offset_ref]);
				pred[j + offset_pred] = ref[j + offset_ref];
				//printf("pred[%d]=%d\n", j + offset_pred, pred[j + offset_pred]);
			}
			offset_pred += s_pred;
			offset_ref += s_ref;
		}
	}
}
//static const s16 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (57), (19), ((s16)(-7)), (3), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-7)), (19), (57), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((s16)(-3)), (63), (4), ((s16)(-2)), (1), 0 }, { ((s16)(-1)), (2), ((s16)(-5)), (62), (8), ((s16)(-3)), (1), 0 }, { ((s16)(-1)), (3), ((s16)(-8)), (60), (13), ((s16)(-4)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (58), (17), ((s16)(-5)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-11)), (52), (26), ((s16)(-8)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-9)), (47), (31), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (45), (34), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (34), (45), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (31), (47), ((s16)(-9)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-8)), (26), (52), ((s16)(-11)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-5)), (17), (58), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-4)), (13), (60), ((s16)(-8)), (3), ((s16)(-1)) }, { 0, (1), ((s16)(-3)), (8), (62), ((s16)(-5)), (2), ((s16)(-1)) }, { 0, (1), ((s16)(-2)), (4), (63), ((s16)(-3)), (1), 0 } } };

static void com_mc_l_0n(pel *ref, int gmv_x, int gmv_y, U10 s_ref, U7 s_pred, pel *pred, U4 w, U4 h, U4 bit_depth,
	U1 bHpFilter)
{
	static const S8 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (57), (19), ((s16)(-7)), (3), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-7)), (19), (57), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((s16)(-3)), (63), (4), ((s16)(-2)), (1), 0 }, { ((s16)(-1)), (2), ((s16)(-5)), (62), (8), ((s16)(-3)), (1), 0 }, { ((s16)(-1)), (3), ((s16)(-8)), (60), (13), ((s16)(-4)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (58), (17), ((s16)(-5)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-11)), (52), (26), ((s16)(-8)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-9)), (47), (31), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (45), (34), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (34), (45), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (31), (47), ((s16)(-9)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-8)), (26), (52), ((s16)(-11)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-5)), (17), (58), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-4)), (13), (60), ((s16)(-8)), (3), ((s16)(-1)) }, { 0, (1), ((s16)(-3)), (8), (62), ((s16)(-5)), (2), ((s16)(-1)) }, { 0, (1), ((s16)(-2)), (4), (63), ((s16)(-3)), (1), 0 } } };

	int dy;
	int offset_ref = 0;
	int offset_pred = 0;
	U4 i;
	U4 j;
	s32 pt;
	if (bHpFilter) {
		dy = gmv_y & 15;
		offset_ref += ((gmv_y >> 4) - 3) * s_ref + (gmv_x >> 4);
	}
	else {
		dy = gmv_y & 0x3;
		offset_ref += ((gmv_y >> 2) - 3) * s_ref + (gmv_x >> 2);
	}
	//const s16 *coeff_ver = bHpFilter ? tbl_mc_l_coeff_hp[dy] : tbl_mc_l_coeff[dy];
	//const s16 *coeff_ver = tbl_mc_l_coeff_all[bHpFilter][dy];
	{

		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				//printf("ref[%d]=%d ", j + offset_ref, ref[j + offset_ref]);
				//printf("ref[%d]=%d ", s_ref + j + offset_ref, ref[s_ref + j + offset_ref]);
				//printf("ref[%d]=%d ", s_ref * 2 + j + offset_ref, ref[s_ref * 2 + j + offset_ref]);
				//printf("ref[%d]=%d ", s_ref * 3 + j + offset_ref, ref[s_ref * 3 + j + offset_ref]);
				//printf("ref[%d]=%d ", s_ref * 4 + j + offset_ref, ref[s_ref * 4 + j + offset_ref]);
				//printf("ref[%d]=%d ", s_ref * 5 + j + offset_ref, ref[s_ref * 5 + j + offset_ref]);
				//printf("ref[%d]=%d ", s_ref * 6 + j + offset_ref, ref[s_ref * 6 + j + offset_ref]);
				//printf("ref[%d]=%d\n", s_ref * 7 + j + offset_ref, ref[s_ref * 7 + j + offset_ref]);
				pt = ((tbl_mc_l_coeff_all[bHpFilter][dy][0]) * (ref[j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][1]) * (ref[s_ref + j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][2]) * (ref[s_ref * 2 + j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][3]) * (ref[s_ref * 3 + j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][4]) * (ref[s_ref * 4 + j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][5]) * (ref[s_ref * 5 + j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][6]) * (ref[s_ref * 6 + j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][7]) * (ref[s_ref * 7 + j + offset_ref]) + (1 << 5)) >> 6;
				pred[j + offset_pred] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				//printf("pred[%d]=%d\n", j + offset_pred, pred[j + offset_pred]);
			}
			offset_ref += s_ref;
			offset_pred += s_pred;
		}
	}
}

static void com_mc_l_n0(pel *ref, int gmv_x, int gmv_y, U10 s_ref, U7 s_pred, pel *pred, U4 w, U4 h, U4 bit_depth,
	U1 bHpFilter)
{
	static const S8 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (57), (19), ((s16)(-7)), (3), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-7)), (19), (57), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((s16)(-3)), (63), (4), ((s16)(-2)), (1), 0 }, { ((s16)(-1)), (2), ((s16)(-5)), (62), (8), ((s16)(-3)), (1), 0 }, { ((s16)(-1)), (3), ((s16)(-8)), (60), (13), ((s16)(-4)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (58), (17), ((s16)(-5)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-11)), (52), (26), ((s16)(-8)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-9)), (47), (31), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (45), (34), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (34), (45), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (31), (47), ((s16)(-9)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-8)), (26), (52), ((s16)(-11)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-5)), (17), (58), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-4)), (13), (60), ((s16)(-8)), (3), ((s16)(-1)) }, { 0, (1), ((s16)(-3)), (8), (62), ((s16)(-5)), (2), ((s16)(-1)) }, { 0, (1), ((s16)(-2)), (4), (63), ((s16)(-3)), (1), 0 } } };

	int dx;
	int offset_ref = 0;
	int offset_pred = 0;
	U4 i;
	U4 j;
	s32 pt;
	if (bHpFilter) {
		dx = gmv_x & 15;
		offset_ref += (gmv_y >> 4) * s_ref + (gmv_x >> 4) - 3;
	}
	else {
		dx = gmv_x & 0x3;
		offset_ref += (gmv_y >> 2) * s_ref + (gmv_x >> 2) - 3;
	}
	//const s16 *coeff_hor = bHpFilter ? tbl_mc_l_coeff_hp[dx] : tbl_mc_l_coeff[dx];
	//const s16 *coeff_hor = tbl_mc_l_coeff_all[bHpFilter][dx];
	{

		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				//printf("ref[%d]=%d ", j + offset_ref, ref[j + offset_ref]);
				//printf("ref[%d]=%d ", 1 + j + offset_ref, ref[1 + j + offset_ref]);
				//printf("ref[%d]=%d ", 1 * 2 + j + offset_ref, ref[1 * 2 + j + offset_ref]);
				//printf("ref[%d]=%d ", 1 * 3 + j + offset_ref, ref[1 * 3 + j + offset_ref]);
				//printf("ref[%d]=%d ", 1 * 4 + j + offset_ref, ref[1 * 4 + j + offset_ref]);
				//printf("ref[%d]=%d ", 1 * 5 + j + offset_ref, ref[1 * 5 + j + offset_ref]);
				//printf("ref[%d]=%d ", 1 * 6 + j + offset_ref, ref[1 * 6 + j + offset_ref]);
				//printf("ref[%d]=%d\n", 1 * 7 + j + offset_ref, ref[1 * 7 + j + offset_ref]);
				pt = ((tbl_mc_l_coeff_all[bHpFilter][dx][0]) * (ref[j + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][1]) * (ref[j + 1 + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][2]) * (ref[j + 2 + offset_ref]) + (tbl_mc_l_coeff_all[bHpFilter][dx][3])
					* (ref[j + 3 + offset_ref]) + (tbl_mc_l_coeff_all[bHpFilter][dx][4]) * (ref[j + 4 + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][5]) * (ref[j + 5 + offset_ref]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][6]) * (ref[j + 6 + offset_ref]) + (tbl_mc_l_coeff_all[bHpFilter][dx][7])
					* (ref[j + 7 + offset_ref]) + (1 << 5)) >> 6;
				pred[j + offset_pred] = ((pel)((0 >(((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				//printf("pred[%d]=%d\n", j + offset_pred, pred[j + offset_pred]);
			}
			offset_ref += s_ref;
			offset_pred += s_pred;
		}
	}
}

static void com_mc_l_nn(pel *ref, int gmv_x, int gmv_y, U10 s_ref, U7 s_pred, s16 *pred, U4 w, U4 h, U4 bit_depth,
	U1 bHpFilter)
{
	static const S8 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (57), (19), ((s16)(-7)), (3), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-7)), (19), (57), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((s16)(-3)), (63), (4), ((s16)(-2)), (1), 0 }, { ((s16)(-1)), (2), ((s16)(-5)), (62), (8), ((s16)(-3)), (1), 0 }, { ((s16)(-1)), (3), ((s16)(-8)), (60), (13), ((s16)(-4)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (58), (17), ((s16)(-5)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-11)), (52), (26), ((s16)(-8)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-9)), (47), (31), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (45), (34), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (34), (45), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (31), (47), ((s16)(-9)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-8)), (26), (52), ((s16)(-11)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-5)), (17), (58), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-4)), (13), (60), ((s16)(-8)), (3), ((s16)(-1)) }, { 0, (1), ((s16)(-3)), (8), (62), ((s16)(-5)), (2), ((s16)(-1)) }, { 0, (1), ((s16)(-2)), (4), (63), ((s16)(-3)), (1), 0 } } };

	s16 buf[64 + 8][128 >> 1];

#pragma HLS ARRAY_PARTITION variable=buf block factor=72 dim=1 partition
	int dx;
	int dy;
	int offset_ref = 0;
	int offset_pred = 0;
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = (1 << (shift2 - 1));
	s16 ref_temp[8];
	U4 i;
	U4 j;
	//s16 * b;
	s32 pt;

	if (bHpFilter) {
		dx = gmv_x & 15;
		dy = gmv_y & 15;
		offset_ref += ((gmv_y >> 4) - 3) * s_ref + (gmv_x >> 4) - 3;
	}
	else {
		dx = gmv_x & 0x3;
		dy = gmv_y & 0x3;
		offset_ref += ((gmv_y >> 2) - 3) * s_ref + (gmv_x >> 2) - 3;
	}


#pragma HLS ARRAY_PARTITION variable=ref_temp complete dim=0
	{

		//b = buf;
		for (i = 0; i < h + 7; i++) {

#pragma HLS loop_tripcount max = 71
			//printf("ref[%d]=%d ", 0 + offset_ref, ref[0 + offset_ref]);
			//printf("ref[%d]=%d ", 1 + 0 + offset_ref, ref[1 + 0 + offset_ref]);
			//printf("ref[%d]=%d ", 1 * 2 + 0 + offset_ref, ref[1 * 2 + 0 + offset_ref]);
			//printf("ref[%d]=%d ", 1 * 3 + 0 + offset_ref, ref[1 * 3 + 0 + offset_ref]);
			//printf("ref[%d]=%d ", 1 * 4 + 0 + offset_ref, ref[1 * 4 + 0 + offset_ref]);
			//printf("ref[%d]=%d ", 1 * 5 + 0 + offset_ref, ref[1 * 5 + 0 + offset_ref]);
			//printf("ref[%d]=%d ", 1 * 6 + 0 + offset_ref, ref[1 * 6 + 0 + offset_ref]);
			ref_temp[0] = ref[offset_ref];
			ref_temp[1] = ref[1 + offset_ref];
			ref_temp[2] = ref[2 + offset_ref];
			ref_temp[3] = ref[3 + offset_ref];
			ref_temp[4] = ref[4 + offset_ref];
			ref_temp[5] = ref[5 + offset_ref];
			ref_temp[6] = ref[6 + offset_ref];
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				//printf("ref[%d]=%d\n", 1 * 7 + j + offset_ref, ref[1 * 7 + j + offset_ref]);
				ref_temp[7] = ref[j + 7 + offset_ref];
				buf[i][j] = ((s16)(((tbl_mc_l_coeff_all[bHpFilter][dx][0]) * (ref_temp[0]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][1]) * (ref_temp[1]) + (tbl_mc_l_coeff_all[bHpFilter][dx][2]) * (
					ref_temp[2]) + (tbl_mc_l_coeff_all[bHpFilter][dx][3]) * (ref_temp[3]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][4]) * (ref_temp[4]) + (tbl_mc_l_coeff_all[bHpFilter][dx][5]) * (
					ref_temp[5]) + (tbl_mc_l_coeff_all[bHpFilter][dx][6]) * (ref_temp[6]) + (
					tbl_mc_l_coeff_all[bHpFilter][dx][7]) * (ref_temp[7]) + add1) >> shift1));
				ref_temp[0] = ref_temp[1];
				ref_temp[1] = ref_temp[2];
				ref_temp[2] = ref_temp[3];
				ref_temp[3] = ref_temp[4];
				ref_temp[4] = ref_temp[5];
				ref_temp[5] = ref_temp[6];
				ref_temp[6] = ref_temp[7];
			}
			offset_ref += s_ref;
		}
		//b = buf;
		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				//pt = (MAC_8TAP(tbl_mc_l_coeff_all[bHpFilter][dy], buf[j + offset_buf], buf[j + w + offset_buf], buf[j + w * 2 + offset_buf], buf[j + w * 3 + offset_buf], buf[j + w * 4 + offset_buf], buf[j + w * 5 + offset_buf], buf[j + w * 6 + offset_buf], buf[j + w * 7 + offset_buf]) + add2) >> shift2;
				pt = ((tbl_mc_l_coeff_all[bHpFilter][dy][0]) * (buf[i][j]) + (tbl_mc_l_coeff_all[bHpFilter][dy][1])
					* (buf[i + 1][j]) + (tbl_mc_l_coeff_all[bHpFilter][dy][2]) * (buf[i + 2][j]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][3]) * (buf[i + 3][j]) + (tbl_mc_l_coeff_all[bHpFilter][dy][4]) * (
					buf[i + 4][j]) + (tbl_mc_l_coeff_all[bHpFilter][dy][5]) * (buf[i + 5][j]) + (
					tbl_mc_l_coeff_all[bHpFilter][dy][6]) * (buf[i + 6][j]) + (tbl_mc_l_coeff_all[bHpFilter][dy][7]) * (
					buf[i + 7][j]) + add2) >> shift2;
				pred[j + offset_pred] = ((pel)((0 >(((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				//printf("pred[%d]=%d\n", j + offset_pred, pred[j + offset_pred]);
			}
			offset_pred += s_pred;
		}
	}
}

static void com_tbl_mc_l_1(int ori_mv_x, int ori_mv_y, pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 *pred,
	int w, int h, int bit_depth)
{
	//add by luke
	int dx;
	int dy;
	dx = ori_mv_x & 0x3;
	dy = ori_mv_y & 0x3;
	if (dx == 0 && dy == 0) {
		com_mc_l_00(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, 0);
	}
	else if (dx == 0 && dy != 0) {
		com_mc_l_0n(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}
	else if (dx != 0 && dy == 0) {
		com_mc_l_n0(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}
	else if (dx != 0 && dy != 0) {
		com_mc_l_nn(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}
}

static void mv_in_ref_window(const int x_pos, const int y_pos, const int cu_width, const int cu_height, const int pic_w, const int pic_h, const S14 mv[MV_D], S12 qpel_gmv_of_refp_cu[MV_D])
{
#pragma HLS INLINE OFF
	int x_ext = x_pos << 2;
	int y_ext = y_pos << 2;
	int w_ext = cu_width << 2;
	int h_ext = cu_height << 2;

	int min_clip[MV_D];
	int max_clip[MV_D];
	s16 mv_t[2];
#pragma HLS array_partition variable=min_clip complete dim=0
#pragma HLS array_partition variable=max_clip complete dim=0
#pragma HLS array_partition variable=mv_t complete dim=0

	min_clip[MV_X] = ((-MAX_CU_SIZE - 4) << 2) - x_ext;
	min_clip[MV_Y] = ((-MAX_CU_SIZE - 4) << 2) - y_ext;
	max_clip[MV_X] = ((pic_w - 1 + MAX_CU_SIZE + 4) << 2) - x_ext - w_ext + 4;
	max_clip[MV_Y] = ((pic_h - 1 + MAX_CU_SIZE + 4) << 2) - y_ext - h_ext + 4;

	mv_t[MV_X] = COM_CLIP3(min_clip[MV_X], max_clip[MV_X], (int)mv[MV_X]);
	mv_t[MV_Y] = COM_CLIP3(min_clip[MV_Y], max_clip[MV_Y], (int)mv[MV_Y]);

	qpel_gmv_of_refp_cu[MV_X] = ((x_ext & ((MAX_CU_SIZE2 << 2) - 1)) + mv_t[MV_X] + (REFP_SURD_OFF_W << 2)) >> 2;
	qpel_gmv_of_refp_cu[MV_Y] = ((y_ext & ((MAX_CU_SIZE2 << 2) - 1)) + mv_t[MV_Y] + (REFP_SURD_OFF_H << 2)) >> 2;
}

static BOOL com_mc_channel_l(CU_POS *cu, int pic_w, int pic_h, s8 refi, s16 mv[2], int stride_luma, pel *ref_y,
	pel *pred, int bit_depth)
{
	s16 mv_t[2];
	int min_clip[2];
	int max_clip[2];
	int x_ext = cu->x_pos << 2;
	int y_ext = cu->y_pos << 2;
	int w_ext = cu->cu_width << 2;
	int h_ext = cu->cu_height << 2;
	int x_ext_of_ctu;
	int y_ext_of_ctu;
	int qpel_gmv_x_of_ctu;
	int qpel_gmv_y_of_ctu;
	int qpel_gmv_x_of_refp_cu;
	int qpel_gmv_y_of_refp_cu;



	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0] = mv[0];
	mv_t[1] = mv[1];
	if (refi >= 0) {
		if (x_ext + mv[0] < min_clip[0]) {
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + mv[1] < min_clip[1]) {
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + mv[0] + w_ext - 4 > max_clip[0]) {
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + mv[1] + h_ext - 4 > max_clip[1]) {
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
		y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);
		qpel_gmv_x_of_ctu = x_ext_of_ctu + mv_t[0];
		qpel_gmv_y_of_ctu = y_ext_of_ctu + mv_t[1];
		qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
		qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);

		if (cu->cu_width < 16 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width - 16 - 3 < 0){ return FALSE; }
		if (cu->cu_height < 16 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height - 16 - 3 < 0){ return FALSE; }
		if ((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < SWH){


			com_tbl_mc_l_1(mv[0], mv[1], ref_y, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, stride_luma, 1 << 7 >> 1,
				pred, cu->cu_width, cu->cu_height, bit_depth);
			return 1;
		}
		else {
			return 0;
		}
	}
	return 0;
}

static int com_had_16x8(pel *org, pel *cur, U13 s_org, U12 s_cur, U4 bit_depth)
{
	u8 k;
	u8 i;
	u8 j;
	u8 jj;
	int satd = 0;
	pel diff[128];
	pel m1[8][16];
	pel m2[8][16];
	for (k = 0; k < 128; k += 16) {
		diff[k + 0] = (org[0] - cur[0]);
		diff[k + 1] = (org[1] - cur[1]);
		diff[k + 2] = (org[2] - cur[2]);
		diff[k + 3] = (org[3] - cur[3]);
		diff[k + 4] = (org[4] - cur[4]);
		diff[k + 5] = (org[5] - cur[5]);
		diff[k + 6] = (org[6] - cur[6]);
		diff[k + 7] = (org[7] - cur[7]);
		diff[k + 8] = (org[8] - cur[8]);
		diff[k + 9] = (org[9] - cur[9]);
		diff[k + 10] = (org[10] - cur[10]);
		diff[k + 11] = (org[11] - cur[11]);
		diff[k + 12] = (org[12] - cur[12]);
		diff[k + 13] = (org[13] - cur[13]);
		diff[k + 14] = (org[14] - cur[14]);
		diff[k + 15] = (org[15] - cur[15]);
		cur += s_cur;
		org += s_org;
	}
	for (j = 0; j < 8; j++) {
		jj = j << 4;
		m2[j][0] = diff[jj] + diff[jj + 8];
		m2[j][1] = diff[jj + 1] + diff[jj + 9];
		m2[j][2] = diff[jj + 2] + diff[jj + 10];
		m2[j][3] = diff[jj + 3] + diff[jj + 11];
		m2[j][4] = diff[jj + 4] + diff[jj + 12];
		m2[j][5] = diff[jj + 5] + diff[jj + 13];
		m2[j][6] = diff[jj + 6] + diff[jj + 14];
		m2[j][7] = diff[jj + 7] + diff[jj + 15];
		m2[j][8] = diff[jj] - diff[jj + 8];
		m2[j][9] = diff[jj + 1] - diff[jj + 9];
		m2[j][10] = diff[jj + 2] - diff[jj + 10];
		m2[j][11] = diff[jj + 3] - diff[jj + 11];
		m2[j][12] = diff[jj + 4] - diff[jj + 12];
		m2[j][13] = diff[jj + 5] - diff[jj + 13];
		m2[j][14] = diff[jj + 6] - diff[jj + 14];
		m2[j][15] = diff[jj + 7] - diff[jj + 15];
		m1[j][0] = m2[j][0] + m2[j][4];
		m1[j][1] = m2[j][1] + m2[j][5];
		m1[j][2] = m2[j][2] + m2[j][6];
		m1[j][3] = m2[j][3] + m2[j][7];
		m1[j][4] = m2[j][0] - m2[j][4];
		m1[j][5] = m2[j][1] - m2[j][5];
		m1[j][6] = m2[j][2] - m2[j][6];
		m1[j][7] = m2[j][3] - m2[j][7];
		m1[j][8] = m2[j][8] + m2[j][12];
		m1[j][9] = m2[j][9] + m2[j][13];
		m1[j][10] = m2[j][10] + m2[j][14];
		m1[j][11] = m2[j][11] + m2[j][15];
		m1[j][12] = m2[j][8] - m2[j][12];
		m1[j][13] = m2[j][9] - m2[j][13];
		m1[j][14] = m2[j][10] - m2[j][14];
		m1[j][15] = m2[j][11] - m2[j][15];
		m2[j][0] = m1[j][0] + m1[j][2];
		m2[j][1] = m1[j][1] + m1[j][3];
		m2[j][2] = m1[j][0] - m1[j][2];
		m2[j][3] = m1[j][1] - m1[j][3];
		m2[j][4] = m1[j][4] + m1[j][6];
		m2[j][5] = m1[j][5] + m1[j][7];
		m2[j][6] = m1[j][4] - m1[j][6];
		m2[j][7] = m1[j][5] - m1[j][7];
		m2[j][8] = m1[j][8] + m1[j][10];
		m2[j][9] = m1[j][9] + m1[j][11];
		m2[j][10] = m1[j][8] - m1[j][10];
		m2[j][11] = m1[j][9] - m1[j][11];
		m2[j][12] = m1[j][12] + m1[j][14];
		m2[j][13] = m1[j][13] + m1[j][15];
		m2[j][14] = m1[j][12] - m1[j][14];
		m2[j][15] = m1[j][13] - m1[j][15];
		m1[j][0] = m2[j][0] + m2[j][1];
		m1[j][1] = m2[j][0] - m2[j][1];
		m1[j][2] = m2[j][2] + m2[j][3];
		m1[j][3] = m2[j][2] - m2[j][3];
		m1[j][4] = m2[j][4] + m2[j][5];
		m1[j][5] = m2[j][4] - m2[j][5];
		m1[j][6] = m2[j][6] + m2[j][7];
		m1[j][7] = m2[j][6] - m2[j][7];
		m1[j][8] = m2[j][8] + m2[j][9];
		m1[j][9] = m2[j][8] - m2[j][9];
		m1[j][10] = m2[j][10] + m2[j][11];
		m1[j][11] = m2[j][10] - m2[j][11];
		m1[j][12] = m2[j][12] + m2[j][13];
		m1[j][13] = m2[j][12] - m2[j][13];
		m1[j][14] = m2[j][14] + m2[j][15];
		m1[j][15] = m2[j][14] - m2[j][15];
	}
	for (i = 0; i < 16; i++) {
		m2[0][i] = m1[0][i] + m1[4][i];
		m2[1][i] = m1[1][i] + m1[5][i];
		m2[2][i] = m1[2][i] + m1[6][i];
		m2[3][i] = m1[3][i] + m1[7][i];
		m2[4][i] = m1[0][i] - m1[4][i];
		m2[5][i] = m1[1][i] - m1[5][i];
		m2[6][i] = m1[2][i] - m1[6][i];
		m2[7][i] = m1[3][i] - m1[7][i];
		m1[0][i] = m2[0][i] + m2[2][i];
		m1[1][i] = m2[1][i] + m2[3][i];
		m1[2][i] = m2[0][i] - m2[2][i];
		m1[3][i] = m2[1][i] - m2[3][i];
		m1[4][i] = m2[4][i] + m2[6][i];
		m1[5][i] = m2[5][i] + m2[7][i];
		m1[6][i] = m2[4][i] - m2[6][i];
		m1[7][i] = m2[5][i] - m2[7][i];
		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
		m2[4][i] = m1[4][i] + m1[5][i];
		m2[5][i] = m1[4][i] - m1[5][i];
		m2[6][i] = m1[6][i] + m1[7][i];
		m2[7][i] = m1[6][i] - m1[7][i];
	}
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 16; j++) {
			satd += abs(m2[i][j]);
		}
	}

	//satd = ((int)(((double)satd) / sqrt(16.0 * 8.0) * 2.0));
	satd = (int)(((s64)satd * 20000 + 56569) / 113137);//fix_point num = 10000
	return satd;
}

static int com_had_8x16(pel *org, pel *cur, U13 s_org, U12 s_cur, U4 bit_depth)
{
	u8 k;
	u8 i;
	u8 j;
	u8 jj;
	int satd = 0;
	pel diff[128];
	pel m1[16][8];
	pel m2[16][8];
	for (k = 0; k < 128; k += 8) {
		diff[k + 0] = (org[0] - cur[0]);
		diff[k + 1] = (org[1] - cur[1]);
		diff[k + 2] = (org[2] - cur[2]);
		diff[k + 3] = (org[3] - cur[3]);
		diff[k + 4] = (org[4] - cur[4]);
		diff[k + 5] = (org[5] - cur[5]);
		diff[k + 6] = (org[6] - cur[6]);
		diff[k + 7] = (org[7] - cur[7]);
		cur += s_cur;
		org += s_org;
	}
	for (j = 0; j < 16; j++) {
		jj = j << 3;
		m2[j][0] = diff[jj] + diff[jj + 4];
		m2[j][1] = diff[jj + 1] + diff[jj + 5];
		m2[j][2] = diff[jj + 2] + diff[jj + 6];
		m2[j][3] = diff[jj + 3] + diff[jj + 7];
		m2[j][4] = diff[jj] - diff[jj + 4];
		m2[j][5] = diff[jj + 1] - diff[jj + 5];
		m2[j][6] = diff[jj + 2] - diff[jj + 6];
		m2[j][7] = diff[jj + 3] - diff[jj + 7];
		m1[j][0] = m2[j][0] + m2[j][2];
		m1[j][1] = m2[j][1] + m2[j][3];
		m1[j][2] = m2[j][0] - m2[j][2];
		m1[j][3] = m2[j][1] - m2[j][3];
		m1[j][4] = m2[j][4] + m2[j][6];
		m1[j][5] = m2[j][5] + m2[j][7];
		m1[j][6] = m2[j][4] - m2[j][6];
		m1[j][7] = m2[j][5] - m2[j][7];
		m2[j][0] = m1[j][0] + m1[j][1];
		m2[j][1] = m1[j][0] - m1[j][1];
		m2[j][2] = m1[j][2] + m1[j][3];
		m2[j][3] = m1[j][2] - m1[j][3];
		m2[j][4] = m1[j][4] + m1[j][5];
		m2[j][5] = m1[j][4] - m1[j][5];
		m2[j][6] = m1[j][6] + m1[j][7];
		m2[j][7] = m1[j][6] - m1[j][7];
	}
	for (i = 0; i < 8; i++) {
		m1[0][i] = m2[0][i] + m2[8][i];
		m1[1][i] = m2[1][i] + m2[9][i];
		m1[2][i] = m2[2][i] + m2[10][i];
		m1[3][i] = m2[3][i] + m2[11][i];
		m1[4][i] = m2[4][i] + m2[12][i];
		m1[5][i] = m2[5][i] + m2[13][i];
		m1[6][i] = m2[6][i] + m2[14][i];
		m1[7][i] = m2[7][i] + m2[15][i];
		m1[8][i] = m2[0][i] - m2[8][i];
		m1[9][i] = m2[1][i] - m2[9][i];
		m1[10][i] = m2[2][i] - m2[10][i];
		m1[11][i] = m2[3][i] - m2[11][i];
		m1[12][i] = m2[4][i] - m2[12][i];
		m1[13][i] = m2[5][i] - m2[13][i];
		m1[14][i] = m2[6][i] - m2[14][i];
		m1[15][i] = m2[7][i] - m2[15][i];
		m2[0][i] = m1[0][i] + m1[4][i];
		m2[1][i] = m1[1][i] + m1[5][i];
		m2[2][i] = m1[2][i] + m1[6][i];
		m2[3][i] = m1[3][i] + m1[7][i];
		m2[4][i] = m1[0][i] - m1[4][i];
		m2[5][i] = m1[1][i] - m1[5][i];
		m2[6][i] = m1[2][i] - m1[6][i];
		m2[7][i] = m1[3][i] - m1[7][i];
		m2[8][i] = m1[8][i] + m1[12][i];
		m2[9][i] = m1[9][i] + m1[13][i];
		m2[10][i] = m1[10][i] + m1[14][i];
		m2[11][i] = m1[11][i] + m1[15][i];
		m2[12][i] = m1[8][i] - m1[12][i];
		m2[13][i] = m1[9][i] - m1[13][i];
		m2[14][i] = m1[10][i] - m1[14][i];
		m2[15][i] = m1[11][i] - m1[15][i];
		m1[0][i] = m2[0][i] + m2[2][i];
		m1[1][i] = m2[1][i] + m2[3][i];
		m1[2][i] = m2[0][i] - m2[2][i];
		m1[3][i] = m2[1][i] - m2[3][i];
		m1[4][i] = m2[4][i] + m2[6][i];
		m1[5][i] = m2[5][i] + m2[7][i];
		m1[6][i] = m2[4][i] - m2[6][i];
		m1[7][i] = m2[5][i] - m2[7][i];
		m1[8][i] = m2[8][i] + m2[10][i];
		m1[9][i] = m2[9][i] + m2[11][i];
		m1[10][i] = m2[8][i] - m2[10][i];
		m1[11][i] = m2[9][i] - m2[11][i];
		m1[12][i] = m2[12][i] + m2[14][i];
		m1[13][i] = m2[13][i] + m2[15][i];
		m1[14][i] = m2[12][i] - m2[14][i];
		m1[15][i] = m2[13][i] - m2[15][i];
		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
		m2[4][i] = m1[4][i] + m1[5][i];
		m2[5][i] = m1[4][i] - m1[5][i];
		m2[6][i] = m1[6][i] + m1[7][i];
		m2[7][i] = m1[6][i] - m1[7][i];
		m2[8][i] = m1[8][i] + m1[9][i];
		m2[9][i] = m1[8][i] - m1[9][i];
		m2[10][i] = m1[10][i] + m1[11][i];
		m2[11][i] = m1[10][i] - m1[11][i];
		m2[12][i] = m1[12][i] + m1[13][i];
		m2[13][i] = m1[12][i] - m1[13][i];
		m2[14][i] = m1[14][i] + m1[15][i];
		m2[15][i] = m1[14][i] - m1[15][i];
	}
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 8; j++) {
			satd += abs(m2[i][j]);
		}
	}

	//satd = ((int)(((double)satd) / sqrt(16.0 * 8.0) * 2.0));
	satd = (int)(((s64)satd * 20000 + 56569) / 113137);//fix_point num = 10000
	return satd;
}

static int com_had_8x4(pel *org, pel *cur, U13 s_org, U12 s_cur)
{
	int k;
	int i;
	int j;
	int jj;
	int satd = 0;
	int diff[32];
	int m1[4][8];
	int m2[4][8];
	for (k = 0; k < 32; k += 8) {
		diff[k + 0] = (org[0] - cur[0]);
		diff[k + 1] = (org[1] - cur[1]);
		diff[k + 2] = (org[2] - cur[2]);
		diff[k + 3] = (org[3] - cur[3]);
		diff[k + 4] = (org[4] - cur[4]);
		diff[k + 5] = (org[5] - cur[5]);
		diff[k + 6] = (org[6] - cur[6]);
		diff[k + 7] = (org[7] - cur[7]);
		cur += s_cur;
		org += s_org;
	}
	for (j = 0; j < 4; j++) {
		jj = j << 3;
		m2[j][0] = diff[jj] + diff[jj + 4];
		m2[j][1] = diff[jj + 1] + diff[jj + 5];
		m2[j][2] = diff[jj + 2] + diff[jj + 6];
		m2[j][3] = diff[jj + 3] + diff[jj + 7];
		m2[j][4] = diff[jj] - diff[jj + 4];
		m2[j][5] = diff[jj + 1] - diff[jj + 5];
		m2[j][6] = diff[jj + 2] - diff[jj + 6];
		m2[j][7] = diff[jj + 3] - diff[jj + 7];
		m1[j][0] = m2[j][0] + m2[j][2];
		m1[j][1] = m2[j][1] + m2[j][3];
		m1[j][2] = m2[j][0] - m2[j][2];
		m1[j][3] = m2[j][1] - m2[j][3];
		m1[j][4] = m2[j][4] + m2[j][6];
		m1[j][5] = m2[j][5] + m2[j][7];
		m1[j][6] = m2[j][4] - m2[j][6];
		m1[j][7] = m2[j][5] - m2[j][7];
		m2[j][0] = m1[j][0] + m1[j][1];
		m2[j][1] = m1[j][0] - m1[j][1];
		m2[j][2] = m1[j][2] + m1[j][3];
		m2[j][3] = m1[j][2] - m1[j][3];
		m2[j][4] = m1[j][4] + m1[j][5];
		m2[j][5] = m1[j][4] - m1[j][5];
		m2[j][6] = m1[j][6] + m1[j][7];
		m2[j][7] = m1[j][6] - m1[j][7];
	}
	for (i = 0; i < 8; i++) {
		m1[0][i] = m2[0][i] + m2[2][i];
		m1[1][i] = m2[1][i] + m2[3][i];
		m1[2][i] = m2[0][i] - m2[2][i];
		m1[3][i] = m2[1][i] - m2[3][i];
		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
	}
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 8; j++) {
			satd += abs(m2[i][j]);
		}
	}

	//satd = ((int)(((double)satd) / sqrt(4.0 * 8.0) * 2.0));
	satd = (int)(((s64)satd * 40000 + 56569) / 113137);//fix_point num = 20000

	return satd;
}

static int com_had_4x8(pel *org, pel *cur, U13 s_org, U12 s_cur)
{
	int k;
	int i;
	int j;
	int jj;
	int satd = 0;
	int diff[32];
	int m1[8][4];
	int m2[8][4];
	for (k = 0; k < 32; k += 4) {
		diff[k + 0] = (org[0] - cur[0]);
		diff[k + 1] = (org[1] - cur[1]);
		diff[k + 2] = (org[2] - cur[2]);
		diff[k + 3] = (org[3] - cur[3]);
		cur += s_cur;
		org += s_org;
	}
	for (j = 0; j < 8; j++) {
		jj = j << 2;
		m2[j][0] = diff[jj] + diff[jj + 2];
		m2[j][1] = diff[jj + 1] + diff[jj + 3];
		m2[j][2] = diff[jj] - diff[jj + 2];
		m2[j][3] = diff[jj + 1] - diff[jj + 3];
		m1[j][0] = m2[j][0] + m2[j][1];
		m1[j][1] = m2[j][0] - m2[j][1];
		m1[j][2] = m2[j][2] + m2[j][3];
		m1[j][3] = m2[j][2] - m2[j][3];
	}
	for (i = 0; i < 4; i++) {
		m2[0][i] = m1[0][i] + m1[4][i];
		m2[1][i] = m1[1][i] + m1[5][i];
		m2[2][i] = m1[2][i] + m1[6][i];
		m2[3][i] = m1[3][i] + m1[7][i];
		m2[4][i] = m1[0][i] - m1[4][i];
		m2[5][i] = m1[1][i] - m1[5][i];
		m2[6][i] = m1[2][i] - m1[6][i];
		m2[7][i] = m1[3][i] - m1[7][i];
		m1[0][i] = m2[0][i] + m2[2][i];
		m1[1][i] = m2[1][i] + m2[3][i];
		m1[2][i] = m2[0][i] - m2[2][i];
		m1[3][i] = m2[1][i] - m2[3][i];
		m1[4][i] = m2[4][i] + m2[6][i];
		m1[5][i] = m2[5][i] + m2[7][i];
		m1[6][i] = m2[4][i] - m2[6][i];
		m1[7][i] = m2[5][i] - m2[7][i];
		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
		m2[4][i] = m1[4][i] + m1[5][i];
		m2[5][i] = m1[4][i] - m1[5][i];
		m2[6][i] = m1[6][i] + m1[7][i];
		m2[7][i] = m1[6][i] - m1[7][i];
	}
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 4; j++) {
			satd += abs(m2[i][j]);
		}
	}

	//satd = ((int)(((double)satd) / sqrt(4.0 * 8.0) * 2.0));
	satd = (int)(((s64)satd * 40000 + 56569) / 113137);//fix_point num = 20000
	return satd;
}

static int com_had_8x8(pel *org, pel *cur, U13 s_org, U12 s_cur, U4 bit_depth)
{
	u8 k;
	u8 i;
	u8 j;
	u8 jj;
	int satd = 0;
	int diff[64];
	pel m1[8][8];
	pel m2[8][8];
	pel m3[8][8];
	for (k = 0; k < 64; k += 8) {
		diff[k + 0] = (org[0] - cur[0]);
		diff[k + 1] = (org[1] - cur[1]);
		diff[k + 2] = (org[2] - cur[2]);
		diff[k + 3] = (org[3] - cur[3]);
		diff[k + 4] = (org[4] - cur[4]);
		diff[k + 5] = (org[5] - cur[5]);
		diff[k + 6] = (org[6] - cur[6]);
		diff[k + 7] = (org[7] - cur[7]);
		cur += s_cur;
		org += s_org;
	}
	/* horizontal */
	for (j = 0; j < 8; j++) {
		jj = j << 3;
		m2[j][0] = diff[jj] + diff[jj + 4];
		m2[j][1] = diff[jj + 1] + diff[jj + 5];
		m2[j][2] = diff[jj + 2] + diff[jj + 6];
		m2[j][3] = diff[jj + 3] + diff[jj + 7];
		m2[j][4] = diff[jj] - diff[jj + 4];
		m2[j][5] = diff[jj + 1] - diff[jj + 5];
		m2[j][6] = diff[jj + 2] - diff[jj + 6];
		m2[j][7] = diff[jj + 3] - diff[jj + 7];
		m1[j][0] = m2[j][0] + m2[j][2];
		m1[j][1] = m2[j][1] + m2[j][3];
		m1[j][2] = m2[j][0] - m2[j][2];
		m1[j][3] = m2[j][1] - m2[j][3];
		m1[j][4] = m2[j][4] + m2[j][6];
		m1[j][5] = m2[j][5] + m2[j][7];
		m1[j][6] = m2[j][4] - m2[j][6];
		m1[j][7] = m2[j][5] - m2[j][7];
		m2[j][0] = m1[j][0] + m1[j][1];
		m2[j][1] = m1[j][0] - m1[j][1];
		m2[j][2] = m1[j][2] + m1[j][3];
		m2[j][3] = m1[j][2] - m1[j][3];
		m2[j][4] = m1[j][4] + m1[j][5];
		m2[j][5] = m1[j][4] - m1[j][5];
		m2[j][6] = m1[j][6] + m1[j][7];
		m2[j][7] = m1[j][6] - m1[j][7];
	}
	/* vertical */
	for (i = 0; i < 8; i++) {
		m3[0][i] = m2[0][i] + m2[4][i];
		m3[1][i] = m2[1][i] + m2[5][i];
		m3[2][i] = m2[2][i] + m2[6][i];
		m3[3][i] = m2[3][i] + m2[7][i];
		m3[4][i] = m2[0][i] - m2[4][i];
		m3[5][i] = m2[1][i] - m2[5][i];
		m3[6][i] = m2[2][i] - m2[6][i];
		m3[7][i] = m2[3][i] - m2[7][i];
		m1[0][i] = m3[0][i] + m3[2][i];
		m1[1][i] = m3[1][i] + m3[3][i];
		m1[2][i] = m3[0][i] - m3[2][i];
		m1[3][i] = m3[1][i] - m3[3][i];
		m1[4][i] = m3[4][i] + m3[6][i];
		m1[5][i] = m3[5][i] + m3[7][i];
		m1[6][i] = m3[4][i] - m3[6][i];
		m1[7][i] = m3[5][i] - m3[7][i];
		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
		m2[4][i] = m1[4][i] + m1[5][i];
		m2[5][i] = m1[4][i] - m1[5][i];
		m2[6][i] = m1[6][i] + m1[7][i];
		m2[7][i] = m1[6][i] - m1[7][i];
	}
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			satd += abs(m2[i][j]);
		}
	}
	satd = (satd + 2) >> 2;
	return satd;
}

static int com_had_4x4(pel *org, pel *cur, U13 s_org, U12 s_cur)
{

#pragma HLS INLINE off
	int k;
	int satd = 0;
	int diff[16];
	int m[16];
	int d[16];
	for (k = 0; k < 16; k += 4) {

#pragma HLS LOOP_TRIPCOUNT max=4
		diff[k + 0] = (int)(org[0] - cur[0]);
		diff[k + 1] = (int)(org[1] - cur[1]);
		diff[k + 2] = (int)(org[2] - cur[2]);
		diff[k + 3] = (int)(org[3] - cur[3]);
		cur += s_cur;
		org += s_org;
	}
	m[0] = diff[0] + diff[12];
	m[1] = diff[1] + diff[13];
	m[2] = diff[2] + diff[14];
	m[3] = diff[3] + diff[15];
	m[4] = diff[4] + diff[8];
	m[5] = diff[5] + diff[9];
	m[6] = diff[6] + diff[10];
	m[7] = diff[7] + diff[11];
	m[8] = diff[4] - diff[8];
	m[9] = diff[5] - diff[9];
	m[10] = diff[6] - diff[10];
	m[11] = diff[7] - diff[11];
	m[12] = diff[0] - diff[12];
	m[13] = diff[1] - diff[13];
	m[14] = diff[2] - diff[14];
	m[15] = diff[3] - diff[15];
	d[0] = m[0] + m[4];
	d[1] = m[1] + m[5];
	d[2] = m[2] + m[6];
	d[3] = m[3] + m[7];
	d[4] = m[8] + m[12];
	d[5] = m[9] + m[13];
	d[6] = m[10] + m[14];
	d[7] = m[11] + m[15];
	d[8] = m[0] - m[4];
	d[9] = m[1] - m[5];
	d[10] = m[2] - m[6];
	d[11] = m[3] - m[7];
	d[12] = m[12] - m[8];
	d[13] = m[13] - m[9];
	d[14] = m[14] - m[10];
	d[15] = m[15] - m[11];
	m[0] = d[0] + d[3];
	m[1] = d[1] + d[2];
	m[2] = d[1] - d[2];
	m[3] = d[0] - d[3];
	m[4] = d[4] + d[7];
	m[5] = d[5] + d[6];
	m[6] = d[5] - d[6];
	m[7] = d[4] - d[7];
	m[8] = d[8] + d[11];
	m[9] = d[9] + d[10];
	m[10] = d[9] - d[10];
	m[11] = d[8] - d[11];
	m[12] = d[12] + d[15];
	m[13] = d[13] + d[14];
	m[14] = d[13] - d[14];
	m[15] = d[12] - d[15];
	d[0] = m[0] + m[1];
	d[1] = m[0] - m[1];
	d[2] = m[2] + m[3];
	d[3] = m[3] - m[2];
	d[4] = m[4] + m[5];
	d[5] = m[4] - m[5];
	d[6] = m[6] + m[7];
	d[7] = m[7] - m[6];
	d[8] = m[8] + m[9];
	d[9] = m[8] - m[9];
	d[10] = m[10] + m[11];
	d[11] = m[11] - m[10];
	d[12] = m[12] + m[13];
	d[13] = m[12] - m[13];
	d[14] = m[14] + m[15];
	d[15] = m[15] - m[14];
	for (k = 0; k < 16; k++) {

#pragma HLS LOOP_TRIPCOUNT max=16
		satd += abs(d[k]);
	}
	satd = (satd + 1) >> 1;
	return satd;
}

static int com_had_2x2(pel *org, pel *cur, U13 s_org, U12 s_cur)
{

#pragma HLS INLINE off
	int satd = 0;
	int diff[4];
	int m[4];
	diff[0] = (int)(org[0] - cur[0]);
	diff[1] = (int)(org[1] - cur[1]);
	diff[2] = (int)(org[s_org] - cur[0 + s_cur]);
	diff[3] = (int)(org[s_org + 1] - cur[1 + s_cur]);
	m[0] = diff[0] + diff[2];
	m[1] = diff[1] + diff[3];
	m[2] = diff[0] - diff[2];
	m[3] = diff[1] - diff[3];
	satd += abs(m[0] + m[1]);
	satd += abs(m[0] - m[1]);
	satd += abs(m[2] + m[3]);
	satd += abs(m[2] - m[3]);
	return satd;
}

static int com_had(U7 w, U7 h, s16 *addr_org, s16 *addr_curr, U13 s_org, U12 s_cur, U4 bit_depth)
{

#pragma HLS INLINE off
	pel *org = (pel *)addr_org;
	pel *cur = (pel *)addr_curr;
	U7 x;
	U7 y;
	int sum = 0;
	U1 step = 1;
	U18 offset_org;
	U17 offset_cur;

	if (w > h && (h & 7) == 0 && (w & 15) == 0) {
		offset_org = s_org << 3;
		offset_cur = s_cur << 3;
		for (y = 0; y < h; y += 8) {
#pragma HLS LOOP_TRIPCOUNT max=4
			for (x = 0; x < w; x += 16) {
#pragma HLS LOOP_TRIPCOUNT max=4
				sum += com_had_16x8(&org[x], &cur[x], s_org, s_cur, bit_depth);
			}
			org += offset_org;
			cur += offset_cur;
		}
	}
	else if (w < h && (w & 7) == 0 && (h & 15) == 0) {
		offset_org = s_org << 4;
		offset_cur = s_cur << 4;
		for (y = 0; y < h; y += 16) {
#pragma HLS LOOP_TRIPCOUNT max=4
			for (x = 0; x < w; x += 8) {
#pragma HLS LOOP_TRIPCOUNT max=4
				sum += com_had_8x16(&org[x], &cur[x], s_org, s_cur, bit_depth);
			}
			org += offset_org;
			cur += offset_cur;
		}
	}
//	else if (w > h && (h & 3) == 0 && (w & 7) == 0) {
//		offset_org = s_org << 2;
//		offset_cur = s_cur << 2;
//		for (y = 0; y < h; y += 4) {
//#pragma HLS LOOP_TRIPCOUNT max=8
//			for (x = 0; x < w; x += 8) {
//#pragma HLS LOOP_TRIPCOUNT max=8
//				sum += com_had_8x4(&org[x], &cur[x], s_org, s_cur);
//			}
//			org += offset_org;
//			cur += offset_cur;
//		}
//	}
//	else if (w < h && (w & 3) == 0 && (h & 7) == 0) {
//		offset_org = s_org << 3;
//		offset_cur = s_cur << 3;
//		for (y = 0; y < h; y += 8) {
//#pragma HLS LOOP_TRIPCOUNT max=8
//			for (x = 0; x < w; x += 4) {
//#pragma HLS LOOP_TRIPCOUNT max=8
//				sum += com_had_4x8(&org[x], &cur[x], s_org, s_cur);
//			}
//			org += offset_org;
//			cur += offset_cur;
//		}
//	}
	else if (w % 8 == 0 && h % 8 == 0) {
		offset_org = s_org << 3;
		offset_cur = s_cur << 3;
		for (y = 0; y < h; y += 8) {
#pragma HLS LOOP_TRIPCOUNT max=8
			for (x = 0; x < w; x += 8) {
#pragma HLS LOOP_TRIPCOUNT max=8
				sum += com_had_8x8(&org[x], &cur[x * step], s_org, s_cur, bit_depth);
			}
			org += offset_org;
			cur += offset_cur;
		}
	}
//	else if (w % 4 == 0 && h % 4 == 0) {
//		offset_org = s_org << 2;
//		offset_cur = s_cur << 2;
//		for (y = 0; y < h; y += 4) {
//
//#pragma HLS LOOP_TRIPCOUNT max=1
//			for (x = 0; x < w; x += 4) {
//
//#pragma HLS LOOP_TRIPCOUNT max=1
//				sum += com_had_4x4(&org[x], &cur[x * step], s_org, s_cur);
//			}
//			org += offset_org;
//			cur += offset_cur;
//		}
//	}
//	else if (w % 2 == 0 && h % 2 == 0) {
//		offset_org = s_org << 1;
//		offset_cur = s_cur << 1;
//		for (y = 0; y < h; y += 2) {
//
//#pragma HLS LOOP_TRIPCOUNT max=2
//			for (x = 0; x < w; x += 2) {
//
//#pragma HLS LOOP_TRIPCOUNT max=2
//				sum += com_had_2x2(&org[x], &cur[x * step], s_org, s_cur);
//			}
//			org += offset_org;
//			cur += offset_cur;
//		}
//	}

#if MD_PRINT
	else {
		{
			if (!0) {
				0 ? ((void)0) : __assert_HLS("0", "../separate/enc_sad.c", (unsigned int)1664, __FUNCTION__);
			}
		}
		;
	}
#endif
	return sum >> (bit_depth - 8);
}

static u32 calc_satd_16b(U7 pu_w, U7 pu_h, s16 *src1, s16 *src2, U13 s_src1, U13 s_src2)
{
	//U2 i, j;

#pragma HLS INLINE off
	u32 cost = 0;
	U2 num_seg_in_pu_w = 1;
	U2 num_seg_in_pu_h = 1;
	S5 seg_w_log2 = com_tbl_log2[pu_w];
	S5 seg_h_log2 = com_tbl_log2[pu_h];
	//s16 *src1_seg;
	//s16 *src2_seg;
	s16 *s1 = (s16 *)src1;
	s16 *s2 = (s16 *)src2;
	if (seg_w_log2 == -1) {
		num_seg_in_pu_w = 3;
		seg_w_log2 = (pu_w == 48 ? 4 : ((pu_w == 24 ? 3 : 2)));
	}
	if (seg_h_log2 == -1) {
		num_seg_in_pu_h = 3;
		seg_h_log2 = (pu_h == 48 ? 4 : ((pu_h == 24 ? 3 : 2)));
	}
	if (num_seg_in_pu_w == 1 && num_seg_in_pu_h == 1) {
		//added by liuky
		cost += ((unsigned int)(com_had(1 << seg_w_log2, 1 << seg_h_log2, s1, s2, s_src1, s_src2, 8)));
		return cost;
	}

	//for (j = 0; j < num_seg_in_pu_h; j++) {
	//	for (i = 0; i < num_seg_in_pu_w; i++) {
	//		src1_seg = s1 + (1 << seg_w_log2) * i + (1 << seg_h_log2) * j * s_src1;
	//		src2_seg = s2 + (1 << seg_w_log2) * i + (1 << seg_h_log2) * j * s_src2;
	//		//added by liuky
	//		cost += ((unsigned int)(com_had(1 << seg_w_log2, 1 << seg_h_log2, src1_seg, src2_seg, s_src1, s_src2, 8)));
	//	}
	//}
	return cost;
}

S21 mc_8tag(const u8 idx, const S15 data[8])
{
#pragma HLS INLINE
#pragma HLS function_instantiate variable=idx

	static const S8 tbl_mc_l_coeff[4][8] = {
		{ 0, 0, 0, 64, 0, 0, 0, 0 },
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 } };
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0

	return tbl_mc_l_coeff[idx][0] * data[0] +
		tbl_mc_l_coeff[idx][1] * data[1] +
		tbl_mc_l_coeff[idx][2] * data[2] +
		tbl_mc_l_coeff[idx][3] * data[3] +
		tbl_mc_l_coeff[idx][4] * data[4] +
		tbl_mc_l_coeff[idx][5] * data[5] +
		tbl_mc_l_coeff[idx][6] * data[6] +
		tbl_mc_l_coeff[idx][7] * data[7];
}

static void enc_bit_est_inter_skip(U4 * bit_cnt, const SKIP_MODE_INTER mode[SKIP_MODE_NUM])
{
#pragma HLS INLINE OFF
#if MD_PRINT
	assert(UMVE_BASE_NUM == 2 && UMVE_REFINE_STEP > 1);
#endif
	U3 ref_step = (mode->umve_idx >= UMVE_MAX_REFINE_NUM ? U5(mode->umve_idx - UMVE_MAX_REFINE_NUM) :U5( mode->umve_idx)) >> 2;
	*bit_cnt = (3 + mode->umve_flag ?
		U4(2 + (ref_step == UMVE_REFINE_STEP - 1 ? (U4)ref_step : (U4)((U4)ref_step + 1))) :
		U4((mode->skip_idx == 0 ? 0 : mode->skip_idx - 2) + (mode->skip_idx == 11 ? 0 : 1)));
}

static S5 encoder(const U1 * input, const U4 width)
{
#pragma HLS INLINE
#pragma HLS function_instantiate variable=width
#if MD_PRINT
	assert(width <= 16);
#endif
	return 	(width > 0 && input[0]) ? 0 : (
		(width > 1 && input[1]) ? 1 : (
		(width > 2 && input[2]) ? 2 : (
		(width > 3 && input[3]) ? 3 : (
		(width > 4 && input[4]) ? 4 : (
		(width > 5 && input[5]) ? 5 : (
		(width > 6 && input[6]) ? 6 : (
		(width > 7 && input[7]) ? 7 : (
		(width > 8 && input[8]) ? 8 : (
		(width > 9 && input[9]) ? 9 : (
		(width > 10 && input[10]) ? 10 : (
		(width > 11 && input[11]) ? 11 : (
		(width > 12 && input[12]) ? 12 : (
		(width > 13 && input[13]) ? 13 : (
		(width > 14 && input[14]) ? 14 : (
		(width > 15 && input[15]) ? 14 : -1)))))))))))))));
}

void shifting(S15 *reg, const S15 in_data, const U4 depth)
{
#pragma HLS INLINE
#pragma HLS function_instantiate variable=depth
#if MD_PRINT
	assert(0 < depth && depth <= 8);
#endif
	if (depth > 0) reg[0] = depth > 1 ? reg[1] : in_data;
	if (depth > 1) reg[1] = depth > 2 ? reg[2] : in_data;
	if (depth > 2) reg[2] = depth > 3 ? reg[3] : in_data;
	if (depth > 3) reg[3] = depth > 4 ? reg[4] : in_data;
	if (depth > 4) reg[4] = depth > 5 ? reg[5] : in_data;
	if (depth > 5) reg[5] = depth > 6 ? reg[6] : in_data;
	if (depth > 6) reg[6] = depth > 7 ? reg[7] : in_data;
	if (depth > 7) reg[7] = in_data;
}

void mc_center(
	SKIP_MODE_INTER mode[SKIP_MODE_NUM],
	S12 mc_pos[SKIP_MC_NUM][MV_D],
	const U12 x_pos,
	const U12 y_pos,
	const U7 cu_width,
	const U7 cu_height,
	const U12 pic_w,
	const U12 pic_h
	) {
#pragma HLS INLINE OFF

	const S14 mask = (-1) ^ 3;
	U1 flag[SKIP_MODE_NUM] = {};
	S14 mv_ctr[SKIP_MC_NUM][MV_D] = {};
	S14 mc_mv[SKIP_MC_NUM][MV_D] = {};
#pragma HLS array_partition variable=mv_ctr complete dim=0
#pragma HLS array_partition variable=flag complete dim=0
#pragma HLS array_partition variable=mc_mv complete dim=0

	mv_ctr[0][MV_X] = mode[1].mvp[MV_X] & mask;
	mv_ctr[0][MV_Y] = mode[1].mvp[MV_Y] & mask;
	mc_mv[0][MV_X] = mv_ctr[0][MV_X] - (SKIP_MC_HOR_PAD << 2);
	mc_mv[0][MV_Y] = mv_ctr[0][MV_Y] - (SKIP_MC_VER_PAD << 2);
#if SKIP_MC_NUM > 1
	for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		flag[i] = mode[i].valid &&
			(COM_ABS16(mode[i].mvp[MV_X] - mv_ctr[0][MV_X]) >(SKIP_MC_HOR_PAD << 2) ||
			COM_ABS16(mode[i].mvp[MV_Y] - mv_ctr[0][MV_Y]) > (SKIP_MC_VER_PAD << 2));
	}
	S5 idx1 = encoder(flag, SKIP_MODE_NUM);
	mv_ctr[1][MV_X] = idx1 >= 0 ? (S14)(mode[idx1].mvp[MV_X] & mask) : (S14)0;
	mv_ctr[1][MV_Y] = idx1 >= 0 ? (S14)(mode[idx1].mvp[MV_Y] & mask) : (S14)0;;
	mc_mv[1][MV_X] = idx1 >= 0 ? (S14)(mv_ctr[1][MV_X] - (SKIP_MC_HOR_PAD << 2)) : (S14)0;;
	mc_mv[1][MV_Y] = idx1 >= 0 ? (S14)(mv_ctr[1][MV_Y] - (SKIP_MC_VER_PAD << 2)) : (S14)0;;
#endif
#if MD_PRINT
	assert(SKIP_MC_NUM <= 2);
#endif

	for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		for (int j = 0; j < SKIP_MC_NUM; ++j){
#pragma HLS UNROLL
			flag[j] = COM_ABS16(mode[i].mvp[MV_X] - mv_ctr[j][MV_X]) <= (SKIP_MC_HOR_PAD << 2) &&
				COM_ABS16(mode[i].mvp[MV_Y] - mv_ctr[j][MV_Y]) <= (SKIP_MC_VER_PAD << 2);
		}
		mode[i].mv_idx = encoder(flag, SKIP_MC_NUM);
		mode[i].mvd[MV_X] = mode[i].mvp[MV_X] - mc_mv[mode[i].mv_idx][MV_X];
		mode[i].mvd[MV_Y] = mode[i].mvp[MV_Y] - mc_mv[mode[i].mv_idx][MV_Y];
		mode[i].x = mode[i].mvd[MV_X] >> 2;
		mode[i].y = mode[i].mvd[MV_Y] >> 2;
		mode[i].dx = mode[i].mvd[MV_X] & 3;
		mode[i].dy = mode[i].mvd[MV_Y] & 3;
	}
	for (int i = 0; i < SKIP_MC_NUM; ++i){
#pragma HLS UNROLL
		mv_in_ref_window(x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_mv[i], mc_pos[i]);
	}
}


static void print_mode_list(s64 cost_list[4 + 8], int *mode_list)
{
#if MD_PRINT
	int  j;
	printf("mode: ");
	for (j = 0; j < 12; ++j){ printf("%d ", mode_list[j]); }
	printf("\ncost: ");
	for (j = 0; j < 12; ++j){ printf("%lld ", mode_list[j] >= 0 ? cost_list[j] : -1); }
	printf("\n");
#endif
}

BOOL check_mv_in_refwin(const int top, const int left, const int x_pos, const int y_pos, const int cu_width, const int cu_height, const int x_chroma, const int y_chroma, const BOOL chroma_motion)
{
	if (!(left >= 0 && left + cu_width + 7 < SWW &&
		top >= 0 && top + cu_height + 7 < SWH))
	{
		return FALSE;
	}

	//if (chroma_motion &&
	//	!(left + x_chroma - x_pos >= 0 &&
	//	top + y_chroma - y_pos >= 0)){
	//	return FALSE;
	//}

	return TRUE;
}

static void check_same_mv(const SKIP_MODE_INTER mode[SKIP_MODE_NUM], U1 valid[SKIP_MODE_NUM], const S64 cost[SKIP_MODE_NUM])
{
	
	for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		U1 same = FALSE;
		for (int j = 0; j < SKIP_MODE_NUM; ++j){
#pragma HLS UNROLL
			same |= mode[j].valid &&
				mode[i].mvp[MV_X] == mode[j].mvp[MV_X] &&
				mode[i].mvp[MV_Y] == mode[j].mvp[MV_Y] &&
				(cost[i] > cost[j] || (j < i && cost[i] == cost[j]));
		}
		valid[i] = !same && mode[i].valid;
	}
}

static void sort_skip_cand(const S64 cost[SKIP_MODE_NUM], const U1 valid[SKIP_MODE_NUM], S7 mode_list[SKIP_RDO_NUM])
{
#pragma HLS UNROLL

	int rank[SKIP_MODE_NUM] = {};
#pragma HLS array_partition variable=rank complete dim=0

	for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		rank[i] = valid[i] ? 0 : -1;
		for (int j = 0; j < SKIP_MODE_NUM; ++j){
#pragma HLS UNROLL
			rank[i] += (valid[i] && valid[j] && (cost[i] > cost[j] || (i > j && cost[i] == cost[j]))) ? 1 : 0;
		}
	}

	for (int i = 0; i < SKIP_RDO_NUM; ++i){
#pragma HLS UNROLL
		U1 flag[SKIP_MODE_NUM] = {};
#pragma HLS array_partition variable=rank complete dim=0

		for (int j = 0; j < SKIP_MODE_NUM; ++j){
#pragma HLS UNROLL
			flag[j] = rank[j] == i;
		}
		mode_list[i] = encoder(flag, SKIP_MODE_NUM);
	}
}

void make_skip_cand(S7 mode_list[SKIP_RDO_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const S32 mode_sad[SKIP_MODE_NUM])
{
#pragma HLS INLINE OFF

	S64 cost[SKIP_MODE_NUM] = {};
	U1 valid[SKIP_MODE_NUM] = {};
#pragma HLS array_partition variable=cost complete dim=0
#pragma HLS array_partition variable=valid complete dim=0

	for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		U4 bit_cnt;
		enc_bit_est_inter_skip(&bit_cnt, mode + i);
		cost[i] = mode_sad[i] * 1000 + bit_cnt;
	}
	check_same_mv(mode, valid, cost);
	sort_skip_cand(cost, valid, mode_list);
}

void output_skip_mode(SKIP_RDO * skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], SKIP_ELEMENT * mode, S7 mode_list[MAX_INTER_SKIP_RDO])
{
#pragma HLS   ARRAY_PARTITION variable=mode_list complete dim=0
	for (int i = 0; i < 5; i++) {
#pragma HLS LOOP_TRIPCOUNT max=5
#pragma HLS	PIPELINE
#pragma HLS UNROLL
		if (mode_list[i] < 0 || i == 4){
			skip_rdo->num_rdo = i;
			break;
		}
		skip_element_copy(skip_rdo_mode + i, mode + mode_list[i]);
	}
}

void print_skip_mode(MD_COM_MODE* mod_info_curr, SKIP_RDO * skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO * merge_rdo)
{
#if MD_PRINT
	static int cnt = 0; ++cnt;
	//if (cnt >= 781621){
		printf("%d ", cnt);
		printf("pos=(%d %d) size=(%d %d): ", mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height);
		int i, j;
		for (i = 0; i < skip_rdo->num_rdo; ++i){
			//printf("(%s %d %d %d)", skip_rdo->mode[i].affine_flag ? "affine" : skip_rdo->mode[i].umve_flag ? "umve" : "skip", skip_rdo->mode[i].umve_flag ? skip_rdo->mode[i].umve_idx : skip_rdo->mode[i].skip_idx, skip_rdo->mode[i].mvp[MV_X], skip_rdo->mode[i].mvp[MV_Y]);
			printf("(%s %d %d %d)", skip_rdo_mode[i].affine_flag ? "affine" : skip_rdo_mode[i].umve_flag ? "umve" : "skip", skip_rdo_mode[i].umve_flag ? skip_rdo_mode[i].umve_idx : skip_rdo_mode[i].skip_idx, skip_rdo_mode[i].mvp[MV_X], skip_rdo_mode[i].mvp[MV_Y]);
		}
		for (i = 0; i < merge_rdo->num_cands; ++i){
			if (!merge_rdo->valid[i]){ continue; }
			printf("(%s ", "affine");
			for (j = 0; j < merge_rdo->mrg_list_cp_num[i]; ++j){
				printf("(%d %d) ", merge_rdo->mrg_list_cp_mv[i][REFP_0][j][MV_X], merge_rdo->mrg_list_cp_mv[i][REFP_0][j][MV_Y]);
			}
			printf(")");
		}
		printf("\n");
	//}
#endif
}

static void get_range_ipel_sw_fetch(ENC_SW *sw, s16 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], int w, int h)
{
	int max_search_range = 128;
	int ag_size = 0;
	int offset_x;
	int offset_y;
	int rangexy;
	int min_x = (int)CtrPos[0][0][0];
	int min_y = (int)CtrPos[0][1][0];
	int max_x = ((int)CtrPos[1][0][0]) - w;
	int max_y = ((int)CtrPos[1][1][0]) - h;
	rangexy = max_search_range;
#if MD_PRINT
	rangexy <= max_search_range ? ((void)0) : __assert_HLS("rangexy <= max_search_range", "../separate/enc_pinter.c",
		(unsigned int)3441, __FUNCTION__);
#endif
	offset_x = rangexy;
	offset_y = rangexy;
	sw->range[2][0][0] = ((s16)((min_x > ((max_x < sw->center[2][0] - offset_x ? max_x : (
		sw->center[2][0]) - offset_x)) ? min_x : ((max_x < sw->center[2][0] -
		offset_x ? max_x : sw->center[2][0] - offset_x)))));
	sw->range[2][1][0] = ((s16)((min_x >((max_x < sw->center[2][0] + offset_x ? max_x : (
		sw->center[2][0]) + offset_x)) ? min_x : ((max_x < sw->center[2][0] + (
		offset_x) ? max_x : sw->center[2][0] + offset_x)))));
	sw->range[2][0][1] = ((s16)((min_y >((max_y < sw->center[2][1] - offset_y ? max_y : (
		sw->center[2][1]) - offset_y)) ? min_y : ((max_y < sw->center[2][1] - (
		offset_y) ? max_y : sw->center[2][1] - offset_y)))));
	sw->range[2][1][1] = ((s16)((min_y >((max_y < sw->center[2][1] + offset_y ? max_y : (
		sw->center[2][1]) + offset_y)) ? min_y : ((max_y < sw->center[2][1] + (
		offset_y) ? max_y : sw->center[2][1] + offset_y)))));
	rangexy = 32;
	offset_x = rangexy;
	offset_y = rangexy;
	sw->range[1][0][0] = ((s16)((min_x >((max_x < sw->center[2][0] - offset_x ? max_x : ((
		int)sw->center[2][0]) - offset_x)) ? min_x : ((max_x < sw->center[2][0] - ((int)((
		s16)offset_x)) ? max_x : sw->center[2][0] - offset_x)))));
	sw->range[1][1][0] = ((s16)((min_x >((max_x < sw->center[2][0] + offset_x ? max_x : ((
		int)sw->center[2][0]) + offset_x)) ? min_x : ((max_x < sw->center[2][0] + ((int)((
		s16)offset_x)) ? max_x : sw->center[2][0] + offset_x)))));
	sw->range[1][0][1] = ((s16)((min_y >((max_y < sw->center[2][1] - offset_y ? max_y : ((
		int)sw->center[2][1]) - offset_y)) ? min_y : ((max_y < sw->center[2][1] - ((int)((
		s16)offset_y)) ? max_y : sw->center[2][1] - offset_y)))));
	sw->range[1][1][1] = ((s16)((min_y >((max_y < sw->center[2][1] + offset_y ? max_y : ((
		int)sw->center[2][1]) + offset_y)) ? min_y : ((max_y < sw->center[2][1] + ((int)((
		s16)offset_y)) ? max_y : sw->center[2][1] + offset_y)))));
	rangexy = 8;
	offset_x = rangexy;
	offset_y = rangexy;
	sw->range[0][0][0] = ((s16)((min_x >((max_x < sw->center[2][0] - offset_x ? max_x : ((
		int)sw->center[2][0]) - offset_x)) ? min_x : ((max_x < sw->center[2][0] - ((int)((
		s16)offset_x)) ? max_x : sw->center[2][0] - offset_x)))));
	sw->range[0][1][0] = ((s16)((min_x >((max_x < sw->center[2][0] + offset_x ? max_x : ((
		int)sw->center[2][0]) + offset_x)) ? min_x : ((max_x < sw->center[2][0] + ((int)((
		s16)offset_x)) ? max_x : sw->center[2][0] + offset_x)))));
	sw->range[0][0][1] = ((s16)((min_y >((max_y < sw->center[2][1] - offset_y ? max_y : ((
		int)sw->center[2][1]) - offset_y)) ? min_y : ((max_y < sw->center[2][1] - ((int)((
		s16)offset_y)) ? max_y : sw->center[2][1] - offset_y)))));
	sw->range[0][1][1] = ((s16)((min_y >((max_y < sw->center[2][1] + offset_y ? max_y : ((
		int)sw->center[2][1]) + offset_y)) ? min_y : ((max_y < sw->center[2][1] + ((int)((
		s16)offset_y)) ? max_y : sw->center[2][1] + offset_y)))));
	sw->search_lenth[2][0] = ((s16)(((int)sw->range[2][1][0]) - ((int)sw->range[2][0][0])));
	sw->search_lenth[2][1] = ((s16)(((int)sw->range[2][1][1]) - ((int)sw->range[2][0][1])));
	sw->search_lenth[1][0] = ((s16)(((int)sw->range[1][1][0]) - ((int)sw->range[1][0][0])));
	sw->search_lenth[1][1] = ((s16)(((int)sw->range[1][1][1]) - ((int)sw->range[1][0][1])));
	sw->search_lenth[0][0] = ((s16)(((int)sw->range[0][1][0]) - ((int)sw->range[0][0][0])));
	sw->search_lenth[0][1] = ((s16)(((int)sw->range[0][1][1]) - ((int)sw->range[0][0][1])));
}

static void fetch_plane_lv2_1_fetch(ENC_SW *sw, s16 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS])
{
	int i = 0;
	int j = 0;
	int k = 0;
	int stride_buf_lv0 = sw->buffer_stride[0];
	int stride_buf_lv1 = sw->buffer_stride[1];
	int stride_buf_lv2 = sw->buffer_stride[2];
	int refp_offset = 0;
	int lv1_start_y = 0;
	int lv1_start_x = 0;
	int lv0_start_y = 0;
	int lv0_start_x = 0;
	int start_x_l = 0;
	int start_y_l = 0;
	//int lenth_hor = Fetch_Refwin_ptr->CtrPos[RANGE_MAX][DIR_HOR][PIC_DIM] - Fetch_Refwin_ptr->CtrPos[RANGE_MIN][DIR_HOR][PIC_DIM];
	//int lenth_ver = Fetch_Refwin_ptr->CtrPos[RANGE_MAX][DIR_VER][PIC_DIM] - Fetch_Refwin_ptr->CtrPos[RANGE_MIN][DIR_VER][PIC_DIM];
	i = 0;
	lv1_start_y = ((int)sw->range[1][0][1]) - ((int)sw->range[2][0][1]);
	lv1_start_x = ((int)sw->range[1][0][0]) - ((int)sw->range[2][0][0]);
	lv0_start_y = ((int)sw->range[0][0][1]) - ((int)sw->range[2][0][1]);
	lv0_start_x = ((int)sw->range[0][0][0]) - ((int)sw->range[2][0][0]);
	//sw->pic_y_lv0_ptr = Fetch_Refwin_ptr->RefWindowLCU;
	sw->ctr_point[0][0][0] = ((s16)(((int)sw->range[0][0][0]) - ((int)CtrPos[0][0][0]) + ((
		int)CtrPos[0][0][1])));
	sw->ctr_point[0][0][1] = ((s16)(((int)sw->range[0][0][1]) - ((int)CtrPos[0][1][0]) + ((
		int)CtrPos[0][1][1])));
	sw->ctr_point[0][1][0] = ((s16)(((int)sw->ctr_point[0][0][0]) + ((int)sw->search_lenth[0][0])));
	sw->ctr_point[0][1][1] = ((s16)(((int)sw->ctr_point[0][0][1]) + ((int)sw->search_lenth[0][1])));
	//sw->pic_y_lv1_ptr = Fetch_Refwin_ptr->RefWindowLCU_LV1;
	sw->ctr_point[1][0][0] = ((s16)(((int)sw->range[1][0][0] - (int)CtrPos[0][0][0]) >> 1));
	sw->ctr_point[1][0][1] = ((s16)(((int)sw->range[1][0][1] - (int)CtrPos[0][1][0]) >> 1));
	sw->ctr_point[1][1][0] = ((s16)((((int)sw->ctr_point[1][0][0] + ((int)sw->search_lenth[1][0])) >> 1)));
	sw->ctr_point[1][1][1] = ((s16)((((int)sw->ctr_point[1][0][1] + ((int)sw->search_lenth[1][1])) >> 1)));
	//sw->pic_y_lv2_ptr = Fetch_Refwin_ptr->RefWindowLCU_LV2;
	sw->ctr_point[2][0][0] = ((s16)(((int)sw->range[2][0][0] - (int)CtrPos[0][0][0]) >> 2));
	sw->ctr_point[2][0][1] = ((s16)(((int)sw->range[2][0][1] - (int)CtrPos[0][1][0]) >> 2));
	sw->ctr_point[2][1][0] = (s16)(((int)sw->ctr_point[2][0][0] + (int)sw->search_lenth[2][0]) >> 2);
	sw->ctr_point[2][1][1] = (s16)(((int)sw->ctr_point[2][0][1] + (int)sw->search_lenth[2][1]) >> 2);
}

static void construct_search_window_fetch(ENC_SW *sw, s16 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
	int x, int y, s8 ref_idx, int lidx, int w, int h)
{

#pragma HLS INLINE off
	sw->fetch_from_pic_y = 1;
	sw->pool_type = 0;
	sw->hor_range = ((s16)72);
	sw->ver_range = ((s16)72);
	sw->ref_idx = ref_idx;
	sw->lidx = lidx;
	sw->center[0][0] = ((s16)x);
	sw->center[0][1] = ((s16)y);
	sw->center[1][0] = ((s16)x);
	sw->center[1][1] = ((s16)y);
	sw->center[2][0] = ((s16)x);
	sw->center[2][1] = ((s16)y);
	sw->buffer_stride[0] = SWW;
	sw->buffer_stride[1] = SWW >> 1;
	sw->buffer_stride[2] = SWW >> 2;
	sw->consist_lv0 = 1;
	get_range_ipel_sw_fetch(sw, CtrPos, w, h);
	fetch_plane_lv2_1_fetch(sw, CtrPos);
}

static void get_mvp_default_line(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_ptr, MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2], ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN])
{
#pragma HLS inline off
	U23 scup = mod_info_curr->scup;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U13 pic_w = md_fw_ptr->pic_width;
	U13 pic_h = md_fw_ptr->pic_height;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 h_scu = md_fw_ptr->pic_height_in_scu;
	//s16(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
	//s8(*map_refi)[REFP_NUM] = pic_map->map_refi;
	//u32* map_scu = pic_map->map_scu;
	int x_scu_left = 0;
	int cnt;
	int hv;
	int ptr_cur_ref;
	int mvPredType = 3;
	s8 rFrameL;
	s8 rFrameU;
	s8 rFrameUR;
	int neb_addr[3 + 1];
	int valid_flag[3 + 1];
	s32 refi[3];
	s32 MVPs[3][2];
	int x_scu = mod_info_curr->x_scu;
	int y_scu = mod_info_curr->y_scu;
	int cu_width_in_scu = cu_width >> 2;
	int offset_in_line = 0;
	//check_mvp_motion_availability(info, mod_info_curr, pic_map, neb_addr, valid_flag, lidx);
	int i = 0;
	int use_line = 0;
	int debug = 0;
	s16 x_scu_line[5 - 1];
	s16 y_scu_line[5 - 1];
	int d_inner = 1;
	S2 refi_nebs[NEB_NUMS];
	//u32 scu_nebs[NEB_NUMS];
	s32 mv_nebs[NEB_NUMS][MV_D];

	// A
	neb_addr[0] = scup - 1;
	// B
	neb_addr[1] = scup - pic_width_in_scu;
	// C
	neb_addr[2] = scup - pic_width_in_scu + cu_width_in_scu;
	// D
	neb_addr[3] = scup - pic_width_in_scu - 1;
	valid_flag[0] = x_scu > 0;
	valid_flag[1] = y_scu > 0;
	valid_flag[2] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu;
	valid_flag[3] = x_scu > 0 && y_scu > 0;
	x_scu_line[0] = x_scu - me_line_ptr->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_ptr->min_scu_y;
	x_scu_line[1] = x_scu - me_line_ptr->min_scu_x;
	y_scu_line[1] = y_scu - me_line_ptr->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_ptr->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_ptr->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_ptr->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_ptr->min_scu_y - 1;
	if (x_scu == me_line_ptr->min_scu_x) {
		offset_in_line = y_scu - me_line_ptr->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].refi[lidx];
			valid_flag[0] = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]] >> 15 & ((
					unsigned int)1)))
				&& ((int)inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].refi[lidx]) >= 0;
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->left_nebs[neb_addr[0]].refi[lidx]);
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = inner_nebs[1 -
				me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].mv[lidx][0];
			mv_nebs[0][1] = inner_nebs[1 -
				me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_ptr->min_scu_y) {
			offset_in_line = y_scu - me_line_ptr->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].refi[lidx];
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]] >> 31 & ((
						unsigned int)1)))
					&& !((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]] >> 15 & ((
						unsigned int)1)))
					&& ((int)inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].refi[lidx]) >= 0;
			}
			if (valid_flag[3]) {
				mv_nebs[3][0] = inner_nebs[1 -
					me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].mv[lidx][0];
				mv_nebs[3][1] = inner_nebs[1 -
					me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].mv[lidx][1];
			}
			d_inner = 0;
		}
	}
	else {
		if (valid_flag[0]) {
			refi_nebs[0] = inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]].refi[lidx];
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(map_scu[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[0]);
			valid_flag[0] = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]].mv[lidx][0];
			mv_nebs[0][1] =
				inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]].mv[lidx][1];
		}
	}
	if (y_scu == me_line_ptr->min_scu_y) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
//		for (i = 1; i < 3 + d_inner; i++) {
		for (i = 1; i < 4; i++) {
#pragma HLS loop_tripcount max = 4
#pragma HLS UNROLL
			if(i < 3 + d_inner)
			{
			if (valid_flag[i]) {
				refi_nebs[i] = above_nebs_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET].refi[lidx];
				valid_flag[i] = valid_flag[i]
					&& ((int)(me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET] >> 31 & ((unsigned int)1)))
					&& !((int)(me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET] >> 15 & ((unsigned int)1)))
					&& ((int)refi_nebs[i]) >= 0;
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] = above_nebs_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][0];
				mv_nebs[i][1] = above_nebs_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][1];
			}
			}
		}
	}
	else {
//		for (i = 1; i < 3 + d_inner; i++) {
		for (i = 1; i < 4; i++) {
#pragma HLS loop_tripcount max = 4
#pragma HLS UNROLL
			if(i < 3 + d_inner){
			if (valid_flag[i]) {
				if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
					valid_flag[i] = 0;
				}
				if (valid_flag[i])
				{
					refi_nebs[i] = inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]].refi[lidx];
					//valid_flag[i] = valid_flag[i] && MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[i]);
					valid_flag[i] = valid_flag[i]
						&& ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]] >> 31 & ((
							unsigned int)1)))
						&& !((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]] >> 15 & ((
							unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
				}
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] =
					inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]].mv[lidx][0];
				mv_nebs[i][1] =
					inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]].mv[lidx][1];
			}
			/*if (valid_flag[i]){
			if(me_line_ptr->mv_nebs[i][MV_X] != map_mv[neb_addr[i]][lidx][MV_X] || me_line_ptr->mv_nebs[i][MV_Y] != map_mv[neb_addr[i]][lidx][MV_Y])
			printf("Wrong");
			}*/
		}
		}
	}
	if (!valid_flag[2]) {
		// D
		neb_addr[2] = neb_addr[3];
		valid_flag[2] = valid_flag[3];
		refi_nebs[2] = refi_nebs[3];
		mv_nebs[2][0] = mv_nebs[3][0];
		mv_nebs[2][1] = mv_nebs[3][1];
	}
	ptr_cur_ref = refp_ptr[cur_refi][lidx];
	for (cnt = 0; cnt < 3; cnt++) {
#pragma HLS UNROLL
		if (valid_flag[cnt]) {
			refi[cnt] = refi_nebs[cnt];
#if MD_PRINT
			((int)refi[cnt]) >= 0 ? ((void)0) : __assert_HLS("((refi[cnt]) >= 0)", "../separate/com_util.c", (unsigned int)723,
				__FUNCTION__);
#endif
			int ptr_neb_ref = refp_ptr[refi[cnt]][lidx];
			//scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, map_mv[neb_addr[cnt]][lidx], MVPs[cnt]);
			scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, mv_nebs[cnt], MVPs[cnt]);
		}
		else {
			refi[cnt] = -1;
			MVPs[cnt][0] = 0;
			MVPs[cnt][1] = 0;
		}
	}
	rFrameL = refi[0];
	rFrameU = refi[1];
	rFrameUR = refi[2];
	if (rFrameL != -1 && rFrameU == -1 && rFrameUR == -1) {
		mvPredType = 0;
	}
	else if (rFrameL == -1 && rFrameU != -1 && rFrameUR == -1) {
		mvPredType = 1;
	}
	else if (rFrameL == -1 && rFrameU == -1 && rFrameUR != -1) {
		mvPredType = 2;
	}
	for (hv = 0; hv < 2; hv++) {
#pragma HLS UNROLL
		s16 mva = MVPs[0][hv];
		s16 mvb = MVPs[1][hv];
		s16 mvc = MVPs[2][hv];
		switch (mvPredType) {
		case 3:
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
				mvp[hv] = (mvb + mvc) / 2;
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
				mvp[hv] = (mvc + mva) / 2;
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
				mvp[hv] = (mva + mvb) / 2;
			}
			else {
				s16 mva_ext = abs(mva - mvb);
				s16 mvb_ext = abs(mvb - mvc);
				s16 mvc_ext = abs(mvc - mva);
				s16 pred_vec = mva_ext < ((mvb_ext < mvc_ext ? mvb_ext : mvc_ext)) ? mva_ext : ((mvb_ext < mvc_ext ? mvb_ext :
					mvc_ext));
				if (pred_vec == mva_ext) {
					mvp[hv] = (mva + mvb) / 2;
				}
				else if (pred_vec == mvb_ext) {
					mvp[hv] = (mvb + mvc) / 2;
				}
				else {
					mvp[hv] = (mvc + mva) / 2;
				}
			}
			break;
		case 0:
			mvp[hv] = mva;
			break;
		case 1:
			mvp[hv] = mvb;
			break;
		case 2:
			mvp[hv] = mvc;
			break;
		default:
#if MD_PRINT
			0 ? ((void)0) : __assert_HLS("0", "../separate/com_util.c", (unsigned int)800, __FUNCTION__);
#endif
			break;
		}
	}
	// clip MVP after rounding (rounding process might result in 32768)
	CPMV mvp_x;
	CPMV mvp_y;
	com_mv_rounding_s32((s32)mvp[0], (s32)mvp[1], &mvp_x, &mvp_y, (int)amvr_idx, (int)amvr_idx);
	mvp[0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
	mvp[1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
}

static void com_get_mvp_hmvp(COM_MOTION motion, int lidx, int ptr_cur, s8 cur_refi, s32 mvp[2], int refp_ptr[17][2],
	int amvr_idx)
{
	int ptr_hmvp_ref;
	s8 refi_hmvp = motion.ref_idx[lidx];
	int ptr_cur_ref = refp_ptr[cur_refi][lidx];
	if (((int)refi_hmvp) >= 0) {
		ptr_hmvp_ref = refp_ptr[refi_hmvp][lidx];
		scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_hmvp_ref, motion.mv[lidx], mvp);
	}
	else {
		int lidx1 = (int)(lidx == ((int)PRED_L0) ? PRED_L1 : PRED_L0);
		refi_hmvp = motion.ref_idx[lidx1];
		ptr_hmvp_ref = refp_ptr[refi_hmvp][lidx1];
		scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_hmvp_ref, motion.mv[lidx1], mvp);
	}
	// clip MVP after rounding (rounding process might result in 32768)
	int mvp_x;
	int mvp_y;
	com_mv_rounding_s32((s32)mvp[0], (s32)mvp[1], &mvp_x, &mvp_y, amvr_idx, amvr_idx);
	mvp[0] = ((s16)((((int)((s16)0x8000)) > ((((int)((s16)0x7FFF)) < mvp_x ? ((int)((s16)0x7FFF)) : mvp_x)) ? ((
		int)((s16)0x8000)) : ((((int)((s16)0x7FFF)) < mvp_x ? ((int)((s16)0x7FFF)) : mvp_x)))));
	mvp[1] = ((s16)((((int)((s16)0x8000)) > ((((int)((s16)0x7FFF)) < mvp_y ? ((int)((s16)0x7FFF)) : mvp_y)) ? ((
		int)((s16)0x8000)) : ((((int)((s16)0x7FFF)) < mvp_y ? ((int)((s16)0x7FFF)) : mvp_y)))));
}

void derive_mvp_line_all(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2])
{
#pragma HLS inline off
#pragma HLS ARRAY_PARTITION variable=mvp complete dim=0
	U23 scup = mod_info_curr->scup;
	U8 emvp_flag = mod_info_curr->mvp_from_hmvp_flag;
	u8 mvr_idx = 0;
	CPMV mvp_x = 0;
	CPMV mvp_y = 0;
	if (!emvp_flag) {
		get_mvp_default_line(md_fw_ptr, me_line_ptr, mod_info_curr, refp_ptr, ptr, ref_list, (s8)ref_idx, mvr_idx, mvp[mvr_idx], inner_nebs, above_nebs_line);
		for (mvr_idx = 1; mvr_idx < 4; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				com_mv_rounding_s32(mvp[0][0], mvp[0][1], &mvp_x, &mvp_y, mvr_idx, mvr_idx);

				mvp[mvr_idx][0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
				mvp[mvr_idx][1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
			}
		}
	}
	else {
		for (mvr_idx = 0; mvr_idx < 5; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				mvp[mvr_idx][0] = 0;
				mvp[mvr_idx][1] = 0;
			}
		}
	}
}
static void com_mv_rounding_s16(s16 hor, s16 ver, s16 *rounded_hor, s16 *rounded_ver, int right_shift, int left_shift)
{
	int add = right_shift > 0 ? 1 << (right_shift - 1) : 0;
	*rounded_hor = ((hor >= 0 ? (hor + add) >> right_shift << left_shift : -((-hor + add) >> right_shift <<
		left_shift)));
	*rounded_ver = ((ver >= 0 ? (ver + add) >> right_shift << left_shift : -((-ver + add) >> right_shift <<
		left_shift)));
}

static void com_mv_rounding_s16_down(s16 hor, s16 ver, s16 *rounded_hor, s16 *rounded_ver, int right_shift, int left_shift)
{
	
	*rounded_hor = ((hor >= 0 ? (hor) >> right_shift << left_shift : -((-hor) >> right_shift <<
		left_shift)));
	*rounded_ver = ((ver >= 0 ? (ver) >> right_shift << left_shift : -((-ver) >> right_shift <<
		left_shift)));
}

static u32 get_exp_golomb_bits(u32 abs_mvd)
{
	int bits = 0;
	int len_i;
	int len_c;
	int nn;
	/* abs(mvd) */
	nn = ((int)((abs_mvd + (unsigned int)1) >> 1));
	for (len_i = 0; len_i < 16 && nn != 0; len_i++) {

#pragma HLS LOOP_TRIPCOUNT max=16
		nn >>= 1;
	}
	len_c = (len_i << 1) + 1;
	bits += len_c;
	/* sign */
	if (abs_mvd) {
		bits++;
	}
	return (u32)bits;
}

static u8 get_enc_tbl_mv_bits(int a)
{
	static u8 enc_tbl_mv_bits_data[4096] =
	{
		22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 12, 12,
		12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 8, 8, 8, 8, 8, 8, 8, 8, 6, 6, 6, 6, 4, 4,
		1, 4, 4, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 8, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
		12, 12, 12, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
		14, 14, 14, 14, 14, 14, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
		18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
		20, 20, 20, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
		22, 22, 22, 22, 22, 22, 22, 24, 24,
	};

	u8 *enc_tbl_mv_bits = enc_tbl_mv_bits_data + 2047;

	return enc_tbl_mv_bits[a];
}


static u8 get_enc_tbl_refi_bits(int a, int b)
{
	u8 enc_tbl_refi_bits[17][16] = { { 0 }, { 0 }, { ((u8)1), ((u8)1) }, { ((u8)1), ((u8)2), ((u8)2) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)3) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)4) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)5) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)6) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)7) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)8) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)9) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)10), ((u8)10) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)10), ((u8)11), ((u8)11) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)10), ((u8)11), ((u8)12), ((u8)12) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)10), ((u8)11), ((u8)12), ((u8)13), ((u8)13) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)10), ((u8)11), ((u8)12), ((u8)13), ((u8)14), ((u8)14) }, { ((u8)1), ((u8)2), ((u8)3), ((u8)4), ((u8)5), ((u8)6), ((u8)7), ((u8)8), ((u8)9), ((u8)10), ((u8)11), ((u8)12), ((u8)13), ((u8)14), ((u8)15), ((u8)15) } };
	return enc_tbl_refi_bits[a][b];
}

static int get_mv_bits_with_mvr(int mvd_x, int mvd_y, int num_refp, int refi, u8 mvr_idx)
{

#pragma HLS INLINE off
	int bits = 0;
	int res1;
	int res2;
	res1 = ((int)(get_exp_golomb_bits((u32)(abs(mvd_x) >> ((int)mvr_idx)))));
	res2 = ((int)(get_enc_tbl_mv_bits(mvd_x >> ((int)mvr_idx))));
	bits = (mvd_x >> ((int)mvr_idx) > 2048 || mvd_x >> ((int)mvr_idx) <= -2048 ? res1 : res2);
	res1 = ((int)(get_exp_golomb_bits((u32)(abs(mvd_y) >> ((int)mvr_idx)))));
	res2 = ((int)(get_enc_tbl_mv_bits(mvd_y >> ((int)mvr_idx))));
	bits += (mvd_y >> ((int)mvr_idx) > 2048 || mvd_y >> ((int)mvr_idx) <= -2048 ? res1 : res2);
	bits += ((int)(get_enc_tbl_refi_bits(num_refp, refi)));
	if (((int)mvr_idx) == 5 - 1) {
		bits += ((int)mvr_idx);
	}
	else {
		bits += ((int)mvr_idx) + 1;
	}
	return bits;
}

static u32 calc_bit_cost(u32 lambda_mv, int mv_bits)
{
	return (u32)((lambda_mv * ((unsigned int)mv_bits) + ((unsigned int)(1 << 15))) >> 16);
}
//extern ENC_ME_CU *me_cu_info_global;

static void init_temp_mvinfo(s16 mvc[2], s16 gmvp[2], s16 mvt[2])
{
	mvc[0] = 0;
	mvc[1] = 0;
	gmvp[0] = 0;
	gmvp[1] = 0;
	mvt[0] = 0;
	mvt[1] = 0;
}

static void qpel_search_window_luma_25p_col(pel *ref, pel *pred, int s_ref, int s_pred, int w, int h,
	s16 ctrpos[2][2][2], s16 cx, s16 cy)
{

#pragma HLS INLINE off
	s16 tbl_mc_l_coeff[4][8] = { { 0, 0, 0, ((s16)64), 0, 0, 0, 0 }, { ((s16)(-1)), ((s16)4), ((s16)(-10)), ((s16)57), ((s16)19), ((s16)(-7)), ((s16)3), ((s16)(-1)) }, { ((s16)(-1)), ((s16)4), ((s16)(-11)), ((s16)40), ((s16)40), ((s16)(-11)), ((s16)4), ((s16)(-1)) }, { ((s16)(-1)), ((s16)3), ((s16)(-7)), ((s16)19), ((s16)57), ((s16)(-10)), ((s16)4), ((s16)(-1)) } };
	int row;
	int col;
	int dx;
	int dy;
	int ref_offset;
	int pred_offset;
	int hor_buf_offset;
	int x_offset_min;
	int x_offset_max;
	int y_offset_min;
	int y_offset_max;
	pel pt;
	pel hor_buf[64 + 8] = { ((pel)0) };
	for (col = -2; col <= ((w - 1) << 2) + 2; col++) {

#pragma HLS LOOP_TRIPCOUNT max=33
		// Horizontal interp for intermediate values, (h + 8) rows.
		dx = (col + 4) & 0x3;
		if (dx == 0) {
			x_offset_min = 0;
			x_offset_max = 0;
		}
		else {
			x_offset_min = -3;
			x_offset_max = 4;
		}
		if (x_offset_min + ((((int)cx) + dx) >> 2) >= ((int)ctrpos[0][0][0])
			&& x_offset_max + ((((int)cx) + dx) >> 2) <= ((int)ctrpos[1][0][0])) {
			// Int scale.
			for (row = -4; row < h + 4; row++) {

#pragma HLS LOOP_TRIPCOUNT max=16
				// The col position decides the interp way.
				if (dx != 0) {
					// Hor interp
					ref_offset = (col < 0 ? -4 : (col >> 2) - 3);
					ref_offset += row * s_ref;
					hor_buf[row + 4] = ((pel)(((int)tbl_mc_l_coeff[dx][0]) * ((int)ref[ref_offset + 0 * 1]) + ((
						int)tbl_mc_l_coeff[dx][1]) * ((int)ref[ref_offset + 1 * 1]) + ((int)tbl_mc_l_coeff[dx][2]) * ((
						int)ref[ref_offset + 2 * 1]) + ((int)tbl_mc_l_coeff[dx][3]) * ((int)ref[ref_offset + 3 * 1]) + ((
						int)tbl_mc_l_coeff[dx][4]) * ((int)ref[ref_offset + 4 * 1]) + ((int)tbl_mc_l_coeff[dx][5]) * ((
						int)ref[ref_offset + 5 * 1]) + ((int)tbl_mc_l_coeff[dx][6]) * ((int)ref[ref_offset + 6 * 1]) + ((
						int)tbl_mc_l_coeff[dx][7]) * ((int)ref[ref_offset + 7 * 1])));
				}
			}
			// Vertical interp.
			// q-pel scale.
			for (row = -2; row <= ((h - 1) << 2) + 2; row++) {

#pragma HLS LOOP_TRIPCOUNT max=33
				dy = (row + 4) & 0x3;
				if (dy == 0) {
					y_offset_min = 0;
					y_offset_max = 0;
				}
				else {
					y_offset_min = -3;
					y_offset_max = 4;
				}
				if (y_offset_min + ((((int)cy) + dy) >> 2) >= ((int)ctrpos[0][1][0])
					&& y_offset_max + ((((int)cy) + dy) >> 2) <= ((int)ctrpos[1][1][0])) {
					pred_offset = col + 2;
					pred_offset += s_pred * (row + 2);
					if (dx == 0 && dy == 0) {
						// Copy int position
						ref_offset = (row >> 2) * s_ref + (col >> 2);
						pred[pred_offset] = ref[ref_offset];
					}
					else if (dx != 0 && dy == 0) {
						// Hor interp
						ref_offset = (row >> 2) * s_ref;
						ref_offset += (col < 0 ? -4 : (col >> 2) - 3);
						pt = ((pel)((((int)tbl_mc_l_coeff[dx][0]) * ((int)ref[ref_offset + 0 * 1]) + ((int)tbl_mc_l_coeff[dx][1]) * ((
							int)ref[ref_offset + 1 * 1]) + ((int)tbl_mc_l_coeff[dx][2]) * ((int)ref[ref_offset + 2 * 1]) + ((
							int)tbl_mc_l_coeff[dx][3]) * ((int)ref[ref_offset + 3 * 1]) + ((int)tbl_mc_l_coeff[dx][4]) * ((
							int)ref[ref_offset + 4 * 1]) + ((int)tbl_mc_l_coeff[dx][5]) * ((int)ref[ref_offset + 5 * 1]) + ((
							int)tbl_mc_l_coeff[dx][6]) * ((int)ref[ref_offset + 6 * 1]) + ((int)tbl_mc_l_coeff[dx][7]) * ((
							int)ref[ref_offset + 7 * 1]) + (1 << 5)) >> 6));
						pred[pred_offset] = ((pel)((0 >(((1 << 8) - 1 < ((int)pt) ? (1 << 8) - 1 : ((int)pt))) ? 0 : (((1 << 8) - 1 < ((
							int)pt) ? (1 << 8) - 1 : ((int)pt))))));
					}
					else if (dx == 0 && dy != 0) {
						// Ver interp
						ref_offset = (row < 0 ? -4 * s_ref : ((row >> 2) - 3) * s_ref);
						ref_offset += col >> 2;
						pt = ((pel)((((int)tbl_mc_l_coeff[dy][0]) * ((int)ref[ref_offset + 0 * s_ref]) + ((int)tbl_mc_l_coeff[dy][1]) * ((
							int)ref[ref_offset + 1 * s_ref]) + ((int)tbl_mc_l_coeff[dy][2]) * ((int)ref[ref_offset + 2 * s_ref]) + ((
							int)tbl_mc_l_coeff[dy][3]) * ((int)ref[ref_offset + 3 * s_ref]) + ((int)tbl_mc_l_coeff[dy][4]) * ((
							int)ref[ref_offset + 4 * s_ref]) + ((int)tbl_mc_l_coeff[dy][5]) * ((int)ref[ref_offset + 5 * s_ref]) + ((
							int)tbl_mc_l_coeff[dy][6]) * ((int)ref[ref_offset + 6 * s_ref]) + ((int)tbl_mc_l_coeff[dy][7]) * ((
							int)ref[ref_offset + 7 * s_ref]) + (1 << 5)) >> 6));
						pred[pred_offset] = ((pel)((0 >(((1 << 8) - 1 < ((int)pt) ? (1 << 8) - 1 : ((int)pt))) ? 0 : (((1 << 8) - 1 < ((
							int)pt) ? (1 << 8) - 1 : ((int)pt))))));
					}
					else {
						// Ver interp
						hor_buf_offset = (row < 0 ? 0 : (row >> 2) + 1);
						pt = ((pel)((((int)tbl_mc_l_coeff[dy][0]) * ((int)hor_buf[hor_buf_offset + 0 * 1]) + ((
							int)tbl_mc_l_coeff[dy][1]) * ((int)hor_buf[hor_buf_offset + 1 * 1]) + ((int)tbl_mc_l_coeff[dy][2]) * ((
							int)hor_buf[hor_buf_offset + 2 * 1]) + ((int)tbl_mc_l_coeff[dy][3]) * ((int)hor_buf[hor_buf_offset + 3 * 1]) + ((
							int)tbl_mc_l_coeff[dy][4]) * ((int)hor_buf[hor_buf_offset + 4 * 1]) + ((int)tbl_mc_l_coeff[dy][5]) * ((
							int)hor_buf[hor_buf_offset + 5 * 1]) + ((int)tbl_mc_l_coeff[dy][6]) * ((int)hor_buf[hor_buf_offset + 6 * 1]) + ((
							int)tbl_mc_l_coeff[dy][7]) * ((int)hor_buf[hor_buf_offset + 7 * 1]) + (1 << (12 - 1))) >> 12));
						pred[pred_offset] = ((pel)((0 >(((1 << 8) - 1 < ((int)pt) ? (1 << 8) - 1 : ((int)pt))) ? 0 : (((1 << 8) - 1 < ((
							int)pt) ? (1 << 8) - 1 : ((int)pt))))));
					}
				}
			}
		}
	}
}

static void pred2block(pel *pred, pel *block, int s_pred, int s_block, int dx, int dy, int w, int h)
{
	int i;
	int j;
	pred += dx + 2 + (dy + 2) * s_pred;
	for (i = 0; i < h; i++) {

#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < w; j++) {

#pragma HLS LOOP_TRIPCOUNT max=64
			block[j] = pred[j * 4];
		}
		block += s_block;
		pred += s_pred * 4;
	}
}


static void derive_mv_range_com(s16 mv_com, S14 ctrpos[2][2][2], s16 mv_clip_range[RANGE_NUM], int cu_size_dir, int pos_father, int pos, BOOL chroma_motion, int cur_mvr, u8 mv_dir)
{

#pragma HLS inline off

	int mv_x = mv_com + (pos << 2);

	int offset_min, offset_max;

	int pattern = mv_x & 0x3;

	if (pattern == 0 || cur_mvr >= 2) {
		offset_min = 0;
		offset_max = 0;
	}
	else {
		offset_min = -3;
		offset_max = 4;
	}

	mv_clip_range[RANGE_MIN] = ctrpos[RANGE_MIN][mv_dir][PIC_DIM] - offset_min - pos;

	mv_clip_range[RANGE_MAX] = ctrpos[RANGE_MAX][mv_dir][PIC_DIM] - cu_size_dir - offset_min - pos;
}

static int mv_inbound_clip_mvr(s16 mv[MV_D], s16 mv_clip[MV_D], s16 mvp[MV_D], S14 ctrpos[2][2][2], int cu_width, int cu_height, int x_father, int y_father, int x_pos, int y_pos, BOOL chroma_motion, int cur_mvr, int bits = 2){

#pragma HLS array_partition variable=mv complete dim=0
#pragma HLS array_partition variable=mv_clip complete dim=0
#pragma HLS array_partition variable=mvp complete dim=0
#pragma HLS array_partition variable=ctrpos complete dim=0

	s16 mv_clip_range[MV_D][RANGE_NUM];
	s16 mv_temp[MV_D];
	u8 mv_dir;
	int cu_size[MV_D] = { cu_width, cu_height };
	int pos[MV_D] = { x_pos, y_pos };
	int father_pos[MV_D] = { x_father, y_father };
#pragma HLS array_partition variable=mv_clip_range complete dim=0
#pragma HLS array_partition variable=mv_temp complete dim=0
#pragma HLS array_partition variable=cu_size complete dim=0
#pragma HLS array_partition variable=pos complete dim=0
#pragma HLS array_partition variable=father_pos complete dim=0


	for (mv_dir = 0; mv_dir < MV_D; mv_dir++)
	{
#pragma HLS UNROLL
		derive_mv_range_com(mv[mv_dir], ctrpos, mv_clip_range[mv_dir], cu_size[mv_dir], father_pos[mv_dir], pos[mv_dir], chroma_motion, cur_mvr, mv_dir);

		mv_temp[mv_dir] = COM_CLIP3(mv_clip_range[mv_dir][RANGE_MIN], mv_clip_range[mv_dir][RANGE_MAX], mv[mv_dir]);

		derive_mv_range_com(mv_temp[mv_dir], ctrpos, mv_clip_range[mv_dir], cu_size[mv_dir], father_pos[mv_dir], pos[mv_dir], chroma_motion, cur_mvr, mv_dir);
        
		mv_clip[mv_dir] = mv_temp[mv_dir];
	}
	return 1;
}



int pinter_fme_all_cu_pred(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D])
{
#pragma HLS array_partition variable=sub_pel_mv complete dim=0
#pragma HLS array_partition variable=mvp_all complete dim=0
#pragma HLS array_partition variable=mv_all complete dim=0
#pragma HLS array_partition variable=best_mv_uni_inner complete dim=0
//#pragma HLS array_partition variable=CtrPos complete dim=0
	if (x_pos == 280 && y_pos == 440 && cu_width == 8 && cu_height == 8)
		int u = 0;

	for (refi_cur = 0; refi_cur < 2; refi_cur++)
	{
#pragma HLS loop_tripcount max = 2
#pragma HLS UNROLL
		if(refi_cur < num_refp_cnt){
		if (me_level > 1 && (curr_mvr == 0 || curr_mvr == 1))
		{
			mv_all[refi_cur][curr_mvr][MV_X] = sub_pel_mv[MV_X];
			mv_all[refi_cur][curr_mvr][MV_Y] = sub_pel_mv[MV_Y];
		}

		if (curr_mvr < MAX_NUM_AFFINE_MVR)
		{
			best_mv_uni_inner[lidx][refi_cur][MV_X] = mv_all[refi_cur][curr_mvr][MV_X];
			best_mv_uni_inner[lidx][refi_cur][MV_Y] = mv_all[refi_cur][curr_mvr][MV_Y];
		}}
	}

	return 0;
}
int pinter_fme_all_cu_pred_8_16(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D])
{
	if (x_pos == 280 && y_pos == 440 && cu_width == 8 && cu_height == 8)
		int u = 0;

	for (refi_cur = 0; refi_cur < 2; refi_cur++)
	{
#pragma HLS UNROLL
		if (refi_cur < num_refp_cnt) {
			if (me_level > 1 && (curr_mvr == 0 || curr_mvr == 1))
			{
				mv_all[refi_cur][curr_mvr][MV_X] = sub_pel_mv[MV_X];
				mv_all[refi_cur][curr_mvr][MV_Y] = sub_pel_mv[MV_Y];
			}

			if (curr_mvr < MAX_NUM_AFFINE_MVR)
			{
				best_mv_uni_inner[lidx][refi_cur][MV_X] = mv_all[refi_cur][curr_mvr][MV_X];
				best_mv_uni_inner[lidx][refi_cur][MV_Y] = mv_all[refi_cur][curr_mvr][MV_Y];
			}
		}
	}

	return 0;
}
int pinter_fme_all_cu_pred_16_8(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D])
{

	if (x_pos == 280 && y_pos == 440 && cu_width == 8 && cu_height == 8)
		int u = 0;

	for (refi_cur = 0; refi_cur < 2; refi_cur++)
	{
#pragma HLS UNROLL
		if (refi_cur < num_refp_cnt) {
			if (me_level > 1 && (curr_mvr == 0 || curr_mvr == 1))
			{
				mv_all[refi_cur][curr_mvr][MV_X] = sub_pel_mv[MV_X];
				mv_all[refi_cur][curr_mvr][MV_Y] = sub_pel_mv[MV_Y];
			}

			if (curr_mvr < MAX_NUM_AFFINE_MVR)
			{
				best_mv_uni_inner[lidx][refi_cur][MV_X] = mv_all[refi_cur][curr_mvr][MV_X];
				best_mv_uni_inner[lidx][refi_cur][MV_Y] = mv_all[refi_cur][curr_mvr][MV_Y];
			}
		}
	}

	return 0;
}
int pinter_fme_all_cu_pred_16_16(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D])
{
	//#pragma HLS array_partition variable=CtrPos complete dim=0
	if (x_pos == 280 && y_pos == 440 && cu_width == 8 && cu_height == 8)
		int u = 0;

	for (refi_cur = 0; refi_cur < 2; refi_cur++)
	{
#pragma HLS loop_tripcount max = 2
    if(refi_cur < num_refp_cnt){
		if (me_level > 1 && (curr_mvr == 0 || curr_mvr == 1))
		{
			mv_all[refi_cur][curr_mvr][MV_X] = sub_pel_mv[MV_X];
			mv_all[refi_cur][curr_mvr][MV_Y] = sub_pel_mv[MV_Y];
		}

		if (curr_mvr < MAX_NUM_AFFINE_MVR)
		{
			best_mv_uni_inner[lidx][refi_cur][MV_X] = mv_all[refi_cur][curr_mvr][MV_X];
			best_mv_uni_inner[lidx][refi_cur][MV_Y] = mv_all[refi_cur][curr_mvr][MV_Y];
		}
    }
	}

	return 0;
}



void fme_set_mv_refi(S14* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2], 
	MD_COM_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx)
{
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;
	//refi_L0L1[lidx] = best_refi;
	mv[0] = mv[0] >> curr_mvr << curr_mvr;
	mv[1] = mv[1] >> curr_mvr << curr_mvr;
	mvd[0] = mv[0] - mvp_all[best_refi][curr_mvr][0];
	mvd[1] = mv[1] - mvp_all[best_refi][curr_mvr][1];
	{
		int amvr_shift = curr_mvr;
		mvd[0] = mvd[0] >> amvr_shift << amvr_shift;
		mvd[1] = mvd[1] >> amvr_shift << amvr_shift;
		int mv_x = mvd[0] + mvp_all[best_refi][curr_mvr][0];
		int mv_y = mvd[1] + mvp_all[best_refi][curr_mvr][1];
		mv[0] = (-32768) > (32767 < mv_x ? 32767 : mv_x) ? (-32768) : (32767 < mv_x ? 32767 : mv_x);
		mv[1] = (-32768) > (32767 < mv_y ? 32767 : mv_y) ? (-32768) : (32767 < mv_y ? 32767 : mv_y);
	}
}

void fme_set_mv_refi(S14* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx)
{
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;
	//refi_L0L1[lidx] = best_refi;
	mv[0] = mv[0] >> curr_mvr << curr_mvr;
	mv[1] = mv[1] >> curr_mvr << curr_mvr;
	mvd[0] = mv[0] - mvp_all[best_refi][curr_mvr][0];
	mvd[1] = mv[1] - mvp_all[best_refi][curr_mvr][1];
	{
		int amvr_shift = curr_mvr;
		mvd[0] = mvd[0] >> amvr_shift << amvr_shift;
		mvd[1] = mvd[1] >> amvr_shift << amvr_shift;
		int mv_x = mvd[0] + mvp_all[best_refi][curr_mvr][0];
		int mv_y = mvd[1] + mvp_all[best_refi][curr_mvr][1];
		mv[0] = (-32768) > (32767 < mv_x ? 32767 : mv_x) ? (-32768) : (32767 < mv_x ? 32767 : mv_x);
		mv[1] = (-32768) > (32767 < mv_y ? 32767 : mv_y) ? (-32768) : (32767 < mv_y ? 32767 : mv_y);
	}
}
void fme_set_mv_refi_8_16(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx)
{
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;
	//refi_L0L1[lidx] = best_refi;
	mv[0] = mv[0] >> curr_mvr << curr_mvr;
	mv[1] = mv[1] >> curr_mvr << curr_mvr;
	mvd[0] = mv[0] - mvp_all[best_refi][curr_mvr][0];
	mvd[1] = mv[1] - mvp_all[best_refi][curr_mvr][1];
	{
		int amvr_shift = curr_mvr;
		mvd[0] = mvd[0] >> amvr_shift << amvr_shift;
		mvd[1] = mvd[1] >> amvr_shift << amvr_shift;
		int mv_x = mvd[0] + mvp_all[best_refi][curr_mvr][0];
		int mv_y = mvd[1] + mvp_all[best_refi][curr_mvr][1];
		mv[0] = (-32768) > (32767 < mv_x ? 32767 : mv_x) ? (-32768) : (32767 < mv_x ? 32767 : mv_x);
		mv[1] = (-32768) > (32767 < mv_y ? 32767 : mv_y) ? (-32768) : (32767 < mv_y ? 32767 : mv_y);
	}
}
void fme_set_mv_refi_16_8(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx)
{
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;
	//refi_L0L1[lidx] = best_refi;
	mv[0] = mv[0] >> curr_mvr << curr_mvr;
	mv[1] = mv[1] >> curr_mvr << curr_mvr;
	mvd[0] = mv[0] - mvp_all[best_refi][curr_mvr][0];
	mvd[1] = mv[1] - mvp_all[best_refi][curr_mvr][1];
	{
		int amvr_shift = curr_mvr;
		mvd[0] = mvd[0] >> amvr_shift << amvr_shift;
		mvd[1] = mvd[1] >> amvr_shift << amvr_shift;
		int mv_x = mvd[0] + mvp_all[best_refi][curr_mvr][0];
		int mv_y = mvd[1] + mvp_all[best_refi][curr_mvr][1];
		mv[0] = (-32768) > (32767 < mv_x ? 32767 : mv_x) ? (-32768) : (32767 < mv_x ? 32767 : mv_x);
		mv[1] = (-32768) > (32767 < mv_y ? 32767 : mv_y) ? (-32768) : (32767 < mv_y ? 32767 : mv_y);
	}
}
void fme_set_mv_refi_16_16(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx)
{
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;
	//refi_L0L1[lidx] = best_refi;
	mv[0] = mv[0] >> curr_mvr << curr_mvr;
	mv[1] = mv[1] >> curr_mvr << curr_mvr;
	mvd[0] = mv[0] - mvp_all[best_refi][curr_mvr][0];
	mvd[1] = mv[1] - mvp_all[best_refi][curr_mvr][1];
	{
		int amvr_shift = curr_mvr;
		mvd[0] = mvd[0] >> amvr_shift << amvr_shift;
		mvd[1] = mvd[1] >> amvr_shift << amvr_shift;
		int mv_x = mvd[0] + mvp_all[best_refi][curr_mvr][0];
		int mv_y = mvd[1] + mvp_all[best_refi][curr_mvr][1];
		mv[0] = (-32768) > (32767 < mv_x ? 32767 : mv_x) ? (-32768) : (32767 < mv_x ? 32767 : mv_x);
		mv[1] = (-32768) > (32767 < mv_y ? 32767 : mv_y) ? (-32768) : (32767 < mv_y ? 32767 : mv_y);
	}
}


void fme_set_mv_refi_64(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_COM_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx, S3 refi_for_ip[2])
{
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	/*mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;*/
	refi_for_ip[0] = t0;
	refi_for_ip[1] = t1;
	//refi_L0L1[lidx] = best_refi;
	mv[0] = mv[0] >> curr_mvr << curr_mvr;
	mv[1] = mv[1] >> curr_mvr << curr_mvr;
	mvd[0] = mv[0] - mvp_all[best_refi][curr_mvr][0];
	mvd[1] = mv[1] - mvp_all[best_refi][curr_mvr][1];
	{
		int amvr_shift = curr_mvr;
		mvd[0] = mvd[0] >> amvr_shift << amvr_shift;
		mvd[1] = mvd[1] >> amvr_shift << amvr_shift;
		int mv_x = mvd[0] + mvp_all[best_refi][curr_mvr][0];
		int mv_y = mvd[1] + mvp_all[best_refi][curr_mvr][1];
		mv[0] = (-32768) > (32767 < mv_x ? 32767 : mv_x) ? (-32768) : (32767 < mv_x ? 32767 : mv_x);
		mv[1] = (-32768) > (32767 < mv_y ? 32767 : mv_y) ? (-32768) : (32767 < mv_y ? 32767 : mv_y);
	}
}


void mv_clip(U16 x, U16 y, U13 pic_w, U13 pic_h, U10 w, U10 h, S3 refi[2], s16 mv[2][2], s16(*mv_t)[2])
{
	int min_clip[2];
	int max_clip[2];
	x <<= 2;
	y <<= 2;
	w <<= 2;
	h <<= 2;
	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0][0] = mv[0][0];
	mv_t[0][1] = mv[0][1];
	mv_t[1][0] = mv[1][0];
	mv_t[1][1] = mv[1][1];
	if (((int)refi[0]) >= 0) {
		if (x + ((int)mv[0][0]) < min_clip[0]) {
			mv_t[0][0] = ((min_clip[0] - x));
		}
		if (y + ((int)mv[0][1]) < min_clip[1]) {
			mv_t[0][1] = ((min_clip[1] - y));
		}
		if (x + ((int)mv[0][0]) + w - 4 > max_clip[0]) {
			mv_t[0][0] = ((max_clip[0] - x - w + 4));
		}
		if (y + ((int)mv[0][1]) + h - 4 > max_clip[1]) {
			mv_t[0][1] = ((max_clip[1] - y - h + 4));
		}
	}
	if (((int)refi[1]) >= 0) {
		if (x + ((int)mv[1][0]) < min_clip[0]) {
			mv_t[1][0] = ((min_clip[0] - x));
		}
		if (y + ((int)mv[1][1]) < min_clip[1]) {
			mv_t[1][1] = ((min_clip[1] - y));
		}
		if (x + ((int)mv[1][0]) + w - 4 > max_clip[0]) {
			mv_t[1][0] = ((max_clip[0] - x - w + 4));
		}
		if (y + ((int)mv[1][1]) + h - 4 > max_clip[1]) {
			mv_t[1][1] = ((max_clip[1] - y - h + 4));
		}
	}
}
void mv_clip_1(U16 x, U16 y, U13 pic_w, U13 pic_h, U10 w, U10 h, S3 refi[2], s16 mv[2][2], s16(*mv_t)[2])
{
#pragma HLS array_partition variable=mv complete dim=0
#pragma HLS array_partition variable=refi complete dim=0
	int min_clip[2];
	int max_clip[2];
#pragma HLS array_partition variable=min_clip complete dim=0
#pragma HLS array_partition variable=max_clip complete dim=0
	x <<= 2;
	y <<= 2;
	w <<= 2;
	h <<= 2;
	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0][0] = mv[0][0];
	mv_t[0][1] = mv[0][1];
	mv_t[1][0] = mv[1][0];
	mv_t[1][1] = mv[1][1];
	if (((int)refi[0]) >= 0) {
		if (x + ((int)mv[0][0]) < min_clip[0]) {
			mv_t[0][0] = ((min_clip[0] - x));
		}
		if (y + ((int)mv[0][1]) < min_clip[1]) {
			mv_t[0][1] = ((min_clip[1] - y));
		}
		if (x + ((int)mv[0][0]) + w - 4 > max_clip[0]) {
			mv_t[0][0] = ((max_clip[0] - x - w + 4));
		}
		if (y + ((int)mv[0][1]) + h - 4 > max_clip[1]) {
			mv_t[0][1] = ((max_clip[1] - y - h + 4));
		}
	}
	if (((int)refi[1]) >= 0) {
		if (x + ((int)mv[1][0]) < min_clip[0]) {
			mv_t[1][0] = ((min_clip[0] - x));
		}
		if (y + ((int)mv[1][1]) < min_clip[1]) {
			mv_t[1][1] = ((min_clip[1] - y));
		}
		if (x + ((int)mv[1][0]) + w - 4 > max_clip[0]) {
			mv_t[1][0] = ((max_clip[0] - x - w + 4));
		}
		if (y + ((int)mv[1][1]) + h - 4 > max_clip[1]) {
			mv_t[1][1] = ((max_clip[1] - y - h + 4));
		}
	}
}


static void com_mc_fetch(pel *refpic_y, MD_COM_MODE *mod_info_curr, CHANNEL_TYPE channel, int bit_depth, int pic_width,
	int pic_height, u8 curr_mvr, s16 CtrPos[2][2][2], pel pred_cache[64 * 64])
{
	//int scup = mod_info_curr->scup;
	int x = mod_info_curr->x_pos;
	int y = mod_info_curr->y_pos;
	int w = mod_info_curr->cu_width;
	int h = mod_info_curr->cu_height;
	int pic_w = pic_width;
	int pic_h = pic_height;
	S3 *refi = mod_info_curr->refi;
	//s16(*mv)[MV_D] = mod_info_curr->mv;
	int pred_stride = mod_info_curr->cu_width;
	//pel(*pred)[MAX_CU_DIM] = mod_info_curr->pred;
	int qpel_gmv_x;
	int qpel_gmv_y;
	s16 mv_t[2][2];
	int stride_l = SWW;
	//pel * refpic_y = Fetch_Refwin_ptr->RefWindowLCU;
	int qpel_gmv_x_in_sw = 0;
	int qpel_gmv_y_in_sw = 0;
	int fme = 0;
	int out = 0;
	int dx;
	int dy;
	int x_offset_min = 0;
	int x_offset_max = 0;
	int y_offset_min = 0;
	int y_offset_max = 0;

	mv_clip_1(x, y, pic_w, pic_h, w, h, refi, mod_info_curr->mv, mv_t);
	if (((int)refi[0]) >= 0) {
		/* forward */
		//ref_pic = refp_f.pic;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0][0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[0][1]);
		qpel_gmv_x_in_sw = qpel_gmv_x - (((int)CtrPos[0][0][0]) << 2) + (((int)CtrPos[0][0][1]) << 2);
		qpel_gmv_y_in_sw = qpel_gmv_y - (((int)CtrPos[0][1][0]) << 2) + (((int)CtrPos[0][1][1]) << 2);
		dx = qpel_gmv_x & 0x3;
		dy = qpel_gmv_y & 0x3;
		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			x_offset_max = 0;
			y_offset_min = 0;
			y_offset_max = 0;
			fme = 0;
		}
		else if (dx != 0 && dy == 0) {
			x_offset_min = -3;
			x_offset_max = 4;
			y_offset_min = 0;
			y_offset_max = 0;
			fme = 1;
		}
		else if (dx == 0 && dy != 0) {
			x_offset_min = 0;
			x_offset_max = 0;
			y_offset_min = -3;
			y_offset_max = 4;
			fme = 1;
		}
		else {
			x_offset_min = -3;
			x_offset_max = 4;
			y_offset_min = -3;
			y_offset_max = 4;
			fme = 1;
		}
#if MD_PRINT
		if (x_offset_min + (qpel_gmv_x >> 2) < ((int)CtrPos[0][0][0])
			|| x_offset_max + (qpel_gmv_x >> 2) > ((int)CtrPos[1][0][0])) {
			printf("X out->[%d,%d],", (int)CtrPos[0][0][0], (int)CtrPos[1][0][0]);
			out = 1;
		}
		if (y_offset_min + (qpel_gmv_y >> 2) < ((int)CtrPos[0][1][0])
			|| y_offset_max + (qpel_gmv_y >> 2) > ((int)CtrPos[1][1][0])) {
			printf("Y out->[%d,%d],", (int)CtrPos[0][1][0], (int)CtrPos[1][1][0]);
			out = 1;
		}
		if (out) {
			printf("Pos->[%d,%d],Level:%d\n", qpel_gmv_x, qpel_gmv_y, fme);
		}
		out == 0 ? ((void)0) : __assert_HLS("out == 0", "../separate/com_mc.c", (unsigned int)3518, __FUNCTION__);
#endif
		if (((unsigned int)channel) != ((unsigned int)CHANNEL_C)) {
			//com_mc_l( mv[REFP_0][0], mv[REFP_0][1], ref_pic->y, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_luma, pred_stride, pred[Y_C], w, h, bit_depth );
			//add by luke
			com_tbl_mc_l_1(mod_info_curr->mv[REFP_0][0], mod_info_curr->mv[REFP_0][1], refpic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l, pred_stride, pred_cache, w, h, bit_depth);
			// Output pred from fme, mc only when fme was skipped.
			//com_mc_l_00(refpic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l, pred_stride, pred_cache, w, h, 0);
		}
	}
}

void init_skip_merge_rdo(SKIP_RDO *skip_rdo, MERGE_RDO *merge_rdo, S7 mode_list[MAX_INTER_SKIP_RDO])
{
#pragma HLS array_partition variable=mode_list complete dim=0
	int i;
	skip_rdo->num_rdo = 0;

	for (i = 0; i < MAX_INTER_SKIP_RDO; ++i){
#pragma HLS loop_tripcount max = 12
#pragma HLS PIPELINE
#pragma HLS UNROLL
		mode_list[i] = -1;
	}
}





static void init_ame_buffer(ENC_AFFME_IN *in)
{
	U13 i = 0;
	//int j = 0;
	//int k = 0;
	for (i = 0; i < (1 << 7 >> 1) * (1 << 7 >> 1); i++) {

#pragma HLS loop_tripcount max = 4096
#pragma HLS PIPELINE
		in->pred_buf[i] = ((pel)0);
		in->p_error[i] = ((pel)0);
		in->i_gradient[0][i] = 0;
		in->i_gradient[1][i] = 0;
	}
	for (i = 0; i < 4; i++) {

#pragma HLS loop_tripcount max = 3
#pragma HLS PIPELINE
		in->ac_mv_scale[i][0] = 0;
		in->cp_mv_four[i][0] = 0;
		in->temp_affine_mvp[i][0] = 0;
		in->temp_trans_mv[i][0] = 0;
		in->ac_mv_scale[i][1] = 0;
		in->cp_mv_four[i][1] = 0;
		in->temp_affine_mvp[i][1] = 0;
		in->temp_trans_mv[i][1] = 0;
	}
	for (i = 0; i < 2; i++) {

#pragma HLS loop_tripcount max = 2
#pragma HLS PIPELINE
		in->mvf_range[i][0] = 0;
		in->mvf_range[i][1] = 0;
	}
	for (i = 0; i < 2; i++) {

#pragma HLS loop_tripcount max = 2
#pragma HLS PIPELINE
		in->mv_cliped[i][0] = 0;
		in->mv_cliped[i][1] = 0;
	}
}

static void set_ame_global_info(ENC_AFFME_IN *in)
{
	in->pic_height_chroma = in->pic_height_luma >> 1;
	in->pic_width_chroma = in->pic_width_luma >> 1;
	in->stride_org_y = 64;
	in->stride_org_uv = 64;
	in->u2v_x_offset = 32;
}

static void init_ame_in(ENC_AFFME_IN *in)
{
	init_ame_buffer(in);
	set_ame_global_info(in);
}

void get_affme_in(MD_FW *md_fw_ptr, ENC_AFFME_IN *in, MD_COM_MODE *mod_info_curr, U1 lidx,
	MD_INPUT *md_input_ptr, u8 curr_mvr, u32 lambda_mv)
{
	U13 x = mod_info_curr->x_pos;
	U13 y = mod_info_curr->y_pos;
	U13 x_in_buf = x - md_input_ptr->pix_x;
	U13 y_in_buf = y - md_input_ptr->pix_y;
	U12 x_in_uv = x_in_buf >> 1;
	U12 y_in_uv = y_in_buf >> 1;
	U5 i = 0;
	U5 j = 0;
	in->pic_height_luma = md_fw_ptr->pic_height;
	in->pic_width_luma = md_fw_ptr->pic_width;
	init_ame_in(in);
	in->org_offset[0] = x_in_buf + in->stride_org_y * y_in_buf;
	in->org_offset[1] = x_in_uv + in->stride_org_uv * y_in_uv;
	in->org_offset[2] = x_in_uv + in->u2v_x_offset + in->stride_org_uv * y_in_uv;
	in->lidx = lidx;
	in->bit_depth = md_fw_ptr->bit_depth_internal;
#if ENABLE_BFRAME
	in->num_refp = md_fw_ptr->num_refp[0]; //((u8 )ctx->rpm.num_refp[lidx]);
#else
	in->num_refp = md_fw_ptr->num_refp; //((u8 )ctx->rpm.num_refp[lidx]);
#endif
	
	/* ctx and pi removal. */
	in->lambda_mv = lambda_mv;
	//in->mot_bits = mot_bits;
	in->curr_mvr = curr_mvr;
	//in->refp = pi->refp;
	//memcpy(in->refp, pi->refp, sizeof(COM_REFP)*MAX_NUM_REF_PICS*REFP_NUM);
	for (i = 0; i < 17; i++) {

#pragma HLS loop_tripcount max = 17
		for (j = 0; j < 2; j++) {

#pragma HLS loop_tripcount max = 2
			in->refp_ptr[i][j] = md_fw_ptr->refp_ptr[i][j];
		}
	}
	//in->affine_mvp_scale = (affine_mvp_scale);
	//in->affine_mv_scale = (affine_mv_scale);
	//in->map_scu = ctx->map.map_scu;
	in->ptr = md_fw_ptr->cur_ptr;
}

static void com_get_affine_mvp_scaling_line(MD_FW *md_fw_ptr, ENC_ME_LINE_MAP *me_line_ptr, MD_COM_MODE *mod_info_curr, int refp_ptr[17][2],
	int ptr, U1 lidx, CPMV mvp[4][2], u8 curr_mvr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN])
{
	U19 scup = mod_info_curr->scup;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	s8 cur_refi = mod_info_curr->refi[lidx];
	int ptr_cur_ref;
	U11 x_scu = scup % pic_width_in_scu;
	U11 y_scu = scup / pic_width_in_scu;
	U5 cu_width_in_scu = cu_width >> 2;
	U3 i;
	U3 j;
	S2 refi_tmp;
	s32 mvp_lt[2];
	s32 mvp_rt[2];
	U19 neb_addr[5];
	U1 valid_flag[5];
	U19 neb_addr_org[5];
	U11 x_scu_line[5];
	U11 y_scu_line[5];
	U11 x_scu_left = 0;
	U11 offset_in_line;
	S4 valid_neb_idx = -1;
	U1 use_line = 1;
	U1 d_inner = 1;
	U1 prt = 0;
	S2 refi_nebs[NEB_NUMS] = { -1, -1 };
	//u32 scu_nebs[NEB_NUMS];
	S14 mv_nebs[NEB_NUMS][MV_D] = { 0 };

	ptr_cur_ref = refp_ptr[cur_refi][lidx];
	for (j = 0; j < 4; j++) {

#pragma HLS loop_tripcount max = 2
		mvp[j][0] = 0;
		mvp[j][1] = 0;
	}
	////-------------------  LT  -------------------//
	//neb_addr_lt[0] = scup - 1;                     // A
	//neb_addr_lt[1] = scup - pic_width_in_scu;      // B
	//neb_addr_lt[2] = scup - pic_width_in_scu - 1;  // D
	//valid_flag_lt[0] = x_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_lt[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_lt[0]]);
	//valid_flag_lt[1] = y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_lt[1]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_lt[1]]);
	//valid_flag_lt[2] = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_lt[2]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_lt[2]]);
	//valid_flag_org[0] = valid_flag_lt[0] && REFI_IS_VALID(map_refi[neb_addr_lt[0]][lidx]);
	//valid_flag_org[1] = valid_flag_lt[1] && REFI_IS_VALID(map_refi[neb_addr_lt[1]][lidx]);
	//valid_flag_org[3] = valid_flag_lt[2] && REFI_IS_VALID(map_refi[neb_addr_lt[2]][lidx]);
	////-------------------  RT  -------------------//
	//neb_addr_rt[0] = scup - pic_width_in_scu + cu_width_in_scu - 1;     // G
	//neb_addr_rt[1] = scup - pic_width_in_scu + cu_width_in_scu;         // C
	//valid_flag_rt[0] = y_scu > 0 && MCU_GET_CODED_FLAG(map_scu[neb_addr_rt[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_rt[0]]);
	//valid_flag_rt[1] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu && MCU_GET_CODED_FLAG(map_scu[neb_addr_rt[1]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr_rt[1]]);
	//valid_flag_org[4] = valid_flag_rt[0] && REFI_IS_VALID(map_refi[neb_addr_rt[0]][lidx]);
	//valid_flag_org[2] = valid_flag_rt[1] && REFI_IS_VALID(map_refi[neb_addr_rt[1]][lidx]);
	//-------------------  LT  -------------------//
	// A
	neb_addr[0] = neb_addr_org[0] = scup - 1;
	// B
	neb_addr[1] = neb_addr_org[1] = scup - pic_width_in_scu;
	// D
	neb_addr[3] = neb_addr_org[3] = scup - pic_width_in_scu - 1;
	valid_flag[0] = x_scu > 0;
	valid_flag[1] = y_scu > 0;
	valid_flag[3] = x_scu > 0 && y_scu > 0;
	x_scu_line[0] = x_scu - me_line_ptr->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_ptr->min_scu_y;
	x_scu_line[1] = x_scu - me_line_ptr->min_scu_x;
	y_scu_line[1] = y_scu - me_line_ptr->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_ptr->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_ptr->min_scu_y - 1;
	//-------------------  RT  -------------------//
	// G
	neb_addr[4] = neb_addr_org[4] = scup - pic_width_in_scu + cu_width_in_scu - 1;
	// C
	neb_addr[2] = neb_addr_org[2] = scup - pic_width_in_scu + cu_width_in_scu;
	valid_flag[4] = y_scu > 0;
	valid_flag[2] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu;
	x_scu_line[4] = x_scu - me_line_ptr->min_scu_x + cu_width_in_scu - 1;
	y_scu_line[4] = y_scu - me_line_ptr->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_ptr->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_ptr->min_scu_y - 1;
	if (x_scu == me_line_ptr->min_scu_x && use_line) {
		offset_in_line = y_scu - me_line_ptr->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].refi[lidx];
			valid_flag[0] = valid_flag[0]
				&& ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]] >> 15 & ((
				unsigned int)1)))
				&& ((int)inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].refi[lidx]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = inner_nebs[1 -
				me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].mv[lidx][0];
			mv_nebs[0][1] = inner_nebs[1 -
				me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_ptr->min_scu_y) {
			offset_in_line = y_scu - me_line_ptr->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].refi[lidx];
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]] >> 31 & ((
					unsigned int)1)))
					&& !((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]] >> 15 & ((
					unsigned int)1)))
					&& ((int)inner_nebs[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].refi[lidx]) >= 0;
			}
			if (valid_flag[3]) {
				mv_nebs[3][0] = inner_nebs[1 -
					me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].mv[lidx][0];
				mv_nebs[3][1] = inner_nebs[1 -
					me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]].mv[lidx][1];
			}
			d_inner = 0;
		}
	}
	else {
		if (valid_flag[0]) {
			refi_nebs[0] = inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]].refi[lidx];
			valid_flag[0] = valid_flag[0]
				&& ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]] >> 15 & ((
				unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
			//assert(MCU_GET_CODED_FLAG(map_scu[neb_addr[0]]) == MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]]));
			//assert(MCU_GET_INTRA_FLAG(map_scu[neb_addr[0]]) == MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]]));
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]].mv[lidx][0];
			mv_nebs[0][1] =
				inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]].mv[lidx][1];
		}
	}
	if (y_scu == me_line_ptr->min_scu_y && use_line) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
		// G
		neb_addr[4] = offset_in_line + cu_width_in_scu - 1;
		for (i = 1; i < 5; i++) {

#pragma HLS loop_tripcount max = 5
			if (d_inner == 0 && i == 3) {
				continue;
			}
			if (valid_flag[i]) {
				refi_nebs[i] = above_nebs_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET].refi[lidx];
				valid_flag[i] = valid_flag[i]
					&& ((int)(me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET] >> 31 & ((unsigned int)1)))
					&& !((int)(me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET] >> 15 & ((unsigned int)1)))
					&& ((int)refi_nebs[i]) >= 0;
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] = above_nebs_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][0];
				mv_nebs[i][1] = above_nebs_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET].mv[lidx][1];
			}
		}
	}
	else {
		for (i = 1; i < 5; i++) {

#pragma HLS loop_tripcount max = 5
			if (d_inner == 0 && i == 3) {
				continue;
			}
			if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
				valid_flag[i] = 0;
			}
			if (valid_flag[i]) {
				refi_nebs[i] = inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]].refi[lidx];
				valid_flag[i] = valid_flag[i]
					&& ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]] >> 31 & ((
					unsigned int)1)))
					&& !((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
				//assert(MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) == MCU_GET_CODED_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]]));
				//assert(MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) == MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]]));
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] =
					inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]].mv[lidx][0];
				mv_nebs[i][1] =
					inner_nebs[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]].mv[lidx][1];
			}
		}
	}
	if (valid_flag[0]) {
		valid_neb_idx = 0;
	}
	else if (valid_flag[1]) {
		valid_neb_idx = 1;
	}
	else if (valid_flag[3]) {
		valid_neb_idx = 3;
	}
	if (valid_neb_idx >= 0) {
		refi_tmp = ((int)refi_nebs[valid_neb_idx]);
		int ptr_neb_ref = refp_ptr[refi_tmp][lidx];
		scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, (s32 *)mv_nebs[valid_neb_idx], mvp_lt);
	}
	else {
		mvp_lt[0] = ((s16)0);
		mvp_lt[1] = ((s16)0);
	}
	if (valid_neb_idx >= 0) {
		int in_map = valid_neb_idx;
		if (valid_neb_idx == 3) {
			in_map = valid_neb_idx - 1;
		}
		if (prt) {
#if MD_PRINT
			printf("(%d,%d,%d,%d)", in_map, neb_addr[valid_neb_idx], (int)mv_nebs[valid_neb_idx][0],
				(int)mv_nebs[valid_neb_idx][1]);
#endif
		}
	}
	else {
		if (prt) {
#if MD_PRINT
			printf("(%d,%d,%d,%d)", -1, -1, 0, 0);
#endif
		}
	}

	valid_neb_idx = -1;
	if (valid_flag[4]) {
		valid_neb_idx = 4;
	}
	else if (valid_flag[2]) {
		valid_neb_idx = 2;
	}
	if (valid_neb_idx >= 0) {
		refi_tmp = ((int)refi_nebs[valid_neb_idx]);
		int ptr_neb_ref = refp_ptr[refi_tmp][lidx];
		scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, (s32 *)mv_nebs[valid_neb_idx], mvp_rt);
	}
	else {
		mvp_rt[0] = 0;
		mvp_rt[1] = 0;
	}
	if (valid_neb_idx >= 0) {
		int in_map = valid_neb_idx;
		if (valid_neb_idx == 4) {
			in_map = 0;
		}
		else {
			in_map = 1;
		}
		if (prt) {
#if MD_PRINT
			printf("(%d,%d,%d,%d)", in_map, neb_addr[valid_neb_idx], (int)mv_nebs[valid_neb_idx][0],
				(int)mv_nebs[valid_neb_idx][1]);
#endif
		}
	}
	else {
		if (prt) {
#if MD_PRINT
			printf("(%d,%d,%d,%d)", -1, -1, 0, 0);
#endif
		}
	}
	/*for (k = 0; k < AFFINE_MAX_NUM_RT; k++)
	{
	if (valid_flag_rt[k] && REFI_IS_VALID(map_refi[neb_addr_rt[k]][lidx]))
	{
	refi_tmp = map_refi[neb_addr_rt[k]][lidx];
	int ptr_neb_ref = refp[refi_tmp][lidx].ptr;
	scaling_mv1(ptr, ptr_cur_ref, ptr, ptr_neb_ref, map_mv[neb_addr_rt[k]][lidx], mvp_rt);
	cnt_rt++;
	break;
	}
	}
	if (cnt_rt == 0)
	{
	mvp_rt[MV_X] = 0;
	mvp_rt[MV_Y] = 0;
	cnt_rt++;
	}*/
	mvp[0][0] = mvp_lt[0];
	mvp[0][1] = mvp_lt[1];
	mvp[1][0] = mvp_rt[0];
	mvp[1][1] = mvp_rt[1];
	for (i = 0; i < 2; i++) {

#pragma HLS loop_tripcount max = 2
		// convert to 1/16 precision
		CPMV mvp_x = mvp[i][0] << 2;
		CPMV mvp_y = mvp[i][1] << 2;
		// rounding
		int amvr_shift = curr_mvr == 0 ? 2 : ((curr_mvr == 1 ? 4 : 0));
		com_mv_rounding_s32(mvp_x, mvp_y, &mvp_x, &mvp_y, amvr_shift, amvr_shift);
		// clipping
		mvp[i][0] = (((-(1 << (18 - 1))) >((((1 << (18 - 1)) - 1) < mvp_x ? (CPMV)((
			1 << (18 - 1)) - 1) : mvp_x)) ? (CPMV)(-(1 << (18 - 1))) : (CPMV)((((1 << (18 - 1)) - 1) < mvp_x ? ((
			1 << (18 - 1)) - 1) : mvp_x))));
		mvp[i][1] = (((-(1 << (18 - 1))) > ((((1 << (18 - 1)) - 1) < mvp_y ? (CPMV)((
			1 << (18 - 1)) - 1) : mvp_y)) ? (CPMV)(-(1 << (18 - 1))) : (CPMV)((((1 << (18 - 1)) - 1) < mvp_y ? ((
			1 << (18 - 1)) - 1) : mvp_y))));
	}
}

static s8 get_com_tbl_log2(U8 a)
{
	s8 com_tbl_log2[257] = { ((s8)(-1)), ((s8)(-1)), ((s8)1), ((s8)(-1)), ((s8)2), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)3), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)4), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)5), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)6), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)7), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)(-1)), ((s8)8)
		/* 0, 1 */
		/* 2, 3 */
		/* 4 ~ 7 */
		/* 8 ~ 15 */
		/* 16 ~ 31 */
		/* 31 ~ 63 */
		/* 64 ~ 127 */
		/* 128 ~ 255 */
		/* 256 */
	};
	return com_tbl_log2[a];
}

static void calc_mvf_range(CPMV mv_range[2][2], s32 cp_mv_four[4][2], U7 cu_width, U7 cu_height, CPMV ac_mv[4][2],
	U2 cp_num, U13 x, U13 y, U4 sub_w, U4 sub_h)
{
	s8 cu_width_log2 = get_com_tbl_log2(cu_width);
	s8 cu_height_log2 = get_com_tbl_log2(cu_height);
#if MD_PRINT
	cu_width_log2 >= 4 ? ((void)0) : __assert_HLS("get_com_tbl_log2(cu_width) >= 4",
		"../separate/enc_pinter.c", (unsigned int)1819, __FUNCTION__);
	cu_height_log2 >= 4 ? ((void)0) : __assert_HLS("get_com_tbl_log2(cu_height) >= 4",
		"../separate/enc_pinter.c", (unsigned int)1820, __FUNCTION__);
#endif
	u8 half_w;
	u8 half_h;
	U2 i;
	s32 dmv_hor_x;
	s32 dmv_ver_x;
	s32 dmv_hor_y;
	s32 dmv_ver_y;
	s32 mv_scale_hor = (ac_mv[0][0]) << 7;
	s32 mv_scale_ver = (ac_mv[0][1]) << 7;
	U1 limit_qpos_paterren = 0;
	half_w = sub_w >> 1;
	half_h = sub_h >> 1;
	for (i = 0; i < cp_num; i++) {

#pragma HLS loop_tripcount max = 2
#if MD_PRINT
		ac_mv[i][0] >= ((s32)(-(1 << (18 - 1)))) && ac_mv[i][0] <= ((s32)((1 << (18 - 1)) - 1)) ? ((
			void)0) : __assert_HLS("ac_mv[i][0] >= ((s32)(-(1<<(18 - 1)))) && ac_mv[i][0] <= ((s32)((1<<(18 - 1)) - 1))",
			"../separate/enc_pinter.c", (unsigned int)1832, __FUNCTION__);
		ac_mv[i][1] >= ((s32)(-(1 << (18 - 1)))) && ac_mv[i][1] <= ((s32)((1 << (18 - 1)) - 1)) ? ((
			void)0) : __assert_HLS("ac_mv[i][1] >= ((s32)(-(1<<(18 - 1)))) && ac_mv[i][1] <= ((s32)((1<<(18 - 1)) - 1))",
			"../separate/enc_pinter.c", (unsigned int)1833, __FUNCTION__);
#endif
	}
	// deltaMvHor
	dmv_hor_x = (ac_mv[1][0] - ac_mv[0][0]) << 7 >> cu_width_log2;
	dmv_hor_y = (ac_mv[1][1] - ac_mv[0][1]) << 7 >> cu_width_log2;
	// deltaMvVer
	dmv_ver_x = -dmv_hor_y;
	dmv_ver_y = dmv_hor_x;
	cp_mv_four[0][0] = mv_scale_hor;
	cp_mv_four[0][1] = mv_scale_ver;
	cp_mv_four[1][0] = mv_scale_hor + dmv_hor_x * cu_width;
	cp_mv_four[1][1] = mv_scale_ver + dmv_hor_y * cu_width;
	cp_mv_four[2][0] = mv_scale_hor - dmv_hor_y * (cu_height - half_h);
	cp_mv_four[2][1] = mv_scale_ver + dmv_hor_x * (cu_height - half_h);
	cp_mv_four[3][0] = mv_scale_hor + dmv_hor_x * (cu_width - half_w) - dmv_hor_y * (cu_height - half_h);
	cp_mv_four[3][1] = mv_scale_ver + dmv_hor_y * (cu_width - half_w) + dmv_hor_x * (cu_height - half_h);
	mv_range[0][0] = (cp_mv_four[0][0] < ((cp_mv_four[1][0] < ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] :
		cp_mv_four[3][0])) ? cp_mv_four[1][0] : ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))))
		? cp_mv_four[0][0] : ((cp_mv_four[1][0] < ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))
		? cp_mv_four[1][0] : ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0])))));
	mv_range[1][0] = (cp_mv_four[0][0] > ((cp_mv_four[1][0] > ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] :
		cp_mv_four[3][0])) ? cp_mv_four[1][0] : ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))))
		? cp_mv_four[0][0] : ((cp_mv_four[1][0] > ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))
		? cp_mv_four[1][0] : ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0])))));
	mv_range[0][1] = (cp_mv_four[0][1] < ((cp_mv_four[1][1] < ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] :
		cp_mv_four[3][1])) ? cp_mv_four[1][1] : ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))))
		? cp_mv_four[0][1] : ((cp_mv_four[1][1] < ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))
		? cp_mv_four[1][1] : ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1])))));
	mv_range[1][1] = (cp_mv_four[0][1] > ((cp_mv_four[1][1] > ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] :
		cp_mv_four[3][1])) ? cp_mv_four[1][1] : ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))))
		? cp_mv_four[0][1] : ((cp_mv_four[1][1] > ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))
		? cp_mv_four[1][1] : ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1])))));
	if ((dmv_hor_x >= 0 && dmv_hor_y >= 0) || (dmv_hor_x >= 0 && dmv_hor_y < 0)) {
		limit_qpos_paterren = 0;
	}
	else {
		limit_qpos_paterren = 1;
	}
	com_mv_rounding_s32(mv_range[0][0], mv_range[0][1], &mv_range[0][0], &mv_range[0][1], 7, 0);
	com_mv_rounding_s32(mv_range[1][0], mv_range[1][1], &mv_range[1][0], &mv_range[1][1], 7, 0);
	mv_range[0][0] = ((-131072) >((131071 < mv_range[0][0] ? (CPMV)131071 : mv_range[0][0])) ? (CPMV)(
		-131072) : ((131071 < mv_range[0][0] ? (CPMV)131071 : mv_range[0][0])));
	mv_range[1][0] = ((-131072) > ((131071 < mv_range[1][0] ? (CPMV)131071 : mv_range[1][0])) ? (CPMV)(
		-131072) : ((131071 < mv_range[1][0] ? (CPMV)131071 : mv_range[1][0])));
	mv_range[0][1] = ((-131072) > ((131071 < mv_range[0][1] ? (CPMV)131071 : mv_range[0][1])) ? (CPMV)(
		-131072) : ((131071 < mv_range[0][1] ? (CPMV)131071 : mv_range[0][1])));
	mv_range[1][1] = ((-131072) > ((131071 < mv_range[1][1] ? (CPMV)131071 : mv_range[1][1])) ? (CPMV)(
		-131072) : ((131071 < mv_range[1][1] ? (CPMV)131071 : mv_range[1][1])));
	if (limit_qpos_paterren) {
		mv_range[0][0] = mv_range[0][0] + (((x + cu_width - sub_w) << 4));
		mv_range[1][0] = mv_range[1][0] + (((x + 0) << 4));
		mv_range[0][1] = mv_range[0][1] + (((y + cu_height - sub_h) << 4));
		mv_range[1][1] = mv_range[1][1] + (((y + 0) << 4));
	}
	else {
		mv_range[0][0] = mv_range[0][0] + (((x + 0) << 4));
		mv_range[1][0] = mv_range[1][0] + (((x + cu_width - sub_w) << 4));
		mv_range[0][1] = mv_range[0][1] + (((y + 0) << 4));
		mv_range[1][1] = mv_range[1][1] + (((y + cu_height - sub_h) << 4));
	}
}

static int is_mvf_valid(pel RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], CPMV mvf_range[2][2], U4 sub_w, U4 sub_h)
{
	U1 result = 1;
	U7 interp_offset = 4;
	if (mvf_range[0][0] - (interp_offset << 4) < (CtrPos[0][0][0]) << 4) {
		result = 0;
	}
	if (mvf_range[0][1] - (interp_offset << 4) < (CtrPos[0][1][0]) << 4) {
		result = 0;
	}
	if (mvf_range[1][0] + ((sub_w + interp_offset) << 4) > (CtrPos[1][0][0]) << 4) {
		result = 0;
	}
	if (mvf_range[1][1] + ((sub_h + interp_offset) << 4) > (CtrPos[1][1][0]) << 4) {
		result = 0;
	}
	return result;
}

static int adjust_cpmv_iter(pel RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], ENC_AFFME_IN *in, U13 x, U13 y, U13 pic_w, U13 pic_h, U7 cu_width, U7 cu_height,
	CPMV ac_mv[4][2], U2 cp_num, int mvr_idx, U4 sub_w, U4 sub_h)
{
	U2 i;
	s8 cu_width_log2 = get_com_tbl_log2(cu_width);
	s8 cu_height_log2 = get_com_tbl_log2(cu_height);
#if MD_PRINT
	cu_width_log2 >= 4 ? ((void)0) : __assert_HLS("get_com_tbl_log2(cu_width) >= 4",
		"../separate/enc_pinter.c", (unsigned int)1901, __FUNCTION__);
	cu_height_log2 >= 4 ? ((void)0) : __assert_HLS("get_com_tbl_log2(cu_height) >= 4",
		"../separate/enc_pinter.c", (unsigned int)1902, __FUNCTION__);
#endif
	U3 scale_iter_num = 4;
	U3 iter = 0;
	U3 numerator = 1;
	U3 denominator_log2 = 1;
	U2 vertex = 0;
	U1 prev_checked = 0;
	U4 zero_cnt = 0;
	U3 amvr_shift = (mvr_idx == 0 ? 2 : ((mvr_idx == 1 ? 4 : 0)));
	for (i = 0; i < cp_num; i++) {

#pragma HLS loop_tripcount max = 2
#if MD_PRINT
		ac_mv[i][0] >= ((s32)(-(1 << (18 - 1)))) && ac_mv[i][0] <= ((s32)((1 << (18 - 1)) - 1)) ? ((
			void)0) : __assert_HLS("ac_mv[i][0] >= ((s32)(-(1<<(18 - 1)))) && ac_mv[i][0] <= ((s32)((1<<(18 - 1)) - 1))",
			"../separate/enc_pinter.c", (unsigned int)1917, __FUNCTION__);
		ac_mv[i][1] >= ((s32)(-(1 << (18 - 1)))) && ac_mv[i][1] <= ((s32)((1 << (18 - 1)) - 1)) ? ((
			void)0) : __assert_HLS("ac_mv[i][1] >= ((s32)(-(1<<(18 - 1)))) && ac_mv[i][1] <= ((s32)((1<<(18 - 1)) - 1))",
			"../separate/enc_pinter.c", (unsigned int)1918, __FUNCTION__);
#endif
	}
	for (vertex = 0; vertex < cp_num; vertex++) {

#pragma HLS loop_tripcount max = 2
		in->ac_mv_scale[vertex][0] = ac_mv[vertex][0];
		in->ac_mv_scale[vertex][1] = ac_mv[vertex][1];
	}
	calc_mvf_range(in->mvf_range, in->cp_mv_four, cu_width, cu_height, in->ac_mv_scale, cp_num, x, y, sub_w, sub_h);
	if (!is_mvf_valid(RefWindowLCU, CtrPos, in->mvf_range, sub_w, sub_h)) {
		for (vertex = 0; vertex < cp_num; vertex++) {

#pragma HLS loop_tripcount max = 2
			in->ac_mv_scale[vertex][0] = ac_mv[vertex][0] >> 4;
			in->ac_mv_scale[vertex][1] = ac_mv[vertex][1] >> 4;
		}
		calc_mvf_range(in->mvf_range, in->cp_mv_four, cu_width, cu_height, in->ac_mv_scale, cp_num, x, y, sub_w, sub_h);
		if (!is_mvf_valid(RefWindowLCU, CtrPos, in->mvf_range, sub_w, sub_h)) {
			scale_iter_num = 0;
		}
		for (iter = 0; iter < scale_iter_num; iter++) {

#pragma HLS loop_tripcount max = 4
			zero_cnt = 0;
			for (vertex = 0; vertex < cp_num; vertex++) {

#pragma HLS loop_tripcount max = 2
				in->ac_mv_scale[vertex][0] = numerator * (ac_mv[vertex][0] >> denominator_log2);
				in->ac_mv_scale[vertex][1] = numerator * (ac_mv[vertex][1] >> denominator_log2);
				if (((int)amvr_shift) > 0) {
					com_mv_rounding_s32(in->ac_mv_scale[0][0], in->ac_mv_scale[0][1], &in->ac_mv_scale[0][0], &in->ac_mv_scale[0][1],
						(int)amvr_shift, (int)amvr_shift);
					com_mv_rounding_s32(in->ac_mv_scale[1][0], in->ac_mv_scale[1][1], &in->ac_mv_scale[1][0], &in->ac_mv_scale[1][1],
						(int)amvr_shift, (int)amvr_shift);
				}
				if (in->ac_mv_scale[vertex][0] == 0 && in->ac_mv_scale[vertex][1]) {
					zero_cnt++;
				}
			}
			calc_mvf_range(in->mvf_range, in->cp_mv_four, cu_width, cu_height, in->ac_mv_scale, cp_num, x, y, sub_w, sub_h);
			if (is_mvf_valid(RefWindowLCU, CtrPos, in->mvf_range, sub_w, sub_h)) {
				denominator_log2++;
				numerator <<= 1;
				numerator++;
				prev_checked = 1;
			}
			else if (prev_checked) {
				numerator--;
				numerator >>= 1;
				denominator_log2--;
				break;
			}
			else {
				denominator_log2++;
				numerator <<= 1;
				numerator--;
				prev_checked = 0;
			}
			if (zero_cnt == cp_num) {
				if (prev_checked) {
					numerator--;
					numerator >>= 1;
					denominator_log2--;
				}
				break;
			}
		}
		if (prev_checked) {
			for (vertex = 0; vertex < cp_num; vertex++) {

#pragma HLS loop_tripcount max = 2
				ac_mv[vertex][0] = numerator * (ac_mv[vertex][0] >> denominator_log2);
				ac_mv[vertex][1] = numerator * (ac_mv[vertex][1] >> denominator_log2);
			}
			if (((int)amvr_shift) > 0) {
				com_mv_rounding_s32(ac_mv[0][0], ac_mv[0][1], &ac_mv[0][0], &ac_mv[0][1], (int)amvr_shift, (int)amvr_shift);
				com_mv_rounding_s32(ac_mv[1][0], ac_mv[1][1], &ac_mv[1][0], &ac_mv[1][1], (int)amvr_shift, (int)amvr_shift);
			}
		}
		return prev_checked;
	}
	else {
		return 1;
	}
}

static void com_tbl_mc_l_hp(int ori_mv_x, int ori_mv_y, pel *ref, int gmv_x, int gmv_y, U10 s_ref, U7 s_pred,
	s16 *pred, U4 w, U4 h, U4 bit_depth)
{
	//add by luke
	int dx;
	int dy;
	dx = ori_mv_x & 0xF;
	dy = ori_mv_y & 0xF;
	if (dx == 0 && dy == 0) {
		//printf("00: ");
		com_mc_l_00(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, 1);
	}
	else if (dx == 0 && dy != 0) {
		//printf("0n: ");
		com_mc_l_0n(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
	}
	else if (dx != 0 && dy == 0) {
		//printf("n0: ");
		com_mc_l_n0(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
	}
	else if (dx != 0 && dy != 0) {
		//printf("nn: ");
		com_mc_l_nn(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
	}
}

static int com_affine_mc_l_fetch(pel RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U13 x, U13 y, U13 pic_w, U13 pic_h,
	U7 cu_width, U7 cu_height, CPMV ac_mv[4][2], pel pred[(1 << 7 >> 1) * (1 << 7 >> 1)], U2 cp_num, U4 sub_w,
	U4 sub_h, U4 bit_depth, U12 x_chroma, U12 y_chroma)
{
	U2 i;
	s8 cu_width_log2 = get_com_tbl_log2(cu_width);
	s8 cu_height_log2 = get_com_tbl_log2(cu_height);
#if MD_PRINT
	cu_width_log2 >= 4 ? ((void)0) : __assert_HLS("get_com_tbl_log2(cu_width) >= 4",
		"../separate/com_mc.c", (unsigned int)3837, __FUNCTION__);
	cu_height_log2 >= 4 ? ((void)0) : __assert_HLS("get_com_tbl_log2(cu_height) >= 4",
		"../separate/com_mc.c", (unsigned int)3838, __FUNCTION__);
#endif
	int qpel_gmv_x;
	int qpel_gmv_y;
	int qpel_gmv_x_in_sw = 0;
	int qpel_gmv_y_in_sw = 0;
	pel *pred_y = pred;
	U7 w;
	U7 h;
	U3 half_w;
	U3 half_h;
	s32 dmv_hor_x;
	s32 dmv_ver_x;
	s32 dmv_hor_y;
	s32 dmv_ver_y;
	s32 mv_scale_hor = (ac_mv[0][0]) << 7;
	s32 mv_scale_ver = (ac_mv[0][1]) << 7;
	CPMV mv_scale_tmp_hor;
	CPMV mv_scale_tmp_ver;
	s32 hor_max;
	s32 hor_min;
	s32 ver_max;
	s32 ver_min;
	s32 mv_scale_tmp_hor_ori;
	s32 mv_scale_tmp_ver_ori;
	pel *pic_y = RefWindowLCU;
	int stride_y = SWW;
	for (i = 0; i < cp_num; i++) {

#pragma HLS loop_tripcount max = 2
#if MD_PRINT
		ac_mv[i][0] >= (-(1 << (18 - 1))) && ac_mv[i][0] <= ((1 << (18 - 1)) - 1) ? (void)0 : __assert_HLS("ac_mv[i][0] >= ((s32)(-(1<<(18 - 1)))) && ac_mv[i][0] <= ((s32)((1<<(18 - 1)) - 1))",
			"../separate/com_mc.c", 3856, __FUNCTION__);
		ac_mv[i][1] >= (-(1 << (18 - 1))) && ac_mv[i][1] <= ((1 << (18 - 1)) - 1) ? (void)0 : __assert_HLS("ac_mv[i][1] >= ((s32)(-(1<<(18 - 1)))) && ac_mv[i][1] <= ((s32)((1<<(18 - 1)) - 1))",
			"../separate/com_mc.c", 3857, __FUNCTION__);
#endif
	}
	hor_max = (pic_w + (1 << 7) + 4 - x - cu_width + 1) << 4;
	ver_max = (pic_h + (1 << 7) + 4 - y - cu_height + 1) << 4;
	hor_min = (-(1 << 7) - 4 - x) << 4;
	ver_min = (-(1 << 7) - 4 - y) << 4;
	half_w = sub_w >> 1;
	half_h = sub_h >> 1;
	// deltaMvHor
	dmv_hor_x = (ac_mv[1][0] - ac_mv[0][0]) << 7 >> cu_width_log2;
	dmv_hor_y = (ac_mv[1][1] - ac_mv[0][1]) << 7 >> cu_width_log2;
	if (cp_num == 3) {
		// deltaMvVer
		dmv_ver_x = (ac_mv[2][0] - ac_mv[0][0]) << 7 >> cu_height_log2;
		dmv_ver_y = (ac_mv[2][1] - ac_mv[0][1]) << 7 >> cu_height_log2;
	}
	else {
		// deltaMvVer
		dmv_ver_x = -dmv_hor_y;
		dmv_ver_y = dmv_hor_x;
	}
	for (h = 0; h < cu_height; h += sub_h) {

#pragma HLS loop_tripcount max = 16
		for (w = 0; w < cu_width; w += sub_w) {

#pragma HLS loop_tripcount max = 16
			int pos_x = w + half_w;
			int pos_y = h + half_h;
			if (w == 0 && h == 0) {
				pos_x = 0;
				pos_y = 0;
			}
			else if (w + sub_w == cu_width && h == 0) {
				pos_x = cu_width;
				pos_y = 0;
			}
			else if (w == 0 && h + sub_h == cu_height && cp_num == 3) {
				pos_x = 0;
				pos_y = cu_height;
			}
			mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
			mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;
			com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
			mv_scale_tmp_hor = ((-131072) > ((131071 < mv_scale_tmp_hor ? (CPMV)131071 : mv_scale_tmp_hor)) ?
				(CPMV)(-131072) : (CPMV)((131071 < mv_scale_tmp_hor ? (CPMV)131071 : mv_scale_tmp_hor)));
			mv_scale_tmp_ver = ((-131072) > ((131071 < mv_scale_tmp_ver ? (CPMV)131071 : mv_scale_tmp_ver)) ?
				(CPMV)(-131072) : (CPMV)((131071 < mv_scale_tmp_ver ? (CPMV)131071 : mv_scale_tmp_ver)));
			mv_scale_tmp_hor_ori = mv_scale_tmp_hor;
			mv_scale_tmp_ver_ori = mv_scale_tmp_ver;
			mv_scale_tmp_hor = (hor_max < ((hor_min > mv_scale_tmp_hor ? (CPMV)hor_min : mv_scale_tmp_hor)) ? (CPMV)hor_max : ((CPMV)(
				hor_min > mv_scale_tmp_hor ? (CPMV)hor_min : mv_scale_tmp_hor)));
			mv_scale_tmp_ver = (ver_max < ((ver_min > mv_scale_tmp_ver ? (CPMV)ver_min : mv_scale_tmp_ver)) ? (CPMV)ver_max : ((CPMV)(
				ver_min > mv_scale_tmp_ver ? (CPMV)ver_min : mv_scale_tmp_ver)));
			qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
			qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;
			qpel_gmv_x_in_sw = qpel_gmv_x - (((int)CtrPos[0][0][0]) << 4) + (((
				int)CtrPos[0][0][1]) << 4);
			qpel_gmv_y_in_sw = qpel_gmv_y - (((int)CtrPos[0][1][0]) << 4) + (((
				int)CtrPos[0][1][1]) << 4);
			if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, CtrPos, sub_w, sub_h, x_chroma, y_chroma, x, y, 0, 4)) {
				//printf("%s:%d: Affine motion is out of ref window!\npos=(%d %d) size=(%d %d) scu=(%d %d) ac_mv=(", __FUNCTION__, __LINE__, x, y, cu_width, cu_height, w >> 2, h >> 2);
				//for (int i = 0; i < cp_num; ++i) {
				//	printf("(%d %d)", ac_mv[i][MV_X], ac_mv[i][MV_Y]);
				//}
				//printf(") gmv=(%d %d)\n", (qpel_gmv_x >> 4), (qpel_gmv_y >> 4));
				return 0;
			}
			com_tbl_mc_l_hp(mv_scale_tmp_hor_ori, mv_scale_tmp_ver_ori, pic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_y,
				cu_width, pred_y + w, sub_w, sub_h, bit_depth);
		}
		pred_y += cu_width * sub_h;
	}
	return 1;
}

static int get_affine_mv_bits(CPMV mv[4][2], CPMV mvp[4][2], U5 num_refp, U5 refi, U2 vertex_num, u8 curr_mvr)
{
	int bits = 0;
	U2 vertex;
	int mvd_x;
	int mvd_y;
	U3 amvr_shift;

	for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
		mvd_x = mv[vertex][0] - mvp[vertex][0];
		mvd_y = mv[vertex][1] - mvp[vertex][1];
		amvr_shift = ((curr_mvr) == 0 ? 2 : (((curr_mvr) == 1 ? 4 : 0)));
		// note: if clipped to MAX value, the mv/mvp might not align with amvr shift
#if MD_PRINT
		if (mv[vertex][0] != ((1 << (18 - 1)) - 1) && mvp[vertex][0] != ((1 << (18 - 1)) - 1)) {
			mvd_x == mvd_x >> amvr_shift << amvr_shift ? (void)0 :
				__assert_HLS("mvd_x == ((mvd_x >> amvr_shift) << amvr_shift)", "../separate/enc_pinter.c", 1804,
				__FUNCTION__);
		}
		if (mv[vertex][1] != (((1 << (18 - 1)) - 1)) && mvp[vertex][1] != (((1 << (18 - 1)) - 1))) {
			mvd_y == mvd_y >> amvr_shift << amvr_shift ? (void)0 :
				__assert_HLS("mvd_y == ((mvd_y >> amvr_shift) << amvr_shift)", "../separate/enc_pinter.c", 1806,
				__FUNCTION__);
		}
#endif
		mvd_x >>= (amvr_shift);
		mvd_y >>= (amvr_shift);
		bits += (mvd_x > 2048
			|| mvd_x <= -2048 ? get_exp_golomb_bits(abs(mvd_x)) : (get_enc_tbl_mv_bits(mvd_x)));
		bits += (mvd_y > 2048
			|| mvd_y <= -2048 ? get_exp_golomb_bits(abs(mvd_y)) : (get_enc_tbl_mv_bits(mvd_y)));
	}
	bits += (get_enc_tbl_refi_bits(num_refp, refi));
	return bits;
}

static void scaled_horizontal_sobel_filter_re(pel pred[(1 << 7 >> 1) * (1 << 7 >> 1)], U7 pred_stride,
	int derivate[(1 << 7 >> 1) * (1 << 7 >> 1)], U7 derivate_buf_stride, U7 width, U7 height)
{
	U7 j;
	U7 k;
	U13 center;
	for (j = 1; j < height - 1; j++) {

#pragma HLS loop_tripcount max = 2
		for (k = 1; k < width - 1; k++) {

#pragma HLS loop_tripcount max = 2
			center = j * pred_stride + k;
			derivate[j * derivate_buf_stride + k] = pred[center + 1 - pred_stride] - pred[center - 1 - pred_stride]
				+ pred[center + 1] * 2 - pred[center - 1] * 2 + pred[center + 1 + pred_stride] - 
				pred[center - 1 + pred_stride];
		}
		derivate[j * derivate_buf_stride] = derivate[j * derivate_buf_stride + 1];
		derivate[j * derivate_buf_stride + width - 1] = derivate[j * derivate_buf_stride + width - 2];
	}
	derivate[0] = derivate[derivate_buf_stride + 1];
	derivate[width - 1] = derivate[derivate_buf_stride + width - 2];
	derivate[(height - 1) * derivate_buf_stride] = derivate[(height - 2) * derivate_buf_stride + 1];
	derivate[(height - 1) * derivate_buf_stride + width - 1] = derivate[(height - 2) * derivate_buf_stride + (width - 2)];
	for (j = 1; j < width - 1; j++) {

#pragma HLS loop_tripcount max = 2
		derivate[j] = derivate[derivate_buf_stride + j];
		derivate[(height - 1) * derivate_buf_stride + j] = derivate[(height - 2) * derivate_buf_stride + j];
	}
}

static void scaled_vertical_sobel_filter_re(pel pred[(1 << 7 >> 1) * (1 << 7 >> 1)], U7 pred_stride,
	int derivate[(1 << 7 >> 1) * (1 << 7 >> 1)], U7 derivate_buf_stride, U7 width, U7 height)
{
	U7 k;
	U7 j;
	U13 center;
	for (k = 1; k < width - 1; k++) {

#pragma HLS loop_tripcount max = 2
		for (j = 1; j < height - 1; j++) {

#pragma HLS loop_tripcount max = 2
			center = j * pred_stride + k;
			derivate[j * derivate_buf_stride + k] = pred[center + pred_stride - 1] - pred[center - pred_stride - 1]
				+ pred[center + pred_stride] * 2 - pred[center - pred_stride] * 2 + 
				pred[center + pred_stride + 1] - pred[center - pred_stride + 1];
		}
		derivate[k] = derivate[derivate_buf_stride + k];
		derivate[(height - 1) * derivate_buf_stride + k] = derivate[(height - 2) * derivate_buf_stride + k];
	}
	derivate[0] = derivate[derivate_buf_stride + 1];
	derivate[width - 1] = derivate[derivate_buf_stride + width - 2];
	derivate[(height - 1) * derivate_buf_stride] = derivate[(height - 2) * derivate_buf_stride + 1];
	derivate[(height - 1) * derivate_buf_stride + width - 1] = derivate[(height - 2) * derivate_buf_stride + (width - 2)];
	for (j = 1; j < height - 1; j++) {

#pragma HLS loop_tripcount max = 2
		derivate[j * derivate_buf_stride] = derivate[j * derivate_buf_stride + 1];
		derivate[j * derivate_buf_stride + width - 1] = derivate[j * derivate_buf_stride + width - 2];
	}
}

static void equal_coeff_computer_re(pel residue[(1 << 7 >> 1) * (1 << 7 >> 1)],
	int derivate[2][(1 << 7 >> 1) * (1 << 7 >> 1)], U7 derivate_buf_stride, s64 equal_coeff[7][7], U7 width, U7 height,
	U3 vertex_num)
{
	U4 affine_param_num = vertex_num << 1;
	U7 j;
	U7 k;
	U4 col;
	U4 row;
	s64 intermediates[2];
	int iC[6];
	U13 iIdx;
	for (j = 0; j != height; j++) {

#pragma HLS loop_tripcount max = 4
		for (k = 0; k != width; k++) {

#pragma HLS loop_tripcount max = 4

			iIdx = j * derivate_buf_stride + k;
			if (vertex_num == 2) {
				iC[0] = derivate[0][iIdx];
				iC[1] = k * derivate[0][iIdx];
				iC[1] += j * derivate[1][iIdx];
				iC[2] = derivate[1][iIdx];
				iC[3] = j * derivate[0][iIdx];
				iC[3] -= k * derivate[1][iIdx];
			}
			else {
				iC[0] = derivate[0][iIdx];
				iC[1] = k * derivate[0][iIdx];
				iC[2] = derivate[1][iIdx];
				iC[3] = k * derivate[1][iIdx];
				iC[4] = j * derivate[0][iIdx];
				iC[5] = j * derivate[1][iIdx];
			}
			for (col = 0; col < affine_param_num; col++) {

#pragma HLS loop_tripcount max = 5
				intermediates[0] = iC[col];
				for (row = 0; row < affine_param_num; row++) {

#pragma HLS loop_tripcount max = 5
					intermediates[1] = intermediates[0] * (iC[row]);
					equal_coeff[col + 1][row] += intermediates[1];
				}
				intermediates[1] = intermediates[0] * (residue[iIdx]);
				equal_coeff[col + 1][affine_param_num] += intermediates[1] * (8);
			}
		}
	}
}

static void solve_equal(s64 equal_coeff[7][7], U3 order, s64 affine_para_fixed_s64[6])
{
	U4 m;
	U4 n;
	s64 equal_coeff_fixed_s64[7][7];
	int i;
	int j;
	int k;
	s64 mul_fixed_s64 = 0;
	s64 div_fixed_s64 = 0;
	s64 temp = 0;
	U3 temp_idx;

	for (m = 0; m <= 6; m++) {
		for (n = 0; n <= 6; n++) {
			equal_coeff_fixed_s64[m][n] = equal_coeff[m][n] / (256);
			//equal_coeff_fixed_s64[m][n] = (s64)(equal_coeff[m][n]);
		}
	}
	//for (m = 0; m <= 6; m++)
	//{
	//	for (n = 0; n <= 6; n++)
	//	{
	//		printf("equal_coeff_fixed_s64[%d][%d] :%d\n", m, n, equal_coeff_fixed_s64[m][n]);
	//	}
	//}

	// row echelon
	for (i = 1; i < order; i++) {

#pragma HLS loop_tripcount max = 4
		// find column avs3_max
		temp = abs((int)equal_coeff_fixed_s64[i][i - 1]);
		temp_idx = i;
		for (j = i + 1; j < order + 1; j++) {

#pragma HLS loop_tripcount max = 4
			if ((abs(equal_coeff_fixed_s64[j][i - 1])) > temp) {
				temp = abs(equal_coeff_fixed_s64[j][i - 1]);
				temp_idx = j;
			}
		}
		// swap line
		if (temp_idx != i) {
			for (j = 0; j < order + 1; j++) {

#pragma HLS loop_tripcount max = 5
				equal_coeff_fixed_s64[0][j] = equal_coeff_fixed_s64[i][j];
				equal_coeff_fixed_s64[i][j] = equal_coeff_fixed_s64[temp_idx][j];
				equal_coeff_fixed_s64[temp_idx][j] = equal_coeff_fixed_s64[0][j];
			}
		}
		// elimination first column
		mul_fixed_s64 = 0;
		div_fixed_s64 = 0;

		for (j = i + 1; j < order + 1; j++) {

#pragma HLS loop_tripcount max = 4
			for (k = i; k < order + 1; k++) {

#pragma HLS loop_tripcount max = 4

				mul_fixed_s64 = equal_coeff_fixed_s64[i][k] * equal_coeff_fixed_s64[j][i - 1];

				if (equal_coeff_fixed_s64[i][i - 1] != (0)) {

					div_fixed_s64 = mul_fixed_s64 / equal_coeff_fixed_s64[i][i - 1];
				}
				else {

					div_fixed_s64 = mul_fixed_s64;
				}

				equal_coeff_fixed_s64[j][k] = equal_coeff_fixed_s64[j][k] - div_fixed_s64;

			}
		}
	}

	if (equal_coeff_fixed_s64[order][order - 1] != (0)) {
		affine_para_fixed_s64[order - 1] = equal_coeff_fixed_s64[order][order] * (10000) /
			equal_coeff_fixed_s64[order][order - 1];
	}
	else {

		affine_para_fixed_s64[order - 1] = equal_coeff_fixed_s64[order][order] * (10000);
	}

	for (i = order - 2; i >= 0; i--) {

#pragma HLS loop_tripcount max = 5
		temp = 0;
		for (j = i + 1; j < order; j++) {

#pragma HLS loop_tripcount max = 5
			temp += equal_coeff_fixed_s64[i + 1][j] * affine_para_fixed_s64[j];
		}
		if (equal_coeff_fixed_s64[i + 1][i] != (0)) {
			affine_para_fixed_s64[i] = (equal_coeff_fixed_s64[i + 1][order] * (10000) - temp) / equal_coeff_fixed_s64[i
				+ 1][i];
		}
		else {

			affine_para_fixed_s64[i] = equal_coeff_fixed_s64[i + 1][order] * (10000) - temp;
		}

	}
}

//static int com_affine_mc_l_fetch_adjust(CPMV mv_range[2][2], s32 cp_mv_four[4][2],
//	strFetch_ref_window *Fetch_Ref_window_ptr, int x, int y, int pic_w, int pic_h, int cu_width, int cu_height,
//	CPMV ac_mv[4][2], pel pred[(1 << 7 >> 1) * (1 << 7 >> 1)], int cp_num, int sub_w, int sub_h, int bit_depth)
//{
//#if MD_PRINT
//	(get_com_tbl_log2(cu_width)) >= 4 ? ((void)(0)) : __assert_HLS("get_com_tbl_log2(cu_width) >= 4",
//		"../separate/com_mc.c", 3688, __FUNCTION__);
//	(get_com_tbl_log2(cu_height)) >= 4 ? ((void)(0)) : __assert_HLS("get_com_tbl_log2(cu_height) >= 4",
//		"../separate/com_mc.c", 3689, __FUNCTION__);
//#endif
//	int result = 1;
//	int qpel_gmv_x;
//	int qpel_gmv_y;
//	int qpel_gmv_x_in_sw = 0;
//	int qpel_gmv_y_in_sw = 0;
//	pel *pred_y = pred;
//	int w;
//	int h;
//	int half_w;
//	int half_h;
//	s32 dmv_hor_x;
//	s32 dmv_ver_x;
//	s32 dmv_hor_y;
//	s32 dmv_ver_y;
//	s32 mv_scale_hor = ac_mv[0][0] << 7;
//	s32 mv_scale_ver = ac_mv[0][1] << 7;
//	CPMV mv_scale_tmp_hor;
//	CPMV mv_scale_tmp_ver;
//	s8 cu_width_log2 = get_com_tbl_log2(cu_width);
//	s8 cu_height_log2 = get_com_tbl_log2(cu_height);
//	s32 hor_max;
//	s32 hor_min;
//	s32 ver_max;
//	s32 ver_min;
//	s32 mv_scale_tmp_hor_ori;
//	s32 mv_scale_tmp_ver_ori;
//	U8 *pic_y = Fetch_Ref_window_ptr->RefWindowLCU;
//	int stride_y = SWW;
//	int limit_qpos_paterren = 0;
//	int interp_offset = 4;
//	int i;
//	for (i = 0; i < cp_num; i++) {
//
//#pragma HLS loop_tripcount max = 2
//#if MD_PRINT
//		ac_mv[i][0] >= (-(1 << (18 - 1))) && ac_mv[i][0] <= ((1 << (18 - 1)) - 1) ? ((void)(
//			0)) : __assert_HLS("ac_mv[i][0] >= (-(1<<(18 - 1))) && ac_mv[i][0] <= ((1<<(18 - 1)) - 1)",
//			"../separate/com_mc.c", 3713, __FUNCTION__);
//		ac_mv[i][1] >= (-(1 << (18 - 1))) && ac_mv[i][1] <= ((1 << (18 - 1)) - 1) ? ((void)(
//			0)) : __assert_HLS("ac_mv[i][1] >= (-(1<<(18 - 1))) && ac_mv[i][1] <= ((1<<(18 - 1)) - 1)",
//			"../separate/com_mc.c", 3714, __FUNCTION__);
//#endif
//	}
//	hor_max = (pic_w + (1 << 7) + 4 - x - cu_width + 1) << 4;
//	ver_max = (pic_h + (1 << 7) + 4 - y - cu_height + 1) << 4;
//	hor_min = (-(1 << 7) - 4 - x) << 4;
//	ver_min = (-(1 << 7) - 4 - y) << 4;
//	half_w = sub_w >> 1;
//	half_h = sub_h >> 1;
//	// deltaMvHor
//	dmv_hor_x = (ac_mv[1][0] - ac_mv[0][0]) << 7 >> cu_width_log2;
//	dmv_hor_y = (ac_mv[1][1] - ac_mv[0][1]) << 7 >> cu_width_log2;
//	if (cp_num == 3) {
//		// deltaMvVer
//		dmv_ver_x = (ac_mv[2][0] - ac_mv[0][0]) << 7 >> cu_height_log2;
//		dmv_ver_y = (ac_mv[2][1] - ac_mv[0][1]) << 7 >> cu_height_log2;
//	}
//	else {
//		// deltaMvVer
//		dmv_ver_x = -dmv_hor_y;
//		dmv_ver_y = dmv_hor_x;
//	}
//	cp_mv_four[0][0] = mv_scale_hor;
//	cp_mv_four[0][1] = mv_scale_ver;
//	cp_mv_four[1][0] = mv_scale_hor + (dmv_hor_x << cu_width_log2);
//	cp_mv_four[1][1] = mv_scale_ver + (dmv_hor_y << cu_width_log2);
//	cp_mv_four[2][0] = mv_scale_hor - dmv_hor_y * (cu_height - half_h);
//	cp_mv_four[2][1] = mv_scale_ver + dmv_hor_x * (cu_height - half_h);
//	cp_mv_four[3][0] = mv_scale_hor + dmv_hor_x * (cu_width - half_w) - dmv_hor_y * (cu_height - half_h);
//	cp_mv_four[3][1] = mv_scale_ver + dmv_hor_y * (cu_width - half_w) + dmv_hor_x * (cu_height - half_h);
//	mv_range[0][0] = (cp_mv_four[0][0] < ((cp_mv_four[1][0] < ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] :
//		cp_mv_four[3][0])) ? cp_mv_four[1][0] : ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))))
//		? cp_mv_four[0][0] : ((cp_mv_four[1][0] < ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))
//		? cp_mv_four[1][0] : ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0])))));
//	mv_range[1][0] = (cp_mv_four[0][0] > ((cp_mv_four[1][0] > ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] :
//		cp_mv_four[3][0])) ? cp_mv_four[1][0] : ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))))
//		? cp_mv_four[0][0] : ((cp_mv_four[1][0] > ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))
//		? cp_mv_four[1][0] : ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0])))));
//	mv_range[0][1] = (cp_mv_four[0][1] < ((cp_mv_four[1][1] < ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] :
//		cp_mv_four[3][1])) ? cp_mv_four[1][1] : ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))))
//		? cp_mv_four[0][1] : ((cp_mv_four[1][1] < ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))
//		? cp_mv_four[1][1] : ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1])))));
//	mv_range[1][1] = (cp_mv_four[0][1] > ((cp_mv_four[1][1] > ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] :
//		cp_mv_four[3][1])) ? cp_mv_four[1][1] : ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))))
//		? cp_mv_four[0][1] : ((cp_mv_four[1][1] > ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))
//		? cp_mv_four[1][1] : ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1])))));
//	if ((dmv_hor_x >= 0 && dmv_hor_y >= 0) || (dmv_hor_x >= 0 && dmv_hor_y < 0)) {
//		limit_qpos_paterren = 0;
//	}
//	else {
//		limit_qpos_paterren = 1;
//	}
//	com_mv_rounding_s32(mv_range[0][0], mv_range[0][1], &mv_range[0][0], &mv_range[0][1], 7, 0);
//	com_mv_rounding_s32(mv_range[1][0], mv_range[1][1], &mv_range[1][0], &mv_range[1][1], 7, 0);
//	mv_range[0][0] = ((-131072) > ((131071 < mv_range[0][0] ? (S32)131071 : (S32)mv_range[0][0])) ? (S32)(-131072) : (S32)((131071 < mv_range[0][0] ? (S32)131071 : (S32)mv_range[0][0])));
//	mv_range[1][0] = ((-131072) > ((131071 < mv_range[1][0] ? (S32)131071 : (S32)mv_range[1][0])) ? (S32)(-131072) : (S32)((131071 < mv_range[1][0] ? (S32)131071 : (S32)mv_range[1][0])));
//	mv_range[0][1] = ((-131072) > ((131071 < mv_range[0][1] ? (S32)131071 : (S32)mv_range[0][1])) ? (S32)(-131072) : (S32)((131071 < mv_range[0][1] ? (S32)131071 : (S32)mv_range[0][1])));
//	mv_range[1][1] = ((-131072) > ((131071 < mv_range[1][1] ? (S32)131071 : (S32)mv_range[1][1])) ? (S32)(-131072) : (S32)((131071 < mv_range[1][1] ? (S32)131071 : (S32)mv_range[1][1])));
//	if (limit_qpos_paterren) {
//		mv_range[0][0] = mv_range[0][0] + ((x + cu_width - sub_w) << 4);
//		mv_range[1][0] = mv_range[1][0] + ((x + 0) << 4);
//		mv_range[0][1] = mv_range[0][1] + ((y + cu_height - sub_h) << 4);
//		mv_range[1][1] = mv_range[1][1] + ((y + 0) << 4);
//	}
//	else {
//		mv_range[0][0] = mv_range[0][0] + ((x + 0) << 4);
//		mv_range[1][0] = mv_range[1][0] + ((x + cu_width - sub_w) << 4);
//		mv_range[0][1] = mv_range[0][1] + ((y + 0) << 4);
//		mv_range[1][1] = mv_range[1][1] + ((y + cu_height - sub_h) << 4);
//	}
//	if (mv_range[0][0] - (interp_offset << 4) < (Fetch_Ref_window_ptr->CtrPos[0][0][0]) << 4) {
//		result = 0;
//	}
//	if (mv_range[0][1] - (interp_offset << 4) < (Fetch_Ref_window_ptr->CtrPos[0][1][0]) << 4) {
//		result = 0;
//	}
//	if (mv_range[1][0] + ((sub_w + interp_offset) << 4) > (Fetch_Ref_window_ptr->CtrPos[1][0][0]) << 4) {
//		result = 0;
//	}
//	if (mv_range[1][1] + ((sub_h + interp_offset) << 4) > (Fetch_Ref_window_ptr->CtrPos[1][1][0]) << 4) {
//		result = 0;
//	}
//	if (result > 0) {
//		for (h = 0; h < cu_height; h += sub_h) {
//
//#pragma HLS loop_tripcount max = 16
//			for (w = 0; w < cu_width; w += sub_w) {
//
//#pragma HLS loop_tripcount max = 16
//				int pos_x = w + half_w;
//				int pos_y = h + half_h;
//				if (w == 0 && h == 0) {
//					pos_x = 0;
//					pos_y = 0;
//				}
//				else if (w + sub_w == cu_width && h == 0) {
//					pos_x = cu_width;
//					pos_y = 0;
//				}
//				else if (w == 0 && h + sub_h == cu_height && cp_num == 3) {
//					pos_x = 0;
//					pos_y = cu_height;
//				}
//				mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
//				mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;
//				com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
//				mv_scale_tmp_hor = ((-131072) > ((131071 < mv_scale_tmp_hor ? (CPMV)131071 : mv_scale_tmp_hor)) ? (CPMV)(-131072) : (CPMV)((131071 < mv_scale_tmp_hor ? (CPMV)131071 : mv_scale_tmp_hor)));
//				mv_scale_tmp_ver = ((-131072) > ((131071 < mv_scale_tmp_ver ? (CPMV)131071 : mv_scale_tmp_ver)) ? (CPMV)(-131072) : (CPMV)((131071 < mv_scale_tmp_ver ? (CPMV)131071 : mv_scale_tmp_ver)));
//				mv_scale_tmp_hor_ori = mv_scale_tmp_hor;
//				mv_scale_tmp_ver_ori = mv_scale_tmp_ver;
//				mv_scale_tmp_hor = (hor_max < ((hor_min > mv_scale_tmp_hor ? (CPMV)hor_min : mv_scale_tmp_hor)) ? (CPMV)hor_max : ((CPMV)(hor_min > mv_scale_tmp_hor ? (CPMV)hor_min : mv_scale_tmp_hor)));
//				mv_scale_tmp_ver = (ver_max < ((ver_min > mv_scale_tmp_ver ? (CPMV)ver_min : mv_scale_tmp_ver)) ? (CPMV)ver_max : ((CPMV)(ver_min > mv_scale_tmp_ver ? (CPMV)ver_min : mv_scale_tmp_ver)));
//				qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
//				qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;
//				qpel_gmv_x_in_sw = qpel_gmv_x - (((int)Fetch_Ref_window_ptr->CtrPos[0][0][0]) << 4) + (((
//					int)Fetch_Ref_window_ptr->CtrPos[0][0][1]) << 4);
//				qpel_gmv_y_in_sw = qpel_gmv_y - (((int)Fetch_Ref_window_ptr->CtrPos[0][1][0]) << 4) + (((
//					int)Fetch_Ref_window_ptr->CtrPos[0][1][1]) << 4);
//				if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, Fetch_Ref_window_ptr->CtrPos, sub_w, sub_h, Fetch_Ref_window_ptr->x_chroma, Fetch_Ref_window_ptr->y_chroma, x, y, 0, 4)) {
//#if MD_PRINT
//					printf("%s:%d: Affine motion is out of ref window!\npos=(%d %d) size=(%d %d) scu=(%d %d) ac_mv=(", __FUNCTION__, __LINE__, x, y, cu_width, cu_height, w >> 2, h >> 2);
//					for (int i = 0; i < cp_num; ++i) {
//						printf("(%d %d)", ac_mv[i][MV_X], ac_mv[i][MV_Y]);
//					}
//					printf(") gmv=(%d %d)\n", (qpel_gmv_x >> 4), (qpel_gmv_y >> 4));
//#endif
//					return 0;
//				}
//				com_tbl_mc_l_hp(mv_scale_tmp_hor_ori, mv_scale_tmp_ver_ori, pic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_y,
//						cu_width, pred_y + w, sub_w, sub_h, bit_depth);
//			}
//			pred_y += cu_width * sub_h;
//		}
//	}
//	return result;
//}

static int com_affine_mc_l_fetch_adjust_debug(s32 mv_range[2][2], s32 cp_mv_four[4][2],
	pel RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U13 x, U13 y, U13 pic_w, U13 pic_h, U7 cu_width, U7 cu_height,
	CPMV ac_mv[4][2], pel pred[(1 << 7 >> 1) * (1 << 7 >> 1)], U2 cp_num, U4 sub_w, U4 sub_h, U4 bit_depth, U12 x_chroma, U12 y_chroma)
{
	s8 cu_width_log2 = get_com_tbl_log2(cu_width);
	s8 cu_height_log2 = get_com_tbl_log2(cu_height);
#if MD_PRINT
	cu_width_log2 >= 4 ? ((void)(0)) : __assert_HLS("get_com_tbl_log2(cu_width) >= 4",
		"../separate/com_mc.c", 3688, __FUNCTION__);
	cu_height_log2 >= 4 ? ((void)(0)) : __assert_HLS("get_com_tbl_log2(cu_height) >= 4",
		"../separate/com_mc.c", 3689, __FUNCTION__);
#endif
	int result = 1;
	int qpel_gmv_x;
	int qpel_gmv_y;
	int qpel_gmv_x_in_sw = 0;
	int qpel_gmv_y_in_sw = 0;
	pel *pred_y = pred;
	int w;
	int h;
	int half_w;
	int half_h;
	s32 dmv_hor_x;
	s32 dmv_ver_x;
	s32 dmv_hor_y;
	s32 dmv_ver_y;
	s32 mv_scale_hor = ac_mv[0][0] << 7;
	s32 mv_scale_ver = ac_mv[0][1] << 7;
	s32 mv_scale_tmp_hor;
	s32 mv_scale_tmp_ver;
	s32 hor_max;
	s32 hor_min;
	s32 ver_max;
	s32 ver_min;
	s32 mv_scale_tmp_hor_ori;
	s32 mv_scale_tmp_ver_ori;
	pel *pic_y = RefWindowLCU;
	int stride_y = SWW;
	int limit_qpos_paterren = 0;
	int interp_offset = 4;
	int i;
	for (i = 0; i < cp_num; i++) {

#pragma HLS loop_tripcount max = 2
#if MD_PRINT
		ac_mv[i][0] >= (-(1 << (18 - 1))) && ac_mv[i][0] <= ((1 << (18 - 1)) - 1) ? ((void)(
			0)) : __assert_HLS("ac_mv[i][0] >= (-(1<<(18 - 1))) && ac_mv[i][0] <= ((1<<(18 - 1)) - 1)",
			"../separate/com_mc.c", 3713, __FUNCTION__);
		ac_mv[i][1] >= (-(1 << (18 - 1))) && ac_mv[i][1] <= ((1 << (18 - 1)) - 1) ? ((void)(
			0)) : __assert_HLS("ac_mv[i][1] >= (-(1<<(18 - 1))) && ac_mv[i][1] <= ((1<<(18 - 1)) - 1)",
			"../separate/com_mc.c", 3714, __FUNCTION__);
#endif
	}
	hor_max = (pic_w + (1 << 7) + 4 - x - cu_width + 1) << 4;
	ver_max = (pic_h + (1 << 7) + 4 - y - cu_height + 1) << 4;
	hor_min = (-(1 << 7) - 4 - x) << 4;
	ver_min = (-(1 << 7) - 4 - y) << 4;
	half_w = sub_w >> 1;
	half_h = sub_h >> 1;
	// deltaMvHor
	dmv_hor_x = (ac_mv[1][0] - ac_mv[0][0]) << 7 >> cu_width_log2;
	dmv_hor_y = (ac_mv[1][1] - ac_mv[0][1]) << 7 >> cu_width_log2;
	if (cp_num == 3) {
		// deltaMvVer
		dmv_ver_x = (ac_mv[2][0] - ac_mv[0][0]) << 7 >> cu_height_log2;
		dmv_ver_y = (ac_mv[2][1] - ac_mv[0][1]) << 7 >> cu_height_log2;
	}
	else {
		// deltaMvVer
		dmv_ver_x = -dmv_hor_y;
		dmv_ver_y = dmv_hor_x;
	}
	cp_mv_four[0][0] = mv_scale_hor;
	cp_mv_four[0][1] = mv_scale_ver;
	cp_mv_four[1][0] = mv_scale_hor + (dmv_hor_x << cu_width_log2);
	cp_mv_four[1][1] = mv_scale_ver + (dmv_hor_y << cu_width_log2);
	cp_mv_four[2][0] = mv_scale_hor - dmv_hor_y * (cu_height - half_h);
	cp_mv_four[2][1] = mv_scale_ver + dmv_hor_x * (cu_height - half_h);
	cp_mv_four[3][0] = mv_scale_hor + dmv_hor_x * (cu_width - half_w) - dmv_hor_y * (cu_height - half_h);
	cp_mv_four[3][1] = mv_scale_ver + dmv_hor_y * (cu_width - half_w) + dmv_hor_x * (cu_height - half_h);
	mv_range[0][0] = (cp_mv_four[0][0] < ((cp_mv_four[1][0] < ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] :
		cp_mv_four[3][0])) ? cp_mv_four[1][0] : ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))))
		? cp_mv_four[0][0] : ((cp_mv_four[1][0] < ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))
		? cp_mv_four[1][0] : ((cp_mv_four[2][0] < cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0])))));
	mv_range[1][0] = (cp_mv_four[0][0] > ((cp_mv_four[1][0] > ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] :
		cp_mv_four[3][0])) ? cp_mv_four[1][0] : ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))))
		? cp_mv_four[0][0] : ((cp_mv_four[1][0] > ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0]))
		? cp_mv_four[1][0] : ((cp_mv_four[2][0] > cp_mv_four[3][0] ? cp_mv_four[2][0] : cp_mv_four[3][0])))));
	mv_range[0][1] = (cp_mv_four[0][1] < ((cp_mv_four[1][1] < ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] :
		cp_mv_four[3][1])) ? cp_mv_four[1][1] : ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))))
		? cp_mv_four[0][1] : ((cp_mv_four[1][1] < ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))
		? cp_mv_four[1][1] : ((cp_mv_four[2][1] < cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1])))));
	mv_range[1][1] = (cp_mv_four[0][1] > ((cp_mv_four[1][1] > ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] :
		cp_mv_four[3][1])) ? cp_mv_four[1][1] : ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))))
		? cp_mv_four[0][1] : ((cp_mv_four[1][1] > ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1]))
		? cp_mv_four[1][1] : ((cp_mv_four[2][1] > cp_mv_four[3][1] ? cp_mv_four[2][1] : cp_mv_four[3][1])))));
	if ((dmv_hor_x >= 0 && dmv_hor_y >= 0) || (dmv_hor_x >= 0 && dmv_hor_y < 0)) {
		limit_qpos_paterren = 0;
	}
	else {
		limit_qpos_paterren = 1;
	}
	com_mv_rounding_s32(mv_range[0][0], mv_range[0][1], &mv_range[0][0], &mv_range[0][1], 7, 0);
	com_mv_rounding_s32(mv_range[1][0], mv_range[1][1], &mv_range[1][0], &mv_range[1][1], 7, 0);
	mv_range[0][0] = ((-131072) >((131071 < mv_range[0][0] ? 131071 : mv_range[0][0])) ? (
		-131072) : ((131071 < mv_range[0][0] ? 131071 : mv_range[0][0])));
	mv_range[1][0] = ((-131072) > ((131071 < mv_range[1][0] ? 131071 : mv_range[1][0])) ? (
		-131072) : ((131071 < mv_range[1][0] ? 131071 : mv_range[1][0])));
	mv_range[0][1] = ((-131072) > ((131071 < mv_range[0][1] ? 131071 : mv_range[0][1])) ? (
		-131072) : ((131071 < mv_range[0][1] ? 131071 : mv_range[0][1])));
	mv_range[1][1] = ((-131072) > ((131071 < mv_range[1][1] ? 131071 : mv_range[1][1])) ? (
		-131072) : ((131071 < mv_range[1][1] ? 131071 : mv_range[1][1])));
	if (limit_qpos_paterren) {
		mv_range[0][0] = mv_range[0][0] + ((x + cu_width - sub_w) << 4);
		mv_range[1][0] = mv_range[1][0] + ((x + 0) << 4);
		mv_range[0][1] = mv_range[0][1] + ((y + cu_height - sub_h) << 4);
		mv_range[1][1] = mv_range[1][1] + ((y + 0) << 4);
	}
	else {
		mv_range[0][0] = mv_range[0][0] + ((x + 0) << 4);
		mv_range[1][0] = mv_range[1][0] + ((x + cu_width - sub_w) << 4);
		mv_range[0][1] = mv_range[0][1] + ((y + 0) << 4);
		mv_range[1][1] = mv_range[1][1] + ((y + cu_height - sub_h) << 4);
	}
	if (mv_range[0][0] - (interp_offset << 4) < (CtrPos[0][0][0]) << 4) {
		result = 0;
	}
	if (mv_range[0][1] - (interp_offset << 4) < (CtrPos[0][1][0]) << 4) {
		result = 0;
	}
	if (mv_range[1][0] + ((sub_w + interp_offset) << 4) > (CtrPos[1][0][0]) << 4) {
		result = 0;
	}
	if (mv_range[1][1] + ((sub_h + interp_offset) << 4) > (CtrPos[1][1][0]) << 4) {
		result = 0;
	}
	if (result > 0) {
		for (h = 0; h < cu_height; h += sub_h) {

#pragma HLS loop_tripcount max = 16
			//printf("gmv_in_sw: ");
			for (w = 0; w < cu_width; w += sub_w) {

#pragma HLS loop_tripcount max = 16
				int pos_x = w + half_w;
				int pos_y = h + half_h;
				if (w == 0 && h == 0) {
					pos_x = 0;
					pos_y = 0;
				}
				else if (w + sub_w == cu_width && h == 0) {
					pos_x = cu_width;
					pos_y = 0;
				}
				else if (w == 0 && h + sub_h == cu_height && cp_num == 3) {
					pos_x = 0;
					pos_y = cu_height;
				}
				mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
				mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;
				com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
				mv_scale_tmp_hor = ((-131072) > ((131071 < mv_scale_tmp_hor ? 131071 : mv_scale_tmp_hor)) ?
					(-131072) : ((131071 < mv_scale_tmp_hor ? 131071 : mv_scale_tmp_hor)));
				mv_scale_tmp_ver = ((-131072) > ((131071 < mv_scale_tmp_ver ? 131071 : mv_scale_tmp_ver)) ?
					(-131072) : ((131071 < mv_scale_tmp_ver ? 131071 : mv_scale_tmp_ver)));
				mv_scale_tmp_hor_ori = mv_scale_tmp_hor;
				mv_scale_tmp_ver_ori = mv_scale_tmp_ver;
				mv_scale_tmp_hor = (hor_max < ((hor_min > mv_scale_tmp_hor ? hor_min : mv_scale_tmp_hor)) ? hor_max : ((
					hor_min > mv_scale_tmp_hor ? hor_min : mv_scale_tmp_hor)));
				mv_scale_tmp_ver = (ver_max < ((ver_min > mv_scale_tmp_ver ? ver_min : mv_scale_tmp_ver)) ? ver_max : ((
					ver_min > mv_scale_tmp_ver ? ver_min : mv_scale_tmp_ver)));
				qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
				qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;
				qpel_gmv_x_in_sw = qpel_gmv_x - (((int)CtrPos[0][0][0]) << 4) + (((
					int)CtrPos[0][0][1]) << 4);
				qpel_gmv_y_in_sw = qpel_gmv_y - (((int)CtrPos[0][1][0]) << 4) + (((
					int)CtrPos[0][1][1]) << 4);
				//printf("(%d+%d, %d+%d) ", (qpel_gmv_x_in_sw >> 4), (qpel_gmv_x_in_sw & 0xf), (qpel_gmv_y_in_sw >> 4), (qpel_gmv_y_in_sw & 0xf));
				if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, CtrPos, sub_w, sub_h, x_chroma, y_chroma, x, y, 0, 4)) {
					//printf("%s:%d: Affine motion is out of ref window!\npos=(%d %d) size=(%d %d) scu=(%d %d) ac_mv=(", __FUNCTION__, __LINE__, x, y, cu_width, cu_height, w >> 2, h >> 2);
					//for (int i = 0; i < cp_num; ++i) {
					//	printf("(%d %d)", ac_mv[i][MV_X], ac_mv[i][MV_Y]);
					//}
					//printf(") gmv=(%d %d)\n", (qpel_gmv_x >> 4), (qpel_gmv_y >> 4));
					return 0;
				}
				com_tbl_mc_l_hp(mv_scale_tmp_hor_ori, mv_scale_tmp_ver_ori, pic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_y,
					cu_width, pred_y + w, sub_w, sub_h, bit_depth);
				//printf("pred_y+%d=\n", w);
				//for (int i = 0; i < 4; ++i){
				//	for (int j = 0; j < 4; ++j){
				//		printf("%d ", pred_y[w + cu_width * i + j]);
				//	}
				//	printf("\n");
				//}
			}
			pred_y += cu_width * sub_h;
			//printf("\n");
		}
	}
	return result;
}

static u32 pinter_affine_me_gradient_fetch_in_iter(pel RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], ENC_AFFME_IN *in, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH], U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, s8 refi, U1 lidx, CPMV mvp[4][2], CPMV mv[4][2], U1 bi, U3 vertex_num, U4 sub_w, U4 sub_h, U12 x_chroma, U12 y_chroma)
{
#pragma HLS INLINE
	U4 bit_depth = in->bit_depth;
	s8 ri = refi - 1;
	CPMV mvt[4][2];
	s16 mvd[4][2];
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	u32 cost;
	u32 cost_best = 0xFFFFFFFF;
	//COM_PIC* refp = in->refp[ri][lidx].pic;
	//pel *pred = in->pred_buf;
	int mv_bits;
	int best_bits;
	U3 vertex;
	U3 iter;
	//int iter_num = 7;
	U3 para_num = (vertex_num << 1) + 1;
	U3 affine_param_num = para_num - 1;
	s64 affine_para[6] = {};
	s64 delta_mv[6] = {};
	s64 equal_coeff_t[7][7] = { 0 };
	s64 equal_coeff[7][7] = { 0 };
	//pel    *error = in->p_error;
	//int    *derivate[2];
	//derivate[0] = in->i_gradient[0];
	//derivate[1] = in->i_gradient[1];
	U1 mc_mvf_valid = 1;
	U3 iter_num;
	if (cu_width_log2 == 5 && cu_height_log2==5)
	{
		iter_num = 1;
	}
	else
	{
		iter_num = 7;
	}
	for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
		mvt[vertex][0] = mv[vertex][0];
		mvt[vertex][1] = mv[vertex][1];
		mvd[vertex][0] = 0;
		mvd[vertex][1] = 0;
	}
	/* do motion compensation with start mv */
	for (int i = 0; i < MAX_CU_DIM; ++i){
		in->pred_buf[i] = 0;
	}
	U1 valid = com_affine_mc_l_fetch(RefWindowLCU, CtrPos, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height, mvt,
		in->pred_buf, vertex_num, sub_w, sub_h, bit_depth, x_chroma, y_chroma);
	if (!valid){
		return 0xFFFFFFFF;
	}

	/* get mvd bits*/
	best_bits = get_affine_mv_bits(mvt, mvp, in->num_refp, ri, vertex_num, in->curr_mvr);
	cost_best = ((in->lambda_mv * best_bits + (1 << 15)) >> 16);
	cost_best += calc_satd_16b(cu_width, cu_height, p_fenc_LCU_Y + in->org_offset[0], in->pred_buf,
		in->stride_org_y, cu_width) >> bi;
	if (vertex_num == 3) {
		iter_num = (bi ? 5 - 2 : 7 - 2);
	}
	for (iter = 0; iter < iter_num; iter++) {

#pragma HLS loop_tripcount max = 7
		//printf("iter %d: ", iter);
		U3 row;
		U3 col;
		U1 all_zero = 0;

		enc_diff_16b(cu_width_log2, cu_height_log2, p_fenc_LCU_Y + in->org_offset[0], in->pred_buf,
			in->stride_org_y, cu_width, cu_width, in->p_error);

		scaled_horizontal_sobel_filter_re(in->pred_buf, cu_width, in->i_gradient[0], cu_width, cu_width, cu_height);

		scaled_vertical_sobel_filter_re(in->pred_buf, cu_width, in->i_gradient[1], cu_width, cu_width, cu_height);

		for (row = 0; row < para_num; row++) {

#pragma HLS loop_tripcount max = 5
			for (col = 0; col < para_num; col++) {

#pragma HLS loop_tripcount max = 5
				equal_coeff_t[row][col] = 0;
			}
			//memset_re(&equal_coeff_t[row][0], 0, para_num * sizeof(s64));
		}
		equal_coeff_computer_re(in->p_error, in->i_gradient, cu_width, equal_coeff_t, cu_width, cu_height, vertex_num);
		for (row = 0; row < para_num; row++) {

#pragma HLS loop_tripcount max = 5
			for (col = 0; col < para_num; col++) {

#pragma HLS loop_tripcount max = 5
				equal_coeff[row][col] = equal_coeff_t[row][col];
			}
		}
		//printf("equal_coeff=(");
		//for (int i = 0; i < para_num; ++i){
		//	printf("(");
		//	for (int j = 0; j < para_num; ++j){
		//		print_s64(equal_coeff[i][j]);
		//		printf(" ");
		//	}
		//	printf(")\n");
		//}
		//printf(")\n");
		//printf("affine_param_num = ");
		//print_s64(affine_param_num);
		//printf("\n");
		solve_equal(equal_coeff, affine_param_num, affine_para);
		//printf("affine_para=(");
		//for (int i = 0; i < 6; ++i){
		//	print_s64(affine_para[i]);
		//	printf(" ");
		//}
		//printf(") ");

		{
			delta_mv[0] = affine_para[0];
			delta_mv[2] = affine_para[2];
			delta_mv[1] = affine_para[1] * (cu_width)+affine_para[0];
			delta_mv[3] = -affine_para[3] * (cu_width)+affine_para[2];
			//printf("delta_mv=(");
			//for (int i = 0; i < 6; ++i){
			//	print_s64(affine_para[i]);
			//	printf(" ");
			//}
			//printf(")\n");
			u8 amvr_shift = ((in->curr_mvr) == 0 ? 2 : (((in->curr_mvr) == 1 ? 4 : 0)));
			if (amvr_shift == 0) {
				mvd[0][0] = (((delta_mv[0] << 4) + (((delta_mv[0] >= (
					0) ? 5000 : -5000)))) / (10000));
				mvd[0][1] = (((delta_mv[2] << 4) + (((delta_mv[2] >= (
					0) ? 5000 : -5000)))) / (10000));
				mvd[1][0] = (((delta_mv[1] << 4) + (((delta_mv[1] >= (
					0) ? 5000 : -5000)))) / (10000));
				mvd[1][1] = (((delta_mv[3] << 4) + (((delta_mv[3] >= (
					0) ? 5000 : -5000)))) / (10000));
			}
			else {
				mvd[0][0] = (((delta_mv[0] << 2) + (((delta_mv[0] >= (
					0) ? 5000 : -5000)))) / (10000));
				mvd[0][1] = (((delta_mv[2] << 2) + (((delta_mv[2] >= (
					0) ? 5000 : -5000)))) / (10000));
				mvd[1][0] = (((delta_mv[1] << 2) + (((delta_mv[1] >= (
					0) ? 5000 : -5000)))) / (10000));
				mvd[1][1] = (((delta_mv[3] << 2) + (((delta_mv[3] >= (
					0) ? 5000 : -5000)))) / (10000));
				//  1/16-pixel
				mvd[0][0] <<= 2;
				mvd[0][1] <<= 2;
				mvd[1][0] <<= 2;
				mvd[1][1] <<= 2;
				if (((int)amvr_shift) > 0) {
					com_mv_rounding_s16(mvd[0][0], mvd[0][1], &mvd[0][0], &mvd[0][1], (int)amvr_shift, (int)amvr_shift);
					com_mv_rounding_s16(mvd[1][0], mvd[1][1], &mvd[1][0], &mvd[1][1], (int)amvr_shift, (int)amvr_shift);
				}
			}
		}
		//printf("mvd=(%d %d %d %d)\n", mvd[0][MV_X], mvd[0][MV_Y], mvd[1][MV_X], mvd[1][MV_Y]);
		for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
			if ((mvd[vertex][0]) != 0 || (mvd[vertex][1]) != 0) {
				all_zero = 0;
				break;
			}
			all_zero = 1;
		}
		if (all_zero) {
			break;
		}
		for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
			s32 mvx = mvt[vertex][0] + mvd[vertex][0];
			s32 mvy = mvt[vertex][1] + mvd[vertex][1];
			mvt[vertex][0] = (((-(1 << (18 - 1))) >((((1 << (18 - 1)) - 1) < mvx ? ((
				1 << (18 - 1)) - 1) : mvx)) ? (-(1 << (18 - 1))) : ((((1 << (18 - 1)) - 1) < mvx ? ((
				1 << (18 - 1)) - 1) : mvx))));
			mvt[vertex][1] = (((-(1 << (18 - 1))) > ((((1 << (18 - 1)) - 1) < mvy ? ((
				1 << (18 - 1)) - 1) : mvy)) ? (-(1 << (18 - 1))) : ((((1 << (18 - 1)) - 1) < mvy ? ((
				1 << (18 - 1)) - 1) : mvy))));
			u8 amvr_shift = (in->curr_mvr) == 0 ? 2 : (((in->curr_mvr) == 1 ? 4 : 0));
			if (mvt[vertex][0] == ((1 << (18 - 1)) - 1)) {
				mvt[vertex][0] = mvt[vertex][0] >> amvr_shift << amvr_shift;
			}
			if (mvt[vertex][1] == ((1 << (18 - 1)) - 1)) {
				mvt[vertex][1] = mvt[vertex][1] >> amvr_shift << amvr_shift;
			}
		}
		//printf("mvt=(");
		//for (int i = 0; i < vertex_num; ++i){
		//	printf("(%d %d) ", mvt[i][0], mvt[i][1]);
		//}
		//printf(")\n");
		pel  pred_buf[MAX_CU_DIM];
		mc_mvf_valid = com_affine_mc_l_fetch_adjust_debug(in->mvf_range, in->cp_mv_four, RefWindowLCU, CtrPos, x, y, in->pic_width_luma,
			in->pic_height_luma, cu_width, cu_height, mvt, pred_buf, vertex_num, sub_w, sub_h, bit_depth, x_chroma, y_chroma);
		if (mc_mvf_valid) {
			for (int i = 0; i < cu_height; ++i){
#pragma HLS LOOP_TRIPCOUNT max=64
				for (int j = 0; j < cu_width; ++j){
#pragma HLS LOOP_TRIPCOUNT max=64
					in->pred_buf[(i << cu_width_log2) + j] = pred_buf[(i << cu_width_log2) + j];
				}
			}
			//printf("pred2: ");
			//for (int i = 0; i < cu_height; ++i){
			//	printf("(");
			//	for (int j = 0; j < cu_width; ++j){
			//		printf("%d ", in->pred_buf[i * cu_width + j]);
			//	}
			//	printf(")\n");
			//}
			mv_bits = get_affine_mv_bits(mvt, mvp, (int)(in->num_refp), (int)ri, vertex_num, in->curr_mvr);
			/*if (bi)
			{
			mv_bits += mot_bits[1 - lidx];
			}*/
			cost = ((u32)((in->lambda_mv * ((unsigned int)mv_bits) + ((unsigned int)(1 << 15))) >> 16));
			cost += calc_satd_16b(cu_width, cu_height, p_fenc_LCU_Y + in->org_offset[0], in->pred_buf,
				in->stride_org_y, cu_width) >> bi;
			if (cost < cost_best) {
				cost_best = cost;
				best_bits = mv_bits;
				for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
					mv[vertex][0] = mvt[vertex][0];
					mv[vertex][1] = mvt[vertex][1];
				}
			}
		}
		else{ break; }
	}
	//exit(0);
	return cost_best - ((in->lambda_mv * best_bits + (1 << 15)) >> 16);
}

void affine_me_all_refi_fetch(MD_FW *md_fw_ptr, pel RefWindowLCU[SWW*SWH], 
#if ENABLE_BFRAME
	pel RefWindowLCU_ref1[SWW * SWH], MV_DIR mv_dir,
#endif
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], ENC_ME_LINE_MAP *me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	ENC_AFFME_IN *in, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH], MD_COM_MODE *mod_info_curr, s8 *refi_temp, int lidx,
	int best_mv_uni_inner[2][4][2])
{
#if ENABLE_BFRAME
	S8 refi_cur = 0;
#else
	U5 refi_cur = 0;
#endif
	
	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	u32 mecost;
	u32 best_mecost;
	int mebits;
	int best_bits = 0;
	U13 x = mod_info_curr->x_pos;
	U13 y = mod_info_curr->y_pos;
	U2 vertex = 0;
	U3 sub_w = 4;
	U3 sub_h = 4;
	U4 bit_depth = in->bit_depth;
	s8 t0;
	s8 t1;
	S3 *refi;
	U2 vertex_num = 2;
	U1 adjust_fine_mvp = 1;
	U1 adjust_fine_trans = 1;
	U3 startmv_idx = 0;
	u32 mvp_best = 0xFFFFFFFF;
	u32 mvp_temp = 0xFFFFFFFF;
	S3 refi_t[2];
	pel *pred;
	CPMV tmp_mvx;
	CPMV tmp_mvy;
	U1 mc_mvf_valid = 1;
	pel pred_buf[MAX_CU_DIM];
	U7 i;
	U7 j;
	refi = mod_info_curr->refi;
	best_mecost = 0xFFFFFFFF;
#if ENABLE_BFRAME
	pel* refpic_y;
	if (mv_dir.cur_refi == 0 && mv_dir.lidx == 0)
	{
		refpic_y = RefWindowLCU;
	}
	else
	{
		refpic_y = RefWindowLCU_ref1;
	}
	lidx = mv_dir.cur_refi;
	refi_cur = mv_dir.cur_refi;
#else
	for (refi_cur = 0; refi_cur < ((int)(in->num_refp)); refi_cur++)
#endif
	{

#pragma HLS loop_tripcount max = 1
		mod_info_curr->refi[lidx] = refi_cur;
		com_get_affine_mvp_scaling_line(md_fw_ptr, me_line_map_ptr, mod_info_curr, in->refp_ptr, in->ptr, lidx,
			in->affine_mvp_scale[lidx][refi_cur], in->curr_mvr, inner_nebs, above_nebs_line);
		//printf("affine_mvp_scale: (");
		//for (vertex = 0; vertex < vertex_num; vertex++) {
		//	printf("(%d %d) ", in->affine_mvp_scale[lidx][refi_cur][vertex][0], in->affine_mvp_scale[lidx][refi_cur][vertex][1]);
		//}
		//printf(") ");
		{
			mvp_best = 0xFFFFFFFF;
			mvp_temp = 0xFFFFFFFF;
			pred = in->pred_buf;
			for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
				in->temp_affine_mvp[vertex][0] = in->affine_mvp_scale[lidx][refi_cur][vertex][0];
				in->temp_affine_mvp[vertex][1] = in->affine_mvp_scale[lidx][refi_cur][vertex][1];
			}
			// get cost of mvp
#if ENABLE_BFRAME
			adjust_fine_mvp = adjust_cpmv_iter(refpic_y, CtrPos, in, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
				in->temp_affine_mvp, vertex_num, (int)(in->curr_mvr), sub_w, sub_h);
			if (adjust_fine_mvp) {
				mc_mvf_valid = com_affine_mc_l_fetch(refpic_y, CtrPos, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
					in->temp_affine_mvp, pred_buf, vertex_num, sub_w, sub_h, bit_depth, mod_info_curr->x_chroma, mod_info_curr->y_chroma);
#else
			adjust_fine_mvp = adjust_cpmv_iter(RefWindowLCU, CtrPos, in, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
				in->temp_affine_mvp, vertex_num, (int)(in->curr_mvr), sub_w, sub_h);
			if (adjust_fine_mvp) {
				mc_mvf_valid = com_affine_mc_l_fetch(RefWindowLCU, CtrPos, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
					in->temp_affine_mvp, pred_buf, vertex_num, sub_w, sub_h, bit_depth, mod_info_curr->x_chroma, mod_info_curr->y_chroma);
#endif

				//printf("fine mvp: (");
				//for (vertex = 0; vertex < vertex_num; vertex++) {
				//	printf("(%d %d) ", in->temp_affine_mvp[vertex][0], in->temp_affine_mvp[vertex][1]);
				//}
				//printf(") ");
				if (mc_mvf_valid)
				{
					for (i = 0; i < cu_height; i++)
#pragma HLS LOOP_TRIPCOUNT max = 64
					{
						for (j = 0; j < cu_width; j++)
#pragma HLS LOOP_TRIPCOUNT max = 64
						{
							pred[(i << cu_width_log2) + j] = pred_buf[(i << cu_width_log2) + j];
						}
					}
					//printf("pred=\n");
					//for (int i = 0; i < cu_height; ++i){
					//	for (int j = 0; j < cu_width; ++j){
					//		printf("%d ", pred[i * cu_width + j]);
					//	}
					//	printf("\n");
					//}
					mvp_best = calc_satd_16b(cu_width, cu_height, p_fenc_LCU_Y + in->org_offset[0], pred, in->stride_org_y,
						cu_width);
					mebits = get_affine_mv_bits(in->temp_affine_mvp, in->affine_mvp_scale[lidx][refi_cur], (int)(in->num_refp), refi_cur,
						vertex_num, in->curr_mvr);
					mvp_best += ((in->lambda_mv * mebits + (1 << 15)) >> 16);
					//printf("bits=%d cost=%d\n", mebits, mvp_best);
				}
				else
				{
					adjust_fine_mvp = 0;
				}
			}
			// get cost of translational mv
			//printf("best_mv_uni_inner: (%d %d)\n", best_mv_uni_inner[lidx][refi_cur][0], best_mv_uni_inner[lidx][refi_cur][1]);
			in->mv_cliped[lidx][0] = best_mv_uni_inner[lidx][refi_cur][0];
			in->mv_cliped[lidx][1] = best_mv_uni_inner[lidx][refi_cur][1];
			refi_t[lidx] = refi_cur;
			refi_t[1 - lidx] = -1;
			mv_clip(x, y, md_fw_ptr->pic_width, md_fw_ptr->pic_height, cu_width, cu_height, refi_t, in->mv_cliped,
				in->mv_cliped);
			for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
				in->temp_trans_mv[vertex][0] = ((CPMV)in->mv_cliped[lidx][0]);
				in->temp_trans_mv[vertex][1] = ((CPMV)in->mv_cliped[lidx][1]);

				tmp_mvx = in->temp_trans_mv[vertex][0] << 2;
				tmp_mvy = in->temp_trans_mv[vertex][1] << 2;
				if (((int)(in->curr_mvr)) == 1) {
					com_mv_rounding_s32(tmp_mvx, tmp_mvy, &tmp_mvx, &tmp_mvy, 4, 4);
				}
				in->temp_trans_mv[vertex][0] = (((-(1 << (18 - 1))) > ((((1 << (18 - 1)) - 1) < tmp_mvx ? (CPMV)((
					1 << (18 - 1)) - 1) : tmp_mvx)) ? (CPMV)(-(1 << (18 - 1))) : (CPMV)((((1 << (18 - 1)) - 1) < tmp_mvx ? (CPMV)((
					1 << (18 - 1)) - 1) : tmp_mvx))));
				in->temp_trans_mv[vertex][1] = (((-(1 << (18 - 1))) > ((((1 << (18 - 1)) - 1) < tmp_mvy ? (CPMV)((
					1 << (18 - 1)) - 1) : tmp_mvy)) ? (CPMV)(-(1 << (18 - 1))) : (CPMV)((((1 << (18 - 1)) - 1) < tmp_mvy ? (CPMV)((
					1 << (18 - 1)) - 1) : tmp_mvy))));
			}
#if ENABLE_BFRAME
			adjust_fine_trans = adjust_cpmv_iter(refpic_y, CtrPos, in, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
				in->temp_trans_mv, vertex_num, (int)(in->curr_mvr), sub_w, sub_h);
			if (adjust_fine_trans) {
				mc_mvf_valid = com_affine_mc_l_fetch(refpic_y, CtrPos, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
					in->temp_trans_mv, pred_buf, vertex_num, sub_w, sub_h, bit_depth, mod_info_curr->x_chroma, mod_info_curr->y_chroma);
#else
			adjust_fine_trans = adjust_cpmv_iter(RefWindowLCU, CtrPos, in, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
				in->temp_trans_mv, vertex_num, (int)(in->curr_mvr), sub_w, sub_h);
			if (adjust_fine_trans) {
				mc_mvf_valid = com_affine_mc_l_fetch(RefWindowLCU, CtrPos, x, y, in->pic_width_luma, in->pic_height_luma, cu_width, cu_height,
					in->temp_trans_mv, pred_buf, vertex_num, sub_w, sub_h, bit_depth, mod_info_curr->x_chroma, mod_info_curr->y_chroma);
#endif
			
				//printf("fine trans: (");
				//for (vertex = 0; vertex < vertex_num; vertex++) {
				//	printf("(%d %d) ", in->temp_trans_mv[vertex][0], in->temp_trans_mv[vertex][1]);
				//}
				//printf(") ");
				if (mc_mvf_valid)
				{
					for (i = 0; i < cu_height; i++)
#pragma HLS LOOP_TRIPCOUNT max = 64
					{
						for (j = 0; j < cu_width; j++)
#pragma HLS LOOP_TRIPCOUNT max = 64
						{
							pred[(i << cu_width_log2) + j] = pred_buf[(i << cu_width_log2) + j];
						}
					}
					in->cost_trans[lidx][refi_cur] = calc_satd_16b(cu_width, cu_height, p_fenc_LCU_Y + in->org_offset[0],
						pred, in->stride_org_y, cu_width);
					mebits = get_affine_mv_bits(in->temp_trans_mv, in->affine_mvp_scale[lidx][refi_cur], (int)(in->num_refp), refi_cur,
						vertex_num, in->curr_mvr);
					mvp_temp = in->cost_trans[lidx][refi_cur] + ((u32)((in->lambda_mv * ((unsigned int)mebits) + ((unsigned int)(
						1 << 15))) >> 16));
					//printf("bits=%d cost=%d\n", mebits, mvp_temp);
				}
				else
				{
					adjust_fine_trans = 0;
				}
			}
			if (adjust_fine_mvp && adjust_fine_trans) {
				if (mvp_temp < mvp_best) {
					startmv_idx = 2;
				}
				else {
					startmv_idx = 1;
				}
			}
			else if (adjust_fine_mvp) {
				startmv_idx = 1;
			}
			else if (adjust_fine_trans) {
				startmv_idx = 2;
			}
			else {
				startmv_idx = 0;
			}
			/*start with zeros*/
			if (startmv_idx == 0) {
				for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
					mod_info_curr->affine_mv[lidx][vertex][0] = 0;
					mod_info_curr->affine_mv[lidx][vertex][1] = 0;
				}
			}
			else
				/*start with mvp*/
			if (startmv_idx == 1) {
				for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
					mod_info_curr->affine_mv[lidx][vertex][0] = in->temp_affine_mvp[vertex][0];
					mod_info_curr->affine_mv[lidx][vertex][1] = in->temp_affine_mvp[vertex][1];
				}
			}
			else {
				for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
					mod_info_curr->affine_mv[lidx][vertex][0] = in->temp_trans_mv[vertex][0];
					mod_info_curr->affine_mv[lidx][vertex][1] = in->temp_trans_mv[vertex][1];
				}
			}
		}
		/* affine motion search */
#if ENABLE_BFRAME
		mecost = pinter_affine_me_gradient_fetch_in_iter(refpic_y, CtrPos, in, p_fenc_LCU_Y, x, y, cu_width_log2, cu_height_log2, (s8)(refi_cur + 1), lidx,
			in->affine_mvp_scale[lidx][refi_cur], mod_info_curr->affine_mv[lidx], 0, vertex_num, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma);
#else
		mecost = pinter_affine_me_gradient_fetch_in_iter(RefWindowLCU, CtrPos, in, p_fenc_LCU_Y, x, y, cu_width_log2, cu_height_log2, (s8)(refi_cur + 1), lidx,
			in->affine_mvp_scale[lidx][refi_cur], mod_info_curr->affine_mv[lidx], 0, vertex_num, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma);
#endif
		
		// update MVP bits
		t0 = (((lidx == 0 ? refi_cur : (U5)-1)));
		t1 = (((lidx == 1 ? refi_cur : (U5)-1)));
		refi[0] = t0;
		refi[1] = t1;
		mebits = get_affine_mv_bits(mod_info_curr->affine_mv[lidx], in->affine_mvp_scale[lidx][refi_cur], (int)(in->num_refp),
			refi_cur, vertex_num, in->curr_mvr);
		if (mecost != 0xFFFFFFFF)
		{
			mecost += ((in->lambda_mv * mebits + (1 << 15)) >> 16);
		}
		/* save affine per ref me results */
		for (vertex = 0; vertex < vertex_num; vertex++) {

#pragma HLS loop_tripcount max = 2
			in->affine_mv_scale[lidx][refi_cur][vertex][0] = mod_info_curr->affine_mv[lidx][vertex][0];
			in->affine_mv_scale[lidx][refi_cur][vertex][1] = mod_info_curr->affine_mv[lidx][vertex][1];
		}
		if (mecost < best_mecost) {
			best_mecost = mecost;
			best_bits = mebits;
			*refi_temp = refi_cur;
		}
	}
}

BOOL derive_mode_info(int idx, SKIP_RDO *skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO *merge, MD_KERNEL_MODE*mod_info_curr)
{
#pragma HLS array_partition variable=mod_info_curr->mv complete dim=0
#pragma HLS array_partition variable=mod_info_curr->refi complete dim=0

	if (idx < skip->num_rdo) {
		int skip_idx = idx;
		mod_info_curr->cu_mode = MODE_DIR;
		mod_info_curr->umve_flag = skip_rdo_mode[skip_idx].umve_flag;
		mod_info_curr->umve_idx = skip_rdo_mode[skip_idx].umve_idx;
		mod_info_curr->affine_flag = skip_rdo_mode[skip_idx].affine_flag;
		mod_info_curr->skip_idx = skip_rdo_mode[skip_idx].skip_idx;
		mod_info_curr->mv[0][0] = skip_rdo_mode[skip_idx].mvp[REFP_0][MV_X];
		mod_info_curr->mv[0][1] = skip_rdo_mode[skip_idx].mvp[REFP_0][MV_Y];
		mod_info_curr->refi[0] = skip_rdo_mode[skip_idx].refi[REFP_0];
#if ENABLE_BFRAME
		mod_info_curr->mv[1][0] = skip_rdo_mode[skip_idx].mvp[REFP_1][MV_X];
		mod_info_curr->mv[1][1] = skip_rdo_mode[skip_idx].mvp[REFP_1][MV_Y];
		mod_info_curr->refi[1] = skip_rdo_mode[skip_idx].refi[REFP_1];
#else
		mod_info_curr->mv[1][0] = 0;
		mod_info_curr->mv[1][1] = 0;
		mod_info_curr->refi[1] = -1;
#endif
	}
	return 1;
}

BOOL derive_mode_info(int idx, SKIP_RDO* skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge, MD_COM_MODE* mod_info_curr)
{
#pragma HLS array_partition variable=mod_info_curr->mv complete dim=0
#pragma HLS array_partition variable=mod_info_curr->refi complete dim=0

	if (idx < skip->num_rdo)
	{
		int skip_idx = idx;
		mod_info_curr->cu_mode = MODE_DIR;
		mod_info_curr->umve_flag = skip_rdo_mode[skip_idx].umve_flag;
		mod_info_curr->umve_idx = skip_rdo_mode[skip_idx].umve_idx;
		mod_info_curr->affine_flag = skip_rdo_mode[skip_idx].affine_flag;
		mod_info_curr->skip_idx = skip_rdo_mode[skip_idx].skip_idx;
		mod_info_curr->mv[0][0] = skip_rdo_mode[skip_idx].mvp[REFP_0][MV_X];
		mod_info_curr->mv[0][1] = skip_rdo_mode[skip_idx].mvp[REFP_0][MV_Y];
		mod_info_curr->refi[0] = skip_rdo_mode[skip_idx].refi[REFP_0];
#if ENABLE_BFRAME
		mod_info_curr->mv[1][0] = skip_rdo_mode[skip_idx].mvp[REFP_1][MV_X];
		mod_info_curr->mv[1][1] = skip_rdo_mode[skip_idx].mvp[REFP_1][MV_Y];
		mod_info_curr->refi[1] = skip_rdo_mode[skip_idx].refi[REFP_1];
#else
		mod_info_curr->mv[1][0] = 0;
		mod_info_curr->mv[1][1] = 0;
		mod_info_curr->refi[1] = -1;
#endif
	}
	return 1;
}

BOOL derive_mode_info_64(int idx, SKIP_RDO* skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge, MD_COM_MODE* mod_info_curr,
	S3 refi_for_ip[2], S14 mv_for_ip[2][2], U8* umve_flag, U8* skip_idx_in)
{
#pragma HLS array_partition variable=mod_info_curr->mv complete dim=0
#pragma HLS array_partition variable=mod_info_curr->refi complete dim=0

	if (idx < skip->num_rdo)
	{
		int skip_idx = idx;
		//mod_info_curr->cu_mode = MODE_DIR;
		//mod_info_curr->umve_flag = skip_rdo_mode[skip_idx].umve_flag;
		*umve_flag = skip_rdo_mode[skip_idx].umve_flag;
		mod_info_curr->umve_idx = skip_rdo_mode[skip_idx].umve_idx;
		mod_info_curr->affine_flag = skip_rdo_mode[skip_idx].affine_flag;
		//mod_info_curr->skip_idx = skip_rdo_mode[skip_idx].skip_idx;
		*skip_idx_in = skip_rdo_mode[skip_idx].skip_idx;
		mv_for_ip[0][0] = skip_rdo_mode[skip_idx].mvp[REFP_0][MV_X];
		mv_for_ip[0][1] = skip_rdo_mode[skip_idx].mvp[REFP_0][MV_Y];
		refi_for_ip[0] = skip_rdo_mode[skip_idx].refi[REFP_0];
#if ENABLE_BFRAME
		mv_for_ip[1][0] = skip_rdo_mode[skip_idx].mvp[REFP_1][MV_X];
		mv_for_ip[1][1] = skip_rdo_mode[skip_idx].mvp[REFP_1][MV_Y];
		refi_for_ip[1] = skip_rdo_mode[skip_idx].refi[REFP_1];
#else
		mv_for_ip[1][0] = 0;
		mv_for_ip[1][1] = 0;
		refi_for_ip[1] = -1;
#endif
	}
	return 1;
}

static BOOL com_mc_cu_y(CU_POS *cu, int pic_w, int pic_h, s8 refi, s16 mv[2], int stride_luma, pel *ref_y,
	pel pred_y[(1 << 7 >> 1) * (1 << 7 >> 1)], int bit_depth)
{
	int scup = cu->scup;
	int x = cu->x_pos;
	int y = cu->y_pos;
	int w = cu->cu_width;
	int h = cu->cu_height;
	s16 mv_t[2];
	int min_clip[2];
	int max_clip[2];
	int x_ext = cu->x_pos << 2;
	int y_ext = cu->y_pos << 2;
	int w_ext = cu->cu_width << 2;
	int h_ext = cu->cu_height << 2;
	int x_ext_of_ctu;
	int y_ext_of_ctu;
	int qpel_gmv_x_of_ctu;
	int qpel_gmv_y_of_ctu;
	int qpel_gmv_x_of_refp_cu;
	int qpel_gmv_y_of_refp_cu;

	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0] = mv[0];
	mv_t[1] = mv[1];
	if (((int)refi) >= 0) {
		if (x_ext + ((int)mv[0]) < min_clip[0]) {
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + ((int)mv[1]) < min_clip[1]) {
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + ((int)mv[0]) + w_ext - 4 > max_clip[0]) {
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + ((int)mv[1]) + h_ext - 4 > max_clip[1]) {
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
		y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);
		qpel_gmv_x_of_ctu = x_ext_of_ctu + ((int)mv_t[0]);
		qpel_gmv_y_of_ctu = y_ext_of_ctu + ((int)mv_t[1]);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
		qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);

		//		assert((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < SWH);
		if (!((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < SWH)){ return FALSE; }

		com_tbl_mc_l_1((int)mv[0], (int)mv[1], ref_y, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, stride_luma, cu->cu_width,
			pred_y, cu->cu_width, cu->cu_height, bit_depth);
	}
#if MD_PRINT
	else {
		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3213);
	}
#endif
	return 1;
}




static void com_mc_c_00(pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 *pred, int w, int h, int bit_depth,
	int bHPFilter)
{
	int i;
	int j;
	int offset_ref = 0;
	int offset_pred = 0;
	if (bHPFilter) {
		gmv_x >>= 5;
		gmv_y >>= 5;
	}
	else {
		gmv_x >>= 3;
		gmv_y >>= 3;
	}
	//ref += gmv_y * s_ref + gmv_x;
	offset_ref += gmv_y * s_ref + gmv_x;
	{
		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				pred[j + offset_pred] = ref[j + offset_ref];
			}
			offset_pred += s_pred;
			offset_ref += s_ref;
		}
	}
}
static const s16 tbl_mc_c_coeff_all[2][32][4] = { { { 0, (64), 0, 0 }, { ((s16)(-4)), (62), (6), 0 }, { ((s16)(-6)), (56), (15), ((s16)(-1)) }, { ((s16)(-5)), (47), (25), ((s16)(-3)) }, { ((s16)(-4)), (36), (36), ((s16)(-4)) }, { ((s16)(-3)), (25), (47), ((s16)(-5)) }, { ((s16)(-1)), (15), (56), ((s16)(-6)) }, { 0, (6), (62), ((s16)(-4)) }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } }, { { 0, (64), 0, 0 }, { ((s16)(-1)), (63), (2), 0 }, { ((s16)(-2)), (62), (4), 0 }, { ((s16)(-2)), (60), (7), ((s16)(-1)) }, { ((s16)(-2)), (58), (10), ((s16)(-2)) }, { ((s16)(-3)), (57), (12), ((s16)(-2)) }, { ((s16)(-4)), (56), (14), ((s16)(-2)) }, { ((s16)(-4)), (55), (15), ((s16)(-2)) }, { ((s16)(-4)), (54), (16), ((s16)(-2)) }, { ((s16)(-5)), (53), (18), ((s16)(-2)) }, { ((s16)(-6)), (52), (20), ((s16)(-2)) }, { ((s16)(-6)), (49), (24), ((s16)(-3)) }, { ((s16)(-6)), (46), (28), ((s16)(-4)) }, { ((s16)(-5)), (44), (29), ((s16)(-4)) }, { ((s16)(-4)), (42), (30), ((s16)(-4)) }, { ((s16)(-4)), (39), (33), ((s16)(-4)) }, { ((s16)(-4)), (36), (36), ((s16)(-4)) }, { ((s16)(-4)), (33), (39), ((s16)(-4)) }, { ((s16)(-4)), (30), (42), ((s16)(-4)) }, { ((s16)(-4)), (29), (44), ((s16)(-5)) }, { ((s16)(-4)), (28), (46), ((s16)(-6)) }, { ((s16)(-3)), (24), (49), ((s16)(-6)) }, { ((s16)(-2)), (20), (52), ((s16)(-6)) }, { ((s16)(-2)), (18), (53), ((s16)(-5)) }, { ((s16)(-2)), (16), (54), ((s16)(-4)) }, { ((s16)(-2)), (15), (55), ((s16)(-4)) }, { ((s16)(-2)), (14), (56), ((s16)(-4)) }, { ((s16)(-2)), (12), (57), ((s16)(-3)) }, { ((s16)(-2)), (10), (58), ((s16)(-2)) }, { ((s16)(-1)), (7), (60), ((s16)(-2)) }, { 0, (4), (62), ((s16)(-2)) }, { 0, (2), (63), ((s16)(-1)) } } };

static void com_mc_c_0n(pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 *pred, int w, int h, int bit_depth,
	int bHPFilter)
{
	int dy;
	int offset_ref = 0;
	int offset_pred = 0;
	if (bHPFilter) {
		dy = gmv_y & 31;
		offset_ref += ((gmv_y >> 5) - 1) * s_ref + (gmv_x >> 5);
	}
	else {
		dy = gmv_y & 0x7;
		offset_ref += ((gmv_y >> 3) - 1) * s_ref + (gmv_x >> 3);
	}
	//const s16 *coeff_ver = bHPFilter ? tbl_mc_c_coeff_hp[dy] : tbl_mc_c_coeff[dy];
	//const s16 *coeff_ver = tbl_mc_c_coeff_all[bHPFilter][dy];
	{
		int i;
		int j;
		s32 pt;
		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				pt = (((int)tbl_mc_c_coeff_all[bHPFilter][dy][0]) * ((int)ref[j + offset_ref]) + ((
					int)tbl_mc_c_coeff_all[bHPFilter][dy][1]) * ((int)ref[s_ref + j + offset_ref]) + ((
					int)tbl_mc_c_coeff_all[bHPFilter][dy][2]) * ((int)ref[s_ref * 2 + j + offset_ref]) + ((
					int)tbl_mc_c_coeff_all[bHPFilter][dy][3]) * ((int)ref[s_ref * 3 + j + offset_ref]) + (1 << 5)) >> 6;
				pred[j + offset_pred] = ((s16)((0 >(((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += s_pred;
			offset_ref += s_ref;
		}
	}
}

static void com_mc_c_n0(pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 *pred, int w, int h, int bit_depth,
	int bHPFilter)
{
	int dx;
	int offset_ref = 0;
	int offset_pred = 0;
	if (bHPFilter) {
		dx = gmv_x & 31;
		offset_ref += (gmv_y >> 5) * s_ref + (gmv_x >> 5) - 1;
	}
	else {
		dx = gmv_x & 0x7;
		offset_ref += (gmv_y >> 3) * s_ref + (gmv_x >> 3) - 1;
	}
	//const s16 *coeff_hor = bHPFilter ? tbl_mc_c_coeff_hp[dx] : tbl_mc_c_coeff[dx];
	//const s16 *coeff_hor = tbl_mc_c_coeff_all[bHPFilter][dx];
	{
		int i;
		int j;
		s32 pt;
		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				pt = (((int)tbl_mc_c_coeff_all[bHPFilter][dx][0]) * ((int)ref[j + offset_ref]) + ((
					int)tbl_mc_c_coeff_all[bHPFilter][dx][1]) * ((int)ref[j + 1 + offset_ref]) + ((int)
					tbl_mc_c_coeff_all[bHPFilter][dx][2]) * ((int)ref[j + 2 + offset_ref]) + ((int)tbl_mc_c_coeff_all[bHPFilter][dx][3])
					* ((int)ref[j + 3 + offset_ref]) + (1 << 5)) >> 6;
				pred[j + offset_pred] = ((s16)((0 >(((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += s_pred;
			offset_ref += s_ref;
		}
	}
}

static void com_mc_c_nn(pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 *pred, int w, int h, int bit_depth,
	int bHPFilter)
{
	static  s16 buf[68 * 64];
	int dx;
	int dy;
	int offset_ref = 0;
	int offset_pred = 0;
	int offset_buf = 0;
	if (bHPFilter) {
		dx = gmv_x & 31;
		dy = gmv_y & 31;
		offset_ref += ((gmv_y >> 5) - 1) * s_ref + (gmv_x >> 5) - 1;
	}
	else {
		dx = gmv_x & 0x7;
		dy = gmv_y & 0x7;
		offset_ref += ((gmv_y >> 3) - 1) * s_ref + (gmv_x >> 3) - 1;
	}
	//const s16 *coeff_hor = bHPFilter ? tbl_mc_c_coeff_hp[dx] : tbl_mc_c_coeff[dx];
	//const s16 *coeff_ver = bHPFilter ? tbl_mc_c_coeff_hp[dy] : tbl_mc_c_coeff[dy];
	//const s16 *coeff_hor = tbl_mc_c_coeff_all[bHPFilter][dx];
	//const s16 *coeff_ver = tbl_mc_c_coeff_all[bHPFilter][dy];
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = 1 << (shift2 - 1);
	{
		//s16        *b;
		int i;
		int j;
		s32 pt;
		//b = buf;
		for (i = 0; i < h + 3; i++) {

#pragma HLS loop_tripcount max = 67
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				buf[j + offset_buf] = ((s16)((((int)tbl_mc_c_coeff_all[bHPFilter][dx][0]) * ((int)ref[j + offset_ref]) + ((
					int)tbl_mc_c_coeff_all[bHPFilter][dx][1]) * ((int)ref[j + 1 + offset_ref]) + ((int)
					tbl_mc_c_coeff_all[bHPFilter][dx][2]) * ((int)ref[j + 2 + offset_ref]) + ((int)tbl_mc_c_coeff_all[bHPFilter][dx][3])
					* ((int)ref[j + 3 + offset_ref]) + add1) >> shift1));
			}
			offset_ref += s_ref;
			offset_buf += w;
		}
		//b = buf;
		offset_buf = 0;
		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 64
				pt = (((int)tbl_mc_c_coeff_all[bHPFilter][dy][0]) * ((int)buf[j + offset_buf]) + ((
					int)tbl_mc_c_coeff_all[bHPFilter][dy][1]) * ((int)buf[j + w + offset_buf]) + ((int)
					tbl_mc_c_coeff_all[bHPFilter][dy][2]) * ((int)buf[j + 2 * w + offset_buf]) + ((int)
					tbl_mc_c_coeff_all[bHPFilter][dy][3]) * ((int)buf[j + 3 * w + offset_buf]) + add2) >> shift2;
				pred[j + offset_pred] = ((s16)((0 >(((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += s_pred;
			offset_buf += w;
		}
	}
}

void com_tbl_mc_c_hp(int ori_mv_x, int ori_mv_y, pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred,
	s16 *pred, int w, int h, int bit_depth)
{
	//add by luke
	int dx;
	int dy;
	dx = ori_mv_x & 0x1F;
	dy = ori_mv_y & 0x1F;
	if (dx == 0) {
		if (dy == 0) {
			com_mc_c_00(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
		}
		else {
			com_mc_c_0n(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
		}
	}
	else {
		if (dy == 0) {
			com_mc_c_n0(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
		}
		else {
			com_mc_c_nn(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 1);
		}
	}
}


void com_tbl_mc_c_1(int ori_mv_x, int ori_mv_y, pel *ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 *pred,
	int w, int h, int bit_depth)
{
	//add by luke
	int dx;
	int dy;
	dx = ori_mv_x & 0x7;
	dy = ori_mv_y & 0x7;
	/*
	if (dx == 0 && dy == 0)
	{
	com_mc_c_00(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}
	else if (dx == 0 && dy != 0)
	{
	com_mc_c_0n(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}
	else if (dx != 0 && dy == 0)
	{
	com_mc_c_n0(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}
	else if (dx != 0 && dy != 0)
	{
	com_mc_c_nn(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
	}*/
	if (dx == 0) {
		if (dy == 0) {
			com_mc_c_00(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
		}
		else {
			com_mc_c_0n(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
		}
	}
	else {
		if (dy == 0) {
			com_mc_c_n0(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
		}
		else {
			com_mc_c_nn(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0);
		}
	}
}

void get_ref_u_wsc4(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx, U2* U_ref0)
{
#pragma HLS INLINE 
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->lidx complete dim=0
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->cur_refi complete dim=0
	* U_ref0 = 0;
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			*U_ref0 = 1;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			*U_ref0 = 2;
		}
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			* U_ref0 = 1;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			*U_ref0 = 2;
		}
	}
#endif
}
static void com_mc_c_00_uv4(pel UP_tmp_mc[7][7],pel VP_tmp_mc[7][7],int gmv_x, int gmv_y,  
	s16 pred_u[4][4], s16 pred_v[4][4], int w, int h, int bit_depth)
{
#pragma HLS INLINE
	int i;
	int j;
		gmv_x >>= 3;
		gmv_y >>= 3;
	for (i = 0; i < 4; i++) {
#pragma HLS PIPELINE
		for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
			pred_u[i][j] = UP_tmp_mc[i+1][j + 1];
			pred_v[i][j] = VP_tmp_mc[i+1][j + 1];
		}
	}
}
static void com_mc_c_0n_uv4(pel UP_tmp_mc[7][7], pel VP_tmp_mc[7][7], int gmv_x, int gmv_y,  
	s16 pred_u[4][4], s16 pred_v[4][4], int w, int h, int bit_depth,s16 tbl_mc_c_coeff_all_dy[4])
{
#pragma HLS INLINE
	int dy;
	int offset_pred = 0;
	dy = gmv_y & 0x7;
		int i;
		int j;
		s32 pt;
		for (i = 0; i < 4; i++) {
#pragma HLS PIPELINE
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				pt = (
					((int)tbl_mc_c_coeff_all_dy[0]) * ((int)UP_tmp_mc[i][j + 1]) +
					((int)tbl_mc_c_coeff_all_dy[1]) * ((int)UP_tmp_mc[i + 1][j + 1]) +
					((int)tbl_mc_c_coeff_all_dy[2]) * ((int)UP_tmp_mc[i + 2][j + 1]) +
					((int)tbl_mc_c_coeff_all_dy[3]) * ((int)UP_tmp_mc[i + 3][j + 1]) + (1 << 5)) >> 6;
				pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				pt = (
					((int)tbl_mc_c_coeff_all_dy[0]) * ((int)VP_tmp_mc[i][j + 1]) +
					((int)tbl_mc_c_coeff_all_dy[1]) * ((int)VP_tmp_mc[i + 1][j + 1]) +
					((int)tbl_mc_c_coeff_all_dy[2]) * ((int)VP_tmp_mc[i + 2][j + 1]) +
					((int)tbl_mc_c_coeff_all_dy[3]) * ((int)VP_tmp_mc[i + 3][j + 1]) + (1 << 5)) >> 6;
				pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += w;
		}
}
static void com_mc_c_n0_uv4(pel UP_tmp_mc[7][7], pel VP_tmp_mc[7][7], int gmv_x, int gmv_y,  
	s16 pred_u[4][4], s16 pred_v[4][4], int w, int h, int bit_depth, s16 tbl_mc_c_coeff_all_dx[4])
{
#pragma HLS INLINE
	int dx;
	int offset_pred = 0;
		dx = gmv_x & 0x7;
		int i;
		int j;
		s32 pt;
		for (i = 0; i < 4; i++) {
#pragma HLS PIPELINE
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				pt = (
					((int)tbl_mc_c_coeff_all_dx[0]) * ((int)UP_tmp_mc[i + 1][j]) +
					((int)tbl_mc_c_coeff_all_dx[1]) * ((int)UP_tmp_mc[i + 1][j + 1 ]) +
					((int)tbl_mc_c_coeff_all_dx[2]) * ((int)UP_tmp_mc[i + 1][j + 2 ]) +
					((int)tbl_mc_c_coeff_all_dx[3]) * ((int)UP_tmp_mc[i + 1][j + 3 ]) + (1 << 5)) >> 6;
				pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				pt = (
					((int)tbl_mc_c_coeff_all_dx[0]) * ((int)VP_tmp_mc[i + 1][j]) +
					((int)tbl_mc_c_coeff_all_dx[1]) * ((int)VP_tmp_mc[i + 1][j + 1 ]) +
					((int)tbl_mc_c_coeff_all_dx[2]) * ((int)VP_tmp_mc[i + 1][j + 2 ]) +
					((int)tbl_mc_c_coeff_all_dx[3]) * ((int)VP_tmp_mc[i + 1][j + 3 ]) + (1 << 5)) >> 6;
				pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += w;
		}
}
static void com_mc_c_nn_uv4(pel UP_tmp_mc[7][7], pel VP_tmp_mc[7][7], int gmv_x, int gmv_y, 
	s16 pred_u[4][4], s16 pred_v[4][4], int w, int h, int bit_depth, s16 tbl_mc_c_coeff_all_dx[4], s16 tbl_mc_c_coeff_all_dy[4])
{
#pragma HLS INLINE
	static  s16 buf_u[7][4];
	static  s16 buf_v[7][4];
#pragma HLS ARRAY_PARTITION variable = buf_u complete dim = 0
#pragma HLS ARRAY_PARTITION variable = buf_v complete dim = 0
	int dx;
	int dy;
	int offset_ref = 0;
	int offset_pred = 0;
		dx = gmv_x & 0x7;
		dy = gmv_y & 0x7;
		offset_ref += ((gmv_y >> 3) - 1) * 128 + (gmv_x >> 3) - 1;
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = 1 << (shift2 - 1);
	{
		int i;
		int j;
		s32 pt;
		for (i = 0; i < 4 + 3; i++) {
#pragma HLS UNROLL	
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				buf_u[i][j] = ((s16)((
					((int)tbl_mc_c_coeff_all_dx[0]) * ((int)UP_tmp_mc[i][j ]) +
					((int)tbl_mc_c_coeff_all_dx[1]) * ((int)UP_tmp_mc[i][j + 1 ]) +
					((int)tbl_mc_c_coeff_all_dx[2]) * ((int)UP_tmp_mc[i][j + 2 ]) +
					((int)tbl_mc_c_coeff_all_dx[3]) * ((int)UP_tmp_mc[i][j + 3 ]) + add1) >> shift1));
				buf_v[i][j] = ((s16)((
					((int)tbl_mc_c_coeff_all_dx[0]) * ((int)VP_tmp_mc[i][j ]) +
					((int)tbl_mc_c_coeff_all_dx[1]) * ((int)VP_tmp_mc[i][j + 1 ]) +
					((int)tbl_mc_c_coeff_all_dx[2]) * ((int)VP_tmp_mc[i][j + 2 ]) +
					((int)tbl_mc_c_coeff_all_dx[3]) * ((int)VP_tmp_mc[i][j + 3 ]) + add1) >> shift1));
			}
			offset_ref += 128;
		}
		for (i = 0; i < 4; i++) {
#pragma HLS PIPELINE
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				pt = (
					((int)tbl_mc_c_coeff_all_dy[0]) * ((int)buf_u[i + 0][j]) + 
					((int)tbl_mc_c_coeff_all_dy[1]) * ((int)buf_u[i + 1][j]) + 
					((int)tbl_mc_c_coeff_all_dy[2]) * ((int)buf_u[i + 2][j]) + 
					((int)tbl_mc_c_coeff_all_dy[3]) * ((int)buf_u[i + 3][j]) + add2) >> shift2;
				pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				pt = (
					((int)tbl_mc_c_coeff_all_dy[0]) * ((int)buf_v[i + 0][j]) +
					((int)tbl_mc_c_coeff_all_dy[1]) * ((int)buf_v[i + 1][j]) +
					((int)tbl_mc_c_coeff_all_dy[2]) * ((int)buf_v[i + 2][j]) +
					((int)tbl_mc_c_coeff_all_dy[3]) * ((int)buf_v[i + 3][j]) + add2) >> shift2;
				pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += w;
		}
	}
}
void com_tbl_mc_c_1_uv4_wsc(int ori_mv_x, int ori_mv_y, int gmv_x, int gmv_y, 
	s16 pred_u[4][4], s16 pred_v[4][4],int w, int h, int bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr, U2 UV_flag)
{
#pragma HLS INLINE 
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_UP cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_VP cyclic factor=128 dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1 cyclic factor=128 dim=1
#pragma HLS ARRAY_RESHAPE variable = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1 cyclic factor=128 dim=1
#endif
#pragma HLS ARRAY_PARTITION variable=pred_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_v complete dim=2
	pel UP_tmp[128];
	pel VP_tmp[128];
	pel UP_tmp_mc[7][7];
	pel VP_tmp_mc[7][7];
#pragma HLS ARRAY_PARTITION variable=UP_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=VP_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=UP_tmp_mc complete dim=0
#pragma HLS ARRAY_PARTITION variable=VP_tmp_mc complete dim=0
	for (int i = 0; i < 4 + 3; i++) {
#pragma HLS UNROLL
		for (int j = 0; j < 128; j++) {
#pragma HLS UNROLL
			if (UV_flag == 1) {
				UP_tmp[j] = Fetch_Ref_window_ptr->RefWindowLCU_UP[j + (((gmv_y >> 3) - 1) + i) * 128];
				VP_tmp[j] = Fetch_Ref_window_ptr->RefWindowLCU_VP[j + (((gmv_y >> 3) - 1) + i) * 128];
			}
#if ENABLE_BFRAME
			else if (UV_flag == 2) {
				UP_tmp[j] = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1[j + (((gmv_y >> 3) - 1) + i) * 128];
				VP_tmp[j] = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1[j + (((gmv_y >> 3) - 1) + i) * 128];
			}
#endif
		}
		for (int j = 0; j < 4 + 3; j++) {
#pragma HLS UNROLL
			UP_tmp_mc[i][j] = UP_tmp[j + (gmv_x >> 3) - 1];
			VP_tmp_mc[i][j] = VP_tmp[j + (gmv_x >> 3) - 1];
		}
	}
	int coeff_dx;
	int coeff_dy;
	coeff_dx = gmv_x & 0x7;
	coeff_dy = gmv_y & 0x7;
	const s16 tbl_mc_c_coeff_all_0[32][4] = {
		{ 0, (64), 0, 0 }, { ((s16)(-4)), (62), (6), 0 }, { ((s16)(-6)), (56), (15), ((s16)(-1)) }, { ((s16)(-5)), (47), (25), ((s16)(-3)) },
		{ ((s16)(-4)), (36), (36), ((s16)(-4)) }, { ((s16)(-3)), (25), (47), ((s16)(-5)) }, { ((s16)(-1)), (15), (56), ((s16)(-6)) }, { 0, (6), (62), ((s16)(-4)) },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
		{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
	s16 tbl_mc_c_coeff_all_dx[4];
	s16 tbl_mc_c_coeff_all_dy[4];
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_all_0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_all_dx complete dim=0
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_all_dy complete dim=0
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		tbl_mc_c_coeff_all_dx[i] = tbl_mc_c_coeff_all_0[coeff_dx][i];
	}
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		tbl_mc_c_coeff_all_dy[i] = tbl_mc_c_coeff_all_0[coeff_dy][i];
	}
	int dx;
	int dy;
	dx = ori_mv_x & 0x7;
	dy = ori_mv_y & 0x7;
	if (dx == 0) {
		if (dy == 0) {
			com_mc_c_00_uv4(UP_tmp_mc, VP_tmp_mc, gmv_x, gmv_y,   pred_u, pred_v, w, h, bit_depth);
		}
		else {
			com_mc_c_0n_uv4(UP_tmp_mc, VP_tmp_mc, gmv_x, gmv_y,   pred_u, pred_v, w, h, bit_depth,  tbl_mc_c_coeff_all_dy);
		}
	}
	else {
		if (dy == 0) {
			com_mc_c_n0_uv4(UP_tmp_mc, VP_tmp_mc, gmv_x, gmv_y,   pred_u, pred_v, w, h, bit_depth, tbl_mc_c_coeff_all_dx);
		}
		else {
			com_mc_c_nn_uv4(UP_tmp_mc, VP_tmp_mc, gmv_x, gmv_y,   pred_u, pred_v, w, h, bit_depth, tbl_mc_c_coeff_all_dx, tbl_mc_c_coeff_all_dy);
		}
	}
}
 BOOL com_mc_cu_uv4(MD_COM_MODE_BEST* mod_info_curr, int pic_w, int pic_h, S3 refi[REFP_NUM], s16 mv[REFP_NUM][2], 
	 pel pred_u[4][4], pel pred_v[4][4], int bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr)
{
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->refp_pic_ptr complete dim=0
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->refp_is_library_picture complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_v complete dim=2
	int scup = mod_info_curr->scup;
	int x = mod_info_curr->x_pos;
	int y = mod_info_curr->y_pos;
	int w = mod_info_curr->cu_width;
	int h = mod_info_curr->cu_height;
	//int	w1 = w>>1;
	//int	h1 = h>>1;
	int	w1 = 4;
	int	h1 = 4;
	s16 mv_t[2];
	int min_clip[2];
	int max_clip[2];
#pragma HLS ARRAY_PARTITION variable=mv_t complete dim=0
#pragma HLS ARRAY_PARTITION variable=min_clip complete dim=0
#pragma HLS ARRAY_PARTITION variable=max_clip complete dim=0
	int x_ext = mod_info_curr->x_pos << 2;
	int y_ext = mod_info_curr->y_pos << 2;
	int w_ext = mod_info_curr->cu_width << 2;
	int h_ext = mod_info_curr->cu_height << 2;
	int x_ext_of_ctu;
	int y_ext_of_ctu;
	int qpel_gmv_x_of_ctu;
	int qpel_gmv_y_of_ctu;
	int qpel_gmv_x_of_refp_cu;
	int qpel_gmv_y_of_refp_cu;
	int dx;
	int dy;

	int bidx = 0;
	static pel pred_u0[4][4];
	static pel pred_v0[4][4];
	static pel pred_u1[4][4];
	static pel pred_v1[4][4];
#pragma HLS ARRAY_PARTITION variable=pred_u0 complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_v0 complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_u1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_v1 complete dim=0
	U1 flag_u0=0;
	U1 flag_u1=0;
	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0] = mv[REFP_0][0];
	mv_t[1] = mv[REFP_0][1];
	if (refi[REFP_0] >= 0)
	{
		if (x_ext + mv[REFP_0][0] < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + mv[REFP_0][1] < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + mv[REFP_0][0] + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + mv[REFP_0][1] + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
		y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);
		qpel_gmv_x_of_ctu = x_ext_of_ctu + ((int)mv_t[0]);
		qpel_gmv_y_of_ctu = y_ext_of_ctu + ((int)mv_t[1]);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
		qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);
		//		assert((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < REFP_CU_SIZE && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < REFP_CU_SIZE);
		dx = qpel_gmv_x_of_refp_cu & 3;
		dy = qpel_gmv_y_of_refp_cu & 3;
		//assert((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + (dx ? 4 : 0) < SWW && (qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + (dy ? 4 : 0) < SWH);
		if (!((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + mod_info_curr->cu_width + (dx ? 4 : 0) <= SWW && (qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + mod_info_curr->cu_height + (dy ? 4 : 0) <= SWH)){ return FALSE; }

		w >= 8 && h >= 8 ? ((void)0) : __assert_HLS("w >= 8 && h >= 8", "../separate/com_mc.c", (unsigned int)3258,
			__FUNCTION__);
		//add by luke
		//pel ref_u0[SWH / 2*SWW / 2];
		//pel ref_v0[SWH / 2*SWW / 2];
		//get_ref_u(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0, ref_u0);
		//get_ref_v(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0, ref_v0);
		//com_tbl_mc_c_1(mv[REFP_0][0], mv[REFP_0][1], ref_u0, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, stride_chroma,
		//	mod_info_curr->cu_width >> 1, pred_u0, mod_info_curr->cu_width >> 1, mod_info_curr->cu_height >> 1, bit_depth);
		//com_tbl_mc_c_1(mv[REFP_0][0], mv[REFP_0][1], ref_v0, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, stride_chroma,
		//	mod_info_curr->cu_width >> 1, pred_v0, mod_info_curr->cu_width >> 1, mod_info_curr->cu_height >> 1, bit_depth);
		U2 UV_flag;
		get_ref_u_wsc4(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0, &UV_flag);
		com_tbl_mc_c_1_uv4_wsc(mv[REFP_0][0], mv[REFP_0][1],  qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, 
			 pred_u0,pred_v0, w1, h1, bit_depth, Fetch_Ref_window_ptr, UV_flag);
		++bidx;
		flag_u0 = 1;
	}
#if !ENABLE_BFRAME
	else {
#if MD_PRINT
		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3267);
#endif
	}
#else
	/* check identical motion */
	if (REFI_IS_VALID(refi[REFP_0]) && REFI_IS_VALID(refi[REFP_1]))
	{
#if LIBVC_ON
		if (Fetch_Ref_window_ptr->refp_pic_ptr[refi[REFP_0]][REFP_0] == Fetch_Ref_window_ptr->refp_pic_ptr[refi[REFP_1]][REFP_1] &&
			mv[REFP_0][MV_X] == mv[REFP_1][MV_X] && mv[REFP_0][MV_Y] == mv[REFP_1][MV_Y] &&
			Fetch_Ref_window_ptr->refp_is_library_picture[refi[REFP_0]][REFP_0] == Fetch_Ref_window_ptr->refp_is_library_picture[refi[REFP_1]][REFP_1])
#else
		if (refp[refi[REFP_0]][REFP_0].pic->ptr == refp[refi[REFP_1]][REFP_1].pic->ptr && mv[REFP_0][MV_X] == mv[REFP_1][MV_X] && mv[REFP_0][MV_Y] == mv[REFP_1][MV_Y])
#endif
		{
			if (flag_u0 ==1) {
				for (int j = 0; j < 4; j++)
				{
#pragma HLS UNROLL
					for (int i = 0; i < 4; i++)
					{
#pragma HLS UNROLL
						pred_u[j][i] = pred_u0[j][i];
						pred_v[j][i] = pred_v0[j][i];
					}
				}
			}
			return 1;
		}
	}

	mv_t[0] = mv[REFP_1][0];
	mv_t[1] = mv[REFP_1][1];
	//static pel pred_snd_u[64 * 64];
	//static pel pred_snd_v[64 * 64];
	if (REFI_IS_VALID(refi[REFP_1]))
	{
		/* backward */
		//pel* pred_buf_u = bidx ? pred_snd_u : pred_u;
		//pel* pred_buf_v = bidx ? pred_snd_v : pred_v;
		if (x_ext + mv[REFP_1][0] < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + mv[REFP_1][1] < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + mv[REFP_1][0] + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + mv[REFP_1][1] + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
		y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);
		qpel_gmv_x_of_ctu = x_ext_of_ctu + ((int)mv_t[0]);
		qpel_gmv_y_of_ctu = y_ext_of_ctu + ((int)mv_t[1]);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
		qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);
		//		assert((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < REFP_CU_SIZE && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < REFP_CU_SIZE);
		dx = qpel_gmv_x_of_refp_cu & 3;
		dy = qpel_gmv_y_of_refp_cu & 3;
		//assert((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + (dx ? 4 : 0) < SWW && (qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + (dy ? 4 : 0) < SWH);
		if (!((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + mod_info_curr->cu_width + (dx ? 4 : 0) < SWW && (qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + mod_info_curr->cu_height + (dy ? 4 : 0) < SWH)) { return FALSE; }

		w >= 8 && h >= 8 ? ((void)0) : __assert_HLS("w >= 8 && h >= 8", "../separate/com_mc.c", (unsigned int)3258,
			__FUNCTION__);
		//pel ref_u0[SWH / 2 * SWW / 2];
		//pel ref_v0[SWH / 2 * SWW / 2];
		//get_ref_u(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1, ref_u0);
		//get_ref_v(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1, ref_v0);
		//com_tbl_mc_c_1(mv[REFP_1][0], mv[REFP_1][1], ref_u0, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, 128,
		//	mod_info_curr->cu_width >> 1, pred_u1, mod_info_curr->cu_width >> 1, mod_info_curr->cu_height >> 1, bit_depth);
		//com_tbl_mc_c_1(mv[REFP_1][0], mv[REFP_1][1], ref_v0, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, 128,
		//	mod_info_curr->cu_width >> 1, pred_v1, mod_info_curr->cu_width >> 1, mod_info_curr->cu_height >> 1, bit_depth);
		U2 UV_flag;
		get_ref_u_wsc4(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1, &UV_flag);
		com_tbl_mc_c_1_uv4_wsc(mv[REFP_1][0], mv[REFP_1][1],  qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu,  
			pred_u1,pred_v1, w1, h1, bit_depth, Fetch_Ref_window_ptr, UV_flag);
		bidx++;
		flag_u1 = 1;
	}


	if ((flag_u0 == 1)&&(flag_u1 ==1 )) {
		for (int j = 0; j < 4; j++)
	{
#pragma HLS UNROLL
			for (int i = 0; i < 4; i++)
			{
#pragma HLS UNROLL
				pred_u[j][i] = (pred_u0[j][i] + pred_u1[j][i] + 1) >> 1;
				pred_v[j][i] = (pred_v0[j][i] + pred_v1[j][i] + 1) >> 1;
			}
		}
	}
	else if ((flag_u0 == 1) && (flag_u1 == 0)) {
		for (int j = 0; j < 4; j++)
		{
#pragma HLS UNROLL
			for (int i = 0; i < 4; i++)
			{
#pragma HLS UNROLL
				pred_u[j][i] = pred_u0[j][i] ;
				pred_v[j][i] = pred_v0[j][i] ;
			}
		}
	}
	else if ((flag_u0 == 0) && (flag_u1 == 1)) {
		for (int j = 0; j < 4; j++)
		{
#pragma HLS UNROLL
			for (int i = 0; i < 4; i++)
			{
#pragma HLS UNROLL
				pred_u[j][i] = pred_u1[j][i] ;
				pred_v[j][i] = pred_v1[j][i] ;
			}
		}
	}
#endif
	return 1;
}




void enc_set_affine_mvf(int affine_subblock_size_idx, ENC_CU_DATA_ARRAY *cu_data, MD_COM_MODE_BEST *mi)
{
	//ENC_CU_DATA_ARRAY *cu_data;
	int cu_width_log2;
	int cu_height_log2;
	U5 cu_w_in_scu;
	U5 cu_h_in_scu;
	U2 sub_w_in_scu;
	U2 sub_h_in_scu;
	int w;
	int h;
	int x;
	int y;
	int lidx;
	int idx;
	U9 cp_num = ((int)(mi->affine_flag)) + 1;
	U10 aff_scup[4];
	U4 sub_w = 4;
	U4 sub_h = 4;
	U3 half_w;
	U3 half_h;

	if (affine_subblock_size_idx > 0) {
		sub_w = 8;
		sub_h = 8;
	}
	if (((int)mi->refi[0]) >= 0 && ((int)mi->refi[1]) >= 0) {
		sub_w = 8;
		sub_h = 8;
	}
	half_w = sub_w >> 1;
	half_h = sub_h >> 1;
	sub_w_in_scu = sub_w >> 2;
	sub_h_in_scu = sub_h >> 2;
	cu_width_log2 = ((int)com_tbl_log2[mi->cu_width]);
	cu_height_log2 = ((int)com_tbl_log2[mi->cu_height]);
	cu_w_in_scu = mi->cu_width >> 2;
	cu_h_in_scu = mi->cu_height >> 2;
	aff_scup[0] = 0;
	aff_scup[1] = cu_w_in_scu - 1;
	aff_scup[2] = (cu_h_in_scu - 1) * cu_w_in_scu;
	aff_scup[3] = cu_w_in_scu - 1 + (cu_h_in_scu - 1) * cu_w_in_scu;
	for (lidx = 0; lidx < 2; lidx++) {
		if (((int)mi->refi[lidx]) >= 0) {
			CPMV(*ac_mv)[2] = mi->affine_mv[lidx];
			s32 dmv_hor_x;
			s32 dmv_ver_x;
			s32 dmv_hor_y;
			s32 dmv_ver_y;
			s32 mv_scale_hor = ((s32)ac_mv[0][0]) << 7;
			s32 mv_scale_ver = ((s32)ac_mv[0][1]) << 7;
			CPMV mv_scale_tmp_hor;
			CPMV mv_scale_tmp_ver;
			// convert to 2^(storeBit + iBit) precision
			// deltaMvHor
			dmv_hor_x = (((s32)ac_mv[1][0]) - ((s32)ac_mv[0][0])) << 7 >> mi->cu_width_log2;
			dmv_hor_y = (((s32)ac_mv[1][1]) - ((s32)ac_mv[0][1])) << 7 >> mi->cu_width_log2;
			if (cp_num == 3) {
				// deltaMvVer
				dmv_ver_x = (((s32)ac_mv[2][0]) - ((s32)ac_mv[0][0])) << 7 >> mi->cu_height_log2;
				dmv_ver_y = (((s32)ac_mv[2][1]) - ((s32)ac_mv[0][1])) << 7 >> mi->cu_height_log2;
			}
			else {
				// deltaMvVer
				dmv_ver_x = -dmv_hor_y;
				dmv_ver_y = dmv_hor_x;
			}
			idx = 0;
			for (h = 0; h < cu_h_in_scu; h += sub_h_in_scu) {
#pragma HLS LOOP_TRIPCOUNT max=16
				for (w = 0; w < cu_w_in_scu; w += sub_w_in_scu) {
#pragma HLS LOOP_TRIPCOUNT max=16
					int pos_x = (w << 2) + half_w;
					int pos_y = (h << 2) + half_h;
					if (w == 0 && h == 0) {
						pos_x = 0;
						pos_y = 0;
					}
					else if (w + sub_w_in_scu == cu_w_in_scu && h == 0) {
						pos_x = cu_w_in_scu << 2;
						pos_y = 0;
					}
					else if (w == 0 && h + sub_h_in_scu == cu_h_in_scu && cp_num == 3) {
						pos_x = 0;
						pos_y = cu_h_in_scu << 2;
					}
					mv_scale_tmp_hor = mv_scale_hor + dmv_hor_x * pos_x + dmv_ver_x * pos_y;
					mv_scale_tmp_ver = mv_scale_ver + dmv_hor_y * pos_x + dmv_ver_y * pos_y;
					// 1/16 precision, 18 bits, for MC
					com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
					mv_scale_tmp_hor = (((-131072)) > (((131071) < mv_scale_tmp_hor ? (CPMV)(131071) : mv_scale_tmp_hor)) ? (CPMV)(-131072) : (CPMV)(((131071) < mv_scale_tmp_hor ? (CPMV)(131071) : mv_scale_tmp_hor)));
					mv_scale_tmp_ver = (((-131072)) > (((131071) < mv_scale_tmp_ver ? (CPMV)(131071) : mv_scale_tmp_ver)) ? (CPMV)(-131072) : (CPMV)(((131071) < mv_scale_tmp_ver ? (CPMV)(131071) : mv_scale_tmp_ver)));
					// 1/4 precision, 16 bits, for mv storage
					com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 2, 0);
					mv_scale_tmp_hor = ((((s16)0x8000)) > (((((s16)0x7FFF)) < mv_scale_tmp_hor ? (((CPMV)0x7FFF)) : mv_scale_tmp_hor)) ? (((CPMV)0x8000)) : (((((CPMV)0x7FFF)) < mv_scale_tmp_hor ? (((CPMV)0x7FFF)) : mv_scale_tmp_hor)));
					mv_scale_tmp_ver = ((((s16)0x8000)) > (((((s16)0x7FFF)) < mv_scale_tmp_ver ? (((CPMV)0x7FFF)) : mv_scale_tmp_ver)) ? (((CPMV)0x8000)) : (((((CPMV)0x7FFF)) < mv_scale_tmp_ver ? (((CPMV)0x7FFF)) : mv_scale_tmp_ver)));
					// save MV for each 4x4 block
					for (y = h; y < h + sub_h_in_scu; y++) {
#pragma HLS LOOP_TRIPCOUNT max=2
						for (x = w; x < w + sub_w_in_scu; x++) {
#pragma HLS LOOP_TRIPCOUNT max=2
							idx = x + y * cu_w_in_scu;
							cu_data->mv[idx][lidx][0] = ((s16)mv_scale_tmp_hor);
							cu_data->mv[idx][lidx][1] = ((s16)mv_scale_tmp_ver);
						}
					}
				}
			}
			// save mvd for encoding, and reset vertex mv
			for (h = 0; h < cp_num; h++) {
#pragma HLS LOOP_TRIPCOUNT max=3
				cu_data->mvd[aff_scup[h]][lidx][0] = mi->affine_mvd[lidx][h][0];
				cu_data->mvd[aff_scup[h]][lidx][1] = mi->affine_mvd[lidx][h][1];
			}
		}
	}
}


static void fetch_ref_line_16(U8 cu_width, S15 hor_ref[SKIP_MC_OUT_WIDTH_16][8], const pel ref_y[SWH * SWW], const U10 r_off, const U10 c_off)
{
#pragma HLS INLINE OFF
	pel line_buf[SWW] = {};
	pel buf[23 + 16] = {};
#pragma HLS array_partition variable=line_buf complete dim=0
#pragma HLS array_partition variable=buf complete dim=0

	for (int i = 0; i < SWW; ++i){
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 && (r_off < SWH) ? (pel)(ref_y[r_off * SWW + i]) : (pel)0;
	}
	for (int i = 0; i < cu_width + 23; ++i){
#pragma HLS loop_tripcount max=39
#pragma HLS UNROLL
		buf[i] = c_off >= 0 ? (pel)(line_buf[c_off + i]) : (pel)0;
	}
	for (int i = 0; i < cu_width + 16; ++i){
#pragma HLS loop_tripcount max=32
#pragma HLS UNROLL
		for (int j = 0; j < 8; ++j){
#pragma HLS UNROLL
			hor_ref[i][j] = buf[i + j];
		}
	}
}

static void fetch_org_line_16(U8 cu_width, S15 hor_org[16][SKIP_MC_VER_NUM], const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U6 r_off, const U6 c_off)
{
#pragma HLS INLINE OFF
	pel line_buf[MAX_CU_SIZE_FETCH] = {};
#pragma HLS array_partition variable=line_buf complete dim=0
	for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i) {
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 ? (pel)(org_y[r_off][i]) : (pel)0;
	}
	for (int i = 0; i < cu_width; ++i) {
#pragma HLS UNROLL
		shifting(hor_org[i], line_buf[c_off + i], SKIP_MC_VER_NUM);
	}
}

static void mc_sad_l_skip_16(S32 sad[SKIP_MODE_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const U1 mv_idx, const U12 x_pos, const U12 y_pos, const U7 cu_width, const U7 cu_height, const U12 pic_w, const U12 pic_h, const S12 mc_pos[MV_D], const pel ref_y[SWH * SWW], const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U12 x_chroma, const U12 y_chroma, const U1 chroma_motion)
{
#pragma HLS INLINE OFF

	U1 flag[SKIP_MODE_NUM] = {};
	S15 hor_ref[SKIP_MC_OUT_WIDTH_16][8] = {};
	S15 hor_buf[SKIP_MC_OUT_WIDTH_16][4][8] = {};
	S15 hor_org[16][SKIP_MC_VER_NUM] = {};
#pragma HLS array_partition variable=flag complete dim=0
#pragma HLS array_partition variable=hor_ref complete dim=0
#pragma HLS array_partition variable=hor_buf complete dim=0
#pragma HLS array_partition variable=hor_org complete dim=0

	const U10 ref_top = mc_pos[MV_Y] - 3;
	const U10 ref_left = mc_pos[MV_X] - 3;
	const U6 org_top = y_pos & (MAX_CU_SIZE_FETCH - 1);
	const U6 org_left = x_pos & (MAX_CU_SIZE_FETCH - 1);

	if (ref_top + SKIP_MC_IN_HEIGHT_16 > SWH) { return; }
	if (ref_left + SKIP_MC_IN_WIDTH_16 > SWW) { return; }

	for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		flag[i] = REFI_IS_VALID(mode[i].refi) && mode[i].mv_idx == mv_idx && check_mv_in_refwin(ref_top + (mode[i].mvd[MV_Y] >> 2), ref_left + (mode[i].mvd[MV_X] >> 2), x_pos, y_pos, cu_width, cu_height, x_chroma, y_chroma, chroma_motion);
	}
	static int pflag = 1;
	//LOOP_MC_SAD:
	for (int r = 0; r < SKIP_MC_IN_HEIGHT_16; ++r){
#pragma HLS PIPELINE II=1
		if (r > cu_height + 11)
			break;

		fetch_ref_line_16(cu_width, hor_ref, ref_y, ref_top + r, ref_left);
		fetch_org_line_16(cu_width, hor_org, org_y, org_top + r - 7, org_left);

		for (int c = 0; c < (SKIP_MC_HOR_NUM + cu_width - 1); ++c){
#pragma HLS UNROLL
			S9 pred[4][4] = {};
#pragma HLS array_partition variable=pred complete dim=0

			for (int dx = 0; dx < 4; ++dx){
#pragma HLS UNROLL
				S15 hor_data = mc_8tag(dx, hor_ref[c]);
				shifting(hor_buf[c][dx], hor_data, 8);
				for (int dy = 0; dy < 4; ++dy){
#pragma HLS UNROLL
					S21 col_buf = mc_8tag(dy, hor_buf[c][dx]);
					S9 pt = (col_buf + 2048) >> 12;
					pred[dx][dy] = COM_CLIP3((S9)0, (S9)((1 << 8) - 1), pt);
				}
			}

			for (int i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
				const S8 x = c - (mode[i].mvd[MV_X] >> 2);
				const S8 y = r - 7 - (mode[i].mvd[MV_Y] >> 2);
				if (flag[i]) {
					sad[i] += (x >= 0 && x < cu_width&& y >= 0 && y < cu_height) ?
						(S32)COM_ABS16(pred[mode[i].mvd[MV_X] & 3][mode[i].mvd[MV_Y] & 3] - hor_org[x][SKIP_MC_VER_NUM - (mode[i].mvd[MV_Y] >> 2) - 1]) :
						(S32)0;
				}
				//if (pflag && i == 0 && flag[i] && x >= 0 && x < cu_width && y >= 0 && y < cu_height){
				//printf("%d %d %d\n", x, y, COM_ABS16(pred[mode[i].mvd[MV_X] & 3][mode[i].mvd[MV_Y] & 3] - hor_org[x][SKIP_MC_VER_NUM - (mode[i].mvd[MV_Y] >> 2) - 1]));
				//}
			}
		}
	}
	pflag = 0;
}

static void make_skip_sad_16(
	S32 mode_sad[SKIP_MODE_NUM],
	const SKIP_MODE_INTER mode[SKIP_MODE_NUM],
	const U12 x_pos,
	const U12 y_pos,
	const U7 cu_width,
	const U7 cu_height,
	const U12 x_chroma,
	const U12 y_chroma,
	const U1 chroma_motion,
	const U12 pic_w,
	const U12 pic_h,
	const pel ref_y[SWH * SWW],
	const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	const S12 mc_pos[SKIP_MC_NUM][MV_D]
	) {
#pragma HLS INLINE OFF
	//#pragma HLS function_instantiate variable=cu_height

	for (int mv_idx = 0; mv_idx < SKIP_MC_NUM; ++mv_idx){
		//if (cu_width == 16 && cu_height == 8)
		//{
		//	FUNC_NAME(mc_sad_l_skip, 16, 8)(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
		//}
		//else if (cu_width == 8 && cu_height == 16)
		//{
		//	FUNC_NAME(mc_sad_l_skip, 8, 16)(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
		//}
		//else if (cu_width == 16 && cu_height == 4)
		//{
		//	FUNC_NAME(mc_sad_l_skip, 16, 4)(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
		//}
		//else if (cu_width == 4 && cu_height == 16)
		//{
		//	FUNC_NAME(mc_sad_l_skip, 4, 16)(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
		//}
		//else // (cu_width == 16 && cu_height == 16)
		//{
		//	FUNC_NAME(mc_sad_l_skip, 16, 16)(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
		//}

		mc_sad_l_skip_16(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
	}
}

S2 get_ref_idx_y(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx) 
{
#pragma HLS INLINE
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			return 0;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			return 0;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	return -1;
#endif
}

S2 get_ref_idx_y_16(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			return 0;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			return 0;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	return -1;
#endif
}

S2 get_ref_idx_y_8_16(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			return 0;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			return 0;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	return -1;
#endif
}
S2 get_ref_idx_y_16_8(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			return 0;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			return 0;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	return -1;
#endif
}
S2 get_ref_idx_y_16_16(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			return 0;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			return 0;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return 1;
		}
		assert(false);
	}
	return -1;
#endif
}
pel* get_ref_y(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx)
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			return Fetch_Ref_window_ptr->RefWindowLCU;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return Fetch_Ref_window_ptr->RefWindowLCU_ref1;
		}
		assert(false);
	}else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			return Fetch_Ref_window_ptr->RefWindowLCU;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			return Fetch_Ref_window_ptr->RefWindowLCU_ref1;
		}
		assert(false);
	}
	return NULL;
#endif
}

pel* get_ref_u(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx, pel RefWindowLCU_UP[(SWH * SWW) >> 2])
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_UP[i] = Fetch_Ref_window_ptr->RefWindowLCU_UP[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_UP;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_UP[i] = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_UP[i] = Fetch_Ref_window_ptr->RefWindowLCU_UP[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_UP;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_UP[i] = Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_UP_ref1;
		}
		assert(false);
	}
	return NULL;
#endif
}

pel* get_ref_v(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx, pel RefWindowLCU_VP[(SWH * SWW) >> 2])
{
#if ENABLE_BFRAME
	if (Fetch_Ref_window_ptr->code_stru == RA)
	{
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_0] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_VP[i] = Fetch_Ref_window_ptr->RefWindowLCU_VP[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_VP;
		}
		if (lidx == Fetch_Ref_window_ptr->lidx[REFP_1] && refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_VP[i] = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1;
		}
		assert(false);
	}
	else
	{
		if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_0])
		{
#endif
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_VP[i] = Fetch_Ref_window_ptr->RefWindowLCU_VP[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_VP;
#if ENABLE_BFRAME
		}
		else if (refi == Fetch_Ref_window_ptr->cur_refi[REFP_1])
		{
			for (int i = 0; i < 12288; i++) {
				RefWindowLCU_VP[i] = Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1[i];
			}
			return Fetch_Ref_window_ptr->RefWindowLCU_VP_ref1;
		}
		assert(false);
	}
	return NULL;
#endif
}

void check_skip_mv_inbound_8_16(SKIP_ELEMENT* mode, int x_pos, int y_pos, S14 CtrPos_final[RANGE_NUM][DIR_DIM][MAP_DIMS], int cu_width, int cu_height, MD_COM_MODE* mod_info_curr
#if ENABLE_BFRAME
	, S14 CtrPos_final_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window_16* Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
)
{
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
		if (mode[i].valid && (mode[i].refi[0] >= 0
#if ENABLE_BFRAME
			|| mode[i].refi[1] >= 0
#endif // ENABLE_BFRAME
			))
		{
#if !ENABLE_BFRAME
			int gmv_x = (x_pos << 2) + mode[i].mvp[REFP_0][MV_X];
			int gmv_y = (y_pos << 2) + mode[i].mvp[REFP_0][MV_Y];
			if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
			{
				mode[i].valid = 0;
			}
#else
			int gmv_x = (x_pos << 2) + mode[i].mvp[REFP_0][MV_X];
			int gmv_y = (y_pos << 2) + mode[i].mvp[REFP_0][MV_Y];
			if (REFI_IS_VALID(mode[i].refi[0])) {
				S2 cur_refw_idx = get_ref_idx_y_8_16(Fetch_Ref_window_ptr, mode[i].refi[REFP_0], REFP_0);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
				if (cur_refw_idx == 0)
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y,  CtrPos_final, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				else
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y,  CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}

				}
			}
			gmv_x = (x_pos << 2) + mode[i].mvp[REFP_1][MV_X];
			gmv_y = (y_pos << 2) + mode[i].mvp[REFP_1][MV_Y];
			if (REFI_IS_VALID(mode[i].refi[1])) {
				S2 cur_refw_idx = get_ref_idx_y_8_16(Fetch_Ref_window_ptr, mode[i].refi[REFP_1], REFP_1);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
				if (cur_refw_idx == 0)
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final , cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				else
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				
			}
#endif // !ENABLE_BFRAME
		}
	}
}
void check_skip_mv_inbound_16_8(SKIP_ELEMENT* mode, int x_pos, int y_pos, S14 CtrPos_final[RANGE_NUM][DIR_DIM][MAP_DIMS], int cu_width, int cu_height, MD_COM_MODE* mod_info_curr
#if ENABLE_BFRAME
	, S14 CtrPos_final_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window_16* Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
)
{
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
		if (mode[i].valid && (mode[i].refi[0] >= 0
#if ENABLE_BFRAME
			|| mode[i].refi[1] >= 0
#endif // ENABLE_BFRAME
			))
		{
#if !ENABLE_BFRAME
			int gmv_x = (x_pos << 2) + mode[i].mvp[REFP_0][MV_X];
			int gmv_y = (y_pos << 2) + mode[i].mvp[REFP_0][MV_Y];
			if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
			{
				mode[i].valid = 0;
			}
#else
			int gmv_x = (x_pos << 2) + mode[i].mvp[REFP_0][MV_X];
			int gmv_y = (y_pos << 2) + mode[i].mvp[REFP_0][MV_Y];
			if (REFI_IS_VALID(mode[i].refi[0])) {
				S2 cur_refw_idx = get_ref_idx_y_16_8(Fetch_Ref_window_ptr, mode[i].refi[REFP_0], REFP_0);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
				if (cur_refw_idx == 0)
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final , cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				else
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y,  CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				
			}
			gmv_x = (x_pos << 2) + mode[i].mvp[REFP_1][MV_X];
			gmv_y = (y_pos << 2) + mode[i].mvp[REFP_1][MV_Y];
			if (REFI_IS_VALID(mode[i].refi[1])) {
				S2 cur_refw_idx = get_ref_idx_y_16_8(Fetch_Ref_window_ptr, mode[i].refi[REFP_1], REFP_1);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
				if (cur_refw_idx == 0)
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final , cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				else
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y,  CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}

				
			}
#endif // !ENABLE_BFRAME
		}
	}
}
void check_skip_mv_inbound_16_16(SKIP_ELEMENT* mode, int x_pos, int y_pos, S14 CtrPos_final[RANGE_NUM][DIR_DIM][MAP_DIMS], int cu_width, int cu_height, MD_COM_MODE* mod_info_curr
#if ENABLE_BFRAME
	, S14 CtrPos_final_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window_16* Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
)
{
	int i_tmp = 0;
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
#pragma HLS UNROLL
			if (mode[i_tmp].valid && (mode[i_tmp].refi[0] >= 0
#if ENABLE_BFRAME
				|| mode[i_tmp].refi[1] >= 0
#endif // ENABLE_BFRAME
				))
		{
#if !ENABLE_BFRAME
			int gmv_x = (x_pos << 2) + mode[i].mvp[REFP_0][MV_X];
			int gmv_y = (y_pos << 2) + mode[i].mvp[REFP_0][MV_Y];
			if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
			{
				mode[i].valid = 0;
			}
#else
			int gmv_x = (x_pos << 2) + mode[i].mvp[REFP_0][MV_X];
			int gmv_y = (y_pos << 2) + mode[i].mvp[REFP_0][MV_Y];
			if (REFI_IS_VALID(mode[i].refi[0])) {
				S2 cur_refw_idx = get_ref_idx_y_16_16(Fetch_Ref_window_ptr, mode[i].refi[REFP_0], REFP_0);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
			/*	if (!mv_inbound_new_skip(gmv_x, gmv_y, cur_refw_idx == 0 ? CtrPos_final : CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
				{
					mode[i].valid = 0;
				}*/
				if (cur_refw_idx == 0) {
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				else
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
			}
			gmv_x = (x_pos << 2) + mode[i].mvp[REFP_1][MV_X];
			gmv_y = (y_pos << 2) + mode[i].mvp[REFP_1][MV_Y];
			if (REFI_IS_VALID(mode[i].refi[1])) {
				S2 cur_refw_idx = get_ref_idx_y_16_16(Fetch_Ref_window_ptr, mode[i].refi[REFP_1], REFP_1);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
			/*	if (!mv_inbound_new_skip(gmv_x, gmv_y, cur_refw_idx == 0 ? CtrPos_final : CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
				{
					mode[i].valid = 0;
				}*/
				if (cur_refw_idx == 0) {
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
				else
				{
					if (!mv_inbound_new_skip(gmv_x, gmv_y, CtrPos_final_ref1, cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
					{
						mode[i].valid = 0;
					}
				}
			}
#endif // !ENABLE_BFRAME
		}
			i_tmp++;
	}
}

#if USE_ENH
void derive_inter_cands_8_16(SKIP_MERGE_INPUT* input, U4* num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, int cu_width, int cu_height, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	if (!run_direct_skip) {
		return;
	}

#if SUB_TMVP
	// scu position data
	static int sb_scup[SBTMVP_NUM][MV_D];
	static U4 co_x_scu_of_ctu[SBTMVP_NUM];
	static U4 co_y_scu_of_ctu[SBTMVP_NUM];
	//	int sb_scup_co[SBTMVP_NUM];
	int x_scup = x_scu;
	int y_scup = y_scu;

	for (int p = 0; p < SBTMVP_NUM; p++)
	{
		//sb_scup[p] = (x_scup + y_scup * pic_width_in_scu) + pic_width_in_scu * ((cu_height >> 2) - 1) * (p / 2) + ((cu_width >> 2) - 1) * (p % 2);

		sb_scup[p][MV_X] = x_scup + ((cu_width >> 2) - 1) * (p % 2);
		sb_scup[p][MV_Y] = y_scup + ((cu_height >> 2) - 1) * (p / 2);

		co_x_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_X], pic_width_in_scu);
		co_y_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_Y], pic_height_in_scu);

		//sb_scup_co[p] = get_colocal_scup_sbtmvp(sb_scup[p], pic_width_in_scu, pic_height_in_scu);
	}
#endif

	static SKIP_ELEMENT tmvp = {};
	static SKIP_ELEMENT spat[3] = {};

	// insert TMVP
	derive_tmvp_mode(input, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, &tmvp, neb, neb_map);
	skip_element_copy(mode, &tmvp);

#if SUB_TMVP
	if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
		if (input->slice_type == SLICE_P)
		{
			core_sbtmvp->isValid = TRUE;
			for (int p = 0; p < SBTMVP_NUM; p++)
			{
				if (
					REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu[p]][co_x_scu_of_ctu[p]][REFP_0])
					)
				{
					get_col_mv_from_list0_ext(input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p], core_sbtmvp->sbTmvp[p].mv, core_sbtmvp->sbTmvp[p].ref_idx);
					//get_col_mv_from_list0(core_sbtmvp->sbTmvp[p].mv[REFP_0], input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p]);

				}
				else
				{
					copy_mv(core_sbtmvp->sbTmvp[p].mv[REFP_0], mode[0].mvp[REFP_0]);
				}
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_X] = 0;
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_Y] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_0] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_1] = -1;
			}
		}	
#if ENABLE_BFRAME
		else
		{
			core_sbtmvp->isValid = FALSE;
		}
#endif
	}
#endif

	// insert list_01, list_1, list_0
	derive_spatial_mode(neb, neb_map, spat);
	if (input->slice_type == SLICE_P)
	{
		spat[0].valid = spat[1].valid = FALSE;
	}
	skip_element_copy(mode + 1, spat);
	skip_element_copy(mode + 2, spat + 1);
	skip_element_copy(mode + 3, spat + 2);

	// insert UMVE
	*num_cands_woUMVE = PRED_DIR_NUM + 1;

#if !ENABLE_BFRAME
	assert(input->umve_enable_flag);
#endif
	if (input->umve_enable_flag)
	{
		derive_umve_mode(neb, neb_map, mode + 4 + SKIP_HMVP_NUM, &tmvp, input);
	}

#if ENABLE_BFRAME
	if (input->slice_type == SLICE_B)
	{
		for (int i = 0; i < SKIP_MODE_NUM; ++i)
		{
			if (mode[i].valid)
			{
				check_refi_8_16(mode[i].refi, Fetch_Ref_window_ptr);
			}
		}
	}
#endif

	//	print_inter_cands(mode);
}
void derive_inter_cands_16_8(SKIP_MERGE_INPUT* input, U4* num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, int cu_width, int cu_height, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	if (!run_direct_skip) {
		return;
	}

#if SUB_TMVP
	// scu position data
	static int sb_scup[SBTMVP_NUM][MV_D];
	static U4 co_x_scu_of_ctu[SBTMVP_NUM];
	static U4 co_y_scu_of_ctu[SBTMVP_NUM];
	//	int sb_scup_co[SBTMVP_NUM];
	int x_scup = x_scu;
	int y_scup = y_scu;

	for (int p = 0; p < SBTMVP_NUM; p++)
	{
		//sb_scup[p] = (x_scup + y_scup * pic_width_in_scu) + pic_width_in_scu * ((cu_height >> 2) - 1) * (p / 2) + ((cu_width >> 2) - 1) * (p % 2);

		sb_scup[p][MV_X] = x_scup + ((cu_width >> 2) - 1) * (p % 2);
		sb_scup[p][MV_Y] = y_scup + ((cu_height >> 2) - 1) * (p / 2);

		co_x_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_X], pic_width_in_scu);
		co_y_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_Y], pic_height_in_scu);

		//sb_scup_co[p] = get_colocal_scup_sbtmvp(sb_scup[p], pic_width_in_scu, pic_height_in_scu);
	}
#endif

	static SKIP_ELEMENT tmvp = {};
	static SKIP_ELEMENT spat[3] = {};

	// insert TMVP
	derive_tmvp_mode(input, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, &tmvp, neb, neb_map);
	skip_element_copy(mode, &tmvp);

#if SUB_TMVP
	if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
		if (input->slice_type == SLICE_P)
		{
			core_sbtmvp->isValid = TRUE;
			for (int p = 0; p < SBTMVP_NUM; p++)
			{
				if (
					REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu[p]][co_x_scu_of_ctu[p]][REFP_0])
					)
				{
					get_col_mv_from_list0_ext(input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p], core_sbtmvp->sbTmvp[p].mv, core_sbtmvp->sbTmvp[p].ref_idx);
					//get_col_mv_from_list0(core_sbtmvp->sbTmvp[p].mv[REFP_0], input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p]);

				}
				else
				{
					copy_mv(core_sbtmvp->sbTmvp[p].mv[REFP_0], mode[0].mvp[REFP_0]);
				}
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_X] = 0;
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_Y] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_0] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_1] = -1;
			}
		}
#if ENABLE_BFRAME
		else
		{
			core_sbtmvp->isValid = FALSE;
		}
#endif
	}
#endif

	// insert list_01, list_1, list_0
	derive_spatial_mode(neb, neb_map, spat);
	if (input->slice_type == SLICE_P)
	{
		spat[0].valid = spat[1].valid = FALSE;
	}
	skip_element_copy(mode + 1, spat);
	skip_element_copy(mode + 2, spat + 1);
	skip_element_copy(mode + 3, spat + 2);

	// insert UMVE
	*num_cands_woUMVE = PRED_DIR_NUM + 1;

#if !ENABLE_BFRAME
	assert(input->umve_enable_flag);
#endif
	if (input->umve_enable_flag)
	{
		derive_umve_mode(neb, neb_map, mode + 4 + SKIP_HMVP_NUM, &tmvp, input);
	}

#if ENABLE_BFRAME
	if (input->slice_type == SLICE_B)
	{
		for (int i = 0; i < SKIP_MODE_NUM; ++i)
		{
			if (mode[i].valid)
			{
				check_refi_16_8(mode[i].refi, Fetch_Ref_window_ptr);
			}
		}
	}
#endif

	//	print_inter_cands(mode);
}
void derive_inter_cands_16_16(SKIP_MERGE_INPUT* input, U4* num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, int cu_width, int cu_height, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	if (!run_direct_skip) {
		return;
	}

#if SUB_TMVP
	// scu position data
	static int sb_scup[SBTMVP_NUM][MV_D];
	static U4 co_x_scu_of_ctu[SBTMVP_NUM];
	static U4 co_y_scu_of_ctu[SBTMVP_NUM];
	//	int sb_scup_co[SBTMVP_NUM];
	int x_scup = x_scu;
	int y_scup = y_scu;

	for (int p = 0; p < SBTMVP_NUM; p++)
	{
		//sb_scup[p] = (x_scup + y_scup * pic_width_in_scu) + pic_width_in_scu * ((cu_height >> 2) - 1) * (p / 2) + ((cu_width >> 2) - 1) * (p % 2);

		sb_scup[p][MV_X] = x_scup + ((cu_width >> 2) - 1) * (p % 2);
		sb_scup[p][MV_Y] = y_scup + ((cu_height >> 2) - 1) * (p / 2);

		co_x_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_X], pic_width_in_scu);
		co_y_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_Y], pic_height_in_scu);

		//sb_scup_co[p] = get_colocal_scup_sbtmvp(sb_scup[p], pic_width_in_scu, pic_height_in_scu);
	}
#endif

	static SKIP_ELEMENT tmvp = {};
	static SKIP_ELEMENT spat[3] = {};

	// insert TMVP
	derive_tmvp_mode(input, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, &tmvp, neb, neb_map);
	skip_element_copy(mode, &tmvp);

#if SUB_TMVP
	if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
		if (input->slice_type == SLICE_P)
		{
			core_sbtmvp->isValid = TRUE;
			for (int p = 0; p < SBTMVP_NUM; p++)
			{
				if (
					REFI_IS_VALID(input->refp_map_refi1[REFP_0][co_y_scu_of_ctu[p]][co_x_scu_of_ctu[p]][REFP_0])
					)
				{
					get_col_mv_from_list0_ext(input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p], core_sbtmvp->sbTmvp[p].mv, core_sbtmvp->sbTmvp[p].ref_idx);
					//get_col_mv_from_list0(core_sbtmvp->sbTmvp[p].mv[REFP_0], input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p]);

				}
				else
				{
					copy_mv(core_sbtmvp->sbTmvp[p].mv[REFP_0], mode[0].mvp[REFP_0]);
				}
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_X] = 0;
				core_sbtmvp->sbTmvp[p].mv[REFP_1][MV_Y] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_0] = 0;
				core_sbtmvp->sbTmvp[p].ref_idx[REFP_1] = -1;
			}
		}
#if ENABLE_BFRAME
		else
		{
			core_sbtmvp->isValid = FALSE;
		}
#endif
	}
#endif

	// insert list_01, list_1, list_0
	derive_spatial_mode(neb, neb_map, spat);
	if (input->slice_type == SLICE_P)
	{
		spat[0].valid = spat[1].valid = FALSE;
	}
	skip_element_copy(mode + 1, spat);
	skip_element_copy(mode + 2, spat + 1);
	skip_element_copy(mode + 3, spat + 2);

	// insert UMVE
	*num_cands_woUMVE = PRED_DIR_NUM + 1;

#if !ENABLE_BFRAME
	assert(input->umve_enable_flag);
#endif
	if (input->umve_enable_flag)
	{
		derive_umve_mode(neb, neb_map, mode + 4 + SKIP_HMVP_NUM, &tmvp, input);
	}

#if ENABLE_BFRAME
	if (input->slice_type == SLICE_B)
	{
		for (int i = 0; i < SKIP_MODE_NUM; ++i)
		{
			if (mode[i].valid)
			{
				check_refi_16_16(mode[i].refi, Fetch_Ref_window_ptr);
			}
		}
	}
#endif

	//	print_inter_cands(mode);
}
#else
void derive_inter_cands_16(SKIP_MERGE_INPUT_16* skip_in_16,
	U4* num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window_16* Fetch_Ref_window_ptr)
{
	if (!run_direct_skip) {
		return;
	}

	static SKIP_ELEMENT tmvp = {};
	static SKIP_ELEMENT spat[3] = {};

	// insert TMVP
	derive_tmvp_mode_16(skip_in_16, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, &tmvp, neb, neb_map);
	skip_element_copy(mode, &tmvp);


	// insert list_01, list_1, list_0
	derive_spatial_mode(neb, neb_map, spat);
	if (skip_in_16->slice_type == SLICE_P)
	{
		spat[0].valid = spat[1].valid = FALSE;
	}
	skip_element_copy(mode + 1, spat);
	skip_element_copy(mode + 2, spat + 1);
	skip_element_copy(mode + 3, spat + 2);

	// insert UMVE
	*num_cands_woUMVE = PRED_DIR_NUM + 1;

#if !ENABLE_BFRAME
	assert(input->umve_enable_flag);
#endif
	if (skip_in_16->umve_enable_flag)
	{
		derive_umve_mode_16(neb, neb_map, mode + 4 + SKIP_HMVP_NUM, &tmvp, skip_in_16);
	}

#if ENABLE_BFRAME
	if (skip_in_16->slice_type == SLICE_B)
	{
		for (int i = 0; i < SKIP_MODE_NUM; ++i)
		{
			if (mode[i].valid)
			{
				check_refi_16(mode[i].refi, Fetch_Ref_window_ptr);
			}
		}
	}
#endif

	//	print_inter_cands(mode);
}

#endif

void derive_neb_cu_8_16(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height,
	NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV],
	const U32 map_scu_up_line[6], const U32 map_cu_mode_up_line[6],
	const U32 map_scu_left_line[4], const U32 map_cu_mode_left_line[4],
	const U1 curr_ctu_idx,
	const ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], const ENC_ME_NEB_INFO left_nebs[(16 >> 2)])
{
#pragma HLS INLINE OFF

	U5 cu_width_in_scu = cu_width >> MIN_CU_LOG2;
	U5 cu_height_in_scu = cu_height >> MIN_CU_LOG2;

	U5 x_scu_of_ctu = x_scu & (MAX_CU_IN_SCU - 1);
	U5 y_scu_of_ctu = y_scu & (MAX_CU_IN_SCU - 1);

	neb[0].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[0].y_scu_of_ctu = y_scu_of_ctu + cu_height_in_scu - 1;
	neb[1].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu - 1;
	neb[1].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[2].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu;
	neb[2].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[3].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[3].y_scu_of_ctu = y_scu_of_ctu;
	neb[4].x_scu_of_ctu = x_scu_of_ctu;
	neb[4].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[5].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[5].y_scu_of_ctu = y_scu_of_ctu - 1;


	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; i++) {
#pragma HLS UNROLL
		S6 x = neb[i].x_scu_of_ctu;
		S6 y = neb[i].y_scu_of_ctu;
#if MD_PRINT
		assert(x >= -1 && x <= MAX_CU_IN_SCU && y >= -1 && y < MAX_CU_IN_SCU);
#endif
		if (neb[i].y_scu_of_ctu == -1) { // top, top left, top right
			neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_X];
			neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_Y];
			neb_map[i].map_refi[REFP_0] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_0];
			neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_X];
			neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_Y];
			neb_map[i].map_refi[REFP_1] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_1];
			neb_map[i].map_scu = map_scu_up_line[x - x_scu_of_ctu + 1];
			neb_map[i].map_cu_mode = map_cu_mode_up_line[x - x_scu_of_ctu + 1];
		}
		else if (x == MAX_CU_IN_SCU) { // right
			neb_map[i].map_mv[REFP_0][MV_X] = 0;
			neb_map[i].map_mv[REFP_0][MV_Y] = 0;
			neb_map[i].map_refi[REFP_0] = REFI_INVALID;
			neb_map[i].map_mv[REFP_1][MV_X] = 0;
			neb_map[i].map_mv[REFP_1][MV_Y] = 0;
			neb_map[i].map_refi[REFP_1] = REFI_INVALID;
			neb_map[i].map_scu = 0;
			neb_map[i].map_cu_mode = 0;
		}
		else if (x == -1) { // left
			if (i == 5)
			{
				neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[0].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[0].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = up_nebs[0].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[0].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[0].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = up_nebs[0].refi[REFP_1];
				neb_map[i].map_scu = map_scu_up_line[0];
				neb_map[i].map_cu_mode = map_cu_mode_up_line[0];
			}
			else
			{
				neb_map[i].map_mv[REFP_0][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = left_nebs[y - y_scu_of_ctu].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = left_nebs[y - y_scu_of_ctu].refi[REFP_1];
				neb_map[i].map_scu = map_scu_left_line[y - y_scu_of_ctu];
				neb_map[i].map_cu_mode = map_cu_mode_left_line[y - y_scu_of_ctu];
			}
		}
		else {
			if (i == 0 || i == 3)
			{
				neb_map[i].map_mv[REFP_0][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = left_nebs[y - y_scu_of_ctu].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = left_nebs[y - y_scu_of_ctu].refi[REFP_1];
				neb_map[i].map_scu = map_scu_left_line[y - y_scu_of_ctu];
				neb_map[i].map_cu_mode = map_cu_mode_left_line[y - y_scu_of_ctu];
			}
			else
			{
				neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_1];
				neb_map[i].map_scu = map_scu_up_line[x - x_scu_of_ctu + 1];
				neb_map[i].map_cu_mode = map_cu_mode_up_line[x - x_scu_of_ctu + 1];
			}
		}
	}

	neb[0].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[0].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[0].map_scu);
	neb[1].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[1].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[1].map_scu);
	neb[2].valid_flag = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu&& MCU_GET_CODED_FLAG(neb_map[2].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[2].map_scu);
	neb[3].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[3].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[3].map_scu);
	neb[4].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[4].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[4].map_scu);
	neb[5].valid_flag = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[5].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[5].map_scu);
}
void derive_neb_cu_16_8(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height,
	NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV],
	const U32 map_scu_up_line[6], const U32 map_cu_mode_up_line[6],
	const U32 map_scu_left_line[4], const U32 map_cu_mode_left_line[4],
	const U1 curr_ctu_idx,
	const ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], const ENC_ME_NEB_INFO left_nebs[(16 >> 2)])
{
#pragma HLS INLINE OFF

	U5 cu_width_in_scu = cu_width >> MIN_CU_LOG2;
	U5 cu_height_in_scu = cu_height >> MIN_CU_LOG2;

	U5 x_scu_of_ctu = x_scu & (MAX_CU_IN_SCU - 1);
	U5 y_scu_of_ctu = y_scu & (MAX_CU_IN_SCU - 1);

	neb[0].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[0].y_scu_of_ctu = y_scu_of_ctu + cu_height_in_scu - 1;
	neb[1].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu - 1;
	neb[1].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[2].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu;
	neb[2].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[3].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[3].y_scu_of_ctu = y_scu_of_ctu;
	neb[4].x_scu_of_ctu = x_scu_of_ctu;
	neb[4].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[5].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[5].y_scu_of_ctu = y_scu_of_ctu - 1;


	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; i++) {
#pragma HLS UNROLL
		S6 x = neb[i].x_scu_of_ctu;
		S6 y = neb[i].y_scu_of_ctu;
#if MD_PRINT
		assert(x >= -1 && x <= MAX_CU_IN_SCU && y >= -1 && y < MAX_CU_IN_SCU);
#endif
		if (neb[i].y_scu_of_ctu == -1) { // top, top left, top right
			neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_X];
			neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_Y];
			neb_map[i].map_refi[REFP_0] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_0];
			neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_X];
			neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_Y];
			neb_map[i].map_refi[REFP_1] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_1];
			neb_map[i].map_scu = map_scu_up_line[x - x_scu_of_ctu + 1];
			neb_map[i].map_cu_mode = map_cu_mode_up_line[x - x_scu_of_ctu + 1];
		}
		else if (x == MAX_CU_IN_SCU) { // right
			neb_map[i].map_mv[REFP_0][MV_X] = 0;
			neb_map[i].map_mv[REFP_0][MV_Y] = 0;
			neb_map[i].map_refi[REFP_0] = REFI_INVALID;
			neb_map[i].map_mv[REFP_1][MV_X] = 0;
			neb_map[i].map_mv[REFP_1][MV_Y] = 0;
			neb_map[i].map_refi[REFP_1] = REFI_INVALID;
			neb_map[i].map_scu = 0;
			neb_map[i].map_cu_mode = 0;
		}
		else if (x == -1) { // left
			if (i == 5)
			{
				neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[0].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[0].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = up_nebs[0].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[0].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[0].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = up_nebs[0].refi[REFP_1];
				neb_map[i].map_scu = map_scu_up_line[0];
				neb_map[i].map_cu_mode = map_cu_mode_up_line[0];
			}
			else
			{
				neb_map[i].map_mv[REFP_0][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = left_nebs[y - y_scu_of_ctu].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = left_nebs[y - y_scu_of_ctu].refi[REFP_1];
				neb_map[i].map_scu = map_scu_left_line[y - y_scu_of_ctu];
				neb_map[i].map_cu_mode = map_cu_mode_left_line[y - y_scu_of_ctu];
			}
		}
		else {
			if (i == 0 || i == 3)
			{
				neb_map[i].map_mv[REFP_0][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = left_nebs[y - y_scu_of_ctu].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = left_nebs[y - y_scu_of_ctu].refi[REFP_1];
				neb_map[i].map_scu = map_scu_left_line[y - y_scu_of_ctu];
				neb_map[i].map_cu_mode = map_cu_mode_left_line[y - y_scu_of_ctu];
			}
			else
			{
				neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_1];
				neb_map[i].map_scu = map_scu_up_line[x - x_scu_of_ctu + 1];
				neb_map[i].map_cu_mode = map_cu_mode_up_line[x - x_scu_of_ctu + 1];
			}
		}
	}

	neb[0].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[0].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[0].map_scu);
	neb[1].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[1].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[1].map_scu);
	neb[2].valid_flag = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu&& MCU_GET_CODED_FLAG(neb_map[2].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[2].map_scu);
	neb[3].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[3].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[3].map_scu);
	neb[4].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[4].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[4].map_scu);
	neb[5].valid_flag = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[5].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[5].map_scu);
}
void derive_neb_cu_16_16(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height,
	NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV],
	const U32 map_scu_up_line[6], const U32 map_cu_mode_up_line[6],
	const U32 map_scu_left_line[4], const U32 map_cu_mode_left_line[4],
	const U1 curr_ctu_idx,
	const ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], const ENC_ME_NEB_INFO left_nebs[(16 >> 2)])
{
#pragma HLS INLINE OFF

	U5 cu_width_in_scu = cu_width >> MIN_CU_LOG2;
	U5 cu_height_in_scu = cu_height >> MIN_CU_LOG2;

	U5 x_scu_of_ctu = x_scu & (MAX_CU_IN_SCU - 1);
	U5 y_scu_of_ctu = y_scu & (MAX_CU_IN_SCU - 1);

	neb[0].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[0].y_scu_of_ctu = y_scu_of_ctu + cu_height_in_scu - 1;
	neb[1].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu - 1;
	neb[1].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[2].x_scu_of_ctu = x_scu_of_ctu + cu_width_in_scu;
	neb[2].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[3].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[3].y_scu_of_ctu = y_scu_of_ctu;
	neb[4].x_scu_of_ctu = x_scu_of_ctu;
	neb[4].y_scu_of_ctu = y_scu_of_ctu - 1;
	neb[5].x_scu_of_ctu = x_scu_of_ctu - 1;
	neb[5].y_scu_of_ctu = y_scu_of_ctu - 1;


	for (int i = 0; i < NUM_SKIP_SPATIAL_MV; i++) {
#pragma HLS UNROLL
		S6 x = neb[i].x_scu_of_ctu;
		S6 y = neb[i].y_scu_of_ctu;
#if MD_PRINT
		assert(x >= -1 && x <= MAX_CU_IN_SCU && y >= -1 && y < MAX_CU_IN_SCU);
#endif
		if (neb[i].y_scu_of_ctu == -1) { // top, top left, top right
			neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_X];
			neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_Y];
			neb_map[i].map_refi[REFP_0] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_0];
			neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_X];
			neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_Y];
			neb_map[i].map_refi[REFP_1] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_1];
			neb_map[i].map_scu = map_scu_up_line[x - x_scu_of_ctu + 1];
			neb_map[i].map_cu_mode = map_cu_mode_up_line[x - x_scu_of_ctu + 1];
		}
		else if (x == MAX_CU_IN_SCU) { // right
			neb_map[i].map_mv[REFP_0][MV_X] = 0;
			neb_map[i].map_mv[REFP_0][MV_Y] = 0;
			neb_map[i].map_refi[REFP_0] = REFI_INVALID;
			neb_map[i].map_mv[REFP_1][MV_X] = 0;
			neb_map[i].map_mv[REFP_1][MV_Y] = 0;
			neb_map[i].map_refi[REFP_1] = REFI_INVALID;
			neb_map[i].map_scu = 0;
			neb_map[i].map_cu_mode = 0;
		}
		else if (x == -1) { // left
			if (i == 5)
			{
				neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[0].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[0].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = up_nebs[0].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[0].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[0].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = up_nebs[0].refi[REFP_1];
				neb_map[i].map_scu = map_scu_up_line[0];
				neb_map[i].map_cu_mode = map_cu_mode_up_line[0];
			}
			else
			{
				neb_map[i].map_mv[REFP_0][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = left_nebs[y - y_scu_of_ctu].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = left_nebs[y - y_scu_of_ctu].refi[REFP_1];
				neb_map[i].map_scu = map_scu_left_line[y - y_scu_of_ctu];
				neb_map[i].map_cu_mode = map_cu_mode_left_line[y - y_scu_of_ctu];
			}
		}
		else {
			if (i == 0 || i == 3)
			{
				neb_map[i].map_mv[REFP_0][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = left_nebs[y - y_scu_of_ctu].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = left_nebs[y - y_scu_of_ctu].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = left_nebs[y - y_scu_of_ctu].refi[REFP_1];
				neb_map[i].map_scu = map_scu_left_line[y - y_scu_of_ctu];
				neb_map[i].map_cu_mode = map_cu_mode_left_line[y - y_scu_of_ctu];
			}
			else
			{
				neb_map[i].map_mv[REFP_0][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_X];
				neb_map[i].map_mv[REFP_0][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_0][MV_Y];
				neb_map[i].map_refi[REFP_0] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_0];
				neb_map[i].map_mv[REFP_1][MV_X] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_X];
				neb_map[i].map_mv[REFP_1][MV_Y] = up_nebs[x - x_scu_of_ctu + 1].mv[REFP_1][MV_Y];
				neb_map[i].map_refi[REFP_1] = up_nebs[x - x_scu_of_ctu + 1].refi[REFP_1];
				neb_map[i].map_scu = map_scu_up_line[x - x_scu_of_ctu + 1];
				neb_map[i].map_cu_mode = map_cu_mode_up_line[x - x_scu_of_ctu + 1];
			}
		}
	}

	neb[0].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[0].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[0].map_scu);
	neb[1].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[1].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[1].map_scu);
	neb[2].valid_flag = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu&& MCU_GET_CODED_FLAG(neb_map[2].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[2].map_scu);
	neb[3].valid_flag = x_scu > 0 && MCU_GET_CODED_FLAG(neb_map[3].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[3].map_scu);
	neb[4].valid_flag = y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[4].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[4].map_scu);
	neb[5].valid_flag = x_scu > 0 && y_scu > 0 && MCU_GET_CODED_FLAG(neb_map[5].map_scu) && !MCU_GET_INTRA_FLAG(neb_map[5].map_scu);
}

static void get_mvp_default_line_8_16(MD_FW* md_fw_ptr, 
	ENC_ME_LINE_MAP_SIZE<16>* me_line_ptr_16,
	MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2], 
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)])
{
#pragma HLS inline off
	U23 scup = mod_info_curr->scup;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U13 pic_w = md_fw_ptr->pic_width;
	U13 pic_h = md_fw_ptr->pic_height;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 h_scu = md_fw_ptr->pic_height_in_scu;
	//s16(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
	//s8(*map_refi)[REFP_NUM] = pic_map->map_refi;
	//u32* map_scu = pic_map->map_scu;
	int x_scu_left = 0;
	int cnt;
	int hv;
	int ptr_cur_ref;
	int mvPredType = 3;
	s8 rFrameL;
	s8 rFrameU;
	s8 rFrameUR;
	int neb_addr[3 + 1];
	int valid_flag[3 + 1];
	s32 refi[3];
	s32 MVPs[3][2];
	int x_scu = mod_info_curr->x_scu;
	int y_scu = mod_info_curr->y_scu;
	int cu_width_in_scu = cu_width >> 2;
	int offset_in_line = 0;
	//check_mvp_motion_availability(info, mod_info_curr, pic_map, neb_addr, valid_flag, lidx);
	int i = 0;
	int use_line = 0;
	int debug = 0;
	s16 x_scu_line[5 - 1];
	s16 y_scu_line[5 - 1];
	int d_inner = 1;
	S2 refi_nebs[NEB_NUMS];
	//u32 scu_nebs[NEB_NUMS];
	s32 mv_nebs[NEB_NUMS][MV_D];

	// A
	neb_addr[0] = scup - 1;
	// B
	neb_addr[1] = scup - pic_width_in_scu;
	// C
	neb_addr[2] = scup - pic_width_in_scu + cu_width_in_scu;
	// D
	neb_addr[3] = scup - pic_width_in_scu - 1;
	valid_flag[0] = x_scu > 0;
	valid_flag[1] = y_scu > 0;
	valid_flag[2] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu;
	valid_flag[3] = x_scu > 0 && y_scu > 0;
	x_scu_line[0] = x_scu - me_line_ptr_16->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_ptr_16->min_scu_y;
	x_scu_line[1] = x_scu - me_line_ptr_16->min_scu_x;
	y_scu_line[1] = y_scu - me_line_ptr_16->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_ptr_16->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_ptr_16->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_ptr_16->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_ptr_16->min_scu_y - 1;
	if (x_scu == me_line_ptr_16->min_scu_x) {
		offset_in_line = y_scu - me_line_ptr_16->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			//assert(me_line_ptr_16->map_scu_left_line[0] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]]);
			valid_flag[0] = ((int)(me_line_ptr_16->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr_16->map_scu_left_line[0] >> 15 & ((
					unsigned int)1)))
				&& ((int)left_nebs[0].refi[lidx]) >= 0;
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->left_nebs[neb_addr[0]].refi[lidx]);
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] = left_nebs[0].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_ptr_16->min_scu_y) {
			offset_in_line = y_scu - me_line_ptr_16->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = up_nebs[0].refi[lidx];
				//assert(me_line_ptr_16->map_scu_up_line[0] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]]);
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_ptr_16->map_scu_up_line[0] >> 31 & ((
						unsigned int)1)))
					&& !((int)(me_line_ptr_16->map_scu_up_line[0] >> 15 & ((
						unsigned int)1)))
					&& ((int)up_nebs[0].refi[lidx]) >= 0;
			}
			if (valid_flag[3]) {
				mv_nebs[3][0] = up_nebs[0].mv[lidx][0];
				mv_nebs[3][1] = up_nebs[0].mv[lidx][1];
			}
			d_inner = 0;
		}
	}
	else {
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(map_scu[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[0]);
			//assert(me_line_ptr_16->map_scu_left_line[0] == me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]]);
			valid_flag[0] = ((int)(me_line_ptr_16->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr_16->map_scu_left_line[0] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] =
				left_nebs[0].mv[lidx][1];
		}
	}
	if (y_scu == me_line_ptr_16->min_scu_y) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
		for (i = 1; i < 3 + d_inner; i++) {
#pragma HLS loop_tripcount max = 3
#pragma HLS PIPELINE
			if (valid_flag[i]) {
				refi_nebs[i] = up_nebs[neb_addr[i] - offset_in_line + 1].refi[lidx];
				//assert(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] == me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET]);
				valid_flag[i] = valid_flag[i]
					&& ((int)(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 31 & ((unsigned int)1)))
					&& !((int)(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 15 & ((unsigned int)1)))
					&& ((int)refi_nebs[i]) >= 0;
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][0];
				mv_nebs[i][1] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][1];
			}
		}
	}
	else {
		for (i = 1; i < 3 + d_inner; i++) {
#pragma HLS loop_tripcount max = 3
#pragma HLS PIPELINE II=2
			if (valid_flag[i]) {
				if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
					valid_flag[i] = 0;
				}
				if (valid_flag[i])
				{
					refi_nebs[i] = up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].refi[lidx];
					//valid_flag[i] = valid_flag[i] && MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[i]);
					//assert(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr->min_scu_x) + 1] == me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]]);
					valid_flag[i] = valid_flag[i]
						&& ((int)(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1] >> 31 & ((
							unsigned int)1)))
						&& !((int)(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1] >> 15 & ((
							unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
				}
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] =
					up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].mv[lidx][0];
				mv_nebs[i][1] =
					up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].mv[lidx][1];
			}
			/*if (valid_flag[i]){
			if(me_line_ptr->mv_nebs[i][MV_X] != map_mv[neb_addr[i]][lidx][MV_X] || me_line_ptr->mv_nebs[i][MV_Y] != map_mv[neb_addr[i]][lidx][MV_Y])
			printf("Wrong");
			}*/
		}
	}
	if (!valid_flag[2]) {
		// D
		neb_addr[2] = neb_addr[3];
		valid_flag[2] = valid_flag[3];
		refi_nebs[2] = refi_nebs[3];
		mv_nebs[2][0] = mv_nebs[3][0];
		mv_nebs[2][1] = mv_nebs[3][1];
	}
	ptr_cur_ref = refp_ptr[cur_refi][lidx];
	for (cnt = 0; cnt < 3; cnt++) {
#pragma HLS PIPELINE
		if (valid_flag[cnt]) {
			refi[cnt] = refi_nebs[cnt];
#if MD_PRINT
			((int)refi[cnt]) >= 0 ? ((void)0) : __assert_HLS("((refi[cnt]) >= 0)", "../separate/com_util.c", (unsigned int)723,
				__FUNCTION__);
#endif
			int ptr_neb_ref = refp_ptr[refi[cnt]][lidx];
			//scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, map_mv[neb_addr[cnt]][lidx], MVPs[cnt]);
			scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, mv_nebs[cnt], MVPs[cnt]);
		}
		else {
			refi[cnt] = -1;
			MVPs[cnt][0] = 0;
			MVPs[cnt][1] = 0;
		}
	}
	rFrameL = refi[0];
	rFrameU = refi[1];
	rFrameUR = refi[2];
	if (rFrameL != -1 && rFrameU == -1 && rFrameUR == -1) {
		mvPredType = 0;
	}
	else if (rFrameL == -1 && rFrameU != -1 && rFrameUR == -1) {
		mvPredType = 1;
	}
	else if (rFrameL == -1 && rFrameU == -1 && rFrameUR != -1) {
		mvPredType = 2;
	}
	for (hv = 0; hv < 2; hv++) {
#pragma HLS PIPELINE II=2
		s16 mva = MVPs[0][hv];
		s16 mvb = MVPs[1][hv];
		s16 mvc = MVPs[2][hv];
		switch (mvPredType) {
		case 3:
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
				mvp[hv] = (mvb + mvc) / 2;
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
				mvp[hv] = (mvc + mva) / 2;
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
				mvp[hv] = (mva + mvb) / 2;
			}
			else {
				s16 mva_ext = abs(mva - mvb);
				s16 mvb_ext = abs(mvb - mvc);
				s16 mvc_ext = abs(mvc - mva);
				s16 pred_vec = mva_ext < ((mvb_ext < mvc_ext ? mvb_ext : mvc_ext)) ? mva_ext : ((mvb_ext < mvc_ext ? mvb_ext :
					mvc_ext));
				if (pred_vec == mva_ext) {
					mvp[hv] = (mva + mvb) / 2;
				}
				else if (pred_vec == mvb_ext) {
					mvp[hv] = (mvb + mvc) / 2;
				}
				else {
					mvp[hv] = (mvc + mva) / 2;
				}
			}
			break;
		case 0:
			mvp[hv] = mva;
			break;
		case 1:
			mvp[hv] = mvb;
			break;
		case 2:
			mvp[hv] = mvc;
			break;
		default:
#if MD_PRINT
			0 ? ((void)0) : __assert_HLS("0", "../separate/com_util.c", (unsigned int)800, __FUNCTION__);
#endif
			break;
		}
	}
	// clip MVP after rounding (rounding process might result in 32768)
	CPMV mvp_x;
	CPMV mvp_y;
	com_mv_rounding_s32((s32)mvp[0], (s32)mvp[1], &mvp_x, &mvp_y, (int)amvr_idx, (int)amvr_idx);
	mvp[0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
	mvp[1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
}

static void get_mvp_default_line_16_8(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_ptr_16,
	MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2],
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)])
{
#pragma HLS inline off
	U23 scup = mod_info_curr->scup;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U13 pic_w = md_fw_ptr->pic_width;
	U13 pic_h = md_fw_ptr->pic_height;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 h_scu = md_fw_ptr->pic_height_in_scu;
	//s16(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
	//s8(*map_refi)[REFP_NUM] = pic_map->map_refi;
	//u32* map_scu = pic_map->map_scu;
	int x_scu_left = 0;
	int cnt;
	int hv;
	int ptr_cur_ref;
	int mvPredType = 3;
	s8 rFrameL;
	s8 rFrameU;
	s8 rFrameUR;
	int neb_addr[3 + 1];
	int valid_flag[3 + 1];
	s8 refi[3];
	s32 MVPs[3][2];
	int x_scu = mod_info_curr->x_scu;
	int y_scu = mod_info_curr->y_scu;
	int cu_width_in_scu = cu_width >> 2;
	int offset_in_line = 0;
	//check_mvp_motion_availability(info, mod_info_curr, pic_map, neb_addr, valid_flag, lidx);
	int i = 0;
	int use_line = 0;
	int debug = 0;
	s16 x_scu_line[5 - 1];
	s16 y_scu_line[5 - 1];
	int d_inner = 1;
	S2 refi_nebs[NEB_NUMS];
	//u32 scu_nebs[NEB_NUMS];
	s32 mv_nebs[NEB_NUMS][MV_D];

	// A
	neb_addr[0] = scup - 1;
	// B
	neb_addr[1] = scup - pic_width_in_scu;
	// C
	neb_addr[2] = scup - pic_width_in_scu + cu_width_in_scu;
	// D
	neb_addr[3] = scup - pic_width_in_scu - 1;
	valid_flag[0] = x_scu > 0;
	valid_flag[1] = y_scu > 0;
	valid_flag[2] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu;
	valid_flag[3] = x_scu > 0 && y_scu > 0;
	x_scu_line[0] = x_scu - me_line_ptr_16->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_ptr_16->min_scu_y;
	x_scu_line[1] = x_scu - me_line_ptr_16->min_scu_x;
	y_scu_line[1] = y_scu - me_line_ptr_16->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_ptr_16->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_ptr_16->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_ptr_16->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_ptr_16->min_scu_y - 1;
	if (x_scu == me_line_ptr_16->min_scu_x) {
		offset_in_line = y_scu - me_line_ptr_16->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			//assert(me_line_ptr_16->map_scu_left_line[0] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]]);
			valid_flag[0] = ((int)(me_line_ptr_16->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr_16->map_scu_left_line[0] >> 15 & ((
					unsigned int)1)))
				&& ((int)left_nebs[0].refi[lidx]) >= 0;
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->left_nebs[neb_addr[0]].refi[lidx]);
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] = left_nebs[0].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_ptr_16->min_scu_y) {
			offset_in_line = y_scu - me_line_ptr_16->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = up_nebs[0].refi[lidx];
				//assert(me_line_ptr_16->map_scu_up_line[0] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]]);
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_ptr_16->map_scu_up_line[0] >> 31 & ((
						unsigned int)1)))
					&& !((int)(me_line_ptr_16->map_scu_up_line[0] >> 15 & ((
						unsigned int)1)))
					&& ((int)up_nebs[0].refi[lidx]) >= 0;
			}
			if (valid_flag[3]) {
				mv_nebs[3][0] = up_nebs[0].mv[lidx][0];
				mv_nebs[3][1] = up_nebs[0].mv[lidx][1];
			}
			d_inner = 0;
		}
	}
	else {
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(map_scu[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[0]);
			//assert(me_line_ptr_16->map_scu_left_line[0] == me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]]);
			valid_flag[0] = ((int)(me_line_ptr_16->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr_16->map_scu_left_line[0] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] =
				left_nebs[0].mv[lidx][1];
		}
	}
	if (y_scu == me_line_ptr_16->min_scu_y) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
		for (i = 1; i < 3 + d_inner; i++) {
#pragma HLS loop_tripcount max = 3
#pragma HLS PIPELINE
			if (valid_flag[i]) {
				refi_nebs[i] = up_nebs[neb_addr[i] - offset_in_line + 1].refi[lidx];
				//assert(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] == me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET]);
				valid_flag[i] = valid_flag[i]
					&& ((int)(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 31 & ((unsigned int)1)))
					&& !((int)(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 15 & ((unsigned int)1)))
					&& ((int)refi_nebs[i]) >= 0;
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][0];
				mv_nebs[i][1] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][1];
			}
		}
	}
	else {
		for (i = 1; i < 3 + d_inner; i++) {
#pragma HLS loop_tripcount max = 3
#pragma HLS PIPELINE II=2
			if (valid_flag[i]) {
				if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
					valid_flag[i] = 0;
				}
				if (valid_flag[i])
				{
					refi_nebs[i] = up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].refi[lidx];
					//valid_flag[i] = valid_flag[i] && MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[i]);
					//assert(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr->min_scu_x) + 1] == me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]]);
					valid_flag[i] = valid_flag[i]
						&& ((int)(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1] >> 31 & ((
							unsigned int)1)))
						&& !((int)(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1] >> 15 & ((
							unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
				}
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] =
					up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].mv[lidx][0];
				mv_nebs[i][1] =
					up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].mv[lidx][1];
			}
			/*if (valid_flag[i]){
			if(me_line_ptr->mv_nebs[i][MV_X] != map_mv[neb_addr[i]][lidx][MV_X] || me_line_ptr->mv_nebs[i][MV_Y] != map_mv[neb_addr[i]][lidx][MV_Y])
			printf("Wrong");
			}*/
		}
	}
	if (!valid_flag[2]) {
		// D
		neb_addr[2] = neb_addr[3];
		valid_flag[2] = valid_flag[3];
		refi_nebs[2] = refi_nebs[3];
		mv_nebs[2][0] = mv_nebs[3][0];
		mv_nebs[2][1] = mv_nebs[3][1];
	}
	ptr_cur_ref = refp_ptr[cur_refi][lidx];
	for (cnt = 0; cnt < 3; cnt++) {
#pragma HLS PIPELINE
		if (valid_flag[cnt]) {
			refi[cnt] = refi_nebs[cnt];
#if MD_PRINT
			((int)refi[cnt]) >= 0 ? ((void)0) : __assert_HLS("((refi[cnt]) >= 0)", "../separate/com_util.c", (unsigned int)723,
				__FUNCTION__);
#endif
			int ptr_neb_ref = refp_ptr[refi[cnt]][lidx];
			//scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, map_mv[neb_addr[cnt]][lidx], MVPs[cnt]);
			scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, mv_nebs[cnt], MVPs[cnt]);
		}
		else {
			refi[cnt] = -1;
			MVPs[cnt][0] = 0;
			MVPs[cnt][1] = 0;
		}
	}
	rFrameL = refi[0];
	rFrameU = refi[1];
	rFrameUR = refi[2];
	if (rFrameL != -1 && rFrameU == -1 && rFrameUR == -1) {
		mvPredType = 0;
	}
	else if (rFrameL == -1 && rFrameU != -1 && rFrameUR == -1) {
		mvPredType = 1;
	}
	else if (rFrameL == -1 && rFrameU == -1 && rFrameUR != -1) {
		mvPredType = 2;
	}
	for (hv = 0; hv < 2; hv++) {
#pragma HLS PIPELINE II=2
		s16 mva = MVPs[0][hv];
		s16 mvb = MVPs[1][hv];
		s16 mvc = MVPs[2][hv];
		switch (mvPredType) {
		case 3:
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
				mvp[hv] = (mvb + mvc) / 2;
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
				mvp[hv] = (mvc + mva) / 2;
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
				mvp[hv] = (mva + mvb) / 2;
			}
			else {
				s16 mva_ext = abs(mva - mvb);
				s16 mvb_ext = abs(mvb - mvc);
				s16 mvc_ext = abs(mvc - mva);
				s16 pred_vec = mva_ext < ((mvb_ext < mvc_ext ? mvb_ext : mvc_ext)) ? mva_ext : ((mvb_ext < mvc_ext ? mvb_ext :
					mvc_ext));
				if (pred_vec == mva_ext) {
					mvp[hv] = (mva + mvb) / 2;
				}
				else if (pred_vec == mvb_ext) {
					mvp[hv] = (mvb + mvc) / 2;
				}
				else {
					mvp[hv] = (mvc + mva) / 2;
				}
			}
			break;
		case 0:
			mvp[hv] = mva;
			break;
		case 1:
			mvp[hv] = mvb;
			break;
		case 2:
			mvp[hv] = mvc;
			break;
		default:
#if MD_PRINT
			0 ? ((void)0) : __assert_HLS("0", "../separate/com_util.c", (unsigned int)800, __FUNCTION__);
#endif
			break;
		}
	}
	// clip MVP after rounding (rounding process might result in 32768)
	CPMV mvp_x;
	CPMV mvp_y;
	com_mv_rounding_s32((s32)mvp[0], (s32)mvp[1], &mvp_x, &mvp_y, (int)amvr_idx, (int)amvr_idx);
	mvp[0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
	mvp[1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
}

static void get_mvp_default_line_16_16(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_ptr_16,
	MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2],
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)])
{
#pragma HLS inline off
	U23 scup = mod_info_curr->scup;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U13 pic_w = md_fw_ptr->pic_width;
	U13 pic_h = md_fw_ptr->pic_height;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 h_scu = md_fw_ptr->pic_height_in_scu;
	//s16(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
	//s8(*map_refi)[REFP_NUM] = pic_map->map_refi;
	//u32* map_scu = pic_map->map_scu;
	int x_scu_left = 0;
	int cnt;
	int hv;
	int ptr_cur_ref;
	int mvPredType = 3;
	s8 rFrameL;
	s8 rFrameU;
	s8 rFrameUR;
	int neb_addr[3 + 1];
	int valid_flag[3 + 1];
	s32 refi[3];
	s32 MVPs[3][2];
	int x_scu = mod_info_curr->x_scu;
	int y_scu = mod_info_curr->y_scu;
	int cu_width_in_scu = cu_width >> 2;
	int offset_in_line = 0;
	//check_mvp_motion_availability(info, mod_info_curr, pic_map, neb_addr, valid_flag, lidx);
	int i = 0;
	int use_line = 0;
	int debug = 0;
	s16 x_scu_line[5 - 1];
	s16 y_scu_line[5 - 1];
	int d_inner = 1;
	S2 refi_nebs[NEB_NUMS];
	//u32 scu_nebs[NEB_NUMS];
	s32 mv_nebs[NEB_NUMS][MV_D];

	// A
	neb_addr[0] = scup - 1;
	// B
	neb_addr[1] = scup - pic_width_in_scu;
	// C
	neb_addr[2] = scup - pic_width_in_scu + cu_width_in_scu;
	// D
	neb_addr[3] = scup - pic_width_in_scu - 1;
	valid_flag[0] = x_scu > 0;
	valid_flag[1] = y_scu > 0;
	valid_flag[2] = y_scu > 0 && x_scu + cu_width_in_scu < pic_width_in_scu;
	valid_flag[3] = x_scu > 0 && y_scu > 0;
	x_scu_line[0] = x_scu - me_line_ptr_16->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_ptr_16->min_scu_y;
	x_scu_line[1] = x_scu - me_line_ptr_16->min_scu_x;
	y_scu_line[1] = y_scu - me_line_ptr_16->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_ptr_16->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_ptr_16->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_ptr_16->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_ptr_16->min_scu_y - 1;
	if (x_scu == me_line_ptr_16->min_scu_x) {
		offset_in_line = y_scu - me_line_ptr_16->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			//assert(me_line_ptr_16->map_scu_left_line[0] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]]);
			valid_flag[0] = ((int)(me_line_ptr_16->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr_16->map_scu_left_line[0] >> 15 & ((
					unsigned int)1)))
				&& ((int)left_nebs[0].refi[lidx]) >= 0;
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->left_nebs[neb_addr[0]].refi[lidx]);
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] = left_nebs[0].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_ptr_16->min_scu_y) {
			offset_in_line = y_scu - me_line_ptr_16->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = up_nebs[0].refi[lidx];
				//assert(me_line_ptr_16->map_scu_up_line[0] == me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[3]]);
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_ptr_16->map_scu_up_line[0] >> 31 & ((
						unsigned int)1)))
					&& !((int)(me_line_ptr_16->map_scu_up_line[0] >> 15 & ((
						unsigned int)1)))
					&& ((int)up_nebs[0].refi[lidx]) >= 0;
			}
			if (valid_flag[3]) {
				mv_nebs[3][0] = up_nebs[0].mv[lidx][0];
				mv_nebs[3][1] = up_nebs[0].mv[lidx][1];
			}
			d_inner = 0;
		}
	}
	else {
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(map_scu[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[0]);
			//assert(me_line_ptr_16->map_scu_left_line[0] == me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]]);
			valid_flag[0] = ((int)(me_line_ptr_16->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_ptr_16->map_scu_left_line[0] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] =
				left_nebs[0].mv[lidx][1];
		}
	}
	if (y_scu == me_line_ptr_16->min_scu_y) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
		//for (i = 1; i < 3 + d_inner; i++) {
		for (i = 1; i < 4; i++)
		{
#pragma HLS loop_tripcount max = 4
#pragma HLS UNROLL
			if (i < 3 + d_inner)
			{
				if (valid_flag[i]) {
					refi_nebs[i] = up_nebs[neb_addr[i] - offset_in_line + 1].refi[lidx];
					//assert(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] == me_line_ptr->new_map_scu_above_line[neb_addr[i] - me_line_ptr->min_scu_x + SCUX_OFFSET]);
					valid_flag[i] = valid_flag[i]
						&& ((int)(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 31 & ((unsigned int)1)))
						&& !((int)(me_line_ptr_16->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 15 & ((unsigned int)1)))
						&& ((int)refi_nebs[i]) >= 0;
				}
				if (valid_flag[i]) {
					mv_nebs[i][0] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][0];
					mv_nebs[i][1] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][1];
				}
			}
		}
	}
	else 
	{
		//for (i = 1; i < 3 + d_inner; i++) {
		for (i = 1; i < 4; i++) 
		{
#pragma HLS loop_tripcount max = 4
#pragma HLS UNROLL
			if (i < 3 + d_inner)
			{

				if (valid_flag[i]) {
					if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
						valid_flag[i] = 0;
					}
					if (valid_flag[i])
					{
						refi_nebs[i] = up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].refi[lidx];
						//valid_flag[i] = valid_flag[i] && MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[i]);
						//assert(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr->min_scu_x) + 1] == me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[i]][y_scu_line[i]]);
						valid_flag[i] = valid_flag[i]
							&& ((int)(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1] >> 31 & ((
								unsigned int)1)))
							&& !((int)(me_line_ptr_16->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1] >> 15 & ((
								unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
					}
				}
				if (valid_flag[i]) {
					mv_nebs[i][0] =
						up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].mv[lidx][0];
					mv_nebs[i][1] =
						up_nebs[x_scu_line[i] - (x_scu - me_line_ptr_16->min_scu_x) + 1].mv[lidx][1];
				}
				/*if (valid_flag[i]){
				if(me_line_ptr->mv_nebs[i][MV_X] != map_mv[neb_addr[i]][lidx][MV_X] || me_line_ptr->mv_nebs[i][MV_Y] != map_mv[neb_addr[i]][lidx][MV_Y])
				printf("Wrong");
				}*/
			}
		}
	}
	if (!valid_flag[2]) {
		// D
		neb_addr[2] = neb_addr[3];
		valid_flag[2] = valid_flag[3];
		refi_nebs[2] = refi_nebs[3];
		mv_nebs[2][0] = mv_nebs[3][0];
		mv_nebs[2][1] = mv_nebs[3][1];
	}
	ptr_cur_ref = refp_ptr[cur_refi][lidx];
	for (cnt = 0; cnt < 3; cnt++) {
#pragma HLS UNROLL
//#pragma HLS PIPELINE
		if (valid_flag[cnt]) {
			refi[cnt] = refi_nebs[cnt];
#if MD_PRINT
			((int)refi[cnt]) >= 0 ? ((void)0) : __assert_HLS("((refi[cnt]) >= 0)", "../separate/com_util.c", (unsigned int)723,
				__FUNCTION__);
#endif
			int ptr_neb_ref = refp_ptr[refi[cnt]][lidx];
			//scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, map_mv[neb_addr[cnt]][lidx], MVPs[cnt]);
			scaling_mv1(ptr_cur, ptr_cur_ref, ptr_cur, ptr_neb_ref, mv_nebs[cnt], MVPs[cnt]);
		}
		else {
			refi[cnt] = -1;
			MVPs[cnt][0] = 0;
			MVPs[cnt][1] = 0;
		}
	}
	rFrameL = refi[0];
	rFrameU = refi[1];
	rFrameUR = refi[2];
	if (rFrameL != -1 && rFrameU == -1 && rFrameUR == -1) {
		mvPredType = 0;
	}
	else if (rFrameL == -1 && rFrameU != -1 && rFrameUR == -1) {
		mvPredType = 1;
	}
	else if (rFrameL == -1 && rFrameU == -1 && rFrameUR != -1) {
		mvPredType = 2;
	}
	for (hv = 0; hv < 2; hv++) {
#pragma HLS UNROLL
//#pragma HLS PIPELINE II=2
		s16 mva = MVPs[0][hv];
		s16 mvb = MVPs[1][hv];
		s16 mvc = MVPs[2][hv];
		switch (mvPredType) {
		case 3:
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
				mvp[hv] = (mvb + mvc) / 2;
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
				mvp[hv] = (mvc + mva) / 2;
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
				mvp[hv] = (mva + mvb) / 2;
			}
			else {
				s16 mva_ext = abs(mva - mvb);
				s16 mvb_ext = abs(mvb - mvc);
				s16 mvc_ext = abs(mvc - mva);
				s16 pred_vec = mva_ext < ((mvb_ext < mvc_ext ? mvb_ext : mvc_ext)) ? mva_ext : ((mvb_ext < mvc_ext ? mvb_ext :
					mvc_ext));
				if (pred_vec == mva_ext) {
					mvp[hv] = (mva + mvb) / 2;
				}
				else if (pred_vec == mvb_ext) {
					mvp[hv] = (mvb + mvc) / 2;
				}
				else {
					mvp[hv] = (mvc + mva) / 2;
				}
			}
			break;
		case 0:
			mvp[hv] = mva;
			break;
		case 1:
			mvp[hv] = mvb;
			break;
		case 2:
			mvp[hv] = mvc;
			break;
		default:
#if MD_PRINT
			0 ? ((void)0) : __assert_HLS("0", "../separate/com_util.c", (unsigned int)800, __FUNCTION__);
#endif
			break;
		}
	}
	// clip MVP after rounding (rounding process might result in 32768)
	CPMV mvp_x;
	CPMV mvp_y;
	com_mv_rounding_s32((s32)mvp[0], (s32)mvp[1], &mvp_x, &mvp_y, (int)amvr_idx, (int)amvr_idx);
	mvp[0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
	mvp[1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
}

void derive_mvp_line_all_8_16(MD_FW* md_fw_ptr, 
	ENC_ME_LINE_MAP_SIZE<16>* me_line_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2])
{
#pragma HLS inline off
#pragma HLS ARRAY_PARTITION variable=mvp complete dim=0
	U23 scup = mod_info_curr->scup;
	U8 emvp_flag = mod_info_curr->mvp_from_hmvp_flag;
	u8 mvr_idx = 0;
	CPMV mvp_x = 0;
	CPMV mvp_y = 0;
	if (!emvp_flag) {
		get_mvp_default_line_8_16(md_fw_ptr, me_line_ptr_16, mod_info_curr, refp_ptr, ptr, ref_list, (s8)ref_idx, mvr_idx, mvp[mvr_idx], up_nebs, left_nebs);
		for (mvr_idx = 1; mvr_idx < 4; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				com_mv_rounding_s32(mvp[0][0], mvp[0][1], &mvp_x, &mvp_y, mvr_idx, mvr_idx);

				mvp[mvr_idx][0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
				mvp[mvr_idx][1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
			}
		}
	}
	else {
		for (mvr_idx = 0; mvr_idx < 5; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				mvp[mvr_idx][0] = 0;
				mvp[mvr_idx][1] = 0;
			}
		}
	}
}
void derive_mvp_line_all_16_8(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2])
{
#pragma HLS inline off
#pragma HLS ARRAY_PARTITION variable=mvp complete dim=0
	U23 scup = mod_info_curr->scup;
	U8 emvp_flag = mod_info_curr->mvp_from_hmvp_flag;
	u8 mvr_idx = 0;
	CPMV mvp_x = 0;
	CPMV mvp_y = 0;
	if (!emvp_flag) {
		get_mvp_default_line_16_8(md_fw_ptr, me_line_ptr_16, mod_info_curr, refp_ptr, ptr, ref_list, (s8)ref_idx, mvr_idx, mvp[mvr_idx], up_nebs, left_nebs);
		for (mvr_idx = 1; mvr_idx < 4; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				com_mv_rounding_s32(mvp[0][0], mvp[0][1], &mvp_x, &mvp_y, mvr_idx, mvr_idx);

				mvp[mvr_idx][0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
				mvp[mvr_idx][1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
			}
		}
	}
	else {
		for (mvr_idx = 0; mvr_idx < 5; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				mvp[mvr_idx][0] = 0;
				mvp[mvr_idx][1] = 0;
			}
		}
	}
}
void derive_mvp_line_all_16_16(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2])
{
#pragma HLS inline off
#pragma HLS ARRAY_PARTITION variable=mvp complete dim=0
	U23 scup = mod_info_curr->scup;
	U8 emvp_flag = mod_info_curr->mvp_from_hmvp_flag;
	u8 mvr_idx = 0;
	CPMV mvp_x = 0;
	CPMV mvp_y = 0;
	if (!emvp_flag) {
		get_mvp_default_line_16_16(md_fw_ptr, me_line_ptr_16, mod_info_curr, refp_ptr, ptr, ref_list, (s8)ref_idx, mvr_idx, mvp[mvr_idx], up_nebs, left_nebs);
		for (mvr_idx = 1; mvr_idx < 4; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				com_mv_rounding_s32(mvp[0][0], mvp[0][1], &mvp_x, &mvp_y, mvr_idx, mvr_idx);

				mvp[mvr_idx][0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
				mvp[mvr_idx][1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
			}
		}
	}
	else {
		for (mvr_idx = 0; mvr_idx < 5; mvr_idx++) {
#pragma HLS UNROLL
			if (mvr_idx < mvr_num) {
				mvp[mvr_idx][0] = 0;
				mvp[mvr_idx][1] = 0;
			}
		}
	}
}
void prepare_luma_inter_8_16(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16* skip_in_16,
	MD_COM_MODE* mod_info_curr,
	SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	int num_amvr, u32 lambda_mv, s8 refi_cur,
#if ENABLE_BFRAME
	s8 num_refp_cnt[2],
#else
	s8 num_refp_cnt,
#endif
	int ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2],
	U1 valid_mode[17], int amvr_enable_flag, ENC_FME_INFO_ALL_16* fme_mv_info,
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir, MV_DIR* mv_dir,
#endif
	S14 sub_pel_mv[MV_D], SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp, SKIP_MERGE_INPUT* skip_in
#endif
)
{
#pragma HLS inline off

#pragma HLS ARRAY_PARTITION variable=valid_mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode complete dim=0
	U12 x_pos = mod_info_curr->x_pos;
	U12 y_pos = mod_info_curr->y_pos;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U10 x_scu = x_pos >> 2;
	U10 y_scu = y_pos >> 2;

	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U11 x0_scu;
	U11 y0_scu;
	x0_scu = PEL2SCU(x_pos - md_input_ptr->pix_x);
	y0_scu = PEL2SCU(y_pos - md_input_ptr->pix_y);

	U1 curr_ctu_idx = me_line_map_ptr_16->curr_ctu_idx;
	U1 above_line_idx = me_line_map_ptr_16->above_line_idx;
	U12 pix_x_scu = skip_in_16->pix_x >> MIN_CU_LOG2;
	//SKIP_ELEMENT mode[SKIP_MODE_NUM];
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
#pragma HLS UNROLL
		mode[i].valid = 0;
		mode[i].skip_flag = 0;
		mode[i].skip_idx = 0;
		mode[i].umve_flag = 0;
		mode[i].umve_idx = 0;
		mode[i].affine_flag = 0;
		mode[i].mvp[REFP_0][0] = 0;
		mode[i].mvp[REFP_0][1] = 0;
		mode[i].refi[REFP_0] = -1;
#if ENABLE_BFRAME
		mode[i].mvp[REFP_1][0] = 0;
		mode[i].mvp[REFP_1][1] = 0;
		mode[i].refi[REFP_1] = -1;
#endif
	}
	U6 num_cands_all = 0;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 pic_height_in_scu = md_fw_ptr->pic_height_in_scu;
	//U1 allow_affine = 1;
	U1 run_direct_skip = cu_width * cu_height >= 64;
	//U1 run_affine_merge = allow_affine && cu_width >= 16 && cu_height >= 16;
	NEB_CU neb[NUM_SKIP_SPATIAL_MV];
#pragma HLS ARRAY_PARTITION variable=neb complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS UNROLL
		neb[i].x_scu_of_ctu = 0;
		neb[i].y_scu_of_ctu = 0;
		neb[i].valid_flag = 0;
	}
	SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];
#pragma HLS ARRAY_PARTITION variable=neb_map complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS UNROLL
		neb_map[i].map_mv[REFP_0][0] = 0;
		neb_map[i].map_mv[REFP_0][1] = 0;
		neb_map[i].map_refi[REFP_0] = REFI_INVALID;
#if ENABLE_BFRAME
		neb_map[i].map_mv[REFP_1][0] = 0;
		neb_map[i].map_mv[REFP_1][1] = 0;
		neb_map[i].map_refi[REFP_1] = REFI_INVALID;
#endif
		neb_map[i].map_scu = 0;
		neb_map[i].map_cu_mode = 0;
	}
	S7 mode_list[MAX_INTER_SKIP_RDO];

	init_skip_merge_rdo(skip_rdo, merge_rdo, mode_list);
	derive_neb_cu_8_16(pic_width_in_scu, x_scu, y_scu, cu_width, cu_height,
		neb, neb_map,
		me_line_map_ptr_16->map_scu_up_line, me_line_map_ptr_16->map_cu_mode_up_line,
		me_line_map_ptr_16->map_scu_left_line, me_line_map_ptr_16->map_cu_mode_left_line,
		curr_ctu_idx,
		up_nebs, left_nebs);
#if USE_ENH
	derive_inter_cands_8_16(skip_in,
		(U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr, cu_width, cu_height, core_sbtmvp);
#else
	derive_inter_cands_16(skip_in_16,
		(U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr);
#endif
	if (core_sbtmvp->isValid && md_fw_ptr->num_refp[REFP_0] > 1)
	{
		core_sbtmvp->isValid = FALSE;
	}
#if SKIP_RESTRI
	check_skip_mv_inbound_8_16(mode, x_pos, y_pos, md_input_ptr->CtrPos2MD, cu_width, cu_height, mod_info_curr
#if ENABLE_BFRAME
		, md_input_ptr->CtrPos2MD_ref1, Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
	);
#endif

	for (int i = 0, j = 0; i < SKIP_MODE_NUM && j < 4; ++i) {
#pragma HLS loop_tripcount max = 10
		if (mode[i].valid && (mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(mode[i].refi[REFP_1])))
		{
			mode_list[j++] = i;
		}
	}
	output_skip_mode(skip_rdo, skip_rdo_mode, mode, mode_list);
	//print_skip_mode(mod_info_curr, skip_rdo, skip_rdo_mode, merge_rdo);

	mod_info_curr->affine_flag = 0;
	mod_info_curr->mvp_from_hmvp_flag = 0;

		sub_pel_mv[MV_X] = fme_mv_info->sadTree8x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree8x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block8x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block8x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	
#if ENABLE_BFRAME
	for (refi_cur = 0; refi_cur < num_refp_cnt[mv_dir->lidx]; refi_cur++) {
#pragma HLS loop_tripcount max = 2
		derive_mvp_line_all_8_16(md_fw_ptr, me_line_map_ptr_16, up_nebs, left_nebs,
			mod_info_curr, ptr, mv_dir->lidx, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#else
	for (refi_cur = 0; refi_cur < num_refp_cnt; refi_cur++) {
#pragma HLS loop_tripcount max = 1
		derive_mvp_line_all_16(md_fw_ptr, me_line_map_ptr_16, up_nebs, left_nebs, mod_info_curr, ptr, 0, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#endif



	for (int x = 0; x < 4; x++) {
#pragma HLS UNROLL
		if (x < skip_rdo->num_rdo)
		{
			if (PATCH_MODE())
				valid_mode[x] = 0;
			else
				valid_mode[x] = 1;

#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
			{
				if (md_input_ptr->RefreshMode == 0)
				{
					if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
						valid_mode[x] = 1;
				}
				else
				{
					if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
						valid_mode[x] = 1;
				}

			}
#endif

		}
		else
		{
			valid_mode[x] = 0;
		}
	}

	for (int y = 0; y < 5; y++) {
#pragma HLS UNROLL
			valid_mode[y + 4] = 0;
	}

	for (int z = 0; z < 5; z++) {
		//if (amvr_enable_flag)
		//	valid_mode[z + 9] = 1;
		//else
#pragma HLS UNROLL
		{
			if (PATCH_MODE())
				valid_mode[9] = 0;
			else
				valid_mode[9] = 1;
#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
			{
				if (md_input_ptr->RefreshMode == 0)
				{
					if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
						valid_mode[9] = 0;
				}
				else
				{
					if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
						valid_mode[9] = 0;
				}

			}
#endif

			valid_mode[10] = 0;
			valid_mode[11] = 0;
			valid_mode[12] = 0;
			valid_mode[13] = 0;
		}
	}

	for (int w = 0; w < 3; w++) {
#pragma HLS UNROLL
		{
			valid_mode[w + 14] = 0;
		}
	}



}
void prepare_luma_inter_16_8(MD_FW* md_fw_ptr, 
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16* skip_in_16,
	MD_COM_MODE* mod_info_curr,
	SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	int num_amvr, u32 lambda_mv, s8 refi_cur, 
#if ENABLE_BFRAME
	s8 num_refp_cnt[2],
#else
	s8 num_refp_cnt,
#endif
	int ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2],
	U1 valid_mode[17], int amvr_enable_flag, ENC_FME_INFO_ALL_16* fme_mv_info, 
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir, MV_DIR* mv_dir,
#endif
	S14 sub_pel_mv[MV_D], SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp, SKIP_MERGE_INPUT* skip_in
#endif
)
{
#pragma HLS inline off

#pragma HLS ARRAY_PARTITION variable=valid_mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode complete dim=0
	U12 x_pos = mod_info_curr->x_pos;
	U12 y_pos = mod_info_curr->y_pos;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U10 x_scu = x_pos >> 2;
	U10 y_scu = y_pos >> 2;

	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U11 x0_scu;
	U11 y0_scu;
	x0_scu = PEL2SCU(x_pos - md_input_ptr->pix_x);
	y0_scu = PEL2SCU(y_pos - md_input_ptr->pix_y);

	U1 curr_ctu_idx = me_line_map_ptr_16->curr_ctu_idx;
	U1 above_line_idx = me_line_map_ptr_16->above_line_idx;
	U12 pix_x_scu = skip_in_16->pix_x >> MIN_CU_LOG2;
	//SKIP_ELEMENT mode[SKIP_MODE_NUM];
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
#pragma HLS UNROLL
		mode[i].valid = 0;
		mode[i].skip_flag = 0;
		mode[i].skip_idx = 0;
		mode[i].umve_flag = 0;
		mode[i].umve_idx = 0;
		mode[i].affine_flag = 0;
		mode[i].mvp[REFP_0][0] = 0;
		mode[i].mvp[REFP_0][1] = 0;
		mode[i].refi[REFP_0] = -1;
#if ENABLE_BFRAME
		mode[i].mvp[REFP_1][0] = 0;
		mode[i].mvp[REFP_1][1] = 0;
		mode[i].refi[REFP_1] = -1;
#endif
	}
	U6 num_cands_all = 0;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 pic_height_in_scu = md_fw_ptr->pic_height_in_scu;
	//U1 allow_affine = 1;
	U1 run_direct_skip = cu_width * cu_height >= 64;
	//U1 run_affine_merge = allow_affine && cu_width >= 16 && cu_height >= 16;
	NEB_CU neb[NUM_SKIP_SPATIAL_MV];
#pragma HLS ARRAY_PARTITION variable=neb complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS UNROLL
		neb[i].x_scu_of_ctu = 0;
		neb[i].y_scu_of_ctu = 0;
		neb[i].valid_flag = 0;
	}
	SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];
#pragma HLS ARRAY_PARTITION variable=neb_map complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS UNROLL
		neb_map[i].map_mv[REFP_0][0] = 0;
		neb_map[i].map_mv[REFP_0][1] = 0;
		neb_map[i].map_refi[REFP_0] = REFI_INVALID;
#if ENABLE_BFRAME
		neb_map[i].map_mv[REFP_1][0] = 0;
		neb_map[i].map_mv[REFP_1][1] = 0;
		neb_map[i].map_refi[REFP_1] = REFI_INVALID;
#endif
		neb_map[i].map_scu = 0;
		neb_map[i].map_cu_mode = 0;
	}
	S7 mode_list[MAX_INTER_SKIP_RDO];

	init_skip_merge_rdo(skip_rdo, merge_rdo, mode_list);
	derive_neb_cu_16_8(pic_width_in_scu, x_scu, y_scu, cu_width, cu_height,
		neb, neb_map,
		me_line_map_ptr_16->map_scu_up_line, me_line_map_ptr_16->map_cu_mode_up_line,
		me_line_map_ptr_16->map_scu_left_line, me_line_map_ptr_16->map_cu_mode_left_line,
		curr_ctu_idx,
		up_nebs, left_nebs);
#if USE_ENH
	derive_inter_cands_16_8(skip_in,
		(U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr, cu_width, cu_height, core_sbtmvp);
#else
	derive_inter_cands_16(skip_in_16,
		(U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr);
#endif
	if (core_sbtmvp->isValid && md_fw_ptr->num_refp[REFP_0] > 1)
	{
		core_sbtmvp->isValid = FALSE;
	}
#if SKIP_RESTRI
	check_skip_mv_inbound_16_8(mode, x_pos, y_pos, md_input_ptr->CtrPos2MD, cu_width, cu_height, mod_info_curr
#if ENABLE_BFRAME
		, md_input_ptr->CtrPos2MD_ref1, Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
	);
#endif

	for (int i = 0, j = 0; i < SKIP_MODE_NUM && j < 4; ++i) {
#pragma HLS loop_tripcount max = 10
		if (mode[i].valid && (mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(mode[i].refi[REFP_1])))
		{
			mode_list[j++] = i;
		}
	}
	output_skip_mode(skip_rdo, skip_rdo_mode, mode, mode_list);
	//print_skip_mode(mod_info_curr, skip_rdo, skip_rdo_mode, merge_rdo);

	mod_info_curr->affine_flag = 0;
	mod_info_curr->mvp_from_hmvp_flag = 0;

	if (cu_width == 16 && cu_height == 8)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree16x8Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree16x8Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block16x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block16x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (cu_width == 8 && cu_height == 16)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree8x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree8x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block8x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block8x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
//	else if (cu_width == 16 && cu_height == 4)
//	{
//		sub_pel_mv[MV_X] = fme_mv_info->sadTree16x4Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
//		sub_pel_mv[MV_Y] = fme_mv_info->sadTree16x4Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
//#if ENABLE_BFRAME
//		mv_dir->cur_refi = me_mv_dir->block16x4MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
//		mv_dir->lidx = me_mv_dir->block16x4MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
//#endif
//	}
//	else if (cu_width == 4 && cu_height == 16)
//	{
//		sub_pel_mv[MV_X] = fme_mv_info->sadTree4x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
//		sub_pel_mv[MV_Y] = fme_mv_info->sadTree4x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
//#if ENABLE_BFRAME
//		mv_dir->cur_refi = me_mv_dir->block4x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
//		mv_dir->lidx = me_mv_dir->block4x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
//#endif
//	}
	else //(cu_width == 16 && cu_height == 16)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree16x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree16x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block16x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block16x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
#if ENABLE_BFRAME
	for (refi_cur = 0; refi_cur < num_refp_cnt[mv_dir->lidx]; refi_cur++) {
#pragma HLS loop_tripcount max = 2
		derive_mvp_line_all_16_8(md_fw_ptr, me_line_map_ptr_16, up_nebs, left_nebs,
			mod_info_curr, ptr, mv_dir->lidx, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#else
	for (refi_cur = 0; refi_cur < num_refp_cnt; refi_cur++) {
#pragma HLS loop_tripcount max = 1
		derive_mvp_line_all_16(md_fw_ptr, me_line_map_ptr_16, up_nebs, left_nebs, mod_info_curr, ptr, 0, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#endif



	for (int x = 0; x < 4; x++) {
#pragma HLS UNROLL
		if (x < skip_rdo->num_rdo)
		{
			if (PATCH_MODE())
				valid_mode[x] = 0;
			else
				valid_mode[x] = 1;

#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
			{
				if (md_input_ptr->RefreshMode == 0)
				{
					if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
						valid_mode[x] = 1;
				}
				else
				{
					if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
						valid_mode[x] = 1;
				}

			}
#endif

		}
		else
		{
			valid_mode[x] = 0;
		}
	}

	for (int y = 0; y < 5; y++) {
#pragma HLS UNROLL
			valid_mode[y + 4] = 0;
	}

	for (int z = 0; z < 5; z++) {
		//if (amvr_enable_flag)
		//	valid_mode[z + 9] = 1;
		//else
#pragma HLS UNROLL
		{
			if (PATCH_MODE())
				valid_mode[9] = 0;
			else
				valid_mode[9] = 1;
#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
			{
				if (md_input_ptr->RefreshMode == 0)
				{
					if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
						valid_mode[9] = 0;
				}
				else
				{
					if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
						valid_mode[9] = 0;
				}

			}
#endif

			valid_mode[10] = 0;
			valid_mode[11] = 0;
			valid_mode[12] = 0;
			valid_mode[13] = 0;
		}
	}

	for (int w = 0; w < 3; w++) {
#pragma HLS UNROLL
		{
			valid_mode[w + 14] = 0;
		}
	}

	

}
void prepare_luma_inter_16_16(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs[1 + (16 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(16 >> 2)],
	SKIP_MERGE_INPUT_16* skip_in_16,
	MD_COM_MODE* mod_info_curr,
	SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	int num_amvr, u32 lambda_mv, s8 refi_cur,
#if ENABLE_BFRAME
	s8 num_refp_cnt[2],
#else
	s8 num_refp_cnt,
#endif
	int ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2],
	U1 valid_mode[17], int amvr_enable_flag, ENC_FME_INFO_ALL_16* fme_mv_info,
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir, MV_DIR* mv_dir,
#endif
	S14 sub_pel_mv[MV_D], SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window_16* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp, SKIP_MERGE_INPUT* skip_in
#endif
)
{
#pragma HLS ARRAY_PARTITION variable=&valid_mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=&mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=up_nebs complete dim=0
#pragma HLS ARRAY_PARTITION variable=left_nebs complete dim=0
#pragma HLS ARRAY_PARTITION variable=refp_ptr complete dim=0
/*
#pragma HLS ARRAY_PARTITION variable=skip_rdo_mode complete dim=1
*/
#pragma HLS ARRAY_PARTITION variable=mvp_all complete dim=0
#pragma HLS ARRAY_PARTITION variable=valid_mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=sub_pel_mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode complete dim=0

#pragma HLS ARRAY_PARTITION variable=valid_mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode complete dim=0
	U12 x_pos = mod_info_curr->x_pos;
	U12 y_pos = mod_info_curr->y_pos;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U10 x_scu = x_pos >> 2;
	U10 y_scu = y_pos >> 2;

	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U11 x0_scu;
	U11 y0_scu;
	x0_scu = PEL2SCU(x_pos - md_input_ptr->pix_x);
	y0_scu = PEL2SCU(y_pos - md_input_ptr->pix_y);

	U1 curr_ctu_idx = me_line_map_ptr_16->curr_ctu_idx;
	U1 above_line_idx = me_line_map_ptr_16->above_line_idx;
	U12 pix_x_scu = skip_in_16->pix_x >> MIN_CU_LOG2;
	//SKIP_ELEMENT mode[SKIP_MODE_NUM];
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
#pragma HLS UNROLL
		mode[i].valid = 0;
		mode[i].skip_flag = 0;
		mode[i].skip_idx = 0;
		mode[i].umve_flag = 0;
		mode[i].umve_idx = 0;
		mode[i].affine_flag = 0;
		mode[i].mvp[REFP_0][0] = 0;
		mode[i].mvp[REFP_0][1] = 0;
		mode[i].refi[REFP_0] = -1;
#if ENABLE_BFRAME
		mode[i].mvp[REFP_1][0] = 0;
		mode[i].mvp[REFP_1][1] = 0;
		mode[i].refi[REFP_1] = -1;
#endif
	}
	U6 num_cands_all = 0;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 pic_height_in_scu = md_fw_ptr->pic_height_in_scu;
	//U1 allow_affine = 1;
	U1 run_direct_skip = cu_width * cu_height >= 64;
	//U1 run_affine_merge = allow_affine && cu_width >= 16 && cu_height >= 16;
	NEB_CU neb[NUM_SKIP_SPATIAL_MV];
#pragma HLS ARRAY_PARTITION variable=neb complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS UNROLL
		neb[i].x_scu_of_ctu = 0;
		neb[i].y_scu_of_ctu = 0;
		neb[i].valid_flag = 0;
	}
	SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];
#pragma HLS ARRAY_PARTITION variable=neb_map complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS UNROLL
		neb_map[i].map_mv[REFP_0][0] = 0;
		neb_map[i].map_mv[REFP_0][1] = 0;
		neb_map[i].map_refi[REFP_0] = REFI_INVALID;
#if ENABLE_BFRAME
		neb_map[i].map_mv[REFP_1][0] = 0;
		neb_map[i].map_mv[REFP_1][1] = 0;
		neb_map[i].map_refi[REFP_1] = REFI_INVALID;
#endif
		neb_map[i].map_scu = 0;
		neb_map[i].map_cu_mode = 0;
	}
	S7 mode_list[MAX_INTER_SKIP_RDO];
	S7 mode_list_tmp[MAX_INTER_SKIP_RDO];
#pragma HLS ARRAY_PARTITION variable=mode_list complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->CtrPos2MD complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->CtrPos2MD_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode_list_tmp complete dim=0
	init_skip_merge_rdo(skip_rdo, merge_rdo, mode_list);
	derive_neb_cu_16_16(pic_width_in_scu, x_scu, y_scu, cu_width, cu_height,
		neb, neb_map,
		me_line_map_ptr_16->map_scu_up_line, me_line_map_ptr_16->map_cu_mode_up_line,
		me_line_map_ptr_16->map_scu_left_line, me_line_map_ptr_16->map_cu_mode_left_line,
		curr_ctu_idx,
		up_nebs, left_nebs);
#if USE_ENH
	derive_inter_cands_16_16(skip_in,
		(U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr, cu_width, cu_height, core_sbtmvp);
#else
	derive_inter_cands_16(skip_in_16,
		(U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr);
#endif
	if (core_sbtmvp->isValid && md_fw_ptr->num_refp[REFP_0] > 1)
	{
		core_sbtmvp->isValid = FALSE;
	}
#if SKIP_RESTRI
	check_skip_mv_inbound_16_16(mode, x_pos, y_pos, md_input_ptr->CtrPos2MD, cu_width, cu_height, mod_info_curr
#if ENABLE_BFRAME
		, md_input_ptr->CtrPos2MD_ref1, Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
	);
#endif

//	for (int i = 0, j = 0; i < SKIP_MODE_NUM && j < 4; ++i) {
//#pragma HLS loop_tripcount max = 10
//		if (mode[i].valid && (mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(mode[i].refi[REFP_1])))
//		{
//			mode_list[j++] = i;
//		}
//	}
	for (int i = 0; i < SKIP_MODE_NUM; ++i) {

#pragma HLS loop_tripcount max = 12
#pragma HLS	UNROLL
		if (mode[i].valid && (mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(mode[i].refi[REFP_1])))
		{

			mode_list_tmp[i] = i;
		}
		else
			mode_list_tmp[i] = -2;
	}
	int j = 0;
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
#pragma HLS	UNROLL

		if (mode_list_tmp[i] >= 0)
		{
			if (j < 4)
				mode_list[j++] = mode_list_tmp[i];
		}
	}
	output_skip_mode(skip_rdo, skip_rdo_mode, mode, mode_list);
	//print_skip_mode(mod_info_curr, skip_rdo, skip_rdo_mode, merge_rdo);

	mod_info_curr->affine_flag = 0;
	mod_info_curr->mvp_from_hmvp_flag = 0;
		sub_pel_mv[MV_X] = fme_mv_info->sadTree16x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree16x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block16x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block16x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	
#if ENABLE_BFRAME
	//for (refi_cur = 0; refi_cur < num_refp_cnt[mv_dir->lidx]; refi_cur++) {
		for (refi_cur = 0; refi_cur < 2; refi_cur++) {
#pragma HLS loop_tripcount max = 2
#pragma HLS UNROLL
			if (refi_cur < num_refp_cnt[mv_dir->lidx]) {
				derive_mvp_line_all_16_16(md_fw_ptr, me_line_map_ptr_16, up_nebs, left_nebs,
					mod_info_curr, ptr, mv_dir->lidx, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
			}
	}
#else
	for (refi_cur = 0; refi_cur < num_refp_cnt; refi_cur++) {
#pragma HLS loop_tripcount max = 1
		derive_mvp_line_all_16(md_fw_ptr, me_line_map_ptr_16, up_nebs, left_nebs, mod_info_curr, ptr, 0, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#endif



	for (int x = 0; x < 4; x++) {
#pragma HLS UNROLL
		if (x < skip_rdo->num_rdo)
		{
			if (PATCH_MODE())
				valid_mode[x] = 0;
			else
				valid_mode[x] = 1;

#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
			{
				if (md_input_ptr->RefreshMode == 0)
				{
					if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
						valid_mode[x] = 1;
				}
				else
				{
					if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
						valid_mode[x] = 1;
				}

			}
#endif

		}
		else
		{
			valid_mode[x] = 0;
		}
	}

	for (int y = 0; y < 5; y++) {
#pragma HLS UNROLL
			valid_mode[y + 4] = 0;
	}

	for (int z = 0; z < 5; z++) {
		//if (amvr_enable_flag)
		//	valid_mode[z + 9] = 1;
		//else
#pragma HLS UNROLL
		{
			if (PATCH_MODE())
				valid_mode[9] = 0;
			else
				valid_mode[9] = 1;
#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
			{
				if (md_input_ptr->RefreshMode == 0)
				{
					if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
						valid_mode[9] = 0;
				}
				else
				{
					if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
						valid_mode[9] = 0;
				}

			}
#endif

			valid_mode[10] = 0;
			valid_mode[11] = 0;
			valid_mode[12] = 0;
			valid_mode[13] = 0;
		}
	}

	for (int w = 0; w < 3; w++) {
#pragma HLS UNROLL
		{
			valid_mode[w + 14] = 0;
		}
	}



}

void fetch_ref_line_32(U8 w, U8 h, S15 hor_ref[48][8], const pel ref_y[SWH * SWW], const U10 r_off, const U10 c_off)
{
#pragma HLS INLINE OFF
	pel line_buf[SWW] = {};
	pel buf[23 + 32] = {};
#pragma HLS array_partition variable=line_buf complete dim=0
#pragma HLS array_partition variable=buf complete dim=0

	for (int i = 0; i < SWW; ++i) {
#pragma HLS UNROLL
		line_buf[i] = (r_off >= 0) && (r_off < SWH) ? (pel)(ref_y[r_off * SWW + i]) : (pel)0;
	}
	for (int i = 0; i < w + 23; ++i) {
#pragma HLS loop_tripcount max=55
#pragma HLS UNROLL
		buf[i] = c_off >= 0 ? (pel)(line_buf[c_off + i]) : (pel)0;
	}
	for (int i = 0; i < w + 16; ++i) {
#pragma HLS loop_tripcount max=48
#pragma HLS UNROLL
		for (int j = 0; j < 8; ++j) {
#pragma HLS UNROLL
			hor_ref[i][j] = buf[i + j];
		}
	}
}

void fetch_org_line_32(U8 w, S15 hor_org[32][SKIP_MC_VER_NUM], const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U6 r_off, const U6 c_off)
{
#pragma HLS INLINE OFF
	pel line_buf[MAX_CU_SIZE_FETCH] = {};
#pragma HLS array_partition variable=line_buf complete dim=0
	for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i) {
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 ? (pel)(org_y[r_off][i]) : (pel)0;
	}
	for (int i = 0; i < w; ++i) {
#pragma HLS UNROLL
		shifting(hor_org[i], line_buf[c_off + i], SKIP_MC_VER_NUM);
	}
}


void mc_sad_l_skip_32(S32 sad[SKIP_MODE_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const U1 mv_idx, const U12 x_pos, const U12 y_pos, const U7 cu_width, const U7 cu_height,
	const U12 pic_w, const U12 pic_h, const S12 mc_pos[MV_D], const pel ref_y[SWH * SWW], const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U12 x_chroma, const U12 y_chroma,
	const U1 chroma_motion)
{
#pragma HLS INLINE OFF

	U1 flag[SKIP_MODE_NUM] = {};
	S15 hor_ref[48][8] = {};
	S15 hor_buf[48][4][8] = {};
	S15 hor_org[32][SKIP_MC_VER_NUM] = {};
#pragma HLS array_partition variable=flag complete dim=0
#pragma HLS array_partition variable=hor_ref complete dim=0
#pragma HLS array_partition variable=hor_buf complete dim=0
#pragma HLS array_partition variable=hor_org complete dim=0

	const U10 ref_top = mc_pos[MV_Y] - 3;
	const U10 ref_left = mc_pos[MV_X] - 3;
	const U6 org_top = y_pos & (MAX_CU_SIZE_FETCH - 1);
	const U6 org_left = x_pos & (MAX_CU_SIZE_FETCH - 1);

	if (ref_top + 11 + cu_height > SWH) { return; }
	if (ref_left + SKIP_MC_HOR_NUM + cu_width - 1 + 7 > SWW) { return; }

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
#pragma HLS UNROLL
		flag[i] = REFI_IS_VALID(mode[i].refi) && mode[i].mv_idx == mv_idx && check_mv_in_refwin(ref_top + (mode[i].mvd[MV_Y] >> 2), ref_left + (mode[i].mvd[MV_X] >> 2), x_pos, y_pos, cu_width, cu_height, x_chroma, y_chroma, chroma_motion);
	}
	static int pflag = 1;
	//LOOP_MC_SAD:
	for (int r = 0; r < 11 + cu_height; ++r) {
#pragma HLS PIPELINE II=1

		fetch_ref_line_32(cu_width,cu_height,hor_ref, ref_y, ref_top + r, ref_left);
		fetch_org_line_32(cu_width,hor_org, org_y, org_top + r - 7, org_left);

		for (int c = 0; c < (SKIP_MC_HOR_NUM + cu_width - 1); ++c) {
#pragma HLS UNROLL
			S9 pred[4][4] = {};
#pragma HLS array_partition variable=pred complete dim=0

			for (int dx = 0; dx < 4; ++dx) {
#pragma HLS UNROLL
				S15 hor_data = mc_8tag(dx, hor_ref[c]);
				shifting(hor_buf[c][dx], hor_data, 8);
				for (int dy = 0; dy < 4; ++dy) {
#pragma HLS UNROLL
					S21 col_buf = mc_8tag(dy, hor_buf[c][dx]);
					S9 pt = (col_buf + 2048) >> 12;
					pred[dx][dy] = COM_CLIP3((S9)0, (S9)((1 << 8) - 1), pt);
				}
			}

			for (int i = 0; i < SKIP_MODE_NUM; ++i) {
#pragma HLS UNROLL
				const S8 x = c - (mode[i].mvd[MV_X] >> 2);
				const S8 y = r - 7 - (mode[i].mvd[MV_Y] >> 2);
				if (flag[i]) {
					sad[i] += (x >= 0 && x < cu_width&& y >= 0 && y < cu_height) ?
						(S32)COM_ABS16(pred[mode[i].mvd[MV_X] & 3][mode[i].mvd[MV_Y] & 3] - hor_org[x][SKIP_MC_VER_NUM - (mode[i].mvd[MV_Y] >> 2) - 1]) :
						(S32)0;
				}
				//if (pflag && i == 0 && flag[i] && x >= 0 && x < cu_width && y >= 0 && y < cu_height){
				//printf("%d %d %d\n", x, y, COM_ABS16(pred[mode[i].mvd[MV_X] & 3][mode[i].mvd[MV_Y] & 3] - hor_org[x][SKIP_MC_VER_NUM - (mode[i].mvd[MV_Y] >> 2) - 1]));
				//}
			}
		}
	}
	pflag = 0;
}
static void make_skip_sad_32(
	U8 w,
	U8 h,
	S32 mode_sad[SKIP_MODE_NUM],
	const SKIP_MODE_INTER mode[SKIP_MODE_NUM],
	const U12 x_pos,
	const U12 y_pos,
	const U7 cu_width,
	const U7 cu_height,
	const U12 x_chroma,
	const U12 y_chroma,
	const U1 chroma_motion,
	const U12 pic_w,
	const U12 pic_h,
	const pel ref_y[SWH * SWW],
	const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	const S12 mc_pos[SKIP_MC_NUM][MV_D]
) {
#pragma HLS INLINE OFF
	//#pragma HLS function_instantiate variable=cu_height

	for (int mv_idx = 0; mv_idx < SKIP_MC_NUM; ++mv_idx) {
		mc_sad_l_skip_32(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
	}
}

//static void make_cand_list_32(U8 w, U8 h, MD_FW* md_fw_ptr, S7* mode_list, MD_COM_MODE* mod_info_curr_ptr, SKIP_MERGE_INPUT* input, SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window* refw, strFetch_Orig_Lcu* org)
//{
//	U1 valid[SKIP_MODE_NUM] = { 0 };
//
//	SKIP_MODE_INTER mode_temp[SKIP_MODE_NUM];
//	S12 mc_pos[SKIP_MC_NUM][MV_D];
//	S32 mode_sad[SKIP_MODE_NUM];
//	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//		mode_temp[i].skip_flag = mode[i].skip_flag;
//		mode_temp[i].affine_flag = mode[i].affine_flag;
//		mode_temp[i].umve_flag = mode[i].umve_flag;
//		mode_temp[i].skip_idx = mode[i].skip_idx;
//		mode_temp[i].umve_idx = mode[i].umve_idx;
//		mode_temp[i].mvp[MV_X] = mode[i].mvp[MV_X];
//		mode_temp[i].mvp[MV_Y] = mode[i].mvp[MV_Y];
//		mode_temp[i].refi = mode[i].refi;
//		mode_temp[i].valid = mode[i].valid;
//		mode_sad[i] = 0;
//	}
//
//	mc_center(mode_temp, mc_pos, mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, mod_info_curr_ptr->cu_width, mod_info_curr_ptr->cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height);
//
//	make_skip_sad_32(w, h, mode_sad, mode_temp, mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, mod_info_curr_ptr->cu_width, mod_info_curr_ptr->cu_height, refw->x_chroma, refw->y_chroma, mod_info_curr_ptr->chroma_motion, md_fw_ptr->pic_width, md_fw_ptr->pic_height, refw->RefWindowLCU, org->y_org, mc_pos);
//
//	make_skip_cand(mode_list, mode_temp, mode_sad);
//}

