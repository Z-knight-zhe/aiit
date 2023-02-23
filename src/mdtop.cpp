#include "md_cmod.h"
#include "md_dummy.h"
#include "md1.h"
#include "md2.h"

void set_pu_16x16_seperate_axis_port(Load2PU& in_buf, PUAxisInPorts_16x16 &axis_in)
{
	// seperate axis ports combining in struct, this function does not be synthesised, ports will be wrapped in RTL
	axis_in.in_buf_buffer_skip_in_slice_type = in_buf.buffer.skip_in.slice_type;
	axis_in.in_buf_buffer_skip_in_ptr = in_buf.buffer.skip_in.ptr;
	axis_in.in_buf_buffer_skip_in_umve_enable_flag = in_buf.buffer.skip_in.umve_enable_flag;
	axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru = in_buf.buffer.Fetch_Ref_window_ptr.code_stru;
	axis_in.in_buf_line_buffer_y0 = in_buf.line_buffer.y0;
	axis_in.in_buf_config_md_fw_ptr_num_refp_0 = in_buf.config.md_fw_ptr.num_refp[0];
	axis_in.in_buf_config_md_fw_ptr_num_refp_1 = in_buf.config.md_fw_ptr.num_refp[1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0 = in_buf.config.md_input_ptr.CtrPos2MD[0][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1 = in_buf.config.md_input_ptr.CtrPos2MD[0][0][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0 = in_buf.config.md_input_ptr.CtrPos2MD[0][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1 = in_buf.config.md_input_ptr.CtrPos2MD[0][1][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0 = in_buf.config.md_input_ptr.CtrPos2MD[1][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_1 = in_buf.config.md_input_ptr.CtrPos2MD[1][0][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0 = in_buf.config.md_input_ptr.CtrPos2MD[1][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_1 = in_buf.config.md_input_ptr.CtrPos2MD[1][1][1];
#if ENABLE_BFRAME
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_1 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_1 = in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][1];
#endif // ENABLE_BFRAME		
}

void set_pu_8x8_seperate_axis_in_port(const Load2PU& in_buf, PUAxisInPorts_8x8& axis_in)
{
// axis seperate in
	axis_in.in_buf_config_md_fw_ptr_pic_width=in_buf.config.md_fw_ptr.pic_width;
	axis_in.in_buf_config_md_fw_ptr_pic_height=in_buf.config.md_fw_ptr.pic_height;
	axis_in.in_buf_config_md_fw_ptr_slice_type=in_buf.config.md_fw_ptr.slice_type;
	axis_in.in_buf_config_md_fw_ptr_pic_width_in_scu=in_buf.config.md_fw_ptr.pic_width_in_scu;
	axis_in.in_buf_config_md_fw_ptr_pic_height_in_scu=in_buf.config.md_fw_ptr.pic_height_in_scu;
	axis_in.in_buf_config_md_fw_ptr_bit_depth_internal=in_buf.config.md_fw_ptr.bit_depth_internal;
	axis_in.in_buf_config_md_fw_ptr_max_part_ratio=in_buf.config.md_fw_ptr.max_part_ratio;
	axis_in.in_buf_config_md_fw_ptr_min_cu_size=in_buf.config.md_fw_ptr.min_cu_size;
	axis_in.in_buf_config_md_fw_ptr_max_split_times=in_buf.config.md_fw_ptr.max_split_times;
	axis_in.in_buf_config_md_fw_ptr_min_qt_size=in_buf.config.md_fw_ptr.min_qt_size;
	axis_in.in_buf_config_md_fw_ptr_max_bt_size=in_buf.config.md_fw_ptr.max_bt_size;
	axis_in.in_buf_config_md_fw_ptr_max_eqt_size=in_buf.config.md_fw_ptr.max_eqt_size;
	axis_in.in_buf_config_md_fw_ptr_mipf_enable_flag=in_buf.config.md_fw_ptr.mipf_enable_flag;
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_0_0=in_buf.config.md_fw_ptr.refp_ptr[0][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_0_1=in_buf.config.md_fw_ptr.refp_ptr[0][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_1_0=in_buf.config.md_fw_ptr.refp_ptr[1][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_1_1=in_buf.config.md_fw_ptr.refp_ptr[1][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_2_0=in_buf.config.md_fw_ptr.refp_ptr[2][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_2_1=in_buf.config.md_fw_ptr.refp_ptr[2][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_3_0=in_buf.config.md_fw_ptr.refp_ptr[3][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_3_1=in_buf.config.md_fw_ptr.refp_ptr[3][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_4_0=in_buf.config.md_fw_ptr.refp_ptr[4][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_4_1=in_buf.config.md_fw_ptr.refp_ptr[4][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_5_0=in_buf.config.md_fw_ptr.refp_ptr[5][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_5_1=in_buf.config.md_fw_ptr.refp_ptr[5][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_6_0=in_buf.config.md_fw_ptr.refp_ptr[6][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_6_1=in_buf.config.md_fw_ptr.refp_ptr[6][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_7_0=in_buf.config.md_fw_ptr.refp_ptr[7][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_7_1=in_buf.config.md_fw_ptr.refp_ptr[7][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_8_0=in_buf.config.md_fw_ptr.refp_ptr[8][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_8_1=in_buf.config.md_fw_ptr.refp_ptr[8][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_9_0=in_buf.config.md_fw_ptr.refp_ptr[9][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_9_1=in_buf.config.md_fw_ptr.refp_ptr[9][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_10_0=in_buf.config.md_fw_ptr.refp_ptr[10][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_10_1=in_buf.config.md_fw_ptr.refp_ptr[10][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_11_0=in_buf.config.md_fw_ptr.refp_ptr[11][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_11_1=in_buf.config.md_fw_ptr.refp_ptr[11][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_12_0=in_buf.config.md_fw_ptr.refp_ptr[12][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_12_1=in_buf.config.md_fw_ptr.refp_ptr[12][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_13_0=in_buf.config.md_fw_ptr.refp_ptr[13][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_13_1=in_buf.config.md_fw_ptr.refp_ptr[13][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_14_0=in_buf.config.md_fw_ptr.refp_ptr[14][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_14_1=in_buf.config.md_fw_ptr.refp_ptr[14][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_15_0=in_buf.config.md_fw_ptr.refp_ptr[15][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_15_1=in_buf.config.md_fw_ptr.refp_ptr[15][1];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_16_0=in_buf.config.md_fw_ptr.refp_ptr[16][0];
	axis_in.in_buf_config_md_fw_ptr_refp_ptr_16_1=in_buf.config.md_fw_ptr.refp_ptr[16][1];
	axis_in.in_buf_config_md_fw_ptr_num_refp_0=in_buf.config.md_fw_ptr.num_refp[0];
	axis_in.in_buf_config_md_fw_ptr_num_refp_1=in_buf.config.md_fw_ptr.num_refp[1];
	axis_in.in_buf_config_md_fw_ptr_cur_ptr=in_buf.config.md_fw_ptr.cur_ptr;
	axis_in.in_buf_config_md_input_ptr_pix_x=in_buf.config.md_input_ptr.pix_x;
	axis_in.in_buf_config_md_input_ptr_pix_y=in_buf.config.md_input_ptr.pix_y;
	axis_in.in_buf_config_md_input_ptr_lambda_y=in_buf.config.md_input_ptr.lambda_y;
	axis_in.in_buf_config_md_input_ptr_qp_y=in_buf.config.md_input_ptr.qp_y;
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0=in_buf.config.md_input_ptr.CtrPos2MD[0][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1=in_buf.config.md_input_ptr.CtrPos2MD[0][0][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0=in_buf.config.md_input_ptr.CtrPos2MD[0][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1=in_buf.config.md_input_ptr.CtrPos2MD[0][1][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0=in_buf.config.md_input_ptr.CtrPos2MD[1][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0=in_buf.config.md_input_ptr.CtrPos2MD[1][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0=in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1=in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][0][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0=in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1=in_buf.config.md_input_ptr.CtrPos2MD_ref1[0][1][1];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0=in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][0][0];
	axis_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0=in_buf.config.md_input_ptr.CtrPos2MD_ref1[1][1][0];
	axis_in.in_buf_config_md_input_ptr_cu_dqp_enable=in_buf.config.md_input_ptr.cu_dqp_enable;
	axis_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp=in_buf.config.md_input_ptr.cu_qp_group_pred_qp;
	axis_in.in_buf_config_min_scu_x=in_buf.config.min_scu_x;
	axis_in.in_buf_config_min_scu_y=in_buf.config.min_scu_y;
	axis_in.in_buf_buffer_skip_in_slice_type=in_buf.buffer.skip_in.slice_type;
	axis_in.in_buf_buffer_skip_in_ptr=in_buf.buffer.skip_in.ptr;
	axis_in.in_buf_buffer_skip_in_umve_enable_flag=in_buf.buffer.skip_in.umve_enable_flag;
	axis_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru=in_buf.buffer.Fetch_Ref_window_ptr.code_stru;
	axis_in.in_buf_insn_x0=in_buf.insn.x0;
	axis_in.in_buf_insn_y0=in_buf.insn.y0;
	axis_in.in_buf_insn_cu_width_log2=in_buf.insn.cu_width_log2;
	axis_in.in_buf_insn_cud=in_buf.insn.cud;
	axis_in.in_buf_insn_qt_depth=in_buf.insn.qt_depth;
	axis_in.in_buf_insn_bet_depth=in_buf.insn.bet_depth;
	axis_in.in_buf_insn_cu_width=in_buf.insn.cu_width;
	axis_in.in_buf_insn_cu_height=in_buf.insn.cu_height;
	axis_in.in_buf_insn_tree_status=in_buf.insn.tree_status;
	axis_in.in_buf_insn_split_mode=in_buf.insn.split_mode;
	axis_in.in_buf_insn_tag=in_buf.insn.tag;
	axis_in.in_buf_line_buffer_y0=in_buf.line_buffer.y0;
	//axis_in.out_buf_cons_pred_mode=out_buf.cons_pred_mode;
}

#if !VMOD_DUMMY
void vmod(LCUConfig& config, LCUInput& buffer, Arbitor& ref
#if MD_CHANNEL_DEBUG
	, Channel& input, Channel& output
#endif
	, Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], Instruction mem_store[PR_SIZE], ENC_CU_DATA& md_output, Register& reg)
{
#pragma HLS INLINE off

#if CMOD
	// stream port unused may cause core dump
#if vmod_interface
//#pragma HLS INTERFACE axis port=config.md_fw_ptr
//#pragma HLS INTERFACE axis port=config.md_input_ptr
#endif

#pragma HLS RESOURCE variable=ref->ref_0_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_0_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_1_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_1_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_2_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_2_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_3_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_3_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_4_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_4_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_5_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_5_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_6_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_6_1 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_7_0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ref->ref_7_1 core=RAM_1P_BRAM

	static int cnt = 0; ++cnt;

	static Load2Bp bp_in_buf[BP_NUM];
#if MD_CHANNEL_DEBUG
	static Load2BpChn bp_chn_in_buf;
	static Load2OutChn out_chn_in_buf;
	static BpChn2Store bp_chn_out_buf;
#endif
	static Load2PU      pu_in_buf[PU_NUM];
	static PU2Store     pu_out_buf[PU_NUM];
#if BYPASS_AXIS_ADAPTER
	static Load2Bp_AXIS bypass_in[PU_NUM];
	static PU2Store_AXIS bypass_out[PU_NUM];
#endif // BYPASS_AXIS_ADAPTER
	static ShareMemory  pool;
	static CuDataPool   cu_data;
	static Load2Output  out_in_buf;
	static Output2Store out_out_buf;
#if OUTPUT_AXIS_ADAPTER
	static U8 output_axis_in;
	 U8 ram_out_out_buf_line_buffer_y0[1];
#endif // OUTPUT_AXIS_ADAPTER
#if PU_16X8_AXIS_ADAPTER
       static PUAxisInPorts_16x8 pu_16x8_in;
       static PURamOutPorts_16x8 pu_16x8_out;
#endif // PU_16X8_AXIS_ADAPTER
#if PU_8X16_AXIS_ADAPTER
       static PUAxisInPorts_8x16 pu_8x16_in;
       static PURamOutPorts_8x16 pu_8x16_out;
#endif // PU_8X16_AXIS_ADAPTER
#if STORE_AXIS_ADAPTER
	   Store_RAM ram_in[PU_NUM];
#endif

	InstructionRegister<Instruction> ir1, ir2, ir;
	PRMan pu_man;
	hls::stream<U32> status;
	stream_type<ap_uint<1> > push_data_load[PU_NUM];
	reverse_stream_type<ap_uint<1> > push_data_store[PU_NUM];
#pragma HLS STREAM variable=status depth=2
	ir1.init();
	ir2.init();
	ir.init();
	pu_man.init();

	bool load_done = false, store_done = false;
	int cnt2 = 0;
LOOP_VMOD:while (!load_done || !store_done)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		++cnt2;
		if (!load_done)
		{
			load_done = load_core(ir1, ir2, mem_load, mem_pu, pu_man, status, reg
#if MD_CHANNEL_DEBUG
				, bp_chn_in_buf, out_chn_in_buf
#endif
				, pu_in_buf
#if MD_CHANNEL_DEBUG
				, input
#endif
				, pool, config, buffer, push_data_load, bp_in_buf, cu_data, out_in_buf);
		}
		for (int i = 0; i < BP_NUM; ++i)
		{
			if (!push_data_load[BYPASS + i].empty() && !push_data_store[BYPASS + i].sbuf.full())
			{
				push_data_load[BYPASS + i].read();
#if BYPASS_AXIS_ADAPTER
				axis_in[BYPASS + i].in_buf_insn_x0	=	bp_in_buf[BYPASS + i].insn.x0;
				axis_in[BYPASS + i].in_buf_insn_y0	=	bp_in_buf[BYPASS + i].insn.y0;
				axis_in[BYPASS + i].in_buf_insn_cu_width_log2	=	bp_in_buf[BYPASS + i].insn.cu_width_log2;
				axis_in[BYPASS + i].in_buf_insn_cu_height_log2	=	bp_in_buf[BYPASS + i].insn.cu_height_log2;
				axis_in[BYPASS + i].in_buf_insn_qt_depth	=	bp_in_buf[BYPASS + i].insn.qt_depth;
				axis_in[BYPASS + i].in_buf_insn_bet_depth	=	bp_in_buf[BYPASS + i].insn.bet_depth;
				axis_in[BYPASS + i].in_buf_insn_cu_width	=	bp_in_buf[BYPASS + i].insn.cu_width;
				axis_in[BYPASS + i].in_buf_insn_cu_height	=	bp_in_buf[BYPASS + i].insn.cu_height;
				axis_in[BYPASS + i].in_buf_insn_split_mode	=	bp_in_buf[BYPASS + i].insn.split_mode;
				axis_in[BYPASS + i].in_buf_insn_tag	=	bp_in_buf[BYPASS + i].insn.tag;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_pic_width	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.pic_width;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_pic_height	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.pic_height;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_max_part_ratio	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.max_part_ratio;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_min_cu_size	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.min_cu_size;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_max_split_times	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.max_split_times;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_min_qt_size	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.min_qt_size;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_max_bt_size	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.max_bt_size;
				axis_in[BYPASS + i].in_buf_config_md_fw_ptr_max_eqt_size	=	bp_in_buf[BYPASS + i].config.md_fw_ptr.max_eqt_size;
				axis_in[BYPASS + i].in_buf_config_md_input_ptr_lambda_y	=	bp_in_buf[BYPASS + i].config.md_input_ptr.lambda_y;
				axis_in[BYPASS + i].in_buf_line_buffer_y0	=	bp_in_buf[BYPASS + i].line_buffer.y0;
				bypass(bp_in_buf[BYPASS + i], pu_out_buf[BYPASS + i], axis_in[BYPASS + i], ram_out[BYPASS + i]);
				pu_out_buf[BYPASS + i].line_buffer.y0	=	ram_out[BYPASS + i].out_buf_line_buffer_y0[0];
				pu_out_buf[BYPASS + i].cost_temp	=	ram_out[BYPASS + i].out_buf_cost_temp[0];
				pu_out_buf[BYPASS + i].split_mode	=	ram_out[BYPASS + i].out_buf_split_mode[0];
				pu_out_buf[BYPASS + i].tag	=	ram_out[BYPASS + i].tag[0];
#else
				bypass(bp_in_buf[BYPASS + i], pu_out_buf[BYPASS + i]);
#endif // BYPASS_AXIS_ADAPTER
				push_data_store[BYPASS + i].sbuf.write(1);
			}
		}
#if !ONLY_QT_QT
		if (!push_data_load[KERNEL_32X32].empty() && !push_data_store[KERNEL_32X32].sbuf.full())
		{
			push_data_load[KERNEL_32X32].read();

			pu<KERNEL_32X32>(pu_in_buf[KERNEL_32X32], pu_out_buf[KERNEL_32X32], ref);
			push_data_store[KERNEL_32X32].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X32_1].empty() && !push_data_store[KERNEL_16X32_1].sbuf.full())
		{
			push_data_load[KERNEL_16X32_1].read();

			pu<KERNEL_16X32_1>(pu_in_buf[KERNEL_16X32_1], pu_out_buf[KERNEL_16X32_1], ref);
			push_data_store[KERNEL_16X32_1].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X16_1].empty() && !push_data_store[KERNEL_16X16_1].sbuf.full())
		{
			push_data_load[KERNEL_16X16_1].read();

			pu<KERNEL_16X16_1>(pu_in_buf[KERNEL_16X16_1], pu_out_buf[KERNEL_16X16_1], ref);
			push_data_store[KERNEL_16X16_1].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_8X16_1].empty() && !push_data_store[KERNEL_8X16_1].sbuf.full())
		{
			push_data_load[KERNEL_8X16_1].read();

			pu<KERNEL_8X16_1>(pu_in_buf[KERNEL_8X16_1], pu_out_buf[KERNEL_8X16_1], ref);
			push_data_store[KERNEL_8X16_1].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X8_1].empty() && !push_data_store[KERNEL_16X8_1].sbuf.full())
		{
			push_data_load[KERNEL_16X8_1].read();

			pu<KERNEL_16X8_1>(pu_in_buf[KERNEL_16X8_1], pu_out_buf[KERNEL_16X8_1], ref);
			push_data_store[KERNEL_16X8_1].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_32X16_1].empty() && !push_data_store[KERNEL_32X16_1].sbuf.full())
		{
			push_data_load[KERNEL_32X16_1].read();

			pu<KERNEL_32X16_1>(pu_in_buf[KERNEL_32X16_1], pu_out_buf[KERNEL_32X16_1], ref);
			push_data_store[KERNEL_32X16_1].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X16_2].empty() && !push_data_store[KERNEL_16X16_2].sbuf.full())
		{
			push_data_load[KERNEL_16X16_2].read();

			pu<KERNEL_16X16_2>(pu_in_buf[KERNEL_16X16_2], pu_out_buf[KERNEL_16X16_2], ref);
			push_data_store[KERNEL_16X16_2].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_8X16_2].empty() && !push_data_store[KERNEL_8X16_2].sbuf.full())
		{
			push_data_load[KERNEL_8X16_2].read();

			pu<KERNEL_8X16_2>(pu_in_buf[KERNEL_8X16_2], pu_out_buf[KERNEL_8X16_2], ref);
			push_data_store[KERNEL_8X16_2].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X8_2].empty() && !push_data_store[KERNEL_16X8_2].sbuf.full())
		{
			push_data_load[KERNEL_16X8_2].read();

			pu<KERNEL_16X8_2>(pu_in_buf[KERNEL_16X8_2], pu_out_buf[KERNEL_16X8_2], ref);
			push_data_store[KERNEL_16X8_2].sbuf.write(1);
		}
#endif
		if (!push_data_load[KERNEL_16X16].empty() && !push_data_store[KERNEL_16X16].sbuf.full())
		{
			push_data_load[KERNEL_16X16].read();

			PUAxisInPorts_16x16 axis_in;
			PURamOut_16x16 ram_out;
			set_pu_16x16_seperate_axis_port(pu_in_buf[KERNEL_16X16], axis_in);
			pu_KERNEL_16X16(pu_in_buf[KERNEL_16X16], pu_out_buf[KERNEL_16X16], ref, axis_in, ram_out);
			push_data_store[KERNEL_16X16].sbuf.write(1);
#if DEBUG_PRINT_PU_COST
			printf("%d,%d: (%d,%d) pu_16x16=%lld\n", cnt, cnt2, pu_in_buf[KERNEL_16X16].insn.x0, pu_in_buf[KERNEL_16X16].insn.y0, pu_out_buf[KERNEL_16X16].cost_temp);
#endif
		}
#if !ONLY_NS_16X16
		if (!push_data_load[KERNEL_8X16].empty() && !push_data_store[KERNEL_8X16].sbuf.full())
		{
			push_data_load[KERNEL_8X16].read();
#if PU_8X16_AXIS_ADAPTER
			pu_8x16_in.in_buf_config_md_fw_ptr_pic_width = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.pic_width;
			pu_8x16_in.in_buf_config_md_fw_ptr_pic_height = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.pic_height;
			pu_8x16_in.in_buf_config_md_fw_ptr_slice_type = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.slice_type;
			pu_8x16_in.in_buf_config_md_fw_ptr_pic_width_in_scu = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.pic_width_in_scu;
			pu_8x16_in.in_buf_config_md_fw_ptr_pic_height_in_scu = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.pic_height_in_scu;
			pu_8x16_in.in_buf_config_md_fw_ptr_log2_culine = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.log2_culine;
			pu_8x16_in.in_buf_config_md_fw_ptr_bit_depth_internal = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.bit_depth_internal;
			pu_8x16_in.in_buf_config_md_fw_ptr_max_part_ratio = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.max_part_ratio;
			pu_8x16_in.in_buf_config_md_fw_ptr_min_cu_size = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.min_cu_size;
			pu_8x16_in.in_buf_config_md_fw_ptr_max_split_times = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.max_split_times;
			pu_8x16_in.in_buf_config_md_fw_ptr_min_qt_size = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.min_qt_size;
			pu_8x16_in.in_buf_config_md_fw_ptr_max_bt_size = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.max_bt_size;
			pu_8x16_in.in_buf_config_md_fw_ptr_max_eqt_size = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.max_eqt_size;
			pu_8x16_in.in_buf_config_md_fw_ptr_mipf_enable_flag = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.mipf_enable_flag;
			pu_8x16_in.in_buf_config_md_fw_ptr_cur_ptr = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.cur_ptr;
			pu_8x16_in.in_buf_config_md_input_ptr_pix_x = pu_in_buf[KERNEL_8X16].config.md_input_ptr.pix_x;
			pu_8x16_in.in_buf_config_md_input_ptr_pix_y = pu_in_buf[KERNEL_8X16].config.md_input_ptr.pix_y;
			pu_8x16_in.in_buf_config_md_input_ptr_lambda_y = pu_in_buf[KERNEL_8X16].config.md_input_ptr.lambda_y;
			pu_8x16_in.in_buf_config_md_input_ptr_qp_y = pu_in_buf[KERNEL_8X16].config.md_input_ptr.qp_y;
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD[0][0][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD[0][0][1];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD[0][1][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD[0][1][1];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD[1][0][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD[1][1][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD_ref1[0][0][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD_ref1[0][0][1];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD_ref1[0][1][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD_ref1[0][1][1];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD_ref1[1][0][0];
			pu_8x16_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 = pu_in_buf[KERNEL_8X16].config.md_input_ptr.CtrPos2MD_ref1[1][1][0];
			pu_8x16_in.in_buf_config_md_input_ptr_cu_dqp_enable = pu_in_buf[KERNEL_8X16].config.md_input_ptr.cu_dqp_enable;
			pu_8x16_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp = pu_in_buf[KERNEL_8X16].config.md_input_ptr.cu_qp_group_pred_qp;
			pu_8x16_in.in_buf_config_min_scu_x = pu_in_buf[KERNEL_8X16].config.min_scu_x;
			pu_8x16_in.in_buf_config_min_scu_y = pu_in_buf[KERNEL_8X16].config.min_scu_y;
			pu_8x16_in.in_buf_buffer_skip_in_slice_type = pu_in_buf[KERNEL_8X16].buffer.skip_in.slice_type;
			pu_8x16_in.in_buf_buffer_skip_in_ptr = pu_in_buf[KERNEL_8X16].buffer.skip_in.ptr;
			pu_8x16_in.in_buf_buffer_skip_in_umve_enable_flag = pu_in_buf[KERNEL_8X16].buffer.skip_in.umve_enable_flag;
			pu_8x16_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru = pu_in_buf[KERNEL_8X16].buffer.Fetch_Ref_window_ptr.code_stru;
			pu_8x16_in.in_buf_insn_x0 = pu_in_buf[KERNEL_8X16].insn.x0;
			pu_8x16_in.in_buf_insn_y0 = pu_in_buf[KERNEL_8X16].insn.y0;
			pu_8x16_in.in_buf_insn_cup = pu_in_buf[KERNEL_8X16].insn.cup;
			pu_8x16_in.in_buf_insn_cu_width_log2 = pu_in_buf[KERNEL_8X16].insn.cu_width_log2;
			pu_8x16_in.in_buf_insn_cud = pu_in_buf[KERNEL_8X16].insn.cud;
			pu_8x16_in.in_buf_insn_qt_depth = pu_in_buf[KERNEL_8X16].insn.qt_depth;
			pu_8x16_in.in_buf_insn_bet_depth = pu_in_buf[KERNEL_8X16].insn.bet_depth;
			pu_8x16_in.in_buf_insn_cons_pred_mode = pu_in_buf[KERNEL_8X16].insn.cons_pred_mode;
			pu_8x16_in.in_buf_insn_cu_width = pu_in_buf[KERNEL_8X16].insn.cu_width;
			pu_8x16_in.in_buf_insn_cu_height = pu_in_buf[KERNEL_8X16].insn.cu_height;
			pu_8x16_in.in_buf_insn_tree_status = pu_in_buf[KERNEL_8X16].insn.tree_status;
			pu_8x16_in.in_buf_insn_split_mode = pu_in_buf[KERNEL_8X16].insn.split_mode;
			pu_8x16_in.in_buf_insn_tag = pu_in_buf[KERNEL_8X16].insn.tag;
			pu_8x16_in.in_buf_line_buffer_y0 = pu_in_buf[KERNEL_8X16].line_buffer.y0;
			pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[0][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[0][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[0][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[0][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[1][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[1][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[1][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[1][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[2][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[2][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[2][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[2][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[3][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[3][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[3][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[3][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[4][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[4][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[4][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[4][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[5][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[5][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[5][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[5][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[6][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[6][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[6][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[6][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[7][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[7][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[7][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[7][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[8][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[8][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[8][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[8][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[9][0]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[9][0]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[9][1]  = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[9][1]         ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[10][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[10][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[10][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[10][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[11][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[11][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[11][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[11][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[12][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[12][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[12][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[12][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[13][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[13][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[13][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[13][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[14][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[14][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[14][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[14][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[15][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[15][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[15][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[15][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[16][0] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[16][0]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_refp_ptr[16][1] = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.refp_ptr[16][1]        ;           
            pu_8x16_in.in_buf_config_md_fw_ptr_num_refp[0]     = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.num_refp[0]            ;      
            pu_8x16_in.in_buf_config_md_fw_ptr_num_refp[1]     = pu_in_buf[KERNEL_8X16].config.md_fw_ptr.num_refp[1]            ; 
			pu_KERNEL_8X16(pu_in_buf[KERNEL_8X16], pu_out_buf[KERNEL_8X16], ref, pu_8x16_in, pu_8x16_out);
			pu_out_buf[KERNEL_8X16].line_buffer.y0 = pu_8x16_out.out_buf_line_buffer_y0[0];
			pu_out_buf[KERNEL_8X16].cost_temp = pu_8x16_out.out_buf_cost_temp[0];
			pu_out_buf[KERNEL_8X16].cons_pred_mode = pu_8x16_out.out_buf_cons_pred_mode[0];
			pu_out_buf[KERNEL_8X16].tree_status_child = pu_8x16_out.out_buf_tree_status_child[0];
			pu_out_buf[KERNEL_8X16].split_mode = pu_8x16_out.out_buf_split_mode[0];
			pu_out_buf[KERNEL_8X16].tag = pu_8x16_out.out_buf_tag[0];
#else

			pu_KERNEL_8X16(pu_in_buf[KERNEL_8X16], pu_out_buf[KERNEL_8X16], ref);
#endif // PU_8X16_AXIS_ADAPTER
			push_data_store[KERNEL_8X16].sbuf.write(1);
#if DEBUG_PRINT_PU_COST
			printf("%d,%d: (%d,%d) pu_8x16=%lld\n", cnt, cnt2, pu_in_buf[KERNEL_16X16].insn.x0, pu_in_buf[KERNEL_16X16].insn.y0, pu_out_buf[KERNEL_8X16].cost_temp);
#endif
		}
		if (!push_data_load[KERNEL_16X8].empty() && !push_data_store[KERNEL_16X8].sbuf.full())
		{
			push_data_load[KERNEL_16X8].read();
#if PU_16X8_AXIS_ADAPTER
			pu_16x8_in.in_buf_config_md_fw_ptr_pic_width = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.pic_width;
			pu_16x8_in.in_buf_config_md_fw_ptr_pic_height = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.pic_height;
			pu_16x8_in.in_buf_config_md_fw_ptr_slice_type = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.slice_type;
			pu_16x8_in.in_buf_config_md_fw_ptr_pic_width_in_scu = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.pic_width_in_scu;
			pu_16x8_in.in_buf_config_md_fw_ptr_pic_height_in_scu = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.pic_height_in_scu;
			pu_16x8_in.in_buf_config_md_fw_ptr_log2_culine = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.log2_culine;
			pu_16x8_in.in_buf_config_md_fw_ptr_bit_depth_internal = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.bit_depth_internal;
			pu_16x8_in.in_buf_config_md_fw_ptr_max_part_ratio = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.max_part_ratio;
			pu_16x8_in.in_buf_config_md_fw_ptr_min_cu_size = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.min_cu_size;
			pu_16x8_in.in_buf_config_md_fw_ptr_max_split_times = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.max_split_times;
			pu_16x8_in.in_buf_config_md_fw_ptr_min_qt_size = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.min_qt_size;
			pu_16x8_in.in_buf_config_md_fw_ptr_max_bt_size = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.max_bt_size;
			pu_16x8_in.in_buf_config_md_fw_ptr_max_eqt_size = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.max_eqt_size;
			pu_16x8_in.in_buf_config_md_fw_ptr_mipf_enable_flag = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.mipf_enable_flag;
			pu_16x8_in.in_buf_config_md_fw_ptr_cur_ptr = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.cur_ptr;
			pu_16x8_in.in_buf_config_md_input_ptr_pix_x = pu_in_buf[KERNEL_16X8].config.md_input_ptr.pix_x;
			pu_16x8_in.in_buf_config_md_input_ptr_pix_y = pu_in_buf[KERNEL_16X8].config.md_input_ptr.pix_y;
			pu_16x8_in.in_buf_config_md_input_ptr_lambda_y = pu_in_buf[KERNEL_16X8].config.md_input_ptr.lambda_y;
			pu_16x8_in.in_buf_config_md_input_ptr_qp_y = pu_in_buf[KERNEL_16X8].config.md_input_ptr.qp_y;
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD[0][0][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_0_0_1 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD[0][0][1];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD[0][1][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_0_1_1 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD[0][1][1];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_1_0_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD[1][0][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_1_1_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD[1][1][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD_ref1[0][0][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD_ref1[0][0][1];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD_ref1[0][1][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD_ref1[0][1][1];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD_ref1[1][0][0];
			pu_16x8_in.in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 = pu_in_buf[KERNEL_16X8].config.md_input_ptr.CtrPos2MD_ref1[1][1][0];
			pu_16x8_in.in_buf_config_md_input_ptr_cu_dqp_enable = pu_in_buf[KERNEL_16X8].config.md_input_ptr.cu_dqp_enable;
			pu_16x8_in.in_buf_config_md_input_ptr_cu_qp_group_pred_qp = pu_in_buf[KERNEL_16X8].config.md_input_ptr.cu_qp_group_pred_qp;
			pu_16x8_in.in_buf_config_min_scu_x = pu_in_buf[KERNEL_16X8].config.min_scu_x;
			pu_16x8_in.in_buf_config_min_scu_y = pu_in_buf[KERNEL_16X8].config.min_scu_y;
			pu_16x8_in.in_buf_buffer_skip_in_slice_type = pu_in_buf[KERNEL_16X8].buffer.skip_in.slice_type;
			pu_16x8_in.in_buf_buffer_skip_in_ptr = pu_in_buf[KERNEL_16X8].buffer.skip_in.ptr;
			pu_16x8_in.in_buf_buffer_skip_in_umve_enable_flag = pu_in_buf[KERNEL_16X8].buffer.skip_in.umve_enable_flag;
			pu_16x8_in.in_buf_buffer_Fetch_Ref_window_ptr_code_stru = pu_in_buf[KERNEL_16X8].buffer.Fetch_Ref_window_ptr.code_stru;
			pu_16x8_in.in_buf_insn_x0 = pu_in_buf[KERNEL_16X8].insn.x0;
			pu_16x8_in.in_buf_insn_y0 = pu_in_buf[KERNEL_16X8].insn.y0;
			pu_16x8_in.in_buf_insn_cup = pu_in_buf[KERNEL_16X8].insn.cup;
			pu_16x8_in.in_buf_insn_cu_width_log2 = pu_in_buf[KERNEL_16X8].insn.cu_width_log2;
			pu_16x8_in.in_buf_insn_cud = pu_in_buf[KERNEL_16X8].insn.cud;
			pu_16x8_in.in_buf_insn_qt_depth = pu_in_buf[KERNEL_16X8].insn.qt_depth;
			pu_16x8_in.in_buf_insn_bet_depth = pu_in_buf[KERNEL_16X8].insn.bet_depth;
			pu_16x8_in.in_buf_insn_cons_pred_mode = pu_in_buf[KERNEL_16X8].insn.cons_pred_mode;
			pu_16x8_in.in_buf_insn_cu_width = pu_in_buf[KERNEL_16X8].insn.cu_width;
			pu_16x8_in.in_buf_insn_cu_height = pu_in_buf[KERNEL_16X8].insn.cu_height;
			pu_16x8_in.in_buf_insn_tree_status = pu_in_buf[KERNEL_16X8].insn.tree_status;
			pu_16x8_in.in_buf_insn_split_mode = pu_in_buf[KERNEL_16X8].insn.split_mode;
			pu_16x8_in.in_buf_insn_tag = pu_in_buf[KERNEL_16X8].insn.tag;
			pu_16x8_in.in_buf_line_buffer_y0 = pu_in_buf[KERNEL_16X8].line_buffer.y0;
			pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[0][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[0][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[0][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[0][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[1][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[1][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[1][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[1][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[2][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[2][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[2][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[2][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[3][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[3][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[3][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[3][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[4][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[4][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[4][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[4][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[5][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[5][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[5][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[5][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[6][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[6][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[6][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[6][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[7][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[7][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[7][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[7][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[8][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[8][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[8][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[8][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[9][0]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[9][0]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[9][1]  = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[9][1]         ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[10][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[10][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[10][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[10][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[11][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[11][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[11][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[11][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[12][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[12][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[12][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[12][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[13][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[13][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[13][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[13][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[14][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[14][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[14][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[14][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[15][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[15][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[15][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[15][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[16][0] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[16][0]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_refp_ptr[16][1] = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.refp_ptr[16][1]        ;           
            pu_16x8_in.in_buf_config_md_fw_ptr_num_refp[0]     = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.num_refp[0]            ;      
            pu_16x8_in.in_buf_config_md_fw_ptr_num_refp[1]     = pu_in_buf[KERNEL_16X8].config.md_fw_ptr.num_refp[1]            ; 
			pu_KERNEL_16X8(pu_in_buf[KERNEL_16X8], pu_out_buf[KERNEL_16X8], ref, pu_16x8_in, pu_16x8_out);
			pu_out_buf[KERNEL_16X8].line_buffer.y0 = pu_16x8_out.out_buf_line_buffer_y0[0];
			pu_out_buf[KERNEL_16X8].cost_temp = pu_16x8_out.out_buf_cost_temp[0];
			pu_out_buf[KERNEL_16X8].cons_pred_mode = pu_16x8_out.out_buf_cons_pred_mode[0];
			pu_out_buf[KERNEL_16X8].tree_status_child = pu_16x8_out.out_buf_tree_status_child[0];
			pu_out_buf[KERNEL_16X8].split_mode = pu_16x8_out.out_buf_split_mode[0];
			pu_out_buf[KERNEL_16X8].tag = pu_16x8_out.out_buf_tag[0];
#else
			pu_KERNEL_16X8(pu_in_buf[KERNEL_16X8], pu_out_buf[KERNEL_16X8], ref);
#endif // PU_16X8_AXIS_ADAPTER

			push_data_store[KERNEL_16X8].sbuf.write(1);
#if DEBUG_PRINT_PU_COST
			printf("%d,%d: (%d,%d) pu_16x8=%lld\n", cnt, cnt2, pu_in_buf[KERNEL_16X16].insn.x0, pu_in_buf[KERNEL_16X16].insn.y0, pu_out_buf[KERNEL_16X8].cost_temp);
#endif
		}
		if (!push_data_load[KERNEL_8X8].empty() && !push_data_store[KERNEL_8X8].sbuf.full())
		{
			PUAxisInPorts_8x8 axis_in;
			PURamOut_8x8 ram_out;
			push_data_load[KERNEL_8X8].read();
			set_pu_8x8_seperate_axis_in_port(pu_in_buf[KERNEL_8X8], axis_in);
			pu_KERNEL_8X8(pu_in_buf[KERNEL_8X8], pu_out_buf[KERNEL_8X8], ref, axis_in, ram_out);
			push_data_store[KERNEL_8X8].sbuf.write(1);
#if DEBUG_PRINT_PU_COST
			printf("%d,%d: (%d,%d) pu_8x8=%lld\n", cnt, cnt2, pu_in_buf[KERNEL_16X16].insn.x0, pu_in_buf[KERNEL_16X16].insn.y0, pu_out_buf[KERNEL_8X8].cost_temp);
#endif
		}
#endif //!ONLY_NS_16X16
#if !ONLY_QT_QT
		if (!push_data_load[KERNEL_64X64].empty() && !push_data_store[KERNEL_64X64].sbuf.full())
		{
			push_data_load[KERNEL_64X64].read();

			pu<KERNEL_64X64>(pu_in_buf[KERNEL_64X64], pu_out_buf[KERNEL_64X64], ref);
			push_data_store[KERNEL_64X64].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_32X64].empty() && !push_data_store[KERNEL_32X64].sbuf.full())
		{
			push_data_load[KERNEL_32X64].read();

			pu<KERNEL_32X64>(pu_in_buf[KERNEL_32X64], pu_out_buf[KERNEL_32X64], ref);
			push_data_store[KERNEL_32X64].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X64].empty() && !push_data_store[KERNEL_16X64].sbuf.full())
		{
			push_data_load[KERNEL_16X64].read();

			pu<KERNEL_16X64>(pu_in_buf[KERNEL_16X64], pu_out_buf[KERNEL_16X64], ref);
			push_data_store[KERNEL_16X64].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X32_2].empty() && !push_data_store[KERNEL_16X32_2].sbuf.full())
		{
			push_data_load[KERNEL_16X32_2].read();

			pu<KERNEL_16X32_2>(pu_in_buf[KERNEL_16X32_2], pu_out_buf[KERNEL_16X32_2], ref);
			push_data_store[KERNEL_16X32_2].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X16_4].empty() && !push_data_store[KERNEL_16X16_4].sbuf.full())
		{
			push_data_load[KERNEL_16X16_4].read();

			pu<KERNEL_16X16_4>(pu_in_buf[KERNEL_16X16_4], pu_out_buf[KERNEL_16X16_4], ref);
			push_data_store[KERNEL_16X16_4].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_8X16_4].empty() && !push_data_store[KERNEL_8X16_4].sbuf.full())
		{
			push_data_load[KERNEL_8X16_4].read();

			pu<KERNEL_8X16_4>(pu_in_buf[KERNEL_8X16_4], pu_out_buf[KERNEL_8X16_4], ref);
			push_data_store[KERNEL_8X16_4].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X8_4].empty() && !push_data_store[KERNEL_16X8_4].sbuf.full())
		{
			push_data_load[KERNEL_16X8_4].read();

			pu<KERNEL_16X8_4>(pu_in_buf[KERNEL_16X8_4], pu_out_buf[KERNEL_16X8_4], ref);
			push_data_store[KERNEL_16X8_4].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_8X64].empty() && !push_data_store[KERNEL_8X64].sbuf.full())
		{
			push_data_load[KERNEL_8X64].read();

			pu<KERNEL_8X64>(pu_in_buf[KERNEL_8X64], pu_out_buf[KERNEL_8X64], ref);
			push_data_store[KERNEL_8X64].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_64X32].empty() && !push_data_store[KERNEL_64X32].sbuf.full())
		{
			push_data_load[KERNEL_64X32].read();

			pu<KERNEL_64X32>(pu_in_buf[KERNEL_64X32], pu_out_buf[KERNEL_64X32], ref);
			push_data_store[KERNEL_64X32].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_64X16].empty() && !push_data_store[KERNEL_64X16].sbuf.full())
		{
			push_data_load[KERNEL_64X16].read();

			pu<KERNEL_64X16>(pu_in_buf[KERNEL_64X16], pu_out_buf[KERNEL_64X16], ref);
			push_data_store[KERNEL_64X16].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_32X16_2].empty() && !push_data_store[KERNEL_32X16_2].sbuf.full())
		{
			push_data_load[KERNEL_32X16_2].read();

			pu<KERNEL_32X16_2>(pu_in_buf[KERNEL_32X16_2], pu_out_buf[KERNEL_32X16_2], ref);
			push_data_store[KERNEL_32X16_2].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X16_5].empty() && !push_data_store[KERNEL_16X16_5].sbuf.full())
		{
			push_data_load[KERNEL_16X16_5].read();

			pu<KERNEL_16X16_5>(pu_in_buf[KERNEL_16X16_5], pu_out_buf[KERNEL_16X16_5], ref);
			push_data_store[KERNEL_16X16_5].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_8X16_5].empty() && !push_data_store[KERNEL_8X16_5].sbuf.full())
		{
			push_data_load[KERNEL_8X16_5].read();

			pu<KERNEL_8X16_5>(pu_in_buf[KERNEL_8X16_5], pu_out_buf[KERNEL_8X16_5], ref);
			push_data_store[KERNEL_8X16_5].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_16X8_5].empty() && !push_data_store[KERNEL_16X8_5].sbuf.full())
		{
			push_data_load[KERNEL_16X8_5].read();

			pu<KERNEL_16X8_5>(pu_in_buf[KERNEL_16X8_5], pu_out_buf[KERNEL_16X8_5], ref);
			push_data_store[KERNEL_16X8_5].sbuf.write(1);
		}
		if (!push_data_load[KERNEL_64X8].empty() && !push_data_store[KERNEL_64X8].sbuf.full())
		{
			push_data_load[KERNEL_64X8].read();

			pu<KERNEL_64X8>(pu_in_buf[KERNEL_64X8], pu_out_buf[KERNEL_64X8], ref);
			push_data_store[KERNEL_64X8].sbuf.write(1);
		}
#endif
#if MD_CHANNEL_DEBUG
		if (!push_data_load[BYPASS_CHANNEL].empty() && !push_data_store[BYPASS_CHANNEL].sbuf.full())
		{
			push_data_load[BYPASS_CHANNEL].read();
			bypass_channel(bp_chn_out_buf, bp_chn_in_buf);
			push_data_store[BYPASS_CHANNEL].sbuf.write(1);
		}
		if (!push_data_load[OUTPUT_CHANNEL].empty() && !push_data_store[OUTPUT_CHANNEL].sbuf.full())
		{
			push_data_load[OUTPUT_CHANNEL].read();
			output_channel(out_chn_in_buf, output);
			//push_data_store[PU_OUTPUT_CHANNEL].sbuf.write(1);
		}
#endif
		if (!push_data_load[PU_OUTPUT].empty() && !push_data_store[PU_OUTPUT].sbuf.full())
		{
			push_data_load[PU_OUTPUT].read();
#if OUTPUT_AXIS_ADAPTER
			output_axis_in = out_in_buf.line_buffer.y0;
			pu_output(out_in_buf, out_out_buf, md_output, output_axis_in, ram_out_out_buf_line_buffer_y0);
			out_out_buf.line_buffer.y0 = ram_out_out_buf_line_buffer_y0[0];
#else
			pu_output(out_in_buf, out_out_buf, md_output);
#endif
			push_data_store[PU_OUTPUT].sbuf.write(1);
		}
		if (!store_done)
		{
#if STORE_AXIS_ADAPTER
			for (int i=0; i<PU_NUM; i++) {
				ram_in[i].line_buffer_y0[0] = pu_out_buf[i].line_buffer.y0;
				ram_in[i].cost_temp[0]      = pu_out_buf[i].cost_temp;
				ram_in[i].split_mode[0]     = pu_out_buf[i].split_mode;
				ram_in[i].tag[0]            = pu_out_buf[i].tag;
				ram_in[i].cu_data_temp_cu_qp_group_pred_qp[0]      = pu_out_buf[i].cu_data_temp.cu_qp_group_pred_qp;
				ram_in[i].curr_cu_data_temp_cu_qp_group_pred_qp[0] = pu_out_buf[i].curr_cu_data_temp.cu_qp_group_pred_qp;
				ram_in[i].curr_cu_data_temp_64_cu_qp_group_pred_qp[0] = pu_out_buf[i].curr_cu_data_temp_64.cu_qp_group_pred_qp;
			}
#endif //STORE_AXIS_ADAPTER

			store_done = store_core(ir, mem_store, status, reg, push_data_store, config
#if MD_CHANNEL_DEBUG
				, bp_chn_out_buf
#endif
				, pu_out_buf, 
#if STORE_AXIS_ADAPTER
				ram_in,
#endif
				pool, cu_data, out_out_buf);
		}
	}
#endif // CMOD
}
#endif // !VMOD_DUMMY

void read_m_axi(Instruction load[PR_SIZE], Instruction pu[PR_SIZE], Instruction store[PR_SIZE], Instruction* mem_load, Instruction* mem_pu, Instruction* mem_store)
{
#pragma HLS INLINE off

#pragma HLS INTERFACE m_axi port=mem_load  offset=slave bundle=mdprogs
#pragma HLS INTERFACE m_axi port=mem_pu    offset=slave bundle=mdprogs
#pragma HLS INTERFACE m_axi port=mem_store offset=slave bundle=mdprogs

	memcpy(load,  mem_load,  PR_SIZE * sizeof(Instruction));
	memcpy(pu,    mem_pu,    PR_SIZE * sizeof(Instruction));
	memcpy(store, mem_store, PR_SIZE * sizeof(Instruction));
}

void read_axis(LCUConfig& config, FME_OUT_PARA_SET* fme_out_para_set, int min_scu_x, int min_scu_y)
{
#pragma HLS INLINE off

	write_in_para_set_md(fme_out_para_set, &config.md_fw_ptr, &config.md_input_ptr);
	config.min_scu_x = min_scu_x;
	config.min_scu_y = min_scu_y;
}

void print(pel d[71][71])
{
	for (int i = 0; i < 71; ++i)
	{
		for (int j = 0; j < 71; ++j)
		{
			printf("%d ", d[i][j]);
		}
		printf("\n");
	}
}

void write_mem(LCUConfig& config, ENC_CU_DATA& cu_data, ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], pel dbk_y[71][71], ENC_CU_DATA* md2_output, ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output)
{
#pragma HLS inline

#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_input->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_input->map_cu_mode_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_output->map_scu_inner complete dim=1
#pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_output->map_cu_mode_inner complete dim=1

#pragma HLS ARRAY_PARTITION variable=md2_output->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->split_mode cyclic factor=7 dim=2   
#pragma HLS ARRAY_PARTITION variable=md2_output->mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=md2_output->mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mvd cyclic factor=2 dim=3  
#pragma HLS ARRAY_PARTITION variable=md2_output->num_nz_coef cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=md2_output->mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=md2_output->ipm cyclic factor=2 dim=1
// #pragma HLS ARRAY_PARTITION variable=dbk_y cyclic factor=4 dim=1					// Conflict with DBK
// #pragma HLS ARRAY_PARTITION variable=dbk_y cyclic factor=4 dim=2

// #pragma HLS ARRAY_PARTITION variable=cu_data.split_mode cyclic factor=5 dim=1
// #pragma HLS ARRAY_PARTITION variable=cu_data.split_mode cyclic factor=7 dim=2   
// #pragma HLS ARRAY_PARTITION variable=cu_data.mpm_ext cyclic factor=8 dim=1  
// #pragma HLS ARRAY_PARTITION variable=cu_data.refi cyclic factor=2 dim=2
// #pragma HLS ARRAY_PARTITION variable=cu_data.mv cyclic factor=2 dim=2
// #pragma HLS ARRAY_PARTITION variable=cu_data.mv cyclic factor=2 dim=3
// #pragma HLS ARRAY_PARTITION variable=cu_data.mvd cyclic factor=2 dim=2
// #pragma HLS ARRAY_PARTITION variable=cu_data.mvd cyclic factor=2 dim=3      
// #pragma HLS ARRAY_PARTITION variable=cu_data.num_nz_coef cyclic factor=3 dim=1
// #pragma HLS ARRAY_PARTITION variable=cu_data.coef_y cyclic factor=4 dim=1
// #pragma HLS ARRAY_PARTITION variable=cu_data.coef_y cyclic factor=4 dim=2
// #pragma HLS ARRAY_PARTITION variable=cu_data.reco_y cyclic factor=4 dim=1
// #pragma HLS ARRAY_PARTITION variable=cu_data.reco_y cyclic factor=4 dim=2


#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].refi complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0].mv complete dim=2

	me_line_map_ptr_output->min_scu_x = me_line_map_ptr_input->min_scu_x;
	me_line_map_ptr_output->min_scu_y = me_line_map_ptr_input->min_scu_y;
	me_line_map_ptr_output->stride_in_map = me_line_map_ptr_input->stride_in_map;
	me_line_map_ptr_output->stride_in_line = me_line_map_ptr_input->stride_in_line;
	me_line_map_ptr_output->pic_width_in_scu = me_line_map_ptr_input->pic_width_in_scu;
	me_line_map_ptr_output->pic_height_in_scu = me_line_map_ptr_input->pic_height_in_scu;
	me_line_map_ptr_output->above_line_idx = me_line_map_ptr_input->above_line_idx;
	me_line_map_ptr_output->curr_ctu_idx = me_line_map_ptr_input->curr_ctu_idx;
	me_line_map_ptr_output->x_ctu = me_line_map_ptr_input->x_ctu;
	me_line_map_ptr_output->y_ctu = me_line_map_ptr_input->y_ctu;
	
	store_enc_info_top_paral(&config.md_fw_ptr, &config.md_input_ptr,
		0, 0, NO_MODE_CONS, 64,
		64, TREE_LC, 0,
		&cu_data,
		md2_output, dbk_y,
		cur_ctu_mi,
		me_line_map_ptr_input, me_line_map_ptr_output,
		0, 0);
	//copy_enc_me_line_map(me_line_map_ptr_input, me_line_map_ptr_output);
	for (int i = 0; i < ABOVE_LINE_LEN; i++)
	{
		me_line_map_ptr_output->new_map_scu_above_line[i] = me_line_map_ptr_input->new_map_scu_above_line[i];
		me_line_map_ptr_output->new_map_cu_mode_above_line[i] = me_line_map_ptr_input->new_map_cu_mode_above_line[i];
	}

#if PRINT_MD_OUTPUT
	print(dbk_y);
#endif
}

#if ONLY_QT_QT
// void md_run(Instruction* mem_load, Instruction* mem_pu, Instruction* mem_store,
// 			pel fme2mdrefbuf[2560][8][2][16],			// FETCH
// #if ENABLE_BFRAME
// 			ME_MV_DIR & me_mv_dir_fme2md,				// FME, no scalar.
// #endif
// 			ENC_ME_NEB_INFO cur_ctu_mi[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], // 2CHROMA
// 			SKIP_MERGE_INPUT & skip_input_ptr,			// FETCH
// 			RMD_OUTPUT & rmd_output,					// RMD, No scalar.
// 			ENC_FME_INFO_ALL & fme_mv_info,				// FME, No scalar.
// 			pel dbk_y[71][71],							// 2DBK
// 			FME_OUT_PARA_SET & fme_out_para_set, FME_OUT_PARA_SET_AXIS & axis_fme_out_para_set, FME_OUT_PARA_SET & fme_out_para_set_md2chroma,	// FME, 2CHROMA
// 			ENC_CU_DATA & md_output_luma,				// 2CHROMA, no scalar.
// 			ENC_ME_LINE_MAP * me_line_map_ptr,   ENC_ME_LINE_MAP_AXIS * axis_me_line_map_ptr,   ENC_ME_LINE_MAP * me_line_map_ptr_luma,			// FETCH, 2CHROMA
// 			strFetch_ref_window & Fetch_Ref_window, strFetch_ref_window & Fetch_Ref_window_md2chroma)											// FETCH, 2CHROMA
void md_run(
			// FIXME ZP: 20221022 cosim SEGV
			Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], Instruction mem_store[PR_SIZE], Register& reg,
			LCUInput & buffer, Arbitor & ref, int axis_Fetch_Ref_window_ptr_code_stru,
			ENC_ME_NEB_INFO cur_ctu_mi[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], // 2CHROMA
			pel dbk_y[71][71],							// 2DBK
			FME_OUT_PARA_SET & fme_out_para_set, FME_OUT_PARA_SET_AXIS & axis_fme_out_para_set,	// FME, 2CHROMA
			ENC_CU_DATA & md_output_luma,				// 2CHROMA, no scalar.
			ENC_ME_LINE_MAP * me_line_map_ptr,   ENC_ME_LINE_MAP_AXIS * axis_me_line_map_ptr,   ENC_ME_LINE_MAP * me_line_map_ptr_luma, 
			const pel ram_in_buffer_p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], const SKIP_MERGE_INPUT_AXIS_IN &skip_in_axis_in)			// FETCH, 2CHROMA
{
#pragma HLS INLINE off

#if 1 // Turn on all interface pragmas

#define _MD_RUN_AXIS_EN_ 1
#define _MD_RUN_IF_PARTITION_EN_ 1
// =================================================================================================
// INTERFACE CONSTRAINTS
// =================================================================================================
#if 1 // RAM core
#pragma HLS RESOURCE variable=ref													core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.RefWindowLCU           	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.swOutLv0               	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.CtrPos                 	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.RefWindowLCU_UP        	core=RAM_1P_BRAM         
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.RefWindowLCU_VP        	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.lidx                   	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.cur_refi               	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.refp_pic_ptr           	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.refp_is_library_picture	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.CtrPos_ref1            	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.RefWindowLCU_ref1      	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.swOutLv0_ref1          	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.RefWindowLCU_UP_ref1   	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.Fetch_Ref_window_ptr.RefWindowLCU_VP_ref1   	core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=buffer.me_mv_dir.block64x64MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block32x32MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block64x32MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block32x64MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block16x16MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block32x16MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block16x32MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block64x16MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block16x64MvDir 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block8x8MvDir   						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block16x8MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block8x16MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block32x8MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block8x32MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block64x8MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block8x64MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block32x4MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block4x32MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block16x4MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block4x16MvDir  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block8x4MvDir   						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.me_mv_dir.block4x8MvDir   						core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=cur_ctu_mi 											core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=buffer.skip_in.refp_map_mv    						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.skip_in.refp_map_refi  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.skip_in.refp_map_mv1   						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.skip_in.refp_map_refi1 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.skip_in.refp_list_ptr1 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.skip_in.refp_ptr1      						core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_8x8   						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_8x16  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_16x8  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_8x32  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_32x8  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_16x16 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_16x32 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_32x16 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_32x32 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_64x8  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_64x16 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_64x32 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_8x64  						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_16x64 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_32x64 						core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.g_rmd_output.modes_64x64 						core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree64x64Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree64x32Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree32x64Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree64x16Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree16x64Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree64x8Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree8x64Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree32x4Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree4x32Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree32x32Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree16x16Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree32x16Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree16x32Fmv 					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree8x8Fmv   					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree16x8Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree8x16Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree32x8Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree8x32Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree4x8Fmv   					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree8x4Fmv   					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree16x4Fmv  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=buffer.fme_mv_info.sadTree4x16Fmv  					core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=dbk_y 												core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=fme_out_para_set.refp_ptr            					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.num_refp            					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.CtrPosFME_ref1      					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.CtrPos2MD_ref1      					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.lambda              					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.dist_chroma_weight  					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.CtrPosFME           					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.CtrPos2MD           					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.p_fenc_LCU_Y        					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.p_fenc_LCU_U        					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.p_fenc_LCU_V        					core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=fme_out_para_set.CuQPMap             					core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=md_output_luma.split_mode                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pb_part                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.tb_part                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pred_mode                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mpm                            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mpm_ext                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.ipm                            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.refi                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mvr_idx                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.umve_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.umve_idx                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.skip_idx                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mvp_from_hmvp_flag             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mv                             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mvd                            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.num_nz_coef                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.map_scu                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.affine_flag                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.smvd_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.map_cu_mode                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.map_pb_tb_part                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.depth                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.ipf_flag                       core=RAM_1P_BRAM
//#pragma HLS RESOURCE variable=md_output_luma.coef                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.coef_y                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.coef_u                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.coef_v                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.reco_y                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.reco_u                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.reco_v                         core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sawp_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sawp_idx0                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sawp_idx1                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_mvr_flag0                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_mvr_idx0                   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_mvr_flag1                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_mvr_idx1                   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.bvr_idx                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.inter_filter_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.ipc_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.bgc_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.bgc_idx                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.ibc_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.cbvp_idx                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.cnt_hbvp_cands                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.map_usp                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sp_flag                        core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sub_string_no                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sp_strInfo                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sp_copy_direction              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.is_sp_pix_completed            core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.cs2_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.evs_copy_direction             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.evs_sub_string_no              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.evs_str_copy_info              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.unpred_pix_num                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.unpred_pix_info                core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.equal_val_str_present_flag     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.unpredictable_pix_present_flag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pvbuf_size                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pvbuf_size_prev                core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.p_SRB                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pvbuf_reused_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.p_SRB_prev                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.all_comp_flag                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.all_comp_pre_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.m_dpb_idx                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.m_dpb_idx_prev                 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.m_dpb_reYonly                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.m_dpb_reYonly_prev             core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.cuS_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.cuS_pre_flag                   core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pv_x                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pv_x_prev                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pv_y                           core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.pv_y_prev                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.srb_u                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.srb_v                          core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.affine_umve_flag               core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.affine_umve_idx                core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.etmvp_flag                     core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.mvap_flag                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sub_tmvp_flag                  core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_idx0                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.awp_idx1                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.st_chroma_tu_flag              core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.iip_flag                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.ist_tu_flag                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.est_tu_flag                    core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.sawp_mpm                       core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.y_org_sao                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.u_org_sao                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.v_org_sao                      core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=md_output_luma.cu_luma_qp                     core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=me_line_map_ptr->new_map_scu_above_line     		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->new_map_cu_mode_above_line 		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->map_scu_inner              		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->map_cu_mode_inner          		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->co_left_mv                 		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->co_left_refi               		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr->co_left_ptr_ref            		core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->new_map_scu_above_line     	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->new_map_cu_mode_above_line 	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->map_scu_inner              	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->map_cu_mode_inner          	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->co_left_mv                 	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->co_left_refi               	core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=me_line_map_ptr_luma->co_left_ptr_ref            	core=RAM_1P_BRAM
#endif 

#if (1 & _MD_RUN_AXIS_EN_)
#pragma HLS INTERFACE axis port=axis_Fetch_Ref_window_ptr_code_stru
#pragma HLS INTERFACE axis port=reg._sys

#pragma HLS INTERFACE axis port=axis_fme_out_para_set.pic_width
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.pic_height
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.slice_type
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.max_cuwh
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.log2_culine
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.max_part_ratio
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.min_cu_size
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.max_split_times
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.min_qt_size
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.max_bt_size
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.max_eqt_size
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.cur_ptr
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.pix_x
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.pix_y
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.qp_y
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.x_last_ctu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.y_last_ctu
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.umve_set_flag
#pragma HLS INTERFACE axis port=axis_fme_out_para_set.cu_qp_group_pred_qp

// #pragma HLS INTERFACE axis port=me_line_map_ptr->min_scu_x
// #pragma HLS INTERFACE axis port=me_line_map_ptr->min_scu_y
// #pragma HLS INTERFACE axis port=me_line_map_ptr->stride_in_map
// #pragma HLS INTERFACE axis port=me_line_map_ptr->stride_in_line
// #pragma HLS INTERFACE axis port=me_line_map_ptr->width_in_scu
// #pragma HLS INTERFACE axis port=me_line_map_ptr->height_in_scu
// #pragma HLS INTERFACE axis port=me_line_map_ptr->above_line_idx
// #pragma HLS INTERFACE axis port=me_line_map_ptr->curr_ctu_idx
// #pragma HLS INTERFACE axis port=me_line_map_ptr->x_ctu
// #pragma HLS INTERFACE axis port=me_line_map_ptr->y_ctu

#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->min_scu_x
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->min_scu_y
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->stride_in_map
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->stride_in_line
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->above_line_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->curr_ctu_idx
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->x_ctu
#pragma HLS INTERFACE axis port=axis_me_line_map_ptr->y_ctu

#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->min_scu_x
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->min_scu_y
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->stride_in_map
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->stride_in_line
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->pic_width_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->pic_height_in_scu
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->above_line_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->curr_ctu_idx
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->x_ctu
#pragma HLS INTERFACE axis port=me_line_map_ptr_luma->y_ctu

#pragma HLS INTERFACE axis port=skip_in_axis_in.buffer_skip_in_slice_type         
#pragma HLS INTERFACE axis port=skip_in_axis_in.buffer_skip_in_ptr
#pragma HLS INTERFACE axis port=skip_in_axis_in.buffer_skip_in_umve_enable_flag
#pragma HLS INTERFACE axis port=skip_in_axis_in.buffer_skip_in_pix_x

// #pragma HLS INTERFACE axis port=reg._sys
#endif
// =================================================================================================

// =================================================================================================
// PARTITION OF INTERFACE
// =================================================================================================
#if (1 && _MD_RUN_IF_PARTITION_EN_)
// --- FETCH ------------------------------------------------------------------------------
// #pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window.RefWindowLCU_UP              dim=1 cyclic factor=128
// #pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window.RefWindowLCU_VP              dim=1 cyclic factor=128
// #pragma HLS ARRAY_PARTITION variable=skip_input_ptr.refp_map_mv   				dim=0 complete
// #pragma HLS ARRAY_PARTITION variable=skip_input_ptr.refp_map_refi 				dim=0 complete
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->new_map_scu_above_line     dim=1 cyclic factor=48
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->new_map_cu_mode_above_line dim=1 cyclic factor=48
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_scu_inner              dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr->map_cu_mode_inner          dim=1 complete
// ----------------------------------------------------------------------------------------
// --- RMD --------------------------------------------------------------------------------
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_8x8   dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_16x8  dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_32x8  dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_64x8  dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_8x16  dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_16x16 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_32x16 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_64x16 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_8x32  dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_16x32 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_32x32 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_64x32 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_8x64  dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_16x64 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_32x64 dim=2 complete
#pragma HLS ARRAY_PARTITION variable=buffer.g_rmd_output.modes_64x64 dim=2 complete
// ----------------------------------------------------------------------------------------
// --- FME --------------------------------------------------------------------------------
// #pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf	dim=1 block factor=2
// #pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf	dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf	dim=3 complete
// #pragma HLS ARRAY_PARTITION variable=fme2mdrefbuf	dim=4 complete
// fme_mv_info      NO PARTITION
// me_mv_dir_fme2md NO PARTITION
#pragma HLS ARRAY_PARTITION variable=fme_out_para_set.p_fenc_LCU_Y dim=1 cyclic factor=64
#pragma HLS ARRAY_PARTITION variable=fme_out_para_set.p_fenc_LCU_U dim=1 cyclic factor=32
#pragma HLS ARRAY_PARTITION variable=fme_out_para_set.p_fenc_LCU_V dim=1 cyclic factor=32
#pragma HLS ARRAY_PARTITION variable=buffer.p_fenc_LCU_Y			dim=1 cyclic factor=64				// assign with fme_out_para_set.p_fenc_LCU_Y
// -----------------------------------------------------------------------------------------
// --- MD_CHROMA ---------------------------------------------------------------------------
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0][0].refi    complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0][0].mv      complete dim=1
#pragma HLS ARRAY_PARTITION variable=cur_ctu_mi[0][0][0].mv      complete dim=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.coef_y       dim=1 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=md_output_luma.coef_y       dim=2 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=md_output_luma.reco_y       dim=1 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=md_output_luma.reco_y       dim=2 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=md_output_luma.split_mode   dim=1 cyclic factor=5
#pragma HLS ARRAY_PARTITION variable=md_output_luma.split_mode   dim=2 cyclic factor=7
#pragma HLS ARRAY_PARTITION variable=md_output_luma.mpm_ext      dim=1 cyclic factor=8
#pragma HLS ARRAY_PARTITION variable=md_output_luma.mpm          dim=1 cyclic factor=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.refi         dim=2 cyclic factor=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.mv           dim=2 cyclic factor=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.mv           dim=3 cyclic factor=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.mvd          dim=2 cyclic factor=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.mvd          dim=3 cyclic factor=2
#pragma HLS ARRAY_PARTITION variable=md_output_luma.num_nz_coef  dim=1 cyclic factor=3
// #pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma.RefWindowLCU_UP         dim=1 cyclic factor=128
// #pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma.RefWindowLCU_VP         dim=1 cyclic factor=128
// #pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma.RefWindowLCU_UP_ref1    dim=1 cyclic factor=128
// #pragma HLS ARRAY_RESHAPE variable=Fetch_Ref_window_md2chroma.RefWindowLCU_VP_ref1    dim=1 cyclic factor=128
// -----------------------------------------------------------------------------------------
// --- DBK ---------------------------------------------------------------------------------
#pragma HLS ARRAY_PARTITION variable=dbk_y complete dim=2
// -----------------------------------------------------------------------------------------
// --- MD_CHROMA ---------------------------------------------------------------------------
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_luma->new_map_scu_above_line     dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_luma->new_map_cu_mode_above_line dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_luma->map_scu_inner              dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=me_line_map_ptr_luma->map_cu_mode_inner          dim=1 complete
// -----------------------------------------------------------------------------------------
#endif
// =================================================================================================
#endif 

#pragma HLS ARRAY_PARTITION variable=ram_in_buffer_p_fenc_LCU_Y cyclic factor=64 dim=1

	for (int i = 0; i < MAX_CU_SIZE_FETCH; i++) {
#pragma HLS PIPELINE
		for (int j = 0; j < MAX_CU_SIZE_FETCH; j++) {
			buffer.p_fenc_LCU_Y[j][i] = ram_in_buffer_p_fenc_LCU_Y[j][i];
		}
	}

    buffer.skip_in.slice_type = skip_in_axis_in.buffer_skip_in_slice_type;
    buffer.skip_in.ptr = skip_in_axis_in.buffer_skip_in_ptr;
    buffer.skip_in.umve_enable_flag = skip_in_axis_in.buffer_skip_in_umve_enable_flag;
    buffer.skip_in.pix_x = skip_in_axis_in.buffer_skip_in_pix_x;

#if 1 // AXIS in
	fme_out_para_set.pic_width			 = axis_fme_out_para_set.pic_width			;
	fme_out_para_set.pic_height			 = axis_fme_out_para_set.pic_height			;
	fme_out_para_set.max_cuwh			 = axis_fme_out_para_set.max_cuwh			;
	fme_out_para_set.pix_x				 = axis_fme_out_para_set.pix_x				;
	fme_out_para_set.pix_y				 = axis_fme_out_para_set.pix_y				;
	fme_out_para_set.slice_type			 = axis_fme_out_para_set.slice_type			;
	fme_out_para_set.pic_width_in_scu	 = axis_fme_out_para_set.pic_width_in_scu	;
	fme_out_para_set.pic_height_in_scu	 = axis_fme_out_para_set.pic_height_in_scu	;
	fme_out_para_set.log2_max_cuwh		 = axis_fme_out_para_set.log2_max_cuwh		;
	fme_out_para_set.log2_culine		 = axis_fme_out_para_set.log2_culine		;
	fme_out_para_set.bit_depth_internal	 = axis_fme_out_para_set.bit_depth_internal	;
	fme_out_para_set.max_part_ratio		 = axis_fme_out_para_set.max_part_ratio		;
	fme_out_para_set.min_cu_size		 = axis_fme_out_para_set.min_cu_size		;
	fme_out_para_set.max_split_times	 = axis_fme_out_para_set.max_split_times	;
	fme_out_para_set.min_qt_size		 = axis_fme_out_para_set.min_qt_size		;
	fme_out_para_set.max_bt_size		 = axis_fme_out_para_set.max_bt_size		;
	fme_out_para_set.max_eqt_size		 = axis_fme_out_para_set.max_eqt_size		;
	fme_out_para_set.cur_ptr			 = axis_fme_out_para_set.cur_ptr			;
	fme_out_para_set.eipm_enable_flag	 = axis_fme_out_para_set.eipm_enable_flag	;
	fme_out_para_set.mipf_enable_flag	 = axis_fme_out_para_set.mipf_enable_flag	;
	fme_out_para_set.interpf_enable_flag = axis_fme_out_para_set.interpf_enable_flag;
	fme_out_para_set.qp_y				 = axis_fme_out_para_set.qp_y				;
	fme_out_para_set.x_last_ctu			 = axis_fme_out_para_set.x_last_ctu			;
	fme_out_para_set.y_last_ctu			 = axis_fme_out_para_set.y_last_ctu			;
	fme_out_para_set.umve_set_flag		 = axis_fme_out_para_set.umve_set_flag		;
	fme_out_para_set.cu_qp_group_pred_qp = axis_fme_out_para_set.cu_qp_group_pred_qp;
	if (fme_out_para_set.slice_type != 1) {
		buffer.Fetch_Ref_window_ptr.code_stru = axis_Fetch_Ref_window_ptr_code_stru;	// fetch��slice_typeΪ1�ǣ������Fetch_Ref_window_code_stru�ź�
	}
	me_line_map_ptr->min_scu_x		     = axis_me_line_map_ptr->min_scu_x		    ;
	me_line_map_ptr->min_scu_y		     = axis_me_line_map_ptr->min_scu_y		    ;
	me_line_map_ptr->stride_in_map	     = axis_me_line_map_ptr->stride_in_map	    ;
	me_line_map_ptr->stride_in_line	     = axis_me_line_map_ptr->stride_in_line     ;
	me_line_map_ptr->pic_width_in_scu    = axis_me_line_map_ptr->pic_width_in_scu   ;
	me_line_map_ptr->pic_height_in_scu   = axis_me_line_map_ptr->pic_height_in_scu  ;
	me_line_map_ptr->above_line_idx	     = axis_me_line_map_ptr->above_line_idx     ;
	me_line_map_ptr->curr_ctu_idx	     = axis_me_line_map_ptr->curr_ctu_idx	    ;
	me_line_map_ptr->x_ctu			     = axis_me_line_map_ptr->x_ctu			    ;
	me_line_map_ptr->y_ctu			     = axis_me_line_map_ptr->y_ctu			    ;
#endif
	// STATIC_C LCUInput buffer;
	// STATIC_C Arbitor  ref;
	// md_input_connection(buffer, ref, rmd_output, me_mv_dir_fme2md, fme_mv_info, skip_input_ptr, Fetch_Ref_window, fme_out_para_set.p_fenc_LCU_Y, fme2mdrefbuf);
	
	md_run_qtqt(buffer, ref, mem_load, mem_pu, mem_store, reg, &fme_out_para_set, cur_ctu_mi[me_line_map_ptr->curr_ctu_idx], dbk_y, &md_output_luma, me_line_map_ptr, me_line_map_ptr_luma);

#if 1 // 2 CHROMA
#if 0 // Duplicate by FME
	fme_out_para_set_md2chroma.pic_width												  = fme_out_para_set.pic_width												  ;
	fme_out_para_set_md2chroma.pic_height												  = fme_out_para_set.pic_height												  ;
	fme_out_para_set_md2chroma.slice_type												  = fme_out_para_set.slice_type												  ;
	fme_out_para_set_md2chroma.affine_enable_flag										  = fme_out_para_set.affine_enable_flag										  ;
	fme_out_para_set_md2chroma.pic_width_in_scu											  = fme_out_para_set.pic_width_in_scu											  ;
	fme_out_para_set_md2chroma.pic_height_in_scu										  = fme_out_para_set.pic_height_in_scu										  ;
	fme_out_para_set_md2chroma.log2_max_cuwh											  = fme_out_para_set.log2_max_cuwh											  ;
	fme_out_para_set_md2chroma.log2_culine												  = fme_out_para_set.log2_culine												  ;
	fme_out_para_set_md2chroma.max_cuwh													  = fme_out_para_set.max_cuwh													  ;
	fme_out_para_set_md2chroma.bit_depth_internal										  = fme_out_para_set.bit_depth_internal										  ;
	fme_out_para_set_md2chroma.max_part_ratio											  = fme_out_para_set.max_part_ratio											  ;
	fme_out_para_set_md2chroma.min_cu_size												  = fme_out_para_set.min_cu_size												  ;
	fme_out_para_set_md2chroma.max_split_times											  = fme_out_para_set.max_split_times											  ;
	fme_out_para_set_md2chroma.min_qt_size												  = fme_out_para_set.min_qt_size												  ;
	fme_out_para_set_md2chroma.max_bt_size												  = fme_out_para_set.max_bt_size												  ;
	fme_out_para_set_md2chroma.max_eqt_size												  = fme_out_para_set.max_eqt_size												  ;
	fme_out_para_set_md2chroma.cur_ptr													  = fme_out_para_set.cur_ptr													  ;
	fme_out_para_set_md2chroma.pix_x													  = fme_out_para_set.pix_x													  ;
	fme_out_para_set_md2chroma.pix_y													  = fme_out_para_set.pix_y													  ;
	fme_out_para_set_md2chroma.eipm_enable_flag											  = fme_out_para_set.eipm_enable_flag											  ;
	fme_out_para_set_md2chroma.mipf_enable_flag											  = fme_out_para_set.mipf_enable_flag											  ;
	fme_out_para_set_md2chroma.interpf_enable_flag										  = fme_out_para_set.interpf_enable_flag										  ;
	fme_out_para_set_md2chroma.qp_y														  = fme_out_para_set.qp_y														  ;
	fme_out_para_set_md2chroma.qp_u														  = fme_out_para_set.qp_u														  ;
	fme_out_para_set_md2chroma.qp_v														  = fme_out_para_set.qp_v														  ;
	fme_out_para_set_md2chroma.x_last_ctu												  = fme_out_para_set.x_last_ctu												  ;
	fme_out_para_set_md2chroma.y_last_ctu												  = fme_out_para_set.y_last_ctu												  ;
	fme_out_para_set_md2chroma.umve_set_flag											  = fme_out_para_set.umve_set_flag											  ;
	fme_out_para_set_md2chroma.cu_dqp_enable											  = fme_out_para_set.cu_dqp_enable											  ;
	fme_out_para_set_md2chroma.cu_qp_group_area_size									  = fme_out_para_set.cu_qp_group_area_size									  ;
	fme_out_para_set_md2chroma.cu_qp_group_pred_qp										  = fme_out_para_set.cu_qp_group_pred_qp										  ;
	fme_out_para_set_md2chroma.chroma_quant_param_delta_cb								  = fme_out_para_set.chroma_quant_param_delta_cb								  ;
	fme_out_para_set_md2chroma.chroma_quant_param_delta_cr								  = fme_out_para_set.chroma_quant_param_delta_cr								  ;
	fme_out_para_set_md2chroma.md_slice_qp												  = fme_out_para_set.md_slice_qp												  ;
	for (int i = 0; i < 17; i++) {
		for (int j = 0; j < 2; j++) {
			fme_out_para_set_md2chroma.refp_ptr[i][j] = fme_out_para_set.refp_ptr[i][j];
		}
	}
	for (int i = 0; i < REFP_NUM; i++) {
		fme_out_para_set_md2chroma.num_refp[i] = fme_out_para_set.num_refp[i];
	}
	for (int i = 0; i < RANGE_NUM; i++) {
		for (int j = 0; j < DIR_DIM; j++) {
			for (int k = 0; k < MAP_DIMS; k++) {
				fme_out_para_set_md2chroma.CtrPosFME_ref1[i][j][k] = fme_out_para_set.CtrPosFME_ref1[i][j][k];
				fme_out_para_set_md2chroma.CtrPos2MD_ref1[i][j][k] = fme_out_para_set.CtrPos2MD_ref1[i][j][k];
				fme_out_para_set_md2chroma.CtrPosFME[i][j][k]      = fme_out_para_set.CtrPosFME[i][j][k];
				fme_out_para_set_md2chroma.CtrPos2MD[i][j][k]      = fme_out_para_set.CtrPos2MD[i][j][k];
			}
		}
	}
	for (int i = 0; i < 3; i++) {
		fme_out_para_set_md2chroma.lambda[i] = fme_out_para_set.lambda[i];
	}
	for (int i = 0; i < 2; i++) {
		fme_out_para_set_md2chroma.dist_chroma_weight[i] = fme_out_para_set.dist_chroma_weight[i];
	}
	for (int i = 0; i < MAX_CU_SIZE_FETCH; i++) {
		for (int j = 0; j < MAX_CU_SIZE_FETCH; j++) {
			fme_out_para_set_md2chroma.p_fenc_LCU_Y[i][j] = fme_out_para_set.p_fenc_LCU_Y[i][j];
		}
	}
	for (int i = 0; i < MAX_CU_SIZE_FETCH / 2; i++) {
		for (int j = 0; j < MAX_CU_SIZE_FETCH / 2; j++) {
			fme_out_para_set_md2chroma.p_fenc_LCU_U[i][j] = fme_out_para_set.p_fenc_LCU_U[i][j];
			fme_out_para_set_md2chroma.p_fenc_LCU_V[i][j] = fme_out_para_set.p_fenc_LCU_V[i][j];
		}
	}
	for (int i = 0; i < 256; i++) {
		fme_out_para_set_md2chroma.CuQPMap[i] = fme_out_para_set.CuQPMap[i];
	}
#endif

#if 0 // Haoyun: FETCH does not wr.
	Fetch_Ref_window_md2chroma.code_stru = Fetch_Ref_window.code_stru;
	for (int i = 0; i < SWW * SWH; i++) {
		Fetch_Ref_window_md2chroma.RefWindowLCU     [i] = Fetch_Ref_window.RefWindowLCU     [i];
		Fetch_Ref_window_md2chroma.RefWindowLCU_ref1[i] = Fetch_Ref_window.RefWindowLCU_ref1[i];
	}

	for (int i = 0; i < SWH; i++) {
		for (int j = 0; j < SWW; j++) {
			Fetch_Ref_window_md2chroma.swOutLv0     [i][j] = Fetch_Ref_window.swOutLv0     [i][j];
			Fetch_Ref_window_md2chroma.swOutLv0_ref1[i][j] = Fetch_Ref_window.swOutLv0_ref1[i][j];
		}
	}
#endif
#if 0 // Re-wire at enc.top
	for (int i = 0; i < (SWH * SWW) >> 2; i++) {
		Fetch_Ref_window_md2chroma.RefWindowLCU_UP     [i] = Fetch_Ref_window.RefWindowLCU_UP[i];
		Fetch_Ref_window_md2chroma.RefWindowLCU_VP     [i] = Fetch_Ref_window.RefWindowLCU_VP[i];
		Fetch_Ref_window_md2chroma.RefWindowLCU_UP_ref1[i] = Fetch_Ref_window.RefWindowLCU_UP_ref1[i];
		Fetch_Ref_window_md2chroma.RefWindowLCU_VP_ref1[i] = Fetch_Ref_window.RefWindowLCU_VP_ref1[i];
	}
#endif
#if 0 // Duplicated for CHROMA by FETCH
	for (int i = 0; i < RANGE_NUM; i++) {
		for (int j = 0; j < DIR_DIM; j++) {
			for (int k = 0; k < MAP_DIMS; k++) {
				Fetch_Ref_window_md2chroma.CtrPos     [i][j][k] = Fetch_Ref_window.CtrPos     [i][j][k];
				Fetch_Ref_window_md2chroma.CtrPos_ref1[i][j][k] = Fetch_Ref_window.CtrPos_ref1[i][j][k];
			}
		}
	}
	for (int i = 0; i < REFP_NUM; i++) {
		Fetch_Ref_window_md2chroma.lidx    [i] = Fetch_Ref_window.lidx    [i];
		Fetch_Ref_window_md2chroma.cur_refi[i] = Fetch_Ref_window.cur_refi[i];
	}
	for (int i = 0; i < MAX_NUM_REF_PICS; i++) {
		for (int j = 0; j < REFP_NUM; j++) {
			Fetch_Ref_window_md2chroma.refp_pic_ptr           [i][j] = Fetch_Ref_window.refp_pic_ptr[i][j];
			Fetch_Ref_window_md2chroma.refp_is_library_picture[i][j] = Fetch_Ref_window.refp_is_library_picture[i][j];
		}
	}
#endif

#if 0 // MD 2 CHROMA
	for (int i=0; i<SCU_IN_LCU_NUMS_LEN; i++) {
		for (int j=0; j<MV_D; j++) {
			me_line_map_ptr_luma->co_left_mv[i][j] = me_line_map_ptr->co_left_mv[i][j];
		}
		me_line_map_ptr_luma->co_left_refi[i] = me_line_map_ptr->co_left_refi[i];
	}
#endif
#endif
}

void md_run_qtqt(LCUInput& buffer, Arbitor& ref, Instruction* mem_load, Instruction* mem_pu, Instruction* mem_store, Register& reg,
    FME_OUT_PARA_SET* fme_out_para_set, ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], pel dbk_y[71][71], ENC_CU_DATA* md2_output, ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output)
{
#pragma HLS INLINE off

	STATIC_C LCUConfig   config;
	static   Instruction load [PR_SIZE];
	static   Instruction pu   [PR_SIZE];
	static   Instruction store[PR_SIZE];
	STATIC_C ENC_CU_DATA cu_data;

#pragma HLS ARRAY_PARTITION variable=cu_data.split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.split_mode cyclic factor=7 dim=2   
#pragma HLS ARRAY_PARTITION variable=cu_data.mpm_ext cyclic factor=8 dim=1  
#pragma HLS ARRAY_PARTITION variable=cu_data.refi cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mv cyclic factor=2 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data.mvd cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mvd cyclic factor=2 dim=3      
#pragma HLS ARRAY_PARTITION variable=cu_data.num_nz_coef cyclic factor=3 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data.mpm cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data.ipm cyclic factor=2 dim=1


#if 1	// TOUCH
	U32 zero = (fme_out_para_set->pic_width > 0) ? (U32)0 : (U32)fme_out_para_set->log2_max_cuwh;

	config . md_fw_ptr . pic_width = zero;
    config . md_fw_ptr . pic_height = zero;
    config . md_fw_ptr . slice_type = zero;
    config . md_fw_ptr . slice_qp = zero;
    config . md_fw_ptr . max_dt_size = zero;
    config . md_fw_ptr . affine_enable_flag   = (fme_out_para_set->pic_width > 0) ? (U32)1 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . umve_enable_flag     = (fme_out_para_set->pic_width > 0) ? (U32)2 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . dt_intra_enable_flag = (fme_out_para_set->pic_width > 0) ? (U32)3 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . num_of_hmvp_cand     = (fme_out_para_set->pic_width > 0) ? (U32)4 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . ipf_enable_flag      = (fme_out_para_set->pic_width > 0) ? (U32)5 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . pic_width_in_scu = zero;
    config . md_fw_ptr . pic_height_in_scu = zero;
    config . md_fw_ptr . log2_max_cuwh = zero;
    config . md_fw_ptr . log2_culine = zero;
    config . md_fw_ptr . max_cuwh = zero;
    config . md_fw_ptr . f_scu = zero;
    config . md_fw_ptr . gop_size             = (fme_out_para_set->pic_width > 0) ? (U32)6 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . bit_depth_internal = zero;
    config . md_fw_ptr . max_part_ratio = zero;
    config . md_fw_ptr . min_cu_size = zero;
    config . md_fw_ptr . max_split_times = zero;
    config . md_fw_ptr . min_qt_size = zero;
    config . md_fw_ptr . max_bt_size = zero;
    config . md_fw_ptr . max_eqt_size = zero;
    config . md_fw_ptr . affine_subblock_size_idx             = (fme_out_para_set->pic_width > 0) ? (U32)7 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . ipcm_enable_flag                     = (fme_out_para_set->pic_width > 0) ? (U32)8 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . position_based_transform_enable_flag = (fme_out_para_set->pic_width > 0) ? (U32)9 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . secondary_transform_enable_flag      = (fme_out_para_set->pic_width > 0) ? (U32)10 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . tscpm_enable_flag                    = (fme_out_para_set->pic_width > 0) ? (U32)11 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . amvr_enable_flag                     = (fme_out_para_set->pic_width > 0) ? (U32)12 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . emvr_enable_flag                     = (fme_out_para_set->pic_width > 0) ? (U32)13 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . eipm_enable_flag = zero;
    config . md_fw_ptr . mipf_enable_flag = zero;
    config . md_fw_ptr . interpf_enable_flag = zero;
    config . md_fw_ptr . umve_set_flag = zero;
    config . md_fw_ptr . cur_ptr = zero;
    config . md_fw_ptr . chroma_quant_param_delta_cb = (fme_out_para_set->pic_width > 0) ? (U32)14 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_fw_ptr . chroma_quant_param_delta_cr = (fme_out_para_set->pic_width > 0) ? (U32)15 : (U32)(fme_out_para_set->log2_max_cuwh);
    config . md_input_ptr . pix_x = zero;
    config . md_input_ptr . pix_y = zero;
    config . md_input_ptr . lambda_mv = zero;
    config . md_input_ptr . lambda_y = zero;
    config . md_input_ptr . qp_y = zero;
    config . md_input_ptr . x_last_ctu = zero;
    config . md_input_ptr . y_last_ctu = zero;
#endif

	if (reg.boot() == 1)
	{
		read_m_axi(load, pu, store, mem_load, mem_pu, mem_store);
	}
	read_axis(config, fme_out_para_set, me_line_map_ptr_input->min_scu_x, me_line_map_ptr_input->min_scu_y);

#if 1 // TOUCH
    config . md_fw_ptr . max_dt_size += zero;
    config . md_fw_ptr . affine_enable_flag += zero;
    config . md_fw_ptr . umve_enable_flag += zero;
    config . md_fw_ptr . dt_intra_enable_flag += zero;
    config . md_fw_ptr . num_of_hmvp_cand += zero;
    config . md_fw_ptr . ipf_enable_flag += zero;
    config . md_fw_ptr . gop_size += zero;
    config . md_fw_ptr . affine_subblock_size_idx += zero;
    config . md_fw_ptr . ipcm_enable_flag += zero;
    config . md_fw_ptr . position_based_transform_enable_flag += zero;
    config . md_fw_ptr . secondary_transform_enable_flag += zero;
    config . md_fw_ptr . tscpm_enable_flag += zero;
    config . md_fw_ptr . amvr_enable_flag += zero;
    config . md_fw_ptr . emvr_enable_flag += zero;
    config . md_fw_ptr . chroma_quant_param_delta_cb += zero;
    config . md_fw_ptr . chroma_quant_param_delta_cr += zero;
    config . md_input_ptr . lambda_mv += zero;

	// Bitwidth
	U1 zero_1b = zero & 1;
	config . md_fw_ptr . f_scu += (zero_1b << 31);
	config . md_input_ptr . lambda_mv += (zero_1b << 31);
#endif

	vmod(config, buffer, ref, load, pu, store, cu_data, reg);

	write_mem(config, cu_data, cur_ctu_mi, dbk_y, md2_output, me_line_map_ptr_input, me_line_map_ptr_output);
}
#endif

void md_input_connection(LCUInput& buffer, Arbitor& ref, RMD_OUTPUT& g_rmd_output, ME_MV_DIR& me_mv_dir, ENC_FME_INFO_ALL& fme_mv_info, SKIP_MERGE_INPUT& skip_in, strFetch_ref_window& Fetch_Ref_window_ptr, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], pel fme2mdrefbuf[2560][8][2][16])
{
#pragma HLS INLINE off

	copy(buffer.g_rmd_output, g_rmd_output);
	copy(buffer.me_mv_dir, me_mv_dir);
	copy(buffer.fme_mv_info, fme_mv_info);
	for (int i = 0; i < RANGE_NUM; ++i)
	{
		for (int j = 0; j < DIR_DIM; ++j)
		{
			for (int k = 0; k < MAP_DIMS; ++k)
			{
				buffer.CtrPos[i][j][k] = Fetch_Ref_window_ptr.CtrPos[i][j][k];
				buffer.CtrPos_ref1[i][j][k] = Fetch_Ref_window_ptr.CtrPos_ref1[i][j][k];
			}
		}
	}
	for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i)
	{
		for (int j = 0; j < MAX_CU_SIZE_FETCH; ++j)
		{
			buffer.p_fenc_LCU_Y[i][j] = p_fenc_LCU_Y[i][j];
		}
	}
	copy(buffer.skip_in, skip_in);
	copy_idx(buffer.Fetch_Ref_window_ptr, Fetch_Ref_window_ptr);
	ref = fme2mdrefbuf;
}
