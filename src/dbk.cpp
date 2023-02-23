#include <assert.h>
#include <stdlib.h>
#include "iime.h"
#include "dbk.h"
//#include "math.h"
#pragma warning(disable:4102)
#pragma warning(disable:26812)

//static const S8 com_tbl_log2[257] = { ((S8)(-1)), ((S8)(-1)), ((S8)1), ((S8)(-1)), ((S8)2), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)3), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)4), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)5), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)6), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)7), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)(-1)), ((S8)8)
static const S4 com_tbl_log2[257] = { ((S4)(-1)), ((S4)(-1)), ((S4)1), ((S4)(-1)), ((S4)2), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)3), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)4), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)5), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)6), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)7), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)(-1)), ((S4)8)
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
#define CONV_LOG2(v)                    (com_tbl_log2[v])
//#if DBK2SAO

//extern pel prprc_sq_dbk_u[60 + 1][2][2];
//extern pel prprc_sq_dbk_v[60 + 1][2][2];
//extern pel prprc_rctngl_hor_dbk_u[60][2][36];
//extern pel prprc_rctngl_hor_dbk_v[60][2][36];
//extern pel prprc_rctngl_ver_dbk_u[36][2];
//extern pel prprc_rctngl_ver_dbk_v[36][2];
//pel temp_sao_lcu_array_y[70][70];
//pel temp_sao_lcu_array_y_dupli1[70][70];
//extern pel temp_sao_lcu_array_u[38][38];
//extern pel temp_sao_lcu_array_v[38][38];
//pel dbk_lcu_array_y[68][68];
//pel dbk_lcu_array_u[36][36];
//pel dbk_lcu_array_v[36][36];
/*
static void dbk_2_sao_y(pel dbkout_y[71][71], pel dbk_mid_y[71][71], U13 lcu_pos, U6 lcu_stride, pel rctngl_hor_dbk_y[60][2][68], pel rctngl_ver_dbk_y[68][2], pel small_sq_dbk_y[60 + 1][2][2])
{
	int i;
	int j;

	// Expand the current DBK output LCU window left by 2 pixels & up by 2 pixels, to deal with the data dependency of SAO filtering process
	// If current LCU is at the edge of pic, adding padding pixels to fullfill shifted 66*66
	// Generally
	// Add paddings

	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			dbkout_y[i][j] = small_sq_dbk_y[lcu_pos % (lcu_stride + 1)][i][j];
			dbk_mid_y[i][j] = small_sq_dbk_y[lcu_pos % (lcu_stride + 1)][i][j];
		}
	}
	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 2; j < 70; j++)
		{
			dbkout_y[i][j] = rctngl_hor_dbk_y[lcu_pos % lcu_stride][i][j - 2];
			dbk_mid_y[i][j] = rctngl_hor_dbk_y[lcu_pos % lcu_stride][i][j - 2];
		}
	}
	for (i = 2; i < 70; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			dbkout_y[i][j] = rctngl_ver_dbk_y[i - 2][j];
			dbk_mid_y[i][j] = rctngl_ver_dbk_y[i - 2][j];
		}
	}

	// If the right bound LCU width > 60 or the bottom bound LCU height > 60, set the special buffers for right & bottom bound
	// If the pic contains integral 64*64s

	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 68; j++)
		{
			rctngl_hor_dbk_y[lcu_pos % lcu_stride][i][j] = dbk_mid_y[i + 64][j + 2];
		}
	}
	for (i = 0; i < 68; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			rctngl_ver_dbk_y[i][j] = dbk_mid_y[i + 2][j + 64];
		}
	}
	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			small_sq_dbk_y[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_y[i + 64][j + 64];
		}
	}
}

static void dbk_2_sao_u(pel dbkout_u[39][39], pel dbk_mid_u[39][39], U13 lcu_pos, U6 lcu_stride, pel rctngl_hor_dbk_u[60][2][36], pel rctngl_ver_dbk_u[36][2], pel small_sq_dbk_u[60 + 1][2][2],
	pel prprc_sq_dbk_u[60 + 1][2][2], pel prprc_rctngl_hor_dbk_u[60][2][36], pel prprc_rctngl_ver_dbk_u[36][2]
)
{
	int i;
	int j;

	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 36; j++)
		{
			dbkout_u[i + 2][j + 2] = prprc_rctngl_hor_dbk_u[lcu_pos % lcu_stride][i][j];
			dbk_mid_u[i + 2][j + 2] = prprc_rctngl_hor_dbk_u[lcu_pos % lcu_stride][i][j];
			prprc_rctngl_hor_dbk_u[lcu_pos % lcu_stride][i][j] = dbk_mid_u[i + 34][j + 2];
		}
	}
	for (i = 0; i < 36; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			dbkout_u[i + 2][j + 2] = prprc_rctngl_ver_dbk_u[i][j];
			dbk_mid_u[i + 2][j + 2] = prprc_rctngl_ver_dbk_u[i][j];
			prprc_rctngl_ver_dbk_u[i][j] = dbk_mid_u[i + 2][j + 34];
		}
	}
	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			dbkout_u[i + 2][j + 2] = prprc_sq_dbk_u[lcu_pos % (lcu_stride + 1)][i][j];
			dbk_mid_u[i + 2][j + 2] = prprc_sq_dbk_u[lcu_pos % (lcu_stride + 1)][i][j];
			prprc_sq_dbk_u[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_u[i + 34][j + 34];
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Add paddings
	// 2x2 Small Square

	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			dbkout_u[i][j] = small_sq_dbk_u[lcu_pos % (lcu_stride + 1)][i][j];
			dbk_mid_u[i][j] = small_sq_dbk_u[lcu_pos % (lcu_stride + 1)][i][j];
		}
	}
	// Up 2 ROW
	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 2; j < 38; j++)
		{
			dbkout_u[i][j] = rctngl_hor_dbk_u[lcu_pos % lcu_stride][i][j - 2];
			dbk_mid_u[i][j] = rctngl_hor_dbk_u[lcu_pos % lcu_stride][i][j - 2];
		}
	}
	// Left 2 COLUMN
	for (i = 2; i < 38; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			dbkout_u[i][j] = rctngl_ver_dbk_u[i - 2][j];
			dbk_mid_u[i][j] = rctngl_ver_dbk_u[i - 2][j];
		}
	}
	// Set sq and rct buffers from DBK output buffers
	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 36; j++)
		{
			rctngl_hor_dbk_u[lcu_pos % lcu_stride][i][j] = dbk_mid_u[i + 32][j + 2];
		}
	}
	for (i = 0; i < 36; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			rctngl_ver_dbk_u[i][j] = dbk_mid_u[i + 2][j + 32];
		}
	}
	for (i = 0; i < 2; i++)
	{
#pragma HLS PIPELINE
		for (j = 0; j < 2; j++)
		{
			small_sq_dbk_u[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_u[i + 32][j + 32];
		}
	}
}

static void dbk_2_sao_v(pel dbkout_v[39][39], pel dbk_mid_v[39][39], U13 lcu_pos, U6 lcu_stride, pel rctngl_hor_dbk_v[60][2][36], pel rctngl_ver_dbk_v[36][2], pel small_sq_dbk_v[60 + 1][2][2],
	pel prprc_sq_dbk_v[60 + 1][2][2], pel prprc_rctngl_hor_dbk_v[60][2][36], pel prprc_rctngl_ver_dbk_v[36][2]
)
{
	int i;
	int j;

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 36; j++)
		{
#pragma HLS PIPELINE
			dbkout_v[i + 2][j + 2] = prprc_rctngl_hor_dbk_v[lcu_pos % lcu_stride][i][j];
			dbk_mid_v[i + 2][j + 2] = prprc_rctngl_hor_dbk_v[lcu_pos % lcu_stride][i][j];
			prprc_rctngl_hor_dbk_v[lcu_pos % lcu_stride][i][j] = dbk_mid_v[i + 34][j + 2];
		}
	}
	for (i = 0; i < 36; i++)
	{
		for (j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			dbkout_v[i + 2][j + 2] = prprc_rctngl_ver_dbk_v[i][j];
			dbk_mid_v[i + 2][j + 2] = prprc_rctngl_ver_dbk_v[i][j];
			prprc_rctngl_ver_dbk_v[i][j] = dbk_mid_v[i + 2][j + 34];
		}
	}
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			dbkout_v[i + 2][j + 2] = prprc_sq_dbk_v[lcu_pos % (lcu_stride + 1)][i][j];
			dbk_mid_v[i + 2][j + 2] = prprc_sq_dbk_v[lcu_pos % (lcu_stride + 1)][i][j];
			prprc_sq_dbk_v[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_v[i + 34][j + 34];
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Add paddings

	// 2x2 Small Square
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			dbkout_v[i][j] = small_sq_dbk_v[lcu_pos % (lcu_stride + 1)][i][j];
			dbk_mid_v[i][j] = small_sq_dbk_v[lcu_pos % (lcu_stride + 1)][i][j];
		}
	}
	// Up 2 ROW
	for (i = 0; i < 2; i++)
	{
		for (j = 2; j < 38; j++)
		{
#pragma HLS PIPELINE
			dbkout_v[i][j] = rctngl_hor_dbk_v[lcu_pos % lcu_stride][i][j - 2];
			dbk_mid_v[i][j] = rctngl_hor_dbk_v[lcu_pos % lcu_stride][i][j - 2];
		}
	}
	// Left 2 COLUMN
	for (i = 2; i < 38; i++)
	{
		for (j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			dbkout_v[i][j] = rctngl_ver_dbk_v[i - 2][j];
			dbk_mid_v[i][j] = rctngl_ver_dbk_v[i - 2][j];
		}
	}
	// Set sq and rct buffers from DBK output buffers
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 36; j++)
		{
#pragma HLS PIPELINE
			rctngl_hor_dbk_v[lcu_pos % lcu_stride][i][j] = dbk_mid_v[i + 32][j + 2];
		}
	}
	for (i = 0; i < 36; i++)
	{
		for (j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			rctngl_ver_dbk_v[i][j] = dbk_mid_v[i + 2][j + 32];
		}
	}
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
#pragma HLS PIPELINE
			small_sq_dbk_v[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_v[i + 32][j + 32];
		}
	}
}

void dbk_2_sao(pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	U13 lcu_pos, U6 lcu_stride,
	pel rctngl_hor_dbk_y[60][2][68], pel rctngl_hor_dbk_u[60][2][36], pel rctngl_hor_dbk_v[60][2][36],
	pel rctngl_ver_dbk_y[68][2], pel rctngl_ver_dbk_u[36][2], pel rctngl_ver_dbk_v[36][2], pel small_sq_dbk_y[60 + 1][2][2],
	pel small_sq_dbk_u[60 + 1][2][2], pel small_sq_dbk_v[60 + 1][2][2],
	pel prprc_sq_dbk_u[60 + 1][2][2], pel prprc_sq_dbk_v[60 + 1][2][2],
	pel prprc_rctngl_hor_dbk_u[60][2][36], pel prprc_rctngl_hor_dbk_v[60][2][36],
    pel prprc_rctngl_ver_dbk_u[36][2], pel prprc_rctngl_ver_dbk_v[36][2]
)
{
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=dbkout_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbkout_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbkout_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=rctngl_ver_dbk_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=rctngl_ver_dbk_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=rctngl_ver_dbk_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=small_sq_dbk_y complete dim=3
#pragma HLS ARRAY_PARTITION variable=small_sq_dbk_u complete dim=3
#pragma HLS ARRAY_PARTITION variable=small_sq_dbk_v complete dim=3
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_y complete dim=3
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_u complete dim=3
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_v complete dim=3
#pragma HLS ARRAY_PARTITION variable=prprc_sq_dbk_u complete dim=3
#pragma HLS ARRAY_PARTITION variable=prprc_sq_dbk_v complete dim=3
#pragma HLS ARRAY_PARTITION variable=prprc_rctngl_hor_dbk_u complete dim=3
#pragma HLS ARRAY_PARTITION variable=prprc_rctngl_hor_dbk_v complete dim=3
#pragma HLS ARRAY_PARTITION variable=prprc_rctngl_ver_dbk_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=prprc_rctngl_ver_dbk_v complete dim=2
	dbk_2_sao_y(dbkout_y, dbk_mid_y, lcu_pos, lcu_stride, rctngl_hor_dbk_y, rctngl_ver_dbk_y, small_sq_dbk_y);
	dbk_2_sao_u(dbkout_u, dbk_mid_u, lcu_pos, lcu_stride, rctngl_hor_dbk_u, rctngl_ver_dbk_u, small_sq_dbk_u, 
		prprc_sq_dbk_u, prprc_rctngl_hor_dbk_u, prprc_rctngl_ver_dbk_u);
	dbk_2_sao_v(dbkout_v, dbk_mid_v, lcu_pos, lcu_stride, rctngl_hor_dbk_v, rctngl_ver_dbk_v, small_sq_dbk_v,
		prprc_sq_dbk_v, prprc_rctngl_hor_dbk_v, prprc_rctngl_ver_dbk_v);
}
*/
//#endif

/*
static void com_get_split_mode_DBK_1(U3* split_mode, U3 cud, U8 cup, U7 cu_width, U7 cu_height,
	U6 line_map_split[5][7][256])
{
#pragma HLS inline
	//int pos = cup + ((cu_height >> 1 >> 2) * (lcu_s >> 2) + (cu_width >> 1 >> 2));//lcu_s=64
	U8 pos = cup + ((cu_height >> 3) * 16 + (cu_width >> 3));
	U3 shape = (U3)(SQUARE + (com_tbl_log2[cu_width] - com_tbl_log2[cu_height]));

	if (cu_width < 8 && cu_height < 8) {
		*split_mode = (NO_SPLIT);
		return;
	}

	*split_mode = ((U3)(line_map_split[cud][shape][pos]) & 0x07);
	return;
}

static int com_split_part_count_1(U3 split_mode)
{
#pragma HLS inline
	switch (split_mode) {
	case SPLIT_BI_VER:
		;
	case SPLIT_BI_HOR:
		return 2;
	case SPLIT_EQT_VER:
		;
	case SPLIT_EQT_HOR:
		return 4;
	case SPLIT_QUAD:
		return 4;
	default:
		// NO_SPLIT
		return 0;
	}
}

static SPLIT_DIR com_split_get_direction_1(U3 mode)
{
#pragma HLS inline
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

static int com_split_is_vertical_1(U3 mode)
{
	return ((com_split_get_direction_1(mode))) == (SPLIT_VER) ? 1 : 0;
}

static int dbk_com_split_get_part_size_1(U3 split_mode, int part_num, int length)
{
#pragma HLS inline
	int ans = length;
	switch (split_mode) {
	case SPLIT_QUAD:
		;
	case SPLIT_BI_HOR:
		;
	case SPLIT_BI_VER:
		ans = length >> 1;
		break;
	case SPLIT_EQT_HOR:
		;
	case SPLIT_EQT_VER:
		if (part_num == 1 || part_num == 2) {
			ans = length >> 1;
		}
		else {
			ans = length >> 2;
		}
		break;
	}
	return ans;
}

static int dbk_com_split_get_part_size_idx_1(U3 split_mode, int part_num, int length_idx)
{
#pragma HLS inline
	int ans = length_idx;
	switch (split_mode) {
	case SPLIT_QUAD:
		;
	case SPLIT_BI_HOR:
		;
	case SPLIT_BI_VER:
		ans = length_idx - 1;
		break;
	case SPLIT_EQT_HOR:
		;
	case SPLIT_EQT_VER:
		if (part_num == 1 || part_num == 2) {
			ans = length_idx - 1;
		}
		else {
			ans = length_idx - 2;
		}
		break;
	}
	return ans;
}

static void dbk_com_split_get_part_structure_1(U3 split_mode, U13 x0, U13 y0, U7 cu_width, U7 cu_height, U8 cup,
	U3 cud, COM_SPLIT_STRUCT* split_struct)
{
#pragma HLS inline off
#pragma HLS ARRAY_PARTITION variable=split_struct->width complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct->height complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct->log_cuw complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct->log_cuh complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct->x_pos complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct->y_pos complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct->cup complete dim=0
	int i;
	S4 log_cuw;
	S4 log_cuh;
	int cup_w;
	int cup_h;
	split_struct->part_count = com_split_part_count_1(split_mode);
	log_cuw = (com_tbl_log2[cu_width]);
	log_cuh = (com_tbl_log2[cu_height]);
	split_struct->x_pos[0] = x0;
	split_struct->y_pos[0] = y0;
	split_struct->cup[0] = cup;
	switch (split_mode) {
	case NO_SPLIT: {
					   split_struct->width[0] = cu_width;
					   split_struct->height[0] = cu_height;
					   split_struct->log_cuw[0] = log_cuw;
					   split_struct->log_cuh[0] = log_cuh;
	}
		break;
	case SPLIT_QUAD: {
						 split_struct->width[0] = cu_width >> 1;
						 split_struct->height[0] = cu_height >> 1;
						 split_struct->log_cuw[0] = log_cuw - 1;
						 split_struct->log_cuh[0] = log_cuh - 1;
        if(split_struct->part_count ==4){
        for (i = 1; i < 4; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=4
            split_struct->width[i] = split_struct->width[0];
            split_struct->height[i] = split_struct->height[0];
            split_struct->log_cuw[i] = split_struct->log_cuw[0];
            split_struct->log_cuh[i] = split_struct->log_cuh[0];
        }
    }
        else if (split_struct->part_count == 2) {
            for (i = 1; i < 2; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=2
                split_struct->width[i] = split_struct->width[0];
                split_struct->height[i] = split_struct->height[0];
                split_struct->log_cuw[i] = split_struct->log_cuw[0];
                split_struct->log_cuh[i] = split_struct->log_cuh[0];
            }
        }

        split_struct->x_pos[1] = x0 + split_struct->width[0];
        split_struct->y_pos[1] = y0;
        split_struct->x_pos[2] = x0;
        split_struct->y_pos[2] = y0 + split_struct->height[0];
        split_struct->x_pos[3] = split_struct->x_pos[1];
        split_struct->y_pos[3] = split_struct->y_pos[2];
        cup_w = split_struct->width[0] >> 2;
        cup_h = split_struct->height[0] >> 2 << 4;
        split_struct->cup[1] = cup + cup_w;
        split_struct->cup[2] = cup + cup_h;
        split_struct->cup[3] = split_struct->cup[1] + cup_h;
        split_struct->cud = cud + 1;
    }
                        break;
    default: {
        if (com_split_is_vertical_1(split_mode)) {
            if(split_struct->part_count==4){
            for (i = 0; i < 4; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=4
                split_struct->width[i] = dbk_com_split_get_part_size_1(split_mode, i, cu_width);
                split_struct->log_cuw[i] = dbk_com_split_get_part_size_idx_1(split_mode, i, log_cuw);
                if (split_mode == SPLIT_EQT_VER) {
                    if (i == 0 || i == 3) {
                        split_struct->height[i] = cu_height;
                        split_struct->log_cuh[i] = log_cuh;
                    }
                    else {
                        split_struct->height[i] = cu_height >> 1;
                        split_struct->log_cuh[i] = log_cuh - 1;
                    }
                }
                else {
                    split_struct->height[i] = cu_height;
                    split_struct->log_cuh[i] = log_cuh;
                    if (i) {
                        split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
                        split_struct->y_pos[i] = split_struct->y_pos[i - 1];
                        split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> 2);
                    }
                }
            }
        }
            else if (split_struct->part_count == 2 ) {
                for (i = 0; i < 2; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=2
                    split_struct->width[i] = dbk_com_split_get_part_size_1(split_mode, i, cu_width);
                    split_struct->log_cuw[i] = dbk_com_split_get_part_size_idx_1(split_mode, i, log_cuw);
                    if (split_mode == SPLIT_EQT_VER) {
                        if (i == 0 || i == 3) {
                            split_struct->height[i] = cu_height;
                            split_struct->log_cuh[i] = log_cuh;
                        }
                        else {
                            split_struct->height[i] = cu_height >> 1;
                            split_struct->log_cuh[i] = log_cuh - 1;
                        }
                    }
                    else {
                        split_struct->height[i] = cu_height;
                        split_struct->log_cuh[i] = log_cuh;
                        if (i) {
                            split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
                            split_struct->y_pos[i] = split_struct->y_pos[i - 1];
                            split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> 2);
                        }
                    }
                }
            }


            if (split_mode == (SPLIT_EQT_VER)) {
                split_struct->x_pos[1] = split_struct->x_pos[0] + split_struct->width[0];
                split_struct->y_pos[1] = split_struct->y_pos[0];
                split_struct->cup[1] = split_struct->cup[0] + (split_struct->width[0] >> 2);
                cup_h = split_struct->height[1] >> 2 << 4;
                split_struct->x_pos[2] = split_struct->x_pos[1];
                split_struct->y_pos[2] = split_struct->y_pos[1] + split_struct->height[1];
                split_struct->cup[2] = split_struct->cup[1] + cup_h;
                split_struct->x_pos[3] = split_struct->x_pos[1] + split_struct->width[1];
                split_struct->y_pos[3] = split_struct->y_pos[1];
                split_struct->cup[3] = split_struct->cup[1] + (split_struct->width[1] >> 2);
            }
        }
        else {
            if(split_struct->part_count == 4){
            for (i = 0; i < 4; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=4
						 if (split_mode == (SPLIT_EQT_HOR)) {
							 if (i == 0 || i == 3) {
								 split_struct->width[i] = cu_width;
								 split_struct->log_cuw[i] = log_cuw;
							 }
							 else {
								 split_struct->width[i] = cu_width >> 1;
								 split_struct->log_cuw[i] = log_cuw - 1;
							 }
						 }
						 else {
							 split_struct->width[i] = cu_width;
							 split_struct->log_cuw[i] = log_cuw;
							 if (i) {
								 split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
								 split_struct->x_pos[i] = split_struct->x_pos[i - 1];
								 split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->height[i - 1] >> 2 << 4);
							 }
						 }
						 split_struct->height[i] = dbk_com_split_get_part_size_1(split_mode, i, cu_height);
						 split_struct->log_cuh[i] = dbk_com_split_get_part_size_idx_1(split_mode, i, log_cuh);
					 }
        }
            else if (split_struct->part_count == 2) {
                for (i = 0; i < 2; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=2
                    if (split_mode == (SPLIT_EQT_HOR)) {
                        if (i == 0 || i == 3) {
                            split_struct->width[i] = cu_width;
                            split_struct->log_cuw[i] = log_cuw;
                        }
                        else {
                            split_struct->width[i] = cu_width >> 1;
                            split_struct->log_cuw[i] = log_cuw - 1;
                        }
                    }
                    else {
                        split_struct->width[i] = cu_width;
                        split_struct->log_cuw[i] = log_cuw;
                        if (i) {
                            split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
                            split_struct->x_pos[i] = split_struct->x_pos[i - 1];
                            split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->height[i - 1] >> 2 << 4);
                        }
                    }
                    split_struct->height[i] = dbk_com_split_get_part_size_1(split_mode, i, cu_height);
                    split_struct->log_cuh[i] = dbk_com_split_get_part_size_idx_1(split_mode, i, log_cuh);
                }
            }

					 if (split_mode == (SPLIT_EQT_HOR)) {
						 split_struct->y_pos[1] = split_struct->y_pos[0] + split_struct->height[0];
						 split_struct->x_pos[1] = split_struct->x_pos[0];
						 split_struct->cup[1] = split_struct->cup[0] + (split_struct->height[0] >> 2 << 4);
						 split_struct->y_pos[2] = split_struct->y_pos[1];
						 split_struct->x_pos[2] = split_struct->x_pos[1] + split_struct->width[1];
						 split_struct->cup[2] = split_struct->cup[1] + (split_struct->width[1] >> 2);
						 split_struct->y_pos[3] = split_struct->y_pos[1] + split_struct->height[1];
						 split_struct->x_pos[3] = split_struct->x_pos[1];
						 split_struct->cup[3] = split_struct->cup[1] + (split_struct->height[1] >> 2 << 4);
					 }
				 }
				 switch (split_mode) {
				 case (int)SPLIT_BI_VER:
					 split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height ? 0 : 1));
					 break;
				 case (int)SPLIT_BI_HOR:
					 split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height ? 0 : 1));
					 break;
				 default:
					 // Triple tree case
					 split_struct->cud = cud + ((cu_width == cu_height ? 0 : 1));
					 break;
				 }
	}
		break;
	}
}
*/
U7 get_pow(int num) {
	U7 pow_num = 0;
	if (num == 2) {
		pow_num = 4;
	}
	else if (num == 3) {
		pow_num = 8;
	}
	else if (num == 4) {
		pow_num = 16;
	}
	else if (num == 5) {
		pow_num = 32;
	}
	else if (num == 6) {
		pow_num = 64;
	}
	else {
		pow_num = 66;
	}
	return pow_num;
}

static void InitPpbEdgeilter_2_ver(U2 ppbEdgeFilter_ctu_ver0[16][16])
{
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_ver0 complete dim=0
#pragma HLS inline off

	int x_i;
	int y_i;
	for (x_i = 0; x_i < 16; x_i++) {
#pragma HLS UNROLL
		for (y_i = 0; y_i < 16; y_i++) {
#pragma HLS UNROLL
			ppbEdgeFilter_ctu_ver0[y_i][x_i] = 0;
		}
	}
}

static void InitPpbEdgeilter_2_hor(U2 ppbEdgeFilter_ctu_hor1[16][16])
{
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_hor1 complete dim=0
#pragma HLS inline off

	int x_i;
	int y_i;
	for (y_i = 0; y_i < 16; y_i++) {
#pragma HLS UNROLL
		for (x_i = 0; x_i < 16; x_i++) {
#pragma HLS UNROLL
			ppbEdgeFilter_ctu_hor1[y_i][x_i] = 0;
		}
	}
}


static void xSetEdgeFilterParam_hor_avs2_3(U2 edgecondition, U13 lcu_x, U13 lcu_y, U2 ppbEdgeFilter_ctu_hor1[16][16], U7 blk_x[16][16], U7 blk_y[16][16])
{
#pragma HLS inline off
	hor_edge_1: for (int j = 0; j < 16; j++) {
		#pragma HLS PIPELINE
		hor_edge_2: for (int i = 0; i < 16; i++) {
			#pragma HLS PIPELINE
			if (ppbEdgeFilter_ctu_hor1[j][i] != 0) {
				continue;
			}
			int ver_offset = 0;

			if (blk_y[j][i] == 4) ver_offset = 1;
			else if (blk_y[j][i] == 8) ver_offset = 2;
			else if (blk_y[j][i] == 16) ver_offset = 4;
			else if (blk_y[j][i] == 32) ver_offset = 8;
			else if (blk_y[j][i] == 64) ver_offset = 16;
			//else printf("error!\n");

			//if (j==0){
			//	ppbEdgeFilter_ctu_hor1[j][i] = 2;
			//}
			//else{

			if (ver_offset>0){
				if (ver_offset ==1 ){
					ppbEdgeFilter_ctu_hor1[j][i] = 2;
				}
				else if (ver_offset==2){
					if ((j%2)==0){
						ppbEdgeFilter_ctu_hor1[j][i] = 2;
					}
					else{
						ppbEdgeFilter_ctu_hor1[j][i] = 1;
					}
				}
				else if((j&(ver_offset-1))==0){
					ppbEdgeFilter_ctu_hor1[j][i] = 2;
				}
				else{
					ppbEdgeFilter_ctu_hor1[j][i] = 1;
				}
			}
			else{
				ppbEdgeFilter_ctu_hor1[j][i] = 1;
			}
		//	}
		}
	}

	for (int j = 0; j < 16; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 16; i++)
		{
			if (lcu_y > 0) {
				if (((j % 2) == 0)) {
					if (ppbEdgeFilter_ctu_hor1[j][i] == 1)
						ppbEdgeFilter_ctu_hor1[j][i] = 0;
				}
				else {
					ppbEdgeFilter_ctu_hor1[j][i] = 0;
				}
			}
			else {
				if ((j > 0) && ((j % 2) == 0)) {
					if (ppbEdgeFilter_ctu_hor1[j][i] == 1)
						ppbEdgeFilter_ctu_hor1[j][i] = 0;
				}
				else {
					ppbEdgeFilter_ctu_hor1[j][i] = 0;
				}
			}
		}
	}
}

