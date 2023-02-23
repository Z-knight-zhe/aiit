#include "md_testbench.h"
#include "md1.h"
#include "md2.h"
//#define DATA_DECL(...) extern EXTEND_ARGS(_DATA_DECL(__VA_ARGS__));
//#define DATA_DECL_AR(...) extern EXTEND_ARGS(_DATA_DECL_AR(__VA_ARGS__));
//PORT_GLOBAL_DECL
//#undef DATA_DECL
//#undef DATA_DECL_AR
//
//void copy(
//#define DATA_DECL(type, name) ,EXTEND_ARGS(_DATA_DECL(type&, name##_dst))
//#define DATA_DECL_AR(type, name, ...) ,EXTEND_ARGS(_DATA_DECL_AR(type, name##_dst, __VA_ARGS__))
//	EXTRACT_ARGS(PORT_GLOBAL_DECL),
//#undef DATA_DECL
//#undef DATA_DECL_AR
//#define DATA_DECL(type, name) ,const EXTEND_ARGS(_DATA_DECL(type&, name##_src))
//#define DATA_DECL_AR(type, name, ...) ,const EXTEND_ARGS(_DATA_DECL_AR(type, name##_src, __VA_ARGS__))
//	EXTRACT_ARGS(PORT_GLOBAL_DECL)
//#undef DATA_DECL
//#undef DATA_DECL_AR
//)
//{
//#define DATA_DECL(type, name) memcpy(&name##_dst, &name##_src, sizeof(name));
//#define DATA_DECL_AR(type, name, ...) memcpy(name##_dst, name##_src, sizeof(name));
//	PORT_GLOBAL_DECL
//#undef DATA_DECL
//#undef DATA_DECL_AR
//}

//s64 MD_PORT::run_64x64()
//{
//#define DATA_DECL(type, name) ,name
//#define DATA_DECL_AR(type, name, ...) ,name
//	//return return_value = MD_FUNC_NAME(md_kernel, 64, 64)(EXTRACT_ARGS(PORT_INTERFACE_DECL));
//	return return_value = md_basic_64x64(EXTRACT_ARGS(PORT_INTERFACE_DECL));
//#undef DATA_DECL
//#undef DATA_DECL_AR
//}

int MD_PORT::run_md()
{
#define DATA_DECL(type, name) ,name
#define DATA_DECL_AR(type, name, ...) ,name
	md_run(EXTRACT_ARGS(PORT_INTERFACE_DECL));
	return 0;
#undef DATA_DECL
#undef DATA_DECL_AR
}

template <typename T>
void print_buf(const T& data)
{
	char* buf = (char*)&data;
	size_t sz = sizeof(T);
	for (int i = 0; i < sz; ++i) {
		printf("%x", (char)buf[i]);
	}
}

template <typename T>
void print_compare_error(const T& left, const T& right, const char* const type, const char* const data)
{
	printf("%s::%s is wrong! (", type, data);
	print_buf(left);
	printf(", ");
	print_buf(right);
	printf(")\n");
	fflush(stdout);
}

#define OP_EQUAL_BEGIN(type) \
bool operator==(const type & left, const type & right) \
{ \
	int res = true;
#define OP_EQUAL_END \
	return res; \
}

#define _COMPARE(data, type) if(left.data != right.data){ print_compare_error(left.data, right.data, #type, #data); res = false; }

OP_EQUAL_BEGIN(MD_FW)
#define COMPARE(data) _COMPARE(data, MD_FW)
	COMPARE(pic_width);
	COMPARE(pic_height);
	COMPARE(slice_type);
	COMPARE(max_dt_size);
	COMPARE(affine_enable_flag);
	COMPARE(dt_intra_enable_flag);//
	COMPARE(num_of_hmvp_cand);
	COMPARE(ipf_enable_flag);
	COMPARE(pic_width_in_scu);
	COMPARE(pic_height_in_scu);
	COMPARE(log2_max_cuwh);
	COMPARE(log2_culine);
	COMPARE(max_cuwh);
	COMPARE(f_scu);
	COMPARE(gop_size);
