#include "md1.h"
#include "md_cmod.h"

void compare_cost(int& idx, s64& cost_best, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn)
{
	cost_best = MAX_S64;
	idx = -1;
	for (int i = 0; i < PU_NUM; ++i)
	{
#pragma HLS PIPELINE
		if (insn.flag_pu[i])
		{
			s64 cost = pu_out_buf[i].cost_temp;
			if (cost_best > cost)
			{
				cost_best = cost;
				idx = i;
			}
		}
	}
}

void store_n_1(LCUConfig& config, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn, Channel& channel, CuDataPool& cu_data)
{
	int idx;
	s64 cost;
#pragma HLS ARRAY_PARTITION variable=pu_out_buf complete dim=1 // 4min->4min20s, ~8%
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp_64.reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].cu_data_temp.reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.mpm_ext complete dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.num_nz_coef complete dim=1
//#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.coef_y cyclic factor=4 dim=1
//#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.coef_y cyclic factor=4 dim=2
//#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.reco_y cyclic factor=4 dim=1
//#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].curr_cu_data_temp.reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.up[0].mv cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.up[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.up[0].refi cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.up[0].rec cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.left[0].mv cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.left[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.left[0].refi cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.left[0].rec cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.topleft[0].mv cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.topleft[0].mv cyclic factor=2 dim=2
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.topleft[0].refi cyclic factor=2 dim=1
#pragma HLS ARRAY_PARTITION variable=pu_out_buf[0].linebuffer.topleft[0].rec cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].split_mode complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].split_mode complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data[0].split_mode cyclic factor=16 dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data[0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data[0].mv complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data[0].mvd complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data[0].mvd complete dim=3
#pragma HLS ARRAY_PARTITION variable=cu_data[0].refi complete dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data[0].mpm_ext cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].mpm complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].ipm complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].num_nz_coef complete dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=cu_data[0].reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=cu_data[0].reco_y cyclic factor=4 dim=2
	compare_cost(idx, cost, pu_out_buf, insn);

	copy(channel.line_buffer, pu_out_buf[idx].line_buffer);

	if (idx < BYPASS + BP_NUM)
	{
		copy(channel.cu_data_ptr, pu_out_buf[idx].cu_data_ptr);
		channel.cu_data_ptr.set_cost(insn.x0 & 63, insn.y0 & 63, insn.cu_width, insn.cu_height, cost);
		write_split(insn.x0 & 63, insn.y0 & 63, insn.cu_width_log2, insn.cu_height_log2, insn.cud, pu_out_buf[idx].split_mode, pu_out_buf[idx].cu_data_ptr, cu_data);
	}
	else
	{
		if (insn.cu_width == 64 || insn.cu_height == 64)
		{
			write_memory(insn.cu_width, insn.cu_height, cu_data + insn.mem_ptr, pu_out_buf[idx].curr_cu_data_temp_64);
		}
		else if (insn.cu_width == 32 || insn.cu_height == 32)
		{
			write_memory(insn.cu_width, insn.cu_height, cu_data + insn.mem_ptr, pu_out_buf[idx].cu_data_temp);
		}
		else if (insn.cu_width == 16 || insn.cu_height == 16)
		{
			assert(insn.cu_width == 16 && insn.cu_height == 16);
			write_memory(insn.cu_width, insn.cu_height, cu_data + insn.mem_ptr, pu_out_buf[idx].curr_cu_data_temp);
		}
		else
		{
			assert(false);
		}
		write_split(cu_data + insn.mem_ptr, insn.x0 & 63, insn.y0 & 63, insn.cu_width_log2, insn.cu_height_log2, insn.cud, pu_out_buf[idx].split_mode);
		channel.cu_data_ptr.set(insn.x0 & 63, insn.y0 & 63, insn.cu_width, insn.cu_height, insn.mem_ptr, cost);
	}

#if DEBUG_PRINT_MD_COST
	PRINT_MD_COST_P1(insn.x0, insn.y0, insn.cu_width, insn.cu_height, cost);
#endif
}

