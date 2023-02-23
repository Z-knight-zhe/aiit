#include "fme.h"
//#include "fme_testbench.h"
#define CHECK832 0
#define PRINTF 0
#if COLLECT_FME_DATA
FILE *open_file(char* filename, char* mode)
{
	FILE *f;
	f = fopen(filename, mode);
	if (!f)
	{
		perror(filename);
		exit(0);
	}
	return f;
}

FILE* fme_input_info_f;
FILE* fme_output_info_f;
static int fme_cu_cnt = 0;

void fme_save_indata(U8 RefWindowLCU[88][88], U8 OrgY[64][64], ME_MV_INFO_BW *me_mv_info,
	ME_PMV_INFO_BW *me_pmv_info, IME_OUT_PARA_SET* ime_out_para_set)
{
	fwrite(OrgY, sizeof(U8), 64 * 64, fme_input_info_f);
	fwrite(RefWindowLCU, sizeof(U8), 88 * 88, fme_input_info_f);
	fwrite(me_mv_info, sizeof(ME_MV_INFO_BW), 1, fme_input_info_f);
	fwrite(me_pmv_info, sizeof(ME_PMV_INFO_BW), 1, fme_input_info_f);
	fwrite(ime_out_para_set, sizeof(IME_OUT_PARA_SET), 1, fme_input_info_f);
}

void fme_save_outdata(ENC_FME_INFO_ALL *fme_mv_info, FME_OUT_PARA_SET* fme_out_para_set, U8 Fetch_Refwin_ptr_refwin_md[FME_SW_SIZE][FME_SW_SIZE])
{
	fwrite(fme_mv_info, sizeof(ENC_FME_INFO_ALL), 1, fme_output_info_f);
	fwrite(fme_out_para_set, sizeof(FME_OUT_PARA_SET), 1, fme_output_info_f);
	fwrite(Fetch_Refwin_ptr_refwin_md, sizeof(U8), 88 * 88, fme_output_info_f);
}
#endif

/*
void write_out_para_set_fme(IME_OUT_PARA_SET* in_para, FME_OUT_PARA_SET* out_para,
	U5 *ime_num_refp, U13 *ime_pix_x, U13 *ime_pix_y, U32 *ime_lambda_mv, U2 *ime_slice_type, S14 ime_CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U4 * bit_depth2
#if ENABLE_BFRAME
	, U5 *ime_num_refp_1,
S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS]
#endif
)
{
#if ORR_T	
#pragma HLS ARRAY_PARTITION variable=in_para->y_org complete dim=1
#else
#pragma HLS ARRAY_PARTITION variable=in_para->y_org complete dim=2
#endif
#pragma HLS ARRAY_PARTITION variable=in_para->u_org complete dim=2
#pragma HLS ARRAY_PARTITION variable=in_para->v_org complete dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_Y cyclic factor=64 dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_U cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_V cyclic factor=32 dim=2

#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_Y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_U core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_V core=RAM_1P_BRAM


#pragma HLS ARRAY_PARTITION variable=in_para->CuQPMap complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_para->CuQPMap complete dim=1

#if ENABLE_BFRAME
	*ime_num_refp = in_para->num_refp[0];
	*ime_num_refp_1 = in_para->num_refp[REFP_1];
	ime_CtrPos_1[0][0][0] = in_para->CtrPos_ref1[0][0][0];
	ime_CtrPos_1[0][0][1] = in_para->CtrPos_ref1[0][0][1];
	ime_CtrPos_1[0][1][0] = in_para->CtrPos_ref1[0][1][0];
	ime_CtrPos_1[0][1][1] = in_para->CtrPos_ref1[0][1][1];
	ime_CtrPos_1[1][0][0] = in_para->CtrPos_ref1[1][0][0];
	ime_CtrPos_1[1][0][1] = in_para->CtrPos_ref1[1][0][1];
	ime_CtrPos_1[1][1][0] = in_para->CtrPos_ref1[1][1][0];
	ime_CtrPos_1[1][1][1] = in_para->CtrPos_ref1[1][1][1];

	out_para->CtrPosFME_ref1[0][0][0] = in_para->CtrPosFME_ref1[0][0][0];
	out_para->CtrPosFME_ref1[0][0][1] = in_para->CtrPosFME_ref1[0][0][1];
	out_para->CtrPosFME_ref1[0][1][0] = in_para->CtrPosFME_ref1[0][1][0];
	out_para->CtrPosFME_ref1[0][1][1] = in_para->CtrPosFME_ref1[0][1][1];
	out_para->CtrPosFME_ref1[1][0][0] = in_para->CtrPosFME_ref1[1][0][0];
	out_para->CtrPosFME_ref1[1][0][1] = in_para->CtrPosFME_ref1[1][0][1];
	out_para->CtrPosFME_ref1[1][1][0] = in_para->CtrPosFME_ref1[1][1][0];
	out_para->CtrPosFME_ref1[1][1][1] = in_para->CtrPosFME_ref1[1][1][1];

	out_para->CtrPos2MD_ref1[0][0][0] = in_para->CtrPos2MD_ref1[0][0][0];
	out_para->CtrPos2MD_ref1[0][0][1] = in_para->CtrPos2MD_ref1[0][0][1];
	out_para->CtrPos2MD_ref1[0][1][0] = in_para->CtrPos2MD_ref1[0][1][0];
	out_para->CtrPos2MD_ref1[0][1][1] = in_para->CtrPos2MD_ref1[0][1][1];
	out_para->CtrPos2MD_ref1[1][0][0] = in_para->CtrPos2MD_ref1[1][0][0];
	out_para->CtrPos2MD_ref1[1][0][1] = in_para->CtrPos2MD_ref1[1][0][1];
	out_para->CtrPos2MD_ref1[1][1][0] = in_para->CtrPos2MD_ref1[1][1][0];
	out_para->CtrPos2MD_ref1[1][1][1] = in_para->CtrPos2MD_ref1[1][1][1];
#else
	* ime_num_refp = in_para->num_refp[0];
#endif

	#if USE_INTRA_REFRESH
	out_para->EncRefresh = in_para->EncRefresh;
	out_para->RefreshSize = in_para->RefreshSize;
	out_para->RefreshMode = in_para->RefreshMode;
	out_para->pirStartRow = in_para->pirStartRow;
	out_para->pirEndRow = in_para->pirEndRow;
	out_para->pirStartCol = in_para->pirStartCol;
	out_para->pirEndCol = in_para->pirEndCol;
	out_para->numCuInWidth = in_para->numCuInWidth;
	out_para->numCuInHeight = in_para->numCuInHeight;
#endif
#if USE_SPEED_LEVEL
	out_para->speed_level = in_para->speed_level;
#endif
#if USE_ROI_MODE_CONFIG
	out_para->valid_mode[0] = in_para->valid_mode[0];
	out_para->valid_mode[1] = in_para->valid_mode[1];
	out_para->valid_mode[2] = in_para->valid_mode[2];
#endif
#if CUDQP_QP_MAP
	out_para->cu_dqp_enable = in_para->cu_dqp_enable;
	out_para->cu_qp_group_area_size = in_para->cu_qp_group_area_size;
	out_para->cu_qp_group_pred_qp = in_para->cu_qp_group_pred_qp;
	out_para->md_slice_qp = in_para->md_slice_qp;

	for (int i = 0; i < 256; i++)
	{
#pragma HLS UNROLL
		out_para->CuQPMap[i] = in_para->CuQPMap[i];
	}

#endif
	
	*ime_pix_x = in_para->pix_x;
	*ime_pix_y = in_para->pix_y;
	*ime_lambda_mv = in_para->lambda_mv;
	*ime_slice_type = in_para->slice_type;
	ime_CtrPos[0][0][0] = in_para->CtrPos[0][0][0];
	ime_CtrPos[0][0][1] = in_para->CtrPos[0][0][1];
	ime_CtrPos[0][1][0] = in_para->CtrPos[0][1][0];
	ime_CtrPos[0][1][1] = in_para->CtrPos[0][1][1];
	ime_CtrPos[1][0][0] = in_para->CtrPos[1][0][0];
	ime_CtrPos[1][0][1] = in_para->CtrPos[1][0][1];
	ime_CtrPos[1][1][0] = in_para->CtrPos[1][1][0];
	ime_CtrPos[1][1][1] = in_para->CtrPos[1][1][1];

	int iter;

	/////Frame-level paramters
	out_para->pic_width = in_para->pic_width;
	out_para->pic_height = in_para->pic_height;
	out_para->slice_type = in_para->slice_type;
	out_para->pic_width_in_scu = in_para->pic_width_in_scu;
	out_para->pic_height_in_scu = in_para->pic_height_in_scu;
	out_para->log2_max_cuwh = in_para->log2_max_cuwh;
	out_para->log2_culine = in_para->log2_culine;
	out_para->max_cuwh = in_para->max_cuwh;
	//out_para->f_scu = in_para->f_scu;

	//out_para->gop_size = in_para->gop_size;
#if EIPM
	out_para->eipm_enable_flag = in_para->eipm_enable_flag;
#endif
#if MIPF
	out_para->mipf_enable_flag = in_para->mipf_enable_flag;
#endif
#if INTERPF
	out_para->interpf_enable_flag = in_para->interpf_enable_flag;
#endif
#if CTX_CONTROL_LUKE
	out_para->bit_depth_internal = in_para->bit_depth_internal;
	*bit_depth2 = in_para->bit_depth_internal;
	out_para->max_part_ratio = in_para->max_part_ratio;
	out_para->min_cu_size = in_para->min_cu_size;
	out_para->max_split_times = in_para->max_split_times;
	out_para->min_qt_size = in_para->min_qt_size;
	out_para->max_bt_size = in_para->max_bt_size;
	out_para->max_eqt_size = in_para->max_eqt_size;
	//out_para->affine_subblock_size_idx = in_para->affine_subblock_size_idx;
	//out_para->ipcm_enable_flag = in_para->ipcm_enable_flag;
	//out_para->position_based_transform_enable_flag = in_para->position_based_transform_enable_flag;
	//out_para->secondary_transform_enable_flag = in_para->secondary_transform_enable_flag;
	//out_para->tscpm_enable_flag = in_para->tscpm_enable_flag;
	//out_para->amvr_enable_flag = in_para->amvr_enable_flag;
	//out_para->emvr_enable_flag = in_para->emvr_enable_flag;
#endif
#if CUDQP_QP_MAP
	out_para->chroma_quant_param_delta_cb = in_para->chroma_quant_param_delta_cb;
	out_para->chroma_quant_param_delta_cr = in_para->chroma_quant_param_delta_cr;
#endif
	for (iter = 0; iter < 17; iter++)
	{
		out_para->refp_ptr[iter][0] = in_para->refp_ptr[iter][0];
		out_para->refp_ptr[iter][1] = in_para->refp_ptr[iter][1];
	}
#if ENABLE_BFRAME
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#else
	//out_para->num_refp = in_para->num_refp;
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#endif
	
	out_para->cur_ptr = in_para->cur_ptr; // ref pic


	out_para->pix_x = in_para->pix_x;
	out_para->pix_y = in_para->pix_y;
#if CTU_CONTROL_LUKE
	//out_para->lcu_cnt = in_para->lcu_cnt;
#endif
	//out_para->smb_num_width = in_para->smb_num_width;
	//out_para->smb_num_height = in_para->smb_num_height;
#if CTU_CONTROL_LUKE


	out_para->qp_y = in_para->qp_y;
	out_para->qp_u = in_para->qp_u;
	out_para->qp_v = in_para->qp_v;
	out_para->affine_enable_flag = in_para->affine_enable_flag;
	out_para->umve_set_flag = in_para->umve_set_flag;
	//out_para->lambda_mv = in_para->lambda_mv;
	out_para->lambda[0] = in_para->lambda[0];
	out_para->lambda[1] = in_para->lambda[1];
	out_para->lambda[2] = in_para->lambda[2];
	//out_para->sqrt_lambda[0] = in_para->sqrt_lambda[0];
	//out_para->sqrt_lambda[1] = in_para->sqrt_lambda[1];
	//out_para->sqrt_lambda[2] = in_para->sqrt_lambda[2];
	out_para->dist_chroma_weight[0] = in_para->dist_chroma_weight[0];
	out_para->dist_chroma_weight[1] = in_para->dist_chroma_weight[1];

	//out_para->ptr = in_para->ptr;
	out_para->x_last_ctu = in_para->x_last_ctu;
	out_para->y_last_ctu = in_para->y_last_ctu;
#endif
	//out_para->CtrPos[0][0][0] = in_para->CtrPos[0][0][0];
	//out_para->CtrPos[0][0][1] = in_para->CtrPos[0][0][1];
	//out_para->CtrPos[0][1][0] = in_para->CtrPos[0][1][0];
	//out_para->CtrPos[0][1][1] = in_para->CtrPos[0][1][1];
	//out_para->CtrPos[1][0][0] = in_para->CtrPos[1][0][0];
	//out_para->CtrPos[1][0][1] = in_para->CtrPos[1][0][1];
	//out_para->CtrPos[1][1][0] = in_para->CtrPos[1][1][0];
	//out_para->CtrPos[1][1][1] = in_para->CtrPos[1][1][1];

	out_para->CtrPosFME[0][0][0] = in_para->CtrPosFME[0][0][0];
	out_para->CtrPosFME[0][0][1] = in_para->CtrPosFME[0][0][1];
	out_para->CtrPosFME[0][1][0] = in_para->CtrPosFME[0][1][0];
	out_para->CtrPosFME[0][1][1] = in_para->CtrPosFME[0][1][1];
	out_para->CtrPosFME[1][0][0] = in_para->CtrPosFME[1][0][0];
	out_para->CtrPosFME[1][0][1] = in_para->CtrPosFME[1][0][1];
	out_para->CtrPosFME[1][1][0] = in_para->CtrPosFME[1][1][0];
	out_para->CtrPosFME[1][1][1] = in_para->CtrPosFME[1][1][1];

	out_para->CtrPos2MD[0][0][0] = in_para->CtrPos2MD[0][0][0];
	out_para->CtrPos2MD[0][0][1] = in_para->CtrPos2MD[0][0][1];
	out_para->CtrPos2MD[0][1][0] = in_para->CtrPos2MD[0][1][0];
	out_para->CtrPos2MD[0][1][1] = in_para->CtrPos2MD[0][1][1];
	out_para->CtrPos2MD[1][0][0] = in_para->CtrPos2MD[1][0][0];
	out_para->CtrPos2MD[1][0][1] = in_para->CtrPos2MD[1][0][1];
	out_para->CtrPos2MD[1][1][0] = in_para->CtrPos2MD[1][1][0];
	out_para->CtrPos2MD[1][1][1] = in_para->CtrPos2MD[1][1][1];

pel u_org_reg[32];
pel v_org_reg[32];
//pel y_org_reg[64];
#pragma HLS ARRAY_PARTITION variable=u_org_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable=v_org_reg complete dim=1
//#pragma HLS ARRAY_PARTITION variable=y_org_reg complete dim=1
	//copy to fme add by xgq
	for (int j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		for(int k = 0; k < 32; k++)
		{
#pragma HLS UNROLL
			u_org_reg[k] = in_para->u_org[j][k];
			v_org_reg[k] = in_para->v_org[j][k];
		}

		for (int i = 0; i < 32; i++)
		{
#pragma HLS UNROLL
		//	out_para->p_fenc_LCU_UV[j * 64 + i] = in_para->u_org[j][i];
		//	out_para->p_fenc_LCU_UV[j * 64 + i + 32] = in_para->v_org[j][i];
			out_para->p_fenc_LCU_U[j][i] = u_org_reg[i];
			out_para->p_fenc_LCU_V[j][i] = v_org_reg[i];
		}
	}


	for (int j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 64; i++)
		{
#pragma HLS UNROLL
#if ORR_T
			out_para->p_fenc_LCU_Y[j][i] = in_para->y_org[i][j];
#else
			out_para->p_fenc_LCU_Y[j][i] = in_para->y_org[j][i];
#endif
		}
	}
}
*/


void ime_out_copy(IME_OUT_PARA_SET* in_para, FME_OUT_PARA_SET_COPY* out_para,
	U5* ime_num_refp, U13* ime_pix_x, U13* ime_pix_y, U32* ime_lambda_mv, U2* ime_slice_type, S14 ime_CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U4* bit_depth2
#if ENABLE_BFRAME
	, U5* ime_num_refp_1,
	S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS]
#endif
)
{
#if ORR_T
#pragma HLS ARRAY_PARTITION variable=in_para->y_org complete dim=1
#else
#pragma HLS ARRAY_PARTITION variable=in_para->y_org complete dim=2
#endif
#pragma HLS ARRAY_PARTITION variable=in_para->u_org complete dim=2
#pragma HLS ARRAY_PARTITION variable=in_para->v_org complete dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_Y cyclic factor=64 dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_U cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_V cyclic factor=32 dim=2

#pragma HLS ARRAY_PARTITION variable=out_para->CuQPMap complete dim=1
#pragma HLS ARRAY_PARTITION variable=in_para->CuQPMap complete dim=1

#if ENABLE_BFRAME
	*ime_num_refp = in_para->num_refp[0];
	*ime_num_refp_1 = in_para->num_refp[REFP_1];
	ime_CtrPos_1[0][0][0] = in_para->CtrPos_ref1[0][0][0];
	ime_CtrPos_1[0][0][1] = in_para->CtrPos_ref1[0][0][1];
	ime_CtrPos_1[0][1][0] = in_para->CtrPos_ref1[0][1][0];
	ime_CtrPos_1[0][1][1] = in_para->CtrPos_ref1[0][1][1];
	ime_CtrPos_1[1][0][0] = in_para->CtrPos_ref1[1][0][0];
	ime_CtrPos_1[1][0][1] = in_para->CtrPos_ref1[1][0][1];
	ime_CtrPos_1[1][1][0] = in_para->CtrPos_ref1[1][1][0];
	ime_CtrPos_1[1][1][1] = in_para->CtrPos_ref1[1][1][1];

	out_para->CtrPosFME_ref1[0][0][0] = in_para->CtrPosFME_ref1[0][0][0];
	out_para->CtrPosFME_ref1[0][0][1] = in_para->CtrPosFME_ref1[0][0][1];
	out_para->CtrPosFME_ref1[0][1][0] = in_para->CtrPosFME_ref1[0][1][0];
	out_para->CtrPosFME_ref1[0][1][1] = in_para->CtrPosFME_ref1[0][1][1];
	out_para->CtrPosFME_ref1[1][0][0] = in_para->CtrPosFME_ref1[1][0][0];
	out_para->CtrPosFME_ref1[1][0][1] = in_para->CtrPosFME_ref1[1][0][1];
	out_para->CtrPosFME_ref1[1][1][0] = in_para->CtrPosFME_ref1[1][1][0];
	out_para->CtrPosFME_ref1[1][1][1] = in_para->CtrPosFME_ref1[1][1][1];

	out_para->CtrPos2MD_ref1[0][0][0] = in_para->CtrPos2MD_ref1[0][0][0];
	out_para->CtrPos2MD_ref1[0][0][1] = in_para->CtrPos2MD_ref1[0][0][1];
	out_para->CtrPos2MD_ref1[0][1][0] = in_para->CtrPos2MD_ref1[0][1][0];
	out_para->CtrPos2MD_ref1[0][1][1] = in_para->CtrPos2MD_ref1[0][1][1];
	out_para->CtrPos2MD_ref1[1][0][0] = in_para->CtrPos2MD_ref1[1][0][0];
	out_para->CtrPos2MD_ref1[1][0][1] = in_para->CtrPos2MD_ref1[1][0][1];
	out_para->CtrPos2MD_ref1[1][1][0] = in_para->CtrPos2MD_ref1[1][1][0];
	out_para->CtrPos2MD_ref1[1][1][1] = in_para->CtrPos2MD_ref1[1][1][1];
#else
	* ime_num_refp = in_para->num_refp[0];
#endif

#if USE_INTRA_REFRESH
	out_para->EncRefresh = in_para->EncRefresh;
	out_para->RefreshSize = in_para->RefreshSize;
	out_para->RefreshMode = in_para->RefreshMode;
	out_para->pirStartRow = in_para->pirStartRow;
	out_para->pirEndRow = in_para->pirEndRow;
	out_para->pirStartCol = in_para->pirStartCol;
	out_para->pirEndCol = in_para->pirEndCol;
	out_para->numCuInWidth = in_para->numCuInWidth;
	out_para->numCuInHeight = in_para->numCuInHeight;
#endif
#if USE_SPEED_LEVEL
	out_para->speed_level = in_para->speed_level;
#endif
#if USE_ROI_MODE_CONFIG
	out_para->valid_mode[0] = in_para->valid_mode[0];
	out_para->valid_mode[1] = in_para->valid_mode[1];
	out_para->valid_mode[2] = in_para->valid_mode[2];
#endif
#if CUDQP_QP_MAP
	out_para->cu_dqp_enable = in_para->cu_dqp_enable;
	out_para->cu_qp_group_area_size = in_para->cu_qp_group_area_size;
	out_para->cu_qp_group_pred_qp = in_para->cu_qp_group_pred_qp;
	out_para->md_slice_qp = in_para->md_slice_qp;

	for (int i = 0; i < 256; i++)
	{
#pragma HLS UNROLL
		out_para->CuQPMap[i] = in_para->CuQPMap[i];
	}

#endif

	* ime_pix_x = in_para->pix_x;
	*ime_pix_y = in_para->pix_y;
	*ime_slice_type = in_para->slice_type;
	ime_CtrPos[0][0][0] = in_para->CtrPos[0][0][0];
	ime_CtrPos[0][0][1] = in_para->CtrPos[0][0][1];
	ime_CtrPos[0][1][0] = in_para->CtrPos[0][1][0];
	ime_CtrPos[0][1][1] = in_para->CtrPos[0][1][1];
	ime_CtrPos[1][0][0] = in_para->CtrPos[1][0][0];
	ime_CtrPos[1][0][1] = in_para->CtrPos[1][0][1];
	ime_CtrPos[1][1][0] = in_para->CtrPos[1][1][0];
	ime_CtrPos[1][1][1] = in_para->CtrPos[1][1][1];

	int iter;

	/////Frame-level paramters
	out_para->pic_width = in_para->pic_width;
	out_para->pic_height = in_para->pic_height;
	out_para->slice_type = in_para->slice_type;
	out_para->pic_width_in_scu = in_para->pic_width_in_scu;
	out_para->pic_height_in_scu = in_para->pic_height_in_scu;
	out_para->log2_max_cuwh = in_para->log2_max_cuwh;
	out_para->log2_culine = in_para->log2_culine;
	out_para->max_cuwh = in_para->max_cuwh;
	//out_para->f_scu = in_para->f_scu;
	/* gop size */
	//out_para->gop_size = in_para->gop_size;
#if EIPM
	out_para->eipm_enable_flag = in_para->eipm_enable_flag;
#endif
#if MIPF
	out_para->mipf_enable_flag = in_para->mipf_enable_flag;
#endif
#if INTERPF
	out_para->interpf_enable_flag = in_para->interpf_enable_flag;
#endif
#if CTX_CONTROL_LUKE
	out_para->bit_depth_internal = in_para->bit_depth_internal;
	*bit_depth2 = in_para->bit_depth_internal;
	out_para->max_part_ratio = in_para->max_part_ratio;
	out_para->min_cu_size = in_para->min_cu_size;
	out_para->max_split_times = in_para->max_split_times;
	out_para->min_qt_size = in_para->min_qt_size;
	out_para->max_bt_size = in_para->max_bt_size;
	out_para->max_eqt_size = in_para->max_eqt_size;
	//out_para->affine_subblock_size_idx = in_para->affine_subblock_size_idx;
	//out_para->ipcm_enable_flag = in_para->ipcm_enable_flag;
	//out_para->position_based_transform_enable_flag = in_para->position_based_transform_enable_flag;
	//out_para->secondary_transform_enable_flag = in_para->secondary_transform_enable_flag;
	//out_para->tscpm_enable_flag = in_para->tscpm_enable_flag;
	//out_para->amvr_enable_flag = in_para->amvr_enable_flag;
	//out_para->emvr_enable_flag = in_para->emvr_enable_flag;
#endif
#if CUDQP_QP_MAP
	out_para->chroma_quant_param_delta_cb = in_para->chroma_quant_param_delta_cb;
	out_para->chroma_quant_param_delta_cr = in_para->chroma_quant_param_delta_cr;
#endif
	for (iter = 0; iter < 17; iter++)
	{
#pragma HLS UNROLL
		out_para->refp_ptr[iter][0] = in_para->refp_ptr[iter][0];
		out_para->refp_ptr[iter][1] = in_para->refp_ptr[iter][1];
	}
#if ENABLE_BFRAME
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#else
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#endif

	out_para->cur_ptr = in_para->cur_ptr; // ref pic


	out_para->pix_x = in_para->pix_x;
	out_para->pix_y = in_para->pix_y;
#if CTU_CONTROL_LUKE
	//out_para->lcu_cnt = in_para->lcu_cnt;
#endif
	//out_para->smb_num_width = in_para->smb_num_width;
	//out_para->smb_num_height = in_para->smb_num_height;
#if CTU_CONTROL_LUKE

	/* QP for luma of current encoding CU */
	out_para->qp_y = in_para->qp_y;
	/* QP for chroma of current encoding CU */
	out_para->qp_u = in_para->qp_u;
	out_para->qp_v = in_para->qp_v;
	out_para->affine_enable_flag = in_para->affine_enable_flag;
	out_para->umve_set_flag = in_para->umve_set_flag;
	//out_para->lambda_mv = in_para->lambda_mv;
	out_para->lambda[0] = in_para->lambda[0];
	out_para->lambda[1] = in_para->lambda[1];
	out_para->lambda[2] = in_para->lambda[2];
	//out_para->sqrt_lambda[0] = in_para->sqrt_lambda[0];
	//out_para->sqrt_lambda[1] = in_para->sqrt_lambda[1];
	//out_para->sqrt_lambda[2] = in_para->sqrt_lambda[2];
	out_para->dist_chroma_weight[0] = in_para->dist_chroma_weight[0];
	out_para->dist_chroma_weight[1] = in_para->dist_chroma_weight[1];
	/* current frame numbser */
	//out_para->ptr = in_para->ptr;
	out_para->x_last_ctu = in_para->x_last_ctu;
	out_para->y_last_ctu = in_para->y_last_ctu;
#endif
	out_para->CtrPosFME[0][0][0] = in_para->CtrPosFME[0][0][0];
	out_para->CtrPosFME[0][0][1] = in_para->CtrPosFME[0][0][1];
	out_para->CtrPosFME[0][1][0] = in_para->CtrPosFME[0][1][0];
	out_para->CtrPosFME[0][1][1] = in_para->CtrPosFME[0][1][1];
	out_para->CtrPosFME[1][0][0] = in_para->CtrPosFME[1][0][0];
	out_para->CtrPosFME[1][0][1] = in_para->CtrPosFME[1][0][1];
	out_para->CtrPosFME[1][1][0] = in_para->CtrPosFME[1][1][0];
	out_para->CtrPosFME[1][1][1] = in_para->CtrPosFME[1][1][1];

	out_para->CtrPos2MD[0][0][0] = in_para->CtrPos2MD[0][0][0];
	out_para->CtrPos2MD[0][0][1] = in_para->CtrPos2MD[0][0][1];
	out_para->CtrPos2MD[0][1][0] = in_para->CtrPos2MD[0][1][0];
	out_para->CtrPos2MD[0][1][1] = in_para->CtrPos2MD[0][1][1];
	out_para->CtrPos2MD[1][0][0] = in_para->CtrPos2MD[1][0][0];
	out_para->CtrPos2MD[1][0][1] = in_para->CtrPos2MD[1][0][1];
	out_para->CtrPos2MD[1][1][0] = in_para->CtrPos2MD[1][1][0];
	out_para->CtrPos2MD[1][1][1] = in_para->CtrPos2MD[1][1][1];

	pel u_org_reg[32];
	pel v_org_reg[32];
#pragma HLS ARRAY_PARTITION variable=u_org_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable=v_org_reg complete dim=1
	//copy to fme add by xgq
//	for (int j = 0; j < 32; j++)
//	{
//#pragma HLS PIPELINE
//		for (int k = 0; k < 32; k++)
//		{
//#pragma HLS UNROLL
//			u_org_reg[k] = in_para->u_org[j][k];
//			v_org_reg[k] = in_para->v_org[j][k];
//		}
//
//		for (int i = 0; i < 32; i++)
//		{
//#pragma HLS UNROLL
//			//	out_para->p_fenc_LCU_UV[j * 64 + i] = in_para->u_org[j][i];
//			//	out_para->p_fenc_LCU_UV[j * 64 + i + 32] = in_para->v_org[j][i];
//			out_para->p_fenc_LCU_U[j][i] = u_org_reg[i];
//			out_para->p_fenc_LCU_V[j][i] = v_org_reg[i];
//		}
//	}
for (int j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		for (int k = 0; k < 32; k++)
		{
#pragma HLS UNROLL
			out_para->p_fenc_LCU_U[j][k] = in_para->u_org[j][k];
			out_para->p_fenc_LCU_V[j][k] = in_para->v_org[j][k];
		}
}

	for (int j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 64; i++)
		{
#pragma HLS UNROLL
#if ORR_T
			out_para->p_fenc_LCU_Y[j][i] = in_para->y_org[i][j];
#else
			out_para->p_fenc_LCU_Y[j][i] = in_para->y_org[j][i];
#endif
		}
	}
}

void write_out_para_set_fme(FME_OUT_PARA_SET_COPY* in_para, FME_OUT_PARA_SET* out_para)
{

#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_Y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_U cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_V cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=in_para->CuQPMap complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_para->CuQPMap complete dim=1
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_Y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_U core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_V core=RAM_1P_BRAM
#if ENABLE_BFRAME
	out_para->CtrPosFME_ref1[0][0][0] = in_para->CtrPosFME_ref1[0][0][0];
	out_para->CtrPosFME_ref1[0][0][1] = in_para->CtrPosFME_ref1[0][0][1];
	out_para->CtrPosFME_ref1[0][1][0] = in_para->CtrPosFME_ref1[0][1][0];
	out_para->CtrPosFME_ref1[0][1][1] = in_para->CtrPosFME_ref1[0][1][1];
	out_para->CtrPosFME_ref1[1][0][0] = in_para->CtrPosFME_ref1[1][0][0];
	out_para->CtrPosFME_ref1[1][0][1] = in_para->CtrPosFME_ref1[1][0][1];
	out_para->CtrPosFME_ref1[1][1][0] = in_para->CtrPosFME_ref1[1][1][0];
	out_para->CtrPosFME_ref1[1][1][1] = in_para->CtrPosFME_ref1[1][1][1];

	out_para->CtrPos2MD_ref1[0][0][0] = in_para->CtrPos2MD_ref1[0][0][0];
	out_para->CtrPos2MD_ref1[0][0][1] = in_para->CtrPos2MD_ref1[0][0][1];
	out_para->CtrPos2MD_ref1[0][1][0] = in_para->CtrPos2MD_ref1[0][1][0];
	out_para->CtrPos2MD_ref1[0][1][1] = in_para->CtrPos2MD_ref1[0][1][1];
	out_para->CtrPos2MD_ref1[1][0][0] = in_para->CtrPos2MD_ref1[1][0][0];
	out_para->CtrPos2MD_ref1[1][0][1] = in_para->CtrPos2MD_ref1[1][0][1];
	out_para->CtrPos2MD_ref1[1][1][0] = in_para->CtrPos2MD_ref1[1][1][0];
	out_para->CtrPos2MD_ref1[1][1][1] = in_para->CtrPos2MD_ref1[1][1][1];

#endif

#if USE_INTRA_REFRESH
	out_para->EncRefresh = in_para->EncRefresh;
	out_para->RefreshSize = in_para->RefreshSize;
	out_para->RefreshMode = in_para->RefreshMode;
	out_para->pirStartRow = in_para->pirStartRow;
	out_para->pirEndRow = in_para->pirEndRow;
	out_para->pirStartCol = in_para->pirStartCol;
	out_para->pirEndCol = in_para->pirEndCol;
	out_para->numCuInWidth = in_para->numCuInWidth;
	out_para->numCuInHeight = in_para->numCuInHeight;
#endif
#if USE_SPEED_LEVEL
	out_para->speed_level = in_para->speed_level;
#endif
#if USE_ROI_MODE_CONFIG
	out_para->valid_mode[0] = in_para->valid_mode[0];
	out_para->valid_mode[1] = in_para->valid_mode[1];
	out_para->valid_mode[2] = in_para->valid_mode[2];
#endif
#if CUDQP_QP_MAP
	out_para->cu_dqp_enable = in_para->cu_dqp_enable;
	out_para->cu_qp_group_area_size = in_para->cu_qp_group_area_size;
	out_para->cu_qp_group_pred_qp = in_para->cu_qp_group_pred_qp;
	out_para->md_slice_qp = in_para->md_slice_qp;

	for (int i = 0; i < 256; i++)
	{
#pragma HLS UNROLL
		out_para->CuQPMap[i] = in_para->CuQPMap[i];
	}

#endif
	int iter;
	/////Frame-level paramters
	out_para->pic_width = in_para->pic_width;
	out_para->pic_height = in_para->pic_height;
	out_para->slice_type = in_para->slice_type;
	out_para->pic_width_in_scu = in_para->pic_width_in_scu;
	out_para->pic_height_in_scu = in_para->pic_height_in_scu;
	out_para->log2_max_cuwh = in_para->log2_max_cuwh;
	out_para->log2_culine = in_para->log2_culine;
	out_para->max_cuwh = in_para->max_cuwh;
	//out_para->f_scu = in_para->f_scu;
	/* gop size */
	//out_para->gop_size = in_para->gop_size;
#if EIPM
	out_para->eipm_enable_flag = in_para->eipm_enable_flag;
#endif
#if MIPF
	out_para->mipf_enable_flag = in_para->mipf_enable_flag;
#endif
#if INTERPF
	out_para->interpf_enable_flag = in_para->interpf_enable_flag;
#endif
#if CTX_CONTROL_LUKE
	out_para->bit_depth_internal = in_para->bit_depth_internal;
	out_para->max_part_ratio = in_para->max_part_ratio;
	out_para->min_cu_size = in_para->min_cu_size;
	out_para->max_split_times = in_para->max_split_times;
	out_para->min_qt_size = in_para->min_qt_size;
	out_para->max_bt_size = in_para->max_bt_size;
	out_para->max_eqt_size = in_para->max_eqt_size;
	//out_para->affine_subblock_size_idx = in_para->affine_subblock_size_idx;
	//out_para->ipcm_enable_flag = in_para->ipcm_enable_flag;
	//out_para->position_based_transform_enable_flag = in_para->position_based_transform_enable_flag;
	//out_para->secondary_transform_enable_flag = in_para->secondary_transform_enable_flag;
	//out_para->tscpm_enable_flag = in_para->tscpm_enable_flag;
	//out_para->amvr_enable_flag = in_para->amvr_enable_flag;
	//out_para->emvr_enable_flag = in_para->emvr_enable_flag;
#endif
#if CUDQP_QP_MAP
	out_para->chroma_quant_param_delta_cb = in_para->chroma_quant_param_delta_cb;
	out_para->chroma_quant_param_delta_cr = in_para->chroma_quant_param_delta_cr;
#endif
	for (iter = 0; iter < 17; iter++)
	{
		out_para->refp_ptr[iter][0] = in_para->refp_ptr[iter][0];
		out_para->refp_ptr[iter][1] = in_para->refp_ptr[iter][1];
	}
#if ENABLE_BFRAME
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#else
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#endif

	out_para->cur_ptr = in_para->cur_ptr; // ref pic


	out_para->pix_x = in_para->pix_x;
	out_para->pix_y = in_para->pix_y;
#if CTU_CONTROL_LUKE
	//out_para->lcu_cnt = in_para->lcu_cnt;
#endif

#if CTU_CONTROL_LUKE

	// QP for luma of current encoding CU 
	out_para->qp_y = in_para->qp_y;
	// QP for chroma of current encoding CU 
	out_para->qp_u = in_para->qp_u;
	out_para->qp_v = in_para->qp_v;
	out_para->affine_enable_flag = in_para->affine_enable_flag;
	out_para->umve_set_flag = in_para->umve_set_flag;
	//out_para->lambda_mv = in_para->lambda_mv;
	out_para->lambda[0] = in_para->lambda[0];
	out_para->lambda[1] = in_para->lambda[1];
	out_para->lambda[2] = in_para->lambda[2];
	//out_para->sqrt_lambda[0] = in_para->sqrt_lambda[0];
	//out_para->sqrt_lambda[1] = in_para->sqrt_lambda[1];
	//out_para->sqrt_lambda[2] = in_para->sqrt_lambda[2];
	out_para->dist_chroma_weight[0] = in_para->dist_chroma_weight[0];
	out_para->dist_chroma_weight[1] = in_para->dist_chroma_weight[1];
	/* current frame numbser */
	//out_para->ptr = in_para->ptr;
	out_para->x_last_ctu = in_para->x_last_ctu;
	out_para->y_last_ctu = in_para->y_last_ctu;
#endif


	out_para->CtrPosFME[0][0][0] = in_para->CtrPosFME[0][0][0];
	out_para->CtrPosFME[0][0][1] = in_para->CtrPosFME[0][0][1];
	out_para->CtrPosFME[0][1][0] = in_para->CtrPosFME[0][1][0];
	out_para->CtrPosFME[0][1][1] = in_para->CtrPosFME[0][1][1];
	out_para->CtrPosFME[1][0][0] = in_para->CtrPosFME[1][0][0];
	out_para->CtrPosFME[1][0][1] = in_para->CtrPosFME[1][0][1];
	out_para->CtrPosFME[1][1][0] = in_para->CtrPosFME[1][1][0];
	out_para->CtrPosFME[1][1][1] = in_para->CtrPosFME[1][1][1];

	out_para->CtrPos2MD[0][0][0] = in_para->CtrPos2MD[0][0][0];
	out_para->CtrPos2MD[0][0][1] = in_para->CtrPos2MD[0][0][1];
	out_para->CtrPos2MD[0][1][0] = in_para->CtrPos2MD[0][1][0];
	out_para->CtrPos2MD[0][1][1] = in_para->CtrPos2MD[0][1][1];
	out_para->CtrPos2MD[1][0][0] = in_para->CtrPos2MD[1][0][0];
	out_para->CtrPos2MD[1][0][1] = in_para->CtrPos2MD[1][0][1];
	out_para->CtrPos2MD[1][1][0] = in_para->CtrPos2MD[1][1][0];
	out_para->CtrPos2MD[1][1][1] = in_para->CtrPos2MD[1][1][1];

/*	pel u_org_reg[32];
	pel v_org_reg[32];
#pragma HLS ARRAY_PARTITION variable=u_org_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable=v_org_reg complete dim=1*/

	//copy to fme add by xgq
	for (int j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		for (int k = 0; k < 32; k++)
		{
#pragma HLS UNROLL
			out_para->p_fenc_LCU_U[j][k] = in_para->p_fenc_LCU_U[j][k];
			out_para->p_fenc_LCU_V[j][k] = in_para->p_fenc_LCU_V[j][k];
		}

/*		for (int i = 0; i < 32; i++)
		{
#pragma HLS UNROLL
			out_para->p_fenc_LCU_U[j][i] = u_org_reg[i];
			out_para->p_fenc_LCU_V[j][i] = v_org_reg[i];
		}*/

	}


	for (int j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 64; i++)
		{
#pragma HLS UNROLL
#if ORR_T
			out_para->p_fenc_LCU_Y[j][i] = in_para->p_fenc_LCU_Y[i][j];
#else
			out_para->p_fenc_LCU_Y[j][i] = in_para->p_fenc_LCU_Y[j][i];
#endif
		}
	}
}

