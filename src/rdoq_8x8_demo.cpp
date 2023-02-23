#if 1
#include "md1.h"
#else
#include "ap_int.h"
#include <stdlib.h>

//unsigned types
typedef  ap_uint<1>						U1;
typedef  ap_uint<2>						U2;
typedef  ap_uint<3>						U3;
typedef  ap_uint<4>						U4;
typedef  ap_uint<5>						U5;
typedef  ap_uint<6>						U6;
typedef  ap_uint<7>						U7;
typedef  ap_uint<8>						U8;
typedef  ap_uint<9>						U9;
typedef  ap_uint<10>					U10;
typedef  ap_uint<11>					U11;
typedef  ap_uint<12>					U12;
typedef  ap_uint<13>					U13;
typedef  ap_uint<14>					U14;
typedef  ap_uint<15>					U15;
typedef  ap_uint<16>					U16;
typedef  ap_uint<17>				    U17;
typedef  ap_uint<18>					U18;
typedef  ap_uint<19>					U19;
typedef  ap_uint<20>					U20;
typedef  ap_uint<21>					U21;
typedef  ap_uint<22>					U22;
typedef  ap_uint<23>					U23;
typedef  ap_uint<24>					U24;
typedef  ap_uint<25>					U25;
typedef  ap_uint<26>					U26;
typedef  ap_uint<27>				    U27;
typedef  ap_uint<28>					U28;
typedef  ap_uint<29>					U29;
typedef  ap_uint<30>					U30;
typedef  ap_uint<31>					U31;
typedef  ap_uint<32>					U32;
typedef  ap_uint<33>					U33;
typedef  ap_uint<34>					U34;
typedef  ap_uint<35>					U35;
typedef  ap_uint<36>					U36;
typedef  ap_uint<37>				    U37;
typedef  ap_uint<38>					U38;
typedef  ap_uint<39>					U39;
typedef  ap_uint<40>					U40;
typedef  ap_uint<41>					U41;
typedef  ap_uint<42>					U42;
typedef  ap_uint<43>					U43;
typedef  ap_uint<44>					U44;
typedef  ap_uint<45>					U45;
typedef  ap_uint<46>					U46;
typedef  ap_uint<47>				    U47;
typedef  ap_uint<48>					U48;
typedef  ap_uint<49>					U49;
typedef  ap_uint<50>					U50;
typedef  ap_uint<51>					U51;
typedef  ap_uint<52>					U52;
typedef  ap_uint<53>					U53;
typedef  ap_uint<54>					U54;
typedef  ap_uint<55>					U55;
typedef  ap_uint<56>					U56;
typedef  ap_uint<57>				    U57;
typedef  ap_uint<58>					U58;
typedef  ap_uint<59>					U59;
typedef  ap_uint<60>					U60;
typedef  ap_uint<61>					U61;
typedef  ap_uint<62>					U62;
typedef  ap_uint<63>					U63;
typedef  ap_uint<64>					U64;

//signed types

typedef  ap_int<1>						S1;
typedef  ap_int<2>						S2;
typedef  ap_int<3>						S3;
typedef  ap_int<4>						S4;
typedef  ap_int<5>						S5;
typedef  ap_int<6>						S6;
typedef  ap_int<7>						S7;
typedef  ap_int<8>						S8;
typedef  ap_int<9>						S9;
typedef  ap_int<10>						S10;
typedef  ap_int<11>						S11;
typedef  ap_int<12>						S12;
typedef  ap_int<13>						S13;
typedef  ap_int<14>						S14;
typedef  ap_int<15>						S15;
typedef  ap_int<16>						S16;
typedef  ap_int<17> 			  	    S17;
typedef  ap_int<18>						S18;
typedef  ap_int<19>						S19;
typedef  ap_int<20>						S20;
typedef  ap_int<21>						S21;
typedef  ap_int<22>						S22;
typedef  ap_int<23>						S23;
typedef  ap_int<24>						S24;
typedef  ap_int<25>						S25;
typedef  ap_int<26>						S26;
typedef  ap_int<27>				  	    S27;
typedef  ap_int<28>						S28;
typedef  ap_int<29>						S29;
typedef  ap_int<30>						S30;
typedef  ap_int<31>						S31;
typedef  ap_int<32>						S32;
typedef  ap_int<33>						S33;
typedef  ap_int<34>						S34;
typedef  ap_int<35>						S35;
typedef  ap_int<36>						S36;
typedef  ap_int<37>				  	    S37;
typedef  ap_int<38>						S38;
typedef  ap_int<39>						S39;
typedef  ap_int<40>						S40;
typedef  ap_int<41>						S41;
typedef  ap_int<42>						S42;
typedef  ap_int<43>						S43;
typedef  ap_int<44>						S44;
typedef  ap_int<45>						S45;
typedef  ap_int<46>						S46;
typedef  ap_int<47>				  	    S47;
typedef  ap_int<48>						S48;
typedef  ap_int<49>						S49;
typedef  ap_int<50>						S50;
typedef  ap_int<51>						S51;
typedef  ap_int<52>						S52;
typedef  ap_int<53>						S53;
typedef  ap_int<54>						S54;
typedef  ap_int<55>						S55;
typedef  ap_int<56>						S56;
typedef  ap_int<57>				  	    S57;
typedef  ap_int<58>						S58;
typedef  ap_int<59>						S59;
typedef  ap_int<60>						S60;
typedef  ap_int<61>						S61;
typedef  ap_int<62>						S62;
typedef  ap_int<63>						S63;
typedef  ap_int<64>						S64;
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define COM_INT32_MAX           ((S32)0x7FFFFFFF)
#define Y_C                             0

struct _RDOQ_MODEL {
	U32 cbf[3];
	U32 run_rdoq[24];
	U32 level[24];
	U32 last1[12];
	U32 last2[22];
};

typedef struct _RDOQ_MODEL RDOQ_MODEL;

#endif
#pragma warning(disable:6297)
#pragma warning(disable:4334)

#define GET_I_COST(rate, lamba)  (rate*lamba)
static void level_core_8(U8 w, U8 h, U32 level[24], U32 level1[24], U5 t0[64], U7 i, U14 max_num)
{
#pragma HLS ARRAY_PARTITION variable=level1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS ARRAY_PARTITION variable=level complete dim=1
	int tmp = 0;
	int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	max_num = COM_MIN(max_num, (U14)64);
	for (pos = 0; pos < 64; pos++)
	{
#pragma HLS UNROLL
		if (pos < max_num)
			tmp += !(t0[pos] - i);
	}
	level[i] += tmp;
	level1[i + 1] += 8 * tmp;
}
static void run_core_8(U8 w, U8 h, U32 run[24], U32 run1[24], U5 t0[64], U7 i, U1 mat_run[64], U14 max_num)
{
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
#pragma HLS ARRAY_PARTITION variable=run1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
	int tmp = 0;
	int tmp2 = 0;
	int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
#pragma HLS UNROLL
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
			tmp2 += mat_run[pos];
		}
	}
	run[i] += tmp;
	run1[i + 1] += tmp2;
}
static void last1_core_8(U8 w, U8 h, U32 last1[12], U4 t0[64], U7 i, U14 max_num)
{
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
	int tmp = 0;
	int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
#pragma HLS UNROLL
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
		}
	}
	last1[i] += tmp;
}
static void last2_core_8(U8 w, U8 h, U32 last2[22], U5 t0[64], U7 i, U14 max_num)
{
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS FUNCTION_INSTANTIATE variable=i
	int tmp = 0;
	int pos = 0;
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	for (pos = 0; pos < 64; pos++)
	{
#pragma HLS UNROLL
		if (pos < max_num)
		{
			tmp += !(t0[pos] - i);
		}
	}
	last2[i] += tmp;
}
static void model_update_8(U8 w, U8 h, u32 last1[12], u32 last2[22], u32 run[24], u32 level[24], U1 mat_run[64], U24 mat_prev_level[64], U2 ch_type, U14 max_num)
{
#pragma HLS ARRAY_PARTITION variable=last2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=last1 complete dim=1
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=run complete dim=1
#pragma HLS ARRAY_PARTITION variable=level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1

	U7 scan_pos = 0;
	U5 t0[64] = { 0 };
#pragma HLS ARRAY_PARTITION variable=t0 complete dim=1
	U4 last1_pos[64] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last1_pos complete dim=1
	U5 last2_pos[64] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last2_pos complete dim=1
	U32 last1_tmp[12] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last1_tmp complete dim=1
	U32 last2_tmp[22] = { 0 };
#pragma HLS ARRAY_PARTITION variable=last2_tmp complete dim=1
	U32 level_tmp[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=level_tmp complete dim=1
	U32 level_tmp1[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=level_tmp1 complete dim=1
	U32 run_tmp[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=run_tmp complete dim=1
	U32 run_tmp1[24] = { 0 };
#pragma HLS ARRAY_PARTITION variable=run_tmp1 complete dim=1
	int ace_log2 = 0;

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num)
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
			else if (scan_pos + 1 < 64) {
				ace_log2 = 5;
			}
			else if (scan_pos + 1 < 128) {
				ace_log2 = 6;
			}
			else {
				ace_log2 = 5;
			}

			t0[scan_pos] = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5))) << 1) + (ch_type == 0 ? 0 : 12);
			last1_pos[scan_pos] = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5)))) + (ch_type == 0 ? 0 : 6);
			last2_pos[scan_pos] = ace_log2 + (ch_type == 0 ? 0 : 12);
		}
	}

	for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
	{
#pragma HLS UNROLL
		level_core_8(w, h, level_tmp, level_tmp1, t0, scan_pos, max_num);
		run_core_8(w, h, run_tmp, run_tmp1, t0, scan_pos, mat_run, max_num);
	}

	for (scan_pos = 0; scan_pos < 12; scan_pos++)
	{
#pragma HLS UNROLL
		last1_core_8(w, h, last1_tmp, last1_pos, scan_pos, max_num);
		last1[scan_pos] += last1_tmp[scan_pos];
	}

	for (scan_pos = 0; scan_pos < 22; scan_pos++)
	{
#pragma HLS UNROLL
		last2_core_8(w, h, last2_tmp, last2_pos, scan_pos, max_num);
		last2[scan_pos] += last2_tmp[scan_pos];
	}

	for (scan_pos = 0; scan_pos < 24; scan_pos++)
	{
#pragma HLS UNROLL
		level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
		run[scan_pos] += run_tmp[scan_pos] + run_tmp1[scan_pos];
	}
}
static void model_update_8_luma(U8 w, U8 h, U1 mat_run[64], U24 mat_prev_level[64], U14 max_num, int* sum_run, U32 last1_tmp[6], U32 last2_tmp[7])
{
	int ace_log2 = 0;
	U7 scan_pos = 0;
	int sum_run_temp = 0;
	U32 last1_tmp_temp[6];
	U32 last2_tmp_temp[7];
	for (int i = 0; i < 6; i++)
	{
		last1_tmp_temp[i] = 0;
		last2_tmp_temp[i] = 0;
	}
	last2_tmp_temp[6] = 0;
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		if (scan_pos < max_num)
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
			else if (scan_pos + 1 < 64) {
				ace_log2 = 5;
			}
			else if (scan_pos + 1 < 128) {
				ace_log2 = 6;
			}
			else {
				ace_log2 = 5;
			}
			U4 last1_pos = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5))));
			U5 last2_pos = ace_log2;
			sum_run_temp += mat_run[scan_pos];
			// easier for hw implementation.
			switch (last1_pos)
			{
			case  0: last1_tmp_temp[0]++; break;
			case  1: last1_tmp_temp[1]++; break;
			case  2: last1_tmp_temp[2]++; break;
			case  3: last1_tmp_temp[3]++; break;
			case  4: last1_tmp_temp[4]++; break;
			case  5: last1_tmp_temp[5]++; break;
			default: break;
			}
			switch (last2_pos)
			{
			case  0: last2_tmp_temp[0]++; break;
			case  1: last2_tmp_temp[1]++; break;
			case  2: last2_tmp_temp[2]++; break;
			case  3: last2_tmp_temp[3]++; break;
			case  4: last2_tmp_temp[4]++; break;
			case  5: last2_tmp_temp[5]++; break;
			case  6: last2_tmp_temp[6]++; break;
			default: break;
			}
		}
	}

	*sum_run = sum_run_temp;
	for (int i = 0; i < 6; i++)
	{
		last1_tmp[i] = last1_tmp_temp[i];
		last2_tmp[i] = last2_tmp_temp[i];
	}
	last2_tmp[6] = last2_tmp_temp[6];


}