#if !ONLY_QT_QT
// TODO: merge all size
void store_n_1_16x16(LCUConfig& config, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn, Channel& channel)
{
	s64 cost_best = MAX_S64;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data_best;
	U3 best_split_mode[1] = { NO_SPLIT };
	U2 best_cons_pred_mode[1] = { NO_MODE_CONS };
	U2 best_tree_status_child[1] = { TREE_LC };
	U2 tree_status_temp[1] = { insn.tree_status };

#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->coef_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=1
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->reco_y cyclic factor=4 dim=2
#pragma HLS ARRAY_PARTITION variable=curr_cu_data_best->mpm_ext cyclic factor=8 dim=1
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_best_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY null_curr_cu_data_best_ptr2;

	if (insn.flag_pu[KERNEL_16X16])
	{
		compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[KERNEL_16X16].cost_temp, &pu_out_buf[KERNEL_16X16].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best,
			&null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2, insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
			pu_out_buf[KERNEL_16X16].split_mode, best_split_mode, best_cons_pred_mode, pu_out_buf[KERNEL_16X16].cons_pred_mode, pu_out_buf[KERNEL_16X16].tree_status_child, best_tree_status_child, tree_status_temp, 4);
	}
	if (insn.flag_pu[KERNEL_8X16])
	{
		compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[KERNEL_8X16].cost_temp, &pu_out_buf[KERNEL_8X16].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best,
			&null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2, insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
			pu_out_buf[KERNEL_8X16].split_mode, best_split_mode, best_cons_pred_mode, pu_out_buf[KERNEL_8X16].cons_pred_mode, pu_out_buf[KERNEL_8X16].tree_status_child, best_tree_status_child, tree_status_temp, 4);
	}
	if (insn.flag_pu[KERNEL_16X8])
	{
		compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[KERNEL_16X8].cost_temp, &pu_out_buf[KERNEL_16X8].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best,
			&null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2, insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
			pu_out_buf[KERNEL_16X8].split_mode, best_split_mode, best_cons_pred_mode, pu_out_buf[KERNEL_16X8].cons_pred_mode, pu_out_buf[KERNEL_16X8].tree_status_child, best_tree_status_child, tree_status_temp, 4);
	}
	if (insn.flag_pu[KERNEL_8X8])
	{
		compare_store_top(&config.md_fw_ptr, cost_best, pu_out_buf[KERNEL_8X8].cost_temp, &pu_out_buf[KERNEL_8X8].curr_cu_data_temp, &null_curr_cu_data_ptr_32, &curr_cu_data_best,
			&null_curr_cu_data_best_ptr_32, &null_curr_cu_data_best_ptr2, insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status,
			pu_out_buf[KERNEL_8X8].split_mode, best_split_mode, best_cons_pred_mode, pu_out_buf[KERNEL_8X8].cons_pred_mode, pu_out_buf[KERNEL_8X8].tree_status_child, best_tree_status_child, tree_status_temp, 4);
	}

	int scu_h = (insn.y0 + 16 > config.md_fw_ptr.pic_height ? ((int)(config.md_fw_ptr.pic_height - insn.y0) >> 2) : (16 >> 2));
	int scu_w = (insn.x0 + 16 > config.md_fw_ptr.pic_width ? ((int)(config.md_fw_ptr.pic_width - insn.x0) >> 2) : (16 >> 2));
	STATIC_C ENC_CU_DATA_ARRAY null_curr_cu_data_array;
	STATIC_C IP_ARRAY_SIZE<32, 32> null_IP_buffer_32x32;
	STATIC_C MPM_ARRAY_SIZE<32, 32> null_MPM_buffer_32x32;
	STATIC_C IP_MAP_SCU_SIZE<64, 64> null_IP_map_scu_64x64;
	update_nebs_qt(channel.nebs_line_hor, channel.nebs_line_ver, curr_cu_data_best.refi, curr_cu_data_best.mv, insn.cu_width, insn.cu_height, insn.part_num);
	channel.me_line_map_16.update_me_line_qt(insn.part_num, curr_cu_data_best.map_scu, curr_cu_data_best.map_cu_mode, insn.cu_width, insn.cu_height);
	update_line_buffer_qt(insn.part_num, insn.cu_width, insn.cu_height, scu_h, scu_w, insn.cu_width_log2, \
		&curr_cu_data_best, &channel.IP_buffer_16x16, &channel.MPM_buffer_16x16, &channel.IP_map_scu_ptr_32x32, \
		&null_curr_cu_data_array, &null_IP_buffer_32x32, &null_MPM_buffer_32x32, &null_IP_map_scu_64x64);

	int idx;
	s64 cost;
	compare_cost(idx, cost, pu_out_buf, insn);
	copy(channel.line_buffer, pu_out_buf[idx].line_buffer);

	store_enc_info_best_16_top(&config.md_fw_ptr, &curr_cu_data_best, insn.x0, insn.y0, insn.cup, insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.cons_pred_mode, insn.cu_width, insn.cu_height, insn.tree_status, best_split_mode, best_cons_pred_mode,
		&channel.prev_cu_data_ptr,
		insn.x_pos, insn.y_pos, insn.prev_up_log2_sub_cuw);
	channel.cost_best[(insn.y0 - insn.y_pos) >> MIN_CU_LOG2][(insn.x0 - insn.x_pos) >> MIN_CU_LOG2] = cost_best > MAX_S64 ? MAX_S64 : cost_best;

}

void store_n_1_16x64_64x16(LCUConfig& config, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn, Channel& channel)
{
	s64 cost_best = MAX_S64;

	//TODO: 封装store函数
	//cost_compare_fixed_point_one_time<64>(&config.md_fw_ptr, &cost_best, cost, curr_cu_data_temp, curr_cu_data_best, cu_width_log2, cu_height_log2, cud, tree_status,
	//	split_mode, &best_split_mode, &best_cons_pred_mode, cons_pred_mode_child, tree_status_child, &best_tree_status_child, &insn.tree_status, 3);

	//if (cu_width == 16 && cu_height == 64)
	//{
	//	IP_buffer_16x64->update_bi_ver(part_num, curr_cu_data_best->reco_y);
	//	IP_map_scu_16x64->update_bi_ver(part_num, IP_map_scu_ptr_32x64->cur_map_scu, curr_cu_data_best->map_scu);
	//	MPM_buffer_16x64->update_bi_ver(part_num, curr_cu_data_best->ipm[0]);
	//	update_nebs_bi_ver(nebs_line_ver, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
	//	me_line_map_64->update_bi_ver(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
	//}
	//else if (cu_width == 64 && cu_height == 16)
	//{
	//	IP_buffer_64x16->update_bi_hor(part_num, curr_cu_data_best->reco_y);
	//	IP_map_scu_64x16->update_bi_hor(part_num, IP_map_scu_ptr_64x32->cur_map_scu, curr_cu_data_best->map_scu);
	//	MPM_buffer_64x16->update_bi_hor(part_num, curr_cu_data_best->ipm[0]);
	//	update_nebs_bi_hor(nebs_line_hor, curr_cu_data_best->refi, curr_cu_data_best->mv, cu_width, cu_height, part_num);
	//	me_line_map_64->update_bi_hor(part_num, curr_cu_data_best->map_scu, curr_cu_data_best->map_cu_mode, cu_width, cu_height);
	//}

	//store_enc_info_best(md_fw_ptr, curr_cu_data_best, x0, y0, cup, cu_width_log2, cu_height_log2, cud, cons_pred_mode, cu_width, cu_height, tree_status, &best_split_mode, &best_cons_pred_mode, num_split_to_try, prev_cu_data_ptr,
	//	split_struct_x_pos_0, split_struct_y_pos_0, prev_up_log2_sub_cuw);

	//channel.cost_best = cost_best > MAX_S64 ? MAX_S64 : cost_best;
}

