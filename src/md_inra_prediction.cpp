#include "com_def.h"
#include <math.h>
#include <stdlib.h>
#include "md1.h"

void ipred_vert_4(pel *src_up, pel dst[4 * 4], int w, int h)
{

#pragma HLS INLINE 
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
	int i;
	int j;
	for (i = 0; i < 4; i++) {

#pragma HLS UNROLL
		for (j = 0; j < 4; j++) {

#pragma HLS UNROLL
			dst[j] = src_up[j];
		}
		dst += w;
	}
}

void ipred_hor_4(pel src_le[11], pel dst[11], int w, int h)
{

#pragma HLS INLINE 
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
	int i;
	int j;
	{
		for (i = 0; i < 4; i++) {

#pragma HLS UNROLL
			for (j = 0; j < 4; j++) {

#pragma HLS UNROLL
				dst[j] = src_le[0];
			}
			dst += w;
			src_le++;
		}
	}
}

void ipred_dc_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], int w, int h, int bit_depth, u16 avail_cu)
{
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
#pragma HLS INLINE
#if MD_PRINT
	((int)com_tbl_log2[w]) >= 2 ? ((void)0) : __assert_HLS("com_tbl_log2[w] >= 2", "../separate/com_ipred.c",
		(unsigned int)1489, __FUNCTION__);
	((int)com_tbl_log2[h]) >= 2 ? ((void)0) : __assert_HLS("com_tbl_log2[h] >= 2", "../separate/com_ipred.c",
		(unsigned int)1490, __FUNCTION__);
#endif
	int dc = 0;
	int wh;
	int i;
	int j;
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1

	if ((((int)avail_cu) & 1 << 1) == 1 << 1) {
		for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
			dc += ((int)src_le[i]);
		}
		if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
			for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
				dc += ((int)src_up[j]);
			}
//			dc = ((dc + ((w + h) >> 1)) * (4096 / (w + h))) >> 12;
			dc = ((dc + 4) * (4096 / 8)) >> 12;
		}
		else {
//			dc = (dc + (h >> 1)) >> ((int)com_tbl_log2[h]);
			dc = (dc + 2) >> 2;
		}
	}
	else if ((((int)avail_cu) & 1 << 0) == 1 << 0) {
		for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
			dc += ((int)src_up[j]);
		}
//		dc = (dc + (w >> 1)) >> ((int)com_tbl_log2[w]);
			dc = (dc + 2) >> 2;
	}
	else {
		dc = 1 << (bit_depth - 1);
	}
	wh = w * h;

	for (i = 0; i < 16; i++) {
#pragma HLS UNROLL
		dst[i] = ((pel)dc);
	}
}

void ipred_plane_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], U4 bit_depth, U7 w, U7 h)
{
#pragma HLS INLINE 
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

#if MD_PRINT
	((int)com_tbl_log2[w]) >= 2 ? ((void)0) : __assert_HLS("com_tbl_log2[w] >= 2", "../separate/com_ipred.c",
		(unsigned int)1539, __FUNCTION__);
	((int)com_tbl_log2[h]) >= 2 ? ((void)0) : __assert_HLS("com_tbl_log2[h] >= 2", "../separate/com_ipred.c",
		(unsigned int)1540, __FUNCTION__);
#endif
	//pel *rsrc;
	int coef_h = 0;
	int coef_v = 0;
	int a;
	int b;
	int c;
	int x;
	int y;
	U6 w2 = w >> 1;
	U6 h2 = h >> 1;
	int ib_mult[5] = { (13), (17), (5), (11), (23) };
	int ib_shift[5] = { (7), (10), (11), (15), (19) };
	int idx_w = ((int)com_tbl_log2[w]) - 2;
	int idx_h = ((int)com_tbl_log2[h]) - 2;
	int im_h;
	int is_h;
	int im_v;
	int is_v;
	int temp;
	int temp2;
	im_h = ib_mult[idx_w];
	is_h = ib_shift[idx_w];
	im_v = ib_mult[idx_h];
	is_v = ib_shift[idx_h];
	//rsrc = src_up + (w2 - 1);
	//rsrc = src_up;
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1

	for (x = 1; x < 2 + 1; x++) {
#pragma HLS UNROLL
		coef_h += x * (((int)src_up[w2 + x]) - ((int)src_up[w2 - x]));
	}
	//rsrc = src_le + (h2 - 1);
	//rsrc = src_le;
	for (y = 1; y < 2 + 1; y++) {
#pragma HLS UNROLL
		coef_v += y * (((int)src_le[h2 + y]) - ((int)src_le[h2 - y]));
	}
	a = (((int)src_le[h]) + ((int)src_up[w])) << 4;
	b = ((coef_h << 5) * im_h + (1 << (is_h - 1))) >> is_h;
	c = ((coef_v << 5) * im_v + (1 << (is_v - 1))) >> is_v;
	temp = a - (h2 - 1) * c - (w2 - 1) * b + 16;
	for (y = 0; y < 4; y++) {
#pragma HLS UNROLL
		temp2 = temp;
		for (x = 0; x < 4; x++) {
#pragma HLS UNROLL
			dst[x] = ((pel)(temp2 >> 5));
			dst[x] = ((pel)((0 >(((1 << bit_depth) - 1 < ((int)dst[x]) ? (1 << bit_depth) - 1 : ((int)dst[x]))) ? 0 : (((1 << bit_depth) - 1 < ((
				int)dst[x]) ? (1 << bit_depth) - 1 : ((int)dst[x]))))));
			temp2 += b;
		}
		temp += c;
		dst += w;
	}
}

