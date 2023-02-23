#include "md1.h"
#include "md_cmod.h"

void copy_param(Load2PU& in_buf, LCUConfig& config, LCUInput& buffer, InstructionPU& insn_pu)
{
	copy(insn_pu.cu_width, insn_pu.cu_height, in_buf.buffer, buffer);
	copy(in_buf.config, config);
	copy(in_buf.insn, insn_pu);
}

#if !ONLY_QT_QT
void load_line_buffer_16x16(InstructionPU& insn_pu, Load2PU& in_buf, Channel& channel, InstructionLoad& insn)
{		
	int x_pos_lcu = insn_pu.x0 & 63;
	int y_pos_lcu = insn_pu.y0 & 63;
	S7 ip_array_size_idx = get_ip_array_size_idx(32, 32);

	STATIC_C IP_ARRAY_SIZE  <32, 32> null_IP_buffer_dst_32x32;
	STATIC_C IP_MAP_SCU_SIZE<32, 32> null_IP_map_scu_dst_32x32;
	STATIC_C MPM_ARRAY_SIZE <32, 32> null_MPM_buffer_dst_32x32;
	STATIC_C IP_ARRAY_SIZE  <32, 32> null_IP_buffer_pre_32x32;
	STATIC_C MPM_ARRAY_SIZE <32, 32> null_MPM_buffer_pre_32x32;
	STATIC_C IP_ARRAY_SIZE  <64, 64> null_IP_buffer_src_64x64;
	STATIC_C IP_MAP_SCU_SIZE<64, 64> null_IP_map_scu_src_64x64;
	STATIC_C MPM_ARRAY_SIZE <64, 64> null_MPM_buffer_src_64x64;

	fetch_nebs_qt(in_buf.up_nebs, in_buf.left_nebs, channel.up_nebs_32, channel.left_nebs_32, channel.nebs_line_hor, channel.nebs_line_ver, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);
	in_buf.me_line_map_16 = channel.me_line_map_16;
	in_buf.me_line_map_16.fetch_me_line_qt(insn.part_num, &channel.me_line_map_ptr_32, insn_pu.cu_width, insn_pu.cu_height);
	fetch_line_buffer_qt(insn.part_num, insn_pu.cu_width, insn_pu.cu_height, \
		&in_buf.IP_buffer_16x16, &in_buf.IP_map_scu_16x16, &in_buf.MPM_buffer_16x16, \
		&channel.IP_buffer_16x16, &channel.MPM_buffer_16x16, \
		&channel.IP_buffer_ptr_32x32, &channel.IP_map_scu_ptr_32x32, &channel.MPM_buffer_ptr_32x32, \
		&null_IP_buffer_dst_32x32, &null_IP_map_scu_dst_32x32, &null_MPM_buffer_dst_32x32, &null_IP_buffer_pre_32x32, \
		&null_MPM_buffer_pre_32x32, &null_IP_buffer_src_64x64, &null_IP_map_scu_src_64x64, &null_MPM_buffer_src_64x64);
}
#endif

void cp_buffer_CtrPos(Load2PU pu_in_buf[PU_NUM], LCUInput& buffer, bool flag_pu[PU_NUM])
{
#pragma HLS INLINE off

#if 1
#pragma HLS ARRAY_PARTITION variable=pu_in_buf 	dim=1 complete
#pragma HLS ARRAY_PARTITION variable=flag_pu	dim=1 complete
#endif
	for (int i = 0; i < RANGE_NUM; ++i) {
		for (int j = 0; j < DIR_DIM; ++j) {
			for (int k = 0; k < MAP_DIMS; ++k) {
				for (int pu_idx=0; pu_idx<PU_NUM; pu_idx++) {
#pragma HLS UNROLL
					if (flag_pu[pu_idx]) {pu_in_buf[pu_idx].buffer.CtrPos[i][j][k] = buffer.CtrPos[i][j][k];}
				}
			}
		}
	}
}

void copy_in_buf_up(/*MapScu& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM])
{
#pragma HLS ARRAY_PARTITION variable=src.up[0].rec complete dim=1
	for(int k = 0; k < 32; k++)
	{
#pragma HLS PIPELINE
		U32 temp_src_up_map_scu = src.up[k].map_scu;
		U32 temp_src_up_map_cu_mode = src.up[k].map_cu_mode;
		U32 temp_src_up_ip_map_scu = src.up[k].ip_map_scu;
		U32 temp_src_up_ipm = src.up[k].ipm;
		for(int ii = 0; ii < PU_NUM; ii++)
		{
			if(flag_pu[ii])
			{
				in_buf[ii].line_buffer.up[k].map_scu = temp_src_up_map_scu;
				in_buf[ii].line_buffer.up[k].map_cu_mode = temp_src_up_map_cu_mode;
				in_buf[ii].line_buffer.up[k].ip_map_scu = temp_src_up_ip_map_scu;
				in_buf[ii].line_buffer.up[k].ipm = temp_src_up_ipm;
			}
		}
#if !SIMPLE_MD
		for (int i = 0; i < REFP_NUM; ++i)
		{
			for (int j = 0; j < MV_D; ++j)
			{
				S14 temp_src_up_mv = src.up[k].mv[i][j];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
					if(flag_pu[ii])
					{
						in_buf[ii].line_buffer.up[k].mv[i][j] = temp_src_up_mv;
					}
				}
			}

			S2 temp_src_up_refi = src.up[k].refi[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.up[k].refi[i] = temp_src_up_refi;
				}
			}
		}
#endif // !SIMPLE_MD
		for (int i = 0; i < MIN_CU_SIZE; ++i)
		{
			pel temp_src_up_rec = src.up[k].rec[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.up[k].rec[i] = temp_src_up_rec;
				}
			}
		}
	}

#if 0
	for (int i = 0; i < REFP_NUM; ++i)
	{
		for (int j = 0; j < MV_D; ++j)
		{
			for(int k = 0; k < 32; k++)
			{
#pragma HLS PIPELINE
				S14 temp_src_up_mv = src.up[k].mv[i][j];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						in_buf[ii].line_buffer.up[k].mv[i][j] = temp_src_up_mv;
					}
				}
			}
		}

		for(int k = 0; k < 32; k++)
		{
#pragma HLS PIPELINE
			S2 temp_src_up_refi = src.up[k].refi[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.up[k].refi[i] = temp_src_up_refi;
				}
			}
		}
	}
	for (int i = 0; i < MIN_CU_SIZE; ++i)
	{
		for(int k = 0; k < 32; k++)
		{
#pragma HLS PIPELINE
			pel temp_src_up_rec = src.up[k].rec[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.up[k].rec[i] = temp_src_up_rec;
				}
			}
		}
	}
#endif
}

void copy_in_buf_left(/*MapScu& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM])
{
#pragma HLS ARRAY_PARTITION variable=src.left[0].rec complete dim=1
	for(int k = 0; k < 32; k++)
	{
#pragma HLS PIPELINE
		U32 temp_src_left_map_scu = src.left[k].map_scu;
		U32 temp_src_left_map_cu_mode = src.left[k].map_cu_mode;
		U32 temp_src_left_ip_map_scu = src.left[k].ip_map_scu;
		U32 temp_src_left_ipm = src.left[k].ipm;
		for(int ii = 0; ii < PU_NUM; ii++)
		{
			if(flag_pu[ii])
			{
				in_buf[ii].line_buffer.left[k].map_scu = temp_src_left_map_scu;
				in_buf[ii].line_buffer.left[k].map_cu_mode = temp_src_left_map_cu_mode;
				in_buf[ii].line_buffer.left[k].ip_map_scu = temp_src_left_ip_map_scu;
				in_buf[ii].line_buffer.left[k].ipm = temp_src_left_ipm;
			}
		}

#if !SIMPLE_MD
		for (int i = 0; i < REFP_NUM; ++i)
		{
			for (int j = 0; j < MV_D; ++j)
			{
				S14 temp_src_left_mv = src.left[k].mv[i][j];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
					if(flag_pu[ii])
					{
						in_buf[ii].line_buffer.left[k].mv[i][j] = temp_src_left_mv;
					}
				}
			}

			S2 temp_src_left_refi = src.left[k].refi[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.left[k].refi[i] = temp_src_left_refi;
				}
			}
		}
#endif // !SIMPLE_MD
		for (int i = 0; i < MIN_CU_SIZE; ++i)
		{
			pel temp_src_left_rec = src.left[k].rec[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.left[k].rec[i] = temp_src_left_rec;
				}
			}
		}
	}

#if 0
	for (int i = 0; i < REFP_NUM; ++i)
	{
		for (int j = 0; j < MV_D; ++j)
		{
			for(int k = 0; k < 32; k++)
			{
#pragma HLS PIPELINE
				S14 temp_src_left_mv = src.left[k].mv[i][j];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						in_buf[ii].line_buffer.left[k].mv[i][j] = temp_src_left_mv;
					}
				}
			}
		}
		for(int k = 0; k < 32; k++)
		{
#pragma HLS PIPELINE
			S2 temp_src_left_refi = src.left[k].refi[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.left[k].refi[i] = temp_src_left_refi;
				}
			}
		}
	}
	for (int i = 0; i < MIN_CU_SIZE; ++i)
	{
		for(int k = 0; k < 32; k++)
		{
#pragma HLS PIPELINE
			pel temp_src_left_rec = src.left[k].rec[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.left[k].rec[i] = temp_src_left_rec;
				}
			}
		}
	}
#endif
}	

void copy_in_buf_topleft(/*MapScu& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM])
{
#pragma HLS ARRAY_PARTITION variable=src.topleft[0].rec complete dim=1
	for(int k = 0; k < 32; k++)
	{
#pragma HLS PIPELINE
		U32 temp_src_topleft_map_scu = src.topleft[k].map_scu;
		U32 temp_src_topleft_map_cu_mode = src.topleft[k].map_cu_mode;
		U32 temp_src_topleft_ip_map_scu = src.topleft[k].ip_map_scu;
		U32 temp_src_topleft_ipm = src.topleft[k].ipm;
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				in_buf[ii].line_buffer.topleft[k].map_scu = temp_src_topleft_map_scu;
				in_buf[ii].line_buffer.topleft[k].map_cu_mode = temp_src_topleft_map_cu_mode;
				in_buf[ii].line_buffer.topleft[k].ip_map_scu = temp_src_topleft_ip_map_scu;
				in_buf[ii].line_buffer.topleft[k].ipm = temp_src_topleft_ipm;
			}
		}

#if !SIMPLE_MD
		for (int i = 0; i < REFP_NUM; ++i)
		{
			for (int j = 0; j < MV_D; ++j)
			{
				S14 temp_src_topleft_mv = src.topleft[k].mv[i][j];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
					if(flag_pu[ii])
					{
						in_buf[ii].line_buffer.topleft[k].mv[i][j] = temp_src_topleft_mv;
					}
				}
			}

			S2 temp_src_topleft_refi = src.topleft[k].refi[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.topleft[k].refi[i] = temp_src_topleft_refi;
				}
			}
		}
#endif // !SIMPLE_MD
		for (int i = 0; i < MIN_CU_SIZE; ++i)
		{
			pel temp_src_topleft_rec = src.topleft[k].rec[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.topleft[k].rec[i] = temp_src_topleft_rec;
				}
			}
		}
	}

#if 0
	for (int i = 0; i < REFP_NUM; ++i)
	{
		for (int j = 0; j < MV_D; ++j)
		{
			for(int k = 0; k < 32; k++)
			{
#pragma HLS PIPELINE
				S14 temp_src_topleft_mv = src.topleft[k].mv[i][j];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						in_buf[ii].line_buffer.topleft[k].mv[i][j] = temp_src_topleft_mv;
					}
				}
			}
		}
		for(int k = 0; k < 32; k++)
		{
#pragma HLS PIPELINE
			S2 temp_src_topleft_refi = src.topleft[k].refi[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.topleft[k].refi[i] = temp_src_topleft_refi;
				}
			}
		}
	}
	for (int i = 0; i < MIN_CU_SIZE; ++i)
	{
		for(int k = 0; k < 32; k++)
		{
#pragma HLS PIPELINE
			pel temp_src_topleft_rec = src.topleft[k].rec[i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					in_buf[ii].line_buffer.topleft[k].rec[i] = temp_src_topleft_rec;
				}
			}
		}
	}
#endif
}

void copy_in_buf_y(Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < 32; ++i)
	{
#pragma HLS PIPELINE
		U8 temp_src_y = src.y[i];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				in_buf[ii].line_buffer.y[i] = temp_src_y;
			}
		}
	}
}

void copy_line_buffer_all(/*LineBuffer& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM])
{
	copy_in_buf_up(in_buf, src, flag_pu);
	copy_in_buf_left(in_buf, src, flag_pu);
	copy_in_buf_topleft(in_buf, src, flag_pu);
	copy_in_buf_y(in_buf, src, flag_pu);
}

void copy_line_buffer_pu(/*LineBuffer& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM])
{
#pragma HLS ARRAY_PARTITION variable=in_buf 	dim=1 complete
#pragma HLS ARRAY_PARTITION variable=flag_pu	dim=1 complete
	copy_line_buffer_all(in_buf, src, flag_pu);
#if 0
	for (int i = 0; i < 32; ++i)
	{
		//copy(dst.up[i], src.up[i], flag_pu);
		//copy(dst.left[i], src.left[i], flag_pu);
		//copy(dst.topleft[i], src.topleft[i], flag_pu);
#pragma HLS PIPELINE
		U8 temp_src_y = src.y[i];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				in_buf[ii].line_buffer.y[i] = temp_src_y;
			}
		}

	}
#endif
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			in_buf[ii].line_buffer.y0 = src.y0;
		}
	}
}

void copy_inst_PU(/*InstructionPU& _Left*/Load2PU pu_in_buf[PU_NUM], const InstructionPU _Right[PU_NUM], bool flag_pu[PU_NUM])
{
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			pu_in_buf[ii].insn.pu_idx = _Right[ii].pu_idx;
			pu_in_buf[ii].insn.x0 = _Right[ii].x0;
			pu_in_buf[ii].insn.y0 = _Right[ii].y0;
			pu_in_buf[ii].insn.cup = _Right[ii].cup;
			pu_in_buf[ii].insn.cu_width_log2 = _Right[ii].cu_width_log2;
			pu_in_buf[ii].insn.cu_height_log2 = _Right[ii].cu_height_log2;
			pu_in_buf[ii].insn.cud = _Right[ii].cud;
			pu_in_buf[ii].insn.qt_depth = _Right[ii].qt_depth;
			pu_in_buf[ii].insn.bet_depth = _Right[ii].bet_depth;
			pu_in_buf[ii].insn.cons_pred_mode = _Right[ii].cons_pred_mode;
			pu_in_buf[ii].insn.cu_width = _Right[ii].cu_width;
			pu_in_buf[ii].insn.cu_height = _Right[ii].cu_height;
			pu_in_buf[ii].insn.tree_status = _Right[ii].tree_status;
			pu_in_buf[ii].insn.split_mode = _Right[ii].split_mode;
			pu_in_buf[ii].insn.tag = _Right[ii].tag;
		}
	}
}

void copy_lcu_config(/*LCUConfig& _Left*/Load2PU pu_in_buf[PU_NUM], const LCUConfig& _Right, bool flag_pu[PU_NUM])
{
	copy_md_fw_ptr(pu_in_buf, _Right.md_fw_ptr, flag_pu);
	copy_luma(pu_in_buf, _Right.md_input_ptr, flag_pu);
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			pu_in_buf[ii].config.min_scu_x = _Right.min_scu_x;
			pu_in_buf[ii].config.min_scu_y = _Right.min_scu_y;
		}
	}
}