void store_n_1_16x16_md2(LCUConfig& config, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn, Channel& channel)
{
	U3 best_split_mode = NO_SPLIT;
	U2 best_cons_pred_mode = NO_MODE_CONS;
	U2 best_tree_status_child = TREE_LC;
	U2 tree_status_temp = insn.tree_status;
	static ENC_CU_DATA_ARRAY_SIZE<16>	curr_cu_data_best;

	channel.cost_best[(insn.y0 - insn.y_pos) >> MIN_CU_LOG2][(insn.x0 - insn.x_pos) >> MIN_CU_LOG2] = MAX_S64;
	store_md2(&config.md_fw_ptr, channel.cost_best[(insn.y0 - insn.y_pos) >> MIN_CU_LOG2][(insn.x0 - insn.x_pos) >> MIN_CU_LOG2], pu_out_buf[0].cost_temp, pu_out_buf[1].cost_temp, pu_out_buf[2].cost_temp, pu_out_buf[0].curr_cu_data_temp, pu_out_buf[1].curr_cu_data_temp, pu_out_buf[2].curr_cu_data_temp, curr_cu_data_best,
		insn.cu_width_log2, insn.cu_height_log2, insn.cud, insn.tree_status, best_split_mode, best_cons_pred_mode, pu_out_buf[0].cons_pred_mode, pu_out_buf[1].cons_pred_mode,
		pu_out_buf[2].cons_pred_mode, pu_out_buf[0].tree_status_child, pu_out_buf[1].tree_status_child, pu_out_buf[2].tree_status_child, best_tree_status_child, tree_status_temp, //COMPARE_STORE
		pu_out_buf[0].split_mode_father, channel.IP_buffer_16x16, channel.IP_map_scu_16x16, channel.MPM_buffer_16x16, channel.nebs_line_ver, channel.nebs_line_hor, channel.me_line_map_16, insn.part_num,  //SWITCH
		insn.cu_width, insn.cu_height, &channel.IP_map_scu_ptr_16x32, &channel.IP_map_scu_ptr_32x16,
		insn.x0, insn.y0, insn.cup, insn.cons_pred_mode, 3, &channel.prev_cu_data_ptr,
		insn.x_pos, insn.y_pos, insn.prev_up_log2_sub_cuw, &best_split_mode, &curr_cu_data_best);//store_enc_info_best_16_top
}
#endif
#if MD_CHANNEL_DEBUG
void store_chn_1(InstructionStore& insn, BpChn2Store& bp_chn_out_buf, ShareMemory& pool)
{
	copy(pool.channel[insn.pool_idx].line_buffer, bp_chn_out_buf.line_buffer);
	pool.channel[insn.pool_idx].cu_data_ptr.init();

	if (insn.cu_width == 32 && insn.cu_height == 32)
	{	
		//memcpy(pool.channel[insn.pool_idx].up_nebs_32, bp_out_buf.up_nebs_32, sizeof(pool.channel[insn.pool_idx].up_nebs_32));
		//memcpy(pool.channel[insn.pool_idx].left_nebs_32, bp_out_buf.left_nebs_32, sizeof(pool.channel[insn.pool_idx].left_nebs_32));
		for (size_t i = 0; i < 10; i++)
		{
			pool.channel[insn.pool_idx].up_nebs_32[i] = bp_chn_out_buf.up_nebs_32[i];
		}
		for (size_t i = 0; i < 8; i++)
		{
			pool.channel[insn.pool_idx].left_nebs_32[i] = bp_chn_out_buf.left_nebs_32[i];
		}

		pool.channel[insn.pool_idx].me_line_map_ptr_32 = bp_chn_out_buf.me_line_map_ptr_32;
		pool.channel[insn.pool_idx].IP_buffer_ptr_32x32 = bp_chn_out_buf.IP_buffer_ptr_32x32;
		pool.channel[insn.pool_idx].IP_map_scu_ptr_32x32 = bp_chn_out_buf.IP_map_scu_ptr_32x32;
		pool.channel[insn.pool_idx].MPM_buffer_ptr_32x32 = bp_chn_out_buf.MPM_buffer_ptr_32x32;
	}
		else if (insn.cu_width == 64 && insn.cu_height == 64)
	{
		pool.channel[insn.pool_idx].IP_buffer_ptr_64x64 = bp_chn_out_buf.IP_buffer_ptr_64x64;
		pool.channel[insn.pool_idx].IP_map_scu_ptr_64x64 = bp_chn_out_buf.IP_map_scu_ptr_64x64;
		pool.channel[insn.pool_idx].MPM_buffer_ptr_64x64 = bp_chn_out_buf.MPM_buffer_ptr_64x64;
		for (size_t i = 0; i < 18; i++)
		{
			pool.channel[insn.pool_idx].up_nebs_64[i] = bp_chn_out_buf.up_nebs_64[i];
		}

		pool.channel[insn.pool_idx].me_line_map_ptr_64 = bp_chn_out_buf.me_line_map_ptr_64;
		for (size_t i = 0; i < 16; i++)
		{
			pool.channel[insn.pool_idx].left_nebs_64[i] = bp_chn_out_buf.left_nebs_64[i];
		}
		for (size_t i = 0; i < 18; i++)
		{
			pool.channel[insn.pool_idx].up_nebs_32x32[i] = bp_chn_out_buf.up_nebs_32x32[i];
		}
		for (size_t i = 0; i < 16; i++)
		{
			pool.channel[insn.pool_idx].left_nebs_32x32[i] = bp_chn_out_buf.left_nebs_32x32[i];
		}

		pool.channel[insn.pool_idx].IP_map_scu_ptr_64x64 = bp_chn_out_buf.IP_map_scu_ptr_64x64;
		//pool.channel[insn.pool_idx].prev_cu_data_ptr_32x32 = bp_chn_out_buf.prev_cu_data_ptr_32x32;
		pool.channel[insn.pool_idx].prev_cu_data_ptr_32x32.copy(bp_chn_out_buf.prev_cu_data_ptr_32x32);

		for (size_t i = 0; i < 10; i++)
		{
			pool.channel[insn.pool_idx].up_nebs_32[i] = bp_chn_out_buf.up_nebs_32[i];
		}
		for (size_t i = 0; i < 8; i++)
		{
			pool.channel[insn.pool_idx].left_nebs_32[i] = bp_chn_out_buf.left_nebs_32[i];
		}

		pool.channel[insn.pool_idx].me_line_map_ptr_32 = bp_chn_out_buf.me_line_map_ptr_32;
		pool.channel[insn.pool_idx].IP_buffer_ptr_32x32 = bp_chn_out_buf.IP_buffer_ptr_32x32;
		pool.channel[insn.pool_idx].IP_map_scu_ptr_32x32 = bp_chn_out_buf.IP_map_scu_ptr_32x32;
		pool.channel[insn.pool_idx].MPM_buffer_ptr_32x32 = bp_chn_out_buf.MPM_buffer_ptr_32x32;
		for (int i = 0; i < 8; ++i)
		{
			pool.channel[insn.pool_idx].nebs_line_ver_32x32[i] = bp_chn_out_buf.nebs_line_ver_32x32[i];
		}
		for (int i = 0; i < 16; ++i)
		{
			pool.channel[insn.pool_idx].nebs_line_hor_32x32[i] = bp_chn_out_buf.nebs_line_hor_32x32[i];
		}
	}

	else
	{
		assert(false);
	}
}
#endif
#if !ONLY_QT_QT
void store_n_1_32x32(InstructionStore& insn, PU2Store pu_out_buf[PU_NUM], Channel& channel)
{
	assert(insn.flag_pu[0]);
	for (int i = 1; i < PU_NUM; ++i)
	{
#pragma HLS UNROLL
		assert(insn.flag_pu[i] == 0);
	}
	channel.prev_cu_data_ptr = pu_out_buf[0].cu_data_temp;
	channel.cost_best[0][0] = pu_out_buf[0].cost_temp;
	channel.IP_map_scu_ptr_32x32 = pu_out_buf[0].IP_map_scu_ptr_32x32;
}