void ipred_bi_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], U4 bit_depth, int w, int h)
{
#pragma HLS INLINE 
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

#if MD_PRINT
	((int)com_tbl_log2[w]) >= 2 ? ((void)0) : __assert_HLS("com_tbl_log2[w] >= 2", "../separate/com_ipred.c",
		(unsigned int)1593, __FUNCTION__);
	((int)com_tbl_log2[h]) >= 2 ? ((void)0) : __assert_HLS("com_tbl_log2[h] >= 2", "../separate/com_ipred.c",
		(unsigned int)1594, __FUNCTION__);
#endif
	int x;
	int y;
	int ishift_x = (int)com_tbl_log2[w];
	int ishift_y = (int)com_tbl_log2[h];
	int ishift = ishift_x < ishift_y ? ishift_x : ishift_y;
	int ishift_xy = ishift_x + ishift_y + 1;
	int offset = 1 << (ishift_x + ishift_y);
	int a;
	int b;
	int c;
	int wt;
	int wxy;
	int tmp;
	int predx;
	int ref_up[128 >> 1];
	int ref_le[128 >> 1];
	int up[128 >> 1];
	int le[128 >> 1];
	int wy[128 >> 1];
	int wc;
	int tbl_wc[6] = { (-1), (21), (13), (7), (4), (2) };
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
#pragma HLS ARRAY_PARTITION variable=ref_up complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1
#pragma HLS ARRAY_PARTITION variable=ref_le complete dim=1
#pragma HLS ARRAY_PARTITION variable=up complete dim=1
#pragma HLS ARRAY_PARTITION variable=le complete dim=1
#pragma HLS ARRAY_PARTITION variable=wy complete dim=1
#pragma HLS ARRAY_PARTITION variable=tbl_wc complete dim=1

	wc = (ishift_x > ishift_y ? ishift_x - ishift_y : ishift_y - ishift_x);
	{
#if MD_PRINT
		wc <= 5 ? ((void)0) : __assert_HLS("wc <= 5", "../separate/com_ipred.c", (unsigned int)1607, __FUNCTION__);
#endif
	}
	;
	wc = tbl_wc[wc];
	for (x = 0; x < 4; x++) {
#pragma HLS UNROLL
		ref_up[x] = ((int)src_up[x]);
	}
	for (y = 0; y < 4; y++) {
#pragma HLS UNROLL
		ref_le[y] = ((int)src_le[y]);
	}
	a = ((int)src_up[w - 1]);
	b = ((int)src_le[h - 1]);
	c = (w == h ? (a + b + 1) >> 1 : (((a << ishift_x) + (b << ishift_y)) * wc + (1 << (ishift + 5))) >> (ishift + 6));
	wt = (c << 1) - a - b;
	for (x = 0; x < 4; x++) {
#pragma HLS UNROLL
		up[x] = b - ref_up[x];
		ref_up[x] <<= ishift_y;
	}
	tmp = 0;
	for (y = 0; y < 4; y++) {
#pragma HLS UNROLL
		le[y] = a - ref_le[y];
		ref_le[y] <<= ishift_x;
		wy[y] = tmp;
		tmp += wt;
	}
	for (y = 0; y < 4; y++) {
#pragma HLS UNROLL
		predx = ref_le[y];
		wxy = 0;
		for (x = 0; x < 4; x++) {

#pragma HLS UNROLL
			predx += le[y];
			ref_up[x] += up[x];
			dst[x] = ((pel)(((predx << ishift_y) + (ref_up[x] << ishift_x) + wxy + offset) >> ishift_xy));
			dst[x] = ((pel)((0 >(((1 << bit_depth) - 1 < ((int)dst[x]) ? (1 << bit_depth) - 1 : ((int)dst[x]))) ? 0 : (((1 << bit_depth) - 1 < ((
				int)dst[x]) ? (1 << bit_depth) - 1 : ((int)dst[x]))))));
			wxy += wy[y];
		}
		dst += w;
	}
}

