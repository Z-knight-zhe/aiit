#include "com_def.h"
#include "enc_def.h"
#include "type_def.h"


//#define sao_bug 1

#define COLLECT_SAO_DATA     0
#define RFC_DDR				 0

#if COLLECT_SAO_DATA
#define SAO_CU_NUM_TV                                        10
#define	SAO_INPUT_dbk_out_para_set_min_scu_x				 "sao_input_dbk_out_para_set_min_scu_x.dat"
#define	SAO_INPUT_dbk_out_para_set_min_scu_y				 "sao_input_dbk_out_para_set_min_scu_y.dat"
#define	SAO_INPUT_me_line_map_ptr_map_cu_mode_inner          "sao_input_me_line_map_ptr_map_cu_mode_inner.dat"
#define	SAO_INPUT_dbk_out_para_set_curr_ctu_idx				 "sao_input_dbk_out_para_set_curr_ctu_idx.dat"
#define	SAO_INPUT_dbk_out_para_set_map_scu_inner			 "sao_input_dbk_out_para_set_map_scu_inner.dat"
#define	SAO_INPUT_inner_nebs2sao							 "sao_input_inner_nebs2sao.dat"
#define	SAO_INPUT_dbk_out_para_set_pic_width				 "sao_input_dbk_out_para_set_pic_width.dat"
#define	SAO_INPUT_dbk_out_para_set_pic_height				 "sao_input_dbk_out_para_set_pic_height.dat"
#define	SAO_INPUT_dbk_out_para_set_pic_width_in_lcu			 "sao_input_dbk_out_para_set_pic_width_in_lcu.dat"
#define	SAO_INPUT_dbk_out_para_set_pic_height_in_lcu		 "sao_input_dbk_out_para_set_pic_height_in_lcu.dat"
#define	SAO_INPUT_dbk_out_para_set_x_pel					 "sao_input_dbk_out_para_set_x_pel.dat"
#define	SAO_INPUT_dbk_out_para_set_y_pel					 "sao_input_dbk_out_para_set_y_pel.dat"
#define	SAO_INPUT_dbkout_y									 "sao_input_dbkout_y.dat"
#define	SAO_INPUT_dbkout_u									 "sao_input_dbkout_u.dat"
#define	SAO_INPUT_dbkout_v									 "sao_input_dbkout_v.dat"
#define	SAO_INPUT_Orig_Lcu_ptr_y_org_sao					 "sao_input_Orig_Lcu_ptr_y_org_sao.dat"
#define	SAO_INPUT_Orig_Lcu_ptr_u_org_sao					 "sao_input_Orig_Lcu_ptr_u_org_sao.dat"
#define	SAO_INPUT_Orig_Lcu_ptr_v_org_sao					 "sao_input_Orig_Lcu_ptr_v_org_sao.dat"
#define	SAO_INPUT_fetch2sao_fw_ptr							 "sao_input_fetch2sao_fw_ptr.dat"
#define SAO_INPUT_qp_offset                                  "sao_input_qp_offset.dat"
#define	SAO_OUTPUT_cur_saoParam								 "sao_output_cur_saoParam.dat"
#define	SAO_OUTPUT_ctx_map_scu							     "sao_output_ctx_map_scu.dat"
#define	SAO_OUTPUT_ctx_map_mv								 "sao_output_ctx_map_mv.dat"
#define	SAO_OUTPUT_ctx_map_refi								 "sao_output_ctx_map_refi.dat"
#define	SAO_OUTPUT_ctx_map_cu_mode							 "sao_output_ctx_map_cu_mode.dat"
#define	SAO_OUTPUT_y_rec									 "sao_output_y_rec.dat"
#define	SAO_OUTPUT_u_rec									 "sao_output_u_rec.dat"
#define	SAO_OUTPUT_v_rec									 "sao_output_v_rec.dat"
#define	SAO_OUTPUT_sao_test_y                                "sao_output_sao_test_y.dat"
#define	SAO_OUTPUT_sao_test_u                                "sao_output_sao_test_u.dat"
#define	SAO_OUTPUT_sao_test_v                                "sao_output_sao_test_v.dat"
#define	SAO_OUTPUT_AEC_DATA_input							 "sao_output_AEC_DATA_input.dat"
#endif


