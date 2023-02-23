#include "com_def.h"
#include <math.h>
#include <stdlib.h>
#include "md1.h"


int encode_umve_idx_est(int umve_idx)
{
	U3 enc_est;
	enc_est = 0;
	if (umve_idx<20)
		enc_est++;
	else
	{
		enc_est += 2;


	}
	if (umve_idx<4 || (umve_idx >= 20 && umve_idx<24))
		enc_est++;
	else
	{
		enc_est += 2;


	}

	enc_est += 2;
	return enc_est;
}

int encode_affine_flag_est(int flag, u8 affine_enable_flag, int  cu_width_log2, int cu_height_log2)
{
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 enc_est = 0;
	if (cu_width_log2 >= 4 && cu_height_log2 >= 4 && 1) {
		enc_est++;
	}
	return enc_est;
}

int sbac_write_truncate_unary_sym_est(U8 sym, U8 max_num)
{

#pragma HLS INLINE
	U8 enc_est;
	enc_est = (max_num - (1) > sym + (1) ? sym + (1) : max_num - (1));
	return enc_est;
}


static int enc_eco_abs_mvd_est(U16 sym)
{
	U5 enc_est = 0;
	U4 exp_golomb_order = 0;
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
		else //if (sym >= (1023) && sym < (2047)) 
		{
			exp_golomb_order = 10;
		}
		//else if (sym >= (2047) && sym < (4095)) {
		//	exp_golomb_order = 11;
		//}
		enc_est = enc_est + 2 * exp_golomb_order;
		//enc_est+=sbac_write_unary_sym_ep_est(exp_golomb_order);
		//enc_est+=sbac_encode_bins_ep_msb_est(exp_golomb_order);
	}
	return enc_est;
}


int encode_mvd_est(S16 mvd[2])
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



int is_cu_nz(int nz[4][3])
{
	U8 cu_nz = 0;
	cu_nz = nz[0][0] + nz[1][0] + nz[2][0] + nz[3][0] + nz[0][1] + nz[0][1] + nz[1][1] + nz[2][1] + nz[3][1] + nz[0][2] + nz[1][2] + nz[2][2] + nz[3][2];
	return cu_nz ? 1 : 0;
}

static int enc_eco_cbf_est_skip(RDOQ_MODEL model_cnt,int tb_avaliable, int pb_part_size, int tb_part_size, int num_nz[4][3], u8 pred_mode,
	S8 ipm[4][2], u8 tree_status, int cu_width_log2, int cu_height_log2)
{
	U3 enc_est = 0;
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
			model_cnt.cbf[1]++;
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			model_cnt.cbf[0]++;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);
			enc_est += 1;
			model_cnt.cbf[0]++;
				
		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
				model_cnt.cbf[1]++;
			}
		}
	}
	return enc_est;
}