void write_out_para_set_fme_chroma(FME_OUT_PARA_SET_COPY* in_para, FME_OUT_PARA_SET_CHROMA* out_para
)
{
#pragma HLS ARRAY_PARTITION variable=in_para->p_fenc_LCU_Y cyclic factor=64 dim=2
#pragma HLS ARRAY_PARTITION variable=in_para->p_fenc_LCU_U cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=in_para->p_fenc_LCU_V cyclic factor=32 dim=2

#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_Y cyclic factor=64 dim=1
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_U cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=out_para->p_fenc_LCU_V cyclic factor=32 dim=1
#pragma HLS ARRAY_PARTITION variable=in_para->CuQPMap complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_para->CuQPMap complete dim=1
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_Y core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_U core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_para->p_fenc_LCU_V core=RAM_1P_BRAM

#if ENABLE_BFRAME
	out_para->CtrPosFME_ref1[0][0][0] = in_para->CtrPosFME_ref1[0][0][0];
	out_para->CtrPosFME_ref1[0][0][1] = in_para->CtrPosFME_ref1[0][0][1];
	out_para->CtrPosFME_ref1[0][1][0] = in_para->CtrPosFME_ref1[0][1][0];
	out_para->CtrPosFME_ref1[0][1][1] = in_para->CtrPosFME_ref1[0][1][1];
	out_para->CtrPosFME_ref1[1][0][0] = in_para->CtrPosFME_ref1[1][0][0];
	out_para->CtrPosFME_ref1[1][0][1] = in_para->CtrPosFME_ref1[1][0][1];
	out_para->CtrPosFME_ref1[1][1][0] = in_para->CtrPosFME_ref1[1][1][0];
	out_para->CtrPosFME_ref1[1][1][1] = in_para->CtrPosFME_ref1[1][1][1];

	out_para->CtrPos2MD_ref1[0][0][0] = in_para->CtrPos2MD_ref1[0][0][0];
	out_para->CtrPos2MD_ref1[0][0][1] = in_para->CtrPos2MD_ref1[0][0][1];
	out_para->CtrPos2MD_ref1[0][1][0] = in_para->CtrPos2MD_ref1[0][1][0];
	out_para->CtrPos2MD_ref1[0][1][1] = in_para->CtrPos2MD_ref1[0][1][1];
	out_para->CtrPos2MD_ref1[1][0][0] = in_para->CtrPos2MD_ref1[1][0][0];
	out_para->CtrPos2MD_ref1[1][0][1] = in_para->CtrPos2MD_ref1[1][0][1];
	out_para->CtrPos2MD_ref1[1][1][0] = in_para->CtrPos2MD_ref1[1][1][0];
	out_para->CtrPos2MD_ref1[1][1][1] = in_para->CtrPos2MD_ref1[1][1][1];

#endif

#if USE_INTRA_REFRESH
	out_para->EncRefresh = in_para->EncRefresh;
	out_para->RefreshSize = in_para->RefreshSize;
	out_para->RefreshMode = in_para->RefreshMode;
	out_para->pirStartRow = in_para->pirStartRow;
	out_para->pirEndRow = in_para->pirEndRow;
	out_para->pirStartCol = in_para->pirStartCol;
	out_para->pirEndCol = in_para->pirEndCol;
	out_para->numCuInWidth = in_para->numCuInWidth;
	out_para->numCuInHeight = in_para->numCuInHeight;
#endif
#if USE_SPEED_LEVEL
	out_para->speed_level = in_para->speed_level;
#endif
#if USE_ROI_MODE_CONFIG
	out_para->valid_mode[0] = in_para->valid_mode[0];
	out_para->valid_mode[1] = in_para->valid_mode[1];
	out_para->valid_mode[2] = in_para->valid_mode[2];
#endif
#if CUDQP_QP_MAP
	out_para->cu_dqp_enable = in_para->cu_dqp_enable;
	out_para->cu_qp_group_area_size = in_para->cu_qp_group_area_size;
	out_para->cu_qp_group_pred_qp = in_para->cu_qp_group_pred_qp;
	out_para->md_slice_qp = in_para->md_slice_qp;

	for (int i = 0; i < 256; i++)
	{
#pragma HLS UNROLL
		out_para->CuQPMap[i] = in_para->CuQPMap[i];
	}

#endif



	int iter;

	/////Frame-level paramters
	out_para->pic_width = in_para->pic_width;
	out_para->pic_height = in_para->pic_height;
	out_para->slice_type = in_para->slice_type;
	out_para->pic_width_in_scu = in_para->pic_width_in_scu;
	out_para->pic_height_in_scu = in_para->pic_height_in_scu;
	out_para->log2_max_cuwh = in_para->log2_max_cuwh;
	out_para->log2_culine = in_para->log2_culine;
	out_para->max_cuwh = in_para->max_cuwh;

#if EIPM
	out_para->eipm_enable_flag = in_para->eipm_enable_flag;
#endif
#if MIPF
	out_para->mipf_enable_flag = in_para->mipf_enable_flag;
#endif
#if INTERPF
	out_para->interpf_enable_flag = in_para->interpf_enable_flag;
#endif
#if CTX_CONTROL_LUKE
	out_para->bit_depth_internal = in_para->bit_depth_internal;
	out_para->max_part_ratio = in_para->max_part_ratio;
	out_para->min_cu_size = in_para->min_cu_size;
	out_para->max_split_times = in_para->max_split_times;
	out_para->min_qt_size = in_para->min_qt_size;
	out_para->max_bt_size = in_para->max_bt_size;
	out_para->max_eqt_size = in_para->max_eqt_size;
#endif
#if CUDQP_QP_MAP
	out_para->chroma_quant_param_delta_cb = in_para->chroma_quant_param_delta_cb;
	out_para->chroma_quant_param_delta_cr = in_para->chroma_quant_param_delta_cr;
#endif
	for (iter = 0; iter < 17; iter++)
	{
#pragma HLS UNROLL
		out_para->refp_ptr[iter][0] = in_para->refp_ptr[iter][0];
		out_para->refp_ptr[iter][1] = in_para->refp_ptr[iter][1];
	}
#if ENABLE_BFRAME
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#else
	out_para->num_refp[0] = in_para->num_refp[0];
	out_para->num_refp[1] = in_para->num_refp[1];
#endif

	out_para->cur_ptr = in_para->cur_ptr; // ref pic


	out_para->pix_x = in_para->pix_x;
	out_para->pix_y = in_para->pix_y;
#if CTU_CONTROL_LUKE

#endif

#if CTU_CONTROL_LUKE


	out_para->qp_y = in_para->qp_y;

	out_para->qp_u = in_para->qp_u;
	out_para->qp_v = in_para->qp_v;
	out_para->affine_enable_flag = in_para->affine_enable_flag;
	out_para->umve_set_flag = in_para->umve_set_flag;

	out_para->lambda[0] = in_para->lambda[0];
	out_para->lambda[1] = in_para->lambda[1];
	out_para->lambda[2] = in_para->lambda[2];

	out_para->dist_chroma_weight[0] = in_para->dist_chroma_weight[0];
	out_para->dist_chroma_weight[1] = in_para->dist_chroma_weight[1];
	out_para->x_last_ctu = in_para->x_last_ctu;
	out_para->y_last_ctu = in_para->y_last_ctu;
#endif


	out_para->CtrPosFME[0][0][0] = in_para->CtrPosFME[0][0][0];
	out_para->CtrPosFME[0][0][1] = in_para->CtrPosFME[0][0][1];
	out_para->CtrPosFME[0][1][0] = in_para->CtrPosFME[0][1][0];
	out_para->CtrPosFME[0][1][1] = in_para->CtrPosFME[0][1][1];
	out_para->CtrPosFME[1][0][0] = in_para->CtrPosFME[1][0][0];
	out_para->CtrPosFME[1][0][1] = in_para->CtrPosFME[1][0][1];
	out_para->CtrPosFME[1][1][0] = in_para->CtrPosFME[1][1][0];
	out_para->CtrPosFME[1][1][1] = in_para->CtrPosFME[1][1][1];

	out_para->CtrPos2MD[0][0][0] = in_para->CtrPos2MD[0][0][0];
	out_para->CtrPos2MD[0][0][1] = in_para->CtrPos2MD[0][0][1];
	out_para->CtrPos2MD[0][1][0] = in_para->CtrPos2MD[0][1][0];
	out_para->CtrPos2MD[0][1][1] = in_para->CtrPos2MD[0][1][1];
	out_para->CtrPos2MD[1][0][0] = in_para->CtrPos2MD[1][0][0];
	out_para->CtrPos2MD[1][0][1] = in_para->CtrPos2MD[1][0][1];
	out_para->CtrPos2MD[1][1][0] = in_para->CtrPos2MD[1][1][0];
	out_para->CtrPos2MD[1][1][1] = in_para->CtrPos2MD[1][1][1];

	pel u_org_reg[32];
	pel v_org_reg[32];
#pragma HLS ARRAY_PARTITION variable=u_org_reg complete dim=1
#pragma HLS ARRAY_PARTITION variable=v_org_reg complete dim=1

	//copy to fme add by xgq
//for (int j = 0; j < 32; j++)
//	{
//#pragma HLS PIPELINE
//		for (int k = 0; k < 32; k++)
//		{
//#pragma HLS UNROLL
//			out_para->p_fenc_LCU_U[j][k] = in_para->p_fenc_LCU_U[j][k];
//			out_para->p_fenc_LCU_V[j][k] = in_para->p_fenc_LCU_V[j][k];
//		}
//	}
	for (int j = 0; j < 32; j++)
	{
#pragma HLS PIPELINE
		for(int k = 0; k < 32; k++)
		{
#pragma HLS UNROLL
			u_org_reg[k] = in_para->p_fenc_LCU_U[j][k];
			v_org_reg[k] = in_para->p_fenc_LCU_V[j][k];
		}

		for (int i = 0; i < 32; i++)
		{
#pragma HLS UNROLL
		//	out_para->p_fenc_LCU_UV[j * 64 + i] = in_para->u_org[j][i];
		//	out_para->p_fenc_LCU_UV[j * 64 + i + 32] = in_para->v_org[j][i];
			out_para->p_fenc_LCU_U[j][i] = u_org_reg[i];
			out_para->p_fenc_LCU_V[j][i] = v_org_reg[i];
		}
	}
	for (int j = 0; j < 64; j++)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < 64; i++)
		{
#pragma HLS UNROLL
#if ORR_T
			out_para->p_fenc_LCU_Y[j][i] = in_para->p_fenc_LCU_Y[i][j];
#else
			out_para->p_fenc_LCU_Y[j][i] = in_para->p_fenc_LCU_Y[j][i];
#endif
		}
	}
}

//void set_CtrPos_SW88x88(U8 sw_tl_x, U8 sw_tl_y, U7 CtrPos_SW88x88[RANGE_NUM][DIR_DIM])
//{
//	if (sw_tl_x < 4)
//	{
//		CtrPos_SW88x88[RANGE_MIN][MV_X] = 4 - sw_tl_x;
//	}
//	else
//	{
//		CtrPos_SW88x88[RANGE_MIN][MV_X] = 0;
//	}
//	if (sw_tl_y < 4)
//	{
//		CtrPos_SW88x88[RANGE_MIN][MV_Y] = 4 - sw_tl_y;
//	}
//	else
//	{
//		CtrPos_SW88x88[RANGE_MIN][MV_Y] = 0;
//	}
//	if (sw_tl_x > 236)//if (sw_tl_x + 79 > 315)
//	{
//		CtrPos_SW88x88[RANGE_MAX][MV_X] = 323 - sw_tl_x;//CtrPos_SW88x88[RANGE_MAX][MV_X] = 87 - ((sw_tl_x + 79) - 315);
//	}
//	else
//	{
//		CtrPos_SW88x88[RANGE_MAX][MV_X] = 87;
//	}
//	if (sw_tl_y + 79 > 315)
//	{
//		CtrPos_SW88x88[RANGE_MAX][MV_Y] = 323 - sw_tl_y;//CtrPos_SW88x88[RANGE_MAX][MV_Y] = 87 - ((sw_tl_y + 79) - 315);
//	}
//	else
//	{
//		CtrPos_SW88x88[RANGE_MAX][MV_Y] = 87;
//	}
//}

void set_CtrPos_SW88x88(U8 sw_tl_x, U8 sw_tl_y, U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM])
{
#if FME_PRAGMA_ON
//#pragma HLS ARRAY_PARTITION variable=CtrPos_SW88x88 complete dim=0
#pragma HLS INLINE OFF
#endif
	if (sw_tl_x < 4)
	{
		CtrPos_SW88x88[RANGE_MIN][MV_X] = 4 - sw_tl_x;
	}
	else
	{
		CtrPos_SW88x88[RANGE_MIN][MV_X] = 0;
	}
	if (sw_tl_y < 4)
	{
		CtrPos_SW88x88[RANGE_MIN][MV_Y] = 4 - sw_tl_y;
	}
	else
	{
		CtrPos_SW88x88[RANGE_MIN][MV_Y] = 0;
	}
	if (sw_tl_x + 79 > (SWW - 1 - FENC_PADDING))//if (sw_tl_x > 236)//if (sw_tl_x + 79 > 315)
	{
		CtrPos_SW88x88[RANGE_MAX][MV_X] = (SWW - 1 + FENC_PADDING) - sw_tl_x;//CtrPos_SW88x88[RANGE_MAX][MV_X] = 323 - sw_tl_x;//CtrPos_SW88x88[RANGE_MAX][MV_X] = 87 - ((sw_tl_x + 79) - 315);		
	}
	else
	{
		CtrPos_SW88x88[RANGE_MAX][MV_X] = 87;
	}
	if (sw_tl_y + 79 > (SWW - 1 - FENC_PADDING))
	{
		CtrPos_SW88x88[RANGE_MAX][MV_Y] = (SWW - 1 + FENC_PADDING) - sw_tl_y;//CtrPos_SW88x88[RANGE_MAX][MV_Y] = 323 - sw_tl_y;//CtrPos_SW88x88[RANGE_MAX][MV_Y] = 87 - ((sw_tl_y + 79) - 315);
	}
	else
	{
		CtrPos_SW88x88[RANGE_MAX][MV_Y] = 87;
	}
}

void orgdatacopy(pel OrgY_2d[64][64], pel OrgY_2d1[64][64], pel OrgY_2d2[64][64], pel OrgY_2d3[64][64],
	pel OrgY_2d4[64][64], pel OrgY_2d5[64][64])
{
//#pragma HLS ARRAY_PARTITION variable=OrgY_2d complete dim=2
#pragma HLS ARRAY_PARTITION variable=OrgY_2d1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=OrgY_2d2 complete dim=2
#pragma HLS ARRAY_PARTITION variable=OrgY_2d3 complete dim=2
#pragma HLS ARRAY_PARTITION variable=OrgY_2d4 complete dim=2
#pragma HLS ARRAY_PARTITION variable=OrgY_2d5 complete dim=2
	int i, j;
	for (i = 0; i < 64; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		for (j = 0; j < 64; j++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
#if ORR_T
			OrgY_2d1[i][j] = OrgY_2d[j][i];
			OrgY_2d2[i][j] = OrgY_2d[j][i];
			OrgY_2d3[i][j] = OrgY_2d[j][i];
			OrgY_2d4[i][j] = OrgY_2d[j][i];
			OrgY_2d5[i][j] = OrgY_2d[j][i];
#else
			OrgY_2d1[i][j] = OrgY_2d[i][j];
			OrgY_2d2[i][j] = OrgY_2d[i][j];
			OrgY_2d3[i][j] = OrgY_2d[i][j];
			OrgY_2d4[i][j] = OrgY_2d[i][j];
			OrgY_2d5[i][j] = OrgY_2d[i][j];
#endif
		}
	}
}


//void refdatacopy(pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE], pel RefWindowLCU_2d1[88][88], pel RefWindowLCU_2d2[88][88], pel RefWindowLCU_2d3[88][88],
//	pel RefWindowLCU_2d4[88][88], pel RefWindowLCU_2d5[88][88]
//	,S14 offset_fme_sww_h[MV_D])
//{
//	int i, j;
//	for (i = 0; i < 88; i++)
//	{
//#if FME_PRAGMA_ON
//#pragma HLS PIPELINE
//#endif
//		for (j = 0; j < 88; j++)
//		{
//#if FME_PRAGMA_ON		
//#pragma HLS UNROLL
//#endif
//
//
//		//RefWindowLCU_2d1[i][j] = RefWindowLCU_2d[i][j];
//		//RefWindowLCU_2d2[i][j] = RefWindowLCU_2d[i][j];
//		//RefWindowLCU_2d3[i][j] = RefWindowLCU_2d[i][j];
//		//RefWindowLCU_2d4[i][j] = RefWindowLCU_2d[i][j];
//		//RefWindowLCU_2d5[i][j] = RefWindowLCU_2d[i][j];
//		//refwin_md[i][j] = RefWindowLCU_2d[i][j];
//
//			RefWindowLCU_2d1[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
//			RefWindowLCU_2d2[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
//			RefWindowLCU_2d3[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
//			RefWindowLCU_2d4[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
//			RefWindowLCU_2d5[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
//		}
//	}
//}
void refdatacopy(pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE], pel RefWindowLCU_2d1[88][88], pel RefWindowLCU_2d2[88][88], pel RefWindowLCU_2d3[88][88],
	pel RefWindowLCU_2d4[88][88], pel RefWindowLCU_2d5[88][88]
	, S14 offset_fme_sww_h[MV_D])
{
//#pragma HLS array_partition variable=RefWindowLCU_2d complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d1 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d2 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d3 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d4 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d5 complete dim=2

	int i, j;

	for (i = 0; i < 88; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		for (j = 0; j < 88; j++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			RefWindowLCU_2d1[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
			RefWindowLCU_2d2[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
			RefWindowLCU_2d3[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
			RefWindowLCU_2d4[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
			RefWindowLCU_2d5[i][j] = RefWindowLCU_2d[i + offset_fme_sww_h[1]][j + offset_fme_sww_h[0]];
		}
	}

}



void datacopy(pel OrgY_2d[64][64], pel OrgY_2d1[64][64], pel OrgY_2d2[64][64], pel OrgY_2d3[64][64], 
	pel OrgY_2d4[64][64], pel OrgY_2d5[64][64],
	pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE], pel RefWindowLCU_2d1[88][88], pel RefWindowLCU_2d2[88][88],
	pel RefWindowLCU_2d3[88][88], pel RefWindowLCU_2d4[88][88], pel RefWindowLCU_2d5[88][88]
	,S14 offset_fme_sww_h[MV_D])
{
#pragma HLS INLINE OFF
	orgdatacopy(OrgY_2d, OrgY_2d1, OrgY_2d2, OrgY_2d3, OrgY_2d4, OrgY_2d5);
	refdatacopy(RefWindowLCU_2d, RefWindowLCU_2d1, RefWindowLCU_2d2, RefWindowLCU_2d3, 
		RefWindowLCU_2d4, RefWindowLCU_2d5,
		offset_fme_sww_h);
}


void data_prepare_8x8_8x16(int i, U6 *org_offsetx, U6 *org_offsety, U8 *ref_offsetx, U8 *ref_offsety,
	U12 *x, U12 *y, U1 *ref_flag, U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	U12 ctu_x, U12 ctu_y, U7 *height, U7 *width, S14 *mvix, S14 *mviy, S9 sw_offset_x, S9 sw_offset_y,
	S14 mv8x8[8][8][2], S14 mv8x16[8][4][2], S14 *pmvx, S14 *pmvy, S14 pmv8x8[8][8][2], S14 pmv8x16[8][4][2]
#if ENABLE_BFRAME			
    ,U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR *me_mv_dir,
	U1 *lidx, S8 *cur_refi
#endif
)
{
	U6 cu_posx, cu_posy, x_scu, y_scu;
	S12 ref_left, ref_right, ref_top, ref_bottom;
	U1 flag_left, flag_right, flag_top, flag_bottom;
	if (i < 64)
	{
		x_scu = (i >> 3) << 1;
		y_scu = (i % 8) << 1;
		cu_posx = x_scu;
		cu_posy = y_scu;

		*height = 8;
		*width = 8;

		*mvix = mv8x8[i >> 3][i % 8][MV_X];
		*mviy = mv8x8[i >> 3][i % 8][MV_Y];
		*pmvx = pmv8x8[i >> 3][i % 8][MV_X];
		*pmvy = pmv8x8[i >> 3][i % 8][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block8x8MvDir[i >> 3][i % 8].lidx;
		*cur_refi = me_mv_dir->block8x8MvDir[i >> 3][i % 8].cur_refi;
#endif
	}
	else 
	{
		x_scu = ((i - 64) >> 3) << 1;
		y_scu = ((i - 64) % 8) << 1;
		cu_posx = ((i - 64) >> 3) << 1;
		cu_posy = (((i - 64) >> 1) % 4) << 2;

		*height = 16;
		*width = 8;

		*mvix = mv8x16[(i - 64) >> 3][((i - 64) >> 1) % 4][MV_X];
		*mviy = mv8x16[(i - 64) >> 3][((i - 64) >> 1) % 4][MV_Y];
		*pmvx = pmv8x16[(i - 64) >> 3][((i - 64) >> 1) % 4][MV_X];
		*pmvy = pmv8x16[(i - 64) >> 3][((i - 64) >> 1) % 4][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block8x16MvDir[(i - 64) >> 3][((i - 64) >> 1) % 4].lidx;
		*cur_refi = me_mv_dir->block8x16MvDir[(i - 64) >> 3][((i - 64) >> 1) % 4].cur_refi;
#endif
	}

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
	ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y - 4;
	ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x - 4;
	ref_bottom = ref_top + *height + 8;
	ref_right = ref_left + *width + 8;

	flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
	flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
	flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
	flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;

#if ENABLE_BFRAME
	}
	else
	{
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
	}
#endif


	if (flag_top && flag_left && flag_bottom && flag_right)
	{
		*ref_flag = 1;
	}
	else
	{
		*ref_flag = 0;
	}

	*x = (cu_posx << 2) + ctu_x;
	*y = (cu_posy << 2) + ctu_y;
	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);

	*org_offsetx = x_scu << 2;
	*org_offsety = y_scu << 2;

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
	*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
	*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
#if ENABLE_BFRAME
	}
	else
	{
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
	}
#endif
}

void data_prepare_8x32_8x64(int i, U6 *org_offsetx, U6 *org_offsety, U8 *ref_offsetx, U8 *ref_offsety,
	U12 *x, U12 *y, U1 *ref_flag, U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	U12 ctu_x, U12 ctu_y, U7 *height, U7 *width, S14 *mvix, S14 *mviy, S9 sw_offset_x, S9 sw_offset_y,
	S14 mv8x32[8][2][2], S14 mv8x64[8][1][2], S14 *pmvx, S14 *pmvy, S14 pmv8x32[8][2][2], S14 pmv8x64[8][1][2]
#if ENABLE_BFRAME			
	, U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR *me_mv_dir,
	U1 *lidx, S8 *cur_refi
#endif
)
{
	U6 cu_posx, cu_posy, x_scu, y_scu;
	S12 ref_left, ref_right, ref_top, ref_bottom;
	U1 flag_left, flag_right, flag_top, flag_bottom;
	if (i < 64)
	{
		x_scu = (i >> 3) << 1;
		y_scu = (i % 8) << 1;
		cu_posx = (i >> 3) << 1;
		cu_posy = ((i >> 2) % 2) << 3;

		*height = 32;
		*width = 8;

		*mvix = mv8x32[i >> 3][(i >> 2) % 2][MV_X];
		*mviy = mv8x32[i >> 3][(i >> 2) % 2][MV_Y];
		*pmvx = pmv8x32[i >> 3][(i >> 2) % 2][MV_X];
		*pmvy = pmv8x32[i >> 3][(i >> 2) % 2][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block8x32MvDir[i >> 3][(i >> 2) % 2].lidx;
		*cur_refi = me_mv_dir->block8x32MvDir[i >> 3][(i >> 2) % 2].cur_refi;
#endif
	}
	else
	{
		x_scu = ((i - 64) >> 3) << 1;
		y_scu = ((i - 64) % 8) << 1;
		cu_posx = ((i - 64) >> 3) << 1;
		cu_posy = 0;

		*height = 64;
		*width = 8;

		*mvix = mv8x64[(i - 64) >> 3][0][MV_X];
		*mviy = mv8x64[(i - 64) >> 3][0][MV_Y];
		*pmvx = pmv8x64[(i - 64) >> 3][0][MV_X];
		*pmvy = pmv8x64[(i - 64) >> 3][0][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block8x64MvDir[(i - 64) >> 3][0].lidx;
		*cur_refi = me_mv_dir->block8x64MvDir[(i - 64) >> 3][0].cur_refi;
#endif
	}

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;

#if ENABLE_BFRAME
	}
	else
	{
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
	}
#endif

	if (flag_top && flag_left && flag_bottom && flag_right)
	{
		*ref_flag = 1;
	}
	else
	{
		*ref_flag = 0;
	}

	*x = (cu_posx << 2) + ctu_x;
	*y = (cu_posy << 2) + ctu_y;
	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);

	*org_offsetx = x_scu << 2;
	*org_offsety = y_scu << 2;

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
#if ENABLE_BFRAME
	}
	else
	{
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
	}
#endif
}


void data_prepare_16x8_32x8_64x8_64x64(int i, U6* org_offsetx, U6* org_offsety, U8* ref_offsetx, U8* ref_offsety,
	U12* x, U12* y, U1* ref_flag, U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	U12 ctu_x, U12 ctu_y, U7* height, U7* width, S14* mvix, S14* mviy, S9 sw_offset_x, S9 sw_offset_y,
	S14 mv16x8[4][8][2], S14 mv32x8[2][8][2], S14 mv64x8[1][8][2], S14 mv64x64[1][1][2],
	S14* pmvx, S14* pmvy, S14 pmv16x8[4][8][2], S14 pmv32x8[2][8][2], S14 pmv64x8[1][8][2], S14 pmv64x64[1][1][2]
#if ENABLE_BFRAME			
	, U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR* me_mv_dir,
	U1* lidx, S8* cur_refi
#endif
)
{
	U6 cu_posx, cu_posy, x_scu, y_scu;
	S12 ref_left, ref_right, ref_top, ref_bottom;
	U1 flag_left, flag_right, flag_top, flag_bottom;
	if (i < 32)
	{
		x_scu = (i >> 3) << 2;
		y_scu = (i % 8) << 1;
		cu_posx = x_scu;
		cu_posy = y_scu;

		*height = 8;
		*width = 16;

		*mvix = mv16x8[i >> 3][i % 8][MV_X];
		*mviy = mv16x8[i >> 3][i % 8][MV_Y];
		*pmvx = pmv16x8[i >> 3][i % 8][MV_X];
		*pmvy = pmv16x8[i >> 3][i % 8][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block16x8MvDir[i >> 3][i % 8].lidx;
		*cur_refi = me_mv_dir->block16x8MvDir[i >> 3][i % 8].cur_refi;
#endif
	}
	else if (i < 64)
	{
		x_scu = ((i - 32) % 4) << 2;
		y_scu = ((i - 32) >> 2) << 1;
		cu_posx = (((i - 32) >> 1) % 2) << 3;
		cu_posy = ((i - 32) >> 2) << 1;

		*height = 8;
		*width = 32;

		*mvix = mv32x8[((i - 32) >> 1) % 2][(i - 32) >> 2][MV_X];
		*mviy = mv32x8[((i - 32) >> 1) % 2][(i - 32) >> 2][MV_Y];
		*pmvx = pmv32x8[((i - 32) >> 1) % 2][(i - 32) >> 2][MV_X];
		*pmvy = pmv32x8[((i - 32) >> 1) % 2][(i - 32) >> 2][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block32x8MvDir[((i - 32) >> 1) % 2][(i - 32) >> 2].lidx;
		*cur_refi = me_mv_dir->block32x8MvDir[((i - 32) >> 1) % 2][(i - 32) >> 2].cur_refi;
#endif
	}
	else if (i < 96)
	{
		x_scu = ((i - 64) % 4) << 2;
		y_scu = ((i - 64) >> 2) << 1;
		cu_posx = 0;
		cu_posy = ((i - 64) >> 2) << 1;

		*height = 8;
		*width = 64;

		*mvix = mv64x8[0][(i - 64) >> 2][MV_X];
		*mviy = mv64x8[0][(i - 64) >> 2][MV_Y];
		*pmvx = pmv64x8[0][(i - 64) >> 2][MV_X];
		*pmvy = pmv64x8[0][(i - 64) >> 2][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block64x8MvDir[0][(i - 64) >> 2].lidx;
		*cur_refi = me_mv_dir->block64x8MvDir[0][(i - 64) >> 2].cur_refi;
#endif
	}
	else
	{
		x_scu = ((i - 96) % 4) << 2;
		y_scu = ((i - 96) >> 2) << 1;
		cu_posx = 0;
		cu_posy = 0;

		*height = 64;
		*width = 64;

		*mvix = mv64x64[0][0][MV_X];
		*mviy = mv64x64[0][0][MV_Y];
		*pmvx = pmv64x64[0][0][MV_X];
		*pmvy = pmv64x64[0][0][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block64x64MvDir[0][0].lidx;
		*cur_refi = me_mv_dir->block64x64MvDir[0][0].cur_refi;
#endif
	}

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;

#if ENABLE_BFRAME
	}
	else
	{
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
	}
#endif

	if (flag_top && flag_left && flag_bottom && flag_right)
	{
		*ref_flag = 1;
	}
	else
	{
		*ref_flag = 0;
	}

	*x = (cu_posx << 2) + ctu_x;
	*y = (cu_posy << 2) + ctu_y;
	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);

	*org_offsetx = x_scu << 2;
	*org_offsety = y_scu << 2;

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		* ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
#if ENABLE_BFRAME
	}
	else
	{
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
	}
#endif
}


void data_prepare_16x16_16x32_16x64_32x32(int i, U6 *org_offsetx, U6 *org_offsety, U8 *ref_offsetx, U8 *ref_offsety,
	U12 *x, U12 *y, U1 *ref_flag, U7 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	U12 ctu_x, U12 ctu_y, U7 *height, U7 *width, S14 *mvix, S14 *mviy, S9 sw_offset_x, S9 sw_offset_y,
	S14 mv16x16[4][4][2], S14 mv16x32[4][2][2], S14 mv16x64[4][1][2], S14 mv32x32[2][2][2],
	S14 *pmvx, S14 *pmvy, S14 pmv16x16[4][4][2], S14 pmv16x32[4][2][2], S14 pmv16x64[4][1][2], S14 pmv32x32[2][2][2]
#if ENABLE_BFRAME			
	, U7 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR *me_mv_dir,
	U1 *lidx, S8 *cur_refi
#endif
)
{
	U6 cu_posx, cu_posy, x_scu, y_scu;
	S12 ref_left, ref_right, ref_top, ref_bottom;
	U1 flag_left, flag_right, flag_top, flag_bottom;
	if (i < 32)
	{
		x_scu = (i >> 3) << 2;
		y_scu = (i % 8) << 1;
		cu_posx = x_scu;
		cu_posy = ((i >> 1) % 4) << 2;

		*height = 16;
		*width = 16;

		*mvix = mv16x16[i >> 3][(i >> 1) % 4][MV_X];
		*mviy = mv16x16[i >> 3][(i >> 1) % 4][MV_Y];
		*pmvx = pmv16x16[i >> 3][(i >> 1) % 4][MV_X];
		*pmvy = pmv16x16[i >> 3][(i >> 1) % 4][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block16x16MvDir[i >> 3][(i >> 1) % 4].lidx;
		*cur_refi = me_mv_dir->block16x16MvDir[i >> 3][(i >> 1) % 4].cur_refi;
#endif
	}
	else if (i < 64)
	{
		x_scu = ((i - 32) >> 3) << 2;
		y_scu = ((i - 32) % 8) << 1;
		cu_posx = x_scu;
		cu_posy = (((i - 32) / 4) % 2) << 3;

		*height = 32;
		*width = 16;

		*mvix = mv16x32[(i - 32) >> 3][((i - 32) / 4) % 2][MV_X];
		*mviy = mv16x32[(i - 32) >> 3][((i - 32) / 4) % 2][MV_Y];
		*pmvx = pmv16x32[(i - 32) >> 3][((i - 32) / 4) % 2][MV_X];
		*pmvy = pmv16x32[(i - 32) >> 3][((i - 32) / 4) % 2][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block16x32MvDir[(i - 32) >> 3][((i - 32) / 4) % 2].lidx;
		*cur_refi = me_mv_dir->block16x32MvDir[(i - 32) >> 3][((i - 32) / 4) % 2].cur_refi;
#endif
	}
	else if (i < 96)
	{
		x_scu = ((i - 64) >> 3) << 2;
		y_scu = ((i - 64) % 8) << 1;
		cu_posx = x_scu;
		cu_posy = 0;

		*height = 64;
		*width = 16;

		*mvix = mv16x64[(i - 64) >> 3][0][MV_X];
		*mviy = mv16x64[(i - 64) >> 3][0][MV_Y];
		*pmvx = pmv16x64[(i - 64) >> 3][0][MV_X];
		*pmvy = pmv16x64[(i - 64) >> 3][0][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block16x64MvDir[(i - 64) >> 3][0].lidx;
		*cur_refi = me_mv_dir->block16x64MvDir[(i - 64) >> 3][0].cur_refi;
#endif
	}
	else
	{
		x_scu = ((i - 96) % 2 << 2) + (((i - 96) >> 4) << 3);
		y_scu = (((i - 96) % 16) >> 1) << 1;
		cu_posx = ((i - 96) >> 4) << 3;
		cu_posy = (((i - 96) >> 3) % 2) << 3;

		*height = 32;
		*width = 32;

		*mvix = mv32x32[(i - 96) >> 4][(((i - 96) >> 3) % 2)][MV_X];
		*mviy = mv32x32[(i - 96) >> 4][(((i - 96) >> 3) % 2)][MV_Y];
		*pmvx = pmv32x32[(i - 96) >> 4][(((i - 96) >> 3) % 2)][MV_X];
		*pmvy = pmv32x32[(i - 96) >> 4][(((i - 96) >> 3) % 2)][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block32x32MvDir[(i - 96) >> 4][(((i - 96) >> 3) % 2)].lidx;
		*cur_refi = me_mv_dir->block32x32MvDir[(i - 96) >> 4][(((i - 96) >> 3) % 2)].cur_refi;
#endif
	}

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;

#if ENABLE_BFRAME
	}
	else
	{
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
	}
#endif

	if (flag_top && flag_left && flag_bottom && flag_right)
	{
		*ref_flag = 1;
	}
	else
	{
		*ref_flag = 0;
	}

	*x = (cu_posx << 2) + ctu_x;
	*y = (cu_posy << 2) + ctu_y;
	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);

	*org_offsetx = x_scu << 2;
	*org_offsety = y_scu << 2;

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
#if ENABLE_BFRAME
	}
	else
	{
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
	}
#endif
}

void data_prepare_32x16_64x16_32x64_64x32(int i, U6 *org_offsetx, U6 *org_offsety, U8 *ref_offsetx, U8 *ref_offsety,
	U12 *x, U12 *y, U1 *ref_flag, U7 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	U12 ctu_x, U12 ctu_y, U7 *height, U7 *width, S14 *mvix, S14 *mviy, S9 sw_offset_x, S9 sw_offset_y,
	S14 mv32x16[2][4][2], S14 mv64x16[1][4][2], S14 mv32x64[2][1][2], S14 mv64x32[1][2][2],
	S14 *pmvx, S14 *pmvy, S14 pmv32x16[2][4][2], S14 pmv64x16[1][4][2], S14 pmv32x64[2][1][2], S14 pmv64x32[1][2][2]
#if ENABLE_BFRAME			
	, U7 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR *me_mv_dir,
	U1 *lidx, S8 *cur_refi
#endif
)
{
	U6 cu_posx, cu_posy, x_scu, y_scu;
	S12 ref_left, ref_right, ref_top, ref_bottom;
	U1 flag_left, flag_right, flag_top, flag_bottom;
	if (i < 32)
	{
		x_scu = ((i % 2) << 2) + ((i >> 4) << 3);
		y_scu = ((i % 16) >> 1) << 1;
		cu_posx = (i >> 4) << 3;
		cu_posy = ((i >> 2) % 4) << 2;

		*height = 16;
		*width = 32;

		*mvix = mv32x16[i >> 4][(i >> 2) % 4][MV_X];
		*mviy = mv32x16[i >> 4][(i >> 2) % 4][MV_Y];
		*pmvx = pmv32x16[i >> 4][(i >> 2) % 4][MV_X];
		*pmvy = pmv32x16[i >> 4][(i >> 2) % 4][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block32x16MvDir[i >> 4][(i >> 2) % 4].lidx;
		*cur_refi = me_mv_dir->block32x16MvDir[i >> 4][(i >> 2) % 4].cur_refi;
#endif
	}
	else if (i < 64)
	{
		x_scu = ((i - 32) % 4) << 2;
		y_scu = ((i - 32) >> 2) << 1;
		cu_posx = 0;
		cu_posy = ((i - 32) >> 3) << 2;

		*height = 16;
		*width = 64;

		*mvix = mv64x16[0][(i - 32) >> 3][MV_X];
		*mviy = mv64x16[0][(i - 32) >> 3][MV_Y];
		*pmvx = pmv64x16[0][(i - 32) >> 3][MV_X];
		*pmvy = pmv64x16[0][(i - 32) >> 3][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block64x16MvDir[0][(i - 32) >> 3].lidx;
		*cur_refi = me_mv_dir->block64x16MvDir[0][(i - 32) >> 3].cur_refi;
#endif
	}
	else if (i < 96)
	{
		x_scu = ((i - 64) >> 3) << 2;
		y_scu = ((i - 64) % 8) << 1;
		cu_posx = ((i - 64) >> 4) << 3;
		cu_posy = 0;

		*height = 64;
		*width = 32;

		*mvix = mv32x64[(i - 64) >> 4][0][MV_X];
		*mviy = mv32x64[(i - 64) >> 4][0][MV_Y];
		*pmvx = pmv32x64[(i - 64) >> 4][0][MV_X];
		*pmvy = pmv32x64[(i - 64) >> 4][0][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block32x64MvDir[(i - 64) >> 4][0].lidx;
		*cur_refi = me_mv_dir->block32x64MvDir[(i - 64) >> 4][0].cur_refi;
#endif
	}
	else
	{
		x_scu = ((i - 96) % 4) << 2;
		y_scu = ((i - 96) >> 2) << 1;
		cu_posx = 0;
		cu_posy = ((i - 96) >> 4) << 3;

		*height = 32;
		*width = 64;

		*mvix = mv64x32[0][(i - 96) >> 4][MV_X];
		*mviy = mv64x32[0][(i - 96) >> 4][MV_Y];
		*pmvx = pmv64x32[0][(i - 96) >> 4][MV_X];
		*pmvy = pmv64x32[0][(i - 96) >> 4][MV_Y];
#if ENABLE_BFRAME
		*lidx = me_mv_dir->block64x32MvDir[0][(i - 96) >> 4].lidx;
		*cur_refi = me_mv_dir->block64x32MvDir[0][(i - 96) >> 4].cur_refi;
#endif
	}

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;

#if ENABLE_BFRAME
	}
	else
	{
		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 - 4;
		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 - 4;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
	}
#endif

	if (flag_top && flag_left && flag_bottom && flag_right)
	{
		*ref_flag = 1;
	}
	else
	{
		*ref_flag = 0;
	}

	*x = (cu_posx << 2) + ctu_x;
	*y = (cu_posy << 2) + ctu_y;
	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);

	*org_offsetx = x_scu << 2;
	*org_offsety = y_scu << 2;

#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
#if ENABLE_BFRAME
	}
	else
	{
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
	}
#endif
}

void data_prepare_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32(
	int i, U6* org_offsetx, U6* org_offsety, U8* ref_offsetx, U8* ref_offsety, U12* x, U12* y, U1* ref_flag, U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	U12 ctu_x, U12 ctu_y, U7* height, U7* width, S14* mvix, S14* mviy, S9 sw_offset_x, S9 sw_offset_y,
	S14 mv16x16[4][4][2], S14 mv32x32[2][2][2],
	S14 mv16x32[4][2][2], S14 mv32x16[2][4][2],
	S14 mv16x64[4][1][2], S14 mv64x16[1][4][2],
	S14 mv32x64[2][1][2], S14 mv64x32[1][2][2],
	S14* pmvx, S14* pmvy,
	S14 pmv16x16[4][4][2], S14 pmv32x32[2][2][2],
	S14 pmv16x32[4][2][2], S14 pmv32x16[2][4][2],
	S14 pmv16x64[4][1][2], S14 pmv64x16[1][4][2],
	S14 pmv32x64[2][1][2], S14 pmv64x32[1][2][2]
#if ENABLE_BFRAME			
	, U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR* me_mv_dir,
	U1* lidx, S8* cur_refi
#endif
)
{
	U6 cu_posx, cu_posy, x_scu, y_scu;
	S12 ref_left, ref_right, ref_top, ref_bottom;
	U1 flag_left, flag_right, flag_top, flag_bottom;
	if (i < 16)
	{
		x_scu = (i >> 2) << 2;
		y_scu = (i % 4) << 2;
		cu_posx = x_scu;
		cu_posy = y_scu;

		*height = 16;
		*width = 16;

		*mvix = mv16x16[i >> 2][i % 4][MV_X];
		*mviy = mv16x16[i >> 2][i % 4][MV_Y];
		*pmvx = pmv16x16[i >> 2][i % 4][MV_X];
		*pmvy = pmv16x16[i >> 2][i % 4][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block16x16MvDir[i >> 2][i % 4].lidx;
		*cur_refi = me_mv_dir->block16x16MvDir[i >> 2][i % 4].cur_refi;
#endif

	}
	else if (i < 32)
	{
		x_scu = (((i - 16) >> 3) << 3) + ((((i - 16) >> 1) % 2) << 2);
		y_scu = ((((i - 16) >> 2) % 2) << 3) + (((i - 16) % 2) << 2);
		cu_posx = ((i - 16) >> 3) << 3;
		cu_posy = (((i - 16) >> 2) % 2) << 3;

		*height = 32;
		*width = 32;

		*mvix = mv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_X];
		*mviy = mv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_Y];
		*pmvx = pmv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_X];
		*pmvy = pmv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block32x32MvDir[(i - 16) >> 3][(((i - 16) >> 2) % 2)].lidx;
		*cur_refi = me_mv_dir->block32x32MvDir[(i - 16) >> 3][(((i - 16) >> 2) % 2)].cur_refi;
#endif

	}
	else if (i < 48)
	{
		x_scu = ((i - 32) >> 2) << 2;
		y_scu = ((i - 32) % 4) << 2;
		cu_posx = ((i - 32) >> 2) << 2;
		cu_posy = (((i - 32) >> 1) % 2) << 3;

		*height = 32;
		*width = 16;

		*mvix = mv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_X];
		*mviy = mv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_Y];
		*pmvx = pmv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_X];
		*pmvy = pmv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block16x32MvDir[(i - 32) >> 2][((i - 32) >> 1) % 2].lidx;
		*cur_refi = me_mv_dir->block16x32MvDir[(i - 32) >> 2][((i - 32) >> 1) % 2].cur_refi;