static void model_update_8_chroma(U8 w, U8 h, U1 mat_run[64], U24 mat_prev_level[64], U14 max_num, int* sum_run, U32 last1_tmp[6], U32 last2_tmp[7])
{
	int ace_log2 = 0;
	U7 scan_pos = 0;
	int sum_run_temp = 0;
	U32 last1_tmp_temp[6];
	U32 last2_tmp_temp[7];
	for (int i = 0; i < 6; i++)
	{
		last1_tmp_temp[i] = 0;
		last2_tmp_temp[i] = 0;
	}
	last2_tmp_temp[6] = 0;
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		if (scan_pos < max_num)
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
			else if (scan_pos + 1 < 64) {
				ace_log2 = 5;
			}
			else if (scan_pos + 1 < 128) {
				ace_log2 = 6;
			}
			else {
				ace_log2 = 5;
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
			case  18: last2_tmp_temp[6]++; break;
			default: break;
			}
		}
	}
	*sum_run = sum_run_temp;
	for (int i = 0; i < 6; i++)
	{
		last1_tmp[i] = last1_tmp_temp[i];
		last2_tmp[i] = last2_tmp_temp[i];
	}
	last2_tmp[6] = last2_tmp_temp[6];
}


static S64 get_ic_rate_cost_rl0(U5 run, U5 ctx_run, S64 lambda, s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2])
{

#pragma HLS INLINE off
	U20 rate;

	rate = 0;
	if (run == ((unsigned int)0)) {
		rate += rdoq_est_run_local[ctx_run][0];
	}
	else {
		rate += rdoq_est_run_local[ctx_run + 1][0];
	}

	return ((rate)*lambda);
}

static S64 get_ic_rate_cost_rl(U32 abs_level, U5 run, U5 ctx_run, U5 ctx_level, S64 lambda, U1 last_pos, s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2])
{

#pragma HLS INLINE off
	int rate;
	int rate_tmp[3] = { 0 };
	int golomb;
	if (abs_level <= 9) {
		golomb = 1;
	}
	else if (abs_level <= 11) {
		golomb = 3;
	}
	else if (abs_level <= 15) {
		golomb = 5;
	}
	else if (abs_level <= 23) {
		golomb = 7;
	}
	else if (abs_level <= 39) {
		golomb = 9;
	}
	else if (abs_level <= 71) {
		golomb = 11;
	}
	else if (abs_level <= 135) {
		golomb = 13;
	}
	else if (abs_level <= 263) {
		golomb = 15;
	}
	else if (abs_level <= 519) {
		golomb = 17;
	}
	else if (abs_level <= 1031) {
		golomb = 19;
	}
	else if (abs_level <= 2055) {
		golomb = 21;
	}
	else if (abs_level <= 4103) {
		golomb = 23;
	}
	else {
		golomb = 25;
	}
	// sign of coeff
	rate = 32768;
	if (!(last_pos)) {
		if (run == 0) {
			rate += rdoq_est_run_local[ctx_run][1];
		}
		else {
			rate += rdoq_est_run_local[ctx_run + 1][1];
		}
	}

	rate_tmp[0] += rdoq_est_level_local[ctx_level][1];
	rate_tmp[1] += rdoq_est_level_local[ctx_level][0];
	rate_tmp[1] += rdoq_est_level_local[ctx_level + (1)][0] * 7;
	rate_tmp[1] += 32768 * golomb;
	rate_tmp[2] += rdoq_est_level_local[ctx_level][0];
	rate_tmp[2] += rdoq_est_level_local[ctx_level + (1)][0] * ((abs_level - (2)));
	rate_tmp[2] += rdoq_est_level_local[ctx_level + (1)][1];

	if (abs_level == (1)) {
		rate += rate_tmp[0];
	}
	else if (abs_level >= (9)) {
		rate += rate_tmp[1];
	}
	else {
		rate += rate_tmp[2];
	}
	return ((rate)*lambda);
}

static S64 cal_cost(S32 last_pos, S32 q_bits, U32 max_abs_level, U36 level_double, U30 err_scale, s64* rd64_uncoded_cost, s64* rd64_coded_cost, U5 run, U5 ctx_run, U5 ctx_level, S64 lambda, s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2])
{
	S64 i64Delta1 = level_double - (max_abs_level << q_bits);
	S64 err1 = i64Delta1 * err_scale >> 20;
	S64 cost1 = err1 * err1 + get_ic_rate_cost_rl(max_abs_level, run, (S32)ctx_run, (U32)ctx_level, lambda, last_pos,
		rdoq_est_run_local, rdoq_est_level_local);

	//cost1 = cost1 >> 24;
	//cost1 = avs3_min(cost1, 65535);
	return cost1;
}
static void cal_cost0(U36 level_double, U30 err_scale, s64* rd64_uncoded_cost, s64* rd64_coded_cost, U5 run, U5 ctx_run, U5 ctx_level, S64 lambda, s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2])
{
	S64 err = level_double * err_scale >> 20;
	*rd64_uncoded_cost = err * err;
	*rd64_coded_cost = *rd64_uncoded_cost + get_ic_rate_cost_rl0(run, (S32)ctx_run, lambda, rdoq_est_run_local, rdoq_est_level_local);
	//*rd64_uncoded_cost = *rd64_uncoded_cost >> 24;
	//*rd64_coded_cost = *rd64_coded_cost >> 24;
}


static U24 get_coded_level_rl_demo(s64* rd64_uncoded_cost, s64* rd64_coded_cost, U36 level_double, u32 max_abs_level,
	U5 run, U5 ctx_run, U5 ctx_level, s32 q_bits, U30 err_scale, s64 lambda, int last_pos, s32 rdoq_est_run_local[24][2],
	s32 rdoq_est_level_local[24][2])
{

#pragma HLS INLINE off
	U32 best_abs_level = 0;
	S64 cost1 = 0;
	cal_cost0(level_double, err_scale, rd64_uncoded_cost, rd64_coded_cost, run, ctx_run, ctx_level, lambda, rdoq_est_run_local, rdoq_est_level_local);
	cost1 = cal_cost(last_pos, q_bits, max_abs_level, level_double, err_scale, rd64_uncoded_cost, rd64_coded_cost, run, ctx_run, ctx_level, lambda, rdoq_est_run_local, rdoq_est_level_local);

	if (cost1 < *rd64_coded_cost) {
		*rd64_coded_cost = cost1;
		best_abs_level = max_abs_level;
	}
	else {
		best_abs_level = (0);
	}
	return best_abs_level;
}