#if CTX_CONTROL_LUKE
	COMPARE(bit_depth_internal);
	COMPARE(max_part_ratio);
	COMPARE(min_cu_size);
	COMPARE(max_split_times);
	COMPARE(min_qt_size);
	COMPARE(max_bt_size);
	COMPARE(max_eqt_size);
	COMPARE(affine_subblock_size_idx);
	COMPARE(ipcm_enable_flag);
	COMPARE(position_based_transform_enable_flag);
	COMPARE(secondary_transform_enable_flag);
	COMPARE(tscpm_enable_flag);
	COMPARE(amvr_enable_flag);
	COMPARE(emvr_enable_flag);
#endif
	for (int i = 0; i < 17; ++i){
		for (int j = 0; j < 2; ++j){
			COMPARE(refp_ptr[i][j]);
		}
	}
	COMPARE(num_refp);
	COMPARE(cur_ptr); // ref pic
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(MD_INPUT)
#define COMPARE(data) _COMPARE(data, MD_INPUT)
	COMPARE(pix_x);
	COMPARE(pix_y);
	COMPARE(lambda_mv);
	for (int i = 0; i < 3; ++i){
		COMPARE(lambda[i]);
	}
	COMPARE(qp_y);
	COMPARE(qp_u);
	COMPARE(qp_v);
	for (int i = 0; i < 2; ++i){
		COMPARE(dist_chroma_weight[i]);
	}
	COMPARE(x_last_ctu);
	COMPARE(y_last_ctu);
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(RMD_OUTPUT)
#define COMPARE(data) _COMPARE(data, RMD_OUTPUT)
	for (int i = 0; i < RMD_CU_SIZE_NUM; ++i){
		for (int j = 0; j < IPD_RDO_CNT; ++j){
			COMPARE(mode16x16[i][j]);
		}
	}
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(strFetch_ref_window)
#define COMPARE(data) _COMPARE(data, strFetch_ref_window)
	COMPARE(ref_left);
	COMPARE(ref_right);
	COMPARE(ref_up);
	COMPARE(ref_down);
	for (int i = 0; i < SWW*SWH; ++i){
		COMPARE(RefWindowLCU[i]);
	}
	for (int i = 0; i < SWH / 2; ++i){
		for (int j = 0; j < SWW / 2; ++j){
			COMPARE(RefWindowLCU_U[i][j]);
			COMPARE(RefWindowLCU_V[i][j]);
		}
	}
	for (int i = 0; i < RANGE_NUM; ++i){
		for (int j = 0; j < DIR_DIM; ++j){
			for (int k = 0; k < MAP_DIMS; ++k){
				COMPARE(CtrPos[i][j][k]);
			}
		}
	}
	for (int i = 0; i < ((SWH * SWW) >> 2); ++i){
		COMPARE(RefWindowLCU_UP[i]);
		COMPARE(RefWindowLCU_VP[i]);
	}
	for (int i = 0; i < 320; ++i){
		for (int j = 0; j < 320; ++j){
			COMPARE(swOutLv0[i][j]);
		}
	}
	for (int i = 0; i < 160; ++i){
		for (int j = 0; j < 160; ++j){
			COMPARE(swOutLv1[i][j]);
		}
	}
	for (int i = 0; i < 80; ++i){
		for (int j = 0; j < 80; ++j){
			COMPARE(swOutLv2[i][j]);
		}
	}
	COMPARE(x_chroma);
	COMPARE(y_chroma);
	COMPARE(last_child);
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(strFetch_Orig_Lcu)
#define COMPARE(data) _COMPARE(data, strFetch_Orig_Lcu)
	for (int i = 0; i < MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH; ++i){
		COMPARE(p_fenc_LCU_Y[i]);
	}
	for (int i = 0; i < MAX_CU_SIZE_FETCH*MAX_CU_SIZE_FETCH / 2; ++i){
		COMPARE(p_fenc_LCU_UV[i]);
	}
	for (int i = 0; i < 64; ++i){
		for (int j = 0; j < 64; ++j){
			COMPARE(y_org[i][j]);
		}
	}
	for (int i = 0; i < 32; ++i){
		for (int j = 0; j < 32; ++j){
			COMPARE(u_org[i][j]);
			COMPARE(v_org[i][j]);
		}
	}
	for (int i = 0; i < 64; ++i){
		for (int j = 0; j < 64; ++j){
			COMPARE(y_orgT[i][j]);
		}
	}
	for (int i = 0; i < MAX_CU_SIZE_FETCH >> 1; ++i){
		for (int j = 0; j < MAX_CU_SIZE_FETCH >> 1; ++j){
			COMPARE(y_org_lv1_2d[i][j]);
		}
	}
	for (int i = 0; i < MAX_CU_SIZE_FETCH >> 2; ++i){
		for (int j = 0; j < MAX_CU_SIZE_FETCH >> 2; ++j){
			COMPARE(y_org_lv2_2d[i][j]);
		}
	}
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(ENC_ME_LINE_MAP)
/*
#define COMPARE(data) _COMPARE(data, ENC_ME_LINE_MAP)
	COMPARE(min_scu_x);
	COMPARE(min_scu_y);
	COMPARE(stride_in_map);
	COMPARE(stride_in_line);
	COMPARE(pic_width_in_scu);
	COMPARE(pic_height_in_scu);
	COMPARE(above_line_idx);
	COMPARE(curr_ctu_idx);
	COMPARE(x_ctu);
	COMPARE(y_ctu);
	for (int i = 0; i < MAX_CU_DEPTH; ++i){
		for (int j = 0; j < NUM_BLOCK_SHAPE; ++j){
			for (int k = 0; k < MAX_CU_CNT_IN_LCU; ++k){
				COMPARE(line_map_split[i][j][k]);
			}
		}
	}
	for (int i = 0; i < 2; ++i){
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j){
			for (int k = 0; k < SCU_IN_LCU_NUMS_LEN; ++k){
				COMPARE(map_scu_inner[i][j][k]);
				COMPARE(map_cu_mode_inner[i][j][k]);
			}
		}
		for (int j = 0; j < (MAX_WIDTH >> MIN_CU_LOG2); ++j){
			COMPARE(map_scu_above_line[i][j]);
			COMPARE(map_cu_mode_above_line[i][j]);
		}
	}
	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; ++i){
		for (int j = 0; j < MV_D; ++j){
			COMPARE(co_left_mv[i][j]);
		}
		COMPARE(co_left_refi[i]);
	}
	for (int i = 0; i < NEB_NUMS; ++i){
		COMPARE(refi_nebs[i]);
		COMPARE(scu_nebs[i]);
		for (int j = 0; j < MV_D; ++j){
			COMPARE(mv_nebs[i][j]);
		}
	}
	COMPARE(if_init);
#undef COMPARE
	*/