#endif

	}
	else if (i < 64)
	{
		x_scu = (((i - 48) >> 3) << 3) + (((i - 48) % 2) << 2);
		y_scu = (((i - 48) % 8) >> 1) << 2;
		cu_posx = ((i - 48) >> 3) << 3;
		cu_posy = (((i - 48) % 8) >> 1) << 2;

		*height = 16;
		*width = 32;

		*mvix = mv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_X];
		*mviy = mv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_Y];
		*pmvx = pmv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_X];
		*pmvy = pmv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block32x16MvDir[(i - 48) >> 3][((i - 48) % 8) >> 1].lidx;
		*cur_refi = me_mv_dir->block32x16MvDir[(i - 48) >> 3][((i - 48) % 8) >> 1].cur_refi;
#endif

	}
	else if (i < 80)
	{
		x_scu = ((i - 64) >> 2) << 2;
		y_scu = ((i - 64) % 4) << 2;
		cu_posx = ((i - 64) >> 2) << 2;
		cu_posy = 0;

		*height = 64;
		*width = 16;

		*mvix = mv16x64[(i - 64) >> 2][0][MV_X];
		*mviy = mv16x64[(i - 64) >> 2][0][MV_Y];
		*pmvx = pmv16x64[(i - 64) >> 2][0][MV_X];
		*pmvy = pmv16x64[(i - 64) >> 2][0][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block16x64MvDir[(i - 64) >> 2][0].lidx;
		*cur_refi = me_mv_dir->block16x64MvDir[(i - 64) >> 2][0].cur_refi;
#endif

	}
	else if (i < 96)
	{
		x_scu = ((i - 80) % 4) << 2;
		y_scu = ((i - 80) >> 2) << 2;;
		cu_posx = 0;
		cu_posy = ((i - 80) >> 2) << 2;

		*height = 16;
		*width = 64;

		*mvix = mv64x16[0][(i - 80) >> 2][MV_X];
		*mviy = mv64x16[0][(i - 80) >> 2][MV_Y];
		*pmvx = pmv64x16[0][(i - 80) >> 2][MV_X];
		*pmvy = pmv64x16[0][(i - 80) >> 2][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block64x16MvDir[0][(i - 80) >> 2].lidx;
		*cur_refi = me_mv_dir->block64x16MvDir[0][(i - 80) >> 2].cur_refi;
#endif

	}
	else if (i < 112)
	{
		x_scu = ((i - 96) >> 2) << 2;
		y_scu = ((i - 96) % 4) << 2;
		cu_posx = ((i - 96) >> 3) << 3;
		cu_posy = 0;

		*height = 64;
		*width = 32;

		*mvix = mv32x64[(i - 96) >> 3][0][MV_X];
		*mviy = mv32x64[(i - 96) >> 3][0][MV_Y];
		*pmvx = pmv32x64[(i - 96) >> 3][0][MV_X];
		*pmvy = pmv32x64[(i - 96) >> 3][0][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block32x64MvDir[(i - 96) >> 3][0].lidx;
		*cur_refi = me_mv_dir->block32x64MvDir[(i - 96) >> 3][0].cur_refi;
#endif

	}
	else
	{
		x_scu = (((i - 112) % 8) >> 1) << 2;
		y_scu = (((i - 112) >> 3) << 3) + (((i - 112) % 2) << 2);
		cu_posx = 0;
		cu_posy = ((i - 112) >> 3) << 3;

		*height = 32;
		*width = 64;

		*mvix = mv64x32[0][(i - 112) >> 3][MV_X];
		*mviy = mv64x32[0][(i - 112) >> 3][MV_Y];
		*pmvx = pmv64x32[0][(i - 112) >> 3][MV_X];
		*pmvy = pmv64x32[0][(i - 112) >> 3][MV_Y];
#if ENABLE_BFRAME
		* lidx = me_mv_dir->block64x32MvDir[0][(i - 112) >> 3].lidx;
		*cur_refi = me_mv_dir->block64x32MvDir[0][(i - 112) >> 3].cur_refi;
#endif

	}
#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
	ref_top = 8 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y ;
	ref_left = 8 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x ;
	ref_bottom = ref_top + *height + 8;
	ref_right = ref_left + *width + 8;

	flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
	flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
	flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
	flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;
#if ENABLE_BFRAME
	}
	else
	{
		ref_top = 8 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 ;
		ref_left = 8 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 ;
		ref_bottom = ref_top + *height + 8;
		ref_right = ref_left + *width + 8;

		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
	}
#endif

	if (flag_top && flag_left && flag_bottom && flag_right)
	{
		*ref_flag = 1;
	}
	else
	{
		*ref_flag = 0;
	}

	*x = (cu_posx << 2) + ctu_x;
	*y = (cu_posy << 2) + ctu_y;
	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);

	*org_offsetx = x_scu << 2;
	*org_offsety = y_scu << 2;
#if ENABLE_BFRAME
	if (*lidx == 0 && *cur_refi == 0)
	{
#endif
	*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
	*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;

#if ENABLE_BFRAME
	}
	else
	{
		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
	}
#endif


}

//void data_prepare_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32_64x64(
//	int i, U6 *org_offsetx, U6 *org_offsety, U8 *ref_offsetx, U8 *ref_offsety, U12 *x, U12 *y, U1 *ref_flag, U7 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
//	U12 ctu_x, U12 ctu_y, U7 *height, U7 *width, S14 *mvix, S14 *mviy, S9 sw_offset_x, S9 sw_offset_y,
//	S14 mv16x16[4][4][2], S14 mv32x32[2][2][2],
//	S14 mv16x32[4][2][2], S14 mv32x16[2][4][2],
//	S14 mv16x64[4][1][2], S14 mv64x16[1][4][2],
//	S14 mv32x64[2][1][2], S14 mv64x32[1][2][2], S14 mv64x64[1][1][2],
//	S14 *pmvx, S14 *pmvy,
//	S14 pmv16x16[4][4][2], S14 pmv32x32[2][2][2],
//	S14 pmv16x32[4][2][2], S14 pmv32x16[2][4][2],
//	S14 pmv16x64[4][1][2], S14 pmv64x16[1][4][2],
//	S14 pmv32x64[2][1][2], S14 pmv64x32[1][2][2],S14 pmv64x64[1][1][2]
//#if ENABLE_BFRAME			
//	, U7 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR* me_mv_dir,
//	U1* lidx, S8* cur_refi
//#endif
//)
//{
//	U6 cu_posx, cu_posy, x_scu, y_scu;
//	S12 ref_left, ref_right, ref_top, ref_bottom;
//	U1 flag_left, flag_right, flag_top, flag_bottom;
//	if (i < 16)
//	{
//		x_scu = (i >> 2) << 2;
//		y_scu = (i % 4) << 2;
//		cu_posx = x_scu;
//		cu_posy = y_scu;
//
//		*height = 16;
//		*width = 16;
//
//		*mvix = mv16x16[i >> 2][i % 4][MV_X];
//		*mviy = mv16x16[i >> 2][i % 4][MV_Y];
//		*pmvx = pmv16x16[i >> 2][i % 4][MV_X];
//		*pmvy = pmv16x16[i >> 2][i % 4][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block16x16MvDir[i >> 2][i % 4].lidx;
//		*cur_refi = me_mv_dir->block16x16MvDir[i >> 2][i % 4].cur_refi;
//#endif
//	}
//	else if (i < 32)
//	{
//		x_scu = (((i - 16) >> 3) << 3) + ((((i - 16) >> 1) % 2) << 2);
//		y_scu = ((((i - 16) >> 2) % 2) << 3) + (((i - 16) % 2) << 2);
//		cu_posx = ((i - 16) >> 3) << 3;
//		cu_posy = (((i - 16) >> 2) % 2) << 3;
//
//		*height = 32;
//		*width = 32;
//
//		*mvix = mv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_X];
//		*mviy = mv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_Y];
//		*pmvx = pmv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_X];
//		*pmvy = pmv32x32[(i - 16) >> 3][((i - 16) >> 2) % 2][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block32x32MvDir[(i - 16) >> 3][(((i - 16) >> 2) % 2)].lidx;
//		*cur_refi = me_mv_dir->block32x32MvDir[(i - 16) >> 3][(((i - 16) >> 2) % 2)].cur_refi;
//#endif
//	}
//	else if (i < 48)
//	{
//		x_scu = ((i - 32) >> 2) << 2;
//		y_scu = ((i - 32) % 4) << 2;
//		cu_posx = ((i - 32) >> 2) << 2;
//		cu_posy = (((i - 32) >> 1) % 2) << 3;
//
//		*height = 32;
//		*width = 16;
//
//		*mvix = mv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_X];
//		*mviy = mv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_Y];
//		*pmvx = pmv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_X];
//		*pmvy = pmv16x32[(i - 32) >> 2][((i - 32) >> 1) % 2][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block16x32MvDir[(i - 32) >> 2][((i - 32) >> 1) % 2].lidx;
//		*cur_refi = me_mv_dir->block16x32MvDir[(i - 32) >> 2][((i - 32) >> 1) % 2].cur_refi;
//#endif
//	}
//	else if (i < 64)
//	{
//		x_scu = (((i - 48) >> 3) << 3) + (((i - 48) % 2) << 2);
//		y_scu = (((i - 48) % 8) >> 1) << 2;
//		cu_posx = ((i - 48) >> 3) << 3;
//		cu_posy = (((i - 48) % 8) >> 1) << 2;
//
//		*height = 16;
//		*width = 32;
//
//		*mvix = mv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_X];
//		*mviy = mv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_Y];
//		*pmvx = pmv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_X];
//		*pmvy = pmv32x16[(i - 48) >> 3][((i - 48) % 8) >> 1][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block32x16MvDir[(i - 48) >> 3][((i - 48) % 8) >> 1].lidx;
//		*cur_refi = me_mv_dir->block32x16MvDir[(i - 48) >> 3][((i - 48) % 8) >> 1].cur_refi;
//#endif
//	}
//	else if (i < 80)
//	{
//		x_scu = ((i - 64) >> 2) << 2;
//		y_scu = ((i - 64) % 4) << 2;
//		cu_posx = ((i - 64) >> 2) << 2;
//		cu_posy = 0;
//
//		*height = 64;
//		*width = 16;
//
//		*mvix = mv16x64[(i - 64) >> 2][0][MV_X];
//		*mviy = mv16x64[(i - 64) >> 2][0][MV_Y];
//		*pmvx = pmv16x64[(i - 64) >> 2][0][MV_X];
//		*pmvy = pmv16x64[(i - 64) >> 2][0][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block16x64MvDir[(i - 64) >> 2][0].lidx;
//		*cur_refi = me_mv_dir->block16x64MvDir[(i - 64) >> 2][0].cur_refi;
//#endif
//	}
//	else if (i < 96)
//	{
//		x_scu = ((i - 80) % 4) << 2;
//		y_scu = ((i - 80) >> 2) << 2;;
//		cu_posx = 0;
//		cu_posy = ((i - 80) >> 2) << 2;
//
//		*height = 16;
//		*width = 64;
//
//		*mvix = mv64x16[0][(i - 80) >> 2][MV_X];
//		*mviy = mv64x16[0][(i - 80) >> 2][MV_Y];
//		*pmvx = pmv64x16[0][(i - 80) >> 2][MV_X];
//		*pmvy = pmv64x16[0][(i - 80) >> 2][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block64x16MvDir[0][(i - 80) >> 2].lidx;
//		*cur_refi = me_mv_dir->block64x16MvDir[0][(i - 80) >> 2].cur_refi;
//#endif
//	}
//	else if (i < 112)
//	{
//		x_scu = ((i - 96) >> 2) << 2;
//		y_scu = ((i - 96) % 4) << 2;
//		cu_posx = ((i - 96) >> 3) << 3;
//		cu_posy = 0;
//
//		*height = 64;
//		*width = 32;
//
//		*mvix = mv32x64[(i - 96) >> 3][0][MV_X];
//		*mviy = mv32x64[(i - 96) >> 3][0][MV_Y];
//		*pmvx = pmv32x64[(i - 96) >> 3][0][MV_X];
//		*pmvy = pmv32x64[(i - 96) >> 3][0][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block32x64MvDir[(i - 96) >> 3][0].lidx;
//		*cur_refi = me_mv_dir->block32x64MvDir[(i - 96) >> 3][0].cur_refi;
//#endif
//	}
//	else if(i < 128)
//	{
//		x_scu = (((i - 112) % 8) >> 1) << 2;
//		y_scu = (((i - 112) >> 3) << 3) + (((i - 112) % 2) << 2);
//		cu_posx = 0;
//		cu_posy = ((i - 112) >> 3) << 3;
//			
//		*height = 32;
//		*width = 64;
//
//		*mvix = mv64x32[0][(i - 112) >> 3][MV_X];
//		*mviy = mv64x32[0][(i - 112) >> 3][MV_Y];
//		*pmvx = pmv64x32[0][(i - 112) >> 3][MV_X];
//		*pmvy = pmv64x32[0][(i - 112) >> 3][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block64x32MvDir[0][(i - 112) >> 3].lidx;
//		*cur_refi = me_mv_dir->block64x32MvDir[0][(i - 112) >> 3].cur_refi;
//#endif
//	}
//	else
//	{
//		x_scu = ((i - 128) % 4) << 2;
//		y_scu = ((i - 128) >> 2) << 2;
//		cu_posx = 0;
//		cu_posy = 0;
//
//		*height = 64;
//		*width = 64;
//
//		*mvix = mv64x64[0][0][MV_X];
//		*mviy = mv64x64[0][0][MV_Y];
//		*pmvx = pmv64x64[0][0][MV_X];
//		*pmvy = pmv64x64[0][0][MV_Y];
//#if ENABLE_BFRAME
//		* lidx = me_mv_dir->block64x64MvDir[0][0].lidx;
//		*cur_refi = me_mv_dir->block64x64MvDir[0][0].cur_refi;
//#endif
//
//	}
//#if ENABLE_BFRAME
//	if (*lidx == 0 && *cur_refi == 0)
//	{
//#endif
//	ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y - 4;
//	ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x - 4;
//	ref_bottom = ref_top + *height + 8;
//	ref_right = ref_left + *width + 8;
//
//	flag_top = CtrPos_SW88x88[RANGE_MIN][MV_Y] <= ref_top;
//	flag_left = CtrPos_SW88x88[RANGE_MIN][MV_X] <= ref_left;
//	flag_bottom = CtrPos_SW88x88[RANGE_MAX][MV_Y] >= ref_bottom;
//	flag_right = CtrPos_SW88x88[RANGE_MAX][MV_X] >= ref_right;
//
//#if ENABLE_BFRAME
//	}
//	else
//	{
//		ref_top = 12 + (cu_posy << 2) + (*mviy >> 2) + sw_offset_y_1 - 4;
//		ref_left = 12 + (cu_posx << 2) + (*mvix >> 2) + sw_offset_x_1 - 4;
//		ref_bottom = ref_top + *height + 8;
//		ref_right = ref_left + *width + 8;
//
//		flag_top = CtrPos_SW88x88_1[RANGE_MIN][MV_Y] <= ref_top;
//		flag_left = CtrPos_SW88x88_1[RANGE_MIN][MV_X] <= ref_left;
//		flag_bottom = CtrPos_SW88x88_1[RANGE_MAX][MV_Y] >= ref_bottom;
//		flag_right = CtrPos_SW88x88_1[RANGE_MAX][MV_X] >= ref_right;
//	}
//#endif
//
//	if (flag_top && flag_left && flag_bottom && flag_right)
//	{
//		*ref_flag = 1;
//	}
//	else
//	{
//		*ref_flag = 0;
//	}
//
//	*x = (cu_posx << 2) + ctu_x;
//	*y = (cu_posy << 2) + ctu_y;
//	*mvix = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mvix);
//	*mviy = COM_CLIP3(COM_INT14_MIN, COM_INT14_MAX, *mviy);
//
//	*org_offsetx = x_scu << 2;
//	*org_offsety = y_scu << 2;
//
//#if ENABLE_BFRAME
//	if (*lidx == 0 && *cur_refi == 0)
//	{
//#endif
//	*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
//	*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
//
//#if ENABLE_BFRAME
//	}
//	else
//	{
//		*ref_offsetx = 8 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x_1;//12 + (x_scu << 2) + (*mvix >> 2) + sw_offset_x - 4;
//		*ref_offsety = 8 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y_1;//12 + (y_scu << 2) + (*mviy >> 2) + sw_offset_y - 4;
//	}
//#endif
//
//}


void copy_8x8org_1(pel org[8][8], pel OrgY_2d[64][64], U6 org_offsetx, U6 org_offsety)
{
//#pragma HLS INLINE OFF
	int h, w;
	for (h = 0; h < 8; h++)
	{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
		for (w = 0; w < 8; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			org[h][w] = OrgY_2d[h + org_offsety/8*8][w + org_offsetx/8*8];
		}
	}
}

void copy_8x8org_2(pel org[8][8], pel OrgY_2d[64][64], U6 org_offsetx, U6 org_offsety)
{
	//#pragma HLS INLINE OFF
	int h, w;
	for (h = 0; h < 8; h++)
	{
#if FME_PRAGMA_ON	
//#pragma HLS PIPELINE
#endif
		for (w = 0; w < 8; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif	
			org[h][w] = OrgY_2d[h + org_offsety/8*8][w + org_offsetx/8*8];
		}
	}
}

//void copy_16x8org_1(int i, pel org[8][16],pel org1[8][8], pel OrgY_2d[64][64], pel OrgY_2d1[64][64], U6 org_offsetx, U6 org_offsety)
//{
//#pragma HLS INLINE OFF
//	int h, w;
//	for (h = 0; h < 8; h++)
//	{
//#if FME_PRAGMA_ON	
////#pragma HLS PIPELINE
//#endif
//		if (i < 96) 
//		{
//			for (w = 0; w < 16; w++)
//			{
//#if FME_PRAGMA_ON		
//#pragma HLS UNROLL
//#endif
//				org[h][w] = OrgY_2d[h + org_offsety][w + org_offsetx];
//			}
//		}
//		else
//		{
//			for (w = 0; w < 8; w++)
//			{
//#if FME_PRAGMA_ON		
//#pragma HLS UNROLL
//#endif
//				org1[h][w] = OrgY_2d1[h + org_offsety][w + org_offsetx];
//			}
//		}
//	}
//}
void copy_16x8org_1(pel org[8][16], pel OrgY_2d[64][64], U6 org_offsetx, U6 org_offsety)
{
#pragma HLS INLINE OFF
	int h, w;
	for (h = 0; h < 8; h++)
	{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
		for (w = 0; w < 16; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			org[h][w] = OrgY_2d[h + org_offsety/8*8][w + org_offsetx/8*8];
		}
	}
}
void copy_16x8org_2(pel org[8][16], pel OrgY_2d[64][64], U6 org_offsetx, U6 org_offsety)
{
#pragma HLS INLINE OFF
	int h, w;
	for (h = 0; h < 8; h++)
	{
#if FME_PRAGMA_ON	
		//#pragma HLS PIPELINE
#endif
		for (w = 0; w < 16; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			org[h][w] = OrgY_2d[h + org_offsety][w + org_offsetx];
		}
	}
}

void copy_16x8org_3(pel org[8][16], pel OrgY_2d[64][64], U6 org_offsetx, U6 org_offsety)
{
#pragma HLS INLINE OFF
	int h, w;
	for (h = 0; h < 8; h++)
	{
#if FME_PRAGMA_ON	
		//#pragma HLS PIPELINE
#endif
		for (w = 0; w < 16; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			org[h][w] = OrgY_2d[h + org_offsety][w + org_offsetx];
		}
	}
}

void copy_16x16org(pel org[16][16], pel OrgY_2d[64][64], U6 org_offsetx, U6 org_offsety)
{
#pragma HLS INLINE OFF
	int h, w;
	for (h = 0; h < 16; h++)
	{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
		for (w = 0; w < 16; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			org[h][w] = OrgY_2d[h + org_offsety][w + org_offsetx];
		}
	}
}

void orgline_copy_16x16(int i, pel OrgY[16][16], pel org[16])
{
	int w;
	for (w = 0; w < 16; w++)
	{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
		org[w] = OrgY[i][w];
	}
}

void copy_16x16ref_1(pel ref[16][16], pel RefWindowLCU_2d[88][88], U8 ref_offsetx, U8 ref_offsety
#if ENABLE_BFRAME			
	, pel RefWindowLCU_2d_ref1[88][88], U1 lidx, S8 cur_refi
#endif	
)
{
//#pragma HLS INLINE OFF
	int h, w;
	pel RefWindowLCU_sub[88];
#if FME_PRAGMA_ON	
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_sub complete dim=1
#pragma HLS array_partition variable=RefWindowLCU_2d complete dim=2
#pragma HLS array_partition variable=ref complete dim=2
#endif	
#if ENABLE_BFRAME
#pragma HLS array_partition variable=RefWindowLCU_2d_ref1 complete dim=2
#endif
#if ENABLE_BFRAME	
	if (lidx == 0 && cur_refi == 0)
	{
#endif

		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE II=2
#endif	
			for(int k=0;k<88;k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d[h + ref_offsety][k];
			}

			for (w = 0; w < 16; w++)
			{
#if FME_PRAGMA_ON		
//#pragma HLS UNROLL
#endif
				//ref[h][w] = RefWindowLCU_2d[h + ref_offsety/8*8][w + ref_offsetx/8*8];
				ref[h][w] = RefWindowLCU_sub[w + ref_offsetx];
		
			}
		}

#if ENABLE_BFRAME	
	}
	else
	{
		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE II=2
#endif
			for(int k=0;k<88;k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d_ref1[h + ref_offsety][k];
			}

			for (w = 0; w < 16; w++)
			{
#if FME_PRAGMA_ON		
//#pragma HLS UNROLL
#endif
				ref[h][w] = RefWindowLCU_sub[w + ref_offsetx];
			}
		}
	}
#endif

}

void copy_16x16ref_2(pel ref[16][16], pel RefWindowLCU_2d[88][88], U8 ref_offsetx, U8 ref_offsety
#if ENABLE_BFRAME			
	, pel RefWindowLCU_2d_ref1[88][88], U1 lidx, S8 cur_refi
#endif	
)
{
	//#pragma HLS INLINE OFF
	int h, w;
	pel RefWindowLCU_sub[88];
#pragma HLS ARRAY_PARTITION variable=ref complete dim=2
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_sub complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d complete dim=2
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1 complete dim=2
#endif
#if ENABLE_BFRAME	
	if (lidx == 0 && cur_refi == 0)
	{
#endif

		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE II=2
#endif
			for(int k=0;k<88;k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d[h + ref_offsety][k];
			}

			for (w = 0; w < 16; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				//ref[h][w] = RefWindowLCU_2d[h + ref_offsety/8*8][w + ref_offsetx/8*8];
				ref[h][w] = RefWindowLCU_sub[w + ref_offsetx];
		
			}
		}

#if ENABLE_BFRAME	
	}
	else
	{
		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE II=2
#endif
			for(int k=0;k<88;k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d_ref1[h + ref_offsety][k];
			}

			for (w = 0; w < 16; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				//ref[h][w] = RefWindowLCU_2d[h + ref_offsety/8*8][w + ref_offsetx/8*8];
				ref[h][w] = RefWindowLCU_sub[w + ref_offsetx];
		
			}
		}
	}
#endif

}

void copy_24x16ref_1(pel ref[16][24], pel RefWindowLCU_2d[88][88], U8 ref_offsetx, U8 ref_offsety
#if ENABLE_BFRAME			
	, pel RefWindowLCU_2d_ref1[88][88], U1 lidx, S8 cur_refi
#endif	
)
{
#pragma HLS INLINE OFF
	int h, w;
	pel RefWindowLCU_sub[88];
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_sub complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d complete  dim=2
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1 complete  dim=2
#endif
#if ENABLE_BFRAME	
	if (lidx == 0 && cur_refi == 0)
	{
#endif

		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE II=2
#endif
			for(int k = 0; k < 88; k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d[h + ref_offsety][k];
			}

			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				ref[h][w] = RefWindowLCU_sub[w + ref_offsetx];
			}
		}

#if ENABLE_BFRAME	
	}
	else
	{
		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
			for(int k = 0; k < 88; k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d_ref1[h + ref_offsety][k];
			}
			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				//ref[h][w] = RefWindowLCU_2d_ref1[h + ref_offsety][w + ref_offsetx];
				ref[h][w] = RefWindowLCU_sub[w + ref_offsetx];
			}
		}
	}
#endif
}


void copy_24x16ref_2(pel ref[16][24], pel RefWindowLCU_2d[88][88], U8 ref_offsetx, U8 ref_offsety
#if ENABLE_BFRAME			
	, pel RefWindowLCU_2d_ref1[88][88], U1 lidx, S8 cur_refi
#endif	
)
{
#pragma HLS INLINE OFF
	int h, w;

#if ENABLE_BFRAME	
	if (lidx == 0 && cur_refi == 0)
	{
#endif

		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				ref[h][w] = RefWindowLCU_2d[h + ref_offsety][w + ref_offsetx];
			}
		}

#if ENABLE_BFRAME	
	}
	else
	{
		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				ref[h][w] = RefWindowLCU_2d_ref1[h + ref_offsety][w + ref_offsetx];
			}
		}
	}
#endif
}

void copy_24x16ref_3(pel ref[16][24], pel RefWindowLCU_2d[88][88], U8 ref_offsetx, U8 ref_offsety
#if ENABLE_BFRAME			
	, pel RefWindowLCU_2d_ref1[88][88], U1 lidx, S8 cur_refi
#endif	
)
{
#pragma HLS INLINE OFF
	int h, w;

#if ENABLE_BFRAME	
	if (lidx == 0 && cur_refi == 0)
	{
#endif

		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				ref[h][w] = RefWindowLCU_2d[h + ref_offsety][w + ref_offsetx];
			}
		}

#if ENABLE_BFRAME	
	}
	else
	{
		for (h = 0; h < 16; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				ref[h][w] = RefWindowLCU_2d_ref1[h + ref_offsety][w + ref_offsetx];
			}
		}
	}
#endif
}

void copy_24x24ref(pel ref0[12][24], pel ref1[12][24], pel RefWindowLCU_2d[88][88], U8 ref_offsetx, U8 ref_offsety
#if ENABLE_BFRAME			
	, pel RefWindowLCU_2d_ref0[88][88], pel RefWindowLCU_2d_ref1[88][88], U1 lidx, S8 cur_refi
#endif
)
{
#pragma HLS INLINE OFF
	int h, w;
	pel RefWindowLCU_sub[88];
	pel RefWindowLCU_sub1[88];
#if FME_PRAGMA_ON
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_sub complete dim=1
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_sub1 complete dim=1
//#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d complete dim=2
#pragma HLS ARRAY_PARTITION variable=ref0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ref1 complete dim=2
#endif
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1 complete dim=2
#endif
#if ENABLE_BFRAME	
	if (lidx == 0 && cur_refi == 0)
	{
#endif

	for (h = 0; h < 12; h++)
	{
#if FME_PRAGMA_ON	 
#pragma HLS PIPELINE 
#endif
		for(int k =0;k<88;k++)
		{
			RefWindowLCU_sub[k] = RefWindowLCU_2d[(h << 1) + ref_offsety][k];
			RefWindowLCU_sub1[k]= RefWindowLCU_2d[(h << 1) + 1 + ref_offsety][k];
		}
		for (w = 0; w < 24; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
		//	ref0[h][w] = RefWindowLCU_2d[(h << 1) + ref_offsety][w + ref_offsetx];
		//	ref1[h][w] = RefWindowLCU_2d[(h << 1) + 1 + ref_offsety][w + ref_offsetx];
			ref0[h][w] = RefWindowLCU_sub[w + ref_offsetx];
			ref1[h][w] = RefWindowLCU_sub1[w + ref_offsetx];
		}
	}

#if ENABLE_BFRAME	
}
	else
	{
		for (h = 0; h < 12; h++)
		{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE 
#endif
			for(int k =0;k<88;k++)
			{
				RefWindowLCU_sub[k] = RefWindowLCU_2d_ref0[(h << 1) + ref_offsety][k];
				RefWindowLCU_sub1[k]= RefWindowLCU_2d_ref1[(h << 1) + 1 + ref_offsety][k];
			}

			for (w = 0; w < 24; w++)
			{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
				//ref0[h][w] = RefWindowLCU_2d_ref0[(h << 1) + ref_offsety][w + ref_offsetx];
				//ref1[h][w] = RefWindowLCU_2d_ref1[(h << 1) + 1 + ref_offsety][w + ref_offsetx];
				ref0[h][w] = RefWindowLCU_sub[w + ref_offsetx];
				ref1[h][w] = RefWindowLCU_sub1[w + ref_offsetx];
			}
		}
	}
#endif
}

U32 me_spel_satd_8x8(pel org[8][8], pel cur[8][8])
{
	//#pragma HLS INLINE
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=org complete dim=0
#pragma HLS array_partition variable=cur complete dim=0
#endif
	U4 h, w, i;
	U7 j, jj;
	U32 satd = 0;
	S9 diff[64];
	S16 m1[8][8], m2[8][8], m3[8][8],m4[8][8];
#if FME_PRAGMA_ON	
#pragma HLS array_partition variable=diff complete dim=0
#pragma HLS array_partition variable=m1 complete dim=0
#pragma HLS array_partition variable=m2 complete dim=0
#pragma HLS array_partition variable=m3 complete dim=0
#endif
	for (h = 0; h < 8; h++)
	{
#if FME_PRAGMA_ON	
#pragma HLS PIPELINE
#endif
		for (w = 0; w < 8; w++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
			diff[w + h * 8] = org[h][w] - cur[h][w];
		}
	}
	/* horizontal */
	for (j = 0; j < 8; j++)
	{
#if FME_PRAGMA_ON	
#pragma HLS UNROLL
#endif
		jj = j << 3;
		m2[j][0] = diff[jj] + diff[jj + 4];
		m2[j][1] = diff[jj + 1] + diff[jj + 5];
		m2[j][2] = diff[jj + 2] + diff[jj + 6];
		m2[j][3] = diff[jj + 3] + diff[jj + 7];
		m2[j][4] = diff[jj] - diff[jj + 4];
		m2[j][5] = diff[jj + 1] - diff[jj + 5];
		m2[j][6] = diff[jj + 2] - diff[jj + 6];
		m2[j][7] = diff[jj + 3] - diff[jj + 7];
		m1[j][0] = m2[j][0] + m2[j][2];
		m1[j][1] = m2[j][1] + m2[j][3];
		m1[j][2] = m2[j][0] - m2[j][2];
		m1[j][3] = m2[j][1] - m2[j][3];
		m1[j][4] = m2[j][4] + m2[j][6];
		m1[j][5] = m2[j][5] + m2[j][7];
		m1[j][6] = m2[j][4] - m2[j][6];
		m1[j][7] = m2[j][5] - m2[j][7];

		m4[j][0] = m1[j][0] + m1[j][1];
		m4[j][1] = m1[j][0] - m1[j][1];
		m4[j][2] = m1[j][2] + m1[j][3];
		m4[j][3] = m1[j][2] - m1[j][3];
		m4[j][4] = m1[j][4] + m1[j][5];
		m4[j][5] = m1[j][4] - m1[j][5];
		m4[j][6] = m1[j][6] + m1[j][7];
		m4[j][7] = m1[j][6] - m1[j][7];
	}
	/* vertical */
	for (i = 0; i < 8; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		m3[0][i] = m4[0][i] + m4[4][i];
		m3[1][i] = m4[1][i] + m4[5][i];
		m3[2][i] = m4[2][i] + m4[6][i];
		m3[3][i] = m4[3][i] + m4[7][i];
		m3[4][i] = m4[0][i] - m4[4][i];
		m3[5][i] = m4[1][i] - m4[5][i];
		m3[6][i] = m4[2][i] - m4[6][i];
		m3[7][i] = m4[3][i] - m4[7][i];

		m1[0][i] = m3[0][i] + m3[2][i];
		m1[1][i] = m3[1][i] + m3[3][i];
		m1[2][i] = m3[0][i] - m3[2][i];
		m1[3][i] = m3[1][i] - m3[3][i];
		m1[4][i] = m3[4][i] + m3[6][i];
		m1[5][i] = m3[5][i] + m3[7][i];
		m1[6][i] = m3[4][i] - m3[6][i];
		m1[7][i] = m3[5][i] - m3[7][i];

		m2[0][i] = m1[0][i] + m1[1][i];
		m2[1][i] = m1[0][i] - m1[1][i];
		m2[2][i] = m1[2][i] + m1[3][i];
		m2[3][i] = m1[2][i] - m1[3][i];
		m2[4][i] = m1[4][i] + m1[5][i];
		m2[5][i] = m1[4][i] - m1[5][i];
		m2[6][i] = m1[6][i] + m1[7][i];
		m2[7][i] = m1[6][i] - m1[7][i];
	}
	for (i = 0; i < 8; i++)
	{
#if FME_PRAGMA_ON	
#pragma HLS UNROLL
#endif
		for (j = 0; j < 8; j++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif

#ifdef VIVADO_HLS
			satd += COM_ABS16(m2[i][j]);
#else
			satd += abs(m2[i][j]);
#endif			
		}
	}
	satd = ((satd + 2) >> 2);
	return satd;
}