static S4 get_transform_shift1(const U4 bit_depth, const U4 tr_size_log2)
{
	return 15 - bit_depth - tr_size_log2;
}
static U4 get_ace_log2(U8 scan_pos)
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

void pre_quant(U8 scan[64], s16 src_coef[8][8], U6 qp, S32 q_bits, U36 tmp_level_double[64], S16 tmp_coef[64])
{
	U3 cu_width_log2 = 3;
	U3 cu_height_log2 = 3;
	U16 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

	const U16 scale = quant_scale[qp];
	const U4 ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const U8 ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const U8 ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const U20 q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	for (U8 scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		U8 x = blk_pos & (8 - 1);
		U8 y = blk_pos >> 3;

		U36 level_double = src_coef[y][x];
		S25 max_abs_level;
		U1 lower_int;
		U36 temp_level;
		temp_level = (U36)abs(src_coef[y][x]) * q_value;
		level_double = (U36)COM_MIN((temp_level),(U36)(COM_INT32_MAX - (1 << (q_bits - 1))));
		tmp_level_double[blk_pos] = level_double;
		max_abs_level = (S32)(level_double >> q_bits);
		lower_int = ((level_double - (max_abs_level << q_bits)) < (1 << (q_bits - 1))) ? 1 : 0;
		if (!lower_int)
		{
			max_abs_level++;
		}
		tmp_coef[blk_pos] = src_coef[y][x] > 0 ? (S16)max_abs_level : (S16)(-max_abs_level);
	}
}

void doc(s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], U24 lev_opt[64], S64 mat_uncoded_cost[64], S64 mat_coded_cost[64], S16 tmp_dst_coef[64], U36 tmp_level_double[64], U8 scan[64], S16 tmp_coef[64], U24 mat_prev_level[64], U1 mat_run[64], U2 ch_type, S32 q_bits, S64 err_scale, S64 lambda)
{
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	S16 tmp = 0;
	mat_prev_level[0] = 6;

	for (U8 scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];

		if (tmp_coef[blk_pos])
		{
			mat_prev_level[scan_pos] = abs(tmp_coef[blk_pos]);
			tmp = abs(tmp_coef[blk_pos]);
			mat_run[scan_pos] = 0;
		}
		else
		{
			mat_prev_level[scan_pos] = 1;
			mat_run[scan_pos] = 1;
		}
	}

	for (U8 scan_pos = 0; scan_pos < 64; scan_pos++)
	{

		U8 blk_pos = scan[scan_pos];
		U24 level;
		U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);

		int iflast = (scan_pos == 63);
		level = get_coded_level_rl_demo(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[blk_pos], abs(tmp_coef[blk_pos]),
			mat_run[scan_pos], ctx_run, ctx_run, q_bits, err_scale, lambda, iflast, rdoq_est_run_local, rdoq_est_level_local);
		S17 level_cbf = level;
		tmp_dst_coef[blk_pos] = (S16)(tmp_coef[blk_pos] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);

		mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
		mat_coded_cost[scan_pos] = d64_coded_cost;
		lev_opt[scan_pos] = level;
	}
}

void DLNP_new(int* best_last_idx_p1, U24 lev_opt[64], S64 mat_coded_cost[64], S64 mat_uncoded_cost[64], S16 tmp_dst_coef[64], S32 rdoq_est_cbf[3][2], U8 scan[64], U24 mat_prev_level[64], S32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda)
{
	const U1 ctx_last = (ch_type == Y_C) ? 0 : 1;
	S64 d64_cost_last_zero[64] = { 0 };

	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U8 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost[64] = { 0 };
	S64 base_cost_buffer[64] = { 0 };
	// added by xfhuang.
	S64 base_cost_buffer_tmp[64] = { 0 };
	U1 lev_opt_tmp[64] = { 0 };
	S64 d64_cost_last_zero_tmp[64] = { 0 };
	S64 d64_base_cost_tmp[64] = { 0 };
	S64 d64_best_cost_tmp = 0;
	static int count_tmp = 0;
	int best_last_idx_p1_tmp = 0;

	count_tmp++;

	if (count_tmp == 20445)
	{
		int k = 0;
	}

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
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos);
		U4 flag = 0;
		if (mat_prev_level[scan_pos] - 1 < 5)
		{
			flag = mat_prev_level[scan_pos] - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_zero[scan_pos] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
		//d64_cost_last_zero[scan_pos] = d64_cost_last_zero[scan_pos] >> 24;
		d64_cost_last_zero_tmp[blk_pos] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda; // block scan order
		//d64_cost_last_zero_tmp[blk_pos] = d64_cost_last_zero_tmp[blk_pos] >> 24;
	}

	// added by xfhuang 
	S64 d64_cost_last_one_tmp[64];
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos + 1);
		U24 prev_level = mat_prev_level[scan_pos];
		U4 flag = 0;
		if (prev_level - 1 < 5)
		{
			flag = prev_level - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_one_tmp[blk_pos] = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda); // block scan order
		//d64_cost_last_one_tmp[blk_pos] = d64_cost_last_one_tmp[blk_pos] >> 24;
	}

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		for (U8 i = 0; i < 64; i++)
		{
			if (i <= scan_pos)
			{
				if (lev_opt[i])
				{
					base_cost_buffer[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
				}
				else
				{
					base_cost_buffer[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i];
				}
				// base_cost_buffer[scan_pos] += -i;
				d64_base_cost_tmp[blk_pos] = base_cost_buffer[scan_pos];
			}
		}
	}

