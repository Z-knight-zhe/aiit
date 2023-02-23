#include "com_def.h"
#include"enc_def.h"
#include "type_def.h"
#include "md_cmod.h"
#include "md2.h"
/*#include<iostream>
#include <cstdlib>
#include<string>
#include<fstream>
using namespace std;*/

////////////////////////////////////////////////////////////////////////
//#define DEBUG_PRINT_MD_COST 1
//#define DEBUG_PRINT_PU_COST 1

extern int g_cost_idx;
#define PRINT_MD_COST \
	PRINT_MD_COST_C(cost_best)  

#define PRINT_MD_COST_C(cost_temp) \
	PRINT_MD_COST_P1(x0,y0,cu_width,cu_height,cost_temp)  

#define PRINT_MD_COST_P0(x0,y0,cu_width,cu_height,cost_best)  \
	printf(" -- %s(%d,%d,%d,%d): cost=%d\n", \
		__func__, \
		x0,y0,cu_width,cu_height, \
		cost_best); \
	fflush(stdout);

#define PRINT_MD_COST_P1(x0,y0,cu_width,cu_height,cost_best)  \
	/*if (x0<64&&y0<64)*/ \
	printf(" -- %d(%3d,%3d,%3d,%3d): cost=%6d func=%s\n", \
		g_cost_idx++, \
		x0,y0,cu_width,cu_height, \
		cost_best, __func__); \
	fflush(stdout);
////////////////////////////////////////////////////////////////////////


#define ENABLE_MD32C_BB 0
#define MD32C_TB 0
#if MD32C_TB
#define MD32C_TB_MAXCNT 10
#define MD32C_TB_LOOPTEST 1
#include "iime.h"
#endif

#ifdef VIVADO_HLS
#define MD_PRINT 0 ///////////////////////////it is used to debug
#else
#define MD_PRINT 0 ///////////////////////////it is used to debug
#define MD_PRINT_ENH 0 ///////////////////////////it is used to debug
#endif

#ifndef __MERLIN_MD1_H__

#define __MERLIN_MD1_H__

#ifdef MD_CMOD
#define MD_MAX_RDO_NUM 2
#define MD_MAX_RDO_NUM_INTER 1
#else
#define MD_MAX_RDO_NUM_INTER 3
#if EIPM
#define MD_MAX_RDO_EIPM_NUM 2
#else
#define MD_MAX_RDO_EIPM_NUM 0
#endif
#if INTERPF
#define MD_MAX_RDO_INTERPF_NUM 1
#else
#define MD_MAX_RDO_INTERPF_NUM 0
#endif

#if USE_EIPM_RMD
#if USE_SPEED_LEVEL
#define MD_MAX_RDO_NUM (IPD_RDO_CNT + EXTRA_RDO_MODE)
#else
#define MD_MAX_RDO_NUM 5

#define size_8x8_rdo_num	MD_MAX_RDO_NUM
#define size_8x16_rdo_num	MD_MAX_RDO_NUM
#define size_16x8_rdo_num	MD_MAX_RDO_NUM
#define MAX_RDO_NUM_SIZE(m, n)	size_##m##x##n##_rdo_num

#define size_8x8_rdo_num_inter	MD_MAX_RDO_NUM_INTER
#define size_8x16_rdo_num_inter	MD_MAX_RDO_NUM_INTER
#define size_16x8_rdo_num_inter	MD_MAX_RDO_NUM_INTER
#define MAX_RDO_NUM_INTER_SIZE(m, n)	size_##m##x##n##_rdo_num_inter

#define size_8x8_skip_mode_num		12
#define size_8x16_skip_mode_num		12
#define size_16x8_skip_mode_num		12
#define SKIP_MODE_NUM_SIZE(m, n)	size_##m##x##n##_skip_mode_num
#endif
#else
#define MD_MAX_RDO_NUM (5 + MD_MAX_RDO_EIPM_NUM)
#endif


#endif


typedef struct _SKIP_MERGE_INPUT SKIP_MERGE_INPUT;
typedef s16 pel;
typedef int CPMV;
typedef u32 SBAC_CTX_MODEL;
typedef long long COM_MTIME;
typedef struct SAOstatdata SAOStatData;
typedef int BOOL;
typedef struct _SKIP_RDO SKIP_RDO;
typedef struct _MERGE_RDO MERGE_RDO;
#if SUB_TMVP
typedef struct _CORE_SBTMVP {
	BOOL          sbTmvp_flag;
	BOOL          best_sbTmvp_flag;
	COM_MOTION    sbTmvp[SBTMVP_NUM];
	COM_MOTION    best_sbTmvp[SBTMVP_NUM];
	BOOL		  isValid;
} CORE_SBTMVP;
#endif

#if CUDQP_QP_MAP
int md_com_is_cu_dqp(COM_INFO* info);
#endif

#if INTERPF


void pred_inter_filter(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int component, int pfIdx, pel* pred_y, pel* pred_u, pel* pred_v,
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride);
void pred_inter_filter_luma_core(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, pel* pred_y,
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride);
void pred_inter_filter_chroma_core(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, pel* pred_u, pel* pred_v,
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride);
void pred_inter_filter_chroma_single_core(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, int component, pel* pred,
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int stride);
template<unsigned KERNEL_SIZE>
void bufferDim1To2(pel src[KERNEL_SIZE * KERNEL_SIZE], pel dist[KERNEL_SIZE][KERNEL_SIZE])
{
	pel* src_cur = src;
	for (int h = 0; h < KERNEL_SIZE; h++)
	{
		for (int w = 0; w < KERNEL_SIZE; w++)
		{
			dist[h][w] = src_cur[w];
		}
		src_cur += KERNEL_SIZE;
	}
}
template<unsigned KERNEL_SIZE>
void bufferDim2To1(pel dst[KERNEL_SIZE * KERNEL_SIZE], pel src[KERNEL_SIZE][KERNEL_SIZE])
{
	pel* dst_cur = dst;
	for (int h = 0; h < KERNEL_SIZE; h++)
	{
		for (int w = 0; w < KERNEL_SIZE; w++)
		{
			dst_cur[w] = src[h][w];
		}
		dst_cur += KERNEL_SIZE;
	}
}
template<unsigned KERNEL_SIZE>
void pred_inter_filter_luma(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, pel pred_y[KERNEL_SIZE][KERNEL_SIZE], pel pred_w[KERNEL_SIZE][KERNEL_SIZE],
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
	static pel buffer_dim1[KERNEL_SIZE * KERNEL_SIZE];
	bufferDim2To1<KERNEL_SIZE>(buffer_dim1, pred_y);
	pred_inter_filter_luma_core(x, y, cu_width, cu_height, pic_width_in_scu, pic_height_in_scu, bit_depth, pfIdx, buffer_dim1, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr, KERNEL_SIZE);
	bufferDim1To2<KERNEL_SIZE>(buffer_dim1, pred_w);
}

template<unsigned KERNEL_SIZE>
void pred_inter_filter_chroma(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, pel pred_u[KERNEL_SIZE][KERNEL_SIZE], pel pred_v[KERNEL_SIZE][KERNEL_SIZE],
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
	static pel buffer_dim1_U[KERNEL_SIZE * KERNEL_SIZE];
	static pel buffer_dim1_V[KERNEL_SIZE * KERNEL_SIZE];
	bufferDim2To1<KERNEL_SIZE>(buffer_dim1_U, pred_u);
	bufferDim2To1<KERNEL_SIZE>(buffer_dim1_V, pred_v);
	pred_inter_filter_chroma_core(x, y, cu_width, cu_height, pic_width_in_scu, pic_height_in_scu, bit_depth, pfIdx, buffer_dim1_U, buffer_dim1_V, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr, KERNEL_SIZE);
	bufferDim1To2<KERNEL_SIZE>(buffer_dim1_U, pred_u);
	bufferDim1To2<KERNEL_SIZE>(buffer_dim1_V, pred_v);
}

template<unsigned KERNEL_SIZE>
void pred_inter_filter_chroma_single(int x, int y, int cu_width, int cu_height, int pic_width_in_scu, int pic_height_in_scu, int bit_depth, int pfIdx, int component, pel pred[KERNEL_SIZE][KERNEL_SIZE],
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr)
{
	static pel buffer_dim1[KERNEL_SIZE * KERNEL_SIZE];
	bufferDim2To1<KERNEL_SIZE>(buffer_dim1, pred);
	pred_inter_filter_chroma_single_core(x, y, cu_width, cu_height, pic_width_in_scu, pic_height_in_scu, bit_depth, pfIdx, component, buffer_dim1, me_line_ptr, IP_buffer_ptr, MAP_SCU_IP_ptr, KERNEL_SIZE);
	bufferDim1To2<KERNEL_SIZE>(buffer_dim1, pred);
}

template<unsigned KERNEL_SIZE>
u8 fast_apply_interpf(U3 w, U3 h, s16* src1, s16 pred[KERNEL_SIZE][KERNEL_SIZE], s16 predInterPF[KERNEL_SIZE][KERNEL_SIZE], int s_src1)
{
#pragma HLS INLINE off
	int offset_s1 = 0;
	U8 i;
	U8 j;
	U64 distPred = 0, distInterPF = 0;
	u8 res = 0;
	for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
#pragma HLS pipeline
		for (j = 0; j < 64; j++) {
#pragma HLS UNROLL
			if (j < w)
			{
				distPred += COM_ABS16((src1[j + offset_s1] - pred[i][j]));
				distInterPF += COM_ABS16((src1[j + offset_s1] - predInterPF[i][j]));
			}

		}
		offset_s1 += s_src1;
	}
	if (distInterPF < distPred)
	{
		for (i = 0; i < h; i++) {
#pragma HLS LOOP_TRIPCOUNT max=64
#pragma HLS pipeline
			for (j = 0; j < 64; j++) {
#pragma HLS UNROLL
				if (j < w)
				{
					pred[i][j] = predInterPF[i][j];
				}

			}
		}
		res = 1;
	}
	return res;
}

u8 is_apply_interpf(int interpf_enable_flag, int w, int h);
#endif
extern void md1_com_check_split_mode_top(U14 x0, U14 y0, U8 cu_width, U8 cu_height, U13 pic_width, U13 pic_height, U2 split_mode, U1* do_split);
extern s64 prepare_cu_info_COM_combine_only_qt(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct);

extern s64 prepare_cu_info_COM_combine_only_qt_chroma(MD_FW* md_fw_ptr, s64 lambdaY,
	U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct);
extern void get_part_info_init(U11 pic_width_in_scu, U13 x, U13 y, U7 w, U7 h, U4 part_size, COM_PART_INFO* sub_info);
extern void get_part_info(U11 pic_width_in_scu, U13 x, U13 y, U7 w, U7 h, U4 part_size, COM_PART_INFO* sub_info);
extern void scaling_mv1(int ptr_cur, int ptr_cur_ref, int ptr_neb, int ptr_neb_ref, s32 mvp[MV_D], s32 mv[MV_D]);
extern void copy_mv(s32 dst[MV_D], const s32 src[MV_D]);
extern void create_motion(COM_MOTION* motion_dst, s32 mv_new[REFP_NUM][MV_D], s32 refi_new[REFP_NUM]);
extern int same_motion(COM_MOTION motion1, COM_MOTION motion2);
extern void copy_motion(COM_MOTION* motion_dst, COM_MOTION motion_src);
extern void create_motion_refp_0(COM_MOTION* motion_dst, s32 mv_new[MV_D], s32 refi_new);
extern void init_motion(COM_MOTION* motion_dst);
extern void md_com_split_get_part_structure(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud, U3 log2_culine, COM_SPLIT_STRUCT* split_struct);
extern void com_split_get_part_structure_only_bt(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud, U3 log2_culine, COM_SPLIT_STRUCT* split_struct);
extern void com_split_get_part_structure_only_qt(U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud, U3 log2_culine, COM_SPLIT_STRUCT* split_struct);
extern void com_split_get_part_structure(U3 split_mode, U14 x0, U14 y0, U8 cu_width, U8 cu_height, U12 cup, U4 cud, U3 log2_culine, COM_SPLIT_STRUCT* split_struct);
extern void com_split_get_split_rdo_order(U8 cu_width, U8 cu_height, U3 splits[MAX_SPLIT_NUM]);

void com_set_cons_pred_mode(U2 cons_pred_mode, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s,
	U8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))]);
void com_check_split_mode(U14 x0, U14 y0, MD_FW* md_fw_ptr, U1* split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3 qt_depth, U3 bet_depth, U3* num_split, U3 split_mode);
u8 com_constrain_pred_mode(U7 w, U7 h, U3 split, U2 slice_type);
void com_check_split_mode_only_qt(U14 x0, U14 y0, MD_FW* md_fw_ptr, U1* split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3 bet_depth, U3* num_split);
void md2_com_check_split_mode(U14 x0, U14 y0, MD_FW* md_fw_ptr, U1* split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U3* num_split, U3 split_mode);
void com_set_split_mode(U3 split_mode, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s,
	U8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))]);

void clip_pred(pel dst[4 * 4], const int w, const int h, int bit_depth);
void fetch_port_16_md2(
	// input
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif // ENABLE_BFRAME
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	SKIP_MERGE_INPUT* skip_in,
	ENC_FME_INFO_ALL* fme_mv_info,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	// Supplementary information
	int x_pos_pic, int y_pos_pic, int cu_width, int cu_height, int pic_width, int pic_height,
	// output
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir_16,
#endif // ENABLE_BFRAME
	pel org_y_16[16][16],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_16,
	SKIP_MERGE_INPUT_16* skip_in_16,
	ENC_FME_INFO_ALL_16* fme_mv_info_16,
	strFetch_ref_window_16* Fetch_Ref_window_ptr_16
);
void fetch_port_16_md2_tst(
	// input
#if ENABLE_BFRAME
	ME_MV_DIR* me_mv_dir,
#endif // ENABLE_BFRAME
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	SKIP_MERGE_INPUT* skip_in,
	ENC_FME_INFO_ALL* fme_mv_info,
	strFetch_ref_window* Fetch_Ref_window_ptr,
	// Supplementary information
	int x_pos_pic, int y_pos_pic, int cu_width, int cu_height, int pic_width, int pic_height,
	// output
#if ENABLE_BFRAME
	ME_MV_DIR_16* me_mv_dir_16,
#endif // ENABLE_BFRAME
	pel org_y_16[16][16],
	SKIP_MERGE_INPUT_16* skip_in_16,
	ENC_FME_INFO_ALL_16* fme_mv_info_16,
	strFetch_ref_window_16* Fetch_Ref_window_ptr_16
);

void print_s64(const s64& val);
void com_tbl_mc_c_hp(int ori_mv_x, int ori_mv_y, pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred,
	s16* pred, int w, int h, int bit_depth);
void com_tbl_mc_c_1(int ori_mv_x, int ori_mv_y, pel* ref, int gmv_x, int gmv_y, int s_ref, int s_pred, s16* pred,
	int w, int h, int bit_depth);
void get_IP_pixel_chroma(U7 width, U7 height, u16 avail_cu, U11 pic_width_in_scu, U11 pic_height_in_scu,
	U4 bit_depth, U2 ch_type, pel* left, pel* up, U12 x, U12 y,
	ENC_ME_LINE_MAP* me_line_ptr, IP_ARRAY* IP_buffer_ptr);
#ifdef MD_CMOD
int get_bit_est_intra_chroma(MD_COM_MODE_BEST* mod_info_curr_ptr, int cu_width, int cu_height, s16* coef_tmp/*[N_C - 1][1024]*/, u8 tscpm_enable_flag);
#else
s64 get_bit_est_intra_chroma(MD_COM_MODE_BEST* mod_info_curr_ptr, int cu_width, int cu_height, s16* coef_tmp/*[N_C - 1][1024]*/, u8 tscpm_enable_flag);
s64 get_bit_est_intra_chroma8(int cu_width, int cu_height, int est_bit_intra);
int enc_eco_xcoef_est_Chroma8(s16 coef[8][8], int cu_width_c, int cu_height_c, int num_nz);
s64 get_bit_est_intra_chroma_32( int cu_width_chroma, int cu_height_chroma,   int est_bit_intra);
int encode_intra_dir_c_est(u8 ipm, u8 ipm_l, u8 tscpm_enable_flag);
int enc_eco_xcoef_est_32(s16 coef[32][32], int cu_width, int cu_height, int num_nz);

#endif
int mode_cu_init_chroma(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array,
	MD_COM_MODE_BEST* bst_info_ptr, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status, U2* cons_pred_mode, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_INPUT* md_input_ptr
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);
void copy_to_cu_data_chroma(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1]);
void print_chroma(MD_COM_MODE_BEST* mod_info, int cnt, pel pred[2][16][16], pel resi[2][16][16]);
void print_cu_split(int x, int y, int w, int h, int tree, int num, int part, int xc, int yc, int last, int depth);

