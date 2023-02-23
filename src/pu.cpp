#include "md_cmod.h"
#include "md1.h"
#include "md2.h"
#include "md_kernel_64.h"

#if !ONLY_QT_QT
template <>
void pu<KERNEL_8X64>(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	out_buf.cost_temp = md_64x8_8x64_loop(
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		&in_buf.me_line_map_64,
		in_buf.up_nebs_64, in_buf.left_nebs_64,
		&in_buf.buffer.skip_in, 
		true, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		&out_buf.curr_cu_data_temp_64,
		&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child,
		&in_buf.buffer.fme_mv_info,
		&in_buf.IP_buffer_16x64, &in_buf.IP_buffer_64x16,
		&in_buf.MPM_buffer_16x64, &in_buf.MPM_buffer_64x16,
		&in_buf.IP_map_scu_16x64, &in_buf.IP_map_scu_64x16,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);
}

template <>
void pu<KERNEL_64X8>(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	out_buf.cost_temp = md_64x8_8x64_loop(
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		&in_buf.me_line_map_64,
		in_buf.up_nebs_64, in_buf.left_nebs_64,
		&in_buf.buffer.skip_in,
		true, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		&out_buf.curr_cu_data_temp_64,
		&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child,
		&in_buf.buffer.fme_mv_info,
		&in_buf.IP_buffer_16x64, &in_buf.IP_buffer_64x16,
		&in_buf.MPM_buffer_16x64, &in_buf.MPM_buffer_64x16,
		&in_buf.IP_map_scu_16x64, &in_buf.IP_map_scu_64x16,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);
}

template <>
void pu<KERNEL_16X32>(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	out_buf.cost_temp = md_32x32_32x16_16x32_loop_md2(SPLIT_BI_HOR,	//TODO: 确认好是SPLIT_BI_VER还是SPLIT_BI_HOR
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		&in_buf.me_line_map_64,
		in_buf.up_nebs_64, in_buf.left_nebs_64,
		&in_buf.buffer.skip_in,
		true, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		&out_buf.curr_cu_data_temp_64,
		&out_buf.cons_pred_mode, &out_buf.tree_status_child,
		&in_buf.buffer.fme_mv_info,
		&in_buf.IP_buffer_16x64, &in_buf.IP_buffer_64x16,
		&in_buf.MPM_buffer_16x64, &in_buf.MPM_buffer_64x16,
		&in_buf.IP_map_scu_16x64, &in_buf.IP_map_scu_64x16,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);
}

template <>
void pu<KERNEL_32X16>(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	out_buf.cost_temp = md_32x32_32x16_16x32_loop_md2(SPLIT_BI_VER,	//TODO: 确认好是SPLIT_BI_VER还是SPLIT_BI_HOR
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		&in_buf.me_line_map_64,
		in_buf.up_nebs_64, in_buf.left_nebs_64,
		&in_buf.buffer.skip_in,
		true, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		&out_buf.curr_cu_data_temp_64,
		&out_buf.cons_pred_mode, &out_buf.tree_status_child,
		&in_buf.buffer.fme_mv_info,
		&in_buf.IP_buffer_16x64, &in_buf.IP_buffer_64x16,
		&in_buf.MPM_buffer_16x64, &in_buf.MPM_buffer_64x16,
		&in_buf.IP_map_scu_16x64, &in_buf.IP_map_scu_64x16,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);
}

template <>
void pu<KERNEL_16X64>(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(&in_buf.buffer.g_rmd_output, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.x0, in_buf.insn.y0, ip_list, in_buf.config.md_input_ptr.speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(&in_buf.buffer.g_rmd_output, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.x0, in_buf.insn.y0, ip_list);
#endif

	U1 boundary = !(in_buf.insn.x0 + in_buf.insn.cu_width <= in_buf.config.md_fw_ptr.pic_width && in_buf.insn.y0 + in_buf.insn.cu_height <= in_buf.config.md_fw_ptr.pic_height);
	U2 NS_luma_pred_mode = ONLY_INTRA;

	STATIC_C IP_ARRAY_SIZE<64, 64> null_IP_buffer_ptr_64x64;
	STATIC_C IP_ARRAY_SIZE<32, 64> null_IP_buffer_ptr_32x64;
	STATIC_C IP_ARRAY_SIZE<64, 32> null_IP_buffer_ptr_64x32;
	STATIC_C MPM_ARRAY_SIZE<64, 64> null_MPM_buffer_ptr_64x64;
	STATIC_C MPM_ARRAY_SIZE<32, 64> null_MPM_buffer_ptr_32x64;
	STATIC_C MPM_ARRAY_SIZE<64, 32> null_MPM_buffer_ptr_64x32;
	STATIC_C IP_MAP_SCU_SIZE<64, 64> null_IP_map_scu_ptr_64x64;
	STATIC_C IP_MAP_SCU_SIZE<32, 64> null_IP_map_scu_ptr_32x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 32> null_IP_map_scu_ptr_64x32;

	STATIC_C IP_ARRAY_SIZE<8, 64> null_IP_buffer_ptr_8x64;
	STATIC_C IP_ARRAY_SIZE<64, 8> null_IP_buffer_ptr_64x8;
	STATIC_C MPM_ARRAY_SIZE<8, 64> null_MPM_buffer_ptr_8x64;
	STATIC_C MPM_ARRAY_SIZE<64, 8> null_MPM_buffer_ptr_64x8;
	STATIC_C IP_MAP_SCU_SIZE<8, 64> null_IP_map_scu_ptr_8x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 8> null_IP_map_scu_ptr_64x8;

	out_buf.cost_temp = md_kernel_64(in_buf.insn.cu_width, in_buf.insn.cu_height,
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, ip_list,
#if ENABLE_BFRAME
		& in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		&in_buf.me_line_map_64,
		in_buf.up_nebs_64, in_buf.left_nebs_64,
		&in_buf.buffer.skip_in, boundary, true, &NS_luma_pred_mode,
		in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2,
		in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.tree_status,
		&out_buf.curr_cu_data_temp_64,
		&in_buf.buffer.fme_mv_info,
		&null_IP_buffer_ptr_64x64, &null_IP_buffer_ptr_32x64, &null_IP_buffer_ptr_64x32, &in_buf.IP_buffer_16x64, &in_buf.IP_buffer_64x16,
		&null_IP_buffer_ptr_8x64,
		&null_IP_buffer_ptr_64x8,
		&null_MPM_buffer_ptr_64x64, &null_MPM_buffer_ptr_32x64, &null_MPM_buffer_ptr_64x32, &in_buf.MPM_buffer_16x64, &in_buf.MPM_buffer_64x16,
		&null_MPM_buffer_ptr_8x64,
		&null_MPM_buffer_ptr_64x8,
		&null_IP_map_scu_ptr_64x64, &null_IP_map_scu_ptr_32x64, &null_IP_map_scu_ptr_64x32, &in_buf.IP_map_scu_16x64, &in_buf.IP_map_scu_64x16,
		&null_IP_map_scu_ptr_8x64,
		&null_IP_map_scu_ptr_64x8,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);

	out_buf.split_mode = in_buf.insn.split_mode;
	out_buf.tag = in_buf.insn.tag;

#if KERNEL_SIZE_CLOSE(16, 64)
	out_buf.cost_temp = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(16, 64)
}

template <>
void pu<KERNEL_64X16>(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	// TODO: HLS预处理先去掉了，后面加上

#if USE_SPEED_LEVEL
	static u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];
	get_rmd_result_new(&in_buf.buffer.g_rmd_output, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.x0, in_buf.insn.y0, ip_list, in_buf.config.md_input_ptr.speed_level);
#else
	static u8 ip_list[IPD_RDO_CNT];
	get_rmd_result_new(&in_buf.buffer.g_rmd_output, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.x0, in_buf.insn.y0, ip_list);
#endif

	U1 boundary = !(in_buf.insn.x0 + in_buf.insn.cu_width <= in_buf.config.md_fw_ptr.pic_width && in_buf.insn.y0 + in_buf.insn.cu_height <= in_buf.config.md_fw_ptr.pic_height);
	U2 NS_luma_pred_mode = ONLY_INTRA;


	STATIC_C IP_ARRAY_SIZE<64, 64> null_IP_buffer_ptr_64x64;
	STATIC_C IP_ARRAY_SIZE<32, 64> null_IP_buffer_ptr_32x64;
	STATIC_C IP_ARRAY_SIZE<64, 32> null_IP_buffer_ptr_64x32;
	STATIC_C MPM_ARRAY_SIZE<64, 64> null_MPM_buffer_ptr_64x64;
	STATIC_C MPM_ARRAY_SIZE<32, 64> null_MPM_buffer_ptr_32x64;
	STATIC_C MPM_ARRAY_SIZE<64, 32> null_MPM_buffer_ptr_64x32;
	STATIC_C IP_MAP_SCU_SIZE<64, 64> null_IP_map_scu_ptr_64x64;
	STATIC_C IP_MAP_SCU_SIZE<32, 64> null_IP_map_scu_ptr_32x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 32> null_IP_map_scu_ptr_64x32;

	STATIC_C IP_ARRAY_SIZE<8, 64> null_IP_buffer_ptr_8x64;
	STATIC_C IP_ARRAY_SIZE<64, 8> null_IP_buffer_ptr_64x8;
	STATIC_C MPM_ARRAY_SIZE<8, 64> null_MPM_buffer_ptr_8x64;
	STATIC_C MPM_ARRAY_SIZE<64, 8> null_MPM_buffer_ptr_64x8;
	STATIC_C IP_MAP_SCU_SIZE<8, 64> null_IP_map_scu_ptr_8x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 8> null_IP_map_scu_ptr_64x8;

	out_buf.cost_temp = md_kernel_64(in_buf.insn.cu_width, in_buf.insn.cu_height,
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, ip_list,
#if ENABLE_BFRAME
		&in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		&in_buf.me_line_map_64,
		in_buf.up_nebs_64, in_buf.left_nebs_64,
		&in_buf.buffer.skip_in, boundary, true, &NS_luma_pred_mode,
		in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2,
		in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.tree_status,
		&out_buf.curr_cu_data_temp_64,
		&in_buf.buffer.fme_mv_info,
		&null_IP_buffer_ptr_64x64, &null_IP_buffer_ptr_32x64, &null_IP_buffer_ptr_64x32, &in_buf.IP_buffer_16x64, &in_buf.IP_buffer_64x16,
		&null_IP_buffer_ptr_8x64,
		&null_IP_buffer_ptr_64x8,
		&null_MPM_buffer_ptr_64x64, &null_MPM_buffer_ptr_32x64, &null_MPM_buffer_ptr_64x32, &in_buf.MPM_buffer_16x64, &in_buf.MPM_buffer_64x16,
		&null_MPM_buffer_ptr_8x64,
		&null_MPM_buffer_ptr_64x8,
		&null_IP_map_scu_ptr_64x64, &null_IP_map_scu_ptr_32x64, &null_IP_map_scu_ptr_64x32, &in_buf.IP_map_scu_16x64, &in_buf.IP_map_scu_64x16,
		&null_IP_map_scu_ptr_8x64,
		&null_IP_map_scu_ptr_64x8,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);

	out_buf.split_mode = in_buf.insn.split_mode;
	out_buf.tag = in_buf.insn.tag;

#if KERNEL_SIZE_CLOSE(16, 64)
	out_buf.cost_temp = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(16, 64)
}
#endif

