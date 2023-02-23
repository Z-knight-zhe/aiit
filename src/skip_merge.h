void FUNC_NAME(fetch_ref_line, W, H)(pel hor_ref[SKIP_MC_OUT_WIDTH][8], const pel ref_y[SWH * SWW], const int r_off, const int c_off)
{
#pragma HLS INLINE OFF
	pel line_buf[SWW];
	pel buf[SKIP_MC_IN_WIDTH];
#pragma HLS array_partition variable=line_buf complete dim=0
#pragma HLS array_partition variable=buf complete dim=0
	int i, j;
	for (i = 0; i < SWW; ++i){
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 ? ref_y[r_off * SWW + i] : 0;
	}
	for (i = 0; i < SKIP_MC_IN_WIDTH; ++i){
#pragma HLS UNROLL
		buf[i] = c_off >= 0 ? line_buf[c_off + i] : 0;
	}
	for (i = 0; i < SKIP_MC_OUT_WIDTH; ++i){
#pragma HLS UNROLL
		for (j = 0; j < 8; ++j){
#pragma HLS UNROLL
			hor_ref[i][j] = buf[i + j];
		}
	}
}

void FUNC_NAME(fetch_org_line, W, H)(pel hor_org[W][SKIP_MC_VER_NUM], const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const int r_off, const int c_off)
{
#pragma HLS INLINE OFF
	pel line_buf[MAX_CU_SIZE_FETCH];
#pragma HLS array_partition variable=line_buf complete dim=0
	int i;
	for (i = 0; i < MAX_CU_SIZE_FETCH; ++i) {
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 ? org_y[r_off][i] : 0;
	}
	for (i = 0; i < W; ++i) {
#pragma HLS UNROLL
		shifting(hor_org[i], line_buf[c_off + i], SKIP_MC_VER_NUM);
	}
}

