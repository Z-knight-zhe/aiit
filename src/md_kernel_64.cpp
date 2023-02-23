#include "md_kernel_64.h"
#include "md_dummy.h"
//#pragma warning(disable:4334)
//#pragma warning(disable:4018)
//#pragma warning(disable:26453)
//#pragma warning(disable:6297)
//extern ENC_CTX* ctx;
#define UPDATE_OPT
#define LUMA_64_PIPELINE_RATE_OPT

#define MD_KERNEL_64_MOD_YHY 1

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

template<typename T, unsigned size>
void memcpy_1d(T src[size], T dst[size]) {
	for (int i = 0; i < size; i++)
		dst[i] = src[i];
}
template<typename T, unsigned d1, unsigned d2>
void memcpy_2d(T src[d1][d2], T dst[d1][d2]) {
	for (int i = 0; i < d1; i++)
		for (int j = 0; j < d2; j++)
			dst[i][j] = src[i][j];
}

//void get_col_mv_from_list0_64(s16 refp_map_mv[MAX_CU_IN_SCU][MAX_CU_IN_SCU][MV_D], S2 refp_map_refi[MAX_CU_IN_SCU][MAX_CU_IN_SCU], U10 co_x_scu_of_ctu, U10 co_y_scu_of_ctu, S14 mvp[2][2])
//{
//#if MD_PRINT
//	assert(refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu] == 0);
//#endif
//	mvp[REFP_0][MV_X] = refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][0];
//	mvp[REFP_0][MV_Y] = refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][1];
//}
#if ENABLE_BFRAME
//void get_col_mv_from_list0_64(s32 mvp[MV_D], const SKIP_MERGE_INPUT* input, U10 co_x_scu_of_ctu, U10 co_y_scu_of_ctu)
//{
//	s32 mvc[MV_D];
//	mvc[MV_X] = input->refp_map_mv1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_X];
//	mvc[MV_Y] = input->refp_map_mv1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][MV_Y];
//	int ptr_col = input->refp_ptr1[0][REFP_0];
//	int ptr_col_ref = input->refp_list_ptr1[REFP_0][input->refp_map_refi1[REFP_0][co_y_scu_of_ctu][co_x_scu_of_ctu][REFP_0]];
//#if !ENABLE_BFRAME
//	assert(input->ptr - input->refp_ptr1[0][REFP_0] == 1);
//	assert(ptr_col - ptr_col_ref == 1);
//#endif
//	scaling_mv1(input->ptr, input->refp_ptr1[0][REFP_0], ptr_col, ptr_col_ref, mvc, mvp);
//}
#endif
void derive_neb_cu_64(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height,
	NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV],
	const U32 map_scu_up_line[6], const U32 map_cu_mode_up_line[6],
	const U32 map_scu_left_line[4], const U32 map_cu_mode_left_line[4],
	const U1 curr_ctu_idx,
	const ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], const ENC_ME_NEB_INFO left_nebs[(64 >> 2)])
{
//#pragma HLS INLINE OFF

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
//#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 6
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

#if SUB_TMVP

U4 get_colocal_scu_len(const U10 cur, const U10 pic_size_in_scu)
{
//#pragma HLS INLINE
	const U10 mask = (-1) ^ 3;
	const U10 col = (cur & mask) + 2;
	const U10 result = col >= pic_size_in_scu ? (U10)((cur & mask) + pic_size_in_scu) >> 1 : col;
	return result & (MAX_CU_IN_SCU - 1);
}

int get_colocal_scup_sbtmvp_64(int scup, int pic_width_in_scu, int pic_height_in_scu)
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


}void get_col_mv_from_list0_ext_64(const SKIP_MERGE_INPUT* input, U4 co_x_scu_of_ctu, U4 co_y_scu_of_ctu, s32 mvp[REFP_NUM][MV_D], s32 refi[REFP_NUM])
{
	//s16 mvc[MV_D];

	// ref ref ptr
	/*int ptr_col;
	int ptr_col_ref;
	ptr_col = input->ptr;*/


	//assert(REFI_IS_VALID(refp->map_refi[scup_co][REFP_0]) || REFI_IS_VALID(refp->map_refi[scup_co][REFP_1]));

//	if (REFI_IS_VALID(refp->map_refi[scup_co][REFP_0]))

	if (REFI_IS_VALID(input->refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu]))

	{

		const U1 valid = REFI_IS_VALID(input->refp_map_refi[co_y_scu_of_ctu][co_x_scu_of_ctu]);

		mvp[REFP_0][MV_X] = valid ? input->refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][MV_X] : (s16)0;
		mvp[REFP_0][MV_Y] = valid ? input->refp_map_mv[co_y_scu_of_ctu][co_x_scu_of_ctu][MV_Y] : (s16)0;

		//ptr_col_ref = refp->list_ptr[REFP_0][refp->map_refi[scup_co][REFP_0]];
		//ptr_col_ref = ptr_col - 1;
		//scaling_mv1(1, 0, ptr_col, ptr_col_ref, mvc, mvp[REFP_0]);
		refi[REFP_0] = 0;
	}
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

void derive_inter_cands_64(SKIP_MERGE_INPUT* input, U4* num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window* Fetch_Ref_window_ptr
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
	int sb_scup[SBTMVP_NUM][MV_D];
	U4 co_x_scu_of_ctu[SBTMVP_NUM];
	U4 co_y_scu_of_ctu[SBTMVP_NUM];
	//	int sb_scup_co[SBTMVP_NUM];
	int x_scup = x_scu;
	int y_scup = y_scu;

	for (int p = 0; p < SBTMVP_NUM; p++)
	{
 #pragma HLS loop_tripcount max = 4
		//sb_scup[p] = (x_scup + y_scup * pic_width_in_scu) + pic_width_in_scu * ((cu_height >> 2) - 1) * (p / 2) + ((cu_width >> 2) - 1) * (p % 2);

		sb_scup[p][MV_X] = x_scup + ((cu_width >> 2) - 1) * (p % 2);
		sb_scup[p][MV_Y] = y_scup + ((cu_height >> 2) - 1) * (p / 2);

		co_x_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_X], pic_width_in_scu);
		co_y_scu_of_ctu[p] = get_colocal_scu_len(sb_scup[p][MV_Y], pic_height_in_scu);

		//sb_scup_co[p] = get_colocal_scup_sbtmvp(sb_scup[p], pic_width_in_scu, pic_height_in_scu);
	}
#endif
	SKIP_ELEMENT tmvp;
	SKIP_ELEMENT spat[3];

	// insert TMVP
	derive_tmvp_mode(input, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, &tmvp, neb, neb_map);
	skip_element_copy(mode, &tmvp);
	// insert sbTmvp
#if SUB_TMVP
	if (cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
		if (input->slice_type == SLICE_P)
		{
			core_sbtmvp->isValid = TRUE;
			for (int p = 0; p < SBTMVP_NUM; p++)
			{
				if (
					REFI_IS_VALID(input->refp_map_refi[co_y_scu_of_ctu[p]][co_x_scu_of_ctu[p]])
					)
				{
					get_col_mv_from_list0_ext_64(input, co_x_scu_of_ctu[p], co_y_scu_of_ctu[p], core_sbtmvp->sbTmvp[p].mv, core_sbtmvp->sbTmvp[p].ref_idx);

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
				check_refi(mode[i].refi, Fetch_Ref_window_ptr);
			}
		}
	}
#endif

	//	print_inter_cands(mode);
}

void output_skip_mode_64(SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], SKIP_ELEMENT* mode, S7 mode_list[MAX_INTER_SKIP_RDO])
{
	for (int i = 0; i < skip_rdo->num_rdo; i++) {
#pragma HLS LOOP_TRIPCOUNT max=12
		if (mode_list[i] < 0 || i == 4) {
			skip_rdo->num_rdo = i;
			break;
		}
		skip_element_copy(skip_rdo_mode + i, mode + mode_list[i]);
	}
}
S5 encoder(const U1* input, const U4 width)
{
//#pragma HLS INLINE
//#pragma HLS function_instantiate variable=width
	assert(width <= 16);
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
void mv_in_ref_window(const U13 x_pos, const U13 y_pos, const U7 cu_width, const U7 cu_height, const U13 pic_w, const U13 pic_h, const S14 mv[MV_D], S12 qpel_gmv_of_refp_cu[MV_D])
{
//#pragma HLS INLINE OFF
	int x_ext = x_pos << 2;
	int y_ext = y_pos << 2;
	int w_ext = cu_width << 2;
	int h_ext = cu_height << 2;

	int min_clip[MV_D];
	int max_clip[MV_D];
	s16 mv_t[2];
//#pragma HLS array_partition variable=min_clip complete dim=0
//#pragma HLS array_partition variable=max_clip complete dim=0
//#pragma HLS array_partition variable=mv_t complete dim=0

	min_clip[MV_X] = ((-MAX_CU_SIZE - 4) << 2) - x_ext;
	min_clip[MV_Y] = ((-MAX_CU_SIZE - 4) << 2) - y_ext;
	max_clip[MV_X] = ((pic_w - 1 + MAX_CU_SIZE + 4) << 2) - x_ext - w_ext + 4;
	max_clip[MV_Y] = ((pic_h - 1 + MAX_CU_SIZE + 4) << 2) - y_ext - h_ext + 4;

	mv_t[MV_X] = COM_CLIP3((S14)min_clip[MV_X], (S14)max_clip[MV_X], mv[MV_X]);
	mv_t[MV_Y] = COM_CLIP3((S14)min_clip[MV_Y], (S14)max_clip[MV_Y], mv[MV_Y]);

	qpel_gmv_of_refp_cu[MV_X] = ((x_ext & ((MAX_CU_SIZE2 << 2) - 1)) + mv_t[MV_X] + (REFP_SURD_OFF_W << 2)) >> 2;
	qpel_gmv_of_refp_cu[MV_Y] = ((y_ext & ((MAX_CU_SIZE2 << 2) - 1)) + mv_t[MV_Y] + (REFP_SURD_OFF_H << 2)) >> 2;
}


void enc_bit_est_inter_skip(U4* bit_cnt, const SKIP_MODE_INTER mode[SKIP_MODE_NUM])
{
//#pragma HLS INLINE OFF
#if MD_PRINT
	assert(UMVE_BASE_NUM == 2 && UMVE_REFINE_STEP > 1);
#endif
	U3 ref_step = (mode->umve_idx >= UMVE_MAX_REFINE_NUM ? U5(mode->umve_idx - UMVE_MAX_REFINE_NUM) : U5(mode->umve_idx)) >> 2;
	*bit_cnt = (3 + mode->umve_flag ?
		U4(2 + (ref_step == UMVE_REFINE_STEP - 1 ? U4(ref_step) : (U4)(ref_step + 1))) :
		U4((mode->skip_idx == 0 ? 0 : mode->skip_idx - 2) + (mode->skip_idx == 11 ? 0 : 1)));
}
void check_same_mv(const SKIP_MODE_INTER mode[SKIP_MODE_NUM], U1 valid[SKIP_MODE_NUM], const S64 cost[SKIP_MODE_NUM])
{

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
		U1 same = FALSE;
		for (int j = 0; j < SKIP_MODE_NUM; ++j) {
//#pragma HLS UNROLL
			same |= mode[j].valid &&
				mode[i].mvp[MV_X] == mode[j].mvp[MV_X] &&
				mode[i].mvp[MV_Y] == mode[j].mvp[MV_Y] &&
				(cost[i] > cost[j] || (j < i&& cost[i] == cost[j]));
		}
		valid[i] = !same && mode[i].valid;
	}
}
void sort_skip_cand(const S64 cost[SKIP_MODE_NUM], const U1 valid[SKIP_MODE_NUM], S7 mode_list[SKIP_RDO_NUM])
{
//#pragma HLS UNROLL

	int rank[SKIP_MODE_NUM] = {};
//#pragma HLS array_partition variable=rank complete dim=0

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
		rank[i] = valid[i] ? 0 : -1;
		for (int j = 0; j < SKIP_MODE_NUM; ++j) {
//#pragma HLS UNROLL
			rank[i] += (valid[i] && valid[j] && (cost[i] > cost[j] || (i > j && cost[i] == cost[j]))) ? 1 : 0;
		}
	}

	for (int i = 0; i < SKIP_RDO_NUM; ++i) {
//#pragma HLS UNROLL
		U1 flag[SKIP_MODE_NUM] = {};
//#pragma HLS array_partition variable=rank complete dim=0

		for (int j = 0; j < SKIP_MODE_NUM; ++j) {
//#pragma HLS UNROLL
			flag[j] = rank[j] == i;
		}
		mode_list[i] = encoder(flag, SKIP_MODE_NUM);
	}
}

void fetch_ref_line_64(U7 w, U7 h, S15 hor_ref[SKIP_MC_OUT_WIDTH_64][8], const U8 ref_y[SWH * SWW], const U10 r_off, const U10 c_off)
{
//#pragma HLS INLINE OFF
	U8 line_buf[SWW] = {};
	U8 buf[23 + 64] = {};
//#pragma HLS array_partition variable=line_buf complete dim=0
//#pragma HLS array_partition variable=buf complete dim=0
	for (int i = 0; i < SWW; ++i) {
//#pragma HLS UNROLL
		line_buf[i] = (r_off >= 0) && (r_off < SWH) ? (U8)(ref_y[r_off * SWW + i]) : (U8)0;
	}
	for (int i = 0; i < 87; ++i) {
//#pragma HLS UNROLL
		if (i < 23 + w)
			buf[i] = c_off >= 0 ? (U8)(line_buf[c_off + i]) : (U8)0;
	}
	for (int i = 0; i < 80; ++i) {
//#pragma HLS UNROLL
		if (i < 16 + w)
		{
			for (int j = 0; j < 8; ++j) {
//#pragma HLS UNROLL
				hor_ref[i][j] = buf[i + j];
			}
		}
	}
}
S21 mc_8tag_64(const u8 idx, const S15 data[8])
{
//#pragma HLS INLINE
//#pragma HLS function_instantiate variable=idx

	static const S8 tbl_mc_l_coeff[4][8] = {
		{ 0, 0, 0, 64, 0, 0, 0, 0 },
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 } };
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0

	return tbl_mc_l_coeff[idx][0] * data[0] +
		tbl_mc_l_coeff[idx][1] * data[1] +
		tbl_mc_l_coeff[idx][2] * data[2] +
		tbl_mc_l_coeff[idx][3] * data[3] +
		tbl_mc_l_coeff[idx][4] * data[4] +
		tbl_mc_l_coeff[idx][5] * data[5] +
		tbl_mc_l_coeff[idx][6] * data[6] +
		tbl_mc_l_coeff[idx][7] * data[7];
}
void shifting_64(S15* reg, const S15 in_data, const U4 depth)
{
//#pragma HLS INLINE
//#pragma HLS function_instantiate variable=depth
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

void fetch_org_line_64(U7 w, U7 h, S15 hor_org[64][SKIP_MC_VER_NUM], const U8 org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U6 r_off, const U6 c_off)
{
//#pragma HLS INLINE OFF
	U8 line_buf[MAX_CU_SIZE_FETCH] = {};
//#pragma HLS array_partition variable=line_buf complete dim=0
	for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i) {
//#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 ? (U8)(org_y[r_off][i]) : (U8)0;
	}
	for (int i = 0; i < 64; ++i) {
//#pragma HLS UNROLL
		if (i < w)
			shifting_64(hor_org[i], line_buf[c_off + i], SKIP_MC_VER_NUM);
	}
}

BOOL check_mv_in_refwin_64(const int top, const int left, const U13 x_pos, const U13 y_pos, const U7 cu_width, const U7 cu_height, const U12 x_chroma, const U12 y_chroma, const BOOL chroma_motion)
{
	if (!(left >= 0 && left + cu_width + 7 < SWW &&
		top >= 0 && top + cu_height + 7 < SWH))
	{
		return FALSE;
	}

	//if (chroma_motion &&
	//	!(left + x_chroma - x_pos >= 0 &&
	//		top + y_chroma - y_pos >= 0)) {
	//	return FALSE;
	//}

	return TRUE;
}

void mc_sad_l_skip_64(U7 w, U7 h, S32 sad[SKIP_MODE_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const U1 mv_idx, const U12 x_pos, const U12 y_pos, const U7 cu_width, const U7 cu_height, const U12 pic_w, const U12 pic_h, const S12 mc_pos[MV_D], const U8 ref_y[SWH * SWW], const U8 org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U12 x_chroma, const U12 y_chroma, const U1 chroma_motion)
{
//#pragma HLS INLINE OFF

	U1 flag[SKIP_MODE_NUM] = {};
	S15 hor_ref[SKIP_MC_OUT_WIDTH_64][8] = {};
	S15 hor_buf[SKIP_MC_OUT_WIDTH_64][4][8] = {};
	S15 hor_org[64][SKIP_MC_VER_NUM] = {};
//#pragma HLS array_partition variable=flag complete dim=0
//#pragma HLS array_partition variable=hor_ref complete dim=0
//#pragma HLS array_partition variable=hor_buf complete dim=0
//#pragma HLS array_partition variable=hor_org complete dim=0

	const U10 ref_top = mc_pos[MV_Y] - 3;
	const U10 ref_left = mc_pos[MV_X] - 3;
	const U6 org_top = y_pos & (MAX_CU_SIZE_FETCH - 1);
	const U6 org_left = x_pos & (MAX_CU_SIZE_FETCH - 1);

	if (ref_top + SKIP_MC_VER_NUM + h - 1 + 7 > SWH) { return; }
	if (ref_left + SKIP_MC_HOR_NUM + w - 1 + 7 > SWW) { return; }

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
		flag[i] = REFI_IS_VALID(mode[i].refi) && mode[i].mv_idx == mv_idx && check_mv_in_refwin_64(ref_top + (mode[i].mvd[MV_Y] >> 2), ref_left + (mode[i].mvd[MV_X] >> 2), x_pos, y_pos, cu_width, cu_height, x_chroma, y_chroma, chroma_motion);
	}
	static int pflag = 1;
	//LOOP_MC_SAD:
	int SKIP_MC_IN_HEIGHT_loop = SKIP_MC_VER_NUM + h - 1 + 7;
	for (int r = 0; r < SKIP_MC_IN_HEIGHT_loop; ++r) {
//#pragma HLS PIPELINE II=1
//#pragma HLS loop_tripcount max=75
		fetch_ref_line_64(w, h, hor_ref, ref_y, ref_top + r, ref_left);
		fetch_org_line_64(w, h, hor_org, org_y, org_top + r - 7, org_left);

		for (int c = 0; c < 80; ++c) {
			if (c >= (SKIP_MC_HOR_NUM + cu_width - 1))
				break;

//#pragma HLS UNROLL
			S9 pred[4][4] = {};
//#pragma HLS array_partition variable=pred complete dim=0

			for (int dx = 0; dx < 4; ++dx) {
//#pragma HLS UNROLL
				S15 hor_data = mc_8tag_64(dx, hor_ref[c]);
				shifting_64(hor_buf[c][dx], hor_data, 8);
				for (int dy = 0; dy < 4; ++dy) {
//#pragma HLS UNROLL
					S21 col_buf = mc_8tag_64(dy, hor_buf[c][dx]);
					S9 pt = (col_buf + 2048) >> 12;
					pred[dx][dy] = COM_CLIP3((S9)0, (S9)((1 << 8) - 1), pt);
				}
			}

			for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
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

void mc_center_64(
	SKIP_MODE_INTER mode[SKIP_MODE_NUM],
	S12 mc_pos[SKIP_MC_NUM][MV_D],
	const U12 x_pos,
	const U12 y_pos,
	const U7 cu_width,
	const U7 cu_height,
	const U12 pic_w,
	const U12 pic_h
) {
//#pragma HLS INLINE OFF

	const S14 mask = (-1) ^ 3;
	U1 flag[SKIP_MODE_NUM] = {};
	S14 mv_ctr[SKIP_MC_NUM][MV_D] = {};
	S14 mc_mv[SKIP_MC_NUM][MV_D] = {};
//#pragma HLS array_partition variable=mv_ctr complete dim=0
//#pragma HLS array_partition variable=flag complete dim=0
//#pragma HLS array_partition variable=mc_mv complete dim=0

	mv_ctr[0][MV_X] = mode[1].mvp[MV_X] & mask;
	mv_ctr[0][MV_Y] = mode[1].mvp[MV_Y] & mask;
	mc_mv[0][MV_X] = mv_ctr[0][MV_X] - (SKIP_MC_HOR_PAD << 2);
	mc_mv[0][MV_Y] = mv_ctr[0][MV_Y] - (SKIP_MC_VER_PAD << 2);
#if SKIP_MC_NUM > 1
	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
		flag[i] = mode[i].valid &&
			(COM_ABS16(mode[i].mvp[MV_X] - mv_ctr[0][MV_X]) > (SKIP_MC_HOR_PAD << 2) ||
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

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
		for (int j = 0; j < SKIP_MC_NUM; ++j) {
//#pragma HLS UNROLL
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
	for (int i = 0; i < SKIP_MC_NUM; ++i) {
//#pragma HLS UNROLL
		mv_in_ref_window(x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_mv[i], mc_pos[i]);
	}
}
void make_skip_cand_64(S7 mode_list[SKIP_RDO_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const S32 mode_sad[SKIP_MODE_NUM])
{
//#pragma HLS INLINE OFF

	S64 cost[SKIP_MODE_NUM] = {};
	U1 valid[SKIP_MODE_NUM] = {};
//#pragma HLS array_partition variable=cost complete dim=0
//#pragma HLS array_partition variable=valid complete dim=0

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
//#pragma HLS UNROLL
		U4 bit_cnt;
		enc_bit_est_inter_skip(&bit_cnt, mode + i);
		cost[i] = mode_sad[i] * 1000 + bit_cnt;
	}
	check_same_mv(mode, valid, cost);
	sort_skip_cand(cost, valid, mode_list);
}


void make_skip_sad_64(U7 w, U7 h,
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
	const U8 ref_y[SWH * SWW],
	const U8 org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	const S12 mc_pos[SKIP_MC_NUM][MV_D]
) {
//#pragma HLS INLINE OFF
	////#pragma HLS function_instantiate variable=cu_height

	for (int mv_idx = 0; mv_idx < SKIP_MC_NUM; ++mv_idx) {
		mc_sad_l_skip_64(w, h, mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
	}
}

void get_mvp_default_line(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_ptr, MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2], ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN])
{
	int scup = mod_info_curr->scup;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	int pic_w = md_fw_ptr->pic_width;
	int pic_h = md_fw_ptr->pic_height;
	U13 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	int h_scu = md_fw_ptr->pic_height_in_scu;
	S2 refi_nebs[NEB_NUMS] = { -1,-1 };
	//u32 scu_nebs[NEB_NUMS];
	s32 mv_nebs[NEB_NUMS][MV_D] = { 0 };
	//s16(*map_mv)[REFP_NUM][MV_D] = pic_map->map_mv;
	//s8(*map_refi)[REFP_NUM] = pic_map->map_refi;
	//u32* map_scu = pic_map->map_scu;
	int x_scu_left = 0;
	int cnt;
	int hv;
	int ptr_cur_ref;
	int mvPredType = 3;
	int rFrameL;
	int rFrameU;
	int rFrameUR;
	int neb_addr[3 + 1];
	int valid_flag[3 + 1];
	s32 refi[3];
	s32 MVPs[3][2];
	int x_scu = mod_info_curr->x_scu;
	int y_scu = mod_info_curr->y_scu;
	U7 cu_width_in_scu = cu_width >> 2;
	int offset_in_line = 0;
	//check_mvp_motion_availability(info, mod_info_curr, pic_map, neb_addr, valid_flag, lidx);
	int i = 0;
	int use_line = 0;
	int debug = 0;
	int x_scu_line[5 - 1];
	int y_scu_line[5 - 1];
	int d_inner = 1;
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
			valid_flag[0] = valid_flag[0]
				&& ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][x_scu_left][neb_addr[0]] >> 31 & ((
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
			valid_flag[0] = valid_flag[0]
				&& ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][x_scu_line[0]][y_scu_line[0]] >> 31 & ((
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
		for (i = 1; i < 3 + d_inner; i++) {

//#pragma HLS loop_tripcount max = 3
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
		for (i = 1; i < 3 + d_inner; i++) {

//#pragma HLS loop_tripcount max = 3
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

//#pragma HLS loop_tripcount max = 3
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
			refi[cnt] = ((s8)(-1));
			MVPs[cnt][0] = ((s16)0);
			MVPs[cnt][1] = ((s16)0);
		}
	}
	rFrameL = ((int)refi[0]);
	rFrameU = ((int)refi[1]);
	rFrameUR = ((int)refi[2]);
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

//#pragma HLS loop_tripcount max = 2
		s32 mva = (s32)MVPs[0][hv];
		s32 mvb = (s32)MVPs[1][hv];
		s32 mvc = (s32)MVPs[2][hv];
		if (mvPredType == 3)
		{
			if ((mva < 0 && mvb > 0 && mvc > 0) || (mva > 0 && mvb < 0 && mvc < 0)) {
				mvp[hv] = ((s16)((mvb + mvc) / 2));
			}
			else if ((mvb < 0 && mva > 0 && mvc > 0) || (mvb > 0 && mva < 0 && mvc < 0)) {
				mvp[hv] = ((s16)((mvc + mva) / 2));
			}
			else if ((mvc < 0 && mva > 0 && mvb > 0) || (mvc > 0 && mva < 0 && mvb < 0)) {
				mvp[hv] = ((s16)((mva + mvb) / 2));
			}
			else {
				s32 mva_ext = abs(mva - mvb);
				s32 mvb_ext = abs(mvb - mvc);
				s32 mvc_ext = abs(mvc - mva);
				s32 pred_vec = mva_ext < ((mvb_ext < mvc_ext ? mvb_ext : mvc_ext)) ? mva_ext : ((mvb_ext < mvc_ext ? mvb_ext :
					mvc_ext));
				if (pred_vec == mva_ext) {
					mvp[hv] = ((s16)((mva + mvb) / 2));
				}
				else if (pred_vec == mvb_ext) {
					mvp[hv] = ((s16)((mvb + mvc) / 2));
				}
				else {
					mvp[hv] = ((s16)((mvc + mva) / 2));
				}
			}
		}
		else if (mvPredType == 0)
			mvp[hv] = ((s16)mva);
		else if (mvPredType == 1)
			mvp[hv] = ((s16)mvb);
		else if (mvPredType == 2)
			mvp[hv] = ((s16)mvc);
#if MD_PRINT
		else
		{
			0 ? ((void)0) : __assert_HLS("0", "../separate/com_util.c", (unsigned int)800, __FUNCTION__);
		}
#endif
	}
	// clip MVP after rounding (rounding process might result in 32768)
	CPMV mvp_x;
	CPMV mvp_y;
	com_mv_rounding_s32((s32)mvp[0], (s32)mvp[1], &mvp_x, &mvp_y, (int)amvr_idx, (int)amvr_idx);
	mvp[0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
	mvp[1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
}

static void get_mvp_default_line_64(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2],
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)])
{
//#pragma HLS inline off
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
	x_scu_line[0] = x_scu - me_line_map_ptr_64->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_map_ptr_64->min_scu_y;
	x_scu_line[1] = x_scu - me_line_map_ptr_64->min_scu_x;
	y_scu_line[1] = y_scu - me_line_map_ptr_64->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_map_ptr_64->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_map_ptr_64->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_map_ptr_64->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_map_ptr_64->min_scu_y - 1;
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		offset_in_line = y_scu - me_line_map_ptr_64->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			valid_flag[0] = ((int)(me_line_map_ptr_64->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_map_ptr_64->map_scu_left_line[0] >> 15 & ((
					unsigned int)1)))
				&& ((int)left_nebs[0].refi[lidx]) >= 0;
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->left_nebs[neb_addr[0]].refi[lidx]);
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] = left_nebs[0].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_map_ptr_64->min_scu_y) {
			offset_in_line = y_scu - me_line_map_ptr_64->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = up_nebs[0].refi[lidx];
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_map_ptr_64->map_scu_up_line[0] >> 31 & ((
						unsigned int)1)))
					&& !((int)(me_line_map_ptr_64->map_scu_up_line[0] >> 15 & ((
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
			valid_flag[0] = ((int)(me_line_map_ptr_64->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_map_ptr_64->map_scu_left_line[0] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] =
				left_nebs[0].mv[lidx][1];
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
		for (i = 1; i < 3 + d_inner; i++) {
#pragma HLS loop_tripcount max = 3
//#pragma HLS PIPELINE
			if (valid_flag[i]) {
				refi_nebs[i] = up_nebs[neb_addr[i] - offset_in_line + 1].refi[lidx];
				valid_flag[i] = valid_flag[i]
					&& ((int)(me_line_map_ptr_64->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 31 & ((unsigned int)1)))
					&& !((int)(me_line_map_ptr_64->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 15 & ((unsigned int)1)))
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
//#pragma HLS PIPELINE II=2
			if (valid_flag[i]) {
				if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
					valid_flag[i] = 0;
				}
				if (valid_flag[i])
				{
					refi_nebs[i] = up_nebs[x_scu_line[i] - (x_scu - me_line_map_ptr_64->min_scu_x) + 1].refi[lidx];
					//valid_flag[i] = valid_flag[i] && MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[i]);
					valid_flag[i] = valid_flag[i]
						&& ((int)(me_line_map_ptr_64->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_map_ptr_64->min_scu_x) + 1] >> 31 & ((
							unsigned int)1)))
						&& !((int)(me_line_map_ptr_64->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_map_ptr_64->min_scu_x) + 1] >> 15 & ((
							unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
				}
			}
			if (valid_flag[i]) {
				mv_nebs[i][0] =
					up_nebs[x_scu_line[i] - (x_scu - me_line_map_ptr_64->min_scu_x) + 1].mv[lidx][0];
				mv_nebs[i][1] =
					up_nebs[x_scu_line[i] - (x_scu - me_line_map_ptr_64->min_scu_x) + 1].mv[lidx][1];
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

void derive_mvp_line_all_64(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO up_nebs[1 + (64 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(64 >> 2)],
	MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2])
{
	int scup = mod_info_curr->scup;
	int emvp_flag = (int)(mod_info_curr->mvp_from_hmvp_flag);
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	u8 mvr_idx = 0;
	CPMV mvp_x = 0;
	CPMV mvp_y = 0;
	if (!emvp_flag) {
		get_mvp_default_line_64(md_fw_ptr, me_line_map_ptr_64, mod_info_curr, refp_ptr, ptr, ref_list, (s8)ref_idx, mvr_idx, mvp[mvr_idx], up_nebs, left_nebs);
		for (mvr_idx = ((u8)1); mvr_idx < mvr_num; mvr_idx++) {

#pragma HLS loop_tripcount max = 4
			com_mv_rounding_s32(mvp[0][0], mvp[0][1], &mvp_x, &mvp_y, mvr_idx, mvr_idx);
			mvp[mvr_idx][0] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_x ? (((s16)0x7FFF)) : (s16)mvp_x)))));
			mvp[mvr_idx][1] = ((s16)(((((s16)0x8000)) > (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)) ? (((s16)0x8000)) : (((((s16)0x7FFF)) < mvp_y ? (((s16)0x7FFF)) : (s16)mvp_y)))));
		}
	}
	else {
		for (mvr_idx = ((u8)0); ((int)mvr_idx) < mvr_num; mvr_idx++) {

#pragma HLS loop_tripcount max = 5
			mvp[mvr_idx][0] = 0;
			mvp[mvr_idx][1] = 0;
		}
	}
}

void prepare_luma_inter_64(U7 w, U7 h, MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	SKIP_MERGE_INPUT* in, MD_COM_MODE* mod_info_curr,
	SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	int num_amvr, u32 lambda_mv, s8 refi_cur,
#if ENABLE_BFRAME
	s8 num_refp_cnt[2],
#else
	s8 num_refp_cnt,
#endif
	int ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2],
	U1 valid_mode[17], int amvr_enable_flag, ENC_FME_INFO_ALL* fme_mv_info,
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, MV_DIR* mv_dir,
#endif
	S14 sub_pel_mv[MV_D], SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
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

	U1 curr_ctu_idx = me_line_map_ptr_64->curr_ctu_idx;
	U1 above_line_idx = me_line_map_ptr_64->above_line_idx;
	U12 pix_x_scu = in->pix_x >> MIN_CU_LOG2;
	//SKIP_ELEMENT mode[SKIP_MODE_NUM];
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
 #pragma HLS loop_tripcount max = 12
		mode[i].valid = 0;
		mode[i].skip_flag = 0;
		mode[i].skip_idx = 0;
		mode[i].umve_flag = 0;
		mode[i].umve_idx = 0;
		mode[i].affine_flag = 0;
		mode[i].mvp[REFP_0][0] = 0;
		mode[i].mvp[REFP_0][1] = 0;
		mode[i].refi[REFP_0] = -1;
		mode[i].mvp[REFP_1][0] = 0;
		mode[i].mvp[REFP_1][1] = 0;
		mode[i].refi[REFP_1] = -1;
	}
	U6 num_cands_all = 0;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 pic_height_in_scu = md_fw_ptr->pic_height_in_scu;
	U1 allow_affine = 0;
	U1 run_direct_skip = 1/*cu_width * cu_height >= 64*/;
	U1 run_affine_merge = allow_affine && cu_width >= 16 && cu_height >= 16;
	NEB_CU neb[NUM_SKIP_SPATIAL_MV];
	for (int i = 0; i < 6; ++i)
	{
		neb[i].x_scu_of_ctu = 0;
		neb[i].y_scu_of_ctu = 0;
		neb[i].valid_flag = 0;
	}
	SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];
	for (int i = 0; i < 6; ++i)
	{
		neb_map[i].map_mv[REFP_0][0] = 0;
		neb_map[i].map_mv[REFP_0][1] = 0;
		neb_map[i].map_refi[REFP_0] = REFI_INVALID;
		neb_map[i].map_mv[REFP_1][0] = 0;
		neb_map[i].map_mv[REFP_1][1] = 0;
		neb_map[i].map_refi[REFP_1] = REFI_INVALID;
		neb_map[i].map_scu = 0;
		neb_map[i].map_cu_mode = 0;
	}
	S7 mode_list[MAX_INTER_SKIP_RDO];

	init_skip_merge_rdo(skip_rdo, merge_rdo, mode_list);
	derive_neb_cu_64(pic_width_in_scu, x_scu, y_scu, cu_width, cu_height,
		neb, neb_map,
		me_line_map_ptr_64->map_scu_up_line, me_line_map_ptr_64->map_cu_mode_up_line,
		me_line_map_ptr_64->map_scu_left_line, me_line_map_ptr_64->map_cu_mode_left_line,
		curr_ctu_idx,
		up_nebs, left_nebs);
	derive_inter_cands_64(in, (U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr
#if SUB_TMVP
		, mod_info_curr->cu_width, mod_info_curr->cu_height, core_sbtmvp
#endif
	);
	if (core_sbtmvp->isValid && md_fw_ptr->num_refp[REFP_0] > 1)
	{
		core_sbtmvp->isValid = FALSE;
	}
#if SKIP_RESTRI
	check_skip_mv_inbound(mode, x_pos, y_pos, md_input_ptr->CtrPos2MD, cu_width, cu_height, mod_info_curr
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
	output_skip_mode_64(skip_rdo, skip_rdo_mode, mode, mode_list);
	//print_skip_mode(mod_info_curr, skip_rdo, skip_rdo_mode, merge_rdo);

	mod_info_curr->affine_flag = 0;
	mod_info_curr->mvp_from_hmvp_flag = 0;


	if (w == 64 && h == 64)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree64x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree64x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block64x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block64x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (w == 64 && h == 32)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree64x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree64x32Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block64x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block64x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (w == 64 && h == 16)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree64x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree64x16Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block64x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block64x16MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (w == 64 && h == 8)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree64x8Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree64x8Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block64x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block64x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (w == 32 && h == 64)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree32x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree32x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block32x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block64x32MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else if (w == 16 && h == 64)
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree16x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree16x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block16x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block16x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
	else
	{
		sub_pel_mv[MV_X] = fme_mv_info->sadTree8x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_X];
		sub_pel_mv[MV_Y] = fme_mv_info->sadTree8x64Fmv[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][MV_Y];
#if ENABLE_BFRAME
		mv_dir->cur_refi = me_mv_dir->block8x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
		mv_dir->lidx = me_mv_dir->block8x64MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
	}
#if ENABLE_BFRAME
	for (refi_cur = 0; refi_cur < num_refp_cnt[mv_dir->lidx]; refi_cur++)
	{
#pragma HLS loop_tripcount max = 2
		derive_mvp_line_all_64(md_fw_ptr, me_line_map_ptr_64, up_nebs, left_nebs, mod_info_curr, ptr, mv_dir->lidx, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#else
	for (refi_cur = 0; refi_cur < num_refp_cnt; refi_cur++)
	{
		derive_mvp_line_all_64(md_fw_ptr, me_line_map_ptr_64, up_nebs, left_nebs, mod_info_curr, ptr, 0, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);
	}
#endif

	for (int x = 0; x < 4; x++) {
		if (x < skip_rdo->num_rdo)
		{
			valid_mode[x] = 1;
		}
		else
		{
			valid_mode[x] = 0;
		}
#if USE_INTRA_REFRESH
		if (md_input_ptr->EncRefresh && md_fw_ptr->slice_type != SLICE_I)
		{
			if (md_input_ptr->RefreshMode == 0)
			{
				if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
					valid_mode[x] = 0;
			}
			else
			{
				if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
					valid_mode[x] = 0;
			}

		}
#endif
	}

	for (int y = 0; y < 5; y++) {

			valid_mode[y + 4] = 0;

#if USE_INTRA_REFRESH
		if (md_input_ptr->EncRefresh && md_fw_ptr->slice_type != SLICE_I)
		{
			if (md_input_ptr->RefreshMode == 0)
			{
				if (y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
					valid_mode[y + 4] = 0;
			}
			else
			{
				if (x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
					valid_mode[y + 4] = 0;
			}

		}
#endif
	}

	for (int z = 0; z < 5; z++) {
		//if (amvr_enable_flag)
		//	valid_mode[z + 9] = 1;
		//else
		{
			valid_mode[9] = 1;
#if USE_INTRA_REFRESH
			if (md_input_ptr->EncRefresh && md_fw_ptr->slice_type != SLICE_I)
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

	for (int p = 0; p < 3; p++) {
		//if (w >= 16 && h >= 16)
		//{
		//	if (allow_affine)
		//	{
		//		//if (!(core_inner->mod_info_best.cu_mode == 2 && !core_inner->mod_info_best.affine_flag)) {
		//		if (amvr_enable_flag)
		//			valid_mode[p + 14] = 1;
		//		else
		//		{
		//			valid_mode[14] = 1;
		//			valid_mode[15] = 0;
		//			valid_mode[16] = 0;
		//		}
		//	}
		//}
		//else
		valid_mode[p + 14] = 0;
	}
}


void get_IP_pixel_64(U8 w, U8 h, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel left[2 * 64 + 3], pel up[2 * 64 + 3], U14 x, U14 y,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 64>* IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>* IP_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	IP_MAP_SCU_SIZE<8, 64>* IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>* IP_map_scu_ptr_64x8)
{
	int i;
	int j;
	int width_in_scu = ch_type == 0 ? w >> 2 : w >> (2 - 1);
	int height_in_scu = ch_type == 0 ? h >> 2 : h >> (2 - 1);
	//4
	U4 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
	U2 unit_size_bit = ch_type == 0 ? 2 : 1;
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
	int pic_x = x % 64;
	int pic_y = y % 64;
	int pic_x_scu = pic_x >> 2;
	int pic_y_scu = pic_y >> 2;
	int flag;

	S7 ip_array_size_idx = get_ip_array_size_idx(w, h);
#if MD_PRINT
	assert(ip_array_size_idx != -1);
#endif

	if (w == 64 && h == 64)
	{
		if ((avail_cu & 1 << 0) == 1 << 0)
		{
			int w_temp = (pic_width_in_scu << 2) - x;
			for (i = 0; i < (w_temp >= 128 ? 128 : w_temp); i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=128
				up[i + 3] = IP_buffer_ptr_64x64->up_line[i];
			}
			for (i = (w_temp >= 128 ? 128 : w_temp); i < 128; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=128
				up[i + 3] = up[w_temp + 2];
			}
		}
		if ((avail_cu & 1 << 1) == 1 << 1)
		{
			for (i = 0; i < 64; i++)
			{
				left[i + 3] = IP_buffer_ptr_64x64->left_line[i];
			}
			for (int i = 64; i < 128; ++i)
			{
				left[i + 3] = left[66];
			}
		}
		if ((avail_cu & 1 << 2) == 1 << 2)
		{
			up[2] = left[2] = IP_buffer_ptr_64x64->top_left;
		}
		else if ((avail_cu & 1 << 0) == 1 << 0)
		{
			up[2] = left[2] = up[3];
		}
		else if ((avail_cu & 1 << 1) == 1 << 1)
		{
			up[2] = left[2] = left[3];
		}
	}
	//	else
	//	{
	//		if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
	//			if (pic_y == 0) {
	//				for (i = 0; i < w; i++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=8
	//					up[i + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + i];
	//
	//				}
	//				for (i = 0; i < pad_up_in_scu; i++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=2
	//					if (x_scu + width_in_scu + i < pic_width_in_scu) {
	//						flag = ((int)(MAP_SCU_IP_ptr->map_scu_above_line[MAP_SCU_IP_ptr->above_line_idx][x_scu + width_in_scu + i] >> 31 & ((
	//							unsigned int)1)));
	//					}
	//					else {
	//						flag = 0;
	//					}
	//					if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
	//						for (j = 0; j < unit_size; j++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x + w + (i << unit_size_bit) + j];
	//						}
	//					}
	//					else {
	//						for (j = 0; j < unit_size; j++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
	//						}
	//					}
	//				}
	//			}
	//			else {
	//				for (i = 0; i < w; i++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=8
	//					up[i + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + i];
	//				}
	//				for (i = 0; i < pad_up_in_scu; i++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=2
	//					if (pic_x_scu + width_in_scu + i < 16) {
	//						flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1]
	//							>> 31 & ((unsigned int)1)));
	//					}
	//					else {
	//						flag = 0;
	//					}
	//					if (x_scu + width_in_scu + i < pic_width_in_scu && flag) {
	//						for (j = 0; j < unit_size; j++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + w + (i << unit_size_bit) + j];
	//						}
	//					}
	//					else {
	//						for (j = 0; j < unit_size; j++) {
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							up[w + (i << unit_size_bit) + j + 3] = up[w + (i << unit_size_bit) + 2];
	//						}
	//					}
	//				}
	//			}
	//		}
	//		if ((((int)avail_cu) & 1 << 1) == 1 << 1)
	//		{
	//			if (pic_x == 0)
	//			{
	//				for (i = 0; i < h; i++)
	//				{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=8
	//					left[i + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + i];
	//				}
	//				for (i = 0; i < pad_le_in_scu; i++)
	//				{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=2
	//					if (pic_y_scu + height_in_scu + i < 16)
	//					{
	//						flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31
	//							& ((unsigned int)1)));
	//					}
	//					else
	//					{
	//						flag = 0;
	//					}
	//					if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
	//					{
	//						for (j = 0; j < unit_size; j++)
	//						{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + h + (i << unit_size_bit) + j];
	//
	//						}
	//					}
	//					else
	//					{
	//						for (j = 0; j < unit_size; j++)
	//						{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
	//						}
	//					}
	//				}
	//			}
	//			else
	//			{
	//				for (i = 0; i < h; i++)
	//				{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=8
	//					left[i + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][pic_y + i][(pic_x >> 2) - 1];
	//
	//				}
	//				for (i = 0; i < pad_le_in_scu; i++)
	//				{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=2
	//					if (pic_y_scu + height_in_scu + i < 16)
	//					{
	//						flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i]
	//							>> 31 & ((unsigned int)1)));
	//					}
	//					else
	//					{
	//						flag = 0;
	//					}
	//					if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
	//					{
	//						for (j = 0; j < unit_size; j++)
	//						{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							left[h + (i << unit_size_bit) + j + 3] = IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][h + (i << unit_size_bit) + j + pic_y][(pic_x >> 2) - 1];
	//						}
	//					}
	//					else
	//					{
	//						for (j = 0; j < unit_size; j++)
	//						{
	//
	////#pragma HLS LOOP_TRIPCOUNT max=4
	//							left[h + (i << unit_size_bit) + j + 3] = left[h + (i << unit_size_bit) + 2];
	//						}
	//					}
	//				}
	//			}
	//		}
	//		if ((((int)avail_cu) & 1 << 2) == 1 << 2)
	//		{
	//			if (pic_y == 0 && pic_x == 0)
	//			{
	//				up[2] = left[2] = ((pel)(IP_buffer_ptr->top_left));
	//			}
	//			else if (pic_y == 0)
	//			{
	//				up[2] = left[2] = IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x - 1];
	//			}
	//			else if (pic_x == 0)
	//			{
	//				up[2] = left[2] = IP_buffer_ptr->left_size[ip_array_size_idx][pic_y - 1];
	//			}
	//			else
	//			{
	//				up[2] = left[2] = IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x - 1];
	//
	//			}
	//		}
	//		else if ((((int)avail_cu) & 1 << 0) == 1 << 0)
	//		{
	//			up[2] = left[2] = up[3];
	//		}
	//		else if ((((int)avail_cu) & 1 << 1) == 1 << 1)
	//		{
	//			up[2] = left[2] = left[3];
	//		}
	//	}
	else if (w == 64 && h == 32)
	{
		IP_buffer_ptr_64x32->const_up(up, IP_map_scu_ptr_64x32, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu);
		IP_buffer_ptr_64x32->const_left(left, IP_map_scu_ptr_64x32, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu);
		IP_buffer_ptr_64x32->const_top_left(up, left, avail_cu);
	}
	else if (w == 32 && h == 64)
	{
		IP_buffer_ptr_32x64->const_up(up, IP_map_scu_ptr_32x64, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu);
		IP_buffer_ptr_32x64->const_left(left, IP_map_scu_ptr_32x64, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu);
		IP_buffer_ptr_32x64->const_top_left(up, left, avail_cu);
	}
	else if (w == 64 && h == 16)
	{
		IP_buffer_ptr_64x16->const_up(up, IP_map_scu_ptr_64x16, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu);
		IP_buffer_ptr_64x16->const_left(left, IP_map_scu_ptr_64x16, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu);
		IP_buffer_ptr_64x16->const_top_left(up, left, avail_cu);
	}
	else if (w == 16 && h == 64)
	{
		IP_buffer_ptr_16x64->const_up(up, IP_map_scu_ptr_16x64, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu);
		IP_buffer_ptr_16x64->const_left(left, IP_map_scu_ptr_16x64, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu);
		IP_buffer_ptr_16x64->const_top_left(up, left, avail_cu);
	}
	else if (w == 64 && h == 8)
	{
		IP_buffer_ptr_64x8->const_up(up, IP_map_scu_ptr_64x8, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu);
		IP_buffer_ptr_64x8->const_left(left, IP_map_scu_ptr_64x8, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu);
		IP_buffer_ptr_64x8->const_top_left(up, left, avail_cu);
	}
	else if (w == 8 && h == 64)
	{
		IP_buffer_ptr_8x64->const_up(up, IP_map_scu_ptr_8x64, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu);
		IP_buffer_ptr_8x64->const_left(left, IP_map_scu_ptr_8x64, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu);
		IP_buffer_ptr_8x64->const_top_left(up, left, avail_cu);
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
						assert(up[w + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x + w + (i << unit_size_bit) + j]);
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
	if ((((int)avail_cu) & 1 << 1) == 1 << 1)
	{
		if (pic_x == 0)
		{
			for (i = 0; i < h; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				assert(left[i + 3] == IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[1 - MAP_SCU_IP_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31
						& ((unsigned int)1)));
				}
				else
				{
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[h + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->left_size[ip_array_size_idx][pic_y + h + (i << unit_size_bit) + j]);

					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[h + (i << unit_size_bit) + j + 3] == left[h + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
		else
		{
			for (i = 0; i < h; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				assert(left[i + 3] == IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][pic_y + i][(pic_x >> 2) - 1]);

			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					flag = ((int)(MAP_SCU_IP_ptr->map_scu_inner[MAP_SCU_IP_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i]
						>> 31 & ((unsigned int)1)));
				}
				else
				{
					flag = 0;
				}
				if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[h + (i << unit_size_bit) + j + 3] == IP_buffer_ptr->left_innerLB_size[ip_array_size_idx][h + (i << unit_size_bit) + j + pic_y][(pic_x >> 2) - 1]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						assert(left[h + (i << unit_size_bit) + j + 3] == left[h + (i << unit_size_bit) + 2]);
					}
				}
			}
		}
	}
	if ((((int)avail_cu) & 1 << 2) == 1 << 2)
	{
		if (pic_y == 0 && pic_x == 0)
		{
			assert(up[2] == ((pel)(IP_buffer_ptr->top_left)));
		}
		else if (pic_y == 0)
		{
			assert(up[2] == IP_buffer_ptr->up_temp_size[ip_array_size_idx][pic_x - 1]);
		}
		else if (pic_x == 0)
		{
			assert(up[2] == IP_buffer_ptr->left_size[ip_array_size_idx][pic_y - 1]);
		}
		else
		{
			assert(up[2] == IP_buffer_ptr->up_innerLB_size[ip_array_size_idx][(pic_y >> 2) - 1][pic_x - 1]);

		}
	}
#endif

	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}


static int get_rmd_result_64(U3 cu_width_log2, U3 cu_heght_log2, U13 x, U13 y, u8 ipred_list[],
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE]
	, U8 speed_level
#else
	u8 rmd_mode[IPD_RDO_CNT]
#endif
#if EIPM
	, int eipm_enable_flag
#endif
)
{
	int i;
	int w = cu_width_log2 - 2;
	int h = cu_heght_log2 - 2;
	int x0 = x >> 2;
	int y0 = y >> 2;
	int idx = get_g_map_idx(w, h, x0, y0);
	//int cnt = ((((cu_width_log2 - cu_heght_log2) >= 2) || ((cu_heght_log2 - cu_width_log2) >= 2)) ? (IPD_RDO_CNT - 1) : IPD_RDO_CNT);
#if USE_SPEED_LEVEL
	int cnt = IPD_RDO_CNT;
	if (speed_level == 1)
		cnt = IPD_RDO_CNT; //5
	else if (speed_level == 2)
		cnt = EXTRA_RDO_MODE;//10
	else
		cnt = IPD_RDO_CNT + EXTRA_RDO_MODE;//15
#else
	int cnt = IPD_RDO_CNT;
#endif
#if EIPM && !USE_EIPM_RMD
	if (eipm_enable_flag)
	{


		for (int i = 0; i < IPD_RDO_CNT; i++)
		{
#pragma HLS loop_tripcount max = 5
			if (rmd_mode[i] >= 3 && rmd_mode[i] <= 31)
			{
				if (rmd_mode[i] == 3)
				{
					ipred_list[5] = 34;
				}
				else if (rmd_mode[i] > 3 && rmd_mode[i] <= 11)
				{
					ipred_list[5] = rmd_mode[i] + 30;
					ipred_list[6] = rmd_mode[i] + 31;
				}
				else if (rmd_mode[i] == 12)
				{
					ipred_list[5] = 43;
					ipred_list[6] = 44;
				}
				else if (rmd_mode[i] > 12 && rmd_mode[i] <= 23)
				{
					ipred_list[5] = rmd_mode[i] + 32;
					ipred_list[6] = rmd_mode[i] + 33;
				}
				else if (rmd_mode[i] == 24)
				{
					ipred_list[5] = 57;
					ipred_list[6] = 58;
				}
				else if (rmd_mode[i] > 24 && rmd_mode[i] <= 30)
				{
					ipred_list[5] = rmd_mode[i] + 34;
					ipred_list[6] = rmd_mode[i] + 35;
				}
				else
				{
					ipred_list[5] = 65;
				}
				break;
			}
		}
	}
#endif
	for (i = 0; i < cnt; i++) {
#pragma HLS loop_tripcount max = 5
		ipred_list[i] = rmd_mode[i];
	}

	return cnt;
}

u16 com_get_avail_intra_ip_64x64(IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_64x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_64x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_64x64->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_64x32(IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x32->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x32->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_64x32->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_64x32->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_64x32->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x32->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x32->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_32x64(IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_32x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_32x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_32x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_32x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_32x64->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_32x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_32x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_64x16(IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x16->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x16->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_64x16->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_64x16->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_64x16->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x16->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x16->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_16x64(IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_16x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_16x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_16x64->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_64x8(IP_MAP_SCU_SIZE<64, 8>* IP_map_scu_ptr_64x8, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x8->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x8->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_64x8->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_64x8->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_64x8->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x8->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_64x8->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_8x64(IP_MAP_SCU_SIZE<8, 64>* IP_map_scu_ptr_8x64, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x64->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_8x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_8x64->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_8x64->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x64->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}

void com_get_mpm_64x64(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x64->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x64->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x64->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x64->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}
void com_get_mpm_64x32(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x32->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x32->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x32->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x32->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}
void com_get_mpm_32x64(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_32x64->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_32x64->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_32x64->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_32x64->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}
void com_get_mpm_64x16(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<64, 16>* MPM_buffer_ptr_64x16, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x16->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x16->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x16->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x16->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}
void com_get_mpm_16x64(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<16, 64>* MPM_buffer_ptr_16x64, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_16x64->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_16x64->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_16x64->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_16x64->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}
void com_get_mpm_64x8(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<64, 8>* MPM_buffer_ptr_64x8, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x8->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_64x8->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x8->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_64x8->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}
void com_get_mpm_8x64(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<8, 64>* MPM_buffer_ptr_8x64, ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64)
{
//#pragma HLS INLINE off
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = x_scu - me_line_map_ptr_64->min_scu_x;
	int pic_y_scu = y_scu - me_line_map_ptr_64->min_scu_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (x_scu == me_line_map_ptr_64->min_scu_x) {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_8x64->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_64->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_8x64->left;
			valid_l = 1;
		}
	}
	if (y_scu == me_line_map_ptr_64->min_scu_y) {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_8x64->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_64->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_8x64->up;
			valid_u = 1;
		}
	}
	mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}
}

void prepare_intra_info_64(U7 w, U7 h, MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	MD_COM_MODE* mod_info_curr_ptr,
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
	U3 cu_width_log2, U3 cu_height_log2, U13 pix_x, U13 pix_y, U14 sub_x, U14 sub_y, u8 ipred_list[IPRED_LIST_NUM], int* pred_cnt, pel src_le_temp[2 * 64 + 3], pel src_up_temp[2 * 64 + 3], u16* avail_tb,
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE]
	, U8 speed_level
#else
	u8 rmd_mode[IPD_RDO_CNT]
#endif
)
{
	int pb_x_scu = mod_info_curr_ptr->x_pos >> 2;
	int pb_y_scu = mod_info_curr_ptr->y_pos >> 2;
	int tb_x_scu;
	int tb_y_scu;
	int i;
	int bit_depth;
	U8 width = w;
	U8 height = h;
	if (w == 64 && h == 64)
	{
		com_get_mpm_64x64(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_64x64, me_line_map_ptr_64);
	}
	else if (w == 64 && h == 32)
	{
		com_get_mpm_64x32(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_64x32, me_line_map_ptr_64);
	}
	else if (w == 32 && h == 64)
	{
		com_get_mpm_32x64(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_32x64, me_line_map_ptr_64);
	}
	else if (w == 64 && h == 16)
	{
		com_get_mpm_64x16(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_64x16, me_line_map_ptr_64);
	}
	else if (w == 16 && h == 64)
	{
		com_get_mpm_16x64(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_16x64, me_line_map_ptr_64);
	}
	else if (w == 64 && h == 8)
	{
		com_get_mpm_64x8(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_64x8, me_line_map_ptr_64);
	}
	else if (w == 8 && h == 64)
	{
		com_get_mpm_8x64(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_8x64, me_line_map_ptr_64);
	}
	*pred_cnt = get_rmd_result_64(cu_width_log2, cu_height_log2, pix_x, pix_y, ipred_list, rmd_mode
#if USE_SPEED_LEVEL
		, speed_level
#endif
#if EIPM
		, md_fw_ptr->eipm_enable_flag
#endif
	);
	bit_depth = md_fw_ptr->bit_depth_internal;
	tb_x_scu = sub_x >> 2;
	tb_y_scu = sub_y >> 2;

	if (w == 64 && h == 64)
	{
		*avail_tb = com_get_avail_intra_ip_64x64(IP_map_scu_ptr_64x64, tb_x_scu, tb_y_scu);
	}
	else if (w == 64 && h == 32)
	{
		*avail_tb = com_get_avail_intra_ip_64x32(IP_map_scu_ptr_64x32, tb_x_scu, tb_y_scu);
	}
	else if (w == 32 && h == 64)
	{
		*avail_tb = com_get_avail_intra_ip_32x64(IP_map_scu_ptr_32x64, tb_x_scu, tb_y_scu);
	}
	else if (w == 64 && h == 16)
	{
		*avail_tb = com_get_avail_intra_ip_64x16(IP_map_scu_ptr_64x16, tb_x_scu, tb_y_scu);
	}
	else if (w == 16 && h == 64)
	{
		*avail_tb = com_get_avail_intra_ip_16x64(IP_map_scu_ptr_16x64, tb_x_scu, tb_y_scu);
	}
	else if (w == 64 && h == 8)
	{
		*avail_tb = com_get_avail_intra_ip_64x8(IP_map_scu_ptr_64x8, tb_x_scu, tb_y_scu);
	}
	else if (w == 8 && h == 64)
	{
		*avail_tb = com_get_avail_intra_ip_8x64(IP_map_scu_ptr_8x64, tb_x_scu, tb_y_scu);
	}

	for (i = 0; i < ((height << 1) + 3); i++) {
 #pragma HLS loop_tripcount max = 131
		src_le_temp[i] = (1 << (bit_depth - 1));
	}
	for (i = 0; i < ((width << 1) + 3); i++) {
 #pragma HLS loop_tripcount max = 131
		src_up_temp[i] = (1 << (bit_depth - 1));
	}
	get_IP_pixel_64(width, height, *avail_tb, md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu, bit_depth, 0, src_le_temp, src_up_temp, sub_x, sub_y,
		IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32, IP_buffer_ptr_16x64, IP_buffer_ptr_64x16, IP_buffer_ptr_8x64, IP_buffer_ptr_64x8,
		IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32, IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16, IP_map_scu_ptr_8x64, IP_map_scu_ptr_64x8);
}



void cu_plane_nz_cln_64(int dst[4][3], U3 plane)
{

//#pragma HLS INLINE off
	int i;
	for (i = 0; i < 4; i++) {

//#pragma HLS LOOP_TRIPCOUNT max=4
		dst[i][plane] = 0;
	}
}

void get_tmp_8(int j, pel dst[64][64], int tmp[8])
{
	dst[0][(j + 0) & 63] = tmp[0];
	dst[4][(j + 4) & 63] = tmp[1];
	dst[2][(j + 2) & 63] = tmp[2];
	dst[6][(j + 6) & 63] = tmp[3];
	dst[1][(j + 1) & 63] = tmp[4];
	dst[3][(j + 3) & 63] = tmp[5];
	dst[5][(j + 5) & 63] = tmp[6];
	dst[7][(j + 7) & 63] = tmp[7];
}

void get_tmp_16(int j, pel dst[64][64], int tmp[16]) 
{
	dst[0][(j + 0) & 63] = tmp[0];
	dst[8][(j + 8) & 63] = tmp[1];
	dst[4][(j + 4) & 63] = tmp[2];
	dst[12][(j + 12) & 63] = tmp[3];
	dst[2][(j + 2) & 63] = tmp[4];
	dst[6][(j + 6) & 63] = tmp[5];
	dst[10][(j + 10) & 63] = tmp[6];
	dst[14][(j + 14) & 63] = tmp[7];
	dst[1][(j + 1) & 63] = tmp[8];
	dst[3][(j + 3) & 63] = tmp[9];
	dst[5][(j + 5) & 63] = tmp[10];
	dst[7][(j + 7) & 63] = tmp[11];
	dst[9][(j + 9) & 63] = tmp[12];
	dst[11][(j + 11) & 63] = tmp[13];
	dst[13][(j + 13) & 63] = tmp[14];
	dst[15][(j + 15) & 63] = tmp[15];
}

void get_tmp_32(int j, pel dst[64][64], int tmp[32])
{
	dst[0][(j + 0) & 63] = tmp[0];
	dst[16][(j + 16) & 63] = tmp[1];
	dst[8][(j + 8) & 63] = tmp[2];
	dst[24][(j + 24) & 63] = tmp[3];
	dst[4][(j + 4) & 63] = tmp[4];
	dst[12][(j + 12) & 63] = tmp[5];
	dst[20][(j + 20) & 63] = tmp[6];
	dst[28][(j + 28) & 63] = tmp[7];
	dst[2][(j + 2) & 63] = tmp[8];
	dst[6][(j + 6) & 63] = tmp[9];
	dst[10][(j + 10) & 63] = tmp[10];
	dst[14][(j + 14) & 63] = tmp[11];
	dst[18][(j + 18) & 63] = tmp[12];
	dst[22][(j + 22) & 63] = tmp[13];
	dst[26][(j + 26) & 63] = tmp[14];
	dst[30][(j + 30) & 63] = tmp[15];
	dst[1][(j + 1) & 63] = tmp[16];
	dst[3][(j + 3) & 63] = tmp[17];
	dst[5][(j + 5) & 63] = tmp[18];
	dst[7][(j + 7) & 63] = tmp[19];
	dst[9][(j + 9) & 63] = tmp[20];
	dst[11][(j + 11) & 63] = tmp[21];
	dst[13][(j + 13) & 63] = tmp[22];
	dst[15][(j + 15) & 63] = tmp[23];
	dst[17][(j + 17) & 63] = tmp[24];
	dst[19][(j + 19) & 63] = tmp[25];
	dst[21][(j + 21) & 63] = tmp[26];
	dst[23][(j + 23) & 63] = tmp[27];
	dst[25][(j + 25) & 63] = tmp[28];
	dst[27][(j + 27) & 63] = tmp[29];
	dst[29][(j + 29) & 63] = tmp[30];
	dst[31][(j + 31) & 63] = tmp[31];
}

void get_tmp_64(int j, pel dst[64][64], int tmp[64])
{
	dst[0][(j + 0) & 63] = tmp[0];
	dst[32][(j + 32) & 63] = tmp[1];
	dst[16][(j + 16) & 63] = tmp[2];
	dst[48][(j + 48) & 63] = tmp[3];
	dst[8][(j + 8) & 63] = tmp[4];
	dst[24][(j + 24) & 63] = tmp[5];
	dst[40][(j + 40) & 63] = tmp[6];
	dst[56][(j + 56) & 63] = tmp[7];

	dst[4][(j + 4) & 63] = tmp[8];
	dst[12][(j + 12) & 63] = tmp[9];
	dst[20][(j + 20) & 63] = tmp[10];
	dst[28][(j + 28) & 63] = tmp[11];
	dst[36][(j + 36) & 63] = tmp[12];
	dst[44][(j + 44) & 63] = tmp[13];
	dst[52][(j + 52) & 63] = tmp[14];
	dst[60][(j + 60) & 63] = tmp[15];

	dst[2][(j + 2) & 63] = tmp[16];
	dst[6][(j + 6) & 63] = tmp[17];
	dst[10][(j + 10) & 63] = tmp[18];
	dst[14][(j + 14) & 63] = tmp[19];
	dst[18][(j + 18) & 63] = tmp[20];
	dst[22][(j + 22) & 63] = tmp[21];
	dst[26][(j + 26) & 63] = tmp[22];
	dst[30][(j + 30) & 63] = tmp[23];
	dst[34][(j + 34) & 63] = tmp[24];
	dst[38][(j + 38) & 63] = tmp[25];
	dst[42][(j + 42) & 63] = tmp[26];
	dst[46][(j + 46) & 63] = tmp[27];
	dst[50][(j + 50) & 63] = tmp[28];
	dst[54][(j + 54) & 63] = tmp[29];
	dst[58][(j + 58) & 63] = tmp[30];
	dst[62][(j + 62) & 63] = tmp[31];

	dst[1][(j + 1) & 63] = tmp[32];
	dst[3][(j + 3) & 63] = tmp[33];
	dst[5][(j + 5) & 63] = tmp[34];
	dst[7][(j + 7) & 63] = tmp[35];
	dst[9][(j + 9) & 63] = tmp[36];
	dst[11][(j + 11) & 63] = tmp[37];
	dst[13][(j + 13) & 63] = tmp[38];
	dst[15][(j + 15) & 63] = tmp[39];
	dst[17][(j + 17) & 63] = tmp[40];
	dst[19][(j + 19) & 63] = tmp[41];
	dst[21][(j + 21) & 63] = tmp[42];
	dst[23][(j + 23) & 63] = tmp[43];
	dst[25][(j + 25) & 63] = tmp[44];
	dst[27][(j + 27) & 63] = tmp[45];
	dst[29][(j + 29) & 63] = tmp[46];
	dst[31][(j + 31) & 63] = tmp[47];
	dst[33][(j + 33) & 63] = tmp[48];
	dst[35][(j + 35) & 63] = tmp[49];
	dst[37][(j + 37) & 63] = tmp[50];
	dst[39][(j + 39) & 63] = tmp[51];
	dst[41][(j + 41) & 63] = tmp[52];
	dst[43][(j + 43) & 63] = tmp[53];
	dst[45][(j + 45) & 63] = tmp[54];
	dst[47][(j + 47) & 63] = tmp[55];
	dst[49][(j + 49) & 63] = tmp[56];
	dst[51][(j + 51) & 63] = tmp[57];
	dst[53][(j + 53) & 63] = tmp[58];
	dst[55][(j + 55) & 63] = tmp[59];
	dst[57][(j + 57) & 63] = tmp[60];
	dst[59][(j + 59) & 63] = tmp[61];
	dst[61][(j + 61) & 63] = tmp[62];
	dst[63][(j + 63) & 63] = tmp[63];
}

void dct_row_64(U7 w, U7 h, s16 src[64][64], s16 dst[64][64], U4 shift)
{
#pragma HLS ARRAY_PARTITION variable=dst cyclic factor=64 dim=2
#pragma HLS ARRAY_PARTITION variable=src cyclic factor=16 dim=2
	U7 size = 64;
	U8 size_b = size - 1;
	int E[64][32];
	int O[64][32];
	int EE[64][16];
	int EO[64][16];
	int EEE[64][8];
	int EEO[64][8];
	int EEEE[64][4];
	int EEEO[64][4];
	int EEEEE[64][2];
	int EEEEO[64][2];
	int j;
	int k;
	int tmp[64];
#pragma HLS ARRAY_PARTITION variable=E cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=O cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=EE cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=EO cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=EEE cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=EEO cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=EEEE cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=EEEO cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=EEEEE complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEEEO complete dim=2
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=0
	int add = 1 << (shift - 1);

	for (j = 0; j < h; j++) {
#pragma HLS PIPELINE II=4
#pragma HLS loop_tripcount max=64
		//E / O
		for (k = 0; k < 32; k++) {
//#pragma HLS UNROLL
			E[j][k] = (src[j][k]) + (src[j][63 - k]);
			O[j][k] = (src[j][k]) - (src[j][63 - k]);
		}
		// EE / EO
		if (w == 64) {
			for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
				EE[j][k] = E[j][k] + E[j][31 - k];
				EO[j][k] = E[j][k] - E[j][31 - k];
			}
		}
		else if (w == 32) {
			for (k = 0; k < 16; k++)
			{
//#pragma HLS UNROLL
				EE[j][k] = src[j][k] + src[j][31 - k];
				EO[j][k] = src[j][k] - src[j][31 - k];
			}
		}
		// EEE / EEO
		if (w == 64 || w == 32) {
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				EEE[j][k] = EE[j][k] + EE[j][15 - k];
				EEO[j][k] = EE[j][k] - EE[j][15 - k];
			}
		}
		else if (w == 16) {
			for (k = 0; k < 8; k++)
			{
//#pragma HLS UNROLL
				EEE[j][k] = src[j][k] + src[j][15 - k];
				EEO[j][k] = src[j][k] - src[j][15 - k];
			}
		}
		//EEEE / EEEO
		if (w == 64 || w == 32 || w == 16) {
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				EEEE[j][k] = EEE[j][k] + EEE[j][7 - k];
				EEEO[j][k] = EEE[j][k] - EEE[j][7 - k];
			}
		}
		else if (w == 8) {
			for (k = 0; k < 4; k++)
			{
//#pragma HLS UNROLL
				EEEE[j][k] = src[j][k] + src[j][7 - k];
				EEEO[j][k] = src[j][k] - src[j][7 - k];
			}
		}

		//EEEEE / EEEEO
		EEEEE[j][0] = EEEE[j][0] + EEEE[j][3];
		EEEEO[j][0] = EEEE[j][0] - EEEE[j][3];
		EEEEE[j][1] = EEEE[j][1] + EEEE[j][2];
		EEEEO[j][1] = EEEE[j][1] - EEEE[j][2];
		tmp[0] = (((32 * EEEEE[j][0] + 32 * EEEEE[j][1] + add) >> shift));
		tmp[1] = (((32 * EEEEE[j][0] - 32 * EEEEE[j][1] + add) >> shift));
		tmp[2] = (((42 * EEEEO[j][0] + 17 * EEEEO[j][1] + add) >> shift));
		tmp[3] = (((17 * EEEEO[j][0] - 42 * EEEEO[j][1] + add) >> shift));

		tmp[4] = ((44 * EEEO[j][0] + 38 * EEEO[j][1] + 25 * EEEO[j][2] + 9 * EEEO[j][3] + add) >> shift);
		tmp[5] = ((38 * EEEO[j][0] - 9 * EEEO[j][1] - 44 * EEEO[j][2] - 25 * EEEO[j][3] + add) >> shift);
		tmp[6] = ((25 * EEEO[j][0] - 44 * EEEO[j][1] + 9 * EEEO[j][2] + 38 * EEEO[j][3] + add) >> shift);
		tmp[7] = ((9 * EEEO[j][0] - 25 * EEEO[j][1] + 38 * EEEO[j][2] - 44 * EEEO[j][3] + add) >> shift);

		tmp[8] = ((((45) * EEO[j][0] + (43) * EEO[j][1] + (40) * EEO[j][2] + (35) * EEO[j][3] + (29) * EEO[j][4] + (21) * EEO[j][5] + (13) * EEO[j][6] + (4) * EEO[j][7] + add) >> shift));
		tmp[9] = ((((43) * EEO[j][0] + (29) * EEO[j][1] + (4) * EEO[j][2] + (-21) * EEO[j][3] + (-40) * EEO[j][4] + (-45) * EEO[j][5] + (-35) * EEO[j][6] + (-13) * EEO[j][7] + add) >> shift));
		tmp[10] = ((((40) * EEO[j][0] + (4) * EEO[j][1] + (-35) * EEO[j][2] + (-43) * EEO[j][3] + (-13) * EEO[j][4] + (29) * EEO[j][5] + (45) * EEO[j][6] + (21) * EEO[j][7] + add) >> shift));
		tmp[11] = ((((35) * EEO[j][0] + (-21) * EEO[j][1] + (-43) * EEO[j][2] + (4) * EEO[j][3] + (45) * EEO[j][4] + (13) * EEO[j][5] + (-40) * EEO[j][6] + (-29) * EEO[j][7] + add) >> shift));
		tmp[12] = ((((29) * EEO[j][0] + (-40) * EEO[j][1] + (-13) * EEO[j][2] + (45) * EEO[j][3] + (-4) * EEO[j][4] + (-43) * EEO[j][5] + (21) * EEO[j][6] + (35) * EEO[j][7] + add) >> shift));
		tmp[13] = ((((21) * EEO[j][0] + (-45) * EEO[j][1] + (29) * EEO[j][2] + (13) * EEO[j][3] + (-43) * EEO[j][4] + (35) * EEO[j][5] + (4) * EEO[j][6] + (-40) * EEO[j][7] + add) >> shift));
		tmp[14] = ((((13) * EEO[j][0] + (-35) * EEO[j][1] + (45) * EEO[j][2] + (-40) * EEO[j][3] + (21) * EEO[j][4] + (4) * EEO[j][5] + (-29) * EEO[j][6] + (43) * EEO[j][7] + add) >> shift));
		tmp[15] = ((((4) * EEO[j][0] + (-13) * EEO[j][1] + (21) * EEO[j][2] + (-29) * EEO[j][3] + (35) * EEO[j][4] + (-40) * EEO[j][5] + (43) * EEO[j][6] + (-45) * EEO[j][7] + add) >> shift));

		tmp[16] = ((((45) * EO[j][0] + (45) * EO[j][1] + (44) * EO[j][2] + (43) * EO[j][3] + (41) * EO[j][4] + (39) * EO[j][5] + (36) * EO[j][6] + (34) * EO[j][7] + (30) * EO[j][8] + (27) * EO[j][9] + (23) * EO[j][10] + (19) * EO[j][11] + (15) * EO[j][12] + (11) * EO[j][13] + (7) * EO[j][14] + (2) * EO[j][15] + add) >> shift));
		tmp[17] = ((((45) * EO[j][0] + (41) * EO[j][1] + (34) * EO[j][2] + (23) * EO[j][3] + (11) * EO[j][4] + (-2) * EO[j][5] + (-15) * EO[j][6] + (-27) * EO[j][7] + (-36) * EO[j][8] + (-43) * EO[j][9] + (-45) * EO[j][10] + (-44) * EO[j][11] + (-39) * EO[j][12] + (-30) * EO[j][13] + (-19) * EO[j][14] + (-7) * EO[j][15] + add) >> shift));
		tmp[18] = ((((44) * EO[j][0] + (34) * EO[j][1] + (15) * EO[j][2] + (-7) * EO[j][3] + (-27) * EO[j][4] + (-41) * EO[j][5] + (-45) * EO[j][6] + (-39) * EO[j][7] + (-23) * EO[j][8] + (-2) * EO[j][9] + (19) * EO[j][10] + (36) * EO[j][11] + (45) * EO[j][12] + (43) * EO[j][13] + (30) * EO[j][14] + (11) * EO[j][15] + add) >> shift));
		tmp[19] = ((((43) * EO[j][0] + (23) * EO[j][1] + (-7) * EO[j][2] + (-34) * EO[j][3] + (-45) * EO[j][4] + (-36) * EO[j][5] + (-11) * EO[j][6] + (19) * EO[j][7] + (41) * EO[j][8] + (44) * EO[j][9] + (27) * EO[j][10] + (-2) * EO[j][11] + (-30) * EO[j][12] + (-45) * EO[j][13] + (-39) * EO[j][14] + (-15) * EO[j][15] + add) >> shift));
		tmp[20] = ((((41) * EO[j][0] + (11) * EO[j][1] + (-27) * EO[j][2] + (-45) * EO[j][3] + (-30) * EO[j][4] + (7) * EO[j][5] + (39) * EO[j][6] + (43) * EO[j][7] + (15) * EO[j][8] + (-23) * EO[j][9] + (-45) * EO[j][10] + (-34) * EO[j][11] + (2) * EO[j][12] + (36) * EO[j][13] + (44) * EO[j][14] + (19) * EO[j][15] + add) >> shift));
		tmp[21] = ((((39) * EO[j][0] + (-2) * EO[j][1] + (-41) * EO[j][2] + (-36) * EO[j][3] + (7) * EO[j][4] + (43) * EO[j][5] + (34) * EO[j][6] + (-11) * EO[j][7] + (-44) * EO[j][8] + (-30) * EO[j][9] + (15) * EO[j][10] + (45) * EO[j][11] + (27) * EO[j][12] + (-19) * EO[j][13] + (-45) * EO[j][14] + (-23) * EO[j][15] + add) >> shift));
		tmp[22] = ((((36) * EO[j][0] + (-15) * EO[j][1] + (-45) * EO[j][2] + (-11) * EO[j][3] + (39) * EO[j][4] + (34) * EO[j][5] + (-19) * EO[j][6] + (-45) * EO[j][7] + (-7) * EO[j][8] + (41) * EO[j][9] + (30) * EO[j][10] + (-23) * EO[j][11] + (-44) * EO[j][12] + (-2) * EO[j][13] + (43) * EO[j][14] + (27) * EO[j][15] + add) >> shift));
		tmp[23] = ((((34) * EO[j][0] + (-27) * EO[j][1] + (-39) * EO[j][2] + (19) * EO[j][3] + (43) * EO[j][4] + (-11) * EO[j][5] + (-45) * EO[j][6] + (2) * EO[j][7] + (45) * EO[j][8] + (7) * EO[j][9] + (-44) * EO[j][10] + (-15) * EO[j][11] + (41) * EO[j][12] + (23) * EO[j][13] + (-36) * EO[j][14] + (-30) * EO[j][15] + add) >> shift));
		tmp[24] = ((((30) * EO[j][0] + (-36) * EO[j][1] + (-23) * EO[j][2] + (41) * EO[j][3] + (15) * EO[j][4] + (-44) * EO[j][5] + (-7) * EO[j][6] + (45) * EO[j][7] + (-2) * EO[j][8] + (-45) * EO[j][9] + (11) * EO[j][10] + (43) * EO[j][11] + (-19) * EO[j][12] + (-39) * EO[j][13] + (27) * EO[j][14] + (34) * EO[j][15] + add) >> shift));
		tmp[25] = ((((27) * EO[j][0] + (-43) * EO[j][1] + (-2) * EO[j][2] + (44) * EO[j][3] + (-23) * EO[j][4] + (-30) * EO[j][5] + (41) * EO[j][6] + (7) * EO[j][7] + (-45) * EO[j][8] + (19) * EO[j][9] + (34) * EO[j][10] + (-39) * EO[j][11] + (-11) * EO[j][12] + (45) * EO[j][13] + (-15) * EO[j][14] + (-36) * EO[j][15] + add) >> shift));
		tmp[26] = ((((23) * EO[j][0] + (-45) * EO[j][1] + (19) * EO[j][2] + (27) * EO[j][3] + (-45) * EO[j][4] + (15) * EO[j][5] + (30) * EO[j][6] + (-44) * EO[j][7] + (11) * EO[j][8] + (34) * EO[j][9] + (-43) * EO[j][10] + (7) * EO[j][11] + (36) * EO[j][12] + (-41) * EO[j][13] + (2) * EO[j][14] + (39) * EO[j][15] + add) >> shift));
		tmp[27] = ((((19) * EO[j][0] + (-44) * EO[j][1] + (36) * EO[j][2] + (-2) * EO[j][3] + (-34) * EO[j][4] + (45) * EO[j][5] + (-23) * EO[j][6] + (-15) * EO[j][7] + (43) * EO[j][8] + (-39) * EO[j][9] + (7) * EO[j][10] + (30) * EO[j][11] + (-45) * EO[j][12] + (27) * EO[j][13] + (11) * EO[j][14] + (-41) * EO[j][15] + add) >> shift));
		tmp[28] = ((((15) * EO[j][0] + (-39) * EO[j][1] + (45) * EO[j][2] + (-30) * EO[j][3] + (2) * EO[j][4] + (27) * EO[j][5] + (-44) * EO[j][6] + (41) * EO[j][7] + (-19) * EO[j][8] + (-11) * EO[j][9] + (36) * EO[j][10] + (-45) * EO[j][11] + (34) * EO[j][12] + (-7) * EO[j][13] + (-23) * EO[j][14] + (43) * EO[j][15] + add) >> shift));
		tmp[29] = ((((11) * EO[j][0] + (-30) * EO[j][1] + (43) * EO[j][2] + (-45) * EO[j][3] + (36) * EO[j][4] + (-19) * EO[j][5] + (-2) * EO[j][6] + (23) * EO[j][7] + (-39) * EO[j][8] + (45) * EO[j][9] + (-41) * EO[j][10] + (27) * EO[j][11] + (-7) * EO[j][12] + (-15) * EO[j][13] + (34) * EO[j][14] + (-44) * EO[j][15] + add) >> shift));
		tmp[30] = ((((7) * EO[j][0] + (-19) * EO[j][1] + (30) * EO[j][2] + (-39) * EO[j][3] + (44) * EO[j][4] + (-45) * EO[j][5] + (43) * EO[j][6] + (-36) * EO[j][7] + (27) * EO[j][8] + (-15) * EO[j][9] + (2) * EO[j][10] + (11) * EO[j][11] + (-23) * EO[j][12] + (34) * EO[j][13] + (-41) * EO[j][14] + (45) * EO[j][15] + add) >> shift));
		tmp[31] = ((((2) * EO[j][0] + (-7) * EO[j][1] + (11) * EO[j][2] + (-15) * EO[j][3] + (19) * EO[j][4] + (-23) * EO[j][5] + (27) * EO[j][6] + (-30) * EO[j][7] + (34) * EO[j][8] + (-36) * EO[j][9] + (39) * EO[j][10] + (-41) * EO[j][11] + (43) * EO[j][12] + (-44) * EO[j][13] + (45) * EO[j][14] + (-45) * EO[j][15] + add) >> shift));

		tmp[32] = ((((45) * O[j][0] + (45) * O[j][1] + (45) * O[j][2] + (45) * O[j][3] + (44) * O[j][4] + (44) * O[j][5] + (43) * O[j][6] + (42) * O[j][7] + (41) * O[j][8] + (40) * O[j][9] + (39) * O[j][10] + (38) * O[j][11] + (37) * O[j][12] + (36) * O[j][13] + (34) * O[j][14] + (33) * O[j][15] + (31) * O[j][16] + (30) * O[j][17] + (28) * O[j][18] + (26) * O[j][19] + (24) * O[j][20] + (22) * O[j][21] + (20) * O[j][22] + (18) * O[j][23] + (16) * O[j][24] + (14) * O[j][25] + (12) * O[j][26] + (10) * O[j][27] + (8) * O[j][28] + (6) * O[j][29] + (3) * O[j][30] + (1) * O[j][31] + add) >> shift));
		tmp[33] = ((((45) * O[j][0] + (44) * O[j][1] + (42) * O[j][2] + (39) * O[j][3] + (36) * O[j][4] + (31) * O[j][5] + (26) * O[j][6] + (20) * O[j][7] + (14) * O[j][8] + (8) * O[j][9] + (1) * O[j][10] + (-6) * O[j][11] + (-12) * O[j][12] + (-18) * O[j][13] + (-24) * O[j][14] + (-30) * O[j][15] + (-34) * O[j][16] + (-38) * O[j][17] + (-41) * O[j][18] + (-44) * O[j][19] + (-45) * O[j][20] + (-45) * O[j][21] + (-45) * O[j][22] + (-43) * O[j][23] + (-40) * O[j][24] + (-37) * O[j][25] + (-33) * O[j][26] + (-28) * O[j][27] + (-22) * O[j][28] + (-16) * O[j][29] + (-10) * O[j][30] + (-3) * O[j][31] + add) >> shift));
		tmp[34] = ((((45) * O[j][0] + (42) * O[j][1] + (37) * O[j][2] + (30) * O[j][3] + (20) * O[j][4] + (10) * O[j][5] + (-1) * O[j][6] + (-12) * O[j][7] + (-22) * O[j][8] + (-31) * O[j][9] + (-38) * O[j][10] + (-43) * O[j][11] + (-45) * O[j][12] + (-45) * O[j][13] + (-41) * O[j][14] + (-36) * O[j][15] + (-28) * O[j][16] + (-18) * O[j][17] + (-8) * O[j][18] + (3) * O[j][19] + (14) * O[j][20] + (24) * O[j][21] + (33) * O[j][22] + (39) * O[j][23] + (44) * O[j][24] + (45) * O[j][25] + (44) * O[j][26] + (40) * O[j][27] + (34) * O[j][28] + (26) * O[j][29] + (16) * O[j][30] + (6) * O[j][31] + add) >> shift));
		tmp[35] = ((((45) * O[j][0] + (39) * O[j][1] + (30) * O[j][2] + (16) * O[j][3] + (1) * O[j][4] + (-14) * O[j][5] + (-28) * O[j][6] + (-38) * O[j][7] + (-44) * O[j][8] + (-45) * O[j][9] + (-40) * O[j][10] + (-31) * O[j][11] + (-18) * O[j][12] + (-3) * O[j][13] + (12) * O[j][14] + (26) * O[j][15] + (37) * O[j][16] + (44) * O[j][17] + (45) * O[j][18] + (41) * O[j][19] + (33) * O[j][20] + (20) * O[j][21] + (6) * O[j][22] + (-10) * O[j][23] + (-24) * O[j][24] + (-36) * O[j][25] + (-43) * O[j][26] + (-45) * O[j][27] + (-42) * O[j][28] + (-34) * O[j][29] + (-22) * O[j][30] + (-8) * O[j][31] + add) >> shift));
		tmp[36] = ((((44) * O[j][0] + (36) * O[j][1] + (20) * O[j][2] + (1) * O[j][3] + (-18) * O[j][4] + (-34) * O[j][5] + (-44) * O[j][6] + (-45) * O[j][7] + (-37) * O[j][8] + (-22) * O[j][9] + (-3) * O[j][10] + (16) * O[j][11] + (33) * O[j][12] + (43) * O[j][13] + (45) * O[j][14] + (38) * O[j][15] + (24) * O[j][16] + (6) * O[j][17] + (-14) * O[j][18] + (-31) * O[j][19] + (-42) * O[j][20] + (-45) * O[j][21] + (-39) * O[j][22] + (-26) * O[j][23] + (-8) * O[j][24] + (12) * O[j][25] + (30) * O[j][26] + (41) * O[j][27] + (45) * O[j][28] + (40) * O[j][29] + (28) * O[j][30] + (10) * O[j][31] + add) >> shift));
		tmp[37] = ((((44) * O[j][0] + (31) * O[j][1] + (10) * O[j][2] + (-14) * O[j][3] + (-34) * O[j][4] + (-45) * O[j][5] + (-42) * O[j][6] + (-28) * O[j][7] + (-6) * O[j][8] + (18) * O[j][9] + (37) * O[j][10] + (45) * O[j][11] + (40) * O[j][12] + (24) * O[j][13] + (1) * O[j][14] + (-22) * O[j][15] + (-39) * O[j][16] + (-45) * O[j][17] + (-38) * O[j][18] + (-20) * O[j][19] + (3) * O[j][20] + (26) * O[j][21] + (41) * O[j][22] + (45) * O[j][23] + (36) * O[j][24] + (16) * O[j][25] + (-8) * O[j][26] + (-30) * O[j][27] + (-43) * O[j][28] + (-44) * O[j][29] + (-33) * O[j][30] + (-12) * O[j][31] + add) >> shift));
		tmp[38] = ((((43) * O[j][0] + (26) * O[j][1] + (-1) * O[j][2] + (-28) * O[j][3] + (-44) * O[j][4] + (-42) * O[j][5] + (-24) * O[j][6] + (3) * O[j][7] + (30) * O[j][8] + (44) * O[j][9] + (41) * O[j][10] + (22) * O[j][11] + (-6) * O[j][12] + (-31) * O[j][13] + (-45) * O[j][14] + (-40) * O[j][15] + (-20) * O[j][16] + (8) * O[j][17] + (33) * O[j][18] + (45) * O[j][19] + (39) * O[j][20] + (18) * O[j][21] + (-10) * O[j][22] + (-34) * O[j][23] + (-45) * O[j][24] + (-38) * O[j][25] + (-16) * O[j][26] + (12) * O[j][27] + (36) * O[j][28] + (45) * O[j][29] + (37) * O[j][30] + (14) * O[j][31] + add) >> shift));
		tmp[39] = ((((42) * O[j][0] + (20) * O[j][1] + (-12) * O[j][2] + (-38) * O[j][3] + (-45) * O[j][4] + (-28) * O[j][5] + (3) * O[j][6] + (33) * O[j][7] + (45) * O[j][8] + (34) * O[j][9] + (6) * O[j][10] + (-26) * O[j][11] + (-44) * O[j][12] + (-39) * O[j][13] + (-14) * O[j][14] + (18) * O[j][15] + (41) * O[j][16] + (43) * O[j][17] + (22) * O[j][18] + (-10) * O[j][19] + (-37) * O[j][20] + (-45) * O[j][21] + (-30) * O[j][22] + (1) * O[j][23] + (31) * O[j][24] + (45) * O[j][25] + (36) * O[j][26] + (8) * O[j][27] + (-24) * O[j][28] + (-44) * O[j][29] + (-40) * O[j][30] + (-16) * O[j][31] + add) >> shift));
		tmp[40] = ((((41) * O[j][0] + (14) * O[j][1] + (-22) * O[j][2] + (-44) * O[j][3] + (-37) * O[j][4] + (-6) * O[j][5] + (30) * O[j][6] + (45) * O[j][7] + (31) * O[j][8] + (-3) * O[j][9] + (-36) * O[j][10] + (-45) * O[j][11] + (-24) * O[j][12] + (12) * O[j][13] + (40) * O[j][14] + (42) * O[j][15] + (16) * O[j][16] + (-20) * O[j][17] + (-44) * O[j][18] + (-38) * O[j][19] + (-8) * O[j][20] + (28) * O[j][21] + (45) * O[j][22] + (33) * O[j][23] + (-1) * O[j][24] + (-34) * O[j][25] + (-45) * O[j][26] + (-26) * O[j][27] + (10) * O[j][28] + (39) * O[j][29] + (43) * O[j][30] + (18) * O[j][31] + add) >> shift));
		tmp[41] = ((((40) * O[j][0] + (8) * O[j][1] + (-31) * O[j][2] + (-45) * O[j][3] + (-22) * O[j][4] + (18) * O[j][5] + (44) * O[j][6] + (34) * O[j][7] + (-3) * O[j][8] + (-38) * O[j][9] + (-42) * O[j][10] + (-12) * O[j][11] + (28) * O[j][12] + (45) * O[j][13] + (26) * O[j][14] + (-14) * O[j][15] + (-43) * O[j][16] + (-37) * O[j][17] + (-1) * O[j][18] + (36) * O[j][19] + (44) * O[j][20] + (16) * O[j][21] + (-24) * O[j][22] + (-45) * O[j][23] + (-30) * O[j][24] + (10) * O[j][25] + (41) * O[j][26] + (39) * O[j][27] + (6) * O[j][28] + (-33) * O[j][29] + (-45) * O[j][30] + (-20) * O[j][31] + add) >> shift));
		tmp[42] = ((((39) * O[j][0] + (1) * O[j][1] + (-38) * O[j][2] + (-40) * O[j][3] + (-3) * O[j][4] + (37) * O[j][5] + (41) * O[j][6] + (6) * O[j][7] + (-36) * O[j][8] + (-42) * O[j][9] + (-8) * O[j][10] + (34) * O[j][11] + (43) * O[j][12] + (10) * O[j][13] + (-33) * O[j][14] + (-44) * O[j][15] + (-12) * O[j][16] + (31) * O[j][17] + (44) * O[j][18] + (14) * O[j][19] + (-30) * O[j][20] + (-45) * O[j][21] + (-16) * O[j][22] + (28) * O[j][23] + (45) * O[j][24] + (18) * O[j][25] + (-26) * O[j][26] + (-45) * O[j][27] + (-20) * O[j][28] + (24) * O[j][29] + (45) * O[j][30] + (22) * O[j][31] + add) >> shift));
		tmp[43] = ((((38) * O[j][0] + (-6) * O[j][1] + (-43) * O[j][2] + (-31) * O[j][3] + (16) * O[j][4] + (45) * O[j][5] + (22) * O[j][6] + (-26) * O[j][7] + (-45) * O[j][8] + (-12) * O[j][9] + (34) * O[j][10] + (41) * O[j][11] + (1) * O[j][12] + (-40) * O[j][13] + (-36) * O[j][14] + (10) * O[j][15] + (44) * O[j][16] + (28) * O[j][17] + (-20) * O[j][18] + (-45) * O[j][19] + (-18) * O[j][20] + (30) * O[j][21] + (44) * O[j][22] + (8) * O[j][23] + (-37) * O[j][24] + (-39) * O[j][25] + (3) * O[j][26] + (42) * O[j][27] + (33) * O[j][28] + (-14) * O[j][29] + (-45) * O[j][30] + (-24) * O[j][31] + add) >> shift));
		tmp[44] = ((((37) * O[j][0] + (-12) * O[j][1] + (-45) * O[j][2] + (-18) * O[j][3] + (33) * O[j][4] + (40) * O[j][5] + (-6) * O[j][6] + (-44) * O[j][7] + (-24) * O[j][8] + (28) * O[j][9] + (43) * O[j][10] + (1) * O[j][11] + (-42) * O[j][12] + (-30) * O[j][13] + (22) * O[j][14] + (45) * O[j][15] + (8) * O[j][16] + (-39) * O[j][17] + (-34) * O[j][18] + (16) * O[j][19] + (45) * O[j][20] + (14) * O[j][21] + (-36) * O[j][22] + (-38) * O[j][23] + (10) * O[j][24] + (45) * O[j][25] + (20) * O[j][26] + (-31) * O[j][27] + (-41) * O[j][28] + (3) * O[j][29] + (44) * O[j][30] + (26) * O[j][31] + add) >> shift));
		tmp[45] = ((((36) * O[j][0] + (-18) * O[j][1] + (-45) * O[j][2] + (-3) * O[j][3] + (43) * O[j][4] + (24) * O[j][5] + (-31) * O[j][6] + (-39) * O[j][7] + (12) * O[j][8] + (45) * O[j][9] + (10) * O[j][10] + (-40) * O[j][11] + (-30) * O[j][12] + (26) * O[j][13] + (42) * O[j][14] + (-6) * O[j][15] + (-45) * O[j][16] + (-16) * O[j][17] + (37) * O[j][18] + (34) * O[j][19] + (-20) * O[j][20] + (-44) * O[j][21] + (-1) * O[j][22] + (44) * O[j][23] + (22) * O[j][24] + (-33) * O[j][25] + (-38) * O[j][26] + (14) * O[j][27] + (45) * O[j][28] + (8) * O[j][29] + (-41) * O[j][30] + (-28) * O[j][31] + add) >> shift));
		tmp[46] = ((((34) * O[j][0] + (-24) * O[j][1] + (-41) * O[j][2] + (12) * O[j][3] + (45) * O[j][4] + (1) * O[j][5] + (-45) * O[j][6] + (-14) * O[j][7] + (40) * O[j][8] + (26) * O[j][9] + (-33) * O[j][10] + (-36) * O[j][11] + (22) * O[j][12] + (42) * O[j][13] + (-10) * O[j][14] + (-45) * O[j][15] + (-3) * O[j][16] + (44) * O[j][17] + (16) * O[j][18] + (-39) * O[j][19] + (-28) * O[j][20] + (31) * O[j][21] + (37) * O[j][22] + (-20) * O[j][23] + (-43) * O[j][24] + (8) * O[j][25] + (45) * O[j][26] + (6) * O[j][27] + (-44) * O[j][28] + (-18) * O[j][29] + (38) * O[j][30] + (30) * O[j][31] + add) >> shift));
		tmp[47] = ((((33) * O[j][0] + (-30) * O[j][1] + (-36) * O[j][2] + (26) * O[j][3] + (38) * O[j][4] + (-22) * O[j][5] + (-40) * O[j][6] + (18) * O[j][7] + (42) * O[j][8] + (-14) * O[j][9] + (-44) * O[j][10] + (10) * O[j][11] + (45) * O[j][12] + (-6) * O[j][13] + (-45) * O[j][14] + (1) * O[j][15] + (45) * O[j][16] + (3) * O[j][17] + (-45) * O[j][18] + (-8) * O[j][19] + (44) * O[j][20] + (12) * O[j][21] + (-43) * O[j][22] + (-16) * O[j][23] + (41) * O[j][24] + (20) * O[j][25] + (-39) * O[j][26] + (-24) * O[j][27] + (37) * O[j][28] + (28) * O[j][29] + (-34) * O[j][30] + (-31) * O[j][31] + add) >> shift));
		tmp[48] = ((((31) * O[j][0] + (-34) * O[j][1] + (-28) * O[j][2] + (37) * O[j][3] + (24) * O[j][4] + (-39) * O[j][5] + (-20) * O[j][6] + (41) * O[j][7] + (16) * O[j][8] + (-43) * O[j][9] + (-12) * O[j][10] + (44) * O[j][11] + (8) * O[j][12] + (-45) * O[j][13] + (-3) * O[j][14] + (45) * O[j][15] + (-1) * O[j][16] + (-45) * O[j][17] + (6) * O[j][18] + (45) * O[j][19] + (-10) * O[j][20] + (-44) * O[j][21] + (14) * O[j][22] + (42) * O[j][23] + (-18) * O[j][24] + (-40) * O[j][25] + (22) * O[j][26] + (38) * O[j][27] + (-26) * O[j][28] + (-36) * O[j][29] + (30) * O[j][30] + (33) * O[j][31] + add) >> shift));
		tmp[49] = ((((30) * O[j][0] + (-38) * O[j][1] + (-18) * O[j][2] + (44) * O[j][3] + (6) * O[j][4] + (-45) * O[j][5] + (8) * O[j][6] + (43) * O[j][7] + (-20) * O[j][8] + (-37) * O[j][9] + (31) * O[j][10] + (28) * O[j][11] + (-39) * O[j][12] + (-16) * O[j][13] + (44) * O[j][14] + (3) * O[j][15] + (-45) * O[j][16] + (10) * O[j][17] + (42) * O[j][18] + (-22) * O[j][19] + (-36) * O[j][20] + (33) * O[j][21] + (26) * O[j][22] + (-40) * O[j][23] + (-14) * O[j][24] + (45) * O[j][25] + (1) * O[j][26] + (-45) * O[j][27] + (12) * O[j][28] + (41) * O[j][29] + (-24) * O[j][30] + (-34) * O[j][31] + add) >> shift));
		tmp[50] = ((((28) * O[j][0] + (-41) * O[j][1] + (-8) * O[j][2] + (45) * O[j][3] + (-14) * O[j][4] + (-38) * O[j][5] + (33) * O[j][6] + (22) * O[j][7] + (-44) * O[j][8] + (-1) * O[j][9] + (44) * O[j][10] + (-20) * O[j][11] + (-34) * O[j][12] + (37) * O[j][13] + (16) * O[j][14] + (-45) * O[j][15] + (6) * O[j][16] + (42) * O[j][17] + (-26) * O[j][18] + (-30) * O[j][19] + (40) * O[j][20] + (10) * O[j][21] + (-45) * O[j][22] + (12) * O[j][23] + (39) * O[j][24] + (-31) * O[j][25] + (-24) * O[j][26] + (43) * O[j][27] + (3) * O[j][28] + (-45) * O[j][29] + (18) * O[j][30] + (36) * O[j][31] + add) >> shift));
		tmp[51] = ((((26) * O[j][0] + (-44) * O[j][1] + (3) * O[j][2] + (41) * O[j][3] + (-31) * O[j][4] + (-20) * O[j][5] + (45) * O[j][6] + (-10) * O[j][7] + (-38) * O[j][8] + (36) * O[j][9] + (14) * O[j][10] + (-45) * O[j][11] + (16) * O[j][12] + (34) * O[j][13] + (-39) * O[j][14] + (-8) * O[j][15] + (45) * O[j][16] + (-22) * O[j][17] + (-30) * O[j][18] + (42) * O[j][19] + (1) * O[j][20] + (-43) * O[j][21] + (28) * O[j][22] + (24) * O[j][23] + (-44) * O[j][24] + (6) * O[j][25] + (40) * O[j][26] + (-33) * O[j][27] + (-18) * O[j][28] + (45) * O[j][29] + (-12) * O[j][30] + (-37) * O[j][31] + add) >> shift));
		tmp[52] = ((((24) * O[j][0] + (-45) * O[j][1] + (14) * O[j][2] + (33) * O[j][3] + (-42) * O[j][4] + (3) * O[j][5] + (39) * O[j][6] + (-37) * O[j][7] + (-8) * O[j][8] + (44) * O[j][9] + (-30) * O[j][10] + (-18) * O[j][11] + (45) * O[j][12] + (-20) * O[j][13] + (-28) * O[j][14] + (44) * O[j][15] + (-10) * O[j][16] + (-36) * O[j][17] + (40) * O[j][18] + (1) * O[j][19] + (-41) * O[j][20] + (34) * O[j][21] + (12) * O[j][22] + (-45) * O[j][23] + (26) * O[j][24] + (22) * O[j][25] + (-45) * O[j][26] + (16) * O[j][27] + (31) * O[j][28] + (-43) * O[j][29] + (6) * O[j][30] + (38) * O[j][31] + add) >> shift));
		tmp[53] = ((((22) * O[j][0] + (-45) * O[j][1] + (24) * O[j][2] + (20) * O[j][3] + (-45) * O[j][4] + (26) * O[j][5] + (18) * O[j][6] + (-45) * O[j][7] + (28) * O[j][8] + (16) * O[j][9] + (-45) * O[j][10] + (30) * O[j][11] + (14) * O[j][12] + (-44) * O[j][13] + (31) * O[j][14] + (12) * O[j][15] + (-44) * O[j][16] + (33) * O[j][17] + (10) * O[j][18] + (-43) * O[j][19] + (34) * O[j][20] + (8) * O[j][21] + (-42) * O[j][22] + (36) * O[j][23] + (6) * O[j][24] + (-41) * O[j][25] + (37) * O[j][26] + (3) * O[j][27] + (-40) * O[j][28] + (38) * O[j][29] + (1) * O[j][30] + (-39) * O[j][31] + add) >> shift));
		tmp[54] = ((((20) * O[j][0] + (-45) * O[j][1] + (33) * O[j][2] + (6) * O[j][3] + (-39) * O[j][4] + (41) * O[j][5] + (-10) * O[j][6] + (-30) * O[j][7] + (45) * O[j][8] + (-24) * O[j][9] + (-16) * O[j][10] + (44) * O[j][11] + (-36) * O[j][12] + (-1) * O[j][13] + (37) * O[j][14] + (-43) * O[j][15] + (14) * O[j][16] + (26) * O[j][17] + (-45) * O[j][18] + (28) * O[j][19] + (12) * O[j][20] + (-42) * O[j][21] + (38) * O[j][22] + (-3) * O[j][23] + (-34) * O[j][24] + (44) * O[j][25] + (-18) * O[j][26] + (-22) * O[j][27] + (45) * O[j][28] + (-31) * O[j][29] + (-8) * O[j][30] + (40) * O[j][31] + add) >> shift));
		tmp[55] = ((((18) * O[j][0] + (-43) * O[j][1] + (39) * O[j][2] + (-10) * O[j][3] + (-26) * O[j][4] + (45) * O[j][5] + (-34) * O[j][6] + (1) * O[j][7] + (33) * O[j][8] + (-45) * O[j][9] + (28) * O[j][10] + (8) * O[j][11] + (-38) * O[j][12] + (44) * O[j][13] + (-20) * O[j][14] + (-16) * O[j][15] + (42) * O[j][16] + (-40) * O[j][17] + (12) * O[j][18] + (24) * O[j][19] + (-45) * O[j][20] + (36) * O[j][21] + (-3) * O[j][22] + (-31) * O[j][23] + (45) * O[j][24] + (-30) * O[j][25] + (-6) * O[j][26] + (37) * O[j][27] + (-44) * O[j][28] + (22) * O[j][29] + (14) * O[j][30] + (-41) * O[j][31] + add) >> shift));
		tmp[56] = ((((16) * O[j][0] + (-40) * O[j][1] + (44) * O[j][2] + (-24) * O[j][3] + (-8) * O[j][4] + (36) * O[j][5] + (-45) * O[j][6] + (31) * O[j][7] + (-1) * O[j][8] + (-30) * O[j][9] + (45) * O[j][10] + (-37) * O[j][11] + (10) * O[j][12] + (22) * O[j][13] + (-43) * O[j][14] + (41) * O[j][15] + (-18) * O[j][16] + (-14) * O[j][17] + (39) * O[j][18] + (-44) * O[j][19] + (26) * O[j][20] + (6) * O[j][21] + (-34) * O[j][22] + (45) * O[j][23] + (-33) * O[j][24] + (3) * O[j][25] + (28) * O[j][26] + (-45) * O[j][27] + (38) * O[j][28] + (-12) * O[j][29] + (-20) * O[j][30] + (42) * O[j][31] + add) >> shift));
		tmp[57] = ((((14) * O[j][0] + (-37) * O[j][1] + (45) * O[j][2] + (-36) * O[j][3] + (12) * O[j][4] + (16) * O[j][5] + (-38) * O[j][6] + (45) * O[j][7] + (-34) * O[j][8] + (10) * O[j][9] + (18) * O[j][10] + (-39) * O[j][11] + (45) * O[j][12] + (-33) * O[j][13] + (8) * O[j][14] + (20) * O[j][15] + (-40) * O[j][16] + (45) * O[j][17] + (-31) * O[j][18] + (6) * O[j][19] + (22) * O[j][20] + (-41) * O[j][21] + (44) * O[j][22] + (-30) * O[j][23] + (3) * O[j][24] + (24) * O[j][25] + (-42) * O[j][26] + (44) * O[j][27] + (-28) * O[j][28] + (1) * O[j][29] + (26) * O[j][30] + (-43) * O[j][31] + add) >> shift));
		tmp[58] = ((((12) * O[j][0] + (-33) * O[j][1] + (44) * O[j][2] + (-43) * O[j][3] + (30) * O[j][4] + (-8) * O[j][5] + (-16) * O[j][6] + (36) * O[j][7] + (-45) * O[j][8] + (41) * O[j][9] + (-26) * O[j][10] + (3) * O[j][11] + (20) * O[j][12] + (-38) * O[j][13] + (45) * O[j][14] + (-39) * O[j][15] + (22) * O[j][16] + (1) * O[j][17] + (-24) * O[j][18] + (40) * O[j][19] + (-45) * O[j][20] + (37) * O[j][21] + (-18) * O[j][22] + (-6) * O[j][23] + (28) * O[j][24] + (-42) * O[j][25] + (45) * O[j][26] + (-34) * O[j][27] + (14) * O[j][28] + (10) * O[j][29] + (-31) * O[j][30] + (44) * O[j][31] + add) >> shift));
		tmp[59] = ((((10) * O[j][0] + (-28) * O[j][1] + (40) * O[j][2] + (-45) * O[j][3] + (41) * O[j][4] + (-30) * O[j][5] + (12) * O[j][6] + (8) * O[j][7] + (-26) * O[j][8] + (39) * O[j][9] + (-45) * O[j][10] + (42) * O[j][11] + (-31) * O[j][12] + (14) * O[j][13] + (6) * O[j][14] + (-24) * O[j][15] + (38) * O[j][16] + (-45) * O[j][17] + (43) * O[j][18] + (-33) * O[j][19] + (16) * O[j][20] + (3) * O[j][21] + (-22) * O[j][22] + (37) * O[j][23] + (-45) * O[j][24] + (44) * O[j][25] + (-34) * O[j][26] + (18) * O[j][27] + (1) * O[j][28] + (-20) * O[j][29] + (36) * O[j][30] + (-44) * O[j][31] + add) >> shift));
		tmp[60] = ((((8) * O[j][0] + (-22) * O[j][1] + (34) * O[j][2] + (-42) * O[j][3] + (45) * O[j][4] + (-43) * O[j][5] + (36) * O[j][6] + (-24) * O[j][7] + (10) * O[j][8] + (6) * O[j][9] + (-20) * O[j][10] + (33) * O[j][11] + (-41) * O[j][12] + (45) * O[j][13] + (-44) * O[j][14] + (37) * O[j][15] + (-26) * O[j][16] + (12) * O[j][17] + (3) * O[j][18] + (-18) * O[j][19] + (31) * O[j][20] + (-40) * O[j][21] + (45) * O[j][22] + (-44) * O[j][23] + (38) * O[j][24] + (-28) * O[j][25] + (14) * O[j][26] + (1) * O[j][27] + (-16) * O[j][28] + (30) * O[j][29] + (-39) * O[j][30] + (45) * O[j][31] + add) >> shift));
		tmp[61] = ((((6) * O[j][0] + (-16) * O[j][1] + (26) * O[j][2] + (-34) * O[j][3] + (40) * O[j][4] + (-44) * O[j][5] + (45) * O[j][6] + (-44) * O[j][7] + (39) * O[j][8] + (-33) * O[j][9] + (24) * O[j][10] + (-14) * O[j][11] + (3) * O[j][12] + (8) * O[j][13] + (-18) * O[j][14] + (28) * O[j][15] + (-36) * O[j][16] + (41) * O[j][17] + (-45) * O[j][18] + (45) * O[j][19] + (-43) * O[j][20] + (38) * O[j][21] + (-31) * O[j][22] + (22) * O[j][23] + (-12) * O[j][24] + (1) * O[j][25] + (10) * O[j][26] + (-20) * O[j][27] + (30) * O[j][28] + (-37) * O[j][29] + (42) * O[j][30] + (-45) * O[j][31] + add) >> shift));
		tmp[62] = ((((3) * O[j][0] + (-10) * O[j][1] + (16) * O[j][2] + (-22) * O[j][3] + (28) * O[j][4] + (-33) * O[j][5] + (37) * O[j][6] + (-40) * O[j][7] + (43) * O[j][8] + (-45) * O[j][9] + (45) * O[j][10] + (-45) * O[j][11] + (44) * O[j][12] + (-41) * O[j][13] + (38) * O[j][14] + (-34) * O[j][15] + (30) * O[j][16] + (-24) * O[j][17] + (18) * O[j][18] + (-12) * O[j][19] + (6) * O[j][20] + (1) * O[j][21] + (-8) * O[j][22] + (14) * O[j][23] + (-20) * O[j][24] + (26) * O[j][25] + (-31) * O[j][26] + (36) * O[j][27] + (-39) * O[j][28] + (42) * O[j][29] + (-44) * O[j][30] + (45) * O[j][31] + add) >> shift));
		tmp[63] = ((((1) * O[j][0] + (-3) * O[j][1] + (6) * O[j][2] + (-8) * O[j][3] + (10) * O[j][4] + (-12) * O[j][5] + (14) * O[j][6] + (-16) * O[j][7] + (18) * O[j][8] + (-20) * O[j][9] + (22) * O[j][10] + (-24) * O[j][11] + (26) * O[j][12] + (-28) * O[j][13] + (30) * O[j][14] + (-31) * O[j][15] + (33) * O[j][16] + (-34) * O[j][17] + (36) * O[j][18] + (-37) * O[j][19] + (38) * O[j][20] + (-39) * O[j][21] + (40) * O[j][22] + (-41) * O[j][23] + (42) * O[j][24] + (-43) * O[j][25] + (44) * O[j][26] + (-44) * O[j][27] + (45) * O[j][28] + (-45) * O[j][29] + (45) * O[j][30] + (-45) * O[j][31] + add) >> shift));

		if (w == 8) {
			get_tmp_8(j, dst, tmp);
		}
		else if (w == 16) {
			get_tmp_16(j, dst, tmp);
		}
		else if (w == 32) {
			get_tmp_32(j, dst, tmp);
		}
		else
			get_tmp_64(j, dst, tmp);
	}
}

void dct_col_64(U7 w, U7 h, s16 src[64][64], s16 dst[64][64], U4 shift)
{


	U7 size = 64;
	U8 size_b = size - 1;
	int E[64][32];
	int O[64][32];
	int EE[64][16];
	int EO[64][16];
	int EEE[64][8];
	int EEO[64][8];
	int EEEE[64][4];
	int EEEO[64][4];
	int EEEEE[64][2];
	int EEEEO[64][2];
	int tmp[64];
	int j;
	int k;
	int add = 1 << (shift - 1);
#pragma HLS ARRAY_PARTITION variable=E complete dim=2
#pragma HLS ARRAY_PARTITION variable=O complete dim=2
#pragma HLS ARRAY_PARTITION variable=EE complete dim=2
#pragma HLS ARRAY_PARTITION variable=EO complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEEEE complete dim=2
#pragma HLS ARRAY_PARTITION variable=EEEEO complete dim=2
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src cyclic factor=64 dim=2
#pragma HLS ARRAY_PARTITION variable=dst cyclic factor=16 dim=1

	for (j = 0; j < w; j++) {
#pragma HLS PIPELINE II=4
#pragma HLS loop_tripcount max=64
		// E / O
		for (k = 0; k < 32; k++) {
			E[j][k] = (src[j][(k + j) & size_b]) + (src[j][(63 - k + j) & size_b]);
			O[j][k] = (src[j][(k + j) & size_b]) - (src[j][(63 - k + j) & size_b]);
		}
		// EE / EO
		if (h == 64) {
			for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
				EE[j][k] = E[j][k] + E[j][31 - k];
				EO[j][k] = E[j][k] - E[j][31 - k];
			}
		}
		else if (h == 32) {
			for (k = 0; k < 16; k++)
			{
//#pragma HLS UNROLL
				EE[j][k] = src[j][(k + j) & size_b] + src[j][(31 - k + j) & size_b];
				EO[j][k] = src[j][(k + j) & size_b] - src[j][(31 - k + j) & size_b];
			}
		}
		// EEE / EEO
		if (h == 64 || h == 32) {
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				EEE[j][k] = EE[j][k] + EE[j][15 - k];
				EEO[j][k] = EE[j][k] - EE[j][15 - k];
			}
		}
		else if (h == 16) {
			for (k = 0; k < 8; k++)
			{
//#pragma HLS UNROLL
				EEE[j][k] = src[j][(k + j) & size_b] + src[j][(15 - k + j) & size_b];
				EEO[j][k] = src[j][(k + j) & size_b] - src[j][(15 - k + j) & size_b];
			}
		}
		// EEEE / EEEO
		if (h == 64 || h == 32 || h == 16) {
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				EEEE[j][k] = EEE[j][k] + EEE[j][7 - k];
				EEEO[j][k] = EEE[j][k] - EEE[j][7 - k];
			}
		}
		else if (h == 8) {
			for (k = 0; k < 4; k++)
			{
//#pragma HLS UNROLL
				EEEE[j][k] = src[j][(k + j) & size_b] + src[j][(7 - k + j) & size_b];
				EEEO[j][k] = src[j][(k + j) & size_b] - src[j][(7 - k + j) & size_b];
			}
		}

		EEEEE[j][0] = EEEE[j][0] + EEEE[j][3];
		EEEEO[j][0] = EEEE[j][0] - EEEE[j][3];
		EEEEE[j][1] = EEEE[j][1] + EEEE[j][2];
		EEEEO[j][1] = EEEE[j][1] - EEEE[j][2];

		tmp[0] = (((32 * EEEEE[j][0] + 32 * EEEEE[j][1] + add) >> shift));
		tmp[1] = (((32 * EEEEE[j][0] - 32 * EEEEE[j][1] + add) >> shift));
		tmp[2] = (((42 * EEEEO[j][0] + 17 * EEEEO[j][1] + add) >> shift));
		tmp[3] = (((17 * EEEEO[j][0] - 42 * EEEEO[j][1] + add) >> shift));

		tmp[4] = ((((44) * EEEO[j][0] + (38) * EEEO[j][1] + (25) * EEEO[j][2] + (9) * EEEO[j][3] + add) >> shift));
		tmp[5] = ((((38) * EEEO[j][0] + (-9) * EEEO[j][1] + (-44) * EEEO[j][2] + (-25) * EEEO[j][3] + add) >> shift));
		tmp[6] = ((((25) * EEEO[j][0] + (-44) * EEEO[j][1] + (9) * EEEO[j][2] + (38) * EEEO[j][3] + add) >> shift));
		tmp[7] = ((((9) * EEEO[j][0] + (-25) * EEEO[j][1] + (38) * EEEO[j][2] + (-44) * EEEO[j][3] + add) >> shift));

		tmp[8] = ((((45) * EEO[j][0] + (43) * EEO[j][1] + (40) * EEO[j][2] + (35) * EEO[j][3] + (29) * EEO[j][4] + (21) * EEO[j][5] + (13) * EEO[j][6] + (4) * EEO[j][7] + add) >> shift));
		tmp[9] = ((((43) * EEO[j][0] + (29) * EEO[j][1] + (4) * EEO[j][2] + (-21) * EEO[j][3] + (-40) * EEO[j][4] + (-45) * EEO[j][5] + (-35) * EEO[j][6] + (-13) * EEO[j][7] + add) >> shift));
		tmp[10] = ((((40) * EEO[j][0] + (4) * EEO[j][1] + (-35) * EEO[j][2] + (-43) * EEO[j][3] + (-13) * EEO[j][4] + (29) * EEO[j][5] + (45) * EEO[j][6] + (21) * EEO[j][7] + add) >> shift));
		tmp[11] = ((((35) * EEO[j][0] + (-21) * EEO[j][1] + (-43) * EEO[j][2] + (4) * EEO[j][3] + (45) * EEO[j][4] + (13) * EEO[j][5] + (-40) * EEO[j][6] + (-29) * EEO[j][7] + add) >> shift));
		tmp[12] = ((((29) * EEO[j][0] + (-40) * EEO[j][1] + (-13) * EEO[j][2] + (45) * EEO[j][3] + (-4) * EEO[j][4] + (-43) * EEO[j][5] + (21) * EEO[j][6] + (35) * EEO[j][7] + add) >> shift));
		tmp[13] = ((((21) * EEO[j][0] + (-45) * EEO[j][1] + (29) * EEO[j][2] + (13) * EEO[j][3] + (-43) * EEO[j][4] + (35) * EEO[j][5] + (4) * EEO[j][6] + (-40) * EEO[j][7] + add) >> shift));
		tmp[14] = ((((13) * EEO[j][0] + (-35) * EEO[j][1] + (45) * EEO[j][2] + (-40) * EEO[j][3] + (21) * EEO[j][4] + (4) * EEO[j][5] + (-29) * EEO[j][6] + (43) * EEO[j][7] + add) >> shift));
		tmp[15] = ((((4) * EEO[j][0] + (-13) * EEO[j][1] + (21) * EEO[j][2] + (-29) * EEO[j][3] + (35) * EEO[j][4] + (-40) * EEO[j][5] + (43) * EEO[j][6] + (-45) * EEO[j][7] + add) >> shift));

		tmp[16] = ((((45) * EO[j][0] + (45) * EO[j][1] + (44) * EO[j][2] + (43) * EO[j][3] + (41) * EO[j][4] + (39) * EO[j][5] + (36) * EO[j][6] + (34) * EO[j][7] + (30) * EO[j][8] + (27) * EO[j][9] + (23) * EO[j][10] + (19) * EO[j][11] + (15) * EO[j][12] + (11) * EO[j][13] + (7) * EO[j][14] + (2) * EO[j][15] + add) >> shift));
		tmp[17] = ((((45) * EO[j][0] + (41) * EO[j][1] + (34) * EO[j][2] + (23) * EO[j][3] + (11) * EO[j][4] + (-2) * EO[j][5] + (-15) * EO[j][6] + (-27) * EO[j][7] + (-36) * EO[j][8] + (-43) * EO[j][9] + (-45) * EO[j][10] + (-44) * EO[j][11] + (-39) * EO[j][12] + (-30) * EO[j][13] + (-19) * EO[j][14] + (-7) * EO[j][15] + add) >> shift));
		tmp[18] = ((((44) * EO[j][0] + (34) * EO[j][1] + (15) * EO[j][2] + (-7) * EO[j][3] + (-27) * EO[j][4] + (-41) * EO[j][5] + (-45) * EO[j][6] + (-39) * EO[j][7] + (-23) * EO[j][8] + (-2) * EO[j][9] + (19) * EO[j][10] + (36) * EO[j][11] + (45) * EO[j][12] + (43) * EO[j][13] + (30) * EO[j][14] + (11) * EO[j][15] + add) >> shift));
		tmp[19] = ((((43) * EO[j][0] + (23) * EO[j][1] + (-7) * EO[j][2] + (-34) * EO[j][3] + (-45) * EO[j][4] + (-36) * EO[j][5] + (-11) * EO[j][6] + (19) * EO[j][7] + (41) * EO[j][8] + (44) * EO[j][9] + (27) * EO[j][10] + (-2) * EO[j][11] + (-30) * EO[j][12] + (-45) * EO[j][13] + (-39) * EO[j][14] + (-15) * EO[j][15] + add) >> shift));
		tmp[20] = ((((41) * EO[j][0] + (11) * EO[j][1] + (-27) * EO[j][2] + (-45) * EO[j][3] + (-30) * EO[j][4] + (7) * EO[j][5] + (39) * EO[j][6] + (43) * EO[j][7] + (15) * EO[j][8] + (-23) * EO[j][9] + (-45) * EO[j][10] + (-34) * EO[j][11] + (2) * EO[j][12] + (36) * EO[j][13] + (44) * EO[j][14] + (19) * EO[j][15] + add) >> shift));
		tmp[21] = ((((39) * EO[j][0] + (-2) * EO[j][1] + (-41) * EO[j][2] + (-36) * EO[j][3] + (7) * EO[j][4] + (43) * EO[j][5] + (34) * EO[j][6] + (-11) * EO[j][7] + (-44) * EO[j][8] + (-30) * EO[j][9] + (15) * EO[j][10] + (45) * EO[j][11] + (27) * EO[j][12] + (-19) * EO[j][13] + (-45) * EO[j][14] + (-23) * EO[j][15] + add) >> shift));
		tmp[22] = ((((36) * EO[j][0] + (-15) * EO[j][1] + (-45) * EO[j][2] + (-11) * EO[j][3] + (39) * EO[j][4] + (34) * EO[j][5] + (-19) * EO[j][6] + (-45) * EO[j][7] + (-7) * EO[j][8] + (41) * EO[j][9] + (30) * EO[j][10] + (-23) * EO[j][11] + (-44) * EO[j][12] + (-2) * EO[j][13] + (43) * EO[j][14] + (27) * EO[j][15] + add) >> shift));
		tmp[23] = ((((34) * EO[j][0] + (-27) * EO[j][1] + (-39) * EO[j][2] + (19) * EO[j][3] + (43) * EO[j][4] + (-11) * EO[j][5] + (-45) * EO[j][6] + (2) * EO[j][7] + (45) * EO[j][8] + (7) * EO[j][9] + (-44) * EO[j][10] + (-15) * EO[j][11] + (41) * EO[j][12] + (23) * EO[j][13] + (-36) * EO[j][14] + (-30) * EO[j][15] + add) >> shift));
		tmp[24] = ((((30) * EO[j][0] + (-36) * EO[j][1] + (-23) * EO[j][2] + (41) * EO[j][3] + (15) * EO[j][4] + (-44) * EO[j][5] + (-7) * EO[j][6] + (45) * EO[j][7] + (-2) * EO[j][8] + (-45) * EO[j][9] + (11) * EO[j][10] + (43) * EO[j][11] + (-19) * EO[j][12] + (-39) * EO[j][13] + (27) * EO[j][14] + (34) * EO[j][15] + add) >> shift));
		tmp[25] = ((((27) * EO[j][0] + (-43) * EO[j][1] + (-2) * EO[j][2] + (44) * EO[j][3] + (-23) * EO[j][4] + (-30) * EO[j][5] + (41) * EO[j][6] + (7) * EO[j][7] + (-45) * EO[j][8] + (19) * EO[j][9] + (34) * EO[j][10] + (-39) * EO[j][11] + (-11) * EO[j][12] + (45) * EO[j][13] + (-15) * EO[j][14] + (-36) * EO[j][15] + add) >> shift));
		tmp[26] = ((((23) * EO[j][0] + (-45) * EO[j][1] + (19) * EO[j][2] + (27) * EO[j][3] + (-45) * EO[j][4] + (15) * EO[j][5] + (30) * EO[j][6] + (-44) * EO[j][7] + (11) * EO[j][8] + (34) * EO[j][9] + (-43) * EO[j][10] + (7) * EO[j][11] + (36) * EO[j][12] + (-41) * EO[j][13] + (2) * EO[j][14] + (39) * EO[j][15] + add) >> shift));
		tmp[27] = ((((19) * EO[j][0] + (-44) * EO[j][1] + (36) * EO[j][2] + (-2) * EO[j][3] + (-34) * EO[j][4] + (45) * EO[j][5] + (-23) * EO[j][6] + (-15) * EO[j][7] + (43) * EO[j][8] + (-39) * EO[j][9] + (7) * EO[j][10] + (30) * EO[j][11] + (-45) * EO[j][12] + (27) * EO[j][13] + (11) * EO[j][14] + (-41) * EO[j][15] + add) >> shift));
		tmp[28] = ((((15) * EO[j][0] + (-39) * EO[j][1] + (45) * EO[j][2] + (-30) * EO[j][3] + (2) * EO[j][4] + (27) * EO[j][5] + (-44) * EO[j][6] + (41) * EO[j][7] + (-19) * EO[j][8] + (-11) * EO[j][9] + (36) * EO[j][10] + (-45) * EO[j][11] + (34) * EO[j][12] + (-7) * EO[j][13] + (-23) * EO[j][14] + (43) * EO[j][15] + add) >> shift));
		tmp[29] = ((((11) * EO[j][0] + (-30) * EO[j][1] + (43) * EO[j][2] + (-45) * EO[j][3] + (36) * EO[j][4] + (-19) * EO[j][5] + (-2) * EO[j][6] + (23) * EO[j][7] + (-39) * EO[j][8] + (45) * EO[j][9] + (-41) * EO[j][10] + (27) * EO[j][11] + (-7) * EO[j][12] + (-15) * EO[j][13] + (34) * EO[j][14] + (-44) * EO[j][15] + add) >> shift));
		tmp[30] = ((((7) * EO[j][0] + (-19) * EO[j][1] + (30) * EO[j][2] + (-39) * EO[j][3] + (44) * EO[j][4] + (-45) * EO[j][5] + (43) * EO[j][6] + (-36) * EO[j][7] + (27) * EO[j][8] + (-15) * EO[j][9] + (2) * EO[j][10] + (11) * EO[j][11] + (-23) * EO[j][12] + (34) * EO[j][13] + (-41) * EO[j][14] + (45) * EO[j][15] + add) >> shift));
		tmp[31] = ((((2) * EO[j][0] + (-7) * EO[j][1] + (11) * EO[j][2] + (-15) * EO[j][3] + (19) * EO[j][4] + (-23) * EO[j][5] + (27) * EO[j][6] + (-30) * EO[j][7] + (34) * EO[j][8] + (-36) * EO[j][9] + (39) * EO[j][10] + (-41) * EO[j][11] + (43) * EO[j][12] + (-44) * EO[j][13] + (45) * EO[j][14] + (-45) * EO[j][15] + add) >> shift));

		tmp[32] = ((((45) * O[j][0] + (45) * O[j][1] + (45) * O[j][2] + (45) * O[j][3] + (44) * O[j][4] + (44) * O[j][5] + (43) * O[j][6] + (42) * O[j][7] + (41) * O[j][8] + (40) * O[j][9] + (39) * O[j][10] + (38) * O[j][11] + (37) * O[j][12] + (36) * O[j][13] + (34) * O[j][14] + (33) * O[j][15] + (31) * O[j][16] + (30) * O[j][17] + (28) * O[j][18] + (26) * O[j][19] + (24) * O[j][20] + (22) * O[j][21] + (20) * O[j][22] + (18) * O[j][23] + (16) * O[j][24] + (14) * O[j][25] + (12) * O[j][26] + (10) * O[j][27] + (8) * O[j][28] + (6) * O[j][29] + (3) * O[j][30] + (1) * O[j][31] + add) >> shift));
		tmp[33] = ((((45) * O[j][0] + (44) * O[j][1] + (42) * O[j][2] + (39) * O[j][3] + (36) * O[j][4] + (31) * O[j][5] + (26) * O[j][6] + (20) * O[j][7] + (14) * O[j][8] + (8) * O[j][9] + (1) * O[j][10] + (-6) * O[j][11] + (-12) * O[j][12] + (-18) * O[j][13] + (-24) * O[j][14] + (-30) * O[j][15] + (-34) * O[j][16] + (-38) * O[j][17] + (-41) * O[j][18] + (-44) * O[j][19] + (-45) * O[j][20] + (-45) * O[j][21] + (-45) * O[j][22] + (-43) * O[j][23] + (-40) * O[j][24] + (-37) * O[j][25] + (-33) * O[j][26] + (-28) * O[j][27] + (-22) * O[j][28] + (-16) * O[j][29] + (-10) * O[j][30] + (-3) * O[j][31] + add) >> shift));
		tmp[34] = ((((45) * O[j][0] + (42) * O[j][1] + (37) * O[j][2] + (30) * O[j][3] + (20) * O[j][4] + (10) * O[j][5] + (-1) * O[j][6] + (-12) * O[j][7] + (-22) * O[j][8] + (-31) * O[j][9] + (-38) * O[j][10] + (-43) * O[j][11] + (-45) * O[j][12] + (-45) * O[j][13] + (-41) * O[j][14] + (-36) * O[j][15] + (-28) * O[j][16] + (-18) * O[j][17] + (-8) * O[j][18] + (3) * O[j][19] + (14) * O[j][20] + (24) * O[j][21] + (33) * O[j][22] + (39) * O[j][23] + (44) * O[j][24] + (45) * O[j][25] + (44) * O[j][26] + (40) * O[j][27] + (34) * O[j][28] + (26) * O[j][29] + (16) * O[j][30] + (6) * O[j][31] + add) >> shift));
		tmp[35] = ((((45) * O[j][0] + (39) * O[j][1] + (30) * O[j][2] + (16) * O[j][3] + (1) * O[j][4] + (-14) * O[j][5] + (-28) * O[j][6] + (-38) * O[j][7] + (-44) * O[j][8] + (-45) * O[j][9] + (-40) * O[j][10] + (-31) * O[j][11] + (-18) * O[j][12] + (-3) * O[j][13] + (12) * O[j][14] + (26) * O[j][15] + (37) * O[j][16] + (44) * O[j][17] + (45) * O[j][18] + (41) * O[j][19] + (33) * O[j][20] + (20) * O[j][21] + (6) * O[j][22] + (-10) * O[j][23] + (-24) * O[j][24] + (-36) * O[j][25] + (-43) * O[j][26] + (-45) * O[j][27] + (-42) * O[j][28] + (-34) * O[j][29] + (-22) * O[j][30] + (-8) * O[j][31] + add) >> shift));
		tmp[36] = ((((44) * O[j][0] + (36) * O[j][1] + (20) * O[j][2] + (1) * O[j][3] + (-18) * O[j][4] + (-34) * O[j][5] + (-44) * O[j][6] + (-45) * O[j][7] + (-37) * O[j][8] + (-22) * O[j][9] + (-3) * O[j][10] + (16) * O[j][11] + (33) * O[j][12] + (43) * O[j][13] + (45) * O[j][14] + (38) * O[j][15] + (24) * O[j][16] + (6) * O[j][17] + (-14) * O[j][18] + (-31) * O[j][19] + (-42) * O[j][20] + (-45) * O[j][21] + (-39) * O[j][22] + (-26) * O[j][23] + (-8) * O[j][24] + (12) * O[j][25] + (30) * O[j][26] + (41) * O[j][27] + (45) * O[j][28] + (40) * O[j][29] + (28) * O[j][30] + (10) * O[j][31] + add) >> shift));
		tmp[37] = ((((44) * O[j][0] + (31) * O[j][1] + (10) * O[j][2] + (-14) * O[j][3] + (-34) * O[j][4] + (-45) * O[j][5] + (-42) * O[j][6] + (-28) * O[j][7] + (-6) * O[j][8] + (18) * O[j][9] + (37) * O[j][10] + (45) * O[j][11] + (40) * O[j][12] + (24) * O[j][13] + (1) * O[j][14] + (-22) * O[j][15] + (-39) * O[j][16] + (-45) * O[j][17] + (-38) * O[j][18] + (-20) * O[j][19] + (3) * O[j][20] + (26) * O[j][21] + (41) * O[j][22] + (45) * O[j][23] + (36) * O[j][24] + (16) * O[j][25] + (-8) * O[j][26] + (-30) * O[j][27] + (-43) * O[j][28] + (-44) * O[j][29] + (-33) * O[j][30] + (-12) * O[j][31] + add) >> shift));
		tmp[38] = ((((43) * O[j][0] + (26) * O[j][1] + (-1) * O[j][2] + (-28) * O[j][3] + (-44) * O[j][4] + (-42) * O[j][5] + (-24) * O[j][6] + (3) * O[j][7] + (30) * O[j][8] + (44) * O[j][9] + (41) * O[j][10] + (22) * O[j][11] + (-6) * O[j][12] + (-31) * O[j][13] + (-45) * O[j][14] + (-40) * O[j][15] + (-20) * O[j][16] + (8) * O[j][17] + (33) * O[j][18] + (45) * O[j][19] + (39) * O[j][20] + (18) * O[j][21] + (-10) * O[j][22] + (-34) * O[j][23] + (-45) * O[j][24] + (-38) * O[j][25] + (-16) * O[j][26] + (12) * O[j][27] + (36) * O[j][28] + (45) * O[j][29] + (37) * O[j][30] + (14) * O[j][31] + add) >> shift));
		tmp[39] = ((((42) * O[j][0] + (20) * O[j][1] + (-12) * O[j][2] + (-38) * O[j][3] + (-45) * O[j][4] + (-28) * O[j][5] + (3) * O[j][6] + (33) * O[j][7] + (45) * O[j][8] + (34) * O[j][9] + (6) * O[j][10] + (-26) * O[j][11] + (-44) * O[j][12] + (-39) * O[j][13] + (-14) * O[j][14] + (18) * O[j][15] + (41) * O[j][16] + (43) * O[j][17] + (22) * O[j][18] + (-10) * O[j][19] + (-37) * O[j][20] + (-45) * O[j][21] + (-30) * O[j][22] + (1) * O[j][23] + (31) * O[j][24] + (45) * O[j][25] + (36) * O[j][26] + (8) * O[j][27] + (-24) * O[j][28] + (-44) * O[j][29] + (-40) * O[j][30] + (-16) * O[j][31] + add) >> shift));
		tmp[40] = ((((41) * O[j][0] + (14) * O[j][1] + (-22) * O[j][2] + (-44) * O[j][3] + (-37) * O[j][4] + (-6) * O[j][5] + (30) * O[j][6] + (45) * O[j][7] + (31) * O[j][8] + (-3) * O[j][9] + (-36) * O[j][10] + (-45) * O[j][11] + (-24) * O[j][12] + (12) * O[j][13] + (40) * O[j][14] + (42) * O[j][15] + (16) * O[j][16] + (-20) * O[j][17] + (-44) * O[j][18] + (-38) * O[j][19] + (-8) * O[j][20] + (28) * O[j][21] + (45) * O[j][22] + (33) * O[j][23] + (-1) * O[j][24] + (-34) * O[j][25] + (-45) * O[j][26] + (-26) * O[j][27] + (10) * O[j][28] + (39) * O[j][29] + (43) * O[j][30] + (18) * O[j][31] + add) >> shift));
		tmp[41] = ((((40) * O[j][0] + (8) * O[j][1] + (-31) * O[j][2] + (-45) * O[j][3] + (-22) * O[j][4] + (18) * O[j][5] + (44) * O[j][6] + (34) * O[j][7] + (-3) * O[j][8] + (-38) * O[j][9] + (-42) * O[j][10] + (-12) * O[j][11] + (28) * O[j][12] + (45) * O[j][13] + (26) * O[j][14] + (-14) * O[j][15] + (-43) * O[j][16] + (-37) * O[j][17] + (-1) * O[j][18] + (36) * O[j][19] + (44) * O[j][20] + (16) * O[j][21] + (-24) * O[j][22] + (-45) * O[j][23] + (-30) * O[j][24] + (10) * O[j][25] + (41) * O[j][26] + (39) * O[j][27] + (6) * O[j][28] + (-33) * O[j][29] + (-45) * O[j][30] + (-20) * O[j][31] + add) >> shift));
		tmp[42] = ((((39) * O[j][0] + (1) * O[j][1] + (-38) * O[j][2] + (-40) * O[j][3] + (-3) * O[j][4] + (37) * O[j][5] + (41) * O[j][6] + (6) * O[j][7] + (-36) * O[j][8] + (-42) * O[j][9] + (-8) * O[j][10] + (34) * O[j][11] + (43) * O[j][12] + (10) * O[j][13] + (-33) * O[j][14] + (-44) * O[j][15] + (-12) * O[j][16] + (31) * O[j][17] + (44) * O[j][18] + (14) * O[j][19] + (-30) * O[j][20] + (-45) * O[j][21] + (-16) * O[j][22] + (28) * O[j][23] + (45) * O[j][24] + (18) * O[j][25] + (-26) * O[j][26] + (-45) * O[j][27] + (-20) * O[j][28] + (24) * O[j][29] + (45) * O[j][30] + (22) * O[j][31] + add) >> shift));
		tmp[43] = ((((38) * O[j][0] + (-6) * O[j][1] + (-43) * O[j][2] + (-31) * O[j][3] + (16) * O[j][4] + (45) * O[j][5] + (22) * O[j][6] + (-26) * O[j][7] + (-45) * O[j][8] + (-12) * O[j][9] + (34) * O[j][10] + (41) * O[j][11] + (1) * O[j][12] + (-40) * O[j][13] + (-36) * O[j][14] + (10) * O[j][15] + (44) * O[j][16] + (28) * O[j][17] + (-20) * O[j][18] + (-45) * O[j][19] + (-18) * O[j][20] + (30) * O[j][21] + (44) * O[j][22] + (8) * O[j][23] + (-37) * O[j][24] + (-39) * O[j][25] + (3) * O[j][26] + (42) * O[j][27] + (33) * O[j][28] + (-14) * O[j][29] + (-45) * O[j][30] + (-24) * O[j][31] + add) >> shift));
		tmp[44] = ((((37) * O[j][0] + (-12) * O[j][1] + (-45) * O[j][2] + (-18) * O[j][3] + (33) * O[j][4] + (40) * O[j][5] + (-6) * O[j][6] + (-44) * O[j][7] + (-24) * O[j][8] + (28) * O[j][9] + (43) * O[j][10] + (1) * O[j][11] + (-42) * O[j][12] + (-30) * O[j][13] + (22) * O[j][14] + (45) * O[j][15] + (8) * O[j][16] + (-39) * O[j][17] + (-34) * O[j][18] + (16) * O[j][19] + (45) * O[j][20] + (14) * O[j][21] + (-36) * O[j][22] + (-38) * O[j][23] + (10) * O[j][24] + (45) * O[j][25] + (20) * O[j][26] + (-31) * O[j][27] + (-41) * O[j][28] + (3) * O[j][29] + (44) * O[j][30] + (26) * O[j][31] + add) >> shift));
		tmp[45] = ((((36) * O[j][0] + (-18) * O[j][1] + (-45) * O[j][2] + (-3) * O[j][3] + (43) * O[j][4] + (24) * O[j][5] + (-31) * O[j][6] + (-39) * O[j][7] + (12) * O[j][8] + (45) * O[j][9] + (10) * O[j][10] + (-40) * O[j][11] + (-30) * O[j][12] + (26) * O[j][13] + (42) * O[j][14] + (-6) * O[j][15] + (-45) * O[j][16] + (-16) * O[j][17] + (37) * O[j][18] + (34) * O[j][19] + (-20) * O[j][20] + (-44) * O[j][21] + (-1) * O[j][22] + (44) * O[j][23] + (22) * O[j][24] + (-33) * O[j][25] + (-38) * O[j][26] + (14) * O[j][27] + (45) * O[j][28] + (8) * O[j][29] + (-41) * O[j][30] + (-28) * O[j][31] + add) >> shift));
		tmp[46] = ((((34) * O[j][0] + (-24) * O[j][1] + (-41) * O[j][2] + (12) * O[j][3] + (45) * O[j][4] + (1) * O[j][5] + (-45) * O[j][6] + (-14) * O[j][7] + (40) * O[j][8] + (26) * O[j][9] + (-33) * O[j][10] + (-36) * O[j][11] + (22) * O[j][12] + (42) * O[j][13] + (-10) * O[j][14] + (-45) * O[j][15] + (-3) * O[j][16] + (44) * O[j][17] + (16) * O[j][18] + (-39) * O[j][19] + (-28) * O[j][20] + (31) * O[j][21] + (37) * O[j][22] + (-20) * O[j][23] + (-43) * O[j][24] + (8) * O[j][25] + (45) * O[j][26] + (6) * O[j][27] + (-44) * O[j][28] + (-18) * O[j][29] + (38) * O[j][30] + (30) * O[j][31] + add) >> shift));
		tmp[47] = ((((33) * O[j][0] + (-30) * O[j][1] + (-36) * O[j][2] + (26) * O[j][3] + (38) * O[j][4] + (-22) * O[j][5] + (-40) * O[j][6] + (18) * O[j][7] + (42) * O[j][8] + (-14) * O[j][9] + (-44) * O[j][10] + (10) * O[j][11] + (45) * O[j][12] + (-6) * O[j][13] + (-45) * O[j][14] + (1) * O[j][15] + (45) * O[j][16] + (3) * O[j][17] + (-45) * O[j][18] + (-8) * O[j][19] + (44) * O[j][20] + (12) * O[j][21] + (-43) * O[j][22] + (-16) * O[j][23] + (41) * O[j][24] + (20) * O[j][25] + (-39) * O[j][26] + (-24) * O[j][27] + (37) * O[j][28] + (28) * O[j][29] + (-34) * O[j][30] + (-31) * O[j][31] + add) >> shift));
		tmp[48] = ((((31) * O[j][0] + (-34) * O[j][1] + (-28) * O[j][2] + (37) * O[j][3] + (24) * O[j][4] + (-39) * O[j][5] + (-20) * O[j][6] + (41) * O[j][7] + (16) * O[j][8] + (-43) * O[j][9] + (-12) * O[j][10] + (44) * O[j][11] + (8) * O[j][12] + (-45) * O[j][13] + (-3) * O[j][14] + (45) * O[j][15] + (-1) * O[j][16] + (-45) * O[j][17] + (6) * O[j][18] + (45) * O[j][19] + (-10) * O[j][20] + (-44) * O[j][21] + (14) * O[j][22] + (42) * O[j][23] + (-18) * O[j][24] + (-40) * O[j][25] + (22) * O[j][26] + (38) * O[j][27] + (-26) * O[j][28] + (-36) * O[j][29] + (30) * O[j][30] + (33) * O[j][31] + add) >> shift));
		tmp[49] = ((((30) * O[j][0] + (-38) * O[j][1] + (-18) * O[j][2] + (44) * O[j][3] + (6) * O[j][4] + (-45) * O[j][5] + (8) * O[j][6] + (43) * O[j][7] + (-20) * O[j][8] + (-37) * O[j][9] + (31) * O[j][10] + (28) * O[j][11] + (-39) * O[j][12] + (-16) * O[j][13] + (44) * O[j][14] + (3) * O[j][15] + (-45) * O[j][16] + (10) * O[j][17] + (42) * O[j][18] + (-22) * O[j][19] + (-36) * O[j][20] + (33) * O[j][21] + (26) * O[j][22] + (-40) * O[j][23] + (-14) * O[j][24] + (45) * O[j][25] + (1) * O[j][26] + (-45) * O[j][27] + (12) * O[j][28] + (41) * O[j][29] + (-24) * O[j][30] + (-34) * O[j][31] + add) >> shift));
		tmp[50] = ((((28) * O[j][0] + (-41) * O[j][1] + (-8) * O[j][2] + (45) * O[j][3] + (-14) * O[j][4] + (-38) * O[j][5] + (33) * O[j][6] + (22) * O[j][7] + (-44) * O[j][8] + (-1) * O[j][9] + (44) * O[j][10] + (-20) * O[j][11] + (-34) * O[j][12] + (37) * O[j][13] + (16) * O[j][14] + (-45) * O[j][15] + (6) * O[j][16] + (42) * O[j][17] + (-26) * O[j][18] + (-30) * O[j][19] + (40) * O[j][20] + (10) * O[j][21] + (-45) * O[j][22] + (12) * O[j][23] + (39) * O[j][24] + (-31) * O[j][25] + (-24) * O[j][26] + (43) * O[j][27] + (3) * O[j][28] + (-45) * O[j][29] + (18) * O[j][30] + (36) * O[j][31] + add) >> shift));
		tmp[51] = ((((26) * O[j][0] + (-44) * O[j][1] + (3) * O[j][2] + (41) * O[j][3] + (-31) * O[j][4] + (-20) * O[j][5] + (45) * O[j][6] + (-10) * O[j][7] + (-38) * O[j][8] + (36) * O[j][9] + (14) * O[j][10] + (-45) * O[j][11] + (16) * O[j][12] + (34) * O[j][13] + (-39) * O[j][14] + (-8) * O[j][15] + (45) * O[j][16] + (-22) * O[j][17] + (-30) * O[j][18] + (42) * O[j][19] + (1) * O[j][20] + (-43) * O[j][21] + (28) * O[j][22] + (24) * O[j][23] + (-44) * O[j][24] + (6) * O[j][25] + (40) * O[j][26] + (-33) * O[j][27] + (-18) * O[j][28] + (45) * O[j][29] + (-12) * O[j][30] + (-37) * O[j][31] + add) >> shift));
		tmp[52] = ((((24) * O[j][0] + (-45) * O[j][1] + (14) * O[j][2] + (33) * O[j][3] + (-42) * O[j][4] + (3) * O[j][5] + (39) * O[j][6] + (-37) * O[j][7] + (-8) * O[j][8] + (44) * O[j][9] + (-30) * O[j][10] + (-18) * O[j][11] + (45) * O[j][12] + (-20) * O[j][13] + (-28) * O[j][14] + (44) * O[j][15] + (-10) * O[j][16] + (-36) * O[j][17] + (40) * O[j][18] + (1) * O[j][19] + (-41) * O[j][20] + (34) * O[j][21] + (12) * O[j][22] + (-45) * O[j][23] + (26) * O[j][24] + (22) * O[j][25] + (-45) * O[j][26] + (16) * O[j][27] + (31) * O[j][28] + (-43) * O[j][29] + (6) * O[j][30] + (38) * O[j][31] + add) >> shift));
		tmp[53] = ((((22) * O[j][0] + (-45) * O[j][1] + (24) * O[j][2] + (20) * O[j][3] + (-45) * O[j][4] + (26) * O[j][5] + (18) * O[j][6] + (-45) * O[j][7] + (28) * O[j][8] + (16) * O[j][9] + (-45) * O[j][10] + (30) * O[j][11] + (14) * O[j][12] + (-44) * O[j][13] + (31) * O[j][14] + (12) * O[j][15] + (-44) * O[j][16] + (33) * O[j][17] + (10) * O[j][18] + (-43) * O[j][19] + (34) * O[j][20] + (8) * O[j][21] + (-42) * O[j][22] + (36) * O[j][23] + (6) * O[j][24] + (-41) * O[j][25] + (37) * O[j][26] + (3) * O[j][27] + (-40) * O[j][28] + (38) * O[j][29] + (1) * O[j][30] + (-39) * O[j][31] + add) >> shift));
		tmp[54] = ((((20) * O[j][0] + (-45) * O[j][1] + (33) * O[j][2] + (6) * O[j][3] + (-39) * O[j][4] + (41) * O[j][5] + (-10) * O[j][6] + (-30) * O[j][7] + (45) * O[j][8] + (-24) * O[j][9] + (-16) * O[j][10] + (44) * O[j][11] + (-36) * O[j][12] + (-1) * O[j][13] + (37) * O[j][14] + (-43) * O[j][15] + (14) * O[j][16] + (26) * O[j][17] + (-45) * O[j][18] + (28) * O[j][19] + (12) * O[j][20] + (-42) * O[j][21] + (38) * O[j][22] + (-3) * O[j][23] + (-34) * O[j][24] + (44) * O[j][25] + (-18) * O[j][26] + (-22) * O[j][27] + (45) * O[j][28] + (-31) * O[j][29] + (-8) * O[j][30] + (40) * O[j][31] + add) >> shift));
		tmp[55] = ((((18) * O[j][0] + (-43) * O[j][1] + (39) * O[j][2] + (-10) * O[j][3] + (-26) * O[j][4] + (45) * O[j][5] + (-34) * O[j][6] + (1) * O[j][7] + (33) * O[j][8] + (-45) * O[j][9] + (28) * O[j][10] + (8) * O[j][11] + (-38) * O[j][12] + (44) * O[j][13] + (-20) * O[j][14] + (-16) * O[j][15] + (42) * O[j][16] + (-40) * O[j][17] + (12) * O[j][18] + (24) * O[j][19] + (-45) * O[j][20] + (36) * O[j][21] + (-3) * O[j][22] + (-31) * O[j][23] + (45) * O[j][24] + (-30) * O[j][25] + (-6) * O[j][26] + (37) * O[j][27] + (-44) * O[j][28] + (22) * O[j][29] + (14) * O[j][30] + (-41) * O[j][31] + add) >> shift));
		tmp[56] = ((((16) * O[j][0] + (-40) * O[j][1] + (44) * O[j][2] + (-24) * O[j][3] + (-8) * O[j][4] + (36) * O[j][5] + (-45) * O[j][6] + (31) * O[j][7] + (-1) * O[j][8] + (-30) * O[j][9] + (45) * O[j][10] + (-37) * O[j][11] + (10) * O[j][12] + (22) * O[j][13] + (-43) * O[j][14] + (41) * O[j][15] + (-18) * O[j][16] + (-14) * O[j][17] + (39) * O[j][18] + (-44) * O[j][19] + (26) * O[j][20] + (6) * O[j][21] + (-34) * O[j][22] + (45) * O[j][23] + (-33) * O[j][24] + (3) * O[j][25] + (28) * O[j][26] + (-45) * O[j][27] + (38) * O[j][28] + (-12) * O[j][29] + (-20) * O[j][30] + (42) * O[j][31] + add) >> shift));
		tmp[57] = ((((14) * O[j][0] + (-37) * O[j][1] + (45) * O[j][2] + (-36) * O[j][3] + (12) * O[j][4] + (16) * O[j][5] + (-38) * O[j][6] + (45) * O[j][7] + (-34) * O[j][8] + (10) * O[j][9] + (18) * O[j][10] + (-39) * O[j][11] + (45) * O[j][12] + (-33) * O[j][13] + (8) * O[j][14] + (20) * O[j][15] + (-40) * O[j][16] + (45) * O[j][17] + (-31) * O[j][18] + (6) * O[j][19] + (22) * O[j][20] + (-41) * O[j][21] + (44) * O[j][22] + (-30) * O[j][23] + (3) * O[j][24] + (24) * O[j][25] + (-42) * O[j][26] + (44) * O[j][27] + (-28) * O[j][28] + (1) * O[j][29] + (26) * O[j][30] + (-43) * O[j][31] + add) >> shift));
		tmp[58] = ((((12) * O[j][0] + (-33) * O[j][1] + (44) * O[j][2] + (-43) * O[j][3] + (30) * O[j][4] + (-8) * O[j][5] + (-16) * O[j][6] + (36) * O[j][7] + (-45) * O[j][8] + (41) * O[j][9] + (-26) * O[j][10] + (3) * O[j][11] + (20) * O[j][12] + (-38) * O[j][13] + (45) * O[j][14] + (-39) * O[j][15] + (22) * O[j][16] + (1) * O[j][17] + (-24) * O[j][18] + (40) * O[j][19] + (-45) * O[j][20] + (37) * O[j][21] + (-18) * O[j][22] + (-6) * O[j][23] + (28) * O[j][24] + (-42) * O[j][25] + (45) * O[j][26] + (-34) * O[j][27] + (14) * O[j][28] + (10) * O[j][29] + (-31) * O[j][30] + (44) * O[j][31] + add) >> shift));
		tmp[59] = ((((10) * O[j][0] + (-28) * O[j][1] + (40) * O[j][2] + (-45) * O[j][3] + (41) * O[j][4] + (-30) * O[j][5] + (12) * O[j][6] + (8) * O[j][7] + (-26) * O[j][8] + (39) * O[j][9] + (-45) * O[j][10] + (42) * O[j][11] + (-31) * O[j][12] + (14) * O[j][13] + (6) * O[j][14] + (-24) * O[j][15] + (38) * O[j][16] + (-45) * O[j][17] + (43) * O[j][18] + (-33) * O[j][19] + (16) * O[j][20] + (3) * O[j][21] + (-22) * O[j][22] + (37) * O[j][23] + (-45) * O[j][24] + (44) * O[j][25] + (-34) * O[j][26] + (18) * O[j][27] + (1) * O[j][28] + (-20) * O[j][29] + (36) * O[j][30] + (-44) * O[j][31] + add) >> shift));
		tmp[60] = ((((8) * O[j][0] + (-22) * O[j][1] + (34) * O[j][2] + (-42) * O[j][3] + (45) * O[j][4] + (-43) * O[j][5] + (36) * O[j][6] + (-24) * O[j][7] + (10) * O[j][8] + (6) * O[j][9] + (-20) * O[j][10] + (33) * O[j][11] + (-41) * O[j][12] + (45) * O[j][13] + (-44) * O[j][14] + (37) * O[j][15] + (-26) * O[j][16] + (12) * O[j][17] + (3) * O[j][18] + (-18) * O[j][19] + (31) * O[j][20] + (-40) * O[j][21] + (45) * O[j][22] + (-44) * O[j][23] + (38) * O[j][24] + (-28) * O[j][25] + (14) * O[j][26] + (1) * O[j][27] + (-16) * O[j][28] + (30) * O[j][29] + (-39) * O[j][30] + (45) * O[j][31] + add) >> shift));
		tmp[61] = ((((6) * O[j][0] + (-16) * O[j][1] + (26) * O[j][2] + (-34) * O[j][3] + (40) * O[j][4] + (-44) * O[j][5] + (45) * O[j][6] + (-44) * O[j][7] + (39) * O[j][8] + (-33) * O[j][9] + (24) * O[j][10] + (-14) * O[j][11] + (3) * O[j][12] + (8) * O[j][13] + (-18) * O[j][14] + (28) * O[j][15] + (-36) * O[j][16] + (41) * O[j][17] + (-45) * O[j][18] + (45) * O[j][19] + (-43) * O[j][20] + (38) * O[j][21] + (-31) * O[j][22] + (22) * O[j][23] + (-12) * O[j][24] + (1) * O[j][25] + (10) * O[j][26] + (-20) * O[j][27] + (30) * O[j][28] + (-37) * O[j][29] + (42) * O[j][30] + (-45) * O[j][31] + add) >> shift));
		tmp[62] = ((((3) * O[j][0] + (-10) * O[j][1] + (16) * O[j][2] + (-22) * O[j][3] + (28) * O[j][4] + (-33) * O[j][5] + (37) * O[j][6] + (-40) * O[j][7] + (43) * O[j][8] + (-45) * O[j][9] + (45) * O[j][10] + (-45) * O[j][11] + (44) * O[j][12] + (-41) * O[j][13] + (38) * O[j][14] + (-34) * O[j][15] + (30) * O[j][16] + (-24) * O[j][17] + (18) * O[j][18] + (-12) * O[j][19] + (6) * O[j][20] + (1) * O[j][21] + (-8) * O[j][22] + (14) * O[j][23] + (-20) * O[j][24] + (26) * O[j][25] + (-31) * O[j][26] + (36) * O[j][27] + (-39) * O[j][28] + (42) * O[j][29] + (-44) * O[j][30] + (45) * O[j][31] + add) >> shift));
		tmp[63] = ((((1) * O[j][0] + (-3) * O[j][1] + (6) * O[j][2] + (-8) * O[j][3] + (10) * O[j][4] + (-12) * O[j][5] + (14) * O[j][6] + (-16) * O[j][7] + (18) * O[j][8] + (-20) * O[j][9] + (22) * O[j][10] + (-24) * O[j][11] + (26) * O[j][12] + (-28) * O[j][13] + (30) * O[j][14] + (-31) * O[j][15] + (33) * O[j][16] + (-34) * O[j][17] + (36) * O[j][18] + (-37) * O[j][19] + (38) * O[j][20] + (-39) * O[j][21] + (40) * O[j][22] + (-41) * O[j][23] + (42) * O[j][24] + (-43) * O[j][25] + (44) * O[j][26] + (-44) * O[j][27] + (45) * O[j][28] + (-45) * O[j][29] + (45) * O[j][30] + (-45) * O[j][31] + add) >> shift));

		if (h == 8) {
			dst[0][j] = tmp[0];
			dst[4][j] = tmp[1];
			dst[2][j] = tmp[2];
			dst[6][j] = tmp[3];
			dst[1][j] = tmp[4];
			dst[3][j] = tmp[5];
			dst[5][j] = tmp[6];
			dst[7][j] = tmp[7];
		}
		else if (h == 16) {
			dst[0][j] = tmp[0];
			dst[8][j] = tmp[1];
			dst[4][j] = tmp[2];
			dst[12][j] = tmp[3];
			dst[2][j] = tmp[4];
			dst[6][j] = tmp[5];
			dst[10][j] = tmp[6];
			dst[14][j] = tmp[7];
			dst[1][j] = tmp[8];
			dst[3][j] = tmp[9];
			dst[5][j] = tmp[10];
			dst[7][j] = tmp[11];
			dst[9][j] = tmp[12];
			dst[11][j] = tmp[13];
			dst[13][j] = tmp[14];
			dst[15][j] = tmp[15];
		}
		else if (h == 32) {
			dst[0][j] = tmp[0];
			dst[16][j] = tmp[1];
			dst[8][j] = tmp[2];
			dst[24][j] = tmp[3];
			dst[4][j] = tmp[4];
			dst[12][j] = tmp[5];
			dst[20][j] = tmp[6];
			dst[28][j] = tmp[7];
			dst[2][j] = tmp[8];
			dst[6][j] = tmp[9];
			dst[10][j] = tmp[10];
			dst[14][j] = tmp[11];
			dst[18][j] = tmp[12];
			dst[22][j] = tmp[13];
			dst[26][j] = tmp[14];
			dst[30][j] = tmp[15];
			dst[1][j] = tmp[16];
			dst[3][j] = tmp[17];
			dst[5][j] = tmp[18];
			dst[7][j] = tmp[19];
			dst[9][j] = tmp[20];
			dst[11][j] = tmp[21];
			dst[13][j] = tmp[22];
			dst[15][j] = tmp[23];
			dst[17][j] = tmp[24];
			dst[19][j] = tmp[25];
			dst[21][j] = tmp[26];
			dst[23][j] = tmp[27];
			dst[25][j] = tmp[28];
			dst[27][j] = tmp[29];
			dst[29][j] = tmp[30];
			dst[31][j] = tmp[31];
		}
		else if (h == 64) {
			dst[0][j] = tmp[0];
			dst[32][j] = tmp[1];
			dst[16][j] = tmp[2];
			dst[48][j] = tmp[3];

			dst[8][j] = tmp[4];
			dst[24][j] = tmp[5];
			dst[40][j] = tmp[6];
			dst[56][j] = tmp[7];
			;
			dst[4][j] = tmp[8];
			dst[12][j] = tmp[9];
			dst[20][j] = tmp[10];
			dst[28][j] = tmp[11];
			dst[36][j] = tmp[12];
			dst[44][j] = tmp[13];
			dst[52][j] = tmp[14];
			dst[60][j] = tmp[15];

			dst[2][j] = tmp[16];
			dst[6][j] = tmp[17];
			dst[10][j] = tmp[18];
			dst[14][j] = tmp[19];
			dst[18][j] = tmp[20];
			dst[22][j] = tmp[21];
			dst[26][j] = tmp[22];
			dst[30][j] = tmp[23];
			dst[34][j] = tmp[24];
			dst[38][j] = tmp[25];
			dst[42][j] = tmp[26];
			dst[46][j] = tmp[27];
			dst[50][j] = tmp[28];
			dst[54][j] = tmp[29];
			dst[58][j] = tmp[30];
			dst[62][j] = tmp[31];

			dst[1][j] = tmp[32];
			dst[3][j] = tmp[33];
			dst[5][j] = tmp[34];
			dst[7][j] = tmp[35];
			dst[9][j] = tmp[36];
			dst[11][j] = tmp[37];
			dst[13][j] = tmp[38];
			dst[15][j] = tmp[39];
			dst[17][j] = tmp[40];
			dst[19][j] = tmp[41];
			dst[21][j] = tmp[42];
			dst[23][j] = tmp[43];
			dst[25][j] = tmp[44];
			dst[27][j] = tmp[45];
			dst[29][j] = tmp[46];
			dst[31][j] = tmp[47];
			dst[33][j] = tmp[48];
			dst[35][j] = tmp[49];
			dst[37][j] = tmp[50];
			dst[39][j] = tmp[51];
			dst[41][j] = tmp[52];
			dst[43][j] = tmp[53];
			dst[45][j] = tmp[54];
			dst[47][j] = tmp[55];
			dst[49][j] = tmp[56];
			dst[51][j] = tmp[57];
			dst[53][j] = tmp[58];
			dst[55][j] = tmp[59];
			dst[57][j] = tmp[60];
			dst[59][j] = tmp[61];
			dst[61][j] = tmp[62];
			dst[63][j] = tmp[63];
		}
	}
}

/*  IDCT core  */
void inv_dct_col_64(U7 w, U7 h, s16 coeff[64][64], s16 block[64][64], U4 shift)
{
#pragma HLS ARRAY_PARTITION variable=coeff cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=block cyclic factor=16 dim=2
	U7 size = 64;
	U8 size_b = size - 1;
	if (h == 8) {
		int E[4], O[4];
		int EE[2], EO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < w; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
			O[0] = (44) * coeff[1][i] + (38) * coeff[3][i] + (25) * coeff[5][i] + (9) * coeff[7][i];
			O[1] = (38) * coeff[1][i] + (-9) * coeff[3][i] + (-44) * coeff[5][i] + (-25) * coeff[7][i];
			O[2] = (25) * coeff[1][i] + (-44) * coeff[3][i] + (9) * coeff[5][i] + (38) * coeff[7][i];
			O[3] = (9) * coeff[1][i] + (-25) * coeff[3][i] + (38) * coeff[5][i] + (-44) * coeff[7][i];

			EO[0] = (42) * coeff[2][i] + (17) * coeff[6][i];
			EO[1] = (17) * coeff[2][i] + (-42) * coeff[6][i];
			EE[0] = (32) * coeff[0][i] + (32) * coeff[4][i];
			EE[1] = (32) * coeff[0][i] + (-32) * coeff[4][i];

			E[0] = EE[0] + EO[0];
			E[3] = EE[0] - EO[0];
			E[1] = EE[1] + EO[1];
			E[2] = EE[1] - EO[1];
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);
				block[i][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);
			}
		}
	}
	else if (h == 16) {

		int E[8], O[8];
		int EE[4], EO[4];
		int EEE[2], EEO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < w; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
			O[0] = (45) * coeff[1][i] + (43) * coeff[3][i] + (40) * coeff[5][i] + (35) * coeff[7][i] + (29) * coeff[9][i] + (21) * coeff[11][i] + (13) * coeff[13][i] + (4) * coeff[15][i];
			O[1] = (43) * coeff[1][i] + (29) * coeff[3][i] + (4) * coeff[5][i] + (-21) * coeff[7][i] + (-40) * coeff[9][i] + (-45) * coeff[11][i] + (-35) * coeff[13][i] + (-13) * coeff[15][i];
			O[2] = (40) * coeff[1][i] + (4) * coeff[3][i] + (-35) * coeff[5][i] + (-43) * coeff[7][i] + (-13) * coeff[9][i] + (29) * coeff[11][i] + (45) * coeff[13][i] + (21) * coeff[15][i];
			O[3] = (35) * coeff[1][i] + (-21) * coeff[3][i] + (-43) * coeff[5][i] + (4) * coeff[7][i] + (45) * coeff[9][i] + (13) * coeff[11][i] + (-40) * coeff[13][i] + (-29) * coeff[15][i];
			O[4] = (29) * coeff[1][i] + (-40) * coeff[3][i] + (-13) * coeff[5][i] + (45) * coeff[7][i] + (-4) * coeff[9][i] + (-43) * coeff[11][i] + (21) * coeff[13][i] + (35) * coeff[15][i];
			O[5] = (21) * coeff[1][i] + (-45) * coeff[3][i] + (29) * coeff[5][i] + (13) * coeff[7][i] + (-43) * coeff[9][i] + (35) * coeff[11][i] + (4) * coeff[13][i] + (-40) * coeff[15][i];
			O[6] = (13) * coeff[1][i] + (-35) * coeff[3][i] + (45) * coeff[5][i] + (-40) * coeff[7][i] + (21) * coeff[9][i] + (4) * coeff[11][i] + (-29) * coeff[13][i] + (43) * coeff[15][i];
			O[7] = (4) * coeff[1][i] + (-13) * coeff[3][i] + (21) * coeff[5][i] + (-29) * coeff[7][i] + (35) * coeff[9][i] + (-40) * coeff[11][i] + (43) * coeff[13][i] + (-45) * coeff[15][i];

			EO[0] = (44) * coeff[2][i] + (38) * coeff[6][i] + (25) * coeff[10][i] + (9) * coeff[14][i];
			EO[1] = (38) * coeff[2][i] + (-9) * coeff[6][i] + (-44) * coeff[10][i] + (-25) * coeff[14][i];
			EO[2] = (25) * coeff[2][i] + (-44) * coeff[6][i] + (9) * coeff[10][i] + (38) * coeff[14][i];
			EO[3] = (9) * coeff[2][i] + (-25) * coeff[6][i] + (38) * coeff[10][i] + (-44) * coeff[14][i];

			EEO[0] = (42) * coeff[4][i] + (17) * coeff[12][i];
			EEE[0] = (32) * coeff[0][i] + (32) * coeff[8][i];
			EEO[1] = (17) * coeff[4][i] + (-42) * coeff[12][i];
			EEE[1] = (32) * coeff[0][i] + (-32) * coeff[8][i];

			for (k = 0; k < 2; k++) {
//#pragma HLS UNROLL
				EE[k] = EEE[k] + EEO[k];
				EE[k + 2] = EEE[1 - k] - EEO[1 - k];
			}
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				E[k] = EE[k] + EO[k];
				E[k + 4] = EE[3 - k] - EO[3 - k];
			}
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[i][k + 8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7 - k] - O[7 - k] + rnd_factor) >> shift);
			}
		}
	}
	else if (h == 32) {

		int E[16], O[16];
		int EE[8], EO[8];
		int EEE[4], EEO[4];
		int EEEE[2], EEEO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < w; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
			O[0] = (45) * coeff[1][i] + (45) * coeff[3][i] + (44) * coeff[5][i] + (43) * coeff[7][i] + (41) * coeff[9][i] + (39) * coeff[11][i] + (36) * coeff[13][i] + (34) * coeff[15][i] + (30) * coeff[17][i] + (27) * coeff[19][i] + (23) * coeff[21][i] + (19) * coeff[23][i] + (15) * coeff[25][i] + (11) * coeff[27][i] + (7) * coeff[29][i] + (2) * coeff[31][i];
			O[1] = (45) * coeff[1][i] + (41) * coeff[3][i] + (34) * coeff[5][i] + (23) * coeff[7][i] + (11) * coeff[9][i] + (-2) * coeff[11][i] + (-15) * coeff[13][i] + (-27) * coeff[15][i] + (-36) * coeff[17][i] + (-43) * coeff[19][i] + (-45) * coeff[21][i] + (-44) * coeff[23][i] + (-39) * coeff[25][i] + (-30) * coeff[27][i] + (-19) * coeff[29][i] + (-7) * coeff[31][i];
			O[2] = (44) * coeff[1][i] + (34) * coeff[3][i] + (15) * coeff[5][i] + (-7) * coeff[7][i] + (-27) * coeff[9][i] + (-41) * coeff[11][i] + (-45) * coeff[13][i] + (-39) * coeff[15][i] + (-23) * coeff[17][i] + (-2) * coeff[19][i] + (19) * coeff[21][i] + (36) * coeff[23][i] + (45) * coeff[25][i] + (43) * coeff[27][i] + (30) * coeff[29][i] + (11) * coeff[31][i];
			O[3] = (43) * coeff[1][i] + (23) * coeff[3][i] + (-7) * coeff[5][i] + (-34) * coeff[7][i] + (-45) * coeff[9][i] + (-36) * coeff[11][i] + (-11) * coeff[13][i] + (19) * coeff[15][i] + (41) * coeff[17][i] + (44) * coeff[19][i] + (27) * coeff[21][i] + (-2) * coeff[23][i] + (-30) * coeff[25][i] + (-45) * coeff[27][i] + (-39) * coeff[29][i] + (-15) * coeff[31][i];
			O[4] = (41) * coeff[1][i] + (11) * coeff[3][i] + (-27) * coeff[5][i] + (-45) * coeff[7][i] + (-30) * coeff[9][i] + (7) * coeff[11][i] + (39) * coeff[13][i] + (43) * coeff[15][i] + (15) * coeff[17][i] + (-23) * coeff[19][i] + (-45) * coeff[21][i] + (-34) * coeff[23][i] + (2) * coeff[25][i] + (36) * coeff[27][i] + (44) * coeff[29][i] + (19) * coeff[31][i];
			O[5] = (39) * coeff[1][i] + (-2) * coeff[3][i] + (-41) * coeff[5][i] + (-36) * coeff[7][i] + (7) * coeff[9][i] + (43) * coeff[11][i] + (34) * coeff[13][i] + (-11) * coeff[15][i] + (-44) * coeff[17][i] + (-30) * coeff[19][i] + (15) * coeff[21][i] + (45) * coeff[23][i] + (27) * coeff[25][i] + (-19) * coeff[27][i] + (-45) * coeff[29][i] + (-23) * coeff[31][i];
			O[6] = (36) * coeff[1][i] + (-15) * coeff[3][i] + (-45) * coeff[5][i] + (-11) * coeff[7][i] + (39) * coeff[9][i] + (34) * coeff[11][i] + (-19) * coeff[13][i] + (-45) * coeff[15][i] + (-7) * coeff[17][i] + (41) * coeff[19][i] + (30) * coeff[21][i] + (-23) * coeff[23][i] + (-44) * coeff[25][i] + (-2) * coeff[27][i] + (43) * coeff[29][i] + (27) * coeff[31][i];
			O[7] = (34) * coeff[1][i] + (-27) * coeff[3][i] + (-39) * coeff[5][i] + (19) * coeff[7][i] + (43) * coeff[9][i] + (-11) * coeff[11][i] + (-45) * coeff[13][i] + (2) * coeff[15][i] + (45) * coeff[17][i] + (7) * coeff[19][i] + (-44) * coeff[21][i] + (-15) * coeff[23][i] + (41) * coeff[25][i] + (23) * coeff[27][i] + (-36) * coeff[29][i] + (-30) * coeff[31][i];
			O[8] = (30) * coeff[1][i] + (-36) * coeff[3][i] + (-23) * coeff[5][i] + (41) * coeff[7][i] + (15) * coeff[9][i] + (-44) * coeff[11][i] + (-7) * coeff[13][i] + (45) * coeff[15][i] + (-2) * coeff[17][i] + (-45) * coeff[19][i] + (11) * coeff[21][i] + (43) * coeff[23][i] + (-19) * coeff[25][i] + (-39) * coeff[27][i] + (27) * coeff[29][i] + (34) * coeff[31][i];
			O[9] = (27) * coeff[1][i] + (-43) * coeff[3][i] + (-2) * coeff[5][i] + (44) * coeff[7][i] + (-23) * coeff[9][i] + (-30) * coeff[11][i] + (41) * coeff[13][i] + (7) * coeff[15][i] + (-45) * coeff[17][i] + (19) * coeff[19][i] + (34) * coeff[21][i] + (-39) * coeff[23][i] + (-11) * coeff[25][i] + (45) * coeff[27][i] + (-15) * coeff[29][i] + (-36) * coeff[31][i];
			O[10] = (23) * coeff[1][i] + (-45) * coeff[3][i] + (19) * coeff[5][i] + (27) * coeff[7][i] + (-45) * coeff[9][i] + (15) * coeff[11][i] + (30) * coeff[13][i] + (-44) * coeff[15][i] + (11) * coeff[17][i] + (34) * coeff[19][i] + (-43) * coeff[21][i] + (7) * coeff[23][i] + (36) * coeff[25][i] + (-41) * coeff[27][i] + (2) * coeff[29][i] + (39) * coeff[31][i];
			O[11] = (19) * coeff[1][i] + (-44) * coeff[3][i] + (36) * coeff[5][i] + (-2) * coeff[7][i] + (-34) * coeff[9][i] + (45) * coeff[11][i] + (-23) * coeff[13][i] + (-15) * coeff[15][i] + (43) * coeff[17][i] + (-39) * coeff[19][i] + (7) * coeff[21][i] + (30) * coeff[23][i] + (-45) * coeff[25][i] + (27) * coeff[27][i] + (11) * coeff[29][i] + (-41) * coeff[31][i];
			O[12] = (15) * coeff[1][i] + (-39) * coeff[3][i] + (45) * coeff[5][i] + (-30) * coeff[7][i] + (2) * coeff[9][i] + (27) * coeff[11][i] + (-44) * coeff[13][i] + (41) * coeff[15][i] + (-19) * coeff[17][i] + (-11) * coeff[19][i] + (36) * coeff[21][i] + (-45) * coeff[23][i] + (34) * coeff[25][i] + (-7) * coeff[27][i] + (-23) * coeff[29][i] + (43) * coeff[31][i];
			O[13] = (11) * coeff[1][i] + (-30) * coeff[3][i] + (43) * coeff[5][i] + (-45) * coeff[7][i] + (36) * coeff[9][i] + (-19) * coeff[11][i] + (-2) * coeff[13][i] + (23) * coeff[15][i] + (-39) * coeff[17][i] + (45) * coeff[19][i] + (-41) * coeff[21][i] + (27) * coeff[23][i] + (-7) * coeff[25][i] + (-15) * coeff[27][i] + (34) * coeff[29][i] + (-44) * coeff[31][i];
			O[14] = (7) * coeff[1][i] + (-19) * coeff[3][i] + (30) * coeff[5][i] + (-39) * coeff[7][i] + (44) * coeff[9][i] + (-45) * coeff[11][i] + (43) * coeff[13][i] + (-36) * coeff[15][i] + (27) * coeff[17][i] + (-15) * coeff[19][i] + (2) * coeff[21][i] + (11) * coeff[23][i] + (-23) * coeff[25][i] + (34) * coeff[27][i] + (-41) * coeff[29][i] + (45) * coeff[31][i];
			O[15] = (2) * coeff[1][i] + (-7) * coeff[3][i] + (11) * coeff[5][i] + (-15) * coeff[7][i] + (19) * coeff[9][i] + (-23) * coeff[11][i] + (27) * coeff[13][i] + (-30) * coeff[15][i] + (34) * coeff[17][i] + (-36) * coeff[19][i] + (39) * coeff[21][i] + (-41) * coeff[23][i] + (43) * coeff[25][i] + (-44) * coeff[27][i] + (45) * coeff[29][i] + (-45) * coeff[31][i];

			EO[0] = (45) * coeff[2][i] + (43) * coeff[6][i] + (40) * coeff[10][i] + (35) * coeff[14][i] + (29) * coeff[18][i] + (21) * coeff[22][i] + (13) * coeff[26][i] + (4) * coeff[30][i];
			EO[1] = (43) * coeff[2][i] + (29) * coeff[6][i] + (4) * coeff[10][i] + (-21) * coeff[14][i] + (-40) * coeff[18][i] + (-45) * coeff[22][i] + (-35) * coeff[26][i] + (-13) * coeff[30][i];
			EO[2] = (40) * coeff[2][i] + (4) * coeff[6][i] + (-35) * coeff[10][i] + (-43) * coeff[14][i] + (-13) * coeff[18][i] + (29) * coeff[22][i] + (45) * coeff[26][i] + (21) * coeff[30][i];
			EO[3] = (35) * coeff[2][i] + (-21) * coeff[6][i] + (-43) * coeff[10][i] + (4) * coeff[14][i] + (45) * coeff[18][i] + (13) * coeff[22][i] + (-40) * coeff[26][i] + (-29) * coeff[30][i];
			EO[4] = (29) * coeff[2][i] + (-40) * coeff[6][i] + (-13) * coeff[10][i] + (45) * coeff[14][i] + (-4) * coeff[18][i] + (-43) * coeff[22][i] + (21) * coeff[26][i] + (35) * coeff[30][i];
			EO[5] = (21) * coeff[2][i] + (-45) * coeff[6][i] + (29) * coeff[10][i] + (13) * coeff[14][i] + (-43) * coeff[18][i] + (35) * coeff[22][i] + (4) * coeff[26][i] + (-40) * coeff[30][i];
			EO[6] = (13) * coeff[2][i] + (-35) * coeff[6][i] + (45) * coeff[10][i] + (-40) * coeff[14][i] + (21) * coeff[18][i] + (4) * coeff[22][i] + (-29) * coeff[26][i] + (43) * coeff[30][i];
			EO[7] = (4) * coeff[2][i] + (-13) * coeff[6][i] + (21) * coeff[10][i] + (-29) * coeff[14][i] + (35) * coeff[18][i] + (-40) * coeff[22][i] + (43) * coeff[26][i] + (-45) * coeff[30][i];

			EEO[0] = (44) * coeff[4][i] + (38) * coeff[12][i] + (25) * coeff[20][i] + (9) * coeff[28][i];
			EEO[1] = (38) * coeff[4][i] + (-9) * coeff[12][i] + (-44) * coeff[20][i] + (-25) * coeff[28][i];
			EEO[2] = (25) * coeff[4][i] + (-44) * coeff[12][i] + (9) * coeff[20][i] + (38) * coeff[28][i];
			EEO[3] = (9) * coeff[4][i] + (-25) * coeff[12][i] + (38) * coeff[20][i] + (-44) * coeff[28][i];

			EEEO[0] = (42) * coeff[8][i] + (17) * coeff[24][i];
			EEEO[1] = (17) * coeff[8][i] + (-42) * coeff[24][i];
			EEEE[0] = (32) * coeff[0][i] + (32) * coeff[16][i];
			EEEE[1] = (32) * coeff[0][i] + (-32) * coeff[16][i];

			EEE[0] = EEEE[0] + EEEO[0];
			EEE[3] = EEEE[0] - EEEO[0];
			EEE[1] = EEEE[1] + EEEO[1];
			EEE[2] = EEEE[1] - EEEO[1];

			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				EE[k] = EEE[k] + EEO[k];
				EE[k + 4] = EEE[3 - k] - EEO[3 - k];
			}
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				E[k] = EE[k] + EO[k];
				E[k + 8] = EE[7 - k] - EO[7 - k];
			}
			for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
				block[(i + k) & size_b][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[(i + k + 16) & size_b][k + 16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15 - k] - O[15 - k] + rnd_factor) >> shift);
			}
		}
	}
	else {

		int E[32], O[32];
		int EE[16], EO[16];
		int EEE[8], EEO[8];
		int EEEE[4], EEEO[4];
		int EEEEE[2], EEEEO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEEO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < w; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
			O[0] = (45) * coeff[1][i] + (45) * coeff[3][i] + (45) * coeff[5][i] + (45) * coeff[7][i] + (44) * coeff[9][i] + (44) * coeff[11][i] + (43) * coeff[13][i] + (42) * coeff[15][i] + (41) * coeff[17][i] + (40) * coeff[19][i] + (39) * coeff[21][i] + (38) * coeff[23][i] + (37) * coeff[25][i] + (36) * coeff[27][i] + (34) * coeff[29][i] + (33) * coeff[31][i] + (31) * coeff[33][i] + (30) * coeff[35][i] + (28) * coeff[37][i] + (26) * coeff[39][i] + (24) * coeff[41][i] + (22) * coeff[43][i] + (20) * coeff[45][i] + (18) * coeff[47][i] + (16) * coeff[49][i] + (14) * coeff[51][i] + (12) * coeff[53][i] + (10) * coeff[55][i] + (8) * coeff[57][i] + (6) * coeff[59][i] + (3) * coeff[61][i] + (1) * coeff[63][i];
			O[1] = (45) * coeff[1][i] + (44) * coeff[3][i] + (42) * coeff[5][i] + (39) * coeff[7][i] + (36) * coeff[9][i] + (31) * coeff[11][i] + (26) * coeff[13][i] + (20) * coeff[15][i] + (14) * coeff[17][i] + (8) * coeff[19][i] + (1) * coeff[21][i] + (-6) * coeff[23][i] + (-12) * coeff[25][i] + (-18) * coeff[27][i] + (-24) * coeff[29][i] + (-30) * coeff[31][i] + (-34) * coeff[33][i] + (-38) * coeff[35][i] + (-41) * coeff[37][i] + (-44) * coeff[39][i] + (-45) * coeff[41][i] + (-45) * coeff[43][i] + (-45) * coeff[45][i] + (-43) * coeff[47][i] + (-40) * coeff[49][i] + (-37) * coeff[51][i] + (-33) * coeff[53][i] + (-28) * coeff[55][i] + (-22) * coeff[57][i] + (-16) * coeff[59][i] + (-10) * coeff[61][i] + (-3) * coeff[63][i];
			O[2] = (45) * coeff[1][i] + (42) * coeff[3][i] + (37) * coeff[5][i] + (30) * coeff[7][i] + (20) * coeff[9][i] + (10) * coeff[11][i] + (-1) * coeff[13][i] + (-12) * coeff[15][i] + (-22) * coeff[17][i] + (-31) * coeff[19][i] + (-38) * coeff[21][i] + (-43) * coeff[23][i] + (-45) * coeff[25][i] + (-45) * coeff[27][i] + (-41) * coeff[29][i] + (-36) * coeff[31][i] + (-28) * coeff[33][i] + (-18) * coeff[35][i] + (-8) * coeff[37][i] + (3) * coeff[39][i] + (14) * coeff[41][i] + (24) * coeff[43][i] + (33) * coeff[45][i] + (39) * coeff[47][i] + (44) * coeff[49][i] + (45) * coeff[51][i] + (44) * coeff[53][i] + (40) * coeff[55][i] + (34) * coeff[57][i] + (26) * coeff[59][i] + (16) * coeff[61][i] + (6) * coeff[63][i];
			O[3] = (45) * coeff[1][i] + (39) * coeff[3][i] + (30) * coeff[5][i] + (16) * coeff[7][i] + (1) * coeff[9][i] + (-14) * coeff[11][i] + (-28) * coeff[13][i] + (-38) * coeff[15][i] + (-44) * coeff[17][i] + (-45) * coeff[19][i] + (-40) * coeff[21][i] + (-31) * coeff[23][i] + (-18) * coeff[25][i] + (-3) * coeff[27][i] + (12) * coeff[29][i] + (26) * coeff[31][i] + (37) * coeff[33][i] + (44) * coeff[35][i] + (45) * coeff[37][i] + (41) * coeff[39][i] + (33) * coeff[41][i] + (20) * coeff[43][i] + (6) * coeff[45][i] + (-10) * coeff[47][i] + (-24) * coeff[49][i] + (-36) * coeff[51][i] + (-43) * coeff[53][i] + (-45) * coeff[55][i] + (-42) * coeff[57][i] + (-34) * coeff[59][i] + (-22) * coeff[61][i] + (-8) * coeff[63][i];
			O[4] = (44) * coeff[1][i] + (36) * coeff[3][i] + (20) * coeff[5][i] + (1) * coeff[7][i] + (-18) * coeff[9][i] + (-34) * coeff[11][i] + (-44) * coeff[13][i] + (-45) * coeff[15][i] + (-37) * coeff[17][i] + (-22) * coeff[19][i] + (-3) * coeff[21][i] + (16) * coeff[23][i] + (33) * coeff[25][i] + (43) * coeff[27][i] + (45) * coeff[29][i] + (38) * coeff[31][i] + (24) * coeff[33][i] + (6) * coeff[35][i] + (-14) * coeff[37][i] + (-31) * coeff[39][i] + (-42) * coeff[41][i] + (-45) * coeff[43][i] + (-39) * coeff[45][i] + (-26) * coeff[47][i] + (-8) * coeff[49][i] + (12) * coeff[51][i] + (30) * coeff[53][i] + (41) * coeff[55][i] + (45) * coeff[57][i] + (40) * coeff[59][i] + (28) * coeff[61][i] + (10) * coeff[63][i];
			O[5] = (44) * coeff[1][i] + (31) * coeff[3][i] + (10) * coeff[5][i] + (-14) * coeff[7][i] + (-34) * coeff[9][i] + (-45) * coeff[11][i] + (-42) * coeff[13][i] + (-28) * coeff[15][i] + (-6) * coeff[17][i] + (18) * coeff[19][i] + (37) * coeff[21][i] + (45) * coeff[23][i] + (40) * coeff[25][i] + (24) * coeff[27][i] + (1) * coeff[29][i] + (-22) * coeff[31][i] + (-39) * coeff[33][i] + (-45) * coeff[35][i] + (-38) * coeff[37][i] + (-20) * coeff[39][i] + (3) * coeff[41][i] + (26) * coeff[43][i] + (41) * coeff[45][i] + (45) * coeff[47][i] + (36) * coeff[49][i] + (16) * coeff[51][i] + (-8) * coeff[53][i] + (-30) * coeff[55][i] + (-43) * coeff[57][i] + (-44) * coeff[59][i] + (-33) * coeff[61][i] + (-12) * coeff[63][i];
			O[6] = (43) * coeff[1][i] + (26) * coeff[3][i] + (-1) * coeff[5][i] + (-28) * coeff[7][i] + (-44) * coeff[9][i] + (-42) * coeff[11][i] + (-24) * coeff[13][i] + (3) * coeff[15][i] + (30) * coeff[17][i] + (44) * coeff[19][i] + (41) * coeff[21][i] + (22) * coeff[23][i] + (-6) * coeff[25][i] + (-31) * coeff[27][i] + (-45) * coeff[29][i] + (-40) * coeff[31][i] + (-20) * coeff[33][i] + (8) * coeff[35][i] + (33) * coeff[37][i] + (45) * coeff[39][i] + (39) * coeff[41][i] + (18) * coeff[43][i] + (-10) * coeff[45][i] + (-34) * coeff[47][i] + (-45) * coeff[49][i] + (-38) * coeff[51][i] + (-16) * coeff[53][i] + (12) * coeff[55][i] + (36) * coeff[57][i] + (45) * coeff[59][i] + (37) * coeff[61][i] + (14) * coeff[63][i];
			O[7] = (42) * coeff[1][i] + (20) * coeff[3][i] + (-12) * coeff[5][i] + (-38) * coeff[7][i] + (-45) * coeff[9][i] + (-28) * coeff[11][i] + (3) * coeff[13][i] + (33) * coeff[15][i] + (45) * coeff[17][i] + (34) * coeff[19][i] + (6) * coeff[21][i] + (-26) * coeff[23][i] + (-44) * coeff[25][i] + (-39) * coeff[27][i] + (-14) * coeff[29][i] + (18) * coeff[31][i] + (41) * coeff[33][i] + (43) * coeff[35][i] + (22) * coeff[37][i] + (-10) * coeff[39][i] + (-37) * coeff[41][i] + (-45) * coeff[43][i] + (-30) * coeff[45][i] + (1) * coeff[47][i] + (31) * coeff[49][i] + (45) * coeff[51][i] + (36) * coeff[53][i] + (8) * coeff[55][i] + (-24) * coeff[57][i] + (-44) * coeff[59][i] + (-40) * coeff[61][i] + (-16) * coeff[63][i];
			O[8] = (41) * coeff[1][i] + (14) * coeff[3][i] + (-22) * coeff[5][i] + (-44) * coeff[7][i] + (-37) * coeff[9][i] + (-6) * coeff[11][i] + (30) * coeff[13][i] + (45) * coeff[15][i] + (31) * coeff[17][i] + (-3) * coeff[19][i] + (-36) * coeff[21][i] + (-45) * coeff[23][i] + (-24) * coeff[25][i] + (12) * coeff[27][i] + (40) * coeff[29][i] + (42) * coeff[31][i] + (16) * coeff[33][i] + (-20) * coeff[35][i] + (-44) * coeff[37][i] + (-38) * coeff[39][i] + (-8) * coeff[41][i] + (28) * coeff[43][i] + (45) * coeff[45][i] + (33) * coeff[47][i] + (-1) * coeff[49][i] + (-34) * coeff[51][i] + (-45) * coeff[53][i] + (-26) * coeff[55][i] + (10) * coeff[57][i] + (39) * coeff[59][i] + (43) * coeff[61][i] + (18) * coeff[63][i];
			O[9] = (40) * coeff[1][i] + (8) * coeff[3][i] + (-31) * coeff[5][i] + (-45) * coeff[7][i] + (-22) * coeff[9][i] + (18) * coeff[11][i] + (44) * coeff[13][i] + (34) * coeff[15][i] + (-3) * coeff[17][i] + (-38) * coeff[19][i] + (-42) * coeff[21][i] + (-12) * coeff[23][i] + (28) * coeff[25][i] + (45) * coeff[27][i] + (26) * coeff[29][i] + (-14) * coeff[31][i] + (-43) * coeff[33][i] + (-37) * coeff[35][i] + (-1) * coeff[37][i] + (36) * coeff[39][i] + (44) * coeff[41][i] + (16) * coeff[43][i] + (-24) * coeff[45][i] + (-45) * coeff[47][i] + (-30) * coeff[49][i] + (10) * coeff[51][i] + (41) * coeff[53][i] + (39) * coeff[55][i] + (6) * coeff[57][i] + (-33) * coeff[59][i] + (-45) * coeff[61][i] + (-20) * coeff[63][i];
			O[10] = (39) * coeff[1][i] + (1) * coeff[3][i] + (-38) * coeff[5][i] + (-40) * coeff[7][i] + (-3) * coeff[9][i] + (37) * coeff[11][i] + (41) * coeff[13][i] + (6) * coeff[15][i] + (-36) * coeff[17][i] + (-42) * coeff[19][i] + (-8) * coeff[21][i] + (34) * coeff[23][i] + (43) * coeff[25][i] + (10) * coeff[27][i] + (-33) * coeff[29][i] + (-44) * coeff[31][i] + (-12) * coeff[33][i] + (31) * coeff[35][i] + (44) * coeff[37][i] + (14) * coeff[39][i] + (-30) * coeff[41][i] + (-45) * coeff[43][i] + (-16) * coeff[45][i] + (28) * coeff[47][i] + (45) * coeff[49][i] + (18) * coeff[51][i] + (-26) * coeff[53][i] + (-45) * coeff[55][i] + (-20) * coeff[57][i] + (24) * coeff[59][i] + (45) * coeff[61][i] + (22) * coeff[63][i];
			O[11] = (38) * coeff[1][i] + (-6) * coeff[3][i] + (-43) * coeff[5][i] + (-31) * coeff[7][i] + (16) * coeff[9][i] + (45) * coeff[11][i] + (22) * coeff[13][i] + (-26) * coeff[15][i] + (-45) * coeff[17][i] + (-12) * coeff[19][i] + (34) * coeff[21][i] + (41) * coeff[23][i] + (1) * coeff[25][i] + (-40) * coeff[27][i] + (-36) * coeff[29][i] + (10) * coeff[31][i] + (44) * coeff[33][i] + (28) * coeff[35][i] + (-20) * coeff[37][i] + (-45) * coeff[39][i] + (-18) * coeff[41][i] + (30) * coeff[43][i] + (44) * coeff[45][i] + (8) * coeff[47][i] + (-37) * coeff[49][i] + (-39) * coeff[51][i] + (3) * coeff[53][i] + (42) * coeff[55][i] + (33) * coeff[57][i] + (-14) * coeff[59][i] + (-45) * coeff[61][i] + (-24) * coeff[63][i];
			O[12] = (37) * coeff[1][i] + (-12) * coeff[3][i] + (-45) * coeff[5][i] + (-18) * coeff[7][i] + (33) * coeff[9][i] + (40) * coeff[11][i] + (-6) * coeff[13][i] + (-44) * coeff[15][i] + (-24) * coeff[17][i] + (28) * coeff[19][i] + (43) * coeff[21][i] + (1) * coeff[23][i] + (-42) * coeff[25][i] + (-30) * coeff[27][i] + (22) * coeff[29][i] + (45) * coeff[31][i] + (8) * coeff[33][i] + (-39) * coeff[35][i] + (-34) * coeff[37][i] + (16) * coeff[39][i] + (45) * coeff[41][i] + (14) * coeff[43][i] + (-36) * coeff[45][i] + (-38) * coeff[47][i] + (10) * coeff[49][i] + (45) * coeff[51][i] + (20) * coeff[53][i] + (-31) * coeff[55][i] + (-41) * coeff[57][i] + (3) * coeff[59][i] + (44) * coeff[61][i] + (26) * coeff[63][i];
			O[13] = (36) * coeff[1][i] + (-18) * coeff[3][i] + (-45) * coeff[5][i] + (-3) * coeff[7][i] + (43) * coeff[9][i] + (24) * coeff[11][i] + (-31) * coeff[13][i] + (-39) * coeff[15][i] + (12) * coeff[17][i] + (45) * coeff[19][i] + (10) * coeff[21][i] + (-40) * coeff[23][i] + (-30) * coeff[25][i] + (26) * coeff[27][i] + (42) * coeff[29][i] + (-6) * coeff[31][i] + (-45) * coeff[33][i] + (-16) * coeff[35][i] + (37) * coeff[37][i] + (34) * coeff[39][i] + (-20) * coeff[41][i] + (-44) * coeff[43][i] + (-1) * coeff[45][i] + (44) * coeff[47][i] + (22) * coeff[49][i] + (-33) * coeff[51][i] + (-38) * coeff[53][i] + (14) * coeff[55][i] + (45) * coeff[57][i] + (8) * coeff[59][i] + (-41) * coeff[61][i] + (-28) * coeff[63][i];
			O[14] = (34) * coeff[1][i] + (-24) * coeff[3][i] + (-41) * coeff[5][i] + (12) * coeff[7][i] + (45) * coeff[9][i] + (1) * coeff[11][i] + (-45) * coeff[13][i] + (-14) * coeff[15][i] + (40) * coeff[17][i] + (26) * coeff[19][i] + (-33) * coeff[21][i] + (-36) * coeff[23][i] + (22) * coeff[25][i] + (42) * coeff[27][i] + (-10) * coeff[29][i] + (-45) * coeff[31][i] + (-3) * coeff[33][i] + (44) * coeff[35][i] + (16) * coeff[37][i] + (-39) * coeff[39][i] + (-28) * coeff[41][i] + (31) * coeff[43][i] + (37) * coeff[45][i] + (-20) * coeff[47][i] + (-43) * coeff[49][i] + (8) * coeff[51][i] + (45) * coeff[53][i] + (6) * coeff[55][i] + (-44) * coeff[57][i] + (-18) * coeff[59][i] + (38) * coeff[61][i] + (30) * coeff[63][i];
			O[15] = (33) * coeff[1][i] + (-30) * coeff[3][i] + (-36) * coeff[5][i] + (26) * coeff[7][i] + (38) * coeff[9][i] + (-22) * coeff[11][i] + (-40) * coeff[13][i] + (18) * coeff[15][i] + (42) * coeff[17][i] + (-14) * coeff[19][i] + (-44) * coeff[21][i] + (10) * coeff[23][i] + (45) * coeff[25][i] + (-6) * coeff[27][i] + (-45) * coeff[29][i] + (1) * coeff[31][i] + (45) * coeff[33][i] + (3) * coeff[35][i] + (-45) * coeff[37][i] + (-8) * coeff[39][i] + (44) * coeff[41][i] + (12) * coeff[43][i] + (-43) * coeff[45][i] + (-16) * coeff[47][i] + (41) * coeff[49][i] + (20) * coeff[51][i] + (-39) * coeff[53][i] + (-24) * coeff[55][i] + (37) * coeff[57][i] + (28) * coeff[59][i] + (-34) * coeff[61][i] + (-31) * coeff[63][i];
			O[16] = (31) * coeff[1][i] + (-34) * coeff[3][i] + (-28) * coeff[5][i] + (37) * coeff[7][i] + (24) * coeff[9][i] + (-39) * coeff[11][i] + (-20) * coeff[13][i] + (41) * coeff[15][i] + (16) * coeff[17][i] + (-43) * coeff[19][i] + (-12) * coeff[21][i] + (44) * coeff[23][i] + (8) * coeff[25][i] + (-45) * coeff[27][i] + (-3) * coeff[29][i] + (45) * coeff[31][i] + (-1) * coeff[33][i] + (-45) * coeff[35][i] + (6) * coeff[37][i] + (45) * coeff[39][i] + (-10) * coeff[41][i] + (-44) * coeff[43][i] + (14) * coeff[45][i] + (42) * coeff[47][i] + (-18) * coeff[49][i] + (-40) * coeff[51][i] + (22) * coeff[53][i] + (38) * coeff[55][i] + (-26) * coeff[57][i] + (-36) * coeff[59][i] + (30) * coeff[61][i] + (33) * coeff[63][i];
			O[17] = (30) * coeff[1][i] + (-38) * coeff[3][i] + (-18) * coeff[5][i] + (44) * coeff[7][i] + (6) * coeff[9][i] + (-45) * coeff[11][i] + (8) * coeff[13][i] + (43) * coeff[15][i] + (-20) * coeff[17][i] + (-37) * coeff[19][i] + (31) * coeff[21][i] + (28) * coeff[23][i] + (-39) * coeff[25][i] + (-16) * coeff[27][i] + (44) * coeff[29][i] + (3) * coeff[31][i] + (-45) * coeff[33][i] + (10) * coeff[35][i] + (42) * coeff[37][i] + (-22) * coeff[39][i] + (-36) * coeff[41][i] + (33) * coeff[43][i] + (26) * coeff[45][i] + (-40) * coeff[47][i] + (-14) * coeff[49][i] + (45) * coeff[51][i] + (1) * coeff[53][i] + (-45) * coeff[55][i] + (12) * coeff[57][i] + (41) * coeff[59][i] + (-24) * coeff[61][i] + (-34) * coeff[63][i];
			O[18] = (28) * coeff[1][i] + (-41) * coeff[3][i] + (-8) * coeff[5][i] + (45) * coeff[7][i] + (-14) * coeff[9][i] + (-38) * coeff[11][i] + (33) * coeff[13][i] + (22) * coeff[15][i] + (-44) * coeff[17][i] + (-1) * coeff[19][i] + (44) * coeff[21][i] + (-20) * coeff[23][i] + (-34) * coeff[25][i] + (37) * coeff[27][i] + (16) * coeff[29][i] + (-45) * coeff[31][i] + (6) * coeff[33][i] + (42) * coeff[35][i] + (-26) * coeff[37][i] + (-30) * coeff[39][i] + (40) * coeff[41][i] + (10) * coeff[43][i] + (-45) * coeff[45][i] + (12) * coeff[47][i] + (39) * coeff[49][i] + (-31) * coeff[51][i] + (-24) * coeff[53][i] + (43) * coeff[55][i] + (3) * coeff[57][i] + (-45) * coeff[59][i] + (18) * coeff[61][i] + (36) * coeff[63][i];
			O[19] = (26) * coeff[1][i] + (-44) * coeff[3][i] + (3) * coeff[5][i] + (41) * coeff[7][i] + (-31) * coeff[9][i] + (-20) * coeff[11][i] + (45) * coeff[13][i] + (-10) * coeff[15][i] + (-38) * coeff[17][i] + (36) * coeff[19][i] + (14) * coeff[21][i] + (-45) * coeff[23][i] + (16) * coeff[25][i] + (34) * coeff[27][i] + (-39) * coeff[29][i] + (-8) * coeff[31][i] + (45) * coeff[33][i] + (-22) * coeff[35][i] + (-30) * coeff[37][i] + (42) * coeff[39][i] + (1) * coeff[41][i] + (-43) * coeff[43][i] + (28) * coeff[45][i] + (24) * coeff[47][i] + (-44) * coeff[49][i] + (6) * coeff[51][i] + (40) * coeff[53][i] + (-33) * coeff[55][i] + (-18) * coeff[57][i] + (45) * coeff[59][i] + (-12) * coeff[61][i] + (-37) * coeff[63][i];
			O[20] = (24) * coeff[1][i] + (-45) * coeff[3][i] + (14) * coeff[5][i] + (33) * coeff[7][i] + (-42) * coeff[9][i] + (3) * coeff[11][i] + (39) * coeff[13][i] + (-37) * coeff[15][i] + (-8) * coeff[17][i] + (44) * coeff[19][i] + (-30) * coeff[21][i] + (-18) * coeff[23][i] + (45) * coeff[25][i] + (-20) * coeff[27][i] + (-28) * coeff[29][i] + (44) * coeff[31][i] + (-10) * coeff[33][i] + (-36) * coeff[35][i] + (40) * coeff[37][i] + (1) * coeff[39][i] + (-41) * coeff[41][i] + (34) * coeff[43][i] + (12) * coeff[45][i] + (-45) * coeff[47][i] + (26) * coeff[49][i] + (22) * coeff[51][i] + (-45) * coeff[53][i] + (16) * coeff[55][i] + (31) * coeff[57][i] + (-43) * coeff[59][i] + (6) * coeff[61][i] + (38) * coeff[63][i];
			O[21] = (22) * coeff[1][i] + (-45) * coeff[3][i] + (24) * coeff[5][i] + (20) * coeff[7][i] + (-45) * coeff[9][i] + (26) * coeff[11][i] + (18) * coeff[13][i] + (-45) * coeff[15][i] + (28) * coeff[17][i] + (16) * coeff[19][i] + (-45) * coeff[21][i] + (30) * coeff[23][i] + (14) * coeff[25][i] + (-44) * coeff[27][i] + (31) * coeff[29][i] + (12) * coeff[31][i] + (-44) * coeff[33][i] + (33) * coeff[35][i] + (10) * coeff[37][i] + (-43) * coeff[39][i] + (34) * coeff[41][i] + (8) * coeff[43][i] + (-42) * coeff[45][i] + (36) * coeff[47][i] + (6) * coeff[49][i] + (-41) * coeff[51][i] + (37) * coeff[53][i] + (3) * coeff[55][i] + (-40) * coeff[57][i] + (38) * coeff[59][i] + (1) * coeff[61][i] + (-39) * coeff[63][i];
			O[22] = (20) * coeff[1][i] + (-45) * coeff[3][i] + (33) * coeff[5][i] + (6) * coeff[7][i] + (-39) * coeff[9][i] + (41) * coeff[11][i] + (-10) * coeff[13][i] + (-30) * coeff[15][i] + (45) * coeff[17][i] + (-24) * coeff[19][i] + (-16) * coeff[21][i] + (44) * coeff[23][i] + (-36) * coeff[25][i] + (-1) * coeff[27][i] + (37) * coeff[29][i] + (-43) * coeff[31][i] + (14) * coeff[33][i] + (26) * coeff[35][i] + (-45) * coeff[37][i] + (28) * coeff[39][i] + (12) * coeff[41][i] + (-42) * coeff[43][i] + (38) * coeff[45][i] + (-3) * coeff[47][i] + (-34) * coeff[49][i] + (44) * coeff[51][i] + (-18) * coeff[53][i] + (-22) * coeff[55][i] + (45) * coeff[57][i] + (-31) * coeff[59][i] + (-8) * coeff[61][i] + (40) * coeff[63][i];
			O[23] = (18) * coeff[1][i] + (-43) * coeff[3][i] + (39) * coeff[5][i] + (-10) * coeff[7][i] + (-26) * coeff[9][i] + (45) * coeff[11][i] + (-34) * coeff[13][i] + (1) * coeff[15][i] + (33) * coeff[17][i] + (-45) * coeff[19][i] + (28) * coeff[21][i] + (8) * coeff[23][i] + (-38) * coeff[25][i] + (44) * coeff[27][i] + (-20) * coeff[29][i] + (-16) * coeff[31][i] + (42) * coeff[33][i] + (-40) * coeff[35][i] + (12) * coeff[37][i] + (24) * coeff[39][i] + (-45) * coeff[41][i] + (36) * coeff[43][i] + (-3) * coeff[45][i] + (-31) * coeff[47][i] + (45) * coeff[49][i] + (-30) * coeff[51][i] + (-6) * coeff[53][i] + (37) * coeff[55][i] + (-44) * coeff[57][i] + (22) * coeff[59][i] + (14) * coeff[61][i] + (-41) * coeff[63][i];
			O[24] = (16) * coeff[1][i] + (-40) * coeff[3][i] + (44) * coeff[5][i] + (-24) * coeff[7][i] + (-8) * coeff[9][i] + (36) * coeff[11][i] + (-45) * coeff[13][i] + (31) * coeff[15][i] + (-1) * coeff[17][i] + (-30) * coeff[19][i] + (45) * coeff[21][i] + (-37) * coeff[23][i] + (10) * coeff[25][i] + (22) * coeff[27][i] + (-43) * coeff[29][i] + (41) * coeff[31][i] + (-18) * coeff[33][i] + (-14) * coeff[35][i] + (39) * coeff[37][i] + (-44) * coeff[39][i] + (26) * coeff[41][i] + (6) * coeff[43][i] + (-34) * coeff[45][i] + (45) * coeff[47][i] + (-33) * coeff[49][i] + (3) * coeff[51][i] + (28) * coeff[53][i] + (-45) * coeff[55][i] + (38) * coeff[57][i] + (-12) * coeff[59][i] + (-20) * coeff[61][i] + (42) * coeff[63][i];
			O[25] = (14) * coeff[1][i] + (-37) * coeff[3][i] + (45) * coeff[5][i] + (-36) * coeff[7][i] + (12) * coeff[9][i] + (16) * coeff[11][i] + (-38) * coeff[13][i] + (45) * coeff[15][i] + (-34) * coeff[17][i] + (10) * coeff[19][i] + (18) * coeff[21][i] + (-39) * coeff[23][i] + (45) * coeff[25][i] + (-33) * coeff[27][i] + (8) * coeff[29][i] + (20) * coeff[31][i] + (-40) * coeff[33][i] + (45) * coeff[35][i] + (-31) * coeff[37][i] + (6) * coeff[39][i] + (22) * coeff[41][i] + (-41) * coeff[43][i] + (44) * coeff[45][i] + (-30) * coeff[47][i] + (3) * coeff[49][i] + (24) * coeff[51][i] + (-42) * coeff[53][i] + (44) * coeff[55][i] + (-28) * coeff[57][i] + (1) * coeff[59][i] + (26) * coeff[61][i] + (-43) * coeff[63][i];
			O[26] = (12) * coeff[1][i] + (-33) * coeff[3][i] + (44) * coeff[5][i] + (-43) * coeff[7][i] + (30) * coeff[9][i] + (-8) * coeff[11][i] + (-16) * coeff[13][i] + (36) * coeff[15][i] + (-45) * coeff[17][i] + (41) * coeff[19][i] + (-26) * coeff[21][i] + (3) * coeff[23][i] + (20) * coeff[25][i] + (-38) * coeff[27][i] + (45) * coeff[29][i] + (-39) * coeff[31][i] + (22) * coeff[33][i] + (1) * coeff[35][i] + (-24) * coeff[37][i] + (40) * coeff[39][i] + (-45) * coeff[41][i] + (37) * coeff[43][i] + (-18) * coeff[45][i] + (-6) * coeff[47][i] + (28) * coeff[49][i] + (-42) * coeff[51][i] + (45) * coeff[53][i] + (-34) * coeff[55][i] + (14) * coeff[57][i] + (10) * coeff[59][i] + (-31) * coeff[61][i] + (44) * coeff[63][i];
			O[27] = (10) * coeff[1][i] + (-28) * coeff[3][i] + (40) * coeff[5][i] + (-45) * coeff[7][i] + (41) * coeff[9][i] + (-30) * coeff[11][i] + (12) * coeff[13][i] + (8) * coeff[15][i] + (-26) * coeff[17][i] + (39) * coeff[19][i] + (-45) * coeff[21][i] + (42) * coeff[23][i] + (-31) * coeff[25][i] + (14) * coeff[27][i] + (6) * coeff[29][i] + (-24) * coeff[31][i] + (38) * coeff[33][i] + (-45) * coeff[35][i] + (43) * coeff[37][i] + (-33) * coeff[39][i] + (16) * coeff[41][i] + (3) * coeff[43][i] + (-22) * coeff[45][i] + (37) * coeff[47][i] + (-45) * coeff[49][i] + (44) * coeff[51][i] + (-34) * coeff[53][i] + (18) * coeff[55][i] + (1) * coeff[57][i] + (-20) * coeff[59][i] + (36) * coeff[61][i] + (-44) * coeff[63][i];
			O[28] = (8) * coeff[1][i] + (-22) * coeff[3][i] + (34) * coeff[5][i] + (-42) * coeff[7][i] + (45) * coeff[9][i] + (-43) * coeff[11][i] + (36) * coeff[13][i] + (-24) * coeff[15][i] + (10) * coeff[17][i] + (6) * coeff[19][i] + (-20) * coeff[21][i] + (33) * coeff[23][i] + (-41) * coeff[25][i] + (45) * coeff[27][i] + (-44) * coeff[29][i] + (37) * coeff[31][i] + (-26) * coeff[33][i] + (12) * coeff[35][i] + (3) * coeff[37][i] + (-18) * coeff[39][i] + (31) * coeff[41][i] + (-40) * coeff[43][i] + (45) * coeff[45][i] + (-44) * coeff[47][i] + (38) * coeff[49][i] + (-28) * coeff[51][i] + (14) * coeff[53][i] + (1) * coeff[55][i] + (-16) * coeff[57][i] + (30) * coeff[59][i] + (-39) * coeff[61][i] + (45) * coeff[63][i];
			O[29] = (6) * coeff[1][i] + (-16) * coeff[3][i] + (26) * coeff[5][i] + (-34) * coeff[7][i] + (40) * coeff[9][i] + (-44) * coeff[11][i] + (45) * coeff[13][i] + (-44) * coeff[15][i] + (39) * coeff[17][i] + (-33) * coeff[19][i] + (24) * coeff[21][i] + (-14) * coeff[23][i] + (3) * coeff[25][i] + (8) * coeff[27][i] + (-18) * coeff[29][i] + (28) * coeff[31][i] + (-36) * coeff[33][i] + (41) * coeff[35][i] + (-45) * coeff[37][i] + (45) * coeff[39][i] + (-43) * coeff[41][i] + (38) * coeff[43][i] + (-31) * coeff[45][i] + (22) * coeff[47][i] + (-12) * coeff[49][i] + (1) * coeff[51][i] + (10) * coeff[53][i] + (-20) * coeff[55][i] + (30) * coeff[57][i] + (-37) * coeff[59][i] + (42) * coeff[61][i] + (-45) * coeff[63][i];
			O[30] = (3) * coeff[1][i] + (-10) * coeff[3][i] + (16) * coeff[5][i] + (-22) * coeff[7][i] + (28) * coeff[9][i] + (-33) * coeff[11][i] + (37) * coeff[13][i] + (-40) * coeff[15][i] + (43) * coeff[17][i] + (-45) * coeff[19][i] + (45) * coeff[21][i] + (-45) * coeff[23][i] + (44) * coeff[25][i] + (-41) * coeff[27][i] + (38) * coeff[29][i] + (-34) * coeff[31][i] + (30) * coeff[33][i] + (-24) * coeff[35][i] + (18) * coeff[37][i] + (-12) * coeff[39][i] + (6) * coeff[41][i] + (1) * coeff[43][i] + (-8) * coeff[45][i] + (14) * coeff[47][i] + (-20) * coeff[49][i] + (26) * coeff[51][i] + (-31) * coeff[53][i] + (36) * coeff[55][i] + (-39) * coeff[57][i] + (42) * coeff[59][i] + (-44) * coeff[61][i] + (45) * coeff[63][i];
			O[31] = (1) * coeff[1][i] + (-3) * coeff[3][i] + (6) * coeff[5][i] + (-8) * coeff[7][i] + (10) * coeff[9][i] + (-12) * coeff[11][i] + (14) * coeff[13][i] + (-16) * coeff[15][i] + (18) * coeff[17][i] + (-20) * coeff[19][i] + (22) * coeff[21][i] + (-24) * coeff[23][i] + (26) * coeff[25][i] + (-28) * coeff[27][i] + (30) * coeff[29][i] + (-31) * coeff[31][i] + (33) * coeff[33][i] + (-34) * coeff[35][i] + (36) * coeff[37][i] + (-37) * coeff[39][i] + (38) * coeff[41][i] + (-39) * coeff[43][i] + (40) * coeff[45][i] + (-41) * coeff[47][i] + (42) * coeff[49][i] + (-43) * coeff[51][i] + (44) * coeff[53][i] + (-44) * coeff[55][i] + (45) * coeff[57][i] + (-45) * coeff[59][i] + (45) * coeff[61][i] + (-45) * coeff[63][i];

			EO[0] = (45) * coeff[2][i] + (45) * coeff[6][i] + (44) * coeff[10][i] + (43) * coeff[14][i] + (41) * coeff[18][i] + (39) * coeff[22][i] + (36) * coeff[26][i] + (34) * coeff[30][i] + (30) * coeff[34][i] + (27) * coeff[38][i] + (23) * coeff[42][i] + (19) * coeff[46][i] + (15) * coeff[50][i] + (11) * coeff[54][i] + (7) * coeff[58][i] + (2) * coeff[62][i];
			EO[1] = (45) * coeff[2][i] + (41) * coeff[6][i] + (34) * coeff[10][i] + (23) * coeff[14][i] + (11) * coeff[18][i] + (-2) * coeff[22][i] + (-15) * coeff[26][i] + (-27) * coeff[30][i] + (-36) * coeff[34][i] + (-43) * coeff[38][i] + (-45) * coeff[42][i] + (-44) * coeff[46][i] + (-39) * coeff[50][i] + (-30) * coeff[54][i] + (-19) * coeff[58][i] + (-7) * coeff[62][i];
			EO[2] = (44) * coeff[2][i] + (34) * coeff[6][i] + (15) * coeff[10][i] + (-7) * coeff[14][i] + (-27) * coeff[18][i] + (-41) * coeff[22][i] + (-45) * coeff[26][i] + (-39) * coeff[30][i] + (-23) * coeff[34][i] + (-2) * coeff[38][i] + (19) * coeff[42][i] + (36) * coeff[46][i] + (45) * coeff[50][i] + (43) * coeff[54][i] + (30) * coeff[58][i] + (11) * coeff[62][i];
			EO[3] = (43) * coeff[2][i] + (23) * coeff[6][i] + (-7) * coeff[10][i] + (-34) * coeff[14][i] + (-45) * coeff[18][i] + (-36) * coeff[22][i] + (-11) * coeff[26][i] + (19) * coeff[30][i] + (41) * coeff[34][i] + (44) * coeff[38][i] + (27) * coeff[42][i] + (-2) * coeff[46][i] + (-30) * coeff[50][i] + (-45) * coeff[54][i] + (-39) * coeff[58][i] + (-15) * coeff[62][i];
			EO[4] = (41) * coeff[2][i] + (11) * coeff[6][i] + (-27) * coeff[10][i] + (-45) * coeff[14][i] + (-30) * coeff[18][i] + (7) * coeff[22][i] + (39) * coeff[26][i] + (43) * coeff[30][i] + (15) * coeff[34][i] + (-23) * coeff[38][i] + (-45) * coeff[42][i] + (-34) * coeff[46][i] + (2) * coeff[50][i] + (36) * coeff[54][i] + (44) * coeff[58][i] + (19) * coeff[62][i];
			EO[5] = (39) * coeff[2][i] + (-2) * coeff[6][i] + (-41) * coeff[10][i] + (-36) * coeff[14][i] + (7) * coeff[18][i] + (43) * coeff[22][i] + (34) * coeff[26][i] + (-11) * coeff[30][i] + (-44) * coeff[34][i] + (-30) * coeff[38][i] + (15) * coeff[42][i] + (45) * coeff[46][i] + (27) * coeff[50][i] + (-19) * coeff[54][i] + (-45) * coeff[58][i] + (-23) * coeff[62][i];
			EO[6] = (36) * coeff[2][i] + (-15) * coeff[6][i] + (-45) * coeff[10][i] + (-11) * coeff[14][i] + (39) * coeff[18][i] + (34) * coeff[22][i] + (-19) * coeff[26][i] + (-45) * coeff[30][i] + (-7) * coeff[34][i] + (41) * coeff[38][i] + (30) * coeff[42][i] + (-23) * coeff[46][i] + (-44) * coeff[50][i] + (-2) * coeff[54][i] + (43) * coeff[58][i] + (27) * coeff[62][i];
			EO[7] = (34) * coeff[2][i] + (-27) * coeff[6][i] + (-39) * coeff[10][i] + (19) * coeff[14][i] + (43) * coeff[18][i] + (-11) * coeff[22][i] + (-45) * coeff[26][i] + (2) * coeff[30][i] + (45) * coeff[34][i] + (7) * coeff[38][i] + (-44) * coeff[42][i] + (-15) * coeff[46][i] + (41) * coeff[50][i] + (23) * coeff[54][i] + (-36) * coeff[58][i] + (-30) * coeff[62][i];
			EO[8] = (30) * coeff[2][i] + (-36) * coeff[6][i] + (-23) * coeff[10][i] + (41) * coeff[14][i] + (15) * coeff[18][i] + (-44) * coeff[22][i] + (-7) * coeff[26][i] + (45) * coeff[30][i] + (-2) * coeff[34][i] + (-45) * coeff[38][i] + (11) * coeff[42][i] + (43) * coeff[46][i] + (-19) * coeff[50][i] + (-39) * coeff[54][i] + (27) * coeff[58][i] + (34) * coeff[62][i];
			EO[9] = (27) * coeff[2][i] + (-43) * coeff[6][i] + (-2) * coeff[10][i] + (44) * coeff[14][i] + (-23) * coeff[18][i] + (-30) * coeff[22][i] + (41) * coeff[26][i] + (7) * coeff[30][i] + (-45) * coeff[34][i] + (19) * coeff[38][i] + (34) * coeff[42][i] + (-39) * coeff[46][i] + (-11) * coeff[50][i] + (45) * coeff[54][i] + (-15) * coeff[58][i] + (-36) * coeff[62][i];
			EO[10] = (23) * coeff[2][i] + (-45) * coeff[6][i] + (19) * coeff[10][i] + (27) * coeff[14][i] + (-45) * coeff[18][i] + (15) * coeff[22][i] + (30) * coeff[26][i] + (-44) * coeff[30][i] + (11) * coeff[34][i] + (34) * coeff[38][i] + (-43) * coeff[42][i] + (7) * coeff[46][i] + (36) * coeff[50][i] + (-41) * coeff[54][i] + (2) * coeff[58][i] + (39) * coeff[62][i];
			EO[11] = (19) * coeff[2][i] + (-44) * coeff[6][i] + (36) * coeff[10][i] + (-2) * coeff[14][i] + (-34) * coeff[18][i] + (45) * coeff[22][i] + (-23) * coeff[26][i] + (-15) * coeff[30][i] + (43) * coeff[34][i] + (-39) * coeff[38][i] + (7) * coeff[42][i] + (30) * coeff[46][i] + (-45) * coeff[50][i] + (27) * coeff[54][i] + (11) * coeff[58][i] + (-41) * coeff[62][i];
			EO[12] = (15) * coeff[2][i] + (-39) * coeff[6][i] + (45) * coeff[10][i] + (-30) * coeff[14][i] + (2) * coeff[18][i] + (27) * coeff[22][i] + (-44) * coeff[26][i] + (41) * coeff[30][i] + (-19) * coeff[34][i] + (-11) * coeff[38][i] + (36) * coeff[42][i] + (-45) * coeff[46][i] + (34) * coeff[50][i] + (-7) * coeff[54][i] + (-23) * coeff[58][i] + (43) * coeff[62][i];
			EO[13] = (11) * coeff[2][i] + (-30) * coeff[6][i] + (43) * coeff[10][i] + (-45) * coeff[14][i] + (36) * coeff[18][i] + (-19) * coeff[22][i] + (-2) * coeff[26][i] + (23) * coeff[30][i] + (-39) * coeff[34][i] + (45) * coeff[38][i] + (-41) * coeff[42][i] + (27) * coeff[46][i] + (-7) * coeff[50][i] + (-15) * coeff[54][i] + (34) * coeff[58][i] + (-44) * coeff[62][i];
			EO[14] = (7) * coeff[2][i] + (-19) * coeff[6][i] + (30) * coeff[10][i] + (-39) * coeff[14][i] + (44) * coeff[18][i] + (-45) * coeff[22][i] + (43) * coeff[26][i] + (-36) * coeff[30][i] + (27) * coeff[34][i] + (-15) * coeff[38][i] + (2) * coeff[42][i] + (11) * coeff[46][i] + (-23) * coeff[50][i] + (34) * coeff[54][i] + (-41) * coeff[58][i] + (45) * coeff[62][i];
			EO[15] = (2) * coeff[2][i] + (-7) * coeff[6][i] + (11) * coeff[10][i] + (-15) * coeff[14][i] + (19) * coeff[18][i] + (-23) * coeff[22][i] + (27) * coeff[26][i] + (-30) * coeff[30][i] + (34) * coeff[34][i] + (-36) * coeff[38][i] + (39) * coeff[42][i] + (-41) * coeff[46][i] + (43) * coeff[50][i] + (-44) * coeff[54][i] + (45) * coeff[58][i] + (-45) * coeff[62][i];

			EEO[0] = (45) * coeff[4][i] + (43) * coeff[12][i] + (40) * coeff[20][i] + (35) * coeff[28][i] + (29) * coeff[36][i] + (21) * coeff[44][i] + (13) * coeff[52][i] + (4) * coeff[60][i];
			EEO[1] = (43) * coeff[4][i] + (29) * coeff[12][i] + (4) * coeff[20][i] + (-21) * coeff[28][i] + (-40) * coeff[36][i] + (-45) * coeff[44][i] + (-35) * coeff[52][i] + (-13) * coeff[60][i];
			EEO[2] = (40) * coeff[4][i] + (4) * coeff[12][i] + (-35) * coeff[20][i] + (-43) * coeff[28][i] + (-13) * coeff[36][i] + (29) * coeff[44][i] + (45) * coeff[52][i] + (21) * coeff[60][i];
			EEO[3] = (35) * coeff[4][i] + (-21) * coeff[12][i] + (-43) * coeff[20][i] + (4) * coeff[28][i] + (45) * coeff[36][i] + (13) * coeff[44][i] + (-40) * coeff[52][i] + (-29) * coeff[60][i];
			EEO[4] = (29) * coeff[4][i] + (-40) * coeff[12][i] + (-13) * coeff[20][i] + (45) * coeff[28][i] + (-4) * coeff[36][i] + (-43) * coeff[44][i] + (21) * coeff[52][i] + (35) * coeff[60][i];
			EEO[5] = (21) * coeff[4][i] + (-45) * coeff[12][i] + (29) * coeff[20][i] + (13) * coeff[28][i] + (-43) * coeff[36][i] + (35) * coeff[44][i] + (4) * coeff[52][i] + (-40) * coeff[60][i];
			EEO[6] = (13) * coeff[4][i] + (-35) * coeff[12][i] + (45) * coeff[20][i] + (-40) * coeff[28][i] + (21) * coeff[36][i] + (4) * coeff[44][i] + (-29) * coeff[52][i] + (43) * coeff[60][i];
			EEO[7] = (4) * coeff[4][i] + (-13) * coeff[12][i] + (21) * coeff[20][i] + (-29) * coeff[28][i] + (35) * coeff[36][i] + (-40) * coeff[44][i] + (43) * coeff[52][i] + (-45) * coeff[60][i];

			EEEO[0] = (44) * coeff[8][i] + (38) * coeff[24][i] + (25) * coeff[40][i] + (9) * coeff[56][i];
			EEEO[1] = (38) * coeff[8][i] + (-9) * coeff[24][i] + (-44) * coeff[40][i] + (-25) * coeff[56][i];
			EEEO[2] = (25) * coeff[8][i] + (-44) * coeff[24][i] + (9) * coeff[40][i] + (38) * coeff[56][i];
			EEEO[3] = (9) * coeff[8][i] + (-25) * coeff[24][i] + (38) * coeff[40][i] + (-44) * coeff[56][i];

			EEEEO[0] = (42) * coeff[16][i] + (17) * coeff[48][i];
			EEEEO[1] = (17) * coeff[16][i] + (-42) * coeff[48][i];
			EEEEE[0] = (32) * coeff[0][i] + (32) * coeff[32][i];
			EEEEE[1] = (32) * coeff[0][i] + (-32) * coeff[32][i];

			EEEE[0] = EEEEE[0] + EEEEO[0];
			EEEE[3] = EEEEE[0] - EEEEO[0];
			EEEE[1] = EEEEE[1] + EEEEO[1];
			EEEE[2] = EEEEE[1] - EEEEO[1];
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				EEE[k] = EEEE[k] + EEEO[k];
				EEE[k + 4] = EEEE[3 - k] - EEEO[3 - k];
			}
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				EE[k] = EEE[k] + EEO[k];
				EE[k + 8] = EEE[7 - k] - EEO[7 - k];
			}
			for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
				E[k] = EE[k] + EO[k];
				E[k + 16] = EE[15 - k] - EO[15 - k];
			}
			for (k = 0; k < 32; k++) {
//#pragma HLS UNROLL
				block[(i + k) & size_b][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[(i + k + 32) & size_b][k + 32] = Clip3(-(1 << 15), (1 << 15) - 1, (E[31 - k] - O[31 - k] + rnd_factor) >> shift);
			}
		}
	}
}

void inv_dct_row_64(U7 w, U7 h, s16 coeff[64][64], s16 block[64][64], U4 shift)
{
//#pragma HLS ARRAY_PARTITION variable=coeff complete dim=1
//#pragma HLS ARRAY_PARTITION variable=block complete dim=2
#pragma HLS ARRAY_PARTITION variable=coeff cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=block cyclic factor=16 dim=2
	U7 size = 64;
	U8 size_b = size - 1;
	if (w == 8) {


		int E[4], O[4];
		int EE[2], EO[2];
		int data[8];
#pragma HLS ARRAY_PARTITION variable=data complete dim=0
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < h; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
			for (int j = 0; j < 8; j++)
				data[j] = coeff[j][i];
			O[0] = (44) * data[1] + (38) * data[3] +  (25) * data[5] +   (9) * data[7];
			O[1] = (38) * data[1] + (-9) * data[3] + (-44) * data[5] + (-25) * data[7];
			O[2] =(25) * data[1] + (-44) * data[3] +   (9) * data[5] +  (38) * data[7];
			O[3] = (9) * data[1] + (-25) * data[3] +  (38) * data[5] + (-44) * data[7];

			EO[0] = (42) * data[2] +  (17) * data[6];
			EO[1] = (17) * data[2] + (-42) * data[6];
			EE[0] = (32) * data[0] +  (32) * data[4];
			EE[1] = (32) * data[0] + (-32) * data[4];

			E[0] = EE[0] + EO[0];
			E[3] = EE[0] - EO[0];
			E[1] = EE[1] + EO[1];
			E[2] = EE[1] - EO[1];
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);
				block[i][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);

			}
		}
	}
	else if (w == 16) {

		int E[8], O[8];
		int EE[4], EO[4];
		int EEE[2], EEO[2];
		int data[16];
#pragma HLS ARRAY_PARTITION variable=data complete dim=0
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < h; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
			for (int j = 0; j < 16; j++)
				data[j] = coeff[j][i];
			O[0] = (45) * data[1] + (43) * data[3] + (40) * data[5] + (35) * data[7] + (29) * data[9] + (21) * data[11] + (13) * data[13] + (4) * data[15];
			O[1] = (43) * data[1] + (29) * data[3] + (4) * data[5] + (-21) * data[7] + (-40) * data[9] + (-45) * data[11] + (-35) * data[13] + (-13) * data[15];
			O[2] = (40) * data[1] + (4) * data[3] + (-35) * data[5] + (-43) * data[7] + (-13) * data[9] + (29) * data[11] + (45) * data[13] + (21) * data[15];
			O[3] = (35) * data[1] + (-21) * data[3] + (-43) * data[5] + (4) * data[7] + (45) * data[9] + (13) * data[11] + (-40) * data[13] + (-29) * data[15];
			O[4] = (29) * data[1] + (-40) * data[3] + (-13) * data[5] + (45) * data[7] + (-4) * data[9] + (-43) * data[11] + (21) * data[13] + (35) * data[15];
			O[5] = (21) * data[1] + (-45) * data[3] + (29) * data[5] + (13) * data[7] + (-43) * data[9] + (35) * data[11] + (4) * data[13]+ (-40) * data[15];
			O[6] = (13) * data[1] + (-35) * data[3] + (45) * data[5] + (-40) * data[7] + (21) * data[9] + (4) * data[11] + (-29) * data[13] + (43) * data[15];
			O[7] = (4) * data[1] + (-13) * data[3] + (21) * data[5] + (-29) * data[7] + (35) * data[9] + (-40) * data[11] + (43) * data[13] + (-45) * data[15];

			EO[0] = (44) * data[2] + (38) * data[6] + (25) * data[10] + (9) * data[14];
			EO[1] = (38) * data[2] + (-9) * data[6] + (-44) * data[10] + (-25) * data[14];
			EO[2] = (25) * data[2] + (-44) * data[6] + (9) * data[10] + (38) * data[14];
			EO[3] = (9) * data[2] + (-25) * data[6] + (38) * data[10] + (-44) * data[14];

			EEO[0] = (42) * data[4] + (17) * data[12];
			EEE[0] = (32) * data[0] + (32) * data[8];
			EEO[1] = (17) * data[4] + (-42) * data[12];
			EEE[1] = (32) * data[0] + (-32) * data[8];

			for (k = 0; k < 2; k++) {
//#pragma HLS UNROLL
				EE[k] = EEE[k] + EEO[k];
				EE[k + 2] = EEE[1 - k] - EEO[1 - k];
			}
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				E[k] = EE[k] + EO[k];
				E[k + 4] = EE[3 - k] - EO[3 - k];
			}
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[i][k + 8] = Clip3(-(1 << 15), (1 << 15) - 1, (E[7 - k] - O[7 - k] + rnd_factor) >> shift);
			}
		}
	}
	else if (w == 32) {

		int E[16], O[16];
		int EE[8], EO[8];
		int EEE[4], EEO[4];
		int EEEE[2], EEEO[2];
		int data[32];
#pragma HLS ARRAY_PARTITION variable=data complete dim=0
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < h; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
			for (int j = 0; j < 32; j++)
				data[j] = coeff[j][i];
			O[0] = (45) * data[1] + (45) * data[3] + (44) * data[5] + (43) * data[7] + (41) * data[9] + (39) * data[11] + (36) * data[13] + (34) * data[15] + (30) * data[17] + (27) * data[19] + (23) * data[21] + (19) * data[23] + (15) * data[25] + (11) * data[27] + (7) * data[29] + (2) * data[31];
			O[1] = (45) * data[1] + (41) * data[3] + (34) * data[5] + (23) * data[7] + (11) * data[9] + (-2) * data[11] + (-15) * data[13] + (-27) * data[15] + (-36) * data[17] + (-43) * data[19] + (-45) * data[21] + (-44) * data[23] + (-39) * data[25] + (-30) * data[27] + (-19) * data[29] + (-7) * data[31];
			O[2] = (44) * data[1] + (34) * data[3] + (15) * data[5] + (-7) * data[7] + (-27) * data[9] + (-41) * data[11] + (-45) * data[13] + (-39) * data[15] + (-23) * data[17] + (-2) * data[19] + (19) * data[21] + (36) * data[23] + (45) * data[25] + (43) * data[27] + (30) * data[29] + (11) * data[31];
			O[3] = (43) * data[1] + (23) * data[3] + (-7) * data[5] + (-34) * data[7] + (-45) * data[9] + (-36) * data[11] + (-11) * data[13] + (19) * data[15] + (41) * data[17] + (44) * data[19] + (27) * data[21] + (-2) * data[23] + (-30) * data[25] + (-45) * data[27] + (-39) * data[29] + (-15) * data[31];
			O[4] = (41) * data[1] + (11) * data[3] + (-27) * data[5] + (-45) * data[7] + (-30) * data[9] + (7) * data[11] + (39) * data[13] + (43) * data[15] + (15) * data[17] + (-23) * data[19] + (-45) * data[21] + (-34) * data[23] + (2) * data[25] + (36) * data[27] + (44) * data[29] + (19) * data[31];
			O[5] = (39) * data[1] + (-2) * data[3] + (-41) * data[5] + (-36) * data[7] + (7) * data[9] + (43) * data[11] + (34) * data[13] + (-11) * data[15] + (-44) * data[17] + (-30) * data[19] + (15) * data[21] + (45) * data[23] + (27) * data[25] + (-19) * data[27] + (-45) * data[29] + (-23) * data[31];
			O[6] = (36) * data[1] + (-15) * data[3] + (-45) * data[5] + (-11) * data[7] + (39) * data[9] + (34) * data[11] + (-19) * data[13] + (-45) * data[15] + (-7) * data[17] + (41) * data[19] + (30) * data[21] + (-23) * data[23] + (-44) * data[25] + (-2) * data[27] + (43) * data[29] + (27) * data[31];
			O[7] = (34) * data[1] + (-27) * data[3] + (-39) * data[5] + (19) * data[7] + (43) * data[9] + (-11) * data[11] + (-45) * data[13] + (2) * data[15] + (45) * data[17] + (7) * data[19] + (-44) * data[21] + (-15) * data[23] + (41) * data[25] + (23) * data[27] + (-36) * data[29] + (-30) * data[31];
			O[8] = (30) * data[1] + (-36) * data[3] + (-23) * data[5] + (41) * data[7] + (15) * data[9] + (-44) * data[11] + (-7) * data[13] + (45) * data[15] + (-2) * data[17] + (-45) * data[19] + (11) * data[21] + (43) * data[23] + (-19) * data[25] + (-39) * data[27] + (27) * data[29] + (34) * data[31];
			O[9] = (27) * data[1] + (-43) * data[3] + (-2) * data[5] + (44) * data[7] + (-23) * data[9] + (-30) * data[11] + (41) * data[13] + (7) * data[15] + (-45) * data[17] + (19) * data[19] + (34) * data[21] + (-39) * data[23] + (-11) * data[25] + (45) * data[27] + (-15) * data[29] + (-36) * data[31];
			O[10] = (23) * data[1] + (-45) * data[3] + (19) * data[5] + (27) * data[7] + (-45) * data[9] + (15) * data[11] + (30) * data[13] + (-44) * data[15] + (11) * data[17] + (34) * data[19] + (-43) * data[21] + (7) * data[23] + (36) * data[25] + (-41) * data[27] + (2) * data[29] + (39) * data[31];
			O[11] = (19) * data[1] + (-44) * data[3] + (36) * data[5] + (-2) * data[7] + (-34) * data[9] + (45) * data[11] + (-23) * data[13] + (-15) * data[15] + (43) * data[17] + (-39) * data[19] + (7) * data[21] + (30) * data[23] + (-45) * data[25] + (27) * data[27] + (11) * data[29] + (-41) * data[31];
			O[12] = (15) * data[1] + (-39) * data[3] + (45) * data[5] + (-30) * data[7] + (2) * data[9] + (27) * data[11] + (-44) * data[13] + (41) * data[15] + (-19) * data[17] + (-11) * data[19] + (36) * data[21] + (-45) * data[23] + (34) * data[25] + (-7) * data[27] + (-23) * data[29] + (43) * data[31];
			O[13] = (11) * data[1] + (-30) * data[3] + (43) * data[5] + (-45) * data[7] + (36) * data[9] + (-19) * data[11] + (-2) * data[13] + (23) * data[15] + (-39) * data[17] + (45) * data[19] + (-41) * data[21] + (27) * data[23] + (-7) * data[25] + (-15) * data[27] + (34) * data[29] + (-44) * data[31];
			O[14] = (7) * data[1] + (-19) * data[3] + (30) * data[5] + (-39) * data[7] + (44) * data[9] + (-45) * data[11] + (43) * data[13] + (-36) * data[15] + (27) * data[17] + (-15) * data[19] + (2) * data[21] + (11) * data[23] + (-23) * data[25] + (34) * data[27] + (-41) * data[29] + (45) * data[31];
			O[15] = (2) * data[1] + (-7) * data[3] + (11) * data[5] + (-15) * data[7] + (19) * data[9] + (-23) * data[11] + (27) * data[13] + (-30) * data[15] + (34) * data[17] + (-36) * data[19] + (39) * data[21] + (-41) * data[23] + (43) * data[25] + (-44) * data[27] + (45) * data[29] + (-45) * data[31];

			EO[0] = (45) * data[2] + (43) * data[6] + (40) * data[10] + (35) * data[14] + (29) * data[18] + (21) * data[22] + (13) * data[26] + (4) * data[30];
			EO[1] = (43) * data[2] + (29) * data[6] + (4) * data[10] + (-21) * data[14] + (-40) * data[18] + (-45) * data[22] + (-35) * data[26] + (-13) * data[30];
			EO[2] = (40) * data[2] + (4) * data[6] + (-35) * data[10] + (-43) * data[14] + (-13) * data[18] + (29) * data[22] + (45) * data[26] + (21) * data[30];
			EO[3] = (35) * data[2] + (-21) * data[6] + (-43) * data[10] + (4) * data[14] + (45) * data[18] + (13) * data[22] + (-40) * data[26] + (-29) * data[30];
			EO[4] = (29) * data[2] + (-40) * data[6] + (-13) * data[10] + (45) * data[14] + (-4) * data[18] + (-43) * data[22] + (21) * data[26] + (35) * data[30];
			EO[5] = (21) * data[2] + (-45) * data[6] + (29) * data[10] + (13) * data[14] + (-43) * data[18] + (35) * data[22] + (4) * data[26] + (-40) * data[30];
			EO[6] = (13) * data[2] + (-35) * data[6] + (45) * data[10] + (-40) * data[14] + (21) * data[18] + (4) * data[22] + (-29) * data[26] + (43) * data[30];
			EO[7] = (4) * data[2] + (-13) * data[6] + (21) * data[10] + (-29) * data[14] + (35) * data[18] + (-40) * data[22] + (43) * data[26] + (-45) * data[30];

			EEO[0] = (44) * data[4] + (38) * data[12] + (25) * data[20] + (9) * data[28];
			EEO[1] = (38) * data[4] + (-9) * data[12] + (-44) * data[20] + (-25) * data[28];
			EEO[2] = (25) * data[4] + (-44) * data[12] + (9) * data[20] + (38) * data[28];
			EEO[3] = (9) * data[4] + (-25) * data[12] + (38) * data[20] + (-44) * data[28];

			EEEO[0] = (42) * data[8] + (17) * data[24];
			EEEO[1] = (17) * data[8] + (-42) * data[24];
			EEEE[0] = (32) * data[0] + (32) * data[16];
			EEEE[1] = (32) * data[0] + (-32) * data[16];
			EEE[0] = EEEE[0] + EEEO[0];
			EEE[3] = EEEE[0] - EEEO[0];
			EEE[1] = EEEE[1] + EEEO[1];
			EEE[2] = EEEE[1] - EEEO[1];
			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				EE[k] = EEE[k] + EEO[k];
				EE[k + 4] = EEE[3 - k] - EEO[3 - k];
			}
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				E[k] = EE[k] + EO[k];
				E[k + 8] = EE[7 - k] - EO[7 - k];
			}
			for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[i][k + 16] = Clip3(-(1 << 15), (1 << 15) - 1, (E[15 - k] - O[15 - k] + rnd_factor) >> shift);
			}
		}
	}
	else {

		int E[32], O[32];
		int EE[16], EO[16];
		int EEE[8], EEO[8];
		int EEEE[4], EEEO[4];
		int EEEEE[2], EEEEO[2];
		pel data[64];
#pragma HLS ARRAY_PARTITION variable=data complete dim=0
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEO complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEEE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EEEEO complete dim=0
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < h; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
			for (int j = 0; j < 64; j++)
				data[j] = coeff[j][i];
			O[0] = (45) * data[1] + (45) * data[3] + (45) * data[5] + (45) * data[7] + (44) * data[9] + (44) * data[11] + (43) * data[13] + (42) * data[15] + (41) * data[17] + (40) * data[19] + (39) * data[21] + (38) * data[23] + (37) * data[25] + (36) * data[27] + (34) * data[29] + (33) * data[31] + (31) * data[33] + (30) * data[35] + (28) * data[37] + (26) * data[39] + (24) * data[41] + (22) * data[43] + (20) * data[45] + (18) * data[47] + (16) * data[49] + (14) * data[51] + (12) * data[53] + (10) * data[55] + (8) * data[57] + (6) * data[59] + (3) * data[61] + (1) * data[63];
			O[1] = (45) * data[1] + (44) * data[3] + (42) * data[5] + (39) * data[7] + (36) * data[9] + (31) * data[11] + (26) * data[13] + (20) * data[15] + (14) * data[17] + (8) * data[19] + (1) * data[21] + (-6) * data[23] + (-12) * data[25] + (-18) * data[27] + (-24) * data[29] + (-30) * data[31] + (-34) * data[33] + (-38) * data[35] + (-41) * data[37] + (-44) * data[39] + (-45) * data[41] + (-45) * data[43] + (-45) * data[45] + (-43) * data[47] + (-40) * data[49] + (-37) * data[51] + (-33) * data[53] + (-28) * data[55] + (-22) * data[57] + (-16) * data[59] + (-10) * data[61] + (-3) * data[63];
			O[2] = (45) * data[1] + (42) * data[3] + (37) * data[5] + (30) * data[7] + (20) * data[9] + (10) * data[11] + (-1) * data[13] + (-12) * data[15] + (-22) * data[17] + (-31) * data[19] + (-38) * data[21] + (-43) * data[23] + (-45) * data[25] + (-45) * data[27] + (-41) * data[29] + (-36) * data[31] + (-28) * data[33] + (-18) * data[35] + (-8) * data[37] + (3) * data[39] + (14) * data[41] + (24) * data[43] + (33) * data[45] + (39) * data[47] + (44) * data[49] + (45) * data[51] + (44) * data[53] + (40) * data[55] + (34) * data[57] + (26) * data[59] + (16) * data[61] + (6) * data[63];
			O[3] = (45) * data[1] + (39) * data[3] + (30) * data[5] + (16) * data[7] + (1) * data[9] + (-14) * data[11] + (-28) * data[13] + (-38) * data[15] + (-44) * data[17] + (-45) * data[19] + (-40) * data[21] + (-31) * data[23] + (-18) * data[25] + (-3) * data[27] + (12) * data[29] + (26) * data[31] + (37) * data[33] + (44) * data[35] + (45) * data[37] + (41) * data[39] + (33) * data[41] + (20) * data[43] + (6) * data[45] + (-10) * data[47] + (-24) * data[49] + (-36) * data[51] + (-43) * data[53] + (-45) * data[55] + (-42) * data[57] + (-34) * data[59] + (-22) * data[61] + (-8) * data[63];
			O[4] = (44) * data[1] + (36) * data[3] + (20) * data[5] + (1) * data[7] + (-18) * data[9] + (-34) * data[11] + (-44) * data[13] + (-45) * data[15] + (-37) * data[17] + (-22) * data[19] + (-3) * data[21] + (16) * data[23] + (33) * data[25] + (43) * data[27] + (45) * data[29] + (38) * data[31] + (24) * data[33] + (6) * data[35] + (-14) * data[37] + (-31) * data[39] + (-42) * data[41] + (-45) * data[43] + (-39) * data[45] + (-26) * data[47] + (-8) * data[49] + (12) * data[51] + (30) * data[53] + (41) * data[55] + (45) * data[57] + (40) * data[59] + (28) * data[61] + (10) * data[63];
			O[5] = (44) * data[1] + (31) * data[3] + (10) * data[5] + (-14) * data[7] + (-34) * data[9] + (-45) * data[11] + (-42) * data[13] + (-28) * data[15] + (-6) * data[17] + (18) * data[19] + (37) * data[21] + (45) * data[23] + (40) * data[25] + (24) * data[27] + (1) * data[29] + (-22) * data[31] + (-39) * data[33] + (-45) * data[35] + (-38) * data[37] + (-20) * data[39] + (3) * data[41] + (26) * data[43] + (41) * data[45] + (45) * data[47] + (36) * data[49] + (16) * data[51] + (-8) * data[53] + (-30) * data[55] + (-43) * data[57] + (-44) * data[59] + (-33) * data[61] + (-12) * data[63];
			O[6] = (43) * data[1] + (26) * data[3] + (-1) * data[5] + (-28) * data[7] + (-44) * data[9] + (-42) * data[11] + (-24) * data[13] + (3) * data[15] + (30) * data[17] + (44) * data[19] + (41) * data[21] + (22) * data[23] + (-6) * data[25] + (-31) * data[27] + (-45) * data[29] + (-40) * data[31] + (-20) * data[33] + (8) * data[35] + (33) * data[37] + (45) * data[39] + (39) * data[41] + (18) * data[43] + (-10) * data[45] + (-34) * data[47] + (-45) * data[49] + (-38) * data[51] + (-16) * data[53] + (12) * data[55] + (36) * data[57] + (45) * data[59] + (37) * data[61] + (14) * data[63];
			O[7] = (42) * data[1] + (20) * data[3] + (-12) * data[5] + (-38) * data[7] + (-45) * data[9] + (-28) * data[11] + (3) * data[13] + (33) * data[15] + (45) * data[17] + (34) * data[19] + (6) * data[21] + (-26) * data[23] + (-44) * data[25] + (-39) * data[27] + (-14) * data[29] + (18) * data[31] + (41) * data[33] + (43) * data[35] + (22) * data[37] + (-10) * data[39] + (-37) * data[41] + (-45) * data[43] + (-30) * data[45] + (1) * data[47] + (31) * data[49] + (45) * data[51] + (36) * data[53] + (8) * data[55] + (-24) * data[57] + (-44) * data[59] + (-40) * data[61] + (-16) * data[63];
			O[8] = (41) * data[1] + (14) * data[3] + (-22) * data[5] + (-44) * data[7] + (-37) * data[9] + (-6) * data[11] + (30) * data[13] + (45) * data[15] + (31) * data[17] + (-3) * data[19] + (-36) * data[21] + (-45) * data[23] + (-24) * data[25] + (12) * data[27] + (40) * data[29] + (42) * data[31] + (16) * data[33] + (-20) * data[35] + (-44) * data[37] + (-38) * data[39] + (-8) * data[41] + (28) * data[43] + (45) * data[45] + (33) * data[47] + (-1) * data[49] + (-34) * data[51] + (-45) * data[53] + (-26) * data[55] + (10) * data[57] + (39) * data[59] + (43) * data[61] + (18) * data[63];
			O[9] = (40) * data[1] + (8) * data[3] + (-31) * data[5] + (-45) * data[7] + (-22) * data[9] + (18) * data[11] + (44) * data[13] + (34) * data[15] + (-3) * data[17] + (-38) * data[19] + (-42) * data[21] + (-12) * data[23] + (28) * data[25] + (45) * data[27] + (26) * data[29] + (-14) * data[31] + (-43) * data[33] + (-37) * data[35] + (-1) * data[37] + (36) * data[39] + (44) * data[41] + (16) * data[43] + (-24) * data[45] + (-45) * data[47] + (-30) * data[49] + (10) * data[51] + (41) * data[53] + (39) * data[55] + (6) * data[57] + (-33) * data[59] + (-45) * data[61] + (-20) * data[63];
			O[10] = (39) * data[1] + (1) * data[3] + (-38) * data[5] + (-40) * data[7] + (-3) * data[9] + (37) * data[11] + (41) * data[13] + (6) * data[15] + (-36) * data[17] + (-42) * data[19] + (-8) * data[21] + (34) * data[23] + (43) * data[25] + (10) * data[27] + (-33) * data[29] + (-44) * data[31] + (-12) * data[33] + (31) * data[35] + (44) * data[37] + (14) * data[39] + (-30) * data[41] + (-45) * data[43] + (-16) * data[45] + (28) * data[47] + (45) * data[49] + (18) * data[51] + (-26) * data[53] + (-45) * data[55] + (-20) * data[57] + (24) * data[59] + (45) * data[61] + (22) * data[63];
			O[11] = (38) * data[1] + (-6) * data[3] + (-43) * data[5] + (-31) * data[7] + (16) * data[9] + (45) * data[11] + (22) * data[13] + (-26) * data[15] + (-45) * data[17] + (-12) * data[19] + (34) * data[21] + (41) * data[23] + (1) * data[25] + (-40) * data[27] + (-36) * data[29] + (10) * data[31] + (44) * data[33] + (28) * data[35] + (-20) * data[37] + (-45) * data[39] + (-18) * data[41] + (30) * data[43] + (44) * data[45] + (8) * data[47] + (-37) * data[49] + (-39) * data[51] + (3) * data[53] + (42) * data[55] + (33) * data[57] + (-14) * data[59] + (-45) * data[61] + (-24) * data[63];
			O[12] = (37) * data[1] + (-12) * data[3] + (-45) * data[5] + (-18) * data[7] + (33) * data[9] + (40) * data[11] + (-6) * data[13] + (-44) * data[15] + (-24) * data[17] + (28) * data[19] + (43) * data[21] + (1) * data[23] + (-42) * data[25] + (-30) * data[27] + (22) * data[29] + (45) * data[31] + (8) * data[33] + (-39) * data[35] + (-34) * data[37] + (16) * data[39] + (45) * data[41] + (14) * data[43] + (-36) * data[45] + (-38) * data[47] + (10) * data[49] + (45) * data[51] + (20) * data[53] + (-31) * data[55] + (-41) * data[57] + (3) * data[59] + (44) * data[61] + (26) * data[63];
			O[13] = (36) * data[1] + (-18) * data[3] + (-45) * data[5] + (-3) * data[7] + (43) * data[9] + (24) * data[11] + (-31) * data[13] + (-39) * data[15] + (12) * data[17] + (45) * data[19] + (10) * data[21] + (-40) * data[23] + (-30) * data[25] + (26) * data[27] + (42) * data[29] + (-6) * data[31] + (-45) * data[33] + (-16) * data[35] + (37) * data[37] + (34) * data[39] + (-20) * data[41] + (-44) * data[43] + (-1) * data[45] + (44) * data[47] + (22) * data[49] + (-33) * data[51] + (-38) * data[53] + (14) * data[55] + (45) * data[57] + (8) * data[59] + (-41) * data[61] + (-28) * data[63];
			O[14] = (34) * data[1] + (-24) * data[3] + (-41) * data[5] + (12) * data[7] + (45) * data[9] + (1) * data[11] + (-45) * data[13] + (-14) * data[15] + (40) * data[17] + (26) * data[19] + (-33) * data[21] + (-36) * data[23] + (22) * data[25] + (42) * data[27] + (-10) * data[29] + (-45) * data[31] + (-3) * data[33] + (44) * data[35] + (16) * data[37] + (-39) * data[39] + (-28) * data[41] + (31) * data[43] + (37) * data[45] + (-20) * data[47] + (-43) * data[49] + (8) * data[51] + (45) * data[53] + (6) * data[55] + (-44) * data[57] + (-18) * data[59] + (38) * data[61] + (30) * data[63];
			O[15] = (33) * data[1] + (-30) * data[3] + (-36) * data[5] + (26) * data[7] + (38) * data[9] + (-22) * data[11] + (-40) * data[13] + (18) * data[15] + (42) * data[17] + (-14) * data[19] + (-44) * data[21] + (10) * data[23] + (45) * data[25] + (-6) * data[27] + (-45) * data[29] + (1) * data[31] + (45) * data[33] + (3) * data[35] + (-45) * data[37] + (-8) * data[39] + (44) * data[41] + (12) * data[43] + (-43) * data[45] + (-16) * data[47] + (41) * data[49] + (20) * data[51] + (-39) * data[53] + (-24) * data[55] + (37) * data[57] + (28) * data[59] + (-34) * data[61] + (-31) * data[63];
			O[16] = (31) * data[1] + (-34) * data[3] + (-28) * data[5] + (37) * data[7] + (24) * data[9] + (-39) * data[11] + (-20) * data[13] + (41) * data[15] + (16) * data[17] + (-43) * data[19] + (-12) * data[21] + (44) * data[23] + (8) * data[25] + (-45) * data[27] + (-3) * data[29] + (45) * data[31] + (-1) * data[33] + (-45) * data[35] + (6) * data[37] + (45) * data[39] + (-10) * data[41] + (-44) * data[43] + (14) * data[45] + (42) * data[47] + (-18) * data[49] + (-40) * data[51] + (22) * data[53] + (38) * data[55] + (-26) * data[57] + (-36) * data[59] + (30) * data[61] + (33) * data[63];
			O[17] = (30) * data[1] + (-38) * data[3] + (-18) * data[5] + (44) * data[7] + (6) * data[9] + (-45) * data[11] + (8) * data[13] + (43) * data[15] + (-20) * data[17] + (-37) * data[19] + (31) * data[21] + (28) * data[23] + (-39) * data[25] + (-16) * data[27] + (44) * data[29] + (3) * data[31] + (-45) * data[33] + (10) * data[35] + (42) * data[37] + (-22) * data[39] + (-36) * data[41] + (33) * data[43] + (26) * data[45] + (-40) * data[47] + (-14) * data[49] + (45) * data[51] + (1) * data[53] + (-45) * data[55] + (12) * data[57] + (41) * data[59] + (-24) * data[61] + (-34) * data[63];
			O[18] = (28) * data[1] + (-41) * data[3] + (-8) * data[5] + (45) * data[7] + (-14) * data[9] + (-38) * data[11] + (33) * data[13] + (22) * data[15] + (-44) * data[17] + (-1) * data[19] + (44) * data[21] + (-20) * data[23] + (-34) * data[25] + (37) * data[27] + (16) * data[29] + (-45) * data[31] + (6) * data[33] + (42) * data[35] + (-26) * data[37] + (-30) * data[39] + (40) * data[41] + (10) * data[43] + (-45) * data[45] + (12) * data[47] + (39) * data[49] + (-31) * data[51] + (-24) * data[53] + (43) * data[55] + (3) * data[57] + (-45) * data[59] + (18) * data[61] + (36) * data[63];
			O[19] = (26) * data[1] + (-44) * data[3] + (3) * data[5] + (41) * data[7] + (-31) * data[9] + (-20) * data[11] + (45) * data[13] + (-10) * data[15] + (-38) * data[17] + (36) * data[19] + (14) * data[21] + (-45) * data[23] + (16) * data[25] + (34) * data[27] + (-39) * data[29] + (-8) * data[31] + (45) * data[33] + (-22) * data[35] + (-30) * data[37] + (42) * data[39] + (1) * data[41] + (-43) * data[43] + (28) * data[45] + (24) * data[47] + (-44) * data[49] + (6) * data[51] + (40) * data[53] + (-33) * data[55] + (-18) * data[57] + (45) * data[59] + (-12) * data[61] + (-37) * data[63];
			O[20] = (24) * data[1] + (-45) * data[3] + (14) * data[5] + (33) * data[7] + (-42) * data[9] + (3) * data[11] + (39) * data[13] + (-37) * data[15] + (-8) * data[17] + (44) * data[19] + (-30) * data[21] + (-18) * data[23] + (45) * data[25] + (-20) * data[27] + (-28) * data[29] + (44) * data[31] + (-10) * data[33] + (-36) * data[35] + (40) * data[37] + (1) * data[39] + (-41) * data[41] + (34) * data[43] + (12) * data[45] + (-45) * data[47] + (26) * data[49] + (22) * data[51] + (-45) * data[53] + (16) * data[55] + (31) * data[57] + (-43) * data[59] + (6) * data[61] + (38) * data[63];
			O[21] = (22) * data[1] + (-45) * data[3] + (24) * data[5] + (20) * data[7] + (-45) * data[9] + (26) * data[11] + (18) * data[13] + (-45) * data[15] + (28) * data[17] + (16) * data[19] + (-45) * data[21] + (30) * data[23] + (14) * data[25] + (-44) * data[27] + (31) * data[29] + (12) * data[31] + (-44) * data[33] + (33) * data[35] + (10) * data[37] + (-43) * data[39] + (34) * data[41] + (8) * data[43] + (-42) * data[45] + (36) * data[47] + (6) * data[49] + (-41) * data[51] + (37) * data[53] + (3) * data[55] + (-40) * data[57] + (38) * data[59] + (1) * data[61] + (-39) * data[63];
			O[22] = (20) * data[1] + (-45) * data[3] + (33) * data[5] + (6) * data[7] + (-39) * data[9] + (41) * data[11] + (-10) * data[13] + (-30) * data[15] + (45) * data[17] + (-24) * data[19] + (-16) * data[21] + (44) * data[23] + (-36) * data[25] + (-1) * data[27] + (37) * data[29] + (-43) * data[31] + (14) * data[33] + (26) * data[35] + (-45) * data[37] + (28) * data[39] + (12) * data[41] + (-42) * data[43] + (38) * data[45] + (-3) * data[47] + (-34) * data[49] + (44) * data[51] + (-18) * data[53] + (-22) * data[55] + (45) * data[57] + (-31) * data[59] + (-8) * data[61] + (40) * data[63];
			O[23] = (18) * data[1] + (-43) * data[3] + (39) * data[5] + (-10) * data[7] + (-26) * data[9] + (45) * data[11] + (-34) * data[13] + (1) * data[15] + (33) * data[17] + (-45) * data[19] + (28) * data[21] + (8) * data[23] + (-38) * data[25] + (44) * data[27] + (-20) * data[29] + (-16) * data[31] + (42) * data[33] + (-40) * data[35] + (12) * data[37] + (24) * data[39] + (-45) * data[41] + (36) * data[43] + (-3) * data[45] + (-31) * data[47] + (45) * data[49] + (-30) * data[51] + (-6) * data[53] + (37) * data[55] + (-44) * data[57] + (22) * data[59] + (14) * data[61] + (-41) * data[63];
			O[24] = (16) * data[1] + (-40) * data[3] + (44) * data[5] + (-24) * data[7] + (-8) * data[9] + (36) * data[11] + (-45) * data[13] + (31) * data[15] + (-1) * data[17] + (-30) * data[19] + (45) * data[21] + (-37) * data[23] + (10) * data[25] + (22) * data[27] + (-43) * data[29] + (41) * data[31] + (-18) * data[33] + (-14) * data[35] + (39) * data[37] + (-44) * data[39] + (26) * data[41] + (6) * data[43] + (-34) * data[45] + (45) * data[47] + (-33) * data[49] + (3) * data[51] + (28) * data[53] + (-45) * data[55] + (38) * data[57] + (-12) * data[59] + (-20) * data[61] + (42) * data[63];
			O[25] = (14) * data[1] + (-37) * data[3] + (45) * data[5] + (-36) * data[7] + (12) * data[9] + (16) * data[11] + (-38) * data[13] + (45) * data[15] + (-34) * data[17] + (10) * data[19] + (18) * data[21] + (-39) * data[23] + (45) * data[25] + (-33) * data[27] + (8) * data[29] + (20) * data[31] + (-40) * data[33] + (45) * data[35] + (-31) * data[37] + (6) * data[39] + (22) * data[41] + (-41) * data[43] + (44) * data[45] + (-30) * data[47] + (3) * data[49] + (24) * data[51] + (-42) * data[53] + (44) * data[55] + (-28) * data[57] + (1) * data[59] + (26) * data[61] + (-43) * data[63];
			O[26] = (12) * data[1] + (-33) * data[3] + (44) * data[5] + (-43) * data[7] + (30) * data[9] + (-8) * data[11] + (-16) * data[13] + (36) * data[15] + (-45) * data[17] + (41) * data[19] + (-26) * data[21] + (3) * data[23] + (20) * data[25] + (-38) * data[27] + (45) * data[29] + (-39) * data[31] + (22) * data[33] + (1) * data[35] + (-24) * data[37] + (40) * data[39] + (-45) * data[41] + (37) * data[43] + (-18) * data[45] + (-6) * data[47] + (28) * data[49] + (-42) * data[51] + (45) * data[53] + (-34) * data[55] + (14) * data[57] + (10) * data[59] + (-31) * data[61] + (44) * data[63];
			O[27] = (10) * data[1] + (-28) * data[3] + (40) * data[5] + (-45) * data[7] + (41) * data[9] + (-30) * data[11] + (12) * data[13] + (8) * data[15] + (-26) * data[17] + (39) * data[19] + (-45) * data[21] + (42) * data[23] + (-31) * data[25] + (14) * data[27] + (6) * data[29] + (-24) * data[31] + (38) * data[33] + (-45) * data[35] + (43) * data[37] + (-33) * data[39] + (16) * data[41] + (3) * data[43] + (-22) * data[45] + (37) * data[47] + (-45) * data[49] + (44) * data[51] + (-34) * data[53] + (18) * data[55] + (1) * data[57] + (-20) * data[59] + (36) * data[61] + (-44) * data[63];
			O[28] = (8) * data[1] + (-22) * data[3] + (34) * data[5] + (-42) * data[7] + (45) * data[9] + (-43) * data[11] + (36) * data[13] + (-24) * data[15] + (10) * data[17] + (6) * data[19] + (-20) * data[21] + (33) * data[23] + (-41) * data[25] + (45) * data[27] + (-44) * data[29] + (37) * data[31] + (-26) * data[33] + (12) * data[35] + (3) * data[37] + (-18) * data[39] + (31) * data[41] + (-40) * data[43] + (45) * data[45] + (-44) * data[47] + (38) * data[49] + (-28) * data[51] + (14) * data[53] + (1) * data[55] + (-16) * data[57] + (30) * data[59] + (-39) * data[61] + (45) * data[63];
			O[29] = (6) * data[1] + (-16) * data[3] + (26) * data[5] + (-34) * data[7] + (40) * data[9] + (-44) * data[11] + (45) * data[13] + (-44) * data[15] + (39) * data[17] + (-33) * data[19] + (24) * data[21] + (-14) * data[23] + (3) * data[25] + (8) * data[27] + (-18) * data[29] + (28) * data[31] + (-36) * data[33] + (41) * data[35] + (-45) * data[37] + (45) * data[39] + (-43) * data[41] + (38) * data[43] + (-31) * data[45] + (22) * data[47] + (-12) * data[49] + (1) * data[51] + (10) * data[53] + (-20) * data[55] + (30) * data[57] + (-37) * data[59] + (42) * data[61] + (-45) * data[63];
			O[30] = (3) * data[1] + (-10) * data[3] + (16) * data[5] + (-22) * data[7] + (28) * data[9] + (-33) * data[11] + (37) * data[13] + (-40) * data[15] + (43) * data[17] + (-45) * data[19] + (45) * data[21] + (-45) * data[23] + (44) * data[25] + (-41) * data[27] + (38) * data[29] + (-34) * data[31] + (30) * data[33] + (-24) * data[35] + (18) * data[37] + (-12) * data[39] + (6) * data[41] + (1) * data[43] + (-8) * data[45] + (14) * data[47] + (-20) * data[49] + (26) * data[51] + (-31) * data[53] + (36) * data[55] + (-39) * data[57] + (42) * data[59] + (-44) * data[61] + (45) * data[63];
			O[31] = (1) * data[1] + (-3) * data[3] + (6) * data[5] + (-8) * data[7] + (10) * data[9] + (-12) * data[11] + (14) * data[13] + (-16) * data[15] + (18) * data[17] + (-20) * data[19] + (22) * data[21] + (-24) * data[23] + (26) * data[25] + (-28) * data[27] + (30) * data[29] + (-31) * data[31] + (33) * data[33] + (-34) * data[35] + (36) * data[37] + (-37) * data[39] + (38) * data[41] + (-39) * data[43] + (40) * data[45] + (-41) * data[47] + (42) * data[49] + (-43) * data[51] + (44) * data[53] + (-44) * data[55] + (45) * data[57] + (-45) * data[59] + (45) * data[61] + (-45) * data[63];

			EO[0] = (45) * data[2] + (45) * data[6] + (44) * data[10] + (43) * data[14] + (41) * data[18] + (39) * data[22] + (36) * data[26] + (34) * data[30] + (30) * data[34] + (27) * data[38] + (23) * data[42] + (19) * data[46] + (15) * data[50] + (11) * data[54] + (7) * data[58] + (2) * data[62];
			EO[1] = (45) * data[2] + (41) * data[6] + (34) * data[10] + (23) * data[14] + (11) * data[18] + (-2) * data[22] + (-15) * data[26] + (-27) * data[30] + (-36) * data[34] + (-43) * data[38] + (-45) * data[42] + (-44) * data[46] + (-39) * data[50] + (-30) * data[54] + (-19) * data[58] + (-7) * data[62];
			EO[2] = (44) * data[2] + (34) * data[6] + (15) * data[10] + (-7) * data[14] + (-27) * data[18] + (-41) * data[22] + (-45) * data[26] + (-39) * data[30] + (-23) * data[34] + (-2) * data[38] + (19) * data[42] + (36) * data[46] + (45) * data[50] + (43) * data[54] + (30) * data[58] + (11) * data[62];
			EO[3] = (43) * data[2] + (23) * data[6] + (-7) * data[10] + (-34) * data[14] + (-45) * data[18] + (-36) * data[22] + (-11) * data[26] + (19) * data[30] + (41) * data[34] + (44) * data[38] + (27) * data[42] + (-2) * data[46] + (-30) * data[50] + (-45) * data[54] + (-39) * data[58] + (-15) * data[62];
			EO[4] = (41) * data[2] + (11) * data[6] + (-27) * data[10] + (-45) * data[14] + (-30) * data[18] + (7) * data[22] + (39) * data[26] + (43) * data[30] + (15) * data[34] + (-23) * data[38] + (-45) * data[42] + (-34) * data[46] + (2) * data[50] + (36) * data[54] + (44) * data[58] + (19) * data[62];
			EO[5] = (39) * data[2] + (-2) * data[6] + (-41) * data[10] + (-36) * data[14] + (7) * data[18] + (43) * data[22] + (34) * data[26] + (-11) * data[30] + (-44) * data[34] + (-30) * data[38] + (15) * data[42] + (45) * data[46] + (27) * data[50] + (-19) * data[54] + (-45) * data[58] + (-23) * data[62];
			EO[6] = (36) * data[2] + (-15) * data[6] + (-45) * data[10] + (-11) * data[14] + (39) * data[18] + (34) * data[22] + (-19) * data[26] + (-45) * data[30] + (-7) * data[34] + (41) * data[38] + (30) * data[42] + (-23) * data[46] + (-44) * data[50] + (-2) * data[54] + (43) * data[58] + (27) * data[62];
			EO[7] = (34) * data[2] + (-27) * data[6] + (-39) * data[10] + (19) * data[14] + (43) * data[18] + (-11) * data[22] + (-45) * data[26] + (2) * data[30] + (45) * data[34] + (7) * data[38] + (-44) * data[42] + (-15) * data[46] + (41) * data[50] + (23) * data[54] + (-36) * data[58] + (-30) * data[62];
			EO[8] = (30) * data[2] + (-36) * data[6] + (-23) * data[10] + (41) * data[14] + (15) * data[18] + (-44) * data[22] + (-7) * data[26] + (45) * data[30] + (-2) * data[34] + (-45) * data[38] + (11) * data[42] + (43) * data[46] + (-19) * data[50] + (-39) * data[54] + (27) * data[58] + (34) * data[62];
			EO[9] = (27) * data[2] + (-43) * data[6] + (-2) * data[10] + (44) * data[14] + (-23) * data[18] + (-30) * data[22] + (41) * data[26] + (7) * data[30] + (-45) * data[34] + (19) * data[38] + (34) * data[42] + (-39) * data[46] + (-11) * data[50] + (45) * data[54] + (-15) * data[58] + (-36) * data[62];
			EO[10] = (23) * data[2] + (-45) * data[6] + (19) * data[10] + (27) * data[14] + (-45) * data[18] + (15) * data[22] + (30) * data[26] + (-44) * data[30] + (11) * data[34] + (34) * data[38] + (-43) * data[42] + (7) * data[46] + (36) * data[50] + (-41) * data[54] + (2) * data[58] + (39) * data[62];
			EO[11] = (19) * data[2] + (-44) * data[6] + (36) * data[10] + (-2) * data[14] + (-34) * data[18] + (45) * data[22] + (-23) * data[26] + (-15) * data[30] + (43) * data[34] + (-39) * data[38] + (7) * data[42] + (30) * data[46] + (-45) * data[50] + (27) * data[54] + (11) * data[58] + (-41) * data[62];
			EO[12] = (15) * data[2] + (-39) * data[6] + (45) * data[10] + (-30) * data[14] + (2) * data[18] + (27) * data[22] + (-44) * data[26] + (41) * data[30] + (-19) * data[34] + (-11) * data[38] + (36) * data[42] + (-45) * data[46] + (34) * data[50] + (-7) * data[54] + (-23) * data[58] + (43) * data[62];
			EO[13] = (11) * data[2] + (-30) * data[6] + (43) * data[10] + (-45) * data[14] + (36) * data[18] + (-19) * data[22] + (-2) * data[26] + (23) * data[30] + (-39) * data[34] + (45) * data[38] + (-41) * data[42] + (27) * data[46] + (-7) * data[50] + (-15) * data[54] + (34) * data[58] + (-44) * data[62];
			EO[14] = (7) * data[2] + (-19) * data[6] + (30) * data[10] + (-39) * data[14] + (44) * data[18] + (-45) * data[22] + (43) * data[26] + (-36) * data[30] + (27) * data[34] + (-15) * data[38] + (2) * data[42] + (11) * data[46] + (-23) * data[50] + (34) * data[54] + (-41) * data[58] + (45) * data[62];
			EO[15] = (2) * data[2] + (-7) * data[6] + (11) * data[10] + (-15) * data[14] + (19) * data[18] + (-23) * data[22] + (27) * data[26] + (-30) * data[30] + (34) * data[34] + (-36) * data[38] + (39) * data[42] + (-41) * data[46] + (43) * data[50] + (-44) * data[54] + (45) * data[58] + (-45) * data[62];

			EEO[0] = (45) * data[4] + (43) * data[12] + (40) * data[20] + (35) * data[28] + (29) * data[36] + (21) * data[44] + (13) * data[52] + (4) * data[60];
			EEO[1] = (43) * data[4] + (29) * data[12] + (4) * data[20] + (-21) * data[28] + (-40) * data[36] + (-45) * data[44] + (-35) * data[52] + (-13) * data[60];
			EEO[2] = (40) * data[4] + (4) * data[12] + (-35) * data[20] + (-43) * data[28] + (-13) * data[36] + (29) * data[44] + (45) * data[52] + (21) * data[60];
			EEO[3] = (35) * data[4] + (-21) * data[12] + (-43) * data[20] + (4) * data[28] + (45) * data[36] + (13) * data[44] + (-40) * data[52] + (-29) * data[60];
			EEO[4] = (29) * data[4] + (-40) * data[12] + (-13) * data[20] + (45) * data[28] + (-4) * data[36] + (-43) * data[44] + (21) * data[52] + (35) * data[60];
			EEO[5] = (21) * data[4] + (-45) * data[12] + (29) * data[20] + (13) * data[28] + (-43) * data[36] + (35) * data[44] + (4) * data[52] + (-40) * data[60];
			EEO[6] = (13) * data[4] + (-35) * data[12] + (45) * data[20] + (-40) * data[28] + (21) * data[36] + (4) * data[44] + (-29) * data[52] + (43) * data[60];
			EEO[7] = (4) * data[4] + (-13) * data[12] + (21) * data[20] + (-29) * data[28] + (35) * data[36] + (-40) * data[44] + (43) * data[52] + (-45) * data[60];

			EEEO[0] = (44) * data[8] + (38) * data[24] + (25) * data[40] + (9) * data[56];
			EEEO[1] = (38) * data[8] + (-9) * data[24] + (-44) * data[40] + (-25) * data[56];
			EEEO[2] = (25) * data[8] + (-44) * data[24] + (9) * data[40] + (38) * data[56];
			EEEO[3] = (9) * data[8] + (-25) * data[24] + (38) * data[40] + (-44) * data[56];

			EEEEO[0] = (42) * data[16] + (17) * data[48];
			EEEEO[1] = (17) * data[16] + (-42) * data[48];
			EEEEE[0] = (32) * data[0] + (32) * data[32];
			EEEEE[1] = (32) * data[0] + (-32) * data[32];

			EEEE[0] = EEEEE[0] + EEEEO[0];
			EEEE[3] = EEEEE[0] - EEEEO[0];
			EEEE[1] = EEEEE[1] + EEEEO[1];
			EEEE[2] = EEEEE[1] - EEEEO[1];

			for (k = 0; k < 4; k++) {
//#pragma HLS UNROLL
				EEE[k] = EEEE[k] + EEEO[k];
				EEE[k + 4] = EEEE[3 - k] - EEEO[3 - k];
			}
			for (k = 0; k < 8; k++) {
//#pragma HLS UNROLL
				EE[k] = EEE[k] + EEO[k];
				EE[k + 8] = EEE[7 - k] - EEO[7 - k];
			}
			for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
				E[k] = EE[k] + EO[k];
				E[k + 16] = EE[15 - k] - EO[15 - k];
			}
			for (k = 0; k < 32; k++) {
//#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, (E[k] + O[k] + rnd_factor) >> shift);
				block[i][k + 32] = Clip3(-(1 << 15), (1 << 15) - 1, (E[31 - k] - O[31 - k] + rnd_factor) >> shift);
			}
		}
	}
}
/*  sub func  */
void recon_64(PART_SIZE part, s16 resi[64][64], pel pred[64][64], pel pred_for_update[64][64], int is_coef, U3 plane, U7 cu_width, U7 cu_height, int s_rec, pel rec[64][64], pel rec_for_update[64][64], U4 bit_depth)
{

//#pragma HLS INLINE off
	int i;
	int j;
	//get_part_num(part);

	pel p;

	/* + tb_y * s_rec + tb_x*/
	/* just copy pred to rec */
	U1 is_coef_not_zero = is_coef != 0;
#pragma HLS ARRAY_PARTITION variable=pred_for_update cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=rec_for_update cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=pred cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=rec cyclic factor=16 dim=2

	for (i = 0; i < 64; i++) {
#pragma HLS PIPELINE II=4
		for (j = 0; j < 64; j++) {
			if (i < cu_height && j < cu_width) {
				//#pragma HLS LOOP_TRIPCOUNT max=64
				p = pred[i][j];
				if (is_coef_not_zero)
					p += resi[i][j];
				rec[i][j] = (((0 > (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)) ? 0 : (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)))));
				pred_for_update[i][j] = pred[i][j];
				rec_for_update[i][j] = rec[i][j];
			}
		}
		//r += s_rec;
	}
}

void itrans_h_64(U7 w, U7 h, s16 resi[64][64], s16 coef_temp[64][64], U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth)
{
	U4 shift2;
	s16 block1[64][64];
	U7 size = 64;
	U8 size_b = size - 1;

	{
		shift2 = com_get_inverse_trans_shift(cu_height_log2, 1, bit_depth);


//		if (w == 32 || w == 64) {
//			for (int i = 0; i < w; i++) {
//#pragma HLS PIPELINE II=4
//#pragma HLS LOOP_TRIPCOUNT max = 64
//				for (int j = 0; j < 64; j++) {
//					if (j < h) {
//						block1[(i + j) & size_b][j] = coef_temp[i][j];
//					}
//				}
//			}
//		}
//		else {
//			for (int i = 0; i < w; i++) {
//#pragma HLS PIPELINE II=4
//#pragma HLS LOOP_TRIPCOUNT max = 64
//				for (int j = 0; j < 64; j++) {
//					if (j < h) {
//						block1[i][j] = coef_temp[i][j];
//					}
//				}
//			}
//		}
//
//		inv_dct_row_64(w, h, block1, resi, shift2);
		inv_dct_row_64(w, h, coef_temp, resi, shift2);
	}
}


void itrans_v_64(U7 w, U7 h, s16 coef_dq[64][64], s16 coef_temp[64][64], U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth)
{

//#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=coef_temp cyclic factor=16 dim=2
	U4 shift1 = com_get_inverse_trans_shift(cu_width_log2, 0, bit_depth);
	s16 block1[64][64];
	U7 size = 64;
	U8 size_b = size - 1;


	inv_dct_col_64(w, h, coef_dq, block1, shift1);

	if (h == 32 || h == 64) {
		for (int i = 0; i < w; i++)
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
			for (int j = 0; j < 64; j++) {
//#pragma HLS LOOP_TRIPCOUNT max = 64
				if (j < h) {
					coef_temp[i][j] = block1[(i + j) & size_b][j];
				}
			}
	}
	else {
		for (int i = 0; i < w; i++)
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
			for (int j = 0; j < 64; j++) {
//#pragma HLS LOOP_TRIPCOUNT max = 64
				if (j < h) {
					coef_temp[i][j] = block1[i][j];
				}
			}
	}
}

void dquant_64(U7 w, U7 h, U6 qp, s16 coef[64][64], s16 coef_out[64][64], U3 log2_w, U3 log2_h, U4 bit_depth, int* num_nz_for_ith)
{

//#pragma HLS INLINE off
	int i;
	int j;
	int log2_size = (log2_w + log2_h) >> 1;
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
		10, 9, 9, 9, 9, 9, 9, 9,  //11, 10, 10, 10, 10, 10, 10, 10,
		9, 8, 8, 8, 8, 8, 8, 8,  //10,  9,  9,  9,  9,  9,  9,  9,
		7, 7, 7, 7, 7, 7, 7, 7,  // 8,  8,  8,  8,  8,  8,  8,  8,
		6, 6, 6, 6, 6, 6, 6, 6,  // 7,  7,  7,  7,  7,  7,  7,  7,
		5, 5, 5, 5, 5, 5, 5, 5,  // 6,  6,  6,  6,  6,  6,  6,  6
	};
	int scale = com_tbl_dq_scale[qp];
	// +1 is used to compensate for the mismatching of shifts in quantization and inverse quantization
	int shift = com_tbl_dq_shift[qp] - get_transform_shift(bit_depth, log2_size) + 1;
	int offset = shift == 0 ? 0 : 1 << (shift - 1);
	int sr_x = 0;
	int sr_y = 0;
#pragma HLS ARRAY_PARTITION variable=coef cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=coef_out cyclic factor=16 dim=1
	for (i = 0; i < w; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < 64; j++) {
			if (j < h) {
			if (coef[j][i]) {
				sr_y = j > sr_y ? j : sr_y;
				sr_x = i > sr_x ? i : sr_x;
			}
			//int weight = (i | j) & 0xE0 ? 0 : 64;
			//int lev = (((coef[i][j] * weight >> 2) * scale >> 4) + offset) >> shift;
				int weight = (i | j) & 0xE0 ? 0 : coef[j][i];
			int lev = (weight * scale + offset) >> shift;
			lev = (lev > 32767 ? 32767 : ((lev < -32768 ? -32768 : lev)));
			if (refix) {
				lev = (lev * 181 + 128) >> 8;
			}
				coef_out[j][i] = (lev);
			}
		}
	}
	*num_nz_for_ith = (sr_y << 20) + (sr_x << 12) + *num_nz_for_ith;
}

/*  top func  */
u8 transform_hor_64(U7 w, U7 h, U4 bit_depth, s16 resi[64][64], s16 coef_tmp[64][64])
{
	//xingxing pay attention to it

	//cu_plane_nz_cln_64(num_nz, type);
	U3 tb_width_log2 = com_tbl_log2[w];
	U3 tb_height_log2 = com_tbl_log2[h];

	U7 size = 64;

	U4 shift1;
	//U4 shift2;

	shift1 = com_get_forward_trans_shift(tb_width_log2, 0, bit_depth);
	//shift2 = com_get_forward_trans_shift(tb_height_log2, 1, bit_depth);
	dct_row_64(w, h, resi, coef_tmp, shift1);

	return 1;
}
/*  top func  */
u8 transform_ver_64(U7 w, U7 h, U4 bit_depth, s16 coef_tmp[64][64], s16 coef[64][64])
{
	U3 tb_width_log2 = com_tbl_log2[w];
	U3 tb_height_log2 = com_tbl_log2[h];

	//U4 shift1;
	U4	shift2;
	U7 size = 64;

	//shift1 = com_get_forward_trans_shift(tb_width_log2, 0, bit_depth);
	shift2 = com_get_forward_trans_shift(tb_height_log2, 1, bit_depth);
	dct_col_64(w, h, coef_tmp, coef, shift2);

	return 1;
}
void inv_transform_ver_64(U8 w, U8 h, U4 bit_depth, U6 qp_y, int num_nz_for_itv, pel qcoef_for_dq[64][64], int* num_nz_for_ith, int* num_nz_for_up, pel itcoef_v[64][64])
{
	U3 tb_width_log2 = com_tbl_log2[w];
	U3 tb_height_log2 = com_tbl_log2[h];
	s16 coef_dq[64][64];
	int num_nz_read = num_nz_for_itv;
	dquant_64(w, h, qp_y, qcoef_for_dq, coef_dq, tb_width_log2, tb_height_log2, bit_depth,&num_nz_read);
	itrans_v_64(w, h, coef_dq, itcoef_v, tb_width_log2, tb_height_log2, bit_depth);

	*num_nz_for_ith = num_nz_read;
	*num_nz_for_up = num_nz_read;
}

void inv_transform_hor_64(U8 w, U8 h, U4 bit_depth, int tb_part, U3 cu_width_log2, U3 cu_height_log2, U3 type,
	int num_nz_for_ith,
	pel orgY_for_ssd[64 * 64], pel pred_for_rec[64][64], pel itcoef_v[64][64],
	s32* dist,
	pel pred_for_update[64][64], pel rec_for_update[64][64])
{
	s16 resi[64][64];
	int s_org = 64;

	//pel* resi_tb = resi + coef_offset_tb;
	pel rec[64][64];
	s32 cost = 0;

#pragma HLS ARRAY_PARTITION variable=orgY_for_ssd cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=pred_for_rec cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=pred_for_update cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=rec_for_update cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=rec cyclic factor=16 dim=2
	itrans_h_64(w, h, resi, itcoef_v, cu_width_log2, cu_height_log2, bit_depth);

	recon_64((PART_SIZE)(type == 0 ? tb_part : (SIZE_2Nx2N)), resi, pred_for_rec, pred_for_update,
		num_nz_for_ith, type, w, h, w, rec, rec_for_update, bit_depth);
	*dist = 0;
	if (!type)
	{
		cost += ((enc_ssd_16b_1_64(cu_width_log2, cu_height_log2, rec, orgY_for_ssd, w, s_org, bit_depth)));
		*dist = cost;
		//printf("cost=%d\n", *dist);
	}
	//memcpy_2d<pel, 64, 64>(pred_for_rec, pred_for_update);
	//memcpy_2d<pel, 64, 64>(rec, rec_for_update);
}

int quant_64(U7 w, U7 h, U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth, s16 coef[64][64], s16 coef_out[64][64], s16 coef_out1[64][64], s16 coef_out2[64][64], U2 slice_type, U14 scale)
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
//#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < w; j++)
		{
//#pragma HLS LOOP_TRIPCOUNT max=64
			sign = COM_SIGN_GET(coef[i][j]);
			lev = ((int)(COM_ABS(coef[i][j]) * scale_x_ns_scale + offset) >> shift);
			coef_out[i][j] = COM_SIGN_SET(lev, sign);
			coef_out1[i][j] = COM_SIGN_SET(lev, sign);
			coef_out2[i][j] = COM_SIGN_SET(lev, sign);
			//bug fix for rate estimation
			//if ((i + j) >= w)
			//{
			//	coef[i][j] = 0;
			//}

			num_nz_coef += !!(coef_out[i][j]);
		}
	}
	return num_nz_coef;
}

void level_core_64(U7 w, U7 h, u32 level[24], u32 level1[24], U5 t0[64], U7 i, int max_num)
{
//#pragma HLS ARRAY_PARTITION variable=level1 complete dim=1
//#pragma HLS FUNCTION_INSTANTIATE variable=i
//#pragma HLS ARRAY_PARTITION variable=level complete dim=1
	int tmp = 0;
	int pos = 0;
	int UPDATE_SIZE = COM_MIN(w, h);
//#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
//#pragma HLS UNROLL
		if (pos < max_num && pos < UPDATE_SIZE)
		{
			tmp += !(t0[pos] - i);
		}
	}
	level[i] += tmp;
	level1[i + 1] += 8 * tmp;
}
void run_core_64(U7 w, U7 h, u32 run[24], u32 run1[24], U5 t0[64], U7 i, U1 mat_run[64], int max_num)
{
//#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
//#pragma HLS ARRAY_PARTITION variable=run1 complete dim=1
//#pragma HLS FUNCTION_INSTANTIATE variable=i
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1
	int tmp = 0;
	int tmp2 = 0;
	int pos = 0;
	int UPDATE_SIZE = COM_MIN(w, h);
//#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
//#pragma HLS UNROLL
		if (pos < max_num && pos < UPDATE_SIZE)
		{
			tmp += !(t0[pos] - i);
			tmp2 += mat_run[pos];
		}
	}
	run[i] += tmp;
	run1[i + 1] += tmp2;
}
void last1_core_64(U7 w, U7 h, u32 last1[12], U4 t0[64], U7 i, int max_num)
{
//#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
//#pragma HLS FUNCTION_INSTANTIATE variable=i
	int tmp = 0;
	int pos = 0;
	int UPDATE_SIZE = COM_MIN(w, h);
//#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
//#pragma HLS UNROLL
		if (pos < max_num && pos < UPDATE_SIZE)
		{
			tmp += !(t0[pos] - i);
		}
	}
	last1[i] += tmp;
}
void last2_core_64(U7 w, U7 h, u32 last2[22], U5 t0[64], U7 i, int max_num)
{
//#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
//#pragma HLS FUNCTION_INSTANTIATE variable=i
	int tmp = 0;
	int pos = 0;
	int UPDATE_SIZE = COM_MIN(w, h);
//#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
//#pragma HLS UNROLL
		if (pos < max_num && pos < UPDATE_SIZE)
		{
			tmp += !(t0[pos] - i);
		}
	}
	last2[i] += tmp;
}
void model_update_64(int base_idx, U7 w, U7 h, u32 last1[12], u32 last2[22], u32 run[24], u32 level[24], U1 mat_run[64], U24 mat_prev_level[64], U2 ch_type, int max_num)
{
//#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
//#pragma HLS PIPELINE II=1
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1
//#pragma HLS ARRAY_PARTITION variable=level complete dim=1
//#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=1
//#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
	int UPDATE_SIZE = COM_MIN(w, h);
	U7 scan_pos = 0;
	U5 t0[64] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	U4 last1_pos[64] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=last1_pos complete dim=1
	U5 last2_pos[64] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=last2_pos complete dim=1
	u32 last1_tmp[12] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=last1_tmp complete dim=1
	u32 last2_tmp[22] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=last2_tmp complete dim=1
	u32 level_tmp[24] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=level_tmp complete dim=1
	u32 level_tmp1[24] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=level_tmp1 complete dim=1
	u32 run_tmp[24] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=run_tmp complete dim=1
	u32 run_tmp1[24] = { 0 };
//#pragma HLS ARRAY_PARTITION variable=run_tmp1 complete dim=1
	int ace_log2 = 0;

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos >= UPDATE_SIZE)
			break;

		if (scan_pos < max_num)
		{
			if (scan_pos + 1 + base_idx < 2) {
				ace_log2 = 0;
			}
			else if (scan_pos + 1 + base_idx < 4) {
				ace_log2 = 1;
			}
			else if (scan_pos + 1 + base_idx < 8) {
				ace_log2 = 2;
			}
			else if (scan_pos + 1 + base_idx < 16) {
				ace_log2 = 3;
			}
			else if (scan_pos + 1 + base_idx < 32) {
				ace_log2 = 4;
			}
			else if (scan_pos + 1 + base_idx < 64) {
				ace_log2 = 5;
			}
			else if (scan_pos + 1 + base_idx < 128) {
				ace_log2 = 6;
			}
			else if (scan_pos + 1 + base_idx < 256) {
				ace_log2 = 7;
			}
			else if (scan_pos + 1 + base_idx < 512) {
				ace_log2 = 8;
			}
			else if (scan_pos + 1 + base_idx < 1024) {
				ace_log2 = 9;
			}
			else if (scan_pos + 1 + base_idx < 2048) {
				ace_log2 = 10;
			}
			else if (scan_pos + 1 + base_idx < 4096) {
				ace_log2 = 11;
			}
			else {
				ace_log2 = 11;
			}

			t0[scan_pos] = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) * 2) + (ch_type == 0 ? 0 : 12);
			last1_pos[scan_pos] = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5))) + (ch_type == 0 ? 0 : 6);
			last2_pos[scan_pos] = ace_log2 + (ch_type == 0 ? 0 : 12);
		}
	}

	for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
	{
//#pragma HLS UNROLL
//#pragma HLS LOOP_TRIPCOUNT max=12
		level_core_64(w, h, level_tmp, level_tmp1, t0, scan_pos, max_num);
		run_core_64(w, h, run_tmp, run_tmp1, t0, scan_pos, mat_run, max_num);
	}

	for (scan_pos = 0; scan_pos < 12; scan_pos++)
	{
//#pragma HLS UNROLL
		last1_core_64(w, h, last1_tmp, last1_pos, scan_pos, max_num);
		last1[scan_pos] += last1_tmp[scan_pos];
	}

	for (scan_pos = 0; scan_pos < 22; scan_pos++)
	{
//#pragma HLS UNROLL
		last2_core_64(w, h, last2_tmp, last2_pos, scan_pos, max_num);
		last2[scan_pos] += last2_tmp[scan_pos];
	}

	for (scan_pos = 0; scan_pos < 24; scan_pos++)
	{
//#pragma HLS UNROLL
		level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
		run[scan_pos] += run_tmp[scan_pos] + run_tmp1[scan_pos];
	}
}

void rdoq_data_prepare_64(U7 w, U7 h, U2 ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], U1 is_intra)
{
//#pragma HLS INLINE

	int scan_pos;
	int loop_num = COM_MIN((U7)cnt, h);
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS UNROLL
		if (scan_pos < loop_num)
		{
			if (cnt & 0x01)
				src_coef[scan_pos] = src_tmp[loop_num - scan_pos - 1];
			else
				src_coef[scan_pos] = src_tmp[scan_pos];
		}

	}
}

void out_coef_64(U7 w, U7 h, int cnt, s16 dst_coef[64], s16 dst_tmp[64])
{
	int scan_pos;
	int MAT_SIZE = COM_MIN(w, h);
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos < MAT_SIZE)
		{
			if (scan_pos < cnt && cnt < MAT_SIZE)
			{
				if (cnt & 1)
				{
					dst_tmp[scan_pos] = dst_coef[cnt - scan_pos - 1];
				}
				else
				{
					dst_tmp[scan_pos] = dst_coef[scan_pos];
				}
			}
			else if (scan_pos < cnt && cnt >= MAT_SIZE)
			{
				if (cnt & 1)
				{
					dst_tmp[scan_pos] = dst_coef[MAT_SIZE - scan_pos - 1];
				}
				else
				{
					dst_tmp[scan_pos] = dst_coef[scan_pos];
				}
			}
		}

	}
}
int clean_nz_64(U7 w, U7 h, s16 dst_tmp[64][64], int x_pos, int y_pos, int pzb)
{
	int i;
	int j;
	static int mask[64][64] = { 0 };
//#pragma HLS ARRAY_PARTITION variable = mask complete dim = 0
	int num_nz_coef = 0;
	for (i = 0; i < 64; i++)
	{
//#pragma HLS UNROLL
		if (i >= h)
			break;

		for (j = 0; j < 64; j++)
		{
//#pragma HLS UNROLL
			if (j >= w)
				break;

			if (!pzb)
				mask[i][j] = 0;
			else
			{
				if ((i + j) > x_pos)
				{
					mask[i][j] = 0;
				}
				else if ((i + j) == x_pos)
				{
					if (x_pos & 1)
					{
						if (i > y_pos)
						{
							mask[i][j] = 0;
						}
						else
						{
							mask[i][j] = 1;
						}
					}
					else
					{
						if (i < y_pos)
						{
							mask[i][j] = 0;
						}
						else
						{
							mask[i][j] = 1;
						}
					}
				}
				else
				{
					mask[i][j] = 1;
				}
			}
			dst_tmp[i][j] = dst_tmp[i][j] * mask[i][j];
			num_nz_coef += !!(dst_tmp[i][j]);
		}
	}
	return num_nz_coef;
}
void read_data_64(U7 w, U7 h, s16 coef64[64], s16 src_coef[64][64], int i)
{
	int j;
	int loop_num = COM_MIN((U7)i, (U7)(h - 1));
	if (i == 0)
	{
		coef64[0] = src_coef[0][i];
	}
	else
	{
		for (j = 0; j < 64; j++)
		{
//#pragma HLS UNROLL
			if (j <= loop_num)
				coef64[j] = src_coef[j][i - j];
		}
	}
}
void write_data_64(U7 w, U7 h, s16 dst_tmp[64][64], s16 dst64[64], int i)
{
	int j;
	int loop_num = COM_MIN((U7)i, (U7)(h - 1));
	if (i == 0)
	{
		dst_tmp[0][i] = dst64[0];
	}
	else
	{
		for (j = 0; j < 64; j++)
		{
//#pragma HLS UNROLL
			if (j <= loop_num)
				dst_tmp[j][i - j] = dst64[j];
		}
	}
}
#define GET_I_COST(rate, lamba)  (rate*lamba)
void rdoq_core_64(U7 w, U7 h, RDOQ_MODEL* model_cnt, U24 last_level[64 + 1], U24 prev_level[64 + 1], int* pzb, s64 base_cost, int* x_pos, int* y_pos, s64 best_cost_out[64 + 1], s64 last_cost[64 + 1], int q_value, int cnt, int q_bits, s64 err_scale, s64 lambda, U1 is_intra, s16 coef64[64], s16 dst_coef[64], U2 ch_type, RDOQ_ARRAY* rdoq_array)
{
//#pragma HLS PIPELINE II=1
	int MAT_SIZE = COM_MIN(w, h);
	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	int num_nz_coef = 0;
	int cyc = cnt - 1;
	int scan_pos;
	static s64 d64_base_cost[64] = { 0 };
	s64 d64_coded_cost = 0;
	s64 d64_uncoded_cost = 0;
	static s16 tmp_coef[64] = { 0 };
	static s16 src_coef[64] = { 0 };
	U36 tmp_level_double[64] = { 0 };
	s64 best_cost = best_cost_out[cyc];
//#pragma HLS ARRAY_PARTITION variable = d64_base_cost complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = tmp_coef complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = tmp_level_double complete dim = 0

	int last_idx = (((1 + cnt) * cnt) >> 1) - 1;
	int base_idx = ((1 + cyc) * cyc >> 1);

	rdoq_data_prepare_64(w, h, ch_type, lambda, cnt, coef64, src_coef, is_intra);

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos < MAT_SIZE)
		{
			U36 level_double = src_coef[scan_pos];
			U24 max_abs_level;
			U1 lower_int;
			U36 temp_level;
			temp_level = ((s64)COM_ABS(src_coef[scan_pos]) * (s64)q_value);
			level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - ((s64)1 << (q_bits - 1)));
			tmp_level_double[scan_pos] = level_double;
			max_abs_level = (u32)(level_double >> q_bits);
			lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < ((s64)1 << (q_bits - 1))) ? 1 : 0;
			if (!lower_int)
			{
				max_abs_level++;
			}
			tmp_coef[scan_pos] = src_coef[scan_pos] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
		}
	}

	static U24 mat_prev_level[64] = { 0 };
	U1 mat_run[64] = { 0 };
//#pragma HLS ARRAY_PARTITION variable = mat_prev_level complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = mat_run complete dim = 0

	if (last_level[cyc])
	{
		mat_prev_level[0] = last_level[cyc];
		mat_run[0] = 0;
	}
	else
	{
		mat_prev_level[0] = prev_level[cyc];
		mat_run[0] = 1;
	}

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos < MAT_SIZE)
		{
			if (COM_ABS(tmp_coef[scan_pos]))
			{
				mat_prev_level[scan_pos] = COM_ABS(tmp_coef[scan_pos]);
				mat_run[scan_pos] = 0;
			}
			else
			{
				mat_prev_level[scan_pos] = 1;
				mat_run[scan_pos] = 1;
			}
		}

	}

	static s64 mat_uncoded_cost[64] = { 0 };
	static s64 mat_coded_cost[64] = { 0 };
	static U24 lev_opt[64] = { 0 };
	u16 ctx_run = 0;
//#pragma HLS ARRAY_PARTITION variable = mat_uncoded_cost complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = mat_coded_cost complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = lev_opt complete dim = 0

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos < MAT_SIZE)
		{
			U24 level;
			U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)) << 1) + (ch_type == Y_C ? 0 : 12);

			level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[scan_pos], COM_ABS(tmp_coef[scan_pos]),
				mat_run[scan_pos], ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, 0, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level);
			dst_coef[scan_pos] = (s16)(tmp_coef[scan_pos] < 0 ? -(s16)(level) : (s16)level);

			mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
			mat_coded_cost[scan_pos] = d64_coded_cost;
			lev_opt[scan_pos] = level;
		}
	}

	static s64 d64_cost_last_zero[64] = { 0 };
//#pragma HLS ARRAY_PARTITION variable = d64_cost_last_zero complete dim = 0
	int ace_log2 = 0;
	if (cyc == 0)
	{
		d64_cost_last_zero[0] = 0;
	}
	else
	{
		if ((base_idx) < 2)
		{
			ace_log2 = 0;
		}
		else if ((base_idx) < 4)
		{
			ace_log2 = 1;
		}
		else if ((base_idx) < 8)
		{
			ace_log2 = 2;
		}
		else if ((base_idx) < 16)
		{
			ace_log2 = 3;
		}
		else if ((base_idx) < 32)
		{
			ace_log2 = 4;
		}
		else if ((base_idx) < 64)
		{
			ace_log2 = 5;
		}
		else if ((base_idx) < 128)
		{
			ace_log2 = 6;
		}
		else if ((base_idx) < 256)
		{
			ace_log2 = 7;
		}
		else if ((base_idx) < 512)
		{
			ace_log2 = 8;
		}
		else if ((base_idx) < 1024)
		{
			ace_log2 = 9;
		}
		else if ((base_idx) < 2048)
		{
			ace_log2 = 10;
		}
		else if ((base_idx) < 4096)
		{
			ace_log2 = 11;
		}
		else
		{
			ace_log2 = 11;
		}
		d64_cost_last_zero[0] = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(prev_level[cyc] - 1), (U24)5)][ace_log2][0], lambda);
		//d64_cost_last_zero[0] = d64_cost_last_zero[0] >> 24;

	}

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos >= MAT_SIZE)
			break;

		if ((scan_pos + base_idx) < 2)
		{
			ace_log2 = 0;
		}
		else if ((scan_pos + base_idx) < 4)
		{
			ace_log2 = 1;
		}
		else if ((scan_pos + base_idx) < 8)
		{
			ace_log2 = 2;
		}
		else if ((scan_pos + base_idx) < 16)
		{
			ace_log2 = 3;
		}
		else if ((scan_pos + base_idx) < 32)
		{
			ace_log2 = 4;
		}
		else if ((scan_pos + base_idx) < 64)
		{
			ace_log2 = 5;
		}
		else if ((scan_pos + base_idx) < 128)
		{
			ace_log2 = 6;
		}
		else if ((scan_pos + base_idx) < 256)
		{
			ace_log2 = 7;
		}
		else if ((scan_pos + base_idx) < 512)
		{
			ace_log2 = 8;
		}
		else if ((scan_pos + base_idx) < 1024)
		{
			ace_log2 = 9;
		}
		else if ((scan_pos + base_idx) < 2048)
		{
			ace_log2 = 10;
		}
		else if ((scan_pos + base_idx) < 4096)
		{
			ace_log2 = 11;
		}
		else
		{
			ace_log2 = 11;
		}
		d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)5)][ace_log2][0], lambda);
		//d64_cost_last_zero[scan_pos] = d64_cost_last_zero[scan_pos] >> 24;
	}

	if (cnt != 1)
		d64_base_cost[0] = last_cost[cyc];
	else
		d64_base_cost[0] = base_cost;

	d64_base_cost[0] -= mat_uncoded_cost[0];
	d64_base_cost[0] += mat_coded_cost[0];
	d64_base_cost[0] += d64_cost_last_zero[0] * !!(lev_opt[0]);
	for (scan_pos = 1; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos < MAT_SIZE)
		{
			d64_base_cost[scan_pos] = d64_base_cost[0];
			for (int i = 1; i < 64; i++)
			{
//#pragma HLS UNROLL
				if (i <= scan_pos)
					d64_base_cost[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * !!(lev_opt[i]);
			}
		}

	}

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
//#pragma HLS UNROLL
		if (scan_pos >= MAT_SIZE)
			break;

		if (scan_pos < cnt)
		{
			if ((scan_pos + base_idx + 1) < 2)
			{
				ace_log2 = 0;
			}
			else if ((scan_pos + base_idx + 1) < 4)
			{
				ace_log2 = 1;
			}
			else if ((scan_pos + base_idx + 1) < 8)
			{
				ace_log2 = 2;
			}
			else if ((scan_pos + base_idx + 1) < 16)
			{
				ace_log2 = 3;
			}
			else if ((scan_pos + base_idx + 1) < 32)
			{
				ace_log2 = 4;
			}
			else if ((scan_pos + base_idx + 1) < 64)
			{
				ace_log2 = 5;
			}
			else if ((scan_pos + base_idx + 1) < 128)
			{
				ace_log2 = 6;
			}
			else if ((scan_pos + base_idx + 1) < 256)
			{
				ace_log2 = 7;
			}
			else if ((scan_pos + base_idx + 1) < 512)
			{
				ace_log2 = 8;
			}
			else if ((scan_pos + base_idx + 1) < 1024)
			{
				ace_log2 = 9;
			}
			else if ((scan_pos + base_idx + 1) < 2048)
			{
				ace_log2 = 10;
			}
			else if ((scan_pos + base_idx + 1) < 4096)
			{
				ace_log2 = 11;
			}
			else
			{
				ace_log2 = 11;
			}
			U24 prev_level = mat_prev_level[scan_pos];
			s64 d64_cost_last_one = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(prev_level - 1), (U24)5)][ace_log2][1], lambda);
			//d64_cost_last_one = d64_cost_last_one >> 24;
			if (COM_ABS(dst_coef[scan_pos]))
			{
				s64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

				if (d64_cur_is_last_cost < best_cost)
				{
					*pzb = 1;
					best_cost = d64_cur_is_last_cost;
					if (cnt <= MAT_SIZE)
					{
						if (cnt & 1)
						{
							*y_pos = cnt - scan_pos - 1;
						}
						else
						{
							*y_pos = scan_pos;
						}
					}
					else
					{
						if (cnt & 1)
						{
							*y_pos = MAT_SIZE - scan_pos - 1;
						}
						else
						{
							*y_pos = scan_pos;
						}
					}
					*x_pos = cnt - 1;
				}
			}
		}
	}
	model_update_64(base_idx, w, h, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, cnt);

	if (cnt <= MAT_SIZE)
	{
		last_cost[cyc + 1] = d64_base_cost[cnt - 1];
		prev_level[cyc + 1] = mat_prev_level[cnt - 1];
		last_level[cyc + 1] = COM_ABS(tmp_coef[cnt - 1]);
	}
	else
	{
		last_cost[cyc + 1] = d64_base_cost[MAT_SIZE - 1];
		prev_level[cyc + 1] = mat_prev_level[MAT_SIZE - 1];
		last_level[cyc + 1] = COM_ABS(tmp_coef[MAT_SIZE - 1]);
	}
	best_cost_out[cyc + 1] = best_cost;
}
int rdoq_top_64(U7 w, U7 h, RDOQ_MODEL* model_cnt, U6 qp, s64 d_lambda, U1 is_intra, s16 coef[64][64], s16 coef_out[64][64], U3 cu_width_log2, U3 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{

	static U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
	const U14 scale = quant_scale[qp];
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);
	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	const int q_bits = QUANT_SHIFT + tr_shift;
	int num_nz_coef = 0;
	const s64 lambda = (d_lambda * (1 << 15) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	//const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
	static U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	static s64 last_cost[64 + 1];
	static s64 best_cost[64 + 1];
	int x_pos = 0;
	int y_pos = 0;
	static s16 dst64_tmp[64];
	s64 d64_base_cost0 = 0;
	static s16 tmp_coef[64];
	static s16 dst_coef[64];
	static U24 last_level[64 + 1];
	static U24 prev_level[64 + 1];
	last_level[0] = 6;
	prev_level[0] = 6;
	int pzb = 0;
	last_cost[0] = 0;
//#pragma HLS ARRAY_PARTITION variable = last_cost complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = best_cost complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = dst64_tmp complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = tmp_coef complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = dst_coef complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = last_level complete dim = 0
//#pragma HLS ARRAY_PARTITION variable = prev_level complete dim = 0

	if (!is_intra && ch_type == Y_C)
	{
		d64_base_cost0 = 0;
		best_cost[0] = 0;
	}
	else
	{
		d64_base_cost0 = GET_I_COST(rdoq_array->rdoq_est_cbf[ch_type][1], lambda);
		//d64_base_cost0 = d64_base_cost0 >> 24;
		best_cost[0] = GET_I_COST(rdoq_array->rdoq_est_cbf[ch_type][0], lambda);
		//best_cost[0] = best_cost[0] >> 24;
	}

	for (int cyc = 0; cyc < w; cyc++)
	{
//#pragma HLS DATAFLOW
		read_data_64(w, h, tmp_coef, coef, cyc);
		rdoq_core_64(w, h, model_cnt, last_level, prev_level, &pzb, d64_base_cost0, &x_pos, &y_pos, best_cost, last_cost, q_value, cyc + 1, q_bits, err_scale, lambda, is_intra, tmp_coef, dst64_tmp, ch_type, rdoq_array);
		out_coef_64(w, h, cyc + 1, dst64_tmp, dst_coef);
		write_data_64(w, h, coef_out, dst_coef, cyc);

	}
	num_nz_coef = clean_nz_64(w, h, coef_out, x_pos, y_pos, pzb);
	return num_nz_coef;
}


static U4 get_ace_log2(U32 scan_pos)
{
	U4 ace_log2 = 0;
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
	else if ((scan_pos) < 64)
	{
		ace_log2 = 5;
	}
	else if ((scan_pos) < 128)
	{
		ace_log2 = 6;
	}
	else if ((scan_pos) < 256)
	{
		ace_log2 = 7;
	}
	else if ((scan_pos) < 512)
	{
		ace_log2 = 8;
	}
	else if ((scan_pos) < 1024)
	{
		ace_log2 = 9;
	}
	else if ((scan_pos) < 2048)
	{
		ace_log2 = 10;
	}
	else if ((scan_pos) < 4096)
	{
		ace_log2 = 11;
	}
	else
	{
		ace_log2 = 11;
	}
	return ace_log2;
}


void rdoq_64_64_hardware(s16 src_coef[64][64], U6 qp, S16 tmp_dst_coef_out[4096], u32 last1[12], u32 last2[22],
	u32 run[24], u32 level[24], int* best_last_idx_p1,
	int rdoq_est_cbf[3][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2],
	s32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda, U4 bit_depth, int* final_x, int* final_y)
{
	U3 cu_width_log2 = 6;
	U3 cu_height_log2 = 6;
	static U16 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
	static U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };
	const U14 scale = quant_scale[qp];
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);
	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	const int q_bits = QUANT_SHIFT + tr_shift;
	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	static S16 tmp_dst_coef[64];
	static U36 tmp_level_double[64];
	static S16 tmp_coef[64];
	static S64 d64_cost_last_zero[64] = { 0 };
	//const U20 q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	static U24 lev_opt[64];
	static S64 mat_coded_cost[64];
	static S64 mat_uncoded_cost[64];
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U32 scan_pos = 0;
	S64 d64_best_cost = 0;
	static S64 d64_base_cost[4096] = { 0 };
	static S64 base_cost_buffer[4096] = { 0 };
	// added by xfhuang.
	static S64 base_cost_buffer_tmp[64] = { 0 };
	//S64 d64_base_cost_tmp[1024] = { 0 };
	S64 d64_best_cost_tmp = 0;
	int best_last_idx_p1_tmp = 0;
	U24 mat_prev_level[64] = { 0 };
	static U1 mat_run[64] = { 0 };
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	static S64 d64_cost_last_one_tmp[64] = { 0 };
	S64 finalRdoqCost = 0;
	static S64 tempCost[64];   // all scan line cost
	static S64 endPosCost[64]; // as the last position cost.
	static S64 rdoqD64LastOne[64]; // shifter buffer the d64_cost_last_one_tmp
	static int rdoq_last_x[64];
	static int rdoq_last_y[64];
	for (int i = 0; i < 64; i++)
	{
		rdoqD64LastOne[i] = -1;
		rdoq_last_x[i] = -1;
		rdoq_last_y[i] = -1;
	}
	int final_last_x = -1, final_last_y = -1;
	int final_rdoq_last_x = -1, final_rdoq_last_y = -1;
	static U5 t0[64] = { 0 };
	static U4 last1_pos[64] = { 0 };
	static U5 last2_pos[64] = { 0 };


	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		d64_best_cost_tmp = 0;
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		//d64_best_cost = d64_best_cost >> 24;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		//cbf_cost = cbf_cost >> 24;
		// optimize: d64bestCost (cbf=0 cost) compare with (base_cost_buffer[scan_pos] + cbf=1 cost)
		// is same: (cbf=0 cost) - (cbf=1 cost) compare with (base_cost_buffer[scan_pos])
		d64_best_cost = d64_best_cost - cbf_cost;
		d64_best_cost_tmp = d64_best_cost;
	}

	// added by xfhuang 
	// added by xfhuang 

#if 1
	// backward to block pose
	// mat_coded_cost mat_uncoded_cost lev_opt are input as zig-zag scan ordeer

	for (int j = 0; j < 64; j++) // vertical
	{
		static u32 last1_tmp[12] = { 0 };
		static u32 last2_tmp[22] = { 0 };
		static u32 level_tmp[24] = { 0 };
		static u32 level_tmp1[24] = { 0 };
		static u32 run_tmp[24] = { 0 };
		static u32 run_tmp1[24] = { 0 };
		static U12 pos[64];
		//pre_quant
		for (int i = 0; i < 64; i++)
		{
			if (i < 64 - j) {
				U36 level_double = src_coef[i][j];
				S25 max_abs_level;
				U1 lower_int;
				U36 temp_level;
				temp_level = (U36)abs(src_coef[i][j]) * q_value;
				level_double = (U36)COM_MIN((temp_level), (U36)(COM_INT32_MAX - (1 << (q_bits - 1))));
				tmp_level_double[i] = level_double;
				max_abs_level = (S32)(level_double >> q_bits);
				lower_int = ((level_double - (max_abs_level << q_bits)) < (1 << (q_bits - 1))) ? 1 : 0;
				if (!lower_int)
				{
					max_abs_level++;
				}
				tmp_coef[i] = src_coef[i][j] > 0 ? (S16)max_abs_level : (S16)(-max_abs_level);
			}
		}
		//doc
		for (int i = 0; i < 64; i++)
		{
			//U32 blk_pos = scan[scan_pos - 1];
			if (i < 64 - j) {
				if (tmp_coef[i])
				{
					mat_prev_level[i] = abs(tmp_coef[i]);
					//tmp = abs(tmp_coef[blk_pos]);
					mat_run[i] = 0;
				}
				else
				{
					mat_prev_level[i] = 1;
					mat_run[i] = 1;
				}
			}
		}

		for (int i = 0; i < 64; i++)
		{
			if (i < 64 - j) {
				//U32 blk_pos = scan[scan_pos];
				U24 level;
				U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);

				int iflast = (i * 64 + j == 4095);
				level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
					mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, iflast, rdoq_est_run_local, rdoq_est_level_local);
				S17 level_cbf = level;
				tmp_dst_coef[i] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
				tmp_dst_coef_out[i * 64 + j] = tmp_dst_coef[i];
				mat_uncoded_cost[i] = d64_uncoded_cost;
				mat_coded_cost[i] = d64_coded_cost;
				lev_opt[i] = level;
			}
		}
		//model update
		for (int i = 0; i < 64; i++)
		{
//#pragma HLS UNROLL
			if (i + j <= 63) {
				if ((i + j) % 2 == 1) {
					pos[i] = (i + j + 1) * (i + j) / 2 + i;
				}
				else {
					pos[i] = (i + j + 1) * (i + j) / 2 + j;
				}
				ace_log2 = get_ace_log2(pos[i] + 1);
				t0[i] = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == 0 ? 0 : 12);
				last1_pos[i] = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5)))) + (ch_type == 0 ? 0 : 6);
				last2_pos[i] = ace_log2 + (ch_type == 0 ? 0 : 12);
			}
		}
		for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
		{
//#pragma HLS UNROLL
			level_core_64(64, 64, level_tmp, level_tmp1, t0, scan_pos, 64 - j);
			run_core_64(64, 64, run_tmp, run_tmp1, t0, scan_pos, mat_run, 64 - j);
		}

		for (scan_pos = 0; scan_pos < 12; scan_pos++)
		{
//#pragma HLS UNROLL
			last1_core_64(64, 64, last1_tmp, last1_pos, scan_pos, 64 - j);
			last1[scan_pos] += last1_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 22; scan_pos++)
		{
//#pragma HLS UNROLL
			last2_core_64(64, 64, last2_tmp, last2_pos, scan_pos, 64 - j);
			last2[scan_pos] += last2_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 24; scan_pos++)
		{
//#pragma HLS UNROLL
			level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
			run[scan_pos] += run_tmp[scan_pos] + run_tmp1[scan_pos];
		}

		//dlnp prepare
		for (int i = 0; i < 64; i++)
		{
			//U8 blk_pos = scan[scan_pos];
			if (i + j <= 63) {
				ace_log2 = get_ace_log2(pos[i]);
				U4 flag = 0;
				if (mat_prev_level[i] - 1 < 5)
				{
					flag = mat_prev_level[i] - 1;
				}
				else
				{
					flag = 5;
				}
				d64_cost_last_zero[i] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
				//d64_cost_last_zero_tmp[blk_pos] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda; // block scan order
			}
		}

		for (int i = 0; i < 64; i++)
		{
			//int blk_pos = scan[scan_pos];
			if (i < 64 - j) {
				if (lev_opt[i])
				{
					base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
				}
				else
				{
					base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i];
				}
				// base_cost_buffer_tmp[blk_pos] = -scan_pos;
				//lev_opt_tmp[blk_pos] = lev_opt[scan_pos] != 0;
			}
		}
		for (int i = 0; i < 64; i++)
		{
			//U8 blk_pos = scan[scan_pos];
			if (i < 64 - j) {
				ace_log2 = get_ace_log2(pos[i]);
				U24 prev_level = mat_prev_level[i];
				U4 flag = 0;
				if (prev_level - 1 < 5)
				{
					flag = prev_level - 1;
				}
				else
				{
					flag = 5;
				}
				d64_cost_last_one_tmp[i] = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda); // block scan order
			}
		}

		//dlnp core
		for (int i = 0; i < 64; i++) // horizontal
		{
			if (j == 0)
			{
				// tempCost[j] = base_cost_buffer_tmp[j];
				if (lev_opt[i])
				{

					rdoq_last_x[i] = i;
					rdoq_last_y[i] = j;
					rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
				}
				tempCost[i] = base_cost_buffer_tmp[i];
				if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
				{
					if (lev_opt[i]) endPosCost[i] = base_cost_buffer_tmp[i];
					else endPosCost[i] = 0;
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (lev_opt[i]) endPosCost[i] = 0;
					else endPosCost[i] = base_cost_buffer_tmp[i];

					// a special case (0, 0) position.
					if (i == 0)  // substract here is to get the actual endPosCost
					{
						endPosCost[i] = tempCost[i] - endPosCost[i];
					}
				}
			}
			else
			{
				if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
				{

					if (lev_opt[i] && rdoq_last_y[i] != -1) // compare for intialized case.
					{
						// add the rdoqD64LastOne consider here.
						// compare the best LNP for bottom-right scan
						if (tempCost[i] + base_cost_buffer_tmp[i] + d64_cost_last_one_tmp[i] < endPosCost[i] + rdoqD64LastOne[i])
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}
					}
					if (lev_opt[i] && rdoq_last_y[i] == -1) // cost initial. purpose.
					{
						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}

					tempCost[i] += base_cost_buffer_tmp[i];
				}
				else // above - top scan for (i+j)%2 is even case
				{
					S64 curCost = base_cost_buffer_tmp[i];  // care the bit-width. not int.


					if (lev_opt[i] && rdoq_last_y[i] != -1) // the previous non-zero position
					{
						// consider d64_cost_last_one_tmp part cost.
						if (tempCost[i] - endPosCost[i] + rdoqD64LastOne[i] - d64_cost_last_one_tmp[i] > 0) // previous position is better
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}
					}

					if (lev_opt[i] && rdoq_last_y[i] == -1) // first non-zero position
					{
						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						// from start to current position (except current pos.)
						endPosCost[i] = tempCost[i];
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}

					tempCost[i] += curCost; // from the start point to current pos cost.

					if (i == 0)  // substract here is to get the actual endPosCost
					{
						endPosCost[i] = tempCost[i] - endPosCost[i];
					}
				}
			}
		}

		// each row assign.

		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];

		// cross check.

		if (rdoq_last_y[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];

		// shift register.
		for (int i = 1; i < 64; i++)
		{
			rdoq_last_x[i - 1] = rdoq_last_x[i];
			rdoq_last_y[i - 1] = rdoq_last_y[i];
			tempCost[i - 1] = tempCost[i];
			endPosCost[i - 1] = endPosCost[i];
			rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
		}
	}

	// back to zig-zag scan order this can be optimized later.
	/*for (scan_pos = 0; scan_pos < 4096; scan_pos++)
	{
		U32 blk_pos = scan[scan_pos];
		if (blk_pos == final_rdoq_last_x * 64 + final_rdoq_last_y)
		{
			best_last_idx_p1_tmp = scan_pos + 1;
		}
	}*/
#endif
	* final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;
	//*best_last_idx_p1 = best_last_idx_p1_tmp;
}


void CleanNz_64_64(int* num_nz_coef, s16 dst_tmp[64][64], S16 tmp_dst_coef[4096], int final_x, int final_y)
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			if (i + j < final_x + final_y) {
				dst_tmp[i][j] = tmp_dst_coef[i * 64 + j];
			}
			else if (i + j == final_x + final_y) {
				if ((final_x + final_y) % 2 == 1) {
					if (i<final_x && j>final_y) {
						dst_tmp[i][j] = 0;
					}
					else {
						dst_tmp[i][j] = tmp_dst_coef[i * 64 + j];
					}
				}
				else {
					if (i > final_x && j < final_y) {
						dst_tmp[i][j] = 0;
					}
					else {
						dst_tmp[i][j] = tmp_dst_coef[i * 64 + j];
					}
				}
			}
			else {
				dst_tmp[i][j] = 0;
			}
			*num_nz_coef += !!(dst_tmp[i][j]);
		}

	}
}


int rdoq_top_64_64(U7 w, U7 h, RDOQ_MODEL* model_cnt, U6 qp, s64 d_lambda, U1 is_intra, s16 coef[64][64], s16 coef_out[64][64], U3 cu_width_log2, U3 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{
	int num_nz_coef = 0;
	const s64 lambda = (d_lambda * (1 << 15) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	static S16 tmp_dst_coef[4096] = { 0 };
	int best_last_idx_p1 = 0;
	int final_x = 0;
	int final_y = 0;
	rdoq_64_64_hardware(coef, qp, tmp_dst_coef, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level,
		&best_last_idx_p1, rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level,
		rdoq_array->rdoq_est_last, ch_type, is_intra, lambda, bit_depth, &final_x, &final_y);
	CleanNz_64_64(&num_nz_coef, coef_out, tmp_dst_coef, final_x, final_y);
	return num_nz_coef;
}
void rdoq_64_all_hardware(U3 cu_width_log2, U3 cu_height_log2, s16 src_coef[64][64], U6 qp, S16 tmp_dst_coef_out[64][64], int* best_last_idx_p1,
	int rdoq_est_cbf[3][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2],
	s32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda, U4 bit_depth, int* final_x, int* final_y)
{
	U8 cu_width = 1 << cu_width_log2;
	U8 cu_height = 1 << cu_height_log2;
	U16 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
	U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };
	const U14 scale = quant_scale[qp];
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);
	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	const int q_bits = QUANT_SHIFT + tr_shift;
	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	S16 tmp_dst_coef[64];
	U36 tmp_level_double[64];
	S16 tmp_coef[64];
	S64 d64_cost_last_zero[64];
	U24 lev_opt[64];
	//S64 mat_coded_cost[64];
	//S64 mat_uncoded_cost[64];
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U32 scan_pos = 0;
	S64 d64_best_cost = 0;
	//S64 d64_base_cost[4096] = { 0 };
	//S64 base_cost_buffer[4096] = { 0 };
	S64 base_cost_buffer_tmp[64];
	S64 d64_best_cost_tmp = 0;
	int best_last_idx_p1_tmp = 0;
	U24 mat_prev_level[64];
	U1 mat_run[64];
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	S64 d64_cost_last_one_tmp[64];
	S64 finalRdoqCost = 0;
	S64 tempCost[64];   // all scan line cost
	S64 endPosCost[64]; // as the last position cost.
	S64 rdoqD64LastOne[64]; // shifter buffer the d64_cost_last_one_tmp
	int rdoq_last_x[64];
	int rdoq_last_y[64];

#pragma HLS ARRAY_PARTITION variable=rdoqD64LastOne complete dim=1
#pragma HLS ARRAY_PARTITION variable=rdoq_last_x complete dim=1
#pragma HLS ARRAY_PARTITION variable=rdoq_last_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_one_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=base_cost_buffer_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=1
#pragma HLS ARRAY_PARTITION variable=rdoq_est_run_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_est_level_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_est_last_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_coef cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef_out cyclic factor=16 dim=1

	for (int i = 0; i < 64; i++)
	{
#pragma HLS UNROLL
		rdoqD64LastOne[i] = -1;
		rdoq_last_x[i] = -1;
		rdoq_last_y[i] = -1;
		mat_prev_level[i] = 0;
		mat_run[i] = 0;
		d64_cost_last_one_tmp[i] = 0;
		base_cost_buffer_tmp[i] = 0;
		d64_cost_last_zero[i] = 0;
	}
	int final_last_x = -1, final_last_y = -1;
	int final_rdoq_last_x = -1, final_rdoq_last_y = -1;
	int shift_t = 0;
	if (cu_width == 8 || cu_height == 8) {
		shift_t = 8;
	}
	else if (cu_width == 16 || cu_height == 16) {
		shift_t = 16;
	}
	else if (cu_width == 32 || cu_height == 32) {
		shift_t = 32;
	}
	else {
		shift_t = 64;
	}
	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		d64_best_cost_tmp = 0;
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		d64_best_cost = d64_best_cost - cbf_cost;
		d64_best_cost_tmp = d64_best_cost;
	}
#if 1
	for (int j = 0; j < cu_width; j++) // vertical
	{
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int i = 0; i < 64; i++)
		{
			if (i < cu_height) {
				if (i < cu_width - j) {
					S16 tmp = (i < 32 && j < 32) ? src_coef[i][j] : 0;
					U36 level_double = tmp;
					S25 max_abs_level;
					U1 lower_int;
					U36 temp_level;
					temp_level = (U36)abs(tmp) * q_value;
					level_double = (U36)COM_MIN((temp_level), (U36)(COM_INT32_MAX - (1 << (q_bits - 1))));
					tmp_level_double[i] = level_double;
					max_abs_level = (S32)(level_double >> q_bits);
					lower_int = ((level_double - (max_abs_level << q_bits)) < (1 << (q_bits - 1))) ? 1 : 0;
					if (!lower_int)
					{
						max_abs_level++;
					}
					tmp_coef[i] = tmp > 0 ? (S16)max_abs_level : (S16)(-max_abs_level);

					if (tmp_coef[i])
					{
						mat_prev_level[i] = abs(tmp_coef[i]);
						mat_run[i] = 0;
					}
					else
					{
						mat_prev_level[i] = 1;
						mat_run[i] = 1;
					}

					U24 level;
					U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);
					level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
						mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, 0, rdoq_est_run_local, rdoq_est_level_local);
					S17 level_cbf = level;
					tmp_dst_coef[i] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
					tmp_dst_coef_out[i][j] = tmp_dst_coef[i];
					//mat_uncoded_cost[i] = d64_uncoded_cost;
					//mat_coded_cost[i] = d64_coded_cost;
					lev_opt[i] = level;

					if (i + j < 3) {
						ace_log2 = i + j;
					}
					else if (i + j < 4) {
						ace_log2 = 3;
					}
					else if (i + j < 6) {
						ace_log2 = 4;
					}
					else if (i + j < 10) {
						ace_log2 = 5;
					}
					else if (i + j < 14) {
						ace_log2 = 6;
					}
					else if (i + j < 21) {
						ace_log2 = 7;
					}
					else {
						ace_log2 = 8;
					}
					U4 flag = 0;
					if (mat_prev_level[i] - 1 < 5)
					{
						flag = mat_prev_level[i] - 1;
					}
					else
					{
						flag = 5;
					}
					d64_cost_last_zero[i] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
					d64_cost_last_one_tmp[i] = rdoq_est_last_local[ctx_last][flag][ace_log2][1] * lambda; // block scan order

					if (lev_opt[i])
					{
						base_cost_buffer_tmp[i] = d64_coded_cost - d64_uncoded_cost + d64_cost_last_zero[i];
					}
					else
					{
						base_cost_buffer_tmp[i] = d64_coded_cost - d64_uncoded_cost;
					}
				}
			}
		}

		for (int i = 0; i < 64; i++) // horizontal
		{
			if (i < cu_height) {
				if (j == 0)
				{
					if (lev_opt[i])
					{
						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}
					tempCost[i] = base_cost_buffer_tmp[i];
					if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
					{
						if (lev_opt[i]) endPosCost[i] = base_cost_buffer_tmp[i];
						else endPosCost[i] = 0;
					}
					else // above - top scan for (i+j)%2 is even case
					{
						if (lev_opt[i]) endPosCost[i] = 0;
						else endPosCost[i] = base_cost_buffer_tmp[i];
						if (i == 0)  // substract here is to get the actual endPosCost
						{
							endPosCost[i] = tempCost[i] - endPosCost[i];
						}
					}
				}
				else if (i == shift_t - 1)
				{
					rdoq_last_x[i] = -1;
					rdoq_last_y[i] = -1;
					rdoqD64LastOne[i] = -1;
					if (lev_opt[i])
					{
						rdoq_last_x[i] = i;
						rdoq_last_y[i] = j;
						rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
					}
					tempCost[i] = base_cost_buffer_tmp[i];
					if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
					{
						if (lev_opt[i]) endPosCost[i] = base_cost_buffer_tmp[i];
						else endPosCost[i] = 0;
					}
					else // above - top scan for (i+j)%2 is even case
					{
						if (lev_opt[i]) endPosCost[i] = 0;
						else endPosCost[i] = base_cost_buffer_tmp[i];
					}
				}
				else
				{
					if ((i + j) % 2 == 0) // bottom - left scan for (i+j)%2 is odd case
					{
						if (lev_opt[i] && rdoq_last_y[i] != -1) // compare for intialized case.
						{
							if (tempCost[i] + base_cost_buffer_tmp[i] + d64_cost_last_one_tmp[i] < endPosCost[i] + rdoqD64LastOne[i])
							{
								rdoq_last_x[i] = i;
								rdoq_last_y[i] = j;
								endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
								rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
							}
						}
						if (lev_opt[i] && rdoq_last_y[i] == -1) // cost initial. purpose.
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i] + base_cost_buffer_tmp[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}
						tempCost[i] += base_cost_buffer_tmp[i];
					}
					else // above - top scan for (i+j)%2 is even case
					{
						S64 curCost = base_cost_buffer_tmp[i];  // care the bit-width. not int.
						if (lev_opt[i] && rdoq_last_y[i] != -1) // the previous non-zero position
						{
							if (tempCost[i] - endPosCost[i] + rdoqD64LastOne[i] - d64_cost_last_one_tmp[i] > 0) // previous position is better
							{
								rdoq_last_x[i] = i;
								rdoq_last_y[i] = j;
								endPosCost[i] = tempCost[i];
								rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
							}
						}
						if (lev_opt[i] && rdoq_last_y[i] == -1) // first non-zero position
						{
							rdoq_last_x[i] = i;
							rdoq_last_y[i] = j;
							endPosCost[i] = tempCost[i];
							rdoqD64LastOne[i] = d64_cost_last_one_tmp[i];
						}
						tempCost[i] += curCost; // from the start point to current pos cost.
						if (i == 0)  // substract here is to get the actual endPosCost
						{
							endPosCost[i] = tempCost[i] - endPosCost[i];
						}
					}
				}
			}
		}
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];
		if (rdoq_last_y[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];
		for (int i = 1; i < 64; i++)
		{
#pragma HLS UNROLL
			if (i < shift_t) {
				rdoq_last_x[i - 1] = rdoq_last_x[i];
				rdoq_last_y[i - 1] = rdoq_last_y[i];
				tempCost[i - 1] = tempCost[i];
				endPosCost[i - 1] = endPosCost[i];
				rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
			}
		}
	}
#endif
	* final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;
}
void CleanNz_64_all(U7 w, U7 h, int* num_nz_coef, s16 dst_tmp[64][64], s16 dst_tmp1[64][64], s16 dst_tmp2[64][64], S16 tmp_dst_coef[64][64], int final_x, int final_y)
{
	s16 tmp = 0;
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=dst_tmp cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=dst_tmp1 cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=dst_tmp2 cyclic factor=16 dim=1
	for (int j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE II=4
		for (int i = 0; i < 64; i++)
		{
			if (i < h && j < w) {
				if (i + j < final_x + final_y) {
					tmp = tmp_dst_coef[i][j];
				}
				else if (i + j == final_x + final_y) {
					if ((final_x + final_y) % 2 == 1) {
						if (i > final_x && j < final_y) {
							tmp = 0;
						}
						else {
							tmp = tmp_dst_coef[i][j];
						}
					}
					else {
						if (i < final_x && j > final_y) {
							tmp = 0;
						}
						else {
							tmp = tmp_dst_coef[i][j];
						}
					}
				}
				else {
					tmp = 0;
				}
				*num_nz_coef += !!(tmp);

				if (i < 32 && j < 32) {
					tmp = tmp;
				}
				else {
					tmp = 0;
				}
				dst_tmp1[i][j] = tmp;
				dst_tmp2[i][j] = tmp;
			}
			dst_tmp[i][j] = tmp;
		}
	}
}
int rdoq_top_64_all(U7 w, U7 h, U6 qp, s64 d_lambda, U1 is_intra, s16 coef[64][64], s16 coef_out[64][64], s16 coef_out1[64][64], s16 coef_out2[64][64], U3 cu_width_log2, U3 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{
	int num_nz_coef = 0;
	const s64 lambda = (d_lambda * (1 << 15) + 500) / 1024;
	S16 tmp_dst_coef[64][64];
	int best_last_idx_p1 = 0;
	int final_x = 0;
	int final_y = 0;
	rdoq_64_all_hardware(cu_width_log2, cu_height_log2, coef, qp, tmp_dst_coef,
		&best_last_idx_p1, rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level,
		rdoq_array->rdoq_est_last, ch_type, is_intra, lambda, bit_depth, &final_x, &final_y);
	CleanNz_64_all(w, h, &num_nz_coef, coef_out, coef_out1, coef_out2, tmp_dst_coef, final_x, final_y);
	return num_nz_coef;
}


static void memcpy_rewrintra_2d_to_1d_64(s16* des, s16 src[64][64], U3 cu_width_log2, U3 cu_height_log2)
{
	int w = 1 << cu_width_log2;
	int h = 1 << cu_height_log2;

//#pragma HLS INLINE off
	int i, j;
	for (i = 0; i < h; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < 64; j++) {
//#pragma HLS UNROLL
			if (j < w)
				des[(i << cu_width_log2) + j] = src[i][j];
		}

	}
}

void quant_nnz_64(U3 cu_width_log2, U3 cu_height_log2, U2 ch_type, U2 slice_type, U4 bit_depth, U6 qp,
	s64 lambda_y_for_quant, U1 is_intra_for_quant,
	RDOQ_ARRAY* rdoq_array,
	pel coef[64][64],
	s64* lambda_y_for_re, U1* is_intra_for_re, int* num_nz_for_re, int* num_nz_for_itv,
	pel qcoef_for_dq[64][64], pel qcoef_for_re[64][64], pel qcoef_for_update[64][64])
{
	int num_nz_coef = 0;
	int i;
	int j;
	static U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
	U14 scale = quant_scale[qp];
	U7 width = 1 << cu_width_log2;
	U7 height = 1 << cu_height_log2;
	int SIZE = width * height;


#if RDOQ_OPEN

	num_nz_coef = rdoq_top_64_all(width, height, qp, lambda_y_for_quant, is_intra_for_quant, coef, qcoef_for_dq, qcoef_for_re, qcoef_for_update, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_array);
#else
	for (int i = 32; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			coef[i][j] = 0;
		}
	}
	for (int i = 0; i < 32; i++)
	{
		for (int j = 32; j < 64; j++)
		{
			coef[i][j] = 0;
		}
	}
	num_nz_coef = quant_64(width, height, cu_width_log2, cu_height_log2, bit_depth, coef, qcoef_for_dq, qcoef_for_re, qcoef_for_update, slice_type, scale);
#endif

	* is_intra_for_re = is_intra_for_quant;
	*num_nz_for_re = num_nz_coef;
	*num_nz_for_itv = num_nz_coef;
	*lambda_y_for_re = lambda_y_for_quant;
}

static int get_md_rdo_num_64(U7 w, U7 h, U3 qt_depth)
{
	int max_rdo_num = 5;

	if (qt_depth == 2)
	{
		if (w == 16 && h == 16)
			max_rdo_num = 2;
		else
			max_rdo_num = 3;
	}

	return max_rdo_num;
}

void Ipred_vert_64(U7 w, U7 h, pel src_up[128 + 3], pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64])
{
	int i, j;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
			////#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache_16x16[i][j] = src_up[j];
				coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cache_16x16[i + 1][j] = src_up[j];
				coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
	}
}
void Ipred_hor_64(U7 w, U7 h, pel src_le[128 + 3], pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64])
{
	int i, j;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
			////#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache_16x16[i][j] = src_le[i];
				coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cache_16x16[i + 1][j] = src_le[i + 1];
				coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
	}
}
void Ipred_dc_64(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, u16 avail_cu, pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64])
{
	int dc = 0;
	//	int wh;
	int i;
	int j;
	int offset_s1 = 0;
	int s_src2 = w;
	int s_diff = w;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < 64; i = i + 2) {
//#pragma HLS UNROLL
			if (i < h)
			{
				dc += (src_le[i]);
				dc += (src_le[i + 1]);
			}
		}
		if (((avail_cu) & 1 << 0) == 1 << 0) {
			for (j = 0; j < 64; j = j + 2) {
//#pragma HLS UNROLL
				if (j < w)
				{
					dc += (src_up[j]);
					dc += (src_up[j + 1]);
				}
			}
			if (w == h)
				dc = (dc + ((128) >> 1)) * (4096 / (128)) >> 12;
			if ((w + h) == 96)
				dc = (dc + ((96) >> 1)) * (4096 / (96)) >> 12;
			if ((w + h) == 80)
				dc = (dc + ((80) >> 1)) * (4096 / (80)) >> 12;
			if ((w + h) == 72)
				dc = (dc + ((72) >> 1)) * (4096 / (72)) >> 12;
		}
		else {
			dc = (dc + (h >> 1)) >> (com_tbl_log2[h]);
		}
	}
	else if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (j = 0; j < 64; j = j + 2) {
//#pragma HLS UNROLL
			if (j < w)
			{
				dc += (src_up[j]);
				dc += (src_up[j + 1]);
			}
		}
		dc = (dc + (w >> 1)) >> (com_tbl_log2[w]);
	}
	else {
		dc = 1 << (bit_depth - 1);
	}

	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
				//////#pragma HLS UNROLL factor=8
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
			////#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache_16x16[i][j] = dc;
				coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cache_16x16[i + 1][j] = dc;
				coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}

		}
	}

}
void Ipred_plane_64(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64])
{
	int coef_h = 0;
	int coef_v = 0;
	int a;
	int b;
	int c;
	int x;
	int y;
	int w2 = w >> 1;
	int h2 = h >> 1;
	s8 ib_mult[5] = { (13), (17), (5), (11), (23) };
	s8 ib_shift[5] = { (7), (10), (11), (15), (19) };
	s8 idx_w = (com_tbl_log2[w]) - 2;
	s8 idx_h = (com_tbl_log2[h]) - 2;
	s8 im_h;
	s8 is_h;
	s8 im_v;
	s8 is_v;
	int temp;

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
	int pred_cache, pred_cachea;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=ib_mult complete dim=0
//#pragma HLS ARRAY_PARTITION variable=ib_shift complete dim=0

	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];
	//rsrc = src_up + (w2 - 1);
	//rsrc = src_up;
	for (x = 1; x < 32 + 1; x = x + 4) {
//#pragma HLS UNROLL
		if (x < w2 + 1)
		{
			coef_h += x * ((src_up[w2 + x]) - (src_up[w2 - x]));
			coef_h += (x + 1) * ((src_up[w2 + x + 1]) - (src_up[w2 - (x + 1)]));
			coef_h += (x + 2) * ((src_up[w2 + x + 2]) - (src_up[w2 - (x + 2)]));
			coef_h += (x + 3) * ((src_up[w2 + x + 3]) - (src_up[w2 - (x + 3)]));
		}
	}
	//rsrc = src_le + (h2 - 1);
	//rsrc = src_le;
	for (y = 1; y < 32 + 1; y = y + 4) {
//#pragma HLS UNROLL
		if (y < h2 + 1)
		{
			coef_v += y * ((src_le[h2 + y]) - (src_le[h2 - y]));
			coef_v += (y + 1) * ((src_le[h2 + y + 1]) - (src_le[h2 - (y + 1)]));
			coef_v += (y + 2) * ((src_le[h2 + y + 2]) - (src_le[h2 - (y + 2)]));
			coef_v += (y + 3) * ((src_le[h2 + y + 3]) - (src_le[h2 - (y + 3)]));
		}
	}
	a = (src_le[h] + src_up[w]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

	int i, j;
	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		//temp2 = temp;
		for (j = 0; j < 64; j++) {
//#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache = ((temp + b * j + c * i) >> 5);
				pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));
				//temp2 += b;
				coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cachea = ((temp + b * j + c * (i + 1)) >> 5);
				pred_cache_16x16[i + 1][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cachea) ? (1 << bit_depth) - 1 : (pred_cachea))) ? 0 : (((1 << bit_depth) - 1 < (pred_cachea) ? (1 << bit_depth) - 1 : (pred_cachea))))));
				//temp2 += b;
				coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
		//temp += c;
	}

}
void Ipred_bi_64(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64])
{
	int x;
	int y;
	int ishift_x = com_tbl_log2[w];
	int ishift_y = com_tbl_log2[h];
	int ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy, wxya;
	int tmp;
	int predx, predxa;
	static int ref_up[128 >> 1];
	static int ref_le[128 >> 1];
	static int up[128 >> 1];
	static int le[128 >> 1];
	static int wy[128 >> 1];
	int wc;
	int tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=ref_up cyclic factor=32 dim=0
//#pragma HLS ARRAY_PARTITION variable=ref_le cyclic factor=32 dim=0
//#pragma HLS ARRAY_PARTITION variable=up cyclic factor=32 dim=0
//#pragma HLS ARRAY_PARTITION variable=le cyclic factor=32 dim=0
//#pragma HLS ARRAY_PARTITION variable=wy cyclic factor=32 dim=0

	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	wc = tbl_wc[wc];
	a = (src_up[w - 1]);
	b = (src_le[h - 1]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 64; x++) {
//#pragma HLS UNROLL
		if (x < w)
		{
			up[x] = b - src_up[x];
			//		ref_up[x] = src_up[x] << ishift_y;
			ref_up[x] = src_up[x];
			ref_up[x] <<= ishift_y;
		}
	}
	tmp = 0;
	for (y = 0; y < 64; y++) {
//#pragma HLS UNROLL
		if (y < h)
		{
			le[y] = a - src_le[y];
			//		ref_le[y] = src_le[y] << ishift_x;
			ref_le[y] = src_le[y];
			ref_le[y] <<= ishift_x;
			wy[y] = wt * y;
		}
	}
	int i, j; int temp, tempa;
	for (i = 0; i < 64; i = i + 2) {
//#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
//#pragma HLS UNROLL
			if (j < w)
			{
				predx = ref_le[i] + (j + 1) * le[i];
				ref_up[j] += up[j];
				wxy = j * wy[i];
				temp = ((predx << ishift_y) + (ref_up[j] << ishift_x) + wxy + offset) >> ishift_xy;
				pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));
				coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				predxa = ref_le[i + 1] + (j + 1) * le[i + 1];
				ref_up[j] += up[j];
				wxya = j * wy[i + 1];
				tempa = ((predxa << ishift_y) + (ref_up[j] << ishift_x) + wxya + offset) >> ishift_xy;
				pred_cache_16x16[i + 1][j] = (((0 > (((1 << bit_depth) - 1 < (tempa) ? (1 << bit_depth) - 1 : (tempa))) ? 0 : (((1 << bit_depth) - 1 < (tempa) ? (1 << bit_depth) - 1 : (tempa))))));
				coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
	}

}
void ipred_ang1_64(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, pel  orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF
	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[9][2] = { { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) }, { (372), (2816) }, { (256), (4096) }, { (128), (8192) } };
	////#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	s16 dxy = ipred_dxdy[ipm - 3][0];
	int i, j, t_dx, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int x = 2147483647;
	int t_dxa, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int xa = 2147483647;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
	for (j = 0; j < 64; j = j + 2)
	{
//#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			////#pragma HLS UNROLL
			if (i < w)
			{
				/* case x-line */
				/**************************** 1 ****************************/
				t_dx = (j + 1) * dxy >> 10;
				offset = ((j + 1) * dxy >> 5) - (t_dx << 5);
				x = i + t_dx;

				pn_n1 = x - 1;
				p = x;
				pn = x + 1;
				pn_p2 = x + 2;
				pos_max = w * 2 - 1;

				pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);

#if MIPF
				filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 1)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 2)
					temp_pel = (pel)((src_up[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_up[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
						src_up[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 3)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j][i] = temp_pel;
				coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#else

				pred_cache_16x16[j][i] = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + (src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
				coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#endif

				/**************************** 2 ****************************/
				t_dxa = (j + 2) * dxy >> 10;
				offseta = ((j + 2) * dxy >> 5) - (t_dxa << 5);
				xa = i + t_dxa;

				pn_n1a = xa - 1;
				pa = xa;
				pna = xa + 1;
				pn_p2a = xa + 2;
				pos_maxa = w * 2 - 1;

				pn_n1a = (pn_n1a < pos_maxa ? pn_n1a : pos_maxa);
				pa = (pa < pos_maxa ? pa : pos_maxa);
				pna = (pna < pos_maxa ? pna : pos_maxa);
				pn_p2a = (pn_p2a < pos_maxa ? pn_p2a : pos_maxa);

#if MIPF
				filter_idx = mipf_enable_flag ? ((j + 1) < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_extra_smooth[offseta][1] +
						src_up[pna + 3] * com_tbl_ipred_adi_extra_smooth[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_semi_sharp[offseta][1] +
						src_up[pna + 3] * com_tbl_ipred_adi_semi_sharp[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_up[pn_n1a + 3] * tbl_mc_c_coeff_hp[offseta][0] + src_up[pa + 3] * tbl_mc_c_coeff_hp[offseta][1] +
						src_up[pna + 3] * tbl_mc_c_coeff_hp[offseta][2] + src_up[pn_p2a + 3] * tbl_mc_c_coeff_hp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_bilinear[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_bilinear[offseta][1] +
						src_up[pna + 3] * com_tbl_ipred_adi_bilinear[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_bilinear[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j + 1][i] = temp_pel;
				coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
#else

				pred_cache_16x16[j + 1][i] = ((src_up[pn_n1a + 3]) * (32 - offseta) + (src_up[pa + 3]) * (64 - offseta) + (src_up[pna + 3]) * (32 + offseta) + (src_up[pn_p2a + 3]) * offseta + 64) >> 7;
				coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
#endif




			}
		}
	}

}
void ipred_ang2_64(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF

	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[8][2] = { { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };
	////#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	s16 dxy = ipred_dxdy[ipm - 25][1];
	int i, j, t_dy, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int y = 2147483647;
	int t_dya, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int ya = 2147483647;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
	for (j = 0; j < 64; j = j + 2)
	{
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
//#pragma HLS UNROLL
			if (i < w)
			{
				/**************************** 1 ****************************/
				t_dy = (i + 1) * dxy >> 10;
				offset = ((i + 1) * dxy >> 5) - (t_dy << 5);
				y = j + t_dy;
				pn_n1 = y - 1;
				p = y;
				pn = y + 1;
				pn_p2 = y + 2;
				pos_max = h * 2 - 1;

				pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);


#if MIPF									
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_le[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_le[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_le[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_le[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
						src_le[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_le[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
						src_le[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j][i] = temp_pel;
				coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];

#else
				pred_cache_16x16[j][i] = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
				coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#endif



				/**************************** 2 ****************************/
				t_dya = (i + 1) * dxy >> 10;
				offseta = ((i + 1) * dxy >> 5) - (t_dya << 5);
				ya = j + 1 + t_dya;
				pn_n1a = ya - 1;
				pa = ya;
				pna = ya + 1;
				pn_p2a = ya + 2;
				pos_maxa = h * 2 - 1;

				pn_n1a = (pn_n1a < pos_maxa ? pn_n1a : pos_maxa);
				pa = (pa < pos_maxa ? pa : pos_maxa);
				pna = (pna < pos_maxa ? pna : pos_maxa);
				pn_p2a = (pn_p2a < pos_maxa ? pn_p2a : pos_maxa);

#if MIPF									
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_extra_smooth[offseta][1] +
						src_le[pna + 3] * com_tbl_ipred_adi_extra_smooth[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_semi_sharp[offseta][1] +
						src_le[pna + 3] * com_tbl_ipred_adi_semi_sharp[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_le[pn_n1a + 3] * tbl_mc_c_coeff_hp[offseta][0] + src_le[pa + 3] * tbl_mc_c_coeff_hp[offseta][1] +
						src_le[pna + 3] * tbl_mc_c_coeff_hp[offseta][2] + src_le[pn_p2a + 3] * tbl_mc_c_coeff_hp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_bilinear[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_bilinear[offseta][1] +
						src_le[pna + 3] * com_tbl_ipred_adi_bilinear[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_bilinear[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1


				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j + 1][i] = temp_pel;
				coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];

#else
				pred_cache_16x16[j + 1][i] = ((src_le[pn_n1a + 3]) * (32 - offseta) + (src_le[pa + 3]) * (64 - offseta) + (src_le[pna + 3]) * (32 + offseta) + (src_le[pn_p2a + 3]) * offseta + 64) >> 7;
				coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
#endif


			}
		}
	}

}
void ipred_ang3_64(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, pel orgY[64 * 64], s16 pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF
	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	//pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[11][2] = { { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) } };
	////#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	s16 dxy0 = ipred_dxdy[ipm - 13][0];
	s16 dxy1 = ipred_dxdy[ipm - 13][1];
	s16 dxya = ipred_dxdy[ipm - 13][0];
	s16 dxyb = ipred_dxdy[ipm - 13][1];
	int i, j;
	int offset, offset_x, offset_y, xx, yy, t_dx, t_dy;
	int p, pn, pn_n1, pn_p2;
	pel temp_pel = 0;
	int offseta, offset_xa, offset_ya, xxa, yya, t_dxa, t_dya;
	int pa, pna, pn_n1a, pn_p2a;
	pel temp_pela = 0;
	pel temp_pelax = 0;
	pel temp_pelay = 0;
	pel temp_pel0 = 0;
	pel temp_pel1 = 0;
	int xxyya, xxyy;
//#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
//#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
//#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=64  dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 cyclic factor=2  dim=1
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=2
//#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 cyclic factor=2  dim=1
	for (j = 0; j < 64; j = j + 2)
	{
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			////#pragma HLS UNROLL
			if (i < w)
			{
				{
					/**************************** 1 ****************************/
					t_dx = (j + 1) * dxy0 >> 10;
					offset_x = ((j + 1) * dxy0 >> 5) - (t_dx << 5);
					t_dx = -t_dx;
					xx = i + t_dx;
					t_dy = (i + 1) * dxy1 >> 10;
					offset_y = ((i + 1) * dxy1 >> 5) - (t_dy << 5);
					t_dy = -t_dy;
					yy = j + t_dy;
					offset = (yy <= -1) ? offset_x : offset_y;
					xxyy = (yy <= -1) ? xx : yy;
					pn_n1 = xxyy + 1;
					p = xxyy;
					pn = xxyy - 1;
					pn_p2 = xxyy - 2;

#if MIPF
					if ((ipm < 12) || ((ipm >= 13) && (ipm <= 17)) || ((ipm >= 34) && (ipm <= 50))) {
						filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
					}
					else if (((ipm >= 18) && (ipm <= 23)) || ((ipm >= 25) && (ipm <= 32)) || ((ipm >= 51) && (ipm <= 65))) {
						filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
					}

					if (filter_idx == 0)
						temp_pel0 = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
							src_up[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 1)
						temp_pel0 = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
							src_up[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 2)
						temp_pel0 = (pel)((src_up[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_up[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
							src_up[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 3)
						temp_pel0 = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
							src_up[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

					if (filter_idx == 0)
						temp_pel1 = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
							src_le[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
					else if (filter_idx == 1)
						temp_pel1 = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
							src_le[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
					else if (filter_idx == 2)
						temp_pel1 = (pel)((src_le[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_le[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
							src_le[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_le[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
					else if (filter_idx == 3)
						temp_pel1 = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
							src_le[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1

					temp_pel = (yy <= -1) ? temp_pel0 : temp_pel1;
					temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
#else
					temp_pel0 = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + ((
						int)src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
					temp_pel1 = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
					temp_pel = (yy <= -1) ? temp_pel0 : temp_pel1;
#endif

					pred_cache_16x16[j][i] = temp_pel;
					coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
				}
				{
					/**************************** 2 ****************************/
					t_dxa = (j + 2) * dxya >> 10;
					offset_xa = ((j + 2) * dxya >> 5) - (t_dxa << 5);
					t_dxa = -t_dxa;
					xxa = i + t_dxa;
					t_dya = (i + 1) * dxyb >> 10;
					offset_ya = ((i + 1) * dxyb >> 5) - (t_dya << 5);
					t_dya = -t_dya;
					yya = j + 1 + t_dya;
					offseta = (yya <= -1) ? offset_xa : offset_ya;
					xxyya = (yya <= -1) ? xxa : yya;
					pn_n1a = xxyya + 1;
					pa = xxyya;
					pna = xxyya - 1;
					pn_p2a = xxyya - 2;

#if MIPF
					if ((ipm < 12) || ((ipm >= 13) && (ipm <= 17)) || ((ipm >= 34) && (ipm <= 50))) {
						filter_idx = mipf_enable_flag ? ((j + 1) < td ? is_small + 1 : is_small) : 3;
					}
					else if (((ipm >= 18) && (ipm <= 23)) || ((ipm >= 25) && (ipm <= 32)) || ((ipm >= 51) && (ipm <= 65))) {
						filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
					}

					if (filter_idx == 0)
						temp_pelax = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_extra_smooth[offseta][1] +
							src_up[pna + 3] * com_tbl_ipred_adi_extra_smooth[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
					else if (filter_idx == 1)
						temp_pelax = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_semi_sharp[offseta][1] +
							src_up[pna + 3] * com_tbl_ipred_adi_semi_sharp[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
					else if (filter_idx == 2)
						temp_pelax = (pel)((src_up[pn_n1a + 3] * tbl_mc_c_coeff_hp[offseta][0] + src_up[pa + 3] * tbl_mc_c_coeff_hp[offseta][1] +
							src_up[pna + 3] * tbl_mc_c_coeff_hp[offseta][2] + src_up[pn_p2a + 3] * tbl_mc_c_coeff_hp[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
					else if (filter_idx == 3)
						temp_pelax = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_bilinear[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_bilinear[offseta][1] +
							src_up[pna + 3] * com_tbl_ipred_adi_bilinear[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_bilinear[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1

					if (filter_idx == 0)
						temp_pelay = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_extra_smooth[offseta][1] +
							src_le[pna + 3] * com_tbl_ipred_adi_extra_smooth[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 1)
						temp_pelay = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_semi_sharp[offseta][1] +
							src_le[pna + 3] * com_tbl_ipred_adi_semi_sharp[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 2)
						temp_pelay = (pel)((src_le[pn_n1a + 3] * tbl_mc_c_coeff_hp[offseta][0] + src_le[pa + 3] * tbl_mc_c_coeff_hp[offseta][1] +
							src_le[pna + 3] * tbl_mc_c_coeff_hp[offseta][2] + src_le[pn_p2a + 3] * tbl_mc_c_coeff_hp[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 3)
						temp_pelay = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_bilinear[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_bilinear[offseta][1] +
							src_le[pna + 3] * com_tbl_ipred_adi_bilinear[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_bilinear[offseta][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

					temp_pela = (yya <= -1) ? temp_pelax : temp_pelay;
#else

					temp_pelax = ((src_up[pn_n1a + 3]) * (32 - offseta) + (src_up[pa + 3]) * (64 - offseta) + ((
						int)src_up[pna + 3]) * (32 + offseta) + (src_up[pn_p2a + 3]) * offseta + 64) >> 7;
					temp_pelay = ((src_le[pn_n1a + 3]) * (32 - offseta) + (src_le[pa + 3]) * (64 - offseta) + (src_le[pna + 3]) * (32 + offseta) + (src_le[pn_p2a + 3]) * offseta + 64) >> 7;
					temp_pela = (yya <= -1) ? temp_pelax : temp_pelay;
					temp_pela = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pela);
#endif
					temp_pela = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pela);
					pred_cache_16x16[j + 1][i] = temp_pela;
					coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
				}
			}
		}
	}

}

void Ipred_64_diff(U7 w, U7 h, pel orgY[64 * 64], pel orgY_cp[64 * 64], s16 pred_cache_16x16[64][64], s16 pred_cache_16x16_cp[64][64], s16 coef_tmp_16x16[64][64])
{
	int i, j;

	for (i = 0; i < 64; i = i + 1) {
#pragma HLS PIPELINE II=4
		for (j = 0; j < 64; j++) {
			if (j < w)
			{
				coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
			}
			orgY_cp[i * 64 + j] = orgY[i * 64 + j];
			pred_cache_16x16_cp[i][j] = pred_cache_16x16[i][j];
		}
	}
}

#if MD_KERNEL_64_MOD_YHY
void Ipred_vert_64_pred(U7 w, U7 h, pel src_up[128 + 3], s16 pred_cache_64x64[64][64])
{

#pragma HLS ARRAY_PARTITION variable=pred_cache_64x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0

	int i, j;
	for (i = 0; i < 64; i++) 
	{
#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) 
		{
			if (j < w && i < h)
			{
				pred_cache_64x64[i][j] = src_up[j];
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
			}
		}
	}
}
void Ipred_hor_64_pred(U7 w, U7 h, pel src_le[128 + 3], s16 pred_cache_64x64[64][64])
{

#pragma HLS ARRAY_PARTITION variable=pred_cache_64x64 complete dim=2
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1

	int i, j;
	for (i = 0; i < 64; i++) 
	{
#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) 
		{
			if (j < w && i < h)
			{
				pred_cache_64x64[i][j] = src_le[i];
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
			}
		}
	}
}
void Ipred_dc_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, u16 avail_cu, s16 pred_cache_64x64[64][64])
{
	int dc = 0;
	//	int wh;
	int i;
	int j;
	int offset_s1 = 0;
	int s_src2 = w;
	int s_diff = w;

#pragma HLS array_partition variable = pred_cache_64x64 complete dim=2
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0

	if (((avail_cu) & 1 << 1) == 1 << 1) 
	{
		for (i = 0; i < 64; i++) 
		{
#pragma HLS UNROLL
			if (i < h)
			{
				dc += (src_le[i]);
			}
		}
		if (((avail_cu) & 1 << 0) == 1 << 0) 
		{
			for (j = 0; j < 64; j++) 
			{
#pragma HLS UNROLL
				if (j < w)
				{
					dc += (src_up[j]);
				}
			}
			if (w == h)
				dc = (dc + ((128) >> 1)) * (4096 / (128)) >> 12;
			if ((w + h) == 96)
				dc = (dc + ((96) >> 1)) * (4096 / (96)) >> 12;
			if ((w + h) == 80)
				dc = (dc + ((80) >> 1)) * (4096 / (80)) >> 12;
			if ((w + h) == 72)
				dc = (dc + ((72) >> 1)) * (4096 / (72)) >> 12;
		}
		else 
		{
			dc = (dc + (h >> 1)) >> (com_tbl_log2[h]);
		}
	}
	else if (((avail_cu) & 1 << 0) == 1 << 0) 
	{
		for (j = 0; j < 64; j++) 
		{
#pragma HLS UNROLL
			if (j < w)
			{
				dc += (src_up[j]);
			}
		}
		dc = (dc + (w >> 1)) >> (com_tbl_log2[w]);
	}
	else 
	{
		dc = 1 << (bit_depth - 1);
	}

	for (i = 0; i < 64; i++) 
	{
#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) 
		{
			if (j < w && i < h)
			{
				pred_cache_64x64[i][j] = dc;
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
			}

		}
	}

}
void Ipred_plane_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, s16 pred_cache_64x64[64][64])
{
	int coef_h = 0;
	int coef_v = 0;
	int a;
	int b;
	int c;
	int x;
	int y;
	int w2 = w >> 1;
	int h2 = h >> 1;
	s8 ib_mult[5] = { (13), (17), (5), (11), (23) };
	s8 ib_shift[5] = { (7), (10), (11), (15), (19) };
	s8 idx_w = (com_tbl_log2[w]) - 2;
	s8 idx_h = (com_tbl_log2[h]) - 2;
	s8 im_h;
	s8 is_h;
	s8 im_v;
	s8 is_v;
	int temp;

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
	int pred_cache, pred_cachea;

	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];

#pragma HLS array_partition variable = com_tbl_log2 complete dim=0
#pragma HLS array_partition variable = ib_mult complete dim=0
#pragma HLS array_partition variable = ib_shift complete dim=0
#pragma HLS array_partition variable = pred_cache_64x64 complete dim=2
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0

	//rsrc = src_up + (w2 - 1);
	//rsrc = src_up;
	for (x = 1; x < 33; x++) {
#pragma HLS UNROLL
		if (x < w2 + 1)
		{
			coef_h += x * ((src_up[w2 + x]) - (src_up[w2 - x]));
		}
	}
	//rsrc = src_le + (h2 - 1);
	//rsrc = src_le;
	for (y = 1; y < 33; y++) 
	{
#pragma HLS UNROLL
		if (y < h2 + 1)
		{
			coef_v += y * ((src_le[h2 + y]) - (src_le[h2 - y]));
		}
	}
	a = (src_le[h] + src_up[w]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

	int i, j;
	for (i = 0; i < 64; i++) 
	{
#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) 
		{
			if (j < w && i < h)
			{
				pred_cache = ((temp + b * j + c * i) >> 5);
				pred_cache_64x64[i][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
			}
		}
	}

}
void Ipred_bi_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, s16 pred_cache_64x64[64][64])
{
	int x;
	int y;
	int ishift_x = com_tbl_log2[w];
	int ishift_y = com_tbl_log2[h];
	int ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy, wxya;
	int tmp;
	int predx, predxa;
	static int ref_up[128 >> 1];
	static int ref_le[128 >> 1];
	static int up[128 >> 1];
	static int le[128 >> 1];
	static int wy[128 >> 1];
	int wc;
	int tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };

#pragma HLS array_partition variable = com_tbl_log2 complete dim=0
#pragma HLS array_partition variable = pred_cache_64x64 complete dim=2
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
#pragma HLS array_partition variable = ref_up complete dim=0
#pragma HLS array_partition variable = ref_le complete dim=0
#pragma HLS array_partition variable = up complete dim=0
#pragma HLS array_partition variable = le complete dim=0
#pragma HLS array_partition variable = wy complete dim=0

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;

	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	wc = tbl_wc[wc];
	a = (src_up[w - 1]);
	b = (src_le[h - 1]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 64; x++) 
	{
#pragma HLS UNROLL
		if (x < w)
		{
			up[x] = b - src_up[x];
			//		ref_up[x] = src_up[x] << ishift_y;
			ref_up[x] = src_up[x];
			ref_up[x] <<= ishift_y;
		}
	}
	tmp = 0;
	for (y = 0; y < 64; y++) 
	{
#pragma HLS UNROLL
		if (y < h)
		{
			le[y] = a - src_le[y];
			//		ref_le[y] = src_le[y] << ishift_x;
			ref_le[y] = src_le[y];
			ref_le[y] <<= ishift_x;
			wy[y] = wt * y;
		}
	}
	int i, j; int temp, tempa;
	for (i = 0; i < 64; i++) 
	{
#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) 
		{
			if (j < w && i < h)
			{
				predx = ref_le[i] + (j + 1) * le[i];
				ref_up[j] += up[j];
				wxy = j * wy[i];
				temp = ((predx << ishift_y) + (ref_up[j] << ishift_x) + wxy + offset) >> ishift_xy;
				pred_cache_64x64[i][j] = (((0 > (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
			}
		}
	}

}
void ipred_ang1_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, s16 pred_cache_64x64[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF
	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[9][2] = { { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) }, { (372), (2816) }, { (256), (4096) }, { (128), (8192) } };

#pragma HLS array_partition variable = ipred_dxdy complete dim=0
#pragma HLS array_partition variable = pred_cache_64x64  complete dim=2
//#pragma HLS array_partition variable = pred_cache_64x64  cyclic factor=8 dim=1
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0

	s16 dxy = ipred_dxdy[ipm - 3][0];
	int i, j, t_dx;
	int p, pn, pn_n1, pn_p2, pos_max;
	int x = 2147483647;
	int t_dxa, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int xa = 2147483647;

	for (j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			if (i < w && j < h)
			{
				/* case x-line */
				/**************************** 1 ****************************/
				t_dx = (j + 1) * dxy >> 10;
				offseta = ((j + 1) * dxy >> 5) - (t_dx << 5);
				x = i + t_dx;

				pn_n1 = x - 1;
				p = x;
				pn = x + 1;
				pn_p2 = x + 2;
				pos_max = w * 2 - 1;

				pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);

#if MIPF
				filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;

				filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
				
				s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][0] : com_tbl_ipred_adi_bilinear[offseta][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][1] : com_tbl_ipred_adi_bilinear[offseta][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][2] : com_tbl_ipred_adi_bilinear[offseta][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][3] : com_tbl_ipred_adi_bilinear[offseta][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];


				s32 buffer1 = src_up[pn_n1 + 3] * data1;
				s32 buffer2 = src_up[p + 3] * data2;
				s32 buffer3 = src_up[pn + 3] * data3;
				s32 buffer4 = src_up[pn_p2 + 3] * data4;

				temp_pel = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_64x64[j][i] = temp_pel;
#else

				pred_cache_64x64[j][i] = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + (src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
				//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#endif
			}
		}
	}

}
void ipred_ang2_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, s16 pred_cache_64x64[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF

	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[8][2] = { { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };

#pragma HLS array_partition variable = pred_cache_64x64  complete dim=2
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
//#pragma HLS array_partition variable = com_tbl_log2 complete dim=0
#pragma HLS array_partition variable = ipred_dxdy complete dim=0

	s16 dxy = ipred_dxdy[ipm - 25][1];
	int i, j, t_dy;
	int p, pn, pn_n1, pn_p2, pos_max;
	int y = 2147483647;
	int t_dya, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int ya = 2147483647;

	for (j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			if (i < w && j < h)
			{
				/**************************** 1 ****************************/
				t_dy = (i + 1) * dxy >> 10;
				offseta = ((i + 1) * dxy >> 5) - (t_dy << 5);
				y = j + t_dy;
				pn_n1 = y - 1;
				p = y;
				pn = y + 1;
				pn_p2 = y + 2;
				pos_max = h * 2 - 1;

				pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);

#if MIPF									
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

				s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][0] : com_tbl_ipred_adi_bilinear[offseta][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][1] : com_tbl_ipred_adi_bilinear[offseta][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][2] : com_tbl_ipred_adi_bilinear[offseta][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][3] : com_tbl_ipred_adi_bilinear[offseta][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];
				s32 buffer1 = src_le[pn_n1+3] * data1;
				s32 buffer2 = src_le[p+3] * data2;
				s32 buffer3 = src_le[pn+3] * data3;
				s32 buffer4 = src_le[pn_p2+3] * data4;
				temp_pel = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_64x64[j][i] = temp_pel;
#else
				pred_cache_64x64[j][i] = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
				//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#endif
			}
		}
	}

}
void ipred_ang3_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, s16 pred_cache_64x64[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF
	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	//pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[11][2] = { { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) } };
	s16 dxy0 = ipred_dxdy[ipm - 13][0];
	s16 dxy1 = ipred_dxdy[ipm - 13][1];
	s16 dxya = ipred_dxdy[ipm - 13][0];
	s16 dxyb = ipred_dxdy[ipm - 13][1];
	int i, j;
	int offset_x, offset_y, xx, yy, t_dx, t_dy;
	int p, pn, pn_n1, pn_p2;
	pel temp_pel = 0;
	int offseta, offset_xa, offset_ya, xxa, yya, t_dxa, t_dya;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int xy = -1, yx = -1;
	int xa, xb, ya, yb, xc, yc, xd, yd;
	pel temp_pela = 0;
	pel temp_pelax = 0;
	pel temp_pelay = 0;
	pel temp_pel0 = 0;
	pel temp_pel1 = 0;
	int xxyya, xxyy;

#pragma HLS array_partition variable = pred_cache_32x32  complete dim=2
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
#pragma HLS array_partition variable = ipred_dxdy complete dim=0

	for (j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			if (i < w && j < h)
			{
				/**************************** 1 ****************************/
				
				if ((ipm < 12) || ((ipm >= 13) && (ipm <= 17)) || ((ipm >= 34) && (ipm <= 50))) {
					filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
				}
				else if (((ipm >= 18) && (ipm <= 23)) || ((ipm >= 25) && (ipm <= 32)) || ((ipm >= 51) && (ipm <= 65))) {
					filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
				}
				t_dxa = (j + 1) * ipred_dxdy[ipm - 13][0] >> 10;
				offset_xa = ((j + 1) * ipred_dxdy[ipm - 13][0] >> 5) - (t_dxa << 5);
				t_dxa = -t_dxa;
				xxa = i + t_dxa;
				t_dya = (i + 1) * ipred_dxdy[ipm - 13][1] >> 10;
				offset_ya = ((i + 1) * ipred_dxdy[ipm - 13][1] >> 5) - (t_dya << 5);
				t_dya = -t_dya;
				yya = j + t_dya;

				if (yya <= -1) {
					offseta = offset_xa;
					xa = xxa;
					ya = yx;
				}
				else {
					offseta = offset_ya;
					ya = yya;
					xa = xy;
				}
				if (ya == -1) {
					pn_n1a = xa + 1;
					pa = xa;
					pna = xa - 1;
					pn_p2a = xa - 2;
					pos_maxa = w * 2 - 1;
				}
				else {
					pn_n1a = ya + 1;
					pa = ya;
					pna = ya - 1;
					pn_p2a = ya - 2;
					pos_maxa = h * 2 - 1;
				}

				pn_n1a += 3;
				pa += 3;
				pna += 3;
				pn_p2a += 3;

				int src_pa, src_pna, src_pn_n1a, src_pn_p2a;

				if (ya == -1) {
					src_pn_n1a = src_up[pn_n1a];
					src_pa = src_up[pa];
					src_pna = src_up[pna];
					src_pn_p2a = src_up[pn_p2a];
				}
				else if (xa == -1) {
					src_pn_n1a = src_le[pn_n1a];
					src_pa = src_le[pa];
					src_pna = src_le[pna];
					src_pn_p2a = src_le[pn_p2a];
				}
				else {
					src_pn_n1a = 0;
					src_pa = 0;
					src_pna = 0;
					src_pn_p2a = 0;
				}

#if MIPF

                s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][0] : com_tbl_ipred_adi_bilinear[offseta][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][1] : com_tbl_ipred_adi_bilinear[offseta][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][2] : com_tbl_ipred_adi_bilinear[offseta][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offseta][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offseta][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offseta][3] : com_tbl_ipred_adi_bilinear[offseta][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];

				s32 buffer1 = src_pn_n1a * data1;
				s32 buffer2 = src_pa * data2;
				s32 buffer3 = src_pna * data3;
				s32 buffer4 = src_pn_p2a * data4;

				temp_pela = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				temp_pela = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pela);
				pred_cache_64x64[j][i] = temp_pela;
#else
				temp_pela = ((src_up[pn_n1a + 3]) * (32 - offseta) + (src_up[pa + 3]) * (64 - offseta) + ((
					int)src_up[pna + 3]) * (32 + offseta) + (src_up[pn_p2a + 3]) * offseta + 64) >> 7;
#endif		
			}
		}
	}
}
#else
void Ipred_vert_64_pred(U7 w, U7 h, pel src_up[128 + 3], s16 pred_cache_16x16[64][64])
{
	int i, j;

	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
			////#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache_16x16[i][j] = src_up[j];
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cache_16x16[i + 1][j] = src_up[j];
				//coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
	}
}
void Ipred_hor_64_pred(U7 w, U7 h, pel src_le[128 + 3], s16 pred_cache_16x16[64][64])
{
	int i, j;

	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
			////#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache_16x16[i][j] = src_le[i];
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cache_16x16[i + 1][j] = src_le[i + 1];
				//coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
	}
}
void Ipred_dc_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, u16 avail_cu, s16 pred_cache_16x16[64][64])
{
	int dc = 0;
	//	int wh;
	int i;
	int j;
	int offset_s1 = 0;
	int s_src2 = w;
	int s_diff = w;

	if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < 64; i = i + 2) {
//#pragma HLS UNROLL
			if (i < h)
			{
				dc += (src_le[i]);
				dc += (src_le[i + 1]);
			}
		}
		if (((avail_cu) & 1 << 0) == 1 << 0) {
			for (j = 0; j < 64; j = j + 2) {
//#pragma HLS UNROLL
				if (j < w)
				{
					dc += (src_up[j]);
					dc += (src_up[j + 1]);
				}
			}
			if (w == h)
				dc = (dc + ((128) >> 1)) * (4096 / (128)) >> 12;
			if ((w + h) == 96)
				dc = (dc + ((96) >> 1)) * (4096 / (96)) >> 12;
			if ((w + h) == 80)
				dc = (dc + ((80) >> 1)) * (4096 / (80)) >> 12;
			if ((w + h) == 72)
				dc = (dc + ((72) >> 1)) * (4096 / (72)) >> 12;
		}
		else {
			dc = (dc + (h >> 1)) >> (com_tbl_log2[h]);
		}
	}
	else if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (j = 0; j < 64; j = j + 2) {
//#pragma HLS UNROLL
			if (j < w)
			{
				dc += (src_up[j]);
				dc += (src_up[j + 1]);
			}
		}
		dc = (dc + (w >> 1)) >> (com_tbl_log2[w]);
	}
	else {
		dc = 1 << (bit_depth - 1);
	}

	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
				//////#pragma HLS UNROLL factor=8
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
			////#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache_16x16[i][j] = dc;
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cache_16x16[i + 1][j] = dc;
				//coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}

		}
	}

}
void Ipred_plane_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, s16 pred_cache_16x16[64][64])
{
	int coef_h = 0;
	int coef_v = 0;
	int a;
	int b;
	int c;
	int x;
	int y;
	int w2 = w >> 1;
	int h2 = h >> 1;
	s8 ib_mult[5] = { (13), (17), (5), (11), (23) };
	s8 ib_shift[5] = { (7), (10), (11), (15), (19) };
	s8 idx_w = (com_tbl_log2[w]) - 2;
	s8 idx_h = (com_tbl_log2[h]) - 2;
	s8 im_h;
	s8 is_h;
	s8 im_v;
	s8 is_v;
	int temp;

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
	int pred_cache, pred_cachea;


	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];
	//rsrc = src_up + (w2 - 1);
	//rsrc = src_up;
	for (x = 1; x < 32 + 1; x = x + 4) {
//#pragma HLS UNROLL
		if (x < w2 + 1)
		{
			coef_h += x * ((src_up[w2 + x]) - (src_up[w2 - x]));
			coef_h += (x + 1) * ((src_up[w2 + x + 1]) - (src_up[w2 - (x + 1)]));
			coef_h += (x + 2) * ((src_up[w2 + x + 2]) - (src_up[w2 - (x + 2)]));
			coef_h += (x + 3) * ((src_up[w2 + x + 3]) - (src_up[w2 - (x + 3)]));
		}
	}
	//rsrc = src_le + (h2 - 1);
	//rsrc = src_le;
	for (y = 1; y < 32 + 1; y = y + 4) {
//#pragma HLS UNROLL
		if (y < h2 + 1)
		{
			coef_v += y * ((src_le[h2 + y]) - (src_le[h2 - y]));
			coef_v += (y + 1) * ((src_le[h2 + y + 1]) - (src_le[h2 - (y + 1)]));
			coef_v += (y + 2) * ((src_le[h2 + y + 2]) - (src_le[h2 - (y + 2)]));
			coef_v += (y + 3) * ((src_le[h2 + y + 3]) - (src_le[h2 - (y + 3)]));
		}
	}
	a = (src_le[h] + src_up[w]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;

	int i, j;
	for (i = 0; i < 64; i = i + 2) {
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		//temp2 = temp;
		for (j = 0; j < 64; j++) {
//#pragma HLS UNROLL
			if (j < w)
			{
				pred_cache = ((temp + b * j + c * i) >> 5);
				pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));
				//temp2 += b;
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				pred_cachea = ((temp + b * j + c * (i + 1)) >> 5);
				pred_cache_16x16[i + 1][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cachea) ? (1 << bit_depth) - 1 : (pred_cachea))) ? 0 : (((1 << bit_depth) - 1 < (pred_cachea) ? (1 << bit_depth) - 1 : (pred_cachea))))));
				//temp2 += b;
				//coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
		//temp += c;
	}

}
void Ipred_bi_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, s16 pred_cache_16x16[64][64])
{
	int x;
	int y;
	int ishift_x = com_tbl_log2[w];
	int ishift_y = com_tbl_log2[h];
	int ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy, wxya;
	int tmp;
	int predx, predxa;
	static int ref_up[128 >> 1];
	static int ref_le[128 >> 1];
	static int up[128 >> 1];
	static int le[128 >> 1];
	static int wy[128 >> 1];
	int wc;
	int tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;


	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	wc = tbl_wc[wc];
	a = (src_up[w - 1]);
	b = (src_le[h - 1]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 64; x++) {
//#pragma HLS UNROLL
		if (x < w)
		{
			up[x] = b - src_up[x];
			//		ref_up[x] = src_up[x] << ishift_y;
			ref_up[x] = src_up[x];
			ref_up[x] <<= ishift_y;
		}
	}
	tmp = 0;
	for (y = 0; y < 64; y++) {
//#pragma HLS UNROLL
		if (y < h)
		{
			le[y] = a - src_le[y];
			//		ref_le[y] = src_le[y] << ishift_x;
			ref_le[y] = src_le[y];
			ref_le[y] <<= ishift_x;
			wy[y] = wt * y;
		}
	}
	int i, j; int temp, tempa;
	for (i = 0; i < 64; i = i + 2) {
//#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
//#pragma HLS UNROLL
			if (j < w)
			{
				predx = ref_le[i] + (j + 1) * le[i];
				ref_up[j] += up[j];
				wxy = j * wy[i];
				temp = ((predx << ishift_y) + (ref_up[j] << ishift_x) + wxy + offset) >> ishift_xy;
				pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));
				//coef_tmp_16x16[i][j] = orgY[(i << 6) + j] - pred_cache_16x16[i][j];
				predxa = ref_le[i + 1] + (j + 1) * le[i + 1];
				ref_up[j] += up[j];
				wxya = j * wy[i + 1];
				tempa = ((predxa << ishift_y) + (ref_up[j] << ishift_x) + wxya + offset) >> ishift_xy;
				pred_cache_16x16[i + 1][j] = (((0 > (((1 << bit_depth) - 1 < (tempa) ? (1 << bit_depth) - 1 : (tempa))) ? 0 : (((1 << bit_depth) - 1 < (tempa) ? (1 << bit_depth) - 1 : (tempa))))));
				//coef_tmp_16x16[i + 1][j] = orgY[((i + 1) << 6) + j] - pred_cache_16x16[i + 1][j];
			}
		}
	}

}
void ipred_ang1_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, s16 pred_cache_16x16[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF
	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[9][2] = { { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) }, { (372), (2816) }, { (256), (4096) }, { (128), (8192) } };
	////#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	s16 dxy = ipred_dxdy[ipm - 3][0];
	int i, j, t_dx, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int x = 2147483647;
	int t_dxa, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int xa = 2147483647;

	for (j = 0; j < 64; j = j + 2)
	{
//#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			////#pragma HLS UNROLL
			if (i < w)
			{
				/* case x-line */
				/**************************** 1 ****************************/
				t_dx = (j + 1) * dxy >> 10;
				offset = ((j + 1) * dxy >> 5) - (t_dx << 5);
				x = i + t_dx;

				pn_n1 = x - 1;
				p = x;
				pn = x + 1;
				pn_p2 = x + 2;
				pos_max = w * 2 - 1;

				pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);

#if MIPF
				filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 1)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 2)
					temp_pel = (pel)((src_up[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_up[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
						src_up[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 3)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j][i] = temp_pel;
				//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#else

				pred_cache_16x16[j][i] = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + (src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
				//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#endif

				/**************************** 2 ****************************/
				t_dxa = (j + 2) * dxy >> 10;
				offseta = ((j + 2) * dxy >> 5) - (t_dxa << 5);
				xa = i + t_dxa;

				pn_n1a = xa - 1;
				pa = xa;
				pna = xa + 1;
				pn_p2a = xa + 2;
				pos_maxa = w * 2 - 1;

				pn_n1a = (pn_n1a < pos_maxa ? pn_n1a : pos_maxa);
				pa = (pa < pos_maxa ? pa : pos_maxa);
				pna = (pna < pos_maxa ? pna : pos_maxa);
				pn_p2a = (pn_p2a < pos_maxa ? pn_p2a : pos_maxa);

#if MIPF
				filter_idx = mipf_enable_flag ? ((j + 1) < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_extra_smooth[offseta][1] +
						src_up[pna + 3] * com_tbl_ipred_adi_extra_smooth[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_semi_sharp[offseta][1] +
						src_up[pna + 3] * com_tbl_ipred_adi_semi_sharp[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_up[pn_n1a + 3] * tbl_mc_c_coeff_hp[offseta][0] + src_up[pa + 3] * tbl_mc_c_coeff_hp[offseta][1] +
						src_up[pna + 3] * tbl_mc_c_coeff_hp[offseta][2] + src_up[pn_p2a + 3] * tbl_mc_c_coeff_hp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_up[pn_n1a + 3] * com_tbl_ipred_adi_bilinear[offseta][0] + src_up[pa + 3] * com_tbl_ipred_adi_bilinear[offseta][1] +
						src_up[pna + 3] * com_tbl_ipred_adi_bilinear[offseta][2] + src_up[pn_p2a + 3] * com_tbl_ipred_adi_bilinear[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j + 1][i] = temp_pel;
				//coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
#else

				pred_cache_16x16[j + 1][i] = ((src_up[pn_n1a + 3]) * (32 - offseta) + (src_up[pa + 3]) * (64 - offseta) + (src_up[pna + 3]) * (32 + offseta) + (src_up[pn_p2a + 3]) * offseta + 64) >> 7;
				//coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
#endif




			}
		}
	}

}
void ipred_ang2_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, s16 pred_cache_16x16[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF

	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[8][2] = { { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };
	////#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	s16 dxy = ipred_dxdy[ipm - 25][1];
	int i, j, t_dy, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int y = 2147483647;
	int t_dya, offseta;
	int pa, pna, pn_n1a, pn_p2a, pos_maxa;
	int ya = 2147483647;

	for (j = 0; j < 64; j = j + 2)
	{
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
//#pragma HLS UNROLL
			if (i < w)
			{
				/**************************** 1 ****************************/
				t_dy = (i + 1) * dxy >> 10;
				offset = ((i + 1) * dxy >> 5) - (t_dy << 5);
				y = j + t_dy;
				pn_n1 = y - 1;
				p = y;
				pn = y + 1;
				pn_p2 = y + 2;
				pos_max = h * 2 - 1;

				pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);


#if MIPF									
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_le[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_le[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_le[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_le[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
						src_le[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_le[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_le[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_le[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
						src_le[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_le[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j][i] = temp_pel;
				//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];

#else
				pred_cache_16x16[j][i] = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
				//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
#endif



				/**************************** 2 ****************************/
				t_dya = (i + 1) * dxy >> 10;
				offseta = ((i + 1) * dxy >> 5) - (t_dya << 5);
				ya = j + 1 + t_dya;
				pn_n1a = ya - 1;
				pa = ya;
				pna = ya + 1;
				pn_p2a = ya + 2;
				pos_maxa = h * 2 - 1;

				pn_n1a = (pn_n1a < pos_maxa ? pn_n1a : pos_maxa);
				pa = (pa < pos_maxa ? pa : pos_maxa);
				pna = (pna < pos_maxa ? pna : pos_maxa);
				pn_p2a = (pn_p2a < pos_maxa ? pn_p2a : pos_maxa);

#if MIPF									
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

				if (filter_idx == 0)
					temp_pel = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_extra_smooth[offseta][1] +
						src_le[pna + 3] * com_tbl_ipred_adi_extra_smooth[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_extra_smooth[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_semi_sharp[offseta][1] +
						src_le[pna + 3] * com_tbl_ipred_adi_semi_sharp[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_semi_sharp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_le[pn_n1a + 3] * tbl_mc_c_coeff_hp[offseta][0] + src_le[pa + 3] * tbl_mc_c_coeff_hp[offseta][1] +
						src_le[pna + 3] * tbl_mc_c_coeff_hp[offseta][2] + src_le[pn_p2a + 3] * tbl_mc_c_coeff_hp[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_le[pn_n1a + 3] * com_tbl_ipred_adi_bilinear[offseta][0] + src_le[pa + 3] * com_tbl_ipred_adi_bilinear[offseta][1] +
						src_le[pna + 3] * com_tbl_ipred_adi_bilinear[offseta][2] + src_le[pn_p2a + 3] * com_tbl_ipred_adi_bilinear[offseta][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1


				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j + 1][i] = temp_pel;
				//coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];

#else
				pred_cache_16x16[j + 1][i] = ((src_le[pn_n1a + 3]) * (32 - offseta) + (src_le[pa + 3]) * (64 - offseta) + (src_le[pna + 3]) * (32 + offseta) + (src_le[pn_p2a + 3]) * offseta + 64) >> 7;
				//coef_tmp_16x16[j + 1][i] = orgY[((j + 1) << 6) + i] - pred_cache_16x16[j + 1][i];
#endif


			}
		}
	}

}
void ipred_ang3_64_pred(U7 w, U7 h, pel src_le[128 + 3], pel src_up[128 + 3], U4 bit_depth, int ipm, s16 pred_cache_16x16[64][64]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF
	static const s16 com_tbl_ipred_adi_bilinear[32][4] =
	{
		/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
		{ 32, 64, 32,  0 },
		{ 31, 63, 33,  1 },
		{ 30, 62, 34,  2 },
		{ 29, 61, 35,  3 },
		{ 28, 60, 36,  4 },
		{ 27, 59, 37,  5 },
		{ 26, 58, 38,  6 },
		{ 25, 57, 39,  7 },
		{ 24, 56, 40,  8 },
		{ 23, 55, 41,  9 },
		{ 22, 54, 42, 10 },
		{ 21, 53, 43, 11 },
		{ 20, 52, 44, 12 },
		{ 19, 51, 45, 13 },
		{ 18, 50, 46, 14 },
		{ 17, 49, 47, 15 },
		{ 16, 48, 48, 16 },
		{ 15, 47, 49, 17 },
		{ 14, 46, 50, 18 },
		{ 13, 45, 51, 19 },
		{ 12, 44, 52, 20 },
		{ 11, 43, 53, 21 },
		{ 10, 42, 54, 22 },
		{ 9, 41, 55, 23 },
		{ 8, 40, 56, 24 },
		{ 7, 39, 57, 25 },
		{ 6, 38, 58, 26 },
		{ 5, 37, 59, 27 },
		{ 4, 36, 60, 28 },
		{ 3, 35, 61, 29 },
		{ 2, 34, 62, 30 },
		{ 1, 33, 63, 31 },
	};

	static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
	{
		/* best for CfP extra smooth */
		{ 23, 82, 21, 2 },
		{ 21, 82, 23, 2 },
		{ 19, 82, 25, 2 },
		{ 18, 81, 27, 2 },
		{ 16, 80, 30, 2 },
		{ 15, 79, 32, 2 },
		{ 13, 78, 35, 2 },
		{ 12, 77, 37, 2 },
		{ 11, 75, 40, 2 },
		{ 10, 74, 42, 2 },
		{ 9, 72, 45, 2 },
		{ 8, 70, 47, 3 },
		{ 7, 68, 50, 3 },
		{ 6, 67, 52, 3 },
		{ 6, 64, 54, 4 },
		{ 5, 62, 57, 4 },
		{ 4, 60, 60, 4 },
		{ 4, 57, 62, 5 },
		{ 4, 54, 64, 6 },
		{ 3, 52, 67, 6 },
		{ 3, 50, 68, 7 },
		{ 3, 47, 70, 8 },
		{ 2, 45, 72, 9 },
		{ 2, 42, 74, 10 },
		{ 2, 40, 75, 11 },
		{ 2, 37, 77, 12 },
		{ 2, 35, 78, 13 },
		{ 2, 32, 79, 15 },
		{ 2, 30, 80, 16 },
		{ 2, 27, 81, 18 },
		{ 2, 25, 82, 19 },
		{ 2, 23, 82, 21 },
	};

	static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
	{
		// semi-sharp
		{ 11, 106, 12, -1 },
		{ 9, 105, 15, -1 },
		{ 7, 105, 18, -2 },
		{ 6, 103, 21, -2 },
		{ 4, 102, 25, -3 },
		{ 2, 100, 29, -3 },
		{ 1, 99, 32, -4 },
		{ 0, 96, 36, -4 },
		{ -1, 94, 39, -4 },
		{ -2, 92, 43, -5 },
		{ -3, 89, 47, -5 },
		{ -4, 86, 51, -5 },
		{ -4, 83, 55, -6 },
		{ -5, 80, 59, -6 },
		{ -5, 77, 62, -6 },
		{ -6, 74, 66, -6 },
		{ -6, 70, 70, -6 },
		{ -6, 66, 74, -6 },
		{ -6, 62, 77, -5 },
		{ -6, 59, 80, -5 },
		{ -6, 55, 83, -4 },
		{ -5, 51, 86, -4 },
		{ -5, 47, 89, -3 },
		{ -5, 43, 92, -2 },
		{ -4, 39, 94, -1 },
		{ -4, 36, 96, 0 },
		{ -4, 32, 99, 1 },
		{ -3, 29, 100, 2 },
		{ -3, 25, 102, 4 },
		{ -2, 21, 103, 6 },
		{ -2, 18, 105, 7 },
		{ -1, 15, 105, 9 },
	};

	static const s16 tbl_mc_c_coeff_hp[32][4] =
	{
		{ 0, 64, 0, 0 },
		{ -1, 63,  2,  0 },
		{ -2, 62,  4,  0 },
		{ -2, 60,  7,  -1 },
		{ -2, 58, 10, -2 },
		{ -3, 57, 12, -2 },
		{ -4, 56, 14, -2 },
		{ -4, 55, 15, -2 },
		{ -4, 54, 16, -2 },
		{ -5, 53, 18, -2 },
		{ -6, 52, 20, -2 },
		{ -6, 49, 24, -3 },
		{ -6, 46, 28, -4 },
		{ -5, 44, 29, -4 },
		{ -4, 42, 30, -4 },
		{ -4, 39, 33, -4 },
		{ -4, 36, 36, -4 },
		{ -4, 33, 39, -4 },
		{ -4, 30, 42, -4 },
		{ -4, 29, 44, -5 },
		{ -4, 28, 46, -6 },
		{ -3, 24, 49, -6 },
		{ -2, 20, 52, -6 },
		{ -2, 18, 53, -5 },
		{ -2, 16, 54, -4 },
		{ -2, 15, 55, -4 },
		{ -2, 14, 56, -4 },
		{ -2, 12, 57, -3 },
		{ -2, 10, 58, -2 },
		{ -1,  7, 60, -2 },
		{ 0,  4, 62, -2 },
		{ 0,  2, 63, -1 },
	};
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
	//pel temp_pel = (pel)0;
#endif

	static const s16 ipred_dxdy[11][2] = { { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) } };
	////#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	s16 dxy0 = ipred_dxdy[ipm - 13][0];
	s16 dxy1 = ipred_dxdy[ipm - 13][1];
	s16 dxya = ipred_dxdy[ipm - 13][0];
	s16 dxyb = ipred_dxdy[ipm - 13][1];
	int i, j;
	int offset, offset_x, offset_y, xx, yy, t_dx, t_dy;
	int p, pn, pn_n1, pn_p2;
	pel temp_pel = 0;
	int offseta, offset_xa, offset_ya, xxa, yya, t_dxa, t_dya;
	int pa, pna, pn_n1a, pn_p2a;
	pel temp_pela = 0;
	pel temp_pelax = 0;
	pel temp_pelay = 0;
	pel temp_pel0 = 0;
	pel temp_pel1 = 0;
	int xxyya, xxyy;

	for (j = 0; j < 64; j = j + 2)
	{
		////#pragma HLS LOOP_TRIPCOUNT max=64
//#pragma HLS PIPELINE
		for (i = 0; i < 64; i++)
		{
			////#pragma HLS UNROLL
			if (i < w)
			{
				for (int k = 1; k <= 2; k++)
				{
					if (k == 1)
					{
						t_dx = (j + k) * dxy0 >> 10;
						offset_x = ((j + k) * dxy0 >> 5) - (t_dx << 5);
						t_dy = (i + 1) * dxy1 >> 10;
						offset_y = ((i + 1) * dxy1 >> 5) - (t_dy << 5);
						t_dy = -t_dy;
						yy = j + t_dy;
					}
					else
					{
						t_dx = (j + k) * dxya >> 10;
						offset_x = ((j + k) * dxya >> 5) - (t_dx << 5);
						t_dy = (i + 1) * dxyb >> 10;
						offset_y = ((i + 1) * dxyb >> 5) - (t_dy << 5);
						t_dy = -t_dy;
						yy = j + t_dy + 1;
					}
					t_dx = -t_dx;
					xx = i + t_dx;

					offset = (yy <= -1) ? offset_x : offset_y;
					xxyy = (yy <= -1) ? xx : yy;
					pn_n1 = xxyy + 1;
					p = xxyy;
					pn = xxyy - 1;
					pn_p2 = xxyy - 2;

					pn_n1 += 3;
					p += 3;
					pn += 3;
					pn_p2 += 3;

					int src_p, src_pn, src_pn_n1, src_pn_p2;
					if (yy <= -1)
					{
						src_pn_n1 = src_up[pn_n1];
						src_p = src_up[p];
						src_pn = src_up[pn];
						src_pn_p2 = src_up[pn_p2];
					}
					else
					{
						src_pn_n1 = src_le[pn_n1];
						src_p = src_le[p];
						src_pn = src_le[pn];
						src_pn_p2 = src_le[pn_p2];
					}

#if MIPF
					if ((ipm < 12) || ((ipm >= 13) && (ipm <= 17)) || ((ipm >= 34) && (ipm <= 50))) {
						filter_idx = mipf_enable_flag ? ((j + k - 1) < td ? is_small + 1 : is_small) : 3;
					}
					else if (((ipm >= 18) && (ipm <= 23)) || ((ipm >= 25) && (ipm <= 32)) || ((ipm >= 51) && (ipm <= 65))) {
						filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
					}

					if (filter_idx == 0)
						temp_pel = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
							src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 1)
						temp_pel = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
							src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 2)
						temp_pel = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
							src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
					else if (filter_idx == 3)
						temp_pel = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
							src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
							filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

					temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
#else
					temp_pel0 = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + ((
						int)src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
					temp_pel1 = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
					temp_pel = (yy <= -1) ? temp_pel0 : temp_pel1;
#endif
					pred_cache_16x16[j + k - 1][i] = temp_pel;
					//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
				}
			}
		}
	}
}
#endif

void init_inter_data_64(MD_COM_MODE* mod_info_curr_ptr)
{
//#pragma HLS array_partition variable=mod_info_curr_ptr->mv complete dim=0
//#pragma HLS array_partition variable=mod_info_curr_ptr->mvd complete dim=0
//#pragma HLS array_partition variable=mod_info_curr_ptr->affine_mv complete dim=0
//#pragma HLS array_partition variable=mod_info_curr_ptr->affine_mvd complete dim=0
//#pragma HLS array_partition variable=mod_info_curr_ptr->num_nz complete dim=0

	mod_info_curr_ptr->skip_idx = 0;
	mod_info_curr_ptr->smvd_flag = 0;

	//	for (int i = 0; i < 2; i++) {
	//
	////#pragma HLS UNROLL
	//		for (int j = 0; j < 2; j++) {
	//
	////#pragma HLS UNROLL
	//			mod_info_curr_ptr->mv[i][j] = 0;
	//			mod_info_curr_ptr->mvd[i][j] = 0;
	//		}
	//		mod_info_curr_ptr->refi[i] = 0;
	//		//add by xujch
	//	}
	//
	//	for (int m = 0; m < 4; m++) {
	//
	////#pragma HLS UNROLL
	//		for (int n = 0; n < 3; n++) {
	//
	////#pragma HLS UNROLL
	//			mod_info_curr_ptr->num_nz[m][n] = 0;
	//		}
	//	}
	//
	//
	for (int c = 0; c < 2; c++) {
//#pragma HLS UNROLL
		for (int d = 0; d < 4; d++) {
//#pragma HLS UNROLL
			for (int e = 0; e < 2; e++) {
//#pragma HLS UNROLL
				mod_info_curr_ptr->affine_mv[c][d][e] = 0;
				mod_info_curr_ptr->affine_mvd[c][d][e] = 0;
			}
		}
		//add by xujch
	}
}

static void com_mc_l_00_64(pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, pel pred[64][64], U7 w_l, U7 h_u, int w, int h, int bHpFilter)
{
	int i;
	int j;
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

//#pragma HLS loop_tripcount max = 64
//#pragma HLS PIPELINE
			for (j = 0; j < 64; j++) {
//#pragma HLS UNROLL
				if (j < w)
					pred[i + h_u][j + w_l] = ref[j + offset_ref];
			}
			offset_pred += s_pred;
			offset_ref += s_ref;
		}
	}
}

static void com_mc_l_n0_0n_64(pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[64][64], U7 w_l, U7 h_u, int w, int h, U4 bit_depth, U1 bHpFilter, U1 b_n0, const s16 tbl_mc_l_coeff_all_2[2][16][8])
{
	//s16 tbl_mc_l_coeff_all_2[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };

	U4 dxy;
	int offset_ref = 0;
	int offset_pred = 0;
	u8 i;
	u8 j;
	int stride = s_ref;
	s32 pt, index;
	if (bHpFilter)
	{
		if (b_n0) {
			dxy = gmv_x & 15;
			offset_ref += (gmv_y >> 4) * s_ref + (gmv_x >> 4) - 3;
			stride = 1;
		}
		else
		{
			dxy = gmv_y & 15;
			offset_ref += ((gmv_y >> 4) - 3) * s_ref + (gmv_x >> 4);
		}
	}
	else
	{
		if (b_n0) {
			dxy = gmv_x & 0x3;
			offset_ref += (gmv_y >> 2) * s_ref + (gmv_x >> 2) - 3;
			stride = 1;
		}
		else {
			dxy = gmv_y & 0x3;
			offset_ref += ((gmv_y >> 2) - 3) * s_ref + (gmv_x >> 2);
		}
	}

	{

		for (i = 0; i < h; i++) {

//#pragma HLS loop_tripcount max = 32
			for (j = 0; j < w; j++) {

//#pragma HLS loop_tripcount max = 32

				index = j + offset_ref;
				pt = 0;
				for (u8 k = 0; k < 8; k++)
				{
//#pragma HLS UNROLL
					pt += tbl_mc_l_coeff_all_2[bHpFilter][dxy][k] * ref[index];
					index += stride;
				}


				pt = (pt + (1 << 5)) >> 6;

				pred[i + h_u][j + w_l] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_ref += s_ref;
			offset_pred += s_pred;
		}
	}
}

static void com_mc_l_nn_64(pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, pel pred[64][64], U7 w_l, U7 h_u, int w, int h, U4 bit_depth,
	int bHpFilter)
{
	static s32 buf[64 + 8][128 >> 1];
	static s16 tbl_mc_l_coeff_all_3[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (57), (19), ((s16)(-7)), (3), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-7)), (19), (57), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((s16)(-3)), (63), (4), ((s16)(-2)), (1), 0 }, { ((s16)(-1)), (2), ((s16)(-5)), (62), (8), ((s16)(-3)), (1), 0 }, { ((s16)(-1)), (3), ((s16)(-8)), (60), (13), ((s16)(-4)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-10)), (58), (17), ((s16)(-5)), (1), 0 }, { ((s16)(-1)), (4), ((s16)(-11)), (52), (26), ((s16)(-8)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-9)), (47), (31), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (45), (34), ((s16)(-10)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-11)), (40), (40), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (34), (45), ((s16)(-11)), (4), ((s16)(-1)) }, { ((s16)(-1)), (4), ((s16)(-10)), (31), (47), ((s16)(-9)), (3), ((s16)(-1)) }, { ((s16)(-1)), (3), ((s16)(-8)), (26), (52), ((s16)(-11)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-5)), (17), (58), ((s16)(-10)), (4), ((s16)(-1)) }, { 0, (1), ((s16)(-4)), (13), (60), ((s16)(-8)), (3), ((s16)(-1)) }, { 0, (1), ((s16)(-3)), (8), (62), ((s16)(-5)), (2), ((s16)(-1)) }, { 0, (1), ((s16)(-2)), (4), (63), ((s16)(-3)), (1), 0 } } };


//#pragma HLS ARRAY_PARTITION variable=buf block factor=72 dim=1 partition
	int dx;
	int dy;
	int offset_ref = 0;
	int offset_pred = 0;
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = (1 << (shift2 - 1));
	s32 ref_temp[8];
	u8 i, j, k;

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


//#pragma HLS ARRAY_PARTITION variable=ref_temp complete dim=0
	{

		//b = buf;
		for (i = 0; i < h + 7; i++) {

//#pragma HLS loop_tripcount max = 71
			ref_temp[0] = ref[offset_ref];
			ref_temp[1] = ref[1 + offset_ref];
			ref_temp[2] = ref[2 + offset_ref];
			ref_temp[3] = ref[3 + offset_ref];
			ref_temp[4] = ref[4 + offset_ref];
			ref_temp[5] = ref[5 + offset_ref];
			ref_temp[6] = ref[6 + offset_ref];
			for (j = 0; j < w; j++) {

//#pragma HLS loop_tripcount max = 64
				ref_temp[7] = ref[j + 7 + offset_ref];
				//buf[i][j] = ((s16)(((tbl_mc_l_coeff_all_3[bHpFilter][dx][0]) * (ref_temp[0]) + (
				//	tbl_mc_l_coeff_all_3[bHpFilter][dx][1]) * (ref_temp[1]) + (tbl_mc_l_coeff_all_3[bHpFilter][dx][2]) * (
				//		ref_temp[2]) + (tbl_mc_l_coeff_all_3[bHpFilter][dx][3]) * (ref_temp[3]) + (
				//			tbl_mc_l_coeff_all_3[bHpFilter][dx][4]) * (ref_temp[4]) + (tbl_mc_l_coeff_all_3[bHpFilter][dx][5]) * (
				//				ref_temp[5]) + (tbl_mc_l_coeff_all_3[bHpFilter][dx][6]) * (ref_temp[6]) + (
				//					tbl_mc_l_coeff_all_3[bHpFilter][dx][7]) * (ref_temp[7]) + add1) >> shift1));

				buf[i][j] = 0;
				for (k = 0; k < 8; k++)
				{
//#pragma HLS UNROLL
					buf[i][j] += tbl_mc_l_coeff_all_3[bHpFilter][dx][k] * ref_temp[k];
				}
				buf[i][j] = (buf[i][j] + add1) >> shift1;

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

//#pragma HLS loop_tripcount max = 64
			for (j = 0; j < w; j++) {

//#pragma HLS loop_tripcount max = 64
				//pt = ((tbl_mc_l_coeff_all_3[bHpFilter][dy][0]) * (buf[i][j]) + (tbl_mc_l_coeff_all_3[bHpFilter][dy][1])
				//	* (buf[i + 1][j]) + (tbl_mc_l_coeff_all_3[bHpFilter][dy][2]) * (buf[i + 2][j]) + (
				//		tbl_mc_l_coeff_all_3[bHpFilter][dy][3]) * (buf[i + 3][j]) + (tbl_mc_l_coeff_all_3[bHpFilter][dy][4]) * (
				//			buf[i + 4][j]) + (tbl_mc_l_coeff_all_3[bHpFilter][dy][5]) * (buf[i + 5][j]) + (
				//				tbl_mc_l_coeff_all_3[bHpFilter][dy][6]) * (buf[i + 6][j]) + (tbl_mc_l_coeff_all_3[bHpFilter][dy][7]) * (
				//					buf[i + 7][j]) + add2) >> shift2;

				pt = 0;
				for (k = 0; k < 8; k++)
				{
//#pragma HLS UNROLL
					pt += tbl_mc_l_coeff_all_3[bHpFilter][dy][k] * buf[i + k][j];
				}
				pt = (pt + add2) >> shift2;

				pred[i + h_u][j + w_l] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
			offset_pred += s_pred;
		}
	}
}

void com_tbl_mc_l_1_64(int ori_mv_x, int ori_mv_y, pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[64][64],
	U7 w_l, U7 h_u, U7 w, U7 h, U4 bit_depth, int bHpFilter)
{
	//add by luke
	int dx;
	int dy;

	if (bHpFilter == 1)
	{
		dx = ori_mv_x & 0xF;
		dy = ori_mv_y & 0xF;
	}
	else
	{
		dx = ori_mv_x & 0x3;
		dy = ori_mv_y & 0x3;
	}

	static s16 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };


	if (dx == 0 && dy == 0) {
		com_mc_l_00_64(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w_l, h_u, w, h, bHpFilter);
	}
	else if (dx != 0 && dy != 0) {
		com_mc_l_nn_64(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w_l, h_u, w, h, bit_depth, bHpFilter);
	}
	else
		com_mc_l_n0_0n_64(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w_l, h_u, w, h, bit_depth, bHpFilter, (dx != 0 && dy == 0), tbl_mc_l_coeff_all);
}

void com_tbl_mc_l_1_64_ME(int ori_mv_x, int ori_mv_y, pel refwin_md[88][88], int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[64][64],
	U7 w_l, U7 h_u, U7 w, U7 h, U4 bit_depth, int bHpFilter)
{
	//add by luke
	int dx;
	int dy;

	if (bHpFilter == 1)
	{
		dx = ori_mv_x & 0xF;
		dy = ori_mv_y & 0xF;
	}
	else
	{
		dx = ori_mv_x & 0x3;
		dy = ori_mv_y & 0x3;
	}

	static s16 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };

	static pel ref[88 * 88];
	for (int i = 0; i < 88; i++)
	{
		for (int j = 0; j < 88; j++)
		{
			ref[i * 88 + j] = refwin_md[i][j];
		}
	}

	if (dx == 0 && dy == 0) {
		com_mc_l_00_64(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w_l, h_u, w, h, bHpFilter);
	}
	else if (dx != 0 && dy != 0) {
		com_mc_l_nn_64(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w_l, h_u, w, h, bit_depth, bHpFilter);
	}
	else
		com_mc_l_n0_0n_64(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w_l, h_u, w, h, bit_depth, bHpFilter, (dx != 0 && dy == 0), tbl_mc_l_coeff_all);
}

void refwin_copy_line_64(int i, pel refwin_md[512 * 512], U6 x, U6 y,
	pel refwin1[8][72], pel refwin2[8][72], pel refwin3[8][72], pel refwin4[8][72], pel refwin5[8][72],
	pel refwin6[8][72], pel refwin7[8][72], pel refwin8[8][72], pel refwin9[8][72])
{
//#pragma HLS INLINE OFF
	static pel ref_line[512];
//#pragma HLS array_partition variable=ref_line complete dim=0

	for (int j = 0; j < 512; j++)
	{
//#pragma HLS UNROLL
		ref_line[j] = refwin_md[y * 512 + j];
	}

	for (int k = 0; k < 72; k++)
	{
//#pragma HLS UNROLL
		if (i < 8)
		{
			refwin1[i][k] = ref_line[k + x];
		}
		else if (i < 16)
		{
			refwin2[i - 8][k] = ref_line[k + x];
		}
		else if (i < 24)
		{
			refwin3[i - 16][k] = ref_line[k + x];
		}
		else if (i < 32)
		{
			refwin4[i - 24][k] = ref_line[k + x];
		}
		else if (i < 40)
		{
			refwin5[i - 32][k] = ref_line[k + x];
		}
		else if (i < 48)
		{
			refwin6[i - 40][k] = ref_line[k + x];
		}
		else if (i < 56)
		{
			refwin7[i - 48][k] = ref_line[k + x];
		}
		else if (i < 64)
		{
			refwin8[i - 56][k] = ref_line[k + x];
		}
		else
		{
			refwin9[i - 64][k] = ref_line[k + x];
		}
	}
}

static void refwin_copy_64(U6 ref_offsetx, U6 ref_offsety, pel* refwin_md, pel refwin1[8][72],
	pel refwin2[8][72], pel refwin3[8][72], pel refwin4[8][72], pel refwin5[8][72],
	pel refwin6[8][72], pel refwin7[8][72], pel refwin8[8][72], pel refwin9[8][72])
{
//#pragma HLS INLINE OFF
	for (int i = 0; i < 72; i++)
	{
//#pragma HLS PIPELINE
		refwin_copy_line_64(i, refwin_md, ref_offsetx, i + ref_offsety,
			refwin1, refwin2, refwin3, refwin4, refwin5,
			refwin6, refwin7, refwin8, refwin9);
	}
}

static void inter_hor_64(S32 hor_buf[8][64], int dx, pel refwin[8][72], int add1, int shift1)
{
	static S8 tbl_mc_l_coeff[4][8] =
	{
		{ 0, 0, 0, 64, 0, 0, 0, 0 },
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0

	for (int i = 0; i < 8; i++)
	{
//#pragma HLS PIPELINE
		for (int j = 0; j < 64; j++)
		{
//#pragma HLS UNROLL
			hor_buf[i][j] = (MAC_8TAP(tbl_mc_l_coeff[dx], refwin[i], j) + add1) >> shift1;
		}
	}
}

void construc_verbuf1_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8];
		ver_buf[k + 8] = hor_buf2[k][i * 8];
		ver_buf[k + 16] = hor_buf3[k][i * 8];
		ver_buf[k + 24] = hor_buf4[k][i * 8];
		ver_buf[k + 32] = hor_buf5[k][i * 8];
		ver_buf[k + 40] = hor_buf6[k][i * 8];
		ver_buf[k + 48] = hor_buf7[k][i * 8];
		ver_buf[k + 56] = hor_buf8[k][i * 8];
		ver_buf[k + 64] = hor_buf9[k][i * 8];
	}
}

void construc_verbuf2_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 1];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 1];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 1];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 1];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 1];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 1];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 1];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 1];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 1];
	}
}

void construc_verbuf3_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 2];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 2];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 2];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 2];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 2];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 2];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 2];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 2];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 2];
	}
}

void construc_verbuf4_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 3];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 3];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 3];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 3];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 3];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 3];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 3];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 3];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 3];
	}
}

void construc_verbuf5_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 4];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 4];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 4];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 4];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 4];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 4];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 4];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 4];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 4];
	}
}

void construc_verbuf6_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 5];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 5];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 5];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 5];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 5];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 5];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 5];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 5];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 5];
	}
}

void construc_verbuf7_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 6];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 6];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 6];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 6];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 6];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 6];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 6];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 6];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 6];
	}
}

void construc_verbuf8_64(int i, S32 ver_buf[72], S32 hor_buf1[8][64], S32 hor_buf2[8][64], S32 hor_buf3[8][64],
	S32 hor_buf4[8][64], S32 hor_buf5[8][64], S32 hor_buf6[8][64], S32 hor_buf7[8][64], S32 hor_buf8[8][64], S32 hor_buf9[8][64])
{
//#pragma HLS INLINE
	for (int k = 0; k < 8; k++)
	{
		ver_buf[k] = hor_buf1[k][i * 8 + 7];
		ver_buf[k + 8] = hor_buf2[k][i * 8 + 7];
		ver_buf[k + 16] = hor_buf3[k][i * 8 + 7];
		ver_buf[k + 24] = hor_buf4[k][i * 8 + 7];
		ver_buf[k + 32] = hor_buf5[k][i * 8 + 7];
		ver_buf[k + 40] = hor_buf6[k][i * 8 + 7];
		ver_buf[k + 48] = hor_buf7[k][i * 8 + 7];
		ver_buf[k + 56] = hor_buf8[k][i * 8 + 7];
		ver_buf[k + 64] = hor_buf9[k][i * 8 + 7];
	}
}

void com_tbl_mc_l_1_64_t(int ori_mv_x, int ori_mv_y, pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[64][64],
	U7 w_l, U7 h_u, U7 w, U7 h, U4 bit_depth, int bHpFilter, pel RefWindowLCU_t[512 * 512])
{
//#pragma HLS array_partition variable=pred complete dim=1
//#pragma HLS array_partition variable=pred cyclic factor=8 dim=2
//#pragma HLS array_partition variable=RefWindowLCU_t cyclic factor=512 dim=1
	int         i, j;
	int dx;
	int dy;
	if (bHpFilter == 1)
	{
		dx = ori_mv_x & 0xF;
		dy = ori_mv_y & 0xF;
	}
	else
	{
		dx = ori_mv_x & 0x3;
		dy = ori_mv_y & 0x3;
	}

	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = (1 << (shift2 - 1));

	static S8 tbl_mc_l_coeff[4][8] =
	{
		{ 0, 0, 0, 64, 0, 0, 0, 0 },
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0

	static S32 hor_buf1[8][64];
	static S32 hor_buf2[8][64];
	static S32 hor_buf3[8][64];
	static S32 hor_buf4[8][64];
	static S32 hor_buf5[8][64];
	static S32 hor_buf6[8][64];
	static S32 hor_buf7[8][64];
	static S32 hor_buf8[8][64];
	static S32 hor_buf9[8][64];
//#pragma HLS array_partition variable=hor_buf1 complete dim=0
//#pragma HLS array_partition variable=hor_buf2 complete dim=0
//#pragma HLS array_partition variable=hor_buf3 complete dim=0
//#pragma HLS array_partition variable=hor_buf4 complete dim=0
//#pragma HLS array_partition variable=hor_buf5 complete dim=0
//#pragma HLS array_partition variable=hor_buf6 complete dim=0
//#pragma HLS array_partition variable=hor_buf7 complete dim=0
//#pragma HLS array_partition variable=hor_buf8 complete dim=0
//#pragma HLS array_partition variable=hor_buf9 complete dim=0

	static pel refwin1[8][72];
	static pel refwin2[8][72];
	static pel refwin3[8][72];
	static pel refwin4[8][72];
	static pel refwin5[8][72];
	static pel refwin6[8][72];
	static pel refwin7[8][72];
	static pel refwin8[8][72];
	static pel refwin9[8][72];
//#pragma HLS array_partition variable=refwin1 complete dim=2
//#pragma HLS array_partition variable=refwin2 complete dim=2
//#pragma HLS array_partition variable=refwin3 complete dim=2
//#pragma HLS array_partition variable=refwin4 complete dim=2
//#pragma HLS array_partition variable=refwin5 complete dim=2
//#pragma HLS array_partition variable=refwin6 complete dim=2
//#pragma HLS array_partition variable=refwin7 complete dim=2
//#pragma HLS array_partition variable=refwin8 complete dim=2
//#pragma HLS array_partition variable=refwin9 complete dim=2

	refwin_copy_64((gmv_x >> 2) - 3, (gmv_y >> 2) - 3, RefWindowLCU_t, refwin1, refwin2, refwin3, refwin4,
		refwin5, refwin6, refwin7, refwin8, refwin9);

	inter_hor_64(hor_buf1, dx, refwin1, add1, shift1);
	inter_hor_64(hor_buf2, dx, refwin2, add1, shift1);
	inter_hor_64(hor_buf3, dx, refwin3, add1, shift1);
	inter_hor_64(hor_buf4, dx, refwin4, add1, shift1);
	inter_hor_64(hor_buf5, dx, refwin5, add1, shift1);
	inter_hor_64(hor_buf6, dx, refwin6, add1, shift1);
	inter_hor_64(hor_buf7, dx, refwin7, add1, shift1);
	inter_hor_64(hor_buf8, dx, refwin8, add1, shift1);
	inter_hor_64(hor_buf9, dx, refwin9, add1, shift1);

	static S32 ver_buf1[72];
	static S32 ver_buf2[72];
	static S32 ver_buf3[72];
	static S32 ver_buf4[72];
	static S32 ver_buf5[72];
	static S32 ver_buf6[72];
	static S32 ver_buf7[72];
	static S32 ver_buf8[72];
//#pragma HLS array_partition variable=ver_buf1 complete dim=0
//#pragma HLS array_partition variable=ver_buf2 complete dim=0
//#pragma HLS array_partition variable=ver_buf3 complete dim=0
//#pragma HLS array_partition variable=ver_buf4 complete dim=0
//#pragma HLS array_partition variable=ver_buf5 complete dim=0
//#pragma HLS array_partition variable=ver_buf6 complete dim=0
//#pragma HLS array_partition variable=ver_buf7 complete dim=0
//#pragma HLS array_partition variable=ver_buf8 complete dim=0

	for (i = 0; i < 8; i++)
	{
//#pragma HLS PIPELINE

		construc_verbuf1_64(i, ver_buf1, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf2_64(i, ver_buf2, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf3_64(i, ver_buf3, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf4_64(i, ver_buf4, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf5_64(i, ver_buf5, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf6_64(i, ver_buf6, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf7_64(i, ver_buf7, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);
		construc_verbuf8_64(i, ver_buf8, hor_buf1, hor_buf2, hor_buf3, hor_buf4, hor_buf5, hor_buf6, hor_buf7, hor_buf8, hor_buf9);

		for (j = 0; j < 64; j++)
		{
//#pragma HLS UNROLL
			int pt1 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf1, j) + add2) >> shift2;
			int pt2 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf2, j) + add2) >> shift2;
			int pt3 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf3, j) + add2) >> shift2;
			int pt4 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf4, j) + add2) >> shift2;
			int pt5 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf5, j) + add2) >> shift2;
			int pt6 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf6, j) + add2) >> shift2;
			int pt7 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf7, j) + add2) >> shift2;
			int pt8 = (MAC_8TAP(tbl_mc_l_coeff[dy], ver_buf8, j) + add2) >> shift2;
			pred[j][i * 8 + 0] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt1);
			pred[j][i * 8 + 1] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt2);
			pred[j][i * 8 + 2] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt3);
			pred[j][i * 8 + 3] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt4);
			pred[j][i * 8 + 4] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt5);
			pred[j][i * 8 + 5] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt6);
			pred[j][i * 8 + 6] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt7);
			pred[j][i * 8 + 7] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt8);
		}
	}
}



void com_mc_fetch_64(U7 w, U7 h, pel* refpic_y, MD_COM_MODE* mod_info_curr, U2 channel, U4 bit_depth, U13 pic_width,
	U13 pic_height, u8 curr_mvr, S14 CtrPos[2][2][2], pel pred_cache[64][64], pel RefWindowLCU_t[512 * 512])
{
	//int scup = mod_info_curr->scup;
	int x = mod_info_curr->x_pos;
	int y = mod_info_curr->y_pos;
	int pic_w = pic_width;
	int pic_h = pic_height;
	S3* refi = mod_info_curr->refi;
	int pred_stride = mod_info_curr->cu_width;
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

			//add by luke
			//com_tbl_mc_l_1_64(mod_info_curr->mv[REFP_0][0], mod_info_curr->mv[REFP_0][1], refpic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l, pred_stride, pred_cache, 0, 0, w, h, bit_depth, 0);
			com_tbl_mc_l_1_64_t(mod_info_curr->mv[REFP_0][0], mod_info_curr->mv[REFP_0][1], refpic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l, pred_stride, pred_cache, 0, 0, w, h, bit_depth, 0,
				RefWindowLCU_t);
		}
	}
}

void diff_16b_64(U7 w, U7 h, s16* src1, s16* src1_cp, s16 src2[64][64], s16 src2_cp[64][64], int s_src1, int s_src2, int s_diff, s16 diff[64][64])
{

	U8 i;
	U8 j;
	for (i = 0; i < 64; i++) {
#pragma HLS PIPELINE II=4
		for (j = 0; j < 64; j++) {
			if (j < w && i < h) {
				diff[i][j] = ((s16)((src1[j + 64*i]) - (src2[i][j])));
			}
			src1_cp[j + 64*i] = src1[j + 64*i];
			src2_cp[i][j] = src2[i][j];
		}
	}
}

void enc_diff_16b_64(U3 log2w, U3 log2h, s16* src1, s16* src1_cp, s16 src2[64][64], s16 src2_cp[64][64], int s_src1, int s_src2, int s_diff, s16 diff[64][64])
{

//#pragma HLS INLINE off
	int x = log2w;
	int y = log2h;
	U7 w = 1 << log2w;
	U7 h = 1 << log2h;
	{
		diff_16b_64(w,h,src1, src1_cp, src2, src2_cp, s_src1, s_src2, s_diff, diff);
	}
}

s64 enc_ssd_16b_1_64(U3 w_log2, U3 h_log2, s16 src1[64][64], s16* src2, int s_src1, int s_src2, U4 bit_depth)
{
	int offset2 = 0;
//#pragma HLS INLINE off
	const int shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	U8 i, j;
	int diff;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
		for (j = 0; j < 64; j++) {
			if (j < w) {
				diff = (/*(int)*/src1[i][j]) - (/*(int)*/src2[64*i + j]);
				ssd += ((diff * diff >> shift));
			}
		}
		//offset2 += s_src2;
	}
	return ssd;
	//add by xujch
}
static int enc_eco_abs_mvd_est(U16 sym)
{
	U5 enc_est = 0;
	U5 exp_golomb_order = 0;
	// 0, 1, 2
	if (sym < (3)) {
		if (sym == (0)) {
			enc_est++;
		}
		else if (sym == (1)) {
			enc_est += 2;
		}
		else if (sym == (2)) {
			enc_est += 3;
		}
	}
	else {
		U1 offset;
		sym -= (3);
		offset = ((sym & (1)));
		enc_est += 3;
		enc_est++;
		sym = (sym - (offset)) >> 1;
		// exp_golomb part
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else //if (sym >= (1023) && sym < (2047)) 
		{
			exp_golomb_order = 20;
		}
		//else if (sym >= (2047) && sym < (4095)) {
		//	exp_golomb_order = 11;
		//}
		enc_est += exp_golomb_order;
		//enc_est+=sbac_write_unary_sym_ep_est(exp_golomb_order);
		//enc_est+=sbac_encode_bins_ep_msb_est(exp_golomb_order);
	}
	return enc_est;
}

int enc_eco_cbf_est_64(int num_nz[4][3], u8 pred_mode,
	S8 ipm[4][2], U2 tree_status, U3 cu_width_log2, U3 cu_height_log2)
{
	U4 enc_est = 0;
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 ctp_zero_flag = !is_cu_nz(num_nz);
	/* code allcbf */
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				enc_est++;
				if (ctp_zero_flag) {
					return 0;
				}
			}
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
		}
		if (num_nz[0][1] + num_nz[0][2] == 0 && tree_status == TREE_LC) {
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;

		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
			}
		}
	}
	return enc_est;
}

static int encode_intra_dir_est(u8 ipm, u8 mpm[2])
{
	U3 enc_est = 0;
	S8 ipm_code = (ipm) == (mpm[0]) ? -2 : (((mpm[1]) == (ipm) ? -1 : (((ipm) < (mpm[0]) ? (ipm) : (((ipm) < (mpm[1]) ? (ipm)-1 : (ipm)-2))))));
	if (ipm_code < 0) {
		enc_est += 2;
	}
	else {
		enc_est += 6;
	}
	return enc_est;
}

static int encode_umve_idx_est(int umve_idx)
{
	U3 enc_est;
	enc_est = 0;
	if (umve_idx < 20)
		enc_est++;
	else
	{
		enc_est += 2;


	}
	if (umve_idx < 4 || (umve_idx >= 20 && umve_idx < 24))
		enc_est++;
	else
	{
		enc_est += 2;


	}

	enc_est += 2;
	return enc_est;
}

static int sbac_write_truncate_unary_sym_est(U8 sym, U8 max_num)
{

//#pragma HLS INLINE
	U8 enc_est;
	enc_est = (max_num - (1) > sym + (1) ? sym + (1) : max_num - (1));
	return enc_est;
}

static int encode_skip_idx_cu_est(u8  skip_idx, u8 num_hmvp_cands, u8 slice_type)
{

	U8 enc_est = 0;
	U8 val;
	U8 max_skip_num;
	// for P slice, change 3, 4, ..., 13 to 1, 2, ..., 11
	if ((slice_type) == 2 && skip_idx > 0) {
		skip_idx -= 2;
	}
	val = skip_idx;
	max_skip_num = (((slice_type) == 2 ? 2 : (PRED_DIR_NUM)+1)) + num_hmvp_cands;
	enc_est += val;
	if (skip_idx != max_skip_num - 1) {
		enc_est++;
	}
	return enc_est;
}
static int encode_affine_flag_est_64(U1 flag, u8 affine_enable_flag, U3 cu_width_log2, U3 cu_height_log2)
{
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 enc_est = 0;
	if (cu_width_log2 >= 4 && cu_height_log2 >= 4 && 1) {
		enc_est++;
	}
	return enc_est;
}

static int encode_mvd_est(S16 mvd[2])
{
	U16 mv;
	U5 enc_est = 0;

	mv = (mvd[0]);
	if ((mvd[0]) < 0) {

		mv = ((-(mvd[0])));
	}
	enc_est += enc_eco_abs_mvd_est(mv);
	if (mv) {
		enc_est++;
	}

	mv = (mvd[1]);
	if ((mvd[1]) < 0) {

		mv = ((-(mvd[1])));
	}
	enc_est += enc_eco_abs_mvd_est(mv);
	if (mv) {
		enc_est++;
	}
	return enc_est;
}


int enc_bit_est_flag_64(U2 slice_type, U21 f_scu, u8 curr_mvr, U2 tree_status, U2 cons_pred_mode, U2 cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], U3 cu_width_log2, U3 cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], int cur_ptr, int refp_ptr[17][2], u8 num_of_hmvp_cand, u8 mpm[4][2], int intra)
{
	U8 enc_est = 0;
	if (intra) {
		if (slice_type != 1) {
			if ((cons_pred_mode) != (ONLY_INTRA)) {
				enc_est += 2;
			}
			if (cons_pred_mode == NO_MODE_CONS) {
				enc_est++;
			}
		}
		enc_est += encode_intra_dir_est(ipm[0][0], mpm[0]);
		enc_est++;
	}
	else {
		U2 skip_flag = (cu_mode == 2);
		U2 dir_flag = (cu_mode == 3);
		U1 umve_enable_flag = 1;
		U4 num_of_hmvp_cand = 8;
		U1 amvr_enable_flag = 1;
		U1 emvr_enable_flag = 1;
		U1 smvd_enable_flag = 1;
		enc_est++;
		if (!skip_flag) {
			enc_est++;
		}
		if (skip_flag || dir_flag) {
			if (umve_enable_flag) {
				enc_est++;
			}
			if (umve_flag) {
				enc_est += encode_umve_idx_est((umve_idx));
			}
			else {
				if (cu_width_log2 >= 4 && cu_height_log2 >= 4) {
					enc_est++;
				}
				if (affine_flag) {
					enc_est += sbac_write_truncate_unary_sym_est((skip_idx), 5);
				}
				else {
					enc_est += encode_skip_idx_cu_est((skip_idx), num_of_hmvp_cand, slice_type);
				}
			}
		}
		else {
			if (((cons_pred_mode)) == (NO_MODE_CONS)) {
				enc_est++;
			}
			enc_est += encode_affine_flag_est_64(((affine_flag)) != 0, 1, cu_width_log2, cu_height_log2);
			if (amvr_enable_flag) {
				// also imply ctx->info.sqh.num_of_hmvp_cand is not zero
				if (emvr_enable_flag && !affine_flag) {
					enc_est++;
				}
				if (affine_flag) {
					enc_est += sbac_write_truncate_unary_sym_est(curr_mvr, 3);
				}
				else {
					enc_est += sbac_write_truncate_unary_sym_est(curr_mvr, 5);
				}
			}
			{
				S8 refi0 = refi[0];
				S8 refi1 = refi[1];
				if ((smvd_enable_flag) && refi0 >= 0 && refi1 >= 0 && cur_ptr - (refp_ptr[0][0]) == (refp_ptr[0][1]) - cur_ptr && (affine_flag) == 0 && !(mvp_from_hmvp_flag)) {
					enc_est++;
				}
				if ((slice_type == 2 || slice_type == 3) && refi0 >= 0) {
					if ((smvd_flag) == 0)
						if ((num_refp) > 1) {
							enc_est += sbac_write_truncate_unary_sym_est(refi0, num_refp);
						}
					if (affine_flag) {
						S16 affine_mvd_real[2];
						U3 amvr_shift = ((curr_mvr) == 0 ? 2 : (((curr_mvr) == 1 ? 4 : 0)));
						affine_mvd_real[0] = (((affine_mvd[0][0][0]) >> (amvr_shift)));
						affine_mvd_real[1] = (((affine_mvd[0][0][1]) >> (amvr_shift)));
						enc_est += encode_mvd_est(affine_mvd_real);
						S16 affine_mvd_real_1[2];
						affine_mvd_real_1[0] = (((affine_mvd[0][1][0]) >> (amvr_shift)));
						affine_mvd_real_1[1] = (((affine_mvd[0][1][1]) >> (amvr_shift)));
						enc_est += encode_mvd_est(affine_mvd_real_1);
					}
					else {
						S16 a_mvd[2];
						a_mvd[0] = (((mvd[0][0]) >> (curr_mvr)));
						a_mvd[1] = (((mvd[0][1]) >> (curr_mvr)));
						enc_est += encode_mvd_est(a_mvd);
					}
				}
			}
		}


		if (tree_status != TREE_C) {
			enc_est += enc_eco_cbf_est_64(num_nz, cu_mode, ipm, tree_status, cu_width_log2, cu_height_log2);
		}
		else if (!((cu_mode) == 0 && (ipm[0][0]) == 33
			&& (ipm[0][1]) == 0)) {
			enc_est += 2;
		}
	}

	return enc_est;
}

int enc_eco_level_est_2(u16 sym)
{

//#pragma HLS INLINE
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
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 20;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 22;
		}
		enc_est += exp_golomb_order;
		//sbac_write_unary_sym_ep_est(exp_golomb_order);
		//sbac_encode_bins_ep_msb_est(exp_golomb_order);
	}
	return enc_est;
}
int enc_eco_run_est_2(U10 sym, u16 level)
{

//#pragma HLS INLINE
	U5 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (sym == 0) {
		if (level == 0) {
			enc_est = 0;
		}
		else {
			enc_est = 1;
		}
	}
	else if (sym < (16)) {
		enc_est += sym + 1;
	}
	else {
		sym -= (16);
		enc_est += 16;
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 20;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 22;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}

int rate_coef_est_64(s16 coef[64], U10 run[64], u8 w)
{
//#pragma HLS INLINE
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1
//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	int i;
	for (i = 0; i < 64; i++)
	{
//#pragma HLS UNROLL
		if (i < w)
		{
			u16  level;
			level = abs(coef[i]);
			//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
			//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
			enc_est += enc_eco_run_est_2(run[i], level);
			enc_est += enc_eco_level_est_2(level);
		}
	}
	return enc_est;
}

int rate_coef_est_64_coef_0(u8 w)
{
//#pragma HLS INLINE
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 64; i++)
	{
//#pragma HLS UNROLL

		//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
		//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
		if (i < w)
		{
			enc_est += enc_eco_run_est_2(0, 0);
			enc_est += enc_eco_level_est_2(0);
		}
	}
	return enc_est;
}

int enc_eco_run_est_2_only(U10 sym)
{

//#pragma HLS INLINE
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
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 20;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 22;
		}
		enc_est += exp_golomb_order;
	}
	return enc_est;
}


int rate_coef_est_64_run(U10 run[64], u8 w)
{
//#pragma HLS INLINE
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1
	////#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;
	for (i = 0; i < 64; i++)
	{
//#pragma HLS UNROLL
		if (i < w)
		{
			//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
			//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
			enc_est += enc_eco_run_est_2_only(run[i]);
			//enc_est += enc_eco_level_est_2(level);
		}
	}
	return enc_est;
}

int enc_eco_coef_run_64_1(U7 w, U7 h, s16 coef[4096], U3 log2_w, U3 log2_h) {
	U15 enc_est = 0;
	static U10 run[64];
	for (U7 z = 0; z < 64; z++) {
//#pragma HLS UNROLL
		if (z < w)
		{
			run[z] = 0;
		}
	}
	//	run_end[0] = 0;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < w; i++)
	{
//#pragma HLS PIPELINE II=1
//#pragma HLS LOOP_TRIPCOUNT max=64
		/*if (i == 0)
		{
			if (coef[i]) {
				run_num = 0;
			}
			else
				run_num = 1;
		}
		else
		{*/
		if (i & 0x01)
		{
			for (j = 63; j >= 0; j--)
			{
//#pragma hls unroll
				if (j <= i)
				{
					if (coef[(j << log2_w) + i - j]) {
						if (!run_num) {
							run[i - j] = 65;
						}
						else {
							run[i - j] = run_num;
						}
						run_num = 0;
					}
					else
					{
						run[i - j] = 0;
						run_num++;
					}
				}
			}
		}
		else
		{
			for (j = 0; j < 64; j++)
			{
//#pragma hls unroll
				if (j <= i)
				{
					if (coef[(j << log2_w) + i - j]) {
						if (!run_num) {
							run[j] = 65;
						}
						else {
							run[j] = run_num;
						}
						run_num = 0;
					}
					else
					{
						run[j] = 0;
						run_num++;
					}
				}
			}
		}
		/*}*/


		enc_est += rate_coef_est_64_run(run, w);
	}

	return enc_est;
}

int enc_eco_level_est_2_only(u16 sym)
{

//#pragma HLS INLINE
	U5 exp_golomb_order = 0;
	U6 enc_est = 0;
	if (sym < (9)) {
		enc_est = sym;
		enc_est += 2;
	}
	else {
		sym -= (9);
		//enc_est += sbac_write_truncate_unary_sym_est(8, 9);
		enc_est = 10;
		if (sym == (0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= (1) && sym < (3)) {
			exp_golomb_order = 2;
		}
		else if (sym >= (3) && sym < (7)) {
			exp_golomb_order = 4;
		}
		else if (sym >= (7) && sym < (15)) {
			exp_golomb_order = 6;
		}
		else if (sym >= (15) && sym < (31)) {
			exp_golomb_order = 8;
		}
		else if (sym >= (31) && sym < (63)) {
			exp_golomb_order = 10;
		}
		else if (sym >= (63) && sym < (127)) {
			exp_golomb_order = 12;
		}
		else if (sym >= (127) && sym < (255)) {
			exp_golomb_order = 14;
		}
		else if (sym >= (255) && sym < (511)) {
			exp_golomb_order = 16;
		}
		else if (sym >= (512) && sym < (1023)) {
			exp_golomb_order = 18;
		}
		else if (sym >= (1023) && sym < (2047)) {
			exp_golomb_order = 20;
		}
		else if (sym >= (2047) && sym < (4095)) {
			exp_golomb_order = 22;
		}
		enc_est += exp_golomb_order;
		//sbac_write_unary_sym_ep_est(exp_golomb_order);
		//sbac_encode_bins_ep_msb_est(exp_golomb_order);
	}
	return enc_est;
}


int rate_coef_est_64_level(s16 coef[64], u8 w)
{
//#pragma HLS INLINE
//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;
	for (i = 0; i < 64; i++)
	{
//#pragma HLS UNROLL
		if (i < w)
		{
			u16  level;
			if (coef[i]) {
				level = abs(coef[i]);
				//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
				//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
				//enc_est += enc_eco_run_est_2(run[i], level);
				enc_est += enc_eco_level_est_2_only(level);
			}
		}
	}
	return enc_est;
}

int enc_eco_coef_level_64_1(U7 w, U7 h, s16 coef[4096], U3 log2_w, U3 log2_h) {
	U15 enc_est = 0;
	static s16 coef_out[64];
	for (U7 z = 0; z < 64; z++) {
//#pragma HLS UNROLL
		if (z < w)
		{
			coef_out[z] = 0;
		}
	}
	S7 j;
	for (U7 i = 0; i < w; i++)
	{
//#pragma HLS PIPELINE II=1
//#pragma HLS LOOP_TRIPCOUNT max=64
		if (i == 0)
		{
			coef_out[0] = coef[i];
		}
		else
		{
			if (i & 0x01)
			{
				for (j = 63; j >= 0; j--)
				{
//#pragma hls unroll
					if (j <= i)
					{
						coef_out[i - j] = coef[(j << log2_w) + i - j];
					}
				}
			}
			else
			{
				for (j = 0; j < 64; j++)
				{
//#pragma hls unroll
					if (j <= i)
					{
						coef_out[j] = coef[(j << log2_w) + i - j];
					}
				}
			}
		}


		enc_est += rate_coef_est_64_level(coef_out, w);
	}

	return enc_est;
}

int enc_eco_coef_64_1_new(U7 w, U7 h, s16 coef[4096], U3 log2_w, U3 log2_h)
{
	U15 enc_est = 0;
	enc_est += enc_eco_coef_run_64_1(w, h, coef, log2_w, log2_h);
	enc_est += enc_eco_coef_level_64_1(w, h, coef, log2_w, log2_h);
	return enc_est;
}
static int rate_coef_level_est_64(s16 coef[64], u8 w)
{
	//#pragma HLS INLINE
	//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 64; i++)
	{
		//#pragma HLS UNROLL
		if (i < w)
		{
			u16  level;
			if (coef[i]) {
				level = abs(coef[i]);
				//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
				//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
				enc_est += enc_eco_level_est_2_only(level);
			}
		}
	}
	return enc_est;
}

static int enc_eco_coef_64_run(U8 cu_width, U8 cu_height, s16 coef[64][64], U3 log2_w, U3 log2_h) {
//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	int final_rate = 0;
	int last_x[64];  // the last non-zero position of scan-line.
	int last_y[64];

	int first_x[64]; // the first non-zero position of scan-line.
	int first_y[64];

	int sumRate[64];

	int final_last_x = -1, final_last_y = -1;
	int final_zig_zag = 0;

	static s16 coef_out[64];
	int max_num = 0;

#pragma HLS ARRAY_PARTITION variable=last_x complete dim=1
#pragma HLS ARRAY_PARTITION variable=last_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=first_x complete dim=1
#pragma HLS ARRAY_PARTITION variable=first_y complete dim=1
#pragma HLS ARRAY_PARTITION variable=sumRate complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef cyclic factor=16 dim=1

	for (U7 z = 0; z < 64; z++) {
#ifdef LUMA_64_PIPELINE_RATE_OPT
#pragma HLS UNROLL
#endif
		coef_out[z] = 0;
		last_x[z] = -1;
		last_y[z] = -1;
		first_x[z] = -1;
		first_y[z] = -1;
		sumRate[z] = 0;
	}

	//memset(last_x, -1, sizeof(int) * 64);
	//memset(last_y, -1, sizeof(int) * 64);

	//memset(first_x, -1, sizeof(int) * 64);
	//memset(first_y, -1, sizeof(int) * 64);
	int shift_t = 0;
	if (cu_width == 8 || cu_height == 8) {
		shift_t = 8;
	}
	else if (cu_width == 16 || cu_height == 16) {
		shift_t = 16;
	}
	else if (cu_width == 32 || cu_height == 32) {
		shift_t = 32;
	}
	else {
		shift_t = 64;
	}
	for (int j = 0; j < cu_width; j++) // vertical
	{
#ifdef LUMA_64_PIPELINE_RATE_OPT
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max=64
#endif
		if (cu_width == 64 && cu_height == 8) {
			max_num = (j < 56 ? 8 : 64 - j);
		}
		else if (cu_width == 8 && cu_height == 64)
		{
			max_num = 8 - j;
		}
		else if (cu_width == 64 && cu_height == 16)
		{
			max_num = (j < 48 ? 16 : 64 - j);
		}
		else if (cu_width == 16 && cu_height == 64)
		{
			max_num = 16 - j;
		}
		else if (cu_width == 64 && cu_height == 32)
		{
			max_num = (j < 32 ? 32 : 64 - j);
		}
		else if (cu_width == 32 && cu_height == 64) {
			max_num = 32 - j;
		}
		else {
			max_num = 64 - j;
		}

		int zzz = 0;
		for (int i = 0; i < 64; i++) // horizontal
		{
			if (i < cu_height) {
				if (i < max_num) {
					coef_out[i] = coef[i][j];
				}

				if (j == 0) // first input row data.
				{
					if (coef[i][j])
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

					if (coef[i][j])
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
						if (first_x[i] == -1 && coef[i][j])
						{
							first_x[i] = i;
							first_y[i] = j;
						}

						if (coef[i][j])
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
						if (coef[i][j] && last_x[i] == -1) // first non-zero position
						{
							last_x[i] = i;
							last_y[i] = j;
						}

						if (coef[i][j] && first_x[i] != -1)
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

						if (coef[i][j])
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
		for (int i = 1; i < 64; i++)
		{
#pragma HLS UNROLL
			if (i < shift_t) {
				last_x[i - 1] = last_x[i];
				last_y[i - 1] = last_y[i];

				first_x[i - 1] = first_x[i];
				first_y[i - 1] = first_y[i];

				sumRate[i - 1] = sumRate[i];
			}
		}
		final_rate += rate_coef_level_est_64(coef_out, max_num);
	}
	return final_rate;
}

int enc_eco_coef_64(U7 w, U7 h, s16 coef[64][64], U3 log2_w, U3 log2_h)
{
	U15 enc_est = 0;
	enc_est += enc_eco_coef_64_run(w, h, coef, log2_w, log2_h);
	return enc_est;
}

int enc_eco_coef_64_1(U7 w, U7 h, s16 coef[4096], U3 log2_w, U3 log2_h)
{
	U15 enc_est = 0;
	static s16 coef_out[64];
	//s16 coef[h][w];
	static U10 run[64];
//#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1

//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	//	FUNC_NAME(ready_coef, w, h)(coef_in, coef);
	for (U7 z = 0; z < 64; z++) {
//#pragma HLS UNROLL
		if (z < w)
		{
			run[z] = 0;
			coef_out[z] = 0;
		}
	}
	//	run_end[0] = 0;
	U10 run_num = 0;
	S7 j;
	for (U7 i = 0; i < w; i++)
	{
//#pragma HLS PIPELINE II=1
//#pragma HLS LOOP_TRIPCOUNT max=64
		if (i == 0)
		{
			coef_out[0] = coef[i];
			if (coef[i]) {
				run_num = 0;
			}
			else
				run_num = 1;
		}
		else
		{
			if (i & 0x01)
			{
				for (j = 63; j >= 0; j--)
				{
//#pragma hls unroll
					if (j <= i)
					{
						coef_out[i - j] = coef[(j << log2_w) + i - j];
						if (coef[(j << log2_w) + i - j]) {
							run[i - j] = run_num;
							run_num = 0;
						}
						else
						{
							run[i - j] = 0;
							run_num++;
						}
					}
				}
			}
			else
			{
				for (j = 0; j < 64; j++)
				{
//#pragma hls unroll
					if (j <= i)
					{
						coef_out[j] = coef[(j << log2_w) + i - j];
						if (coef[(j << log2_w) + i - j]) {
							run[j] = run_num;
							run_num = 0;
						}
						else
						{
							run[j] = 0;
							run_num++;
						}
					}
				}
			}
		}


		enc_est += rate_coef_est_64(coef_out, run, w);
	}

	return enc_est;
}

static int enc_eco_coef_64_1_coef_0(U7 w, U7 h)
{
	U15 enc_est = 0;

	for (U7 i = 0; i < w; i++)
	{
//#pragma HLS PIPELINE II=1
//#pragma HLS LOOP_TRIPCOUNT max=64
		enc_est += rate_coef_est_64_coef_0(w);
	}

	return enc_est;
}

int enc_eco_coef_run_64_2(U7 cu_width, U7 cu_height, s16 coef[4096], U3 log2_w, U3 log2_h) {
	U15 enc_est = 0;
	static U10 run[64];
	for (U7 z = 0; z < 64; z++) {
//#pragma HLS UNROLL
		run[z] = 0;
	}
	U10 run_num = 0;
	S7 j;
	U7 max_cnt = 0;
	for (U7 i = 0; i < cu_width; i++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=64

		if (i < cu_height)
			max_cnt = i;

		if (i & 1)
		{
			for (j = 0; j <= 31; j++)
			{
//#pragma hls unroll
				if (j <= max_cnt)
				{
					if (coef[(j << log2_w) + i - j]) {
						if (!run_num) {
							run[j] = 65;
						}
						else {
							run[j] = run_num;
						}
						run_num = 0;
					}
					else
					{
						run[j] = 0;
						run_num++;
					}
				}
			}
		}
		else
		{
			for (j = 31; j >= 0; j--)
			{
//#pragma hls unroll
				if (j <= max_cnt)
				{
					if (coef[(j << log2_w) + i - j]) {
						if (!run_num) {
							run[max_cnt - j] = 65;
						}
						else {
							run[max_cnt - j] = run_num;
						}
						run_num = 0;
					}
					else
					{
						run[max_cnt - j] = 0;
						run_num++;
					}
				}
			}
		}

		enc_est += rate_coef_est_64_run(run, cu_height);
	}
	return enc_est;
}

int enc_eco_coef_level_64_2(U7 cu_width, U7 cu_height, s16 coef[4096], U3 log2_w, U3 log2_h) {
	U15 enc_est = 0;
	static s16 coef_out[64];
//#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1

//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	//coef_ready_16x16(coef_in, coef);
	//FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U7 z = 0; z < 64; z++) {
//#pragma HLS UNROLL
		coef_out[z] = 0;
	}
	U10 run_num = 0;
	S7 j;
	U7 max_cnt = 0;
	for (U7 i = 0; i < cu_width; i++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=64

		if (i < cu_height)
			max_cnt = i;

		if (i & 1)
		{
			for (j = 0; j <= 31; j++)
			{
//#pragma hls unroll
				if (j <= max_cnt)
				{
					coef_out[j] = coef[(j << log2_w) + i - j];
				}
			}
		}
		else
		{
			for (j = 31; j >= 0; j--)
			{
//#pragma hls unroll
				if (j <= max_cnt)
				{
					coef_out[max_cnt - j] = coef[(j << log2_w) + i - j];
				}
			}
		}

		enc_est += rate_coef_est_64_level(coef_out, cu_height);
	}
	return enc_est;
}

int  enc_eco_coef_64_2_new(U8 cu_width, U8 cu_height, s16 coef[64 * 64], U3 log2_w, U3 log2_h) {
	U15 enc_est = 0;
	enc_est += enc_eco_coef_run_64_2(cu_width, cu_height, coef, log2_w, log2_h);
	enc_est += enc_eco_coef_level_64_2(cu_width, cu_height, coef, log2_w, log2_h);
	return enc_est;
}

int  enc_eco_coef_64_2(U8 cu_width, U8 cu_height, s16 coef[64 * 64], U3 log2_w, U3 log2_h) {
	U15 enc_est = 0;
	static s16 coef_out[64];
	//s16 coef[H][W];
	static U10 run[64];
//#pragma HLS ARRAY_PARTITION variable=coef_out complete dim=1
//#pragma HLS ARRAY_PARTITION variable=run complete dim=1

//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	//coef_ready_16x16(coef_in, coef);
	//FUNC_NAME(ready_coef, W, H)(coef_in, coef);
	for (U7 z = 0; z < 64; z++) {
//#pragma HLS UNROLL
		run[z] = 0;
		coef_out[z] = 0;
	}
	U10 run_num = 0;
	S7 j;
	U7 max_cnt = 0;
	for (U7 i = 0; i < cu_width; i++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=64

		if (i < cu_height)
			max_cnt = i;

		if (i & 1)
		{
			for (j = 0; j <= 31; j++)
			{
//#pragma hls unroll
				if (j <= max_cnt)
				{
					coef_out[j] = coef[(j << log2_w) + i - j];
					if (coef[(j << log2_w) + i - j]) {
						run[j] = run_num;
						run_num = 0;
					}
					else
					{
						run[j] = 0;
						run_num++;
					}
				}
			}
		}
		else
		{
			for (j = 31; j >= 0; j--)
			{
//#pragma hls unroll
				if (j <= max_cnt)
				{
					coef_out[max_cnt - j] = coef[(j << log2_w) + i - j];
					if (coef[(j << log2_w) + i - j]) {
						run[max_cnt - j] = run_num;
						run_num = 0;
					}
					else
					{
						run[max_cnt - j] = 0;
						run_num++;
					}
				}
			}
		}

		enc_est += rate_coef_est_64(coef_out, run, cu_height);
	}
	return enc_est;
}

int  enc_eco_coef_64_2_coef_0(U8 cu_width, U8 cu_height) {
	U15 enc_est = 0;
	for (U7 i = 0; i < cu_width; i++)
	{
//#pragma HLS PIPELINE II=1
//#pragma HLS LOOP_TRIPCOUNT max=64
		enc_est += rate_coef_est_64_coef_0(cu_height);
	}
	return enc_est;
}



int get_bitcnt_est_64_coef_0(U21 f_scu, u8 curr_mvr, U2 tree_status,
	U2 ch_type, s32 slice_type, U2 cons_pred_mode, U2 cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag,
	unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp,
	s16 affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], U3 cu_width_log2, U3 cu_height_log2, int tb_part,
	int pb_part, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], unsigned char slice_type_md, int cur_ptr, int refp_ptr[17][2], u8 mpm[4][2], U1 is_intra)
{

	U16 enc_est;
	enc_est = enc_bit_est_flag_64(slice_type, f_scu, curr_mvr, tree_status, cons_pred_mode, cu_mode, umve_flag, umve_idx, affine_flag, 1, skip_idx, refi, mvp_from_hmvp_flag, smvd_flag, num_refp, affine_mvd, mvd, cu_width_log2, cu_height_log2, num_nz, ipm, cur_ptr, refp_ptr, 8, mpm, is_intra);

	return enc_est;

}


s64 pinter_me_pred_64(U7 w, U7 h, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_COM_MODE* mod_info_curr_ptr,
	s64 lambdaY, s64 dist_pred[1], pel* org_y, int stride_org, s64* cost_best,
	u8 curr_mvr, s16 resi_t[64][64], U2 tree_status, U2 cons_pred_mode, pel pred_cache[64][64], U1 is_intra,
	pel RefWindowLCU_t[512 * 512])
{
	MD_COM_MODE* mod_info_curr = mod_info_curr_ptr;

	/* prediction */
#if MD_PRINT
	((int)mod_info_curr->refi[0]) >= 0
		&& !(((int)mod_info_curr->refi[1]) >= 0) ? ((void)0) :
		__assert_HLS("((mod_info_curr->refi[0]) >= 0) && !((mod_info_curr->refi[1]) >= 0)", "../separate/enc_pinter.c",
			(unsigned int)1053, __FUNCTION__);
#endif

	com_mc_fetch_64(w, h, RefWindowLCU, mod_info_curr, tree_status, md_fw_ptr->bit_depth_internal,
		md_fw_ptr->pic_width, md_fw_ptr->pic_height, curr_mvr, CtrPos, pred_cache, RefWindowLCU_t);

	return *cost_best;
}

int com_affine_mc_pred_y_64(U7 w_input, U7 h_input, pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_COM_MODE* mod_info_curr,
	pel pred[64][64], U3 sub_w, U3 sub_h, U1 lidx, U4 bit_depth, U13 pic_w, U13 pic_h, S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS])
{
	U13 x = mod_info_curr->x_pos;
	U13 y = mod_info_curr->y_pos;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U2 cp_num = ((int)(mod_info_curr->affine_flag)) + 1;
	S3* refi = mod_info_curr->refi;
	CPMV(*ac_mv)[2] = mod_info_curr->affine_mv[lidx];
	pel* pic_y = RefWindowLCU;
	//int y_offset = 0;
	//int uv_offset = 0;
	int qpel_gmv_x_in_sw = 0;
	int qpel_gmv_y_in_sw = 0;
#if MD_PRINT
	(com_tbl_log2[cu_width]) >= 4 ? ((void)0) : __assert_HLS("com_tbl_log2[cu_width] >= 4", "../separate/com_mc.c",
		3936, __FUNCTION__);
	(com_tbl_log2[cu_height]) >= 4 ? ((void)0) : __assert_HLS("com_tbl_log2[cu_height] >= 4",
		"../separate/com_mc.c", 3937, __FUNCTION__);
#endif
	int qpel_gmv_x;
	int qpel_gmv_y;
	//int predy_offset = 0;
	U7 w;
	U7 h;
	U3 half_w;
	U3 half_h;
	int dmv_hor_x;
	int dmv_ver_x;
	int dmv_hor_y;
	int dmv_ver_y;
	s32 mv_scale_hor = ((s32)ac_mv[0][0]) << 7;
	s32 mv_scale_ver = ((s32)ac_mv[0][1]) << 7;
	CPMV mv_scale_tmp_hor;
	CPMV mv_scale_tmp_ver;
	s32 hor_max;
	s32 hor_min;
	s32 ver_max;
	s32 ver_min;
	s32 mv_scale_tmp_hor_ori;
	s32 mv_scale_tmp_ver_ori;
	s32 mv_save[64 >> 2][64 >> 2][2];
	U9 stride_l = SWW;
	// get clip MV Range
	hor_max = (pic_w + (1 << 7) + 4 - x - cu_width + 1) << 4;
	ver_max = (pic_h + (1 << 7) + 4 - y - cu_height + 1) << 4;
	hor_min = (-(1 << 7) - 4 - x) << 4;
	ver_min = (-(1 << 7) - 4 - y) << 4;
	// get sub block size
	half_w = sub_w >> 1;
	half_h = sub_h >> 1;
	// convert to 2^(storeBit + bit) precision
	// deltaMvHor
	dmv_hor_x = (ac_mv[1][0] - ac_mv[0][0]) << 7 >> (com_tbl_log2[cu_width]);
	dmv_hor_y = (ac_mv[1][1] - ac_mv[0][1]) << 7 >> (com_tbl_log2[cu_width]);
	if (cp_num == 3) {
		// deltaMvVer
		dmv_ver_x = (ac_mv[2][0] - ac_mv[0][0]) << 7 >> (com_tbl_log2[cu_height]);
		dmv_ver_y = (ac_mv[2][1] - ac_mv[0][1]) << 7 >> (com_tbl_log2[cu_height]);
	}
	else {
		// deltaMvVer
		dmv_ver_x = -dmv_hor_y;
		dmv_ver_y = dmv_hor_x;
	}
	// get prediction block by block (luma)
	for (h = 0; h < cu_height; h += sub_h) {

//#pragma HLS loop_tripcount max = 16
		for (w = 0; w < cu_width; w += sub_w) {

//#pragma HLS loop_tripcount max = 16
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
			// 1/16 precision, 18 bits, for MC
			com_mv_rounding_s32(mv_scale_tmp_hor, mv_scale_tmp_ver, &mv_scale_tmp_hor, &mv_scale_tmp_ver, 7, 0);
			mv_scale_tmp_hor = ((-131072) > ((131071 < mv_scale_tmp_hor ? (CPMV)131071 : mv_scale_tmp_hor)) ? (CPMV)(-131072) : (CPMV)((131071 < mv_scale_tmp_hor ? (CPMV)131071 : mv_scale_tmp_hor)));
			mv_scale_tmp_ver = ((-131072) > ((131071 < mv_scale_tmp_ver ? (CPMV)131071 : mv_scale_tmp_ver)) ? (CPMV)(-131072) : (CPMV)((131071 < mv_scale_tmp_ver ? (CPMV)131071 : mv_scale_tmp_ver)));
			// save MVF for chroma interpolation
			int w_scu = w >> 2;
			int h_scu = h >> 2;
			mv_save[w_scu][h_scu][0] = mv_scale_tmp_hor;
			mv_save[w_scu][h_scu][1] = mv_scale_tmp_ver;
			if (sub_w == 8 && sub_h == 8) {
				mv_save[w_scu + 1][h_scu][0] = mv_scale_tmp_hor;
				mv_save[w_scu + 1][h_scu][1] = mv_scale_tmp_ver;
				mv_save[w_scu][h_scu + 1][0] = mv_scale_tmp_hor;
				mv_save[w_scu][h_scu + 1][1] = mv_scale_tmp_ver;
				mv_save[w_scu + 1][h_scu + 1][0] = mv_scale_tmp_hor;
				mv_save[w_scu + 1][h_scu + 1][1] = mv_scale_tmp_ver;
			}
			// clip
			mv_scale_tmp_hor_ori = mv_scale_tmp_hor;
			mv_scale_tmp_ver_ori = mv_scale_tmp_ver;
			mv_scale_tmp_hor = (hor_max < ((hor_min > mv_scale_tmp_hor ? (CPMV)hor_min : mv_scale_tmp_hor)) ? (CPMV)hor_max : (CPMV)((hor_min > mv_scale_tmp_hor ? (CPMV)hor_min : mv_scale_tmp_hor)));
			mv_scale_tmp_ver = (ver_max < ((ver_min > mv_scale_tmp_ver ? (CPMV)ver_min : mv_scale_tmp_ver)) ? (CPMV)ver_max : (CPMV)((ver_min > mv_scale_tmp_ver ? (CPMV)ver_min : mv_scale_tmp_ver)));
			qpel_gmv_x = ((x + w) << 4) + mv_scale_tmp_hor;
			qpel_gmv_y = ((y + h) << 4) + mv_scale_tmp_ver;
			//convert to qpel_gmv in ref window
			qpel_gmv_x_in_sw = qpel_gmv_x - (((int)CtrPos[0][0][0]) << 4) + (((
				int)CtrPos[0][0][1]) << 4);
			qpel_gmv_y_in_sw = qpel_gmv_y - (((int)CtrPos[0][1][0]) << 4) + (((
				int)CtrPos[0][1][1]) << 4);
#if SKIP_RESTRI
			if (!mv_inbound_new_skip(qpel_gmv_x, qpel_gmv_y, CtrPos2MD, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x, y, 0, 4))
#else
			if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, CtrPos, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x, y, 0, 4))
#endif
			{
				//printf("%s:%d: Affine motion is out of ref window!\npos=(%d %d) size=(%d %d) scu=(%d %d) ac_mv=(", __FUNCTION__, __LINE__, x, y, cu_width, cu_height, w >> 2, w >> 2);
				//for (int i = 0; i < cp_num; ++i) {
				//	printf("(%d %d)", ac_mv[i][MV_X], ac_mv[i][MV_Y]);
				//}
				//printf(") gmv=(%d %d)\n", (qpel_gmv_x >> 4), (qpel_gmv_y >> 4));
				return 0;
			}

			com_tbl_mc_l_1_64(mv_scale_tmp_hor_ori, mv_scale_tmp_ver_ori, pic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l,
				cu_width, pred, w, h, sub_w, sub_h, bit_depth, 1);

		}
		//pred += cu_width * sub_h;
	}
	//
	if (mod_info_curr->chroma_motion)
	{
		sub_w = 8;
		sub_h = 8;
		int w_scu;
		int h_scu;
		for (h = 0; h < cu_height; h += sub_h) {

//#pragma HLS loop_tripcount max = 64
			for (w = 0; w < cu_width; w += sub_w) {

//#pragma HLS loop_tripcount max = 64
				w_scu = w >> 2;
				h_scu = h >> 2;
				mv_scale_tmp_hor = (mv_save[w_scu][h_scu][0] + mv_save[w_scu + 1][h_scu][0] + mv_save[w_scu][h_scu + 1][0] +
					mv_save[w_scu + 1][h_scu + 1][0] + 2) >> 2;
				mv_scale_tmp_ver = (mv_save[w_scu][h_scu][1] + mv_save[w_scu + 1][h_scu][1] + mv_save[w_scu][h_scu + 1][1] +
					mv_save[w_scu + 1][h_scu + 1][1] + 2) >> 2;
				mv_scale_tmp_hor_ori = mv_scale_tmp_hor;
				mv_scale_tmp_ver_ori = mv_scale_tmp_ver;
				mv_scale_tmp_hor = (hor_max < ((hor_min > mv_scale_tmp_hor ? hor_min : mv_scale_tmp_hor)) ? hor_max : ((
					hor_min > mv_scale_tmp_hor ? hor_min : mv_scale_tmp_hor)));
				mv_scale_tmp_ver = (ver_max < ((ver_min > mv_scale_tmp_ver ? ver_min : mv_scale_tmp_ver)) ? ver_max : ((
					ver_min > mv_scale_tmp_ver ? ver_min : mv_scale_tmp_ver)));
				qpel_gmv_x = ((mod_info_curr->x_chroma + w) << 4) + mv_scale_tmp_hor;
				qpel_gmv_y = ((mod_info_curr->y_chroma + h) << 4) + mv_scale_tmp_ver;
				//com_mc_c_hp( mv_scale_tmp_hor_ori, mv_scale_tmp_ver_ori, ref_pic->u, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_chroma, cu_width >> 1, pred_u + (w >> 1), sub_w >> 1, sub_h >> 1, bit_depth );
				//com_mc_c_hp( mv_scale_tmp_hor_ori, mv_scale_tmp_ver_ori, ref_pic->v, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_chroma, cu_width >> 1, pred_v + (w >> 1), sub_w >> 1, sub_h >> 1, bit_depth );
				//add by luke
				/* add_yrb */
				//qpel_gmv_x_in_sw = qpel_gmv_x - (((int)Fetch_Refwin_ptr->CtrPos[0][0][0]) << 4) + (((
				//	int)Fetch_Refwin_ptr->CtrPos[0][0][1]) << 4);
				//qpel_gmv_y_in_sw = qpel_gmv_y - (((int)Fetch_Refwin_ptr->CtrPos[0][1][0]) << 4) + (((
				//	int)Fetch_Refwin_ptr->CtrPos[0][1][1]) << 4);
#if SKIP_RESTRI
				if (!mv_inbound_new_skip(qpel_gmv_x, qpel_gmv_y, CtrPos2MD, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x, y, 0, 4))
#else
				if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, CtrPos, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x, y, 0, 4))
#endif
				{
					//printf("%s:%d: Affine motion is out of ref window!\npos=(%d %d) size=(%d %d) scu=(%d %d) ac_mv=(", __FUNCTION__, __LINE__, x, y, cu_width, cu_height, w_scu, h_scu);
					//for (int i = 0; i < cp_num; ++i) {
					//	printf("(%d %d)", ac_mv[i][MV_X], ac_mv[i][MV_Y]);
					//}
					//printf(") gmv=(%d %d)\n", (qpel_gmv_x >> 4), (qpel_gmv_y >> 4));
					return 0;
				}
			}
		}
	}

	return 1;
}

BOOL analyze_me_uni_pred_64(MD_COM_MODE* mod_info_curr,
	u8 num_refp_cnt, u8 curr_mvr, U7 cu_width, U7 cu_height, s16 mvp_all[2][5][2], s16 mv_all[2][5][2],
	int best_mv_uni_inner[2][4][2],
	int refi_cur, U2 tree_status,
	S14 sub_pel_mv[MV_D],
	S3 refi_for_ip[2], s16 mvd_for_ip[2][2], s16 mv_for_ip[2][2], U4 bit_depth
#if ENABLE_BFRAME
	, MV_DIR* mv_dir
#endif
)
{
#if ENABLE_BFRAME
	int lidx = mv_dir->lidx;
	int best_refi = mv_dir->cur_refi;
#else
	int lidx = 0;
	int best_refi = 0;
#endif
	int me_level = 3;
	int is_intra = 0;
	mod_info_curr->cu_mode = 1;
	mod_info_curr->affine_flag = 0;
	s16* mv;
	s16* mvd;
	mv = mv_for_ip[lidx];
	mvd = mvd_for_ip[lidx];

	//init_inter_data_64(mod_info_curr);
	refi_for_ip[0] = 0;
	refi_for_ip[1] = 0;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			mv_for_ip[i][j] = 0;
			mvd_for_ip[i][j] = 0;
		}
	}
#if ENABLE_BFRAME
	if (best_refi != 0 || lidx != 0)
	{
		best_refi = pinter_fme_all_cu_pred(mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->x_chroma, mod_info_curr->y_chroma, mod_info_curr->chroma_motion,
			refi_cur, num_refp_cnt, curr_mvr, lidx, cu_width, cu_height, mvp_all, mv_all, me_level,
			best_mv_uni_inner, sub_pel_mv);
		if (best_refi < 0) { return FALSE; }
		best_refi = mv_dir->cur_refi;
	}
	else
#endif
	{
		best_refi = pinter_fme_all_cu_pred(mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->x_chroma, mod_info_curr->y_chroma, mod_info_curr->chroma_motion,
			refi_cur, num_refp_cnt, curr_mvr, lidx, cu_width, cu_height, mvp_all, mv_all, me_level,
			best_mv_uni_inner, sub_pel_mv);
		if (best_refi < 0) { return FALSE; }
	}

	fme_set_mv_refi_64(mv, mvd, mv_all, mvp_all, mod_info_curr, best_refi, curr_mvr, lidx, refi_for_ip);

	/* prediction */
#if MD_PRINT && !ENABLE_BFRAME
	((int)refi_for_ip[0]) >= 0
		&& !(((int)refi_for_ip[1]) >= 0) ? ((void)0) :
		__assert_HLS("((mod_info_curr->refi[0]) >= 0) && !((mod_info_curr->refi[1]) >= 0)", "../separate/enc_pinter.c",
			(unsigned int)1053, __FUNCTION__);
#endif

	return TRUE;
}

int com_affine_mc_fetch_pred_y_64(U7 w, U7 h, pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_COM_MODE* mod_info_curr, U4 bit_depth,
	U13 pic_w, U13 pic_h, pel pred_cache[64][64], S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS]
#if ENABLE_BFRAME
	, U1 lidx
#endif
)
{
	S3* refi = mod_info_curr->refi;
	U4 sub_w = 4;
	U4 sub_h = 4;
	if (refi[0] >= 0 && refi[1] >= 0) {
		sub_w = 8;
		sub_h = 8;
	}
	if (refi[0] >= 0) {
		return com_affine_mc_pred_y_64(w, h, RefWindowLCU, CtrPos, mod_info_curr, pred_cache, sub_w, sub_h,
#if ENABLE_BFRAME
			lidx,
#else
			0,
#endif
			bit_depth, pic_w, pic_h, CtrPos2MD);
	}
	return 0;
}

void get_pred_y_64(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred[64][64])
{
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_1 dim=2
	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;


#if 1
	pel buffer_0[27][8][2][16];

	U6 gmvxdiv32 = gmvx_refp_int & 31;
	int i, j, k, m;
	int m_tmp;

	for (m = 0; m < 27; m++) {
//#pragma HLS PIPELINE
		for (k = 0; k < 16; k++) {
//#pragma HLS UNROLL
#define FLAT(i, j) buffer_0[m][i][j][k] = ref.ref_##i##_##j [(gmvx_refp_int / 32 + (m % 3)) + ((gmvy_refp_int / 8 + m / 3) * 5) + 80 * frac_idx + lidx * 1280][k];
			FLAT_8_2
#undef FLAT
			//printf("%d ", buffer_0[m][i][j][k]);
		}
	}
	//printf("\n");
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
#if ENABLE_BFRAME
			pred[i][j] = buffer_0[((gmvy_refp_int & 7) + i) / 8 * 3 + (((gmvx_refp_int & 31) + j) >> 5)]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#else
			pred[i][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}
	}
#else
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
#if ENABLE_BFRAME
			pred[i][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx + lidx * 1280]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#else
			pred[i][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}
	}
#endif // 1
	return;
}



BOOL com_mc_cu_y_64(U7 w_input, U7 h_input, CU_POS* cu, U13 pic_w, U13 pic_h, S3 refi[REFP_NUM], s16 mv[REFP_NUM][2], int stride_luma,
	pel pred_y[64][64], U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	Arbitor& ref
	, U2 mode_type
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp, int mode_idx, int umve_flag
#endif
)
{
	int w = cu->cu_width;
	int h = cu->cu_height;
	int scup = cu->scup;
	int x = cu->x_pos;
	int y = cu->y_pos;
	S14 mv_t[2];
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
#if MD_KERNEL_64_MOD_YHY

#pragma HLS ARRAY_PARTITION variable=core_sbtmvp->sbTmvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv_t complete dim=0
#pragma HLS ARRAY_PARTITION variable=min_clip complete dim=0
#pragma HLS ARRAY_PARTITION variable=max_clip complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi complete dim=0

	//int bidx = 0;
	U1 ref0_valid = 0, ref1_valid = 0;
    pel pred_ref0[64][64];

#pragma HLS ARRAY_PARTITION variable=pred_ref0 complete dim=0

	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0] = mv[REFP_0][0];
	mv_t[1] = mv[REFP_0][1];
	if (refi[REFP_0] >= 0)
	{
		if (x_ext + (mv[REFP_0][0]) < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + (mv[REFP_0][1]) < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + (mv[REFP_0][0]) + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + (mv[REFP_0][1]) + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		int qpel_gmv_x;
		int qpel_gmv_y;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[1]);
		int dx = qpel_gmv_x & 0x3;
		int dy = qpel_gmv_y & 0x3;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

#if ENABLE_BFRAME
		S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));
#else
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2));
#endif // ENABLE_BFRAME


#if SUB_TMVP
		if (mode_idx == 0 && cu->cu_width >= SBTMVP_MIN_SIZE && cu->cu_height >= SBTMVP_MIN_SIZE && mode_type >= MODE_SKIP && umve_flag == 0)
		{
			if (!core_sbtmvp->isValid)
				return FALSE;
			core_sbtmvp->isValid = TRUE;
#if DEBUG_CU
			if (isProblem)
			{
				printf("CU %d,%d,%d,%d\n", cu->x_pos, cu->y_pos, cu->cu_width, cu->cu_height);
				printf("ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}\n",
					CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1]);
			}
#endif
			int cu_width = cu->cu_width;
			int cu_height = cu->cu_height;
			int x = cu->x_pos;
			int y = cu->y_pos;
			int sub_w = cu_width / SBTMVP_NUM_1D;
			int sub_h = cu_height / SBTMVP_NUM_1D;
			int h = 0;
			int w = 0;
			int tmp_x = x;
			int tmp_y = y;
			pel pred_tmp[64][64];

#pragma HLS ARRAY_PARTITION variable=pred_tmp complete dim=0

			for (int k = 0; k < SBTMVP_NUM; k++)
			{
#pragma HLS LOOP_TRIPCOUNT max=4
				if (core_sbtmvp->sbTmvp[k].ref_idx[0] >= 0) {
					w = (k % 2) * sub_w;
					h = (k / 2) * sub_h;
					x = tmp_x + w;
					y = tmp_y + h;

					x_ext = x << 2;
					y_ext = y << 2;
					w_ext = sub_w << 2;
					h_ext = sub_h << 2;

					int mv_sbtmvp[2];
					mv_sbtmvp[0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv_sbtmvp[1] = core_sbtmvp->sbTmvp[k].mv[0][1];
					x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
					y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);

					if (x_ext + (mv_sbtmvp[0]) < min_clip[0]) {
						mv_sbtmvp[0] = ((min_clip[0] - x_ext));
					}
					if (y_ext + (mv_sbtmvp[1]) < min_clip[1]) {
						mv_sbtmvp[1] = ((min_clip[1] - y_ext));
					}
					if (x_ext + (mv_sbtmvp[0]) + w_ext - 4 > max_clip[0]) {
						mv_sbtmvp[0] = ((max_clip[0] - x_ext - w_ext + 4));
					}
					if (y_ext + (mv_sbtmvp[1]) + h_ext - 4 > max_clip[1]) {
						mv_sbtmvp[1] = ((max_clip[1] - y_ext - h_ext + 4));
					}

					qpel_gmv_x = (x << 2) + ((int)mv_sbtmvp[0]);
					qpel_gmv_y = (y << 2) + ((int)mv_sbtmvp[1]);
					dx = qpel_gmv_x & 0x3;
					dy = qpel_gmv_y & 0x3;

					if (dx == 0 && dy == 0) {
						x_offset_min = 0;
						y_offset_min = 0;
					}
					else {
						x_offset_min = -3;
						y_offset_min = -3;
					}
#if ENABLE_BFRAME
					cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0);
					qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
					qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));
#else
					qpel_gmv_x_of_refp_cu = qpel_gmv_x - ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2));
					qpel_gmv_y_of_refp_cu = qpel_gmv_y - ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2));
#endif // ENABLE_BFRAME
					if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + sub_w + x_offset_max <= (160 + 4) &&
						(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + sub_h + y_offset_max <= (128 + 4)))
					{
#if MD_PRINT_ENH
						if (mode_type == MODE_SKIP)
							printf("<MODE_SKIP:SUB_TMVP> ");
						else if (mode_type == MODE_DIR)
							printf("<MODE_DIR:SUB_TMVP> ");
						printf("out of ref0 window ERROR: sub cu %d=(%d %d %d %d)", k, x, y, sub_w, sub_h);
						printf("mv=(");
						printf("%d %d", mv_sbtmvp[MV_X], mv_sbtmvp[MV_Y]);
						printf(")\n ");
#if ENABLE_BFRAME
						printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
							cur_refw_idx,
							CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
							CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
						);
						printf("\n");
#endif 
						fflush(stdout);
#endif
						core_sbtmvp->isValid = FALSE;
						return FALSE;
					}

					get_pred_y_64(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
						cur_refw_idx,
#endif // ENBALE_BFRAME
						pred_tmp);
#if DEBUG_CU
					if (isProblem)
					{
						printf("CU %d (%d,%d) (%d,%d)\n", k, mv_sbtmvp[0], mv_sbtmvp[1], qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu);
					}
#endif
					//copy pred_tmp to pred output
					//int offset = h * 64 + w;
					for (int i = 0; i < 32; i++)
					{
#pragma HLS UNROLL
						for (int j = 0; j < 32; j++)
						{
                            if((h + i)<64 && (w+j)<64)
							{
                                pred_ref0[h + i][w + j] = pred_tmp[i][j];//[offset + i * 64 + j]  [i * 64 + j]
							}
#if DEBUG_CU
							if (isProblem)
							{
								printf("%d,", pred_tmp[i][j]);
							}
#endif
						}
#if DEBUG_CU
						if (isProblem)
						{
							printf("\n");
						}
#endif
					}

					/*FILE* sbtmvp = fopen("sbtmvp.csv", "w");
					for (int i = 0; i < 64; i++) {
						for (int j = 0; j < 64; j++) {
							pel curr = pred[i * 64 + j];
							fprintf(sbtmvp, "%d, ", (int)curr);
						}
						fprintf(sbtmvp, "\n");
					}
					fclose(sbtmvp);*/
				}
				else {
#if MD_PRINT
					printf("%s:%d:sbtmvp refi is invalid\n", __FUNCTION__, 3267);
#endif
				}
			}
		}
		else {
#endif
			if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + w + x_offset_max <= (160 + 4) &&
				(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + h + y_offset_max <= (128 + 4)))
			{
#if MD_PRINT
				if (mode_type == MODE_SKIP)
					printf("<MODE_SKIP> ");
				else if (mode_type == MODE_INTER)
					printf("<MODE_INTER> ");
				else if (mode_type == MODE_DIR)
					printf("<MODE_DIR> ");
				printf("out of ref0 window ERROR: cu=(%d %d %d %d)", x, y, w, h);
				printf("mv=(");
				printf("%d %d", mv_t[MV_X], mv_t[MV_Y]);
				printf(")\n ");
#if ENABLE_BFRAME
				printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
					cur_refw_idx,
					CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
					CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
				);
				printf("\n");
#endif 
				printf("\n");
				fflush(stdout);
#endif
				return FALSE;
			}

			get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
				cur_refw_idx,
#endif // ENBALE_BFRAME
				pred_ref0);
#if SUB_TMVP
		}
#endif
		//++bidx;
		ref0_valid = 1;
	}
	if (ref0_valid)
	{
		for (int i = 0; i < 64; i++)
		{
#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
			{
				if(i<h&&j<w)
				{
					pred_y[i][j] = pred_ref0[i][j];
				}
			}
		}
	}
#if !ENABLE_BFRAME
	else {
#if MD_PRINT
		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3213);
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
			return 1;
		}
	}

	mv_t[0] = mv[REFP_1][0];
	mv_t[1] = mv[REFP_1][1];
	static pel pred_ref1[64][64];
	if (REFI_IS_VALID(refi[REFP_1]))
	{
		/* backward */
		//pel(*pred)[64] = bidx ? pred_snd : pred_y;
		//COM_PIC* ref_pic;
		//ref_pic = refp[refi[REFP_1]][REFP_1].pic;
		if (x_ext + (mv[REFP_1][0]) < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + (mv[REFP_1][1]) < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + (mv[REFP_1][0]) + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + (mv[REFP_1][1]) + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		int qpel_gmv_x;
		int qpel_gmv_y;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[1]);
		int dx = qpel_gmv_x & 0x3;
		int dy = qpel_gmv_y & 0x3;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

		S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));

		//if (channel != CHANNEL_C)
		//{
		//if (!((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < SWH)) { return FALSE; }
		if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + w + x_offset_max <= (160 + 4) &&
			(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + h + y_offset_max <= (128 + 4)))
		{
#if MD_PRINT
			if (mode_type == MODE_SKIP)
				printf("<MODE_SKIP> ");
			else if (mode_type == MODE_INTER)
				printf("<MODE_INTER> ");
			else if (mode_type == MODE_DIR)
				printf("<MODE_DIR> ");
			printf("out of ref1 window ERROR: cu=(%d %d %d %d)", x, y, w, h);
			printf("mv=(");
			printf("%d %d", mv_t[MV_X], mv_t[MV_Y]);
			printf(")\n");
			printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
				cur_refw_idx,
				CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
				CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
			);
			printf("\n");
			fflush(stdout);
#endif
			return FALSE;
		}

		get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref, cur_refw_idx, pred_ref1);
		//}

//		if (channel != CHANNEL_L)
//		{
//#if CHROMA_NOT_SPLIT
//			assert(w >= 8 && h >= 8);
//#endif
//			com_mc_c(mv[REFP_1][0], mv[REFP_1][1], ref_pic->u, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_chroma, pred_stride >> 1, pred[U_C], w >> 1, h >> 1, bit_depth);
//			com_mc_c(mv[REFP_1][0], mv[REFP_1][1], ref_pic->v, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_chroma, pred_stride >> 1, pred[V_C], w >> 1, h >> 1, bit_depth);
//		}
		//bidx++;
		ref1_valid = 1;
	}

	if (ref0_valid && ref1_valid)
	{
		//if (channel != CHANNEL_C)
		//{
#if SIMD_MC
		average_16b_no_clip_sse(pred_y[0], pred_snd[0], pred_y[0], 64, 64, 64, w, h);
#else
		//pel(*p0)[64] = pred_y;
		//pel(*p1)[64] = pred_snd;
		for (int j = 0; j < 64; j++)
		{
#pragma HLS UNROLL
			for (int i = 0; i < 64; i++)
			{
				if(i<w&&j<h)
				{
				    pred_y[j][i] = (pred_ref0[j][i] + pred_ref1[j][i] + 1) >> 1;
				}
			}
		}
#endif
		//}

//		if (channel != CHANNEL_L)
//		{
//#if SIMD_MC
//			w >>= 1;
//			h >>= 1;
//			pred_stride >>= 1;
//			average_16b_no_clip_sse(pred_buf[U_C], pred_snd[U_C], pred_buf[U_C], pred_stride, pred_stride, pred_stride, w, h);
//			average_16b_no_clip_sse(pred_buf[V_C], pred_snd[V_C], pred_buf[V_C], pred_stride, pred_stride, pred_stride, w, h);
//#else
//			p0 = pred_buf[U_C];
//			p1 = pred_snd[U_C];
//			p2 = pred_buf[V_C];
//			p3 = pred_snd[V_C];
//			w >>= 1;
//			h >>= 1;
//			pred_stride >>= 1;
//			for (j = 0; j < h; j++)
//			{
//				for (i = 0; i < w; i++)
//				{
//					p0[i] = (p0[i] + p1[i] + 1) >> 1;
//					p2[i] = (p2[i] + p3[i] + 1) >> 1;
//				}
//				p0 += pred_stride;
//				p1 += pred_stride;
//				p2 += pred_stride;
//				p3 += pred_stride;
//			}
//#endif
//		}
	}
	else
	{
		if (ref0_valid)
		{
			/////
		}
		else if (ref1_valid)
		{
			for (int j = 0; j < 64; j++)
			{
#pragma HLS UNROLL
				for (int i = 0; i < 64; i++)
				{
					if(i<w&&j<h)
					{
                        pred_y[j][i] = pred_ref1[j][i];
					}
				}
			}
		}
		else
		{
#if MD_PRINT
			printf("%s:%d:all refs are invalid\n", __FUNCTION__, 7922);
#endif
		}
	}
#endif
	return 1;
#else
	//int bidx = 0;
	U1 ref0_valid = 0, ref1_valid = 0;

	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0] = mv[REFP_0][0];
	mv_t[1] = mv[REFP_0][1];
	if (refi[REFP_0] >= 0)
	{
		if (x_ext + (mv[REFP_0][0]) < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + (mv[REFP_0][1]) < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + (mv[REFP_0][0]) + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + (mv[REFP_0][1]) + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		int qpel_gmv_x;
		int qpel_gmv_y;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[1]);
		int dx = qpel_gmv_x & 0x3;
		int dy = qpel_gmv_y & 0x3;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

#if ENABLE_BFRAME
		S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));
#else
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2));
#endif // ENABLE_BFRAME


#if SUB_TMVP
		if (mode_idx == 0 && cu->cu_width >= SBTMVP_MIN_SIZE && cu->cu_height >= SBTMVP_MIN_SIZE && mode_type >= MODE_SKIP && umve_flag == 0)
		{
			if (!core_sbtmvp->isValid)
				return FALSE;
			core_sbtmvp->isValid = TRUE;
#if DEBUG_CU
			if (isProblem)
			{
				printf("CU %d,%d,%d,%d\n", cu->x_pos, cu->y_pos, cu->cu_width, cu->cu_height);
				printf("ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}\n",
					CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1]);
			}
#endif
			int cu_width = cu->cu_width;
			int cu_height = cu->cu_height;
			int x = cu->x_pos;
			int y = cu->y_pos;
			int sub_w = cu_width / SBTMVP_NUM_1D;
			int sub_h = cu_height / SBTMVP_NUM_1D;
			int h = 0;
			int w = 0;
			int tmp_x = x;
			int tmp_y = y;
			pel pred_tmp[64][64];
			for (int k = 0; k < SBTMVP_NUM; k++)
			{
				if (core_sbtmvp->sbTmvp[k].ref_idx[0] >= 0) {
					w = (k % 2) * sub_w;
					h = (k / 2) * sub_h;
					x = tmp_x + w;
					y = tmp_y + h;

					x_ext = x << 2;
					y_ext = y << 2;
					w_ext = sub_w << 2;
					h_ext = sub_h << 2;

					int mv_sbtmvp[2];
					mv_sbtmvp[0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv_sbtmvp[1] = core_sbtmvp->sbTmvp[k].mv[0][1];
					x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
					y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);

					if (x_ext + (mv_sbtmvp[0]) < min_clip[0]) {
						mv_sbtmvp[0] = ((min_clip[0] - x_ext));
					}
					if (y_ext + (mv_sbtmvp[1]) < min_clip[1]) {
						mv_sbtmvp[1] = ((min_clip[1] - y_ext));
					}
					if (x_ext + (mv_sbtmvp[0]) + w_ext - 4 > max_clip[0]) {
						mv_sbtmvp[0] = ((max_clip[0] - x_ext - w_ext + 4));
					}
					if (y_ext + (mv_sbtmvp[1]) + h_ext - 4 > max_clip[1]) {
						mv_sbtmvp[1] = ((max_clip[1] - y_ext - h_ext + 4));
					}

					qpel_gmv_x = (x << 2) + ((int)mv_sbtmvp[0]);
					qpel_gmv_y = (y << 2) + ((int)mv_sbtmvp[1]);
					dx = qpel_gmv_x & 0x3;
					dy = qpel_gmv_y & 0x3;

					if (dx == 0 && dy == 0) {
						x_offset_min = 0;
						y_offset_min = 0;
					}
					else {
						x_offset_min = -3;
						y_offset_min = -3;
					}
#if ENABLE_BFRAME
					cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0);
					qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
					qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));
#else
					qpel_gmv_x_of_refp_cu = qpel_gmv_x - ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2));
					qpel_gmv_y_of_refp_cu = qpel_gmv_y - ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2));
#endif // ENABLE_BFRAME
					if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + sub_w + x_offset_max <= (160 + 4) &&
						(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + sub_h + y_offset_max <= (128 + 4)))
					{
#if MD_PRINT_ENH
						if (mode_type == MODE_SKIP)
							printf("<MODE_SKIP:SUB_TMVP> ");
						else if (mode_type == MODE_DIR)
							printf("<MODE_DIR:SUB_TMVP> ");
						printf("out of ref0 window ERROR: sub cu %d=(%d %d %d %d)", k, x, y, sub_w, sub_h);
						printf("mv=(");
						printf("%d %d", mv_sbtmvp[MV_X], mv_sbtmvp[MV_Y]);
						printf(")\n ");
#if ENABLE_BFRAME
						printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
							cur_refw_idx,
							CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
							CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
						);
						printf("\n");
#endif 
						fflush(stdout);
#endif
						core_sbtmvp->isValid = FALSE;
						return FALSE;
					}

					get_pred_y_64(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
						cur_refw_idx,
#endif // ENBALE_BFRAME
						pred_tmp);
#if DEBUG_CU
					if (isProblem)
					{
						printf("CU %d (%d,%d) (%d,%d)\n", k, mv_sbtmvp[0], mv_sbtmvp[1], qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu);
					}
#endif
					//copy pred_tmp to pred output
					//int offset = h * 64 + w;
					for (int i = 0; i < sub_h; i++)
					{
						for (int j = 0; j < sub_w; j++)
						{
							pred_y[h + i][w + j] = pred_tmp[i][j];//[offset + i * 64 + j]  [i * 64 + j]
#if DEBUG_CU
							if (isProblem)
							{
								printf("%d,", pred_tmp[i][j]);
							}
#endif
						}
#if DEBUG_CU
						if (isProblem)
						{
							printf("\n");
						}
#endif
					}

					/*FILE* sbtmvp = fopen("sbtmvp.csv", "w");
					for (int i = 0; i < 64; i++) {
						for (int j = 0; j < 64; j++) {
							pel curr = pred[i * 64 + j];
							fprintf(sbtmvp, "%d, ", (int)curr);
						}
						fprintf(sbtmvp, "\n");
					}
					fclose(sbtmvp);*/
				}
				else {
#if MD_PRINT
					printf("%s:%d:sbtmvp refi is invalid\n", __FUNCTION__, 3267);
#endif
				}
			}
		}
		else {
#endif
			if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + w + x_offset_max <= (160 + 4) &&
				(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + h + y_offset_max <= (128 + 4)))
			{
#if MD_PRINT
				if (mode_type == MODE_SKIP)
					printf("<MODE_SKIP> ");
				else if (mode_type == MODE_INTER)
					printf("<MODE_INTER> ");
				else if (mode_type == MODE_DIR)
					printf("<MODE_DIR> ");
				printf("out of ref0 window ERROR: cu=(%d %d %d %d)", x, y, w, h);
				printf("mv=(");
				printf("%d %d", mv_t[MV_X], mv_t[MV_Y]);
				printf(")\n ");
#if ENABLE_BFRAME
				printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
					cur_refw_idx,
					CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
					CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
				);
				printf("\n");
#endif 
				printf("\n");
				fflush(stdout);
#endif
				return FALSE;
			}

			get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
				cur_refw_idx,
#endif // ENBALE_BFRAME
				pred_y);
#if SUB_TMVP
		}
#endif
		//++bidx;
		ref0_valid = 1;
	}
#if !ENABLE_BFRAME
	else {
#if MD_PRINT
		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3213);
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
			return 1;
		}
	}

	mv_t[0] = mv[REFP_1][0];
	mv_t[1] = mv[REFP_1][1];
	static pel pred_ref1[64][64];
	if (REFI_IS_VALID(refi[REFP_1]))
	{
		/* backward */
		//pel(*pred)[64] = bidx ? pred_snd : pred_y;
		//COM_PIC* ref_pic;
		//ref_pic = refp[refi[REFP_1]][REFP_1].pic;
		if (x_ext + (mv[REFP_1][0]) < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + (mv[REFP_1][1]) < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + (mv[REFP_1][0]) + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + (mv[REFP_1][1]) + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		int qpel_gmv_x;
		int qpel_gmv_y;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[1]);
		int dx = qpel_gmv_x & 0x3;
		int dy = qpel_gmv_y & 0x3;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

		S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));

		//if (channel != CHANNEL_C)
		//{
		//if (!((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < SWH)) { return FALSE; }
		if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + w + x_offset_max <= (160 + 4) &&
			(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + h + y_offset_max <= (128 + 4)))
		{
#if MD_PRINT
			if (mode_type == MODE_SKIP)
				printf("<MODE_SKIP> ");
			else if (mode_type == MODE_INTER)
				printf("<MODE_INTER> ");
			else if (mode_type == MODE_DIR)
				printf("<MODE_DIR> ");
			printf("out of ref1 window ERROR: cu=(%d %d %d %d)", x, y, w, h);
			printf("mv=(");
			printf("%d %d", mv_t[MV_X], mv_t[MV_Y]);
			printf(")\n");
			printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
				cur_refw_idx,
				CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
				CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
			);
			printf("\n");
			fflush(stdout);
#endif
			return FALSE;
		}

		get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref, cur_refw_idx, pred_ref1);
		//}

//		if (channel != CHANNEL_L)
//		{
//#if CHROMA_NOT_SPLIT
//			assert(w >= 8 && h >= 8);
//#endif
//			com_mc_c(mv[REFP_1][0], mv[REFP_1][1], ref_pic->u, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_chroma, pred_stride >> 1, pred[U_C], w >> 1, h >> 1, bit_depth);
//			com_mc_c(mv[REFP_1][0], mv[REFP_1][1], ref_pic->v, qpel_gmv_x, qpel_gmv_y, ref_pic->stride_chroma, pred_stride >> 1, pred[V_C], w >> 1, h >> 1, bit_depth);
//		}
		//bidx++;
		ref1_valid = 1;
	}

	if (ref0_valid && ref1_valid)
	{
		//if (channel != CHANNEL_C)
		//{
#if SIMD_MC
		average_16b_no_clip_sse(pred_y[0], pred_snd[0], pred_y[0], 64, 64, 64, w, h);
#else
		//pel(*p0)[64] = pred_y;
		//pel(*p1)[64] = pred_snd;
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				pred_y[j][i] = (pred_y[j][i] + pred_ref1[j][i] + 1) >> 1;
			}
		}
#endif
		//}

//		if (channel != CHANNEL_L)
//		{
//#if SIMD_MC
//			w >>= 1;
//			h >>= 1;
//			pred_stride >>= 1;
//			average_16b_no_clip_sse(pred_buf[U_C], pred_snd[U_C], pred_buf[U_C], pred_stride, pred_stride, pred_stride, w, h);
//			average_16b_no_clip_sse(pred_buf[V_C], pred_snd[V_C], pred_buf[V_C], pred_stride, pred_stride, pred_stride, w, h);
//#else
//			p0 = pred_buf[U_C];
//			p1 = pred_snd[U_C];
//			p2 = pred_buf[V_C];
//			p3 = pred_snd[V_C];
//			w >>= 1;
//			h >>= 1;
//			pred_stride >>= 1;
//			for (j = 0; j < h; j++)
//			{
//				for (i = 0; i < w; i++)
//				{
//					p0[i] = (p0[i] + p1[i] + 1) >> 1;
//					p2[i] = (p2[i] + p3[i] + 1) >> 1;
//				}
//				p0 += pred_stride;
//				p1 += pred_stride;
//				p2 += pred_stride;
//				p3 += pred_stride;
//			}
//#endif
//		}
	}
	else
	{
		if (ref0_valid)
		{
			/////
		}
		else if (ref1_valid)
		{
			for (int j = 0; j < h; j++)
			{
				for (int i = 0; i < w; i++)
				{
					pred_y[j][i] = pred_ref1[j][i];
				}
			}
		}
		else
		{
#if MD_PRINT
			printf("%s:%d:all refs are invalid\n", __FUNCTION__, 7922);
#endif
		}
	}
#endif
	return 1;

#endif
}

BOOL rdo_mc_y_64(u8 mode_type,
	S14 CtrPosSKIP[2][2][2], Arbitor& ref,
	int x_pos, int y_pos, int cu_width, int cu_height,
	s16 mv[2][2], s16 mv_t[2][2], U11 stride, U4 bit_depth, U2 channel,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred_y[64][64]
#if SUB_TMVP
	, U13 pic_w, U13 pic_h, u8 rdo_idx, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	int x = x_pos;
	int y = y_pos;
	int w = cu_width;
	int h = cu_height;

	if (mode_type == 0)
	{
		int dx;
		int dy;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;

		int qpel_gmv_x;
		int qpel_gmv_y;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0][0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[0][1]);
		int qpel_gmv_x_of_refp_cu = qpel_gmv_x - (((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2);
		int qpel_gmv_y_of_refp_cu = qpel_gmv_y - (((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2);
		dx = qpel_gmv_x & 0x3;
		dy = qpel_gmv_y & 0x3;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

		if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu_width + x_offset_max <= (160 + 4) &&
			(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu_height + y_offset_max <= (128 + 4)))
		{
#if MD_PRINT
			printf("Error: cu=(%d %d %d %d)",
				x_pos,
				y_pos,
				cu_width,
				cu_height
			);
			printf("mv=(");
			printf("%d %d", mv_t[REFP_0][MV_X], mv_t[REFP_0][MV_Y]);
			printf(") ");
			printf("\n");
			fflush(stdout);
#endif
			return FALSE;
		}
#if SUB_TMVP
		int min_clip[2];
		int max_clip[2];
		min_clip[0] = (-(1 << 7) - 4) << 2;
		min_clip[1] = (-(1 << 7) - 4) << 2;
		max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
		max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
		if (rdo_idx == 0 && cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE)
		{
			int x = x_pos;
			int y = y_pos;
			int sub_w = cu_width / SBTMVP_NUM_1D;
			int sub_h = cu_height / SBTMVP_NUM_1D;
			int h = 0;
			int w = 0;
			int tmp_x = x;
			int tmp_y = y;
			pel pred_tmp[64][64];
			for (int k = 0; k < SBTMVP_NUM; k++)
			{
				if (core_sbtmvp->sbTmvp[k].ref_idx[0] >= 0) {
					w = (k % 2) * sub_w;
					h = (k / 2) * sub_h;
					x = tmp_x + w;
					y = tmp_y + h;

					int x_ext = x << 2;
					int y_ext = y << 2;
					int w_ext = sub_w << 2;
					int h_ext = sub_h << 2;

					/*mv[0][0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv[0][1] = core_sbtmvp->sbTmvp[k].mv[0][1];
					mv_t[0][0] = mv[0][0];
					mv_t[0][1] = mv[0][1];*/

					/*if (x_ext + ((int)mv[0]) < min_clip[0]) {
						mv_t[0][0] = ((s16)(min_clip[0] - x_ext));
					}
					if (y_ext + ((int)mv[1]) < min_clip[1]) {
						mv_t[0][1] = ((s16)(min_clip[1] - y_ext));
					}
					if (x_ext + ((int)mv[0]) + w_ext - 4 > max_clip[0]) {
						mv_t[0][0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
					}
					if (y_ext + ((int)mv[1]) + h_ext - 4 > max_clip[1]) {
						mv_t[0][1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
					}*/

					/*x_ext_of_ctu = x_ext & (1 << 7 >> 1 << 2) - 1;
					y_ext_of_ctu = y_ext & (1 << 7 >> 1 << 2) - 1;
					qpel_gmv_x_of_ctu = x_ext_of_ctu + ((int)mv_t[0]);
					qpel_gmv_y_of_ctu = y_ext_of_ctu + ((int)mv_t[1]);
					qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
					qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);*/
					int mv_sbtmvp[2];
					mv_sbtmvp[0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv_sbtmvp[1] = core_sbtmvp->sbTmvp[k].mv[0][1];
					int x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
					int y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);

					if (x_ext + (mv_sbtmvp[0]) < min_clip[0]) {
						mv_sbtmvp[0] = ((min_clip[0] - x_ext));
					}
					if (y_ext + (mv_sbtmvp[1]) < min_clip[1]) {
						mv_sbtmvp[1] = ((min_clip[1] - y_ext));
					}
					if (x_ext + (mv_sbtmvp[0]) + w_ext - 4 > max_clip[0]) {
						mv_sbtmvp[0] = ((max_clip[0] - x_ext - w_ext + 4));
					}
					if (y_ext + (mv_sbtmvp[1]) + h_ext - 4 > max_clip[1]) {
						mv_sbtmvp[1] = ((max_clip[1] - y_ext - h_ext + 4));
					}

					int qpel_gmv_x_of_ctu = x_ext_of_ctu + (mv_sbtmvp[0]);
					int qpel_gmv_y_of_ctu = y_ext_of_ctu + (mv_sbtmvp[1]);
					qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
					qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);
					if (!((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + sub_w + 4 <= SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 &&
						(qpel_gmv_y_of_refp_cu >> 2) + sub_h + 4 <= SWH)) {
						return FALSE;
					}

					get_pred_y_64(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
						0,
#endif // ENABLE_BFRAME
						pred_tmp);
					//copy pred_tmp to pred output
					//int offset = h * 64 + w;
					for (int i = 0; i < sub_h; i++)
					{
						for (int j = 0; j < sub_w; j++)
						{
							pred_y[h + i][w + j] = pred_tmp[i][j];//[offset + i * 64 + j]  [i * 64 + j]
						}
					}

					/*FILE* sbtmvp = fopen("sbtmvp.csv", "w");
					for (int i = 0; i < 64; i++) {
						for (int j = 0; j < 64; j++) {
							pel curr = pred[i * 64 + j];
							fprintf(sbtmvp, "%d, ", (int)curr);
						}
						fprintf(sbtmvp, "\n");
					}
					fclose(sbtmvp);*/
				}
				else {
#if MD_PRINT
					printf("%s:%d:sbtmvp refi is invalid\n", __FUNCTION__, 3267);
#endif
				}
			}
		}
		else {
#endif
			get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
				0,
#endif // ENABLE_BFRAME
				pred_y);
#if SUB_TMVP
		}
#endif
	}
	else if (mode_type == 1)
	{
		int qpel_gmv_x;
		int qpel_gmv_y;
		int fme = 0;
		int out = 0;
		int dx;
		int dy;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;
		/* forward */
#if ENABLE_BFRAME
		qpel_gmv_x = (x << 2) + ((int)mv_t[lidx][0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[lidx][1]);
#else
		qpel_gmv_x = (x << 2) + ((int)mv_t[0][0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[0][1]);
#endif

		int qpel_gmv_x_of_refp_cu_64 = qpel_gmv_x - (((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2);
		int qpel_gmv_y_of_refp_cu_64 = qpel_gmv_y - (((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2);
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
#endif

#if ENABLE_BFRAME
		get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu_64, qpel_gmv_y_of_refp_cu_64, dx, dy, ref, lidx, pred_y);
#else
		get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu_64, qpel_gmv_y_of_refp_cu_64, dx, dy, fme2mdrefbuf, pred_y);
#endif
	}

	return 1;
}

BOOL com_mc_y_64(u8 mode_type, CU_POS* cu, U13 pic_w, U13 pic_h, S3 refi[2], s16 mv[2][2], int stride_luma,
	pel pred_y[64][64], U4 bit_depth, U2 channel,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS], Arbitor& ref
#if ENABLE_BFRAME
	, U1 lidx
#endif
#if SUB_TMVP
	, u8 rdo_idx, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	int scup = cu->scup;
	int x = cu->x_pos;
	int y = cu->y_pos;
	int w = cu->cu_width;
	int h = cu->cu_height;
	s16 mv_t[2][2];
//#pragma HLS array_partition variable=mv_t complete dim=0
	int mc_flag = 1;
	int qpel_gmv_x_of_refp_cu = 0;
	int qpel_gmv_y_of_refp_cu = 0;

	mv_clip_1(x, y, pic_w, pic_h, w, h, refi, mv, mv_t);

	if ((mode_type == 0 || mode_type == 1) && ((refi[0]) >= 0)) {
		if (!rdo_mc_y_64(mode_type,
			CtrPosSKIP, ref,
			x, y, w, h, mv, mv_t, stride_luma, bit_depth, channel,
#if ENABLE_BFRAME
			lidx,
#endif // ENABLE_BFRAME
			pred_y
#if SUB_TMVP
			, pic_w, pic_h, rdo_idx, core_sbtmvp
#endif
		))
			return FALSE;
	}
	else {
#if MD_PRINT
		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3213);
#endif
	}
	return 1;
}

BOOL mc_skip_y_64(U7 w, U7 h, MD_COM_MODE* mod_info_curr,
	U13 pic_width, U13 pic_height, pel pred_cache[64][64], U4 bit_depth,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	Arbitor& ref
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp, int mode_idx
#endif
)
{
	{
		CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };
		if (!com_mc_cu_y_64(w, h, &cu, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
			SWW, pred_cache, bit_depth, Fetch_Ref_window_ptr,
			CtrPos2MD,
#if ENABLE_BFRAME
			CtrPos2MD_ref1,
#endif
			ref
			, MODE_SKIP
#if SUB_TMVP
			, core_sbtmvp, mode_idx, mod_info_curr->umve_flag
#endif	
		)) {
			return 0;
		}
	}
	return 1;
}

BOOL make_skip_resi_64(int idx, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_COM_MODE* mod_info_curr,
	U13 pic_width, U13 pic_height, U3 cu_width_log2, U3 cu_height_log2, pel* org_y,
	int stride_org_l, U7 cu_width, U7 cu_height, U4 bit_depth,
	S3 refi_for_ip[2], S14 mv_for_ip[2][2],U8* umve_flag, U8* skip_idx)
{
	if (!derive_mode_info_64(idx, skip_rdo, skip_rdo_mode, merge_rdo, mod_info_curr, refi_for_ip, mv_for_ip,umve_flag,skip_idx)) {
		return 0;
	}

	//CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };
	//if (!com_mc_cu_y_64(w, h, &cu, pic_width, pic_height, mod_info_curr->refi[0], mod_info_curr->mv[0],
	//	SWW, RefWindowLCU, pred_cache, 8, RefWindowLCU_t)) {
	//	return 0;
	//}

	//enc_diff_16b_64(w, h, cu_width_log2, cu_height_log2, org_y, pred_cache, stride_org_l, cu_width, cu_width, resi_t);
	//dist_pred[0] = enc_ssd_16b_1_64(w, h, cu_width_log2, cu_height_log2, pred_cache, org_y, 1 << cu_width_log2, 64, 8);
	//mod_info_curr->cu_mode = 3;
	//cu_nz_cln(mod_info_curr->num_nz);
	//mod_info_curr->tb_part = ((int)SIZE_2Nx2N);
	return 1;
}

void intra_inter_pred_64(U7 w, U7 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, u16 avail_tb, u8 rdo_idx,
	MD_FW* md_fw_ptr, SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MD_COM_MODE* mod_info_curr_ptr,
	u8 rdo_list[MD_MAX_RDO_NUM], U1 valid_mode[17], u8 intra_mode_list[IPRED_LIST_NUM], pel src_le_temp[128 + 3], pel src_up_temp[128 + 3],
	S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], pel orgY[4096],
	u8* curr_mvr_for_re, u8* mode_type_for_ssd_fz, s8* inter_mode_idx_for_ssd_fz, U1* valid_flag_for_ssd_fz, U1* is_intra_for_quant,
	S3	mod_info_refi_for_re_fz[2], s16 mod_info_mvd_for_re_fz[2][2], S14 mod_info_mv_for_up[2][2], s16 mod_info_affine_mvd_for_re_fz[2][4][2],
	S8 mod_info_ipm_for_re_fz[4][2],
	pel orgY_for_ssd_forced_zeros[4096],
	s16 diff[64][64], s16 pred_for_ssd_forced_zeros[64][64],
	U4 bit_depth, 
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS], U8* skip_idx_out, U8* umve_flag_out,
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif
	//pel fme2mdrefbuf[2560][8][2][16]
	U5* num_refp_for_ssd, U5* num_refp_for_re,
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[64][64], pel pred_y_inter2[2][64][64], pel pred_y_intra[5][64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
#if USE_SPEED_LEVEL
	, U8 speed_level
#endif
)
{
	s8 intra_mode_idx;
	s8 inter_mode_idx;
	u8 mode_type = 0;
	u8 curr_mvr = 0;
	U1 valid_flag = 0;
	U1 is_intra;

	S3 refi_for_ip[2];
	s16 mvd_for_ip[2][2];
	S14 mv_for_ip[2][2];
	static s16 affine_mvd_for_ip[2][4][2];
	S8 ipm_for_ip[4][2];

	//static pel pred_for_diff[64][64];
	//s8 intra_mode_idx;

	inter_mode_idx = rdo_list[rdo_idx];
	U8 skip_idx = mod_info_curr_ptr->skip_idx;
	U8 umve_flag = mod_info_curr_ptr->umve_flag;
	
#pragma HLS ARRAY_PARTITION variable=orgY cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=orgY_for_ssd_forced_zeros cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=pred_for_ssd_forced_zeros cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=diff cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=pred_for_diff cyclic factor=16 dim=2
#if USE_SPEED_LEVEL
	int extra_intra = 0;
	if (speed_level == 1)
		extra_intra = 0;
	else if (speed_level == 2)
		extra_intra = 5;
	else
		extra_intra = 10;
#else
	int extra_intra = 0;
#endif

	if (inter_mode_idx >= 17)
	{
		is_intra = 1;
		intra_mode_idx = intra_mode_list[inter_mode_idx - 17];
		inter_mode_idx = -1;
	}
	else
	{
		is_intra = 0;
		intra_mode_idx = 0;
	}

	if (is_intra != 0) {

		Ipred_64_diff(w, h, orgY, orgY_for_ssd_forced_zeros, pred_y_intra[rdo_idx], pred_for_ssd_forced_zeros, diff);

		ipm_for_ip[0][0] = intra_mode_idx;
		ipm_for_ip[0][1] = -1;
		valid_flag = 1;
	}
	else {
		static s16 mv_all[2][5][2];
		static int best_mv_uni_inner[2][4][2];

#if ENABLE_BFRAME
		u8 num_refp_cnt = md_fw_ptr->num_refp[mv_dir->lidx];
#else
		u8 num_refp_cnt = md_fw_ptr->num_refp;
#endif

		int refi_cur = 0;
		valid_flag = valid_mode[inter_mode_idx];
		static U2 mode_type_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 };
		static u8 curr_mvr_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 1, 2 };
		/*if (valid_flag == 1) */ {
			mode_type = mode_type_table[inter_mode_idx];

			curr_mvr = curr_mvr_table[inter_mode_idx];
			U14 pic_height = md_fw_ptr->pic_height_in_scu << 2;
			U14 pic_width = md_fw_ptr->pic_width_in_scu << 2;
			if (mode_type == 0 || mode_type == 1)
			{
				if (mode_type == 1) {
					mod_info_curr_ptr->cu_mode = 1;
					mod_info_curr_ptr->affine_flag = 0;
					//refi_for_ip[0] = 0;
					//refi_for_ip[1] = 0;
					for (int i = 0; i < 2; i++) {
						for (int j = 0; j < 2; j++) {
							mv_for_ip[i][j] = 0;
							mvd_for_ip[i][j] = 0;
						}
					}
#if ENABLE_BFRAME
					U1 lidx = mv_dir->lidx;
#else
					U1 lidx = 0;
#endif
					mv_for_ip[lidx][0] = mv_copy[lidx][0];
					mv_for_ip[lidx][1] = mv_copy[lidx][1];
					mvd_for_ip[lidx][0] = mvd_copy[lidx][0];
					mvd_for_ip[lidx][1] = mvd_copy[lidx][1];
					refi_for_ip[0] = refi_copy[0];
					refi_for_ip[1] = refi_copy[1];
				}
				else if (mode_type == 2)
				{
					assert(mode_type != 2);
				}
				else
				{
					//make_skip_resi_64(inter_mode_idx, skip_rdo, skip_rdo_mode, &merge_rdo, mod_info_curr_ptr, pic_width, pic_height,
					//	cu_width_log2, cu_height_log2, orgY, 64, w, h, bit_depth,
					//	refi_for_ip, mv_for_ip, &umve_flag, &skip_idx);
					derive_mode_info_64(inter_mode_idx, skip_rdo, skip_rdo_mode, &merge_rdo, mod_info_curr_ptr, refi_for_ip, mv_for_ip, &umve_flag, &skip_idx);
				}

				if (mode_type == 0) {
					enc_diff_16b_64(cu_width_log2, cu_height_log2, orgY, orgY_for_ssd_forced_zeros, pred_y_inter2[rdo_idx-1], pred_for_ssd_forced_zeros, 64, w, w, diff);
				}
				else if (mode_type == 1) {
					enc_diff_16b_64(cu_width_log2, cu_height_log2, orgY, orgY_for_ssd_forced_zeros, pred_y_inter1, pred_for_ssd_forced_zeros, 64, w, w, diff);
				}
				//enc_diff_16b_64(cu_width_log2, cu_height_log2, orgY, orgY_for_ssd_forced_zeros, pred_for_diff, pred_for_ssd_forced_zeros, 64, w, w, diff);
			}
			else {
				for (int i = 0; i < 64; i++)
#pragma HLS PIPELINE II=4
					for (int j = 0; j < 64; j++)
					{
						orgY_for_ssd_forced_zeros[i * 64 + j] = orgY[i * 64 + j];
						//pred_for_ssd_forced_zeros[i][j] = pred_for_diff[i][j];
					}
			}
		}
	}

	*curr_mvr_for_re = curr_mvr;
	*mode_type_for_ssd_fz = mode_type;
	*inter_mode_idx_for_ssd_fz = inter_mode_idx;
	*valid_flag_for_ssd_fz = valid_flag;
	*is_intra_for_quant = is_intra;
	memcpy_1d<S3, 2>(refi_for_ip, mod_info_refi_for_re_fz);
	memcpy_2d<s16, 2, 2>(mvd_for_ip, mod_info_mvd_for_re_fz);
	memcpy_2d<S14, 2, 2>(mv_for_ip, mod_info_mv_for_up);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 2; k++)
				mod_info_affine_mvd_for_re_fz[i][j][k] = affine_mvd_for_ip[i][j][k];
	memcpy_2d<S8, 4, 2>(ipm_for_ip, mod_info_ipm_for_re_fz);

	*skip_idx_out = skip_idx;
	*umve_flag_out = umve_flag;
	*num_refp_for_ssd = md_fw_ptr->num_refp[0];
	*num_refp_for_re = md_fw_ptr->num_refp[0];
}


int enc_eco_cbf_est_skip_64(U1 tb_avaliable, int num_nz[4][3], U2 pred_mode,
	S8 ipm[4][2], U2 tree_status, U3 cu_width_log2, U3 cu_height_log2)
{
	U4 enc_est = 0;
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				if (cu_width_log2 > 6 || cu_height_log2 > 6) {
					enc_est++;
				}
				else {
					enc_est++;
				}

				return 0;

			}
		}
		if (tb_avaliable) {
			enc_est++;
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);
			enc_est += 1;

		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
			}
		}
	}
	return enc_est;
}

int enc_bit_est_flag_skip_64(s32 slice_type, U2 tree_status, u8 umve_flag, u8 umve_idx, u8 affine_flag, u8 skip_idx,
	U3 cu_width_log2, U3 cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8 ipm[MAX_NUM_PB][2], u8 num_of_hmvp_cand)
{
	int enc_est = 0;
	int umve_enable_flag = 1;
	enc_est++;
	if (umve_enable_flag) {
		enc_est++;
	}
	if (umve_flag) {
		enc_est += encode_umve_idx_est((umve_idx));
	}
	else {
		if (cu_width_log2 >= 4 && cu_height_log2 >= 4) {
			enc_est++;
		}
		if (affine_flag) {
			enc_est += sbac_write_truncate_unary_sym_est((skip_idx), 5);
		}
		else {
			enc_est += encode_skip_idx_cu_est((skip_idx), num_of_hmvp_cand, slice_type);
		}
	}

	if (tree_status != TREE_C) {
		enc_est += enc_eco_cbf_est_skip_64(0, num_nz, 2, ipm, tree_status, cu_width_log2, cu_height_log2);
	}
	else if (!((2) == 0 && (ipm[0][0]) == 33
		&& (ipm[0][1]) == 0)) {
		enc_est += 2;

	}
	return enc_est;
}

static void init_scan(u16* scan, int size_x, int size_y, int scan_type)
{

//#pragma HLS INLINE off
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

//#pragma HLS LOOP_TRIPCOUNT max=127
			/* decreasing loop */
			if (l % 2) {
				x = (l < size_x - 1 ? l : size_x - 1);
				y = (0 > l - (size_x - 1) ? 0 : l - (size_x - 1));
				while (x >= 0 && y < size_y) {

//#pragma HLS LOOP_TRIPCOUNT max=64
					scan[pos] = ((u16)(y * size_x + x));
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

//#pragma HLS LOOP_TRIPCOUNT max=64
					scan[pos] = ((u16)(y * size_x + x));
					pos++;
					x++;
					y--;
				}
			}
		}
	}
}
static int enc_eco_run_est(u32 sym)
{

//#pragma HLS INLINE
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

//#pragma HLS INLINE
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
		////#pragma HLS LOOP_TRIPCOUNT max=11
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
static int enc_eco_run_length_cc_est_2(s16* coef, int log2_w, int log2_h, int num_sig)
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

//#pragma HLS loop_tripcount max=4096
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

//#pragma HLS loop_tripcount max=4096
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


void rate_est_64(U8 cu_width, U8 cu_height, u8 tree_status, u8 cons_pred_mode,
	s64 lambda_y_for_re, int num_nz_for_re, u8 mode_type_for_re, u8 curr_mvr_for_re, U1 is_intra_for_re,
	MD_FW* md_fw_ptr, MD_COM_MODE* mod_info_curr_ptr,
	int md_fw_ptr_refp_ptr_for_re[17][2], S3 mod_info_refi_for_re[2], s16 mod_info_mvd_for_re[2][2], s16 mod_info_affine_mvd_for_re[2][4][2], u8 mod_info_mpm_for_re[4][2], S8 mod_info_ipm_for_re[4][2],
	pel qcoef_for_re[64][64],
	int* num_nz_for_up, u8* mode_type_for_up, u8* curr_mvr_for_up, U1* is_intra_for_up,
	S3 mod_info_refi_for_up[2], s16 mod_info_mvd_for_up[2][2], u8 mod_info_mpm_for_up[4][2], S8 mod_info_ipm_for_up[4][2],
	s64 flag_cost[2], U5 num_refp_for_re, U2 cu_mode_cp, U8 skip_idx, U8 umve_flag)
{
	flag_cost[0] = 0;
	int num_nz_re[4][3];
	cu_nz_cln(num_nz_re);
	num_nz_re[0][0] = num_nz_for_re;
#if ENABLE_BFRAME
	U16 flag_est = enc_bit_est_flag_64(md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr_for_re, tree_status, cons_pred_mode,
		cu_mode_cp, umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag,
		1, skip_idx, mod_info_refi_for_re, mod_info_curr_ptr->mvp_from_hmvp_flag,
		mod_info_curr_ptr->smvd_flag, num_refp_for_re, mod_info_affine_mvd_for_re, mod_info_mvd_for_re, mod_info_curr_ptr->cu_width_log2,
		mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re,
		md_fw_ptr->cur_ptr, md_fw_ptr_refp_ptr_for_re, 8, mod_info_mpm_for_re, is_intra_for_re);
#else
	U16 flag_est = enc_bit_est_flag_64(md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr_for_re, tree_status, cons_pred_mode,
		mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag,
		1, mod_info_curr_ptr->skip_idx, mod_info_refi_for_re, mod_info_curr_ptr->mvp_from_hmvp_flag,
		mod_info_curr_ptr->smvd_flag, md_fw_ptr->num_refp, mod_info_affine_mvd_for_re, mod_info_mvd_for_re, mod_info_curr_ptr->cu_width_log2,
		mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re,
		md_fw_ptr->cur_ptr, md_fw_ptr_refp_ptr_for_re, 8, mod_info_curr_ptr->mpm, is_intra_for_re);
#endif

	U16 coef_est;

#if RATE_EST_SWITCH
	/*if (cu_height == 64)
		coef_est = enc_eco_coef_64_1_new(cu_width, cu_height, qcoef_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2);
	else
		coef_est = enc_eco_coef_64_2_new(cu_width, cu_height, qcoef_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2);*/
	coef_est = enc_eco_coef_64(cu_width, cu_height, qcoef_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2);
#else
	static s16 coef_tmp[4096];
	for (int i = 0; i < cu_height; i++)
	{
		for (int j = 0; j < cu_width; j++)
		{
			coef_tmp[i * cu_width + j] = qcoef_for_re[i][j];
		}
	}
	coef_est = enc_eco_run_length_cc_est_2(coef_tmp, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re[0][0]);
#endif
	/*FILE* file1 = NULL;
	file1 = fopen("coef.txt", "w+");
	for (int i = 0; i < 4096; i++)
	{
		fprintf(file1, "coef_y_pb_part[%d]=%d\n", i, qcoef_for_re[i]);
	}
	fclose(file1);
	printf("flag_est=%d coef_est=%d\n", flag_est, coef_est);*/
	U16 bit_cnt = flag_est + coef_est;
	flag_cost[0] += (bit_cnt * lambda_y_for_re + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;

	if (is_intra_for_re == 0) {
		if (mode_type_for_re == 0) {
			flag_cost[1] = 0;
			flag_est = enc_bit_est_flag_skip_64(md_fw_ptr->slice_type, tree_status, mod_info_curr_ptr->umve_flag,
				mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, mod_info_curr_ptr->skip_idx,
				mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re, 8);
			flag_cost[1] += (flag_est * lambda_y_for_re + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
		}
	}
	*curr_mvr_for_up = curr_mvr_for_re;
	*mode_type_for_up = mode_type_for_re;
	*is_intra_for_up = is_intra_for_re;
	*num_nz_for_up = num_nz_for_re;
	memcpy_1d<S3, 2>(mod_info_refi_for_re, mod_info_refi_for_up);
	memcpy_2d<s16, 2, 2>(mod_info_mvd_for_re, mod_info_mvd_for_up);
	memcpy_2d<u8, 4, 2>(mod_info_mpm_for_re, mod_info_mpm_for_up);
	memcpy_2d<S8, 4, 2>(mod_info_ipm_for_re, mod_info_ipm_for_up);
}

void memcpy_1d_2_1d_64(s16 des[64 * 64], s16 src[64 * 64], int n, int flag)
{
//#pragma HLS INLINE off
//#pragma HLS ARRAY_PARTITION variable=des cyclic factor=256 dim=1
//#pragma HLS ARRAY_PARTITION variable=src cyclic factor=256 dim=1
	int z;
	if (flag) {
		if (n == 512)
			for (z = 0; z < 512; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
		else if (n == 1024)
			for (z = 0; z < 1024; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
		else if (n == 2048)
			for (z = 0; z < 2048; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
		else if (n == 4096)
			for (z = 0; z < 4096; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
	}
	else {
		if (n == 512)
			for (z = 0; z < 512; z++)
//#pragma HLS UNROLL
				des[z] = 0;
		else if (n == 1024)
			for (z = 0; z < 1024; z++)
//#pragma HLS UNROLL
				des[z] = 0;
		else if (n == 2048)
			for (z = 0; z < 2048; z++)
//#pragma HLS UNROLL
				des[z] = 0;
		else if (n == 4096)
			for (z = 0; z < 4096; z++)
//#pragma HLS UNROLL
				des[z] = 0;
	}
}
void memcpy_1d_2_1d_64(u8 des[64 * 64], s16 src[64 * 64], int n, int flag)
{
//#pragma HLS INLINE off
//#pragma HLS ARRAY_PARTITION variable=des cyclic factor=256 dim=1
//#pragma HLS ARRAY_PARTITION variable=src cyclic factor=256 dim=1
	int z;
	if (flag) {
		if (n == 512)
			for (z = 0; z < 512; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
		else if (n == 1024)
			for (z = 0; z < 1024; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
		else if (n == 2048)
			for (z = 0; z < 2048; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
		else if (n == 4096)
			for (z = 0; z < 4096; z++)
//#pragma HLS UNROLL
				des[z] = src[z];
	}
	else {
		if (n == 512)
			for (z = 0; z < 512; z++)
//#pragma HLS UNROLL
				des[z] = 0;
		else if (n == 1024)
			for (z = 0; z < 1024; z++)
//#pragma HLS UNROLL
				des[z] = 0;
		else if (n == 2048)
			for (z = 0; z < 2048; z++)
//#pragma HLS UNROLL
				des[z] = 0;
		else if (n == 4096)
			for (z = 0; z < 4096; z++)
//#pragma HLS UNROLL
				des[z] = 0;
	}
}
void memcpy_2d_2_1d_64(s16 des[64 * 64], s16 src[64][64], int w, int h)
{
//#pragma HLS INLINE off
//#pragma HLS ARRAY_PARTITION variable=des cyclic factor=256 dim=1
//#pragma HLS ARRAY_PARTITION variable=src complete dim=1
//#pragma HLS ARRAY_PARTITION variable=src cyclic factor=4 dim=2
	if (w == 8 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
//#pragma HLS UNROLL
				des[i * 8 + j] = src[i][j];
	}
	else if (w == 64 && h == 8) {
		for (int i = 0; i < 8; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
	else if (w == 16 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 16; j++)
//#pragma HLS UNROLL
				des[i * 16 + j] = src[i][j];
	}
	else if (w == 64 && h == 16) {
		for (int i = 0; i < 16; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
	else if (w == 32 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 32; j++)
//#pragma HLS UNROLL
				des[i * 32 + j] = src[i][j];
	}
	else if (w == 64 && h == 32) {
		for (int i = 0; i < 32; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
	else if (w == 64 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
}
void memcpy_2d_2_1d_64(u8 des[64 * 64], s16 src[64][64], int w, int h)
{
//#pragma HLS INLINE off
//#pragma HLS ARRAY_PARTITION variable=des cyclic factor=256 dim=1
//#pragma HLS ARRAY_PARTITION variable=src complete dim=1
//#pragma HLS ARRAY_PARTITION variable=src cyclic factor=4 dim=2
	if (w == 8 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
//#pragma HLS UNROLL
				des[i * 8 + j] = src[i][j];
	}
	else if (w == 64 && h == 8) {
		for (int i = 0; i < 8; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
	else if (w == 16 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 16; j++)
//#pragma HLS UNROLL
				des[i * 16 + j] = src[i][j];
	}
	else if (w == 64 && h == 16) {
		for (int i = 0; i < 16; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
	else if (w == 32 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 32; j++)
//#pragma HLS UNROLL
				des[i * 32 + j] = src[i][j];
	}
	else if (w == 64 && h == 32) {
		for (int i = 0; i < 32; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
	else if (w == 64 && h == 64) {
		for (int i = 0; i < 64; i++)
//#pragma HLS UNROLL
			for (int j = 0; j < 64; j++)
//#pragma HLS UNROLL
				des[i * 64 + j] = src[i][j];
	}
}

void update_rdo_luma_64(U7 w, U7 h,
	U1 valid_flag_for_up, s64 dist_pred_for_up, s32 dist, s64 cost_best_temp_for_up, int num_nz_for_up, u8 mode_type_for_up, U1 is_intra_for_up, u8 curr_mvr_for_up,
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr, MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr,
	S3 mod_info_refi_for_up[2], s16 mod_info_mvd_for_up[2][2], S14 mod_info_mv_for_up[2][2], u8 mod_info_mpm_for_up[4][2], S8 mod_info_ipm_for_up[4][2],
	s64 flag_cost[2], pel pred_for_update[64][64], pel rec_for_update[64][64], pel qcoef_for_update[64][64],
	s64* cost_best, U2* cu_mode_cp, int* tb_part_cp, U8 skip_idx, U8 umve_flag
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
#if USE_ROI_MODE_CONFIG
	, U1 valid_mode_config[3]
#endif
)
{
	//if (!valid_flag_for_up)
	//	return;
#ifdef UPDATE_OPT
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->coef_y cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->reco_y cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=qcoef_for_update cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=rec_for_update cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=pred_for_update cyclic factor=16 dim=2
#endif
	s64 cost_pb_best = MAX_S64;
	s64 cost = 0;
	U7 pb_w = w;
	U7 pb_h = h;
	s64 cost_pb_temp = 0;
	int bin_cnt_luma[1] = { 0 };
	U13 i;
	U13 j;
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	s16 wxh = s16(w) * s16(h);
	cu_nz_cln(mod_info_curr_ptr->num_nz);
	mod_info_curr_ptr->num_nz[0][0] = num_nz_for_up;
	if (is_intra_for_up || mod_info_curr_ptr->num_nz[0][0])
	{
		cost = dist;
	}

	cost += flag_cost[0];

#if USE_ROI_MODE_CONFIG
	if (valid_mode_config[2] == 0 && mod_info_curr_ptr->cu_mode == 0)
		cost = MAX_S64;

	if (valid_mode_config[1] == 0 && mod_info_curr_ptr->cu_mode == 1)
		cost = MAX_S64;

	if (valid_mode_config[0] == 0 && (mod_info_curr_ptr->cu_mode == 2 || mod_info_curr_ptr->cu_mode == 3))
		cost = MAX_S64;
#endif

	if (is_intra_for_up == 0) {
		if (mod_info_curr_ptr->num_nz[0][0]) {
			//if (cost < cost_best_temp_for_up) {
				cost_best_temp_for_up = cost;
			//}
		}

		U2 lidx;
		U7 cu_width = w;
		U7 cu_height = h;
		U2 vertex;
		U2 vertex_num;


		static U11 skip_mode_2_thread_s64[64] = { (1000), (1002), (1004), (1005), (1007), (1009), (1010), (1012), (1014), (1016), (1017), (1019), (1020), (1022), (1024), (1026), (1027), (1029), (1030), (1032), (1034), (1036), (1037), (1039), (1040), (1042), (1044), (1046), (1047), (1049), (1050), (1052), (1054), (1056), (1057), (1059), (1060), (1062), (1064), (1066), (1067), (1069), (1070), (1072), (1074), (1076), (1077), (1079), (1080), (1082), (1084), (1086), (1087), (1089), (1090), (1092), (1094), (1096), (1097), (1099), (1100), (1102), (1104), (1106) };

#if SUB_TMVP
		BOOL isSubTMVP = w >= SBTMVP_MIN_SIZE && h >= SBTMVP_MIN_SIZE && *cu_mode_cp >=MODE_SKIP && skip_idx==0 && umve_flag==0 ;
		BOOL rdoRun = TRUE;
		if (isSubTMVP)
			rdoRun = core_sbtmvp->isValid;
#endif

		if (cost_best_temp_for_up < *cost_best
#if SUB_TMVP
			&& rdoRun
#endif	
			)
		{

			best_info_ptr->cu_mode = *cu_mode_cp;
			best_info_ptr->pb_part = mod_info_curr_ptr->pb_part;
			best_info_ptr->tb_part = *tb_part_cp;

			best_info_ptr->umve_flag = umve_flag;
			*cost_best = cost_best_temp_for_up;
			if (best_info_ptr->cu_mode != 0) {
				if (best_info_ptr->cu_mode == 2 || best_info_ptr->cu_mode == 3) {
					best_info_ptr->mvr_idx = ((u8)0);
				}
				else {
					best_info_ptr->mvp_from_hmvp_flag = mod_info_curr_ptr->mvp_from_hmvp_flag;
					best_info_ptr->mvr_idx = curr_mvr_for_up;
				}
				best_info_ptr->refi[0] = mod_info_refi_for_up[0];
				best_info_ptr->refi[1] = mod_info_refi_for_up[1];
				for (lidx = 0; lidx < 2; lidx++) {
					best_info_ptr->mv[lidx][0] = mod_info_mv_for_up[lidx][0];
					best_info_ptr->mv[lidx][1] = mod_info_mv_for_up[lidx][1];
					best_info_ptr->mvd[lidx][0] = mod_info_mvd_for_up[lidx][0];
					best_info_ptr->mvd[lidx][1] = mod_info_mvd_for_up[lidx][1];
				}
				best_info_ptr->smvd_flag = mod_info_curr_ptr->smvd_flag;
				best_info_ptr->affine_flag = mod_info_curr_ptr->affine_flag;
				if (best_info_ptr->affine_flag) {

					vertex_num = (best_info_ptr->affine_flag) + 1;
					for (lidx = 0; lidx < 2; lidx++) {
						for (vertex = 0; vertex < 4; vertex++) {
							if (vertex < vertex_num)
							{
								best_info_ptr->affine_mv[lidx][vertex][0] = mod_info_curr_ptr->affine_mv[lidx][vertex][0];
								best_info_ptr->affine_mv[lidx][vertex][1] = mod_info_curr_ptr->affine_mv[lidx][vertex][1];
								best_info_ptr->affine_mvd[lidx][vertex][0] = mod_info_curr_ptr->affine_mvd[lidx][vertex][0];
								best_info_ptr->affine_mvd[lidx][vertex][1] = mod_info_curr_ptr->affine_mvd[lidx][vertex][1];
							}

						}
					}
				}
				//memcpy_2d_2_1d_64(best_info_ptr->pred_y, pred_for_update, w, h);

				if (best_info_ptr->cu_mode == 2) {
					if (((int)(best_info_ptr->umve_flag)) != 0) {
						best_info_ptr->umve_idx = mod_info_curr_ptr->umve_idx;
					}
					else {
						best_info_ptr->skip_idx = skip_idx;
					}

					for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
						best_info_ptr->num_nz[i_numnz][0] = 0;
					}


				}
				else {
					if (best_info_ptr->cu_mode == 3) {
						if (best_info_ptr->umve_flag) {
							best_info_ptr->umve_idx = mod_info_curr_ptr->umve_idx;
						}
						else {
							best_info_ptr->skip_idx = skip_idx;
						}
					}

					for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
						best_info_ptr->num_nz[i_numnz][0] = mod_info_curr_ptr->num_nz[i_numnz][0];
					}

				}

				for (U8 i = 0; i < h; i++)
				{
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
					for (U8 j = 0; j < 64; j++)
					{
            if(j<w){
						  cu_data_temp_ptr->coef_y[i][j] = best_info_ptr->cu_mode == 2 ? 0 : qcoef_for_update[i][j];
            }
					}
				}
			}

			U1 b_num_nz = mod_info_curr_ptr->num_nz[0][0] == 0;
			if (b_num_nz)
			{
				for (U8 i = 0; i < h; i++)
				{
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
					for (U8 j = 0; j < 64; j++)
					{
						if (j < w) {
							cu_data_temp_ptr->reco_y[i][j] = pred_for_update[i][j];
						}
					}
				}
			}
			else
			{
				for (U8 i = 0; i < h; i++)
				{
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
					for (U8 j = 0; j < 64; j++)
					{
						if (j < w) {
							cu_data_temp_ptr->reco_y[i][j] = rec_for_update[i][j];
						}
					}
				}
			}

			//coef_rec_write_flag[0] = 1;
		}
	}
	else {
		cost_pb_temp += cost;
		if (cost_pb_temp < *cost_best) {

			best_info_ptr->cu_mode = MODE_INTRA;
			best_info_ptr->affine_flag = 0;
			*cost_best = cost_pb_temp;
			//memcpy_1d_2_1d_64(best_info_ptr->coef_y, qcoef_for_update, wxh, 1);
			//memcpy_2d_2_1d_64(best_info_ptr->rec_y, rec_for_update, w, h);
			for (U8 i = 0; i < h; i++)
			{
#pragma HLS PIPELINE II=4
#pragma HLS LOOP_TRIPCOUNT max = 64
				for (U8 j = 0; j < 64; j++)
				{
					if (j < w) {
						cu_data_temp_ptr->coef_y[i][j] = qcoef_for_update[i][j];
						cu_data_temp_ptr->reco_y[i][j] = rec_for_update[i][j];
					}
				}
			}
			for (i = 0; i < 4; ++i) {
				for (j = 0; j < 2; ++j) {
					best_info_ptr->mpm[i][j] = mod_info_mpm_for_up[i][j];
					best_info_ptr->ipm[i][j] = mod_info_ipm_for_up[i][j];
				}
			}
			for (i = 0; i < 4; ++i) {
				for (j = 0; j < 3; ++j) {
					best_info_ptr->num_nz[i][j] = mod_info_curr_ptr->num_nz[i][j];
				}
			}
		}
	}

}



void rdo_prepare_64(U7 w, U7 h, MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	SKIP_MERGE_INPUT* skip_in, MD_COM_MODE* mod_info_curr_ptr,
	MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	U1 valid_mode[17], U1 amvr_enable_flag,
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
	U3 cu_width_log2, U3 cu_height_log2,
	u8 ipred_list[IPRED_LIST_NUM], pel src_le_temp[2 * 64 + 3], pel src_up_temp[2 * 64 + 3], u16* avail_tb, u8 rmd_mode[IPRED_LIST_NUM],
	U3 qt_depth, U5 all_inter_mode_num,
	U7 cu_width, U7 cu_height,
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, MV_DIR* mv_dir,
#endif
	pel* org_y,
	U2 tree_status, u8 curr_cons_pred_mode,
	u8 inter_num[1], u8 intra_num[1], u8 rdo_list[MD_MAX_RDO_NUM], SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], SKIP_RDO* skip_rdo,
	SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], strFetch_ref_window* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	U3 max_rdo_num = MD_MAX_RDO_NUM;
	U3 max_inter = 0;
	;
	U14 sub_x = mod_info_curr_ptr->x_pos;
	U14 sub_y = mod_info_curr_ptr->y_pos;

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;
#if ENABLE_BFRAME
	s8 num_refp_cnt[REFP_NUM] = { md_fw_ptr->num_refp[0], md_fw_ptr->num_refp[1] };
#else
	u8 num_refp_cnt = md_fw_ptr->num_refp;
#endif


	s8 refi_cur = 0;

	int num_amvr;
	/*	if (amvr_enable_flag) {
			num_amvr = 5;
		}
		else*/ {
		num_amvr = 1;
	}

		inter_num[0] = 0;
		intra_num[0] = 0;
		if (!(md_fw_ptr->slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA)) {
			max_inter = (curr_cons_pred_mode == ONLY_INTER) ? MD_MAX_RDO_NUM : MD_MAX_RDO_NUM_INTER;
			prepare_luma_inter_64(w, h, md_fw_ptr,
				me_line_map_ptr_64,
				up_nebs, left_nebs, skip_in, &mod_info_curr_ptr[0],
				skip_rdo, skip_rdo_mode, merge_rdo, md_input_ptr,
				num_amvr, md_input_ptr->lambda_mv, refi_cur, num_refp_cnt, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mvp_all,
				valid_mode, amvr_enable_flag, fme_mv_info,
#if ENABLE_BFRAME
				me_mv_dir, mv_dir,
#endif
				sub_pel_mv, skip_mode, Fetch_Ref_window_ptr
#if SUB_TMVP
				, core_sbtmvp
#endif
			);


			if (valid_mode[9])
			{
				rdo_list[inter_num[0]] = 9;
				inter_num[0]++;
			}

			//if (valid_mode[4])
			//{
			//	rdo_list[inter_num[0]] = 4;
			//	inter_num[0]++;
			//}
			for (int mode_index = 0; mode_index < 4; mode_index++)
			{
				if (inter_num[0] >= max_inter)
					break;
				if (valid_mode[mode_index] == 1)
				{
					rdo_list[inter_num[0]] = mode_index;
					inter_num[0]++;
				}
			}
		}


		if (!(IS_INTER_SLICE(md_fw_ptr->slice_type) && curr_cons_pred_mode == ONLY_INTER))
		{
			int pred_cnt[1] = { MD_MAX_RDO_NUM };

			prepare_intra_info_64(w, h, md_fw_ptr,
				me_line_map_ptr_64,
				&mod_info_curr_ptr[0],
				IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32, IP_buffer_ptr_16x64, IP_buffer_ptr_64x16, IP_buffer_ptr_8x64, IP_buffer_ptr_64x8,
				MPM_buffer_ptr_64x64, MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32, MPM_buffer_ptr_16x64, MPM_buffer_ptr_64x16, MPM_buffer_ptr_8x64, MPM_buffer_ptr_64x8,
				IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32, IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16, IP_map_scu_ptr_8x64, IP_map_scu_ptr_64x8,
				cu_width_log2, cu_height_log2, pix_x, pix_y, sub_x, sub_y, ipred_list, pred_cnt, src_le_temp, src_up_temp, avail_tb, rmd_mode
#if USE_SPEED_LEVEL
				, md_input_ptr->speed_level
#endif
			);
#if USE_SPEED_LEVEL
			if (md_input_ptr->speed_level == 1)
				max_rdo_num = IPD_RDO_CNT;
			else if (md_input_ptr->speed_level == 2)
				max_rdo_num = EXTRA_RDO_MODE;
			else
				max_rdo_num = IPD_RDO_CNT + EXTRA_RDO_MODE;
#endif
			intra_num[0] = avs3_min(pred_cnt[0], (int)(max_rdo_num - inter_num[0]));
			for (int intra_index = 0; intra_index < intra_num[0]; intra_index++)
			{
#pragma HLS loop_tripcount max = 5
				rdo_list[inter_num[0] + intra_index] = all_inter_mode_num + intra_index;
			}
		}
}


void rdo_skip_64_phase1(SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], int idx, MD_COM_MODE* mod_info_curr, U1* flag_continue)
{

#pragma HLS INLINE OFF
	S3 mod_info_curr_refi[2];
#pragma HLS ARRAY_PARTITION variable=mod_info_curr_refi complete dim=0

	SKIP_ELEMENT* mode = skip_mode + idx;
	mod_info_curr->cu_mode = MODE_SKIP;
	mod_info_curr->umve_flag = mode->umve_flag;
	mod_info_curr->umve_idx = mode->umve_idx;
	mod_info_curr->affine_flag = mode->affine_flag;
	mod_info_curr->skip_idx = mode->skip_idx;
	mod_info_curr->mv[0][0] = mode->mvp[REFP_0][MV_X];
	mod_info_curr->mv[0][1] = mode->mvp[REFP_0][MV_Y];
	mod_info_curr->refi[0] = mode->refi[REFP_0];
	mod_info_curr_refi[0] = mode->refi[REFP_0];
#if ENABLE_BFRAME
	mod_info_curr->mv[1][0] = mode->mvp[REFP_1][MV_X];
	mod_info_curr->mv[1][1] = mode->mvp[REFP_1][MV_Y];
	mod_info_curr->refi[1] = mode->refi[REFP_1];
	mod_info_curr_refi[1] = mode->refi[REFP_1];
#else
	mod_info_curr->mv[1][0] = 0;
	mod_info_curr->mv[1][1] = 0;
	mod_info_curr->refi[1] = -1;
	mod_info_curr_refi[1] = -1;
#endif

	* flag_continue = 0;
	if (mode->valid == 0) { *flag_continue = 1; }
#if ENABLE_BFRAME
	if (!REFI_IS_VALID(mod_info_curr_refi[REFP_0]) && !REFI_IS_VALID(mod_info_curr_refi[REFP_1])) { *flag_continue = 1; }
#else
	if (mod_info_curr_refi[0] == -1) { *flag_continue = 1; }
#endif


}



s64 enc_ssd_16b_1_64_wsc_v1(U3 w_log2, U3 h_log2, s16 src1[64][64], s16 src1_cp[64][64], s16 src2[4096], U4 bit_depth)
{
	int offset2 = 0;
#pragma HLS INLINE off
	const int shift = (bit_depth - 8) << 1;
	s64 ssd = 0;
	U8 i, j;
	int diff;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	for (i = 0; i < 64; i++) {
#pragma HLS PIPELINE
		for (j = 0; j < 64; j++) {
#pragma HLS UNROLL
			if ((i < h) && (j < w)) {
				diff = (/*(int)*/src1[i][j]) - (/*(int)*/src2[i * 64 + j]);
			}
			else {
				diff = 0;
			}
			ssd += ((diff * diff >> shift));
			src1_cp[i][j] = src1[i][j];
		}
		//offset2 += s_src2;
	}
	return ssd;
	//add by xujch
}


static int encode_umve_idx_est_wsc(int umve_idx)
{
	U3 enc_est;
	enc_est = 0;
	if (umve_idx < 20)
		enc_est++;
	else
	{
		enc_est += 2;


	}
	if (umve_idx < 4 || (umve_idx >= 20 && umve_idx < 24))
		enc_est++;
	else
	{
		enc_est += 2;


	}

	enc_est += 2;
	return enc_est;
}


static int sbac_write_truncate_unary_sym_est_wsc(U8 sym, U8 max_num)
{

#pragma HLS INLINE
	U8 enc_est;
	enc_est = (max_num - (1) > sym + (1) ? sym + (1) : max_num - (1));
	return enc_est;
}
static int encode_skip_idx_cu_est_wsc(u8  skip_idx, u8 num_hmvp_cands, u8 slice_type)
{

	U8 enc_est = 0;
	U8 val;
	U8 max_skip_num;
	// for P slice, change 3, 4, ..., 13 to 1, 2, ..., 11
	if ((slice_type) == 2 && skip_idx > 0) {
		skip_idx -= 2;
	}
	val = skip_idx;
	max_skip_num = (((slice_type) == 2 ? 2 : (PRED_DIR_NUM)+1)) + num_hmvp_cands;
	enc_est += val;
	if (skip_idx != max_skip_num - 1) {
		enc_est++;
	}
	return enc_est;
}


int enc_eco_cbf_est_skip_64_wsc(/*RDOQ_MODEL* model_cnt,*/ U1 tb_avaliable, U2 pred_mode,
	S8 ipm[4][2], U2 tree_status, U3 cu_width_log2, U3 cu_height_log2)
{
	U4 enc_est = 0;
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				if (cu_width_log2 > 6 || cu_height_log2 > 6) {
					enc_est++;
				}
				else {
					enc_est++;
				}

				return 0;

			}
		}
		if (tb_avaliable) {
			enc_est++;
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
			//	model_cnt->cbf[1]++;
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			//	model_cnt->cbf[0]++;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);
			enc_est += 1;
			//	model_cnt->cbf[0]++;

		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
				//		model_cnt->cbf[1]++;
			}
		}
	}
	return enc_est;
}



int enc_bit_est_flag_skip_64_wsc(/*RDOQ_MODEL* model_cnt, */s32 slice_type, U2 tree_status, u8 umve_flag, u8 umve_idx, u8 affine_flag, u8 skip_idx,
	U3 cu_width_log2, U3 cu_height_log2, S8 ipm[MAX_NUM_PB][2])
{
	int enc_est = 0;
	int umve_enable_flag = 1;
	enc_est++;
	if (umve_enable_flag) {
		enc_est++;
	}
	if (umve_flag) {
		enc_est += encode_umve_idx_est_wsc((umve_idx));
	}
	else {
		if (cu_width_log2 >= 4 && cu_height_log2 >= 4) {
			enc_est++;
		}
		if (affine_flag) {
			enc_est += sbac_write_truncate_unary_sym_est_wsc((skip_idx), 5);
		}
		else {
			enc_est += encode_skip_idx_cu_est_wsc((skip_idx), 8, slice_type);
		}
	}

	if (tree_status != TREE_C) {
		enc_est += enc_eco_cbf_est_skip_64_wsc(/*model_cnt,*/ 0, 2, ipm, tree_status, cu_width_log2, cu_height_log2);
	}
	else if (!((2) == 0 && (ipm[0][0]) == 33
		&& (ipm[0][1]) == 0)) {
		enc_est += 2;
		//		model_cnt->cbf[1]++;

	}
	return enc_est;
}




void rdo_skip_64_phase3(U7 w, U7 h, U3 cu_width_log2, U3 cu_height_log2, s16 pred_cache[64][64], s16 pred_cache_cp[64][64], s16 org_y[4096], U4 bit_depth,/* RDOQ_MODEL* model_cnt,*/
	U2 slice_type, U2 tree_status, s64 lambda_y, MD_COM_MODE* mod_info_curr, s64* cost
)
{
#pragma HLS INLINE OFF
	//pel pred_cache_tmp[64][64];// = {};
#pragma HLS ARRAY_PARTITION variable=pred_cache_tmp complete dim=2
	//skip64_pred(w, h, pred_cache, valid_falg0, valid_falg1, valid_falg0_return, pred_y0_tmp, pred_y1_tmp, pred_cache_tmp);
	s64 ssdcost = enc_ssd_16b_1_64_wsc_v1(cu_width_log2, cu_height_log2, pred_cache, pred_cache_cp, org_y, bit_depth);//
	//cu_nz_cln(mod_info_curr->num_nz);
	//mod_info_curr->tb_part = ((int)SIZE_2Nx2N);//////////////////
	U16 bit_est = enc_bit_est_flag_skip_64_wsc(/*model_cnt,*/ slice_type, tree_status, mod_info_curr->umve_flag, mod_info_curr->umve_idx,
		mod_info_curr->affine_flag, mod_info_curr->skip_idx, cu_width_log2, cu_height_log2, mod_info_curr->ipm);
	*cost = ssdcost + ((bit_est * lambda_y + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE);
}



void rdo_skip_64_phase4(ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr, int idx, U4 bit_depth,
	U1 flag_continue, U1 bool_flag, s64 cost, MD_COM_MODE_BEST* best_info_ptr, MD_COM_MODE* mod_info_curr,
	s16 pred_y_tmp[64][64], s16 reco_y_tmp[64][64], pel pred_cache[64][64], s64* cost_best_skip
#if SUB_TMVP               
	, CORE_SBTMVP* core_sbtmvp
#endif
)

{
#pragma HLS INLINE OFF

//#if SUB_TMVP

	COM_MOTION sbTmvp_cp[4];
	for (int k = 0; k < 4; k++) {
		sbTmvp_cp[k] = core_sbtmvp->sbTmvp[k];
	}


	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	//// if skip tmvp mode
	//if (idx == 0 && cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
	//	core_sbtmvp->sbTmvp_flag = 1;
	//}
	//else {
	//	core_sbtmvp->sbTmvp_flag = 0;
	//}
//#endif

	static s64 cost_best_c;
	if (idx == 0) {
		cost_best_c = MAX_S64;
	}
	if ((flag_continue == 1) || (bool_flag == 0)) {}
	else {
#if SUB_TMVP
		// if skip tmvp mode
		if (idx == 0 && cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
			core_sbtmvp->sbTmvp_flag = 1;
			if (core_sbtmvp->isValid == FALSE)
				return;
		}
		else {
			core_sbtmvp->sbTmvp_flag = 0;
		}
#endif

		if (cost < cost_best_c) {
#if SUB_TMVP
			core_sbtmvp->best_sbTmvp_flag = core_sbtmvp->sbTmvp_flag;
			if (core_sbtmvp->best_sbTmvp_flag) {
				for (int i = 0; i < 4; i++)//SBTMVP_NUM
				{
					core_sbtmvp->best_sbTmvp[i].mv[REFP_0][MV_X] = sbTmvp_cp[i].mv[REFP_0][MV_X];
					core_sbtmvp->best_sbTmvp[i].mv[REFP_0][MV_Y] = sbTmvp_cp[i].mv[REFP_0][MV_Y];
					core_sbtmvp->best_sbTmvp[i].mv[REFP_1][MV_X] = sbTmvp_cp[i].mv[REFP_1][MV_X];
					core_sbtmvp->best_sbTmvp[i].mv[REFP_1][MV_Y] = sbTmvp_cp[i].mv[REFP_1][MV_Y];
					core_sbtmvp->best_sbTmvp[i].ref_idx[REFP_0] = sbTmvp_cp[i].ref_idx[REFP_0];
					core_sbtmvp->best_sbTmvp[i].ref_idx[REFP_1] = sbTmvp_cp[i].ref_idx[REFP_1];
				}
				for (int lidx = 0; lidx < 2; lidx++)
				{
					best_info_ptr->mvd[lidx][0] = 0;
					best_info_ptr->mvd[lidx][1] = 0;
				}
			}
			//}
#endif


			best_info_ptr->cu_mode = mod_info_curr->cu_mode;
			best_info_ptr->pb_part = mod_info_curr->pb_part;
			best_info_ptr->tb_part = 0;// mod_info_curr->tb_part;

			best_info_ptr->umve_flag = mod_info_curr->umve_flag;
			cost_best_c = cost;

			best_info_ptr->mvr_idx = ((u8)0);
			best_info_ptr->refi[0] = mod_info_curr->refi[0];
			best_info_ptr->refi[1] = mod_info_curr->refi[1];
			for (int lidx = 0; lidx < 2; lidx++) {
#pragma HLS UNROLL
				best_info_ptr->mv[lidx][0] = mod_info_curr->mv[lidx][0];
				best_info_ptr->mv[lidx][1] = mod_info_curr->mv[lidx][1];
				best_info_ptr->mvd[lidx][0] = mod_info_curr->mvd[lidx][0];
				best_info_ptr->mvd[lidx][1] = mod_info_curr->mvd[lidx][1];
			}

#if SUB_TMVP
			if (core_sbtmvp->best_sbTmvp_flag) {
				for (int lidx = 0; lidx < 2; lidx++)
				{
					best_info_ptr->mvd[lidx][MV_X] = 0;
					best_info_ptr->mvd[lidx][MV_Y] = 0;
				}
			}
#endif
			best_info_ptr->smvd_flag = mod_info_curr->smvd_flag;
			/*best_info_ptr->affine_flag = mod_info_curr->affine_flag;
			if (best_info_ptr->affine_flag) {

				int vertex_num = (best_info_ptr->affine_flag) + 1;
				for (int lidx = 0; lidx < 2; lidx++) {
#pragma HLS UNROLL
					for (int vertex = 0; vertex < 4; vertex++) {
#pragma HLS UNROLL
						if (vertex < vertex_num)
						{
							best_info_ptr->affine_mv[lidx][vertex][0] = mod_info_curr->affine_mv[lidx][vertex][0];
							best_info_ptr->affine_mv[lidx][vertex][1] = mod_info_curr->affine_mv[lidx][vertex][1];
							best_info_ptr->affine_mvd[lidx][vertex][0] = mod_info_curr->affine_mvd[lidx][vertex][0];
							best_info_ptr->affine_mvd[lidx][vertex][1] = mod_info_curr->affine_mvd[lidx][vertex][1];
						}
					}
				}
			}*/

			if (((int)(best_info_ptr->umve_flag)) != 0) {
				best_info_ptr->umve_idx = mod_info_curr->umve_idx;
			}
			else {
				best_info_ptr->skip_idx = mod_info_curr->skip_idx;
			}

			for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
#pragma HLS UNROLL
				best_info_ptr->num_nz[i_numnz][0] = 0;
			}

			for (int i = 0; i < 64; i++) {
#pragma HLS PIPELINE
				for (int j = 0; j < 64; j++) {
#pragma HLS UNROLL
					//cu_data_temp_ptr->coef_y[i * 64 + j] = 0;
					cu_data_temp_ptr->coef_y[i][j] = 0;
					pred_y_tmp[i][j] = pred_cache[i][j];
					s16 pred = pred_cache[i][j];
					reco_y_tmp[i][j] = 0 > pred ? 0 : (pred > ((1 << bit_depth) - 1) ? ((1 << bit_depth) - 1) : pred);

				}
			}
		}

	}

	*cost_best_skip = cost_best_c;

}





void rdo_skip_64_dataflow_64_64(U8 w, U8 h, s64* cost_best, MD_COM_MODE_BEST* best_info_ptr,
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr, MD_COM_MODE* mod_info_curr,
	pel org_y[4096], SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], MERGE_RDO* merge,
	U13 pic_width, U13 pic_height, U2 slice_type, U2 tree_status, s64 lambda_y, int curr_cons_pred_mode, U4 bit_depth,
	U1 bool_flag,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	//pel fme2mdrefbuf[2560][8][2][16],
	pel pred_y_skip[12][64][64],
	s16 pred_y_tmp[64][64],
	s16 reco_y_tmp[64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif

)
{

#pragma HLS ARRAY_PARTITION variable=pred_y_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=reco_y_tmp complete dim=2

#pragma HLS ARRAY_PARTITION variable=org_y cyclic factor=64 dim=1

#pragma HLS ARRAY_PARTITION variable=CtrPos2MD complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPos2MD_ref1 complete dim=0

#pragma HLS ARRAY_PARTITION variable=best_info_ptr->pred_y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->reco_y cyclic factor=64 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->coef_y cyclic factor=64 dim=2

#pragma HLS ARRAY_PARTITION variable=best_info_ptr->affine_mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->affine_mvd complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->affine_mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->affine_mvd complete dim=0

#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mvd complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->mvd complete dim=0

#pragma HLS ARRAY_PARTITION variable=best_info_ptr->num_nz complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->refi complete dim=0

#pragma HLS ARRAY_PARTITION variable=skip_mode complete dim=0

	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U14 wxh = ((U14)cu_width) * cu_height;
	//pel pred_cache[64][64];// = {};
//#pragma HLS ARRAY_PARTITION variable=pred_cache complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_skip complete dim=3

	U1 flag_continue;
	//U1 bool_flag;

	s64 cost;
	s64 cost_best_skip;

	//static pel pred_y0_tmp[64][64];
	//static pel pred_y1_tmp[64][64];
//#pragma HLS ARRAY_PARTITION variable=pred_y0_tmp complete dim=2
//#pragma HLS ARRAY_PARTITION variable=pred_y1_tmp complete dim=2
	//U1 valid_falg0;
	//U1 valid_falg1;
	//U1 valid_falg0_return;
	COM_MOTION sbTmvp_cp[4];
#pragma HLS ARRAY_PARTITION variable=sbTmvp_cp complete dim=1
#pragma HLS ARRAY_PARTITION variable=sbTmvp_cp->ref_idx complete dim=0
#pragma HLS ARRAY_PARTITION variable=sbTmvp_cp->mv complete dim=0
	pel pred_y_skip_cp[64][64];
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_cp complete dim=2
	for (int idx = 0; idx < 12; ++idx)
	{
#pragma HLS DATAFLOW
		rdo_skip_64_phase1(skip_mode, idx, mod_info_curr, &flag_continue);

		rdo_skip_64_phase3(w, h, cu_width_log2, cu_height_log2, pred_y_skip[idx], pred_y_skip_cp, org_y, bit_depth, /*model_cnt,*/
			slice_type, tree_status, lambda_y, mod_info_curr, &cost);

		rdo_skip_64_phase4(cu_data_temp_ptr, idx, bit_depth, flag_continue, bool_flag, cost, best_info_ptr, mod_info_curr,
			pred_y_tmp, reco_y_tmp, pred_y_skip_cp, &cost_best_skip
#if SUB_TMVP
			, core_sbtmvp
#endif
		);
	}

	*cost_best = cost_best_skip;

}



void rdo_skip_64_64(U8 w, U8 h, s64* cost_best, MD_COM_MODE_BEST* best_info_ptr,
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr, MD_COM_MODE* mod_info_curr,
	pel org_y[4096], SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], MERGE_RDO* merge,
	U13 pic_width, U13 pic_height, U2 slice_type, U2 tree_status, s64 lambda_y, int curr_cons_pred_mode, U4 bit_depth,
	U1 bool_flag,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	//pel fme2mdrefbuf[2560][8][2][16]
	pel pred_y_skip[13][64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U14 wxh = ((U14)cu_width) * cu_height;
	s16 pred_y_tmp[64][64];//
	s16 reco_y_tmp[64][64];//
	pel pred_cache[64][64];//

	if (slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA) { return; }
	rdo_skip_64_dataflow_64_64(w, h, cost_best, best_info_ptr,
		cu_data_temp_ptr, mod_info_curr, org_y, skip_mode, merge,
		pic_width, pic_height, slice_type, tree_status, lambda_y, curr_cons_pred_mode, bit_depth,
		bool_flag,
		Fetch_Ref_window_ptr, CtrPos2MD,
#if ENABLE_BFRAME
		CtrPos2MD_ref1,
#endif
		//fme2mdrefbuf,
		pred_y_skip,
		pred_y_tmp,
		reco_y_tmp
#if SUB_TMVP
		, core_sbtmvp
#endif
	);
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->pred_y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_temp_ptr->reco_y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=pred_y_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=reco_y_tmp complete dim=2

	for (int i = 0; i < 64; i++)//cu_height
	{
#pragma HLS PIPELINE
		for (int j = 0; j < 64; j++)// cu_width
		{
			best_info_ptr->pred_y[i][j] = pred_y_tmp[i][j];
		}
	}

	for (int i = 0; i < 64; i++) {//cu_height
#pragma HLS PIPELINE
		for (int j = 0; j < 64; j++) {//cu_width									
			//cu_data_temp_ptr->reco_y[(i << cu_width_log2) + j] = reco_y_tmp[i][j];// 0 > pred ? 0 : (pred > ((1 << bit_depth) - 1) ? ((1 << bit_depth) - 1) : pred);
			cu_data_temp_ptr->reco_y[i][j] = reco_y_tmp[i][j];

		}

	}
}






void rdo_skip_64(U8 w, U8 h, s64* cost_best, MD_COM_MODE_BEST* best_info_ptr,
	ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr, MD_COM_MODE* mod_info_curr,
	pel* org_y, SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], MERGE_RDO* merge,
	U13 pic_width, U13 pic_height, U2 slice_type, U2 tree_status, s64 lambda_y, int curr_cons_pred_mode, U4 bit_depth,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	//pel fme2mdrefbuf[2560][8][2][16]
	pel pred_y_skip[13][64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U14 wxh = ((U14)cu_width) * cu_height;

	if (slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA) { return; }
	for (int idx = 0; idx < SKIP_MODE_NUM; ++idx)
	{
			SKIP_ELEMENT* mode = skip_mode + idx;
			if (mode->valid == 0) { continue; }

			mod_info_curr->cu_mode = MODE_SKIP;
			mod_info_curr->umve_flag = mode->umve_flag;
			mod_info_curr->umve_idx = mode->umve_idx;
			mod_info_curr->affine_flag = mode->affine_flag;
			mod_info_curr->skip_idx = mode->skip_idx;
			mod_info_curr->mv[0][0] = mode->mvp[REFP_0][MV_X];
			mod_info_curr->mv[0][1] = mode->mvp[REFP_0][MV_Y];
			mod_info_curr->refi[0] = mode->refi[REFP_0];
#if ENABLE_BFRAME
			mod_info_curr->mv[1][0] = mode->mvp[REFP_1][MV_X];
			mod_info_curr->mv[1][1] = mode->mvp[REFP_1][MV_Y];
			mod_info_curr->refi[1] = mode->refi[REFP_1];
#else
			mod_info_curr->mv[1][0] = 0;
			mod_info_curr->mv[1][1] = 0;
			mod_info_curr->refi[1] = -1;
#endif
			// skip index 1 and 2 for P slice
#if ENABLE_BFRAME
		if (!REFI_IS_VALID(mod_info_curr->refi[REFP_0]) && !REFI_IS_VALID(mod_info_curr->refi[REFP_1])) { continue; }
#else
		if (mod_info_curr->refi[0] == -1) { continue; }
#endif
		s64 cost = enc_ssd_16b_1_64(cu_width_log2, cu_height_log2, pred_y_skip[idx], org_y, cu_width, 64, bit_depth);
		cu_nz_cln(mod_info_curr->num_nz);
		mod_info_curr->tb_part = ((int)SIZE_2Nx2N);

		// calc bit est
		U16 bit_est = enc_bit_est_flag_skip_64(slice_type, tree_status, mod_info_curr->umve_flag, mod_info_curr->umve_idx, mod_info_curr->affine_flag, mod_info_curr->skip_idx, cu_width_log2, cu_height_log2, mod_info_curr->num_nz, mod_info_curr->ipm, 8);
		cost += (bit_est * lambda_y + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
#if SUB_TMVP
		// if skip tmvp mode
		if (idx == 0 && cu_width >= SBTMVP_MIN_SIZE && cu_height >= SBTMVP_MIN_SIZE) {
			core_sbtmvp->sbTmvp_flag = 1;
		}
		else {
			core_sbtmvp->sbTmvp_flag = 0;
		}
#endif
		// calc bit est
		if (cost < *cost_best) {
#if SUB_TMVP

			core_sbtmvp->best_sbTmvp_flag = core_sbtmvp->sbTmvp_flag;
			if (core_sbtmvp->best_sbTmvp_flag) {
				if (!core_sbtmvp->isValid)
					continue;
				for (int i = 0; i < SBTMVP_NUM; i++)
				{
					core_sbtmvp->best_sbTmvp[i].mv[REFP_0][MV_X] = core_sbtmvp->sbTmvp[i].mv[REFP_0][MV_X];
					core_sbtmvp->best_sbTmvp[i].mv[REFP_0][MV_Y] = core_sbtmvp->sbTmvp[i].mv[REFP_0][MV_Y];
					core_sbtmvp->best_sbTmvp[i].mv[REFP_1][MV_X] = core_sbtmvp->sbTmvp[i].mv[REFP_1][MV_X];
					core_sbtmvp->best_sbTmvp[i].mv[REFP_1][MV_Y] = core_sbtmvp->sbTmvp[i].mv[REFP_1][MV_Y];
					core_sbtmvp->best_sbTmvp[i].ref_idx[REFP_0] = core_sbtmvp->sbTmvp[i].ref_idx[REFP_0];
					core_sbtmvp->best_sbTmvp[i].ref_idx[REFP_1] = core_sbtmvp->sbTmvp[i].ref_idx[REFP_1];

				}
				for (int lidx = 0; lidx < 2; lidx++)
				{
					best_info_ptr->mvd[lidx][0] = 0;
					best_info_ptr->mvd[lidx][1] = 0;
				}
			}
			//}
#endif
			best_info_ptr->cu_mode = mod_info_curr->cu_mode;
			best_info_ptr->pb_part = mod_info_curr->pb_part;
			best_info_ptr->tb_part = mod_info_curr->tb_part;
			best_info_ptr->umve_flag = mod_info_curr->umve_flag;
			*cost_best = cost;

			best_info_ptr->mvr_idx = ((u8)0);
			best_info_ptr->refi[0] = mod_info_curr->refi[0];
			best_info_ptr->refi[1] = mod_info_curr->refi[1];
			for (int lidx = 0; lidx < 2; lidx++) {
//#pragma HLS UNROLL
				best_info_ptr->mv[lidx][0] = mod_info_curr->mv[lidx][0];
				best_info_ptr->mv[lidx][1] = mod_info_curr->mv[lidx][1];
				best_info_ptr->mvd[lidx][0] = mod_info_curr->mvd[lidx][0];
				best_info_ptr->mvd[lidx][1] = mod_info_curr->mvd[lidx][1];
			}
#if SUB_TMVP
			if (core_sbtmvp->best_sbTmvp_flag) {
				for (int lidx = 0; lidx < 2; lidx++)
				{
					best_info_ptr->mvd[lidx][MV_X] = 0;
					best_info_ptr->mvd[lidx][MV_Y] = 0;
				}
			}

			//core_sbtmvp->sbTmvp_flag = 0;
#endif
			best_info_ptr->smvd_flag = mod_info_curr->smvd_flag;
			for (int i = 0; i < cu_height; i++)
			{
//#pragma HLS loop_tripcount max = 64
				for (int j = 0; j < cu_width; j++)
				{
//#pragma HLS loop_tripcount max = 64
					best_info_ptr->pred_y[i][j] = pred_y_skip[idx][i][j];
				}
			}


			if (((int)(best_info_ptr->umve_flag)) != 0) {
				best_info_ptr->umve_idx = mod_info_curr->umve_idx;
			}
			else {
				best_info_ptr->skip_idx = mod_info_curr->skip_idx;
			}

			for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
//#pragma HLS UNROLL
				best_info_ptr->num_nz[i_numnz][0] = 0;
			}


			for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
//#pragma HLS UNROLL
				best_info_ptr->num_nz[i_numnz][0] = 0;
			}

			for (int i = 0; i < cu_height; i++)
			{
				for (int j = 0; j < cu_width; j++)
				{
					cu_data_temp_ptr->coef_y[i][j] = 0;
				}
			}

			for (int i = 0; i < 64; i++) {
//#pragma HLS UNROLL
				if (i < cu_height)
				{
					for (int j = 0; j < 64; j++) {
//#pragma HLS UNROLL
						if (j < cu_width)
						{
							s16 pred = pred_y_skip[idx][i][j];
							cu_data_temp_ptr->reco_y[i][j] = 0 > pred ? 0 : (pred > ((1 << bit_depth) - 1) ? ((1 << bit_depth) - 1) : pred);
						}
					}
				}
			}
		}

	}
}

void ssd_bitcnt_64(U2 tree_status, u8 cons_pred_mode,
	s64 lambdaY, u8 mode_type_for_ssd_fz, int inter_mode_idx_for_ssd_fz, U1 valid_flag_for_ssd_fz,
	MD_FW* md_fw_ptr, MD_COM_MODE* mod_info_curr,
	S3 mod_info_refi_for_re_fz[2], s16 mod_info_mvd_for_re_fz[2][2], s16 mod_info_affine_mvd_for_re_fz[2][4][2], u8	mod_info_mpm_for_re_fz[4][2], S8	mod_info_ipm_for_re_fz[4][2],
	pel orgY_for_ssd_forced_zeros[4096], pel pred_for_ssd_forced_zeros[64][64],
	s64* lambda_y_for_quant, u8* mode_type_for_re, U1* valid_flag_for_up, s64* dist_pred_for_up, s64* cost_best_temp_for_up,
	int md_fw_ptr_refp_ptr_for_re[17][2], S3 mod_info_refi_for_re[2], s16 mod_info_mvd_for_re[2][2], s16 mod_info_affine_mvd_for_re[2][4][2], u8 mod_info_mpm_for_re[4][2], S8 mod_info_ipm_for_re[4][2],
	pel orgY_for_ssd[4096], pel pred_for_rec[64][64], U5 num_refp_for_ssd, U2* cu_mode_cp, int* tb_part_cp, U8 skip_idx, U8 umve_flag)
{
	*mode_type_for_re = mode_type_for_ssd_fz;
	*valid_flag_for_up = valid_flag_for_ssd_fz;
	*lambda_y_for_quant = lambdaY;
	//memcpy_1d<S3, 2>(mod_info_refi_for_re_fz, mod_info_refi_for_re);
	//memcpy_2d<s16, 2, 2>(mod_info_mvd_for_re_fz, mod_info_mvd_for_re);
	//for (int i = 0; i < 2; i++)
	//	for (int j = 0; j < 4; j++)
	//		for (int k = 0; k < 2; k++)
	//			mod_info_affine_mvd_for_re[i][j][k] = mod_info_affine_mvd_for_re_fz[i][j][k];
	//memcpy_2d<u8, 4, 2>(mod_info_mpm_for_re_fz, mod_info_mpm_for_re);
	//memcpy_2d<S8, 4, 2>(mod_info_ipm_for_re_fz, mod_info_ipm_for_re);

	//memcpy_1d<pel, 4096>(orgY_for_ssd_forced_zeros, orgY_for_ssd);
	//memcpy_2d<pel, 64, 64>(pred_for_ssd_forced_zeros, pred_for_rec);
	//memcpy_2d<int, 17, 2>(md_fw_ptr->refp_ptr, md_fw_ptr_refp_ptr_for_re);
	mod_info_refi_for_re[0] = mod_info_refi_for_re_fz[0];
	mod_info_refi_for_re[1] = mod_info_refi_for_re_fz[1];

#pragma HLS ARRAY_PARTITION variable=pred_for_ssd_forced_zeros cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=orgY_for_ssd_forced_zeros cyclic factor=16 dim=1
#pragma HLS ARRAY_PARTITION variable=pred_for_rec cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=orgY_for_ssd cyclic factor=16 dim=1

	for (int i = 0; i < 64; i++)
	{
#pragma HLS PIPELINE II=4
		for (int j = 0; j < 64; j++)
		{
			pred_for_rec[i][j] = pred_for_ssd_forced_zeros[i][j];
			orgY_for_ssd[64 * i + j] = orgY_for_ssd_forced_zeros[64 * i + j];
			if (j < 2) {
				mod_info_mvd_for_re[j][0] = mod_info_mvd_for_re_fz[j][0];
				mod_info_mvd_for_re[j][1] = mod_info_mvd_for_re_fz[j][1];
				if (i < 4) {
					mod_info_mpm_for_re[i][j] = mod_info_mpm_for_re_fz[i][j];
					mod_info_ipm_for_re[i][j] = mod_info_ipm_for_re_fz[i][j];
					mod_info_affine_mvd_for_re[j][i][0] = mod_info_affine_mvd_for_re_fz[j][i][0];
					mod_info_affine_mvd_for_re[j][i][1] = mod_info_affine_mvd_for_re_fz[j][i][1];
				}
				if (i < 17)
					md_fw_ptr_refp_ptr_for_re[i][j] = md_fw_ptr->refp_ptr[i][j];
			}
		}
	}
	if (inter_mode_idx_for_ssd_fz < 0)
		return;
	s64 dist_pred = MAX_S64;
	s64 cost_best_temp = MAX_S64;
	u8 curr_mvr_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 1, 2 };
	int curr_mvr = curr_mvr_table[inter_mode_idx_for_ssd_fz];
	int cu_width_log2 = mod_info_curr->cu_width_log2;
	int cu_height_log2 = mod_info_curr->cu_height_log2;
	int bit_depth = md_fw_ptr->bit_depth_internal;
	int is_intra = 0;
	/*if (valid_flag_for_ssd_fz == 1) */ {
		if (mode_type_for_ssd_fz == 1 || mode_type_for_ssd_fz == 0)
		{

			dist_pred = enc_ssd_16b_1_64(cu_width_log2, cu_height_log2, pred_for_ssd_forced_zeros, orgY_for_ssd_forced_zeros, 1 << cu_width_log2, 64, bit_depth);
			int num_nz_fz[4][3];
			cu_nz_cln(num_nz_fz);
			*cu_mode_cp = mod_info_curr->cu_mode;
			//mod_info_curr->tb_part = ((int)SIZE_2Nx2N);
			*tb_part_cp = ((int)SIZE_2Nx2N);
			if (mode_type_for_ssd_fz == 0)
			{
				*cu_mode_cp = 3;
			}
			else
			{
				cost_best_temp = dist_pred;
#if ENABLE_BFRAME
				int bit_cnt = get_bitcnt_est_64_coef_0(md_fw_ptr->f_scu, curr_mvr, tree_status, 0, 2, cons_pred_mode, *cu_mode_cp,
					umve_flag, mod_info_curr->umve_idx, mod_info_curr->affine_flag, 1, skip_idx,
					mod_info_refi_for_re_fz, mod_info_curr->mvp_from_hmvp_flag, mod_info_curr->smvd_flag, num_refp_for_ssd, mod_info_affine_mvd_for_re_fz,
					mod_info_mvd_for_re_fz, mod_info_curr->cu_width_log2, mod_info_curr->cu_height_log2, *tb_part_cp, mod_info_curr->pb_part,
					num_nz_fz, mod_info_ipm_for_re_fz, md_fw_ptr->slice_type, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mod_info_mpm_for_re_fz, is_intra);
#else
				int bit_cnt = get_bitcnt_est_64_coef_0(md_fw_ptr->f_scu, curr_mvr, tree_status, 0, 2, cons_pred_mode, mod_info_curr->cu_mode,
					mod_info_curr->umve_flag, mod_info_curr->umve_idx, mod_info_curr->affine_flag, 1, mod_info_curr->skip_idx,
					mod_info_refi_for_re_fz, mod_info_curr->mvp_from_hmvp_flag, mod_info_curr->smvd_flag, md_fw_ptr->num_refp, mod_info_affine_mvd_for_re_fz,
					mod_info_mvd_for_re_fz, mod_info_curr->cu_width_log2, mod_info_curr->cu_height_log2, mod_info_curr->tb_part, mod_info_curr->pb_part,
					num_nz_fz, mod_info_ipm_for_re_fz, md_fw_ptr->slice_type, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mod_info_mpm_for_re_fz, is_intra);
#endif

				cost_best_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
			}
		}
	}
	*dist_pred_for_up = dist_pred;
	*cost_best_temp_for_up = cost_best_temp;
}


#if !LUMA_64_PIPELINE_DUMMY
void rdo_luma_64_pipeline(u8 all_rdo_num, U8 w, U8 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, U2 curr_cons_pred_mode, U4 bit_depth, u16 avail_tb,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr, ENC_CU_DATA_ARRAY_SIZE<64>* cu_data_temp_ptr,
	u8 rdo_list[MD_MAX_RDO_NUM], U1 valid_mode[17], u8 ipred_list[IPRED_LIST_NUM], pel src_le_temp[2 * 64 + 3], pel src_up_temp[2 * 64 + 3], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	pel orgY[64 * 64],
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif
	s64* cost_best,
	//pel fme2mdrefbuf[2560][8][2][16]
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[64][64], pel pred_y_inter2[2][64][64], pel pred_y_intra[5][64][64]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	static pel diff[64][64];
	static pel pred_for_ssd_forced_zeros[64][64];
	static pel pred_for_rec[64][64];
	static pel pred_for_update[64][64];
	static pel coef_h[64][64];
	static pel coef[64][64];
	static pel qcoef_for_dq[64][64];
	static pel qcoef_for_re[64][64];
	static pel qcoef_for_update[64][64];
	static pel itcoef_v[64][64];
	static pel rec_for_update[64][64];
	static pel orgY_for_ssd_forced_zeros[4096];
	static pel orgY_for_ssd[4096];
	u8	curr_mvr_for_re, curr_mvr_for_up;
	u8	mode_type_for_ssd_fz, mode_type_for_re, mode_type_for_up;
	s8	inter_mode_idx_for_ssd_fz;
	U1	valid_flag_for_ssd_fz, valid_flag_for_up;
	U1	is_intra_for_quant, is_intra_for_re, is_intra_for_up;
	int	num_nz_for_re, num_nz_for_ith, num_nz_for_itv, num_nz_for_up;
	s64	dist_pred_for_up = MAX_S64;
	s64	cost_best_temp_for_up = MAX_S64;
	int	md_fw_ptr_refp_ptr_for_re[17][2];
	s64	lambda_y_for_quant, lambda_y_for_re;
	S3	mod_info_refi_for_re_fz[2], mod_info_refi_for_re[2], mod_info_refi_for_up[2];
	static s16	mod_info_mvd_for_re_fz[2][2], mod_info_mvd_for_re[2][2], mod_info_mvd_for_up[2][2];
	static S14	mod_info_mv_for_up[2][2];
	static s16 mod_info_affine_mvd_for_re_fz[2][4][2], mod_info_affine_mvd_for_re[2][4][2];
	static u8	mod_info_mpm_for_re[4][2], mod_info_mpm_for_up[4][2];
	static S8	mod_info_ipm_for_re_fz[4][2], mod_info_ipm_for_re[4][2], mod_info_ipm_for_up[4][2];
	s64 flag_cost[2];
	s32 dist;
	U5 num_refp_for_ssd, num_refp_for_re;
	U2 cu_mode_cp;
	U8 skip_idx_cp, umve_flag_cp;
	int tb_part_cp;

#ifdef MD_CMOD
	for (u8 rdo_idx = 0; rdo_idx < all_rdo_num; rdo_idx++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=2
#elif USE_ENH
#if USE_EIPM_RMD
#if USE_SPEED_LEVEL
	int rdo_cnt = 5;
	if (md_input_ptr->speed_level == 1)
		rdo_cnt = IPD_RDO_CNT; //5
	else if (md_input_ptr->speed_level == 2)
		rdo_cnt = EXTRA_RDO_MODE;//10
	else
		rdo_cnt = IPD_RDO_CNT + EXTRA_RDO_MODE;//15
#else
	int rdo_cnt = 5;
#endif
	for (u8 rdo_idx = 0; rdo_idx < rdo_cnt; rdo_idx++)
#else
	for (u8 rdo_idx = 0; rdo_idx < all_rdo_num; rdo_idx++)
#endif
	{
//#pragma HLS LOOP_TRIPCOUNT max=7
#else
	static int count = 0;
	count++;
	//printf("count=%d\n", count);
	/*FILE* file = NULL;
	file = fopen("cost.txt", "w+");
*/
	for (u8 rdo_idx = 0; rdo_idx < 5; rdo_idx++)
	{
//#pragma HLS LOOP_TRIPCOUNT max=5
#endif
		//static pel pred_for_diff[8][8];
#pragma HLS DATAFLOW
#if !AZB_ENABLE
		intra_inter_pred_64(w, h, tree_status, cu_width_log2, cu_height_log2, avail_tb, rdo_idx,
			md_fw_ptr, skip_rdo, merge_rdo, skip_rdo_mode, mod_info_curr_ptr,
			rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY,
			&curr_mvr_for_re, &mode_type_for_ssd_fz, &inter_mode_idx_for_ssd_fz, &valid_flag_for_ssd_fz, &is_intra_for_quant,
			mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_mv_for_up, mod_info_affine_mvd_for_re_fz, mod_info_ipm_for_re_fz,
			orgY_for_ssd_forced_zeros, diff, pred_for_ssd_forced_zeros, md_fw_ptr->bit_depth_internal,
			md_input_ptr->CtrPos2MD, &skip_idx_cp, &umve_flag_cp,
#if ENABLE_BFRAME
			md_input_ptr->CtrPos2MD_ref1, mv_dir, Fetch_Ref_window_ptr,
#endif
			//fme2mdrefbuf,
			&num_refp_for_ssd, &num_refp_for_re,
			mv_copy, mvd_copy, refi_copy, pred_y_inter1, pred_y_inter2, pred_y_intra
#if SUB_TMVP
			, core_sbtmvp
#endif
#if USE_SPEED_LEVEL
			, md_input_ptr->speed_level
#endif
		);

		ssd_bitcnt_64(tree_status, curr_cons_pred_mode,
			md_input_ptr->lambda_y, mode_type_for_ssd_fz, inter_mode_idx_for_ssd_fz, valid_flag_for_ssd_fz,
			md_fw_ptr, mod_info_curr_ptr,
			mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_affine_mvd_for_re_fz, mod_info_curr_ptr->mpm, mod_info_ipm_for_re_fz,
			orgY_for_ssd_forced_zeros, pred_for_ssd_forced_zeros,
			&lambda_y_for_quant, &mode_type_for_re, &valid_flag_for_up, &dist_pred_for_up, &cost_best_temp_for_up,
			md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
			orgY_for_ssd, pred_for_rec, num_refp_for_ssd, &cu_mode_cp, &tb_part_cp, skip_idx_cp, umve_flag_cp);

		transform_hor_64(w, h, bit_depth, diff, coef_h);
		transform_ver_64(w, h, bit_depth, coef_h, coef);

		quant_nnz_64(cu_width_log2, cu_height_log2, 0, md_fw_ptr->slice_type, bit_depth, md_input_ptr->qp_y,
			lambda_y_for_quant, is_intra_for_quant,
			rdoq_array,
			coef,
			&lambda_y_for_re, &is_intra_for_re, &num_nz_for_re, &num_nz_for_itv,
			qcoef_for_dq, qcoef_for_re, qcoef_for_update);

		rate_est_64(w, h, tree_status, curr_cons_pred_mode,
			lambda_y_for_re, num_nz_for_re, mode_type_for_re, curr_mvr_for_re, is_intra_for_re,
			md_fw_ptr, mod_info_curr_ptr,
			md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
			qcoef_for_re,
			&num_nz_for_up, &mode_type_for_up, &curr_mvr_for_up, &is_intra_for_up,
			mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
			flag_cost, num_refp_for_re, cu_mode_cp, skip_idx_cp, umve_flag_cp);

		inv_transform_ver_64(w, h, bit_depth, md_input_ptr->qp_y, num_nz_for_itv, qcoef_for_dq, &num_nz_for_ith, &num_nz_for_up, itcoef_v);

		inv_transform_hor_64(w, h, bit_depth, tb_part_cp, cu_width_log2, cu_height_log2, 0,
			num_nz_for_ith,
			orgY_for_ssd, pred_for_rec, itcoef_v,
			&dist,
			pred_for_update, rec_for_update);

		
		update_rdo_luma_64(w, h,
			valid_flag_for_up, dist_pred_for_up, dist, cost_best_temp_for_up, num_nz_for_up, mode_type_for_up, is_intra_for_up, curr_mvr_for_up,
			cu_data_temp_ptr, mod_info_curr_ptr, best_info_ptr,
			mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mv_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
			flag_cost, pred_for_update, rec_for_update, qcoef_for_update,
			cost_best, &cu_mode_cp, &tb_part_cp, skip_idx_cp, umve_flag_cp
#if SUB_TMVP
			, core_sbtmvp
#endif
#if USE_ROI_MODE_CONFIG
			, md_input_ptr->valid_mode_config
#endif
		);
#if SET_MD_64_MAX_COST
		* cost_best = 1 << 25;
#endif
#if DEBUG_CU
		if (isProblem)
		{
			printf("RdoIdxCur:%d cu_mode=%d,umve_flag=%d,affine_flag=%d,skip_idx=%d,SbTmvpValid=%d Mv=(%d,%d)\n", rdo_idx, mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->affine_flag, mod_info_curr_ptr->skip_idx, core_sbtmvp->isValid, mod_info_curr_ptr->mv[0][0], mod_info_curr_ptr->mv[0][1]);
			for (int z = 0; z < h; z++)
			{
				for (int zz = 0; zz < w; zz++)
				{
					printf("%d,", pred_for_rec[z][zz]);
				}
				printf("\n");
			}
			printf("RdoIdxBest:%d cu_mode=%d,umve_flag=%d,affine_flag=%d,skip_idx=%d Mv=(%d,%d)\n", rdo_idx, best_info_ptr->cu_mode, best_info_ptr->umve_flag, best_info_ptr->affine_flag, best_info_ptr->skip_idx, best_info_ptr->mv[0][0], best_info_ptr->mv[0][1]);
		}
#endif
		/*fprintf(file, "cost_best_temp=%d\n", cost_best_temp_for_up);
		fprintf(file, "cost_best=%d\n", *cost_best);*/
#else
		if (md_fw_ptr->slice_type == 1)
		{
			intra_inter_pred_64(w, h, tree_status, cu_width_log2, cu_height_log2, avail_tb, rdo_idx,
				md_fw_ptr, skip_rdo, merge_rdo, skip_rdo_mode, mod_info_curr_ptr,
				rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY, RefWindowLCU, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_ref1, CtrPos_ref1, mv_dir, refwin_md_ref1, CtrPosFME_ref1, Fetch_Ref_window_ptr,
#endif
				& curr_mvr_for_re, &mode_type_for_ssd_fz, &inter_mode_idx_for_ssd_fz, &valid_flag_for_ssd_fz, &is_intra_for_quant,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_mv_for_up, mod_info_affine_mvd_for_re_fz, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, diff, pred_for_ssd_forced_zeros, md_fw_ptr->bit_depth_internal, refwin_md, CtrPosFME);

			ssd_bitcnt_64(tree_status, curr_cons_pred_mode,
				md_input_ptr->lambda_y, mode_type_for_ssd_fz, inter_mode_idx_for_ssd_fz, valid_flag_for_ssd_fz,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_affine_mvd_for_re_fz, mod_info_curr_ptr->mpm, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, pred_for_ssd_forced_zeros,
				&lambda_y_for_quant, &mode_type_for_re, &valid_flag_for_up, &dist_pred_for_up, &cost_best_temp_for_up,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				orgY_for_ssd, pred_for_rec);

			transform_hor_64(w, h, bit_depth, diff, coef_h);
			transform_ver_64(w, h, bit_depth, coef_h, coef);

			quant_nnz_64(cu_width_log2, cu_height_log2, 0, md_fw_ptr->slice_type, bit_depth, md_input_ptr->qp_y,
				lambda_y_for_quant, is_intra_for_quant,
				model_cnt, rdoq_array,
				coef,
				&lambda_y_for_re, &is_intra_for_re, &num_nz_for_re, &num_nz_for_itv,
				qcoef_for_dq, qcoef_for_re, qcoef_for_update);
			/*for (int i = 0; i < 4096; i++)
			{
				fprintf(file, "qcoef_for_re[%d]=%d\n", i, qcoef_for_re[i]);
			}*/
			rate_est_64(w, h, tree_status, curr_cons_pred_mode,
				lambda_y_for_re, num_nz_for_re, mode_type_for_re, curr_mvr_for_re, is_intra_for_re,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				qcoef_for_re,
				&num_nz_for_up, &mode_type_for_up, &curr_mvr_for_up, &is_intra_for_up,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost);

			inv_transform_ver_64(w, h, bit_depth, md_input_ptr->qp_y, num_nz_for_itv, qcoef_for_dq, &num_nz_for_ith, itcoef_v);
			//fprintf(file, "cost_best_temp=%d cost_best=%d dist=%d\n", cost_best_temp_for_up, *cost_best, dist);
			inv_transform_hor_64(w, h, bit_depth, mod_info_curr_ptr->tb_part, cu_width_log2, cu_height_log2, 0,
				num_nz_for_ith,
				orgY_for_ssd, pred_for_rec, itcoef_v,
				&dist,
				pred_for_update, rec_for_update);
			/*printf("dist=%d\n", dist);
			fprintf(file, "cost_best_temp=%d\n", cost_best_temp_for_up);
			fprintf(file, "cost_best=%d\n", *cost_best);
			fprintf(file, "dist_pred=%d\n", dist_pred_for_up);
			fprintf(file, "dist=%d\n", dist);
			printf("dist_pred=%d\n", dist_pred_for_up);
			fprintf(file, "flag_cost[0]=%d\n", flag_cost[0]);
			fprintf(file, "flag_cost[1]=%d\n", flag_cost[1]);*/
			update_rdo_luma_64(w, h,
				valid_flag_for_up, dist_pred_for_up, dist, cost_best_temp_for_up, num_nz_for_up, mode_type_for_up, is_intra_for_up, curr_mvr_for_up,
				cu_data_temp_ptr, mod_info_curr_ptr, best_info_ptr,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mv_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost, pred_for_update, rec_for_update, qcoef_for_update,
				cost_best);
		}
		else
		{
			intra_inter_pred_64(w, h, tree_status, cu_width_log2, cu_height_log2, avail_tb, rdo_idx,
				md_fw_ptr, skip_rdo, merge_rdo, skip_rdo_mode, mod_info_curr_ptr,
				rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY, RefWindowLCU, CtrPos,
				&curr_mvr_for_re, &mode_type_for_ssd_fz, &inter_mode_idx_for_ssd_fz, &valid_flag_for_ssd_fz, &is_intra_for_re,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_mv_for_up, mod_info_affine_mvd_for_re_fz, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, diff, pred_for_ssd_forced_zeros, md_fw_ptr->bit_depth_internal, refwin_md, CtrPosFME);

			ssd_bitcnt_64(tree_status, curr_cons_pred_mode,
				md_input_ptr->lambda_y, mode_type_for_ssd_fz, inter_mode_idx_for_ssd_fz, valid_flag_for_ssd_fz,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_affine_mvd_for_re_fz, mod_info_curr_ptr->mpm, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, pred_for_ssd_forced_zeros,
				&lambda_y_for_re, &mode_type_for_re, &valid_flag_for_up, &dist_pred_for_up, &cost_best_temp_for_up,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				orgY_for_ssd, pred_for_rec);

			num_nz_for_re = 0;
			for (int i = 0; i < 4096; ++i)
			{
				qcoef_for_update[i] = 0;
			}

			rate_est_64(w, h, tree_status, curr_cons_pred_mode,
				lambda_y_for_re, num_nz_for_re, mode_type_for_re, curr_mvr_for_re, is_intra_for_re,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				qcoef_for_update,
				&num_nz_for_up, &mode_type_for_up, &curr_mvr_for_up, &is_intra_for_up,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost);

			dist = dist_pred_for_up;
			if (valid_flag_for_ssd_fz == 1)
			{
				if (mode_type_for_ssd_fz == 1 || mode_type_for_ssd_fz == 0)
				{
					dist_pred_for_up = MAX_S64;
				}
			}

			update_rdo_luma_64(w, h,
				valid_flag_for_up, dist_pred_for_up, dist, cost_best_temp_for_up, num_nz_for_up, mode_type_for_up, is_intra_for_up, curr_mvr_for_up,
				cu_data_temp_ptr, mod_info_curr_ptr, best_info_ptr,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mv_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost, pred_for_ssd_forced_zeros, pred_for_rec, qcoef_for_update,
				cost_best);
		}
#endif
	}
	/*fprintf(file, "count=%d cost=%d\n", count, *cost_best);
	fclose(file);*/
	}
#endif //!LUMA_64_PIPELINE_DUMMY

	void ipred_ang_eipm_64_pred(pel src_le[131], pel src_up[131], int w, int h, int ipm
#if MIPF
		, int is_luma, int mipf_enable_flag, U4 bit_depth, pel pred_for_diff[64][64]
#endif
	)
	{
		pel tmp, tmp1;
		static const int com_tbl_ipred_dxdy[IPD_CNT][2] = /* {dx/dy, dy/dx} */
		{
			{0, 0},{0, 0},{0, 0},                                 //0~2
			{2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //3~6
			{744, 1408},{512, 2048},                              //7~8
			{372, 2816},                                          //9
			{256, 4096},                                          //10
			{128, 8192},{0, 0},{128, 8192},{256, 4096},           //11~14
			{372, 2816},{512, 2048},{744, 1408},{1024, 1024},     //15~18
			{1408, 744},{2048, 512},{2816, 372},                  //19~21
			{4096, 256},                                          //22
			{8192, 128},{0, 0},{8192, 128},{4096, 256},           //23~26
			{2816, 372},{2048, 512},{1408, 744},{1024, 1024},     //27~30
			{744, 1408},{ 512, 2048 },{0, 0},{2340, 448},         //31~34
			{1640, 640},                                          //35
			{1168, 898},{898, 1168},{640, 1640},{448, 2340},      //36~39
			{320, 3276},{192, 5460},{64, 16384},{32, 32768},      //40~43
			{32, 32768},{64, 16384},{192, 5460},{320, 3276},      //44~47
			{448, 2340},                                          //48
			{640, 1640},{898, 1168},{1168, 898},{1640, 640},      //49~52
			{2340, 448},{3276, 320},{5460, 192},{16384, 64},      //53~56
			{32768, 32},{32768, 32},{16384, 64},{5460, 192},      //57~60
			{3276, 320},{2340, 448},                              //61~62
			{1640, 640},{1168, 898},{898, 1168}                   //63~65
		};
#if MIPF

		static const s16 com_tbl_ipred_adi_bilinear[32][4] =
		{
			/* AVS-2 design == bilinear interpolation + {1,2,1} ref smoothing */
			{ 32, 64, 32,  0 },
			{ 31, 63, 33,  1 },
			{ 30, 62, 34,  2 },
			{ 29, 61, 35,  3 },
			{ 28, 60, 36,  4 },
			{ 27, 59, 37,  5 },
			{ 26, 58, 38,  6 },
			{ 25, 57, 39,  7 },
			{ 24, 56, 40,  8 },
			{ 23, 55, 41,  9 },
			{ 22, 54, 42, 10 },
			{ 21, 53, 43, 11 },
			{ 20, 52, 44, 12 },
			{ 19, 51, 45, 13 },
			{ 18, 50, 46, 14 },
			{ 17, 49, 47, 15 },
			{ 16, 48, 48, 16 },
			{ 15, 47, 49, 17 },
			{ 14, 46, 50, 18 },
			{ 13, 45, 51, 19 },
			{ 12, 44, 52, 20 },
			{ 11, 43, 53, 21 },
			{ 10, 42, 54, 22 },
			{ 9, 41, 55, 23 },
			{ 8, 40, 56, 24 },
			{ 7, 39, 57, 25 },
			{ 6, 38, 58, 26 },
			{ 5, 37, 59, 27 },
			{ 4, 36, 60, 28 },
			{ 3, 35, 61, 29 },
			{ 2, 34, 62, 30 },
			{ 1, 33, 63, 31 },
		};

		static const s16 com_tbl_ipred_adi_extra_smooth[32][4] =
		{
			/* best for CfP extra smooth */
			{ 23, 82, 21, 2 },
			{ 21, 82, 23, 2 },
			{ 19, 82, 25, 2 },
			{ 18, 81, 27, 2 },
			{ 16, 80, 30, 2 },
			{ 15, 79, 32, 2 },
			{ 13, 78, 35, 2 },
			{ 12, 77, 37, 2 },
			{ 11, 75, 40, 2 },
			{ 10, 74, 42, 2 },
			{ 9, 72, 45, 2 },
			{ 8, 70, 47, 3 },
			{ 7, 68, 50, 3 },
			{ 6, 67, 52, 3 },
			{ 6, 64, 54, 4 },
			{ 5, 62, 57, 4 },
			{ 4, 60, 60, 4 },
			{ 4, 57, 62, 5 },
			{ 4, 54, 64, 6 },
			{ 3, 52, 67, 6 },
			{ 3, 50, 68, 7 },
			{ 3, 47, 70, 8 },
			{ 2, 45, 72, 9 },
			{ 2, 42, 74, 10 },
			{ 2, 40, 75, 11 },
			{ 2, 37, 77, 12 },
			{ 2, 35, 78, 13 },
			{ 2, 32, 79, 15 },
			{ 2, 30, 80, 16 },
			{ 2, 27, 81, 18 },
			{ 2, 25, 82, 19 },
			{ 2, 23, 82, 21 },
		};

		static const s16 com_tbl_ipred_adi_semi_sharp[32][4] =
		{
			// semi-sharp
			{ 11, 106, 12, -1 },
			{ 9, 105, 15, -1 },
			{ 7, 105, 18, -2 },
			{ 6, 103, 21, -2 },
			{ 4, 102, 25, -3 },
			{ 2, 100, 29, -3 },
			{ 1, 99, 32, -4 },
			{ 0, 96, 36, -4 },
			{ -1, 94, 39, -4 },
			{ -2, 92, 43, -5 },
			{ -3, 89, 47, -5 },
			{ -4, 86, 51, -5 },
			{ -4, 83, 55, -6 },
			{ -5, 80, 59, -6 },
			{ -5, 77, 62, -6 },
			{ -6, 74, 66, -6 },
			{ -6, 70, 70, -6 },
			{ -6, 66, 74, -6 },
			{ -6, 62, 77, -5 },
			{ -6, 59, 80, -5 },
			{ -6, 55, 83, -4 },
			{ -5, 51, 86, -4 },
			{ -5, 47, 89, -3 },
			{ -5, 43, 92, -2 },
			{ -4, 39, 94, -1 },
			{ -4, 36, 96, 0 },
			{ -4, 32, 99, 1 },
			{ -3, 29, 100, 2 },
			{ -3, 25, 102, 4 },
			{ -2, 21, 103, 6 },
			{ -2, 18, 105, 7 },
			{ -1, 15, 105, 9 },
		};

		static const s16 tbl_mc_c_coeff_hp[32][4] =
		{
			{ 0, 64, 0, 0 },
			{ -1, 63,  2,  0 },
			{ -2, 62,  4,  0 },
			{ -2, 60,  7,  -1 },
			{ -2, 58, 10, -2 },
			{ -3, 57, 12, -2 },
			{ -4, 56, 14, -2 },
			{ -4, 55, 15, -2 },
			{ -4, 54, 16, -2 },
			{ -5, 53, 18, -2 },
			{ -6, 52, 20, -2 },
			{ -6, 49, 24, -3 },
			{ -6, 46, 28, -4 },
			{ -5, 44, 29, -4 },
			{ -4, 42, 30, -4 },
			{ -4, 39, 33, -4 },
			{ -4, 36, 36, -4 },
			{ -4, 33, 39, -4 },
			{ -4, 30, 42, -4 },
			{ -4, 29, 44, -5 },
			{ -4, 28, 46, -6 },
			{ -3, 24, 49, -6 },
			{ -2, 20, 52, -6 },
			{ -2, 18, 53, -5 },
			{ -2, 16, 54, -4 },
			{ -2, 15, 55, -4 },
			{ -2, 14, 56, -4 },
			{ -2, 12, 57, -3 },
			{ -2, 10, 58, -2 },
			{ -1,  7, 60, -2 },
			{ 0,  4, 62, -2 },
			{ 0,  2, 63, -1 },
		};
#else
		static const s16 com_tbl_ipred_adi[32][4] = { { (32), (64), (32), 0 }, { (31), (63), (33), (1) }, { (30), (62), (34), (2) }, { (29), (61), (35), (3) }, { (28), (60), (36), (4) }, { (27), (59), (37), (5) }, { (26), (58), (38), (6) }, { (25), (57), (39), (7) }, { (24), (56), (40), (8) }, { (23), (55), (41), (9) }, { (22), (54), (42), (10) }, { (21), (53), (43), (11) }, { (20), (52), (44), (12) }, { (19), (51), (45), (13) }, { (18), (50), (46), (14) }, { (17), (49), (47), (15) }, { (16), (48), (48), (16) }, { (15), (47), (49), (17) }, { (14), (46), (50), (18) }, { (13), (45), (51), (19) }, { (12), (44), (52), (20) }, { (11), (43), (53), (21) }, { (10), (42), (54), (22) }, { (9), (41), (55), (23) }, { (8), (40), (56), (24) }, { (7), (39), (57), (25) }, { (6), (38), (58), (26) }, { (5), (37), (59), (27) }, { (4), (36), (60), (28) }, { (3), (35), (61), (29) }, { (2), (34), (62), (30) }, { (1), (33), (63), (31) } };
#endif

#if MIPF
		//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
		const int filter_bits_list[4] = { 7, 7, 6, 7 };
		const int filter_offset_list[4] = { 64, 64 ,32, 64 };
		const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
		const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
		int filter_idx;
#else
		const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
		const int filter_offset = ADI_4T_FILTER_OFFSET;
		const int filter_bits = ADI_4T_FILTER_BITS;
#endif

		const int* mt = com_tbl_ipred_dxdy[ipm];

		//const pel* src_ch = NULL;
		//const s16* filter;

		int offset_x[MAX_CU_SIZE], offset_y[MAX_CU_SIZE];
		int t_dx[MAX_CU_SIZE], t_dy[MAX_CU_SIZE];
		int i, j;
		int offset;
		int pos_max = w + h - 1;
		int p, pn, pn_n1, pn_p2;
		int dxy = 0;
		int use_x = 0;
		int src_p, src_pn, src_pn_n1, src_pn_p2;
		int x = 0, y = 0;

		for (i = 0; i < w; i++)
		{
			GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
		}
		for (j = 0; j < h; j++)
		{
			GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
		}

		for (j = 0; j < h; j++)
		{
			for (i = 0; i < w; i++)
			{
				if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
				{
					filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
					use_x = 1;
					y = j + t_dy[i];
					x = i + t_dx[j];
				}
				else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
				{
					filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
					use_x = 0;
					y = j + t_dy[i];
					x = i + t_dx[j];
				}
				else
				{
					dxy = 1;
					y = j - t_dy[i];
					x = i - t_dx[j];
					use_x = (y <= -1) ? 1 : 0;
					if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT))
						filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
					else
						filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
				}

				if (use_x == 1)
				{
					offset = offset_x[j];
					pos_max = w * 2 - 1;
					if (dxy == 1)
					{
						pn_n1 = x + 1;
						p = x;
						pn = x - 1;
						pn_p2 = x - 2;
					}
					else
					{
						pn_n1 = x - 1;
						p = x;
						pn = x + 1;
						pn_p2 = x + 2;
					}
				}
				else
				{
					offset = offset_y[i];
					pos_max = h * 2 - 1;

					if (dxy == 1)
					{
						pn_n1 = y + 1;
						p = y;
						pn = y - 1;
						pn_p2 = y - 2;
				}
					else
					{
						pn_n1 = y - 1;
						p = y;
						pn = y + 1;
						pn_p2 = y + 2;
					}
		}

				pn_n1 = COM_MIN(pn_n1, pos_max);
				p = COM_MIN(p, pos_max);
				pn = COM_MIN(pn, pos_max);
				pn_p2 = COM_MIN(pn_p2, pos_max);

				if (use_x == 1)
				{
					src_pn_n1 = src_up[pn_n1];
					src_p = src_up[p];
					src_pn = src_up[pn];
					src_pn_p2 = src_up[pn_p2];
				}
				else
				{
					src_pn_n1 = src_le[pn_n1];
					src_p = src_le[p];
					src_pn = src_le[pn];
					src_pn_p2 = src_le[pn_p2];
				}

				if (filter_idx == 0)
					tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 1)
					tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 2)
					tmp = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
						src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);
				else if (filter_idx == 3)
					tmp = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
						src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);

				tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
				pred_for_diff[j][i] = tmp1;
	}
	}
}


void skip64_pred(U7 w, U7 h, s16 pred_cache[64][64],
	U1 valid_falg0,
	U1 valid_falg1,
	U1 valid_falg0_return,
	pel pred_y0_tmp[64][64],
	pel pred_y1_tmp[64][64],
	pel pred_cache_tmp[64][64]
)
{
	if ((valid_falg0_return == 1) && (valid_falg0 == 1)) {
		for (int j = 0; j < 64; j++)
		{
#pragma HLS PIPELINE
			for (int i = 0; i < 64; i++)
			{
#pragma HLS UNROLL
				if ((j < h) && (i < w)) {
					pred_cache[j][i] = pred_y0_tmp[j][i];
					pred_cache_tmp[j][i] = pred_y0_tmp[j][i];
				}
			}
		}
	}
	else if ((valid_falg0 == 1) && (valid_falg1 == 1)) {
		for (int j = 0; j < 64; j++)
		{
#pragma HLS PIPELINE
			for (int i = 0; i < 64; i++)
			{
#pragma HLS UNROLL
				if ((j < h) && (i < w)) {
					pred_cache[j][i] = (pred_y0_tmp[j][i] + pred_y1_tmp[j][i] + 1) >> 1;
					pred_cache_tmp[j][i] = (pred_y0_tmp[j][i] + pred_y1_tmp[j][i] + 1) >> 1;
				}
			}
		}
	}
	else if ((valid_falg0 == 1) && (valid_falg1 == 0)) {
		for (int j = 0; j < 64; j++)
		{
#pragma HLS PIPELINE
			for (int i = 0; i < 64; i++)
			{
#pragma HLS UNROLL
				if ((j < h) && (i < w)) {
					pred_cache[j][i] = pred_y0_tmp[j][i];
					pred_cache_tmp[j][i] = pred_y0_tmp[j][i];
				}
			}
		}
	}
	else if ((valid_falg0 == 0) && (valid_falg1 == 1)) {
		for (int j = 0; j < 64; j++)
		{
#pragma HLS PIPELINE
			for (int i = 0; i < 64; i++)
			{
#pragma HLS UNROLL
				if ((j < h) && (i < w)) {
					pred_cache[j][i] = pred_y1_tmp[j][i];
					pred_cache_tmp[j][i] = pred_y1_tmp[j][i];
				}
			}
		}
	}
}



void get_pred_y_64_0(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred[64][64])
{
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_1 dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=2
//#pragma HLS ARRAY_PARTITION variable=refwin_mc complete dim=3
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=4

	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;

	//printf("gmvx_refp_int = %d,%d\n ",gmvx_refp_int, gmvy_refp_int);


	pel buffer_0[5 + 5][8][2][16];
	pel buffer_1[2][8][2][16];
	pel buffer_2[2][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_0 cyclic  factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=4
#pragma HLS ARRAY_PARTITION variable=buffer_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=buffer_2 complete dim=0



	U6 gmvxdiv32 = gmvx_refp_int & 31;
	int i, j, k, m;
	int m_tmp;

	for (m = 0; m < 10; m++)
	{
#pragma HLS PIPELINE
		for (k = 0; k < 16; k++)
		{
#pragma HLS UNROLL
#define FLAT(i, j) buffer_0[m][i][j][k] = ref.ref_##i##_##j [(gmvx_refp_int / 32 + (m & 1)) + ((gmvy_refp_int / 8 + m / 2) * 5) + 80 * frac_idx + lidx * 1280][k];
			FLAT_8_2
#undef FLAT
		}
	}



	//FILE* fp_v2_buffer2 = fopen("skip_pred64x64_v2_buffer2.txt", "w+");

	for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		//10 mux 2
		int mux10to2 = (((gmvy_refp_int & 7) + i) / 8);
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int ix = 0; ix < 8; ix++) {
#pragma HLS UNROLL
				for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
					for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL
						buffer_1[mx][ix][jx][kx] = buffer_0[mux10to2 * 2 + mx][ix][jx][kx];
					}
				}
			}
		}

		int mux8to1 = (gmvy_refp_int + i) & 7;
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
				for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL

					buffer_2[mx][jx][kx] = buffer_1[mx][mux8to1][jx][kx];
				}
			}
		}


		for (int j = 0; j < 32; j++) {
#pragma HLS UNROLL
#if ENABLE_BFRAME
			pred[i][j] = buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
			//fprintf(fp_v2_buffer2, "pred[%d][%d]=%d,%d,%d,%d,buffer_2=%d\n", i, j, pred[i][j], (((gmvx_refp_int & 31) + j) >> 5), ((gmvx_refp_int + j) & 31), (gmvx_refp_int + j) & 15, buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15]);


#else
			pred[i][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}





	}
	//fclose(fp_v2_buffer2);
	return;
}

void get_pred_y_64_1(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred[64][64])
{
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_1 dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=2
//#pragma HLS ARRAY_PARTITION variable=refwin_mc complete dim=3
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=4

	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;

	//printf("gmvx_refp_int = %d,%d\n ",gmvx_refp_int, gmvy_refp_int);


	pel buffer_0[5 + 5][8][2][16];
	pel buffer_1[2][8][2][16];
	pel buffer_2[2][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_0 cyclic  factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=4
#pragma HLS ARRAY_PARTITION variable=buffer_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=buffer_2 complete dim=0



	U6 gmvxdiv32 = gmvx_refp_int & 31;
	int i, j, k, m;
	int m_tmp;

	for (m = 0; m < 10; m++) {
#pragma HLS PIPELINE
		for (k = 0; k < 16; k++) {
#pragma HLS UNROLL
#define FLAT(i, j) buffer_0[m][i][j][k] = ref.ref_##i##_##j [(gmvx_refp_int / 32 + (m & 1)) + ((gmvy_refp_int / 8 + m / 2) * 5) + 80 * frac_idx + lidx * 1280][k];
			FLAT_8_2
#undef FLAT
		}
	}


	//FILE* fp_v2_buffer21 = fopen("skip_pred64x64_v2_buffer21.txt", "w+");


	for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		//10 mux 2
		int mux10to2 = (((gmvy_refp_int & 7) + i) / 8);
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int ix = 0; ix < 8; ix++) {
#pragma HLS UNROLL
				for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
					for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL
						buffer_1[mx][ix][jx][kx] = buffer_0[mux10to2 * 2 + mx][ix][jx][kx];
					}
				}
			}
		}

		int mux8to1 = (gmvy_refp_int + i) & 7;
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
				for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL

					buffer_2[mx][jx][kx] = buffer_1[mx][mux8to1][jx][kx];
				}
			}
		}


		for (int j = 0; j < 32; j++) {
#pragma HLS UNROLL
#if ENABLE_BFRAME
			pred[i][j + 32] = buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
			//fprintf(fp_v2_buffer21, "pred[%d][%d]=%d,%d,%d,%d,buffer_2=%d\n", i, j, pred[i][j], (((gmvx_refp_int & 31) + j) >> 5), ((gmvx_refp_int + j) & 31), (gmvx_refp_int + j) & 15, buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15]);


#else
			pred[i][j + 32] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}



	}
	//fclose(fp_v2_buffer21);
	return;
}
void get_pred_y_64_2(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred[64][64])
{
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_1 dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=2
//#pragma HLS ARRAY_PARTITION variable=refwin_mc complete dim=3
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=4

	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;

	//printf("gmvx_refp_int = %d,%d\n ",gmvx_refp_int, gmvy_refp_int);


	pel buffer_0[5 + 5][8][2][16];
	pel buffer_1[2][8][2][16];
	pel buffer_2[2][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_0 cyclic  factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=4
#pragma HLS ARRAY_PARTITION variable=buffer_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=buffer_2 complete dim=0



	U6 gmvxdiv32 = gmvx_refp_int & 31;
	int i, j, k, m;
	int m_tmp;

	for (m = 0; m < 10; m++)
	{
#pragma HLS PIPELINE
		for (k = 0; k < 16; k++)
		{
#pragma HLS UNROLL
#define FLAT(i, j) buffer_0[m][i][j][k] = ref.ref_##i##_##j [(gmvx_refp_int / 32 + (m & 1)) + ((gmvy_refp_int / 8 + m / 2) * 5) + 80 * frac_idx + lidx * 1280][k];
			FLAT_8_2
#undef FLAT
		}
	}




	for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		//10 mux 2
		int mux10to2 = (((gmvy_refp_int & 7) + i) / 8);
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int ix = 0; ix < 8; ix++) {
#pragma HLS UNROLL
				for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
					for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL
						buffer_1[mx][ix][jx][kx] = buffer_0[mux10to2 * 2 + mx][ix][jx][kx];
					}
				}
			}
		}

		int mux8to1 = (gmvy_refp_int + i) & 7;
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
				for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL

					buffer_2[mx][jx][kx] = buffer_1[mx][mux8to1][jx][kx];
				}
			}
		}


		for (int j = 0; j < 32; j++) {
#pragma HLS UNROLL
#if ENABLE_BFRAME
			pred[i + 32][j] = buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];


#else
			pred[i + 32][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}



	}
	return;
}


void get_pred_y_64_3(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred[64][64])
{
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_1 dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=2
//#pragma HLS ARRAY_PARTITION variable=refwin_mc complete dim=3
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=4

	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;

	//printf("gmvx_refp_int = %d,%d\n ",gmvx_refp_int, gmvy_refp_int);


	pel buffer_0[5 + 5][8][2][16];
	pel buffer_1[2][8][2][16];
	pel buffer_2[2][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_0 cyclic  factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=4
#pragma HLS ARRAY_PARTITION variable=buffer_1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=buffer_2 complete dim=0



	U6 gmvxdiv32 = gmvx_refp_int & 31;
	int i, j, k, m;
	int m_tmp;

	for (m = 0; m < 10; m++)
	{
#pragma HLS PIPELINE
		for (k = 0; k < 16; k++)
		{
#pragma HLS UNROLL
#define FLAT(i, j) buffer_0[m][i][j][k] = ref.ref_##i##_##j [(gmvx_refp_int / 32 + (m & 1)) + ((gmvy_refp_int / 8 + m / 2) * 5) + 80 * frac_idx + lidx * 1280][k];
			FLAT_8_2
#undef FLAT
		}
	}




	for (int i = 0; i < 32; i++) {
#pragma HLS PIPELINE
		//10 mux 2
		int mux10to2 = (((gmvy_refp_int & 7) + i) / 8);
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int ix = 0; ix < 8; ix++) {
#pragma HLS UNROLL
				for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
					for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL
						buffer_1[mx][ix][jx][kx] = buffer_0[mux10to2 * 2 + mx][ix][jx][kx];
					}
				}
			}
		}

		int mux8to1 = (gmvy_refp_int + i) & 7;
		for (int mx = 0; mx < 2; mx++) {
#pragma HLS UNROLL
			for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
				for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL

					buffer_2[mx][jx][kx] = buffer_1[mx][mux8to1][jx][kx];
				}
			}
		}


		for (int j = 0; j < 32; j++) {
#pragma HLS UNROLL
#if ENABLE_BFRAME
			//pred[i][j] = buffer_0[((gmvy_refp_int & 7) + i) / 8 * 3 + (((gmvx_refp_int & 31) + j) >> 5)][(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
			//pred[i][j] = buffer_1[(((gmvx_refp_int & 31) + j) >> 5)][(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
			pred[i + 32][j + 32] = buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];


#else
			pred[i + 32][j + 32] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}



	}
	return;
}



void get_pred_y_64_wsc_v1(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred[64][64])
{
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_0_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_1_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_2_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_3_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_4_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_5_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_6_1 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_0 dim=2
#pragma HLS ARRAY_RESHAPE variable=ref->ref_7_1 dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;

	//printf("gmvx_refp_int = %d,%d\n ",gmvx_refp_int, gmvy_refp_int);
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=2
//#pragma HLS ARRAY_PARTITION variable=refwin_mc complete dim=3
//#pragma HLS ARRAY_RESHAPE variable=refwin_mc complete dim=4

	pel buffer_0[27][8][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_0 cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=buffer_0 complete dim=4

	pel buffer_1[3][8][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_1 complete dim=0
	pel buffer_2[3][2][16];
#pragma HLS ARRAY_PARTITION variable=buffer_2 complete dim=0

	U6 gmvxdiv32 = gmvx_refp_int & 31;
	int i, j, k, m;
	int m_tmp;

	//2560 mux 27
	for (m = 0; m < 27; m++)
	{
#pragma HLS PIPELINE
		for (k = 0; k < 16; k++)
		{
#pragma HLS UNROLL
#define FLAT(i, j) buffer_0[m][i][j][k] = ref.ref_##i##_##j [(gmvx_refp_int / 32 + (m % 3)) + ((gmvy_refp_int / 8 + m / 3) * 5) + 80 * frac_idx + lidx * 1280][k];
			FLAT_8_2
#undef FLAT
		}
	}

	for (int i = 0; i < 64; i++) {
#pragma HLS PIPELINE
		//27 mux 3
		int mux27to3 = (((gmvy_refp_int & 7) + i) / 8);
		for (int mx = 0; mx < 3; mx++) {
#pragma HLS UNROLL
			for (int ix = 0; ix < 8; ix++) {
#pragma HLS UNROLL
				for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
					for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL
						buffer_1[mx][ix][jx][kx] = buffer_0[mux27to3 * 3 + mx][ix][jx][kx];
					}
				}
			}
		}
		//3 mux 1
		int mux8to1 = (gmvy_refp_int + i) & 7;
		for (int mx = 0; mx < 3; mx++) {
#pragma HLS UNROLL
			for (int jx = 0; jx < 2; jx++) {
#pragma HLS UNROLL
				for (int kx = 0; kx < 16; kx++) {
#pragma HLS UNROLL

					buffer_2[mx][jx][kx] = buffer_1[mx][mux8to1][jx][kx];
				}
			}
		}

		//96 mux 64
		for (int j = 0; j < 64; j++) {
#pragma HLS UNROLL
#if ENABLE_BFRAME
			pred[i][j] = buffer_2[(((gmvx_refp_int & 31) + j) >> 5)][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];


#else
			pred[i][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + i) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif
		}
	}

	return;
}




BOOL com_mc_cu_y_64_64(U7 w_input, U7 h_input, MD_COM_MODE* mod_info_curr, U13 pic_w, U13 pic_h, S3 refi[REFP_NUM], s16 mv[REFP_NUM][2], int stride_luma,
	U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	Arbitor& ref
	, U2 mode_type, 
	U1* bool_flag,
	U1* valid_falg0,
	U1* valid_falg1,
	U1* valid_falg0_return,//
	pel pred_y0_tmp[64][64],
	pel pred_y1_tmp[64][64]
#if SUB_TMVP                 
	, CORE_SBTMVP* core_sbtmvp, int mode_idx, int umve_flag
#endif                      
)
{

#pragma HLS INLINE OFF
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv complete dim=0


#pragma HLS ARRAY_PARTITION variable=pred_y0_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y1_tmp complete dim=2


	* valid_falg0 = 0;
	*valid_falg1 = 0;
	*valid_falg0_return = 0;

	*bool_flag = 1;
	CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };

	int w = cu.cu_width;
	int h = cu.cu_height;
	int scup = cu.scup;
	int x = cu.x_pos;
	int y = cu.y_pos;
	S14 mv_t[2];
	int min_clip[2];
	int max_clip[2];
	int x_ext = cu.x_pos << 2;
	int y_ext = cu.y_pos << 2;
	int w_ext = cu.cu_width << 2;
	int h_ext = cu.cu_height << 2;
	int x_ext_of_ctu;
	int y_ext_of_ctu;
	int qpel_gmv_x_of_ctu;
	int qpel_gmv_y_of_ctu;
	int qpel_gmv_x_of_refp_cu;
	int qpel_gmv_y_of_refp_cu;

	//int bidx = 0;
	//U1 ref0_valid = 0, ref1_valid = 0;

	min_clip[0] = (-(1 << 7) - 4) << 2;
	min_clip[1] = (-(1 << 7) - 4) << 2;
	max_clip[0] = (pic_w - 1 + (1 << 7) + 4) << 2;
	max_clip[1] = (pic_h - 1 + (1 << 7) + 4) << 2;
	mv_t[0] = mv[REFP_0][0];
	mv_t[1] = mv[REFP_0][1];
	if (refi[REFP_0] >= 0)
	{
		if (x_ext + (mv[REFP_0][0]) < min_clip[0])
		{
			mv_t[0] = ((s16)(min_clip[0] - x_ext));
		}
		if (y_ext + (mv[REFP_0][1]) < min_clip[1])
		{
			mv_t[1] = ((s16)(min_clip[1] - y_ext));
		}
		if (x_ext + (mv[REFP_0][0]) + w_ext - 4 > max_clip[0])
		{
			mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
		}
		if (y_ext + (mv[REFP_0][1]) + h_ext - 4 > max_clip[1])
		{
			mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
		}
		/* forward */
		int qpel_gmv_x;
		int qpel_gmv_y;
		qpel_gmv_x = (x << 2) + ((int)mv_t[0]);
		qpel_gmv_y = (y << 2) + ((int)mv_t[1]);
		int dx = qpel_gmv_x & 0x3;
		int dy = qpel_gmv_y & 0x3;
		int x_offset_min = 0;
		int x_offset_max = 0;
		int y_offset_min = 0;
		int y_offset_max = 0;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

#if ENABLE_BFRAME
		S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));
#else
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2));
#endif // ENABLE_BFRAME


#if SUB_TMVP
		if (mode_idx == 0 && cu.cu_width >= SBTMVP_MIN_SIZE && cu.cu_height >= SBTMVP_MIN_SIZE && mode_type >= MODE_SKIP && umve_flag == 0)
		{
			if (!core_sbtmvp->isValid) {
				//return FALSE;
				*bool_flag = 0;

				return FALSE;
			}
#if DEBUG_CU
			if (isProblem)
			{
				printf("CU %d,%d,%d,%d\n", cu->x_pos, cu->y_pos, cu->cu_width, cu->cu_height);
				printf("ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}\n",
					CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1]);
			}
#endif
			int cu_width = cu.cu_width;
			int cu_height = cu.cu_height;
			int x = cu.x_pos;
			int y = cu.y_pos;
			int sub_w = cu_width / SBTMVP_NUM_1D;
			int sub_h = cu_height / SBTMVP_NUM_1D;
			int h = 0;
			int w = 0;
			int tmp_x = x;
			int tmp_y = y;
			//pel pred_tmp[64][64];
			for (int k = 0; k < SBTMVP_NUM; k++)
			{
#pragma HLS LOOP_TRIPCOUNT max=4
				if (core_sbtmvp->sbTmvp[k].ref_idx[0] >= 0) {
					w = (k % 2) * sub_w;
					h = (k / 2) * sub_h;
					x = tmp_x + w;
					y = tmp_y + h;

					x_ext = x << 2;
					y_ext = y << 2;
					w_ext = sub_w << 2;
					h_ext = sub_h << 2;

					int mv_sbtmvp[2];
					mv_sbtmvp[0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv_sbtmvp[1] = core_sbtmvp->sbTmvp[k].mv[0][1];
					x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
					y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);

					if (x_ext + (mv_sbtmvp[0]) < min_clip[0]) {
						mv_sbtmvp[0] = ((min_clip[0] - x_ext));
					}
					if (y_ext + (mv_sbtmvp[1]) < min_clip[1]) {
						mv_sbtmvp[1] = ((min_clip[1] - y_ext));
					}
					if (x_ext + (mv_sbtmvp[0]) + w_ext - 4 > max_clip[0]) {
						mv_sbtmvp[0] = ((max_clip[0] - x_ext - w_ext + 4));
					}
					if (y_ext + (mv_sbtmvp[1]) + h_ext - 4 > max_clip[1]) {
						mv_sbtmvp[1] = ((max_clip[1] - y_ext - h_ext + 4));
					}

					qpel_gmv_x = (x << 2) + ((int)mv_sbtmvp[0]);
					qpel_gmv_y = (y << 2) + ((int)mv_sbtmvp[1]);
					dx = qpel_gmv_x & 0x3;
					dy = qpel_gmv_y & 0x3;

					if (dx == 0 && dy == 0) {
						x_offset_min = 0;
						y_offset_min = 0;
					}
					else {
						x_offset_min = -3;
						y_offset_min = -3;
					}
#if ENABLE_BFRAME
					cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0);
					qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
					qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));
#else
					qpel_gmv_x_of_refp_cu = qpel_gmv_x - ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2));
					qpel_gmv_y_of_refp_cu = qpel_gmv_y - ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2));
#endif // ENABLE_BFRAME
					if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + sub_w + x_offset_max <= (160 + 4) &&
						(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + sub_h + y_offset_max <= (128 + 4)))
					{
#if MD_PRINT_ENH
						if (mode_type == MODE_SKIP)
							printf("<MODE_SKIP:SUB_TMVP> ");
						else if (mode_type == MODE_DIR)
							printf("<MODE_DIR:SUB_TMVP> ");
						printf("out of ref0 window ERROR: sub cu %d=(%d %d %d %d)", k, x, y, sub_w, sub_h);
						printf("mv=(");
						printf("%d %d", mv_sbtmvp[MV_X], mv_sbtmvp[MV_Y]);
						printf(")\n ");
#if ENABLE_BFRAME
						printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
							cur_refw_idx,
							CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
							CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
						);
						printf("\n");
#endif 
						fflush(stdout);
#endif
						core_sbtmvp->isValid = FALSE;
						
						*bool_flag = 0;
						return FALSE;
					}

//					get_pred_y_64(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, fme2mdrefbuf,
//#if ENABLE_BFRAME
//						cur_refw_idx,
//#endif // ENBALE_BFRAME
//						pred_tmp);

					if (k == 0)
					{
						get_pred_y_64_0(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
							cur_refw_idx,
#endif // ENBALE_BFRAME
							pred_y0_tmp);
					}
					else if (k == 1)
					{
						get_pred_y_64_1(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
							cur_refw_idx,
#endif // ENBALE_BFRAME
							pred_y0_tmp);
					}
					else if (k == 2)
					{
						get_pred_y_64_2(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
							cur_refw_idx,
#endif // ENBALE_BFRAME
							pred_y0_tmp);
					}
					else if (k == 3)
					{
						get_pred_y_64_3(sub_w, sub_h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
							cur_refw_idx,
#endif // ENBALE_BFRAME
							pred_y0_tmp);
					}
#if DEBUG_CU
					if (isProblem)
					{
						printf("CU %d (%d,%d) (%d,%d)\n", k, mv_sbtmvp[0], mv_sbtmvp[1], qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu);
					}
#endif
//					//copy pred_tmp to pred output
//					//int offset = h * 64 + w;
//					for (int i = 0; i < sub_h; i++)
//					{
//						for (int j = 0; j < sub_w; j++)
//						{
//							pred_y[h + i][w + j] = pred_y0_tmp[i][j];//[offset + i * 64 + j]  [i * 64 + j]
//#if DEBUG_CU
//							if (isProblem)
//							{
//								printf("%d,", pred_y0_tmp[i][j]);
//							}
//#endif
//						}
//#if DEBUG_CU
//						if (isProblem)
//						{
//							printf("\n");
//						}
//#endif
//					}

					/*FILE* sbtmvp = fopen("sbtmvp.csv", "w");
					for (int i = 0; i < 64; i++) {
						for (int j = 0; j < 64; j++) {
							pel curr = pred[i * 64 + j];
							fprintf(sbtmvp, "%d, ", (int)curr);
						}
						fprintf(sbtmvp, "\n");
					}
					fclose(sbtmvp);*/
				}
				else {
#if MD_PRINT
					printf("%s:%d:sbtmvp refi is invalid\n", __FUNCTION__, 3267);
#endif
				}
			}
		}
		else {
#endif
			if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + w + x_offset_max <= (160 + 4) &&
				(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + h + y_offset_max <= (128 + 4)))
			{
#if MD_PRINT
				if (mode_type == MODE_SKIP)
					printf("<MODE_SKIP> ");
				else if (mode_type == MODE_INTER)
					printf("<MODE_INTER> ");
				else if (mode_type == MODE_DIR)
					printf("<MODE_DIR> ");
				printf("out of ref0 window ERROR: cu=(%d %d %d %d)", x, y, w, h);
				printf("mv=(");
				printf("%d %d", mv_t[MV_X], mv_t[MV_Y]);
				printf(")\n ");
#if ENABLE_BFRAME
				printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
					cur_refw_idx,
					CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
					CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
				);
				printf("\n");
#endif 
				printf("\n");
				fflush(stdout);
#endif
				//return FALSE;
				* bool_flag = 0;
			}

//			get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, fme2mdrefbuf,
//#if ENABLE_BFRAME
//				cur_refw_idx,
//#endif // ENBALE_BFRAME
//				pred_y);

			get_pred_y_64_wsc_v1(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
				cur_refw_idx,
#endif // ENBALE_BFRAME
				pred_y0_tmp);
			//*valid_falg1 = 1;
#if SUB_TMVP
		}
#endif
		//++bidx;
		//ref0_valid = 1;
		if (*bool_flag == 0) {
			*valid_falg0 = 0;
		}
		else {
			*valid_falg0 = 1;
		}
	}
#if !ENABLE_BFRAME
	else {
#if MD_PRINT
		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3213);
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
			//return 1;
			*valid_falg0_return = 1;
		}
	}
  
	if (*valid_falg0_return == 1) {
	}
	else {
		mv_t[0] = mv[REFP_1][0];
		mv_t[1] = mv[REFP_1][1];
		//static pel pred_ref1[64][64];
		if (REFI_IS_VALID(refi[REFP_1]))
		{
			/* backward */
			//pel(*pred)[64] = bidx ? pred_snd : pred_y;
			//COM_PIC* ref_pic;
			//ref_pic = refp[refi[REFP_1]][REFP_1].pic;
			if (x_ext + (mv[REFP_1][0]) < min_clip[0])
			{
				mv_t[0] = ((s16)(min_clip[0] - x_ext));
			}
			if (y_ext + (mv[REFP_1][1]) < min_clip[1])
			{
				mv_t[1] = ((s16)(min_clip[1] - y_ext));
			}
			if (x_ext + (mv[REFP_1][0]) + w_ext - 4 > max_clip[0])
			{
				mv_t[0] = ((s16)(max_clip[0] - x_ext - w_ext + 4));
			}
			if (y_ext + (mv[REFP_1][1]) + h_ext - 4 > max_clip[1])
			{
				mv_t[1] = ((s16)(max_clip[1] - y_ext - h_ext + 4));
			}
			/* forward */
			int qpel_gmv_x;
			int qpel_gmv_y;
			qpel_gmv_x = (x << 2) + ((int)mv_t[0]);
			qpel_gmv_y = (y << 2) + ((int)mv_t[1]);
			int dx = qpel_gmv_x & 0x3;
			int dy = qpel_gmv_y & 0x3;
			int x_offset_min = 0;
			int x_offset_max = 0;
			int y_offset_min = 0;
			int y_offset_max = 0;

		if (dx == 0 && dy == 0) {
			x_offset_min = 0;
			y_offset_min = 0;
		}
		else {
			x_offset_min = -3;
			y_offset_min = -3;
		}
		x_offset_max = 4;
		y_offset_max = 4;

		S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1);
		qpel_gmv_x_of_refp_cu = qpel_gmv_x - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][0][0]) << 2) + (((int)CtrPosSKIP_ref1[0][0][1]) << 2)));
		qpel_gmv_y_of_refp_cu = qpel_gmv_y - (cur_refw_idx == 0 ? ((((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2)) : ((((int)CtrPosSKIP_ref1[0][1][0]) << 2) + (((int)CtrPosSKIP_ref1[0][1][1]) << 2)));

		//if (channel != CHANNEL_C)
		//{
		//if (!((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + 4 < SWH)) { return FALSE; }
		if (!((qpel_gmv_x_of_refp_cu >> 2) + x_offset_min >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + w + x_offset_max <= (160 + 4) &&
			(qpel_gmv_y_of_refp_cu >> 2) + y_offset_min >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + h + y_offset_max <= (128 + 4)))
		{
#if MD_PRINT
			if (mode_type == MODE_SKIP)
				printf("<MODE_SKIP> ");
			else if (mode_type == MODE_INTER)
				printf("<MODE_INTER> ");
			else if (mode_type == MODE_DIR)
				printf("<MODE_DIR> ");
			printf("out of ref1 window ERROR: cu=(%d %d %d %d)", x, y, w, h);
			printf("mv=(");
			printf("%d %d", mv_t[MV_X], mv_t[MV_Y]);
			printf(")\n");
			printf("cur_refw_idx is %d, ctrpos_skip_ref0={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}, ctrpos_skip_ref1={PIC_DIM:[(%d, %d),(%d, %d)], RW_DIM:[(%d, %d),(%d, %d)]}",
				cur_refw_idx,
				CtrPosSKIP[0][0][0], CtrPosSKIP[0][1][0], CtrPosSKIP[1][0][0], CtrPosSKIP[1][1][0], CtrPosSKIP[0][0][1], CtrPosSKIP[0][1][1], CtrPosSKIP[1][0][1], CtrPosSKIP[1][1][1],
				CtrPosSKIP_ref1[0][0][0], CtrPosSKIP_ref1[0][1][0], CtrPosSKIP_ref1[1][0][0], CtrPosSKIP_ref1[1][1][0], CtrPosSKIP_ref1[0][0][1], CtrPosSKIP_ref1[0][1][1], CtrPosSKIP_ref1[1][0][1], CtrPosSKIP_ref1[1][1][1]
			);
			printf("\n");
			fflush(stdout);
#endif
			* bool_flag = 0;
			//return FALSE;
		}

			//get_pred_y_64(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, fme2mdrefbuf, cur_refw_idx, pred_ref1);
			get_pred_y_64_wsc_v1(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref, cur_refw_idx, pred_y1_tmp);
			if (*bool_flag == 0) {
				*valid_falg1 = 0;
			}
			else {
				*valid_falg1 = 1;
			}
		}
	}
#endif
	return 0;
}

#if MD_KERNEL_64_MOD_YHY
void get_ref_intra_64_64(u8 rdo_list[5], u8 intra_mode_list[IPRED_LIST_NUM], pel src_le_temp[128 + 3], pel src_up_temp[128 + 3],
		pel pred_y_intra[5][64][64], U4 bit_depth_internal, int mipf_enable_flag, U8 w, U8 h, u16 avail_tb)
{
#pragma HLS ARRAY_PARTITION variable=rdo_list complete dim=0
#pragma HLS ARRAY_PARTITION variable=intra_mode_list complete dim=0

    //get intra_pred
	for (U8 rdo_idx = 0; rdo_idx < 5; rdo_idx++) 
	{
#pragma HLS LOOP_TRIPCOUNT max=5
		s8 inter_mode_idx = rdo_list[rdo_idx];
		U1 is_intra_write;
		s8 intra_mode_idx;

		if (inter_mode_idx >= 17)
		{
			is_intra_write = 1;
			intra_mode_idx = intra_mode_list[inter_mode_idx - 17];
			inter_mode_idx = -1;
		}
		else
		{
			is_intra_write = 0;
			intra_mode_idx = 0;
		}

		if (is_intra_write != 0) 
		{
			if (intra_mode_idx == 12)
				Ipred_vert_64_pred(w, h, src_up_temp + 3, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 24)
				Ipred_hor_64_pred(w, h, src_le_temp + 3, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 0)
				Ipred_dc_64_pred(w, h, src_le_temp + 3, src_up_temp + 3, bit_depth_internal, avail_tb, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 1)
				Ipred_plane_64_pred(w, h, src_le_temp + 2, src_up_temp + 2, bit_depth_internal, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 2)
				Ipred_bi_64_pred(w, h, src_le_temp + 3, src_up_temp + 3, bit_depth_internal, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx >= 3 && intra_mode_idx <= 11)
				ipred_ang1_64_pred(w, h, src_le_temp, src_up_temp, bit_depth_internal, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, mipf_enable_flag
#endif
				);
			else if (intra_mode_idx >= 25 && intra_mode_idx <= 32)
				ipred_ang2_64_pred(w, h, src_le_temp, src_up_temp, bit_depth_internal, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, mipf_enable_flag
#endif
				);
			else
			{
#if EIPM
				if (intra_mode_idx > 33)
				{
					ipred_ang_eipm_64_pred(src_le_temp + 3, src_up_temp + 3, w, h, intra_mode_idx
#if MIPF
						, 1, mipf_enable_flag, bit_depth_internal, pred_y_intra[rdo_idx]
#endif
					);

				}
				else
#endif
					ipred_ang3_64_pred(w, h, src_le_temp, src_up_temp, bit_depth_internal, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
						, 1, mipf_enable_flag
#endif
					);
			}
		}
	}
}

void get_ref_inter_64_64(u8 w, u8 h,MD_FW * md_fw_ptr, int curr_cons_pred_mode, SKIP_ELEMENT skip_mode[SKIP_MODE_NUM],  MD_COM_MODE * mod_info_curr,
		u8 rdo_list[5], MV_DIR * mv_dir, s16 mvp_all[2][5][2], U2 tree_status, S14 sub_pel_mv[MV_D], SKIP_RDO * skip_rdo,
		S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_skip_inter[13][64][64], strFetch_ref_window * Fetch_Ref_window_ptr,
		S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],  S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], Arbitor& ref, pel pred_y_inter2[2][64][64], 
		CORE_SBTMVP * core_sbtmvp,u8 intra_mode_list[IPRED_LIST_NUM],U13 pic_width, U13 pic_height,U1 * bool_flag_tmp)
{

#pragma HLS ARRAY_PARTITION variable=pred_y_inter2 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_inter2 complete dim=3
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter complete dim=3
#pragma HLS ARRAY_PARTITION variable=rdo_list complete dim=0
#pragma HLS ARRAY_PARTITION variable=intra_mode_list complete dim=0

#pragma HLS ARRAY_PARTITION variable=skip_mode complete dim=0
#pragma HLS ARRAY_PARTITION variable=skip_mode[11].refi complete dim=0
//#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP complete dim=0
//#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=mvd_copy complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi_copy complete dim=0

	//get skip pred
    if (!(md_fw_ptr->slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA))
	{
		for (int idx = 0; idx < SKIP_MODE_NUM + 1; ++idx)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=13
			if (idx < SKIP_MODE_NUM)
			{
				SKIP_ELEMENT* mode = skip_mode + idx;
				if (mode->valid == 0) 
				{ 
					continue; 
				}
#pragma HLS ARRAY_PARTITION variable=mode->mvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode->refi complete dim=0
//#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mv complete dim=0
//#pragma HLS ARRAY_PARTITION variable=mod_info_curr->refi complete dim=0
				mod_info_curr->cu_mode = MODE_SKIP;
				mod_info_curr->umve_flag = mode->umve_flag;
				mod_info_curr->mv[0][0] = mode->mvp[REFP_0][MV_X];
				mod_info_curr->mv[0][1] = mode->mvp[REFP_0][MV_Y];
				mod_info_curr->refi[0] = mode->refi[REFP_0];
#if ENABLE_BFRAME
				mod_info_curr->mv[1][0] = mode->mvp[REFP_1][MV_X];
				mod_info_curr->mv[1][1] = mode->mvp[REFP_1][MV_Y];
				mod_info_curr->refi[1] = mode->refi[REFP_1];
#else
				mod_info_curr->mv[1][0] = 0;
				mod_info_curr->mv[1][1] = 0;
				mod_info_curr->refi[1] = -1;
#endif
				// skip index 1 and 2 for P slice

#if ENABLE_BFRAME
				if (!REFI_IS_VALID(mod_info_curr->refi[REFP_0]) && !REFI_IS_VALID(mod_info_curr->refi[REFP_1])) { continue; }
#else
				if (mod_info_curr->refi[0] == -1) { continue; }
#endif
			}
			else 
			{
				mod_info_curr->cu_mode = MODE_INTER;
				s8 inter_mode_idx = rdo_list[0];
				static s16 mv_all[2][5][2];
				static int best_mv_uni_inner[2][4][2];

#pragma HLS ARRAY_PARTITION variable=mv_all complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_mv_uni_inner complete dim=0

				u8 mode_type = 0;
				u8 curr_mvr = 0;
				int refi_cur = 0;
#if ENABLE_BFRAME
				u8 num_refp_cnt = md_fw_ptr->num_refp[mv_dir->lidx];
#else
				u8 num_refp_cnt = md_fw_ptr->num_refp;

#endif
				if (inter_mode_idx < 17) {
					static U2 mode_type_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 };
					static u8 curr_mvr_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 1, 2 };
					mode_type = mode_type_table[inter_mode_idx];
					curr_mvr = curr_mvr_table[inter_mode_idx];
					if (mode_type == 1) {
						BOOL res = analyze_me_uni_pred_64(mod_info_curr, num_refp_cnt, curr_mvr, w, h,
							mvp_all, mv_all, best_mv_uni_inner,
							refi_cur, tree_status, sub_pel_mv,
							mod_info_curr->refi, mvd_copy, mod_info_curr->mv, md_fw_ptr->bit_depth_internal
#if ENABLE_BFRAME
							, mv_dir
#endif
						);
#if ENABLE_BFRAME
						U1 lidx = mv_dir->lidx;
#else 
						U1 lidx = 0;
#endif	
						mv_copy[lidx][0] = mod_info_curr->mv[lidx][0];
						mv_copy[lidx][1] = mod_info_curr->mv[lidx][1];
						refi_copy[0] = mod_info_curr->refi[0];
						refi_copy[1] = mod_info_curr->refi[1];
					}
				}
			}
			// calc pred info
			CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };
			if (w == 64 && h == 64) {
				U1 bool_flag;
				U1 valid_falg0;
				U1 valid_falg1;
				U1 valid_falg0_return;
				static pel pred_y0_tmp[64][64];
				static pel pred_y1_tmp[64][64];

#pragma HLS ARRAY_PARTITION variable=pred_y0_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y1_tmp complete dim=2

				com_mc_cu_y_64_64(w, h, mod_info_curr, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
					SWW, md_fw_ptr->bit_depth_internal, Fetch_Ref_window_ptr,
					CtrPos2MD,
#if ENABLE_BFRAME
					CtrPos2MD_ref1,
#endif
					ref,
					mod_info_curr->cu_mode, &bool_flag, &valid_falg0, &valid_falg1, &valid_falg0_return, pred_y0_tmp, pred_y1_tmp//fme2mdrefbuf
#if SUB_TMVP
					, core_sbtmvp, idx, mod_info_curr->umve_flag
#endif
				);
				*bool_flag_tmp = bool_flag;
				pel pred_cache_tmp[64][64];// = {};
				skip64_pred(w, h, pred_y_skip_inter[idx], valid_falg0, valid_falg1, valid_falg0_return, pred_y0_tmp, pred_y1_tmp, pred_cache_tmp);

			}
			else {
				BOOL res = com_mc_cu_y_64(w, h, &cu, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
					SWW, pred_y_skip_inter[idx], md_fw_ptr->bit_depth_internal, Fetch_Ref_window_ptr,
					CtrPos2MD,
#if ENABLE_BFRAME
					CtrPos2MD_ref1,
#endif
					ref
					, mod_info_curr->cu_mode
#if SUB_TMVP
					, core_sbtmvp, idx, mod_info_curr->umve_flag
#endif	
				);
			}
		}
	}

	//pred inter
	U8 mode_list[MAX_INTER_SKIP_RDO];

#pragma HLS ARRAY_PARTITION variable=mode_list complete dim=0
	for (int i = 0, j = 0; i < 12 && j < 4; ++i) 
	{
#pragma HLS PIPELINE
		if (skip_mode[i].valid && (skip_mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(skip_mode[i].refi[REFP_1])))
		{
			mode_list[j++] = i;
		}
	}

	for (U8 rdo_idx = 1; rdo_idx < 3; rdo_idx++) 
	{
#pragma HLS PIPELINE
		s8 inter_mode_idx = rdo_list[rdo_idx];
		U1 is_intra_write;
		s8 intra_mode_idx;

		if (inter_mode_idx >= 17)
		{
			is_intra_write = 1;
			intra_mode_idx = intra_mode_list[inter_mode_idx - 17];
			inter_mode_idx = -1;
		}
		else
		{
			is_intra_write = 0;
			intra_mode_idx = 0;
		}

		if (is_intra_write != 1) {
			if (inter_mode_idx < skip_rdo->num_rdo) {
				int skip_idx = inter_mode_idx;
				if (rdo_idx == 1 || rdo_idx == 2) {
					for (int j = 0; j < 64; j++) {
						for (int k = 0; k < 64; k++) {
							pred_y_inter2[rdo_idx - 1][j][k] = pred_y_skip_inter[mode_list[skip_idx]][j][k];
						}
					}
				}
			}
		}
	}
}

void get_ref_pel(U8 w, U8 h, MD_COM_MODE * mod_info_curr,
	SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], MERGE_RDO * merge,
	U13 pic_width, U13 pic_height, U2 slice_type, U2 tree_status, int curr_cons_pred_mode, U4 bit_depth,
	u16 avail_tb, MD_FW * md_fw_ptr, SKIP_RDO * skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	u8 rdo_list[MD_MAX_RDO_NUM], U1 valid_mode[17], u8 intra_mode_list[IPRED_LIST_NUM], pel src_le_temp[128 + 3], pel src_up_temp[128 + 3],
	S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	U1 * bool_flag_tmp,
	strFetch_ref_window * Fetch_Ref_window_ptr,
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], MV_DIR * mv_dir,
#endif
	Arbitor & ref
	, S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_skip_inter[13][64][64], pel pred_y_intra[5][64][64], pel pred_y_inter2[2][64][64]
#if SUB_TMVP
	, CORE_SBTMVP * core_sbtmvp
#endif
)
{
	static s16 mvd_for_ip[2][2];
	u8 rdo_list_intra[5];

#pragma HLS ARRAY_PARTITION variable=rdo_list_intra complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdo_list complete dim=0

	for(int i=0;i<5;i++)
	{
#pragma HLS UNROLL
		rdo_list_intra[i] = rdo_list[i];
	}

	u8 w_cp = w;
	u8 h_cp = h;

    get_ref_inter_64_64(w, h, md_fw_ptr, curr_cons_pred_mode, skip_mode, mod_info_curr,
		                rdo_list, mv_dir, mvp_all, tree_status, sub_pel_mv, skip_rdo,
		                mv_copy, mvd_copy, refi_copy, pred_y_skip_inter, Fetch_Ref_window_ptr,
		                CtrPos2MD, CtrPos2MD_ref1, ref, pred_y_inter2, 
		                core_sbtmvp, intra_mode_list, pic_width, pic_height, bool_flag_tmp);

	get_ref_intra_64_64(rdo_list_intra, intra_mode_list, src_le_temp, src_up_temp,
		                pred_y_intra, bit_depth, md_fw_ptr->mipf_enable_flag, w_cp, h_cp, avail_tb);
}
#else
void get_ref_pel(U8 w, U8 h, MD_COM_MODE * mod_info_curr,
	SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], MERGE_RDO * merge,
	U13 pic_width, U13 pic_height, U2 slice_type, U2 tree_status, int curr_cons_pred_mode, U4 bit_depth,
	u16 avail_tb, MD_FW* md_fw_ptr, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	u8 rdo_list[MD_MAX_RDO_NUM], U1 valid_mode[17], u8 intra_mode_list[IPRED_LIST_NUM], pel src_le_temp[128 + 3], pel src_up_temp[128 + 3],
	S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	U1* bool_flag_tmp,
	strFetch_ref_window * Fetch_Ref_window_ptr,
	S14 CtrPos2MD[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPos2MD_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], MV_DIR* mv_dir,
#endif
	Arbitor& ref
	, S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_skip_inter[13][64][64], pel pred_y_intra[5][64][64], pel pred_y_inter2[2][64][64]
#if SUB_TMVP
	, CORE_SBTMVP * core_sbtmvp
#endif
)
{
	s16 coef_tmp1[64][64];
	static s16 mvd_for_ip[2][2];

	if (slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA) { }
	else {
		for (int idx = 0; idx < SKIP_MODE_NUM+1; ++idx)
		{
			if (idx < SKIP_MODE_NUM)
			{
				SKIP_ELEMENT* mode = skip_mode + idx;
				if (mode->valid == 0) { continue; }

				mod_info_curr->cu_mode = MODE_SKIP;
				mod_info_curr->umve_flag = mode->umve_flag;
				mod_info_curr->mv[0][0] = mode->mvp[REFP_0][MV_X];
				mod_info_curr->mv[0][1] = mode->mvp[REFP_0][MV_Y];
				mod_info_curr->refi[0] = mode->refi[REFP_0];
#if ENABLE_BFRAME
				mod_info_curr->mv[1][0] = mode->mvp[REFP_1][MV_X];
				mod_info_curr->mv[1][1] = mode->mvp[REFP_1][MV_Y];
				mod_info_curr->refi[1] = mode->refi[REFP_1];
#else
				mod_info_curr->mv[1][0] = 0;
				mod_info_curr->mv[1][1] = 0;
				mod_info_curr->refi[1] = -1;
#endif
				// skip index 1 and 2 for P slice

#if ENABLE_BFRAME
				if (!REFI_IS_VALID(mod_info_curr->refi[REFP_0]) && !REFI_IS_VALID(mod_info_curr->refi[REFP_1])) { continue; }
#else
				if (mod_info_curr->refi[0] == -1) { continue; }
#endif
			}
			else {
				mod_info_curr->cu_mode = MODE_INTER;
				s8 inter_mode_idx = rdo_list[0];
				static s16 mv_all[2][5][2];
				static int best_mv_uni_inner[2][4][2];
				u8 mode_type = 0;
				u8 curr_mvr = 0;
				int refi_cur = 0;
#if ENABLE_BFRAME
				u8 num_refp_cnt = md_fw_ptr->num_refp[mv_dir->lidx];
#else
				u8 num_refp_cnt = md_fw_ptr->num_refp;

#endif
				if (inter_mode_idx < 17) {
					static U2 mode_type_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 };
					static u8 curr_mvr_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 1, 2 };
					mode_type = mode_type_table[inter_mode_idx];
					curr_mvr = curr_mvr_table[inter_mode_idx];
					if (mode_type == 1) {
						BOOL res = analyze_me_uni_pred_64(mod_info_curr, num_refp_cnt, curr_mvr, w, h,
							mvp_all, mv_all, best_mv_uni_inner,
							refi_cur, tree_status, sub_pel_mv,
							mod_info_curr->refi, mvd_copy, mod_info_curr->mv, bit_depth
#if ENABLE_BFRAME
							, mv_dir
#endif
						);
#if ENABLE_BFRAME
						U1 lidx = mv_dir->lidx;
#else 
						U1 lidx = 0;
#endif	
						mv_copy[lidx][0] = mod_info_curr->mv[lidx][0];
						mv_copy[lidx][1] = mod_info_curr->mv[lidx][1];
						refi_copy[0] = mod_info_curr->refi[0];
						refi_copy[1] = mod_info_curr->refi[1];
					}
				}
			}
			// calc pred info
			CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };
			if (w == 64 && h == 64) {
				U1 bool_flag;
				U1 valid_falg0;
				U1 valid_falg1;
				U1 valid_falg0_return;
				static pel pred_y0_tmp[64][64];
				static pel pred_y1_tmp[64][64];
#pragma HLS ARRAY_PARTITION variable=pred_y0_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y1_tmp complete dim=2
				com_mc_cu_y_64_64(w, h, mod_info_curr, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
					SWW, bit_depth, Fetch_Ref_window_ptr,
					CtrPos2MD,
#if ENABLE_BFRAME
					CtrPos2MD_ref1,
#endif
					ref,
					mod_info_curr->cu_mode, &bool_flag, &valid_falg0, &valid_falg1, &valid_falg0_return, pred_y0_tmp, pred_y1_tmp//fme2mdrefbuf
#if SUB_TMVP
					, core_sbtmvp, idx, mod_info_curr->umve_flag
#endif
				);
				*bool_flag_tmp = bool_flag;
				pel pred_cache_tmp[64][64];// = {};
				skip64_pred(w, h, pred_y_skip_inter[idx], valid_falg0, valid_falg1, valid_falg0_return, pred_y0_tmp, pred_y1_tmp, pred_cache_tmp);

			}
			else {
				BOOL res = com_mc_cu_y_64(w, h, &cu, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
					SWW, pred_y_skip_inter[idx], bit_depth, Fetch_Ref_window_ptr,
					CtrPos2MD,
#if ENABLE_BFRAME
				CtrPos2MD_ref1,
#endif
				ref
				, mod_info_curr->cu_mode
#if SUB_TMVP
				, core_sbtmvp, idx, mod_info_curr->umve_flag
#endif	
				);
			}
		}
	}

	//pred inter
	U8 mode_list[MAX_INTER_SKIP_RDO];
	for (int i = 0, j = 0; i < 12 && j < 4; ++i) {
		if (skip_mode[i].valid && (skip_mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(skip_mode[i].refi[REFP_1])))
		{
			mode_list[j++] = i;
		}
	}

	for (U8 rdo_idx = 1; rdo_idx < 3; rdo_idx++) {

		s8 inter_mode_idx = rdo_list[rdo_idx];
		U1 is_intra_write;
		s8 intra_mode_idx;

		if (inter_mode_idx >= 17)
		{
			is_intra_write = 1;
			intra_mode_idx = intra_mode_list[inter_mode_idx - 17];
			inter_mode_idx = -1;
		}
		else
		{
			is_intra_write = 0;
			intra_mode_idx = 0;
		}

		if (is_intra_write != 1) {
			if (inter_mode_idx < skip_rdo->num_rdo) {
				int skip_idx = inter_mode_idx;
				if (rdo_idx == 1) {
					for (int j = 0; j < 64; j++) {
						for (int k = 0; k < 64; k++) {
							pred_y_inter2[rdo_idx - 1][j][k] = pred_y_skip_inter[mode_list[skip_idx]][j][k];
						}
					}
					continue;
				}
				if (rdo_idx == 2) {
					for (int j = 0; j < 64; j++) {
						for (int k = 0; k < 64; k++) {
							pred_y_inter2[rdo_idx - 1][j][k] = pred_y_skip_inter[mode_list[skip_idx]][j][k];
						}
					}
					continue;
				}
			}
		}
	}


	//get intra_pred
	for (U8 rdo_idx = 0; rdo_idx < 5; rdo_idx++) {

		s8 inter_mode_idx = rdo_list[rdo_idx];
		U1 is_intra_write;
		s8 intra_mode_idx;

		if (inter_mode_idx >= 17)
		{
			is_intra_write = 1;
			intra_mode_idx = intra_mode_list[inter_mode_idx - 17];
			inter_mode_idx = -1;
		}
		else
		{
			is_intra_write = 0;
			intra_mode_idx = 0;
		}

		if (is_intra_write !=0) {
			if (intra_mode_idx == 12)
				Ipred_vert_64_pred(w, h, src_up_temp + 3, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 24)
				Ipred_hor_64_pred(w, h, src_le_temp + 3, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 0)
				Ipred_dc_64_pred(w, h, src_le_temp + 3, src_up_temp + 3, bit_depth, avail_tb, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 1)
				Ipred_plane_64_pred(w, h, src_le_temp + 2, src_up_temp + 2, bit_depth, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 2)
				Ipred_bi_64_pred(w, h, src_le_temp + 3, src_up_temp + 3, bit_depth, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx >= 3 && intra_mode_idx <= 11)
				ipred_ang1_64_pred(w, h, src_le_temp, src_up_temp, bit_depth, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, md_fw_ptr->mipf_enable_flag
#endif
				);
			else if (intra_mode_idx >= 25 && intra_mode_idx <= 32)
				ipred_ang2_64_pred(w, h, src_le_temp, src_up_temp, bit_depth, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, md_fw_ptr->mipf_enable_flag
#endif
				);
			else
			{
#if EIPM
				if (intra_mode_idx > 33)
				{
					ipred_ang_eipm_64_pred(src_le_temp + 3, src_up_temp + 3, w, h, intra_mode_idx
#if MIPF
						, 1, md_fw_ptr->mipf_enable_flag, bit_depth, pred_y_intra[rdo_idx]
#endif
					);

				}
				else
#endif
					ipred_ang3_64_pred(w, h, src_le_temp, src_up_temp, bit_depth, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
						, 1, md_fw_ptr->mipf_enable_flag
#endif
					);
			}
		}
	}
}
#endif


s64 rdo_luma_64(U1 isNS_allow, U8 w, U8 h, RDOQ_ARRAY * rdoq_array, MD_FW * md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<64>*me_line_map_ptr_64,
	ENC_ME_NEB_INFO * up_nebs, ENC_ME_NEB_INFO * left_nebs,
	SKIP_MERGE_INPUT * skip_in,
	U2 tree_status, MD_COM_MODE_BEST * best_info_ptr, MD_INPUT * md_input_ptr, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH ][ MAX_CU_SIZE_FETCH],
#if ENABLE_BFRAME
	ME_MV_DIR * me_mv_dir,
#endif
	MD_COM_MODE * mod_info_curr_ptr,
	ENC_CU_DATA_ARRAY_SIZE<64>*cu_data_temp_ptr,
	U2 curr_cons_pred_mode, U9 cup, U3 qt_depth,
	ENC_FME_INFO_ALL * fme_mv_info,
	u8 rmd_mode[IPRED_LIST_NUM],
	IP_ARRAY_SIZE<64, 64>*IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 64>*IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>*IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>*IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>*IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 64>*IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>*IP_buffer_ptr_64x8,
	MPM_ARRAY_SIZE<64, 64>*MPM_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<32, 64>*MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>*MPM_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<16, 64>*MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>*MPM_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<8, 64>*MPM_buffer_ptr_8x64, MPM_ARRAY_SIZE<64, 8>*MPM_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>*IP_map_scu_ptr_64x64,
	IP_MAP_SCU_SIZE<32, 64>*IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>*IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>*IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>*IP_map_scu_ptr_64x16,
	IP_MAP_SCU_SIZE<8, 64>*IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>*IP_map_scu_ptr_64x8,
	Arbitor& ref,
	strFetch_ref_window * Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP * core_sbtmvp
#endif
)
{
	if (!isNS_allow)
		return 0;

	U4 bit_depth = md_fw_ptr->bit_depth_internal;
	s64 cost_best = MAX_S64;
	U3 cu_width_log2 = mod_info_curr_ptr[0].cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr[0].cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	U1 amvr_enable_flag = 1;

	U14 sub_x = mod_info_curr_ptr->x_pos;
	U14 sub_y = mod_info_curr_ptr->y_pos;

	u8 curr_mvr = 0;

	static s16 coef_tmp_64x64[64][64];
	static s16 coef_tmp_h[64][64];
	static s16 coef_tmp_v[64][64];
	static pel rec_y_pb_part[4096];
	static pel pred_cache_64x64[64][64];
	static s16 coef_y_pb_part[4096];
	u8 mode_type = 0;
	//int i = 0;
	U1 valid_mode[17];   //{SKIP:0,1,2,3;Skip_Merge:4,5,6,7,8;Mode1:9,10,11,12,13;Mode2:14,15,16};    //{Mode 0:(skip mode:fix 4, skip_merger:0~5), Mode1:(ME:AMVR:FIX 1/5); Mode 2(AFF_ME::FIX 1/3)
	U1 valid_flag[1] = { 0 };
	//(u8 )ctx->rpm.num_refp[0];

	//pel* orgY = p_fenc_LCU_Y + (mod_info_curr_ptr->x_pos - md_input_ptr->pix_x) +
	//	(mod_info_curr_ptr->y_pos - md_input_ptr->pix_y) * 64;
	static pel orgY[64 * 64];
	for (int i = 0; i < cu_height; i++)
//#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < cu_width; j++)
//#pragma HLS LOOP_TRIPCOUNT max=64
			orgY[(i << 6) + j] = p_fenc_LCU_Y[(mod_info_curr_ptr->y_pos - md_input_ptr->pix_y + i)][ (mod_info_curr_ptr->x_pos - md_input_ptr->pix_x + j)];

#if 1 //DEBUG_CU
	if (mod_info_curr_ptr->x_pos == 32 && mod_info_curr_ptr->y_pos == 64 && cu_width == 8 && cu_height == 64 && pic_cnt==9)
		isProblem = 1;
#endif
	s64 lambda_y = md_input_ptr->lambda_y;
	SKIP_ELEMENT skip_mode[SKIP_MODE_NUM];

	u8 ipred_list[IPRED_LIST_NUM];

	static pel src_le_temp[128 + 3];
	static pel src_up_temp[128 + 3];
	//pel src_le_temp[128 + 3];
	//pel src_up_temp[128 + 3];
	u16 avail_tb;

	u8 rdo_list[MD_MAX_RDO_NUM] = { 0 }, inter_rdo_num[1] = { 0 }, intra_rdo_num[1] = { 0 };
	static SKIP_RDO skip_rdo;
	static SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO];
	S14 sub_pel_mv[MV_D];
	static MERGE_RDO merge_rdo;
	static s16 mvp_all[2][5][2];
#if ENABLE_BFRAME
	MV_DIR mv_dir;
#endif
	rdo_prepare_64(w, h, md_fw_ptr,
		me_line_map_ptr_64,
		up_nebs, left_nebs,
		skip_in, mod_info_curr_ptr,
		&merge_rdo, md_input_ptr, valid_mode, amvr_enable_flag, fme_mv_info,
		IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32, IP_buffer_ptr_16x64, IP_buffer_ptr_64x16, IP_buffer_ptr_8x64, IP_buffer_ptr_64x8,
		MPM_buffer_ptr_64x64, MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32, MPM_buffer_ptr_16x64, MPM_buffer_ptr_64x16, MPM_buffer_ptr_8x64, MPM_buffer_ptr_64x8,
		IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32, IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16, IP_map_scu_ptr_8x64, IP_map_scu_ptr_64x8,
		cu_width_log2, cu_height_log2, ipred_list, src_le_temp, src_up_temp, &avail_tb, rmd_mode,
		qt_depth, 17,
		cu_width, cu_height,
#if ENABLE_BFRAME
		me_mv_dir, &mv_dir,
#endif
		orgY, tree_status, curr_cons_pred_mode,
		inter_rdo_num, intra_rdo_num, rdo_list, skip_mode, &skip_rdo, skip_rdo_mode, sub_pel_mv, mvp_all, Fetch_Ref_window_ptr
#if SUB_TMVP
		, core_sbtmvp
#endif
	);



	u8 all_rdo_num = inter_rdo_num[0] + intra_rdo_num[0];

	static pel pred_y_skip_inter[13][64][64];
	static pel pred_y_intra[5][64][64];
	static pel pred_y_inter2[2][64][64];
	static S14 mv_copy[2][2];
	static s16 mvd_copy[2][2];
	static S3 refi_copy[2];
	U1 bool_flag;
	get_ref_pel(w, h, mod_info_curr_ptr,
		skip_mode, &merge_rdo, md_fw_ptr->pic_width, md_fw_ptr->pic_height, md_fw_ptr->slice_type, tree_status, curr_cons_pred_mode, bit_depth,
		avail_tb, md_fw_ptr, &skip_rdo, skip_rdo_mode,
		rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all,
		&bool_flag,
		Fetch_Ref_window_ptr,
		md_input_ptr->CtrPos2MD,
#if ENABLE_BFRAME
		md_input_ptr->CtrPos2MD_ref1, &mv_dir,
#endif
		ref, 
		mv_copy, mvd_copy, refi_copy, pred_y_skip_inter, pred_y_intra, pred_y_inter2
#if SUB_TMVP
		, core_sbtmvp
#endif
	);

#if !DISABLE_RDO_SKIP
	if (w == 64 && h == 64) {
	//if (0) {
		rdo_skip_64_64(w, h, &cost_best, best_info_ptr, cu_data_temp_ptr, mod_info_curr_ptr,
			orgY, skip_mode, &merge_rdo, md_fw_ptr->pic_width, md_fw_ptr->pic_height, md_fw_ptr->slice_type, tree_status, lambda_y, curr_cons_pred_mode, bit_depth,
			bool_flag,
			Fetch_Ref_window_ptr,
			md_input_ptr->CtrPos2MD,
#if ENABLE_BFRAME
			md_input_ptr->CtrPos2MD_ref1,
#endif
			//fme2mdrefbuf
			pred_y_skip_inter
#if SUB_TMVP
			, core_sbtmvp
#endif
		);
	}
	else {
		rdo_skip_64(w, h, &cost_best, best_info_ptr, cu_data_temp_ptr, mod_info_curr_ptr,
			orgY, skip_mode, &merge_rdo, md_fw_ptr->pic_width, md_fw_ptr->pic_height, md_fw_ptr->slice_type, tree_status, lambda_y, curr_cons_pred_mode, bit_depth,
			Fetch_Ref_window_ptr,
			md_input_ptr->CtrPos2MD,
#if ENABLE_BFRAME
			md_input_ptr->CtrPos2MD_ref1,
#endif
			//fme2mdrefbuf
			pred_y_skip_inter
#if SUB_TMVP
			, core_sbtmvp
#endif
		);
	}
#endif // DISABLE_RDO_SKIP	
	if (op_patch_mode_on)
		cost_best = MAX_S64;
#if USE_INTRA_REFRESH
	if (md_input_ptr->EncRefresh && md_fw_ptr->slice_type != SLICE_I)
	{
		if (md_input_ptr->RefreshMode == 0)
		{
			if (mod_info_curr_ptr->y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && mod_info_curr_ptr->y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
				cost_best = MAX_S64;
		}
		else
		{
			if (mod_info_curr_ptr->x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && mod_info_curr_ptr->x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
				cost_best = MAX_S64;
		}

	}
#endif
#if DEBUG_CU
	if (isProblem)
	{
		printf("AfterSkip: cu_mode=%d,umve_flag=%d,affine_flag=%d,skip_idx=%d\n", best_info_ptr->cu_mode, best_info_ptr->umve_flag, best_info_ptr->affine_flag, best_info_ptr->skip_idx);
	}
#endif
	rdo_luma_64_pipeline(all_rdo_num, w, h, tree_status, cu_width_log2, cu_height_log2, curr_cons_pred_mode, bit_depth, avail_tb,
		rdoq_array, md_fw_ptr, md_input_ptr, &skip_rdo, merge_rdo, skip_mode, skip_rdo_mode, mod_info_curr_ptr, best_info_ptr, cu_data_temp_ptr,
		rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY,
#if ENABLE_BFRAME
		& mv_dir, Fetch_Ref_window_ptr,
#endif
		&cost_best,
		//fme2mdrefbuf
		mv_copy, mvd_copy, refi_copy, pred_y_skip_inter[12], pred_y_inter2, pred_y_intra
#if SUB_TMVP
		, core_sbtmvp
#endif
	);
#if DEBUG_CU
	if (isProblem)
	{
		printf("AfterRDO: cu_mode=%d,umve_flag=%d,affine_flag=%d,skip_idx=%d\n", best_info_ptr->cu_mode, best_info_ptr->umve_flag, best_info_ptr->affine_flag, best_info_ptr->skip_idx);
	}
#endif
#if DEBUG_CU
	if (isProblem == 1)
		isProblem = 0;
#endif
	return cost_best;
}

void copy_to_cu_data_luma_64(U1 isNS_allow, U7 w, U7 h, U6 qp_y, ENC_CU_DATA_ARRAY_SIZE<64>*cu_data, MD_COM_MODE_BEST * mod_info_best, U2 tree_status,
	int slice_num, int affine_subblock_size_idx, U2 NS_luma_pred_mode[1]
#if SUB_TMVP
	, CORE_SBTMVP * core_sbtmvp
#endif
#if CUDQP_QP_MAP
	, int cu_dqp_enable, int cu_qp_group_pred_qp
#endif
)
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
	int pel_x = mod_info_best->x_scu * 4;
	int pel_y = mod_info_best->y_scu * 4;
	int pel_x_top = (mod_info_best->x_scu * 4 - (((mod_info_best->x_scu * 4) >> 6) << 6)) >> 2;
	int pel_y_top = (mod_info_best->y_scu * 4 - (((mod_info_best->y_scu * 4) >> 6) << 6)) >> 2;
	cu_width_log2 = mod_info_best->cu_width_log2;
	cu_height_log2 = mod_info_best->cu_height_log2;


	if (tree_status != TREE_C) {
		num_luma_tb = get_part_num((PART_SIZE)(mod_info_best->tb_part));
		for (i = 0; i < h; i++)
		{
			for (j = 0; j < w; j++)
			{
				num_coef_y += (((int)cu_data->coef_y[i][j]) != 0 ? 1 : 0);
			}
		}

		for (i = 0; i < num_luma_tb; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=4
			num_nnz_sum += mod_info_best->num_nz[i][0];
		}
	}
	if (tree_status == TREE_LC) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	else if (((int)tree_status) == ((int)TREE_L)) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	if (((int)tree_status) == ((int)TREE_C)) {
		idx = 0;
		for (j = 0; j < h >> 2; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
			for (i = 0; i < w >> 2; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
				if (mod_info_best->cu_mode == 0) {
					cu_data->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
				for (k = 1; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
				}
			}
			idx += w >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < h >> 2; j++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
			for (i = 0; i < w >> 2; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=16
				pb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->pb_part), i << 2, j << 2, w, h);
				tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, w, h);
				pb_idx_u = 0;
				tb_idx_u = 0;
				cu_data->pred_mode[idx + i] = ((u8)(mod_info_best->cu_mode));
				cu_data->umve_flag[idx + i] = mod_info_best->umve_flag;
				cu_data->umve_idx[idx + i] = mod_info_best->umve_idx;
				cu_data->pb_part[idx + i] = mod_info_best->pb_part;
				cu_data->tb_part[idx + i] = mod_info_best->tb_part;
#if CUDQP_QP_MAP
				cu_data->cu_luma_qp[idx + i] = cu_qp_group_pred_qp;
				//cu_data->luma_pred_qp[idx + i] = qp_y;
#endif
				for (k = 0; k < 3; k++) {
					cu_data->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
				}
				cu_data->map_scu[idx + i] = (cu_data->map_scu[idx + i] & 0xFF807F80) | ((unsigned int)(slice_num & 0x7F)) | (((
					unsigned int)qp_y << 16)) | ((unsigned int)((mod_info_best->cu_mode == 0) << 15)) | ((unsigned int)(1 << 31));
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
#if CUDQP_QP_MAP
				if (cu_dqp_enable && !cu_cbf_flag)
				{
					//change qp to pred_qp if no residual
					MCU_SET_QP(cu_data->map_scu[idx + i], cu_qp_group_pred_qp);
					//assert(ctx->cu_qp_group.pred_qp >= 0 && ctx->cu_qp_group.pred_qp <= MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
				}
#endif
				cu_data->map_cu_mode[idx + i] = (cu_data->map_cu_mode[idx + i] & 0xFFFFFFF0) | ((unsigned int)((pel_x_top & 0xF) << 0));
				cu_data->map_cu_mode[idx + i] = (cu_data->map_cu_mode[idx + i] & 0xFFFFFF0F) | ((unsigned int)((pel_y_top & 0xF) << 4));
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
					cu_data->mvd[idx + i][0][0] = ((s16)0);
					cu_data->mvd[idx + i][0][1] = ((s16)0);
					cu_data->mvd[idx + i][1][0] = ((s16)0);
					cu_data->mvd[idx + i][1][1] = ((s16)0);
					cu_data->refi[idx + i][0] = ((s8)(-1));
					cu_data->refi[idx + i][1] = ((s8)(-1));
					cu_data->skip_idx[idx + i] = 0;
					cu_data->smvd_flag[idx + i] = 0;
				}
				else {
					cu_data->mpm[0][idx + i] = 0;
					cu_data->mpm[1][idx + i] = 0;
					cu_data->ipm[0][idx + i] = 0;
					cu_data->ipm[1][idx + i] = 0;
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
#if SUB_TMVP
					if (core_sbtmvp != 0) {
						if (mod_info_best->umve_flag == 0 && mod_info_best->cu_mode >= MODE_SKIP && mod_info_best->skip_idx == 0 && mod_info_best->cu_width >= SBTMVP_MIN_SIZE && mod_info_best->cu_height >= SBTMVP_MIN_SIZE)
						{
							// sub block position
							int blk = ((i >= (mod_info_best->cu_width >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? 1 : 0) + ((j >= (mod_info_best->cu_height >> MIN_CU_LOG2) / SBTMVP_NUM_1D) ? SBTMVP_NUM_1D : 0);
							cu_data->refi[idx + i][0] = core_sbtmvp->best_sbTmvp[blk].ref_idx[0];
							cu_data->refi[idx + i][1] = core_sbtmvp->best_sbTmvp[blk].ref_idx[1];

							cu_data->mv[idx + i][0][0] = core_sbtmvp->best_sbTmvp[blk].mv[0][0];
							cu_data->mv[idx + i][0][1] = core_sbtmvp->best_sbTmvp[blk].mv[0][1];
							cu_data->mv[idx + i][1][0] = core_sbtmvp->best_sbTmvp[blk].mv[1][0];
							cu_data->mv[idx + i][1][1] = core_sbtmvp->best_sbTmvp[blk].mv[1][1];

							cu_data->mvd[idx + i][0][0] = 0;
							cu_data->mvd[idx + i][0][1] = 0;
							cu_data->mvd[idx + i][1][0] = 0;
							cu_data->mvd[idx + i][1][1] = 0;

						}
					}
#endif
				}
			}
			idx += w >> 2;
		}
		//if (mod_info_best->affine_flag) {
		//	enc_set_affine_mvf(affine_subblock_size_idx, cu_data, mod_info_best);
		//}
	}

	NS_luma_pred_mode[0] = cu_data->pred_mode[0];

}

int mode_cu_init_64(RDOQ_MODEL * rdoq_model, RDOQ_MODEL * model_cnt, RDOQ_ARRAY * rdoq_array, MD_COM_MODE * mod_info_curr_ptr, MD_COM_MODE_BEST * bst_info_ptr,
	/*COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info, COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,*/
	U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status,
	U2 * cons_pred_mode, ENC_CU_DATA_ARRAY_SIZE<64>*cu_data_temp_ptr)
{
	MD_COM_MODE* mod_info_curr = mod_info_curr_ptr;
	MD_COM_MODE_BEST* bst_info = bst_info_ptr;
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

	if (tree_status != TREE_C) {
		for (m = 0; m < cuw_scu * cuh_scu; m++) {
//#pragma HLS LOOP_TRIPCOUNT max=256
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

	if (tree_status == TREE_C) {
		int cu_w_scu = cu_width >> 2;
		int cu_h_scu = cu_height >> 2;
		// bottom-right
		int luma_pos_scu = cu_w_scu - 1 + (cu_h_scu - 1) * cu_w_scu;
		luma_pred_mode = cu_data_temp_ptr->pred_mode[luma_pos_scu];
		*cons_pred_mode = ((((luma_pred_mode) == 0 ? ONLY_INTRA : ONLY_INTER)));
		bst_info->cu_mode = mod_info_curr->cu_mode = luma_pred_mode;
		bst_info->ipm[0][0] = mod_info_curr->ipm[0][0] = cu_data_temp_ptr->ipm[0][luma_pos_scu];
		bst_info->ipm[0][1] = mod_info_curr->ipm[0][1] = 0;
		bst_info->affine_flag = mod_info_curr->affine_flag = cu_data_temp_ptr->affine_flag[luma_pos_scu];

		for (i = 0; i < 2; i++) {
			bst_info->refi[i] = mod_info_curr->refi[i] = cu_data_temp_ptr->refi[luma_pos_scu][i];
			bst_info->mv[i][0] = mod_info_curr->mv[i][0] = cu_data_temp_ptr->mv[luma_pos_scu][i][0];
			bst_info->mv[i][0] = mod_info_curr->mv[i][1] = cu_data_temp_ptr->mv[luma_pos_scu][i][1];
		}
	}
	return 0;
}

s64 init_basic_leaf_64(U1 isNS_allow, RDOQ_MODEL * rdoq_model, RDOQ_MODEL * model_cnt, RDOQ_ARRAY * rdoq_array, MD_FW * md_fw_ptr, s64 lambdaY, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, MD_COM_MODE_BEST * bst_info_ptr,
	U3 qt_depth, U3 bet_depth, U13 x0, U13 y0, U9 cup, U2 tree_status,
	U2 * cons_pred_mode, MD_COM_MODE * mod_info_curr_temp_ptr,/* COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info,
	COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,*/
	ENC_CU_DATA_ARRAY_SIZE<64>*cu_data_temp_ptr)
{
	if (!isNS_allow)
		return MAX_S64;

	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	int bit_cnt;
	s64 cost_temp = 0;
	int pos = 0 + ((cu_height >> 1 >> 2) * (cu_width >> 2) + (cu_width >> 1 >> 2));
	int shape = (SQUARE)+(com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	int bit_rate = 0;
	cu_data_temp_ptr->split_mode[cud][shape][pos] = NO_SPLIT;
	bit_rate += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr->split_mode, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
	bit_cnt = bit_rate;
	cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	mode_cu_init_64(rdoq_model, model_cnt, rdoq_array, mod_info_curr_temp_ptr, bst_info_ptr, x0, y0, cu_width_log2, cu_height_log2, cud, md_fw_ptr->pic_width_in_scu,
		tree_status, cons_pred_mode, cu_data_temp_ptr);
	return cost_temp;
}

#if !MD_KERNEL_64_DUMMY
s64 md_kernel_64(U7 w, U7 h, MD_FW * md_fw_ptr, MD_INPUT * md_input_ptr,
	u8 rmd_mode[IPRED_LIST_NUM],
#if ENABLE_BFRAME
	ME_MV_DIR * me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>*me_line_map_ptr_64,
	ENC_ME_NEB_INFO * up_nebs, ENC_ME_NEB_INFO * left_nebs,
	SKIP_MERGE_INPUT * skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status,
	ENC_CU_DATA_ARRAY_SIZE<64>*cu_data_temp_ptr,
	ENC_FME_INFO_ALL * fme_mv_info,
	IP_ARRAY_SIZE<64, 64>*IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 64>*IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>*IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>*IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>*IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 64>*IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>*IP_buffer_ptr_64x8,
	MPM_ARRAY_SIZE<64, 64>*MPM_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<32, 64>*MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>*MPM_buffer_ptr_64x32,
	MPM_ARRAY_SIZE<16, 64>*MPM_buffer_ptr_16x64, MPM_ARRAY_SIZE<64, 16>*MPM_buffer_ptr_64x16,
	MPM_ARRAY_SIZE<8, 64>*MPM_buffer_ptr_8x64, MPM_ARRAY_SIZE<64, 8>*MPM_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>*IP_map_scu_ptr_64x64,
	IP_MAP_SCU_SIZE<32, 64>*IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>*IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>*IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>*IP_map_scu_ptr_64x16,
	IP_MAP_SCU_SIZE<8, 64>*IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>*IP_map_scu_ptr_64x8,
	Arbitor& ref,
	strFetch_ref_window * Fetch_Ref_window_ptr)
{

#if DISABLE_CU_64
	return MAX_S64;
#endif

#if DEBUG_MD_64
	if (x0 == 0 && y0 == 0 && cu_width_log2 == 5 && cu_height_log2 == 6 && cud == 0 && pic_cnt == 9 && qt_depth == 0 && bet_depth == 1)
	{
		isProblem = 1;

	}
#endif
	s64 ns_cost = (s64)MAX_S64;
	static MD_COM_MODE mod_info_curr_temp;
	static MD_COM_MODE_BEST mod_info_best;
	s8 coef_rec_write_flag[1] = { 0 };
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
	mod_info_curr_temp.x_chroma = cu_data_temp_ptr->x_chroma;
	mod_info_curr_temp.y_chroma = cu_data_temp_ptr->y_chroma;
	mod_info_curr_temp.last_child = cu_data_temp_ptr->last_child;
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
	if (!boundary) {
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable) {
			u32 luma_map_scu;
			int scu_stride = 16;
			int x_scu_in_LCU = (x0 % 64) >> 2;
			int y_scu_in_LCU = (y0 % 64) >> 2;
			int luma_cup;
			U6 cu_w_scu = w >> 2;
			U6 cu_h_scu = h >> 2;
			luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));

			md_input_ptr->qp_y = md_input_ptr->CuQPMap[luma_cup];
			md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
		}
#endif
		ns_cost = init_basic_leaf_64(isNS_allow, &rdoq_model_t, &model_cnt_t, &rdoq_array, md_fw_ptr, md_input_ptr->lambda_y, cu_width_log2, cu_height_log2, cud, &mod_info_best, qt_depth, bet_depth, x0, y0, cup, tree_status, &cons_pred_mode,
			&mod_info_curr_temp, cu_data_temp_ptr);

		ns_cost += rdo_luma_64(isNS_allow, w, h, &rdoq_array, md_fw_ptr,
			me_line_map_ptr_64,
			up_nebs, left_nebs,
			skip_in, tree_status, &mod_info_best, md_input_ptr, p_fenc_LCU_Y,
#if ENABLE_BFRAME
			me_mv_dir,
#endif
			& mod_info_curr_temp, cu_data_temp_ptr, cons_pred_mode, cup, qt_depth, fme_mv_info,
			rmd_mode,
			IP_buffer_ptr_64x64, IP_buffer_ptr_32x64, IP_buffer_ptr_64x32, IP_buffer_ptr_16x64, IP_buffer_ptr_64x16, IP_buffer_ptr_8x64, IP_buffer_ptr_64x8,
			MPM_buffer_ptr_64x64, MPM_buffer_ptr_32x64, MPM_buffer_ptr_64x32, MPM_buffer_ptr_16x64, MPM_buffer_ptr_64x16, MPM_buffer_ptr_8x64, MPM_buffer_ptr_64x8,
			IP_map_scu_ptr_64x64, IP_map_scu_ptr_32x64, IP_map_scu_ptr_64x32, IP_map_scu_ptr_16x64, IP_map_scu_ptr_64x16, IP_map_scu_ptr_8x64, IP_map_scu_ptr_64x8,
			ref,
			Fetch_Ref_window_ptr
#if SUB_TMVP
			, core_sbtmvp_ptr
#endif
		);

		copy_to_cu_data_luma_64(isNS_allow, w, h, md_input_ptr->qp_y, cu_data_temp_ptr, &mod_info_best, tree_status, 0, 0, NS_luma_pred_mode
#if SUB_TMVP
			, core_sbtmvp_ptr
#endif
#if CUDQP_QP_MAP
			, md_input_ptr->cu_dqp_enable, md_input_ptr->cu_qp_group_pred_qp
#endif
		);
	}
	return ns_cost;
}
#endif //!MD_KERNEL_64_DUMMY