#if 1
	// backward to block pose
	// mat_coded_cost mat_uncoded_cost lev_opt are input as zig-zag scan ordeer
	for (int scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		int blk_pos = scan[scan_pos];
		if (lev_opt[scan_pos])
		{
			base_cost_buffer_tmp[blk_pos] = mat_coded_cost[scan_pos] - mat_uncoded_cost[scan_pos] + d64_cost_last_zero[scan_pos];
		}
		else
		{
			base_cost_buffer_tmp[blk_pos] = mat_coded_cost[scan_pos] - mat_uncoded_cost[scan_pos];
		}
		// base_cost_buffer_tmp[blk_pos] = -scan_pos;
		lev_opt_tmp[blk_pos] = lev_opt[scan_pos] != 0;
	}

	S64 finalRdoqCost = 0;
	S64 tempCost[8];   // all scan line cost
	S64 endPosCost[8]; // as the last position cost.
	S64 rdoqD64LastOne[8] = { -1,-1,-1,-1,-1,-1,-1,-1 }; // shifter buffer the d64_cost_last_one_tmp
	int last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int rdoq_last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int rdoq_last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int final_last_x = -1, final_last_y = -1;
	int final_rdoq_last_x = -1, final_rdoq_last_y = -1;

	for (int i = 0; i < 8; i++) // horizontal
	{
		int zzz = 0;
		for (int j = 0; j < 8; j++) // vertical
		{
			if (i == 0)
			{
				// tempCost[j] = base_cost_buffer_tmp[j];
				if (lev_opt_tmp[j])
				{
					last_x[j] = i;
					last_y[j] = j;

					rdoq_last_x[j] = i;
					rdoq_last_y[j] = j;
					rdoqD64LastOne[j] = d64_cost_last_one_tmp[j];
				}
				tempCost[j] = base_cost_buffer_tmp[j];
				if ((i + j) % 2 == 1) // bottom - left scan for (i+j)%2 is odd case
				{
					if (lev_opt_tmp[j]) endPosCost[j] = base_cost_buffer_tmp[j];
					else endPosCost[j] = 0;
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (lev_opt_tmp[j]) endPosCost[j] = 0;
					else endPosCost[j] = base_cost_buffer_tmp[j];

					// a special case (0, 0) position.
					if (j == 0)  // substract here is to get the actual endPosCost
					{
						endPosCost[j] = tempCost[j] - endPosCost[j];
					}
				}
			}
			else if (j == 7)
			{
				last_x[j] = -1;
				last_y[j] = -1;

				rdoq_last_x[j] = -1;
				rdoq_last_y[j] = -1;
				rdoqD64LastOne[j] = -1;
				if (lev_opt_tmp[i * 8 + j])
				{
					last_x[j] = i;
					last_y[j] = j;

					rdoq_last_x[j] = i;
					rdoq_last_y[j] = j;

					rdoqD64LastOne[j] = d64_cost_last_one_tmp[i * 8 + j];
				}
				tempCost[j] = base_cost_buffer_tmp[i * 8 + j];

				if ((i + j) % 2 == 1) // bottom - left scan for (i+j)%2 is odd case
				{
					if (lev_opt_tmp[i * 8 + j]) endPosCost[j] = base_cost_buffer_tmp[i * 8 + j];
					else endPosCost[j] = 0;
				}
				else // above - top scan for (i+j)%2 is even case
				{
					if (lev_opt_tmp[i * 8 + j]) endPosCost[j] = 0;
					else endPosCost[j] = base_cost_buffer_tmp[i * 8 + j];
				}
			}
			else
			{
				if ((i + j) % 2 == 1) // bottom - left scan for (i+j)%2 is odd case
				{
					if (lev_opt_tmp[i * 8 + j])
					{
						last_x[j] = i;
						last_y[j] = j;
					}

					if (lev_opt_tmp[i * 8 + j] && rdoq_last_x[j] != -1) // compare for intialized case.
					{
						// add the rdoqD64LastOne consider here.
						// compare the best LNP for bottom-right scan
						if (tempCost[j] + base_cost_buffer_tmp[i * 8 + j] + d64_cost_last_one_tmp[i * 8 + j] < endPosCost[j] + rdoqD64LastOne[j])
						{
							rdoq_last_x[j] = i;
							rdoq_last_y[j] = j;
							endPosCost[j] = tempCost[j] + base_cost_buffer_tmp[i * 8 + j];
							rdoqD64LastOne[j] = d64_cost_last_one_tmp[i * 8 + j];
						}
					}
					if (lev_opt_tmp[i * 8 + j] && rdoq_last_x[j] == -1) // cost initial. purpose.
					{
						rdoq_last_x[j] = i;
						rdoq_last_y[j] = j;
						endPosCost[j] = tempCost[j] + base_cost_buffer_tmp[i * 8 + j];
						rdoqD64LastOne[j] = d64_cost_last_one_tmp[i * 8 + j];
					}

					tempCost[j] += base_cost_buffer_tmp[i * 8 + j];
				}
				else // above - top scan for (i+j)%2 is even case
				{
					S64 curCost = base_cost_buffer_tmp[i * 8 + j];  // care the bit-width. not int.

					if (lev_opt_tmp[i * 8 + j] && last_x[j] == -1) // first non-zero position
					{
						last_x[j] = i;
						last_y[j] = j;
					}

					if (lev_opt_tmp[i * 8 + j] && rdoq_last_x[j] != -1) // the previous non-zero position
					{
						// consider d64_cost_last_one_tmp part cost.
						if (tempCost[j] - endPosCost[j] + rdoqD64LastOne[j] - d64_cost_last_one_tmp[i * 8 + j] > 0) // previous position is better
						{
							rdoq_last_x[j] = i;
							rdoq_last_y[j] = j;
							endPosCost[j] = tempCost[j];
						}
					}

					if (lev_opt_tmp[i * 8 + j] && rdoq_last_x[j] == -1) // first non-zero position
					{
						rdoq_last_x[j] = i;
						rdoq_last_y[j] = j;
						// from start to current position (except current pos.)
						endPosCost[j] = tempCost[j];
						rdoqD64LastOne[j] = d64_cost_last_one_tmp[i * 8 + j];
					}

					tempCost[j] += curCost; // from the start point to current pos cost.

					if (j == 0)  // substract here is to get the actual endPosCost
					{
						endPosCost[j] = tempCost[j] - endPosCost[j];
					}
				}
			}
		}

		// each row assign.
		if (last_x[0] != -1)
		{
			final_last_x = last_x[0];
			final_last_y = last_y[0];
		}

		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];

		int index = rdoq_last_x[0] * 8 + rdoq_last_y[0];

		// cross check.
		if (index >= 0 && temp_RdoqCost != d64_base_cost_tmp[index])
		{
			int check = 0;
			// printf("file:%s, func: %s, line: %d\n", __FILE__, __func__, __LINE__);
			// assert(false);
		}

		if (rdoq_last_x[0] != -1 && temp_RdoqCost + d64_cost_last_one_tmp[index] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + d64_cost_last_one_tmp[index];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];

		// shift register.
		for (int j = 1; j < 8; j++)
		{
			last_x[j - 1] = last_x[j];
			last_y[j - 1] = last_y[j];
			rdoq_last_x[j - 1] = rdoq_last_x[j];
			rdoq_last_y[j - 1] = rdoq_last_y[j];
			tempCost[j - 1] = tempCost[j];
			endPosCost[j - 1] = endPosCost[j];
			rdoqD64LastOne[j - 1] = rdoqD64LastOne[j];
		}
	}


	for (int i = 0; i < 8; i++)
	{
		if (last_x[i] != -1)
		{
			final_last_x = last_x[i];
			final_last_y = last_y[i];
		}
	}
	if (final_last_x == -1) {
		final_last_x = 0;
		final_last_y = 0;
	}

	// decide for remaining scan lines 
	// not 8; only need iterate 7 times 
	for (int i = 0; i < 7; i++)
	{
		if ((rdoq_last_x[i] + rdoq_last_y[i]) % 2 == 0) // above right scan.
		{
			endPosCost[i] = tempCost[i] - endPosCost[i];
		}
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[i];
		int index = rdoq_last_x[i] * 8 + rdoq_last_y[i];
		if (rdoq_last_x[i] != -1 && temp_RdoqCost + d64_cost_last_one_tmp[index] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + d64_cost_last_one_tmp[index];
			final_rdoq_last_x = rdoq_last_x[i];
			final_rdoq_last_y = rdoq_last_y[i];

			if (index > 0 && temp_RdoqCost != d64_base_cost_tmp[index])
			{
				int check = 0;
				// printf("file:%s, func: %s, line: %d\n", __FILE__, __func__, __LINE__);
				// assert(false);
			}
		}
		finalRdoqCost += tempCost[i];
	}

	// back to zig-zag scan order this can be optimized later.
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		if (blk_pos == final_rdoq_last_x * 8 + final_rdoq_last_y)
		{
			best_last_idx_p1_tmp = scan_pos + 1;
		}
	}