void interp_horizontal_8x8_1(pel ref[16][16], S16 hor_buf0[8][16], S16 hor_buf1[8][16], S16 hor_buf2[8][16],
	S16 hor_buf3[8][16])
{


	pel line_buffer[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer complete dim=0
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2
#pragma HLS array_partition variable=ref complete dim=2
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE II=2
#endif
		//ref_line
		for (int j = 0; j < 16; j++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			line_buffer[j] = ref[i][j];
		}

		//hor_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			hor_buf0[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
		}

		//hor_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
	//		hor_buf1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);
			hor_buf1[k1][k11] = MAC_8TAP_1FME(line_buffer, 1 + k1);;
		}

		//hor_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
	//		hor_buf2[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
			hor_buf2[k2][k22] = MAC_8TAP_2FME(line_buffer, 1 + k2);
		}

		//hor_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
	//		hor_buf3[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
			hor_buf3[k3][k33] = MAC_8TAP_3FME(line_buffer, k3);
		}
	}
}

void interp_horizontal_8x8_2(pel ref[16][16], S16 hor_buf0[8][16], S16 hor_buf1[8][16], S16 hor_buf2[8][16],
	S16 hor_buf3[8][16])
{
	pel line_buffer[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE II=2
#endif
		//ref_line
		for (int j = 0; j < 16; j++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			line_buffer[j] = ref[i][j];
		}

		//hor_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL 
#endif
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			hor_buf0[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
		}

		//hor_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
//			hor_buf1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);
			hor_buf1[k1][k11] = MAC_8TAP_1FME(line_buffer, 1 + k1);;
		}

		//hor_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL 
#endif
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
	//		hor_buf2[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
			hor_buf2[k2][k22] = MAC_8TAP_2FME(line_buffer, 1 + k2);
		}

		//hor_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL 
#endif
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
	//		hor_buf3[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
			hor_buf3[k3][k33] = MAC_8TAP_3FME(line_buffer, k3);
		}
	}
}



//void interp_horizontal_16x8_1(int flag,pel ref[16][24], pel ref1[16][16], S16 hor_buf0[16][16], S16 hor_buf1[16][16], S16 hor_buf2[16][16],
//	S16 hor_buf3[16][16], S16 hor_buf01[8][16], S16 hor_buf11[8][16], S16 hor_buf21[8][16],S16 hor_buf31[8][16])
//{
//	pel line_buffer[24];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
//#if FME_PRAGMA_ON
//#pragma HLS array_partition variable=line_buffer complete dim=0
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
//#endif
//
//	for (int i = 0; i < 16; i++)
//	{
//		if (flag < 96)
//		{
//#if FME_PRAGMA_ON
//#pragma HLS PIPELINE
//#endif
//			//ref_line
//			for (int j = 0; j < 24; j++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				line_buffer[j] = ref[i][j];
//			}
//
//			//hor_buf0
//			for (int k0 = 0; k0 < 16; k0++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
//				hor_buf0[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
//			}
//
//			//hor_buf1
//			for (int k1 = 0; k1 < 16; k1++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
//				hor_buf1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);;
//			}
//
//			//hor_buf2
//			for (int k2 = 0; k2 < 16; k2++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
//				hor_buf2[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
//			}
//
//			//hor_buf3
//			for (int k3 = 0; k3 < 16; k3++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
//				hor_buf3[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
//			}
//		}
//		else
//		{
//#if FME_PRAGMA_ON
//#pragma HLS PIPELINE
//#endif
//			//ref_line
//			for (int j = 0; j < 16; j++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				line_buffer[j] = ref1[i][j];
//			}
//
//
//			//hor_buf0
//			for (int k0 = 0; k0 < 8; k0++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
//				hor_buf01[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
//			}
//
//			//hor_buf1
//			for (int k1 = 0; k1 < 8; k1++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
//				hor_buf11[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);;
//			}
//
//			//hor_buf2
//			for (int k2 = 0; k2 < 8; k2++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
//				hor_buf21[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
//			}
//
//			//hor_buf3
//			for (int k3 = 0; k3 < 8; k3++)
//			{
//#if FME_PRAGMA_ON
//#pragma HLS UNROLL
//#endif
//				int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
//				hor_buf31[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
//			}
//		}
//	}
//}
void interp_horizontal_16x8_1(pel ref[16][24], S16 hor_buf0[16][16], S16 hor_buf1[16][16], S16 hor_buf2[16][16],
	S16 hor_buf3[16][16])
{
	pel line_buffer[24];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=ref complete dim=2
#pragma HLS array_partition variable=line_buffer complete dim=0
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE 
#endif
		//ref_line
		for (int j = 0; j < 24; j++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			line_buffer[j] = ref[i][j];
		}

		//hor_buf0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			hor_buf0[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
		}

		//hor_buf1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
		//	hor_buf1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);
			hor_buf1[k1][k11] = MAC_8TAP_1FME(line_buffer, 1 + k1);
		}

		//hor_buf2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
		//	hor_buf2[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
			hor_buf2[k2][k22] = MAC_8TAP_2FME(line_buffer, 1 + k2);
		}

		//hor_buf3
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
		//	hor_buf3[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
			hor_buf3[k3][k33] = MAC_8TAP_3FME(line_buffer, k3);
		}
	}
}


void interp_horizontal_16x8_2(pel ref[16][24], S16 hor_buf0[16][16], S16 hor_buf1[16][16], S16 hor_buf2[16][16],
	S16 hor_buf3[16][16])
{
	pel line_buffer[24];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//ref_line
		for (int j = 0; j < 24; j++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			line_buffer[j] = ref[i][j];
		}

		//hor_buf0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			hor_buf0[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
		}

		//hor_buf1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
			hor_buf1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);;
		}

		//hor_buf2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
			hor_buf2[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
		}

		//hor_buf3
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
			hor_buf3[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
		}
	}
}

void interp_horizontal_16x8_3(pel ref[16][24], S16 hor_buf0[16][16], S16 hor_buf1[16][16], S16 hor_buf2[16][16],
	S16 hor_buf3[16][16])
{
	pel line_buffer[24];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//ref_line
		for (int j = 0; j < 24; j++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			line_buffer[j] = ref[i][j];
		}

		//hor_buf0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			hor_buf0[k0][k00] = ((S16)line_buffer[k0 + 4]) << 6;
		}

		//hor_buf1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
			hor_buf1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);;
		}

		//hor_buf2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
			hor_buf2[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
		}

		//hor_buf3
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
			hor_buf3[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
		}
	}
}
// |*************|*************|
// |             |             |
// |             |             |
// |16  bufx_0   |    bufx_1   |
// |             |             |
// |     12      |             |
// |*************|*************|


// *-----------------*
// *12	       	     *
// *      bufx_0     *
// *        16       *
// *-----------------*
// *  	       	     *     
// *      bufx_1     * 
// *                 *
// *-----------------*

void interp_horizontal_16x16(pel ref0[12][24], pel ref1[12][24],
	S16 hor_buf0_0[16][16], S16 hor_buf0_1[16][16],
	S16 hor_buf1_0[16][16], S16 hor_buf1_1[16][16],
	S16 hor_buf2_0[16][16], S16 hor_buf2_1[16][16],
	S16 hor_buf3_0[16][16], S16 hor_buf3_1[16][16])
{
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=ref0 complete dim=2
#pragma HLS array_partition variable=ref1 complete dim=2
#pragma HLS RESOURCE variable=ref0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref1 core=RAM_1P_BRAM

#pragma HLS array_partition variable=hor_buf0_0 complete dim=2
#pragma HLS array_partition variable=hor_buf1_0 complete dim=2
#pragma HLS array_partition variable=hor_buf2_0 complete dim=2
#pragma HLS array_partition variable=hor_buf3_0 complete dim=2
#pragma HLS array_partition variable=hor_buf0_1 complete dim=2
#pragma HLS array_partition variable=hor_buf1_1 complete dim=2
#pragma HLS array_partition variable=hor_buf2_1 complete dim=2
#pragma HLS array_partition variable=hor_buf3_1 complete dim=2
#endif
	pel line_buffer0[24];
	pel line_buffer1[24];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE II=2
#endif
		//ref_line
		if (i < 12)
		{
			for (int j = 0; j < 24; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				line_buffer0[j] = ref0[i][j];
				line_buffer1[j] = ref1[i][j];
			}
		}

		//hor_buf0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			hor_buf0_0[k0][k00] = ((S16)line_buffer0[k0 + 4]) << 6;
			hor_buf0_1[k0][k00] = ((S16)line_buffer1[k0 + 4]) << 6;
		}

		//hor_buf1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
		//	hor_buf1_0[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
		//	hor_buf1_1[k1][k11] = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			hor_buf1_0[k1][k11] = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			hor_buf1_1[k1][k11] = MAC_8TAP_1FME(line_buffer1, 1 + k1);
		}

		//hor_buf2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
		//	hor_buf2_0[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
		//	hor_buf2_1[k2][k22] = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			hor_buf2_0[k2][k22] = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			hor_buf2_1[k2][k22] = MAC_8TAP_2FME(line_buffer1, 1 + k2);
		}

		//hor_buf3
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
		//	hor_buf3_0[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
		//	hor_buf3_1[k3][k33] = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			hor_buf3_0[k3][k33] = MAC_8TAP_3FME(line_buffer0, k3);
			hor_buf3_1[k3][k33] = MAC_8TAP_3FME(line_buffer1, k3);
		}
		
	}
}

void interp_vertical_8x8_1(S16 hor_buf0[8][16], S16 hor_buf1[8][16], S16 hor_buf2[8][16], S16 hor_buf3[8][16],
	pel ver_buf0[16][32], pel ver_buf1[16][32], U4 bit_depth) 
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};

	pel ver_buf0_1[16][32];
	pel ver_buf1_1[16][32];
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2

//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2

#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE II=2
#endif
		//hor_line
		if (i < 4)
		{
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j3 > 15) ? (i << 1) % 8 + j3 - 16 : (i << 1) % 8 + j3;
				line_buffer0[j3] = hor_buf3[(i << 1)][jt];
			}
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j3 > 15) ? ((i << 1) + 1) % 8 + j3 - 16 : ((i << 1) + 1) % 8 + j3;
				line_buffer1[j3] = hor_buf3[((i << 1) + 1)][jt];
			}
		}
		else if (i < 8)
		{
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j0 > 15) ? (i << 1) % 8 + j0 - 16 : (i << 1) % 8 + j0;
				line_buffer0[j0] = hor_buf0[(i << 1) - 8][jt];
			}
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j0 > 15) ? ((i << 1) + 1) % 8 + j0 - 16 : ((i << 1) + 1) % 8 + j0;
				line_buffer1[j0] = hor_buf0[((i << 1) + 1) - 8][jt];
			}
		}
		else if (i < 12)
		{
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j1 > 15) ? (i << 1) % 8 + j1 - 16 : (i << 1) % 8 + j1;
				line_buffer0[j1] = hor_buf1[(i << 1) - 16][jt];
			}
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j1 > 15) ? ((i << 1) + 1) % 8 + j1 - 16 : ((i << 1) + 1) % 8 + j1;
				line_buffer1[j1] = hor_buf1[((i << 1) + 1) - 16][jt];
			}
		}
		else
		{
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j2 > 15) ? (i << 1) % 8 + j2 - 16 : (i << 1) % 8 + j2;
				line_buffer0[j2] = hor_buf2[(i << 1) - 24][jt];
			}
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j2 > 15) ? ((i << 1) + 1) % 8 + j2 - 16 : ((i << 1) + 1) % 8 + j2;
				line_buffer1[j2] = hor_buf2[((i << 1) + 1) - 24][jt];
			}
		}

		//ver_buf0

		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S18 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			S18 pt2 = line_buffer1[k0 + 4];
			S12 pt3 = (pt2 + (1 << 5)) >> 6;
			if( i % 2 == 0) {
			ver_buf0[8 + (i >> 1)][0 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf0[8 + (i >> 1)][0 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);
			}
			else{
			ver_buf0[8 + (i >> 1)][16 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf0[8 + (i >> 1)][16 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);

			}
		}


		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;

	//		S24 pt2 = MAC_8TAP_FME(line_buffer1, 1 + k1);
			S24 pt2 = MAC_8TAP_1FME(line_buffer1, 1 + k1);
			S12 pt3 = (pt2 + (1 << 11)) >> 12;
			if( i % 2 == 0) {
			ver_buf1[0 + (i >> 1)][0 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf1[0 + (i >> 1)][0 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);
			}
			else {
			ver_buf1[0 + (i >> 1)][16 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf1[0 + (i >> 1)][16 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);
			}
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;

	//		S24 pt2 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt2 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt3 = (pt2 + (1 << 11)) >> 12;
			if( i % 2 == 0) {
			ver_buf1[8 + (i >> 1)][0 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf1[8 + (i >> 1)][0 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);
			}
			else {
			ver_buf1[8 + (i >> 1)][16 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf1[8 + (i >> 1)][16 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);

			}
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;

	//		S24 pt2 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt2 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt3 = (pt2 + (1 << 11)) >> 12;
			if( i % 2 == 0) {
			ver_buf0[0 + (i >> 1)][0+ k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf0[0 + (i >> 1)][0 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);
			}
			else{
			ver_buf0[0 + (i >> 1)][16 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			ver_buf0[0 + (i >> 1)][16 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt3);

			}
		}

	}
}

void interp_vertical_8x8_2(S16 hor_buf0[8][16], S16 hor_buf1[8][16], S16 hor_buf2[8][16], S16 hor_buf3[8][16],
	pel ver_buf0[16][32], pel ver_buf1[16][32], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2
//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2
#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line
		if (i < 4)
		{
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j3 > 15) ? (i << 1) % 8 + j3 - 16 : (i << 1) % 8 + j3;
				line_buffer0[j3] = hor_buf3[(i << 1)][jt];
			}
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j3 > 15) ? ((i << 1) + 1) % 8 + j3 - 16 : ((i << 1) + 1) % 8 + j3;
				line_buffer1[j3] = hor_buf3[((i << 1) + 1)][jt];
			}
		}
		else if (i < 8)
		{
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j0 > 15) ? (i << 1) % 8 + j0 - 16 : (i << 1) % 8 + j0;
				line_buffer0[j0] = hor_buf0[(i << 1) - 8][jt];
			}
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j0 > 15) ? ((i << 1) + 1) % 8 + j0 - 16 : ((i << 1) + 1) % 8 + j0;
				line_buffer1[j0] = hor_buf0[((i << 1) + 1) - 8][jt];
			}
		}
		else if (i < 12)
		{
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j1 > 15) ? (i << 1) % 8 + j1 - 16 : (i << 1) % 8 + j1;
				line_buffer0[j1] = hor_buf1[(i << 1) - 16][jt];
			}
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j1 > 15) ? ((i << 1) + 1) % 8 + j1 - 16 : ((i << 1) + 1) % 8 + j1;
				line_buffer1[j1] = hor_buf1[((i << 1) + 1) - 16][jt];
			}
		}
		else
		{
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 8 + j2 > 15) ? (i << 1) % 8 + j2 - 16 : (i << 1) % 8 + j2;
				line_buffer0[j2] = hor_buf2[(i << 1) - 24][jt];
			}
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 8 + j2 > 15) ? ((i << 1) + 1) % 8 + j2 - 16 : ((i << 1) + 1) % 8 + j2;
				line_buffer1[j2] = hor_buf2[((i << 1) + 1) - 24][jt];
			}
		}


		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S18 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if(i % 2 == 0) {
				ver_buf0[8 + (i >> 1)][0 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[8 + (i >> 1)][16 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S18 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if(i % 2 == 0) {
				ver_buf0[8 + (i >> 1)][0 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[8 + (i >> 1)][16 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if(i % 2 == 0) {
				ver_buf1[0 + (i >> 1)][0 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[0 + (i >> 1)][16 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			
		
		
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
//			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer1, 1 + k1);
			S12 pt4 = (pt0 + (1 << 11)) >> 12;
			if(i % 2 == 0) {
				ver_buf1[0 + (i >> 1)][0 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt4);
			}
			else {
				ver_buf1[0 + (i >> 1)][16 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt4);
			}
			
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
//			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if(i % 2 == 0) {
				ver_buf1[8 + (i >> 1)][0 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[8 + (i >> 1)][16 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if(i % 2 == 0) {
				ver_buf1[8 + (i >> 1)][0 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[8 + (i >> 1)][16 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if(i % 2 == 0) {
				ver_buf0[0 + (i >> 1)][0 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[0 + (i >> 1)][16 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL 
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if(i % 2 == 0) {
				ver_buf0[0 + (i >> 1)][0 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[0 + (i >> 1)][16 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		}
	}
}
void interp_vertical_16x8_1_1(S16 hor_buf3[16][16], S16 hor_buf0[16][16], pel ver_buf3[8][64], pel ver_buf0[8][64], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2
//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf3 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf3 complete dim=2
#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf3 core=RAM_1P_BRAM
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE II=2
#endif
		//hor_line
		if (i < 8)
		{
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j3 > 15) ? (i << 1) % 16 + j3 - 16 : (i << 1) % 16 + j3;
				line_buffer0[j3] = hor_buf3[(i << 1)][jt];
			}
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j3 > 15) ? ((i << 1) + 1) % 16 + j3 - 16 : ((i << 1) + 1) % 16 + j3;
				line_buffer1[j3] = hor_buf3[((i << 1) + 1)][jt];
			}
		}
		else
		{
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j0 > 15) ? (i << 1) % 16 + j0 - 16 : (i << 1) % 16 + j0;
				line_buffer0[j0] = hor_buf0[(i << 1) - 16][jt];
			}
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j0 > 15) ? ((i << 1) + 1) % 16 + j0 - 16 : ((i << 1) + 1) % 16 + j0;
				line_buffer1[j0] = hor_buf0[((i << 1) + 1) - 16][jt];
			}
		}

		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf3[4 + (i >> 2)][0 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[4 + (i >> 2)][16 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[4 + (i >> 2)][32 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[4 + (i >> 2)][48 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf3[4 + (i >> 2)][0 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[4 + (i >> 2)][16 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[4 + (i >> 2)][32 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[4 + (i >> 2)][48 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[0 + (i >> 2)][0 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[0 + (i >> 2)][16 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[0 + (i >> 2)][32 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[0 + (i >> 2)][48 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME( line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[0 + (i >> 2)][0 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[0 + (i >> 2)][16 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[0 + (i >> 2)][32 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[0 + (i >> 2)][48 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[4 + (i >> 2)][0 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[4 + (i >> 2)][16 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[4 + (i >> 2)][32 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[4 + (i >> 2)][48 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}

			//ver_buf0[4 + (i >> 2)][((i % 4) << 4) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[4 + (i >> 2)][0 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[4 + (i >> 2)][16 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[4 + (i >> 2)][32 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[4 + (i >> 2)][48 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf0[4 + (i >> 2)][((i % 4) << 4) + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf3[0 + (i >> 2)][0 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[0 + (i >> 2)][16 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[0 + (i >> 2)][32 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[0 + (i >> 2)][48 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf3[0 + (i >> 2)][((i % 4) << 4) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf3[0 + (i >> 2)][0 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[0 + (i >> 2)][16 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[0 + (i >> 2)][32 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[0 + (i >> 2)][48 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf3[0 + (i >> 2)][((i % 4) << 4) + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}
void interp_vertical_16x8_2_1(S16 hor_buf1[16][16], S16 hor_buf2[16][16], pel ver_buf1[8][64], pel ver_buf2[8][64], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE II=2
#endif
		//hor_line

		if (i < 8)
		{
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j1 > 15) ? (i << 1) % 16 + j1 - 16 : (i << 1) % 16 + j1;
				line_buffer0[j1] = hor_buf1[i << 1][jt];
			}
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j1 > 15) ? ((i << 1) + 1) % 16 + j1 - 16 : ((i << 1) + 1) % 16 + j1;
				line_buffer1[j1] = hor_buf1[(i << 1) + 1][jt];
			}
		}
		else
		{
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j2 > 15) ? (i << 1) % 16 + j2 - 16 : (i << 1) % 16 + j2;
				line_buffer0[j2] = hor_buf2[(i << 1) - 16][jt];
			}
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j2 > 15) ? ((i << 1) + 1) % 16 + j2 - 16 : ((i << 1) + 1) % 16 + j2;
				line_buffer1[j2] = hor_buf2[((i << 1) + 1) - 16][jt];
			}
		}


		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf1[4 + (i >> 2)][0 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[4 + (i >> 2)][16 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[4 + (i >> 2)][32 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[4 + (i >> 2)][48 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			//ver_buf1[4 + (i >> 2)][((i % 4) << 4) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf1[4 + (i >> 2)][0 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[4 + (i >> 2)][16 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[4 + (i >> 2)][32 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[4 + (i >> 2)][48 + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			//ver_buf1[4 + (i >> 2)][((i % 4) << 4) + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[0 + (i >> 2)][0 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[0 + (i >> 2)][16 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[0 + (i >> 2)][32 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[0 + (i >> 2)][48 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			//ver_buf2[0 + (i >> 2)][((i % 4) << 4) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[0 + (i >> 2)][0 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[0 + (i >> 2)][16 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[0 + (i >> 2)][32 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[0 + (i >> 2)][48 + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			//ver_buf2[0 + (i >> 2)][((i % 4) << 4) + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME( line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[4 + (i >> 2)][0 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[4 + (i >> 2)][16 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[4 + (i >> 2)][32 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[4 + (i >> 2)][48 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			//ver_buf2[4 + (i >> 2)][((i % 4) << 4) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[4 + (i >> 2)][0 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[4 + (i >> 2)][16 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[4 + (i >> 2)][32 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[4 + (i >> 2)][48 + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			//ver_buf2[4 + (i >> 2)][((i % 4) << 4) + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf1[0 + (i >> 2)][0 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[0 + (i >> 2)][16 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[0 + (i >> 2)][32 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[0 + (i >> 2)][48 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf1[0 + (i >> 2)][((i % 4) << 4) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
	//		S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf1[0 + (i >> 2)][0 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[0 + (i >> 2)][16 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[0 + (i >> 2)][32 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[0 + (i >> 2)][48 + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf1[0 + (i >> 2)][((i % 4) << 4) + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x8_1_2(S16 hor_buf3[16][16], S16 hor_buf0[16][16], pel ver_buf3[8][64], pel ver_buf0[8][64], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line
		if (i < 8)
		{
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j3 > 15) ? (i << 1) % 16 + j3 - 16 : (i << 1) % 16 + j3;
				line_buffer0[j3] = hor_buf3[(i << 1)][jt];
			}
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j3 > 15) ? ((i << 1) + 1) % 16 + j3 - 16 : ((i << 1) + 1) % 16 + j3;
				line_buffer1[j3] = hor_buf3[((i << 1) + 1)][jt];
			}
		}
		else
		{
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j0 > 15) ? (i << 1) % 16 + j0 - 16 : (i << 1) % 16 + j0;
				line_buffer0[j0] = hor_buf0[(i << 1) - 16][jt];
			}
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j0 > 15) ? ((i << 1) + 1) % 16 + j0 - 16 : ((i << 1) + 1) % 16 + j0;
				line_buffer1[j0] = hor_buf0[((i << 1) + 1) - 16][jt];
			}
		}

		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf3[4 + (i >> 2)][((i % 4) << 4) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf3[4 + (i >> 2)][((i % 4) << 4) + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[0 + (i >> 2)][((i % 4) << 4) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[0 + (i >> 2)][((i % 4) << 4) + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[4 + (i >> 2)][((i % 4) << 4) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[4 + (i >> 2)][((i % 4) << 4) + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf3[0 + (i >> 2)][((i % 4) << 4) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf3[0 + (i >> 2)][((i % 4) << 4) + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x8_2_2(S16 hor_buf1[16][16], S16 hor_buf2[16][16], pel ver_buf1[8][64], pel ver_buf2[8][64], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line

		if (i < 8)
		{
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j1 > 15) ? (i << 1) % 16 + j1 - 16 : (i << 1) % 16 + j1;
				line_buffer0[j1] = hor_buf1[i << 1][jt];
			}
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j1 > 15) ? ((i << 1) + 1) % 16 + j1 - 16 : ((i << 1) + 1) % 16 + j1;
				line_buffer1[j1] = hor_buf1[(i << 1) + 1][jt];
			}
		}
		else
		{
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j2 > 15) ? (i << 1) % 16 + j2 - 16 : (i << 1) % 16 + j2;
				line_buffer0[j2] = hor_buf2[(i << 1) - 16][jt];
			}
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j2 > 15) ? ((i << 1) + 1) % 16 + j2 - 16 : ((i << 1) + 1) % 16 + j2;
				line_buffer1[j2] = hor_buf2[((i << 1) + 1) - 16][jt];
			}
		}


		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf1[4 + (i >> 2)][((i % 4) << 4) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf1[4 + (i >> 2)][((i % 4) << 4) + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[0 + (i >> 2)][((i % 4) << 4) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[0 + (i >> 2)][((i % 4) << 4) + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[4 + (i >> 2)][((i % 4) << 4) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[4 + (i >> 2)][((i % 4) << 4) + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf1[0 + (i >> 2)][((i % 4) << 4) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf1[0 + (i >> 2)][((i % 4) << 4) + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x8_1_3(S16 hor_buf3[16][16], S16 hor_buf0[16][16], pel ver_buf3[8][64], pel ver_buf0[8][64], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line
		if (i < 8)
		{
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j3 > 15) ? (i << 1) % 16 + j3 - 16 : (i << 1) % 16 + j3;
				line_buffer0[j3] = hor_buf3[(i << 1)][jt];
			}
			for (int j3 = 0; j3 < 16; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j3 > 15) ? ((i << 1) + 1) % 16 + j3 - 16 : ((i << 1) + 1) % 16 + j3;
				line_buffer1[j3] = hor_buf3[((i << 1) + 1)][jt];
			}
		}
		else
		{
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j0 > 15) ? (i << 1) % 16 + j0 - 16 : (i << 1) % 16 + j0;
				line_buffer0[j0] = hor_buf0[(i << 1) - 16][jt];
			}
			for (int j0 = 0; j0 < 16; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j0 > 15) ? ((i << 1) + 1) % 16 + j0 - 16 : ((i << 1) + 1) % 16 + j0;
				line_buffer1[j0] = hor_buf0[((i << 1) + 1) - 16][jt];
			}
		}

		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf3[4 + (i >> 2)][((i % 4) << 4) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf3[4 + (i >> 2)][((i % 4) << 4) + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[0 + (i >> 2)][((i % 4) << 4) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[0 + (i >> 2)][((i % 4) << 4) + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[4 + (i >> 2)][((i % 4) << 4) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf0[4 + (i >> 2)][((i % 4) << 4) + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf3[0 + (i >> 2)][((i % 4) << 4) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf3[0 + (i >> 2)][((i % 4) << 4) + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x8_2_3(S16 hor_buf1[16][16], S16 hor_buf2[16][16], pel ver_buf1[8][64], pel ver_buf2[8][64], U4 bit_depth)
{
	S16 line_buffer0[16];
	S16 line_buffer1[16];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line

		if (i < 8)
		{
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j1 > 15) ? (i << 1) % 16 + j1 - 16 : (i << 1) % 16 + j1;
				line_buffer0[j1] = hor_buf1[i << 1][jt];
			}
			for (int j1 = 0; j1 < 16; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j1 > 15) ? ((i << 1) + 1) % 16 + j1 - 16 : ((i << 1) + 1) % 16 + j1;
				line_buffer1[j1] = hor_buf1[(i << 1) + 1][jt];
			}
		}
		else
		{
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j2 > 15) ? (i << 1) % 16 + j2 - 16 : (i << 1) % 16 + j2;
				line_buffer0[j2] = hor_buf2[(i << 1) - 16][jt];
			}
			for (int j2 = 0; j2 < 16; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j2 > 15) ? ((i << 1) + 1) % 16 + j2 - 16 : ((i << 1) + 1) % 16 + j2;
				line_buffer1[j2] = hor_buf2[((i << 1) + 1) - 16][jt];
			}
		}


		//ver_buf0
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf1[4 + (i >> 2)][((i % 4) << 4) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 8; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			ver_buf1[4 + (i >> 2)][((i % 4) << 4) + k0 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[0 + (i >> 2)][((i % 4) << 4) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 8; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[0 + (i >> 2)][((i % 4) << 4) + k1 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[4 + (i >> 2)][((i % 4) << 4) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 8; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf2[4 + (i >> 2)][((i % 4) << 4) + k2 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf1[0 + (i >> 2)][((i % 4) << 4) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 8; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			ver_buf1[0 + (i >> 2)][((i % 4) << 4) + k3 + 8] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x16_1_4(S16 hor_buf3_0[16][16], S16 hor_buf3_1[16][16],
	S16 hor_buf0_0[16][16], S16 hor_buf0_1[16][16], pel ver_buf3[8][128], pel ver_buf0[8][128], U4 bit_depth)
{
	S16 line_buffer0[24];
	S16 line_buffer1[24];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line
		if (i < 8)
		{
			for (int j3 = 0; j3 < 12; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j3 > 15) ? (i << 1) % 16 + j3 - 16 : (i << 1) % 16 + j3;
				line_buffer0[j3 * 2] = hor_buf3_0[(i << 1)][jt];
				line_buffer0[j3 * 2 + 1] = hor_buf3_1[(i << 1)][jt];
			}
			for (int j3 = 0; j3 < 12; j3++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j3 > 15) ? ((i << 1) + 1) % 16 + j3 - 16 : ((i << 1) + 1) % 16 + j3;
				line_buffer1[j3 * 2] = hor_buf3_0[((i << 1) + 1)][jt];
				line_buffer1[j3 * 2 + 1] = hor_buf3_1[((i << 1) + 1)][jt];
			}
		}
		else
		{
			for (int j0 = 0; j0 < 12; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j0 > 15) ? (i << 1) % 16 + j0 - 16 : (i << 1) % 16 + j0;
				line_buffer0[j0 * 2] = hor_buf0_0[(i << 1) - 16][jt];
				line_buffer0[j0 * 2 + 1] = hor_buf0_1[(i << 1) - 16][jt];
			}
			for (int j0 = 0; j0 < 12; j0++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j0 > 15) ? ((i << 1) + 1) % 16 + j0 - 16 : ((i << 1) + 1) % 16 + j0;
				line_buffer1[j0 * 2] = hor_buf0_0[((i << 1) + 1) - 16][jt];
				line_buffer1[j0 * 2 + 1] = hor_buf0_1[((i << 1) + 1) - 16][jt];
			}
		}

		//ver_buf0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf3[4 + (i >> 2)][0 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[4 + (i >> 2)][32 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[4 + (i >> 2)][64 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[4 + (i >> 2)][96 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf3[4 + (i >> 2)][((i % 4) << 5) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf3[4 + (i >> 2)][0 + k0 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[4 + (i >> 2)][32 + k0 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[4 + (i >> 2)][64 + k0 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[4 + (i >> 2)][96 + k0 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf3[4 + (i >> 2)][((i % 4) << 5) + k0 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[0 + (i >> 2)][0 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[0 + (i >> 2)][32 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[0 + (i >> 2)][64 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[0 + (i >> 2)][96 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}

		//	ver_buf0[0 + (i >> 2)][((i % 4) << 5) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[0 + (i >> 2)][0 + k1 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[0 + (i >> 2)][32 + k1 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[0 + (i >> 2)][64 + k1 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[0 + (i >> 2)][96 + k1 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf0[0 + (i >> 2)][((i % 4) << 5) + k1 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[4 + (i >> 2)][0 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[4 + (i >> 2)][32 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[4 + (i >> 2)][64 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[4 + (i >> 2)][96 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf0[4 + (i >> 2)][((i % 4) << 5) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf0[4 + (i >> 2)][16 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf0[4 + (i >> 2)][48 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf0[4 + (i >> 2)][80 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf0[4 + (i >> 2)][112 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf0[4 + (i >> 2)][((i % 4) << 5) + k2 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf3[0 + (i >> 2)][0 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[0 + (i >> 2)][32 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[0 + (i >> 2)][64 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[0 + (i >> 2)][96 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf3[0 + (i >> 2)][((i % 4) << 5) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf3[0 + (i >> 2)][16 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf3[0 + (i >> 2)][48 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf3[0 + (i >> 2)][80 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf3[0 + (i >> 2)][112 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf3[0 + (i >> 2)][((i % 4) << 5) + k3 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x16_2_4(S16 hor_buf1_0[16][16], S16 hor_buf1_1[16][16],
	S16 hor_buf2_0[16][16], S16 hor_buf2_1[16][16], pel ver_buf1[8][128], pel ver_buf2[8][128], U4 bit_depth)
{
	S16 line_buffer0[24];
	S16 line_buffer1[24];
//	S8 tbl_mc_l_coeff[3][8] =
//	{
//		{ -1, 4, -10, 57, 19, -7, 3, -1 },
//		{ -1, 4, -11, 40, 40, -11, 4, -1 },
//		{ -1, 3, -7, 19, 57, -10, 4, -1 }
//	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer0 complete dim=0
#pragma HLS array_partition variable=line_buffer1 complete dim=0
//#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line

		if (i < 8)
		{
			for (int j1 = 0; j1 < 12; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j1 > 15) ? (i << 1) % 16 + j1 - 16 : (i << 1) % 16 + j1;
				line_buffer0[j1 * 2] = hor_buf1_0[i << 1][jt];
				line_buffer0[j1 * 2 + 1] = hor_buf1_1[i << 1][jt];
			}
			for (int j1 = 0; j1 < 12; j1++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j1 > 15) ? ((i << 1) + 1) % 16 + j1 - 16 : ((i << 1) + 1) % 16 + j1;
				line_buffer1[j1 * 2] = hor_buf1_0[(i << 1) + 1][jt];
				line_buffer1[j1 * 2 + 1] = hor_buf1_1[(i << 1) + 1][jt];
			}
		}
		else
		{
			for (int j2 = 0; j2 < 12; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = ((i << 1) % 16 + j2 > 15) ? (i << 1) % 16 + j2 - 16 : (i << 1) % 16 + j2;
				line_buffer0[j2 * 2] = hor_buf2_0[(i << 1) - 16][jt];
				line_buffer0[j2 * 2 + 1] = hor_buf2_1[(i << 1) - 16][jt];
			}
			for (int j2 = 0; j2 < 12; j2++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				int jt = (((i << 1) + 1) % 16 + j2 > 15) ? ((i << 1) + 1) % 16 + j2 - 16 : ((i << 1) + 1) % 16 + j2;
				line_buffer1[j2 * 2] = hor_buf2_0[((i << 1) + 1) - 16][jt];
				line_buffer1[j2 * 2 + 1] = hor_buf2_1[((i << 1) + 1) - 16][jt];
			}
		}


		//ver_buf0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer0[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf1[4 + (i >> 2)][0 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[4 + (i >> 2)][32 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[4 + (i >> 2)][64 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[4 + (i >> 2)][96 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf1[4 + (i >> 2)][((i % 4) << 5) + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = line_buffer1[k0 + 4];
			S12 pt1 = (pt0 + (1 << 5)) >> 6;
			if( i % 4 == 0){
				ver_buf1[4 + (i >> 2)][16 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[4 + (i >> 2)][48 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[4 + (i >> 2)][80 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[4 + (i >> 2)][112 + k0] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf1[4 + (i >> 2)][((i % 4) << 5) + k0 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer0, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer0, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[0 + (i >> 2)][0 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[0 + (i >> 2)][32 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[0 + (i >> 2)][64 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[0 + (i >> 2)][96 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf2[0 + (i >> 2)][((i % 4) << 5) + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer1, 1 + k1);
			S24 pt0 = MAC_8TAP_1FME(line_buffer1, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[0 + (i >> 2)][16 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[0 + (i >> 2)][48 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[0 + (i >> 2)][80 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[0 + (i >> 2)][112 + k1] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf2[0 + (i >> 2)][((i % 4) << 5) + k1 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer0, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer0, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[4 + (i >> 2)][0 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[4 + (i >> 2)][32 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[4 + (i >> 2)][64 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[4 + (i >> 2)][96 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf2[4 + (i >> 2)][((i % 4) << 5) + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer1, 1 + k2);
			S24 pt0 = MAC_8TAP_2FME(line_buffer1, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf2[4 + (i >> 2)][16 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf2[4 + (i >> 2)][48 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf2[4 + (i >> 2)][80 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf2[4 + (i >> 2)][112 + k2] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
//			ver_buf2[4 + (i >> 2)][((i % 4) << 5) + k2 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//ver_buf3
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer0, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer0, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf1[0 + (i >> 2)][0+ k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[0 + (i >> 2)][32 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[0 + (i >> 2)][64 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[0 + (i >> 2)][96 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf1[0 + (i >> 2)][((i % 4) << 5) + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		//	S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer1, k3);
			S24 pt0 = MAC_8TAP_3FME(line_buffer1, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			if( i % 4 == 0){
				ver_buf1[0 + (i >> 2)][16 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 1){
				ver_buf1[0 + (i >> 2)][48 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else if (i % 4 == 2){
				ver_buf1[0 + (i >> 2)][80 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
			else {
				ver_buf1[0 + (i >> 2)][112 + k3] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
			}
		//	ver_buf1[0 + (i >> 2)][((i % 4) << 5) + k3 + 16] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void interp_vertical_16x16(S16 hor_buf0[16][16], S16 hor_buf1[16][16],
	pel fb3[16][16], pel fb0[16][16], pel fb1[16][16], pel fb2[16][16], U4 bit_depth)
{
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=fb3 complete dim=2
#pragma HLS array_partition variable=fb0 complete dim=2
#pragma HLS array_partition variable=fb1 complete dim=2
#pragma HLS array_partition variable=fb2 complete dim=2
#endif
	S16 line_buffer[24];
	S8 tbl_mc_l_coeff[3][8] =
	{
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=line_buffer complete dim=0
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		//hor_line
		for (int j3 = 0; j3 < 12; j3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			int jt = (i + j3 > 15) ? i + j3 - 16 : i + j3;
			line_buffer[j3 * 2] = hor_buf0[i][jt];
			line_buffer[j3 * 2 + 1] = hor_buf1[i][jt];
		}

		//0
		for (int k0 = 0; k0 < 16; k0++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S16 pt0 = line_buffer[k0 + 4];
			S10 pt1 = (pt0 + (1 << 5)) >> 6;
			int k00 = (k0 + i > 15) ? k0 + i - 16 : k0 + i;
			fb0[k0][k00] = COM_CLIP3((S10)0, (S10)((1 << bit_depth) - 1), pt1);
		}

		//1
		for (int k1 = 0; k1 < 16; k1++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[0], line_buffer, 1 + k1);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			int k11 = (k1 + i > 15) ? k1 + i - 16 : k1 + i;
			fb1[k1][k11] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//2
		for (int k2 = 0; k2 < 16; k2++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[1], line_buffer, 1 + k2);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			int k22 = (k2 + i > 15) ? k2 + i - 16 : k2 + i;
			fb2[k2][k22] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}

		//-1
		for (int k3 = 0; k3 < 16; k3++)
		{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
			S24 pt0 = MAC_8TAP_FME(tbl_mc_l_coeff[2], line_buffer, k3);
			S12 pt1 = (pt0 + (1 << 11)) >> 12;
			int k33 = (k3 + i > 15) ? k3 + i - 16 : k3 + i;
			fb3[k3][k33] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt1);
		}
	}
}

void diff_line_16x16(int i, pel org[16], pel fb[16], S10 diff[16])
{
#pragma HLS INLINE OFF
	pel fb_temp[16];
	for (int j = 0; j < 16; j++)
	{
		int k = (i + j > 15) ? i + j - 16 : i + j;
		fb_temp[j] = fb[k];
	}
	for (int w = 0; w < 16; w++)
	{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
		diff[w] = org[w] - fb_temp[w];
	}
}


void hadamard_line_h_16x16(int i, S10 diff[16], S16 had[16][16])
{
	S16 m1[8], m2[8];
	S16 m1_2[8], m2_2[8];
	S16 had_temp[16];
#if FME_PRAGMA_ON	
#pragma HLS array_partition variable=diff complete dim=0
#pragma HLS array_partition variable=had complete dim=2

#pragma HLS array_partition variable=m1 complete dim=0
#pragma HLS array_partition variable=m2 complete dim=0
#pragma HLS array_partition variable=m1_2 complete dim=0
#pragma HLS array_partition variable=m2_2 complete dim=0
#pragma HLS array_partition variable=had_temp complete dim=0
#endif
		m2[0] = diff[0] + diff[4];
		m2[1] = diff[1] + diff[5];
		m2[2] = diff[2] + diff[6];
		m2[3] = diff[3] + diff[7];
		m2[4] = diff[0] - diff[4];
		m2[5] = diff[1] - diff[5];
		m2[6] = diff[2] - diff[6];
		m2[7] = diff[3] - diff[7];
		m1[0] = m2[0] + m2[2];
		m1[1] = m2[1] + m2[3];
		m1[2] = m2[0] - m2[2];
		m1[3] = m2[1] - m2[3];
		m1[4] = m2[4] + m2[6];
		m1[5] = m2[5] + m2[7];
		m1[6] = m2[4] - m2[6];
		m1[7] = m2[5] - m2[7];
		had_temp[0] = m1[0] + m1[1];
		had_temp[1] = m1[0] - m1[1];
		had_temp[2] = m1[2] + m1[3];
		had_temp[3] = m1[2] - m1[3];
		had_temp[4] = m1[4] + m1[5];
		had_temp[5] = m1[4] - m1[5];
		had_temp[6] = m1[6] + m1[7];
		had_temp[7] = m1[6] - m1[7];

		m2_2[0] = diff[8] + diff[12];
		m2_2[1] = diff[9] + diff[13];
		m2_2[2] = diff[10] + diff[14];
		m2_2[3] = diff[11] + diff[15];
		m2_2[4] = diff[8] - diff[12];
		m2_2[5] = diff[9] - diff[13];
		m2_2[6] = diff[10] - diff[14];
		m2_2[7] = diff[11] - diff[15];
		m1_2[0] = m2_2[0] + m2_2[2];
		m1_2[1] = m2_2[1] + m2_2[3];
		m1_2[2] = m2_2[0] - m2_2[2];
		m1_2[3] = m2_2[1] - m2_2[3];
		m1_2[4] = m2_2[4] + m2_2[6];
		m1_2[5] = m2_2[5] + m2_2[7];
		m1_2[6] = m2_2[4] - m2_2[6];
		m1_2[7] = m2_2[5] - m2_2[7];
		had_temp[8] = m1_2[0] + m1_2[1];
		had_temp[9] = m1_2[0] - m1_2[1];
		had_temp[10] = m1_2[2] + m1_2[3];
		had_temp[11] = m1_2[2] - m1_2[3];
		had_temp[12] = m1_2[4] + m1_2[5];
		had_temp[13] = m1_2[4] - m1_2[5];
		had_temp[14] = m1_2[6] + m1_2[7];
		had_temp[15] = m1_2[6] - m1_2[7];

		for (int j = 0; j < 16; j++)
		{
			int k = (i + j > 15) ? i + j - 16 : i + j;
			had[j][k] = had_temp[j];
		}
}


void hadamard_h_16x16(pel OrgY[16][16],
	pel fb03[16][16], pel fb00[16][16], pel fb01[16][16], pel fb02[16][16],
	pel fb13[16][16], pel fb10[16][16], pel fb11[16][16], pel fb12[16][16],
	pel fb23[16][16], pel fb20[16][16], pel fb21[16][16], pel fb22[16][16],
	pel fb33[16][16], pel fb30[16][16], pel fb31[16][16], pel fb32[16][16],
	S16 had03[16][16], S16 had00[16][16], S16 had01[16][16], S16 had02[16][16], 
	S16 had13[16][16], S16 had10[16][16], S16 had11[16][16], S16 had12[16][16],
	S16 had23[16][16], S16 had20[16][16], S16 had21[16][16], S16 had22[16][16], 
	S16 had33[16][16], S16 had30[16][16], S16 had31[16][16], S16 had32[16][16])
{
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=OrgY complete dim=2
#pragma HLS RESOURCE variable=OrgY core=RAM_1P_BRAM

#pragma HLS array_partition variable=fb33 complete dim=2
#pragma HLS array_partition variable=fb03 complete dim=2
#pragma HLS array_partition variable=fb13 complete dim=2
#pragma HLS array_partition variable=fb23 complete dim=2
#pragma HLS array_partition variable=fb30 complete dim=2
#pragma HLS array_partition variable=fb00 complete dim=2
#pragma HLS array_partition variable=fb10 complete dim=2
#pragma HLS array_partition variable=fb20 complete dim=2
#pragma HLS array_partition variable=fb31 complete dim=2
#pragma HLS array_partition variable=fb01 complete dim=2
#pragma HLS array_partition variable=fb11 complete dim=2
#pragma HLS array_partition variable=fb21 complete dim=2
#pragma HLS array_partition variable=fb32 complete dim=2
#pragma HLS array_partition variable=fb02 complete dim=2
#pragma HLS array_partition variable=fb12 complete dim=2
#pragma HLS array_partition variable=fb22 complete dim=2

#pragma HLS array_partition variable=had33 complete dim=2
#pragma HLS array_partition variable=had03 complete dim=2
#pragma HLS array_partition variable=had13 complete dim=2
#pragma HLS array_partition variable=had23 complete dim=2
#pragma HLS array_partition variable=had30 complete dim=2
#pragma HLS array_partition variable=had00 complete dim=2
#pragma HLS array_partition variable=had10 complete dim=2
#pragma HLS array_partition variable=had20 complete dim=2
#pragma HLS array_partition variable=had31 complete dim=2
#pragma HLS array_partition variable=had01 complete dim=2
#pragma HLS array_partition variable=had11 complete dim=2
#pragma HLS array_partition variable=had21 complete dim=2
#pragma HLS array_partition variable=had32 complete dim=2
#pragma HLS array_partition variable=had02 complete dim=2
#pragma HLS array_partition variable=had12 complete dim=2
#pragma HLS array_partition variable=had22 complete dim=2
#endif

	pel org[16];
	S10 diff03[16], diff00[16], diff01[16], diff02[16];
	S10 diff13[16], diff10[16], diff11[16], diff12[16];
	S10 diff23[16], diff20[16], diff21[16], diff22[16];
	S10 diff33[16], diff30[16], diff31[16], diff32[16];
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=org complete dim=0
#pragma HLS array_partition variable=diff03 complete dim=0
#pragma HLS array_partition variable=diff00 complete dim=0
#pragma HLS array_partition variable=diff01 complete dim=0
#pragma HLS array_partition variable=diff02 complete dim=0
#pragma HLS array_partition variable=diff13 complete dim=0
#pragma HLS array_partition variable=diff10 complete dim=0
#pragma HLS array_partition variable=diff11 complete dim=0
#pragma HLS array_partition variable=diff12 complete dim=0
#pragma HLS array_partition variable=diff23 complete dim=0
#pragma HLS array_partition variable=diff20 complete dim=0
#pragma HLS array_partition variable=diff21 complete dim=0
#pragma HLS array_partition variable=diff22 complete dim=0
#pragma HLS array_partition variable=diff33 complete dim=0
#pragma HLS array_partition variable=diff30 complete dim=0
#pragma HLS array_partition variable=diff31 complete dim=0
#pragma HLS array_partition variable=diff32 complete dim=0
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		orgline_copy_16x16(i, OrgY, org);

		diff_line_16x16(i, org, fb33[i], diff33);		
		diff_line_16x16(i, org, fb30[i], diff30);
		diff_line_16x16(i, org, fb31[i], diff31);
		diff_line_16x16(i, org, fb32[i], diff32);
		diff_line_16x16(i, org, fb03[i], diff03);
		diff_line_16x16(i, org, fb00[i], diff00);
		diff_line_16x16(i, org, fb01[i], diff01);
		diff_line_16x16(i, org, fb02[i], diff02);
		diff_line_16x16(i, org, fb13[i], diff13);
		diff_line_16x16(i, org, fb10[i], diff10);
		diff_line_16x16(i, org, fb11[i], diff11);
		diff_line_16x16(i, org, fb12[i], diff12);
		diff_line_16x16(i, org, fb23[i], diff23);
		diff_line_16x16(i, org, fb20[i], diff20);
		diff_line_16x16(i, org, fb21[i], diff21);
		diff_line_16x16(i, org, fb22[i], diff22);

		hadamard_line_h_16x16(i, diff33, had33);					
		hadamard_line_h_16x16(i, diff30, had30);
		hadamard_line_h_16x16(i, diff31, had31);
		hadamard_line_h_16x16(i, diff32, had32);
		hadamard_line_h_16x16(i, diff03, had03);
		hadamard_line_h_16x16(i, diff00, had00);
		hadamard_line_h_16x16(i, diff01, had01);
		hadamard_line_h_16x16(i, diff02, had02);
		hadamard_line_h_16x16(i, diff13, had13);
		hadamard_line_h_16x16(i, diff10, had10);
		hadamard_line_h_16x16(i, diff11, had11);
		hadamard_line_h_16x16(i, diff12, had12);
		hadamard_line_h_16x16(i, diff23, had23);
		hadamard_line_h_16x16(i, diff20, had20);
		hadamard_line_h_16x16(i, diff21, had21);
		hadamard_line_h_16x16(i, diff22, had22);

	}
	
}

void hadamard_line_v_16x16(int i, S16 had[16][16], U32 satdcost[2][16])
{
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=had complete dim=2
#endif
	S16 had_temp[16];
	S16 m1[8], m2[8];
	S16 m1_2[8], m2_2[8];
#if FME_PRAGMA_ON	
#pragma HLS array_partition variable=had_temp complete dim=0
#pragma HLS array_partition variable=m1 complete dim=0
#pragma HLS array_partition variable=m2 complete dim=0
#pragma HLS array_partition variable=m1_2 complete dim=0
#pragma HLS array_partition variable=m2_2 complete dim=0
#endif

	for (int j = 0; j < 16; j++)
	{
		int k = (i + j > 15) ? i + j - 16 : i + j;
		had_temp[j] = had[i][k];
	}

		m2[0] = had_temp[0] + had_temp[4];
		m2[1] = had_temp[1] + had_temp[5];
		m2[2] = had_temp[2] + had_temp[6];
		m2[3] = had_temp[3] + had_temp[7];
		m2[4] = had_temp[0] - had_temp[4];
		m2[5] = had_temp[1] - had_temp[5];
		m2[6] = had_temp[2] - had_temp[6];
		m2[7] = had_temp[3] - had_temp[7];
		m1[0] = m2[0] + m2[2];
		m1[1] = m2[1] + m2[3];
		m1[2] = m2[0] - m2[2];
		m1[3] = m2[1] - m2[3];
		m1[4] = m2[4] + m2[6];
		m1[5] = m2[5] + m2[7];
		m1[6] = m2[4] - m2[6];
		m1[7] = m2[5] - m2[7];
		m2[0] = m1[0] + m1[1];
		m2[1] = m1[0] - m1[1];
		m2[2] = m1[2] + m1[3];
		m2[3] = m1[2] - m1[3];
		m2[4] = m1[4] + m1[5];
		m2[5] = m1[4] - m1[5];
		m2[6] = m1[6] + m1[7];
		m2[7] = m1[6] - m1[7];

		m2_2[0] = had_temp[8] + had_temp[12];
		m2_2[1] = had_temp[9] + had_temp[13];
		m2_2[2] = had_temp[10] + had_temp[14];
		m2_2[3] = had_temp[11] + had_temp[15];
		m2_2[4] = had_temp[8] - had_temp[12];
		m2_2[5] = had_temp[9] - had_temp[13];
		m2_2[6] = had_temp[10] - had_temp[14];
		m2_2[7] = had_temp[11] - had_temp[15];
		m1_2[0] = m2_2[0] + m2_2[2];
		m1_2[1] = m2_2[1] + m2_2[3];
		m1_2[2] = m2_2[0] - m2_2[2];
		m1_2[3] = m2_2[1] - m2_2[3];
		m1_2[4] = m2_2[4] + m2_2[6];
		m1_2[5] = m2_2[5] + m2_2[7];
		m1_2[6] = m2_2[4] - m2_2[6];
		m1_2[7] = m2_2[5] - m2_2[7];
		m2_2[0] = m1_2[0] + m1_2[1];
		m2_2[1] = m1_2[0] - m1_2[1];
		m2_2[2] = m1_2[2] + m1_2[3];
		m2_2[3] = m1_2[2] - m1_2[3];
		m2_2[4] = m1_2[4] + m1_2[5];
		m2_2[5] = m1_2[4] - m1_2[5];
		m2_2[6] = m1_2[6] + m1_2[7];
		m2_2[7] = m1_2[6] - m1_2[7];

		U32 temp0 = 0, temp1 = 0;
		for (int j = 0; j < 8; j++)
		{
#if FME_PRAGMA_ON		
#pragma HLS UNROLL
#endif
#ifdef VIVADO_HLS
			temp0 += COM_ABS16(m2[j]);
			temp1 += COM_ABS16(m2_2[j]);			
#else
			temp0 += abs(m2[j]);
			temp1 += abs(m2_2[j]);
#endif			
		}
		satdcost[0][i] = temp0;
		satdcost[1][i] = temp1;
}


void hadamard_v_16x16(S16 had03[16][16], S16 had00[16][16], S16 had01[16][16], S16 had02[16][16],
	S16 had13[16][16], S16 had10[16][16], S16 had11[16][16], S16 had12[16][16],
	S16 had23[16][16], S16 had20[16][16], S16 had21[16][16], S16 had22[16][16],
	S16 had33[16][16], S16 had30[16][16], S16 had31[16][16], S16 had32[16][16], U32 satd[16][2][16])
{
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=had33 complete dim=2
#pragma HLS array_partition variable=had03 complete dim=2
#pragma HLS array_partition variable=had13 complete dim=2
#pragma HLS array_partition variable=had23 complete dim=2
#pragma HLS array_partition variable=had30 complete dim=2
#pragma HLS array_partition variable=had00 complete dim=2
#pragma HLS array_partition variable=had10 complete dim=2
#pragma HLS array_partition variable=had20 complete dim=2
#pragma HLS array_partition variable=had31 complete dim=2
#pragma HLS array_partition variable=had01 complete dim=2
#pragma HLS array_partition variable=had11 complete dim=2
#pragma HLS array_partition variable=had21 complete dim=2
#pragma HLS array_partition variable=had32 complete dim=2
#pragma HLS array_partition variable=had02 complete dim=2
#pragma HLS array_partition variable=had12 complete dim=2
#pragma HLS array_partition variable=had22 complete dim=2
#pragma HLS array_partition variable=satd complete dim=1
#endif

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		hadamard_line_v_16x16(i, had33, satd[0]);
		hadamard_line_v_16x16(i, had03, satd[1]);
		hadamard_line_v_16x16(i, had13, satd[2]);
		hadamard_line_v_16x16(i, had23, satd[3]);
		hadamard_line_v_16x16(i, had30, satd[4]);
		hadamard_line_v_16x16(i, had00, satd[5]);
		hadamard_line_v_16x16(i, had10, satd[6]);
		hadamard_line_v_16x16(i, had20, satd[7]);
		hadamard_line_v_16x16(i, had31, satd[8]);		
		hadamard_line_v_16x16(i, had01, satd[9]);
		hadamard_line_v_16x16(i, had11, satd[10]);
		hadamard_line_v_16x16(i, had21, satd[11]);
		hadamard_line_v_16x16(i, had32, satd[12]);
		hadamard_line_v_16x16(i, had02, satd[13]);	
		hadamard_line_v_16x16(i, had12, satd[14]);	
		hadamard_line_v_16x16(i, had22, satd[15]);
	}
}


void calculate_satdcost_8x8_1(pel org[8][8], pel ver_buf0[16][32], pel ver_buf1[16][32], U32 satdcost[16])
{
	pel block[8][8];
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=block complete dim=0
//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2

#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM

#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (i < 8)
		{
			for (int j = 0; j < 8; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif				
					if (j % 4 == 0) { 
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][0 + k];
					}
					else if (j % 4 == 1) {
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][8 + k];
					}
					else if (j % 4 == 2) {
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][16 + k];
					}
					else {
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][24 + k];
					}
				}
			}
		}
		else
		{
			for (int j = 0; j < 8; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					if (j % 4 == 0) { 
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][0 + k];
					}
					else if (j % 4 == 1) {
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][8 + k];
					}
					else if (j % 4 == 2) {
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][16 + k];
					}
					else {
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][24 + k];
					}
				}
			}
		}
		satdcost[i] = me_spel_satd_8x8(org, block);
	}
}

void calculate_satdcost_8x8_2(pel org[8][8], pel ver_buf0[16][32], pel ver_buf1[16][32], U32 satdcost[16])
{
	pel block[8][8];
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=block complete dim=0
#pragma HLS array_partition variable=org complete dim=0
#pragma HLS array_partition variable=satdcost complete dim=0
//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2

#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM

#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (i < 8)
		{
			for (int j = 0; j < 8; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					if (j % 4 == 0) { 
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][0 + k];
					}
					else if (j % 4 == 1) {
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][8 + k];
					}
					else if (j % 4 == 2) {
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][16 + k];
					}
					else {
						block[k][j] = ver_buf0[(i << 1) + (j >> 2)][24 + k];
					}
				}
			}
		}
		else
		{
			for (int j = 0; j < 8; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					if (j % 4 == 0) { 
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][0 + k];
					}
					else if (j % 4 == 1) {
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][8 + k];
					}
					else if (j % 4 == 2) {
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][16 + k];
					}
					else {
						block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 2)][24 + k];
					}
				}
			}
		}
		satdcost[i] = me_spel_satd_8x8(org, block);
	}
}

void calculate_satdcost_16x8_1(pel org[8][16], pel ver_buf0[8][64], pel ver_buf1[8][64], pel ver_buf2[8][64], pel ver_buf3[8][64], U32 satdcost[16])
{
#if FME_PRAGMA_ON
#pragma HLS INLINE OFF
#endif
	pel org_temp[8][8];
	pel block[8][16];
	pel block_temp[8][8];
	U32 satdcost_temp[2];
	U8 block_idx[16] = {
		0, 1, 4, 5,
		8, 9, 12, 13,
		2, 3, 6, 7,
		10, 11, 14, 15 };
#if FME_PRAGMA_ON
#pragma HLS ARRAY_PARTITION variable=org complete dim=0
#pragma HLS ARRAY_PARTITION variable=org_temp complete dim=0
#pragma HLS ARRAY_PARTITION variable=block complete dim=0
#pragma HLS ARRAY_PARTITION variable=block_temp complete dim=0
#pragma HLS ARRAY_PARTITION variable=satdcost_temp complete dim=0
#pragma HLS array_partition variable=satdcost complete dim=1

//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf2 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf3 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2
#pragma HLS array_partition variable=ver_buf2 complete dim=2
#pragma HLS array_partition variable=ver_buf3 complete dim=2

#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf3 core=RAM_1P_BRAM
#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (i < 4)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				//	block[k][j] = ver_buf3[(i << 1) + (j >> 3)][((j % 8) << 3) + k];
				   if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][0 + k]:
									    ver_buf3[(i << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][8 + k]:
									    ver_buf3[(i << 1) + 1][8 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][16 + k]:
									    ver_buf3[(i << 1) + 1][16 + k];
					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][24 + k]:
									    ver_buf3[(i << 1) + 1][24 + k];
					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][32 + k]:
									    ver_buf3[(i << 1) + 1][32 + k];	
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][40 + k]:
									    ver_buf3[(i << 1) + 1][40 + k];	
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][48 + k]:
									    ver_buf3[(i << 1) + 1][48 + k];	
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][56 + k]:
									    ver_buf3[(i << 1) + 1][56 + k];	
					}
				//	block[k][j] = (j<8)?ver_buf3[(i  << 1) + 0][((j % 8) << 3) + k]:
				//					    ver_buf3[(i  << 1) + 1][((j % 8) << 3) + k];
				}
			}
		}
		else if (i < 8)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					//block[k][j] = ver_buf0[((i - 4) << 1) + (j >> 3)][((j % 8) << 3) + k];
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][0 + k]:
									    ver_buf0[((i - 4) << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][8 + k]:
									    ver_buf0[((i - 4) << 1) + 1][8 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][16 + k]:
									    ver_buf0[((i - 4) << 1) + 1][16 + k];
					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][24 + k]:
									    ver_buf0[((i - 4) << 1) + 1][24 + k];
					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][32 + k]:
									    ver_buf0[((i - 4) << 1) + 1][32 + k];
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][40 + k]:
									    ver_buf0[((i - 4) << 1) + 1][40 + k];
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][48 + k]:
									    ver_buf0[((i - 4) << 1) + 1][48 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][56 + k]:
									    ver_buf0[((i - 4) << 1) + 1][56 + k];
					}

				//	block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][((j % 8) << 3) + k]:
				//					    ver_buf0[((i - 4) << 1) + 1][((j % 8) << 3) + k];
				}
			}
		}
		else if (i < 12)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				//	block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 3)][((j % 8) << 3) + k];
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][0 + k]:
									    ver_buf1[((i - 8) << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][8 + k]:
									    ver_buf1[((i - 8) << 1) + 1][8 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][16 + k]:
									    ver_buf1[((i - 8) << 1) + 1][16 + k];
					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][24 + k]:
									    ver_buf1[((i - 8) << 1) + 1][24 + k];
					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][32 + k]:
									    ver_buf1[((i - 8) << 1) + 1][32 + k];
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][40 + k]:
									    ver_buf1[((i - 8) << 1) + 1][40 + k];
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][48 + k]:
									    ver_buf1[((i - 8) << 1) + 1][48 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][56 + k]:
									    ver_buf1[((i - 8) << 1) + 1][56 + k];
					}
				//	block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][((j % 8) << 3) + k]:
				//					    ver_buf1[((i - 8) << 1) + 1][((j % 8) << 3) + k];
				}
			}
		}
		else
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				//	block[k][j] = ver_buf2[((i - 12) << 1) + (j >> 3)][((j % 8) << 3) + k];
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][0 + k]:
									    ver_buf2[((i - 12) << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][8 + k]:
									    ver_buf2[((i - 12) << 1) + 1][8 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][16 + k]:
									    ver_buf2[((i - 12) << 1) + 1][16 + k];
					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][24 + k]:
									    ver_buf2[((i - 12) << 1) + 1][24 + k];
					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][32 + k]:
									    ver_buf2[((i - 12) << 1) + 1][32 + k];
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][40 + k]:
									    ver_buf2[((i - 12) << 1) + 1][40 + k];
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][48 + k]:
									    ver_buf2[((i - 12) << 1) + 1][48 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][56 + k]:
									    ver_buf2[((i - 12) << 1) + 1][56 + k];
					}
				//	block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][((j % 8) << 3) + k]:
				//					    ver_buf2[((i - 12) << 1) + 1][((j % 8) << 3) + k];
				}
			}
		}

		for (int m = 0; m < 2; m++)
		{
			for (int n = 0; n < 8; n++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int p = 0; p < 8; p++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					org_temp[n][p] = org[n][(m << 3) + p];
					block_temp[n][p] = block[n][(m << 3) + p];

				}
			}
			satdcost_temp[m] = me_spel_satd_8x8(org_temp, block_temp);
		}
		satdcost[block_idx[i]] = satdcost_temp[0] + satdcost_temp[1];
	}
}