static void xSetEdgeFilterParam_ver_avs2_3(U2 edgecondition, U13 lcu_x, U13 lcu_y, U2 ppbEdgeFilter_ctu_ver0[16][16], U7 blk_x[16][16], U7 blk_y[16][16])
{
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_ver0 complete dim=0
#pragma HLS inline off
	for (int j = 0; j < 16; j++) {
#pragma HLS PIPELINE
		for (int i = 0; i < 16; i++) {
#pragma HLS PIPELINE
			if (ppbEdgeFilter_ctu_ver0[j][i] != 0) {
				continue;
			}
			int hor_offset = 0;

			if (blk_x[j][i] == 4) hor_offset = 1;
			else if (blk_x[j][i] == 8) hor_offset = 2;
			else if (blk_x[j][i] == 16) hor_offset = 4;
			else if (blk_x[j][i] == 32) hor_offset = 8;
			else if (blk_x[j][i] == 64) hor_offset = 16;
			//else printf("error!\n");

			//if (j==0){
			//	ppbEdgeFilter_ctu_ver0[j][i] = 2;
			//}
			//else{
			if (hor_offset>0){
				if (hor_offset==1){
					ppbEdgeFilter_ctu_ver0[j][i] = 2;
				}
				else if (hor_offset==2){
					if ((i%2)==0){
						ppbEdgeFilter_ctu_ver0[j][i] = 2;
					}
					else {
						ppbEdgeFilter_ctu_ver0[j][i] = 1;
					}
				}
				else if((i&(hor_offset-1))==0){
					ppbEdgeFilter_ctu_ver0[j][i] = 2;
				}
				else{
					ppbEdgeFilter_ctu_ver0[j][i] = 1;
				}
			}
			else{
				ppbEdgeFilter_ctu_ver0[j][i] = 1;
			}
			//}
		}
	}

	for (int i = 0; i < 16; i++)
	{
#pragma HLS PIPELINE
		for (int j = 0; j < 16; j++)
		{
			if (lcu_x > 0) {
				if (((i % 2) == 0)) {
					if (ppbEdgeFilter_ctu_ver0[j][i] == 1)
						ppbEdgeFilter_ctu_ver0[j][i] = 0;
				}
				else {
					ppbEdgeFilter_ctu_ver0[j][i] = 0;
				}
			}
			else {
				if ((i > 0) && ((i % 2) == 0)) {
					if (ppbEdgeFilter_ctu_ver0[j][i] == 1)
						ppbEdgeFilter_ctu_ver0[j][i] = 0;
				}
				else {
					ppbEdgeFilter_ctu_ver0[j][i] = 0;
				}
			}
		}
	}
}


static void xSetEdgeFilter_SCU_mode0_2(U13 pic_width, U13 pic_height, U13 x, U13 y, U2 ppbEdgeFilter_ctu_ver0[16][16], U2 ppbEdgeFilter_ctu_hor1[16][16],
	U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
#pragma HLS inline off
	U7 blk_x[16][16];
	U7 blk_y[16][16];
	//InitPpbEdgeilter_2_ver(blk_x);
	//InitPpbEdgeilter_2_ver(blk_y);
#pragma HLS ARRAY_PARTITION variable=blk_x complete dim=2
#pragma HLS ARRAY_PARTITION variable=blk_y complete dim=2
	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; i++)
	{
#pragma HLS PIPELINE
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; j++)
		{
			if (((x+(i<<2))>(pic_width-1)) || ((y+(j<<2))>(pic_height-1))){
				continue;
			}
			U32 mode = cur_ctu_cu_mode[i][j];
			int offset_x = (mode & 0x0000000F) >> 0;
			int offset_y = (mode & 0x000000F0) >> 4;
			int pel_x = (mode & 0x0000FF00) >> 8;
			int pel_y = (mode & 0x00FF0000) >> 16;
			int width = (mode & 0x0F000000) >> 24;
			int height = (mode & 0xF0000000) >> 28;
			pel_x += offset_x;
			pel_y += offset_y;
			if ((pel_x > 15) || (pel_y > 15))
			{
				assert(false);
				printf("error!\n");
			}
			else{
				//blk_x[pel_y][pel_x] = pow(2,width);
				//blk_y[pel_y][pel_x] = pow(2,height);
				blk_x[pel_y][pel_x] = get_pow(width);
				blk_y[pel_y][pel_x] = get_pow(height);
			}
			//blk_x[pel_y][pel_x] = get_pow(width);
			//blk_y[pel_y][pel_x] = get_pow(height);
		}
	}

	xSetEdgeFilterParam_hor_avs2_3(2, x, y, ppbEdgeFilter_ctu_hor1, blk_x, blk_y);
	xSetEdgeFilterParam_ver_avs2_3(2, x, y, ppbEdgeFilter_ctu_ver0, blk_x, blk_y);
}




static void Cal_ppBEdgeFilter_2(U2 ppbEdgeFilter_ctu_ver0[16][16], U2 ppbEdgeFilter_ctu_hor1[16][16],
	U6 pic_width_in_lcu, U13 pic_width, U13 pic_height, U11 pic_width_in_scu,
	U13 lcu_x, U13 lcu_y, 
	//U6 line_map_split[5][7][256],
	U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
#pragma HLS inline off
	//#pragma HLS pipeline
	InitPpbEdgeilter_2_ver(ppbEdgeFilter_ctu_ver0);
	InitPpbEdgeilter_2_hor(ppbEdgeFilter_ctu_hor1);

	xSetEdgeFilter_SCU_mode0_2(pic_width, pic_height, lcu_x, lcu_y, ppbEdgeFilter_ctu_ver0, ppbEdgeFilter_ctu_hor1, cur_ctu_cu_mode);
}

static void Copydbktosixfour_YUV_1(U13 lcu_pos, pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel sixfour_buffer[2][64][4], pel U30x3_buffer[2][36][4], pel V30x3_buffer[2][36][4])
{
#pragma HLS inline off

	int i;
	int j;
	//right side 60*4
	for (i = 0; i < 64; ++i) {
#pragma HLS PIPELINE
		for (j = 0; j < 4; ++j) {
			sixfour_buffer[lcu_pos % 2][i][j] = dbk_y[i + 6][66 + j];
		}
	}

	for (i = 0; i < 32; ++i) {
#pragma HLS PIPELINE
		for (j = 0; j < 4; ++j) {
			U30x3_buffer[lcu_pos % 2][i][j] = dbk_u[6 + i][34 + j];
			V30x3_buffer[lcu_pos % 2][i][j] = dbk_v[i + 6][34 + j];
		}
	}
}

static void Copydbktofoursix_YUV_1(U13 lcu_pos, U6 lcu_stride, pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel foursix_buffer[61][4][64], pel U3x30_buffer[61][4][36],
	pel V3x30_buffer[61][4][36])
{
#pragma HLS inline off
	int i;
	int j;
	//bottom side 4*60
	for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
		for (j = 0; j < 64; ++j) {
			foursix_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_y[66 + i][j + 6];
		}
	}
	for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
		for (j = 0; j < 32; ++j) {
			U3x30_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_u[34 + i][j + 6];
			V3x30_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_v[34 + i][j + 6];
		}
	}
}

static void Copydbktofourfour_YUV_1(U13 lcu_pos, U6 lcu_stride, 
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel fourfour_buffer[62][4][4], pel U3x3_buffer[62][4][4],
	pel V3x3_buffer[62][4][4])
{
#pragma HLS inline off

	int i;
	int j;
	//right bottom 4*4
	for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
		for (j = 0; j < 4; ++j) {
			fourfour_buffer[lcu_pos % (lcu_stride + 2)][i][j] = dbk_y[66 + i][66 + j];
			U3x3_buffer[lcu_pos % (lcu_stride + 2)][i][j] = dbk_u[34 + i][34 + j];
			V3x3_buffer[lcu_pos % (lcu_stride + 2)][i][j] = dbk_v[34 + i][34 + j];
		}
	}
}

static void Copyfourfourtodbk_YUV_1(U13 lcu_x, U13 lcu_y, U13 lcu_pos, U6 lcu_stride,
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39],
	pel fourfour_buffer[62][4][4], pel U3x3_buffer[62][4][4],
	pel V3x3_buffer[62][4][4])
{
#pragma HLS inline off
	int i;
	int j;
	if (lcu_x > 0 && lcu_y > 0) {
		for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 4; ++j) {
				//dbk_y[i+2][j+2] = fourfour_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbkout_y[i+2][j+2] = fourfour_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbk_mid_y[i + 2][j + 2] = fourfour_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbk_ver_y[i + 2][j + 2] = fourfour_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];


				//dbk_u[i+2][j+2] = U3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbkout_u[i+2][j+2] = U3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbk_mid_u[i + 2][j + 2] = U3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbk_ver_u[i + 2][j + 2] = U3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];

				//dbk_v[i+2][j+2] = V3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbkout_v[i+2][j+2] = V3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbk_mid_v[i + 2][j + 2] = V3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];
				dbk_ver_v[i + 2][j + 2] = V3x3_buffer[(lcu_pos - lcu_stride - 1) % (lcu_stride + 2)][i][j];

			}
		}
	}
}

static void CopysixfourtodbkYUV_1(U13 lcu_x, U13 lcu_pos, 
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39],
	pel sixfour_buffer[2][64][4], pel U30x3_buffer[2][36][4], pel V30x3_buffer[2][36][4])
{
#pragma HLS inline off

	int i;
	int j;
	if (lcu_x > 0)//left side 2 joint VER and HOR    
	{
		for (i = 0; i < 64; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 4; ++j) {
				//dbk_y[i + 6][j+2] = sixfour_buffer[(lcu_pos - 1) % 2][i][j];
				dbkout_y[i + 6][j+2] = sixfour_buffer[(lcu_pos - 1) % 2][i][j];
				dbk_mid_y[i + 6][j + 2] = sixfour_buffer[(lcu_pos - 1) % 2][i][j];
				dbk_ver_y[i + 6][j + 2] = sixfour_buffer[(lcu_pos - 1) % 2][i][j];
			}
		}
		for (i = 0; i < 32; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 4; ++j) {
				//dbk_u[i + 6][j + 2] = U30x3_buffer[(lcu_pos - 1) % 2][i][j];
				dbkout_u[i + 6][j + 2] = U30x3_buffer[(lcu_pos - 1) % 2][i][j];
				dbk_mid_u[i + 6][j + 2] = U30x3_buffer[(lcu_pos - 1) % 2][i][j];
				dbk_ver_u[i + 6][j + 2] = U30x3_buffer[(lcu_pos - 1) % 2][i][j];

				//dbk_v[i + 6][j + 2] = V30x3_buffer[(lcu_pos - 1) % 2][i][j];
				dbkout_v[i + 6][j + 2] = V30x3_buffer[(lcu_pos - 1) % 2][i][j];
				dbk_mid_v[i + 6][j + 2] = V30x3_buffer[(lcu_pos - 1) % 2][i][j];
				dbk_ver_v[i + 6][j + 2] = V30x3_buffer[(lcu_pos - 1) % 2][i][j];
			}
		}
	}
}

static void Copyfoursixtodbk_Y_1(U13 lcu_y, U13 lcu_pos, U6 lcu_stride, 
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39],
	pel foursix_buffer[61][4][64], pel U3x30_buffer[61][4][36],
	pel V3x30_buffer[61][4][36])
{
#pragma HLS inline off

	int i;
	int j;
	if (lcu_y > 0) {
		for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 64; ++j) {
				// dbk_y[i+2][j + 6] = foursix_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbkout_y[i+2][j + 6] = foursix_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbk_mid_y[i + 2][j + 6] = foursix_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbk_ver_y[i + 2][j + 6] = foursix_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
			}
		}
		for (i = 0; i < 4; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 32; ++j) {
				//dbk_u[i+2][j + 6] = U3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbkout_u[i+2][j + 6] = U3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbk_mid_u[i + 2][j + 6] = U3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbk_ver_u[i + 2][j + 6] = U3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];


				//dbk_v[i+2][j + 6] = V3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbkout_v[i+2][j + 6] = V3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbk_mid_v[i + 2][j + 6] = V3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
				dbk_ver_v[i + 2][j + 6] = V3x30_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 1)][i][j];
			}
		}
	}
}

static void Copydbktolinebuffer_YUV_1(U13 lcu_x, U13 lcu_y, U13 lcu_pos, U6 lcu_stride, pel 
	dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39],
	pel foursix_buffer[61][4][64], pel U3x30_buffer[61][4][36],
	pel V3x30_buffer[61][4][36], pel sixfour_buffer[2][64][4], pel U30x3_buffer[2][36][4], pel V30x3_buffer[2][36][4],
	pel fourfour_buffer[62][4][4], pel U3x3_buffer[62][4][4], pel V3x3_buffer[62][4][4])
{
#pragma HLS inline off

	Copydbktosixfour_YUV_1(lcu_pos, dbk_y,dbk_u,dbk_v, sixfour_buffer, U30x3_buffer,
		V30x3_buffer);
	Copydbktofoursix_YUV_1(lcu_pos, lcu_stride, dbk_y, dbk_u, dbk_v, foursix_buffer,
		U3x30_buffer, V3x30_buffer);
	Copydbktofourfour_YUV_1(lcu_pos, lcu_stride, 
		dbk_y, dbk_u, dbk_v,
		fourfour_buffer, U3x3_buffer, V3x3_buffer);
	Copyfourfourtodbk_YUV_1(lcu_x, lcu_y, lcu_pos, lcu_stride, 
		dbk_y, dbk_u, dbk_v,
		dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v,
		dbk_ver_y, dbk_ver_u, dbk_ver_v,
		fourfour_buffer, U3x3_buffer, V3x3_buffer);
	CopysixfourtodbkYUV_1(lcu_x, lcu_pos, 
		dbk_y, dbk_u, dbk_v,
		dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v,
		dbk_ver_y, dbk_ver_u, dbk_ver_v,
		sixfour_buffer, U30x3_buffer, V30x3_buffer);
	Copyfoursixtodbk_Y_1(lcu_y, lcu_pos, lcu_stride, 
		dbk_y, dbk_u, dbk_v,
		dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v,
		dbk_ver_y, dbk_ver_u, dbk_ver_v,
		foursix_buffer, U3x30_buffer, V3x30_buffer);
}

void dbk2sao(ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16], ENC_ME_NEB_INFO inner_nebs_sao[2][16][16], ENC_ME_NEB_INFO inner_nebs_md_chroma_tmp[2][16][16],
		U32 map_scu_inner[2][16][16], U32 me_line_map_ptr_DBK_map_scu_inner[2][16][16]){
	S14 mv=0;
	S2 refi=0;
 #pragma HLS ARRAY_PARTITION variable=inner_nebs_md_chroma complete dim=3
  #pragma HLS ARRAY_PARTITION variable=inner_nebs_md_chroma_tmp complete dim=3
   #pragma HLS ARRAY_PARTITION variable=inner_nebs_sao complete dim=3
 
 	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
   for(int k=0;k<16;k++){
			for (int r = 0; r < 2; r++) {
				for (int s = 0; s < 2; s++) {
          #pragma HLS PIPELINE
						mv = inner_nebs_md_chroma[i][j][k].mv[r][s];
						inner_nebs_sao[i][j][k].mv[r][s]           = mv;
						inner_nebs_md_chroma_tmp[i][j][k].mv[r][s] = mv;
				}
          refi = inner_nebs_md_chroma[i][j][k].refi[r];
					inner_nebs_sao[i][j][k].refi[r] = refi;
					inner_nebs_md_chroma_tmp[i][j][k].refi[r] = refi;
			}
      }
		}
	}
 
   	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
   for(int k=0;k<16;k++){
       #pragma HLS PIPELINE
      	map_scu_inner[i][j][k] = me_line_map_ptr_DBK_map_scu_inner[i][j][k];
      }
		}
	}
}


static void Data_Prepare_2(U6 pic_width_in_lcu, U13 pic_width, U13 pic_height, U11 pic_width_in_scu,
	U13 lcu_x, U13 lcu_y, 
	//U6 line_map_split[5][7][256], 
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39],
	U13 lcu_pos, U6 lcu_stride,
	pel foursix_buffer[61][4][64], pel U3x30_buffer[61][4][36], pel V3x30_buffer[61][4][36], pel sixfour_buffer[2][64][4],
	pel U30x3_buffer[2][36][4], pel V30x3_buffer[2][36][4], pel fourfour_buffer[62][4][4], pel U3x3_buffer[62][4][4],
	pel V3x3_buffer[62][4][4], U2 ppbEdgeFilter_ctu_ver0[16][16], U2 ppbEdgeFilter_ctu_hor1[16][16],
	U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16], ENC_ME_NEB_INFO inner_nebs_sao[2][16][16], ENC_ME_NEB_INFO inner_nebs_md_chroma_tmp[2][16][16],
	U32 map_scu_inner[2][16][16], U32 me_line_map_ptr_DBK_map_scu_inner[2][16][16])
{
#pragma HLS inline off

	Cal_ppBEdgeFilter_2(ppbEdgeFilter_ctu_ver0, ppbEdgeFilter_ctu_hor1, pic_width_in_lcu, pic_width, pic_height, pic_width_in_scu, lcu_x, lcu_y, 
	//line_map_split, 
	cur_ctu_cu_mode);
	Copydbktolinebuffer_YUV_1(lcu_x, lcu_y, lcu_pos, lcu_stride, dbk_y, dbk_u, dbk_v, dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v,
		dbk_ver_y, dbk_ver_u, dbk_ver_v,
		foursix_buffer, U3x30_buffer, V3x30_buffer, sixfour_buffer,
		U30x3_buffer, V30x3_buffer, fourfour_buffer, U3x3_buffer, V3x3_buffer);
	dbk2sao(inner_nebs_md_chroma, inner_nebs_sao,inner_nebs_md_chroma_tmp, map_scu_inner, me_line_map_ptr_DBK_map_scu_inner);
}


static U32 MbQ_prepare_1(U11 mb_x, U1 curr_ctu_idx, U11 min_scu_x, S5 scu_x_in_line, S5 scu_y_in_line,
	U32 map_scu_above_line[48], U32 map_scu_inner[2][16][16])
{
#pragma HLS inline
	U32 MbQ;
	if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line >= 0 && scu_y_in_line <= 15) {
		MbQ = map_scu_inner[curr_ctu_idx][scu_x_in_line][scu_y_in_line];
	}
	else if (scu_x_in_line < 0 && scu_y_in_line >= 0 && scu_y_in_line <= 15) {
		MbQ = map_scu_inner[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line];
	}
	else {
		MbQ = map_scu_above_line[mb_x - min_scu_x + SCUX_OFFSET];
	}
	return MbQ;
}

static U32 MbP_prepare_hor(U11 mb_x, U1 dir, S5 scu_x_in_line, S5 scu_y_in_line, U1 curr_ctu_idx, U11 min_scu_x,
	U32 map_scu_above_line[48], U32 map_scu_inner[2][16][16])
{
#pragma HLS inline
	U32 MbP;
		//up
		if (scu_y_in_line == 0) {
			MbP = map_scu_above_line[mb_x - min_scu_x + SCUX_OFFSET];
		}
		else
			//right-down
		if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line > 0 && scu_y_in_line <= 15) {
			MbP = map_scu_inner[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1];
		}
		else
			//left-down
		{
			MbP = map_scu_inner[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1];
		}
    return MbP;
	}

static U32 MbP_prepare_ver(U11 mb_x, S5 scu_x_in_line, S5 scu_y_in_line, U1 curr_ctu_idx, U11 min_scu_x,
    U32 map_scu_above_line[48], U32 map_scu_inner[2][16][16])
{
#pragma HLS inline off
    U32 MbP;
		//up
		if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line < 0) {
			MbP = map_scu_above_line[mb_x - min_scu_x - 1 + SCUX_OFFSET];
		}
		else
			//right-down
		if (scu_x_in_line > 0 && scu_x_in_line <= 15 && scu_y_in_line >= 0 && scu_y_in_line <= 15) {
			MbP = map_scu_inner[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line];
		}
		else
			//left-down
		{
			MbP = map_scu_inner[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line];
	}
	return MbP;
}


static const int com_tbl_qp_chroma_adjust[64] = { (0), (1), (2), (3), (4), (5), (6), (7), (8), (9), (10), (11), (12), (13), (14), (15), (16), (17), (18), (19), (20), (21), (22), (23), (24), (25), (26), (27), (28), (29), (30), (31), (32), (33), (34), (35), (36), (37), (38), (39), (40), (41), (42), (42), (43), (43), (44), (44), (45), (45), (46), (46), (47), (47), (48), (48), (48), (49), (49), (49), (50), (50), (50), (51)
//this table aligned with spec
};

static U8 cQPus_prepare_1(U8 MbP_qp, U8 MbQ_qp, U3 chroma_quant_param_delta_cb, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth)
{
#pragma HLS inline

	U8 c_p_QPu = MbP_qp + chroma_quant_param_delta_cb - fetch2dbk_fw_ptr_qp_offset_bit_depth;
	U8 c_q_QPu = MbQ_qp + chroma_quant_param_delta_cb - fetch2dbk_fw_ptr_qp_offset_bit_depth;

	c_p_QPu = (c_p_QPu > 63 ? (U8)63 : ((c_p_QPu < 0 - 16 ? (U8)(0 - 16) : c_p_QPu)));
	c_q_QPu = (c_q_QPu > 63 ? (U8)63 : ((c_q_QPu < 0 - 16 ? (U8)(0 - 16) : c_q_QPu)));
	if (c_p_QPu >= 0) {
		c_p_QPu = com_tbl_qp_chroma_adjust[(63 < c_p_QPu ? (U8)63 : c_p_QPu)];
	}
	if (c_q_QPu >= 0) {
		c_q_QPu = com_tbl_qp_chroma_adjust[(63 < c_q_QPu ? (U8)63 : c_q_QPu)];
	}

	c_p_QPu = ((int)c_p_QPu + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth > (int)63 + fetch2dbk_fw_ptr_qp_offset_bit_depth ? (int)63 + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth : (((int)c_p_QPu < (int)0 ? (int)0 : (int)c_p_QPu + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth)));

	c_q_QPu = ((int)c_q_QPu + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth > (int)63 + fetch2dbk_fw_ptr_qp_offset_bit_depth ? (int)63 + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth : (((int)c_q_QPu < (int)0 ? (int)0 : (int)c_q_QPu + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth)));
	U8 cQPus = (c_p_QPu + c_q_QPu + 1) >> 1;

	return cQPus;
}

static U8 cQPvs_prepare_1(U8 MbP_qp, U8 MbQ_qp, U3 chroma_quant_param_delta_cr, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth)
{

#pragma HLS inline off

	U8 c_p_QPv = MbP_qp + chroma_quant_param_delta_cr - fetch2dbk_fw_ptr_qp_offset_bit_depth;
	U8 c_q_QPv = MbQ_qp + chroma_quant_param_delta_cr - fetch2dbk_fw_ptr_qp_offset_bit_depth;
	c_p_QPv = (c_p_QPv > 63 ? (U8)63 : ((c_p_QPv < 0 - 16 ? (U8)(0 - 16) : c_p_QPv)));
	c_q_QPv = (c_q_QPv > 63 ? (U8)63 : ((c_q_QPv < 0 - 16 ? (U8)(0 - 16) : c_q_QPv)));
	if (c_p_QPv >= 0) {
		c_p_QPv = com_tbl_qp_chroma_adjust[(63 < c_p_QPv ? (U8)63 : c_p_QPv)];
	}
	if (c_q_QPv >= 0) {
		c_q_QPv = com_tbl_qp_chroma_adjust[(63 < c_q_QPv ? (U8)63 : c_q_QPv)];
	}

	c_p_QPv = ((int)c_p_QPv + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth > (int)63 + fetch2dbk_fw_ptr_qp_offset_bit_depth ? (int)63 + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth : (((int)c_p_QPv < (int)0 ? (int)0 : (int)c_p_QPv + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth)));
	c_q_QPv = ((int)c_q_QPv + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth > (int)63 + fetch2dbk_fw_ptr_qp_offset_bit_depth ? (int)63 + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth : (((int)c_q_QPv < (int)0 ? (int)0 : (int)c_q_QPv + (int)fetch2dbk_fw_ptr_qp_offset_bit_depth)));
	U8 cQPvs = (c_p_QPv + c_q_QPv + 1) >> 1;
	return cQPvs;
}

static void EdgeLoopY(pel & L3, pel& L2, pel&L1, pel&L0, pel&R0, pel&R1, pel&R2, pel&R3, dbk_ushort Beta, dbk_ushort Alpha, U7 mb_x, U7 mb_y, U1 dir,
	U1 skipFilteringFlag)
{
#pragma HLS inline off
	//fs stands for filtering strength.  The larger fs is, the stronger filter is applied.
    pel xL3, xL2, xL1, xL0, xR0, xR1, xR2, xR3;
    xL3 = L3;
    xL2 = L2;
    xL1 = L1;
    xL0 = L0;
    xR0 = R0;
    xR1 = R1;
    xR2 = R2;
    xR3 = R3;
	U3 fs;
	U2 FlatnessL;
	U2 FlatnessR;
	pel AbsDelta = abs(R0 - L0);
	if (!skipFilteringFlag) {
		FlatnessL = (abs(L1 - L0) < Beta ? 2 : 0);
		if (abs(L2 - L0) < Beta) {
			FlatnessL++;
		}
		FlatnessR = (abs(R0 - R1) < Beta ? 2 : 0);
		if (abs(R0 - R2) < Beta) {
			FlatnessR++;
		}
		switch (FlatnessL + FlatnessR) {
		case 6:
			fs = (abs(R0 - R1) <= Beta >> 2 && abs(L0 - L1) <= Beta >> 2 && AbsDelta < Alpha ? 4 : 3);
			break;
		case 5:
			fs = (R1 == R0 && L0 == L1 ? 3 : 2);
			break;
		case 4:
			fs = (FlatnessL == 2 ? 2 : 1);
			break;
		case 3:
			fs = (abs(L1 - R1) < Beta ? 1 : 0);
			break;
		default:
			fs = 0;
			break;
		}
		{
			switch (fs) {
			case 4:
                xR0 = ((pel)((R2 * 3 + (R1 << 3) + R0 * 10 + (L0 << 3) + L1 * 3 + 16) >> 5));
                xL2 = ((pel)(((L3 << 1) + (L2 << 1) + (L1 << 1) + L0 + R0 + 4) >> 3));
                xR1 = ((pel)(((R2 << 2) + R1 * 5 + (R0 << 2) + L0 * 3 + 8) >> 4));
                xL1 = ((pel)(((L2 << 2) + L1 * 5 + (L0 << 2) + R0 * 3 + 8) >> 4));
                xL0 = ((pel)((L2 * 3 + (L1 << 3) + L0 * 10 + (R0 << 3) + R1 * 3 + 16) >> 5));
                xR2 = ((pel)(((R3 << 1) + (R2 << 1) + (R1 << 1) + R0 + L0 + 4) >> 3));
                break;
            case 3:
                xR0 = ((pel)((L1 + (L0 << 2) + (R0 << 2) + (R0 << 1) + (R1 << 2) + R2 + 8) >> 4));
                xL1 = ((pel)((L2 * 3 + (L1 << 3) + (L0 << 2) + R0 + 8) >> 4));
                xL0 = ((pel)((L2 + (L1 << 2) + (L0 << 2) + (L0 << 1) + (R0 << 2) + R1 + 8) >> 4));
                xR1 = ((pel)((R2 * 3 + (R1 << 3) + (R0 << 2) + L0 + 8) >> 4));
                break;
            case 2:
                xR0 = ((pel)(((L0 << 1) + L0 + (R0 << 3) + (R0 << 1) + (R1 << 1) + R1 + 8) >> 4));
                xL0 = ((pel)(((L1 << 1) + L1 + (L0 << 3) + (L0 << 1) + (R0 << 1) + R0 + 8) >> 4));

                break;
            case 1:
                xR0 = ((pel)((R0 * 3 + L0 + 2) >> 2));
                xL0 = ((pel)((L0 * 3 + R0 + 2) >> 2));

                break;
            default:
                break;
            }
        }

    }
    L3 = xL3;
    L2 = xL2;
    L1 = xL1;
    L0 = xL0;
    R0 = xR0;
    R1 = xR1;
    R2 = xR2;
    R3 = xR3;
    //if (dir == 0) {
    //    //dbkbuffer_y[mb_y][mb_x - 4] = xL3;
    //    dbkbuffer_y[mb_y][mb_x - 3] = xL2;
    //    dbkbuffer_y[mb_y][mb_x - 2] = xL1;
    //    dbkbuffer_y[mb_y][mb_x - 1] = xL0;
    //    dbkbuffer_y[mb_y][mb_x] = xR0;
    //    dbkbuffer_y[mb_y][mb_x + 1] = xR1;
    //    dbkbuffer_y[mb_y][mb_x + 2] = xR2;
    //    //dbkbuffer_y[mb_y][mb_x + 3] = xR3;
    //}
    //else {
    //    //dbkbuffer_y[mb_y - 4][mb_x] = xL3;
    //    dbkbuffer_y[mb_y - 3][mb_x] = xL2;
    //    dbkbuffer_y[mb_y - 2][mb_x] = xL1;
    //    dbkbuffer_y[mb_y - 1][mb_x] = xL0;
    //    dbkbuffer_y[mb_y][mb_x] = xR0;
    //    dbkbuffer_y[mb_y + 1][mb_x] = xR1;
    //    dbkbuffer_y[mb_y + 2][mb_x] = xR2;
    //    //dbkbuffer_y[mb_y + 3][mb_x] = xR3;
    //}
}