#endif

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		d64_base_cost[scan_pos] = base_cost_buffer[scan_pos];
	}

	int doc_last_pos = 0;
	int doc_last_pos_pre = 0;
	int doc_last_blk_pos = 0;
	// FILE* fp = fopen("last_pos.txt", "a+");
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos + 1);
		U24 prev_level = mat_prev_level[scan_pos];
		U4 flag = 0;
		if (prev_level - 1 < 5)
		{
			flag = prev_level - 1;
		}
		else
		{
			flag = 5;
		}
		S64 d64_cost_last_one = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda);
		if (tmp_dst_coef[blk_pos])
		{
			S64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

			if (d64_cur_is_last_cost < d64_best_cost)
			{
				d64_best_cost = d64_cur_is_last_cost;
				*best_last_idx_p1 = scan_pos + 1;
			}
		}
		// temp code added by xfhuang.
		if (tmp_dst_coef[blk_pos])
		{
			doc_last_blk_pos = blk_pos;
			doc_last_pos_pre = doc_last_pos;
			doc_last_pos = scan_pos + 1;
		}
	}
	// just a check.
	if (doc_last_blk_pos != final_last_x * 8 + final_last_y)
	{
		int k = 0;
	}
	if (*best_last_idx_p1 == best_last_idx_p1_tmp && d64_best_cost_tmp != d64_best_cost)
	{
		int tmp = 0;
	}
	if (*best_last_idx_p1 != best_last_idx_p1_tmp)
	{
		int tmp = 0;
	}
	// assign with new method.
	*best_last_idx_p1 = best_last_idx_p1_tmp;
	// fprintf(fp, "%d, %d, %d\n", doc_last_pos, *best_last_idx_p1, doc_last_pos_pre);
	// fclose(fp);
	int tmp_sum = 0;
}
void DLNP_iter(int* best_last_idx_p1, U24 lev_opt[64], S64 mat_coded_cost[64], S64 mat_uncoded_cost[64], S16 tmp_dst_coef[64], S32 rdoq_est_cbf[3][2], U8 scan[64], U24 mat_prev_level[64], S32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda)
{
	const U1 ctx_last = (ch_type == Y_C) ? 0 : 1;
	S64 d64_cost_last_zero[64] = { 0 };
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U8 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost_base[64] = { 0 };
	S64 d64_base_cost[64] = { 0 };
	S64 base_cost_buffer[64] = { 0 };
	static int c = 0;
	c++;
	if (c == 306) {
		int k = 0;
	}
	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		/*for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
			d64_base_cost_base[scan_pos] = 0;
		}*/
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		/*for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
			d64_base_cost_base[scan_pos] = cbf_cost;
		}*/
		d64_best_cost = d64_best_cost - cbf_cost;
	}

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{

		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos);
		U4 flag = 0;
		if (mat_prev_level[scan_pos] - 1 < 5)
		{
			flag = mat_prev_level[scan_pos] - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_zero[scan_pos] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
	}


	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		if (lev_opt[scan_pos])
		{
			base_cost_buffer[scan_pos] += mat_coded_cost[scan_pos] - mat_uncoded_cost[scan_pos] + d64_cost_last_zero[scan_pos];
		}
		else
		{
			base_cost_buffer[scan_pos] += mat_coded_cost[scan_pos] - mat_uncoded_cost[scan_pos];
		}
	}
	S64 d64_cost_last_one[64] = { 0 };
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos + 1);
		U24 prev_level = mat_prev_level[scan_pos];
		U4 flag = 0;
		if (prev_level - 1 < 5)
		{
			flag = prev_level - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_one[scan_pos] = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda);
	}
	s8 rdoq_pos_1[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_pos_2[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_pos_3[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_pos_4[4] = { -1,-1,-1,-1 };
	s8 rdoq_pos_5[2] = { -1,-1 };
	s8 pos1 = 0;
	s8 pos2 = 0;
	for (int i = 0; i < 32; i++)
	{
		if (lev_opt[i * 2] && lev_opt[i * 2 + 1]) {
			if (d64_cost_last_one[i * 2] < base_cost_buffer[i * 2 + 1] + d64_cost_last_one[i * 2 + 1]) {
				rdoq_pos_1[i] = i * 2;
			}
			else {
				rdoq_pos_1[i] = i * 2 + 1;
			}
		}
		else if (lev_opt[i * 2] && !lev_opt[i * 2 + 1]) {
			rdoq_pos_1[i] = i * 2;
		}
		else if (!lev_opt[i * 2] && lev_opt[i * 2 + 1]) {
			rdoq_pos_1[i] = i * 2 + 1;
		}
	}
	for (int i = 0; i < 16; i++)
	{
		S64 base_cost_temp = 0;
		pos1 = rdoq_pos_1[i * 2];
		pos2 = rdoq_pos_1[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_2[i] = pos1;
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_2[i] = pos2;
		}
		else if (pos1 != -1 && pos2 != -1) {
			for (int j = pos2; j > pos1; j--)
			{
				base_cost_temp += base_cost_buffer[j];
			}

			if (d64_cost_last_one[pos1] < base_cost_temp + d64_cost_last_one[pos2]) {
				rdoq_pos_2[i] = pos1;
			}
			else {
				rdoq_pos_2[i] = pos2;
			}
		}

	}
	for (int i = 0; i < 8; i++)
	{
		S64 base_cost_temp = 0;
		pos1 = rdoq_pos_2[i * 2];
		pos2 = rdoq_pos_2[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_3[i] = pos1;
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_3[i] = pos2;
		}
		else if (pos1 != -1 && pos2 != -1) {
			for (int j = pos2; j > pos1; j--)
			{
				base_cost_temp += base_cost_buffer[j];
			}

			if (d64_cost_last_one[pos1] < base_cost_temp + d64_cost_last_one[pos2]) {
				rdoq_pos_3[i] = pos1;
			}
			else {
				rdoq_pos_3[i] = pos2;
			}
		}

	}
	for (int i = 0; i < 4; i++)
	{
		S64 base_cost_temp = 0;
		pos1 = rdoq_pos_3[i * 2];
		pos2 = rdoq_pos_3[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_4[i] = pos1;
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_4[i] = pos2;
		}
		else if (pos1 != -1 && pos2 != -1) {
			for (int j = pos2; j > pos1; j--)
			{
				base_cost_temp += base_cost_buffer[j];
			}

			if (d64_cost_last_one[pos1] < base_cost_temp + d64_cost_last_one[pos2]) {
				rdoq_pos_4[i] = pos1;
			}
			else {
				rdoq_pos_4[i] = pos2;
			}
		}

	}
	for (int i = 0; i < 2; i++)
	{
		S64 base_cost_temp = 0;
		pos1 = rdoq_pos_4[i * 2];
		pos2 = rdoq_pos_4[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_5[i] = pos1;
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_5[i] = pos2;
		}
		else if (pos1 != -1 && pos2 != -1) {
			for (int j = pos2; j > pos1; j--)
			{
				base_cost_temp += base_cost_buffer[j];
			}

			if (d64_cost_last_one[pos1] < base_cost_temp + d64_cost_last_one[pos2]) {
				rdoq_pos_5[i] = pos1;
			}
			else {
				rdoq_pos_5[i] = pos2;
			}
		}

	}
	S64 base_cost_temp = 0;
	pos1 = rdoq_pos_5[0];
	pos2 = rdoq_pos_5[1];
	if (pos1 != -1 && pos2 == -1) {
		*best_last_idx_p1 = pos1;
	}
	else if (pos1 == -1 && pos2 != -1) {
		*best_last_idx_p1 = pos2;
	}
	else if (pos1 != -1 && pos2 != -1) {
		for (int j = pos2; j > pos1; j--)
		{
			base_cost_temp += base_cost_buffer[j];
		}

		if (d64_cost_last_one[pos1] < base_cost_temp + d64_cost_last_one[pos2]) {
			*best_last_idx_p1 = pos1;
		}
		else {
			*best_last_idx_p1 = pos2;
		}
	}
	s64 best_pos_cost = 0;
	for (int i = 0; i <= *best_last_idx_p1; i++)
	{
		best_pos_cost += base_cost_buffer[i];
	}
	if (best_pos_cost + d64_cost_last_one[*best_last_idx_p1] > d64_best_cost) {
		*best_last_idx_p1 = 0;
	}
	else {
		*best_last_idx_p1 = *best_last_idx_p1 + 1;
	}
}

void DLNP_iter_new(int* best_last_idx_p1, U24 lev_opt[64], S64 mat_coded_cost[64], S64 mat_uncoded_cost[64], S16 tmp_dst_coef[64], int rdoq_est_cbf[3][2], U8 scan[64], U24 mat_prev_level[64], s32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda)
{
	const U1 ctx_last = (ch_type == Y_C) ? 0 : 1;
	S64 d64_cost_last_zero[64] = { 0 };
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U8 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost_base[64] = { 0 };
	S64 d64_base_cost[64] = { 0 };
	S64 base_cost_buffer[64] = { 0 };
	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		/*for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
			d64_base_cost_base[scan_pos] = 0;
		}*/
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		/*for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
			d64_base_cost_base[scan_pos] = cbf_cost;
		}*/
		d64_best_cost = d64_best_cost - cbf_cost;
	}
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=0
#pragma HLS ARRAY_PARTITION variable=d64_base_cost_base complete dim=0
#pragma HLS ARRAY_PARTITION variable=d64_base_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=base_cost_buffer complete dim=0
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos);
		U4 flag = 0;
		if (mat_prev_level[scan_pos] - 1 < 5)
		{
			flag = mat_prev_level[scan_pos] - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_zero[scan_pos] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
	}


	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (lev_opt[scan_pos])
		{
			base_cost_buffer[scan_pos] += mat_coded_cost[scan_pos] - mat_uncoded_cost[scan_pos] + d64_cost_last_zero[scan_pos];
		}
		else
		{
			base_cost_buffer[scan_pos] += mat_coded_cost[scan_pos] - mat_uncoded_cost[scan_pos];
		}
	}
	S64 d64_cost_last_one[64] = { 0 };