void copy_md_fw_ptr(/*_MD_FW& _Left*/Load2PU pu_in_buf[PU_NUM], const _MD_FW& _Right, bool flag_pu[PU_NUM])
{
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			pu_in_buf[ii].config.md_fw_ptr.pic_width = _Right.pic_width;
			pu_in_buf[ii].config.md_fw_ptr.pic_height = _Right.pic_height;
			pu_in_buf[ii].config.md_fw_ptr.slice_type = _Right.slice_type;
			pu_in_buf[ii].config.md_fw_ptr.slice_qp = _Right.slice_qp;
			pu_in_buf[ii].config.md_fw_ptr.max_dt_size = _Right.max_dt_size;
			pu_in_buf[ii].config.md_fw_ptr.affine_enable_flag = _Right.affine_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.umve_enable_flag = _Right.umve_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.dt_intra_enable_flag = _Right.dt_intra_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.num_of_hmvp_cand = _Right.num_of_hmvp_cand;
			pu_in_buf[ii].config.md_fw_ptr.ipf_enable_flag = _Right.ipf_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.pic_width_in_scu = _Right.pic_width_in_scu;
			pu_in_buf[ii].config.md_fw_ptr.pic_height_in_scu = _Right.pic_height_in_scu;
			pu_in_buf[ii].config.md_fw_ptr.log2_max_cuwh = _Right.log2_max_cuwh;
			pu_in_buf[ii].config.md_fw_ptr.log2_culine = _Right.log2_culine;
			pu_in_buf[ii].config.md_fw_ptr.max_cuwh = _Right.max_cuwh;
			pu_in_buf[ii].config.md_fw_ptr.f_scu = _Right.f_scu;
			pu_in_buf[ii].config.md_fw_ptr.gop_size = _Right.gop_size;
#if CTX_CONTROL_LUKE
			pu_in_buf[ii].config.md_fw_ptr.bit_depth_internal = _Right.bit_depth_internal;
			pu_in_buf[ii].config.md_fw_ptr.max_part_ratio = _Right.max_part_ratio;
			pu_in_buf[ii].config.md_fw_ptr.min_cu_size = _Right.min_cu_size;
			pu_in_buf[ii].config.md_fw_ptr.max_split_times = _Right.max_split_times;
			pu_in_buf[ii].config.md_fw_ptr.min_qt_size = _Right.min_qt_size;
			pu_in_buf[ii].config.md_fw_ptr.max_bt_size = _Right.max_bt_size;
			pu_in_buf[ii].config.md_fw_ptr.max_eqt_size = _Right.max_eqt_size;
			pu_in_buf[ii].config.md_fw_ptr.affine_subblock_size_idx = _Right.affine_subblock_size_idx;
			pu_in_buf[ii].config.md_fw_ptr.ipcm_enable_flag = _Right.ipcm_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.position_based_transform_enable_flag = _Right.position_based_transform_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.secondary_transform_enable_flag = _Right.secondary_transform_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.tscpm_enable_flag = _Right.tscpm_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.amvr_enable_flag = _Right.amvr_enable_flag;
			pu_in_buf[ii].config.md_fw_ptr.emvr_enable_flag = _Right.emvr_enable_flag;
#endif
#if EIPM
			pu_in_buf[ii].config.md_fw_ptr.eipm_enable_flag = _Right.eipm_enable_flag;
#endif
#if MIPF
			pu_in_buf[ii].config.md_fw_ptr.mipf_enable_flag = _Right.mipf_enable_flag;
#endif
#if INTERPF
			pu_in_buf[ii].config.md_fw_ptr.interpf_enable_flag = _Right.interpf_enable_flag;
#endif
#if UMVE_ENH
			pu_in_buf[ii].config.md_fw_ptr.umve_set_flag = _Right.umve_set_flag;
#endif
			pu_in_buf[ii].config.md_fw_ptr.cur_ptr = _Right.cur_ptr; // ref pic
#if CUDQP_QP_MAP
			pu_in_buf[ii].config.md_fw_ptr.chroma_quant_param_delta_cb = _Right.chroma_quant_param_delta_cb;
			pu_in_buf[ii].config.md_fw_ptr.chroma_quant_param_delta_cr = _Right.chroma_quant_param_delta_cr;
#endif
		}
	}

	for (int i = 0; i < 17; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
#pragma HLS PIPELINE
			int tmp_Right_refp_ptr = _Right.refp_ptr[i][j];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					pu_in_buf[ii].config.md_fw_ptr.refp_ptr[i][j] = tmp_Right_refp_ptr;
				}
			}
		}
	}

#if ENABLE_BFRAME
	for (int j = 0; j < REFP_NUM; ++j)
	{
#pragma HLS PIPELINE
		U5 tmp_Right_num_refp = _Right.num_refp[j];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				pu_in_buf[ii].config.md_fw_ptr.num_refp[j] = tmp_Right_num_refp;
			}
		}
	}
#else
for(int ii = 0; ii < PU_NUM; ii++)
{
#pragma HLS UNROLL
	if(flag_pu[ii])
	{
		pu_in_buf[ii].config.md_fw_ptr.num_refp = _Right.num_refp;
	}
}
#endif
}

void copy_luma(/*_MD_INPUT& _Left*/Load2PU pu_in_buf[PU_NUM], const _MD_INPUT& _Right, bool flag_pu[PU_NUM])
{
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			pu_in_buf[ii].config.md_input_ptr.pix_x = _Right.pix_x;
			pu_in_buf[ii].config.md_input_ptr.pix_y = _Right.pix_y;
			pu_in_buf[ii].config.md_input_ptr.lambda_mv = _Right.lambda_mv;
			pu_in_buf[ii].config.md_input_ptr.lambda_y = _Right.lambda_y;
			pu_in_buf[ii].config.md_input_ptr.qp_y = _Right.qp_y;
			pu_in_buf[ii].config.md_input_ptr.x_last_ctu = _Right.x_last_ctu;
			pu_in_buf[ii].config.md_input_ptr.y_last_ctu = _Right.y_last_ctu;
			pu_in_buf[ii].config.md_input_ptr.cu_dqp_enable       = _Right.cu_dqp_enable;
			pu_in_buf[ii].config.md_input_ptr.cu_qp_group_pred_qp = _Right.cu_qp_group_pred_qp;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
#pragma HLS PIPELINE
		s64 temp_Right_lambda = _Right.lambda[i];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				pu_in_buf[ii].config.md_input_ptr.lambda[i] = temp_Right_lambda;
			}
		}
	}

	for (int i = 0; i < 2; ++i)
	{
#pragma HLS PIPELINE
		S64 temp_Right_dist_chroma_weight = _Right.dist_chroma_weight[i];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				pu_in_buf[ii].config.md_input_ptr.dist_chroma_weight[i] = temp_Right_dist_chroma_weight;
			}
		}
	}

	for (int i = 0; i < RANGE_NUM; ++i)
	{
		for (int j = 0; j < DIR_DIM; ++j)
		{
			for (int k = 0; k < MAP_DIMS; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_CtrlPos2MD = _Right.CtrPos2MD[i][j][k];
				S14 temp_Right_CtrPos2MD_ref1 = _Right.CtrPos2MD_ref1[i][j][k];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						pu_in_buf[ii].config.md_input_ptr.CtrPos2MD[i][j][k] = temp_Right_CtrlPos2MD;
#if ENABLE_BFRAME
						pu_in_buf[ii].config.md_input_ptr.CtrPos2MD_ref1[i][j][k] = temp_Right_CtrPos2MD_ref1;
#endif // ENABLE_BFRAME
					}
				}
			}
		}
	}

	for (int i = 0; i < 256; ++i) {
#pragma HLS PIPELINE
		int temp_Right_CuQPMap = _Right.CuQPMap[i];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				pu_in_buf[ii].config.md_input_ptr.CuQPMap[i] = temp_Right_CuQPMap;
			}
		}
	}
}

void copy_lcu_input(/*LCUInput& _Left*/Load2PU pu_in_buf[PU_NUM], LCUInput& _Right, bool flag_pu[PU_NUM])
{
	//copy(w, h, _Left.g_rmd_output, _Right.g_rmd_output);
	//copy(w, h, _Left.me_mv_dir, _Right.me_mv_dir);
	//copy(w, h, _Left.fme_mv_info, _Right.fme_mv_info);
	copy_g_rmd_output(pu_in_buf, _Right.g_rmd_output, flag_pu);
#if !SIMPLE_MD
	copy_me_mv_dir(pu_in_buf, _Right.me_mv_dir, flag_pu);
	copy_fme_mv_info(pu_in_buf, _Right.fme_mv_info, flag_pu);
#endif //!SIMPLE_MD
	//_Left.g_rmd_output = _Right.g_rmd_output;
	//_Left.me_mv_dir = _Right.me_mv_dir;
	//_Left.fme_mv_info = _Right.fme_mv_info;
	for (int i = 0; i < RANGE_NUM; ++i)
	{
#pragma HLS UNROLL
		for (int j = 0; j < DIR_DIM; ++j)
		{
#pragma HLS UNROLL
			for (int k = 0; k < MAP_DIMS; ++k)
			{
#pragma HLS UNROLL
				S14 temp_Right_CtrPos = _Right.CtrPos[i][j][k];
				S14 temp_Right_CtrPos_ref1 = _Right.CtrPos_ref1[i][j][k];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						pu_in_buf[ii].buffer.CtrPos[i][j][k] = temp_Right_CtrPos;
	#if ENABLE_BFRAME
						pu_in_buf[ii].buffer.CtrPos_ref1[i][j][k] = temp_Right_CtrPos_ref1;
	#endif // ENABLE_BFRAME
					}
				}
			}
		}
	}
	copy_p_fenc_LCU_Y(pu_in_buf, _Right, flag_pu);
	copy_skip_merge_input(pu_in_buf, _Right.skip_in, flag_pu);
#if !SIMPLE_MD
	copy_idx_Fetch_Ref_window_ptr(pu_in_buf, _Right.Fetch_Ref_window_ptr, flag_pu);
#endif //!SIMPLE_MD
}	

void COPY_RMD_8x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) * (64 / 8); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(8, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(8, 8)
				}
			}
		}
	}
}

void COPY_RMD_16x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) * (64 / 8); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(16, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(16, 8)
				}
			}
		}
	}
}

void COPY_RMD_32x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) * (64 / 8); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(32, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(32, 8)
				}
			}
		}
	}
}

void COPY_RMD_64x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) * (64 / 8); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(64, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(64, 8)
				}
			}
		}
	}
}

void COPY_RMD_8x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) * (64 / 16); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(8, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(8, 16)
				}
			}
		}
	}
}

void COPY_RMD_16x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) * (64 / 16); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(16, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(16, 16)
				}
			}
		}
	}
}

void COPY_RMD_32x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) * (64 / 16); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(32, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(32, 16)
				}
			}
		}
	}
}

void COPY_RMD_64x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) * (64 / 16); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(64, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(64, 16)
				}
			}
		}
	}
}

void COPY_RMD_8x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) * (64 / 32); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(8, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(8, 32)
				}
			}
		}
	}
}

void COPY_RMD_16x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) * (64 / 32); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(16, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(16, 32)
				}
			}
		}
	}
}

void COPY_RMD_32x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) * (64 / 32); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(32, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(32, 32)
				}
			}
		}
	}
}

void COPY_RMD_64x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) * (64 / 32); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(64, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(64, 32)
				}
			}
		}
	}
}

void COPY_RMD_8x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) * (64 / 64); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(8, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(8, 64)
				}
			}
		}
	}
}

void COPY_RMD_16x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) * (64 / 64); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(16, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(16, 64)
				}
			}
		}
	}
}

void COPY_RMD_32x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) * (64 / 64); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(32, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(32, 64)
				}
			}
		}
	}
}

void COPY_RMD_64x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) * (64 / 64); ++i)
	{
		for (int j = 0; j < IPD_RDO_CNT; ++j)
		{
#pragma HLS PIPELINE
			u8 temp_Right_modes;
			COPY_RMD_TEMP_VAR_MACRO(64, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_RMD_MACRO(64, 64)
				}
			}
		}
	}
}
void copy_g_rmd_output(/*RMD_OUTPUT& _Left*/Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM])
{
#if !SIMPLE_MD
	COPY_RMD_8x8(pu_in_buf, _Right, flag_pu);		// ..
	COPY_RMD_16x8(pu_in_buf, _Right, flag_pu);		// ..
	COPY_RMD_32x8(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_64x8(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_8x16(pu_in_buf, _Right, flag_pu);		// ..
	
	COPY_RMD_32x16(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_64x16(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_8x32(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_16x32(pu_in_buf, _Right, flag_pu);		// NO QT
	
	COPY_RMD_64x32(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_8x64(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_16x64(pu_in_buf, _Right, flag_pu);		// NO QT
	COPY_RMD_32x64(pu_in_buf, _Right, flag_pu);		// NO QT
#endif // !SIMPLE_MD
	COPY_RMD_32x32(pu_in_buf, _Right, flag_pu);	
	COPY_RMD_16x16(pu_in_buf, _Right, flag_pu);	
	COPY_RMD_64x64(pu_in_buf, _Right, flag_pu);
}

void COPY_ME_64x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(64, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(64, 64)
				}
			}
		}
	}
}

void COPY_ME_32x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(32, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(32, 32)
				}
			}
		}
	}
}

void COPY_ME_64x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(64, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(64, 32)
				}
			}
		}
	}
}

void COPY_ME_32x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(32, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(32, 64)
				}
			}
		}
	}
}

void COPY_ME_16x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(16, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(16, 16)
				}
			}
		}
	}
}

void COPY_ME_32x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(32, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(32, 16)
				}
			}
		}
	}
}

void COPY_ME_16x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(16, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(16, 32)
				}
			}
		}
	}
}

void COPY_ME_64x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(64, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(64, 16)
				}
			}
		}
	}
}

void COPY_ME_16x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(16, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(16, 64)
				}
			}
		}
	}
}

void COPY_ME_8x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(8, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(8, 8)
				}
			}
		}
	}
}

void COPY_ME_16x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(16, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(16, 8)
				}
			}
		}
	}
}

void COPY_ME_8x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(8, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(8, 16)
				}
			}
		}
	}
}

void COPY_ME_32x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(32, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(32, 8)
				}
			}
		}
	}
}

void COPY_ME_8x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(8, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(8, 32)
				}
			}
		}
	}
}

void COPY_ME_64x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(64, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(64, 8)
				}
			}
		}
	}
}

void COPY_ME_8x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(8, 64)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(8, 64)
				}
			}
		}
	}
}

void COPY_ME_32x4(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 4); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(32, 4)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(32, 4)
				}
			}
		}
	}
}

void COPY_ME_4x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 4) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(4, 32)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(4, 32)
				}
			}
		}
	}
}

void COPY_ME_16x4(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 4); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(16, 4)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(16, 4)
				}
			}
		}
	}
}

void COPY_ME_4x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 4) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(4, 16)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(4, 16)
				}
			}
		}
	}
}

void COPY_ME_8x4(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 4); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(8, 4)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(8, 4)
				}
			}
		}
	}
}

void COPY_ME_4x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 4) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
#pragma HLS PIPELINE
			MV_DIR temp_Right_blockMvDir;
			COPY_ME_TEMP_VAR_MACRO(4, 8)
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					COPY_ME_MACRO(4, 8)
				}
			}
		}
	}
}

void copy_me_mv_dir(/*_ME_MV_DIR_BW& _Left*/Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM])
{
	COPY_ME_64x64(pu_in_buf,  _Right, flag_pu);
	COPY_ME_32x32(pu_in_buf,  _Right, flag_pu);
	COPY_ME_64x32(pu_in_buf,  _Right, flag_pu);
	COPY_ME_32x64(pu_in_buf,  _Right, flag_pu);
	COPY_ME_16x16(pu_in_buf,  _Right, flag_pu);
	COPY_ME_32x16(pu_in_buf,  _Right, flag_pu);
	COPY_ME_16x32(pu_in_buf,  _Right, flag_pu);
	COPY_ME_64x16(pu_in_buf,  _Right, flag_pu);
	COPY_ME_16x64(pu_in_buf,  _Right, flag_pu);
	COPY_ME_8x8(pu_in_buf,  _Right, flag_pu);
	COPY_ME_16x8(pu_in_buf,  _Right, flag_pu);
	COPY_ME_8x16(pu_in_buf,  _Right, flag_pu);
	COPY_ME_32x8(pu_in_buf,  _Right, flag_pu);
	COPY_ME_8x32(pu_in_buf,  _Right, flag_pu);
	COPY_ME_64x8(pu_in_buf,  _Right, flag_pu);
	COPY_ME_8x64(pu_in_buf,  _Right, flag_pu);
	COPY_ME_32x4(pu_in_buf,  _Right, flag_pu);
	COPY_ME_4x32(pu_in_buf,  _Right, flag_pu);
	COPY_ME_16x4(pu_in_buf,  _Right, flag_pu);
	COPY_ME_4x16(pu_in_buf,  _Right, flag_pu);
	COPY_ME_8x4(pu_in_buf,  _Right, flag_pu);
	COPY_ME_4x8(pu_in_buf,  _Right, flag_pu);
}

void COPY_FME_64x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(64, 64)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(64, 64)
					}
				}
			}
		}
	}
}

void COPY_FME_32x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(32, 32)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(32, 32)
					}
				}
			}
		}
	}
}

void COPY_FME_64x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(64, 32)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(64, 32)
					}
				}
			}
		}
	}
}

void COPY_FME_32x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(32, 64)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(32, 64)
					}
				}
			}
		}
	}
}

void COPY_FME_16x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(16, 16)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(16, 16)
					}
				}
			}
		}
	}
}

void COPY_FME_32x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(32, 16)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(32, 16)
					}
				}
			}
		}
	}
}

void COPY_FME_16x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(16, 32)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(16, 32)
					}
				}
			}
		}
	}
}

void COPY_FME_64x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(64, 16)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(64, 16)
					}
				}
			}
		}
	}
}

void COPY_FME_16x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(16, 64)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(16, 64)
					}
				}
			}
		}
	}
}

void COPY_FME_8x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(8, 8)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(8, 8)
					}
				}
			}
		}
	}
}

void COPY_FME_16x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(16, 8)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(16, 8)
					}
				}
			}
		}
	}
}

void COPY_FME_8x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(8, 16)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(8, 16)
					}
				}
			}
		}
	}
}

void COPY_FME_32x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(32, 8)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(32, 8)
					}
				}
			}
		}
	}
}

void COPY_FME_8x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(8, 32)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(8, 32)
					}
				}
			}
		}
	}
}

void COPY_FME_64x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 64) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(64, 8)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(64, 8)
					}
				}
			}
		}
	}
}

void COPY_FME_8x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 64); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(8, 64)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(8, 64)
					}
				}
			}
		}
	}
}

void COPY_FME_4x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 4) ; ++i)
	{
		for (int j = 0; j < (64 / 8); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(4, 8)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(4, 8)
					}
				}
			}
		}
	}
}