template<size_t SIZE>
void cu_data_cpy(ENC_CU_DATA_ARRAY_INNER* src, ENC_CU_DATA_ARRAY_SIZE<SIZE>* dst, int cu_width, int cu_height, int cdebug)
{
	int w_scu = cu_width >> 2;
	int h_scu = cu_height >> 2;
	if (cdebug)
	{
		for (int i = 0; i < cu_height; i++)
		{
			for (int j = 0; j < cu_width; j++)
			{
				assert(src->coef_y[i * cu_width + j] == dst->coef_y[i][j]);
				assert(src->reco_y[i * cu_width + j] == dst->reco_y[i][j]);
			}
		}
		for (int i = 0; i < h_scu; i++)
		{
			for (int j = 0; j < w_scu; j++)
			{
				for (int m = 0; m < 2; m++)
				{
					assert(src->refi[i * w_scu + j][m] == dst->refi[i * w_scu + j][m]);
					for (int n = 0; n < 2; n++)
					{
						assert(src->mv[i * w_scu + j][m][n] == dst->mv[i * w_scu + j][m][n]);
						assert(src->mvd[i * w_scu + j][m][n] == dst->mvd[i * w_scu + j][m][n]);
					}
				}
				assert(src->pb_part[i * w_scu + j] == dst->pb_part[i * w_scu + j]);
				assert(src->tb_part[i * w_scu + j] == dst->tb_part[i * w_scu + j]);
				assert(src->pred_mode[i * w_scu + j] == dst->pred_mode[i * w_scu + j]);
				assert(src->mvr_idx[i * w_scu + j] == dst->mvr_idx[i * w_scu + j]);
				assert(src->umve_flag[i * w_scu + j] == dst->umve_flag[i * w_scu + j]);
				assert(src->umve_idx[i * w_scu + j] == dst->umve_idx[i * w_scu + j]);
				assert(src->skip_idx[i * w_scu + j] == dst->skip_idx[i * w_scu + j]);
				assert(src->map_scu[i * w_scu + j] == dst->map_scu[i * w_scu + j]);
				assert(src->affine_flag[i * w_scu + j] == dst->affine_flag[i * w_scu + j]);
				assert(src->smvd_flag[i * w_scu + j] == dst->smvd_flag[i * w_scu + j]);
				assert(src->map_cu_mode[i * w_scu + j] == dst->map_cu_mode[i * w_scu + j]);
				assert(src->map_pb_tb_part[i * w_scu + j] == dst->map_pb_tb_part[i * w_scu + j]);
				assert(src->depth[i * w_scu + j] == dst->depth[i * w_scu + j]);
				assert(src->ipf_flag[i * w_scu + j] == dst->ipf_flag[i * w_scu + j]);
				assert(src->mvp_from_hmvp_flag[i * w_scu + j] == dst->mvp_from_hmvp_flag[i * w_scu + j]);
			}
		}
		for (int k = 0; k < 2; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					/*assert(src->mpm[k][i * w_scu + j] == dst->mpm[k][i * w_scu + j]);
					assert(src->ipm[k][i * w_scu + j] == dst->ipm[k][i * w_scu + j]);*/
				}
			}
		}
		for (int k = 0; k < 8; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					assert(src->mpm_ext[k][i * w_scu + j] == dst->mpm_ext[k][i * w_scu + j]);
				}
			}
		}
		for (int k = 0; k < 1; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					assert(src->num_nz_coef[k][i * w_scu + j] == dst->num_nz_coef[k][i * w_scu + j]);
				}
			}
		}
		assert(src->x_chroma == dst->x_chroma);
		assert(src->y_chroma == dst->y_chroma);
		assert(src->x_last == dst->x_last);
		assert(src->y_last == dst->y_last);
		assert(src->last_child == dst->last_child);
		for (int i = 0; i < cu_height; i++)
		{
			for (int j = 0; j < cu_width; j++)
			{
				assert(src->coef_y[i * cu_width + j] == dst->coef_y[i][j]);
				assert(src->reco_y[i * cu_width + j] == dst->reco_y[i][j]);
			}
		}
		//for (int i = 0; i < (cu_height >> 1); i++)
		//{
		//	for (int j = 0; j < (cu_width >> 1); j++)
		//	{
		//		assert(src->coef_u[i * (cu_width >> 1) + j] == dst->coef_u[i][j]);
		//		assert(src->coef_v[i * (cu_width >> 1) + j] == dst->coef_v[i][j]);
		//		assert(src->reco_u[i * (cu_width >> 1) + j] == dst->reco_u[i][j]);
		//		assert(src->reco_v[i * (cu_width >> 1) + j] == dst->reco_v[i][j]);
		//	}
		//}
	}
	else
	{
		for (int i = 0; i < h_scu; i++)
		{
			for (int j = 0; j < w_scu; j++)
			{
				for (int m = 0; m < 2; m++)
				{
					src->refi[i * w_scu + j][m] = dst->refi[i * w_scu + j][m];
					for (int n = 0; n < 2; n++)
					{
						src->mv[i * w_scu + j][m][n] = dst->mv[i * w_scu + j][m][n];
						src->mvd[i * w_scu + j][m][n] = dst->mvd[i * w_scu + j][m][n];
					}
				}
				src->pb_part[i * w_scu + j] = dst->pb_part[i * w_scu + j];
				src->tb_part[i * w_scu + j] = dst->tb_part[i * w_scu + j];
				src->pred_mode[i * w_scu + j] = dst->pred_mode[i * w_scu + j];
#if CUDQP_QP_MAP
				src->cu_luma_qp[i * w_scu + j] = dst->cu_luma_qp[i * w_scu + j];
#endif
				src->mvr_idx[i * w_scu + j] = dst->mvr_idx[i * w_scu + j];
				src->umve_flag[i * w_scu + j] = dst->umve_flag[i * w_scu + j];
				src->umve_idx[i * w_scu + j] = dst->umve_idx[i * w_scu + j];
				src->skip_idx[i * w_scu + j] = dst->skip_idx[i * w_scu + j];
				src->map_scu[i * w_scu + j] = dst->map_scu[i * w_scu + j];
				src->affine_flag[i * w_scu + j] = dst->affine_flag[i * w_scu + j];
				src->smvd_flag[i * w_scu + j] = dst->smvd_flag[i * w_scu + j];
				src->map_cu_mode[i * w_scu + j] = dst->map_cu_mode[i * w_scu + j];
				src->map_pb_tb_part[i * w_scu + j] = dst->map_pb_tb_part[i * w_scu + j];
				src->depth[i * w_scu + j] = dst->depth[i * w_scu + j];
				src->ipf_flag[i * w_scu + j] = dst->ipf_flag[i * w_scu + j];
				src->mvp_from_hmvp_flag[i * w_scu + j] = dst->mvp_from_hmvp_flag[i * w_scu + j];
			}
		}
		for (int k = 0; k < 2; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					src->mpm[k][i * w_scu + j] = dst->mpm[k][i * w_scu + j];
					src->ipm[k][i * w_scu + j] = dst->ipm[k][i * w_scu + j];
				}
			}
		}
		for (int k = 0; k < 8; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					src->mpm_ext[k][i * w_scu + j] = dst->mpm_ext[k][i * w_scu + j];
				}
			}
		}
		for (int k = 0; k < 1; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					src->num_nz_coef[k][i * w_scu + j] = dst->num_nz_coef[k][i * w_scu + j];
				}
			}
		}
		src->x_chroma = dst->x_chroma;
		src->y_chroma = dst->y_chroma;
		src->x_last = dst->x_last;
		src->y_last = dst->y_last;
		src->last_child = dst->last_child;
		for (int i = 0; i < cu_height; i++)
		{
			for (int j = 0; j < cu_width; j++)
			{
				src->coef_y[i * cu_width + j] = dst->coef_y[i][j];
				src->reco_y[i * cu_width + j] = dst->reco_y[i][j];
			}
		}
		//for (int i = 0; i < (cu_height >> 1); i++)
		//{
		//	for (int j = 0; j < (cu_width >> 1); j++)
		//	{
		//		src->coef_u[i * (cu_width >> 1) + j] = dst->coef_u[i][j];
		//		src->coef_v[i * (cu_width >> 1) + j] = dst->coef_v[i][j];
		//		src->reco_u[i * (cu_width >> 1) + j] = dst->reco_u[i][j];
		//		src->reco_v[i * (cu_width >> 1) + j] = dst->reco_v[i][j];
		//	}
		//}
	}
}
template<size_t SIZE>
void cu_data_cpy(ENC_CU_DATA_ARRAY* src, ENC_CU_DATA_ARRAY_SIZE<SIZE>* dst, int cu_width, int cu_height, int cdebug)
{
	int w_scu = cu_width >> 2;
	int h_scu = cu_height >> 2;
	if (cdebug)
	{
		for (int i = 0; i < cu_height; i++)
		{
			for (int j = 0; j < cu_width; j++)
			{
				assert(src->coef_y[i * cu_width + j] == dst->coef_y[i][j]);
				assert(src->reco_y[i * cu_width + j] == dst->reco_y[i][j]);
			}
		}
		for (int i = 0; i < h_scu; i++)
		{
			for (int j = 0; j < w_scu; j++)
			{
				for (int m = 0; m < 2; m++)
				{
					assert(src->refi[i * w_scu + j][m] == dst->refi[i * w_scu + j][m]);
					for (int n = 0; n < 2; n++)
					{
						assert(src->mv[i * w_scu + j][m][n] == dst->mv[i * w_scu + j][m][n]);
						assert(src->mvd[i * w_scu + j][m][n] == dst->mvd[i * w_scu + j][m][n]);
					}
				}
				assert(src->pb_part[i * w_scu + j] == dst->pb_part[i * w_scu + j]);
				assert(src->tb_part[i * w_scu + j] == dst->tb_part[i * w_scu + j]);
				assert(src->pred_mode[i * w_scu + j] == dst->pred_mode[i * w_scu + j]);
				assert(src->mvr_idx[i * w_scu + j] == dst->mvr_idx[i * w_scu + j]);
				assert(src->umve_flag[i * w_scu + j] == dst->umve_flag[i * w_scu + j]);
				assert(src->umve_idx[i * w_scu + j] == dst->umve_idx[i * w_scu + j]);
				assert(src->skip_idx[i * w_scu + j] == dst->skip_idx[i * w_scu + j]);
				assert(src->map_scu[i * w_scu + j] == dst->map_scu[i * w_scu + j]);
				assert(src->affine_flag[i * w_scu + j] == dst->affine_flag[i * w_scu + j]);
				assert(src->smvd_flag[i * w_scu + j] == dst->smvd_flag[i * w_scu + j]);
				assert(src->map_cu_mode[i * w_scu + j] == dst->map_cu_mode[i * w_scu + j]);
				assert(src->map_pb_tb_part[i * w_scu + j] == dst->map_pb_tb_part[i * w_scu + j]);
				assert(src->depth[i * w_scu + j] == dst->depth[i * w_scu + j]);
				assert(src->ipf_flag[i * w_scu + j] == dst->ipf_flag[i * w_scu + j]);
				assert(src->mvp_from_hmvp_flag[i * w_scu + j] == dst->mvp_from_hmvp_flag[i * w_scu + j]);
			}
		}
		for (int k = 0; k < 2; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					/*assert(src->mpm[k][i * w_scu + j] == dst->mpm[k][i * w_scu + j]);
					assert(src->ipm[k][i * w_scu + j] == dst->ipm[k][i * w_scu + j]);*/
				}
			}
		}
		for (int k = 0; k < 8; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					assert(src->mpm_ext[k][i * w_scu + j] == dst->mpm_ext[k][i * w_scu + j]);
				}
			}
		}
		for (int k = 0; k < 1; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					assert(src->num_nz_coef[k][i * w_scu + j] == dst->num_nz_coef[k][i * w_scu + j]);
				}
			}
		}
		assert(src->x_chroma == dst->x_chroma);
		assert(src->y_chroma == dst->y_chroma);
		assert(src->x_last == dst->x_last);
		assert(src->y_last == dst->y_last);
		assert(src->last_child == dst->last_child);
		for (int i = 0; i < cu_height; i++)
		{
			for (int j = 0; j < cu_width; j++)
			{
				assert(src->coef_y[i * cu_width + j] == dst->coef_y[i][j]);
				assert(src->reco_y[i * cu_width + j] == dst->reco_y[i][j]);
			}
		}
		//for (int i = 0; i < (cu_height >> 1); i++)
		//{
		//	for (int j = 0; j < (cu_width >> 1); j++)
		//	{
		//		assert(src->coef_u[i * (cu_width >> 1) + j] == dst->coef_u[i][j]);
		//		assert(src->coef_v[i * (cu_width >> 1) + j] == dst->coef_v[i][j]);
		//		assert(src->reco_u[i * (cu_width >> 1) + j] == dst->reco_u[i][j]);
		//		assert(src->reco_v[i * (cu_width >> 1) + j] == dst->reco_v[i][j]);
		//	}
		//}
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 7; j++)
			{
				for (int k = 0; k < 256; k++)
				{
					src->split_mode[i][j][k] = dst->split_mode[i][j][k];
				}
			}
		}

		for (int i = 0; i < h_scu; i++)
		{
			for (int j = 0; j < w_scu; j++)
			{
				for (int m = 0; m < 2; m++)
				{
					src->refi[i * w_scu + j][m] = dst->refi[i * w_scu + j][m];
					for (int n = 0; n < 2; n++)
					{
						src->mv[i * w_scu + j][m][n] = dst->mv[i * w_scu + j][m][n];
						src->mvd[i * w_scu + j][m][n] = dst->mvd[i * w_scu + j][m][n];
					}
				}
				src->pb_part[i * w_scu + j] = dst->pb_part[i * w_scu + j];
				src->tb_part[i * w_scu + j] = dst->tb_part[i * w_scu + j];
				src->pred_mode[i * w_scu + j] = dst->pred_mode[i * w_scu + j];
#if CUDQP_QP_MAP
				src->cu_luma_qp[i * w_scu + j] = dst->cu_luma_qp[i * w_scu + j];
#endif
				src->mvr_idx[i * w_scu + j] = dst->mvr_idx[i * w_scu + j];
				src->umve_flag[i * w_scu + j] = dst->umve_flag[i * w_scu + j];
				src->umve_idx[i * w_scu + j] = dst->umve_idx[i * w_scu + j];
				src->skip_idx[i * w_scu + j] = dst->skip_idx[i * w_scu + j];
				src->map_scu[i * w_scu + j] = dst->map_scu[i * w_scu + j];
				src->affine_flag[i * w_scu + j] = dst->affine_flag[i * w_scu + j];
				src->smvd_flag[i * w_scu + j] = dst->smvd_flag[i * w_scu + j];
				src->map_cu_mode[i * w_scu + j] = dst->map_cu_mode[i * w_scu + j];
				src->map_pb_tb_part[i * w_scu + j] = dst->map_pb_tb_part[i * w_scu + j];
				src->depth[i * w_scu + j] = dst->depth[i * w_scu + j];
				src->ipf_flag[i * w_scu + j] = dst->ipf_flag[i * w_scu + j];
				src->mvp_from_hmvp_flag[i * w_scu + j] = dst->mvp_from_hmvp_flag[i * w_scu + j];
			}
		}
		for (int k = 0; k < 2; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					src->mpm[k][i * w_scu + j] = dst->mpm[k][i * w_scu + j];
					src->ipm[k][i * w_scu + j] = dst->ipm[k][i * w_scu + j];
				}
			}
		}
		for (int k = 0; k < 8; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					src->mpm_ext[k][i * w_scu + j] = dst->mpm_ext[k][i * w_scu + j];
				}
			}
		}
		for (int k = 0; k < 1; k++)
		{
			for (int i = 0; i < h_scu; i++)
			{
				for (int j = 0; j < w_scu; j++)
				{
					src->num_nz_coef[k][i * w_scu + j] = dst->num_nz_coef[k][i * w_scu + j];
				}
			}
		}
		src->x_chroma = dst->x_chroma;
		src->y_chroma = dst->y_chroma;
		src->x_last = dst->x_last;
		src->y_last = dst->y_last;
		src->last_child = dst->last_child;
		for (int i = 0; i < cu_height; i++)
		{
			for (int j = 0; j < cu_width; j++)
			{
				src->coef_y[i * cu_width + j] = dst->coef_y[i][j];
				src->reco_y[i * cu_width + j] = dst->reco_y[i][j];
			}
		}
		//for (int i = 0; i < (cu_height >> 1); i++)
		//{
		//	for (int j = 0; j < (cu_width >> 1); j++)
		//	{
		//		src->coef_u[i * (cu_width >> 1) + j] = dst->coef_u[i][j];
		//		src->coef_v[i * (cu_width >> 1) + j] = dst->coef_v[i][j];
		//		src->reco_u[i * (cu_width >> 1) + j] = dst->reco_u[i][j];
		//		src->reco_v[i * (cu_width >> 1) + j] = dst->reco_v[i][j];
		//	}
		//}
	}
}
template<size_t SIZE>
void cu_data_cpy_c(ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY_CHROMA_SIZE<2 * SIZE>* src, int cu_width, int cu_height, int cdebug)
{
	int w_scu = cu_width >> 2;
	int h_scu = cu_height >> 2;
	if (cdebug)
	{
		for (int i = 0; i < 32; i++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=32
			for (int j = 0; j < 32; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=32
				if(i < cu_height >> 1 && j < cu_width >> 1)
				{
					assert(dst->coef_u[i * (cu_width >> 1) + j] == src->coef_u[i][j]);
					assert(dst->coef_v[i * (cu_width >> 1) + j] == src->coef_v[i][j]);
					assert(dst->reco_u[i * (cu_width >> 1) + j] == src->reco_u[i][j]);
					assert(dst->reco_v[i * (cu_width >> 1) + j] == src->reco_v[i][j]);
				}
			}
		}

		for (int i = 0; i < 16; i++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int j = 0; j < 16; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				if(i < h_scu && j < w_scu)
				{
					assert(dst->map_scu[i * w_scu + j] == src->map_scu[i * w_scu + j]);

					for (int m = 0; m < 2; m++)
						assert(dst->ipm[m][i * w_scu + j] == src->ipm[m][i * w_scu + j]);
					for (int m = 0; m < N_C; m++)
						assert(dst->num_nz_coef[m][i * w_scu + j] == src->num_nz_coef[m][i * w_scu + j]);
				}
			}
		}

		assert(dst->x_chroma == src->x_chroma);
		assert(dst->y_chroma == src->y_chroma);
		assert(dst->x_last == src->x_last);
		assert(dst->y_last == src->y_last);
		assert(dst->last_child == src->last_child);
	}
	else
	{
		for (int i = 0; i < 32; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=32
#pragma HLS PIPELINE
			for (int j = 0; j < 32; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=32
				if(i < cu_height >> 1 && j < cu_width >> 1)
				{
					dst->coef_u[i * (cu_width >> 1) + j] = src->coef_u[i][j];
					dst->coef_v[i * (cu_width >> 1) + j] = src->coef_v[i][j];

					dst->reco_u[i * (cu_width >> 1) + j] = src->reco_u[i][j];
					dst->reco_v[i * (cu_width >> 1) + j] = src->reco_v[i][j];
				}
			}
		}

		for (int i = 0; i < 16; i++)
		{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int j = 0; j < 16; j++)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				if(i < h_scu && j < w_scu)
				{
					dst->map_scu[i * w_scu + j] = src->map_scu[i * w_scu + j];

					for (int m = 0; m < 2; m++)
						dst->ipm[m][i * w_scu + j] = src->ipm[m][i * w_scu + j];
					for (int m = 0; m < N_C; m++)
						dst->num_nz_coef[m][i * w_scu + j] = src->num_nz_coef[m][i * w_scu + j];
				}
			}
		}

		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = src->last_child;
	}
}

void init_md_kernel_input(MD_KERNEL_INPUT* md_kernel_input, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U13 x0, U13 y0, U3 cu_width_log2, U3 cu_height_log2, U11 pic_width_in_scu, U2 tree_status);
void init_md_kernel_input_16(MD_KERNEL_INPUT* md_kernel_input, 
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_temp_ptr_16,
	U13 x0, U13 y0, U3 cu_width_log2, U3 cu_height_log2, U11 pic_width_in_scu, U2 tree_status);

void store_enc_info_best_16_top(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw);

void compare_store_top_16(MD_FW* md_fw_ptr, s64& cost_best, s64 cost, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_temp_array_ptr_8, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2 cons_pred_mode_child, U2 tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num);

s64 md_chroma_8_temp(U8 w, U8 h, RDOQ_MODEL* rdoq_model_c, RDOQ_MODEL* model_cnt_c, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][(((1 << (7)) >> 1) >> 2)][(((1 << (7)) >> 1) >> 2)],
	ENC_CU_DATA_ARRAY_INNER* curr_cu_data_temp_array_ptr, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U2 tree_status, U4 cud, MD_INPUT* md_input_ptr, pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	U1 need_dochroma, U12 cup, U2 ctx_cons_pred_mode,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr,  strFetch_ref_window* Fetch_Ref_window_ptr);

s64 prepare_cu_info_16(MD_FW* md_fw_ptr, s64 lambdaY, U13 x0, U13 y0,
	U12 cup, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct);

void store_enc_info_best_16_top2(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_INNER* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY_INNER* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw);

int copy_cu_data_array_dst_16_top(ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY_INNER* src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status, U3 best_split_mode,
	U3 shape, U9 pos, U8 split_mode[5][7][256]);

inline int mv_inbound_new(int mv_x, int mv_y, S14 ctrpos[2][2][2], int cu_width, int cu_height, int x_father, int y_father, int x_pos, int y_pos, BOOL chroma_motion, int bits = 2) {

	int x_offset_min, x_offset_max;
	int y_offset_min, y_offset_max;
	int pattern_x;
	int pattern_y;
	if (bits == 2) {
		pattern_x = mv_x & 0x3;
		pattern_y = mv_y & 0x3;
	}
	else if (bits == 4) {
		pattern_x = mv_x & 0xf;
		pattern_y = mv_y & 0xf;
	}
	else { assert(0); }
	int gmv_x = mv_x >> bits;
	int gmv_y = mv_y >> bits;

	if (pattern_x == 0) {
		x_offset_min = 0;
		x_offset_max = 0;
	}
	else {
		x_offset_min = -3;
		x_offset_max = 4;
	}
	if (pattern_y == 0) {
		y_offset_min = 0;
		y_offset_max = 0;
	}
	else {
		y_offset_min = -3;
		y_offset_max = 4;
	}

	if (chroma_motion &&
		((x_offset_min + gmv_x + x_father - x_pos < ctrpos[RANGE_MIN][MV_X][PIC_DIM]) ||
			(y_offset_min + gmv_y + y_father - y_pos < ctrpos[RANGE_MIN][MV_Y][PIC_DIM])))
	{
		return 0;
	}
	if (x_offset_min + gmv_x >= ctrpos[RANGE_MIN][MV_X][PIC_DIM] && x_offset_max + gmv_x + cu_width <= ctrpos[RANGE_MAX][MV_X][PIC_DIM] &&
		y_offset_min + gmv_y >= ctrpos[RANGE_MIN][MV_Y][PIC_DIM] && y_offset_max + gmv_y + cu_height <= ctrpos[RANGE_MAX][MV_Y][PIC_DIM]) {

		return 1;
	}
	else {
		return 0;
	}
}