void getSaoMergeNeighbor(COM_INFO* info, int pic_width_scu, int pic_width_lcu, int lcu_pos, int mb_y, int mb_x, int* MergeAvail, SAOBlkParam sao_merge_param[NUM_SAO_MERGE_TYPES][3]);
void no_filter_bypass(int use_deblock, int sample_adaptive_offset_enable_flag, COM_PIC* recpic, int pix_y_luma, int pix_x_luma, int rec_stride_luma, int rec_stride_chroma,
	pel dbk_y[71][71], pel dbk_u[39][39], pel dbk_v[39][39], pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39]);

#if 0//原有代码部分：sao_run、sao_output_rearrange、rfc_run
void sao_run(u32* ctx_map_scu, s16* ctx_map_mv, s8* ctx_map_refi, u32* ctx_map_cu_mode,
	U11 dbk_out_para_set_min_scu_x, U11 dbk_out_para_set_min_scu_y, U32 me_line_map_ptr_map_cu_mode_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U1 dbk_out_para_set_curr_ctu_idx, U32 dbk_out_para_set_map_scu_inner[2][16][16],
	ENC_ME_NEB_INFO inner_nebs2sao[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height, U6 dbk_out_para_set_pic_width_in_lcu, U6 dbk_out_para_set_pic_height_in_lcu,
	U12 dbk_out_para_set_x_pel, U12 dbk_out_para_set_y_pel, pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39], SAOBlkParam cur_saoParam[3],
	pel Orig_Lcu_ptr_y_org_sao[64][64], pel Orig_Lcu_ptr_u_org_sao[32][32], pel Orig_Lcu_ptr_v_org_sao[32][32],
	pel* y_rec, pel* u_rec, pel* v_rec,
	FETCH2SAO_FW* fetch2sao_fw_ptr, SAO2AEC* AEC_DATA_input, U32 qp_offset,
	pel sao_pic_rec_test_y[69][69], pel sao_pic_rec_test_u[36][36], pel sao_pic_rec_test_v[36][36]);

void sao_output_rearrange(int max_smb_num, int lcu_cnt, U6 pic_width_in_lcu, int pix_y_luma, int pix_x_luma, int pix_y_chroma, int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma,
	pel* y_rec, pel* u_rec, pel* v_rec,
	pel sao_test_y[69][69], pel sao_test_u[36][36], pel sao_test_v[36][36]);

void rfc_run(int pix_y_luma, int pix_x_luma, U13 pic_height, U13 pic_width, int rec_stride_luma, int rec_stride_chroma,
	pel sao_pic_rec_test_y[69][69], pel sao_pic_rec_test_u[36][36], pel sao_pic_rec_test_v[36][36],
	pel* y_rfcBin, U16* y_rfcPrm, pel* u_rfcBin, U16* u_rfcPrm, pel* v_rfcBin, U16* v_rfcPrm);
#endif//原有代码部分：sao_run、sao_output_rearrange、rfc_run
/*******************************************基于函数重载定义**********************************************************/
typedef enum
{
	FIRST_COL = 0,
	FIRST_ROW_LAST_COL,
	FIRST_ROW_NOT_FIRST_COL_NOT_LAST_COL,
	OTHER_ROW_LAST_COL,
	OTHER_ROW_NOT_FIRST_COL_NOT_LAST_COL,
	LAST_ROW_LAST_COL,
	LAST_ROW_NOT_FIRST_COL_NOT_LAST_COL,
}LCU_POSITION_T;

typedef struct
{
	//通过下面4个参数确定当前CTU的分布情况 
	int lcu_use_left;
	int lcu_use_right;
	int lcu_use_up;
	int lcu_use_down;

	LCU_POSITION_T lcu_positon;

}LCU_2RFC_PARAM_T;