OP_EQUAL_END

OP_EQUAL_BEGIN(ENC_ME_NEB_INFO)
#define COMPARE(data) _COMPARE(data, ENC_ME_NEB_INFO)
	for (int i = 0; i < REFP_NUM; ++i){
		for (int j = 0; j < MV_D; ++j){
			COMPARE(mv[i][j]);
		}
		//COMPARE(refi[i]);
		if (left.refi[i] != right.refi[i]) { print_compare_error(left.refi[i], right.refi[i], "ENC_ME_NEB_INFO", "refi[i]"); res = false; }
		fflush(stdout);
	}
	COMPARE(is_valid);
#if TB_SPLIT_EXT
	COMPARE(pb_tb_part);
#endif
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(SKIP_MERGE_INPUT)
#define COMPARE(data) _COMPARE(data, SKIP_MERGE_INPUT)
	for (int i = 0; i < MAX_CU_IN_SCU; ++i){
		for (int j = 0; j < MAX_CU_IN_SCU; ++j){
			for (int k = 0; k < MV_D; ++k){
				COMPARE(refp_map_mv[i][j][k]);
			}
			COMPARE(refp_map_refi[i][j]);
		}
	}
	COMPARE(pix_x);
	COMPARE(s64_sqrt_lambda_y);
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(COM_MOTION)
#define COMPARE(data) _COMPARE(data, COM_MOTION)
	for (int i = 0; i < REFP_NUM; ++i){
		for (int j = 0; j < MV_D; ++j){
			COMPARE(mv[i][j]);
		}
		COMPARE(ref_idx[i]);
	}
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(ENC_CU_DATA_ARRAY)
/*
#define COMPARE(data) _COMPARE(data, ENC_CU_DATA_ARRAY)
	for (int i = 0; i < MAX_CU_DEPTH; ++i){
		for (int j = 0; j < NUM_BLOCK_SHAPE; ++j){
			for (int k = 0; k < MAX_CU_CNT_IN_LCU; ++k){
				COMPARE(split_mode[i][j][k]);
			}
		}
	}
	for (int i = 0; i < MAX_CU_CNT_IN_LCU; ++i){
		for (int j = 0; j < REFP_NUM; ++j){
			for (int k = 0; k < MV_D; ++k){
				COMPARE(mv[i][j][k]);
				COMPARE(mvd[i][j][k]);
			}
		}
	}
#define CU_SIZE (1 << (MAX_CU_DEPTH - 1))* (1 << (MAX_CU_DEPTH - 1))
	for (int i = 0; i < CU_SIZE; ++i){
		COMPARE(pb_part[i]);
		COMPARE(tb_part[i]);
		COMPARE(pred_mode[i]);
		COMPARE(mvr_idx[i]);
		COMPARE(umve_flag[i]);
		COMPARE(umve_idx[i]);
		COMPARE(skip_idx[i]);
		COMPARE(map_scu[i]);
		COMPARE(affine_flag[i]);
		COMPARE(smvd_flag[i]);
		COMPARE(map_cu_mode[i]);
		COMPARE(map_pb_tb_part[i]);
		COMPARE(depth[i]);
		COMPARE(ipf_flag[i]);
		COMPARE(mvp_from_hmvp_flag[i]);
	}
	for (int i = 0; i < 2; ++i){
		for (int j = 0; j < CU_SIZE; ++j){
			COMPARE(mpm[i][j]);
		}
	}
	for (int i = 0; i < 8; ++i){
		for (int j = 0; j < CU_SIZE; ++j){
			COMPARE(mpm_ext[i][j]);
		}
	}
	for (int i = 0; i < 2; ++i){
		for (int j = 0; j < CU_SIZE; ++j){
			COMPARE(ipm[i][j]);
		}
	}
	for (int i = 0; i < CU_SIZE; ++i){
		for (int j = 0; j < REFP_NUM; ++j){
			COMPARE(refi[i][j]);
		}
	}
	for (int i = 0; i < N_C; ++i){
		for (int j = 0; j < CU_SIZE; ++j){
			COMPARE(num_nz_coef[i][j]);
		}
	}
	for (int i = 0; i < (CU_SIZE << 4); ++i){
		COMPARE(coef_y[i]);
	}
	for (int i = 0; i < ((CU_SIZE << 4) >> 2); ++i){
		COMPARE(coef_u[i]);
		COMPARE(coef_v[i]);
	}
	for (int i = 0; i < (CU_SIZE << 4); ++i){
		COMPARE(reco_y[i]);
	}
	for (int i = 0; i < ((CU_SIZE << 4) >> 2); ++i){
		COMPARE(reco_u[i]);
		COMPARE(reco_v[i]);
	}
#undef CU_SIZE
#undef COMPARE
	*/