S2 get_ref_idx_y(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx);
S2 get_ref_idx_y_16(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx);
S2 get_ref_idx_y_8_16(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx);
S2 get_ref_idx_y_16_8(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx);
S2 get_ref_idx_y_16_16(strFetch_ref_window_16* Fetch_Ref_window_ptr, s8 refi, int lidx);
inline int mv_inbound_new_skip(int mv_x, int mv_y, S14 ctrpos_skip[2][2][2]/*S14 ctrpos[2][2][2]*/, int cu_width, int cu_height, int x_father, int y_father, int x_pos, int y_pos, BOOL chroma_motion, int bits = 2)
{
	int x_offset_min, x_offset_max;
	int y_offset_min, y_offset_max;
	int pattern_x;
	int pattern_y;
	if (bits == 2)
	{
		pattern_x = mv_x & 0x3;
		pattern_y = mv_y & 0x3;
	}
	else if (bits == 4)
	{
		pattern_x = mv_x & 0xf;
		pattern_y = mv_y & 0xf;
	}
	else { assert(0); }
	int gmv_x = mv_x >> bits;
	int gmv_y = mv_y >> bits;
	//if (pattern_x == 0)
	//{
	//	x_offset_min = 0;
	//	x_offset_max = 0;
	//}
	//else
	//{
	//	x_offset_min = -3;
	//	x_offset_max = 4;
	//}
	//if (pattern_y == 0)
	//{
	//	y_offset_min = 0;
	//	y_offset_max = 0;
	//}
	//else
	//{
	//	y_offset_min = -3;
	//	y_offset_max = 4;
	//}

	if (pattern_x == 0 && pattern_y == 0) {
		x_offset_min = 0;
		y_offset_min = 0;
	}
	else {
		x_offset_min = -3;
		y_offset_min = -3;
	}
	x_offset_max = 0;
	y_offset_max = 0;

	if (chroma_motion &&
		((x_offset_min + gmv_x + x_father - x_pos < ctrpos_skip[RANGE_MIN][MV_X][PIC_DIM]) ||
			(y_offset_min + gmv_y + y_father - y_pos < ctrpos_skip[RANGE_MIN][MV_Y][PIC_DIM])))
	{
		return 0;
	}
	if (x_offset_min + gmv_x >= ctrpos_skip[RANGE_MIN][MV_X][PIC_DIM] && x_offset_max + gmv_x + cu_width <= (ctrpos_skip[RANGE_MAX][MV_X][PIC_DIM] - 4) &&
		y_offset_min + gmv_y >= ctrpos_skip[RANGE_MIN][MV_Y][PIC_DIM] && y_offset_max + gmv_y + cu_height <= (ctrpos_skip[RANGE_MAX][MV_Y][PIC_DIM] - 4))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
inline void check_skip_mv_inbound(SKIP_ELEMENT* mode, int x_pos, int y_pos, S14 CtrPos_final[RANGE_NUM][DIR_DIM][MAP_DIMS], int cu_width, int cu_height, MD_COM_MODE* mod_info_curr
#if ENABLE_BFRAME
	, S14 CtrPos_final_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], strFetch_ref_window* Fetch_Ref_window_ptr
#endif // ENABLE_BFRAME
)
{ 
	int i_tmp = 0;
	for (int i = 0; i < SKIP_MODE_NUM; ++i)
	{
#pragma HLS	UNROLL
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
				S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, mode[i].refi[REFP_0], REFP_0);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
				if (cur_refw_idx == 0){
				if (!mv_inbound_new_skip(gmv_x, gmv_y,  CtrPos_final , cu_width, cu_height, mod_info_curr->x_chroma, mod_info_curr->y_chroma, x_pos, y_pos, mod_info_curr->chroma_motion))
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
				S2 cur_refw_idx = get_ref_idx_y(Fetch_Ref_window_ptr, mode[i].refi[REFP_1], REFP_1);
				assert(cur_refw_idx == 0 || cur_refw_idx == 1);
				if (cur_refw_idx == 0){
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
		i_tmp++;
	}
}


template<size_t w_chroma, size_t h_chroma>
u16 com_get_avail_intra_ip_chroma(U11 x_scu, U11 y_scu, IP_MAP_SCU_SIZE<w_chroma << 1, h_chroma << 1>* IP_map_scu_ptr)
{

#pragma HLS INLINE off
	u16 avail = (u16)0;
	U11 pic_x_scu = x_scu & 15;
	U11 pic_y_scu = y_scu & 15;
	if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr->map_scu_left_line_new[0] >> 31 & (1)))) {
			avail |= 1 << 1;
		}
	}
	if (y_scu > 0) {
		if (pic_y_scu == 0) {
			if ((IP_map_scu_ptr->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
		else {
			if ((IP_map_scu_ptr->map_scu_up_line[0] >> 31 & (1))) {
				avail |= 1 << 0;
			}
		}
	}
	if (pic_y_scu == 0) {
		if (x_scu > 0)
			if (x_scu > 0
				&& ((IP_map_scu_ptr->map_scu_top_left_new >> 31 & (1)))) {
				avail |= 1 << 2;
			}
	}
	else if (pic_x_scu == 0) {
		if (x_scu > 0
			&& ((IP_map_scu_ptr->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	else {
		if (x_scu > 0
			&& ((IP_map_scu_ptr->map_scu_top_left_new >> 31 & (1)))) {
			avail |= 1 << 2;
		}
	}
	return avail;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/* nebs buf */
inline void get_up_nebs_line(ENC_ME_NEB_INFO* up_nebs,
	ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	int cu_width, int cu_height, int x_pos_pic, int y_pos_pic, int debug = 0)
{
	int x_scu_pos_lcu = (x_pos_pic >> 2) & 15;
	int y_scu_pos_lcu = (y_pos_pic >> 2) & 15;

	if (debug)
	{
		if (x_pos_pic > 0)
		{
			if (y_scu_pos_lcu == 0)
			{
				for (int i = 0; i < REFP_NUM; i++)
				{
					assert(up_nebs[0].refi[i] == above_nebs_line[16 + x_scu_pos_lcu - 1].refi[i]);
					for (int j = 0; j < MV_D; j++)
						assert(up_nebs[0].mv[i][j] == above_nebs_line[16 + x_scu_pos_lcu - 1].mv[i][j]);
				}
			}
			else
			{
				if (x_scu_pos_lcu == 0)
				{
					for (int i = 0; i < REFP_NUM; i++)
					{
						assert(up_nebs[0].refi[i] == inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].refi[i]);
						for (int j = 0; j < MV_D; j++)
							assert(up_nebs[0].mv[i][j] == inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].mv[i][j]);
					}
				}
				else
				{
					for (int i = 0; i < REFP_NUM; i++)
					{
						assert(up_nebs[0].refi[i] == inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1].refi[i]);
						for (int j = 0; j < MV_D; j++)
							assert(up_nebs[0].mv[i][j] == inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1].mv[i][j]);
					}
				}
			}
			//up_nebs[0] = (y_scu_pos_lcu == 0 ?
			//	above_nebs_line[16 + x_scu_pos_lcu - 1] :
			//	(x_scu_pos_lcu == 0 ?
			//		inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1] :
			//		inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1]
			//		));
		}
		else
		{
			if (y_scu_pos_lcu > 0)
			{
				for (int i = 0; i < REFP_NUM; i++)
				{
					assert(up_nebs[0].refi[i] == inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].refi[i]);
					for (int j = 0; j < MV_D; j++)
						assert(up_nebs[0].mv[i][j] == inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].mv[i][j]);
				}

				//up_nebs[0] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1];
			}
			else
			{
				for (int i = 0; i < REFP_NUM; i++)
				{
					assert(up_nebs[0].refi[i] == above_nebs_line[16 + x_scu_pos_lcu - 1].refi[i]);
					for (int j = 0; j < MV_D; j++)
						assert(up_nebs[0].mv[i][j] == above_nebs_line[16 + x_scu_pos_lcu - 1].mv[i][j]);
				}
			}
		}

		for (int i = 0; i < ((cu_width >> 2) + 1); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=17
			if (y_scu_pos_lcu == 0)
			{
				for (int m = 0; m < REFP_NUM; m++)
				{
					assert(up_nebs[i + 1].refi[m] == above_nebs_line[16 + x_scu_pos_lcu + i].refi[m]);
					for (int n = 0; n < MV_D; n++)
						assert(up_nebs[i + 1].mv[m][n] == above_nebs_line[16 + x_scu_pos_lcu + i].mv[m][n]);
				}

				//up_nebs[i + 1] = above_nebs_line[16 + x_scu_pos_lcu + i];
			}
			else
			{
				if (x_scu_pos_lcu + i < 16)
				{
					for (int m = 0; m < REFP_NUM; m++)
					{
						assert(up_nebs[i + 1].refi[m] == inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1].refi[m]);
						for (int n = 0; n < MV_D; n++)
							assert(up_nebs[i + 1].mv[m][n] == inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1].mv[m][n]);
					}

					//up_nebs[i + 1] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1];
				}
			}
		}
	}
	else
	{
		if (x_pos_pic > 0)
		{
			if (y_scu_pos_lcu == 0)
			{
				for (int i = 0; i < REFP_NUM; i++)
				{
					up_nebs[0].refi[i] = above_nebs_line[16 + x_scu_pos_lcu - 1].refi[i];
					for (int j = 0; j < MV_D; j++)
						up_nebs[0].mv[i][j] = above_nebs_line[16 + x_scu_pos_lcu - 1].mv[i][j];
				}
			}
			else
			{
				if (x_scu_pos_lcu == 0)
				{
					for (int i = 0; i < REFP_NUM; i++)
					{
						up_nebs[0].refi[i] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].refi[i];
						for (int j = 0; j < MV_D; j++)
							up_nebs[0].mv[i][j] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].mv[i][j];
					}
				}
				else
				{
					for (int i = 0; i < REFP_NUM; i++)
					{
						up_nebs[0].refi[i] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1].refi[i];
						for (int j = 0; j < MV_D; j++)
							up_nebs[0].mv[i][j] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1].mv[i][j];
					}
				}
			}
			//up_nebs[0] = (y_scu_pos_lcu == 0 ?
			//	above_nebs_line[16 + x_scu_pos_lcu - 1] :
			//	(x_scu_pos_lcu == 0 ?
			//		inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1] :
			//		inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1]
			//		));
		}
		else
		{
			if (y_scu_pos_lcu > 0)
			{
				for (int i = 0; i < REFP_NUM; i++)
				{
					up_nebs[0].refi[i] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].refi[i];
					for (int j = 0; j < MV_D; j++)
						up_nebs[0].mv[i][j] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1].mv[i][j];
				}

				//up_nebs[0] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1];
			}
			else
			{
				for (int i = 0; i < REFP_NUM; i++)
				{
					up_nebs[0].refi[i] = above_nebs_line[16 + x_scu_pos_lcu - 1].refi[i];
					for (int j = 0; j < MV_D; j++)
						up_nebs[0].mv[i][j] = above_nebs_line[16 + x_scu_pos_lcu - 1].mv[i][j];
				}
			}
		}

		for (int i = 0; i < ((cu_width >> 2) + 1); i++)
		{
			if (y_scu_pos_lcu == 0)
			{
				for (int m = 0; m < REFP_NUM; m++)
				{
					up_nebs[i + 1].refi[m] = above_nebs_line[16 + x_scu_pos_lcu + i].refi[m];
					for (int n = 0; n < MV_D; n++)
						up_nebs[i + 1].mv[m][n] = above_nebs_line[16 + x_scu_pos_lcu + i].mv[m][n];
				}

				//up_nebs[i + 1] = above_nebs_line[16 + x_scu_pos_lcu + i];
			}
			else
			{
				if (x_scu_pos_lcu + i < 16)
				{
					for (int m = 0; m < REFP_NUM; m++)
					{
						up_nebs[i + 1].refi[m] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1].refi[m];
						for (int n = 0; n < MV_D; n++)
							up_nebs[i + 1].mv[m][n] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1].mv[m][n];
					}

					//up_nebs[i + 1] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1];
				}
			}
		}
	}
}
inline void get_left_nebs_line(ENC_ME_NEB_INFO* left_nebs,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	int cu_width, int cu_height, int x_pos_pic, int y_pos_pic, int debug = 0)
{
	int x_scu_pos_lcu = (x_pos_pic >> 2) & 15;
	int y_scu_pos_lcu = (y_pos_pic >> 2) & 15;

	if (debug)
	{
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			if (y_scu_pos_lcu + i < 16)
			{
				if (x_scu_pos_lcu == 0)
				{
					for (int m = 0; m < REFP_NUM; m++)
					{
						assert(left_nebs[i].refi[m] == inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i].refi[m]);
						for (int n = 0; n < MV_D; n++)
							assert(left_nebs[i].mv[m][n] == inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i].mv[m][n]);
					}
				}
				else
				{
					for (int m = 0; m < REFP_NUM; m++)
					{
						assert(left_nebs[i].refi[m] == inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i].refi[m]);
						for (int n = 0; n < MV_D; n++)
							assert(left_nebs[i].mv[m][n] == inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i].mv[m][n]);
					}
				}
				//left_nebs[i] = x_scu_pos_lcu == 0 ?
				//	inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i] :
				//	inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i];
			}
		}
	}
	else
	{
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			if (y_scu_pos_lcu + i < 16)
			{
				if (x_scu_pos_lcu == 0)
				{
					for (int m = 0; m < REFP_NUM; m++)
					{
						left_nebs[i].refi[m] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i].refi[m];
						for (int n = 0; n < MV_D; n++)
							left_nebs[i].mv[m][n] = inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i].mv[m][n];
					}
				}
				else
				{
					for (int m = 0; m < REFP_NUM; m++)
					{
						left_nebs[i].refi[m] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i].refi[m];
						for (int n = 0; n < MV_D; n++)
							left_nebs[i].mv[m][n] = inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i].mv[m][n];
					}
				}
				//left_nebs[i] = x_scu_pos_lcu == 0 ?
				//	inner_nebs[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i] :
				//	inner_nebs[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i];
			}
		}
	}
}
inline void fetch_nebs_bi_ver(
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO* nebs_line_ver,
	int cu_width, int cu_height, int part_num)
{
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < (cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[i].mv[m][n];
				}
			}
		}
		break;
	case 1:
		for (int i = 0; i < (cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[i + (cu_width >> 2)].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[i + (cu_width >> 2)].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = nebs_line_ver[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = nebs_line_ver[i].mv[m][n];
				}
			}
		}
		break;
	default:
		assert(0);
	}
}
inline void fetch_nebs_bi_hor(
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO* nebs_line_hor,
	int cu_width, int cu_height, int part_num)
{
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < (cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[i].mv[m][n];
				}
			}
		}
		break;
	case 1:
		for (int m = 0; m < REFP_NUM; m++)
		{
#pragma HLS pipeline
			up_nebs[0].refi[m] = left_nebs_father[(cu_height >> 2) - 1].refi[m];
			for (int n = 0; n < MV_D; n++)
			{
				up_nebs[0].mv[m][n] = left_nebs_father[(cu_height >> 2) - 1].mv[m][n];
			}
		}
		for (int i = 0; i < (cu_width >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i + 1].refi[m] = nebs_line_hor[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i + 1].mv[m][n] = nebs_line_hor[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
#pragma HLS pipeline
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[(cu_height >> 2) + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[(cu_height >> 2) + i].mv[m][n];
				}
			}
		}
		break;
	default:
		assert(0);
	}
}
inline void fetch_nebs_qt(
	ENC_ME_NEB_INFO* up_nebs, ENC_ME_NEB_INFO* left_nebs,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO* nebs_line_hor, ENC_ME_NEB_INFO* nebs_line_ver,
	int cu_width, int cu_height, int part_num)
{
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < (cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[i].mv[m][n];
				}
			}
		}
		break;
	case 1:
		for (int i = 0; i < (cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = up_nebs_father[(cu_width >> 2) + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = up_nebs_father[(cu_width >> 2) + i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = nebs_line_ver[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = nebs_line_ver[i].mv[m][n];
				}
			}
		}
		break;
	case 2:
		for (int m = 0; m < REFP_NUM; m++)
		{
			up_nebs[0].refi[m] = left_nebs_father[(cu_width >> 2) - 1].refi[m];
			for (int n = 0; n < MV_D; n++)
			{
				up_nebs[0].mv[m][n] = left_nebs_father[(cu_width >> 2) - 1].mv[m][n];
			}
		}
		for (int i = 0; i < (cu_width >> 2) + 1; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=17
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i + 1].refi[m] = nebs_line_hor[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i + 1].mv[m][n] = nebs_line_hor[i].mv[m][n];
				}
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = left_nebs_father[(cu_height >> 2) + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = left_nebs_father[(cu_height >> 2) + i].mv[m][n];
				}
			}
		}
		break;
	case 3:
		for (int i = 0; i < (cu_width >> 2) + 1; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=17
			for (int m = 0; m < REFP_NUM; m++)
			{
				up_nebs[i].refi[m] = nebs_line_hor[(cu_width >> 2) - 1 + i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					up_nebs[i].mv[m][n] = nebs_line_hor[(cu_width >> 2) - 1 + i].mv[m][n];
				}
			}
		}
		for (int m = 0; m < REFP_NUM; m++)
		{
			up_nebs[(cu_width >> 2) + 1].refi[m] = -1;
			for (int n = 0; n < MV_D; n++)
			{
				up_nebs[(cu_width >> 2) + 1].mv[m][n] = 0;
			}
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int m = 0; m < REFP_NUM; m++)
			{
				left_nebs[i].refi[m] = nebs_line_ver[i].refi[m];
				for (int n = 0; n < MV_D; n++)
				{
					left_nebs[i].mv[m][n] = nebs_line_ver[i].mv[m][n];
				}
			}
		}
		break;
	default:
		assert(0);
	}
}
inline void fetch_line_buffer_qt(U3 part_num, U8 cu_width, U8 cu_height, \
	IP_ARRAY_SIZE<16, 16>* IP_buffer_dst_16x16, IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_dst_16x16, MPM_ARRAY_SIZE<16, 16>* MPM_buffer_dst_16x16, \
	IP_ARRAY_SIZE<16, 16>* IP_buffer_pre_16x16, MPM_ARRAY_SIZE<16, 16>* MPM_buffer_pre_16x16, \
	IP_ARRAY_SIZE<32, 32>* IP_buffer_src_32x32, IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_src_32x32, MPM_ARRAY_SIZE<32, 32>* MPM_buffer_src_32x32, \
	IP_ARRAY_SIZE<32, 32>* IP_buffer_dst_32x32, IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_dst_32x32, MPM_ARRAY_SIZE<32, 32>* MPM_buffer_dst_32x32, \
	IP_ARRAY_SIZE<32, 32>* IP_buffer_pre_32x32, MPM_ARRAY_SIZE<32, 32>* MPM_buffer_pre_32x32, \
	IP_ARRAY_SIZE<64, 64>* IP_buffer_src_64x64, IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_src_64x64, MPM_ARRAY_SIZE<64, 64>* MPM_buffer_src_64x64)
{
	if ((cu_width == 16) && (cu_height == 16))
	{
		switch (part_num)
		{
		case 0:
			// IP buffer
			for (int i = 0; i < (16 << 1); ++i)
			{
				IP_buffer_dst_16x16->up_line[i] = IP_buffer_src_32x32->up_line[i];
				IP_buffer_dst_16x16->left_line[i] = IP_buffer_src_32x32->left_line[i];
			}
			IP_buffer_dst_16x16->top_left = IP_buffer_src_32x32->top_left;
			// IP_MAP_SCU buffer
			for (int i = 0; i < ((16 >> 2) << 1); i++)
			{
				IP_map_scu_dst_16x16->map_scu_up_line[i] = IP_map_scu_src_32x32->map_scu_up_line[i];
				IP_map_scu_dst_16x16->map_scu_left_line_new[i] = IP_map_scu_src_32x32->map_scu_left_line_new[i];
			}
			IP_map_scu_dst_16x16->map_scu_top_left_new = IP_map_scu_src_32x32->map_scu_top_left_new;
			// MPM buffer
			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_dst_16x16->up_line[i] = MPM_buffer_src_32x32->up_line[i];
				MPM_buffer_dst_16x16->left_line[i] = MPM_buffer_src_32x32->left_line[i];
			}
			MPM_buffer_dst_16x16->up = MPM_buffer_dst_16x16->up_line[0];
			MPM_buffer_dst_16x16->left = MPM_buffer_dst_16x16->left_line[0];
			break;
		case 1:
			for (int i = 0; i < (16 << 1); ++i)
			{
				IP_buffer_dst_16x16->up_line[i] = IP_buffer_src_32x32->up_line[i + 16];
			}
			for (int i = 0; i < 16; ++i)
			{
				IP_buffer_dst_16x16->left_line[i] = IP_buffer_pre_16x16->rec_line_ver[i];
			}
			for (int i = 16; i < 32; ++i)
			{
				IP_buffer_dst_16x16->left_line[i] = 0;
			}
			IP_buffer_dst_16x16->top_left = IP_buffer_src_32x32->up_line[16 - 1];

			for (int i = 0; i < ((16 >> 2) << 1); i++)
			{
				IP_map_scu_dst_16x16->map_scu_up_line[i] = IP_map_scu_src_32x32->map_scu_up_line[i + (16 >> 2)];
				IP_map_scu_dst_16x16->map_scu_left_line_new[i] = IP_map_scu_src_32x32->cur_map_scu[(16 >> 2) - 1][i];
			}
			IP_map_scu_dst_16x16->map_scu_top_left_new = IP_map_scu_src_32x32->map_scu_up_line[(16 >> 2) - 1];

			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_dst_16x16->up_line[i] = MPM_buffer_src_32x32->up_line[i + (16 >> 2)];
				MPM_buffer_dst_16x16->left_line[i] = MPM_buffer_pre_16x16->ipm_line_ver[i];
			}
			MPM_buffer_dst_16x16->up = MPM_buffer_dst_16x16->up_line[0];
			MPM_buffer_dst_16x16->left = MPM_buffer_dst_16x16->left_line[0];
			break;
		case 2:
			for (int i = 0; i < (16 << 1); ++i)
			{
				IP_buffer_dst_16x16->up_line[i] = IP_buffer_pre_16x16->rec_line_hor[i];
			}
			for (int i = 0; i < (16 << 1); ++i)
			{
				IP_buffer_dst_16x16->left_line[i] = IP_buffer_src_32x32->left_line[i + 16];
			}
			IP_buffer_dst_16x16->top_left = IP_buffer_src_32x32->left_line[16 - 1];

			for (int i = 0; i < ((16 >> 2) << 1); i++)
			{
				IP_map_scu_dst_16x16->map_scu_up_line[i] = IP_map_scu_src_32x32->cur_map_scu[i][(16 >> 2) - 1];
				IP_map_scu_dst_16x16->map_scu_left_line_new[i] = IP_map_scu_src_32x32->map_scu_left_line_new[i + (16 >> 2)];
			}
			IP_map_scu_dst_16x16->map_scu_top_left_new = IP_map_scu_src_32x32->map_scu_left_line_new[(16 >> 2) - 1];

			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_dst_16x16->up_line[i] = MPM_buffer_pre_16x16->ipm_line_hor[i];
				MPM_buffer_dst_16x16->left_line[i] = MPM_buffer_src_32x32->left_line[i + (16 >> 2)];
			}
			MPM_buffer_dst_16x16->up = MPM_buffer_dst_16x16->up_line[0];
			MPM_buffer_dst_16x16->left = MPM_buffer_dst_16x16->left_line[0];
			break;
		case 3:
			for (int i = 0; i < 16; ++i)
			{
				IP_buffer_dst_16x16->up_line[i] = IP_buffer_pre_16x16->rec_line_hor[i + 16];
				IP_buffer_dst_16x16->left_line[i] = IP_buffer_pre_16x16->rec_line_ver[i + 16];
			}
			for (int i = 16; i < 32; ++i)
			{
				IP_buffer_dst_16x16->up_line[i] = 0;
				IP_buffer_dst_16x16->left_line[i] = 0;
			}
			IP_buffer_dst_16x16->top_left = IP_buffer_pre_16x16->rec_line_hor[16 - 1];

			clear_map_scu_line(IP_map_scu_dst_16x16->map_scu_up_line, (16 >> 2) << 1);
			clear_map_scu_line(IP_map_scu_dst_16x16->map_scu_left_line_new, (16 >> 2) << 1);
			for (int i = 0; i < (16 >> 2); i++)
			{
				IP_map_scu_dst_16x16->map_scu_up_line[i] = IP_map_scu_src_32x32->cur_map_scu[(16 >> 2) + i][(16 >> 2) - 1];
				IP_map_scu_dst_16x16->map_scu_left_line_new[i] = IP_map_scu_src_32x32->cur_map_scu[(16 >> 2) - 1][(16 >> 2) + i];
			}
			IP_map_scu_dst_16x16->map_scu_top_left_new = IP_map_scu_src_32x32->cur_map_scu[(16 >> 2) - 1][(16 >> 2) - 1];

			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_dst_16x16->up_line[i] = MPM_buffer_pre_16x16->ipm_line_hor[i + (16 >> 2)];
				MPM_buffer_dst_16x16->left_line[i] = MPM_buffer_pre_16x16->ipm_line_ver[i];
			}
			MPM_buffer_dst_16x16->up = MPM_buffer_dst_16x16->up_line[0];
			MPM_buffer_dst_16x16->left = MPM_buffer_dst_16x16->left_line[0];
			break;
		default:
			assert(0);
		}
	}
	else if ((cu_width == 32) && (cu_height == 32))
	{
		switch (part_num)
		{
		case 0:
			// IP buffer
			for (int i = 0; i < (32 << 1); ++i)
			{
				IP_buffer_dst_32x32->up_line[i] = IP_buffer_src_64x64->up_line[i];
				IP_buffer_dst_32x32->left_line[i] = IP_buffer_src_64x64->left_line[i];
			}
			IP_buffer_dst_32x32->top_left = IP_buffer_src_64x64->top_left;
			// IP_MAP_SCU buffer
			for (int i = 0; i < ((32 >> 2) << 1); i++)
			{
				IP_map_scu_dst_32x32->map_scu_up_line[i] = IP_map_scu_src_64x64->map_scu_up_line[i];
				IP_map_scu_dst_32x32->map_scu_left_line_new[i] = IP_map_scu_src_64x64->map_scu_left_line_new[i];
			}
			IP_map_scu_dst_32x32->map_scu_top_left_new = IP_map_scu_src_64x64->map_scu_top_left_new;
			// MPM buffer
			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_dst_32x32->up_line[i] = MPM_buffer_src_64x64->up_line[i];
				MPM_buffer_dst_32x32->left_line[i] = MPM_buffer_src_64x64->left_line[i];
			}
			MPM_buffer_dst_32x32->up = MPM_buffer_dst_32x32->up_line[0];
			MPM_buffer_dst_32x32->left = MPM_buffer_dst_32x32->left_line[0];
			break;
		case 1:
			for (int i = 0; i < (32 << 1); ++i)
			{
				IP_buffer_dst_32x32->up_line[i] = IP_buffer_src_64x64->up_line[i + 32];
			}
			for (int i = 0; i < 32; ++i)
			{
				IP_buffer_dst_32x32->left_line[i] = IP_buffer_pre_32x32->rec_line_ver[i];
			}
			for (int i = 32; i < 64; ++i)
			{
				IP_buffer_dst_32x32->left_line[i] = 0;
			}
			IP_buffer_dst_32x32->top_left = IP_buffer_src_64x64->up_line[32 - 1];

			for (int i = 0; i < ((32 >> 2) << 1); i++)
			{
				IP_map_scu_dst_32x32->map_scu_up_line[i] = IP_map_scu_src_64x64->map_scu_up_line[i + (32 >> 2)];
				IP_map_scu_dst_32x32->map_scu_left_line_new[i] = IP_map_scu_src_64x64->cur_map_scu[(32 >> 2) - 1][i];
			}
			IP_map_scu_dst_32x32->map_scu_top_left_new = IP_map_scu_src_64x64->map_scu_up_line[(32 >> 2) - 1];

			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_dst_32x32->up_line[i] = MPM_buffer_src_64x64->up_line[i + (32 >> 2)];
				MPM_buffer_dst_32x32->left_line[i] = MPM_buffer_pre_32x32->ipm_line_ver[i];
			}
			MPM_buffer_dst_32x32->up = MPM_buffer_dst_32x32->up_line[0];
			MPM_buffer_dst_32x32->left = MPM_buffer_dst_32x32->left_line[0];
			break;
		case 2:
			for (int i = 0; i < (32 << 1); ++i)
			{
				IP_buffer_dst_32x32->up_line[i] = IP_buffer_pre_32x32->rec_line_hor[i];
			}
			for (int i = 0; i < (32 << 1); ++i)
			{
				if (i + 32 < 64)
				{
					IP_buffer_dst_32x32->left_line[i] = IP_buffer_src_64x64->left_line[i + 32];
				}
				else
				{
					IP_buffer_dst_32x32->left_line[i] = 0;
				}
			}
			IP_buffer_dst_32x32->top_left = IP_buffer_src_64x64->left_line[32 - 1];

			for (int i = 0; i < ((32 >> 2) << 1); i++)
			{
				IP_map_scu_dst_32x32->map_scu_up_line[i] = IP_map_scu_src_64x64->cur_map_scu[i][(32 >> 2) - 1];
				IP_map_scu_dst_32x32->map_scu_left_line_new[i] = IP_map_scu_src_64x64->map_scu_left_line_new[i + (32 >> 2)];
			}
			IP_map_scu_dst_32x32->map_scu_top_left_new = IP_map_scu_src_64x64->map_scu_left_line_new[(32 >> 2) - 1];

			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_dst_32x32->up_line[i] = MPM_buffer_pre_32x32->ipm_line_hor[i];
				MPM_buffer_dst_32x32->left_line[i] = MPM_buffer_src_64x64->left_line[i + (32 >> 2)];
			}
			MPM_buffer_dst_32x32->up = MPM_buffer_dst_32x32->up_line[0];
			MPM_buffer_dst_32x32->left = MPM_buffer_dst_32x32->left_line[0];
			break;
		case 3:
			for (int i = 0; i < 32; ++i)
			{
				IP_buffer_dst_32x32->up_line[i] = IP_buffer_pre_32x32->rec_line_hor[i + 32];
				IP_buffer_dst_32x32->left_line[i] = IP_buffer_pre_32x32->rec_line_ver[i + 32];
			}
			for (int i = 32; i < 64; ++i)
			{
				IP_buffer_dst_32x32->up_line[i] = 0;
				IP_buffer_dst_32x32->left_line[i] = 0;
			}
			IP_buffer_dst_32x32->top_left = IP_buffer_pre_32x32->rec_line_hor[32 - 1];

			clear_map_scu_line(IP_map_scu_dst_32x32->map_scu_up_line, (32 >> 2) << 1);
			clear_map_scu_line(IP_map_scu_dst_32x32->map_scu_left_line_new, (32 >> 2) << 1);
			for (int i = 0; i < (32 >> 2); i++)
			{
				IP_map_scu_dst_32x32->map_scu_up_line[i] = IP_map_scu_src_64x64->cur_map_scu[(32 >> 2) + i][(32 >> 2) - 1];
				IP_map_scu_dst_32x32->map_scu_left_line_new[i] = IP_map_scu_src_64x64->cur_map_scu[(32 >> 2) - 1][(32 >> 2) + i];
			}
			IP_map_scu_dst_32x32->map_scu_top_left_new = IP_map_scu_src_64x64->cur_map_scu[(32 >> 2) - 1][(32 >> 2) - 1];

			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_dst_32x32->up_line[i] = MPM_buffer_pre_32x32->ipm_line_hor[i + (32 >> 2)];
				MPM_buffer_dst_32x32->left_line[i] = MPM_buffer_pre_32x32->ipm_line_ver[i];
			}
			MPM_buffer_dst_32x32->up = MPM_buffer_dst_32x32->up_line[0];
			MPM_buffer_dst_32x32->left = MPM_buffer_dst_32x32->left_line[0];
			break;
		default:
			assert(0);
		}
	}
}