#pragma HLS   ARRAY_PARTITION variable=d64_cost_last_one complete dim=0
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos + 1);
		U24 prev_level = mat_prev_level[scan_pos];
		U4 flag = 0;
		if (prev_level - 1 < 5)
		{
			flag = prev_level - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_one[scan_pos] = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda);
	}
	s8 rdoq_pos_1[32] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_pos_2[16] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_pos_3[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_pos_4[4] = { -1,-1,-1,-1 };
	s8 rdoq_pos_5[2] = { -1,-1 };
	s64 pre_cost_1[32] = { 0 };
	s64 pre_cost_2[16] = { 0 };
	s64 pre_cost_3[8] = { 0 };
	s64 pre_cost_4[4] = { 0 };
	s64 pre_cost_5[2] = { 0 };
	s64 latt_cost_1[32] = { 0 };
	s64 latt_cost_2[16] = { 0 };
	s64 latt_cost_3[8] = { 0 };
	s64 latt_cost_4[4] = { 0 };
	s64 latt_cost_5[2] = { 0 };
	s8 pos1 = 0;
	s8 pos2 = 0;
	for (int i = 0; i < 32; i++)
	{
#pragma HLS UNROLL
		if (lev_opt[i * 2] && lev_opt[i * 2 + 1]) {
			if (d64_cost_last_one[i * 2] < base_cost_buffer[i * 2 + 1] + d64_cost_last_one[i * 2 + 1]) {
				rdoq_pos_1[i] = i * 2;
			}
			else {
				rdoq_pos_1[i] = i * 2 + 1;
			}
		}
		else if (lev_opt[i * 2] && !lev_opt[i * 2 + 1]) {
			rdoq_pos_1[i] = i * 2;
		}
		else if (!lev_opt[i * 2] && lev_opt[i * 2 + 1]) {
			rdoq_pos_1[i] = i * 2 + 1;
		}
		if (rdoq_pos_1[i] == -1) {

			pre_cost_1[i] = base_cost_buffer[i * 2] + base_cost_buffer[i * 2 + 1];
		}
		else if (rdoq_pos_1[i] % 2 == 1) {

			pre_cost_1[i] = base_cost_buffer[i * 2] + base_cost_buffer[i * 2 + 1];
			latt_cost_1[i] = 0;

		}
		else {
			pre_cost_1[i] = base_cost_buffer[i * 2];
			latt_cost_1[i] = base_cost_buffer[i * 2 + 1];
		}
	}
	for (int i = 0; i < 16; i++)
	{
#pragma HLS UNROLL
		pos1 = rdoq_pos_1[i * 2];
		pos2 = rdoq_pos_1[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_2[i] = pos1;
			pre_cost_2[i] = pre_cost_1[i * 2];
			latt_cost_2[i] = latt_cost_1[i * 2] + latt_cost_1[i * 2 + 1] + pre_cost_1[i * 2 + 1];
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_2[i] = pos2;
			pre_cost_2[i] = pre_cost_1[i * 2 + 1] + pre_cost_1[i * 2] + latt_cost_1[i * 2];
			latt_cost_2[i] = latt_cost_1[i * 2 + 1];
		}
		else if (pos1 != -1 && pos2 != -1) {
			if (d64_cost_last_one[pos1] < pre_cost_1[i * 2 + 1] + latt_cost_1[i * 2] + d64_cost_last_one[pos2])
			{
				rdoq_pos_2[i] = pos1;
				pre_cost_2[i] = pre_cost_1[i * 2];
				latt_cost_2[i] = latt_cost_1[i * 2] + latt_cost_1[i * 2 + 1] + pre_cost_1[i * 2 + 1];
			}
			else {
				rdoq_pos_2[i] = pos2;
				pre_cost_2[i] = pre_cost_1[i * 2 + 1] + pre_cost_1[i * 2] + latt_cost_1[i * 2];
				latt_cost_2[i] = latt_cost_1[i * 2 + 1];
			}
		}
		else {
			pre_cost_2[i] = pre_cost_1[i * 2] + pre_cost_1[i * 2 + 1];
		}


	}
	for (int i = 0; i < 8; i++)
	{
#pragma HLS UNROLL
		pos1 = rdoq_pos_2[i * 2];
		pos2 = rdoq_pos_2[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_3[i] = pos1;
			pre_cost_3[i] = pre_cost_2[i * 2];
			latt_cost_3[i] = latt_cost_2[i * 2] + latt_cost_2[i * 2 + 1] + pre_cost_2[i * 2 + 1];
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_3[i] = pos2;
			pre_cost_3[i] = pre_cost_2[i * 2 + 1] + pre_cost_2[i * 2] + latt_cost_2[i * 2];
			latt_cost_3[i] = latt_cost_2[i * 2 + 1];
		}
		else if (pos1 != -1 && pos2 != -1) {
			if (d64_cost_last_one[pos1] < pre_cost_2[i * 2 + 1] + latt_cost_2[i * 2] + d64_cost_last_one[pos2]) {
				rdoq_pos_3[i] = pos1;
				pre_cost_3[i] = pre_cost_2[i * 2];
				latt_cost_3[i] = latt_cost_2[i * 2] + latt_cost_2[i * 2 + 1] + pre_cost_2[i * 2 + 1];
			}
			else {
				rdoq_pos_3[i] = pos2;
				pre_cost_3[i] = pre_cost_2[i * 2 + 1] + pre_cost_2[i * 2] + latt_cost_2[i * 2];
				latt_cost_3[i] = latt_cost_2[i * 2 + 1];
			}
		}
		else {
			pre_cost_3[i] = pre_cost_2[i * 2] + pre_cost_2[i * 2 + 1];
		}

	}
	for (int i = 0; i < 4; i++)
	{
#pragma HLS UNROLL
		pos1 = rdoq_pos_3[i * 2];
		pos2 = rdoq_pos_3[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_4[i] = pos1;
			pre_cost_4[i] = pre_cost_3[i * 2];
			latt_cost_4[i] = latt_cost_3[i * 2] + latt_cost_3[i * 2 + 1] + pre_cost_3[i * 2 + 1];
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_4[i] = pos2;
			pre_cost_4[i] = pre_cost_3[i * 2 + 1] + pre_cost_3[i * 2] + latt_cost_3[i * 2];
			latt_cost_4[i] = latt_cost_3[i * 2 + 1];
		}
		else if (pos1 != -1 && pos2 != -1) {
			if (d64_cost_last_one[pos1] < pre_cost_3[i * 2 + 1] + latt_cost_3[i * 2] + d64_cost_last_one[pos2]) {
				rdoq_pos_4[i] = pos1;
				pre_cost_4[i] = pre_cost_3[i * 2];
				latt_cost_4[i] = latt_cost_3[i * 2] + latt_cost_3[i * 2 + 1] + pre_cost_3[i * 2 + 1];
			}
			else {
				rdoq_pos_4[i] = pos2;
				pre_cost_4[i] = pre_cost_3[i * 2 + 1] + pre_cost_3[i * 2] + latt_cost_3[i * 2];
				latt_cost_4[i] = latt_cost_3[i * 2 + 1];
			}
		}
		else {
			pre_cost_4[i] = pre_cost_3[i * 2] + pre_cost_3[i * 2 + 1];
		}

	}
	for (int i = 0; i < 2; i++)
	{
#pragma HLS UNROLL
		pos1 = rdoq_pos_4[i * 2];
		pos2 = rdoq_pos_4[i * 2 + 1];
		if (pos1 != -1 && pos2 == -1) {
			rdoq_pos_5[i] = pos1;
			pre_cost_5[i] = pre_cost_4[i * 2];
			latt_cost_5[i] = latt_cost_4[i * 2] + latt_cost_4[i * 2 + 1] + pre_cost_4[i * 2 + 1];
		}
		else if (pos1 == -1 && pos2 != -1) {
			rdoq_pos_5[i] = pos2;
			pre_cost_5[i] = pre_cost_4[i * 2 + 1] + pre_cost_4[i * 2] + latt_cost_4[i * 2];
			latt_cost_5[i] = latt_cost_4[i * 2 + 1];
		}
		else if (pos1 != -1 && pos2 != -1) {
			if (d64_cost_last_one[pos1] < pre_cost_4[i * 2 + 1] + latt_cost_4[i * 2] + d64_cost_last_one[pos2]) {
				rdoq_pos_5[i] = pos1;
				pre_cost_5[i] = pre_cost_4[i * 2];
				latt_cost_5[i] = latt_cost_4[i * 2] + latt_cost_4[i * 2 + 1] + pre_cost_4[i * 2 + 1];
			}
			else {
				rdoq_pos_5[i] = pos2;
				pre_cost_5[i] = pre_cost_4[i * 2 + 1] + pre_cost_4[i * 2] + latt_cost_4[i * 2];
				latt_cost_5[i] = latt_cost_4[i * 2 + 1];
			}
		}
		else {
			pre_cost_5[i] = pre_cost_4[i * 2] + pre_cost_4[i * 2 + 1];
		}

	}
	pos1 = rdoq_pos_5[0];
	pos2 = rdoq_pos_5[1];
	if (pos1 != -1 && pos2 == -1) {
		*best_last_idx_p1 = pos1;
	}
	else if (pos1 == -1 && pos2 != -1) {
		*best_last_idx_p1 = pos2;
	}
	else if (pos1 != -1 && pos2 != -1) {
		if (d64_cost_last_one[pos1] < pre_cost_5[1] + latt_cost_5[0] + d64_cost_last_one[pos2]) {
			*best_last_idx_p1 = pos1;
		}
		else {
			*best_last_idx_p1 = pos2;
		}
	}
	s64 best_pos_cost = 0;
	//for (int i = 0; i <= *best_last_idx_p1; i++)
	for (int i = 0; i < 64; i++)
	{
#pragma HLS  loop_tripcount max=64
#pragma HLS UNROLL
		if (i <= *best_last_idx_p1)
		best_pos_cost += base_cost_buffer[i];
	}
	if (best_pos_cost + d64_cost_last_one[*best_last_idx_p1] > d64_best_cost) {
		*best_last_idx_p1 = 0;
	}
	else {
		*best_last_idx_p1 = *best_last_idx_p1 + 1;
	}
}


void DLNP(int* best_last_idx_p1, U24 lev_opt[64], S64 mat_coded_cost[64], S64 mat_uncoded_cost[64], S16 tmp_dst_coef[64], S32 rdoq_est_cbf[3][2], U8 scan[64], U24 mat_prev_level[64], S32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda)
{
	const U1 ctx_last = (ch_type == Y_C) ? 0 : 1;
	S64 d64_cost_last_zero[64] = { 0 };
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U8 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost_base[64] = { 0 };
	S64 d64_base_cost[64] = { 0 };
	S64 base_cost_buffer[64] = { 0 };

	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
			d64_base_cost_base[scan_pos] = 0;
		}
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		d64_best_cost = d64_best_cost >> 24;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		cbf_cost = cbf_cost >> 24;
		for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
			d64_base_cost_base[scan_pos] = cbf_cost;
		}
	}

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{

		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos);
		U4 flag = 0;
		if (mat_prev_level[scan_pos] - 1 < 5)
		{
			flag = mat_prev_level[scan_pos] - 1;
		}
		else
		{
			flag = 5;
		}
		d64_cost_last_zero[scan_pos] = rdoq_est_last_local[ctx_last][flag][ace_log2][0] * lambda;
		d64_cost_last_zero[scan_pos] = d64_cost_last_zero[scan_pos] >> 24;
	}


	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		for (U8 i = 0; i < 64; i++)
		{
			if (i <= scan_pos)
			{
				if (lev_opt[i])
				{
					base_cost_buffer[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
				}
				else
				{
					base_cost_buffer[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i];
				}
			}
		}
	}


	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		d64_base_cost[scan_pos] = base_cost_buffer[scan_pos] + d64_base_cost_base[scan_pos];
	}


	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		ace_log2 = get_ace_log2(scan_pos + 1);
		U24 prev_level = mat_prev_level[scan_pos];
		U4 flag = 0;
		if (prev_level - 1 < 5)
		{
			flag = prev_level - 1;
		}
		else
		{
			flag = 5;
		}
		S64 d64_cost_last_one = GET_I_COST(rdoq_est_last_local[ctx_last][flag][ace_log2][1], lambda);
		d64_cost_last_one = d64_cost_last_one >> 24;
		if (tmp_dst_coef[blk_pos])
		{
			S64 d64_cur_is_last_cost = d64_base_cost[scan_pos] + d64_cost_last_one;

			if (d64_cur_is_last_cost < d64_best_cost)
			{
				d64_best_cost = d64_cur_is_last_cost;
				*best_last_idx_p1 = scan_pos + 1;
			}
		}
	}
}