static dbk_ushort ALPHA_TABLE[64] = { ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)3), ((dbk_ushort)3), ((dbk_ushort)3), ((dbk_ushort)3), ((dbk_ushort)4), ((dbk_ushort)4), ((dbk_ushort)4), ((dbk_ushort)5), ((dbk_ushort)5), ((dbk_ushort)6), ((dbk_ushort)6), ((dbk_ushort)7), ((dbk_ushort)7), ((dbk_ushort)8), ((dbk_ushort)9), ((dbk_ushort)10), ((dbk_ushort)10), ((dbk_ushort)11), ((dbk_ushort)12), ((dbk_ushort)13), ((dbk_ushort)15), ((dbk_ushort)16), ((dbk_ushort)17), ((dbk_ushort)19), ((dbk_ushort)21), ((dbk_ushort)23), ((dbk_ushort)25), ((dbk_ushort)27), ((dbk_ushort)29), ((dbk_ushort)32), ((dbk_ushort)35), ((dbk_ushort)38), ((dbk_ushort)41), ((dbk_ushort)45), ((dbk_ushort)49), ((dbk_ushort)54), ((dbk_ushort)59) };
static dbk_ushort BETA_TABLE[64] = { ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)0), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)1), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)2), ((dbk_ushort)3), ((dbk_ushort)3), ((dbk_ushort)3), ((dbk_ushort)3), ((dbk_ushort)4), ((dbk_ushort)4), ((dbk_ushort)5), ((dbk_ushort)5), ((dbk_ushort)5), ((dbk_ushort)6), ((dbk_ushort)6), ((dbk_ushort)7), ((dbk_ushort)8), ((dbk_ushort)8), ((dbk_ushort)9), ((dbk_ushort)10), ((dbk_ushort)11), ((dbk_ushort)12), ((dbk_ushort)13), ((dbk_ushort)14), ((dbk_ushort)15), ((dbk_ushort)16), ((dbk_ushort)17), ((dbk_ushort)18), ((dbk_ushort)19), ((dbk_ushort)20), ((dbk_ushort)21), ((dbk_ushort)22), ((dbk_ushort)23), ((dbk_ushort)23), ((dbk_ushort)24), ((dbk_ushort)24), ((dbk_ushort)25), ((dbk_ushort)25), ((dbk_ushort)26), ((dbk_ushort)27) };

static void EdgeLoopX_Y(pel ybuffer[8][8], U8 QP, U1 dir, U1 skipFilteringFlag, U7 mb_y,
    U7 mb_x, U13 pic_width, U13 pic_height, U4 fetch2dbk_fw_ptr_bit_depth)
{

#pragma HLS inline off
	dbk_ushort Alpha;
	dbk_ushort Beta;
    //int     shift1 = 0;
	//int     skipFilteringFlag = 0;
	// FlatnessL and FlatnessR describe how flat the curve is of one codingUnit.
	int alpha_c_offset = 0;
	int beta_offset = 0;
	int shift = fetch2dbk_fw_ptr_bit_depth - 8;
    /*  Alpha = ALPHA_TABLE[COM_CLIP3(MIN_QUANT, MAX_QUANT_BASE, QP + alpha_c_offset)] << shift1;
        Beta = BETA_TABLE[COM_CLIP3(MIN_QUANT, MAX_QUANT_BASE, QP + beta_offset)] << shift1;*/
    Alpha = (ALPHA_TABLE[(0 > (((63 < QP + alpha_c_offset) ? (dbk_ushort)63 : (dbk_ushort)(QP + alpha_c_offset))) ? 0 : ((63 < QP + alpha_c_offset ? (dbk_ushort)63 : (dbk_ushort)(QP + alpha_c_offset))))]) << shift;
    Beta = (BETA_TABLE[(0 > ((63 < QP + beta_offset ? (dbk_ushort)63 : (dbk_ushort)(QP + beta_offset))) ? 0 : ((63 < QP + beta_offset ? (dbk_ushort)63 : (dbk_ushort)(QP + beta_offset))))]) << shift;
    U7 mb_y_1 = mb_y;
    U7 mb_x_1 = mb_x;
    U7 mb_y_2 = mb_y;
    U7 mb_x_2 = mb_x;
    U7 mb_y_3 = mb_y;
    U7 mb_x_3 = mb_x;
    if (dir == 0) {
        mb_y_1 = mb_y + 1;
        mb_y_2 = mb_y + 2;
        mb_y_3 = mb_y + 3;
        ///////////////////pixel 1///////////////////////////////////
        pel L13 = ybuffer[4][0];//[((int)dbkbuffer_y[mb_y][mb_x - 4]);
        pel L12 = ybuffer[4][1]; //((int)dbkbuffer_y[mb_y][mb_x - 3]);
        pel L11 = ybuffer[4][2];//((int)dbkbuffer_y[mb_y][mb_x - 2]);
        pel L10 = ybuffer[4][3];//((int)dbkbuffer_y[mb_y][mb_x - 1]);
        pel R10 = ybuffer[4][4];//((int)dbkbuffer_y[mb_y][mb_x]);
        pel R11 = ybuffer[4][5];//((int)dbkbuffer_y[mb_y][mb_x + 1]);
        pel R12 = ybuffer[4][6];//((int)dbkbuffer_y[mb_y][mb_x + 2]);
        pel R13 = ybuffer[4][7];//((int)dbkbuffer_y[mb_y][mb_x + 3]);
        EdgeLoopY(L13, L12, L11, L10, R10, R11, R12, R13, Beta, Alpha, mb_x, mb_y, dir, skipFilteringFlag);
        ybuffer[4][1] = L12;//dbkbuffer_y[mb_y][mb_x - 3] = L2;
        ybuffer[4][2] = L11;//dbkbuffer_y[mb_y][mb_x - 2] = L1;
        ybuffer[4][3] = L10;//dbkbuffer_y[mb_y][mb_x - 1] = L0;
        ybuffer[4][4] = R10;//dbkbuffer_y[mb_y][mb_x] = R0;
        ybuffer[4][5] = R11;//dbkbuffer_y[mb_y][mb_x + 1] = R1;
        ybuffer[4][6] = R12;//dbkbuffer_y[mb_y][mb_x + 2] = R2;
        ///////////////////pixel 2///////////////////////////////////
        pel L23 = ybuffer[5][0];//((int)dbkbuffer_y[mb_y + 1][mb_x - 4]);
        pel L22 = ybuffer[5][1];//((int)dbkbuffer_y[mb_y + 1][mb_x - 3]);
        pel L21 = ybuffer[5][2];//((int)dbkbuffer_y[mb_y + 1][mb_x - 2]);
        pel L20 = ybuffer[5][3];//((int)dbkbuffer_y[mb_y + 1][mb_x - 1]);
        pel R20 = ybuffer[5][4];//((int)dbkbuffer_y[mb_y + 1][mb_x]);
        pel R21 = ybuffer[5][5];//((int)dbkbuffer_y[mb_y + 1][mb_x + 1]);
        pel R22 = ybuffer[5][6];//((int)dbkbuffer_y[mb_y + 1][mb_x + 2]);
        pel R23 = ybuffer[5][7];//((int)dbkbuffer_y[mb_y + 1][mb_x + 3]);
        EdgeLoopY(L23, L22, L21, L20, R20, R21, R22, R23, Beta, Alpha, mb_x_1, mb_y_1, dir, skipFilteringFlag);
        ybuffer[5][1] = L22;//dbkbuffer_y[mb_y + 1][mb_x - 3] = L2;
        ybuffer[5][2] = L21;//dbkbuffer_y[mb_y + 1][mb_x - 2] = L1;
        ybuffer[5][3] = L20;//dbkbuffer_y[mb_y + 1][mb_x - 1] = L0;
        ybuffer[5][4] = R20;//dbkbuffer_y[mb_y + 1][mb_x] = R0;
        ybuffer[5][5] = R21;//dbkbuffer_y[mb_y + 1][mb_x + 1] = R1;
        ybuffer[5][6] = R22;//dbkbuffer_y[mb_y + 1][mb_x + 2] = R2;
        ///////////////////pixel 3///////////////////////////////////
        pel L33 = ybuffer[6][0];// ((int)dbkbuffer_y[mb_y + 2][mb_x - 4]);
        pel L32 = ybuffer[6][1];// ((int)dbkbuffer_y[mb_y + 2][mb_x - 3]);
        pel L31 = ybuffer[6][2];// ((int)dbkbuffer_y[mb_y + 2][mb_x - 2]);
        pel L30 = ybuffer[6][3];// ((int)dbkbuffer_y[mb_y + 2][mb_x - 1]);
        pel R30 = ybuffer[6][4];// ((int)dbkbuffer_y[mb_y + 2][mb_x]);
        pel R31 = ybuffer[6][5];// ((int)dbkbuffer_y[mb_y + 2][mb_x + 1]);
        pel R32 = ybuffer[6][6];// ((int)dbkbuffer_y[mb_y + 2][mb_x + 2]);
        pel R33 = ybuffer[6][7];// ((int)dbkbuffer_y[mb_y+2][mb_x + 3]);
        EdgeLoopY(L33, L32, L31, L30, R30, R31, R32, R33, Beta, Alpha, mb_x_2, mb_y_2, dir, skipFilteringFlag);
        ybuffer[6][1] = L32;//dbkbuffer_y[mb_y + 2][mb_x - 3] = L2;
        ybuffer[6][2] = L31;//dbkbuffer_y[mb_y + 2][mb_x - 2] = L1;
        ybuffer[6][3] = L30;//dbkbuffer_y[mb_y + 2][mb_x - 1] = L0;
        ybuffer[6][4] = R30;//dbkbuffer_y[mb_y + 2][mb_x] = R0;
        ybuffer[6][5] = R31;//dbkbuffer_y[mb_y + 2][mb_x + 1] = R1;
        ybuffer[6][6] = R32;//dbkbuffer_y[mb_y + 2][mb_x + 2] = R2;
        ///////////////////pixel 4///////////////////////////////////
        pel L43 = ybuffer[7][0];// ((int)dbkbuffer_y[mb_y + 3][mb_x - 4]);
        pel L42 = ybuffer[7][1];// ((int)dbkbuffer_y[mb_y + 3][mb_x - 3]);
        pel L41 = ybuffer[7][2];// ((int)dbkbuffer_y[mb_y + 3][mb_x - 2]);
        pel L40 = ybuffer[7][3];// ((int)dbkbuffer_y[mb_y + 3][mb_x - 1]);
        pel R40 = ybuffer[7][4];// ((int)dbkbuffer_y[mb_y + 3][mb_x]);
        pel R41 = ybuffer[7][5];// ((int)dbkbuffer_y[mb_y + 3][mb_x + 1]);
        pel R42 = ybuffer[7][6];// ((int)dbkbuffer_y[mb_y + 3][mb_x + 2]);
        pel R43 = ybuffer[7][7];// ((int)dbkbuffer_y[mb_y + 3][mb_x + 3]);
        EdgeLoopY(L43, L42, L41, L40, R40, R41, R42, R43, Beta, Alpha, mb_x_3, mb_y_3, dir, skipFilteringFlag);
        ybuffer[7][1] = L42;//dbkbuffer_y[mb_y + 3][mb_x - 3] = L2;
        ybuffer[7][2] = L41;//dbkbuffer_y[mb_y + 3][mb_x - 2] = L1;
        ybuffer[7][3] = L40;//dbkbuffer_y[mb_y + 3][mb_x - 1] = L0;
        ybuffer[7][4] = R40;//dbkbuffer_y[mb_y + 3][mb_x] = R0;
        ybuffer[7][5] = R41;//dbkbuffer_y[mb_y + 3][mb_x + 1] = R1;
        ybuffer[7][6] = R42;//dbkbuffer_y[mb_y + 3][mb_x + 2] = R2;
    }
    else {
        mb_x_1 = mb_x + 1;
        mb_x_2 = mb_x + 2;
        mb_x_3 = mb_x + 3;
        ///////////////////pixel 1///////////////////////////////////
        pel L13 = ybuffer[0][4];// ((int)dbkbuffer_y[mb_y - 4][mb_x]);
        pel L12 = ybuffer[1][4];// ((int)dbkbuffer_y[mb_y - 3][mb_x]);
        pel L11 = ybuffer[2][4];// ((int)dbkbuffer_y[mb_y - 2][mb_x]);
        pel L10 = ybuffer[3][4];// ((int)dbkbuffer_y[mb_y - 1][mb_x]);
        pel R10 = ybuffer[4][4];// ((int)dbkbuffer_y[mb_y][mb_x]);
        pel R11 = ybuffer[5][4];// ((int)dbkbuffer_y[mb_y + 1][mb_x]);
        pel R12 = ybuffer[6][4];// ((int)dbkbuffer_y[mb_y + 2][mb_x]);
        pel R13 = ybuffer[7][4];// ((int)dbkbuffer_y[mb_y + 3][mb_x]);
        EdgeLoopY(L13, L12, L11, L10, R10, R11, R12, R13, Beta, Alpha, mb_x, mb_y, dir, skipFilteringFlag);
        ybuffer[1][4] = L12;//dbkbuffer_y[mb_y - 3][mb_x] = L2;
        ybuffer[2][4] = L11;//dbkbuffer_y[mb_y - 2][mb_x] = L1;
        ybuffer[3][4] = L10;//dbkbuffer_y[mb_y - 1][mb_x] = L0;
        ybuffer[4][4] = R10;//dbkbuffer_y[mb_y][mb_x] = R0;
        ybuffer[5][4] = R11;//dbkbuffer_y[mb_y + 1][mb_x] = R1;
        ybuffer[6][4] = R12;//dbkbuffer_y[mb_y + 2][mb_x] = R2;
        ///////////////////pixel 2///////////////////////////////////
        pel L23 = ybuffer[0][5];// ((int)dbkbuffer_y[mb_y - 4][mb_x + 1]);
        pel L22 = ybuffer[1][5];// ((int)dbkbuffer_y[mb_y - 3][mb_x + 1]);
        pel L21 = ybuffer[2][5];// ((int)dbkbuffer_y[mb_y - 2][mb_x + 1]);
        pel L20 = ybuffer[3][5];// ((int)dbkbuffer_y[mb_y - 1][mb_x + 1]);
        pel R20 = ybuffer[4][5];// ((int)dbkbuffer_y[mb_y][mb_x + 1]);
        pel R21 = ybuffer[5][5];// ((int)dbkbuffer_y[mb_y + 1][mb_x + 1]);
        pel R22 = ybuffer[6][5];// ((int)dbkbuffer_y[mb_y + 2][mb_x + 1]);
        pel R23 = ybuffer[7][5];// ((int)dbkbuffer_y[mb_y + 3][mb_x + 1]);
        EdgeLoopY(L23, L22, L21, L20, R20, R21, R22, R23, Beta, Alpha, mb_x_1, mb_y_1, dir, skipFilteringFlag);
        ybuffer[1][5] = L22;//dbkbuffer_y[mb_y - 3][mb_x + 1] = L2;
        ybuffer[2][5] = L21;//dbkbuffer_y[mb_y - 2][mb_x + 1] = L1;
        ybuffer[3][5] = L20;//dbkbuffer_y[mb_y - 1][mb_x + 1] = L0;
        ybuffer[4][5] = R20;//dbkbuffer_y[mb_y][mb_x + 1] = R0;
        ybuffer[5][5] = R21;//dbkbuffer_y[mb_y + 1][mb_x + 1] = R1;
        ybuffer[6][5] = R22;//dbkbuffer_y[mb_y + 2][mb_x + 1] = R2;
        ///////////////////pixel 3///////////////////////////////////
        pel L33 = ybuffer[0][6];// ((int)dbkbuffer_y[mb_y - 4][mb_x + 2]);
        pel L32 = ybuffer[1][6];// ((int)dbkbuffer_y[mb_y - 3][mb_x + 2]);
        pel L31 = ybuffer[2][6];// ((int)dbkbuffer_y[mb_y - 2][mb_x + 2]);
        pel L30 = ybuffer[3][6];// ((int)dbkbuffer_y[mb_y - 1][mb_x + 2]);
        pel R30 = ybuffer[4][6];// ((int)dbkbuffer_y[mb_y][mb_x + 2]);
        pel R31 = ybuffer[5][6];// ((int)dbkbuffer_y[mb_y + 1][mb_x + 2]);
        pel R32 = ybuffer[6][6];// ((int)dbkbuffer_y[mb_y + 2][mb_x + 2]);
        pel R33 = ybuffer[7][6];// ((int)dbkbuffer_y[mb_y + 3][mb_x + 2]);
        EdgeLoopY(L33, L32, L31, L30, R30, R31, R32, R33, Beta, Alpha, mb_x_2, mb_y_2, dir, skipFilteringFlag);
        ybuffer[1][6] = L32;//dbkbuffer_y[mb_y - 3][mb_x + 2] = L2;
        ybuffer[2][6] = L31;//dbkbuffer_y[mb_y - 2][mb_x + 2] = L1;
        ybuffer[3][6] = L30;//dbkbuffer_y[mb_y - 1][mb_x + 2] = L0;
        ybuffer[4][6] = R30;//dbkbuffer_y[mb_y][mb_x + 2] = R0;
        ybuffer[5][6] = R31;//dbkbuffer_y[mb_y + 1][mb_x + 2] = R1;
        ybuffer[6][6] = R32;//dbkbuffer_y[mb_y + 2][mb_x + 2] = R2;
        ///////////////////pixel 4///////////////////////////////////
        pel L43 = ybuffer[0][7]; //((int)dbkbuffer_y[mb_y - 4][mb_x + 3]);
        pel L42 = ybuffer[1][7]; //((int)dbkbuffer_y[mb_y - 3][mb_x + 3]);
        pel L41 = ybuffer[2][7]; //((int)dbkbuffer_y[mb_y - 2][mb_x + 3]);
        pel L40 = ybuffer[3][7]; //((int)dbkbuffer_y[mb_y - 1][mb_x + 3]);
        pel R40 = ybuffer[4][7]; //((int)dbkbuffer_y[mb_y][mb_x + 3]);
        pel R41 = ybuffer[5][7]; //((int)dbkbuffer_y[mb_y + 1][mb_x + 3]);
        pel R42 = ybuffer[6][7]; //((int)dbkbuffer_y[mb_y + 2][mb_x + 3]);
        pel R43 = ybuffer[7][7]; //((int)dbkbuffer_y[mb_y + 3][mb_x+3]);
        EdgeLoopY(L43, L42, L41, L40, R40, R41, R42, R43, Beta, Alpha, mb_x_3, mb_y_3, dir, skipFilteringFlag);
        ybuffer[1][7] = L42;//dbkbuffer_y[mb_y - 3][mb_x + 3] = L2;
        ybuffer[2][7] = L41;//dbkbuffer_y[mb_y - 2][mb_x + 3] = L1;
        ybuffer[3][7] = L40;//dbkbuffer_y[mb_y - 1][mb_x + 3] = L0;
        ybuffer[4][7] = R40;//dbkbuffer_y[mb_y][mb_x + 3] = R0;
        ybuffer[5][7] = R41;//dbkbuffer_y[mb_y + 1][mb_x + 3] = R1;
        ybuffer[6][7] = R42;//dbkbuffer_y[mb_y + 2][mb_x + 3] = R2;
    }
}

static void EdgeLoopU(pel&L2, pel&L1, pel&L0, pel& R0, pel&R1, pel&R2, dbk_ushort Beta, dbk_ushort Alpha, U7 mb_x, U7 mb_y, U1 dir,
    U1 skipFilteringFlag)
{

#pragma HLS inline off
    pel AbsDelta;
    pel xL2 = L2;
    pel xL1 = L1;
    pel xL0 = L0;
    pel xR0 = R0;
    pel xR1 = R1;
    pel xR2 = R2;
    //fs stands for filtering strength.  The larger fs is, the stronger filter is applied.
    U3 fs;
    U2 FlatnessL;
	U2 FlatnessR;
    AbsDelta = abs(R0 - L0);
    if (!skipFilteringFlag) {
        FlatnessL = (abs(L1 - L0) < Beta ? 2 : 0);
        if (abs(L2 - L0) < Beta) {
            FlatnessL++;
        }
		FlatnessR = (abs(R0 - R1) < Beta ? 2 : 0);
		if (abs(R0 - R2) < Beta) {
			FlatnessR++;
		}
		switch (FlatnessL + FlatnessR) {
		case 6:
			fs = (abs(R0 - R1) <= Beta >> 2 && abs(L0 - L1) <= Beta >> 2 && AbsDelta < Alpha ? 4 : 3);
			break;
		case 5:
			fs = (R1 == R0 && L0 == L1 ? 3 : 2);
			break;
		case 4:
			fs = (FlatnessL == 2 ? 2 : 1);
			break;
		case 3:
			fs = (abs(L1 - R1) < Beta ? 1 : 0);
			break;
		default:
			fs = 0;
			break;
		}
		if (fs > 0) {
            fs--;
        }
        switch (fs) {
        case 3:
                xL1 = ((pel)((L2 * 3 + (L1 << 3) + L0 * 3 + (R0 << 1) + 8) >> 4));
                xR1 = ((pel)((R2 * 3 + (R1 << 3) + R0 * 3 + (L0 << 1) + 8) >> 4));
        case 2:
            ;
        case 1:
                xL0 = ((pel)((L1 * 3 + L0 * 10 + R0 * 3 + 8) >> 4));
                xR0 = ((pel)((R1 * 3 + R0 * 10 + L0 * 3 + 8) >> 4));
            break;
        default:
            break;
        }
    }
    L2 = xL2;
    L1 = xL1;
    L0 = xL0;
    R0 = xR0;
    R1 = xR1;
    R2 = xR2;
}

static void EdgeLoopV(pel & L2, pel& L1, pel& L0, pel& R0, pel& R1, pel& R2, dbk_ushort Beta, dbk_ushort Alpha, U7 mb_x, U7 mb_y, U1 dir,
    U1 skipFilteringFlag)
{

#pragma HLS inline off
    pel AbsDelta;
    pel xL2 = L2;
    pel xL1 = L1;
    pel xL0 = L0;
    pel xR0 = R0;
    pel xR1 = R1;
    pel xR2 = R2;
    //fs stands for filtering strength.  The larger fs is, the stronger filter is applied.
    U3 fs;
    U2 FlatnessL;
    U2 FlatnessR;
    AbsDelta = abs(R0 - L0);
    if (!skipFilteringFlag) {
        FlatnessL = (abs(L1 - L0) < Beta ? 2 : 0);
        if (abs(L2 - L0) < Beta) {
            FlatnessL++;
        }
		FlatnessR = (abs(R0 - R1) < Beta ? 2 : 0);
		if (abs(R0 - R2) < Beta) {
			FlatnessR++;
		}
		switch (FlatnessL + FlatnessR) {
		case 6:
			fs = (abs(R0 - R1) <= Beta >> 2 && abs(L0 - L1) <= Beta >> 2 && AbsDelta < Alpha ? 4 : 3);
			break;
		case 5:
			fs = (R1 == R0 && L0 == L1 ? 3 : 2);
			break;
		case 4:
			fs = (FlatnessL == 2 ? 2 : 1);
			break;
		case 3:
			fs = (abs(L1 - R1) < Beta ? 1 : 0);
			break;
		default:
			fs = 0;
			break;
        }
        if (fs > 0) {
            fs--;
        }
        switch (fs) {
        case 3:
            xL1 = ((pel)((L2 * 3 + (L1 << 3) + L0 * 3 + (R0 << 1) + 8) >> 4));
            xR1 = ((pel)((R2 * 3 + (R1 << 3) + R0 * 3 + (L0 << 1) + 8) >> 4));
        case 2:
            ;
        case 1:
            xL0 = ((pel)((L1 * 3 + L0 * 10 + R0 * 3 + 8) >> 4));
            xR0 = ((pel)((R1 * 3 + R0 * 10 + L0 * 3 + 8) >> 4));
            break;
        default:
            break;
        }
    }
    L2 = xL2;
    L1 = xL1;
    L0 = xL0;
    R0 = xR0;
    R1 = xR1;
    R2 = xR2;
}

static void EdgeLoopX_U(pel ubuffer[6][6], U8 QP, U1 dir, U1 skipFilteringFlag, U7 mb_y,
    U7 mb_x, U13 pic_width, U13 pic_height, U4 fetch2dbk_fw_ptr_bit_depth)
{

#pragma HLS inline off
    dbk_ushort Alpha;
    dbk_ushort Beta;
    // int     shift1 = 0;
	//int     skipFilteringFlag = 0;
	// FlatnessL and FlatnessR describe how flat the curve is of one codingUnit.
	int alpha_c_offset = 0;
	int beta_offset = 0;
	int shift = fetch2dbk_fw_ptr_bit_depth - 8;
	/* Alpha = ALPHA_TABLE[COM_CLIP3(MIN_QUANT, MAX_QUANT_BASE, QP + alpha_c_offset)] << shift1;
        Beta = BETA_TABLE[COM_CLIP3(MIN_QUANT, MAX_QUANT_BASE, QP + beta_offset)] << shift1;*/
    Alpha = ALPHA_TABLE[(0 > ((63 < QP + alpha_c_offset ? 63 : (dbk_ushort)(QP + alpha_c_offset))) ? 0 : (dbk_ushort)((
        63 < QP + alpha_c_offset ? 63 : (dbk_ushort)(QP + alpha_c_offset))))] << shift;
    Beta = BETA_TABLE[(0 > ((63 < QP + beta_offset ? (dbk_ushort)63 : (dbk_ushort)(QP + beta_offset))) ? 0 : ((
        63 < QP + beta_offset ? 63 : (dbk_ushort)(QP + beta_offset))))] << shift;

    U7 mb_y_2 = mb_y;
    U7 mb_x_2 = mb_x;
    if (dir == 0) {
        mb_y_2 = mb_y + 1;
        pel L12 =ubuffer[3][0];// (dbkbuffer_u[mb_y][mb_x - 3]);
        pel L11 =ubuffer[3][1];// (dbkbuffer_u[mb_y][mb_x - 2]);
        pel L10 =ubuffer[3][2];// (dbkbuffer_u[mb_y][mb_x - 1]);
        pel R10 =ubuffer[3][3];// (dbkbuffer_u[mb_y][mb_x]);
        pel R11 =ubuffer[3][4];// (dbkbuffer_u[mb_y][mb_x + 1]);
        pel R12 =ubuffer[3][5];// (dbkbuffer_u[mb_y][mb_x + 2]);
        EdgeLoopU(L12, L11, L10, R10, R11, R12, Beta, Alpha, mb_x, mb_y, dir, skipFilteringFlag);
        //dbkbuffer_u[mb_y][mb_x - 3])= L2;
        ubuffer[3][1] = L11;//dbkbuffer_u[mb_y][mb_x - 2] = L11;
        ubuffer[3][2] = L10;//dbkbuffer_u[mb_y][mb_x - 1] = L10;
        ubuffer[3][3] = R10;//    dbkbuffer_u[mb_y][mb_x] = R10;
        ubuffer[3][4] = R11;//dbkbuffer_u[mb_y][mb_x + 1] = R11;
        //dbkbuffer_u[mb_y][mb_x + 2]) = R2;
        pel L22 = ubuffer[4][0];
        pel L21 = ubuffer[4][1];
        pel L20 = ubuffer[4][2];
        pel R20 = ubuffer[4][3];
        pel R21 = ubuffer[4][4];
        pel R22 = ubuffer[4][5];
        EdgeLoopU(L22, L21, L20, R20, R21, R22, Beta, Alpha, mb_x_2, mb_y_2, dir, skipFilteringFlag);
        ubuffer[4][1] = L21;
        ubuffer[4][2] = L20;
        ubuffer[4][3] = R20;
        ubuffer[4][4] = R21;
    }
    else {
        mb_x_2 = mb_x + 1;
        pel L12 = ubuffer[0][3];
        pel L11 = ubuffer[1][3];
        pel L10 = ubuffer[2][3];
        pel R10 = ubuffer[3][3];
        pel R11 = ubuffer[4][3];
        pel R12 = ubuffer[5][3];
        EdgeLoopU(L12, L11, L10, R10, R11, R12, Beta, Alpha, mb_x, mb_y, dir, skipFilteringFlag);
        //dbkbuffer_u[mb_y][mb_x - 3])= L2;
        ubuffer[1][3] = L11;
        ubuffer[2][3] = L10;
        ubuffer[3][3] = R10;
        ubuffer[4][3] = R11;
        pel L22 = (ubuffer[0][4]);
        pel L21 = (ubuffer[1][4]);
        pel L20 = (ubuffer[2][4]);
        pel R20 = (ubuffer[3][4]);
        pel R21 = (ubuffer[4][4]);
        pel R22 = (ubuffer[5][4]);
        EdgeLoopU(L22, L21, L20, R20, R21, R22, Beta, Alpha, mb_x_2, mb_y_2, dir, skipFilteringFlag);
        ubuffer[1][4] = L21;
        ubuffer[2][4] = L20;
        ubuffer[3][4] = R20;
        ubuffer[4][4] = R21;
    }
}