void store_32x32_to_64x64(InstructionStore& insn_store, Channel& channel, LCUConfig& config,
	s64 cost[1],
	U2 tree_status_child[1],
	U2 cons_pred_mode_child[1],
	U3 split_mode[1],
	ENC_ME_NEB_INFO                         nebs_line_ver[8],
	ENC_ME_NEB_INFO                         nebs_line_hor[8],
	COM_SPLIT_STRUCT split_struct,
	ENC_CU_DATA_ARRAY_SIZE<32>	cu_data_temp[1])
{
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16> null_curr_cu_data_ptr;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16> null_curr_cu_data_best_ptr;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<32> null_curr_cu_data_best_ptr_32;
	STATIC_C ENC_CU_DATA_ARRAY_SIZE<16> null_curr_cu_data_best_ptr_16;
	STATIC_C IP_ARRAY_SIZE<16, 16> null_IP_buffer_16x16;
	STATIC_C MPM_ARRAY_SIZE<16, 16> null_MPM_buffer_16x16;
	STATIC_C IP_MAP_SCU_SIZE<32, 32> null_IP_map_scu_32x32;

	insn_store.best_split_mode = NO_SPLIT;
	insn_store.best_cons_pred_mode = NO_MODE_CONS;
	insn_store.best_tree_status_child = TREE_LC;
	STATIC_C ENC_CU_DATA_ARRAY curr_cu_data_best;

	//printf("cost=%lld, split_mode=%d, tree_status_child=%d\n", cost[0], split_mode[0], tree_status_child[0]);
	//printf("nebs_line_ver=\n");
	//for (int i = 0; i < 8; ++i)
	//{
	//	nebs_line_ver[i].print();
	//}
	//printf("nebs_line_hor=\n");
	//for (int i = 0; i < 16; ++i)
	//{
	//	nebs_line_hor[i].print();
	//}
	//printf("IP_buffer_32x32=\n");
	//channel.IP_buffer_32x32.print();
	//printf("MPM_buffer_32x32=\n");
	//channel.MPM_buffer_32x32.print();
	//printf("IP_map_scu_ptr_64x64=\n");
	//channel.IP_map_scu_ptr_64x64.print();
	//printf("cu_data\n");
	//for (int i = 0; i < sizeof(cu_data_temp[0]); ++i)
	//{
	//	printf("%x", *((char*)cu_data_temp));
	//}
	//printf("\n");

	int best_index = cost[0] == MAX_S64 ? -1 : 0;
	U2 tree_status_temp = insn_store.tree_status;
	int scu_h = (insn_store.y0 + 32 > config.md_fw_ptr.pic_height ? ((config.md_fw_ptr.pic_height - insn_store.y0) >> 2) : (32 >> 2));
	int scu_w = (insn_store.x0 + 32 > config.md_fw_ptr.pic_width ? ((config.md_fw_ptr.pic_width - insn_store.x0) >> 2) : (32 >> 2));
	channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2] = best_index == -1 ? MAX_S64 : cost[best_index];

	if (best_index != -1)
	{
		copy_split_mode<32>(cu_data_temp, &curr_cu_data_best,
			insn_store.cu_width_log2, insn_store.cu_height_log2, insn_store.cud, best_index);
		compare_store_top(&config.md_fw_ptr, channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2], cost[best_index], &null_curr_cu_data_ptr, &cu_data_temp[best_index], &null_curr_cu_data_best_ptr, &null_curr_cu_data_best_ptr_32, &curr_cu_data_best,
			insn_store.cu_width_log2, insn_store.cu_height_log2, insn_store.cud, insn_store.tree_status,
			split_mode[best_index], &insn_store.best_split_mode, &insn_store.best_cons_pred_mode, cons_pred_mode_child[best_index], tree_status_child[best_index], &insn_store.best_tree_status_child, &tree_status_temp, 0);
	}

	update_nebs_qt(nebs_line_hor, nebs_line_ver, curr_cu_data_best.refi, curr_cu_data_best.mv, insn_store.cu_width, insn_store.cu_height, insn_store.part_num);

	channel.me_line_map_32.update_me_line_qt(insn_store.part_num, curr_cu_data_best.map_scu, curr_cu_data_best.map_cu_mode, insn_store.cu_width, insn_store.cu_height);

	update_line_buffer_qt(insn_store.part_num, insn_store.cu_width, insn_store.cu_height, scu_h, scu_w, insn_store.cu_width_log2, &null_curr_cu_data_best_ptr_16, \
		& null_IP_buffer_16x16, &null_MPM_buffer_16x16, &null_IP_map_scu_32x32, \
		&curr_cu_data_best, &channel.IP_buffer_32x32, &channel.MPM_buffer_32x32, &channel.IP_map_scu_ptr_64x64);

	store_enc_info_best(&config.md_fw_ptr, &curr_cu_data_best, insn_store.x0, insn_store.y0, insn_store.cup, insn_store.cu_width_log2, insn_store.cu_height_log2, \
		insn_store.cud, insn_store.cons_pred_mode, insn_store.cu_width, insn_store.cu_height, insn_store.tree_status, &insn_store.best_split_mode, &insn_store.best_cons_pred_mode, \
		insn_store.num_split_to_try, &channel.prev_cu_data_ptr_32x32, split_struct.x_pos[0], split_struct.y_pos[0], insn_store.prev_up_log2_sub_cuw);

	//return channel.cost_best[(insn_store.y0 - insn_store.y_pos) >> MIN_CU_LOG2][(insn_store.x0 - insn_store.x_pos) >> MIN_CU_LOG2];
}
#endif

