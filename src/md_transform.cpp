//#include "enc_def.h"
#include "com_def.h"
#include <math.h>
#include <stdlib.h>
#include "md1.h"

extern s8 com_tbl_tm2[NUM_TRANS_TYPE][2][2];

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


void cu_plane_nz_cln(int dst[4][3], int plane)
{

#pragma HLS INLINE off
	int i;
	for (i = 0; i < 4; i++) {

#pragma HLS LOOP_TRIPCOUNT max=4
		dst[i][plane] = 0;
	}
}

U4 com_get_forward_trans_shift(U3 log2_size, U3 type, U4 bit_depth)
{
#pragma HLS INLINE

#if MD_PRINT
	log2_size <= 6 ? ((void)0) : __assert_HLS("log2_size <= 6", "../separate/enc_pintra.c", (unsigned int)2081,
		__FUNCTION__);
#endif

	return (type == 0) ? (U4)(log2_size + bit_depth - 10) : (U4)(log2_size + 5);
}

///******************   DCT-2 row transform   ******************************************/

 S4 get_transform_shift(const U4 bit_depth, const U4 tr_size_log2)
{
#pragma HLS INLINE //off
	return 15 - bit_depth - tr_size_log2;
}

 U4 com_get_inverse_trans_shift(U3 log2_size, U3 type, U4 bit_depth)
{

#pragma HLS INLINE //off
#if MD_PRINT
	log2_size <= 6 ? ((void)0) : __assert_HLS("log2_size <= 6", "../separate/com_itdq.c", (unsigned int)40,
		__FUNCTION__);
#endif
	return type == 0 ? (U4)5 : (U4)(15 + 5 - bit_depth);
}

#define W 64
#define H 64
#include "transform_func.h"
#define W 64
#define H 32
#include "transform_func.h"
#define W 32
#define H 64
#include "transform_func.h"
#define W 64
#define H 16
#include "transform_func.h"
#define W 16
#define H 64
#include "transform_func.h"
#define W 64
#define H 8
#include "transform_func.h"
#define W 8
#define H 64
#include "transform_func.h"
  //32
#define W 32
#define H 32
#include "transform_func.h"
#define W 32
#define H 16
#include "transform_func.h"
#define W 16
#define H 32
#include "transform_func.h"
#define W 32
#define H 8
#include "transform_func.h"
#define W 8
#define H 32
#include "transform_func.h"
#define W 32
#define H 4
#include "transform_func.h"
#define W 4
#define H 32
#include "transform_func.h"
//16
#define W 16
#define H 16
#include "transform_func.h"
#define W 16
#define H 8
#include "transform_func.h"
#define W 8
#define H 16
#include "transform_func.h"
#define W 16
#define H 4
#include "transform_func.h"
#define W 4
#define H 16
#include "transform_func.h"
//8
#define W 8
#define H 8
#include "transform_func.h"
#define W 8
#define H 4
#include "transform_func.h"
#define W 4
#define H 8
#include "transform_func.h"
#define W 4
#define H 4
#include "transform_func.h"