#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "sao.h"

#ifndef __SYNTHESIS__
#include "rfc_cpr.hpp"
#endif

typedef long long SAO_BIG_SIZE;
#define SAO_MAX_BIG  MAX_S64

#ifndef __SYNTHESIS__
#if COLLECT_SAO_DATA
FILE* open_file(char* filename, char* mode)
{
    FILE* f;
    f = fopen(filename, mode);
    if (!f)
    {
        perror(filename);
        exit(0);
    }
    return f;
}

FILE* sao_input_dbk_out_para_set_min_scu_x;
FILE* sao_input_dbk_out_para_set_min_scu_y;
FILE* sao_input_me_line_map_ptr_map_cu_mode_inner;
FILE* sao_input_dbk_out_para_set_curr_ctu_idx;
FILE* sao_input_dbk_out_para_set_map_scu_inner;
FILE* sao_input_inner_nebs2sao;
FILE* sao_input_dbk_out_para_set_pic_width;
FILE* sao_input_dbk_out_para_set_pic_height;
FILE* sao_input_dbk_out_para_set_pic_width_in_lcu;
FILE* sao_input_dbk_out_para_set_pic_height_in_lcu;
FILE* sao_input_dbk_out_para_set_x_pel;
FILE* sao_input_dbk_out_para_set_y_pel;
FILE* sao_input_dbkout_y;
FILE* sao_input_dbkout_u;
FILE* sao_input_dbkout_v;
//FILE* sao_input_cur_saoParam;
FILE* sao_input_Orig_Lcu_ptr_y_org_sao;
FILE* sao_input_Orig_Lcu_ptr_u_org_sao;
FILE* sao_input_Orig_Lcu_ptr_v_org_sao;
FILE* sao_input_fetch2sao_fw_ptr;
FILE* sao_input_qp_offset;

FILE* sao_output_cur_saoParam;
FILE* sao_output_ctx_map_scu;
FILE* sao_output_ctx_map_mv;
FILE* sao_output_ctx_map_refi;
FILE* sao_output_ctx_map_cu_mode;
FILE* sao_output_y_rec;
FILE* sao_output_u_rec;
FILE* sao_output_v_rec;
FILE* sao_output_AEC_DATA_input;

static int sao_cu_cnt = 0;

void sao_save_indata(u32* ctx_map_scu, s16* ctx_map_mv, s8* ctx_map_refi, u32* ctx_map_cu_mode,
    U11* dbk_out_para_set_min_scu_x, U11* dbk_out_para_set_min_scu_y, U32 me_line_map_ptr_map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
    U1* dbk_out_para_set_curr_ctu_idx, U32 dbk_out_para_set_map_scu_inner[2][16][16],
    ENC_ME_NEB_INFO inner_nebs2sao[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
    U13* dbk_out_para_set_pic_width, U13* dbk_out_para_set_pic_height, U6* dbk_out_para_set_pic_width_in_lcu, U6* dbk_out_para_set_pic_height_in_lcu,
    U12* dbk_out_para_set_x_pel, U12* dbk_out_para_set_y_pel, pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39], 
    pel/*U8*/ Orig_Lcu_ptr_y_org_sao[64][64], pel/*U8*/ Orig_Lcu_ptr_u_org_sao[32][32], pel/*U8*/ Orig_Lcu_ptr_v_org_sao[32][32], 
	pel* y_rec, pel* u_rec, pel* v_rec, FETCH2SAO_FW* fetch2sao_fw_ptr, SAO2AEC* AEC_DATA_input, U32* qp_offset)
{
    fwrite(dbk_out_para_set_min_scu_x, sizeof(U11), 1, sao_input_dbk_out_para_set_min_scu_x);
    fwrite(dbk_out_para_set_min_scu_y, sizeof(U11), 1, sao_input_dbk_out_para_set_min_scu_y);
    fwrite(me_line_map_ptr_map_cu_mode_inner, 2 * 16 * 16 * sizeof(U32), 1, sao_input_me_line_map_ptr_map_cu_mode_inner);
    fwrite(dbk_out_para_set_curr_ctu_idx, sizeof(U1), 1, sao_input_dbk_out_para_set_curr_ctu_idx);
    fwrite(dbk_out_para_set_map_scu_inner, 2 * 16 * 16 * sizeof(U32), 1, sao_input_dbk_out_para_set_map_scu_inner);
    fwrite(inner_nebs2sao, 2 * 16 * 16 * sizeof(ENC_ME_NEB_INFO), 1, sao_input_inner_nebs2sao);
    fwrite(dbk_out_para_set_pic_width, sizeof(U13), 1, sao_input_dbk_out_para_set_pic_width);
    fwrite(dbk_out_para_set_pic_height, sizeof(U13), 1, sao_input_dbk_out_para_set_pic_height);
    fwrite(dbk_out_para_set_pic_width_in_lcu, sizeof(U6), 1, sao_input_dbk_out_para_set_pic_width_in_lcu);
    fwrite(dbk_out_para_set_pic_height_in_lcu, sizeof(U6), 1, sao_input_dbk_out_para_set_pic_height_in_lcu);
    fwrite(dbk_out_para_set_x_pel, sizeof(U12), 1, sao_input_dbk_out_para_set_x_pel);
    fwrite(dbk_out_para_set_y_pel, sizeof(U12), 1, sao_input_dbk_out_para_set_y_pel);
    fwrite(dbkout_y, sizeof(pel), 71 * 71, sao_input_dbkout_y);
    fwrite(dbkout_u, sizeof(pel), 39 * 39, sao_input_dbkout_u);
    fwrite(dbkout_v, sizeof(pel), 39 * 39, sao_input_dbkout_v);
    fwrite(Orig_Lcu_ptr_y_org_sao, sizeof(pel), 64 * 64, sao_input_Orig_Lcu_ptr_y_org_sao);
    fwrite(Orig_Lcu_ptr_u_org_sao, sizeof(pel), 32 * 32, sao_input_Orig_Lcu_ptr_u_org_sao);
    fwrite(Orig_Lcu_ptr_v_org_sao, sizeof(pel), 32 * 32, sao_input_Orig_Lcu_ptr_v_org_sao);
    fwrite(fetch2sao_fw_ptr, sizeof(FETCH2SAO_FW), 1, sao_input_fetch2sao_fw_ptr);
    fwrite(qp_offset, sizeof(U32), 1, sao_input_qp_offset);
}
void sao_save_outdata(SAOBlkParam cur_saoParam[3], u32* ctx_map_scu, s16* ctx_map_mv, s8* ctx_map_refi,
    u32* ctx_map_cu_mode, pel* y_rec, pel* u_rec, pel* v_rec, SAO2AEC* AEC_DATA_input)
{
    fwrite(cur_saoParam, 3 * sizeof(SAOBlkParam), 1, sao_output_cur_saoParam);
    fwrite(ctx_map_scu, sizeof(u32), 6240, sao_output_ctx_map_scu);
    fwrite(ctx_map_mv, sizeof(s16), 6240 * 4, sao_output_ctx_map_mv);
    fwrite(ctx_map_refi, sizeof(s8), 6240 * 2, sao_output_ctx_map_refi);
    fwrite(ctx_map_cu_mode, sizeof(u32), 6240, sao_output_ctx_map_cu_mode);
    fwrite(y_rec - ((704 + 1) * 144), sizeof(pel), 371712, sao_output_y_rec);
    fwrite(u_rec - ((352 + 1) * 72), sizeof(pel), 92928, sao_output_u_rec);
    fwrite(v_rec - ((352 + 1) * 72), sizeof(pel), 92928, sao_output_v_rec);
    fwrite(AEC_DATA_input, sizeof(SAO2AEC), 1, sao_output_AEC_DATA_input);
}
#endif
#endif //__SYNTHESIS__

//extern CTU_Level_Parameters fetch_output;
//extern FRAME_LEVEL_FW frame_level_fw;
//extern ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
//extern ENC_ME_NEB_INFO above_nebs_line[2][MAX_WIDTH >> MIN_CU_LOG2];
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



static void dbk_2_sao_y(//u8 dbk_lcu_array_y[68][68],  u8 dbkbuffer_y[68][68], int pix_x, int pix_y, int rec_Stride, int lcu_pos, int LCU_row_num, int LCU_col_num, 
                        //u8 rctngl_hor_dbk_y[MAX_NUM_LCU_ROW][2][68],    u8 rctngl_ver_dbk_y[68][2],  u8 small_sq_dbk_y[MAX_NUM_LCU_ROW + 1][2][2], 
                        int lcu_pos, int LCU_col_num,
                        pel dbkbuffer_y[68][68],                           
                        //u8 rctngl_hor_dbk_y[MAX_NUM_LCU_ROW + 1][2][68],  u8 rctngl_ver_dbk_y[68][2],
                        pel temp_sao_lcu_array_y[70][70],                  pel temp_sao_lcu_array_y_dupli1[70][70])
{
#pragma HLS ARRAY_PARTITION variable=dbkbuffer_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_y_dupli1 complete dim=2

    static pel rctngl_ver_dbk_y[68][2];
    static pel rctngl_hor_dbk_y[MAX_NUM_LCU_ROW + 1][2][68];
#pragma HLS ARRAY_PARTITION variable=rctngl_ver_dbk_y complete dim=0
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_y complete dim=2
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_y complete dim=3
    //int rec_offset_y = pix_y * rec_Stride + pix_x;
    int i;
    int j;
    
    int rctngl_0idx = lcu_pos % (LCU_col_num + 1);
    int rctngl_1idx = (lcu_pos + 1) % (LCU_col_num + 1);
// For Y channel, DBK & SAO share the same shifted pattern
    for (i = 0; i < 68; i++) 
    {
#pragma HLS pipeline
        for (j = 0; j < 68; j++) 
        {
#pragma HLS unroll
            //dbk_lcu_array_y[i][j]                     = dbkbuffer_y[i][j];
            temp_sao_lcu_array_y       [i + 2][j + 2] = dbkbuffer_y[i][j];
            temp_sao_lcu_array_y_dupli1[i + 2][j + 2] = dbkbuffer_y[i][j];
            //pY[(i - 4) * rec_Stride + (j - 4) + rec_offset_y] = dbkbuffer_y[i][j];
        }
    }
// Expand the current DBK output LCU window left by 2 pixels & up by 2 pixels, to deal with the data dependency of SAO filtering process
// If current LCU is at the edge of pic, adding padding pixels to fullfill shifted 66*66
// Generally
// Add paddings

    for (i = 0; i < 2; i++)
    {
#pragma HLS pipeline
        for (j = 0; j < 2; j++) 
        {
#pragma HLS unroll
            temp_sao_lcu_array_y       [i][j] = rctngl_hor_dbk_y[rctngl_0idx][i][j+62];
            temp_sao_lcu_array_y_dupli1[i][j] = rctngl_hor_dbk_y[rctngl_0idx][i][j+62];
        }
    }

    for (i = 0; i < 2; i++) 
    {
#pragma HLS pipeline
        for (j = 0; j < 68; j++) 
        {
#pragma HLS unroll
            temp_sao_lcu_array_y       [i][j+2] = rctngl_hor_dbk_y[rctngl_1idx][i][j];
            temp_sao_lcu_array_y_dupli1[i][j+2] = rctngl_hor_dbk_y[rctngl_1idx][i][j];

            rctngl_hor_dbk_y[rctngl_0idx][i][j] = dbkbuffer_y[i + 62][j];
        }
    }

    for (i = 0; i < 68; i++) // !: As we can see here, linebuffer_68x2 utilized area is acturally (2~66)x(0~2) = 64x2, NOT 68x2.
                             // Because Pic. Height is always k*64, where k belongs to INTEGER. There will NOT appear extra lines at the bottom of the Pic.
    {
#pragma HLS pipeline
        for (j = 0; j < 2; j++) 
        {
#pragma HLS unroll
            temp_sao_lcu_array_y       [i+2][j] = rctngl_ver_dbk_y[i][j];
            temp_sao_lcu_array_y_dupli1[i+2][j] = rctngl_ver_dbk_y[i][j];
			
			rctngl_ver_dbk_y[i][j] = dbkbuffer_y[i][j + 62];
        }
    }

// If the right bound LCU width > 60 or the bottom bound LCU height > 60, set the special buffers for right & bottom bound
// If the pic contains integral 64*64s
//    for (i = 0; i < 2; i++)
//    {
//        for (j = 0; j < 68; j++)
//        {
//#pragma HLS PIPELINE
//            rctngl_hor_dbk_y[lcu_pos % (LCU_col_num + 1)][i][j] = dbkbuffer_y[i + 62][j];
//        }
//    }
//    for (i = 0; i < 64; i++) 
//    {
//        for (j = 0; j < 2; j++) 
//        {
//#pragma HLS PIPELINE
//            rctngl_ver_dbk_y[i][j] = dbkbuffer_y[i][j + 62];
//        }
//    }
}

static void dbk_2_sao_u(//u8 dbk_lcu_array_u[36][36],          u8 dbkbuffer_u[36][36],
                        //int pix_x_chroma, int pix_y_chroma, int rec_Stride_chroma, int lcu_pos, int LCU_row_num, int LCU_col_num, 
                        //u8 rctngl_hor_dbk_u[MAX_NUM_LCU_ROW][2][36],  u8 rctngl_ver_dbk_u[36][2], u8 small_sq_dbk_u[MAX_NUM_LCU_ROW + 1][2][2], 
                        int lcu_pos, int LCU_col_num,
                        pel dbkbuffer_u[36][36],                           //u8 dbk_lcu_array_u[36][36],
                        pel temp_sao_lcu_array_u[38][38])
{
#pragma HLS ARRAY_PARTITION variable=dbkbuffer_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_u complete dim=2
    static pel rctngl_ver_dbk_u[34][4];
    static pel rctngl_hor_dbk_u[MAX_NUM_LCU_ROW + 1][4][34];
#pragma HLS ARRAY_PARTITION variable=rctngl_ver_dbk_u complete dim=0
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_u complete dim=2
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_u complete dim=3

    //int rec_offset_uv = pix_y_chroma * rec_Stride_chroma + pix_x_chroma;
    int i;
    int j;

    int rctngl_0idx = lcu_pos % (LCU_col_num + 1);
    int rctngl_1idx = (lcu_pos + 1) % (LCU_col_num + 1);
    for (i = 2; i < 36; i++) 
    {
#pragma HLS pipeline
        for (j = 2; j < 36; j++) 
        {
#pragma HLS unroll
          //dbk_lcu_array_u[i][j]              = dbkbuffer_u[i][j];
            temp_sao_lcu_array_u[i + 2][j + 2] = dbkbuffer_u[i][j];
        }
    }

    for (i = 0; i < 4; i++)
    {
#pragma HLS pipeline
        for (j = 0; j < 4; j++)
        {
#pragma HLS unroll
            temp_sao_lcu_array_u[i][j] = rctngl_hor_dbk_u[rctngl_0idx][i][j + 28];
        }
    }

    for (i = 0; i < 4; i++)
    {
#pragma HLS pipeline
        for (j = 0; j < 34; j++)
        {
#pragma HLS unroll
            temp_sao_lcu_array_u[i][j + 4] = rctngl_hor_dbk_u[rctngl_1idx][i][j];
            rctngl_hor_dbk_u[rctngl_0idx][i][j] = dbkbuffer_u[i + 30][j + 2];
        }
    }

    for (i = 0; i < 34; i++) 
    {
#pragma HLS pipeline
        for (j = 0; j < 4; j++) 
        {
#pragma HLS unroll
            temp_sao_lcu_array_u[i + 4][j] = rctngl_ver_dbk_u[i][j];
            rctngl_ver_dbk_u[i][j] = dbkbuffer_u[i + 2][j + 30];
        }
    }
//----------------------------------------------------------------------------------------------------------
    //for (i = 0; i < 4; i++)
    //{
    //    for (j = 0; j < 34; j++)
    //    {
    //        rctngl_hor_dbk_u[lcu_pos % (LCU_col_num + 1)][i][j] = dbkbuffer_u[i + 30][j + 2];
    //    }
    //}
}


static void dbk_2_sao_v(//u8 dbk_lcu_array_v[36][36],     u8 dbkbuffer_v[36][36],  int pix_x_chroma, int pix_y_chroma, int rec_Stride_chroma, int lcu_pos, int LCU_row_num, int LCU_col_num, 
                        //u8 rctngl_hor_dbk_v[MAX_NUM_LCU_ROW][2][36], u8 rctngl_ver_dbk_v[36][2], u8 small_sq_dbk_v[MAX_NUM_LCU_ROW + 1][2][2], 
                        int lcu_pos, int LCU_col_num,
                        pel dbkbuffer_v[36][36],                          //u8 dbk_lcu_array_v[36][36],
                        pel temp_sao_lcu_array_v[38][38])
{
#pragma HLS ARRAY_PARTITION variable=dbkbuffer_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_v complete dim=2
    static pel rctngl_ver_dbk_v[34][4];
    static pel rctngl_hor_dbk_v[MAX_NUM_LCU_ROW + 1][4][34];
#pragma HLS ARRAY_PARTITION variable=rctngl_ver_dbk_v complete dim=0
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_v complete dim=2
#pragma HLS ARRAY_PARTITION variable=rctngl_hor_dbk_v complete dim=3

    //int rec_offset_uv = pix_y_chroma * rec_Stride_chroma + pix_x_chroma;
    int i;
    int j;

    int rctngl_0idx = lcu_pos % (LCU_col_num + 1);
    int rctngl_1idx = (lcu_pos + 1) % (LCU_col_num + 1);
    for (i = 2; i < 36; i++)
    {
#pragma HLS pipeline
        for (j = 2; j < 36; j++)
        {
#pragma HLS PIPELINE
            temp_sao_lcu_array_v[i + 2][j + 2] = dbkbuffer_v[i][j];
        }
    }

    for (i = 0; i < 4; i++)
    {
#pragma HLS pipeline
        for (j = 0; j < 4; j++)
        {
#pragma HLS PIPELINE
            temp_sao_lcu_array_v[i][j] = rctngl_hor_dbk_v[rctngl_0idx][i][j + 28];
        }
    }

    for (i = 0; i < 4; i++)
    {
#pragma HLS pipeline
        for (j = 0; j < 34; j++)
        {
#pragma HLS PIPELINE
            temp_sao_lcu_array_v[i][j + 4] = rctngl_hor_dbk_v[rctngl_1idx][i][j];
            rctngl_hor_dbk_v[rctngl_0idx][i][j] = dbkbuffer_v[i + 30][j + 2];
        }
    }

    for (i = 0; i < 34; i++)
    {
#pragma HLS pipeline
        for (j = 0; j < 4; j++)
        {
#pragma HLS PIPELINE
            temp_sao_lcu_array_v[i + 4][j] = rctngl_ver_dbk_v[i][j];
            rctngl_ver_dbk_v[i][j]         = dbkbuffer_v[i + 2][j + 30];
        }
    }
//----------------------------------------------------------------------------------------------------------
    //for (i = 0; i < 4; i++)
    //{
    //    for (j = 0; j < 34; j++)
    //    {
    //        rctngl_hor_dbk_v[lcu_pos % (LCU_col_num + 1)][i][j] = dbkbuffer_v[i + 30][j + 2];
    //    }
    //}
}

void dbk_2_sao(int lcu_pos, int LCU_col_num,
               pel dbkbuffer_y[68][68],                                                    pel dbkbuffer_u[36][36],           pel dbkbuffer_v[36][36], 
               pel temp_sao_lcu_array_y[70][70],  pel temp_sao_lcu_array_y_dupli1[70][70],  pel temp_sao_lcu_array_u[38][38],  pel temp_sao_lcu_array_v[38][38])
               //u8 dbk_lcu_array_y[68][68],                                              u8 dbk_lcu_array_u[36][36],       u8 dbk_lcu_array_v[36][36])
{
#pragma HLS INLINE off
    dbk_2_sao_y(lcu_pos, LCU_col_num, 
                dbkbuffer_y,          
              //rctngl_hor_dbk_y,     rctngl_ver_dbk_y,
                temp_sao_lcu_array_y, temp_sao_lcu_array_y_dupli1);

    dbk_2_sao_u(lcu_pos, LCU_col_num,
                dbkbuffer_u,      //dbk_lcu_array_u,
                temp_sao_lcu_array_u);

    dbk_2_sao_v(lcu_pos, LCU_col_num,
                dbkbuffer_v,      //dbk_lcu_array_v,
                temp_sao_lcu_array_v);
}

static void com_mcpy_SBAC_CTX_MODEL1(SBAC_CTX_MODEL *dst,SBAC_CTX_MODEL *src,int sz)
{
#pragma HLS INLINE off
    for (int i = 0; i < sz; i++) 
    {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=24
        dst[i] = src[i];
    }
}

static void com_mcpy_COM_SBAC_CTX_SAO(COM_SBAC_CTX *dst,COM_SBAC_CTX *src)
{
#pragma HLS INLINE off
    com_mcpy_SBAC_CTX_MODEL1(dst -> sao_merge_flag,src -> sao_merge_flag,3);
    com_mcpy_SBAC_CTX_MODEL1(dst -> sao_mode,src -> sao_mode,1);
    com_mcpy_SBAC_CTX_MODEL1(dst -> sao_offset,src -> sao_offset,1);
}

static void com_mcpy_ENC_SBAC_SAO(ENC_SBAC *dst,ENC_SBAC *src)
{
#pragma HLS INLINE off

    dst -> range = src -> range;
    dst -> code = src -> code;
    dst -> left_bits = src -> left_bits;
    dst -> stacked_ff = src -> stacked_ff;
    dst -> pending_byte = src -> pending_byte;
    dst -> is_pending_byte = src -> is_pending_byte;
    com_mcpy_COM_SBAC_CTX_SAO(&dst -> ctx,&src -> ctx);
    dst -> bitcounter = src -> bitcounter;
}

unsigned char get_eo_type(pel a, pel c, pel b)
{
    unsigned char left_sign  = (c > a) ? 2 : ((c == a) ? 1 : 0);
    unsigned char right_sign = (c > b) ? 2 : ((c == b) ? 1 : 0);
    unsigned char edge_type = left_sign + right_sign;
    return edge_type;
}

void get_stat_eo_8pix(pel a[8], pel c[8], pel b[8], int diff[8], bool pix_valid[8], int diff_sum[5], int count_sum[5])
{
    unsigned char edge_type;
    int diff_temp [8][5];
    int count_temp[8][5];
#pragma HLS ARRAY_PARTITION variable=diff_temp  complete dim=0
#pragma HLS ARRAY_PARTITION variable=count_temp complete dim=0
    LOOP_STATISTIC_EO_PIX: for (int i=0; i<8; i++) { // unroll
#pragma HLS UNROLL
        edge_type = get_eo_type(a[i], c[i], b[i]);
        diff_temp [i][0] = ((edge_type == 0) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][1] = ((edge_type == 1) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][2] = ((edge_type == 2) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][3] = ((edge_type == 3) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][4] = ((edge_type == 4) && (pix_valid[i])) ? diff[i] : 0;
        count_temp[i][0] = ((edge_type == 0) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][1] = ((edge_type == 1) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][2] = ((edge_type == 2) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][3] = ((edge_type == 3) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][4] = ((edge_type == 4) && (pix_valid[i])) ?       1 : 0;
    }
    diff_sum [0] += (diff_temp[0][0] + diff_temp[1][0] + diff_temp[2][0] + diff_temp[3][0] + diff_temp[4][0] + diff_temp[5][0] + diff_temp[6][0] + diff_temp[7][0]);
    diff_sum [1] += (diff_temp[0][1] + diff_temp[1][1] + diff_temp[2][1] + diff_temp[3][1] + diff_temp[4][1] + diff_temp[5][1] + diff_temp[6][1] + diff_temp[7][1]);
    diff_sum [2] += (diff_temp[0][2] + diff_temp[1][2] + diff_temp[2][2] + diff_temp[3][2] + diff_temp[4][2] + diff_temp[5][2] + diff_temp[6][2] + diff_temp[7][2]);
    diff_sum [3] += (diff_temp[0][3] + diff_temp[1][3] + diff_temp[2][3] + diff_temp[3][3] + diff_temp[4][3] + diff_temp[5][3] + diff_temp[6][3] + diff_temp[7][3]);
    diff_sum [4] += (diff_temp[0][4] + diff_temp[1][4] + diff_temp[2][4] + diff_temp[3][4] + diff_temp[4][4] + diff_temp[5][4] + diff_temp[6][4] + diff_temp[7][4]);
    count_sum[0] += count_temp[0][0] + count_temp[1][0] + count_temp[2][0] + count_temp[3][0] + count_temp[4][0] + count_temp[5][0] + count_temp[6][0] + count_temp[7][0];
    count_sum[1] += count_temp[0][1] + count_temp[1][1] + count_temp[2][1] + count_temp[3][1] + count_temp[4][1] + count_temp[5][1] + count_temp[6][1] + count_temp[7][1];
    count_sum[2] += count_temp[0][2] + count_temp[1][2] + count_temp[2][2] + count_temp[3][2] + count_temp[4][2] + count_temp[5][2] + count_temp[6][2] + count_temp[7][2];
    count_sum[3] += count_temp[0][3] + count_temp[1][3] + count_temp[2][3] + count_temp[3][3] + count_temp[4][3] + count_temp[5][3] + count_temp[6][3] + count_temp[7][3];
    count_sum[4] += count_temp[0][4] + count_temp[1][4] + count_temp[2][4] + count_temp[3][4] + count_temp[4][4] + count_temp[5][4] + count_temp[6][4] + count_temp[7][4];
}

void get_stat_bo_8pix(int rightShiftBit, pel c[8], int diff[8], bool pix_valid[8], int diff_sum[32], int count_sum[32])
{
    unsigned char band_type;
    int diff_temp [8][32];
    int count_temp[8][32];
#pragma HLS ARRAY_PARTITION variable=diff_temp  complete dim=0
#pragma HLS ARRAY_PARTITION variable=count_temp complete dim=0
    LOOP_STATISTIC_BO_PIX: for (int i=0; i<8; i++) { // UNROLL
#pragma HLS UNROLL
        band_type = (unsigned char)(c[i] >> rightShiftBit);
        for (unsigned char band=0; band<32; band++) { // UNROLL
#pragma HLS UNROLL
            diff_temp [i][band] = ((band_type == band) && pix_valid[i]) ? diff[i] : 0;
            count_temp[i][band] = ((band_type == band) && pix_valid[i]) ?       1 : 0;
        }
    }
    for (int band=0; band<32; band++) {
#pragma HLS UNROLL
        diff_sum [band] += diff_temp [0][band] + diff_temp [1][band] + diff_temp [2][band] + diff_temp [3][band] + diff_temp [4][band] + diff_temp [5][band] + diff_temp [6][band] + diff_temp [7][band];
        count_sum[band] += count_temp[0][band] + count_temp[1][band] + count_temp[2][band] + count_temp[3][band] + count_temp[4][band] + count_temp[5][band] + count_temp[6][band] + count_temp[7][band];
    }
}

void block_reform_luma(pel org_lcu_array_y1[64][64], pel dbkbuffer_y1[68][68], pel org_buf[3][176][8], pel rec_buf[3][176][8])
{
#pragma HLS INLINE off
    // the input buffer will be orgnized as below.
    // for pixel (row, col) in [64,64]
    // dim2: 3: is line index. will be row%3
    // dim1: 176: is the buffer index;  (row / 3) * 8 + col / 8;
    // dim0: 8: is the col%8
    // will be 3 BRAMs, with data width 8x10bit, and depth 171.

    LOOP_BLOCK_REFORM_LUMA_ROW: for (int row = 0; row < 64; row++) { // pipeline
#pragma HLS PIPELINE
        LOOP_BLOCK_REFORM_LUMA_COL: for (int col = 0; col < 64; col++) { // unroll factor proposed to be 8

            int idx0 = col % 8;
            int idx1 = (row / 3) * 8 + col / 8;
            int idx2 = row % 3;
            org_buf[idx2][idx1][idx0] = org_lcu_array_y1[row][col];
            rec_buf[idx2][idx1][idx0] = dbkbuffer_y1[row + 4][col + 4];
        }
    }
}

void getStatblk_Luma_hw(unsigned char sao_Y_enable, int bit_depth, int lcu_pix_height, int lcu_pix_width,  
    //pel dbkbuffer_y1[68][68], pel org_lcu_array_y1[64][64], 
    pel org_buf[3][176][8], pel rec_buf[3][176][8],
    int pSaoStat_YEO0_diff[32],   int pSaoStat_YEO0_count[32], 
    int pSaoStat_YEO90_diff[32],  int pSaoStat_YEO90_count[32],
    int pSaoStat_YEO135_diff[32], int pSaoStat_YEO135_count[32], 
    int pSaoStat_YEO45_diff[32],  int pSaoStat_YEO45_count[32],
    int pSaoStat_YBO_diff[32],    int pSaoStat_YBO_count[32])
{
#pragma HLS INLINE off
    if ((!sao_Y_enable) || (lcu_pix_height==0) || (lcu_pix_width==0)) { return; }

    int rightShiftBit = bit_depth - 5; // BO 
    int stat_eo0_diff_sum   [ 5]; // = { 0, 0, 0, 0, 0 }; 
    int stat_eo0_count_sum  [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo90_diff_sum  [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo90_count_sum [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo135_diff_sum [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo135_count_sum[ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo45_diff_sum  [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo45_count_sum [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_bo_diff_sum    [32]; // = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int stat_bo_count_sum   [32]; // = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#pragma HLS ARRAY_PARTITION variable=stat_eo0_diff_sum      complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo0_count_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo90_diff_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo90_count_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo135_diff_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo135_count_sum   complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo45_diff_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo45_count_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_bo_diff_sum       complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_bo_count_sum      complete dim=0
    for (int i=0; i<32; i++) {
        if (i<5) {
            stat_eo0_diff_sum  [i] = 0; stat_eo0_count_sum  [i] = 0;
            stat_eo90_diff_sum [i] = 0; stat_eo90_count_sum [i] = 0;
            stat_eo135_diff_sum[i] = 0; stat_eo135_count_sum[i] = 0;
            stat_eo45_diff_sum [i] = 0; stat_eo45_count_sum [i] = 0;
        }
        stat_bo_diff_sum[i] = 0; stat_bo_count_sum[i] = 0;
    }
    
    // 3 is three lines. 
    // 17 is left 1 pixel + 8 processing pixels + 8 right pixels
    // org_subblock and rec_subblock will partition into registers. 
    pel org_subblock[3][17]; // REG
    pel rec_subblock[3][17]; // REG
#pragma HLS ARRAY_PARTITION variable=org_subblock complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_subblock complete dim=0

    // buf init.
    int row = 0, col = 0;
    LOOP_STATISTIC_LUMA_SUBBLOCK_INIT: for (int i = 0; i < 8; i++) { // unroll
#pragma HLS UNROLL
        // 1st ROW of LCU
        org_subblock[1][1 + i] = org_buf[row % 3][(row) / 3 * 8 + col][i];
        rec_subblock[1][1 + i] = rec_buf[row % 3][(row) / 3 * 8 + col][i];
        // 2nd ROW of LCU
        org_subblock[2][1 + i] = org_buf[(row + 1) % 3][(row + 1) / 3 * 8 + col][i];
        rec_subblock[2][1 + i] = rec_buf[(row + 1) % 3][(row + 1) / 3 * 8 + col][i];
    }

//     LOOP_STATISTIC_LUMA_ROW: for (int row = 0; row < 64; row++) { // PIPELINE
// #pragma HLS PIPELINE II=1
//         LOOP_STATISTIC_LUMA_COL: for (int col = 0; col < 64/8; col++) { // PIPELINE
// #pragma HLS PIPELINE II=1


    LOOP_STATISTIC_LUMA: for (int k = 0; k < 64*8; k++) { // where row=64, col=64/8=8.  // pipeline
#pragma HLS PIPELINE II=1
            int row_above ;
            int row_cur   ;
            int row_bottom;
            int col_right ;
            // if (col+1>=8) { // when exceed the right boundary. access the left line. 
            if (col == 7) {
                row_above  = row;
                row_cur    = row + 1;
                row_bottom = row + 2;
                col_right  = 0;
            } else {        // get the right data.
                row_above  = row - 1;
                row_cur    = row;
                row_bottom = row + 1;
                col_right  = col + 1;
            }
            
            for (int i = 0; i < 8; i++) { // Load right dependency, and pre-fetch data for next loop.  // unroll
#pragma HLS UNROLL
                if (row_above >= 0)   org_subblock[0][9 + i] = org_buf[row_above  % 3][row_above  / 3 * 8 + col_right][i];
                if (row_cur    < 64)  org_subblock[1][9 + i] = org_buf[row_cur    % 3][row_cur    / 3 * 8 + col_right][i];
                if (row_bottom < 64)  org_subblock[2][9 + i] = org_buf[row_bottom % 3][row_bottom / 3 * 8 + col_right][i];

                if (row_above >= 0)   rec_subblock[0][9 + i] = rec_buf[row_above  % 3][row_above  / 3 * 8 + col_right][i];
                if (row_cur    < 64)  rec_subblock[1][9 + i] = rec_buf[row_cur    % 3][row_cur    / 3 * 8 + col_right][i];
                if (row_bottom < 64)  rec_subblock[2][9 + i] = rec_buf[row_bottom % 3][row_bottom / 3 * 8 + col_right][i];
            }

            int col_mult_8 = col * 8;

            //// TEST
            //printf("dbk_buffer: \n");
            //for (int i = 0; i < 8; i++) { printf(" %d ,", dbkbuffer_y1[4 + row][4 + col_mult_8 + i]); } printf("\n");

            //printf("rec_subblock: \n");
            //for (int i = 0; i < 8; i++) { printf(" %d ,", rec_subblock[0][1 + i]); } printf("\n");
            //for (int i = 0; i < 8; i++) { printf(" %d ,", rec_subblock[1][1 + i]); } printf("\n");
            //for (int i = 0; i < 8; i++) { printf(" %d ,", rec_subblock[2][1 + i]); } printf("\n");
            //// END TEST

            // --- Parallely statistic 8 pixels ------------------------------------------------
            bool pix_valid_eo0[8];
            bool pix_valid_eo90[8];
            bool pix_valid_eo135[8];
            bool pix_valid_eo45[8];
            bool pix_valid_bo[8];
            pel org_c[8];
            pel eo0_a[8];   pel eo_c[8]; pel eo0_b[8];
            pel eo90_a[8];               pel eo90_b[8];
            pel eo135_a[8];              pel eo135_b[8];
            pel eo45_a[8];               pel eo45_b[8];
            int diff[8];
            
            for (int i = 0; i < 8; i++) { // UNROLL
#pragma HLS UNROLL
                org_c[i]   = org_subblock[1][1 + i];
                eo_c[i]    = rec_subblock[1][1 + i];
                eo0_a[i]   = rec_subblock[1][0 + i]; eo0_b[i]   = rec_subblock[1][2 + i];
                eo90_a[i]  = rec_subblock[0][1 + i]; eo90_b[i]  = rec_subblock[2][1 + i];
                eo135_a[i] = rec_subblock[0][0 + i]; eo135_b[i] = rec_subblock[2][2 + i];
                eo45_a[i]  = rec_subblock[0][2 + i]; eo45_b[i]  = rec_subblock[2][0 + i];
                diff[i] = (int)org_c[i] - (int)eo_c[i];
                if (i != 0) {
                    pix_valid_eo0[i]   = ((col_mult_8 + i + 1) < lcu_pix_width) &&               ( row      < lcu_pix_height);
                    pix_valid_eo90[i]  = ((col_mult_8 + i    ) < lcu_pix_width) && (row != 0) && ((row + 1) < lcu_pix_height);
                    pix_valid_eo135[i] = ((col_mult_8 + i + 1) < lcu_pix_width) && (row != 0) && ((row + 1) < lcu_pix_height);
                    pix_valid_eo45[i]  = ((col_mult_8 + i + 1) < lcu_pix_width) && (row != 0) && ((row + 1) < lcu_pix_height);
                    pix_valid_bo[i]    = ((col_mult_8 + i    ) < lcu_pix_width) &&               ( row      < lcu_pix_height);
                }
            }
            pix_valid_eo0[0]   = (col != 0) && ((col_mult_8 + 1) < lcu_pix_width) &&               ( row      < lcu_pix_height);
            pix_valid_eo90[0]  =               ( col_mult_8      < lcu_pix_width) && (row != 0) && ((row + 1) < lcu_pix_height);
            pix_valid_eo135[0] = (col != 0) && ((col_mult_8 + 1) < lcu_pix_width) && (row != 0) && ((row + 1) < lcu_pix_height);
            pix_valid_eo45[0]  = (col != 0) && ((col_mult_8 + 1) < lcu_pix_width) && (row != 0) && ((row + 1) < lcu_pix_height);
            pix_valid_bo[0]    =               ( col_mult_8      < lcu_pix_width) &&               ( row      < lcu_pix_height);

            //printf("row: %d, col: %d   \n",  row, col);
            //printf("pix_valid: %d, %d, %d, %d, %d, %d, %d, %d.  \n", pix_valid_eo90[0], pix_valid_eo90[1], pix_valid_eo90[2], pix_valid_eo90[3], pix_valid_eo90[4], pix_valid_eo90[5], pix_valid_eo90[6], pix_valid_eo90[7]);
            get_stat_eo_8pix(eo0_a,   eo_c, eo0_b,   diff, pix_valid_eo0,   stat_eo0_diff_sum,   stat_eo0_count_sum);
            get_stat_eo_8pix(eo90_a,  eo_c, eo90_b,  diff, pix_valid_eo90,  stat_eo90_diff_sum,  stat_eo90_count_sum);
            get_stat_eo_8pix(eo135_a, eo_c, eo135_b, diff, pix_valid_eo135, stat_eo135_diff_sum, stat_eo135_count_sum);
            get_stat_eo_8pix(eo45_a,  eo_c, eo45_b,  diff, pix_valid_eo45,  stat_eo45_diff_sum,  stat_eo45_count_sum);
            get_stat_bo_8pix(rightShiftBit, eo_c,    diff, pix_valid_bo,    stat_bo_diff_sum,    stat_bo_count_sum);
            // ---------------------------------------------------------------------------------
            // register shift.
            for (int i = 0; i < 9; i++) { // UNROLL
#pragma HLS UNROLL
                org_subblock[0][i] = org_subblock[0][i + 8];
                org_subblock[1][i] = org_subblock[1][i + 8];
                org_subblock[2][i] = org_subblock[2][i + 8];

                rec_subblock[0][i] = rec_subblock[0][i + 8];
                rec_subblock[1][i] = rec_subblock[1][i + 8];
                rec_subblock[2][i] = rec_subblock[2][i + 8];
             }
            
        if (col == (8-1)) {
            col = 0;
            row ++;
        } else {
            col ++;
        }
    }

    for (int i=0; i<32; i++) { // temp, unroll
#pragma HLS UNROLL
        if (i < 5) {
            pSaoStat_YEO0_diff  [i] = stat_eo0_diff_sum  [i];  pSaoStat_YEO0_count  [i] = stat_eo0_count_sum  [i];
            pSaoStat_YEO90_diff [i] = stat_eo90_diff_sum [i];  pSaoStat_YEO90_count [i] = stat_eo90_count_sum [i];
            pSaoStat_YEO135_diff[i] = stat_eo135_diff_sum[i];  pSaoStat_YEO135_count[i] = stat_eo135_count_sum[i];
            pSaoStat_YEO45_diff [i] = stat_eo45_diff_sum [i];  pSaoStat_YEO45_count [i] = stat_eo45_count_sum [i];
        }
        pSaoStat_YBO_diff[i] = stat_bo_diff_sum[i];  pSaoStat_YBO_count[i] = stat_bo_count_sum[i];
    }
}

void block_reform_chroma(pel org_lcu_array[32][32], pel dbkbuffer[36][36], pel org_buf[3][88][4], pel rec_buf[3][88][4])
{
#pragma HLS INLINE off
    // the input buffer will be orgnized as below.
    // for pixel (row, col) in [32,32]
    // dim2: 3: is line index. will be row%3
    // dim1: 88: is the buffer index;  (row / 3) * 8 + col / 4;
    // dim0: 4: is the col%4
    // will be 3 BRAMs, with data width 8x10bit, and depth 171.

    LOOP_BLOCK_REFORM_CHROMA_ROW: for (int row = 0; row < 32; row++) { // pipeline
#pragma HLS PIPELINE
        LOOP_BLOCK_REFORM_CHROMA_COL: for (int col = 0; col < 32; col++) { // unroll factor proposed to be 4

            int idx0 = col % 4;
            int idx1 = (row / 3) * 8 + col / 4;
            int idx2 = row % 3;
            org_buf[idx2][idx1][idx0] = org_lcu_array[row][col];
            rec_buf[idx2][idx1][idx0] = dbkbuffer[row + 4][col + 4];
        }
    }
}

#if 1
void get_stat_eo_4pix(pel a[4], pel c[4], pel b[4], int diff[4], bool pix_valid[4], int diff_sum[5], int count_sum[5])
{
    unsigned char edge_type;
    int diff_temp [4][5];
    int count_temp[4][5];
#pragma HLS ARRAY_PARTITION variable=diff_temp  complete dim=0
#pragma HLS ARRAY_PARTITION variable=count_temp complete dim=0
    for (int i = 0; i < 4; i++) {   // UNROLL
#pragma HLS UNROLL
        edge_type = get_eo_type(a[i], c[i], b[i]);
        diff_temp [i][0] = ((edge_type == 0) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][1] = ((edge_type == 1) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][2] = ((edge_type == 2) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][3] = ((edge_type == 3) && (pix_valid[i])) ? diff[i] : 0;
        diff_temp [i][4] = ((edge_type == 4) && (pix_valid[i])) ? diff[i] : 0;
        count_temp[i][0] = ((edge_type == 0) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][1] = ((edge_type == 1) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][2] = ((edge_type == 2) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][3] = ((edge_type == 3) && (pix_valid[i])) ?       1 : 0;
        count_temp[i][4] = ((edge_type == 4) && (pix_valid[i])) ?       1 : 0;
    }
    diff_sum [0] += diff_temp[0][0] + diff_temp[1][0] + diff_temp[2][0] + diff_temp[3][0];
    diff_sum [1] += diff_temp[0][1] + diff_temp[1][1] + diff_temp[2][1] + diff_temp[3][1];
    diff_sum [2] += diff_temp[0][2] + diff_temp[1][2] + diff_temp[2][2] + diff_temp[3][2];
    diff_sum [3] += diff_temp[0][3] + diff_temp[1][3] + diff_temp[2][3] + diff_temp[3][3];
    diff_sum [4] += diff_temp[0][4] + diff_temp[1][4] + diff_temp[2][4] + diff_temp[3][4];
    count_sum[0] += count_temp[0][0] + count_temp[1][0] + count_temp[2][0] + count_temp[3][0];
    count_sum[1] += count_temp[0][1] + count_temp[1][1] + count_temp[2][1] + count_temp[3][1];
    count_sum[2] += count_temp[0][2] + count_temp[1][2] + count_temp[2][2] + count_temp[3][2];
    count_sum[3] += count_temp[0][3] + count_temp[1][3] + count_temp[2][3] + count_temp[3][3];
    count_sum[4] += count_temp[0][4] + count_temp[1][4] + count_temp[2][4] + count_temp[3][4];
}
void get_stat_bo_4pix(int rightShiftBit, pel c[4], int diff[4], bool pix_valid[4], int diff_sum[32], int count_sum[32])
{
    unsigned char band_type;
    int diff_temp [4][32];
    int count_temp[4][32];
#pragma HLS ARRAY_PARTITION variable=diff_temp  complete dim=0
#pragma HLS ARRAY_PARTITION variable=count_temp complete dim=0
    for (int i = 0; i < 4; i++) { // UNROLL
#pragma HLS UNROLL
        band_type = (unsigned char)(c[i] >> rightShiftBit);
        for (unsigned char band = 0; band < 32; band++) { // UNROLL
#pragma HLS UNROLL
            diff_temp [i][band] = ((band_type == band) && pix_valid[i]) ? diff[i] : 0;
            count_temp[i][band] = ((band_type == band) && pix_valid[i]) ?       1 : 0;
        }
    }
    for (int band = 0; band < 32; band++) { // unroll
#pragma HLS UNROLL
        diff_sum [band] += diff_temp [0][band] + diff_temp [1][band] + diff_temp [2][band] + diff_temp [3][band];
        count_sum[band] += count_temp[0][band] + count_temp[1][band] + count_temp[2][band] + count_temp[3][band];
    }
}
#else
void get_stat_eo(pel a, pel c, pel b, int diff, int stat_diff[5], int stat_count[5])
{
    unsigned char left_sign  = (c > a) ? 2 : ((c == a) ? 1 : 0);
    unsigned char right_sign = (c > b) ? 2 : ((c == b) ? 1 : 0);
    unsigned char edge_type = left_sign + right_sign;

    if (edge_type == 0) {
        stat_diff[0] += diff; stat_count[0] += 1;
    } else if (edge_type == 1) {
        stat_diff[1] += diff; stat_count[1] += 1;
    } else if (edge_type == 2) {
        stat_diff[2] += diff; stat_count[2] += 1;
    } else if (edge_type == 3) {
        stat_diff[3] += diff; stat_count[3] += 1;
    } else if (edge_type == 4) {
        stat_diff[4] += diff; stat_count[4] += 1;
    }
}
void get_stat_bo(pel rec, int rightShiftBit, int diff, int stat_diff[32], int stat_count[32])
{
    unsigned char band_type = (unsigned char)((unsigned int)rec >> (unsigned int)rightShiftBit);

    stat_diff [band_type] += diff;
    stat_count[band_type] +=    1;
}
#endif 

#if 1   // Statistic Chroma, 4 pixel parallely.
static void getStatblk_Chroma_hw(u8 sao_comp_enable, int bit_depth, int lcu_height, int lcu_width,
    pel org_buf[3][88][4], pel rec_buf[3][88][4],
    int pSaoStat_EO0_diff[32],   int pSaoStat_EO0_count[32],   
    int pSaoStat_EO90_diff[32],  int pSaoStat_EO90_count[32], 
    int pSaoStat_EO135_diff[32], int pSaoStat_EO135_count[32], 
    int pSaoStat_EO45_diff[32],  int pSaoStat_EO45_count[32], 
    int pSaoStat_BO_diff[32],    int pSaoStat_BO_count[32])
{
    if ((!sao_comp_enable) || (lcu_height == 0) || (lcu_width == 0)) { return; }

    int rightShiftBit = bit_depth - 5; // BO
    // temp. Will be deleted.
    int stat_eo0_diff_sum   [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo0_count_sum  [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo90_diff_sum  [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo90_count_sum [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo135_diff_sum [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo135_count_sum[ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo45_diff_sum  [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_eo45_count_sum [ 5];// = { 0, 0, 0, 0, 0 };
    int stat_bo_diff_sum    [32];// = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int stat_bo_count_sum   [32];// = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#pragma HLS ARRAY_PARTITION variable=stat_eo0_diff_sum      complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo0_count_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo90_diff_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo90_count_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo135_diff_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo135_count_sum   complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo45_diff_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo45_count_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_bo_diff_sum       complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_bo_count_sum      complete dim=0

    for (int i=0; i<32; i++) {
#pragma HLS UNROLL
        if (i<5) {
            stat_eo0_diff_sum  [i] = 0; stat_eo0_count_sum  [i] = 0;
            stat_eo90_diff_sum [i] = 0; stat_eo90_count_sum [i] = 0;
            stat_eo135_diff_sum[i] = 0; stat_eo135_count_sum[i] = 0;
            stat_eo45_diff_sum [i] = 0; stat_eo45_count_sum [i] = 0;
        }
        stat_bo_diff_sum[i] = 0; stat_bo_count_sum[i] = 0;
    }
    // 3 is three lines. 
    // 9 is left 1 pixel + 4 processing pixels + 4 right pixels
    // org_subblock and rec_subblock will partition into registers. 
    pel org_subblock[3][9] = {0,};  // REG
    pel rec_subblock[3][9] = {0,};  // REG
#pragma HLS ARRAY_PARTITION variable=org_subblock complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_subblock complete dim=0

    // buf init.
    int row = 0, col = 0;
    LOOP_STATISTIC_CHROMA_SUBBLOCK_INIT: for (int i = 0; i < 4; i++) { // unroll
#pragma HLS UNROLL
        // 1st ROW of LCU
        org_subblock[1][1 + i] = org_buf[row % 3][(row) / 3 * 8 + col][i];
        rec_subblock[1][1 + i] = rec_buf[row % 3][(row) / 3 * 8 + col][i];
        // 2nd ROW of LCU
        org_subblock[2][1 + i] = org_buf[(row + 1) % 3][(row + 1) / 3 * 8 + col][i];
        rec_subblock[2][1 + i] = rec_buf[(row + 1) % 3][(row + 1) / 3 * 8 + col][i];
    }

    LOOP_STATISTIC_CHROMA_ROW: for (int row = 0; row < 32; row++) { // pipeline
//#pragma HLS PIPELINE II=1
        LOOP_STATISTIC_CHROMA_COL: for (int col = 0; col < 32/4; col++) { // pipeline
#pragma HLS PIPELINE II=1
            int row_above ;
            int row_cur   ;
            int row_bottom;
            int col_right ;
          //if (col+1>=8) { // when exceed the right boundary. access the left line. 
          if (col == 7) {
                row_above  = row;
                row_cur    = row + 1;
                row_bottom = row + 2;
                col_right  = 0;
            } else {        // get the right data.
                row_above  = row - 1;
                row_cur    = row;
                row_bottom = row + 1;
                col_right  = col + 1;
            }
            
            for (int i = 0; i < 4; i++) { // Load right dependency, and pre-fetch data for next loop.  // unroll
#pragma HLS UNROLL
                if (row_above >= 0)   org_subblock[0][5 + i] = org_buf[row_above  % 3][row_above  / 3 * 8 + col_right][i];
                if (row_cur    < 32)  org_subblock[1][5 + i] = org_buf[row_cur    % 3][row_cur    / 3 * 8 + col_right][i];
                if (row_bottom < 32)  org_subblock[2][5 + i] = org_buf[row_bottom % 3][row_bottom / 3 * 8 + col_right][i];

                if (row_above >= 0)   rec_subblock[0][5 + i] = rec_buf[row_above  % 3][row_above  / 3 * 8 + col_right][i];
                if (row_cur    < 32)  rec_subblock[1][5 + i] = rec_buf[row_cur    % 3][row_cur    / 3 * 8 + col_right][i];
                if (row_bottom < 32)  rec_subblock[2][5 + i] = rec_buf[row_bottom % 3][row_bottom / 3 * 8 + col_right][i];
            }

            int col_mult_4 = col * 4;

            //// TEST
            //printf("dbk_buffer: \n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", dbkbuffer[4 + row][4 + col_mult_4 + i]); } printf("\n");
            //printf("rec_subblock: \n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", rec_subblock[0][1 + i]); } printf("\n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", rec_subblock[1][1 + i]); } printf("\n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", rec_subblock[2][1 + i]); } printf("\n");
            //// END TEST

            // --- Parallely statistic 4 pixels ------------------------------------------------
            bool pix_valid_eo0  [4];
            bool pix_valid_eo90 [4];
            bool pix_valid_eo135[4];
            bool pix_valid_eo45 [4];
            bool pix_valid_bo   [4];
            pel org_c  [4];
            pel eo0_a  [4]; pel eo_c[4]; pel eo0_b  [4];
            pel eo90_a [4];              pel eo90_b [4];
            pel eo135_a[4];              pel eo135_b[4];
            pel eo45_a [4];              pel eo45_b [4];
            int diff   [4];
            
            for (int i = 0; i < 4; i++) { // UNROLL
#pragma HLS UNROLL
                org_c  [i] = org_subblock[1][1 + i];
                eo_c   [i] = rec_subblock[1][1 + i];
                eo0_a  [i] = rec_subblock[1][0 + i]; eo0_b  [i] = rec_subblock[1][2 + i];
                eo90_a [i] = rec_subblock[0][1 + i]; eo90_b [i] = rec_subblock[2][1 + i];
                eo135_a[i] = rec_subblock[0][0 + i]; eo135_b[i] = rec_subblock[2][2 + i];
                eo45_a [i] = rec_subblock[0][2 + i]; eo45_b [i] = rec_subblock[2][0 + i];
                diff   [i] = (int)org_c[i] - (int)eo_c[i];
                if (i != 0) {
                    pix_valid_eo0  [i] = ((col_mult_4 + i + 1) < lcu_width) &&               ( row      < lcu_height);
                    pix_valid_eo90 [i] = ((col_mult_4 + i    ) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
                    pix_valid_eo135[i] = ((col_mult_4 + i + 1) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
                    pix_valid_eo45 [i] = ((col_mult_4 + i + 1) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
                    pix_valid_bo   [i] = ((col_mult_4 + i    ) < lcu_width) &&               ( row      < lcu_height);
                }
            }
            pix_valid_eo0  [0] = (col != 0) && ((col_mult_4 + 1) < lcu_width) &&               ( row      < lcu_height);
            pix_valid_eo90 [0] =               ( col_mult_4      < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
            pix_valid_eo135[0] = (col != 0) && ((col_mult_4 + 1) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
            pix_valid_eo45 [0] = (col != 0) && ((col_mult_4 + 1) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
            pix_valid_bo   [0] =               ( col_mult_4      < lcu_width) &&               ( row      < lcu_height);

            //printf("row: %d, col: %d   \n",  row, col);
            //printf("pix_valid: %d, %d, %d, %d.  \n", pix_valid_eo0[0], pix_valid_eo0[1], pix_valid_eo0[2], pix_valid_eo0[3]);
            get_stat_eo_4pix(eo0_a,   eo_c, eo0_b,   diff, pix_valid_eo0,   stat_eo0_diff_sum,   stat_eo0_count_sum);
            get_stat_eo_4pix(eo90_a,  eo_c, eo90_b,  diff, pix_valid_eo90,  stat_eo90_diff_sum,  stat_eo90_count_sum);
            get_stat_eo_4pix(eo135_a, eo_c, eo135_b, diff, pix_valid_eo135, stat_eo135_diff_sum, stat_eo135_count_sum);
            get_stat_eo_4pix(eo45_a,  eo_c, eo45_b,  diff, pix_valid_eo45,  stat_eo45_diff_sum,  stat_eo45_count_sum);
            get_stat_bo_4pix(rightShiftBit, eo_c,    diff, pix_valid_bo,    stat_bo_diff_sum,    stat_bo_count_sum);
            // ---------------------------------------------------------------------------------
            // register shift.
            for (int i = 0; i < 5; i++) { // UNROLL
#pragma HLS UNROLL
                org_subblock[0][i] = org_subblock[0][i + 4];
                org_subblock[1][i] = org_subblock[1][i + 4];
                org_subblock[2][i] = org_subblock[2][i + 4];

                rec_subblock[0][i] = rec_subblock[0][i + 4];
                rec_subblock[1][i] = rec_subblock[1][i + 4];
                rec_subblock[2][i] = rec_subblock[2][i + 4];
            }
        }
    }

    for (int i=0; i<32; i++) { // temp, unroll
#pragma HLS UNROLL
        if (i < 5) {
            pSaoStat_EO0_diff[i]   = stat_eo0_diff_sum[i];   pSaoStat_EO0_count[i]   = stat_eo0_count_sum[i];
            pSaoStat_EO90_diff[i]  = stat_eo90_diff_sum[i];  pSaoStat_EO90_count[i]  = stat_eo90_count_sum[i];
            pSaoStat_EO135_diff[i] = stat_eo135_diff_sum[i]; pSaoStat_EO135_count[i] = stat_eo135_count_sum[i];
            pSaoStat_EO45_diff[i]  = stat_eo45_diff_sum[i];  pSaoStat_EO45_count[i]  = stat_eo45_count_sum[i];
        }
        pSaoStat_BO_diff[i] = stat_bo_diff_sum[i];  pSaoStat_BO_count[i] = stat_bo_count_sum[i];
    }
}
#else // Statistic Chroma, serially. 
      // From HW perspective, Obviously, this function is better. However, Co-sim for this function was failed.
static void getStatblk_Chroma_hw(u8 sao_comp_enable, int bit_depth, int lcu_height, int lcu_width,
    pel org_buf[3][88][4], pel rec_buf[3][88][4],
    int pSaoStat_EO0_diff[32],   int pSaoStat_EO0_count[32],   
    int pSaoStat_EO90_diff[32],  int pSaoStat_EO90_count[32], 
    int pSaoStat_EO135_diff[32], int pSaoStat_EO135_count[32], 
    int pSaoStat_EO45_diff[32],  int pSaoStat_EO45_count[32], 
    int pSaoStat_BO_diff[32],    int pSaoStat_BO_count[32])
{
    if ((!sao_comp_enable) || (lcu_height == 0) || (lcu_width == 0)) { return; }

    int rightShiftBit = bit_depth - 5; // BO
    // temp. Will be deleted.
    int stat_eo0_diff_sum   [ 5]; // = { 0, 0, 0, 0, 0 }; 
    int stat_eo0_count_sum  [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo90_diff_sum  [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo90_count_sum [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo135_diff_sum [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo135_count_sum[ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo45_diff_sum  [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_eo45_count_sum [ 5]; // = { 0, 0, 0, 0, 0 };
    int stat_bo_diff_sum    [32]; // = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int stat_bo_count_sum   [32]; // = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#pragma HLS ARRAY_PARTITION variable=stat_eo0_diff_sum      complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo0_count_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo90_diff_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo90_count_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo135_diff_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo135_count_sum   complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo45_diff_sum     complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_eo45_count_sum    complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_bo_diff_sum       complete dim=0
#pragma HLS ARRAY_PARTITION variable=stat_bo_count_sum      complete dim=0
    for (int i=0; i<32; i++) {
        if (i<5) {
            stat_eo0_diff_sum  [i] = 0; stat_eo0_count_sum  [i] = 0;
            stat_eo90_diff_sum [i] = 0; stat_eo90_count_sum [i] = 0;
            stat_eo135_diff_sum[i] = 0; stat_eo135_count_sum[i] = 0;
            stat_eo45_diff_sum [i] = 0; stat_eo45_count_sum [i] = 0;
        }
        stat_bo_diff_sum[i] = 0; stat_bo_count_sum[i] = 0;
    }

    // 3 is three lines. 
    // 9 is left 1 pixel + 4 processing pixels + 4 right pixels
    // org_subblock and rec_subblock will partition into registers. 
    pel org_subblock[3][9];  // REG
    pel rec_subblock[3][9];  // REG
#pragma HLS ARRAY_PARTITION variable=org_subblock complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_subblock complete dim=0

    // buf init.
    int row = 0, col = 0;
    LOOP_STATISTIC_CHROMA_SUBBLOCK_INIT: for (int i = 0; i < 4; i++) { // unroll
#pragma HLS UNROLL
        // 1st ROW of LCU
        org_subblock[1][1 + i] = org_buf[row % 3][(row) / 3 * 8 + col][i];
        rec_subblock[1][1 + i] = rec_buf[row % 3][(row) / 3 * 8 + col][i];
        // 2nd ROW of LCU
        org_subblock[2][1 + i] = org_buf[(row + 1) % 3][(row + 1) / 3 * 8 + col][i];
        rec_subblock[2][1 + i] = rec_buf[(row + 1) % 3][(row + 1) / 3 * 8 + col][i];
    }

    LOOP_STATISTIC_CHROMA_ROW: for (int row = 0; row < 32; row++) { // pipeline
//#pragma HLS PIPELINE II=1
        LOOP_STATISTIC_CHROMA_COL: for (int col = 0; col < 32/4; col++) { // pipeline
#pragma HLS PIPELINE II=1
            int row_above ;
            int row_cur   ;
            int row_bottom;
            int col_right ;
            //if (col+1>=8) { // when exceed the right boundary. access the left line. 
            if (col == 7) {
                row_above  = row;
                row_cur    = row + 1;
                row_bottom = row + 2;
                col_right  = 0;
            } else {        // get the right data.
                row_above  = row - 1;
                row_cur    = row;
                row_bottom = row + 1;
                col_right  = col + 1;
            }
            
            for (int i = 0; i < 4; i++) { // Load right dependency, and pre-fetch data for next loop.  // unroll
#pragma HLS UNROLL
                if (row_above >= 0)   org_subblock[0][5 + i] = org_buf[row_above  % 3][row_above  / 3 * 8 + col_right][i];
                if (row_cur    < 32)  org_subblock[1][5 + i] = org_buf[row_cur    % 3][row_cur    / 3 * 8 + col_right][i];
                if (row_bottom < 32)  org_subblock[2][5 + i] = org_buf[row_bottom % 3][row_bottom / 3 * 8 + col_right][i];

                if (row_above >= 0)   rec_subblock[0][5 + i] = rec_buf[row_above  % 3][row_above  / 3 * 8 + col_right][i];
                if (row_cur    < 32)  rec_subblock[1][5 + i] = rec_buf[row_cur    % 3][row_cur    / 3 * 8 + col_right][i];
                if (row_bottom < 32)  rec_subblock[2][5 + i] = rec_buf[row_bottom % 3][row_bottom / 3 * 8 + col_right][i];
            }

            int col_mult_4 = col * 4;

            //// TEST
            //printf("dbk_buffer: \n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", dbkbuffer[4 + row][4 + col_mult_4 + i]); } printf("\n");
            //printf("rec_subblock: \n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", rec_subblock[0][1 + i]); } printf("\n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", rec_subblock[1][1 + i]); } printf("\n");
            //for (int i = 0; i < 4+4; i++) { printf(" %d ,", rec_subblock[2][1 + i]); } printf("\n");
            //// END TEST

            // --- serially statistic 4 pixels ------------------------------------------------
            LOOP_STATISTIC_CHROMA_SUBBLOCK: for (int i=0; i<4; i++) { // pipeline
                pel org_c   = org_subblock[1][1 + i];
                pel eo0_a   = rec_subblock[1][0 + i];   pel eo_c  = rec_subblock[1][1 + i];  pel eo0_b   = rec_subblock[1][2 + i];
                pel eo90_a  = rec_subblock[0][1 + i];                                        pel eo90_b  = rec_subblock[2][1 + i];
                pel eo135_a = rec_subblock[0][0 + i];                                        pel eo135_b = rec_subblock[2][2 + i];
                pel eo45_a  = rec_subblock[0][2 + i];                                        pel eo45_b  = rec_subblock[2][0 + i];
                int diff    = (int)org_c - (int)eo_c;
                bool pix_valid_eo0   = !((col == 0)&&(i==0)) && ((col_mult_4 + i + 1) < lcu_width) &&               ( row      < lcu_height);
                bool pix_valid_eo90  =                          ((col_mult_4 + i    ) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
                bool pix_valid_eo135 = !((col == 0)&&(i==0)) && ((col_mult_4 + i + 1) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
                bool pix_valid_eo45  = !((col == 0)&&(i==0)) && ((col_mult_4 + i + 1) < lcu_width) && (row != 0) && ((row + 1) < lcu_height);
                bool pix_valid_bo    =                          ((col_mult_4 + i    ) < lcu_width) &&               ( row      < lcu_height);
                if (pix_valid_eo0)   get_stat_eo(eo0_a,   eo_c, eo0_b,   diff, stat_eo0_diff_sum,   stat_eo0_count_sum);
                if (pix_valid_eo90)  get_stat_eo(eo90_a,  eo_c, eo90_b,  diff, stat_eo90_diff_sum,  stat_eo90_count_sum);
                if (pix_valid_eo135) get_stat_eo(eo135_a, eo_c, eo135_b, diff, stat_eo135_diff_sum, stat_eo135_count_sum);
                if (pix_valid_eo45)  get_stat_eo(eo45_a,  eo_c, eo45_b,  diff, stat_eo45_diff_sum,  stat_eo45_count_sum);
                if (pix_valid_bo)    get_stat_bo(eo_c, rightShiftBit,    diff, stat_bo_diff_sum,    stat_bo_count_sum);
            }
            // ---------------------------------------------------------------------------------

            // register shift.
            for (int i = 0; i < 5; i++) { // UNROLL
#pragma HLS UNROLL
                org_subblock[0][i] = org_subblock[0][i + 4];
                org_subblock[1][i] = org_subblock[1][i + 4];
                org_subblock[2][i] = org_subblock[2][i + 4];

                rec_subblock[0][i] = rec_subblock[0][i + 4];
                rec_subblock[1][i] = rec_subblock[1][i + 4];
                rec_subblock[2][i] = rec_subblock[2][i + 4];
            }
        }
    }

    // Copy results out:
    for (int i=0; i<32; i++) { // temp, unroll
#pragma HLS UNROLL
        if (i < 5) {
            pSaoStat_EO0_diff  [i] = stat_eo0_diff_sum  [i]; pSaoStat_EO0_count  [i] = stat_eo0_count_sum  [i];
            pSaoStat_EO90_diff [i] = stat_eo90_diff_sum [i]; pSaoStat_EO90_count [i] = stat_eo90_count_sum [i];
            pSaoStat_EO135_diff[i] = stat_eo135_diff_sum[i]; pSaoStat_EO135_count[i] = stat_eo135_count_sum[i];
            pSaoStat_EO45_diff [i] = stat_eo45_diff_sum [i]; pSaoStat_EO45_count [i] = stat_eo45_count_sum [i];
        }
        pSaoStat_BO_diff[i] = stat_bo_diff_sum[i];  pSaoStat_BO_count[i] = stat_bo_count_sum[i];
    }
}
#endif 

#ifndef __SYNTHESIS__
void stat_check(int num, int* eo_diff_golden, int* eo_diff, int* eo_count_golden, int* eo_count)
{
    static int err_cnt = 0;
    for (int i = 0; i < num; i++) {
        if (eo_diff_golden[i] != eo_diff[i] || eo_count_golden[i] != eo_count[i]) {
            printf("ERROR!!!!    diff:  %d,  %d. \n", eo_diff_golden[i], eo_diff[i]);
            printf("             count: %d,  %d. \n", eo_count_golden[i], eo_count[i]);
            err_cnt++;
            printf("err cnt: %d.  \n", err_cnt);
        }
        printf("count[%d]: %d, %d.      \n", i, eo_count_golden[i], eo_count[i]);
    }
    printf("err cnt: %d.  \n", err_cnt);
}
#endif //__SYNTHESIS__

void get_sao_statistic(int bit_depth, u8 slice_sao_enable[3], int lcu_pix_height, int lcu_pix_width, 
                       //pel rec_lcu_array_y[64][64], pel rec_lcu_array_u[32][32], pel rec_lcu_array_v[32][32], 
                       pel dbkbuffer_y1[68][68],     /*pel dbkbuffer_y2[68][68],     pel dbkbuffer_y3[68][68],     pel dbkbuffer_y4[68][68],*/     pel dbkbuffer_u[36][36],     pel dbkbuffer_v[36][36],
                       pel org_lcu_array_y1[64][64], /*pel org_lcu_array_y2[64][64], pel org_lcu_array_y3[64][64], pel org_lcu_array_y4[64][64],*/ pel org_lcu_array_u[32][32], pel org_lcu_array_v[32][32],
                       int pSaoStat_YEO0_diff[32],   int pSaoStat_YEO0_count[32],   int pSaoStat_YEO90_diff[32], int pSaoStat_YEO90_count[32], 
                       int pSaoStat_YEO135_diff[32], int pSaoStat_YEO135_count[32], int pSaoStat_YEO45_diff[32], int pSaoStat_YEO45_count[32], 
                       int pSaoStat_YBO_diff[32],    int pSaoStat_YBO_count[32],    int pSaoStat_UEO0_diff[32],  int pSaoStat_UEO0_count[32], 
                       int pSaoStat_UEO90_diff[32],  int pSaoStat_UEO90_count[32],  int pSaoStat_UEO135_diff[32],int pSaoStat_UEO135_count[32], 
                       int pSaoStat_UEO45_diff[32],  int pSaoStat_UEO45_count[32],  int pSaoStat_UBO_diff[32],   int pSaoStat_UBO_count[32], 
                       int pSaoStat_VEO0_diff[32],   int pSaoStat_VEO0_count[32],   int pSaoStat_VEO90_diff[32], int pSaoStat_VEO90_count[32], 
                       int pSaoStat_VEO135_diff[32], int pSaoStat_VEO135_count[32], int pSaoStat_VEO45_diff[32], int pSaoStat_VEO45_count[32], 
                       int pSaoStat_VBO_diff[32],    int pSaoStat_VBO_count[32])
{
#pragma HLS inline off

    u8 sao_Y_enable = slice_sao_enable[0];
    u8 sao_U_enable = slice_sao_enable[1];
    u8 sao_V_enable = slice_sao_enable[2];

    // the input buffer will be orgnized as below.
    // for pixel (row, col) in [64,64]
    // dim2: 3: is line index. will be row%3
    // dim1: 171: is the buffer index;  (row / 3) * 8 + col / 8;
    // dim0: 8: is the col%8
    // will be 3 BRAMs, with data width 8x10bit, and depth 171.
    static pel org_buf_y[3][176][8];
    static pel rec_buf_y[3][176][8];
#pragma HLS ARRAY_RESHAPE variable=org_buf_y block factor=8 dim=3
#pragma HLS ARRAY_RESHAPE variable=rec_buf_y block factor=8 dim=3
    block_reform_luma(org_lcu_array_y1, dbkbuffer_y1, org_buf_y, rec_buf_y);
    static pel org_buf_u[3][88][4];
    static pel rec_buf_u[3][88][4];
    static pel org_buf_v[3][88][4];
    static pel rec_buf_v[3][88][4];
#pragma HLS ARRAY_RESHAPE variable=org_buf_u block factor=4 dim=3
#pragma HLS ARRAY_RESHAPE variable=rec_buf_u block factor=4 dim=3
#pragma HLS ARRAY_RESHAPE variable=org_buf_v block factor=4 dim=3
#pragma HLS ARRAY_RESHAPE variable=rec_buf_v block factor=4 dim=3
    block_reform_chroma(org_lcu_array_u, dbkbuffer_u, org_buf_u, rec_buf_u);
    block_reform_chroma(org_lcu_array_v, dbkbuffer_v, org_buf_v, rec_buf_v);
    getStatblk_Luma_hw(sao_Y_enable, bit_depth, lcu_pix_height, lcu_pix_width, org_buf_y, rec_buf_y,
        pSaoStat_YEO0_diff,   pSaoStat_YEO0_count,   pSaoStat_YEO90_diff, pSaoStat_YEO90_count,
        pSaoStat_YEO135_diff, pSaoStat_YEO135_count, pSaoStat_YEO45_diff, pSaoStat_YEO45_count,
        pSaoStat_YBO_diff,    pSaoStat_YBO_count);
    int lcu_height_c = lcu_pix_height >> 1;
    int lcu_width_c  = lcu_pix_width  >> 1;
    getStatblk_Chroma_hw(sao_U_enable, bit_depth, lcu_height_c, lcu_width_c, org_buf_u, rec_buf_u,
        pSaoStat_UEO0_diff,   pSaoStat_UEO0_count,   pSaoStat_UEO90_diff, pSaoStat_UEO90_count,
        pSaoStat_UEO135_diff, pSaoStat_UEO135_count, pSaoStat_UEO45_diff, pSaoStat_UEO45_count,
        pSaoStat_UBO_diff,    pSaoStat_UBO_count);
    getStatblk_Chroma_hw(sao_V_enable, bit_depth, lcu_height_c, lcu_width_c, org_buf_v, rec_buf_v,
        pSaoStat_VEO0_diff,   pSaoStat_VEO0_count,   pSaoStat_VEO90_diff, pSaoStat_VEO90_count,
        pSaoStat_VEO135_diff, pSaoStat_VEO135_count, pSaoStat_VEO45_diff, pSaoStat_VEO45_count,
        pSaoStat_VBO_diff,    pSaoStat_VBO_count);
}

static void off_sao(int lcu_idx,  int rec_sao_param_local_y_modeIdc[62],    int rec_sao_param_local_y_typeIdc[62],   int rec_sao_param_local_y_startBand[62], 
                                  int rec_sao_param_local_y_startBand2[62], int rec_sao_param_local_y_deltaband[62], int rec_sao_param_local_y_offset[62][32],
                                  int rec_sao_param_local_u_modeIdc[62],    int rec_sao_param_local_u_typeIdc[62],   int rec_sao_param_local_u_startBand[62],
                                  int rec_sao_param_local_u_startBand2[62], int rec_sao_param_local_u_deltaband[62], int rec_sao_param_local_u_offset[62][32],
                                  int rec_sao_param_local_v_modeIdc[62],    int rec_sao_param_local_v_typeIdc[62],   int rec_sao_param_local_v_startBand[62],
                                  int rec_sao_param_local_v_startBand2[62], int rec_sao_param_local_v_deltaband[62], int rec_sao_param_local_v_offset[62][32],
                                  SAOBlkParam cur_saoParam[3])
{
#pragma HLS inline off
    int i;
    int j;
    //int lcu_idx = lcu_pos % (LCU_col_num + 2);
    
    for (i = 0; i < 3; i++) 
    {
        // Current SaoParam
        cur_saoParam[i].modeIdc = ((int)SAO_MODE_OFF);
        cur_saoParam[i].typeIdc = -1;
        cur_saoParam[i].startBand = -1;
        cur_saoParam[i].startBand2 = -1;
        cur_saoParam[i].deltaband = -1;

        for (j = 0; j < 32; j++) 
        {
#pragma HLS PIPELINE
            cur_saoParam[i].offset[j] = 0;
        }
    }
    // Wr. SAO Param for AEC
    //rec_saoParam[lcu_idx][0].modeIdc = ((int)SAO_MODE_OFF);
    //rec_saoParam[lcu_idx][0].typeIdc = -1;
    //rec_saoParam[lcu_idx][0].startBand = -1;
    //rec_saoParam[lcu_idx][0].startBand2 = -1;
    //rec_saoParam[lcu_idx][0].deltaband = -1;
    //rec_saoParam[lcu_idx][1].modeIdc = ((int)SAO_MODE_OFF);
    //rec_saoParam[lcu_idx][1].typeIdc = -1;
    //rec_saoParam[lcu_idx][1].startBand = -1;
    //rec_saoParam[lcu_idx][1].startBand2 = -1;
    //rec_saoParam[lcu_idx][1].deltaband = -1;
    //rec_saoParam[lcu_idx][2].modeIdc = ((int)SAO_MODE_OFF);
    //rec_saoParam[lcu_idx][2].typeIdc = -1;
    //rec_saoParam[lcu_idx][2].startBand = -1;
    //rec_saoParam[lcu_idx][2].startBand2 = -1;
    //rec_saoParam[lcu_idx][2].deltaband = -1;

    // Save SAO Param localy, as Line Buffer.
    rec_sao_param_local_y_modeIdc[lcu_idx] = ((int)SAO_MODE_OFF);
    rec_sao_param_local_y_typeIdc[lcu_idx] = -1;
    rec_sao_param_local_u_modeIdc[lcu_idx] = ((int)SAO_MODE_OFF);
    rec_sao_param_local_u_typeIdc[lcu_idx] = -1;
    rec_sao_param_local_v_modeIdc[lcu_idx] = ((int)SAO_MODE_OFF);
    rec_sao_param_local_v_typeIdc[lcu_idx] = -1;

    for (j = 0; j < 32; j++) 
    {
#pragma HLS PIPELINE
        //rec_saoParam[lcu_idx][0].offset[j] = 0;
        //rec_saoParam[lcu_idx][1].offset[j] = 0;
        //rec_saoParam[lcu_idx][2].offset[j] = 0;
        rec_sao_param_local_y_offset[lcu_idx][j] = 0;
        rec_sao_param_local_u_offset[lcu_idx][j] = 0;
        rec_sao_param_local_v_offset[lcu_idx][j] = 0;
    }

}




static void copySAOParam_for_blk(SAOBlkParam *saopara_dst,SAOBlkParam *saopara_src)
{
#pragma HLS inline off
    int i;
    int j;
    for (i = 0; i < 3; i++) 
    {
#pragma HLS LOOP_TRIPCOUNT max=3
        saopara_dst[i].modeIdc    = saopara_src[i].modeIdc;
        saopara_dst[i].typeIdc    = saopara_src[i].typeIdc;
        saopara_dst[i].startBand  = saopara_src[i].startBand;
        saopara_dst[i].startBand2 = saopara_src[i].startBand2;
        saopara_dst[i].deltaband  = saopara_src[i].deltaband;
        for (j = 0; j < 32; j++) 
        {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=32
            saopara_dst[i] . offset[j] = saopara_src[i] . offset[j];
        }
    }
}

static void copySAOParam_for_blk_1(int *saopara_dst_modeIdc,int *saopara_dst_typeIdc,int *saopara_dst_startBand,int *saopara_dst_startBand2, int *saopara_dst_deltaband, int (*saopara_dst_offset)[32], SAOBlkParam *saopara_src)
{
#pragma HLS inline off
    int i;
    int j;
    for (i = 0; i < 3; i++) 
    {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=3
        saopara_dst_modeIdc[i]     = saopara_src[i].modeIdc;
        saopara_dst_typeIdc[i]     = saopara_src[i].typeIdc;
        saopara_dst_startBand[i]   = saopara_src[i].startBand;
        saopara_dst_startBand2[i]  = saopara_src[i].startBand2;
        saopara_dst_deltaband[i]   = saopara_src[i].deltaband;
        for (j = 0; j < 32; j++) 
        {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=32
            saopara_dst_offset[i][j] = saopara_src[i].offset[j];
        }
    }
}


static void copySAOParam_for_blk_2(SAOBlkParam *saopara_dst,int *saopara_src_modeIdc,int *saopara_src_typeIdc,int *saopara_src_startBand,int *saopara_src_startBand2,int *saopara_src_deltaband,int (*saopara_src_offset)[32])
{
#pragma HLS inline off
    int i;
    int j;
    for (i = 0; i < 3; i++) 
    {
#pragma HLS LOOP_TRIPCOUNT max=3
        saopara_dst[i].modeIdc     = saopara_src_modeIdc[i];
        saopara_dst[i].typeIdc     = saopara_src_typeIdc[i];
        saopara_dst[i].startBand   = saopara_src_startBand[i];
        saopara_dst[i].startBand2  = saopara_src_startBand2[i];
        saopara_dst[i].deltaband   = saopara_src_deltaband[i];
        for (j = 0; j < 32; j++) 
        {
#pragma  HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=32
            saopara_dst[i].offset[j] = saopara_src_offset[i][j];
        }
    }
}

static void copySAOParam_for_blk_3(int *saopara_dst_modeIdc,int *saopara_dst_typeIdc,int *saopara_dst_startBand,int *saopara_dst_startBand2,int *saopara_dst_deltaband,
    int (*saopara_dst_offset)[32],int *saopara_src_modeIdc,int *saopara_src_typeIdc,int *saopara_src_startBand,int *saopara_src_startBand2,int *saopara_src_deltaband,int (*saopara_src_offset)[32])
{
#pragma HLS inline off
    int i;
    int j;
    for (i = 0; i < 3; i++) 
    {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=3
        saopara_dst_modeIdc[i] = saopara_src_modeIdc[i];
        saopara_dst_typeIdc[i] = saopara_src_typeIdc[i];
        saopara_dst_startBand[i] = saopara_src_startBand[i];
        saopara_dst_startBand2[i] = saopara_src_startBand2[i];
        saopara_dst_deltaband[i] = saopara_src_deltaband[i];
        for (j = 0; j < 32; j++) 
        {
#pragma HLS unroll
#pragma HLS LOOP_TRIPCOUNT max=32
            saopara_dst_offset[i][j] = saopara_src_offset[i][j];
        }
    }
}


void getSaoMergeNeighbor(/*COM_INFO *info, int pic_width_scu, int pic_width_lcu,*/ int LCU_col_num, int lcu_pos, int mb_y, int mb_x, bool *MergeAvail, 
                         SAOBlkParam sao_merge_param[NUM_SAO_MERGE_TYPES][3],
                         int rec_sao_param_local_y_modeIdc[62],    int rec_sao_param_local_y_typeIdc[62],   int rec_sao_param_local_y_startBand[62],
                         int rec_sao_param_local_y_startBand2[62], int rec_sao_param_local_y_deltaband[62], int rec_sao_param_local_y_offset[62][32],
                         int rec_sao_param_local_u_modeIdc[62],    int rec_sao_param_local_u_typeIdc[62],   int rec_sao_param_local_u_startBand[62],
                         int rec_sao_param_local_u_startBand2[62], int rec_sao_param_local_u_deltaband[62], int rec_sao_param_local_u_offset[62][32],
                         int rec_sao_param_local_v_modeIdc[62],    int rec_sao_param_local_v_typeIdc[62],   int rec_sao_param_local_v_startBand[62],
                         int rec_sao_param_local_v_startBand2[62], int rec_sao_param_local_v_deltaband[62], int rec_sao_param_local_v_offset[62][32])
{
#pragma HLS inline off
    int lcu_idx_up;
    int lcu_idx_left;
    //int mb_nr;
    bool mergeup_avail;
    bool mergeleft_avail;

    // calculated at top.
    //int LCU_col_num = (info->pic_width >> info->log2_max_cuwh) + ((info->pic_width % (1 << info->log2_max_cuwh) ? 1 : 0));


    //mb_nr = mb_y * pic_width_scu + mb_x;
    //mergeup_avail = (mb_y == 0) ? 0 : is_same_patch(map_patch_idx, mb_nr, mb_nr - pic_width_scu) ? 1 : 0;
    //mergeleft_avail = (mb_x == 0) ? 0 : is_same_patch(map_patch_idx, mb_nr, mb_nr - 1) ? 1 : 0;

    mergeup_avail   = (mb_y == 0) ? false : true;
    mergeleft_avail = (mb_x == 0) ? false : true;

    if (mergeleft_avail) {
        // CIRCULAR_BUFFER
        //sao_left_param = rec_saoParam[LCU_col_num];
        //sao_left_param = rec_saoParam[(lcu_pos - 1 + LCU_col_num + 2) % (LCU_col_num + 2)];
        //copySAOParam_for_blk(sao_merge_param[SAO_MERGE_LEFT], sao_left_param);
        lcu_idx_left = (lcu_pos - 1 + LCU_col_num + 2) % (LCU_col_num + 2);
        sao_merge_param[SAO_MERGE_LEFT][0].modeIdc    = rec_sao_param_local_y_modeIdc[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][0].typeIdc    = rec_sao_param_local_y_typeIdc[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][0].startBand  = rec_sao_param_local_y_startBand[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][0].startBand2 = rec_sao_param_local_y_startBand2[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][0].deltaband  = rec_sao_param_local_y_deltaband[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][1].modeIdc    = rec_sao_param_local_u_modeIdc[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][1].typeIdc    = rec_sao_param_local_u_typeIdc[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][1].startBand  = rec_sao_param_local_u_startBand[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][1].startBand2 = rec_sao_param_local_u_startBand2[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][1].deltaband  = rec_sao_param_local_u_deltaband[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][2].modeIdc    = rec_sao_param_local_v_modeIdc[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][2].typeIdc    = rec_sao_param_local_v_typeIdc[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][2].startBand  = rec_sao_param_local_v_startBand[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][2].startBand2 = rec_sao_param_local_v_startBand2[lcu_idx_left];
        sao_merge_param[SAO_MERGE_LEFT][2].deltaband  = rec_sao_param_local_v_deltaband[lcu_idx_left];
        for (int j = 0; j < 32; j++)
        {
#pragma HLS unroll 
            sao_merge_param[SAO_MERGE_LEFT][0].offset[j] = rec_sao_param_local_y_offset[lcu_idx_left][j];
            sao_merge_param[SAO_MERGE_LEFT][1].offset[j] = rec_sao_param_local_u_offset[lcu_idx_left][j];
            sao_merge_param[SAO_MERGE_LEFT][2].offset[j] = rec_sao_param_local_v_offset[lcu_idx_left][j];
        }
    }
    if (mergeup_avail) {
        // CIRCULAR_BUFFER
        //sao_up_param = rec_saoParam[1];
        //sao_up_param = rec_saoParam[(lcu_pos - LCU_col_num + LCU_col_num + 2) % (LCU_col_num + 2)];
        //copySAOParam_for_blk(sao_merge_param[SAO_MERGE_ABOVE], sao_up_param);
        lcu_idx_up = (lcu_pos - LCU_col_num + LCU_col_num + 2) % (LCU_col_num + 2);
        sao_merge_param[SAO_MERGE_ABOVE][0].modeIdc    = rec_sao_param_local_y_modeIdc[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][0].typeIdc    = rec_sao_param_local_y_typeIdc[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][0].startBand  = rec_sao_param_local_y_startBand[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][0].startBand2 = rec_sao_param_local_y_startBand2[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][0].deltaband  = rec_sao_param_local_y_deltaband[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][1].modeIdc    = rec_sao_param_local_u_modeIdc[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][1].typeIdc    = rec_sao_param_local_u_typeIdc[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][1].startBand  = rec_sao_param_local_u_startBand[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][1].startBand2 = rec_sao_param_local_u_startBand2[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][1].deltaband  = rec_sao_param_local_u_deltaband[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][2].modeIdc    = rec_sao_param_local_v_modeIdc[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][2].typeIdc    = rec_sao_param_local_v_typeIdc[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][2].startBand  = rec_sao_param_local_v_startBand[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][2].startBand2 = rec_sao_param_local_v_startBand2[lcu_idx_up];
        sao_merge_param[SAO_MERGE_ABOVE][2].deltaband  = rec_sao_param_local_v_deltaband[lcu_idx_up];
        for (int j = 0; j < 32; j++) {
#pragma HLS unroll
            sao_merge_param[SAO_MERGE_ABOVE][0].offset[j] = rec_sao_param_local_y_offset[lcu_idx_up][j];
            sao_merge_param[SAO_MERGE_ABOVE][1].offset[j] = rec_sao_param_local_u_offset[lcu_idx_up][j];
            sao_merge_param[SAO_MERGE_ABOVE][2].offset[j] = rec_sao_param_local_v_offset[lcu_idx_up][j];
        }
    }
    MergeAvail[SAO_MERGE_LEFT] = mergeleft_avail;
    MergeAvail[SAO_MERGE_ABOVE] = mergeup_avail;
}

static SAO_BIG_SIZE distortion_cal(SAO_BIG_SIZE count, int offset, int diff)
{
#pragma HLS inline
    return (count * (SAO_BIG_SIZE)offset - (diff * (SAO_BIG_SIZE)2)) * (SAO_BIG_SIZE)offset;
}

static SAO_BIG_SIZE get_distortion_eo(int* pSaoStat_diff, int* pSaoStat_count, int* sao_cur_param_offset)
{
#pragma HLS inline off
    SAO_BIG_SIZE dist = (SAO_BIG_SIZE)0;
    LOOP_GET_DIST_EO: for (int classIdc = 0; classIdc < NUM_SAO_EO_CLASSES; classIdc++) {
#pragma  HLS PIPELINE
        dist += distortion_cal((SAO_BIG_SIZE)(pSaoStat_count[classIdc]), sao_cur_param_offset[classIdc], pSaoStat_diff[classIdc]);
    }
    return dist;
}
static SAO_BIG_SIZE get_distortion_bo(int pSaoStat_diff[32], int pSaoStat_count[32], int(*sao_cur_param_offset)[32])
{
#pragma HLS inline off
    SAO_BIG_SIZE dist = 0;
    LOOP_GET_DIST_BO: for (int bandIdx = 0; bandIdx < 32; bandIdx++) {
#pragma  HLS PIPELINE
        dist += distortion_cal((SAO_BIG_SIZE)pSaoStat_count[bandIdx], (*sao_cur_param_offset)[bandIdx], pSaoStat_diff[bandIdx]);
    }
    return dist;
}
static SAO_BIG_SIZE get_distortion_2(int type, int *pSaoStat_diff, int *pSaoStat_count, int (*sao_cur_param_offset)[32])
{
#pragma HLS inline off
//#pragma HLS array_partition variable=pSaoStat_diff complete dim=2
//#pragma HLS array_partition variable=pSaoStat_count complete dim=2
//#pragma HLS array_partition variable=sao_cur_param_offset complete dim=2
    int classIdc;
    int bandIdx;
    SAO_BIG_SIZE dist = 0;
    switch(type){
        case (int )SAO_TYPE_EO_0:
        ;
        case (int )SAO_TYPE_EO_90:
        ;
        case (int )SAO_TYPE_EO_135:
        ;
        case (int )SAO_TYPE_EO_45: {
            for (classIdc = 0; classIdc < ((int )NUM_SAO_EO_CLASSES); classIdc++) {
#pragma  HLS PIPELINE
                dist += distortion_cal((SAO_BIG_SIZE)pSaoStat_count[classIdc], (*sao_cur_param_offset)[classIdc], pSaoStat_diff[classIdc]);
            }
        }
        break; 
        case (int )SAO_TYPE_BO: {
            for (classIdc = 0; classIdc < 32; classIdc++) {
#pragma  HLS PIPELINE
                bandIdx = classIdc % (1 << 5);
                dist += distortion_cal((SAO_BIG_SIZE)pSaoStat_count[bandIdx], (*sao_cur_param_offset)[bandIdx], pSaoStat_diff[bandIdx]);
            }
        }
        break; 
        default: {
            printf("Not a supported type");
            //0?((void )0) : __assert_HLS("0","../separate/com_sao.c",(unsigned int )110,__FUNCTION__);
            exit(- 1);
        }
    }
    return dist;

}

static void RDO_sao_mergeflag_AEC(int value1, int value2, int *pbin_count)
{
#pragma HLS inline off
    int act_sym = value1;
    int act_ctx = value2;
  
    if (act_ctx == 1) 
    {
        ( *pbin_count)++;
    }
    else if (act_ctx == 2) 
    {
        ( *pbin_count)++;
        if (act_sym != 1) 
        {
            ( *pbin_count)++;
        }
    }
}

static void RDO_sao_mergeflag_LCU(bool mergeleft_avail, bool mergeup_avail, int *saoBlkParam_modeIdc, int *saoBlkParam_typeIdc, int *pbin_count)
{
#pragma HLS inline off
    bool MergeLeft = 0;
    bool MergeUp = 0;
    int value1 = 0;
    int value2 = 0;
    if (mergeleft_avail) {
        MergeLeft = ((*saoBlkParam_modeIdc == (int )SAO_MODE_MERGE) && (*saoBlkParam_typeIdc == (int )SAO_MERGE_LEFT));
        if (MergeLeft) {
            value1 = 1;
        }
        else {
            value1 = 0;
        }
    }
    if (mergeup_avail && !MergeLeft) {
        MergeUp = ((*saoBlkParam_modeIdc == (int )SAO_MODE_MERGE) && (*saoBlkParam_typeIdc == (int )SAO_MERGE_ABOVE));
        if (MergeUp) {
            value1 = 1 + (int)mergeleft_avail;
        }
        else {
            value1 = 0;
        }
    }
    value2 = (int)mergeleft_avail + (int)mergeup_avail;
    //RDO_sao_mergeflag_AEC(value1, value2, ctx);
    RDO_sao_mergeflag_AEC(value1, value2, pbin_count);
}


SAO_BIG_SIZE sao_merge_dist(int type, 
    int *pSaoStat_EO0_diff,   int *pSaoStat_EO0_count,
    int *pSaoStat_EO90_diff,  int *pSaoStat_EO90_count,
    int *pSaoStat_EO135_diff, int *pSaoStat_EO135_count,
    int *pSaoStat_EO45_diff,  int *pSaoStat_EO45_count,
    int *pSaoStat_BO_diff,    int *pSaoStat_BO_count,
    int (*temp_sao_param_offset)[32], SAO_BIG_SIZE sao_labmda)
{
    SAO_BIG_SIZE dist = 0;

    switch  (type )
    {
    case (SAO_TYPE_EO_0):
         dist = get_distortion_2(type, pSaoStat_EO0_diff,   pSaoStat_EO0_count,   temp_sao_param_offset);
        break;
    case (SAO_TYPE_EO_90):
         dist = get_distortion_2(type, pSaoStat_EO90_diff,  pSaoStat_EO90_count,  temp_sao_param_offset);
        break;
    case (SAO_TYPE_EO_135):
         dist = get_distortion_2(type, pSaoStat_EO135_diff, pSaoStat_EO135_count, temp_sao_param_offset);
        break;
    case (SAO_TYPE_EO_45):
         dist = get_distortion_2(type, pSaoStat_EO45_diff,  pSaoStat_EO45_count,  temp_sao_param_offset);
        break;
    case (SAO_TYPE_BO):
         dist = get_distortion_2(type, pSaoStat_BO_diff,    pSaoStat_BO_count,    temp_sao_param_offset);
        break;
    default:
        break;
    }
     return (dist << LAMBDA_SCALE) / sao_labmda;
}



static SAO_BIG_SIZE sao_merge_param_rdo(/*int pic_width1,*/ int log2_max_cuwh, /*int pic_width_in_scu,int pic_width_in_lcu,*/ int LCU_col_num, /*int *bin_count,*/ unsigned char slice_sao_enable[3],int lcu_pos,int pix_y,int pix_x, SAO_BIG_SIZE *sao_labmda,
    int *sao_cur_param_modeIdc,     int *sao_cur_param_typeIdc,     int *sao_cur_param_startBand,     int *sao_cur_param_startBand2,     int *sao_cur_param_deltaband,     int (*sao_cur_param_offset)[32],
    int *rec_sao_cur_param_modeIdc, int *rec_sao_cur_param_typeIdc, //int *rec_sao_cur_param_startBand, int *rec_sao_cur_param_startBand2, int *rec_sao_cur_param_deltaband, int (*rec_sao_cur_param_offset)[32],
    int rec_sao_param_local_y_modeIdc[62],    int rec_sao_param_local_y_typeIdc[62],   int rec_sao_param_local_y_startBand[62],
    int rec_sao_param_local_y_startBand2[62], int rec_sao_param_local_y_deltaband[62], int rec_sao_param_local_y_offset[62][32],
    int rec_sao_param_local_u_modeIdc[62],    int rec_sao_param_local_u_typeIdc[62],   int rec_sao_param_local_u_startBand[62],
    int rec_sao_param_local_u_startBand2[62], int rec_sao_param_local_u_deltaband[62], int rec_sao_param_local_u_offset[62][32],
    int rec_sao_param_local_v_modeIdc[62],    int rec_sao_param_local_v_typeIdc[62],   int rec_sao_param_local_v_startBand[62],
    int rec_sao_param_local_v_startBand2[62], int rec_sao_param_local_v_deltaband[62], int rec_sao_param_local_v_offset[62][32],
    bool *MergeLeftAvail, bool *MergeUpAvail, 
    int *pSaoStat_YEO0_diff,   int *pSaoStat_YEO0_count, 
    int *pSaoStat_YEO90_diff,  int *pSaoStat_YEO90_count, 
    int *pSaoStat_YEO135_diff, int *pSaoStat_YEO135_count, 
    int *pSaoStat_YEO45_diff,  int *pSaoStat_YEO45_count, 
    int *pSaoStat_YBO_diff,    int *pSaoStat_YBO_count,
    int *pSaoStat_UEO0_diff,   int *pSaoStat_UEO0_count, 
    int *pSaoStat_UEO90_diff,  int *pSaoStat_UEO90_count, 
    int *pSaoStat_UEO135_diff, int *pSaoStat_UEO135_count, 
    int *pSaoStat_UEO45_diff,  int *pSaoStat_UEO45_count, 
    int *pSaoStat_UBO_diff,    int *pSaoStat_UBO_count, 
    int *pSaoStat_VEO0_diff,   int *pSaoStat_VEO0_count, 
    int *pSaoStat_VEO90_diff,  int *pSaoStat_VEO90_count, 
    int *pSaoStat_VEO135_diff, int *pSaoStat_VEO135_count, 
    int *pSaoStat_VEO45_diff,  int *pSaoStat_VEO45_count, 
    int *pSaoStat_VBO_diff,    int *pSaoStat_VBO_count)
{
#pragma HLS inline off
    static  SAOBlkParam merge_candidate[NUM_SAO_MERGE_TYPES][3];
#pragma HLS array_partition variable=merge_candidate complete dim=0
    static int temp_sao_param_offset[3][32];
    int temp_sao_param_deltaband[3];
    int temp_sao_param_startBand2[3];
    int temp_sao_param_startBand[3];
    int temp_sao_param_typeIdc[3];
    int temp_sao_param_modeIdc[3];
#pragma HLS array_partition variable=temp_sao_param_offset complete dim=0
#pragma HLS array_partition variable=temp_sao_param_deltaband complete dim=0
#pragma HLS array_partition variable=temp_sao_param_startBand2 complete dim=0
#pragma HLS array_partition variable=temp_sao_param_startBand complete dim=0
#pragma HLS array_partition variable=temp_sao_param_typeIdc complete dim=0
#pragma HLS array_partition variable=temp_sao_param_modeIdc complete dim=0
    SAO_BIG_SIZE sao_labmda_y = sao_labmda[0];
    SAO_BIG_SIZE sao_labmda_u = sao_labmda[1];
    SAO_BIG_SIZE sao_labmda_v = sao_labmda[2];

    // 2021.03.18, liaozhx
    //int mb_x = pix_x >> 2;
    //int mb_y = pix_y >> 2;
    int mb_x = pix_x >> log2_max_cuwh;
    int mb_y = pix_y >> log2_max_cuwh;

    bool merge_avail[2];
#pragma HLS array_partition variable=merge_avail complete dim=0
    int mergeIdx;
    int compIdx;
  
    SAO_BIG_SIZE curdist = 0;
    int currate;
    SAO_BIG_SIZE curcost;
    SAO_BIG_SIZE mincost;
    mincost = (SAO_BIG_SIZE)SAO_MAX_BIG; //9223372036854775807 = MAX_S64
  
    int bin_count = 0;

    bin_count = 0;

    for (int i = 0; i < 3; i++)
    {
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=3
        rec_sao_cur_param_modeIdc[i] = 0;
        rec_sao_cur_param_typeIdc[i] = 0;
    }

    for(int i = 0; i < 3; i++) {
#pragma HLS pipeline
        for(int j = 0; j < 32; j++) {
#pragma HLS unroll
            temp_sao_param_offset[i][j] = 0;
        }
    }

    for(int i = 0; i < NUM_SAO_MERGE_TYPES; i++) {
#pragma HLS pipeline
        for(int j = 0; j < 3; j++) {
#pragma HLS unroll
            for(int k = 0; k < 32; k++) {
#pragma HLS unroll
                merge_candidate[i][j].offset[k] = 0;
            }
        }
    }
#if sao_print
	FILE *file_1;
	file_1 = fopen("rec_saoParam.txt", "w+");
	for (int i = 0; i < 62; i++)
	{
		fprintf(file_1, "lcu_idx=%d\n", i);
		fprintf(file_1, "%d %d %d %d %d\n", rec_sao_param_local_y_deltaband[i], rec_sao_param_local_y_modeIdc[i], rec_sao_param_local_y_startBand[i], rec_sao_param_local_y_startBand2[i], rec_sao_param_local_y_typeIdc[i]);
		fprintf(file_1, "%d %d %d %d %d\n", rec_sao_param_local_u_deltaband[i], rec_sao_param_local_u_modeIdc[i], rec_sao_param_local_u_startBand[i], rec_sao_param_local_u_startBand2[i], rec_sao_param_local_u_typeIdc[i]);
		fprintf(file_1, "%d %d %d %d %d\n", rec_sao_param_local_v_deltaband[i], rec_sao_param_local_v_modeIdc[i], rec_sao_param_local_v_startBand[i], rec_sao_param_local_v_startBand2[i], rec_sao_param_local_v_typeIdc[i]);
	}
	for (int i = 0; i < 62; i++)
	{
		fprintf(file_1, "lcu_idx=%d\n",i);

	for (int j = 0; j < 32; j++)
	{
		fprintf(file_1, "%d ", rec_sao_param_local_y_offset[i][j]);
	}
	fprintf(file_1, "\n");
	for (int j = 0; j < 32; j++)
	{
		fprintf(file_1, "%d ", rec_sao_param_local_u_offset[i][j]);
	}
	fprintf(file_1, "\n");
	for (int j = 0; j < 32; j++)
	{
		fprintf(file_1, "%d ", rec_sao_param_local_v_offset[i][j]);
	}
	fprintf(file_1, "\n");
	}
	fclose(file_1);
#endif
    getSaoMergeNeighbor(LCU_col_num, lcu_pos, mb_y, mb_x, merge_avail, merge_candidate, 
                        rec_sao_param_local_y_modeIdc, rec_sao_param_local_y_typeIdc, rec_sao_param_local_y_startBand, rec_sao_param_local_y_startBand2, rec_sao_param_local_y_deltaband, rec_sao_param_local_y_offset,
                        rec_sao_param_local_u_modeIdc, rec_sao_param_local_u_typeIdc, rec_sao_param_local_u_startBand, rec_sao_param_local_u_startBand2, rec_sao_param_local_u_deltaband, rec_sao_param_local_u_offset,
                        rec_sao_param_local_v_modeIdc, rec_sao_param_local_v_typeIdc, rec_sao_param_local_v_startBand, rec_sao_param_local_v_startBand2, rec_sao_param_local_v_deltaband, rec_sao_param_local_v_offset);
    *MergeLeftAvail = merge_avail[SAO_MERGE_LEFT];
    *MergeUpAvail   = merge_avail[SAO_MERGE_ABOVE];

#if sao_print
	FILE *file_2;
	file_2 = fopen("merge_candidate.txt", "w+");
	fprintf(file_2, "%d %d %d %d %d\n", merge_candidate[0][0].deltaband, merge_candidate[0][0].modeIdc, merge_candidate[0][0].startBand, merge_candidate[0][0].startBand2, merge_candidate[0][0].typeIdc);
	fprintf(file_2, "%d %d %d %d %d\n", merge_candidate[0][1].deltaband, merge_candidate[0][1].modeIdc, merge_candidate[0][1].startBand, merge_candidate[0][1].startBand2, merge_candidate[0][1].typeIdc);
	fprintf(file_2, "%d %d %d %d %d\n", merge_candidate[0][2].deltaband, merge_candidate[0][2].modeIdc, merge_candidate[0][2].startBand, merge_candidate[0][2].startBand2, merge_candidate[0][2].typeIdc);
	for (int i = 0; i < 32; i++)
	{
		fprintf(file_2, "%d ", merge_candidate[0][0].offset[i]);
	}
	fprintf(file_2, "\n");
	for (int i = 0; i < 32; i++)
	{
		fprintf(file_2, "%d ", merge_candidate[0][1].offset[i]);
	}
	fprintf(file_2, "\n");
	for (int i = 0; i < 32; i++)
	{
		fprintf(file_2, "%d ", merge_candidate[0][2].offset[i]);
	}
	fprintf(file_2, "\n");
	fclose(file_2);
#endif

    for (mergeIdx = 0; mergeIdx < ((int )NUM_SAO_MERGE_TYPES); mergeIdx++) 
    {
		bin_count = 0;
#pragma HLS LOOP_TRIPCOUNT max=2
//#pragma HLS pipeline
        if (merge_avail[mergeIdx]) 
        {
            for (compIdx = 0; compIdx < 3; compIdx++) 
            {
#pragma HLS unroll
                if (!slice_sao_enable[compIdx]) 
                {
                    merge_candidate[mergeIdx][compIdx].modeIdc = ((int )SAO_MODE_OFF);
                }
            }
            

            curdist = 0;
            copySAOParam_for_blk_1(temp_sao_param_modeIdc,temp_sao_param_typeIdc,temp_sao_param_startBand,temp_sao_param_startBand2,temp_sao_param_deltaband,temp_sao_param_offset, merge_candidate[mergeIdx]);
			/*
			for (int i = 0; i < 32; i++)
			{
				printf("first\n");
				printf("temp_sao_param_offset[0][%d]=%d ",i, temp_sao_param_offset[0][i]);
				printf("\n");
			}
			*/
            // --- Calc Merged Distortion --------------------------------------------------------------------------------------------------------------------------------

            // Y component
            //type = merge_candidate[mergeIdx][0].typeIdc;
            //mode = merge_candidate[mergeIdx][0].modeIdc;
            temp_sao_param_typeIdc[0] = mergeIdx;
            temp_sao_param_modeIdc[0] = ((int)SAO_MODE_MERGE);

            if (merge_candidate[mergeIdx][0].modeIdc != 0)  // If merged SAO Mode is NOT SAO_MODE_OFF
            {
                curdist += sao_merge_dist(merge_candidate[mergeIdx][0].typeIdc, 
                                          pSaoStat_YEO0_diff,   pSaoStat_YEO0_count,   pSaoStat_YEO90_diff, pSaoStat_YEO90_count,
                                          pSaoStat_YEO135_diff, pSaoStat_YEO135_count, pSaoStat_YEO45_diff, pSaoStat_YEO45_count, pSaoStat_YBO_diff,    pSaoStat_YBO_count,
                                          &temp_sao_param_offset[0], sao_labmda_y);
            }

            // U component
            temp_sao_param_typeIdc[1] = mergeIdx;
            temp_sao_param_modeIdc[1] = ((int)SAO_MODE_MERGE);

            if (merge_candidate[mergeIdx][1].modeIdc != 0)
            {
                curdist += sao_merge_dist(merge_candidate[mergeIdx][1].typeIdc, 
                                          pSaoStat_UEO0_diff,   pSaoStat_UEO0_count,   pSaoStat_UEO90_diff, pSaoStat_UEO90_count,
                                          pSaoStat_UEO135_diff, pSaoStat_UEO135_count, pSaoStat_UEO45_diff, pSaoStat_UEO45_count, pSaoStat_UBO_diff,    pSaoStat_UBO_count,
                                          &temp_sao_param_offset[1], sao_labmda_u);
            }

            // V component
            temp_sao_param_typeIdc[2] = mergeIdx;
            temp_sao_param_modeIdc[2] = ((int)SAO_MODE_MERGE);

            if (merge_candidate[mergeIdx][2].modeIdc != 0)
            {
                curdist += sao_merge_dist(merge_candidate[mergeIdx][2].typeIdc, 
                                          pSaoStat_VEO0_diff,   pSaoStat_VEO0_count,   pSaoStat_VEO90_diff, pSaoStat_VEO90_count,
                                          pSaoStat_VEO135_diff, pSaoStat_VEO135_count, pSaoStat_VEO45_diff, pSaoStat_VEO45_count, pSaoStat_VBO_diff,    pSaoStat_VBO_count,
                                          &temp_sao_param_offset[2], sao_labmda_v);
            }

            /*for (compIdx = 0; compIdx < 3; compIdx++) 
            {
                type = merge_candidate[mergeIdx][compIdx].typeIdc;
                mode = merge_candidate[mergeIdx][compIdx].modeIdc;
                temp_sao_param_typeIdc[compIdx] = mergeIdx;
                temp_sao_param_modeIdc[compIdx] = ((int )SAO_MODE_MERGE);
                if (mode != ((int )SAO_MODE_OFF)) 
                {
                    if (compIdx == 0) 
                    {
                        switch(type)
                        {
                            case (int )SAO_TYPE_EO_0:
                                curdist += get_distortion_2(type, pSaoStat_YEO0_diff,   pSaoStat_YEO0_count,  &temp_sao_param_offset[0]) * ((long long )1000) / sao_labmda_y;
                            break;                             
                            case (int )SAO_TYPE_EO_90:        
                                curdist += get_distortion_2(type, pSaoStat_YEO90_diff,  pSaoStat_YEO90_count, &temp_sao_param_offset[0]) * ((long long )1000) / sao_labmda_y;
                            break;                             
                            case (int )SAO_TYPE_EO_135:        
                                curdist += get_distortion_2(type, pSaoStat_YEO135_diff, pSaoStat_YEO135_count,&temp_sao_param_offset[0]) * ((long long )1000) / sao_labmda_y;
                            break;                             
                            case (int )SAO_TYPE_EO_45:        
                                curdist += get_distortion_2(type, pSaoStat_YEO45_diff,  pSaoStat_YEO45_count, &temp_sao_param_offset[0]) * ((long long )1000) / sao_labmda_y;
                            break;                                                         
                            case (int )SAO_TYPE_BO:                                        
                                curdist += get_distortion_2(type, pSaoStat_YBO_diff,    pSaoStat_YBO_count,   &temp_sao_param_offset[0]) * ((long long )1000) / sao_labmda_y;
                            break; 
                        }
                    }
                    else if (compIdx == 1) 
                    {
                        switch(type)
                        {
                            case (int )SAO_TYPE_EO_0:
                                curdist += get_distortion_2(type,pSaoStat_UEO0_diff,  pSaoStat_UEO0_count,  &temp_sao_param_offset[1]) * ((long long )1000) / sao_labmda_u;
                            break; 
                            case (int )SAO_TYPE_EO_90:
                                curdist += get_distortion_2(type,pSaoStat_UEO90_diff, pSaoStat_UEO90_count, &temp_sao_param_offset[1]) * ((long long )1000) / sao_labmda_u;
                            break; 
                            case (int )SAO_TYPE_EO_135:
                                curdist += get_distortion_2(type,pSaoStat_UEO135_diff,pSaoStat_UEO135_count,&temp_sao_param_offset[1]) * ((long long )1000) / sao_labmda_u;
                            break; 
                            case (int )SAO_TYPE_EO_45:
                                curdist += get_distortion_2(type,pSaoStat_UEO45_diff, pSaoStat_UEO45_count, &temp_sao_param_offset[1]) * ((long long )1000) / sao_labmda_u;
                            break; 
                            case (int )SAO_TYPE_BO:
                                curdist += get_distortion_2(type,pSaoStat_UBO_diff,   pSaoStat_UBO_count,   &temp_sao_param_offset[1]) * ((long long )1000) / sao_labmda_u;
                            break; 
                        }
                    }
                    else 
                    {
                        switch(type)
                        {
                            case (int )SAO_TYPE_EO_0:
                                curdist += get_distortion_2(type,pSaoStat_VEO0_diff,  pSaoStat_VEO0_count,  &temp_sao_param_offset[2]) * ((long long )1000) / sao_labmda_v;
                            break; 
                            case (int )SAO_TYPE_EO_90:
                                curdist += get_distortion_2(type,pSaoStat_VEO90_diff, pSaoStat_VEO90_count, &temp_sao_param_offset[2]) * ((long long )1000) / sao_labmda_v;
                            break; 
                            case (int )SAO_TYPE_EO_135:
                                curdist += get_distortion_2(type,pSaoStat_VEO135_diff,pSaoStat_VEO135_count,&temp_sao_param_offset[2]) * ((long long )1000) / sao_labmda_v;
                            break; 
                            case (int )SAO_TYPE_EO_45:
                                curdist += get_distortion_2(type,pSaoStat_VEO45_diff, pSaoStat_VEO45_count, &temp_sao_param_offset[2]) * ((long long )1000) / sao_labmda_v;
                            break; 
                            case (int )SAO_TYPE_BO:
                                curdist += get_distortion_2(type,pSaoStat_VBO_diff,   pSaoStat_VBO_count,   &temp_sao_param_offset[2]) * ((long long )1000) / sao_labmda_v;
                            break; 
                        }
                    }
                }
            }*/

            RDO_sao_mergeflag_LCU( *MergeLeftAvail, *MergeUpAvail, temp_sao_param_modeIdc, temp_sao_param_typeIdc, &bin_count);//0cycle
            currate =  bin_count;
            curcost = (SAO_BIG_SIZE)currate + curdist;

            if (curcost < mincost) 
            {
                mincost = curcost;
                copySAOParam_for_blk_3(sao_cur_param_modeIdc, sao_cur_param_typeIdc, sao_cur_param_startBand, sao_cur_param_startBand2, sao_cur_param_deltaband, sao_cur_param_offset,
                                       temp_sao_param_modeIdc,temp_sao_param_typeIdc,temp_sao_param_startBand,temp_sao_param_startBand2,temp_sao_param_deltaband,temp_sao_param_offset);
                //copySAOParam_for_blk_1(rec_sao_cur_param_modeIdc,rec_sao_cur_param_typeIdc,rec_sao_cur_param_startBand,rec_sao_cur_param_startBand2,rec_sao_cur_param_deltaband,rec_sao_cur_param_offset,merge_candidate[mergeIdx]);
                for (int i = 0; i < 3; i++)
                {
#pragma HLS LOOP_TRIPCOUNT max=3
#pragma HLS unroll
                    rec_sao_cur_param_modeIdc[i] = merge_candidate[mergeIdx][i].modeIdc;
                    rec_sao_cur_param_typeIdc[i] = merge_candidate[mergeIdx][i].typeIdc;
                }
            }
        }
    }
    return mincost;
}


static void RDO_sao_mode_AEC(int value1, int *pbin_count)
{
#pragma HLS inline off
    int act_sym = value1;
    if (act_sym == 0) {
        *pbin_count += 1;
    } else {
        *pbin_count += 2;
    }
}

static void RDO_sao_mode(int *saoBlkParam_modeIdc,int *saoBlkParam_typeIdc,int *pbin_count)
{
#pragma HLS inline off
    int value1;

    if ( *saoBlkParam_modeIdc == ((int )SAO_MODE_OFF)) {
        value1 = 0;
    }
    else {
        if ( *saoBlkParam_typeIdc == ((int )SAO_TYPE_BO)) {
            value1 = 1;
        }
        else {
            value1 = 3;
        }
    }
    RDO_sao_mode_AEC(value1, pbin_count);
}
static void RDO_sao_mode_new_eo(int* pbin_count)
{
#pragma HLS inline off
    RDO_sao_mode_AEC(3, pbin_count);
}
static void RDO_sao_mode_new_bo(int* pbin_count)
{
#pragma HLS inline off
    RDO_sao_mode_AEC(1, pbin_count);
}

static int saoclip[NUM_SAO_OFFSET][3] = {{(- 1), (6), (7)}, {(0), (1), (1)}, {(0), (0), (0)}, {(- 1), (0), (1)}, {(- 6), (1), (7)}, {(- 7), (7), (7)}};

static int offset_estimation(int typeIdx,int classIdx, SAO_BIG_SIZE lambda,int offset_ori,int count,int diff, SAO_BIG_SIZE *bestCost,int EO_OFFSET_MAP[8])
{
#pragma HLS inline
    int cur_offset = offset_ori;
    int offset_best = 0;
    int lower_bd;
    int upper_bd;
    int Th;
    int temp_offset;
    int start_offset;
    int end_offset;
    int temprate;
    SAO_BIG_SIZE tempdist;
    SAO_BIG_SIZE tempcost;
    SAO_BIG_SIZE mincost;
    SAO_BIG_SIZE lambda_s64 = lambda;
    int offset_type;

    if (typeIdx == ((int )SAO_TYPE_BO)) {
        offset_type = ((int )SAO_CLASS_BO);
    }
    else {
        offset_type = classIdx;
    }

    lower_bd = saoclip[offset_type][0];
    upper_bd = saoclip[offset_type][1];
    Th = saoclip[offset_type][2];

    {// Inlined function: SAOClip() 
        char rose__temp = 1;
        int rose_temp;
        if (upper_bd < cur_offset) {
            rose_temp = upper_bd;
        }
        else {
            rose_temp = cur_offset;
        }
        rose__temp = ((char )((bool )(lower_bd > rose_temp)));
        if ((bool )rose__temp) {
            cur_offset = lower_bd;
        }
        else {
            if (upper_bd < cur_offset) {
                cur_offset = upper_bd;
            }
            else {
                cur_offset = cur_offset;
            }
        }
    }

    if (typeIdx == ((int )SAO_TYPE_BO)) {
        if (cur_offset >= 0) {
            start_offset = 0;
        }
        else {
            start_offset = cur_offset;
        }
        if (cur_offset >= 0) {
            end_offset = cur_offset;
        }
        else {
            end_offset = 0;
        }
    }
    else {
        //typeIdx >= ((int )SAO_TYPE_EO_0) && typeIdx <= ((int )SAO_TYPE_EO_45)?((void )0) : ((void )(__assert_HLS("typeIdx >= SAO_TYPE_EO_0 && typeIdx <= SAO_TYPE_EO_45","../separate/enc_mode.c",(unsigned int )6518,__FUNCTION__)));
        switch(classIdx){
        case (int )SAO_CLASS_EO_FULL_VALLEY:
            start_offset = - 1;
            if (cur_offset > 1) {
                end_offset = cur_offset;
            }
            else {
                end_offset = 1;
            }
            break; 
        case (int )SAO_CLASS_EO_HALF_VALLEY:
            start_offset = 0;
            end_offset = 1;
            break; 
        case (int )SAO_CLASS_EO_HALF_PEAK:
            start_offset = - 1;
            end_offset = 0;
            break; 
        case (int )SAO_CLASS_EO_FULL_PEAK:
            if (cur_offset < - 1) {
                start_offset = cur_offset;
            }
            else {
                start_offset = - 1;
            }
            end_offset = 1;
            break; 
        default:
            printf("Not a supported SAO mode\n");
            //0?((void )0) : __assert_HLS("0","../separate/enc_mode.c",(unsigned int )6539,__FUNCTION__);
            exit(- 1);
        }
    }

    mincost = (SAO_BIG_SIZE)SAO_MAX_BIG;

    for (temp_offset = start_offset; temp_offset <= end_offset; temp_offset += 1) {
#pragma  HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=8
        int offset = temp_offset;

        //offset >= - 7 && offset <= 7?((void )0) : __assert_HLS("offset >= -7 && offset <= 7","../separate/enc_mode.c",(unsigned int )6549,__FUNCTION__);
        if (typeIdx == ((int )SAO_TYPE_BO)) {
            //offset_type == ((int )SAO_CLASS_BO)?((void )0) : __assert_HLS("offset_type == SAO_CLASS_BO","../separate/enc_mode.c",(unsigned int )6552,__FUNCTION__);
            temprate = abs(offset);
            if (temprate) {
                temprate = temprate + 1;
            }
            else {
                temprate = 0;
            }
        }
        else {
            if (classIdx == ((int )SAO_CLASS_EO_HALF_VALLEY) || classIdx == ((int )SAO_CLASS_EO_HALF_PEAK)) {
                temprate = abs(offset);
            }
            else {
                int rose_temp_0;
                if (classIdx == ((int )SAO_CLASS_EO_FULL_VALLEY)) {
                    rose_temp_0 = offset;
                }
                else {
                    rose_temp_0 = -offset;
                }
                temprate = (&EO_OFFSET_MAP[1])[rose_temp_0];
            }
        }

        if (temprate == Th) {
            temprate = temprate;
        }
        else {
            temprate = temprate + 1;
        }

        tempdist = distortion_cal((SAO_BIG_SIZE)count, temp_offset, diff);
        tempcost = tempdist + ((lambda_s64 * ((SAO_BIG_SIZE)temprate) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE);

        if (tempcost < mincost) {
            mincost = tempcost;
            offset_best = temp_offset;
            *bestCost = tempcost;
        }
    }
    return offset_best;
}
static int offset_estimation_eo(int classIdx, SAO_BIG_SIZE lambda, int offset_ori, int count, int diff, SAO_BIG_SIZE* bestCost, int EO_OFFSET_MAP[8])
{
#pragma HLS inline
    int cur_offset = offset_ori;
    int offset_best = 0;
    int lower_bd;
    int upper_bd;
    int Th;
    int temp_offset;
    int start_offset;
    int end_offset;
    int temprate;
    SAO_BIG_SIZE tempdist;
    SAO_BIG_SIZE tempcost;
    SAO_BIG_SIZE mincost;
    SAO_BIG_SIZE lambda_s64 = lambda;
    int offset_type = classIdx;

    lower_bd = saoclip[offset_type][0];
    upper_bd = saoclip[offset_type][1];
    Th       = saoclip[offset_type][2];

// Inlined function: SAOClip() 
    int rose_temp;
    if (upper_bd < cur_offset) {
        rose_temp = upper_bd;
    } else {
        rose_temp = cur_offset;
    }

    if (lower_bd > rose_temp) {
        cur_offset = lower_bd;
    } else {
        cur_offset = rose_temp;
    }

    switch (classIdx) {
    case (int)SAO_CLASS_EO_FULL_VALLEY:
        start_offset = -1;
        if (cur_offset > 1) {
            end_offset = cur_offset;
        } else {
            end_offset = 1;
        }
        break;
    case (int)SAO_CLASS_EO_HALF_VALLEY:
        start_offset = 0;
        end_offset = 1;
        break;
    case (int)SAO_CLASS_EO_HALF_PEAK:
        start_offset = -1;
        end_offset = 0;
        break;
    case (int)SAO_CLASS_EO_FULL_PEAK:
        if (cur_offset < -1) {
            start_offset = cur_offset;
        } else {
            start_offset = -1;
        }
        end_offset = 1;
        break;
    default:
        printf("Not a supported SAO mode\n");
        exit(-1);
    }

    mincost = (SAO_BIG_SIZE)SAO_MAX_BIG;

    for (temp_offset = start_offset; temp_offset <= end_offset; temp_offset += 1) {
#pragma  HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=8
        int offset = temp_offset;

        if (classIdx == ((int)SAO_CLASS_EO_HALF_VALLEY) || classIdx == ((int)SAO_CLASS_EO_HALF_PEAK)) {
            temprate = abs(offset);
        } else {
            int rose_temp_0;
            if (classIdx == ((int)SAO_CLASS_EO_FULL_VALLEY)) {
                rose_temp_0 = offset;
            } else {
                rose_temp_0 = -offset;
            }
            temprate = (&EO_OFFSET_MAP[1])[rose_temp_0];
        }

        if (temprate == Th) {
            temprate = temprate;
        } else {
            temprate = temprate + 1;
        }

        tempdist = distortion_cal((SAO_BIG_SIZE)count, temp_offset, diff);
        tempcost = tempdist + ((lambda_s64 * ((SAO_BIG_SIZE)temprate) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE);

        if (tempcost < mincost) {
            mincost = tempcost;
            offset_best = temp_offset;
            *bestCost = tempcost;
        }
    }
    return offset_best;
}
static int offset_estimation_bo(int classIdx, SAO_BIG_SIZE lambda, int offset_ori, int count, int diff, SAO_BIG_SIZE* bestCost, int EO_OFFSET_MAP[8])
{
#pragma HLS inline
    int cur_offset = offset_ori;
    int offset_best = 0;
    int lower_bd;
    int upper_bd;
    int Th;
    int temp_offset;
    int start_offset;
    int end_offset;
    int temprate;
    SAO_BIG_SIZE tempdist;
    SAO_BIG_SIZE tempcost;
    SAO_BIG_SIZE mincost;
    SAO_BIG_SIZE lambda_s64 = lambda;
    int offset_type = ((int)SAO_CLASS_BO);


    lower_bd = saoclip[offset_type][0];
    upper_bd = saoclip[offset_type][1];
    Th = saoclip[offset_type][2];

// Inlined function: SAOClip() 
    int rose_temp;
    if (upper_bd < cur_offset) {
        rose_temp = upper_bd;
    } else {
        rose_temp = cur_offset;
    }

    if (lower_bd > rose_temp) {
        cur_offset = lower_bd;
    } else {
        cur_offset = rose_temp;
    }

    if (cur_offset >= 0) {
        start_offset = 0;
        end_offset   = cur_offset;
    } else {
        start_offset = cur_offset;
        end_offset   = 0;
    }

    mincost = (SAO_BIG_SIZE)SAO_MAX_BIG;

    LOOP_OFFSET_EST_BO: for (temp_offset = start_offset; temp_offset <= end_offset; temp_offset += 1) {
#pragma  HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=8
        int offset = temp_offset;

        temprate = abs(offset);
        if (temprate) {
            temprate = temprate + 1;
        } else {
            temprate = 0;
        }

        if (temprate == Th) {
            temprate = temprate;
        } else {
            temprate = temprate + 1;
        }

        tempdist = distortion_cal((SAO_BIG_SIZE)count, temp_offset, diff);
        tempcost = tempdist + ((lambda_s64 * ((SAO_BIG_SIZE)temprate) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE);

        if (tempcost < mincost) {
            mincost = tempcost;
            offset_best = temp_offset;
            *bestCost = tempcost;
        }
    }
    return offset_best;
}




void bo_sort_1(SAO_BIG_SIZE lambda_s64, int deltaband_cost_tmp[17], SAO_BIG_SIZE classcost[32], 
    SAO_BIG_SIZE* cost_best, int* saoBlkParam_startBand, int* saoBlkParam_startBand2) 
{
#pragma HLS inline off
    int delta_band12;
    SAO_BIG_SIZE cost_bandsum;

    for (int start_band1 = 0; start_band1 < 8; start_band1++) {
#pragma HLS LOOP_TRIPCOUNT max= 31
        for (int start_band2 = start_band1 + 2; start_band2 < 31; start_band2++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max= 29
            cost_bandsum = ((SAO_BIG_SIZE)(classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1]));
            if (start_band2 - start_band1 > 1 << 5 >> 1) {
                delta_band12 = 32 - start_band2 + start_band1;
            }
            else {
                delta_band12 = start_band2 - start_band1;
            }
            //delta_band12 >= 0 && delta_band12 <= 1 << 5 >> 1?((void )0) : __assert_HLS("delta_band12 >= 0 && delta_band12 <= ((1<<5) >> 1)","../separate/enc_mode.c",(unsigned int )6414,__FUNCTION__);
            cost_bandsum += (lambda_s64 * ((SAO_BIG_SIZE)deltaband_cost_tmp[delta_band12]) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE;

            if (cost_bandsum < (*cost_best)) {
                (*cost_best) = cost_bandsum;
                *saoBlkParam_startBand = start_band1;
                *saoBlkParam_startBand2 = start_band2;
            }
        }
    }
}

void bo_sort_2(SAO_BIG_SIZE lambda_s64, int deltaband_cost_tmp[17], SAO_BIG_SIZE classcost[32],
    SAO_BIG_SIZE* cost_best, int* saoBlkParam_startBand, int* saoBlkParam_startBand2) 
{
#pragma HLS inline off
    int delta_band12;
    SAO_BIG_SIZE cost_bandsum;
    for (int start_band1 = 8; start_band1 < 16; start_band1++) {
#pragma HLS LOOP_TRIPCOUNT max= 31
        for (int start_band2 = start_band1 + 2; start_band2 < 31; start_band2++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max= 29
            cost_bandsum = ((SAO_BIG_SIZE)(classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1]));
            if (start_band2 - start_band1 > 1 << 5 >> 1) {
                delta_band12 = 32 - start_band2 + start_band1;
            }
            else {
                delta_band12 = start_band2 - start_band1;
            }
            //delta_band12 >= 0 && delta_band12 <= 1 << 5 >> 1?((void )0) : __assert_HLS("delta_band12 >= 0 && delta_band12 <= ((1<<5) >> 1)","../separate/enc_mode.c",(unsigned int )6414,__FUNCTION__);
            cost_bandsum += (lambda_s64 * ((SAO_BIG_SIZE)deltaband_cost_tmp[delta_band12]) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE;

            if (cost_bandsum < (*cost_best)) {
                (*cost_best) = cost_bandsum;
                *saoBlkParam_startBand = start_band1;
                *saoBlkParam_startBand2 = start_band2;
            }
        }
    }
}
void bo_sort_3(SAO_BIG_SIZE lambda_s64, int deltaband_cost_tmp[17], SAO_BIG_SIZE classcost[32],
    SAO_BIG_SIZE* cost_best, int* saoBlkParam_startBand, int* saoBlkParam_startBand2) 
{
#pragma HLS inline off
    int delta_band12;
    SAO_BIG_SIZE cost_bandsum;
    for (int start_band1 = 16; start_band1 < 24; start_band1++) {
#pragma HLS LOOP_TRIPCOUNT max= 31
        for (int start_band2 = start_band1 + 2; start_band2 < 31; start_band2++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max= 29
            cost_bandsum = ((SAO_BIG_SIZE)(classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1]));
            if (start_band2 - start_band1 > 1 << 5 >> 1) {
                delta_band12 = 32 - start_band2 + start_band1;
            }
            else {
                delta_band12 = start_band2 - start_band1;
            }
            //delta_band12 >= 0 && delta_band12 <= 1 << 5 >> 1?((void )0) : __assert_HLS("delta_band12 >= 0 && delta_band12 <= ((1<<5) >> 1)","../separate/enc_mode.c",(unsigned int )6414,__FUNCTION__);
            cost_bandsum += (lambda_s64 * ((SAO_BIG_SIZE)deltaband_cost_tmp[delta_band12]) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE;

            if (cost_bandsum < (*cost_best)) {
                (*cost_best) = cost_bandsum;
                *saoBlkParam_startBand = start_band1;
                *saoBlkParam_startBand2 = start_band2;
            }
        }
    }
}
void bo_sort_4(SAO_BIG_SIZE lambda_s64, int deltaband_cost_tmp[17], SAO_BIG_SIZE classcost[32],
    SAO_BIG_SIZE* cost_best, int* saoBlkParam_startBand, int* saoBlkParam_startBand2) 
{
#pragma HLS inline off
    int delta_band12;
    SAO_BIG_SIZE cost_bandsum;
    for (int start_band1 = 24; start_band1 < 31; start_band1++) {
#pragma HLS LOOP_TRIPCOUNT max= 31
        for (int start_band2 = start_band1 + 2; start_band2 < 31; start_band2++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max= 29
            cost_bandsum = ((SAO_BIG_SIZE)(classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1]));
            if (start_band2 - start_band1 > 1 << 5 >> 1) {
                delta_band12 = 32 - start_band2 + start_band1;
            }
            else {
                delta_band12 = start_band2 - start_band1;
            }
            //delta_band12 >= 0 && delta_band12 <= 1 << 5 >> 1?((void )0) : __assert_HLS("delta_band12 >= 0 && delta_band12 <= ((1<<5) >> 1)","../separate/enc_mode.c",(unsigned int )6414,__FUNCTION__);
            cost_bandsum += (lambda_s64 * ((SAO_BIG_SIZE)deltaband_cost_tmp[delta_band12]) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE;

            if (cost_bandsum < (*cost_best)) {
                (*cost_best) = cost_bandsum;
                *saoBlkParam_startBand  = start_band1;
                *saoBlkParam_startBand2 = start_band2;
            }
        }
    }
}

void bo_mode_sort(
    SAO_BIG_SIZE lambda_s64_1,   SAO_BIG_SIZE lambda_s64_2,   SAO_BIG_SIZE lambda_s64_3,   SAO_BIG_SIZE lambda_s64_4,
    int deltaband_cost1[17],     int deltaband_cost2[17],     int deltaband_cost3[17],     int deltaband_cost4[17],
    SAO_BIG_SIZE classcost1[32], SAO_BIG_SIZE classcost2[32], SAO_BIG_SIZE classcost3[32], SAO_BIG_SIZE classcost4[32],
    SAO_BIG_SIZE* cost_best1,    SAO_BIG_SIZE* cost_best2,    SAO_BIG_SIZE* cost_best3,    SAO_BIG_SIZE* cost_best4,
    int* startBand1_1, int* startBand1_2, int* startBand2_1, int* startBand2_2,
    int* startBand3_1, int* startBand3_2, int* startBand4_1, int* startBand4_2) 
{
#pragma HLS inline off
    bo_sort_1(lambda_s64_1, deltaband_cost1, classcost1, cost_best1, startBand1_1, startBand1_2);
    bo_sort_2(lambda_s64_2, deltaband_cost2, classcost2, cost_best2, startBand2_1, startBand2_2);
    bo_sort_3(lambda_s64_3, deltaband_cost3, classcost3, cost_best3, startBand3_1, startBand3_2);
    bo_sort_4(lambda_s64_4, deltaband_cost4, classcost4, cost_best4, startBand4_1, startBand4_2);
}

void bo_best(SAO_BIG_SIZE cost[4], int startband[4][2], int* best_startBand1, int* best_startBand2)
{
    // signed int cost_best_tmp;
//     cost_best_tmp = cost[0];
//     *best_startBand1 = startband[0][0];
//     *best_startBand2 = startband[0][1];

//     for (int i = 1; i < 4; i++) {
// #pragma HLS PIPELINE
//         if (cost[i] < cost_best_tmp) {
//             cost_best_tmp = cost[i];
//             *best_startBand1 = startband[i][0];
//             *best_startBand2 = startband[i][1];
//         }
//     }
    SAO_BIG_SIZE cost_temp_0, cost_temp_1;
    int startBand_temp_0[2];
    int startBand_temp_1[2];

    if (cost[1] < cost[0]) {
        cost_temp_0 = cost[1];
        startBand_temp_0[0] = startband[1][0];
        startBand_temp_0[1] = startband[1][1];
    } else {
        cost_temp_0 = cost[0];
        startBand_temp_0[0] = startband[0][0];
        startBand_temp_0[1] = startband[0][1];
    }
    if (cost[3] < cost[2]) {
        cost_temp_1 = cost[3];
        startBand_temp_1[0] = startband[3][0];
        startBand_temp_1[1] = startband[3][1];
    } else {
        cost_temp_1 = cost[2];
        startBand_temp_1[0] = startband[2][0];
        startBand_temp_1[1] = startband[2][1];
    }
    if (cost_temp_1 < cost_temp_0) {
        *best_startBand1 = startBand_temp_1[0];
        *best_startBand2 = startBand_temp_1[1];
    } else {
        *best_startBand1 = startBand_temp_0[0];
        *best_startBand2 = startBand_temp_0[1];
    }
}

static int deltaband_cost[17] = {(- 1), (- 1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16)};

//static void find_offset_2(int typeIdc, long *pSaoStat_diff,int *pSaoStat_count,int *saoBlkParam_startBand,
//    int *saoBlkParam_startBand2,int *saoBlkParam_deltaband,int (*saoBlkParam_offset)[32],signed int lambda,
//    int EO_OFFSET_MAP[8])
//{
//#pragma HLS inline off
//    int class_i;
//    signed int classcost[32];
//    signed int offth;
//    signed int lambda_s64 = lambda;
//    signed int lambda_s64_1 = lambda;
//    signed int lambda_s64_2 = lambda;
//    signed int lambda_s64_3 = lambda;
//    signed int lambda_s64_4 = lambda;
//    int rose_temp;
//    if (typeIdc == ((int )SAO_TYPE_BO)) {
//        rose_temp = 1 << 5;
//    }
//    else {
//        rose_temp = ((int )NUM_SAO_EO_CLASSES);
//    }
//
//    int num_class = rose_temp;
//    signed int mincost_bandsum;
//    int sb_temp[2];
//#pragma HLS array_partition variable=sb_temp complete dim=0
//    int db_temp;
//
//    for (class_i = 0; class_i < num_class; class_i++) {
//#pragma HLS PIPELINE
//#pragma HLS LOOP_TRIPCOUNT max=32
//        //EO - PLAIN
//        if (typeIdc != ((int )SAO_TYPE_BO) && class_i == ((int )SAO_CLASS_EO_PLAIN)) {
//            ( *saoBlkParam_offset)[class_i] = 0;
//            continue; 
//        }
//        if (pSaoStat_count[class_i] == 0) {
//            //offset will be zero
//            ( *saoBlkParam_offset)[class_i] = 0;
//            continue; 
//        }
//
//        int rose_temp_0;
//
//        if (pSaoStat_diff[class_i] > ((int )0)) {
//            rose_temp_0 = 1;
//        }
//        else {
//            if (pSaoStat_diff[class_i] < ((int )0)) {
//                rose_temp_0 = - 1;
//            }
//            else {
//            rose_temp_0 = 0;
//            }
//        }
//
//        offth = ((signed int )rose_temp_0);
//        ( *saoBlkParam_offset)[class_i] = ((int )(pSaoStat_diff[class_i] * ((int )2) / ((int )pSaoStat_count[class_i]) * ((int )2) + offth)) / 2;
//    }
//
//    if (typeIdc == ((int )SAO_TYPE_BO)) {
//        for (class_i = 0; class_i <32; class_i++) {
////#pragma HLS unroll
//#pragma HLS LOOP_TRIPCOUNT max=32
//            ( *saoBlkParam_offset)[class_i] = offset_estimation(typeIdc,class_i,lambda,( *saoBlkParam_offset)[class_i], pSaoStat_count[class_i], pSaoStat_diff[class_i],&classcost[class_i],EO_OFFSET_MAP);
//        }
//        mincost_bandsum = ((signed int )MAX_S32);
//
//#if 0
//        signed int classcost1[32];
//        signed int classcost2[32];
//        signed int classcost3[32];
//        signed int classcost4[32];
//#pragma HLS array_partition variable=classcost complete dim=0
//#pragma HLS array_partition variable=classcost1 complete dim=0
//#pragma HLS array_partition variable=classcost2 complete dim=0
//#pragma HLS array_partition variable=classcost3 complete dim=0
//#pragma HLS array_partition variable=classcost4 complete dim=0
//        static int deltaband_cost1[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
//        static int deltaband_cost2[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
//        static int deltaband_cost3[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
//        static int deltaband_cost4[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
//// #pragma HLS array_partition variable=deltaband_cost1 complete dim=0
//// #pragma HLS array_partition variable=deltaband_cost2 complete dim=0
//// #pragma HLS array_partition variable=deltaband_cost3 complete dim=0
//// #pragma HLS array_partition variable=deltaband_cost4 complete dim=0
//        for (int i=0; i < 32; i++) {
//#pragma HLS unroll
//            classcost1[i] = classcost[i];
//            classcost2[i] = classcost[i];
//            classcost3[i] = classcost[i];
//            classcost4[i] = classcost[i];
//        }
//        
//        int startband[4][2];
//#pragma HLS array_partition variable=startband complete dim=0
//        signed int cost[4];
//#pragma HLS array_partition variable=cost complete dim=0
//        for (int i=0; i<4; i++) {
//            cost[i] = ((signed int)MAX_S32);
//        }
//
//        bo_mode_sort(
//            lambda_s64_1, lambda_s64_2, lambda_s64_3, lambda_s64_4,
//            deltaband_cost1, deltaband_cost2, deltaband_cost3, deltaband_cost4,
//            classcost1, classcost2, classcost3, classcost4,
//            &cost[0], &cost[1], &cost[2], &cost[3],
//            &startband[0][0], &startband[0][1], &startband[1][0], &startband[1][1],
//            &startband[2][0], &startband[2][1], &startband[3][0], &startband[3][1]
//        );
//
//        int best_startBand1_tmp;
//        int best_startBand2_tmp;
//        // signed int cost_best_tmp;
//        // cost_best_tmp = cost[0];
//        // best_startBand1_tmp = startband[0][0];
//        // best_startBand2_tmp = startband[0][1];
//        // for (int i = 1; i < 4; i++) {
//        //     if (cost[i] < cost_best_tmp) {
//        //         cost_best_tmp = cost[i];
//        //         best_startBand1_tmp = startband[i][0];
//        //         best_startBand2_tmp = startband[i][1];
//        //     }
//        // }
//
//        bo_best(cost, startband, &best_startBand1_tmp, &best_startBand2_tmp);
//
//        *saoBlkParam_startBand  = best_startBand1_tmp;
//        *saoBlkParam_startBand2 = best_startBand2_tmp;
//#else
//        signed int cost_bandsum;
//        int start_band1_tmp, start_band2_tmp;
//        int delta_band12;
//        for (int start_band1 = 0; start_band1 < (1 << 5) - 1; start_band1++) {
//#pragma HLS LOOP_TRIPCOUNT max= 31
//           for (int start_band2 = start_band1 + 2; start_band2 < (1 << 5) - 1; start_band2++) {
//#pragma HLS PIPELINE
//#pragma HLS LOOP_TRIPCOUNT max= 29
//                cost_bandsum = ((signed int )(classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1]));
//                if (start_band2 - start_band1 > 1 << 5 >> 1) {
//                   delta_band12 = 32 - start_band2 + start_band1;
//                }
//                else {
//                    delta_band12 = start_band2 - start_band1;
//                }
//                //delta_band12 >= 0 && delta_band12 <= 1 << 5 >> 1?((void )0) : __assert_HLS("delta_band12 >= 0 && delta_band12 <= ((1<<5) >> 1)","../separate/enc_mode.c",(unsigned int )6414,__FUNCTION__);
//                cost_bandsum += (lambda_s64 * ((int)deltaband_cost[delta_band12]) + ((int)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE;
//
//                if (cost_bandsum < mincost_bandsum) {
//                    mincost_bandsum = cost_bandsum;
//                    start_band1_tmp = start_band1;
//                    start_band2_tmp = start_band2;
//                }
//            }
//        }
//        *saoBlkParam_startBand  = start_band1_tmp;
//        *saoBlkParam_startBand2 = start_band2_tmp;
//#endif
//
////        for (class_i = 0; class_i < 32; class_i++) {
////#pragma HLS unroll
////#pragma HLS LOOP_TRIPCOUNT max=32
////            if (((class_i >=  *saoBlkParam_startBand) && (class_i <=  *saoBlkParam_startBand + 1)) || ((class_i >=  *saoBlkParam_startBand2) && (class_i <=  *saoBlkParam_startBand2 + 1))) {
////                continue; 
////            }
////            ( *saoBlkParam_offset)[class_i] = 0;
////        }
//		for (class_i = 0; class_i < 32; class_i++) {
//#pragma HLS unroll
//#pragma HLS LOOP_TRIPCOUNT max=32
//            if ( (class_i != *saoBlkParam_startBand) && (class_i != *saoBlkParam_startBand+1) && (class_i != *saoBlkParam_startBand2) && (class_i != *saoBlkParam_startBand2+1) ) {
//                (*saoBlkParam_offset)[class_i] = 0;
//            }
//        }
//
//        if ( *saoBlkParam_startBand <  *saoBlkParam_startBand2) {
//            sb_temp[0] =  *saoBlkParam_startBand;
//            sb_temp[1] =  *saoBlkParam_startBand2;
//        }
//        else {
//            sb_temp[0] =  *saoBlkParam_startBand2;
//            sb_temp[1] =  *saoBlkParam_startBand;
//        }
//        // if ( *saoBlkParam_startBand >  *saoBlkParam_startBand2) {
//        //     sb_temp[1] =  *saoBlkParam_startBand;
//        // }
//        // else {
//        //     sb_temp[1] =  *saoBlkParam_startBand2;
//        // }
//
//        db_temp = sb_temp[1] - sb_temp[0];
//        if (db_temp > 16) {
//            *saoBlkParam_deltaband = 32 - db_temp;
//            *saoBlkParam_startBand = sb_temp[1];
//            *saoBlkParam_startBand2 = sb_temp[0];
//        }
//        else {
//            *saoBlkParam_deltaband = db_temp;
//            *saoBlkParam_startBand = sb_temp[0];
//            *saoBlkParam_startBand2 = sb_temp[1];
//        }
//    }
//    else {
//        //typeIdc >= ((int )SAO_TYPE_EO_0) && typeIdc <= ((int )SAO_TYPE_EO_45)?((void )0) : __assert_HLS("typeIdc >= SAO_TYPE_EO_0 && typeIdc <= SAO_TYPE_EO_45","../separate/enc_mode.c",(unsigned int )6452,__FUNCTION__);
//        // LOOP EO 5 types (find best offset for each type)
//        for (class_i = 0; class_i < 5; class_i++) {//num_class
//#pragma HLS LOOP_TRIPCOUNT max=5
//            if (class_i == ((int )SAO_CLASS_EO_PLAIN)) {
//                ( *saoBlkParam_offset)[class_i] = 0;
//                classcost[class_i] = ((signed int )0);
//            }
//            else {
//                ( *saoBlkParam_offset)[class_i] = offset_estimation(typeIdc,class_i,lambda,( *saoBlkParam_offset)[class_i], pSaoStat_count[class_i], pSaoStat_diff[class_i], &classcost[class_i], EO_OFFSET_MAP);
//            }
//        }
//        *saoBlkParam_startBand = 0;
//    }
//}
static void find_offset_eo(int* pSaoStat_diff, int* pSaoStat_count, int* saoBlkParam_offset, SAO_BIG_SIZE lambda, int EO_OFFSET_MAP[8])
{
#pragma HLS inline off
    SAO_BIG_SIZE classcost[32];
    SAO_BIG_SIZE offth;
    int rose_temp = (int)NUM_SAO_EO_CLASSES;

    int num_class = rose_temp;
    SAO_BIG_SIZE mincost_bandsum;
    int sb_temp[2];
#pragma HLS array_partition variable=sb_temp complete dim=0
    int db_temp;

    for (int class_i = 0; class_i < NUM_SAO_EO_CLASSES; class_i++) {
#pragma HLS PIPELINE
        //EO - PLAIN
        if (class_i == ((int)SAO_CLASS_EO_PLAIN)) {
            saoBlkParam_offset[class_i] = 0;
            continue;
        }
        if (pSaoStat_count[class_i] == 0) {
            saoBlkParam_offset[class_i] = 0;
            continue;
        }

        int offth;
        if (pSaoStat_diff[class_i] > 0) {
            offth = 1;
        } else if (pSaoStat_diff[class_i] < 0) {
            offth = -1;
        } else {
            offth = 0;
        }

        saoBlkParam_offset[class_i] = (int)( (SAO_BIG_SIZE)pSaoStat_diff[class_i] * 2 / (SAO_BIG_SIZE)pSaoStat_count[class_i] * 2 + (SAO_BIG_SIZE)offth) / 2;
    }

    // EO 5 types, find best offset
    for (int class_i = 0; class_i < 5; class_i++) {
        if (class_i == ((int)SAO_CLASS_EO_PLAIN)) {
            saoBlkParam_offset[class_i] = 0;
            classcost[class_i] = 0;
        } else {
            saoBlkParam_offset[class_i] = offset_estimation_eo(class_i, lambda, saoBlkParam_offset[class_i], pSaoStat_count[class_i], pSaoStat_diff[class_i], &classcost[class_i], EO_OFFSET_MAP);
        }
    }
}
static void find_offset_bo(int pSaoStat_diff[32], int pSaoStat_count[32], int* saoBlkParam_startBand, int* saoBlkParam_startBand2, int* saoBlkParam_deltaband, int(*saoBlkParam_offset)[32], 
    SAO_BIG_SIZE lambda, int EO_OFFSET_MAP[8])
{
#pragma HLS inline off
    SAO_BIG_SIZE classcost[32];
    SAO_BIG_SIZE offth;
    SAO_BIG_SIZE lambda_s64 = lambda;
    SAO_BIG_SIZE lambda_s64_1 = lambda;
    SAO_BIG_SIZE lambda_s64_2 = lambda;
    SAO_BIG_SIZE lambda_s64_3 = lambda;
    SAO_BIG_SIZE lambda_s64_4 = lambda;
    int num_class = 1 << 5;
    SAO_BIG_SIZE mincost_bandsum;
    int sb_temp[2];
#pragma HLS array_partition variable=sb_temp complete dim=0
    int db_temp;
    int offset_tmp[32];
    LOOP_OFFSET_INIT: for (int class_i = 0; class_i < 32; class_i++) {
#pragma HLS PIPELINE
        if (pSaoStat_count[class_i] == 0) {
           offset_tmp[class_i] = 0;
           // (*saoBlkParam_offset)[class_i] = 0;
            continue;
        }

        int rose_temp_0;

        if (pSaoStat_diff[class_i] > 0) {
            rose_temp_0 = 1;
        } else if (pSaoStat_diff[class_i] < 0) {
            rose_temp_0 = -1;
        } else {
            rose_temp_0 = 0;
        }

        offth = (SAO_BIG_SIZE)rose_temp_0;
        offset_tmp[class_i] = ((int)(pSaoStat_diff[class_i] * ((SAO_BIG_SIZE)2) / ((SAO_BIG_SIZE)pSaoStat_count[class_i]) * ((SAO_BIG_SIZE)2) + offth)) / 2;
    }

    LOOP_OFFSET_REFINE: for (int class_i = 0; class_i < 32; class_i++) {
        (*saoBlkParam_offset)[class_i] = offset_estimation_bo(class_i, lambda, offset_tmp[class_i], pSaoStat_count[class_i], pSaoStat_diff[class_i], &classcost[class_i], EO_OFFSET_MAP);
    }
    mincost_bandsum = (SAO_BIG_SIZE)SAO_MAX_BIG;

#if 0
    SAO_BIG_SIZE classcost1[32];
    SAO_BIG_SIZE classcost2[32];
    SAO_BIG_SIZE classcost3[32];
    SAO_BIG_SIZE classcost4[32];
#pragma HLS array_partition variable=classcost complete dim=0
#pragma HLS array_partition variable=classcost1 complete dim=0
#pragma HLS array_partition variable=classcost2 complete dim=0
#pragma HLS array_partition variable=classcost3 complete dim=0
#pragma HLS array_partition variable=classcost4 complete dim=0
    static int deltaband_cost1[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
    static int deltaband_cost2[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
    static int deltaband_cost3[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
    static int deltaband_cost4[17] = { (-1), (-1), (2), (2), (4), (4), (4), (4), (8), (8), (8), (8), (8), (8), (8), (8), (16) };
    // #pragma HLS array_partition variable=deltaband_cost1 complete dim=0
    // #pragma HLS array_partition variable=deltaband_cost2 complete dim=0
    // #pragma HLS array_partition variable=deltaband_cost3 complete dim=0
    // #pragma HLS array_partition variable=deltaband_cost4 complete dim=0
    for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        classcost1[i] = classcost[i];
        classcost2[i] = classcost[i];
        classcost3[i] = classcost[i];
        classcost4[i] = classcost[i];
    }

    int startband[4][2];
#pragma HLS array_partition variable=startband complete dim=0
    SAO_BIG_SIZE cost[4];
#pragma HLS array_partition variable=cost complete dim=0
    for (int i = 0; i < 4; i++) {
        cost[i] = (SAO_BIG_SIZE)SAO_MAX_BIG;
    }

    bo_mode_sort(
        lambda_s64_1, lambda_s64_2, lambda_s64_3, lambda_s64_4,
        deltaband_cost1, deltaband_cost2, deltaband_cost3, deltaband_cost4,
        classcost1, classcost2, classcost3, classcost4,
        &cost[0], &cost[1], &cost[2], &cost[3],
        &startband[0][0], &startband[0][1], &startband[1][0], &startband[1][1],
        &startband[2][0], &startband[2][1], &startband[3][0], &startband[3][1]
    );

    int best_startBand1_tmp;
    int best_startBand2_tmp;
    // SAO_BIG_SIZE cost_best_tmp;
    // cost_best_tmp = cost[0];
    // best_startBand1_tmp = startband[0][0];
    // best_startBand2_tmp = startband[0][1];
    // for (int i = 1; i < 4; i++) {
    //     if (cost[i] < cost_best_tmp) {
    //         cost_best_tmp = cost[i];
    //         best_startBand1_tmp = startband[i][0];
    //         best_startBand2_tmp = startband[i][1];
    //     }
    // }

    bo_best(cost, startband, &best_startBand1_tmp, &best_startBand2_tmp);

    *saoBlkParam_startBand  = best_startBand1_tmp;
    *saoBlkParam_startBand2 = best_startBand2_tmp;
#else
    SAO_BIG_SIZE cost_bandsum;
    int start_band1_tmp, start_band2_tmp;
    int delta_band12;
    LOOP_BAND_BEST_1: for (int start_band1 = 0; start_band1 < 29; start_band1++) {  // [0:32-2-1)
#pragma HLS LOOP_TRIPCOUNT max= 29
        LOOP_BAND_BEST_2: for (int start_band2 = start_band1 + 2; start_band2 < 31; start_band2++) {    // [2:32-1)
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max= 29
            cost_bandsum = ((SAO_BIG_SIZE)(classcost[start_band1] + classcost[start_band1 + 1] + classcost[start_band2] + classcost[start_band2 + 1]));
            if (start_band2 - start_band1 > 16) {
                delta_band12 = 32 - start_band2 + start_band1;
            } else {
                delta_band12 = start_band2 - start_band1;
            }
            cost_bandsum += (lambda_s64 * ((SAO_BIG_SIZE)deltaband_cost[delta_band12]) + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE;

            if (cost_bandsum < mincost_bandsum) {
                mincost_bandsum = cost_bandsum;
                start_band1_tmp = start_band1;
                start_band2_tmp = start_band2;
            }
        }
    }
    *saoBlkParam_startBand  = start_band1_tmp;
    *saoBlkParam_startBand2 = start_band2_tmp;
#endif

    LOOP_OFFSET_NOT_USE_ZERO: for (int class_i = 0; class_i < 32; class_i++) {
#pragma HLS unroll
        if ((class_i != *saoBlkParam_startBand) && (class_i != *saoBlkParam_startBand + 1) && (class_i != *saoBlkParam_startBand2) && (class_i != *saoBlkParam_startBand2 + 1)) {
            (*saoBlkParam_offset)[class_i] = 0;
        }
    }

    if (*saoBlkParam_startBand < *saoBlkParam_startBand2) {
        sb_temp[0] = *saoBlkParam_startBand;
        sb_temp[1] = *saoBlkParam_startBand2;
    } else {
        sb_temp[0] = *saoBlkParam_startBand2;
        sb_temp[1] = *saoBlkParam_startBand;
    }

    db_temp = sb_temp[1] - sb_temp[0];
    if (db_temp > 16) {
        *saoBlkParam_deltaband  = 32 - db_temp;
        *saoBlkParam_startBand  = sb_temp[1];
        *saoBlkParam_startBand2 = sb_temp[0];
    } else {
        *saoBlkParam_deltaband  = db_temp;
        *saoBlkParam_startBand  = sb_temp[0];
        *saoBlkParam_startBand2 = sb_temp[1];
    }
}

static void RDO_sao_offset_AEC(int value1, int value2, int *pbin_count,int EO_OFFSET_MAP[8])
{
#pragma HLS inline off
    int act_sym;
    int temp;
    int maxvalue;
    int offset_type = value2;
    if (offset_type == ((int )SAO_CLASS_EO_FULL_VALLEY)) {
        act_sym = EO_OFFSET_MAP[value1 + 1];
    }
    else {
        if (offset_type == ((int )SAO_CLASS_EO_FULL_PEAK)) {
            act_sym = EO_OFFSET_MAP[-value1 + 1];
        } else {
            act_sym = abs(value1);
        }
    }
    maxvalue = saoclip[offset_type][2];
    temp = act_sym;
    if (temp == 0) {
        ( *pbin_count)++;
    }
    else {
        while(temp != 0){
#pragma HLS LOOP_TRIPCOUNT max=7
            ( *pbin_count)++;
            temp--;
        }
        if (act_sym < maxvalue) {
            ( *pbin_count)++;
        }
    }
    if (offset_type == ((int )SAO_CLASS_BO) && act_sym) {
        ( *pbin_count)++;
    }
}

static int RDO_sao_offset(int *saoBlkParam_typeIdc,int *saoBlkParam_startBand,int *saoBlkParam_startBand2,int (*saoBlkParam_offset)[32],int *pbin_count,int EO_OFFSET_MAP[8])
{
    int value1;
    int value2;
    int i;
    int bandIdxBO[4];
    //saoBlkParam -> modeIdc == ((int )SAO_MODE_NEW)?((void )0) : __assert_HLS("saoBlkParam->modeIdc == SAO_MODE_NEW","../separate/enc_eco.c",(unsigned int )2795,__FUNCTION__);
    if ( *saoBlkParam_typeIdc == ((int )SAO_TYPE_BO)) {
        bandIdxBO[0] =  *saoBlkParam_startBand;
        bandIdxBO[1] = bandIdxBO[0] + 1;
        bandIdxBO[2] =  *saoBlkParam_startBand2;
        bandIdxBO[3] = bandIdxBO[2] + 1;
        for (i = 0; i < 4; i++) {
#pragma HLS LOOP_TRIPCOUNT max=4
            value1 = ( *saoBlkParam_offset)[bandIdxBO[i]];
            value2 = ((int )SAO_CLASS_BO);
            //RDO_sao_offset_AEC(value1, value2, ctx);
            RDO_sao_offset_AEC(value1,value2,pbin_count,EO_OFFSET_MAP);
        }
    }
    else {
        for (i = ((int )SAO_CLASS_EO_FULL_VALLEY); i < ((int )NUM_SAO_EO_CLASSES); i++) {
#pragma HLS LOOP_TRIPCOUNT max=5
            if (i == ((int )SAO_CLASS_EO_PLAIN)) {
                continue; 
            }
            value1 = ( *saoBlkParam_offset)[i];
            value2 = i;
            //RDO_sao_offset_AEC(value1, value2, ctx);
            RDO_sao_offset_AEC(value1,value2,pbin_count,EO_OFFSET_MAP);
        }
    } 
    return 0;
}
static int RDO_sao_offset_eo(int* saoBlkParam_offset, int* pbin_count, int EO_OFFSET_MAP[8])
{
    int value1;
    int value2;
    int bandIdxBO[4];
    for (int i = (int)SAO_CLASS_EO_FULL_VALLEY; i < (int)NUM_SAO_EO_CLASSES; i++) {
#pragma HLS LOOP_TRIPCOUNT max=5
        if (i == ((int)SAO_CLASS_EO_PLAIN)) {
            continue;
        }
        value1 = saoBlkParam_offset[i];
        value2 = i;
        RDO_sao_offset_AEC(value1, value2, pbin_count, EO_OFFSET_MAP);
    }
    return 0;
}
static int RDO_sao_offset_bo(int* saoBlkParam_startBand, int* saoBlkParam_startBand2, int(*saoBlkParam_offset)[32], int* pbin_count, int EO_OFFSET_MAP[8])
{
    int value1;
    int value2;
    int bandIdxBO[4];

    bandIdxBO[0] = *saoBlkParam_startBand;
    bandIdxBO[1] = *saoBlkParam_startBand  + 1;
    bandIdxBO[2] = *saoBlkParam_startBand2;
    bandIdxBO[3] = *saoBlkParam_startBand2 + 1;
    for (int i = 0; i < 4; i++) {
        value1 = (*saoBlkParam_offset)[bandIdxBO[i]];
        value2 = (int)SAO_CLASS_BO;
        RDO_sao_offset_AEC(value1, value2, pbin_count, EO_OFFSET_MAP);
    }
    return 0;
}

static void RDO_sao_type_AEC(int value1, int value2, int *pbin_count)
{
#pragma HLS inline off
    int act_sym = value1;
    int temp;
    int length;
    int exp_golomb_order;
    temp = act_sym;
    exp_golomb_order = 1;
    switch (value2) {
        case 0:
            length = 2;
            break; 
        case 1:
            length = 5;
            break; 
        case 2:
            length = 4;
            break; 
        default:
            length = 0;
            break; 
    }

    if (value2 == 2) {
        while (1) {
#pragma HLS LOOP_TRIPCOUNT max=4
            if (((unsigned int )temp) >= ((unsigned int )(1 << exp_golomb_order))) {
                ( *pbin_count)++;
                temp = temp - (1 << exp_golomb_order);
                exp_golomb_order++;
            }
            else {
                if (exp_golomb_order == 4) {
                    exp_golomb_order = 0;
                }
                else {
                    ( *pbin_count)++;
                }
                //next binary part
                while(exp_golomb_order--){
#pragma HLS LOOP_TRIPCOUNT max=3
                    ( *pbin_count)++;
                }
                break; 
            }
        }
    }
    else {
        for (int i = 0; i < length; i++) {
            ( *pbin_count)++;
            temp = temp >> 1;
        }
    }
}

static void RDO_sao_type(int *saoBlkParam_typeIdc,int *saoBlkParam_startBand,int *saoBlkParam_deltaband,int *pbin_count)
{
#pragma HLS inline off
    int value1;
    int value2;
    if ( *saoBlkParam_typeIdc == ((int )SAO_TYPE_BO)) {
        value1 =  *saoBlkParam_startBand;
        //write start band for BO
        value2 = 1;
        //RDO_sao_type_AEC(value1, value2, ctx);
        RDO_sao_type_AEC(value1,value2,pbin_count);
        value1 =  *saoBlkParam_deltaband - 2;
        //write delta start band for BO
        value2 = 2;
        //RDO_sao_type_AEC(value1, value2, ctx);
        RDO_sao_type_AEC(value1,value2,pbin_count);
    }
    else {
        //saoBlkParam -> typeIdc >= ((int )SAO_TYPE_EO_0) && saoBlkParam -> typeIdc <= ((int )SAO_TYPE_EO_45)?((void )0) : ((void )(__assert_HLS("saoBlkParam->typeIdc >= SAO_TYPE_EO_0 && saoBlkParam->typeIdc <= SAO_TYPE_EO_45","../separate/enc_eco.c",(unsigned int )2904,__FUNCTION__)));
        value1 =  *saoBlkParam_typeIdc;
        value2 = 0;
        //RDO_sao_type_AEC(value1, value2, ctx);
        RDO_sao_type_AEC(value1,value2,pbin_count);
    }
}
static void RDO_sao_type_eo(int type, int* pbin_count)
{
#pragma HLS inline off
    int value1 = type;
    int value2 = 0;
    RDO_sao_type_AEC(value1, value2, pbin_count);
}
static void RDO_sao_type_bo(int* saoBlkParam_startBand, int* saoBlkParam_deltaband, int* pbin_count)
{
#pragma HLS inline off
    RDO_sao_type_AEC((*saoBlkParam_startBand),     1, pbin_count);
    RDO_sao_type_AEC((*saoBlkParam_deltaband - 2), 2, pbin_count);

}


//static void SAORDCost_comp_new(int type, long *pSaoStat_diff,int *pSaoStat_count,int *temp_sao_param_modeIdc,int *temp_sao_param_typeIdc,int *temp_sao_param_startBand,int *temp_sao_param_startBand2,int *temp_sao_param_deltaband,int (*temp_sao_param_offset)[32],signed int sao_lambda,int *currate,int *curdist,signed int *curcost,int EO_OFFSET_MAP[8])
//{
//#pragma HLS inline off
//    int bin_count_copy = 0;
//    *temp_sao_param_modeIdc = ((int )SAO_MODE_NEW);
//    *temp_sao_param_typeIdc = type;
//
//    find_offset_2(type, pSaoStat_diff, pSaoStat_count, temp_sao_param_startBand, temp_sao_param_startBand2, temp_sao_param_deltaband, temp_sao_param_offset, sao_lambda,EO_OFFSET_MAP);
//    *curdist = get_distortion_2(type, pSaoStat_diff, pSaoStat_count, temp_sao_param_offset);
//
//    RDO_sao_mode(temp_sao_param_modeIdc,temp_sao_param_typeIdc,&bin_count_copy);
//    RDO_sao_offset(temp_sao_param_typeIdc,temp_sao_param_startBand,temp_sao_param_startBand2,temp_sao_param_offset,&bin_count_copy,EO_OFFSET_MAP);
//    RDO_sao_type(temp_sao_param_typeIdc,temp_sao_param_startBand,temp_sao_param_deltaband,&bin_count_copy);
//
//    *currate = bin_count_copy;
//    *curcost = *curdist + (((*currate) * sao_lambda + ((int)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE);
//
//}
static void sao_rdcost_comp_new_eo(int type, int *pSaoStat_diff, int *pSaoStat_count, int* temp_sao_param_modeIdc, int* temp_sao_param_typeIdc, int *temp_sao_param_offset, SAO_BIG_SIZE sao_lambda, int* currate, SAO_BIG_SIZE* curdist, SAO_BIG_SIZE* curcost, int EO_OFFSET_MAP[8])
{
#pragma HLS inline off
    int bin_count_copy = 0;
    *temp_sao_param_modeIdc = (int)SAO_MODE_NEW;
    *temp_sao_param_typeIdc = type;

    RDO_sao_mode_new_eo(&bin_count_copy);
    RDO_sao_type_eo(type, &bin_count_copy);
    find_offset_eo(pSaoStat_diff, pSaoStat_count, temp_sao_param_offset, sao_lambda, EO_OFFSET_MAP);
    *curdist = get_distortion_eo(pSaoStat_diff, pSaoStat_count, temp_sao_param_offset);
    RDO_sao_offset_eo(temp_sao_param_offset, &bin_count_copy, EO_OFFSET_MAP);

    *currate = bin_count_copy;
    *curcost = *curdist + (((*currate) * sao_lambda + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE);
}
static void sao_rdcost_comp_new_bo(int pSaoStat_diff[32], int pSaoStat_count[32], int* temp_sao_param_modeIdc, int* temp_sao_param_typeIdc, int* temp_sao_param_startBand, int* temp_sao_param_startBand2, int* temp_sao_param_deltaband, int(*temp_sao_param_offset)[32], SAO_BIG_SIZE sao_lambda, int* currate, SAO_BIG_SIZE* curdist, SAO_BIG_SIZE* curcost, int EO_OFFSET_MAP[8])
{
#pragma HLS inline off
    int bin_count_copy = 0;
    *temp_sao_param_modeIdc = (int)SAO_MODE_NEW;
    *temp_sao_param_typeIdc = (int)SAO_TYPE_BO;

    RDO_sao_mode_new_bo(&bin_count_copy);
    find_offset_bo(pSaoStat_diff, pSaoStat_count, temp_sao_param_startBand, temp_sao_param_startBand2, temp_sao_param_deltaband, temp_sao_param_offset, sao_lambda, EO_OFFSET_MAP);
    *curdist = get_distortion_bo(pSaoStat_diff, pSaoStat_count, temp_sao_param_offset);
    RDO_sao_offset_bo(temp_sao_param_startBand, temp_sao_param_startBand2, temp_sao_param_offset, &bin_count_copy, EO_OFFSET_MAP);
    RDO_sao_type_bo(temp_sao_param_startBand, temp_sao_param_deltaband, &bin_count_copy);

    *currate = bin_count_copy;
    *curcost = *curdist + (((*currate) * sao_lambda + ((SAO_BIG_SIZE)((1 << LAMBDA_SCALE) / 2))) >> LAMBDA_SCALE);
}



static void copySAOParam_for_blk_onecomponent_pipe(int *saopara_dst_modeIdc, int *saopara_dst_typeIdc, int *saopara_dst_startBand, int *saopara_dst_startBand2, int *saopara_dst_deltaband, int saopara_dst_offset[32],
                                                   int *saopara_src_modeIdc, int *saopara_src_typeIdc, int *saopara_src_startBand, int *saopara_src_startBand2, int *saopara_src_deltaband, int saopara_src_offset[32])
{
    *saopara_dst_modeIdc    = *saopara_src_modeIdc;
    *saopara_dst_typeIdc    = *saopara_src_typeIdc;
    *saopara_dst_startBand  = *saopara_src_startBand;
    *saopara_dst_startBand2 = *saopara_src_startBand2;
    *saopara_dst_deltaband  = *saopara_src_deltaband;

    for (int j = 0; j < 32; j++) {
#pragma  HLS PIPELINE
        saopara_dst_offset[j] = saopara_src_offset[j];
    }
}
static void copySAOParam_for_blk_onecomponent_pipe2(int* saopara_dst_modeIdc, int* saopara_dst_typeIdc, int* saopara_dst_startBand, int* saopara_dst_startBand2, int* saopara_dst_deltaband, int saopara_dst_offset[32], 
                                                    int* saopara_src_modeIdc, int* saopara_src_typeIdc,                                                                                      int saopara_src_offset[32])
{
    *saopara_dst_modeIdc = *saopara_src_modeIdc;
    *saopara_dst_typeIdc = *saopara_src_typeIdc;
    *saopara_dst_startBand  = 0;
    *saopara_dst_startBand2 = 0;
    *saopara_dst_deltaband  = 0;

    for (int j = 0; j < 32; j++) {
#pragma  HLS PIPELINE
        saopara_dst_offset[j] = saopara_src_offset[j];
    }
}

static void copySAOParam_for_blk_onecomponent_1(SAOBlkParam *saopara_dst,int saopara_src_modeIdc,int saopara_src_typeIdc,
int saopara_src_startBand,int saopara_src_startBand2,int saopara_src_deltaband,int saopara_src_offset[32])
{
#pragma HLS inline off
    int j;
    saopara_dst->modeIdc = saopara_src_modeIdc;
    saopara_dst->typeIdc = saopara_src_typeIdc;
    saopara_dst->startBand = saopara_src_startBand;
    saopara_dst->startBand2 = saopara_src_startBand2;
    saopara_dst->deltaband = saopara_src_deltaband;
    for (j = 0; j < 32; j++) 
    {
#pragma HLS PIPELINE
        saopara_dst->offset[j] = saopara_src_offset[j];
    }
}

static void sao_rdo_result(
    int *sao_cur_param_modeIdc,       int *sao_cur_param_typeIdc,       int *sao_cur_param_startBand,       int *sao_cur_param_startBand2,       int *sao_cur_param_deltaband,       int (*sao_cur_param_offset)[32],
    int *temp_sao_param_Y0_modeIdc,   int *temp_sao_param_Y0_typeIdc,   int *temp_sao_param_Y0_startBand,   int *temp_sao_param_Y0_startBand2,   int *temp_sao_param_Y0_deltaband,   int (*temp_sao_param_Y0_offset)[32],
    int *temp_sao_param_Y90_modeIdc,  int *temp_sao_param_Y90_typeIdc,  int *temp_sao_param_Y90_startBand,  int *temp_sao_param_Y90_startBand2,  int *temp_sao_param_Y90_deltaband,  int (*temp_sao_param_Y90_offset)[32],
    int *temp_sao_param_Y135_modeIdc, int *temp_sao_param_Y135_typeIdc, int *temp_sao_param_Y135_startBand, int *temp_sao_param_Y135_startBand2, int *temp_sao_param_Y135_deltaband, int (*temp_sao_param_Y135_offset)[32],
    int *temp_sao_param_Y45_modeIdc,  int *temp_sao_param_Y45_typeIdc,  int *temp_sao_param_Y45_startBand,  int *temp_sao_param_Y45_startBand2,  int *temp_sao_param_Y45_deltaband,  int (*temp_sao_param_Y45_offset)[32],
    int *temp_sao_param_YBO_modeIdc,  int *temp_sao_param_YBO_typeIdc,  int *temp_sao_param_YBO_startBand,  int *temp_sao_param_YBO_startBand2,  int *temp_sao_param_YBO_deltaband,  int (*temp_sao_param_YBO_offset)[32],
    SAO_BIG_SIZE *mincost,   SAO_BIG_SIZE *mindist,  int *minrate,
    SAO_BIG_SIZE curcost0,   SAO_BIG_SIZE curdist0,  int currate0,   
	SAO_BIG_SIZE curcost90,  SAO_BIG_SIZE curdist90, int currate90,
    SAO_BIG_SIZE curcost135, SAO_BIG_SIZE curdist135,int currate135, 
	SAO_BIG_SIZE curcost45,  SAO_BIG_SIZE curdist45, int currate45,
    SAO_BIG_SIZE curcostBO,  SAO_BIG_SIZE curdistBO, int currateBO)
{
    int temp = 5;
    if (curcost0 < *mincost) {
        temp = 0;
        *mincost = curcost0;
    }
    if (curcost90 < *mincost) {
        temp = 1;
        *mincost = curcost90;
    }
    if (curcost135 < *mincost) {
        temp = 2;
        *mincost = curcost135;
    }
    if (curcost45 < *mincost) {
        temp = 3;
        *mincost = curcost45;
    }
    if (curcostBO < *mincost) {
        temp = 4;
        *mincost = curcostBO;
    }
    switch (temp) {
        case 0:
        {
            *mindist = curdist0;
            *minrate = currate0;
            copySAOParam_for_blk_onecomponent_pipe(sao_cur_param_modeIdc,sao_cur_param_typeIdc,sao_cur_param_startBand,sao_cur_param_startBand2,sao_cur_param_deltaband, *sao_cur_param_offset,temp_sao_param_Y0_modeIdc,temp_sao_param_Y0_typeIdc,temp_sao_param_Y0_startBand,temp_sao_param_Y0_startBand2,temp_sao_param_Y0_deltaband, *temp_sao_param_Y0_offset);
            //copySAOParam_for_blk_onecomponent_pipe(rec_sao_cur_param_modeIdc,rec_sao_cur_param_typeIdc,rec_sao_cur_param_startBand,rec_sao_cur_param_startBand2,rec_sao_cur_param_deltaband,rec_sao_cur_param_offset,temp_sao_param_Y0_modeIdc,temp_sao_param_Y0_typeIdc,temp_sao_param_Y0_startBand,temp_sao_param_Y0_startBand2,temp_sao_param_Y0_deltaband,temp_sao_param_Y0_offset);
        }
        break; 
        case 1:
        {
            *mindist = curdist90;
            *minrate = currate90;
            copySAOParam_for_blk_onecomponent_pipe(sao_cur_param_modeIdc,sao_cur_param_typeIdc,sao_cur_param_startBand,sao_cur_param_startBand2,sao_cur_param_deltaband, *sao_cur_param_offset,temp_sao_param_Y90_modeIdc,temp_sao_param_Y90_typeIdc,temp_sao_param_Y90_startBand,temp_sao_param_Y90_startBand2,temp_sao_param_Y90_deltaband, *temp_sao_param_Y90_offset);
            //copySAOParam_for_blk_onecomponent_pipe(rec_sao_cur_param_modeIdc,rec_sao_cur_param_typeIdc,rec_sao_cur_param_startBand,rec_sao_cur_param_startBand2,rec_sao_cur_param_deltaband,rec_sao_cur_param_offset,temp_sao_param_Y90_modeIdc,temp_sao_param_Y90_typeIdc,temp_sao_param_Y90_startBand,temp_sao_param_Y90_startBand2,temp_sao_param_Y90_deltaband,temp_sao_param_Y90_offset);
        }
        break; 
        case 2:
        {
            *mindist = curdist135;
            *minrate = currate135;
            copySAOParam_for_blk_onecomponent_pipe(sao_cur_param_modeIdc,sao_cur_param_typeIdc,sao_cur_param_startBand,sao_cur_param_startBand2,sao_cur_param_deltaband, *sao_cur_param_offset,temp_sao_param_Y135_modeIdc,temp_sao_param_Y135_typeIdc,temp_sao_param_Y135_startBand,temp_sao_param_Y135_startBand2,temp_sao_param_Y135_deltaband, *temp_sao_param_Y135_offset);
            //copySAOParam_for_blk_onecomponent_pipe(rec_sao_cur_param_modeIdc,rec_sao_cur_param_typeIdc,rec_sao_cur_param_startBand,rec_sao_cur_param_startBand2,rec_sao_cur_param_deltaband,rec_sao_cur_param_offset,temp_sao_param_Y135_modeIdc,temp_sao_param_Y135_typeIdc,temp_sao_param_Y135_startBand,temp_sao_param_Y135_startBand2,temp_sao_param_Y135_deltaband,temp_sao_param_Y135_offset);
        }
        break; 
        case 3:
        {
            *mindist = curdist45;
            *minrate = currate45;
            copySAOParam_for_blk_onecomponent_pipe(sao_cur_param_modeIdc,sao_cur_param_typeIdc,sao_cur_param_startBand,sao_cur_param_startBand2,sao_cur_param_deltaband, *sao_cur_param_offset,temp_sao_param_Y45_modeIdc,temp_sao_param_Y45_typeIdc,temp_sao_param_Y45_startBand,temp_sao_param_Y45_startBand2,temp_sao_param_Y45_deltaband, *temp_sao_param_Y45_offset);
            //copySAOParam_for_blk_onecomponent_pipe(rec_sao_cur_param_modeIdc,rec_sao_cur_param_typeIdc,rec_sao_cur_param_startBand,rec_sao_cur_param_startBand2,rec_sao_cur_param_deltaband,rec_sao_cur_param_offset,temp_sao_param_Y45_modeIdc,temp_sao_param_Y45_typeIdc,temp_sao_param_Y45_startBand,temp_sao_param_Y45_startBand2,temp_sao_param_Y45_deltaband,temp_sao_param_Y45_offset);
        }
        break; 
        case 4:
        {
            *mindist = curdistBO;
            *minrate = currateBO;
            copySAOParam_for_blk_onecomponent_pipe(sao_cur_param_modeIdc,sao_cur_param_typeIdc,sao_cur_param_startBand,sao_cur_param_startBand2,sao_cur_param_deltaband, *sao_cur_param_offset,temp_sao_param_YBO_modeIdc,temp_sao_param_YBO_typeIdc,temp_sao_param_YBO_startBand,temp_sao_param_YBO_startBand2,temp_sao_param_YBO_deltaband, *temp_sao_param_YBO_offset);
            //copySAOParam_for_blk_onecomponent_pipe(rec_sao_cur_param_modeIdc,rec_sao_cur_param_typeIdc,rec_sao_cur_param_startBand,rec_sao_cur_param_startBand2,rec_sao_cur_param_deltaband,rec_sao_cur_param_offset,temp_sao_param_YBO_modeIdc,temp_sao_param_YBO_typeIdc,temp_sao_param_YBO_startBand,temp_sao_param_YBO_startBand2,temp_sao_param_YBO_deltaband,temp_sao_param_YBO_offset);
        }
        break; 
        default:
        break; 
    }
}
static void sao_rdo_result2(
    int* sao_cur_param_modeIdc, int* sao_cur_param_typeIdc, int* sao_cur_param_startBand, int* sao_cur_param_startBand2, int* sao_cur_param_deltaband, int sao_cur_param_offset[32],
    int* temp_sao_param_Y0_modeIdc,   int* temp_sao_param_Y0_typeIdc,   int temp_sao_param_Y0_offset[32],
    int* temp_sao_param_Y90_modeIdc,  int* temp_sao_param_Y90_typeIdc,  int temp_sao_param_Y90_offset[32],
    int* temp_sao_param_Y135_modeIdc, int* temp_sao_param_Y135_typeIdc, int temp_sao_param_Y135_offset[32],
    int* temp_sao_param_Y45_modeIdc,  int* temp_sao_param_Y45_typeIdc,  int temp_sao_param_Y45_offset[32],
    int* temp_sao_param_YBO_modeIdc,  int* temp_sao_param_YBO_typeIdc,  int* temp_sao_param_YBO_startBand,  int* temp_sao_param_YBO_startBand2,  int* temp_sao_param_YBO_deltaband,  int temp_sao_param_YBO_offset[32],
    SAO_BIG_SIZE* mincost,   SAO_BIG_SIZE* mindist,   int* minrate,
    SAO_BIG_SIZE curcost0,   SAO_BIG_SIZE curdist0,   int currate0, 
	SAO_BIG_SIZE curcost90,  SAO_BIG_SIZE curdist90,  int currate90, 
	SAO_BIG_SIZE curcost135, SAO_BIG_SIZE curdist135, int currate135, 
	SAO_BIG_SIZE curcost45,  SAO_BIG_SIZE curdist45,  int currate45,
    SAO_BIG_SIZE curcostBO,  SAO_BIG_SIZE curdistBO,  int currateBO)
{
    int temp = 5;
    if (curcost0 < *mincost) {
        temp = 0;
        *mincost = curcost0;
    }
    if (curcost90 < *mincost) {
        temp = 1;
        *mincost = curcost90;
    }
    if (curcost135 < *mincost) {
        temp = 2;
        *mincost = curcost135;
    }
    if (curcost45 < *mincost) {
        temp = 3;
        *mincost = curcost45;
    }
    if (curcostBO < *mincost) {
        temp = 4;
        *mincost = curcostBO;
    }
    switch (temp) {
    case 0:
    {
        *mindist = curdist0;
        *minrate = currate0;
        copySAOParam_for_blk_onecomponent_pipe2(sao_cur_param_modeIdc,     sao_cur_param_typeIdc,     sao_cur_param_startBand, sao_cur_param_startBand2, sao_cur_param_deltaband, sao_cur_param_offset, 
                                               temp_sao_param_Y0_modeIdc, temp_sao_param_Y0_typeIdc,                                                                              temp_sao_param_Y0_offset);
    }
    break;
    case 1:
    {
        *mindist = curdist90;
        *minrate = currate90;
        copySAOParam_for_blk_onecomponent_pipe2(sao_cur_param_modeIdc,      sao_cur_param_typeIdc,      sao_cur_param_startBand, sao_cur_param_startBand2, sao_cur_param_deltaband, sao_cur_param_offset, 
                                                temp_sao_param_Y90_modeIdc, temp_sao_param_Y90_typeIdc,                                                                             temp_sao_param_Y90_offset);
    }
    break;
    case 2:
    {
        *mindist = curdist135;
        *minrate = currate135;
        copySAOParam_for_blk_onecomponent_pipe2(sao_cur_param_modeIdc,       sao_cur_param_typeIdc,      sao_cur_param_startBand, sao_cur_param_startBand2, sao_cur_param_deltaband, sao_cur_param_offset, 
                                                temp_sao_param_Y135_modeIdc, temp_sao_param_Y135_typeIdc,                                                                            temp_sao_param_Y135_offset);
    }
    break;
    case 3:
    {
        *mindist = curdist45;
        *minrate = currate45;
        copySAOParam_for_blk_onecomponent_pipe2(sao_cur_param_modeIdc,      sao_cur_param_typeIdc,      sao_cur_param_startBand, sao_cur_param_startBand2, sao_cur_param_deltaband, sao_cur_param_offset, 
                                                temp_sao_param_Y45_modeIdc, temp_sao_param_Y45_typeIdc,                                                                             temp_sao_param_Y45_offset);
    }
    break;
    case 4:
    {
        *mindist = curdistBO;
        *minrate = currateBO;
        copySAOParam_for_blk_onecomponent_pipe(sao_cur_param_modeIdc,      sao_cur_param_typeIdc,      sao_cur_param_startBand,      sao_cur_param_startBand2,      sao_cur_param_deltaband,      sao_cur_param_offset, 
                                               temp_sao_param_YBO_modeIdc, temp_sao_param_YBO_typeIdc, temp_sao_param_YBO_startBand, temp_sao_param_YBO_startBand2, temp_sao_param_YBO_deltaband, temp_sao_param_YBO_offset);
    }
    break;
    default:
        break;
    }
}

void sao_rdo_eo(int pSaoStat_UEO_diff[4][32], int pSaoStat_UEO_count[4][32], int temp_sao_param_UEO_modeIdc[4], int temp_sao_param_UEO_typeIdc[4], int temp_sao_param_UEO_offset[4][32],
    SAO_BIG_SIZE sao_lambda, SAO_BIG_SIZE curcost[4], int currate[4], SAO_BIG_SIZE curdist[4], int EO_OFFSET_MAP_DULPI2[4][8])
{
#pragma HLS INLINE off
    for (int type = 0; type < 4; type++) {
        sao_rdcost_comp_new_eo(type, pSaoStat_UEO_diff[type], pSaoStat_UEO_count[type], &temp_sao_param_UEO_modeIdc[type], &temp_sao_param_UEO_typeIdc[type], temp_sao_param_UEO_offset[type], sao_lambda, &currate[type], &curdist[type], &curcost[type], EO_OFFSET_MAP_DULPI2[type]);
    }
}

static void sao_rdo_y(bool slice_sao_enable,
    int *sao_cur_param_modeIdc,   int *sao_cur_param_typeIdc,     int *sao_cur_param_startBand,     int *sao_cur_param_startBand2,     int *sao_cur_param_deltaband,     int (*sao_cur_param_offset)[32],
    SAO_BIG_SIZE sao_lambda,  int *minrate, SAO_BIG_SIZE *mindist,
    int pSaoStat_YEO_diff[4][32], int pSaoStat_YEO_count[4][32],
    int *pSaoStat_YBO_diff,       int *pSaoStat_YBO_count)
{
#pragma HLS inline off
    static int EO_OFFSET_MAP_DULPI1[4][8] = { {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)} };
    static int EO_OFFSET_MAP_DULPI15[8] = { (3), (1), (0), (2), (4), (5), (6), (7) };
    SAO_BIG_SIZE mincost = 0;
    int          currate[4] = { 0 , 0, 0, 0 };
    SAO_BIG_SIZE curdist[4] = { 0 , 0, 0, 0 };
    SAO_BIG_SIZE curcost[4] = { 0 , 0, 0, 0 };
    SAO_BIG_SIZE curcostBO  = 0;
    int          currateBO  = 0;
    SAO_BIG_SIZE curdistBO  = 0;
    int temp_sao_param_YEO_modeIdc[4];
    int temp_sao_param_YEO_typeIdc[4];
    int temp_sao_param_YEO_offset [4][32];

    int temp_sao_param_YBO_modeIdc = 0;
    int temp_sao_param_YBO_typeIdc = 0;
    int temp_sao_param_YBO_offset[32];
    int temp_sao_param_YBO_startBand  = 0;
    int temp_sao_param_YBO_startBand2 = 0;
    int temp_sao_param_YBO_deltaband  = 0;
#pragma HLS array_partition variable=temp_sao_param_YBO_offset  complete dim=0

    for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        temp_sao_param_YEO_offset[0][i] = 0;
        temp_sao_param_YEO_offset[1][i] = 0;
        temp_sao_param_YEO_offset[2][i] = 0;
        temp_sao_param_YEO_offset[3][i] = 0;
        temp_sao_param_YBO_offset   [i] = 0;
    }

    // Init as SAO OFF MODE
    *sao_cur_param_modeIdc = (int)SAO_MODE_OFF;
    *minrate = 1;
    *mindist = 0;
    mincost = (sao_lambda + ((1 << LAMBDA_SCALE) / 2)) >> LAMBDA_SCALE;

    /////SAO NEW MODE/////
    if (slice_sao_enable) {
        sao_rdo_eo            (pSaoStat_YEO_diff, pSaoStat_YEO_count, temp_sao_param_YEO_modeIdc, temp_sao_param_YEO_typeIdc, temp_sao_param_YEO_offset, sao_lambda, curcost, currate, curdist, EO_OFFSET_MAP_DULPI1);
        sao_rdcost_comp_new_bo(pSaoStat_YBO_diff, pSaoStat_YBO_count, &temp_sao_param_YBO_modeIdc, &temp_sao_param_YBO_typeIdc, &temp_sao_param_YBO_startBand, &temp_sao_param_YBO_startBand2,  &temp_sao_param_YBO_deltaband,  &temp_sao_param_YBO_offset,  sao_lambda, &currateBO,  &curdistBO,  &curcostBO,  EO_OFFSET_MAP_DULPI15);
        sao_rdo_result2(sao_cur_param_modeIdc,     sao_cur_param_typeIdc,     sao_cur_param_startBand,     sao_cur_param_startBand2,     sao_cur_param_deltaband,     *sao_cur_param_offset,
            &temp_sao_param_YEO_modeIdc[0], &temp_sao_param_YEO_typeIdc[0], temp_sao_param_YEO_offset[0],
            &temp_sao_param_YEO_modeIdc[1], &temp_sao_param_YEO_typeIdc[1], temp_sao_param_YEO_offset[1],
            &temp_sao_param_YEO_modeIdc[2], &temp_sao_param_YEO_typeIdc[2], temp_sao_param_YEO_offset[2],
            &temp_sao_param_YEO_modeIdc[3], &temp_sao_param_YEO_typeIdc[3], temp_sao_param_YEO_offset[3],
            &temp_sao_param_YBO_modeIdc,  &temp_sao_param_YBO_typeIdc,  &temp_sao_param_YBO_startBand,  &temp_sao_param_YBO_startBand2,  &temp_sao_param_YBO_deltaband,  temp_sao_param_YBO_offset,
            &mincost, mindist, minrate,
            curcost[0], curdist[0], currate[0], curcost[1], curdist[1], currate[1], curcost[2], curdist[2], currate[2], curcost[3], curdist[3], currate[3],
            curcostBO, curdistBO, currateBO);
    } else {  // !slice_sao_enable
        *mindist = 0;
        *minrate = 0;
    }
}

static void sao_rdo_u(bool slice_sao_enable,
    int *sao_cur_param_modeIdc,   int *sao_cur_param_typeIdc,    int *sao_cur_param_startBand,    int *sao_cur_param_startBand2,    int *sao_cur_param_deltaband,     int (*sao_cur_param_offset)[32],
    SAO_BIG_SIZE sao_lambda,  int *minrate, SAO_BIG_SIZE *mindist,
    int pSaoStat_UEO_diff[4][32], int pSaoStat_UEO_count[4][32],
    int pSaoStat_UBO_diff[32],    int pSaoStat_UBO_count[32])
{
#pragma HLS inline off
    static int EO_OFFSET_MAP_DULPI2[4][8] = { {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)} };
    static int EO_OFFSET_MAP_DULPI25[8] = { (3), (1), (0), (2), (4), (5), (6), (7) };

    SAO_BIG_SIZE mincost = 0;
    SAO_BIG_SIZE curcost[4] = { 0, 0, 0, 0 };
    int          currate[4] = { 0, 0, 0, 0 };
    SAO_BIG_SIZE curdist[4] = { 0, 0, 0, 0 };
    SAO_BIG_SIZE curcostBO = 0;
    int          currateBO = 0;
    SAO_BIG_SIZE curdistBO = 0;

    int temp_sao_param_UEO_modeIdc[4];
    int temp_sao_param_UEO_typeIdc[4];
    int temp_sao_param_UEO_offset [4][32];

    int temp_sao_param_UBO_modeIdc = 0;
    int temp_sao_param_UBO_typeIdc = 0;
    int temp_sao_param_UBO_offset[32];
    int temp_sao_param_UBO_startBand  = 0;
    int temp_sao_param_UBO_startBand2 = 0;
    int temp_sao_param_UBO_deltaband  = 0;
#pragma HLS array_partition variable=temp_sao_param_UBO_offset complete  dim=0

    for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        temp_sao_param_UEO_offset[0][i] = 0;
        temp_sao_param_UEO_offset[1][i] = 0;
        temp_sao_param_UEO_offset[2][i] = 0;
        temp_sao_param_UEO_offset[3][i] = 0;
        temp_sao_param_UBO_offset[i] = 0;
    }

    // Init as SAO OFF MODE
    *sao_cur_param_modeIdc = (int)SAO_MODE_OFF;
    *minrate = 1;
    *mindist = 0;
    mincost = (sao_lambda + ((1 << LAMBDA_SCALE) / 2)) >> LAMBDA_SCALE;

    /////SAO NEW MODE/////
    if (slice_sao_enable) {
        sao_rdo_eo            (pSaoStat_UEO_diff, pSaoStat_UEO_count, temp_sao_param_UEO_modeIdc, temp_sao_param_UEO_typeIdc, temp_sao_param_UEO_offset, sao_lambda, curcost, currate, curdist, EO_OFFSET_MAP_DULPI2);
        sao_rdcost_comp_new_bo(pSaoStat_UBO_diff, pSaoStat_UBO_count, &temp_sao_param_UBO_modeIdc, &temp_sao_param_UBO_typeIdc, &temp_sao_param_UBO_startBand, &temp_sao_param_UBO_startBand2, &temp_sao_param_UBO_deltaband, &temp_sao_param_UBO_offset,  sao_lambda, &currateBO,  &curdistBO,  &curcostBO,  EO_OFFSET_MAP_DULPI25);
        sao_rdo_result2(sao_cur_param_modeIdc,           sao_cur_param_typeIdc,    sao_cur_param_startBand,    sao_cur_param_startBand2,    sao_cur_param_deltaband, *sao_cur_param_offset,
            &temp_sao_param_UEO_modeIdc[0], &temp_sao_param_UEO_typeIdc[0], temp_sao_param_UEO_offset[0],
            &temp_sao_param_UEO_modeIdc[1], &temp_sao_param_UEO_typeIdc[1], temp_sao_param_UEO_offset[1],
            &temp_sao_param_UEO_modeIdc[2], &temp_sao_param_UEO_typeIdc[2], temp_sao_param_UEO_offset[2],
            &temp_sao_param_UEO_modeIdc[3], &temp_sao_param_UEO_typeIdc[3], temp_sao_param_UEO_offset[3],
            &temp_sao_param_UBO_modeIdc,  &temp_sao_param_UBO_typeIdc,  &temp_sao_param_UBO_startBand,  &temp_sao_param_UBO_startBand2,  &temp_sao_param_UBO_deltaband,  temp_sao_param_UBO_offset,
            &mincost, mindist, minrate,
            curcost[0], curdist[0], currate[0], curcost[1], curdist[1], currate[1], curcost[2], curdist[2], currate[2], curcost[3], curdist[3], currate[3],
            curcostBO, curdistBO, currateBO);
    } else {
        *mindist = 0;
        *minrate = 0;
    }
}

static void sao_rdo_v(bool slice_sao_enable,
    int *sao_cur_param_modeIdc,   int *sao_cur_param_typeIdc, int *sao_cur_param_startBand, int *sao_cur_param_startBand2, int *sao_cur_param_deltaband, int (*sao_cur_param_offset)[32],
    SAO_BIG_SIZE sao_lambda,  int *minrate, SAO_BIG_SIZE *mindist,
    int pSaoStat_VEO_diff[4][32], int pSaoStat_VEO_count[4][32], 
    int pSaoStat_VBO_diff[32],    int pSaoStat_VBO_count[32])
{
#pragma HLS inline off
    static int EO_OFFSET_MAP_DULPI3[4][8] = { {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)}, {(3), (1), (0), (2), (4), (5), (6), (7)} };
    static int EO_OFFSET_MAP_DULPI35[8] = { (3), (1), (0), (2), (4), (5), (6), (7) };
    SAO_BIG_SIZE mincost = 0;

    SAO_BIG_SIZE curcost[4] = { 0, 0, 0, 0 };
    int          currate[4] = { 0, 0, 0, 0 };
    SAO_BIG_SIZE curdist[4] = { 0, 0, 0, 0 };
    SAO_BIG_SIZE curcostBO = 0;
    int          currateBO = 0;
    SAO_BIG_SIZE curdistBO = 0;

    int temp_sao_param_VEO_modeIdc[4];
    int temp_sao_param_VEO_typeIdc[4];
    int temp_sao_param_VEO_offset [4][32];
    int temp_sao_param_VBO_modeIdc = 0;
    int temp_sao_param_VBO_typeIdc = 0;
    int temp_sao_param_VBO_offset[32];
    int temp_sao_param_VBO_startBand  = 0;
    int temp_sao_param_VBO_startBand2 = 0;
    int temp_sao_param_VBO_deltaband  = 0;
#pragma HLS array_partition variable=temp_sao_param_VBO_offset  complete dim=0

    for (int i = 0; i < 32; i++) {
#pragma HLS unroll
        temp_sao_param_VEO_offset[0][i] = 0;
        temp_sao_param_VEO_offset[1][i] = 0;
        temp_sao_param_VEO_offset[2][i] = 0;
        temp_sao_param_VEO_offset[3][i] = 0;
        temp_sao_param_VBO_offset[i] = 0;
    }

    // Init as SAO OFF MODE
    *sao_cur_param_modeIdc = (int)SAO_MODE_OFF;
    *minrate = 1;
    *mindist = 0;
    mincost = (sao_lambda + ((1 << LAMBDA_SCALE) / 2)) >> LAMBDA_SCALE;

    /////SAO NEW MODE/////
    if (slice_sao_enable) {
        sao_rdo_eo            (pSaoStat_VEO_diff, pSaoStat_VEO_count, temp_sao_param_VEO_modeIdc, temp_sao_param_VEO_typeIdc, temp_sao_param_VEO_offset, sao_lambda, curcost, currate, curdist, EO_OFFSET_MAP_DULPI3);
        sao_rdcost_comp_new_bo(pSaoStat_VBO_diff, pSaoStat_VBO_count, &temp_sao_param_VBO_modeIdc,  &temp_sao_param_VBO_typeIdc,  &temp_sao_param_VBO_startBand, &temp_sao_param_VBO_startBand2, &temp_sao_param_VBO_deltaband, &temp_sao_param_VBO_offset, sao_lambda, &currateBO,  &curdistBO,  &curcostBO,  EO_OFFSET_MAP_DULPI35);
        sao_rdo_result2(sao_cur_param_modeIdc, sao_cur_param_typeIdc, sao_cur_param_startBand, sao_cur_param_startBand2, sao_cur_param_deltaband, *sao_cur_param_offset,
            &temp_sao_param_VEO_modeIdc[0], &temp_sao_param_VEO_typeIdc[0], temp_sao_param_VEO_offset[0],
            &temp_sao_param_VEO_modeIdc[1], &temp_sao_param_VEO_typeIdc[1], temp_sao_param_VEO_offset[1],
            &temp_sao_param_VEO_modeIdc[2], &temp_sao_param_VEO_typeIdc[2], temp_sao_param_VEO_offset[2],
            &temp_sao_param_VEO_modeIdc[3], &temp_sao_param_VEO_typeIdc[3], temp_sao_param_VEO_offset[3],
            &temp_sao_param_VBO_modeIdc,  &temp_sao_param_VBO_typeIdc,  &temp_sao_param_VBO_startBand, &temp_sao_param_VBO_startBand2, &temp_sao_param_VBO_deltaband, temp_sao_param_VBO_offset,
            &mincost, mindist, minrate,
            curcost[0], curdist[0], currate[0], curcost[1], curdist[1], currate[1], curcost[2], curdist[2], currate[2], curcost[3], curdist[3], currate[3],
            curcostBO, curdistBO, currateBO);
    } else {
        *mindist = 0;
        *minrate = 0;
    }
}


static SAO_BIG_SIZE sao_new_param_rdo(/*int *bin_count,*/ unsigned char slice_sao_enable[3], bool MergeLeftAvail, bool MergeUpAvail, SAO_BIG_SIZE sao_lambda[3],
    int *sao_param_temp_new_Y_modeIdc, int *sao_param_temp_new_Y_typeIdc, int *sao_param_temp_new_Y_startBand, int *sao_param_temp_new_Y_startBand2, int *sao_param_temp_new_Y_deltaband, int (*sao_param_temp_new_Y_offset)[32],
    int *sao_param_temp_new_U_modeIdc, int *sao_param_temp_new_U_typeIdc, int *sao_param_temp_new_U_startBand, int *sao_param_temp_new_U_startBand2, int *sao_param_temp_new_U_deltaband, int (*sao_param_temp_new_U_offset)[32],
    int *sao_param_temp_new_V_modeIdc, int *sao_param_temp_new_V_typeIdc, int *sao_param_temp_new_V_startBand, int *sao_param_temp_new_V_startBand2, int *sao_param_temp_new_V_deltaband, int (*sao_param_temp_new_V_offset)[32],
    int pSaoStat_YEO_diff[4][32], int pSaoStat_YEO_count[4][32], 
    int pSaoStat_YBO_diff[32],    int pSaoStat_YBO_count[32],
    int pSaoStat_UEO_diff[4][32], int pSaoStat_UEO_count[4][32], 
    int pSaoStat_UBO_diff[32],    int pSaoStat_UBO_count[32],
    int pSaoStat_VEO_diff[4][32], int pSaoStat_VEO_count[4][32], 
    int pSaoStat_VBO_diff[32],    int pSaoStat_VBO_count[32])
{
#pragma HLS inline off
    SAO_BIG_SIZE mindist_Y = 0;
    SAO_BIG_SIZE mindist_U = 0;
    SAO_BIG_SIZE mindist_V = 0;
    int minrate_Y = 0;
    int minrate_U = 0;
    int minrate_V = 0;
    SAO_BIG_SIZE sao_lambda_Y;
    SAO_BIG_SIZE sao_lambda_U;
    SAO_BIG_SIZE sao_lambda_V;
    bool slice_sao_enable_Y;
    bool slice_sao_enable_U;
    bool slice_sao_enable_V;
    SAO_BIG_SIZE normmodecost;
    int mergeflag_rate = 0;
    int bin_count = 0;
    bin_count = 0;

    *sao_param_temp_new_Y_modeIdc = ((int )SAO_MODE_OFF);
//////////////////////////////////////////coding MERGE FLAG///////////////////////////////////////////////////////////
    if (MergeLeftAvail | MergeUpAvail) {
        //RDO_sao_mergeflag_LCU(MergeLeftAvail, MergeUpAvail, sao_param_temp_new_Y_modeIdc, sao_param_temp_new_Y_typeIdc, bin_count);
        int value2 = (int)MergeLeftAvail + (int)MergeUpAvail;
        RDO_sao_mergeflag_AEC(0, value2, &bin_count);
        mergeflag_rate =  bin_count;
    }
///////////////////////////////////////////////SAO NEW MODE///////////////////////////////////////////////////////////
    sao_lambda_Y = sao_lambda[0];
    sao_lambda_U = sao_lambda[1];
    sao_lambda_V = sao_lambda[2];
    slice_sao_enable_Y = (bool)slice_sao_enable[0];
    slice_sao_enable_U = (bool)slice_sao_enable[1];
    slice_sao_enable_V = (bool)slice_sao_enable[2];
    sao_rdo_y(slice_sao_enable_Y,
        sao_param_temp_new_Y_modeIdc, sao_param_temp_new_Y_typeIdc, sao_param_temp_new_Y_startBand, sao_param_temp_new_Y_startBand2, sao_param_temp_new_Y_deltaband, sao_param_temp_new_Y_offset,
        sao_lambda_Y, &minrate_Y, &mindist_Y,
        pSaoStat_YEO_diff, pSaoStat_YEO_count, pSaoStat_YBO_diff, pSaoStat_YBO_count);
    sao_rdo_u(slice_sao_enable_U,
        sao_param_temp_new_U_modeIdc, sao_param_temp_new_U_typeIdc, sao_param_temp_new_U_startBand, sao_param_temp_new_U_startBand2, sao_param_temp_new_U_deltaband, sao_param_temp_new_U_offset,
        sao_lambda_U, &minrate_U, &mindist_U,
        pSaoStat_UEO_diff, pSaoStat_UEO_count, pSaoStat_UBO_diff, pSaoStat_UBO_count);
    sao_rdo_v(slice_sao_enable_V,
        sao_param_temp_new_V_modeIdc, sao_param_temp_new_V_typeIdc, sao_param_temp_new_V_startBand, sao_param_temp_new_V_startBand2, sao_param_temp_new_V_deltaband, sao_param_temp_new_V_offset,
        sao_lambda_V, &minrate_V, &mindist_V,
        pSaoStat_VEO_diff, pSaoStat_VEO_count, pSaoStat_VBO_diff, pSaoStat_VBO_count);

    normmodecost = ((SAO_BIG_SIZE)(mindist_Y + mindist_U + mindist_V)) * (1 << LAMBDA_SCALE) / sao_lambda_Y;
    normmodecost += ((SAO_BIG_SIZE)minrate_Y) + ((SAO_BIG_SIZE)minrate_U) + ((SAO_BIG_SIZE)minrate_V) + ((SAO_BIG_SIZE)mergeflag_rate);

    return normmodecost;
}


static void get_sao_param(/*int pic_width,*/ int log2_max_cuwh, /*int pic_width_in_scu, int pic_width_in_lcu,*/ /*int *bin_count,*/ unsigned char slice_sao_enable[3],
    int LCU_col_num, int lcu_pos, int pix_y, int pix_x, SAO_BIG_SIZE *sao_labmda, 
    int pSaoStat_YEO_diff[4][32], int pSaoStat_YEO_count[4][32], 
    int pSaoStat_YBO_diff[32],    int pSaoStat_YBO_count[32],
    int pSaoStat_UEO_diff[4][32], int pSaoStat_UEO_count[4][32], 
    int pSaoStat_UBO_diff[32],    int pSaoStat_UBO_count[32],
    int pSaoStat_VEO_diff[4][32], int pSaoStat_VEO_count[4][32], 
    int pSaoStat_VBO_diff[32],    int pSaoStat_VBO_count[32],
    int rec_sao_param_local_y_modeIdc[62],    int rec_sao_param_local_y_typeIdc[62],     
    int rec_sao_param_local_y_startBand[62],  int rec_sao_param_local_y_startBand2[62], int rec_sao_param_local_y_deltaband[62], int rec_sao_param_local_y_offset[62][32], 
    int rec_sao_param_local_u_modeIdc[62],    int rec_sao_param_local_u_typeIdc[62],     
    int rec_sao_param_local_u_startBand[62],  int rec_sao_param_local_u_startBand2[62], int rec_sao_param_local_u_deltaband[62], int rec_sao_param_local_u_offset[62][32], 
    int rec_sao_param_local_v_modeIdc[62],    int rec_sao_param_local_v_typeIdc[62],     
    int rec_sao_param_local_v_startBand[62],  int rec_sao_param_local_v_startBand2[62], int rec_sao_param_local_v_deltaband[62], int rec_sao_param_local_v_offset[62][32], 
    SAOBlkParam cur_saoParam[3], SAOBlkParam rec_saoParam[60 + 2][3])
{
#pragma HLS inline off

    //int pix_x = mb_x << 2;
    //int pix_y = mb_y << 2;
    bool MergeLeftAvail = false;
    bool MergeUpAvail   = false;
//  SAO_BIG_SIZE mcost       = (SAO_BIG_SIZE)SAO_MAX_BIG;
    SAO_BIG_SIZE mincost     = (SAO_BIG_SIZE)SAO_MAX_BIG;
    SAO_BIG_SIZE mcost_merge = (SAO_BIG_SIZE)SAO_MAX_BIG;
    SAO_BIG_SIZE mcost_new   = (SAO_BIG_SIZE)SAO_MAX_BIG;

    int lcu_idx = lcu_pos % (LCU_col_num + 2);

    static int sao_param_temp_merge_offset[3][32];
    int sao_param_temp_merge_deltaband[3];
    int sao_param_temp_merge_startBand2[3];
    int sao_param_temp_merge_startBand[3];
    int sao_param_temp_merge_typeIdc[3];
    int sao_param_temp_merge_modeIdc[3];
    int rec_sao_param_temp_merge_modeIdc[3];
    int rec_sao_param_temp_merge_typeIdc[3];
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_merge_offset complete dim=0
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_merge_deltaband complete dim=0
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_merge_startBand2 complete dim=0
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_merge_startBand complete dim=0
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_merge_typeIdc complete dim=0
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_merge_modeIdc complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_sao_param_temp_merge_modeIdc complete dim=0
#pragma HLS ARRAY_PARTITION variable=rec_sao_param_temp_merge_typeIdc complete dim=0

    static int sao_param_temp_new_Y_offset[32];
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_new_Y_offset complete dim=0
    int sao_param_temp_new_Y_deltaband = 0;
    int sao_param_temp_new_Y_startBand2 = 0;
    int sao_param_temp_new_Y_startBand = 0;
    int sao_param_temp_new_Y_typeIdc = 0;
    int sao_param_temp_new_Y_modeIdc = 0;
    static int sao_param_temp_new_U_offset[32];
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_new_U_offset complete dim=0
    int sao_param_temp_new_U_deltaband = 0;
    int sao_param_temp_new_U_startBand2 = 0;
    int sao_param_temp_new_U_startBand = 0;
    int sao_param_temp_new_U_typeIdc = 0;
    int sao_param_temp_new_U_modeIdc = 0;
    static int sao_param_temp_new_V_offset[32];
#pragma HLS ARRAY_PARTITION variable=sao_param_temp_new_V_offset complete dim=0
    int sao_param_temp_new_V_deltaband = 0;
    int sao_param_temp_new_V_startBand2 = 0;
    int sao_param_temp_new_V_startBand = 0;
    int sao_param_temp_new_V_typeIdc = 0;
    int sao_param_temp_new_V_modeIdc = 0;


    int i, j;
    for(i = 0; i < 32; i++)
    {
#pragma HLS unroll
        sao_param_temp_new_Y_offset[i]=0;
        sao_param_temp_new_U_offset[i]=0;
        sao_param_temp_new_V_offset[i]=0;
    }

    if (!slice_sao_enable[0] && !slice_sao_enable[1] && !slice_sao_enable[2]) 
    {
        //printf("here\n");
        off_sao(lcu_idx, 
        rec_sao_param_local_y_modeIdc,rec_sao_param_local_y_typeIdc,rec_sao_param_local_y_startBand,rec_sao_param_local_y_startBand2,rec_sao_param_local_y_deltaband,rec_sao_param_local_y_offset,
        rec_sao_param_local_u_modeIdc,rec_sao_param_local_u_typeIdc,rec_sao_param_local_u_startBand,rec_sao_param_local_u_startBand2,rec_sao_param_local_u_deltaband,rec_sao_param_local_u_offset,
        rec_sao_param_local_v_modeIdc,rec_sao_param_local_v_typeIdc,rec_sao_param_local_v_startBand,rec_sao_param_local_v_startBand2,rec_sao_param_local_v_deltaband,rec_sao_param_local_v_offset,cur_saoParam);
        return;
    }

    //printf("%d \n",*bin_count);
	/*
	printf("log2_max_cuwh=%d LCU_col_num=%d lcu_pos=%d pix_y=%d pix_x=%d\n", log2_max_cuwh, LCU_col_num, lcu_pos, pix_y, pix_x);
	for (int i = 0; i < 3; i++)
	{
		printf("slice_sao_enable[%d]=%d\n", i, slice_sao_enable[i]);
		printf("sao_labmda[%d]=%d\n", i, sao_labmda[i]);
	}
	*/
#if 0
	FILE *file_1;
	file_1 = fopen("rec_saoParam.txt", "w+");
	fprintf(file_1, "lcu_idx=%d\n", lcu_idx);
	fprintf(file_1, "%d %d %d %d %d\n", rec_sao_param_local_y_deltaband[lcu_idx], rec_sao_param_local_y_modeIdc[lcu_idx], rec_sao_param_local_y_startBand[lcu_idx], rec_sao_param_local_y_startBand2[lcu_idx], rec_sao_param_local_y_typeIdc[lcu_idx]);
	fprintf(file_1, "%d %d %d %d %d\n", rec_sao_param_local_u_deltaband[lcu_idx], rec_sao_param_local_u_modeIdc[lcu_idx], rec_sao_param_local_u_startBand[lcu_idx], rec_sao_param_local_u_startBand2[lcu_idx], rec_sao_param_local_u_typeIdc[lcu_idx]);
	fprintf(file_1, "%d %d %d %d %d\n", rec_sao_param_local_v_deltaband[lcu_idx], rec_sao_param_local_v_modeIdc[lcu_idx], rec_sao_param_local_v_startBand[lcu_idx], rec_sao_param_local_v_startBand2[lcu_idx], rec_sao_param_local_v_typeIdc[lcu_idx]);
	for (int i = 0; i < 32; i++)
	{
		fprintf(file_1, "%d ", rec_sao_param_local_y_offset[lcu_idx][i]);
	}
	fprintf(file_1, "\n");
	for (int i = 0; i < 32; i++)
	{
		fprintf(file_1, "%d ", rec_sao_param_local_u_offset[lcu_idx][i]);
	}
	fprintf(file_1, "\n");
	for (int i = 0; i < 32; i++)
	{
		fprintf(file_1, "%d ", rec_sao_param_local_v_offset[lcu_idx][i]);
	}
	fprintf(file_1, "\n");
	fclose(file_1);
#endif
    mcost_merge = sao_merge_param_rdo(/*pic_width,*/ log2_max_cuwh, /*pic_width_in_scu, pic_width_in_lcu,*/ LCU_col_num, /*bin_count,*/ slice_sao_enable, lcu_pos,pix_y, pix_x, sao_labmda,
        sao_param_temp_merge_modeIdc,    sao_param_temp_merge_typeIdc,    sao_param_temp_merge_startBand,    sao_param_temp_merge_startBand2,    sao_param_temp_merge_deltaband,    sao_param_temp_merge_offset,
        rec_sao_param_temp_merge_modeIdc,rec_sao_param_temp_merge_typeIdc, //rec_sao_param_temp_merge_startBand,rec_sao_param_temp_merge_startBand2,rec_sao_param_temp_merge_deltaband,rec_sao_param_temp_merge_offset,
        rec_sao_param_local_y_modeIdc, rec_sao_param_local_y_typeIdc, rec_sao_param_local_y_startBand, rec_sao_param_local_y_startBand2, rec_sao_param_local_y_deltaband, rec_sao_param_local_y_offset,
        rec_sao_param_local_u_modeIdc, rec_sao_param_local_u_typeIdc, rec_sao_param_local_u_startBand, rec_sao_param_local_u_startBand2, rec_sao_param_local_u_deltaband, rec_sao_param_local_u_offset,
        rec_sao_param_local_v_modeIdc, rec_sao_param_local_v_typeIdc, rec_sao_param_local_v_startBand, rec_sao_param_local_v_startBand2, rec_sao_param_local_v_deltaband, rec_sao_param_local_v_offset,
        &MergeLeftAvail, &MergeUpAvail,
      //pSaoStat_YEO0_diff, pSaoStat_YEO0_count, pSaoStat_YEO90_diff, pSaoStat_YEO90_count, pSaoStat_YEO135_diff, pSaoStat_YEO135_count, pSaoStat_YEO45_diff, pSaoStat_YEO45_count, 
        pSaoStat_YEO_diff[0], pSaoStat_YEO_count[0], pSaoStat_YEO_diff[1], pSaoStat_YEO_count[1], pSaoStat_YEO_diff[2], pSaoStat_YEO_count[2], pSaoStat_YEO_diff[3], pSaoStat_YEO_count[3], pSaoStat_YBO_diff, pSaoStat_YBO_count,
        pSaoStat_UEO_diff[0], pSaoStat_UEO_count[0], pSaoStat_UEO_diff[1], pSaoStat_UEO_count[1], pSaoStat_UEO_diff[2], pSaoStat_UEO_count[2], pSaoStat_UEO_diff[3], pSaoStat_UEO_count[3], pSaoStat_UBO_diff, pSaoStat_UBO_count,
        pSaoStat_VEO_diff[0], pSaoStat_VEO_count[0], pSaoStat_VEO_diff[1], pSaoStat_VEO_count[1], pSaoStat_VEO_diff[2], pSaoStat_VEO_count[2], pSaoStat_VEO_diff[3], pSaoStat_VEO_count[3], pSaoStat_VBO_diff, pSaoStat_VBO_count);
	

    mcost_new = sao_new_param_rdo(/*bin_count,*/ slice_sao_enable, MergeLeftAvail, MergeUpAvail, sao_labmda,
        &sao_param_temp_new_Y_modeIdc,     &sao_param_temp_new_Y_typeIdc,     &sao_param_temp_new_Y_startBand,     &sao_param_temp_new_Y_startBand2,     &sao_param_temp_new_Y_deltaband,     &sao_param_temp_new_Y_offset,
        &sao_param_temp_new_U_modeIdc,     &sao_param_temp_new_U_typeIdc,     &sao_param_temp_new_U_startBand,     &sao_param_temp_new_U_startBand2,     &sao_param_temp_new_U_deltaband,     &sao_param_temp_new_U_offset,
        &sao_param_temp_new_V_modeIdc,     &sao_param_temp_new_V_typeIdc,     &sao_param_temp_new_V_startBand,     &sao_param_temp_new_V_startBand2,     &sao_param_temp_new_V_deltaband,     &sao_param_temp_new_V_offset,
        pSaoStat_YEO_diff, pSaoStat_YEO_count, pSaoStat_YBO_diff, pSaoStat_YBO_count,
        pSaoStat_UEO_diff, pSaoStat_UEO_count, pSaoStat_UBO_diff, pSaoStat_UBO_count,
        pSaoStat_VEO_diff, pSaoStat_VEO_count, pSaoStat_VBO_diff, pSaoStat_VBO_count);

    //printf("%ld  %ld\n",mcost_merge,mcost_new);

    if (mcost_merge <= mcost_new)   // Use SAO Param calced by MERGE MODE
    {
        mincost = mcost_merge;
        copySAOParam_for_blk_2(cur_saoParam, sao_param_temp_merge_modeIdc,sao_param_temp_merge_typeIdc,sao_param_temp_merge_startBand,sao_param_temp_merge_startBand2,sao_param_temp_merge_deltaband,sao_param_temp_merge_offset);
        //copySAOParam_for_blk(rec_saoParam[lcu_pos%(LCU_col_num + 2)], rec_sao_param_temp_merge);
        //recSaoParamLocal

        rec_sao_param_local_y_modeIdc[lcu_idx]    = rec_sao_param_temp_merge_modeIdc[0];
        rec_sao_param_local_y_typeIdc[lcu_idx]    = rec_sao_param_temp_merge_typeIdc[0];
        rec_sao_param_local_y_startBand[lcu_idx]  = sao_param_temp_merge_startBand[0];
        rec_sao_param_local_y_startBand2[lcu_idx] = sao_param_temp_merge_startBand2[0];
        rec_sao_param_local_y_deltaband[lcu_idx]  = sao_param_temp_merge_deltaband[0];
        rec_sao_param_local_u_modeIdc[lcu_idx]    = rec_sao_param_temp_merge_modeIdc[1];
        rec_sao_param_local_u_typeIdc[lcu_idx]    = rec_sao_param_temp_merge_typeIdc[1];
        rec_sao_param_local_u_startBand[lcu_idx]  = sao_param_temp_merge_startBand[1];
        rec_sao_param_local_u_startBand2[lcu_idx] = sao_param_temp_merge_startBand2[1];
        rec_sao_param_local_u_deltaband[lcu_idx]  = sao_param_temp_merge_deltaband[1];
        rec_sao_param_local_v_modeIdc[lcu_idx]    = rec_sao_param_temp_merge_modeIdc[2];
        rec_sao_param_local_v_typeIdc[lcu_idx]    = rec_sao_param_temp_merge_typeIdc[2];
        rec_sao_param_local_v_startBand[lcu_idx]  = sao_param_temp_merge_startBand[2];
        rec_sao_param_local_v_startBand2[lcu_idx] = sao_param_temp_merge_startBand2[2];
        rec_sao_param_local_v_deltaband[lcu_idx]  = sao_param_temp_merge_deltaband[2];
		for (j = 0; j < 32; j++)

		{
#pragma HLS unroll
			rec_sao_param_local_y_offset[lcu_idx][j] = sao_param_temp_merge_offset[0][j];
			rec_sao_param_local_u_offset[lcu_idx][j] = sao_param_temp_merge_offset[1][j];
			rec_sao_param_local_v_offset[lcu_idx][j] = sao_param_temp_merge_offset[2][j];
		
		}
#if sao_print
		FILE *file7;
		file7 = fopen("merge_rec_sao_param_local.txt", "w+");
		fprintf(file7, "%d\n", rec_sao_param_temp_merge_modeIdc[0]);
		fprintf(file7, "%d\n", rec_sao_param_temp_merge_typeIdc[0]);
		fprintf(file7, "%d\n", sao_param_temp_merge_startBand[0]);
		fprintf(file7, "%d\n", sao_param_temp_merge_startBand2[0]);
		fprintf(file7, "%d\n", sao_param_temp_merge_deltaband[0]);
		fprintf(file7, "%d\n", rec_sao_param_temp_merge_modeIdc[1]);
		fprintf(file7, "%d\n", rec_sao_param_temp_merge_typeIdc[1]);
		fprintf(file7, "%d\n", sao_param_temp_merge_startBand[1]);
		fprintf(file7, "%d\n", sao_param_temp_merge_startBand2[1]);
		fprintf(file7, "%d\n", sao_param_temp_merge_deltaband[1]);
		fprintf(file7, "%d\n", rec_sao_param_temp_merge_modeIdc[2]);
		fprintf(file7, "%d\n", rec_sao_param_temp_merge_typeIdc[2]);
		fprintf(file7, "%d\n", sao_param_temp_merge_startBand[2]);
		fprintf(file7, "%d\n", sao_param_temp_merge_startBand2[2]);
		fprintf(file7, "%d\n", sao_param_temp_merge_deltaband[2]);
		fprintf(file7, "\n");
			for (j = 0; j < 32; j++)
		{

			fprintf(file7, "%d ", sao_param_temp_merge_offset[0][j]);

		}
		fprintf(file7, "\n");
		for (j = 0; j < 32; j++)

		{
			fprintf(file7, "%d ", sao_param_temp_merge_offset[1][j]);

		}
		fprintf(file7, "\n");
		for (j = 0; j < 32; j++)

		{
			fprintf(file7, "%d ", sao_param_temp_merge_offset[2][j]);

		}
		fprintf(file7, "\n");
		fclose(file7);
#endif
    }
    else
    // mcost_new < mcost_merge        // Use SAO Param calced by NEW MODE
    {
        mincost = mcost_new;
        copySAOParam_for_blk_onecomponent_1(&cur_saoParam[0],
        sao_param_temp_new_Y_modeIdc, sao_param_temp_new_Y_typeIdc, sao_param_temp_new_Y_startBand, sao_param_temp_new_Y_startBand2, sao_param_temp_new_Y_deltaband, sao_param_temp_new_Y_offset);
        copySAOParam_for_blk_onecomponent_1(&cur_saoParam[1],
        sao_param_temp_new_U_modeIdc, sao_param_temp_new_U_typeIdc, sao_param_temp_new_U_startBand, sao_param_temp_new_U_startBand2, sao_param_temp_new_U_deltaband, sao_param_temp_new_U_offset);
        copySAOParam_for_blk_onecomponent_1(&cur_saoParam[2], 
        sao_param_temp_new_V_modeIdc, sao_param_temp_new_V_typeIdc, sao_param_temp_new_V_startBand, sao_param_temp_new_V_startBand2, sao_param_temp_new_V_deltaband, sao_param_temp_new_V_offset);
#if 0
		FILE *file6;
		file6 = fopen("cur_saoParam.txt", "w+");
		fprintf(file6, "%d %d %d %d %d\n", cur_saoParam[0].deltaband, cur_saoParam[0].modeIdc, cur_saoParam[0].startBand, cur_saoParam[0].startBand2, cur_saoParam[0].typeIdc);
		fprintf(file6, "%d %d %d %d %d\n", cur_saoParam[1].deltaband, cur_saoParam[1].modeIdc, cur_saoParam[1].startBand, cur_saoParam[1].startBand2, cur_saoParam[1].typeIdc);
		fprintf(file6, "%d %d %d %d %d\n", cur_saoParam[2].deltaband, cur_saoParam[2].modeIdc, cur_saoParam[2].startBand, cur_saoParam[2].startBand2, cur_saoParam[2].typeIdc);
		for (int i = 0; i < 32; i++)
		{
			fprintf(file6, "%d ", cur_saoParam[0].offset[i]);
		}
		fprintf(file6, "\n");
		for (int i = 0; i < 32; i++)
		{
			fprintf(file6, "%d ", cur_saoParam[1].offset[i]);
		}
		fprintf(file6, "\n");
		for (int i = 0; i < 32; i++)
		{
			fprintf(file6, "%d ", cur_saoParam[2].offset[i]);
		}
		fprintf(file6, "\n");
		fclose(file6);
#endif
        //recSaoParamLocal
        rec_sao_param_local_y_modeIdc[lcu_idx]    = sao_param_temp_new_Y_modeIdc;
        rec_sao_param_local_y_typeIdc[lcu_idx]    = sao_param_temp_new_Y_typeIdc;
        rec_sao_param_local_y_startBand[lcu_idx]  = sao_param_temp_new_Y_startBand;
        rec_sao_param_local_y_startBand2[lcu_idx] = sao_param_temp_new_Y_startBand2;
        rec_sao_param_local_y_deltaband[lcu_idx]  = sao_param_temp_new_Y_deltaband;
        rec_sao_param_local_u_modeIdc[lcu_idx]    = sao_param_temp_new_U_modeIdc;
        rec_sao_param_local_u_typeIdc[lcu_idx]    = sao_param_temp_new_U_typeIdc;
        rec_sao_param_local_u_startBand[lcu_idx]  = sao_param_temp_new_U_startBand;
        rec_sao_param_local_u_startBand2[lcu_idx] = sao_param_temp_new_U_startBand2;
        rec_sao_param_local_u_deltaband[lcu_idx]  = sao_param_temp_new_U_deltaband;
        rec_sao_param_local_v_modeIdc[lcu_idx]    = sao_param_temp_new_V_modeIdc;
        rec_sao_param_local_v_typeIdc[lcu_idx]    = sao_param_temp_new_V_typeIdc;
        rec_sao_param_local_v_startBand[lcu_idx]  = sao_param_temp_new_V_startBand;
        rec_sao_param_local_v_startBand2[lcu_idx] = sao_param_temp_new_V_startBand2;
        rec_sao_param_local_v_deltaband[lcu_idx]  = sao_param_temp_new_V_deltaband;
        for (j = 0; j < 32; j++) 
        {
#pragma HLS unroll
            rec_sao_param_local_y_offset[lcu_idx][j]  = sao_param_temp_new_Y_offset[j];
            rec_sao_param_local_u_offset[lcu_idx][j]  = sao_param_temp_new_U_offset[j];
            rec_sao_param_local_v_offset[lcu_idx][j]  = sao_param_temp_new_V_offset[j];
        }
#if sao_print
		FILE *file8;
		file8 = fopen("new_rec_sao_param_local.txt", "w+");
		fprintf(file8, "%d\n", sao_param_temp_new_Y_modeIdc);
		fprintf(file8, "%d\n", sao_param_temp_new_Y_typeIdc);
		fprintf(file8, "%d\n", sao_param_temp_new_Y_startBand);
		fprintf(file8, "%d\n", sao_param_temp_new_Y_startBand2);
		fprintf(file8, "%d\n", sao_param_temp_new_Y_deltaband);
		fprintf(file8, "%d\n", sao_param_temp_new_U_modeIdc);
		fprintf(file8, "%d\n", sao_param_temp_new_U_typeIdc);
		fprintf(file8, "%d\n", sao_param_temp_new_U_startBand);
		fprintf(file8, "%d\n", sao_param_temp_new_U_startBand2);
		fprintf(file8, "%d\n", sao_param_temp_new_U_deltaband);
		fprintf(file8, "%d\n", sao_param_temp_new_V_modeIdc);
		fprintf(file8, "%d\n", sao_param_temp_new_V_typeIdc);
		fprintf(file8, "%d\n", sao_param_temp_new_V_startBand);
		fprintf(file8, "%d\n", sao_param_temp_new_V_startBand2);
		fprintf(file8, "%d\n", sao_param_temp_new_V_deltaband);
		fprintf(file8, "\n");
		for (j = 0; j < 32; j++)
		{

			fprintf(file8, "%d ", sao_param_temp_new_Y_offset[j]);

		}
		fprintf(file8, "\n");
		for (j = 0; j < 32; j++)

		{
			fprintf(file8, "%d ", sao_param_temp_new_U_offset[j]);

		}
		fprintf(file8, "\n");
		for (j = 0; j < 32; j++)

		{
			fprintf(file8, "%d ", sao_param_temp_new_V_offset[j]);

		}
		fprintf(file8, "\n");
		fclose(file8);
#endif
    }
#if sao_print
	FILE *file5;
	file5 = fopen("rec_sao_param_local.txt", "w+");
	fprintf(file5, "%d\n", rec_sao_param_local_y_modeIdc[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_y_typeIdc[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_y_startBand[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_y_startBand2[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_y_deltaband[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_u_modeIdc[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_u_typeIdc[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_u_startBand[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_u_startBand2[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_u_deltaband[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_v_modeIdc[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_v_typeIdc[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_v_startBand[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_v_startBand2[lcu_idx]);
	fprintf(file5, "%d\n", rec_sao_param_local_v_deltaband[lcu_idx]);
	fprintf(file5, "\n");
	for (j = 0; j < 32; j++)

	{

		fprintf(file5, "%d ", rec_sao_param_local_y_offset[lcu_idx][j]);
		
	}
	fprintf(file5, "\n");
	for (j = 0; j < 32; j++)

	{
		fprintf(file5, "%d ", rec_sao_param_local_u_offset[lcu_idx][j]);
		
	}
	fprintf(file5, "\n");
	for (j = 0; j < 32; j++)

	{
		fprintf(file5, "%d ", rec_sao_param_local_v_offset[lcu_idx][j]);
		
	}
	fprintf(file5, "\n");
	fclose(file5);
#endif
}


static BOOL is_same_patch(s8 *map_patch_idx,int mb_nr1,int mb_nr2)
{
    //mb_nr1 >= 0?((void )0) : __assert_HLS("mb_nr1 >= 0","../separate/com_sao.c",(unsigned int )374,__FUNCTION__);
    //mb_nr2 >= 0?((void )0) : __assert_HLS("mb_nr2 >= 0","../separate/com_sao.c",(unsigned int )375,__FUNCTION__);

    // As we temperarily do not have concept of PATCH, always return 1, indicating lcus are in the same patch.   keep this function anyway.
    //return ((int )map_patch_idx[mb_nr1]) == ((int )map_patch_idx[mb_nr2]);
    return 1;
}

void lcu_boundary(int pix_x, int pix_y, int pic_width, int pic_height, int lcu_width, int lcu_height, 
                  int *lcu_avail_up, int * lcu_avail_down, int* lcu_avail_left, int* lcu_avail_right)
{
    *lcu_avail_up    = (pix_y == 0) ? 0 : 1;
    *lcu_avail_down  = ((pix_y + lcu_height) >= pic_height) ? 0 : 1;
    *lcu_avail_left  = (pix_x == 0) ? 0 : 1;
    *lcu_avail_right = ((pix_x + lcu_width) >= pic_width) ? 0 : 1;
}

static void checkBoundaryProc(int pic_width1, int pic_height1, int pic_width_in_scu, s8 *map_patch_idx, u8 cross_patch_loop_filter, int pix_y, int pix_x, int lcu_pix_height, int lcu_pix_width, int comp, 
                              int *lcu_process_left,   int *lcu_process_right,   int *lcu_process_up,       int *lcu_process_down, 
                              int *lcu_process_upleft, int *lcu_process_upright, int *lcu_process_leftdown, int *lcu_process_rightdwon, int filter_on)
{
    int pic_width  = comp ? (pic_width1 >> 1)  : pic_width1;
    int pic_height = comp ? (pic_height1 >> 1) : pic_height1;
    int pic_mb_width = pic_width_in_scu;
    int mb_size_in_bit = comp ? (2 - 1) : 2;
    int mb_nr_cur;
    int mb_nr_up;
    int mb_nr_down;
    int mb_nr_left;
    int mb_nr_right;
    int mb_nr_upleft;
    int mb_nr_upright;
    int mb_nr_leftdown;
    int mb_nr_rightdown;
    //s8 *map_patch_idx = map_patch_idx1;
    int cross_patch_flag = (int)cross_patch_loop_filter;
    
    mb_nr_cur       = (pix_y                    >> mb_size_in_bit) * pic_mb_width + (pix_x                   >> mb_size_in_bit);
    mb_nr_up        = ((pix_y - 1)              >> mb_size_in_bit) * pic_mb_width + (pix_x                   >> mb_size_in_bit);
    mb_nr_down      = ((pix_y + lcu_pix_height) >> mb_size_in_bit) * pic_mb_width + (pix_x                   >> mb_size_in_bit);
    mb_nr_left      = (pix_y                    >> mb_size_in_bit) * pic_mb_width + ((pix_x - 1)             >> mb_size_in_bit);
    mb_nr_right     = (pix_y                    >> mb_size_in_bit) * pic_mb_width + ((pix_x + lcu_pix_width) >> mb_size_in_bit);
    mb_nr_upleft    = ((pix_y - 1)              >> mb_size_in_bit) * pic_mb_width + ((pix_x - 1)             >> mb_size_in_bit);
    mb_nr_upright   = ((pix_y - 1)              >> mb_size_in_bit) * pic_mb_width + ((pix_x + lcu_pix_width) >> mb_size_in_bit);
    mb_nr_leftdown  = ((pix_y + lcu_pix_height) >> mb_size_in_bit) * pic_mb_width + ((pix_x - 1)             >> mb_size_in_bit);
    mb_nr_rightdown = ((pix_y + lcu_pix_height) >> mb_size_in_bit) * pic_mb_width + ((pix_x + lcu_pix_width) >> mb_size_in_bit);
    
    *lcu_process_up        = ((pix_y == 0)                                                                        ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_up)        ? 1 : cross_patch_flag));
    *lcu_process_down      = ((pix_y >= (pic_height - lcu_pix_height))                                            ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_down)      ? 1 : cross_patch_flag));
    *lcu_process_left      = ((pix_x == 0)                                                                        ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_left)      ? 1 : cross_patch_flag));
    *lcu_process_right     = ((pix_x >= (pic_width - lcu_pix_width))                                              ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_right)     ? 1 : cross_patch_flag));
    *lcu_process_upleft    = (((pix_x == 0) || (pix_y == 0))                                                      ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_upleft)    ? 1 : cross_patch_flag));
    *lcu_process_upright   = (((pix_x >= (pic_width - lcu_pix_width)) || (pix_y == 0))                            ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_upright)   ? 1 : cross_patch_flag));
    *lcu_process_leftdown  = ((pix_x == 0) || (pix_y >= (pic_height - lcu_pix_height)))                           ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_leftdown)  ? 1 : cross_patch_flag);
    *lcu_process_rightdwon = ((pix_x >= (pic_width - lcu_pix_width)) || (pix_y >= (pic_height - lcu_pix_height))) ? 0 : (is_same_patch(map_patch_idx,mb_nr_cur,mb_nr_rightdown) ? 1 : cross_patch_flag);
    
    if (!filter_on) 
    {
        *lcu_process_down = 0;
        *lcu_process_right = 0;
        *lcu_process_leftdown = 0;
        *lcu_process_rightdwon = 0;
    }
}

//static void SaoCalcRecPixY(pel *pY,int dstStride,SAOBlkParam rec_saoParam[60 + 2],int pix_y,int pix_x,
//            int dst_shift_y,int dst_shift_x,int sample_bit_depth,int lcu_idx,int edgeType,u8 temp_sao_lcu_array_y[70][70],
//            int y,int x)
//{
//#pragma HLS inline
//    int rec_idx;
//    rec_idx = (pix_y + y - dst_shift_y) * dstStride + pix_x + x - dst_shift_x;
//    pY[rec_idx] = ((pel )((0 > (((1 << sample_bit_depth) - 1 < ((int )temp_sao_lcu_array_y[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType]?(1 << sample_bit_depth) - 1 : ((int )temp_sao_lcu_array_y[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType]))?0 : (((1 << sample_bit_depth) - 1 < ((int )temp_sao_lcu_array_y[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType]?(1 << sample_bit_depth) - 1 : ((int )temp_sao_lcu_array_y[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType])))));
//}

static pel SaoCalcRecPixY_2(int dstStride, /*SAOBlkParam rec_saoParam[60 + 2],*/ signed int rec_sao_param_local_y_offset[62][32], int sample_bit_depth, int lcu_idx, int edgeType, pel temp_sao_lcu_array_y[70][70],
    int y, int x)
{
#pragma HLS inline
    signed int temp_1, temp_2;
    pel pixValue;

    temp_1 = (signed int)(1 << sample_bit_depth) - 1;
    temp_2 = (signed int)temp_sao_lcu_array_y[y][x] + (signed int)rec_sao_param_local_y_offset[lcu_idx][edgeType];

    if (temp_2 < 0)
    {
        pixValue = 0;
    }
    else if (temp_1 < temp_2)
    {
        pixValue = (pel)temp_1;
    }
    else
    {
        pixValue = (pel)temp_2;
    }

    //pixValue = ((pel)((0 > (((1 << sample_bit_depth) - 1 < ((int)temp_sao_lcu_array_y[y][x]) +
    //    rec_saoParam[lcu_idx].offset[edgeType] ? (1 << sample_bit_depth) - 1 : ((int)temp_sao_lcu_array_y[y][x]) +
    //    rec_saoParam[lcu_idx].offset[edgeType])) ? 0 : (((1 << sample_bit_depth) - 1 < ((int)temp_sao_lcu_array_y[y][x]) +
    //        rec_saoParam[lcu_idx].offset[edgeType] ? (1 << sample_bit_depth) - 1 : ((int)temp_sao_lcu_array_y[y][x]) +
    //        rec_saoParam[lcu_idx].offset[edgeType])))));
    return pixValue;
}

static int SaoTypeEO0(pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli[70][70],int y,int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int leftSign;
    int rightSign;
    int edgeType;
    a = temp_sao_lcu_array_y[y][x - 1];
    c = temp_sao_lcu_array_y[y][x];
    //b = temp_sao_lcu_array_y[y][x+1];
    b = temp_sao_lcu_array_y_dupli[y][x + 1];
    leftSign = (((int )c) > ((int )a)?2 : ((((int )c) == ((int )a)?1 : 0)));
    rightSign = (((int )c) > ((int )b)?2 : ((((int )c) == ((int )b)?1 : 0)));
    edgeType = leftSign + rightSign;
    return edgeType;
}

static int SaoTypeEO90(pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli[70][70],int y,int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upSign;
    int DownSign;
    int edgeType;
    a = temp_sao_lcu_array_y[y - 1][x];
    c = temp_sao_lcu_array_y[y]    [x];
    //b = temp_sao_lcu_array_y[y+1][x];
    b = temp_sao_lcu_array_y_dupli[y + 1][x];
    upSign   = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    DownSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
    edgeType = upSign + DownSign;
    return edgeType;
}

static int SaoTypeEO135(pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli[70][70],int y,int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upLeftSign;
    int downRightSign;
    int edgeType;
    a = temp_sao_lcu_array_y[y - 1][x - 1];
    c = temp_sao_lcu_array_y[y]    [x];
    //b = temp_sao_lcu_array_y[y+1][x+1];
    b = temp_sao_lcu_array_y_dupli[y + 1][x + 1];
    upLeftSign    = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    downRightSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
    edgeType = upLeftSign + downRightSign;
    return edgeType;
}

static int SaoTypeEO45(pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli[70][70],int y,int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upRightSign;
    int downLeftSign;
    int edgeType;
    a = temp_sao_lcu_array_y[y - 1][x + 1];
    c = temp_sao_lcu_array_y[y][x];
    //b = temp_sao_lcu_array_y[y+1][x-1];
    b = temp_sao_lcu_array_y_dupli[y + 1][x - 1];
    upRightSign  = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    downLeftSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
    edgeType = upRightSign + downLeftSign;
    return edgeType;
}

static int SaoTypeBO(int sample_bit_depth, pel temp_sao_lcu_array_y[70][70], int y, int x)
{
#pragma HLS inline
    int bandType;
    int bitRightShift;
    bitRightShift = sample_bit_depth - 5;
    bandType = ((int )temp_sao_lcu_array_y[y][x]) >> bitRightShift;
    return bandType;
}

static int SaoType_test(int sample_bit_depth,pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli[70][70], int y, int x, int type_up)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upSign;
    int DownSign;
    int edgeType;
switch(type_up)
{
case (int )SAO_TYPE_EO_0:
{
    a = temp_sao_lcu_array_y[y][x-1];
    c = temp_sao_lcu_array_y[y]    [x];
    b = temp_sao_lcu_array_y_dupli[y][x+1];
}
break;
case (int )SAO_TYPE_EO_90:
{
    a = temp_sao_lcu_array_y[y - 1][x];
    c = temp_sao_lcu_array_y[y]    [x];
    b = temp_sao_lcu_array_y_dupli[y + 1][x];
}
break;
case (int )SAO_TYPE_EO_135:
{
    a = temp_sao_lcu_array_y[y - 1][x - 1];
    c = temp_sao_lcu_array_y[y]    [x];
    b = temp_sao_lcu_array_y_dupli[y + 1][x + 1];
}
break;
case (int )SAO_TYPE_EO_45:
{
    a = temp_sao_lcu_array_y[y - 1][x + 1];
    c = temp_sao_lcu_array_y[y][x];
    b = temp_sao_lcu_array_y_dupli[y + 1][x - 1];
}
break;
case (int )SAO_TYPE_BO:
{
    upSign = sample_bit_depth - 5;
    edgeType = ((int )temp_sao_lcu_array_y[y][x]) >> upSign;
    a = 0;
    b = 0;
    c = 0;
}
break;
}
    upSign   = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    DownSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
	if (type_up != (int )SAO_TYPE_BO){
		edgeType = upSign + DownSign;
	}
    return edgeType;
}
void SaoCalcShiftBlk_Y(int sample_bit_depth, pel sao_rslt_array_y[70][70], int dstStride, //SAOBlkParam rec_saoParam[60 + 2], 
                       int rec_sao_param_local_y_modeIdc[62], int rec_sao_param_local_y_typeIdc[62], int rec_sao_param_local_y_offset[62][32],
                       int lcu_pix_height, int lcu_pix_width,
                       int lcu_available_left, int lcu_available_right, int lcu_available_up,int lcu_available_down,             // int lcu_available_upleft, int lcu_available_upright,int lcu_available_leftdown,int lcu_available_rightdwon,
                       pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli[70][70], 
                       int sq_lcu_idx, int up2row_lcu_idx, int left2col_lcu_idx, int cur_lcu_idx,
                       int* sq_sao_mode_y_ptr,     int* up2row_sao_mode_y_ptr, int* left2col_sao_mode_y_ptr, int* cur_lcu_sao_mode_y_ptr,
                       int* up2row_y_start_x_ptr,  int* up2row_y_end_x_ptr,    int* left2col_y_start_y_ptr,  int* left2col_y_end_y_ptr, 
                       int* cur_lcu_y_start_x_ptr, int* cur_lcu_y_end_x_ptr,   int* cur_lcu_y_start_y_ptr,   int* cur_lcu_y_end_y_ptr)
{
#pragma HLS ARRAY_PARTITION variable=sao_rslt_array_y               cyclic factor=70 dim=2 partition
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_y           cyclic factor=70 dim=2 partition
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_y_dupli     cyclic factor=70 dim=2 partition
#pragma HLS ARRAY_PARTITION variable=rec_sao_param_local_y_offset   complete dim=2 

    int type;
    int type_left_up;
    int type_up;
    int type_left;
    
    int x;
    int y;
    int edgeType=0;
    
    
    int dst_shift_x;
    int dst_shift_y;
    
    int y_test;
    if (lcu_pix_height <= 0 || lcu_pix_width <= 0) // If LCU does NOT exit.
    {
        return ;
    }
    dst_shift_x = 6;
    dst_shift_y = 6;

    //*sq_sao_mode_y_ptr = rec_saoParam[sq_lcu_idx].modeIdc;
    //*up2row_sao_mode_y_ptr = rec_saoParam[up2row_lcu_idx].modeIdc;
    //*left2col_sao_mode_y_ptr = rec_saoParam[left2col_lcu_idx].modeIdc;
    //*cur_lcu_sao_mode_y_ptr = rec_saoParam[cur_lcu_idx].modeIdc;
    *sq_sao_mode_y_ptr = rec_sao_param_local_y_modeIdc[sq_lcu_idx];
    *up2row_sao_mode_y_ptr = rec_sao_param_local_y_modeIdc[up2row_lcu_idx];
    *left2col_sao_mode_y_ptr = rec_sao_param_local_y_modeIdc[left2col_lcu_idx];
    *cur_lcu_sao_mode_y_ptr = rec_sao_param_local_y_modeIdc[cur_lcu_idx];
#if !SIMPLE_MD

//----------------------------------------------------------------------------------------
// Up-left 2x2 square:
    if (lcu_available_up && lcu_available_left) 
    {
        if (*sq_sao_mode_y_ptr != ((int )SAO_MODE_OFF)) 
        {
            type_left_up = rec_sao_param_local_y_typeIdc[sq_lcu_idx];
            edgeType = SaoType_test(sample_bit_depth,temp_sao_lcu_array_y,temp_sao_lcu_array_y_dupli,1,1,type_left_up);
            sao_rslt_array_y[1][1] = SaoCalcRecPixY_2(dstStride, rec_sao_param_local_y_offset, sample_bit_depth, sq_lcu_idx, edgeType, temp_sao_lcu_array_y, 1, 1);
        }
    }

#endif
//----------------------------------------------------------------------------------------
// Upside 2 ROWs
    if (lcu_available_up) 
    {
        if ( *up2row_sao_mode_y_ptr != ((int )SAO_MODE_OFF)) 
        {
            type_up = rec_sao_param_local_y_typeIdc[up2row_lcu_idx];
            switch(type_up)
            {
                case (int )SAO_TYPE_EO_0:
                {
                    *up2row_y_start_x_ptr = (lcu_available_left ? 2 : 7);
                    *up2row_y_end_x_ptr = (lcu_available_right ? lcu_pix_width + 1 : lcu_pix_width + 5);
                }
                break;
                case (int )SAO_TYPE_EO_90:
                {
                    *up2row_y_start_x_ptr = (lcu_available_left ? 2 : 6);
                    *up2row_y_end_x_ptr = (lcu_available_right ? lcu_pix_width + 1 : lcu_pix_width + 6);
                }
                break;
                case (int )SAO_TYPE_EO_135:
                {
                    //*up2row_y_start_x_ptr = (lcu_available_upleft ? 2 : 7);
                    *up2row_y_start_x_ptr = (lcu_available_left ? 2 : 7);
                    *up2row_y_end_x_ptr = (lcu_available_right ? lcu_pix_width + 1 : lcu_pix_width + 5);
                }
                break;
                case (int )SAO_TYPE_EO_45:
                {
                    *up2row_y_start_x_ptr = (lcu_available_left ? 2 : 7);
                    *up2row_y_end_x_ptr = (lcu_available_right ? lcu_pix_width + 1 : lcu_pix_width + 5);
                }
                break;
                case (int )SAO_TYPE_BO:
                {
                    *up2row_y_start_x_ptr = (lcu_available_left ? 2 : 6);
                    *up2row_y_end_x_ptr = (lcu_available_right ? lcu_pix_width + 1 : lcu_pix_width + 6);
                }
                break;
            }
#if !SIMPLE_MD
            LOOP_1:for (x = *up2row_y_start_x_ptr; x < *up2row_y_end_x_ptr; x++)
                    {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=68
                edgeType = SaoType_test(sample_bit_depth,temp_sao_lcu_array_y,temp_sao_lcu_array_y_dupli,1,x,type_up);
                        sao_rslt_array_y[1][x] = SaoCalcRecPixY_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_y_offset, sample_bit_depth, up2row_lcu_idx, edgeType, temp_sao_lcu_array_y, 1, x);
                    }
#endif
                }
            }

    
//----------------------------------------------------------------------------------------
// Leftsied 2 COLUMs
    if (lcu_available_left) 
    {
        if (*left2col_sao_mode_y_ptr != ((int )SAO_MODE_OFF)) 
        {
            type_left = rec_sao_param_local_y_typeIdc[left2col_lcu_idx];
            switch(type_left)
            {
                case (int )SAO_TYPE_EO_0:
                {
                    *left2col_y_start_y_ptr = (lcu_available_up ? 2 : 6);
                    *left2col_y_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 6);
                }
                break;
                case (int )SAO_TYPE_EO_90:
                {
                    *left2col_y_start_y_ptr = (lcu_available_up ? 2 : 7);
                    *left2col_y_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 5);
                }
                break;
                case (int )SAO_TYPE_EO_135:
                {
                    *left2col_y_start_y_ptr = (lcu_available_up ? 2 : 7);
                    *left2col_y_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 5);
                }
                break;
                case (int )SAO_TYPE_EO_45:
                {
                    *left2col_y_start_y_ptr = (lcu_available_up ? 2 : 7);
                    *left2col_y_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 5);
                }
                break;
                case (int )SAO_TYPE_BO:
                {
                    *left2col_y_start_y_ptr = (lcu_available_up ? 2 : 6);
                    *left2col_y_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 6);
                }
                break;
                default:
                {
                    printf("Not a supported SAO types\n");
                    exit(- 1);
                }
            }
#if !SIMPLE_MD
            LOOP_2:for (y = *left2col_y_start_y_ptr; y < *left2col_y_end_y_ptr; y++)
            {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=68
                edgeType = SaoType_test(sample_bit_depth,temp_sao_lcu_array_y,temp_sao_lcu_array_y_dupli,y,1,type_left);
                sao_rslt_array_y[y][1] = SaoCalcRecPixY_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_y_offset, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_y, y, 1);
            }
#endif
        }
    }
    
//----------------------------------------------------------------------------------------
// Rest pixels
    if (*cur_lcu_sao_mode_y_ptr != ((int )SAO_MODE_OFF)) 
    {
        type = rec_sao_param_local_y_typeIdc[cur_lcu_idx];
        switch(type)
        {
            case (int )SAO_TYPE_EO_0:
            {
                *cur_lcu_y_start_x_ptr = (lcu_available_left ? 2 : 7);
                *cur_lcu_y_end_x_ptr = (lcu_available_right ? 65 : lcu_pix_width + 5);
                *cur_lcu_y_start_y_ptr = (lcu_available_up ? 2 : 6);
                *cur_lcu_y_end_y_ptr = (lcu_available_down ? 65 : lcu_pix_height + 6);
				y_test = 70;
            }
            break;
            case (int )SAO_TYPE_EO_90:
            {
                *cur_lcu_y_start_x_ptr = (lcu_available_left ? 2 : 6);
                *cur_lcu_y_end_x_ptr = (lcu_available_right ? 65 : lcu_pix_width + 6);
                *cur_lcu_y_start_y_ptr = (lcu_available_up ? 2 : 7);
                *cur_lcu_y_end_y_ptr = (lcu_available_down ? 65 : lcu_pix_height + 5);
				y_test = 69;
            }
            break;
            case (int )SAO_TYPE_EO_135:
            {

                *cur_lcu_y_start_x_ptr = (lcu_available_left ? 2 : 7);
                *cur_lcu_y_end_x_ptr = (lcu_available_right ? 65 : lcu_pix_width + 5);
                *cur_lcu_y_start_y_ptr = (lcu_available_up ? 2 : 7);
                *cur_lcu_y_end_y_ptr = (lcu_available_down ? 65 : lcu_pix_height + 5);
				y_test = 69;

            }
            break;
            case (int )SAO_TYPE_EO_45:
            {
                *cur_lcu_y_start_x_ptr = (lcu_available_left ? 2 : 7);
                *cur_lcu_y_end_x_ptr = (lcu_available_right ? 65 : lcu_pix_width + 5);
                *cur_lcu_y_start_y_ptr = (lcu_available_up ? 2 : 7);
                *cur_lcu_y_end_y_ptr = (lcu_available_down ? 65 : lcu_pix_height + 5);
				y_test = 69;
 
            }
            break;
            case (int )SAO_TYPE_BO:
            {
                *cur_lcu_y_start_x_ptr = (lcu_available_left ? 2 : 6);
                *cur_lcu_y_end_x_ptr = (lcu_available_right ? 65 : lcu_pix_width + 6);
                *cur_lcu_y_start_y_ptr = (lcu_available_up ? 2 : 6);
                *cur_lcu_y_end_y_ptr = (lcu_available_down ? 65 : lcu_pix_height + 6);
				y_test = 70;

            }
            break;
            default:
            {
                printf("Not a supported SAO types\n");
                exit(- 1);
            }
        }
#if !SIMPLE_MD
            LOOP_8:for (y = 2; y < y_test; y++)
            {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=68
                for (x = 2; x < 69; x++)
                {
#pragma HLS UNROLL factor=67
                    edgeType = SaoType_test(sample_bit_depth,temp_sao_lcu_array_y,temp_sao_lcu_array_y_dupli,y,x,type);
                    sao_rslt_array_y[y][x] = SaoCalcRecPixY_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_y_offset, sample_bit_depth, cur_lcu_idx, edgeType, temp_sao_lcu_array_y, y, x);
    }
				if ((int )SAO_TYPE_BO == type)
				{
					edgeType = SaoTypeBO(sample_bit_depth,temp_sao_lcu_array_y,y,x);
                    sao_rslt_array_y[y][x] = SaoCalcRecPixY_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_y_offset, sample_bit_depth, cur_lcu_idx, edgeType, temp_sao_lcu_array_y, y, x);
				} else if ((int )SAO_TYPE_EO_90 == type) {
					edgeType = SaoTypeEO90(temp_sao_lcu_array_y,temp_sao_lcu_array_y_dupli,y,x);
                    sao_rslt_array_y[y][x] = SaoCalcRecPixY_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_y_offset, sample_bit_depth, cur_lcu_idx, edgeType, temp_sao_lcu_array_y, y, x);
}
            }
#endif
    }
}
//static void SaoCalcRecPixU(pel *pUV,int dstStride,SAOBlkParam rec_saoParam[60 + 2],int pix_y,int pix_x,
//                           int dst_shift_y,int dst_shift_x,int sample_bit_depth,int lcu_idx,int edgeType,u8 temp_sao_lcu_array_u[38][38],
//                           int y,int x)
//{
//#pragma HLS inline
//  int rec_idx;
//  rec_idx = (pix_y + y - dst_shift_y) * dstStride + pix_x + x - dst_shift_x;
//  pUV[rec_idx] = ((pel )((0 > (((1 << sample_bit_depth) - 1 < ((int )temp_sao_lcu_array_u[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType]?(1 << sample_bit_depth) - 1 : ((int )temp_sao_lcu_array_u[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType]))?0 : (((1 << sample_bit_depth) - 1 < ((int )temp_sao_lcu_array_u[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType]?(1 << sample_bit_depth) - 1 : ((int )temp_sao_lcu_array_u[y][x]) + rec_saoParam[lcu_idx] . offset[edgeType])))));
//}

static pel SaoCalcRecPixUV_2(int dstStride, /*SAOBlkParam rec_saoParam[60 + 2],*/ signed int rec_sao_param_local_offset[62][32], int sample_bit_depth, int lcu_idx, int edgeType, pel temp_sao_lcu_array[38][38], int y, int x)
{
#pragma HLS inline

    signed int temp_1, temp_2;

    pel pixValue;

    temp_1 = (signed int)(1 << sample_bit_depth) - 1;
    //temp_2 = (int)temp_sao_lcu_array[y][x] + rec_saoParam[lcu_idx].offset[edgeType];
    temp_2 = (signed int)temp_sao_lcu_array[y][x] + (signed int)rec_sao_param_local_offset[lcu_idx][edgeType];

    if (temp_2 < 0)
    {
        pixValue = 0;
    }
    else if (temp_1 < temp_2)
    {
        pixValue = (pel)temp_1;
    }
    else
    {
        pixValue = (pel)temp_2;
    }
    //pixValue = (pel)((0 > ((temp_1 < temp_2 ? temp_1 : temp_2) ? 0 : (temp_1 < temp_2 ? temp_1 : temp_2)) ));

    return pixValue;
}

static int SaoTypeUVEO0(pel temp_sao_lcu_array_uv[38][38], int y, int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int leftSign;
    int rightSign;
    int edgeType;
    a = temp_sao_lcu_array_uv[y][x - 1];
    c = temp_sao_lcu_array_uv[y][x];
    b = temp_sao_lcu_array_uv[y][x + 1];
    leftSign  = (((int)c) > ((int)a) ? 2 : ((((int)c) == ((int)a) ? 1 : 0)));
    rightSign = (((int)c) > ((int)b) ? 2 : ((((int)c) == ((int)b) ? 1 : 0)));
    edgeType = leftSign + rightSign;
    return edgeType;
}

static int SaoTypeUVEO90(pel temp_sao_lcu_array_uv[38][38], int y, int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upSign;
    int DownSign;
    int edgeType;
    a = temp_sao_lcu_array_uv[y - 1][x];
    c = temp_sao_lcu_array_uv[y]    [x];
    b = temp_sao_lcu_array_uv[y + 1][x];
    upSign   = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    DownSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
    edgeType = upSign + DownSign;
    return edgeType;
}

static int SaoTypeUVEO135(pel temp_sao_lcu_array_uv[38][38],int y,int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upLeftSign;
    int downRightSign;
    int edgeType;
    a = temp_sao_lcu_array_uv[y - 1][x - 1];
    c = temp_sao_lcu_array_uv[y]    [x];
    b = temp_sao_lcu_array_uv[y + 1][x + 1];
    upLeftSign    = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    downRightSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
    edgeType = upLeftSign + downRightSign;
    return edgeType;
}

static int SaoTypeUVEO45(pel temp_sao_lcu_array_uv[38][38],int y,int x)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upRightSign;
    int downLeftSign;
    int edgeType;
    a = temp_sao_lcu_array_uv[y - 1][x + 1];
    c = temp_sao_lcu_array_uv[y]    [x];
    b = temp_sao_lcu_array_uv[y + 1][x - 1];
    upRightSign  = (((int )c) > ((int )a) ? 2 : (((int )c) == ((int )a) ? 1 : 0));
    downLeftSign = (((int )c) > ((int )b) ? 2 : (((int )c) == ((int )b) ? 1 : 0));
    edgeType = upRightSign + downLeftSign;
    return edgeType;
}

static int SaoTypeUVBO(int sample_bit_depth, pel temp_sao_lcu_array_uv[38][38], int y, int x)
{
#pragma HLS inline
    int bandType;
    int bitRightShift;
    bitRightShift = sample_bit_depth - 5;
    bandType = ((int )temp_sao_lcu_array_uv[y][x]) >> bitRightShift;
    return bandType;
}

static int SaoTypeUV_Test(int sample_bit_depth, pel temp_sao_lcu_array_uv[38][38], int y, int x,int type_up)
{
#pragma HLS inline
    pel a;
    pel c;
    pel b;
    int upSign;
    int DownSign;
    int edgeType;
switch(type_up)
{
case (int )SAO_TYPE_EO_0:
{
    a = temp_sao_lcu_array_uv[y][x - 1];
    c = temp_sao_lcu_array_uv[y][x];
    b = temp_sao_lcu_array_uv[y][x + 1];
}
break;
case (int )SAO_TYPE_EO_90:
{
    a = temp_sao_lcu_array_uv[y - 1][x];
    c = temp_sao_lcu_array_uv[y]    [x];
    b = temp_sao_lcu_array_uv[y + 1][x];
}
break;
case (int )SAO_TYPE_EO_135:
{
    a = temp_sao_lcu_array_uv[y - 1][x - 1];
    c = temp_sao_lcu_array_uv[y]    [x];
    b = temp_sao_lcu_array_uv[y + 1][x + 1];
}
break;
case (int )SAO_TYPE_EO_45:
{
    a = temp_sao_lcu_array_uv[y - 1][x + 1];
    c = temp_sao_lcu_array_uv[y]    [x];
    b = temp_sao_lcu_array_uv[y + 1][x - 1];
}
break;
case (int )SAO_TYPE_BO:
{
    upSign = sample_bit_depth - 5;
    edgeType = ((int )temp_sao_lcu_array_uv[y][x]) >> upSign;
    a = 0;
    b = 0;
    c = 0;
}
break;
}
	upSign   = (((int )c) > ((int )a) ? 2 : ((((int )c) == ((int )a) ? 1 : 0)));
    DownSign = (((int )c) > ((int )b) ? 2 : ((((int )c) == ((int )b) ? 1 : 0)));
	if (type_up != (int )SAO_TYPE_BO){
		edgeType = upSign + DownSign;
	}
    return edgeType;
}
static void SaoCalcShiftBlk_UV(int sample_bit_depth, pel sao_rslt_array_uv[38][38], int dstStride, //SAOBlkParam rec_saoParam[60 + 2],
                               int rec_sao_param_local_modeIdc[62], int rec_sao_param_local_typeIdc[62], int rec_sao_param_local_offset[62][32],
                               int lcu_pix_height,int lcu_pix_width,
                               int lcu_available_left,int lcu_available_right,int lcu_available_up,int lcu_available_down,                   // int lcu_available_upleft,int lcu_available_upright,int lcu_available_leftdown,int lcu_available_rightdwon,
                               pel temp_sao_lcu_array_uv[38][38], 
                               int sq_lcu_idx, int up2row_lcu_idx, int left2col_lcu_idx, int cur_lcu_idx, 
                               int* sq_sao_mode_ptr,     int* up2row_sao_mode_ptr, int* left2col_sao_mode_ptr, int* cur_lcu_sao_mode_ptr,
                               int* up2row_start_x_ptr,  int* up2row_end_x_ptr,    int* left2col_start_y_ptr,  int* left2col_end_y_ptr, 
                               int* cur_lcu_start_x_ptr, int* cur_lcu_end_x_ptr,   int* cur_lcu_start_y_ptr,   int* cur_lcu_end_y_ptr)
{
#pragma HLS ARRAY_PARTITION variable=sao_rslt_array_uv                 cyclic factor=38 dim=2 partition
#pragma HLS ARRAY_PARTITION variable=temp_sao_lcu_array_uv             cyclic factor=38 dim=2 partition
#pragma HLS ARRAY_PARTITION variable=rec_sao_param_local_offset        complete dim=2 
    int type;
    //int type_left_up;
    //int type_up;
    //int type_left;
    //int dst_shift_x;
    //int dst_shift_y;
    int x;
    int y;
    int edgeType=0;

	int x_test;
	int y_test;
    if (lcu_pix_height <= 0 || lcu_pix_width <= 0) // If LCU does NOT exit.
    {
        return ;
    }

    //dst_shift_x = 6;
    //dst_shift_y = 6;

    //*sq_sao_mode_ptr = rec_saoParam[sq_lcu_idx].modeIdc;
    //*up2row_sao_mode_ptr = rec_saoParam[up2row_lcu_idx].modeIdc;
    //*left2col_sao_mode_ptr = rec_saoParam[left2col_lcu_idx].modeIdc;
    //*cur_lcu_sao_mode_ptr = rec_saoParam[cur_lcu_idx].modeIdc;
    //*sq_sao_mode_ptr       = rec_sao_param_local_modeIdc[sq_lcu_idx];
    //*up2row_sao_mode_ptr   = rec_sao_param_local_modeIdc[up2row_lcu_idx];
    //*left2col_sao_mode_ptr = rec_sao_param_local_modeIdc[left2col_lcu_idx];
    *cur_lcu_sao_mode_ptr  = rec_sao_param_local_modeIdc[cur_lcu_idx];

//// Up-left 2x2 small sq.
//    if (lcu_available_up && lcu_available_left) 
//    {
//        if (*sq_sao_mode_ptr != ((int )SAO_MODE_OFF)) 
//        {
//            type_left_up = rec_sao_param_local_typeIdc[sq_lcu_idx];
//            switch(type_left_up)
//            {
//                case (int )SAO_TYPE_EO_0:
//                {
//                    edgeType = SaoTypeUVEO0(temp_sao_lcu_array_uv,1,1);
//                }
//                break;
//                case (int )SAO_TYPE_EO_90:
//                {
//                    edgeType = SaoTypeUVEO90(temp_sao_lcu_array_uv,1,1);
//                }
//                break;
//                case (int )SAO_TYPE_EO_135:
//                {
//                    edgeType = SaoTypeUVEO135(temp_sao_lcu_array_uv,1,1);
//                }
//                break;
//                case (int )SAO_TYPE_EO_45:
//                {
//                    edgeType = SaoTypeUVEO45(temp_sao_lcu_array_uv,1,1);
//                }
//                break;
//                case (int )SAO_TYPE_BO:
//                {
//                    edgeType = SaoTypeUVBO(sample_bit_depth,temp_sao_lcu_array_uv,1,1);
//                }
//                break;
//            }
//            //pic_rec->u[(pix_y + 1 - 6) * DstStride + pix_x + 1 - 6] = (pel)COM_CLIP3(0, ((1 << sample_bit_depth) - 1), temp_sao_lcu_array_u[1][1] + rec_saoParam[lcu_idx][1].offset[edgeType]);    // Original form
//            //sao_rslt_array_uv[1][1] = SaoCalcRecPixUV_2(dstStride, rec_saoParam, sample_bit_depth, sq_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, 1);
//            sao_rslt_array_uv[1][1] = SaoCalcRecPixUV_2(dstStride, rec_sao_param_local_offset, sample_bit_depth, sq_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, 1);
//        }
//    }
//// Upside 2 ROWs
//    if (lcu_available_up) 
//    {
//        if (*up2row_sao_mode_ptr != ((int )SAO_MODE_OFF)) 
//        {
//            type_up = rec_sao_param_local_typeIdc[up2row_lcu_idx];
//            switch(type_up)
//            {
//                case (int )SAO_TYPE_EO_0:
//                {
//                    *up2row_start_x_ptr = lcu_available_left  ?  2 : 7;
//                    *up2row_end_x_ptr   = lcu_available_right ? 33 : (lcu_pix_width + 5);
//                    for (x = 2; x < 37; x++)
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO0(temp_sao_lcu_array_uv, 1, x);
//                        //SaoCalcRecPixU(pUV, dstStride, rec_saoParam, pix_y, pix_x, dst_shift_y, dst_shift_x, sample_bit_depth, up2row_lcu_idx,edgeType, temp_sao_lcu_array_uv, 1, x);
//                        sao_rslt_array_uv[1][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, up2row_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, x);
//                    }
//                }
//                break;
//                case (int )SAO_TYPE_EO_90:
//                {
//                    *up2row_start_x_ptr = lcu_available_left  ?  2 : 6;
//                    *up2row_end_x_ptr   = lcu_available_right ? 33 : (lcu_pix_width + 6);
//                    for (x = 2; x < 38; x++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO90(temp_sao_lcu_array_uv, 1, x);
//                        sao_rslt_array_uv[1][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, up2row_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, x);
//                    }
//                }
//                break;
//                case (int )SAO_TYPE_EO_135:
//                {
//                    //*up2row_start_x_ptr = (lcu_available_upleft ? 2 : 7);
//                    *up2row_start_x_ptr = lcu_available_left  ?  2 : 7;
//                    *up2row_end_x_ptr   = lcu_available_right ? 33 : (lcu_pix_width + 5);
//                    for (x = 2; x < 37; x++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO135(temp_sao_lcu_array_uv, 1, x);
//                        sao_rslt_array_uv[1][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, up2row_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, x);
//                    }
//                }
//                break;
//                case (int )SAO_TYPE_EO_45:
//                {
//                    *up2row_start_x_ptr = lcu_available_left  ?  2 : 7;
//                    *up2row_end_x_ptr   = lcu_available_right ? 33 : (lcu_pix_width + 5);
//                    for (x = 2; x < 37; x++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO45(temp_sao_lcu_array_uv, 1, x);
//                        sao_rslt_array_uv[1][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, up2row_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, x);
//                    }
//
//                }
//                break;
//                case (int )SAO_TYPE_BO:
//                {
//                    *up2row_start_x_ptr = lcu_available_left  ?  2 : 6;
//                    *up2row_end_x_ptr   = lcu_available_right ? 33 : (lcu_pix_width + 6);
//                    for (x = 2; x < 38; x++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVBO(sample_bit_depth, temp_sao_lcu_array_uv, 1, x);
//                        sao_rslt_array_uv[1][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, up2row_lcu_idx, edgeType, temp_sao_lcu_array_uv, 1, x);
//                    }
//                }
//                break;
//            }
//        }
//    }
//// Leftsied 2 COLUMs
//    if (lcu_available_left) 
//    {
//        if (*left2col_sao_mode_ptr != ((int )SAO_MODE_OFF)) 
//        {
//            type_left = rec_sao_param_local_typeIdc[left2col_lcu_idx];
//            switch(type_left)
//            {
//                case (int )SAO_TYPE_EO_0:
//                {
//                    *left2col_start_y_ptr = (lcu_available_up ? 2 : 6);
//                    *left2col_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 6);
//                    for (y = *left2col_start_y_ptr; y < *left2col_end_y_ptr; y++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO0(temp_sao_lcu_array_uv, y, 1);
//                        //SaoCalcRecPixU(pUV, dstStride, rec_saoParam, pix_y, pix_x, dst_shift_y, dst_shift_x, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, 1);
//                        sao_rslt_array_uv[y][1] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, 1);
//                    }
//                }
//                break;
//                case (int )SAO_TYPE_EO_90:
//                {
//                    *left2col_start_y_ptr = (lcu_available_up ? 2 : 7);
//                    *left2col_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 5);
//                    for (y = *left2col_start_y_ptr; y < *left2col_end_y_ptr; y++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO90(temp_sao_lcu_array_uv, y, 1);
//                        sao_rslt_array_uv[y][1] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, 1);
//                    }
//
//                }
//                break;
//                case (int )SAO_TYPE_EO_135:
//{
//                    *left2col_start_y_ptr = (lcu_available_up ? 2 : 7);
//                    *left2col_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 5);
//                    for (y = *left2col_start_y_ptr; y < *left2col_end_y_ptr; y++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO135(temp_sao_lcu_array_uv, y, 1);
//                        sao_rslt_array_uv[y][1] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, 1);
//                    }
//                }
//                break;
//                case (int )SAO_TYPE_EO_45:
//                {
//                    *left2col_start_y_ptr = (lcu_available_up ? 2 : 7);
//                    *left2col_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 5);
//                    for (y = *left2col_start_y_ptr; y < *left2col_end_y_ptr; y++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVEO45(temp_sao_lcu_array_uv, y, 1);
//                        sao_rslt_array_uv[y][1] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, 1);
//                    }
//                }
//                break;
//                case (int )SAO_TYPE_BO:
//                {
//                    *left2col_start_y_ptr = (lcu_available_up ? 2 : 6);
//                    *left2col_end_y_ptr = (lcu_available_down ? lcu_pix_height + 1 : lcu_pix_height + 6);
//                    for (y = *left2col_start_y_ptr; y < *left2col_end_y_ptr; y++) 
//                    {
//#pragma HLS PIPELINE 
//#pragma HLS LOOP_TRIPCOUNT max=36
//                        edgeType = SaoTypeUVBO(sample_bit_depth, temp_sao_lcu_array_uv, y, 1);
//                        sao_rslt_array_uv[y][1] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, left2col_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, 1);
//                    }
//
//                }
//                break;
//                default:
//                {
//                    printf("Not a supported SAO types\n");
//                    exit(- 1);
//                }
//            }
//        }
//    }

// Rest Pixels
    if (*cur_lcu_sao_mode_ptr != ((int )SAO_MODE_OFF)) 
    {
        type = rec_sao_param_local_typeIdc[cur_lcu_idx];
        switch(type)
        {
            case (int )SAO_TYPE_EO_0:
            {
                //*cur_lcu_start_x_ptr = lcu_available_left  ?  2 : 7;
                //*cur_lcu_end_x_ptr   = lcu_available_right ? 37 : (lcu_pix_width + 5);
                //*cur_lcu_start_y_ptr = lcu_available_up    ?  2 : 6;
                //*cur_lcu_end_y_ptr   = lcu_available_down  ? 38 : (lcu_pix_height + 6);
                *cur_lcu_start_y_ptr = lcu_available_up   ? 2 : 6;
                *cur_lcu_end_y_ptr   = lcu_pix_height + 6;
                *cur_lcu_start_x_ptr = lcu_available_left ? 2 : 7;
                *cur_lcu_end_x_ptr   = lcu_pix_width + 5;
				x_test = 37;
				y_test = 38;



            }
            break;
            case (int )SAO_TYPE_EO_90:
            {
                //*cur_lcu_start_x_ptr = lcu_available_left  ?  2 : 6;
                //*cur_lcu_end_x_ptr   = lcu_available_right ? 38 : (lcu_pix_width + 6);
                //*cur_lcu_start_y_ptr = lcu_available_up    ?  2 : 7;
                //*cur_lcu_end_y_ptr   = lcu_available_down  ? 37 : (lcu_pix_height + 5);
                
                *cur_lcu_start_y_ptr = lcu_available_up   ? 2 : 7;
                *cur_lcu_end_y_ptr   = lcu_pix_height + 5;
                *cur_lcu_start_x_ptr = lcu_available_left ? 2 : 6;
                *cur_lcu_end_x_ptr   = lcu_pix_width + 6;
				x_test = 38;
				y_test = 37;


            }
            break;
            case (int )SAO_TYPE_EO_135:
            {
                //*cur_lcu_start_x_ptr = lcu_available_left  ?  2 : 7;
                //*cur_lcu_end_x_ptr   = lcu_available_right ? 37 : (lcu_pix_width + 5);
                //*cur_lcu_start_y_ptr = lcu_available_up    ?  2 : 7;
                //*cur_lcu_end_y_ptr   = lcu_available_down  ? 37 : (lcu_pix_height + 5);

                *cur_lcu_start_y_ptr = lcu_available_up   ? 2 : 7;
                *cur_lcu_end_y_ptr   = lcu_pix_height + 5;
                *cur_lcu_start_x_ptr = lcu_available_left ? 2 : 7;
                *cur_lcu_end_x_ptr   = lcu_pix_width + 5;
				x_test = 37;
				y_test = 37;
                


            }
            break;
            case (int )SAO_TYPE_EO_45:
            {
                //*cur_lcu_start_x_ptr = lcu_available_left  ?  2 : 7;
                //*cur_lcu_end_x_ptr   = lcu_available_right ? 37 : (lcu_pix_width + 5);
                //*cur_lcu_start_y_ptr = lcu_available_up    ?  2 : 7;
                //*cur_lcu_end_y_ptr   = lcu_available_down  ? 37 : (lcu_pix_height + 5);
                *cur_lcu_start_y_ptr = lcu_available_up   ? 2 : 7;
                *cur_lcu_end_y_ptr   = lcu_pix_height + 5;
                *cur_lcu_start_x_ptr = lcu_available_left ? 2 : 7;
                *cur_lcu_end_x_ptr   = lcu_pix_width + 5;
				x_test = 37;
				y_test = 37;
                

            }
            break;
            case (int )SAO_TYPE_BO:
            {
                //*cur_lcu_start_x_ptr = lcu_available_left  ?  2 : 6;
                //*cur_lcu_end_x_ptr   = lcu_available_right ? 38 : (lcu_pix_width + 6);
                //*cur_lcu_start_y_ptr = lcu_available_up    ?  2 : 6;
                //*cur_lcu_end_y_ptr   = lcu_available_down  ? 38 : (lcu_pix_height + 6);
                *cur_lcu_start_y_ptr = lcu_available_up   ? 2 : 6;
                *cur_lcu_end_y_ptr   = lcu_pix_height + 6;
                *cur_lcu_start_x_ptr = lcu_available_left ? 2 : 6;
                *cur_lcu_end_x_ptr   = lcu_pix_width + 6;
				x_test = 38;
				y_test = 38;

            }
            break;
            default:
            {
                printf("Not a supported SAO types\n");
                exit(- 1);
            }
        }
#if !SIMPLE_MD || SAO_YUV
        LOOP_UV:for (y=2; y<y_test; y++)
                {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=36
                    for (x=2; x<37; x++)
                    {
#pragma HLS UNROLL factor=35
                        edgeType = SaoTypeUV_Test(sample_bit_depth, temp_sao_lcu_array_uv, y, x,type);
                        sao_rslt_array_uv[y][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, cur_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, x);
                    }
					if ((int )SAO_TYPE_BO == type)
					{
						edgeType = SaoTypeUVBO(sample_bit_depth,temp_sao_lcu_array_uv,y,x);
						sao_rslt_array_uv[y][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, cur_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, x);
					} else if ((int )SAO_TYPE_EO_90 == type) {
						edgeType = SaoTypeUVEO90(temp_sao_lcu_array_uv, y, x);
						sao_rslt_array_uv[y][x] = SaoCalcRecPixUV_2(dstStride, /*rec_saoParam,*/rec_sao_param_local_offset, sample_bit_depth, cur_lcu_idx, edgeType, temp_sao_lcu_array_uv, y, x);
            }
        }
#endif
    }
}

static void SAO_on_shifted_LCU(int pix_x, int pix_y, int pic_width, int pic_height, int lcu_width, int lcu_height, int LCU_col_num, int lcu_pos, int sample_bit_depth, int rec_Stride_luma, int rec_Stride_chroma,
                               //int pic_width_in_scu, s8 *map_patch_idx, u8 cross_patch_loop_filter,  // 2021-03-23, liaozhx: No PATCH concept, keep this line for illustration.
                               int rec_sao_param_local_y_modeIdc[62], int rec_sao_param_local_y_typeIdc[62], int rec_sao_param_local_y_offset[62][32],                               //SAOBlkParam rec_sao_param_local_y[60 + 2], 
                               int rec_sao_param_local_u_modeIdc[62], int rec_sao_param_local_u_typeIdc[62], int rec_sao_param_local_u_offset[62][32],                               //SAOBlkParam rec_sao_param_local_u[60 + 2], 
                               int rec_sao_param_local_v_modeIdc[62], int rec_sao_param_local_v_typeIdc[62], int rec_sao_param_local_v_offset[62][32],                               //SAOBlkParam rec_sao_param_local_v[60 + 2], 
                               pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli1[70][70], pel temp_sao_lcu_array_u[38][38], pel temp_sao_lcu_array_v[38][38],
                               pel sao_rslt_array_y[70][70],                                             pel sao_rslt_array_u[38][38],     pel sao_rslt_array_v[38][38],
                               int* lcu_avail_left_ptr,    int* lcu_avail_right_ptr,   int* lcu_avail_up_ptr,        int* lcu_avail_down_ptr, 
                             //int* lcu_avail_up_left_ptr, int* lcu_avail_up_right_ptr,int* lcu_avail_down_left_ptr, int* lcu_avail_down_right_ptr, 
                               int* sq_sao_mode_y_ptr,     int* up2row_sao_mode_y_ptr, int* left2col_sao_mode_y_ptr, int* cur_lcu_sao_mode_y_ptr,
                               int* up2row_y_start_x_ptr,  int* up2row_y_end_x_ptr,    int* left2col_y_start_y_ptr,  int* left2col_y_end_y_ptr,
                               int* cur_lcu_y_start_x_ptr, int* cur_lcu_y_end_x_ptr,   int* cur_lcu_y_start_y_ptr,   int* cur_lcu_y_end_y_ptr,
                               int* sq_sao_mode_u_ptr,     int* up2row_sao_mode_u_ptr, int* left2col_sao_mode_u_ptr, int* cur_lcu_sao_mode_u_ptr,
                               int* up2row_u_start_x_ptr,  int* up2row_u_end_x_ptr,    int* left2col_u_start_y_ptr,  int* left2col_u_end_y_ptr,
                               int* cur_lcu_u_start_x_ptr, int* cur_lcu_u_end_x_ptr,   int* cur_lcu_u_start_y_ptr,   int* cur_lcu_u_end_y_ptr,
                               int* sq_sao_mode_v_ptr,     int* up2row_sao_mode_v_ptr, int* left2col_sao_mode_v_ptr, int* cur_lcu_sao_mode_v_ptr,
                               int* up2row_v_start_x_ptr,  int* up2row_v_end_x_ptr,    int* left2col_v_start_y_ptr,  int* left2col_v_end_y_ptr,
                               int* cur_lcu_v_start_x_ptr, int* cur_lcu_v_end_x_ptr,   int* cur_lcu_v_start_y_ptr,   int* cur_lcu_v_end_y_ptr)
{
#pragma HLS inline off

    int lcu_pix_height_luma;
    int lcu_pix_width_luma;

    int lcu_pix_height_chroma;
    int lcu_pix_width_chroma;
    int sq_lcu_idx;
    int up2row_lcu_idx;
    int left2col_lcu_idx;
    int cur_lcu_idx;

    int temp_1, temp_2, temp_3, temp_4;

// Y
    lcu_pix_width_luma  = lcu_width;
    lcu_pix_height_luma = lcu_height;

    //checkBoundaryProc( pic_width,  pic_height,  pic_width_in_scu,  map_patch_idx,  cross_patch_loop_filter,  pix_y_luma, pix_x_luma, lcu_pix_height_luma, lcu_pix_width_luma, 0, 
    //                   lcu_avail_left_ptr, lcu_avail_right_ptr, lcu_avail_up_ptr, lcu_avail_down_ptr, lcu_avail_up_left_ptr, lcu_avail_up_right_ptr, lcu_avail_down_left_ptr, lcu_avail_down_right_ptr, 1);

    lcu_boundary(pix_x, pix_y, pic_width, pic_height, lcu_width, lcu_height, lcu_avail_up_ptr, lcu_avail_down_ptr, lcu_avail_left_ptr, lcu_avail_right_ptr);

    temp_1 = lcu_pos + 1;
    temp_2 = temp_1 + 1;            //lcu_pos + 2
    temp_3 = temp_1 + LCU_col_num;  // lcu_pos + LCU_col_num + 1
    temp_4 = LCU_col_num + 2;
    //sq_lcu_idx = (lcu_pos + 1) % (LCU_col_num + 2);                         //(lcu_pos - LCU_col_num - 1 + LCU_col_num + 2) % (LCU_col_num + 2);
    //up2row_lcu_idx = (lcu_pos + 2) % (LCU_col_num + 2);                     //(lcu_pos - LCU_col_num + LCU_col_num + 2) % (LCU_col_num + 2);
    //left2col_lcu_idx = (lcu_pos + LCU_col_num + 1) % (LCU_col_num + 2);     // (lcu_pos - 1 + LCU_col_num + 2) % (LCU_col_num + 2);
    //cur_lcu_idx = lcu_pos % (LCU_col_num + 2);
    sq_lcu_idx       = temp_1  % temp_4;                       //(lcu_pos - LCU_col_num - 1 + LCU_col_num + 2) % (LCU_col_num + 2);
    up2row_lcu_idx   = temp_2  % temp_4;                   //(lcu_pos - LCU_col_num + LCU_col_num + 2) % (LCU_col_num + 2);
    left2col_lcu_idx = temp_3  % temp_4;                 // (lcu_pos - 1 + LCU_col_num + 2) % (LCU_col_num + 2);
    cur_lcu_idx      = lcu_pos % temp_4;                     // lcu_pos % (LCU_col_num + 2)

    SaoCalcShiftBlk_Y(sample_bit_depth, sao_rslt_array_y, rec_Stride_luma, //rec_sao_param_local_y, 
                      rec_sao_param_local_y_modeIdc, rec_sao_param_local_y_typeIdc, rec_sao_param_local_y_offset,
                      lcu_pix_height_luma, lcu_pix_width_luma,
                      *lcu_avail_left_ptr, *lcu_avail_right_ptr, *lcu_avail_up_ptr, *lcu_avail_down_ptr,      // *lcu_avail_up_left_ptr, *lcu_avail_up_right_ptr, *lcu_avail_down_left_ptr, *lcu_avail_down_right_ptr,
                      temp_sao_lcu_array_y, temp_sao_lcu_array_y_dupli1, sq_lcu_idx, up2row_lcu_idx, left2col_lcu_idx, cur_lcu_idx,
                      sq_sao_mode_y_ptr,     up2row_sao_mode_y_ptr, left2col_sao_mode_y_ptr, cur_lcu_sao_mode_y_ptr,
                      up2row_y_start_x_ptr,  up2row_y_end_x_ptr,    left2col_y_start_y_ptr,  left2col_y_end_y_ptr,
                      cur_lcu_y_start_x_ptr, cur_lcu_y_end_x_ptr,   cur_lcu_y_start_y_ptr,   cur_lcu_y_end_y_ptr);
    // U
    lcu_pix_width_chroma  = lcu_width  >> 1;
    lcu_pix_height_chroma = lcu_height >> 1;

    SaoCalcShiftBlk_UV(sample_bit_depth, sao_rslt_array_u, rec_Stride_chroma, //rec_sao_param_local_u, 
                       rec_sao_param_local_u_modeIdc, rec_sao_param_local_u_typeIdc, rec_sao_param_local_u_offset,
                       lcu_pix_height_chroma, lcu_pix_width_chroma,
                       *lcu_avail_left_ptr, *lcu_avail_right_ptr, *lcu_avail_up_ptr, *lcu_avail_down_ptr,    // *lcu_avail_up_left_ptr, *lcu_avail_up_right_ptr, *lcu_avail_down_left_ptr, *lcu_avail_down_right_ptr,
                       temp_sao_lcu_array_u, sq_lcu_idx, up2row_lcu_idx, left2col_lcu_idx, cur_lcu_idx, 
                       sq_sao_mode_u_ptr, up2row_sao_mode_u_ptr, left2col_sao_mode_u_ptr, cur_lcu_sao_mode_u_ptr,
                       up2row_u_start_x_ptr, up2row_u_end_x_ptr, left2col_u_start_y_ptr, left2col_u_end_y_ptr,
                       cur_lcu_u_start_x_ptr, cur_lcu_u_end_x_ptr, cur_lcu_u_start_y_ptr, cur_lcu_u_end_y_ptr);
    // V
    SaoCalcShiftBlk_UV(sample_bit_depth, sao_rslt_array_v, rec_Stride_chroma, //rec_sao_param_local_v, 
                       rec_sao_param_local_v_modeIdc, rec_sao_param_local_v_typeIdc, rec_sao_param_local_v_offset,
                       lcu_pix_height_chroma, lcu_pix_width_chroma,
                       *lcu_avail_left_ptr, *lcu_avail_right_ptr, *lcu_avail_up_ptr, *lcu_avail_down_ptr,    // *lcu_avail_up_left_ptr, *lcu_avail_up_right_ptr, *lcu_avail_down_left_ptr, *lcu_avail_down_right_ptr,
                       temp_sao_lcu_array_v, sq_lcu_idx, up2row_lcu_idx, left2col_lcu_idx, cur_lcu_idx, 
                       sq_sao_mode_v_ptr,     up2row_sao_mode_v_ptr, left2col_sao_mode_v_ptr, cur_lcu_sao_mode_v_ptr,
                       up2row_v_start_x_ptr,  up2row_v_end_x_ptr,    left2col_v_start_y_ptr,  left2col_v_end_y_ptr,
                       cur_lcu_v_start_x_ptr, cur_lcu_v_end_x_ptr,   cur_lcu_v_start_y_ptr,   cur_lcu_v_end_y_ptr);
}

//static void SAO_write_rec(int pix_x_luma, int pix_y_luma, int pix_x_chroma, int pix_y_chroma, int rec_stride_luma, int rec_stride_chroma, 
//    COM_PIC *pic_rec, pel best_rec_y[64][64], pel best_rec_u[32][32], pel    best_rec_v[32][32])
//{
//
//#pragma HLS INLINE off
//
//  int rec_offset_y = pix_y_luma * rec_stride_luma + pix_x_luma;
//  int rec_offset_uv = pix_y_chroma * rec_stride_chroma + pix_x_chroma;
//  int i;
//  int j;
//  for (i = 0; i < 64; i++) {
//    for (j = 0; j < 64; j++) {
//        (pic_rec->y + rec_offset_y)[i * rec_stride_luma + j] = best_rec_y[i][j];
//    }
//  }
//  for (i = 0; i < 32; i++) {
//    for (j = 0; j < 32; j++) {
//        (pic_rec->u + rec_offset_uv)[i * rec_stride_chroma + j] = best_rec_u[i][j];
//        (pic_rec->v + rec_offset_uv)[i * rec_stride_chroma + j] = best_rec_v[i][j];
//    }
//  }
//}

void sao_process(int log2_max_cuwh, int pic_width, int pic_height, int pic_width_in_lcu, int pic_height_in_lcu, int bit_depth_internal, 
    u8 sample_adaptive_offset_enable_flag, int qp_offset_bit_depth,
    int lcu_cnt, /*int *bin_count,*/ int max_smb_num, int x_pel, int y_pel, /*ENC_SBAC *s_sao_init,*/ int pic_width_in_scu, 
    pel dbkbuffer_y1[68][68], /*pel dbkbuffer_y2[68][68], pel dbkbuffer_y3[68][68], pel dbkbuffer_y4[68][68],*/ pel dbkbuffer_u[36][36], pel dbkbuffer_v[36][36],    //pel best_rec_y[64][64], pel best_rec_u[32][32], pel    best_rec_v[32][32],
    u8 slice_sao_enable[3],
    pel y_org1[64][64],       /*pel y_org2[64][64],       pel y_org3[64][64],       pel y_org4[64][64],      */ pel u_org[32][32],       pel v_org[32][32],
    s64 lambda[3], 
    int rec_stride_luma, int rec_stride_chroma, int pix_x_luma, int pix_y_luma, int pix_x_chroma, int pix_y_chroma,
    pel temp_sao_lcu_array_y[70][70], pel temp_sao_lcu_array_y_dupli1[70][70], pel temp_sao_lcu_array_u[38][38], pel temp_sao_lcu_array_v[38][38],
  //u8 dbk_lcu_array_y[68][68],                                              u8 dbk_lcu_array_u[36][36],      u8 dbk_lcu_array_v[36][36],
    pel sao_rslt_array_y[70][70], pel sao_rslt_array_u[38][38], pel sao_rslt_array_v[38][38],
    int* lcu_avail_left_ptr,    int* lcu_avail_right_ptr,    int* lcu_avail_up_ptr,        int* lcu_avail_down_ptr,
  //int* lcu_avail_up_left_ptr, int* lcu_avail_up_right_ptr, int* lcu_avail_down_left_ptr, int* lcu_avail_down_right_ptr,
    int* sq_sao_mode_y_ptr,     int* up2row_sao_mode_y_ptr,  int* left2col_sao_mode_y_ptr, int* cur_lcu_sao_mode_y_ptr,
    int* up2row_y_start_x_ptr,  int* up2row_y_end_x_ptr,     int* left2col_y_start_y_ptr,  int* left2col_y_end_y_ptr,
    int* cur_lcu_y_start_x_ptr, int* cur_lcu_y_end_x_ptr,    int* cur_lcu_y_start_y_ptr,   int* cur_lcu_y_end_y_ptr,
    int* sq_sao_mode_u_ptr,     int* up2row_sao_mode_u_ptr,  int* left2col_sao_mode_u_ptr, int* cur_lcu_sao_mode_u_ptr,
    int* up2row_u_start_x_ptr,  int* up2row_u_end_x_ptr,     int* left2col_u_start_y_ptr,  int* left2col_u_end_y_ptr,
    int* cur_lcu_u_start_x_ptr, int* cur_lcu_u_end_x_ptr,    int* cur_lcu_u_start_y_ptr,   int* cur_lcu_u_end_y_ptr,
    int* sq_sao_mode_v_ptr,     int* up2row_sao_mode_v_ptr,  int* left2col_sao_mode_v_ptr, int* cur_lcu_sao_mode_v_ptr,
    int* up2row_v_start_x_ptr,  int* up2row_v_end_x_ptr,     int* left2col_v_start_y_ptr,  int* left2col_v_end_y_ptr,
    int* cur_lcu_v_start_x_ptr, int* cur_lcu_v_end_x_ptr,    int* cur_lcu_v_start_y_ptr,   int* cur_lcu_v_end_y_ptr,
	SAOBlkParam cur_saoParam[3], SAOBlkParam rec_saoParam[60 + 2][3])
{
#pragma HLS inline off
#ifndef __SYNTHESIS__
    static int saoStat_YEO_diff [4][32];
    static int saoStat_YEO_count[4][32];
    static int saoStat_YBO_diff [32];
    static int saoStat_YBO_count[32];

    static int saoStat_UEO_diff [4][32];
    static int saoStat_UEO_count[4][32];

    static int saoStat_UBO_diff [32];
    static int saoStat_UBO_count[32];

    static int saoStat_VEO_diff [4][32];
    static int saoStat_VEO_count[4][32];

    static int saoStat_VBO_diff [32];
    static int saoStat_VBO_count[32];
#else
    int saoStat_YEO_diff [4][32];
    int saoStat_YEO_count[4][32];
    int saoStat_YBO_diff [32];
    int saoStat_YBO_count[32];

    int saoStat_UEO_diff [4][32];
    int saoStat_UEO_count[4][32];

    int saoStat_UBO_diff [32];
    int saoStat_UBO_count[32];

    int saoStat_VEO_diff [4][32];
    int saoStat_VEO_count[4][32];

    int saoStat_VBO_diff [32];
    int saoStat_VBO_count[32];

#pragma HLS RESOURCE variable=saoStat_YEO_diff core=RAM_2P_BRAM 
#pragma HLS RESOURCE variable=saoStat_YEO_count core=RAM_2P_BRAM 
#pragma HLS ARRAY_PARTITION variable=saoStat_YEO_diff  complete dim=1
#pragma HLS ARRAY_PARTITION variable=saoStat_YEO_count complete dim=1
   
#pragma HLS RESOURCE variable=saoStat_UEO_diff core=RAM_2P_BRAM 
#pragma HLS RESOURCE variable=saoStat_UEO_count core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=saoStat_UEO_diff  complete dim=1
#pragma HLS ARRAY_PARTITION variable=saoStat_UEO_count complete dim=1
    
#pragma HLS RESOURCE variable=saoStat_VEO_diff core=RAM_2P_BRAM 
#pragma HLS RESOURCE variable=saoStat_VEO_count core=RAM_2P_BRAM
#pragma HLS ARRAY_PARTITION variable=saoStat_VEO_diff  complete dim=1
#pragma HLS ARRAY_PARTITION variable=saoStat_VEO_count complete dim=1
#endif

    static int rec_sao_param_local_y_modeIdc[62];            // rec_sao_param_local_y is a local SAO param buffer, which contains SAP paramters of up-left LCU, up-side LCU and left-side LCU. Therefore, STATIC key word is necessary.
    static int rec_sao_param_local_y_typeIdc[62];
    static int rec_sao_param_local_y_deltaband[62];
    static int rec_sao_param_local_y_startBand2[62];
    static int rec_sao_param_local_y_startBand[62];
    static int rec_sao_param_local_y_offset[62][32];
    static int rec_sao_param_local_u_modeIdc[62];            // rec_sao_param_local_y is a local SAO param buffer, which contains SAP paramters of up-left LCU, up-side LCU and left-side LCU. Therefore, STATIC key word is necessary.
    static int rec_sao_param_local_u_typeIdc[62];
    static int rec_sao_param_local_u_deltaband[62];
    static int rec_sao_param_local_u_startBand2[62];
    static int rec_sao_param_local_u_startBand[62];
    static int rec_sao_param_local_u_offset[62][32];
    static int rec_sao_param_local_v_modeIdc[62];            // rec_sao_param_local_y is a local SAO param buffer, which contains SAP paramters of up-left LCU, up-side LCU and left-side LCU. Therefore, STATIC key word is necessary.
    static int rec_sao_param_local_v_typeIdc[62];
    static int rec_sao_param_local_v_deltaband[62];
    static int rec_sao_param_local_v_startBand2[62];
    static int rec_sao_param_local_v_startBand[62];
    static int rec_sao_param_local_v_offset[62][32];
//#pragma HLS ARRAY_PARTITION variable=rec_sao_param_local_y_offset complete dim=2
//#pragma HLS ARRAY_PARTITION variable=rec_sao_param_local_u_offset complete dim=2
//#pragma HLS ARRAY_PARTITION variable=rec_sao_param_local_v_offset complete dim=2


    int lcu_pos = max_smb_num - lcu_cnt - 1;
//  int LCU_row_num = (pic_height >> log2_max_cuwh) + ((pic_height % (1 << log2_max_cuwh) ? 1 : 0));
    int LCU_col_num = (pic_width >> log2_max_cuwh) + ((pic_width % (1 << log2_max_cuwh) ? 1 : 0));

    //DBK2SAO();
    //COM_BSW sao_bs;
    //unsigned char a[1] = { ((unsigned char)0) };
    //unsigned char *b = a;
    //sao_bs.cur = b;
    //ENC_SBAC s_temp_run;
    //ENC_SBAC* sao_sbac = &s_temp_run;

    int bit_depth = bit_depth_internal;
    int lcu_x = x_pel;
    int lcu_y = y_pel;
    int lcu_max_width  = 1 << log2_max_cuwh;
    int lcu_max_height = 1 << log2_max_cuwh;
    int lcu_width  = (lcu_max_width  < (pic_width  - lcu_x)) ? lcu_max_width  : (pic_width  - lcu_x);
    int lcu_height = (lcu_max_height < (pic_height - lcu_y)) ? lcu_max_height : (pic_height - lcu_y);

    // printf("LCU[%d] (%d, %d)   W: %d, H: %d. \n", lcu_pos, lcu_y, lcu_x, lcu_width, lcu_height);


    SAO_BIG_SIZE sao_lambda[3];
#pragma HLS ARRAY_PARTITION variable=sao_lambda complete dim=0
    int scale_lambda;
    SAO_BIG_SIZE ctx_lambda_0 = (SAO_BIG_SIZE)lambda[0];

    // if (sample_adaptive_offset_enable_flag)  // No longer concern this flag, sao_2_pic_rec() func. will deal with operations when SAO is off.

    // Initialization
    //com_mcpy_ENC_SBAC_SAO(&s_temp_run, s_sao_init);
#if !SIMPLE_MD || SAO_YUV
    dbk_2_sao(lcu_pos, LCU_col_num,
              dbkbuffer_y1,                                      dbkbuffer_u,          dbkbuffer_v, 
              temp_sao_lcu_array_y, temp_sao_lcu_array_y_dupli1, temp_sao_lcu_array_u, temp_sao_lcu_array_v);
            //dbk_lcu_array_y,                                   dbk_lcu_array_u,      dbk_lcu_array_v);

    // Statistics
    get_sao_statistic(bit_depth, slice_sao_enable, lcu_height, lcu_width, 
        dbkbuffer_y1, dbkbuffer_u, dbkbuffer_v,
        y_org1,       u_org,       v_org,
      //saoStat_YEO0_diff,   saoStat_YEO0_count,    saoStat_YEO90_diff,   saoStat_YEO90_count,   saoStat_YEO135_diff,  saoStat_YEO135_count,  saoStat_YEO45_diff,   saoStat_YEO45_count, 
        saoStat_YEO_diff[0], saoStat_YEO_count[0], saoStat_YEO_diff[1], saoStat_YEO_count[1], saoStat_YEO_diff[2], saoStat_YEO_count[2], saoStat_YEO_diff[3], saoStat_YEO_count[3], saoStat_YBO_diff, saoStat_YBO_count,
        saoStat_UEO_diff[0], saoStat_UEO_count[0], saoStat_UEO_diff[1], saoStat_UEO_count[1], saoStat_UEO_diff[2], saoStat_UEO_count[2], saoStat_UEO_diff[3], saoStat_UEO_count[3], saoStat_UBO_diff, saoStat_UBO_count,
        saoStat_VEO_diff[0], saoStat_VEO_count[0], saoStat_VEO_diff[1], saoStat_VEO_count[1], saoStat_VEO_diff[2], saoStat_VEO_count[2], saoStat_VEO_diff[3], saoStat_VEO_count[3], saoStat_VBO_diff,  saoStat_VBO_count);


    // Mode Decision
    scale_lambda = (bit_depth == 10 ? qp_offset_bit_depth : 1);
    
    for (int compIdx = 0; compIdx < 3; compIdx++) {
#pragma HLS UNROLL
        sao_lambda[compIdx] = ctx_lambda_0 * scale_lambda;
    }

    get_sao_param(/*pic_width,*/ log2_max_cuwh, /*pic_width_in_scu, pic_width_in_lcu,*/ /*&bin_count,*/ slice_sao_enable, LCU_col_num,  lcu_pos, lcu_y, lcu_x, sao_lambda, 
      //saoStat_YEO0_diff, saoStat_YEO0_count, saoStat_YEO90_diff, saoStat_YEO90_count, saoStat_YEO135_diff, saoStat_YEO135_count, saoStat_YEO45_diff, saoStat_YEO45_count, 
        saoStat_YEO_diff, saoStat_YEO_count, saoStat_YBO_diff, saoStat_YBO_count,    
      //saoStat_UEO0_diff,   saoStat_UEO0_count,    saoStat_UEO90_diff,   saoStat_UEO90_count,   saoStat_UEO135_diff,  saoStat_UEO135_count,  saoStat_UEO45_diff,   saoStat_UEO45_count,  
        saoStat_UEO_diff, saoStat_UEO_count, saoStat_UBO_diff, saoStat_UBO_count,
      //saoStat_VEO0_diff,   saoStat_VEO0_count,    saoStat_VEO90_diff,   saoStat_VEO90_count,   saoStat_VEO135_diff,  saoStat_VEO135_count,  saoStat_VEO45_diff,   saoStat_VEO45_count,  
        saoStat_VEO_diff, saoStat_VEO_count, saoStat_VBO_diff, saoStat_VBO_count,
        rec_sao_param_local_y_modeIdc,    rec_sao_param_local_y_typeIdc,    rec_sao_param_local_y_startBand,   rec_sao_param_local_y_startBand2,
        rec_sao_param_local_y_deltaband,  rec_sao_param_local_y_offset,     rec_sao_param_local_u_modeIdc,     rec_sao_param_local_u_typeIdc,
        rec_sao_param_local_u_startBand,  rec_sao_param_local_u_startBand2, rec_sao_param_local_u_deltaband,   rec_sao_param_local_u_offset,
        rec_sao_param_local_v_modeIdc,    rec_sao_param_local_v_typeIdc,    rec_sao_param_local_v_startBand,   rec_sao_param_local_v_startBand2,
        rec_sao_param_local_v_deltaband,  rec_sao_param_local_v_offset, cur_saoParam, rec_saoParam);
#endif

    // Calculation
    SAO_on_shifted_LCU(lcu_x, lcu_y,  pic_width,  pic_height,  lcu_width, lcu_height,  LCU_col_num, lcu_pos, bit_depth, rec_stride_luma, rec_stride_chroma,
                       //pic_width_in_scu, map_patch_idx,  cross_patch_loop_filter,
                       rec_sao_param_local_y_modeIdc, rec_sao_param_local_y_typeIdc, rec_sao_param_local_y_offset,                        //rec_sao_param_local_y, 
                       rec_sao_param_local_u_modeIdc, rec_sao_param_local_u_typeIdc, rec_sao_param_local_u_offset,                        //rec_sao_param_local_u, 
                       rec_sao_param_local_v_modeIdc, rec_sao_param_local_v_typeIdc, rec_sao_param_local_v_offset,                        //rec_sao_param_local_v, 
                       temp_sao_lcu_array_y, temp_sao_lcu_array_y_dupli1, temp_sao_lcu_array_u, temp_sao_lcu_array_v,
                       sao_rslt_array_y,                                  sao_rslt_array_u,     sao_rslt_array_v, 
                       lcu_avail_left_ptr,    lcu_avail_right_ptr,   lcu_avail_up_ptr,        lcu_avail_down_ptr,     //lcu_avail_up_left_ptr, lcu_avail_up_right_ptr, lcu_avail_down_left_ptr, lcu_avail_down_right_ptr,
                       sq_sao_mode_y_ptr,     up2row_sao_mode_y_ptr, left2col_sao_mode_y_ptr, cur_lcu_sao_mode_y_ptr,
                       up2row_y_start_x_ptr,  up2row_y_end_x_ptr,    left2col_y_start_y_ptr,  left2col_y_end_y_ptr,
                       cur_lcu_y_start_x_ptr, cur_lcu_y_end_x_ptr,   cur_lcu_y_start_y_ptr,   cur_lcu_y_end_y_ptr,
                       sq_sao_mode_u_ptr,     up2row_sao_mode_u_ptr, left2col_sao_mode_u_ptr, cur_lcu_sao_mode_u_ptr,
                       up2row_u_start_x_ptr,  up2row_u_end_x_ptr,    left2col_u_start_y_ptr,  left2col_u_end_y_ptr,
                       cur_lcu_u_start_x_ptr, cur_lcu_u_end_x_ptr,   cur_lcu_u_start_y_ptr,   cur_lcu_u_end_y_ptr,
                       sq_sao_mode_v_ptr,     up2row_sao_mode_v_ptr, left2col_sao_mode_v_ptr, cur_lcu_sao_mode_v_ptr,
                       up2row_v_start_x_ptr,  up2row_v_end_x_ptr,    left2col_v_start_y_ptr,  left2col_v_end_y_ptr,
                       cur_lcu_v_start_x_ptr, cur_lcu_v_end_x_ptr,   cur_lcu_v_start_y_ptr,   cur_lcu_v_end_y_ptr);

    // This func. used to deal with the copying operations when SAO is off.
    //SAO_write_rec();
}

//该函数决定是保存DBK(去方块滤波)输出数值还是保存sao滤波后的数值
void sao_2_pic_rec2(int max_smb_num, int lcu_cnt, int pic_width, int log2_max_cuwh,
    pel sao_pic_rec_test_y[69][69], pel sao_pic_rec_test_u[36][36], pel sao_pic_rec_test_v[36][36],
    pel dbkbuffer_y[68][68], pel dbkbuffer_u[36][36], pel dbkbuffer_v[36][36],
    u8 sample_adaptive_offset_enable_flag, int pix_y_luma, int pix_x_luma, int pix_y_chroma, int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma,
    pel temp_sao_lcu_array_u[38][38], pel temp_sao_lcu_array_v[38][38],
    //u8 dbk_lcu_array_y[68][68], u8 dbk_lcu_array_u[36][36],   u8 dbk_lcu_array_v[36][36],
    pel sao_rslt_array_y[70][70], pel sao_rslt_array_u[38][38], pel sao_rslt_array_v[38][38],
    int lcu_avail_left, int lcu_avail_up,
    int sq_sao_mode_y, int up2row_sao_mode_y, int left2col_sao_mode_y, int cur_lcu_sao_mode_y,
    int up2row_y_start_x, int up2row_y_end_x, int left2col_y_start_y, int left2col_y_end_y,
    int cur_lcu_y_start_x, int cur_lcu_y_end_x, int cur_lcu_y_start_y, int cur_lcu_y_end_y,
    int sq_sao_mode_u, int up2row_sao_mode_u, int left2col_sao_mode_u, int cur_lcu_sao_mode_u,
    int up2row_u_start_x, int up2row_u_end_x, int left2col_u_start_y, int left2col_u_end_y,
    int cur_lcu_u_start_x, int cur_lcu_u_end_x, int cur_lcu_u_start_y, int cur_lcu_u_end_y,
    int sq_sao_mode_v, int up2row_sao_mode_v, int left2col_sao_mode_v, int cur_lcu_sao_mode_v,
    int up2row_v_start_x, int up2row_v_end_x, int left2col_v_start_y, int left2col_v_end_y,
    int cur_lcu_v_start_x, int cur_lcu_v_end_x, int cur_lcu_v_start_y, int cur_lcu_v_end_y)
{
    //对于4k视频来说，3840/64=60;所以每一行CTU个数最多为60
    /*
    line_buff_1x1:存储DBK(去方块滤波)输出但未经SAO处理的CTU最右下角一个数据；
    line_buff_1x68：存储DBK(去方块滤波)输出但未经SAO处理的CTU最下侧一行的数据
    line_buff_68x1：存储DBK(去方块滤波)输出但未经SAO处理的CTU最右侧一列的数据
    上述三个数组，都是为未进行处理的临近CTU提供DBK输出数值
    */
    static pel line_buff_1x1[MAX_NUM_LCU_ROW + 1];
    static pel line_buff_1x68[MAX_NUM_LCU_ROW][68];
    static pel line_buff_68x1[68];

    int lcu_pos = max_smb_num - lcu_cnt - 1;//当前处理CTU的标记(例如：416*240 -> 448*256  共28个CTU(0~27)，每行7个CTU，共4行)
    //  int LCU_row_num = (pic_height >> log2_max_cuwh) + ((pic_height % (1 << log2_max_cuwh) ? 1 : 0));
    int LCU_col_num = (pic_width >> log2_max_cuwh) + ((pic_width % (1 << log2_max_cuwh) ? 1 : 0));//每一行CTU的个数
    /*
    以416*240分辨率为例：共28个CTU(0~27)，每行7个CTU，共4行
    lcu_pos                         idx_0                    idx1_0
     0, 1, 2, 3, 4, 5, 6            0,1,2,3,4,5,6            0,1,2,3,4,5,6
     7, 8, 9,10,11,12,13            7,0,1,2,3,4,5            0,1,2,3,4,5,6
    14,15,16,17,18,19,20            6,7,0,1,2,3,4            0,1,2,3,4,5,6
    21,22,23,24,25,26,27            5,6,7,0,1,2,3            0,1,2,3,4,5,6
    */
    int idx_0  = lcu_pos % (LCU_col_num + 1);
    int idx1_0 = lcu_pos % LCU_col_num;

#pragma HLS RESOURCE variable=line_buff_1x1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=line_buff_1x68 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=line_buff_68x1 core=RAM_1P_BRAM

#pragma HLS array_partition variable=line_buff_1x1 complete
#pragma HLS array_partition variable=line_buff_1x68 complete dim=2
#pragma HLS array_partition variable=line_buff_68x1 complete

#pragma HLS array_partition variable=sao_pic_rec_test_y complete dim=2
#pragma HLS array_partition variable=sao_pic_rec_test_u complete dim=2
#pragma HLS array_partition variable=sao_pic_rec_test_v complete dim=2

#pragma HLS array_partition variable=dbkbuffer_y complete dim=2
#pragma HLS array_partition variable=dbkbuffer_u complete dim=2
#pragma HLS array_partition variable=dbkbuffer_v complete dim=2

#pragma HLS array_partition variable=sao_rslt_array_y complete dim=2
#pragma HLS array_partition variable=sao_rslt_array_u complete dim=2
#pragma HLS array_partition variable=sao_rslt_array_v complete dim=2

#pragma HLS array_partition variable=temp_sao_lcu_array_u complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_v complete dim=2

    for (int y = 0; y < 69; y++) 
    {
#pragma HLS pipeline
        for (int x = 0; x < 69; x++) 
        {
#pragma HLS unroll
            if (y == 0) 
            {// (0,0~68): 1st ROW
                if (x == 0) 
                {   // 1st ROW;(0,0)：sao打开、上侧存在CTU、左侧存在CTU、RDOQ之后决定滤波
                    if (sample_adaptive_offset_enable_flag && lcu_avail_up && lcu_avail_left && (sq_sao_mode_y != (int)SAO_MODE_OFF)) 
                    {
                        sao_pic_rec_test_y[y][x] = sao_rslt_array_y[y + 1][x + 1];//使用经过sao处理的数据
                    } 
                    else 
                    {
                        sao_pic_rec_test_y[y][x] = line_buff_1x1[idx_0];//使用DBK输出但未经sao处理过的数据
                    }
                }
                else 
                { // (0,1~68): 1st ROW
                    if (sample_adaptive_offset_enable_flag && lcu_avail_up && (up2row_sao_mode_y != (int)SAO_MODE_OFF) && (up2row_y_start_x <= x + 1) && (x + 1 < up2row_y_end_x)) 
                    {
                        sao_pic_rec_test_y[y][x] = sao_rslt_array_y[y + 1][x + 1];
                    } 
                    else 
                    {
                        sao_pic_rec_test_y[y][x] = line_buff_1x68[idx1_0][x-1];
                    }
                }
            }
            else if (x == 0) 
            { // (1~68, 0): 1st COL
                if (sample_adaptive_offset_enable_flag && lcu_avail_left && (left2col_sao_mode_y != (int)SAO_MODE_OFF) && (left2col_y_start_y <= y + 1) && (y + 1 < left2col_y_end_y)) 
                {
                    sao_pic_rec_test_y[y][x] = sao_rslt_array_y[y + 1][x + 1];
                } 
                else 
                {
                    sao_pic_rec_test_y[y][x] = line_buff_68x1[y - 1];
                }
            }
            else 
            { // [1~68][1~68]
                if (sample_adaptive_offset_enable_flag && (cur_lcu_sao_mode_y != (int)SAO_MODE_OFF) && (cur_lcu_y_start_y <= y + 1) && (y + 1 < cur_lcu_y_end_y) && (cur_lcu_y_start_x <= x + 1) && (x + 1 < cur_lcu_y_end_x)) 
                {
                    sao_pic_rec_test_y[y][x] = sao_rslt_array_y[y + 1][x + 1];
                } 
                else 
                {
                    sao_pic_rec_test_y[y][x] = dbkbuffer_y[y - 1][x - 1];
                }
            }
        }
    }

    line_buff_1x1[idx_0] = dbkbuffer_y[63][63];

    for (int x = 0; x < 68; x++) 
    {
#pragma HLS unroll
        line_buff_1x68[idx1_0][x] = dbkbuffer_y[63][x];
    }
    for (int y = 0; y < 68; y++) 
    {
#pragma HLS unroll
        line_buff_68x1[y] = dbkbuffer_y[y][63];
    }

    for (int y = 0; y < 36; y++) 
    {
#pragma HLS pipeline
        for (int x = 0; x < 36; x++) 
        {
#pragma HLS unroll
            if (sample_adaptive_offset_enable_flag && (cur_lcu_sao_mode_u != (int)SAO_MODE_OFF) && (cur_lcu_u_start_y <= y + 2) && (y + 2 < cur_lcu_u_end_y) && (cur_lcu_u_start_x <= x + 2) && (x + 2 < cur_lcu_u_end_x)) 
            {
                sao_pic_rec_test_u[y][x] = sao_rslt_array_u[y + 2][x + 2];
            } else 
            {
                sao_pic_rec_test_u[y][x] = temp_sao_lcu_array_u[y + 2][x + 2];
            }

            if (sample_adaptive_offset_enable_flag && (cur_lcu_sao_mode_v != (int)SAO_MODE_OFF) && (cur_lcu_v_start_y <= y + 2) && (y + 2 < cur_lcu_v_end_y) && (cur_lcu_v_start_x <= x + 2) && (x + 2 < cur_lcu_v_end_x)) 
            {
                sao_pic_rec_test_v[y][x] = sao_rslt_array_v[y + 2][x + 2];
            } 
            else 
            {
                sao_pic_rec_test_v[y][x] = temp_sao_lcu_array_v[y + 2][x + 2];
            }
        }
    }
}

void sao_2_pic_rec_luma(int max_smb_num, int lcu_cnt, int pic_width, int log2_max_cuwh, u8 sample_adaptive_offset_enable_flag, int pix_y_luma, int pix_x_luma, int rec_stride_luma,
    pel* y_rec, pel dbkbuffer_y[68][68], pel sao_rslt_array_y[70][70],
    int lcu_avail_left,    int lcu_avail_up,
    int sq_sao_mode_y,     int up2row_sao_mode_y, int left2col_sao_mode_y, int cur_lcu_sao_mode_y,
    int up2row_y_start_x,  int up2row_y_end_x,    int left2col_y_start_y,  int left2col_y_end_y,
    int cur_lcu_y_start_x, int cur_lcu_y_end_x,   int cur_lcu_y_start_y,   int cur_lcu_y_end_y)
{
#pragma HLS INLINE off
    static pel line_buff_1x1[MAX_NUM_LCU_ROW + 1];
    static pel line_buff_1x68[MAX_NUM_LCU_ROW][68];
    static pel line_buff_68x1[68];
    int lcu_pos = max_smb_num - lcu_cnt - 1;
    //  int LCU_row_num = (pic_height >> log2_max_cuwh) + ((pic_height % (1 << log2_max_cuwh) ? 1 : 0));
    int LCU_col_num = (pic_width >> log2_max_cuwh) + ((pic_width % (1 << log2_max_cuwh) ? 1 : 0));
    int idx_0 = lcu_pos % (LCU_col_num + 1);
    int idx1_0 = lcu_pos % LCU_col_num;

    LOOP_SAO_WR_OUTPUT_Y_1: for (int y = 0; y < 69; y++) {
        LOOP_SAO_WR_OUTPUT_Y_2: for (int x = 0; x < 69; x++) {
#pragma HLS PIPELINE
          //if ((pix_y_luma + y - 5) * rec_stride_luma + pix_x_luma + x - 5 >= 0) {
            if ((pix_y_luma + y - 5 >= 0) && (pix_x_luma + x - 5 >= 0)) {   // If current pixel is inside the picture.
                pel temp;
                if (y == 0) {
                    if (x == 0) { // (0,0)
                        if (sample_adaptive_offset_enable_flag && lcu_avail_up && lcu_avail_left && (sq_sao_mode_y != (int)SAO_MODE_OFF)) {
                            temp = sao_rslt_array_y[y + 1][x + 1];
                        } else {
                            temp = line_buff_1x1[idx_0];
                        }
                    }
                    else { // (0,1~68): 1st ROW
                        if (sample_adaptive_offset_enable_flag && lcu_avail_up && (up2row_sao_mode_y != (int)SAO_MODE_OFF) && (up2row_y_start_x <= x + 1) && (x + 1 < up2row_y_end_x)) {
                            temp = sao_rslt_array_y[y + 1][x + 1];
                        } else {
                            temp = line_buff_1x68[idx1_0][x - 1];
                        }
                    }
                }
                else if (x == 0) { // (1~68, 0): 1st COL
                    if (sample_adaptive_offset_enable_flag && lcu_avail_left && (left2col_sao_mode_y != (int)SAO_MODE_OFF) && (left2col_y_start_y <= y + 1) && (y + 1 < left2col_y_end_y)) {
                        temp = sao_rslt_array_y[y + 1][x + 1];
                    } else {
                        temp = line_buff_68x1[y - 1];
                    }
                }
                else { // [1~68][1~68]
                    if (sample_adaptive_offset_enable_flag && (cur_lcu_sao_mode_y != (int)SAO_MODE_OFF) && (cur_lcu_y_start_y <= y + 1) && (y + 1 < cur_lcu_y_end_y) && (cur_lcu_y_start_x <= x + 1) && (x + 1 < cur_lcu_y_end_x)) {
                        temp = sao_rslt_array_y[y + 1][x + 1];
                    } else {
                        temp = dbkbuffer_y[y - 1][x - 1];
                    }
                }

                y_rec[(pix_y_luma + y - 5) * rec_stride_luma + pix_x_luma + x - 5] = temp;
            }
        }
    }
    line_buff_1x1[idx_0] = dbkbuffer_y[63][63];
    LOOP_SAO_WR_OUTPUT_LINEBUFF_1: for (int x = 0; x < 68; x++) {
#pragma HLS PIPELINE
        line_buff_1x68[idx1_0][x] = dbkbuffer_y[63][x];
    }
    LOOP_SAO_WR_OUTPUT_LINEBUFF_2: for (int y = 0; y < 68; y++) {
#pragma HLS PIPELINE
        line_buff_68x1[y] = dbkbuffer_y[y][63];
    }
}

void sao_2_pic_rec_chroma(u8 sample_adaptive_offset_enable_flag, int pix_y_luma, int pix_x_luma, int pix_y_chroma, int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma,
    pel* u_rec,                       pel* v_rec,
    pel temp_sao_lcu_array_u[38][38], pel temp_sao_lcu_array_v[38][38],
    pel sao_rslt_array_u[38][38],     pel sao_rslt_array_v[38][38],
    int lcu_avail_left,    int lcu_avail_up,
    int sq_sao_mode_u,     int up2row_sao_mode_u, int left2col_sao_mode_u, int cur_lcu_sao_mode_u,
    int up2row_u_start_x,  int up2row_u_end_x,    int left2col_u_start_y,  int left2col_u_end_y,
    int cur_lcu_u_start_x, int cur_lcu_u_end_x,   int cur_lcu_u_start_y,   int cur_lcu_u_end_y,
    int sq_sao_mode_v,     int up2row_sao_mode_v, int left2col_sao_mode_v, int cur_lcu_sao_mode_v,
    int up2row_v_start_x,  int up2row_v_end_x,    int left2col_v_start_y,  int left2col_v_end_y,
    int cur_lcu_v_start_x, int cur_lcu_v_end_x,   int cur_lcu_v_start_y,   int cur_lcu_v_end_y)
{
#pragma HLS INLINE off
    LOOP_SAO_WR_OUTPUT_UV_1: for (int y = 0; y < 36; y++) {
        LOOP_SAO_WR_OUTPUT_UV_2: for (int x = 0; x < 36; x++) {
#pragma HLS PIPELINE
          //if ((pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4 >= 0) {
            if ((pix_y_chroma + y - 4 >= 0) && (pix_x_chroma + x - 4 >= 0)) {
                pel temp_u, temp_v;
                // U:
                if (sample_adaptive_offset_enable_flag && (cur_lcu_sao_mode_u != (int)SAO_MODE_OFF) && (cur_lcu_u_start_y <= y + 2) && (y + 2 < cur_lcu_u_end_y) && (cur_lcu_u_start_x <= x + 2) && (x + 2 < cur_lcu_u_end_x)) {
                    temp_u = sao_rslt_array_u[y + 2][x + 2];
                } else {
                    temp_u = temp_sao_lcu_array_u[y + 2][x + 2];
                }
                // V:
                if (sample_adaptive_offset_enable_flag && (cur_lcu_sao_mode_v != (int)SAO_MODE_OFF) && (cur_lcu_v_start_y <= y + 2) && (y + 2 < cur_lcu_v_end_y) && (cur_lcu_v_start_x <= x + 2) && (x + 2 < cur_lcu_v_end_x)) {
                    temp_v = sao_rslt_array_v[y + 2][x + 2];
                } else {
                    temp_v = temp_sao_lcu_array_v[y + 2][x + 2];
                }

                u_rec[(pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4] = temp_u;
                v_rec[(pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4] = temp_v;
            }
        }
    }
}

void sao_2_pic_rec3(int max_smb_num, int lcu_cnt, int pic_width, int log2_max_cuwh,
    u8 sample_adaptive_offset_enable_flag, int pix_y_luma, int pix_x_luma, int pix_y_chroma, int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma,
    pel* y_rec,                   pel* u_rec,                       pel* v_rec,
    pel dbkbuffer_y[68][68],      pel temp_sao_lcu_array_u[38][38], pel temp_sao_lcu_array_v[38][38],
    pel sao_rslt_array_y[70][70], pel sao_rslt_array_u[38][38],     pel sao_rslt_array_v[38][38],
    int lcu_avail_left,    int lcu_avail_up,
    int sq_sao_mode_y,     int up2row_sao_mode_y, int left2col_sao_mode_y, int cur_lcu_sao_mode_y,
    int up2row_y_start_x,  int up2row_y_end_x,    int left2col_y_start_y,  int left2col_y_end_y,
    int cur_lcu_y_start_x, int cur_lcu_y_end_x,   int cur_lcu_y_start_y,   int cur_lcu_y_end_y,
    int sq_sao_mode_u,     int up2row_sao_mode_u, int left2col_sao_mode_u, int cur_lcu_sao_mode_u,
    int up2row_u_start_x,  int up2row_u_end_x,    int left2col_u_start_y,  int left2col_u_end_y,
    int cur_lcu_u_start_x, int cur_lcu_u_end_x,   int cur_lcu_u_start_y,   int cur_lcu_u_end_y,
    int sq_sao_mode_v,     int up2row_sao_mode_v, int left2col_sao_mode_v, int cur_lcu_sao_mode_v,
    int up2row_v_start_x,  int up2row_v_end_x,    int left2col_v_start_y,  int left2col_v_end_y,
    int cur_lcu_v_start_x, int cur_lcu_v_end_x,   int cur_lcu_v_start_y,   int cur_lcu_v_end_y)
{
    sao_2_pic_rec_luma(max_smb_num, lcu_cnt, pic_width, log2_max_cuwh, sample_adaptive_offset_enable_flag, pix_y_luma, pix_x_luma, rec_stride_luma,
        y_rec, dbkbuffer_y, sao_rslt_array_y,
        lcu_avail_left,    lcu_avail_up,
        sq_sao_mode_y,     up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y,
        up2row_y_start_x,  up2row_y_end_x,    left2col_y_start_y,  left2col_y_end_y,
        cur_lcu_y_start_x, cur_lcu_y_end_x,   cur_lcu_y_start_y,   cur_lcu_y_end_y);
    sao_2_pic_rec_chroma(sample_adaptive_offset_enable_flag, pix_y_luma, pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma,
        u_rec,                v_rec,
        temp_sao_lcu_array_u, temp_sao_lcu_array_v,
        sao_rslt_array_u,     sao_rslt_array_v,
        lcu_avail_left,    lcu_avail_up,
        sq_sao_mode_u,     up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u,
        up2row_u_start_x,  up2row_u_end_x,    left2col_u_start_y,  left2col_u_end_y,
        cur_lcu_u_start_x, cur_lcu_u_end_x,   cur_lcu_u_start_y,   cur_lcu_u_end_y,
        sq_sao_mode_v,     up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v,
        up2row_v_start_x,  up2row_v_end_x,    left2col_v_start_y,  left2col_v_end_y,
        cur_lcu_v_start_x, cur_lcu_v_end_x,   cur_lcu_v_start_y,   cur_lcu_v_end_y);
}

void sao_2_pic_rec(pel* y_rec, pel* u_rec, pel* v_rec, //pel best_rec_y[64][64], pel best_rec_u[32][32], pel    best_rec_v[32][32], 
                pel dbkbuffer_y[68][68],       pel dbkbuffer_u[36][36],          pel dbkbuffer_v[36][36],
                u8 sample_adaptive_offset_enable_flag, int pix_y_luma, int pix_x_luma, int pix_y_chroma, int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma,
                                              pel temp_sao_lcu_array_u[38][38], pel temp_sao_lcu_array_v[38][38],
              //u8 dbk_lcu_array_y[68][68],   u8 dbk_lcu_array_u[36][36],      u8 dbk_lcu_array_v[36][36],
                pel sao_rslt_array_y[70][70], pel sao_rslt_array_u[38][38], pel sao_rslt_array_v[38][38],
                int lcu_avail_left, int lcu_avail_up,
                int sq_sao_mode_y,     int up2row_sao_mode_y, int left2col_sao_mode_y, int cur_lcu_sao_mode_y,
                int up2row_y_start_x,  int up2row_y_end_x,    int left2col_y_start_y,  int left2col_y_end_y,
                int cur_lcu_y_start_x, int cur_lcu_y_end_x,   int cur_lcu_y_start_y,   int cur_lcu_y_end_y,
                int sq_sao_mode_u,     int up2row_sao_mode_u, int left2col_sao_mode_u, int cur_lcu_sao_mode_u,
                int up2row_u_start_x,  int up2row_u_end_x,    int left2col_u_start_y,  int left2col_u_end_y,
                int cur_lcu_u_start_x, int cur_lcu_u_end_x,   int cur_lcu_u_start_y,   int cur_lcu_u_end_y,
                int sq_sao_mode_v,     int up2row_sao_mode_v, int left2col_sao_mode_v, int cur_lcu_sao_mode_v,
                int up2row_v_start_x,  int up2row_v_end_x,    int left2col_v_start_y,  int left2col_v_end_y,
                int cur_lcu_v_start_x, int cur_lcu_v_end_x,   int cur_lcu_v_start_y,   int cur_lcu_v_end_y)
{
    int x, y;

    if (sample_adaptive_offset_enable_flag)
    {
#if 1
// --- Y --------------------------------------------------------------------------------------------------------
        // 1st Step: write with dbk_y.
        for (y = 0; y < 68; y++) {
            for (x = 0; x < 68; x++) {
#pragma HLS PIPELINE
                if ((pix_y_luma + y - 4) * rec_stride_luma + (pix_x_luma + x - 4)>=0) {
                    y_rec[(pix_y_luma + y - 4) * rec_stride_luma + (pix_x_luma + x - 4)] = dbkbuffer_y[y][x];
                }
            }
        }
        // 2nd Step: write with sao_filt_result.
        if (lcu_avail_up && lcu_avail_left) // 2x2
        {
            if (sq_sao_mode_y != ((int)SAO_MODE_OFF))
            {
                if ((pix_y_luma + 1 - 6) * rec_stride_luma + pix_x_luma + 1 - 6 >= 0) {
                    y_rec[(pix_y_luma + 1 - 6) * rec_stride_luma + pix_x_luma + 1 - 6] = sao_rslt_array_y[1][1];
                }
            }
        }
        if (lcu_avail_up) // 2 ROW
        {
            if (up2row_sao_mode_y != ((int)SAO_MODE_OFF)) {
                for (x = up2row_y_start_x; x < up2row_y_end_x; x++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=68
                    if ((pix_y_luma + 1 - 6) * rec_stride_luma + pix_x_luma + x - 6 >= 0) {
                        y_rec[(pix_y_luma + 1 - 6) * rec_stride_luma + pix_x_luma + x - 6] = sao_rslt_array_y[1][x];
                    }
                }
            }
        }
        if (lcu_avail_left) // 2 COLUMN
        {
            if (left2col_sao_mode_y != ((int)SAO_MODE_OFF)) {
                for (y = left2col_y_start_y; y < left2col_y_end_y; y++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=68
                    if ((pix_y_luma + y - 6) * rec_stride_luma + pix_x_luma + 1 - 6 >= 0) {
                        y_rec[(pix_y_luma + y - 6) * rec_stride_luma + pix_x_luma + 1 - 6] = sao_rslt_array_y[y][1];
                    }
                }
            }
        }
        if (cur_lcu_sao_mode_y != ((int)SAO_MODE_OFF)) {
            for (y = cur_lcu_y_start_y; y < cur_lcu_y_end_y; y++) {
#pragma HLS LOOP_TRIPCOUNT max=68
                for (x = cur_lcu_y_start_x; x < cur_lcu_y_end_x; x++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=68
                    if ((pix_y_luma + y - 6) * rec_stride_luma + pix_x_luma + x - 6 >= 0) {
                        y_rec[(pix_y_luma + y - 6) * rec_stride_luma + pix_x_luma + x - 6] = sao_rslt_array_y[y][x];
                    }
                }
            }
        }
          /*  printf("cur_lcu_y_start_y= %d\n", cur_lcu_y_start_y);
            printf("cur_lcu_y_start_x = %d\n", cur_lcu_y_start_x);
            printf("cur_lcu_y_start_y= %d\n", cur_lcu_y_end_y);
            printf("cur_lcu_y_start_x = %d\n", cur_lcu_y_end_x);
            printf("cur_lcu_u_start_y= %d\n", cur_lcu_u_start_y);
            printf("cur_lcu_u_start_x = %d\n", cur_lcu_u_start_x);
            printf("cur_lcu_u_start_y= %d\n", cur_lcu_u_end_y);
            printf("cur_lcu_u_start_x = %d\n", cur_lcu_u_end_x);
            printf("cur_lcu_v_start_y= %d\n", cur_lcu_v_start_y);
            printf("cur_lcu_v_start_x = %d\n", cur_lcu_v_start_x);
            printf("cur_lcu_v_start_y= %d\n", cur_lcu_v_end_y);
            printf("cur_lcu_v_start_x = %d\n", cur_lcu_v_end_x);*/
#endif 
//------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------
// U
        for (int i = 2; i < 38; i++) {
            for (int j = 2; j < 38; j++) {
#pragma HLS PIPELINE
                if ((pix_y_chroma + i - 6) * rec_stride_chroma + (pix_x_chroma + j - 6) >= 0) {
                    //recpic->u[(pix_y_chroma + i - 4) * rec_stride_chroma + (pix_x_chroma + j - 4)] = dbk_lcu_array_u[i][j];
                    u_rec[(pix_y_chroma + i - 6) * rec_stride_chroma + (pix_x_chroma + j - 6)] = temp_sao_lcu_array_u[i][j];
                }
            }
        }

//        if (lcu_avail_up && lcu_avail_left) // 2x2
//        {
//            if (sq_sao_mode_u != ((int)SAO_MODE_OFF))
//            {
//                recpic->u[(pix_y_chroma + 1 - 6) * rec_stride_chroma + pix_x_chroma + 1 - 6] = sao_rslt_array_u[1][1];
//            }
//        }
//        if (lcu_avail_up) // 2 ROW
//        {
//            if (up2row_sao_mode_u != ((int)SAO_MODE_OFF))
//            {
//                for (x = up2row_u_start_x; x < up2row_u_end_x; x++)
//                {
//#pragma HLS PIPELINE
//#pragma HLS LOOP_TRIPCOUNT max=36
//                    recpic->u[(pix_y_chroma + 1 - 6) * rec_stride_chroma + pix_x_chroma + x - 6] = sao_rslt_array_u[1][x];
//                }
//            }
//        }
//        if (lcu_avail_left) // 2 COLUMN
//        {
//            if (left2col_sao_mode_u != ((int)SAO_MODE_OFF))
//            {
//                for (y = left2col_u_start_y; y < left2col_u_end_y; y++)
//                {
//#pragma HLS PIPELINE
//#pragma HLS LOOP_TRIPCOUNT max=36
//                    recpic->u[(pix_y_chroma + y - 6) * rec_stride_chroma + pix_x_chroma + 1 - 6] = sao_rslt_array_u[y][1];
//                }
//
//            }
//        }
        if (cur_lcu_sao_mode_u != ((int)SAO_MODE_OFF)) {
            for (y = cur_lcu_u_start_y; y < cur_lcu_u_end_y; y++) {
#pragma HLS LOOP_TRIPCOUNT max=36
                for (x = cur_lcu_u_start_x; x < cur_lcu_u_end_x; x++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=36
                    if ((pix_y_chroma + y - 6) * rec_stride_chroma + pix_x_chroma + x - 6 >= 0) {
                        u_rec[(pix_y_chroma + y - 6) * rec_stride_chroma + pix_x_chroma + x - 6] = sao_rslt_array_u[y][x];
                    }
                }
            }
        }
//-----------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------
// V
        for (int i = 0; i < 36; i++) {
            for (int j = 0; j < 36; j++) {
#pragma HLS PIPELINE
                if ((pix_y_chroma + i - 4) * rec_stride_chroma + (pix_x_chroma + j - 4) >= 0) {
                    //recpic->v[(pix_y_chroma + i - 4) * rec_stride_chroma + (pix_x_chroma + j - 4)] = dbk_lcu_array_v[i][j];
                    v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + (pix_x_chroma + j - 4)] = temp_sao_lcu_array_v[i + 2][j + 2];
                }
            }
        }

//        if (lcu_avail_up && lcu_avail_left) // 2x2
//        {
//            if (sq_sao_mode_v != ((int)SAO_MODE_OFF))
//            {
//                recpic->v[(pix_y_chroma + 1 - 6) * rec_stride_chroma + pix_x_chroma + 1 - 6] = sao_rslt_array_v[1][1];
//            }
//        }
//        if (lcu_avail_up) // 2 ROW
//        {
//            if (up2row_sao_mode_v != ((int)SAO_MODE_OFF))
//            {
//                for (x = up2row_v_start_x; x < up2row_v_end_x; x++)
//                {
//#pragma HLS PIPELINE
//#pragma HLS LOOP_TRIPCOUNT max=36
//                    recpic->v[(pix_y_chroma + 1 - 6) * rec_stride_chroma + pix_x_chroma + x - 6] = sao_rslt_array_v[1][x];
//                }
//            }
//        }
//        if (lcu_avail_left) // 2 COLUMN
//        {
//            if (left2col_sao_mode_v != ((int)SAO_MODE_OFF))
//            {
//                for (y = left2col_v_start_y; y < left2col_v_end_y; y++)
//                {
//#pragma HLS PIPELINE
//#pragma HLS LOOP_TRIPCOUNT max=36
//                    recpic->v[(pix_y_chroma + y - 6) * rec_stride_chroma + pix_x_chroma + 1 - 6] = sao_rslt_array_v[y][1];
//                }
//
//            }
//        }

        if (cur_lcu_sao_mode_v != ((int)SAO_MODE_OFF)) {
            for (y = cur_lcu_v_start_y; y < cur_lcu_v_end_y; y++) {
#pragma HLS LOOP_TRIPCOUNT max=36
                for (x = cur_lcu_v_start_x; x < cur_lcu_v_end_x; x++) {
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=36
                    if ((pix_y_chroma + y - 6) * rec_stride_chroma + pix_x_chroma + x - 6 >= 0) {
                        v_rec[(pix_y_chroma + y - 6) * rec_stride_chroma + pix_x_chroma + x - 6] = sao_rslt_array_v[y][x];
                    }
                }
            }
        }
    }
//-----------------------------------------------------------------------------------------------------------------------------------
    else { // (!sample_adaptive_offset_enable_flag)
#if 1
        for (y = 0; y < 64; y++) {
            for (x = 0; x < 64; x++) {
#pragma HLS PIPELINE
                if ((pix_y_luma + y - 4) * rec_stride_luma + pix_x_luma + x - 4 >= 0) {
                    //recpic->y[(pix_y_luma + y) * rec_stride_luma + pix_x_luma + x] = best_rec_y[y][x];
                    y_rec[(pix_y_luma + y - 4) * rec_stride_luma + pix_x_luma + x - 4] = dbkbuffer_y[y][x];
                }
            }
        }
#endif 
        for (y = 0; y < 32; y++) {
            for (x = 0; x < 32; x++) {
#pragma HLS PIPELINE
                if (((pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4 >= 0)&&((pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4)) {
                    //recpic->u[(pix_y_chroma + y) * rec_stride_chroma + pix_x_chroma + x] = best_rec_u[y][x];
                    //recpic->v[(pix_y_chroma + y) * rec_stride_chroma + pix_x_chroma + x] = best_rec_v[y][x];
                    u_rec[(pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4] = dbkbuffer_u[y][x];
                    v_rec[(pix_y_chroma + y - 4) * rec_stride_chroma + pix_x_chroma + x - 4] = dbkbuffer_v[y][x];
                }
            }
        }
    }
}

#if 1
//目前该函数ctx_map_mv ctx_map_refi地址未做32bit对齐，故hls综合处理的cycle数比较大(15367)；将ctx_map_mv ctx_map_refi的指针
//类型修改为u32*之后，cycle数会大幅度降低(2000多一点)
void update_ctx_map_frame_level(u32* ctx_map_scu, s32* ctx_map_mv, s32* ctx_map_refi, u32* ctx_map_cu_mode, 
	U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height, U10 dbk_out_para_set_pic_width_in_scu,
	U11 dbk_out_para_set_min_scu_x, U11 dbk_out_para_set_min_scu_y, U32 map_cu_mode_inner_to_store[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U1 dbk_out_para_set_curr_ctu_idx, U32 dbk_out_para_set_map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	int dbk_out_para_set_x_pel, int dbk_out_para_set_y_pel, ENC_ME_NEB_INFO inner_nebs2sao[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{

    int lcuw=64; 
    int lcuh=64;

//Output of update_ctx_map_frame_level:
#pragma HLS INTERFACE m_axi port=ctx_map_scu        offset=slave bundle=m_axi_map depth=6400
#pragma HLS INTERFACE m_axi port=ctx_map_mv         offset=slave bundle=m_axi_map depth=25600
#pragma HLS INTERFACE m_axi port=ctx_map_refi       offset=slave bundle=m_axi_map depth=12800
#pragma HLS INTERFACE m_axi port=ctx_map_cu_mode    offset=slave bundle=m_axi_map depth=6400

#pragma HLS RESOURCE variable=dbk_out_para_set_map_scu_inner core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=map_cu_mode_inner_to_store core=RAM_1P_BRAM

#pragma HLS array_partition variable=dbk_out_para_set_map_scu_inner complete dim=1
#pragma HLS array_partition variable=map_cu_mode_inner_to_store complete dim=1

#pragma HLS RESOURCE variable=inner_nebs2sao core=RAM_1P_BRAM
#pragma HLS array_partition variable=inner_nebs2sao complete dim=3

#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_width
#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_height
#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_width_in_scu
#pragma HLS INTERFACE axis port=dbk_out_para_set_min_scu_x
#pragma HLS INTERFACE axis port=dbk_out_para_set_min_scu_y
#pragma HLS INTERFACE axis port=dbk_out_para_set_curr_ctu_idx
#pragma HLS INTERFACE axis port=dbk_out_para_set_x_pel
#pragma HLS INTERFACE axis port=dbk_out_para_set_y_pel

	int src_cuw = lcuw;
	int src_cuh = lcuh;

	int size_depth;
	int w;
	int h;
	int size;
	int size_ipm;
	int size_mv;
	int size_refi;
	int log2_src_cuw;
	int log2_src_cuh;
	int scu_x;
	int scu_y;
	u32* map_cu_mode = (u32*)0;
	u32* src_map_cu_mode = (u32*)0;
	scu_x = dbk_out_para_set_x_pel >> 2;
	scu_y = dbk_out_para_set_y_pel >> 2;
	log2_src_cuw = (com_tbl_log2[src_cuw]);
	log2_src_cuh = (com_tbl_log2[src_cuh]);

	int x_scu_line = scu_x - dbk_out_para_set_min_scu_x;
	int y_scu_line = scu_y - dbk_out_para_set_min_scu_y;

	if (dbk_out_para_set_x_pel + src_cuw > dbk_out_para_set_pic_width) {
		w = (dbk_out_para_set_pic_width - dbk_out_para_set_x_pel) >> 2;
	} else {
		w = src_cuw >> 2;
	}
	if (dbk_out_para_set_y_pel + src_cuh > dbk_out_para_set_pic_height) {
		h = (dbk_out_para_set_pic_height - dbk_out_para_set_y_pel) >> 2;
	} else {
		h = src_cuh >> 2;
	}

	int  dt_frame_scu = dbk_out_para_set_pic_width_in_scu;
#if 0
	UPDATE_CTX_MAP_SCU: for (int i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
        for (int j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
            ctx_map_scu[scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu] = dbk_out_para_set_map_scu_inner[dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i];
        }
    }
	UPDATE_CTX_MAP_CU_MODE: for (int i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
        for (int j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
            ctx_map_cu_mode[scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu] = dbk_out_para_set_map_cu_mode_inner[dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i];
        }
    }
	UPDATE_CTX_MAP_REFI: for (int i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
        for (int j = 0; j < w/2; j++) {
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS PIPELINE
            U8 temp_0 = inner_nebs[dbk_out_para_set_curr_ctu_idx][x_scu_line + 2*j    ][y_scu_line + i].refi[0];
            U8 temp_1 = inner_nebs[dbk_out_para_set_curr_ctu_idx][x_scu_line + 2*j    ][y_scu_line + i].refi[1];
            U8 temp_2 = inner_nebs[dbk_out_para_set_curr_ctu_idx][x_scu_line + 2*j + 1][y_scu_line + i].refi[0];
            U8 temp_3 = inner_nebs[dbk_out_para_set_curr_ctu_idx][x_scu_line + 2*j + 1][y_scu_line + i].refi[1];
            U32 temp = (temp_3 << 24) || (temp_2 << 16) || (temp_1 << 8) || temp_0;
            ctx_map_refi[(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + i * dt_frame_scu) * 2 + j] = temp;
        }
    }
	UPDATE_CTX_MAP_MV: for (int i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
        for (int k = 0; k < w*2; k++) {
#pragma HLS LOOP_TRIPCOUNT max=32
#pragma HLS PIPELINE
            /*ap_uint<16>*/U16 temp_0 = inner_nebs[dbk_out_para_set_curr_ctu_idx][x_scu_line + (k>>1)][y_scu_line + i].mv[(k%4) >> 1][0];
            /*ap_uint<16>*/U16 temp_1 = inner_nebs[dbk_out_para_set_curr_ctu_idx][x_scu_line + (k>>1)][y_scu_line + i].mv[(k%4) >> 1][1];
            //U32 temp = (temp_1, temp_0);
            U32 temp = (temp_1 << 16) || temp_0; 
            ctx_map_mv[(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + i * dt_frame_scu) * 4 + k] = temp;
        }
    }
#else
	for (int i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=16
		for (int j = 0; j < w; j++) {
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS PIPELINE
			ctx_map_scu    [scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu]           = dbk_out_para_set_map_scu_inner    [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i];
			ctx_map_cu_mode[scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu]           = map_cu_mode_inner_to_store[dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i];
			ctx_map_refi   [(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu) * 2]     = inner_nebs2sao                        [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i].refi[0];
			ctx_map_refi   [(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu) * 2 + 1] = inner_nebs2sao                        [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i].refi[1];
			ctx_map_mv     [(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu) * 4]     = inner_nebs2sao                        [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i].mv[0][0];
			ctx_map_mv     [(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu) * 4 + 1] = inner_nebs2sao                        [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i].mv[0][1];
			ctx_map_mv     [(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu) * 4 + 2] = inner_nebs2sao                        [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i].mv[1][0];
			ctx_map_mv     [(scu_y * dbk_out_para_set_pic_width_in_scu + scu_x + j + i * dt_frame_scu) * 4 + 3] = inner_nebs2sao                        [dbk_out_para_set_curr_ctu_idx][x_scu_line + j][y_scu_line + i].mv[1][1];
			/*	map_pb_tb_part[j + i * dt_frame_scu] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_line + j][y_scu_line + i].pb_tb_part; */
		}
	}
#endif
}
#endif

	void sao_bypass(SAO2AEC* AEC_DATA_input, FETCH2SAO_FW fetch2sao_fw,
		U12 dbk_out_para_set_x_pel, U12 dbk_out_para_set_y_pel,
		U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height,
		U6 dbk_out_para_set_pic_width_in_lcu) 
{
		U12 x_pel = dbk_out_para_set_x_pel;
		U12 y_pel = dbk_out_para_set_y_pel;
		U7 max_cuwh = fetch2sao_fw.max_cuwh;
		U3 log2_max_cuwh = fetch2sao_fw.log2_max_cuwh;
		U13 pic_width = dbk_out_para_set_pic_width;
		u8 sample_adaptive_offset_enable_flag = fetch2sao_fw.sample_adaptive_offset_enable_flag;

#if ENABLE_BFRAME
        AEC_DATA_input->rpm_num_refp[0] = fetch2sao_fw.num_refp[0];
        AEC_DATA_input->rpm_num_refp[1] = fetch2sao_fw.num_refp[1];
#else
        AEC_DATA_input->rpm_num_refp[0] = fetch2sao_fw.num_refp;
        AEC_DATA_input->rpm_num_refp[1] = 0;
#endif
		
		AEC_DATA_input->slice_sao_enable[0] = 1;
		AEC_DATA_input->slice_sao_enable[1] = 1;
		AEC_DATA_input->slice_sao_enable[2] = 1;
		AEC_DATA_input->x0 = x_pel;
		AEC_DATA_input->y0 = y_pel;
		AEC_DATA_input->cup = 0;
		AEC_DATA_input->cu_width = fetch2sao_fw.cu_width;
		AEC_DATA_input->cu_height = fetch2sao_fw.cu_height;
        
		AEC_DATA_input->cud = 0;
		AEC_DATA_input->parent_split = 0;
		AEC_DATA_input->qt_depth = 0;
		AEC_DATA_input->bet_depth = 0;
		AEC_DATA_input->cons_pred_mode = 0;
		AEC_DATA_input->tree_status = 0;
		AEC_DATA_input->log2_culine = fetch2sao_fw.log2_culine;
		AEC_DATA_input->pic_header_slice_type = fetch2sao_fw.slice_type;
		AEC_DATA_input->info_max_cuwh = max_cuwh;
		AEC_DATA_input->info_pic_width = pic_width;
		AEC_DATA_input->info_pic_height = dbk_out_para_set_pic_height;
		AEC_DATA_input->sample_adaptive_offset_enable_flag = sample_adaptive_offset_enable_flag;
		AEC_DATA_input->lcu_cnt = fetch2sao_fw.lcu_cnt + 1;
        AEC_DATA_input->lcu_num = fetch2sao_fw.lcu_num;
       
		/*AEC_DATA_input->lcu_num = (x_pel >> log2_max_cuwh) + (y_pel >> log2_max_cuwh) * (pic_width >> log2_max_cuwh);*/
		AEC_DATA_input->sqh_umve_enable_flag = 1;
		AEC_DATA_input->sqh_num_of_hmvp_cand = fetch2sao_fw.num_of_hmvp_cand;
		AEC_DATA_input->sqh_affine_enable_flag = 1;
		AEC_DATA_input->sqh_amvr_enable_flag = fetch2sao_fw.amvr_enable_flag;
		AEC_DATA_input->sqh_emvr_enable_flag = fetch2sao_fw.emvr_enable_flag;
		AEC_DATA_input->sqh_dt_intra_enable_flag = fetch2sao_fw.dt_intra_enable_flag;
		AEC_DATA_input->sqh_position_based_transform_enable_flag = fetch2sao_fw.position_based_transform_enable_flag;
		AEC_DATA_input->sqh_tscpm_enable_flag = fetch2sao_fw.tscpm_enable_flag;
		AEC_DATA_input->sqh_ipf_enable_flag = fetch2sao_fw.ipf_enable_flag;
		AEC_DATA_input->sqh_sample_adaptive_offset_enable_flag = sample_adaptive_offset_enable_flag;
		AEC_DATA_input->info_pic_width_in_scu = fetch2sao_fw.pic_width_in_scu;
		AEC_DATA_input->info_pic_width_in_lcu = dbk_out_para_set_pic_width_in_lcu;
		AEC_DATA_input->x_lcu = x_pel >> log2_max_cuwh;
		AEC_DATA_input->y_lcu = y_pel >> log2_max_cuwh;
		AEC_DATA_input->x_pel = x_pel;
		AEC_DATA_input->y_pel = y_pel;
		AEC_DATA_input->ptr = fetch2sao_fw.cur_ptr;
		AEC_DATA_input->MergeLeftAvail = (x_pel >> 2) == 0 ? 0 : 1;
		AEC_DATA_input->MergeUpAvail = (y_pel >> 2) == 0 ? 0 : 1;
	}

//buffer for RFC 
//luma
pel saoBuffer64x64[64][64];
pel saoBuffer8x4096[8][4096];
//chroma
pel saoBuffer32x32_u[32][32];
pel saoBuffer8x2048_u[8][2048];
pel saoBuffer32x32_v[32][32];
pel saoBuffer8x2048_v[8][2048];

#if 0//原有代码部分：sao_run、sao_output_rearrange、rfc_run
//only to adapt the function ports
//luma
pel recBuffer[192][192];
//chroma
pel recBuffer_u[192][192];
pel recBuffer_v[192][192];
#endif//原有代码部分：sao_run、sao_output_rearrange、rfc_run

#ifndef __SYNTHESIS__
//luma
pel rfcBinBufferFra[4096][4096];
prm_t rfcPrmBufferFra[1024][128];
//chroma
pel rfcBinBufferFra_u[4096][4096];
prm_t rfcPrmBufferFra_u[1024][128];
pel rfcBinBufferFra_v[4096][4096];
prm_t rfcPrmBufferFra_v[1024][128];
#endif

#if 0//原有代码部分：sao_run、sao_output_rearrange、rfc_run
void sao_run(u32* ctx_map_scu, s16* ctx_map_mv, s8* ctx_map_refi,u32* ctx_map_cu_mode,
	U11 dbk_out_para_set_min_scu_x, U11 dbk_out_para_set_min_scu_y, U32 me_line_map_ptr_map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U1 dbk_out_para_set_curr_ctu_idx, U32 dbk_out_para_set_map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO inner_nebs2sao[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height, U6 dbk_out_para_set_pic_width_in_lcu, U6 dbk_out_para_set_pic_height_in_lcu,
	U12 dbk_out_para_set_x_pel, U12 dbk_out_para_set_y_pel, pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39], SAOBlkParam cur_saoParam[3],
	pel Orig_Lcu_ptr_y_org_sao[64][64], pel Orig_Lcu_ptr_u_org_sao[32][32], pel Orig_Lcu_ptr_v_org_sao[32][32], 
    pel* y_rec, pel* u_rec, pel* v_rec,
    //U8* y_rfcBin, U16* y_rfcPrm, U8* u_rfcBin, U16* u_rfcPrm, U8* v_rfcBin, U16* v_rfcPrm,
    FETCH2SAO_FW *fetch2sao_fw_ptr, SAO2AEC* AEC_DATA_input, U32 qp_offset,
    pel sao_pic_rec_test_y[69][69], pel sao_pic_rec_test_u[36][36], pel sao_pic_rec_test_v[36][36])
{
#pragma HLS inline off

#ifndef __SYNTHESIS__
#if COLLECT_SAO_DATA
    if (sao_cu_cnt == 0) {
        sao_input_dbk_out_para_set_min_scu_x = open_file(SAO_INPUT_dbk_out_para_set_min_scu_x, "wb");
        sao_input_dbk_out_para_set_min_scu_y = open_file(SAO_INPUT_dbk_out_para_set_min_scu_y, "wb");
        sao_input_me_line_map_ptr_map_cu_mode_inner = open_file(SAO_INPUT_me_line_map_ptr_map_cu_mode_inner, "wb");
        sao_input_dbk_out_para_set_curr_ctu_idx = open_file(SAO_INPUT_dbk_out_para_set_curr_ctu_idx, "wb");
        sao_input_dbk_out_para_set_map_scu_inner = open_file(SAO_INPUT_dbk_out_para_set_map_scu_inner, "wb");
        sao_input_inner_nebs2sao = open_file(SAO_INPUT_inner_nebs2sao, "wb");
        sao_input_dbk_out_para_set_pic_width = open_file(SAO_INPUT_dbk_out_para_set_pic_width, "wb");
        sao_input_dbk_out_para_set_pic_height = open_file(SAO_INPUT_dbk_out_para_set_pic_height, "wb");
        sao_input_dbk_out_para_set_pic_width_in_lcu = open_file(SAO_INPUT_dbk_out_para_set_pic_width_in_lcu, "wb");
        sao_input_dbk_out_para_set_pic_height_in_lcu = open_file(SAO_INPUT_dbk_out_para_set_pic_height_in_lcu, "wb");
        sao_input_dbk_out_para_set_x_pel = open_file(SAO_INPUT_dbk_out_para_set_x_pel, "wb");
        sao_input_dbk_out_para_set_y_pel = open_file(SAO_INPUT_dbk_out_para_set_y_pel, "wb");
        sao_input_dbkout_y = open_file(SAO_INPUT_dbkout_y, "wb");
        sao_input_dbkout_u = open_file(SAO_INPUT_dbkout_u, "wb");
        sao_input_dbkout_v = open_file(SAO_INPUT_dbkout_v, "wb");
        sao_input_Orig_Lcu_ptr_y_org_sao = open_file(SAO_INPUT_Orig_Lcu_ptr_y_org_sao, "wb");
        sao_input_Orig_Lcu_ptr_u_org_sao = open_file(SAO_INPUT_Orig_Lcu_ptr_u_org_sao, "wb");
        sao_input_Orig_Lcu_ptr_v_org_sao = open_file(SAO_INPUT_Orig_Lcu_ptr_v_org_sao, "wb");
        sao_input_fetch2sao_fw_ptr = open_file(SAO_INPUT_fetch2sao_fw_ptr, "wb");
        sao_input_qp_offset = open_file(SAO_INPUT_qp_offset, "wb");

        sao_output_cur_saoParam = open_file(SAO_OUTPUT_cur_saoParam, "wb");
        sao_output_ctx_map_scu = open_file(SAO_OUTPUT_ctx_map_scu, "wb");
        sao_output_ctx_map_mv = open_file(SAO_OUTPUT_ctx_map_mv, "wb");
        sao_output_ctx_map_refi = open_file(SAO_OUTPUT_ctx_map_refi, "wb");
        sao_output_ctx_map_cu_mode = open_file(SAO_OUTPUT_ctx_map_cu_mode, "wb");
        sao_output_y_rec = open_file(SAO_OUTPUT_y_rec, "wb");
        sao_output_u_rec = open_file(SAO_OUTPUT_u_rec, "wb");
        sao_output_v_rec = open_file(SAO_OUTPUT_v_rec, "wb");
        sao_output_AEC_DATA_input = open_file(SAO_OUTPUT_AEC_DATA_input, "wb");
    }
    if (sao_cu_cnt < SAO_CU_NUM_TV) {
        sao_save_indata(ctx_map_scu, ctx_map_mv, ctx_map_refi, ctx_map_cu_mode,
            &dbk_out_para_set_min_scu_x, &dbk_out_para_set_min_scu_y, me_line_map_ptr_map_cu_mode_inner,
            &dbk_out_para_set_curr_ctu_idx, dbk_out_para_set_map_scu_inner,
            inner_nebs2sao,
            &dbk_out_para_set_pic_width, &dbk_out_para_set_pic_height, &dbk_out_para_set_pic_width_in_lcu, &dbk_out_para_set_pic_height_in_lcu,
            &dbk_out_para_set_x_pel, &dbk_out_para_set_y_pel, dbkout_y, dbkout_u, dbkout_v, 
            Orig_Lcu_ptr_y_org_sao, Orig_Lcu_ptr_u_org_sao, Orig_Lcu_ptr_v_org_sao, y_rec, u_rec, v_rec, fetch2sao_fw_ptr, AEC_DATA_input, &qp_offset);
        printf("SAO: LCU %d  INPUT DATA SAVED. \n", sao_cu_cnt);
    }
#endif
#endif //__SYNTHESIS__

// Output of update_ctx_map_frame_level:
#pragma HLS INTERFACE m_axi port=ctx_map_scu        offset=slave bundle=m_axi_map depth=6240
#pragma HLS INTERFACE m_axi port=ctx_map_mv         offset=slave bundle=m_axi_map depth=24960
#pragma HLS INTERFACE m_axi port=ctx_map_refi       offset=slave bundle=m_axi_map depth=12480
#pragma HLS INTERFACE m_axi port=ctx_map_cu_mode    offset=slave bundle=m_axi_map depth=6240
//#pragma HLS RESOURCE variable=ctx_map_cu_mode core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ctx_map_scu core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ctx_map_mv core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ctx_map_refi core=RAM_1P_BRAM
//-----------------------------------------------------------------------------
#pragma HLS INTERFACE m_axi port=y_rec  offset=slave bundle=y_rec depth=270192
#pragma HLS INTERFACE m_axi port=u_rec  offset=slave bundle=u_rec depth=67512
#pragma HLS INTERFACE m_axi port=v_rec  offset=slave bundle=v_rec depth=67512
//#pragma HLS RESOURCE variable=y_rec core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=u_rec core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=v_rec core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=Orig_Lcu_ptr_y_org_sao core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Orig_Lcu_ptr_u_org_sao core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=Orig_Lcu_ptr_v_org_sao core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_v core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fetch2sao_fw_ptr->lambda core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->stride_luma
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->stride_chroma
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->lcu_cnt
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pix_x
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pix_y
#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_width
#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_height
#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_width_in_lcu
#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_height_in_lcu
#pragma HLS INTERFACE axis port=dbk_out_para_set_x_pel
#pragma HLS INTERFACE axis port=dbk_out_para_set_y_pel
#pragma HLS RESOURCE variable=dbk_out_para_set_map_scu_inner core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_map_cu_mode_inner core=RAM_1P_BRAM
#pragma HLS array_partition variable=me_line_map_ptr_map_cu_mode_inner complete dim=2
#pragma HLS array_partition variable=me_line_map_ptr_map_cu_mode_inner complete dim=3
#pragma HLS INTERFACE axis port=dbk_out_para_set_min_scu_x
#pragma HLS INTERFACE axis port=dbk_out_para_set_min_scu_y
#pragma HLS INTERFACE axis port=dbk_out_para_set_curr_ctu_idx
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width_in_scu
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_height

#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->max_cuwh
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->log2_max_cuwh
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->num_refp
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->log2_culine
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->slice_type
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->num_of_hmvp_cand
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->affine_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->amvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->emvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->dt_intra_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->tscpm_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->ipf_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cur_ptr
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cu_height
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cu_width
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->lcu_num
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->bit_depth

#pragma HLS RESOURCE variable=inner_nebs2sao core=RAM_1P_BRAM
#pragma HLS array_partition variable=inner_nebs2sao complete dim=3
#pragma HLS RESOURCE variable=cur_saoParam core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=AEC_DATA_input->rpm_num_refp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=AEC_DATA_input->slice_sao_enable core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=AEC_DATA_input->x0
#pragma HLS INTERFACE axis port=AEC_DATA_input->y0
#pragma HLS INTERFACE axis port=AEC_DATA_input->cup
#pragma HLS INTERFACE axis port=AEC_DATA_input->cu_width
#pragma HLS INTERFACE axis port=AEC_DATA_input->cu_height
#pragma HLS INTERFACE axis port=AEC_DATA_input->cud
#pragma HLS INTERFACE axis port=AEC_DATA_input->parent_split
#pragma HLS INTERFACE axis port=AEC_DATA_input->qt_depth
#pragma HLS INTERFACE axis port=AEC_DATA_input->bet_depth
#pragma HLS INTERFACE axis port=AEC_DATA_input->cons_pred_mode
#pragma HLS INTERFACE axis port=AEC_DATA_input->tree_status
#pragma HLS INTERFACE axis port=AEC_DATA_input->log2_culine
#pragma HLS INTERFACE axis port=AEC_DATA_input->pic_header_slice_type
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_max_cuwh
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_width
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_height
#pragma HLS INTERFACE axis port=AEC_DATA_input->sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->lcu_cnt
#pragma HLS INTERFACE axis port=AEC_DATA_input->lcu_num
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_umve_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_num_of_hmvp_cand
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_affine_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_amvr_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_emvr_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_dt_intra_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_tscpm_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_ipf_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_width_in_scu
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_width_in_lcu
#pragma HLS INTERFACE axis port=AEC_DATA_input->x_lcu
#pragma HLS INTERFACE axis port=AEC_DATA_input->y_lcu
#pragma HLS INTERFACE axis port=AEC_DATA_input->x_pel
#pragma HLS INTERFACE axis port=AEC_DATA_input->y_pel
#pragma HLS INTERFACE axis port=AEC_DATA_input->ptr
#pragma HLS INTERFACE axis port=AEC_DATA_input->MergeLeftAvail
#pragma HLS INTERFACE axis port=AEC_DATA_input->MergeUpAvail
#pragma HLS INTERFACE axis port=qp_offset

	static SAOBlkParam rec_saoParam[60 + 2][3];//for printf
    static FETCH2SAO_FW fetch2sao_fw;
	fetch2sao_fw.pic_width = fetch2sao_fw_ptr->pic_width;
	fetch2sao_fw.pic_height = fetch2sao_fw_ptr->pic_height;
	fetch2sao_fw.pic_width_in_scu = fetch2sao_fw_ptr->pic_width_in_scu;
	fetch2sao_fw.sample_adaptive_offset_enable_flag =
	fetch2sao_fw_ptr->sample_adaptive_offset_enable_flag;
	fetch2sao_fw.lambda[0] = fetch2sao_fw_ptr->lambda[0];
	fetch2sao_fw.lambda[1] = fetch2sao_fw_ptr->lambda[1];
	fetch2sao_fw.lambda[2] = fetch2sao_fw_ptr->lambda[2];
	fetch2sao_fw.stride_luma = fetch2sao_fw_ptr->stride_luma;
	fetch2sao_fw.stride_chroma = fetch2sao_fw_ptr->stride_chroma;
	fetch2sao_fw.lcu_cnt  = fetch2sao_fw_ptr->lcu_cnt;
	fetch2sao_fw.max_cuwh = fetch2sao_fw_ptr->max_cuwh;
	fetch2sao_fw.log2_max_cuwh = fetch2sao_fw_ptr->log2_max_cuwh;
#if ENABLE_BFRAME
    fetch2sao_fw.num_refp[0] = fetch2sao_fw_ptr->num_refp[0];
    fetch2sao_fw.num_refp[1] = fetch2sao_fw_ptr->num_refp[1];
#else
    fetch2sao_fw.num_refp = fetch2sao_fw_ptr->num_refp;
#endif
	fetch2sao_fw.log2_culine = fetch2sao_fw_ptr->log2_culine;
	fetch2sao_fw.slice_type = fetch2sao_fw_ptr->slice_type;
	fetch2sao_fw.num_of_hmvp_cand = fetch2sao_fw_ptr->num_of_hmvp_cand;
	fetch2sao_fw.affine_enable_flag = 1;
	fetch2sao_fw.amvr_enable_flag = fetch2sao_fw_ptr->amvr_enable_flag;
	fetch2sao_fw.emvr_enable_flag = fetch2sao_fw_ptr->emvr_enable_flag;
	fetch2sao_fw.dt_intra_enable_flag = fetch2sao_fw_ptr->dt_intra_enable_flag;
	fetch2sao_fw.position_based_transform_enable_flag =
	fetch2sao_fw_ptr->position_based_transform_enable_flag;
	fetch2sao_fw.tscpm_enable_flag = fetch2sao_fw_ptr->tscpm_enable_flag;
	fetch2sao_fw.ipf_enable_flag = fetch2sao_fw_ptr->ipf_enable_flag;
	fetch2sao_fw.cur_ptr = fetch2sao_fw_ptr->cur_ptr;
	fetch2sao_fw.pix_x = fetch2sao_fw_ptr->pix_x;
	fetch2sao_fw.pix_y = fetch2sao_fw_ptr->pix_y;
    fetch2sao_fw.cu_width = fetch2sao_fw_ptr->cu_width;
    fetch2sao_fw.cu_height = fetch2sao_fw_ptr->cu_height;
    fetch2sao_fw.lcu_num = fetch2sao_fw_ptr->lcu_num;
	//copy dbk_out_para_set
	U13 dbk_out_para_set_pic_width_copy = dbk_out_para_set_pic_width;
	U13 dbk_out_para_set_pic_height_copy = dbk_out_para_set_pic_height;
	U11 dbk_out_para_set_min_scu_x_copy = dbk_out_para_set_min_scu_x;
	U11 dbk_out_para_set_min_scu_y_copy = dbk_out_para_set_min_scu_y;
	U6  dbk_out_para_set_pic_width_in_lcu_copy  = dbk_out_para_set_pic_width_in_lcu;
	U6  dbk_out_para_set_pic_height_in_lcu_copy = dbk_out_para_set_pic_height_in_lcu;
	U12 dbk_out_para_set_y_pel_copy = dbk_out_para_set_y_pel;
	U12 dbk_out_para_set_x_pel_copy = dbk_out_para_set_x_pel;
	U1  dbk_out_para_set_curr_ctu_idx_copy = dbk_out_para_set_curr_ctu_idx;
    int pix_y_luma, pix_x_luma;
    int pix_y_chroma, pix_x_chroma;
    int rec_stride_luma, rec_stride_chroma;

    int pic_width_chroma  = fetch2sao_fw.pic_width  / 2;
    int pic_height_chroma = fetch2sao_fw.pic_height / 2;
    static pel temp_sao_lcu_array_y[70][70];
    static pel temp_sao_lcu_array_y_dupli1[70][70];
    static pel temp_sao_lcu_array_u[38][38];
    static pel temp_sao_lcu_array_v[38][38];
#pragma HLS array_partition variable=temp_sao_lcu_array_y complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_y_dupli1 complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_u complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_v complete dim=2
    //static u8 dbk_lcu_array_y[68][68];
    //static u8 dbk_lcu_array_u[36][36];
    //static u8 dbk_lcu_array_v[36][36];

    static pel sao_rslt_array_y[70][70];
    static pel sao_rslt_array_u[38][38];
    static pel sao_rslt_array_v[38][38];
#pragma HLS array_partition variable=sao_rslt_array_y complete dim=2
#pragma HLS array_partition variable=sao_rslt_array_u complete dim=2
#pragma HLS array_partition variable=sao_rslt_array_v complete dim=2

    int lcu_avail_left, lcu_avail_right, lcu_avail_up, lcu_avail_down;
    //int lcu_avail_up_left, lcu_avail_up_right, lcu_avail_down_left, lcu_avail_down_right;

    int sq_sao_mode_y, up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y;
    int up2row_y_start_x,  up2row_y_end_x,  left2col_y_start_y, left2col_y_end_y;
    int cur_lcu_y_start_x, cur_lcu_y_end_x, cur_lcu_y_start_y,  cur_lcu_y_end_y;

    int sq_sao_mode_u, up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u;
    int up2row_u_start_x,  up2row_u_end_x,  left2col_u_start_y, left2col_u_end_y;
    int cur_lcu_u_start_x, cur_lcu_u_end_x, cur_lcu_u_start_y,  cur_lcu_u_end_y;

    int sq_sao_mode_v, up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v;
    int up2row_v_start_x,  up2row_v_end_x,  left2col_v_start_y, left2col_v_end_y;
    int cur_lcu_v_start_x, cur_lcu_v_end_x, cur_lcu_v_start_y,  cur_lcu_v_end_y;
	u8  slice_sao_enable[3];
	slice_sao_enable[0] = 1;
	slice_sao_enable[1] = 1;
	slice_sao_enable[2] = 1;
    u8  sao_enable_flag = fetch2sao_fw.sample_adaptive_offset_enable_flag;

    pix_y_luma = dbk_out_para_set_y_pel_copy;
    pix_x_luma = dbk_out_para_set_x_pel_copy;
    pix_y_chroma = pix_y_luma >> 1;
    pix_x_chroma = pix_x_luma >> 1;
    rec_stride_luma   = fetch2sao_fw.stride_luma;
    rec_stride_chroma = fetch2sao_fw.stride_chroma;
	int max_smb_num = dbk_out_para_set_pic_width_in_lcu_copy* dbk_out_para_set_pic_height_in_lcu_copy;
//
//#if defined SAO_TV_ACTIVE
//    sao_test_module.writeParameter2File(&lcu_cnt, sizeof(int), 1);  // LCU count
//    sao_test_module.writeParameter2File(&x_pel,   sizeof(int), 1);  // LCU coordinate
//    sao_test_module.writeParameter2File(&y_pel,   sizeof(int), 1);  // LCU coordinate
//
//    sao_test_module.writeParameter2File(y_org, (sizeof(u8) * 64 * 64), 1);       // Fetch output
//    sao_test_module.writeParameter2File(u_org, (sizeof(u8) * 32 * 32), 1);       // Fetch output
//    sao_test_module.writeParameter2File(v_org, (sizeof(u8) * 32 * 32), 1);       // Fetch output
//
//    sao_test_module.writeParameter2File(dbkbuffer_y, (sizeof(u8) * 68 * 68), 1);       // DBK output
//    sao_test_module.writeParameter2File(dbkbuffer_u, (sizeof(u8) * 36 * 36), 1);       // DBK output
//    sao_test_module.writeParameter2File(dbkbuffer_v, (sizeof(u8) * 36 * 36), 1);       // DBK output
//#endif//SAO_TV_ACTIVE
	int log2_max_cuwh = 6;
	int bit_depth_internal = fetch2sao_fw_ptr->bit_depth;
	int qp_offset_bit_depth = qp_offset;
    //printf("qp_offset_bit_depth: %d", qp_offset_bit_depth);
    static pel dbkout_temp_y1[68][68];
    //static pel dbkout_temp_y2[68][68];
    //static pel dbkout_temp_y3[68][68];
    //static pel dbkout_temp_y4[68][68];
    static pel dbkout_temp_u[36][36];
    static pel dbkout_temp_v[36][36];
#pragma HLS array_partition variable=dbkout_temp_y1 complete dim=2
//#pragma HLS array_partition variable=dbkout_temp_y2 complete dim=2
//#pragma HLS array_partition variable=dbkout_temp_y3 complete dim=2
//#pragma HLS array_partition variable=dbkout_temp_y4 complete dim=2
#pragma HLS array_partition variable=dbkout_temp_u complete dim=2
#pragma HLS array_partition variable=dbkout_temp_v complete dim=2

#pragma HLS array_partition variable=dbkout_y complete dim=2
#pragma HLS array_partition variable=dbkout_u complete dim=2
#pragma HLS array_partition variable=dbkout_v complete dim=2
	for (int j = 0; j < 68; j++) {
#pragma HLS pipeline 
		for (int i = 0; i < 68; i++) {
#pragma HLS unroll
			dbkout_temp_y1[j][i] = dbkout_y[j + 2][i + 2];
            //dbkout_temp_y2[j][i] = dbkout_y[j + 2][i + 2];
            //dbkout_temp_y3[j][i] = dbkout_y[j + 2][i + 2];
            //dbkout_temp_y4[j][i] = dbkout_y[j + 2][i + 2];
		}
	}
	for (int j = 0; j < 36; j++) {
#pragma HLS pipeline 
		for (int i = 0; i < 36; i++) {
#pragma HLS unroll
			dbkout_temp_u[j][i] = dbkout_u[j + 2][i + 2];
			dbkout_temp_v[j][i] = dbkout_v[j + 2][i + 2];
		}
	}

    static pel org_temp_y1[64][64];
    //static pel org_temp_y2[64][64];
    //static pel org_temp_y3[64][64];
    //static pel org_temp_y4[64][64];
    static pel org_temp_u[32][32];
    static pel org_temp_v[32][32];
#pragma HLS array_partition variable=org_temp_y1 complete dim=2
//#pragma HLS array_partition variable=org_temp_y2 complete dim=2
//#pragma HLS array_partition variable=org_temp_y3 complete dim=2
//#pragma HLS array_partition variable=org_temp_y4 complete dim=2
#pragma HLS array_partition variable=org_temp_u complete dim=2
#pragma HLS array_partition variable=org_temp_v complete dim=2

#pragma HLS array_partition variable=Orig_Lcu_ptr_y_org_sao complete dim=2
#pragma HLS array_partition variable=Orig_Lcu_ptr_u_org_sao complete dim=2
#pragma HLS array_partition variable=Orig_Lcu_ptr_v_org_sao complete dim=2
    for (int j = 0; j < 64; j++) {
#pragma HLS pipeline 
        for (int i = 0; i < 64; i++) {
#pragma HLS unroll
            org_temp_y1[j][i] = Orig_Lcu_ptr_y_org_sao[j][i];
            //org_temp_y2[j][i] = Orig_Lcu_ptr_y_org_sao[j][i];
            //org_temp_y3[j][i] = Orig_Lcu_ptr_y_org_sao[j][i];
            //org_temp_y4[j][i] = Orig_Lcu_ptr_y_org_sao[j][i];
        }
    }
    for (int j = 0; j < 32; j++) {
#pragma HLS pipeline 
        for (int i = 0; i < 32; i++) {
#pragma HLS unroll
            org_temp_u[j][i] = Orig_Lcu_ptr_u_org_sao[j][i];
            org_temp_v[j][i] = Orig_Lcu_ptr_v_org_sao[j][i];
        }
    }

    sao_process(log2_max_cuwh, dbk_out_para_set_pic_width_copy, dbk_out_para_set_pic_height_copy, dbk_out_para_set_pic_width_in_lcu_copy, 
        dbk_out_para_set_pic_height_in_lcu_copy,  bit_depth_internal,
        fetch2sao_fw.sample_adaptive_offset_enable_flag,  qp_offset_bit_depth,
        fetch2sao_fw.lcu_cnt, /*bin_count,*/ max_smb_num, dbk_out_para_set_x_pel_copy, dbk_out_para_set_y_pel_copy, /*s_sao_init,*/ fetch2sao_fw.pic_width_in_scu,
        dbkout_temp_y1, /*dbkout_temp_y2, dbkout_temp_y3, dbkout_temp_y4,*/ dbkout_temp_u, dbkout_temp_v, slice_sao_enable,
        org_temp_y1,    /*org_temp_y2,    org_temp_y3,    org_temp_y4,   */ org_temp_u,    org_temp_v,
        fetch2sao_fw.lambda,
        rec_stride_luma, rec_stride_chroma, pix_x_luma, pix_y_luma, pix_x_chroma, pix_y_chroma,
        temp_sao_lcu_array_y, temp_sao_lcu_array_y_dupli1, temp_sao_lcu_array_u, temp_sao_lcu_array_v,
        //dbk_lcu_array_y,                                   dbk_lcu_array_u,      dbk_lcu_array_v, 
        sao_rslt_array_y,                                  sao_rslt_array_u,     sao_rslt_array_v,
        &lcu_avail_left, &lcu_avail_right, &lcu_avail_up, &lcu_avail_down, //&lcu_avail_up_left, &lcu_avail_up_right, &lcu_avail_down_left, &lcu_avail_down_right,
        &sq_sao_mode_y,     &up2row_sao_mode_y, &left2col_sao_mode_y, &cur_lcu_sao_mode_y,
        &up2row_y_start_x,  &up2row_y_end_x,    &left2col_y_start_y,  &left2col_y_end_y,
        &cur_lcu_y_start_x, &cur_lcu_y_end_x,   &cur_lcu_y_start_y,   &cur_lcu_y_end_y,
        &sq_sao_mode_u,     &up2row_sao_mode_u, &left2col_sao_mode_u, &cur_lcu_sao_mode_u,
        &up2row_u_start_x,  &up2row_u_end_x,    &left2col_u_start_y,  &left2col_u_end_y,
        &cur_lcu_u_start_x, &cur_lcu_u_end_x,   &cur_lcu_u_start_y,   &cur_lcu_u_end_y,
        &sq_sao_mode_v,     &up2row_sao_mode_v, &left2col_sao_mode_v, &cur_lcu_sao_mode_v,
        &up2row_v_start_x,  &up2row_v_end_x,    &left2col_v_start_y,  &left2col_v_end_y,
        &cur_lcu_v_start_x, &cur_lcu_v_end_x,   &cur_lcu_v_start_y,   &cur_lcu_v_end_y, cur_saoParam, rec_saoParam);

    //sao_2_pic_rec(y_rec, u_rec, v_rec, /*best_rec_y, best_rec_u, best_rec_v,*/ dbkout_temp_y1, dbkout_temp_u, dbkout_temp_v,
    //              sao_enable_flag, pix_y_luma, pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma,
    //                                 temp_sao_lcu_array_u,  temp_sao_lcu_array_v, 
    //            //dbk_lcu_array_y,   dbk_lcu_array_u,  dbk_lcu_array_v, 
    //              sao_rslt_array_y,  sao_rslt_array_u, sao_rslt_array_v,
    //              lcu_avail_left,    lcu_avail_up, 
    //              sq_sao_mode_y,     up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y, 
    //              up2row_y_start_x,  up2row_y_end_x,    left2col_y_start_y,  left2col_y_end_y,
    //              cur_lcu_y_start_x, cur_lcu_y_end_x,   cur_lcu_y_start_y,   cur_lcu_y_end_y,
    //              sq_sao_mode_u,     up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u,
    //              up2row_u_start_x,  up2row_u_end_x,    left2col_u_start_y,  left2col_u_end_y,
    //              cur_lcu_u_start_x, cur_lcu_u_end_x,   cur_lcu_u_start_y,   cur_lcu_u_end_y,
    //              sq_sao_mode_v,     up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v,
    //              up2row_v_start_x,  up2row_v_end_x,    left2col_v_start_y,  left2col_v_end_y,
    //              cur_lcu_v_start_x, cur_lcu_v_end_x,   cur_lcu_v_start_y,   cur_lcu_v_end_y);

#if 1
    static pel sao_test_y[69][69];
    static pel sao_test_u[36][36];
    static pel sao_test_v[36][36];
    sao_2_pic_rec2(max_smb_num, fetch2sao_fw.lcu_cnt, dbk_out_para_set_pic_width_copy, log2_max_cuwh,
        sao_test_y, sao_test_u, sao_test_v,
        dbkout_temp_y1, dbkout_temp_u, dbkout_temp_v,
        sao_enable_flag, pix_y_luma, pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma,
        temp_sao_lcu_array_u, temp_sao_lcu_array_v,
        //dbk_lcu_array_y,   dbk_lcu_array_u,  dbk_lcu_array_v, 
        sao_rslt_array_y, sao_rslt_array_u, sao_rslt_array_v,
        lcu_avail_left, lcu_avail_up,
        sq_sao_mode_y, up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y,
        up2row_y_start_x, up2row_y_end_x, left2col_y_start_y, left2col_y_end_y,
        cur_lcu_y_start_x, cur_lcu_y_end_x, cur_lcu_y_start_y, cur_lcu_y_end_y,
        sq_sao_mode_u, up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u,
        up2row_u_start_x, up2row_u_end_x, left2col_u_start_y, left2col_u_end_y,
        cur_lcu_u_start_x, cur_lcu_u_end_x, cur_lcu_u_start_y, cur_lcu_u_end_y,
        sq_sao_mode_v, up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v,
        up2row_v_start_x, up2row_v_end_x, left2col_v_start_y, left2col_v_end_y,
        cur_lcu_v_start_x, cur_lcu_v_end_x, cur_lcu_v_start_y, cur_lcu_v_end_y);
#else
	sao_2_pic_rec3(max_smb_num, fetch2sao_fw.lcu_cnt, dbk_out_para_set_pic_width_copy, log2_max_cuwh,
        sao_enable_flag, pix_y_luma, pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma,
        y_rec,            u_rec,                v_rec,
        dbkout_temp_y1,   temp_sao_lcu_array_u, temp_sao_lcu_array_v,
        sao_rslt_array_y, sao_rslt_array_u,     sao_rslt_array_v,
        lcu_avail_left,    lcu_avail_up,
        sq_sao_mode_y,     up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y,
        up2row_y_start_x,  up2row_y_end_x,    left2col_y_start_y,  left2col_y_end_y,
        cur_lcu_y_start_x, cur_lcu_y_end_x,   cur_lcu_y_start_y,   cur_lcu_y_end_y,
        sq_sao_mode_u,     up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u,
        up2row_u_start_x,  up2row_u_end_x,    left2col_u_start_y,  left2col_u_end_y,
        cur_lcu_u_start_x, cur_lcu_u_end_x,   cur_lcu_u_start_y,   cur_lcu_u_end_y,
        sq_sao_mode_v,     up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v,
        up2row_v_start_x,  up2row_v_end_x,    left2col_v_start_y,  left2col_v_end_y,
        cur_lcu_v_start_x, cur_lcu_v_end_x,   cur_lcu_v_start_y,   cur_lcu_v_end_y);
#endif 
//-------------------------------------------------------------------------------------------------------------------------------------------------

    int lcu_max_width  = 1 << log2_max_cuwh;
    int lcu_max_height = 1 << log2_max_cuwh;
    int lcu_width  = (lcu_max_width  < (dbk_out_para_set_pic_width_copy  - dbk_out_para_set_x_pel_copy)) ? lcu_max_width  : (dbk_out_para_set_pic_width_copy  - dbk_out_para_set_x_pel_copy);
    int lcu_height = (lcu_max_height < (dbk_out_para_set_pic_height_copy - dbk_out_para_set_y_pel_copy)) ? lcu_max_height : (dbk_out_para_set_pic_height_copy - dbk_out_para_set_y_pel_copy);

    int lcu_start_x = lcu_avail_left  ?  0 : 5;
    int lcu_end_x   = lcu_avail_right ? 64 : lcu_width + 5;     // 64 : 69
    int lcu_start_y = lcu_avail_up    ?  0 : 5;
    int lcu_end_y   = lcu_avail_down  ? 64 : lcu_height + 5;    // 64 : 69
    // available pixels in current LCU is :
    // for (int x=lcu_start_x; x<lcu_end_x; x++) {
    //   for (int y=lcu_start_y; y<lcu_end_y; y++) {}
    // }

    for (int i = 0; i < 69; i++) {
        for (int j = 0; j < 69; j++) {
            sao_pic_rec_test_y[i][j] = sao_test_y[i][j];
        }
    }

    for (int i = 0; i < 36; i++) {
        for (int j = 0; j < 36; j++) {
            sao_pic_rec_test_u[i][j] = sao_test_u[i][j];
            sao_pic_rec_test_v[i][j] = sao_test_v[i][j];


        }
    }

//-------------------------------------------------------------------------------------------------------------------------------------------------
// Re-arrange SAO output.
    sao_output_rearrange(max_smb_num, fetch2sao_fw.lcu_cnt, dbk_out_para_set_pic_width_in_lcu_copy, pix_y_luma, pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma,
        y_rec, u_rec, v_rec, sao_test_y, sao_test_u, sao_test_v);

//-------------------------------------------------------------------------------------------------------------------------------------------------

	update_ctx_map_frame_level(ctx_map_scu, ctx_map_mv, ctx_map_refi, ctx_map_cu_mode,
		fetch2sao_fw.pic_width, fetch2sao_fw.pic_height, fetch2sao_fw.pic_width_in_scu, dbk_out_para_set_min_scu_x_copy, dbk_out_para_set_min_scu_y_copy,
		me_line_map_ptr_map_cu_mode_inner, dbk_out_para_set_curr_ctu_idx_copy, dbk_out_para_set_map_scu_inner,
		fetch2sao_fw.pix_x, fetch2sao_fw.pix_y, 64, 64, inner_nebs2sao);

	sao_bypass(AEC_DATA_input, fetch2sao_fw, pix_x_luma, pix_y_luma,
		dbk_out_para_set_pic_width_copy, dbk_out_para_set_pic_height_copy, dbk_out_para_set_pic_width_in_lcu_copy);


    /*********************************** RFC ******************************************/
    //rfc_run(pix_y_luma, pix_x_luma, fetch2sao_fw.pic_height, fetch2sao_fw.pic_width, rec_stride_luma, rec_stride_chroma,
    //    sao_pic_rec_test_y, sao_pic_rec_test_u, sao_pic_rec_test_v, y_rfcBin, y_rfcPrm, u_rfcBin, u_rfcPrm, v_rfcBin, v_rfcPrm);

    //CPR m_cpr1;
    ////compress the luma data
    ////the first col: only copy data to buffer and don't compress
    //if (pix_x_luma == 0) {
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
    //        }
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            saoBuffer8x4096[3+i][j] = sao_pic_rec_test_y[ i][j + 5];
    //        }
    //    }
    //    if (pix_y_luma == 0) {
    //        for (int i = 0; i < 8; i++) {
    //            for (int j = 0; j < 64; j++) {
    //                saoBuffer8x4096[i][j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
    //            }
    //        }
    //    }
    //}
    ////the first row and not the last col: compress one 56x64 block
    //else if (pix_y_luma == 0 && pix_x_luma + 64 < fetch2sao_fw.pic_width) {
    //    //updata the last five col data in buffer according to currunt LCU       
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
    //    }
    //    
    //    //copy data to the function ports
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer64x64[i][j];
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 56);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma + i][pix_x_luma - 64 + j];
    //        }
    //    }
    //    for (int i = 0; i < 56 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma - 64) / 32 + j];
    //        }
    //    }
    //    //updata the buffer data
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j+5];
    //        }
    //    }
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = -5; j < 64; j++)
    //           saoBuffer8x4096[i][pix_x_luma + j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
    //    } 
    //}
    ////the first row and the last col: compress two 56x64 blocks
    //else if (pix_y_luma == 0 && pix_x_luma + 64 >= fetch2sao_fw.pic_width) {
    //    //compress the previous block
    //    //updata the last five col data in buffer according to currunt LCU       
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer64x64[i][j];
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 56);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma + i][pix_x_luma - 64 + j];
    //        }
    //    }
    //    for (int i = 0; i < 56 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma - 64) / 32 + j];
    //        }
    //    }
    //    //compress the current block
    //    //copy data to the function ports
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 56);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma , pix_y_luma, 64, 56);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma , pix_y_luma, 64, 56);
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma  + j] = rfcBinBufferFra[pix_y_luma + i][pix_x_luma  + j];
    //        }
    //    }
    //    for (int i = 0; i < 56 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma ) / 32 + j] = rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma ) / 32 + j];
    //        }
    //    }
    //    //copy data to buffer 
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = -5; j < 64; j++)
    //            saoBuffer8x4096[i][pix_x_luma + j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
    //    }
    //}
    ////the last row and not the last col: compress one 72x64 block
    //else if (pix_y_luma + 64 >= fetch2sao_fw.pic_height && pix_x_luma + 64 < fetch2sao_fw.pic_width) {
    //    //updata the last five col data in buffer according to currunt LCU       
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 69; j++)
    //            saoBuffer8x4096[3+i][pix_x_luma - 5  + j] = sao_pic_rec_test_y[i][j];
    //    }
    //    
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
    //    }
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[8+i][j] = saoBuffer64x64[i][j];
    //    }
    //   
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 72);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma-8, 64, 72);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma-8, 64, 72);
    //    for (int i = 0; i < 72; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma -8  + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma-8+ i][pix_x_luma - 64 + j];
    //        }
    //    }
    //    for (int i = 0; i < 72 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[((pix_y_luma -8)/ 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma -8)/ 4 + i][(pix_x_luma - 64) / 32 + j];
    //        }
    //    }
    //    //updata the buffer data
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
    //        }
    //    }
    //}
    ////the last row and last col: compress two 72x64 blocks
    //else if (pix_y_luma + 64 >= fetch2sao_fw.pic_height && pix_x_luma + 64 >= fetch2sao_fw.pic_width) {
    //    //compress the first (previous) blcok
    //    //updata the last five col data in buffer according to currunt LCU
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
    //    }
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[8 + i][j] = saoBuffer64x64[i][j];
    //    }
    //  
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 72);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma-8, 64, 72);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma-8, 64, 72);
    //    for (int i = 0; i < 72; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[((pix_y_luma-8) + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma -8 + i][pix_x_luma - 64 + j];
    //        }
    //    }
    //    for (int i = 0; i < 72 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[((pix_y_luma -8)/ 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma -8)/ 4 + i][(pix_x_luma - 64) / 32 + j];
    //        }
    //    }
    //    // compress the second (current) blcok
    //    
    //    for (int i = 0; i < 3; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma + j];
    //    }
    //    for (int i = 0; i < 69; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[3 + i][j] = sao_pic_rec_test_y[i][5 + j];
    //    }
    //    m_cpr1.proc(recBuffer, 0, 64, 72);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma , pix_y_luma-8, 64, 72);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma , pix_y_luma-8, 64, 72);
    //    for (int i = 0; i < 72; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma - 8+ i) * rec_stride_luma + pix_x_luma  + j] = rfcBinBufferFra[(pix_y_luma -8)+ i][pix_x_luma  + j];
    //        }
    //    }
    //    for (int i = 0; i < 72 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[((pix_y_luma -8)/ 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] = rfcPrmBufferFra[(pix_y_luma -8)/ 4 + i][(pix_x_luma ) / 32 + j];
    //        }
    //    }
    //}
    //// not the first or the last row and the last col : compress two 64x64 blcoks
    //else if (pix_x_luma + 64 >= fetch2sao_fw.pic_width) {
    //    //compress the first (previous) blcok
    //    //updata the last five col data in buffer according to currunt LCU
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 69; j++)
    //            saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
    //    }
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[8 + i][j] = saoBuffer64x64[i][j];
    //    }
    //  
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 64);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma-8, 64, 64);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma-8, 64, 64);
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma-8 + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma-8 + i][pix_x_luma - 64 + j];
    //        }
    //    }
    //    for (int i = 0; i < 64 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[((pix_y_luma-8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma-8) / 4 + i][(pix_x_luma - 64) / 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 64; j++)
    //            saoBuffer8x4096[i][pix_x_luma - 64 + j] = saoBuffer64x64[56+i][j];
    //    }
    //    // compress the second (current) blcok
    //    for (int i = 0; i < 3; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma + j];
    //    }
    //    for (int i = 0; i < 61; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[3 + i][j] = sao_pic_rec_test_y[i][5+j];
    //    }
    //    m_cpr1.proc(recBuffer, 0, 64, 64);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma , pix_y_luma-8, 64, 64);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma , pix_y_luma-8, 64, 64);
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma -8 + i) * rec_stride_luma + pix_x_luma  + j] = rfcBinBufferFra[pix_y_luma -8 + i][pix_x_luma  + j];
    //        }
    //    }
    //    for (int i = 0; i < 64 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[((pix_y_luma -8)/ 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] = rfcPrmBufferFra[(pix_y_luma -8)/ 4 + i][(pix_x_luma ) / 32 + j];
    //        }
    //    }
    //    //copy data to buffer 
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = -5; j < 64; j++)
    //            saoBuffer8x4096[i][pix_x_luma + j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
    //    }
    //}
    ////the blcoks insides LCU : compress one 64x64 block
    //else {
    //    //updata the last five col data in buffer according to currunt LCU
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 69; j++)
    //            saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
    //    }
    //    
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
    //    }
    //    for (int i = 0; i < 56; i++) {
    //        for (int j = 0; j < 64; j++)
    //            recBuffer[8 + i][j] = saoBuffer64x64[i][j];
    //    }
    //  
    //    //compress
    //    m_cpr1.proc(recBuffer, 0, 64, 64);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            y_rfcBin[(pix_y_luma-8 + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[(pix_y_luma-8) + i][pix_x_luma - 64 + j];
    //        }
    //    }
    //    for (int i = 0; i < 64 / 4; i++) {
    //        for (int j = 0; j < 64 / 32; j++) {
    //            y_rfcPrm[((pix_y_luma-8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma-8) / 4 + i][(pix_x_luma - 64) / 32 + j];
    //        }
    //    }
    //    //copy data to buffer 
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 64; j++)
    //            saoBuffer8x4096[i][pix_x_luma - 64 + j] = saoBuffer64x64[56+i][j];
    //    }
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 5; j++)
    //            saoBuffer8x4096[i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[5+56+i][j];
    //    }
    //    for (int i = 0; i < 64; i++) {
    //        for (int j = 0; j < 64; j++) {
    //            saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
    //        }
    //    }
    //}
    ////compress the chroma data
    ////the first col: only copy data to buffer and don't compress
    //if (pix_x_chroma== 0) {
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[5 + i][j + 5];
    //            saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[5 + i][j + 5];
    //        }
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer8x2048_u[3 + i][j] = sao_pic_rec_test_u[i][j + 5];
    //            saoBuffer8x2048_v[3 + i][j] = sao_pic_rec_test_v[i][j + 5];
    //        }
    //    }
    //    if (pix_y_chroma == 0) {
    //        for (int i = 0; i < 8; i++) {
    //            for (int j = 0; j < 32; j++) {
    //                saoBuffer8x2048_u[i][j] = sao_pic_rec_test_u[5 + 24 + i][j + 5];
    //                saoBuffer8x2048_v[i][j] = sao_pic_rec_test_v[5 + 24 + i][j + 5];
    //            }
    //        }
    //    }
    //}
    ////the first row and not the last col: compress one 24x32 block
    //else if (pix_y_chroma == 0 && pix_x_chroma + 32 < pic_width_chroma) {
    //    //updata the last five col data in buffer according to currunt LCU       
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer32x32_u[i][27 + j] = sao_pic_rec_test_u[5 + i][j];
    //            saoBuffer32x32_v[i][27 + j] = sao_pic_rec_test_v[5 + i][j];
    //        }
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer32x32_u[i][j];
    //            recBuffer_v[i][j] = saoBuffer32x32_v[i][j];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 24);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 24);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma - 32 + j];
    //            v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 24 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //            v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //        }
    //    }
    //    //updata the buffer data
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[5 + i][j + 5];
    //            saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[5 + i][j + 5];
    //        }
    //    }
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = -5; j < 32; j++) {
    //            saoBuffer8x2048_u[i][pix_x_chroma + j] = sao_pic_rec_test_u[5 + 24 + i][j + 5];
    //            saoBuffer8x2048_v[i][pix_x_chroma + j] = sao_pic_rec_test_v[5 + 24 + i][j + 5];
    //        }
    //    }
    //}
    ////the first row and the last col: compress two 24x32 blocks
    //else if (pix_y_chroma == 0 && pix_x_chroma + 32 >= pic_width_chroma) {
    //    //compress the previous block
    //    //updata the last five col data in buffer according to currunt LCU       
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer32x32_u[i][27 + j] = sao_pic_rec_test_u[5 + i][j];
    //            saoBuffer32x32_v[i][27 + j] = sao_pic_rec_test_v[5 + i][j];
    //        }
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer32x32_u[i][j];
    //            recBuffer_v[i][j] = saoBuffer32x32_v[i][j];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 24);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 24);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma - 32 + j];
    //            v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 24 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //            v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //        }
    //    }
    //    //compress the current block
    //    //copy data to the function ports
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = sao_pic_rec_test_u[5 + i][j + 5];
    //            recBuffer_v[i][j] = sao_pic_rec_test_v[5 + i][j + 5];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 24);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 24);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 24);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 24);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 24);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 24);
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma + j];
    //            v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma + j];
    //        }
    //    }
    //    for (int i = 0; i < 24 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
    //            v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
    //        }
    //    }
    //    //copy data to buffer 
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = -5; j < 32; j++) {
    //            saoBuffer8x2048_u[i][pix_x_chroma + j] = sao_pic_rec_test_u[5 + 24 + i][j + 5];
    //            saoBuffer8x2048_v[i][pix_x_chroma + j] = sao_pic_rec_test_v[5 + 24 + i][j + 5];
    //        }
    //    }
    //}
    ////the last row and not the last col: compress one 40x32 block
    //else if (pix_y_chroma + 32 >= pic_height_chroma && pix_x_chroma + 32 < pic_width_chroma) {
    //    //updata the last five col data in buffer according to currunt LCU       
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer32x32_u[i][27 + j] = sao_pic_rec_test_u[5 + i][j];
    //            saoBuffer32x32_v[i][27 + j] = sao_pic_rec_test_v[5 + i][j];
    //        }
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 37; j++) {
    //            saoBuffer8x2048_u[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_u[i][j];
    //            saoBuffer8x2048_v[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_v[i][j];
    //        }
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
    //            recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
    //            recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 40);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 40);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    for (int i = 0; i < 40; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
    //            v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 40 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //            v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //        }
    //    }
    //    //updata the buffer data
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[5 + i][j + 5];
    //            saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[5 + i][j + 5];
    //        }
    //    }
    //}
    ////the last row and last col: compress two 40x32 blocks
    //else if (pix_y_chroma + 32 >= pic_height_chroma && pix_x_chroma + 32 >= pic_width_chroma) {
    //    //compress the first (previous) blcok
    //    //updata the last five col data in buffer according to currunt LCU
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer32x32_u[i][27 + j] = sao_pic_rec_test_u[5 + i][j];
    //            saoBuffer32x32_v[i][27 + j] = sao_pic_rec_test_v[5 + i][j];
    //        }
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer8x2048_u[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_u[i][j];
    //            saoBuffer8x2048_v[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_v[i][j];
    //        }
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
    //            recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
    //            recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 40);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 40);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
    //    for (int i = 0; i < 40; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[((pix_y_chroma - 8) + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
    //            v_rfcBin[((pix_y_chroma - 8) + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 40 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //            v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //        }
    //    }
    //    // compress the second (current) blcok
    //    for (int i = 0; i < 3; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma + j];
    //            recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma + j];
    //        }
    //    }
    //    for (int i = 0; i < 37; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[3 + i][j] = sao_pic_rec_test_u[i][5 + j];
    //            recBuffer_v[3 + i][j] = sao_pic_rec_test_v[i][5 + j];
    //        }
    //    }
    //    m_cpr1.proc(recBuffer_u, 0, 32, 40);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 40);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 40);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 40);
    //    for (int i = 0; i < 40; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_u[(pix_y_chroma - 8) + i][pix_x_chroma + j];
    //            v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_v[(pix_y_chroma - 8) + i][pix_x_chroma + j];
    //        }
    //    }
    //    for (int i = 0; i < 40 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
    //            v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
    //        }
    //    }
    //}
    //// not the first or the last row and the last col : compress two 32x32 blcoks
    //else if (pix_x_chroma + 32 >= pic_width_chroma) {
    //    //compress the first (previous) blcok
    //    //updata the last five col data in buffer according to currunt LCU
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer32x32_u[i][27 + j] = sao_pic_rec_test_u[5 + i][j];
    //            saoBuffer32x32_v[i][27 + j] = sao_pic_rec_test_v[5 + i][j];
    //        }
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 37; j++) {
    //            saoBuffer8x2048_u[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_u[i][j];
    //            saoBuffer8x2048_v[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_v[i][j];
    //        }
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
    //            recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
    //            recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 32);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 32);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
    //            v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 32 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //            v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer8x2048_u[i][pix_x_chroma - 32 + j] = saoBuffer32x32_u[24 + i][j];
    //            saoBuffer8x2048_v[i][pix_x_chroma - 32 + j] = saoBuffer32x32_v[24 + i][j];
    //        }
    //    }
    //    // compress the second (current) blcok
    //    for (int i = 0; i < 3; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma + j];
    //            recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma + j];
    //        }
    //    }
    //    for (int i = 0; i < 29; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[3 + i][j] = sao_pic_rec_test_u[i][5 + j];
    //            recBuffer_v[3 + i][j] = sao_pic_rec_test_v[i][5 + j];
    //        }
    //    }
    //    m_cpr1.proc(recBuffer_u, 0, 32, 32);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 32);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 32);
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma + j];
    //            v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma + j];
    //        }
    //    }
    //    for (int i = 0; i < 32 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
    //            v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
    //        }
    //    }
    //    //copy data to buffer 
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = -5; j < 32; j++) {
    //            saoBuffer8x2048_u[i][pix_x_chroma + j] = sao_pic_rec_test_u[5 + 24 + i][j + 5];
    //            saoBuffer8x2048_v[i][pix_x_chroma + j] = sao_pic_rec_test_v[5 + 24 + i][j + 5];
    //        }
    //    }
    //}
    ////the blcoks insides LCU : compress one 32x32 block
    //else {
    //    //updata the last five col data in buffer according to currunt LCU
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer32x32_u[i][27 + j] = sao_pic_rec_test_u[5 + i][j];
    //            saoBuffer32x32_v[i][27 + j] = sao_pic_rec_test_v[5 + i][j];
    //        }
    //    }
    //    for (int i = 0; i < 5; i++) {
    //        for (int j = 0; j < 37; j++) {
    //            saoBuffer8x2048_u[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_u[i][j];
    //            saoBuffer8x2048_v[3 + i][pix_x_chroma - 5 + j] = sao_pic_rec_test_v[i][j];
    //        }
    //    }
    //    //copy data to the function ports
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
    //            recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 24; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
    //            recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
    //        }
    //    }
    //    //compress
    //    m_cpr1.proc(recBuffer_u, 0, 32, 32);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.proc(recBuffer_v, 0, 32, 32);
    //    m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[(pix_y_chroma - 8) + i][pix_x_chroma - 32 + j];
    //            v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[(pix_y_chroma - 8) + i][pix_x_chroma - 32 + j];
    //        }
    //    }
    //    for (int i = 0; i < 32 / 4; i++) {
    //        for (int j = 0; j < 32 / 32; j++) {
    //            u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //            v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
    //        }
    //    }
    //    //copy data to buffer 
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer8x2048_u[i][pix_x_chroma - 32 + j] = saoBuffer32x32_u[24 + i][j];
    //            saoBuffer8x2048_v[i][pix_x_chroma - 32 + j] = saoBuffer32x32_v[24 + i][j];
    //        }
    //    }
    //    for (int i = 0; i < 8; i++) {
    //        for (int j = 0; j < 5; j++) {
    //            saoBuffer8x2048_u[i][pix_x_chroma - 5 + j] = sao_pic_rec_test_u[5 + 24 + i][j];
    //            saoBuffer8x2048_v[i][pix_x_chroma - 5 + j] = sao_pic_rec_test_v[5 + 24 + i][j];
    //        }
    //    }
    //    for (int i = 0; i < 32; i++) {
    //        for (int j = 0; j < 32; j++) {
    //            saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[5 + i][j + 5];
    //            saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[5 + i][j + 5];
    //        }
    //    }
    //}

#ifndef __SYNTHESIS__
#if COLLECT_SAO_DATA
    if (sao_cu_cnt < SAO_CU_NUM_TV) {
        sao_save_outdata(cur_saoParam, ctx_map_scu, ctx_map_mv, ctx_map_refi, ctx_map_cu_mode,
        y_rec, u_rec, v_rec, AEC_DATA_input);
        printf("SAO: LCU %d OUTPUT DATA SAVED. \n", sao_cu_cnt);
        printf("------------------------------ \n", sao_cu_cnt);
        sao_cu_cnt += 1;
    }
    else if (sao_cu_cnt == SAO_CU_NUM_TV) {
        fclose(sao_input_dbk_out_para_set_min_scu_x);
        fclose(sao_input_dbk_out_para_set_min_scu_y);
        fclose(sao_input_me_line_map_ptr_map_cu_mode_inner);
        fclose(sao_input_dbk_out_para_set_curr_ctu_idx);
        fclose(sao_input_dbk_out_para_set_map_scu_inner);
        fclose(sao_input_inner_nebs2sao);
        fclose(sao_input_dbk_out_para_set_pic_width);
        fclose(sao_input_dbk_out_para_set_pic_height);
        fclose(sao_input_dbk_out_para_set_pic_width_in_lcu);
        fclose(sao_input_dbk_out_para_set_pic_height_in_lcu);
        fclose(sao_input_dbk_out_para_set_x_pel);
        fclose(sao_input_dbk_out_para_set_y_pel);
        fclose(sao_input_dbkout_y);
        fclose(sao_input_dbkout_u);
        fclose(sao_input_dbkout_v);
        fclose(sao_input_Orig_Lcu_ptr_y_org_sao);
        fclose(sao_input_Orig_Lcu_ptr_u_org_sao);
        fclose(sao_input_Orig_Lcu_ptr_v_org_sao);
        fclose(sao_input_fetch2sao_fw_ptr);
        fclose(sao_input_qp_offset);
        fclose(sao_output_cur_saoParam);
        fclose(sao_output_ctx_map_scu);
        fclose(sao_output_ctx_map_mv);
        fclose(sao_output_ctx_map_refi);
        fclose(sao_output_ctx_map_cu_mode);
        fclose(sao_output_y_rec);
        fclose(sao_output_u_rec);
        fclose(sao_output_v_rec);
        fclose(sao_output_AEC_DATA_input);

        printf("Files closed. \n");
        sao_cu_cnt += 1;
    }
#endif
#endif //#ifndef __SYNTHESIS__

//#if defined SAO_TV_ACTIVE   // Write SAO Result to file
//    pel sao_pic_rec_test_y[69][69];
//    pel sao_pic_rec_test_u[37][37];
//    pel sao_pic_rec_test_v[37][37];
//
//    // For TestVector sao_pic_rec_test_y, u, v only:
//    for (y = 0; y < 69; y++)
//    {
//        for (x = 0; x < 69; x++)
//        {
//            sao_pic_rec_test_y[y][x] = recpic->y[(pix_y_luma + y - 5) * rec_stride_luma + pix_x_luma + x - 5];
//        }
//    }
//    for (y = 0; y < 37; y++)
//    {
//        for (x = 0; x < 37; x++)
//        {
//            sao_pic_rec_test_u[y][x] = recpic->u[(pix_y_chroma + y - 5) * rec_stride_chroma + pix_x_chroma + x - 5];
//            sao_pic_rec_test_v[y][x] = recpic->v[(pix_y_chroma + y - 5) * rec_stride_chroma + pix_x_chroma + x - 5];
//        }
//    }
//    sao_test_module.writeOutputGolden2File(sao_pic_rec_test_y, (sizeof(pel) * 69 * 69));   // SAO output, Reconstructed Pic.
//    sao_test_module.writeOutputGolden2File(sao_pic_rec_test_u, (sizeof(pel) * 37 * 37));   // SAO output
//    sao_test_module.writeOutputGolden2File(sao_pic_rec_test_v, (sizeof(pel) * 37 * 37));   // SAO output
//    sao_test_module.writeOutputGolden2File(cur_saoParam, ((sizeof(int) * 5 + sizeof(int) * 32) * 3));   // SAO output, SAO Parameters
//                                                                                                        // int modeIdc; //NEW, MERGE, OFF
//                                                                                                        // int typeIdc; //NEW: EO_0, EO_90, EO_135, EO_45, BO. MERGE: left, above
//                                                                                                        // int startBand; //BO: starting band index
//                                                                                                        // int startBand2;
//                                                                                                        // int deltaband;
//                                                                                                        // int offset[MAX_NUM_SAO_CLASSES];
//#endif//SAO_TV_ACTIVE

}


void sao_output_rearrange(int max_smb_num, int lcu_cnt, U6 pic_width_in_lcu, int pix_y_luma, int pix_x_luma, int pix_y_chroma, int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma,
    pel* y_rec, pel* u_rec, pel* v_rec, pel sao_test_y[69][69], pel sao_test_u[36][36], pel sao_test_v[36][36]) 
{
#pragma HLS INLINE off
    static pel rfc_line_y_1[3][3846];// [1925];
    static pel rfc_line_y_2[3][3846];// [1925];
    static pel rfc_line_u_1[4][3846];// [1925];
    static pel rfc_line_u_2[4][3846];// [1925];
    static pel rfc_line_v_1[4][3846];// [1925];
    static pel rfc_line_v_2[4][3846];// [1925];
    static pel sao_test_y_temp[69][69];
    static pel sao_test_u_temp[36][36];
    static pel sao_test_v_temp[36][36];
    static pel sao_rfc_y_1     [56][64];
    static pel sao_rfc_y_1_last[56][64];
    static pel sao_rfc_y_2     [64][64];
    static pel sao_rfc_y_2_last[64][64];
    static pel sao_rfc_y_3     [72][64];
    static pel sao_rfc_y_3_last[72][64];
    static pel sao_rfc_u_1     [24][32];
    static pel sao_rfc_u_1_last[24][32];
    static pel sao_rfc_u_2     [32][32];
    static pel sao_rfc_u_2_last[32][32];
    static pel sao_rfc_u_3     [40][32];
    static pel sao_rfc_u_3_last[40][32];
    static pel sao_rfc_v_1     [24][32];
    static pel sao_rfc_v_1_last[24][32];
    static pel sao_rfc_v_2     [32][32];
    static pel sao_rfc_v_2_last[32][32];
    static pel sao_rfc_v_3     [40][32];
    static pel sao_rfc_v_3_last[40][32];
    int lcu_pos_inrow = (max_smb_num - lcu_cnt - 1) % pic_width_in_lcu;
    if (max_smb_num - lcu_cnt > 1) {
        if (max_smb_num - lcu_cnt < pic_width_in_lcu + 1) {
            for (int y = 0; y < 56; y++) {
                for (int x = 0; x < 59; x++) {
                    sao_rfc_y_1[y][x] = sao_test_y_temp[y + 5][x + 5];
                }
            }
            for (int y = 0; y < 56; y++) {
                for (int x = 59; x < 64; x++) {
                    sao_rfc_y_1[y][x] = sao_test_y[y + 5][x - 59];
                }
            }
            for (int y = 0; y < 24; y++) {
                for (int x = 0; x < 28; x++) {
                    sao_rfc_u_1[y][x] = sao_test_u_temp[y + 4][x + 4];
                    sao_rfc_v_1[y][x] = sao_test_v_temp[y + 4][x + 4];
                }
            }
            for (int y = 0; y < 24; y++) {
                for (int x = 28; x < 32; x++) {
                    sao_rfc_u_1[y][x] = sao_test_u[y + 4][x - 28];
                    sao_rfc_v_1[y][x] = sao_test_v[y + 4][x - 28];
                }
            }
#if RFC_DDR
            for (int y = 0; y < 56; y++) {
                for (int x = 0; x < 64; x++) {
                    y_rec[(pix_y_luma + y) * rec_stride_luma + pix_x_luma - 64 + x] = sao_rfc_y_1[y][x];
                }
            }
            for (int y = 0; y < 24; y++) {
                for (int x = 0; x < 32; x++) {
                    u_rec[(pix_y_chroma + y) * rec_stride_chroma + pix_x_chroma - 32 + x] = sao_rfc_u_1[y][x];
                    v_rec[(pix_y_chroma + y) * rec_stride_chroma + pix_x_chroma - 32 + x] = sao_rfc_v_1[y][x];
                }
            }
#endif
            if ((max_smb_num - lcu_cnt) % pic_width_in_lcu == 0) {
                for (int y = 0; y < 56; y++) {
                    for (int x = 0; x < 64; x++) {
                        sao_rfc_y_1_last[y][x] = sao_test_y[y + 5][x + 5];
                    }
                }
                for (int y = 0; y < 24; y++) {
                    for (int x = 0; x < 32; x++) {
                        sao_rfc_u_1_last[y][x] = sao_test_u[y + 4][x + 4];
                        sao_rfc_v_1_last[y][x] = sao_test_v[y + 4][x + 4];
                    }
                }
#if RFC_DDR
                for (int y = 0; y < 56; y++) {
                    for (int x = 0; x < 64; x++) {
                        y_rec[(pix_y_luma + y) * rec_stride_luma + pix_x_luma + x] = sao_rfc_y_1_last[y][x];
                    }
                }
                for (int y = 0; y < 24; y++) {
                    for (int x = 0; x < 32; x++) {
                        u_rec[(pix_y_chroma + y) * rec_stride_chroma + pix_x_chroma + x] = sao_rfc_u_1_last[y][x];
                        v_rec[(pix_y_chroma + y) * rec_stride_chroma + pix_x_chroma + x] = sao_rfc_v_1_last[y][x];
                    }
                }
#endif
            }
        }
        else if (max_smb_num - lcu_cnt >= pic_width_in_lcu + 2 && max_smb_num - lcu_cnt <= max_smb_num - pic_width_in_lcu) {
            if ((max_smb_num - lcu_cnt - 1) % pic_width_in_lcu != 0) {
                for (int y = 3; y < 64; y++) {
                    for (int x = 0; x < 59; x++) {
                        sao_rfc_y_2[y][x] = sao_test_y_temp[y - 3][x + 5];
                    }
                }
                for (int y = 3; y < 64; y++) {
                    for (int x = 59; x < 64; x++) {
                        sao_rfc_y_2[y][x] = sao_test_y[y - 3][x - 59];
                    }
                }
                for (int y = 0; y < 3; y++) {
                    for (int x = 0; x < 64; x++) {
                        if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                            sao_rfc_y_2[y][x] = rfc_line_y_2[y][x + 5 + (lcu_pos_inrow - 1) * 64];
                        }
                        else {
                            sao_rfc_y_2[y][x] = rfc_line_y_1[y][x + 5 + (lcu_pos_inrow - 1) * 64];
                        }
                    }
                }
                for (int y = 4; y < 32; y++) {
                    for (int x = 0; x < 28; x++) {
                        sao_rfc_u_2[y][x] = sao_test_u_temp[y - 4][x + 4];
                        sao_rfc_v_2[y][x] = sao_test_v_temp[y - 4][x + 4];
                    }
                }
                for (int y = 4; y < 32; y++) {
                    for (int x = 28; x < 32; x++) {
                        sao_rfc_u_2[y][x] = sao_test_u[y - 4][x - 28];
                        sao_rfc_v_2[y][x] = sao_test_v[y - 4][x - 28];
                    }
                }
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 32; x++) {
                        if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                            sao_rfc_u_2[y][x] = rfc_line_u_2[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                            sao_rfc_v_2[y][x] = rfc_line_v_2[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                        }
                        else {
                            sao_rfc_u_2[y][x] = rfc_line_u_1[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                            sao_rfc_v_2[y][x] = rfc_line_v_1[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                        }
                    }
                }
#if RFC_DDR
                for (int y = 0; y < 64; y++) {
                    for (int x = 0; x < 64; x++) {
                        y_rec[(pix_y_luma + y - 8) * rec_stride_luma + pix_x_luma - 64 + x] = sao_rfc_y_2[y][x];
                    }
                }
                for (int y = 0; y < 32; y++) {
                    for (int x = 0; x < 32; x++) {
                        u_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma - 32 + x] = sao_rfc_u_2[y][x];
                        v_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma - 32 + x] = sao_rfc_v_2[y][x];
                    }
                }
#endif
            }
            if ((max_smb_num - lcu_cnt) % pic_width_in_lcu == 0) {
                for (int y = 3; y < 64; y++) {
                    for (int x = 0; x < 64; x++) {
                        sao_rfc_y_2_last[y][x] = sao_test_y[y - 3][x + 5];
                    }
                }
                for (int y = 0; y < 3; y++) {
                    for (int x = 0; x < 64; x++) {
                        if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                            sao_rfc_y_2_last[y][x] = rfc_line_y_2[y][x + 5 + (lcu_pos_inrow) * 64];
                        }
                        else {
                            sao_rfc_y_2_last[y][x] = rfc_line_y_1[y][x + 5 + (lcu_pos_inrow) * 64];
                        }
                    }
                }
                for (int y = 4; y < 32; y++) {
                    for (int x = 0; x < 32; x++) {
                        sao_rfc_u_2_last[y][x] = sao_test_u[y - 4][x + 4];
                        sao_rfc_v_2_last[y][x] = sao_test_v[y - 4][x + 4];
                    }
                }
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 32; x++) {
                        if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                            sao_rfc_u_2_last[y][x] = rfc_line_u_2[y][x + 4 + (lcu_pos_inrow) * 32];
                            sao_rfc_v_2_last[y][x] = rfc_line_v_2[y][x + 4 + (lcu_pos_inrow) * 32];
                        }
                        else {
                            sao_rfc_u_2_last[y][x] = rfc_line_u_1[y][x + 4 + (lcu_pos_inrow) * 32];
                            sao_rfc_v_2_last[y][x] = rfc_line_v_1[y][x + 4 + (lcu_pos_inrow) * 32];
                        }
                    }
                }
#if RFC_DDR
                for (int y = 0; y < 64; y++) {
                    for (int x = 0; x < 64; x++) {
                        y_rec[(pix_y_luma + y - 8) * rec_stride_luma + pix_x_luma + x] = sao_rfc_y_2_last[y][x];
                    }
                }
                for (int y = 0; y < 32; y++) {
                    for (int x = 0; x < 32; x++) {
                        u_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma + x] = sao_rfc_u_2_last[y][x];
                        v_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma + x] = sao_rfc_v_2_last[y][x];
                    }
                }
#endif
            }
        }
        else if (max_smb_num - lcu_cnt > max_smb_num - pic_width_in_lcu + 1) {
            for (int y = 3; y < 72; y++) {
                for (int x = 0; x < 59; x++) {
                    sao_rfc_y_3[y][x] = sao_test_y_temp[y - 3][x + 5];
                }
            }
            for (int y = 3; y < 72; y++) {
                for (int x = 59; x < 64; x++) {
                    sao_rfc_y_3[y][x] = sao_test_y[y - 3][x - 59];
                }
            }
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 64; x++) {
                    if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                        sao_rfc_y_3[y][x] = rfc_line_y_2[y][x + 5 + (lcu_pos_inrow - 1) * 64];
                    }
                    else {
                        sao_rfc_y_3[y][x] = rfc_line_y_1[y][x + 5 + (lcu_pos_inrow - 1) * 64];
                    }
                }
            }
            for (int y = 4; y < 40; y++) {
                for (int x = 0; x < 28; x++) {
                    sao_rfc_u_3[y][x] = sao_test_u_temp[y - 4][x + 4];
                    sao_rfc_v_3[y][x] = sao_test_v_temp[y - 4][x + 4];
                }
            }
            for (int y = 4; y < 40; y++) {
                for (int x = 28; x < 32; x++) {
                    sao_rfc_u_3[y][x] = sao_test_u[y - 4][x - 28];
                    sao_rfc_v_3[y][x] = sao_test_v[y - 4][x - 28];
                }
            }
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 32; x++) {
                    if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                        sao_rfc_u_3[y][x] = rfc_line_u_2[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                        sao_rfc_v_3[y][x] = rfc_line_v_2[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                    }
                    else {
                        sao_rfc_u_3[y][x] = rfc_line_u_1[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                        sao_rfc_v_3[y][x] = rfc_line_v_1[y][x + 4 + (lcu_pos_inrow - 1) * 32];
                    }
                }
            }
#if RFC_DDR
            for (int y = 0; y < 72; y++) {
                for (int x = 0; x < 64; x++) {
                    y_rec[(pix_y_luma + y - 8) * rec_stride_luma + pix_x_luma - 64 + x] = sao_rfc_y_3[y][x];
                }
            }
            for (int y = 0; y < 40; y++) {
                for (int x = 0; x < 32; x++) {
                    u_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma - 32 + x] = sao_rfc_u_3[y][x];
                    v_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma - 32 + x] = sao_rfc_v_3[y][x];
                }
            }
#endif
            if ((max_smb_num - lcu_cnt) % pic_width_in_lcu == 0) {
                for (int y = 3; y < 72; y++) {
                    for (int x = 0; x < 64; x++) {
                        sao_rfc_y_3_last[y][x] = sao_test_y[y - 3][x + 5];
                    }
                }
                for (int y = 0; y < 3; y++) {
                    for (int x = 0; x < 64; x++) {
                        if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                            sao_rfc_y_3_last[y][x] = rfc_line_y_2[y][x + 5 + (lcu_pos_inrow) * 64];
                        }
                        else {
                            sao_rfc_y_3_last[y][x] = rfc_line_y_1[y][x + 5 + (lcu_pos_inrow) * 64];
                        }
                    }
                }
                for (int y = 4; y < 40; y++) {
                    for (int x = 0; x < 32; x++) {
                        sao_rfc_u_3_last[y][x] = sao_test_u[y - 4][x + 4];
                        sao_rfc_v_3_last[y][x] = sao_test_v[y - 4][x + 4];
                    }
                }
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 32; x++) {
                        if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
                            sao_rfc_u_3_last[y][x] = rfc_line_u_2[y][x + 4 + (lcu_pos_inrow) * 32];
                            sao_rfc_v_3_last[y][x] = rfc_line_v_2[y][x + 4 + (lcu_pos_inrow) * 32];
                        }
                        else {
                            sao_rfc_u_3_last[y][x] = rfc_line_u_1[y][x + 4 + (lcu_pos_inrow) * 32];
                            sao_rfc_v_3_last[y][x] = rfc_line_v_1[y][x + 4 + (lcu_pos_inrow) * 32];
                        }
                    }
                }
#if RFC_DDR
                for (int y = 0; y < 72; y++) {
                    for (int x = 0; x < 64; x++) {
                        y_rec[(pix_y_luma + y - 8) * rec_stride_luma + pix_x_luma + x] = sao_rfc_y_3_last[y][x];
                    }
                }
                for (int y = 0; y < 40; y++) {
                    for (int x = 0; x < 32; x++) {
                        u_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma + x] = sao_rfc_u_3_last[y][x];
                        v_rec[(pix_y_chroma + y - 8) * rec_stride_chroma + pix_x_chroma + x] = sao_rfc_v_3_last[y][x];
                    }
                }
#endif
            }
        }
    }
    for (int y = 0; y < 69; y++) {
        for (int x = 0; x < 69; x++) {
            sao_test_y_temp[y][x] = sao_test_y[y][x];
        }
    }
    for (int y = 0; y < 36; y++) {
        for (int x = 0; x < 36; x++) {
            sao_test_u_temp[y][x] = sao_test_u[y][x];
            sao_test_v_temp[y][x] = sao_test_v[y][x];
        }
    }
    if (((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2 == 0) {
        if ((max_smb_num - lcu_cnt) % pic_width_in_lcu == 0) {
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 69; x++) {
                    rfc_line_y_1[y][x + lcu_pos_inrow * 64] = sao_test_y[y + 61][x];
                }
            }
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 36; x++) {
                    rfc_line_u_1[y][x + lcu_pos_inrow * 32] = sao_test_u[y + 28][x];
                    rfc_line_v_1[y][x + lcu_pos_inrow * 32] = sao_test_v[y + 28][x];
                }
            }
        }
        else {
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 64; x++) {
                    rfc_line_y_1[y][x + lcu_pos_inrow * 64] = sao_test_y[y + 61][x];
                }
            }
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 32; x++) {
                    rfc_line_u_1[y][x + lcu_pos_inrow * 32] = sao_test_u[y + 28][x];
                    rfc_line_v_1[y][x + lcu_pos_inrow * 32] = sao_test_v[y + 28][x];
                }
            }
        }
    }
    else {
        if ((max_smb_num - lcu_cnt) % pic_width_in_lcu == 0) {
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 69; x++) {
                    rfc_line_y_2[y][x + lcu_pos_inrow * 64] = sao_test_y[y + 61][x];
                }
            }
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 36; x++) {
                    rfc_line_u_2[y][x + lcu_pos_inrow * 32] = sao_test_u[y + 28][x];
                    rfc_line_v_2[y][x + lcu_pos_inrow * 32] = sao_test_v[y + 28][x];
                }
            }
        }
        else {
            for (int y = 0; y < 3; y++) {
                for (int x = 0; x < 64; x++) {
                    rfc_line_y_2[y][x + lcu_pos_inrow * 64] = sao_test_y[y + 61][x];
                }
            }
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 32; x++) {
                    rfc_line_u_2[y][x + lcu_pos_inrow * 32] = sao_test_u[y + 28][x];
                    rfc_line_v_2[y][x + lcu_pos_inrow * 32] = sao_test_v[y + 28][x];
                }
            }
        }
    }
}


void rfc_run(int pix_y_luma, int pix_x_luma, U13 pic_height, U13 pic_width, int rec_stride_luma, int rec_stride_chroma, 
    pel sao_pic_rec_test_y[69][69], pel sao_pic_rec_test_u[36][36], pel sao_pic_rec_test_v[36][36],
    pel* y_rfcBin, U16* y_rfcPrm, pel* u_rfcBin, U16* u_rfcPrm, pel* v_rfcBin, U16* v_rfcPrm)
{
    int pix_y_chroma = pix_y_luma >> 1;
    int pix_x_chroma = pix_x_luma >> 1;
    int pic_height_chroma = pic_height >> 1;
    int pic_width_chroma  = pic_width  >> 1;
    CPR m_cpr1;
    //compress the luma data
    //the first col: only copy data to buffer and don't compress
    if (pix_x_luma == 0) {
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
            }
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 64; j++) {
                saoBuffer8x4096[3 + i][j] = sao_pic_rec_test_y[i][j + 5];
            }
        }
        if (pix_y_luma == 0) {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 64; j++) {
                    saoBuffer8x4096[i][j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
                }
            }
        }
    }
    //the first row and not the last col: compress one 56x64 block
    else if (pix_y_luma == 0 && pix_x_luma + 64 < pic_width) {
        //updata the last five col data in buffer according to currunt LCU       
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
        }

        //copy data to the function ports
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer64x64[i][j];
        }
        //compress
        m_cpr1.proc(recBuffer, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma + i][pix_x_luma - 64 + j];

            }
        }
        for (int i = 0; i < 56 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }
        //updata the buffer data
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++) {
                saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = -5; j < 64; j++)
                saoBuffer8x4096[i][pix_x_luma + j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
        }
    }
    //the first row and the last col: compress two 56x64 blocks
    else if (pix_y_luma == 0 && pix_x_luma + 64 >= pic_width) {
        //compress the previous block
        //updata the last five col data in buffer according to currunt LCU       
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
        }

        //copy data to the function ports
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer64x64[i][j];
        }
        //compress
        m_cpr1.proc(recBuffer, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma + i][pix_x_luma - 64 + j];

            }
        }
        for (int i = 0; i < 56 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }

        //compress the current block

        //copy data to the function ports
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
        }
        //compress
        m_cpr1.proc(recBuffer, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma + j] = rfcBinBufferFra[pix_y_luma + i][pix_x_luma + j];

            }
        }
        for (int i = 0; i < 56 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] = rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma) / 32 + j];

            }
        }
        //copy data to buffer 
        for (int i = 0; i < 8; i++) {
            for (int j = -5; j < 64; j++)
                saoBuffer8x4096[i][pix_x_luma + j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
        }
    }
    //the last row and not the last col: compress one 72x64 block
    else if (pix_y_luma + 64 >= pic_height && pix_x_luma + 64 < pic_width) {
        //updata the last five col data in buffer according to currunt LCU       
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 69; j++)
                saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
        }
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[8 + i][j] = saoBuffer64x64[i][j];
        }

        //compress
        m_cpr1.proc(recBuffer, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma - 64 + j];

            }
        }
        for (int i = 0; i < 72 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }
        //updata the buffer data
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
            }
        }
    }
    //the last row and last col: compress two 72x64 blocks
    else if (pix_y_luma + 64 >= pic_height && pix_x_luma + 64 >= pic_width) {
        //compress the first (previous) blcok
        //updata the last five col data in buffer according to currunt LCU
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
        }
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[8 + i][j] = saoBuffer64x64[i][j];
        }

        //compress
        m_cpr1.proc(recBuffer, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[((pix_y_luma - 8) + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma - 64 + j];

            }
        }
        for (int i = 0; i < 72 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }

        // compress the second (current) blcok

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma + j];
        }
        for (int i = 0; i < 69; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[3 + i][j] = sao_pic_rec_test_y[i][5 + j];
        }

        m_cpr1.proc(recBuffer, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma + j] = rfcBinBufferFra[(pix_y_luma - 8) + i][pix_x_luma + j];

            }
        }
        for (int i = 0; i < 72 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] = rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma) / 32 + j];

            }
        }
    }
    // not the first or the last row and the last col : compress two 64x64 blcoks
    else if (pix_x_luma + 64 >= pic_width) {
        //compress the first (previous) blcok
        //updata the last five col data in buffer according to currunt LCU
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 69; j++)
                saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
        }
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[8 + i][j] = saoBuffer64x64[i][j];
        }

        //compress
        m_cpr1.proc(recBuffer, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma - 64 + j];

            }
        }
        for (int i = 0; i < 64 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++)
                saoBuffer8x4096[i][pix_x_luma - 64 + j] = saoBuffer64x64[56 + i][j];
        }

        // compress the second (current) blcok
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma + j];
        }
        for (int i = 0; i < 61; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[3 + i][j] = sao_pic_rec_test_y[i][5 + j];
        }
        m_cpr1.proc(recBuffer, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma + j] = rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma + j];

            }
        }
        for (int i = 0; i < 64 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] = rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma) / 32 + j];

            }
        }
        //copy data to buffer 
        for (int i = 0; i < 8; i++) {
            for (int j = -5; j < 64; j++)
                saoBuffer8x4096[i][pix_x_luma + j] = sao_pic_rec_test_y[5 + 56 + i][j + 5];
        }

    }
    //the blcoks insides LCU : compress one 64x64 block
    else {
        //updata the last five col data in buffer according to currunt LCU
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer64x64[i][59 + j] = sao_pic_rec_test_y[5 + i][j];
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 69; j++)
                saoBuffer8x4096[3 + i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[i][j];
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[i][j] = saoBuffer8x4096[i][pix_x_luma - 64 + j];
        }
        for (int i = 0; i < 56; i++) {
            for (int j = 0; j < 64; j++)
                recBuffer[8 + i][j] = saoBuffer64x64[i][j];
        }

        //compress
        m_cpr1.proc(recBuffer, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] = rfcBinBufferFra[(pix_y_luma - 8) + i][pix_x_luma - 64 + j];

            }
        }
        for (int i = 0; i < 64 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] = rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }
        //copy data to buffer 
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++)
                saoBuffer8x4096[i][pix_x_luma - 64 + j] = saoBuffer64x64[56 + i][j];
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 5; j++)
                saoBuffer8x4096[i][pix_x_luma - 5 + j] = sao_pic_rec_test_y[5 + 56 + i][j];
        }
        for (int i = 0; i < 64; i++) {
            for (int j = 0; j < 64; j++) {
                saoBuffer64x64[i][j] = sao_pic_rec_test_y[5 + i][j + 5];
            }
        }

    }


    int a = 1 ;
    //compress the chroma data
    //the first col: only copy data to buffer and don't compress
    if (pix_x_chroma == 0) {
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[4 + i][j + 4];
                saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[4 + i][j + 4];
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer8x2048_u[4 + i][j] = sao_pic_rec_test_u[i][j + 4];
                saoBuffer8x2048_v[4 + i][j] = sao_pic_rec_test_v[i][j + 4];
            }
        }
        if (pix_y_chroma == 0) {
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 32; j++) {
                    saoBuffer8x2048_u[i][j] = sao_pic_rec_test_u[4 + 24 + i][j + 4];
                    saoBuffer8x2048_v[i][j] = sao_pic_rec_test_v[4 + 24 + i][j + 4];
                }
            }
        }
    }
    //the first row and not the last col: compress one 24x32 block
    else if (pix_y_chroma == 0 && pix_x_chroma + 32 < pic_width_chroma) {
        //updata the last five col data in buffer according to currunt LCU       
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer32x32_u[i][28 + j] = sao_pic_rec_test_u[4 + i][j];
                saoBuffer32x32_v[i][28 + j] = sao_pic_rec_test_v[4 + i][j];
            }
        }

        //copy data to the function ports
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer32x32_u[i][j];
                recBuffer_v[i][j] = saoBuffer32x32_v[i][j];
            }
        }
        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 24);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);

        m_cpr1.proc(recBuffer_v, 0, 32, 24);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);

        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma - 32 + j];

            }
        }
        for (int i = 0; i < 24 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
        //updata the buffer data
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[4 + i][j + 4];
                saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[4 + i][j + 4];
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = -4; j < 32; j++) {
                saoBuffer8x2048_u[i][pix_x_chroma + j] = sao_pic_rec_test_u[4 + 24 + i][j + 4];
                saoBuffer8x2048_v[i][pix_x_chroma + j] = sao_pic_rec_test_v[4 + 24 + i][j + 4];
            }
        }
    }
    //the first row and the last col: compress two 24x32 blocks
    else if (pix_y_chroma == 0 && pix_x_chroma + 32 >= pic_width_chroma) {
        //compress the previous block
        //updata the last five col data in buffer according to currunt LCU       
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer32x32_u[i][28 + j] = sao_pic_rec_test_u[4 + i][j];
                saoBuffer32x32_v[i][28 + j] = sao_pic_rec_test_v[4 + i][j];
            }
        }

        //copy data to the function ports
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer32x32_u[i][j];
                recBuffer_v[i][j] = saoBuffer32x32_v[i][j];
            }
        }
        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 24);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 24);

        m_cpr1.proc(recBuffer_v, 0, 32, 24);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 24);

        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 24 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];

            }
        }

        //compress the current block

        //copy data to the function ports
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = sao_pic_rec_test_u[4 + i][j + 4];
                recBuffer_v[i][j] = sao_pic_rec_test_v[4 + i][j + 4];
            }
        }
        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 24);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 24);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 24);

        m_cpr1.proc(recBuffer_v, 0, 32, 24);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 24);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 24);

        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma + j];

            }
        }
        for (int i = 0; i < 24 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
        //copy data to buffer 
        for (int i = 0; i < 8; i++) {
            for (int j = -4; j < 32; j++) {
                saoBuffer8x2048_u[i][pix_x_chroma + j] = sao_pic_rec_test_u[4 + 24 + i][j + 4];
                saoBuffer8x2048_v[i][pix_x_chroma + j] = sao_pic_rec_test_v[4 + 24 + i][j + 4];
            }
        }
    }
    //the last row and not the last col: compress one 40x32 block
    else if (pix_y_chroma + 32 >= pic_height_chroma && pix_x_chroma + 32 < pic_width_chroma) {
        //updata the last five col data in buffer according to currunt LCU       
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer32x32_u[i][28 + j] = sao_pic_rec_test_u[4 + i][j];
                saoBuffer32x32_v[i][28 + j] = sao_pic_rec_test_v[4 + i][j];
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 36; j++) {
                saoBuffer8x2048_u[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_u[i][j];
                saoBuffer8x2048_v[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_v[i][j];
            }
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
                recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
                recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
            }
        }

        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 40);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);

        m_cpr1.proc(recBuffer_v, 0, 32, 40);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);

        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 40 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
        //updata the buffer data
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[4 + i][j + 4];
                saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[4 + i][j + 4];
            }
        }
    }
    //the last row and last col: compress two 40x32 blocks
    else if (pix_y_chroma + 32 >= pic_height_chroma && pix_x_chroma + 32 >= pic_width_chroma) {
        //compress the first (previous) blcok
        //updata the last five col data in buffer according to currunt LCU
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer32x32_u[i][28 + j] = sao_pic_rec_test_u[4 + i][j];
                saoBuffer32x32_v[i][28 + j] = sao_pic_rec_test_v[4 + i][j];
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer8x2048_u[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_u[i][j];
                saoBuffer8x2048_v[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_v[i][j];
            }
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
                recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
                recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
            }
        }

        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 40);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);

        m_cpr1.proc(recBuffer_v, 0, 32, 40);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 40);

        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[((pix_y_chroma - 8) + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
                v_rfcBin[((pix_y_chroma - 8) + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 40 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }

        // compress the second (current) blcok

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma + j];
                recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma + j];
            }
        }
        for (int i = 0; i < 36; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[4 + i][j] = sao_pic_rec_test_u[i][4 + j];
                recBuffer_v[4 + i][j] = sao_pic_rec_test_v[i][4 + j];
            }
        }

        m_cpr1.proc(recBuffer_u, 0, 32, 40);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 40);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 40);


        m_cpr1.proc(recBuffer_v, 0, 32, 40);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 40);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 40);

        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_u[(pix_y_chroma - 8) + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_v[(pix_y_chroma - 8) + i][pix_x_chroma + j];

            }
        }
        for (int i = 0; i < 40 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    // not the first or the last row and the last col : compress two 32x32 blcoks
    else if (pix_x_chroma + 32 >= pic_width_chroma) {
        //compress the first (previous) blcok
        //updata the last five col data in buffer according to currunt LCU
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer32x32_u[i][28 + j] = sao_pic_rec_test_u[4 + i][j];
                saoBuffer32x32_v[i][28 + j] = sao_pic_rec_test_v[4 + i][j];
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 36; j++) {
                saoBuffer8x2048_u[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_u[i][j];
                saoBuffer8x2048_v[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_v[i][j];
            }
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
                recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
                recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
            }
        }

        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);

        m_cpr1.proc(recBuffer_v, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);

        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 32 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer8x2048_u[i][pix_x_chroma - 32 + j] = saoBuffer32x32_u[24 + i][j];
                saoBuffer8x2048_v[i][pix_x_chroma - 32 + j] = saoBuffer32x32_v[24 + i][j];
            }
        }

        // compress the second (current) blcok
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma + j];
                recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma + j];
            }
        }
        for (int i = 0; i < 28; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[4 + i][j] = sao_pic_rec_test_u[i][4 + j];
                recBuffer_v[4 + i][j] = sao_pic_rec_test_v[i][4 + j];
            }
        }
        m_cpr1.proc(recBuffer_u, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 8, 32, 32);

        m_cpr1.proc(recBuffer_v, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 8, 32, 32);

        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_u[pix_y_chroma - 8 + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma + j] = rfcBinBufferFra_v[pix_y_chroma - 8 + i][pix_x_chroma + j];
            }
        }
        for (int i = 0; i < 32 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
        //copy data to buffer 
        for (int i = 0; i < 8; i++) {
            for (int j = -4; j < 32; j++) {
                saoBuffer8x2048_u[i][pix_x_chroma + j] = sao_pic_rec_test_u[4 + 24 + i][j + 4];
                saoBuffer8x2048_v[i][pix_x_chroma + j] = sao_pic_rec_test_v[4 + 24 + i][j + 4];
            }
        }

    }
    //the blcoks insides LCU : compress one 32x32 block
    else {
        //updata the last five col data in buffer according to currunt LCU
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer32x32_u[i][28 + j] = sao_pic_rec_test_u[4 + i][j];
                saoBuffer32x32_v[i][28 + j] = sao_pic_rec_test_v[4 + i][j];
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 36; j++) {
                saoBuffer8x2048_u[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_u[i][j];
                saoBuffer8x2048_v[4 + i][pix_x_chroma - 4 + j] = sao_pic_rec_test_v[i][j];
            }
        }

        //copy data to the function ports
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[i][j] = saoBuffer8x2048_u[i][pix_x_chroma - 32 + j];
                recBuffer_v[i][j] = saoBuffer8x2048_v[i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 32; j++) {
                recBuffer_u[8 + i][j] = saoBuffer32x32_u[i][j];
                recBuffer_v[8 + i][j] = saoBuffer32x32_v[i][j];
            }
        }

        //compress
        m_cpr1.proc(recBuffer_u, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);

        m_cpr1.proc(recBuffer_v, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 8, 32, 32);

        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                u_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_u[(pix_y_chroma - 8) + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 8 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = rfcBinBufferFra_v[(pix_y_chroma - 8) + i][pix_x_chroma - 32 + j];
            }
        }
        for (int i = 0; i < 32 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_u[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 8) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] = rfcPrmBufferFra_v[(pix_y_chroma - 8) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
        //copy data to buffer 
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer8x2048_u[i][pix_x_chroma - 32 + j] = saoBuffer32x32_u[24 + i][j];
                saoBuffer8x2048_v[i][pix_x_chroma - 32 + j] = saoBuffer32x32_v[24 + i][j];
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 4; j++) {
                saoBuffer8x2048_u[i][pix_x_chroma - 4 + j] = sao_pic_rec_test_u[4 + 24 + i][j];
                saoBuffer8x2048_v[i][pix_x_chroma - 4 + j] = sao_pic_rec_test_v[4 + 24 + i][j];
            }
        }
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                saoBuffer32x32_u[i][j] = sao_pic_rec_test_u[4 + i][j + 4];
                saoBuffer32x32_v[i][j] = sao_pic_rec_test_v[4 + i][j + 4];
            }
        }

    }
}
#endif//原有代码部分：sao_run、sao_output_rearrange、rfc_run

/***************************************sao_run函数重构*********************************************************************/
void sao_run(/*U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height, U6 dbk_out_para_set_pic_width_in_lcu, U6 dbk_out_para_set_pic_height_in_lcu,*/
    U12 fetch_output_sao_pix_x, U12 fetch_output_sao_pix_y, pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39], SAOBlkParam cur_saoParam[3],
    pel md2sao_y_org_sao[64][64], pel md2sao_u_org_sao[32][32], pel md2sao_v_org_sao[32][32],
    FETCH2SAO_FW* fetch2sao_fw_ptr, SAO2AEC* AEC_DATA_input, /*U32 qp_offset,*/ LCU_2RFC_PARAM_T* lcu_2rfc_param, SAO_2RTL_PARAM_T* sao_2rtl_param,
    pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
    pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32])
{
#pragma HLS inline off

#ifndef __SYNTHESIS__
#if COLLECT_SAO_DATA
    if (sao_cu_cnt == 0) {
        sao_input_dbk_out_para_set_min_scu_x = open_file(SAO_INPUT_dbk_out_para_set_min_scu_x, "wb");
        sao_input_dbk_out_para_set_min_scu_y = open_file(SAO_INPUT_dbk_out_para_set_min_scu_y, "wb");
        sao_input_me_line_map_ptr_map_cu_mode_inner = open_file(SAO_INPUT_me_line_map_ptr_map_cu_mode_inner, "wb");
        sao_input_dbk_out_para_set_curr_ctu_idx = open_file(SAO_INPUT_dbk_out_para_set_curr_ctu_idx, "wb");
        sao_input_dbk_out_para_set_map_scu_inner = open_file(SAO_INPUT_dbk_out_para_set_map_scu_inner, "wb");
        sao_input_inner_nebs2sao = open_file(SAO_INPUT_inner_nebs2sao, "wb");
        sao_input_dbk_out_para_set_pic_width = open_file(SAO_INPUT_dbk_out_para_set_pic_width, "wb");
        sao_input_dbk_out_para_set_pic_height = open_file(SAO_INPUT_dbk_out_para_set_pic_height, "wb");
        sao_input_dbk_out_para_set_pic_width_in_lcu = open_file(SAO_INPUT_dbk_out_para_set_pic_width_in_lcu, "wb");
        sao_input_dbk_out_para_set_pic_height_in_lcu = open_file(SAO_INPUT_dbk_out_para_set_pic_height_in_lcu, "wb");
        sao_input_dbk_out_para_set_x_pel = open_file(SAO_INPUT_dbk_out_para_set_x_pel, "wb");
        sao_input_dbk_out_para_set_y_pel = open_file(SAO_INPUT_dbk_out_para_set_y_pel, "wb");
        sao_input_dbkout_y = open_file(SAO_INPUT_dbkout_y, "wb");
        sao_input_dbkout_u = open_file(SAO_INPUT_dbkout_u, "wb");
        sao_input_dbkout_v = open_file(SAO_INPUT_dbkout_v, "wb");
        sao_input_Orig_Lcu_ptr_y_org_sao = open_file(SAO_INPUT_Orig_Lcu_ptr_y_org_sao, "wb");
        sao_input_Orig_Lcu_ptr_u_org_sao = open_file(SAO_INPUT_Orig_Lcu_ptr_u_org_sao, "wb");
        sao_input_Orig_Lcu_ptr_v_org_sao = open_file(SAO_INPUT_Orig_Lcu_ptr_v_org_sao, "wb");
        sao_input_fetch2sao_fw_ptr = open_file(SAO_INPUT_fetch2sao_fw_ptr, "wb");
        sao_input_qp_offset = open_file(SAO_INPUT_qp_offset, "wb");

        sao_output_cur_saoParam = open_file(SAO_OUTPUT_cur_saoParam, "wb");
        sao_output_ctx_map_scu = open_file(SAO_OUTPUT_ctx_map_scu, "wb");
        sao_output_ctx_map_mv = open_file(SAO_OUTPUT_ctx_map_mv, "wb");
        sao_output_ctx_map_refi = open_file(SAO_OUTPUT_ctx_map_refi, "wb");
        sao_output_ctx_map_cu_mode = open_file(SAO_OUTPUT_ctx_map_cu_mode, "wb");
        sao_output_y_rec = open_file(SAO_OUTPUT_y_rec, "wb");
        sao_output_u_rec = open_file(SAO_OUTPUT_u_rec, "wb");
        sao_output_v_rec = open_file(SAO_OUTPUT_v_rec, "wb");
        sao_output_AEC_DATA_input = open_file(SAO_OUTPUT_AEC_DATA_input, "wb");
    }
    if (sao_cu_cnt < SAO_CU_NUM_TV) {
        sao_save_indata(ctx_map_scu, ctx_map_mv, ctx_map_refi, ctx_map_cu_mode,
            &dbk_out_para_set_min_scu_x, &dbk_out_para_set_min_scu_y, me_line_map_ptr_map_cu_mode_inner,
            &dbk_out_para_set_curr_ctu_idx, dbk_out_para_set_map_scu_inner,
            inner_nebs2sao,
            &dbk_out_para_set_pic_width, &dbk_out_para_set_pic_height, &dbk_out_para_set_pic_width_in_lcu, &dbk_out_para_set_pic_height_in_lcu,
            &dbk_out_para_set_x_pel, &dbk_out_para_set_y_pel, dbkout_y, dbkout_u, dbkout_v,
            Orig_Lcu_ptr_y_org_sao, Orig_Lcu_ptr_u_org_sao, Orig_Lcu_ptr_v_org_sao, y_rec, u_rec, v_rec, fetch2sao_fw_ptr, AEC_DATA_input, &qp_offset);
        printf("SAO: LCU %d  INPUT DATA SAVED. \n", sao_cu_cnt);
    }
#endif
#endif //__SYNTHESIS__

#pragma HLS RESOURCE variable=md2sao_y_org_sao core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2sao_u_org_sao core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md2sao_v_org_sao core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=dbkout_v core=RAM_1P_BRAM
//#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_width
//#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_height
//#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_width_in_lcu
//#pragma HLS INTERFACE axis port=dbk_out_para_set_pic_height_in_lcu
#pragma HLS INTERFACE axis port=fetch_output_sao_pix_x
#pragma HLS INTERFACE axis port=fetch_output_sao_pix_y

#pragma HLS RESOURCE variable=fetch2sao_fw_ptr->lambda core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->stride_luma
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->stride_chroma
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->lcu_cnt
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pix_x
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pix_y
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_height
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width_in_scu
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_width_in_lcu
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->pic_height_in_lcu

#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->max_cuwh
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->log2_max_cuwh
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->num_refp
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->log2_culine
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->slice_type
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->num_of_hmvp_cand

#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->amvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->emvr_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->dt_intra_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->tscpm_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->ipf_enable_flag
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cur_ptr
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cu_height
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->cu_width
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->lcu_num
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->bit_depth
#pragma HLS INTERFACE axis port=fetch2sao_fw_ptr->qp_offset_bit_depth

#pragma HLS RESOURCE variable=cur_saoParam core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=AEC_DATA_input->rpm_num_refp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=AEC_DATA_input->slice_sao_enable core=RAM_1P_BRAM
#pragma HLS INTERFACE axis port=AEC_DATA_input->x0
#pragma HLS INTERFACE axis port=AEC_DATA_input->y0
#pragma HLS INTERFACE axis port=AEC_DATA_input->cup
#pragma HLS INTERFACE axis port=AEC_DATA_input->cu_width
#pragma HLS INTERFACE axis port=AEC_DATA_input->cu_height
#pragma HLS INTERFACE axis port=AEC_DATA_input->cud
#pragma HLS INTERFACE axis port=AEC_DATA_input->parent_split
#pragma HLS INTERFACE axis port=AEC_DATA_input->qt_depth
#pragma HLS INTERFACE axis port=AEC_DATA_input->bet_depth
#pragma HLS INTERFACE axis port=AEC_DATA_input->cons_pred_mode
#pragma HLS INTERFACE axis port=AEC_DATA_input->tree_status
#pragma HLS INTERFACE axis port=AEC_DATA_input->log2_culine
#pragma HLS INTERFACE axis port=AEC_DATA_input->pic_header_slice_type
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_max_cuwh
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_width
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_height
#pragma HLS INTERFACE axis port=AEC_DATA_input->sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->lcu_cnt
#pragma HLS INTERFACE axis port=AEC_DATA_input->lcu_num
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_umve_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_num_of_hmvp_cand
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_affine_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_amvr_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_emvr_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_dt_intra_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_tscpm_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_sample_adaptive_offset_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->sqh_ipf_enable_flag
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_width_in_scu
#pragma HLS INTERFACE axis port=AEC_DATA_input->info_pic_width_in_lcu
#pragma HLS INTERFACE axis port=AEC_DATA_input->x_lcu
#pragma HLS INTERFACE axis port=AEC_DATA_input->y_lcu
#pragma HLS INTERFACE axis port=AEC_DATA_input->x_pel
#pragma HLS INTERFACE axis port=AEC_DATA_input->y_pel
#pragma HLS INTERFACE axis port=AEC_DATA_input->ptr
#pragma HLS INTERFACE axis port=AEC_DATA_input->MergeLeftAvail
#pragma HLS INTERFACE axis port=AEC_DATA_input->MergeUpAvail
//#pragma HLS INTERFACE axis port=qp_offset

#pragma HLS RESOURCE variable=sao_pic_rec_test_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_pic_rec_test_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_pic_rec_test_v core=RAM_1P_BRAM
#pragma HLS array_reshape variable=sao_pic_rec_test_y complete dim=2
#pragma HLS array_reshape variable=sao_pic_rec_test_u complete dim=2
#pragma HLS array_reshape variable=sao_pic_rec_test_v complete dim=2 

#pragma HLS RESOURCE variable=sao_pic_rec_test_y_last core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_pic_rec_test_u_last core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_pic_rec_test_v_last core=RAM_1P_BRAM
#pragma HLS array_reshape variable=sao_pic_rec_test_y_last complete dim=2
#pragma HLS array_reshape variable=sao_pic_rec_test_u_last complete dim=2
#pragma HLS array_reshape variable=sao_pic_rec_test_v_last complete dim=2

#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_luma_width
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_luma_height
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_chroma_width
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_chroma_height
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_luma_pos_x
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_luma_pos_y
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_chroma_pos_x
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_chroma_pos_y
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_luma_col_nums
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_luma_row_nums
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_chroma_col_nums
#pragma HLS INTERFACE axis port=sao_2rtl_param->lcu_chroma_row_nums

    static SAOBlkParam rec_saoParam[60 + 2][3];//for printf
    static FETCH2SAO_FW fetch2sao_fw;
    fetch2sao_fw.pic_width = fetch2sao_fw_ptr->pic_width;
    fetch2sao_fw.pic_height = fetch2sao_fw_ptr->pic_height;
    fetch2sao_fw.pic_width_in_scu = fetch2sao_fw_ptr->pic_width_in_scu;
    fetch2sao_fw.pic_width_in_lcu = fetch2sao_fw_ptr->pic_width_in_lcu;
    fetch2sao_fw.pic_height_in_lcu = fetch2sao_fw_ptr->pic_height_in_lcu;
    fetch2sao_fw.sample_adaptive_offset_enable_flag =
        fetch2sao_fw_ptr->sample_adaptive_offset_enable_flag;
    fetch2sao_fw.lambda[0] = fetch2sao_fw_ptr->lambda[0];
    fetch2sao_fw.lambda[1] = fetch2sao_fw_ptr->lambda[1];
    fetch2sao_fw.lambda[2] = fetch2sao_fw_ptr->lambda[2];
    fetch2sao_fw.stride_luma = fetch2sao_fw_ptr->stride_luma;
    fetch2sao_fw.stride_chroma = fetch2sao_fw_ptr->stride_chroma;
    fetch2sao_fw.lcu_cnt = fetch2sao_fw_ptr->lcu_cnt;
    fetch2sao_fw.max_cuwh = fetch2sao_fw_ptr->max_cuwh;
    fetch2sao_fw.log2_max_cuwh = fetch2sao_fw_ptr->log2_max_cuwh;
#if ENABLE_BFRAME
    fetch2sao_fw.num_refp[0] = fetch2sao_fw_ptr->num_refp[0];
    fetch2sao_fw.num_refp[1] = fetch2sao_fw_ptr->num_refp[1];
#else
    fetch2sao_fw.num_refp = fetch2sao_fw_ptr->num_refp;
#endif
    fetch2sao_fw.log2_culine = fetch2sao_fw_ptr->log2_culine;
    fetch2sao_fw.slice_type = fetch2sao_fw_ptr->slice_type;
    fetch2sao_fw.num_of_hmvp_cand = fetch2sao_fw_ptr->num_of_hmvp_cand;
    fetch2sao_fw.affine_enable_flag = 1;
    fetch2sao_fw.amvr_enable_flag = fetch2sao_fw_ptr->amvr_enable_flag;
    fetch2sao_fw.emvr_enable_flag = fetch2sao_fw_ptr->emvr_enable_flag;
    fetch2sao_fw.dt_intra_enable_flag = fetch2sao_fw_ptr->dt_intra_enable_flag;
    fetch2sao_fw.position_based_transform_enable_flag =
        fetch2sao_fw_ptr->position_based_transform_enable_flag;
    fetch2sao_fw.tscpm_enable_flag = fetch2sao_fw_ptr->tscpm_enable_flag;
    fetch2sao_fw.ipf_enable_flag = fetch2sao_fw_ptr->ipf_enable_flag;
    fetch2sao_fw.cur_ptr = fetch2sao_fw_ptr->cur_ptr;
    fetch2sao_fw.pix_x = fetch2sao_fw_ptr->pix_x;
    fetch2sao_fw.pix_y = fetch2sao_fw_ptr->pix_y;
    fetch2sao_fw.cu_width = fetch2sao_fw_ptr->cu_width;
    fetch2sao_fw.cu_height = fetch2sao_fw_ptr->cu_height;
    fetch2sao_fw.lcu_num = fetch2sao_fw_ptr->lcu_num;
    //copy dbk_out_para_set
    U13 dbk_out_para_set_pic_width_copy = fetch2sao_fw.pic_width;
    U13 dbk_out_para_set_pic_height_copy = fetch2sao_fw.pic_height;
    
    U6  dbk_out_para_set_pic_width_in_lcu_copy = fetch2sao_fw.pic_width_in_lcu;
    U6  dbk_out_para_set_pic_height_in_lcu_copy = fetch2sao_fw.pic_height_in_lcu;
    U12 dbk_out_para_set_y_pel_copy = fetch_output_sao_pix_y;
    U12 dbk_out_para_set_x_pel_copy = fetch_output_sao_pix_x;

    int pix_y_luma, pix_x_luma;
    int pix_y_chroma, pix_x_chroma;
    int rec_stride_luma, rec_stride_chroma;

    int pic_width_chroma = fetch2sao_fw.pic_width / 2;
    int pic_height_chroma = fetch2sao_fw.pic_height / 2;
    static pel temp_sao_lcu_array_y[70][70];
    static pel temp_sao_lcu_array_y_dupli1[70][70];
    static pel temp_sao_lcu_array_u[38][38];
    static pel temp_sao_lcu_array_v[38][38];
#pragma HLS array_partition variable=temp_sao_lcu_array_y complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_y_dupli1 complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_u complete dim=2
#pragma HLS array_partition variable=temp_sao_lcu_array_v complete dim=2

    static pel sao_rslt_array_y[70][70];
    static pel sao_rslt_array_u[38][38];
    static pel sao_rslt_array_v[38][38];
#pragma HLS array_partition variable=sao_rslt_array_y complete dim=2
#pragma HLS array_partition variable=sao_rslt_array_u complete dim=2
#pragma HLS array_partition variable=sao_rslt_array_v complete dim=2

    int lcu_avail_left, lcu_avail_right, lcu_avail_up, lcu_avail_down;

    int sq_sao_mode_y, up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y;
    int up2row_y_start_x, up2row_y_end_x, left2col_y_start_y, left2col_y_end_y;
    int cur_lcu_y_start_x, cur_lcu_y_end_x, cur_lcu_y_start_y, cur_lcu_y_end_y;

    int sq_sao_mode_u, up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u;
    int up2row_u_start_x, up2row_u_end_x, left2col_u_start_y, left2col_u_end_y;
    int cur_lcu_u_start_x, cur_lcu_u_end_x, cur_lcu_u_start_y, cur_lcu_u_end_y;

    int sq_sao_mode_v, up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v;
    int up2row_v_start_x, up2row_v_end_x, left2col_v_start_y, left2col_v_end_y;
    int cur_lcu_v_start_x, cur_lcu_v_end_x, cur_lcu_v_start_y, cur_lcu_v_end_y;
    u8  slice_sao_enable[3];
    slice_sao_enable[0] = 1;
    slice_sao_enable[1] = 1;
    slice_sao_enable[2] = 1;
    u8  sao_enable_flag = fetch2sao_fw.sample_adaptive_offset_enable_flag;

    pix_y_luma = dbk_out_para_set_y_pel_copy;
    pix_x_luma = dbk_out_para_set_x_pel_copy;
    pix_y_chroma = pix_y_luma >> 1;
    pix_x_chroma = pix_x_luma >> 1;
    rec_stride_luma = fetch2sao_fw.stride_luma;
    rec_stride_chroma = fetch2sao_fw.stride_chroma;
    int max_smb_num = dbk_out_para_set_pic_width_in_lcu_copy * dbk_out_para_set_pic_height_in_lcu_copy;

    int log2_max_cuwh = 6;
    int bit_depth_internal = fetch2sao_fw_ptr->bit_depth;
    int qp_offset_bit_depth = fetch2sao_fw_ptr->qp_offset_bit_depth;
    static pel dbkout_temp_y1[68][68];
    static pel dbkout_temp_u[36][36];
    static pel dbkout_temp_v[36][36];
#pragma HLS array_partition variable=dbkout_temp_y1 complete dim=2
#pragma HLS array_partition variable=dbkout_temp_u complete dim=2
#pragma HLS array_partition variable=dbkout_temp_v complete dim=2

#pragma HLS array_partition variable=dbkout_y complete dim=2
#pragma HLS array_partition variable=dbkout_u complete dim=2
#pragma HLS array_partition variable=dbkout_v complete dim=2
    for (int j = 0; j < 68; j++) {
#pragma HLS pipeline 
        for (int i = 0; i < 68; i++) {
#pragma HLS unroll
            dbkout_temp_y1[j][i] = dbkout_y[j + 2][i + 2];
        }
    }
    for (int j = 0; j < 36; j++) {
#pragma HLS pipeline 
        for (int i = 0; i < 36; i++) {
#pragma HLS unroll
            dbkout_temp_u[j][i] = dbkout_u[j + 2][i + 2];
            dbkout_temp_v[j][i] = dbkout_v[j + 2][i + 2];
        }
    }

    static pel org_temp_y1[64][64];
    static pel org_temp_u[32][32];
    static pel org_temp_v[32][32];
#pragma HLS array_partition variable=org_temp_y1 complete dim=2
#pragma HLS array_partition variable=org_temp_u complete dim=2
#pragma HLS array_partition variable=org_temp_v complete dim=2

#pragma HLS array_partition variable=md2sao_y_org_sao complete dim=2
#pragma HLS array_partition variable=md2sao_u_org_sao complete dim=2
#pragma HLS array_partition variable=md2sao_v_org_sao complete dim=2
    for (int j = 0; j < 64; j++) {
#pragma HLS pipeline 
        for (int i = 0; i < 64; i++) {
#pragma HLS unroll
            org_temp_y1[j][i] = md2sao_y_org_sao[j][i];
        }
    }
    for (int j = 0; j < 32; j++) {
#pragma HLS pipeline 
        for (int i = 0; i < 32; i++) {
#pragma HLS unroll
            org_temp_u[j][i] = md2sao_u_org_sao[j][i];
            org_temp_v[j][i] = md2sao_v_org_sao[j][i];
        }
    }

    sao_process(log2_max_cuwh, dbk_out_para_set_pic_width_copy, dbk_out_para_set_pic_height_copy, dbk_out_para_set_pic_width_in_lcu_copy,
        dbk_out_para_set_pic_height_in_lcu_copy, bit_depth_internal,
        fetch2sao_fw.sample_adaptive_offset_enable_flag, qp_offset_bit_depth,
        fetch2sao_fw.lcu_cnt, /*bin_count,*/ max_smb_num, dbk_out_para_set_x_pel_copy, dbk_out_para_set_y_pel_copy, /*s_sao_init,*/ fetch2sao_fw.pic_width_in_scu,
        dbkout_temp_y1, /*dbkout_temp_y2, dbkout_temp_y3, dbkout_temp_y4,*/ dbkout_temp_u, dbkout_temp_v, slice_sao_enable,
        org_temp_y1,    /*org_temp_y2,    org_temp_y3,    org_temp_y4,   */ org_temp_u, org_temp_v,
        fetch2sao_fw.lambda,
        rec_stride_luma, rec_stride_chroma, pix_x_luma, pix_y_luma, pix_x_chroma, pix_y_chroma,
        temp_sao_lcu_array_y, temp_sao_lcu_array_y_dupli1, temp_sao_lcu_array_u, temp_sao_lcu_array_v,
        //dbk_lcu_array_y,                                   dbk_lcu_array_u,      dbk_lcu_array_v, 
        sao_rslt_array_y, sao_rslt_array_u, sao_rslt_array_v,
        &lcu_avail_left, &lcu_avail_right, &lcu_avail_up, &lcu_avail_down, //&lcu_avail_up_left, &lcu_avail_up_right, &lcu_avail_down_left, &lcu_avail_down_right,
        &sq_sao_mode_y, &up2row_sao_mode_y, &left2col_sao_mode_y, &cur_lcu_sao_mode_y,
        &up2row_y_start_x, &up2row_y_end_x, &left2col_y_start_y, &left2col_y_end_y,
        &cur_lcu_y_start_x, &cur_lcu_y_end_x, &cur_lcu_y_start_y, &cur_lcu_y_end_y,
        &sq_sao_mode_u, &up2row_sao_mode_u, &left2col_sao_mode_u, &cur_lcu_sao_mode_u,
        &up2row_u_start_x, &up2row_u_end_x, &left2col_u_start_y, &left2col_u_end_y,
        &cur_lcu_u_start_x, &cur_lcu_u_end_x, &cur_lcu_u_start_y, &cur_lcu_u_end_y,
        &sq_sao_mode_v, &up2row_sao_mode_v, &left2col_sao_mode_v, &cur_lcu_sao_mode_v,
        &up2row_v_start_x, &up2row_v_end_x, &left2col_v_start_y, &left2col_v_end_y,
        &cur_lcu_v_start_x, &cur_lcu_v_end_x, &cur_lcu_v_start_y, &cur_lcu_v_end_y, cur_saoParam, rec_saoParam);

    static pel sao_test_y[69][69];
    static pel sao_test_u[36][36];
    static pel sao_test_v[36][36];
    sao_2_pic_rec2(max_smb_num, fetch2sao_fw.lcu_cnt, dbk_out_para_set_pic_width_copy, log2_max_cuwh,
        sao_test_y, sao_test_u, sao_test_v,
        dbkout_temp_y1, dbkout_temp_u, dbkout_temp_v,
        sao_enable_flag, pix_y_luma, pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma,
        temp_sao_lcu_array_u, temp_sao_lcu_array_v,
        sao_rslt_array_y, sao_rslt_array_u, sao_rslt_array_v,
        lcu_avail_left, lcu_avail_up,
        sq_sao_mode_y, up2row_sao_mode_y, left2col_sao_mode_y, cur_lcu_sao_mode_y,
        up2row_y_start_x, up2row_y_end_x, left2col_y_start_y, left2col_y_end_y,
        cur_lcu_y_start_x, cur_lcu_y_end_x, cur_lcu_y_start_y, cur_lcu_y_end_y,
        sq_sao_mode_u, up2row_sao_mode_u, left2col_sao_mode_u, cur_lcu_sao_mode_u,
        up2row_u_start_x, up2row_u_end_x, left2col_u_start_y, left2col_u_end_y,
        cur_lcu_u_start_x, cur_lcu_u_end_x, cur_lcu_u_start_y, cur_lcu_u_end_y,
        sq_sao_mode_v, up2row_sao_mode_v, left2col_sao_mode_v, cur_lcu_sao_mode_v,
        up2row_v_start_x, up2row_v_end_x, left2col_v_start_y, left2col_v_end_y,
        cur_lcu_v_start_x, cur_lcu_v_end_x, cur_lcu_v_start_y, cur_lcu_v_end_y);
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------------------------------------------------------
    // Re-arrange SAO output.

    int lcu_chroma_col_nums_cpy=dbk_out_para_set_pic_width_in_lcu_copy%2?(dbk_out_para_set_pic_width_in_lcu_copy/2+1):
                                                        (dbk_out_para_set_pic_width_in_lcu_copy/2);
    int lcu_chroma_row_nums_cpy=dbk_out_para_set_pic_height_in_lcu_copy%2?(dbk_out_para_set_pic_height_in_lcu_copy/2+1):
                                                        (dbk_out_para_set_pic_height_in_lcu_copy/2);

    sao_2rtl_param->lcu_luma_col_nums=dbk_out_para_set_pic_width_in_lcu_copy;
    sao_2rtl_param->lcu_luma_row_nums=dbk_out_para_set_pic_height_in_lcu_copy;
    sao_2rtl_param->lcu_chroma_col_nums=lcu_chroma_col_nums_cpy;
    sao_2rtl_param->lcu_chroma_row_nums=lcu_chroma_row_nums_cpy;

    sao_2rtl_param->lcu_luma_pos_x=pix_x_luma;
    sao_2rtl_param->lcu_luma_pos_y=pix_y_luma;
    sao_2rtl_param->lcu_chroma_pos_x=pix_x_chroma;
    sao_2rtl_param->lcu_chroma_pos_y=pix_y_chroma;

    LCU_2RFC_PARAM_T lcu_2rfc_param_cpy;

    lcu_2rfc_param_cpy.lcu_use_left = lcu_avail_left;
    lcu_2rfc_param_cpy.lcu_use_right = lcu_avail_right;
    lcu_2rfc_param_cpy.lcu_use_up = lcu_avail_up;
    lcu_2rfc_param_cpy.lcu_use_down = lcu_avail_down;

    sao_output_rearrange(max_smb_num, fetch2sao_fw.lcu_cnt, dbk_out_para_set_pic_width_in_lcu_copy, pix_y_luma,
        pix_x_luma, pix_y_chroma, pix_x_chroma, rec_stride_luma, rec_stride_chroma, &lcu_2rfc_param_cpy,sao_2rtl_param,
        sao_test_y, sao_test_u, sao_test_v,
        sao_pic_rec_test_y, sao_pic_rec_test_u, sao_pic_rec_test_v,
        sao_pic_rec_test_y_last, sao_pic_rec_test_u_last, sao_pic_rec_test_v_last);
    
#pragma HLS INTERFACE axis port=lcu_2rfc_param->lcu_use_left
#pragma HLS INTERFACE axis port=lcu_2rfc_param->lcu_use_right
#pragma HLS INTERFACE axis port=lcu_2rfc_param->lcu_use_up
#pragma HLS INTERFACE axis port=lcu_2rfc_param->lcu_use_down
#pragma HLS INTERFACE axis port=lcu_2rfc_param->lcu_positon

    lcu_2rfc_param->lcu_use_left = lcu_2rfc_param_cpy.lcu_use_left;
    lcu_2rfc_param->lcu_use_right = lcu_2rfc_param_cpy.lcu_use_right;
    lcu_2rfc_param->lcu_use_up = lcu_2rfc_param_cpy.lcu_use_up;
    lcu_2rfc_param->lcu_use_down = lcu_2rfc_param_cpy.lcu_use_down;
    lcu_2rfc_param->lcu_positon = lcu_2rfc_param_cpy.lcu_positon;
    //-------------------------------------------------------------------------------------------------------------------------------------------------

    sao_bypass(AEC_DATA_input, fetch2sao_fw, pix_x_luma, pix_y_luma,
        dbk_out_para_set_pic_width_copy, dbk_out_para_set_pic_height_copy, dbk_out_para_set_pic_width_in_lcu_copy);

#ifndef __SYNTHESIS__
#if COLLECT_SAO_DATA
    if (sao_cu_cnt < SAO_CU_NUM_TV) {
        sao_save_outdata(cur_saoParam, ctx_map_scu, ctx_map_mv, ctx_map_refi, ctx_map_cu_mode,
            y_rec, u_rec, v_rec, AEC_DATA_input);
        printf("SAO: LCU %d OUTPUT DATA SAVED. \n", sao_cu_cnt);
        printf("------------------------------ \n", sao_cu_cnt);
        sao_cu_cnt += 1;
    }
    else if (sao_cu_cnt == SAO_CU_NUM_TV) {
        fclose(sao_input_dbk_out_para_set_min_scu_x);
        fclose(sao_input_dbk_out_para_set_min_scu_y);
        fclose(sao_input_me_line_map_ptr_map_cu_mode_inner);
        fclose(sao_input_dbk_out_para_set_curr_ctu_idx);
        fclose(sao_input_dbk_out_para_set_map_scu_inner);
        fclose(sao_input_inner_nebs2sao);
        fclose(sao_input_dbk_out_para_set_pic_width);
        fclose(sao_input_dbk_out_para_set_pic_height);
        fclose(sao_input_dbk_out_para_set_pic_width_in_lcu);
        fclose(sao_input_dbk_out_para_set_pic_height_in_lcu);
        fclose(sao_input_dbk_out_para_set_x_pel);
        fclose(sao_input_dbk_out_para_set_y_pel);
        fclose(sao_input_dbkout_y);
        fclose(sao_input_dbkout_u);
        fclose(sao_input_dbkout_v);
        fclose(sao_input_Orig_Lcu_ptr_y_org_sao);
        fclose(sao_input_Orig_Lcu_ptr_u_org_sao);
        fclose(sao_input_Orig_Lcu_ptr_v_org_sao);
        fclose(sao_input_fetch2sao_fw_ptr);
        fclose(sao_input_qp_offset);
        fclose(sao_output_cur_saoParam);
        fclose(sao_output_ctx_map_scu);
        fclose(sao_output_ctx_map_mv);
        fclose(sao_output_ctx_map_refi);
        fclose(sao_output_ctx_map_cu_mode);
        fclose(sao_output_y_rec);
        fclose(sao_output_u_rec);
        fclose(sao_output_v_rec);
        fclose(sao_output_AEC_DATA_input);

        printf("Files closed. \n");
        sao_cu_cnt += 1;
    }
#endif
#endif //#ifndef __SYNTHESIS__
}
/***************************************************************************************************************/

/*************************************************sao_output_rerange函数重构***************************************/
void sao_output_rearrange(int max_smb_num, int lcu_cnt, U6 pic_width_in_lcu, int pix_y_luma, int pix_x_luma, int pix_y_chroma,
    int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param,SAO_2RTL_PARAM_T* sao_2rtl_param,
    pel sao_test_y[69][69], pel sao_test_u[36][36], pel sao_test_v[36][36],
    pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
    pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32])
{
#pragma HLS INLINE off
    //static pel rfc_line_y[2][3][3846];//3840/64=60 ---> 59*64+69=3845;最后一个维度进行展开为3846，超出底层阈值1024
    static pel rfc_line_y[2][180][69];//最多60个CTU，每个需要额外存储3行，故为180；每一行需要存储64 or 69个像素值，故采用最大值69

    static pel sao_test_y_temp[69][69];
    static pel sao_test_u_temp[36][36];
    static pel sao_test_v_temp[36][36];

/*****************************添加pragma参数信息**********************************************/
#pragma HLS RESOURCE variable=sao_test_y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_test_u core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_test_v core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_test_y_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_test_u_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=sao_test_v_temp core=RAM_1P_BRAM

#pragma HLS array_partition variable=sao_test_y complete dim=2
#pragma HLS array_partition variable=sao_test_u complete dim=2
#pragma HLS array_partition variable=sao_test_v complete dim=2
#pragma HLS array_partition variable=sao_test_y_temp complete dim=2
#pragma HLS array_partition variable=sao_test_u_temp complete dim=2
#pragma HLS array_partition variable=sao_test_v_temp complete dim=2

#pragma HLS RESOURCE variable=rfc_line_y core=RAM_1P_BRAM
#pragma HLS array_partition variable=rfc_line_y complete dim=3
/*********************************************************************************************/

    int lcu_pos_inrow = (max_smb_num - lcu_cnt - 1) % pic_width_in_lcu;
    int rfc_line_use_flag = ((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2;

    //第一列数据：只有copy的操作
    if (0 == lcu_2rfc_param->lcu_use_left)
    {
        sao_2rtl_param->lcu_luma_width=0;
        sao_2rtl_param->lcu_luma_height=0;
        sao_2rtl_param->lcu_chroma_width=0;
        sao_2rtl_param->lcu_chroma_height=0;

        LOOP_yhy_1_Y:for (int y = 0; y < 69; ++y)
        {
#pragma HLS pipeline 
            LOOP_yhy_1_X:for (int x = 0; x < 69; ++x)
            {
#pragma HLS unroll
                sao_test_y_temp[y][x] = sao_test_y[y][x];

                if(y<36 && x<36)
                {
                    sao_test_u_temp[y][x] = sao_test_u[y][x];
                    sao_test_v_temp[y][x] = sao_test_v[y][x];
                }
            }
        }

        int inrow_offset=lcu_pos_inrow * 3;

        LOOP_yhy_2_Y:for (int y = 0; y < 3; y++)
        {
#pragma HLS pipeline 
            int idx_y=y+61;
            int idx_y1=inrow_offset+y;
            LOOP_yhy_2_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                rfc_line_y[rfc_line_use_flag][idx_y1][x] = sao_test_y[idx_y][x];
            }
        }

        lcu_2rfc_param->lcu_positon = FIRST_COL;

        return;
    }
    //第一行非第一列
    else if (0 == lcu_2rfc_param->lcu_use_up)
    {     

        sao_2rtl_param->lcu_luma_width=64;
        sao_2rtl_param->lcu_luma_height=56;
        sao_2rtl_param->lcu_chroma_width=32;
        sao_2rtl_param->lcu_chroma_height=28;

        LOOP_yhy_3_Y:for (int y = 0; y < 56; ++y)
        {
#pragma HLS pipeline
            int idx_y=y+5;
            LOOP_yhy_3_X:for (int x = 0; x < 64; ++x)
            {
#pragma HLS unroll
                if(x<59)
                {
                    sao_pic_rec_test_y[y][x] = sao_test_y_temp[idx_y][x + 5];
                }
                else
                {
                    sao_pic_rec_test_y[y][x] = sao_test_y[idx_y][x - 59];
                }
            }
        }

        LOOP_yhy_4_Y:for (int y = 0; y < 28; y++)
        {
#pragma HLS pipeline
            int idx_y=y+4;
            LOOP_yhy_4_X:for (int x = 0; x < 32; x++)
            {
#pragma HLS unroll
                if(x<28)
                {
                    sao_pic_rec_test_u[y][x] = sao_test_u_temp[idx_y][x + 4];
                    sao_pic_rec_test_v[y][x] = sao_test_v_temp[idx_y][x + 4];
                }
                else
                {
                    sao_pic_rec_test_u[y][x] = sao_test_u[idx_y][x - 28];
                    sao_pic_rec_test_v[y][x] = sao_test_v[idx_y][x - 28];
                }

            }
        }

        int inrow_offset=lcu_pos_inrow * 3;

        LOOP_yhy_5_Y:for (int y = 0; y < 3; y++)
        {
#pragma HLS pipeline 
            int idx_y=y+61;
            int idx_y1=inrow_offset+y;
            LOOP_yhy_5_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                rfc_line_y[rfc_line_use_flag][idx_y1][x] = sao_test_y[idx_y][x];
            }
        }

        //最后一列
        if(0 == lcu_2rfc_param->lcu_use_right)
        {

            LOOP_yhy_6_Y:for (int y = 0; y < 56; y++)
            {
#pragma HLS pipeline
                int idx_y=y+5;
                LOOP_yhy_6_X:for (int x = 0; x < 64; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_y_last[y][x] = sao_test_y[idx_y][x + 5];
                }
            }

            for (int y = 0; y < 28; y++)
            {
#pragma HLS pipeline
                int idx_y1=y+4;
                for (int x = 0; x < 32; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_u_last[y][x] = sao_test_u[idx_y1][x + 4];
                    sao_pic_rec_test_v_last[y][x] = sao_test_v[idx_y1][x + 4];
                }
            }

            LOOP_yhy_7_Y:for (int y = 0; y < 3; y++)
            {
#pragma HLS pipeline
                int idx_y=y + 61;
                int idx_y1=inrow_offset+y;
                LOOP_yhy_7_X:for (int x = 64; x < 69; x++)
                {
#pragma HLS unroll
                    rfc_line_y[rfc_line_use_flag][idx_y1][x] = sao_test_y[idx_y][x];
                }
            }

            lcu_2rfc_param->lcu_positon = FIRST_ROW_LAST_COL;

            return;

        }

        LOOP_yhy_8_Y:for (int y = 0; y < 69; y++)
        {
#pragma HLS pipeline 
            LOOP_yhy_8_X:for (int x = 0; x < 69; x++)
            {
#pragma HLS unroll
                sao_test_y_temp[y][x] = sao_test_y[y][x];

                if(y<36 && x<36)
                {
                    sao_test_u_temp[y][x] = sao_test_u[y][x];
                    sao_test_v_temp[y][x] = sao_test_v[y][x];
                }
            }
        }

        lcu_2rfc_param->lcu_positon = FIRST_ROW_NOT_FIRST_COL_NOT_LAST_COL;

        return;
    }
    //其余行非第一列
    else if (lcu_2rfc_param->lcu_use_down != 0)
    {

        sao_2rtl_param->lcu_luma_width=64;
        sao_2rtl_param->lcu_luma_height=64;
        sao_2rtl_param->lcu_chroma_width=32;
        sao_2rtl_param->lcu_chroma_height=32;

        LOOP_yhy_9_Y:for (int y = 3; y < 64; y++)
        {
#pragma HLS pipeline
            int idx_y=y-3;
            LOOP_yhy_9_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                if(x<59)
                {
                    sao_pic_rec_test_y[y][x] = sao_test_y_temp[idx_y][x + 5];
                }
                else
                {
                    sao_pic_rec_test_y[y][x] = sao_test_y[idx_y][x - 59];
                }
            }
        }

        int inrow_offset=lcu_pos_inrow * 3;
        int inrow_offset_1=(lcu_pos_inrow - 1) * 3;
        int rfc_line_use_flag_no=!rfc_line_use_flag;

        LOOP_yhy_10_Y:for (int y = 0; y < 3; y++)
        {
#pragma HLS pipeline
            int idx_y=y+61;
            int idx_y1=inrow_offset+y;
            int idx_y2=inrow_offset_1+y;
            LOOP_yhy_10_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                if(x<59)
                {
                    sao_pic_rec_test_y[y][x] = rfc_line_y[rfc_line_use_flag_no][idx_y2][x + 5];
                }
                else
                {
                    sao_pic_rec_test_y[y][x] = rfc_line_y[rfc_line_use_flag_no][idx_y1][x - 59];
                }
                
            }
        }

        LOOP_yhy_11_Y:for (int y = 0; y < 3; y++)
        {
#pragma HLS pipeline
            int idx_y=y+61;
            int idx_y1=inrow_offset+y;
            LOOP_yhy_11_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                rfc_line_y[rfc_line_use_flag][idx_y1][x] = sao_test_y[idx_y][x];
            }
        }

        LOOP_yhy_12_Y:for (int y = 0; y < 32; y++)
        {
#pragma HLS pipeline
            LOOP_yhy_12_X:for (int x = 0; x < 32; x++)
            {
#pragma HLS unroll
                if(x<28)
                {
                    sao_pic_rec_test_u[y][x] = sao_test_u_temp[y][x + 4];
                    sao_pic_rec_test_v[y][x] = sao_test_v_temp[y][x + 4];
                }
                else
                {
                    sao_pic_rec_test_u[y][x] = sao_test_u[y][x - 28];
                    sao_pic_rec_test_v[y][x] = sao_test_v[y][x - 28];
                }
            }
        }

        //最后一列
        if (0 == lcu_2rfc_param->lcu_use_right)
        {

            LOOP_yhy_13_Y:for (int y = 3; y < 64; y++)
            {
#pragma HLS pipeline
                int idx_y=y-3;
                LOOP_yhy_13_X:for (int x = 0; x < 64; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_y_last[y][x] = sao_test_y[idx_y][x + 5];
                }
            }

            for (int y = 0; y < 3; y++)
            {
#pragma HLS pipeline
                int idx_y1=inrow_offset+y;
                for (int x = 0; x < 64; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_y_last[y][x] = rfc_line_y[rfc_line_use_flag_no][idx_y1][x + 5];
                }
            }

            LOOP_yhy_14_Y:for (int y = 0; y < 32; y++)
            {
#pragma HLS pipeline
                LOOP_yhy_14_X:for (int x = 0; x < 32; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_u_last[y][x] = sao_test_u[y][x + 4];
                    sao_pic_rec_test_v_last[y][x] = sao_test_v[y][x + 4];
                }
            }

            LOOP_yhy_15_Y:for (int y = 0; y < 3; y++)
            {
#pragma HLS pipeline
                int idx_y=y+61;
                int idx_y1=inrow_offset+y;
                LOOP_yhy_15_X:for (int x = 64; x < 69; x++)
                {
#pragma HLS unroll
                    rfc_line_y[rfc_line_use_flag][idx_y1][x] = sao_test_y[idx_y][x];
                }
            }

            lcu_2rfc_param->lcu_positon = OTHER_ROW_LAST_COL;

            return;
        }

        LOOP_yhy_16_Y:for (int y = 0; y < 69; y++)
        {
#pragma HLS pipeline
            LOOP_yhy_16_X:for (int x = 0; x < 69; x++)
            {
#pragma HLS unroll
                sao_test_y_temp[y][x] = sao_test_y[y][x];

                if(y<36 && x<36)
                {
                    sao_test_u_temp[y][x] = sao_test_u[y][x];
                    sao_test_v_temp[y][x] = sao_test_v[y][x];
                }
            }
        }

        lcu_2rfc_param->lcu_positon = OTHER_ROW_NOT_FIRST_COL_NOT_LAST_COL;

        return;
    }
    //最后一行非第一列
    else if (0 == lcu_2rfc_param->lcu_use_down)
    {
        sao_2rtl_param->lcu_luma_width=64;
        sao_2rtl_param->lcu_luma_height=72;
        sao_2rtl_param->lcu_chroma_width=32;
        sao_2rtl_param->lcu_chroma_height=36;

        LOOP_yhy_17_Y:for (int y = 3; y < 72; y++)
        {
#pragma HLS pipeline
            int idx_y=y-3;
            LOOP_yhy_17_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                if(x<59)
                {
                    sao_pic_rec_test_y[y][x] = sao_test_y_temp[idx_y][x + 5];
                }
                else
                {
                    sao_pic_rec_test_y[y][x] = sao_test_y[idx_y][x - 59];
                }
            }
        }

        int inrow_offset=lcu_pos_inrow * 3;
        int inrow_offset_1=(lcu_pos_inrow - 1) * 3;

        int rfc_line_use_flag_no=!rfc_line_use_flag;

        LOOP_yhy_18_Y:for (int y = 0; y < 3; y++)
        {
#pragma HLS pipeline
            int idx_y1=inrow_offset+y;
            int idx_y2=inrow_offset_1+y;
            LOOP_yhy_18_X:for (int x = 0; x < 64; x++)
            {
#pragma HLS unroll
                if(x<59)
                {
                    sao_pic_rec_test_y[y][x] = rfc_line_y[rfc_line_use_flag_no][idx_y2][x + 5];
                }
                else
                {
                    sao_pic_rec_test_y[y][x] = rfc_line_y[rfc_line_use_flag_no][idx_y1][x - 59];
                }
                
            }
        }

        LOOP_yhy_19_Y:for (int y = 0; y < 36; y++)
        {
#pragma HLS pipeline
            LOOP_yhy_19_X:for (int x = 0; x < 32; x++)
            {
#pragma HLS unroll
                if(x<28)
                {
                    sao_pic_rec_test_u[y][x] = sao_test_u_temp[y][x + 4];
                    sao_pic_rec_test_v[y][x] = sao_test_v_temp[y][x + 4];
                }
                else
                {
                    sao_pic_rec_test_u[y][x] = sao_test_u[y][x - 28];
                    sao_pic_rec_test_v[y][x] = sao_test_v[y][x - 28];
                }
            }
        }

        //最后一列
        if (0 == lcu_2rfc_param->lcu_use_right)
        {

            LOOP_yhy_20_Y:for (int y = 3; y < 72; y++)
            {
#pragma HLS pipeline
                int idx_y=y-3;
                LOOP_yhy_20_X:for (int x = 0; x < 64; x++)
                {
#pragma HLS unroll

                    sao_pic_rec_test_y_last[y][x] = sao_test_y[idx_y][x + 5];
                }
            }


            for (int y = 0; y < 3; y++)
            {
#pragma HLS pipeline
                int idx_y1=inrow_offset+y;
                for (int x = 0; x < 64; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_y_last[y][x] = rfc_line_y[rfc_line_use_flag_no][idx_y1][x + 5];
                }
            }

            LOOP_yhy_21_Y:for (int y = 0; y < 36; y++)
            {
#pragma HLS pipeline
                LOOP_yhy_21_X:for (int x = 0; x < 32; x++)
                {
#pragma HLS unroll
                    sao_pic_rec_test_u_last[y][x] = sao_test_u[y][x + 4];
                    sao_pic_rec_test_v_last[y][x] = sao_test_v[y][x + 4];
                }
            }

            lcu_2rfc_param->lcu_positon = LAST_ROW_LAST_COL;

            return;
        }

        LOOP_yhy_22_Y:for (int y = 0; y < 69; y++)
        {
#pragma HLS pipeline
            LOOP_yhy_22_X:for (int x = 0; x < 69; x++)
            {
#pragma HLS unroll
                sao_test_y_temp[y][x] = sao_test_y[y][x];

                if(y<36 && x<36)
                {
                    sao_test_u_temp[y][x] = sao_test_u[y][x];
                    sao_test_v_temp[y][x] = sao_test_v[y][x];
                }
            }
        }

        lcu_2rfc_param->lcu_positon = LAST_ROW_NOT_FIRST_COL_NOT_LAST_COL;

        return;
    }
    else
    {
        //防止综合异常退出
        sao_2rtl_param->lcu_luma_width=0;
        sao_2rtl_param->lcu_luma_height=0;
        sao_2rtl_param->lcu_chroma_width=0;
        sao_2rtl_param->lcu_chroma_height=0;
    }
    return;
}
/***************************************************************************************************************/
/*************************************************sao_output_rerange函数重构***************************************/

void sao_output_rearrange_skip(int max_smb_num, int lcu_cnt, U6 pic_width_in_lcu, int pix_y_luma, int pix_x_luma, int pix_y_chroma,
    int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param, SAO_2RTL_PARAM_T* sao_2rtl_param)
{

   

    int lcu_pos_inrow = (max_smb_num - lcu_cnt - 1) % pic_width_in_lcu;
    int rfc_line_use_flag = ((max_smb_num - lcu_cnt - 1) / pic_width_in_lcu) % 2;

    //第一列数据：只有copy的操作
    if (0 == lcu_2rfc_param->lcu_use_left)
    {
        sao_2rtl_param->lcu_luma_width = 0;
        sao_2rtl_param->lcu_luma_height = 0;
        sao_2rtl_param->lcu_chroma_width = 0;
        sao_2rtl_param->lcu_chroma_height = 0;

    

    int inrow_offset = lcu_pos_inrow * 3;



lcu_2rfc_param->lcu_positon = FIRST_COL;

return;
    }
    //第一行非第一列
    else if (0 == lcu_2rfc_param->lcu_use_up)
    {

        sao_2rtl_param->lcu_luma_width = 64;
        sao_2rtl_param->lcu_luma_height = 56;
        sao_2rtl_param->lcu_chroma_width = 32;
        sao_2rtl_param->lcu_chroma_height = 28;

   



int inrow_offset = lcu_pos_inrow * 3;



//最后一列
if (0 == lcu_2rfc_param->lcu_use_right)
{







lcu_2rfc_param->lcu_positon = FIRST_ROW_LAST_COL;

return;

}



lcu_2rfc_param->lcu_positon = FIRST_ROW_NOT_FIRST_COL_NOT_LAST_COL;

return;
    }
    //其余行非第一列
    else if (lcu_2rfc_param->lcu_use_down != 0)
    {

        sao_2rtl_param->lcu_luma_width = 64;
        sao_2rtl_param->lcu_luma_height = 64;
        sao_2rtl_param->lcu_chroma_width = 32;
        sao_2rtl_param->lcu_chroma_height = 32;

   

    int inrow_offset = lcu_pos_inrow * 3;
    int inrow_offset_1 = (lcu_pos_inrow - 1) * 3;
    int rfc_line_use_flag_no = !rfc_line_use_flag;







//最后一列
if (0 == lcu_2rfc_param->lcu_use_right)
{









lcu_2rfc_param->lcu_positon = OTHER_ROW_LAST_COL;

return;
}



lcu_2rfc_param->lcu_positon = OTHER_ROW_NOT_FIRST_COL_NOT_LAST_COL;

return;
    }
    //最后一行非第一列
    else if (0 == lcu_2rfc_param->lcu_use_down)
    {
        sao_2rtl_param->lcu_luma_width = 72;
        sao_2rtl_param->lcu_luma_height = 64;
        sao_2rtl_param->lcu_chroma_width = 36;
        sao_2rtl_param->lcu_chroma_height = 32;

   

    int inrow_offset = lcu_pos_inrow * 3;
    int inrow_offset_1 = (lcu_pos_inrow - 1) * 3;

    int rfc_line_use_flag_no = !rfc_line_use_flag;





//最后一列
if (0 == lcu_2rfc_param->lcu_use_right)
{








lcu_2rfc_param->lcu_positon = LAST_ROW_LAST_COL;

return;
}



lcu_2rfc_param->lcu_positon = LAST_ROW_NOT_FIRST_COL_NOT_LAST_COL;

return;
    }
    else
    {
        //防止综合异常退出
        sao_2rtl_param->lcu_luma_width = 0;
        sao_2rtl_param->lcu_luma_height = 0;
        sao_2rtl_param->lcu_chroma_width = 0;
        sao_2rtl_param->lcu_chroma_height = 0;
    }
    return;
}
/***************************************************************************************************************/
/*************************************************rfc_run函数重构************************************************/

#ifndef __SYNTHESIS__
void rfc_run(int pix_y_luma, int pix_x_luma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param,
    pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
    pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32],
    pel* y_rec, pel* u_rec, pel* v_rec,
    pel* y_rfcBin, u16* y_rfcPrm, pel* u_rfcBin, u16* u_rfcPrm, pel* v_rfcBin, u16* v_rfcPrm)
{
    int pix_y_chroma = pix_y_luma >> 1;
    int pix_x_chroma = pix_x_luma >> 1;
    CPR m_cpr1;

    switch (lcu_2rfc_param->lcu_positon)
    {
    case 0:
    {
    }
    break;
    case 1:
    {
        //compress luma 最后一列
        m_cpr1.proc_72x64(sao_pic_rec_test_y_last, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma + j] =
                    rfcBinBufferFra[pix_y_luma + i][pix_x_luma + j];
                //复制重组数据到DDR
                y_rec[(pix_y_luma + i) * rec_stride_luma + pix_x_luma + j] = sao_pic_rec_test_y_last[i][j];
            }
        }
        for (int i = 0; i < 56 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] =
                    rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma) / 32 + j];
            }
        }

        //compress chroma 最后一列
        m_cpr1.proc_36x32(sao_pic_rec_test_u_last, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 28);

        m_cpr1.proc_36x32(sao_pic_rec_test_v_last, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 28);

        for (int i = 0; i < 28; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma + j];
                //复制重组数据到DDR
                u_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] = sao_pic_rec_test_u_last[i][j];
                v_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] = sao_pic_rec_test_v_last[i][j];
            }
        }
        for (int i = 0; i < 28 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    case 2:
    {
        //compress luma
        m_cpr1.proc_72x64(sao_pic_rec_test_y, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] =
                    rfcBinBufferFra[pix_y_luma + i][pix_x_luma - 64 + j];
                //复制重组数据到DDR
                y_rec[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] = sao_pic_rec_test_y[i][j];
            }
        }
        for (int i = 0; i < 56 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] =
                    rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma - 64) / 32 + j];
            }
        }
        //compress chroma
        m_cpr1.proc_36x32(sao_pic_rec_test_u, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 28);

        m_cpr1.proc_36x32(sao_pic_rec_test_v, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 28);

        for (int i = 0; i < 28; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma - 32 + j];
                //复制重组数据到DDR
                u_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = sao_pic_rec_test_u[i][j];
                v_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] = sao_pic_rec_test_v[i][j];
            }
        }
        for (int i = 0; i < 28 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
    }
    break;
    case 3:
    {
        //compress luma:最后一列
        m_cpr1.proc_72x64(sao_pic_rec_test_y_last, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma + j] =
                    rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma + j];
                //复制重组数据到DDR
                y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma + j] = sao_pic_rec_test_y_last[i][j];
            }
        }
        for (int i = 0; i < 64 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma) / 32 + j];
            }
        }

        //compress chroma:最后一列
        m_cpr1.proc_36x32(sao_pic_rec_test_u_last, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 32);

        m_cpr1.proc_36x32(sao_pic_rec_test_v_last, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 32);

        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_u[pix_y_chroma - 4 + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_v[pix_y_chroma - 4 + i][pix_x_chroma + j];
                //复制重组数据到DDR
                u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] = sao_pic_rec_test_u_last[i][j];
                v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] = sao_pic_rec_test_v_last[i][j];
            }
        }
        for (int i = 0; i < 32 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    case 4:
    {
        //compress luma
        m_cpr1.proc_72x64(sao_pic_rec_test_y, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] =
                    rfcBinBufferFra[(pix_y_luma - 8) + i][pix_x_luma - 64 + j];
                //复制重组数据到DDR
                y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma - 64 + j] = sao_pic_rec_test_y[i][j];
            }
        }
        for (int i = 0; i < 64 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];
            }
        }

        //compress chroma
        m_cpr1.proc_36x32(sao_pic_rec_test_u, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);

        m_cpr1.proc_36x32(sao_pic_rec_test_v, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);

        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_u[(pix_y_chroma - 4) + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_v[(pix_y_chroma - 4) + i][pix_x_chroma - 32 + j];

                //复制重组数据到DDR
                u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j] = sao_pic_rec_test_u[i][j];
                v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j] = sao_pic_rec_test_v[i][j];
            }
        }
        for (int i = 0; i < 32 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
    }
    break;
    case 5:
    {
        //数据压缩、复制重组及压缩数据到DDR
        //compress luma:最后一列
        m_cpr1.proc_72x64(sao_pic_rec_test_y_last, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma + j] =
                    rfcBinBufferFra[(pix_y_luma - 8) + i][pix_x_luma + j];
                //复制重组数据到DDR
                y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma + j] = sao_pic_rec_test_y_last[i][j];
            }
        }
        for (int i = 0; i < 72 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma) / 32 + j];
            }
        }

        //compress chroma:最后一列
        m_cpr1.proc_36x32(sao_pic_rec_test_u_last, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 36);

        m_cpr1.proc_36x32(sao_pic_rec_test_v_last, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 36);

        for (int i = 0; i < 36; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_u[(pix_y_chroma - 4) + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_v[(pix_y_chroma - 4) + i][pix_x_chroma + j];
                //复制重组数据到DDR
                u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] = sao_pic_rec_test_u_last[i][j];
                v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] = sao_pic_rec_test_v_last[i][j];
            }
        }
        for (int i = 0; i < 36 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    case 6:
    {
        //compress luma
        m_cpr1.proc_72x64(sao_pic_rec_test_y, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] =
                    rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma - 64 + j];
                //复制重组数据到DDR
                y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma - 64 + j] = sao_pic_rec_test_y[i][j];
            }
        }
        for (int i = 0; i < 72 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }

        //compress chroma
        m_cpr1.proc_36x32(sao_pic_rec_test_u, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);

        m_cpr1.proc_36x32(sao_pic_rec_test_v, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);

        for (int i = 0; i < 36; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_u[pix_y_chroma - 4 + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_v[pix_y_chroma - 4 + i][pix_x_chroma - 32 + j];
                //复制重组数据到DDR
                u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j] = sao_pic_rec_test_u[i][j];
                v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j] = sao_pic_rec_test_v[i][j];

            }
        }
        for (int i = 0; i < 36 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
    }
    break;
    default:
        break;
    }
    return;
}
#endif
#ifndef __SYNTHESIS__
void rfc_run_skip(int pix_y_luma, int pix_x_luma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param,
    pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
    pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32],
    pel* y_rec, pel* u_rec, pel* v_rec,
    pel* y_rfcBin, u16* y_rfcPrm, pel* u_rfcBin, u16* u_rfcPrm, pel* v_rfcBin, u16* v_rfcPrm)
{
    int pix_y_chroma = pix_y_luma >> 1;
    int pix_x_chroma = pix_x_luma >> 1;
    CPR m_cpr1;

    switch (lcu_2rfc_param->lcu_positon)
    {
    case 0:
    {
    }
    break;
    case 1:
    {
        //compress luma 最后一列
        for (int i = 0; i < 56; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                
              
                sao_pic_rec_test_y_last[i][j] = y_rec[(pix_y_luma + i) * rec_stride_luma + pix_x_luma + j];
            }
        }
        m_cpr1.proc_72x64(sao_pic_rec_test_y_last, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma + j] =
                    rfcBinBufferFra[pix_y_luma + i][pix_x_luma + j];
                //复制重组数据到DDR
              
            }
        }
        for (int i = 0; i < 56 / 4; i++) {
            for (int j = 0; j < 64 / 32; j++) {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] =
                    rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma) / 32 + j];
            }
        }

        //compress chroma 最后一列
        for (int i = 0; i < 28; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                sao_pic_rec_test_u_last[i][j] = u_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j];
                sao_pic_rec_test_v_last[i][j] = v_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j];
               
            }
        }
        m_cpr1.proc_36x32(sao_pic_rec_test_u_last, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma, 32, 28);

        m_cpr1.proc_36x32(sao_pic_rec_test_v_last, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma, 32, 28);

        for (int i = 0; i < 28; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma + j];
                
            }
        }
        for (int i = 0; i < 28 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    case 2:
    {
        //compress luma
        for (int i = 0; i < 56; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                
                sao_pic_rec_test_y[i][j] = y_rec[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] ;
            }
        }
        m_cpr1.proc_72x64(sao_pic_rec_test_y, 0, 64, 56);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma, 64, 56);

        for (int i = 0; i < 56; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma + i) * rec_stride_luma + pix_x_luma - 64 + j] =
                    rfcBinBufferFra[pix_y_luma + i][pix_x_luma - 64 + j];
               
            }
        }
        for (int i = 0; i < 56 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[(pix_y_luma / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] =
                    rfcPrmBufferFra[pix_y_luma / 4 + i][(pix_x_luma - 64) / 32 + j];
            }
        }
        //compress chroma
        for (int i = 0; i < 28; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                
                sao_pic_rec_test_u[i][j] = u_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] ;
                sao_pic_rec_test_v[i][j] = v_rec[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] ;
            }
        }
        m_cpr1.proc_36x32(sao_pic_rec_test_u, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma, 32, 28);

        m_cpr1.proc_36x32(sao_pic_rec_test_v, 0, 32, 28);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 28);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma, 32, 28);

        for (int i = 0; i < 28; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_u[pix_y_chroma + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_v[pix_y_chroma + i][pix_x_chroma - 32 + j];
                
            }
        }
        for (int i = 0; i < 28 / 4; i++) {
            for (int j = 0; j < 32 / 32; j++) {
                u_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_u[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[(pix_y_chroma / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_v[pix_y_chroma / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
    }
    break;
    case 3:
    {
        //compress luma:最后一列
        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                
                sao_pic_rec_test_y_last[i][j] = y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma + j] ;
            }
        }
        m_cpr1.proc_72x64(sao_pic_rec_test_y_last, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma + j] =
                    rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma + j];
                
            }
        }
        for (int i = 0; i < 64 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma) / 32 + j];
            }
        }

        //compress chroma:最后一列
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                
                 sao_pic_rec_test_u_last[i][j] = u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] ;
                 sao_pic_rec_test_v_last[i][j] = v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] ;
            }
        }
        m_cpr1.proc_36x32(sao_pic_rec_test_u_last, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 32);

        m_cpr1.proc_36x32(sao_pic_rec_test_v_last, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 32);

        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_u[pix_y_chroma - 4 + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_v[pix_y_chroma - 4 + i][pix_x_chroma + j];
                
            }
        }
        for (int i = 0; i < 32 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    case 4:
    {
        //compress luma
        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                
                 sao_pic_rec_test_y[i][j] = y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma - 64 + j] ;
            }
        }
        m_cpr1.proc_72x64(sao_pic_rec_test_y, 0, 64, 64);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 64);

        for (int i = 0; i < 64; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] =
                    rfcBinBufferFra[(pix_y_luma - 8) + i][pix_x_luma - 64 + j];
                
            }
        }
        for (int i = 0; i < 64 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];
            }
        }

        //compress chroma
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
               
                 sao_pic_rec_test_u[i][j] = u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j];
                 sao_pic_rec_test_v[i][j] = v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j] ;
            }
        }
        m_cpr1.proc_36x32(sao_pic_rec_test_u, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);

        m_cpr1.proc_36x32(sao_pic_rec_test_v, 0, 32, 32);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 32);

        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_u[(pix_y_chroma - 4) + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_v[(pix_y_chroma - 4) + i][pix_x_chroma - 32 + j];

                
            }
        }
        for (int i = 0; i < 32 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
    }
    break;
    case 5:
    {
        //数据压缩、复制重组及压缩数据到DDR
        //compress luma:最后一列
        for (int i = 0; i < 72; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                
                 sao_pic_rec_test_y_last[i][j] = y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma + j] ;
            }
        }
        m_cpr1.proc_72x64(sao_pic_rec_test_y_last, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma + j] =
                    rfcBinBufferFra[(pix_y_luma - 8) + i][pix_x_luma + j];
                
            }
        }
        for (int i = 0; i < 72 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma) / 32 + j];
            }
        }

        //compress chroma:最后一列
        for (int i = 0; i < 36; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                
                sao_pic_rec_test_u_last[i][j] = u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j];
                sao_pic_rec_test_v_last[i][j] = v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma + j] ;
            }
        }
        m_cpr1.proc_36x32(sao_pic_rec_test_u_last, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma, pix_y_chroma - 4, 32, 36);

        m_cpr1.proc_36x32(sao_pic_rec_test_v_last, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma, pix_y_chroma - 4, 32, 36);

        for (int i = 0; i < 36; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_u[(pix_y_chroma - 4) + i][pix_x_chroma + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma + j] =
                    rfcBinBufferFra_v[(pix_y_chroma - 4) + i][pix_x_chroma + j];
                
            }
        }
        for (int i = 0; i < 36 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma) / 32 + j];
            }
        }
    }
    case 6:
    {
        //compress luma
        for (int i = 0; i < 72; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                
                sao_pic_rec_test_y[i][j] = y_rec[(pix_y_luma + i - 8) * rec_stride_luma + pix_x_luma - 64 + j] ;
            }
        }
        m_cpr1.proc_72x64(sao_pic_rec_test_y, 0, 64, 72);
        m_cpr1.cpyDatBit(rfcBinBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra, pix_x_luma - 64, pix_y_luma - 8, 64, 72);

        for (int i = 0; i < 72; i++)
        {
            for (int j = 0; j < 64; j++)
            {
                //复制压缩数据到DDR
                y_rfcBin[(pix_y_luma - 8 + i) * rec_stride_luma + pix_x_luma - 64 + j] =
                    rfcBinBufferFra[pix_y_luma - 8 + i][pix_x_luma - 64 + j];
                
            }
        }
        for (int i = 0; i < 72 / 4; i++)
        {
            for (int j = 0; j < 64 / 32; j++)
            {
                y_rfcPrm[((pix_y_luma - 8) / 4 + i) * rec_stride_luma / 32 + (pix_x_luma - 64) / 32 + j] =
                    rfcPrmBufferFra[(pix_y_luma - 8) / 4 + i][(pix_x_luma - 64) / 32 + j];

            }
        }

        //compress chroma
        for (int i = 0; i < 36; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                
                sao_pic_rec_test_u[i][j] = u_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j];
                sao_pic_rec_test_v[i][j] = v_rec[(pix_y_chroma + i - 4) * rec_stride_chroma + pix_x_chroma - 32 + j];

            }
        }
        m_cpr1.proc_36x32(sao_pic_rec_test_u, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_u, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);

        m_cpr1.proc_36x32(sao_pic_rec_test_v, 0, 32, 36);
        m_cpr1.cpyDatBit(rfcBinBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);
        m_cpr1.cpyDatPrm(rfcPrmBufferFra_v, pix_x_chroma - 32, pix_y_chroma - 4, 32, 36);

        for (int i = 0; i < 36; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                //复制压缩数据到DDR
                u_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_u[pix_y_chroma - 4 + i][pix_x_chroma - 32 + j];
                v_rfcBin[(pix_y_chroma - 4 + i) * rec_stride_chroma + pix_x_chroma - 32 + j] =
                    rfcBinBufferFra_v[pix_y_chroma - 4 + i][pix_x_chroma - 32 + j];
               

            }
        }
        for (int i = 0; i < 36 / 4; i++)
        {
            for (int j = 0; j < 32 / 32; j++)
            {
                u_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_u[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
                v_rfcPrm[((pix_y_chroma - 4) / 4 + i) * rec_stride_chroma / 32 + (pix_x_chroma - 32) / 32 + j] =
                    rfcPrmBufferFra_v[(pix_y_chroma - 4) / 4 + i][(pix_x_chroma - 32) / 32 + j];
            }
        }
    }
    break;
    default:
        break;
    }
    return;
}
#endif
/***************************************************************************************************************/