void init(Channel& channel, InstructionStore& insn, LcuLineBuffer& frame)
{
#pragma HLS array_partition variable=frame.up cyclic factor=16 dim=1
	if (insn.x0 == 0 && insn.y0 == 0)
	{
		channel.line_buffer.init();
		frame.init();
	}
	else
	{
		load_line_buffer(insn.x0, insn.y0, channel.line_buffer, frame);
	}
	channel.cu_data_ptr.init();
}

void update(Channel& channel, Output2Store& pu_out_buf, InstructionStore& insn, LcuLineBuffer& frame)
{
	update_line_buffer(insn.x0, insn.y0, 64, 64, frame, pu_out_buf.line_buffer);
}

void store_data(U8 type, LCUConfig& config
#if MD_CHANNEL_DEBUG
	, BpChn2Store& bp_chn_out_buf
#endif
	, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn, ShareMemory& pool, CuDataPool& cu_data, Output2Store& out_out_buf, LcuLineBuffer& frame)
{
	switch (type)
	{
	case STORE_N_1: store_n_1(config, pu_out_buf, insn, pool.channel[insn.pool_idx], cu_data); break;
#if !ONLY_QT_QT
	case STORE_N_1_16x16: store_n_1_16x16(config, pu_out_buf, insn, pool.channel[insn.pool_idx]); break;
	case STORE_N_1_16x16_MD1: assert(false); break;
	case STORE_N_1_16x16_MD2: assert(false); break;
		case STORE_N_1_32x32: store_n_1_32x32(insn, pu_out_buf, pool.channel[insn.pool_idx]); break;

	case STORE_N_1_32x32_32x16_16x32_MD1: assert(false); break;
	case STORE_N_1_32x32_32x16_16x32_MD2: assert(false); break;
	case STORE_N_1_16x64_64x16: store_n_1_16x64_64x16(config, pu_out_buf, insn, pool.channel[insn.pool_idx]); break;
	case STORE_N_1_64x64_64x32_32x64: assert(false); break;
#endif
#if MD_CHANNEL_DEBUG
	case STORE_CHN_1: store_chn_1(insn, bp_chn_out_buf, pool); break;
#endif
	case INIT: init(pool.channel[insn.pool_idx], insn, frame); break;
	case UPDATE: update(pool.channel[insn.pool_idx], out_out_buf, insn, frame); break;
	default: assert(false); break;
	}
}

void store_insn_done(InstructionStore& insn, reverse_stream_type<ap_uint<1> > push_data[PU_NUM], hls::stream<U32>& status)
{
#pragma HLS ARRAY_PARTITION variable=insn.flag_pu complete dim=1
	for (int j = 0; j < PU_NUM; ++j)
	{
#pragma HLS UNROLL
		if (insn.flag_pu[j])
		{
			push_data[j].write(1);
		}
	}
	if (insn.type != UPDATE)
	{
		status.write(insn.status);
	}
}