void calculate_satdcost_16x8_2(pel org[8][16], pel ver_buf0[8][64], pel ver_buf1[8][64], pel ver_buf2[8][64], pel ver_buf3[8][64], U32 satdcost[16])
{
	pel org_temp[8][8];
	pel block[8][16];
	pel block_temp[8][8];
	U32 satdcost_temp[2];
	U8 block_idx[16] = {
		0, 1, 4, 5,
		8, 9, 12, 13,
		2, 3, 6, 7,
		10, 11, 14, 15
};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=org_temp complete dim=0
#pragma HLS array_partition variable=block complete dim=0
#pragma HLS array_partition variable=block_temp complete dim=0
#pragma HLS array_partition variable=satdcost_temp complete dim=0
#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (i < 4)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf3[(i << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}
		else if (i < 8)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf0[((i - 4) << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}
		else if (i < 12)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}
		else
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf2[((i - 12) << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}

		for (int m = 0; m < 2; m++)
		{
			for (int n = 0; n < 8; n++)
			{
				for (int p = 0; p < 8; p++)
				{
					org_temp[n][p] = org[n][(m << 3) + p];
					block_temp[n][p] = block[n][(m << 3) + p];

				}
			}
			satdcost_temp[m] = me_spel_satd_8x8(org_temp, block_temp);
		}
		satdcost[block_idx[i]] = satdcost_temp[0] + satdcost_temp[1];
	}
}

void calculate_satdcost_16x8_3(pel org[8][16], pel ver_buf0[8][64], pel ver_buf1[8][64], pel ver_buf2[8][64], pel ver_buf3[8][64], U32 satdcost[16])
{
	pel org_temp[8][8];
	pel block[8][16];
	pel block_temp[8][8];
	U32 satdcost_temp[2];
	U8 block_idx[16] = {
		0, 1, 4, 5,
		8, 9, 12, 13,
		2, 3, 6, 7,
		10, 11, 14, 15
};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=org_temp complete dim=0
#pragma HLS array_partition variable=block complete dim=0
#pragma HLS array_partition variable=block_temp complete dim=0
#pragma HLS array_partition variable=satdcost_temp complete dim=0
#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (i < 4)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf3[(i << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}
		else if (i < 8)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf0[((i - 4) << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}
		else if (i < 12)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}
		else
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 8; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					block[k][j] = ver_buf2[((i - 12) << 1) + (j >> 3)][((j % 8) << 3) + k];
				}
			}
		}

		for (int m = 0; m < 2; m++)
		{
			for (int n = 0; n < 8; n++)
			{
				for (int p = 0; p < 8; p++)
				{
					org_temp[n][p] = org[n][(m << 3) + p];
					block_temp[n][p] = block[n][(m << 3) + p];

				}
			}
			satdcost_temp[m] = me_spel_satd_8x8(org_temp, block_temp);
		}
		satdcost[block_idx[i]] = satdcost_temp[0] + satdcost_temp[1];
	}
}

void calculate_satdcost_16x16_4(pel org[16][16], pel ver_buf0[8][128], pel ver_buf1[8][128], 
	pel ver_buf2[8][128], pel ver_buf3[8][128], U32 satdcost[16])
{
#if FME_PRAGMA_ON
#pragma HLS INLINE OFF
#endif
	pel org_temp[8][8];
	pel block[16][16];
	pel block_temp[8][8];
	U32 satdcost_temp[4];
	U8 block_idx[16] = {
		0, 1, 4, 5,
		8, 9, 12, 13,
		2, 3, 6, 7,
		10, 11, 14, 15
};
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=org_temp complete dim=0
#pragma HLS array_partition variable=org complete dim=0
#pragma HLS array_partition variable=block complete dim=2
#pragma HLS array_partition variable=block_temp complete dim=2
#pragma HLS array_partition variable=satdcost_temp complete dim=0

//#pragma HLS array_partition variable=ver_buf0 cyclic factor=32 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=32 dim=2
//#pragma HLS array_partition variable=ver_buf2 cyclic factor=32 dim=2
//#pragma HLS array_partition variable=ver_buf3 cyclic factor=32 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2
#pragma HLS array_partition variable=ver_buf2 complete dim=2
#pragma HLS array_partition variable=ver_buf3 complete dim=2
#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf3 core=RAM_1P_BRAM
#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (i < 4)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 16; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][0 + k]:
									    ver_buf3[(i << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][16 + k]:
									    ver_buf3[(i << 1) + 1][16 + k];

					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][32 + k]:
									    ver_buf3[(i << 1) + 1][32 + k];

					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][48 + k]:
									    ver_buf3[(i << 1) + 1][48 + k];
					
					}
					else if( j % 8 == 4){

					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][64 + k]:
									    ver_buf3[(i << 1) + 1][64 + k];
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][80 + k]:
									    ver_buf3[(i << 1) + 1][80 + k];
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][96 + k]:
									    ver_buf3[(i << 1) + 1][96 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf3[(i << 1) + 0][112 + k]:
									    ver_buf3[(i << 1) + 1][112 + k];
					}
				//	block[k][j] = ver_buf3[(i << 1) + (j >> 3)][((j % 8) << 4) + k];
				}
			}
		}
		else if (i < 8)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 16; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][0 + k]:
									    ver_buf0[((i - 4) << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][16 + k]:
									    ver_buf0[((i - 4) << 1) + 1][16 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][32 + k]:
									    ver_buf0[((i - 4) << 1) + 1][32 + k];

					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][48 + k]:
									    ver_buf0[((i - 4) << 1) + 1][48 + k];

					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][64 + k]:
									    ver_buf0[((i - 4) << 1) + 1][64 + k];
					
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][80 + k]:
									    ver_buf0[((i - 4) << 1) + 1][80 + k];
					
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][96 + k]:
									    ver_buf0[((i - 4) << 1) + 1][96 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf0[((i - 4) << 1) + 0][112 + k]:
									    ver_buf0[((i - 4) << 1) + 1][112 + k];
					}
				//	block[k][j] = ver_buf0[((i - 4) << 1) + (j >> 3)][((j % 8) << 4) + k];
				}
			}
		}
		else if (i < 12)
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 16; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][0 + k]:
									    ver_buf1[((i - 8) << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][16 + k]:
									    ver_buf1[((i - 8) << 1) + 1][16 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][32 + k]:
									    ver_buf1[((i - 8) << 1) + 1][32 + k];
					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][48 + k]:
									    ver_buf1[((i - 8) << 1) + 1][48 + k];
					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][64 + k]:
									    ver_buf1[((i - 8) << 1) + 1][64 + k];
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][80 + k]:
									    ver_buf1[((i - 8) << 1) + 1][80 + k];
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][96 + k]:
									    ver_buf1[((i - 8) << 1) + 1][96 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf1[((i - 8) << 1) + 0][112 + k]:
									    ver_buf1[((i - 8) << 1) + 1][112 + k];
					}
				//	block[k][j] = ver_buf1[((i - 8) << 1) + (j >> 3)][((j % 8) << 4) + k];
				}
			}
		}
		else
		{
			for (int j = 0; j < 16; j++)
			{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
				for (int k = 0; k < 16; k++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					if(j % 8 == 0) {
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][0 + k]:
									    ver_buf2[((i - 12) << 1) + 1][0 + k];
					}
					else if (j % 8 == 1){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][16 + k]:
									    ver_buf2[((i - 12) << 1) + 1][16 + k];
					}
					else if (j % 8 == 2){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][32 + k]:
									    ver_buf2[((i - 12) << 1) + 1][32 + k];
					}
					else if( j % 8 == 3){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][48 + k]:
									    ver_buf2[((i - 12) << 1) + 1][48 + k];
					}
					else if( j % 8 == 4){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][64 + k]:
									    ver_buf2[((i - 12) << 1) + 1][64 + k];
					}
					else if( j % 8 == 5){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][80 + k]:
									    ver_buf2[((i - 12) << 1) + 1][80 + k];
					}
					else if( j % 8 == 6){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][96 + k]:
									    ver_buf2[((i - 12) << 1) + 1][96 + k];
					}
					else if( j % 8 == 7){
					block[k][j] = (j<8)?ver_buf2[((i - 12) << 1) + 0][112 + k]:
									    ver_buf2[((i - 12) << 1) + 1][112 + k];
					}
				//	block[k][j] = ver_buf2[((i - 12) << 1) + (j >> 3)][((j % 8) << 4) + k];
				}
			}
		}

		for (int m = 0; m < 2; m++)
		{

			for (int n = 0; n < 2; n++)
			{
#if FME_PRAGMA_ON
#pragma HLS pipeline
#endif		
				for (int p = 0; p < 8; p++)
				{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
					for (int q = 0; q < 8; q++)
					{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
						org_temp[p][q] = org[(m << 3) + p][(n << 3) + q];
						block_temp[p][q] = block[(m << 3) + p][(n << 3) + q];

					}
				}
				satdcost_temp[m * 2 + n] = me_spel_satd_8x8(org_temp, block_temp);
			}			
		}
		satdcost[block_idx[i]] = satdcost_temp[0] + satdcost_temp[1] + satdcost_temp[2] + satdcost_temp[3];
	}
}


void calculate_satdcost_16x16(U32 satd_temp[16][2][16], U32 satdcost[16])
{
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=satd_temp complete dim=1
#pragma HLS array_partition variable=satdcost complete dim=0
#endif
	U32 temp0[16], temp1[16], temp2[16], temp3[16];
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=temp0 complete dim=0
#pragma HLS array_partition variable=temp1 complete dim=0
#pragma HLS array_partition variable=temp2 complete dim=0
#pragma HLS array_partition variable=temp3 complete dim=0
#endif

	for (int j = 0; j < 16; j++)
	{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		temp0[j] = 0;
		temp1[j] = 0;
		temp2[j] = 0;
		temp3[j] = 0;
	}

	for (int j = 0; j < 16; j++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		if (j < 8)
		{
			for (int k = 0; k < 16; k++)
			{
				temp0[k] += satd_temp[k][0][j];
				temp1[k] += satd_temp[k][1][j];
			}
		}
		else
		{
			for (int k = 0; k < 16; k++)
			{
				temp2[k] += satd_temp[k][0][j];
				temp3[k] += satd_temp[k][1][j];
			}
		}
	}

	for (int j = 0; j < 16; j++)
	{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif
		satdcost[j] = ((temp0[j] + 2) >> 2) + ((temp1[j] + 2) >> 2) + ((temp2[j] + 2) >> 2) + ((temp3[j] + 2) >> 2);
	}

}

__inline  U16 get_exp_golomb_bits(U14 abs_mvd)
{
	U16 bits = 0;
	U6 len_i, len_c;
	U15 nn;
	/* abs(mvd) */
	nn = ((abs_mvd + 1) >> 1);
	for (len_i = 0; len_i < 16; len_i++)
	{
		//#if FME_PRAGMA_ON	
#pragma HLS UNROLL
		//#endif
		if (nn == 0)break;
		nn >>= 1;
	}
	len_c = (len_i << 1) + 1;
	bits += len_c;
	/* sign */
	if (abs_mvd)
	{
		bits++;
	}
	return bits;
}

#if ENABLE_BFRAME	
static u8 get_enc_tbl_refi_bits(int a, int b)
{
	U8 enc_tbl_refi_bits[17][16] =
	{
		{ 0, },
		{ 0, },
		{ 1,  1, },
		{ 1,  2,  2, },
		{ 1,  2,  3,  3, },
		{ 1,  2,  3,  4,  4, },
		{ 1,  2,  3,  4,  5,  5, },
		{ 1,  2,  3,  4,  5,  6,  6, },
		{ 1,  2,  3,  4,  5,  6,  7,  7, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  8, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9,  9, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 10, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 12, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 13, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14, },
		{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 15, },
	};

	return enc_tbl_refi_bits[a][b];
}
#endif

U16 get_mv_bits_with_mvr(S14 mvd_x, S14 mvd_y, U5 num_refp, S8 refi, U2 mvr_idx)
{
#pragma HLS INLINE off
	U16 bits = 0;
	bits = get_exp_golomb_bits((U14)(COM_ABS14(mvd_x) >> mvr_idx));
	bits += get_exp_golomb_bits((U14)(COM_ABS14(mvd_y) >> mvr_idx));
#if ENABLE_BFRAME	
	bits += get_enc_tbl_refi_bits(num_refp, refi);
	if (mvr_idx == MAX_NUM_MVR - 1)
	{
		bits += mvr_idx;
	}
	else
	{
		bits += mvr_idx + 1;
	}
#else
	bits += mvr_idx + 1;
#endif
	return bits;
}

void cal_bitscost_1(S14 mvix, S14 mviy, S14 pmvx, S14 pmvy, U5 num_refp, U32 lambda_mv, U32 bitscost[16]
#if ENABLE_BFRAME			
	, U5 ime_num_refp_1, U1 lidx, S8 cur_refi
#endif
)
{
	U2 mvr_idx;
	U16 mv_bits;
	S3 x_offset, y_offset;
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		/* get MVD bits */
		if ((x_offset == 0) && (y_offset == 0))
		{
			mvr_idx = 2;
		}
		else if ((x_offset % 2 == 0) && (y_offset % 2 == 0))
		{
			mvr_idx = 1;
		}
		else {
			mvr_idx = 0;
		}

#if ENABLE_BFRAME			
		U5 num_refp_final;
		if (lidx == 0 && cur_refi == 0)
		{
			num_refp_final = num_refp;
		}
		else
		{
			num_refp_final = ime_num_refp_1;
		}
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp_final, cur_refi, mvr_idx);
#else
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp, 0, mvr_idx);
#endif
		// add inter_dir bits
		mv_bits += 2;
		/* get MVD cost_best */
		bitscost[i] = (U32)((lambda_mv * mv_bits + (U16)(1 << 15)) >> 16);
	}
}

void cal_bitscost_2(S14 mvix, S14 mviy, S14 pmvx, S14 pmvy, U5 num_refp, U32 lambda_mv, U32 bitscost[16]
#if ENABLE_BFRAME			
	, U5 ime_num_refp_1, U1 lidx, S8 cur_refi
#endif
)
{
	U2 mvr_idx;
	U16 mv_bits;
	S3 x_offset, y_offset;
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		/* get MVD bits */
		if ((x_offset == 0) && (y_offset == 0))
		{
			mvr_idx = 2;
		}
		else if ((x_offset % 2 == 0) && (y_offset % 2 == 0))
		{
			mvr_idx = 1;
		}
		else {
			mvr_idx = 0;
		}

#if ENABLE_BFRAME			
		U5 num_refp_final;
		if (lidx == 0 && cur_refi == 0)
		{
			num_refp_final = num_refp;
		}
		else
		{
			num_refp_final = ime_num_refp_1;
		}
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp_final, cur_refi, mvr_idx);
#else
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp, 0, mvr_idx);
#endif
		// add inter_dir bits
		mv_bits += 2;
		/* get MVD cost_best */
		bitscost[i] = (U32)((lambda_mv * mv_bits + (U16)(1 << 15)) >> 16);
	}
}