void COPY_FME_8x4(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 8) ; ++i)
	{
		for (int j = 0; j < (64 / 4); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(8, 4)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(8, 4)
					}
				}
			}
		}
	}
}

void COPY_FME_16x4(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 16) ; ++i)
	{
		for (int j = 0; j < (64 / 4); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(16, 4)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(16, 4)
					}
				}
			}
		}
	}
}

void COPY_FME_4x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 4) ; ++i)
	{
		for (int j = 0; j < (64 / 16); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(4, 16)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(4, 16)
					}
				}
			}
		}
	}
}

void COPY_FME_32x4(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 32) ; ++i)
	{
		for (int j = 0; j < (64 / 4); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(32, 4)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(32, 4)
					}
				}
			}
		}
	}
}

void COPY_FME_4x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < (64 / 4) ; ++i)
	{
		for (int j = 0; j < (64 / 32); ++j)
		{
			for(int k = 0; k < 2; ++k)
			{
#pragma HLS PIPELINE
				S14 temp_Right_sadTreeFmv;
				COPY_FME_TEMP_VAR_MACRO(4, 32)
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						COPY_FME_MACRO(4, 32)
					}
				}
			}
		}
	}
}

void copy_fme_mv_info(/*_ENC_FME_INFO_ALL& _Left*/Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM])
{
	COPY_FME_64x64(pu_in_buf,  _Right, flag_pu);
	COPY_FME_32x32(pu_in_buf,  _Right, flag_pu);
	COPY_FME_64x32(pu_in_buf,  _Right, flag_pu);
	COPY_FME_32x64(pu_in_buf,  _Right, flag_pu);
	COPY_FME_16x16(pu_in_buf,  _Right, flag_pu);
	COPY_FME_32x16(pu_in_buf,  _Right, flag_pu);
	COPY_FME_16x32(pu_in_buf,  _Right, flag_pu);
	COPY_FME_64x16(pu_in_buf,  _Right, flag_pu);
	COPY_FME_16x64(pu_in_buf,  _Right, flag_pu);
	COPY_FME_8x8(pu_in_buf,  _Right, flag_pu);
	COPY_FME_16x8(pu_in_buf,  _Right, flag_pu);
	COPY_FME_8x16(pu_in_buf,  _Right, flag_pu);
	COPY_FME_32x8(pu_in_buf,  _Right, flag_pu);
	COPY_FME_8x32(pu_in_buf,  _Right, flag_pu);
	COPY_FME_64x8(pu_in_buf,  _Right, flag_pu);
	COPY_FME_8x64(pu_in_buf,  _Right, flag_pu);
	COPY_FME_4x8(pu_in_buf,  _Right, flag_pu);
	COPY_FME_8x4(pu_in_buf,  _Right, flag_pu);
	COPY_FME_16x4(pu_in_buf,  _Right, flag_pu);
	COPY_FME_4x16(pu_in_buf,  _Right, flag_pu);
	COPY_FME_32x4(pu_in_buf,  _Right, flag_pu);
	COPY_FME_4x32(pu_in_buf,  _Right, flag_pu);
}

void copy_p_fenc_LCU_Y(/*_ENC_FME_INFO_ALL& _Left*/Load2PU pu_in_buf[PU_NUM], const LCUInput& _Right, bool flag_pu[PU_NUM])
{
#pragma HLS ARRAY_PARTITION variable=_Right.p_fenc_LCU_Y 	dim=1 complete 
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[0].buffer.p_fenc_LCU_Y 	dim=1 complete 

	for (int j = 0; j < MAX_CU_SIZE_FETCH; ++j)
	{
#pragma HLS PIPELINE
		for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i)
		{
			pel temp_Right_p_fenc_LCU_Y = _Right.p_fenc_LCU_Y[i][j];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					pu_in_buf[ii].buffer.p_fenc_LCU_Y[i][j] = temp_Right_p_fenc_LCU_Y;
				}
			}
		}
	}
}

void copy_skip_merge_input(/*_SKIP_MERGE_INPUT& _Left*/Load2PU pu_in_buf[PU_NUM], _SKIP_MERGE_INPUT& _Right, bool flag_pu[PU_NUM])
{
	cp_skip_in(/*&_Left*/pu_in_buf, &_Right, flag_pu);
}

void cp_skip_in(/*_SKIP_MERGE_INPUT * dst*/Load2PU pu_in_buf[PU_NUM], _SKIP_MERGE_INPUT * src, bool flag_pu[PU_NUM])
{
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			pu_in_buf[ii].buffer.skip_in.slice_type       = src->slice_type;
			pu_in_buf[ii].buffer.skip_in.ptr              = src->ptr;
			pu_in_buf[ii].buffer.skip_in.umve_enable_flag = src->umve_enable_flag;
			pu_in_buf[ii].buffer.skip_in.pix_x            = src->pix_x;
		}
	}
	
#if !SIMPLE_MD
	for (int i=0; i<MAX_CU_IN_SCU; i++) {
		for (int j=0; j<MAX_CU_IN_SCU; j++) {
#pragma HLS PIPELINE
			for (int k=0; k<MV_D; k++) {
				s16 temp_src_refp_map_mv = src->refp_map_mv[i][j][k];
				for(int ii = 0; ii < PU_NUM; ii++)
				{
#pragma HLS UNROLL
					if(flag_pu[ii])
					{
						pu_in_buf[ii].buffer.skip_in.refp_map_mv[i][j][k] = temp_src_refp_map_mv;
					}
				}
			}
#pragma HLS PIPELINE
			S2 temp_src_refp_map_refi = src->refp_map_refi[i][j];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					pu_in_buf[ii].buffer.skip_in.refp_map_refi[i][j] = temp_src_refp_map_refi;
				}
			}
		}
	}
	for (int m=0; m<REFP_NUM; m++) {
		for (int i=0; i<MAX_CU_IN_SCU; i++) {
			for (int j=0; j<MAX_CU_IN_SCU; j++) {
#pragma HLS PIPELINE
				for (int k=0; k<MV_D; k++) {
					s32 temp_src_refp_map_mv1 = src->refp_map_mv1[m][i][j][k];
					for(int ii = 0; ii < PU_NUM; ii++)
					{
#pragma HLS UNROLL
						if(flag_pu[ii])
						{
							pu_in_buf[ii].buffer.skip_in.refp_map_mv1[m][i][j][k] = temp_src_refp_map_mv1;
						}
					}
				}
				for (int k=0; k<REFP_NUM; k++) {
#pragma HLS PIPELINE
					S32 temp_src_refp_map_refi1 = src->refp_map_refi1[m][i][j][k];
					for(int ii = 0; ii < PU_NUM; ii++)
					{
#pragma HLS UNROLL
						if(flag_pu[ii])
						{
							pu_in_buf[ii].buffer.skip_in.refp_map_refi1[m][i][j][k] = temp_src_refp_map_refi1;
						}
					}
				}
			}
		}
		for (int i=0; i<MAX_NUM_REF_PICS; i++) {
#pragma HLS PIPELINE
			int temp_src_refp_list_ptr1 = src->refp_list_ptr1[m][i];
			int temp_src_refp_ptr1 = src->refp_ptr1     [i][m];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					pu_in_buf[ii].buffer.skip_in.refp_list_ptr1[m][i] = temp_src_refp_list_ptr1;
					pu_in_buf[ii].buffer.skip_in.refp_ptr1     [i][m] = temp_src_refp_ptr1;
				}
			}
		}
	}
#endif //!SIMPLE_MD
}

void copy_idx_Fetch_Ref_window_ptr(/*__strFetch_ref_window& _Left*/Load2PU pu_in_buf[PU_NUM], const  __strFetch_ref_window& _Right, bool flag_pu[PU_NUM])
{
	for (int i = 0; i < REFP_NUM; ++i)
	{
#pragma HLS PIPELINE
		U1 temp_Right_lidx = _Right.lidx[i];
		S8 temp_Right_cur_refi = _Right.cur_refi[i];
		for(int ii = 0; ii < PU_NUM; ii++)
		{
#pragma HLS UNROLL
			if(flag_pu[ii])
			{
				pu_in_buf[ii].buffer.Fetch_Ref_window_ptr.lidx[i] = temp_Right_lidx;
				pu_in_buf[ii].buffer.Fetch_Ref_window_ptr.cur_refi[i] = temp_Right_cur_refi;
			}
		}
	}
	for (int i = 0; i < REFP_NUM; ++i)
	{
		for (int j = 0; j < MAX_NUM_REF_PICS; ++j) {
#pragma HLS PIPELINE
			int temp_Right_refp_pic_ptr = _Right.refp_pic_ptr[j][i];
			for(int ii = 0; ii < PU_NUM; ii++)
			{
#pragma HLS UNROLL
				if(flag_pu[ii])
				{
					pu_in_buf[ii].buffer.Fetch_Ref_window_ptr.refp_pic_ptr[j][i] = temp_Right_refp_pic_ptr;
				}
			}
		}
	}
	for(int ii = 0; ii < PU_NUM; ii++)
	{
#pragma HLS UNROLL
		if(flag_pu[ii])
		{
			pu_in_buf[ii].buffer.Fetch_Ref_window_ptr.code_stru = _Right.code_stru;
		}
	}
}
void load_1_n(InstructionLoad& insn, Channel& channel, Load2PU pu_in_buf[PU_NUM], LCUConfig& config, LCUInput& buffer, InstructionPU insn_pu[PU_NUM])
{
#pragma HLS INLINE off
#pragma HLS ARRAY_PARTITION variable=pu_in_buf 	dim=1 complete
#pragma HLS ARRAY_PARTITION variable=insn_pu	dim=1 complete
#if 0
	for (int n = 0; n < insn.pu_num; ++n) {
#pragma HLS LOOP_TRIPCOUNT max=32
		Load2PU& in_buf = pu_in_buf[insn_pu[n].pu_idx];

		copy_param(in_buf, config, buffer, insn_pu[n]);

		copy(in_buf.line_buffer, channel.line_buffer);
	}
#else
	bool flag_pu[PU_NUM];
#pragma HLS ARRAY_PARTITION variable=flag_pu 		dim=1 complete
	InstructionPU insn_pu_local[PU_NUM];
#pragma HLS ARRAY_PARTITION variable=insn_pu_local 	dim=1 complete
	for (int i=0; i<PU_NUM; i++) {
#pragma HLS UNROLL
		flag_pu[i] = 0;
	}

	for (int n=0; n<insn.pu_num; n++) {
#pragma HLS LOOP_TRIPCOUNT max=9
#pragma HLS PIPELINE
		U8 pu_idx_temp = insn_pu[n].pu_idx;
		if (pu_idx_temp < PU_NUM) {
			flag_pu[pu_idx_temp] = 1;
			copy(insn_pu_local[pu_idx_temp], insn_pu[n]);
		}
	}
#if 0
	for (int i=0; i<PU_NUM; i++) {
#pragma HLS LOOP_TRIPCOUNT max=4
#pragma HLS LOOP_TRIPCOUNT min=1
#pragma HLS LOOP_TRIPCOUNT avg=4
#pragma HLS UNROLL
		if (flag_pu[i]) {
			copy_param(pu_in_buf[i], config, buffer, insn_pu_local[i]);

			copy(pu_in_buf[i].line_buffer, channel.line_buffer);
		}
	}
#else
	copy_lcu_input(pu_in_buf, buffer, flag_pu);
	copy_lcu_config(pu_in_buf, config, flag_pu);
	copy_inst_PU(pu_in_buf, insn_pu_local, flag_pu);
	copy_line_buffer_pu(pu_in_buf, channel.line_buffer, flag_pu);
#endif
#endif

}

#if !ONLY_QT_QT
void load_line_buffer_16x64_64x16(InstructionPU& insn_pu, Load2PU& in_buf, Channel& channel, InstructionLoad& insn)
{
#pragma HLS INLINE off

	int x_pos_lcu = insn_pu.x0 & 63;
	int y_pos_lcu = insn_pu.y0 & 63;

	//TODO: 暂时改为通过宽高来判断，后面要按照原来的代码，根据ns_64x64和split_mode_father判断
	if (insn_pu.cu_width == 16 && insn_pu.cu_height == 64)
	{
		in_buf.IP_buffer_16x64.fetch_bi_ver(insn.part_num, &channel.IP_buffer_ptr_32x64);
		in_buf.MPM_buffer_16x64.fetch_bi_ver(insn.part_num, &channel.MPM_buffer_ptr_32x64);
		in_buf.IP_map_scu_16x64.fetch_bi_ver(insn.part_num, &channel.IP_map_scu_ptr_32x64);
		fetch_nebs_bi_ver(in_buf.up_nebs_64, in_buf.left_nebs_64, channel.up_nebs_64, channel.left_nebs_64, channel.nebs_line_ver_64, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);
		in_buf.me_line_map_64.fetch_bi_ver(insn.part_num, &channel.me_line_map_ptr_64, insn_pu.cu_width, insn_pu.cu_height);
	}
	else if (insn_pu.cu_width == 64 && insn_pu.cu_height == 16)
	{
		in_buf.IP_buffer_64x16.fetch_bi_hor(insn.part_num, &channel.IP_buffer_ptr_64x32);
		in_buf.MPM_buffer_64x16.fetch_bi_hor(insn.part_num, &channel.MPM_buffer_ptr_64x32);
		in_buf.IP_map_scu_64x16.fetch_bi_hor(insn.part_num, &channel.IP_map_scu_ptr_64x32);
		fetch_nebs_bi_hor(in_buf.up_nebs_64, in_buf.left_nebs_64, channel.up_nebs_64, channel.left_nebs_64, channel.nebs_line_hor_64, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);
		in_buf.me_line_map_64.fetch_bi_hor(insn.part_num, &channel.me_line_map_ptr_64, insn_pu.cu_width, insn_pu.cu_height);
	}
	else if (insn_pu.cu_width == 64 && insn_pu.cu_height == 64)
	{
		for (int i = 0; i < (insn_pu.cu_width >> 2) + 2; i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=18
			cpy_1d_2_1d<S2, REFP_NUM>(channel.up_nebs_64[i].refi, in_buf.up_nebs_64[i].refi);
			cpy_2d_2_2d<S14, REFP_NUM, MV_D>(channel.up_nebs_64[i].mv, in_buf.up_nebs_64[i].mv);
		}
		for (int i = 0; i < (insn_pu.cu_height >> 2); i++)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			cpy_1d_2_1d<S2, REFP_NUM>(channel.left_nebs_64[i].refi, in_buf.left_nebs_64[i].refi);
			cpy_2d_2_2d<S14, REFP_NUM, MV_D>(channel.left_nebs_64[i].mv, in_buf.left_nebs_64[i].mv);
		}

		cpy_1d_2_1d<U32, 18>(channel.me_line_map_ptr_64.map_scu_up_line, in_buf.me_line_map_64.map_scu_up_line);
		cpy_1d_2_1d<U32, 18>(channel.me_line_map_ptr_64.map_cu_mode_up_line, in_buf.me_line_map_64.map_cu_mode_up_line);
		cpy_1d_2_1d<U32, 16>(channel.me_line_map_ptr_64.map_scu_left_line, in_buf.me_line_map_64.map_scu_left_line);
		cpy_1d_2_1d<U32, 16>(channel.me_line_map_ptr_64.map_cu_mode_left_line, in_buf.me_line_map_64.map_cu_mode_left_line);
		in_buf.me_line_map_64.min_scu_x      = channel.me_line_map_ptr_64.min_scu_x;
		in_buf.me_line_map_64.min_scu_y      = channel.me_line_map_ptr_64.min_scu_y;
		in_buf.me_line_map_64.above_line_idx = channel.me_line_map_ptr_64.above_line_idx;
		in_buf.me_line_map_64.curr_ctu_idx   = channel.me_line_map_ptr_64.curr_ctu_idx;
	}
}

void load_line_buffer_16x16_md2(InstructionPU& insn_pu, Load2PU& in_buf, Channel& channel, InstructionLoad& insn, LCUConfig& config, LCUInput& buffer)
{		
	in_buf.IP_buffer_16x16  = channel.IP_buffer_16x16;
	in_buf.MPM_buffer_16x16 = channel.MPM_buffer_16x16;
	in_buf.IP_map_scu_16x16 = channel.IP_map_scu_16x16;
	in_buf.me_line_map_16   = channel.me_line_map_16;

    load_md2(in_buf.ip_list, &buffer.g_rmd_output, insn_pu.cu_width_log2, insn_pu.cu_height_log2, insn_pu.x0, insn_pu.y0, channel.split_mode_father, in_buf.IP_buffer_16x16,
		insn.part_num, &channel.IP_buffer_ptr_16x32, in_buf.IP_map_scu_16x16, &channel.IP_map_scu_ptr_16x32,
		in_buf.MPM_buffer_16x16, &channel.MPM_buffer_ptr_16x32, in_buf.up_nebs, in_buf.left_nebs,
		channel.up_nebs_32, channel.left_nebs_32, channel.nebs_line_hor, insn_pu.cu_width, insn_pu.cu_height,
		in_buf.me_line_map_16, &channel.me_line_map_ptr_32, &channel.IP_buffer_ptr_32x16, &channel.IP_map_scu_ptr_32x16,
		&channel.MPM_buffer_ptr_32x16, channel.nebs_line_ver, &buffer.me_mv_dir, buffer.p_fenc_LCU_Y, &buffer.skip_in,
		&buffer.fme_mv_info, &buffer.Fetch_Ref_window_ptr, &config.md_fw_ptr, in_buf.me_mv_dir_16, in_buf.org_y_16, in_buf.fme_mv_info_16,
		in_buf.Fetch_Ref_window_16, in_buf.skip_in_16);
	in_buf.boundary = channel.boundary;
	in_buf.part_num = insn.part_num;
	in_buf.split_mode_father = channel.split_mode_father;
}
#endif