static void EdgeLoopX_V(pel vbuffer[6][6], U8 QP, U1 dir, U1 skipFilteringFlag, U7 mb_y,
    U7 mb_x, U13 pic_width, U13 pic_height ,U4 fetch2dbk_fw_ptr_bit_depth)
{

#pragma HLS inline off
    dbk_ushort Alpha;
    dbk_ushort Beta;
	//  int     shift1 = 0;
	//int     skipFilteringFlag = 0;
	// FlatnessL and FlatnessR describe how flat the curve is of one codingUnit.
	int alpha_c_offset = 0;
	int beta_offset = 0;
	int shift = fetch2dbk_fw_ptr_bit_depth - 8;
	/*  Alpha = ALPHA_TABLE[COM_CLIP3(MIN_QUANT, MAX_QUANT_BASE, QP + alpha_c_offset)] << shift1;
	Beta = BETA_TABLE[COM_CLIP3(MIN_QUANT, MAX_QUANT_BASE, QP + beta_offset)] << shift1;*/
	Alpha = (ALPHA_TABLE[(0 > ((63 < (dbk_ushort)(QP + alpha_c_offset) ? 63 : (dbk_ushort)(QP + alpha_c_offset))) ? 0 : ((
		63 < QP + alpha_c_offset ? 63 : (dbk_ushort)(QP + alpha_c_offset))))]) << shift;
	Beta = (BETA_TABLE[(0 > ((63 < (dbk_ushort)(QP + beta_offset) ? 63 : (dbk_ushort)(QP + beta_offset))) ? 0 : ((
		63 < (dbk_ushort)(QP + beta_offset) ? 63 : (dbk_ushort)(QP + beta_offset))))]) << shift;

    U7 mb_y_2 = mb_y;
    U7 mb_x_2 = mb_x;
    if (dir == 0) {
        mb_y_2 = mb_y + 1;
        pel L12 = vbuffer[3][0];// (dbkbuffer_u[mb_y][mb_x - 3]);
        pel L11 = vbuffer[3][1];// (dbkbuffer_u[mb_y][mb_x - 2]);
        pel L10 = vbuffer[3][2];// (dbkbuffer_u[mb_y][mb_x - 1]);
        pel R10 = vbuffer[3][3];// (dbkbuffer_u[mb_y][mb_x]);
        pel R11 = vbuffer[3][4];// (dbkbuffer_u[mb_y][mb_x + 1]);
        pel R12 = vbuffer[3][5];// (dbkbuffer_u[mb_y][mb_x + 2]);
        EdgeLoopV(L12, L11, L10, R10, R11, R12, Beta, Alpha, mb_x, mb_y, dir, skipFilteringFlag);
        //dbkbuffer_u[mb_y][mb_x - 3])= L2;
        vbuffer[3][1] = L11;//dbkbuffer_u[mb_y][mb_x - 2] = L11;
        vbuffer[3][2] = L10;//dbkbuffer_u[mb_y][mb_x - 1] = L10;
        vbuffer[3][3] = R10;//    dbkbuffer_u[mb_y][mb_x] = R10;
        vbuffer[3][4] = R11;//dbkbuffer_u[mb_y][mb_x + 1] = R11;
        //dbkbuffer_u[mb_y][mb_x + 2]) = R2;
        pel L22 = vbuffer[4][0];
        pel L21 = vbuffer[4][1];
        pel L20 = vbuffer[4][2];
        pel R20 = vbuffer[4][3];
        pel R21 = vbuffer[4][4];
        pel R22 = vbuffer[4][5];
        EdgeLoopV(L22, L21, L20, R20, R21, R22, Beta, Alpha, mb_x_2, mb_y_2, dir, skipFilteringFlag);
        vbuffer[4][1] = L21;
        vbuffer[4][2] = L20;
        vbuffer[4][3] = R20;
        vbuffer[4][4] = R21;
    }
    else {
        mb_x_2 = mb_x + 1;
        pel L12 = vbuffer[0][3];
        pel L11 = vbuffer[1][3];
        pel L10 = vbuffer[2][3];
        pel R10 = vbuffer[3][3];
        pel R11 = vbuffer[4][3];
        pel R12 = vbuffer[5][3];
        EdgeLoopV(L12, L11, L10, R10, R11, R12, Beta, Alpha, mb_x, mb_y, dir, skipFilteringFlag);
        //dbkbuffer_u[mb_y][mb_x - 3])= L2;
        vbuffer[1][3] = L11;
        vbuffer[2][3] = L10;
        vbuffer[3][3] = R10;
        vbuffer[4][3] = R11;
        pel L22 = (vbuffer[0][4]);
        pel L21 = (vbuffer[1][4]);
        pel L20 = (vbuffer[2][4]);
        pel R20 = (vbuffer[3][4]);
        pel R21 = (vbuffer[4][4]);
        pel R22 = (vbuffer[5][4]);
        EdgeLoopV(L22, L21, L20, R20, R21, R22, Beta, Alpha, mb_x_2, mb_y_2, dir, skipFilteringFlag);
        vbuffer[1][4] = L21;
        vbuffer[2][4] = L20;
        vbuffer[3][4] = R20;
        vbuffer[4][4] = R21;
    }

}



static void Copy_UV3x3_4x2_verandhor_1(U13 lcu_y, U13 lcu_pos, U6 lcu_stride, pel U3x3_buffer[62][4][4],
	pel V3x3_buffer[62][4][4], pel dbk_u[39][39], pel dbk_v[39][39], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
#pragma HLS inline off
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_u_temp complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_v_temp complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=U3x3_buffer complete dim=3
	//#pragma HLS ARRAY_PARTITION variable=V3x3_buffer complete dim=3
	int i;
	int j;
	if (lcu_y > 0) {
		// right short line 29,29-31
		for (i = 2; i < 4; ++i) {
#pragma HLS PIPELINE
			for (j = 0; j < 2; ++j) {
#pragma HLS UNROLL
				//U3x3_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 2)][i][j] = dbk_u[i+2][34 + j];
				U3x3_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 2)][i][j] = dbk_mid_u[i + 2][34 + j];
				//V3x3_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 2)][i][1] = dbk_v[i+2][j + 34];
				V3x3_buffer[(lcu_pos - lcu_stride) % (lcu_stride + 2)][i][1] = dbk_mid_v[i + 2][j + 34];
			}
		}
	}
}

static void Copy_UV30x3_verandhor_1(U13 lcu_pos, U6 lcu_stride, pel U30x3_buffer[2][36][4], pel V30x3_buffer[2][36][4],
	pel dbk_u[39][39], pel dbk_v[39][39], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
#pragma HLS inline off
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_u_temp complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_v_temp complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=U30x3_buffer complete dim=3
	//#pragma HLS ARRAY_PARTITION variable=V30x3_buffer complete dim=3
	int i;
	int j;
	//restore right line 29
	for (i = 0; i < 32; ++i) {
#pragma HLS PIPELINE
		for (j = 0; j < 2; ++j) {
#pragma HLS UNROLL
			U30x3_buffer[lcu_pos % 2][i][j] = dbk_mid_u[i + 6][j + 34];
			//U30x3_buffer[lcu_pos % 2][i][j] = dbk_u[i + 6][j + 34];
			V30x3_buffer[lcu_pos % 2][i][j] = dbk_mid_v[i + 6][j + 34];
			//V30x3_buffer[lcu_pos % 2][i][j] = dbk_v[i + 6][j + 34];
		}
	}
}


static void Copy_UV_verandhor_1(U13 lcu_y, U13 lcu_pos, U6 lcu_stride, pel U30x3_buffer[2][36][4],
	pel V30x3_buffer[2][36][4], pel U3x3_buffer[62][4][4], pel V3x3_buffer[62][4][4], 
	pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39]
)
{
#pragma HLS inline off
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_u_temp complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_v_temp complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=U30x3_buffer complete dim=3
	//#pragma HLS ARRAY_PARTITION variable=V30x3_buffer complete dim=3
	//#pragma HLS ARRAY_PARTITION variable=U3x3_buffer complete dim=3
	//#pragma HLS ARRAY_PARTITION variable=V3x3_buffer complete dim=3

	Copy_UV3x3_4x2_verandhor_1(lcu_y, lcu_pos, lcu_stride, U3x3_buffer, V3x3_buffer, dbk_u, dbk_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
	Copy_UV30x3_verandhor_1(lcu_pos, lcu_stride, U30x3_buffer, V30x3_buffer, dbk_u, dbk_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
}

static void Copy_UV3x30_3x3_1(U13 lcu_pos, U6 lcu_stride, pel U3x30_buffer[61][4][36], pel V3x30_buffer[61][4][36],
	pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39]
)
{

#pragma HLS inline off
	int i;
	int j;
	for (j = 0; j < 32; ++j)
#pragma HLS PIPELINE
	for (i = 0; i < 2; ++i) {
		{
			//U3x30_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_u[34 + i][j + 6];
			U3x30_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_u[34 + i][j + 6];
			//V3x30_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_v[34 + i][j + 6];
			V3x30_buffer[lcu_pos % (lcu_stride + 1)][i][j] = dbk_mid_v[34 + i][j + 6];
		}
	}
}

static void Copy_UV3x3_1(U13 lcu_pos, U6 lcu_stride, pel U3x3_buffer[62][4][4], pel V3x3_buffer[62][4][4],
	pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39]
)
{
#pragma HLS inline off

	int i;
	int j;
	for (j = 0; j < 2; ++j)
#pragma HLS PIPELINE
	for (i = 0; i < 2; ++i) {
		{
			//U3x3_buffer[lcu_pos % (lcu_stride + 2)][j][i] = dbk_u[34 + i][34 + j];
			U3x3_buffer[lcu_pos % (lcu_stride + 2)][j][i] = dbk_mid_u[34 + i][34 + j];

			//V3x3_buffer[lcu_pos % (lcu_stride + 2)][j][i] = dbk_v[34 + i][34 + j];
			V3x3_buffer[lcu_pos % (lcu_stride + 2)][j][i] = dbk_mid_v[34 + i][34 + j];
		}
	}
}

static void Copy_UV3x3_4x2_1(U13 lcu_x, U13 lcu_pos, U6 lcu_stride, pel U3x3_buffer[62][4][4],pel V3x3_buffer[62][4][4],
	pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39]
)
{
#pragma HLS inline off

	int i;
	int j;
	if (lcu_x > 0) {
		// bottom line 30-31,28-29 //
		for (j = 2; j < 4; ++j) {
			for (i = 0; i < 2; ++i) {
#pragma HLS PIPELINE
				//U3x3_buffer[(lcu_pos - 1) % (lcu_stride + 2)][i][j] = dbk_u[34 + i][j + 2];
				U3x3_buffer[(lcu_pos - 1) % (lcu_stride + 2)][i][j] = dbk_mid_u[34 + i][j + 2];

				//V3x3_buffer[(lcu_pos - 1) % (lcu_stride + 2)][i][j] = dbk_v[34 + i][j + 2];
				V3x3_buffer[(lcu_pos - 1) % (lcu_stride + 2)][i][j] = dbk_mid_v[34 + i][j + 2];
			}
		}
	}
}


void CopyppbEdgefilter_buf(U13 lcu_x, U2 ppbEdgeFilter_ctu_buf2_ver0[960],
	U2 ppbEdgeFilter_ctu_ver0[16][16], U2 ppbEdgeFilter_ctu_buf1_ver0[16], U2 ppbEdgeFilter_ctu_buf2_hor1[960],
	U2 ppbEdgeFilter_ctu_hor1[16][16], U2 ppbEdgeFilter_ctu_buf1_hor1[16])
{
#pragma HLS inline off

	for (int DBKb4_y = 0; DBKb4_y < 16; DBKb4_y++) {
#pragma HLS loop_tripcount max=16
#pragma HLS PIPELINE
		ppbEdgeFilter_ctu_buf1_ver0[DBKb4_y] = ppbEdgeFilter_ctu_ver0[DBKb4_y][15];
		ppbEdgeFilter_ctu_buf1_hor1[DBKb4_y] = ppbEdgeFilter_ctu_hor1[DBKb4_y][15];
		ppbEdgeFilter_ctu_buf2_ver0[DBKb4_y + (lcu_x >> 2)] = ppbEdgeFilter_ctu_ver0[15][DBKb4_y];
		ppbEdgeFilter_ctu_buf2_hor1[DBKb4_y + (lcu_x >> 2)] = ppbEdgeFilter_ctu_hor1[15][DBKb4_y];
	}
}


static void CopyData_after_hor_2(U13 lcu_x, U13 lcu_pos, U6 lcu_stride, pel U3x3_buffer[62][4][4],
	pel V3x3_buffer[62][4][4], pel U3x30_buffer[61][4][36], pel V3x30_buffer[61][4][36], 
	pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39],
	U2 ppbEdgeFilter_ctu_buf2_ver0[960],
	U2 ppbEdgeFilter_ctu_ver0[16][16], U2 ppbEdgeFilter_ctu_buf1_ver0[16], U2 ppbEdgeFilter_ctu_buf2_hor1[960],
	U2 ppbEdgeFilter_ctu_hor1[16][16], U2 ppbEdgeFilter_ctu_buf1_hor1[16])
{
#pragma HLS inline off

	Copy_UV3x30_3x3_1(lcu_pos, lcu_stride, U3x30_buffer, V3x30_buffer, dbk_u, dbk_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
	Copy_UV3x3_1(lcu_pos, lcu_stride, U3x3_buffer, V3x3_buffer, dbk_u, dbk_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
	Copy_UV3x3_4x2_1(lcu_x, lcu_pos, lcu_stride, U3x3_buffer, V3x3_buffer, dbk_u, dbk_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
	CopyppbEdgefilter_buf(lcu_x, ppbEdgeFilter_ctu_buf2_ver0,
		ppbEdgeFilter_ctu_ver0, ppbEdgeFilter_ctu_buf1_ver0, ppbEdgeFilter_ctu_buf2_hor1,
		ppbEdgeFilter_ctu_hor1, ppbEdgeFilter_ctu_buf1_hor1);
}

#if ENABLE_BFRAME
U1 SkipFiltering_ver0(U32 MbP, U32 MbQ, S8 refiP[REFP_NUM], S8 refiQ[REFP_NUM], S16 mvP[REFP_NUM][2], S16 mvQ[REFP_NUM][2], int refp[MAX_NUM_REF_PICS][REFP_NUM])
{
#pragma HLS inline off

	//COM_PIC* p_pic0 = REFI_IS_VALID(refiP[REFP_0]) ? refp[refiP[REFP_0]][REFP_0].pic : NULL;
	//COM_PIC* p_pic1 = REFI_IS_VALID(refiP[REFP_1]) ? refp[refiP[REFP_1]][REFP_1].pic : NULL;
	//COM_PIC* q_pic0 = REFI_IS_VALID(refiQ[REFP_0]) ? refp[refiQ[REFP_0]][REFP_0].pic : NULL;
	//COM_PIC* q_pic1 = REFI_IS_VALID(refiQ[REFP_1]) ? refp[refiQ[REFP_1]][REFP_1].pic : NULL;
 
 	int p_pic0 = REFI_IS_VALID(refiP[REFP_0]) ? refp[refiP[REFP_0]][REFP_0] : -1;
	int p_pic1 = REFI_IS_VALID(refiP[REFP_1]) ? refp[refiP[REFP_1]][REFP_1] : -1;
	int q_pic0 = REFI_IS_VALID(refiQ[REFP_0]) ? refp[refiQ[REFP_0]][REFP_0] : -1;
	int q_pic1 = REFI_IS_VALID(refiQ[REFP_1]) ? refp[refiQ[REFP_1]][REFP_1] : -1;


	int reflist;


	if ((p_pic0 == -1 && p_pic1 == -1) || (q_pic0 == -1 && q_pic1 == -1))
	{
		return 0;
	}
	if (MCU_GET_CBF(MbP) || MCU_GET_CBF(MbQ))
	{
		return 0;
	}

	if (p_pic0 == q_pic0 && p_pic1 == q_pic1) {
		for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 2
			if (REFI_IS_VALID(refiP[reflist])) {
				if ((abs(mvP[reflist][MV_X] - mvQ[reflist][MV_X]) >= 4) ||
					(abs(mvP[reflist][MV_Y] - mvQ[reflist][MV_Y]) >= 4)) {
					return 0;
				}
			}
		}
		return 1;
	}

	if (p_pic0 == q_pic1 && p_pic1 == q_pic0) {
		for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 2
			if (REFI_IS_VALID(refiP[reflist])) {
				if ((abs(mvP[reflist][MV_X] - mvQ[!reflist][MV_X]) >= 4) ||
					(abs(mvP[reflist][MV_Y] - mvQ[!reflist][MV_Y]) >= 4)) {
					return 0;
				}
			}
		}
		return 1;
	}
	//if (p_pic0 == q_pic1 && p_pic1 == q_pic0)
	//    if (p_pic0 == q_pic1 && p_pic1 == q_pic0)
	//    {
	//        for (reflist = REFP_0; reflist < 1; reflist++)
	//        {
	//#pragma HLS UNROLL
	//#pragma HLS loop_tripcount max = 2
	//            if (REFI_IS_VALID(refiP[reflist]))
	//            {
	//                if ((abs(mvP[reflist][MV_X] - mvQ[!reflist][MV_X]) >= 4) ||
	//                    (abs(mvP[reflist][MV_Y] - mvQ[!reflist][MV_Y]) >= 4))
	//                {
	//                    return 0;
	//                }
	//            }
	//        }
	//        return 1;
	//    }
	return 0;
}

U1 SkipFiltering_hor1(U32 MbP, U32 MbQ, S8 refiP[REFP_NUM], S8 refiQ[REFP_NUM], S16 mvP[REFP_NUM][2], S16 mvQ[REFP_NUM][2], int refp[MAX_NUM_REF_PICS][REFP_NUM])
{
#pragma HLS inline off

	int p_pic0 = REFI_IS_VALID(refiP[REFP_0]) ? refp[refiP[REFP_0]][REFP_0] : -1;
	int p_pic1 = REFI_IS_VALID(refiP[REFP_1]) ? refp[refiP[REFP_1]][REFP_1] : -1;
	int q_pic0 = REFI_IS_VALID(refiQ[REFP_0]) ? refp[refiQ[REFP_0]][REFP_0] : -1;
	int q_pic1 = REFI_IS_VALID(refiQ[REFP_1]) ? refp[refiQ[REFP_1]][REFP_1] : -1;


	int reflist;


	if ((p_pic0 == -1 && p_pic1 == -1) || (q_pic0 == -1 && q_pic1 == -1))
	{
		return 0;
	}
	if (MCU_GET_CBF(MbP) || MCU_GET_CBF(MbQ))
	{
		return 0;
	}

	if (p_pic0 == q_pic0 && p_pic1 == q_pic1) {
		for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 2
			if (REFI_IS_VALID(refiP[reflist])) {
				if ((abs(mvP[reflist][MV_X] - mvQ[reflist][MV_X]) >= 4) ||
					(abs(mvP[reflist][MV_Y] - mvQ[reflist][MV_Y]) >= 4)) {
					return 0;
				}
			}
		}
		return 1;
	}

	if (p_pic0 == q_pic1 && p_pic1 == q_pic0) {
		for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max = 2
			if (REFI_IS_VALID(refiP[reflist])) {
				if ((abs(mvP[reflist][MV_X] - mvQ[!reflist][MV_X]) >= 4) ||
					(abs(mvP[reflist][MV_Y] - mvQ[!reflist][MV_Y]) >= 4)) {
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}

#else
U1 SkipFiltering_ver0(U32 MbP, U32 MbQ, S8 refiP, S8 refiQ, S16 mvP[2], S16 mvQ[2])
{
#pragma HLS inline off

	BOOL p_pic0 = REFI_IS_VALID(refiP);
	//BOOL p_pic1 = REFI_IS_VALID(refiP[REFP_1]);
	BOOL q_pic0 = REFI_IS_VALID(refiQ);
	//BOOL q_pic1 = REFI_IS_VALID(refiQ[REFP_1]);

	if ((p_pic0 == 0) || (q_pic0 == 0))
	{
		return 0;
	}
	if (MCU_GET_CBF(MbP) || MCU_GET_CBF(MbQ))
	{
		return 0;
	}

	//if (p_pic0 == q_pic0 && p_pic1 == q_pic1)
	if (p_pic0 == q_pic0)//&& p_pic1 == q_pic1)
	{
		//printf("p_pic0=%d\n", p_pic0);
		//printf("q_pic0=%d\n", q_pic0);
		//if (REFI_IS_VALID(refiP[0]))
		//{
		if ((abs(mvP[0] - mvQ[0]) >= 4) ||
			(abs(mvP[1] - mvQ[1]) >= 4))
		{
			return 0;
		}
		//}
		return 1;
	}
	//if (p_pic0 == q_pic1 && p_pic1 == q_pic0)
//    if (p_pic0 == q_pic1 && p_pic1 == q_pic0)
//    {
//        for (reflist = REFP_0; reflist < 1; reflist++)
//        {
//#pragma HLS UNROLL
//#pragma HLS loop_tripcount max = 2
//            if (REFI_IS_VALID(refiP[reflist]))
//            {
//                if ((abs(mvP[reflist][MV_X] - mvQ[!reflist][MV_X]) >= 4) ||
//                    (abs(mvP[reflist][MV_Y] - mvQ[!reflist][MV_Y]) >= 4))
//                {
//                    return 0;
//                }
//            }
//        }
//        return 1;
//    }
	return 0;
}


U1 SkipFiltering_hor1(U32 MbP, U32 MbQ, S8 refiP, S8 refiQ, S16 mvP[2], S16 mvQ[2])
{
#pragma HLS inline off

	BOOL p_pic0 = REFI_IS_VALID(refiP);
	BOOL q_pic0 = REFI_IS_VALID(refiQ);

	if ((p_pic0 == 0) || (q_pic0 == 0))
	{
		return 0;
	}
	if (MCU_GET_CBF(MbP) || MCU_GET_CBF(MbQ))
	{
		return 0;
	}
	if (p_pic0 == q_pic0)
	{
		if ((abs(mvP[0] - mvQ[0]) >= 4) ||
			(abs(mvP[1] - mvQ[1]) >= 4))
		{
			return 0;
		}
		return 1;
	}
	return 0;
}
#endif

static U2 EdgeContion_prepare_ver0(U11 mb_y, U11 mb_x, U11 mbxbegin, U11 mbybegin,
	U2 ppbEdgeFilter_ctu_ver0[16][16], U2 ppbEdgeFilter_ctu_buf1_ver0[16], U2 ppbEdgeFilter_ctu_buf2_ver0[960])
{
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_buf2_ver0 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_buf1_ver0 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_ver0 complete dim=0

#pragma HLS inline off
	U2 EdgeCondition;

	EdgeCondition = 1 && mb_x;

	U11 b4_x_start_ctu = mb_x - mbxbegin;// [lcu_x >> 2 - 1, (lcu_x + lcuw) >> 2 -1] = [0,15] //U4
	U11 b4_y_start_ctu = mb_y - mbybegin;// [lcu_y >> 2 - 1, (lcu_y + lcuh) >> 2 -1] = [0,15] //U4
	U2 ppbEdgeFilter_ctu_t;
	if (mbxbegin == 0 && mb_x < 15 && mbybegin == 0 && mb_y < 15) {
		ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_ver0[b4_y_start_ctu][b4_x_start_ctu];
	}
	else
		//the first column CTUs
	if (mbxbegin == 0 && mb_x < 15) {
		//change ctu to frame now
		if (mb_y == mbybegin) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf2_ver0[mb_x];
		}
		else {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_ver0[b4_y_start_ctu - 1][b4_x_start_ctu];
		}
	}
	else
		// the first line CTUS
	if (mbybegin == 0 && mb_y < 15) {
		if (mb_x == mbxbegin || b4_x_start_ctu == 0) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf1_ver0[b4_y_start_ctu];
		}
		else {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_ver0[b4_y_start_ctu][b4_x_start_ctu - 1];
		}
	}
	else
		//the other CTUs
	{
		if (b4_x_start_ctu == 0 && b4_y_start_ctu == 0) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf2_ver0[mb_x];
		}
		else if (mb_x == mbxbegin) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf1_ver0[b4_y_start_ctu - 1];
		}
		else
			//change ctu to frame now
		if (mb_y == mbybegin) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf2_ver0[mb_x];
		}
		else {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_ver0[b4_y_start_ctu - 1][b4_x_start_ctu - 1];
		}
	}
	EdgeCondition = (ppbEdgeFilter_ctu_t && EdgeCondition ? ppbEdgeFilter_ctu_t : (U2)0);
	return EdgeCondition;
}