bool check_fifo(U8& tag, bool& set, reverse_stream_type<ap_uint<1> >& push_data, Instruction& insn, PU2Store& pu2store
#if STORE_AXIS_ADAPTER
	, Store_RAM & ram_in
#endif
	)
{
#pragma HLS INLINE

	if (!set)
	{
		if (!push_data.full())
		{
#if STORE_AXIS_ADAPTER
			tag = ram_in.tag[0];
#else 
			tag = pu2store.tag;
#endif //STORE_AXIS_ADAPTER
			set = true;
		}
	}
	if (set)
	{
		if (tag == insn.tag_pu())
		{
			set = false;
			return true;
		}
	}
	return false;
}

bool get_insn_fifo_not_empty(Instruction& insn_out, InstructionRegister<Instruction>& ir, reverse_stream_type<ap_uint<1> > push_data[PU_NUM], PU2Store pu_out_buf[PU_NUM], 
#if STORE_AXIS_ADAPTER
	Store_RAM ram_in[PU_NUM],
#endif
	U8 tag[PU_NUM], bool set[PU_NUM])
{
	bool rflag[PU_NUM];
#pragma HLS array_partition variable=pu_out_buf complete dim=1
#pragma HLS array_partition variable=tag complete dim=1
#pragma HLS array_partition variable=set complete dim=1
#pragma HLS array_partition variable=rflag complete dim=1
#pragma HLS array_partition variable=push_data complete dim=1
#if STORE_AXIS_ADAPTER
#pragma HLS array_partition variable=ram_in complete dim=1
#endif
#if STORE_AXIS_ADAPTER
	for (int j = 0; j < PU_NUM; ++j)
	{
#pragma HLS UNROLL
		pu_out_buf[j].line_buffer.y0                           = ram_in[j].line_buffer_y0[0];
		pu_out_buf[j].cost_temp                                = ram_in[j].cost_temp[0];
		pu_out_buf[j].split_mode                               = ram_in[j].split_mode[0];
		pu_out_buf[j].tag                                      = ram_in[j].tag[0];
#if CUDQP_QP_MAP
		pu_out_buf[j].cu_data_temp.cu_qp_group_pred_qp         = ram_in[j].cu_data_temp_cu_qp_group_pred_qp[0];
		pu_out_buf[j].curr_cu_data_temp.cu_qp_group_pred_qp    = ram_in[j].curr_cu_data_temp_cu_qp_group_pred_qp[0];
		pu_out_buf[j].curr_cu_data_temp_64.cu_qp_group_pred_qp = ram_in[j].curr_cu_data_temp_64_cu_qp_group_pred_qp[0];
#endif
	}
#endif

	assert(ir.size);
	LOOP_IR:for (int i = 0; i < ir.size; ++i)
	{
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS PIPELINE
		Instruction& insn_tmp = ir.buf[i];
		//printf("%d\n", insn_tmp.opcode().to_int());

		bool flag = true;
		switch (insn_tmp.opcode())
		{
		case UPDATE:
		case STORE_N_1:
#if !ONLY_QT_QT
		case STORE_N_1_16x16:
		case STORE_N_1_16x16_MD1:
		case STORE_N_1_16x16_MD2:
		case STORE_N_1_32x32_32x16_16x32_MD1:
		case STORE_N_1_32x32_32x16_16x32_MD2:
		case STORE_N_1_16x64_64x16:
		case STORE_N_1_64x64_64x32_32x64:
		case STORE_N_1_32x32:
#endif
			for (int j = 0; j < PU_NUM; ++j)
			{
				rflag[j] = true;
				if (insn_tmp.flag_pu(j))
				{
					rflag[j] = check_fifo(tag[j], set[j], push_data[j], insn_tmp, pu_out_buf[j]
#if STORE_AXIS_ADAPTER
						, ram_in[j]
#endif
					);
					}
					}
			for (int j = 0; j < PU_NUM; ++j)
			{
				if (!rflag[j])
				{
					flag = false;
				}
			}
			break;
#if MD_CHANNEL_DEBUG
		case STORE_CHN_1:
			if (push_data[BYPASS_CHANNEL].full())
			{
				flag = false;
				break;
			}
			break;
#endif
		case INIT: break;
		case DONE:
			flag = (i == 0);
			break;
		default:
			assert(false);
			break;
		}

		if (flag)
		{
			insn_out = insn_tmp;
			ir.remove(i);
			return true;
		}
	}
	return false;
}