static int enc_eco_cbf_est( int tb_avaliable, int pb_part_size, int tb_part_size, int num_nz[4][3], u8 pred_mode,
	s8 ipm[4][2], u8 tree_status, int cu_width_log2, int cu_height_log2)
{
	U3 enc_est = 0;
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 ctp_zero_flag = !is_cu_nz(num_nz);
	/* code allcbf */
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				if (cu_width_log2 > 6 || cu_height_log2 > 6) {
					enc_est++;
				}
				else {
					enc_est++;
				}
				if (ctp_zero_flag) {
					return 0;
				}
			}
		}
		if (tb_avaliable) {
			enc_est++;
		}
		
		if (tree_status == TREE_LC) {
			enc_est += 2;

		}
		if (num_nz[0][1] + num_nz[0][2] == 0 && tb_part_size == (SIZE_2Nx2N) && tree_status == TREE_LC) {
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


static int enc_eco_level_est_2(u16 sym)
{

#pragma HLS INLINE
	U4 exp_golomb_order = 0;
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
		//sbac_write_unary_sym_ep_est(exp_golomb_order);
		//sbac_encode_bins_ep_msb_est(exp_golomb_order);
	}
	return enc_est;
}
static int enc_eco_run_est_2(U10 sym, u16 level)
{

#pragma HLS INLINE
  U4 exp_golomb_order = 0;
  U6 enc_est = 0;

	if (sym == 0){
		if (level == 0){
			enc_est = 0;
		}
		else{
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
	}
	return enc_est;
}
int enc_eco_level_est_3(U10 sym)
{

#pragma HLS INLINE
	U4 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (sym == 0)
		enc_est = 0;
	else if (sym < ( 9)) {
		enc_est = sym;
		//enc_est += 2;
	}
	else {
		sym -= ( 9);
		enc_est = 8;
		if (sym == ( 0)) {
			exp_golomb_order = 0;
		}
		else if (sym >= ( 1) && sym < ( 3)) {
			exp_golomb_order = 1;
		}
		else if (sym >= ( 3) && sym < ( 7)) {
			exp_golomb_order = 2;
		}
		else if (sym >= ( 7) && sym < ( 15)) {
			exp_golomb_order = 3;
		}
		else if (sym >= ( 15) && sym < ( 31)) {
			exp_golomb_order = 4;
		}
		else if (sym >= ( 31) && sym < ( 63)) {
			exp_golomb_order = 5;
		}
		else if (sym >= ( 63) && sym < ( 127)) {
			exp_golomb_order = 6;
		}
		else if (sym >= ( 127) && sym < ( 255)) {
			exp_golomb_order = 7;
		}
		else if (sym >= ( 255) && sym < ( 511)) {
			exp_golomb_order = 8;
		}
		else if (sym >= ( 512) && sym < ( 1023)) {
			exp_golomb_order = 9;
		}
		else if (sym >= ( 1023) && sym < ( 2047)) {
			exp_golomb_order = 10;
		}
		else if (sym >= ( 2047) && sym < ( 4095)) {
			exp_golomb_order = 11;
		}
		enc_est += 2 * exp_golomb_order;
	}
	return enc_est;
}
int enc_eco_run_est_3(U10 run)
{

#pragma HLS INLINE
	U4 exp_golomb_order = 0;
	U6 enc_est = 0;

	if (run < (16)) {
		enc_est += run + 1;
	}
	else {
		run -= (16);
		enc_est += 16;
		if (run == (0)) {
			exp_golomb_order = 0;
		}
		else if (run >= (1) && run < (3)) {
			exp_golomb_order = 1;
		}
		else if (run >= (3) && run < (7)) {
			exp_golomb_order = 2;
		}
		else if (run >= (7) && run < (15)) {
			exp_golomb_order = 3;
		}
		else if (run >= (15) && run < (31)) {
			exp_golomb_order = 4;
		}
		else if (run >= (31) && run < (63)) {
			exp_golomb_order = 5;
		}
		else if (run >= (63) && run < (127)) {
			exp_golomb_order = 6;
		}
		else if (run >= (127) && run < (255)) {
			exp_golomb_order = 7;
		}
		else if (run >= (255) && run < (511)) {
			exp_golomb_order = 8;
		}
		else if (run >= (512) && run < (1023)) {
			exp_golomb_order = 9;
		}
		else if (run >= (1023) && run < (2047)) {
			exp_golomb_order = 10;
		}
		else if (run >= (2047) && run < (4095)) {
			exp_golomb_order = 11;
		}
		enc_est += 2 * exp_golomb_order;
	}
	return enc_est;
}
static int rate_coef_est_4(s16 coef[4], U10 run[4])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;
	for (i = 0; i < 4; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=4
#pragma HLS UNROLL
		u16  level;
		level = abs(coef[i]);
		//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
		//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
		enc_est += enc_eco_run_est_2(run[i], level);
		enc_est += enc_eco_level_est_2(level);
	}
	return enc_est;
}
static int rate_coef_est_8(s16 coef[8], U10 run[8])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;
	for (i = 0; i < 8; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS UNROLL
		u16  level;
		level = abs(coef[i]);
		//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
		//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
		enc_est += enc_eco_run_est_2(run[i], level);
		enc_est += enc_eco_level_est_2(level);
	}
	return enc_est;
}
static int rate_coef_est_16(s16 coef[16], U10 run[16])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;
	for (i = 0; i < 16; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS UNROLL
		u16  level;
		level = abs(coef[i]);
		//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
		//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
		enc_est += enc_eco_run_est_2(run[i], level);
		enc_est += enc_eco_level_est_2(level);
	}
	return enc_est;
}
static int rate_coef_est_32(s16 coef[32], U10 run[32])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;

	for (i = 0; i < 32; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=32
#pragma HLS UNROLL
		u16  level;
		level = abs(coef[i]);
		//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
		//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
		enc_est += enc_eco_run_est_2(run[i], level);
		enc_est += enc_eco_level_est_2(level);
	}
	return enc_est;
}
static int rate_coef_est_64(s16 coef[64], U10 run[64])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
	U12 enc_est = 0;
	U6 i;
	for (i = 0; i < 64; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=64
#pragma HLS UNROLL
		u16  level;
		level = abs(coef[i]);
		//level = ((((coef[i]) ^ (coef[i]) >> 15) - ((coef[i]) >> 15)));
		//level = (coef[i]) ^ ((coef[i]) >> 15) - ((coef[i]) >> 15);
		enc_est += enc_eco_run_est_2(run[i], level);
		enc_est += enc_eco_level_est_2(level);
	}
	return enc_est;
}