OP_EQUAL_END

OP_EQUAL_BEGIN(MD_COM_MODE)
#define COMPARE(data) _COMPARE(data, MD_COM_MODE)
/*	COMPARE(x_scu);
	COMPARE(y_scu);
	COMPARE(cud);
	COMPARE(cu_width);
	COMPARE(cu_height);
	COMPARE(cu_width_log2);
	COMPARE(cu_height_log2);
	COMPARE(x_pos);
	COMPARE(y_pos);
	COMPARE(scup);
	COMPARE(cu_mode);
#if TB_SPLIT_EXT
	COMPARE(pb_part);
	COMPARE(tb_part);
#endif
	for (int i = 0; i < MAX_CU_DIM; ++i){
		COMPARE(rec_y[i]);
	}
	for (int i = 0; i < (MAX_CU_DIM >> 2); ++i){
		COMPARE(rec_u[i]);
		COMPARE(rec_v[i]);
	}
	for (int i = 0; i < MAX_CU_DIM; ++i){
		COMPARE(coef_y[i]);
	}
	for (int i = 0; i < (MAX_CU_DIM >> 2); ++i){
		COMPARE(coef_u[i]);
		COMPARE(coef_v[i]);
	}
	for (int i = 0; i < MAX_CU_DIM; ++i){
		COMPARE(pred_y[i]);
	}
	for (int i = 0; i < (MAX_CU_DIM >> 2); ++i){
		COMPARE(pred_u[i]);
		COMPARE(pred_v[i]);
	}
#if TB_SPLIT_EXT
	for (int i = 0; i < MAX_NUM_TB; ++i){
		for (int j = 0; j < N_C; ++j){
			COMPARE(num_nz[i][j]);
		}
	}
#else
	for (int i = 0; i < N_C; ++i){
		COMPARE(num_nz[i]);
	}
#endif
	for (int i = 0; i < REFP_NUM; ++i){
		COMPARE(refi[i]);
	}
	COMPARE(mvr_idx);
	COMPARE(umve_flag);
	COMPARE(umve_idx);
	COMPARE(skip_idx);
#if EXT_AMVR_HMVP
	COMPARE(mvp_from_hmvp_flag);
#endif
	for (int i = 0; i < REFP_NUM; ++i){
		for (int j = 0; j < MV_D; ++j){
			COMPARE(mvd[i][j]);
			COMPARE(mv[i][j]);
		}
	}
	COMPARE(affine_flag);
	for (int i = 0; i < REFP_NUM; ++i){
		for (int j = 0; j < VER_NUM; ++j){
			for (int k = 0; k < MV_D; ++k){
				COMPARE(affine_mv[i][j][k]);
				COMPARE(affine_mvd[i][j][k]);
			}
		}
	}
#if SMVD
	COMPARE(smvd_flag);
#endif
#if TB_SPLIT_EXT
	for (int i = 0; i < MAX_NUM_PB; ++i){
		for (int j = 0; j < 2; ++j){
			COMPARE(mpm[i][j]);
			COMPARE(ipm[i][j]);
		}
	}
#else
	for (int i = 0; i < 2; ++i){
		COMPARE(mpm[i]);
		COMPARE(ipm[i]);
	}
#endif
	COMPARE(ipf_flag);
	COMPARE(chroma_motion);
	COMPARE(x_luma);
	COMPARE(y_luma);
#undef COMPARE
	*/