bool store_core(InstructionRegister<Instruction>& ir, Instruction mem_store[PR_SIZE], hls::stream<U32>& status, Register& reg, reverse_stream_type<ap_uint<1> > push_data[PU_NUM], LCUConfig& config
#if MD_CHANNEL_DEBUG
	, BpChn2Store& bp_chn_out_buf
#endif
	, PU2Store pu_out_buf[PU_NUM], 
#if STORE_AXIS_ADAPTER
	Store_RAM ram_in[PU_NUM],
#endif
	ShareMemory& pool, CuDataPool& cu_data, Output2Store& out_out_buf)
{
#pragma HLS INLINE

#if 0//store_core_interface
#pragma HLS INTERFACE axis port=config.md_fw_ptr
#pragma HLS INTERFACE axis port=config.md_input_ptr

	if (mem_store[0].opcode() == -1)
	{
		for (int n = 0; n < PU_NUM; ++n)
		{
#pragma HLS UNROLL
#pragma HLS INTERFACE axis port=pu_out_buf[n].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[n].cons_pred_mode
#pragma HLS INTERFACE axis port=pu_out_buf[n].tree_status_child
#pragma HLS INTERFACE axis port=pu_out_buf[n].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[n].tag

			pu_out_buf[n].cost_temp = MAX_COST;
		}
	}
#endif

#pragma HLS RESOURCE variable=mem_store core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pool core=RAM_1P_BRAM
#if MD_CHANNEL_DEBUG
#pragma HLS RESOURCE variable=bp_chn_out_buf.up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=bp_chn_out_buf.left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=bp_chn_out_buf.me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=bp_chn_out_buf.IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=bp_chn_out_buf.MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=bp_chn_out_buf.IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#endif //MD_CHANNEL_DEBUG

#if 0
	if (mem_store[0].opcode() == -1)
	{
		for (int n = 0; n < PU_NUM; ++n)
		{
#pragma HLS UNROLL
#pragma HLS RESOURCE variable=pu_out_buf[n].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[n].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[n].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[n].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[n].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[n].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[n].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM

			pu_out_buf[n].cost_temp = MAX_COST;
		}
	}
#endif
#if 0
#pragma HLS RESOURCE variable=pu_out_buf[0].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].curr_cu_data_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].curr_cu_data_temp core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=pu_out_buf[0].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].up_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].up_nebs_32 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=pu_out_buf[0].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].left_nebs_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].left_nebs_32 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=pu_out_buf[0].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].me_line_map_ptr_32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].me_line_map_ptr_32 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=pu_out_buf[0].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].IP_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].IP_buffer_ptr_32x32 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=pu_out_buf[0].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].MPM_buffer_ptr_32x32 core=RAM_1P_BRAM

#pragma HLS RESOURCE variable=pu_out_buf[0].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[1].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[2].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[3].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[4].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[5].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[6].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[7].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[8].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[9].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[10].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[11].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[12].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[13].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[14].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[15].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[16].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[17].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[18].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[19].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[20].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[21].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[22].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[23].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[24].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[25].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[26].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[27].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=pu_out_buf[28].IP_map_scu_ptr_32x32 core=RAM_1P_BRAM
#endif

//#pragma HLS DATA_PACK variable=config.md_fw_ptr instance=md_fw_ptr
//#pragma HLS DATA_PACK variable=config.md_input_ptr instance=md_input_ptr //Bitwidth of packed elements (16935952 bits) exceeds the maximum threshold (65536 bits)

#pragma HLS ARRAY_PARTITION variable=pu_out_buf complete dim=1 // 4min->4min20s, ~8%

	static U8 tag[PU_NUM];
	static bool set[PU_NUM] = {};
	static LcuLineBuffer frame;

	ir.update(mem_store);

	Instruction insn;
	if (!get_insn_fifo_not_empty(insn, ir, push_data, pu_out_buf,
#if STORE_AXIS_ADAPTER
		ram_in,
#endif
	 	tag, set)) { return false; }

	InstructionStore insn_store;
	insn_decode(insn, insn_store, reg);

	if (insn_store.type == DONE) { return true; }

	for (int i = 0; i < 1; ++i)
	{
		store_data(insn_store.type, config
#if MD_CHANNEL_DEBUG
			, bp_chn_out_buf
#endif
			, pu_out_buf, insn_store, pool, cu_data, out_out_buf, frame);
	}

	for (int i = 0; i < 1; ++i)
	{
		store_insn_done(insn_store, push_data, status);
	}

	return false;
}