inline void update_line_buffer_qt(U3 part_num, U8 cu_width, U8 cu_height, int scu_h, int scu_w, U3 cu_width_log2, \
	ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best_ptr_16, IP_ARRAY_SIZE<16, 16>* IP_buffer_16x16, MPM_ARRAY_SIZE<16, 16>* MPM_buffer_16x16, IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_32x32, \
	ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr_32, IP_ARRAY_SIZE<32, 32>* IP_buffer_32x32, MPM_ARRAY_SIZE<32, 32>* MPM_buffer_32x32, IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_64x64)
{
	if ((cu_width == 16) && (cu_height == 16))
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 16; ++i)
			{
				IP_buffer_16x16->rec_line_hor[i] = curr_cu_data_best_ptr_16->reco_y[16 - 1][i];
				IP_buffer_16x16->rec_line_ver[i] = curr_cu_data_best_ptr_16->reco_y[i][16 - 1];
			}

			for (int i = 0; i < scu_h; i++)
			{
#pragma HLS loop_tripcount max=16
				for (int j = 0; j < scu_w; j++)
				{
#pragma HLS loop_tripcount max=16
					IP_map_scu_32x32->cur_map_scu[j][i] = curr_cu_data_best_ptr_16->map_scu[j + (i << (cu_width_log2 - 2))];
				}
			}

			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_16x16->ipm_line_ver[i] = curr_cu_data_best_ptr_16->ipm[0][((16 >> 2) - 1) + (i << (cu_width_log2 - 2))];
				MPM_buffer_16x16->ipm_line_hor[i] = curr_cu_data_best_ptr_16->ipm[0][i + (((16 >> 2) - 1) << (cu_width_log2 - 2))];
			}
			break;
		case 1:
			for (int i = 0; i < 16; ++i)
			{
				IP_buffer_16x16->rec_line_hor[i + 16] = curr_cu_data_best_ptr_16->reco_y[16 - 1][i];
			}

			for (int i = 0; i < scu_h; i++)
			{
#pragma HLS loop_tripcount max=16
				for (int j = 0; j < scu_w; j++)
				{
#pragma HLS loop_tripcount max=16
					IP_map_scu_32x32->cur_map_scu[(cu_width >> 2) + j][i] = curr_cu_data_best_ptr_16->map_scu[j + (i << (cu_width_log2 - 2))];
				}
			}

			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_16x16->ipm_line_hor[i + (16 >> 2)] = curr_cu_data_best_ptr_16->ipm[0][i + (((16 >> 2) - 1) << (cu_width_log2 - 2))];
			}
			break;
		case 2:
			for (int i = 0; i < 16; ++i)
			{
				IP_buffer_16x16->rec_line_ver[i + 16] = curr_cu_data_best_ptr_16->reco_y[i][16 - 1];
			}

			for (int i = 0; i < scu_h; i++)
			{
#pragma HLS loop_tripcount max=16
				for (int j = 0; j < scu_w; j++)
				{
#pragma HLS loop_tripcount max=16
					IP_map_scu_32x32->cur_map_scu[j][(cu_height >> 2) + i] = curr_cu_data_best_ptr_16->map_scu[j + (i << (cu_width_log2 - 2))];
				}
			}

			for (int i = 0; i < (16 >> 2); i++)
			{
				MPM_buffer_16x16->ipm_line_ver[i] = curr_cu_data_best_ptr_16->ipm[0][((16 >> 2) - 1) + (i << (cu_width_log2 - 2))];
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
	}
	else if ((cu_width == 32) && (cu_height == 32))
	{
		switch (part_num)
		{
		case 0:
			for (int i = 0; i < 32; ++i)
			{
				IP_buffer_32x32->rec_line_hor[i] = curr_cu_data_best_ptr_32->reco_y[32 - 1][i];
				IP_buffer_32x32->rec_line_ver[i] = curr_cu_data_best_ptr_32->reco_y[i][32 - 1];
			}

			for (int i = 0; i < scu_h; i++)
			{
#pragma HLS loop_tripcount max=16
				for (int j = 0; j < scu_w; j++)
				{
#pragma HLS loop_tripcount max=16
					IP_map_scu_64x64->cur_map_scu[j][i] = curr_cu_data_best_ptr_32->map_scu[j + (i << (cu_width_log2 - 2))];
				}
			}

			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_32x32->ipm_line_ver[i] = curr_cu_data_best_ptr_32->ipm[0][((32 >> 2) - 1) + (i << (cu_width_log2 - 2))];
				MPM_buffer_32x32->ipm_line_hor[i] = curr_cu_data_best_ptr_32->ipm[0][i + (((32 >> 2) - 1) << (cu_width_log2 - 2))];
			}
			break;
		case 1:
			for (int i = 0; i < 32; ++i)
			{
				IP_buffer_32x32->rec_line_hor[i + 32] = curr_cu_data_best_ptr_32->reco_y[32 - 1][i];
			}

			for (int i = 0; i < scu_h; i++)
			{
#pragma HLS loop_tripcount max=16
				for (int j = 0; j < scu_w; j++)
				{
#pragma HLS loop_tripcount max=16
					IP_map_scu_64x64->cur_map_scu[(cu_width >> 2) + j][i] = curr_cu_data_best_ptr_32->map_scu[j + (i << (cu_width_log2 - 2))];
				}
			}

			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_32x32->ipm_line_hor[i + (32 >> 2)] = curr_cu_data_best_ptr_32->ipm[0][i + (((32 >> 2) - 1) << (cu_width_log2 - 2))];
			}
			break;
		case 2:
			for (int i = 0; i < 32; ++i)
			{
				IP_buffer_32x32->rec_line_ver[i + 32] = curr_cu_data_best_ptr_32->reco_y[i][32 - 1];
			}

			for (int i = 0; i < scu_h; i++)
			{
#pragma HLS loop_tripcount max=16
				for (int j = 0; j < scu_w; j++)
				{
#pragma HLS loop_tripcount max=16
					IP_map_scu_64x64->cur_map_scu[j][(cu_height >> 2) + i] = curr_cu_data_best_ptr_32->map_scu[j + (i << (cu_width_log2 - 2))];
				}
			}

			for (int i = 0; i < (32 >> 2); i++)
			{
				MPM_buffer_32x32->ipm_line_ver[i] = curr_cu_data_best_ptr_32->ipm[0][((32 >> 2) - 1) + (i << (cu_width_log2 - 2))];
			}
			break;
		case 3:
			break;
		default:
			assert(0);
		}
	}
}

inline void update_nebs_bi_ver(ENC_ME_NEB_INFO* nebs_line_ver, S3 refi[256][2], S14 mv[256][2][2],
	int cu_width, int cu_height, int part_num)
{
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS pipeline 
#pragma HLS LOOP_TRIPCOUNT max=16
			nebs_line_ver[i].refi[0] = refi[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0];
			nebs_line_ver[i].refi[1] = refi[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1];
			nebs_line_ver[i].mv[0][0] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0][0];
			nebs_line_ver[i].mv[0][1] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0][1];
			nebs_line_ver[i].mv[1][0] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1][0];
			nebs_line_ver[i].mv[1][1] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1][1];
		}
		break;
	case 1:
		break;
	default:
		assert(0);
	}
}
inline void update_nebs_bi_hor(ENC_ME_NEB_INFO* nebs_line_hor, S3 refi[256][2], S14 mv[256][2][2],
	int cu_width, int cu_height, int part_num)
{
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < (cu_width >> 2); i++)
		{
#pragma HLS pipeline 
#pragma HLS LOOP_TRIPCOUNT max=16
			nebs_line_hor[i].refi[0] = refi[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0];
			nebs_line_hor[i].refi[1] = refi[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1];
			nebs_line_hor[i].mv[0][0] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0][0];
			nebs_line_hor[i].mv[0][1] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0][1];
			nebs_line_hor[i].mv[1][0] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1][0];
			nebs_line_hor[i].mv[1][1] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1][1];
		}
		break;
	case 1:
		break;
	default:
		assert(0);
	}
}
inline void update_nebs_qt(ENC_ME_NEB_INFO* nebs_line_hor, ENC_ME_NEB_INFO* nebs_line_ver, S3 refi[256][2], S14 mv[256][2][2],
	int cu_width, int cu_height, int part_num)
{
	switch (part_num)
	{
	case 0:
		for (int i = 0; i < (cu_width >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			nebs_line_hor[i].refi[0] = refi[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0];
			nebs_line_hor[i].refi[1] = refi[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1];
			nebs_line_hor[i].mv[0][0] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0][0];
			nebs_line_hor[i].mv[0][1] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0][1];
			nebs_line_hor[i].mv[1][0] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1][0];
			nebs_line_hor[i].mv[1][1] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1][1];
		}
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			nebs_line_ver[i].refi[0] = refi[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0];
			nebs_line_ver[i].refi[1] = refi[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1];
			nebs_line_ver[i].mv[0][0] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0][0];
			nebs_line_ver[i].mv[0][1] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0][1];
			nebs_line_ver[i].mv[1][0] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1][0];
			nebs_line_ver[i].mv[1][1] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1][1];
		}
		break;
	case 1:
		for (int i = 0; i < (cu_width >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			nebs_line_hor[i + (cu_width >> 2)].refi[0] = refi[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0];
			nebs_line_hor[i + (cu_width >> 2)].refi[1] = refi[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1];
			nebs_line_hor[i + (cu_width >> 2)].mv[0][0] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0][0];
			nebs_line_hor[i + (cu_width >> 2)].mv[0][1] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][0][1];
			nebs_line_hor[i + (cu_width >> 2)].mv[1][0] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1][0];
			nebs_line_hor[i + (cu_width >> 2)].mv[1][1] = mv[i + (((cu_height >> 2) - 1) * (cu_width >> 2))][1][1];
		}
		break;
	case 2:
		for (int i = 0; i < (cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			nebs_line_ver[i].refi[0] = refi[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0];
			nebs_line_ver[i].refi[1] = refi[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1];
			nebs_line_ver[i].mv[0][0] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0][0];
			nebs_line_ver[i].mv[0][1] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][0][1];
			nebs_line_ver[i].mv[1][0] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1][0];
			nebs_line_ver[i].mv[1][1] = mv[((cu_width >> 2) - 1) + (i * (cu_width >> 2))][1][1];
		}
		break;
	case 3:
		break;
	default:
		assert(0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/* me_line_map buf */
inline void get_me_line_map_up(U32* map_scu_up_line, U32* map_cu_mode_up_line,
	ENC_ME_LINE_MAP* me_line_map_ptr,
	int cu_width, int cu_height, int x_pos_pic, int y_pos_pic, int cdebug=0)
{
	int x_scu_pos_lcu = (x_pos_pic >> 2) & 15;
	int y_scu_pos_lcu = (y_pos_pic >> 2) & 15;

	if (cdebug)
	{
		if (y_scu_pos_lcu == 0)
		{
			assert(map_scu_up_line[0] == me_line_map_ptr->new_map_scu_above_line[16 + x_scu_pos_lcu - 1]);
			assert(map_cu_mode_up_line[0] == me_line_map_ptr->new_map_cu_mode_above_line[16 + x_scu_pos_lcu - 1]);
		}
		else
		{
			if (x_scu_pos_lcu == 0)
			{
				assert(map_scu_up_line[0] == me_line_map_ptr->map_scu_inner[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1]);
				assert(map_cu_mode_up_line[0] == me_line_map_ptr->map_cu_mode_inner[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1]);
			}
			else
			{
				assert(map_scu_up_line[0] == me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1]);
				assert(map_cu_mode_up_line[0] == me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1]);
			}
		}

		for (int i = 0; i < ((cu_width >> 2) + 1); i++)
		{
#pragma HLS pipeline
			if (x_scu_pos_lcu + i < 16)
			{
				assert(map_scu_up_line[i + 1] == (y_scu_pos_lcu == 0 ?
					me_line_map_ptr->new_map_scu_above_line[16 + x_scu_pos_lcu + i] :
					me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1]));
				assert(map_cu_mode_up_line[i + 1] == (y_scu_pos_lcu == 0 ?
					me_line_map_ptr->new_map_cu_mode_above_line[16 + x_scu_pos_lcu + i] :
					me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1]));
			}
		}
	}
	else
	{
		if (y_scu_pos_lcu == 0)
		{
			map_scu_up_line[0] = me_line_map_ptr->new_map_scu_above_line[16 + x_scu_pos_lcu - 1];
			map_cu_mode_up_line[0] = me_line_map_ptr->new_map_cu_mode_above_line[16 + x_scu_pos_lcu - 1];
		}
		else
		{
			if (x_scu_pos_lcu == 0)
			{
				map_scu_up_line[0] = me_line_map_ptr->map_scu_inner[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1];
				map_cu_mode_up_line[0] = me_line_map_ptr->map_cu_mode_inner[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu - 1];
			}
			else
			{
				map_scu_up_line[0] = me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1];
				map_cu_mode_up_line[0] = me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu - 1];
			}
		}

		for (int i = 0; i < ((cu_width >> 2) + 1); i++)
		{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=17
			map_scu_up_line[i + 1] = y_scu_pos_lcu == 0 ?
				me_line_map_ptr->new_map_scu_above_line[16 + x_scu_pos_lcu + i] :
				me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1];
			map_cu_mode_up_line[i + 1] = y_scu_pos_lcu == 0 ?
				me_line_map_ptr->new_map_cu_mode_above_line[16 + x_scu_pos_lcu + i] :
				me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu + i][y_scu_pos_lcu - 1];
		}
	}
}
inline void get_me_line_map_left(U32* map_scu_left_line, U32* map_cu_mode_left_line,
	ENC_ME_LINE_MAP* me_line_map_ptr,
	int cu_width, int cu_height, int x_pos_pic, int y_pos_pic, int cdebug=0)
{
	int x_scu_pos_lcu = (x_pos_pic >> 2) & 15;
	int y_scu_pos_lcu = (y_pos_pic >> 2) & 15;

	for (int i = 0; i < (cu_height >> 2); i++)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=16
		map_scu_left_line[i] = x_scu_pos_lcu == 0 ?
			me_line_map_ptr->map_scu_inner[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i] :
			me_line_map_ptr->map_scu_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i];
		map_cu_mode_left_line[i] = x_scu_pos_lcu == 0 ?
			me_line_map_ptr->map_cu_mode_inner[1 - me_line_map_ptr->curr_ctu_idx][15][y_scu_pos_lcu + i] :
			me_line_map_ptr->map_cu_mode_inner[me_line_map_ptr->curr_ctu_idx][x_scu_pos_lcu - 1][y_scu_pos_lcu + i];
	}
}

///////////////////////////////////////////////////////////////////////////////////////
/* cu_data */
template<size_t CUR_SIZE>
s64 prepare_cu_info_COM_combine_only_qt(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct)
{
	com_split_get_part_structure_only_qt(x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode_only_qt(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], bet_depth, &split_struct->num_split[i]);
	}

	s64 cost_temp = 0;
	U3 rate_bit = 0;

	if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);
		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr->split_mode, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		cost_temp += (rate_bit * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}
template<size_t CUR_SIZE>
void get_split_structure_COM_combine_only_qt(MD_FW* md_fw_ptr,
	U13 x0, U13 y0, U12 cup, U4 cud,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, COM_SPLIT_STRUCT* split_struct)
{
	com_split_get_part_structure_only_qt(x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode_only_qt(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], bet_depth, &split_struct->num_split[i]);
	}
}
template<size_t CUR_SIZE>
s64 get_split_mode_cost_COM_combine_only_qt(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* cu_data_temp_ptr, U13 x0, U13 y0, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width,
	U7 cu_height, U3 split_mode)
{
	s64 cost_temp = 0;
	U3 rate_bit = 0;

	if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height)
	{
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);
		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr->split_mode, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		cost_temp += (rate_bit * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}
template<size_t CUR_SIZE>
s64 prepare_cu_info_COM_combine_only_bt(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct)
{
	com_split_get_part_structure_only_bt(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], qt_depth, bet_depth + 1, &split_struct->num_split[i], split_mode);
	}

	U2 num_cons_pred_mode_loop[1];
	num_cons_pred_mode_loop[0] = (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) ? 2 : 1;
	s64 cost_temp = 0;
	U3 bit_cnt;
	//U3 part_num = 0;
	U3 rate_bit = 0;
	if (cons_pred_mode == NO_MODE_CONS && ((md_fw_ptr->slice_type)) != 1) {
		if (num_cons_pred_mode_loop[0] == 1) {
			cons_pred_mode_child[0] = NO_MODE_CONS;
		}
		else {
			cons_pred_mode_child[0] = (((cons_flag != 0 ? ONLY_INTER : ONLY_INTRA)));
		}
	}
	else {
		cons_pred_mode_child[0] = cons_pred_mode;
	}

	if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);

		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr->split_mode, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) {
			rate_bit += 1;
		}
		bit_cnt = rate_bit;

		cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}
template<size_t CUR_SIZE>
s64 prepare_cu_info_COM_combine(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct)
{

	md_com_split_get_part_structure(split_mode, x0, y0, cu_width, cu_height, cup, cud, md_fw_ptr->log2_culine, split_struct);

	for (U3 i = 0; i < split_struct->part_count; i++)
	{
#pragma HLS loop_tripcount max=4
		com_check_split_mode(split_struct->x_pos[i], split_struct->y_pos[i], md_fw_ptr, split_struct->split_allow[i], split_struct->log_cuw[i],
			split_struct->log_cuh[i], qt_depth, split_mode == NO_SPLIT ? bet_depth : (U3)(bet_depth + 1), &split_struct->num_split[i], split_mode);
	}

	if (split_mode == NO_SPLIT)
		return 0;

	U2 num_cons_pred_mode_loop[1];
	num_cons_pred_mode_loop[0] = (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) ? 2 : 1;
	s64 cost_temp = 0;
	U3 bit_cnt;
	//U3 part_num = 0;
	U3 rate_bit = 0;
	if (cons_pred_mode == NO_MODE_CONS && ((md_fw_ptr->slice_type)) != 1) {
		if (num_cons_pred_mode_loop[0] == 1) {
			cons_pred_mode_child[0] = NO_MODE_CONS;
		}
		else {
			cons_pred_mode_child[0] = (((cons_flag != 0 ? ONLY_INTER : ONLY_INTRA)));
		}
	}
	else {
		cons_pred_mode_child[0] = cons_pred_mode;
	}

	if (x0 + cu_width <= md_fw_ptr->pic_width && y0 + cu_height <= md_fw_ptr->pic_height) {
		/* consider CU split flag */
		com_set_split_mode(split_mode, cud, 0, cu_width, cu_height, cu_width, cu_data_temp_ptr->split_mode);

		rate_bit += enc_eco_split_mode_est(md_fw_ptr, cu_data_temp_ptr->split_mode, cud, 0, cu_width, cu_height, cu_width, qt_depth, bet_depth, x0, y0);
		if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, split_mode, md_fw_ptr->slice_type)) {
			rate_bit += 1;
		}
		bit_cnt = rate_bit;

		cost_temp += (bit_cnt * lambdaY + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
	}
	return cost_temp;
}