void FUNC_NAME(mc_sad_l_skip, W, H)(s32 sad[SKIP_MODE_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const int mv_idx, const int x_pos, const int y_pos, const int cu_width, const int cu_height, const int pic_w, const int pic_h, const int mc_pos[MV_D], const pel ref_y[SWH * SWW], const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const int x_chroma, const int y_chroma, const BOOL chroma_motion)
{
#pragma HLS INLINE OFF
	//#pragma HLS function_instantiate variable=cu_height
	// #pragma HLS array_partition variable=sad_result complete dim=0
	// #pragma HLS array_partition variable=mv complete dim=0
	// #pragma HLS array_partition variable=ref_y cyclic factor=200 dim=1
	// #pragma HLS array_partition variable=org_y complete dim=2

	int r, c, i, dx, dy;
	BOOL flag[SKIP_MODE_NUM];
	pel hor_ref[SKIP_MC_OUT_WIDTH][8];
	pel hor_buf[SKIP_MC_OUT_WIDTH][4][8];
	pel hor_org[W][SKIP_MC_VER_NUM];
#pragma HLS array_partition variable=flag complete dim=0
#pragma HLS array_partition variable=hor_ref complete dim=0
#pragma HLS array_partition variable=hor_buf complete dim=0
#pragma HLS array_partition variable=hor_org complete dim=0

	const int ref_top = mc_pos[MV_Y] - 3;
	const int ref_left = mc_pos[MV_X] - 3;
	const int org_top = y_pos & (MAX_CU_SIZE_FETCH - 1);
	const int org_left = x_pos & (MAX_CU_SIZE_FETCH - 1);

	for (i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
		flag[i] = REFI_IS_VALID(mode[i].refi) && mode[i].mv_idx == mv_idx && check_mv_in_refwin(ref_top + (mode[i].mvd[MV_Y] >> 2), ref_left + (mode[i].mvd[MV_X] >> 2), x_pos, y_pos, cu_width, cu_height, x_chroma, y_chroma, chroma_motion);
	}
	static int pflag = 1;
LOOP_MC_SAD:
	for (r = 0; r < SKIP_MC_IN_HEIGHT; ++r){
#pragma HLS PIPELINE II=1

		FUNC_NAME(fetch_ref_line, W, H)(hor_ref, ref_y, ref_top + r, ref_left);
		FUNC_NAME(fetch_org_line, W, H)(hor_org, org_y, org_top + r - 7, org_left);

		for (c = 0; c < (SKIP_MC_HOR_NUM + cu_width - 1); ++c){
#pragma HLS UNROLL
			pel pred[4][4];
#pragma HLS array_partition variable=pred complete dim=0

			for (dx = 0; dx < 4; ++dx){
#pragma HLS UNROLL
				pel hor_data = mc_8tag(dx, hor_ref[c]);
				shifting(hor_buf[c][dx], hor_data, 8);
				for (dy = 0; dy < 4; ++dy){
#pragma HLS UNROLL
					s32 col_buf = mc_8tag(dy, hor_buf[c][dx]);
					pel pt = (col_buf + 2048) >> 12;
					pred[dx][dy] = COM_CLIP3(0, (1 << 8) - 1, pt);
				}
			}

			for (i = 0; i < SKIP_MODE_NUM; ++i){
#pragma HLS UNROLL
				const int x = c - (mode[i].mvd[MV_X] >> 2);
				const int y = r - 7 - (mode[i].mvd[MV_Y] >> 2);
				sad[i] += (flag[i] && x >= 0 && x < cu_width && y >= 0 && y < cu_height) ?
					COM_ABS16(pred[mode[i].mvd[MV_X] & 3][mode[i].mvd[MV_Y] & 3] - hor_org[x][SKIP_MC_VER_NUM - (mode[i].mvd[MV_Y] >> 2) - 1]) :
					0;
				//if (pflag && i == 0 && flag[i] && x >= 0 && x < cu_width && y >= 0 && y < cu_height){
				//printf("%d %d %d\n", x, y, COM_ABS16(pred[mode[i].mvd[MV_X] & 3][mode[i].mvd[MV_Y] & 3] - hor_org[x][SKIP_MC_VER_NUM - (mode[i].mvd[MV_Y] >> 2) - 1]));
				//}
			}
		}
	}
	pflag = 0;
}

static void FUNC_NAME(make_skip_sad, W, H)(
	s32 mode_sad[SKIP_MODE_NUM],
	const SKIP_MODE_INTER mode[SKIP_MODE_NUM],
	const int x_pos,
	const int y_pos,
	const int cu_width,
	const int cu_height,
	const int x_chroma,
	const int y_chroma,
	const int chroma_motion,
	const int pic_w,
	const int pic_h,
	const pel ref_y[SWH * SWW],
	const pel org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	const int mc_pos[SKIP_MC_NUM][MV_D]
	) {
#pragma HLS INLINE OFF
	//#pragma HLS function_instantiate variable=cu_height
	int mv_idx;
	for (mv_idx = 0; mv_idx < SKIP_MC_NUM; ++mv_idx){
		FUNC_NAME(mc_sad_l_skip, W, H)(mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
	}
}

static void FUNC_NAME(make_cand_list, W, H)(MD_FW *md_fw_ptr, int *mode_list, MD_COM_MODE *mod_info_curr_ptr, SKIP_MERGE_INPUT * input, SKIP_ELEMENT mode[SKIP_MODE_NUM], strFetch_ref_window *refw, strFetch_Orig_Lcu *org)
{
	int valid[SKIP_MODE_NUM] = { 0 };
	int i;
	int cnt = 0;
	s64 cost_list[4 + 8];
	s64 bit_cnt_list[12];
	s64 bit_cnt, sad;
	int shift;
	s64 cost_y;

	SKIP_MODE_INTER mode_temp[SKIP_MODE_NUM];
	int mc_pos[SKIP_MC_NUM][MV_D];
	s32 mode_sad[SKIP_MODE_NUM];
	for (i = 0; i < SKIP_MODE_NUM; ++i){
		mode_temp[i].skip_flag = mode[i].skip_flag;
		mode_temp[i].affine_flag = mode[i].affine_flag;
		mode_temp[i].umve_flag = mode[i].umve_flag;
		mode_temp[i].skip_idx = mode[i].skip_idx;
		mode_temp[i].umve_idx = mode[i].umve_idx;
		mode_temp[i].mvp[MV_X] = mode[i].mvp[MV_X];
		mode_temp[i].mvp[MV_Y] = mode[i].mvp[MV_Y];
		mode_temp[i].refi = mode[i].refi;
		mode_temp[i].valid = mode[i].valid;
		mode_sad[i] = 0;
	}

	mc_center(mode_temp, mc_pos, mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, mod_info_curr_ptr->cu_width, mod_info_curr_ptr->cu_height, md_fw_ptr->pic_width, md_fw_ptr->pic_height);

	FUNC_NAME(make_skip_sad, W, H)(mode_sad, mode_temp, mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, mod_info_curr_ptr->cu_width, mod_info_curr_ptr->cu_height, refw->x_chroma, refw->y_chroma, mod_info_curr_ptr->chroma_motion, md_fw_ptr->pic_width, md_fw_ptr->pic_height, refw->RefWindowLCU, org->y_org, mc_pos);

	make_skip_cand(mode_list, mode_temp, mode_sad);
}