typedef struct
{
	int lcu_luma_width;//ctu宽度
	int lcu_luma_height;//ctu高度
	int lcu_chroma_width;//ctu宽度
	int lcu_chroma_height;//ctu高度

	int lcu_luma_pos_x;//ctu_x坐标
	int lcu_luma_pos_y;//ctu_y坐标
	int lcu_chroma_pos_x;//ctu_x坐标
	int lcu_chroma_pos_y;//ctu_y坐标

    int lcu_luma_col_nums;//列方向CTU个数
	int lcu_luma_row_nums;//行方向CTU个数
	int lcu_chroma_col_nums;//列方向CTU个数
	int lcu_chroma_row_nums;//行方向CTU个数

}SAO_2RTL_PARAM_T;

void sao_run(/*U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height, U6 dbk_out_para_set_pic_width_in_lcu, U6 dbk_out_para_set_pic_height_in_lcu,*/
    U12 fetch_output_sao_pix_x, U12 fetch_output_sao_pix_y, pel dbkout_y[71][71], pel dbkout_u[39][39], pel dbkout_v[39][39], SAOBlkParam cur_saoParam[3],
    pel md2sao_y_org_sao[64][64], pel md2sao_u_org_sao[32][32], pel md2sao_v_org_sao[32][32],
    FETCH2SAO_FW* fetch2sao_fw_ptr, SAO2AEC* AEC_DATA_input, /*U32 qp_offset,*/ LCU_2RFC_PARAM_T* lcu_2rfc_param, SAO_2RTL_PARAM_T* sao_2rtl_param,
    pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
    pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32]);

void sao_output_rearrange(int max_smb_num, int lcu_cnt, U6 pic_width_in_lcu, int pix_y_luma, int pix_x_luma, int pix_y_chroma,
	int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param,SAO_2RTL_PARAM_T* sao_2rtl_param,
	pel sao_test_y[69][69], pel sao_test_u[36][36], pel sao_test_v[36][36],
	pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
	pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32]);
void sao_output_rearrange_skip(int max_smb_num, int lcu_cnt, U6 pic_width_in_lcu, int pix_y_luma, int pix_x_luma, int pix_y_chroma,
	int pix_x_chroma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param, SAO_2RTL_PARAM_T* sao_2rtl_param);

//目前该函数ctx_map_mv ctx_map_refi地址未做32bit对齐，故hls综合处理的cycle数比较大(15367)；将ctx_map_mv ctx_map_refi的指针
//类型修改为u32*之后，cycle数会大幅度降低(2000多一点)
void update_ctx_map_frame_level(u32* ctx_map_scu, s32* ctx_map_mv, s32* ctx_map_refi, u32* ctx_map_cu_mode, 
	U13 dbk_out_para_set_pic_width, U13 dbk_out_para_set_pic_height, U10 dbk_out_para_set_pic_width_in_scu,
	U11 dbk_out_para_set_min_scu_x, U11 dbk_out_para_set_min_scu_y, U32 map_cu_mode_inner_to_store[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	U1 dbk_out_para_set_curr_ctu_idx, U32 dbk_out_para_set_map_scu_inner[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
	int dbk_out_para_set_x_pel, int dbk_out_para_set_y_pel, ENC_ME_NEB_INFO inner_nebs2sao[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN]);
#ifndef __SYNTHESIS__
void rfc_run(int pix_y_luma, int pix_x_luma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param,
	pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
	pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32],
	pel* y_rec, pel* u_rec, pel* v_rec,
	pel* y_rfcBin, u16* y_rfcPrm, pel* u_rfcBin, u16* u_rfcPrm, pel* v_rfcBin, u16* v_rfcPrm);

void rfc_run_skip(int pix_y_luma, int pix_x_luma, int rec_stride_luma, int rec_stride_chroma, LCU_2RFC_PARAM_T* lcu_2rfc_param,
	pel sao_pic_rec_test_y[72][64], pel sao_pic_rec_test_u[36][32], pel sao_pic_rec_test_v[36][32],
	pel sao_pic_rec_test_y_last[72][64], pel sao_pic_rec_test_u_last[36][32], pel sao_pic_rec_test_v_last[36][32],
	pel* y_rec, pel* u_rec, pel* v_rec,
	pel* y_rfcBin, u16* y_rfcPrm, pel* u_rfcBin, u16* u_rfcPrm, pel* v_rfcBin, u16* v_rfcPrm);
#endif
/******************************************************************************************************************/