template<size_t CUR_SIZE>
void copy_cu_data_array_src(ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* src,
	U14 x_cur_cu, U14 y_cur_cu, U3 cu_width_log2, U3 cu_height_log2, U3 log2_prev_cuw)
{
	#if COPY_CU_DATA_ARRAY_SRC_TEMPLATE
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	
	#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3	
	#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2	
	#endif

	int x_cur_cu_scu = x_cur_cu >> 2;
	int y_cur_cu_scu = y_cur_cu >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);

	dst->x_chroma = src->x_chroma;
	dst->y_chroma = src->y_chroma;
	dst->x_last = src->x_last;
	dst->y_last = src->y_last;
	dst->last_child = src->last_child;

	for (int i = 0; i < cuh_scu; i++)
	{
		#pragma HLS LOOP_TRIPCOUNT max= CUR_SIZE/4
		int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (cu_width_log2 - 2);

		for (int j = 0; j < cuw_scu; j++)
		{
			#pragma HLS LOOP_TRIPCOUNT max= CUR_SIZE/4
			#pragma HLS pipeline II=1
			(dst->pred_mode + idx_dst)[j] = (src->pred_mode + idx_src)[j];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[j] = (src->cu_luma_qp + idx_src)[j];
#endif
			(dst->mpm[0] + idx_dst)[j] = (src->mpm[0] + idx_src)[j];
			(dst->mpm[1] + idx_dst)[j] = (src->mpm[1] + idx_src)[j];
			(dst->ipm[0] + idx_dst)[j] = (src->ipm[0] + idx_src)[j];

			(dst->affine_flag + idx_dst)[j] = (src->affine_flag + idx_src)[j];
			(dst->smvd_flag + idx_dst)[j] = (src->smvd_flag + idx_src)[j];
			(dst->depth + idx_dst)[j] = (src->depth + idx_src)[j];
			(dst->map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];
			(dst->map_cu_mode + idx_dst)[j] = (src->map_cu_mode + idx_src)[j];
			(dst->map_pb_tb_part + idx_dst)[j] = (src->map_pb_tb_part + idx_src)[j];

			(dst->umve_flag + idx_dst)[j] = (src->umve_flag + idx_src)[j];
			(dst->umve_idx + idx_dst)[j] = (src->umve_idx + idx_src)[j];
			(dst->skip_idx + idx_dst)[j] = (src->skip_idx + idx_src)[j];
			(dst->mvr_idx + idx_dst)[j] = (src->mvr_idx + idx_src)[j];
			(dst->mvp_from_hmvp_flag + idx_dst)[j] = (src->mvp_from_hmvp_flag + idx_src)[j];
			(dst->ipf_flag + idx_dst)[j] = (src->ipf_flag + idx_src)[j];

			(dst->mv + idx_dst)[j][0][0] = (src->mv + idx_src)[j][0][0];
			(dst->mv + idx_dst)[j][0][1] = (src->mv + idx_src)[j][0][1];
			(dst->mv + idx_dst)[j][1][0] = (src->mv + idx_src)[j][1][0];
			(dst->mv + idx_dst)[j][1][1] = (src->mv + idx_src)[j][1][1];
			(dst->mvd + idx_dst)[j][0][0] = (src->mvd + idx_src)[j][0][0];
			(dst->mvd + idx_dst)[j][0][1] = (src->mvd + idx_src)[j][0][1];
			(dst->mvd + idx_dst)[j][1][0] = (src->mvd + idx_src)[j][1][0];
			(dst->mvd + idx_dst)[j][1][1] = (src->mvd + idx_src)[j][1][1];
			(dst->refi + idx_dst)[j][0] = (src->refi + idx_src)[j][0];
			(dst->refi + idx_dst)[j][1] = (src->refi + idx_src)[j][1];

			(dst->num_nz_coef[0] + idx_dst)[j] = (src->num_nz_coef[0] + idx_src)[j];
			(dst->pb_part + idx_dst)[j] = (src->pb_part + idx_src)[j];
			(dst->tb_part + idx_dst)[j] = (src->tb_part + idx_src)[j];
			for (int m = 0; m < 8; m++)
			{
				(dst->mpm_ext[m] + idx_dst)[j] = (src->mpm_ext[m] + idx_src)[j];
			}	
			for(int k = 0; k < 4; k++)
			{
				for(int l = 0; l < 4; l++)
				{
					dst->coef_y[y_cur_cu + i*4+k][x_cur_cu + j*4+l] = src->coef_y[i*4+k][j*4+l];
					dst->reco_y[y_cur_cu + i*4+k][x_cur_cu + j*4+l] = src->reco_y[i*4+k][j*4+l];
				}
			}					
		}
	}
}

template<size_t CUR_SIZE>
void copy_cu_data_array_src(ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* src,
	U14 x_cur_cu, U14 y_cur_cu, U3 cu_width_log2, U3 cu_height_log2, U3 log2_prev_cuw)
{
	#if COPY_CU_DATA_ARRAY_SRC_TEMPLATE
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=dst->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=dst->mvd cyclic factor=2 dim=3	
	#pragma HLS ARRAY_PARTITION variable=dst->refi cyclic factor=2 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->coef_y cyclic factor=4 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->reco_y cyclic factor=4 dim=2

	#pragma HLS ARRAY_PARTITION variable=src->mpm_ext cyclic factor=8 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->mpm cyclic factor=2 dim=1
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mv cyclic factor=2 dim=3
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=2
	#pragma HLS ARRAY_PARTITION variable=src->mvd cyclic factor=2 dim=3	
	#pragma HLS ARRAY_PARTITION variable=src->refi cyclic factor=2 dim=2		
	#endif

	int x_cur_cu_scu = x_cur_cu >> 2;
	int y_cur_cu_scu = y_cur_cu >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);

	dst->x_chroma = src->x_chroma;
	dst->y_chroma = src->y_chroma;
	dst->x_last = src->x_last;
	dst->y_last = src->y_last;
	dst->last_child = src->last_child;

	for (int i = 0; i < cuh_scu; i++)
	{
		#pragma HLS LOOP_TRIPCOUNT max= CUR_SIZE/4
		int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (cu_width_log2 - 2);

		for (int j = 0; j < cuw_scu; j++)
		{
			#pragma HLS LOOP_TRIPCOUNT max= CUR_SIZE/4
			#pragma HLS PIPELINE II=1
			(dst->pred_mode + idx_dst)[j] = (src->pred_mode + idx_src)[j];
#if CUDQP_QP_MAP
			(dst->cu_luma_qp + idx_dst)[j] = (src->cu_luma_qp + idx_src)[j];
#endif
			(dst->mpm[0] + idx_dst)[j] = (src->mpm[0] + idx_src)[j];
			(dst->mpm[1] + idx_dst)[j] = (src->mpm[1] + idx_src)[j];
			(dst->ipm[0] + idx_dst)[j] = (src->ipm[0] + idx_src)[j];

			(dst->affine_flag + idx_dst)[j] = (src->affine_flag + idx_src)[j];
			(dst->smvd_flag + idx_dst)[j] = (src->smvd_flag + idx_src)[j];
			(dst->depth + idx_dst)[j] = (src->depth + idx_src)[j];
			(dst->map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];
			(dst->map_cu_mode + idx_dst)[j] = (src->map_cu_mode + idx_src)[j];
			(dst->map_pb_tb_part + idx_dst)[j] = (src->map_pb_tb_part + idx_src)[j];

			(dst->umve_flag + idx_dst)[j] = (src->umve_flag + idx_src)[j];
			(dst->umve_idx + idx_dst)[j] = (src->umve_idx + idx_src)[j];
			(dst->skip_idx + idx_dst)[j] = (src->skip_idx + idx_src)[j];
			(dst->mvr_idx + idx_dst)[j] = (src->mvr_idx + idx_src)[j];
			(dst->mvp_from_hmvp_flag + idx_dst)[j] = (src->mvp_from_hmvp_flag + idx_src)[j];
			(dst->ipf_flag + idx_dst)[j] = (src->ipf_flag + idx_src)[j];

			(dst->mv + idx_dst)[j][0][0] = (src->mv + idx_src)[j][0][0];
			(dst->mv + idx_dst)[j][0][1] = (src->mv + idx_src)[j][0][1];
			(dst->mv + idx_dst)[j][1][0] = (src->mv + idx_src)[j][1][0];
			(dst->mv + idx_dst)[j][1][1] = (src->mv + idx_src)[j][1][1];
			(dst->mvd + idx_dst)[j][0][0] = (src->mvd + idx_src)[j][0][0];
			(dst->mvd + idx_dst)[j][0][1] = (src->mvd + idx_src)[j][0][1];
			(dst->mvd + idx_dst)[j][1][0] = (src->mvd + idx_src)[j][1][0];
			(dst->mvd + idx_dst)[j][1][1] = (src->mvd + idx_src)[j][1][1];
			(dst->refi + idx_dst)[j][0] = (src->refi + idx_src)[j][0];
			(dst->refi + idx_dst)[j][1] = (src->refi + idx_src)[j][1];

			(dst->num_nz_coef[0] + idx_dst)[j] = (src->num_nz_coef[0] + idx_src)[j];
			(dst->pb_part + idx_dst)[j] = (src->pb_part + idx_src)[j];
			(dst->tb_part + idx_dst)[j] = (src->tb_part + idx_src)[j];
			for (int m = 0; m < 8; m++)
			{
				(dst->mpm_ext[m] + idx_dst)[j] = (src->mpm_ext[m] + idx_src)[j];
			}
			for(int k = 0; k < 4; k++)
			{
				for(int l = 0; l < 4; l++)
				{
					dst->coef_y[y_cur_cu + i*4+k][x_cur_cu + j*4+l] = src->coef_y[i*4+k][j*4+l];
					dst->reco_y[y_cur_cu + i*4+k][x_cur_cu + j*4+l] = src->reco_y[i*4+k][j*4+l];
				}
			}
		}
	}
}

template<size_t CUR_SIZE_CHROMA>
int copy_cu_data_array_src_chroma(ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE_CHROMA * 2>* dst, ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE_CHROMA * 2>* src, 
	U14 x_cur_cu, U14 y_cur_cu, U3 cu_width_log2, U3 cu_height_log2, U3 log2_prev_cuw)
{
	int x_cur_cu_scu = x_cur_cu >> 2;
	int y_cur_cu_scu = y_cur_cu >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);

	for (int i = 0; i < 16; i++)
	{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=16
		int idx_dst = ((y_cur_cu_scu + i) << (log2_prev_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (cu_width_log2 - 2);

		for (int j = 0; j < 16; j++)
		{
			if(i < cuh_scu && j < cuw_scu)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				(dst->ipm[1] + idx_dst)[j] = (src->ipm[1] + idx_src)[j];
				(dst->map_scu + idx_dst)[j] = (src->map_scu + idx_src)[j];
				for (int k = 1; k < 3; k++) 
				{
					(dst->num_nz_coef[k] + idx_dst)[j] = (src->num_nz_coef[k] + idx_src)[j];
				}
			}
			else
			{
				break;
			}
		}
	}

	for (int i = 0; i < 32; i++)
	{
#pragma HLS PIPELINE
#pragma HLS LOOP_TRIPCOUNT max=32
		for (int j = 0; j < 32; j++)
		{
#pragma HLS LOOP_TRIPCOUNT max=32
			if(i < (1 << (cu_height_log2 - 1)) && j < (1 << (cu_width_log2 - 1)))
			{
				dst->coef_u[i][j] = src->coef_u[i][j];
				dst->coef_v[i][j] = src->coef_v[i][j];
				dst->reco_u[i][j] = src->reco_u[i][j];
				dst->reco_v[i][j] = src->reco_v[i][j];
			}
			else
			{
				break;
			}
		}
	}

	return 0;
}

extern void compare_store_top(MD_FW* md_fw_ptr,
	s64& cost_best, s64 cost,
	ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_ptr_32, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best_ptr, ENC_CU_DATA_ARRAY_SIZE<32>* curr_cu_data_best_ptr_32,
	ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr2,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2 cons_pred_mode_child, U2 tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num);

template<size_t CUR_SIZE>
void copy_split_mode(
    ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, S4 best_index)
{
#pragma HLS INLINE off
		int x_cur_cu_scu = 0;
		int y_cur_cu_scu = 0;
		int cuh_scu = 1 << (cu_height_log2 - 2);
		int cuw_scu = 1 << (cu_width_log2 - 2);
		for (int i = 0; i < cuh_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
			int idx_dst = ((y_cur_cu_scu + i) << (cu_width_log2 - 2)) + x_cur_cu_scu;
			int idx_src = i << (cu_width_log2 - 2);

            for (int j = 0; j < cuw_scu; j++)
            {
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
#pragma HLS PIPELINE
                for (int m = 0; m < 5; m++)
                {
                    if (m >= cud) 
                    {
#pragma HLS LOOP_TRIPCOUNT max=5
                        for (int n = 0; n < (NUM_BLOCK_SHAPE); n++)
                        {
#pragma HLS LOOP_TRIPCOUNT max=7                   
                            (curr_cu_data_best_ptr->split_mode[m][n] + idx_dst)[j] = (curr_cu_data_ptr[best_index].split_mode[m][n] + idx_src)[j];

                        }
                    }
                }
            }
		}

}
template<size_t CUR_SIZE>
void cost_compare_fixed_point_one_time_paral(
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, S4 best_index)
{
		copy_split_mode<CUR_SIZE>(curr_cu_data_ptr, curr_cu_data_best_ptr,
						cu_width_log2, cu_height_log2, cud, best_index);
		copy_cu_data_array_src<CUR_SIZE>(curr_cu_data_best_ptr, &curr_cu_data_ptr[best_index], 0, 0, cu_width_log2, cu_height_log2, cu_width_log2);
}
template<size_t CUR_SIZE>
void cost_compare_fixed_point_one_time(MD_FW* md_fw_ptr, 
	s64* cost_best, s64* cost, 
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3* split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2* cons_pred_mode_child, U2* tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
#pragma HLS INLINE off	
#if SYN_TOP_COST_COMPARE_FIXED_POINT_ONE_TIME
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->split_mode complete dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->refi cyclic factor=2 dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->split_mode complete dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->refi cyclic factor=2 dim=2
#endif // SYN_TOP_COST_COMPARE_FIXED_POINT_ONE_TIME

	S4 best_index = -1;
	for (U3 i = 0; i < split_mode_num; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max= 4
#pragma HLS PIPELINE
		if (*cost_best > cost[i])
		{
			*cost_best = cost[i];
			best_index = i;
		}
	}

	if (best_index != -1)
	{
		cost_compare_fixed_point_one_time_paral<CUR_SIZE>(
				curr_cu_data_ptr, curr_cu_data_best_ptr,
				cu_width_log2, cu_height_log2, cud, best_index);

		*best_split_mode = split_mode[best_index];
		*best_cons_pred_mode = cons_pred_mode_child[best_index];
		*best_child_status = tree_status_child[best_index];
	}

	if (*best_split_mode != NO_SPLIT)
	{
		*tree_status_temp = TREE_C;
	}
}
template<size_t CUR_SIZE>
void copy_split_mode(
    ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, S4 best_index)
{
#pragma HLS INLINE off
		int x_cur_cu_scu = 0;
		int y_cur_cu_scu = 0;
		int cuh_scu = 1 << (cu_height_log2 - 2);
		int cuw_scu = 1 << (cu_width_log2 - 2);
		for (int i = 0; i < cuh_scu; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
			int idx_dst = ((y_cur_cu_scu + i) << (cu_width_log2 - 2)) + x_cur_cu_scu;
			int idx_src = i << (cu_width_log2 - 2);

            for (int j = 0; j < cuw_scu; j++)
            {
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
#pragma HLS PIPELINE
                for (int m = 0; m < 5; m++)
                {
                    if (m >= cud) 
                    {
#pragma HLS LOOP_TRIPCOUNT max=5
                        for (int n = 0; n < (NUM_BLOCK_SHAPE); n++)
                        {
#pragma HLS LOOP_TRIPCOUNT max=7                   
                            (curr_cu_data_best_ptr->split_mode[m][n] + idx_dst)[j] = (curr_cu_data_ptr[best_index].split_mode[m][n] + idx_src)[j];

                        }
                    }
                }
            }
		}

}
template<size_t CUR_SIZE>
void cost_compare_fixed_point_one_time_paral(
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, S4 best_index)
{
		copy_split_mode<CUR_SIZE>(curr_cu_data_ptr, curr_cu_data_best_ptr,
						cu_width_log2, cu_height_log2, cud, best_index);
		copy_cu_data_array_src<CUR_SIZE>(curr_cu_data_best_ptr, &curr_cu_data_ptr[best_index], 0, 0, cu_width_log2, cu_height_log2, cu_width_log2);
}

template<size_t CUR_SIZE>
void cost_compare_fixed_point_one_time(MD_FW* md_fw_ptr,
	s64* cost_best, s64* cost,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3* split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2* cons_pred_mode_child, U2* tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num)
{
#pragma HLS INLINE off
#if SYN_TOP_COST_COMPARE_FIXED_POINT_ONE_TIME
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->split_mode complete dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best_ptr->refi cyclic factor=2 dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->split_mode complete dim=2

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->mvd cyclic factor=2 dim=3	
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_ptr->refi cyclic factor=2 dim=2		

#endif // SYN_TOP_COST_COMPARE_FIXED_POINT_ONE_TIME
	S4 best_index = -1;
	for (U3 i = 0; i < split_mode_num; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max= 4
#pragma HLS PIPELINE
		if (*cost_best > cost[i])
		{
			*cost_best = cost[i];
			best_index = i;
		}
	}

	if (best_index != -1)
	{
		cost_compare_fixed_point_one_time_paral<CUR_SIZE>(
				curr_cu_data_ptr, curr_cu_data_best_ptr,
				cu_width_log2, cu_height_log2, cud, best_index);

		*best_split_mode = split_mode[best_index];
		*best_cons_pred_mode = cons_pred_mode_child[best_index];
		*best_child_status = tree_status_child[best_index];
	}

	if (*best_split_mode != NO_SPLIT)
	{
		*tree_status_temp = TREE_C;
	}
}

template<size_t CUR_SIZE>
void store_enc_set_split_mode(ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best, 
	U14 x0, U14 y0, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
#pragma HLS INLINE off
	int x_cur_cu_scu = (x0 - x_pos) >> 2;
	int y_cur_cu_scu = (y0 - y_pos) >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);
	for (int i = 0; i < cuh_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
		int idx_dst = ((y_cur_cu_scu + i) << (prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (cu_width_log2 - 2);

		for (int j = 0; j < cuw_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
#pragma HLS PIPELINE
			for (int m = 0; m < 5; m++) {
#pragma HLS LOOP_TRIPCOUNT max=5
				if (m >= cud) {
					for (int n = 0; n < (NUM_BLOCK_SHAPE); n++) {
						(prev_cu_data_ptr->split_mode[m][n] + idx_dst)[j] = (curr_cu_data_best->split_mode[m][n] + idx_src)[j];
					}
				}
			}
		}
	}
}

template<size_t CUR_SIZE>
void store_enc_info_best(MD_FW* md_fw_ptr, 
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best, 
	U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, 
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
#pragma HLS INLINE off
#if STORE_ENC_INFO_BEST_TEMPLATE_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2

#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2
#endif
	/* restore best data */
	com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width,
		curr_cu_data_best->split_mode);
	if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type)) 
	{
		com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width, curr_cu_data_best->split_mode);
	}

	store_enc_set_split_mode<CUR_SIZE>(curr_cu_data_best, x0, y0, cu_width_log2, cu_height_log2,
		cud, prev_cu_data_ptr, x_pos, y_pos, prev_up_log2_sub_cuw);

	prev_cu_data_ptr->get_cu_data_from_curblk(curr_cu_data_best, (x0 - x_pos), (y0 - y_pos), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
	//copy_cu_data_array_dst(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
	//	cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status);

	//update_map_scu(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}

template<size_t CUR_SIZE>
void store_enc_set_split_mode(ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best, 
	U14 x0, U14 y0, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE*2>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
	int x_cur_cu_scu = (x0 - x_pos) >> 2;
	int y_cur_cu_scu = (y0 - y_pos) >> 2;
	int cuh_scu = 1 << (cu_height_log2 - 2);
	int cuw_scu = 1 << (cu_width_log2 - 2);
	for (int i = 0; i < cuh_scu; i++) {
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
		int idx_dst = ((y_cur_cu_scu + i) << (prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
		int idx_src = i << (cu_width_log2 - 2);

		for (int j = 0; j < cuw_scu; j++) {
#pragma HLS LOOP_TRIPCOUNT max=CUR_SIZE/4
#pragma HLS PIPELINE
			for (int m = 0; m < 5; m++) {
#pragma HLS LOOP_TRIPCOUNT max=5
				if (m >= cud) {
					for (int n = 0; n < (NUM_BLOCK_SHAPE); n++) {
						(prev_cu_data_ptr->split_mode[m][n] + idx_dst)[j] = (curr_cu_data_best->split_mode[m][n] + idx_src)[j];
					}
				}
			}
		}
	}
}

template<size_t CUR_SIZE>
void store_enc_info_best(MD_FW* md_fw_ptr,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* curr_cu_data_best,
	U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try,
	ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE * 2>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw)
{
#pragma HLS INLINE off
#if STORE_ENC_INFO_BEST_TEMPLATE_SYNTHESIS
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->split_mode cyclic factor=7 dim=2

#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=prev_cu_data_ptr->split_mode cyclic factor=7 dim=2
#endif
	/* restore best data */
	com_set_split_mode(*best_split_mode, cud, 0, cu_width, cu_height, cu_width,
		curr_cu_data_best->split_mode);
	if (cons_pred_mode == NO_MODE_CONS && com_constrain_pred_mode(cu_width, cu_height, *best_split_mode, md_fw_ptr->slice_type))
	{
		com_set_cons_pred_mode(best_cons_pred_mode[0], cud, 0, cu_width, cu_height, cu_width, curr_cu_data_best->split_mode);
	}

	store_enc_set_split_mode<CUR_SIZE>(curr_cu_data_best, x0, y0, cu_width_log2, cu_height_log2,
		cud, prev_cu_data_ptr, x_pos, y_pos, prev_up_log2_sub_cuw);

	prev_cu_data_ptr->get_cu_data_from_curblk(curr_cu_data_best, (x0 - x_pos), (y0 - y_pos), cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw);
	//copy_cu_data_array_dst(prev_up_cu_data_temp_array_ptr, curr_cu_data_best, x0 - x_pos, y0 - y_pos,
	//	cu_width_log2, cu_height_log2, prev_up_log2_sub_cuw, cud, tree_status);

	//update_map_scu(md_fw_ptr, curr_cu_data_best, x0, y0, cu_width, cu_height, tree_status);
}


template<typename T, size_t dim1>
void cpy_1d_2_1d(T src[dim1], T dst[dim1])
{
#pragma HLS ARRAY_PARTITION variable=src complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst complete dim=1

	for (int i = 0; i < dim1; i++)
#pragma HLS pipeline
		dst[i] = src[i];
}
template<typename T, size_t dim1, size_t dim2>
void cpy_2d_2_2d(T src[dim1][dim2], T dst[dim1][dim2])
{
#pragma HLS ARRAY_PARTITION variable=src complete dim=2
//#pragma HLS ARRAY_PARTITION variable=src complete dim=0
#pragma HLS ARRAY_PARTITION variable=dst complete dim=2

	for (int i = 0; i < dim1; i++)
#pragma HLS pipeline
		for (int j = 0; j < dim2; j++)
			dst[i][j] = src[i][j];
}
template<typename T, size_t dim1, size_t dim2, size_t dim3>
void cpy_3d_2_3d(T src[dim1][dim2][dim3], T dst[dim1][dim2][dim3])
{
#pragma HLS ARRAY_PARTITION variable=src complete dim=2
#pragma HLS ARRAY_PARTITION variable=src complete dim=3
//#pragma HLS ARRAY_PARTITION variable=src complete dim=0
#pragma HLS ARRAY_PARTITION variable=dst complete dim=2
#pragma HLS ARRAY_PARTITION variable=dst complete dim=3

	for (int i = 0; i < dim1; i++)
#pragma HLS pipeline
		for (int j = 0; j < dim2; j++)
			for (int k = 0; k < dim3; k++)
				dst[i][j][k] = src[i][j][k];
}

pel* get_ref_y(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx);
pel* get_ref_u(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx, pel RefWindowLCU_UP[(SWH * SWW) >> 2]);
pel* get_ref_v(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx, pel RefWindowLCU_VP[(SWH * SWW) >> 2]);
pel* get_ref_u(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx);
pel* get_ref_v(strFetch_ref_window* Fetch_Ref_window_ptr, s8 refi, int lidx);

template<size_t CUR_SIZE>
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
template<size_t CUR_SIZE>
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY_SIZE<2 * CUR_SIZE>* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
template<size_t CUR_SIZE>
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
template<size_t CUR_SIZE>
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
template<size_t CUR_SIZE>
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE>* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
template<size_t CUR_SIZE>
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE>* dst, ENC_CU_DATA_ARRAY_CHROMA_SIZE<CUR_SIZE * 2>* src, int depth)
{
	//for (int i = 0; i < size; ++i){
	if (tree == TREE_LC) {
		dst->x_chroma = x;
		dst->y_chroma = y;
		dst->x_last = x + w - 4;
		dst->y_last = y + h - 4;
		dst->last_child = FALSE;
	}
	else {
		dst->x_chroma = src->x_chroma;
		dst->y_chroma = src->y_chroma;
		dst->x_last = src->x_last;
		dst->y_last = src->y_last;
		dst->last_child = x + w == src->x_last + 4 && y + h == src->y_last + 4;
	}
	//}
	return;
	print_cu_split(x, y, w, h, tree, num, part, dst[0].x_chroma, dst[0].y_chroma, dst[0].last_child, depth);
	printf(", last=(%d %d), father=(%d %d %d %d), use=%s\n",
		dst[0].x_last, dst[0].y_last,
		src->x_chroma, src->y_chroma, src->x_last, src->y_last,
		src->last_child ? "true" : "false");
}
void cu_data_chroma_pos(int x, int y, int w, int h, int tree, int num, int part, ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY* src, int depth);


s16 get_g_map_idx(int w, int h, int x0, int y0);
S7 get_ip_array_size_idx(U8 cu_width, U8 cu_height);
void copy_motion_table(COM_MOTION* motion_dst, s8* cnt_cands_dst, const COM_MOTION* motion_src, const s8 cnt_cands_src);
void com_get_split_mode(U3* split_mode, U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s,
	U8(*split_mode_buf)[NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))]);