static U2 EdgeContion_prepare_hor1(U11 mb_y, U11 mb_x, U11 mbxbegin, U11 mbybegin,
	U2 ppbEdgeFilter_ctu_hor1[16][16], U2 ppbEdgeFilter_ctu_buf1_hor1[16], U2 ppbEdgeFilter_ctu_buf2_hor1[960])
{
#pragma HLS inline off
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_hor1 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_buf1_hor1 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_buf2_hor1 complete dim=0
	U2 EdgeCondition;

	EdgeCondition = 1 && mb_y;

	U11 b4_x_start_ctu = mb_x - mbxbegin;
	U11 b4_y_start_ctu = mb_y - mbybegin;
	U2 ppbEdgeFilter_ctu_t;
	if (mbxbegin == 0 && mb_x < 15 && mbybegin == 0 && mb_y < 15) {
		ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_hor1[b4_y_start_ctu][b4_x_start_ctu];
	}
	else
		//the first column CTUs
	if (mbxbegin == 0 && mb_x < 15) {
		//change ctu to frame now
		if (mb_y == mbybegin) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf2_hor1[mb_x];
		}
		else {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_hor1[b4_y_start_ctu - 1][b4_x_start_ctu];
		}
	}
	else
		// the first line CTUS
	if (mbybegin == 0 && mb_y < 15) {
		if (mb_x == mbxbegin || b4_x_start_ctu == 0) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf1_hor1[b4_y_start_ctu];
		}
		else {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_hor1[b4_y_start_ctu][b4_x_start_ctu - 1];
		}
	}
	else
		//the other CTUs
	{
		if (b4_x_start_ctu == 0 && b4_y_start_ctu == 0) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf2_hor1[mb_x];
		}
		else if (mb_x == mbxbegin) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf1_hor1[b4_y_start_ctu - 1];
		}
		else
			//change ctu to frame now
		if (mb_y == mbybegin) {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_buf2_hor1[mb_x];
		}
		else {
			ppbEdgeFilter_ctu_t = ppbEdgeFilter_ctu_hor1[b4_y_start_ctu - 1][b4_x_start_ctu - 1];
		}
	}
	EdgeCondition = (ppbEdgeFilter_ctu_t && EdgeCondition ? ppbEdgeFilter_ctu_t : (U2)0);
	return EdgeCondition;
}
/*
void DeblockMb_avs2_hor1(U3 chroma_quant_param_delta_cb, U3 chroma_quant_param_delta_cr,
    U7 filter_mb_y, U7 filter_mb_x,U6 filter_mb_uy,U6 filter_mb_ux,
    U11 mb_y, U11 mb_x, U1 dir, U11 mbxbegin, U11 mbybegin,
    U32 map_scu_above_line[48],
    U32 map_scu_inner[2][16][16],
	U11 min_scu_x, U11 min_scu_y, U1 curr_ctu_idx, U1 above_line_idx,
    ENC_ME_NEB_INFO inner_nebs2dbk[2][16][16],
    ENC_ME_NEB_INFO above_nebs_line[48], U13 pic_width, U13 pic_height,
    pel ybuffer[8][8],pel ubuffer[6][6], pel vbuffer[6][6])
{
#pragma HLS inline off
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_hor1 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_buf1_hor1 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_buf2_hor1 complete dim=0
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_y complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_u complete dim=2
	//#pragma HLS ARRAY_PARTITION variable=dbkbuffer_v complete dim=2

	//int EdgeCondition = EdgeContion_prepare_hor1(mb_y, mb_x, dir, mbxbegin, mbybegin, ppbEdgeFilter_ctu_hor1,
	//	ppbEdgeFilter_ctu_buf1_hor1, ppbEdgeFilter_ctu_buf2_hor1);
	//// then  4 horicontal
	//if (EdgeCondition) 
	{
		S5 scu_x_in_line = mb_x - min_scu_x;
		S5 scu_y_in_line = mb_y - min_scu_y;

		U32 MbQ = MbQ_prepare_1(mb_x, curr_ctu_idx, min_scu_x, scu_x_in_line, scu_y_in_line, map_scu_above_line,
            map_scu_inner);
		U32 MbP = MbP_prepare_hor(mb_x, dir, scu_x_in_line, scu_y_in_line, curr_ctu_idx, min_scu_x, map_scu_above_line,
            map_scu_inner);
		U8 MbQ_qp = (U8)(MbQ >> 16 & (0x7F));
		U8 MbP_qp = (U8)(MbP >> 16 & (0x7F));
        U1 skipFilteringFlag = 0;
        S8 refiP, refiQ;
        S16 mvP[2], mvQ[2];
        {
            if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line == 0)//right-up
            {

                refiP = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
				refiQ = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[0];

                mvP[0] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
				mvQ[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][0];
                mvP[1] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
				mvQ[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][1];
            }
            else if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line > 0 && scu_y_in_line <= 15)//right-down
            {
                refiP= inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
                refiQ= inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].refi[0];

                mvP[0] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
                mvQ[0] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].mv[0][0];
                mvP[1] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
                mvQ[1] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].mv[0][1];

            }
            else if (scu_x_in_line < 0 && scu_y_in_line == 0)//left-up
            {
                refiP = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].refi[0];
				refiQ = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[0];

                mvP[0] = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][0];
				mvQ[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][0];
                mvP[1] = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][1];
				mvQ[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][1];

            }
            else //left-down
            {
                refiP = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].refi[0];
                refiQ = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].refi[0];

                mvP[0] = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][0];
                mvQ[0] = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].mv[0][0];
                mvP[1] = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][1];
                mvQ[1] = inner_nebs2dbk[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].mv[0][1];

            }
        }
        skipFilteringFlag = SkipFiltering_hor1(MbP, MbQ, refiP, refiQ, mvP, mvQ);
		U8 QPs = (MbP_qp + MbQ_qp + 1) >> 1;
		U8 cQPus = cQPus_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cb, qp_offset_bit_depth);
		U8 cQPvs = cQPvs_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cr, qp_offset_bit_depth);
        EdgeLoopX_Y(ybuffer, QPs, dir, skipFilteringFlag, filter_mb_y, filter_mb_x, pic_width, pic_height, bit_depth);
        if (((mb_y << 2) & 15) == 0) {
            EdgeLoopX_U(ubuffer, cQPus, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, bit_depth);
            EdgeLoopX_V(vbuffer, cQPvs, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, bit_depth);
        }
    }
}
*/
void skip_Mb_hor(U3 chroma_quant_param_delta_cb, U3 chroma_quant_param_delta_cr,
    U7 filter_mb_y, U7 filter_mb_x,U6 filter_mb_uy,U6 filter_mb_ux,
    U11 mb_y, U11 mb_x, U1 dir, U11 mbxbegin, U11 mbybegin,
    U32 map_scu_above_line[48],
    U32 map_scu_inner[2][16][16],
	U11 min_scu_x, U11 min_scu_y, U1 curr_ctu_idx, U1 above_line_idx,
    ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16],
    ENC_ME_NEB_INFO above_nebs_line[48], U13 pic_width, U13 pic_height,
    U1 &skipFilteringFlag, U8 &QPs, U8 &cQPus, U8 &cQPvs, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth
#if ENABLE_BFRAME
	//, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM]
  ,int ref_ptr[MAX_NUM_REF_PICS][REFP_NUM]
#endif
)
{
#pragma HLS inline off
	{
		S5 scu_x_in_line = mb_x - min_scu_x;
		S5 scu_y_in_line = mb_y - min_scu_y;

		U32 MbQ = MbQ_prepare_1(mb_x, curr_ctu_idx, min_scu_x, scu_x_in_line, scu_y_in_line, map_scu_above_line,
            map_scu_inner);
		U32 MbP = MbP_prepare_hor(mb_x, dir, scu_x_in_line, scu_y_in_line, curr_ctu_idx, min_scu_x, map_scu_above_line,
            map_scu_inner);
		U8 MbQ_qp = (U8)(MbQ >> 16 & (0x7F));
		U8 MbP_qp = (U8)(MbP >> 16 & (0x7F));
#if ENABLE_BFRAME
		S8 refiP[REFP_NUM], refiQ[REFP_NUM];
		S16 mvP[REFP_NUM][2], mvQ[REFP_NUM][2];
		int reflist;
		{
			if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line == 0)//right-up
			{
				for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
					refiP[reflist] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[reflist];
					refiQ[reflist] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[reflist];

					mvP[reflist][0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][0];
					mvQ[reflist][0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[reflist][0];
					mvP[reflist][1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][1];
					mvQ[reflist][1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[reflist][1];
				}
			}
			else if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line > 0 && scu_y_in_line <= 15)//right-down
			{
				for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
					refiP[reflist] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[reflist];
					refiQ[reflist] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].refi[reflist];

					mvP[reflist][0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][0];
					mvQ[reflist][0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].mv[reflist][0];
					mvP[reflist][1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][1];
					mvQ[reflist][1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].mv[reflist][1];
				}
			}
			else if (scu_x_in_line < 0 && scu_y_in_line == 0)//left-up
			{
				for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
					refiP[reflist] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].refi[reflist];
					refiQ[reflist] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[reflist];

					mvP[reflist][0] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[reflist][0];
					mvQ[reflist][0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[reflist][0];
					mvP[reflist][1] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[reflist][1];
					mvQ[reflist][1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[reflist][1];
				}

			}
			else //left-down
			{
				for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
					refiP[reflist] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].refi[reflist];
					refiQ[reflist] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].refi[reflist];

					mvP[reflist][0] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[reflist][0];
					mvQ[reflist][0] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].mv[reflist][0];
					mvP[reflist][1] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[reflist][1];
					mvQ[reflist][1] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].mv[reflist][1];
	}
}
		}
		skipFilteringFlag = SkipFiltering_hor1(MbP, MbQ, refiP, refiQ, mvP, mvQ, ref_ptr);
#else
		S8 refiP, refiQ;
		S16 mvP[2], mvQ[2];
		{
			if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line == 0)//right-up
			{
				refiP = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
				refiQ = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[0];

				mvP[0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
				mvQ[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][0];
				mvP[1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
				mvQ[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][1];
			}
			else if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line > 0 && scu_y_in_line <= 15)//right-down
			{
				refiP = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
				refiQ = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].refi[0];

				mvP[0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
				mvQ[0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].mv[0][0];
				mvP[1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
				mvQ[1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line - 1].mv[0][1];

			}
			else if (scu_x_in_line < 0 && scu_y_in_line == 0)//left-up
			{
				refiP = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].refi[0];
				refiQ = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[0];

				mvP[0] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][0];
				mvQ[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][0];
				mvP[1] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][1];
				mvQ[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][1];

			}
			else //left-down
			{
				refiP = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].refi[0];
				refiQ = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].refi[0];

				mvP[0] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][0];
				mvQ[0] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].mv[0][0];
				mvP[1] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line].mv[0][1];
				mvQ[1] = inner_nebs_md_chroma[1 - curr_ctu_idx][16 + scu_x_in_line][scu_y_in_line - 1].mv[0][1];

			}
		}
		skipFilteringFlag = SkipFiltering_hor1(MbP, MbQ, refiP, refiQ, mvP, mvQ);
#endif

		QPs = (MbP_qp + MbQ_qp + 1) >> 1;
		cQPus = cQPus_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_qp_offset_bit_depth);
		cQPvs = cQPvs_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cr, fetch2dbk_fw_ptr_qp_offset_bit_depth);
    }
}

void Filter_YUV_hor(U7 filter_mb_y, U7 filter_mb_x,U6 filter_mb_uy,U6 filter_mb_ux,
    U11 mb_y, U11 mb_x, U1 dir, U1 skipFilteringFlag,
	U8 QPs, U8 cQPus, U8 cQPvs,
    U13 pic_width, U13 pic_height,
    pel ybuffer[8][8],pel ubuffer[6][6], pel vbuffer[6][6], U4 fetch2dbk_fw_ptr_bit_depth, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth)
{
#pragma HLS inline off
	{
        EdgeLoopX_Y(ybuffer, QPs - fetch2dbk_fw_ptr_qp_offset_bit_depth, dir, skipFilteringFlag, filter_mb_y, filter_mb_x, pic_width, pic_height, fetch2dbk_fw_ptr_bit_depth);
        if (((mb_y << 2) & 15) == 0) {
            EdgeLoopX_U(ubuffer, cQPus - fetch2dbk_fw_ptr_qp_offset_bit_depth, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, fetch2dbk_fw_ptr_bit_depth);
            EdgeLoopX_V(vbuffer, cQPvs - fetch2dbk_fw_ptr_qp_offset_bit_depth, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, fetch2dbk_fw_ptr_bit_depth);
        }
    }
}
/*
void DeblockMb_avs2_ver0(U3 chroma_quant_param_delta_cb, U3 chroma_quant_param_delta_cr,
    U7 filter_mb_y, U7 filter_mb_x, U6 filter_mb_uy, U6 filter_mb_ux,
    U11 mb_y, U11 mb_x, U1 dir, U11 mbxbegin, U11 mbybegin,
    U32 map_scu_above_line[48],
    U32 map_scu_inner[2][16][16],
	U11 min_scu_x, U11 min_scu_y, U1 curr_ctu_idx, U1 above_line_idx,
    ENC_ME_NEB_INFO inner_nebs2dbk[2][16][16],
    ENC_ME_NEB_INFO above_nebs_line[48], U13 pic_width, U13 pic_height,
    pel ybuffer[8][8], pel ubuffer[6][6], pel vbuffer[6][6])
{
#pragma HLS inline off
{
		S5 scu_x_in_line = mb_x - min_scu_x;
		S5 scu_y_in_line = mb_y - min_scu_y;
		U32 MbQ = MbQ_prepare_1(mb_x, curr_ctu_idx, min_scu_x, scu_x_in_line, scu_y_in_line, map_scu_above_line,
            map_scu_inner);
		U32 MbP = MbP_prepare_ver(mb_x, scu_x_in_line, scu_y_in_line, curr_ctu_idx, min_scu_x, map_scu_above_line,
            map_scu_inner);
        U8 MbQ_qp = (U8)(MbQ >> 16 & (0x7F));
        U8 MbP_qp = (U8)(MbP >> 16 & (0x7F));
        S8 refiP, refiQ;
        S16 mvP[2], mvQ[2];
        if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line < 0)//up
        {
			refiP = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[0];
			refiQ = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].refi[0];

			mvP[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][0];
			mvQ[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].mv[0][0];
			mvP[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][1];
			mvQ[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].mv[0][1];
        }
        else if (scu_x_in_line > 0 && scu_x_in_line <= 15 && scu_y_in_line >= 0 && scu_y_in_line <= 15)//right-down
        {
            refiP = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
            refiQ = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].refi[0];

            mvP[0] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
            mvQ[0] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].mv[0][0];
            mvP[1] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
            mvQ[1] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].mv[0][1];
        }
        else //left-down
        {
            refiP = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
            refiQ = inner_nebs2dbk[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].refi[0];

            mvP[0] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
            mvQ[0] = inner_nebs2dbk[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].mv[0][0];
            mvP[1] = inner_nebs2dbk[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
            mvQ[1] = inner_nebs2dbk[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].mv[0][1];
        }
        U1 skipFilteringFlag = 0;
        skipFilteringFlag = SkipFiltering_ver0(MbP, MbQ, refiP, refiQ, mvP, mvQ);
        U8 QPs = (MbP_qp + MbQ_qp + 1) >> 1;
        U8 cQPus = cQPus_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cb, qp_offset_bit_depth);
		U8 cQPvs = cQPvs_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cr, qp_offset_bit_depth);
        EdgeLoopX_Y(ybuffer, QPs, dir, skipFilteringFlag, filter_mb_y, filter_mb_x, pic_width, pic_height, bit_depth);
        if (((mb_x << 2) & 15) == 0) {
            EdgeLoopX_U(ubuffer, cQPus, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, bit_depth);
            EdgeLoopX_V(vbuffer, cQPvs, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, bit_depth);
        }
    }
}
*/
void skip_Mb(U3 chroma_quant_param_delta_cb, U3 chroma_quant_param_delta_cr,
    U7 filter_mb_y, U7 filter_mb_x, U6 filter_mb_uy, U6 filter_mb_ux,
    U11 mb_y, U11 mb_x, U1 dir, U11 mbxbegin, U11 mbybegin,
    U32 map_scu_above_line[48],
    U32 map_scu_inner[2][16][16],
	U11 min_scu_x, U11 min_scu_y, U1 curr_ctu_idx, U1 above_line_idx,
    ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16],
    ENC_ME_NEB_INFO above_nebs_line[48], U13 pic_width, U13 pic_height,
	U1 &skipFilteringFlag, U8 &QPs, U8 &cQPus, U8 &cQPvs, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth
#if ENABLE_BFRAME
	//, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM]
  , int ref_ptr[MAX_NUM_REF_PICS][REFP_NUM]
#endif
	)
{
#pragma HLS inline off
{
		S5 scu_x_in_line = mb_x - min_scu_x;
		S5 scu_y_in_line = mb_y - min_scu_y;
		U1 temp_curr_ctu = curr_ctu_idx;
		U32 MbQ = MbQ_prepare_1(mb_x, temp_curr_ctu, min_scu_x, scu_x_in_line, scu_y_in_line, map_scu_above_line,
            map_scu_inner);
		U32 MbP = MbP_prepare_ver(mb_x, scu_x_in_line, scu_y_in_line, temp_curr_ctu, min_scu_x, map_scu_above_line,
            map_scu_inner);
        U8 MbQ_qp = (U8)(MbQ >> 16 & (0x7F));
        U8 MbP_qp = (U8)(MbP >> 16 & (0x7F));
#if ENABLE_BFRAME
		S8 refiP[REFP_NUM], refiQ[REFP_NUM];
		S16 mvP[REFP_NUM][2], mvQ[REFP_NUM][2];
		int reflist;
		if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line < 0)//up
		{
			for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
				refiP[reflist] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[reflist];
				refiQ[reflist] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].refi[reflist];

				mvP[reflist][0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[reflist][0];
				mvQ[reflist][0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].mv[reflist][0];
				mvP[reflist][1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[reflist][1];
				mvQ[reflist][1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].mv[reflist][1];
			}
		}
		else if (scu_x_in_line > 0 && scu_x_in_line <= 15 && scu_y_in_line >= 0 && scu_y_in_line <= 15)//right-down
		{
			for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
				refiP[reflist] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[reflist];
				refiQ[reflist] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].refi[reflist];

				mvP[reflist][0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][0];
				mvQ[reflist][0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].mv[reflist][0];
				mvP[reflist][1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][1];
				mvQ[reflist][1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].mv[reflist][1];
			}
		}
		else //left-down
		{
			for (reflist = REFP_0; reflist < REFP_NUM; reflist++) {
				refiP[reflist] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[reflist];
				refiQ[reflist] = inner_nebs_md_chroma[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].refi[reflist];

				mvP[reflist][0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][0];
				mvQ[reflist][0] = inner_nebs_md_chroma[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].mv[reflist][0];
				mvP[reflist][1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[reflist][1];
				mvQ[reflist][1] = inner_nebs_md_chroma[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].mv[reflist][1];
			}
		}
		skipFilteringFlag = 0;
		skipFilteringFlag = SkipFiltering_ver0(MbP, MbQ, refiP, refiQ, mvP, mvQ, ref_ptr);
#else
		S8 refiP, refiQ;
		S16 mvP[2], mvQ[2];
		if (scu_x_in_line >= 0 && scu_x_in_line <= 15 && scu_y_in_line < 0)//up
		{
			refiP = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].refi[0];
			refiQ = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].refi[0];

			mvP[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][0];
			mvQ[0] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].mv[0][0];
			mvP[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET].mv[0][1];
			mvQ[1] = above_nebs_line[mb_x - min_scu_x + SCUX_OFFSET - 1].mv[0][1];
		}
		else if (scu_x_in_line > 0 && scu_x_in_line <= 15 && scu_y_in_line >= 0 && scu_y_in_line <= 15)//right-down
		{
			refiP = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
			refiQ = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].refi[0];

			mvP[0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
			mvQ[0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].mv[0][0];
			mvP[1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
			mvQ[1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line - 1][scu_y_in_line].mv[0][1];
		}
		else //left-down
		{
			refiP = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].refi[0];
			refiQ = inner_nebs_md_chroma[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].refi[0];

			mvP[0] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][0];
			mvQ[0] = inner_nebs_md_chroma[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].mv[0][0];
			mvP[1] = inner_nebs_md_chroma[curr_ctu_idx][scu_x_in_line][scu_y_in_line].mv[0][1];
			mvQ[1] = inner_nebs_md_chroma[1 - curr_ctu_idx][15 + scu_x_in_line][scu_y_in_line].mv[0][1];
		}
		skipFilteringFlag = 0;
		skipFilteringFlag = SkipFiltering_ver0(MbP, MbQ, refiP, refiQ, mvP, mvQ);
#endif
        
        QPs = (MbP_qp + MbQ_qp + 1) >> 1;
        cQPus = cQPus_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_qp_offset_bit_depth);
		cQPvs = cQPvs_prepare_1(MbP_qp, MbQ_qp, chroma_quant_param_delta_cr, fetch2dbk_fw_ptr_qp_offset_bit_depth);
    }
}

void Filter_YUV(U7 filter_mb_y, U7 filter_mb_x, U6 filter_mb_uy, U6 filter_mb_ux,
    U11 mb_y, U11 mb_x, U1 dir, U1 skipFilteringFlag,
	U8 QPs, U8 cQPus, U8 cQPvs,
	U13 pic_width, U13 pic_height,
	pel ybuffer[8][8], pel ubuffer[6][6], pel vbuffer[6][6] ,U4 fetch2dbk_fw_ptr_bit_depth ,U32 fetch2dbk_fw_ptr_qp_offset_bit_depth)
{
#pragma HLS inline off
{
    EdgeLoopX_Y(ybuffer, QPs - fetch2dbk_fw_ptr_qp_offset_bit_depth, dir, skipFilteringFlag, filter_mb_y, filter_mb_x, pic_width, pic_height, fetch2dbk_fw_ptr_bit_depth);
    if (((mb_x << 2) & 15) == 0) {
        EdgeLoopX_U(ubuffer, cQPus - fetch2dbk_fw_ptr_qp_offset_bit_depth, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, fetch2dbk_fw_ptr_bit_depth);
        EdgeLoopX_V(vbuffer, cQPvs - fetch2dbk_fw_ptr_qp_offset_bit_depth, dir, skipFilteringFlag, filter_mb_uy, filter_mb_ux, pic_width, pic_height, fetch2dbk_fw_ptr_bit_depth);
    }
}
}
/*
void Buffer_ver_prepare(U7 &filter_mb_y, U7 &filter_mb_x, U6 &filter_mb_uy, U6&filter_mb_ux, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height, pel ybuffer[8][8], pel ubuffer[6][6], pel vbuffer[6][6], pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39], pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39]){
    //pel ybuffer[8][8];// 
    U6 mb_y_mod16 = mb_y & 15;//% 16;
    U6 mb_x_mod16 = mb_x & 15; //% 16;
    filter_mb_y = (4 + (mb_y_mod16 << 2)) % 64;
    filter_mb_x = (4 + (mb_x_mod16 << 2)) % 64;
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_x = 4 + (mb_x_mod16 << 2);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_y = 4 + (mb_y_mod16 << 2);
    }
    for (int j = 4; j < 8; ++j) {
#pragma HLS PIPELINE

        for (int i = 0; i < 8; ++i) {
#pragma HLS UNROLL
            ybuffer[j][i] = dbk_ver_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i];
        }
    }
    //pel ubuffer[6][6];// 
    //pel vbuffer[6][6];// 
	U6 block_y_mod16 = mb_y & 15;//% 16;
	U6 block_x_mod16 = mb_x & 15;// % 16;
    filter_mb_uy = (4 + (block_y_mod16 << 1)) % 36;
    filter_mb_ux = (4 + (block_x_mod16 << 1)) % 36;
    if (block_y_mod16 == 15) {
        filter_mb_uy = 2;
    }
    if (block_x_mod16 == 15) {
        filter_mb_ux = 2;
    }
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_ux = 4 + (block_x_mod16 << 1);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_uy = 4 + (block_y_mod16 << 1);
    }
    for (int j = 3; j < 6; ++j) {
#pragma HLS PIPELINE
        for (int i = 0; i < 6; ++i) {
            ubuffer[j][i] = dbk_ver_u[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
        }
    }
    for (int j = 3; j < 6; ++j) {
#pragma HLS PIPELINE
        for (int i = 0; i < 6; ++i) {
#pragma HLS UNROLL
            vbuffer[j][i] = dbk_ver_v[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
        }
    }
}
*/
void Buffer_ver_filter_mb(U7 &filter_mb_y, U7 &filter_mb_x, U6 &filter_mb_uy, U6&filter_mb_ux, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height){
	U6 mb_y_mod16 = mb_y & 15;//% 16;
    U6 mb_x_mod16 = mb_x & 15; //% 16;
    filter_mb_y = (4 + (mb_y_mod16 << 2)) % 64;
    filter_mb_x = (4 + (mb_x_mod16 << 2)) % 64;
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_x = 4 + (mb_x_mod16 << 2);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_y = 4 + (mb_y_mod16 << 2);
    }

	U6 block_y_mod16 = mb_y & 15;//% 16;
	U6 block_x_mod16 = mb_x & 15;// % 16;
    filter_mb_uy = (4 + (block_y_mod16 << 1)) % 36;
    filter_mb_ux = (4 + (block_x_mod16 << 1)) % 36;
    if (block_y_mod16 == 15) {
        filter_mb_uy = 2;
    }
    if (block_x_mod16 == 15) {
        filter_mb_ux = 2;
    }
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_ux = 4 + (block_x_mod16 << 1);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_uy = 4 + (block_y_mod16 << 1);
    }

}

void Buffer_ver_prepare_y(U7 filter_mb_y, U7 filter_mb_x, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height, pel ybuffer[8][8], pel dbk_ver_y[71][71], pel dbkout_y[71][71], pel dbk_mid_y[71][71])
{
/*     U6 mb_y_mod16 = mb_y & 15;//% 16;
    U6 mb_x_mod16 = mb_x & 15; //% 16;
    filter_mb_y = (4 + (mb_y_mod16 << 2)) % 64;
    filter_mb_x = (4 + (mb_x_mod16 << 2)) % 64;
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_x = 4 + (mb_x_mod16 << 2);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_y = 4 + (mb_y_mod16 << 2);
    } */
    for (int j = 4; j < 8; ++j) {
#pragma HLS PIPELINE 
        for (int i = 0; i < 8; ++i) {
			ybuffer[j][i] = dbk_ver_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i];
        }
    }
}

void Buffer_ver_prepare_uv(U6 filter_mb_uy, U6 filter_mb_ux, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height, pel ubuffer[6][6], pel vbuffer[6][6], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
/* 	U6 block_y_mod16 = mb_y & 15;//% 16;
	U6 block_x_mod16 = mb_x & 15;// % 16;
    filter_mb_uy = (4 + (block_y_mod16 << 1)) % 36;
    filter_mb_ux = (4 + (block_x_mod16 << 1)) % 36;
    if (block_y_mod16 == 15) {
        filter_mb_uy = 2;
    }
    if (block_x_mod16 == 15) {
        filter_mb_ux = 2;
    }
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_ux = 4 + (block_x_mod16 << 1);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_uy = 4 + (block_y_mod16 << 1);
    } */
    for (int j = 3; j < 6; ++j) {
#pragma HLS PIPELINE
        for (int i = 0; i < 6; ++i) {
            ubuffer[j][i] = dbk_ver_u[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
            vbuffer[j][i] = dbk_ver_v[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
        }
    }
}

void copy_buffer_y_ver(int filter_mb_y, int filter_mb_x, pel ybuffer[8][8], pel dbk_ver_y[71][71], pel dbkout_y[71][71], pel dbk_mid_y[71][71]){
	if (filter_mb_x <= 64 && filter_mb_x >= 0 && filter_mb_y <= 64 && filter_mb_y >= 0)
	{
		for (int j = 4; j < 8; ++j) {
#pragma HLS PIPELINE
			for (int i = 0; i < 8; ++i) {
				dbkout_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i] = ybuffer[j][i];
				dbk_mid_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i] = ybuffer[j][i];
				dbk_ver_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i] = ybuffer[j][i];
			}
		}
	}
}

void copy_buffer_uv_ver(int filter_mb_uy, int filter_mb_ux, U11 mb_y, U11 mb_x, pel dbk_ver_uv[39][39], pel uvbuffer[6][6], pel dbkout_uv[39][39], pel dbk_mid_uv[39][39]){
	if (filter_mb_ux <= 36 && filter_mb_ux >= 0 && filter_mb_uy <= 36 && filter_mb_uy >= 0)
	{
		for (int j = 3; j < 5; ++j) {
#pragma HLS PIPELINE
			for (int i = 1; i < 5; ++i) {
				dbkout_uv[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i] = uvbuffer[j][i];
				dbk_mid_uv[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i] = uvbuffer[j][i];
				dbk_ver_uv[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i] = uvbuffer[j][i];
			}
		}

	}
}