void cal_bitscost_3(S14 mvix, S14 mviy, S14 pmvx, S14 pmvy, U5 num_refp, U32 lambda_mv, U32 bitscost[16]
#if ENABLE_BFRAME			
	, U5 ime_num_refp_1, U1 lidx, S8 cur_refi
#endif
)
{
	U2 mvr_idx;
	U16 mv_bits;
	S3 x_offset, y_offset;
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		/* get MVD bits */
		if ((x_offset == 0) && (y_offset == 0))
		{
			mvr_idx = 2;
		}
		else if ((x_offset % 2 == 0) && (y_offset % 2 == 0))
		{
			mvr_idx = 1;
		}
		else {
			mvr_idx = 0;
		}

#if ENABLE_BFRAME			
		U5 num_refp_final;
		if (lidx == 0 && cur_refi == 0)
		{
			num_refp_final = num_refp;
		}
		else
		{
			num_refp_final = ime_num_refp_1;
		}
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp_final, cur_refi, mvr_idx);
#else
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp, 0, mvr_idx);
#endif
		// add inter_dir bits
		mv_bits += 2;
		/* get MVD cost_best */
		bitscost[i] = (U32)((lambda_mv * mv_bits + (U16)(1 << 15)) >> 16);
	}
}

void cal_bitscost_4(S14 mvix, S14 mviy, S14 pmvx, S14 pmvy, U5 num_refp, U32 lambda_mv, U32 bitscost[16]
#if ENABLE_BFRAME			
	, U5 ime_num_refp_1, U1 lidx, S8 cur_refi
#endif
)
{
	U2 mvr_idx;
	U16 mv_bits;
	S3 x_offset, y_offset;
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		/* get MVD bits */
		if ((x_offset == 0) && (y_offset == 0))
		{
			mvr_idx = 2;
		}
		else if ((x_offset % 2 == 0) && (y_offset % 2 == 0))
		{
			mvr_idx = 1;
		}
		else {
			mvr_idx = 0;
		}

#if ENABLE_BFRAME			
		U5 num_refp_final;
		if (lidx == 0 && cur_refi == 0)
		{
			num_refp_final = num_refp;
		}
		else
		{
			num_refp_final = ime_num_refp_1;
		}
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp_final, cur_refi, mvr_idx);
#else
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp, 0, mvr_idx);
#endif
		// add inter_dir bits
		mv_bits += 2;
		/* get MVD cost_best */
		bitscost[i] = (U32)((lambda_mv * mv_bits + (U16)(1 << 15)) >> 16);
	}
}

void cal_bitscost_5(S14 mvix, S14 mviy, S14 pmvx, S14 pmvy, U5 num_refp, U32 lambda_mv, U32 bitscost[16]
#if ENABLE_BFRAME			
	, U5 ime_num_refp_1, U1 lidx, S8 cur_refi
#endif
)
{
	U2 mvr_idx;
	U16 mv_bits;
	S3 x_offset, y_offset;
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif
		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		/* get MVD bits */
		if ((x_offset == 0) && (y_offset == 0))
		{
			mvr_idx = 2;
		}
		else if ((x_offset % 2 == 0) && (y_offset % 2 == 0))
		{
			mvr_idx = 1;
		}
		else {
			mvr_idx = 0;
		}

#if ENABLE_BFRAME			
		U5 num_refp_final;
		if (lidx == 0 && cur_refi == 0)
		{
			num_refp_final = num_refp;
		}
		else
		{
			num_refp_final = ime_num_refp_1;
		}
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp_final, cur_refi, mvr_idx);
#else
		mv_bits = get_mv_bits_with_mvr((S14)(mvix + x_offset - pmvx), (S14)(mviy + y_offset - pmvy), num_refp, 0, mvr_idx);
#endif
		// add inter_dir bits
		mv_bits += 2;
		/* get MVD cost_best */
		bitscost[i] = (U32)((lambda_mv * mv_bits + (U16)(1 << 15)) >> 16);
	}
}

U1 mv_inbound_s(S15 mv_x, S15 mv_y, S14 ctrpos1, S14 ctrpos2,S14 ctrpos3, S14 ctrpos4, U7 cu_width, U7 cu_height) {

	S4 x_offset_min, x_offset_max;
	S4 y_offset_min, y_offset_max;
	U2 pattern_x = mv_x & (U2)0x3;
	U2 pattern_y = mv_y & (U2)0x3;

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

	if (x_offset_min + (mv_x >> 2) >= ctrpos1 && x_offset_max + (mv_x >> 2) + cu_width <= ctrpos2 &&
		y_offset_min + (mv_y >> 2) >= ctrpos3 && y_offset_max + (mv_y >> 2) + cu_height <= ctrpos4)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


U1 mv_inbound(S15 mv_x, S15 mv_y, S14 ctrpos[2][2][2], U7 cu_width, U7 cu_height) {

	S4 x_offset_min, x_offset_max;
	S4 y_offset_min, y_offset_max;
	U2 pattern_x = mv_x & (U2)0x3;
	U2 pattern_y = mv_y & (U2)0x3;

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

	if (x_offset_min + (mv_x >> 2) >= ctrpos[RANGE_MIN][MV_X][PIC_DIM] && x_offset_max + (mv_x >> 2) + cu_width <= ctrpos[RANGE_MAX][MV_X][PIC_DIM] &&
		y_offset_min + (mv_y >> 2) >= ctrpos[RANGE_MIN][MV_Y][PIC_DIM] && y_offset_max + (mv_y >> 2) + cu_height <= ctrpos[RANGE_MAX][MV_Y][PIC_DIM])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void cost_compare_16(U32 cost_accu[16], S3 *x_offset, S3 *y_offset)
{
	//#pragma HLS INLINE OFF
#pragma HLS INLINE
	U32 cost_temp;
	U32 cost_best;
	cost_best = (U32)0xFFFFFFFF;
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
//#pragma HLS pipeline
#endif
		cost_temp = cost_accu[i];
		if (cost_temp < cost_best)
		{
			cost_best = cost_temp;
			*x_offset = i % 4;
			*y_offset = i / 4;
		}
	}
}

void cal_costbest_8x8_8x16(int flag, U32 satdcost[16], U32 bitscost[16], U32 cost_accu[16],
	S14 sadTree8x8Fmv[8][8][2], S14 sadTree8x16Fmv[8][4][2],	
	U1 ref_flag, U12 x, U12 y, U7 w, U7 h, S14 mvix, S14 mviy, S14 ctrpos[2][2][2]
#if ENABLE_BFRAME			
	, S14 ime_CtrPos_1[2][2][2], U1 lidx, S8 cur_refi, ME_MV_DIR *me_mv_dir
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
	S14 fmvx, fmvy;
	U1 inbound_flag;
	S3 x_offset, y_offset;
	S15 mv_x, mv_y;
	S15 cx, cy;
	cx = mvix + ((U14)x << 2);
	cy = mviy + ((U14)y << 2);
#if FME_PRAGMA_ON
#pragma HLS ARRAY_PARTITION variable=satdcost complete dim=1
#pragma HLS ARRAY_PARTITION variable=bitscost complete dim=1
#pragma HLS ARRAY_PARTITION variable=cost_accu complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=2
#endif
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=1
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=2
#endif
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#endif

		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		mv_x = cx + x_offset;
		mv_y = cy + y_offset;
#if ENABLE_BFRAME	
		if (lidx == 0 && cur_refi == 0)
		{
#endif
		inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ctrpos[0][0][0], ctrpos[1][0][0],ctrpos[0][1][0], ctrpos[1][1][0], 
				w, h);
		//	inbound_flag = mv_inbound(mv_x, mv_y, ctrpos, w, h);
#if ENABLE_BFRAME	
		}
		else
		{
			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ime_CtrPos_1[0][0][0], ime_CtrPos_1[1][0][0], ime_CtrPos_1[0][1][0], ime_CtrPos_1[1][1][0],
				w, h);
		//	inbound_flag = mv_inbound(mv_x, mv_y, ime_CtrPos_1, w, h);
		}
#endif

		if (inbound_flag && ref_flag)
		{
			if ((flag < 64) || (flag % 2 == 0)) {
				cost_accu[i] = satdcost[i];
				cost_accu[i] += bitscost[i];
			}
			else {
				cost_accu[i] += satdcost[i];
			}
		}
		else {
			cost_accu[i] = (U32)0xFFFFFFFF;
		}
	}


	x_offset = 1;
	y_offset = 1;
	// compare 16 costs and get best cost
	cost_compare_16(cost_accu, &x_offset, &y_offset);

	fmvx = mvix + x_offset - 1;
	fmvy = mviy + y_offset - 1;
#if USE_INTRA_REFRESH
	U12 cuPelX, cuPelY;
	S12 safeX, safeY, maxSafeMV;
	S12 temp_mvx, temp_mvy;

	temp_mvx = fmvx;
	temp_mvy = fmvy;

	if (ime_out_para_set->EncRefresh == 1)
	{
		if (ime_out_para_set->RefreshMode == 0)
		{
			cuPelY = y;
			if (cuPelY < ime_out_para_set->pirStartRow * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndRow < ime_out_para_set->numCuInHeight)
			{
				safeY = ime_out_para_set->pirEndRow * 32 - 4;
				maxSafeMV = (safeY - cuPelY) * 4;
				if (maxSafeMV >= 0 && temp_mvy >= maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvy < maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
			}
		}
		else
		{
			cuPelX = x;
			if (cuPelX < ime_out_para_set->pirStartCol * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndCol < ime_out_para_set->numCuInWidth)
			{
				safeX = ime_out_para_set->pirEndCol * 32 - 4;
				maxSafeMV = (safeX - cuPelX) * 4;
				if (maxSafeMV >= 0 && temp_mvx >= maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvx < maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
			}
		}
	}

	fmvx = temp_mvx;
	fmvy = temp_mvy;
#endif
	if (flag < 64)
	{
		sadTree8x8Fmv[flag / 8][flag % 8][MV_X] = fmvx;
		sadTree8x8Fmv[flag / 8][flag % 8][MV_Y] = fmvy;

	}
	else 
	{
		sadTree8x16Fmv[(flag - 64) / 8][((flag - 64) / 2) % 4][MV_X] = fmvx;
		sadTree8x16Fmv[(flag - 64) / 8][((flag - 64) / 2) % 4][MV_Y] = fmvy;

	}

}

void cal_costbest_8x32_8x64(int flag, U32 satdcost[16], U32 bitscost[16], U32 cost_accu[16],
	S14 sadTree8x32Fmv[8][2][2], S14 sadTree8x64Fmv[8][1][2], U1 ref_flag, U12 x, U12 y, U7 w, U7 h, S14 mvix, S14 mviy, S14 ctrpos[2][2][2]
#if ENABLE_BFRAME			
	, S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS], U1 lidx, S8 cur_refi, ME_MV_DIR *me_mv_dir
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
	S14 fmvx, fmvy;
	U1 inbound_flag;
	S3 x_offset, y_offset;
	S15 mv_x, mv_y;
	S15 cx, cy;
	cx = mvix + ((U14)x << 2);
	cy = mviy + ((U14)y << 2);

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=satdcost complete dim=1
#pragma HLS ARRAY_PARTITION variable=bitscost complete dim=1
#pragma HLS ARRAY_PARTITION variable=cost_accu complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=2
#endif
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=1
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=2
#endif

		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		mv_x = cx + x_offset;
		mv_y = cy + y_offset;
#if ENABLE_BFRAME	
		if (lidx == 0 && cur_refi == 0)
		{
#endif
			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ctrpos[0][0][0], ctrpos[1][0][0], ctrpos[0][1][0], ctrpos[1][1][0],
				w, h); 
		//	inbound_flag = mv_inbound(mv_x, mv_y, ctrpos, w, h);
#if ENABLE_BFRAME	
		}
		else
		{
			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ime_CtrPos_1[0][0][0], ime_CtrPos_1[1][0][0], ime_CtrPos_1[0][1][0], ime_CtrPos_1[1][1][0],
				w, h);
		//	inbound_flag = mv_inbound(mv_x, mv_y, ime_CtrPos_1, w, h);
		}
#endif

		if (inbound_flag && ref_flag)
		{
			if ((flag < 64 && flag % 4 == 0) || (flag % 8 == 0)) {
				cost_accu[i] = satdcost[i];
				cost_accu[i] += bitscost[i];
			}
			else {
				cost_accu[i] += satdcost[i];
			}
		}
		else {
			cost_accu[i] = (U32)0xFFFFFFFF;
		}
	}


	x_offset = 1;
	y_offset = 1;
	// compare 16 costs and get best cost
	cost_compare_16(cost_accu, &x_offset, &y_offset);

	fmvx = mvix + x_offset - 1;
	fmvy = mviy + y_offset - 1;
#if USE_INTRA_REFRESH
	U12 cuPelX, cuPelY;
	S12 safeX, safeY, maxSafeMV;
	S12 temp_mvx, temp_mvy;

	temp_mvx = fmvx;
	temp_mvy = fmvy;

	if (ime_out_para_set->EncRefresh == 1)
	{
		if (ime_out_para_set->RefreshMode == 0)
		{
			cuPelY = y;
			if (cuPelY < ime_out_para_set->pirStartRow * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndRow < ime_out_para_set->numCuInHeight)
			{
				safeY = ime_out_para_set->pirEndRow * 32 - 4;
				maxSafeMV = (safeY - cuPelY) * 4;
				if (maxSafeMV >= 0 && temp_mvy >= maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvy < maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
			}
		}
		else
		{
			cuPelX = x;
			if (cuPelX < ime_out_para_set->pirStartCol * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndCol < ime_out_para_set->numCuInWidth)
			{
				safeX = ime_out_para_set->pirEndCol * 32 - 4;
				maxSafeMV = (safeX - cuPelX) * 4;
				if (maxSafeMV >= 0 && temp_mvx >= maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvx < maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
			}
		}
	}

	fmvx = temp_mvx;
	fmvy = temp_mvy;
#endif
	
	if (flag < 64)
	{
		sadTree8x32Fmv[flag / 8][(flag / 4) % 2][MV_X] = fmvx;
		sadTree8x32Fmv[flag / 8][(flag / 4) % 2][MV_Y] = fmvy;
	}
	else
	{
		sadTree8x64Fmv[(flag - 64) / 8][0][MV_X] = fmvx;
		sadTree8x64Fmv[(flag - 64) / 8][0][MV_Y] = fmvy;
	}

}
void cal_costbest_16x8_32x8_64x8_64x64(int flag, U32 satdcost[16], U32 bitscost[16], U32 cost_accu[16],
	S14 sadTree16x8Fmv[4][8][2], 
	S14 sadTree32x8Fmv[2][8][2], S14 sadTree64x8Fmv[1][8][2], S14 sadTree64x64Fmv[1][1][2],
	U1 ref_flag, U12 x, U12 y, U7 w, U7 h, S14 mvix, S14 mviy, S14 ctrpos[2][2][2]
#if ENABLE_BFRAME			
	, S14 ime_CtrPos_1[2][2][2], U1 lidx, S8 cur_refi, ME_MV_DIR* me_mv_dir
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
	S14 fmvx, fmvy;
	U1 inbound_flag;
	S3 x_offset, y_offset;
	S15 mv_x, mv_y;
	S15 cx, cy;
	cx = mvix + ((U14)x << 2);
	cy = mviy + ((U14)y << 2);
	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
//#pragma HLS UNROLL
#pragma HLS pipeline
#pragma HLS ARRAY_PARTITION variable=satdcost complete dim=1
#pragma HLS ARRAY_PARTITION variable=bitscost complete dim=1
#pragma HLS ARRAY_PARTITION variable=cost_accu complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=2
#endif
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=1
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=2
#endif

		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		mv_x = cx + x_offset;
		mv_y = cy + y_offset;
#if ENABLE_BFRAME	
		if (lidx == 0 && cur_refi == 0)
		{
#endif

			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ctrpos[0][0][0], ctrpos[1][0][0], ctrpos[0][1][0], ctrpos[1][1][0],
				w, h);
			//inbound_flag = mv_inbound(mv_x, mv_y, ctrpos, w, h);
#if ENABLE_BFRAME	
		}
		else
		{
			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ime_CtrPos_1[0][0][0], ime_CtrPos_1[1][0][0], ime_CtrPos_1[0][1][0], ime_CtrPos_1[1][1][0],
				w, h);
			//inbound_flag = mv_inbound(mv_x, mv_y, ime_CtrPos_1, w, h);
		}
#endif

		if (inbound_flag && ref_flag)
		{
			if ((flag < 32) || (flag < 64 && flag % 2 == 0) || (flag < 96 && flag % 4 == 0) || (flag == 96)) {
				cost_accu[i] = satdcost[i];
				cost_accu[i] += bitscost[i];
			}
			else {
				cost_accu[i] += satdcost[i];
			}
		}
		else {
			cost_accu[i] = (U32)0xFFFFFFFF;
		}
	}


	x_offset = 1;
	y_offset = 1;
	// compare 16 costs and get best cost
	cost_compare_16(cost_accu, &x_offset, &y_offset);

	fmvx = mvix + x_offset - 1;
	fmvy = mviy + y_offset - 1;
	
#if USE_INTRA_REFRESH
	U12 cuPelX, cuPelY;
	S12 safeX, safeY, maxSafeMV;
	S12 temp_mvx, temp_mvy;

	temp_mvx = fmvx;
	temp_mvy = fmvy;

	if (ime_out_para_set->EncRefresh == 1)
	{
		if (ime_out_para_set->RefreshMode == 0)
		{
			cuPelY = y;
			if (cuPelY < ime_out_para_set->pirStartRow * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndRow < ime_out_para_set->numCuInHeight)
			{
				safeY = ime_out_para_set->pirEndRow * 32 - 4;
				maxSafeMV = (safeY - cuPelY) * 4;
				if (maxSafeMV >= 0 && temp_mvy >= maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvy < maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
			}
		}
		else
		{
			cuPelX = x;
			if (cuPelX < ime_out_para_set->pirStartCol * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndCol < ime_out_para_set->numCuInWidth)
			{
				safeX = ime_out_para_set->pirEndCol * 32 - 4;
				maxSafeMV = (safeX - cuPelX) * 4;
				if (maxSafeMV >= 0 && temp_mvx >= maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvx < maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
			}
		}
	}

	fmvx = temp_mvx;
	fmvy = temp_mvy;
#endif

	if (flag < 32)
	{
		sadTree16x8Fmv[flag >> 3][flag % 8][MV_X] = fmvx;
		sadTree16x8Fmv[flag >> 3][flag % 8][MV_Y] = fmvy;
	}
	else if (flag < 64)
	{
		sadTree32x8Fmv[((flag - 32) >> 1) % 2][(flag - 32) >> 2][MV_X] = fmvx;
		sadTree32x8Fmv[((flag - 32) >> 1) % 2][(flag - 32) >> 2][MV_Y] = fmvy;
	}
	else if (flag < 96)
	{
		sadTree64x8Fmv[0][(flag - 64) >> 2][MV_X] = fmvx;
		sadTree64x8Fmv[0][(flag - 64) >> 2][MV_Y] = fmvy;
	}
	else
	{
		sadTree64x64Fmv[0][0][MV_X] = fmvx;
		sadTree64x64Fmv[0][0][MV_Y] = fmvy;
	}

}

/*
void cal_costbest_16x16_16x32_16x64_32x32(int flag, U32 satdcost[16], U32 bitscost[16], U32 cost_accu[16],
	S14 sadTree16x16Fmv[4][4][2], S14 sadTree16x32Fmv[4][2][2],
	S14 sadTree16x64Fmv[4][1][2], S14 sadTree32x32Fmv[2][2][2], 
	U1 ref_flag, U12 x, U12 y, U7 w, U7 h, S14 mvix, S14 mviy, S14 ctrpos[2][2][2]
#if ENABLE_BFRAME			
	, S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS], U1 lidx, S8 cur_refi
#endif
)
{
	S14 fmvx, fmvy;
	U1 inbound_flag;
	S3 x_offset, y_offset;
	S15 mv_x, mv_y;
	S15 cx, cy;
	cx = mvix + ((U14)x << 2);
	cy = mviy + ((U14)y << 2);

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif

		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		mv_x = cx + x_offset;
		mv_y = cy + y_offset;
#if ENABLE_BFRAME	
		if (lidx == 0 && cur_refi == 0)
		{
#endif
			inbound_flag = mv_inbound(mv_x, mv_y, ctrpos, w, h);
#if ENABLE_BFRAME	
		}
		else
		{
			inbound_flag = mv_inbound(mv_x, mv_y, ime_CtrPos_1, w, h);
		}
#endif

		if (inbound_flag && ref_flag)
		{
			if ((flag < 32 && flag % 2 == 0) || (flag < 64 && flag % 4 == 0) || (flag % 8 == 0)) {
				cost_accu[i] = satdcost[i];
				cost_accu[i] += bitscost[i];
			}
			else {
				cost_accu[i] += satdcost[i];
			}
		}
		else {
			cost_accu[i] = (U32)0xFFFFFFFF;
		}
	}


	x_offset = 1;
	y_offset = 1;
	// compare 16 costs and get best cost
	cost_compare_16(cost_accu, &x_offset, &y_offset);

	fmvx = mvix + x_offset - 1;
	fmvy = mviy + y_offset - 1;

	if (flag < 32)
	{
		sadTree16x16Fmv[flag >> 3][(flag / 2) % 4][MV_X] = fmvx;
		sadTree16x16Fmv[flag >> 3][(flag / 2) % 4][MV_Y] = fmvy;
	}
	else if (flag < 64)
	{
		sadTree16x32Fmv[(flag - 32) >> 3][((flag - 32) / 4) % 2][MV_X] = fmvx;
		sadTree16x32Fmv[(flag - 32) >> 3][((flag - 32) / 4) % 2][MV_Y] = fmvy;
	}
	else if (flag < 96)
	{
		sadTree16x64Fmv[(flag - 64) >> 3][0][MV_X] = fmvx;
		sadTree16x64Fmv[(flag - 64) >> 3][0][MV_Y] = fmvy;
	}
	else
	{
		sadTree32x32Fmv[(flag - 96) >> 4][(((flag - 96) >> 3) % 2)][MV_X] = fmvx;
		sadTree32x32Fmv[(flag - 96) >> 4][(((flag - 96) >> 3) % 2)][MV_Y] = fmvy;
	}

}

void cal_costbest_32x16_64x16_32x64_64x32(int flag, U32 satdcost[16], U32 bitscost[16], U32 cost_accu[16],
	S14 sadTree32x16Fmv[2][4][2], S14 sadTree64x16Fmv[1][4][2],
	S14 sadTree32x64Fmv[2][1][2], S14 sadTree64x32Fmv[1][2][2],
	U1 ref_flag, U12 x, U12 y, U7 w, U7 h, S14 mvix, S14 mviy, S14 ctrpos[2][2][2]
#if ENABLE_BFRAME			
	, S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS], U1 lidx, S8 cur_refi
#endif
)
{
	S14 fmvx, fmvy;
	U1 inbound_flag;
	S3 x_offset, y_offset;
	S15 mv_x, mv_y;
	S15 cx, cy;
	cx = mvix + ((U14)x << 2);
	cy = mviy + ((U14)y << 2);

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS UNROLL
#endif

		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		mv_x = cx + x_offset;
		mv_y = cy + y_offset;
#if ENABLE_BFRAME	
		if (lidx == 0 && cur_refi == 0)
		{
#endif
			inbound_flag = mv_inbound(mv_x, mv_y, ctrpos, w, h);
#if ENABLE_BFRAME	
		}
		else
		{
			inbound_flag = mv_inbound(mv_x, mv_y, ime_CtrPos_1, w, h);
		}
#endif

		if (inbound_flag && ref_flag)
		{
			if ((flag < 32 && flag % 4 == 0) || (flag < 64 && flag % 8 == 0) || (flag % 16 == 0)) {
				cost_accu[i] = satdcost[i];
				cost_accu[i] += bitscost[i];
			}
			else {
				cost_accu[i] += satdcost[i];
			}
		}
		else {
			cost_accu[i] = (U32)0xFFFFFFFF;
		}
	}


	x_offset = 1;
	y_offset = 1;
	// compare 16 costs and get best cost
	cost_compare_16(cost_accu, &x_offset, &y_offset);

	fmvx = mvix + x_offset - 1;
	fmvy = mviy + y_offset - 1;
	if (flag < 32)
	{
		sadTree32x16Fmv[flag >> 4][(flag >> 2) % 4][MV_X] = fmvx;
		sadTree32x16Fmv[flag >> 4][(flag >> 2) % 4][MV_Y] = fmvy;
	}
	else if (flag < 64)
	{
		sadTree64x16Fmv[0][(flag - 32) >> 3][MV_X] = fmvx;
		sadTree64x16Fmv[0][(flag - 32) >> 3][MV_Y] = fmvy;
	}
	else if (flag < 96)
	{
		sadTree32x64Fmv[(flag - 64) >> 4][0][MV_X] = fmvx;
		sadTree32x64Fmv[(flag - 64) >> 4][0][MV_Y] = fmvy;
	}
	else
	{
		sadTree64x32Fmv[0][(flag - 96) >> 4][MV_X] = fmvx;
		sadTree64x32Fmv[0][(flag - 96) >> 4][MV_Y] = fmvy;
	}

}

*/

void cal_costbest_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32(
	int flag, U32 satdcost[16], U32 bitscost[16], U32 cost_accu[16],
	U1 ref_flag, U12 x, U12 y, U7 w, U7 h, S14 mvix, S14 mviy, S14 ctrpos[2][2][2],
	S14 sadTree16x16Fmv[4][4][2], S14 sadTree32x32Fmv[2][2][2],
	S14 sadTree16x32Fmv[4][2][2], S14 sadTree32x16Fmv[2][4][2],
	S14 sadTree16x64Fmv[4][1][2], S14 sadTree64x16Fmv[1][4][2],
	S14 sadTree32x64Fmv[2][1][2], S14 sadTree64x32Fmv[1][2][2]
#if ENABLE_BFRAME			
	,S14 ime_CtrPos_1[2][2][2], U1 lidx, S8 cur_refi
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
	S14 fmvx, fmvy;
	U1 inbound_flag;
	S3 x_offset, y_offset;
	S15 mv_x, mv_y;
	S15 cx, cy;
	cx = mvix + ((U14)x << 2);
	cy = mviy + ((U14)y << 2);

	for (int i = 0; i < 16; i++)
	{
#if FME_PRAGMA_ON
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION variable=satdcost complete dim=1
#pragma HLS ARRAY_PARTITION variable=bitscost complete dim=1
#pragma HLS ARRAY_PARTITION variable=cost_accu complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=ime_CtrPos_1 complete dim=2
#endif
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=1
#pragma HLS ARRAY_PARTITION variable=ctrpos complete dim=2
#endif

		x_offset = i % 4 - 1;
		y_offset = i / 4 - 1;
		mv_x = cx + x_offset;
		mv_y = cy + y_offset;
#if ENABLE_BFRAME	
		if (lidx == 0 && cur_refi == 0)
		{
#endif
			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ctrpos[0][0][0], ctrpos[1][0][0], ctrpos[0][1][0], ctrpos[1][1][0],
				w, h);
		//	inbound_flag = mv_inbound(mv_x, mv_y, ctrpos, w, h);
#if ENABLE_BFRAME	
		}
		else
		{
			inbound_flag = mv_inbound_s(
				mv_x, mv_y,
				ime_CtrPos_1[0][0][0], ime_CtrPos_1[1][0][0], ime_CtrPos_1[0][1][0], ime_CtrPos_1[1][1][0],
				w, h);
		//	inbound_flag = mv_inbound(mv_x, mv_y, ime_CtrPos_1, w, h);
		}
#endif
		if (inbound_flag && ref_flag)
		{
			if ((flag < 16) || (flag < 32 && flag % 4 == 0) || (flag > 32 && flag < 64 && flag % 2 == 0) ||
				(flag < 96 && flag % 4 == 0) || (flag % 8 == 0))
			{
				cost_accu[i] = satdcost[i];
				cost_accu[i] += bitscost[i];
			}
			else {
				cost_accu[i] += satdcost[i];
			}
		}
		else {
			cost_accu[i] = (U32)0xFFFFFFFF;
		}
	}


	x_offset = 1;
	y_offset = 1;
	// compare 16 costs and get best cost
	cost_compare_16(cost_accu, &x_offset, &y_offset);

	fmvx = mvix + x_offset - 1;
	fmvy = mviy + y_offset - 1;
#if USE_INTRA_REFRESH
	U12 cuPelX, cuPelY;
	S12 safeX, safeY, maxSafeMV;
	S12 temp_mvx, temp_mvy;

	temp_mvx = fmvx;
	temp_mvy = fmvy;

	if (ime_out_para_set->EncRefresh == 1)
	{
		if (ime_out_para_set->RefreshMode == 0)
		{
			cuPelY = y;
			if (cuPelY < ime_out_para_set->pirStartRow * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndRow < ime_out_para_set->numCuInHeight)
			{
				safeY = ime_out_para_set->pirEndRow * 32 - 4;
				maxSafeMV = (safeY - cuPelY) * 4;
				if (maxSafeMV >= 0 && temp_mvy >= maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvy < maxSafeMV)
				{
					temp_mvy = maxSafeMV;
				}
			}
		}
		else
		{
			cuPelX = x;
			if (cuPelX < ime_out_para_set->pirStartCol * 32 * ime_out_para_set->RefreshSize && ime_out_para_set->pirEndCol < ime_out_para_set->numCuInWidth)
			{
				safeX = ime_out_para_set->pirEndCol * 32 - 4;
				maxSafeMV = (safeX - cuPelX) * 4;
				if (maxSafeMV >= 0 && temp_mvx >= maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
				if (maxSafeMV < 0 && temp_mvx < maxSafeMV)
				{
					temp_mvx = maxSafeMV;
				}
			}
		}
	}

	fmvx = temp_mvx;
	fmvy = temp_mvy;
#endif
	if (flag < 16)
	{
		sadTree16x16Fmv[flag >> 2][flag % 4][MV_X] = fmvx;
		sadTree16x16Fmv[flag >> 2][flag % 4][MV_Y] = fmvy;
	}
	else if (flag < 32)
	{
		sadTree32x32Fmv[(flag - 16) >> 3][((flag - 16) >> 2) % 2][MV_X] = fmvx;
		sadTree32x32Fmv[(flag - 16) >> 3][((flag - 16) >> 2) % 2][MV_Y] = fmvy;
	}
	else if (flag < 48)
	{
		sadTree16x32Fmv[(flag - 32) >> 2][((flag - 32) >> 1) % 2][MV_X] = fmvx;
		sadTree16x32Fmv[(flag - 32) >> 2][((flag - 32) >> 1) % 2][MV_Y] = fmvy;
	}
	else if (flag < 64)
	{
		sadTree32x16Fmv[(flag - 48) >> 3][((flag - 48) % 8) >> 1][MV_X] = fmvx;
		sadTree32x16Fmv[(flag - 48) >> 3][((flag - 48) % 8) >> 1][MV_Y] = fmvy;
	}
	else if (flag < 80)
	{
		sadTree16x64Fmv[(flag - 64) >> 2][0][MV_X] = fmvx;
		sadTree16x64Fmv[(flag - 64) >> 2][0][MV_Y] = fmvy;
	}
	else if (flag < 96)
	{
		sadTree64x16Fmv[0][(flag - 80) >> 2][MV_X] = fmvx;
		sadTree64x16Fmv[0][(flag - 80) >> 2][MV_Y] = fmvy;
	}
	else if (flag < 112)
	{
		sadTree32x64Fmv[(flag - 96) >> 3][0][MV_X] = fmvx;
		sadTree32x64Fmv[(flag - 96) >> 3][0][MV_Y] = fmvy;
	}
	else 
	{
		sadTree64x32Fmv[0][(flag - 112) >> 3][MV_X] = fmvx;
		sadTree64x32Fmv[0][(flag - 112) >> 3][MV_Y] = fmvy;
	}

}


void fme_8x8_8x16(U5 num_refp, U12 ctu_x, U12 ctu_y, U32 lambda_mv, pel OrgY_2d[64][64], pel RefWindowLCU_2d[88][88],
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM], 
	S9 sw_offset_x, S9 sw_offset_y,
	S14 sadTree8x8Mv[8][8][2], S14 sadTree8x16Mv[8][4][2],
	S14 sadTree8x8PMv[8][8][2], S14 sadTree8x16PMv[8][4][2],
    S14 sadTree8x8Fmv[8][8][2], 
	S14 sadTree8x16Fmv[8][4][2], 
    U4 bit_depth
#if ENABLE_BFRAME
	, U5 ime_num_refp_1, pel RefWindowLCU_2d_ref1[88][88], S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS],
	U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR *me_mv_dir
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif

)
{
#pragma HLS INLINE OFF
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=OrgY_2d complete dim=2
//#pragma HLS array_partition variable=OrgY_2d cyclic factor=8 dim=2
#pragma HLS RESOURCE variable=OrgY_2d core=RAM_1P_BRAM
#pragma HLS array_partition variable=CtrPos complete dim=0
#pragma HLS array_partition variable=CtrPos_SW88x88 complete dim=0


#endif
	for (int i = 0; i < 64 * 2; i++)
	{
#if FME_PRAGMA_ON	
#pragma HLS dataflow
#endif
		U12 x, y;
		U6 org_offsetx, org_offsety;
		U8 ref_offsetx, ref_offsety;
#if ENABLE_BFRAME
		U8 ref_offsetx_1, ref_offsety_1;
		U1 lidx;
		S8 cur_refi;
#endif
		U7 height, width;
		U1 ref_flag;
		S14 mvix, mviy, pmvx, pmvy;

		pel org[8][8];
		pel ref[16][16];
		U32 satdcost[16];
		U32 bitscost[16];
		//static U32 cost_accu[16];
		U32 cost_accu[16];
		S16 hor_buf0[8][16];//w x h
		S16 hor_buf1[8][16];//w x h
		S16 hor_buf2[8][16];//w x h
		S16 hor_buf3[8][16];//w x h

		pel ver_buf0[16][32];//w x h
		pel ver_buf1[16][32];//w x h

#if FME_PRAGMA_ON	
#pragma HLS array_partition variable=org complete dim=0
//#pragma HLS array_partition variable=ref complete dim=0
//#pragma HLS RESOURCE variable=ref core=RAM_1P_BRAM
#pragma HLS array_partition variable=satdcost complete dim=1
#pragma HLS array_partition variable=bitscost complete dim=1
#pragma HLS array_partition variable=cost_accu complete dim=0

#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2

//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2

#endif
		data_prepare_8x8_8x16(i, &org_offsetx, &org_offsety, &ref_offsetx, &ref_offsety, &x, &y, &ref_flag, CtrPos_SW88x88, ctu_x, ctu_y, &height, &width, &mvix, &mviy,
			sw_offset_x, sw_offset_y, sadTree8x8Mv, sadTree8x16Mv, &pmvx, &pmvy, sadTree8x8PMv, sadTree8x16PMv
#if ENABLE_BFRAME			
			, CtrPos_SW88x88_1, sw_offset_x_1, sw_offset_y_1, me_mv_dir,
			&lidx, &cur_refi
#endif
		);
		copy_8x8org_1(org, OrgY_2d, org_offsetx, org_offsety);
		copy_16x16ref_1(ref, RefWindowLCU_2d, ref_offsetx, ref_offsety
#if ENABLE_BFRAME			
			, RefWindowLCU_2d_ref1, lidx, cur_refi
#endif
		);
		   

		interp_horizontal_8x8_1(ref, hor_buf0, hor_buf1, hor_buf2, hor_buf3);
		interp_vertical_8x8_1(hor_buf0, hor_buf1, hor_buf2, hor_buf3, ver_buf0, ver_buf1, bit_depth);

		calculate_satdcost_8x8_1(org, ver_buf0, ver_buf1, satdcost);

		cal_bitscost_1(mvix, mviy, pmvx, pmvy, num_refp, lambda_mv, bitscost
#if ENABLE_BFRAME			
			, ime_num_refp_1, lidx, cur_refi
#endif
		);

		cal_costbest_8x8_8x16(i, satdcost, bitscost, cost_accu, sadTree8x8Fmv, sadTree8x16Fmv,
			ref_flag, x, y, width, height, mvix, mviy, CtrPos
#if ENABLE_BFRAME			
			, ime_CtrPos_1, lidx, cur_refi, me_mv_dir
#endif
#if USE_INTRA_REFRESH
			,ime_out_para_set
#endif
		);



	}
}