void com_check_split_mode_ECO(MD_FW* md_fw_ptr, U1* split_allow, U3 cu_width_log2,
	U3 cu_height_log2, U1 boundary, U1 boundary_b, U1 boundary_r, U3 qt_depth, U3 bet_depth);

U3 enc_eco_split_mode_est(MD_FW* md_fw_ptr,
	U8 split_mode_buf[MAX_CU_DEPTH][NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))],
	U4 cud, U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s, U3 qt_depth, U3 bet_depth, U13 x, U13 y);
U3 enc_eco_split_mode_est(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U4 cud,
	U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s, U3 qt_depth, U3 bet_depth, U13 x, U13 y);
U3 enc_eco_split_mode_est_8(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U4 cud,
	U12 cup, U7 cu_width, U7 cu_height, U7 lcu_s, U3 qt_depth, U3 bet_depth, U13 x, U13 y);

void cu_nz_cln(int dst[4][3]);
//void init_pb_part(MD_COM_MODE *mod_info_curr);
//void init_tb_part(MD_COM_MODE *mod_info_curr);
void enc_rdoq_bit_est(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array);
int mode_cu_init(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* bst_info_ptr, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status,
	U2* cons_pred_mode, ENC_CU_DATA_ARRAY* cu_data_temp_ptr);
s64 init_basic_top(MD_FW* md_fw_ptr, s64 lambdaY, int cu_width_log2, int cu_height_log2, U4 cud, MD_COM_MODE* bst_info_ptr, U3 qt_depth, U3 bet_depth, int x0, int y0, int cup, u8 tree_status, u8* cons_pred_mode, MD_COM_MODE* mod_info_curr_temp_ptr, COM_PART_INFO* mod_info_curr_pb_info, COM_PART_INFO* mod_info_curr_tb_info, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, u8 ctx_cons_pred_mode[1]);
void init_skip_merge_rdo(SKIP_RDO* skip_rdo, MERGE_RDO* merge_rdo, S7 mode_list[MAX_INTER_SKIP_RDO]);
void get_colocal_scu_coord(int bx, int by, int* xpos, int* ypos, int pic_width_in_scu, int pic_height_in_scu);
void get_col_mv_from_list0(SKIP_REFP* refp, u32 ptr_curr, int co_x_scu_of_ctu, int co_y_scu_of_ctu, s16 mvp[2][2]);

void analyze_intra_cu(MD_FW* md_fw_ptr, RMD_OUTPUT* g_rmd_output, ENC_ME_LINE_MAP* me_line_map_ptr, u8 tree_status, u8 cons_pred_mode, MD_COM_MODE* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_INPUT* md_input_ptr, strFetch_Orig_Lcu* Orig_Lcu_ptr, IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, u8 cons_pred_mode_inter, s64 cost_best_inter[1], int num_nz_inter[4][3], int best_ipd[4], s64 best_dist_y[1], int best_bin_cnt_luma[1], s8 coef_rec_write_flag[1], s64 cost_best_intra[1]);

s64 md_com_NS(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output, strFetch_ref_window* Fetch_Ref_window_ptr, strFetch_Orig_Lcu* Fetch_Orig_Lcu_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, int boundary, int isNS_allow, int NS_luma_pred_mode[1], int x0,
	int y0, int cup, int cu_width_log2, int cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, u8 cons_pred_mode,
	u8 tree_status, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_COM_MODE* mod_info_best,
	ENC_FME_INFO_ALL* fme_mv_info,

	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr);

#if USE_SPEED_LEVEL
int get_rmd_result_new(const RMD_OUTPUT* rmd_output, int cu_width_log2, int cu_heght_log2, int x, int y, u8 ipred_list[], U8 speed_level);
#else
int get_rmd_result_new(const RMD_OUTPUT* rmd_output, int cu_width_log2, int cu_heght_log2, int x, int y, u8 ipred_list[]);
#endif


//=======================================================================================================================================================
// =======================================================================================================================================================
u8 transform_hor_8(U8 w, U8 h, U4 bit_depth, int num_nz[4][3], s16 resi[8][8], s16 coef_tmp[8][8], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);	//added by luohm
u8 transform_ver_8(U8 w, U8 h, U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3],
	s16 coef_tmp[8][8], s16 coef[8][8], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);	//added by luohm
void inv_transform_ver_8(U8 w, U8 h, U4 bit_depth, U6 qp_y, MD_COM_MODE* mod_info_curr_ptr, s16 coef_tmp[8][8], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_hor_8(U8 w, U8 h, MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info,
//	U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[8][8], s32* dist, pel rec[64], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[8][8], U3 type, U1 is_intra, pel* org);
int quant_nnz_4x4(RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[4 * 4], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[4 * 4], s16 coef_u[4 * 4], s16 coef_v[4 * 4]);
int quant_nnz_4x8(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[4 * 8], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[4 * 8], s16 coef_u[4 * 8], s16 coef_v[4 * 8]);
int quant_nnz_8x4(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[8 * 4], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[8 * 4], s16 coef_u[8 * 4], s16 coef_v[8 * 4]);
int quant_nnz_8x8(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[8 * 8], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[8 * 8], s16 coef_u[8 * 8], s16 coef_v[8 * 8]);
int quant_nnz_16x4(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16 * 4], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[16 * 4], s16 coef_u[16 * 4], s16 coef_v[16 * 4]);
int quant_nnz_4x16(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[4 * 16], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[4 * 16], s16 coef_u[4 * 16], s16 coef_v[4 * 16]);
int quant_nnz_16x8(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16 * 8], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[16 * 8], s16 coef_u[16 * 8], s16 coef_v[16 * 8]);
int quant_nnz_8x16(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[8 * 16], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[8 * 16], s16 coef_u[8 * 16], s16 coef_v[8 * 16]);
int quant_nnz_16x16(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16 * 16], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[16 * 16], s16 coef_u[16 * 16], s16 coef_v[16 * 16]);
int quant_nnz_32x4(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[32 * 4], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[32 * 4], s16 coef_u[32 * 4], s16 coef_v[32 * 4]);
int quant_nnz_4x32(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[4 * 32], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[4 * 32], s16 coef_u[4 * 32], s16 coef_v[4 * 32]);
int quant_nnz_8x32(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[8 * 32], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[8 * 32], s16 coef_u[8 * 32], s16 coef_v[8 * 32]);
int quant_nnz_32x8(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[32 * 8], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[32 * 8], s16 coef_u[32 * 8], s16 coef_v[32 * 8]);
int quant_nnz_32x16(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[32 * 16], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[32 * 16], s16 coef_u[32 * 16], s16 coef_v[32 * 16]);
int quant_nnz_16x32(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16 * 32], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[16 * 32], s16 coef_u[16 * 32], s16 coef_v[16 * 32]);
int quant_nnz_32x32(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[3][2], s32 rdoq_est_level[(6 * 4)][2], s32 rdoq_est_last[2][6][12][2], s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[32 * 32], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[32 * 32], s16 coef_u[32 * 32], s16 coef_v[32 * 32]);