OP_EQUAL_END


OP_EQUAL_BEGIN(IP_ARRAY)
#define COMPARE(data) _COMPARE(data, IP_ARRAY)
	for (int i = 0; i < 64; ++i){
		for (int j = 0; j < 64; ++j){
			COMPARE(temp_buffer[i][j]);//64/4
		}
	}
	for (int i = 0; i < 16; ++i){
		for (int j = 0; j < 64; ++j){
			COMPARE(up_innerLB[i][j]);//64/4
		}
	}
	for (int i = 0; i < 64; ++i){
		for (int j = 0; j < 16; ++j){
			COMPARE(left_innerLB[i][j]);//64/4
		}
	}
	for (int i = 0; i < max_buffer_width; ++i){
		COMPARE(up[i]);
	}
	for (int i = 0; i < 128; ++i){
		COMPARE(up_temp[i]);
	}
	for (int i = 0; i < 64; ++i){
		COMPARE(left[i]);
	}
	COMPARE(top_left);
	for (int i = 0; i < 2; ++i){
		for (int j = 0; j < 32; ++j){
			for (int k = 0; k < 32; ++k){
				COMPARE(temp_buffer_chroma[i][j][k]);
			}
		}
		for (int j = 0; j < max_buffer_width / 2; ++j){
			COMPARE(up_chroma[i][j]);
		}
		for (int j = 0; j < 32; ++j){
			COMPARE(left_chroma[i][j]);
		}
		COMPARE(top_left_chroma[i]);
	}
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(MPM_ARRAY)
#define COMPARE(data) _COMPARE(data, MPM_ARRAY)
	for (int i = 0; i < 16; ++i){
		for (int j = 0; j < 16; ++j){
			COMPARE(buffer[i][j]);//64/4
		}
	}
	for (int i = 0; i < max_buffer_width / 4; ++i){
		COMPARE(up[i]);//1920/4
	}
	for (int i = 0; i < 16; ++i){
		COMPARE(left_buffer[i]);
	}
	COMPARE(top_buffer_stride1);
	COMPARE(left_buffer_stride1);
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(IP_MAP_SCU)
#define COMPARE(data) _COMPARE(data, IP_MAP_SCU)
	COMPARE(min_scu_x);
	COMPARE(min_scu_y);
	COMPARE(curr_ctu_idx);
	COMPARE(above_line_idx);
	for (int i = 0; i < 2; ++i){
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j){
			for (int k = 0; k < SCU_IN_LCU_NUMS_LEN; ++k){
				COMPARE(map_scu_inner[i][j][k]);
			}
		}
		for (int j = 0; j < max_buffer_width >> MIN_CU_LOG2; ++j){
			COMPARE(map_scu_above_line[i][j]);
		}
	}
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(ENC_BEF_DATA)
#define COMPARE(data) _COMPARE(data, ENC_BEF_DATA)
	COMPARE(visit);
	COMPARE(nosplit);
	COMPARE(split);
	COMPARE(split_visit);
	for (int i = 0; i < MAX_SPLIT_NUM; ++i) {
		COMPARE(split_cost[i]);
	}
	COMPARE(mvr_idx_history);
	COMPARE(affine_flag_history);