int encode_skip_idx_cu_est(u8  skip_idx, u8 num_hmvp_cands, u8 slice_type)
{

	U8 enc_est = 0;
	U8 val;
	U8 max_skip_num;
	// for P slice, change 3, 4, ..., 13 to 1, 2, ..., 11
	if ((slice_type) == 2 && skip_idx > 0) {
		skip_idx -= 2;
	}
	val = skip_idx;
	max_skip_num = (((slice_type) == 2 ? 2 : (PRED_DIR_NUM) + 1)) + num_hmvp_cands;
	enc_est += val;
	if (skip_idx != max_skip_num - 1) {
		enc_est++;
	}
	return enc_est;
}
int encode_intra_dir_est(u8 ipm, u8 mpm[2])
{
	U3 enc_est = 0;
	S8 ipm_code = (ipm) == (mpm[0]) ? -2 : (((mpm[1]) == (ipm) ? -1 : (((ipm) < (mpm[0]) ? (ipm) : (((ipm) < (mpm[1]) ? (ipm) - 1 : (ipm) - 2))))));
	if (ipm_code < 0) {
		enc_est += 2;
	}
	else {
		enc_est += 6;
	}
	return enc_est;
}

#define W 64
#define H 64
#include "rate_est.h"
#define W 64
#define H 32
#include "rate_est.h"
#define W 32
#define H 64
#include "rate_est.h"
#define W 64
#define H 16
#include "rate_est.h"
#define W 16
#define H 64
#include "rate_est.h"
#define W 64
#define H 8
#include "rate_est.h"
#define W 8
#define H 64
#include "rate_est.h"
//32
#define W 32
#define H 32
#include "rate_est.h"
#define W 32
#define H 16
#include "rate_est.h"
#define W 16
#define H 32
#include "rate_est.h"
#define W 32
#define H 8
#include "rate_est.h"
#define W 8
#define H 32
#include "rate_est.h"
#define W 32
#define H 4
#include "rate_est.h"
#define W 4
#define H 32
#include "rate_est.h"
//16
#define W 16
#define H 16
#include "rate_est.h"
#define W 16
#define H 8
#include "rate_est.h"
#define W 8
#define H 16
#include "rate_est.h"
#define W 16
#define H 4
#include "rate_est.h"
#define W 4
#define H 16
#include "rate_est.h"
//8
#define W 8
#define H 8
#include "rate_est.h"
#define W 8
#define H 4
#include "rate_est.h"
#define W 4
#define H 8
#include "rate_est.h"
#define W 4
#define H 4
#include "rate_est.h"


int enc_eco_cbf_est_32(RDOQ_MODEL* model_cnt, int num_nz[4][3], u8 pred_mode,
	S8 ipm[4][2], u8 tree_status, int cu_width_log2, int cu_height_log2)
{
	U4 enc_est = 0;
	//COM_MODE *mod_info_curr = &core->mod_info_curr;
	U1 ctp_zero_flag = !is_cu_nz(num_nz);
	/* code allcbf */
	if ((pred_mode) != 0) {
		if (!((pred_mode) == 3)) {
			if (tree_status == TREE_LC) {
				if (cu_width_log2 > 6 || cu_height_log2 > 6) {
					enc_est++;
				}
				else {
					enc_est++;
				}
				if (ctp_zero_flag) {
					return 0;
				}
			}
		}

		if (tree_status == TREE_LC) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
		if (num_nz[0][1] + num_nz[0][2] == 0 && tree_status == TREE_LC) {
		}
		else {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			model_cnt->cbf[0]++;
		}
	}
	else {
		if (!((ipm[0][0]) == 33)) {
			//int part_num = get_part_num((PART_SIZE)tb_part_size);

			enc_est += 1;
			model_cnt->cbf[0]++;

		}
		if (tree_status == TREE_LC) {
			if (!((ipm[0][0]) == 33 && (ipm[0][1]) == 0)) {
				enc_est += 2;
				model_cnt->cbf[1]++;
			}
		}
	}
	return enc_est;
}

int enc_bit_est_flag_32(RDOQ_MODEL* model_cnt, s32 slice_type, int f_scu, u8 curr_mvr, u8 tree_status, u8 cons_pred_mode, int cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], int cu_width_log2, int cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], int cur_ptr, int refp_ptr[17][2], u8 num_of_hmvp_cand, u8 mpm[4][2], int intra)
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
			enc_est += encode_affine_flag_est(((affine_flag)) != 0, 1, cu_width_log2, cu_height_log2);
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
			enc_est += enc_eco_cbf_est_32(model_cnt, num_nz, cu_mode, ipm, tree_status, cu_width_log2, cu_height_log2);
		}
		else if (!((cu_mode) == 0 && (ipm[0][0]) == 33
			&& (ipm[0][1]) == 0)) {
			enc_est += 2;
			model_cnt->cbf[1]++;
		}
	}

	return enc_est;
}



