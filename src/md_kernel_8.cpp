#include "md_kernel_8.h"
#include "iime.h"
#include "md_dummy.h"
#include "md_cmod.h"

#pragma warning(disable:4102)
#pragma warning(disable:4334)
#pragma warning(disable:4018)
#pragma warning(disable:6297)
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
#define GET_I_COST(rate, lamba)  (rate*lamba)


template<typename T, unsigned size>
void memcpy_1d(T src[size], T dst[size]) {
#pragma HLS ARRAY_PARTITION variable=dst complete dim=0
#pragma HLS ARRAY_PARTITION variable=src complete dim=0
	for (int i = 0; i < size; i++)
#pragma HLS UNROLL
		dst[i] = src[i];
}
template<typename T, unsigned d1, unsigned d2>
void memcpy_2d(T src[d1][d2], T dst[d1][d2]) {
#pragma HLS ARRAY_PARTITION variable=dst complete dim=0
#pragma HLS ARRAY_PARTITION variable=src complete dim=0
	for (int i = 0; i < d1; i++)
#pragma HLS UNROLL
		for (int j = 0; j < d2; j++)
#pragma HLS UNROLL
			dst[i][j] = src[i][j];
}


void dct_row_8(U8 w, U8 h, s16 src[8][8], s16 dst[8][8], U4 shift)
{
	U8 size = 8;
	U8 size_b = 7;

	U8 j, k;
	static int E[8][4], O[8][4];
	static int EE[8][2], EO[8][2];
	int tmp[8];
	int add;
	if (w == 4)
		add = shift == 0 ? 0 : 1 << (shift - 1);
	else if (w == 8)
		add = 1 << (shift - 1);

	for (j = 0; j < 8; j++)
	{
#pragma HLS UNROLL
		if (j >= h)
			break;

		/* E and O*/
		for (k = 0; k < 4; k++)
		{
#pragma HLS UNROLL
			E[j][k] = src[j][k] + src[j][7 - k];
			O[j][k] = src[j][k] - src[j][7 - k];
		}
		/* EE and EO */
		if (w == 8) {
			EE[j][0] = E[j][0] + E[j][3];
			EO[j][0] = E[j][0] - E[j][3];
			EE[j][1] = E[j][1] + E[j][2];
			EO[j][1] = E[j][1] - E[j][2];
		}
		else if (w == 4)
		{
			EE[j][0] = src[j][0] + src[j][3];
			EO[j][0] = src[j][0] - src[j][3];
			EE[j][1] = src[j][1] + src[j][2];
			EO[j][1] = src[j][1] - src[j][2];
		}
		tmp[0] = ((32 * EE[j][0] + 32 * EE[j][1] + add) >> shift);
		tmp[1] = ((32 * EE[j][0] - 32 * EE[j][1] + add) >> shift);
		tmp[2] = ((42 * EO[j][0] + 17 * EO[j][1] + add) >> shift);
		tmp[3] = ((17 * EO[j][0] - 42 * EO[j][1] + add) >> shift);
		tmp[4] = ((44 * O[j][0] + 38 * O[j][1] + 25 * O[j][2] + 9 * O[j][3] + add) >> shift);
		tmp[5] = ((38 * O[j][0] - 9 * O[j][1] - 44 * O[j][2] - 25 * O[j][3] + add) >> shift);
		tmp[6] = ((25 * O[j][0] - 44 * O[j][1] + 9 * O[j][2] + 38 * O[j][3] + add) >> shift);
		tmp[7] = ((9 * O[j][0] - 25 * O[j][1] + 38 * O[j][2] - 44 * O[j][3] + add) >> shift);

		if (w == 4) {
			dst[0][(j + 0) & size_b] = tmp[0];
			dst[2][(j + 2) & size_b] = tmp[1];
			dst[1][(j + 1) & size_b] = tmp[2];
			dst[3][(j + 3) & size_b] = tmp[3];
		}
		else if (w == 8) {
			dst[0][(j + 0) & size_b] = tmp[0];
			dst[4][(j + 4) & size_b] = tmp[1];
			dst[2][(j + 2) & size_b] = tmp[2];
			dst[6][(j + 6) & size_b] = tmp[3];
			dst[1][(j + 1) & size_b] = tmp[4];
			dst[3][(j + 3) & size_b] = tmp[5];
			dst[5][(j + 5) & size_b] = tmp[6];
			dst[7][(j + 7) & size_b] = tmp[7];
		}
	}
}


void dct_col_8(U8 w, U8 h, s16 src[8][8], s16 dst[8][8], U4 shift)
{
#pragma HLS PIPELINE II=1
	U8 size = 8;
	U8 size_b = 7;

	U8 j, k;
	static int E[8][4], O[8][4];
	static int EE[8][2], EO[8][2];
	int tmp[8];
	int add;

	if (h == 4)
		add = shift == 0 ? 0 : 1 << (shift - 1);
	else if (h == 8)
	    add = 1 << (shift - 1);
	for (j = 0; j < 8; j++)
	{
#pragma HLS UNROLL
		if (j >= w)
			break;

		/* E and O*/
		for (k = 0; k < 4; k++)
		{
			E[j][k] = src[j][(k + j) & size_b] + src[j][(7 - k + j) & size_b];
			O[j][k] = src[j][(k + j) & size_b] - src[j][(7 - k + j) & size_b];
		}
		/* EE and EO */
		if (h == 8) {
			EE[j][0] = E[j][0] + E[j][3];
			EO[j][0] = E[j][0] - E[j][3];
			EE[j][1] = E[j][1] + E[j][2];
			EO[j][1] = E[j][1] - E[j][2];
		}
		else if (h == 4) {
			EE[j][0] = src[j][(0 + j) & size_b] + src[j][(3 + j) & size_b];
			EO[j][0] = src[j][(0 + j) & size_b] - src[j][(3 + j) & size_b];
			EE[j][1] = src[j][(1 + j) & size_b] + src[j][(2 + j) & size_b];
			EO[j][1] = src[j][(1 + j) & size_b] - src[j][(2 + j) & size_b];
		}
			tmp[0] = ((32 * EE[j][0] + 32 * EE[j][1] + add) >> shift);
			tmp[1] = ((32 * EE[j][0] - 32 * EE[j][1] + add) >> shift);
			tmp[2] = ((42 * EO[j][0] + 17 * EO[j][1] + add) >> shift);
			tmp[3] = ((17 * EO[j][0] - 42 * EO[j][1] + add) >> shift);
			tmp[4] = ((44 * O[j][0] + 38 * O[j][1] + 25 * O[j][2] + 9 * O[j][3] + add) >> shift);
			tmp[5] = ((38 * O[j][0] - 9 * O[j][1] - 44 * O[j][2] - 25 * O[j][3] + add) >> shift);
			tmp[6] = ((25 * O[j][0] - 44 * O[j][1] + 9 * O[j][2] + 38 * O[j][3] + add) >> shift);
			tmp[7] = ((9 * O[j][0] - 25 * O[j][1] + 38 * O[j][2] - 44 * O[j][3] + add) >> shift);
		
		if (h == 4) {
			dst[0][j] = tmp[0];
			dst[2][j] = tmp[1];
			dst[1][j] = tmp[2];
			dst[3][j] = tmp[3];
		}
		else if (h == 8) {
			dst[0][j] = tmp[0];
			dst[4][j] = tmp[1];
			dst[2][j] = tmp[2];
			dst[6][j] = tmp[3];
			dst[1][j] = tmp[4];
			dst[3][j] = tmp[5];
			dst[5][j] = tmp[6];
			dst[7][j] = tmp[7];
		}	
	}
}

void inv_dct_col_8(U8 w, U8 h, s16 coeff[8][8], s16 block[8][8], U4 shift)
{

	U7 size = 8;
	if (h == 4){
		int E[2], O[2];
		int rnd_factor = 1 << (shift - 1);

		inv_dct_col_8_label2:for (int i = 0; i < 8; i++){
#pragma HLS UNROLL
			if (i>=w) break;
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
	else if (h == 8){
		int E[4], O[4];
		int EE[2], EO[2];
		int rnd_factor = 1 << (shift - 1);
		int k;
inv_dct_col_8_label3:
		for (int i = 0; i < 8; i++){
#pragma HLS UNROLL
			if (i>=w) break;
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
inv_dct_col_8_label4:
			for (k = 0; k < 4; k++) {
#pragma HLS UNROLL
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);
				block[i][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);

			}
		}
	}
}


void inv_dct_row_8(U8 w, U8 h, s16 coeff[8][8], s16 block[8][8], U4 shift)
{
#pragma HLS PIPELINE

	U7 size = 8;

	if (w == 4){
		int E[2], O[2];
		int rnd_factor = 1 << (shift - 1);
		for (int i = 0; i < 8; i++){
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
	else if (w == 8){
		int E[4], O[4];
		int EE[2], EO[2];
		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < 8; i++){
#pragma HLS UNROLL
			if (i>=h) break;
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
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);
				block[i][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);

			}
		}
	}
}

static s64 enc_ssd_16b_1_8(U3 w_log2, U3 h_log2, s16 src1[8][8], s16 src2[64], pel s_src1, U7 s_src2, U4 bit_depth)
{
#pragma HLS UNROLL
#pragma HLS PIPELINE


	int offset2 = 0;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	const int shift = (bit_depth - 8) << 1;

	s64 ssd = 0;

	int diff;

#if 0

	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		if (i >= h)
			break;

		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j >= w)
				break;

			diff = src1[i][j] - src2[offset2 + j];
			ssd += ((diff * diff >> shift));
		}
		offset2 += s_src2;
	}

#else
	s64 ssd0 = 0;
	s64 ssd1 = 0;
	s64 ssd2 = 0;
	s64 ssd3 = 0;

#pragma HLS ARRAY_PARTITION variable=src2 complete dim=1
	for (int i = 0; i < 4 ; i++) {
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++) {
			diff = src1[i][j] - src2[8*i + j];
			ssd0 += ((diff * diff >> shift));
		}
	}

	for (int i = 0; i < 4 ; i++) {
#pragma HLS UNROLL
		for (int j = 4; j < 8; j++) {
			diff = src1[i][j] - src2[8*i + j];
			ssd1 += ((diff * diff >> shift));
		}
	}

	for (int i = 4; i < 8 ; i++) {
#pragma HLS UNROLL
		for (int j = 0; j < 4; j++) {
			diff = src1[i][j] - src2[8*i + j];
			ssd2 += ((diff * diff >> shift));
		}
	}

	for (int i = 4; i < 8 ; i++) {
#pragma HLS UNROLL
		for (int j = 4; j < 8; j++) {
			diff = src1[i][j] - src2[8*i + j];
			ssd3 += ((diff * diff >> shift));
		}
	}

	if (w == 4 && h == 8)
	{
		ssd = ssd0 + ssd2;
	}
	else if(w == 8 && h == 4)
	{
		ssd = ssd0 + ssd1;
	}
	else if(w == 8 && h == 8)
	{
		ssd = ssd0 + ssd1 + ssd2 + ssd3;
	}

#endif


	return ssd;
	//add by xujch
}
static s64 enc_ssd_16b_2_8(U3 w_log2, U3 h_log2, s16 src1[8][8], s16 src2[8][8], pel s_src1, U7 s_src2, U4 bit_depth)
{
#pragma HLS UNROLL
#pragma HLS PIPELINE


	int offset2 = 0;
	U8 w = 1 << w_log2;
	U8 h = 1 << h_log2;
	const int shift = (bit_depth - 8) << 1;

	s64 ssd = 0;

	int diff;

#if 0

	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		if (i >= h)
			break;

		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j >= w)
				break;

			diff = src1[i][j] - src2[offset2 + j];
			ssd += ((diff * diff >> shift));
		}
		offset2 += s_src2;
	}

#else
	s64 ssd0 = 0;
	s64 ssd1 = 0;
	s64 ssd2 = 0;
	s64 ssd3 = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			//diff = src1[i][j] - src2[s_src2*i + j];
			diff = src1[i][j] - src2[i][j];
			ssd0 += ((diff * diff >> shift));
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 4; j < 8; j++) {
			//diff = src1[i][j] - src2[s_src2*i + j];
			diff = src1[i][j] - src2[i][j];
			ssd1 += ((diff * diff >> shift));
		}
	}

	for (int i = 4; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			//diff = src1[i][j] - src2[s_src2*i + j];
			diff = src1[i][j] - src2[i][j];
			ssd2 += ((diff * diff >> shift));
		}
	}

	for (int i = 4; i < 8; i++) {
		for (int j = 4; j < 8; j++) {
			//diff = src1[i][j] - src2[s_src2*i + j];
			diff = src1[i][j] - src2[i][j];
			ssd3 += ((diff * diff >> shift));
		}
	}

	if (w == 4 && h == 8)
	{
		ssd = ssd0 + ssd2;
	}
	else if (w == 8 && h == 4)
	{
		ssd = ssd0 + ssd1;
	}
	else if (w == 8 && h == 8)
	{
		ssd = ssd0 + ssd1 + ssd2 + ssd3;
	}

#endif


	return ssd;
	//add by xujch
}


void recon_8(PART_SIZE part, s16 resi[8][8], pel pred[8][8], int is_coef, int plane, U7 cu_width, U7 cu_height, int s_rec, pel rec[8][8], U4 bit_depth)
{

#pragma HLS INLINE off
	int i;
	int j;
	//get_part_num(part);
	//int tb_x;
	//int tb_y;
	pel p;
	//pel* r;
	U1 is_coef_zero;
	//get_tb_width_height(cu_width, cu_height, part, &tb_width, &tb_height);

	//get_tb_start_pos(cu_width, cu_height, part, k, &tb_x, &tb_y);
	//r = rec/* + tb_y * s_rec + tb_x*/;
	/* just copy pred to rec */
	is_coef_zero = is_coef == 0;
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		if (i >= cu_height)
			break;

		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j >= cu_width)
				break;

			p = is_coef_zero ? pred[i][j] : resi[i][j] + pred[i][j];
			rec[i][j] = (((0 > (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)) ? 0 : (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)))));
		}
		//r += s_rec;
	}
}

void recon_8(PART_SIZE part, s16 resi[8][8], pel pred[8][8], int(*is_coef)[3], int plane, U7 cu_width, U7 cu_height, int s_rec, pel rec[8][8], U4 bit_depth)
{

#pragma HLS INLINE off
	int i;
	int j;
	//get_part_num(part);
	//int tb_x;
	//int tb_y;
	pel p;
	//pel* r;
	U1 is_coef_zero;
	//get_tb_width_height(cu_width, cu_height, part, &tb_width, &tb_height);

	//get_tb_start_pos(cu_width, cu_height, part, k, &tb_x, &tb_y);
	//r = rec/* + tb_y * s_rec + tb_x*/;
	/* just copy pred to rec */
	is_coef_zero = is_coef[0][plane] == 0;
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		if (i >= cu_height)
			break;

		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j >= cu_width)
				break;

			p = is_coef_zero ? pred[i][j] : resi[i][j] + pred[i][j];
			rec[i][j] = (((0 >(((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)) ? 0 : (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)))));
		}
		//r += s_rec;
	}
}

void recon_chroma8(s16 resi[8][8], pel pred[8][8], int is_coef,  U7 cu_width, U7 cu_height,  pel rec[8][8], U4 bit_depth)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2		
#pragma HLS ARRAY_PARTITION variable=rec complete dim=2
#pragma HLS ARRAY_PARTITION variable=resi complete dim=2	
	

#pragma HLS INLINE off
	int i;
	int j;
	//get_part_num(part);
	//int tb_x;
	//int tb_y;
	pel p;
	//pel* r;
	U1 is_coef_zero;
	//get_tb_width_height(cu_width, cu_height, part, &tb_width, &tb_height);

	//get_tb_start_pos(cu_width, cu_height, part, k, &tb_x, &tb_y);
	//r = rec/* + tb_y * s_rec + tb_x*/;
	/* just copy pred to rec */
	is_coef_zero = is_coef == 0;
	for (i = 0; i < 8; i++) {
#pragma HLS PIPELINE

		for (j = 0; j < 8; j++) {
			if (is_coef_zero ) {
				p =  pred[i][j] ;
			}
			else {
				p =  resi[i][j] + pred[i][j];
			}
			if ((i < cu_height)&&(j < cu_width)){
			rec[i][j] = (((0 > (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)) ? 0 : (((1 << bit_depth) - 1 < p ? (1 << bit_depth) - 1 : p)))));
			}
			else
			{
				rec[i][j] = 0;
			}
		}
		//r += s_rec;
	}
}
void itrans_h_8(U8 w, U8 h, s16 resi[8][8], s16 coef_temp[8][8], U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth)
{
	U4 shift2;
	//s16 block1[8][8];
	//s16 rec1[8][8];
	U7 size = 8;

#pragma HLS INLINE off
	//s16 coef_temp[MAX_TR_DIM];

	shift2 = com_get_inverse_trans_shift(cu_height_log2, 1, bit_depth);

	inv_dct_row_8(w, h, coef_temp, resi, shift2);
}

void itrans_v_8(U8 w, U8 h, s16 coef_dq[8][8], s16 coef_temp[8][8], U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth)
{

#pragma HLS INLINE off
	U4 shift1 = com_get_inverse_trans_shift(cu_width_log2, 0, bit_depth);

	U7 size = 8;


	inv_dct_col_8(w, h, coef_dq, coef_temp, shift1);

}


void dquant_8(U6 qp, s16 coef[8][8], s16 coef_out[8][8], U3 log2_w, U3 log2_h, U4 bit_depth, int* num_nz_for_ith)
{

	u8 i;
	u8 j;
	u8 w = 1 << log2_w;
	u8 h = 1 << log2_h;
	U4 log2_size = ((U4)log2_w + log2_h) >> 1;
	int refix = (log2_w + log2_h) & 1;
	static const int com_tbl_dq_scale[80] = { // [64 + 16]
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

	static const U4 com_tbl_dq_shift[80] = { // [64 + 16]
		14, 14, 14, 14, 14, 14, 14, 14,  //15, 15, 15, 15, 15, 15, 15, 15,
		13, 13, 13, 13, 13, 13, 13, 13,  //14, 14, 14, 14, 14, 14, 14, 14,
		13, 12, 12, 12, 12, 12, 12, 12,  //14, 13, 13, 13, 13, 13, 13, 13,
		11, 11, 11, 11, 11, 11, 11, 11,  //12, 12, 12, 12, 12, 12, 12, 12,
		11, 10, 10, 10, 10, 10, 10, 10,  //12, 11, 11, 11, 11, 11, 11, 11,
		10, 9, 9, 9, 9, 9, 9, 9,  //11, 10, 10, 10, 10, 10, 10, 10,
		9, 8, 8, 8, 8, 8, 8, 8,  //10,  9,  9,  9,  9,  9,  9,  9,
		7, 7, 7, 7, 7, 7, 7, 7,  // 8,  8,  8,  8,  8,  8,  8,  8,
		6, 6, 6, 6, 6, 6, 6, 6,  // 7,  7,  7,  7,  7,  7,  7,  7,
		5, 5, 5, 5, 5, 5, 5, 5,  // 6,  6,  6,  6,  6,  6,  6,  6dquant_16
	};

	int scale = com_tbl_dq_scale[qp];
	// +1 is used to compensate for the mismatching of shifts in quantization and inverse quantization
	int shift = com_tbl_dq_shift[qp] - get_transform_shift(bit_depth, log2_size) + 1;
	int offset = shift == 0 ? 0 : 1 << (shift - 1);

	int sr_x = 0;
	int sr_y = 0;
dquant_8_label0:
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		if (i >= h) break;
dquant_8_label1:
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j >= w) break;
			if (coef[i][j]) {
				sr_y = i > sr_y ? i : sr_y;
				sr_x = j > sr_x ? j : sr_x;
			}
			s16 weight = (i | j) & 0xE0 ? 0 : coef[i][j];
			int lev = (weight * scale + offset) >> shift;
			lev = (lev > 32767 ? 32767 : ((lev < -32768 ? -32768 : lev)));
			if (refix) {
				lev = (lev * 181 + 128) >> 8;
			}
			coef_out[i][j] = (lev);
		}
	}
	*num_nz_for_ith = (sr_y << 20) + (sr_x << 12) + *num_nz_for_ith;
}
void dquant_8_chroma(U8 w, U8 h, U6 qp, s16 coef[8][8], s16 coef_out[8][8], U3 log2_w, U3 log2_h, U4 bit_depth, int* num_nz_for_ith, int num_nz_tmp)
{
#pragma HLS INLINE 
	u8 i;
	u8 j;
	//u8 w = 1 << log2_w;
	//u8 h = 1 << log2_h;
	U4 log2_size = ((U4)log2_w + log2_h) >> 1;
	int refix = (log2_w + log2_h) & 1;
	static const int com_tbl_dq_scale[80] = { // [64 + 16]
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

	static const U4 com_tbl_dq_shift[80] = { // [64 + 16]
		14, 14, 14, 14, 14, 14, 14, 14,  //15, 15, 15, 15, 15, 15, 15, 15,
		13, 13, 13, 13, 13, 13, 13, 13,  //14, 14, 14, 14, 14, 14, 14, 14,
		13, 12, 12, 12, 12, 12, 12, 12,  //14, 13, 13, 13, 13, 13, 13, 13,
		11, 11, 11, 11, 11, 11, 11, 11,  //12, 12, 12, 12, 12, 12, 12, 12,
		11, 10, 10, 10, 10, 10, 10, 10,  //12, 11, 11, 11, 11, 11, 11, 11,
		10, 9, 9, 9, 9, 9, 9, 9,  //11, 10, 10, 10, 10, 10, 10, 10,
		9, 8, 8, 8, 8, 8, 8, 8,  //10,  9,  9,  9,  9,  9,  9,  9,
		7, 7, 7, 7, 7, 7, 7, 7,  // 8,  8,  8,  8,  8,  8,  8,  8,
		6, 6, 6, 6, 6, 6, 6, 6,  // 7,  7,  7,  7,  7,  7,  7,  7,
		5, 5, 5, 5, 5, 5, 5, 5,  // 6,  6,  6,  6,  6,  6,  6,  6dquant_16
	};

	int scale = com_tbl_dq_scale[qp];
	// +1 is used to compensate for the mismatching of shifts in quantization and inverse quantization
	int shift = com_tbl_dq_shift[qp] - get_transform_shift(bit_depth, log2_size) + 1;
	int offset = shift == 0 ? 0 : 1 << (shift - 1);

	int sr_x = 0;
	int sr_y = 0;

	s16 coef_tmp[8];
	int lev_tmp;
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=0
	s16 i_tmp[8];
#pragma HLS ARRAY_PARTITION variable=i_tmp complete dim=0
	U1 sr_x_flag;
	int sr_y_tmp;

		for (j = 0; j < 8; j++) {
#pragma HLS PIPELINE
		for (int i = 0; i < 8; i++) {
			coef_tmp[i] = coef[i][j];
		}
		sr_x_flag = 0;
		for (i = 0; i < 8; i++) {
			if ((i < h) && (j < w)) {
				if (coef_tmp[i]) {
					//sr_y = i > sr_y ? i : sr_y;
					i_tmp[i] = 1;
					//sr_x = j > sr_x ? j : sr_x;
					sr_x_flag = 1;
			}
				else {
					i_tmp[i] = 0;
				}
				s16 weight = (i | j) & 0xE0 ? 0 : coef_tmp[i];
			int lev = (weight * scale + offset) >> shift;
			lev = (lev > 32767 ? 32767 : ((lev < -32768 ? -32768 : lev)));
			if (refix) {
				lev = (lev * 181 + 128) >> 8;
				}
				lev_tmp = lev;
			}
			else {
				lev_tmp = 0;
				i_tmp[i] = 0;
			}
			coef_out[i][j] = lev_tmp;
		}
		sr_y_tmp = 0;
		for (int i = 7; i >= 0; i--) {
			if (i_tmp[i] == 1) {
				sr_y_tmp = i;
				break;
			}
		}
		if (sr_y < sr_y_tmp) {
			sr_y = sr_y_tmp;
		}
		if (sr_x_flag) {
			sr_x = j;
		}
	}
	*num_nz_for_ith = (sr_y << 20) + (sr_x << 12) + num_nz_tmp;
}

u8 transform_hor_8(U8 w, U8 h, U4 bit_depth, pel diff[8][8], pel coef_h[8][8])
{
#pragma HLS INLINE
#pragma HLS   ARRAY_PARTITION variable=diff complete dim=0
#pragma HLS   ARRAY_PARTITION variable=coef_h complete dim=0
	U3 tb_width_log2 = com_tbl_log2[w];
	U3 tb_height_log2 = com_tbl_log2[h];

	U7 size = 8;
	U4 shift1;
	U4 shift2;

	shift1 = com_get_forward_trans_shift(tb_width_log2, 0, bit_depth);
	shift2 = com_get_forward_trans_shift(tb_height_log2, 1, bit_depth);

	dct_row_8(w, h, diff, coef_h, shift1);

	return 1;
}

u8 transform_ver_8(U8 w, U8 h, U4 bit_depth, pel coef_h[8][8], pel coef[8][8])
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=0
	U3 tb_width_log2 = com_tbl_log2[w];
	U3 tb_height_log2 = com_tbl_log2[h];

	U4 shift1, shift2;
	U7 size = 8;

	shift1 = com_get_forward_trans_shift(tb_width_log2, 0, bit_depth);
	shift2 = com_get_forward_trans_shift(tb_height_log2, 1, bit_depth);
	dct_col_8(w, h, coef_h, coef, shift2);

	return 1;
}

void inv_transform_ver_8(U8 w, U8 h, U4 bit_depth, U6 qp_y, int num_nz_for_itv, pel qcoef_for_dq[8][8], int* num_nz_for_ith, int* num_nz_for_up, pel itcoef_v[8][8])
{
#pragma HLS INLINE

	U3 tb_width_log2 = com_tbl_log2[w];
	U3 tb_height_log2 = com_tbl_log2[h];
	static s16 coef_dq[8][8];
	int num_nz_read = num_nz_for_itv;
#pragma HLS   ARRAY_PARTITION variable=qcoef_for_dq complete dim=0
#pragma HLS   ARRAY_PARTITION variable=coef_dq complete dim=0
#pragma HLS   ARRAY_PARTITION variable=itcoef_v complete dim=0
	if (num_nz_read) {
		dquant_8(qp_y, qcoef_for_dq, coef_dq, tb_width_log2, tb_height_log2, bit_depth, &num_nz_read);
		itrans_v_8(w, h, coef_dq, itcoef_v, tb_width_log2, tb_height_log2, bit_depth);
	}

	*num_nz_for_ith = num_nz_read;
	*num_nz_for_up = num_nz_read;
}

void inv_transform_hor_8(U8 w, U8 h, U4 bit_depth, int tb_part, U3 cu_width_log2, U3 cu_height_log2, U3 type,
	int num_nz_for_ith,
	pel orgY_for_ssd[8][8], pel pred_for_rec[8][8], pel itcoef_v[8][8],
	s32* dist_value,
	pel pred_for_update[8][8], pel rec_for_update[8][8])
{
#pragma HLS INLINE
	static pel rec[8][8];
#pragma HLS   ARRAY_PARTITION variable=rec complete dim=0
#pragma HLS   ARRAY_PARTITION variable=orgY_for_ssd complete dim=0
#pragma HLS   ARRAY_PARTITION variable=pred_for_rec complete dim=0
#pragma HLS   ARRAY_PARTITION variable=pred_for_update complete dim=0
#pragma HLS   ARRAY_PARTITION variable=rec_for_update complete dim=0
	static pel resi[8][8];
#pragma HLS   ARRAY_PARTITION variable=resi complete dim=0
	int s_org = 8;

	s32 cost = 0;

	if (num_nz_for_ith) {
		itrans_h_8(w, h, resi, itcoef_v, cu_width_log2, cu_height_log2, bit_depth);
	}

	recon_8((PART_SIZE)(type == 0 ? tb_part : (SIZE_2Nx2N)), resi, pred_for_rec,
		num_nz_for_ith, type, w, h, w, rec, bit_depth);

	if (!type)
	{
		cost += ((enc_ssd_16b_2_8(cu_width_log2, cu_height_log2, rec, orgY_for_ssd, w, s_org, bit_depth)));
		*dist_value = cost;
	}

	memcpy_2d<pel, 8, 8>(pred_for_rec, pred_for_update);
	memcpy_2d<pel, 8, 8>(rec, rec_for_update);
}

void level_core_8(U8 w, U8 h, u32 level[24], u32 level1[24], U5 t0[64], U7 i, U14 max_num)
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
void run_core_8(U8 w, U8 h, u32 run[24], u32 run1[24], U5 t0[64], U7 i, U1 mat_run[64], U14 max_num)
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
void last1_core_8(U8 w, U8 h, u32 last1[12], U4 t0[64], U7 i, U14 max_num)
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
void last2_core_8(U8 w, U8 h, u32 last2[22], U5 t0[64], U7 i, U14 max_num)
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
void model_update_8(U8 w, U8 h, u32 last1[12], u32 last2[22], u32 run[24], u32 level[24], U1 mat_run[64], U24 mat_prev_level[64], int ch_type, U14 max_num, int* sum_run, U32 last1_tmp[6], U32 last2_tmp[7])
{
	U7 scan_pos = 0;
	int ace_log2 = 0;
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

int rdoq_top_8(U6 qp, s64 d_lambda, U1 is_intra, s16 src_coef[8][8], s16 dst_tmp[8][8], U3 cu_width_log2, u32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array)
{
#pragma HLS PIPELINE II=1
#pragma HLS ARRAY_PARTITION variable = src_coef complete dim = 0
#pragma HLS ARRAY_PARTITION variable = dst_tmp complete dim = 0

	static U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };

	const U14 scale = quant_scale[qp];
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);

	const U8 w = 1 << cu_width_log2;
	const U8 h = 1 << cu_height_log2;
	const U14 max_num_coef = w * h;
	u8 scan_tbl_index = 0;
	static u8 scan[64];
	static const u8 scan_t[4][64] = {
		{ 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 13, 10, 7, 11, 14, 15 },
		{ 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 29, 26, 23, 27, 30, 31},
		{ 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 25, 18, 11, 4, 5, 12, 19, 26, 27, 20, 13, 6, 7, 14, 21, 28, 29, 22, 15, 23, 30, 31 },
		{ 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63,}
	};

	if (w == 4/* && h == 8*/) {
		scan_tbl_index = 1;
	}
	else if (/*w == 8 && */h == 4) {
		scan_tbl_index = 2;
	}
	else/* if (w == 8 && h == 8)*/
	{
		scan_tbl_index = 3;
	}

	for (u8 i = 0; i < 64; i++)
	{
#pragma HLS UNROLL
		scan[i] = scan_t[scan_tbl_index][i];
	}

	const int ctx_last = (ch_type == Y_C) ? 0 : 1;
	const int q_bits = QUANT_SHIFT + tr_shift;
	int num_nz_coef = 0;
	int scan_pos;
	s32 ctx_qt_cbf;
	int best_last_idx_p1 = 0;
	static s16 tmp_coef[64] = { 0 };
	static U36 tmp_level_double[64] = { 0 };
	static s16 tmp_dst_coef[64] = { 0 };
	const s64 lambda = ((d_lambda << 15) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	//const s64 lambda = (s64)(d_lambda * (double)(1 << SCALE_BITS) + 0.5);
	static U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	s64 d64_best_cost = 0;
	static s64 d64_base_cost[64] = { 0 };
	s64 d64_coded_cost = 0;
	s64 d64_uncoded_cost = 0;

#pragma HLS ARRAY_PARTITION variable=tmp_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_level_double complete dim=0
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=d64_base_cost complete dim=0

	/* ===== quantization ===== */

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		//#pragma HLS LOOP_TRIPCOUNT max=64
		if (scan_pos >= max_num_coef)
			break;

		u8 blk_pos = scan[scan_pos];
		u8 x = blk_pos & (w - 1);
		u8 y = blk_pos >> cu_width_log2;

		U36 level_double = src_coef[y][x];
		U24 max_abs_level;
		U1 lower_int;
		U36 temp_level;
		temp_level = ((s64)COM_ABS(src_coef[y][x]) * (s64)q_value);
		level_double = (int)COM_MIN(((s64)temp_level), (s64)COM_INT32_MAX - ((s64)1 << (q_bits - 1)));
		tmp_level_double[blk_pos] = level_double;
		max_abs_level = (u32)(level_double >> q_bits);
		lower_int = ((level_double - ((s64)max_abs_level << q_bits)) < ((s64)1 << (q_bits - 1))) ? 1 : 0;
		if (!lower_int)
		{
			max_abs_level++;
		}
		tmp_coef[blk_pos] = src_coef[y][x] > 0 ? (s16)max_abs_level : -(s16)(max_abs_level);
	}

	if (!is_intra && ch_type == Y_C)
	{
		d64_best_cost = 0;
		for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
#pragma HLS UNROLL
			if (scan_pos < max_num_coef)
				d64_base_cost[scan_pos] = 0;
		}
	}
	else
	{
		ctx_qt_cbf = ch_type;
		d64_best_cost = GET_I_COST(rdoq_array->rdoq_est_cbf[ctx_qt_cbf][0], lambda);
		//d64_best_cost = d64_best_cost >> 24;
		for (scan_pos = 0; scan_pos < 64; scan_pos++)
		{
#pragma HLS UNROLL
			if (scan_pos < max_num_coef)
				d64_base_cost[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_cbf[ctx_qt_cbf][1], lambda);
			//d64_base_cost[scan_pos] = d64_base_cost[scan_pos] >> 24;
		}
	}

	static U24 mat_prev_level[64] = { 0 };
	static U1 mat_run[64] = { 0 };
	mat_prev_level[0] = 6;
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=0

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num_coef)
		{
			u32 blk_pos = scan[scan_pos];

			if (COM_ABS(tmp_coef[blk_pos]))
			{
				mat_prev_level[scan_pos] = COM_ABS(tmp_coef[blk_pos]);
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
#pragma HLS ARRAY_PARTITION variable=mat_uncoded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=mat_coded_cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=lev_opt complete dim=0

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num_coef)
		{
			u32 blk_pos = scan[scan_pos];
			U24 level;
			U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);

			int iflast = (scan_pos == max_num_coef - 1);
			level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[blk_pos], COM_ABS(tmp_coef[blk_pos]),
				mat_run[scan_pos], (u16)ctx_run, (u16)ctx_run, q_bits, err_scale, lambda, iflast, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level);
			tmp_dst_coef[blk_pos] = (s16)(tmp_coef[blk_pos] < 0 ? -(s16)(level) : (s16)level);

			mat_uncoded_cost[scan_pos] = d64_uncoded_cost;
			mat_coded_cost[scan_pos] = d64_coded_cost;
			lev_opt[scan_pos] = level;
		}
	}

	static s64 d64_cost_last_zero[64] = { 0 };
	d64_cost_last_zero[0] = 0;
	int ace_log2 = 0;
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=0

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num_coef)
		{
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
			d64_cost_last_zero[scan_pos] = GET_I_COST(rdoq_array->rdoq_est_last[ctx_last][COM_MIN((U24)(mat_prev_level[scan_pos] - 1), (U24)(5))][ace_log2][0], lambda);
			//d64_cost_last_zero[scan_pos] = d64_cost_last_zero[scan_pos] >> 24;
		}

	}
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num_coef)
		{
			for (int i = 0; i < 64; i++)
			{
#pragma HLS UNROLL
				if (i <= scan_pos)
					d64_base_cost[scan_pos] += mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i] * !!(lev_opt[i]);
			}
		}

	}
	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num_coef)
		{
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
			else if ((scan_pos + 1) < 64)
			{
				ace_log2 = 5;
			}
			else if ((scan_pos + 1) < 128)
			{
				ace_log2 = 6;
			}
			else if ((scan_pos + 1) < 256)
			{
				ace_log2 = 7;
			}
			else if ((scan_pos + 1) < 512)
			{
				ace_log2 = 8;
			}
			else if ((scan_pos + 1) < 1024)
			{
				ace_log2 = 9;
			}
			else if ((scan_pos + 1) < 2048)
			{
				ace_log2 = 10;
			}
			else if ((scan_pos + 1) < 4096)
			{
				ace_log2 = 11;
			}
			else
			{
				ace_log2 = 11;
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
	}
	//model_update_8(w, h, model_cnt->last1, model_cnt->last2, model_cnt->run_rdoq, model_cnt->level, mat_run, mat_prev_level, ch_type, max_num_coef, sum_run, last1_tmp, last2_tmp);

	for (scan_pos = 0; scan_pos < 64; scan_pos++)
	{
#pragma HLS UNROLL
		if (scan_pos < max_num_coef)
		{
			u8 blk_pos = scan[scan_pos];
			u8 x = blk_pos & (w - 1);
			u8 y = blk_pos >> cu_width_log2;

			dst_tmp[y][x] = scan_pos < best_last_idx_p1 ? tmp_dst_coef[blk_pos] : 0;
			num_nz_coef += !!(dst_tmp[y][x]);
		}
	}
	return num_nz_coef;
}

static void memcpy_rewrintra_2d_to_1d_8(s16* des, s16 src[8][8], U3 cu_width_log2, U3 cu_height_log2)
{
	U7 w = 1 << cu_width_log2;
	U7 h = 1 << cu_height_log2;

#pragma HLS INLINE off
	int i, j;
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=8
		for (j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=8
			des[(i << cu_width_log2) + j] = src[i][j];
		}

	}
}

static int quant_8(U7 w, U7 h, U3 cu_width_log2, U3 cu_height_log2, U4 bit_depth, s16 coef[8][8], s16 coef_out[8][8],U2 slice_type, U14 scale)
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
			coef_out[i][j] = COM_SIGN_SET(lev, sign);

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
int rdoq_top_8_8(S64 err_scale, RDOQ_MODEL* model_cnt, U6 qp, S64 lambda, U1 is_intra, S16 src_coef[8][8], S16 dst_tmp[8][8], U3 cu_width_log2, U32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array);

int rdoq_top_8_8_luma(S64 err_scale, U6 qp, S64 lambda, U1 is_intra, pel src_coef[8][8], s16 dst_tmp[8][8], U3 cu_width_log2, U32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array);


int rdoq_top_8_8_chroma(S64 err_scale, U6 qp, S64 lambda, U1 is_intra, S16 src_coef[8][8], S16 dst_tmp[8][8], U3 cu_width_log2, U32 cu_height_log2, U2 ch_type, U4 bit_depth, RDOQ_ARRAY* rdoq_array);


void quant_nnz_8_luma(U3 cu_width_log2, U3 cu_height_log2, U2 ch_type, U2 slice_type, U4 bit_depth, U6 qp,
	s64 lambda_y_for_quant, U1 is_intra_for_quant,
	RDOQ_ARRAY* rdoq_array,
	pel coef[8][8],
	s64* lambda_y_for_re, U1* is_intra_for_re, int* num_nz_for_re, int* num_nz_for_itv,
	pel qcoef_for_dq[8][8], pel qcoef_for_re[8][8], pel qcoef_for_update[8][8])
{
#pragma HLS INLINE
	static pel qcoef[8][8];
	int num_nz_coef = 0;
	static U14 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
#pragma HLS ARRAY_PARTITION variable=qcoef_for_re complete dim=0
#pragma HLS ARRAY_PARTITION variable=qcoef_for_update complete dim=0
#pragma HLS ARRAY_PARTITION variable=qcoef complete dim=0
	U14 scale = quant_scale[qp];
	U7 width = 1 << cu_width_log2;
	U7 height = 1 << cu_height_log2;

#if RDOQ_OPEN
	if (width == 8 && height == 8)
	{
		const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
		const S64 lambda_d = ((lambda_y_for_quant << 15) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
		static U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };

		S64 err_scale = err_scale_tbl[qp][log2_size - 1];

		num_nz_coef = rdoq_top_8_8_luma(err_scale, qp, lambda_d, is_intra_for_quant, coef, qcoef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_array);
	}
	/*else
	{
		num_nz_coef = rdoq_top_8(model_cnt, qp, lambda_y_for_quant, is_intra_for_quant, coef, qcoef, cu_width_log2, cu_height_log2, ch_type, bit_depth, rdoq_array, sum_run, last1_tmp, last2_tmp);
	}*/
#else
	num_nz_coef = quant_8(width, height, cu_width_log2, cu_height_log2, bit_depth, coef, qcoef,slice_type, scale);
#endif

	* is_intra_for_re = is_intra_for_quant;
	*num_nz_for_re = num_nz_coef;
	*num_nz_for_itv = num_nz_coef;
	*lambda_y_for_re = lambda_y_for_quant;
	memcpy_2d<pel, 8, 8>(qcoef, qcoef_for_dq);
	/*memcpy_rewrintra_2d_to_1d_8(qcoef_for_re, qcoef, cu_width_log2, cu_height_log2);
	memcpy_rewrintra_2d_to_1d_8(qcoef_for_update, qcoef, cu_width_log2, cu_height_log2);*/
	for (int i = 0; i < height; i++)
	{
#pragma HLS  UNROLL
		for (int j = 0; j < width; j++)
		{
#pragma HLS  UNROLL
			qcoef_for_re[i][j] = qcoef[i][j];
			qcoef_for_update[i][j] = qcoef[i][j];
		}
	}
}

int  enc_eco_coef_8_1(s16 coef[64], U3 log2_w, U3 log2_h)
{
	U15 enc_est = 0;
	U6 scan_pos;
	u16 level;

	U10 run;

	s16 coef_cur;

	U8 w = 1 << log2_w;
	U8 h = 1 << log2_h;
	U8 size = w * h;

	u8 scan_tab_index = 0;
	static u8 scan[3][64] = {
		{ 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 29, 26, 23, 27, 30, 31},
		{ 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 25, 18, 11, 4, 5, 12, 19, 26, 27, 20, 13, 6, 7, 14, 21, 28, 29, 22, 15, 23, 30, 31},
		{ 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63}
	};

	if (w == 4 && h == 8) {
		scan_tab_index = 0;
	}
	else if (w == 8 && h == 4) {
		scan_tab_index = 1;
	}
	else {
		scan_tab_index = 2;
	}
	run = 0;
	for (scan_pos = 0; scan_pos < size; scan_pos++) {
#pragma HLS loop_tripcount max=64
		coef_cur = coef[scan[scan_tab_index][scan_pos]];
		if (coef_cur) {
			level = abs(coef_cur);
			//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
			enc_est += enc_eco_run_est_3(run);
			enc_est += enc_eco_level_est_3(level);
			enc_est++;
			if (scan_pos == size - 1) {
				break;
			}
			run = 0;
			enc_est++;
		}
		else {
			run++;
		}


	}
	return enc_est;
}
int  enc_eco_coef_8(s16 coef[8][8], U3 log2_w, U3 log2_h)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coef complete dim=0
	int i;
	U15 enc_est1 = 0;
	U15 enc_est2 = 0;
	U15 enc_est3 = 0;
	U15 enc_est4 = 0;
	U15 enc_est5 = 0;
	U15 enc_est6 = 0;
	U15 enc_est7 = 0;
	U15 enc_est8 = 0;
	//U8 run = 0;
	U10 run1 = 0;
	U10 run2 = 0;
	U10 run3 = 0;
	U10 run4 = 0;
	U10 run5 = 0;
	U10 run6 = 0;
	U10 run7 = 0;
	U10 run8 = 0;
	U8 runflag1 = 0;
	U8 runflag2 = 0;
	U8 runflag3 = 0;
	U8 runflag4 = 0;
	U8 runflag5 = 0;
	U8 runflag6 = 0;
	U8 runflag7 = 0;
	U15 enc_est = 0;
	U6 scan_pos;
	u16 level;

	U10 run;

	s16 coef_cur;

	U8 w = 1 << log2_w;
	U8 h = 1 << log2_h;
	U8 size = w * h;

	u8 scan_tab_index = 0;
	/*static u8 scan[3][64] = {
		{ 0, 1, 4, 8, 5, 2, 3, 6, 9, 12, 16, 13, 10, 7, 11, 14, 17, 20, 24, 21, 18, 15, 19, 22, 25, 28, 29, 26, 23, 27, 30, 31},
		{ 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 25, 18, 11, 4, 5, 12, 19, 26, 27, 20, 13, 6, 7, 14, 21, 28, 29, 22, 15, 23, 30, 31},
		{ 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63}
	};*/

	static u8 scan[64] = {0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63};

	
	run = 0;
	if (w == 8 && h == 8) {
		for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est1 += enc_eco_run_est_3(run1);
				enc_est1 += enc_eco_level_est_3(level);
				enc_est1++;
				run1 = 0;
				enc_est1++;
			}
			else {
				run1++;
			}
			if (i == 7) {
				runflag1 = run1;
			}
		}
		for (i = 8; i < 16; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				run2 = 0;
			}
			else {
				run2++;
			}
			if (i == 15) {
				runflag2 = run2;
			}
		}
		for (i = 16; i < 24; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				run3 = 0;
			}
			else {
				run3++;
			}
			if (i == 23) {
				runflag3 = run3;
			}
		}
		for (i = 24; i < 32; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				run4 = 0;
			}
			else {
				run4++;
			}
			if (i == 31) {
				runflag4 = run4;
			}
		}
		for (i = 32; i < 40; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				run5 = 0;
			}
			else {
				run5++;
			}
			if (i == 39) {
				runflag5 = run5;
			}
		}
		for (i = 40; i < 48; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				run6 = 0;
			}
			else {
				run6++;
			}
			if (i == 47) {
				runflag6 = run6;
			}
		}
		for (i = 48; i < 56; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (coef_cur) {
				run7 = 0;
			}
			else {
				run7++;
			}
			if (i == 55) {
				runflag7 = run7;
			}

		}
		//////////////////////////////////
		for (i = 8; i < 16; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			if (i == 8) {
				run = runflag1;
			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est2 += enc_eco_run_est_3(run);
				enc_est2 += enc_eco_level_est_3(level);
				enc_est2++;
				run = 0;
				enc_est2++;
			}
			else {
				run++;
			}
		}
		for (i = 16; i < 24; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			//coef_cur = coef[scan[scan_tab_index][scan_pos]];
			if (i == 16) {
				if (runflag2 == 8) {
					run = runflag1 + runflag2;
				}
				else {
					run = runflag2;
				}
			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est3 += enc_eco_run_est_3(run);
				enc_est3 += enc_eco_level_est_3(level);
				enc_est3++;
				run = 0;
				enc_est3++;
			}
			else {
				run++;
			}
		}
		for (i = 24; i < 32; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			//coef_cur = coef[scan[scan_tab_index][scan_pos]];
			if (i == 24) {
				if (runflag3 == 8) {
					if (runflag2 == 8) {
						run = runflag1 + runflag2 + runflag3;
					}
					else {
						run = runflag2 + runflag3;
					}
				}
				else {
					run = runflag3;
				}
			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est4 += enc_eco_run_est_3(run);
				enc_est4 += enc_eco_level_est_3(level);
				enc_est4++;
				run = 0;
				enc_est4++;
			}
			else {
				run++;
			}
		}
		for (i = 32; i < 40; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			//coef_cur = coef[scan[scan_tab_index][scan_pos]];
			if (i == 40) {
				if (runflag4 == 8) {
					if (runflag3 == 8) {
						if (runflag2 == 8) {
							run = runflag1 + runflag2 + runflag3 + runflag4;
						}
						else {
							run = runflag2 + runflag3 + runflag4;
						}
					}
					else {
						run = runflag3 + runflag4;
					}
				}
				else {
					run = runflag4;
				}
			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est5 += enc_eco_run_est_3(run);
				enc_est5 += enc_eco_level_est_3(level);
				enc_est5++;
				run = 0;
				enc_est5++;
			}
			else {
				run++;
			}
		}
		for (i = 40; i < 48; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			//coef_cur = coef[scan[scan_tab_index][scan_pos]];
			if (i == 40) {
				if (runflag5 == 8) {
					if (runflag4 == 8) {
						if (runflag3 == 8) {
							if (runflag2 == 8) {
								run = runflag1 + runflag2 + runflag3 + runflag4 + runflag5;
							}
							else {
								run = runflag2 + runflag3 + runflag4 + runflag5;
							}
						}
						else {
							run = runflag3 + runflag4 + runflag5;
						}

					}
					else {
						run = runflag4 + runflag5;
					}
				}
				else {
					run = runflag5;
				}
			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est6 += enc_eco_run_est_3(run);
				enc_est6 += enc_eco_level_est_3(level);
				enc_est6++;
				run = 0;
				enc_est6++;
			}
			else {
				run++;
			}
		}
		for (i = 48; i < 56; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			//coef_cur = coef[scan[scan_tab_index][scan_pos]];
			if (i == 48) {
				if (runflag6 == 8) {
					if (runflag5 == 8) {
						if (runflag4 == 8) {
							if (runflag3 == 8) {
								if (runflag2 == 8) {
									run = runflag1 + runflag2 + runflag3 + runflag4 + runflag5 + runflag6;
								}
								else {
									run = runflag2 + runflag3 + runflag4 + runflag5 + runflag6;
								}
							}
							else {
								run = runflag3 + runflag4 + runflag5 + runflag6;
							}
						}
						else {
							run = runflag4 + runflag5 + runflag6;
						}
					}
					else {
						run = runflag5 + runflag6;
					}
				}
				else {
					run = runflag6;
				}

			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est7 += enc_eco_run_est_3(run);
				enc_est7 += enc_eco_level_est_3(level);
				enc_est7++;
				run = 0;
				enc_est7++;
			}
			else {
				run++;
			}
		}
		for (i = 56; i < 64; i++) {
#pragma HLS UNROLL
			s16 coef_cur;
			u16 level;
			u8 blk_pos = scan[i];
			u8 x = blk_pos % w;
			u8 y = blk_pos >> log2_w;
			coef_cur = coef[y][x];
			//coef_cur = coef[scan[scan_tab_index][scan_pos]];
			if (i == 56) {
				if (runflag7 == 8) {
					if (runflag6 == 8) {
						if (runflag5 == 8) {
							if (runflag4 == 8) {
								if (runflag3 == 8) {
									if (runflag2 == 8) {
										run = runflag1 + runflag2 + runflag3 + runflag4 + runflag5 + runflag6 + runflag7;
									}
									else {
										run = runflag2 + runflag3 + runflag4 + runflag5 + runflag6 + runflag7;
									}
								}
								else {
									run = runflag3 + runflag4 + runflag5 + runflag6 + runflag7;
								}
							}
							else {
								run = runflag4 + runflag5 + runflag6 + runflag7;
							}
						}
						else {
							run = runflag5 + runflag6 + runflag7;
						}
					}
					else {
						run = runflag6 + runflag7;
					}
				}
				else {
					run = runflag7;
				}

			}
			if (coef_cur) {
				level = abs(coef_cur);
				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
				enc_est8 += enc_eco_run_est_3(run);
				enc_est8 += enc_eco_level_est_3(level);
				enc_est8++;
				if (i == 64 - 1) {
					break;
				}
				run = 0;
				enc_est8++;
			}
			else {
				run++;
			}
		}
		enc_est = enc_est1 + enc_est2 + enc_est3 + enc_est4 + enc_est5 + enc_est6 + enc_est7 + enc_est8;


	}
//	else {
//		for (scan_pos = 0; scan_pos < size; scan_pos++) {
//#pragma HLS loop_tripcount max=64
//			coef_cur = coef[scan[scan_tab_index][scan_pos]];
//			if (coef_cur) {
//				level = abs(coef_cur);
//				//	level = ((((int)coef_cur) ^ ((int)coef_cur) >> 15) - (((int)coef_cur) >> 15));
//				enc_est += enc_eco_run_est_3(run);
//				enc_est += enc_eco_level_est_3(level);
//				enc_est++;
//				if (scan_pos == size - 1) {
//					break;
//				}
//				run = 0;
//				enc_est++;
//			}
//			else {
//				run++;
//			}
//
//
//		}
//	}
	return enc_est;
}

int enc_eco_cbf_est_skip_8(U1 tb_avaliable, int num_nz[4][3], U2 pred_mode,
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

int enc_eco_cbf_est_8(int num_nz[4][3], U2 pred_mode,
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

int enc_bit_est_flag_8(U2 slice_type, U21 f_scu, u8 curr_mvr, U2 tree_status, U2 cons_pred_mode, U2 cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, U1 affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], U3 cu_width_log2, U3 cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], U9 cur_ptr, int refp_ptr[17][2], u8 num_of_hmvp_cand, u8 mpm[4][2], U1 intra)
{
#pragma HLS INLINE
#pragma HLS array_partition variable=mpm complete dim=0
//#pragma HLS array_partition variable=model_cnt->cbf complete dim=0
#pragma HLS array_partition variable=num_nz complete dim=0
#pragma HLS array_partition variable=ipm complete dim=0
#pragma HLS array_partition variable=refi complete dim=0
#pragma HLS array_partition variable=refp_ptr complete dim=0
#pragma HLS array_partition variable=mvd complete dim=0


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
				enc_est += encode_skip_idx_cu_est((skip_idx), num_of_hmvp_cand, slice_type);
			}
		}
		else {
			if (((cons_pred_mode)) == (NO_MODE_CONS)) {
				enc_est++;
			}
			if (amvr_enable_flag) {
				// also imply ctx->info.sqh.num_of_hmvp_cand is not zero
				if (emvr_enable_flag && !affine_flag) {
					enc_est++;
				}
				/*				if (affine_flag) {
									enc_est += sbac_write_truncate_unary_sym_est(curr_mvr, 3);
								}
								else */ {
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
					/*					if (affine_flag) {
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
										else */ {
						S16 a_mvd[2];
						a_mvd[0] = (((mvd[0][0]) >> (curr_mvr)));
						a_mvd[1] = (((mvd[0][1]) >> (curr_mvr)));
						enc_est += encode_mvd_est(a_mvd);
					}
				}
			}
		}


		if (tree_status != TREE_C) {
			enc_est += enc_eco_cbf_est_8(num_nz, cu_mode, ipm, tree_status, cu_width_log2, cu_height_log2);
		}
		else if (!((cu_mode) == 0 && (ipm[0][0]) == 33
			&& (ipm[0][1]) == 0)) {
			enc_est += 2;
		}
	}

	return enc_est;
}

int get_bitcnt_est_8_coef_0(U21 f_scu, u8 curr_mvr, U2 tree_status,
	U2 ch_type, U2 slice_type, U2 cons_pred_mode, U2 cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, U1 affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], U3 cu_width_log2, U3 cu_height_log2, int tb_part, int pb_part, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], U2 slice_type_md, U9 cur_ptr, int refp_ptr[17][2], u8 mpm[4][2], U1 is_intra)
{
#pragma HLS INLINE
	U16 enc_est;
	//U16 coef_est;
	enc_est = enc_bit_est_flag_8(slice_type, f_scu, curr_mvr, tree_status, cons_pred_mode, cu_mode, umve_flag, umve_idx, affine_flag, 1, skip_idx, refi, mvp_from_hmvp_flag, smvd_flag, num_refp, affine_mvd, mvd, cu_width_log2, cu_height_log2, num_nz, ipm, cur_ptr, refp_ptr, 8, mpm, is_intra);
	//coef_est = enc_eco_coef_8(coef, cu_width_log2, cu_height_log2);

	return enc_est/* + coef_est*/;

}

int enc_bit_est_flag_skip_8(U2 slice_type, U2 tree_status, u8 umve_flag, u8 umve_idx, u8 affine_flag, u8 skip_idx, U3 cu_width_log2, U3 cu_height_log2, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], u8 num_of_hmvp_cand)
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
		enc_est += encode_skip_idx_cu_est((skip_idx), num_of_hmvp_cand, slice_type);
	}

	if (tree_status != TREE_C) {
		enc_est += enc_eco_cbf_est_skip_8(0, num_nz, 2, ipm, tree_status, cu_width_log2, cu_height_log2);
	}
	else if (!((2) == 0 && (ipm[0][0]) == 33
		&& (ipm[0][1]) == 0)) {
		enc_est += 2;

	}
	return enc_est;
}

void rate_est_8(U8 cu_width, U8 cu_height, u8 tree_status, u8 cons_pred_mode,u8 num_refp_cnt_for_rate,
	s64 lambda_y_for_re, int num_nz_for_re, u8 mode_type_for_re, u8 curr_mvr_for_re, U1 is_intra_for_re,
	MD_FW* md_fw_ptr, MD_COM_MODE* mod_info_curr_ptr,
	int md_fw_ptr_refp_ptr_for_re[17][2], S3 mod_info_refi_for_re[2], s16 mod_info_mvd_for_re[2][2], s16 mod_info_affine_mvd_for_re[2][4][2], u8 mod_info_mpm_for_re[4][2], S8 mod_info_ipm_for_re[4][2],
	pel qcoef_for_re[8][8],
	u8* mode_type_for_up, u8* curr_mvr_for_up, U1* is_intra_for_up,
	S3 mod_info_refi_for_up[2], s16 mod_info_mvd_for_up[2][2], u8 mod_info_mpm_for_up[4][2], S8 mod_info_ipm_for_up[4][2],
	s64 flag_cost[2])
{
#pragma HLS INLINE
	flag_cost[0] = 0;

	int num_nz_re[4][3];
	cu_nz_cln(num_nz_re);
	num_nz_re[0][0] = num_nz_for_re;
#if ENABLE_BFRAME
	U16 flag_est = enc_bit_est_flag_8(md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr_for_re, tree_status, cons_pred_mode, mod_info_curr_ptr->cu_mode,
		mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, 1, mod_info_curr_ptr->skip_idx, mod_info_refi_for_re, 
		mod_info_curr_ptr->mvp_from_hmvp_flag, mod_info_curr_ptr->smvd_flag, /*md_fw_ptr->num_refp[0],*/
		num_refp_cnt_for_rate, mod_info_affine_mvd_for_re, mod_info_mvd_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2,
		num_nz_re, mod_info_ipm_for_re, md_fw_ptr->cur_ptr, md_fw_ptr_refp_ptr_for_re, 8, mod_info_curr_ptr->mpm, is_intra_for_re);
#else
	U16 flag_est = enc_bit_est_flag_8(md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr_for_re, tree_status, cons_pred_mode, 
		mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag,
		1, mod_info_curr_ptr->skip_idx, mod_info_refi_for_re, mod_info_curr_ptr->mvp_from_hmvp_flag, mod_info_curr_ptr->smvd_flag, /*md_fw_ptr->num_refp,*/ 
		num_refp_cnt_for_rate, mod_info_affine_mvd_for_re, mod_info_mvd_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re,
		mod_info_ipm_for_re, md_fw_ptr->cur_ptr, md_fw_ptr_refp_ptr_for_re, 8, mod_info_curr_ptr->mpm, is_intra_for_re);
#endif
	U16 coef_est = enc_eco_coef_8(qcoef_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2);
	U16 bit_cnt = flag_est + coef_est;
	flag_cost[0] += (bit_cnt * lambda_y_for_re + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;

	if (is_intra_for_re == 0) {
		if (mode_type_for_re == 0) {
			flag_cost[1] = 0;
			flag_est = enc_bit_est_flag_skip_8(md_fw_ptr->slice_type, tree_status, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, mod_info_curr_ptr->skip_idx, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re, 8);
			flag_cost[1] += (flag_est * lambda_y_for_re + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
		}
	}

	*curr_mvr_for_up = curr_mvr_for_re;
	*mode_type_for_up = mode_type_for_re;
	*is_intra_for_up = is_intra_for_re;
	memcpy_1d<S3, 2>(mod_info_refi_for_re, mod_info_refi_for_up);
	memcpy_2d<s16, 2, 2>(mod_info_mvd_for_re, mod_info_mvd_for_up);
	memcpy_2d<u8, 4, 2>(mod_info_mpm_for_re, mod_info_mpm_for_up);
	memcpy_2d<S8, 4, 2>(mod_info_ipm_for_re, mod_info_ipm_for_up);
}

void rate_est_azb_8(U8 cu_width, U8 cu_height, u8 tree_status, u8 cons_pred_mode,
	s64 lambda_y_for_re, int num_nz_for_re, u8 mode_type_for_re, u8 curr_mvr_for_re, U1 is_intra_for_re,
	MD_FW* md_fw_ptr, MD_COM_MODE* mod_info_curr_ptr,
	int md_fw_ptr_refp_ptr_for_re[17][2], S3 mod_info_refi_for_re[2], s16 mod_info_mvd_for_re[2][2], s16 mod_info_affine_mvd_for_re[2][4][2], u8 mod_info_mpm_for_re[4][2], S8 mod_info_ipm_for_re[4][2],
	pel qcoef_for_re[64],
	int* num_nz_for_up, u8* mode_type_for_up, u8* curr_mvr_for_up, U1* is_intra_for_up,
	S3 mod_info_refi_for_up[2], s16 mod_info_mvd_for_up[2][2], u8 mod_info_mpm_for_up[4][2], S8 mod_info_ipm_for_up[4][2],
	s64 flag_cost[2])
{
	flag_cost[0] = 0;

	int num_nz_re[4][3];
	cu_nz_cln(num_nz_re);
	num_nz_re[0][0] = num_nz_for_re;
#if ENABLE_BFRAME
	U16 flag_est = enc_bit_est_flag_8(md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr_for_re, tree_status, cons_pred_mode, mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, 1, mod_info_curr_ptr->skip_idx, mod_info_refi_for_re, mod_info_curr_ptr->mvp_from_hmvp_flag, mod_info_curr_ptr->smvd_flag, md_fw_ptr->num_refp[0], mod_info_affine_mvd_for_re, mod_info_mvd_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re, md_fw_ptr->cur_ptr, md_fw_ptr_refp_ptr_for_re, 8, mod_info_curr_ptr->mpm, is_intra_for_re);
#else
	U16 flag_est = enc_bit_est_flag_8(md_fw_ptr->slice_type, md_fw_ptr->f_scu, curr_mvr_for_re, tree_status, cons_pred_mode, mod_info_curr_ptr->cu_mode, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, 1, mod_info_curr_ptr->skip_idx, mod_info_refi_for_re, mod_info_curr_ptr->mvp_from_hmvp_flag, mod_info_curr_ptr->smvd_flag, md_fw_ptr->num_refp, mod_info_affine_mvd_for_re, mod_info_mvd_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re, md_fw_ptr->cur_ptr, md_fw_ptr_refp_ptr_for_re, 8, mod_info_curr_ptr->mpm, is_intra_for_re);
#endif
	U16 coef_est = 0; // enc_eco_coef_8(qcoef_for_re, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2);
	U16 bit_cnt = flag_est + coef_est;
	flag_cost[0] += (bit_cnt * lambda_y_for_re + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;

	if (is_intra_for_re == 0) {
		if (mode_type_for_re == 0) {
			flag_cost[1] = 0;
			flag_est = enc_bit_est_flag_skip_8(md_fw_ptr->slice_type, tree_status, mod_info_curr_ptr->umve_flag, mod_info_curr_ptr->umve_idx, mod_info_curr_ptr->affine_flag, mod_info_curr_ptr->skip_idx, mod_info_curr_ptr->cu_width_log2, mod_info_curr_ptr->cu_height_log2, num_nz_re, mod_info_ipm_for_re, 8);
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

void memcpy_1d_2_1d_8(s16* des, s16* src, int n)
{
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=des cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=src complete dim=0
	int z;
	if (n == 32)
		for (z = 0; z < 32; z++)
#pragma HLS UNROLL
			des[z] = src[z];

	else if (n == 64)
		for (z = 0; z < 64; z++)
#pragma HLS UNROLL
			des[z] = src[z];
}
void memcpy_2d_2_1d_8(s16 des[64 * 64], s16 src[8][8], int w, int h)
{
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=des cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=src complete dim=0
	if (w == 4 && h == 8) {
		for (int i = 0; i < 8; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 4; j++)
#pragma HLS UNROLL
				des[i * 4 + j] = src[i][j];
	}
	else if (w == 8 && h == 4) {
		for (int i = 0; i < 4; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
#pragma HLS UNROLL
				des[i * 8 + j] = src[i][j];
	}
	else if (w == 8 && h == 8) {
		for (int i = 0; i < 8; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
#pragma HLS UNROLL
				des[i * 8 + j] = src[i][j];
	}
}
void memcpy_2d_2_1d_8(u8 des[64 * 64], s16 src[8][8], int w, int h)
{
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=des cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=src complete dim=0
	if (w == 4 && h == 8) {
		for (int i = 0; i < 8; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 4; j++)
#pragma HLS UNROLL
				des[i * 4 + j] = 0 > src[i][j] ? 0 : (src[i][j] > 255 ? 255 : src[i][j]);
	}
	else if (w == 8 && h == 4) {
		for (int i = 0; i < 4; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
#pragma HLS UNROLL
				des[i * 8 + j] = 0 > src[i][j] ? 0 : (src[i][j] > 255 ? 255 : src[i][j]);
	}
	else if (w == 8 && h == 8) {
		for (int i = 0; i < 8; i++)
#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
#pragma HLS UNROLL
				des[i * 8 + j] = 0 > src[i][j] ? 0 : (src[i][j] > 255 ? 255 : src[i][j]);
	}
}
void update_rdo_luma_8(U7 w, U7 h,
	U1 valid_flag_for_up, s64 dist_pred_for_up, s32 dist_value, s64 cost_best_temp_for_up, int num_nz_for_up, u8 mode_type_for_up, U1 is_intra_for_up, u8 curr_mvr_for_up,
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr,
	S3 mod_info_refi_for_up[2], s16 mod_info_mvd_for_up[2][2], S14 mod_info_mv_for_up[2][2], u8 mod_info_mpm_for_up[4][2], S8 mod_info_ipm_for_up[4][2],
	s64 flag_cost[2], pel pred_for_update[8][8], pel rec_for_update[8][8], pel qcoef_for_update[8][8],
	s64* cost_best
#if USE_ROI_MODE_CONFIG
	,U1 valid_mode_config[3]
#endif
)
{
#pragma HLS INLINE
	w=8;
	h=8;
	s64 cost[2] = { 0 };
	s64 cost_pb_temp = 0;
	int bin_cnt_luma[1] = { 0 };
	U13 i;
	U13 j;
	s16 wxh = s16(w) * s16(h);
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	//if (!valid_flag_for_up) 
	//{
	//	for (i = 0; i < 3; i++)
	//		model_cnt->cbf[i] = model_cnt_inner->cbf[i];

	//	for (i = 0; i < 24; i++)
	//		model_cnt->run_rdoq[i] = model_cnt_inner->run_rdoq[i];

	//	for (i = 0; i < 24; i++)
	//		model_cnt->level[i] = model_cnt_inner->level[i];

	//	for (i = 0; i < 12; i++)
	//		model_cnt->last1[i] = model_cnt_inner->last1[i];

	//	for (i = 0; i < 22; i++)
	//		model_cnt->last2[i] = model_cnt_inner->last2[i];

	//	return;
	//}


#pragma HLS   ARRAY_PARTITION variable=best_info_ptr->mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=best_info_ptr->mvd complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mod_info_curr_ptr->mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mod_info_curr_ptr->mvd complete dim=0


#pragma HLS   ARRAY_PARTITION variable=flag_cost complete dim=0
#pragma HLS   ARRAY_PARTITION variable=cu_data_ptr->coef_y complete dim=0
#pragma HLS   ARRAY_PARTITION variable=cu_data_ptr->reco_y complete dim=0
#pragma HLS   ARRAY_PARTITION variable=qcoef_for_update complete dim=0
#pragma HLS   ARRAY_PARTITION variable=pred_for_update complete dim=0
#pragma HLS   ARRAY_PARTITION variable=rec_for_update complete dim=0

#pragma HLS ARRAY_PARTITION variable=flag_cost complete dim=0


	cu_nz_cln(mod_info_curr_ptr->num_nz);
	mod_info_curr_ptr->num_nz[0][0] = num_nz_for_up;

	if (is_intra_for_up || mod_info_curr_ptr->num_nz[0][0])
	{
		cost[0] = dist_value;
		cost[0] += flag_cost[0];
	}
	if (!is_intra_for_up && !mode_type_for_up) {
		cost[1] = dist_pred_for_up;
		cost[1] += flag_cost[1];

#if USE_ROI_MODE_CONFIG
		if (valid_mode_config[2] == 0 && mod_info_curr_ptr->cu_mode == 0)
			cost[0] = MAX_S64;

		if (valid_mode_config[1] == 0 && mod_info_curr_ptr->cu_mode == 1)
			cost[0] = MAX_S64;

		if (valid_mode_config[0] == 0 && (mod_info_curr_ptr->cu_mode == 2 || mod_info_curr_ptr->cu_mode == 3))
			cost[0] = MAX_S64;
#endif

	}

	if (is_intra_for_up == 0) {
		if (mod_info_curr_ptr->num_nz[0][0]) {
			//if (cost[0] < cost_best_temp_for_up) {
				cost_best_temp_for_up = cost[0];
			//}
		}
		/*if (mode_type_for_up == 0) {
			if (cost[1] < cost_best_temp_for_up) {
				cost_best_temp_for_up = cost[1];
				mod_info_curr_ptr->cu_mode = MODE_SKIP;
				mod_info_curr_ptr->tb_part = (SIZE_2Nx2N);
				for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
					for (j = 0; j < 3; j++) {
#pragma HLS UNROLL
						mod_info_curr_ptr->num_nz[i][j] = 0;
					}
				}
			}
		}*/

		U2 lidx;

		if (cost_best_temp_for_up < *cost_best) {


			best_info_ptr->cu_mode = mod_info_curr_ptr->cu_mode;
			best_info_ptr->pb_part = mod_info_curr_ptr->pb_part;
			best_info_ptr->tb_part = mod_info_curr_ptr->tb_part;
			best_info_ptr->umve_flag = mod_info_curr_ptr->umve_flag;
			*cost_best = cost_best_temp_for_up;
			if (best_info_ptr->cu_mode != 0) {
				if (best_info_ptr->cu_mode == 2 || best_info_ptr->cu_mode == 3) {
					best_info_ptr->mvr_idx = 0;
				}
				else {
					best_info_ptr->mvp_from_hmvp_flag = mod_info_curr_ptr->mvp_from_hmvp_flag;
					best_info_ptr->mvr_idx = curr_mvr_for_up;
				}
				best_info_ptr->refi[0] = mod_info_refi_for_up[0];
				best_info_ptr->refi[1] = mod_info_refi_for_up[1];
				for (lidx = 0; lidx < 2; lidx++) {
#pragma HLS UNROLL
					best_info_ptr->mv[lidx][0] = mod_info_mv_for_up[lidx][0];
					best_info_ptr->mv[lidx][1] = mod_info_mv_for_up[lidx][1];
					best_info_ptr->mvd[lidx][0] = mod_info_mvd_for_up[lidx][0];
					best_info_ptr->mvd[lidx][1] = mod_info_mvd_for_up[lidx][1];
				}
				best_info_ptr->smvd_flag = mod_info_curr_ptr->smvd_flag;
				best_info_ptr->affine_flag = mod_info_curr_ptr->affine_flag;

				//memcpy_2d_2_1d_8(best_info_ptr->pred_y, pred_for_update, w, h);

				if (best_info_ptr->cu_mode == 2) {
					if (best_info_ptr->umve_flag != 0) {
						best_info_ptr->umve_idx = mod_info_curr_ptr->umve_idx;
					}
					else {
						best_info_ptr->skip_idx = mod_info_curr_ptr->skip_idx;
					}
				}
				else {
					if (best_info_ptr->cu_mode == 3) {
						if (best_info_ptr->umve_flag) {
							best_info_ptr->umve_idx = mod_info_curr_ptr->umve_idx;
						}
						else {
							best_info_ptr->skip_idx = mod_info_curr_ptr->skip_idx;
						}
					}
				}

				for (U3 i_numnz = 0; i_numnz < 4; i_numnz++) {
#pragma HLS UNROLL
					best_info_ptr->num_nz[i_numnz][0] = best_info_ptr->cu_mode == 2 ? 0 : mod_info_curr_ptr->num_nz[i_numnz][0];
				}
				for(i = 0; i < h; i++)
#pragma HLS UNROLL
					for(j = 0; j < w; j++)
#pragma HLS UNROLL
						cu_data_ptr->coef_y[i][j] = best_info_ptr->cu_mode == 2 ? 0 : qcoef_for_update[i][j];
			}

			U1 b_num_nz = mod_info_curr_ptr->num_nz[0][0] == 0;
			if (b_num_nz)
			{
				for (int i = 0; i < h; i++)
#pragma HLS UNROLL
					for (int j = 0; j < w; j++)
#pragma HLS UNROLL
						cu_data_ptr->reco_y[i][j] = pred_for_update[i][j];
			}
			else
			{
				for (int i = 0; i < h; i++)
#pragma HLS UNROLL
					for (int j = 0; j < w; j++)
#pragma HLS UNROLL
						cu_data_ptr->reco_y[i][j] = rec_for_update[i][j];
			}
		}
	}
	else {
		cost_pb_temp += cost[0];

		if (cost_pb_temp < *cost_best) {

		
			best_info_ptr->cu_mode = MODE_INTRA;
			best_info_ptr->affine_flag = 0;
			*cost_best = cost_pb_temp;
			//memcpy_1d_2_1d_8(best_info_ptr->coef_y, qcoef_for_update, wxh);
			//memcpy_2d_2_1d_8(best_info_ptr->rec_y, rec_for_update, w, h);

			for (i = 0; i < h; i++) 
			{
#pragma HLS UNROLL
				for (j = 0; j < w; j++)
				{
#pragma HLS UNROLL
					cu_data_ptr->coef_y[i][j] = qcoef_for_update[i][j];
					cu_data_ptr->reco_y[i][j] = rec_for_update[i][j];
				}
			}

#pragma HLS   ARRAY_PARTITION variable=best_info_ptr->mpm complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mod_info_curr_ptr->mpm complete dim=0
#pragma HLS   ARRAY_PARTITION variable=best_info_ptr->ipm complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mod_info_curr_ptr->ipm complete dim=0
#pragma HLS   ARRAY_PARTITION variable=best_info_ptr->num_nz complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mod_info_curr_ptr->num_nz complete dim=0
			for (i = 0; i < 4; ++i) {
#pragma HLS UNROLL
				for (j = 0; j < 2; ++j) {
#pragma HLS UNROLL
					best_info_ptr->mpm[i][j] = mod_info_mpm_for_up[i][j];
					best_info_ptr->ipm[i][j] = mod_info_ipm_for_up[i][j];
				}
			}
			for (i = 0; i < 4; ++i) {
#pragma HLS UNROLL
				for (j = 0; j < 3; ++j) {
#pragma HLS UNROLL
					best_info_ptr->num_nz[i][j] = mod_info_curr_ptr->num_nz[i][j];
				}
			}
		}
	}
}

void Ipred_vert_8(U8 w, U8 h, pel src_up[8*2+3],  s16 pred_cache_16x16[8][8], s16 pred_1[8][8])
{
#pragma HLS ARRAY_PARTITION variable = src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable = pred_cache_16x16 complete dim=2
#pragma HLS ARRAY_PARTITION variable = pred_1 complete dim=2

	int i, j;

	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
			for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
				if (j < w)
				{
					pred_cache_16x16[i][j] = src_up[j];
					pred_1[i][j] = src_up[j];
				}
			}
	}
}
void Ipred_hor_8(U8 w, U8 h, pel src_le[8*2+3],  s16 pred_cache_16x16[8][8], s16 pred_1[8][8])
{
#pragma HLS ARRAY_PARTITION variable = src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable = pred_cache_16x16 complete dim=2
#pragma HLS ARRAY_PARTITION variable = pred_1 complete dim=2

	int i, j;

	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < 8; j++) {

			if (j < w)
			{
				pred_cache_16x16[i][j] = src_le[i];
				pred_1[i][j] = src_le[i];
			}
		}	
	}
}
void Ipred_dc_8(U3 cu_width_log2_c, U3 cu_height_log2_c, U8 w, U8 h, pel src_le[8*2+3], pel src_up[8*2+3], U4 bit_depth, u16 avail_cu,  s16 pred_cache_16x16[8][8], s16 pred_1[8][8])
{
#pragma HLS ARRAY_PARTITION variable = src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable = src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable = pred_cache_16x16 complete dim=2
#pragma HLS ARRAY_PARTITION variable = pred_1 complete dim=2
	int dc = 0;
	//	int wh;
	int i;
	int j;
	int offset_s1 = 0;
	int s_src2 = w;
	int s_diff = w;

	if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
			if (i < h)
				dc += (src_le[i]);
		}
		if (((avail_cu) & 1 << 0) == 1 << 0) {
			for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
				if (j < w)
					dc += (src_up[j]);
			}
			if (w == h)
				dc = (dc + ((16) >> 1)) * (4096 / (16)) >> 12;
			else
				dc = (dc + ((12) >> 1)) * (4096 / (12)) >> 12;
		}
		else {
			dc = (dc + (h >> 1)) >> cu_height_log2_c;// (com_tbl_log2[h]);
		}
	}
	else if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j < w)
				dc += (src_up[j]);
		}
		dc = (dc + (w >> 1)) >> cu_width_log2_c;// (com_tbl_log2[w]);
	}
	else {
		dc = 1 << (bit_depth - 1);
	}
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < 8; j++) {
			if (j < w) {
				pred_cache_16x16[i][j] = dc;
				pred_1[i][j] = dc;
			}
		}
	}
}
void Ipred_plane_Chroma8(U3 cu_width_log2_c, U3 cu_height_log2_c, U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s16 pred_cache_16x16[8][8], s16 pred_1[8][8])
{
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_1 complete dim=2

	int coef_h = 0;
	int coef_v = 0;
	int a;
	int b;
	int c;
	int x;
	int y;
	int w2 = w >> 1;
	int h2 = h >> 1;
	static s8 ib_mult[5] = { (13), (17), (5), (11), (23) };
	static s8 ib_shift[5] = { (7), (10), (11), (15), (19) };
#pragma HLS ARRAY_PARTITION variable=ib_mult complete dim=0
#pragma HLS ARRAY_PARTITION variable=ib_shift complete dim=0
	//s8 idx_w = (com_tbl_log2[w]) - 2;
	//s8 idx_h = (com_tbl_log2[h]) - 2;
	s8 idx_w = cu_width_log2_c - 2;
	s8 idx_h = cu_height_log2_c - 2;
	s8 im_h;
	s8 is_h;
	s8 im_v;
	s8 is_v;
	int temp;
	int temp2;

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
	int pred_cache;

	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];

	for (x = 1; x < 5; x++) {
#pragma HLS UNROLL
		if (x < w2 + 1)
			coef_h += x * ((src_up[w2 + x]) - (src_up[w2 - x]));
	}

	for (y = 1; y < 5; y++) {
#pragma HLS UNROLL
		if (y < h2 + 1)
			coef_v += y * ((src_le[h2 + y]) - (src_le[h2 - y]));
	}
	a = (src_le[h] + src_up[w]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;
	int i, j;
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		{
			temp2 = temp;
			for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
				if (j < w)
				{
					pred_cache = ((temp2 + b * j) >> 5);
					pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));
					pred_1[i][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));

				}
			}
			temp += c;
		}
	}

}

void Ipred_bi_8(U3 cu_width_log2_c, U3 cu_height_log2_c, U8 w, U8 h, pel src_le[2*8+3], pel src_up[2*8+3], U4 bit_depth,  s16 pred_cache_16x16[8][8], s16 pred_1[8][8])
{
#pragma HLS ARRAY_PARTITION variable = src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable = src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable = pred_cache_16x16 complete dim=2
#pragma HLS ARRAY_PARTITION variable = pred_1 complete dim=2

	int x;
	int y;
	s8 ishift_x = cu_width_log2_c;// com_tbl_log2[w];
	s8 ishift_y = cu_height_log2_c;// com_tbl_log2[h];
	s8 ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	s8 ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy;
	int tmp;
	int predx;
	static s16 ref_up[8];
	static s16 ref_le[8];
	static s16 up[8];
	static s16 le[8];
	static s16 wy[8];
	int wc;
	s8 tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };

#pragma HLS ARRAY_PARTITION variable = ref_up complete dim=0
#pragma HLS ARRAY_PARTITION variable = ref_le complete dim=0
#pragma HLS ARRAY_PARTITION variable = up complete dim=0
#pragma HLS ARRAY_PARTITION variable = le complete dim=0
#pragma HLS ARRAY_PARTITION variable = wy complete dim=0

	int offset_s1 = 0;
	int offset_s2 = 0;

	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	wc = tbl_wc[wc];
	a = (src_up[w - 1]);
	b = (src_le[h - 1]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 8; x++) {
#pragma HLS UNROLL
		if (x < w) {
			up[x] = b - src_up[x];
			ref_up[x] = src_up[x];
			ref_up[x] <<= ishift_y;
		}
	}
	tmp = 0;
	for (y = 0; y < 8; y++) {
#pragma HLS UNROLL
		if (y < h) {
			le[y] = a - src_le[y];
			ref_le[y] = src_le[y];
			ref_le[y] <<= ishift_x;
			wy[y] = wt * y;
		}
	}
	u8 i, j;
	int temp;
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
			for (j = 0; j < 8; j++) {
				if (j < w)
				{
					predx = ref_le[i] + (j + 1) * le[i];
					wxy = j*wy[i];
					temp = ((predx << ishift_y) + ((ref_up[j] +(i+1)*up[j])<< ishift_x) + wxy + offset) >> ishift_xy;
					pred_cache_16x16[i][j] = (((0 >(((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));
					pred_1[i][j] = (((0 > (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));

				}
			}
	}

}
void ipred_ang1_8(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s8 ipm, pel orgY[64], s16 pred_cache_16x16[8][8], s16 coef_tmp_16x16[8][8]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#pragma HLS PIPELINE
#if MIPF

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
	//#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	u8 i, j;
	int t_dx, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int x = 2147483647;
#pragma HLS array_partition variable = orgY complete dim=0
#pragma HLS array_partition variable = pred_cache_16x16 complete dim=0
#pragma HLS array_partition variable = coef_tmp_16x16 complete dim=0
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
#pragma HLS array_partition variable = ipred_dxdy complete dim=0
	for (j = 0; j < 8; j++)
	{
#pragma HLS UNROLL
		if (j < h) {
			//#pragma HLS PIPELINE
			//#pragma HLS LOOP_TRIPCOUNT max=8
			for (i = 0; i < 8; i++)
			{
#pragma HLS UNROLL
				//#pragma HLS LOOP_TRIPCOUNT max=8
				if (i < w)
				{
					/* case x-line */
					t_dx = (j + 1) * ipred_dxdy[ipm - 3][0] >> 10;
					offset = ((j + 1) * ipred_dxdy[ipm - 3][0] >> 5) - (t_dx << 5);
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
					//filter = (tbl_filt_list[filter_idx] + offset)[0];
					//temp_pel = (pel)((src_up[pn_n1 + 3] * filter[0] + src_up[p + 3] * filter[1] +
					//	src_up[pn + 3] * filter[2] + src_up[pn_p2 + 3] * filter[3] +
					//	filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1


				if (filter_idx == 0)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_extra_smooth[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_extra_smooth[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 1)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_semi_sharp[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_semi_sharp[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 2)
					temp_pel = (pel)((src_up[pn_n1 + 3] * tbl_mc_c_coeff_hp[offset][0] + src_up[p + 3] * tbl_mc_c_coeff_hp[offset][1] +
						src_up[pn + 3] * tbl_mc_c_coeff_hp[offset][2] + src_up[pn_p2 + 3] * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1
				else if (filter_idx == 3)
					temp_pel = (pel)((src_up[pn_n1 + 3] * com_tbl_ipred_adi_bilinear[offset][0] + src_up[p + 3] * com_tbl_ipred_adi_bilinear[offset][1] +
						src_up[pn + 3] * com_tbl_ipred_adi_bilinear[offset][2] + src_up[pn_p2 + 3] * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// y == -1

					temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
					pred_cache_16x16[j][i] = temp_pel;
					coef_tmp_16x16[j][i] = orgY[j * 8 + i] - pred_cache_16x16[j][i];
#else

					pred_cache_16x16[j][i] = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + (src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
					//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
					coef_tmp_16x16[j][i] = orgY[j * 8 + i] - pred_cache_16x16[j][i];
#endif
				}

			}
		}
	}

}

void ipred_ang2_8(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s8 ipm, pel orgY[64], s16 pred_cache_16x16[8][8], s16 coef_tmp_16x16[8][8]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#pragma HLS PIPELINE
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
	const s16* filter;
	pel temp_pel = (pel)0;
#endif


	static const s16 ipred_dxdy[8][2] = { { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=orgY complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=0
#pragma HLS ARRAY_PARTITION variable=coef_tmp_16x16 complete dim=0
	u8 i, j;
	int t_dy, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int y = 2147483647;
	for (j = 0; j < 8; j++)
	{
#pragma HLS UNROLL
		if (j < h) {

			//#pragma HLS LOOP_TRIPCOUNT max=8
			for (i = 0; i < 8; i++)
			{
#pragma HLS UNROLL
				//#pragma HLS LOOP_TRIPCOUNT max=8
				if (i < w)
				{
					t_dy = (i + 1) * ipred_dxdy[ipm - 25][1] >> 10;
					offset = ((i + 1) * ipred_dxdy[ipm - 25][1] >> 5) - (t_dy << 5);
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
					//filter = (tbl_filt_list[filter_idx] + offset)[0];
					//temp_pel = (pel)((src_le[pn_n1 + 3] * filter[0] + src_le[p + 3] * filter[1] +
					//	src_le[pn + 3] * filter[2] + src_le[pn_p2 + 3] * filter[3] +
					//	filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]); // x == -1

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
					coef_tmp_16x16[j][i] = orgY[(j * 8) + i] - pred_cache_16x16[j][i];

#else

					pred_cache_16x16[j][i] = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
					//coef_tmp_16x16[j][i] = orgY[(j << 6) + i] - pred_cache_16x16[j][i];
					coef_tmp_16x16[j][i] = orgY[(j * 8) + i] - pred_cache_16x16[j][i];
#endif


				}
			}
		}
	}

}

void Ipred_8_diff(U8 w, U8 h, pel orgY[64], s16 pred_cache_16x16[8][8], s16 coef_tmp_16x16[8][8]) {
	int i, j;
#pragma HLS array_partition variable = orgY complete dim=1
#pragma HLS array_partition variable = pred_cache_16x16 complete dim=2
#pragma HLS array_partition variable = coef_tmp_16x16 complete dim=2
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		if (i < h)
		{
			for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
				if (j < w)
				{
					coef_tmp_16x16[i][j] = orgY[i * 8 + j] - pred_cache_16x16[i][j];
				}
			}
		}

	}
}


void Ipred_vert_8_pred(U8 w, U8 h, pel src_up[8 * 2 + 3], s16 pred_cache_16x16[8][8])
{
	int i, j;
#pragma HLS array_partition variable = src_up complete dim=1
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j < w && i < h)
			{
				pred_cache_16x16[i][j] = src_up[j];
			}
		}
	}
}
void Ipred_hor_8_pred(U8 w, U8 h, pel src_le[8 * 2 + 3], s16 pred_cache_16x16[8][8])
{
	int i, j;
#pragma HLS array_partition variable = src_le complete dim=1
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j < w && i < h)
			{
				pred_cache_16x16[i][j] = src_le[i];
			}
		}
	}
}
void Ipred_dc_8_pred(U8 w, U8 h, pel src_le[8 * 2 + 3], pel src_up[8 * 2 + 3], U4 bit_depth, u16 avail_cu, s16 pred_cache_16x16[8][8])
{
	int dc = 0;
	//	int wh;
	int i;
	int j;
	int offset_s1 = 0;
	int s_src2 = w;
	int s_diff = w;
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0

	if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
			if (i < h)
				dc += (src_le[i]);
		}
		if (((avail_cu) & 1 << 0) == 1 << 0) {
			for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
				if (j < w)
					dc += (src_up[j]);
			}
			if (w == h)
				dc = (dc + ((16) >> 1)) * (4096 / (16)) >> 12;
			else
				dc = (dc + ((12) >> 1)) * (4096 / (12)) >> 12;
		}
		else {
			dc = (dc + (h >> 1)) >> (com_tbl_log2[h]);
		}
	}
	else if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j < w)
				dc += (src_up[j]);
		}
		dc = (dc + (w >> 1)) >> (com_tbl_log2[w]);
	}
	else {
		dc = 1 << (bit_depth - 1);
	}
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j < w && i < h) {
				pred_cache_16x16[i][j] = dc;
			}
		}
	}
}
void Ipred_plane_8_pred(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s16 pred_cache_16x16[8][8])
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
	static s8 ib_mult[5] = { (13), (17), (5), (11), (23) };
	static s8 ib_shift[5] = { (7), (10), (11), (15), (19) };
	s8 idx_w = (com_tbl_log2[w]) - 2;
	s8 idx_h = (com_tbl_log2[h]) - 2;
	s8 im_h;
	s8 is_h;
	s8 im_v;
	s8 is_v;
	int temp;
	int temp2;

	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int s_src2 = w;
	int s_diff = w;
	int pred_cache;
#pragma HLS array_partition variable = com_tbl_log2 complete dim=0
#pragma HLS array_partition variable = ib_mult complete dim=0
#pragma HLS array_partition variable = ib_shift complete dim=0
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];
	//rsrc = src_up + (w2 - 1);
	//rsrc = src_up;
	for (x = 1; x < 5; x++) {
#pragma HLS UNROLL
		if (x < w2 + 1)
			coef_h += x * ((src_up[w2 + x]) - (src_up[w2 - x]));
	}
	//rsrc = src_le + (h2 - 1);
	//rsrc = src_le;
	for (y = 1; y < 5; y++) {
#pragma HLS UNROLL
		if (y < h2 + 1)
			coef_v += y * ((src_le[h2 + y]) - (src_le[h2 - y]));
	}
	a = (src_le[h] + src_up[w]) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;
	int i, j;
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		temp2 = temp;
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			pred_cache = ((temp2 + b * j) >> 5);
			pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))) ? 0 : (((1 << bit_depth) - 1 < (pred_cache) ? (1 << bit_depth) - 1 : (pred_cache))))));
		}
		temp += c;
	}

}

void Ipred_bi_8_pred(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s16 pred_cache_16x16[8][8])
{
	int x;
	int y;
	s8 ishift_x = com_tbl_log2[w];
	s8 ishift_y = com_tbl_log2[h];
	s8 ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	s8 ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy;
	int tmp;
	int predx;
	static s16 ref_up[128 >> 1];
	static s16 ref_le[128 >> 1];
	static s16 up[128 >> 1];
	static s16 le[128 >> 1];
	static s16 wy[128 >> 1];
	int wc;
	s8 tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };

	int offset_s1 = 0;
	int offset_s2 = 0;
#pragma HLS array_partition variable = com_tbl_log2 complete dim=0
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
#pragma HLS array_partition variable = ref_up complete dim=0
#pragma HLS array_partition variable = ref_le complete dim=0
#pragma HLS array_partition variable = up complete dim=0
#pragma HLS array_partition variable = le complete dim=0
#pragma HLS array_partition variable = wy complete dim=0
	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	wc = tbl_wc[wc];
	a = (src_up[w - 1]);
	b = (src_le[h - 1]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 8; x++) {
#pragma HLS UNROLL
		if (x < w) {
			up[x] = b - src_up[x];
			ref_up[x] = src_up[x];
			ref_up[x] <<= ishift_y;
		}
	}
	tmp = 0;
	for (y = 0; y < 8; y++) {
#pragma HLS UNROLL
		if (y < h) {
			le[y] = a - src_le[y];
			ref_le[y] = src_le[y];
			ref_le[y] <<= ishift_x;
			wy[y] = wt * y;
		}
	}
	u8 i, j;
	int temp;
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL

		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (j < w && i < h)
			{
				predx = ref_le[i] + (j + 1) * le[i];
				//ref_up[j] += up[j];
				wxy = j * wy[i];
				temp = ((predx << ishift_y) + ((ref_up[j] + (i + 1) * up[j]) << ishift_x) + wxy + offset) >> ishift_xy;
				pred_cache_16x16[i][j] = (((0 > (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))) ? 0 : (((1 << bit_depth) - 1 < (temp) ? (1 << bit_depth) - 1 : (temp))))));

			}
		}
	}

}

void ipred_ang1_8_pred(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s8 ipm, s16 pred_cache_16x16[8][8]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#if MIPF

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
#endif
#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;
	//const s16* filter;
#endif
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_bilinear complete dim=0
#pragma HLS   ARRAY_PARTITION variable=tbl_mc_c_coeff_hp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_extra_smooth complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_semi_sharp complete dim=0
	static const s16 ipred_dxdy[9][2] = { { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) }, { (372), (2816) }, { (256), (4096) }, { (128), (8192) } };
	//#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	u8 i, j;
	int t_dx, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int x = 2147483647;
#pragma HLS array_partition variable = src_le complete dim=0
#pragma HLS array_partition variable = src_up complete dim=0
	for (j = 0; j < 8; j++)
	{
#pragma HLS UNROLL
		for (i = 0; i < 8; i++)
		{
#pragma HLS UNROLL

			if (i < w && j < h)
			{
				/* case x-line */
				t_dx = (j + 1) * ipred_dxdy[ipm - 3][0] >> 10;
				offset = ((j + 1) * ipred_dxdy[ipm - 3][0] >> 5) - (t_dx << 5);
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
				s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];
				s32 buffer1 = src_up[pn_n1 + 3] * data1;
				s32 buffer2 = src_up[p + 3] * data2;
				s32 buffer3 = src_up[pn + 3] * data3;
				s32 buffer4 = src_up[pn_p2 + 3] * data4;
				pel tmp = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				pel tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
				pred_cache_16x16[j][i] = tmp1;
#else
				pred_cache_16x16[j][i] = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + (src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
#endif
			}

		}
	}
}


void ipred_ang2_8_pred(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s8 ipm, s16 pred_cache_16x16[8][8]
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
	const s16* filter;
#endif

#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_bilinear complete dim=0
#pragma HLS   ARRAY_PARTITION variable=tbl_mc_c_coeff_hp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_extra_smooth complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_semi_sharp complete dim=0
	static const s16 ipred_dxdy[8][2] = { { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	u8 i, j;
	int t_dy, offset;
	int p, pn, pn_n1, pn_p2, pos_max;
	int y = 2147483647;
	for (j = 0; j < 8; j++)
	{
#pragma HLS UNROLL
		for (i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			//#pragma HLS LOOP_TRIPCOUNT max=8
			if (i < w && j < h)
			{
				t_dy = (i + 1) * ipred_dxdy[ipm - 25][1] >> 10;
				offset = ((i + 1) * ipred_dxdy[ipm - 25][1] >> 5) - (t_dy << 5);
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

				s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];
				s32 buffer1 = src_le[pn_n1+3] * data1;
				s32 buffer2 = src_le[p+3] * data2;
				s32 buffer3 = src_le[pn+3] * data3;
				s32 buffer4 = src_le[pn_p2+3] * data4;
				pel tmp = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				pel tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
				pred_cache_16x16[j][i] = tmp1;
#else

				pred_cache_16x16[j][i] = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
#endif
			}
		}
	}

}


void ipred_ang3_8_pred(U8 w, U8 h, pel src_le[2 * 8 + 3], pel src_up[2 * 8 + 3], U4 bit_depth, s8 ipm, s16 pred_cache_16x16[8][8]
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
#endif

	static const s16 ipred_dxdy[11][2] = { { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) } };
	////#pragma HLS  HLS ARRAY_PARTITION variable=ipred_dxdy complete dim=0
	u8 i, j;
	int offset, offset_x, offset_y, xx, yy, t_dx, t_dy;
	int p, pn, pn_n1, pn_p2, pos_max;
#pragma HLS   ARRAY_PARTITION variable=ipred_dxdy complete dim=0
#pragma HLS   ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS   ARRAY_PARTITION variable=src_up complete dim=0
	int xxyya, xxyy;
	pel temp_pel0 = 0;
	pel temp_pel1 = 0;
	for (j = 0; j < 8; j++)
	{
#pragma HLS   UNROLL
		for (i = 0; i < 8; i++)
		{
#pragma HLS   UNROLL
			t_dx = (j + 1) * ipred_dxdy[ipm - 13][0] >> 10;
			offset_x = ((j + 1) * ipred_dxdy[ipm - 13][0] >> 5) - (t_dx << 5);
			t_dx = -t_dx;
			xx = i + t_dx;
			t_dy = (i + 1) * ipred_dxdy[ipm - 13][1] >> 10;
			offset_y = ((i + 1) * ipred_dxdy[ipm - 13][1] >> 5) - (t_dy << 5);
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

				pel src_pn_n1 = (yy <= -1) ? src_up[pn_n1 + 3] : src_le[pn_n1 + 3];
				pel src_p = (yy <= -1) ? src_up[p + 3] : src_le[p + 3];
				pel src_pn = (yy <= -1) ? src_up[pn + 3] : src_le[pn + 3];
				pel src_pn_p2 = (yy <= -1) ? src_up[pn_p2 + 3] : src_le[pn_p2 + 3];

				s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];

				s32 buffer1 = src_pn_n1 * data1;
				s32 buffer2 = src_p * data2;
				s32 buffer3 = src_pn * data3;
				s32 buffer4 = src_pn_p2 * data4;
				pel tmp = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				pel tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
				pred_cache_16x16[j][i] = tmp1;
#else
			pel temp_pel0 = 0;
				temp_pel0 = ((src_up[pn_n1 + 3]) * (32 - offset) + (src_up[p + 3]) * (64 - offset) + ((
					int)src_up[pn + 3]) * (32 + offset) + (src_up[pn_p2 + 3]) * offset + 64) >> 7;
				temp_pel1 = ((src_le[pn_n1 + 3]) * (32 - offset) + (src_le[p + 3]) * (64 - offset) + (src_le[pn + 3]) * (32 + offset) + (src_le[pn_p2 + 3]) * offset + 64) >> 7;
				pred_cache_16x16[j][i] = temp_pel;
#endif
		}
	}
}

void com_mc_l_00_8(pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, pel pred[8][8], U7 w, U7 h, U1 bHpFilter)
{
	u8 i;
	u8 j;
	int offset_ref = 0;
	int offset_pred = 0;
	/*	if (bHpFilter) {
			gmv_x >>= 4;
			gmv_y >>= 4;
		}
		else */ {
		gmv_x >>= 2;
		gmv_y >>= 2;
	}
		offset_ref += gmv_y * s_ref + gmv_x;
		{
			for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 8
#pragma HLS PIPELINE
				for (j = 0; j < 8; j++) {
#pragma HLS UNROLL	
					if (j < w)
						pred[i][j] = ref[j + offset_ref];
				}
				offset_pred += s_pred;
				offset_ref += s_ref;
			}
		}
}

//void com_mc_l_0n_8(U8* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, pel pred[8][8], int w, int h, int bit_depth,
//	U1 bHpFilter)
//{
//	s16 tbl_mc_l_coeff_all_1[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };
//
//	U4 dy;
//	int offset_ref = 0;
//	int offset_pred = 0;
//	int i;
//	int j;
//	s32 pt, index;
///*	if (bHpFilter) {
//		dy = gmv_y & 15;
//		offset_ref += ((gmv_y >> 4) - 3) * s_ref + (gmv_x >> 4);
//	}
//	else */{
//		dy = gmv_y & 0x3;
//		offset_ref += ((gmv_y >> 2) - 3) * s_ref + (gmv_x >> 2);
//	}
//	//const s16 *coeff_ver = bHpFilter ? tbl_mc_l_coeff_hp[dy] : tbl_mc_l_coeff[dy];
//	//const s16 *coeff_ver = tbl_mc_l_coeff_all_1[bHpFilter][dy];
//	{
//
//		for (i = 0; i < h; i++) {
//
//#pragma HLS loop_tripcount max = 8
//			for (j = 0; j < w; j++) {
//
//#pragma HLS loop_tripcount max = 8
//				//pt = ((tbl_mc_l_coeff_all_1[0][dy][0]) * (ref[j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][1]) * (ref[s_ref + j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][2]) * (ref[s_ref * 2 + j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][3]) * (ref[s_ref * 3 + j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][4]) * (ref[s_ref * 4 + j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][5]) * (ref[s_ref * 5 + j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][6]) * (ref[s_ref * 6 + j + offset_ref]) + (
//				//	tbl_mc_l_coeff_all_1[0][dy][7]) * (ref[s_ref * 7 + j + offset_ref]) + (1 << 5)) >> 6;
//
//				index = j + offset_ref;
//				pt = 0;
//				for (u8 k = 0; k < 8; k++)
//				{
//					pt += tbl_mc_l_coeff_all_1[0][dy][k] * ref[index];
//					index += s_ref;
//				}
//
//				pt = (pt + (1 << 5)) >> 6;
//
//				pred[i][j] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
//					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
//			}
//			offset_ref += s_ref;
//			offset_pred += s_pred;
//		}
//	}
//}
//
//void com_mc_l_n0_8(U8* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, pel pred[8][8], int w, int h, int bit_depth,
//	U1 bHpFilter)
//{
//	s16 tbl_mc_l_coeff_all_2[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };
//
//	U4 dx;
//	int offset_ref = 0;
//	int offset_pred = 0;
//	u8 i;
//	u8 j;
//	s32 pt, index;
///*	if (bHpFilter) {
//		dx = gmv_x & 15;
//		offset_ref += (gmv_y >> 4) * s_ref + (gmv_x >> 4) - 3;
//	}
//	else */{
//		dx = gmv_x & 0x3;
//		offset_ref += (gmv_y >> 2) * s_ref + (gmv_x >> 2) - 3;
//	}
//	//const s16 *coeff_hor = bHpFilter ? tbl_mc_l_coeff_hp[dx] : tbl_mc_l_coeff[dx];
//	//const s16 *coeff_hor = tbl_mc_l_coeff_all_2[bHpFilter][dx];
//	{
//
//		for (i = 0; i < h; i++) {
//
//#pragma HLS loop_tripcount max = 8
//			for (j = 0; j < w; j++) {
//
//#pragma HLS loop_tripcount max = 8
//				pt = ((tbl_mc_l_coeff_all_2[0][dx][0]) * (ref[j + offset_ref]) +
//					(tbl_mc_l_coeff_all_2[0][dx][1]) * (ref[j + 1 + offset_ref]) +
//					(tbl_mc_l_coeff_all_2[0][dx][2]) * (ref[j + 2 + offset_ref]) + 
//					(tbl_mc_l_coeff_all_2[0][dx][3]) * (ref[j + 3 + offset_ref]) + 
//					(tbl_mc_l_coeff_all_2[0][dx][4]) * (ref[j + 4 + offset_ref]) + 
//					(tbl_mc_l_coeff_all_2[0][dx][5]) * (ref[j + 5 + offset_ref]) +
//					(tbl_mc_l_coeff_all_2[0][dx][6]) * (ref[j + 6 + offset_ref]) +
//					(tbl_mc_l_coeff_all_2[0][dx][7]) * (ref[j + 7 + offset_ref]) + (1 << 5)) >> 6;
//
//				index = j + offset_ref;
//				pt = 0;
//				for (u8 k = 0; k < 8; k++)
//				{
//					pt += tbl_mc_l_coeff_all_2[0][dx][k] * ref[index];
//					index += 1;
//				}
//
//
//				pt = (pt + (1 << 5)) >> 6;
//
//				pred[i][j] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
//					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
//			}
//			offset_ref += s_ref;
//			offset_pred += s_pred;
//		}
//	}
//}

static void com_mc_l_n0_0n_8(pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, pel pred[8][8], U7 w, U7 h, U4 bit_depth, U1 bHpFilter, U1 b_n0, const s16 tbl_mc_l_coeff_all_2[2][16][8])
{
	//s16 tbl_mc_l_coeff_all_2[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };

	U4 dxy;
	int offset_ref = 0;
	int offset_pred = 0;
	u8 i;
	u8 j;
	int stride = s_ref;
	s32 pt, index;
	if (b_n0) {
		dxy = gmv_x & 0x3;
		offset_ref += (gmv_y >> 2) * s_ref + (gmv_x >> 2) - 3;
		stride = 1;
	}
	else {
		dxy = gmv_y & 0x3;
		offset_ref += ((gmv_y >> 2) - 3) * s_ref + (gmv_x >> 2);
	}
	//const s16 *coeff_hor = bHpFilter ? tbl_mc_l_coeff_hp[dx] : tbl_mc_l_coeff[dx];
	//const s16 *coeff_hor = tbl_mc_l_coeff_all_2[bHpFilter][dx];
	{

		for (i = 0; i < h; i++) {

#pragma HLS loop_tripcount max = 8
			for (j = 0; j < w; j++) {
				index = j + offset_ref;
				pt = 0;
				for (u8 k = 0; k < 8; k++)
				{
#pragma HLS UNROLL
					pt += tbl_mc_l_coeff_all_2[0][dxy][k] * ref[index];
					index += stride;
				}


				pt = (pt + (1 << 5)) >> 6;

				pred[i][j] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));

			}
			offset_ref += s_ref;
			offset_pred += s_pred;
		}
	}
}

static void com_mc_l_nn_8(pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[8][8], U7 w, U7 h, U4 bit_depth,
	U1 bHpFilter, const s16 tbl_mc_l_coeff_all_3[2][16][8])
{
	static s32 buf[64 + 8][128 >> 1];
	//s8 tbl_mc_l_coeff_all_3[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };


#pragma HLS ARRAY_PARTITION variable=buf block factor=72 dim=1 partition
	u8 dx;
	u8 dy;
	int offset_ref = 0;
	int offset_pred = 0;
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = (1 << (shift2 - 1));
	static s32 ref_temp[8];
	u8 i, j, k;
	//s16 * b;
	s64 pt;

	/*	if (bHpFilter) {
			dx = gmv_x & 15;
			dy = gmv_y & 15;
			offset_ref += ((gmv_y >> 4) - 3) * s_ref + (gmv_x >> 4) - 3;
		}
		else */ {
		dx = gmv_x & 0x3;
		dy = gmv_y & 0x3;
		offset_ref += ((gmv_y >> 2) - 3) * s_ref + (gmv_x >> 2) - 3;
	}


#pragma HLS ARRAY_PARTITION variable=ref_temp complete dim=0
		{

			//b = buf;
			for (i = 0; i < h + 7; i++) {

#pragma HLS loop_tripcount max = 15
				ref_temp[0] = ref[offset_ref];
				ref_temp[1] = ref[1 + offset_ref];
				ref_temp[2] = ref[2 + offset_ref];
				ref_temp[3] = ref[3 + offset_ref];
				ref_temp[4] = ref[4 + offset_ref];
				ref_temp[5] = ref[5 + offset_ref];
				ref_temp[6] = ref[6 + offset_ref];
				for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 8
					ref_temp[7] = ref[j + 7 + offset_ref];
					buf[i][j] = 0;
					for (k = 0; k < 8; k++)
					{
#pragma HLS UNROLL
						buf[i][j] += tbl_mc_l_coeff_all_3[0][dx][k] * ref_temp[k];
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

#pragma HLS loop_tripcount max = 8
				for (j = 0; j < w; j++) {

#pragma HLS loop_tripcount max = 8
					pt = 0;
					for (k = 0; k < 8; k++)
					{
#pragma HLS UNROLL
						pt += tbl_mc_l_coeff_all_3[0][dy][k] * buf[i + k][j];
					}
					pt = (pt + add2) >> shift2;

					pred[i][j] = ((pel)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
						1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				}
				offset_pred += s_pred;
			}
		}
}

void com_tbl_mc_l_1_8(int ori_mv_x, int ori_mv_y, pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[8][8],
	U7 w, U7 h, U4 bit_depth)
{
	//add by luke
	int dx = ori_mv_x & 0x3;
	int dy = ori_mv_y & 0x3;

	static s16 tbl_mc_l_coeff_all[2][16][8] = { { { 0, 0, 0, (64), 0, 0, 0, 0 }, { ((-1)), (4), ((-10)), (57), (19), ((-7)), (3), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (3), ((-7)), (19), (57), ((-10)), (4), ((-1)) }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } }, { { 0, 0, 0, (64), 0, 0, 0, 0 }, { 0, (1), ((-3)), (63), (4), ((-2)), (1), 0 }, { ((-1)), (2), ((-5)), (62), (8), ((-3)), (1), 0 }, { ((-1)), (3), ((-8)), (60), (13), ((-4)), (1), 0 }, { ((-1)), (4), ((-10)), (58), (17), ((-5)), (1), 0 }, { ((-1)), (4), ((-11)), (52), (26), ((-8)), (3), ((-1)) }, { ((-1)), (3), ((-9)), (47), (31), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (45), (34), ((-10)), (4), ((-1)) }, { ((-1)), (4), ((-11)), (40), (40), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (34), (45), ((-11)), (4), ((-1)) }, { ((-1)), (4), ((-10)), (31), (47), ((-9)), (3), ((-1)) }, { ((-1)), (3), ((-8)), (26), (52), ((-11)), (4), ((-1)) }, { 0, (1), ((-5)), (17), (58), ((-10)), (4), ((-1)) }, { 0, (1), ((-4)), (13), (60), ((-8)), (3), ((-1)) }, { 0, (1), ((-3)), (8), (62), ((-5)), (2), ((-1)) }, { 0, (1), ((-2)), (4), (63), ((-3)), (1), 0 } } };


	if (dx == 0 && dy == 0) {
		com_mc_l_00_8(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, 0);
	}
	else if (dx != 0 && dy != 0) {
		com_mc_l_nn_8(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0, tbl_mc_l_coeff_all);
	}
	else
	{
		com_mc_l_n0_0n_8(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0, (dx != 0 && dy == 0), tbl_mc_l_coeff_all);
	}
}

void com_tbl_mc_l_1_8_ME(int ori_mv_x, int ori_mv_y, pel refwin_md[88][88], int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[8][8],
	U7 w, U7 h, U4 bit_depth)
{
	//add by luke
	int dx = ori_mv_x & 0x3;
	int dy = ori_mv_y & 0x3;

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
		com_mc_l_00_8(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, 0);
	}
	else if (dx != 0 && dy != 0) {
		com_mc_l_nn_8(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0, tbl_mc_l_coeff_all);
	}
	else
	{
		com_mc_l_n0_0n_8(ref, gmv_x, gmv_y, s_ref, s_pred, pred, w, h, bit_depth, 0, (dx != 0 && dy == 0), tbl_mc_l_coeff_all);
	}
}

void refwin_copy_line_8(U6 x, U6 y, pel refwin_md[512 * 512], pel refwin_line[16], pel refwin_line2[16])
{
#pragma HLS INLINE OFF
#pragma HLS array_partition variable=refwin_md cyclic factor=512 dim=1
#pragma HLS array_partition variable=refwin_line complete dim=0
#pragma HLS array_partition variable=refwin_line2 complete dim=0
	static pel ref_line[512];
	static pel ref_line2[512];
#pragma HLS array_partition variable=ref_line complete dim=0
#pragma HLS array_partition variable=ref_line2 complete dim=0
	for (int i = 0; i < 512; i++)
	{
#pragma HLS UNROLL
		ref_line[i] = refwin_md[y * 512 + i];
		ref_line2[i] = refwin_md[(y + 1) * 512 + i];
	}
	for (int j = 0; j < 16; j++)
	{
#pragma HLS UNROLL
		refwin_line[j] = ref_line[x + j];
		refwin_line2[j] = ref_line2[x + j];
	}
}

static void refwin_copy_8(U6 ref_offsetx, U6 ref_offsety, pel refwin_md[512 * 512], pel refwin[16][16])
{
#pragma HLS INLINE OFF
#pragma HLS array_partition variable=refwin complete dim=0
	for (int i = 0; i < 8; i++)
	{
#pragma HLS PIPELINE
		refwin_copy_line_8(ref_offsetx, i * 2 + ref_offsety, refwin_md, refwin[i * 2], refwin[i * 2 + 1]);
	}
}


void com_tbl_mc_l_1_8_t(int x_pos, int y_pos, int ori_mv_x, int ori_mv_y, pel ref[SWW * SWH], int gmv_x, int gmv_y, int s_ref, int s_pred, s16 pred[8][8],
	U7 w, U7 h, U4 bit_depth, pel RefWindowLCU_t[512 * 512])
{
#pragma HLS array_partition variable=RefWindowLCU_t cyclic factor=512 dim=1
#pragma HLS array_partition variable=pred complete dim=0

	int         i, j;
	S32         pt;
	int dx = ori_mv_x & 0x3;
	int dy = ori_mv_y & 0x3;
	int x_pos_inlcu = x_pos % 64;
	int y_pos_inlcu = y_pos % 64;

	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = (1 << (shift2 - 1));

	static S32 hor_buf[8][16];
	static pel refwin[16][16];
	static S8 tbl_mc_l_coeff[4][8] =
	{
		{ 0, 0, 0, 64, 0, 0, 0, 0 },
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#pragma HLS array_partition variable=hor_buf complete dim=0
#pragma HLS array_partition variable=refwin complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0

	//U6 ref_offsetx = 9 + x_pos_inlcu + (ori_mv_x >> 2) + sw_offset_x_md;
	//U6 ref_offsety = 9 + y_pos_inlcu + (ori_mv_y >> 2) + sw_offset_y_md;

	refwin_copy_8((gmv_x >> 2) - 3, (gmv_y >> 2) - 3, RefWindowLCU_t, refwin);

	for (i = 0; i < 8; i++)
	{
#pragma HLS UNROLL
		for (j = 0; j < 15; j++)
		{
#pragma HLS UNROLL
			hor_buf[i][j] = (MAC_8TAP(tbl_mc_l_coeff[dx], refwin[j], i) + add1) >> shift1;
		}
	}

	for (i = 0; i < 8; i++)
	{
#pragma HLS UNROLL
		for (j = 0; j < 8; j++)
		{
#pragma HLS UNROLL
			pt = (MAC_8TAP(tbl_mc_l_coeff[dy], hor_buf[i], j) + add2) >> shift2;
			pred[j][i] = (s16)COM_CLIP3(0, (1 << bit_depth) - 1, pt);
		}
	}

	/*for (i = 0; i < 8; i++)
	{
#pragma HLS UNROLL
		for (j = 0; j < 8; j++)
		{
#pragma HLS UNROLL
			if (i >= h || j >= w)
			{
				pred[i][j] = 0;
			}
		}
	}*/

}

int com_affine_mc_pred_y_8(pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_COM_MODE* mod_info_curr,
	pel pred[64], U3 sub_w, U3 sub_h, U1 lidx, U4 bit_depth, U13 pic_w, U13 pic_h)
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

#pragma HLS loop_tripcount max = 2
		for (w = 0; w < cu_width; w += sub_w) {

#pragma HLS loop_tripcount max = 2
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
			if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, CtrPos, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x, y, 0, 4)) {
				return 0;
			}
		}
		pred += cu_width * sub_h;
	}
	//
	if (mod_info_curr->chroma_motion)
	{
		sub_w = 8;
		sub_h = 8;
		int w_scu;
		int h_scu;
		for (h = 0; h < cu_height; h += sub_h) {

#pragma HLS loop_tripcount max = 1
			for (w = 0; w < cu_width; w += sub_w) {

#pragma HLS loop_tripcount max = 1
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
				if (!mv_inbound_new(qpel_gmv_x, qpel_gmv_y, CtrPos, sub_w, sub_h, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x, y, 0, 4)) {
					return 0;
				}
			}
		}
	}
	return 1;
}

int com_affine_mc_fetch_pred_y_8(pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_COM_MODE* mod_info_curr, U4 bit_depth,
	U13 pic_w, U13 pic_h, pel pred_cache[64]
#if ENABLE_BFRAME
	,U1 lidx
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
		return com_affine_mc_pred_y_8(RefWindowLCU, CtrPos, mod_info_curr, pred_cache, sub_w, sub_h, 
#if ENABLE_BFRAME
			lidx,
#else
			0,
#endif
			bit_depth, pic_w, pic_h);
	}
	return 0;
}


void diff_16b_8(U7 w, U7 h, s16* src1, s16 src2[8][8], int s_src1, int s_src2, int s_diff, s16 diff[8][8])
{

#pragma HLS INLINE off
#pragma HLS array_partition variable=diff complete dim=0
#pragma HLS array_partition variable=src1 complete dim=0
#pragma HLS array_partition variable=src2 complete dim=0
	int i;
	int j;
	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			diff[i][j] = ((s16)(((int)src1[j + i * 8]) - ((int)src2[i][j])));
		}
	}

	for (i = 0; i < 8; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if (i >= h || j >= w)
			{
				diff[i][j] = 0;
			}
		}
	}
}
void diff_16b_8(U7 w, U7 h, s16 src1[8][8], s16 src2[8][8], int s_src1, s16 diff[8][8])
{

#pragma HLS INLINE off
	int offset_s1 = 0;
	int offset_s2 = 0;
	int offset_diff = 0;
	int i;
	int j;
	for (i = 0; i < h; i++) {

#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT max=8
		for (j = 0; j < w; j++) {

#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT max=8
			//diff[j + offset_diff] = ((s16)(((int)src1[j + offset_s1]) - ((int)src2[j + offset_s2])));
			diff[i][j] = ((s16)(((int)src1[i][j]) - ((int)src2[i][j])));
		}
		//offset_s1 += s_src1;
	}
}

void enc_diff_16b_8(U3 log2w, U3 log2h, s16* src1, s16 src2[8][8], int s_src1, int s_src2, int s_diff, s16 diff[8][8])
{

#pragma HLS INLINE off
	U3 x = log2w;
	U3 y = log2h;
	U7 w = 1 << log2w;
	U7 h = 1 << log2h;
	{
		diff_16b_8(w, h, src1, src2, s_src1, s_src2, s_diff, diff);
	}
}
void enc_diff_16b_8(U3 log2w, U3 log2h, s16 src1[8][8], s16 src2[8][8], int s_src1, s16 diff[8][8])
{

#pragma HLS INLINE off
	U3 x = log2w;
	U3 y = log2h;
	U7 w = 1 << log2w;
	U7 h = 1 << log2h;
	{
		diff_16b_8(w, h, src1, src2, s_src1, diff);
	}
}

void init_inter_data_8(MD_COM_MODE* mod_info_curr_ptr)
{
#pragma HLS array_partition variable=mod_info_curr_ptr->mv complete dim=0
#pragma HLS array_partition variable=mod_info_curr_ptr->mvd complete dim=0
#pragma HLS array_partition variable=mod_info_curr_ptr->num_nz complete dim=0
	mod_info_curr_ptr->skip_idx = 0;
	mod_info_curr_ptr->smvd_flag = 0;

}

void get_pred_y_8(int w, int h, int x_refp, int y_refp, int dx, int dy, Arbitor& ref,
#if ENABLE_BFRAME
	U1 cur_refw_idx,
#endif
	pel pred[8][8])
{

//#pragma HLS PIPELINE
#pragma HLS INLINE
//#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
//#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
//#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
//#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
//#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
//#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
//#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
//#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
//#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
//#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
//#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
//#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
//#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
//#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
//#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
//#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1
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
#pragma HLS ARRAY_PARTITION variable=pred complete dim=0
	int frac_idx = dy * 4 + dx;
	int gmvx_refp_int = x_refp >> 2;
	int gmvy_refp_int = y_refp >> 2;

	int m = (gmvx_refp_int & 31) + 7;
	int n = gmvy_refp_int & 7;

	pel buffer0[8][2][16];
#pragma HLS ARRAY_PARTITION variable= buffer0 complete dim=0
	if (m < 32) {

		if (n == 0) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				//buffer0[i][j][k] = refwin_mc[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][(gmvy_refp_int + i) & 7][j][k];
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 1) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 2) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 3) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 4) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 5) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 6) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}

		//buffer0[i][j][k] = refwin_mc[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][(gmvy_refp_int + i) & 7][j][k];

	}
	else {

		if (n == 0) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 1) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 2) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 3) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 4) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 5) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else if (n == 6) {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}
		else {
			for (int k = 0; k < 16; k++) {
#pragma HLS UNROLL
				buffer0[0][0][k] = ref.ref_0_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[0][1][k] = ref.ref_0_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][0][k] = ref.ref_1_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[1][1][k] = ref.ref_1_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][0][k] = ref.ref_2_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[2][1][k] = ref.ref_2_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][0][k] = ref.ref_3_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[3][1][k] = ref.ref_3_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][0][k] = ref.ref_4_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[4][1][k] = ref.ref_4_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][0][k] = ref.ref_5_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[5][1][k] = ref.ref_5_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][0][k] = ref.ref_6_0[(gmvx_refp_int >> 5) + 1 + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[6][1][k] = ref.ref_6_1[(gmvx_refp_int >> 5) + (((gmvy_refp_int >> 3) + 1) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][0][k] = ref.ref_7_0[(gmvx_refp_int >> 5) + 1 + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
				buffer0[7][1][k] = ref.ref_7_1[(gmvx_refp_int >> 5) + ((gmvy_refp_int >> 3) * 5) + 80 * frac_idx + cur_refw_idx * 1280][k];
			}
		}

	}

	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		for (int j = 0; j < 8; j++) {
#pragma HLS UNROLL
#if ENABLE_BFRAME
			pred[i][j] = buffer0[(gmvy_refp_int + i) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#else
			pred[(k * 4 + i)][j] = refwin_mc[((gmvx_refp_int + j) >> 5) + (((gmvy_refp_int + (k * 4 + i)) >> 3) * 5) + 80 * frac_idx]
				[(gmvy_refp_int + (k * 4 + i)) & 7][((gmvx_refp_int + j) & 31) >> 4][(gmvx_refp_int + j) & 15];
#endif                    
		}
	}
	return;
}



BOOL com_mc_cu_y_8(CU_POS* cu, U13 pic_w, U13 pic_h, S3 refi[REFP_NUM], s16 mv[REFP_NUM][2], U11 stride_luma,
	pel pred_y[8][8], U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	Arbitor& ref
	, U2 mode_type
)
{

	int scup = cu->scup;
	int x = cu->x_pos;
	int y = cu->y_pos;
	int w = cu->cu_width;
	int h = cu->cu_height;
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

	//int bidx = 0;
	U1 ref0_valid = 0, ref1_valid = 0;
	pel pred_ref0[8][8];
#pragma HLS ARRAY_PARTITION variable=pred_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_ref0 complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv_t complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=min_clip complete dim=0
#pragma HLS ARRAY_PARTITION variable=max_clip complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi complete dim=0
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
#endif // !ENABLE_BFRAME

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
#endif // MD_MC_LUMA_REMOVE
			fflush(stdout);
#endif
			return FALSE;
		}

		get_pred_y_8(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref,
#if ENABLE_BFRAME
			cur_refw_idx,
#endif // ENABLE_BFRAME
			pred_ref0);

		//++bidx;
		ref0_valid = 1;
	}
	if (ref0_valid)
	{
		for (int i = 0; i < 8; i++)//可能出问题
#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
#pragma HLS UNROLL
				pred_y[i][j] = pred_ref0[i][j];
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
	static pel pred_ref1[8][8];
#pragma HLS ARRAY_PARTITION variable=pred_ref1 complete dim=0
	if (REFI_IS_VALID(refi[REFP_1]))
	{
		/* backward */
		//pel(*pred)[8] = bidx ? pred_snd : pred_y;
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
			printf(") ");
			printf("\n");
			fflush(stdout);
#endif
			return FALSE;
		}

		get_pred_y_8(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref, cur_refw_idx, pred_ref1);
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
		average_16b_no_clip_sse(pred_y[0], pred_snd[0], pred_y[0], 8, 8, 8, w, h);
#else
		//pel(*p0)[8] = pred_y;
		//pel(*p1)[8] = pred_snd;
		for (int j = 0; j < 8; j++)//可能出问题
		{
#pragma HLS UNROLL
			for (int i = 0; i < 8; i++)
			{
#pragma HLS UNROLL
				pred_y[j][i] = (pred_ref0[j][i] + pred_ref1[j][i] + 1) >> 1;
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
			for (int j = 0; j < 8; j++)//可能出问题
			{
#pragma HLS UNROLL
				for (int i = 0; i < 8; i++)
				{
#pragma HLS UNROLL
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
}

BOOL rdo_mc_y_8(u8 mode_type,
	S14 CtrPosSKIP[2][2][2],
	Arbitor& ref,
	int x_pos, int y_pos, int cu_width, int cu_height,
	S14 mv[2][2], s16 mv_t[2][2], U11 stride, U4 bit_depth, U2 channel,
#if ENABLE_BFRAME
	U1 lidx,
#endif
	pel pred_y[8][8])
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

		get_pred_y_8(w, h, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, dx, dy, ref, 
#if ENABLE_BFRAME
			0,
#endif // ENABLE_BFRAME
			pred_y);
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
		int qpel_gmv_x_of_refp_cu_8 = qpel_gmv_x - (((int)CtrPosSKIP[0][0][0]) << 2) + (((int)CtrPosSKIP[0][0][1]) << 2);
		int qpel_gmv_y_of_refp_cu_8 = qpel_gmv_y - (((int)CtrPosSKIP[0][1][0]) << 2) + (((int)CtrPosSKIP[0][1][1]) << 2);
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
		get_pred_y_8(w, h, qpel_gmv_x_of_refp_cu_8, qpel_gmv_y_of_refp_cu_8, dx, dy, ref, lidx, pred_y);
#else
		get_pred_y_8(w, h, qpel_gmv_x_of_refp_cu_8, qpel_gmv_y_of_refp_cu_8, dx, dy, fme2mdrefbuf, pred_y);
#endif
	}

	return 1;
}

//BOOL com_mc_y_8(u8 mode_type, CU_POS* cu, U13 pic_w, U13 pic_h, S3 refi[2], S14 mv[2][2], U11 stride_luma,
//	pel pred_y[8][8], U4 bit_depth, U2 channel, 
//	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS], Arbitor& ref
//#if ENABLE_BFRAME
//	, U1 lidx
//#endif
//)
//{
//	int scup = cu->scup;
//	int x = cu->x_pos;
//	int y = cu->y_pos;
//	int w = cu->cu_width;
//	int h = cu->cu_height;
//	S14 mv_t[2][2];
//#pragma HLS array_partition variable=mv_t complete dim=0
//	int mc_flag = 1;
//	int qpel_gmv_x_of_refp_cu = 0;
//	int qpel_gmv_y_of_refp_cu = 0;
//
//	mv_clip_1(x, y, pic_w, pic_h, w, h, refi, mv, mv_t);
//
//	if ((mode_type == 0 || mode_type == 1) && ((refi[0]) >= 0)) {
//		if (!rdo_mc_y_8(mode_type, 
//			CtrPosSKIP, 
//			ref,
//			x, y, w, h, mv, mv_t, stride_luma, bit_depth, channel, 
//#if ENABLE_BFRAME
//			lidx,
//#endif // ENABLE_BFRAME
//			pred_y))
//			return FALSE;
//	}
//	else {
//#if MD_PRINT
//		printf("%s:%d:refi is invalid\n", __FUNCTION__, 3213);
//#endif
//	}
//	return 1;
//}


BOOL mc_skip_y_8(MD_COM_MODE* mod_info_curr, U13 pic_width, U13 pic_height, pel pred_cache[8][8], U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS], 
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif // ENABLE_BFRAME
	Arbitor& ref
)
{
	{
		CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };
		if (!com_mc_cu_y_8(&cu, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
			SWW, pred_cache, bit_depth, Fetch_Ref_window_ptr,
			CtrPosSKIP, 
#if ENABLE_BFRAME
			CtrPosSKIP_ref1,
#endif // ENABLE_BFRAME
			ref
			, MODE_SKIP
		))
		{
			return 0;
		}
	}
	return 1;
}

BOOL derive_mode_info_8(int idx, SKIP_RDO* skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge, MD_COM_MODE* mod_info_curr,
	S3 refi_for_ip[2], S14 mv_for_ip[2][2])
{
#pragma HLS array_partition variable=mod_info_curr->mv complete dim=0
#pragma HLS array_partition variable=mod_info_curr->refi complete dim=0
	if (idx < skip->num_rdo) {
		int skip_idx = idx;
		mod_info_curr->umve_flag = skip_rdo_mode[skip_idx].umve_flag;
		mod_info_curr->umve_idx = skip_rdo_mode[skip_idx].umve_idx;
		mod_info_curr->affine_flag = skip_rdo_mode[skip_idx].affine_flag;
		mod_info_curr->skip_idx = skip_rdo_mode[skip_idx].skip_idx;
		/*mod_info_curr->mv[0][0] = skip_rdo_mode[skip_idx].mvp[MV_X];
		mod_info_curr->mv[0][1] = skip_rdo_mode[skip_idx].mvp[MV_Y];
		mod_info_curr->mv[1][0] = ((s16)0);
		mod_info_curr->mv[1][1] = ((s16)0);*/
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

BOOL make_skip_resi_8(U6 idx, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, MD_COM_MODE* mod_info_curr,
	U14 pic_width, U14 pic_height, U3 cu_width_log2, U3 cu_height_log2, pel* org_y,
	U7 stride_org_l, U7 cu_width, U7 cu_height, s16 resi_t[8][8], pel pred_cache[8][8], U4 bit_depth,
	S3 refi_for_ip[2], S14 mv_for_ip[2][2])
{
	if (!derive_mode_info_8(idx, skip_rdo, skip_rdo_mode, merge_rdo, mod_info_curr, refi_for_ip, mv_for_ip)) {
		return 0;
	}
	return 1;
}

void com_mc_fetch_8(pel* refpic_y, MD_COM_MODE* mod_info_curr, U2 channel, U4 bit_depth, U13 pic_width,
	U13 pic_height, u8 curr_mvr, S14 CtrPos[2][2][2], pel pred_cache[8][8],
	pel refwin_md[88][88], S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS], pel RefWindowLCU_t[512 * 512])
{
#pragma HLS array_partition variable=mod_info_curr->refi complete dim=0
#pragma HLS array_partition variable=CtrPos complete dim=0
#pragma HLS array_partition variable=mod_info_curr->mv complete dim=0
	int x = mod_info_curr->x_pos;
	int y = mod_info_curr->y_pos;
	int w = mod_info_curr->cu_width;
	int h = mod_info_curr->cu_height;
	int pic_w = pic_width;
	int pic_h = pic_height;
	S3* refi = mod_info_curr->refi;
	int pred_stride = mod_info_curr->cu_width;
	int qpel_gmv_x;
	int qpel_gmv_y;
	s16 mv_t[2][2];
#pragma HLS array_partition variable=mv_t complete dim=0
	int stride_l = SWW;
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
			com_tbl_mc_l_1_8(mod_info_curr->mv[REFP_0][0], mod_info_curr->mv[REFP_0][1], refpic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l, pred_stride, pred_cache, w, h, bit_depth);
			//com_tbl_mc_l_1_8_t(x, y, mod_info_curr->mv[REFP_0][0], mod_info_curr->mv[REFP_0][1], refpic_y, qpel_gmv_x_in_sw, qpel_gmv_y_in_sw, stride_l, pred_stride, pred_cache, w, h, bit_depth,
			//	RefWindowLCU_t);
		}
	}
}

s64 pinter_me_pred_8(RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_COM_MODE* mod_info_curr_ptr,
	s64 lambdaY, s64 dist_pred[1], pel* org_y, int stride_org, s64* cost_best,
	int bit_cnt, u8 curr_mvr, s16 resi_t[8][8], U2 tree_status, U2 cons_pred_mode, pel pred_cache[8][8], U1 is_intra,
	pel refwin_md[88][88], S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS], pel RefWindowLCU_t[512 * 512], U4 bit_depth)
{
#pragma HLS array_partition variable=mod_info_curr_ptr->num_nz complete dim=0
	MD_COM_MODE* mod_info_curr = mod_info_curr_ptr;

	/* prediction */
#if MD_PRINT
	((int)mod_info_curr->refi[0]) >= 0
		&& !(((int)mod_info_curr->refi[1]) >= 0) ? ((void)0) :
		__assert_HLS("((mod_info_curr->refi[0]) >= 0) && !((mod_info_curr->refi[1]) >= 0)", "../separate/enc_pinter.c",
			(unsigned int)1053, __FUNCTION__);
#endif

		com_mc_fetch_8(RefWindowLCU, mod_info_curr, tree_status, bit_depth,
			md_fw_ptr->pic_width, md_fw_ptr->pic_height, curr_mvr, CtrPos, pred_cache,
			refwin_md, CtrPosFME, RefWindowLCU_t);

	return *cost_best;
}

void fme_set_mv_refi_8(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_COM_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx,
	S3 refi_for_ip[2])
{
#pragma HLS inline
	s8 t0, t1;
	// best_refi is fixed to 0.
	mv[0] = mv_all[best_refi][curr_mvr][0];
	mv[1] = mv_all[best_refi][curr_mvr][1];
	//mvp = mvp_scale[lidx][best_refi];
	t0 = lidx == 0 ? best_refi : -1;
	t1 = lidx == 1 ? best_refi : -1;
	refi_for_ip[0] = t0;
	refi_for_ip[1] = t1;
	/*mod_info_curr->refi[0] = t0;
	mod_info_curr->refi[1] = t1;*/
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

BOOL analyze_me_uni_pred_8(MD_COM_MODE* mod_info_curr,
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
#pragma HLS array_partition variable=sub_pel_mv complete dim=0
#pragma HLS array_partition variable=mvp_all complete dim=0
#pragma HLS array_partition variable=mv_all complete dim=0
#pragma HLS array_partition variable=mvd_for_ip complete dim=0
#pragma HLS array_partition variable=mv_for_ip complete dim=0
#pragma HLS array_partition variable=refi_for_ip complete dim=0
#pragma HLS array_partition variable=mod_info_curr->mv complete dim=0
#pragma HLS array_partition variable=mod_info_curr->mvd complete dim=0
#pragma HLS array_partition variable=best_mv_uni_inner complete dim=0

	int bit_cnt = 0;
#if ENABLE_BFRAME
	int lidx = mv_dir->lidx;
	int best_refi = mv_dir->cur_refi;
#else
	int lidx = 0;
	int best_refi = 0;
#endif
	int me_level = 3;
	int is_intra = 0;
	//mod_info_curr->cu_mode = 1;//可能出问题
	mod_info_curr->affine_flag = 0;
	s16* mv;
	s16* mvd;
	/*mv = mod_info_curr->mv[lidx];*/
	/*mvd = mod_info_curr->mvd[lidx];*/
	mv = mv_for_ip[lidx];
	mvd = mvd_for_ip[lidx];

	init_inter_data_8(mod_info_curr);

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
	
	fme_set_mv_refi_8(mv, mvd, mv_all, mvp_all, mod_info_curr, best_refi, curr_mvr, lidx, refi_for_ip);

	/* prediction */
#if MD_PRINT && !ENABLE_BFRAME
	((int)refi_for_ip[0]) >= 0
		&& !(((int)refi_for_ip[1]) >= 0) ? ((void)0) :
		__assert_HLS("((refi_for_ip[0]) >= 0) && !((refi_for_ip[1]) >= 0)", "../separate/enc_pinter.c",
			(unsigned int)1053, __FUNCTION__);
#endif

	return TRUE;
}

void ssd_bitcnt_8(U2 tree_status, u8 cons_pred_mode,u8 num_refp_cnt_for_ssd,
	s64 lambdaY, u8 mode_type_for_ssd_fz, int inter_mode_idx_for_ssd_fz, U1 valid_flag_for_ssd_fz,
	MD_FW* md_fw_ptr, MD_COM_MODE* mod_info_curr,
	S3 mod_info_refi_for_re_fz[2], s16 mod_info_mvd_for_re_fz[2][2], s16 mod_info_affine_mvd_for_re_fz[2][4][2], u8	mod_info_mpm_for_re_fz[4][2], S8	mod_info_ipm_for_re_fz[4][2],
	pel orgY_for_ssd_forced_zeros[8][8], pel pred_for_ssd_forced_zeros[8][8],
	s64* lambda_y_for_quant, u8* mode_type_for_re, U1* valid_flag_for_up, s64* dist_pred_for_up, s64* cost_best_temp_for_up,
	int md_fw_ptr_refp_ptr_for_re[17][2], S3 mod_info_refi_for_re[2], s16 mod_info_mvd_for_re[2][2], s16 mod_info_affine_mvd_for_re[2][4][2], u8 mod_info_mpm_for_re[4][2], S8 mod_info_ipm_for_re[4][2],
	pel orgY_for_ssd[8][8], pel pred_for_rec[8][8])
{
#pragma HLS INLINE
	*mode_type_for_re = mode_type_for_ssd_fz;
	*valid_flag_for_up = valid_flag_for_ssd_fz;
	*lambda_y_for_quant = lambdaY;
	memcpy_1d<S3, 2>(mod_info_refi_for_re_fz, mod_info_refi_for_re);
	memcpy_2d<s16, 2, 2>(mod_info_mvd_for_re_fz, mod_info_mvd_for_re);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 2; k++)
				mod_info_affine_mvd_for_re[i][j][k] = mod_info_affine_mvd_for_re_fz[i][j][k];
	memcpy_2d<u8, 4, 2>(mod_info_mpm_for_re_fz, mod_info_mpm_for_re);
	memcpy_2d<S8, 4, 2>(mod_info_ipm_for_re_fz, mod_info_ipm_for_re);

	//memcpy_1d<pel, 64>(orgY_for_ssd_forced_zeros, orgY_for_ssd);
	memcpy_2d<pel, 8, 8>(orgY_for_ssd_forced_zeros, orgY_for_ssd);
	memcpy_2d<pel, 8, 8>(pred_for_ssd_forced_zeros, pred_for_rec);
	memcpy_2d<int, 17, 2>(md_fw_ptr->refp_ptr, md_fw_ptr_refp_ptr_for_re);

	if (inter_mode_idx_for_ssd_fz < 0)
		return;

	s64 dist_pred = MAX_S64;
	s64 cost_best_temp = MAX_S64;
	static u8 curr_mvr_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 1, 2 };
	int curr_mvr = curr_mvr_table[inter_mode_idx_for_ssd_fz];
	int cu_width_log2 = mod_info_curr->cu_width_log2;
	int cu_height_log2 = mod_info_curr->cu_height_log2;
	int is_intra = 0;
	/*if (valid_flag_for_ssd_fz == 1) */{
		if (mode_type_for_ssd_fz == 1 || mode_type_for_ssd_fz == 0)
		{
			dist_pred = enc_ssd_16b_2_8(cu_width_log2, cu_height_log2, pred_for_ssd_forced_zeros, orgY_for_ssd_forced_zeros, 1 << cu_width_log2, 8, md_fw_ptr->bit_depth_internal);
			int num_nz_fz[4][3];
			cu_nz_cln(num_nz_fz);
			mod_info_curr->tb_part = ((int)SIZE_2Nx2N);
			if (mode_type_for_ssd_fz == 0)
			{
				mod_info_curr->cu_mode = 3;
			}
			else
			{
				cost_best_temp = dist_pred;
#if ENABLE_BFRAME
				int bit_cnt = get_bitcnt_est_8_coef_0(md_fw_ptr->f_scu, curr_mvr, tree_status, 0, 2, cons_pred_mode, mod_info_curr->cu_mode, mod_info_curr->umve_flag, mod_info_curr->umve_idx, mod_info_curr->affine_flag, 1, mod_info_curr->skip_idx, mod_info_refi_for_re_fz, mod_info_curr->mvp_from_hmvp_flag, mod_info_curr->smvd_flag, /*md_fw_ptr->num_refp[0],*/ num_refp_cnt_for_ssd,mod_info_affine_mvd_for_re_fz, mod_info_mvd_for_re_fz, mod_info_curr->cu_width_log2, mod_info_curr->cu_height_log2, mod_info_curr->tb_part, mod_info_curr->pb_part, num_nz_fz, mod_info_ipm_for_re_fz, md_fw_ptr->slice_type, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mod_info_mpm_for_re_fz, is_intra);
#else
				int bit_cnt = get_bitcnt_est_8_coef_0(md_fw_ptr->f_scu, curr_mvr, tree_status, 0, 2, cons_pred_mode, mod_info_curr->cu_mode, mod_info_curr->umve_flag, mod_info_curr->umve_idx, mod_info_curr->affine_flag, 1, mod_info_curr->skip_idx, mod_info_refi_for_re_fz, mod_info_curr->mvp_from_hmvp_flag, mod_info_curr->smvd_flag,/* md_fw_ptr->num_refp,*/ num_refp_cnt_for_ssd, mod_info_affine_mvd_for_re_fz, mod_info_mvd_for_re_fz, mod_info_curr->cu_width_log2, mod_info_curr->cu_height_log2, mod_info_curr->tb_part, mod_info_curr->pb_part, num_nz_fz, mod_info_ipm_for_re_fz, md_fw_ptr->slice_type, md_fw_ptr->cur_ptr, md_fw_ptr->refp_ptr, mod_info_mpm_for_re_fz, is_intra);
#endif
				cost_best_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
			}
		}
	}

	*dist_pred_for_up = dist_pred;
	*cost_best_temp_for_up = cost_best_temp;
}


//static void limit_inter_rdo_num_8(RDOQ_MODEL* model_cnt, U3 max_rdo_num, U5 all_inter_mode_num, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
//	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
//	s64 cost_best_temp[1 + 17],
//	MD_INPUT* md_input_ptr, u8 num_refp_cnt, U7 cu_width, U7 cu_height, s16 mvp_all[2][5][2],
//	s16 mv_all[2][5][2], u32 mecost_all[2][5], U8 RefWindowLCU[SWW*SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
//	MD_COM_MODE mod_info_curr_ptr[5 + 1], COM_PART_INFO mod_info_curr_pb_info[5 + 1], COM_PART_INFO mod_info_curr_tb_info[17 + 1], s64 dist_pred[5 + 1], pel* org_y, int stride_org, int bit_cnt, int refi_cur, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH],
//	SKIP_RDO skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO merge_rdo,
//	U3 cu_width_log2, U3 cu_height_log2, s16 resi_t[5 + 1][8][8], U2 tree_status, U2 ctx_cons_pred_mode, U1 valid_flag[1], U1 valid_mode[17], S14 sub_pel_mv[MV_D], pel pred_cache[5 + 1][8][8])
//{
//	s64 dist_pred_all[30] = { 0 };
//	int best_mv_uni_inner[2][4][2];
//	for (U6 mode_idx = 9; mode_idx < 14; mode_idx++) {
//#pragma HLS UNROLL
//#pragma HLS LOOP_TRIPCOUNT max=5
//		analyze_uni_pred_8(model_cnt, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, &cost_best_temp[1 + mode_idx - 9], md_input_ptr, num_refp_cnt,
//			cu_width, cu_height, mvp_all, mv_all, mecost_all, best_mv_uni_inner, RefWindowLCU, CtrPos, &mod_info_curr_ptr[0], &mod_info_curr_pb_info[0], &mod_info_curr_tb_info[0],
//			&dist_pred[1 + mode_idx - 9], org_y, stride_org, bit_cnt, refi_cur,
//			mode_idx, skip_rdo, skip_rdo_mode, merge_rdo, cu_width_log2, cu_height_log2, resi_t[1 + mode_idx - 9], tree_status, ctx_cons_pred_mode, valid_flag, valid_mode, sub_pel_mv, pred_cache[1 + mode_idx - 9]);
//
//		if (valid_flag[0] == 1)
//		{
//			copy_md_com_mode_info(&mod_info_curr_ptr[0], &mod_info_curr_ptr[1 + mode_idx - 9]);
//			copy_md_com_part_info(&mod_info_curr_pb_info[0], &mod_info_curr_pb_info[1 + mode_idx - 9]);
//			copy_md_com_part_info(&mod_info_curr_tb_info[0], &mod_info_curr_tb_info[1 + mode_idx - 9]);
//			dist_pred_all[mode_idx] = dist_pred[0];
//			valid_mode[mode_idx] = 1;
//
//		}
//		else
//		{
//			dist_pred_all[mode_idx] = MAX_S64;
//			valid_mode[mode_idx] = 0;
//		}
//	}
//
//
//	choose_inter_rdo_mode(max_rdo_num, all_inter_mode_num, dist_pred_all, valid_mode);
//}

void fetch_ref_line_8(U8 w, U8 h, S15 hor_ref[SKIP_MC_HOR_NUM + 8 - 1][8], const U8 ref_y[SWH * SWW], const U10 r_off, const U10 c_off)
{
#pragma HLS INLINE OFF
	U8 line_buf[SWW] = {};
	static U8 buf[23 + 8] = {};
#pragma HLS array_partition variable=line_buf complete dim=0
#pragma HLS array_partition variable=buf complete dim=0

	for (int i = 0; i < SWW; ++i) {
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 && (r_off < SWH) ? (U8)(ref_y[r_off * SWW + i]) : (U8)0;
	}
	for (int i = 0; i < 23 + w; ++i) {
#pragma HLS loop_tripcount max=31
#pragma HLS UNROLL
		buf[i] = c_off >= 0 ? (U8)(line_buf[c_off + i]) : (U8)0;
	}
	for (int i = 0; i < 16 + w; ++i) {
#pragma HLS loop_tripcount max=24
#pragma HLS UNROLL
		for (int j = 0; j < 8; ++j) {
#pragma HLS UNROLL
			hor_ref[i][j] = buf[i + j];
		}
	}
}
void fetch_org_line_8(U8 w, U8 h, S15 hor_org[8][SKIP_MC_VER_NUM], const U8 org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U6 r_off, const U6 c_off)
{
#pragma HLS INLINE OFF
	static U8 line_buf[MAX_CU_SIZE_FETCH] = {};
#pragma HLS array_partition variable=line_buf complete dim=0
	for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i) {
#pragma HLS UNROLL
		line_buf[i] = r_off >= 0 ? (org_y[r_off][i]) : (U8)0;
	}
	for (int i = 0; i < w; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 8
		shifting(hor_org[i], line_buf[c_off + i], SKIP_MC_VER_NUM);
	}
}


void mc_sad_l_skip_8(U8 w, U8 h, S32 sad[SKIP_MODE_NUM], const SKIP_MODE_INTER mode[SKIP_MODE_NUM], const U1 mv_idx, const U12 x_pos, const U12 y_pos, const U7 cu_width, const U7 cu_height, const U12 pic_w, const U12 pic_h, const S12 mc_pos[MV_D], const U8 ref_y[SWH * SWW], const U8 org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const U12 x_chroma, const U12 y_chroma, const U1 chroma_motion)
{
#pragma HLS INLINE OFF
	static U1 flag[SKIP_MODE_NUM] = {};
	static S15 hor_ref[SKIP_MC_HOR_NUM + 8 - 1][8] = {};
	static S15 hor_buf[SKIP_MC_HOR_NUM + 8 - 1][4][8] = {};
	static S15 hor_org[8][SKIP_MC_VER_NUM] = {};
#pragma HLS array_partition variable=flag complete dim=0
#pragma HLS array_partition variable=hor_ref complete dim=0
#pragma HLS array_partition variable=hor_buf complete dim=0
#pragma HLS array_partition variable=hor_org complete dim=0

	const U10 ref_top = mc_pos[MV_Y] - 3;
	const U10 ref_left = mc_pos[MV_X] - 3;
	const U6 org_top = y_pos & (MAX_CU_SIZE_FETCH - 1);
	const U6 org_left = x_pos & (MAX_CU_SIZE_FETCH - 1);

	if (ref_top + SKIP_MC_VER_NUM + h - 1 + 7 > SWH) { return; }
	if (ref_left + SKIP_MC_HOR_NUM + w - 1 + 7 > SWW) { return; }

	for (int i = 0; i < SKIP_MODE_NUM; ++i) {
#pragma HLS UNROLL
		flag[i] = REFI_IS_VALID(mode[i].refi) && mode[i].mv_idx == mv_idx && check_mv_in_refwin(ref_top + (mode[i].mvd[MV_Y] >> 2), ref_left + (mode[i].mvd[MV_X] >> 2), x_pos, y_pos, cu_width, cu_height, x_chroma, y_chroma, chroma_motion);
	}
	static int pflag = 1;
	//LOOP_MC_SAD:
	for (int r = 0; r < SKIP_MC_VER_NUM + h - 1 + 7; ++r) {
#pragma HLS PIPELINE II=1
#pragma HLS loop_tripcount max = 19

		fetch_ref_line_8(w, h, hor_ref, ref_y, ref_top + r, ref_left);
		fetch_org_line_8(w, h, hor_org, org_y, org_top + r - 7, org_left);

		for (int c = 0; c < (SKIP_MC_HOR_NUM + cu_width - 1); ++c) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 24
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
			}
		}
	}
	pflag = 0;
}

static void make_skip_sad_8(
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
	const U8 ref_y[SWH * SWW],
	const U8 org_y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	const S12 mc_pos[SKIP_MC_NUM][MV_D]
) {
#pragma HLS INLINE OFF
	//#pragma HLS function_instantiate variable=cu_height

	for (int mv_idx = 0; mv_idx < SKIP_MC_NUM; ++mv_idx) {
		mc_sad_l_skip_8(w, h, mode_sad, mode, mv_idx, x_pos, y_pos, cu_width, cu_height, pic_w, pic_h, mc_pos[mv_idx], ref_y, org_y, x_chroma, y_chroma, chroma_motion);
	}
}

static void print_skip_mode(MD_COM_MODE* mod_info_curr, SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, S7 mode_list[MAX_INTER_SKIP_RDO])
{
#if MD_PRINT
	static int cnt = 0; ++cnt;
	//if (cnt >= 781621){
	printf("%d ", cnt);
	printf("mode_list=");
	for (int i = 0; i < MAX_INTER_SKIP_RDO; ++i) {
		printf("%d ", mode_list[i]);
	}
	printf("pos=(%d %d) size=(%d %d): ", mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height);
	int i, j;
	for (i = 0; i < skip_rdo->num_rdo; ++i) {
		//printf("(%s %d %d %d)", skip_rdo->mode[i].affine_flag ? "affine" : skip_rdo->mode[i].umve_flag ? "umve" : "skip", skip_rdo->mode[i].umve_flag ? skip_rdo->mode[i].umve_idx : skip_rdo->mode[i].skip_idx, skip_rdo->mode[i].mvp[MV_X], skip_rdo->mode[i].mvp[MV_Y]);
		printf("(%s %d %d %d)", skip_rdo_mode[i].affine_flag ? "affine" : skip_rdo_mode[i].umve_flag ? "umve" : "skip", skip_rdo_mode[i].umve_flag ? skip_rdo_mode[i].umve_idx : skip_rdo_mode[i].skip_idx, skip_rdo_mode[i].mvp[MV_X], skip_rdo_mode[i].mvp[MV_Y]);
	}
	for (i = 0; i < merge_rdo->num_cands; ++i) {
		if (!merge_rdo->valid[i]) { continue; }
		printf("(%s ", "affine");
		for (j = 0; j < merge_rdo->mrg_list_cp_num[i]; ++j) {
			printf("(%d %d) ", merge_rdo->mrg_list_cp_mv[i][REFP_0][j][MV_X], merge_rdo->mrg_list_cp_mv[i][REFP_0][j][MV_Y]);
		}
		printf(")");
	}
	printf("\n");
	//}
	fflush(stdout);
#endif
}

void derive_neb_cu_8(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height,
	NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV],
	const U32 map_scu_up_line[6], const U32 map_cu_mode_up_line[6],
	const U32 map_scu_left_line[4], const U32 map_cu_mode_left_line[4],
	const U1 curr_ctu_idx,
	const ENC_ME_NEB_INFO up_nebs[4], const ENC_ME_NEB_INFO left_nebs[2])
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

static void get_mvp_default_line_8(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	MD_COM_MODE* mod_info_curr, int refp_ptr[17][2],
	int ptr_cur, int lidx, s8 cur_refi, u8 amvr_idx, s16 mvp[2],
	ENC_ME_NEB_INFO up_nebs[1 + (8 >> 2) + 1], ENC_ME_NEB_INFO left_nebs[(8 >> 2)])
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
#pragma HLS ARRAY_PARTITION variable=mv_nebs complete dim=0

#pragma HLS ARRAY_PARTITION variable=refi_nebs complete dim=0
#pragma HLS ARRAY_PARTITION variable=valid_flag complete dim=0
#pragma HLS ARRAY_PARTITION variable=neb_addr complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=MVPs complete dim=0
#pragma HLS ARRAY_PARTITION variable=x_scu_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=y_scu_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi_nebs complete dim=0
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_8x8->map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_8x8->map_scu_left_line complete dim=0
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
	x_scu_line[0] = x_scu - me_line_map_ptr_8x8->min_scu_x - 1;
	y_scu_line[0] = y_scu - me_line_map_ptr_8x8->min_scu_y;
	x_scu_line[1] = x_scu - me_line_map_ptr_8x8->min_scu_x;
	y_scu_line[1] = y_scu - me_line_map_ptr_8x8->min_scu_y - 1;
	x_scu_line[2] = x_scu - me_line_map_ptr_8x8->min_scu_x + cu_width_in_scu;
	y_scu_line[2] = y_scu - me_line_map_ptr_8x8->min_scu_y - 1;
	x_scu_line[3] = x_scu - me_line_map_ptr_8x8->min_scu_x - 1;
	y_scu_line[3] = y_scu - me_line_map_ptr_8x8->min_scu_y - 1;
	if (x_scu == me_line_map_ptr_8x8->min_scu_x) {
		offset_in_line = y_scu - me_line_map_ptr_8x8->min_scu_y;
		x_scu_left = (1 << 7 >> 1 >> 2) - 1;
		// A
		neb_addr[0] = offset_in_line;
		if (valid_flag[0]) {
			refi_nebs[0] = left_nebs[0].refi[lidx];
			valid_flag[0] = ((int)(me_line_map_ptr_8x8->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_map_ptr_8x8->map_scu_left_line[0] >> 15 & ((
					unsigned int)1)))
				&& ((int)left_nebs[0].refi[lidx]) >= 0;
			//valid_flag[0] = valid_flag[0] && MCU_GET_CODED_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && !MCU_GET_INTRA_FLAG(me_line_ptr->map_scu_left[neb_addr[0]]) && REFI_IS_VALID(me_line_ptr->left_nebs[neb_addr[0]].refi[lidx]);
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] = left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] = left_nebs[0].mv[lidx][1];
		}
		//D
		if (y_scu != me_line_map_ptr_8x8->min_scu_y) {
			offset_in_line = y_scu - me_line_map_ptr_8x8->min_scu_y - 1;
			neb_addr[3] = offset_in_line;
			if (valid_flag[3]) {
				refi_nebs[3] = up_nebs[0].refi[lidx];
				valid_flag[3] = valid_flag[3]
					&& ((int)(me_line_map_ptr_8x8->map_scu_up_line[0] >> 31 & ((
						unsigned int)1)))
					&& !((int)(me_line_map_ptr_8x8->map_scu_up_line[0] >> 15 & ((
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
			valid_flag[0] = ((int)(me_line_map_ptr_8x8->map_scu_left_line[0] >> 31 & ((
				unsigned int)1)))
				&& !((int)(me_line_map_ptr_8x8->map_scu_left_line[0] >> 15 & ((
					unsigned int)1))) && ((int)refi_nebs[0]) >= 0;
		}
		if (valid_flag[0]) {
			mv_nebs[0][0] =
				left_nebs[0].mv[lidx][0];
			mv_nebs[0][1] =
				left_nebs[0].mv[lidx][1];
		}
	}
	if (y_scu == me_line_map_ptr_8x8->min_scu_y) {
		offset_in_line = x_scu;
		// B
		neb_addr[1] = offset_in_line;
		// C
		neb_addr[2] = offset_in_line + cu_width_in_scu;
		// D
		neb_addr[3] = offset_in_line - 1;
		//	for (i = 1; i < 3 + d_inner; i++)
		for (i = 1; i < 4; i++) {
			if (i < 3 + d_inner) {
#pragma HLS loop_tripcount max = 3
#pragma HLS UNROLL
				if (valid_flag[i]) {
					refi_nebs[i] = up_nebs[neb_addr[i] - offset_in_line + 1].refi[lidx];
					valid_flag[i] = valid_flag[i]
						&& ((int)(me_line_map_ptr_8x8->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 31 & ((unsigned int)1)))
						&& !((int)(me_line_map_ptr_8x8->map_scu_up_line[neb_addr[i] - offset_in_line + 1] >> 15 & ((unsigned int)1)))
						&& ((int)refi_nebs[i]) >= 0;
				}
				if (valid_flag[i]) {
					mv_nebs[i][0] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][0];
					mv_nebs[i][1] = up_nebs[neb_addr[i] - offset_in_line + 1].mv[lidx][1];
				}
			}
		}
	}
	else {
		//for (i = 1; i < 3 + d_inner; i++) {
		for (i = 1; i < 4; i++) {
#pragma HLS loop_tripcount max = 3
#pragma HLS UNROLL

			if (i < 3 + d_inner)
			{
				if (valid_flag[i]) {
					if (x_scu_line[i] > 15 || y_scu_line[i] > 15) {
						valid_flag[i] = 0;
					}
					if (valid_flag[i])
					{
						refi_nebs[i] = up_nebs[x_scu_line[i] - (x_scu - me_line_map_ptr_8x8->min_scu_x) + 1].refi[lidx];
						//valid_flag[i] = valid_flag[i] && MCU_GET_CODED_FLAG(map_scu[neb_addr[i]]) && !MCU_GET_INTRA_FLAG(map_scu[neb_addr[i]]) && REFI_IS_VALID(me_line_ptr->refi_nebs[i]);
						valid_flag[i] = valid_flag[i]
							&& ((int)(me_line_map_ptr_8x8->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_map_ptr_8x8->min_scu_x) + 1] >> 31 & ((
								unsigned int)1)))
							&& !((int)(me_line_map_ptr_8x8->map_scu_up_line[x_scu_line[i] - (x_scu - me_line_map_ptr_8x8->min_scu_x) + 1] >> 15 & ((
								unsigned int)1))) && ((int)refi_nebs[i]) >= 0;
					}
				}
				if (valid_flag[i]) {
					mv_nebs[i][0] =
						up_nebs[x_scu_line[i] - (x_scu - me_line_map_ptr_8x8->min_scu_x) + 1].mv[lidx][0];
					mv_nebs[i][1] =
						up_nebs[x_scu_line[i] - (x_scu - me_line_map_ptr_8x8->min_scu_x) + 1].mv[lidx][1];
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
//#pragma HLS PIPELINE II=2
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

void derive_mvp_line_all_8(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4], ENC_ME_NEB_INFO left_nebs[2],
	MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2])
{
#pragma HLS inline off
#pragma HLS ARRAY_PARTITION variable=mvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=refp_ptr complete dim=0

#pragma HLS   ARRAY_PARTITION variable=up_nebs complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[0].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[1].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[2].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[3].mv complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	up_nebs[0].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[1].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[2].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[3].refi complete dim=0

#pragma HLS   ARRAY_PARTITION variable=left_nebs complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	left_nebs[0].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	left_nebs[1].mv complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	left_nebs[0].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	left_nebs[1].refi complete dim=0

	U23 scup = mod_info_curr->scup;
	U8 emvp_flag = mod_info_curr->mvp_from_hmvp_flag;
	u8 mvr_idx = 0;
	CPMV mvp_x = 0;
	CPMV mvp_y = 0;
	if (!emvp_flag) {
		get_mvp_default_line_8(md_fw_ptr, me_line_map_ptr_8x8, mod_info_curr, refp_ptr, ptr, ref_list, (s8)ref_idx, mvr_idx, mvp[mvr_idx], up_nebs, left_nebs);
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

void prepare_luma_inter_8(MD_FW* md_fw_ptr, 
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4],
	ENC_ME_NEB_INFO left_nebs[2], 
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
	U13 x_pos = mod_info_curr->x_pos;
	U13 y_pos = mod_info_curr->y_pos;
	U10 x_scu = x_pos >> 2;
	U10 y_scu = y_pos >> 2;

	U7 cu_width = mod_info_curr->cu_width;
	U7 cu_height = mod_info_curr->cu_height;
	U3 cu_width_log2 = mod_info_curr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr->cu_height_log2;
	U11 pic_width_in_scu = md_fw_ptr->pic_width_in_scu;
	U11 pic_height_in_scu = md_fw_ptr->pic_height_in_scu;

	U11 x0_scu;
	U11 y0_scu;
	x0_scu = PEL2SCU(x_pos - md_input_ptr->pix_x);
	y0_scu = PEL2SCU(y_pos - md_input_ptr->pix_y);

	U1 curr_ctu_idx = me_line_map_ptr_8x8->curr_ctu_idx;
	U1 above_line_idx = me_line_map_ptr_8x8->above_line_idx;
	U12 pix_x_scu = in->pix_x >> MIN_CU_LOG2;
#pragma HLS INLINE
#pragma HLS   ARRAY_PARTITION variable=	in->refp_map_mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	in->refp_map_refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	in->refp_list_ptr1 complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	in->refp_ptr1 complete dim=0
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
// #pragma HLS   ARRAY_PARTITION variable=	skip_rdo_mode complete dim=0
//#pragma HLS   ARRAY_PARTITION variable=	mode->mvp complete dim=0
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

	for (int i = 0; i < 5; ++i)//SKIP_MODE_NUM
	{
#pragma HLS	UNROLL
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
	U1 allow_affine = 0;
	U1 run_direct_skip = cu_width * cu_height >= 64;
	U1 run_affine_merge = allow_affine && cu_width >= 16 && cu_height >= 16;
	
	NEB_CU neb[NUM_SKIP_SPATIAL_MV];
#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS	UNROLL
		neb[i].x_scu_of_ctu = 0;
		neb[i].y_scu_of_ctu = 0;
		neb[i].valid_flag = 0;
	}
//	static SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];by ldm
 SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV];
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
#pragma HLS   ARRAY_PARTITION variable=	md_input_ptr->CtrPos2MD complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	md_input_ptr->CtrPos2MD_ref1 complete dim=0

	//by ldm
#pragma HLS   ARRAY_PARTITION variable=	Fetch_Ref_window_ptr->lidx complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	Fetch_Ref_window_ptr->cur_refi complete dim=0

#pragma HLS   ARRAY_PARTITION variable=up_nebs complete dim=0
//#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	up_nebs[0].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[1].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[2].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[3].mv complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	up_nebs[0].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[1].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[2].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	up_nebs[3].refi complete dim=0

#pragma HLS   ARRAY_PARTITION variable=left_nebs complete dim=0
//#pragma HLS   ARRAY_PARTITION variable=neb complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	left_nebs[0].mv complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	left_nebs[1].mv complete dim=0

#pragma HLS   ARRAY_PARTITION variable=	left_nebs[0].refi complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	left_nebs[1].refi complete dim=0
	for (int i = 0; i < 6; ++i)
	{
#pragma HLS	UNROLL
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
	derive_neb_cu_8(pic_width_in_scu, x_scu, y_scu, cu_width, cu_height,
		neb, neb_map,
		me_line_map_ptr_8x8->map_scu_up_line, me_line_map_ptr_8x8->map_cu_mode_up_line,
		me_line_map_ptr_8x8->map_scu_left_line, me_line_map_ptr_8x8->map_cu_mode_left_line,
		curr_ctu_idx,
		up_nebs, left_nebs);
	derive_inter_cands(in, (U4*)&skip_rdo->num_rdo, neb, neb_map, x_scu, y_scu, pic_width_in_scu, pic_height_in_scu, mode, run_direct_skip, Fetch_Ref_window_ptr
#if SUB_TMVP
		, mod_info_curr->cu_width, mod_info_curr->cu_height, core_sbtmvp
#endif
	);
#if SKIP_RESTRI
	check_skip_mv_inbound(mode, x_pos, y_pos, md_input_ptr->CtrPos2MD, cu_width, cu_height, mod_info_curr
#if ENABLE_BFRAME
		, md_input_ptr->CtrPos2MD_ref1, Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
	);
#endif
S7 mode_list_tmp[12];
#pragma HLS   ARRAY_PARTITION variable=mode_list complete dim=0
#pragma HLS   ARRAY_PARTITION variable=mode_list_tmp complete dim=0
//	for (int i = 0, j = 0; i < SKIP_MODE_NUM && j < 4; ++i) {
//#pragma HLS loop_tripcount max = 10
//#pragma HLS	UNROLL
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

		if (mode_list_tmp[i] >= 0 )
		{
			if (j < 4)
				mode_list[j++] = mode_list_tmp[i];
		}
	}
	output_skip_mode(skip_rdo, skip_rdo_mode, mode, mode_list);
	//print_skip_mode(mod_info_curr, skip_rdo, skip_rdo_mode, merge_rdo, mode_list);

	mod_info_curr->affine_flag = 0;
	mod_info_curr->mvp_from_hmvp_flag = 0;

	for (int i = 0; i < 2; i++) {
#pragma HLS	UNROLL
		if (cu_width == 8 && cu_height == 8) {
			sub_pel_mv[i] = fme_mv_info->FME_NAME(sadTree, Fmv, 8, 8)[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][i];
#if ENABLE_BFRAME
			mv_dir->cur_refi = me_mv_dir->block8x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
			mv_dir->lidx = me_mv_dir->block8x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
		}
		else if (cu_width == 4 && cu_height == 8) {
			sub_pel_mv[i] = fme_mv_info->FME_NAME(sadTree, Fmv, 4, 8)[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][i];
#if ENABLE_BFRAME
			mv_dir->cur_refi = me_mv_dir->block4x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
			mv_dir->lidx = me_mv_dir->block4x8MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
		}
		else if (cu_width == 8 && cu_height == 4) {
			sub_pel_mv[i] = fme_mv_info->FME_NAME(sadTree, Fmv, 8, 4)[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2][i];
#if ENABLE_BFRAME
			mv_dir->cur_refi = me_mv_dir->block8x4MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].cur_refi;
			mv_dir->lidx = me_mv_dir->block8x4MvDir[(x0_scu << 2) >> cu_width_log2][(y0_scu << 2) >> cu_height_log2].lidx;
#endif
		}
	}
#pragma HLS   ARRAY_PARTITION variable=mvp_all dim=0
#pragma HLS   ARRAY_PARTITION variable=refp_ptr dim=0
#if ENABLE_BFRAME
//	for (refi_cur = 0; refi_cur < num_refp_cnt[mv_dir->lidx]; refi_cur++) {
	for (refi_cur = 0; refi_cur < 2; refi_cur++) {
#pragma HLS loop_tripcount max = 2
#pragma HLS PIPELINE
       if( refi_cur < num_refp_cnt[mv_dir->lidx])
		derive_mvp_line_all_8(md_fw_ptr, me_line_map_ptr_8x8, up_nebs, left_nebs, mod_info_curr, ptr, mv_dir->lidx, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);

	}
#else
	for (refi_cur = 0; refi_cur < num_refp_cnt; refi_cur++) {
#pragma HLS loop_tripcount max = 1
		derive_mvp_line_all_8(md_fw_ptr, me_line_map_ptr_8x8, up_nebs, left_nebs, mod_info_curr, ptr, 0, (int)refi_cur, refp_ptr, num_amvr, mvp_all[refi_cur]);

	}
#endif
	for (int x = 0; x < 4; x++) {
#pragma HLS	UNROLL
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
		else
		{
			valid_mode[x] = 0;
		}
	}

	for (int y = 0; y < 5; y++) {
#pragma HLS	UNROLL
			valid_mode[y + 4] = 0;
	}

	for (int z = 0; z < 5; z++) {
#pragma HLS	UNROLL
		//if (amvr_enable_flag)
		//	valid_mode[z + 9] = 1;
		//else
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

	for (int p = 0; p < 3; p++) {
#pragma HLS	UNROLL
		valid_mode[p + 14] = 0;
	}

	

}

static int get_md_rdo_num_8(int qt_depth)
{
	int max_rdo_num = 5;

	if (qt_depth == 2)
	{
		max_rdo_num = 3;
	}

	return max_rdo_num;
}

static void get_IP_pixel_8(U8 w, U8 h, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel left[2 * 8 + 3], pel up[2 * 8 + 3], U14 x, U14 y, 
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8)
{
#pragma HLS INLINE
	int i;
	int j;
	int width_in_scu = ch_type == 0 ? w >> 2 : w >> (2 - 1);
	int height_in_scu = ch_type == 0 ? h >> 2 : h >> (2 - 1);
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
	int pic_x_scu = pic_x >> 2;
	int pic_y_scu = pic_y >> 2;
	int flag;

	S7 ip_array_size_idx = get_ip_array_size_idx(w, h);
#if MD_PRINT
	assert(ip_array_size_idx != -1);
#endif
#pragma HLS array_partition variable=IP_buffer_ptr_8x8->up_line dim=0
#pragma HLS array_partition variable=IP_buffer_ptr_8x8->left_line dim=0
#pragma HLS array_partition variable=IP_map_scu_ptr_8x8->map_scu_up_line dim=0

	if ((avail_cu & 1 << 0) == 1 << 0) 
	{
		for (i = 0; i < w; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS UNROLL
			up[i + 3] = IP_buffer_ptr_8x8->up_line[i];
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=2
#pragma HLS UNROLL
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				flag = (IP_map_scu_ptr_8x8->map_scu_up_line[width_in_scu + i] >> 31 & 1);
			}
			else
				flag = 0;

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
#pragma HLS UNROLL
					up[w + (i << unit_size_bit) + j + 3] = IP_buffer_ptr_8x8->up_line[w + (i << unit_size_bit) + j];
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++) {
#pragma HLS LOOP_TRIPCOUNT max=4
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

static int get_rmd_result_8(U3 cu_width_log2, U3 cu_heght_log2, U13 x, U13 y, u8 ipred_list[],
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE],
#else
	u8 rmd_mode[IPD_RDO_CNT],
#endif
#if MIPF
	int eipm_enable_flag
#endif
#if USE_SPEED_LEVEL
	,U8 speed_level
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

#if EIPM && !USE_EIPM_RMD
	if (eipm_enable_flag)
	{
		for (int i = 0; i < IPD_RDO_CNT; i++)
		{
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


	for (i = 0; i < cnt; i++) {
#pragma HLS  UNROLL
		ipred_list[i] = rmd_mode[i];
		//}
	}

	return cnt;
}

u16 com_get_avail_intra_ip_8(IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, U11 x_scu, U11 y_scu)
{
//#pragma HLS INLINE off
#pragma HLS INLINE
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0 && ((IP_map_scu_ptr_8x8->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0 && ((IP_map_scu_ptr_8x8->map_scu_left_line_new[0] >> 31 & (1)))) {
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
		if (x_scu > 0) {
			if (x_scu > 0 && ((IP_map_scu_ptr_8x8->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
		}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0 && ((IP_map_scu_ptr_8x8->map_scu_top_left_new >> 31 & (1)))) {
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

void com_get_mpm_8x8(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8, ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8)
{
//#pragma HLS INLINE off
#pragma HLS INLINE
	u8 ipm_l = 0;
	u8 ipm_u = 0;
	int valid_l = 0;
	int valid_u = 0;
	int mpm_x = x_scu % 16;
	int mpm_y = y_scu % 16;
	int pic_x_scu = mpm_x;
	int pic_y_scu = mpm_y;
	assert(pic_x_scu == mpm_x);
	assert(pic_y_scu == mpm_y);
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((me_line_map_ptr_8x8->map_scu_left_line[0] >> 15 & (1)))
			&& ((me_line_map_ptr_8x8->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_8x8->left;
			valid_l = 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((me_line_map_ptr_8x8->map_scu_left_line[0] >> 31 & (1)))) {
			ipm_l = MPM_buffer_ptr_8x8->left;
			valid_l = 1;
		}
	}
	if (pic_y_scu == 0) {
		if (y_scu > 0
			&& ((me_line_map_ptr_8x8->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_8x8->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_8x8->up;
			valid_u = 1;
		}
	}
	else {
		if (y_scu > 0
			&& ((me_line_map_ptr_8x8->map_scu_up_line[1] >> 15 & (1)))
			&& ((me_line_map_ptr_8x8->map_scu_up_line[1] >> 31 & (1)))) {
			ipm_u = MPM_buffer_ptr_8x8->up;
			valid_u = 1;
		}
	}
	/*mpm[0] = (ipm_l < ipm_u ? ipm_l : ipm_u);
	mpm[1] = (ipm_l > ipm_u ? ipm_l : ipm_u);
	if (mpm[0] == mpm[1]) {
		mpm[0] = 0;
		mpm[1] = ((u8)(((mpm[1]) == 0 ? 2 : (mpm[1]))));
	}*///by ldm
	u8 mpm0_tmp = (ipm_l < ipm_u ? ipm_l : ipm_u);
	u8	mpm1_tmp = (ipm_l > ipm_u ? ipm_l : ipm_u);
		if (mpm0_tmp == mpm1_tmp) {
			mpm[0] = 0;
			mpm[1] = ((u8)(((mpm1_tmp) == 0 ? 2 : (mpm1_tmp))));
		}
		else {
			mpm[0] = mpm0_tmp;
			mpm[1] = mpm1_tmp;
		}
}

void prepare_intra_info_8(MD_FW*md_fw_ptr, 
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	MD_COM_MODE* mod_info_curr_ptr,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8, 
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8,
	U3 cu_width_log2, U3 cu_height_log2, U13 pix_x, U13 pix_y, U14 sub_x, U14 sub_y,
	u8 ipred_list[33], int *pred_cnt, pel src_le_temp[2 * 8 + 3], pel src_up_temp[2 * 8 + 3], u16 *avail_tb, 
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE]
#else
	u8 rmd_mode[IPD_RDO_CNT]
#endif
#if USE_SPEED_LEVEL
	,U8 speed_level
#endif
)
{
	int pb_x_scu = mod_info_curr_ptr->x_pos >> 2;
	int pb_y_scu = mod_info_curr_ptr->y_pos >> 2;
	int tb_x_scu;
	int tb_y_scu;
	int i;
	int bit_depth;
	cu_height_log2= 3;
	cu_width_log2 =3;
	U8 h = (1 << cu_height_log2);
	U8 w = (1 << cu_width_log2);
#pragma HLS INLINE
#pragma HLS   ARRAY_PARTITION variable=src_le_temp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=src_up_temp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=rmd_mode complete dim=0
#pragma HLS   ARRAY_PARTITION variable=ipred_list complete dim=0
	com_get_mpm_8x8(pb_x_scu, pb_y_scu, mod_info_curr_ptr->mpm[0], MPM_buffer_ptr_8x8, me_line_map_ptr_8x8);
	*pred_cnt = get_rmd_result_8(cu_width_log2, cu_height_log2, pix_x, pix_y, ipred_list, rmd_mode
#if MIPF
		, md_fw_ptr->eipm_enable_flag
#endif
#if USE_SPEED_LEVEL
		,speed_level
#endif
	);

	bit_depth = md_fw_ptr->bit_depth_internal;
	tb_x_scu = sub_x >> 2;
	tb_y_scu = sub_y >> 2;
	*avail_tb = com_get_avail_intra_ip_8(IP_map_scu_ptr_8x8,
		tb_x_scu, tb_y_scu);

	for (i = 0; i < ((h << 1) + 3); i++) {
#pragma HLS loop_tripcount max = 19
#pragma HLS UNROLL
		src_le_temp[i] = (1 << (bit_depth - 1));
	}
	for (i = 0; i < ((w << 1) + 3); i++) {
#pragma HLS loop_tripcount max = 19
#pragma HLS UNROLL
		src_up_temp[i] = (1 << (bit_depth - 1));
	}
	get_IP_pixel_8(w, h, *avail_tb, md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu, bit_depth, 0, src_le_temp, src_up_temp, sub_x, sub_y, IP_buffer_ptr_8x8,
		IP_map_scu_ptr_8x8);
}

void rdo_prepare_8(MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4],
	ENC_ME_NEB_INFO left_nebs[2],
	SKIP_MERGE_INPUT* skip_in, MD_COM_MODE* mod_info_curr_ptr,
	MERGE_RDO* merge_rdo, MD_INPUT* md_input_ptr,
	U1 valid_mode[17], U1 amvr_enable_flag,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8,
	U3 cu_width_log2, U3 cu_height_log2,
	u8 ipred_list[IPRED_LIST_NUM], pel src_le_temp[2 * 8 + 3], pel src_up_temp[2 * 8 + 3], u16* avail_tb,
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE],
#else
	u8 rmd_mode[IPD_RDO_CNT],
#endif
	U3 qt_depth, U5 all_inter_mode_num,
	U7 cu_width, U7 cu_height,
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, MV_DIR* mv_dir,
#endif
	pel* org_y,
	U2 tree_status, u8 curr_cons_pred_mode,
	u8 inter_num[1], u8 intra_num[1], u8 rdo_list[MAX_RDO_NUM_SIZE(8, 8)], SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], SKIP_RDO* skip_rdo,
	SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], strFetch_ref_window* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
#pragma HLS PIPELINE
	s8 refi_cur = 0;
#if ENABLE_BFRAME
	s8 num_refp_cnt[REFP_NUM] = { md_fw_ptr->num_refp[0], md_fw_ptr->num_refp[1] };
#else
	u8 num_refp_cnt = md_fw_ptr->num_refp;
#endif
	U3 max_rdo_num = /*MD_MAX_RDO_NUM*/MAX_RDO_NUM_SIZE(8, 8);
	U3 max_inter = 0;
	int pred_cnt[1];
	U14 sub_x = mod_info_curr_ptr->x_pos;
	U14 sub_y = mod_info_curr_ptr->y_pos;

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;

#pragma HLS   ARRAY_PARTITION variable=	valid_mode complete dim=0
#pragma HLS   ARRAY_PARTITION variable=	rdo_list complete dim=0

	int num_amvr;
	if (amvr_enable_flag) {
		num_amvr = 5;
	}
	else {
		num_amvr = 1;
	}

//	inter_num[0] = 0;
//	intra_num[0] = 0;
	int inter_num_tmp=0;
	//printf("pos=(%d %d) size=(%d %d) slice=%d cons_pred_mode=%d\n", mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, cu_width, cu_height, md_fw_ptr->slice_type, curr_cons_pred_mode);
	if (!(md_fw_ptr->slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA)) {
		max_inter = (curr_cons_pred_mode == ONLY_INTER) ? MAX_RDO_NUM_SIZE(8, 8) : MAX_RDO_NUM_INTER_SIZE(8, 8);
		prepare_luma_inter_8(md_fw_ptr, me_line_map_ptr_8x8,
			up_nebs, left_nebs, skip_in, mod_info_curr_ptr,
			skip_rdo, skip_rdo_mode, merge_rdo, md_input_ptr,
			num_amvr, md_input_ptr->lambda_mv, refi_cur, num_refp_cnt, md_fw_ptr->cur_ptr,
			md_fw_ptr->refp_ptr, mvp_all, valid_mode, amvr_enable_flag, fme_mv_info,
#if ENABLE_BFRAME
			me_mv_dir, mv_dir,
#endif
			sub_pel_mv, skip_mode, Fetch_Ref_window_ptr
#if SUB_TMVP
			, core_sbtmvp
#endif
		);

		//limit_inter_rdo_num_8(model_cnt, max_inter, all_inter_mode_num, md_fw_ptr, me_line_map_ptr, inner_nebs, above_nebs_line, cost_best_temp, md_input_ptr, num_refp_cnt,
		//	cu_width, cu_height, mvp_all, mv_all, mecost_all, RefWindowLCU, CtrPos, mod_info_curr_ptr, mod_info_curr_pb_info, mod_info_curr_tb_info,
		//	dist_pred, org_y, stride_org, bit_cnt, refi_cur, p_fenc_LCU_Y,
		//	skip_rdo, skip_rdo_mode, merge_rdo, cu_width_log2, cu_height_log2, resi_t, tree_status, curr_cons_pred_mode, valid_flag, valid_mode, sub_pel_mv, pred_cache);
/*		if (valid_mode[9])
		{
			rdo_list[inter_num[0]] = 9;
			inter_num[0]++;
		}

#ifndef MD_CMOD
		for (int mode_index = 0; mode_index < 4; mode_index++)
		{
//#pragma HLS UNROLL
#pragma HLS PIPELINE
			if (inter_num[0] >= max_inter)
				break;
			if (valid_mode[mode_index] == 1)
			{
				rdo_list[inter_num[0]] = mode_index;
				inter_num[0]++;
			}
		}
#endif*/
		int valid_num = valid_mode[9] + valid_mode[0] + valid_mode[1] + valid_mode[2] + valid_mode[3];
		inter_num_tmp = valid_num >= max_inter ? max_inter : valid_num;
			inter_num[0] = inter_num_tmp;
//			int rdo_list[5];

			int k = 0;
			if (valid_mode[9])
			{
				rdo_list[k] = 9;
				k++;
			}
#ifndef MD_CMOD
			for (int mode_index = 0; mode_index < 4; mode_index++)
			{
#pragma HLS UNROLL
				if (valid_mode[mode_index])
				{
					rdo_list[k] = mode_index;
					k++;
				}
			}
#endif
		//return;
	}


	if (!(IS_INTER_SLICE(md_fw_ptr->slice_type) && curr_cons_pred_mode == ONLY_INTER))
	{
		prepare_intra_info_8(md_fw_ptr,
			me_line_map_ptr_8x8,
			mod_info_curr_ptr, IP_buffer_ptr_8x8,
			MPM_buffer_ptr_8x8,
			IP_map_scu_ptr_8x8,
			cu_width_log2,
			cu_height_log2, pix_x, pix_y, sub_x, sub_y, ipred_list, pred_cnt, src_le_temp, src_up_temp, avail_tb, rmd_mode
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

		intra_num[0] = avs3_min(pred_cnt[0], (int)(max_rdo_num - inter_num_tmp));
		for (int intra_index = 0; intra_index < 5; intra_index++)
		{

#pragma HLS loop_tripcount max = 5
#pragma HLS UNROLL
			if(intra_index < intra_num[0])
			rdo_list[inter_num_tmp + intra_index] = all_inter_mode_num + intra_index;
		}
	}
}

static void intra_inter_pred_8(U7 w, U7 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, u16 avail_tb, u8 rdo_idx,
	MD_FW* md_fw_ptr, SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MD_COM_MODE* mod_info_curr_ptr,
	u8 rdo_list[MAX_RDO_NUM_SIZE(8, 8)], U1 valid_mode[17], u8 intra_mode_list[IPRED_LIST_NUM], pel src_le_temp[8 * 2 + 3], pel src_up_temp[8 * 2 + 3], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], pel orgY[256],
	u8* curr_mvr_for_re, u8* mode_type_for_ssd_fz, s8* inter_mode_idx_for_ssd_fz, U1* valid_flag_for_ssd_fz, U1* is_intra_for_quant,
	S3	mod_info_refi_for_re_fz[2], s16 mod_info_mvd_for_re_fz[2][2], S14 mod_info_mv_for_up[2][2], s16 mod_info_affine_mvd_for_re_fz[2][4][2], S8 mod_info_ipm_for_re_fz[4][2],
	pel orgY_for_ssd_forced_zeros[8][8], pel diff[8][8], pel pred_for_ssd_forced_zeros[8][8], U4 bit_depth, 
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS],
	u8 *num_refp_cnt_for_ssd,
	u8 *num_refp_cnt_for_rate,
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif // ENABLE_BFRAME
	//pel fme2mdrefbuf[2560][8][2][16],
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[8][8], pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 8) - 1][8][8], pel pred_y_intra[MAX_RDO_NUM_SIZE(8, 8)][8][8]
#if USE_SPEED_LEVEL
	, U8 speed_level
#endif
)
{
#pragma HLS INLINE
	s8 intra_mode_idx;
	s8 inter_mode_idx;
	u8 mode_type = 0;
	u8 curr_mvr = 0;
	U1 valid_flag = 0;
	U1 is_intra;

	static S3 refi_for_ip[2];
	static s16 mvd_for_ip[2][2];
	static S14 mv_for_ip[2][2];
	static s16 affine_mvd_for_ip[2][4][2];
	static S8 ipm_for_ip[4][2];

	static pel pred_for_diff[8][8];
#pragma HLS ARRAY_PARTITION variable=md_fw_ptr->num_refp complete dim=0

	inter_mode_idx = rdo_list[rdo_idx];

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

	if (is_intra != 0)
	{
#pragma HLS ARRAY_PARTITION variable=orgY  cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=src_le_temp complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up_temp complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_for_diff complete dim=0
#pragma HLS ARRAY_PARTITION variable=diff complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv_for_ip complete dim=0
#pragma HLS ARRAY_PARTITION variable=mvd_copy complete dim=0

		Ipred_8_diff(w, h, orgY, pred_y_intra[rdo_idx], diff);
		memcpy_2d<pel, 8, 8>(pred_y_intra[rdo_idx], pred_for_ssd_forced_zeros);

		ipm_for_ip[0][0] = intra_mode_idx;
		ipm_for_ip[0][1] = -1;
		//valid_flag = 1;

	}

	else {
		static s16 mv_all[2][5][2];
		static int best_mv_uni_inner[2][4][2];
#if ENABLE_BFRAME
		u8 num_refp_cnt = md_fw_ptr->num_refp[mv_dir->lidx];
		*num_refp_cnt_for_rate = md_fw_ptr->num_refp[mv_dir->lidx];
		*num_refp_cnt_for_ssd = md_fw_ptr->num_refp[mv_dir->lidx];
#else
		u8 num_refp_cnt = md_fw_ptr->num_refp;
		*num_refp_cnt_for_rate = md_fw_ptr->num_refp;
		*num_refp_cnt_for_ssd = md_fw_ptr->num_refp;
#endif
		int refi_cur = 0;
		valid_flag = valid_mode[inter_mode_idx];
		static U2 mode_type_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2 };
		static u8 curr_mvr_table[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 1, 2 };
		/*if (valid_flag == 1) */{
			mode_type = mode_type_table[inter_mode_idx];
			curr_mvr = curr_mvr_table[inter_mode_idx];
			U14 pic_height = md_fw_ptr->pic_height_in_scu << 2;
			U14 pic_width = md_fw_ptr->pic_width_in_scu << 2;
			if (mode_type == 0 || mode_type == 1)
			{
				if (mode_type == 1) {
					mod_info_curr_ptr->cu_mode = 1;
					mod_info_curr_ptr->affine_flag = 0;
					//init_inter_data_8(mod_info_curr_ptr);
					for (int i = 0; i < 2; i++) {
#pragma HLS UNROLL
						for (int j = 0; j < 2; j++) {
#pragma HLS UNROLL
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

					memcpy_2d<pel, 8, 8>(pred_y_inter1, pred_for_ssd_forced_zeros);
				}
				else if (mode_type == 2)
				{
#if MD_PRINT
					__assert_HLS("affine is not support in md kernel 8.", "analyze_uni_pred_8", (unsigned int)3468, __FUNCTION__);
#endif
				}
				else if (mode_type == 0)
				{
#ifndef MD_CMOD
					//make_skip_resi_8(inter_mode_idx, skip_rdo, skip_rdo_mode, &merge_rdo, mod_info_curr_ptr, pic_width, pic_height,
					//	cu_width_log2, cu_height_log2, orgY, 64, w, h, diff, pred_for_diff, bit_depth,
					//	refi_for_ip, mv_for_ip);
					derive_mode_info_8(inter_mode_idx, skip_rdo, skip_rdo_mode, &merge_rdo, mod_info_curr_ptr, refi_for_ip, mv_for_ip);
#endif	
					//CU_POS cu = { (int)mod_info_curr_ptr->scup, mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, mod_info_curr_ptr->cu_width, mod_info_curr_ptr->cu_height };

					//if (!com_mc_cu_y_8(&cu, pic_width, pic_height, refi_for_ip, mv_for_ip,
					//	SWW, pred_for_diff, bit_depth, Fetch_Ref_window_ptr,
					//	CtrPosSKIP,
					//	CtrPosSKIP_ref1,
					//	fme2mdrefbuf
					//	, mode_type == 1 ? MODE_INTER : MODE_DIR))
					//{
					//	//valid_flag = 0;
					//}
					//for (int i = 0; i < 8; i++) {
					//	for (int j = 0; j < 8; j++) {
					//		pred_for_diff[i][j] = pred_y_inter2[rdo_idx-1][i][j];
					//	}
					//}
					memcpy_2d<pel, 8, 8>(pred_y_inter2[rdo_idx - 1], pred_for_ssd_forced_zeros);
				}
//				CU_POS cu = { (int)mod_info_curr_ptr->scup, mod_info_curr_ptr->x_pos, mod_info_curr_ptr->y_pos, mod_info_curr_ptr->cu_width, mod_info_curr_ptr->cu_height };
//#if ENABLE_BFRAME
//				if (!REFI_IS_VALID(refi_for_ip[REFP_0]) && !REFI_IS_VALID(refi_for_ip[REFP_1]))
//				{
//					//valid_flag = 0;
//				}
//				if (!com_mc_cu_y_8(&cu, pic_width, pic_height, refi_for_ip, mv_for_ip,
//					SWW, pred_for_diff, bit_depth, Fetch_Ref_window_ptr,
//					CtrPosSKIP,
//					CtrPosSKIP_ref1,
//					fme2mdrefbuf
//					, mode_type == 1 ? MODE_INTER : MODE_DIR))
//				{
//					//valid_flag = 0;
//				}
//
//#else
//				if (!com_mc_y_8(mode_type, &cu, pic_width, pic_height, refi_for_ip, mv_for_ip,
//					SWW, pred_for_diff, bit_depth, tree_status, 
//					CtrPosSKIP, fme2mdrefbuf
//					))
//				{
//					valid_flag = 0;
//				}
//#endif
				

				enc_diff_16b_8(cu_width_log2, cu_height_log2, orgY, pred_for_ssd_forced_zeros, 8, w, w, diff);
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

		//memcpy_1d<pel, 64>(orgY, orgY_for_ssd_forced_zeros);
		for (int i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			for (int j = 0; j < 8; j++)
			{
#pragma HLS UNROLL
				orgY_for_ssd_forced_zeros[i][j] = orgY[8 * i + j];
			}
		}
		
		//memcpy_2d<pel, 8, 8>(pred_for_diff, pred_for_ssd_forced_zeros);
}

void rdo_skip_8(s64* cost_best, MD_COM_MODE_BEST* best_info_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	MD_COM_MODE* mod_info_curr, pel org_y[64],
	SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], U14 pic_width, U14 pic_height, U2 slice_type, U2 tree_status, s64 lambda_y, U2 curr_cons_pred_mode, U4 bit_depth,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS], 
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif // ENABLE_BFRAME
	//pel fme2mdrefbuf[2560][8][2][16]
	pel pred_y_skip_inter[13][8][8]
)
{
	pel pred_y_skip_inter_cp[8][8];
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->num_nz complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->mvd complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mvd complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->pred_y cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->pred_y cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter complete dim=3
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter_cp complete dim=0
#pragma HLS ARRAY_PARTITION variable=best_info_ptr->num_nz complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data_ptr->coef_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=cu_data_ptr->reco_y complete dim=0

	U3 cu_width_log2 = mod_info_curr->cu_width_log2;

	if (slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA) { return; }

	int valid_skip_num = 0;

	for (int idx = 0; idx < SKIP_MODE_NUM; ++idx) {
#pragma HLS pipeline II=1
#pragma HLS loop_tripcount max=12
		// derive mode info
		if (valid_skip_num < SKIP_MODE_NUM_SIZE(8, 8))
		{
			SKIP_ELEMENT* mode = skip_mode + idx;
#pragma HLS ARRAY_PARTITION variable=mode->mvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode->refi complete dim=0
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
#if ENABLE_BFRAME
			if (!REFI_IS_VALID(mod_info_curr->refi[REFP_0]) && !REFI_IS_VALID(mod_info_curr->refi[REFP_1])) { continue; }
#else
			if (mod_info_curr->refi[0] == -1) { continue; }
#endif

			valid_skip_num++;

			s16 pred;
			
			for (int i = 0; i < 8; i++)
#pragma HLS UNROLL
				for (int j = 0; j < 8; j++)
					pred_y_skip_inter_cp[i][j] = pred_y_skip_inter[idx][i][j];

			s64 cost = enc_ssd_16b_1_8(mod_info_curr->cu_width_log2, mod_info_curr->cu_height_log2, pred_y_skip_inter_cp, org_y, mod_info_curr->cu_width, 8, bit_depth);
			
			for (int i = 0; i < 4; i++)
#pragma HLS UNROLL
				for (int j = 0; j < 3; j++)
					mod_info_curr->num_nz[i][j] = 0;

			mod_info_curr->tb_part = ((int)SIZE_2Nx2N);

			// calc bit est
			U16 bit_est = enc_bit_est_flag_skip_8(slice_type, tree_status, mod_info_curr->umve_flag, mod_info_curr->umve_idx, mod_info_curr->affine_flag, mod_info_curr->skip_idx, mod_info_curr->cu_width_log2, mod_info_curr->cu_height_log2, mod_info_curr->num_nz, mod_info_curr->ipm, 8);
			cost += ((bit_est * lambda_y + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE);

			// compare cost
			if (cost < *cost_best) {

				best_info_ptr->cu_mode = mod_info_curr->cu_mode;
				best_info_ptr->pb_part = mod_info_curr->pb_part;
				best_info_ptr->tb_part = SIZE_2Nx2N;
				best_info_ptr->umve_flag = mod_info_curr->umve_flag;
				*cost_best = cost;
				best_info_ptr->mvr_idx = ((u8)0);
				best_info_ptr->refi[0] = mod_info_curr->refi[0];
				best_info_ptr->refi[1] = mod_info_curr->refi[1];
				for (int lidx = 0; lidx < 2; lidx++) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 2
					best_info_ptr->mv[lidx][0] = mod_info_curr->mv[lidx][0];
					best_info_ptr->mv[lidx][1] = mod_info_curr->mv[lidx][1];
					best_info_ptr->mvd[lidx][0] = mod_info_curr->mvd[lidx][0];
					best_info_ptr->mvd[lidx][1] = mod_info_curr->mvd[lidx][1];
				}
				best_info_ptr->smvd_flag = mod_info_curr->smvd_flag;
				best_info_ptr->affine_flag = mod_info_curr->affine_flag;
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

				for (int i = 0; i < 8; ++i) {
#pragma HLS UNROLL
					for (int j = 0; j < 8; ++j) {
#pragma HLS UNROLL
						if (j < mod_info_curr->cu_width && i < mod_info_curr->cu_height) {
							cu_data_ptr->coef_y[i][j] = 0;
							pred = pred_y_skip_inter[idx][i][j];
							best_info_ptr->pred_y[i][j] = pred;
							cu_data_ptr->reco_y[i][j] = (0 > pred) ? 0 : (pred > ((1 << bit_depth) - 1) ? ((1 << bit_depth) - 1) : pred);
						}
					}
				}
			}
		}
	}
}

s64 enc_ssd_16b_chroma_8(U7 w, U7 h, s16 src1[8][8], s16 src2[8][8],  U4 bit_depth)
{
#pragma HLS INLINE off

	const U2 shift = (bit_depth - 8) << 1;
	s64 ssd = 0;

	U7 i, j;
	int diff;

	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < 8; j++) {
			if (j < w) {
			diff = src1[i][j] - src2[i][j];
			ssd += ((diff * diff >> shift));
			}
		}
	}
	return ssd;

}

static void update_chroma_8(ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef_y_pb_part[(1 << 7 >> 1) * (1 << 7 >> 1)], U1 i, U1 coef_rec_write_flag[1], U2 inter_mode, pel pred[2][8][8], pel rec[2][8][8], int num_nz[4][3], U4 bit_depth)
{
	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	U7 cu_width = mod_info_curr_ptr->cu_width;
	U7 cu_height = mod_info_curr_ptr->cu_height;
	U12 size_tmp = cu_width * cu_height >> 2;
	int w = cu_width >> 1, h = cu_height >> 1;
	int j, k;

	if (inter_mode == 2) {
		num_nz[0][i + 1] = 0;
	}
	if (num_nz[0][i + 1] == 0) {

		for (k = 0; k < h; ++k) {
			for (j = 0; j < w; j++) {
				int d = k * w + j;
				if (i == 0)cu_data_temp_ptr->coef_u[d] = 0;
				else if (i == 1)cu_data_temp_ptr->coef_v[d] = 0;
				if (i == 0) {
					cu_data_temp_ptr->reco_u[d] = mod_info_curr_ptr->rec_u[d] = mod_info_curr_ptr->pred_u[d] = (((0 > (((1 << bit_depth) - 1 < (pred[i][k][j]) ?
						(1 << bit_depth) - 1 : (pred[i][k][j]))) ? 0 : (((1 << bit_depth) - 1 < (pred[i][k][j]) ? (1 << bit_depth) - 1 : (pred[i][k][j]))))));
				}
				else if (i == 1) {
					cu_data_temp_ptr->reco_v[d] = mod_info_curr_ptr->rec_v[d] = mod_info_curr_ptr->pred_v[d] = (((0 > (((1 << bit_depth) - 1 < (pred[i][k][j]) ?
						(1 << bit_depth) - 1 : (pred[i][k][j]))) ? 0 : (((1 << bit_depth) - 1 < (pred[i][k][j]) ? (1 << bit_depth) - 1 : (pred[i][k][j]))))));
				}
			}
		}
		coef_rec_write_flag[0] = 1;
	}
	else {
		//mod_info_curr_ptr->num_nz[0][i + 1] = mod_info_best_ptr->num_nz[0][i + 1];
		for (k = 0; k < h; ++k) {
			for (j = 0; j < w; j++) {
				int d = k * w + j;
				if (i == 0) {
					cu_data_temp_ptr->coef_u[d] = coef_y_pb_part[d];
					cu_data_temp_ptr->reco_u[d] = mod_info_curr_ptr->rec_u[d] = ((pel)((0 > (((1 << bit_depth) - 1 < (rec[i][k][j]) ?
						(1 << bit_depth) - 1 : (rec[i][k][j]))) ? 0 : (((1 << bit_depth) - 1 < (rec[i][k][j]) ? (1 << bit_depth) - 1 : (rec[i][k][j]))))));

				}
				else if (i == 1) {
					cu_data_temp_ptr->coef_v[d] = coef_y_pb_part[d];
					cu_data_temp_ptr->reco_v[d] = mod_info_curr_ptr->rec_v[d] = ((pel)((0 > (((1 << bit_depth) - 1 < (rec[i][k][j]) ?
						(1 << bit_depth) - 1 : (rec[i][k][j]))) ? 0 : (((1 << bit_depth) - 1 < (rec[i][k][j]) ? (1 << bit_depth) - 1 : (rec[i][k][j]))))));
				}
			}
		}
		coef_rec_write_flag[0] = 1;
	}
}


void ipred_ang_val_Chroma8(s16 pred_cache_16x16[8][8], pel src_up[19], pel src_le[19], s8 ipm, U4 bit_depth, int m, int n, int w, int h, s16 pred_1[8][8]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_1 complete dim=2

	u8 i, j;
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


#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_bilinear complete dim=0
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_extra_smooth complete dim=0
#pragma HLS ARRAY_PARTITION variable=com_tbl_ipred_adi_semi_sharp complete dim=0
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_hp complete dim=0
	
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };
	const int filter_bits_list[4] = { 7, 7, 6, 7 };
#pragma HLS ARRAY_PARTITION variable=filter_offset_list complete dim=0
#pragma HLS ARRAY_PARTITION variable=filter_bits_list complete dim=0
	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;

	static const s16 ipred_dxdy1[9] = {  (2816),  (2048),  (1408),  (1024),  (744) ,  (512) ,  (372) ,  (256) ,  (128)};
	static const s16 ipred_dxdy2[8] = { (128)  , (256)  , (372)  , (512)  , (744)  , (1024) , (1408) , (2048)  };
	static const s16 ipred_dxdy3[11][2] = { { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) } };
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ipred_dxdy3 complete dim=0

	int offset;
	int offset_x;
	int offset_y;
	int t_dx;
	int t_dy;
	int xx;
	int xy;
	int yx;
	int yy;
	int x;
	int y;
	int xn;
	int yn;
	int xn_n1;
	int yn_n1;
	int xn_p2;
	int yn_p2;
	int p;
	int pn;
	int pn_n1;
	int pn_p2;
	x = 2147483647;
	xn = 2147483647;
	xn_n1 = 2147483647;
	xn_p2 = 2147483647;
	y = 2147483647;
	yn = 2147483647;
	yn_n1 = 2147483647;
	yn_p2 = 2147483647;
	xx = 2147483647;
	yy = 2147483647;
	pel temp_pel = (pel)0;
	int use_x = -1;
	int pos_max;
	int filter_idx;

	int h_flag = ((ipm >= 3 && ipm <= 11) || (ipm >= 25 && ipm <= 32)) ? h : 8;
	int w_flag = ((ipm >= 3 && ipm <= 11) || (ipm >= 25 && ipm <= 32)) ? w : 8;
	const int dxy = ipm > 24 || ipm < 12 ? -1 : 1;

	for (j = 0; j < h_flag; j++) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (i = 0; i < 8; i++) {
			if (i < w_flag)
			{
				if ((ipm < 12) || ((ipm >= 13) && (ipm <= 17)) || ((ipm >= 34) && (ipm <= 50))) {
					filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;// MIPF
				}
				else if (((ipm >= 18) && (ipm <= 23)) || ((ipm >= 25) && (ipm <= 32)) || ((ipm >= 51) && (ipm <= 65))) {
					filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;// MIPF
				}

				int num_selections = 0;

				if (ipm >= 3 && ipm <= 11) {
					/* case x-line */
					t_dx = (j + 1) * ipred_dxdy1[ipm - 3] >> 10;
					offset = ((j + 1) * ipred_dxdy1[ipm - 3] >> 5) - (t_dx << 5);
					;
					x = i + t_dx;
					y = -1;
				}
				else if (ipm >= 25 && ipm <= 32) {
					t_dy = (i + 1) * ipred_dxdy2[ipm - 25] >> 10;
					offset = ((i + 1) * ipred_dxdy2[ipm - 25] >> 5) - (t_dy << 5);
					;
					x = -1;
					y = j + t_dy;
				}
				else {
					t_dx = (j + 1) * ipred_dxdy3[ipm - 13][0] >> 10;
					offset_x = ((j + 1) * ipred_dxdy3[ipm - 13][0] >> 5) - (t_dx << 5);
					;
					t_dx = -t_dx;
					xx = i + t_dx;
					yx = -1;
					t_dy = (i + 1) * ipred_dxdy3[ipm - 13][1] >> 10;
					offset_y = ((i + 1) * ipred_dxdy3[ipm - 13][1] >> 5) - (t_dy << 5);
					;
					t_dy = -t_dy;
					xy = -1;
					yy = j + t_dy;

					if (yy <= -1) {
						x = xx;
						y = yx;
						offset = offset_x;
					}
					else {
						x = xy;
						y = yy;
						offset = offset_y;
					}
				}

				if (y == -1) {
					pos_max = w * 2 - 1;
					if (dxy < 0) {
						xn_n1 = x - 1;
						xn = x + 1;
						xn_p2 = x + 2;
						xn_n1 = (xn_n1 < pos_max ? xn_n1 : pos_max);
					}
					else {
						xn_n1 = x + 1;
						xn = x - 1;
						xn_p2 = x - 2;
					}
					use_x = 1;
					++num_selections;

				}
				else if (x == -1) {
					pos_max = h * 2 - 1;
					if (dxy < 0) {
						yn_n1 = y - 1;
						yn = y + 1;
						yn_p2 = y + 2;
						yn_n1 = (yn_n1 < pos_max ? yn_n1 : pos_max);
					}
					else {
						yn_n1 = y + 1;
						yn = y - 1;
						yn_p2 = y - 2;
					}
					use_x = 0;
					++num_selections;


				}

				if (use_x == 1) {
					pn_n1 = xn_n1;
					p = x;
					pn = xn;
					pn_p2 = xn_p2;
				}
				else {
					pn_n1 = yn_n1;
					p = y;
					pn = yn;
					pn_p2 = yn_p2;
				}

				p = (p < pos_max ? p : pos_max);
				pn = (pn < pos_max ? pn : pos_max);
				pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);

				pn_n1 += 3;
				p += 3;
				pn += 3;
				pn_p2 += 3;

				int src_p, src_pn, src_pn_n1, src_pn_p2;

				if (y == -1) {
					src_pn_n1 = src_up[pn_n1];
					src_p = src_up[p];
					src_pn = src_up[pn];
					src_pn_p2 = src_up[pn_p2];
				}
				else if (x == -1) {
					src_pn_n1 = src_le[pn_n1];
					src_p = src_le[p];
					src_pn = src_le[pn];
					src_pn_p2 = src_le[pn_p2];
				}
				else {
					src_pn_n1 = 0;
					src_p = 0;
					src_pn = 0;
					src_pn_p2 = 0;
				}

#if 0
				if (filter_idx == 0)
					temp_pel = (pel)((src_pn_n1 * com_tbl_ipred_adi_extra_smooth[offset][0] + src_p * com_tbl_ipred_adi_extra_smooth[offset][1] +
						src_pn * com_tbl_ipred_adi_extra_smooth[offset][2] + src_pn_p2 * com_tbl_ipred_adi_extra_smooth[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// MIPF
				else if (filter_idx == 1)
					temp_pel = (pel)((src_pn_n1 * com_tbl_ipred_adi_semi_sharp[offset][0] + src_p * com_tbl_ipred_adi_semi_sharp[offset][1] +
						src_pn * com_tbl_ipred_adi_semi_sharp[offset][2] + src_pn_p2 * com_tbl_ipred_adi_semi_sharp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// MIPF
				else if (filter_idx == 2)
					temp_pel = (pel)((src_pn_n1 * tbl_mc_c_coeff_hp[offset][0] + src_p * tbl_mc_c_coeff_hp[offset][1] +
						src_pn * tbl_mc_c_coeff_hp[offset][2] + src_pn_p2 * tbl_mc_c_coeff_hp[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// MIPF
				else if (filter_idx == 3)
					temp_pel = (pel)((src_pn_n1 * com_tbl_ipred_adi_bilinear[offset][0] + src_p * com_tbl_ipred_adi_bilinear[offset][1] +
						src_pn * com_tbl_ipred_adi_bilinear[offset][2] + src_pn_p2 * com_tbl_ipred_adi_bilinear[offset][3] +
						filter_offset_list[filter_idx]) >> filter_bits_list[filter_idx]);// MIPF
#else
                    s16 adi_tmp[4];
#pragma HLS ARRAY_PARTITION variable=adi_tmp complete dim=0
                    int filter_offset;
                    int filter_bits;

                    if (filter_idx == 0) {
                        for (int idx = 0; idx < 4; idx++){
#pragma HLS UNROLL
                            adi_tmp[idx] = com_tbl_ipred_adi_extra_smooth[offset][idx];
                        }
                        filter_offset = filter_offset_list[0];
                        filter_bits = filter_bits_list[0];
                    }
                    else if (filter_idx == 1) {
                        for (int idx = 0; idx < 4; idx++){
#pragma HLS UNROLL
                            adi_tmp[idx] = com_tbl_ipred_adi_semi_sharp[offset][idx];
                        }
                        filter_offset = filter_offset_list[1];
                        filter_bits = filter_bits_list[1];
                    }
                    else if (filter_idx == 2) {
                        for (int idx = 0; idx < 4; idx++){
#pragma HLS UNROLL
                            adi_tmp[idx] = tbl_mc_c_coeff_hp[offset][idx];
                        }
                        filter_offset = filter_offset_list[2];
                        filter_bits = filter_bits_list[2];
                    }
                    else if (filter_idx == 3) {
                        for (int idx = 0; idx < 4; idx++){
#pragma HLS UNROLL
                            adi_tmp[idx] = com_tbl_ipred_adi_bilinear[offset][idx];
                        }
                        filter_offset = filter_offset_list[2];
                        filter_bits = filter_bits_list[2];
                    }
                    temp_pel= (pel)((src_pn_n1 * adi_tmp[0] + src_p * adi_tmp[1] + src_pn * adi_tmp[2] + src_pn_p2 * adi_tmp[3] + filter_offset) >> filter_bits);
#endif

				temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pel);
				pred_cache_16x16[j][i] = temp_pel;
				pred_1[j][i] = temp_pel;
			}
		}
		


	}

}

void ipred_ang_Chroma8(pel src_le[19], pel src_up[19], s16 dst[8][8], U4 bit_depth, U7 w, U7 h, S8 ipm,  s16 pred_1[8][8]
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{

	ipred_ang_val_Chroma8(dst, src_up, src_le, ipm, bit_depth, 0, 0, w, h,  pred_1
#if MIPF
		, is_luma, mipf_enable_flag
#endif
	);
}


 void com_ipred_uv_8(U3 cu_width_log2_c, U3 cu_height_log2_c,pel src_le[19], pel src_up[19], pel pred[8][8], S8 ipm_c, S8 ipm, U7 w, U7 h, U4 bit_depth,
	u16 avail_cu,  pel pred_1[8][8], pel piRecoY[16][16], pel nb_y[2][2 * 16 + 3]
#if MIPF
	, int mipf_enable_flag
#endif
)
{
#if USE_TSCPM
	int bAbove = IS_AVAIL(avail_cu, AVAIL_UP);
	int bLeft = IS_AVAIL(avail_cu, AVAIL_LE);
#endif

	if (ipm_c == 0 && (ipm == 12 || ipm == 24 || ipm == 0 || ipm == 2)) {
		ipm_c = (ipm == 12 ? 3 : ((ipm == 24 ? 2 : ((ipm == 0 ? 1 : 4)))));
	}

#if SIMPLE_MD
	Ipred_dc_8(cu_width_log2_c, cu_height_log2_c, w, h, src_le + 3, src_up + 3, bit_depth, avail_cu, pred, pred_1);
#else
		if ((ipm_c == 0) && (ipm == 1 )) {
			Ipred_plane_Chroma8( cu_width_log2_c,  cu_height_log2_c, w, h, src_le + 2, src_up + 2, bit_depth, pred, pred_1);
		}
			
#if EIPM

		else if ((ipm_c == 0) && (ipm > 33)) {
			ipred_ang_eipm_Chroma8(src_le + 3, src_up + 3, w, h, ipm
#if MIPF
				, 0, mipf_enable_flag, bit_depth, pred_1, pred
#endif
			);
		}
			
#endif
		else if ((ipm_c == 0) && (ipm < 34)&&(ipm !=1)) {
			ipred_ang_Chroma8(src_le, src_up, pred, bit_depth, w, h, ipm, pred_1
#if MIPF
				, 0, mipf_enable_flag
#endif
			);
		}
			
		else if (ipm_c == 1) {
			Ipred_dc_8( cu_width_log2_c,  cu_height_log2_c, w, h, src_le + 3, src_up + 3, bit_depth, avail_cu, pred, pred_1);
		}

		else if (ipm_c == 2) {
			Ipred_hor_8(w, h, src_le + 3, pred, pred_1);
		}

		else if (ipm_c == 3) {
			Ipred_vert_8(w, h, src_up + 3, pred, pred_1);
		}

		else if (ipm_c == 4) {
			Ipred_bi_8( cu_width_log2_c,  cu_height_log2_c, w, h, src_le + 3, src_up + 3, bit_depth, pred, pred_1);
		}

#if USE_TSCPM
		else if (ipm_c == 5) {
			ipred_tscpm_Chroma8(pred, pred_1, piRecoY, 64, w, h, bAbove, bLeft, bit_depth, src_le + 3, src_up + 3, nb_y[0], nb_y[1]);
		}
#endif
#endif
}

#if USE_TSCPM
 void Chroma8_const_up_16x16(pel* up, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu,
	 pel IP_buffer_ptr_16x16_up_line[2 * 16], U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1]
	 )
 {
	 if ((((int)avail_cu) & 1 << 0) == 1 << 0)
	 {
		 int flag;
		 for (int i = 0; i < 16; ++i)
		 {
#pragma HLS UNROLL
			 up[i + 3] = IP_buffer_ptr_16x16_up_line[i];
		 }
		 for (int i = 0; i < (16 >> 2); i++)
		 {
#pragma HLS UNROLL
			 if (pic_y == 0 ? x_scu + (16 >> 2) + i < pic_width_in_scu : pic_x_scu + (16 >> 2) + i < 16)
			 {
				 flag = IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) + i] >> 31 & 1;
			 }
			 else
			 {
				 flag = 0;
			 }
			 if (x_scu + (16 >> 2) + i < pic_width_in_scu && flag)
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 up[16 + (i << 2) + j + 3] = IP_buffer_ptr_16x16_up_line[16 + (i << 2) + j];
				 }
			 }
			 else
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 up[16 + (i << 2) + j + 3] = up[16 + (i << 2) + 2];
				 }
			 }
		 }
	 }
 }
 void Chroma8_const_up_16x8(pel* up, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu,
	 pel IP_buffer_ptr_16x8_up_line[2 * 16], U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1]
	 )
 {
	 if ((((int)avail_cu) & 1 << 0) == 1 << 0)
	 {
		 int flag;
		 for (int i = 0; i < 16; ++i)
		 {
#pragma HLS UNROLL
			 up[i + 3] = IP_buffer_ptr_16x8_up_line[i];
		 }
		 for (int i = 0; i < (16 >> 2); i++)
		 {
#pragma HLS UNROLL
			 if (pic_y == 0 ? x_scu + (16 >> 2) + i < pic_width_in_scu : pic_x_scu + (16 >> 2) + i < 16)
			 {
				 flag = IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) + i] >> 31 & 1;
			 }
			 else
			 {
				 flag = 0;
			 }
			 if (x_scu + (16 >> 2) + i < pic_width_in_scu && flag)
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 up[16 + (i << 2) + j + 3] = IP_buffer_ptr_16x8_up_line[16 + (i << 2) + j];
				 }
			 }
			 else
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 up[16 + (i << 2) + j + 3] = up[16 + (i << 2) + 2];
				 }
			 }
		 }
	 }
 }
 void Chroma8_const_up_8x16(pel* up, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu,
	 pel IP_buffer_ptr_8x16_up_line[2 * 8], U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1]
	 )
 {
	 if ((((int)avail_cu) & 1 << 0) == 1 << 0)
	 {
		 int flag;
		 for (int i = 0; i < 8; ++i)
		 {
#pragma HLS UNROLL
			 up[i + 3] = IP_buffer_ptr_8x16_up_line[i];
		 }
		 for (int i = 0; i < (8 >> 2); i++)
		 {
#pragma HLS UNROLL
			 if (pic_y == 0 ? x_scu + (8 >> 2) + i < pic_width_in_scu : pic_x_scu + (8 >> 2) + i < 16)
			 {
				 flag = IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) + i] >> 31 & 1;
			 }
			 else
			 {
				 flag = 0;
			 }
			 if (x_scu + (8 >> 2) + i < pic_width_in_scu && flag)
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 up[8 + (i << 2) + j + 3] = IP_buffer_ptr_8x16_up_line[8 + (i << 2) + j];
				 }
			 }
			 else
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 up[8 + (i << 2) + j + 3] = up[8 + (i << 2) + 2];
				 }
			 }
		 }
	 }
 }
 void Chroma8_const_left_16x16(pel* left, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu,
	 pel IP_buffer_ptr_16x16_left_line[2 * 16], U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1]
	 )
 {
	 if ((avail_cu & 1 << 1) == 1 << 1)
	 {
		 int flag;
		 for (int i = 0; i < 16; ++i)
		 {
#pragma HLS UNROLL
			 left[i + 3] = IP_buffer_ptr_16x16_left_line[i];
		 }
		 for (int i = 0; i < (16 >> 2); i++)
		 {
#pragma HLS UNROLL
			 if (pic_y_scu + (16 >> 2) + i < 16)
			 {
				 flag = IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) + i] >> 31 & 1;
			 }
			 else
			 {
				 flag = 0;
			 }
			 if (y_scu + (16 >> 2) + i < pic_height_in_scu && flag)
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 left[16 + (i << 2) + j + 3] = IP_buffer_ptr_16x16_left_line[16 + (i << 2) + j];
				 }
			 }
			 else
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 left[16 + (i << 2) + j + 3] = left[16 + (i << 2) + 2];
				 }
			 }
		 }
	 }
 }
 void Chroma8_const_left_16x8(pel* left,  int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu,
	 pel IP_buffer_ptr_16x8_left_line[2 * 8], U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1]
	 )
 {
	 if ((avail_cu & 1 << 1) == 1 << 1)
	 {
		 int flag;
		 for (int i = 0; i < 8; ++i)
		 {
#pragma HLS UNROLL
			 left[i + 3] = IP_buffer_ptr_16x8_left_line[i];
		 }
		 for (int i = 0; i < (8 >> 2); i++)
		 {
#pragma HLS UNROLL
			 if (pic_y_scu + (8 >> 2) + i < 16)
			 {
				 flag = IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) + i] >> 31 & 1;
			 }
			 else
			 {
				 flag = 0;
			 }
			 if (y_scu + (8 >> 2) + i < pic_height_in_scu && flag)
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 left[8 + (i << 2) + j + 3] = IP_buffer_ptr_16x8_left_line[8 + (i << 2) + j];
				 }
			 }
			 else
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 left[8 + (i << 2) + j + 3] = left[8 + (i << 2) + 2];
				 }
			 }
		 }
	 }
 }
 void Chroma8_const_left_8x16(pel* left,  int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu,
	 pel IP_buffer_ptr_8x16_left_line[2 * 16], U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1]
	 )
 {
	 if ((avail_cu & 1 << 1) == 1 << 1)
	 {
		 int flag;
		 for (int i = 0; i < 16; ++i)
		 {
#pragma HLS UNROLL
			 left[i + 3] = IP_buffer_ptr_8x16_left_line[i];
		 }
		 for (int i = 0; i < (16 >> 2); i++)
		 {
#pragma HLS UNROLL
			 if (pic_y_scu + (16 >> 2) + i < 16)
			 {
				 flag = IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) + i] >> 31 & 1;
			 }
			 else
			 {
				 flag = 0;
			 }
			 if (y_scu + (16 >> 2) + i < pic_height_in_scu && flag)
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 left[16 + (i << 2) + j + 3] = IP_buffer_ptr_8x16_left_line[16 + (i << 2) + j];
				 }
			 }
			 else
			 {
				 for (int j = 0; j < 4; j++)
				 {
#pragma HLS UNROLL
					 left[16 + (i << 2) + j + 3] = left[16 + (i << 2) + 2];
				 }
			 }
		 }
	 }
 }
 void Chroma8_const_top_left_16x16(pel* up, pel* left, int avail_cu, pel IP_buffer_ptr_16x16_top_left)
 {
	 if ((avail_cu & 1 << 2) == 1 << 2)
	 {
		 up[2] = left[2] = IP_buffer_ptr_16x16_top_left;
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
 void Chroma8_const_top_left_16x8(pel* up, pel* left, int avail_cu, pel IP_buffer_ptr_16x8_top_left)
 {
	 if ((avail_cu & 1 << 2) == 1 << 2)
	 {
		 up[2] = left[2] = IP_buffer_ptr_16x8_top_left;
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
 void Chroma8_const_top_left_8x16(pel* up, pel* left,  int avail_cu, pel IP_buffer_ptr_8x16_top_left)
 {
	 if ((avail_cu & 1 << 2) == 1 << 2)
	 {
		 up[2] = left[2] = IP_buffer_ptr_8x16_top_left;
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
void get_IP_pixel_16_for_chroma(U8 cu_width, U8 cu_height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu, U4 bit_depth, U2 ch_type, 
	pel left[2 * 16 + 3], pel up[2 * 16 + 3], U14 x, U14 y,

	pel	IP_buffer_ptr_8x16_up_line[2 * 8], pel IP_buffer_ptr_8x16_left_line[2 * 16], pel IP_buffer_ptr_8x16_top_left,
	pel	IP_buffer_ptr_16x8_up_line[2 * 16], pel IP_buffer_ptr_16x8_left_line[2 * 8], pel IP_buffer_ptr_16x8_top_left,
	pel	IP_buffer_ptr_16x16_up_line[2 * 16], pel IP_buffer_ptr_16x16_left_line[2 * 16], pel IP_buffer_ptr_16x16_top_left,

	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1]
)
{
#pragma HLS inline off
	U12 i;
	U12 j;
	int width_in_scu = ch_type == 0 ? cu_width >> 2 : cu_width >> (2 - 1);
	int height_in_scu = ch_type == 0 ? cu_height >> 2 : cu_height >> (2 - 1);
	//4
	U8 unit_size = ch_type == 0 ? 1 << 2 : 1 << 2 >> 1;
	U2 unit_size_bit = ch_type == 0 ? 2 : 1;
	//x/4
	int x_scu = ((ch_type == 0 ? x : x << 1)) >> 2;
	//y/4
	int y_scu = ((ch_type == 0 ? y : y << 1)) >> 2;
	//number of padding pixel in the left column   64
	int pad_le = cu_height;
	//number of padding pixel in the upper row     64
	int pad_up = cu_width;
	//16
	int pad_le_in_scu = height_in_scu;
	//16
	int pad_up_in_scu = width_in_scu;
	U8 pic_x = x & 63;
	U8 pic_y = y & 63;
	s16 pic_x_scu = (pic_x >> 2);
	s16 pic_y_scu = (pic_y >> 2);
	int flag;
	//U8 *up_temp_line;
	static pel up_temp_line[128];
	static pel left_line[64];

	S7 ip_array_size_idx = get_ip_array_size_idx(cu_width, cu_height);
#if MD_PRINT
	assert(ip_array_size_idx != -1);
#endif


#pragma HLS ARRAY_PARTITION variable=left complete dim=0
#pragma HLS ARRAY_PARTITION variable=up complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x16_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x16_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_8x16_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_8x16_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x8_left_line complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_left_line_new complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_left_line_new complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_left_line_new complete dim=0


if (cu_width == 16 && cu_height == 16)
{
	Chroma8_const_up_16x16(up,  avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu,  IP_buffer_ptr_16x16_up_line, IP_map_scu_ptr_16x16_map_scu_up_line);
	Chroma8_const_left_16x16(left, avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu, IP_buffer_ptr_16x16_left_line, IP_map_scu_ptr_16x16_map_scu_left_line_new);
	Chroma8_const_top_left_16x16(up, left,  avail_cu, IP_buffer_ptr_16x16_top_left);
}
#if !SIMPLE_MD
else if (cu_width == 8 && cu_height == 16)
{
	Chroma8_const_up_8x16(up, avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu, IP_buffer_ptr_8x16_up_line, IP_map_scu_ptr_8x16_map_scu_up_line);
	Chroma8_const_left_8x16(left,  avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu,  IP_buffer_ptr_8x16_left_line, IP_map_scu_ptr_8x16_map_scu_left_line_new );
	Chroma8_const_top_left_8x16(up, left,  avail_cu, IP_buffer_ptr_8x16_top_left);
}
else if (cu_width == 16 && cu_height == 8)
{
	Chroma8_const_up_16x8(up,  avail_cu, pic_y_scu, pic_x_scu, pic_y, x_scu, pic_width_in_scu, IP_buffer_ptr_16x8_up_line, IP_map_scu_ptr_16x8_map_scu_up_line);
	Chroma8_const_left_16x8(left,avail_cu, pic_y_scu, pic_x_scu, pic_x, y_scu, pic_height_in_scu, IP_buffer_ptr_16x8_left_line, IP_map_scu_ptr_16x8_map_scu_left_line_new);
	Chroma8_const_top_left_16x8(up, left,  avail_cu, IP_buffer_ptr_16x8_top_left);
}
#endif //!SIMPLE_MD

up[1] = left[3];
left[1] = up[3];
up[0] = left[4];
left[0] = up[4];
}

#endif // USE_TSCPM

static __inline void com_mset_8b(pel* dst, pel v, int cnt)
{
#pragma HLS INLINE off
	int i;
	for (i = 0; i < cnt; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=67 
		dst[i] = v;
	}
}

void get_IP_pixel_8x8_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel* left, pel* up, U12 x, U12 y,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16)
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
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < width; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=8
			up[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_8x8->up_line[i]) : (IP_buffer_ptr_V_8x8->up_line[i]);
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=2
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				//flag = pic_y == 0 ?
				//	1 :
				//	((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31 & (1)));
				flag = pic_y == 0 ?
					1 :
					((int)(IP_map_scu_ptr_16x16->map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else
				flag = 0;

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
					up[width + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
						(IP_buffer_ptr_U_8x8->up_line[width + (i << unit_size_bit) + j]) :
						(IP_buffer_ptr_V_8x8->up_line[width + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
					up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0)
		{
			for (i = 0; i < height; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_8x8->left_line[i]) : (IP_buffer_ptr_V_8x8->left_line[i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_16x16->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
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
						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
							(IP_buffer_ptr_U_8x8->left_line[height + (i << unit_size_bit) + j]) :
							(IP_buffer_ptr_V_8x8->left_line[height + (i << unit_size_bit) + j]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < height; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_8x8->left_line[i]) : (IP_buffer_ptr_V_8x8->left_line[i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_16x16->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
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
						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
							(IP_buffer_ptr_U_8x8->left_line[height + (i << unit_size_bit) + j]) :
							(IP_buffer_ptr_V_8x8->left_line[height + (i << unit_size_bit) + j]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up[2] = left[2] = ch_type == 1 ? (IP_buffer_ptr_U_8x8->top_left) : (IP_buffer_ptr_V_8x8->top_left);
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
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


	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}
void get_IP_pixel_4x8_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel* left, pel* up, U12 x, U12 y,
	IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_U_4x8,
	IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_V_4x8,
	IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16)
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
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < width; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=8
			up[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_4x8->up_line[i]) : (IP_buffer_ptr_V_4x8->up_line[i]);
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=2
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				//flag = pic_y == 0 ?
				//	1 :
				//	((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31 & (1)));
				flag = pic_y == 0 ?
					1 :
					((int)(IP_map_scu_ptr_8x16->map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else
				flag = 0;

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
					up[width + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
						(IP_buffer_ptr_U_4x8->up_line[width + (i << unit_size_bit) + j]) :
						(IP_buffer_ptr_V_4x8->up_line[width + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
					up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0)
		{
			for (i = 0; i < height; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_4x8->left_line[i]) : (IP_buffer_ptr_V_4x8->left_line[i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_8x16->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
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
						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
							(IP_buffer_ptr_U_4x8->left_line[height + (i << unit_size_bit) + j]) :
							(IP_buffer_ptr_V_4x8->left_line[height + (i << unit_size_bit) + j]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < height; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_4x8->left_line[i]) : (IP_buffer_ptr_V_4x8->left_line[i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_8x16->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
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
						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
							(IP_buffer_ptr_U_4x8->left_line[height + (i << unit_size_bit) + j]) :
							(IP_buffer_ptr_V_4x8->left_line[height + (i << unit_size_bit) + j]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up[2] = left[2] = ch_type == 1 ? (IP_buffer_ptr_U_4x8->top_left) : (IP_buffer_ptr_V_4x8->top_left);
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
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


	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}
void get_IP_pixel_8x4_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel* left, pel* up, U12 x, U12 y,
	IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_U_8x4,
	IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_V_8x4,
	IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8)
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
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < width; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=8
			up[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_8x4->up_line[i]) : (IP_buffer_ptr_V_8x4->up_line[i]);
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=2
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				//flag = pic_y == 0 ?
				//	1 :
				//	((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu + width_in_scu + i][pic_y_scu - 1] >> 31 & (1)));
				flag = pic_y == 0 ?
					1 :
					((int)(IP_map_scu_ptr_16x8->map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else
				flag = 0;

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
					up[width + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
						(IP_buffer_ptr_U_8x4->up_line[width + (i << unit_size_bit) + j]) :
						(IP_buffer_ptr_V_8x4->up_line[width + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS LOOP_TRIPCOUNT max=4
					up[width + (i << unit_size_bit) + j + 3] = up[width + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		if (pic_x == 0)
		{
			for (i = 0; i < height; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_8x4->left_line[i]) : (IP_buffer_ptr_V_8x4->left_line[i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[1 - me_line_ptr->curr_ctu_idx][15][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_16x8->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
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
						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
							(IP_buffer_ptr_U_8x4->left_line[height + (i << unit_size_bit) + j]) :
							(IP_buffer_ptr_V_8x4->left_line[height + (i << unit_size_bit) + j]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
		else {
			for (i = 0; i < height; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=8
				left[i + 3] = ch_type == 1 ? (IP_buffer_ptr_U_8x4->left_line[i]) : (IP_buffer_ptr_V_8x4->left_line[i]);
			}
			for (i = 0; i < pad_le_in_scu; i++)
			{
#pragma HLS LOOP_TRIPCOUNT max=2
				if (pic_y_scu + height_in_scu + i < 16)
				{
					//flag = ((int)(me_line_ptr->map_scu_inner[me_line_ptr->curr_ctu_idx][pic_x_scu - 1][pic_y_scu + height_in_scu + i] >> 31 & (1)));
					flag = ((int)(IP_map_scu_ptr_16x8->map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
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
						left[height + (i << unit_size_bit) + j + 3] = ch_type == 1 ?
							(IP_buffer_ptr_U_8x4->left_line[height + (i << unit_size_bit) + j]) :
							(IP_buffer_ptr_V_8x4->left_line[height + (i << unit_size_bit) + j]);
					}
				}
				else
				{
					for (j = 0; j < unit_size; j++)
					{
#pragma HLS LOOP_TRIPCOUNT max=4
						left[height + (i << unit_size_bit) + j + 3] = left[height + (i << unit_size_bit) + 2];
					}
				}
			}
		}
	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up[2] = left[2] = ch_type == 1 ? (IP_buffer_ptr_U_8x4->top_left) : (IP_buffer_ptr_V_8x4->top_left);
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up[2] = left[2] = up[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up[2] = left[2] = left[3];
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


	up[1] = left[3];
	left[1] = up[3];
	up[0] = left[4];
	left[0] = up[4];
}


void get_IP_pixel_chroma_8x8(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, pel left_u[19], pel up_u[19], pel left_v[19], pel up_v[19], U12 x, U12 y,

	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8], pel IP_buffer_ptr_U_8x8_left_line[2 * 8], pel IP_buffer_ptr_U_8x8_top_left,
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8], pel IP_buffer_ptr_V_8x8_left_line[2 * 8], pel IP_buffer_ptr_V_8x8_top_left,
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1]
)
{
	int i;
	int j;
	U6 width_in_scu = width >> (2 - 1);
	U6 height_in_scu = height >> (2 - 1);
	//4
	U3 unit_size = 1 << 2 >> 1;
	U2 unit_size_bit = 1;
	//x/4
	U11 x_scu = ((x << 1)) >> 2;
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
	for (i = 0; i < 19; i++)
	{
#pragma HLS UNROLL
		up_u[i] = (1 << (bit_depth - 1));
		left_u[i] = (1 << (bit_depth - 1));
		up_v[i] = (1 << (bit_depth - 1));
		left_v[i] = (1 << (bit_depth - 1));
	}
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < width; i++)
		{
#pragma HLS UNROLL
			up_u[i + 3] = (IP_buffer_ptr_U_8x8_up_line[i]);
			up_v[i + 3] = (IP_buffer_ptr_V_8x8_up_line[i]);
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS UNROLL
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				flag = pic_y == 0 ? 1 : ((int)(IP_map_scu_ptr_16x16_map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else {
				flag = 0;
			}

			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					up_u[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_U_8x8_up_line[width + (i << unit_size_bit) + j]);
					up_v[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_V_8x8_up_line[width + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					up_u[width + (i << unit_size_bit) + j + 3] = up_u[width + (i << unit_size_bit) + 2];
					up_v[width + (i << unit_size_bit) + j + 3] = up_v[width + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {

		for (i = 0; i < height; i++)
		{
#pragma HLS UNROLL
			left_u[i + 3] = (IP_buffer_ptr_U_8x8_left_line[i]);
			left_v[i + 3] = (IP_buffer_ptr_V_8x8_left_line[i]);
		}
		for (i = 0; i < pad_le_in_scu; i++)
		{
#pragma HLS UNROLL
			if (pic_y_scu + height_in_scu + i < 16)
			{
				flag = ((int)(IP_map_scu_ptr_16x16_map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
			}
			else
			{
				flag = 0;
			}
			if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					left_u[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_U_8x8_left_line[height + (i << unit_size_bit) + j]);
					left_v[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_V_8x8_left_line[height + (i << unit_size_bit) + j]);

				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					left_u[height + (i << unit_size_bit) + j + 3] = left_u[height + (i << unit_size_bit) + 2];
					left_v[height + (i << unit_size_bit) + j + 3] = left_v[height + (i << unit_size_bit) + 2];

				}
			}

		}


	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up_u[2] = left_u[2] = (IP_buffer_ptr_U_8x8_top_left);
		up_v[2] = left_v[2] = (IP_buffer_ptr_V_8x8_top_left);
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up_u[2] = left_u[2] = up_u[3];
		up_v[2] = left_v[2] = up_v[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up_u[2] = left_u[2] = left_u[3];
		up_v[2] = left_v[2] = left_v[3];
	}

	up_u[1] = left_u[3];
	left_u[1] = up_u[3];
	up_u[0] = left_u[4];
	left_u[0] = up_u[4];

	up_v[1] = left_v[3];
	left_v[1] = up_v[3];
	up_v[0] = left_v[4];
	//				assert(MCU_GET_CODED_FLAG(map_scu[scup - pic_width_in_scu + width_in_scu + i]) == 1);
	left_v[0] = up_v[4];

}

void get_IP_pixel_chroma_4x8(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, pel left_u[19], pel up_u[19], pel left_v[19], pel up_v[19], U12 x, U12 y,
	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4], pel IP_buffer_ptr_U_4x8_left_line[2 * 8], pel IP_buffer_ptr_U_4x8_top_left,
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4], pel IP_buffer_ptr_V_4x8_left_line[2 * 8], pel IP_buffer_ptr_V_4x8_top_left,
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1]
)
{
	int i;
	int j;
	U6 width_in_scu = width >> (2 - 1);
	U6 height_in_scu = height >> (2 - 1);
	U3 unit_size = 1 << 2 >> 1;
	U2 unit_size_bit = 1;
	U11 x_scu = ((x << 1)) >> 2;
	U11 y_scu = ((y << 1)) >> 2;
	U7 pad_le = height;
	U7 pad_up = width;
	U6 pad_le_in_scu = height_in_scu;
	U6 pad_up_in_scu = width_in_scu;
	U6 pic_x = x % 32;
	U6 pic_y = y % 32;
	for (i = 0; i < 19; i++)
	{
#pragma HLS UNROLL
		up_u[i] = (1 << (bit_depth - 1));
		left_u[i] = (1 << (bit_depth - 1));
		up_v[i] = (1 << (bit_depth - 1));
		left_v[i] = (1 << (bit_depth - 1));
	}
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < width; i++)
		{
#pragma HLS UNROLL
			up_u[i + 3] = (IP_buffer_ptr_U_4x8_up_line[i]);
			up_v[i + 3] = (IP_buffer_ptr_V_4x8_up_line[i]);
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS UNROLL
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				flag = pic_y == 0 ? 1 : ((int)(IP_map_scu_ptr_8x16_map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else {
				flag = 0;
			}
			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					up_u[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_U_4x8_up_line[width + (i << unit_size_bit) + j]);
					up_v[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_V_4x8_up_line[width + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					up_u[width + (i << unit_size_bit) + j + 3] = up_u[width + (i << unit_size_bit) + 2];
					up_v[width + (i << unit_size_bit) + j + 3] = up_v[width + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < height; i++)
		{
#pragma HLS UNROLL
			left_u[i + 3] = (IP_buffer_ptr_U_4x8_left_line[i]);
			left_v[i + 3] = (IP_buffer_ptr_V_4x8_left_line[i]);
		}
		for (i = 0; i < pad_le_in_scu; i++)
		{
#pragma HLS UNROLL
			if (pic_y_scu + height_in_scu + i < 16)
			{
				flag = ((int)(IP_map_scu_ptr_8x16_map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
			}
			else
			{
				flag = 0;
			}
			if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					left_u[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_U_4x8_left_line[height + (i << unit_size_bit) + j]);
					left_v[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_V_4x8_left_line[height + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					left_u[height + (i << unit_size_bit) + j + 3] = left_u[height + (i << unit_size_bit) + 2];
					left_v[height + (i << unit_size_bit) + j + 3] = left_v[height + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up_u[2] = left_u[2] = (IP_buffer_ptr_U_4x8_top_left);
		up_v[2] = left_v[2] = (IP_buffer_ptr_V_4x8_top_left);
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up_u[2] = left_u[2] = up_u[3];
		up_v[2] = left_v[2] = up_v[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up_u[2] = left_u[2] = left_u[3];
		up_v[2] = left_v[2] = left_v[3];
	}
	up_u[1] = left_u[3];
	left_u[1] = up_u[3];
	up_u[0] = left_u[4];
	left_u[0] = up_u[4];
	up_v[1] = left_v[3];
	left_v[1] = up_v[3];
	up_v[0] = left_v[4];
	left_v[0] = up_v[4];
}
void get_IP_pixel_chroma_8x4(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, pel left_u[19], pel up_u[19], pel left_v[19], pel up_v[19], U12 x, U12 y,
\
	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8], pel IP_buffer_ptr_U_8x4_left_line[2 * 4], pel IP_buffer_ptr_U_8x4_top_left,
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8], pel IP_buffer_ptr_V_8x4_left_line[2 * 4], pel IP_buffer_ptr_V_8x4_top_left,
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1]
	)
{
	int i;
	int j;
	U6 width_in_scu = width >> (2 - 1);
	U6 height_in_scu = height >> (2 - 1);
	U3 unit_size = 1 << 2 >> 1;
	U2 unit_size_bit = 1;
	U11 x_scu = ((x << 1)) >> 2;
	U11 y_scu = ((y << 1)) >> 2;
	U7 pad_le = height;
	U7 pad_up = width;
	U6 pad_le_in_scu = height_in_scu;
	U6 pad_up_in_scu = width_in_scu;
	U6 pic_x = x % 32;
	U6 pic_y = y % 32;
	for (i = 0; i < 19; i++)
	{
#pragma HLS UNROLL
		up_u[i] = (1 << (bit_depth - 1));
		left_u[i] = (1 << (bit_depth - 1));
		up_v[i] = (1 << (bit_depth - 1));
		left_v[i] = (1 << (bit_depth - 1));
	}
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	U1 flag = 0;
	if (((avail_cu) & 1 << 0) == 1 << 0) {
		for (i = 0; i < width; i++)
		{
#pragma HLS UNROLL
			up_u[i + 3] = (IP_buffer_ptr_U_8x4_up_line[i]);
			up_v[i + 3] = (IP_buffer_ptr_V_8x4_up_line[i]);
		}
		for (i = 0; i < pad_up_in_scu; i++)
		{
#pragma HLS UNROLL
			if (pic_y == 0 ? x_scu + width_in_scu + i < pic_width_in_scu : pic_x_scu + width_in_scu + i < 16)
			{
				flag = pic_y == 0 ? 1 : ((int)(IP_map_scu_ptr_16x8_map_scu_up_line[width_in_scu + i] >> 31 & (1)));
			}
			else {
				flag = 0;
			}
			if (x_scu + width_in_scu + i < pic_width_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					up_u[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_U_8x4_up_line[width + (i << unit_size_bit) + j]);
					up_v[width + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_V_8x4_up_line[width + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					up_u[width + (i << unit_size_bit) + j + 3] = up_u[width + (i << unit_size_bit) + 2];
					up_v[width + (i << unit_size_bit) + j + 3] = up_v[width + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < height; i++)
		{
#pragma HLS UNROLL
			left_u[i + 3] = (IP_buffer_ptr_U_8x4_left_line[i]);
			left_v[i + 3] = (IP_buffer_ptr_V_8x4_left_line[i]);
		}
		for (i = 0; i < pad_le_in_scu; i++)
		{
#pragma HLS UNROLL
			if (pic_y_scu + height_in_scu + i < 16)
			{
				flag = ((int)(IP_map_scu_ptr_16x8_map_scu_left_line_new[height_in_scu + i] >> 31 & (1)));
			}
			else
			{
				flag = 0;
			}
			if (y_scu + height_in_scu + i < pic_height_in_scu && flag)
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					left_u[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_U_8x4_left_line[height + (i << unit_size_bit) + j]);
					left_v[height + (i << unit_size_bit) + j + 3] = (IP_buffer_ptr_V_8x4_left_line[height + (i << unit_size_bit) + j]);
				}
			}
			else
			{
				for (j = 0; j < unit_size; j++)
				{
#pragma HLS UNROLL
					left_u[height + (i << unit_size_bit) + j + 3] = left_u[height + (i << unit_size_bit) + 2];
					left_v[height + (i << unit_size_bit) + j + 3] = left_v[height + (i << unit_size_bit) + 2];
				}
			}
		}
	}
	if (((avail_cu) & 1 << 2) == 1 << 2) {
		up_u[2] = left_u[2] = (IP_buffer_ptr_U_8x4_top_left);
		up_v[2] = left_v[2] = (IP_buffer_ptr_V_8x4_top_left);
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		up_u[2] = left_u[2] = up_u[3];
		up_v[2] = left_v[2] = up_v[3];
	}
	else if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		up_u[2] = left_u[2] = left_u[3];
		up_v[2] = left_v[2] = left_v[3];
	}
	up_u[1] = left_u[3];
	left_u[1] = up_u[3];
	up_u[0] = left_u[4];
	left_u[0] = up_u[4];
	up_v[1] = left_v[3];
	left_v[1] = up_v[3];
	up_v[0] = left_v[4];
	left_v[0] = up_v[4];
}
void get_IP_pixel_chroma_package_8(MD_FW* md_fw_ptr, U3 cu_width_log2, U3 cu_height_log2, U13 pb_x, U13 pb_y,
	pel nb_u[2][2 * 8 + 3], pel nb_v[2][2 * 8 + 3],
	u16 avail_cu,

	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8], pel IP_buffer_ptr_U_8x8_left_line[2 * 8], pel IP_buffer_ptr_U_8x8_top_left,
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8], pel IP_buffer_ptr_V_8x8_left_line[2 * 8], pel IP_buffer_ptr_V_8x8_top_left,

	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4], pel IP_buffer_ptr_U_4x8_left_line[2 * 8], pel IP_buffer_ptr_U_4x8_top_left,
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4], pel IP_buffer_ptr_V_4x8_left_line[2 * 8], pel IP_buffer_ptr_V_4x8_top_left,

	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8], pel IP_buffer_ptr_U_8x4_left_line[2 * 4], pel IP_buffer_ptr_U_8x4_top_left,
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8], pel IP_buffer_ptr_V_8x4_left_line[2 * 4], pel IP_buffer_ptr_V_8x4_top_left,

	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],

	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],

	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1]
)
{
#pragma HLS INLINE 
#pragma HLS ARRAY_PARTITION variable=nb_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=nb_v complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_left_line_new complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_left_line_new complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_left_line_new complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x8_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x8_left_line complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_4x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_4x8_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_4x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_4x8_left_line complete dim=0

#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x4_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x4_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x4_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x4_left_line complete dim=0

	if (cu_width_log2 == 3 && cu_height_log2 == 3)
	{
		get_IP_pixel_chroma_8x8(1 << cu_width_log2, 1 << cu_height_log2,
			avail_cu,
			md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu,
			md_fw_ptr->bit_depth_internal,
			nb_u[0], nb_u[1], nb_v[0], nb_v[1], pb_x >> 1, pb_y >> 1,
	
			IP_buffer_ptr_U_8x8_up_line, IP_buffer_ptr_U_8x8_left_line,  IP_buffer_ptr_U_8x8_top_left,
			IP_buffer_ptr_V_8x8_up_line, IP_buffer_ptr_V_8x8_left_line,  IP_buffer_ptr_V_8x8_top_left,
			IP_map_scu_ptr_16x16_map_scu_up_line,
			IP_map_scu_ptr_16x16_map_scu_left_line_new
		);
	}
#if !SIMPLE_MD
	else if (cu_width_log2 == 2 && cu_height_log2 == 3)
	{
		get_IP_pixel_chroma_4x8(1 << cu_width_log2, 1 << cu_height_log2,
			avail_cu,
			md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu,
			md_fw_ptr->bit_depth_internal,
			nb_u[0], nb_u[1], nb_v[0], nb_v[1], pb_x >> 1, pb_y >> 1,

			IP_buffer_ptr_U_4x8_up_line,IP_buffer_ptr_U_4x8_left_line,IP_buffer_ptr_U_4x8_top_left,
			IP_buffer_ptr_V_4x8_up_line,IP_buffer_ptr_V_4x8_left_line,IP_buffer_ptr_V_4x8_top_left,
			IP_map_scu_ptr_8x16_map_scu_up_line,
			IP_map_scu_ptr_8x16_map_scu_left_line_new
		);
	}
	else if (cu_width_log2 == 3 && cu_height_log2 == 2)
	{
		get_IP_pixel_chroma_8x4(1 << cu_width_log2, 1 << cu_height_log2,
			avail_cu,
			md_fw_ptr->pic_width_in_scu, md_fw_ptr->pic_height_in_scu,
			md_fw_ptr->bit_depth_internal,
			nb_u[0], nb_u[1], nb_v[0], nb_v[1], pb_x >> 1, pb_y >> 1,
			IP_buffer_ptr_U_8x4_up_line,IP_buffer_ptr_U_8x4_left_line,IP_buffer_ptr_U_8x4_top_left,
			IP_buffer_ptr_V_8x4_up_line,IP_buffer_ptr_V_8x4_left_line,IP_buffer_ptr_V_8x4_top_left,
			IP_map_scu_ptr_16x8_map_scu_up_line,
			IP_map_scu_ptr_16x8_map_scu_left_line_new
		);
	}
#endif
}

u16 com_get_avail_intra_ip_chroma_8x8(U11 x_scu, U11 y_scu, 
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new
	)
{

#pragma HLS INLINE off

#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_left_line_new complete dim=0
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x16_map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x16_map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_16x16_map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_16x16_map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_16x16_map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x16_map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x16_map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_chroma_4x8(U11 x_scu, U11 y_scu, 
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new 
	)
{

#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_left_line_new complete dim=0
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x16_map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x16_map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_8x16_map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_8x16_map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_8x16_map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x16_map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_8x16_map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
u16 com_get_avail_intra_ip_chroma_8x4(U11 x_scu, U11 y_scu, 
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new
	)
{
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_left_line_new complete dim=0
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x8_map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x8_map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr_16x8_map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr_16x8_map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr_16x8_map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x8_map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr_16x8_map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}
void ipred_c_8_prepare(MD_FW* md_fw_ptr, U3 cu_width_log2, U3 cu_height_log2, U13 pb_x, U13 pb_y, MD_COM_MODE_BEST* mod_info_curr_ptr,
	pel nb_y[2][2 * 16 + 3],
	pel nb[2][2 * 8 + 3], pel nb1[2][2 * 8 + 3],

	S8 ipm_01,

	u16* avail_cu_output,

	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8], pel IP_buffer_ptr_U_8x8_left_line[2 * 8], pel IP_buffer_ptr_U_8x8_top_left,
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8], pel IP_buffer_ptr_V_8x8_left_line[2 * 8], pel IP_buffer_ptr_V_8x8_top_left,
	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4], pel IP_buffer_ptr_U_4x8_left_line[2 * 8], pel IP_buffer_ptr_U_4x8_top_left,
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4], pel IP_buffer_ptr_V_4x8_left_line[2 * 8], pel IP_buffer_ptr_V_4x8_top_left,
	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8], pel IP_buffer_ptr_U_8x4_left_line[2 * 4], pel IP_buffer_ptr_U_8x4_top_left,
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8], pel IP_buffer_ptr_V_8x4_left_line[2 * 4], pel IP_buffer_ptr_V_8x4_top_left,
	pel	IP_buffer_ptr_8x16_up_line[2 * 8], pel IP_buffer_ptr_8x16_left_line[2 * 16], pel IP_buffer_ptr_8x16_top_left,
	pel	IP_buffer_ptr_16x8_up_line[2 * 16], pel IP_buffer_ptr_16x8_left_line[2 * 8], pel IP_buffer_ptr_16x8_top_left,
	pel	IP_buffer_ptr_16x16_up_line[2 * 16], pel IP_buffer_ptr_16x16_left_line[2 * 16], pel IP_buffer_ptr_16x16_top_left,
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new
)
{
#pragma HLS INLINE OFF
#pragma HLS ARRAY_PARTITION variable=nb_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=nb complete dim=0
#pragma HLS ARRAY_PARTITION variable=nb1 complete dim=0

	u16 avail_cu=0;

	if (cu_width_log2 == 3 && cu_height_log2 == 3)
	{
		avail_cu = com_get_avail_intra_ip_chroma_8x8(mod_info_curr_ptr->x_scu, mod_info_curr_ptr->y_scu, IP_map_scu_ptr_16x16_map_scu_up_line,
			IP_map_scu_ptr_16x16_map_scu_left_line_new,
			IP_map_scu_ptr_16x16_map_scu_top_left_new );

	}
#if !SIMPLE_MD
	else if (cu_width_log2 == 2 && cu_height_log2 == 3)
	{
		avail_cu = com_get_avail_intra_ip_chroma_4x8(mod_info_curr_ptr->x_scu, mod_info_curr_ptr->y_scu, IP_map_scu_ptr_8x16_map_scu_up_line,
			IP_map_scu_ptr_8x16_map_scu_left_line_new,
			IP_map_scu_ptr_8x16_map_scu_top_left_new );

	}
	else if (cu_width_log2 == 3 && cu_height_log2 == 2)
	{
		avail_cu = com_get_avail_intra_ip_chroma_8x4(mod_info_curr_ptr->x_scu, mod_info_curr_ptr->y_scu, IP_map_scu_ptr_16x8_map_scu_up_line,
			IP_map_scu_ptr_16x8_map_scu_left_line_new,
			IP_map_scu_ptr_16x8_map_scu_top_left_new);

	}
#endif
	get_IP_pixel_chroma_package_8(md_fw_ptr, cu_width_log2, cu_height_log2, pb_x, pb_y, nb, nb1,

		avail_cu,

		IP_buffer_ptr_U_8x8_up_line, IP_buffer_ptr_U_8x8_left_line, IP_buffer_ptr_U_8x8_top_left,
		IP_buffer_ptr_V_8x8_up_line, IP_buffer_ptr_V_8x8_left_line, IP_buffer_ptr_V_8x8_top_left,

		IP_buffer_ptr_U_4x8_up_line, IP_buffer_ptr_U_4x8_left_line, IP_buffer_ptr_U_4x8_top_left,
		IP_buffer_ptr_V_4x8_up_line, IP_buffer_ptr_V_4x8_left_line, IP_buffer_ptr_V_4x8_top_left,
		IP_buffer_ptr_U_8x4_up_line, IP_buffer_ptr_U_8x4_left_line, IP_buffer_ptr_U_8x4_top_left,
		IP_buffer_ptr_V_8x4_up_line, IP_buffer_ptr_V_8x4_left_line, IP_buffer_ptr_V_8x4_top_left,
		 IP_map_scu_ptr_8x16_map_scu_up_line,
		 IP_map_scu_ptr_8x16_map_scu_left_line_new,
		 IP_map_scu_ptr_16x8_map_scu_up_line,
		 IP_map_scu_ptr_16x8_map_scu_left_line_new,

		 IP_map_scu_ptr_16x16_map_scu_up_line,
		 IP_map_scu_ptr_16x16_map_scu_left_line_new
	);

//#if USE_TSCPM
	get_IP_pixel_16_for_chroma(1 << (cu_width_log2 + 1), 1 << (cu_height_log2 + 1), avail_cu, md_fw_ptr->pic_width_in_scu,
		md_fw_ptr->pic_height_in_scu, md_fw_ptr->bit_depth_internal, 0, nb_y[0], nb_y[1], pb_x, pb_y,
		IP_buffer_ptr_8x16_up_line, IP_buffer_ptr_8x16_left_line, IP_buffer_ptr_8x16_top_left,
		IP_buffer_ptr_16x8_up_line, IP_buffer_ptr_16x8_left_line, IP_buffer_ptr_16x8_top_left,
		IP_buffer_ptr_16x16_up_line, IP_buffer_ptr_16x16_left_line, IP_buffer_ptr_16x16_top_left,
		IP_map_scu_ptr_8x16_map_scu_up_line,
		IP_map_scu_ptr_8x16_map_scu_left_line_new,
		IP_map_scu_ptr_16x8_map_scu_up_line,
		IP_map_scu_ptr_16x8_map_scu_left_line_new,
		IP_map_scu_ptr_16x16_map_scu_up_line,
		IP_map_scu_ptr_16x16_map_scu_left_line_new
	);
//#endif // USE_TSCPM

	* avail_cu_output = avail_cu;
}


void get_ref_u_wsc8(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx, U2* U_ref0)
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
void com_mc_c_00_uv8(int gmv_x, int gmv_y, s16 pred_u[8][8], s16 pred_v[8][8], int w, int h, int bit_depth, pel UP_tmp_mc[11][11], pel VP_tmp_mc[11][11])
{
#pragma HLS INLINE OFF
	int i;
	int j;
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < w; j++) {
			pred_u[i][j] = UP_tmp_mc[i + 1][j + 1];// ref_u[j + offset_ref];
			pred_v[i][j] = VP_tmp_mc[i + 1][j + 1];// ref_v[j + offset_ref];
		}
	}
}
void com_mc_c_0n_uv8(int gmv_x, int gmv_y, s16 pred_u[8][8], s16 pred_v[8][8], int w, int h, int bit_depth, pel UP_tmp_mc[11][11], pel VP_tmp_mc[11][11], s16 tbl_mc_c_coeff_all_dy[4])
{
	int i;
	int j;
	s32 pt;
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < w; j++) {
			pt = (
				((int)tbl_mc_c_coeff_all_dy[0]) * ((int)UP_tmp_mc[i][j + 1]) +
				((int)tbl_mc_c_coeff_all_dy[1]) * ((int)UP_tmp_mc[i + 1][j + 1]) +
				((int)tbl_mc_c_coeff_all_dy[2]) * ((int)UP_tmp_mc[i + 2][j + 1]) +
				((int)tbl_mc_c_coeff_all_dy[3]) * ((int)UP_tmp_mc[i + 3][j + 1]) + (1 << 5)) >> 6;
			pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
				1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			pt = (
				((int)tbl_mc_c_coeff_all_dy[0]) * ((int)VP_tmp_mc[i][j + 1]) +
				((int)tbl_mc_c_coeff_all_dy[1]) * ((int)VP_tmp_mc[i + 1][j + 1]) +
				((int)tbl_mc_c_coeff_all_dy[2]) * ((int)VP_tmp_mc[i + 2][j + 1]) +
				((int)tbl_mc_c_coeff_all_dy[3]) * ((int)VP_tmp_mc[i + 3][j + 1]) + (1 << 5)) >> 6;
			pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
				1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
		}
	}
}
void com_mc_c_n0_uv8(int gmv_x, int gmv_y, s16 pred_u[8][8], s16 pred_v[8][8], int w, int h, int bit_depth, pel UP_tmp_mc[11][11], pel VP_tmp_mc[11][11], s16 tbl_mc_c_coeff_all_dx[4])
{
	int i;
	int j;
	s32 pt;
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < w; j++) {
			pt = (
				((int)tbl_mc_c_coeff_all_dx[0]) * ((int)UP_tmp_mc[i + 1][j]) +
				((int)tbl_mc_c_coeff_all_dx[1]) * ((int)UP_tmp_mc[i + 1][j + 1]) +
				((int)tbl_mc_c_coeff_all_dx[2]) * ((int)UP_tmp_mc[i + 1][j + 2]) +
				((int)tbl_mc_c_coeff_all_dx[3]) * ((int)UP_tmp_mc[i + 1][j + 3]) + (1 << 5)) >> 6;
			pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
				1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			pt = (
				((int)tbl_mc_c_coeff_all_dx[0]) * ((int)VP_tmp_mc[i + 1][j]) +
				((int)tbl_mc_c_coeff_all_dx[1]) * ((int)VP_tmp_mc[i + 1][j + 1]) +
				((int)tbl_mc_c_coeff_all_dx[2]) * ((int)VP_tmp_mc[i + 1][j + 2]) +
				((int)tbl_mc_c_coeff_all_dx[3]) * ((int)VP_tmp_mc[i + 1][j + 3]) + (1 << 5)) >> 6;
			pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
				1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
		}
	}
}
void com_mc_c_nn_uv8(int gmv_x, int gmv_y, s16 pred_u[8][8], s16 pred_v[8][8], int w, int h, int bit_depth,
	pel UP_tmp_mc[11][11], pel VP_tmp_mc[11][11], s16 tbl_mc_c_coeff_all_dx[4], s16 tbl_mc_c_coeff_all_dy[4])
{
	static  s16 buf_u[11][8];
	static  s16 buf_v[11][8];
	static  s16 buf_u1[11][8];
	static  s16 buf_v1[11][8];
#pragma HLS ARRAY_PARTITION variable=buf_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=buf_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=buf_u1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=buf_v1 complete dim=2
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = 1 << shift1 >> 1;
	const int add2 = 1 << (shift2 - 1);
	int i;
	int j;
	s32 pt_u[8];
	s32 pt_v[8];
#pragma HLS ARRAY_PARTITION variable=pt_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=pt_v complete dim=0
	s16 buf_u_tmp;
	s16 buf_v_tmp;
	for (i = 0; i < h + 3; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=11 max=11 avg=11
		for (j = 0; j < w; j++) {
			buf_u_tmp = ((s16)((
				((int)tbl_mc_c_coeff_all_dx[0]) * ((int)UP_tmp_mc[i][j]) +
				((int)tbl_mc_c_coeff_all_dx[1]) * ((int)UP_tmp_mc[i][j + 1]) +
				((int)tbl_mc_c_coeff_all_dx[2]) * ((int)UP_tmp_mc[i][j + 2]) +
				((int)tbl_mc_c_coeff_all_dx[3]) * ((int)UP_tmp_mc[i][j + 3]) + add1) >> shift1));
			buf_u[i][j] = buf_u_tmp;
			buf_u1[i][j] = buf_u_tmp;
			buf_v_tmp = ((s16)((
				((int)tbl_mc_c_coeff_all_dx[0]) * ((int)VP_tmp_mc[i][j]) +
				((int)tbl_mc_c_coeff_all_dx[1]) * ((int)VP_tmp_mc[i][j + 1]) +
				((int)tbl_mc_c_coeff_all_dx[2]) * ((int)VP_tmp_mc[i][j + 2]) +
				((int)tbl_mc_c_coeff_all_dx[3]) * ((int)VP_tmp_mc[i][j + 3]) + add1) >> shift1));
			buf_v[i][j] = buf_v_tmp;
			buf_v1[i][j] = buf_v_tmp;
		}
	}
	for (i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
		for (j = 0; j < w; j++) {
			pt_u[j] = (
				((int)tbl_mc_c_coeff_all_dy[0]) * ((int)buf_u[i][j]) +
				((int)tbl_mc_c_coeff_all_dy[1]) * ((int)buf_u[i + 1][j]) +
				((int)tbl_mc_c_coeff_all_dy[2]) * ((int)buf_u1[i + 2][j]) +
				((int)tbl_mc_c_coeff_all_dy[3]) * ((int)buf_u1[i + 3][j]) + add2) >> shift2;
			pt_v[j] = (
				((int)tbl_mc_c_coeff_all_dy[0]) * ((int)buf_v[i][j]) +
				((int)tbl_mc_c_coeff_all_dy[1]) * ((int)buf_v[i + 1][j]) +
				((int)tbl_mc_c_coeff_all_dy[2]) * ((int)buf_v1[i + 2][j]) +
				((int)tbl_mc_c_coeff_all_dy[3]) * ((int)buf_v1[i + 3][j]) + add2) >> shift2;
		}
		for (j = 0; j < w; j++) {
			pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt_u[j] ? (1 << bit_depth) - 1 : pt_u[j])) ? 0 : (((
				1 << bit_depth) - 1 < pt_u[j] ? (1 << bit_depth) - 1 : pt_u[j])))));
			pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt_v[j] ? (1 << bit_depth) - 1 : pt_v[j])) ? 0 : (((
				1 << bit_depth) - 1 < pt_v[j] ? (1 << bit_depth) - 1 : pt_v[j])))));
		}
	}
}
void com_tbl_mc_c_1_uv8_wsc(int ori_mv_x, int ori_mv_y, int gmv_x, int gmv_y, s16 pred_u[8][8], s16 pred_v[8][8], int w, int h, int bit_depth,
	strFetch_ref_window* Fetch_Ref_window_ptr, U2 UV_flag
)
{
#pragma HLS INLINE OFF
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
	pel UP_tmp_mc[11][11];
	pel VP_tmp_mc[11][11];
#pragma HLS ARRAY_PARTITION variable=UP_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=VP_tmp complete dim=0
#pragma HLS ARRAY_PARTITION variable=UP_tmp_mc complete dim=2
#pragma HLS ARRAY_PARTITION variable=VP_tmp_mc complete dim=2
	for (int i = 0; i < h + 3; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=11 max=11 avg=11
		for (int j = 0; j < 128; j++) {
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
		for (int j = 0; j < 8 + 3; j++) {
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
#pragma HLS RESOURCE variable=tbl_mc_c_coeff_all_0 core=ROM_1P
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
#if 0
	if (dx == 0) {
		if (dy == 0) {
			com_mc_c_00_uv8(gmv_x, gmv_y, pred_u, pred_v, 8/*w*/, h, bit_depth, UP_tmp_mc, VP_tmp_mc);
		}
		else {
			com_mc_c_0n_uv8(gmv_x, gmv_y, pred_u, pred_v, 8/*w*/, h, bit_depth, UP_tmp_mc, VP_tmp_mc, tbl_mc_c_coeff_all_dy);
		}
	}
	else {
		if (dy == 0) {
			com_mc_c_n0_uv8(gmv_x, gmv_y, pred_u, pred_v, 8/*w*/, h, bit_depth, UP_tmp_mc, VP_tmp_mc, tbl_mc_c_coeff_all_dx);
		}
		else {
			com_mc_c_nn_uv8(gmv_x, gmv_y, pred_u, pred_v, 8/*w*/, h, bit_depth, UP_tmp_mc, VP_tmp_mc, tbl_mc_c_coeff_all_dx, tbl_mc_c_coeff_all_dy);
		}
	}
#else
	if (dx == 0 && dy == 0)
	{
		com_mc_c_00_uv8(gmv_x, gmv_y, pred_u, pred_v, 8/*w*/, h, bit_depth, UP_tmp_mc, VP_tmp_mc);
	}
	else if (dx != 0 && dy != 0)
	{
		com_mc_c_nn_uv8(gmv_x, gmv_y, pred_u, pred_v, 8/*w*/, h, bit_depth, UP_tmp_mc, VP_tmp_mc, tbl_mc_c_coeff_all_dx, tbl_mc_c_coeff_all_dy);

	}
	else
	{

		s32 pt;
		pel UP_tmp_mc_tmp[4];
		pel VP_tmp_mc_tmp[4];
#pragma HLS ARRAY_PARTITION variable=UP_tmp_mc_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=VP_tmp_mc_tmp complete dim=1
		s16 tbl_mc_c_coeff_all[4];
#pragma HLS ARRAY_PARTITION variable=tbl_mc_c_coeff_all complete dim=0

		for (int i = 0; i < h; i++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
			for (int j = 0; j < 8; j++) {
				if (dy == 0)
				{
					UP_tmp_mc_tmp[0] = UP_tmp_mc[i + 1][j];
					UP_tmp_mc_tmp[1] = UP_tmp_mc[i + 1][j + 1];
					UP_tmp_mc_tmp[2] = UP_tmp_mc[i + 1][j + 2];
					UP_tmp_mc_tmp[3] = UP_tmp_mc[i + 1][j + 3];
					VP_tmp_mc_tmp[0] = VP_tmp_mc[i + 1][j];
					VP_tmp_mc_tmp[1] = VP_tmp_mc[i + 1][j + 1];
					VP_tmp_mc_tmp[2] = VP_tmp_mc[i + 1][j + 2];
					VP_tmp_mc_tmp[3] = VP_tmp_mc[i + 1][j + 3];
					tbl_mc_c_coeff_all[0] = tbl_mc_c_coeff_all_dx[0];
					tbl_mc_c_coeff_all[1] = tbl_mc_c_coeff_all_dx[1];
					tbl_mc_c_coeff_all[2] = tbl_mc_c_coeff_all_dx[2];
					tbl_mc_c_coeff_all[3] = tbl_mc_c_coeff_all_dx[3];
				}
				else
				{
					UP_tmp_mc_tmp[0] = UP_tmp_mc[i ][j + 1];
					UP_tmp_mc_tmp[1] = UP_tmp_mc[i + 1][j + 1];
					UP_tmp_mc_tmp[2] = UP_tmp_mc[i + 2][j + 1];
					UP_tmp_mc_tmp[3] = UP_tmp_mc[i + 3][j + 1];
					VP_tmp_mc_tmp[0] = VP_tmp_mc[i ][j + 1];
					VP_tmp_mc_tmp[1] = VP_tmp_mc[i + 1][j + 1];
					VP_tmp_mc_tmp[2] = VP_tmp_mc[i + 2][j + 1];
					VP_tmp_mc_tmp[3] = VP_tmp_mc[i + 3][j + 1];
					tbl_mc_c_coeff_all[0] = tbl_mc_c_coeff_all_dy[0];
					tbl_mc_c_coeff_all[1] = tbl_mc_c_coeff_all_dy[1];
					tbl_mc_c_coeff_all[2] = tbl_mc_c_coeff_all_dy[2];
					tbl_mc_c_coeff_all[3] = tbl_mc_c_coeff_all_dy[3];

				}
				pt = (
					((int)tbl_mc_c_coeff_all[0]) * ((int)UP_tmp_mc_tmp[0]) +
					((int)tbl_mc_c_coeff_all[1]) * ((int)UP_tmp_mc_tmp[1]) +
					((int)tbl_mc_c_coeff_all[2]) * ((int)UP_tmp_mc_tmp[2]) +
					((int)tbl_mc_c_coeff_all[3]) * ((int)UP_tmp_mc_tmp[3]) + (1 << 5)) >> 6;
				pred_u[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
				pt = (
					((int)tbl_mc_c_coeff_all[0]) * ((int)VP_tmp_mc_tmp[0]) +
					((int)tbl_mc_c_coeff_all[1]) * ((int)VP_tmp_mc_tmp[1]) +
					((int)tbl_mc_c_coeff_all[2]) * ((int)VP_tmp_mc_tmp[2]) +
					((int)tbl_mc_c_coeff_all[3]) * ((int)VP_tmp_mc_tmp[3]) + (1 << 5)) >> 6;
				pred_v[i][j] = ((s16)((0 > (((1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)) ? 0 : (((
					1 << bit_depth) - 1 < pt ? (1 << bit_depth) - 1 : pt)))));
			}
		}
	}
#endif
}
 BOOL com_mc_cu_uv_8(MD_COM_MODE_BEST* mod_info_curr, int pic_w, int pic_h, S3 refi[REFP_NUM], s16 mv[REFP_NUM][2], int stride_chroma,
	 pel pred_u[8][8], pel pred_v[8][8], pel pred1_u[8][8], pel pred1_v[8][8], int bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp, int skip_idx, int cu_mode
#endif
)
{
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->refp_pic_ptr complete dim=0
#pragma HLS ARRAY_PARTITION variable=Fetch_Ref_window_ptr->refp_is_library_picture complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred1_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred1_v complete dim=2
	int scup = mod_info_curr->scup;
	int x = mod_info_curr->x_pos;
	int y = mod_info_curr->y_pos;
	int w = mod_info_curr->cu_width;
	int h = mod_info_curr->cu_height;
	int	w1 = w >> 1;
	int	h1 = h >> 1;
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
	static pel pred_u0[8][8];
	static pel pred_v0[8][8];
	static pel pred_u1[8][8];
	static pel pred_v1[8][8];
#pragma HLS ARRAY_PARTITION variable=pred_u0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_v0 complete dim=2
#pragma HLS DEPENDENCE variable=pred_u0 inter false
#pragma HLS DEPENDENCE variable=pred_v0 inter false
#pragma HLS ARRAY_PARTITION variable=pred_u1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_v1 complete dim=2
#pragma HLS RESOURCE variable=pred_u0 core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=pred_v0 core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=pred_u1 core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=pred_v1 core=RAM_S2P_BRAM
	U1 flag_u0 = 0;
	U1 flag_u1 = 0;

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
		if (!((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + mod_info_curr->cu_width + (dx ? 4 : 0) <= SWW && (qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + mod_info_curr->cu_height + (dy ? 4 : 0) <= SWH)) { return FALSE; }

		w >= 8 && h >= 8 ? ((void)0) : __assert_HLS("w >= 8 && h >= 8", "../separate/com_mc.c", (unsigned int)3258,
			__FUNCTION__);
		//add by luke
		U2 UV_flag;
		get_ref_u_wsc8(Fetch_Ref_window_ptr, refi[REFP_0], REFP_0, &UV_flag);

#if SUB_TMVP
#pragma HLS ARRAY_PARTITION variable=core_sbtmvp->sbTmvp complete dim=0
		if (core_sbtmvp->sbTmvp_flag == 1 && cu_mode >= MODE_SKIP && skip_idx == 0 && mod_info_curr->cu_width >= SBTMVP_MIN_SIZE && mod_info_curr->cu_height >= SBTMVP_MIN_SIZE)
		{
			int cu_width = mod_info_curr->cu_width;
			int cu_height = mod_info_curr->cu_height;
			int sub_w = cu_width >> 2;
			int sub_h = cu_height >> 2;

			int h = 0;
			int w = 0;
			int tmp_x = x;
			int tmp_y = y;
			pel pred_tmp_u[8][8];
			pel pred_tmp_v[8][8];
#pragma HLS ARRAY_PARTITION variable=pred_tmp_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_tmp_v complete dim=2
			for (int k = 0; k < SBTMVP_NUM; k++)
			{
				if (core_sbtmvp->sbTmvp[k].ref_idx[0] >= 0) {
					w = (k % 2) * sub_w;
					h = (k / 2) * sub_h;
					x = tmp_x + (w << 1);
					y = tmp_y + (h << 1);

					/*x_ext = cu->x_pos << 2;
					y_ext = cu->y_pos << 2;
					w_ext = cu->cu_width << 2;
					h_ext = cu->cu_height << 2;*/
					x_ext = x << 2;
					y_ext = y << 2;
					w_ext = sub_w << 2;
					h_ext = sub_h << 2;

					int mv_sbtmvp[2];
#pragma HLS ARRAY_PARTITION variable=mv_sbtmvp complete dim=0
					mv_sbtmvp[0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv_sbtmvp[1] = core_sbtmvp->sbTmvp[k].mv[0][1];
					/*mv_t[0] = core_sbtmvp->sbTmvp[k].mv[0][0];
					mv_t[1] = core_sbtmvp->sbTmvp[k].mv[0][1];*/

					if (x_ext + ((int)mv_sbtmvp[0]) < min_clip[0]) {
						mv_sbtmvp[0] = ((min_clip[0] - x_ext));
					}
					if (y_ext + ((int)mv_sbtmvp[1]) < min_clip[1]) {
						mv_sbtmvp[1] = ((min_clip[1] - y_ext));
					}
					if (x_ext + ((int)mv_sbtmvp[0]) + w_ext - 4 > max_clip[0]) {
						mv_sbtmvp[0] = ((max_clip[0] - x_ext - w_ext + 4));
					}
					if (y_ext + ((int)mv_sbtmvp[1]) + h_ext - 4 > max_clip[1]) {
						mv_sbtmvp[1] = ((max_clip[1] - y_ext - h_ext + 4));
					}

					x_ext_of_ctu = x_ext & ((1 << 7 >> 1 << 2) - 1);
					y_ext_of_ctu = y_ext & ((1 << 7 >> 1 << 2) - 1);
					qpel_gmv_x_of_ctu = x_ext_of_ctu + (mv_sbtmvp[0]);
					qpel_gmv_y_of_ctu = y_ext_of_ctu + (mv_sbtmvp[1]);
					qpel_gmv_x_of_refp_cu = qpel_gmv_x_of_ctu + (REFP_SURD_OFF_W << 2);
					qpel_gmv_y_of_refp_cu = qpel_gmv_y_of_ctu + (REFP_SURD_OFF_H << 2);

					dx = qpel_gmv_x_of_refp_cu & 3;
					dy = qpel_gmv_y_of_refp_cu & 3;
					//assert((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + cu->cu_width + (dx ? 4 : 0) < SWW && (qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + cu->cu_height + (dy ? 4 : 0) < SWH);
					if (!((qpel_gmv_x_of_refp_cu >> 2) - (dx ? 3 : 0) >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + (mod_info_curr->cu_width >> 1) + (dx ? 4 : 0) <= SWW &&
						(qpel_gmv_y_of_refp_cu >> 2) - (dy ? 3 : 0) >= 0 && (qpel_gmv_y_of_refp_cu >> 2) + (mod_info_curr->cu_height >> 1) + (dy ? 4 : 0) <= SWH))
					{
						return FALSE;
					}

					/*w >= 8 && h >= 8 ? ((void)0) : __assert_HLS("w >= 8 && h >= 8", "../separate/com_mc.c", (unsigned int)3258,
						__FUNCTION__);*/
						/*if (!((qpel_gmv_x_of_refp_cu >> 2) - 3 >= 0 && (qpel_gmv_x_of_refp_cu >> 2) + sub_w + 4 < SWW && (qpel_gmv_y_of_refp_cu >> 2) - 3 >= 0 &&
							(qpel_gmv_y_of_refp_cu >> 2) + sub_h + 4 < SWH)) {
							return FALSE;
						}*/
						/*if (sub_w > 64 || sub_h > 64) {
							printf("111");
						}*/
						//com_tbl_mc_c_1((int)mv[0], (int)mv[1], ref_y, qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu,
							//stride_luma, cu->cu_width, pred_tmp, 0, 0, sub_w, sub_h, bit_depth, 0);
					com_tbl_mc_c_1_uv8_wsc(mv_sbtmvp[0], mv_sbtmvp[1],  qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, pred_tmp_u,pred_tmp_v,
						mod_info_curr->cu_width >> 2, mod_info_curr->cu_height >> 2, bit_depth, Fetch_Ref_window_ptr,UV_flag);

		
					//copy pred_tmp to pred output
					//int offset = h * 64 + w;
					 pel pred_u0_buf[8];
					 pel pred_v0_buf[8];
#pragma HLS ARRAY_PARTITION variable=pred_u0_buf complete dim=0
#pragma HLS ARRAY_PARTITION variable=pred_v0_buf complete dim=0
					for (int i = 0; i < sub_h; i++)
					{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=4 max=4 avg=4
						for (int j = 0; j < 8; j++)
						{
							pred_u0_buf[j] = pred_u0[h + i][j] ;
							pred_v0_buf[j] = pred_v0[h + i][j] ;	
						}
						for (int j = 0; j < 4; j++)
						{
							if (j < sub_w) {
								pred_u0_buf[w + j] = pred_tmp_u[i][j];//[offset + i * 64 + j]  [i * 64 + j]
								pred_v0_buf[w + j] = pred_tmp_v[i][j];//[offset + i * 64 + j]  [i * 64 + j]
							}
						}
						for (int j = 0; j < 8; j++)
						{
								pred_u0[h + i][j] = pred_u0_buf[j];//[offset + i * 64 + j]  [i * 64 + j]
								pred_v0[h + i][j] = pred_v0_buf[j];//[offset + i * 64 + j]  [i * 64 + j]	
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
					printf("%s:%d:sbtmvp refi is invalid [%d,%d,%d,%d]\n", __FUNCTION__, 3267, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height);
#endif
				}

			}
		}
		else {
#endif

		com_tbl_mc_c_1_uv8_wsc(mv[REFP_0][0], mv[REFP_0][1],  qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, pred_u0, pred_v0, 
			mod_info_curr->cu_width >> 1, mod_info_curr->cu_height >> 1, bit_depth, Fetch_Ref_window_ptr, UV_flag);

#if SUB_TMVP
		}
#endif
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
			if (flag_u0 == 1) {
				for (int j = 0; j < h1; j++)
				{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
					for (int i = 0; i < 8; i++)
					{
						//if (i < w1) {
						pred_u[j][i] = pred_u0[j][i];
						pred_v[j][i] = pred_v0[j][i];
						pred1_u[j][i] = pred_u0[j][i];
						pred1_v[j][i] = pred_v0[j][i];
						//}
					}
				}
			}
			return 1;
		}
	}

	mv_t[0] = mv[REFP_1][0];
	mv_t[1] = mv[REFP_1][1];
	//static pel pred_snd_u[8][8], pred_snd_v[8][8];
	if (REFI_IS_VALID(refi[REFP_1]))
	{
		/* backward */
		//pel(*pred_buf_u)[8] = bidx ? pred_snd_u : pred_u;
		//pel(*pred_buf_v)[8] = bidx ? pred_snd_v : pred_v;
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

		U2 UV_flag;
		get_ref_u_wsc8(Fetch_Ref_window_ptr, refi[REFP_1], REFP_1, &UV_flag);

		com_tbl_mc_c_1_uv8_wsc(mv[REFP_1][0], mv[REFP_1][1],  qpel_gmv_x_of_refp_cu, qpel_gmv_y_of_refp_cu, pred_u1,pred_v1,
			mod_info_curr->cu_width >> 1, mod_info_curr->cu_height >> 1, bit_depth, Fetch_Ref_window_ptr, UV_flag);
		bidx++;
		flag_u1 = 1;
	}

	if ((flag_u0 == 1) && (flag_u1 == 1)) {
		for (int j = 0; j < h1; j++)
	{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
			for (int i = 0; i < 8; i++)
			{
				//if (i < w1) {
				pred_u[j][i] = (pred_u0[j][i] + pred_u1[j][i] + 1) >> 1;
				pred_v[j][i] = (pred_v0[j][i] + pred_v1[j][i] + 1) >> 1;
				pred1_u[j][i] = (pred_u0[j][i] + pred_u1[j][i] + 1) >> 1;
				pred1_v[j][i] = (pred_v0[j][i] + pred_v1[j][i] + 1) >> 1;
				//}
			}
		}
	}
	else if ((flag_u0 == 1) && (flag_u1 == 0)) {
		for (int j = 0; j < h1; j++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
			for (int i = 0; i < 8; i++)
			{
				//if (i < w1) {
				pred_u[j][i] = pred_u0[j][i];
				pred_v[j][i] = pred_v0[j][i];
				pred1_u[j][i] = pred_u0[j][i];
				pred1_v[j][i] = pred_v0[j][i];
				//}
			}
		}
	}
	else if ((flag_u0 == 0) && (flag_u1 == 1)) {
		for (int j = 0; j < h1; j++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT min=8 max=8 avg=8
			for (int i = 0; i < 8; i++)
			{
			//if (i < w1) {
				pred_u[j][i] = pred_u1[j][i];
				pred_v[j][i] = pred_v1[j][i];
				pred1_u[j][i] = pred_u1[j][i];
				pred1_v[j][i] = pred_v1[j][i];
			//}
		}
	}
}

#endif

	return 1;

	//int stride_org_c = 64;

}


void calcDiffChroma8( U7 cu_width_c, U7 cu_height_c, s16 orgU[8][8], s16 pred[8][8], s16 coef_tmp[8][8],int rdo_idx, int all_rdo_mode)
{
	//if (cu_mode != 0) {
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred complete dim=2
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=2
	s16 coef_tmp_buf[8];
#pragma HLS ARRAY_PARTITION variable=coef_tmp_buf complete dim=0
	if (rdo_idx< all_rdo_mode) {
		for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE ii=1
			for (int j = 0; j < 8; j++) {
				if ((j < cu_width_c) && (i < cu_height_c)) {
					coef_tmp_buf[j] = ((s16)(((int)orgU[i][j]) - ((int)pred[i][j])));
				}
				else {
					coef_tmp_buf[j] = 0;
				}
			}
			for (int j = 0; j < 8; j++) {
				coef_tmp[i][j] = coef_tmp_buf[j];
			}
			}
		}
}

void rdoq_8_all_hardware_chroma(U3 cu_width_log2, U3 cu_height_log2, S16 src_coef[8][8], U6 qp, S32 q_bits, S16 tmp_dst_coef_out[64],
	S32 rdoq_est_cbf[3][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2],
	S32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda, U4 bit_depth, int* final_x, int* final_y)
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
	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	S16 tmp_dst_coef[8];
	U36 tmp_level_double[8];
	S16 tmp_coef[8];
	S64 d64_cost_last_zero[8] = { 0 };
	U24 lev_opt[8];
	S64 mat_coded_cost[8];
	S64 mat_uncoded_cost[8];
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U32 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost[64] = { 0 };
	S64 base_cost_buffer[64] = { 0 };
	S64 base_cost_buffer_tmp[8] = { 0 };
	S64 d64_best_cost_tmp = 0;
	U24 mat_prev_level[8] = { 0 };
	U1 mat_run[8] = { 0 };
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	S64 d64_cost_last_one_tmp[8] = { 0 };
	S64 finalRdoqCost = 0;
	S64 tempCost[8];   // all scan line cost
	S64 endPosCost[8]; // as the last position cost.
	S64 rdoqD64LastOne[8] = { -1,-1,-1,-1,-1,-1,-1,-1 }; // shifter buffer the d64_cost_last_one_tmp
	s8 last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 final_last_x = -1, final_last_y = -1;
	s8 final_rdoq_last_x = -1, final_rdoq_last_y = -1;
	int shift_t;
	if (cu_width == 4 || cu_height == 4) {
		shift_t = 4;
	}
	else {
		shift_t = 8;
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
	for (int j = 0; j < cu_width; j++) // vertical
	{
		for (int i = 0; i < cu_height; i++)
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
		for (int i = 0; i < cu_height; i++)
		{
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
		}
		for (int i = 0; i < cu_height; i++)
		{
			U24 level;
			U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);
			level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
				mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, 0, rdoq_est_run_local, rdoq_est_level_local);
			S17 level_cbf = level;
			tmp_dst_coef[i] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
			tmp_dst_coef_out[i * cu_width + j] = tmp_dst_coef[i];
			mat_uncoded_cost[i] = d64_uncoded_cost;
			mat_coded_cost[i] = d64_coded_cost;
			lev_opt[i] = level;
		}

		for (int i = 0; i < cu_height; i++)
		{
			if (i + j < 3) {
				ace_log2 = i + j;
			}
			else if (i + j < 4) {
				ace_log2 = 3;
			}
			else if (i + j < 6) {
				ace_log2 = 4;
			}
			else {
				ace_log2 = 5;
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
		}
		for (int i = 0; i < cu_height; i++)
		{
			if (lev_opt[i])
			{
				base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
			}
			else
			{
				base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i];
			}
		}
		for (int i = 0; i < cu_height; i++)
		{
			if (i + j < 3) {
				ace_log2 = i + j;
			}
			else if (i + j < 4) {
				ace_log2 = 3;
			}
			else if (i + j < 6) {
				ace_log2 = 4;
			}
			else {
				ace_log2 = 5;
			}
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
		for (int i = 0; i < cu_height; i++) // horizontal
		{
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
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];
		if (rdoq_last_y[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];
		for (int i = 1; i < shift_t; i++)
		{
			rdoq_last_x[i - 1] = rdoq_last_x[i];
			rdoq_last_y[i - 1] = rdoq_last_y[i];
			tempCost[i - 1] = tempCost[i];
			endPosCost[i - 1] = endPosCost[i];
			rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
		}
	}

	for (int i = 0; i < cu_width - 1; i++)
	{
		if ((rdoq_last_x[i] + rdoq_last_y[i]) % 2 == 1) // above right scan.
		{
			endPosCost[i] = tempCost[i] - endPosCost[i];
		}
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[i];
		int index = rdoq_last_x[i] * cu_width + rdoq_last_y[i];
		if (rdoq_last_x[i] != -1 && temp_RdoqCost + rdoqD64LastOne[i] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[i];
			final_rdoq_last_x = rdoq_last_x[i];
			final_rdoq_last_y = rdoq_last_y[i];

		}
		finalRdoqCost += tempCost[i];
	}
	*final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;
}

void rdoq_all_hardware_chroma8(U3 cu_width_log2, U3 cu_height_log2, s16 src_coef[8][8], U6 qp, S16 tmp_dst_coef_out[8][8],
	int rdoq_est_cbf[3][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2],
	s32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 d_lambda, U4 bit_depth, int* final_x, int* final_y, int rdo_idx, int all_rdo_mode)
{
#pragma HLS ARRAY_PARTITION variable=src_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef_out complete dim=1

#pragma HLS ARRAY_PARTITION variable=rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_est_run_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_est_level_local complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_est_last_local complete dim=0

	U8 cu_width = 1 << cu_width_log2;
	U8 cu_height = 1 << cu_height_log2;
	U16 quant_scale[80] = { (16302), (15024), (13777), (12634), (11626), (10624), (9742), (8958), (8192), (7512), (6889), (6305), (5793), (5303), (4878), (4467), (4091), (3756), (3444), (3161), (2894), (2654), (2435), (2235), (2048), (1878), (1722), (1579), (1449), (1329), (1218), (1117), (1024), (939), (861), (790), (724), (664), (609), (558), (512), (470), (430), (395), (362), (332), (304), (279), (256), (235), (215), (197), (181), (166), (152), (140), (128), (116), (108), (99), (91), (83), (76), (69), (64), (59), (54), (49), (45), (41), (38), (35), (32), (30), (27), (25), (23), (21), (19), (18) };
		U30 err_scale_tbl[80][6] = { 32932, 65865, 131731, 263462, 526925, 1053850, 35734, 71468, 142936, 285873, 571747, 1143495, 38968, 77937, 155874, 311749, 623498, 1246996, 42494, 84988, 169976, 339953, 679906, 1359812, 46178, 92356, 184713, 369427, 738855, 1477711, 50533, 101067, 202135, 404270, 808540, 1617081, 55108, 110217, 220435, 440871, 881742, 1763484, 59932, 119864, 239728, 479456, 958912, 1917824, 65536, 131072, 262144, 524288, 1048576, 2097152, 71468, 142936, 285873, 571747, 1143495, 2286990, 77931, 155863, 311726, 623452, 1246905, 2493811, 85150, 170300, 340600, 681200, 1362400, 2724800, 92675, 185351, 370703, 741406, 1482812, 2965625, 101239, 202478, 404956, 809912, 1619825, 3239650, 110059, 220119, 440238, 880477, 1760954, 3521908, 120186, 240372, 480744, 961488, 1922976, 3845952, 131232, 262464, 524928, 1049857, 2099715, 4199430, 142936, 285873, 571747, 1143495, 2286990, 4573980, 155885, 311771, 623543, 1247086, 2494173, 4988347, 169842, 339684, 679368, 1358736, 2717473, 5434947, 185511, 371023, 742046, 1484093, 2968187, 5936374, 202287, 404574, 809149, 1618299, 3236599, 6473198, 220480, 440961, 881923, 1763846, 3527693, 7055387, 240210, 480421, 960842, 1921685, 3843371, 7686742, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 285873, 571747, 1143495, 2286990, 4573980, 9147960, 311771, 623543, 1247086, 2494173, 4988347, 9976695, 340006, 680013, 1360027, 2720055, 5440110, 10880221, 370511, 741022, 1482045, 2964090, 5928181, 11856362, 403966, 807932, 1615864, 3231728, 6463457, 12926914, 440780, 881561, 1763122, 3526245, 7052491, 14104982, 480636, 961272, 1922545, 3845091, 7690183, 15380366, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 571747, 1143495, 2286990, 4573980, 9147960, 18295920, 623543, 1247086, 2494173, 4988347, 9976695, 19953390, 679583, 1359166, 2718333, 5436667, 10873334, 21746669, 741534, 1483068, 2966137, 5932275, 11864550, 23729101, 808540, 1617081, 3234162, 6468324, 12936648, 25873296, 881561, 1763122, 3526245, 7052491, 14104982, 28209965, 962134, 1924268, 3848537, 7697074, 15394148, 30788296, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 1142278, 2284557, 4569114, 9138228, 18276456, 36552913, 1248537, 2497074, 4994148, 9988296, 19976592, 39953184, 1359166, 2718333, 5436667, 10873334, 21746669, 43493339, 1483068, 2966137, 5932275, 11864550, 23729101, 47458202, 1617081, 3234162, 6468324, 12936648, 25873296, 51746593, 1766022, 3532045, 7064090, 14128181, 28256363, 56512727, 1924268, 3848537, 7697074, 15394148, 30788296, 61576592, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 2284557, 4569114, 9138228, 18276456, 36552913, 73105826, 2497074, 4994148, 9988296, 19976592, 39953184, 79906368, 2725233, 5450466, 10900932, 21801864, 43603728, 87207457, 2966137, 5932275, 11864550, 23729101, 47458202, 94916404, 3234162, 6468324, 12936648, 25873296, 51746593, 103493187, 3532045, 7064090, 14128181, 28256363, 56512727, 113025455, 3834792, 7669584, 15339168, 30678337, 61356675, 122713351, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 4628197, 9256395, 18512790, 37025580, 74051160, 148102320, 4971026, 9942053, 19884107, 39768215, 79536431, 159072862, 5422938, 10845877, 21691754, 43383508, 86767016, 173534032, 5899680, 11799360, 23598721, 47197442, 94394885, 188789771, 6468324, 12936648, 25873296, 51746593, 103493187, 206986375, 7064090, 14128181, 28256363, 56512727, 113025455, 226050910, 7780737, 15561475, 31122951, 62245902, 124491805, 248983611, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 9099506, 18199013, 36398027, 72796055, 145592111, 291184223, 9942053, 19884107, 39768215, 79536431, 159072862, 318145725, 10956549, 21913098, 43826196, 87652393, 175304787, 350609575, 11930464, 23860929, 47721858, 95443717, 190887435, 381774870, 13094412, 26188824, 52377649, 104755299, 209510599, 419021199, 14128181, 28256363, 56512727, 113025455, 226050910, 452101820, 15339168, 30678337, 61356675, 122713351, 245426702, 490853405, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 17895697, 35791394, 71582788, 143165576, 286331153, 572662306, 19884107, 39768215, 79536431, 159072862, 318145725, 636291451, 21474836, 42949672, 85899345, 171798691, 343597383, 687194767, 23342213, 46684427, 93368854, 186737708, 373475417, 746950834, 25565281, 51130563, 102261126, 204522252, 409044504, 818089008, 28256363, 56512727, 113025455, 226050910, 452101820, 904203641, 29826161, 59652323, 119304647, 238609294, 477218588, 954437176 };
	
	const int log2_size = (cu_width_log2 + cu_height_log2) >> 1;
	const int tr_shift = get_transform_shift(bit_depth, log2_size);
	const U14 scale = quant_scale[qp];
	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	S32 q_bits = 14 + tr_shift;
	const s64 lambda = (d_lambda * (1 << 15) + 500) >> 10;
	const int ns_shift = ((cu_width_log2 + cu_height_log2) & 1) ? 7 : 0;
	const int ns_scale = ((cu_width_log2 + cu_height_log2) & 1) ? 181 : 1;
	const int ns_offset = ((cu_width_log2 + cu_height_log2) & 1) ? (1 << (ns_shift - 1)) : 0;
	const int q_value = (scale * ns_scale + ns_offset) >> ns_shift;
	//const int ctx_last = (ch_type == Y_C) ? 0 : 1;

	U36 tmp_level_double[8];
	S16 tmp_coef[8];
	S64 d64_cost_last_zero[8];// = { 0 };
	U24 lev_opt[8];
	S64 mat_coded_cost[8];
	S64 mat_uncoded_cost[8];
	S64 base_cost_buffer_tmp[8];// = { 0 };
	U24 mat_prev_level[8];// = { 0 };
	U1 mat_run[8];// = { 0 };


	S64 finalRdoqCost = 0;
	S64 tempCost[8];   // all scan line cost
	S64 endPosCost[8]; // as the last position cost.
	
	S64 d64_cost_last_one_tmp[8];// = { 0 };
	S64 rdoqD64LastOne[8];// = { -1,-1,-1,-1,-1,-1,-1,-1 }; // shifter buffer the d64_cost_last_one_tmp
	s8 rdoq_last_x[8];// = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_last_y[8];// = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 final_rdoq_last_x = -1, final_rdoq_last_y = -1;
#pragma HLS ARRAY_PARTITION variable=tmp_level_double complete dim=1
#pragma HLS ARRAY_PARTITION variable=tmp_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_zero complete dim=1
#pragma HLS ARRAY_PARTITION variable=lev_opt complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_coded_cost complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_uncoded_cost complete dim=1
#pragma HLS ARRAY_PARTITION variable=base_cost_buffer_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_prev_level complete dim=1
#pragma HLS ARRAY_PARTITION variable=mat_run complete dim=1
#pragma HLS ARRAY_PARTITION variable=tempCost complete dim=1
#pragma HLS ARRAY_PARTITION variable=endPosCost complete dim=1
#pragma HLS ARRAY_PARTITION variable=d64_cost_last_one_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=rdoqD64LastOne complete dim=1
#pragma HLS ARRAY_PARTITION variable=rdoq_last_x complete dim=1
#pragma HLS ARRAY_PARTITION variable=rdoq_last_y complete dim=1
	for (int i = 0; i < 8; i++)
	{
#pragma HLS UNROLL
		d64_cost_last_one_tmp[i] = 0;
		rdoqD64LastOne[i] = -1;
		rdoq_last_x[i] = -1;
		rdoq_last_y[i] = -1;
	}

	int shift_t;
	if (cu_height == 8) {
		shift_t = 8;
	}
	else {
		shift_t = 4;
	}

//	S64 d64_best_cost_tmp  = rdoq_est_cbf[ch_type][0] * lambda - rdoq_est_cbf[ch_type][1] * lambda;
	S64 d64_best_cost_tmp  = (rdoq_est_cbf[ch_type][0] - rdoq_est_cbf[ch_type][1]) * lambda;
		if (rdo_idx >= all_rdo_mode) {
		}
		else {
	for (int j = 0; j < 8; j++) // vertical
	{
#pragma HLS PIPELINE II=1
		if (j < cu_width) {
			for (int i = 0; i < 8; i++)
			{
				if (i < cu_height) {
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
			for (int i = 0; i < 8; i++)
			{
				if (i < cu_height) {
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
				}
			}
			for (int i = 0; i < 8; i++)
			{
				if (i < cu_height) {
					s64 d64_uncoded_cost;
					s64 d64_coded_cost;
					U24 level;
					U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);
					level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
						mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, 0, rdoq_est_run_local, rdoq_est_level_local);
					S17 level_cbf = level;
					tmp_dst_coef_out[i][j] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
					mat_uncoded_cost[i] = d64_uncoded_cost;
					mat_coded_cost[i] = d64_coded_cost;
					lev_opt[i] = level;
				}
			}

			for (int i = 0; i < 8; i++)
			{
				if (i < cu_height) {
					U4 ace_log2;
					if (i + j < 3) {
						ace_log2 = i + j;
					}
					else if (i + j < 4) {
						ace_log2 = 3;
					}
					else if (i + j < 6) {
						ace_log2 = 4;
					}
					else {
						ace_log2 = 5;
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
					d64_cost_last_zero[i] = rdoq_est_last_local[1][flag][ace_log2][0] * lambda;
				}
			}
			for (int i = 0; i < 8; i++)
			{
				if (i < cu_height) {
					if (lev_opt[i])
					{
						base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
					}
					else
					{
						base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i];
					}
				}
			}
			for (int i = 0; i < 8; i++)
			{
				if (i < cu_height) {
					U4 ace_log2;
					if (i + j < 3) {
						ace_log2 = i + j;
					}
					else if (i + j < 4) {
						ace_log2 = 3;
					}
					else if (i + j < 6) {
						ace_log2 = 4;
					}
					else {
						ace_log2 = 5;
					}
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
					d64_cost_last_one_tmp[i] = GET_I_COST(rdoq_est_last_local[1][flag][ace_log2][1], lambda); // block scan order
				}
			}
			for (int i = 0; i < 8; i++) // horizontal
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
							//last_x[i] = i;
							//last_y[i] = j;
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
			for (int i = 1; i < 8; i++)
			{
				if (i < shift_t) {
					rdoq_last_x[i - 1] = rdoq_last_x[i];
					rdoq_last_y[i - 1] = rdoq_last_y[i];
					tempCost[i - 1] = tempCost[i];
					endPosCost[i - 1] = endPosCost[i];
					rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
				}
			}
		}
	}

	for (int i = 0; i < 7; i++)
	{
#pragma HLS UNROLL
		if ((rdoq_last_x[i] + rdoq_last_y[i]) % 2 == 1) // above right scan.
		{
			endPosCost[i] = tempCost[i] - endPosCost[i];
		}
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[i];
		int index = rdoq_last_x[i] * cu_width + rdoq_last_y[i];
		if (rdoq_last_x[i] != -1 && temp_RdoqCost + rdoqD64LastOne[i] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[i];
			final_rdoq_last_x = rdoq_last_x[i];
			final_rdoq_last_y = rdoq_last_y[i];

		}
		finalRdoqCost += tempCost[i];
	}
	*final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;
}
}
void CleanNz_8_all(int cu_width, int cu_height, int* num_nz_coef, s16 dst_tmp[8][8], S16 tmp_dst_coef[8][8], int final_x, int final_y,
	s16 coef_uv_pb_part[8][8], int rdo_idx, int all_rdo_mode)
{
#pragma HLS ARRAY_PARTITION variable=dst_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=tmp_dst_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part complete dim=1
	int num_nz_coef_tmp = 0;
	S16 buffer_tmp;
	if (rdo_idx < all_rdo_mode) {
	for (int j = 0; j < 8; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 8; i++)
		{
			if ((j < cu_width)&&(i < cu_height)) {
			if (i + j < final_x + final_y) {
					buffer_tmp = tmp_dst_coef[i][j];
			}
			else if (i + j == final_x + final_y) {
				if ((final_x + final_y) % 2 == 1) {
					if (i > final_x && j < final_y) {
							buffer_tmp = 0;
					}
					else {
							buffer_tmp = tmp_dst_coef[i][j];
					}
				}
				else {
					if (i < final_x && j > final_y) {
							buffer_tmp = 0;
					}
					else {
							buffer_tmp = tmp_dst_coef[i][j];
						}
					}
				}
				else {
					buffer_tmp = 0;
				}
			}
			else {
				buffer_tmp = 0;
			}
			num_nz_coef_tmp += !!(buffer_tmp);
			dst_tmp[i][j] = buffer_tmp;
			coef_uv_pb_part[i][j] = buffer_tmp;
		}
	}
	*num_nz_coef = num_nz_coef_tmp;
	}
}


void rdoq_4_8_hardware_chroma(U3 cu_width_log2, U3 cu_height_log2, S16 src_coef[8][8], U6 qp, S32 q_bits, S16 tmp_dst_coef_out[64],
	S32 rdoq_est_cbf[3][2], s32 rdoq_est_run_local[24][2], s32 rdoq_est_level_local[24][2],
	S32 rdoq_est_last_local[2][6][12][2], U3 ch_type, U1 is_intra, S64 lambda, U4 bit_depth, int* final_x, int* final_y)
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
	s64 err_scale = err_scale_tbl[qp][log2_size - 1];
	S16 tmp_dst_coef[8];
	U36 tmp_level_double[8];
	S16 tmp_coef[8];
	S64 d64_cost_last_zero[8] = { 0 };
	U24 lev_opt[8];
	S64 mat_coded_cost[8];
	S64 mat_uncoded_cost[8];
	d64_cost_last_zero[0] = 0;
	U4 ace_log2 = 0;
	U32 scan_pos = 0;
	S64 d64_best_cost = 0;
	S64 d64_base_cost[64] = { 0 };
	S64 base_cost_buffer[64] = { 0 };
	S64 base_cost_buffer_tmp[8] = { 0 };
	S64 d64_best_cost_tmp = 0;
	U24 mat_prev_level[8] = { 0 };
	U1 mat_run[8] = { 0 };
	s64 d64_uncoded_cost = 0;
	s64 d64_coded_cost = 0;
	S64 d64_cost_last_one_tmp[8] = { 0 };
	S64 finalRdoqCost = 0;
	S64 tempCost[8];   // all scan line cost
	S64 endPosCost[8]; // as the last position cost.
	S64 rdoqD64LastOne[8] = { -1,-1,-1,-1,-1,-1,-1,-1 }; // shifter buffer the d64_cost_last_one_tmp
	s8 last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_last_x[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 rdoq_last_y[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
	s8 final_last_x = -1, final_last_y = -1;
	s8 final_rdoq_last_x = -1, final_rdoq_last_y = -1;
	int shift_t = 8;

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
	for (int j = 0; j < 4; j++) // vertical
	{
		/*if (j < 4) {
			max_num = 8;
		}
		else {
			max_num = 0;
		}*/
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
		for (int i = 0; i < 8; i++)
		{
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
		}
		for (int i = 0; i < 8; i++)
		{
			U24 level;
			U5 ctx_run = ((COM_MIN((U24)(mat_prev_level[i] - 1), (U24)(5))) << 1) + (ch_type == Y_C ? 0 : 12);
			level = get_coded_level_rl(&d64_uncoded_cost, &d64_coded_cost, tmp_level_double[i], abs(tmp_coef[i]),
				mat_run[i], ctx_run, ctx_run, q_bits, err_scale, lambda, 0, rdoq_est_run_local, rdoq_est_level_local);
			S17 level_cbf = level;
			tmp_dst_coef[i] = (S16)(tmp_coef[i] < 0 ? (S16)(-level_cbf) : (S16)level_cbf);
			tmp_dst_coef_out[i * cu_width + j] = tmp_dst_coef[i];
			mat_uncoded_cost[i] = d64_uncoded_cost;
			mat_coded_cost[i] = d64_coded_cost;
			lev_opt[i] = level;
		}

		for (int i = 0; i < 8; i++)
		{
			if (i + j < 3) {
				ace_log2 = i + j;
			}
			else if (i + j < 4) {
				ace_log2 = 3;
			}
			else if (i + j < 6) {
				ace_log2 = 4;
			}
			else {
				ace_log2 = 5;
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
		}
		for (int i = 0; i < 8; i++)
		{
			if (lev_opt[i])
			{
				base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i] + d64_cost_last_zero[i];
			}
			else
			{
				base_cost_buffer_tmp[i] = mat_coded_cost[i] - mat_uncoded_cost[i];
			}
		}
		for (int i = 0; i < 8; i++)
		{
			if (i + j < 3) {
				ace_log2 = i + j;
			}
			else if (i + j < 4) {
				ace_log2 = 3;
			}
			else if (i + j < 6) {
				ace_log2 = 4;
			}
			else {
				ace_log2 = 5;
			}
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
		for (int i = 0; i < 8; i++) // horizontal
		{
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
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[0];
		if (rdoq_last_y[0] != -1 && temp_RdoqCost + rdoqD64LastOne[0] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[0];
			final_rdoq_last_x = rdoq_last_x[0];
			final_rdoq_last_y = rdoq_last_y[0];
		}
		finalRdoqCost += tempCost[0];
		for (int i = 1; i < shift_t; i++)
		{
			rdoq_last_x[i - 1] = rdoq_last_x[i];
			rdoq_last_y[i - 1] = rdoq_last_y[i];
			tempCost[i - 1] = tempCost[i];
			endPosCost[i - 1] = endPosCost[i];
			rdoqD64LastOne[i - 1] = rdoqD64LastOne[i];
		}
	}

	for (int i = 0; i < cu_height - 1; i++)
	{
		if ((rdoq_last_x[i] + rdoq_last_y[i]) % 2 == 1) // above right scan.
		{
			endPosCost[i] = tempCost[i] - endPosCost[i];
		}
		S64 temp_RdoqCost = finalRdoqCost + endPosCost[i];
		int index = rdoq_last_x[i] * cu_width + rdoq_last_y[i];
		if (rdoq_last_x[i] != -1 && temp_RdoqCost + rdoqD64LastOne[i] < d64_best_cost_tmp)
		{
			d64_best_cost_tmp = temp_RdoqCost + rdoqD64LastOne[i];
			final_rdoq_last_x = rdoq_last_x[i];
			final_rdoq_last_y = rdoq_last_y[i];

		}
		finalRdoqCost += tempCost[i];
	}
	*final_x = final_rdoq_last_x;
	*final_y = final_rdoq_last_y;
}



void dquantChroma8(U8 w, U8 h, U3 cu_width_log2_c, U3 cu_height_log2_c, U4 bit_depth, U6 qp_y, int *num_nz, s16 coef_tmp[8][8], s16 coef_dq[8][8],  int num_nz_tmp, int rdo_idx, int all_rdo_mode)
{
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=coef_dq complete dim=1
	if (rdo_idx < all_rdo_mode) {
	//U3 tb_height_log2 = com_tbl_log2[h];
	if (num_nz_tmp) {
			dquant_8_chroma(w, h, qp_y, coef_tmp, coef_dq, cu_width_log2_c, cu_height_log2_c, bit_depth, num_nz, num_nz_tmp);
	}
	else {
			*num_nz = 0;
		}
	}
}
void inv_dct_col_Chroma8(U8 w, U8 h, s16 coeff[8][8], s16 block[8][8], U4 shift)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coeff complete dim=1
#pragma HLS ARRAY_PARTITION variable=block complete dim=1

	U7 size = 8;
	if (h == 4) {
		int E[2], O[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=1
#pragma HLS ARRAY_PARTITION variable=O complete dim=1
		int rnd_factor = 1 << (shift - 1);
	for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE
		//if (i >= w) break;
		O[0] = (42) * coeff[1][i] + (17) * coeff[3][i];
		O[1] = (17) * coeff[1][i] + (-42) * coeff[3][i];
		E[0] = (32) * coeff[0][i] + (32) * coeff[2][i];
		E[1] = (32) * coeff[0][i] + (-32) * coeff[2][i];
	/*	if (i == 0)		 {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 1) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 2) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 3) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 4) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 5) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 6) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}
		else if (i == 7) {block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
}*/
		block[(i + 0) & 7][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);block[(i + 1) & 7][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);block[(i + 2) & 7][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);block[(i + 3) & 7][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);




	}
	}
	else if (h == 8) {
		int E[4], O[4];
		int EE[2], EO[2];
#pragma HLS ARRAY_PARTITION variable = E complete dim = 1
#pragma HLS ARRAY_PARTITION variable=O complete dim=1
#pragma HLS ARRAY_PARTITION variable=EE complete dim=1
#pragma HLS ARRAY_PARTITION variable=EO complete dim=1
		int rnd_factor = 1 << (shift - 1);
		int k;

		for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE
			//if (i >= w) break;
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
/*			if (i == 0)      {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}
			else if (i == 1) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}
			else if (i == 2) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
			}
			else if (i == 3) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}
			else if (i == 4) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}
			else if (i == 5) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}
			else if (i == 6) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}
			else if (i == 7) {for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}
}*/
			for (k = 0; k < 4; k++) {block[(i + k) & 7][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);block[(i + k + 4) & 7][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);}


		}
	}
}
void invTransVerChroma8(U8 w, U8 h, U3 cu_width_log2_c,  U4 bit_depth,  int num_nz, int *num_nz1, s16 coef_dq[8][8], s16 coef_tmp[8][8], int rdo_idx, int all_rdo_mode)
{
	*num_nz1 = num_nz;
	if (rdo_idx < all_rdo_mode) {
		if (num_nz) {
		U4 shift1 = com_get_inverse_trans_shift(cu_width_log2_c, 0, bit_depth);
		inv_dct_col_Chroma8(w, h, coef_dq, coef_tmp, shift1);
		}
	}
}

void inv_dct_row_Chroma8(U8 w, U8 h, s16 coeff[8][8], s16 block[8][8], U4 shift)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=coeff complete dim=1
#pragma HLS ARRAY_PARTITION variable=block complete dim=2
	U7 size = 8;

	if (w == 4) {
		int E[2], O[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
		int rnd_factor = 1 << (shift - 1);
		for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE
			O[0] = (42) * coeff[(i+1)&7][i] + (17) *  coeff[(i+3)&7][i];
			O[1] = (17) * coeff[(i+1)&7][i] + (-42) * coeff[(i+3)&7][i];
			E[0] = (32) * coeff[(i+0)&7][i] + (32) *  coeff[(i+2)&7][i];
			E[1] = (32) * coeff[(i+0)&7][i] + (-32) * coeff[(i+2)&7][i];
			block[i][0] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] + O[0] + rnd_factor) >> shift);
			block[i][1] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] + O[1] + rnd_factor) >> shift);
			block[i][2] = Clip3(-(1 << 15), (1 << 15) - 1, (E[1] - O[1] + rnd_factor) >> shift);
			block[i][3] = Clip3(-(1 << 15), (1 << 15) - 1, (E[0] - O[0] + rnd_factor) >> shift);
		}
	}
	else if (w == 8) {
		int E[4], O[4];
		int EE[2], EO[2];
#pragma HLS ARRAY_PARTITION variable=E complete dim=0
#pragma HLS ARRAY_PARTITION variable=O complete dim=0
#pragma HLS ARRAY_PARTITION variable=EE complete dim=0
#pragma HLS ARRAY_PARTITION variable=EO complete dim=0

		int rnd_factor = 1 << (shift - 1);
		int k;
		for (int i = 0; i < 8; i++) {
#pragma HLS PIPELINE
			O[0] = (44) * coeff[(i+1)&7][i] + (38) *  coeff[(i+3)&7][i] + (25) *  coeff[(i+5)&7][i] + (9) *   coeff[(i+7)&7][i];
			O[1] = (38) * coeff[(i+1)&7][i] + (-9) *  coeff[(i+3)&7][i] + (-44) * coeff[(i+5)&7][i] + (-25) * coeff[(i+7)&7][i];
			O[2] = (25) * coeff[(i+1)&7][i] + (-44) * coeff[(i+3)&7][i] + (9) *   coeff[(i+5)&7][i] + (38) *  coeff[(i+7)&7][i];
			O[3] = (9) *  coeff[(i+1)&7][i] + (-25) * coeff[(i+3)&7][i] + (38) *  coeff[(i+5)&7][i] + (-44) * coeff[(i+7)&7][i];

			EO[0] = (42) * coeff[(i+2)&7][i] + (17) *  coeff[(i+6)&7][i];
			EO[1] = (17) * coeff[(i+2)&7][i] + (-42) * coeff[(i+6)&7][i];
			EE[0] = (32) * coeff[(i+0)&7][i] + (32) *  coeff[(i+4)&7][i];
			EE[1] = (32) * coeff[(i+0)&7][i] + (-32) * coeff[(i+4)&7][i];

			E[0] = EE[0] + EO[0];
			E[3] = EE[0] - EO[0];
			E[1] = EE[1] + EO[1];
			E[2] = EE[1] - EO[1];
			for (k = 0; k < 4; k++) {
				block[i][k] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[k] + O[k] + rnd_factor)) >> shift);
				block[i][k + 4] = Clip3(-(1 << 15), (1 << 15) - 1, ((int)(E[3 - k] - O[3 - k] + rnd_factor)) >> shift);

			}
		}
	}
}

void invTransHorChroma8(U8 w, U8 h, U4 bit_depth, int num_nz, int *num_nz2, U3 cu_height_log2,
 s16 coef_temp[8][8],   pel resi[8][8], int rdo_idx, int all_rdo_mode)
{

	*num_nz2 = num_nz;
	if (rdo_idx < all_rdo_mode) {
		if (num_nz) {
		U4 shift2 = com_get_inverse_trans_shift(cu_height_log2, 1, bit_depth);
		inv_dct_row_Chroma8(w, h, coef_temp, resi, shift2);
	}
	}
}

void deriveRDCostChroma8(U2 cu_mode, U32 dist_chroma_weight, MD_COM_MODE_BEST* mod_info_best,  int cu_width, int cu_height, 
	s16 rec_tmp[8][8], s16 orgU[8][8], s64 lambda_y,
	s16 coef_tmp[8][8], u8 tscpm_enable_flag, int num_nz[12], s64 cost[6], U4 bit_depth, U1 uv, int rdo_idx,
	U7 cu_width_c,U7 cu_height_c
	)
{
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=2
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=2
#pragma HLS ARRAY_PARTITION variable=coef_tmp complete dim=1
#pragma HLS ARRAY_PARTITION variable=cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
	int bin_cnt_chroma;
	s64 ssd_uv;
	static s64 ssd_u;
	static s64 ssd_v;
	int est_bit_intra_uv_tmp;
	static int est_bit_intra_u;
	static int est_bit_intra_v;
	if (cu_mode == 0) {

		 s8 ipm01 = rdo_idx / 2;
		 ssd_uv = (dist_chroma_weight * enc_ssd_16b_chroma_8(cu_width_c, cu_height_c, rec_tmp, orgU, 
			 bit_depth) + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
		 est_bit_intra_uv_tmp = enc_eco_xcoef_est_Chroma8(coef_tmp, cu_width_c, cu_height_c, num_nz[rdo_idx]);
		 if (uv == 0) {
			 ssd_u = ssd_uv;
			 est_bit_intra_u = est_bit_intra_uv_tmp;

		 }
		if (uv == 1)
		{
			ssd_v = ssd_uv;
			est_bit_intra_v = est_bit_intra_uv_tmp;
			int est_bit_intra = 2 + encode_intra_dir_c_est(ipm01, mod_info_best->ipm[PB0][0], tscpm_enable_flag);
			int est_bit_intra_tmp = est_bit_intra + est_bit_intra_u + est_bit_intra_v;
			bin_cnt_chroma = get_bit_est_intra_chroma8( cu_width, cu_height, est_bit_intra_tmp);
			cost[rdo_idx / 2] = ssd_u + ssd_v + ((bin_cnt_chroma * lambda_y + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE);
		}
	}
}

void updateBestModeChroma8_coefu_mux(int i, s16 coef_u_pb_part_tmp[8], s16 coef_buf_u[8][8])
{
#pragma HLS INLINE
	if (i == 0) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 1) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 2) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 3) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 4) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 5) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 6) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
	else if (i == 7) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u[(j + i) & 7][i] = coef_u_pb_part_tmp[j]; }
	}
}
void updateBestModeChroma8_coefv_mux(int i, s16 coef_v_pb_part_tmp[8], s16 coef_buf_v[8][8])
{
#pragma HLS INLINE
	if (i == 0) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 1) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 2) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 3) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 4) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 5) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 6) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
	else if (i == 7) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v[(j + i) & 7][i] = coef_v_pb_part_tmp[j]; }
	}
}

void updateBestModeChroma8_phase(int num_nz[12],
	s64 cost[6], s16 rec_tmp[12][8][8], s16 coef_uv_pb_part[12][8][8], pel pred[12][8][8],
	int* num_nz1,
	int* num_nz2,
	s16 rec_tmp_buf_u[8][8],
	s16 coef_buf_u[8][8],
	pel pred_buf_u[8][8],

	s16 rec_tmp_buf_v[8][8],
	s16 coef_buf_v[8][8],
	pel pred_buf_v[8][8],
	int* best_ipd_tmp
)
{
	static s64 cost_best1;
	cost_best1 = 9223372036854775807;
	static int best_ipd1;
	best_ipd1 = 0;
	*num_nz1 = 0;
	*num_nz2 = 0;
	for (int rdo_idx = 0; rdo_idx < 6; rdo_idx++) {
#pragma HLS UNROLL
		if (cost[rdo_idx] < cost_best1) {
			cost_best1 = cost[rdo_idx];
			best_ipd1 = rdo_idx;
		}
	}
	*num_nz1 = num_nz[best_ipd1 * 2];
	*num_nz2 = num_nz[best_ipd1 * 2 + 1];
	*best_ipd_tmp = best_ipd1;

	s16 coef_u_pb_part_tmp[8];
	s16 coef_v_pb_part_tmp[8];
#pragma HLS ARRAY_PARTITION variable=coef_u_pb_part_tmp complete  dim=0
#pragma HLS ARRAY_PARTITION variable=coef_v_pb_part_tmp complete  dim=0
	for (U8 i = 0; i < 8; i++)
	{
#pragma HLS PIPELINE
		for (U8 j = 0; j < 8; j++)
		{
			coef_u_pb_part_tmp[j] = coef_uv_pb_part[best_ipd1 * 2][j][i];
			coef_v_pb_part_tmp[j] = coef_uv_pb_part[best_ipd1 * 2 + 1][j][i];
		}
		for (U8 j = 0; j < 8; j++)
		{
			rec_tmp_buf_u[i][j] = rec_tmp[best_ipd1 * 2][i][j];
			pred_buf_u[i][j] = pred[best_ipd1 * 2][i][j];
			rec_tmp_buf_v[i][j] = rec_tmp[best_ipd1 * 2 + 1][i][j];
			pred_buf_v[i][j] = pred[best_ipd1 * 2 + 1][i][j];

			updateBestModeChroma8_coefu_mux(i, coef_u_pb_part_tmp, coef_buf_u);
			updateBestModeChroma8_coefv_mux(i, coef_v_pb_part_tmp, coef_buf_v);
		}
	}
}

void updateModeChroma8_coefu_mux(int i, s16 coef_buf_u_tmp[8], s16 coef_buf_u[8][8])
{
#pragma HLS INLINE
	if (i == 0) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 1) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 2) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 3) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 4) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 5) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 6) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
	else if (i == 7) {
		for (U8 j = 0; j < 8; j++) { coef_buf_u_tmp[j] = coef_buf_u[(j + i) & 7][j]; }
	}
}
void updateModeChroma8_coefv_mux(int i, s16 coef_buf_v_tmp[8], s16 coef_buf_v[8][8])
{
#pragma HLS INLINE
	if (i == 0) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 1) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 2) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 3) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 4) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 5) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 6) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
	else if (i == 7) {
		for (U8 j = 0; j < 8; j++) { coef_buf_v_tmp[j] = coef_buf_v[(j + i) & 7][j]; }
	}
}


void updateBestModeChroma8_phase1(
	U2 cu_mode, MD_COM_MODE_BEST* mod_info_best, U4 bit_depth,
	int num_nz[12],
	int num_nz1,
	int num_nz2,
	int best_ipd,
	s16 rec_tmp_buf_u[8][8],
	s16 coef_buf_u[8][8],
	pel pred_buf_u[8][8],
	s16 rec_tmp_buf_v[8][8],
	s16 coef_buf_v[8][8],
	pel pred_buf_v[8][8],
	pel mod_info_best_rec_u[8][8],
	pel mod_info_best_rec_v[8][8],
	pel mod_info_best_coef_u[8][8],
	pel mod_info_best_coef_v[8][8]
) {
	int w = 8;
	int h = 8;
	int j, k;

	s16 coef_buf_u_tmp[8];
	s16 coef_buf_v_tmp[8];
	if (cu_mode == 0) {


		for (U8 i = 0; i < h; i++)
		{
#pragma HLS PIPELINE
			for (U8 j = 0; j < w; j++)
			{
				mod_info_best_rec_u[i][j] = rec_tmp_buf_u[i][j];

				mod_info_best_rec_v[i][j] = rec_tmp_buf_v[i][j];
			}
			updateModeChroma8_coefu_mux(i, coef_buf_u_tmp, coef_buf_u);
			updateModeChroma8_coefv_mux(i, coef_buf_v_tmp, coef_buf_v);
			for (U8 j = 0; j < w; j++)
			{
				mod_info_best_coef_u[i][j] = coef_buf_u_tmp[j];// coef_buf_u[(j + i) & 31][j];
				mod_info_best_coef_v[i][j] = coef_buf_v_tmp[j];// coef_buf_v[(j + i) & 31][j];
			}


		}

#if USE_TSCPM
		mod_info_best->ipm[0][1] = best_ipd;
		mod_info_best->num_nz[0][1] = num_nz1;
		mod_info_best->num_nz[0][2] = num_nz2;
#else
		mod_info_best->ipm[0][1] = best_ipd;
		mod_info_best->num_nz[0][1] = num_nz1;
		mod_info_best->num_nz[0][2] = num_nz2;
#endif
	}
	else {

		if (cu_mode == 2) {
			mod_info_best->num_nz[0][1] = 0;
			mod_info_best->num_nz[0][2] = 0;
		}
		else {
			mod_info_best->num_nz[0][1] = num_nz[best_ipd * 2];
			mod_info_best->num_nz[0][2] = num_nz[best_ipd * 2 + 1];
		}

		for (k = 0; k < h; ++k) {
#pragma HLS PIPELINE
			if (mod_info_best->num_nz[0][1] == 0) {
				for (j = 0; j < w; j++) {
					mod_info_best_coef_u[k][j] = 0;
					mod_info_best_rec_u[k][j] = (pel)((0 > pred_buf_u[k][j]) ? 0 : ((1 << bit_depth) - 1) < pred_buf_u[k][j] ? ((1 << bit_depth) - 1) : pred_buf_u[k][j]);
				}
			}
			else {
				for (j = 0; j < w; j++) {
					mod_info_best_rec_u[k][j] = (pel)((0 > rec_tmp_buf_u[k][j]) ? 0 : ((1 << bit_depth) - 1) < (rec_tmp_buf_u[k][j]) ? ((1 << bit_depth) - 1) : (rec_tmp_buf_u[k][j]));
				}
				updateModeChroma8_coefu_mux(k, coef_buf_u_tmp, coef_buf_u);
				for (j = 0; j < w; j++)
				{
					mod_info_best_coef_u[k][j] = coef_buf_u_tmp[j];// coef_buf_u[(k + j) & 31][j];
				}
			}
			if (mod_info_best->num_nz[0][2] == 0) {
				for (j = 0; j < w; j++) {
					mod_info_best_coef_v[k][j] = 0;
					mod_info_best_rec_v[k][j] = (((0 > pred_buf_v[k][j] ? 0 : (((1 << bit_depth) - 1 < (pred_buf_v[k][j]) ? (1 << bit_depth) - 1 : (pred_buf_v[k][j]))))));
				}
			}
			else {
				//mod_info_best->num_nz[0][i + 1] = mod_info_best_ptr->num_nz[0][i + 1];
				for (j = 0; j < w; j++) {
					mod_info_best_rec_v[k][j] = ((pel)((0 > rec_tmp_buf_v[k][j] ? 0 : (((1 << bit_depth) - 1 < (rec_tmp_buf_v[k][j]) ? (1 << bit_depth) - 1 : (rec_tmp_buf_v[k][j]))))));

				}
				updateModeChroma8_coefv_mux(k, coef_buf_v_tmp, coef_buf_v);
				for (j = 0; j < w; j++)
				{
					mod_info_best_coef_v[k][j] = coef_buf_v_tmp[j];
				}
			}
		}
	}

}

void updateBestModeChroma8(int cu_width_chroma, int cu_height_chroma, U2 cu_mode, MD_COM_MODE_BEST* mod_info_best,
	s16 rec_tmp[12][8][8], s16 coef_uv_pb_part[12][8][8], s64 cost[6],
	U4 bit_depth, pel pred[12][8][8], int num_nz[12],
	pel mod_info_best_rec_u[8][8],
	pel mod_info_best_rec_v[8][8],
	pel mod_info_best_coef_u[8][8],
	pel mod_info_best_coef_v[8][8]
)
{
#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->num_nz complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=3	
//#pragma HLS ARRAY_PARTITION variable=rec_tmp cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part complete dim=2		
//#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=cost complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz complete dim=0	
#pragma HLS ARRAY_PARTITION variable=pred complete dim=3	
//#pragma HLS ARRAY_PARTITION variable=pred cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_u complete  dim=2
#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_v complete  dim=2
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_v complete dim=2

	int best_ipd;
	int num_nz1;
	int num_nz2;
	s16 rec_tmp_buf_u[8][8];
	s16 coef_buf_u[8][8];
	pel pred_buf_u[8][8];
	s16 rec_tmp_buf_v[8][8];
	s16 coef_buf_v[8][8];
	pel pred_buf_v[8][8];
#pragma HLS ARRAY_PARTITION variable=rec_tmp_buf_u complete  dim=2
#pragma HLS ARRAY_PARTITION variable=rec_tmp_buf_v complete  dim=2
#pragma HLS ARRAY_PARTITION variable=pred_buf_u complete  dim=2
#pragma HLS ARRAY_PARTITION variable=pred_buf_v complete  dim=2
#pragma HLS ARRAY_PARTITION variable=coef_buf_u complete  dim=1
#pragma HLS ARRAY_PARTITION variable=coef_buf_v complete  dim=1

	updateBestModeChroma8_phase(num_nz,
		cost, rec_tmp, coef_uv_pb_part, pred,
		&num_nz1,
		&num_nz2,
		rec_tmp_buf_u,
		coef_buf_u,
		pred_buf_u,
		rec_tmp_buf_v,
		coef_buf_v,
		pred_buf_v,
		&best_ipd
	);
	updateBestModeChroma8_phase1(
		cu_mode, mod_info_best, bit_depth,
		num_nz,
		num_nz1,
		num_nz2,
		best_ipd,
		rec_tmp_buf_u,
		coef_buf_u,
		pred_buf_u,
		rec_tmp_buf_v,
		coef_buf_v,
		pred_buf_v,
		mod_info_best_rec_u,
		mod_info_best_rec_v,
		mod_info_best_coef_u,
		mod_info_best_coef_v
	);
}

void reconChroma8(U8 w, U8 h, U4 bit_depth, int num_nz, pel pred_cache[8][8], pel rec[8][8],   pel resi[8][8],int *num_nz_out, int rdo_idx, int all_rdo_mode)
{
	if (rdo_idx < all_rdo_mode) {
		recon_chroma8(resi, pred_cache, num_nz, w, h, rec, bit_depth);
		*num_nz_out = num_nz;
	}
}

#if 0
void updateModeInfoBestChroma32(ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_COM_MODE_BEST* mod_info_best, s16 coef_uv_pb_part[2][64],
	U1 coef_rec_write_flag[1], pel pred[2][8][8], s16 rec_tmp[2][8][8], U4 bit_depth)
{

	if (mod_info_best->cu_mode != 0)
	{
		for (int uv = 0; uv < 2; uv++) {
			update_chroma_8(cu_data_temp_ptr, mod_info_best, coef_uv_pb_part[uv], uv, coef_rec_write_flag, mod_info_best->cu_mode, pred, rec_tmp, mod_info_best->num_nz, bit_depth);
		}

	}
}
#endif

void dct_row_Chroma8(U8 w, U8 h, s16 src[8][8], s16 dst[8][8], U4 shift)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=src complete dim=2
#pragma HLS ARRAY_PARTITION variable=dst complete dim=2

	U8 size = 8;
	U8 size_b = 7;

	U8 j, k;
	int E[4], O[4];
	int EE[2], EO[2];
	int tmp[8];
#pragma HLS ARRAY_PARTITION variable=O complete dim=1
#pragma HLS ARRAY_PARTITION variable=E complete dim=1
#pragma HLS ARRAY_PARTITION variable=EE complete dim=1
#pragma HLS ARRAY_PARTITION variable=EO complete dim=1

#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1

	int add;
	if (w == 4)
		add = shift == 0 ? 0 : 1 << (shift - 1);
	else if (w == 8)
		add = 1 << (shift - 1);

	for (j = 0; j < 8; j++)
	{
#pragma HLS PIPELINE ii=1
		if (j < h) {


		/* E and O*/
		for (k = 0; k < 4; k++)
		{
			E[k] = src[j][k] + src[j][7 - k];
			O[k] = src[j][k] - src[j][7 - k];
		}
		/* EE and EO */
		if (w == 8) {
			EE[0] = E[0] + E[3];
			EO[0] = E[0] - E[3];
			EE[1] = E[1] + E[2];
			EO[1] = E[1] - E[2];
		}
		else if (w == 4)
		{
			EE[0] = src[j][0] + src[j][3];
			EO[0] = src[j][0] - src[j][3];
			EE[1] = src[j][1] + src[j][2];
			EO[1] = src[j][1] - src[j][2];
		}
		tmp[0] = ((32 * EE[0] + 32 * EE[1] + add) >> shift);
		tmp[1] = ((32 * EE[0] - 32 * EE[1] + add) >> shift);
		tmp[2] = ((42 * EO[0] + 17 * EO[1] + add) >> shift);
		tmp[3] = ((17 * EO[0] - 42 * EO[1] + add) >> shift);
		tmp[4] = ((44 * O[0] + 38 * O[1] + 25 * O[2] + 9 * O[3] + add) >> shift);
		tmp[5] = ((38 * O[0] - 9 * O[1] - 44 * O[2] - 25 * O[3] + add) >> shift);
		tmp[6] = ((25 * O[0] - 44 * O[1] + 9 * O[2] + 38 * O[3] + add) >> shift);
		tmp[7] = ((9 * O[0] - 25 * O[1] + 38 * O[2] - 44 * O[3] + add) >> shift);

		if (w == 4) {
/*				if (j == 0) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 1) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 2) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 3) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 4) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 5) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 6) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
				else if (j == 7) { dst[0][(j + 0) & size_b] = tmp[0]; dst[2][(j + 2) & size_b] = tmp[1]; dst[1][(j + 1) & size_b] = tmp[2]; dst[3][(j + 3) & size_b] = tmp[3]; }
*/
		    dst[0][(j) & size_b] = tmp[0];
			 dst[2][(j + 2) & size_b] = tmp[1];
			 dst[1][(j + 1) & size_b] = tmp[2];
			 dst[3][(j + 3) & size_b] = tmp[3];


		}
		else if (w == 8) {
			dst[0][(j ) & size_b] = tmp[0];
			dst[4][(j + 4) & size_b] = tmp[1];
			dst[2][(j + 2) & size_b] = tmp[2];
			dst[6][(j + 6) & size_b] = tmp[3];
			dst[1][(j + 1) & size_b] = tmp[4];
			dst[3][(j + 3) & size_b] = tmp[5];
			dst[5][(j + 5) & size_b] = tmp[6];
			dst[7][(j + 7) & size_b] = tmp[7];
			/*	if (j == 0) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 1) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 2) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 3) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 4) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 5) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 6) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}
				else if (j == 7) {
					dst[0][(j + 0) & size_b] = tmp[0]; dst[4][(j + 4) & size_b] = tmp[1]; dst[2][(j + 2) & size_b] = tmp[2]; dst[6][(j + 6) & size_b] = tmp[3]; dst[1][(j + 1) & size_b] = tmp[4]; dst[3][(j + 3) & size_b] = tmp[5]; dst[5][(j + 5) & size_b] = tmp[6]; dst[7][(j + 7) & size_b] = tmp[7];
				}*/
			}
		}
	}
}
void transform_hor_Chroma8(U7 cu_width_c, U7 cu_height_c, U3 tb_width_log2_c,  U4 bit_depth, pel diff[8][8], pel coef_h[8][8], int rdo_idx, int all_rdo_mode)
{

	if (rdo_idx < all_rdo_mode) {
	U4 shift1;
	shift1 = com_get_forward_trans_shift(tb_width_log2_c, 0, bit_depth);
	dct_row_Chroma8(cu_width_c, cu_height_c, diff, coef_h, shift1);
	}
	//return 1;
	}

void dct_col_Chroma8(U8 w, U8 h, s16 src[8][8], s16 dst[8][8], U4 shift)
{
#pragma HLS INLINE
#pragma HLS ARRAY_PARTITION variable=src complete dim=2
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1

	U8 size = 8;
	U8 size_b = 7;

	U8 j, k;
	static int E[4], O[4];
	static int EE[2], EO[2];
	int tmp[8];
#pragma HLS ARRAY_PARTITION variable=E complete dim=1
#pragma HLS ARRAY_PARTITION variable=O complete dim=1
#pragma HLS ARRAY_PARTITION variable=EE complete dim=1
#pragma HLS ARRAY_PARTITION variable=EO complete dim=1
#pragma HLS ARRAY_PARTITION variable=tmp complete dim=1
	int add;

	if (h == 4)
		add = shift == 0 ? 0 : 1 << (shift - 1);
	else if (h == 8)
		add = 1 << (shift - 1);
	for (j = 0; j < 8; j++)
	{
#pragma HLS PIPELINE
		if (j < w) {


		/* E and O*/
		for (k = 0; k < 4; k++)
		{
			E[k] = src[j][(k + j) & size_b] + src[j][(7 - k + j) & size_b];
			O[k] = src[j][(k + j) & size_b] - src[j][(7 - k + j) & size_b];
		}
		/* EE and EO */
		if (h == 8) {
			EE[0] = E[0] + E[3];
			EO[0] = E[0] - E[3];
			EE[1] = E[1] + E[2];
			EO[1] = E[1] - E[2];
		}
		else if (h == 4) {
			EE[0] = src[j][(0 + j) & size_b] + src[j][(3 + j) & size_b];
			EO[0] = src[j][(0 + j) & size_b] - src[j][(3 + j) & size_b];
			EE[1] = src[j][(1 + j) & size_b] + src[j][(2 + j) & size_b];
			EO[1] = src[j][(1 + j) & size_b] - src[j][(2 + j) & size_b];
		}
		tmp[0] = ((32 * EE[0] + 32 * EE[1] + add) >> shift);
		tmp[1] = ((32 * EE[0] - 32 * EE[1] + add) >> shift);
		tmp[2] = ((42 * EO[0] + 17 * EO[1] + add) >> shift);
		tmp[3] = ((17 * EO[0] - 42 * EO[1] + add) >> shift);
		tmp[4] = ((44 * O[0] + 38 * O[1] + 25 * O[2] + 9 * O[3] + add) >> shift);
		tmp[5] = ((38 * O[0] - 9 * O[1] - 44 * O[2] - 25 * O[3] + add) >> shift);
		tmp[6] = ((25 * O[0] - 44 * O[1] + 9 * O[2] + 38 * O[3] + add) >> shift);
		tmp[7] = ((9 * O[0] - 25 * O[1] + 38 * O[2] - 44 * O[3] + add) >> shift);
		dst[0][j] = tmp[0];
		if (h == 4) {
/*				if (j == 0) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 1) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 2) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 3) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 4) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 5) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 6) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }
				else if (j == 7) { dst[0][j] = tmp[0]; dst[2][j] = tmp[1]; dst[1][j] = tmp[2]; dst[3][j] = tmp[3]; }*/
		//	dst[0][j] = tmp[0];
			dst[2][j] = tmp[1];
			dst[1][j] = tmp[2];
			dst[3][j] = tmp[3];

		}
		else if (h == 8) {
		//	dst[0][j] = tmp[0];
			dst[4][j] = tmp[1];
			dst[2][j] = tmp[2];
			dst[6][j] = tmp[3];
			dst[1][j] = tmp[4];
			dst[3][j] = tmp[5];
			dst[5][j] = tmp[6];
         dst[7][j] = tmp[7];
			/*	if (j == 0) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 1) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 2) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 3) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 4) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 5) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 6) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
				else if (j == 7) { dst[0][j] = tmp[0]; dst[4][j] = tmp[1]; dst[2][j] = tmp[2]; dst[6][j] = tmp[3]; dst[1][j] = tmp[4]; dst[3][j] = tmp[5]; dst[5][j] = tmp[6]; dst[7][j] = tmp[7]; }
*/
			}

		}
	}
}

void transform_ver_Chroma8(U8 w, U8 h, U3 cu_height_log2_c, U4 bit_depth, pel coef_h[8][8], pel coef[8][8], int rdo_idx, int all_rdo_mode)
{
	if (rdo_idx < all_rdo_mode) {
	U4 shift2;
	shift2 = com_get_forward_trans_shift(cu_height_log2_c, 1, bit_depth);
	dct_col_Chroma8(w, h, coef_h, coef, shift2);
	}
	//return 1;
}
void rdo_chroma_dataflow_8_x(int all_rdo_mode, MD_COM_MODE_BEST* mod_info_best, U7 cu_width_c, U7 cu_height_c, U3 cu_width_log2_c, U3 cu_height_log2_c,
	pel orgU[2][8][8],pel pred_0[12][8][8], pel pred_1[12][8][8],  U4 bit_depth,
	int num_nz_out[12], RDOQ_ARRAY* rdoq_array, MD_INPUT* md_input_ptr,  MD_FW* md_fw_ptr, s16 coef_uv_pb_part[12][8][8], pel rec_tmp[12][8][8]
)
{
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_run complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_last complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->lambda complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->dist_chroma_weight complete dim=0
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=3
#pragma HLS ARRAY_PARTITION variable=pred_1 complete dim=3	
#pragma HLS ARRAY_PARTITION variable=pred_0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=3		
#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part complete dim=2
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1
#pragma HLS RESOURCE variable=orgU core=RAM_1P
#pragma HLS RESOURCE variable=pred_1 core=RAM_1P
#pragma HLS RESOURCE variable=pred_0 core=RAM_1P
#pragma HLS RESOURCE variable=rec_tmp core=RAM_1P
#pragma HLS RESOURCE variable=coef_uv_pb_part core=RAM_1P
	U1 uv;
	//U7 cu_width_c = cu_width >> 1;
	//U7 cu_height_c = cu_height >> 1;
	//U3 cu_width_log2_c = cu_width_log2 - 1;
	//U3 cu_height_log2_c = cu_height_log2 - 1;

	//int num_nz;
	for (int rdo_idx = 0; rdo_idx < 12; rdo_idx++)
	{
#pragma HLS DATAFLOW
//#pragma HLS LOOP_TRIPCOUNT min=12 max=12 avg=12
		uv = rdo_idx & 1;
		s16 coef_t_h[8][8];
		s16 coef_t_v[8][8];
		s16 coef_q[8][8];
		s16 coef_dq[8][8];
		s16 coef_it_v[8][8];
		pel resi[8][8];
		s16 diff[8][8];
		S16 tmp_dst_coef[8][8];
		int num_nz_coef;
		int num_nz0, num_nz1, num_nz2;
		int final_x;
		int final_y;
		//if (mod_info_best->cu_mode == 0) mod_info_best->ipm[0][1] = rdo_idx / 2;
#pragma HLS RESOURCE variable=diff core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_t_h core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_t_v core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_q core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_dq core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_it_v core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=resi core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=tmp_dst_coef core=RAM_S2P_BRAM

		calcDiffChroma8( cu_width_c, cu_height_c, orgU[uv], pred_0[rdo_idx], diff,  rdo_idx, all_rdo_mode);
		transform_hor_Chroma8( cu_width_c,  cu_height_c, cu_width_log2_c,  bit_depth, diff, coef_t_h, rdo_idx, all_rdo_mode);
		transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h, coef_t_v, rdo_idx, all_rdo_mode);

		rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v, md_input_ptr->qp_u, tmp_dst_coef,
			rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level,
			rdoq_array->rdoq_est_last, uv + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x, &final_y, rdo_idx, all_rdo_mode);
		CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef, coef_q, tmp_dst_coef, final_x, final_y,  coef_uv_pb_part[rdo_idx], rdo_idx, all_rdo_mode);

		dquantChroma8(cu_width_c, cu_height_c,  cu_width_log2_c,  cu_height_log2_c, bit_depth, md_input_ptr->qp_u, 
			&num_nz0, coef_q, coef_dq,  num_nz_coef, rdo_idx, all_rdo_mode);

		invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c,bit_depth, num_nz0, &num_nz1, coef_dq, coef_it_v, rdo_idx, all_rdo_mode);

		invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1, &num_nz2, cu_height_log2_c, coef_it_v,  resi, rdo_idx, all_rdo_mode);

		reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2,  pred_1[rdo_idx], rec_tmp[rdo_idx],  resi, &num_nz_out[rdo_idx], rdo_idx, all_rdo_mode);
			}
		}
void rdo_chroma_dataflow_8(int all_rdo_mode, MD_COM_MODE_BEST* mod_info_best, U7 cu_width_c, U7 cu_height_c, U3 cu_width_log2_c, U3 cu_height_log2_c,
	pel orgU[2][8][8], pel pred_0[12][8][8], pel pred_1[12][8][8], U4 bit_depth,
	int num_nz_out[12], RDOQ_ARRAY* rdoq_array, MD_INPUT* md_input_ptr, MD_FW* md_fw_ptr, s16 coef_uv_pb_part[12][8][8], pel rec_tmp[12][8][8]
)
{
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_cbf complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_run complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_level complete dim=0
#pragma HLS ARRAY_PARTITION variable=rdoq_array->rdoq_est_last complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->lambda complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
#pragma HLS ARRAY_PARTITION variable=md_input_ptr->dist_chroma_weight complete dim=0


#pragma HLS ARRAY_PARTITION variable=pred_1 complete dim=3
#pragma HLS ARRAY_PARTITION variable=pred_0 complete dim=3	
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=3
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=3		
#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part complete dim=2		
#pragma HLS ARRAY_PARTITION variable=num_nz_out complete dim=1
#pragma HLS RESOURCE variable=orgU core=RAM_1P
#pragma HLS RESOURCE variable=pred_1 core=RAM_1P
#pragma HLS RESOURCE variable=pred_0 core=RAM_1P
#pragma HLS RESOURCE variable=rec_tmp core=RAM_1P
#pragma HLS RESOURCE variable=coef_uv_pb_part core=RAM_1P
	s16 coef_t_h[2][8][8];
	s16 coef_t_v[2][8][8];
	s16 coef_q[2][8][8];
	s16 coef_dq[2][8][8];
	s16 coef_it_v[2][8][8];
	pel resi[2][8][8];
	s16 diff[2][8][8];
	S16 tmp_dst_coef[2][8][8];
	int num_nz_coef[2];
	int num_nz0[2], num_nz1[2], num_nz2[2];
	int final_x[2];
	int final_y[2];
#pragma HLS ARRAY_PARTITION variable=num_nz_coef complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz0 complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=num_nz2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=final_x complete dim=0
#pragma HLS ARRAY_PARTITION variable=final_y complete dim=0
#pragma HLS RESOURCE variable=diff core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_t_h core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_t_v core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_q core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_dq core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=coef_it_v core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=resi core=RAM_S2P_BRAM
#pragma HLS RESOURCE variable=tmp_dst_coef core=RAM_S2P_BRAM
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[0], pred_0[0], diff[0], 0, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[0], coef_t_h[0], 0, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[1], pred_0[1], diff[1], 1, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[0], coef_t_v[0], 0, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[1], coef_t_h[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[0], pred_0[2], diff[0], 2, all_rdo_mode);
#endif //!SIMPLE_MD
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[0], md_input_ptr->qp_u, tmp_dst_coef[0], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 0 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[0], &final_y[0], 0, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[1], coef_t_v[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[0], coef_t_h[0], 2, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[1], pred_0[3], diff[1], 3, all_rdo_mode);
#endif //!SIMPLE_MD
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[0], coef_q[0], tmp_dst_coef[0], final_x[0], final_y[0], coef_uv_pb_part[0], 0, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[1], md_input_ptr->qp_u, tmp_dst_coef[1], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 1 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[1], &final_y[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[0], coef_t_v[0], 2, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[1], coef_t_h[1], 3, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[0], pred_0[4], diff[0], 4, all_rdo_mode);
#endif //!SIMPLE_MD
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[0], coef_q[0], coef_dq[0], num_nz_coef[0], 0, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[1], coef_q[1], tmp_dst_coef[1], final_x[1], final_y[1], coef_uv_pb_part[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[0], md_input_ptr->qp_u, tmp_dst_coef[0], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 0 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[0], &final_y[0], 2, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[1], coef_t_v[1], 3, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[0], coef_t_h[0], 4, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[1], pred_0[5], diff[1], 5, all_rdo_mode);
#endif //!SIMPLE_MD

	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[0], &num_nz1[0], coef_dq[0], coef_it_v[0], 0, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[1], coef_q[1], coef_dq[1], num_nz_coef[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[0], coef_q[0], tmp_dst_coef[0], final_x[0], final_y[0], coef_uv_pb_part[2], 2, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[1], md_input_ptr->qp_u, tmp_dst_coef[1], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 1 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[1], &final_y[1], 3, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[0], coef_t_v[0], 4, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[1], coef_t_h[1], 5, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[0], pred_0[6], diff[0], 6, all_rdo_mode);
#endif //!SIMPLE_MD

	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[0], &num_nz2[0], cu_height_log2_c, coef_it_v[0], resi[0], 0, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[1], &num_nz1[1], coef_dq[1], coef_it_v[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[0], coef_q[0], coef_dq[0], num_nz_coef[0], 2, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[1], coef_q[1], tmp_dst_coef[1], final_x[1], final_y[1], coef_uv_pb_part[3], 3, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[0], md_input_ptr->qp_u, tmp_dst_coef[0], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 0 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[0], &final_y[0], 4, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[1], coef_t_v[1], 5, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[0], coef_t_h[0], 6, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[1], pred_0[7], diff[1], 7, all_rdo_mode);
#endif //!SIMPLE_MD

	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[0], pred_1[0], rec_tmp[0], resi[0], &num_nz_out[0], 0, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[1], &num_nz2[1], cu_height_log2_c, coef_it_v[1], resi[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[0], &num_nz1[0], coef_dq[0], coef_it_v[0], 2, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[1], coef_q[1], coef_dq[1], num_nz_coef[1], 3, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[0], coef_q[0], tmp_dst_coef[0], final_x[0], final_y[0], coef_uv_pb_part[4], 4, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[1], md_input_ptr->qp_u, tmp_dst_coef[1], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 1 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[1], &final_y[1], 5, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[0], coef_t_v[0], 6, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[1], coef_t_h[1], 7, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[0], pred_0[8], diff[0], 8, all_rdo_mode);
#endif //!SIMPLE_MD

	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[1], pred_1[1], rec_tmp[1], resi[1], &num_nz_out[1], 1, all_rdo_mode);
#if !SIMPLE_MD
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[0], &num_nz2[0], cu_height_log2_c, coef_it_v[0], resi[0], 2, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[1], &num_nz1[1], coef_dq[1], coef_it_v[1], 3, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[0], coef_q[0], coef_dq[0], num_nz_coef[0], 4, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[1], coef_q[1], tmp_dst_coef[1], final_x[1], final_y[1], coef_uv_pb_part[5], 5, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[0], md_input_ptr->qp_u, tmp_dst_coef[0], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 0 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[0], &final_y[0], 6, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[1], coef_t_v[1], 7, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[0], coef_t_h[0], 8, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[1], pred_0[9], diff[1], 9, all_rdo_mode);


	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[0], pred_1[2], rec_tmp[2], resi[0], &num_nz_out[2], 2, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[1], &num_nz2[1], cu_height_log2_c, coef_it_v[1], resi[1], 3, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[0], &num_nz1[0], coef_dq[0], coef_it_v[0], 4, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[1], coef_q[1], coef_dq[1], num_nz_coef[1], 5, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[0], coef_q[0], tmp_dst_coef[0], final_x[0], final_y[0], coef_uv_pb_part[6], 6, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[1], md_input_ptr->qp_u, tmp_dst_coef[1], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 1 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[1], &final_y[1], 7, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[0], coef_t_v[0], 8, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[1], coef_t_h[1], 9, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[0], pred_0[10], diff[0], 10, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[1], pred_1[3], rec_tmp[3], resi[1], &num_nz_out[3], 3, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[0], &num_nz2[0], cu_height_log2_c, coef_it_v[0], resi[0], 4, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[1], &num_nz1[1], coef_dq[1], coef_it_v[1], 5, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[0], coef_q[0], coef_dq[0], num_nz_coef[0], 6, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[1], coef_q[1], tmp_dst_coef[1], final_x[1], final_y[1], coef_uv_pb_part[7], 7, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[0], md_input_ptr->qp_u, tmp_dst_coef[0], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 0 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[0], &final_y[0], 8, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[1], coef_t_v[1], 9, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[0], coef_t_h[0], 10, all_rdo_mode);
	calcDiffChroma8(cu_width_c, cu_height_c, orgU[1], pred_0[11], diff[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[0], pred_1[4], rec_tmp[4], resi[0], &num_nz_out[4], 4, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[1], &num_nz2[1], cu_height_log2_c, coef_it_v[1], resi[1], 5, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[0], &num_nz1[0], coef_dq[0], coef_it_v[0], 6, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[1], coef_q[1], coef_dq[1], num_nz_coef[1], 7, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[0], coef_q[0], tmp_dst_coef[0], final_x[0], final_y[0], coef_uv_pb_part[8], 8, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[1], md_input_ptr->qp_u, tmp_dst_coef[1], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 1 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[1], &final_y[1], 9, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[0], coef_t_v[0], 10, all_rdo_mode);
	transform_hor_Chroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, diff[1], coef_t_h[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[1], pred_1[5], rec_tmp[5], resi[1], &num_nz_out[5], 5, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[0], &num_nz2[0], cu_height_log2_c, coef_it_v[0], resi[0], 6, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[1], &num_nz1[1], coef_dq[1], coef_it_v[1], 7, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[0], coef_q[0], coef_dq[0], num_nz_coef[0], 8, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[1], coef_q[1], tmp_dst_coef[1], final_x[1], final_y[1], coef_uv_pb_part[9], 9, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[0], md_input_ptr->qp_u, tmp_dst_coef[0], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 0 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[0], &final_y[0], 10, all_rdo_mode);
	transform_ver_Chroma8(cu_width_c, cu_height_c, cu_height_log2_c, bit_depth, coef_t_h[1], coef_t_v[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[0], pred_1[6], rec_tmp[6], resi[0], &num_nz_out[6], 6, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[1], &num_nz2[1], cu_height_log2_c, coef_it_v[1], resi[1], 7, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[0], &num_nz1[0], coef_dq[0], coef_it_v[0], 8, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[1], coef_q[1], coef_dq[1], num_nz_coef[1], 9, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[0], coef_q[0], tmp_dst_coef[0], final_x[0], final_y[0], coef_uv_pb_part[10], 10, all_rdo_mode);
	rdoq_all_hardware_chroma8(cu_width_log2_c, cu_height_log2_c, coef_t_v[1], md_input_ptr->qp_u, tmp_dst_coef[1], rdoq_array->rdoq_est_cbf, rdoq_array->rdoq_est_run, rdoq_array->rdoq_est_level, rdoq_array->rdoq_est_last, 1 + 1, 1, md_input_ptr->lambda[1], bit_depth, &final_x[1], &final_y[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[1], pred_1[7], rec_tmp[7], resi[1], &num_nz_out[7], 7, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[0], &num_nz2[0], cu_height_log2_c, coef_it_v[0], resi[0], 8, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[1], &num_nz1[1], coef_dq[1], coef_it_v[1], 9, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[0], coef_q[0], coef_dq[0], num_nz_coef[0], 10, all_rdo_mode);
	CleanNz_8_all(cu_width_c, cu_height_c, &num_nz_coef[1], coef_q[1], tmp_dst_coef[1], final_x[1], final_y[1], coef_uv_pb_part[11], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[0], pred_1[8], rec_tmp[8], resi[0], &num_nz_out[8], 8, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[1], &num_nz2[1], cu_height_log2_c, coef_it_v[1], resi[1], 9, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[0], &num_nz1[0], coef_dq[0], coef_it_v[0], 10, all_rdo_mode);
	dquantChroma8(cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, bit_depth, md_input_ptr->qp_u, &num_nz0[1], coef_q[1], coef_dq[1], num_nz_coef[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[1], pred_1[9], rec_tmp[9], resi[1], &num_nz_out[9], 9, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[0], &num_nz2[0], cu_height_log2_c, coef_it_v[0], resi[0], 10, all_rdo_mode);
	invTransVerChroma8(cu_width_c, cu_height_c, cu_width_log2_c, bit_depth, num_nz0[1], &num_nz1[1], coef_dq[1], coef_it_v[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[0], pred_1[10], rec_tmp[10], resi[0], &num_nz_out[10], 10, all_rdo_mode);
	invTransHorChroma8(cu_width_c, cu_height_c, bit_depth, num_nz1[1], &num_nz2[1], cu_height_log2_c, coef_it_v[1], resi[1], 11, all_rdo_mode);
	reconChroma8(cu_width_c, cu_height_c, bit_depth, num_nz2[1], pred_1[11], rec_tmp[11], resi[1], &num_nz_out[11], 11, all_rdo_mode);
#endif //!SIMPLE_MD
}

void rdo_chroma_8_pipeline_pred(MD_COM_MODE_BEST* mod_info_best, MD_FW* md_fw_ptr, strFetch_ref_window* Fetch_Ref_window_ptr,

	U4 bit_depth, pel pred_0[12][8][8],pel pred_1[12][8][8], U13 pb_x, U13 pb_y, int all_rdo_mode, pel piRecoY[16][16]

#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp,
#endif
	U7 cu_width_c ,U7 cu_height_c ,U3 cu_width_log2_c ,U3 cu_height_log2_c ,
	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8], pel IP_buffer_ptr_U_8x8_left_line[2 * 8], pel IP_buffer_ptr_U_8x8_top_left,
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8], pel IP_buffer_ptr_V_8x8_left_line[2 * 8], pel IP_buffer_ptr_V_8x8_top_left,
	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4], pel IP_buffer_ptr_U_4x8_left_line[2 * 8], pel IP_buffer_ptr_U_4x8_top_left,
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4], pel IP_buffer_ptr_V_4x8_left_line[2 * 8], pel IP_buffer_ptr_V_4x8_top_left,
	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8], pel IP_buffer_ptr_U_8x4_left_line[2 * 4], pel IP_buffer_ptr_U_8x4_top_left,
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8], pel IP_buffer_ptr_V_8x4_left_line[2 * 4], pel IP_buffer_ptr_V_8x4_top_left,
	pel	IP_buffer_ptr_8x16_up_line[2 * 8], pel IP_buffer_ptr_8x16_left_line[2 * 16], pel IP_buffer_ptr_8x16_top_left,
	pel	IP_buffer_ptr_16x8_up_line[2 * 16], pel IP_buffer_ptr_16x8_left_line[2 * 8], pel IP_buffer_ptr_16x8_top_left,
	pel	IP_buffer_ptr_16x16_up_line[2 * 16], pel IP_buffer_ptr_16x16_left_line[2 * 16], pel IP_buffer_ptr_16x16_top_left,
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new
)
{

#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->num_nz complete dim=0
//#pragma HLS ARRAY_PARTITION variable=pred_0 cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=pred_0 complete dim=3
//#pragma HLS ARRAY_PARTITION variable=pred_1 cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=pred_1 complete dim=3
	pel nb[2][2][8 * 2 + 3];
#if USE_TSCPM
	pel nb_y[2][16 * 2 + 3];
#pragma HLS ARRAY_PARTITION variable=nb_y complete dim=0
#endif

#pragma HLS ARRAY_PARTITION variable=nb complete dim=0



	for (int i = 0; i < 2 * 8 + 3; i++) {
#pragma HLS UNROLL
		nb[1][0][i] = (1 << bit_depth - 1);
		nb[1][1][i] = (1 << bit_depth - 1);
		nb[0][0][i] = (1 << bit_depth - 1);
		nb[0][1][i] = (1 << bit_depth - 1);
	}

	for (int i = 0; i < 2 * 16 + 3; i++) {
#pragma HLS UNROLL
		nb_y[0][i] = (1 << bit_depth - 1);
		nb_y[1][i] = (1 << bit_depth - 1);
	}


	u16 avail_cu_output ;
#if !SIMPLE_MD
	if (mod_info_best->cu_mode != 0)
	{
		com_mc_cu_uv_8(mod_info_best, md_fw_ptr->pic_width, md_fw_ptr->pic_height, mod_info_best->refi, mod_info_best->mv,
			SWW >> 1, pred_0[0], pred_0[1], pred_1[0], pred_1[1], bit_depth, Fetch_Ref_window_ptr
#if SUB_TMVP
			, core_sbtmvp, mod_info_best->skip_idx, mod_info_best->cu_mode
#endif
		);
	}
	else 
#endif
	{

		ipred_c_8_prepare(md_fw_ptr, cu_width_log2_c, cu_height_log2_c, pb_x, pb_y,
			mod_info_best, nb_y, nb[0], nb[1],

			mod_info_best->ipm[0][1],
			&avail_cu_output,
			IP_buffer_ptr_U_8x8_up_line, IP_buffer_ptr_U_8x8_left_line, IP_buffer_ptr_U_8x8_top_left,
			IP_buffer_ptr_V_8x8_up_line, IP_buffer_ptr_V_8x8_left_line, IP_buffer_ptr_V_8x8_top_left,
			IP_buffer_ptr_U_4x8_up_line, IP_buffer_ptr_U_4x8_left_line, IP_buffer_ptr_U_4x8_top_left,
			IP_buffer_ptr_V_4x8_up_line, IP_buffer_ptr_V_4x8_left_line, IP_buffer_ptr_V_4x8_top_left,
			IP_buffer_ptr_U_8x4_up_line, IP_buffer_ptr_U_8x4_left_line, IP_buffer_ptr_U_8x4_top_left,
			IP_buffer_ptr_V_8x4_up_line, IP_buffer_ptr_V_8x4_left_line, IP_buffer_ptr_V_8x4_top_left,
			IP_buffer_ptr_8x16_up_line, IP_buffer_ptr_8x16_left_line,  IP_buffer_ptr_8x16_top_left,
			IP_buffer_ptr_16x8_up_line, IP_buffer_ptr_16x8_left_line,  IP_buffer_ptr_16x8_top_left,
			IP_buffer_ptr_16x16_up_line, IP_buffer_ptr_16x16_left_line,  IP_buffer_ptr_16x16_top_left,

			IP_map_scu_ptr_8x16_map_scu_up_line,
			IP_map_scu_ptr_8x16_map_scu_left_line_new,
			IP_map_scu_ptr_8x16_map_scu_top_left_new,

			IP_map_scu_ptr_16x8_map_scu_up_line,
			IP_map_scu_ptr_16x8_map_scu_left_line_new,
			IP_map_scu_ptr_16x8_map_scu_top_left_new,
			IP_map_scu_ptr_16x16_map_scu_up_line,
			IP_map_scu_ptr_16x16_map_scu_left_line_new,
			IP_map_scu_ptr_16x16_map_scu_top_left_new
			);
		for (int rdo_idx = 0; rdo_idx < all_rdo_mode; rdo_idx++)
		{
#if SIMPLE_MD
#pragma HLS LOOP_TRIPCOUNT min=2 max=2 avg=2
#else
#pragma HLS LOOP_TRIPCOUNT min=12 max=12 avg=12
#endif //SIMPLE_MD
			com_ipred_uv_8(cu_width_log2_c, cu_height_log2_c,nb[(rdo_idx & 1)][0], nb[(rdo_idx & 1)][1], pred_0[rdo_idx], rdo_idx / 2, mod_info_best->ipm[0][0],
				cu_width_c, cu_height_c, md_fw_ptr->bit_depth_internal, avail_cu_output,
				pred_1[rdo_idx],piRecoY, nb_y
#if MIPF
				, md_fw_ptr->mipf_enable_flag
#endif
			);
		}
	}

}
void rdo_chroma_8_pipeline_updata(int all_rdo_mode,  MD_COM_MODE_BEST* mod_info_best,U7 cu_width_c, U7 cu_height_c, 
	pel orgU[2][8][8], pel pred_0[12][8][8], MD_INPUT* md_input_ptr, U7 cu_width, U7 cu_height, U4 bit_depth,
	s16 coef_uv_pb_part[12][8][8],pel rec_tmp[12][8][8],int num_nz[12],
	pel mod_info_best_rec_u[8][8],
	pel mod_info_best_rec_v[8][8],
	pel mod_info_best_coef_u[8][8],
	pel mod_info_best_coef_v[8][8]
) 
{
#pragma HLS ARRAY_PARTITION variable=rec_tmp complete dim=3	
//#pragma HLS ARRAY_PARTITION variable=rec_tmp cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part complete dim=2	
//#pragma HLS ARRAY_PARTITION variable=coef_uv_pb_part cyclic factor = 2 dim=1	

#pragma HLS ARRAY_PARTITION variable=num_nz complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->ipm complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best->num_nz complete dim=0

//#pragma HLS ARRAY_PARTITION variable=pred_0 cyclic factor = 2 dim=1	
#pragma HLS ARRAY_PARTITION variable=pred_0 complete dim=3

#pragma HLS ARRAY_PARTITION variable=orgU complete dim=1	
#pragma HLS ARRAY_PARTITION variable=orgU complete dim=3

	s64 cost[6];
#pragma HLS ARRAY_PARTITION variable=cost complete dim=0
	for (int i = 0; i < 6; i++)
	{
#pragma HLS UNROLL
		cost[i] = 9223372036854775807;
	}

	U1 uv;
	for (int rdo_idx = 0; rdo_idx < all_rdo_mode; rdo_idx++)
	{
#if SIMPLE_MD
#pragma HLS LOOP_TRIPCOUNT min=2 max=2 avg=2
#else
#pragma HLS LOOP_TRIPCOUNT min=12 max=12 avg=12
#endif //SIMPLE_MD
		uv = rdo_idx & 1;

		deriveRDCostChroma8(mod_info_best->cu_mode, md_input_ptr->dist_chroma_weight[uv], mod_info_best,  cu_width, cu_height, 
			rec_tmp[rdo_idx], orgU[uv], md_input_ptr->lambda[uv],
			coef_uv_pb_part[rdo_idx], 1, num_nz, cost, bit_depth, uv, rdo_idx, cu_width_c, cu_height_c);

	}

	updateBestModeChroma8(cu_width_c, cu_height_c, mod_info_best->cu_mode, mod_info_best,
		rec_tmp, coef_uv_pb_part, cost, bit_depth,
		pred_0, num_nz,
		mod_info_best_rec_u,
		mod_info_best_rec_v,
		mod_info_best_coef_u,
		mod_info_best_coef_v);
}

#if !CHROMA_8_PIPELINE_DUMMY
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
	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8], pel IP_buffer_ptr_U_8x8_left_line[2 * 8], pel IP_buffer_ptr_U_8x8_top_left,
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8], pel IP_buffer_ptr_V_8x8_left_line[2 * 8], pel IP_buffer_ptr_V_8x8_top_left,
	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4], pel IP_buffer_ptr_U_4x8_left_line[2 * 8], pel IP_buffer_ptr_U_4x8_top_left,
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4], pel IP_buffer_ptr_V_4x8_left_line[2 * 8], pel IP_buffer_ptr_V_4x8_top_left,
	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8], pel IP_buffer_ptr_U_8x4_left_line[2 * 4], pel IP_buffer_ptr_U_8x4_top_left,
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8], pel IP_buffer_ptr_V_8x4_left_line[2 * 4], pel IP_buffer_ptr_V_8x4_top_left,
	pel	IP_buffer_ptr_8x16_up_line[2 * 8], pel IP_buffer_ptr_8x16_left_line[2 * 16], pel IP_buffer_ptr_8x16_top_left,
	pel	IP_buffer_ptr_16x8_up_line[2 * 16], pel IP_buffer_ptr_16x8_left_line[2 * 8], pel IP_buffer_ptr_16x8_top_left,
	pel	IP_buffer_ptr_16x16_up_line[2 * 16], pel IP_buffer_ptr_16x16_left_line[2 * 16], pel IP_buffer_ptr_16x16_top_left,
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1],
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new,
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1],
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new
)
{
#pragma HLS resource variable=mod_info_best[0].ipm core=RAM_1P_BRAM 
#pragma HLS resource variable=mod_info_best[0].num_nz core=RAM_1P_BRAM 
	pel pred_0[12][8][8];
	pel pred_1[12][8][8];
	U7 cu_width_c = cu_width >> 1;
	U7 cu_height_c = cu_height >> 1;
	U3 cu_width_log2_c = cu_width_log2 - 1;
	U3 cu_height_log2_c = cu_height_log2 - 1;
	 rdo_chroma_8_pipeline_pred( mod_info_best,  md_fw_ptr,   Fetch_Ref_window_ptr,
		 bit_depth,  pred_0,  pred_1,  pb_x,  pb_y,  all_rdo_mode,  piRecoY
#if SUB_TMVP
		,  core_sbtmvp,
#endif
		 cu_width_c,  cu_height_c,  cu_width_log2_c,  cu_height_log2_c,
		IP_buffer_ptr_U_8x8_up_line, IP_buffer_ptr_U_8x8_left_line, IP_buffer_ptr_U_8x8_top_left,
		IP_buffer_ptr_V_8x8_up_line, IP_buffer_ptr_V_8x8_left_line, IP_buffer_ptr_V_8x8_top_left,
		IP_buffer_ptr_U_4x8_up_line, IP_buffer_ptr_U_4x8_left_line, IP_buffer_ptr_U_4x8_top_left,
		IP_buffer_ptr_V_4x8_up_line, IP_buffer_ptr_V_4x8_left_line, IP_buffer_ptr_V_4x8_top_left,
		IP_buffer_ptr_U_8x4_up_line, IP_buffer_ptr_U_8x4_left_line, IP_buffer_ptr_U_8x4_top_left,
		IP_buffer_ptr_V_8x4_up_line, IP_buffer_ptr_V_8x4_left_line, IP_buffer_ptr_V_8x4_top_left,
		IP_buffer_ptr_8x16_up_line, IP_buffer_ptr_8x16_left_line, IP_buffer_ptr_8x16_top_left,
		IP_buffer_ptr_16x8_up_line, IP_buffer_ptr_16x8_left_line, IP_buffer_ptr_16x8_top_left,
		IP_buffer_ptr_16x16_up_line, IP_buffer_ptr_16x16_left_line, IP_buffer_ptr_16x16_top_left,
		IP_map_scu_ptr_8x16_map_scu_up_line ,
		IP_map_scu_ptr_8x16_map_scu_left_line_new ,
		IP_map_scu_ptr_8x16_map_scu_top_left_new,
		IP_map_scu_ptr_16x8_map_scu_up_line ,
		IP_map_scu_ptr_16x8_map_scu_left_line_new ,
		IP_map_scu_ptr_16x8_map_scu_top_left_new,
		IP_map_scu_ptr_16x16_map_scu_up_line ,
		IP_map_scu_ptr_16x16_map_scu_left_line_new ,
		IP_map_scu_ptr_16x16_map_scu_top_left_new
	);
	 s16 coef_uv_pb_part[12][8][8];
	 pel rec_tmp[12][8][8];
	 int num_nz[12];
	 rdo_chroma_dataflow_8(all_rdo_mode, mod_info_best, cu_width_c, cu_height_c, cu_width_log2_c, cu_height_log2_c, orgU, pred_0, pred_1, bit_depth,
		 num_nz, rdoq_array, md_input_ptr, md_fw_ptr, coef_uv_pb_part, rec_tmp);
	 rdo_chroma_8_pipeline_updata(all_rdo_mode, mod_info_best, cu_width_c, cu_height_c,
		 orgU, pred_0, md_input_ptr, cu_width, cu_height, bit_depth,coef_uv_pb_part, rec_tmp, num_nz,
		 mod_info_best_rec_u,mod_info_best_rec_v,mod_info_best_coef_u,mod_info_best_coef_v
	 );

}
#endif

int rdo_chroma_8(RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best,
	COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, pel orgU[2][8][8], 
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8,
	IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_U_4x8,
	IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_V_4x8,
	IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_U_8x4,
	IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_V_8x4,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U1 coef_rec_write_flag[1], strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[16][16], 
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8, IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
)
{
	static pel rec_tmp[2][8][8];
	static s16 coef_uv_pb_part[2][8][8];
	static pel pred[2][8][8];
	
	U4 bit_depth = md_fw_ptr->bit_depth_internal;
	U3 cu_width_log2 = mod_info_best->cu_width_log2;
	U3 cu_height_log2 = mod_info_best->cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	U13 pb_x = mod_info_best->x_pos;
	U13 pb_y = mod_info_best->y_pos;


	s64 lambda_y = md_input_ptr->lambda_y;
	U12 size_tmp = (1 << (cu_width_log2 - 1)) * (1 << (cu_height_log2 - 1));

	int all_rdo_mode = 0;


	if (tree_status != TREE_LC && tree_status != TREE_C)
	{
		return 0;
	}


#if SIMPLE_MD
	all_rdo_mode = 2;
#else
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
#endif

	pel mod_info_best_rec_u[8][8];
	pel mod_info_best_rec_v[8][8];
	pel mod_info_best_coef_u[8][8];
	pel mod_info_best_coef_v[8][8];

	pel	IP_buffer_ptr_U_8x8_up_line[2 * 8]; pel IP_buffer_ptr_U_8x8_left_line[2 * 8]; pel IP_buffer_ptr_U_8x8_top_left;	//IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_U_8x8;
	pel	IP_buffer_ptr_V_8x8_up_line[2 * 8]; pel IP_buffer_ptr_V_8x8_left_line[2 * 8]; pel IP_buffer_ptr_V_8x8_top_left;	//IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_V_8x8;
	pel	IP_buffer_ptr_U_4x8_up_line[2 * 4]; pel IP_buffer_ptr_U_4x8_left_line[2 * 8]; pel IP_buffer_ptr_U_4x8_top_left;	//IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_U_4x8;
	pel	IP_buffer_ptr_V_4x8_up_line[2 * 4]; pel IP_buffer_ptr_V_4x8_left_line[2 * 8]; pel IP_buffer_ptr_V_4x8_top_left;	//IP_ARRAY_SIZE<4, 8>* IP_buffer_ptr_V_4x8;
	pel	IP_buffer_ptr_U_8x4_up_line[2 * 8]; pel IP_buffer_ptr_U_8x4_left_line[2 * 4]; pel IP_buffer_ptr_U_8x4_top_left;	//IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_U_8x4;
	pel	IP_buffer_ptr_V_8x4_up_line[2 * 8]; pel IP_buffer_ptr_V_8x4_left_line[2 * 4]; pel IP_buffer_ptr_V_8x4_top_left;	//IP_ARRAY_SIZE<8, 4>* IP_buffer_ptr_V_8x4;
	pel	IP_buffer_ptr_8x16_up_line[2 * 8]; pel IP_buffer_ptr_8x16_left_line[2 * 16];	pel IP_buffer_ptr_8x16_top_left;	//IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16;
	pel	IP_buffer_ptr_16x8_up_line[2 * 16]; pel IP_buffer_ptr_16x8_left_line[2 * 8];	pel IP_buffer_ptr_16x8_top_left; //IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8;
	pel	IP_buffer_ptr_16x16_up_line[2 * 16]; pel IP_buffer_ptr_16x16_left_line[2 * 16]; pel IP_buffer_ptr_16x16_top_left;	//IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16;
	U32 IP_map_scu_ptr_8x16_map_scu_up_line[(8 >> 2) << 1];
	U32 IP_map_scu_ptr_8x16_map_scu_left_line_new[(16 >> 2) << 1];
	U32 IP_map_scu_ptr_8x16_map_scu_top_left_new;
	U32 IP_map_scu_ptr_16x8_map_scu_up_line[(16 >> 2) << 1];
	U32 IP_map_scu_ptr_16x8_map_scu_left_line_new[(8 >> 2) << 1];
	U32 IP_map_scu_ptr_16x8_map_scu_top_left_new;
	U32 IP_map_scu_ptr_16x16_map_scu_up_line[(16 >> 2) << 1];
	U32 IP_map_scu_ptr_16x16_map_scu_left_line_new[(16 >> 2) << 1];
	U32 IP_map_scu_ptr_16x16_map_scu_top_left_new;
	


	IP_buffer_ptr_U_8x8_top_left = 0;
	IP_buffer_ptr_V_8x8_top_left = 0;
	IP_buffer_ptr_16x16_top_left = 0;
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_U_8x8_up_line[i]   = 0;
		IP_buffer_ptr_V_8x8_up_line[i]   = 0;
		IP_buffer_ptr_U_8x8_left_line[i] = 0;
		IP_buffer_ptr_V_8x8_left_line[i] = 0;
	}
	for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_16x16_up_line[i]   = 0;
		IP_buffer_ptr_16x16_left_line[i] = 0;
	}
	IP_map_scu_ptr_16x16_map_scu_top_left_new = 0;
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		IP_map_scu_ptr_16x16_map_scu_up_line[i] = 0;
		IP_map_scu_ptr_16x16_map_scu_left_line_new[i] = 0;
	}


	IP_buffer_ptr_U_4x8_top_left = 0;
	IP_buffer_ptr_V_4x8_top_left = 0;
	IP_buffer_ptr_8x16_top_left  = 0;
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_U_4x8_up_line[i] = 0;
		IP_buffer_ptr_V_4x8_up_line[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_U_4x8_left_line[i] = 0;
		IP_buffer_ptr_V_4x8_left_line[i] = 0;
		IP_buffer_ptr_8x16_up_line[i]    = 0;
	}
	for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_8x16_left_line[i] = 0;
	}
	IP_map_scu_ptr_8x16_map_scu_top_left_new = 0;
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		IP_map_scu_ptr_8x16_map_scu_up_line[i] = 0;
	}
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		IP_map_scu_ptr_8x16_map_scu_left_line_new[i] = 0;
	}

	IP_buffer_ptr_U_8x4_top_left = 0;
	IP_buffer_ptr_V_8x4_top_left = 0;
	IP_buffer_ptr_16x8_top_left  = 0;
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_U_8x4_left_line[i] = 0;
		IP_buffer_ptr_V_8x4_left_line[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_U_8x4_up_line[i]  = 0;
		IP_buffer_ptr_V_8x4_up_line[i]  = 0;
		IP_buffer_ptr_16x8_left_line[i] = 0;
	}
	for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
		IP_buffer_ptr_16x8_up_line[i] = 0;
	}
	IP_map_scu_ptr_16x8_map_scu_top_left_new = 0;
	for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
		IP_map_scu_ptr_16x8_map_scu_left_line_new[i] = 0;
	}
	for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
		IP_map_scu_ptr_16x8_map_scu_up_line[i] = 0;
	}

#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best_rec_v complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_best_coef_v complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x8_map_scu_left_line_new complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x8_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x4_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x4_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x4_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x4_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_8x16_map_scu_left_line_new complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_8x16_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_8x16_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_4x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_4x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_4x8_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_4x8_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_map_scu_ptr_16x16_map_scu_left_line_new complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x16_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_16x16_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x8_up_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_V_8x8_left_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=IP_buffer_ptr_U_8x8_left_line complete dim=0


    if ((cu_width_log2 == 4) && (cu_height_log2 == 4))  //wxh == 8x8
	{
		IP_buffer_ptr_U_8x8_top_left = IP_buffer_ptr_U_8x8->top_left;
		IP_buffer_ptr_V_8x8_top_left = IP_buffer_ptr_V_8x8->top_left;
		IP_buffer_ptr_16x16_top_left = IP_buffer_ptr_16x16->top_left;
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_U_8x8_up_line[i] = IP_buffer_ptr_U_8x8->up_line[i];
			IP_buffer_ptr_V_8x8_up_line[i] = IP_buffer_ptr_V_8x8->up_line[i];
			IP_buffer_ptr_U_8x8_left_line[i] = IP_buffer_ptr_U_8x8->left_line[i];
			IP_buffer_ptr_V_8x8_left_line[i] = IP_buffer_ptr_V_8x8->left_line[i];
		}
		for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_16x16_up_line[i] = IP_buffer_ptr_16x16->up_line[i];
			IP_buffer_ptr_16x16_left_line[i] = IP_buffer_ptr_16x16->left_line[i];
		}
		IP_map_scu_ptr_16x16_map_scu_top_left_new = IP_map_scu_ptr_16x16->map_scu_top_left_new;
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			IP_map_scu_ptr_16x16_map_scu_up_line[i] = IP_map_scu_ptr_16x16->map_scu_up_line[i];
			IP_map_scu_ptr_16x16_map_scu_left_line_new[i] = IP_map_scu_ptr_16x16->map_scu_left_line_new[i];
		}
	}
#if !SIMPLE_MD
	else if ((cu_width_log2 == 3) && (cu_height_log2 == 4)) //4x8
	{
		IP_buffer_ptr_U_4x8_top_left = IP_buffer_ptr_U_4x8->top_left;
		IP_buffer_ptr_V_4x8_top_left = IP_buffer_ptr_V_4x8->top_left;
		IP_buffer_ptr_8x16_top_left = IP_buffer_ptr_8x16->top_left;
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_U_4x8_up_line[i] = IP_buffer_ptr_U_4x8->up_line[i];
			IP_buffer_ptr_V_4x8_up_line[i] = IP_buffer_ptr_V_4x8->up_line[i];
		}
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_U_4x8_left_line[i] = IP_buffer_ptr_U_4x8->left_line[i];
			IP_buffer_ptr_V_4x8_left_line[i] = IP_buffer_ptr_V_4x8->left_line[i];
			IP_buffer_ptr_8x16_up_line[i] = IP_buffer_ptr_8x16->up_line[i];
		}
		for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_8x16_left_line[i] = IP_buffer_ptr_8x16->left_line[i];
		}
		IP_map_scu_ptr_8x16_map_scu_top_left_new = IP_map_scu_ptr_8x16->map_scu_top_left_new;
		for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
			IP_map_scu_ptr_8x16_map_scu_up_line[i] = IP_map_scu_ptr_8x16->map_scu_up_line[i];
		}
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			IP_map_scu_ptr_8x16_map_scu_left_line_new[i] = IP_map_scu_ptr_8x16->map_scu_left_line_new[i];
		}
	}
	else if ((cu_width_log2 == 4) && (cu_height_log2 == 3)) // 8x4
	{
		IP_buffer_ptr_U_8x4_top_left = IP_buffer_ptr_U_8x4->top_left;
		IP_buffer_ptr_V_8x4_top_left = IP_buffer_ptr_V_8x4->top_left;
		IP_buffer_ptr_16x8_top_left = IP_buffer_ptr_16x8->top_left;
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_U_8x4_left_line[i] = IP_buffer_ptr_U_8x4->left_line[i];
			IP_buffer_ptr_V_8x4_left_line[i] = IP_buffer_ptr_V_8x4->left_line[i];
		}
		for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_U_8x4_up_line[i] = IP_buffer_ptr_U_8x4->up_line[i];
			IP_buffer_ptr_V_8x4_up_line[i] = IP_buffer_ptr_V_8x4->up_line[i];
			IP_buffer_ptr_16x8_left_line[i] = IP_buffer_ptr_16x8->left_line[i];
		}
		for (int i = 0; i < 32; i++) {
#pragma HLS UNROLL
			IP_buffer_ptr_16x8_up_line[i] = IP_buffer_ptr_16x8->up_line[i];
		}
		IP_map_scu_ptr_16x8_map_scu_top_left_new = IP_map_scu_ptr_16x8->map_scu_top_left_new;
		for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
			IP_map_scu_ptr_16x8_map_scu_left_line_new[i] = IP_map_scu_ptr_16x8->map_scu_left_line_new[i];
		}
		for (int i = 0; i < 8; i++) {
#pragma HLS UNROLL
			IP_map_scu_ptr_16x8_map_scu_up_line[i] = IP_map_scu_ptr_16x8->map_scu_up_line[i];
		}
	}
#endif

  U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	U1 zero_1b = zero & 1;
	all_rdo_mode = all_rdo_mode + (zero_1b << (32 - 1));
	bit_depth = bit_depth + zero + (zero_1b << (8 - 1));
  pb_x  = pb_x + zero + (zero_1b << (16 - 1));
	pb_y  = pb_y + zero + (zero_1b << (16 - 1));
	/*for (int i = 0; i < 2; i = i + 1) {
		for (int j = 0; j < 2; j = j + 1) {
			core_sbtmvp->sbTmvp->mv[i][j] = core_sbtmvp->sbTmvp->mv[i][j] + (zero_1b << (32 - 1));
			core_sbtmvp->sbTmvp->ref_idx[i] = core_sbtmvp->sbTmvp->ref_idx[i] + (zero_1b << (32 - 1));
		}
	}*/

	//core_sbtmvp = core_sbtmvp + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[0].mv[0][0]  = core_sbtmvp[0].sbTmvp[0].mv[0][0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[0].mv[0][1]  = core_sbtmvp[0].sbTmvp[0].mv[0][1] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[1].mv[0][0]  = core_sbtmvp[0].sbTmvp[1].mv[0][0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[1].mv[0][1]  = core_sbtmvp[0].sbTmvp[1].mv[0][1] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[2].mv[0][0]  = core_sbtmvp[0].sbTmvp[2].mv[0][0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[2].mv[0][1]  = core_sbtmvp[0].sbTmvp[2].mv[0][1] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[3].mv[0][0]  = core_sbtmvp[0].sbTmvp[3].mv[0][0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[3].mv[0][1]  = core_sbtmvp[0].sbTmvp[3].mv[0][1] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[0].ref_idx[0]  = core_sbtmvp[0].sbTmvp[0].ref_idx[0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[1].ref_idx[0]  = core_sbtmvp[0].sbTmvp[1].ref_idx[0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[2].ref_idx[0]  = core_sbtmvp[0].sbTmvp[2].ref_idx[0] + (zero_1b << (32 - 1));
	core_sbtmvp[0].sbTmvp[3].ref_idx[0]  = core_sbtmvp[0].sbTmvp[3].ref_idx[0] + (zero_1b << (32 - 1));
		
	core_sbtmvp->sbTmvp_flag = core_sbtmvp->sbTmvp_flag + (zero_1b << (32 - 1));

	mod_info_best->x_scu = mod_info_best->x_scu + (zero_1b << (16 - 1));
	mod_info_best->y_scu = mod_info_best->y_scu + (zero_1b << (16 - 1));

	for (int i = 0; i < 4; i = i + 1) {
#pragma HLS UNROLL
		for (int j = 0; j < 2; j = j + 1) {
#pragma HLS UNROLL
			mod_info_best->ipm[i][j] = mod_info_best->ipm[i][j] + zero + (zero_1b << (8 - 1));
			}
	}
	for (int i = 0; i < 4; i = i + 1) {
#pragma HLS UNROLL
		for (int j = 0; j < 3; j = j + 1) {
#pragma HLS UNROLL
			mod_info_best->num_nz[i][j] = mod_info_best->num_nz[i][j] + zero + (zero_1b << (16 - 1));
		}
	}
 
 
	rdo_chroma_8_pipeline(all_rdo_mode, rdoq_array, md_fw_ptr, tree_status, mod_info_best,
		mod_info_curr_pb_info, md_input_ptr, orgU, 

		RefWindowLCU_UP, RefWindowLCU_VP, CtrPos, 
#if ENABLE_BFRAME
		RefWindowLCU_UP_ref1, RefWindowLCU_VP_ref1, CtrPos_ref1,
#endif
		cu_width_log2, cu_height_log2, cu_width,
		cu_height, pb_x, pb_y, size_tmp, lambda_y, bit_depth, coef_uv_pb_part, rec_tmp, pred, Fetch_Ref_window_ptr,
		piRecoY

#if SUB_TMVP
	, core_sbtmvp
#endif
		, mod_info_best_rec_u,
		mod_info_best_rec_v,
		mod_info_best_coef_u,
		mod_info_best_coef_v,
		IP_buffer_ptr_U_8x8_up_line, IP_buffer_ptr_U_8x8_left_line,  IP_buffer_ptr_U_8x8_top_left,
		IP_buffer_ptr_V_8x8_up_line, IP_buffer_ptr_V_8x8_left_line,  IP_buffer_ptr_V_8x8_top_left,
		IP_buffer_ptr_U_4x8_up_line, IP_buffer_ptr_U_4x8_left_line,  IP_buffer_ptr_U_4x8_top_left,
		IP_buffer_ptr_V_4x8_up_line, IP_buffer_ptr_V_4x8_left_line,  IP_buffer_ptr_V_4x8_top_left,
		IP_buffer_ptr_U_8x4_up_line, IP_buffer_ptr_U_8x4_left_line,  IP_buffer_ptr_U_8x4_top_left,
		IP_buffer_ptr_V_8x4_up_line, IP_buffer_ptr_V_8x4_left_line,  IP_buffer_ptr_V_8x4_top_left,
		IP_buffer_ptr_8x16_up_line,  IP_buffer_ptr_8x16_left_line,  IP_buffer_ptr_8x16_top_left,
		IP_buffer_ptr_16x8_up_line, IP_buffer_ptr_16x8_left_line,  IP_buffer_ptr_16x8_top_left,
		IP_buffer_ptr_16x16_up_line, IP_buffer_ptr_16x16_left_line,  IP_buffer_ptr_16x16_top_left,
		IP_map_scu_ptr_8x16_map_scu_up_line,
		IP_map_scu_ptr_8x16_map_scu_left_line_new,
		IP_map_scu_ptr_8x16_map_scu_top_left_new,
		IP_map_scu_ptr_16x8_map_scu_up_line,
		IP_map_scu_ptr_16x8_map_scu_left_line_new,
		IP_map_scu_ptr_16x8_map_scu_top_left_new,
		IP_map_scu_ptr_16x16_map_scu_up_line,
		IP_map_scu_ptr_16x16_map_scu_left_line_new,
		IP_map_scu_ptr_16x16_map_scu_top_left_new
	);

	for (int k = 0; k < 8; ++k) {
#pragma HLS UNROLL
		for (int j = 0; j < 8; j++) {
#pragma HLS UNROLL
			if ((k < (cu_height / 2)) && (j < (cu_width / 2))) {
				mod_info_best->rec_u[k * cu_width / 2 + j] = mod_info_best_rec_u[k][j];
				mod_info_best->rec_v[k * cu_width / 2 + j] = mod_info_best_rec_v[k][j];
				mod_info_best->coef_u[k * cu_width / 2 + j] = mod_info_best_coef_u[k][j];
				mod_info_best->coef_v[k * cu_width / 2 + j] = mod_info_best_coef_v[k][j];// coef_uv_pb_part[k][(j + k) & 31];
			}
			else
			{
				break;
			}
		}
	}

	return 0;
}
//#endif



#if !LUMA_8_PIPELINE_DUMMY
void rdo_luma_8_pipeline(u8 all_rdo_num, U8 w, U8 h, U2 tree_status, U3 cu_width_log2, U3 cu_height_log2, U2 curr_cons_pred_mode, U4 bit_depth, u16 avail_tb,
	RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_mode[MAX_SKIP_NUM], SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* best_info_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	u8 rdo_list[MAX_RDO_NUM_SIZE(8, 8)], U1 valid_mode[17], u8 ipred_list[IPRED_LIST_NUM], pel src_le_temp[2 * 8 + 3], pel src_up_temp[2 * 8 + 3], S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	pel orgY[8 * 8], 
#if ENABLE_BFRAME
	MV_DIR* mv_dir, strFetch_ref_window* Fetch_Ref_window_ptr,
#endif
	/*s64 flag_cost[2], */s32 dist_value,
	s64 cost_best[1], 
	//pel fme2mdrefbuf[2560][8][2][16],
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_inter1[8][8], pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 8) - 1][8][8], pel pred_y_intra[MAX_RDO_NUM_SIZE(8, 8)][8][8]
)
{
	s64 flag_cost[2];
#ifdef MD_CMOD
	for (u8 rdo_idx = 0; rdo_idx < all_rdo_num; rdo_idx++)
	{
#pragma HLS LOOP_TRIPCOUNT max=2
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
	int rdo_cnt = MAX_RDO_NUM_SIZE(8, 8);
#endif
	for (u8 rdo_idx = 0; rdo_idx < rdo_cnt; rdo_idx++)

#else
	for (u8 rdo_idx = 0; rdo_idx < all_rdo_num; rdo_idx++)
#endif
	{
#pragma HLS LOOP_TRIPCOUNT max=5
#pragma HLS  PIPELINE
#else
	for (u8 rdo_idx = 0; rdo_idx < 5; rdo_idx++)
	{
#pragma HLS LOOP_TRIPCOUNT max=5
#pragma HLS  PIPELINE
#endif
		//static pel pred_for_diff[8][8];
		static pel diff[8][8];
		static pel pred_for_ssd_forced_zeros[8][8];
		static pel pred_for_rec[8][8];
		static pel pred_for_update[8][8];
		static pel coef_h[8][8];
		static pel coef[8][8];
		static pel qcoef_for_dq[8][8];
		static pel qcoef_for_re[8][8];
		static pel qcoef_for_update[8][8];
		static pel itcoef_v[8][8];
		static pel rec_for_update[8][8];
		//static pel orgY_for_ssd_forced_zeros[64];
		//static pel orgY_for_ssd[64];
		static pel orgY_for_ssd_forced_zeros[8][8];
		static pel orgY_for_ssd[8][8];
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
		static S3	mod_info_refi_for_re_fz[2], mod_info_refi_for_re[2], mod_info_refi_for_up[2];
		static s16	mod_info_mvd_for_re_fz[2][2], mod_info_mvd_for_re[2][2], mod_info_mvd_for_up[2][2];
		static S14	mod_info_mv_for_up[2][2];
		static s16 mod_info_affine_mvd_for_re_fz[2][4][2], mod_info_affine_mvd_for_re[2][4][2];
		static u8	mod_info_mpm_for_re[4][2], mod_info_mpm_for_up[4][2];
		static S8	mod_info_ipm_for_re_fz[4][2], mod_info_ipm_for_re[4][2], mod_info_ipm_for_up[4][2];
		w = 8;
		h = 8;
		u8 num_refp_cnt_for_rate;
		u8 num_refp_cnt_for_ssd;
#if !AZB_ENABLE
		intra_inter_pred_8(w, h, tree_status, cu_width_log2, cu_height_log2, avail_tb, rdo_idx,
			md_fw_ptr, skip_rdo, merge_rdo, skip_rdo_mode, mod_info_curr_ptr,
			rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY,
			& curr_mvr_for_re, &mode_type_for_ssd_fz, &inter_mode_idx_for_ssd_fz, &valid_flag_for_ssd_fz, &is_intra_for_quant,
			mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_mv_for_up, mod_info_affine_mvd_for_re_fz, mod_info_ipm_for_re_fz,
			orgY_for_ssd_forced_zeros, diff, pred_for_ssd_forced_zeros, md_fw_ptr->bit_depth_internal, 
			md_input_ptr->CtrPos2MD, 
			& num_refp_cnt_for_ssd,
			& num_refp_cnt_for_rate,
#if ENABLE_BFRAME
			md_input_ptr->CtrPos2MD_ref1, mv_dir, Fetch_Ref_window_ptr,
#endif // ENABLE_BFRAME
			//fme2mdrefbuf,
			mv_copy, mvd_copy, refi_copy, pred_y_inter1, pred_y_inter2, pred_y_intra
#if USE_SPEED_LEVEL
			, md_input_ptr->speed_level
#endif
		);

		ssd_bitcnt_8(tree_status, curr_cons_pred_mode, num_refp_cnt_for_ssd,
			md_input_ptr->lambda_y, mode_type_for_ssd_fz, inter_mode_idx_for_ssd_fz, valid_flag_for_ssd_fz,
			md_fw_ptr, mod_info_curr_ptr,
			mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_affine_mvd_for_re_fz, mod_info_curr_ptr->mpm, mod_info_ipm_for_re_fz,
			orgY_for_ssd_forced_zeros, pred_for_ssd_forced_zeros,
			&lambda_y_for_quant, &mode_type_for_re, &valid_flag_for_up, &dist_pred_for_up, &cost_best_temp_for_up,
			md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
			orgY_for_ssd, pred_for_rec);

		transform_hor_8(w, h, bit_depth, diff, coef_h);
		transform_ver_8(w, h, bit_depth, coef_h, coef);

		quant_nnz_8_luma(cu_width_log2, cu_height_log2, 0, md_fw_ptr->slice_type, bit_depth, md_input_ptr->qp_y,
			lambda_y_for_quant, is_intra_for_quant,
			rdoq_array,
			coef,
			&lambda_y_for_re, &is_intra_for_re, &num_nz_for_re, &num_nz_for_itv,
			qcoef_for_dq, qcoef_for_re, qcoef_for_update);

		rate_est_8(w, h, tree_status, curr_cons_pred_mode, num_refp_cnt_for_rate,
			lambda_y_for_re, num_nz_for_re, mode_type_for_re, curr_mvr_for_re, is_intra_for_re,
			md_fw_ptr, mod_info_curr_ptr,
			md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
			qcoef_for_re,
			&mode_type_for_up, &curr_mvr_for_up, &is_intra_for_up,
			mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
			flag_cost);

		inv_transform_ver_8(w, h, bit_depth, md_input_ptr->qp_y, num_nz_for_itv, qcoef_for_dq, &num_nz_for_ith, &num_nz_for_up, itcoef_v);
		inv_transform_hor_8(w, h, bit_depth, mod_info_curr_ptr->tb_part, cu_width_log2, cu_height_log2, 0,
			num_nz_for_ith,
			orgY_for_ssd, pred_for_rec, itcoef_v,
			&dist_value,
			pred_for_update, rec_for_update);

		update_rdo_luma_8(w, h,
			valid_flag_for_up, dist_pred_for_up, dist_value, cost_best_temp_for_up, num_nz_for_up, mode_type_for_up, is_intra_for_up, curr_mvr_for_up,
			cu_data_ptr,
			mod_info_curr_ptr, best_info_ptr,
			mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mv_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
			flag_cost, pred_for_update, rec_for_update, qcoef_for_update,
			cost_best
#if USE_ROI_MODE_CONFIG
			, md_input_ptr->valid_mode_config
#endif
		);
#else
		if (md_fw_ptr->slice_type == 1)
		{
			intra_inter_pred_8(w, h, tree_status, cu_width_log2, cu_height_log2, avail_tb, rdo_idx,
				md_fw_ptr, skip_rdo, merge_rdo, skip_rdo_mode, mod_info_curr_ptr,
				rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY, RefWindowLCU, CtrPos,
#if ENABLE_BFRAME
				RefWindowLCU_ref1, CtrPos_ref1, mv_dir, refwin_md_ref1, CtrPosFME_ref1, Fetch_Ref_window_ptr,
#endif
				& curr_mvr_for_re, &mode_type_for_ssd_fz, &inter_mode_idx_for_ssd_fz, &valid_flag_for_ssd_fz, &is_intra_for_quant,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_mv_for_up, mod_info_affine_mvd_for_re_fz, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, diff, pred_for_ssd_forced_zeros, md_fw_ptr->bit_depth_internal, refwin_md, CtrPosFME);

			ssd_bitcnt_8(tree_status, curr_cons_pred_mode,
				md_input_ptr->lambda_y, mode_type_for_ssd_fz, inter_mode_idx_for_ssd_fz, valid_flag_for_ssd_fz,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_affine_mvd_for_re_fz, mod_info_curr_ptr->mpm, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, pred_for_ssd_forced_zeros,
				&lambda_y_for_quant, &mode_type_for_re, &valid_flag_for_up, &dist_pred_for_up, &cost_best_temp_for_up,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				orgY_for_ssd, pred_for_rec);

			transform_hor_8(w, h, bit_depth, diff, coef_h);
			transform_ver_8(w, h, bit_depth, coef_h, coef);

			quant_nnz_8(cu_width_log2, cu_height_log2, 0, md_fw_ptr->slice_type, bit_depth, md_input_ptr->qp_y,
				lambda_y_for_quant, is_intra_for_quant,
				model_cnt, rdoq_array,
				coef,
				&lambda_y_for_re, &is_intra_for_re, &num_nz_for_re, &num_nz_for_itv,
				qcoef_for_dq, qcoef_for_re, qcoef_for_update);

			rate_est_8(w, h, tree_status, curr_cons_pred_mode,
				lambda_y_for_re, num_nz_for_re, mode_type_for_re, curr_mvr_for_re, is_intra_for_re,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				qcoef_for_re,
				&num_nz_for_up, &mode_type_for_up, &curr_mvr_for_up, &is_intra_for_up,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost);

			inv_transform_ver_8(w, h, bit_depth, md_input_ptr->qp_y, num_nz_for_itv, qcoef_for_dq, &num_nz_for_ith, itcoef_v);
			inv_transform_hor_8(w, h, bit_depth, mod_info_curr_ptr->tb_part, cu_width_log2, cu_height_log2, 0,
				num_nz_for_ith,
				orgY_for_ssd, pred_for_rec, itcoef_v,
				&dist_value,
				pred_for_update, rec_for_update);

			update_rdo_luma_8(w, h,
				valid_flag_for_up, dist_pred_for_up, dist_value, cost_best_temp_for_up, num_nz_for_up, mode_type_for_up, is_intra_for_up, curr_mvr_for_up,
				cu_data_temp_ptr, mod_info_curr_ptr, best_info_ptr,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mv_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost, pred_for_update, rec_for_update, qcoef_for_update,
				cost_best);
		}
		else
		{
			intra_inter_pred_8(w, h, tree_status, cu_width_log2, cu_height_log2, avail_tb, rdo_idx,
				md_fw_ptr, skip_rdo, merge_rdo, skip_rdo_mode, mod_info_curr_ptr,
				rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY, RefWindowLCU, CtrPos,
				&curr_mvr_for_re, &mode_type_for_ssd_fz, &inter_mode_idx_for_ssd_fz, &valid_flag_for_ssd_fz, &is_intra_for_re,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_mv_for_up, mod_info_affine_mvd_for_re_fz, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, diff, pred_for_ssd_forced_zeros, md_fw_ptr->bit_depth_internal, refwin_md, CtrPosFME);
			ssd_bitcnt_8(tree_status, curr_cons_pred_mode,
				md_input_ptr->lambda_y, mode_type_for_ssd_fz, inter_mode_idx_for_ssd_fz, valid_flag_for_ssd_fz,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				mod_info_refi_for_re_fz, mod_info_mvd_for_re_fz, mod_info_affine_mvd_for_re_fz, mod_info_curr_ptr->mpm, mod_info_ipm_for_re_fz,
				orgY_for_ssd_forced_zeros, pred_for_ssd_forced_zeros,
				&lambda_y_for_re, &mode_type_for_re, &valid_flag_for_up, &dist_pred_for_up, &cost_best_temp_for_up,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				orgY_for_ssd, pred_for_rec);
			for (int i = 0; i < 64; ++i)
			{
				qcoef_for_re[i] = 0;
			}
			num_nz_for_re = 0;
			rate_est_azb_8(w, h, tree_status, curr_cons_pred_mode,
				lambda_y_for_re, num_nz_for_re, mode_type_for_re, curr_mvr_for_re, is_intra_for_re,
				model_cnt, md_fw_ptr, mod_info_curr_ptr,
				md_fw_ptr_refp_ptr_for_re, mod_info_refi_for_re, mod_info_mvd_for_re, mod_info_affine_mvd_for_re, mod_info_mpm_for_re, mod_info_ipm_for_re,
				qcoef_for_re,
				&num_nz_for_up, &mode_type_for_up, &curr_mvr_for_up, &is_intra_for_up,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost);
			dist_value = enc_ssd_16b_1_8(cu_width_log2, cu_height_log2, pred_for_ssd_forced_zeros, orgY, w, 8, bit_depth);
			update_rdo_luma_8(w, h,
				valid_flag_for_up, dist_pred_for_up, dist_value, cost_best_temp_for_up, num_nz_for_up, mode_type_for_up, is_intra_for_up, curr_mvr_for_up,
				cu_data_temp_ptr, mod_info_curr_ptr, best_info_ptr,
				mod_info_refi_for_up, mod_info_mvd_for_up, mod_info_mv_for_up, mod_info_mpm_for_up, mod_info_ipm_for_up,
				flag_cost, pred_for_ssd_forced_zeros, pred_for_ssd_forced_zeros, qcoef_for_re,
				cost_best);
		}
#endif
	}
}
#endif //!LUMA_8_PIPELINE_DUMMY


void ipred_ang_eipm_8_pred(pel src_le[19], pel src_up[19], int w, int h, int ipm
#if MIPF
	, int is_luma, int mipf_enable_flag, U4 bit_depth, pel diff[8][8]
#endif
)
{
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=0
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=0

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

#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_bilinear complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_dxdy complete dim=0
#pragma HLS   ARRAY_PARTITION variable=tbl_mc_c_coeff_hp complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_extra_smooth complete dim=0
#pragma HLS   ARRAY_PARTITION variable=com_tbl_ipred_adi_semi_sharp complete dim=0
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

	if (((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) || (((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))))
	{
		if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT))
			pos_max = w * 2 - 1;
		else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
			pos_max = h * 2 - 1;

		for (j = 0; j < 8; j++)
		{
#pragma HLS UNROLL
			int dx;

			for (i = 0; i < 8; i++)
			{
				int x;
				int src_p, src_pn, src_pn_n1, src_pn_p2;

				if ((ipm < IPD_VER) || (ipm >= IPD_DIA_L_EXT && ipm <= IPD_VER_EXT)) {
					GET_REF_POS(mt[0], j + 1, dx, offset);
					filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;

					x = i + dx;

					pn_n1 = x - 1;
					p = x;
					pn = x + 1;
					pn_p2 = x + 2;

					pn_n1 = COM_MIN(pn_n1, pos_max);
					p = COM_MIN(p, pos_max);
					pn = COM_MIN(pn, pos_max);
					pn_p2 = COM_MIN(pn_p2, pos_max);

					src_pn_n1 = src_up[pn_n1];
					src_p = src_up[p];
					src_pn = src_up[pn];
					src_pn_p2 = src_up[pn_p2];
				}
				else if ((ipm > IPD_HOR && ipm < IPD_IPCM) || (ipm >= IPD_HOR_EXT && ipm < IPD_CNT))
				{
					GET_REF_POS(mt[1], i + 1, dx, offset);
					filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;

					x = j + dx;

					pn_n1 = x - 1;
					p = x;
					pn = x + 1;
					pn_p2 = x + 2;

					pn_n1 = COM_MIN(pn_n1, pos_max);
					p = COM_MIN(p, pos_max);
					pn = COM_MIN(pn, pos_max);
					pn_p2 = COM_MIN(pn_p2, pos_max);

					src_pn_n1 = src_le[pn_n1];
					src_p = src_le[p];
					src_pn = src_le[pn];
					src_pn_p2 = src_le[pn_p2];

				}

				s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
				s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
				s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
				s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
				s16 data5 = filter_offset_list[filter_idx];
				s16 data6 = filter_bits_list[filter_idx];
				s32 buffer1 = src_pn_n1 * data1;
				s32 buffer2 = src_p * data2;
				s32 buffer3 = src_pn * data3;
				s32 buffer4 = src_pn_p2 * data4;
				tmp = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
				tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
				diff[j][i] = tmp1;
				}
			}
	}
	else
	{
		for (i = 0; i < 8; i++)
		{
#pragma HLS UNROLL
			GET_REF_POS(mt[1], i + 1, t_dy[i], offset_y[i]);
			t_dy[i] = -t_dy[i];
		}
		for (j = 0; j < 8; j++)
		{
#pragma HLS UNROLL
			GET_REF_POS(mt[0], j + 1, t_dx[j], offset_x[j]);
			t_dx[j] = -t_dx[j];
		}

		{
			for (j = 0; j < 8; j++)
			{
#pragma HLS UNROLL
				for (i = 0; i < 8; i++)
				{
					int x = i + t_dx[j];
					int y = j + t_dy[i];
					int src_p, src_pn, src_pn_n1, src_pn_p2;

					if (y <= -1)
					{
						offset = offset_x[j];
						pos_max = w * 2 - 1;

						pn_n1 = x + 1;
						p = x;
						pn = x - 1;
						pn_p2 = x - 2;

						pn_n1 = COM_MIN(pn_n1, pos_max);
						p = COM_MIN(p, pos_max);
						pn = COM_MIN(pn, pos_max);
						pn_p2 = COM_MIN(pn_p2, pos_max);

						src_pn_n1 = src_up[pn_n1];
						src_p = src_up[p];
						src_pn = src_up[pn];
						src_pn_p2 = src_up[pn_p2];

					}
					else
					{
						offset = offset_y[i];
						pos_max = h * 2 - 1;

						pn_n1 = y + 1;
						p = y;
						pn = y - 1;
						pn_p2 = y - 2;

						pn_n1 = COM_MIN(pn_n1, pos_max);
						p = COM_MIN(p, pos_max);
						pn = COM_MIN(pn, pos_max);
						pn_p2 = COM_MIN(pn_p2, pos_max);

						src_pn_n1 = src_le[pn_n1];
						src_p = src_le[p];
						src_pn = src_le[pn];
						src_pn_p2 = src_le[pn_p2];

					}

					if (ipm < IPD_DIA_R || (ipm > IPD_VER_EXT && ipm <= IPD_DIA_R_EXT)) {
						filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;
					}
					else
					{
						filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;
					}

					s16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
					s16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
					s16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
					s16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
					s16 data5 = filter_offset_list[filter_idx];
					s16 data6 = filter_bits_list[filter_idx];
					s32 buffer1 = src_pn_n1 * data1;
					s32 buffer2 = src_p * data2;
					s32 buffer3 = src_pn * data3;
					s32 buffer4 = src_pn_p2 * data4;
					tmp = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
					tmp1 = COM_CLIP3(0, ((1 << bit_depth) - 1), tmp);
					diff[j][i] = tmp1;
				}
			}
		}
	}
}


void get_ref_intra_8(u8 all_rdo_num, u8 rdo_list[5], u8 intra_mode_list[5], U7 w, U7 h,
	pel src_le_temp[8 * 2 + 3], pel src_up_temp[8 * 2 + 3], pel pred_y_intra[5][8][8],
	U4 bit_depth, u16 avail_tb, int mipf_enable_flag) {

#pragma HLS ARRAY_PARTITION variable=pred_y_intra complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_intra complete dim=3

	for (U8 rdo_idx = 0; rdo_idx < all_rdo_num; rdo_idx++) {
#pragma HLS PIPELINE
#pragma HLS loop_tripcount max=5
		s8 inter_mode_idx = rdo_list[rdo_idx];
		U1 is_intra;
		s8 intra_mode_idx;
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

		if (is_intra != 0)
		{

			if (intra_mode_idx == 12)
				Ipred_vert_8_pred(w, h, src_up_temp + 3, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 24)
				Ipred_hor_8_pred(w, h, src_le_temp + 3, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 0)
				Ipred_dc_8_pred(w, h, src_le_temp + 3, src_up_temp + 3, bit_depth, avail_tb, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 1)
				Ipred_plane_8_pred(w, h, src_le_temp + 2, src_up_temp + 2, bit_depth, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx == 2)
				Ipred_bi_8_pred(w, h, src_le_temp + 3, src_up_temp + 3, bit_depth, pred_y_intra[rdo_idx]);
			else if (intra_mode_idx >= 3 && intra_mode_idx <= 11)
				ipred_ang1_8_pred(w, h, src_le_temp, src_up_temp, bit_depth, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, mipf_enable_flag
#endif
				);
			else if (intra_mode_idx >= 25 && intra_mode_idx <= 32) {
				ipred_ang2_8_pred(w, h, src_le_temp, src_up_temp, bit_depth, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, mipf_enable_flag
#endif
				);
			}
			else if (intra_mode_idx >= 13 && intra_mode_idx <= 23) {
				ipred_ang3_8_pred(w, h, src_le_temp, src_up_temp, bit_depth, intra_mode_idx, pred_y_intra[rdo_idx]
#if MIPF
					, 1, mipf_enable_flag
#endif
				);
			}

#if EIPM
			else
				ipred_ang_eipm_8_pred(src_le_temp + 3, src_up_temp + 3, w, h, intra_mode_idx
#if MIPF
					, 1, mipf_enable_flag, bit_depth, pred_y_intra[rdo_idx]
#endif
				);
#endif
		}
	}
}

void get_ref_inter_8(MD_FW* md_fw_ptr, U2 curr_cons_pred_mode, SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], MD_COM_MODE* mod_info_curr,
	u8 rdo_list[5], MV_DIR* mv_dir, U7 w, U7 h, S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2], U2 tree_status, U4 bit_depth,
	s16 mvd_copy[2][2], S14 mv_copy[2][2], S3 refi_copy[2], pel pred_y_skip_inter[13][8][8], strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS], S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], Arbitor& ref,
	SKIP_RDO* skip_rdo, pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 8) - 1][8][8]) {
	U14 pic_width = md_fw_ptr->pic_width_in_scu << 2;
	U14 pic_height = md_fw_ptr->pic_height_in_scu << 2;
	U2 slice_type = md_fw_ptr->slice_type;

	s16 coef_tmp1[8][8];
	static s16 mvd_for_ip[2][2];

#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP complete dim=0
#pragma HLS ARRAY_PARTITION variable=CtrPosSKIP_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=refi_copy complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->refi complete dim=0
#pragma HLS ARRAY_PARTITION variable=mod_info_curr->mv complete dim=0
#pragma HLS ARRAY_PARTITION variable=mvd_copy complete dim=0
#pragma HLS ARRAY_PARTITION variable=skip_mode complete dim=0

	if (!(slice_type == 1 || curr_cons_pred_mode == ONLY_INTRA)) {
		for (int idx = 0; idx < (SKIP_MODE_NUM + 1); ++idx) {
#pragma HLS PIPELINE
#pragma HLS loop_tripcount max=12
			if (idx < SKIP_MODE_NUM) {
				// derive mode info
				SKIP_ELEMENT* mode = skip_mode + idx;
				if (mode->valid == 0) { continue; }
#pragma HLS ARRAY_PARTITION variable=mode->mvp complete dim=0
#pragma HLS ARRAY_PARTITION variable=mode->refi complete dim=0
				mod_info_curr->cu_mode = MODE_SKIP;
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
#if ENABLE_BFRAME
					if (!REFI_IS_VALID(mod_info_curr->refi[REFP_0]) && !REFI_IS_VALID(mod_info_curr->refi[REFP_1])) { continue; }
#else
					if (mod_info_curr->refi[0] == -1) { continue; }
#endif
			}
			else {
				//for (int rdo_idx = 0; rdo_idx < 5; rdo_idx++) {
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
						BOOL res = analyze_me_uni_pred_8(mod_info_curr, num_refp_cnt, curr_mvr, w, h,
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
						//mvd_copy[lidx][0] = mvd_for_ip[lidx][0];
						//mvd_copy[lidx][1] = mvd_for_ip[lidx][1];
						refi_copy[0] = mod_info_curr->refi[0];
						refi_copy[1] = mod_info_curr->refi[1];
					}
				}
			}
			// calc pred info
			CU_POS cu = { (int)mod_info_curr->scup, mod_info_curr->x_pos, mod_info_curr->y_pos, mod_info_curr->cu_width, mod_info_curr->cu_height };
			BOOL res = com_mc_cu_y_8(&cu, pic_width, pic_height, mod_info_curr->refi, mod_info_curr->mv,
				SWW, pred_y_skip_inter[idx], bit_depth, Fetch_Ref_window_ptr,
				CtrPosSKIP,
#if ENABLE_BFRAME
					CtrPosSKIP_ref1,
#endif // ENABLE_BFRAME
				ref
				, mod_info_curr->cu_mode
			);
		}
	}
	//pred inter
	U8 mode_list[MAX_INTER_SKIP_RDO];
#pragma HLS ARRAY_PARTITION variable=mode_list complete dim=0

	for (int i = 0, j = 0; i < 12; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=12 
		if (j < 4) {
			if (skip_mode[i].valid && (skip_mode[i].refi[REFP_0] >= 0 || REFI_IS_VALID(skip_mode[i].refi[REFP_1])))
			{
				mode_list[j++] = i;
			}
		}
	}

#pragma HLS ARRAY_PARTITION variable=pred_y_inter2 complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_inter2 complete dim=3
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter complete dim=2
#pragma HLS ARRAY_PARTITION variable=pred_y_skip_inter complete dim=3

	for (U8 rdo_idx = 1; rdo_idx < MAX_RDO_NUM_INTER_SIZE(8, 8); rdo_idx++) {
#pragma HLS PIPELINE
#pragma HLS loop_tripcount max=3 
		s8 inter_mode_idx = rdo_list[rdo_idx];
		U1 is_intra_write;

		if (inter_mode_idx < skip_rdo->num_rdo) {
			int skip_idx = inter_mode_idx;
			if (rdo_idx == 1) {
				for (int j = 0; j < 8; j++) {
#pragma HLS UNROLL
					for (int k = 0; k < 8; k++) {
						pred_y_inter2[rdo_idx - 1][j][k] = pred_y_skip_inter[mode_list[skip_idx]][j][k];
					}
				}
				continue;
			}
			else if (rdo_idx == 2) {
				for (int j = 0; j < 8; j++) {
#pragma HLS UNROLL
					for (int k = 0; k < 8; k++) {
						pred_y_inter2[rdo_idx-1][j][k] = pred_y_skip_inter[mode_list[skip_idx]][j][k];
					}
				}
				continue;
			}
		}
	}
}

void get_ref_pel_8(MD_COM_MODE* mod_info_curr,
	SKIP_ELEMENT skip_mode[SKIP_MODE_NUM], U2 tree_status, U2 curr_cons_pred_mode, U4 bit_depth,
	U7 w, U7 h, u16 avail_tb, MD_FW* md_fw_ptr, SKIP_RDO* skip_rdo, MERGE_RDO merge_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO],
	u8 rdo_list[MAX_RDO_NUM_SIZE(8, 8)], U1 valid_mode[17], u8 intra_mode_list[IPRED_LIST_NUM],
	pel src_le_temp[8 * 2 + 3], pel src_up_temp[8 * 2 + 3],
	S14 sub_pel_mv[MV_D], s16 mvp_all[2][5][2],
	strFetch_ref_window* Fetch_Ref_window_ptr,
	S14 CtrPosSKIP[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	S14 CtrPosSKIP_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], MV_DIR* mv_dir,
#endif // ENABLE_BFRAME
	Arbitor& ref,
	S14 mv_copy[2][2], s16 mvd_copy[2][2], S3 refi_copy[2], pel pred_y_skip_inter[13][8][8], pel pred_y_intra[MAX_RDO_NUM_SIZE(8, 8)][8][8], pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 8) - 1][8][8], u8 all_rdo_num
)
{
	u8 w_inter, h_inter, rdo_list_inter[5];
	int mipf_flag_intra;
	U4 bit_depth_intra;
	w_inter = w;
	h_inter = h;
	for (int i = 0; i < 5; i++) {
#pragma HLS UNROLL
#pragma HLS latency min=1 max=1
		rdo_list_inter[i] = rdo_list[i];
	}
	mipf_flag_intra = md_fw_ptr->mipf_enable_flag;
	bit_depth_intra = bit_depth;


	get_ref_inter_8(md_fw_ptr, curr_cons_pred_mode, skip_mode, mod_info_curr,
		rdo_list_inter, mv_dir, w_inter, h_inter, sub_pel_mv, mvp_all, tree_status, bit_depth,
		mvd_copy, mv_copy, refi_copy, pred_y_skip_inter, Fetch_Ref_window_ptr,
		CtrPosSKIP, CtrPosSKIP_ref1, ref,
		skip_rdo, pred_y_inter2);

	get_ref_intra_8(all_rdo_num, rdo_list, intra_mode_list, w, h,
		src_le_temp, src_up_temp, pred_y_intra,
		bit_depth_intra, avail_tb, mipf_flag_intra);
}


s64 rdo_luma_8(U1 isNS_allow, U8 w, U8 h,
	RDOQ_ARRAY* rdoq_array,
	MD_FW* md_fw_ptr,
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4],
	ENC_ME_NEB_INFO left_nebs[2], 
	SKIP_MERGE_INPUT* skip_in,
	U2 tree_status,
	MD_COM_MODE_BEST* best_info_ptr,
	MD_INPUT* md_input_ptr,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, 
#endif
	MD_COM_MODE* mod_info_curr_ptr,
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	U2 curr_cons_pred_mode, U9 cup, U3 qt_depth,
	ENC_FME_INFO_ALL* fme_mv_info,
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE],
#else
	u8 rmd_mode[IPD_RDO_CNT],
#endif
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
{
	if (!isNS_allow)
		return 0;

	U3 cu_width_log2 = mod_info_curr_ptr->cu_width_log2;
	U3 cu_height_log2 = mod_info_curr_ptr->cu_height_log2;
	U7 cu_width = (1 << cu_width_log2);
	U7 cu_height = (1 << cu_height_log2);
	U14 sub_x = mod_info_curr_ptr->x_pos;
	U14 sub_y = mod_info_curr_ptr->y_pos;

	U13 pix_x = sub_x - md_input_ptr->pix_x;
	U13 pix_y = sub_y - md_input_ptr->pix_y;

	U4 bit_depth = md_fw_ptr->bit_depth_internal;
	U1 amvr_enable_flag = 1;
	int stride_org = 64;

	s64 dist_pred = MAX_S64;
	s32 dist_value = 0;
	int bit_cnt = 0;
	// s64 flag_cost[2] = { 0, 0 };
	s64 cost_best_temp = MAX_S64;
	s64 cost_best[1] = { MAX_S64 };
	s64 lambda_y = md_input_ptr->lambda_y;


	U1 valid_mode[17];//only mode1:9,10,11,12,13 is valid   //{SKIP:0,1,2,3;Skip_Merge:4,5,6,7,8;Mode1:9,10,11,12,13;Mode2:14,15,16};    //{Mode 0:(skip mode:fix 4, skip_merger:0~5), Mode1:(ME:AMVR:FIX 1/5); Mode 2(AFF_ME::FIX 1/3)

	pel orgY[8 * 8];
	for (int i = 0; i < cu_height; i++)
#pragma HLS LOOP_TRIPCOUNT max=8
		for (int j = 0; j < cu_width; j++)
#pragma HLS LOOP_TRIPCOUNT max=8
			orgY[(i << 3) + j] = p_fenc_LCU_Y[(mod_info_curr_ptr->y_pos - md_input_ptr->pix_y + i)][(mod_info_curr_ptr->x_pos - md_input_ptr->pix_x + j)];

	u8 ipred_list[IPRED_LIST_NUM];

	static pel src_le_temp[2 * 8 + 3];
	static pel src_up_temp[2 * 8 + 3];
	//pel src_le_temp[128 + 3];
	//pel src_up_temp[128 + 3];
	u16 avail_tb;

	static MERGE_RDO merge_rdo;
	static SKIP_RDO skip_rdo;
	static SKIP_ELEMENT skip_mode[SKIP_MODE_NUM];
	static SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO];
	static S14 sub_pel_mv[MV_D];
	static s16 mvp_all[2][5][2];

	u8 rdo_list[MAX_RDO_NUM_SIZE(8, 8)] = {}, inter_rdo_num[1] = { 0 }, intra_rdo_num[1] = { 0 };
#if ENABLE_BFRAME
	MV_DIR mv_dir;
#endif

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

	rdo_prepare_8(md_fw_ptr, 
		me_line_map_ptr_8x8,
		up_nebs, left_nebs, skip_in, mod_info_curr_ptr,
		&merge_rdo, md_input_ptr, valid_mode, amvr_enable_flag, fme_mv_info,
		IP_buffer_ptr_8x8,
		MPM_buffer_ptr_8x8, 
		IP_map_scu_ptr_8x8, 
		cu_width_log2, cu_height_log2, ipred_list, src_le_temp, src_up_temp, &avail_tb, rmd_mode,
		qt_depth, 17,
		cu_width, cu_height, 
#if ENABLE_BFRAME
		me_mv_dir, &mv_dir, 
#endif
		orgY, tree_status, curr_cons_pred_mode,
		inter_rdo_num, intra_rdo_num, rdo_list, skip_mode, &skip_rdo, skip_rdo_mode, sub_pel_mv, mvp_all, Fetch_Ref_window_ptr
#if SUB_TMVP
		, core_sbtmvp_ptr
#endif
	);


	u8 all_rdo_num = inter_rdo_num[0] + intra_rdo_num[0];

	static pel pred_y_skip_inter[13][8][8];
	static pel pred_y_inter2[MAX_RDO_NUM_INTER_SIZE(8, 8) - 1][8][8];
	static pel pred_y_intra[MAX_RDO_NUM_SIZE(8, 8)][8][8];

	static S14 mv_copy[2][2];
	static s16 mvd_copy[2][2];
	static S3 refi_copy[2];
#if 1
	get_ref_pel_8(mod_info_curr_ptr,
		skip_mode, tree_status, curr_cons_pred_mode, bit_depth,
		w, h, avail_tb,
		md_fw_ptr, &skip_rdo, merge_rdo, skip_rdo_mode,
		rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all,
		Fetch_Ref_window_ptr,
		md_input_ptr->CtrPos2MD,
#if ENABLE_BFRAME
		md_input_ptr->CtrPos2MD_ref1, &mv_dir,
#endif // ENABLE_BFRAME
		ref
		, mv_copy, mvd_copy, refi_copy, pred_y_skip_inter, pred_y_intra, pred_y_inter2, all_rdo_num
	);
#endif

#ifndef MD_CMOD
#if !DISABLE_RDO_SKIP
	rdo_skip_8(&cost_best[0], best_info_ptr,
		cu_data_ptr,
		mod_info_curr_ptr,
		orgY, skip_mode, md_fw_ptr->pic_width, md_fw_ptr->pic_height, md_fw_ptr->slice_type,
		tree_status, lambda_y, curr_cons_pred_mode, bit_depth, Fetch_Ref_window_ptr,
		md_input_ptr->CtrPos2MD, 
#if ENABLE_BFRAME
		md_input_ptr->CtrPos2MD_ref1,
#endif // ENABLE_BFRAME
		//fme2mdrefbuf
		pred_y_skip_inter
	);
#endif // DISABLE_RDO_SKIP
#endif

	if(PATCH_MODE())
		cost_best[0] = MAX_S64;
#if USE_INTRA_REFRESH
	if (md_input_ptr->EncRefresh &&md_fw_ptr->slice_type != SLICE_I)
	{
		if (md_input_ptr->RefreshMode == 0)
		{
			if (mod_info_curr_ptr->y_pos >= md_input_ptr->pirStartRow * 32 * md_input_ptr->RefreshSize && mod_info_curr_ptr->y_pos < md_input_ptr->pirEndRow * 32 * md_input_ptr->RefreshSize)
				cost_best[0] = MAX_S64;
		}
		else
		{
			if (mod_info_curr_ptr->x_pos >= md_input_ptr->pirStartCol * 32 * md_input_ptr->RefreshSize && mod_info_curr_ptr->x_pos < md_input_ptr->pirEndCol * 32 * md_input_ptr->RefreshSize)
				cost_best[0] = MAX_S64;
		}

	}
#endif

	//dummy Port alignment
#if 1
	pel pred_y_inter1[8][8];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			pred_y_inter1[i][j] = pred_y_skip_inter[12][i][j];
		}
	}
	U32 zero = (cu_width >= 4) ? (U32)0 : (U32)cu_width;
	U1 zero_1b = zero & 1;
	// disappear
#define TOUCH_OUT(a) cost_best[0] += (a)
#define TOUCH(a) (a) = (a) + zero;
#define BIT_WIDTH(a, bit) (a) = (a) + (zero_1b << ((bit)-1))

	TOUCH(curr_cons_pred_mode);
	TOUCH(best_info_ptr->affine_flag);
	TOUCH(best_info_ptr->cu_mode);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			TOUCH(best_info_ptr->ipm[i][j]);
		}
	}
	// TOUCH(best_info_ptr->umve_idx);

	TOUCH(cost_best[0]);
	//TOUCH(flag_cost[0]);
	//TOUCH(flag_cost[1]);

	TOUCH(md_fw_ptr->bit_depth_internal);
	TOUCH(md_fw_ptr->num_refp[0]);
	TOUCH(md_fw_ptr->num_refp[1]);
	TOUCH(md_fw_ptr->refp_ptr[0][0]);
	TOUCH(md_fw_ptr->refp_ptr[0][1]);
	TOUCH(refi_copy[0]);
	TOUCH(refi_copy[1]);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			TOUCH(mv_copy[i][j]);
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			TOUCH(mvd_copy[i][j]);
		}
	}

		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				TOUCH(pred_y_inter1[i][j]);
			}
		}

	for (int n = 0; n < 2; n++)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				TOUCH(pred_y_inter2[n][i][j]);
			}
		}
	}
	for (int n = 0; n < 5; n++)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				TOUCH(pred_y_intra[n][i][j]);
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			TOUCH(mod_info_curr_ptr->mpm[i][j]);
		}
	}
	TOUCH(mod_info_curr_ptr->mvp_from_hmvp_flag);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			TOUCH(mod_info_curr_ptr->num_nz[i][j]);
		}
	}
	TOUCH(mod_info_curr_ptr->pb_part);
	TOUCH(mod_info_curr_ptr->smvd_flag);


	BIT_WIDTH(md_fw_ptr->bit_depth_internal, 8);
	BIT_WIDTH(md_fw_ptr->num_refp[0], 8);
	BIT_WIDTH(md_fw_ptr->num_refp[1], 8);
	BIT_WIDTH(md_fw_ptr->refp_ptr[0][0], 32);
	BIT_WIDTH(md_fw_ptr->refp_ptr[0][1], 32);
	BIT_WIDTH(mod_info_curr_ptr->cu_height_log2, 8);
	BIT_WIDTH(mod_info_curr_ptr->cu_width_log2, 8);
	BIT_WIDTH(rdo_list[0], 8);
	BIT_WIDTH(rdoq_array->rdoq_est_cbf[0][0], 32);
	BIT_WIDTH(rdoq_array->rdoq_est_cbf[0][1], 32);


	BIT_WIDTH(skip_rdo.num_rdo, 8);
	
	for (int j = 0; j < 6; j++)
	{
		for (int m = 0; m < 12; m++)
		{
			for (int n = 0; n < 2; n++)
			{
				BIT_WIDTH(rdoq_array->rdoq_est_last[0][j][m][n], 32);
			}
		}
	}

	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			BIT_WIDTH(rdoq_array->rdoq_est_run[i][j], 32);
		}
	}
	for (int i = 0; i < 24; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			BIT_WIDTH(rdoq_array->rdoq_est_level[i][j], 32);
		}
	}

#endif

	rdo_luma_8_pipeline(all_rdo_num, w, h, tree_status, cu_width_log2, cu_height_log2, curr_cons_pred_mode, bit_depth, avail_tb,
		rdoq_array, md_fw_ptr, md_input_ptr, &skip_rdo, merge_rdo, skip_mode, skip_rdo_mode, mod_info_curr_ptr, best_info_ptr, 
		cu_data_ptr,
		rdo_list, valid_mode, ipred_list, src_le_temp, src_up_temp, sub_pel_mv, mvp_all, orgY, 
#if ENABLE_BFRAME
		&mv_dir, Fetch_Ref_window_ptr,
#endif
		/*flag_cost, */dist_value,
		&cost_best[0],
		//fme2mdrefbuf,
		mv_copy, mvd_copy, refi_copy, pred_y_inter1, pred_y_inter2, pred_y_intra
	);

	return cost_best[0];
}


void copy_to_cu_data_luma_8(U1 isNS_allow, U8 w, U8 h, U6 qp_y, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	MD_COM_MODE_BEST* mod_info_best, U2 tree_status,
	int slice_num, int affine_subblock_size_idx, s8* coef_rec_write_flag, U2 NS_luma_pred_mode[1]
#if CUDQP_QP_MAP
	,int cu_dqp_enable, int cu_qp_group_pred_qp
#endif
)
{
	if (!isNS_allow)
	{
		NS_luma_pred_mode[0] = ONLY_INTRA;
		return;
	}
	u8 i;
	u8 j;
	u8 k;
	u8 wxh = w * h;
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

	int pel_x = mod_info_best->x_scu * 4;
	int pel_y = mod_info_best->y_scu * 4;
	int pel_x_top = (mod_info_best->x_scu * 4 - (((mod_info_best->x_scu * 4) >> 6) << 6)) >> 2;
	int pel_y_top = (mod_info_best->y_scu * 4 - (((mod_info_best->y_scu * 4) >> 6) << 6)) >> 2;
	
	//if (mod_info_best->cu_mode != MODE_INTRA)
	//{
	//	*coef_rec_write_flag = 1;
	//}
	//else
	//	*coef_rec_write_flag = 0;

	if (tree_status != TREE_C) {
//		if (*coef_rec_write_flag == 0) {
//			for (i = 0; i < wxh; i++) {
//#pragma HLS LOOP_TRIPCOUNT max=64
//				cu_data->coef_y[i] = mod_info_best->coef_y[i];
//				cu_data->reco_y[i] = mod_info_best->rec_y[i];
//			}
//		}
		num_luma_tb = get_part_num((PART_SIZE)(mod_info_best->tb_part));
		for (i = 0; i < h; i++) {
			for(j = 0; j < w; j++)
#pragma HLS LOOP_TRIPCOUNT max=64
				num_coef_y += (((int)cu_data_ptr->coef_y[i][j]) != 0 ? 1 : 0);
		}
		for (i = 0; i < num_luma_tb; i++) {
#pragma HLS LOOP_TRIPCOUNT max=4
			num_nnz_sum += mod_info_best->num_nz[i][0];
		}
	}
	
	if (tree_status == TREE_LC) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	else if (tree_status == TREE_L) {
		cu_cbf_flag = (num_coef_y == 0 ? 0 : 1);
	}
	
	if (tree_status == TREE_C) {
		idx = 0;
		for (j = 0; j < h >> 2; j++) {
#pragma HLS LOOP_TRIPCOUNT max=2
			for (i = 0; i < w >> 2; i++) {
#pragma HLS LOOP_TRIPCOUNT max=2
				if (mod_info_best->cu_mode == 0) {
					cu_data_ptr->ipm[1][idx + i] = mod_info_best->ipm[0][1];
				}
				for (k = 1; k < 3; k++) {
					cu_data_ptr->num_nz_coef[k][idx + i] = mod_info_best->num_nz[0][k];
				}
			}
			idx += w >> 2;
		}
	}
	else {
		idx = 0;
		for (j = 0; j < h >> 2; j++) {
#pragma HLS LOOP_TRIPCOUNT max=2
			for (i = 0; i < w >> 2; i++) {
#pragma HLS LOOP_TRIPCOUNT max=2
				pb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->pb_part), i << 2, j << 2, w, h);
				tb_idx_y = get_part_idx((PART_SIZE)(mod_info_best->tb_part), i << 2, j << 2, w, h);
				pb_idx_u = 0;
				tb_idx_u = 0;
				cu_data_ptr->pred_mode[idx + i] = ((u8)(mod_info_best->cu_mode));
				cu_data_ptr->umve_flag[idx + i] = mod_info_best->umve_flag;
				cu_data_ptr->umve_idx[idx + i] = mod_info_best->umve_idx;
				cu_data_ptr->pb_part[idx + i] = mod_info_best->pb_part;
				cu_data_ptr->tb_part[idx + i] = mod_info_best->tb_part;
#if CUDQP_QP_MAP
				cu_data_ptr->cu_luma_qp[idx + i] = cu_qp_group_pred_qp;
				//cu_data_ptr->luma_pred_qp[idx + i] = qp_y;
#endif
				for (k = 0; k < 3; k++) {
					cu_data_ptr->num_nz_coef[k][idx + i] = mod_info_best->num_nz[(k == 0 ? tb_idx_y : tb_idx_u)][k];
				}
				cu_data_ptr->map_scu[idx + i] = (cu_data_ptr->map_scu[idx + i] & 0xFF807F80) | ((unsigned int)(slice_num & 0x7F)) | (((
					unsigned int)qp_y << 16)) | ((unsigned int)((mod_info_best->cu_mode == 0) << 15)) | ((unsigned int)(1 << 31));
				if (mod_info_best->cu_mode == 2) {
					cu_data_ptr->map_scu[idx + i] = cu_data_ptr->map_scu[idx + i] | ((unsigned int)(1 << 23));
				}
				else {
					cu_data_ptr->map_scu[idx + i] = cu_data_ptr->map_scu[idx + i] & ((unsigned int)(~(1 << 23)));
				}
				cu_data_ptr->depth[idx + i] = ((s8)(mod_info_best->cud));
				cu_data_ptr->affine_flag[idx + i] = mod_info_best->affine_flag;
				if (mod_info_best->affine_flag) {
					cu_data_ptr->map_scu[idx + i] = (cu_data_ptr->map_scu[idx + i] & 0xFFFFFCFF) | ((unsigned int)((((int)(
						mod_info_best->affine_flag)) & 0x03) << 8));
				}
				else {
					cu_data_ptr->map_scu[idx + i] = cu_data_ptr->map_scu[idx + i] & 0xFFFFFCFF;
				}
				if (cu_cbf_flag) {
					cu_data_ptr->map_scu[idx + i] = cu_data_ptr->map_scu[idx + i] | ((unsigned int)(1 << 24));
				}
				else {
					cu_data_ptr->map_scu[idx + i] = cu_data_ptr->map_scu[idx + i] & ((unsigned int)(~(1 << 24)));
				}
#if CUDQP_QP_MAP
				if (cu_dqp_enable && !cu_cbf_flag)
				{
					//change qp to pred_qp if no residual
					MCU_SET_QP(cu_data_ptr->map_scu[idx + i], cu_qp_group_pred_qp);
					//assert(ctx->cu_qp_group.pred_qp >= 0 && ctx->cu_qp_group.pred_qp <= MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
				}
#endif
				cu_data_ptr->map_cu_mode[idx + i] = (cu_data_ptr->map_cu_mode[idx + i] & 0xFFFFFFF0) | ((unsigned int)((pel_x_top & 0xF) << 0));
				cu_data_ptr->map_cu_mode[idx + i] = (cu_data_ptr->map_cu_mode[idx + i] & 0xFFFFFF0F) | ((unsigned int)((pel_y_top & 0xF) << 4));
				cu_data_ptr->map_cu_mode[idx + i] = (cu_data_ptr->map_cu_mode[idx + i] & 0xFFFF00FF) | ((unsigned int)((i & 0xFF) << 8));
				cu_data_ptr->map_cu_mode[idx + i] = (cu_data_ptr->map_cu_mode[idx + i] & 0xFF00FFFF) | ((unsigned int)((j & 0xFF) << 16));
				cu_data_ptr->map_cu_mode[idx + i] = (cu_data_ptr->map_cu_mode[idx + i] & 0xF0FFFFFF) | ((unsigned int)((
					cu_width_log2 & 0x0F) << 24));
				cu_data_ptr->map_cu_mode[idx + i] = (cu_data_ptr->map_cu_mode[idx + i] & 0x0FFFFFFF) | ((unsigned int)((
					cu_height_log2 & 0x0F) << 28));
				cu_data_ptr->map_pb_tb_part[idx + i] = (cu_data_ptr->map_pb_tb_part[idx + i] & 0xFFFFFF00) | ((unsigned int)((
					mod_info_best->tb_part & 0xFF) << 0));
				if (mod_info_best->cu_mode == 0) {
					cu_data_ptr->mpm[0][idx + i] = mod_info_best->mpm[pb_idx_y][0];
					cu_data_ptr->mpm[1][idx + i] = mod_info_best->mpm[pb_idx_y][1];
					cu_data_ptr->ipm[0][idx + i] = mod_info_best->ipm[pb_idx_y][0];
					cu_data_ptr->ipm[1][idx + i] = mod_info_best->ipm[pb_idx_u][1];
					cu_data_ptr->ipf_flag[idx + i] = mod_info_best->ipf_flag;
					cu_data_ptr->mv[idx + i][0][0] = (0);
					cu_data_ptr->mv[idx + i][0][1] = (0);
					cu_data_ptr->mv[idx + i][1][0] = (0);
					cu_data_ptr->mv[idx + i][1][1] = (0);
					cu_data_ptr->mvd[idx + i][0][0] = ((s16)0);
					cu_data_ptr->mvd[idx + i][0][1] = ((s16)0);
					cu_data_ptr->mvd[idx + i][1][0] = ((s16)0);
					cu_data_ptr->mvd[idx + i][1][1] = ((s16)0);
					cu_data_ptr->refi[idx + i][0] = (-1);
					cu_data_ptr->refi[idx + i][1] = (-1);
					cu_data_ptr->skip_idx[idx + i] = 0;
					cu_data_ptr->smvd_flag[idx + i] = 0;
				}
				else {
					cu_data_ptr->mpm[0][idx + i] = 0;
					cu_data_ptr->mpm[1][idx + i] = 0;
					cu_data_ptr->ipm[0][idx + i] = 0;
					cu_data_ptr->ipm[1][idx + i] = 0;
					cu_data_ptr->refi[idx + i][0] = mod_info_best->refi[0];
					cu_data_ptr->refi[idx + i][1] = mod_info_best->refi[1];
					cu_data_ptr->mvr_idx[idx + i] = mod_info_best->mvr_idx;
					cu_data_ptr->mvp_from_hmvp_flag[idx + i] = mod_info_best->mvp_from_hmvp_flag;
					cu_data_ptr->smvd_flag[idx + i] = mod_info_best->smvd_flag;
					cu_data_ptr->skip_idx[idx + i] = mod_info_best->skip_idx;
					cu_data_ptr->mv[idx + i][0][0] = mod_info_best->mv[0][0];
					cu_data_ptr->mv[idx + i][0][1] = mod_info_best->mv[0][1];
					cu_data_ptr->mv[idx + i][1][0] = mod_info_best->mv[1][0];
					cu_data_ptr->mv[idx + i][1][1] = mod_info_best->mv[1][1];
					cu_data_ptr->mvd[idx + i][0][0] = mod_info_best->mvd[0][0];
					cu_data_ptr->mvd[idx + i][0][1] = mod_info_best->mvd[0][1];
					cu_data_ptr->mvd[idx + i][1][0] = mod_info_best->mvd[1][0];
					cu_data_ptr->mvd[idx + i][1][1] = mod_info_best->mvd[1][1];
				}

			}
			idx += w >> 2;
		}
		//if (mod_info_best->affine_flag) {
		//	enc_set_affine_mvf(affine_subblock_size_idx, cu_data, mod_info_best);
		//}
	}

	NS_luma_pred_mode[0] = cu_data_ptr->pred_mode[0];
}

#if DEBUG_MD_8
#ifdef WIN32
static FilePrinter fp = FilePrinter("D:\\working\\bframe\\data\\md8", -1, -1);
static FileReader fr = FileReader("D:\\working\\bframe\\data\\md8", -1);
#else
static FilePrinter fp = FilePrinter("/home/zhuxz/avs3ip/bframe/bframe/data", -1, -1);
#endif
#endif

int mode_cu_init_16_8(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* bst_info_ptr,
	/*COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info, COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,*/
	U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status,
	U2* cons_pred_mode, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr)
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
#pragma HLS LOOP_TRIPCOUNT max=256
			cu_data_ptr->ipm[0][m] = 0;
			cu_data_ptr->ipm[1][m] = 0;
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

	return 0;
}


s64 init_basic_leaf_16_8(U1 isNS_allow, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, s64 lambdaY, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, MD_COM_MODE_BEST* bst_info_ptr, 
	U3 qt_depth, U3 bet_depth, U13 x0, U13 y0, U9 cup, U2 tree_status,
	U2* cons_pred_mode, MD_COM_MODE* mod_info_curr_temp_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr)
{
	if (!isNS_allow)
		return MAX_S64;

	U1 split_allow[6];
	U8 cu_width = (1 << cu_width_log2);
	U8 cu_height = (1 << cu_height_log2);
	int bit_cnt;
	s64 cost_temp = 0;
	int pos = 0 + ((cu_height >> 1 >> 2) * (cu_width >> 2) + (cu_width >> 1 >> 2));
	int shape = (SQUARE)+(com_tbl_log2[cu_width] - com_tbl_log2[cu_height]);
	U13 pic_width = md_fw_ptr->pic_width;
	U13 pic_height = md_fw_ptr->pic_height;
	U1 boundary = 0;
	U1 boundary_b = 0;
	U1 boundary_r = 0;
	//int bit_rate = 0;
	//cu_data_temp_ptr->split_mode[cud][shape][pos] = NO_SPLIT;
	//bit_rate++;
	boundary_b = boundary && y0 + cu_height > pic_height && !(x0 + cu_width > pic_width);
	boundary_r = boundary && x0 + cu_width > pic_width && !(y0 + cu_height > pic_height);
	com_check_split_mode_ECO(md_fw_ptr, split_allow, com_tbl_log2[cu_width], com_tbl_log2[cu_height],
		boundary, boundary_b, boundary_r, qt_depth, bet_depth);

	if((cu_height_log2 >=4 || cu_width_log2 >= 4)&& split_allow[SPLIT_QUAD])
		bit_cnt = 2;
	else
		bit_cnt = 1;
	cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	mode_cu_init_16_8(rdoq_model, model_cnt, rdoq_array, mod_info_curr_temp_ptr, bst_info_ptr, x0, y0, cu_width_log2, cu_height_log2, cud, md_fw_ptr->pic_width_in_scu,
		tree_status, cons_pred_mode, cu_data_ptr);
	return cost_temp;
}

extern int md_slice_qp;//for debug
#if !MD_KERNEL_8_DUMMY
#if USE_FACADE_MD_8
s64 md_kernel_8_org(U8 width, U8 height, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, u8 rmd_mode[IPD_RDO_CNT],
	pel RefWindowLCU[SWW * SWH], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_ref1[SWW * SWH], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, pel refwin_md_ref1[88][88], S14 CtrPosFME_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0,
	U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U2 tree_status, ENC_CU_DATA_ARRAY_INNER* cu_data_temp_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, 
	pel refwin_md[88][88], S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window* Fetch_Ref_window_ptr)
#else
s64 md_kernel_8(U8 width, U8 height, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
#if USE_SPEED_LEVEL
	u8 rmd_mode[IPD_RDO_CNT + EXTRA_RDO_MODE],
#else
	u8 rmd_mode[IPD_RDO_CNT],
#endif
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<8>* me_line_map_ptr_8x8,
	ENC_ME_NEB_INFO up_nebs[4],
	ENC_ME_NEB_INFO left_nebs[2],
	SKIP_MERGE_INPUT* skip_in, 
	U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0, U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, 
	ENC_CU_DATA_ARRAY_SIZE<8>* cu_data_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	MPM_ARRAY_SIZE<8, 8>* MPM_buffer_ptr_8x8, 
	IP_MAP_SCU_SIZE<8, 8>*	IP_map_scu_ptr_8x8, 
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr)
#endif
{
#if DISABLE_CU_8
	return MAX_S64;
#endif

#if DEBUG_MD_8 & 0
	if (x0 == 120 && y0 == 848 && cu_width_log2 == 3 && cu_height_log2 == 3 && cud == 3 && pic_cnt == 10 && qt_depth == 3 && bet_depth == 0)
	{
		printf("[%s][StartKernel]\n", __FUNCTION__);
		printf("tree_status:%d, isNS_allow:%d, cons_pred_mode:%d\n", tree_status, isNS_allow, cons_pred_mode);
		fp.printAtribute("width", sizeof(U8), &width, -1);
		fp.printAtribute("height", sizeof(U8), &height, -1);
		fp.printAtribute("rdoq_model", sizeof(RDOQ_MODEL), rdoq_model, -1);
		fp.printAtribute("model_cnt", sizeof(RDOQ_MODEL), model_cnt, -1);
		fp.printAtribute("md_fw_ptr", sizeof(MD_FW), md_fw_ptr, -1);
		fp.printAtribute("md_input_ptr", sizeof(MD_INPUT), md_input_ptr, -1);
		fp.printAtribute("rmd_mode", sizeof(u8) * IPD_RDO_CNT, rmd_mode, -1);
		fp.printAtribute("RefWindowLCU", sizeof(pel) * SWW * SWH, RefWindowLCU, -1);
		fp.printAtribute("CtrPos", sizeof(S14) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPos, -1);
		fp.printAtribute("RefWindowLCU_ref1", sizeof(pel) * SWW * SWH, RefWindowLCU_ref1, -1);
		fp.printAtribute("CtrPos_ref1", sizeof(pel) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPos_ref1, -1);
		fp.printAtribute("me_mv_dir", sizeof(ME_MV_DIR) * 1, me_mv_dir, -1);
		fp.printAtribute("refwin_md_ref1", sizeof(pel) * 88 * 88, refwin_md_ref1, -1);
		fp.printAtribute("CtrPosFME_ref1", sizeof(pel) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPosFME_ref1, -1);
		fp.printAtribute("p_fenc_LCU_Y", sizeof(pel) * MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH, p_fenc_LCU_Y, -1);
		fp.printAtribute("me_line_map_ptr", sizeof(ENC_ME_LINE_MAP) * 1, me_line_map_ptr, -1);
		fp.printAtribute("inner_nebs", sizeof(ENC_ME_NEB_INFO) * 2 * SCU_IN_LCU_NUMS_LEN * SCU_IN_LCU_NUMS_LEN, inner_nebs, -1);
		fp.printAtribute("above_nebs_line", sizeof(ENC_ME_NEB_INFO) * ABOVE_LINE_LEN, above_nebs_line, -1);
		fp.printAtribute("skip_in", sizeof(SKIP_MERGE_INPUT) * 1, skip_in, -1);
		fp.printAtribute("boundary", sizeof(U1) * 1, &boundary, -1);
		fp.printAtribute("isNS_allow", sizeof(U1), &isNS_allow, -1);
		fp.printAtribute("NS_luma_pred_mode", sizeof(U2), &NS_luma_pred_mode[0], -1);
		fp.printAtribute("x0", sizeof(U13), &x0, -1);
		fp.printAtribute("y0", sizeof(U13), &y0, -1);
		fp.printAtribute("cup", sizeof(U12), &cup, -1);
		fp.printAtribute("cu_width_log2", sizeof(U3), &cu_width_log2, -1);
		fp.printAtribute("cu_height_log2", sizeof(U3), &cu_height_log2, -1);
		fp.printAtribute("cud", sizeof(U4), &cud, -1);
		fp.printAtribute("qt_depth", sizeof(U3), &qt_depth, -1);
		fp.printAtribute("bet_depth", sizeof(U3), &bet_depth, -1);
		fp.printAtribute("cons_pred_mode", sizeof(U2), &cons_pred_mode, -1);
		fp.printAtribute("tree_status", sizeof(U2), &tree_status, -1);
		fp.printAtribute("cu_data_temp_ptr", sizeof(ENC_CU_DATA_ARRAY), cu_data_temp_ptr, -1);
		fp.printAtribute("fme_mv_info", sizeof(ENC_FME_INFO_ALL), fme_mv_info, -1);
		fp.printAtribute("IP_buffer_ptr", sizeof(IP_ARRAY), IP_buffer_ptr, -1);
		fp.printAtribute("MPM_buffer_ptr", sizeof(MPM_ARRAY), MPM_buffer_ptr, -1);
		fp.printAtribute("MAP_SCU_IP_ptr", sizeof(IP_MAP_SCU), MAP_SCU_IP_ptr, -1);
		fp.printAtribute("best_affine_mv", sizeof(S14) * 2 * 4 * 2,  -1);
		fp.printAtribute("refwin_md", sizeof(pel) * 88 * 88, refwin_md, -1);
		fp.printAtribute("CtrPosFME", sizeof(S14) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPosFME, -1);
		fp.printAtribute("Fetch_Ref_window_ptr", sizeof(strFetch_ref_window) * 1, Fetch_Ref_window_ptr, -1);
		isProblem = 1;
	}
#endif

	s64 ns_cost = (s64)MAX_S64;
	static MD_COM_MODE mod_info_curr_ptr;
	static MD_COM_MODE_BEST best_info_ptr;
	//COM_PART_INFO mod_info_curr_pb_info;
	//COM_PART_INFO mod_info_curr_tb_info;
	//COM_PART_INFO bst_info_pb_info;
	//COM_PART_INFO bst_info_tb_info;

	s8 coef_rec_write_flag = 0;

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
	mod_info_curr_ptr.x_chroma = cu_data_ptr->x_chroma;
	mod_info_curr_ptr.y_chroma = cu_data_ptr->y_chroma;
	mod_info_curr_ptr.last_child = cu_data_ptr->last_child;

	if (!boundary) {
#if CUDQP_QP_MAP
		if (md_input_ptr->cu_dqp_enable) {
			u32 luma_map_scu;
			int scu_stride = 16;
			int x_scu_in_LCU = (x0 % 64) >> 2;
			int y_scu_in_LCU = (y0 % 64) >> 2;
			int luma_cup;
			U6 cu_w_scu = width >> 2;
			U6 cu_h_scu = height >> 2;
			luma_cup = (y_scu_in_LCU + (cu_h_scu - 1)) * scu_stride + (x_scu_in_LCU + (cu_w_scu - 1));

			md_input_ptr->qp_y = md_input_ptr->CuQPMap[luma_cup];
			md_input_ptr->cu_qp_group_pred_qp = md_input_ptr->qp_y;
		}
		//assert(core->qp_y >= 0 && core->qp_y <= MAX_QUANT_BASE + ctx->info.qp_offset_bit_depth);
#endif
		ns_cost = init_basic_leaf_16_8(isNS_allow, &rdoq_model_t, &model_cnt_t, &rdoq_array, md_fw_ptr, md_input_ptr->lambda_y, cu_width_log2, cu_height_log2, cud, &best_info_ptr, qt_depth, bet_depth, x0, y0, cup, tree_status, &cons_pred_mode,
			&mod_info_curr_ptr, cu_data_ptr);

		ns_cost += rdo_luma_8(isNS_allow, width, height, &rdoq_array, md_fw_ptr, 
			me_line_map_ptr_8x8,
			up_nebs, left_nebs,
			skip_in, 
			tree_status, &best_info_ptr, md_input_ptr, p_fenc_LCU_Y,
#if ENABLE_BFRAME
			me_mv_dir, 
#endif
			&mod_info_curr_ptr, 
			cu_data_ptr,
			cons_pred_mode, cup, qt_depth, 
			fme_mv_info, rmd_mode,
			IP_buffer_ptr_8x8,
			MPM_buffer_ptr_8x8,
			IP_map_scu_ptr_8x8, 
			ref,
			Fetch_Ref_window_ptr);

		copy_to_cu_data_luma_8(isNS_allow, width, height, md_input_ptr->qp_y, cu_data_ptr, &best_info_ptr, tree_status, 0, 0, &coef_rec_write_flag, NS_luma_pred_mode
#if CUDQP_QP_MAP
			,md_input_ptr->cu_dqp_enable, md_input_ptr->cu_qp_group_pred_qp
#endif
		);

	}
#if DEBUG_MD_8
	if (isProblem)
		isProblem = 0;
#endif
	//printf("pos=(%d %d) size=(%d %d) cost=%lld\n", x0, y0, 1 << cu_width_log2, 1 << cu_height_log2, ns_cost);
	
	return ns_cost;
}
#endif

#if USE_FACADE_MD_8
#if !MD_KERNEL_8_DUMMY
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
	U2 tree_status, ENC_CU_DATA_ARRAY_INNER* cu_data_temp_ptr,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, 
	pel refwin_md[88][88], S14 CtrPosFME[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window* Fetch_Ref_window_ptr)
{
	s64 re = 0;

	U8 width_in; U8 height_in;
	static RDOQ_MODEL rdoq_model_in;
	static RDOQ_MODEL model_cnt_in;
	static MD_FW md_fw_ptr_in;
	static MD_INPUT md_input_ptr_in;
	static u8 rmd_mode_in[IPD_RDO_CNT];
	static pel RefWindowLCU_in[SWW * SWH];
	static S14 CtrPos_in[RANGE_NUM][DIR_DIM][MAP_DIMS];
#if ENABLE_BFRAME
	static pel RefWindowLCU_ref1_in[SWW * SWH];
	static S14 CtrPos_ref1_in[RANGE_NUM][DIR_DIM][MAP_DIMS];
	static ME_MV_DIR me_mv_dir_in;
	static pel refwin_md_ref1_in[88][88];
	static S14 CtrPosFME_ref1_in[RANGE_NUM][DIR_DIM][MAP_DIMS];
#endif
	static pel p_fenc_LCU_Y_in[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH];
	static ENC_ME_LINE_MAP me_line_map_ptr_in;
	static ENC_ME_NEB_INFO inner_nebs_in[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	static ENC_ME_NEB_INFO above_nebs_line_in[ABOVE_LINE_LEN];
	static SKIP_MERGE_INPUT skip_in_in;
	U1 boundary_in; U1 isNS_allow_in; U2 NS_luma_pred_mode_in[1]; U13 x0_in;
	U13 y0_in; U12 cup_in; U3 cu_width_log2_in; U3 cu_height_log2_in; U4 cud_in; U3 qt_depth_in; U3 bet_depth_in; U2 cons_pred_mode_in;
	U2 tree_status_in;
	static ENC_CU_DATA_ARRAY cu_data_temp_ptr_in;
	static ENC_FME_INFO_ALL fme_mv_info_in;
	static IP_ARRAY IP_buffer_ptr_in;
	static MPM_ARRAY MPM_buffer_ptr_in;
	static IP_MAP_SCU MAP_SCU_IP_ptr_in;
	static S14 best_affine_mv_in[2][4][2];
	static pel refwin_md_in[88][88];
	static S14 CtrPosFME_in[RANGE_NUM][DIR_DIM][MAP_DIMS];
	static strFetch_ref_window Fetch_Ref_window_ptr_in;

	fr.readAtribute("width", sizeof(U8), &width_in);
	fr.readAtribute("height", sizeof(U8), &height_in);
	fr.readAtribute("rdoq_model", sizeof(RDOQ_MODEL), &rdoq_model_in);
	fr.readAtribute("model_cnt", sizeof(RDOQ_MODEL), &model_cnt_in);
	fr.readAtribute("md_fw_ptr", sizeof(MD_FW), &md_fw_ptr_in);
	fr.readAtribute("md_input_ptr", sizeof(MD_INPUT), &md_input_ptr_in);
	fr.readAtribute("rmd_mode", sizeof(u8) * IPD_RDO_CNT, rmd_mode_in);
	fr.readAtribute("RefWindowLCU", sizeof(pel) * SWW * SWH, RefWindowLCU_in);
	fr.readAtribute("CtrPos", sizeof(S14) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPos_in);
	fr.readAtribute("RefWindowLCU_ref1", sizeof(pel) * SWW * SWH, RefWindowLCU_ref1_in);
	fr.readAtribute("CtrPos_ref1", sizeof(pel) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPos_ref1_in);
	fr.readAtribute("me_mv_dir", sizeof(ME_MV_DIR) * 1, &me_mv_dir_in);
	fr.readAtribute("refwin_md_ref1", sizeof(pel) * 88 * 88, refwin_md_ref1_in);
	fr.readAtribute("CtrPosFME_ref1", sizeof(pel) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPosFME_ref1_in);
	fr.readAtribute("p_fenc_LCU_Y", sizeof(pel) * MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH, p_fenc_LCU_Y_in);
	fr.readAtribute("me_line_map_ptr", sizeof(ENC_ME_LINE_MAP) * 1, &me_line_map_ptr_in);
	fr.readAtribute("inner_nebs", sizeof(ENC_ME_NEB_INFO) * 2 * SCU_IN_LCU_NUMS_LEN * SCU_IN_LCU_NUMS_LEN, inner_nebs_in);
	fr.readAtribute("above_nebs_line", sizeof(ENC_ME_NEB_INFO) * ABOVE_LINE_LEN, above_nebs_line_in);
	fr.readAtribute("skip_in", sizeof(SKIP_MERGE_INPUT) * 1, &skip_in_in);
	fr.readAtribute("boundary", sizeof(U1) * 1, &boundary_in);
	fr.readAtribute("isNS_allow", sizeof(U1), &isNS_allow_in);
	fr.readAtribute("NS_luma_pred_mode", sizeof(U2), &NS_luma_pred_mode_in[0]);
	fr.readAtribute("x0", sizeof(U13), &x0_in);
	fr.readAtribute("y0", sizeof(U13), &y0_in);
	fr.readAtribute("cup", sizeof(U12), &cup_in);
	fr.readAtribute("cu_width_log2", sizeof(U3), &cu_width_log2_in);
	fr.readAtribute("cu_height_log2", sizeof(U3), &cu_height_log2_in);
	fr.readAtribute("cud", sizeof(U4), &cud_in);
	fr.readAtribute("qt_depth", sizeof(U3), &qt_depth_in);
	fr.readAtribute("bet_depth", sizeof(U3), &bet_depth_in);
	fr.readAtribute("cons_pred_mode", sizeof(U2), &cons_pred_mode_in);
	fr.readAtribute("tree_status", sizeof(U2), &tree_status_in);
	fr.readAtribute("cu_data_temp_ptr", sizeof(ENC_CU_DATA_ARRAY), &cu_data_temp_ptr_in);
	fr.readAtribute("fme_mv_info", sizeof(ENC_FME_INFO_ALL), &fme_mv_info_in);
	fr.readAtribute("IP_buffer_ptr", sizeof(IP_ARRAY), &IP_buffer_ptr_in);
	fr.readAtribute("MPM_buffer_ptr", sizeof(MPM_ARRAY), &MPM_buffer_ptr_in);
	fr.readAtribute("MAP_SCU_IP_ptr", sizeof(IP_MAP_SCU), &MAP_SCU_IP_ptr_in);
	fr.readAtribute("best_affine_mv", sizeof(S14) * 2 * 4 * 2, best_affine_mv_in);
	fr.readAtribute("refwin_md", sizeof(pel) * 88 * 88, refwin_md_in);
	fr.readAtribute("CtrPosFME", sizeof(S14) * RANGE_NUM * DIR_DIM * MAP_DIMS, CtrPosFME_in);
	fr.readAtribute("Fetch_Ref_window_ptr", sizeof(strFetch_ref_window) * 1, &Fetch_Ref_window_ptr_in);

	re = md_kernel_8_org(width_in , height_in,&rdoq_model_in, &model_cnt_in, &md_fw_ptr_in, &md_input_ptr_in, rmd_mode_in,
		RefWindowLCU_in, CtrPos_in,
#if ENABLE_BFRAME
		RefWindowLCU_ref1_in, CtrPos_ref1_in, &me_mv_dir_in, refwin_md_ref1_in, CtrPosFME_ref1_in,
#endif
		p_fenc_LCU_Y_in,
		&me_line_map_ptr_in,
		inner_nebs_in, above_nebs_line_in,
		&skip_in_in, boundary_in, isNS_allow_in, NS_luma_pred_mode_in, x0_in,
		y0_in, cup_in, cu_width_log2_in, cu_height_log2_in, cud_in, qt_depth_in, bet_depth_in, cons_pred_mode_in,
		tree_status_in, &cu_data_temp_ptr_in,
		&fme_mv_info_in,
		&IP_buffer_ptr_in, &MPM_buffer_ptr_in, &MAP_SCU_IP_ptr_in, best_affine_mv_in,
		refwin_md_in, CtrPosFME_in, &Fetch_Ref_window_ptr_in);

	return re;
}
#endif
#endif