#if EXT_AMVR_HMVP
	COMPARE(mvr_hmvp_idx_history);
#endif
#undef COMPARE
OP_EQUAL_END

OP_EQUAL_BEGIN(RDOQ_MODEL)
#define COMPARE(data) _COMPARE(data, RDOQ_MODEL)
	for (int i = 0; i < 3; ++i) {
		COMPARE(cbf[i]);
	}
	for (int i = 0; i < 24; ++i) {
		COMPARE(run_rdoq[i]);
		COMPARE(level[i]);
	}
	for (int i = 0; i < 12; ++i) {
		COMPARE(last1[i]);
	}
	for (int i = 0; i < 22; ++i) {
		COMPARE(last2[i]);
	}
#undef COMPARE	
OP_EQUAL_END

OP_EQUAL_BEGIN(ENC_CU_DATA)
#define COMPARE(data) _COMPARE(data, ENC_CU_DATA)
for (int i = 0; i < MAX_CU_DEPTH; ++i) {
	for (int j = 0; j < NUM_BLOCK_SHAPE; j++)
	{
		for (int k = 0; k < MAX_CU_CNT_IN_LCU; k++)
		{
			COMPARE(split_mode[i][j][k]);
		}
	}
}
for (int i = 0; i < CU_SIZE; i++)
{
	COMPARE(pb_part[i]);
	COMPARE(tb_part[i]);
	COMPARE(pred_mode[i]);
	for (int j = 0; j < 2; j++)
	{
		COMPARE(mpm[j][i]);
	}
	for (int j = 0; j < 8; j++)
	{
		COMPARE(mpm_ext[j][i]);
	}
	for (int j = 0; j < 2; j++)
	{
		COMPARE(ipm[j][i]);
	}
	for (int j = 0; j < REFP_NUM; j++)
	{
		COMPARE(refi[i][j]);
	}
	COMPARE(mvr_idx[i]);
	COMPARE(umve_flag[i]);
	COMPARE(umve_idx[i]);
	COMPARE(skip_idx[i]);

	COMPARE(mvp_from_hmvp_flag[i]);
	COMPARE(umve_flag[i]);
	COMPARE(umve_idx[i]);
	COMPARE(skip_idx[i]);
	for (int j = 0; j < N_C; j++)
	{
		COMPARE(num_nz_coef[j][i]);
	}
	COMPARE(map_scu[i]);
	COMPARE(affine_flag[i]);
	COMPARE(smvd_flag[i]);
	COMPARE(map_cu_mode[i]);
	COMPARE(map_pb_tb_part[i]);
	COMPARE(depth[i]);
	COMPARE(ipf_flag[i]);
}