#if MD_CHANNEL_DEBUG
void load_channel(Load2BpChn& bp_chn_in_buf, Channel& input, InstructionLoad& insn)
{
	copy(bp_chn_in_buf.line_buffer, input.line_buffer);
	if (insn.w == 32 && insn.h == 32)
	{
		bp_chn_in_buf.w = 32;
		bp_chn_in_buf.h = 32;
		//memcpy(bp_in_buf.up_nebs_32, input.up_nebs_32, sizeof(bp_in_buf.up_nebs_32));
		//memcpy(bp_in_buf.left_nebs_32, input.left_nebs_32, sizeof(bp_in_buf.left_nebs_32));
		for (size_t i = 0; i < 10; i++)
		{
			bp_chn_in_buf.up_nebs_32[i] = input.up_nebs_32[i];
		}
		for (size_t i = 0; i < 8; i++)
		{
			bp_chn_in_buf.left_nebs_32[i] = input.left_nebs_32[i];
		}
		bp_chn_in_buf.me_line_map_ptr_32   = input.me_line_map_ptr_32;
		bp_chn_in_buf.IP_buffer_ptr_32x32  = input.IP_buffer_ptr_32x32;
		bp_chn_in_buf.IP_map_scu_ptr_32x32 = input.IP_map_scu_ptr_32x32;
		bp_chn_in_buf.MPM_buffer_ptr_32x32 = input.MPM_buffer_ptr_32x32;
	}
	else if (insn.w == 64 && insn.h == 64)
	{
		bp_chn_in_buf.w = 64;
		bp_chn_in_buf.h = 64;

		bp_chn_in_buf.IP_buffer_ptr_64x64  = input.IP_buffer_ptr_64x64;
		bp_chn_in_buf.IP_map_scu_ptr_64x64 = input.IP_map_scu_ptr_64x64;
		bp_chn_in_buf.MPM_buffer_ptr_64x64 = input.MPM_buffer_ptr_64x64;
		for (size_t i = 0; i < 18; i++)
		{
			bp_chn_in_buf.up_nebs_64[i] = input.up_nebs_64[i];
		}

		bp_chn_in_buf.me_line_map_ptr_64 = input.me_line_map_ptr_64;
		for (size_t i = 0; i < 16; i++)
		{
			bp_chn_in_buf.left_nebs_64[i] = input.left_nebs_64[i];
		}
		for (size_t i = 0; i < 18; i++)
		{
			bp_chn_in_buf.up_nebs_32x32[i] = input.up_nebs_32x32[i];
		}
		for (size_t i = 0; i < 16; i++)
		{
			bp_chn_in_buf.left_nebs_32x32[i] = input.left_nebs_32x32[i];
		}

		bp_chn_in_buf.IP_map_scu_ptr_64x64 = input.IP_map_scu_ptr_64x64;
		//bp_chn_in_buf.prev_cu_data_ptr_32x32 = input.prev_cu_data_ptr_32x32;
		bp_chn_in_buf.prev_cu_data_ptr_32x32.copy(input.prev_cu_data_ptr_32x32);

		for (size_t i = 0; i < 10; i++)
		{
			bp_chn_in_buf.up_nebs_32[i] = input.up_nebs_32[i];
		}
		for (size_t i = 0; i < 8; i++)
		{
			bp_chn_in_buf.left_nebs_32[i] = input.left_nebs_32[i];
		}
		bp_chn_in_buf.me_line_map_ptr_32   = input.me_line_map_ptr_32;
		bp_chn_in_buf.IP_buffer_ptr_32x32  = input.IP_buffer_ptr_32x32;
		bp_chn_in_buf.IP_map_scu_ptr_32x32 = input.IP_map_scu_ptr_32x32;
		bp_chn_in_buf.MPM_buffer_ptr_32x32 = input.MPM_buffer_ptr_32x32;
		for (int i = 0; i < 8; ++i)
		{
			bp_chn_in_buf.nebs_line_ver_32x32[i] = input.nebs_line_ver_32x32[i];
		}
		for (int i = 0; i < 16; ++i)
		{
			bp_chn_in_buf.nebs_line_hor_32x32[i] = input.nebs_line_hor_32x32[i];
		}
	}
	else
	{
		assert(false);
	}
}

void load_1_out_chn(InstructionLoad& insn, Load2OutChn& out_chn_in_buf, ShareMemory& pool, CuDataPool& cu_data)
{
	if (insn.version == 2)
	{
		out_chn_in_buf.w = insn.w;
		out_chn_in_buf.h = insn.h;

		copy(out_chn_in_buf.line_buffer, pool.channel[insn.pool_idx].line_buffer);
		read_memory(insn.x0 & 63, insn.y0 & 63, insn.w, insn.h, out_chn_in_buf.prev_cu_data_ptr, cu_data, pool.channel[insn.pool_idx].cu_data_ptr);

		for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; ++i)
		{
#pragma HLS LOOP_TRIPCOUNT max=16
			for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j)
			{
#pragma HLS LOOP_TRIPCOUNT max=16
				out_chn_in_buf.cost_best[i][j] = pool.channel[insn.pool_idx].cu_data_ptr.get_cost();
			}
		}
	}
	else
	{
#if !ONLY_QT_QT
		if (insn.w == 32 && insn.h == 32)
		{
			out_chn_in_buf.w = 32;
			out_chn_in_buf.h = 32;
			out_chn_in_buf.IP_map_scu_ptr_32x32 = pool.channel[insn.pool_idx].IP_map_scu_ptr_32x32;
			out_chn_in_buf.prev_cu_data_ptr = pool.channel[insn.pool_idx].prev_cu_data_ptr;
			for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; ++i)
			{
				for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j)
				{
					out_chn_in_buf.cost_best[i][j] = pool.channel[insn.pool_idx].cost_best[i][j];
				}
			}
			out_chn_in_buf.me_line_map_ptr_32   = pool.channel[insn.pool_idx].me_line_map_ptr_32;
			out_chn_in_buf.IP_buffer_ptr_32x32  = pool.channel[insn.pool_idx].IP_buffer_ptr_32x32;
			out_chn_in_buf.IP_map_scu_ptr_32x32 = pool.channel[insn.pool_idx].IP_map_scu_ptr_32x32;
			out_chn_in_buf.MPM_buffer_ptr_32x32 = pool.channel[insn.pool_idx].MPM_buffer_ptr_32x32;
			for (int i = 0; i < 8; ++i)
			{
				out_chn_in_buf.nebs_line_ver_32x32[i] = pool.channel[insn.pool_idx].nebs_line_ver_32x32[i];
			}
			for (int i = 0; i < 16; ++i)
			{
				out_chn_in_buf.nebs_line_hor_32x32[i] = pool.channel[insn.pool_idx].nebs_line_hor_32x32[i];
			}
		}
		else
		{
			assert(false);
		}
#else
		assert(false);
#endif
	}
}
#endif
#if !ONLY_QT_QT
void load_1_n_16x16(InstructionLoad& insn, Channel& channel, Load2PU pu_in_buf[PU_NUM], LCUConfig& config, LCUInput& buffer, InstructionPU insn_pu[PU_NUM])
{
#pragma HLS INLINE off
	for (int n = 0; n < insn.pu_num; ++n)
	{
#pragma HLS LOOP_TRIPCOUNT max=32
		Load2PU& in_buf = pu_in_buf[insn_pu[n].pu_idx];
		copy_param(in_buf, config, buffer, insn_pu[n]);

		load_line_buffer_16x16(insn_pu[n], in_buf, channel, insn);
	}
}

void load_1_n_64x64_to_16x16(InstructionLoad& insn, ShareMemory& pool, LCUConfig& config, LCUInput& buffer, Load2PU pu_in_buf[PU_NUM], InstructionPU insn_pu[PU_NUM])
{
#pragma HLS INLINE off

	Channel& channel = pool.channel[insn.pool_idx];

	InstructionPU insn_pu_tmp;
	insn_pu_tmp.cu_width = 32;
	insn_pu_tmp.cu_height = 32;

	InstructionLoad insn_load;
	insn_load.part_num = insn.part_num;

	STATIC_C Load2PU in_buf;

	load_64x64_to_32x32(insn_pu_tmp, insn_load,
		in_buf, channel,
		channel.up_nebs_32x32, channel.left_nebs_32x32,
		channel.up_nebs,
		channel.left_nebs, config, buffer);

	STATIC_C Channel chn = channel;
	chn.IP_buffer_ptr_32x32 = in_buf.IP_buffer_32x32;
	chn.IP_map_scu_ptr_32x32 = in_buf.IP_map_scu_32x32;
	for (size_t i = 0; i < 8; i++)
	{
		chn.left_nebs_32[i] = in_buf.left_nebs_32[i];
	}
	chn.me_line_map_ptr_32 = in_buf.me_line_map_32;
	chn.MPM_buffer_ptr_32x32 = in_buf.MPM_buffer_32x32;
	for (size_t i = 0; i < 10; i++)
	{
		chn.up_nebs_32[i] = in_buf.up_nebs_32[i];
	}
	channel.IP_buffer_ptr_32x32 = in_buf.IP_buffer_32x32;
	channel.IP_map_scu_ptr_32x32 = in_buf.IP_map_scu_32x32;
	for (size_t i = 0; i < 8; i++)
	{
		channel.left_nebs_32[i] = in_buf.left_nebs_32[i];
	}
	channel.me_line_map_ptr_32 = in_buf.me_line_map_32;
	channel.MPM_buffer_ptr_32x32 = in_buf.MPM_buffer_32x32;
	for (size_t i = 0; i < 10; i++)
	{
		channel.up_nebs_32[i] = in_buf.up_nebs_32[i];
	}

	insn_load = insn;
	insn_load.part_num = 0;
	load_1_n_16x16(insn_load, chn, pu_in_buf, config, buffer, insn_pu);
}

void load_1_n_16x64_64x16(InstructionLoad& insn, ShareMemory& pool, Load2PU pu_in_buf[PU_NUM], LCUConfig& config, LCUInput& buffer, InstructionPU insn_pu[PU_NUM])
{
#pragma HLS INLINE off
	for (int n = 0; n < insn.pu_num; ++n)
	{
#pragma HLS LOOP_TRIPCOUNT max=32
		Load2PU& in_buf = pu_in_buf[insn_pu[n].pu_idx];

		copy_param(in_buf, config, buffer, insn_pu[n]);

		load_line_buffer_16x64_64x16(insn_pu[n], in_buf, pool.channel[insn.pool_idx], insn);
	}
}

void load_64x64_to_32x32(InstructionPU& insn_pu, InstructionLoad& insn,
	Load2PU& in_buf, Channel& channel,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO  up_nebs[1 + 16 + 1],
	ENC_ME_NEB_INFO  left_nebs[16],
	 LCUConfig & config, LCUInput & buffer)
{
	STATIC_C IP_ARRAY_SIZE  <16, 16> null_IP_buffer_dst_16x16;
	STATIC_C IP_MAP_SCU_SIZE<16, 16> null_IP_map_scu_dst_16x16;
	STATIC_C MPM_ARRAY_SIZE <16, 16> null_MPM_buffer_dst_16x16;
	STATIC_C IP_ARRAY_SIZE  <16, 16> null_IP_buffer_pre_16x16;
	STATIC_C MPM_ARRAY_SIZE <16, 16> null_MPM_buffer_pre_16x16;
	STATIC_C IP_ARRAY_SIZE  <32, 32> null_IP_buffer_src_32x32;
	STATIC_C IP_MAP_SCU_SIZE<32, 32> null_IP_map_scu_src_32x32;
	STATIC_C MPM_ARRAY_SIZE <32, 32> null_MPM_buffer_src_32x32;

	in_buf.IP_buffer_32x32  = channel.IP_buffer_ptr_32x32;
	in_buf.IP_map_scu_32x32 = channel.IP_map_scu_ptr_32x32;
	in_buf.MPM_buffer_32x32 = channel.MPM_buffer_ptr_32x32;
	in_buf.me_line_map_32   = channel.me_line_map_ptr_32;

	fetch_nebs_qt(up_nebs, left_nebs, up_nebs_father, left_nebs_father, channel.nebs_line_hor_32x32, channel.nebs_line_ver_32x32, insn_pu.cu_width, insn_pu.cu_height, insn.part_num);
	in_buf.me_line_map_32.fetch_me_line_qt(insn.part_num, &channel.me_line_map_ptr_64, insn_pu.cu_width, insn_pu.cu_height);
	fetch_line_buffer_qt(insn.part_num, 32, 32, \
		& null_IP_buffer_dst_16x16, &null_IP_map_scu_dst_16x16, &null_MPM_buffer_dst_16x16, \
		& null_IP_buffer_pre_16x16, &null_MPM_buffer_pre_16x16, &null_IP_buffer_src_32x32, \
		& null_IP_map_scu_src_32x32, &null_MPM_buffer_src_32x32, \
		& in_buf.IP_buffer_32x32, &in_buf.IP_map_scu_32x32, &in_buf.MPM_buffer_32x32, \
		& in_buf.IP_buffer_32x32, &in_buf.MPM_buffer_32x32, \
		& channel.IP_buffer_ptr_64x64, &channel.IP_map_scu_ptr_64x64, &channel.MPM_buffer_ptr_64x64);

	copy(in_buf.config, config);
	copy(insn_pu.cu_width, insn_pu.cu_height, in_buf.buffer, buffer);
	for (int i = 0; i < 10; ++i)
	{
		in_buf.up_nebs_32[i] = up_nebs[i];
	}
	for (int i = 0; i < 8; ++i)
	{
		in_buf.left_nebs_32[i] = left_nebs[i];
	}
}
#endif


void read_memory_2(ENC_CU_DATA& data, CuDataPool& mem, int mem_offset[16])
{
	const int x =  0;
	const int y =  0;
	const int w_unit = 4;
	const int h_unit = 4;
	const int w = 64;
	const int h = 64;

	LOOP_READ_MEMORY_1: for (int i = 0; i < (h >> 2); ++i) {
#pragma HLS PIPELINE
		for (int j = 0; j < (w >> 2); ++j) {
			for (int m = 0; m < MAX_CU_DEPTH; ++m) {
				for (int n = 0; n < NUM_BLOCK_SHAPE; ++n) {
					data.split_mode[m][n][i * (w >> 2) + j] = 0;
				}
			}
		}
	}

	LOOP_READ_MEMORY_2: for (int i_unit = 0; i_unit < h_unit; ++i_unit) {
		for (int j_unit = 0; j_unit < w_unit; ++j_unit) {
			int offset = mem_offset[i_unit*w_unit+j_unit];
			for (int i = 0; i < (h >> 2); ++i) {
#pragma HLS PIPELINE
				for (int j = 0; j < (w >> 2); ++j) {
					for (int m = 0; m < MAX_CU_DEPTH; ++m) {
						for (int n = 0; n < NUM_BLOCK_SHAPE; ++n) {
							data.split_mode[m][n][i * (w >> 2) + j] += mem[offset].split_mode[m][n][(i + (y >> 2)) * 16 + j + (x >> 2)];
						}
					}
				}
			}
		}
	}
}

void read_memory_3(ENC_CU_DATA& data, CuDataPool& mem, int mem_offset[16])
{
	const int w_unit = 4;
	const int h_unit = 4;
	const int w = 64;
	const int h = 64;

	LOOP_READ_MEMORY_3: for (int i_unit = 0; i_unit < h_unit; ++i_unit) {
		for (int j_unit = 0; j_unit < w_unit; ++j_unit) {
			int offset = mem_offset[i_unit*w_unit+j_unit];
			for (int i = 0; i < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++i) {
#pragma HLS PIPELINE
				for (int j = 0; j < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++j) {
					int i_dst = i_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + i;
					int j_dst = j_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int srcp  = i      * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int dstp  = i_dst  * (w >> MIN_CU_LOG2) + j_dst;

					for (int k = 0; k < REFP_NUM; ++k) {
						for (int l = 0; l < MV_D; ++l) {
							data.mv [dstp][k][l] = mem[offset].mv [srcp][k][l];
							data.mvd[dstp][k][l] = mem[offset].mvd[srcp][k][l];
						}
						data.refi[dstp][k] = mem[offset].refi[srcp][k];
					}
					data.tb_part           [dstp] = mem[offset].tb_part[srcp];
					data.pred_mode         [dstp] = mem[offset].pred_mode[srcp];
					data.mvr_idx           [dstp] = mem[offset].mvr_idx[srcp];
					data.umve_flag         [dstp] = mem[offset].umve_flag[srcp];
					data.umve_idx          [dstp] = mem[offset].umve_idx[srcp];
					data.skip_idx          [dstp] = mem[offset].skip_idx[srcp];
					data.map_scu           [dstp] = mem[offset].map_scu[srcp];
					data.affine_flag       [dstp] = mem[offset].affine_flag[srcp];
					data.smvd_flag         [dstp] = mem[offset].smvd_flag[srcp];
					data.map_cu_mode       [dstp] = mem[offset].map_cu_mode[srcp];
					data.map_pb_tb_part    [dstp] = mem[offset].map_pb_tb_part[srcp];
					data.depth             [dstp] = mem[offset].depth[srcp];
					data.ipf_flag          [dstp] = mem[offset].ipf_flag[srcp];
					data.mvp_from_hmvp_flag[dstp] = mem[offset].mvp_from_hmvp_flag[srcp];
#if INTERPF
					data.inter_filter_flag [dstp] = mem[offset].inter_filter_flag[srcp];
#endif
					for (int k = 0; k < 2; ++k) {
						data.mpm[k][dstp] = mem[offset].mpm[k][srcp];
						data.ipm[k][dstp] = mem[offset].ipm[k][srcp];
					}
					for (int k = 0; k < 8; ++k) {
						data.mpm_ext[k][dstp] = mem[offset].mpm_ext[k][srcp];
					}
					for (int k = 0; k < N_C; ++k) {
						data.num_nz_coef[k][dstp] = mem[offset].num_nz_coef[k][srcp];
					}
#if CUDQP_QP_MAP
					data.cu_luma_qp[dstp] = mem[offset].cu_luma_qp[srcp];
#endif
				}
			}
		}
	}
}