void Buffer_ver(U7 filter_mb_y, U7 filter_mb_x,U6 filter_mb_uy,U6 filter_mb_ux, U11 mb_y, U11 mb_x, pel ybuffer[8][8], pel ubuffer[6][6],pel vbuffer[6][6], 
	pel dbk_ver_y[71][71], pel dbk_ver_u[39][39], pel dbk_ver_v[39][39], 
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
	copy_buffer_y_ver((int)filter_mb_y,(int)filter_mb_x, ybuffer, dbk_ver_y, dbkout_y, dbk_mid_y);
	copy_buffer_uv_ver((int)filter_mb_uy, (int)filter_mb_ux, mb_y, mb_x, dbk_ver_u, ubuffer, dbkout_u, dbk_mid_u);
	copy_buffer_uv_ver((int)filter_mb_uy, (int)filter_mb_ux, mb_y, mb_x, dbk_ver_v, vbuffer, dbkout_v, dbk_mid_v);
}
/*
void Buffer_hor_prepare(U7& filter_mb_y, U7& filter_mb_x, U6& filter_mb_uy, U6& filter_mb_ux, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height, pel ybuffer[8][8], pel ubuffer[6][6], pel vbuffer[6][6],
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
	U6 mb_y_mod16 = mb_y & 15;//% 16;
	U6 mb_x_mod16 = mb_x & 15; //% 16;
    filter_mb_y = (4 + (mb_y_mod16 << 2)) % 64;
    filter_mb_x = (4 + (mb_x_mod16 << 2)) % 64;
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_x = 4 + (mb_x_mod16 << 2);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_y = 4 + (mb_y_mod16 << 2);
    }
    for (int j = 0; j < 8; ++j) {
#pragma HLS PIPELINE
		for (int i = 4; i < 8; ++i) {
#pragma HLS UNROLL
			//ybuffer[j][i] = dbk_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i];
			ybuffer[j][i] = dbk_mid_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i];
		}
    }
    //pel ubuffer[6][6];// 
    //pel vbuffer[6][6];// 
	U6 block_y_mod16 = mb_y & 15;//% 16;
	U6 block_x_mod16 = mb_x & 15;// % 16;
    filter_mb_uy = (4 + (block_y_mod16 << 1)) % 36;
    filter_mb_ux = (4 + (block_x_mod16 << 1)) % 36;
    if (block_y_mod16 == 15) {
        filter_mb_uy = 2;
    }
    if (block_x_mod16 == 15) {
        filter_mb_ux = 2;
    }
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_ux = 4 + (block_x_mod16 << 1);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_uy = 4 + (block_y_mod16 << 1);
    }
    for (int j = 0; j < 6; ++j) {
#pragma HLS PIPELINE
        for (int i = 3; i < 6; ++i) {
#pragma HLS UNROLL
            //ubuffer[j][i] = dbk_u[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
			ubuffer[j][i] = dbk_mid_u[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
        }
    }
    for (int j = 0; j < 6; ++j) {
#pragma HLS PIPELINE
        for (int i = 3; i < 6; ++i) {
#pragma HLS UNROLL
            //vbuffer[j][i] = dbk_v[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
			vbuffer[j][i] = dbk_mid_v[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
        }
    }
}
*/
void Buffer_hor_prepare_y(U7& filter_mb_y, U7& filter_mb_x, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height, pel ybuffer[8][8],
	pel dbk_y[71][71], pel dbkout_y[71][71], pel dbk_mid_y[71][71])
{
	U6 mb_y_mod16 = mb_y & 15;//% 16;
	U6 mb_x_mod16 = mb_x & 15; //% 16;
    filter_mb_y = (4 + (mb_y_mod16 << 2)) % 64;
    filter_mb_x = (4 + (mb_x_mod16 << 2)) % 64;
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_x = 4 + (mb_x_mod16 << 2);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_y = 4 + (mb_y_mod16 << 2);
    }
    //U7 cow_hor_y = filter_mb_y - 2;
    //U7 col_hor_y = filter_mb_x - 2;
    for (int j = 0; j < 8; ++j) {
#pragma HLS PIPELINE
		for (int i = 4; i < 8; ++i) {
			ybuffer[j][i] = dbk_mid_y[filter_mb_y - 2 + j][filter_mb_x - 2 + i];
		}
    }
}

void Buffer_hor_prepare_uv(U6& filter_mb_uy, U6& filter_mb_ux, U11 mb_y, U11 mb_x, U13 pic_width, U13 pic_height, pel ubuffer[6][6], pel vbuffer[6][6],
	pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
	U6 block_y_mod16 = mb_y & 15;//% 16;
	U6 block_x_mod16 = mb_x & 15;// % 16;
    filter_mb_uy = (4 + (block_y_mod16 << 1)) % 36;
    filter_mb_ux = (4 + (block_x_mod16 << 1)) % 36;
    if (block_y_mod16 == 15) {
        filter_mb_uy = 2;
    }
    if (block_x_mod16 == 15) {
        filter_mb_ux = 2;
    }
    if (mb_x == (pic_width >> 2) - 1) {
        filter_mb_ux = 4 + (block_x_mod16 << 1);
    }
    if (mb_y == (pic_height >> 2) - 1) {
        filter_mb_uy = 4 + (block_y_mod16 << 1);
    }
    for (int j = 0; j < 6; ++j) {
#pragma HLS PIPELINE
        for (int i = 3; i < 6; ++i) {
			ubuffer[j][i] = dbk_mid_u[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
			vbuffer[j][i] = dbk_mid_v[filter_mb_uy - 1 + j][filter_mb_ux - 1 + i];
        }
    }
}

void copy_buffer_y_hor(int filter_mb_y, int filter_mb_x, pel ybuffer[8][8], pel dbk_y[71][71], pel dbkout_y[71][71], pel dbk_mid_y[71][71]){

	if (filter_mb_x <= 64 && filter_mb_x >= 0 && filter_mb_y <= 64 && filter_mb_y >= 0)
	{
		for (int j = 4; j < 8; ++j) {
#pragma HLS PIPELINE
			for (int i = 0; i < 8; ++i) {
				//dbk_y[filter_mb_y - 2 + i][filter_mb_x - 2 + j] = ybuffer[i][j];
				dbkout_y[filter_mb_y - 2 + i][filter_mb_x - 2 + j] = ybuffer[i][j];
				dbk_mid_y[filter_mb_y - 2 + i][filter_mb_x - 2 + j] = ybuffer[i][j];
			}
		}
	}
}

void copy_buffer_uv_hor(int filter_mb_uy, int filter_mb_ux, U11 mb_y, U11 mb_x, pel dbk_uv[39][39], pel uvbuffer[6][6], pel dbkout_uv[39][39], pel dbk_mid_uv[39][39]){
	if (filter_mb_ux <= 36 && filter_mb_ux >= 0 && filter_mb_uy <= 36 && filter_mb_uy >= 0)
	{
		for (int j = 3; j < 5; ++j) {
#pragma HLS PIPELINE
			for (int i = 1; i < 5; ++i) {
				//dbk_uv[filter_mb_uy - 1 + i][filter_mb_ux - 1 + j] = uvbuffer[i][j];
				dbkout_uv[filter_mb_uy - 1 + i][filter_mb_ux - 1 + j] = uvbuffer[i][j];
				dbk_mid_uv[filter_mb_uy - 1 + i][filter_mb_ux - 1 + j] = uvbuffer[i][j];
			}
		}
	}
}



void Buffer_hor(U7 filter_mb_y, U7 filter_mb_x, U6 filter_mb_uy, U6 filter_mb_ux, U11 mb_y, U11 mb_x, pel ybuffer[8][8], pel ubuffer[6][6], pel vbuffer[6][6],
		pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39], 
		pel dbkout_y[71][71], pel dbk_mid_y[71][71], pel dbkout_u[39][39], pel dbk_mid_u[39][39], pel dbkout_v[39][39], pel dbk_mid_v[39][39])
{
	copy_buffer_y_hor((int)filter_mb_y,(int)filter_mb_x, ybuffer, dbk_y, dbkout_y, dbk_mid_y);
	copy_buffer_uv_hor((int)filter_mb_uy, (int)filter_mb_ux, mb_y, mb_x, dbk_u, ubuffer, dbkout_u, dbk_mid_u);
	copy_buffer_uv_hor((int)filter_mb_uy, (int)filter_mb_ux, mb_y, mb_x, dbk_v, vbuffer, dbkout_v, dbk_mid_v);

}


int dbk_com_split_part_count(U3 split_mode)
{
	switch (split_mode) {
	case NO_SPLIT:
		return 1;
	case SPLIT_BI_VER:
		return 2;
	case SPLIT_BI_HOR:
		return 2;
	case SPLIT_EQT_VER:
		return 4;
	case SPLIT_EQT_HOR:
		return 4;
	case SPLIT_QUAD:
		return 4;
	default:
		return 0;
	}
}
SPLIT_DIR dbk_com_split_get_direction(U3 mode)
{
	switch (mode) {
	case SPLIT_BI_HOR:
#if EQT
	case SPLIT_EQT_HOR:
#endif
		return SPLIT_HOR;
	case SPLIT_BI_VER:
#if EQT
	case SPLIT_EQT_VER:
		return SPLIT_VER;
#endif
	default:
		return SPLIT_QT;
	}
}
int dbk_com_split_is_vertical(U3 mode)
{
	return dbk_com_split_get_direction(mode) == SPLIT_VER ? 1 : 0;
}

U8 dbk_com_split_get_part_size(U3 split_mode, U3 part_num, U8 length)
{
	U8 ans = length;
	switch (split_mode) {
	case SPLIT_QUAD:
	case SPLIT_BI_HOR:
	case SPLIT_BI_VER:
		ans = length >> 1;
		break;
#if EQT
	case SPLIT_EQT_HOR:
	case SPLIT_EQT_VER:
		if ((part_num == 1) || (part_num == 2)) {
			ans = length >> 1;
		}
		else {
			ans = length >> 2;
		}
		break;
#endif
	}
	return ans;
}
U3 dbk_com_split_get_part_size_idx(U3 split_mode, U3 part_num, U3 length_idx)
{
	int ans = length_idx;
	switch (split_mode) {
	case SPLIT_QUAD:
	case SPLIT_BI_HOR:
	case SPLIT_BI_VER:
		ans = length_idx - 1;
		break;
#if EQT
	case SPLIT_EQT_HOR:
	case SPLIT_EQT_VER:
		if ((part_num == 1) || (part_num == 2)) {
			ans = length_idx - 1;
		}
		else {
			ans = length_idx - 2;
		}
		break;
#endif
	}
	return ans;
}
void dbk_com_split_get_part_structure(int split_mode, int x0, int y0, int cu_width, int cu_height, int cup, int cud,
	int log2_culine, COM_SPLIT_STRUCT* split_struct)
{
	int i;
	int log_cuw, log_cuh;
	int cup_w, cup_h;
	split_struct->part_count = dbk_com_split_part_count(split_mode);
	log_cuw = CONV_LOG2(cu_width);
	log_cuh = CONV_LOG2(cu_height);
#pragma HLS ARRAY_PARTITION variable=split_struct.width complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct.height complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct.log_cuw complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct.log_cuh complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct.xpos complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct.ypos complete dim=0
#pragma HLS ARRAY_PARTITION variable=split_struct.cup complete dim=0

	split_struct->x_pos[0] = x0;
	split_struct->y_pos[0] = y0;
	split_struct->cup[0] = cup;
	switch (split_mode) {
	case NO_SPLIT: {
		split_struct->width[0] = cu_width;
		split_struct->height[0] = cu_height;
		split_struct->log_cuw[0] = log_cuw;
		split_struct->log_cuh[0] = log_cuh;
	}
				 break;
	case SPLIT_QUAD: {
		split_struct->width[0] = cu_width >> 1;
		split_struct->height[0] = cu_height >> 1;
		split_struct->log_cuw[0] = log_cuw - 1;
		split_struct->log_cuh[0] = log_cuh - 1;
		for (i = 1; i < split_struct->part_count; ++i) {
#pragma HLS UNROLL
#pragma HLS loop_tripcount max=3
			split_struct->width[i] = split_struct->width[0];
			split_struct->height[i] = split_struct->height[0];
			split_struct->log_cuw[i] = split_struct->log_cuw[0];
			split_struct->log_cuh[i] = split_struct->log_cuh[0];
		}
		split_struct->x_pos[1] = x0 + split_struct->width[0];
		split_struct->y_pos[1] = y0;
		split_struct->x_pos[2] = x0;
		split_struct->y_pos[2] = y0 + split_struct->height[0];
		split_struct->x_pos[3] = split_struct->x_pos[1];
		split_struct->y_pos[3] = split_struct->y_pos[2];
		cup_w = (split_struct->width[0] >> MIN_CU_LOG2);
		cup_h = ((split_struct->height[0] >> MIN_CU_LOG2) << log2_culine);
		split_struct->cup[1] = cup + cup_w;
		split_struct->cup[2] = cup + cup_h;
		split_struct->cup[3] = split_struct->cup[1] + cup_h;
		split_struct->cud = cud + 1;
	}
				   break;
	default: {
		if (dbk_com_split_is_vertical((SPLIT_MODE)split_mode)) {
			for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
#pragma HLS UNROLL
				split_struct->width[i] = dbk_com_split_get_part_size(split_mode, i, cu_width);
				split_struct->log_cuw[i] = dbk_com_split_get_part_size_idx(split_mode, i, log_cuw);

				if (split_mode == SPLIT_EQT_VER) {
					if (i == 0 || i == 3) {
						split_struct->height[i] = cu_height;
						split_struct->log_cuh[i] = log_cuh;
					}
					else {
						split_struct->height[i] = cu_height >> 1;
						split_struct->log_cuh[i] = log_cuh - 1;
					}
				}
				else {
					split_struct->height[i] = cu_height;
					split_struct->log_cuh[i] = log_cuh;
					if (i) {
						split_struct->x_pos[i] = split_struct->x_pos[i - 1] + split_struct->width[i - 1];
						split_struct->y_pos[i] = split_struct->y_pos[i - 1];
						split_struct->cup[i] = split_struct->cup[i - 1] + (split_struct->width[i - 1] >> MIN_CU_LOG2);
					}
				}

			}
#if EQT
			if (split_mode == SPLIT_EQT_VER) {
				split_struct->x_pos[1] = split_struct->x_pos[0] + split_struct->width[0];
				split_struct->y_pos[1] = split_struct->y_pos[0];
				split_struct->cup[1] = split_struct->cup[0] + (split_struct->width[0] >> MIN_CU_LOG2);
				cup_h = ((split_struct->height[1] >> MIN_CU_LOG2) << log2_culine);
				split_struct->x_pos[2] = split_struct->x_pos[1];
				split_struct->y_pos[2] = split_struct->y_pos[1] + split_struct->height[1];
				split_struct->cup[2] = split_struct->cup[1] + cup_h;
				split_struct->x_pos[3] = split_struct->x_pos[1] + split_struct->width[1];
				split_struct->y_pos[3] = split_struct->y_pos[1];
				split_struct->cup[3] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
			}
#endif
		}
		else {
			for (i = 0; i < split_struct->part_count; ++i) {
#pragma HLS loop_tripcount max=4
#pragma HLS UNROLL
				if (split_mode == SPLIT_EQT_HOR) {
					if (i == 0 || i == 3) {
						split_struct->width[i] = cu_width;
						split_struct->log_cuw[i] = log_cuw;
					}
					else {
						split_struct->width[i] = cu_width >> 1;
						split_struct->log_cuw[i] = log_cuw - 1;
					}
				}
				else {
					split_struct->width[i] = cu_width;
					split_struct->log_cuw[i] = log_cuw;
					if (i) {
						split_struct->y_pos[i] = split_struct->y_pos[i - 1] + split_struct->height[i - 1];
						split_struct->x_pos[i] = split_struct->x_pos[i - 1];
						split_struct->cup[i] = split_struct->cup[i - 1] + ((split_struct->height[i - 1] >> MIN_CU_LOG2) << log2_culine);
					}
				}

				split_struct->height[i] = dbk_com_split_get_part_size(split_mode, i, cu_height);
				split_struct->log_cuh[i] = dbk_com_split_get_part_size_idx(split_mode, i, log_cuh);
			}
#if EQT
			if (split_mode == SPLIT_EQT_HOR) {
				split_struct->y_pos[1] = split_struct->y_pos[0] + split_struct->height[0];
				split_struct->x_pos[1] = split_struct->x_pos[0];
				split_struct->cup[1] = split_struct->cup[0] + ((split_struct->height[0] >> MIN_CU_LOG2) << log2_culine);
				split_struct->y_pos[2] = split_struct->y_pos[1];
				split_struct->x_pos[2] = split_struct->x_pos[1] + split_struct->width[1];
				split_struct->cup[2] = split_struct->cup[1] + (split_struct->width[1] >> MIN_CU_LOG2);
				split_struct->y_pos[3] = split_struct->y_pos[1] + split_struct->height[1];
				split_struct->x_pos[3] = split_struct->x_pos[1];
				split_struct->cup[3] = split_struct->cup[1] + ((split_struct->height[1] >> MIN_CU_LOG2) << log2_culine);
			}
#endif
		}
		switch (split_mode) {
		case SPLIT_BI_VER:
			split_struct->cud = cud + ((cu_width == cu_height || cu_width < cu_height) ? 0 : 1);
			break;
		case SPLIT_BI_HOR:
			split_struct->cud = cud + ((cu_width == cu_height || cu_width > cu_height) ? 0 : 1);
			break;
		default:
			// Triple tree case
			split_struct->cud = cud + (cu_width == cu_height ? 0 : 1);
			break;
		}
	}
		   break;
	}
}


#if EQT
int dbk_com_split_is_EQT(SPLIT_MODE mode)
{
	return (mode == SPLIT_EQT_HOR) || (mode == SPLIT_EQT_VER) ? 1 : 0;
}
#endif

int  dbk_com_split_is_BT(SPLIT_MODE mode)
{
	return (mode == SPLIT_BI_HOR) || (mode == SPLIT_BI_VER) ? 1 : 0;
}

#if MODE_CONS
u8 dbk_com_constrain_pred_mode(int w, int h, SPLIT_MODE split, u8 slice_type)
{
	if (slice_type == SLICE_I) {
		return 0;
	}
	else {
		int s = w * h;
		if ((dbk_com_split_is_EQT(split) && s == 128) || ((dbk_com_split_is_BT(split) || split == SPLIT_QUAD) && s == 64)) {
			return 1;
		}
		else {
			return 0;
		}
	}
}
#endif

u8 dbk_com_tree_split(int w, int h, SPLIT_MODE split)
{
	if (split == SPLIT_QUAD)
	{
		if (w == 8)
			return 1;
		else
			return 0;
	}
	else if (split == SPLIT_EQT_HOR)
	{
		if (h == 16 || w == 8)
			return 1;
		else
			return 0;
	}
	else if (split == SPLIT_EQT_VER)
	{
		if (w == 16 || h == 8)
			return 1;
		else
			return 0;
	}
	else if (split == SPLIT_BI_HOR)
	{
		if (h == 8)
			return 1;
		else
			return 0;
	}
	else if (split == SPLIT_BI_VER)
	{
		if (w == 8)
			return 1;
		else
			return 0;
	}
	else
	{
		return 0;
	}
}


int dbk_com_get_split_mode(s8* split_mode, u8* cons_pred_mode, int cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[NUM_BLOCK_SHAPE][MAX_CU_CNT_IN_LCU])
{
	int ret = COM_OK;
	//cup = 0;
	int pos = cup + (((cu_height >> 1) >> MIN_CU_LOG2) * (lcu_s >> MIN_CU_LOG2) + ((cu_width >> 1) >> MIN_CU_LOG2));
	int shape = SQUARE + (CONV_LOG2(cu_width) - CONV_LOG2(cu_height));
	assert(shape >= 0 && shape < NUM_BLOCK_SHAPE);
	if (cu_width < 8 && cu_height < 8)
	{
		*split_mode = NO_SPLIT;
		*cons_pred_mode = NO_MODE_CONS;
		return ret;
	}

	*split_mode = split_mode_buf[cud][shape][pos] & 0x07;
	*cons_pred_mode = (split_mode_buf[cud][shape][pos] >> 3) & 0x03;

	return ret;
}

static void coef_rect_to_series_srcc(int max_cuwh, int log2_max_cuwh,
	s16 coef_y[64][64], s16 coef_u[4096], s16 coef_v[4096],
	int x, int y, int cu_width, int cu_height, s16 coef_dst[N_C][MAX_CU_DIM]

)
{
#pragma HLS ARRAY_PARTITION variable=coef_dst complete dim=1
	int i, j, sidx, didx;
	sidx = (x & (max_cuwh - 1)) + ((y & (max_cuwh - 1)) << log2_max_cuwh);
	didx = 0;
	for (j = 0; j < cu_height; j++)
	{
#pragma HLS loop_tripcount max=32
#pragma HLS PIPELINE
		for (i = 0; i < cu_width; i++)
		{
#pragma HLS loop_tripcount max=32
			coef_dst[Y_C][didx++] = coef_y[(y & (max_cuwh - 1)) + j][(x & (max_cuwh - 1)) + i];
		}
		sidx += max_cuwh;
	}
	x >>= 1;
	y >>= 1;
	cu_width >>= 1;
	cu_height >>= 1;
	sidx = (x & ((max_cuwh >> 1) - 1)) + ((y & ((max_cuwh >> 1) - 1)) << (log2_max_cuwh - 1));
	didx = 0;
	for (j = 0; j < cu_height; j++)
	{
#pragma HLS loop_tripcount max=32
#pragma HLS PIPELINE
		for (i = 0; i < cu_width; i++)
		{
#pragma HLS loop_tripcount max=32
			coef_dst[U_C][didx] = coef_u[sidx + i];
			coef_dst[V_C][didx] = coef_v[sidx + i];
			didx++;
		}
		sidx += (max_cuwh >> 1);
	}
}
//int prepare_for_srcc_unit_chroma(ENC_CU_DATA* cu_data, int x, int y, int cup, int cu_width, int cu_height)
//{
//#pragma HLS ARRAY_PARTITION variable=coef cyclic factor=64  dim=2
//#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
//	s16 coef[3][MAX_CU_DIM] = { 0 };
//	/* get coefficients and tq */
//	coef_rect_to_series_srcc(64, 6, cu_data->coef_y, cu_data->coef_u, cu_data->coef_v, x, y, cu_width, cu_height, coef);
//	return COM_OK;
//}
static u16 n_zigzag_scan_srcc(int* col, int* row, int sr_x, int sr_y, int size_x, int size_y, N_ZIGZAG_CHOICE_dbk* choice)
{
	u16 res;
	res = (*col) + (*row) * size_x;

	if (sr_x == 1) {
		(*row)--;
	}
	else if (sr_y == 1) {
		(*col)--;
	}
	else if ((*col) != 0 || (*row) != 0)
	{
		switch (*choice)
		{
		case N_GO_UP_dbk:
			(*row)--;
			if ((*col) == (sr_x - 1))
				(*choice) = N_LEFT_DOWN_dbk;
			else
				(*choice) = N_RIGHT_UP_dbk;
			break;
		case N_RIGHT_UP_dbk:
			(*row)--;
			(*col)++;
			if ((*row) == 0 && (*col) != 0)
				(*choice) = N_LEFT_TOWARDS_dbk;
			else if ((*col) == sr_x - 1)
				(*choice) = N_GO_UP_dbk;
			else
				(*choice) = N_RIGHT_UP_dbk;
			break;
		case N_LEFT_DOWN_dbk:
			(*row)++;
			(*col)--;
			if ((*col) == 0)
				(*choice) = N_GO_UP_dbk;
			else if ((*row) == sr_y - 1)
				(*choice) = N_LEFT_TOWARDS_dbk;
			else
				(*choice) = N_LEFT_DOWN_dbk;
			break;
		case N_LEFT_TOWARDS_dbk:
			(*col)--;
			if ((*row) == 0)
				(*choice) = N_LEFT_DOWN_dbk;
			else
				(*choice) = N_RIGHT_UP_dbk;
			break;
		}
	}
	return res;
}
void coef_reorg_for_srcc(s16 coef[4096], s16 coef_for_srcc[256][16], s16 blkpos_for_srcc[256][16], int line_idx_in_lcu, int num_sig, int cu_width, int cu_height) {
	if (num_sig){
	int line_idx = line_idx_in_lcu;
	int last_scan_set;
	int temp_x;
	int temp_y;
	int sr_x;
	int sr_y;
	int sr_width;
	int sr_height;
	int num_coef;
	int cg_log2_size = LOG2_CG_SIZE;
	int scan_pos_last;
	int ipos;
	int sub_set;
	int blkpos;
	temp_x = (num_sig >> 12) & 0xff;
	temp_y = (num_sig >> 20) & 0xff;
	sr_x = temp_x;
	sr_y = temp_y;
	sr_width = sr_x + 1;
	sr_height = sr_y + 1;
	num_coef = sr_width * sr_height;
	last_scan_set = (num_coef - 1) >> cg_log2_size;
	scan_pos_last = num_coef - 1;
	N_ZIGZAG_CHOICE_dbk choice;
	if ((sr_x + sr_y) % 2 == 0) {
		choice = N_LEFT_TOWARDS_dbk;
	}
	else if ((sr_x + sr_y) % 2 == 1) {
		choice = N_GO_UP_dbk;
	}
	int col = sr_x;
	int row = sr_y;
	ipos = scan_pos_last;
	for (sub_set = last_scan_set; sub_set >= 0; sub_set--)
	{
#pragma HLS loop_tripcount max=64
#pragma HLS PIPELINE
		int sub_pos = sub_set << cg_log2_size;
		int i = 0;
		for (; ipos >= sub_pos; ipos--)
		{
#pragma HLS loop_tripcount max=16
			blkpos = n_zigzag_scan_srcc(&col, &row, sr_width, sr_height, cu_width, cu_height, &choice);
			coef_for_srcc[line_idx][i] = coef[blkpos];
			blkpos_for_srcc[line_idx][i] = blkpos;
			//printf("%d ", coef[blkpos]);
			i++;
		}
		//printf("\n");
		line_idx++;
	}
	}else {

	}
}
int prepare_for_srcc_unit(ENC_CU_DATA* cu_data, int x, int y, int cup, int cu_width, int cu_height, int x_lcu_idx, int y_lcu_idx, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, int skip_flag, U8 line_idx_in_lcu[3])
{
#pragma HLS INLINE OFF
	s16 coef[3][MAX_CU_DIM];
#pragma HLS ARRAY_PARTITION variable=coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=srcc_for_aec->coef_for_srcc complete dim=1
#pragma HLS ARRAY_PARTITION variable=srcc_for_aec->blkpos_for_srcc complete dim=1

#pragma HLS ARRAY_RESHAPE   variable=srcc_for_aec->coef_for_srcc   complete dim=3
#pragma HLS ARRAY_RESHAPE   variable=srcc_for_aec->blkpos_for_srcc complete dim=3

//#pragma HLS ARRAY_PARTITION variable=coef cyclic factor=64 dim=2
	//for (int i = 0;i<3;i++){
//#pragma HLS PIPELINE II=64
//		for (int j = 0; j<4096;j++){
//			coef[i][j] = 0;
//		}
//	}

	//s16 coef[3][MAX_CU_DIM] = { 0 };
	//s16 coef_for_srcc[3][256][16];
	//s16 blkpos_for_srcc[3][256][16];
	/*int* line_idx_in_lcu;
	int* line_idx_in_lcu_u;
	int* line_idx_in_lcu_v;*/
	int x_cu = x - 64 * x_lcu_idx;
	int y_cu = y - 64 * y_lcu_idx;
	//printf("%d %d\n", x_cu, y_cu);
	/* get coefficients and tq */
	int index = (y_cu << 2) + (x_cu >> 2);
	int num_nz_coef_y = cu_data->num_nz_coef[0][index];
	int num_nz_coef_u = cu_data->num_nz_coef[1][index];
	int num_nz_coef_v = cu_data->num_nz_coef[2][index];
	/*if (!skip_flag) {*/
	coef_rect_to_series_srcc(64, 6, cu_data->coef_y, cu_data->coef_u, cu_data->coef_v, x, y, cu_width, cu_height, coef);
	//y
	if (num_nz_coef_y)
		coef_reorg_for_srcc(coef[0], srcc_for_aec->coef_for_srcc[0], srcc_for_aec->blkpos_for_srcc[0], line_idx_in_lcu[0], num_nz_coef_y, cu_width, cu_height);
	//u
	if (num_nz_coef_u)
		coef_reorg_for_srcc(coef[1], srcc_for_aec->coef_for_srcc[1], srcc_for_aec->blkpos_for_srcc[1], line_idx_in_lcu[1], num_nz_coef_u, cu_width >> 1, cu_height >> 1);
	//v
	if (num_nz_coef_v)
		coef_reorg_for_srcc(coef[2], srcc_for_aec->coef_for_srcc[2], srcc_for_aec->blkpos_for_srcc[2], line_idx_in_lcu[2], num_nz_coef_v, cu_width >> 1, cu_height >> 1);
	/*srcc_for_aec->line_idx_in_lcu_y += cu_width * cu_height / 16;
	srcc_for_aec->line_idx_in_lcu_u += cu_width * cu_height / 64;
	srcc_for_aec->line_idx_in_lcu_v += cu_width * cu_height / 64;*/
	line_idx_in_lcu[0] += cu_width * cu_height / 16;
	line_idx_in_lcu[1] += cu_width * cu_height / 64;
	line_idx_in_lcu[2] += cu_width * cu_height / 64;

	/*static int count = 0;
	count++;
	FILE* file;
	file = fopen("coef.txt", "a+");
	fprintf(file, "lcu=%d\n", lcu_num_in_frame);
	fprintf(file, "count=%d\n", count);
	fprintf(file, "%d %d\n", cu_width, cu_height);
	for (int i = 0; i < cu_height; i++)
	{
		for (int j = 0; j < cu_width; j++)
		{
			fprintf(file, "%d ", coef[0][i*cu_height + j]);
		}
		fprintf(file, "\n");
	}
	for (int i = 0; i < (cu_height >> 1); i++)
	{
		for (int j = 0; j < (cu_width >> 1); j++)
		{
			fprintf(file, "%d ", coef[1][i*(cu_height >> 1) + j]);
		}
		fprintf(file, "\n");
	}
	for (int i = 0; i < (cu_height >> 1); i++)
	{
		for (int j = 0; j < (cu_width >> 1); j++)
		{
			fprintf(file, "%d ", coef[2][i*(cu_height >> 1) + j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);*/
	//}

	return COM_OK;
}