void fme_8x32_8x64(U2 num_refp, U12 ctu_x, U12 ctu_y, U32 lambda_mv, pel OrgY_2d[64][64], pel RefWindowLCU_2d[88][88],
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	S9 sw_offset_x, S9 sw_offset_y, 
	S14 sadTree8x32Mv[8][2][2], S14 sadTree8x64Mv[8][1][2],
	S14 sadTree8x32PMv[8][2][2], S14 sadTree8x64PMv[8][1][2],
    S14 sadTree8x32Fmv[8][2][2], S14 sadTree8x64Fmv[8][1][2], U4 bit_depth
#if ENABLE_BFRAME
	, U5 ime_num_refp_1, pel RefWindowLCU_2d_ref1[88][88], S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS],
	U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR *me_mv_dir
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
#pragma HLS INLINE OFF
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=OrgY_2d complete dim=2
#pragma HLS RESOURCE variable=OrgY_2d core=RAM_1P_BRAM
#pragma HLS array_partition variable=CtrPos complete dim=0
#if ENABLE_BFRAME
#pragma HLS array_partition variable=ime_CtrPos_1 complete dim=0
#endif
#pragma HLS array_partition variable=CtrPos_SW88x88 complete dim=0
#endif
	for (int i = 0; i < 64 * 2; i++)
	{
#if FME_PRAGMA_ON	
#pragma HLS dataflow
#endif
		U12 x, y;
		U6 org_offsetx, org_offsety;
		U8 ref_offsetx, ref_offsety;
#if ENABLE_BFRAME
		U8 ref_offsetx_1, ref_offsety_1;
		U1 lidx;
		S8 cur_refi;
#endif
		U7 height, width;
		U1 ref_flag;
		S14 mvix, mviy, pmvx, pmvy;

		pel org[8][8];
		pel ref[16][16];
		U32 satdcost[16];
		U32 bitscost[16];
		//static U32 cost_accu[16];
		U32 cost_accu[16];
		S16 hor_buf0[8][16];//w x h
		S16 hor_buf1[8][16];//w x h
		S16 hor_buf2[8][16];//w x h
		S16 hor_buf3[8][16];//w x h

		pel ver_buf0[16][32];//w x h
		pel ver_buf1[16][32];//w x h

#if FME_PRAGMA_ON	
#pragma HLS ARRAY_PARTITION variable=org complete dim=2
//#pragma HLS array_partition variable=org cyclic factor=8 dim=2
//#pragma HLS ARRAY_PARTITION variable=ref complete dim=2
//#pragma HLS RESOURCE variable=ref core=RAM_1P_BRAM
#pragma HLS array_partition variable=bitscost complete dim=1
#pragma HLS array_partition variable=cost_accu complete dim=0

#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2
//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2
#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM

#endif
		data_prepare_8x32_8x64(i, &org_offsetx, &org_offsety, &ref_offsetx, &ref_offsety, &x, &y, &ref_flag, CtrPos_SW88x88, ctu_x, ctu_y, &height, &width, &mvix, &mviy,
			sw_offset_x, sw_offset_y, sadTree8x32Mv, sadTree8x64Mv, &pmvx, &pmvy, sadTree8x32PMv, sadTree8x64PMv
#if ENABLE_BFRAME			
			, CtrPos_SW88x88_1, sw_offset_x_1, sw_offset_y_1, me_mv_dir,
			&lidx, &cur_refi
#endif
		);
		copy_8x8org_2(org, OrgY_2d, org_offsetx, org_offsety);
		copy_16x16ref_2(ref, RefWindowLCU_2d, ref_offsetx, ref_offsety
#if ENABLE_BFRAME			
			, RefWindowLCU_2d_ref1, lidx, cur_refi
#endif
		);
		interp_horizontal_8x8_2(ref, hor_buf0, hor_buf1, hor_buf2, hor_buf3);
		interp_vertical_8x8_2(hor_buf0, hor_buf1, hor_buf2, hor_buf3, ver_buf0, ver_buf1, bit_depth);
		calculate_satdcost_8x8_2(org, ver_buf0, ver_buf1, satdcost);
		cal_bitscost_2(mvix, mviy, pmvx, pmvy, num_refp, lambda_mv, bitscost
#if ENABLE_BFRAME			
			, ime_num_refp_1, lidx, cur_refi
#endif
		);
		cal_costbest_8x32_8x64(i, satdcost, bitscost, cost_accu, 
			sadTree8x32Fmv, sadTree8x64Fmv, ref_flag, x, y, width, height, mvix, mviy, CtrPos
#if ENABLE_BFRAME			
			, ime_CtrPos_1, lidx, cur_refi, me_mv_dir
#endif
#if USE_INTRA_REFRESH
				, ime_out_para_set
#endif
		);
	}
}

void fme_16x8_32x8_64x8_64x64(U2 num_refp, U12 ctu_x, U12 ctu_y, U32 lambda_mv, pel OrgY_2d[64][64], pel RefWindowLCU_2d[88][88],
    S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM],
	S9 sw_offset_x, S9 sw_offset_y,
	S14 sadTree16x8Mv[4][8][2], S14 sadTree32x8Mv[2][8][2],
	S14 sadTree64x8Mv[1][8][2], S14 sadTree64x64Mv[1][1][2],
	S14 sadTree16x8PMv[4][8][2], S14 sadTree32x8PMv[2][8][2],
	S14 sadTree64x8PMv[1][8][2], S14 sadTree64x64PMv[1][1][2],
	S14 sadTree16x8Fmv[4][8][2],
	S14 sadTree32x8Fmv[2][8][2],
	S14 sadTree64x8Fmv[1][8][2], S14 sadTree64x64Fmv[1][1][2],U4 bit_depth
#if ENABLE_BFRAME
	, U5 ime_num_refp_1, pel RefWindowLCU_2d_ref1[88][88], S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS],
	U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR* me_mv_dir
#endif
#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
#pragma HLS INLINE OFF
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=OrgY_2d complete dim=2
//#pragma HLS array_partition variable=OrgY_2d cyclic factor=8 dim=2
//#pragma HLS RESOURCE variable=OrgY_2d core=RAM_1P_BRAM

#pragma HLS array_partition variable=CtrPos_SW88x88 complete dim=0
#endif
	for (int i = 0; i < 32 * 4; i++)
	{
#if FME_PRAGMA_ON	
#pragma HLS dataflow
#endif
		U12 x, y;
		U6 org_offsetx, org_offsety;
		U8 ref_offsetx, ref_offsety;
#if ENABLE_BFRAME
		U8 ref_offsetx_1, ref_offsety_1;
		U1 lidx;
		S8 cur_refi;
#endif
		U7 height, width;
		U1 ref_flag;
		S14 mvix, mviy, pmvx, pmvy;

		pel org[8][16];
		pel ref[16][24];
		U32 satdcost[16];
		U32 bitscost[16];
		U32 cost_accu[16];

		S16 hor_buf0[16][16];//w x h
		S16 hor_buf1[16][16];//w x h
		S16 hor_buf2[16][16];//w x h
		S16 hor_buf3[16][16];//w x h

		pel ver_buf0[8][64];//(-1,  1) (0,  1) (-1, 2) (0, 2)
		pel ver_buf1[8][64];//( 1, -1) (2, -1) ( 1, 0) (2, 0)
		pel ver_buf2[8][64];//( 1,  1) (2,  1) ( 1, 2) (2, 2)
		pel ver_buf3[8][64];//(-1, -1) (0, -1) (-1, 0) (0, 0)

#if FME_PRAGMA_ON	
#pragma HLS array_partition variable=org complete dim=2
#pragma HLS array_partition variable=ref complete dim=2
//#pragma HLS RESOURCE variable=ref core=RAM_1P_BRAM

#pragma HLS array_partition variable=bitscost complete dim=1
#pragma HLS array_partition variable=cost_accu complete dim=0
#pragma HLS array_partition variable=CtrPos complete dim=0
#if ENABLE_BFRAME
#pragma HLS array_partition variable=ime_CtrPos_1 complete dim=0
#endif
#pragma HLS array_partition variable=hor_buf0 complete dim=2
#pragma HLS array_partition variable=hor_buf1 complete dim=2
#pragma HLS array_partition variable=hor_buf2 complete dim=2
#pragma HLS array_partition variable=hor_buf3 complete dim=2

//#pragma HLS array_partition variable=ver_buf0 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf2 cyclic factor=16 dim=2
//#pragma HLS array_partition variable=ver_buf3 cyclic factor=16 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2
#pragma HLS array_partition variable=ver_buf2 complete dim=2
#pragma HLS array_partition variable=ver_buf3 complete dim=2

#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf3 core=RAM_1P_BRAM
#pragma HLS array_partition variable=satdcost complete dim=1
#endif

		data_prepare_16x8_32x8_64x8_64x64(i, &org_offsetx, &org_offsety, &ref_offsetx, &ref_offsety, &x, &y, &ref_flag, CtrPos_SW88x88, ctu_x, ctu_y, &height, &width, &mvix, &mviy,
			sw_offset_x, sw_offset_y, sadTree16x8Mv, sadTree32x8Mv, sadTree64x8Mv, sadTree64x64Mv,
			&pmvx, &pmvy, sadTree16x8PMv, sadTree32x8PMv, sadTree64x8PMv, sadTree64x64PMv
#if ENABLE_BFRAME			
			, CtrPos_SW88x88_1, sw_offset_x_1, sw_offset_y_1, me_mv_dir,
			&lidx, &cur_refi
#endif
		);
		copy_16x8org_1(org, OrgY_2d, org_offsetx, org_offsety);
		
		copy_24x16ref_1(ref, RefWindowLCU_2d, ref_offsetx, ref_offsety
#if ENABLE_BFRAME			
			, RefWindowLCU_2d_ref1, lidx, cur_refi
#endif
		);

		interp_horizontal_16x8_1(ref, hor_buf0, hor_buf1, hor_buf2, hor_buf3);
		interp_vertical_16x8_1_1(hor_buf3, hor_buf0, ver_buf3, ver_buf0, bit_depth);
		interp_vertical_16x8_2_1(hor_buf1, hor_buf2, ver_buf1, ver_buf2, bit_depth);
		calculate_satdcost_16x8_1(org, ver_buf0, ver_buf1, ver_buf2, ver_buf3, satdcost);

		cal_bitscost_3(mvix, mviy, pmvx, pmvy, num_refp, lambda_mv, bitscost
#if ENABLE_BFRAME			
			, ime_num_refp_1, lidx, cur_refi
#endif
		);

		cal_costbest_16x8_32x8_64x8_64x64(i, satdcost, bitscost, cost_accu, sadTree16x8Fmv,
			sadTree32x8Fmv, sadTree64x8Fmv, sadTree64x64Fmv,
			ref_flag, x, y, width, height, mvix, mviy, CtrPos
#if ENABLE_BFRAME			
			, ime_CtrPos_1, lidx, cur_refi, me_mv_dir
#endif
#if USE_INTRA_REFRESH
				, ime_out_para_set
#endif
		);

	}
}

void fme_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32_test(
	U2 num_refp, U12 ctu_x, U12 ctu_y, U32 lambda_mv, pel OrgY_2d[64][64], pel RefWindowLCU_2d[88][88],
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS], U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM], S9 sw_offset_x, S9 sw_offset_y,
	S14 sadTree16x16Mv[4][4][2], S14 sadTree32x32Mv[2][2][2],
	S14 sadTree16x32Mv[4][2][2], S14 sadTree32x16Mv[2][4][2],
	S14 sadTree16x64Mv[4][1][2], S14 sadTree64x16Mv[1][4][2],
	S14 sadTree32x64Mv[2][1][2], S14 sadTree64x32Mv[1][2][2],
	S14 sadTree16x16PMv[4][4][2], S14 sadTree32x32PMv[2][2][2],
	S14 sadTree16x32PMv[4][2][2], S14 sadTree32x16PMv[2][4][2],
	S14 sadTree16x64PMv[4][1][2], S14 sadTree64x16PMv[1][4][2],
	S14 sadTree32x64PMv[2][1][2], S14 sadTree64x32PMv[1][2][2],
	S14 sadTree16x16Fmv[4][4][2], S14 sadTree32x32Fmv[2][2][2],
	S14 sadTree16x32Fmv[4][2][2], S14 sadTree32x16Fmv[2][4][2],
	S14 sadTree16x64Fmv[4][1][2], S14 sadTree64x16Fmv[1][4][2],
	S14 sadTree32x64Fmv[2][1][2], S14 sadTree64x32Fmv[1][2][2], U4 bit_depth
#if ENABLE_BFRAME
		, U5 ime_num_refp_1, pel RefWindowLCU_2d_ref0[88][88], pel RefWindowLCU_2d_ref1[88][88], S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS],
		U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM], S9 sw_offset_x_1, S9 sw_offset_y_1, ME_MV_DIR* me_mv_dir
#endif

#if USE_INTRA_REFRESH
	, IME_OUT_PARA_SET* ime_out_para_set
#endif
)
{
#pragma HLS INLINE OFF
#if FME_PRAGMA_ON
#pragma HLS array_partition variable=OrgY_2d complete dim=2
#pragma HLS RESOURCE variable=OrgY_2d core=RAM_1P_BRAM
#pragma HLS array_partition variable=CtrPos complete dim=0
#if ENABLE_BFRAME
#pragma HLS array_partition variable=ime_CtrPos_1 complete dim=0
#endif
#pragma HLS array_partition variable=CtrPos_SW88x88 complete dim=0
#endif
	for (int i = 0; i < 16 * 8; i++)
	{
#if FME_PRAGMA_ON	
#pragma HLS dataflow
#endif
		U12 x, y;
		U6 org_offsetx, org_offsety;
		U8 ref_offsetx, ref_offsety;
#if ENABLE_BFRAME
		U8 ref_offsetx_1, ref_offsety_1;
		U1 lidx;
		S8 cur_refi;
#endif
		U7 height, width;
		U1 ref_flag;
		S14 mvix, mviy, pmvx, pmvy;

		pel org[16][16];
		pel ref0[12][24];
		pel ref1[12][24];
		U32 satdcost[16];
		U32 bitscost[16];
		U32 cost_accu[16];

		S16 hor_buf0_0[16][16];//w x (h / 2 + padding)
		S16 hor_buf1_0[16][16];//w x (h / 2 + padding)
		S16 hor_buf2_0[16][16];//w x (h / 2 + padding)
		S16 hor_buf3_0[16][16];//w x (h / 2 + padding)
		S16 hor_buf0_1[16][16];//w x (h / 2 + padding)
		S16 hor_buf1_1[16][16];//w x (h / 2 + padding)
		S16 hor_buf2_1[16][16];//w x (h / 2 + padding)
		S16 hor_buf3_1[16][16];//w x (h / 2 + padding)

		pel ver_buf0[8][128];//(-1,  1) (0,  1) (-1, 2) (0, 2)
		pel ver_buf1[8][128];//( 1, -1) (2, -1) ( 1, 0) (2, 0)
		pel ver_buf2[8][128];//( 1,  1) (2,  1) ( 1, 2) (2, 2)
		pel ver_buf3[8][128];//(-1, -1) (0, -1) (-1, 0) (0, 0)

#if FME_PRAGMA_ON	
//#pragma HLS array_partition variable=org complete dim=0
//#pragma HLS array_partition variable=org cyclic factor=8 dim=2
#pragma HLS array_partition variable=ref0 complete dim=2
#pragma HLS array_partition variable=ref1 complete dim=2
#pragma HLS RESOURCE variable=ref0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref1 core=RAM_1P_BRAM
#pragma HLS array_partition variable=satdcost complete dim=1
#pragma HLS array_partition variable=bitscost complete dim=1
#pragma HLS array_partition variable=cost_accu complete dim=0

#pragma HLS array_partition variable=hor_buf0_0 complete dim=2
#pragma HLS array_partition variable=hor_buf1_0 complete dim=2
#pragma HLS array_partition variable=hor_buf2_0 complete dim=2
#pragma HLS array_partition variable=hor_buf3_0 complete dim=2
#pragma HLS array_partition variable=hor_buf0_1 complete dim=2
#pragma HLS array_partition variable=hor_buf1_1 complete dim=2
#pragma HLS array_partition variable=hor_buf2_1 complete dim=2
#pragma HLS array_partition variable=hor_buf3_1 complete dim=2


//#pragma HLS array_partition variable=ver_buf0 cyclic factor=32 dim=2
//#pragma HLS array_partition variable=ver_buf1 cyclic factor=32 dim=2
//#pragma HLS array_partition variable=ver_buf2 cyclic factor=32 dim=2
//#pragma HLS array_partition variable=ver_buf3 cyclic factor=32 dim=2
#pragma HLS array_partition variable=ver_buf0 complete dim=2
#pragma HLS array_partition variable=ver_buf1 complete dim=2
#pragma HLS array_partition variable=ver_buf2 complete dim=2
#pragma HLS array_partition variable=ver_buf3 complete dim=2

#pragma HLS RESOURCE variable=ver_buf0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf2 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ver_buf3 core=RAM_1P_BRAM
#endif
		data_prepare_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32(
			i, &org_offsetx, &org_offsety, &ref_offsetx, &ref_offsety, &x, &y, &ref_flag, CtrPos_SW88x88, ctu_x, ctu_y, &height, &width,
			&mvix, &mviy, sw_offset_x, sw_offset_y,
			sadTree16x16Mv, sadTree32x32Mv, sadTree16x32Mv, sadTree32x16Mv,
			sadTree16x64Mv, sadTree64x16Mv, sadTree32x64Mv, sadTree64x32Mv,
			&pmvx, &pmvy,
			sadTree16x16PMv, sadTree32x32PMv, sadTree16x32PMv, sadTree32x16PMv,
			sadTree16x64PMv, sadTree64x16PMv, sadTree32x64PMv, sadTree64x32PMv
#if ENABLE_BFRAME			
			, CtrPos_SW88x88_1, sw_offset_x_1, sw_offset_y_1, me_mv_dir,
				&lidx, &cur_refi
#endif
		);
		copy_16x16org(org, OrgY_2d, org_offsetx, org_offsety);
		copy_24x24ref(ref0, ref1, RefWindowLCU_2d, ref_offsetx, ref_offsety
#if ENABLE_BFRAME			
			, RefWindowLCU_2d_ref0, RefWindowLCU_2d_ref1, lidx, cur_refi
#endif
		);
		interp_horizontal_16x16(ref0, ref1,
			hor_buf0_0, hor_buf0_1, hor_buf1_0, hor_buf1_1,
			hor_buf2_0, hor_buf2_1, hor_buf3_0, hor_buf3_1);
		interp_vertical_16x16_1_4(hor_buf3_0, hor_buf3_1, hor_buf0_0, hor_buf0_1, ver_buf3, ver_buf0, bit_depth);
		interp_vertical_16x16_2_4(hor_buf1_0, hor_buf1_1, hor_buf2_0, hor_buf2_1, ver_buf1, ver_buf2, bit_depth);
		calculate_satdcost_16x16_4(org, ver_buf0, ver_buf1, ver_buf2, ver_buf3, satdcost);
		cal_bitscost_5(mvix, mviy, pmvx, pmvy, num_refp, lambda_mv, bitscost
#if ENABLE_BFRAME			
			, ime_num_refp_1, lidx, cur_refi
#endif
		);
		cal_costbest_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32(
			i, satdcost, bitscost, cost_accu, ref_flag, x, y, width, height, mvix, mviy, CtrPos,
			sadTree16x16Fmv, sadTree32x32Fmv, sadTree16x32Fmv, sadTree32x16Fmv,
			sadTree16x64Fmv, sadTree64x16Fmv, sadTree32x64Fmv, sadTree64x32Fmv
#if ENABLE_BFRAME			
			, ime_CtrPos_1, lidx, cur_refi
#endif
#if USE_INTRA_REFRESH
			, ime_out_para_set
#endif
		);
	}

}


void refbuf0_early_store(int ref_buf0[15][23], pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE]
#if ENABLE_BFRAME
	,int ref_buf0_ref1[15][23], pel RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE]
#endif
)
{
#pragma HLS INLINE OFF
	for (int i = 0; i < 12; i++)
	{
#pragma HLS PIPELINE
		for (int j = 0; j < 20; j++)
		{
			ref_buf0[i + 3][j + 3] = RefWindowLCU_2d[i][j];
#if ENABLE_BFRAME
			ref_buf0_ref1[i + 3][j + 3] = RefWindowLCU_2d_ref1[i][j];
#endif
		}
	}
}

//void refwin_line_copy(int i, int j, int refwin_line[FME_SWW_SIZE + 3], pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE])
void refwin_line_copy(int i, int j, int refwin_line[23], pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE]
#if ENABLE_BFRAME
	, int refwin_line_ref1[23], pel RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE]
#endif
)
{
#if FME_PRAGMA_ON
#pragma HLS INLINE OFF
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d cyclic factor=16 dim=2
//#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d complete dim=1
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1 cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1  complete dim=1
#pragma HLS ARRAY_PARTITION variable=refwin_line_ref1 complete dim=1
#endif
#pragma HLS ARRAY_PARTITION variable=refwin_line complete dim=1
#endif


		for (int k = 0; k < 23; k++)
		{
#pragma HLS UNROLL 
			int t = k + ((i + 1) % 10) * 16;
			// FIXME: 
			// refwin_line[3 + k] = (i < 9 && j < 3) ? 0 : 
			// RefWindowLCU_2d[((i + 1) / 10) * 8 + j - 3][k];
			refwin_line[k] = 
				(i < 9 && j < 3) ? 0 : 
				(k < 3 && i%10==9) ? 0 : 
				RefWindowLCU_2d[((i + 1) / 10) * 8 + j - 3][t - 3];
#if ENABLE_BFRAME
			refwin_line_ref1[k] =
				(i < 9 && j < 3) ? 0 :
				(k < 3 && i % 10 == 9) ? 0 :
				RefWindowLCU_2d_ref1[((i + 1) / 10) * 8 + j - 3][t - 3];
#endif
		}


	//}
	//else
	//{
	//	for (int k = 0; k < FME_SWW_SIZE; k++)
	//	{
	//		refwin_line[3 + k] = RefWindowLCU_2d[((i + 1) / 10) * 8 + j - 3][k];
	//	}
	//}
}

//void refbuf_pingpong_1(int i, int j, int ref_buf0[15][23], int ref_buf1[15][23], int refwin_line[FME_SWW_SIZE + 3])
void refbuf_pingpong_1(int i, int j, int ref_buf0[15][23], int ref_buf1[15][23], int refwin_line[23]
#if ENABLE_BFRAME				
	,int ref_buf0_ref1[15][23],int ref_buf1_ref1[15][23],int refwin_line_ref1[23]
#endif	
)
{
#pragma HLS INLINE
	//if ((i % 2) != 0)//1
	//{
		for (int k = 0; k < 23; k++)
		{
#pragma HLS unroll 
			//if ((i % 2) != 0) ref_buf0[j][k] = refwin_line[k + ((i + 1) % 10) * 16];
			//if ((i % 2) == 0) ref_buf1[j][k] = refwin_line[k + ((i + 1) % 10) * 16];
			if ((i % 2) != 0) ref_buf0[j][k] = refwin_line[k];
			if ((i % 2) == 0) ref_buf1[j][k] = refwin_line[k];
#if ENABLE_BFRAME
			if ((i % 2) != 0) ref_buf0_ref1[j][k] = refwin_line_ref1[k];
			if ((i % 2) == 0) ref_buf1_ref1[j][k] = refwin_line_ref1[k];
#endif
		}
	//}
	//else if ((i % 2) == 0)//0
	//{
	//	for (int k = 0; k < 23; k++)
	//	{
	//		ref_buf1[j][k] = refwin_line[k + ((i + 1) % 10) * 16];
	//	}
	//}
}

void refbuf_pingpong_2(int i, int ref_buf[15][23], int ref_buf0[15][23], int ref_buf1[15][23]
#if ENABLE_BFRAME
	,int ref_buf_ref1[15][23], int ref_buf0_ref1[15][23], int ref_buf1_ref1[15][23]
#endif
)
{
#pragma HLS INLINE
//	if (i % 2 == 0)
//	{
		for (int m = 0; m < 15; m++)
		{
#pragma HLS unroll
			for (int k = 0; k < 23; k++)
			{
#pragma HLS unroll 
				ref_buf[m][k] = (i % 2 == 0) ? ref_buf0[m][k] : ref_buf1[m][k];
#if ENABLE_BFRAME
				ref_buf_ref1[m][k] = (i % 2 == 0) ? ref_buf0_ref1[m][k] : ref_buf1_ref1[m][k];
#endif

			}
		}
//	}
//	else
//	{
//		for (int m = 0; m < 15; m++)
//		{
//			for (int k = 0; k < 23; k++)
//			{
//#pragma HLS pipeline
//				ref_buf[m][k] = ref_buf1[m][k];
//			}
//		}
//	}


}

int mult_S8xINT( S8 a, int b )
{
#pragma HLS INLINE OFF
return a*b;
}
// _ZP_HOR
void hor_interp(int j, int hor_buf[16][15], int ref_buf[15][23], int add1, int shift1
#if ENABLE_BFRAME
	, int hor_buf_ref1[16][15], int ref_buf_ref1[15][23]
#endif
)
{
#if FME_PRAGMA_ON
#pragma HLS INLINE OFF
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=ref_buf_ref1  complete dim=0
#pragma HLS ARRAY_PARTITION variable=hor_buf_ref1 complete dim=0
#endif
#endif
	S8 tbl_mc_l_coeff[4][8] =
	{
		{0,   0,   0, 64,  0,   0, 0,  0},
		{ -1, 4, -10, 57, 19,  -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3,  -7, 19, 57, -10, 4, -1 }
	};
#pragma HLS array_partition variable=tbl_mc_l_coeff complete dim=0
	int cond = (j == 5 ||
		j == 6 || 
		j == 7 || 
		j == 9 || 
		j == 10 || 
		j == 11 || 
		j == 13 || 
		j == 14 || 
		j == 15 
		); 
//	if ( cond )
//	{
	    for (int m = 0; m < 15; m++)
		{
#pragma HLS unroll factor=5
			for(int n = 0; n<2;n++){
			for (int k = 0; k < 8; k++)  // ref_buf[0 : 22]
			{
#pragma HLS unroll 
//				int tmp =  MAC_8TAP_FME(tbl_mc_l_coeff[j % 4], ref_buf[m], k);
				int kj = k + 8*n;
				int tmp1_reg1 = (ref_buf[m][kj+1] << 1) + ref_buf[m][kj+1];  // 3
				int tmp1_reg2 =  ref_buf[m][kj+1] << 2 ; //4
				int tmp2_reg1 = (ref_buf[m][kj+2] << 3) + (ref_buf[m][kj+2] << 1); //10
				int tmp2_reg2 = (ref_buf[m][kj+2] << 3) + (ref_buf[m][kj+2] << 1) + ref_buf[m][kj+2]; //11
				int tmp2_reg3 = (ref_buf[m][kj+2] << 2) + (ref_buf[m][kj+2] << 1) + ref_buf[m][kj+2]; // 7
				int tmp3_reg1 = (ref_buf[m][kj + 3] << 5) + (ref_buf[m][kj + 3] << 4) + (ref_buf[m][kj + 3] << 3) + ref_buf[m][kj + 3];  //57
				int tmp3_reg2 = (ref_buf[m][kj+3] << 5) + (ref_buf[m][kj+3] << 3); //40
				int tmp3_reg3 = (ref_buf[m][kj+3] << 4) + (ref_buf[m][kj+3] << 1) + ref_buf[m][kj+3]; //19
				int tmp4_reg1 = (ref_buf[m][kj+4] << 4) + (ref_buf[m][kj+4] << 1) +  ref_buf[m][kj+4] ;//19
				int tmp4_reg2 = (ref_buf[m][kj+4] << 5) + (ref_buf[m][kj+4] << 3); //40
				int tmp4_reg3 = (ref_buf[m][kj+4] << 5) + (ref_buf[m][kj+4] << 4) +  (ref_buf[m][kj+4] << 3) +  ref_buf[m][kj+4]; //57
				int tmp5_reg1 = (ref_buf[m][kj+5] << 2) + (ref_buf[m][kj+5] << 1) +  ref_buf[m][kj+5] ; // 7
				int tmp5_reg2 = (ref_buf[m][kj+5] << 3) + (ref_buf[m][kj+5] << 1) +  ref_buf[m][kj+5]; // 11
				int tmp5_reg3 = (ref_buf[m][kj+5] << 3) + (ref_buf[m][kj+5] << 1); //10
				int tmp6_reg1 = (ref_buf[m][kj+6] << 1) +  ref_buf[m][kj+6]; // 3
				int tmp6_reg2 = ref_buf[m][kj+6] << 2; //4
						
				int add0_7 = ref_buf[m][kj] + ref_buf[m][kj + 7];
				int tmp0 = (j % 4 == 0) ? 0 : -add0_7;
				int tmp1 = (j % 4 == 0) ? 0 :
					(j % 4 == 3) ? tmp1_reg1 : tmp1_reg2;
				int tmp2 = (j % 4 == 0) ? 0 :
					(j % 4 == 1) ? -tmp2_reg1 :
					(j % 4 == 2) ? -tmp2_reg2 : -tmp2_reg3;
				int tmp3 = (j % 4 == 0) ? ref_buf[m][kj + 3] << 6 :
					(j % 4 == 1) ? tmp3_reg1 :
					(j % 4 == 2) ? tmp3_reg2 : tmp3_reg3;
				int tmp4 = (j % 4 == 0) ? 0 :
					(j % 4 == 1) ? tmp4_reg1 :
					(j % 4 == 2) ? tmp4_reg2 : tmp4_reg3;
				int tmp5 = (j % 4 == 0) ? 0 :
					(j % 4 == 1) ? -tmp5_reg1 :
					(j % 4 == 2) ? -tmp5_reg2 : -tmp5_reg3;
				int tmp6 = (j % 4 == 0) ? 0 :
					(j % 4 == 1) ? tmp6_reg1 : tmp6_reg2;
				int tmp = tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5 + tmp6;
				hor_buf[kj][m] = cond ? (tmp + add1) >> shift1 : tmp;
#if ENABLE_BFRAME
				int tmp_B = MAC_8TAP_FME(tbl_mc_l_coeff[j % 4], ref_buf_ref1[m], kj);
				//int tp1_reg1 = (ref_buf_ref1[m][k + 1] << 1) + ref_buf_ref1[m][k + 1];  // 3
				//int tp1_reg2 =  ref_buf_ref1[m][k + 1] << 2; //4
				//int tp2_reg1 = (ref_buf_ref1[m][k + 2] << 3) + (ref_buf_ref1[m][k + 2] << 1); //10
				//int tp2_reg2 = (ref_buf_ref1[m][k + 2] << 3) + (ref_buf_ref1[m][k + 2] << 1) + ref_buf_ref1[m][k + 2]; //11
				//int tp2_reg3 = (ref_buf_ref1[m][k + 2] << 2) + (ref_buf_ref1[m][k + 2] << 1) + ref_buf_ref1[m][k + 2]; // 7
				//int tp3_reg1 = (ref_buf_ref1[m][k + 3] << 5) + (ref_buf_ref1[m][k + 3] << 4) +(ref_buf_ref1[m][k + 3] << 3) + ref_buf_ref1[m][k + 3];  //57
				//int tp3_reg2 = (ref_buf_ref1[m][k + 3] << 5) + (ref_buf_ref1[m][k + 3] << 3); //40
				//int tp3_reg3 = (ref_buf_ref1[m][k + 3] << 4) + (ref_buf_ref1[m][k + 3] << 1) + ref_buf_ref1[m][k + 3]; //19
				//int tp4_reg1 = (ref_buf_ref1[m][k + 4] << 4) + (ref_buf_ref1[m][k + 4] << 1) + ref_buf_ref1[m][k + 4];//19
				//int tp4_reg2 = (ref_buf_ref1[m][k + 4] << 5) + (ref_buf_ref1[m][k + 4] << 3); //40
				//int tp4_reg3 = (ref_buf_ref1[m][k + 4] << 5) + (ref_buf_ref1[m][k + 4] << 4) +(ref_buf_ref1[m][k + 4] << 3) + ref_buf_ref1[m][k + 4]; //57
				//int tp5_reg1 = (ref_buf_ref1[m][k + 5] << 2) + (ref_buf_ref1[m][k + 5] << 1) + ref_buf_ref1[m][k + 5]; // 7
				//int tp5_reg2 = (ref_buf_ref1[m][k + 5] << 3) + (ref_buf_ref1[m][k + 5] << 1) + ref_buf_ref1[m][k + 5]; // 11
				//int tp5_reg3 = (ref_buf_ref1[m][k + 5] << 3) + (ref_buf_ref1[m][k + 5] << 1); //10
				//int tp6_reg1 = (ref_buf_ref1[m][k + 6] << 1) +  ref_buf_ref1[m][k + 6]; // 3
				//int tp6_reg2 =  ref_buf_ref1[m][k + 6] << 2; //4

				//int addB0_7 = ref_buf_ref1[m][k] + ref_buf_ref1[m][k + 7];
				//int tmpB0 = (j % 4 == 0) ? 0 : -add0_7;
				//int tmpB1 = (j % 4 == 0) ? 0 :
				//			(j % 4 == 3) ? tmp1_reg1 : tmp1_reg2;
				//int tmpB2 = (j % 4 == 0) ? 0 :
				//		    (j % 4 == 1) ? -tmp2_reg1 :
				//		    (j % 4 == 2) ? -tmp2_reg2 : -tmp2_reg3;
				//int tmpB3 = (j % 4 == 0) ? ref_buf_ref1[m][k + 3] << 6 :
				//		    (j % 4 == 1) ? tmp3_reg1 :
				//		    (j % 4 == 2) ? tmp3_reg2 : tmp3_reg3;
				//int tmpB4 = (j % 4 == 0) ? 0 :
				//		    (j % 4 == 1) ? tmp4_reg1 :
				//		    (j % 4 == 2) ? tmp4_reg2 : tmp4_reg3;
				//int tmpB5 = (j % 4 == 0) ? 0 :
				//		    (j % 4 == 1) ? -tmp5_reg1 :
				//		    (j % 4 == 2) ? -tmp5_reg2 : -tmp5_reg3;
				//int tmpB6 = (j % 4 == 0) ? 0 :
				//		    (j % 4 == 1) ? tmp6_reg1 : tmp6_reg2;
				//int tmp_B = tmpB0 + tmpB1 + tmpB2 + tmpB3 + tmpB4 + tmpB5 + tmpB6;
				hor_buf_ref1[kj][m] = cond ? (tmp_B + add1) >> shift1 : tmp_B;
#endif

			}
			}
		}
//	}
//	else
//	{
//		for (int k = 0; k < 16; k++)
//		{
//			for (int m = 0; m < 15; m++)
//			{
//#pragma HLS pipeline II=1
//				hor_buf[k][m] = MAC_8TAP_FME(tbl_mc_l_coeff[j % 4], ref_buf[m], k);
//			}
//		}
//	}
}


void ver_interp(int i, int j, int row, int oe, int hor_buf[16][15], pel fme2mdrefbuf[2560][8][2][16], U4 bit_depth, int add2, int shift2
#if ENABLE_BFRAME
, int hor_buf_ref1[16][15]
#endif
)
{
#if FME_PRAGMA_ON
#pragma HLS INLINE OFF
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf block factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf complete dim=2
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf complete dim=3
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf complete dim=4
//#pragma HLS ARRAY_PARTITION variable=hor_buf complete dim=0

#endif
	S8 tbl_mc_l_coeff[4][8] =
	{
		{0, 0, 0, 64, 0, 0, 0, 0},
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
	int cond = (j == 5 ||
		j == 6 ||
		j == 7 ||
		j == 9 ||
		j == 10 ||
		j == 11 ||
		j == 13 ||
		j == 14 ||
		j == 15
		);
//	if (cond)
//	{
	for (int m = 0; m < 16; m++)
	{
#pragma HLS unroll factor=4
		for (int n = 0; n < 2; n++) {			
			for (int k = 0; k < 4; k++)
			{
#pragma HLS unroll 
				int kn = k + n * 4;
				int addr1 = (i % 10) / 2 + ((row + kn) / 8) * 5 + 80 * j;
				//int pt = MAC_8TAP_FME(tbl_mc_l_coeff[j / 4], hor_buf[k], m);
				int tmp1_reg1 = (hor_buf[m][kn + 1] << 1) +  hor_buf[m][kn + 1];  // 3
				int tmp1_reg2 =  hor_buf[m][kn + 1] << 2; //4
				int tmp2_reg1 = (hor_buf[m][kn + 2] << 3) + (hor_buf[m][kn + 2] << 1); //10
				int tmp2_reg2 = (hor_buf[m][kn + 2] << 3) + (hor_buf[m][kn + 2] << 1) + hor_buf[m][kn + 2]; //11
				int tmp2_reg3 = (hor_buf[m][kn + 2] << 2) + (hor_buf[m][kn + 2] << 1) + hor_buf[m][kn + 2]; // 7
				int tmp3_reg1 = (hor_buf[m][kn + 3] << 5) + (hor_buf[m][kn + 3] << 4) +(hor_buf[m][kn + 3] << 3) + hor_buf[m][kn + 3];  //57
				int tmp3_reg2 = (hor_buf[m][kn + 3] << 5) + (hor_buf[m][kn + 3] << 3); //40
				int tmp3_reg3 = (hor_buf[m][kn + 3] << 4) + (hor_buf[m][kn + 3] << 1) + hor_buf[m][kn + 3]; //19
				int tmp4_reg1 = (hor_buf[m][kn + 4] << 4) + (hor_buf[m][kn + 4] << 1) + hor_buf[m][kn + 4];//19
				int tmp4_reg2 = (hor_buf[m][kn + 4] << 5) + (hor_buf[m][kn + 4] << 3); //40
				int tmp4_reg3 = (hor_buf[m][kn + 4] << 5) + (hor_buf[m][kn + 4] << 4) +(hor_buf[m][kn + 4] << 3) + hor_buf[m][kn + 4]; //57
				int tmp5_reg1 = (hor_buf[m][kn + 5] << 2) + (hor_buf[m][kn + 5] << 1) + hor_buf[m][kn + 5]; // 7
				int tmp5_reg2 = (hor_buf[m][kn + 5] << 3) + (hor_buf[m][kn + 5] << 1) + hor_buf[m][kn + 5]; // 11
				int tmp5_reg3 = (hor_buf[m][kn + 5] << 3) + (hor_buf[m][kn + 5] << 1); //10
				int tmp6_reg1 = (hor_buf[m][kn + 6] << 1) +  hor_buf[m][kn + 6]; // 3
				int tmp6_reg2 =  hor_buf[m][kn + 6] << 2; //4

				int add0_7 = hor_buf[m][kn] + hor_buf[m][kn + 7];
				int tmp0 = (j / 4 == 0) ? 0 : -add0_7;
				int tmp1 = (j / 4 == 0) ? 0 :
						   (j / 4 == 3) ? tmp1_reg1 : tmp1_reg2;
				int tmp2 = (j / 4 == 0) ? 0 :
					       (j / 4 == 1) ? -tmp2_reg1 :
					       (j / 4 == 2) ? -tmp2_reg2 : -tmp2_reg3;
				int tmp3 = (j / 4 == 0) ? hor_buf[m][kn + 3] << 6 :
						   (j / 4 == 1) ? tmp3_reg1 :
						   (j / 4 == 2) ? tmp3_reg2 : tmp3_reg3;
				int tmp4 = (j / 4 == 0) ? 0 :
						   (j / 4 == 1) ? tmp4_reg1 :
						   (j / 4 == 2) ? tmp4_reg2 : tmp4_reg3;
				int tmp5 = (j / 4 == 0) ? 0 :
						   (j / 4 == 1) ? -tmp5_reg1 :
						   (j / 4 == 2) ? -tmp5_reg2 : -tmp5_reg3;
				int tmp6 = (j / 4 == 0) ? 0 :
					       (j / 4 == 1) ? tmp6_reg1 : tmp6_reg2;
				int pt = tmp0 + tmp1 + tmp2 + tmp3 + tmp4 + tmp5 + tmp6;
				int pt2 = cond ? (pt + add2) >> shift2 : (pt + (1 << 11)) >> 12;
				fme2mdrefbuf[addr1][kn][oe][m] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt2);

#if ENABLE_BFRAME
				int pt_ref1 = MAC_8TAP_FME(tbl_mc_l_coeff[j / 4], hor_buf_ref1[m], kn);
				int pt2_ref1 = cond ? (pt_ref1 + add2) >> shift2 : (pt_ref1 + (1 << 11)) >> 12;
				fme2mdrefbuf[addr1 + 1280][kn][oe][m] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt2_ref1);
#endif
			}
			}
		}