void CleanNz(int* num_nz_coef, U8 scan[64], int best_last_idx_p1, s16 dst_tmp[8][8], S16 tmp_dst_coef[64])
{
	U1 mask[8][8] = { 0 };


	for (int scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U32 blk_pos = scan[scan_pos];
		U8 x = blk_pos & (8 - 1);
		U8 y = blk_pos >> 3;


		if (scan_pos < best_last_idx_p1)
			mask[y][x] = 1;
		else
			mask[y][x] = 0;


		dst_tmp[y][x] = tmp_dst_coef[blk_pos] * mask[y][x];
		*num_nz_coef += !!(dst_tmp[y][x]);
	}

}

static U4 get_ace_log2_1(U8 blk_pos)
{
	U8 ace_log2_table[64] = { 0,0,2,2,3,3,4,4,
							  1,2,2,3,4,4,4,5,
							  1,3,3,4,4,4,5,5,
							  3,3,4,4,4,5,5,5,
							  3,4,4,5,5,5,5,5,
							  4,4,5,5,5,5,5,5,
							  4,5,5,5,5,5,5,5,
							  5,5,5,5,5,5,5,5 };
	U4 ace_log2 = ace_log2_table[blk_pos];
	return ace_log2;
}

static U4 get_ace_log2_2(U8 blk_pos)
{
	U8 ace_log2_table[64] = { 0,1,2,2,3,4,4,4,
							  1,2,3,3,4,4,4,5,
							  2,3,3,4,4,4,5,5,
							  3,3,4,4,5,5,5,5,
							  3,4,4,5,5,5,5,5,
							  4,4,5,5,5,5,5,5,
							  4,5,5,5,5,5,5,5,
							  5,5,5,5,5,5,5,6 };
	U4 ace_log2 = ace_log2_table[blk_pos];

	return ace_log2;
}