u8 transform_hor_4x4(U4 bit_depth, s16 resi[4 * 4], s16 coef_tmp[4 * 4], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_4x8(U4 bit_depth,  s16 resi[4 * 8], s16 coef_tmp[4 * 8], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_8x4(U4 bit_depth, s16 resi[8 * 4], s16 coef_tmp[8 * 4], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_8x8(U4 bit_depth, s16 resi[8 * 8], s16 coef_tmp[8 * 8], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_16x4(U4 bit_depth,  s16 resi[16 * 4], s16 coef_tmp[16 * 4], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_4x16(U4 bit_depth,  s16 resi[4 * 16], s16 coef_tmp[4 * 16], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_16x8(U4 bit_depth,  s16 resi[16 * 8], s16 coef_tmp[16 * 8], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_8x16(U4 bit_depth,  s16 resi[8 * 16], s16 coef_tmp[8 * 16], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_16x16(U4 bit_depth,  s16 resi[16 * 16], s16 coef_tmp[16 * 16], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_32x4(U4 bit_depth,  s16 resi[32 * 4], s16 coef_tmp[32 * 4], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_4x32(U4 bit_depth,  s16 resi[4 * 32], s16 coef_tmp[4 * 32], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_8x32(U4 bit_depth,  s16 resi[8 * 32], s16 coef_tmp[8 * 32], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_32x8(U4 bit_depth,  s16 resi[32 * 8], s16 coef_tmp[32 * 8], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_32x16(U4 bit_depth,  s16 resi[32 * 16], s16 coef_tmp[32 * 16], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_16x32(U4 bit_depth,  s16 resi[16 * 32], s16 coef_tmp[16 * 32], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
u8 transform_hor_32x32(U4 bit_depth, s16 resi[32 * 32], s16 coef_tmp[32 * 32], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);

u8 transform_ver_4x4(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[4 * 4], s16 coef[4 * 4], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_4x8(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[4 * 8], s16 coef[4 * 8], s16 coef_y_pb_part[4 * 8], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_8x4(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[8 * 4], s16 coef[8 * 4], s16 coef_y_pb_part[8 * 4], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_8x8(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[8 * 8], s16 coef[8 * 8], s16 coef_y_pb_part[8 * 8], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_16x4(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[16 * 4], s16 coef[16 * 4], s16 coef_y_pb_part[16 * 4], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_4x16(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[4 * 16], s16 coef[4 * 16], s16 coef_y_pb_part[4 * 16], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_16x8(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[16 * 8], s16 coef[16 * 8], s16 coef_y_pb_part[16 * 8], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_8x16(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[8 * 16], s16 coef[8 * 16], s16 coef_y_pb_part[8 * 16], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
u8 transform_ver_16x16(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[16 * 16], s16 coef[16 * 16], s16 coef_y_pb_part[16 * 16], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_32x4(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[32 * 4], s16 coef[32 * 4], s16 coef_y_pb_part[32 * 4], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_4x32(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[4 * 32], s16 coef[4 * 32], s16 coef_y_pb_part[4 * 32], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_8x32(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[8 * 32], s16 coef[8 * 32], s16 coef_y_pb_part[8 * 32], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_32x8(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[32 * 8], s16 coef[32 * 8], s16 coef_y_pb_part[32 * 8], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_32x16(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[32 * 16], s16 coef[32 * 16], s16 coef_y_pb_part[32 * 16], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_16x32(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[16 * 32], s16 coef[16 * 32], s16 coef_y_pb_part[16 * 32], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
//u8 transform_ver_32x32(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[32 * 32], s16 coef[32 * 32], s16 coef_y_pb_part[32 * 32], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);

void inv_transform_ver_4x4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[4 * 4], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_4x8(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[4 * 8], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_8x4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[8 * 4], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_8x8(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[8 * 8], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_16x4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[16 * 4], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_4x16(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[4 * 16], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_16x8(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[16 * 8], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_8x16(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[8 * 16], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
void inv_transform_ver_16x16(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[16 * 16], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_32x4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[32 * 4], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_4x32(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[4 * 32], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_8x32(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[8 * 32], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_32x8(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[32 * 8], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_32x16(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[32 * 16], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_16x32(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[16 * 32], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
//void inv_transform_ver_32x32(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[32 * 32], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);

void inv_transform_hor_4x4(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[4 * 4], s32* dist, pel rec[4 * 4], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[4 * 4], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_4x8(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[4 * 8], s32* dist, pel rec[4 * 8], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[4 * 8], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_8x4(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[8 * 4], s32* dist, pel rec[8 * 4], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[8 * 4], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_8x8(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[8 * 8], s32* dist, pel rec[8 * 8], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[8 * 8], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_16x4(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[16 * 4], s32* dist, pel rec[16 * 4], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[16 * 4], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_4x16(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[4 * 16], s32* dist, pel rec[4 * 16], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[4 * 16], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_16x8(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[16 * 8], s32* dist, pel rec[16 * 8], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[16 * 8], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_8x16(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[8 * 16], s32* dist, pel rec[8 * 16], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[8 * 16], U3 type, U1 is_intra, pel* org);
void inv_transform_hor_16x16(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[16 * 16], s32* dist, pel rec[16 * 16], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[16 * 16], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_32x4(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[32 * 4], s32* dist, pel rec[32 * 4], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[32 * 4], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_4x32(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[4 * 32], s32* dist, pel rec[4 * 32], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[4 * 32], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_8x32(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[8 * 32], s32* dist, pel rec[8 * 32], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[8 * 32], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_32x8(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[32 * 8], s32* dist, pel rec[32 * 8], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[32 * 8], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_32x16(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[32 * 16], s32* dist, pel rec[32 * 16], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[32 * 16], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_16x32(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[16 * 32], s32* dist, pel rec[16 * 32], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[16 * 32], U3 type, U1 is_intra, pel* org);
//void inv_transform_hor_32x32(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE_BEST* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[32 * 32], s32* dist, pel rec[32 * 32], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[32 * 32], U3 type, U1 is_intra, pel* org);

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
);

void const_up_16x16(pel* up, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu);
void const_left_16x16(pel* left, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu);
void const_top_left_16x16(pel* up, pel* left, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16, int avail_cu);
void const_up_8x16(pel* up, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu);
void const_left_8x16(pel* left, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu);
void const_top_left_8x16(pel* up, pel* left, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, int avail_cu);
void const_up_16x8(pel* up, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_y, int x_scu, int pic_width_in_scu);
void const_left_16x8(pel* left, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8, int avail_cu, int pic_y_scu, int pic_x_scu, int pic_x, int y_scu, int pic_height_in_scu);
void const_top_left_16x8(pel* up, pel* left, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, int avail_cu);

void com_ipred_uv(pel* src_le, pel* src_up, pel dst[4 * 4], S8 ipm_c, S8 ipm, U7 w, U7 h, U4 bit_depth,
	u16 avail_cu, U2 chType, pel piRecoY[8][8], pel nb_y[2][2 * 8 + 3]
#if MIPF
	, int mipf_enable_flag
#endif
);

int enc_bit_est_intra_chroma(MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef[4096]/*[N_C - 1][MAX_CU_DIM_C]*/, u8 tscpm_enable_flag);
//int enc_bit_est_intra_chroma_16(int cu_width_log2, int cu_height_log2,   s16 coef[12][16][16], u8 tscpm_enable_flag, U4 rdo_idx, S8 ipm_c, S8 ipm, int num_nz[12]);
int enc_eco_xcoef_est_16(s16 coef[16][16], int log2_w, int log2_h, int num_nz);
s64 init_basic_leaf_16_8(U1 isNS_allow, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, s64 lambdaY, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, MD_COM_MODE_BEST* bst_info_ptr,
	U3 qt_depth, U3 bet_depth, U13 x0, U13 y0, U9 cup, U2 tree_status,
	U2* cons_pred_mode, MD_COM_MODE* mod_info_curr_temp_ptr, ENC_CU_DATA_ARRAY_INNER* cu_data_temp_ptr);

int mode_cu_init_16_8(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_COM_MODE* mod_info_curr_ptr, MD_COM_MODE_BEST* bst_info_ptr,
	/*COM_PART_INFO *mod_info_curr_pb_info, COM_PART_INFO *mod_info_curr_tb_info, COM_PART_INFO *bst_info_pb_info, COM_PART_INFO *bst_info_tb_info,*/
	U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U4 cud, U11 pic_width_in_scu, U2 tree_status,
	U2* cons_pred_mode, ENC_CU_DATA_ARRAY_INNER* cu_data_temp_ptr);

s64 md_chroma_8(U8 w, U8 h, MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][(((1 << (7)) >> 1) >> 2)][(((1 << (7)) >> 1) >> 2)],
	ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U2 tree_status, U4 cud, MD_INPUT* md_input_ptr, pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	U1 need_dochroma, U12 cup, U2 ctx_cons_pred_mode,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr,  strFetch_ref_window* Fetch_Ref_window_ptr,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_8x16, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_16x8, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<8, 16>* IP_map_scu_ptr_8x16, IP_MAP_SCU_SIZE<16, 8>* IP_map_scu_ptr_16x8, IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16
	);


//=======================================================================================================================================================
//=======================================================================================================================================================

s64 circuit_32x32_top(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	//ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO up_nebs_father[18], ENC_ME_NEB_INFO left_nebs_father[16], //作为顶层函数综合时需要指定数组大小，否则报错
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father);
s64 circuit_32x32_top_qt(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	//ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO up_nebs_father[18], ENC_ME_NEB_INFO left_nebs_father[16], //作为顶层函数综合时需要指定数组大小，否则报错
	SKIP_MERGE_INPUT* skip_in,
	ENC_CU_DATA* prev_cu_data_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct, U3 prev_up_log2_sub_cuw,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father,

	Instruction mem_load[PR_SIZE],
	Instruction mem_pu[PR_SIZE],
	Instruction mem_store[PR_SIZE]);

s64 circuit_32x32_top_chroma(
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
    pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_CU_DATA_ARRAY_CHROMA_SIZE<64>* prev_cu_data_temp_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num,
	COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father);

s64 md_32x32_32x16_16x32_loop_md1(U3 split_mode, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_MODEL* rdoq_model_c, RDOQ_MODEL* model_cnt_c,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH], pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag, ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
	U2 cons_pred_mode_BTH[1], U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	IP_MAP_SCU* MAP_SCU_IP_ptr, 
	pel fme2mdrefbuf[2560][8][2][16],
	strFetch_ref_window* Fetch_Ref_window_ptr);

//*****************for chroma**************************

void ipred_c_4(MD_FW* md_fw_ptr, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status,
	U1 uv, U13 pb_x, U13 pb_y, pel pred[4 * 4], MD_COM_MODE_BEST* mod_info_curr_ptr,
	s16 coef_tmp[16], pel orgU[16], pel nb_y[2][2 * 8 + 3],
	pel nb1[2 * 4 + 3], pel nb2[2 * 4 + 3], S8 rdo_idx,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	U4 bit_depth, pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output);


void predictChromaByMode4_intra(MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
	pel pred[4 * 4], S8 rdo_idx, s64* cost,
	U3 cu_width_log2, U3 cu_height_log2, U13 pb_x, U13 pb_y, s16 coef_tmp[12][4 * 4], pel orgU[2][4 * 4],
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	pel nb_y[2][8 * 2 + 3], pel nb[3][2][4 * 2 + 3], U1* uv, U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output);

void predictChromaByMode4_intra_prepare(MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
	int rdo_idx, s64* cost,
	U3 cu_width_log2, U3 cu_height_log2, U13 pb_x, U13 pb_y, s16 coef_tmp[12][4 * 4], pel orgU[2][4 * 4],
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	pel nb_y[2][8 * 2 + 3], pel nb[3][2][4 * 2 + 3], U1* uv, U4 bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output);

void ipred_c_prepare(MD_FW* md_fw_ptr, U3 cu_width_log2, U3 cu_height_log2, U2 tree_status,
	U1 uv, U13 pb_x, U13 pb_y, MD_COM_MODE_BEST* mod_info_curr_ptr,
	s16 coef_tmp[12][4 * 4], pel nb_y[2][2 * 8 + 3],
	pel nb[2][2 * 4 + 3], pel nb_v[2][2 * 4 + 3],
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_U_4x4,
	IP_ARRAY_SIZE<4, 4>* IP_buffer_ptr_V_4x4,
	U4 bit_depth, pel piRecoY[8][8],
	IP_ARRAY_SIZE<8, 8>* IP_buffer_ptr_8x8,
	IP_MAP_SCU_SIZE<8, 8>* IP_map_scu_ptr_8x8, u16* avail_cu_output);


void calcDiffChroma4(U2 cu_mode, U3 cu_width, U3 cu_height, s16 orgU[16], s16 pred[4 * 4], s16 coef_tmp[4 * 4]);

void quantChroma4(int* num_nz, RDOQ_ARRAY* rdoq_array, int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[16], int cu_width_log2, int cu_height_log2, 
int ch_type, int slice_type, s16 coef_y_pb_part[16], s16 coefVer_q[16]);

void dquantChroma4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_tmp[16], s16 coef_dq[4 * 4], U3 type, int num_nz_q, int num_nz1[4][3], int num_nz2[4][3]);

void invTransVerChroma4(U4 bit_depth, U6 qp_y, int num_nz[4][3], s16 coef_dq[4 * 4], s16 coef_tmp[4 * 4], U3 type);

void invTransHorChroma4(U4 bit_depth, int num_nz[4][3], s16 coef_temp[4 * 4], U3 type, pel resi[4 * 4]);

void reconChroma4(int x_pos, int y_pos, U13 tb_x, U13 tb_y, U4 bit_depth, int num_nz[4][3], int tb_part, pel pred_cache[4 * 4], pel rec[4 * 4], U3 type, pel resi[4 * 4],int num_nz_out[12], int num_nz_out1[12], S8 rdo_idx, U2 cu_mode);

void deriveRDCostChroma4(U2 cu_mode, U32 dist_chroma_weight, MD_COM_MODE_BEST* mod_info_best, U3 cu_width_log2,
	U3 cu_height_log2, int cu_width, int cu_height, s16 rec_tmp[4 * 4], s16 orgU[16], s64 lambda_y,
	s16 coef_tmp[12][16], u8 tscpm_enable_flag, s64* cost, U4 bit_depth, int uv, int num_nz[4][3],int num_nz_out[12], S8 rdo_idx);

void update_chroma_4(MD_COM_MODE_BEST* mod_info_curr_ptr, s16 coef_y_pb_part[12][16], U1 i, pel pred[4][4], pel rec[4][4], pel rec_1[4][4], int num_nz[4][3],
	U12 size_tmp1, S8 rdo_idx, s64 cost, s64* cost_best1, int* num_nz1, int* num_nz2, int* best_ipd1, U4 bit_depth, int num_nz_out[12],
	pel mod_info_best_rec_u[4][4],
	pel mod_info_best_rec_v[4][4],
	pel mod_info_best_coef_u[4][4],
	pel mod_info_best_coef_v[4][4]);

void md2_com_check_split_mode_top(U14 x0, U14 y0, U8 cu_width, U8 cu_height, U13 pic_width, U13 pic_height, U2 split_mode, U1* do_split);

#if !ONLY_QT_QT
void circuit_64x64_64x32_32x64_branch_top_chroma(U2 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	MD_RESULT* md2_best_result_ptr,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, 
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, 
	ENC_CU_DATA* md2_best_ctu_output_ptr,
	strFetch_ref_window* Fetch_Ref_window_ptr, ENC_CU_DATA* md_output_luma);
#endif

int RDOChroma32(RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, 
	U2 tree_status, MD_COM_MODE_BEST* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, pel orgU[2][32][32], 
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_U_32x32, IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_V_32x32,
	IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_U_16x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_V_16x32,
	IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_U_32x16, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_V_32x16,
	IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_U_8x32, IP_ARRAY_SIZE<8, 32>* IP_buffer_ptr_V_8x32,
	IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_U_32x8, IP_ARRAY_SIZE<32, 8>* IP_buffer_ptr_V_32x8,
	IP_ARRAY_SIZE<4, 32>* IP_buffer_ptr_U_4x32, IP_ARRAY_SIZE<4, 32>* IP_buffer_ptr_V_4x32,
	IP_ARRAY_SIZE<32, 4>* IP_buffer_ptr_U_32x4, IP_ARRAY_SIZE<32, 4>* IP_buffer_ptr_V_32x4,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2],
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U1 coef_rec_write_flag[1], strFetch_ref_window* Fetch_Ref_window_ptr,
	pel piRecoY[64][64], IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64,
	IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32,
	IP_ARRAY_SIZE<16, 64>* IP_buffer_ptr_16x64, IP_ARRAY_SIZE<64, 16>* IP_buffer_ptr_64x16,
	IP_ARRAY_SIZE<8, 64>* IP_buffer_ptr_8x64, IP_ARRAY_SIZE<64, 8>* IP_buffer_ptr_64x8,
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64,
	IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	IP_MAP_SCU_SIZE<16, 64>* IP_map_scu_ptr_16x64, IP_MAP_SCU_SIZE<64, 16>* IP_map_scu_ptr_64x16,
	IP_MAP_SCU_SIZE<8, 64>* IP_map_scu_ptr_8x64, IP_MAP_SCU_SIZE<64, 8>* IP_map_scu_ptr_64x8
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);

void copy_to_cu_data_chroma32(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);

#if !ONLY_QT_QT
int rdo_chroma_16(RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, U2 tree_status, MD_COM_MODE_BEST* mod_info_best,
	MD_INPUT* md_input_ptr, pel orgU[2][16][16], 
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_U_16x16, IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_V_16x16,
	IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_U_8x16, IP_ARRAY_SIZE<8, 16>* IP_buffer_ptr_V_8x16,
	IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_U_16x8, IP_ARRAY_SIZE<16, 8>* IP_buffer_ptr_V_16x8,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS],
#endif
	U1 coef_rec_write_flag[1], strFetch_ref_window* Fetch_Ref_window_ptr, 
	pel piRecoY[32][32], IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, IP_ARRAY_SIZE<16, 32>* IP_buffer_ptr_16x32, IP_ARRAY_SIZE<32, 16>* IP_buffer_ptr_32x16,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32, IP_MAP_SCU_SIZE<16, 32>* IP_map_scu_ptr_16x32, IP_MAP_SCU_SIZE<32, 16>* IP_map_scu_ptr_32x16
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
    );
#endif //!ONLY_QT_QT

void copy_to_cu_data_chroma16(U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1]/*, int coef_nz_y*/);

void copy_to_cu_data_chroma8(U6 qp_y, ENC_CU_DATA_ARRAY_INNER* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status, U1 coef_rec_write_flag[1]);
void update_to_ctx_map_top(MD_FW *md_fw_ptr, MD_INPUT *md_input_ptr, ENC_ME_LINE_MAP *me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	MPM_ARRAY *MPM_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, ENC_CU_DATA *md1_best_ctu_output_ptr);

void store_cu_data_pixel_top_chroma(pel*reco_u, pel*reco_v, U13 x, U13 y, U7 w, U7 h, U13 pic_width,
	U13 pic_height, U2 tree_status, IP_ARRAY *IP_buffer_ptr);

void Copy_LCU_To_Top_linebuffer(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, pel reco_y[CU_SIZE >> 2][CU_SIZE >> 2], pel dbk_y[71][71]);

void store_enc_info_top_chroma(MD_FW *md_fw_ptr, MD_INPUT *md_input_ptr, ENC_ME_LINE_MAP *me_line_map_ptr,
	U12 cup,
	U4 cud, U2 cons_pred_mode, U7 cu_width, U7 cu_height, U2 tree_status, MD_RESULT md_best_result[2],
	ENC_CU_DATA md_best_ctu_output[2], IP_ARRAY *IP_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr,
	ENC_CU_DATA *md2_output, ENC_CU_DATA *ctu_map_cu_data_DBK, pel dbk_u[39][39], pel dbk_v[39][39], ENC_ME_LINE_MAP* me_line_map_ptr_tmp,
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_LINE_MAP* me_line_map_ptr_input,
	U32 cur_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], pel p_fenc_LCU_U[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	pel p_fenc_LCU_V[MAX_CU_SIZE_FETCH / 2][MAX_CU_SIZE_FETCH / 2],
	ENC_ME_LINE_MAP* me_line_map_ptr_input_luma, ENC_CU_DATA *src_luma, U32 cur_ctu_scu_luma[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN]);

void write_back_inner_nebs(ENC_ME_LINE_MAP *me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN]);

void write_back_inner_line_map(ENC_ME_LINE_MAP *dst, ENC_ME_LINE_MAP *src, U32 cur_ctu_scu[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], U32 cur_ctu_cu_mode[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN]);

void copy_to_cu_data_luma_64(U1 isNS_allow, U7 w, U7 h, U6 qp_y, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mod_info_best, U2 tree_status,
	int slice_num, int affine_subblock_size_idx, U2 NS_luma_pred_mode[1]
#if SUB_TMVP
	, CORE_SBTMVP* core_sbtmvp
#endif
);

int copy_cu_data(ENC_CU_DATA *dst, ENC_CU_DATA *src, U13 x, U13 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status,
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH]);

void memcpy_1d_2_1d_64(s16 des[64 * 64], s16 src[64 * 64], int n, int flag);
void memcpy_2d_2_1d_64(s16 des[64 * 64], s16 src[64][64], int w, int h);

void ipred_tscpm_4(pel piPred[4 * 4], pel piRecoY[8][8], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept, pel src_left[11], pel src_up[11],
	pel src_left_luma[19], pel src_up_luma[19]);

void ipred_tscpm_Chroma8(pel piPred[8][8], pel piPred1[8][8], pel piRecoY[16][16], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept,
	pel src_left[19], pel src_up[19], pel src_left_luma[35], pel src_up_luma[35]);

void ipred_tscpm_Chroma16(pel piPred[16][16], pel pred[16][16], pel piRecoY[32][32], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept, pel src_left[35], pel src_up[35],
	pel src_left_luma[67], pel src_up_luma[67]);

void ipred_tscpm_Chroma32(pel piPred[32][32], pel piPred0[32][32], pel piRecoY[64][64], int uiStrideY, int uiWidth, int uiHeight, int bAbove, int bLeft, int bitDept, pel src_left[67], pel src_up[67],
	pel src_left_luma[131], pel src_up_luma[131]);

//*****************************************************

s64 circuit_16x16_top(MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU[SWW * SWH], pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir, 
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH], pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
	U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, 
	pel fme2mdrefbuf[2560][8][2][16],
	strFetch_ref_window* Fetch_Ref_window_ptr);

s64 md_16x16_loop(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_MODEL* rdoq_model_c, RDOQ_MODEL* model_cnt_c,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH], pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag, ENC_CU_DATA_ARRAY* cu_data_temp_ptr,
	U2 cons_pred_mode_QU[1], U2 child_tree_status[1], ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<32, 32>* IP_buffer_ptr_32x32, IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr,
	IP_MAP_SCU_SIZE<32, 32>* IP_map_scu_ptr_32x32,
	IP_MAP_SCU* MAP_SCU_IP_ptr,
	pel fme2mdrefbuf[2560][8][2][16],
	strFetch_ref_window* Fetch_Ref_window_ptr);

#if !ONLY_QT_QT
s64 circuit_64x16_16x64( 
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<64>* me_line_map_ptr_64,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<64, 64>* IP_buffer_ptr_64x64, IP_ARRAY_SIZE<32, 64>* IP_buffer_ptr_32x64, IP_ARRAY_SIZE<64, 32>* IP_buffer_ptr_64x32, 
	MPM_ARRAY_SIZE<64, 64>* MPM_buffer_ptr_64x64, MPM_ARRAY_SIZE<32, 64>* MPM_buffer_ptr_32x64, MPM_ARRAY_SIZE<64, 32>* MPM_buffer_ptr_64x32, 
	IP_MAP_SCU_SIZE<64, 64>* IP_map_scu_ptr_64x64, IP_MAP_SCU_SIZE<32, 64>* IP_map_scu_ptr_32x64, IP_MAP_SCU_SIZE<64, 32>* IP_map_scu_ptr_64x32,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr, int split_mode_father, int ns_64x64);
#endif

//void cost_compare_fixed_point_one(MD_FW*md_fw_ptr, s64 cost_best[1], s64 cost_temp, ENC_CU_DATA_ARRAY *curr_cu_data_temp_array_ptr, ENC_CU_DATA_ARRAY *curr_cu_data_best_ptr, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3 best_split_mode[1], U2 best_cons_pred_mode[1], U2 cons_pred_mode_child, U2 tree_status_temp[1]);
void cost_compare_fixed_point_top_new(s64 cost_temp, ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, MD_RESULT* md1_best_result_ptr, U2 cons_pred_mode_child, ENC_CU_DATA* md1_best_ctu_output_ptr);

void cost_compare_fixed_point_top_new_chroma(s64 cost_temp, ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, MD_RESULT* md1_best_result_ptr, U2 cons_pred_mode_child, ENC_CU_DATA* md1_best_ctu_output_ptr);

void cost_compare_fixed_point(MD_FW* md_fw_ptr, s64 cost_best[1], s64 cost_temp, ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3 split_mode, U3 best_split_mode[1],
	U2 best_cons_pred_mode[1], U2 cons_pred_mode_child, U2 tree_status_child, U2 best_child_status[1], U2 tree_status_temp[1]);
void cost_compare_fixed_point_one_time(MD_FW* md_fw_ptr, s64* cost_best, s64* cost, ENC_CU_DATA_ARRAY* curr_cu_data_temp_array_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr,
	U3 cu_width_log2, U3 cu_height_log2, U4 cud, U2 tree_status, U3* split_mode, U3* best_split_mode,
	U2* best_cons_pred_mode, U2* cons_pred_mode_child, U2* tree_status_child, U2* best_child_status, U2* tree_status_temp, U3 split_mode_num);
int copy_cu_data_array_src(ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY* src, U14 x, U14 y, U3 cu_width_log2,
	U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status);
void store_enc_info_best(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw);

void update_map_scu(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best_ptr, U13 x, U13 y, U8 src_cuw, U8 src_cuh,
	U2 tree_status);
void update_map_scu_prev(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int idx_dst, int x, int y, int src_cuw, int src_cuh, u8 tree_status);
int copy_cu_data_array_dst(ENC_CU_DATA_ARRAY* dst, ENC_CU_DATA_ARRAY* src, U14 x, U14 y, U3 cu_width_log2, U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status);
//void com_set_cons_pred_mode_prev(u8 cons_pred_mode, U4 cud, int cup, int cu_width, int cu_height, int lcu_s, s8(*split_mode_buf)[NUM_BLOCK_SHAPE][(1 << 7 >> 1) * (1 << 7 >> 1) / ((1 << 2) * (1 << 2))], int idx_dst);
u8 com_tree_split(int w, int h, s8 split, u8 slice_type);

int com_split_is_EQT(U3 mode);
void update_skip_candidates(COM_MOTION motion_cands[ALLOWED_HMVP_NUM], s8* num_cands, const int max_hmvp_num, s16 mv_new[REFP_NUM][MV_D], s8 refi_new[REFP_NUM]);
int copy_cu_data_array_src_top(ENC_CU_DATA* dst, ENC_CU_DATA_ARRAY* src, U14 x, U14 y, U3 cu_width_log2, U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status);
int copy_cu_data_array_src_chroma(ENC_CU_DATA* dst, ENC_CU_DATA_ARRAY* src, U14 x, U14 y, U3 cu_width_log2, U3 cu_height_log2, U3 log2_cus, U4 cud, U2 tree_status);

//#define DECL_RDO_LUMA(W, H, ME_SIZE) \
//	s64 FUNC_NAME(rdo_luma, W, H)(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], SKIP_MERGE_INPUT* skip_in, u8 tree_status, MD_COM_MODE* best_info_ptr, MD_INPUT* md_input_ptr, strFetch_Orig_Lcu* Orig_Lcu_ptr, strFetch_ref_window* Fetch_Refwin_ptr, u8 best_tb_part_hist[1], MD_COM_MODE* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, COM_PART_INFO* mod_info_curr_tb_info, COM_PART_INFO* bst_info_pb_info, COM_PART_INFO* bst_info_tb_info, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, u8 curr_cons_pred_mode, int cup, U3 qt_depth, ENC_FME_INFO_ALL *fme_mv_info,  s8 coef_rec_write_flag[1], s64 cost_inter[1], RMD_OUTPUT *g_rmd_output, IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int num_nz_inter[4][3], int best_ipd[4], s64 best_dist_y[1], int best_bin_cnt_luma[1], s64 cost_best_intra[1], int num_nz_y_pb_part[1]);
//DECL_FUNC(DECL_RDO_LUMA)
int rdo_chroma(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, u8 tree_status, MD_COM_MODE* mod_info_inter_ptr, MD_COM_MODE* mod_info_intra_ptr,
	COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, pel* orgU, int best_ipd[4], IP_ARRAY* IP_buffer_ptr,
	MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, strFetch_ref_window* Fetch_Ref_window_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr, s8 coef_rec_write_flag[1]);

int get_part_num(PART_SIZE size);
U2  get_part_idx(PART_SIZE part_size, U7 x, U7 y, U7 w, U7 h);

s64 circuit_64x16_top(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output, strFetch_ref_window* Fetch_Ref_window_ptr, strFetch_Orig_Lcu* Fetch_Orig_Lcu_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
	U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr);

void com_get_mpm(int x_scu, int y_scu, u8 mpm[2], MPM_ARRAY* MPM_buffer_ptr, ENC_ME_LINE_MAP* me_line_ptr);
u16 com_get_avail_intra_ip(IP_MAP_SCU* MAP_SCU_IP_ptr, U11 x_scu, U11 y_scu);

extern void md1_run(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr,
	pel RefWindowLCU[SWW * SWH], pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH], pel p_fenc_LCU_UV[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH / 2],
	ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], SKIP_MERGE_INPUT* skip_in,
	MD_RESULT* md1_best_result_ptr, RMD_OUTPUT* g_rmd_output,
	MD_INPUT* md_input_ptr, ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, ENC_CU_DATA* md1_best_ctu_data_ptr, 
	pel fme2mdrefbuf[2560][8][2][16]
);


void update_luma_intra(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_CU_DATA_ARRAY* cu_data_temp_ptr, MD_COM_MODE* mod_info_best, COM_PART_INFO* mod_info_curr_pb_info, MD_INPUT* md_input_ptr, IP_ARRAY* IP_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr, int tb_w, int tb_h,
	u8 cons_pred_mode, s32* dist_t,
	int best_bin_cnt_luma[1], s64* cost_pb_best, int ipred_mode, int best_ipd_pb_part[4], s32 best_dist_y_pb_part[4],
	pel rec_y_pb_part[(1 << 7 >> 1) * (1 << 7 >> 1)], s16 coef_y_pb_part[(1 << 7 >> 1) * (1 << 7 >> 1)],
	int num_nz_y_pb_part[1], s64* cost_best, int best_ipd[1], s64 best_dist_y[1], int pix_x, int pix_y, int i, int pred_cnt, int is_intra);
//void update_intra_info_map_scu(ENC_ME_LINE_MAP *me_line_map_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr, int tb_x, int tb_y, int tb_w, int tb_h,
//	int pic_width_in_scu, int ipm);

void copy_md_com_mode_info(MD_COM_MODE* src, MD_COM_MODE* dst);
void copy_md_com_part_info(COM_PART_INFO* src, COM_PART_INFO* dst);

void enc_diff_16b(U3 log2w, U3 log2h, s16* src1, s16 src2[4 * 4], U7 s_src1, U7 s_src2, U7 s_diff, s16* diff);

void rdoq_data_prepare_64x64(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);
void rdoq_data_prepare_64x32(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);
void rdoq_data_prepare_64x16(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);
void rdoq_data_prepare_64x8(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);
void rdoq_data_prepare_8x64(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);
void rdoq_data_prepare_16x64(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);
void rdoq_data_prepare_32x64(int ch_type, s64 lambda, int cnt, s16 src_tmp[64], s16 src_coef[64], int is_intra);

void read_data1_64x64(s16 coef64[64], s16 src_coef[64][64], int i);
void read_data1_64x32(s16 coef64[64], s16 src_coef[64][64], int i);
void read_data1_64x16(s16 coef64[64], s16 src_coef[64][64], int i);
void read_data1_64x8(s16 coef64[64], s16 src_coef[64][64], int i);
void read_data1_8x64(s16 coef64[64], s16 src_coef[64][64], int i);
void read_data1_16x64(s16 coef64[64], s16 src_coef[64][64], int i);
void read_data1_32x64(s16 coef64[64], s16 src_coef[64][64], int i);

void write_data1_64x64(s16 dst_tmp[64][64], s16 dst64[64], int i);
void write_data1_64x32(s16 dst_tmp[64][64], s16 dst64[64], int i);
void write_data1_64x16(s16 dst_tmp[64][64], s16 dst64[64], int i);
void write_data1_64x8(s16 dst_tmp[64][64], s16 dst64[64], int i);
void write_data1_8x64(s16 dst_tmp[64][64], s16 dst64[64], int i);
void write_data1_16x64(s16 dst_tmp[64][64], s16 dst64[64], int i);
void write_data1_32x64(s16 dst_tmp[64][64], s16 dst64[64], int i);

#define TR_H(w, h) u8 FUNC_NAME(transform_hor, w, h)(U4 bit_depth, int num_nz[4][3], s16 resi[w * h], s16 coef_tmp[w * h], U7 tb_w, U7 tb_h, U2 slice_type, U3 type, U3 tree_status);
#define TR_V(w, h) u8 FUNC_NAME(transform_ver, w, h)(U2 slice_type, U4 bit_depth, U6 qp_y, int num_nz[4][3], s64 lambda[3], s16 coef_tmp[w * h], s16 coef[w * h], s16 coef_y_pb_part[w * h], U7 tb_w, U7 tb_h, U3 type, U1 is_intra, U3 tree_status);
#define QUANT(w, h) int FUNC_NAME(quant_nnz, w, h)(RDOQ_MODEL* model_cnt, int rdoq_est_cbf[NUM_QT_CBF_CTX][2],s32 rdoq_est_level[NUM_SBAC_CTX_LEVEL][2],s32 rdoq_est_last[2][NUM_SBAC_CTX_LAST1][NUM_SBAC_CTX_LAST2][2],s32 rdoq_est_run[24][2], int bit_depth, int qp, s64 lambda, int is_intra, s16 coef[w * h], int cu_width_log2, int cu_height_log2, int ch_type, int slice_type, s16 coef_y_pb_part[w * h], s16 coef_u[w * h], s16 coef_v[w * h]);

#define INV_TR_V(w, h) void FUNC_NAME(inv_transform_ver, w, h)(U4 bit_depth, U6 qp_y, MD_COM_MODE* mod_info_curr_ptr, s16 coef_tmp[w * h], U3 cu_width_log2, U3 cu_height_log2, U7 tb_w, U7 tb_h, U3 type);
#define INV_TR_H(w, h) void FUNC_NAME(inv_transform_hor, w, h)(MD_FW* md_fw_ptr, U4 bit_depth, MD_COM_MODE* mod_info_curr_ptr, COM_PART_INFO* mod_info_curr_pb_info, U3 cu_width_log2, U3 cu_height_log2, pel pred_cache[w * h], s32* dist, pel rec[w * h], U13 tb_x, U13 tb_y, U7 tb_w, U7 tb_h, int coef_offset_tb, s16 coef_temp[w * h], U3 type, U1 is_intra, pel* org);

//QUANT(4, 4)
//QUANT(4, 8)
//QUANT(8, 4)
//QUANT(8, 8)
//QUANT(16, 4)
//QUANT(4, 16)
//QUANT(16, 8)
//QUANT(8, 16)
//QUANT(16, 16)
//QUANT(32, 4)
//QUANT(4, 32)
//QUANT(8, 32)
//QUANT(32, 8)
//QUANT(32, 16)
//QUANT(16, 32)
//QUANT(32, 32)
//QUANT(64, 8)
//QUANT(8, 64)
//QUANT(64, 16)
//QUANT(16, 64)
//QUANT(32, 64)
//QUANT(64, 32)
//QUANT(64, 64)

//TR_H(4, 4)
//TR_H(4, 8)
//TR_H(8, 4)
//TR_H(8, 8)
//TR_H(16, 4)
//TR_H(4, 16)
//TR_H(16, 8)
//TR_H(8, 16)
//TR_H(16, 16)
//TR_H(32, 4)
//TR_H(4, 32)
//TR_H(8, 32)
//TR_H(32, 8)
//TR_H(32, 16)
//TR_H(16, 32)
//TR_H(32, 32)
//TR_H(64, 8)
//TR_H(8, 64)
//TR_H(64, 16)
//TR_H(16, 64)
//TR_H(32, 64)
//TR_H(64, 32)
//TR_H(64, 64)

//TR_V(4, 4)
//TR_V(4, 8)
//TR_V(8, 4)
//TR_V(8, 8)
//TR_V(16, 4)
//TR_V(4, 16)
//TR_V(16, 8)
//TR_V(8, 16)
//TR_V(16, 16)
//TR_V(32, 4)
//TR_V(4, 32)
//TR_V(8, 32)
//TR_V(32, 8)
//TR_V(32, 16)
//TR_V(16, 32)
//TR_V(32, 32)
//TR_V(64, 8)
//TR_V(8, 64)
//TR_V(64, 16)
//TR_V(16, 64)
//TR_V(32, 64)
//TR_V(64, 32)
//TR_V(64, 64)

//INV_TR_V(4, 4)
//INV_TR_V(4, 8)
//INV_TR_V(8, 4)
//INV_TR_V(8, 8)
//INV_TR_V(16, 4)
//INV_TR_V(4, 16)
//INV_TR_V(16, 8)
//INV_TR_V(8, 16)
//INV_TR_V(16, 16)
//INV_TR_V(32, 4)
//INV_TR_V(4, 32)
//INV_TR_V(8, 32)
//INV_TR_V(32, 8)
//INV_TR_V(32, 16)
//INV_TR_V(16, 32)
//INV_TR_V(32, 32)
//INV_TR_V(64, 8)
//INV_TR_V(8, 64)
//INV_TR_V(64, 16)
//INV_TR_V(16, 64)
//INV_TR_V(32, 64)
//INV_TR_V(64, 32)
//INV_TR_V(64, 64)

//INV_TR_H(4, 4)
//INV_TR_H(4, 8)
//INV_TR_H(8, 4)
//INV_TR_H(8, 8)
//INV_TR_H(16, 4)
//INV_TR_H(4, 16)
//INV_TR_H(16, 8)
//INV_TR_H(8, 16)
//INV_TR_H(16, 16)
//INV_TR_H(32, 4)
//INV_TR_H(4, 32)
//INV_TR_H(8, 32)
//INV_TR_H(32, 8)
//INV_TR_H(32, 16)
//INV_TR_H(16, 32)
//INV_TR_H(32, 32)
//INV_TR_H(64, 8)
//INV_TR_H(8, 64)
//INV_TR_H(64, 16)
//INV_TR_H(16, 64)
//INV_TR_H(32, 64)
//INV_TR_H(64, 32)
//INV_TR_H(64, 64)



S4 get_transform_shift(const U4 bit_depth, const U4 tr_size_log2);

U4 com_get_forward_trans_shift(U3 log2_size, U3 type, U4 bit_depth);

U4 com_get_inverse_trans_shift(U3 log2_size, U3 type, U4 bit_depth);

void get_tb_start_pos(int w, int h, PART_SIZE part, int idx, int* pos_x, int* pos_y);

s64 enc_ssd_16b_1(U3 w_log2, U3 h_log2, s16* src1, s16* src2, U8 s_src1, U7 s_src2, U4 bit_depth);
s64 enc_ssd_16b_1_64(U3 w_log2, U3 h_log2, s16 src1[64][64], s16* src2, int s_src1, int s_src2, U4 bit_depth);
void memcpy_rewrintra(s16* des, s16* src, int n);

//*******************md_basic_kernel***************************************************************************
//#define DECL_MD_KERNEL(W, H, ME_SIZE) \
//	s64 FUNC_NAME(md_kernel, W, H)(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW*md_fw_ptr, MD_INPUT *md_input_ptr, RMD_OUTPUT *g_rmd_output, strFetch_ref_window *Fetch_Ref_window_ptr, strFetch_Orig_Lcu *Fetch_Orig_Lcu_ptr, ENC_ME_LINE_MAP *me_line_map_ptr, \
//		ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], \
//		SKIP_MERGE_INPUT *skip_in, U1 boundary, U1 isNS_allow, U2 NS_luma_pred_mode[1], U13 x0, \
//		U13 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode, \
//		U2 tree_status, ENC_CU_DATA_ARRAY *cu_data_temp_ptr,  \
//		ENC_FME_INFO_ALL *fme_mv_info, \
//		IP_ARRAY *IP_buffer_ptr, MPM_ARRAY *MPM_buffer_ptr, IP_MAP_SCU *MAP_SCU_IP_ptr);
//DECL_FUNC(DECL_MD_KERNEL)

 //*********************intra_prediction****************************************************

void ipred_vert_4(pel src_up[11], pel dst[4 * 4], int w, int h);
void ipred_hor_4(pel src_le[11], pel dst[4 * 4], int w, int h);
void ipred_dc_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], int w, int h, int bit_depth, u16 avail_cu);
void ipred_plane_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], U4 bit_depth, U7 w, U7 h);
void ipred_bi_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], U4 bit_depth, int w, int h);
void ipred_ang_4(pel src_le[11], pel src_up[11], pel dst[4 * 4], U4 bit_depth, U7 w, U7 h, S8 ipm, U2 type
#if MIPF
	, int is_luma, int mipf_enable_flag
#endif
);
void ipred_16x16(MD_COM_MODE* mod_info_curr_ptr, pel* orgY, pel pred_cache[4096], s16 coef_tmp[4096], s8 intra_mode, pel src_le_temp[128 + 3], pel src_up_temp[128 + 3], u16 avail_tb, int cu_width_log2, int cu_height_log2);

void ipred_vert_16x16(pel* src_up, int w, int h, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_hor_16x16(pel* src_le, int w, int h, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_dc_16x16(pel* src_le, pel* src_up, int w, int h, int bit_depth, u16 avail_cu, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_plane_16x16(pel* src_le, pel* src_up, int w, int h, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_bi_16x16(pel* src_le, pel* src_up, int w, int h, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_ang_16x16_3_11(pel* src_le, pel* src_up, int w, int h, int ipm, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_ang_16x16_25_32(pel* src_le, pel* src_up, int w, int h, int ipm, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);
void ipred_ang_16x16_13_23(pel* src_le, pel* src_up, int w, int h, int ipm, pel orgY_16x16[64][64], pel pred_cache_16x16[64][64], s16 coef_tmp_16x16[64][64]);

//********************inter_prediction****************************************************
//#define DECL_PREPARE_LUMA_INTER(W, H, ME_SIZE) \
//void FUNC_NAME(prepare_luma_inter, W, H)(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_map_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], \
//	 SKIP_MERGE_INPUT* in, MD_COM_MODE* mod_info_curr, \
//	 SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge_rdo, S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], MD_INPUT* md_input_ptr, \
//	 int num_amvr, u32 lambda_mv, s8 refi_cur, s8 num_refp_cnt, int ptr, int refp_ptr[17][2], s16 mvp_all[2][5][2], s16 mv_all[2][5][2], \
//	 u32 mecost_all[2][5], U1 valid_mode[17], int amvr_enable_flag,  \
//	 ENC_FME_INFO_ALL *fme_mv_info, strFetch_ref_window *refw, strFetch_Orig_Lcu *org)
//
// DECL_FUNC(DECL_PREPARE_LUMA_INTER)
void derive_neb_cu(const U10 pic_width_in_scu, const U10 x_scu, const U10 y_scu, const U7 cu_width, const U7 cu_height, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], const U32 map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], const U32 map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[MAX_CU_IN_SCU + 2], U32 map_scu_above_line[MAX_CU_IN_SCU + 2], U32 map_cu_mode_above_line[MAX_CU_IN_SCU + 2], const U1 curr_ctu_idx);
void derive_inter_cands(SKIP_MERGE_INPUT* input, U4* num_cands_woUMVE, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT mode[SKIP_MODE_NUM], const U1 run_direct_skip, strFetch_ref_window* Fetch_Ref_window_ptr
#if SUB_TMVP
	, int cu_width, int cu_height, CORE_SBTMVP* core_sbtmvp
#endif
); 

void output_skip_mode(SKIP_RDO* skip_rdo, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], SKIP_ELEMENT* mode, S7 mode_list[MAX_INTER_SKIP_RDO]);
void derive_mvp_line_all(MD_FW* md_fw_ptr, ENC_ME_LINE_MAP* me_line_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN], MD_COM_MODE* mod_info_curr, int ptr, int ref_list,
	int ref_idx, int refp_ptr[17][2], int mvr_num, s16 mvp[5][2]);

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
);
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
);
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
);

int get_bitcnt_est_32(RDOQ_MODEL* model_cnt, int f_scu, u8 curr_mvr, u8 tree_status, s16 coef[32 * 32], int ch_type, s32 slice_type, u8 cons_pred_mode, int cu_mode, u8 umve_flag, u8 umve_idx, u8 affine_flag, unsigned char affine_enable_flag, u8 skip_idx, S3 refi[REFP_NUM], u8 mvp_from_hmvp_flag, u8 smvd_flag, u8 num_refp, s16  affine_mvd[REFP_NUM][VER_NUM][MV_D], s16  mvd[REFP_NUM][MV_D], int cu_width_log2, int cu_height_log2, int tb_part, int pb_part, int num_nz[MAX_NUM_TB][N_C], S8   ipm[MAX_NUM_PB][2], unsigned char slice_type_md, int cur_ptr, int refp_ptr[17][2], u8 mpm[4][2], int is_intra);

int choose_inter_rdo_mode(int max_rdo_num, int all_mode_num, s64 dist_all[17], U1 mode_vaild[17]);

void get_affme_in(MD_FW* md_fw_ptr, ENC_AFFME_IN* in, MD_COM_MODE* mod_info_curr, U1 lidx,
	MD_INPUT* md_input_ptr, u8 curr_mvr, u32 lambda_mv);

void affine_me_all_refi_fetch(MD_FW* md_fw_ptr, pel RefWindowLCU[SWW * SWH],
#if ENABLE_BFRAME
	pel RefWindowLCU_ref1[SWW * SWH], MV_DIR mv_dir,
#endif
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], ENC_ME_LINE_MAP* me_line_map_ptr,
	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
	ENC_AFFME_IN* in, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH * MAX_CU_SIZE_FETCH], MD_COM_MODE* mod_info_curr, s8* refi_temp, int lidx,
	int best_mv_uni_inner[2][4][2]);

extern CPMV com_get_affine_memory_access(CPMV mv[4][2], U7 cu_width, U7 cu_height);
void derive_tmvp_mode(const SKIP_MERGE_INPUT* input, const U10 x_scu, const U10 y_scu, const U10 pic_width_in_scu, const U10 pic_height_in_scu, SKIP_ELEMENT* mode, NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV]);
void skip_element_copy(SKIP_ELEMENT* dst, const SKIP_ELEMENT* src);
void derive_spatial_mode(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], SKIP_ELEMENT* mode);
void derive_umve_mode(const NEB_CU neb[NUM_SKIP_SPATIAL_MV], SKIP_MERGE_MAP_SCU neb_map[NUM_SKIP_SPATIAL_MV], SKIP_ELEMENT mode[SKIP_UMVE_NUM], SKIP_ELEMENT* tmvp, const SKIP_MERGE_INPUT* input);
void check_refi(s32 refi[REFP_NUM], strFetch_ref_window* Fetch_Ref_window_ptr);

extern void com_mv_rounding_s32(s32 hor, int ver, CPMV* rounded_hor, CPMV* rounded_ver, s32 right_shift, int left_shift);

void mv_clip(U16 x, U16 y, U13 pic_w, U13 pic_h, U10 w, U10 h, S3 refi[2], s16 mv[2][2], s16(*mv_t)[2]);
void mv_clip_1(U16 x, U16 y, U13 pic_w, U13 pic_h, U10 w, U10 h, S3 refi[2], s16 mv[2][2], s16(*mv_t)[2]);

int pinter_fme_all_cu_pred(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D]);
int pinter_fme_all_cu_pred_8_16(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D]);
int pinter_fme_all_cu_pred_16_8(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D]);
int pinter_fme_all_cu_pred_16_16(int x_pos, int y_pos, U12 x_chroma, U12 y_chroma, BOOL chroma_motion, s8 refi_cur,
	u8 num_refp_cnt, int curr_mvr, int lidx, int cu_width, int cu_height, s16 mvp_all[REFP_NUM][MAX_NUM_MVR][MV_D],
	s16 mv_all[REFP_NUM][MAX_NUM_MVR][MV_D], int me_level, int best_mv_uni_inner[REFP_NUM][MAX_NUM_ACTIVE_REF_FRAME][MV_D],
	S14 sub_pel_mv[MV_D]);
void fme_set_mv_refi_8_16(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx);
void fme_set_mv_refi_16_8(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx);
void fme_set_mv_refi_16_16(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx);
void fme_set_mv_refi(S14* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_KERNEL_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx);
void fme_set_mv_refi(S14* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_COM_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx);
void fme_set_mv_refi_64(s16* mv, s16* mvd, s16 mv_all[2][5][2], s16 mvp_all[2][5][2],
	MD_COM_MODE* mod_info_curr, int best_refi, u8 curr_mvr, int lidx, S3 refi_for_ip[2]);
BOOL derive_mode_info(int idx, SKIP_RDO* skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge, MD_COM_MODE* mod_info_curr);
BOOL derive_mode_info(int idx, SKIP_RDO* skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge, MD_KERNEL_MODE* mod_info_curr);
BOOL derive_mode_info_64(int idx, SKIP_RDO* skip, SKIP_ELEMENT skip_rdo_mode[MAX_INTER_SKIP_RDO], MERGE_RDO* merge, MD_COM_MODE* mod_info_curr,
	S3 refi_for_ip[2], S14 mv_for_ip[2][2],U8* umve_flag, U8* skip_idx_in);
BOOL com_mc_cu_uv4(MD_COM_MODE_BEST* mod_info_curr, int pic_w, int pic_h, S3 refi[REFP_NUM], s16 mv[REFP_NUM][2],
	pel pred_u[4][4], pel pred_v[4][4], int bit_depth, strFetch_ref_window* Fetch_Ref_window_ptr);
s64 init_basic_leaf(U1 isNS_allow, RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, RDOQ_ARRAY* rdoq_array, MD_FW* md_fw_ptr, s64 lambdaY, U3 cu_width_log2, U3 cu_height_log2, U4 cud, MD_COM_MODE_BEST* bst_info_ptr,
	U3 qt_depth, U3 bet_depth, U13 x0, U13 y0, U9 cup, U2 tree_status,
	U2* cons_pred_mode, MD_COM_MODE* mod_info_curr_temp_ptr,
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr);

//************************rate_estimation***********************************************
void enc_set_affine_mvf(int affine_subblock_size_idx, ENC_CU_DATA_ARRAY* cu_data, MD_COM_MODE_BEST* mi);


int is_cu_nz(int nz[4][3]);



//***************************rdoq********************************************************

extern U24 get_coded_level_rl(s64* rd64_uncoded_cost, s64* rd64_coded_cost, U36 level_double, u32 max_abs_level,
	U5 run, U5 ctx_run, U5 ctx_level, s32 q_bits, U30 err_scale, s64 lambda, int last_pos, s32 rdoq_est_run_local[24][2],
	s32 rdoq_est_level_local[24][2]);

//s64 circuit_32x8_8x32(RDOQ_MODEL* rdoq_model, RDOQ_MODEL* model_cnt, MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output, strFetch_ref_window* Fetch_Ref_window_ptr, strFetch_Orig_Lcu* Fetch_Orig_Lcu_ptr, ENC_ME_LINE_MAP* me_line_map_ptr,
//	ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],ENC_ME_NEB_INFO above_nebs_line[ABOVE_LINE_LEN],
//	SKIP_MERGE_INPUT* skip_in, ENC_CU_DATA_ARRAY* prev_up_cu_data_temp_array_ptr,
//	U3 qt_depth,
//	U3 bet_depth, U2 cons_pred_mode, U2 tree_status, U3 part_num, COM_SPLIT_STRUCT split_struct,
//	U3 prev_up_log2_sub_cuw, U3 prev_up_log2_sub_cuh,
//	ENC_FME_INFO_ALL* fme_mv_info,
//	IP_ARRAY* IP_buffer_ptr, MPM_ARRAY* MPM_buffer_ptr, IP_MAP_SCU* MAP_SCU_IP_ptr);

s64 md2_prepare_cu_info_COM_combine_top(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U7 cu_width, U7 cu_height, U3 split_mode, COM_SPLIT_STRUCT* split_struct);
s64 prepare_cu_info_COM_combine_only_bt(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct);
s64 prepare_cu_info_COM_combine(MD_FW* md_fw_ptr, s64 lambdaY,
	ENC_CU_DATA_ARRAY* cu_data_temp_ptr, U13 x0, U13 y0, U12 cup, U4 cud, U3 qt_depth,
	U3 bet_depth, U2 cons_pred_mode, U2 cons_pred_mode_child[1], U7 cu_width,
	U7 cu_height, U3 split_mode, U2 cons_flag, COM_SPLIT_STRUCT* split_struct);

void copy_md_fw(MD_FW* tmp, MD_FW* dst);

void copy_md_input(MD_INPUT* tmp, MD_INPUT* dst);

void copy_rdoq(RDOQ_MODEL* tmp, RDOQ_MODEL* dst);

void copy_rmd_output(RMD_OUTPUT* tmp, RMD_OUTPUT* dst);

void copy_ref_win(strFetch_ref_window* tmp, strFetch_ref_window* dst);

void copy_orig_lcu(strFetch_Orig_Lcu* tmp, strFetch_Orig_Lcu* dst);

void copy_enc_me_line_map(ENC_ME_LINE_MAP* tmp, ENC_ME_LINE_MAP* dst);

void copy_enc_me_line_map_DBK(ENC_ME_LINE_MAP* tmp, ENC_ME_LINE_MAP* dst);

void copy_enc_me_line_map_AEC(ENC_ME_LINE_MAP* tmp, ENC_ME_LINE_MAP* dst);

void copy_skip_merge(SKIP_MERGE_INPUT* tmp, SKIP_MERGE_INPUT* dst);

void copy_fme_info(ENC_FME_INFO_ALL* tmp, ENC_FME_INFO_ALL* dst);

void copy_ip_array(IP_ARRAY* tmp, IP_ARRAY* dst);

void copy_mpm_array(MPM_ARRAY* tmp, MPM_ARRAY* dst);

void copy_ip_map_scu(IP_MAP_SCU* tmp, IP_MAP_SCU* dst);

#endif

#if ENABLE_BFRAME
S8 get_lidx_by_refi(S3 refi0, S3 refi1);
#endif

s64 md_16x16_16x8_8x16_loop_md1(U3 split_mode,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][ MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4],
	SKIP_MERGE_INPUT* skip_in, U1 isBTH_allow, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_ptr,
	U2 cons_pred_mode_BTH[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);
s64 md_8x8_loop(U1 btv_allowed,
	MD_FW* md_fw_ptr, MD_INPUT* md_input_ptr, RMD_OUTPUT* g_rmd_output,
	pel RefWindowLCU_UP[(SWH * SWW) >> 2], pel RefWindowLCU_VP[(SWH * SWW) >> 2], S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS],
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2], pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2], S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS], ME_MV_DIR* me_mv_dir,
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH],
	ENC_ME_LINE_MAP_SIZE<16>* me_line_map_ptr_16,
	ENC_ME_NEB_INFO up_nebs_father[6], ENC_ME_NEB_INFO left_nebs_father[4],
	SKIP_MERGE_INPUT* skip_in, U3 split_mode, U13 x0, U13 y0,
	U12 cup, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 qt_depth, U3 bet_depth, U2 cons_pred_mode,
	U8 cu_width, U8 cu_height, U2 cons_flag,
	ENC_CU_DATA_ARRAY_SIZE<16>* cu_data_ptr,
	U2 cons_pred_mode_BTV[1], U2 tree_status, U2 child_tree_status[1],
	ENC_FME_INFO_ALL* fme_mv_info,
	IP_ARRAY_SIZE<16, 16>* IP_buffer_ptr_16x16,
	MPM_ARRAY_SIZE<16, 16>* MPM_buffer_ptr_16x16,
	IP_MAP_SCU_SIZE<16, 16>* IP_map_scu_ptr_16x16,
	Arbitor& ref,
	strFetch_ref_window* Fetch_Ref_window_ptr);
void store_enc_info_best_16_top(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY_SIZE<16>* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], ENC_CU_DATA_ARRAY_SIZE<32>* prev_cu_data_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw);

#if !ONLY_QT_QT
void pu_16x16_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);

void pu_16x16_8x16_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);

void pu_16x16_16x8_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);
#endif
void store_enc_info_best(MD_FW* md_fw_ptr, ENC_CU_DATA_ARRAY* curr_cu_data_best, U14 x0, U14 y0, U12 cup, U3 cu_width_log2, U3 cu_height_log2,
	U4 cud, u8 cons_pred_mode, U8 cu_width, U8 cu_height, U2 tree_status, U3* best_split_mode,
	U2 best_cons_pred_mode[1], U3 num_split_to_try, ENC_CU_DATA* prev_up_cu_data_temp_array_ptr, U14 x_pos, U14 y_pos,
	U3 prev_up_log2_sub_cuw);

void copy_enc_me_line_map(ENC_ME_LINE_MAP* tmp, ENC_ME_LINE_MAP* dst);