int prepare_for_srcc_tree_4(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status, SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec)
{

	int ret;

	//ret = prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);

	return 0;
}
int prepare_for_srcc_tree_8x4(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3])
{
	int ret;
	s8 split_mode;



#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif

	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);

	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_tree_4(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child, AEC_DATA_input, srcc_for_aec);
			}
		}
#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);
	}
	return COM_OK;
}
int prepare_for_srcc_tree_8(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3])
{
	int ret;
	s8 split_mode;

#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif

	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);

	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_tree_8x4(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					AEC_DATA_input, srcc_for_aec, srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}
#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp,skip_flag, line_idx_in_lcu);
	}
	return COM_OK;
}

int prepare_for_srcc_tree_16x8(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3])
{
	int ret;
	s8 split_mode;

#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif
	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);

	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_tree_8(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					AEC_DATA_input, srcc_for_aec, srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}
#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);

	}
	return COM_OK;

}

int prepare_for_srcc_tree_16(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3])
{
	int ret;
	s8 split_mode;
#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif

	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);

	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_tree_16x8(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					AEC_DATA_input, srcc_for_aec, srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}
#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);
	}
	return COM_OK;

}

int prepare_for_srcc_32x16(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3])
{
	int ret;
	s8 split_mode;



#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif

	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);

	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_tree_16(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					AEC_DATA_input, srcc_for_aec, srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}
#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);
	}
	return COM_OK;

}

int prepare_for_srcc_tree_32(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3])
{
	int ret;
	s8 split_mode;

#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif

	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);

	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_32x16(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					AEC_DATA_input, srcc_for_aec, srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}

#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);
	}
	return COM_OK;

}
int prepare_for_srcc_64x32(ENC_CU_DATA* ctu_map_cu_data_DBK,
	SRCC_FW_input* AEC_FW_data_in, int x0, int y0, int cup, int cu_width, int cu_height, int cud
	, const int parent_split, int qt_depth, int bet_depth, u8 cons_pred_mode, u8 tree_status,
	SRCC_input* AEC_DATA_input, SRCC_FOR_AEC* srcc_for_aec, SRCC_FOR_AEC* srcc_for_aec_tmp, U8 line_idx_in_lcu[3]) {
	int ret;
	s8 split_mode;
#if CHROMA_NOT_SPLIT
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif
	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, cu_width, cu_height, AEC_FW_data_in->max_cuwh, AEC_DATA_input->cu_data_split_mode);
	if (split_mode != NO_SPLIT)
	{
#if CHROMA_NOT_SPLIT
		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(cu_width, cu_height, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;
#endif
#if MODE_CONS
		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(cu_width, cu_height, (SPLIT_MODE)split_mode, AEC_FW_data_in->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}
#endif
		COM_SPLIT_STRUCT split_struct;
		dbk_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, AEC_FW_data_in->log2_culine, &split_struct);
		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < AEC_FW_data_in->pic_width && y_pos < AEC_FW_data_in->pic_height)
			{
				ret = prepare_for_srcc_tree_32(ctu_map_cu_data_DBK, AEC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					AEC_DATA_input, srcc_for_aec, srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}
#if CHROMA_NOT_SPLIT
		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height);
		//}
#endif
	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, x0, y0, cup, cu_width, cu_height, AEC_DATA_input->x_lcu_idx, AEC_DATA_input->y_lcu_idx, srcc_for_aec, srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);
	}
	return COM_OK;



}
void prepare_for_srcc(ENC_CU_DATA* ctu_map_cu_data_DBK, int cup, int cud, AEC_FW* fetch2dbk_firmware, u8 tree_status, u8 cons_pred_mode, int fetch_output_dbk_lcu_num, SRCC_FOR_AEC* srcc_for_aec) {
	s8 split_mode;
	int ret;
#if CHROMA_NOT_SPLIT
	//TREE_LC = 0
	u8 tree_status_child = TREE_LC;
#endif
#if MODE_CONS
	u8 cons_pred_mode_child;
	u8 cons_pred_mode_child_tmp;
#endif
	static SRCC_input  SRCC_DATA_input;
	SRCC_FW_input SRCC_FW_data_in;
	const int parent_split = NO_SPLIT;
	int qt_depth = 0;
	int bet_depth = 0;
	if (fetch_output_dbk_lcu_num == 0)//look as frame start
	{
		SRCC_DATA_input.lcu_idx = 0;
		SRCC_DATA_input.x_lcu_idx = 0;
		SRCC_DATA_input.y_lcu_idx = 0;
		SRCC_DATA_input.x_pel = 0;
		SRCC_DATA_input.y_pel = 0;
		SRCC_DATA_input.fetch_output_me_line_map_ptr_min_scu_x = 0;
		SRCC_DATA_input.fetch_output_me_line_map_ptr_min_scu_y = 0;

	}
	else
	{
		SRCC_DATA_input.lcu_idx += 1;
		SRCC_DATA_input.x_pel = SRCC_DATA_input.x_lcu_idx * 64;
		SRCC_DATA_input.y_pel = SRCC_DATA_input.y_lcu_idx * 64;
		SRCC_DATA_input.fetch_output_me_line_map_ptr_min_scu_x = SRCC_DATA_input.x_pel >> 2;
		SRCC_DATA_input.fetch_output_me_line_map_ptr_min_scu_y = SRCC_DATA_input.y_pel >> 2;

	}

	int i, j, k;
	/*for (i = 0; i < N_C; ++i)
	{
		SRCC_DATA_input.slice_sao_enable[i] = fetch2dbk_firmware->slice_sao_enable[i];
	}*/

#pragma HLS ARRAY_PARTITION variable=SRCC_DATA_input.cu_data_split_mode cyclic factor=16  dim=3
#pragma HLS ARRAY_PARTITION variable=ctu_map_cu_data_DBK->split_mode cyclic factor=16  dim=3
	copy_split_mode: for (i = 0; i < MAX_CU_DEPTH; i++) {
#pragma HLS PIPELINE
		for (j = 0; j < NUM_BLOCK_SHAPE; j++) {
#pragma HLS PIPELINE II=16
			for (k = 0; k < MAX_CU_CNT_IN_LCU; k++) {
				SRCC_DATA_input.cu_data_split_mode[i][j][k] = ctu_map_cu_data_DBK->split_mode[i][j][k];
			}
		}
	}
	int n = 0;
	SRCC_FOR_AEC srcc_for_aec_tmp;
	U8 line_idx_in_lcu[3];
	line_idx_in_lcu[0] = 0;//y
	line_idx_in_lcu[1] = 0;//u
	line_idx_in_lcu[2] = 0;//v
	/*srcc_for_aec->line_idx_in_lcu_y = 0;
	srcc_for_aec->line_idx_in_lcu_u = 0;
	srcc_for_aec->line_idx_in_lcu_v = 0;*/

	//srcc_for_aec->line_idx_in_lcu_y = 0;
	//srcc_for_aec->line_idx_in_lcu_u = 0;
	//srcc_for_aec->line_idx_in_lcu_v = 0;
	/*SRCC_FW_data_in.ipf_enable_flag = fetch2dbk_firmware->ipf_enable_flag;
	SRCC_FW_data_in.sample_adaptive_offset_enable_flag = fetch2dbk_firmware->sample_adaptive_offset_enable_flag;*/
	//SRCC_FW_data_in.pic_width_in_scu = fetch2dbk_firmware->pic_width_in_scu;
	SRCC_FW_data_in.max_cuwh = fetch2dbk_firmware->max_cuwh;
	SRCC_FW_data_in.pic_width = fetch2dbk_firmware->pic_width;
	SRCC_FW_data_in.pic_height = fetch2dbk_firmware->pic_height;
	SRCC_FW_data_in.slice_type = fetch2dbk_firmware->slice_type;
	SRCC_FW_data_in.log2_culine = fetch2dbk_firmware->log2_culine;
	//SRCC_FW_data_in.lcu_cnt = fetch2dbk_firmware->lcu_total_cnt;

	/*SRCC_FW_data_in.sqh_num_of_hmvp_cand = fetch2dbk_firmware->num_of_hmvp_cand;
	SRCC_FW_data_in.sqh_umve_enable_flag = fetch2dbk_firmware->umve_enable_flag;
	SRCC_FW_data_in.sqh_affine_enable_flag = fetch2dbk_firmware->affine_enable_flag;
	SRCC_FW_data_in.sqh_amvr_enable_flag = fetch2dbk_firmware->amvr_enable_flag;*/

	dbk_com_get_split_mode(&split_mode, &cons_pred_mode_child_tmp, cud, cup, SRCC_FW_data_in.max_cuwh, SRCC_FW_data_in.max_cuwh, fetch2dbk_firmware->max_cuwh, (&SRCC_DATA_input)->cu_data_split_mode);
	if (split_mode != NO_SPLIT)//
	{

		tree_status_child = (tree_status == TREE_LC && dbk_com_tree_split(SRCC_FW_data_in.max_cuwh, SRCC_FW_data_in.max_cuwh, (SPLIT_MODE)split_mode)) ? TREE_L : tree_status;

		if (cons_pred_mode == NO_MODE_CONS && dbk_com_constrain_pred_mode(SRCC_FW_data_in.max_cuwh, SRCC_FW_data_in.max_cuwh, (SPLIT_MODE)split_mode, fetch2dbk_firmware->slice_type))
		{
			cons_pred_mode_child = cons_pred_mode_child_tmp;
		}
		else
		{
			cons_pred_mode_child = cons_pred_mode;
		}

		COM_SPLIT_STRUCT split_struct;

		dbk_com_split_get_part_structure(split_mode, SRCC_DATA_input.x_pel, SRCC_DATA_input.y_pel, SRCC_FW_data_in.max_cuwh, SRCC_FW_data_in.max_cuwh, cup, cud, fetch2dbk_firmware->log2_culine, &split_struct);

		for (int part_num = 0; part_num < split_struct.part_count; ++part_num)
		{
#pragma HLS loop_tripcount max=4
			int cur_part_num = part_num;
			int sub_cuw = split_struct.width[cur_part_num];
			int sub_cuh = split_struct.height[cur_part_num];
			int x_pos = split_struct.x_pos[cur_part_num];
			int y_pos = split_struct.y_pos[cur_part_num];

			if (x_pos < fetch2dbk_firmware->pic_width && y_pos < fetch2dbk_firmware->pic_height) // check if inner the picture
			{
				ret = prepare_for_srcc_64x32(ctu_map_cu_data_DBK, &SRCC_FW_data_in, x_pos, y_pos, split_struct.cup[cur_part_num], sub_cuw, sub_cuh, split_struct.cud
					, split_mode, INC_QT_DEPTH(qt_depth, split_mode), INC_BET_DEPTH(bet_depth, split_mode), cons_pred_mode_child, tree_status_child,
					&SRCC_DATA_input, srcc_for_aec, &srcc_for_aec_tmp, line_idx_in_lcu);
			}
		}

		//if (tree_status_child == TREE_L && tree_status == TREE_LC)
		//{
		//	prepare_for_srcc_unit_chroma(ctu_map_cu_data_DBK, SRCC_DATA_input.x_pel, SRCC_DATA_input.y_pel, cup, SRCC_FW_data_in.max_cuwh, SRCC_FW_data_in.max_cuwh);
		//}

	}
	else
	{
		int skip_flag = 0;
#if CHROMA_NOT_SPLIT //wrong for TREE_C
		if (tree_status != TREE_C)
		{
#endif
			if (ctu_map_cu_data_DBK->pred_mode[cup] == MODE_SKIP)
			{
				skip_flag = 1;
			}
#if CHROMA_NOT_SPLIT
		}
#endif
		prepare_for_srcc_unit(ctu_map_cu_data_DBK, SRCC_DATA_input.x_pel, SRCC_DATA_input.y_pel, cup, SRCC_FW_data_in.max_cuwh, SRCC_FW_data_in.max_cuwh, SRCC_DATA_input.x_lcu_idx,
			SRCC_DATA_input.y_lcu_idx, srcc_for_aec, &srcc_for_aec_tmp, skip_flag, line_idx_in_lcu);

	}
	if (SRCC_DATA_input.x_lcu_idx < (fetch2dbk_firmware->pic_width_in_lcu - 1))
	{
		SRCC_DATA_input.x_lcu_idx += 1;
	}
	else
	{
		SRCC_DATA_input.x_lcu_idx = 0;
		SRCC_DATA_input.y_lcu_idx += 1;
	}
	srcc_for_aec->line_idx_in_lcu_y = srcc_for_aec_tmp.line_idx_in_lcu_y;
	srcc_for_aec->line_idx_in_lcu_u = srcc_for_aec_tmp.line_idx_in_lcu_u;
	srcc_for_aec->line_idx_in_lcu_v = srcc_for_aec_tmp.line_idx_in_lcu_v;
}


// void dbk_run(U13 fetch2md_fw_ptr_pic_width, U13 fetch2md_fw_ptr_pic_height, U6 fetch2md_fw_ptr_pic_width_in_lcu, U6 fetch2md_fw_ptr_pic_height_in_lcu,
// 	U11 fetch2md_fw_ptr_pic_width_in_scu, U3 fetch2md_fw_ptr_chroma_quant_param_delta_cb, U3 fetch2md_fw_ptr_chroma_quant_param_delta_cr,
// 	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
// 	pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39],
// 	U13 fetch_output_dbk_pix_x, U13 fetch_output_dbk_pix_y,
// 	U11 me_line_map_ptr_min_scu_x, U11 me_line_map_ptr_min_scu_y, U1 me_line_map_ptr_curr_ctu_idx, U1 me_line_map_ptr_above_line_idx,
// 	S8 me_line_map_ptr_line_map_split[5][7][256],
// 	U32 me_line_map_ptr_map_scu_inner[2][16][16],
// 	ENC_ME_NEB_INFO new_above_nebs_line[48], U32 me_line_map_ptr_new_map_scu_above_line[48],
// 	ENC_ME_NEB_INFO inner_nebs[2][16][16],
// 	DBK_OUT_PARA_SET* dbk_out_para_set, int use_deblock)

void dbk_run(
	U13 fetch2dbk_fw_ptr_pic_width, U12 fetch2dbk_fw_ptr_pic_height, U7 fetch2dbk_fw_ptr_pic_width_in_lcu, U6 fetch2dbk_fw_ptr_pic_height_in_lcu,
	S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39],
	U12 fetch_output_dbk_pix_x, U12 fetch_output_dbk_pix_y,
	U11 me_line_map_ptr_DBK_min_scu_x, U11 me_line_map_ptr_DBK_min_scu_y,
	//U6 me_line_map_ptr_line_map_split[5][7][256],
	U32 me_line_map_ptr_DBK_map_scu_inner[2][16][16],
	ENC_ME_NEB_INFO new_above_nebs_line_DBK[48], U32 me_line_map_ptr_DBK_new_map_scu_above_line[48],
	ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16], ENC_ME_NEB_INFO inner_nebs_sao[2][16][16],
	DBK_OUT_PARA_SET* dbk_out_para_set, U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U4 fetch2dbk_fw_ptr_bit_depth, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth, 
 U2 use_deblock
#if ENABLE_BFRAME
	//, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM]
 ,int fetch2dbk_fw_ptr_ref_ptr_dbk[MAX_NUM_REF_PICS][REFP_NUM]