void read_memory_4(ENC_CU_DATA& data, CuDataPool& mem, int mem_offset[16])
{
	const int w_unit = 4;
	const int h_unit = 4;
	const int w = 64;
	const int h = 64;

	LOOP_READ_MEMORY_4: for (int i_unit = 0; i_unit < h_unit; ++i_unit) {
		for (int j_unit = 0; j_unit < w_unit; ++j_unit) {
			int offset = mem_offset[i_unit*w_unit+j_unit];
			for (int i = 0; i < CU_DATA_UNIT_SIZE / 4; ++i) {
				for (int j = 0; j < CU_DATA_UNIT_SIZE / 4; ++j) {
#pragma HLS PIPELINE
					for (int m = 0; m < 4; m++) {
						int i_dst = i_unit * CU_DATA_UNIT_SIZE + 4 * i + m;
						for (int n = 0; n < 4; n++) {
							int j_dst = j_unit * CU_DATA_UNIT_SIZE + 4 * j + n;
							data.coef_y[i_dst][j_dst] = mem[offset].coef_y[4*i+m][4*j+n];
							data.reco_y[i_dst][j_dst] = mem[offset].reco_y[4*i+m][4*j+n];
						}
					}
				}
			}
		}
	}
}

void read_memory_0(int x, int y, int w, int h, ENC_CU_DATA& data, CuDataPool& mem, CuDataPtr& ptr)
{

#pragma HLS ARRAY_PARTITION variable=data.split_mode 		dim=1 complete
#pragma HLS ARRAY_PARTITION variable=data.split_mode 		dim=2 complete
#pragma HLS ARRAY_PARTITION variable=data.split_mode 		dim=3 cyclic factor=16
#pragma HLS ARRAY_PARTITION variable=mem[0].split_mode 		dim=1 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].split_mode 		dim=2 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].split_mode 		dim=3 cyclic factor=16

// to be align width output
#pragma HLS ARRAY_PARTITION variable=data.coef_y			dim=1 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=data.coef_y			dim=2 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=data.reco_y			dim=1 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=data.reco_y			dim=2 cyclic factor=4

// to be align width store
#pragma HLS ARRAY_PARTITION variable=mem[0].mv				dim=2 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].mv				dim=3 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].mvd				dim=2 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].mvd				dim=3 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].refi 			dim=2 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].mpm_ext			dim=1 cyclic factor=8
#pragma HLS ARRAY_PARTITION variable=mem[0].mpm 			dim=1 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].ipm 			dim=1 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].num_nz_coef		dim=1 complete
#pragma HLS ARRAY_PARTITION variable=mem[0].coef_y			dim=1 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=mem[0].coef_y			dim=2 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=mem[0].reco_y			dim=1 cyclic factor=4
#pragma HLS ARRAY_PARTITION variable=mem[0].reco_y			dim=2 cyclic factor=4

	const int x_unit = 0;
	const int y_unit = 0;
	const int w_unit = 4;
	const int h_unit = 4;
	assert(x == 0);
	assert(y == 0);
	assert(w == 64);
	assert(h == 64);

	int mem_offset_2[h_unit*w_unit];
	int mem_offset_3[h_unit*w_unit];
	int mem_offset_4[h_unit*w_unit];

	LOOP_READ_MEMORY_0: for (int i_unit = 0; i_unit < h_unit; ++i_unit) {
		for (int j_unit = 0; j_unit < w_unit; ++j_unit) {
#pragma HLS PIPELINE
			int temp = ptr.idx[i_unit + y_unit][j_unit + x_unit];
			assert(temp >= 0);
			mem_offset_2[i_unit*w_unit + j_unit] = temp;
			mem_offset_3[i_unit*w_unit + j_unit] = temp;
			mem_offset_4[i_unit*w_unit + j_unit] = temp;
		}
	}

	read_memory_2(data, mem, mem_offset_2);

	read_memory_3(data, mem, mem_offset_3);
	
	read_memory_4(data, mem, mem_offset_4);
	
}

void load_output(Channel& channel, CuDataPool& mem, Load2Output& out_in_buf)
{
	copy(out_in_buf.line_buffer, channel.line_buffer);
	read_memory_0(0, 0, 64, 64, out_in_buf.cu_data, mem, channel.cu_data_ptr);
	out_in_buf.cu_data.print(64, 64);
}

void load_1_bypass(InstructionLoad& insn, ShareMemory& pool, Load2Bp bp_in_buf[BP_NUM], LCUConfig& config, InstructionPU insn_pu[PU_NUM])
{
	InstructionPU i_pu = insn_pu[0];
	Load2Bp& in_buf = bp_in_buf[i_pu.pu_idx];
	Channel& chn = pool.channel[insn.pool_idx];
	assert(insn.pu_num == 1 && i_pu.pu_idx == 0);

	copy(in_buf.config, config);
	copy(in_buf.insn, i_pu);
	copy(in_buf.cu_data_ptr, chn.cu_data_ptr);
	copy(in_buf.line_buffer, chn.line_buffer);
	in_buf.version = insn.version;

	for (int i = 0; i < SCU_IN_LCU_NUMS_LEN; ++i)
	{
#pragma HLS PIPELINE
		for (int j = 0; j < SCU_IN_LCU_NUMS_LEN; ++j)
		{
			in_buf.cost_best[i][j] = chn.cost_best[i][j];
		}
	}

#if !ONLY_QT_QT
	if (insn.w == 32 && insn.h == 32)
	{
		in_buf.cu_data_temp_ptr     = chn.prev_cu_data_ptr;
		in_buf.IP_map_scu_ptr_32x32 = chn.IP_map_scu_ptr_32x32;
	}
	else
	{
		assert(false);
	}
#endif
}

void load_data(U8 type, InstructionLoad& insn
#if MD_CHANNEL_DEBUG
    , Load2BpChn& bp_chn_in_buf, Load2OutChn& out_chn_in_buf
#endif
    , Load2PU pu_in_buf[PU_NUM]
#if MD_CHANNEL_DEBUG
    , Channel& input
#endif
    , ShareMemory& pool, LCUConfig& config, LCUInput& buffer, InstructionPU insn_pu[PU_NUM], Load2Bp bp_in_buf[BP_NUM], CuDataPool& cu_data, Load2Output& output)
{
#pragma HLS INLINE off
// to be align with pu16x16
#pragma HLS array_partition variable=pu_in_buf[0].buffer.me_mv_dir.block16x16MvDir complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.me_mv_dir.block16x8MvDir complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.me_mv_dir.block8x16MvDir complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.fme_mv_info.sadTree16x16Fmv complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.fme_mv_info.sadTree16x16Fmv complete dim=3
#pragma HLS array_partition variable=pu_in_buf[0].buffer.fme_mv_info.sadTree16x8Fmv complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.fme_mv_info.sadTree16x8Fmv complete dim=3
#pragma HLS array_partition variable=pu_in_buf[0].buffer.fme_mv_info.sadTree8x16Fmv complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.fme_mv_info.sadTree8x16Fmv complete dim=3
#pragma HLS array_partition variable=pu_in_buf[0].buffer.Fetch_Ref_window_ptr.refp_pic_ptr complete dim=2
#pragma HLS array_partition variable=pu_in_buf[0].buffer.Fetch_Ref_window_ptr.refp_is_library_picture complete dim=2

#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.up[0].mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.up[0].mv cyclic factor=2 dim=2
// #pragma HLS array_partition variable=pu_in_buf[0].line_buffer.up[0].refi cyclic factor=2 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.up[0].rec cyclic factor=4 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.left[0].mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.left[0].mv cyclic factor=2 dim=2
// #pragma HLS array_partition variable=pu_in_buf[0].line_buffer.left[0].refi cyclic factor=2 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.left[0].rec cyclic factor=4 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.topleft[0].mv cyclic factor=2 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.topleft[0].mv cyclic factor=2 dim=2
// #pragma HLS array_partition variable=pu_in_buf[0].line_buffer.topleft[0].refi cyclic factor=2 dim=1
#pragma HLS array_partition variable=pu_in_buf[0].line_buffer.topleft[0].rec cyclic factor=4 dim=1

    switch (type)
    {
#if MD_CHANNEL_DEBUG
    case LOAD_CHANNEL: load_channel(bp_chn_in_buf, input, insn); break;
    case LOAD_1_OUT_CHANNEL: load_1_out_chn(insn, out_chn_in_buf, pool, cu_data); break;
#endif
    case LOAD_1_OUTPUT:                  load_output(pool.channel[insn.pool_idx], cu_data, output); break;
    case LOAD_1_N:                       load_1_n(insn, pool.channel[insn.pool_idx], pu_in_buf, config, buffer, insn_pu); break;
#if !ONLY_QT_QT
    case LOAD_1_N_16x16:                 load_1_n_16x16(insn, pool.channel[insn.pool_idx], pu_in_buf, config, buffer, insn_pu); break;
    case LOAD_1_N_16x16_MD1:             assert(false); break;
    case LOAD_1_N_16x16_MD2:             assert(false); break;
    case LOAD_1_N_32x32_32x16_16x32_MD1: assert(false); break;
    case LOAD_1_N_32x32_32x16_16x32_MD2: assert(false); break;
    case LOAD_1_N_16x64_64x16:           load_1_n_16x64_64x16(insn, pool, pu_in_buf, config, buffer, insn_pu); break;
    case LOAD_1_N_64x64_64x32_32x64:     assert(false); break;
#endif
    case LOAD_1_BYPASS:                  load_1_bypass(insn, pool, bp_in_buf, config, insn_pu); break;
    default:                             assert(false); break;
    }
}

void load_insn_done(InstructionLoad& insn, InstructionPU insn_pu[PU_NUM], stream_type<ap_uint<1> > push_data[PU_NUM])
{
	switch (insn.type)
	{
#if MD_CHANNEL_DEBUG
	case LOAD_1_OUT_CHANNEL:
		push_data[OUTPUT_CHANNEL].write(1);
		break;
#endif
	case LOAD_1_OUTPUT:
		push_data[PU_OUTPUT].write(1);
		break;
	case LOAD_1_N:
#if !ONLY_QT_QT
	case LOAD_1_N_16x16:
	case LOAD_1_N_16x16_MD1:
	case LOAD_1_N_16x16_MD2:
	case LOAD_1_N_32x32_32x16_16x32_MD1:
	case LOAD_1_N_32x32_32x16_16x32_MD2:
	case LOAD_1_N_16x64_64x16:
	case LOAD_1_N_64x64_64x32_32x64:
#endif
	case LOAD_1_BYPASS:
		for (int j = 0; j < PU_NUM; ++j)
		{
#pragma HLS UNROLL
			if(j < insn.pu_num)
			{
				push_data[insn_pu[j].pu_idx].write(1);
			}
		}
		break;
#if MD_CHANNEL_DEBUG
	case LOAD_CHANNEL:
		push_data[BYPASS_CHANNEL].write(1);
		break;
#endif
	default:
		assert(false);
	}
}

void get_insn_status_ready(InstructionRegister<Instruction>& ir1, InstructionRegister<Instruction>& ir2, hls::stream<U32>& status)
{
	U32 csr;
	if (!status.read_nb(csr))
	{
		csr = 0;
	}
	bool status_dup = true;

	for (int i = 0; i < ir1.size && !ir2.full(); ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS PIPELINE
		Instruction insn_tmp = ir1.buf[i];

		if ((insn_tmp.opcode() != DONE && (insn_tmp.tag_chn() == csr || insn_tmp.tag_chn() == 0)) || (insn_tmp.opcode() == DONE && i == 0))
		{
			ir2.push(insn_tmp);
			ir1.remove(i--);
			status_dup = false;
		}
	}
	assert(csr == 0 || status_dup == false);
}

bool get_insn_fifo_not_full(InstructionRegister<Instruction>& ir, stream_type<ap_uint<1> > push_data[PU_NUM], Instruction& insn_out)
{
	//assert(ir.size);
	for (int i = 0; i < ir.size; ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS PIPELINE
		Instruction& insn_tmp = ir.buf[i];

		bool flag = true;
		switch (insn_tmp.opcode())
		{
#if MD_CHANNEL_DEBUG
		case LOAD_1_OUT_CHANNEL:
			if (push_data[OUTPUT_CHANNEL].full())
			{
				flag = false;
			}
			break;
#endif
		case LOAD_1_OUTPUT:
			if (push_data[PU_OUTPUT].full())
			{
				flag = false;
			}
			break;
		case LOAD_1_N:
#if !ONLY_QT_QT
		case LOAD_1_N_16x16:
		case LOAD_1_N_16x16_MD1:
		case LOAD_1_N_16x16_MD2:
		case LOAD_1_N_32x32_32x16_16x32_MD1:
		case LOAD_1_N_32x32_32x16_16x32_MD2:
		case LOAD_1_N_16x64_64x16:
		case LOAD_1_N_64x64_64x32_32x64:
#endif
		case LOAD_1_BYPASS:
			for (int j = 0; j < PU_NUM; ++j)
			{
				if (insn_tmp.flag_pu(j))
				{
					if (push_data[j].full())
					{
						flag = false;
						break;
					}
				}
			}
			break;
#if MD_CHANNEL_DEBUG
		case LOAD_CHANNEL:
			if (push_data[BYPASS_CHANNEL].full())
			{
				flag = false;
			}
			break;
#endif
		case DONE:
			flag = (i == 0);
			break;
		default:
			assert(i >= ir.size);
		}

		if (flag && i < ir.size)
		{
			insn_out = insn_tmp;
			ir.remove(i);
			return true;
		}
	}
	return false;
}

#if !LOAD_CORE_DUMMY
bool load_core(InstructionRegister<Instruction>& ir1, InstructionRegister<Instruction>& ir2, Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], PRMan& pu_man, hls::stream<U32>& status, Register& reg
#if MD_CHANNEL_DEBUG
	, Load2BpChn& bp_chn_in_buf, Load2OutChn& out_chn_in_buf

#endif
	, Load2PU pu_in_buf[PU_NUM]
#if MD_CHANNEL_DEBUG
	, Channel& input
#endif
	, ShareMemory& pool, LCUConfig& config, LCUInput& buffer, stream_type<ap_uint<1> > push_data[PU_NUM], Load2Bp bp_in_buf[BP_NUM], CuDataPool& cu_data, Load2Output& out_in_buf)
{

	if (mem_load[0].opcode() == -1)
	{
		for (int n = 0; n < PU_NUM; ++n)
		{
#pragma HLS UNROLL
			pu_in_buf[n].cost_best = MAX_COST;
		}
	}

	ir1.update(mem_load);

	get_insn_status_ready(ir1, ir2, status);

	Instruction insn;
	if(!get_insn_fifo_not_full(ir2, push_data, insn)) { return false; }

	InstructionLoad insn_load;
	InstructionPU   insn_pu[PU_NUM];
	
	insn_decode(insn, insn_load, reg);
	for (int i = 0; i < insn_load.pu_num; ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=9
		insn_decode(pu_man.get(mem_pu), insn_pu[i], reg);
	}

	if (insn_load.type == DONE) { return true; }

	for (int i = 0; i < 1; ++i)
	{
		load_data(insn_load.type, insn_load
#if MD_CHANNEL_DEBUG
			, bp_chn_in_buf, out_chn_in_buf
#endif
			, pu_in_buf
#if MD_CHANNEL_DEBUG
			, input
#endif
			, pool, config, buffer, insn_pu, bp_in_buf, cu_data, out_in_buf);
	}

	for (int i = 0; i < 1; ++i)
	{
		load_insn_done(insn_load, insn_pu, push_data);
	}

	return false;
}
#endif


 
// #define KERNEL_16X16					22
// #define KERNEL_8X16					23
// #define KERNEL_16X8					24
// #define KERNEL_8X8					25

// LOAD TOP
void load(Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], hls::stream<U32>& status, Register& reg
#if MD_CHANNEL_DEBUG
	, Load2BpChn& bp_chn_in_buf, Load2OutChn& out_chn_in_buf
#endif
	, Load2PU pu_in_buf[PU_NUM]
#if MD_CHANNEL_DEBUG
	, Channel& input