void rdoq_8_8_hardware(S16 tmp_dst_coef_out[64], U32 last1[12], U32 last2[22], U32 run[24], U32 level[24], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2], S64 err_scale, S16 src_coef[8][8], U6 qp, S32 q_bits, int* best_last_idx_p1, S32 rdoq_est_cbf[3][2], S32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda)
{
	U3 cu_width_log2 = 3;
	U3 cu_height_log2 = 3;
	U16 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
	U36 tmp_level_double[8];
	S16 tmp_coef[8];
	const U16 scale = quant_scale[qp];
	const U4 ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const U8 ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const U8 ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const U20 q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	S16 tmp = 0;
	//mat_prev_level[0] = 6;
	U8 scan[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

	const U1 ctx_last = (ch_type == Y_C) ? 0 : 1;
	S64 d64_cost_last_zero[8] = { 0 };

	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U8 scan_pos = 0;
	S64 d64_best_cost = 0;
	// added by xfhuang.
	S64 base_cost_buffer_tmp[8] = { 0 };
	S64 d64_best_cost_tmp = 0;
	int best_last_idx_p1_tmp = 0;
	S64 mat_coded_cost[8] = { 0 };
	S64 mat_uncoded_cost[8] = { 0 };
	U24 lev_opt[8] = { 0 };
	U24 mat_prev_level[8] = { 0 };
	U1 mat_run[8] = { 0 };

	S16 tmp_dst_coef[8] = { 0 };

	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		d64_best_cost_tmp = 0;
	}
	else
	{
		d64_best_cost = rdoq_est_cbf[ch_type][0] * lambda;
		S64 cbf_cost = rdoq_est_cbf[ch_type][1] * lambda;
		// optimize: d64bestCost (cbf=0 cost) compare with (base_cost_buffer[scan_pos] + cbf=1 cost)
		// is same: (cbf=0 cost) - (cbf=1 cost) compare with (base_cost_buffer[scan_pos])
		d64_best_cost = d64_best_cost - cbf_cost;
		d64_best_cost_tmp = d64_best_cost;
	}


	// added by xfhuang 
	S64 d64_cost_last_one_tmp[8] = { 0 };

#if 1

	S64 finalRdoqCost = 0;
	S64 tempCost[8];   // all scan line cost
	S64 endPosCost[8]; // as the last position cost.
	S64 rdoqD64LastOne[8] = { -1,-1,-1,-1,-1,-1,-1,-1 }; // shifter buffer the d64_cost_last_one_tmp
	int last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int rdoq_last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int rdoq_last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	int final_last_x = -1, final_last_y = -1;
	int final_rdoq_last_x = -1, final_rdoq_last_y = -1;

	//model_update
	//U7 scan_pos = 0;
	U5 t0[8] = { 0 };
	U4 last1_pos[8] = { 0 };
	U5 last2_pos[8] = { 0 };

	for (int j = 0; j < 8; j++) // horizontal
	{
		U32 last1_tmp[12] = { 0 };
		U32 last2_tmp[22] = { 0 };
		U32 level_tmp[24] = { 0 };
		U32 level_tmp1[24] = { 0 };
		U32 run_tmp[24] = { 0 };
		U32 run_tmp1[24] = { 0 };
		//pre_quant
		for (int i = 0; i < 8; i++)
		{

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
		//doc
		for (U8 i = 0; i < 8; i++)
		{
			//U8 blk_pos = scan[scan_pos];

			if (tmp_coef[i])
			{
				mat_prev_level[i] = abs(tmp_coef[i]);
				tmp = abs(tmp_coef[i]);
				mat_run[i] = 0;
			}
			else
			{
				mat_prev_level[i] = 1;
				mat_run[i] = 1;
			}
		}

		for (U8 i = 0; i < 8; i++)
		{

			//U8 blk_pos = scan[scan_pos];
			U24 level;
			U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);

			int iflast = (i * 8 + j == 63);
			level = get_coded_level_rl_demo(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
				mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, iflast, rdoq_est_run_local, rdoq_est_level_local);
			S17 level_cbf = level;
			tmp_dst_coef[i] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
			tmp_dst_coef_out[i * 8 + j] = tmp_dst_coef[i];
			mat_uncoded_cost[i] = d64_uncoded_cost;
			mat_coded_cost[i] = d64_coded_cost;
			lev_opt[i] = level;
		}

		// model update
		for (int i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			ace_log2 = get_ace_log2_2(i * 8 + j);
			t0[i] = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == 0 ? 0 : 12);
			last1_pos[i] = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5)))) + (ch_type == 0 ? 0 : 6);
			last2_pos[i] = ace_log2 + (ch_type == 0 ? 0 : 12);
		}

		for (scan_pos = 0; scan_pos < 24; scan_pos += 2)
		{
#pragma HLS UNROLL
			level_core_8(8, 8, level_tmp, level_tmp1, t0, scan_pos, 8);
			run_core_8(8, 8, run_tmp, run_tmp1, t0, scan_pos, mat_run, 8);
		}

		for (scan_pos = 0; scan_pos < 12; scan_pos++)
		{
#pragma HLS UNROLL
			last1_core_8(8, 8, last1_tmp, last1_pos, scan_pos, 8);
			last1[scan_pos] += last1_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 22; scan_pos++)
		{
#pragma HLS UNROLL
			last2_core_8(8, 8, last2_tmp, last2_pos, scan_pos, 8);
			last2[scan_pos] += last2_tmp[scan_pos];
		}

		for (scan_pos = 0; scan_pos < 24; scan_pos++)
		{
#pragma HLS UNROLL
			level[scan_pos] += level_tmp[scan_pos] + level_tmp1[scan_pos];
			run[scan_pos] += run_tmp[scan_pos] + run_tmp1[scan_pos];
		}

		//dlnp prepare
		for (int i = 0; i < 8; i++)
		{
			//U8 blk_pos = scan[scan_pos];
			ace_log2 = get_ace_log2_1(i * 8 + j);
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

		for (int i = 0; i < 8; i++)
		{
			//int blk_pos = scan[scan_pos];
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

		for (int i = 0; i < 8; i++)
		{
			//U8 blk_pos = scan[scan_pos];
			ace_log2 = get_ace_log2_2(i * 8 + j);
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
		//dlnp core
		int zzz = 0;
		for (int i = 0; i < 8; i++) // vertical
		{
			if (j == 0)
			{
				// tempCost[j] = base_cost_buffer_tmp[j];
				if (lev_opt[i])
				{
					last_x[i] = i;
					last_y[i] = j;

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
					//if (j == 0)  // substract here is to get the actual endPosCost
					//{
					//	endPosCost[j] = tempCost[j] - endPosCost[j];
					//}
				}
			}
			else if (i == 7)
			{
				last_x[i] = -1;
				last_y[i] = -1;

				rdoq_last_x[i] = -1;
				rdoq_last_y[i] = -1;
				rdoqD64LastOne[i] = -1;
				if (lev_opt[i])
				{
					last_x[i] = i;
					last_y[i] = j;

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
					if (lev_opt[i])
					{
						last_x[i] = i;
						last_y[i] = j;
					}

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

					if (lev_opt[i] && last_x[i] == -1) // first non-zero position
					{
						last_x[i] = i;
						last_y[i] = j;
					}

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
		if (last_y[0] != -1)
		{
			final_last_x = last_x[0];
			final_last_y = last_y[0];
		}

		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];

		//int index = rdoq_last_x[0] * 8 + rdoq_last_y[0];


		if (rdoq_last_x[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];

		// shift register.
		for (int i = 1; i < 8; i++)
		{
			last_x[i - 1] = last_x[i];
			last_y[i - 1] = last_y[i];
			rdoq_last_x[i - 1] = rdoq_last_x[i];
			rdoq_last_y[i - 1] = rdoq_last_y[i];
			tempCost[i - 1] = tempCost[i];
			endPosCost[i - 1] = endPosCost[i];
			rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
		}




	}

	///////////////////////////////////////////
	for (int i = 0; i < 8; i++)
	{
		if (last_x[i] != -1)
		{
			final_last_x = last_x[i];
			final_last_y = last_y[i];
		}
	}
	if (final_last_x == -1) {
		final_last_x = 0;
		final_last_y = 0;
	}

	// decide for remaining scan lines 
	// not 8; only need iterate 7 times 
	for (int i = 0; i < 7; i++)
	{
		if ((rdoq_last_x[i] + rdoq_last_y[i]) % 2 == 1) // above right scan.
		{
			endPosCost[i] = tempCost[i] - endPosCost[i];
		}
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[i];
		int index = rdoq_last_x[i] * 8 + rdoq_last_y[i];
		if (rdoq_last_x[i] != -1 && temp_RdoqCost + rdoqD64LastOne[i] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[i];
			final_rdoq_last_x = rdoq_last_x[i];
			final_rdoq_last_y = rdoq_last_y[i];

		}
		finalRdoqCost += tempCost[i];
	}

	// back to zig-zag scan order this can be optimized later.
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
		U8 blk_pos = scan[scan_pos];
		if (blk_pos == final_rdoq_last_x * 8 + final_rdoq_last_y)
		{
			best_last_idx_p1_tmp = scan_pos + 1;
		}
	}
#endif

	*best_last_idx_p1 = best_last_idx_p1_tmp;
	int tmp_sum = 0;
}


int rdoq_top_8_8(S64 err_scale, RDOQ_MODEL* model_cnt, U6 qp, S64 lambda, U1 is_intra, s16 src_coef[8][8], s16 dst_tmp[8][8], U3 cu_width_log2, 
	U32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=model_cnt->last2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt->last1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt->level complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt->run_rdoq complete dim=0
#pragma HLS ARRAY_PARTITION variable=model_cnt->cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=dst_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_coef complete dim=0

	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift1(bit_depth, log2_size);

	const U8 w = 1 << cu_width_log2;
	const U8 h = 1 << cu_height_log2;
	const U14 max_num_coef = 64;

	U8 scan[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

	const int q_bits = 14 + tr_shift;
	int num_nz_coef = 0;
	int best_last_idx_p1 = 0;
	S16 tmp_coef[64] = { 0 };
	U36 tmp_level_double[64] = { 0 };
	S16 tmp_dst_coef[64] = { 0 };

	U24 mat_prev_level[64] = { 0 };
	U1 mat_run[64] = { 0 };
	S64 mat_uncoded_cost[64] = { 0 };
	S64 mat_coded_cost[64] = { 0 };
	U24 lev_opt[64] = { 0 };


	/* ===== quantization ===== */
#if 0
	rdoq_8_8_hardware(tmp_dst_coef, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, err_scale, src_coef, qp, q_bits, &best_last_idx_p1, rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_last, ch_type, is_intra, lambda);
#else
	pre_quant(scan, src_coef, qp, q_bits, tmp_level_double, tmp_coef);

	doc(rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, lev_opt, mat_uncoded_cost, mat_coded_cost, tmp_dst_coef, tmp_level_double, scan, tmp_coef, mat_prev_level, mat_run, ch_type, q_bits, err_scale, lambda);

	DLNP_iter_new(&best_last_idx_p1, lev_opt, mat_coded_cost, mat_uncoded_cost, tmp_dst_coef, rdoq_array->rdoq_est_cbf, scan, mat_prev_level, rdoq_array->rdoq_est_last, ch_type, is_intra, lambda);

	model_update_8(w, h, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, max_num_coef);
#endif
	CleanNz(&num_nz_coef, scan, best_last_idx_p1, dst_tmp, tmp_dst_coef);

	return num_nz_coef;
}
int rdoq_top_8_8_luma(S64 err_scale, U6 qp, S64 lambda, U1 is_intra, s16 src_coef[8][8], s16 dst_tmp[8][8], U3 cu_width_log2,
	U32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable=dst_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable= rdoq_array->rdoq_est_last complete dim=0
#pragma HLS ARRAY_PARTITION variable= rdoq_array->rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable= rdoq_array->rdoq_est_level complete dim=0
#pragma HLS ARRAY_PARTITION variable= rdoq_array->rdoq_est_run complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_coef complete dim=0

	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift1(bit_depth, log2_size);

	const U8 w = 1 << cu_width_log2;
	const U8 h = 1 << cu_height_log2;
	const U14 max_num_coef = 64;

	U8 scan[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

	const int q_bits = 14 + tr_shift;
	int num_nz_coef = 0;
	int best_last_idx_p1 = 0;
	S16 tmp_coef[64] = { 0 };
	U36 tmp_level_double[64] = { 0 };
	S16 tmp_dst_coef[64] = { 0 };

	U24 mat_prev_level[64] = { 0 };
	U1 mat_run[64] = { 0 };
	S64 mat_uncoded_cost[64] = { 0 };
	S64 mat_coded_cost[64] = { 0 };
	U24 lev_opt[64] = { 0 };


	/* ===== quantization ===== */
#
	pre_quant(scan, src_coef, qp, q_bits, tmp_level_double, tmp_coef);

	doc(rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, lev_opt, mat_uncoded_cost, mat_coded_cost, tmp_dst_coef, tmp_level_double, scan, tmp_coef, mat_prev_level, mat_run, ch_type, q_bits, err_scale, lambda);

	DLNP_iter_new(&best_last_idx_p1, lev_opt, mat_coded_cost, mat_uncoded_cost, tmp_dst_coef, rdoq_array->rdoq_est_cbf, scan, mat_prev_level, rdoq_array->rdoq_est_last, ch_type, is_intra, lambda);

//	model_update_8_luma(w, h, mat_run, mat_prev_level, max_num_coef, sum_run, last1_tmp, last2_tmp);

	CleanNz(&num_nz_coef, scan, best_last_idx_p1, dst_tmp, tmp_dst_coef);

	return num_nz_coef;
}
int rdoq_top_8_8_chroma(S64 err_scale, U6 qp, S64 lambda, U1 is_intra, s16 src_coef[8][8], s16 dst_tmp[8][8], U3 cu_width_log2,
	U32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{
#pragma HLS PIPELINE II=1
//#pragma HLS ARRAY_PARTITION variable=model_cnt->last2 complete dim=0
//#pragma HLS ARRAY_PARTITION variable=model_cnt->last1 complete dim=0
//#pragma HLS ARRAY_PARTITION variable=model_cnt->level complete dim=0
//#pragma HLS ARRAY_PARTITION variable=model_cnt->run_rdoq complete dim=0
//#pragma HLS ARRAY_PARTITION variable=model_cnt->cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=dst_tmp complete dim=0
//#pragma HLS ARRAY_PARTITION variable=rdoq_est_last_local complete dim=0
//#pragma HLS ARRAY_PARTITION variable=rdoq_est_cbf complete dim=0
//#pragma HLS ARRAY_PARTITION variable=rdoq_est_level_local complete dim=0
//#pragma HLS ARRAY_PARTITION variable=rdoq_est_run_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_coef complete dim=0

	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift1(bit_depth, log2_size);

	const U8 w = 1 << cu_width_log2;
	const U8 h = 1 << cu_height_log2;
	const U14 max_num_coef = 64;

	U8 scan[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

	const int q_bits = 14 + tr_shift;
	int num_nz_coef = 0;
	int best_last_idx_p1 = 0;
	S16 tmp_coef[64] = { 0 };
	U36 tmp_level_double[64] = { 0 };
	S16 tmp_dst_coef[64] = { 0 };

	U24 mat_prev_level[64] = { 0 };
	U1 mat_run[64] = { 0 };
	S64 mat_uncoded_cost[64] = { 0 };
	S64 mat_coded_cost[64] = { 0 };
	U24 lev_opt[64] = { 0 };


	/* ===== quantization ===== */
#if 0
	rdoq_8_8_hardware(tmp_dst_coef, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, err_scale, src_coef, qp, q_bits, &best_last_idx_p1, rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_last, ch_type, is_intra, lambda);
#else
	pre_quant(scan, src_coef, qp, q_bits, tmp_level_double, tmp_coef);

	doc(rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, lev_opt, mat_uncoded_cost, mat_coded_cost, tmp_dst_coef, tmp_level_double, scan, tmp_coef, mat_prev_level, mat_run, ch_type, q_bits, err_scale, lambda);

	DLNP_iter_new(&best_last_idx_p1, lev_opt, mat_coded_cost, mat_uncoded_cost, tmp_dst_coef, rdoq_array->rdoq_est_cbf, scan, mat_prev_level, rdoq_array->rdoq_est_last, ch_type, is_intra, lambda);

	//model_update_8_chroma(w, h, mat_run, mat_prev_level, max_num_coef, sum_run, last1_tmp, last2_tmp);
#endif
	CleanNz(&num_nz_coef, scan, best_last_idx_p1, dst_tmp, tmp_dst_coef);

	return num_nz_coef;
}