void ipred_ang_val_4_cal(int filter_idx, int offset, pel src_pn_n1, pel src_p, pel src_pn, pel src_pn_p2, pel* temp_pel, int bit_depth) {

#pragma HLS INLINE 
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

	const int filter_bits_list[4] = { 7, 7, 6, 7 };
	const int filter_offset_list[4] = { 64, 64 ,32, 64 };

	S16 data1 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][0] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][0] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][0] : com_tbl_ipred_adi_bilinear[offset][0];
	S16 data2 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][1] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][1] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][1] : com_tbl_ipred_adi_bilinear[offset][1];
	S16 data3 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][2] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][2] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][2] : com_tbl_ipred_adi_bilinear[offset][2];
	S16 data4 = filter_idx == 0 ? com_tbl_ipred_adi_extra_smooth[offset][3] : filter_idx == 1 ? com_tbl_ipred_adi_semi_sharp[offset][3] : filter_idx == 2 ? tbl_mc_c_coeff_hp[offset][3] : com_tbl_ipred_adi_bilinear[offset][3];
	S16 data5 = filter_offset_list[filter_idx];
	S16 data6 = filter_bits_list[filter_idx];
	S32 buffer1 = src_pn_n1 * data1;
	S32 buffer2 = src_p * data2;
	S32 buffer3 = src_pn * data3;
	S32 buffer4 = src_pn_p2 * data4;
	S32 temp_pe = (pel)((buffer1 + buffer2 + buffer3 + buffer4 + data5) >> data6);
	*temp_pel = COM_CLIP3(0, ((1 << bit_depth) - 1), temp_pe);

}


void ipred_ang_val_4(pel pred_cache_16x16[4 * 4], pel src_up[11], pel src_le[11], int ipm, U4 bit_depth, int n, int w, int h
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#pragma HLS INLINE 
#pragma HLS ARRAY_PARTITION variable=pred_cache_16x16 complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_up complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_le complete dim=1

	static const int com_tbl_ipred_dxdy[33][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) }, { (372), (2816) }, { (256), (4096) }, { (128), (8192) }, { 0, 0 }, { (128), (8192) }, { (256), (4096) }, { (372), (2816) }, { (512), (2048) }, { (744), (1408) }, { (1024), (1024) }, { (1408), (744) }, { (2048), (512) }, { (2816), (372) }, { (4096), (256) }, { (8192), (128) }, { 0, 0 }, { (8192), (128) }, { (4096), (256) }, { (2816), (372) }, { (2048), (512) }, { (1408), (744) }, { (1024), (1024) }, { (744), (1408) }, { (512), (2048) } };

#if MIPF
	//const s16(*tbl_filt_list[4])[4] = { com_tbl_ipred_adi + 32, com_tbl_ipred_adi + 64, tbl_mc_c_coeff_hp, com_tbl_ipred_adi };

	const int is_small = w * h <= (is_luma ? MIPF_TH_SIZE : MIPF_TH_SIZE_CHROMA);
	const int td = is_luma ? MIPF_TH_DIST : MIPF_TH_DIST_CHROMA;
	int filter_idx;


#else
	const s16(*tbl_filt)[4] = com_tbl_ipred_adi;
	const int filter_offset = ADI_4T_FILTER_OFFSET;
	const int filter_bits = ADI_4T_FILTER_BITS;