#endif
#if LOAD_AXIS_ADAPTER
	, Load_AXIS axis_load
#endif
	, ShareMemory& pool, LCUConfig& config, LCUInput& buffer, stream_type<ap_uint<1> > push_data[PU_NUM], Load2Bp bp_in_buf[BP_NUM], CuDataPool& cu_data, Load2Output& out_in_buf)
{

// =================================================================================================
// PARTITION OF INTERFACE
// =================================================================================================
#pragma HLS ARRAY_PARTITION variable=pu_in_buf dim=1 complete
#pragma HLS ARRAY_PARTITION variable=push_data dim=1 complete
#pragma HLS ARRAY_PARTITION variable=bp_in_buf dim=1 complete
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.up[0].mv[0][0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.up[0].refi[0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.up[0].rec[0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.left[0].mv[0][0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.left[0].refi[0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.left[0].rec[0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.topleft[0].mv[0][0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.topleft[0].refi[0] core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool.channel[0].line_buffer.topleft[0].rec[0] core=RAM_1P_BRAM
#if 1 // align with vmod, md_run
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

#pragma HLS ARRAY_PARTITION variable=buffer.p_fenc_LCU_Y dim=1 cyclic factor=64 
#endif
#if 1 // align with PU_16x16
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[22].config.md_fw_ptr.refp_ptr dim=0 complete				// AXIS
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[22].buffer.skip_in.refp_ptr1  dim=1 complete
#endif
#if 1 // align with PU_8x16
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[23].config.md_input_ptr.CtrPos2MD 		dim=1 complete	// AXIS
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[23].config.md_input_ptr.CtrPos2MD 		dim=2 complete
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[23].config.md_input_ptr.CtrPos2MD 		dim=3 complete
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1  	dim=1 complete	// AXIS
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1  	dim=2 complete
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1  	dim=3 complete
#endif
#if 1 // align with PU_8x8
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].config.md_fw_ptr.num_refp			dim=1 complete	// AXIS
// #pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_map_mv1		dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_map_mv1		dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_map_mv1		dim=3 complete
// #pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_map_refi1		dim=1 complete
// #pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_map_refi1		dim=2 complete
// #pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_map_refi1		dim=3 complete
#pragma HLS ARRAY_PARTITION variable=pu_in_buf[25].buffer.skip_in.refp_list_ptr1		dim=1 complete
#endif
// =================================================================================================

// =================================================================================================
// AXIS OF INTERFACE
// =================================================================================================
#define _LOAD_IF_AXIS_EN_  1

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[0][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[0][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[1][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[1][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[2][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[2][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[3][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[3][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[4][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[4][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[5][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[5][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[6][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[6][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[7][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[7][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[8][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[8][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[9][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[9][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[10][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[10][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[11][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[11][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[12][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[12][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[13][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[13][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[14][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[14][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[15][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[15][1]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[16][0]
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.refp_ptr[16][1]

#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[0][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[0][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[1][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[1][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[2][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[2][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[3][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[3][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[4][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[4][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[5][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[5][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[6][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[6][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[7][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[7][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[8][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[8][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[9][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[9][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[10][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[10][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[11][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[11][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[12][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[12][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[13][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[13][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[14][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[14][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[15][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[15][1]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[16][0]
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.refp_ptr[16][1]

#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[0][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[0][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[1][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[1][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[2][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[2][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[3][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[3][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[4][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[4][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[5][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[5][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[6][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[6][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[7][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[7][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[8][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[8][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[9][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[9][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[10][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[10][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[11][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[11][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[12][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[12][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[13][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[13][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[14][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[14][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[15][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[15][1]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[16][0]
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.refp_ptr[16][1]

#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[0][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[0][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[1][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[1][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[2][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[2][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[3][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[3][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[4][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[4][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[5][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[5][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[6][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[6][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[7][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[7][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[8][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[8][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[9][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[9][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[10][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[10][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[11][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[11][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[12][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[12][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[13][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[13][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[14][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[14][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[15][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[15][1]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[16][0]
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.refp_ptr[16][1]
#endif

#if 1
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD[1][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_input_ptr.CtrPos2MD_ref1[1][1][1]

#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD[1][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_input_ptr.CtrPos2MD_ref1[1][1][1]

#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD[1][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_input_ptr.CtrPos2MD_ref1[1][1][1]

#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD[1][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[0][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[0][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[0][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[0][1][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[1][0][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[1][0][1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[1][1][0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_input_ptr.CtrPos2MD_ref1[1][1][1]

#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_fw_ptr.num_refp[0]
#pragma HLS INTERFACE axis  port=pu_in_buf[22].config.md_fw_ptr.num_refp[1]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_fw_ptr.num_refp[0]
#pragma HLS INTERFACE axis  port=pu_in_buf[23].config.md_fw_ptr.num_refp[1]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_fw_ptr.num_refp[0]
#pragma HLS INTERFACE axis  port=pu_in_buf[24].config.md_fw_ptr.num_refp[1]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_fw_ptr.num_refp[0]
#pragma HLS INTERFACE axis  port=pu_in_buf[25].config.md_fw_ptr.num_refp[1]
#endif

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.pic_width// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.pic_width 
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.pic_height	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.slice_type	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.slice_qp	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.max_dt_size	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.affine_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.umve_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.dt_intra_enable_flag  // 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.num_of_hmvp_cand	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.ipf_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.pic_width_in_scu	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.pic_height_in_scu	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.log2_max_cuwh	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.log2_culine	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.max_cuwh	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.f_scu	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.gop_size	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.bit_depth_internal	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.max_part_ratio	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.min_cu_size	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.max_split_times	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.min_qt_size	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.max_bt_size	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.max_eqt_size	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.affine_subblock_size_idx	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.ipcm_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.position_based_transform_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.secondary_transform_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.tscpm_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.amvr_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.emvr_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.eipm_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.mipf_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.interpf_enable_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.umve_set_flag	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.cur_ptr	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cb	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cr	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_fw_ptr.chroma_quant_param_delta_cr

#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.pix_x	//
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.pix_y	// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.lambda_mv	// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.lambda_y	// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.qp_y		// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.x_last_ctu		// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.y_last_ctu		// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.do_chroma		// 
// #pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.do_chroma
// #pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.do_chroma
#pragma HLS INTERFACE axis port=pu_in_buf[0].config.md_input_ptr.cu_dqp_enable		// 
#pragma HLS INTERFACE axis port=pu_in_buf[1].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[2].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[3].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[4].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[5].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[6].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[7].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[8].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[9].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.cu_dqp_enable
#pragma HLS INTERFACE axis port=pu_in_buf[28].config.md_input_ptr.cu_dqp_enable
// #pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_input_ptr.cu_qp_group_area_size	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.cu_qp_group_area_size
// #pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.cu_qp_group_area_size
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_input_ptr.cu_qp_group_pred_qp	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.cu_qp_group_pred_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_input_ptr.chroma_quant_param_delta_cb	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_input_ptr.chroma_quant_param_delta_cr	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=pu_in_buf[0].config.md_input_ptr.md_slice_qp	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[10].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[11].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[12].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[13].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[14].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[15].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[16].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[17].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[18].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[19].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[20].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[21].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[22].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[23].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[24].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[25].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[26].config.md_input_ptr.md_slice_qp
// #pragma HLS INTERFACE axis port=pu_in_buf[27].config.md_input_ptr.md_slice_qp
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[0].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[1].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[2].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[3].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[4].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[5].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[6].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[7].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[8].config.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[9].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[10].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[11].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[12].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[13].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[14].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[15].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[16].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[17].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[18].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[19].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[20].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[21].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[22].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[23].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[24].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[25].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[26].config.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[27].config.min_scu_y
#endif 

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=pu_in_buf[0].buffer.skip_in.slice_type //
#pragma HLS INTERFACE axis  port=pu_in_buf[1].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[2].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[3].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[4].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[5].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[6].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[7].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[8].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[9].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[10].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[11].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[12].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[13].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[14].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[15].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[16].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[17].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[18].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[19].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[20].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[21].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[22].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[23].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[24].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[25].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[26].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=pu_in_buf[27].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=pu_in_buf[0].buffer.skip_in.ptr //
#pragma HLS INTERFACE axis  port=pu_in_buf[1].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[2].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[3].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[4].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[5].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[6].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[7].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[8].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[9].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[10].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[11].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[12].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[13].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[14].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[15].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[16].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[17].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[18].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[19].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[20].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[21].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[22].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[23].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[24].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[25].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[26].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=pu_in_buf[27].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=pu_in_buf[0].buffer.skip_in.umve_enable_flag //
#pragma HLS INTERFACE axis  port=pu_in_buf[1].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[2].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[3].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[4].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[5].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[6].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[7].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[8].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[9].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[10].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[11].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[12].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[13].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[14].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[15].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[16].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[17].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[18].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[19].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[20].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[21].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[22].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[23].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[24].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[25].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[26].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=pu_in_buf[27].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=pu_in_buf[0].buffer.skip_in.pix_x //
#pragma HLS INTERFACE axis  port=pu_in_buf[1].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[2].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[3].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[4].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[5].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[6].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[7].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[8].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[9].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[10].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[11].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[12].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[13].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[14].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[15].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[16].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[17].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[18].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[19].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[20].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[21].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[22].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[23].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[24].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[25].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[26].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=pu_in_buf[27].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=pu_in_buf[0].buffer.Fetch_Ref_window_ptr.code_stru //
#pragma HLS INTERFACE axis  port=pu_in_buf[1].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[2].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[3].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[4].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[5].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[6].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[7].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[8].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis  port=pu_in_buf[9].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[10].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[11].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[12].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[13].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[14].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[15].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[16].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[17].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[18].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[19].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[20].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[21].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[22].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[23].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[24].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[25].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[26].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[27].buffer.Fetch_Ref_window_ptr.code_stru
#pragma HLS INTERFACE axis port=pu_in_buf[28].buffer.Fetch_Ref_window_ptr.code_stru
#endif 

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=pu_in_buf[0].insn	//
#pragma HLS INTERFACE axis  port=pu_in_buf[1].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[2].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[3].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[4].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[5].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[6].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[7].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[8].insn
#pragma HLS INTERFACE axis  port=pu_in_buf[9].insn
#pragma HLS INTERFACE axis port=pu_in_buf[10].insn
#pragma HLS INTERFACE axis port=pu_in_buf[11].insn
#pragma HLS INTERFACE axis port=pu_in_buf[12].insn
#pragma HLS INTERFACE axis port=pu_in_buf[13].insn
#pragma HLS INTERFACE axis port=pu_in_buf[14].insn
#pragma HLS INTERFACE axis port=pu_in_buf[15].insn
#pragma HLS INTERFACE axis port=pu_in_buf[16].insn
#pragma HLS INTERFACE axis port=pu_in_buf[17].insn
#pragma HLS INTERFACE axis port=pu_in_buf[18].insn
#pragma HLS INTERFACE axis port=pu_in_buf[19].insn
#pragma HLS INTERFACE axis port=pu_in_buf[20].insn
#pragma HLS INTERFACE axis port=pu_in_buf[21].insn
#pragma HLS INTERFACE axis port=pu_in_buf[22].insn
#pragma HLS INTERFACE axis port=pu_in_buf[23].insn
#pragma HLS INTERFACE axis port=pu_in_buf[24].insn
#pragma HLS INTERFACE axis port=pu_in_buf[25].insn
#pragma HLS INTERFACE axis port=pu_in_buf[26].insn
#pragma HLS INTERFACE axis port=pu_in_buf[27].insn
#endif 

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=pu_in_buf[0].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[1].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[2].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[3].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[4].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[5].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[6].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[7].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[8].line_buffer.y0
#pragma HLS INTERFACE axis  port=pu_in_buf[9].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[10].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[11].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[12].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[13].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[14].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[15].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[16].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[17].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[18].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[19].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[20].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[21].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[22].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[23].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[24].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[25].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[26].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_in_buf[27].line_buffer.y0
#endif

#if 0
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_16.min_scu_x		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_16.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_16.min_scu_y		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_16.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_16.above_line_idx		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_16.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_16.curr_ctu_idx		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_16.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_16.curr_ctu_idx
#endif 

#if 0
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_64.min_scu_x		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_64.min_scu_x
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_64.min_scu_y		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_64.min_scu_y
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_64.above_line_idx		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_64.above_line_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[0].me_line_map_64.curr_ctu_idx		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[2].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[3].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[4].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[5].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[6].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[7].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[8].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis  port=pu_in_buf[9].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[10].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[11].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[12].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[13].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[14].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[15].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[16].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[17].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[18].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[19].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[20].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[21].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[22].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[23].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[24].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[25].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[26].me_line_map_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=pu_in_buf[27].me_line_map_64.curr_ctu_idx
#endif

#if 0
#pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_buffer_16x16.top_left		//
#pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[10].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[11].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[12].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[13].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[14].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[15].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[16].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[17].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[18].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[19].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[20].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[21].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[22].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[23].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[24].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[25].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[26].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[27].IP_buffer_16x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_buffer_16x64.top_left		//
#pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[10].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[11].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[12].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[13].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[14].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[15].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[16].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[17].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[18].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[19].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[20].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[21].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[22].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[23].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[24].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[25].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[26].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[27].IP_buffer_16x64.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_buffer_64x16.top_left		//
#pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[10].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[11].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[12].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[13].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[14].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[15].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[16].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[17].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[18].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[19].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[20].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[21].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[22].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[23].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[24].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[25].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[26].IP_buffer_64x16.top_left
#pragma HLS INTERFACE axis port=pu_in_buf[27].IP_buffer_64x16.top_left
#endif 

#if 0
#pragma HLS INTERFACE axis  port=pu_in_buf[0].MPM_buffer_16x16.up	//
#pragma HLS INTERFACE axis  port=pu_in_buf[1].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[2].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[3].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[4].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[5].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[6].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[7].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[8].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[9].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[10].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[11].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[12].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[13].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[14].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[15].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[16].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[17].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[18].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[19].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[20].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[21].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[22].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[23].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[24].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[25].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[26].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[27].MPM_buffer_16x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[0].MPM_buffer_16x16.left	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[2].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[3].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[4].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[5].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[6].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[7].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[8].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[9].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[10].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[11].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[12].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[13].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[14].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[15].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[16].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[17].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[18].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[19].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[20].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[21].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[22].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[23].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[24].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[25].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[26].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[27].MPM_buffer_16x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].MPM_buffer_16x64.up		// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[2].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[3].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[4].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[5].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[6].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[7].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[8].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[9].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[10].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[11].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[12].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[13].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[14].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[15].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[16].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[17].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[18].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[19].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[20].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[21].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[22].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[23].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[24].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[25].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[26].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis port=pu_in_buf[27].MPM_buffer_16x64.up
#pragma HLS INTERFACE axis  port=pu_in_buf[0].MPM_buffer_16x64.left	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[2].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[3].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[4].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[5].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[6].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[7].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[8].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[9].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[10].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[11].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[12].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[13].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[14].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[15].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[16].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[17].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[18].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[19].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[20].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[21].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[22].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[23].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[24].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[25].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[26].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis port=pu_in_buf[27].MPM_buffer_16x64.left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].MPM_buffer_64x16.up	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[2].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[3].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[4].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[5].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[6].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[7].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[8].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[9].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[10].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[11].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[12].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[13].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[14].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[15].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[16].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[17].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[18].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[19].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[20].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[21].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[22].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[23].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[24].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[25].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[26].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis port=pu_in_buf[27].MPM_buffer_64x16.up
#pragma HLS INTERFACE axis  port=pu_in_buf[0].MPM_buffer_64x16.left	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[2].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[3].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[4].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[5].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[6].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[7].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[8].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis  port=pu_in_buf[9].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[10].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[11].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[12].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[13].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[14].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[15].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[16].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[17].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[18].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[19].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[20].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[21].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[22].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[23].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[24].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[25].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[26].MPM_buffer_64x16.left
#pragma HLS INTERFACE axis port=pu_in_buf[27].MPM_buffer_64x16.left
#endif 

#if 0
// #pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_map_scu_16x16.map_scu_top_left	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[10].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[11].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[12].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[13].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[14].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[15].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[16].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[17].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[18].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[19].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[20].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[21].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[22].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[23].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[24].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[25].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[26].IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[27].IP_map_scu_16x16.map_scu_top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_map_scu_16x16.map_scu_top_left_new	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[10].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[11].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[12].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[13].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[14].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[15].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[16].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[17].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[18].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[19].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[20].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[21].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[22].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[23].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[24].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[25].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[26].IP_map_scu_16x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[27].IP_map_scu_16x16.map_scu_top_left_new

// #pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_map_scu_16x64.map_scu_top_left	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[10].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[11].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[12].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[13].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[14].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[15].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[16].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[17].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[18].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[19].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[20].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[21].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[22].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[23].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[24].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[25].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[26].IP_map_scu_16x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[27].IP_map_scu_16x64.map_scu_top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_map_scu_16x64.map_scu_top_left_new	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[10].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[11].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[12].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[13].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[14].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[15].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[16].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[17].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[18].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[19].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[20].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[21].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[22].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[23].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[24].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[25].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[26].IP_map_scu_16x64.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[27].IP_map_scu_16x64.map_scu_top_left_new

// #pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_map_scu_64x16.map_scu_top_left	// 
// #pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[10].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[11].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[12].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[13].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[14].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[15].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[16].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[17].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[18].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[19].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[20].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[21].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[22].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[23].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[24].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[25].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[26].IP_map_scu_64x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=pu_in_buf[27].IP_map_scu_64x16.map_scu_top_left
#pragma HLS INTERFACE axis  port=pu_in_buf[0].IP_map_scu_64x16.map_scu_top_left_new	// 
#pragma HLS INTERFACE axis  port=pu_in_buf[1].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[2].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[3].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[4].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[5].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[6].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[7].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[8].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis  port=pu_in_buf[9].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[10].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[11].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[12].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[13].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[14].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[15].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[16].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[17].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[18].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[19].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[20].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[21].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[22].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[23].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[24].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[25].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[26].IP_map_scu_64x16.map_scu_top_left_new
#pragma HLS INTERFACE axis port=pu_in_buf[27].IP_map_scu_64x16.map_scu_top_left_new
#endif // -----------------------------------

#if 0 // MD_CHANNEL_DEBUG 
// --- bp_chn_in_buf -------
#pragma HLS INTERFACE axis port=bp_chn_in_buf.w
#pragma HLS INTERFACE axis port=bp_chn_in_buf.h
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_64.min_scu_x
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_64.min_scu_y
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_64.above_line_idx
#pragma HLS INTERFACE axis port=bp_chn_in_buf.me_line_map_ptr_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_buffer_ptr_32x32.top_left
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_buffer_ptr_32x64.top_left
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_buffer_ptr_64x32.top_left
#pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_buffer_ptr_64x64.top_left
#pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_32x32.left
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_32x64.up
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_32x64.left
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_64x32.up
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_64x32.left
#pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_64x64.up
#pragma HLS INTERFACE axis port=bp_chn_in_buf.MPM_buffer_ptr_64x64.left
#pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_32x32.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_32x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_32x64.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_64x32.map_scu_top_left
// #pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_64x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_64x64.map_scu_top_left
#pragma HLS INTERFACE axis port=bp_chn_in_buf.IP_map_scu_ptr_64x64.map_scu_top_left_new
#endif 

#if 0 // MD_CHANNEL_DEBUG
// --- out_chn_in_buf -------
#pragma HLS INTERFACE axis port=out_chn_in_buf.w
#pragma HLS INTERFACE axis port=out_chn_in_buf.h
//#pragma HLS INTERFACE axis port=out_chn_in_buf.cost_best
#pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_32x32.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_32x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_32x64.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_64x32.map_scu_top_left
// #pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_64x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_64x64.map_scu_top_left
#pragma HLS INTERFACE axis port=out_chn_in_buf.IP_map_scu_ptr_64x64.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_cu_data_ptr.x_chroma
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_cu_data_ptr.y_chroma
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_cu_data_ptr.x_last
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_cu_data_ptr.y_last
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_cu_data_ptr.last_child
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_cu_data_ptr.cu_qp_group_pred_qp
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_up_cu_data_temp_array_ptr.x_chroma
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_up_cu_data_temp_array_ptr.y_chroma
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_up_cu_data_temp_array_ptr.x_last
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_up_cu_data_temp_array_ptr.y_last
// #pragma HLS INTERFACE axis port=out_chn_in_buf.prev_up_cu_data_temp_array_ptr.last_child
#pragma HLS INTERFACE axis port=out_chn_in_buf.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=out_chn_in_buf.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=out_chn_in_buf.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=out_chn_in_buf.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=out_chn_in_buf.IP_buffer_ptr_32x32.top_left
#pragma HLS INTERFACE axis port=out_chn_in_buf.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=out_chn_in_buf.MPM_buffer_ptr_32x32.left
#endif 

#if 0 // MD_CHANNEL_DEBUG
// --- input -------
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_64.min_scu_x
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_64.min_scu_y
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_64.above_line_idx
#pragma HLS INTERFACE axis port=input.me_line_map_ptr_64.curr_ctu_idx
// #pragma HLS INTERFACE axis port=input.prev_cu_data_ptr.x_chroma
// #pragma HLS INTERFACE axis port=input.prev_cu_data_ptr.y_chroma
// #pragma HLS INTERFACE axis port=input.prev_cu_data_ptr.x_last
// #pragma HLS INTERFACE axis port=input.prev_cu_data_ptr.y_last
// #pragma HLS INTERFACE axis port=input.prev_cu_data_ptr.last_child
// #pragma HLS INTERFACE axis port=input.prev_cu_data_ptr.cu_qp_group_pred_qp
// #pragma HLS INTERFACE axis port=input.prev_up_cu_data_temp_array_ptr.x_chroma
// #pragma HLS INTERFACE axis port=input.prev_up_cu_data_temp_array_ptr.y_chroma
// #pragma HLS INTERFACE axis port=input.prev_up_cu_data_temp_array_ptr.x_last
// #pragma HLS INTERFACE axis port=input.prev_up_cu_data_temp_array_ptr.y_last
// #pragma HLS INTERFACE axis port=input.prev_up_cu_data_temp_array_ptr.last_child
#pragma HLS INTERFACE axis port=input.IP_buffer_ptr_32x32.top_left
// #pragma HLS INTERFACE axis port=input.IP_buffer_ptr_32x64.top_left
// #pragma HLS INTERFACE axis port=input.IP_buffer_ptr_64x32.top_left
#pragma HLS INTERFACE axis port=input.IP_buffer_ptr_64x64.top_left
#pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_32x32.left
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_32x64.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_32x64.left
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_64x32.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_64x32.left
#pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_64x64.up
#pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_64x64.left
#pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_32x32.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_32x64.map_scu_top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_32x64.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_64x32.map_scu_top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_64x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_64x64.map_scu_top_left
#pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_64x64.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.IP_buffer_ptr_16x32.top_left
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_16x32.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_16x32.left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_16x32.map_scu_top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_16x32.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.IP_buffer_ptr_32x16.top_left
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_32x16.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_ptr_32x16.left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_32x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_ptr_32x16.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.MPM_buffer_16x16.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_16x16.left
// #pragma HLS INTERFACE axis port=input.MPM_buffer_16x64.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_16x64.left
// #pragma HLS INTERFACE axis port=input.MPM_buffer_64x16.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_64x16.left
// #pragma HLS INTERFACE axis port=input.IP_buffer_16x16.top_left
// #pragma HLS INTERFACE axis port=input.IP_buffer_16x64.top_left
// #pragma HLS INTERFACE axis port=input.IP_buffer_64x16.top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_16x16.map_scu_top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_16x16.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.me_line_map_16.min_scu_x
// #pragma HLS INTERFACE axis port=input.me_line_map_16.min_scu_y
// #pragma HLS INTERFACE axis port=input.me_line_map_16.above_line_idx
// #pragma HLS INTERFACE axis port=input.me_line_map_16.curr_ctu_idx
// #pragma HLS INTERFACE axis port=input.MPM_buffer_32x32.up
// #pragma HLS INTERFACE axis port=input.MPM_buffer_32x32.left
// #pragma HLS INTERFACE axis port=input.IP_buffer_32x32.top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_32x32.map_scu_top_left
// #pragma HLS INTERFACE axis port=input.IP_map_scu_32x32.map_scu_top_left_new
// #pragma HLS INTERFACE axis port=input.me_line_map_32.min_scu_x
// #pragma HLS INTERFACE axis port=input.me_line_map_32.min_scu_y
// #pragma HLS INTERFACE axis port=input.me_line_map_32.above_line_idx
// #pragma HLS INTERFACE axis port=input.me_line_map_32.curr_ctu_idx
// #pragma HLS INTERFACE axis port=input.me_line_map_64.min_scu_x
// #pragma HLS INTERFACE axis port=input.me_line_map_64.min_scu_y
// #pragma HLS INTERFACE axis port=input.me_line_map_64.above_line_idx
// #pragma HLS INTERFACE axis port=input.me_line_map_64.curr_ctu_idx
// #pragma HLS INTERFACE axis port=input.split_mode_father
// #pragma HLS INTERFACE axis port=input.boundary
// #pragma HLS INTERFACE axis port=input.NS_luma_pred_mode
// #pragma HLS INTERFACE axis port=input.best_split_mode
// #pragma HLS INTERFACE axis port=input.best_cons_pred_mode
#endif // ------------------

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=bp_in_buf[0].insn //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].insn
#pragma HLS INTERFACE axis  port=bp_in_buf[2].insn
#pragma HLS INTERFACE axis  port=bp_in_buf[3].insn
#pragma HLS INTERFACE axis  port=bp_in_buf[4].insn
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].insn
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].insn
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].insn
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].insn
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].insn
// #pragma HLS INTERFACE axis port=bp_in_buf[10].insn
// #pragma HLS INTERFACE axis port=bp_in_buf[11].insn
// #pragma HLS INTERFACE axis port=bp_in_buf[12].insn
// #pragma HLS INTERFACE axis port=bp_in_buf[13].insn
// #pragma HLS INTERFACE axis port=bp_in_buf[14].insn
// #pragma HLS INTERFACE axis port=bp_in_buf[15].insn

#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.pic_width //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.pic_width
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.pic_height //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.pic_height
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.slice_type //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.slice_type
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.slice_qp //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.slice_qp
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.max_dt_size //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.max_dt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.affine_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.affine_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.umve_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.umve_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.dt_intra_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.dt_intra_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.num_of_hmvp_cand //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.num_of_hmvp_cand
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.ipf_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.ipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.pic_width_in_scu //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.pic_width_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.pic_height_in_scu //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.pic_height_in_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.log2_max_cuwh //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.log2_max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.log2_culine //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.log2_culine
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.max_cuwh //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.max_cuwh
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.f_scu //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.f_scu
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.gop_size //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.gop_size
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.bit_depth_internal //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.bit_depth_internal
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.max_part_ratio //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.max_part_ratio
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.min_cu_size //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.min_cu_size
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.max_split_times //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.max_split_times
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.min_qt_size //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.min_qt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.max_bt_size //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.max_bt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.max_eqt_size //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.max_eqt_size
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.affine_subblock_size_idx //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.affine_subblock_size_idx
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.ipcm_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.ipcm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.position_based_transform_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.position_based_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.secondary_transform_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.secondary_transform_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.tscpm_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.tscpm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.amvr_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.amvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.emvr_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.emvr_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.eipm_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.eipm_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.mipf_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.mipf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.interpf_enable_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.interpf_enable_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.umve_set_flag //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.umve_set_flag
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.cur_ptr //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.cur_ptr
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cb //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cb
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cr //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cr
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.pix_x //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.pix_x
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.pix_y //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.pix_y
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.lambda_mv //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.lambda_mv
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.lambda_y //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.lambda_y
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.qp_y //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.qp_y
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.x_last_ctu //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.x_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.md_input_ptr.y_last_ctu //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.md_input_ptr.y_last_ctu
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.min_scu_x //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.min_scu_x
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.min_scu_x
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.min_scu_x
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.min_scu_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.min_scu_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.min_scu_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.min_scu_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.min_scu_x
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.min_scu_x
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.min_scu_x
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.min_scu_x
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.min_scu_x
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.min_scu_x
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.min_scu_x
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.min_scu_x
#pragma HLS INTERFACE axis  port=bp_in_buf[0].config.min_scu_y //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].config.min_scu_y
#pragma HLS INTERFACE axis  port=bp_in_buf[2].config.min_scu_y
#pragma HLS INTERFACE axis  port=bp_in_buf[3].config.min_scu_y
#pragma HLS INTERFACE axis  port=bp_in_buf[4].config.min_scu_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].config.min_scu_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].config.min_scu_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].config.min_scu_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].config.min_scu_y
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].config.min_scu_y
// #pragma HLS INTERFACE axis port=bp_in_buf[10].config.min_scu_y
// #pragma HLS INTERFACE axis port=bp_in_buf[11].config.min_scu_y
// #pragma HLS INTERFACE axis port=bp_in_buf[12].config.min_scu_y
// #pragma HLS INTERFACE axis port=bp_in_buf[13].config.min_scu_y
// #pragma HLS INTERFACE axis port=bp_in_buf[14].config.min_scu_y
// #pragma HLS INTERFACE axis port=bp_in_buf[15].config.min_scu_y
#pragma HLS INTERFACE axis  port=bp_in_buf[0].version  //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].version
#pragma HLS INTERFACE axis  port=bp_in_buf[2].version
#pragma HLS INTERFACE axis  port=bp_in_buf[3].version
#pragma HLS INTERFACE axis  port=bp_in_buf[4].version
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].version
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].version
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].version
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].version
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].version
// #pragma HLS INTERFACE axis port=bp_in_buf[10].version
// #pragma HLS INTERFACE axis port=bp_in_buf[11].version
// #pragma HLS INTERFACE axis port=bp_in_buf[12].version
// #pragma HLS INTERFACE axis port=bp_in_buf[13].version
// #pragma HLS INTERFACE axis port=bp_in_buf[14].version
// #pragma HLS INTERFACE axis port=bp_in_buf[15].version
#pragma HLS INTERFACE axis  port=bp_in_buf[0].line_buffer.y0  //
#pragma HLS INTERFACE axis  port=bp_in_buf[1].line_buffer.y0
#pragma HLS INTERFACE axis  port=bp_in_buf[2].line_buffer.y0
#pragma HLS INTERFACE axis  port=bp_in_buf[3].line_buffer.y0
#pragma HLS INTERFACE axis  port=bp_in_buf[4].line_buffer.y0
// #pragma HLS INTERFACE axis  port=bp_in_buf[5].line_buffer.y0
// #pragma HLS INTERFACE axis  port=bp_in_buf[6].line_buffer.y0
// #pragma HLS INTERFACE axis  port=bp_in_buf[7].line_buffer.y0
// #pragma HLS INTERFACE axis  port=bp_in_buf[8].line_buffer.y0
// #pragma HLS INTERFACE axis  port=bp_in_buf[9].line_buffer.y0
// #pragma HLS INTERFACE axis port=bp_in_buf[10].line_buffer.y0
// #pragma HLS INTERFACE axis port=bp_in_buf[11].line_buffer.y0
// #pragma HLS INTERFACE axis port=bp_in_buf[12].line_buffer.y0
// #pragma HLS INTERFACE axis port=bp_in_buf[13].line_buffer.y0
// #pragma HLS INTERFACE axis port=bp_in_buf[14].line_buffer.y0
// #pragma HLS INTERFACE axis port=bp_in_buf[15].line_buffer.y0
#endif

#if (!LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis port=out_in_buf.line_buffer.y0
#endif 


#if LOAD_AXIS_ADAPTER
// AXIS----------------------------------------------------------------AXIS
#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
//#if (1 && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.pic_width// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.pic_width 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.pic_height	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.slice_type	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.slice_qp	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.max_dt_size	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.affine_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.umve_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.dt_intra_enable_flag  // 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.num_of_hmvp_cand	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.ipf_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.pic_width_in_scu	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.pic_height_in_scu	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.log2_max_cuwh	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.log2_culine	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.max_cuwh	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.f_scu	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.gop_size	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.bit_depth_internal	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.max_part_ratio	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.min_cu_size	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.max_split_times	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.min_qt_size	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.max_bt_size	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.max_eqt_size	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.affine_subblock_size_idx	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.ipcm_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.position_based_transform_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.secondary_transform_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.tscpm_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.amvr_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.emvr_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.eipm_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.mipf_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.interpf_enable_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.umve_set_flag	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.cur_ptr	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cb	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cr	// 
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_fw_ptr.chroma_quant_param_delta_cr

#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.pix_x	//
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.pix_y	// 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.lambda_mv	// 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.lambda_y	// 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.lambda_y

#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.qp_y		// 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.qp_y

#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.x_last_ctu		// 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[0].config.md_input_ptr.y_last_ctu		// 
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[1].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[2].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[3].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[4].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[5].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[6].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[7].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[8].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[9].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.md_input_ptr.y_last_ctu

#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].config.min_scu_y
#endif 

//#if (1 && _LOAD_IF_AXIS_EN_)
#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].buffer.skip_in.slice_type //
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].buffer.skip_in.slice_type
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].buffer.skip_in.ptr //
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].buffer.skip_in.ptr
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].buffer.skip_in.ptr
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].buffer.skip_in.umve_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].buffer.skip_in.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].buffer.skip_in.pix_x //
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].buffer.skip_in.pix_x
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].buffer.skip_in.pix_x
#endif 

//#if (1 && _LOAD_IF_AXIS_EN_)
#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].insn	//
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].insn
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].insn
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].insn
#endif 

//#if (1 && _LOAD_IF_AXIS_EN_)
#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[0].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[1].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[2].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[3].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[4].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[5].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[6].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[7].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[8].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.pu_in_buf[9].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[10].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[11].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[12].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[13].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[14].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[15].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[16].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[17].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[18].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[19].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[20].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[21].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[22].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[23].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[24].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[25].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[26].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.pu_in_buf[27].line_buffer.y0
#endif

//#if (1 && _LOAD_IF_AXIS_EN_)
#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].insn //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].insn
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].insn
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].insn
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].insn
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].insn
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].insn
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].insn
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].insn

#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.pic_width //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.pic_width
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.pic_height //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.pic_height
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.slice_type //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.slice_type
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.slice_qp //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.slice_qp
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.max_dt_size //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.max_dt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.affine_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.affine_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.umve_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.umve_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.dt_intra_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.dt_intra_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.num_of_hmvp_cand //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.num_of_hmvp_cand
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.ipf_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.ipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.pic_width_in_scu //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.pic_width_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.pic_height_in_scu //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.pic_height_in_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.log2_max_cuwh //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.log2_max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.log2_culine //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.log2_culine
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.max_cuwh //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.max_cuwh
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.f_scu //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.f_scu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.gop_size //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.gop_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.bit_depth_internal //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.bit_depth_internal
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.max_part_ratio //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.max_part_ratio
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.min_cu_size //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.min_cu_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.max_split_times //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.max_split_times
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.min_qt_size //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.min_qt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.max_bt_size //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.max_bt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.max_eqt_size //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.max_eqt_size
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.affine_subblock_size_idx //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.affine_subblock_size_idx
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.ipcm_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.ipcm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.position_based_transform_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.position_based_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.secondary_transform_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.secondary_transform_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.tscpm_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.tscpm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.amvr_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.amvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.emvr_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.emvr_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.eipm_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.eipm_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.mipf_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.mipf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.interpf_enable_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.interpf_enable_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.umve_set_flag //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.umve_set_flag
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.cur_ptr //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.cur_ptr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cb //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cb
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_fw_ptr.chroma_quant_param_delta_cr //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_fw_ptr.chroma_quant_param_delta_cr
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_fw_ptr.chroma_quant_param_delta_cr

#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.pix_x //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.pix_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.pix_y //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.pix_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.lambda_mv //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.lambda_mv
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.lambda_y //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.lambda_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.qp_y //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.qp_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.x_last_ctu //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.x_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.md_input_ptr.y_last_ctu //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.md_input_ptr.y_last_ctu
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.min_scu_x //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.min_scu_x
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.min_scu_x
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].config.min_scu_y //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].config.min_scu_y
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].config.min_scu_y
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].version  //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].version
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].version
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].version
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].version
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].version
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].version
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].version
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[0].line_buffer.y0  //
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[1].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[2].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[3].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[4].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[5].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[6].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[7].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[8].line_buffer.y0
#pragma HLS INTERFACE axis  port=axis_load.bp_in_buf[9].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[10].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[11].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[12].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[13].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[14].line_buffer.y0
#pragma HLS INTERFACE axis port=axis_load.bp_in_buf[15].line_buffer.y0
#endif