for (int i = 0; i < MAX_CU_CNT_IN_LCU; i++)
{
	for (int j = 0; j < REFP_NUM; j++)
	{
		for (int k = 0; k < MV_D; k++)
		{
			COMPARE(mv[i][j][k]);
			COMPARE(mvd[i][j][k]);
		}
	}
}

for (int i = 0; i < CU_SIZE << 4; i++)
{
	COMPARE(coef_y[i]);
	COMPARE(reco_y[i]);
}
for (int i = 0; i < CU_SIZE << 2; i++)
{
	COMPARE(coef_u[i]);
	COMPARE(coef_v[i]);
	COMPARE(reco_u[i]);
	COMPARE(reco_v[i]);
}
#undef COMPARE	
OP_EQUAL_END

OP_EQUAL_BEGIN(MD_OUT_PARA_SET)
#define COMPARE(data) _COMPARE(data, MD_OUT_PARA_SET)
COMPARE(pic_width);
COMPARE(pic_height);
COMPARE(slice_type);
COMPARE(max_dt_size);
COMPARE(affine_enable_flag);
COMPARE(dt_intra_enable_flag);
COMPARE(num_of_hmvp_cand);
COMPARE(ipf_enable_flag);
COMPARE(pic_width_in_scu);
COMPARE(pic_height_in_scu);
COMPARE(log2_max_cuwh);
COMPARE(log2_culine);
COMPARE(max_cuwh);
COMPARE(f_scu);
COMPARE(gop_size);

#if CTX_CONTROL_LUKE
COMPARE(bit_depth_internal);
COMPARE(max_part_ratio);
COMPARE(min_cu_size);
COMPARE(max_split_times);
COMPARE(min_qt_size);
COMPARE(max_bt_size);
COMPARE(max_eqt_size);
COMPARE(affine_subblock_size_idx);
COMPARE(ipcm_enable_flag);
COMPARE(position_based_transform_enable_flag);
COMPARE(secondary_transform_enable_flag);
COMPARE(tscpm_enable_flag);
COMPARE(amvr_enable_flag);
COMPARE(emvr_enable_flag);
#endif

for (int i = 0; i < 17; i++)
{
	for (int j = 0; j < 2; j++)
	{
		COMPARE(refp_ptr[i][j]);
	}
}

COMPARE(num_refp);
COMPARE(cur_ptr);
COMPARE(pix_x);
COMPARE(pix_y);
#if CTU_CONTROL_LUKE
COMPARE(lcu_cnt);
#endif

#if CTU_CONTROL_LUKE
COMPARE(qp_y);
COMPARE(qp_u);
COMPARE(qp_v);
for (int i = 0; i < 3; i++)
{
	COMPARE(lambda[i]);
	COMPARE(sqrt_lambda[i]);
}