void copy_InstructionPU(InstructionPU &src_insn, InstructionPU &dst_insn)
{
	dst_insn.x0 = src_insn.x0;
	dst_insn.y0 = src_insn.y0;
	dst_insn.cup = src_insn.cup;
	dst_insn.cu_width_log2 = src_insn.cu_width_log2;
	dst_insn.cud = src_insn.cud;
	dst_insn.qt_depth = src_insn.qt_depth;
	dst_insn.bet_depth = src_insn.bet_depth;
	dst_insn.cons_pred_mode = src_insn.cons_pred_mode;
	dst_insn.cu_width = src_insn.cu_width;
	dst_insn.cu_height = src_insn.cu_height;
	dst_insn.tree_status = src_insn.tree_status;
	dst_insn.split_mode = src_insn.split_mode;
	dst_insn.tag = src_insn.tag;
}
void copy_MD_FW(MD_FW &src_md_fw_ptr, MD_FW &dst_md_fw_ptr)
{
	dst_md_fw_ptr.pic_width = src_md_fw_ptr.pic_width;
	dst_md_fw_ptr.pic_height = src_md_fw_ptr.pic_height;
	dst_md_fw_ptr.slice_type = src_md_fw_ptr.slice_type;
	dst_md_fw_ptr.pic_width_in_scu = src_md_fw_ptr.pic_width_in_scu;
	dst_md_fw_ptr.pic_height_in_scu = src_md_fw_ptr.pic_height_in_scu;
	dst_md_fw_ptr.log2_culine = src_md_fw_ptr.log2_culine;
	dst_md_fw_ptr.bit_depth_internal = src_md_fw_ptr.bit_depth_internal;
	dst_md_fw_ptr.max_part_ratio = src_md_fw_ptr.max_part_ratio;
	dst_md_fw_ptr.min_cu_size = src_md_fw_ptr.min_cu_size;
	dst_md_fw_ptr.max_split_times = src_md_fw_ptr.max_split_times;
	dst_md_fw_ptr.min_qt_size = src_md_fw_ptr.min_qt_size;
	dst_md_fw_ptr.max_bt_size = src_md_fw_ptr.max_bt_size;
	dst_md_fw_ptr.max_eqt_size = src_md_fw_ptr.max_eqt_size;
	dst_md_fw_ptr.mipf_enable_flag = src_md_fw_ptr.mipf_enable_flag;
	dst_md_fw_ptr.refp_ptr[0][0] = src_md_fw_ptr.refp_ptr[0][0];
	dst_md_fw_ptr.refp_ptr[0][1] = src_md_fw_ptr.refp_ptr[0][1];
	dst_md_fw_ptr.refp_ptr[1][0] = src_md_fw_ptr.refp_ptr[1][0];
	dst_md_fw_ptr.refp_ptr[1][1] = src_md_fw_ptr.refp_ptr[1][1];
	dst_md_fw_ptr.refp_ptr[2][0] = src_md_fw_ptr.refp_ptr[2][0];
	dst_md_fw_ptr.refp_ptr[2][1] = src_md_fw_ptr.refp_ptr[2][1];
	dst_md_fw_ptr.refp_ptr[3][0] = src_md_fw_ptr.refp_ptr[3][0];
	dst_md_fw_ptr.refp_ptr[3][1] = src_md_fw_ptr.refp_ptr[3][1];
	dst_md_fw_ptr.refp_ptr[4][0] = src_md_fw_ptr.refp_ptr[4][0];
	dst_md_fw_ptr.refp_ptr[4][1] = src_md_fw_ptr.refp_ptr[4][1];
	dst_md_fw_ptr.refp_ptr[5][0] = src_md_fw_ptr.refp_ptr[5][0];
	dst_md_fw_ptr.refp_ptr[5][1] = src_md_fw_ptr.refp_ptr[5][1];
	dst_md_fw_ptr.refp_ptr[6][0] = src_md_fw_ptr.refp_ptr[6][0];
	dst_md_fw_ptr.refp_ptr[6][1] = src_md_fw_ptr.refp_ptr[6][1];
	dst_md_fw_ptr.refp_ptr[7][0] = src_md_fw_ptr.refp_ptr[7][0];
	dst_md_fw_ptr.refp_ptr[7][1] = src_md_fw_ptr.refp_ptr[7][1];
	dst_md_fw_ptr.refp_ptr[8][0] = src_md_fw_ptr.refp_ptr[8][0];
	dst_md_fw_ptr.refp_ptr[8][1] = src_md_fw_ptr.refp_ptr[8][1];
	dst_md_fw_ptr.refp_ptr[9][0] = src_md_fw_ptr.refp_ptr[9][0];
	dst_md_fw_ptr.refp_ptr[9][1] = src_md_fw_ptr.refp_ptr[9][1];
	dst_md_fw_ptr.refp_ptr[10][0] = src_md_fw_ptr.refp_ptr[10][0];
	dst_md_fw_ptr.refp_ptr[10][1] = src_md_fw_ptr.refp_ptr[10][1];
	dst_md_fw_ptr.refp_ptr[11][0] = src_md_fw_ptr.refp_ptr[11][0];
	dst_md_fw_ptr.refp_ptr[11][1] = src_md_fw_ptr.refp_ptr[11][1];
	dst_md_fw_ptr.refp_ptr[12][0] = src_md_fw_ptr.refp_ptr[12][0];
	dst_md_fw_ptr.refp_ptr[12][1] = src_md_fw_ptr.refp_ptr[12][1];
	dst_md_fw_ptr.refp_ptr[13][0] = src_md_fw_ptr.refp_ptr[13][0];
	dst_md_fw_ptr.refp_ptr[13][1] = src_md_fw_ptr.refp_ptr[13][1];
	dst_md_fw_ptr.refp_ptr[14][0] = src_md_fw_ptr.refp_ptr[14][0];
	dst_md_fw_ptr.refp_ptr[14][1] = src_md_fw_ptr.refp_ptr[14][1];
	dst_md_fw_ptr.refp_ptr[15][0] = src_md_fw_ptr.refp_ptr[15][0];
	dst_md_fw_ptr.refp_ptr[15][1] = src_md_fw_ptr.refp_ptr[15][1];
	dst_md_fw_ptr.refp_ptr[16][0] = src_md_fw_ptr.refp_ptr[16][0];
	dst_md_fw_ptr.refp_ptr[16][1] = src_md_fw_ptr.refp_ptr[16][1];
	dst_md_fw_ptr.cur_ptr = src_md_fw_ptr.cur_ptr;
 #if ENABLE_BFRAME
 	cpy_1d_2_1d<U5, REFP_NUM>(src_md_fw_ptr.num_refp, dst_md_fw_ptr.num_refp);
 #else
 	dst_md_fw_ptr.num_refp = src_md_fw_ptr.num_refp;
 #endif
}
void copy_MD_INPUT(MD_INPUT &src_md_input_ptr, MD_INPUT &dst_md_input_ptr)
{
	dst_md_input_ptr.pix_x = src_md_input_ptr.pix_x;
	dst_md_input_ptr.pix_y = src_md_input_ptr.pix_y;
	dst_md_input_ptr.lambda_y = src_md_input_ptr.lambda_y;
	dst_md_input_ptr.qp_y = src_md_input_ptr.qp_y;
	dst_md_input_ptr.CtrPos2MD[0][0][0] = src_md_input_ptr.CtrPos2MD[0][0][0];
	dst_md_input_ptr.CtrPos2MD[0][0][1] = src_md_input_ptr.CtrPos2MD[0][0][1];
	dst_md_input_ptr.CtrPos2MD[0][1][0] = src_md_input_ptr.CtrPos2MD[0][1][0];
	dst_md_input_ptr.CtrPos2MD[0][1][1] = src_md_input_ptr.CtrPos2MD[0][1][1];
	dst_md_input_ptr.CtrPos2MD[1][0][0] = src_md_input_ptr.CtrPos2MD[1][0][0];
	dst_md_input_ptr.CtrPos2MD[1][1][0] = src_md_input_ptr.CtrPos2MD[1][1][0];
	dst_md_input_ptr.CtrPos2MD_ref1[0][0][0] = src_md_input_ptr.CtrPos2MD_ref1[0][0][0];
	dst_md_input_ptr.CtrPos2MD_ref1[0][0][1] = src_md_input_ptr.CtrPos2MD_ref1[0][0][1];
	dst_md_input_ptr.CtrPos2MD_ref1[0][1][0] = src_md_input_ptr.CtrPos2MD_ref1[0][1][0];
	dst_md_input_ptr.CtrPos2MD_ref1[0][1][1] = src_md_input_ptr.CtrPos2MD_ref1[0][1][1];
	dst_md_input_ptr.CtrPos2MD_ref1[1][0][0] = src_md_input_ptr.CtrPos2MD_ref1[1][0][0];
	dst_md_input_ptr.CtrPos2MD_ref1[1][1][0] = src_md_input_ptr.CtrPos2MD_ref1[1][1][0];
	dst_md_input_ptr.cu_dqp_enable = src_md_input_ptr.cu_dqp_enable;
}
void pu_KERNEL_16X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_16x16& axis_in, PURamOut_16x16& ram_out)
{
	// stream port unused may cause core dump
#pragma HLS array_partition variable=in_buf.config.md_fw_ptr.refp_ptr complete dim=1
#pragma HLS array_partition variable=in_buf.config.md_fw_ptr.refp_ptr complete dim=2

#if 1 // Align with PU_8x8
// #pragma HLS ARRAY_PARTITION variable=in_buf.config.md_fw_ptr.refp_ptr 				dim=0 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_fw_ptr.num_refp					dim=1 complete	// NEED AXIS
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1					dim=1 complete	// LOAD HLS problem
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1					dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1					dim=3 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1				dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1				dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1				dim=3 complete
#pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_list_ptr1				dim=1 complete
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_input_ptr.CtrPos2MD 				dim=1 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_input_ptr.CtrPos2MD 				dim=2 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_input_ptr.CtrPos2MD 				dim=3 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_input_ptr.CtrPos2MD_ref1  		dim=1 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_input_ptr.CtrPos2MD_ref1  		dim=2 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_input_ptr.CtrPos2MD_ref1  		dim=3 complete	// NEED AXIS
#endif


#if 1

#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_eqt_size
//#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[0][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[0][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[1][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[1][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[2][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[2][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[3][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[3][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[4][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[4][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[5][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[5][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[6][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[6][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[7][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[7][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[8][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[8][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[9][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[9][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[10][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[10][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[11][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[11][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[12][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[12][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[13][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[13][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[14][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[14][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[15][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[15][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[16][0]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.refp_ptr[16][1]
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=in_buf.config.min_scu_x
#pragma HLS INTERFACE axis port=in_buf.config.min_scu_y
//#pragma HLS INTERFACE axis port=in_buf.buffer.skip_in.slice_type
//#pragma HLS INTERFACE axis port=in_buf.buffer.skip_in.ptr
//#pragma HLS INTERFACE axis port=in_buf.buffer.skip_in.umve_enable_flag
//#pragma HLS INTERFACE axis port=in_buf.buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=in_buf.insn.x0
#pragma HLS INTERFACE axis port=in_buf.insn.y0
#pragma HLS INTERFACE axis port=in_buf.insn.cup
#pragma HLS INTERFACE axis port=in_buf.insn.cu_width_log2
#pragma HLS INTERFACE axis port=in_buf.insn.cud
#pragma HLS INTERFACE axis port=in_buf.insn.qt_depth
#pragma HLS INTERFACE axis port=in_buf.insn.bet_depth
#pragma HLS INTERFACE axis port=in_buf.insn.cons_pred_mode
#pragma HLS INTERFACE axis port=in_buf.insn.cu_width
#pragma HLS INTERFACE axis port=in_buf.insn.cu_height
#pragma HLS INTERFACE axis port=in_buf.insn.tree_status
#pragma HLS INTERFACE axis port=in_buf.insn.split_mode
#pragma HLS INTERFACE axis port=in_buf.insn.tag
//#pragma HLS INTERFACE axis port=in_buf.line_buffer.y0
//#pragma HLS INTERFACE axis port=out_buf.cost_temp
//#pragma HLS INTERFACE axis port=out_buf.cons_pred_mode
//#pragma HLS INTERFACE axis port=out_buf.tree_status_child
//#pragma HLS INTERFACE axis port=out_buf.split_mode
//#pragma HLS INTERFACE axis port=out_buf.tag
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_slice_type
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_ptr
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_umve_enable_flag
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru
#pragma HLS INTERFACE axis port=axis_in.in_buf_line_buffer_y0

#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp_1

#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_1
#if ENABLE_BFRAME
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_1
#endif // ENABLE_BFRAME	
#endif

#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1


#pragma HLS RESOURCE variable=in_buf.buffer core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.me_line_map_16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.up_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.left_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.MPM_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_map_scu_16x16 core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=out_buf.curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_split_mode  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_tag  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_cons_pred_mode  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_tree_status_child  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_cost_temp  core=RAM_1P_BRAM
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
#pragma HLS array_partition variable=out_buf.curr_cu_data_temp.mvd complete dim=2
#pragma HLS array_partition variable=out_buf.curr_cu_data_temp.mvd complete dim=3
#pragma HLS array_partition variable=out_buf.curr_cu_data_temp.num_nz_coef complete dim=1
	
	// assigne seperated axis ports
	in_buf.buffer.skip_in.slice_type = axis_in.in_buf_buffer_skip_in_slice_type;
	in_buf.buffer.skip_in.ptr = axis_in.in_buf_buffer_skip_in_ptr;
	in_buf.buffer.skip_in.umve_enable_flag = axis_in.in_buf_buffer_skip_in_umve_enable_flag;
	in_buf.buffer.Fetch_Ref_window_ptr.code_stru = axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru;
	in_buf.line_buffer.y0 = axis_in.in_buf_line_buffer_y0;

	in_buf.config.md_fw_ptr.num_refp[0] = axis_in.in_buf_config_md_fw_ptr_num_refp_0;
	in_buf.config.md_fw_ptr.num_refp[1] = axis_in.in_buf_config_md_fw_ptr_num_refp_1;

	in_buf.config.md_input_ptr.CtrPos2MD[0][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1;
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1;
	in_buf.config.md_input_ptr.CtrPos2MD[1][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD[1][0][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_1;
	in_buf.config.md_input_ptr.CtrPos2MD[1][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD[1][1][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_1;
	#if ENABLE_BFRAME
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_1;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_1;
	#endif // ENABLE_BFRAME	

    STATIC_C IP_ARRAY_SIZE<16, 16> IP_buffer;
    STATIC_C MPM_ARRAY_SIZE<16, 16> MPM_buffer;
    STATIC_C ENC_ME_LINE_MAP_SIZE<16> me_line_map;
    STATIC_C ENC_ME_NEB_INFO up_nebs[6];
    STATIC_C ENC_ME_NEB_INFO left_nebs[4];
    STATIC_C IP_MAP_SCU_SIZE<16, 16> IP_map_scu;

	InstructionPU insn;
    MD_INPUT md_input_ptr;
	MD_FW md_fw_ptr;
	s64 cost_temp;
	U2 cons_pred_mode;
	U2 tree_status_child;

	
    copy_InstructionPU(in_buf.insn, insn);
	copy_MD_FW(in_buf.config.md_fw_ptr, md_fw_ptr);
    copy_MD_INPUT(in_buf.config.md_input_ptr, md_input_ptr);

	load16(insn.x0, insn.y0, me_line_map, IP_buffer, MPM_buffer, up_nebs, left_nebs, IP_map_scu, in_buf.line_buffer, in_buf.config);

	cost_temp = md_16x16_16x8_8x16_loop_md1(insn.split_mode,
		&/*in_buf.config.*/md_fw_ptr, &md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		& me_line_map,
		up_nebs, left_nebs,
		&in_buf.buffer.skip_in,
		true, insn.x0, insn.y0, insn.cup, insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.qt_depth, insn.bet_depth, insn.cons_pred_mode, insn.cu_width, insn.cu_height, ONLY_INTRA,
		&out_buf.curr_cu_data_temp,
		&cons_pred_mode, insn.tree_status, &tree_status_child, &in_buf.buffer.fme_mv_info,
		& IP_buffer,
		& MPM_buffer,
		& IP_map_scu,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);

	U8 y0 = in_buf.line_buffer.y0;
	copy_excepty0(out_buf.line_buffer, in_buf.line_buffer);
	update16(insn.x0, insn.y0, insn.cu_width, insn.cu_height, y0, in_buf.line_buffer, out_buf.line_buffer, out_buf.curr_cu_data_temp);

	out_buf.split_mode = insn.split_mode;
	out_buf.tag = insn.tag;
	out_buf.cons_pred_mode = cons_pred_mode;
	out_buf.tree_status_child = tree_status_child;
#if KERNEL_SIZE_CLOSE(16, 16)
	out_buf.cost_temp = MAX_S64;
#else
	out_buf.cost_temp = cost_temp;
#endif // !KERNEL_SIZE_CLOSE(16, 16)

    ram_out.out_buf_split_mode[0] = insn.split_mode;
	ram_out.out_buf_tag[0] = insn.tag;
	ram_out.out_buf_cons_pred_mode[0] = cons_pred_mode;
	ram_out.out_buf_tree_status_child[0] = tree_status_child;
#if KERNEL_SIZE_CLOSE(16, 16)
	ram_out.out_buf_cost_temp[0] = MAX_S64;
#else
	ram_out.out_buf_cost_temp[0] = cost_temp;
#endif // !KERNEL_SIZE_CLOSE(16, 16)
}

#if PU_8X16_AXIS_ADAPTER
void pu_KERNEL_8X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_8x16& axis_in, PURamOutPorts_8x16& ram_out)
#else
void pu_KERNEL_8X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
#endif // PU_8X16_AXIS_ADAPTER
{
	// stream port unused may cause core dump
#if 1 // Align with PU_8x8
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_fw_ptr.refp_ptr 			dim=0 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_fw_ptr.num_refp			dim=1 complete	// NEED AXIS
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1			dim=1 complete		// LOAD HLS problem
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1			dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1			dim=3 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1		dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1		dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1		dim=3 complete
#pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_list_ptr1		dim=1 complete
#endif

#if PU_8X16_AXIS_ADAPTER
#pragma HLS ARRAY_PARTITION variable=axis_in.in_buf_config_md_fw_ptr_refp_ptr 			dim=0 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=axis_in.in_buf_config_md_fw_ptr_num_refp			dim=1 complete	// NEED AXIS
#endif

#if pu_interface & 0
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr
#pragma HLS INTERFACE axis port=in_buf.insn
#pragma HLS INTERFACE axis port=in_buf.cost_best
#pragma HLS INTERFACE axis port=out_buf.cost_temp
#pragma HLS INTERFACE axis port=out_buf.cons_pred_mode
#pragma HLS INTERFACE axis port=out_buf.tree_status_child
#pragma HLS INTERFACE axis port=out_buf.split_mode
#pragma HLS INTERFACE axis port=out_buf.tag
#endif

#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1

#if PU_8X16_AXIS_ADAPTER
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width                
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height               
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_slice_type               
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu         
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu        
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_log2_culine              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_bit_depth_internal       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_part_ratio           
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_cu_size              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_split_times          
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_qt_size              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_bt_size              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_eqt_size             
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag         
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_cur_ptr                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_pix_x                 
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_pix_y                 
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_lambda_y              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_qp_y                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_cu_dqp_enable       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp     
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_min_scu_x                          
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_min_scu_y                          
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_slice_type                 
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_ptr                        
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_umve_enable_flag           
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru     
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_x0                                   
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_y0                                   
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cup                                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width_log2                        
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cud                                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_qt_depth                             
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_bet_depth                            
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cons_pred_mode                       
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width                             
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_height                            
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tree_status                          
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_split_mode                           
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tag                                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_line_buffer_y0    
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp[0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp[1]   
#pragma HLS RESOURCE variable=ram_out.out_buf_line_buffer_y0 core=RAM_1P_BRAM    
#pragma HLS RESOURCE variable=ram_out.out_buf_cost_temp core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_cons_pred_mode core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_tree_status_child core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_split_mode core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_tag core=RAM_1P_BRAM                                        
#endif // PU_8X16_AXIS_ADAPTER

#pragma HLS RESOURCE variable=in_buf.buffer core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.me_line_map_16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.up_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.left_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.MPM_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_map_scu_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM

// #pragma HLS DATA_PACK variable=in_buf.config.md_fw_ptr instance=md_fw_ptr
//#pragma HLS DATA_PACK variable=in_buf.config.md_input_ptr instance=md_input_ptr //ERROR: [XFORM 203-1104] Bitwidth of packed elements (16935952 bits) exceeds the maximum threshold (65536 bits)
// #pragma HLS DATA_PACK variable=in_buf.insn instance=insn

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

#if PU_8X16_AXIS_ADAPTER
    in_buf.config.md_fw_ptr.pic_width                  = axis_in.in_buf_config_md_fw_ptr_pic_width               ; 
	in_buf.config.md_fw_ptr.pic_height                 = axis_in.in_buf_config_md_fw_ptr_pic_height              ; 
	in_buf.config.md_fw_ptr.slice_type                 = axis_in.in_buf_config_md_fw_ptr_slice_type              ; 
	in_buf.config.md_fw_ptr.pic_width_in_scu           = axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu        ; 
	in_buf.config.md_fw_ptr.pic_height_in_scu          = axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu       ; 
	in_buf.config.md_fw_ptr.log2_culine                = axis_in.in_buf_config_md_fw_ptr_log2_culine             ; 
	in_buf.config.md_fw_ptr.bit_depth_internal         = axis_in.in_buf_config_md_fw_ptr_bit_depth_internal      ; 
	in_buf.config.md_fw_ptr.max_part_ratio             = axis_in.in_buf_config_md_fw_ptr_max_part_ratio          ; 
	in_buf.config.md_fw_ptr.min_cu_size                = axis_in.in_buf_config_md_fw_ptr_min_cu_size             ; 
	in_buf.config.md_fw_ptr.max_split_times            = axis_in.in_buf_config_md_fw_ptr_max_split_times         ; 
	in_buf.config.md_fw_ptr.min_qt_size                = axis_in.in_buf_config_md_fw_ptr_min_qt_size             ; 
	in_buf.config.md_fw_ptr.max_bt_size                = axis_in.in_buf_config_md_fw_ptr_max_bt_size             ; 
	in_buf.config.md_fw_ptr.max_eqt_size               = axis_in.in_buf_config_md_fw_ptr_max_eqt_size            ; 
	in_buf.config.md_fw_ptr.mipf_enable_flag           = axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag        ; 
	in_buf.config.md_fw_ptr.cur_ptr                    = axis_in.in_buf_config_md_fw_ptr_cur_ptr                 ; 
	in_buf.config.md_input_ptr.pix_x                   = axis_in.in_buf_config_md_input_ptr_pix_x                ; 
	in_buf.config.md_input_ptr.pix_y                   = axis_in.in_buf_config_md_input_ptr_pix_y                ; 
	in_buf.config.md_input_ptr.lambda_y                = axis_in.in_buf_config_md_input_ptr_lambda_y             ; 
	in_buf.config.md_input_ptr.qp_y                    = axis_in.in_buf_config_md_input_ptr_qp_y                 ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][1]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][1]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[1][0][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[1][1][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 ; 
	in_buf.config.md_input_ptr.cu_dqp_enable           = axis_in.in_buf_config_md_input_ptr_cu_dqp_enable        ; 
	in_buf.config.md_input_ptr.cu_qp_group_pred_qp     = axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp  ; 
	in_buf.config.min_scu_x                            = axis_in.in_buf_config_min_scu_x                         ; 
	in_buf.config.min_scu_y                            = axis_in.in_buf_config_min_scu_y                         ; 
	in_buf.buffer.skip_in.slice_type                   = axis_in.in_buf_buffer_skip_in_slice_type                ; 
	in_buf.buffer.skip_in.ptr                          = axis_in.in_buf_buffer_skip_in_ptr                       ; 
	in_buf.buffer.skip_in.umve_enable_flag             = axis_in.in_buf_buffer_skip_in_umve_enable_flag          ; 
	in_buf.buffer.Fetch_Ref_window_ptr.code_stru       = axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru    ; 
	in_buf.insn.x0                                     = axis_in.in_buf_insn_x0                                  ; 
	in_buf.insn.y0                                     = axis_in.in_buf_insn_y0                                  ; 
	in_buf.insn.cup                                    = axis_in.in_buf_insn_cup                                 ; 
	in_buf.insn.cu_width_log2                          = axis_in.in_buf_insn_cu_width_log2                       ; 
	in_buf.insn.cud                                    = axis_in.in_buf_insn_cud                                 ; 
	in_buf.insn.qt_depth                               = axis_in.in_buf_insn_qt_depth                            ; 
	in_buf.insn.bet_depth                              = axis_in.in_buf_insn_bet_depth                           ; 
	in_buf.insn.cons_pred_mode                         = axis_in.in_buf_insn_cons_pred_mode                      ; 
	in_buf.insn.cu_width                               = axis_in.in_buf_insn_cu_width                            ; 
	in_buf.insn.cu_height                              = axis_in.in_buf_insn_cu_height                           ; 
	in_buf.insn.tree_status                            = axis_in.in_buf_insn_tree_status                         ; 
	in_buf.insn.split_mode                             = axis_in.in_buf_insn_split_mode                          ; 
	in_buf.insn.tag                                    = axis_in.in_buf_insn_tag                                 ; 
	in_buf.line_buffer.y0                              = axis_in.in_buf_line_buffer_y0                           ; 
    in_buf.config.md_fw_ptr.refp_ptr[0][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[0][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[1][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[1][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[2][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[2][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[3][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[3][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[4][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[4][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[5][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[5][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[6][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[6][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[7][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[7][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[8][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[8][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[9][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[9][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[10][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[10][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[11][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[11][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[12][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[12][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[13][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[13][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[14][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[14][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[15][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[15][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[16][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[16][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][1]         ;           
    in_buf.config.md_fw_ptr.num_refp[0]                = axis_in.in_buf_config_md_fw_ptr_num_refp[0]             ;      
    in_buf.config.md_fw_ptr.num_refp[1]                = axis_in.in_buf_config_md_fw_ptr_num_refp[1]             ;             
#endif // PU_8X16_AXIS_ADAPTER

	STATIC_C IP_ARRAY_SIZE<16, 16> IP_buffer;
	STATIC_C MPM_ARRAY_SIZE<16, 16> MPM_buffer;
	STATIC_C ENC_ME_LINE_MAP_SIZE<16> me_line_map;
	STATIC_C ENC_ME_NEB_INFO up_nebs[6];
	STATIC_C ENC_ME_NEB_INFO left_nebs[4];
	STATIC_C IP_MAP_SCU_SIZE<16, 16> IP_map_scu;
	load_line_buffer(in_buf.insn.x0, in_buf.insn.y0, me_line_map, IP_buffer, MPM_buffer, up_nebs, left_nebs, IP_map_scu, in_buf.line_buffer, in_buf.config);

	s64 cost_temp;
	U2 cons_pred_mode;
	U2 tree_status_child;

	cost_temp = md_16x16_16x8_8x16_loop_md1(in_buf.insn.split_mode,
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, & in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		& me_line_map,
		up_nebs, left_nebs,
		& in_buf.buffer.skip_in,
		true, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		& out_buf.curr_cu_data_temp,
		& cons_pred_mode, in_buf.insn.tree_status, & tree_status_child, & in_buf.buffer.fme_mv_info,
		& IP_buffer,
		& MPM_buffer,
		& IP_map_scu,
		ref,
		& in_buf.buffer.Fetch_Ref_window_ptr);

	U8 y0 = in_buf.line_buffer.y0;
	copy_excepty0(out_buf.line_buffer, in_buf.line_buffer);
	update_line_buffer(in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cu_width, in_buf.insn.cu_height, y0, in_buf.line_buffer, out_buf.line_buffer, out_buf.curr_cu_data_temp);

	out_buf.split_mode = in_buf.insn.split_mode;
	out_buf.tag = in_buf.insn.tag;
	out_buf.cons_pred_mode = cons_pred_mode;
	out_buf.tree_status_child = tree_status_child;
#if KERNEL_SIZE_CLOSE(8, 16)
	out_buf.cost_temp = MAX_S64 - 1;
#else
	out_buf.cost_temp = cost_temp;
#endif // KERNEL_SIZE_CLOSE(8, 16)
#if PU_8X16_AXIS_ADAPTER
	ram_out.out_buf_line_buffer_y0[0]    = out_buf.line_buffer.y0    ; 
	ram_out.out_buf_cost_temp[0]         = out_buf.cost_temp         ; 
	ram_out.out_buf_cons_pred_mode[0]    = out_buf.cons_pred_mode    ; 
	ram_out.out_buf_tree_status_child[0] = out_buf.tree_status_child ; 
	ram_out.out_buf_split_mode[0]        = out_buf.split_mode        ; 
	ram_out.out_buf_tag[0]               = out_buf.tag               ;     
#endif // PU_8X16_AXIS_ADAPTER

}

#if PU_16X8_AXIS_ADAPTER
void pu_KERNEL_16X8(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_16x8& axis_in, PURamOutPorts_16x8& ram_out)
#else
void pu_KERNEL_16X8(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
#endif // PU_16X8_AXIS_ADAPTER
{

#if 1 // Align with PU_8x8
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_fw_ptr.refp_ptr 			dim=0 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=in_buf.config.md_fw_ptr.num_refp			dim=1 complete	// NEED AXIS
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1			dim=1 complete		// LOAD HLS problem
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1			dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_mv1			dim=3 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1		dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1		dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_map_refi1		dim=3 complete
#pragma HLS ARRAY_PARTITION variable=in_buf.buffer.skip_in.refp_list_ptr1		dim=1 complete
#endif

#if PU_16X8_AXIS_ADAPTER
#pragma HLS ARRAY_PARTITION variable=axis_in.in_buf_config_md_fw_ptr_refp_ptr			dim=0 complete	// NEED AXIS
#pragma HLS ARRAY_PARTITION variable=axis_in.in_buf_config_md_fw_ptr_num_refp			dim=1 complete	// NEED AXIS
#endif

	// stream port unused may cause core dump
#if pu_interface & 0
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr
#pragma HLS INTERFACE axis port=in_buf.insn
#pragma HLS INTERFACE axis port=in_buf.cost_best
#pragma HLS INTERFACE axis port=out_buf.cost_temp
#pragma HLS INTERFACE axis port=out_buf.cons_pred_mode
#pragma HLS INTERFACE axis port=out_buf.tree_status_child
#pragma HLS INTERFACE axis port=out_buf.split_mode
#pragma HLS INTERFACE axis port=out_buf.tag
#endif

#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1

#if PU_16X8_AXIS_ADAPTER
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width                
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height               
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_slice_type               
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu         
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu        
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_log2_culine              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_bit_depth_internal       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_part_ratio           
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_cu_size              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_split_times          
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_qt_size              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_bt_size              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_eqt_size             
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag         
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_cur_ptr                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_pix_x                 
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_pix_y                 
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_lambda_y              
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_qp_y                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0  
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_cu_dqp_enable       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp     
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_min_scu_x                          
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_min_scu_y                          
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_slice_type                 
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_ptr                        
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_umve_enable_flag           
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru     
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_x0                                   
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_y0                                   
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cup                                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width_log2                        
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cud                                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_qt_depth                             
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_bet_depth                            
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cons_pred_mode                       
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width                             
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_height                            
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tree_status                          
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_split_mode                           
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tag                                  
#pragma HLS INTERFACE axis port=axis_in.in_buf_line_buffer_y0       
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][1]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp[0]
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp[1]                       
#pragma HLS RESOURCE variable=ram_out.out_buf_line_buffer_y0 core=RAM_1P_BRAM    
#pragma HLS RESOURCE variable=ram_out.out_buf_cost_temp core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_cons_pred_mode core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_tree_status_child core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_split_mode core=RAM_1P_BRAM  
#pragma HLS RESOURCE variable=ram_out.out_buf_tag core=RAM_1P_BRAM                                                              
#endif // PU_16X8_AXIS_ADAPTER

#pragma HLS RESOURCE variable=in_buf.buffer core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.me_line_map_16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.up_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.left_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.MPM_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_map_scu_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM

// #pragma HLS DATA_PACK variable=in_buf.config.md_fw_ptr instance=md_fw_ptr
//#pragma HLS DATA_PACK variable=in_buf.config.md_input_ptr instance=md_input_ptr //ERROR: [XFORM 203-1104] Bitwidth of packed elements (16935952 bits) exceeds the maximum threshold (65536 bits)
// #pragma HLS DATA_PACK variable=in_buf.insn instance=insn

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

#if PU_16X8_AXIS_ADAPTER
    in_buf.config.md_fw_ptr.pic_width                  = axis_in.in_buf_config_md_fw_ptr_pic_width               ; 
	in_buf.config.md_fw_ptr.pic_height                 = axis_in.in_buf_config_md_fw_ptr_pic_height              ; 
	in_buf.config.md_fw_ptr.slice_type                 = axis_in.in_buf_config_md_fw_ptr_slice_type              ; 
	in_buf.config.md_fw_ptr.pic_width_in_scu           = axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu        ; 
	in_buf.config.md_fw_ptr.pic_height_in_scu          = axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu       ; 
	in_buf.config.md_fw_ptr.log2_culine                = axis_in.in_buf_config_md_fw_ptr_log2_culine             ; 
	in_buf.config.md_fw_ptr.bit_depth_internal         = axis_in.in_buf_config_md_fw_ptr_bit_depth_internal      ; 
	in_buf.config.md_fw_ptr.max_part_ratio             = axis_in.in_buf_config_md_fw_ptr_max_part_ratio          ; 
	in_buf.config.md_fw_ptr.min_cu_size                = axis_in.in_buf_config_md_fw_ptr_min_cu_size             ; 
	in_buf.config.md_fw_ptr.max_split_times            = axis_in.in_buf_config_md_fw_ptr_max_split_times         ; 
	in_buf.config.md_fw_ptr.min_qt_size                = axis_in.in_buf_config_md_fw_ptr_min_qt_size             ; 
	in_buf.config.md_fw_ptr.max_bt_size                = axis_in.in_buf_config_md_fw_ptr_max_bt_size             ; 
	in_buf.config.md_fw_ptr.max_eqt_size               = axis_in.in_buf_config_md_fw_ptr_max_eqt_size            ; 
	in_buf.config.md_fw_ptr.mipf_enable_flag           = axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag        ; 
	in_buf.config.md_fw_ptr.cur_ptr                    = axis_in.in_buf_config_md_fw_ptr_cur_ptr                 ; 
	in_buf.config.md_input_ptr.pix_x                   = axis_in.in_buf_config_md_input_ptr_pix_x                ; 
	in_buf.config.md_input_ptr.pix_y                   = axis_in.in_buf_config_md_input_ptr_pix_y                ; 
	in_buf.config.md_input_ptr.lambda_y                = axis_in.in_buf_config_md_input_ptr_lambda_y             ; 
	in_buf.config.md_input_ptr.qp_y                    = axis_in.in_buf_config_md_input_ptr_qp_y                 ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][1]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][1]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[1][0][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD[1][1][0]      = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0      ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][1] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 ; 
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][0] = axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 ; 
	in_buf.config.md_input_ptr.cu_dqp_enable           = axis_in.in_buf_config_md_input_ptr_cu_dqp_enable        ; 
	in_buf.config.md_input_ptr.cu_qp_group_pred_qp     = axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp  ; 
	in_buf.config.min_scu_x                            = axis_in.in_buf_config_min_scu_x                         ; 
	in_buf.config.min_scu_y                            = axis_in.in_buf_config_min_scu_y                         ; 
	in_buf.buffer.skip_in.slice_type                   = axis_in.in_buf_buffer_skip_in_slice_type                ; 
	in_buf.buffer.skip_in.ptr                          = axis_in.in_buf_buffer_skip_in_ptr                       ; 
	in_buf.buffer.skip_in.umve_enable_flag             = axis_in.in_buf_buffer_skip_in_umve_enable_flag          ; 
	in_buf.buffer.Fetch_Ref_window_ptr.code_stru       = axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru    ; 
	in_buf.insn.x0                                     = axis_in.in_buf_insn_x0                                  ; 
	in_buf.insn.y0                                     = axis_in.in_buf_insn_y0                                  ; 
	in_buf.insn.cup                                    = axis_in.in_buf_insn_cup                                 ; 
	in_buf.insn.cu_width_log2                          = axis_in.in_buf_insn_cu_width_log2                       ; 
	in_buf.insn.cud                                    = axis_in.in_buf_insn_cud                                 ; 
	in_buf.insn.qt_depth                               = axis_in.in_buf_insn_qt_depth                            ; 
	in_buf.insn.bet_depth                              = axis_in.in_buf_insn_bet_depth                           ; 
	in_buf.insn.cons_pred_mode                         = axis_in.in_buf_insn_cons_pred_mode                      ; 
	in_buf.insn.cu_width                               = axis_in.in_buf_insn_cu_width                            ; 
	in_buf.insn.cu_height                              = axis_in.in_buf_insn_cu_height                           ; 
	in_buf.insn.tree_status                            = axis_in.in_buf_insn_tree_status                         ; 
	in_buf.insn.split_mode                             = axis_in.in_buf_insn_split_mode                          ; 
	in_buf.insn.tag                                    = axis_in.in_buf_insn_tag                                 ; 
	in_buf.line_buffer.y0                              = axis_in.in_buf_line_buffer_y0                           ; 
	in_buf.config.md_fw_ptr.refp_ptr[0][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[0][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[0][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[1][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[1][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[1][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[2][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[2][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[2][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[3][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[3][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[3][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[4][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[4][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[4][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[5][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[5][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[5][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[6][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[6][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[6][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[7][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[7][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[7][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[8][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[8][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[8][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[9][0]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][0]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[9][1]             = axis_in.in_buf_config_md_fw_ptr_refp_ptr[9][1]          ;           
    in_buf.config.md_fw_ptr.refp_ptr[10][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[10][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[10][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[11][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[11][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[11][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[12][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[12][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[12][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[13][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[13][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[13][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[14][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[14][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[14][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[15][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[15][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[15][1]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[16][0]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][0]         ;           
    in_buf.config.md_fw_ptr.refp_ptr[16][1]            = axis_in.in_buf_config_md_fw_ptr_refp_ptr[16][1]         ;           
    in_buf.config.md_fw_ptr.num_refp[0]                = axis_in.in_buf_config_md_fw_ptr_num_refp[0]             ;      
    in_buf.config.md_fw_ptr.num_refp[1]                = axis_in.in_buf_config_md_fw_ptr_num_refp[1]             ;   
#endif // PU_16X8_AXIS_ADAPTER    

	STATIC_C IP_ARRAY_SIZE<16, 16> IP_buffer;
	STATIC_C MPM_ARRAY_SIZE<16, 16> MPM_buffer;
	STATIC_C ENC_ME_LINE_MAP_SIZE<16> me_line_map;
	STATIC_C ENC_ME_NEB_INFO up_nebs[6];
	STATIC_C ENC_ME_NEB_INFO left_nebs[4];
	STATIC_C IP_MAP_SCU_SIZE<16, 16> IP_map_scu;
	load_line_buffer(in_buf.insn.x0, in_buf.insn.y0, me_line_map, IP_buffer, MPM_buffer, up_nebs, left_nebs, IP_map_scu, in_buf.line_buffer, in_buf.config);

	out_buf.cost_temp = md_16x16_16x8_8x16_loop_md1(in_buf.insn.split_mode,
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output, in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, &in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		& me_line_map,
		up_nebs, left_nebs,
		&in_buf.buffer.skip_in,
		true, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		&out_buf.curr_cu_data_temp,
		&out_buf.cons_pred_mode, in_buf.insn.tree_status, &out_buf.tree_status_child, &in_buf.buffer.fme_mv_info,
		& IP_buffer,
		& MPM_buffer,
		& IP_map_scu,
		ref,
		&in_buf.buffer.Fetch_Ref_window_ptr);

	U8 y0 = in_buf.line_buffer.y0;
	copy_excepty0(out_buf.line_buffer, in_buf.line_buffer);
	update_line_buffer(in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cu_width, in_buf.insn.cu_height, y0, in_buf.line_buffer, out_buf.line_buffer, out_buf.curr_cu_data_temp);
	out_buf.split_mode = in_buf.insn.split_mode;
	out_buf.tag = in_buf.insn.tag;

#if KERNEL_SIZE_CLOSE(16, 8)
	out_buf.cost_temp = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(16, 8)
#if PU_16X8_AXIS_ADAPTER
	ram_out.out_buf_line_buffer_y0[0]    = out_buf.line_buffer.y0    ; 
	ram_out.out_buf_cost_temp[0]         = out_buf.cost_temp         ; 
	ram_out.out_buf_cons_pred_mode[0]    = out_buf.cons_pred_mode    ; 
	ram_out.out_buf_tree_status_child[0] = out_buf.tree_status_child ; 
	ram_out.out_buf_split_mode[0]        = out_buf.split_mode        ; 
	ram_out.out_buf_tag[0]               = out_buf.tag               ; 
#endif // PU_16X8_AXIS_ADAPTER  

}
void pu_KERNEL_8X8(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, const PUAxisInPorts_8x8& axis_in, PURamOut_8x8& ram_out)
{
//axis in
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_slice_type
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_bit_depth_internal
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_part_ratio
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_cu_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_split_times
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_qt_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_bt_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_eqt_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_1_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_2_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_2_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_3_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_3_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_4_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_4_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_5_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_5_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_6_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_6_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_7_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_7_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_8_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_8_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_9_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_9_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_10_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_10_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_11_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_11_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_12_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_12_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_13_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_13_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_14_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_14_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_15_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_15_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_16_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_refp_ptr_16_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_num_refp_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_cur_ptr
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_pix_x
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_pix_y
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_lambda_y
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_qp_y
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_cu_dqp_enable
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_min_scu_x
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_min_scu_y
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_slice_type
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_ptr
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_skip_in_umve_enable_flag
#pragma HLS INTERFACE axis port=axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_x0
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_y0
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width_log2
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cud
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_qt_depth
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_bet_depth
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_height
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tree_status
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_split_mode
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tag
#pragma HLS INTERFACE axis port=axis_in.in_buf_line_buffer_y0

	// stream port unused may cause core dump
#pragma HLS INTERFACE m_axi port=ref->ref_0_0 offset=off name=ref_0_0
#pragma HLS INTERFACE m_axi port=ref->ref_0_1 offset=off name=ref_0_1
#pragma HLS INTERFACE m_axi port=ref->ref_1_0 offset=off name=ref_1_0
#pragma HLS INTERFACE m_axi port=ref->ref_1_1 offset=off name=ref_1_1
#pragma HLS INTERFACE m_axi port=ref->ref_2_0 offset=off name=ref_2_0
#pragma HLS INTERFACE m_axi port=ref->ref_2_1 offset=off name=ref_2_1
#pragma HLS INTERFACE m_axi port=ref->ref_3_0 offset=off name=ref_3_0
#pragma HLS INTERFACE m_axi port=ref->ref_3_1 offset=off name=ref_3_1
#pragma HLS INTERFACE m_axi port=ref->ref_4_0 offset=off name=ref_4_0
#pragma HLS INTERFACE m_axi port=ref->ref_4_1 offset=off name=ref_4_1
#pragma HLS INTERFACE m_axi port=ref->ref_5_0 offset=off name=ref_5_0
#pragma HLS INTERFACE m_axi port=ref->ref_5_1 offset=off name=ref_5_1
#pragma HLS INTERFACE m_axi port=ref->ref_6_0 offset=off name=ref_6_0
#pragma HLS INTERFACE m_axi port=ref->ref_6_1 offset=off name=ref_6_1
#pragma HLS INTERFACE m_axi port=ref->ref_7_0 offset=off name=ref_7_0
#pragma HLS INTERFACE m_axi port=ref->ref_7_1 offset=off name=ref_7_1

// ram_out
#pragma HLS resource variable=ram_out.out_buf_line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS resource variable=ram_out.out_buf_cost_temp core=RAM_1P_BRAM
#pragma HLS resource variable=ram_out.out_buf_tree_status_child core=RAM_1P_BRAM
#pragma HLS resource variable=ram_out.out_buf_split_mode core=RAM_1P_BRAM
#pragma HLS resource variable=ram_out.out_buf_tag core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=in_buf.buffer core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.me_line_map_16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.up_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.left_nebs core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.MPM_buffer_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_map_scu_16x16 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM

//#pragma HLS DATA_PACK variable=in_buf.config.md_fw_ptr instance=md_fw_ptr
//#pragma HLS DATA_PACK variable=in_buf.config.md_input_ptr instance=md_input_ptr //ERROR: [XFORM 203-1104] Bitwidth of packed elements (16935952 bits) exceeds the maximum threshold (65536 bits)
//#pragma HLS DATA_PACK variable=in_buf.insn instance=insn

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

// axis seperate in
	in_buf.config.md_fw_ptr.pic_width=axis_in.in_buf_config_md_fw_ptr_pic_width;
	in_buf.config.md_fw_ptr.pic_height=axis_in.in_buf_config_md_fw_ptr_pic_height;
	in_buf.config.md_fw_ptr.slice_type=axis_in.in_buf_config_md_fw_ptr_slice_type;
	in_buf.config.md_fw_ptr.pic_width_in_scu=axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu;
	in_buf.config.md_fw_ptr.pic_height_in_scu=axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu;
	in_buf.config.md_fw_ptr.bit_depth_internal=axis_in.in_buf_config_md_fw_ptr_bit_depth_internal;
	in_buf.config.md_fw_ptr.max_part_ratio=axis_in.in_buf_config_md_fw_ptr_max_part_ratio;
	in_buf.config.md_fw_ptr.min_cu_size=axis_in.in_buf_config_md_fw_ptr_min_cu_size;
	in_buf.config.md_fw_ptr.max_split_times=axis_in.in_buf_config_md_fw_ptr_max_split_times;
	in_buf.config.md_fw_ptr.min_qt_size=axis_in.in_buf_config_md_fw_ptr_min_qt_size;
	in_buf.config.md_fw_ptr.max_bt_size=axis_in.in_buf_config_md_fw_ptr_max_bt_size;
	in_buf.config.md_fw_ptr.max_eqt_size=axis_in.in_buf_config_md_fw_ptr_max_eqt_size;
	in_buf.config.md_fw_ptr.mipf_enable_flag=axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag;
	in_buf.config.md_fw_ptr.refp_ptr[0][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_0_0;
	in_buf.config.md_fw_ptr.refp_ptr[0][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_0_1;
	in_buf.config.md_fw_ptr.refp_ptr[1][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_1_0;
	in_buf.config.md_fw_ptr.refp_ptr[1][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_1_1;
	in_buf.config.md_fw_ptr.refp_ptr[2][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_2_0;
	in_buf.config.md_fw_ptr.refp_ptr[2][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_2_1;
	in_buf.config.md_fw_ptr.refp_ptr[3][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_3_0;
	in_buf.config.md_fw_ptr.refp_ptr[3][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_3_1;
	in_buf.config.md_fw_ptr.refp_ptr[4][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_4_0;
	in_buf.config.md_fw_ptr.refp_ptr[4][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_4_1;
	in_buf.config.md_fw_ptr.refp_ptr[5][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_5_0;
	in_buf.config.md_fw_ptr.refp_ptr[5][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_5_1;
	in_buf.config.md_fw_ptr.refp_ptr[6][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_6_0;
	in_buf.config.md_fw_ptr.refp_ptr[6][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_6_1;
	in_buf.config.md_fw_ptr.refp_ptr[7][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_7_0;
	in_buf.config.md_fw_ptr.refp_ptr[7][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_7_1;
	in_buf.config.md_fw_ptr.refp_ptr[8][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_8_0;
	in_buf.config.md_fw_ptr.refp_ptr[8][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_8_1;
	in_buf.config.md_fw_ptr.refp_ptr[9][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_9_0;
	in_buf.config.md_fw_ptr.refp_ptr[9][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_9_1;
	in_buf.config.md_fw_ptr.refp_ptr[10][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_10_0;
	in_buf.config.md_fw_ptr.refp_ptr[10][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_10_1;
	in_buf.config.md_fw_ptr.refp_ptr[11][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_11_0;
	in_buf.config.md_fw_ptr.refp_ptr[11][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_11_1;
	in_buf.config.md_fw_ptr.refp_ptr[12][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_12_0;
	in_buf.config.md_fw_ptr.refp_ptr[12][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_12_1;
	in_buf.config.md_fw_ptr.refp_ptr[13][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_13_0;
	in_buf.config.md_fw_ptr.refp_ptr[13][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_13_1;
	in_buf.config.md_fw_ptr.refp_ptr[14][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_14_0;
	in_buf.config.md_fw_ptr.refp_ptr[14][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_14_1;
	in_buf.config.md_fw_ptr.refp_ptr[15][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_15_0;
	in_buf.config.md_fw_ptr.refp_ptr[15][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_15_1;
	in_buf.config.md_fw_ptr.refp_ptr[16][0]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_16_0;
	in_buf.config.md_fw_ptr.refp_ptr[16][1]=axis_in.in_buf_config_md_fw_ptr_refp_ptr_16_1;
	in_buf.config.md_fw_ptr.num_refp[0]=axis_in.in_buf_config_md_fw_ptr_num_refp_0;
	in_buf.config.md_fw_ptr.num_refp[1]=axis_in.in_buf_config_md_fw_ptr_num_refp_1;
	in_buf.config.md_fw_ptr.cur_ptr=axis_in.in_buf_config_md_fw_ptr_cur_ptr;
	in_buf.config.md_input_ptr.pix_x=axis_in.in_buf_config_md_input_ptr_pix_x;
	in_buf.config.md_input_ptr.pix_y=axis_in.in_buf_config_md_input_ptr_pix_y;
	in_buf.config.md_input_ptr.lambda_y=axis_in.in_buf_config_md_input_ptr_lambda_y;
	in_buf.config.md_input_ptr.qp_y=axis_in.in_buf_config_md_input_ptr_qp_y;
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD[0][0][1]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1;
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD[0][1][1]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1;
	in_buf.config.md_input_ptr.CtrPos2MD[1][0][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD[1][1][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][1]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][1]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0;
	in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][0]=axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0;
	in_buf.config.md_input_ptr.cu_dqp_enable=axis_in.in_buf_config_md_input_ptr_cu_dqp_enable;
	in_buf.config.md_input_ptr.cu_qp_group_pred_qp=axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp;
	in_buf.config.min_scu_x=axis_in.in_buf_config_min_scu_x;
	in_buf.config.min_scu_y=axis_in.in_buf_config_min_scu_y;
	in_buf.buffer.skip_in.slice_type=axis_in.in_buf_buffer_skip_in_slice_type;
	in_buf.buffer.skip_in.ptr=axis_in.in_buf_buffer_skip_in_ptr;
	in_buf.buffer.skip_in.umve_enable_flag=axis_in.in_buf_buffer_skip_in_umve_enable_flag;
	in_buf.buffer.Fetch_Ref_window_ptr.code_stru=axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru;
	in_buf.insn.x0=axis_in.in_buf_insn_x0;
	in_buf.insn.y0=axis_in.in_buf_insn_y0;
	in_buf.insn.cu_width_log2=axis_in.in_buf_insn_cu_width_log2;
	in_buf.insn.cud=axis_in.in_buf_insn_cud;
	in_buf.insn.qt_depth=axis_in.in_buf_insn_qt_depth;
	in_buf.insn.bet_depth=axis_in.in_buf_insn_bet_depth;
	in_buf.insn.cu_width=axis_in.in_buf_insn_cu_width;
	in_buf.insn.cu_height=axis_in.in_buf_insn_cu_height;
	in_buf.insn.tree_status=axis_in.in_buf_insn_tree_status;
	in_buf.insn.split_mode=axis_in.in_buf_insn_split_mode;
	in_buf.insn.tag=axis_in.in_buf_insn_tag;
	in_buf.line_buffer.y0=axis_in.in_buf_line_buffer_y0;
	//out_buf.cons_pred_mode=axis_in.out_buf_cons_pred_mode;

	STATIC_C IP_ARRAY_SIZE<16, 16> IP_buffer;
	STATIC_C MPM_ARRAY_SIZE<16, 16> MPM_buffer;
	STATIC_C ENC_ME_LINE_MAP_SIZE<16> me_line_map;
	STATIC_C ENC_ME_NEB_INFO up_nebs[6];
	STATIC_C ENC_ME_NEB_INFO left_nebs[4];
	STATIC_C IP_MAP_SCU_SIZE<16, 16> IP_map_scu;
	U2 cons_pred_mode = NO_MODE_CONS;

	load_line_buffer(in_buf.insn.x0, in_buf.insn.y0, me_line_map, IP_buffer, MPM_buffer, up_nebs, left_nebs, IP_map_scu, in_buf.line_buffer, in_buf.config);

	out_buf.cost_temp = md_8x8_loop(true,
		&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, &in_buf.buffer.g_rmd_output,
		in_buf.buffer.RefWindowLCU_UP, in_buf.buffer.RefWindowLCU_VP, in_buf.buffer.CtrPos,
#if ENABLE_BFRAME
		in_buf.buffer.RefWindowLCU_UP_ref1, in_buf.buffer.RefWindowLCU_VP_ref1, in_buf.buffer.CtrPos_ref1, & in_buf.buffer.me_mv_dir,
#endif
		in_buf.buffer.p_fenc_LCU_Y,
		& me_line_map,
		up_nebs, left_nebs,
		& in_buf.buffer.skip_in,
		in_buf.insn.split_mode, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.cu_width, in_buf.insn.cu_height, ONLY_INTRA,
		& out_buf.curr_cu_data_temp,
		& cons_pred_mode, in_buf.insn.tree_status, & out_buf.tree_status_child,
		& in_buf.buffer.fme_mv_info,
		& IP_buffer,
		& MPM_buffer,
		& IP_map_scu,
		ref,
		& in_buf.buffer.Fetch_Ref_window_ptr);

	U8 y0 = in_buf.line_buffer.y0;
	copy_excepty0(out_buf.line_buffer, in_buf.line_buffer);
	update_line_buffer(in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cu_width, in_buf.insn.cu_height, y0, in_buf.line_buffer, out_buf.line_buffer, out_buf.curr_cu_data_temp);
	out_buf.split_mode = in_buf.insn.split_mode;
	out_buf.tag = in_buf.insn.tag;

#if KERNEL_SIZE_CLOSE(8, 8)
	out_buf.cost_temp = MAX_S64 - 1;
#endif // KERNEL_SIZE_CLOSE(8, 8)

// axis seperate out
	//axis_out.in_buf_config_md_fw_ptr_bit_depth_internal=in_buf.config.md_fw_ptr.bit_depth_internal;
	//axis_out.in_buf_config_md_fw_ptr_refp_ptr_0_0=in_buf.config.md_fw_ptr.refp_ptr[0][0];
	//axis_out.in_buf_config_md_fw_ptr_refp_ptr_0_1=in_buf.config.md_fw_ptr.refp_ptr[0][1];
	//axis_out.in_buf_config_md_fw_ptr_num_refp_0=in_buf.config.md_fw_ptr.num_refp[0];
	//axis_out.in_buf_config_md_fw_ptr_num_refp_1=in_buf.config.md_fw_ptr.num_refp[1];
	//axis_out.in_buf_config_md_input_ptr_qp_y=in_buf.config.md_input_ptr.qp_y;
	//axis_out.in_buf_config_md_input_ptr_cu_qp_group_pred_qp=in_buf.config.md_input_ptr.cu_qp_group_pred_qp;
	ram_out.out_buf_line_buffer_y0[0] = out_buf.line_buffer.y0;
	ram_out.out_buf_cost_temp[0] = out_buf.cost_temp;
	ram_out.out_buf_tree_status_child[0] = out_buf.tree_status_child;
	ram_out.out_buf_split_mode[0] = out_buf.split_mode;
	ram_out.out_buf_tag[0] = out_buf.tag;
}

#if !ONLY_QT_QT
void pu_16x16_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	STATIC_C IP_ARRAY_SIZE<8, 16> null_IP_buffer_ptr_8x16;
	STATIC_C IP_ARRAY_SIZE<16, 8> null_IP_buffer_ptr_16x8;
	STATIC_C MPM_ARRAY_SIZE<8, 16> null_MPM_buffer_ptr_8x16;
	STATIC_C MPM_ARRAY_SIZE<16, 8> null_MPM_buffer_ptr_16x8;
	STATIC_C IP_MAP_SCU_SIZE<8, 16> null_IP_map_scu_ptr_8x16;
	STATIC_C IP_MAP_SCU_SIZE<16, 8> null_IP_map_scu_ptr_16x8;
	U2 NS_luma_pred_mode = ONLY_INTRA;

	pu_test_0(out_buf.cost_temp, NO_SPLIT,
			&in_buf.config.md_fw_ptr, &in_buf.config.md_input_ptr, in_buf.ip_list,
#if ENABLE_BFRAME
			in_buf.me_mv_dir_16,
#endif
			in_buf.org_y_16,
			in_buf.me_line_map_16, in_buf.up_nebs,
			in_buf.left_nebs,
			in_buf.skip_in_16,
			in_buf.boundary, 1, NS_luma_pred_mode, in_buf.insn.x0, in_buf.insn.y0,
			in_buf.insn.cup, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2,
			in_buf.insn.cud, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cons_pred_mode, in_buf.insn.tree_status,
			out_buf.curr_cu_data_temp,
			in_buf.fme_mv_info_16,
			in_buf.IP_buffer_16x16, null_IP_buffer_ptr_8x16, null_IP_buffer_ptr_16x8,
			in_buf.MPM_buffer_16x16, null_MPM_buffer_ptr_8x16, null_MPM_buffer_ptr_16x8,
			in_buf.IP_map_scu_16x16, null_IP_map_scu_ptr_8x16, null_IP_map_scu_ptr_16x8,
			ref,
			in_buf.Fetch_Ref_window_16
#if SUB_TMVP
			, &in_buf.buffer.skip_in
#endif
			); //split_struct.split_allow[part_num][NO_SPLIT]
	out_buf.split_mode_father = in_buf.split_mode_father;
}

void pu_16x16_8x16_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	STATIC_C COM_SPLIT_STRUCT split_struct_null;
	pu_1(in_buf, out_buf, ref, split_struct_null, in_buf.part_num, in_buf.up_nebs, in_buf.left_nebs, SPLIT_BI_VER);
	out_buf.split_mode_father = in_buf.split_mode_father;
}

void pu_16x16_16x8_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref)
{
	STATIC_C COM_SPLIT_STRUCT split_struct_null;
	pu_1(in_buf, out_buf, ref, split_struct_null, in_buf.part_num, in_buf.up_nebs, in_buf.left_nebs, SPLIT_BI_HOR);
	out_buf.split_mode_father = in_buf.split_mode_father;
}
#endif

s64 split_est(MD_FW* md_fw_ptr, U3 split_mode, U3 cu_width_log2, U3 cu_height_log2, U7 cu_width, U7 cu_height, U3 qt_depth, U3 bet_depth, U13 x, U13 y)
{
	U13 pic_width = md_fw_ptr->pic_width;
	U13 pic_height = md_fw_ptr->pic_height;
	U3 enc_est = 0;
	U1 split_allow[6];
	U3 non_QT_split_mode_num;
	U1 boundary = 0;
	U1 boundary_b = 0;
	U1 boundary_r = 0;
	if (cu_width == 1 << 2 && cu_height == 1 << 2)
	{
		return enc_est;
	}

	boundary = !(x + cu_width <= pic_width && y + cu_height <= pic_height);
	boundary_b = boundary && y + cu_height > pic_height && !(x + cu_width > pic_width);
	boundary_r = boundary && x + cu_width > pic_width && !(y + cu_height > pic_height);
	com_check_split_mode_ECO(md_fw_ptr, split_allow, cu_width_log2, cu_height_log2,
		boundary, boundary_b, boundary_r, qt_depth, bet_depth);
	non_QT_split_mode_num = 0;
	non_QT_split_mode_num = split_allow[1] + split_allow[2] + split_allow[3] + split_allow[4];

	//only QT is allowed
	if (split_allow[SPLIT_QUAD] && !(non_QT_split_mode_num || split_allow[NO_SPLIT]))
	{
		return enc_est;
	}
	else if (split_allow[SPLIT_QUAD])
	{
		enc_est++;
		if (split_mode == SPLIT_QUAD)
		{
			return enc_est;
		}
	}
	if (non_QT_split_mode_num)
	{
		//split flag
		if (split_allow[NO_SPLIT])
		{
			enc_est++;
		}

		if (split_mode != NO_SPLIT)
		{
			U1 HBT = split_allow[SPLIT_BI_HOR];
			int VBT = split_allow[SPLIT_BI_VER];
			int EnableBT = HBT || VBT;
			int HEQT = split_allow[SPLIT_EQT_HOR];
			int VEQT = split_allow[SPLIT_EQT_VER];
			int EnableEQT = HEQT || VEQT;

			u8 split_typ = (split_mode == SPLIT_EQT_HOR || split_mode == SPLIT_EQT_VER);
			if (EnableEQT && EnableBT)
			{
				enc_est++;
			}
			if ((split_typ) == 0)
			{
				if (HBT && VBT)
				{
					enc_est++;
				}
			}
			if ((split_typ) == 1)
			{
				if (HEQT && VEQT)
				{
					enc_est++;
				}
			}
		}
	}
	return enc_est;
}

#if BYPASS_AXIS_ADAPTER
void bypass(Load2Bp& in_buf, PU2Store& out_buf, Load2Bp_AXIS& axis_in, PU2Store_RAM& ram_out)
#else
void bypass(Load2Bp& in_buf, PU2Store& out_buf)
#endif // BYPASS_AXIS_ADAPTER
{
#pragma HLS inline
#if pu_interface
#if BYPASS_AXIS_ADAPTER
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_x0
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_y0
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width_log2
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_height_log2
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_qt_depth
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_bet_depth
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_width
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_cu_height
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_split_mode
#pragma HLS INTERFACE axis port=axis_in.in_buf_insn_tag
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_width
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_pic_height
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_part_ratio
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_cu_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_split_times
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_min_qt_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_bt_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_fw_ptr_max_eqt_size
#pragma HLS INTERFACE axis port=axis_in.in_buf_config_md_input_ptr_lambda_y
#pragma HLS INTERFACE axis port=axis_in.in_buf_line_buffer_y0

#pragma HLS RESOURCE variable=ram_out.out_buf_line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_cost_temp      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_split_mode     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_out.out_buf_tag            core=RAM_1P_BRAM
#else
#pragma HLS INTERFACE axis port=in_buf.insn.x0
#pragma HLS INTERFACE axis port=in_buf.insn.y0
#pragma HLS INTERFACE axis port=in_buf.insn.cu_width_log2
#pragma HLS INTERFACE axis port=in_buf.insn.cu_height_log2
#pragma HLS INTERFACE axis port=in_buf.insn.qt_depth
#pragma HLS INTERFACE axis port=in_buf.insn.bet_depth
#pragma HLS INTERFACE axis port=in_buf.insn.cu_width
#pragma HLS INTERFACE axis port=in_buf.insn.cu_height
#pragma HLS INTERFACE axis port=in_buf.insn.split_mode
#pragma HLS INTERFACE axis port=in_buf.insn.tag
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=in_buf.config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=in_buf.config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=in_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=out_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=out_buf.cost_temp
#pragma HLS INTERFACE axis port=out_buf.split_mode
#pragma HLS INTERFACE axis port=out_buf.tag

#endif // BYPASS_AXIS_ADAPTER
#endif // pu_interface

#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.up[0].mv cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.up[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.up[0].refi cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.up[0].rec cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.left[0].mv cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.left[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.left[0].refi cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.left[0].rec cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.topleft[0].mv cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.topleft[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.topleft[0].refi cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=out_buf.linebuffer.topleft[0].rec cyclic factor=4 dim=1

#if BYPASS_AXIS_ADAPTER
	Load2Bp_AXIS bypass_local;
	in_buf.insn.x0	=	axis_in.in_buf_insn_x0;
	in_buf.insn.y0	= axis_in.in_buf_insn_y0;
	in_buf.insn.cu_width_log2	= axis_in.in_buf_insn_cu_width_log2;
	in_buf.insn.cu_height_log2	= axis_in.in_buf_insn_cu_height_log2;
	in_buf.insn.qt_depth	= axis_in.in_buf_insn_qt_depth;
	in_buf.insn.bet_depth	= axis_in.in_buf_insn_bet_depth;
	in_buf.insn.cu_width	= axis_in.in_buf_insn_cu_width;
	in_buf.insn.cu_height	= axis_in.in_buf_insn_cu_height;
	in_buf.insn.split_mode	= axis_in.in_buf_insn_split_mode;
	// in_buf.insn.tag	=	axis_in.in_buf_insn_tag;
	bypass_local.in_buf_insn_tag	= axis_in.in_buf_insn_tag;		// AXIS read in, and latch localy.
	in_buf.insn.tag         = bypass_local.in_buf_insn_tag;
	in_buf.config.md_fw_ptr.pic_width	= axis_in.in_buf_config_md_fw_ptr_pic_width;
	in_buf.config.md_fw_ptr.pic_height	= axis_in.in_buf_config_md_fw_ptr_pic_height;
	in_buf.config.md_fw_ptr.max_part_ratio	= axis_in.in_buf_config_md_fw_ptr_max_part_ratio;
	in_buf.config.md_fw_ptr.min_cu_size	= axis_in.in_buf_config_md_fw_ptr_min_cu_size;
	in_buf.config.md_fw_ptr.max_split_times	= axis_in.in_buf_config_md_fw_ptr_max_split_times;
	in_buf.config.md_fw_ptr.min_qt_size	= axis_in.in_buf_config_md_fw_ptr_min_qt_size;
	in_buf.config.md_fw_ptr.max_bt_size	= axis_in.in_buf_config_md_fw_ptr_max_bt_size;
	in_buf.config.md_fw_ptr.max_eqt_size	= axis_in.in_buf_config_md_fw_ptr_max_eqt_size;
	in_buf.config.md_input_ptr.lambda_y	= axis_in.in_buf_config_md_input_ptr_lambda_y;
	in_buf.line_buffer.y0	= axis_in.in_buf_line_buffer_y0;
#endif // BYPASS_AXIS_ADAPTER

	if (in_buf.version == 2)
	{
		s64 cost = in_buf.cu_data_ptr.get_cost(in_buf.insn.x0 & 63, in_buf.insn.y0 & 63, in_buf.insn.cu_width, in_buf.insn.cu_height);

		s64 rate_bit = 0;
		if (in_buf.insn.x0 + in_buf.insn.cu_width <= in_buf.config.md_fw_ptr.pic_width && in_buf.insn.y0 + in_buf.insn.cu_height <= in_buf.config.md_fw_ptr.pic_height)
		{
			rate_bit += split_est(&in_buf.config.md_fw_ptr, in_buf.insn.split_mode, in_buf.insn.cu_width_log2, in_buf.insn.cu_height_log2, in_buf.insn.cu_width, in_buf.insn.cu_height, in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.x0, in_buf.insn.y0);
			cost += (rate_bit * in_buf.config.md_input_ptr.lambda_y + (1 << (LAMBDA_SCALE - 1))) >> LAMBDA_SCALE;
		}

		copy(out_buf.cu_data_ptr, in_buf.cu_data_ptr);
		copy(out_buf.line_buffer, in_buf.line_buffer);
		out_buf.cost_temp = cost;
		out_buf.split_mode = in_buf.insn.split_mode;
#if BYPASS_AXIS_ADAPTER
		ram_out.out_buf_line_buffer_y0[0]	=	out_buf.line_buffer.y0;
		ram_out.out_buf_cost_temp[0] =	out_buf.cost_temp;
		ram_out.out_buf_split_mode[0] =	out_buf.split_mode;
		out_buf.tag = bypass_local.in_buf_insn_tag;
		// ram_out.out_buf_tag	=	out_buf.tag;
		ram_out.out_buf_tag[0] = bypass_local.in_buf_insn_tag;
#else
		out_buf.tag = in_buf.insn.tag;						// in_buf.insn.tag will be dangling.
#endif // BYPASS_AXIS_ADAPTER
	}
	else
	{
#if !ONLY_QT_QT
		s64 cost1 = 0;

		if (in_buf.insn.cu_width_log2 == 5 && in_buf.insn.cu_height_log2 == 5)
		{
			cost1 += in_buf.cost_best[0][0];
			cost1 += in_buf.cost_best[0][4];
			cost1 += in_buf.cost_best[4][0];
			cost1 += in_buf.cost_best[4][4];
			cost1 += get_split_mode_cost_COM_combine_only_qt<32>(&in_buf.config.md_fw_ptr, in_buf.config.md_input_ptr.lambda_y, &out_buf.cu_data_temp, in_buf.insn.x0, in_buf.insn.y0, in_buf.insn.cud,
				in_buf.insn.qt_depth, in_buf.insn.bet_depth, in_buf.insn.cu_width, in_buf.insn.cu_height, in_buf.insn.split_mode);

			out_buf.cu_data_temp = in_buf.cu_data_temp_ptr;
		}
		else// if(64x64)
		{
			//cost += ...<64>;
		}
		//assert(cost1 == cost);
		out_buf.cost_temp = cost1;
		out_buf.tag = in_buf.insn.tag;

		out_buf.IP_map_scu_ptr_32x32 = in_buf.IP_map_scu_ptr_32x32;
#else
		assert(false);
#endif
	}
}

#if OUTPUT_AXIS_ADAPTER
void pu_output(Load2Output& in_buf, Output2Store& out_buf, ENC_CU_DATA& cu_data, U8& axis_in_in_buf_line_buffer_y0, U8 ram_out_out_buf_line_buffer_y0[1])
#else
void pu_output(Load2Output& in_buf, Output2Store& out_buf, ENC_CU_DATA& cu_data)
#endif // OUTPUT_AXIS_ADAPTER
{
#if pu_interface
#if OUTPUT_AXIS_ADAPTER
#pragma HLS INTERFACE axis port=axis_in_in_buf_line_buffer_y0
//#pragma HLS INTERFACE axis port=axis_out
#pragma HLS RESOURCE variable=ram_out_out_buf_line_buffer_y0 core=RAM_1P_BRAM
#else
#pragma HLS INTERFACE axis port=in_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=out_buf.line_buffer.y0
#endif // OUTPUT_AXIS_ADAPTER
#endif // pu_interface

#pragma HLS RESOURCE variable=cu_data.ipm core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=cu_data.mpm core=RAM_1P_BRAM

#pragma HLS ARRAY_PARTITION variable=cu_data.split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.split_mode complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mpm_ext cyclic factor=8 dim=1  
#pragma HLS ARRAY_PARTITION variable=cu_data.refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data.mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data.num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.reco_y cyclic factor=4 dim=2


#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.split_mode complete dim=2
#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.split_mode cyclic factor=16 dim=3
#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=in_buf.cu_data.reco_y cyclic factor=4 dim=2

#if OUTPUT_AXIS_ADAPTER
	in_buf.line_buffer.y0 = axis_in_in_buf_line_buffer_y0;
#endif // OUTPUT_AXIS_ADAPTER

	copy(out_buf.line_buffer, in_buf.line_buffer);
	cu_data.copy(in_buf.cu_data);

#if OUTPUT_AXIS_ADAPTER
	ram_out_out_buf_line_buffer_y0[0] = out_buf.line_buffer.y0;
#endif // OUTPUT_AXIS_ADAPTER
}

#if MD_CHANNEL_DEBUG
void bypass_channel(BpChn2Store& out_buf, Load2BpChn& in_buf)
{
	// stream port unused may cause core dump
#if pu_interface
#pragma HLS INTERFACE axis port=in_buf.w
#pragma HLS INTERFACE axis port=in_buf.h

#pragma HLS INTERFACE axis port=out_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_64.min_scu_x
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_64.min_scu_y
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_64.above_line_idx
#pragma HLS INTERFACE axis port=out_buf.me_line_map_ptr_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=out_buf.IP_buffer_ptr_32x32.top_left
#pragma HLS INTERFACE axis port=out_buf.IP_buffer_ptr_64x64.top_left
#pragma HLS INTERFACE axis port=out_buf.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=out_buf.MPM_buffer_ptr_32x32.left
#pragma HLS INTERFACE axis port=out_buf.MPM_buffer_ptr_64x64.up
#pragma HLS INTERFACE axis port=out_buf.MPM_buffer_ptr_64x64.left
#pragma HLS INTERFACE axis port=out_buf.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=out_buf.IP_map_scu_ptr_32x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=out_buf.IP_map_scu_ptr_64x64.map_scu_top_left
#pragma HLS INTERFACE axis port=out_buf.IP_map_scu_ptr_64x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=in_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_64.min_scu_x
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_64.min_scu_y
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_64.above_line_idx
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=in_buf.IP_buffer_ptr_32x32.top_left
#pragma HLS INTERFACE axis port=in_buf.IP_buffer_ptr_64x64.top_left
#pragma HLS INTERFACE axis port=in_buf.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=in_buf.MPM_buffer_ptr_32x32.left
#pragma HLS INTERFACE axis port=in_buf.MPM_buffer_ptr_64x64.up
#pragma HLS INTERFACE axis port=in_buf.MPM_buffer_ptr_64x64.left
#pragma HLS INTERFACE axis port=in_buf.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=in_buf.IP_map_scu_ptr_32x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=in_buf.IP_map_scu_ptr_64x64.map_scu_top_left
#pragma HLS INTERFACE axis port=in_buf.IP_map_scu_ptr_64x64.map_scu_top_left_new
#endif

#pragma HLS RESOURCE variable=in_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=out_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM

	copy(out_buf.line_buffer, in_buf.line_buffer);

	if (in_buf.w == 32 && in_buf.h == 32)
	{
		//memcpy(out_buf.up_nebs_32, in_buf.up_nebs_32, sizeof(out_buf.up_nebs_32));
		//memcpy(out_buf.left_nebs_32, in_buf.left_nebs_32, sizeof(out_buf.left_nebs_32));
		for (size_t i = 0; i < 10; i++)
		{
			out_buf.up_nebs_32[i] = in_buf.up_nebs_32[i];
		}
		for (size_t i = 0; i < 8; i++)
		{
			out_buf.left_nebs_32[i] = in_buf.left_nebs_32[i];
		}
		out_buf.me_line_map_ptr_32 = in_buf.me_line_map_ptr_32;
		out_buf.IP_buffer_ptr_32x32 = in_buf.IP_buffer_ptr_32x32;
		out_buf.IP_map_scu_ptr_32x32 = in_buf.IP_map_scu_ptr_32x32;
		out_buf.MPM_buffer_ptr_32x32 = in_buf.MPM_buffer_ptr_32x32;
	}	else if (in_buf.w == 64 && in_buf.h == 64)
	{
		out_buf.IP_buffer_ptr_64x64 = in_buf.IP_buffer_ptr_64x64;
		out_buf.IP_map_scu_ptr_64x64 = in_buf.IP_map_scu_ptr_64x64;
		out_buf.MPM_buffer_ptr_64x64 = in_buf.MPM_buffer_ptr_64x64;
		for (size_t i = 0; i < 18; i++)
		{
			out_buf.up_nebs_64[i] = in_buf.up_nebs_64[i];
		}

		out_buf.me_line_map_ptr_64 = in_buf.me_line_map_ptr_64;
		for (size_t i = 0; i < 16; i++)
		{
			out_buf.left_nebs_64[i] = in_buf.left_nebs_64[i];
		}
		for (size_t i = 0; i < 18; i++)
		{
			out_buf.up_nebs_32x32[i] = in_buf.up_nebs_32x32[i];
		}
		for (size_t i = 0; i < 16; i++)
		{
			out_buf.left_nebs_32x32[i] = in_buf.left_nebs_32x32[i];
		}

		out_buf.IP_map_scu_ptr_64x64 = in_buf.IP_map_scu_ptr_64x64;
		//out_buf.prev_cu_data_ptr_32x32 = in_buf.prev_cu_data_ptr_32x32;
		out_buf.prev_cu_data_ptr_32x32.copy(in_buf.prev_cu_data_ptr_32x32);

		for (size_t i = 0; i < 10; i++)
		{
			out_buf.up_nebs_32[i] = in_buf.up_nebs_32[i];
		}
		for (size_t i = 0; i < 8; i++)
		{
			out_buf.left_nebs_32[i] = in_buf.left_nebs_32[i];
		}
		out_buf.me_line_map_ptr_32 = in_buf.me_line_map_ptr_32;
		out_buf.IP_buffer_ptr_32x32 = in_buf.IP_buffer_ptr_32x32;
		out_buf.IP_map_scu_ptr_32x32 = in_buf.IP_map_scu_ptr_32x32;
		out_buf.MPM_buffer_ptr_32x32 = in_buf.MPM_buffer_ptr_32x32;
		for (int i = 0; i < 8; ++i)
		{
			out_buf.nebs_line_ver_32x32[i] = in_buf.nebs_line_ver_32x32[i];
		}
		for (int i = 0; i < 16; ++i)
		{
			out_buf.nebs_line_hor_32x32[i] = in_buf.nebs_line_hor_32x32[i];
		}
	}

	else
	{
		assert(false);
	}
}

void output_channel(Load2OutChn& in_buf, Channel& output)
{
	// stream port unused may cause core dump
#if pu_interface
#pragma HLS INTERFACE axis port=in_buf.w
#pragma HLS INTERFACE axis port=in_buf.h

#pragma HLS INTERFACE axis port=in_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=in_buf.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=in_buf.IP_map_scu_ptr_32x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=in_buf.prev_cu_data_ptr.x_chroma
#pragma HLS INTERFACE axis port=in_buf.prev_cu_data_ptr.y_chroma
#pragma HLS INTERFACE axis port=in_buf.prev_cu_data_ptr.x_last
#pragma HLS INTERFACE axis port=in_buf.prev_cu_data_ptr.y_last
#pragma HLS INTERFACE axis port=in_buf.prev_cu_data_ptr.last_child
#pragma HLS INTERFACE axis port=in_buf.prev_cu_data_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=in_buf.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=in_buf.IP_buffer_ptr_32x32.top_left
#pragma HLS INTERFACE axis port=in_buf.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=in_buf.MPM_buffer_ptr_32x32.left
#pragma HLS INTERFACE axis port=output.line_buffer.y0
#pragma HLS INTERFACE axis port=output.prev_cu_data_ptr.x_chroma
#pragma HLS INTERFACE axis port=output.prev_cu_data_ptr.y_chroma
#pragma HLS INTERFACE axis port=output.prev_cu_data_ptr.x_last
#pragma HLS INTERFACE axis port=output.prev_cu_data_ptr.y_last
#pragma HLS INTERFACE axis port=output.prev_cu_data_ptr.last_child
#pragma HLS INTERFACE axis port=output.prev_cu_data_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=output.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=output.IP_map_scu_ptr_32x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=output.MPM_buffer_32x32.up
#pragma HLS INTERFACE axis port=output.MPM_buffer_32x32.left
#pragma HLS INTERFACE axis port=output.IP_buffer_32x32.top_left
#pragma HLS INTERFACE axis port=output.IP_map_scu_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=output.IP_map_scu_32x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=output.me_line_map_32.min_scu_x
#pragma HLS INTERFACE axis port=output.me_line_map_32.min_scu_y
#pragma HLS INTERFACE axis port=output.me_line_map_32.above_line_idx
#pragma HLS INTERFACE axis port=output.me_line_map_32.curr_ctu_idx
#endif

#pragma HLS RESOURCE variable=in_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=in_buf.prev_cu_data_ptr core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=output core=RAM_1P_BRAM

	copy(output.line_buffer, in_buf.line_buffer);
	if (in_buf.w == 32 && in_buf.h == 32)
	{
		output.IP_map_scu_ptr_32x32 = in_buf.IP_map_scu_ptr_32x32;
		output.prev_cu_data_ptr = in_buf.prev_cu_data_ptr;
		for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; ++i)
		{
			for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j)
			{
				output.cost_best[i][j] = in_buf.cost_best[i][j];
			}
		}
		output.me_line_map_32 = in_buf.me_line_map_ptr_32;
		output.IP_buffer_32x32 = in_buf.IP_buffer_ptr_32x32;
		output.IP_map_scu_32x32 = in_buf.IP_map_scu_ptr_32x32;
		output.MPM_buffer_32x32 = in_buf.MPM_buffer_ptr_32x32;
		for (int i = 0; i < 8; ++i)
		{
			output.nebs_line_ver_32x32[i] = in_buf.nebs_line_ver_32x32[i];
		}
		for (int i = 0; i < 16; ++i)
		{
			output.nebs_line_hor_32x32[i] = in_buf.nebs_line_hor_32x32[i];
		}
	}
	else
	{
		assert(false);
	}
}
#endif