//	}
//	else
//	{
//		for (int k = 0; k < 16; k++)
//		{
//			for (int m = 0; m < 8; m++)
//			{
//#pragma HLS pipeline II=1
//				int pt = MAC_8TAP_FME(tbl_mc_l_coeff[j / 4], hor_buf[k], m);
//				int pt2 = (pt + (1 << 11)) >> 12;
//				fme2mdrefbuf[(i % 10) / 2 + ((row + m) / 8) * 5 + 80 * j][m][oe][k] = COM_CLIP3((S12)0, (S12)((1 << bit_depth) - 1), pt2);
//			}
//		}
//	}
}

void search_window_luma_interpolation(U4 bit_depth, pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE], pel fme2mdrefbuf[2560][8][2][16]
#if ENABLE_BFRAME
	, pel RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE]
#endif
)
{

#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d cyclic factor=16 dim=2
#if ENABLE_BFRAME
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1 cyclic factor=16 dim=2
#endif
#pragma HLS RESOURCE variable=RefWindowLCU_2d core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme2mdrefbuf core=RAM_1P_BRAM
	S8 tbl_mc_l_coeff[4][8] =
	{
		{0, 0, 0, 64, 0, 0, 0, 0},
		{ -1, 4, -10, 57, 19, -7, 3, -1 },
		{ -1, 4, -11, 40, 40, -11, 4, -1 },
		{ -1, 3, -7, 19, 57, -10, 4, -1 }
	};
	const int shift1 = bit_depth - 8;
	const int shift2 = 20 - bit_depth;
	const int add1 = (1 << shift1) >> 1;
	//const int add2 = 1 << (shift2 - 1);
	const int add2 = 1 << (19 - bit_depth);
	int refwin_line[23];
	int refwin_line_ref1[23];
	int ref_buf0[15][23];
	int ref_buf0_ref1[15][23];
	int ref_buf1[15][23];
	int ref_buf1_ref1[15][23];
	int ref_buf[15][23];
	int ref_buf_ref1[15][23];
	int hor_buf[16][15];
	int hor_buf_ref1[16][15];

#pragma HLS ARRAY_PARTITION variable=refwin_line complete dim=0
#pragma HLS ARRAY_PARTITION variable=ref_buf0 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ref_buf1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ref_buf  cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=ref_buf  complete dim=2
#pragma HLS ARRAY_PARTITION variable=hor_buf  cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=hor_buf  cyclic factor=5 dim=2

#pragma HLS ARRAY_PARTITION variable=refwin_line_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ref_buf0_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ref_buf1_ref1 complete dim=0
#pragma HLS ARRAY_PARTITION variable=ref_buf_ref1  complete dim=0
#pragma HLS ARRAY_PARTITION variable=hor_buf_ref1 complete dim=0
#pragma HLS RESOURCE variable=fme2mdrefbuf core=RAM_1P_BRAM

//#pragma HLS array_partition variable=hor_buf cyclic factor=15 dim=2

	refbuf0_early_store(ref_buf0, RefWindowLCU_2d
#if ENABLE_BFRAME
		, ref_buf0_ref1, RefWindowLCU_2d_ref1
#endif
	);

	for (int i = 0; i < (FME_SWH_SIZE - 4) * (FME_SWW_SIZE - 4) / (8 * 16); i++)
	{

		for (int j = 0; j < 16; j++)
		{	
#pragma HLS pipeline II=2

			if (j < 15)
			{
			
				refwin_line_copy(i, j, refwin_line, RefWindowLCU_2d
#if ENABLE_BFRAME
					, refwin_line_ref1, RefWindowLCU_2d_ref1
#endif
				);

				refbuf_pingpong_1(i, j, ref_buf0, ref_buf1, refwin_line
#if ENABLE_BFRAME				
					, ref_buf0_ref1, ref_buf1_ref1, refwin_line_ref1
#endif			
				);
			}

			refbuf_pingpong_2(i, ref_buf, ref_buf0, ref_buf1
#if ENABLE_BFRAME
				, ref_buf_ref1, ref_buf0_ref1, ref_buf1_ref1
#endif
			);  

			hor_interp(j, hor_buf, ref_buf, add1, shift1
#if ENABLE_BFRAME
				, hor_buf_ref1, ref_buf_ref1
#endif
			);

			ver_interp(i, j, (i / 10) * 8, i % 2, hor_buf, fme2mdrefbuf, bit_depth, add2, shift2
#if ENABLE_BFRAME			
			,hor_buf_ref1
#endif			
			);

		}
	}


//#if ENABLE_BFRAME
//	refbuf0_early_store(ref_buf0, RefWindowLCU_2d_ref1);
//
//	for (int i = 0; i < (FME_SWH_SIZE - 4) * (FME_SWW_SIZE - 4) / (8 * 16); i++)
//	{
//#pragma HLS PIPELINE
//		for (int j = 0; j < 16; j++)
//		{
//			if (j < 15)
//			{
//				refwin_line_copy(i, j, refwin_line, RefWindowLCU_2d_ref1);
//				refbuf_pingpong_1(i, j, ref_buf0, ref_buf1, refwin_line);
//			}
//			refbuf_pingpong_2(i, ref_buf, ref_buf0, ref_buf1);
//			hor_interp(j, hor_buf, ref_buf, add1, shift1);
//			ver_interp(i, j, (i / 10) * 8, i % 2, hor_buf, fme2mdrefbuf + 1280, bit_depth, add2, shift2);
//		}
//	}
//#endif

}
#if ENABLE_BFRAME
void me_mv_dir_fme2md_copy(ME_MV_DIR* me_mv_dir, ME_MV_DIR* me_mv_dir_fme2md)
{
#pragma HLS PIPELINE
	for (int i = 0; i < 1; ++i)
	{
		for (int j = 0; j < 1; ++j)
		{
			me_mv_dir_fme2md->block64x64MvDir[i][j].lidx = me_mv_dir->block64x64MvDir[i][j].lidx;
			me_mv_dir_fme2md->block64x64MvDir[i][j].cur_refi = me_mv_dir->block64x64MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			me_mv_dir_fme2md->block32x32MvDir[i][j].lidx = me_mv_dir->block32x32MvDir[i][j].lidx;
			me_mv_dir_fme2md->block32x32MvDir[i][j].cur_refi = me_mv_dir->block32x32MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 1; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			me_mv_dir_fme2md->block64x32MvDir[i][j].lidx = me_mv_dir->block64x32MvDir[i][j].lidx;
			me_mv_dir_fme2md->block64x32MvDir[i][j].cur_refi = me_mv_dir->block64x32MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 1; ++j)
		{
			me_mv_dir_fme2md->block32x64MvDir[i][j].lidx = me_mv_dir->block32x64MvDir[i][j].lidx;
			me_mv_dir_fme2md->block32x64MvDir[i][j].cur_refi = me_mv_dir->block32x64MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			me_mv_dir_fme2md->block16x16MvDir[i][j].lidx = me_mv_dir->block16x16MvDir[i][j].lidx;
			me_mv_dir_fme2md->block16x16MvDir[i][j].cur_refi = me_mv_dir->block16x16MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			me_mv_dir_fme2md->block32x16MvDir[i][j].lidx = me_mv_dir->block32x16MvDir[i][j].lidx;
			me_mv_dir_fme2md->block32x16MvDir[i][j].cur_refi = me_mv_dir->block32x16MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			me_mv_dir_fme2md->block16x32MvDir[i][j].lidx = me_mv_dir->block16x32MvDir[i][j].lidx;
			me_mv_dir_fme2md->block16x32MvDir[i][j].cur_refi = me_mv_dir->block16x32MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 1; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			me_mv_dir_fme2md->block64x16MvDir[i][j].lidx = me_mv_dir->block64x16MvDir[i][j].lidx;
			me_mv_dir_fme2md->block64x16MvDir[i][j].cur_refi = me_mv_dir->block64x16MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 1; ++j)
		{
			me_mv_dir_fme2md->block16x64MvDir[i][j].lidx = me_mv_dir->block16x64MvDir[i][j].lidx;
			me_mv_dir_fme2md->block16x64MvDir[i][j].cur_refi = me_mv_dir->block16x64MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			me_mv_dir_fme2md->block8x8MvDir[i][j].lidx = me_mv_dir->block8x8MvDir[i][j].lidx;
			me_mv_dir_fme2md->block8x8MvDir[i][j].cur_refi = me_mv_dir->block8x8MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			me_mv_dir_fme2md->block16x8MvDir[i][j].lidx = me_mv_dir->block16x8MvDir[i][j].lidx;
			me_mv_dir_fme2md->block16x8MvDir[i][j].cur_refi = me_mv_dir->block16x8MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			me_mv_dir_fme2md->block8x16MvDir[i][j].lidx = me_mv_dir->block8x16MvDir[i][j].lidx;
			me_mv_dir_fme2md->block8x16MvDir[i][j].cur_refi = me_mv_dir->block8x16MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			me_mv_dir_fme2md->block32x8MvDir[i][j].lidx = me_mv_dir->block32x8MvDir[i][j].lidx;
			me_mv_dir_fme2md->block32x8MvDir[i][j].cur_refi = me_mv_dir->block32x8MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			me_mv_dir_fme2md->block8x32MvDir[i][j].lidx = me_mv_dir->block8x32MvDir[i][j].lidx;
			me_mv_dir_fme2md->block8x32MvDir[i][j].cur_refi = me_mv_dir->block8x32MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 1; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			me_mv_dir_fme2md->block64x8MvDir[i][j].lidx = me_mv_dir->block64x8MvDir[i][j].lidx;
			me_mv_dir_fme2md->block64x8MvDir[i][j].cur_refi = me_mv_dir->block64x8MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 1; ++j)
		{
			me_mv_dir_fme2md->block8x64MvDir[i][j].lidx = me_mv_dir->block8x64MvDir[i][j].lidx;
			me_mv_dir_fme2md->block8x64MvDir[i][j].cur_refi = me_mv_dir->block8x64MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			me_mv_dir_fme2md->block32x4MvDir[i][j].lidx = me_mv_dir->block32x4MvDir[i][j].lidx;
			me_mv_dir_fme2md->block32x4MvDir[i][j].cur_refi = me_mv_dir->block32x4MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			me_mv_dir_fme2md->block4x32MvDir[i][j].lidx = me_mv_dir->block4x32MvDir[i][j].lidx;
			me_mv_dir_fme2md->block4x32MvDir[i][j].cur_refi = me_mv_dir->block4x32MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			me_mv_dir_fme2md->block16x4MvDir[i][j].lidx = me_mv_dir->block16x4MvDir[i][j].lidx;
			me_mv_dir_fme2md->block16x4MvDir[i][j].cur_refi = me_mv_dir->block16x4MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			me_mv_dir_fme2md->block4x16MvDir[i][j].lidx = me_mv_dir->block4x16MvDir[i][j].lidx;
			me_mv_dir_fme2md->block4x16MvDir[i][j].cur_refi = me_mv_dir->block4x16MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			me_mv_dir_fme2md->block8x4MvDir[i][j].lidx = me_mv_dir->block8x4MvDir[i][j].lidx;
			me_mv_dir_fme2md->block8x4MvDir[i][j].cur_refi = me_mv_dir->block8x4MvDir[i][j].cur_refi;
		}
	}

	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			me_mv_dir_fme2md->block4x8MvDir[i][j].lidx = me_mv_dir->block4x8MvDir[i][j].lidx;
			me_mv_dir_fme2md->block4x8MvDir[i][j].cur_refi = me_mv_dir->block4x8MvDir[i][j].cur_refi;
		}
	}
}
#endif
void fme_run(pel RefWindowLCU_2d[FME_SWH_SIZE][FME_SWW_SIZE], /*U8 Fetch_Refwin_ptr_refwin_md[FME_SW_SIZE][FME_SW_SIZE], pel ime_out_para_set_y_org[64][64],*/
	ME_MV_INFO_BW* me_mv_info, ME_PMV_INFO_BW* me_pmv_info,
	ENC_FME_INFO_ALL* fme_mv_info, IME_OUT_PARA_SET* ime_out_para_set, FME_OUT_PARA_SET* fme_out_para_set, pel fme2mdrefbuf[2560][8][2][16], FME_OUT_PARA_SET_CHROMA* fme_out_para_set_chroma
#if ENABLE_BFRAME
	, pel RefWindowLCU_2d_ref1[FME_SWH_SIZE][FME_SWW_SIZE],
	ME_MV_DIR* me_mv_dir, ME_MV_DIR* me_mv_dir_fme2md,
	pel RefWindowLCU_2d_ref1_tmp[FME_SW_SIZE][FME_SW_SIZE]
#endif
)
{

#if 0
	fme_run_input_tv(RefWindowLCU_2d, ime_out_para_set_y_org, me_mv_info, me_pmv_info, ime_out_para_set, fme2mdrefbuf
#if ENABLE_BFRAME	
		, RefWindowLCU_2d_ref1,
		me_mv_dir
#endif
	);
#endif


#if	FME_INTERFACE
//#pragma HLS RESOURCE variable=RefWindowLCU_2d core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ime_out_para_set->y_org core=RAM_1P_BRAM
#pragma HLS ARRAY_PARTITION variable=ime_out_para_set->y_org complete dim=2
//#pragma HLS RESOURCE variable=ime_out_para_set_y_org core=RAM_1P_BRAM
//#pragma HLS ARRAY_PARTITION variable=ime_out_para_set_y_org complete dim=2

#pragma HLS RESOURCE variable=me_mv_info->sadTree64x64Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree32x32Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree64x32Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree32x64Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree16x16Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree32x16Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree16x32Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree64x16Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree16x64Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree8x8Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree16x8Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree8x16Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree32x8Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree8x32Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree64x8Mv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_mv_info->sadTree8x64Mv core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_pmv_info->sadTree64x64PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree32x32PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree64x32PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree32x64PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree16x16PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree32x16PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree16x32PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree64x16PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree16x64PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree8x8PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree16x8PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree8x16PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree32x8PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree8x32PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree64x8PMv core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_pmv_info->sadTree8x64PMv core=RAM_1P_BRAM

#pragma HLS INTERFACE axis port=ime_out_para_set->pic_width
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_height
#pragma HLS INTERFACE axis port=ime_out_para_set->slice_type
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_width_in_scu
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_height_in_scu
#pragma HLS INTERFACE axis port=ime_out_para_set->log2_max_cuwh
#pragma HLS INTERFACE axis port=ime_out_para_set->log2_culine
#pragma HLS INTERFACE axis port=ime_out_para_set->max_cuwh
//#pragma HLS INTERFACE axis port=ime_out_para_set->f_scu
//#pragma HLS INTERFACE axis port=ime_out_para_set->gop_size

#pragma HLS INTERFACE axis port=ime_out_para_set->bit_depth_internal
#pragma HLS INTERFACE axis port=ime_out_para_set->max_part_ratio
#pragma HLS INTERFACE axis port=ime_out_para_set->min_cu_size
#pragma HLS INTERFACE axis port=ime_out_para_set->max_split_times
#pragma HLS INTERFACE axis port=ime_out_para_set->min_qt_size
#pragma HLS INTERFACE axis port=ime_out_para_set->max_bt_size
#pragma HLS INTERFACE axis port=ime_out_para_set->max_eqt_size
//#pragma HLS INTERFACE axis port=ime_out_para_set->affine_subblock_size_idx
//#pragma HLS INTERFACE axis port=ime_out_para_set->ipcm_enable_flag
//#pragma HLS INTERFACE axis port=ime_out_para_set->position_based_transform_enable_flag
//#pragma HLS INTERFACE axis port=ime_out_para_set->secondary_transform_enable_flag
//#pragma HLS INTERFACE axis port=ime_out_para_set->tscpm_enable_flag
//#pragma HLS INTERFACE axis port=ime_out_para_set->amvr_enable_flag
//#pragma HLS INTERFACE axis port=ime_out_para_set->emvr_enable_flag

#pragma HLS RESOURCE variable=ime_out_para_set->refp_ptr core=RAM_1P_BRAM
#if ENABLE_BFRAME
#pragma HLS RESOURCE variable=ime_out_para_set->num_refp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->num_refp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->num_refp core=RAM_1P_BRAM
#else
//#pragma HLS INTERFACE axis port=ime_out_para_set->num_refp
//#pragma HLS INTERFACE axis port=fme_out_para_set->num_refp
//#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->num_refp
#endif

#pragma HLS INTERFACE axis port=ime_out_para_set->cur_ptr
#pragma HLS INTERFACE axis port=ime_out_para_set->pix_x
#pragma HLS INTERFACE axis port=ime_out_para_set->pix_y
//#pragma HLS INTERFACE axis port=ime_out_para_set->lcu_cnt
//#pragma HLS INTERFACE axis port=ime_out_para_set->smb_num_width
//#pragma HLS INTERFACE axis port=ime_out_para_set->smb_num_height
#pragma HLS INTERFACE axis port=ime_out_para_set->qp_y
#pragma HLS INTERFACE axis port=ime_out_para_set->qp_u
//#pragma HLS INTERFACE axis port=ime_out_para_set->qp_v

#pragma HLS RESOURCE variable=ime_out_para_set->lambda core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ime_out_para_set->sqrt_lambda core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ime_out_para_set->dist_chroma_weight core=RAM_1P_BRAM

//#pragma HLS INTERFACE axis port=ime_out_para_set->ptr
#pragma HLS INTERFACE axis port=ime_out_para_set->x_last_ctu
#pragma HLS INTERFACE axis port=ime_out_para_set->y_last_ctu

#pragma HLS RESOURCE variable=ime_out_para_set->CtrPos core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ime_out_para_set->CtrPos_ref1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ime_out_para_set->CtrPosFME core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ime_out_para_set->swSubLv0TopLeft core=RAM_1P_BRAM

//#pragma HLS RESOURCE variable=ime_out_para_set->y_org core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ime_out_para_set->u_org core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ime_out_para_set->v_org core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ime_out_para_set->RefWindowLCU core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ime_out_para_set->RefWindowLCU_UP core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=ime_out_para_set->RefWindowLCU_VP core=RAM_1P_BRAM
//#pragma HLS INTERFACE axis port=ime_out_para_set->bit_depth_internal
//////////////////////////////////////////////////////////////////////////////////////
//#pragma HLS RESOURCE variable=Fetch_Refwin_ptr_refwin_md core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x64Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x32Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x32Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x64Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x16Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x16Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x32Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x16Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x64Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x8Fmv   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x8Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x16Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x8Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x32Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree64x8Fmv  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x64Fmv  core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=fme_mv_info->sadTree4x8Fmv   core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=fme_mv_info->sadTree8x4Fmv   core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=fme_mv_info->sadTree16x4Fmv  core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=fme_mv_info->sadTree4x16Fmv  core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=fme_mv_info->sadTree32x4Fmv  core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=fme_mv_info->sadTree4x32Fmv  core=RAM_1P_BRAM

#pragma HLS INTERFACE axis port=fme_out_para_set->pic_width 
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_height 
#pragma HLS INTERFACE axis port=fme_out_para_set->slice_type 
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_width_in_scu 
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_height_in_scu 
#pragma HLS INTERFACE axis port=fme_out_para_set->log2_max_cuwh 
#pragma HLS INTERFACE axis port=fme_out_para_set->log2_culine 
#pragma HLS INTERFACE axis port=fme_out_para_set->max_cuwh 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_width 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_height 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->slice_type 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_width_in_scu 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_height_in_scu 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->log2_max_cuwh 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->log2_culine 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_cuwh 


#pragma HLS INTERFACE axis port=fme_out_para_set->max_part_ratio 
#pragma HLS INTERFACE axis port=fme_out_para_set->min_cu_size 
#pragma HLS INTERFACE axis port=fme_out_para_set->max_split_times 
#pragma HLS INTERFACE axis port=fme_out_para_set->min_qt_size 
#pragma HLS INTERFACE axis port=fme_out_para_set->max_bt_size 
#pragma HLS INTERFACE axis port=fme_out_para_set->max_eqt_size 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_part_ratio 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->min_cu_size 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_split_times 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->min_qt_size 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_bt_size 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_eqt_size 


#pragma HLS RESOURCE variable=fme_out_para_set->refp_ptr core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->refp_ptr core=RAM_1P_BRAM

#pragma HLS INTERFACE axis port=fme_out_para_set->cur_ptr 
#pragma HLS INTERFACE axis port=fme_out_para_set->pix_x 
#pragma HLS INTERFACE axis port=fme_out_para_set->pix_y 
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_y 
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_u 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->cur_ptr 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pix_x 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pix_y 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->qp_y 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->qp_u 

#pragma HLS RESOURCE variable=fme_out_para_set->lambda core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->dist_chroma_weight core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->lambda core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->dist_chroma_weight core=RAM_1P_BRAM


#pragma HLS INTERFACE axis port=fme_out_para_set->x_last_ctu 
#pragma HLS INTERFACE axis port=fme_out_para_set->y_last_ctu 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->x_last_ctu 
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->y_last_ctu 


#pragma HLS RESOURCE variable=fme_out_para_set->CtrPosFME core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->CtrPosFME core=RAM_1P_BRAM
#if ENABLE_BFRAME
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPosFME_ref1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->CtrPosFME_ref1 core=RAM_1P_BRAM
#endif
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPos2MD core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->CtrPos2MD core=RAM_1P_BRAM
#if ENABLE_BFRAME
#pragma HLS RESOURCE variable=fme_out_para_set->CtrPos2MD_ref1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->CtrPos2MD_ref1 core=RAM_1P_BRAM
#endif
#pragma HLS INTERFACE axis port=ime_out_para_set->cu_dqp_enable
#pragma HLS INTERFACE axis port=ime_out_para_set->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=ime_out_para_set->cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=ime_out_para_set->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=ime_out_para_set->chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=ime_out_para_set->md_slice_qp
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_width
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_height
#pragma HLS INTERFACE axis port=ime_out_para_set->slice_type
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_width_in_scu
#pragma HLS INTERFACE axis port=ime_out_para_set->pic_height_in_scu
#pragma HLS INTERFACE axis port=ime_out_para_set->log2_max_cuwh
#pragma HLS INTERFACE axis port=ime_out_para_set->log2_culine
#pragma HLS INTERFACE axis port=ime_out_para_set->max_cuwh
#pragma HLS INTERFACE axis port=ime_out_para_set->eipm_enable_flag
#pragma HLS INTERFACE axis port=ime_out_para_set->mipf_enable_flag
#pragma HLS INTERFACE axis port=ime_out_para_set->interpf_enable_flag
//#pragma HLS INTERFACE axis port=ime_out_para_set->bit_depth_internal
#pragma HLS INTERFACE axis port=ime_out_para_set->max_part_ratio
#pragma HLS INTERFACE axis port=ime_out_para_set->min_cu_size
#pragma HLS INTERFACE axis port=ime_out_para_set->max_split_times
#pragma HLS INTERFACE axis port=ime_out_para_set->min_qt_size
#pragma HLS INTERFACE axis port=ime_out_para_set->max_bt_size
#pragma HLS INTERFACE axis port=ime_out_para_set->max_eqt_size

#pragma HLS INTERFACE axis port=ime_out_para_set->cur_ptr
#pragma HLS INTERFACE axis port=ime_out_para_set->pix_x
#pragma HLS INTERFACE axis port=ime_out_para_set->pix_y
#pragma HLS INTERFACE axis port=ime_out_para_set->qp_y
#pragma HLS INTERFACE axis port=ime_out_para_set->qp_u
#pragma HLS INTERFACE axis port=ime_out_para_set->x_last_ctu
#pragma HLS INTERFACE axis port=ime_out_para_set->y_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_width
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_height
#pragma HLS INTERFACE axis port=fme_out_para_set->slice_type
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_width_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set->pic_height_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set->log2_max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set->log2_culine
#pragma HLS INTERFACE axis port=fme_out_para_set->max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set->max_part_ratio
#pragma HLS INTERFACE axis port=fme_out_para_set->min_cu_size
#pragma HLS INTERFACE axis port=fme_out_para_set->max_split_times
#pragma HLS INTERFACE axis port=fme_out_para_set->min_qt_size
#pragma HLS INTERFACE axis port=fme_out_para_set->max_bt_size
#pragma HLS INTERFACE axis port=fme_out_para_set->max_eqt_size
#pragma HLS INTERFACE axis port=fme_out_para_set->cur_ptr
#pragma HLS INTERFACE axis port=fme_out_para_set->pix_x
#pragma HLS INTERFACE axis port=fme_out_para_set->pix_y
#pragma HLS INTERFACE axis port=fme_out_para_set->eipm_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->mipf_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->interpf_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_y
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_u
#pragma HLS INTERFACE axis port=fme_out_para_set->x_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set->y_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set->cu_dqp_enable
#pragma HLS INTERFACE axis port=fme_out_para_set->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=fme_out_para_set->cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=fme_out_para_set->chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=fme_out_para_set->chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=fme_out_para_set->md_slice_qp

#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_width
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_height
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->slice_type
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_width_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pic_height_in_scu
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->log2_max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->log2_culine
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_cuwh
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_part_ratio
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->min_cu_size
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_split_times
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->min_qt_size
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_bt_size
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->max_eqt_size
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->cur_ptr
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pix_x
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->pix_y
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->eipm_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->mipf_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->interpf_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->qp_y
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->qp_u
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->x_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->y_last_ctu
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->cu_dqp_enable
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->cu_qp_group_area_size
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->cu_qp_group_pred_qp
//#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->chroma_quant_param_delta_cb
//#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->md_slice_qp

#endif





#if COLLECT_FME_DATA
	if (ime_out_para_set->slice_type != SLICE_I)
	{
		if (fme_cu_cnt == 0) {
			fme_input_info_f = open_file(FME_INPUT_F, "wb");
			fme_output_info_f = open_file(FME_OUTPUT_F, "wb");
		}
		if (fme_cu_cnt < FME_CU_NUM) {
			fme_save_indata(RefWindowLCU_2d, ime_out_para_set->y_org, me_mv_info,
				me_pmv_info, ime_out_para_set);
			printf("FME: LCU %d  INPUT DATA SAVED. \n", fme_cu_cnt);
		}
	}
#endif
	U5 ime_num_refp;
	U13 ime_pix_x;
	U13 ime_pix_y;
	U32 ime_lambda_mv;
	U2 ime_slice_type;
	S14 ime_CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
#if ENABLE_BFRAME
	static U5 ime_num_refp_1;
	static S14 ime_CtrPos_1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	static pel RefWindowLCU_2d1_ref1[88][88];
	static pel RefWindowLCU_2d2_ref1[88][88];
	static pel RefWindowLCU_2d3_ref1[88][88];
	static pel RefWindowLCU_2d4_ref1[88][88];
	static pel RefWindowLCU_2d4_ref0[88][88];
	static pel RefWindowLCU_2d5_ref1[88][88];
	static U8 CtrPos_SW88x88_1[RANGE_NUM][DIR_DIM];
#endif

	static U8 CtrPos_SW88x88[RANGE_NUM][DIR_DIM] = { 0 };
	//static pel OrgY_2d1[64][64], RefWindowLCU_2d1[88][88];
	pel OrgY_2d1[64][64], RefWindowLCU_2d1[88][88];
	static pel OrgY_2d2[64][64], RefWindowLCU_2d2[88][88];
	static pel OrgY_2d3[64][64], RefWindowLCU_2d3[88][88];
	static pel OrgY_2d4[64][64], RefWindowLCU_2d4[88][88];
	static pel OrgY_2d5[64][64], RefWindowLCU_2d5[88][88];
	U4 bit_depth2;


//#pragma HLS array_partition variable=Fetch_Refwin_ptr_refwin_md complete dim=2
//#pragma HLS array_partition variable=ime_out_para_set_y_org complete dim=1

#pragma HLS array_partition variable=ime_CtrPos complete dim=0
#pragma HLS array_partition variable=CtrPos_SW88x88 complete dim=0

#pragma HLS array_partition variable=RefWindowLCU_2d1 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d2 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d3 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d4 complete dim=2
#pragma HLS array_partition variable=RefWindowLCU_2d5 complete dim=2
#if ENABLE_BFRAME
//#pragma HLS array_partition variable=RefWindowLCU_2d_ref1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=RefWindowLCU_2d_ref1 cyclic factor=16 dim=2
#endif
#pragma HLS array_partition variable=RefWindowLCU_2d cyclic factor=16 dim=2
#pragma HLS RESOURCE variable=RefWindowLCU_2d core=RAM_1P_BRAM

#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf block factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf complete dim=2
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf complete dim=3
#pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf complete dim=4
#pragma HLS ARRAY_PARTITION variable=ime_out_para_set->swSubLv0TopLeft complete dim=1

#pragma HLS INTERFACE axis port=ime_out_para_set->affine_enable_flag
#pragma HLS INTERFACE axis port=ime_out_para_set->qp_v
#pragma HLS INTERFACE axis port=fme_out_para_set->affine_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set->bit_depth_internal
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->affine_enable_flag
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->bit_depth_internal
//#pragma HLS INTERFACE axis port=fme_out_para_set->bit_depth_internal_ap_vld
#pragma HLS INTERFACE axis port=fme_out_para_set->qp_v
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->qp_v
#pragma HLS RESOURCE variable=ime_out_para_set->CuQPMap core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set->CuQPMap core=RAM_1P_BRAM    
#pragma HLS RESOURCE variable=fme_out_para_set_chroma->CuQPMap core=RAM_1P_BRAM             

#pragma HLS array_partition variable=ime_CtrPos complete dim=0
#pragma HLS array_partition variable=CtrPos_SW88x88 complete dim=0
#pragma HLS INTERFACE axis port=fme_out_para_set->umve_set_flag
#pragma HLS INTERFACE axis port=fme_out_para_set_chroma->umve_set_flag
#pragma HLS INTERFACE axis port=ime_out_para_set->umve_set_flag


	FME_OUT_PARA_SET_COPY fme_out_para_set_copy;

	ime_out_copy(ime_out_para_set, &fme_out_para_set_copy, &ime_num_refp, &ime_pix_x, &ime_pix_y, &ime_lambda_mv, &ime_slice_type, ime_CtrPos, &bit_depth2
#if ENABLE_BFRAME
		, &ime_num_refp_1,
		ime_CtrPos_1
#endif
	);

//	write_out_para_set_fme(ime_out_para_set, fme_out_para_set, &ime_num_refp, &ime_pix_x, &ime_pix_y, &ime_lambda_mv, &ime_slice_type, ime_CtrPos, &bit_depth2
//#if ENABLE_BFRAME
//		, &ime_num_refp_1,
//		ime_CtrPos_1
//#endif
//	);
	write_out_para_set_fme(&fme_out_para_set_copy, fme_out_para_set);
	write_out_para_set_fme_chroma(&fme_out_para_set_copy, fme_out_para_set_chroma);
#if !SIMPLE_MD

	if (ime_slice_type != SLICE_I)
	{
#if ENABLE_BFRAME
		U8 sw_tl_x = ime_out_para_set->swSubLv0TopLeft[REFP_0][MV_X];
		U8 sw_tl_y = ime_out_para_set->swSubLv0TopLeft[REFP_0][MV_Y];

		U8 sw_tl_x_1 = ime_out_para_set->swSubLv0TopLeft[REFP_1][MV_X];
		U8 sw_tl_y_1 = ime_out_para_set->swSubLv0TopLeft[REFP_1][MV_Y];
		S9 sw_offset_x_1 = (SWW - 80) / 2 - sw_tl_x_1;
		S9 sw_offset_y_1 = (SWH - 80) / 2 - sw_tl_y_1;


		set_CtrPos_SW88x88(sw_tl_x_1, sw_tl_y_1, CtrPos_SW88x88_1);

		refdatacopy(RefWindowLCU_2d_ref1, RefWindowLCU_2d1_ref1, RefWindowLCU_2d2_ref1, RefWindowLCU_2d3_ref1,
			RefWindowLCU_2d4_ref1, RefWindowLCU_2d5_ref1, ime_out_para_set->offset_fme_sww_h_ref1);

#else

		U8 sw_tl_x = ime_out_para_set->swSubLv0TopLeft[MV_X];
		U8 sw_tl_y = ime_out_para_set->swSubLv0TopLeft[MV_Y];
#endif


		//S9 sw_offset_x = 120 - sw_tl_x;
		//S9 sw_offset_y = 120 - sw_tl_y;
		S9 sw_offset_x = (SWW - 80) / 2 - sw_tl_x;
		S9 sw_offset_y = (SWH - 80) / 2 - sw_tl_y;

		set_CtrPos_SW88x88(sw_tl_x, sw_tl_y, CtrPos_SW88x88);

		datacopy(ime_out_para_set->y_org, OrgY_2d1, OrgY_2d2, OrgY_2d3, OrgY_2d4, OrgY_2d5,
			RefWindowLCU_2d, RefWindowLCU_2d1, RefWindowLCU_2d2, RefWindowLCU_2d3, RefWindowLCU_2d4,
			RefWindowLCU_2d5, ime_out_para_set->offset_fme_sww_h
		);


		fme_8x8_8x16(ime_num_refp, ime_pix_x, ime_pix_y, ime_lambda_mv,
			OrgY_2d1, RefWindowLCU_2d1, ime_CtrPos, CtrPos_SW88x88,
			sw_offset_x, sw_offset_y,
			me_mv_info->sadTree8x8Mv, me_mv_info->sadTree8x16Mv,
			me_pmv_info->sadTree8x8PMv, me_pmv_info->sadTree8x16PMv,
	        fme_mv_info->sadTree8x8Fmv,
			fme_mv_info->sadTree8x16Fmv, bit_depth2
#if ENABLE_BFRAME
			, ime_num_refp_1, RefWindowLCU_2d1_ref1, ime_CtrPos_1, CtrPos_SW88x88_1,
			sw_offset_x_1, sw_offset_y_1, me_mv_dir
#endif
#if USE_INTRA_REFRESH
			, ime_out_para_set
#endif
		);

		fme_8x32_8x64(ime_num_refp, ime_pix_x, ime_pix_y, ime_lambda_mv,
			OrgY_2d2, RefWindowLCU_2d2, ime_CtrPos, CtrPos_SW88x88, 
			sw_offset_x, sw_offset_y, 
			me_mv_info->sadTree8x32Mv, me_mv_info->sadTree8x64Mv,
			me_pmv_info->sadTree8x32PMv, me_pmv_info->sadTree8x64PMv,
			fme_mv_info->sadTree8x32Fmv, fme_mv_info->sadTree8x64Fmv, bit_depth2
#if ENABLE_BFRAME
			, ime_num_refp_1, RefWindowLCU_2d2_ref1, ime_CtrPos_1, CtrPos_SW88x88_1,
			sw_offset_x_1, sw_offset_y_1, me_mv_dir
#endif
#if USE_INTRA_REFRESH
			, ime_out_para_set
#endif
		);
	
	    fme_16x8_32x8_64x8_64x64(ime_num_refp, ime_pix_x, ime_pix_y, ime_lambda_mv,
			OrgY_2d3, RefWindowLCU_2d3, ime_CtrPos, CtrPos_SW88x88,
			sw_offset_x, sw_offset_y,
			me_mv_info->sadTree16x8Mv, me_mv_info->sadTree32x8Mv,
			me_mv_info->sadTree64x8Mv, me_mv_info->sadTree64x64Mv,
			me_pmv_info->sadTree16x8PMv, me_pmv_info->sadTree32x8PMv,
			me_pmv_info->sadTree64x8PMv, me_pmv_info->sadTree64x64PMv,
			fme_mv_info->sadTree16x8Fmv,
			fme_mv_info->sadTree32x8Fmv,
			fme_mv_info->sadTree64x8Fmv, fme_mv_info->sadTree64x64Fmv, bit_depth2
#if ENABLE_BFRAME
			, ime_num_refp_1, RefWindowLCU_2d3_ref1, ime_CtrPos_1, CtrPos_SW88x88_1,
			sw_offset_x_1, sw_offset_y_1, me_mv_dir
#endif
#if USE_INTRA_REFRESH
			, ime_out_para_set
#endif
		);

		fme_16x16_32x32_16x32_32x16_16x64_64x16_32x64_64x32_test(
			ime_num_refp, ime_pix_x, ime_pix_y, ime_lambda_mv,
			OrgY_2d4, RefWindowLCU_2d4, ime_CtrPos, CtrPos_SW88x88, sw_offset_x, sw_offset_y,
			me_mv_info->sadTree16x16Mv, me_mv_info->sadTree32x32Mv,
			me_mv_info->sadTree16x32Mv, me_mv_info->sadTree32x16Mv,
			me_mv_info->sadTree16x64Mv, me_mv_info->sadTree64x16Mv,
			me_mv_info->sadTree32x64Mv, me_mv_info->sadTree64x32Mv,
			me_pmv_info->sadTree16x16PMv, me_pmv_info->sadTree32x32PMv,
			me_pmv_info->sadTree16x32PMv, me_pmv_info->sadTree32x16PMv,
			me_pmv_info->sadTree16x64PMv, me_pmv_info->sadTree64x16PMv,
			me_pmv_info->sadTree32x64PMv, me_pmv_info->sadTree64x32PMv,
			fme_mv_info->sadTree16x16Fmv, fme_mv_info->sadTree32x32Fmv,
			fme_mv_info->sadTree16x32Fmv, fme_mv_info->sadTree32x16Fmv,
			fme_mv_info->sadTree16x64Fmv, fme_mv_info->sadTree64x16Fmv,
			fme_mv_info->sadTree32x64Fmv, fme_mv_info->sadTree64x32Fmv, bit_depth2
#if ENABLE_BFRAME
			, ime_num_refp_1, RefWindowLCU_2d4_ref0, RefWindowLCU_2d4_ref1, ime_CtrPos_1, CtrPos_SW88x88_1,
			sw_offset_x_1, sw_offset_y_1, me_mv_dir
#endif
#if USE_INTRA_REFRESH
			, ime_out_para_set
#endif		
		);


		search_window_luma_interpolation(//fme_out_para_set->bit_depth_internal,
		 bit_depth2, RefWindowLCU_2d, fme2mdrefbuf
#if ENABLE_BFRAME
			, RefWindowLCU_2d_ref1
#endif
		);



#if ENABLE_BFRAME
		me_mv_dir_fme2md_copy(me_mv_dir, me_mv_dir_fme2md);
#endif

#if COLLECT_FME_DATA
		if (ime_out_para_set->slice_type != SLICE_I)
		{
			if (fme_cu_cnt < FME_CU_NUM) {
				fme_save_outdata(fme_mv_info, fme_out_para_set, Fetch_Refwin_ptr_refwin_md);
				printf("FME: LCU %d OUTPUT DATA SAVED. \n", fme_cu_cnt);
				printf("------------------------------\n", fme_cu_cnt);
				fme_cu_cnt += 1;
			}
			else if (fme_cu_cnt == FME_CU_NUM) {
				fclose(fme_input_info_f);
				fclose(fme_output_info_f);
				printf("Files closed. \n");
				fme_cu_cnt += 1;
			}
		}
#endif
	
	}

#if 0
	fme_run_output_tv(fme_mv_info, fme_out_para_set, fme2mdrefbuf
#if ENABLE_BFRAME
		, me_mv_dir
#endif
	);

#endif
#endif
}