//#if (1 && _LOAD_IF_AXIS_EN_)
//#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
//#pragma HLS INTERFACE axis port=axis_load.out_in_buf.line_buffer.y0
//#endif 
#endif
//================================================================================================================================

	InstructionRegister<Instruction> ir1, ir2;
	PRMan pu_man;
	ir1.init();
	ir2.init();
	pu_man.init();

	bool done = false;

#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	// buffer.skip_in.slice_type = axis_load.buffer.skip_in.slice_type;
	// buffer.skip_in.ptr = axis_load.buffer.skip_in.ptr;
	// buffer.skip_in.umve_enable_flag = axis_load.buffer.skip_in.umve_enable_flag;
	// buffer.skip_in.pix_x = axis_load.buffer.skip_in.pix_x;
#endif

	while (!done)
	{
		done = load_core(ir1, ir2, mem_load, mem_pu, pu_man, status, reg
#if MD_CHANNEL_DEBUG
			, bp_chn_in_buf, out_chn_in_buf

#endif
			, pu_in_buf
#if MD_CHANNEL_DEBUG
			, input
#endif
			, pool, config, buffer, push_data, bp_in_buf, cu_data, out_in_buf);
	}
#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	for (int i = 0; i < 28; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=29
#pragma HLS UNROLL
		axis_load.pu_in_buf[i].config.md_fw_ptr.pic_width = pu_in_buf[i].config.md_fw_ptr.pic_width;
		axis_load.pu_in_buf[i].config.md_fw_ptr.pic_height = pu_in_buf[i].config.md_fw_ptr.pic_height;
		axis_load.pu_in_buf[i].config.md_fw_ptr.slice_type = pu_in_buf[i].config.md_fw_ptr.slice_type;
		axis_load.pu_in_buf[i].config.md_fw_ptr.slice_qp = pu_in_buf[i].config.md_fw_ptr.slice_qp;
		axis_load.pu_in_buf[i].config.md_fw_ptr.max_dt_size = pu_in_buf[i].config.md_fw_ptr.max_dt_size;
		axis_load.pu_in_buf[i].config.md_fw_ptr.affine_enable_flag = pu_in_buf[i].config.md_fw_ptr.affine_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.umve_enable_flag = pu_in_buf[i].config.md_fw_ptr.umve_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.dt_intra_enable_flag = pu_in_buf[i].config.md_fw_ptr.dt_intra_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.num_of_hmvp_cand = pu_in_buf[i].config.md_fw_ptr.num_of_hmvp_cand;
		axis_load.pu_in_buf[i].config.md_fw_ptr.ipf_enable_flag = pu_in_buf[i].config.md_fw_ptr.ipf_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.pic_width_in_scu = pu_in_buf[i].config.md_fw_ptr.pic_width_in_scu;
		axis_load.pu_in_buf[i].config.md_fw_ptr.pic_height_in_scu = pu_in_buf[i].config.md_fw_ptr.pic_height_in_scu;
		axis_load.pu_in_buf[i].config.md_fw_ptr.log2_max_cuwh = pu_in_buf[i].config.md_fw_ptr.log2_max_cuwh;
		axis_load.pu_in_buf[i].config.md_fw_ptr.log2_culine = pu_in_buf[i].config.md_fw_ptr.log2_culine;
		axis_load.pu_in_buf[i].config.md_fw_ptr.max_cuwh = pu_in_buf[i].config.md_fw_ptr.max_cuwh;
		axis_load.pu_in_buf[i].config.md_fw_ptr.f_scu = pu_in_buf[i].config.md_fw_ptr.f_scu;
		axis_load.pu_in_buf[i].config.md_fw_ptr.gop_size = pu_in_buf[i].config.md_fw_ptr.gop_size;
		axis_load.pu_in_buf[i].config.md_fw_ptr.bit_depth_internal = pu_in_buf[i].config.md_fw_ptr.bit_depth_internal;
		axis_load.pu_in_buf[i].config.md_fw_ptr.max_part_ratio = pu_in_buf[i].config.md_fw_ptr.max_part_ratio;
		axis_load.pu_in_buf[i].config.md_fw_ptr.min_cu_size = pu_in_buf[i].config.md_fw_ptr.min_cu_size;
		axis_load.pu_in_buf[i].config.md_fw_ptr.max_split_times = pu_in_buf[i].config.md_fw_ptr.max_split_times;
		axis_load.pu_in_buf[i].config.md_fw_ptr.min_qt_size = pu_in_buf[i].config.md_fw_ptr.min_qt_size;
		axis_load.pu_in_buf[i].config.md_fw_ptr.max_bt_size = pu_in_buf[i].config.md_fw_ptr.max_bt_size;
		axis_load.pu_in_buf[i].config.md_fw_ptr.max_eqt_size = pu_in_buf[i].config.md_fw_ptr.max_eqt_size;
		axis_load.pu_in_buf[i].config.md_fw_ptr.affine_subblock_size_idx = pu_in_buf[i].config.md_fw_ptr.affine_subblock_size_idx;
		axis_load.pu_in_buf[i].config.md_fw_ptr.ipcm_enable_flag = pu_in_buf[i].config.md_fw_ptr.ipcm_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.position_based_transform_enable_flag = pu_in_buf[i].config.md_fw_ptr.position_based_transform_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.secondary_transform_enable_flag = pu_in_buf[i].config.md_fw_ptr.secondary_transform_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.tscpm_enable_flag = pu_in_buf[i].config.md_fw_ptr.tscpm_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.amvr_enable_flag = pu_in_buf[i].config.md_fw_ptr.amvr_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.emvr_enable_flag = pu_in_buf[i].config.md_fw_ptr.emvr_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.eipm_enable_flag = pu_in_buf[i].config.md_fw_ptr.eipm_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.mipf_enable_flag = pu_in_buf[i].config.md_fw_ptr.mipf_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.interpf_enable_flag = pu_in_buf[i].config.md_fw_ptr.interpf_enable_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.umve_set_flag = pu_in_buf[i].config.md_fw_ptr.umve_set_flag;
		axis_load.pu_in_buf[i].config.md_fw_ptr.cur_ptr = pu_in_buf[i].config.md_fw_ptr.cur_ptr;
		axis_load.pu_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cb = pu_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cb;
		axis_load.pu_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cr = pu_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cr;
		axis_load.pu_in_buf[i].config.md_input_ptr.pix_x = pu_in_buf[i].config.md_input_ptr.pix_x;
		axis_load.pu_in_buf[i].config.md_input_ptr.pix_y = pu_in_buf[i].config.md_input_ptr.pix_y;
		axis_load.pu_in_buf[i].config.md_input_ptr.lambda_mv = pu_in_buf[i].config.md_input_ptr.lambda_mv;
		axis_load.pu_in_buf[i].config.md_input_ptr.lambda_y = pu_in_buf[i].config.md_input_ptr.lambda_y;
		axis_load.pu_in_buf[i].config.md_input_ptr.qp_y = pu_in_buf[i].config.md_input_ptr.qp_y;
		axis_load.pu_in_buf[i].config.md_input_ptr.x_last_ctu = pu_in_buf[i].config.md_input_ptr.x_last_ctu;
		axis_load.pu_in_buf[i].config.md_input_ptr.y_last_ctu = pu_in_buf[i].config.md_input_ptr.y_last_ctu;
		axis_load.pu_in_buf[i].config.min_scu_x = pu_in_buf[i].config.min_scu_x;
		axis_load.pu_in_buf[i].config.min_scu_y = pu_in_buf[i].config.min_scu_y;
	}