#endif

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
	const int dxy = ipm > 24 || ipm < 12 ? -1 : 1;
	const s16* filter;
	//const s16(*tbl_filt)[4];
	//int filter_offset;
	//int filter_bits;
	const int* mt = com_tbl_ipred_dxdy[ipm];
	//pel * src_ch = NULL;
	int num_selections = 0;
	int use_x = -1;
	int p;
	int pn;
	int pn_n1;
	int pn_p2;
	int pos_max = w + h - 1;
	pel temp_pel = (pel)0;
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
	//filter_offset = 1 << (7 - 1);
	//filter_bits = 7;
	//tbl_filt = com_tbl_ipred_adi;
	{
#if MD_PRINT
		ipm < 33 ? ((void)0) : __assert_HLS("ipm < 33", "../separate/com_ipred.c", (unsigned int)1974, __FUNCTION__);
#endif
	}
	;
	int i, j;
	for (j = 0; j < 4; j++) {
#pragma HLS UNROLL
		for (i = 0; i < 4; i++) {
#pragma HLS UNROLL
			//if (ipm == 12) {
			//	return src_up[i + 3];
			//}
			//else if (ipm == 24) {
			//	return src_le[j + 3];
			//}
			if ((ipm < 12) || ((ipm >= 13) && (ipm <= 17)) || ((ipm >= 34) && (ipm <= 50))) {
				filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;// MIPF
			}
			else if (((ipm >= 18) && (ipm <= 23)) || ((ipm >= 25) && (ipm <= 32)) || ((ipm >= 51) && (ipm <= 65))) {
				filter_idx = mipf_enable_flag ? (i < td ? is_small + 1 : is_small) : 3;// MIPF
			}
			if (ipm < 12) {
				/* case x-line */
				t_dx = (j + 1) * mt[0] >> 10;
				offset = ((j + 1) * mt[0] << 5 >> 10) - (t_dx << 5);
				;
				x = i + t_dx;
				y = -1;
			}
			else if (ipm > 24) {
				t_dy = (i + 1) * mt[1] >> 10;
				offset = ((i + 1) * mt[1] << 5 >> 10) - (t_dy << 5);
				;
				x = -1;
				y = j + t_dy;
			}
			else {
				t_dx = (j + 1) * mt[0] >> 10;
				offset_x = ((j + 1) * mt[0] << 5 >> 10) - (t_dx << 5);
				;
				t_dx = -t_dx;
				xx = i + t_dx;
				yx = -1;
				t_dy = (i + 1) * mt[1] >> 10;
				offset_y = ((i + 1) * mt[1] << 5 >> 10) - (t_dy << 5);
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
			//com_assert(x != INT_MAX);
			//com_assert(y != INT_MAX);
			if (y == -1) {
				if (dxy < 0) {
					xn_n1 = x - 1;
					xn = x + 1;
					xn_p2 = x + 2;
				}
				else {
					xn_n1 = x + 1;
					xn = x - 1;
					xn_p2 = x - 2;
				}
				use_x = 1;
				++num_selections;
				//src_ch = src_up;
				pos_max = w * 2 - 1;
			}
			else if (x == -1) {
				if (dxy < 0) {
					yn_n1 = y - 1;
					yn = y + 1;
					yn_p2 = y + 2;
				}
				else {
					yn_n1 = y + 1;
					yn = y - 1;
					yn_p2 = y - 2;
				}
				use_x = 0;
				++num_selections;
				//src_ch = src_le;
				pos_max = h * 2 - 1;
			}
			//com_assert(num_selections == 1);
			//com_assert(src_ch != NULL);
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
			pn_n1 = (pn_n1 < pos_max ? pn_n1 : pos_max);
			p = (p < pos_max ? p : pos_max);
			pn = (pn < pos_max ? pn : pos_max);
			pn_p2 = (pn_p2 < pos_max ? pn_p2 : pos_max);

#if MIPF
			//filter_idx = mipf_enable_flag ? (j < td ? is_small + 1 : is_small) : 3;// MIPF
			//filter = (tbl_filt_list[filter_idx] + offset)[0];// MIPF
#else
			filter = (tbl_filt + offset)[0];
#endif

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

			ipred_ang_val_4_cal(filter_idx,  offset, src_pn_n1, src_p,  src_pn,  src_pn_p2,  &temp_pel, bit_depth);
			pred_cache_16x16[j * 4 + i] = temp_pel;
			}
		}
}

void ipred_ang_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], U4 bit_depth, U7 w, U7 h, S8 ipm, U2 type
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
)
{
#pragma HLS INLINE 
	ipred_ang_val_4(dst, src_up, src_le, ipm, bit_depth, 0, w, h
#if MIPF
		, is_luma, mipf_enable_flag
#endif
	);

}