#endif
	, ENC_CU_DATA* ctu_map_cu_data_DBK, AEC_FW* fetch2dbk_firmware, int fetch_output_dbk_lcu_num, SRCC_FOR_AEC* srcc_for_aec
)
{
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr_pic_width
#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr_pic_height

//#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr_chroma_quant_param_delta_cb
//#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr_chroma_quant_param_delta_cr

#pragma HLS INTERFACE axis port=fetch_output_dbk_pix_x
#pragma HLS INTERFACE axis port=fetch_output_dbk_pix_y
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK_min_scu_x
#pragma HLS INTERFACE axis port=me_line_map_ptr_DBK_min_scu_y

#pragma HLS INTERFACE axis port=dbk_out_para_set->pic_width
#pragma HLS INTERFACE axis port=dbk_out_para_set->pic_height
#pragma HLS INTERFACE axis port=dbk_out_para_set->x_pel
#pragma HLS INTERFACE axis port=dbk_out_para_set->y_pel
#pragma HLS INTERFACE axis port=dbk_out_para_set->pic_width_in_scu
#pragma HLS INTERFACE axis port=dbk_out_para_set->min_scu_x
#pragma HLS INTERFACE axis port=dbk_out_para_set->min_scu_y
#pragma HLS INTERFACE axis port=dbk_out_para_set->curr_ctu_idx
//#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr_bit_depth
//#pragma HLS INTERFACE axis port=fetch2dbk_fw_ptr_qp_offset_bit_depth
//#pragma HLS INTERFACE axis port=use_deblock

#pragma HLS RESOURCE variable=dbk_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbk_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbk_v core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_v core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr_DBK_map_scu_inner core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_DBK_new_map_scu_above_line core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=new_above_nebs_line_DBK_mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=new_above_nebs_line_DBK_refi core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=inner_nebs_md_chroma_mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=inner_nebs_md_chroma_refi core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=inner_nebs_sao_mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=inner_nebs_sao_refi core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbk_out_para_set->map_scu_inner core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=cur_ctu_cu_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fetch2dbk_fw_ptr_ref_ptr_dbk core=RAM_1P_BRAM

#if 1 //aec port
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->max_cuwh
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_width
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->pic_height
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->slice_type 
#pragma HLS INTERFACE axis port=fetch2dbk_firmware->log2_culine

#pragma HLS INTERFACE axis port=fetch_output_dbk_lcu_num

#pragma HLS RESOURCE variable=srcc_for_aec->coef_for_srcc core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=srcc_for_aec->blkpos_for_srcc core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK->split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK->num_nz_coef core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK->pred_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK_coef_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK_coef_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ctu_map_cu_data_DBK_coef_v core=RAM_1P_BRAM
#endif  //1(aec port)

#pragma HLS ARRAY_PARTITION variable=dbk_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbkout_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbkout_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbkout_v complete dim=2

#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=dbk_out_para_set->map_scu_inner complete dim=1

#pragma HLS ARRAY_PARTITION variable=new_above_nebs_line_DBK complete dim=0
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_DBK_new_map_scu_above_line complete dim=1
	//coef operations for AEC
	prepare_for_srcc(ctu_map_cu_data_DBK, 0, 0, fetch2dbk_firmware, TREE_LC, NO_MODE_CONS, fetch_output_dbk_lcu_num, srcc_for_aec);
int fetch2md_fw_ptr_pic_width_in_scu = fetch2dbk_fw_ptr_pic_width >> 2;

#ifdef COLLECT_DBK_DATA

//#define DATA_DECL(type, name) ,name
//#define DATA_DECL_AR(type, name, ...) ,name

//		int null = 0;
//		static int dbk_lcu_cnt = 0;
//		//if (dbk_lcu_cnt < LCU_TEST_NUM) {
//			static FILE* fp_in = fopen(INPUT_GOLDEN, "wb");
//			if (!fp_in) { printf("Open %s failed!\n", INPUT_GOLDEN); }
//			else {
//				DBK_PORT(EXTRACT_ARGS(PORT_DECL)).write_bin(fp_in);
//			}
//		//}
   dbk_input_tv(fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, fetch2dbk_fw_ptr_pic_width_in_lcu, fetch2dbk_fw_ptr_pic_height_in_lcu,
	fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
	dbk_y, dbk_u, dbk_v,
	//pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39],
	fetch_output_dbk_pix_x, fetch_output_dbk_pix_y,
	me_line_map_ptr_DBK_min_scu_x, me_line_map_ptr_DBK_min_scu_y,
	me_line_map_ptr_DBK_map_scu_inner,
	//new_above_nebs_line_DBK,
	me_line_map_ptr_DBK_new_map_scu_above_line,
	//inner_nebs_md_chroma,
	//ENC_ME_NEB_INFO inner_nebs_sao[2][16][16],
	//DBK_OUT_PARA_SET* dbk_out_para_set, 
	cur_ctu_cu_mode, fetch2dbk_fw_ptr_bit_depth, fetch2dbk_fw_ptr_qp_offset_bit_depth,
	   use_deblock, fetch_ref_ptr, ctu_map_cu_data_DBK, fetch2dbk_firmware, fetch_output_dbk_lcu_num);
#endif
		dbk_out_para_set->pic_width = fetch2dbk_fw_ptr_pic_width;
		dbk_out_para_set->pic_height = fetch2dbk_fw_ptr_pic_height;
		//dbk_out_para_set->pic_width_in_lcu = fetch2dbk_fw_ptr_pic_width_in_lcu;
		//dbk_out_para_set->pic_height_in_lcu = fetch2dbk_fw_ptr_pic_height_in_lcu;
		dbk_out_para_set->pic_width_in_scu = fetch2md_fw_ptr_pic_width_in_scu;
		dbk_out_para_set->x_pel = fetch_output_dbk_pix_x;
		dbk_out_para_set->y_pel = fetch_output_dbk_pix_y;

		dbk_out_para_set->min_scu_x = me_line_map_ptr_DBK_min_scu_x;   
		dbk_out_para_set->min_scu_y = me_line_map_ptr_DBK_min_scu_y;

#pragma HLS ARRAY_PARTITION variable=inner_nebs_sao complete dim=3
#pragma HLS ARRAY_PARTITION variable=inner_nebs_md_chroma complete dim=3
#pragma HLS ARRAY_PARTITION variable=inner_nebs_md_chroma_tmp complete dim=3
//		for (int i = 0; i < 2; i++) {
//			for (int j = 0; j < 16; j++) {
//				#pragma HLS PIPELINE II=16
//				for (int k = 0; k < 16; k++) {
//					#pragma HLS UNROLL
//					inner_nebs_sao[i][j][k].mv[0][0] = inner_nebs2dbk[i][j][k].mv[0][0];
//					inner_nebs_sao[i][j][k].mv[0][1] = inner_nebs2dbk[i][j][k].mv[0][1];
//					inner_nebs_sao[i][j][k].mv[1][0] = inner_nebs2dbk[i][j][k].mv[1][0];
//					inner_nebs_sao[i][j][k].mv[1][1] = inner_nebs2dbk[i][j][k].mv[1][1];
//
//					inner_nebs_sao[i][j][k].refi[0] = inner_nebs2dbk[i][j][k].refi[0];
//					inner_nebs_sao[i][j][k].refi[1] = inner_nebs2dbk[i][j][k].refi[1];
//
//					dbk_out_para_set->map_scu_inner[i][j][k] = me_line_map_ptr_map_scu_inner[i][j][k];
//				}
//			}
//		}


		static pel dbk_mid_y[71][71];
		static pel dbk_mid_u[39][39];
		static pel dbk_mid_v[39][39];

		static pel dbk_ver_y[71][71];
		static pel dbk_ver_u[39][39];
		static pel dbk_ver_v[39][39];


		//copy necessary edge data from input buffer to output buffer
#pragma HLS ARRAY_PARTITION variable=dbk_ver_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_ver_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_ver_v complete dim=2

#pragma HLS ARRAY_PARTITION variable=dbk_mid_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_mid_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=dbk_mid_v complete dim=2

#pragma HLS resource variable=dbk_mid_y core=RAM_2P_BRAM
#pragma HLS resource variable=dbk_mid_u core=RAM_2P_BRAM
#pragma HLS resource variable=dbk_mid_v core=RAM_2P_BRAM

#pragma HLS resource variable=dbk_ver_y core=RAM_2P_BRAM
#pragma HLS resource variable=dbk_ver_u core=RAM_2P_BRAM
#pragma HLS resource variable=dbk_ver_v core=RAM_2P_BRAM


		for (int i = 0; i < 71; i++){
			#pragma HLS PIPELINE
			for (int j = 0; j < 71; j++){
				dbk_ver_y[i][j] = dbk_y[i][j];
				if (i<39 && j<39){
					dbk_ver_u[i][j] = dbk_u[i][j];
					dbk_ver_v[i][j] = dbk_v[i][j];
				}
			}	
		}

		for (int i = 66; i <= 69; i++) {
#pragma HLS PIPELINE
			for (int j = 6; j <= 69; j++) {
				dbkout_y[i][j] = dbk_y[i][j];
				dbk_mid_y[i][j] = dbk_y[i][j];
			}
		}
		for (int j = 6; j <= 69; j++) {
#pragma HLS PIPELINE
			for (int i = 66; i <= 69; i++) {
				dbkout_y[j][i] = dbk_y[j][i];
				dbk_mid_y[j][i] = dbk_y[j][i];
			}
		}
		for (int i = 36; i <= 37; i++) {
#pragma HLS PIPELINE
			for (int j = 6; j <= 37; j++) {
				dbkout_u[i][j] = dbk_u[i][j];
				dbkout_v[i][j] = dbk_v[i][j];
				dbk_mid_u[i][j] = dbk_u[i][j];
				dbk_mid_v[i][j] = dbk_v[i][j];
			}
		}
		for (int j = 6; j <= 37; j++) {
#pragma HLS PIPELINE
			for (int i = 36; i <= 37; i++) {
				dbkout_u[j][i] = dbk_u[j][i];
				dbkout_v[j][i] = dbk_v[j][i];
				dbk_mid_u[j][i] = dbk_u[j][i];
				dbk_mid_v[j][i] = dbk_v[j][i];
			}
		}

		U2 ppbEdgeFilter_ctu_ver0[16][16];
		U2 ppbEdgeFilter_ctu_hor1[16][16];

		static U2 ppbEdgeFilter_ctu_buf1_ver0[16];
		static U2 ppbEdgeFilter_ctu_buf2_ver0[960];

		static U2 ppbEdgeFilter_ctu_buf1_hor1[16];
		static U2 ppbEdgeFilter_ctu_buf2_hor1[960];

		static pel sixfour_buffer[2][64][4];
		//lcu_stride + 1= 3840/64=60 + 1 = 61
		static pel foursix_buffer[61][4][64];
		//lcu_stride + 2= 3840/64=60 + 2 = 62
		static pel fourfour_buffer[62][4][4];
		static pel U30x3_buffer[2][36][4];
		//lcu_stride + 1
		static pel U3x30_buffer[61][4][36];
		//lcu_stride + 2
		static pel U3x3_buffer[62][4][4];
		static pel V30x3_buffer[2][36][4];
		//lcu_stride + 1
		static pel V3x30_buffer[61][4][36];
		//lcu_stride + 2
		static pel V3x3_buffer[62][4][4];
		// 960

#pragma HLS resource variable=ppbEdgeFilter_ctu_buf1_ver0 core=RAM_2P_BRAM
#pragma HLS resource variable=ppbEdgeFilter_ctu_buf2_ver0 core=RAM_2P_BRAM
#pragma HLS resource variable=ppbEdgeFilter_ctu_buf1_hor1 core=RAM_2P_BRAM
#pragma HLS resource variable=ppbEdgeFilter_ctu_buf2_hor1 core=RAM_2P_BRAM
#pragma HLS resource variable=ppbEdgeFilter_ctu_ver0 core=RAM_2P_BRAM
#pragma HLS resource variable=ppbEdgeFilter_ctu_hor1 core=RAM_2P_BRAM
#pragma HLS resource variable=U3x30_buffer core=RAM_2P_BRAM
#pragma HLS resource variable=U3x3_buffer core=RAM_2P_BRAM
#pragma HLS resource variable=V3x30_buffer core=RAM_2P_BRAM
#pragma HLS resource variable=V3x3_buffer core=RAM_2P_BRAM

#pragma HLS ARRAY_PARTITION variable=foursix_buffer complete dim=3
#pragma HLS ARRAY_PARTITION variable=U3x30_buffer complete dim=3
#pragma HLS ARRAY_PARTITION variable=V3x30_buffer complete dim=3

#pragma HLS ARRAY_PARTITION variable=sixfour_buffer complete dim=3
#pragma HLS ARRAY_PARTITION variable=U30x3_buffer complete dim=3
#pragma HLS ARRAY_PARTITION variable=V30x3_buffer complete dim=3

#pragma HLS ARRAY_PARTITION variable=fourfour_buffer complete dim=3
#pragma HLS ARRAY_PARTITION variable=U3x3_buffer complete dim=3
#pragma HLS ARRAY_PARTITION variable=V3x3_buffer complete dim=3

		U11 mb_x;//[lcu_x >> 2 - 1,(lcu_x + lcuw) >> 2] //U11
		U11 mb_y;//[lcu_y >> 2 - 1,(lcu_y + lcuh) >> 2] //U11

		U13 lcu_x = fetch_output_dbk_pix_x;//0-3840 U13
		U13 lcu_y = fetch_output_dbk_pix_y;//0-2160 U13

		U7 lcuw = (U7)(64 < fetch2dbk_fw_ptr_pic_width - lcu_x) ? (U7)64 : (U7)(fetch2dbk_fw_ptr_pic_width - lcu_x);//log2_max_cuwh = 6// 64 U7
		U7 lcuh = (U7)(64 < fetch2dbk_fw_ptr_pic_height - lcu_y) ? (U7)64 : (U7)(fetch2dbk_fw_ptr_pic_height - lcu_y);//log2_max_cuwh = 6//64 U7

		U11 mb_x_begin = lcu_x >> 2;//U11
		U11 mbxbegin = lcu_x > 0 ? U11(mb_x_begin - 1) : mb_x_begin;//U11

		U11 mb_x_end = (lcu_x + lcuw) >> 2;//(3840+64)/4=976// U11
		U11 mbxend = (lcu_x + lcuw == fetch2dbk_fw_ptr_pic_width) ? mb_x_end : U11(mb_x_end - 1);// U11

		U11 mb_y_begin = lcu_y >> 2;//U11
		U11 mbybegin = lcu_y > 0 ? U11(mb_y_begin - 1) : mb_y_begin;////U11

		U11 mb_y_end = (lcu_y + lcuh) >> 2;//U11
		U11 mbyend = (lcu_y + lcuh == fetch2dbk_fw_ptr_pic_height) ? mb_y_end : U11(mb_y_end - 1);//U11

		U13 lcu_pos = (lcu_y >> 6) * ((fetch2dbk_fw_ptr_pic_width + 63) >> 6) + (lcu_x >> 6);//(2160/64)*(3840/64)+(3840/64)=2085//12 U13
		U6 lcu_stride = (fetch2dbk_fw_ptr_pic_width + 63) >> 6;// (3840+63/64)=60//12-6=6//U6

    //////////////////////////////////////////////
	static int line_ptr_above_line_idx, line_ptr_curr_ctu_idx;
	u8 scan_type = (u8)0;
	static int x_ctu, y_ctu;// , x_last_ctu_core, y_last_ctu_core;
	int x_last_ctu = x_ctu;
	int y_last_ctu = y_ctu;
	int x_cur_ctu = fetch_output_dbk_pix_x;// pix_x;
	int y_cur_ctu = fetch_output_dbk_pix_y;// fetch_output_ptr->pix_y;
	int bu_idx = 0;
	int curr_ctu_idx_map = 0;

	if (x_cur_ctu == 0 && y_cur_ctu == 0) {
		scan_type = ((u8)0);
	}
	else if (x_cur_ctu > x_last_ctu && y_cur_ctu == y_last_ctu) {
		scan_type = ((u8)1);
	}
	else if (x_cur_ctu <= x_last_ctu && y_cur_ctu > y_last_ctu) {//change line
		scan_type = ((u8)2);
	}
	else {
		scan_type = ((u8)3);
	}

	// A.Current ctu is the first one
	if (((int)scan_type) == 0) {
		line_ptr_above_line_idx = 0;
		line_ptr_curr_ctu_idx = 0;
	}
	else
	{
		// B. Current ctu is on the last ctu's right
		if (((int)scan_type) == 1) {
			// x_scu = x_last_ctu >> 2;
			// y_scu = y_last_ctu >> 2;
			bu_idx = 1 - line_ptr_above_line_idx;
			curr_ctu_idx_map = line_ptr_curr_ctu_idx;
			line_ptr_curr_ctu_idx = 1 - curr_ctu_idx_map;
		}
		else
		{
			// C. Last ctu is the last ctu in a row
			if (((int)scan_type) == 2) {
				bu_idx = 1 - line_ptr_above_line_idx;
				curr_ctu_idx_map = line_ptr_curr_ctu_idx;
				// 2. Switch current buffer idx
				line_ptr_curr_ctu_idx = 1 - curr_ctu_idx_map;
				// 3. Init inner_map info for the current ctu
				line_ptr_above_line_idx = 1 - line_ptr_above_line_idx;
			}
		}
	}
	x_ctu = x_cur_ctu;
	y_ctu = y_cur_ctu;

	dbk_out_para_set->curr_ctu_idx = line_ptr_curr_ctu_idx;

	#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_ver0 complete dim=1 //1
	#pragma HLS ARRAY_PARTITION variable=ppbEdgeFilter_ctu_hor1 complete dim=2 //2

	ENC_ME_NEB_INFO inner_nebs_md_chroma_tmp[2][16][16];

	Data_Prepare_2(fetch2dbk_fw_ptr_pic_width_in_lcu, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, fetch2md_fw_ptr_pic_width_in_scu, lcu_x,
		lcu_y,
		//me_line_map_ptr_line_map_split, 
		dbk_y, dbk_u, dbk_v, dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v,
		dbk_ver_y, dbk_ver_u, dbk_ver_v,
		lcu_pos, lcu_stride, foursix_buffer, U3x30_buffer, V3x30_buffer, sixfour_buffer,
		U30x3_buffer, V30x3_buffer, fourfour_buffer, U3x3_buffer, V3x3_buffer, ppbEdgeFilter_ctu_ver0, ppbEdgeFilter_ctu_hor1, cur_ctu_cu_mode,
		inner_nebs_md_chroma, inner_nebs_sao, inner_nebs_md_chroma_tmp, dbk_out_para_set->map_scu_inner, me_line_map_ptr_DBK_map_scu_inner);

		pel ybuffer[8][8];
		#pragma HLS ARRAY_PARTITION variable=ybuffer complete dim=0
		//
		pel ubuffer[6][6];
		#pragma HLS ARRAY_PARTITION variable=ubuffer complete dim=0
		//
		pel vbuffer[6][6];
		#pragma HLS ARRAY_PARTITION variable=vbuffer complete dim=0
		//
		U7 filter_mb_y;//0-64//U7
		U7 filter_mb_x;
		U6 filter_mb_uy;
		U6 filter_mb_ux;
		U1 skipFilteringFlag;
		U8 QPs, cQPus, cQPvs;

		for (mb_y = mbybegin; mb_y < mbyend; mb_y++)
		{
#pragma HLS PIPELINE
#pragma HLS loop_tripcount max=16
			for (mb_x = mbxbegin; mb_x < mbxend; mb_x++)
			{
#pragma HLS PIPELINE
				///////////////////////////Ver////////////////////////////////////
#pragma HLS loop_tripcount max=8
/////////////////////////Ver////////////////////////////////////
				if (mb_x % 2 == 0 || mb_x == mbxend - 1)
				{
					U2 EdgeCondition = use_deblock ? EdgeContion_prepare_ver0(mb_y, mb_x, mbxbegin, mbybegin, ppbEdgeFilter_ctu_ver0,
						ppbEdgeFilter_ctu_buf1_ver0, ppbEdgeFilter_ctu_buf2_ver0) : (U2)0;
					//#pragma HLS DATAFLOW
					//Buffer_ver_prepare(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, md_out_para_set_pic_width, md_out_para_set_pic_height, ybuffer, ubuffer, vbuffer,
					//	dbk_ver_y, dbk_ver_u, dbk_ver_v, dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
					Buffer_ver_filter_mb(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height);

					Buffer_ver_prepare_y(filter_mb_y, filter_mb_x, mb_y, mb_x, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, ybuffer,
						dbk_ver_y, dbkout_y, dbk_mid_y);
					Buffer_ver_prepare_uv(filter_mb_uy, filter_mb_ux, mb_y, mb_x, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, ubuffer, vbuffer,
						dbk_ver_u, dbk_ver_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
#if !SIMPLE_MD

					if (EdgeCondition) {
						// DeblockMb_avs2_ver0(fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
						// 	filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, 0, mbxbegin, mbybegin, me_line_map_2dbk_new_map_scu_above_line, me_line_map_ptr_map_scu_inner, me_line_map_2dbk_min_scu_x, me_line_map_2dbk_min_scu_y, line_ptr_curr_ctu_idx,
						// 	line_ptr_above_line_idx, inner_nebs2dbk, fetch2dbk_new_above_nebs_line, md_out_para_set_pic_width, md_out_para_set_pic_height, ybuffer, ubuffer, vbuffer);
						skip_Mb(fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
							filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, 0, mbxbegin, mbybegin, me_line_map_ptr_DBK_new_map_scu_above_line, me_line_map_ptr_DBK_map_scu_inner, me_line_map_ptr_DBK_min_scu_x, me_line_map_ptr_DBK_min_scu_y, line_ptr_curr_ctu_idx,
							line_ptr_above_line_idx, inner_nebs_md_chroma_tmp, new_above_nebs_line_DBK, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height,
							skipFilteringFlag, QPs, cQPus, cQPvs, fetch2dbk_fw_ptr_qp_offset_bit_depth
#if ENABLE_BFRAME
							//, refp
							, fetch2dbk_fw_ptr_ref_ptr_dbk
#endif				 
						);
						Filter_YUV(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux,
							mb_y, mb_x, 0, skipFilteringFlag,
							QPs, cQPus, cQPvs,
							fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height,
							ybuffer, ubuffer, vbuffer, fetch2dbk_fw_ptr_bit_depth, fetch2dbk_fw_ptr_qp_offset_bit_depth);
					}

#endif
					Buffer_ver(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, ybuffer, ubuffer, vbuffer,
						dbk_ver_y, dbk_ver_u, dbk_ver_v, dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
				}

			}
		}

		//Copy_UV_verandhor_1(lcu_y, lcu_pos, lcu_stride, U30x3_buffer, V30x3_buffer, U3x3_buffer, V3x3_buffer,
		//	dbk_u, dbk_v, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
		//UV
		//printf("-------------Hor------------\n");

		for (mb_x = mbxbegin; mb_x < mbxend; mb_x++) {
#pragma HLS PIPELINE
#pragma HLS loop_tripcount max=16
			for (mb_y = mbybegin; mb_y < mbyend; mb_y++) {
#pragma HLS PIPELINE
#pragma HLS loop_tripcount max=8
				if (mb_y % 2 == 0 || mb_y == mbyend - 1)
				{
					U2 EdgeCondition = use_deblock? EdgeContion_prepare_hor1(mb_y, mb_x, mbxbegin, mbybegin, ppbEdgeFilter_ctu_hor1,
						ppbEdgeFilter_ctu_buf1_hor1, ppbEdgeFilter_ctu_buf2_hor1):(U2)0;
					//#pragma HLS DATAFLOW
					//Buffer_hor_prepare(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, md_out_para_set_pic_width, md_out_para_set_pic_height, ybuffer, ubuffer, vbuffer,
					//	dbk_y, dbk_u, dbk_v,
					//	dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
					Buffer_hor_prepare_y(filter_mb_y, filter_mb_x, mb_y, mb_x, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, ybuffer,
							dbk_y, dbkout_y, dbk_mid_y);
					Buffer_hor_prepare_uv(filter_mb_uy, filter_mb_ux, mb_y, mb_x, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, ubuffer, vbuffer,
								dbk_u, dbk_v,
								dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
#if !SIMPLE_MD
					if (EdgeCondition){
						//DeblockMb_avs2_hor1(fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
						//	filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, 1, mbxbegin, mbybegin, me_line_map_2dbk_new_map_scu_above_line, me_line_map_ptr_map_scu_inner, me_line_map_2dbk_min_scu_x, me_line_map_2dbk_min_scu_y, line_ptr_curr_ctu_idx,
						//	line_ptr_above_line_idx, inner_nebs2dbk, fetch2dbk_new_above_nebs_line, md_out_para_set_pic_width, md_out_para_set_pic_height, ybuffer, ubuffer, vbuffer);
						skip_Mb_hor(fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
						  	filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, 1, mbxbegin, mbybegin, me_line_map_ptr_DBK_new_map_scu_above_line, me_line_map_ptr_DBK_map_scu_inner, me_line_map_ptr_DBK_min_scu_x, me_line_map_ptr_DBK_min_scu_y, line_ptr_curr_ctu_idx,
						  	line_ptr_above_line_idx, inner_nebs_md_chroma_tmp, new_above_nebs_line_DBK, fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, skipFilteringFlag, QPs, cQPus, cQPvs, fetch2dbk_fw_ptr_qp_offset_bit_depth
#if ENABLE_BFRAME
						//	, refp
                , fetch2dbk_fw_ptr_ref_ptr_dbk
#endif
							);
						Filter_YUV_hor(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, 1, skipFilteringFlag, QPs, cQPus, cQPvs,
						  	fetch2dbk_fw_ptr_pic_width, fetch2dbk_fw_ptr_pic_height, ybuffer, ubuffer, vbuffer, fetch2dbk_fw_ptr_bit_depth, fetch2dbk_fw_ptr_qp_offset_bit_depth);
					}
#endif
					Buffer_hor(filter_mb_y, filter_mb_x, filter_mb_uy, filter_mb_ux, mb_y, mb_x, ybuffer, ubuffer, vbuffer,
						dbk_y, dbk_u, dbk_v,
						dbkout_y, dbk_mid_y, dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v);
				}

			}
		}

		CopyData_after_hor_2(lcu_x, lcu_pos, lcu_stride, U3x3_buffer, V3x3_buffer, U3x30_buffer, V3x30_buffer,
			dbk_u, dbk_v,
			dbkout_u, dbk_mid_u, dbkout_v, dbk_mid_v,
			ppbEdgeFilter_ctu_buf2_ver0,
			ppbEdgeFilter_ctu_ver0, ppbEdgeFilter_ctu_buf1_ver0, ppbEdgeFilter_ctu_buf2_hor1,
			ppbEdgeFilter_ctu_hor1, ppbEdgeFilter_ctu_buf1_hor1);
		//dbk_run over


#ifdef COLLECT_DBK_DATA
//		//if (dbk_lcu_cnt < LCU_TEST_NUM) {
//			static FILE* fp_out = fopen(OUTPUT_GOLDEN, "wb");
//			if (!fp_out) { printf("Open %s failed!\n", OUTPUT_GOLDEN); }
//			else {
//				DBK_PORT(EXTRACT_ARGS(PORT_DECL)).write_bin(fp_out);
//			}
//		//}
//		//++dbk_lcu_cnt;

   dbk_output_tv(dbkout_y, dbkout_u, dbkout_v,
			//inner_nebs_sao,
			dbk_out_para_set, srcc_for_aec);
#endif

}

#define DEBUG_DBK 0

#if DEBUG_DBK
#define USE_FACADE_DBK 1
#ifdef WIN32
static FilePrinter fp = FilePrinter("D:\\working\\bframe\\data\\dbk", -1, -1);
static FileReader fr = FileReader("D:\\working\\bframe\\data\\dbk", -1);
static int db_cnt = 0;
static int dbk_num = 2;
#else
static FilePrinter fp = FilePrinter("/home/zhuxz/avs3ip/bframe/bframe/data", -1, -1);
#endif
#endif

#if USE_FACADE_DBK
void dbk_run_facade(
	S32 fetch2dbk_fw_ptr_pic_width, S32 fetch2dbk_fw_ptr_pic_height, S32 fetch2dbk_fw_ptr_pic_width_in_lcu, S32 fetch2dbk_fw_ptr_pic_height_in_lcu,
	S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cr,
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39],
	pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39],
	U12 fetch_output_dbk_pix_x, U12 fetch_output_dbk_pix_y,
	U11 me_line_map_ptr_DBK_min_scu_x, U11 me_line_map_ptr_DBK_min_scu_y,
	//U6 me_line_map_ptr_line_map_split[5][7][256],
	U32 me_line_map_ptr_DBK_map_scu_inner[2][16][16],
	ENC_ME_NEB_INFO new_above_nebs_line_DBK[48], U32 me_line_map_ptr_DBK_new_map_scu_above_line[48],
	ENC_ME_NEB_INFO inner_nebs_md_chroma[2][16][16], ENC_ME_NEB_INFO inner_nebs_sao[2][16][16],
	DBK_OUT_PARA_SET* dbk_out_para_set, U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U4 fetch2dbk_fw_ptr_bit_depth, U32 fetch2dbk_fw_ptr_qp_offset_bit_depth
#if ENABLE_BFRAME
	, COM_REFP refp[MAX_NUM_REF_PICS][REFP_NUM]
#endif
)
{
	S32 md_out_para_set_pic_width_in; S32 md_out_para_set_pic_height_in; S32 fetch2dbk_fw_ptr_pic_width_in_lcu_in; S32 fetch2dbk_fw_ptr_pic_height_in_lcu_in;
	S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cb_in; S8 fetch2dbk_fw_ptr_chroma_quant_param_delta_cr_in;
	static pel dbk_y_in[71][71]; static pel dbk_u_in[39][39]; static pel dbk_v_in[39][39];
	static pel dbkout_y_in[71][71]; static pel dbkout_u_in[39][39]; static pel dbkout_v_in[39][39];
	U12 md_out_para_set_pix_x_in; U12 md_out_para_set_pix_y_in;
	U11 me_line_map_2dbk_min_scu_x_in; U11 me_line_map_2dbk_min_scu_y_in;
	static U32 me_line_map_ptr_map_scu_inner_in[2][16][16];
	static ENC_ME_NEB_INFO fetch2dbk_new_above_nebs_line_in[48];
	static U32 me_line_map_2dbk_new_map_scu_above_line_in[48];
	static ENC_ME_NEB_INFO inner_nebs2dbk_in[2][16][16];
	static ENC_ME_NEB_INFO inner_nebs_sao_in[2][16][16];
	DBK_OUT_PARA_SET dbk_out_para_set_in;
	static U32 cur_ctu_cu_mode_in[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	U4 bit_depth_in; U32 qp_offset_bit_depth_in;
#if ENABLE_BFRAME
	static COM_REFP refp_in[MAX_NUM_REF_PICS][REFP_NUM];
#endif


#if DEBUG_DBK && 0
	if ((fetch_output_dbk_pix_x == 64 && fetch_output_dbk_pix_y == 832 && pic_cnt == 10) || (fetch_output_dbk_pix_x == 128 && fetch_output_dbk_pix_y == 832 && pic_cnt == 10))
	{
		printf("[%s][x=%d,y=%d,pic_cnt=%d,db_cnt=%d]\n", __FUNCTION__, fetch_output_dbk_pix_x, fetch_output_dbk_pix_y, pic_cnt, db_cnt);
		fp.printAtribute("md_out_para_set_pic_width_" + std::to_string(db_cnt), sizeof(S32), &fetch2dbk_fw_ptr_pic_width, -1);
		fp.printAtribute("md_out_para_set_pic_height_" + std::to_string(db_cnt), sizeof(S32), &fetch2dbk_fw_ptr_pic_height, -1);
		fp.printAtribute("fetch2dbk_fw_ptr_pic_width_in_lcu_" + std::to_string(db_cnt), sizeof(S32), &fetch2dbk_fw_ptr_pic_width_in_lcu, -1);
		fp.printAtribute("fetch2dbk_fw_ptr_pic_height_in_lcu_" + std::to_string(db_cnt), sizeof(S32), &fetch2dbk_fw_ptr_pic_height_in_lcu, -1);
		fp.printAtribute("fetch2dbk_fw_ptr_chroma_quant_param_delta_cb_" + std::to_string(db_cnt), sizeof(S8), &fetch2dbk_fw_ptr_chroma_quant_param_delta_cb, -1);
		fp.printAtribute("fetch2dbk_fw_ptr_chroma_quant_param_delta_cr_" + std::to_string(db_cnt), sizeof(S8), &fetch2dbk_fw_ptr_chroma_quant_param_delta_cr, -1);
		fp.printAtribute("dbk_y_" + std::to_string(db_cnt), sizeof(pel) * 71 * 71, dbk_y, -1);
		fp.printAtribute("dbk_u_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbk_u, -1);
		fp.printAtribute("dbk_v_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbk_v, -1);
		fp.printAtribute("dbkout_y_" + std::to_string(db_cnt), sizeof(pel) * 71 * 71, dbkout_y, -1);
		fp.printAtribute("dbkout_u_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbkout_u, -1);
		fp.printAtribute("dbkout_v_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbkout_v, -1);
		fp.printAtribute("md_out_para_set_pix_x_" + std::to_string(db_cnt), sizeof(U12), &fetch_output_dbk_pix_x, -1);
		fp.printAtribute("md_out_para_set_pix_y_" + std::to_string(db_cnt), sizeof(U12), &fetch_output_dbk_pix_y, -1);
		fp.printAtribute("me_line_map_2dbk_min_scu_x_" + std::to_string(db_cnt), sizeof(U11), &me_line_map_ptr_DBK_min_scu_x, -1);
		fp.printAtribute("me_line_map_2dbk_min_scu_y_" + std::to_string(db_cnt), sizeof(S32), &me_line_map_ptr_DBK_min_scu_y, -1);
		fp.printAtribute("me_line_map_ptr_map_scu_inner_" + std::to_string(db_cnt), sizeof(U32) * 2 * 16 * 16, me_line_map_ptr_DBK_map_scu_inner, -1);
		fp.printAtribute("fetch2dbk_new_above_nebs_line_" + std::to_string(db_cnt), sizeof(ENC_ME_NEB_INFO) * 48, new_above_nebs_line_DBK, -1);
		fp.printAtribute("me_line_map_2dbk_new_map_scu_above_line_" + std::to_string(db_cnt), sizeof(U32) * 48, me_line_map_ptr_DBK_new_map_scu_above_line, -1);
		fp.printAtribute("dbk_out_para_set_" + std::to_string(db_cnt), sizeof(DBK_OUT_PARA_SET), dbk_out_para_set, -1);
		fp.printAtribute("cur_ctu_cu_mode_" + std::to_string(db_cnt), sizeof(U32) * SCU_IN_LCU_NUMS_LEN * SCU_IN_LCU_NUMS_LEN, cur_ctu_cu_mode, -1);
		fp.printAtribute("bit_depth_" + std::to_string(db_cnt), sizeof(U4), &fetch2dbk_fw_ptr_bit_depth, -1);
		fp.printAtribute("qp_offset_bit_depth_" + std::to_string(db_cnt), sizeof(U32), &fetch2dbk_fw_ptr_qp_offset_bit_depth, -1);
		fp.printAtribute("refp_" + std::to_string(db_cnt), sizeof(COM_REFP) * MAX_NUM_REF_PICS * REFP_NUM, refp, -1);
		db_cnt++;
	}
#endif

	while (db_cnt < dbk_num)
	{
		fr.readAtribute("md_out_para_set_pic_width_" + std::to_string(db_cnt), sizeof(S32), &md_out_para_set_pic_width_in);
		fr.readAtribute("md_out_para_set_pic_height_" + std::to_string(db_cnt), sizeof(S32), &md_out_para_set_pic_height_in);
		fr.readAtribute("fetch2dbk_fw_ptr_pic_width_in_lcu_" + std::to_string(db_cnt), sizeof(S32), &fetch2dbk_fw_ptr_pic_width_in_lcu_in);
		fr.readAtribute("fetch2dbk_fw_ptr_pic_height_in_lcu_" + std::to_string(db_cnt), sizeof(S32), &fetch2dbk_fw_ptr_pic_height_in_lcu_in);
		fr.readAtribute("fetch2dbk_fw_ptr_chroma_quant_param_delta_cb_" + std::to_string(db_cnt), sizeof(S8), &fetch2dbk_fw_ptr_chroma_quant_param_delta_cb_in);
		fr.readAtribute("fetch2dbk_fw_ptr_chroma_quant_param_delta_cr_" + std::to_string(db_cnt), sizeof(S8), &fetch2dbk_fw_ptr_chroma_quant_param_delta_cr_in);
		fr.readAtribute("dbk_y_" + std::to_string(db_cnt), sizeof(pel) * 71 * 71, dbk_y_in);
		fr.readAtribute("dbk_u_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbk_u_in);
		fr.readAtribute("dbk_v_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbk_v_in);
		fr.readAtribute("dbkout_y_" + std::to_string(db_cnt), sizeof(pel) * 71 * 71, dbkout_y_in);
		fr.readAtribute("dbkout_u_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbkout_u_in);
		fr.readAtribute("dbkout_v_" + std::to_string(db_cnt), sizeof(pel) * 39 * 39, dbkout_v_in);
		fr.readAtribute("md_out_para_set_pix_x_" + std::to_string(db_cnt), sizeof(U12), &md_out_para_set_pix_x_in);
		fr.readAtribute("md_out_para_set_pix_y_" + std::to_string(db_cnt), sizeof(U12), &md_out_para_set_pix_y_in);
		fr.readAtribute("me_line_map_2dbk_min_scu_x_" + std::to_string(db_cnt), sizeof(U11), &me_line_map_2dbk_min_scu_x_in);
		fr.readAtribute("me_line_map_2dbk_min_scu_y_" + std::to_string(db_cnt), sizeof(S32), &me_line_map_2dbk_min_scu_y_in);
		fr.readAtribute("me_line_map_ptr_map_scu_inner_" + std::to_string(db_cnt), sizeof(U32) * 2 * 16 * 16, me_line_map_ptr_map_scu_inner_in);
		fr.readAtribute("fetch2dbk_new_above_nebs_line_" + std::to_string(db_cnt), sizeof(ENC_ME_NEB_INFO) * 48, fetch2dbk_new_above_nebs_line_in);
		fr.readAtribute("me_line_map_2dbk_new_map_scu_above_line_" + std::to_string(db_cnt), sizeof(U32) * 48, me_line_map_2dbk_new_map_scu_above_line_in);
		fr.readAtribute("dbk_out_para_set_" + std::to_string(db_cnt), sizeof(DBK_OUT_PARA_SET), &dbk_out_para_set_in);
		fr.readAtribute("cur_ctu_cu_mode_" + std::to_string(db_cnt), sizeof(U32) * SCU_IN_LCU_NUMS_LEN * SCU_IN_LCU_NUMS_LEN, cur_ctu_cu_mode_in);
		fr.readAtribute("bit_depth_" + std::to_string(db_cnt), sizeof(U4), &bit_depth_in);
		fr.readAtribute("qp_offset_bit_depth_" + std::to_string(db_cnt), sizeof(U32), &qp_offset_bit_depth_in);
		fr.readAtribute("refp_" + std::to_string(db_cnt), sizeof(COM_REFP) * MAX_NUM_REF_PICS * REFP_NUM, refp_in);
		printf("[%s][x=%d,y=%d,pic_cnt=%d,db_cnt=%d]\n", __FUNCTION__, md_out_para_set_pix_x_in, md_out_para_set_pix_y_in, pic_cnt, db_cnt);

		for (int i = 0; i < 64; i++)
		{
			for (int j = 0; j < 64; j++)
			{
				printf("%d", dbk_y_in[i + 6][j + 6]);
				if (j != 63)
					printf(",");
			}
			printf("\n");
		}

		dbk_run(
			md_out_para_set_pic_width_in, md_out_para_set_pic_height_in, fetch2dbk_fw_ptr_pic_width_in_lcu_in, fetch2dbk_fw_ptr_pic_height_in_lcu_in,
			fetch2dbk_fw_ptr_chroma_quant_param_delta_cb_in, fetch2dbk_fw_ptr_chroma_quant_param_delta_cr_in,
			dbk_y_in, dbk_u_in, dbk_v_in,
			dbkout_y_in, dbkout_u_in, dbkout_v_in,
			md_out_para_set_pix_x_in, md_out_para_set_pix_y_in,
			me_line_map_2dbk_min_scu_x_in, me_line_map_2dbk_min_scu_y_in,
			me_line_map_ptr_map_scu_inner_in,
			fetch2dbk_new_above_nebs_line_in, me_line_map_2dbk_new_map_scu_above_line_in,
			inner_nebs2dbk_in, inner_nebs_sao_in,
			&dbk_out_para_set_in, cur_ctu_cu_mode_in, bit_depth_in, qp_offset_bit_depth_in
#if ENABLE_BFRAME
			, refp_in);
#endif
		printf("[%s OUT][x=%d,y=%d,pic_cnt=%d,db_cnt=%d]\n", __FUNCTION__, md_out_para_set_pix_x_in, md_out_para_set_pix_y_in, pic_cnt, db_cnt);
		db_cnt++;
	}
}
#endif