void store(Instruction mem_store[PR_SIZE], hls::stream<U32>& status, Register& reg, reverse_stream_type<ap_uint<1> > push_data[PU_NUM], LCUConfig& config
#if MD_CHANNEL_DEBUG
	, BpChn2Store& bp_chn_out_buf
#endif
	, PU2Store pu_out_buf[PU_NUM]
#if STORE_AXIS_ADAPTER
	, Store_RAM ram_in[PU_NUM]
#endif
	, ShareMemory& pool, CuDataPool& cu_data, Output2Store& out_out_buf)
{
	// stream port unused may cause core dump
#if 0//store_interface
#pragma HLS INTERFACE axis port=config.md_fw_ptr
//#pragma HLS INTERFACE axis port=config.md_input_ptr //Abnormal program termination (11)

	if (mem_store[0].opcode() == -1)
	{
		for (int n = 0; n < PU_NUM; ++n)
		{
#pragma HLS UNROLL
#pragma HLS INTERFACE axis port=pu_out_buf[n].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[n].cons_pred_mode
#pragma HLS INTERFACE axis port=pu_out_buf[n].tree_status_child
#pragma HLS INTERFACE axis port=pu_out_buf[n].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[n].tag

			pu_out_buf[n].cost_temp = MAX_COST;
		}
	}
#endif

//#pragma HLS INTERFACE axis port=reg._sys

#if 1
#if !STORE_AXIS_ADAPTER
#if MD_CHANNEL_DEBUG
#pragma HLS INTERFACE axis port=bp_chn_out_buf.line_buffer.y0
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_32.min_scu_x
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_32.min_scu_y
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_32.above_line_idx
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_32.curr_ctu_idx
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_64.min_scu_x
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_64.min_scu_y
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_64.above_line_idx
#pragma HLS INTERFACE axis port=bp_chn_out_buf.me_line_map_ptr_64.curr_ctu_idx
#pragma HLS INTERFACE axis port=bp_chn_out_buf.IP_buffer_ptr_32x32.top_left
#pragma HLS INTERFACE axis port=bp_chn_out_buf.IP_buffer_ptr_64x64.top_left
#pragma HLS INTERFACE axis port=bp_chn_out_buf.MPM_buffer_ptr_32x32.up
#pragma HLS INTERFACE axis port=bp_chn_out_buf.MPM_buffer_ptr_32x32.left
#pragma HLS INTERFACE axis port=bp_chn_out_buf.MPM_buffer_ptr_64x64.up
#pragma HLS INTERFACE axis port=bp_chn_out_buf.MPM_buffer_ptr_64x64.left
#pragma HLS INTERFACE axis port=bp_chn_out_buf.IP_map_scu_ptr_32x32.map_scu_top_left
#pragma HLS INTERFACE axis port=bp_chn_out_buf.IP_map_scu_ptr_32x32.map_scu_top_left_new
#pragma HLS INTERFACE axis port=bp_chn_out_buf.IP_map_scu_ptr_64x64.map_scu_top_left
#pragma HLS INTERFACE axis port=bp_chn_out_buf.IP_map_scu_ptr_64x64.map_scu_top_left_new
#endif //MD_CHANNEL_DEBUG
#pragma HLS INTERFACE axis port=pu_out_buf[0].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[1].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[2].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[3].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[4].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[5].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[6].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[7].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[8].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[9].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[10].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[11].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[12].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[13].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[14].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[15].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[16].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[17].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[18].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[19].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[20].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[21].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[22].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[23].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[24].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[25].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[26].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[27].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[28].line_buffer.y0
#pragma HLS INTERFACE axis port=pu_out_buf[0].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[1].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[2].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[3].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[4].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[5].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[6].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[7].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[8].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[9].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[10].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[11].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[12].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[13].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[14].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[15].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[16].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[17].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[18].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[19].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[20].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[21].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[22].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[23].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[24].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[25].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[26].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[27].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[28].cost_temp
#pragma HLS INTERFACE axis port=pu_out_buf[16].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[17].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[18].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[19].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[20].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[21].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[22].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[23].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[24].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[25].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[26].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[27].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[28].cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[16].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[17].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[18].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[19].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[20].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[21].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[22].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[23].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[24].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[25].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[26].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[27].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[28].curr_cu_data_temp.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[16].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[17].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[18].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[19].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[20].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[21].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[22].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[23].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[24].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[25].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[26].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[27].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[28].curr_cu_data_temp_64.cu_qp_group_pred_qp
#pragma HLS INTERFACE axis port=pu_out_buf[0].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[1].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[2].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[3].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[4].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[5].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[6].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[7].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[8].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[9].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[10].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[11].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[12].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[13].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[14].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[15].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[16].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[17].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[18].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[19].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[20].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[21].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[22].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[23].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[24].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[25].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[26].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[27].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[28].split_mode
#pragma HLS INTERFACE axis port=pu_out_buf[0].tag
#pragma HLS INTERFACE axis port=pu_out_buf[1].tag
#pragma HLS INTERFACE axis port=pu_out_buf[2].tag
#pragma HLS INTERFACE axis port=pu_out_buf[3].tag
#pragma HLS INTERFACE axis port=pu_out_buf[4].tag
#pragma HLS INTERFACE axis port=pu_out_buf[5].tag
#pragma HLS INTERFACE axis port=pu_out_buf[6].tag
#pragma HLS INTERFACE axis port=pu_out_buf[7].tag
#pragma HLS INTERFACE axis port=pu_out_buf[8].tag
#pragma HLS INTERFACE axis port=pu_out_buf[9].tag
#pragma HLS INTERFACE axis port=pu_out_buf[10].tag
#pragma HLS INTERFACE axis port=pu_out_buf[11].tag
#pragma HLS INTERFACE axis port=pu_out_buf[12].tag
#pragma HLS INTERFACE axis port=pu_out_buf[13].tag
#pragma HLS INTERFACE axis port=pu_out_buf[14].tag
#pragma HLS INTERFACE axis port=pu_out_buf[15].tag
#pragma HLS INTERFACE axis port=pu_out_buf[16].tag
#pragma HLS INTERFACE axis port=pu_out_buf[17].tag
#pragma HLS INTERFACE axis port=pu_out_buf[18].tag
#pragma HLS INTERFACE axis port=pu_out_buf[19].tag
#pragma HLS INTERFACE axis port=pu_out_buf[20].tag
#pragma HLS INTERFACE axis port=pu_out_buf[21].tag
#pragma HLS INTERFACE axis port=pu_out_buf[22].tag
#pragma HLS INTERFACE axis port=pu_out_buf[23].tag
#pragma HLS INTERFACE axis port=pu_out_buf[24].tag
#pragma HLS INTERFACE axis port=pu_out_buf[25].tag
#pragma HLS INTERFACE axis port=pu_out_buf[26].tag
#pragma HLS INTERFACE axis port=pu_out_buf[27].tag
#pragma HLS INTERFACE axis port=pu_out_buf[28].tag
#else
#pragma HLS ARRAY_PARTITION variable=ram_in complete dim=1
#pragma HLS RESOURCE variable=ram_in[0].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].line_buffer_y0
#pragma HLS RESOURCE variable=ram_in[6].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].line_buffer_y0 core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[0].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[6].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].cost_temp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[0].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[6].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[0].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[6].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].curr_cu_data_temp_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[0].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[6].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].curr_cu_data_temp_64_cu_qp_group_pred_qp core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[0].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[6].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].split_mode core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[0].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[1].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[2].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[3].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[4].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[5].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[6].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[7].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[8].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[9].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[10].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[11].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[12].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[13].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[14].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[15].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[16].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[17].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[18].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[19].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[20].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[21].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[22].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[23].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[24].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[25].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[26].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[27].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[28].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[29].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[30].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[31].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[32].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[33].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[34].tag core=RAM_1P_BRAM
#pragma HLS RESOURCE variable=ram_in[35].tag core=RAM_1P_BRAM

#endif !//STORE_AXIS_ADAPTER
#endif // store_interface

	InstructionRegister<Instruction> ir;
	ir.init();

	bool done = false;
	LOOP_SC:while (!done)
	{
#pragma HLS LOOP_TRIPCOUNT max=16
		done = store_core(ir, mem_store, status, reg, push_data, config
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