for (int i = 0; i < 2; i++)
{
	COMPARE(dist_chroma_weight[i]);
}

COMPARE(ptr);
COMPARE(x_last_ctu);
COMPARE(y_last_ctu);
#endif

for (int i = 0; i < RANGE_NUM; i++)
{
	for (int j = 0; j < DIR_DIM; j++)
	{
		for (int k = 0; k < MAP_DIMS; k++)
		{
			COMPARE(CtrPos[i][j][k]);
		}
	}
}

COMPARE(x_chroma);
COMPARE(y_chroma);
COMPARE(last_child);

#undef COMPARE	
OP_EQUAL_END


#undef _COMPARE

#define _COMPARE(data, type) if(left.data != right.data){ printf("%s::%s is wrong!\n", #type, #data); res = false; }
#define _COMPARE_PTR(ptr, type) if(*left.ptr != *right.ptr){ printf("%s::%s is wrong!\n", #type, #ptr); res = false; }

OP_EQUAL_BEGIN(MD_PORT)
#define COMPARE(data) _COMPARE(data, MD_PORT)
#define COMPARE_PTR(data) _COMPARE_PTR(data, MD_PORT)
	COMPARE_PTR(Fetch_Ref_window_ptr);
	COMPARE_PTR(Fetch_Orig_Lcu_ptr);
	COMPARE_PTR(rdoq_model);
	COMPARE_PTR(model_cnt);
	COMPARE_PTR(md2_output);
	COMPARE_PTR(md_out_para_set);
#undef COMPARE
OP_EQUAL_END


 MD_PORT md_tb, md_golden;
int md_testbench()
{
	FILE* fp_in_golden = fopen(INPUT_GOLDEN, "rb");
	if (!fp_in_golden){
		printf("Open file %s failed!\n", INPUT_GOLDEN);
		return 1;
	}
	FILE* fp_out_golden = fopen(OUTPUT_GOLDEN, "rb");
	if (!fp_out_golden){
		printf("Open file %s failed!\n", OUTPUT_GOLDEN);
		return 1;
	}
	FILE* fp_in_tb = fopen(INPUT_TB, "wb");
	if (!fp_in_tb){
		printf("Open file %s failed!\n", INPUT_TB);
		return 1;
	}
	FILE* fp_out_tb = fopen(OUTPUT_TB, "wb");
	if (!fp_out_tb){
		printf("Open file %s failed!\n", OUTPUT_TB);
		return 1;
	}
	bool pass = true;
	int i, cnt, fail;

	for (i = 0, cnt = 0, fail = 0; i < 100 &&(cnt = md_tb.read_bin(fp_in_golden)) && md_golden.read_bin(fp_out_golden); ++i){
		//printf("PORT: %d\n", cnt);
		cnt = md_tb.write_bin(fp_in_tb);
		//printf("PORT: %d\n", cnt);
		if (!cnt) { printf("Loop %d: md_in_tb write failed!\n", i); }
		fflush(fp_in_tb);
		fflush(stdout);
#ifdef DEBUG_MD_TB
		if (i == DEBUG_MD_NUM) {
			return 0;
		}
#endif

		md_tb.run_md();

		if (!md_tb.write_bin(fp_out_tb)) { printf("Loop %d: md_out_tb write failed!\n", i); }
		fflush(fp_out_tb);

		if (md_tb == md_golden){
			printf("The %dth data is passed!\n", i);
		}
		else{
			printf("The %dth data is wrong!\n", i);
			++fail;
			pass = false;
		}
	}
	if (pass) { printf("All %d test success!\n", i); }
	else { printf("%d/%d test failed!\n", fail, i); }

	fclose(fp_in_golden);
	fclose(fp_out_golden);
	fclose(fp_in_tb);
	fclose(fp_out_tb);

	return pass ? 0 : 1;
}

int main()
{
	return md_testbench();
}