#endif

#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_) 
	for (int i = 0; i < 28; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=29
#pragma HLS UNROLL
		axis_load.pu_in_buf[i].buffer.skip_in.slice_type = pu_in_buf[i].buffer.skip_in.slice_type;
		axis_load.pu_in_buf[i].buffer.skip_in.ptr = pu_in_buf[i].buffer.skip_in.ptr;
		axis_load.pu_in_buf[i].buffer.skip_in.umve_enable_flag = pu_in_buf[i].buffer.skip_in.umve_enable_flag;
		axis_load.pu_in_buf[i].buffer.skip_in.pix_x = pu_in_buf[i].buffer.skip_in.pix_x;
	}
#endif

#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	for (int i = 0; i < 28; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=29
#pragma HLS UNROLL
		axis_load.pu_in_buf[i].insn = pu_in_buf[i].insn;
		axis_load.pu_in_buf[i].line_buffer.y0 = pu_in_buf[i].line_buffer.y0;
	}
#endif

#if (LOAD_AXIS_ADAPTER && _LOAD_IF_AXIS_EN_)
	for (int i = 0; i < 16; i++)
	{
#pragma HLS LOOP_TRIPCOUNT max=18
#pragma HLS UNROLL
		axis_load.bp_in_buf[i].insn = bp_in_buf[i].insn;
		axis_load.bp_in_buf[i].config.md_fw_ptr.pic_width = bp_in_buf[i].config.md_fw_ptr.pic_width;
		axis_load.bp_in_buf[i].config.md_fw_ptr.pic_height = bp_in_buf[i].config.md_fw_ptr.pic_height;
		axis_load.bp_in_buf[i].config.md_fw_ptr.slice_type = bp_in_buf[i].config.md_fw_ptr.slice_type;
		axis_load.bp_in_buf[i].config.md_fw_ptr.slice_qp = bp_in_buf[i].config.md_fw_ptr.slice_qp;
		axis_load.bp_in_buf[i].config.md_fw_ptr.max_dt_size = bp_in_buf[i].config.md_fw_ptr.max_dt_size;
		axis_load.bp_in_buf[i].config.md_fw_ptr.affine_enable_flag = bp_in_buf[i].config.md_fw_ptr.affine_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.umve_enable_flag = bp_in_buf[i].config.md_fw_ptr.umve_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.dt_intra_enable_flag = bp_in_buf[i].config.md_fw_ptr.dt_intra_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.num_of_hmvp_cand = bp_in_buf[i].config.md_fw_ptr.num_of_hmvp_cand;
		axis_load.bp_in_buf[i].config.md_fw_ptr.ipf_enable_flag = bp_in_buf[i].config.md_fw_ptr.ipf_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.pic_width_in_scu = bp_in_buf[i].config.md_fw_ptr.pic_width_in_scu;
		axis_load.bp_in_buf[i].config.md_fw_ptr.pic_height_in_scu = bp_in_buf[i].config.md_fw_ptr.pic_height_in_scu;
		axis_load.bp_in_buf[i].config.md_fw_ptr.log2_max_cuwh = bp_in_buf[i].config.md_fw_ptr.log2_max_cuwh;
		axis_load.bp_in_buf[i].config.md_fw_ptr.log2_culine = bp_in_buf[i].config.md_fw_ptr.log2_culine;
		axis_load.bp_in_buf[i].config.md_fw_ptr.max_cuwh = bp_in_buf[i].config.md_fw_ptr.max_cuwh;
		axis_load.bp_in_buf[i].config.md_fw_ptr.f_scu = bp_in_buf[i].config.md_fw_ptr.f_scu;
		axis_load.bp_in_buf[i].config.md_fw_ptr.gop_size = bp_in_buf[i].config.md_fw_ptr.gop_size;
		axis_load.bp_in_buf[i].config.md_fw_ptr.bit_depth_internal = bp_in_buf[i].config.md_fw_ptr.bit_depth_internal;
		axis_load.bp_in_buf[i].config.md_fw_ptr.max_part_ratio = bp_in_buf[i].config.md_fw_ptr.max_part_ratio;
		axis_load.bp_in_buf[i].config.md_fw_ptr.min_cu_size = bp_in_buf[i].config.md_fw_ptr.min_cu_size;
		axis_load.bp_in_buf[i].config.md_fw_ptr.max_split_times = bp_in_buf[i].config.md_fw_ptr.max_split_times;
		axis_load.bp_in_buf[i].config.md_fw_ptr.min_qt_size = bp_in_buf[i].config.md_fw_ptr.min_qt_size;
		axis_load.bp_in_buf[i].config.md_fw_ptr.max_bt_size = bp_in_buf[i].config.md_fw_ptr.max_bt_size;
		axis_load.bp_in_buf[i].config.md_fw_ptr.max_eqt_size = bp_in_buf[i].config.md_fw_ptr.max_eqt_size;
		axis_load.bp_in_buf[i].config.md_fw_ptr.affine_subblock_size_idx = bp_in_buf[i].config.md_fw_ptr.affine_subblock_size_idx;
		axis_load.bp_in_buf[i].config.md_fw_ptr.ipcm_enable_flag = bp_in_buf[i].config.md_fw_ptr.ipcm_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.position_based_transform_enable_flag = bp_in_buf[i].config.md_fw_ptr.position_based_transform_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.secondary_transform_enable_flag = bp_in_buf[i].config.md_fw_ptr.secondary_transform_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.tscpm_enable_flag = bp_in_buf[i].config.md_fw_ptr.tscpm_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.amvr_enable_flag = bp_in_buf[i].config.md_fw_ptr.amvr_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.emvr_enable_flag = bp_in_buf[i].config.md_fw_ptr.emvr_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.eipm_enable_flag = bp_in_buf[i].config.md_fw_ptr.eipm_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.mipf_enable_flag = bp_in_buf[i].config.md_fw_ptr.mipf_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.interpf_enable_flag = bp_in_buf[i].config.md_fw_ptr.interpf_enable_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.umve_set_flag = bp_in_buf[i].config.md_fw_ptr.umve_set_flag;
		axis_load.bp_in_buf[i].config.md_fw_ptr.cur_ptr = bp_in_buf[i].config.md_fw_ptr.cur_ptr;
		axis_load.bp_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cb = bp_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cb;
		axis_load.bp_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cr = bp_in_buf[i].config.md_fw_ptr.chroma_quant_param_delta_cr;

		axis_load.bp_in_buf[i].config.md_input_ptr.pix_x = bp_in_buf[i].config.md_input_ptr.pix_x;
		axis_load.bp_in_buf[i].config.md_input_ptr.pix_y = bp_in_buf[i].config.md_input_ptr.pix_y;
		axis_load.bp_in_buf[i].config.md_input_ptr.lambda_mv = bp_in_buf[i].config.md_input_ptr.lambda_mv;
		axis_load.bp_in_buf[i].config.md_input_ptr.lambda_y = bp_in_buf[i].config.md_input_ptr.lambda_y;
		axis_load.bp_in_buf[i].config.md_input_ptr.qp_y = bp_in_buf[i].config.md_input_ptr.qp_y;
		axis_load.bp_in_buf[i].config.md_input_ptr.x_last_ctu = bp_in_buf[i].config.md_input_ptr.x_last_ctu;
		axis_load.bp_in_buf[i].config.md_input_ptr.y_last_ctu = bp_in_buf[i].config.md_input_ptr.y_last_ctu;
		axis_load.bp_in_buf[i].config.min_scu_x = bp_in_buf[i].config.min_scu_x;
		axis_load.bp_in_buf[i].config.min_scu_y = bp_in_buf[i].config.min_scu_y;
		axis_load.bp_in_buf[i].version = bp_in_buf[i].version;
		axis_load.bp_in_buf[i].line_buffer.y0 = bp_in_buf[i].line_buffer.y0;
	}
#endif
}
