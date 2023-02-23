#include "asm.h"
#include "md1.h"

void asm_pix_xy(int* asm_pix_x, int* asm_pix_y, Slice_Level_Parameters* ctu_ctrl_input, CTX_FETCH* fetch_ctx)
{
	static int asm_core_x_lcu_tmp, asm_core_y_lcu_tmp;
	static int asm_core_x_pel, asm_core_y_pel;

	if (ctu_ctrl_input->slice_enable) {
		asm_core_x_lcu_tmp = 0;
		asm_core_y_lcu_tmp = 0;
	}
	else {
		asm_core_x_lcu_tmp++;
	}

	int asm_img_width = fetch_ctx->ctx_info_pic_width;
	int asm_img_height = fetch_ctx->ctx_info_pic_height;
	int asm_lcu_width = (asm_img_width + 63) >> 6;

	if (asm_core_x_lcu_tmp == asm_lcu_width) {
		asm_core_y_lcu_tmp++;
		asm_core_x_lcu_tmp = 0;
	}

	asm_core_x_pel = asm_core_x_lcu_tmp << fetch_ctx->log2_max_cuwh;
	asm_core_y_pel = asm_core_y_lcu_tmp << fetch_ctx->log2_max_cuwh;

	*asm_pix_x = asm_core_x_lcu_tmp << fetch_ctx->log2_max_cuwh;
	*asm_pix_y = asm_core_y_lcu_tmp << fetch_ctx->log2_max_cuwh;
}

void asm_qtqt(int x0, int y0, int log2w0, int log2h0, int pic_w, int pic_h, int log2_culine, int min_qt_size, Instruction*& load, Instruction*& pu, Instruction*& store)
{
	Assembler asmr;
	int tag_chn = 1, tag_pu = 1;
	int idx_chn = 0, idx_bp = 0;

	std::vector<int> mem_ptr2{ 8,9,10,11,20,21,22,23,32,33,34,35,44,45,46,47 };
	std::vector<int> mem_ptr1{ 0,12,24,36 };
	int mem_ptr0 = 0;
	int mem_ptr_ptr1 = 0, mem_ptr_ptr2 = 0;

	STATIC_C MD_FW md_fw;
	md_fw.log2_culine = log2_culine;
	md_fw.min_qt_size = min_qt_size;
	md_fw.pic_width = pic_w;
	md_fw.pic_height = pic_h;

	int w0 = 1 << log2w0;
	int h0 = 1 << log2h0;
	int cup0 = 0;
	int cud0 = 0;
	int qtd0 = 0;
	int btd0 = 0;

	U1 do_split = 1;
	md1_com_check_split_mode_top(x0, y0, w0, h0, pic_w, pic_h, SPLIT_QUAD, &do_split);

	if (do_split)
	{
		asmr.init(idx_chn, tag_chn, x0, y0);

		COM_SPLIT_STRUCT split_struct1;
		com_split_get_part_structure_only_qt(x0, y0, w0, h0, cup0, cud0, md_fw.log2_culine, &split_struct1);

		for (U3 i = 0; i < split_struct1.part_count; i++)
		{
			com_check_split_mode_only_qt(split_struct1.x_pos[i], split_struct1.y_pos[i], &md_fw, split_struct1.split_allow[i], split_struct1.log_cuw[i],
				split_struct1.log_cuh[i], btd0, &split_struct1.num_split[i]);
		}

		for (int part_num1 = 0; part_num1 < split_struct1.part_count; ++part_num1)
		{
			int x1 = split_struct1.x_pos[part_num1];
			int y1 = split_struct1.y_pos[part_num1];
			int log2w1 = split_struct1.log_cuw[part_num1];
			int log2h1 = split_struct1.log_cuh[part_num1];
			int w1 = 1 << log2w1;
			int h1 = 1 << log2h1;
			int cup1 = split_struct1.cup[part_num1];
			int cud1 = split_struct1.cud;
			int qtd1 = qtd0 + 1;
			int btd1 = btd0;

			if (split_struct1.split_allow[part_num1][SPLIT_QUAD])
			{
				COM_SPLIT_STRUCT split_struct2;
				get_split_structure_COM_combine_only_qt<32>(&md_fw, x1, y1, cup1, cud1,
					btd1, w1, h1, &split_struct2);

				for (int part_num2 = 0; part_num2 < split_struct2.part_count; ++part_num2)
				{
					int x2 = split_struct2.x_pos[part_num2];
					int y2 = split_struct2.y_pos[part_num2];
					int log2w2 = split_struct2.log_cuw[part_num2];
					int log2h2 = split_struct2.log_cuh[part_num2];
					int cup2 = split_struct2.cup[part_num2];
					int cud2 = split_struct2.cud;
					int qtd2 = qtd1 + 1;
					int btd2 = btd1;

					std::map<int, bool> pus{
						{KERNEL_16X16, split_struct2.split_allow[part_num2][NO_SPLIT]},
#if !ONLY_NS_16X16
						{KERNEL_8X16, split_struct2.split_allow[part_num2][SPLIT_BI_VER]},
						{KERNEL_16X8, split_struct2.split_allow[part_num2][SPLIT_BI_HOR]},
						{KERNEL_8X8, split_struct2.split_allow[part_num2][SPLIT_QUAD]}
#endif
					};

					asmr.load(idx_chn, tag_chn++, pus)
						.kernel(split_struct2.split_allow[part_num2][NO_SPLIT], KERNEL_16X16, tag_pu
							, NO_SPLIT, btd2, NO_MODE_CONS, cud2, cup2, log2w2, log2h2, qtd2, TREE_LC, x2, y2)
#if !ONLY_NS_16X16
						.kernel(split_struct2.split_allow[part_num2][SPLIT_BI_VER], KERNEL_8X16, tag_pu
							, SPLIT_BI_VER, btd2, NO_MODE_CONS, cud2, cup2, log2w2, log2h2, qtd2, TREE_LC, x2, y2)
						.kernel(split_struct2.split_allow[part_num2][SPLIT_BI_HOR], KERNEL_16X8, tag_pu
							, SPLIT_BI_HOR, btd2, NO_MODE_CONS, cud2, cup2, log2w2, log2h2, qtd2, TREE_LC, x2, y2)
						.kernel(split_struct2.split_allow[part_num2][SPLIT_QUAD], KERNEL_8X8, tag_pu
							, SPLIT_QUAD, btd2, NO_MODE_CONS, cud2, cup2, log2w2, log2h2, qtd2, TREE_LC, x2, y2)
#endif
						;
					asmr.compare(idx_chn, mem_ptr2[mem_ptr_ptr2++], tag_chn, tag_pu++, x2, y2, cud2, log2w2, log2h2, pus);
				}
				asmr.bypass(idx_chn, idx_bp, tag_chn++, tag_pu, x1, y1, log2w1, log2h1, cud1, btd1, qtd1, SPLIT_QUAD);
				asmr.compare(idx_chn, mem_ptr1[mem_ptr_ptr1++], tag_chn, tag_pu++, x1, y1, cud1, log2w1, log2h1, {
						{BYPASS + idx_bp, split_struct1.split_allow[part_num1][SPLIT_QUAD]}
					});
			}
		}
		asmr.bypass(idx_chn, idx_bp, tag_chn++, tag_pu, x0, y0, log2w0, log2h0, cud0, btd0, qtd0, SPLIT_QUAD);
		asmr.compare(idx_chn, mem_ptr0, tag_chn, tag_pu++, x0, y0, cud0, log2w0, log2h0, { {BYPASS + idx_bp, do_split} })
			.output(idx_chn, tag_chn, x0, y0);
	}
	asmr.done();

	asmr.compile(load, pu, store);
}

void md_run_64_ordered(int x, int y, int pic_w, int pic_h, Instruction*& load, Instruction*& pu, Instruction*& store)
{
	int cuw = ((pic_w - 1) >> MIN_CU_LOG2) + 1;
	int cuh = ((pic_h - 1) >> MIN_CU_LOG2) + 1;

	Assembler asmr;
	NodeInfo node_info[45] = {
		{BYPASS_0		,NO_SPLIT		,0,0,0,6,6}, // node 0: 64x64
		{BYPASS_0		,SPLIT_QUAD		,1,1,0,5,5}, // node 1: 64x64->32x32
		{KERNEL_32X32	,NO_SPLIT		,1,1,0,5,5}, // node 2: 64x64->32x32
		{BYPASS_0		,SPLIT_BI_VER	,2,1,1,4,5}, // node 3: 64x64->32x32->16x32
		{KERNEL_16X32_1	,NO_SPLIT		,2,1,1,4,5}, // node 4: 64x64->32x32->16x32
		{BYPASS_0		,SPLIT_BI_HOR	,3,1,2,4,4}, // node 5: 64x64->32x32->16x32->16x16
		{KERNEL_16X16_1	,NO_SPLIT		,3,1,2,4,4}, // node 6: 64x64->32x32->16x32->16x16
		{KERNEL_8X16_1	,SPLIT_BI_VER	,3,1,2,4,4}, // node 7: 64x64->32x32->16x32->16x16->8x16
		{KERNEL_16X8_1	,SPLIT_BI_HOR	,3,1,2,4,4}, // node 8: 64x64->32x32->16x32->16x16->16x8
		{BYPASS_1		,SPLIT_BI_HOR	,2,1,1,5,4}, // node 9: 64x64->32x32->32x16
		{KERNEL_32X16_1	,NO_SPLIT		,2,1,1,5,4}, // node 10: 64x64->32x32->32x16
		{BYPASS_1		,SPLIT_BI_VER	,3,1,2,4,4}, // node 11: 64x64->32x32->32x16->16x16
		{KERNEL_16X16_2	,NO_SPLIT		,3,1,2,4,4}, // node 12: 64x64->32x32->32x16->16x16
		{KERNEL_8X16_2	,SPLIT_BI_VER	,3,1,2,4,4}, // node 13: 64x64->32x32->32x16->16x16->8x16
		{KERNEL_16X8_2	,SPLIT_BI_HOR	,3,1,2,4,4}, // node 14: 64x64->32x32->32x16->16x16->16x8
		{BYPASS_2		,SPLIT_QUAD		,2,2,0,4,4}, // node 15: 64x64->32x32->16x16
		{KERNEL_16X16_3	,NO_SPLIT		,2,2,0,4,4}, // node 16: 64x64->32x32->16x16
		{KERNEL_8X16_3	,SPLIT_BI_VER	,2,2,0,4,4}, // node 17: 64x64->32x32->16x16->8x16
		{KERNEL_16X8_3	,SPLIT_BI_HOR	,2,2,0,4,4}, // node 18: 64x64->32x32->16x16->16x8
		{KERNEL_8X8_1	,SPLIT_QUAD		,2,2,0,4,4}, // node 19: 64x64->32x32->16x16->8x8
		{KERNEL_64X64	,NO_SPLIT		,0,0,0,6,6}, // node 20: 64x64
		{BYPASS_3		,SPLIT_BI_VER	,1,0,1,5,6}, // node 21: 64x64->32x64
		{-1				,NO_SPLIT		,1,0,1,5,6}, // node 22: 64x64->32x64
		{KERNEL_32X64	,NO_SPLIT		,1,0,1,5,6}, // node 23: 64x64->32x64
		{BYPASS_3		,SPLIT_BI_VER	,2,0,2,4,6}, // node 24: 64x64->32x64->16x64
		{KERNEL_16X64	,NO_SPLIT		,2,0,2,4,6}, // node 25: 64x64->32x64->16x64
		{BYPASS_3		,SPLIT_BI_HOR	,3,0,3,4,5}, // node 26: 64x64->32x64->16x64->16x32
		{KERNEL_8X64	,NO_SPLIT		,3,0,3,3,6}, // node 27: 64x64->32x64->16x64->8x64
		{KERNEL_16X32_2	,NO_SPLIT		,3,0,3,4,5}, // node 28: 64x64->32x64->16x64->16x32
		{BYPASS_3		,SPLIT_BI_HOR	,4,0,4,4,4}, // node 29: 64x64->32x64->16x64->16x32->16x16
		{KERNEL_16X16_4	,NO_SPLIT		,4,0,4,4,4}, // node 30: 64x64->32x64->16x64->16x32->16x16
		{KERNEL_8X16_4	,SPLIT_BI_VER	,4,0,4,4,4}, // node 31: 64x64->32x64->16x64->16x32->16x16->8x16
		{KERNEL_16X8_4	,SPLIT_BI_HOR	,4,0,4,4,4}, // node 32: 64x64->32x64->16x64->16x32->16x16->16x8
		{BYPASS_4		,SPLIT_BI_HOR	,1,0,1,6,5}, // node 33: 64x64->64x32
		{-1				,NO_SPLIT		,1,0,1,6,5}, // node 34: 64x64->64x32
		{KERNEL_64X32	,NO_SPLIT		,1,0,1,6,5}, // node 35: 64x64->64x32
		{BYPASS_4		,SPLIT_BI_HOR	,2,0,2,6,4}, // node 36: 64x64->64x32->64x16
		{KERNEL_64X16	,NO_SPLIT		,2,0,2,6,4}, // node 37: 64x64->64x32->64x16
		{BYPASS_4		,SPLIT_BI_VER	,3,0,3,5,4}, // node 38: 64x64->64x32->64x16->32x16
		{KERNEL_64X8	,NO_SPLIT		,3,0,3,6,3}, // node 39: 64x64->64x32->64x16->64x8
		{KERNEL_32X16_2	,NO_SPLIT		,3,0,3,5,4}, // node 40: 64x64->64x32->64x16->32x16
		{BYPASS_4		,SPLIT_BI_VER	,4,0,4,4,4}, // node 41: 64x64->64x32->64x16->32x16->16x16
		{KERNEL_16X16_5	,NO_SPLIT		,4,0,4,4,4}, // node 42: 64x64->64x32->64x16->32x16->16x16
		{KERNEL_8X16_5	,SPLIT_BI_VER	,4,0,4,4,4}, // node 43: 64x64->64x32->64x16->32x16->16x16->8x16
		{KERNEL_16X8_5	,SPLIT_BI_HOR	,4,0,4,4,4}, // node 44: 64x64->64x32->64x16->32x16->16x16->16x8
	};

	asmr.init(0, 1, x, y)
		// x=0,y=0,w=64,h=64,node=0
		.load_kernel(CHANNEL_0, 1, 1, NO_MODE_CONS, 0, TREE_LC, 0, 0, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19],node_info[20],node_info[23],node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32],node_info[35],node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })

		// ******************************************* SPLIT_QUAD_0 *************************************************
		// 
		// x=0,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 0, 2, 1, 0, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 2, 2, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 1, 3, 2, 0, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 3, 1, 0, 0, node_info[5])
		.compare(CHANNEL_0, 0, 4, 1, 0, 0, node_info[3], { node_info[4],node_info[5] })
		// x=16,y=0,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 4, 3, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=16,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 2, 5, 3, 16, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 5, 4, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 3, 6, 4, 16, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 6, 3, 16, 0, node_info[5])
		.compare(CHANNEL_0, 2, 7, 3, 16, 0, node_info[3], { node_info[4],node_info[5] })

		// x=0,y=0,w=16,h=16,node=11
		.compare(CHANNEL_1, 4, 8, 1, 0, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=0,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 8, 2, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 5, 9, 2, 16, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=0,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 9, 1, 0, 0, node_info[11])
		.compare(CHANNEL_1, 4, 10, 1, 0, 0, node_info[9], { node_info[10],node_info[11] })
		// x=0,y=16,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 10, 3, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=0,y=16,w=16,h=16,node=11
		.compare(CHANNEL_1, 6, 11, 3, 0, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=16,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 11, 4, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 7, 12, 4, 16, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=16,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 12, 3, 0, 16, node_info[11])
		.compare(CHANNEL_1, 6, 13, 3, 0, 16, node_info[9], { node_info[10],node_info[11] })

		// x=0,y=0,w=16,h=16,node=15
		.compare(CHANNEL_2, 8, 14, 1, 0, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=0,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 14, 2, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 9, 15, 2, 16, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 15, 3, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 10, 16, 3, 0, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 16, 4, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 11, 17, 4, 16, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })

		// x=0,y=0,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 7, 1, 0, 0, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 13, 1, 0, 0, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 17, 1, 0, 0, node_info[15])
		.compare(CHANNEL_0, 0, 18, 1, 0, 0, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })

		// ******************************************* SPLIT_QUAD_1 *************************************************
		// 
		// x=32,y=0,w=32,h=32,node=1
		.load_kernel(CHANNEL_0, 18, 5, NO_MODE_CONS, 8, TREE_LC, 32, 0, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 12, 19, 5, 32, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 19, 6, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 13, 20, 6, 32, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 20, 5, 32, 0, node_info[5])
		.compare(CHANNEL_0, 12, 21, 5, 32, 0, node_info[3], { node_info[4],node_info[5] })
		// x=48,y=0,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 21, 7, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=48,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 14, 22, 7, 48, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 22, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 15, 23, 8, 48, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 23, 5, 48, 0, node_info[5])
		.compare(CHANNEL_0, 14, 24, 5, 48, 0, node_info[3], { node_info[4],node_info[5] })

		// x=32,y=0,w=16,h=16,node=11
		.compare(CHANNEL_1, 16, 25, 5, 32, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=0,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 25, 6, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 17, 26, 6, 48, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=0,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 26, 5, 32, 0, node_info[11])
		.compare(CHANNEL_1, 16, 27, 5, 32, 0, node_info[9], { node_info[10],node_info[11] })
		// x=32,y=16,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 27, 7, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=32,y=16,w=16,h=16,node=11
		.compare(CHANNEL_1, 18, 28, 7, 32, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=16,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 28, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 19, 29, 8, 48, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=16,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 29, 7, 32, 16, node_info[11])
		.compare(CHANNEL_1, 18, 30, 7, 32, 16, node_info[9], { node_info[10],node_info[11] })

		// x=32,y=0,w=16,h=16,node=15
		.compare(CHANNEL_2, 20, 31, 5, 32, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=0,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 31, 6, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 21, 32, 6, 48, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 32, 7, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 0, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 22, 33, 7, 32, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 33, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 23, 34, 8, 48, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })

		// x=32,y=0,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 24, 5, 32, 0, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 32, 5, 32, 0, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 34, 5, 32, 0, node_info[15])
		.compare(CHANNEL_0, 12, 18, 35, 32, 0, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })

		// ******************************************* SPLIT_QUAD_2 *************************************************
		// 
		// x=0,y=32,w=32,h=32,node=1
		.load_kernel(CHANNEL_0, 35, 9, NO_MODE_CONS, 8 * cuw, TREE_LC, 0, 32, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 24, 36, 9, 0, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 36, 10, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 25, 37, 10, 0, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 37, 9, 0, 32, node_info[5])
		.compare(CHANNEL_0, 24, 38, 9, 0, 32, node_info[3], { node_info[4],node_info[5] })
		// x=16,y=32,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 38, 11, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=16,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 26, 38, 11, 16, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 38, 12, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 27, 39, 12, 16, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 39, 11, 16, 32, node_info[5])
		.compare(CHANNEL_0, 26, 40, 11, 16, 32, node_info[3], { node_info[4],node_info[5] })

		// x=0,y=32,w=16,h=16,node=11
		.compare(CHANNEL_1, 28, 41, 9, 0, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=32,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 41, 10, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 29, 42, 10, 16, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=32,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 42, 9, 0, 32, node_info[11])
		.compare(CHANNEL_1, 28, 43, 9, 0, 32, node_info[9], { node_info[10],node_info[11] })
		// x=0,y=48,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 43, 11, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=0,y=48,w=16,h=16,node=11
		.compare(CHANNEL_1, 30, 44, 11, 0, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=48,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 44, 12, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 31, 45, 12, 16, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=48,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 45, 11, 0, 48, node_info[11])
		.compare(CHANNEL_1, 30, 46, 11, 0, 48, node_info[9], { node_info[10],node_info[11] })

		// x=0,y=32,w=16,h=16,node=15
		.compare(CHANNEL_2, 32, 47, 9, 0, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=32,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 47, 10, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 33, 48, 10, 16, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 48, 11, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 34, 49, 11, 0, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 49, 12, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 35, 50, 12, 16, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })

		// x=0,y=32,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 40, 9, 0, 32, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 46, 9, 0, 32, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 50, 9, 0, 32, node_info[15])
		.compare(CHANNEL_0, 24, 51, 9, 0, 32, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })

		// ******************************************* SPLIT_QUAD_3 *************************************************
		// 
		// x=32,y=32,w=32,h=32,node=1
		.load_kernel(CHANNEL_0, 51, 13, NO_MODE_CONS, 8 * cuw + 8, TREE_LC, 32, 32, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 36, 52, 13, 32, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 52, 14, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 37, 53, 14, 32, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 53, 13, 32, 32, node_info[5])
		.compare(CHANNEL_0, 36, 54, 13, 32, 32, node_info[3], { node_info[4],node_info[5] })
		// x=48,y=32,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 54, 15, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=48,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 38, 55, 15, 48, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 55, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 39, 56, 16, 48, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 56, 15, 48, 32, node_info[5])
		.compare(CHANNEL_0, 38, 57, 15, 48, 32, node_info[3], { node_info[4],node_info[5] })

		// x=32,y=32,w=16,h=16,node=11
		.compare(CHANNEL_1, 40, 58, 13, 32, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=32,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 58, 14, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 41, 59, 14, 48, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=32,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 59, 13, 32, 32, node_info[11])
		.compare(CHANNEL_1, 40, 60, 13, 32, 32, node_info[9], { node_info[10],node_info[11] })
		// x=32,y=48,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 60, 15, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=32,y=48,w=16,h=16,node=11
		.compare(CHANNEL_1, 42, 61, 15, 32, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=48,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 61, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 43, 62, 16, 48, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=48,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 62, 15, 32, 48, node_info[11])
		.compare(CHANNEL_1, 42, 63, 15, 32, 48, node_info[9], { node_info[10],node_info[11] })

		// x=32,y=32,w=16,h=16,node=15
		.compare(CHANNEL_2, 44, 64, 13, 32, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=32,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 64, 14, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 45, 65, 14, 48, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 65, 15, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 46, 66, 15, 32, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 66, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 47, 67, 16, 48, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })

		// x=32,y=32,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 57, 13, 32, 32, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 63, 13, 32, 32, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 67, 13, 32, 32, node_info[15])
		.compare(CHANNEL_0, 36, 68, 13, 32, 32, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })

		// ******************************************* SPLIT_BI_VER_0 *************************************************
		// 
		// x=0,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 48, 69, 1, 0, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 69, 2, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 49, 70, 2, 0, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 70, 1, 0, 0, node_info[29])
		.compare(CHANNEL_3, 48, 71, 1, 0, 0, node_info[26], { node_info[28], node_info[29] })

		// x=0,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 71, 3, NO_MODE_CONS, 8 * cuw, TREE_LC, 0, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=0,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 50, 72, 3, 0, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 72, 4, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 51, 73, 4, 0, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 73, 3, 0, 32, node_info[29])
		.compare(CHANNEL_3, 50, 74, 3, 0, 32, node_info[26], { node_info[28], node_info[29] })

		// x=0,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 74, 1, 0, 0, node_info[26])
		.compare(CHANNEL_3, 48, 75, 1, 0, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })

		// x=16,y=0,w=16,h=64,node=24
		.load_kernel(CHANNEL_3, 75, 5, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=16,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 52, 76, 5, 16, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 76, 6, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 53, 77, 6, 16, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 77, 5, 16, 0, node_info[29])
		.compare(CHANNEL_3, 52, 78, 5, 16, 0, node_info[26], { node_info[28], node_info[29] })

		// x=16,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 78, 7, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=16,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 54, 79, 7, 16, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 79, 8, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 55, 80, 8, 16, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 80, 7, 16, 32, node_info[29])
		.compare(CHANNEL_3, 54, 81, 7, 16, 32, node_info[26], { node_info[28], node_info[29] })

		// x=16,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 81, 5, 16, 0, node_info[26])
		.compare(CHANNEL_3, 52, 82, 5, 16, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })

		// x=0,y=0,w=32,h=64,node=21
		.bypass(CHANNEL_3, BYPASS_3, 82, 1, 0, 0, node_info[24])
		.compare(CHANNEL_3, 48, 83, 1, 0, 0, node_info[21], { node_info[23],node_info[24] })

		// ******************************************* SPLIT_BI_VER_1 *************************************************
		// 
		// x=32,y=0,w=32,h=64,node=21
		.load_kernel(CHANNEL_3, 83, 9, NO_MODE_CONS, 8, TREE_LC, 32, 0, { node_info[23],node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=32,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 56, 84, 9, 32, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 84, 10, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 57, 85, 10, 32, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 86, 9, 32, 0, node_info[29])
		.compare(CHANNEL_3, 56, 87, 9, 32, 0, node_info[26], { node_info[28], node_info[29] })

		// x=32,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 87, 11, NO_MODE_CONS, 8 * cuw + 8, TREE_LC, 32, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=32,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 58, 88, 11, 32, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 88, 12, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 59, 89, 12, 32, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 89, 11, 32, 32, node_info[29])
		.compare(CHANNEL_3, 58, 90, 11, 32, 32, node_info[26], { node_info[28], node_info[29] })

		// x=32,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 90, 9, 32, 0, node_info[26])
		.compare(CHANNEL_3, 56, 91, 9, 32, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })

		// x=48,y=0,w=16,h=64,node=24
		.load_kernel(CHANNEL_3, 91, 13, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=48,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 60, 92, 13, 48, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 92, 14, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 61, 93, 14, 48, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 93, 13, 48, 0, node_info[29])
		.compare(CHANNEL_3, 60, 94, 13, 48, 0, node_info[26], { node_info[28], node_info[29] })

		// x=48,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 94, 15, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=48,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 62, 95, 15, 48, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 95, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 63, 96, 16, 48, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 96, 15, 48, 32, node_info[29])
		.compare(CHANNEL_3, 62, 97, 15, 48, 32, node_info[26], { node_info[28], node_info[29] })

		// x=48,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 97, 12, 48, 0, node_info[26])
		.compare(CHANNEL_3, 60, 98, 12, 48, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })

		// x=32,y=0,w=32,h=64,node=21
		.bypass(CHANNEL_3, BYPASS_3, 98, 9, 32, 0, node_info[24])
		.compare(CHANNEL_3, 56, 99, 9, 32, 0, node_info[21], { node_info[23],node_info[24] })

		// ******************************************* SPLIT_BI_HOR_0 *************************************************
		// 
		// x=0,y=0,w=16,h=16,node=41
		.compare(CHANNEL_4, 64, 100, 1, 0, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=16,y=0,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 100, 2, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 65, 101, 2, 16, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=0,y=0,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 101, 1, 0, 0, node_info[41])
		.compare(CHANNEL_4, 64, 102, 1, 0, 0, node_info[38], { node_info[40],node_info[41] })

		// x=32,y=0,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 102, 3, NO_MODE_CONS, 8, TREE_LC, 32, 0, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=0,w=16,h=16,node=41
		.compare(CHANNEL_4, 66, 103, 3, 32, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=48,y=0,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 103, 4, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 67, 104, 4, 48, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=32,y=0,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 104, 3, 32, 0, node_info[41])
		.compare(CHANNEL_4, 66, 105, 3, 32, 0, node_info[38], { node_info[40],node_info[41] })

		// x=0,y=0,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 105, 1, 0, 0, node_info[38])
		.compare(CHANNEL_4, 64, 106, 1, 0, 0, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// x=0,y=16,w=64,h=16,node=36
		.load_kernel(CHANNEL_4, 107, 5, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=16,w=16,h=16,node=41
		.compare(CHANNEL_4, 68, 108, 5, 0, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=16,y=16,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 108, 6, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 69, 109, 6, 16, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=0,y=16,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 109, 5, 0, 16, node_info[41])
		.compare(CHANNEL_4, 68, 110, 5, 0, 16, node_info[38], { node_info[40],node_info[41] })

		// x=32,y=16,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 110, 7, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=16,w=16,h=16,node=41
		.compare(CHANNEL_4, 70, 111, 7, 32, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=48,y=16,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 111, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 71, 112, 8, 48, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=32,y=16,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 112, 7, 32, 16, node_info[41])
		.compare(CHANNEL_4, 70, 113, 7, 32, 16, node_info[38], { node_info[40],node_info[41] })

		// x=0,y=16,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 113, 5, 0, 16, node_info[38])
		.compare(CHANNEL_4, 68, 114, 5, 0, 16, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// x=0,y=0,w=64,h=32,node=33
		.bypass(CHANNEL_4, BYPASS_4, 114, 1, 0, 0, node_info[36])
		.compare(CHANNEL_4, 64, 115, 1, 0, 0, node_info[33], { node_info[35],node_info[36] })

		// ******************************************* SPLIT_BI_HOR_1 *************************************************
		// 
		// x=0,y=32,w=64,h=32,node=33
		.load_kernel(CHANNEL_4, 115, 9, NO_MODE_CONS, 8 * cuw, TREE_LC, 0, 32, { node_info[35],node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=32,w=16,h=16,node=41
		.compare(CHANNEL_4, 72, 116, 9, 0, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=16,y=32,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 116, 10, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 73, 117, 10, 16, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=0,y=32,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 117, 9, 0, 32, node_info[41])
		.compare(CHANNEL_4, 72, 118, 9, 0, 32, node_info[38], { node_info[40],node_info[41] })

		// x=32,y=32,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 118, 11, NO_MODE_CONS, 8 * cuw + 8, TREE_LC, 32, 32, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=32,w=16,h=16,node=41
		.compare(CHANNEL_4, 74, 119, 11, 32, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=48,y=32,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 119, 12, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 75, 120, 12, 48, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=32,y=32,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 120, 11, 32, 32, node_info[41])
		.compare(CHANNEL_4, 74, 121, 11, 32, 32, node_info[38], { node_info[40],node_info[41] })

		// x=0,y=32,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 121, 9, 0, 32, node_info[38])
		.compare(CHANNEL_4, 72, 122, 9, 0, 32, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// x=0,y=48,w=64,h=16,node=36
		.load_kernel(CHANNEL_4, 122, 13, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=48,w=16,h=16,node=41
		.compare(CHANNEL_4, 76, 123, 13, 0, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=16,y=48,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 123, 14, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 77, 124, 14, 16, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=0,y=48,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 124, 13, 0, 48, node_info[41])
		.compare(CHANNEL_4, 76, 125, 13, 0, 48, node_info[38], { node_info[40],node_info[41] })

		// x=32,y=48,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 125, 15, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=48,w=16,h=16,node=41
		.compare(CHANNEL_4, 78, 126, 15, 32, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=48,y=48,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 126, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 79, 127, 16, 48, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })
		// x=32,y=48,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 127, 15, 32, 48, node_info[41])
		.compare(CHANNEL_4, 78, 128, 15, 32, 48, node_info[38], { node_info[40],node_info[41] })

		// x=0,y=48,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 128, 13, 0, 48, node_info[38])
		.compare(CHANNEL_4, 76, 129, 13, 0, 48, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// x=0,y=32,w=64,h=32,node=33
		.bypass(CHANNEL_4, BYPASS_4, 129, 9, 0, 32, node_info[36])
		.compare(CHANNEL_4, 72, 130, 9, 0, 32, node_info[33], { node_info[35],node_info[36] })

		// x=0,y=0,w=64,h=64,node=0
		.bypass(CHANNEL_0, BYPASS_0, 68, 1, 0, 0, node_info[1])
		.bypass(CHANNEL_3, BYPASS_3, 99, 1, 0, 0, node_info[21])
		.bypass(CHANNEL_4, BYPASS_4, 130, 1, 0, 0, node_info[33])
		.compare(CHANNEL_0, 0, 131, 1, 0, 0, node_info[0], { node_info[1],node_info[20],node_info[21],node_info[33] })

		.output(CHANNEL_0, 131, x, y)
		.done()
		.compile(load, pu, store);
}

void md_run_64_opt(int x, int y, int pic_w, int pic_h, Instruction*& load, Instruction*& pu, Instruction*& store)
{
	int cuw = ((pic_w - 1) >> MIN_CU_LOG2) + 1;
	int cuh = ((pic_h - 1) >> MIN_CU_LOG2) + 1;

	Assembler asmr;
	NodeInfo node_info[45] = {
		{BYPASS_0		,NO_SPLIT		,0,0,0,6,6}, // node 0: 64x64
		{BYPASS_0		,SPLIT_QUAD		,1,1,0,5,5}, // node 1: 64x64->32x32
		{KERNEL_32X32	,NO_SPLIT		,1,1,0,5,5}, // node 2: 64x64->32x32
		{BYPASS_0		,SPLIT_BI_VER	,2,1,1,4,5}, // node 3: 64x64->32x32->16x32
		{KERNEL_16X32_1	,NO_SPLIT		,2,1,1,4,5}, // node 4: 64x64->32x32->16x32
		{BYPASS_0		,SPLIT_BI_HOR	,3,1,2,4,4}, // node 5: 64x64->32x32->16x32->16x16
		{KERNEL_16X16_1	,NO_SPLIT		,3,1,2,4,4}, // node 6: 64x64->32x32->16x32->16x16
		{KERNEL_8X16_1	,SPLIT_BI_VER	,3,1,2,4,4}, // node 7: 64x64->32x32->16x32->16x16->8x16
		{KERNEL_16X8_1	,SPLIT_BI_HOR	,3,1,2,4,4}, // node 8: 64x64->32x32->16x32->16x16->16x8
		{BYPASS_1		,SPLIT_BI_HOR	,2,1,1,5,4}, // node 9: 64x64->32x32->32x16
		{KERNEL_32X16_1	,NO_SPLIT		,2,1,1,5,4}, // node 10: 64x64->32x32->32x16
		{BYPASS_1		,SPLIT_BI_VER	,3,1,2,4,4}, // node 11: 64x64->32x32->32x16->16x16
		{KERNEL_16X16_2	,NO_SPLIT		,3,1,2,4,4}, // node 12: 64x64->32x32->32x16->16x16
		{KERNEL_8X16_2	,SPLIT_BI_VER	,3,1,2,4,4}, // node 13: 64x64->32x32->32x16->16x16->8x16
		{KERNEL_16X8_2	,SPLIT_BI_HOR	,3,1,2,4,4}, // node 14: 64x64->32x32->32x16->16x16->16x8
		{BYPASS_2		,SPLIT_QUAD		,2,2,0,4,4}, // node 15: 64x64->32x32->16x16
		{KERNEL_16X16_3	,NO_SPLIT		,2,2,0,4,4}, // node 16: 64x64->32x32->16x16
		{KERNEL_8X16_3	,SPLIT_BI_VER	,2,2,0,4,4}, // node 17: 64x64->32x32->16x16->8x16
		{KERNEL_16X8_3	,SPLIT_BI_HOR	,2,2,0,4,4}, // node 18: 64x64->32x32->16x16->16x8
		{KERNEL_8X8_1	,SPLIT_QUAD		,2,2,0,4,4}, // node 19: 64x64->32x32->16x16->8x8
		{KERNEL_64X64	,NO_SPLIT		,0,0,0,6,6}, // node 20: 64x64
		{BYPASS_3		,SPLIT_BI_VER	,1,0,1,5,6}, // node 21: 64x64->32x64
		{-1				,NO_SPLIT		,1,0,1,5,6}, // node 22: 64x64->32x64
		{KERNEL_32X64	,NO_SPLIT		,1,0,1,5,6}, // node 23: 64x64->32x64
		{BYPASS_3		,SPLIT_BI_VER	,2,0,2,4,6}, // node 24: 64x64->32x64->16x64
		{KERNEL_16X64	,NO_SPLIT		,2,0,2,4,6}, // node 25: 64x64->32x64->16x64
		{BYPASS_3		,SPLIT_BI_HOR	,3,0,3,4,5}, // node 26: 64x64->32x64->16x64->16x32
		{KERNEL_8X64	,NO_SPLIT		,3,0,3,3,6}, // node 27: 64x64->32x64->16x64->8x64
		{KERNEL_16X32_2	,NO_SPLIT		,3,0,3,4,5}, // node 28: 64x64->32x64->16x64->16x32
		{BYPASS_3		,SPLIT_BI_HOR	,4,0,4,4,4}, // node 29: 64x64->32x64->16x64->16x32->16x16
		{KERNEL_16X16_4	,NO_SPLIT		,4,0,4,4,4}, // node 30: 64x64->32x64->16x64->16x32->16x16
		{KERNEL_8X16_4	,SPLIT_BI_VER	,4,0,4,4,4}, // node 31: 64x64->32x64->16x64->16x32->16x16->8x16
		{KERNEL_16X8_4	,SPLIT_BI_HOR	,4,0,4,4,4}, // node 32: 64x64->32x64->16x64->16x32->16x16->16x8
		{BYPASS_4		,SPLIT_BI_HOR	,1,0,1,6,5}, // node 33: 64x64->64x32
		{-1				,NO_SPLIT		,1,0,1,6,5}, // node 34: 64x64->64x32
		{KERNEL_64X32	,NO_SPLIT		,1,0,1,6,5}, // node 35: 64x64->64x32
		{BYPASS_4		,SPLIT_BI_HOR	,2,0,2,6,4}, // node 36: 64x64->64x32->64x16
		{KERNEL_64X16	,NO_SPLIT		,2,0,2,6,4}, // node 37: 64x64->64x32->64x16
		{BYPASS_4		,SPLIT_BI_VER	,3,0,3,5,4}, // node 38: 64x64->64x32->64x16->32x16
		{KERNEL_64X8	,NO_SPLIT		,3,0,3,6,3}, // node 39: 64x64->64x32->64x16->64x8
		{KERNEL_32X16_2	,NO_SPLIT		,3,0,3,5,4}, // node 40: 64x64->64x32->64x16->32x16
		{BYPASS_4		,SPLIT_BI_VER	,4,0,4,4,4}, // node 41: 64x64->64x32->64x16->32x16->16x16
		{KERNEL_16X16_5	,NO_SPLIT		,4,0,4,4,4}, // node 42: 64x64->64x32->64x16->32x16->16x16
		{KERNEL_8X16_5	,SPLIT_BI_VER	,4,0,4,4,4}, // node 43: 64x64->64x32->64x16->32x16->16x16->8x16
		{KERNEL_16X8_5	,SPLIT_BI_HOR	,4,0,4,4,4}, // node 44: 64x64->64x32->64x16->32x16->16x16->16x8
	};

	asmr.init(0, 1, x, y)

		// ******************************************* 1st 16x16 *************************************************
		// 
		// x=0,y=0,w=64,h=64,node=0
		.load_kernel(CHANNEL_0, 1, 1, NO_MODE_CONS, 0, TREE_LC, 0, 0, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19],node_info[20],node_info[23],node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32],node_info[35],node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 0, 2, 1, 0, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=0,w=16,h=16,node=11
		.compare(CHANNEL_1, 4, 8, 1, 0, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=0,w=16,h=16,node=15
		.compare(CHANNEL_2, 8, 14, 1, 0, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 48, 69, 1, 0, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=0,w=16,h=16,node=41
		.compare(CHANNEL_4, 64, 100, 1, 0, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 2nd 16x16 *************************************************
		// 
		// x=0,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 2, 2, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 1, 3, 2, 0, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=0,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 8, 2, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 5, 9, 2, 16, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=0,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 14, 2, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 9, 15, 2, 16, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 69, 2, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 49, 70, 2, 0, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=0,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 100, 2, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 65, 101, 2, 16, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 1st 16x32/32x16 *************************************************
		// 
		// x=0,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 3, 1, 0, 0, node_info[5])
		.compare(CHANNEL_0, 0, 4, 1, 0, 0, node_info[3], { node_info[4],node_info[5] })
		// x=0,y=0,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 9, 1, 0, 0, node_info[11])
		.compare(CHANNEL_1, 4, 10, 1, 0, 0, node_info[9], { node_info[10],node_info[11] })
		// x=0,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 70, 1, 0, 0, node_info[29])
		.compare(CHANNEL_3, 48, 71, 1, 0, 0, node_info[26], { node_info[28], node_info[29] })
		// x=0,y=0,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 101, 1, 0, 0, node_info[41])
		.compare(CHANNEL_4, 64, 102, 1, 0, 0, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 3rd 16x16 *************************************************
		// 
		// x=16,y=0,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 4, 3, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=16,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 2, 5, 3, 16, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=16,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 10, 3, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[10],node_info[12],node_info[13],node_info[14] })
		// x=0,y=16,w=16,h=16,node=11
		.compare(CHANNEL_1, 6, 11, 3, 0, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 15, 3, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 10, 16, 3, 0, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 71, 3, NO_MODE_CONS, 8 * cuw, TREE_LC, 0, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=0,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 50, 72, 3, 0, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=0,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 102, 3, NO_MODE_CONS, 8, TREE_LC, 32, 0, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=0,w=16,h=16,node=41
		.compare(CHANNEL_4, 66, 103, 3, 32, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 4th 16x16 *************************************************
		// 
		// x=16,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 5, 4, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 3, 6, 4, 16, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=16,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 11, 4, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 7, 12, 4, 16, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 16, 4, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 11, 17, 4, 16, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 72, 4, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 51, 73, 4, 0, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=0,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 103, 4, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 67, 104, 4, 48, 0, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 2nd 16x32/32x16 *************************************************
		// 
		// x=16,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 6, 3, 16, 0, node_info[5])
		.compare(CHANNEL_0, 2, 7, 3, 16, 0, node_info[3], { node_info[4],node_info[5] })
		// x=0,y=16,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 12, 3, 0, 16, node_info[11])
		.compare(CHANNEL_1, 6, 13, 3, 0, 16, node_info[9], { node_info[10],node_info[11] })
		// x=0,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 73, 3, 0, 32, node_info[29])
		.compare(CHANNEL_3, 50, 74, 3, 0, 32, node_info[26], { node_info[28], node_info[29] })
		// x=32,y=0,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 104, 3, 32, 0, node_info[41])
		.compare(CHANNEL_4, 66, 105, 3, 32, 0, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 1st 32x32/16x64/64x32 *************************************************
		// 
		// x=0,y=0,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 7, 1, 0, 0, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 13, 1, 0, 0, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 17, 1, 0, 0, node_info[15])
		.compare(CHANNEL_0, 0, 18, 1, 0, 0, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })
		// x=0,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 74, 1, 0, 0, node_info[26])
		.compare(CHANNEL_3, 48, 75, 1, 0, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })
		// x=0,y=0,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 105, 1, 0, 0, node_info[38])
		.compare(CHANNEL_4, 64, 106, 1, 0, 0, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// ******************************************* 5th 16x16 *************************************************
		// 
		// x=32,y=0,w=32,h=32,node=1
		.load_kernel(CHANNEL_0, 18, 5, NO_MODE_CONS, 8, TREE_LC, 32, 0, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 12, 19, 5, 32, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=0,w=16,h=16,node=11
		.compare(CHANNEL_1, 16, 25, 5, 32, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=0,w=16,h=16,node=15
		.compare(CHANNEL_2, 20, 31, 5, 32, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=0,w=16,h=64,node=24
		.load_kernel(CHANNEL_3, 75, 5, NO_MODE_CONS, 4, TREE_LC, 16, 0, { node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=16,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 52, 76, 5, 16, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=16,w=64,h=16,node=36
		.load_kernel(CHANNEL_4, 107, 5, NO_MODE_CONS, 4 * cuw, TREE_LC, 0, 16, { node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=16,w=16,h=16,node=41
		.compare(CHANNEL_4, 68, 108, 5, 0, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 6th 16x16 *************************************************
		// 
		// x=32,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 19, 6, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 13, 20, 6, 32, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=0,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 25, 6, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 17, 26, 6, 48, 0, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=0,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 31, 6, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 21, 32, 6, 48, 0, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 76, 6, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 53, 77, 6, 16, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=16,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 108, 6, NO_MODE_CONS, 4 * cuw + 4, TREE_LC, 16, 16, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 69, 109, 6, 16, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 3rd 16x32/32x16 *************************************************
		// 
		// x=32,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 20, 5, 32, 0, node_info[5])
		.compare(CHANNEL_0, 12, 21, 5, 32, 0, node_info[3], { node_info[4],node_info[5] })
		// x=32,y=0,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 26, 5, 32, 0, node_info[11])
		.compare(CHANNEL_1, 16, 27, 5, 32, 0, node_info[9], { node_info[10],node_info[11] })
		// x=16,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 77, 5, 16, 0, node_info[29])
		.compare(CHANNEL_3, 52, 78, 5, 16, 0, node_info[26], { node_info[28], node_info[29] })
		// x=0,y=16,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 109, 5, 0, 16, node_info[41])
		.compare(CHANNEL_4, 68, 110, 5, 0, 16, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 7th 16x16 *************************************************
		// 
		// x=48,y=0,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 21, 7, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=48,y=0,w=16,h=16,node=5
		.compare(CHANNEL_0, 14, 22, 7, 48, 0, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=16,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 27, 7, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=32,y=16,w=16,h=16,node=11
		.compare(CHANNEL_1, 18, 28, 7, 32, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 32, 7, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 0, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 22, 33, 7, 32, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 78, 7, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=16,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 54, 79, 7, 16, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=16,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 110, 7, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=16,w=16,h=16,node=41
		.compare(CHANNEL_4, 70, 111, 7, 32, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 8th 16x16 *************************************************
		// 
		// x=48,y=16,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 22, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 15, 23, 8, 48, 16, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=16,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 28, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 19, 29, 8, 48, 16, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=16,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 33, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 23, 34, 8, 48, 16, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=16,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 79, 8, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 55, 80, 8, 16, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=16,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 111, 8, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 71, 112, 8, 48, 16, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 4th 16x32/32x16 *************************************************
		// 
		// x=48,y=0,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 23, 5, 48, 0, node_info[5])
		.compare(CHANNEL_0, 14, 24, 5, 48, 0, node_info[3], { node_info[4],node_info[5] })
		// x=32,y=16,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 29, 7, 32, 16, node_info[11])
		.compare(CHANNEL_1, 18, 30, 7, 32, 16, node_info[9], { node_info[10],node_info[11] })
		// x=16,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 80, 7, 16, 32, node_info[29])
		.compare(CHANNEL_3, 54, 81, 7, 16, 32, node_info[26], { node_info[28], node_info[29] })
		// x=32,y=16,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 112, 7, 32, 16, node_info[41])
		.compare(CHANNEL_4, 70, 113, 7, 32, 16, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 2nd 32x32/16x64/64x16 *************************************************
		// 
		// x=32,y=0,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 24, 5, 32, 0, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 32, 5, 32, 0, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 34, 5, 32, 0, node_info[15])
		.compare(CHANNEL_0, 12, 18, 35, 32, 0, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })
		// x=16,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 81, 5, 16, 0, node_info[26])
		.compare(CHANNEL_3, 52, 82, 5, 16, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })
		// x=0,y=16,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 113, 5, 0, 16, node_info[38])
		.compare(CHANNEL_4, 68, 114, 5, 0, 16, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// ******************************************* 1st 32x64/64x32 *************************************************
		// 
		// x=0,y=0,w=32,h=64,node=21
		.bypass(CHANNEL_3, BYPASS_3, 82, 1, 0, 0, node_info[24])
		.compare(CHANNEL_3, 48, 83, 1, 0, 0, node_info[21], { node_info[23],node_info[24] })
		// x=0,y=0,w=64,h=32,node=33
		.bypass(CHANNEL_4, BYPASS_4, 114, 1, 0, 0, node_info[36])
		.compare(CHANNEL_4, 64, 115, 1, 0, 0, node_info[33], { node_info[35],node_info[36] })

		// ******************************************* 9th 16x16 *************************************************
		// 
		// x=0,y=32,w=32,h=32,node=1
		.load_kernel(CHANNEL_0, 35, 9, NO_MODE_CONS, 8 * cuw, TREE_LC, 0, 32, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=0,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 24, 36, 9, 0, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=32,w=16,h=16,node=11
		.compare(CHANNEL_1, 28, 41, 9, 0, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=32,w=16,h=16,node=15
		.compare(CHANNEL_2, 32, 47, 9, 0, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=0,w=32,h=64,node=21
		.load_kernel(CHANNEL_3, 83, 9, NO_MODE_CONS, 8, TREE_LC, 32, 0, { node_info[23],node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=32,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 56, 84, 9, 32, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=32,w=64,h=32,node=33
		.load_kernel(CHANNEL_4, 115, 9, NO_MODE_CONS, 8 * cuw, TREE_LC, 0, 32, { node_info[35],node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=32,w=16,h=16,node=41
		.compare(CHANNEL_4, 72, 116, 9, 0, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 10th 16x16 *************************************************
		// 
		// x=0,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 36, 10, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 25, 37, 10, 0, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=32,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 41, 10, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 29, 42, 10, 16, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=32,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 47, 10, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 33, 48, 10, 16, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 84, 10, NO_MODE_CONS, 4 * cuw + 8, TREE_LC, 32, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 57, 85, 10, 32, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=32,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 116, 10, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 73, 117, 10, 16, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 5th 16x32/32x16 *************************************************
		// 
		// x=0,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 37, 9, 0, 32, node_info[5])
		.compare(CHANNEL_0, 24, 38, 9, 0, 32, node_info[3], { node_info[4],node_info[5] })
		// x=0,y=32,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 42, 9, 0, 32, node_info[11])
		.compare(CHANNEL_1, 28, 43, 9, 0, 32, node_info[9], { node_info[10],node_info[11] })
		// x=32,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 86, 9, 32, 0, node_info[29])
		.compare(CHANNEL_3, 56, 87, 9, 32, 0, node_info[26], { node_info[28], node_info[29] })
		// x=0,y=32,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 117, 9, 0, 32, node_info[41])
		.compare(CHANNEL_4, 72, 118, 9, 0, 32, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 11th 16x16 *************************************************
		// 
		// x=16,y=32,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 38, 11, NO_MODE_CONS, 8 * cuw + 4, TREE_LC, 16, 32, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=16,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 26, 38, 11, 16, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=0,y=48,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 43, 11, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=0,y=48,w=16,h=16,node=11
		.compare(CHANNEL_1, 30, 44, 11, 0, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=0,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 48, 11, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 34, 49, 11, 0, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 87, 11, NO_MODE_CONS, 8 * cuw + 8, TREE_LC, 32, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=32,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 58, 88, 11, 32, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=32,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 118, 11, NO_MODE_CONS, 8 * cuw + 8, TREE_LC, 32, 32, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=32,w=16,h=16,node=41
		.compare(CHANNEL_4, 74, 119, 11, 32, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 12th 16x16 *************************************************
		// 
		// x=16,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 38, 12, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 27, 39, 12, 16, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=16,y=48,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 44, 12, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 31, 45, 12, 16, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=16,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 49, 12, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 35, 50, 12, 16, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 88, 12, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 59, 89, 12, 32, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=32,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 119, 12, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 75, 120, 12, 48, 32, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 6th 16x32/32x16 *************************************************
		// 
		// x=16,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 39, 11, 16, 32, node_info[5])
		.compare(CHANNEL_0, 26, 40, 11, 16, 32, node_info[3], { node_info[4],node_info[5] })
		// x=0,y=48,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 45, 11, 0, 48, node_info[11])
		.compare(CHANNEL_1, 30, 46, 11, 0, 48, node_info[9], { node_info[10],node_info[11] })
		// x=32,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 89, 11, 32, 32, node_info[29])
		.compare(CHANNEL_3, 58, 90, 11, 32, 32, node_info[26], { node_info[28], node_info[29] })
		// x=32,y=32,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 120, 11, 32, 32, node_info[41])
		.compare(CHANNEL_4, 74, 121, 11, 32, 32, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 3rd 32x32/16x64/64x16 *************************************************
		// 
		// x=0,y=32,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 40, 9, 0, 32, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 46, 9, 0, 32, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 50, 9, 0, 32, node_info[15])
		.compare(CHANNEL_0, 24, 51, 9, 0, 32, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })
		// x=32,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 90, 9, 32, 0, node_info[26])
		.compare(CHANNEL_3, 56, 91, 9, 32, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })
		// x=0,y=32,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 121, 9, 0, 32, node_info[38])
		.compare(CHANNEL_4, 72, 122, 9, 0, 32, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// ******************************************* 13th 16x16 *************************************************
		// 
		// x=32,y=32,w=32,h=32,node=1
		.load_kernel(CHANNEL_0, 51, 13, NO_MODE_CONS, 8 * cuw + 8, TREE_LC, 32, 32, { node_info[2],node_info[4],node_info[6],node_info[7],node_info[8],node_info[10],node_info[12],node_info[13],node_info[14],node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=32,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 36, 52, 13, 32, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=32,w=16,h=16,node=11
		.compare(CHANNEL_1, 40, 58, 13, 32, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=32,w=16,h=16,node=15
		.compare(CHANNEL_2, 44, 64, 13, 32, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=0,w=16,h=64,node=24
		.load_kernel(CHANNEL_3, 91, 13, NO_MODE_CONS, 12, TREE_LC, 48, 0, { node_info[25],node_info[27],node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=48,y=0,w=16,h=16,node=29
		.compare(CHANNEL_3, 60, 92, 13, 48, 0, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=0,y=48,w=64,h=16,node=36
		.load_kernel(CHANNEL_4, 122, 13, NO_MODE_CONS, 12 * cuw, TREE_LC, 0, 48, { node_info[37],node_info[39],node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=0,y=48,w=16,h=16,node=41
		.compare(CHANNEL_4, 76, 123, 13, 0, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 14th 16x16 *************************************************
		// 
		// x=32,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 52, 14, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 37, 53, 14, 32, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=32,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 58, 14, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 41, 59, 14, 48, 32, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=32,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 64, 14, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 45, 65, 14, 48, 32, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=16,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 92, 14, NO_MODE_CONS, 4 * cuw + 12, TREE_LC, 48, 16, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 61, 93, 14, 48, 16, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=16,y=48,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 123, 14, NO_MODE_CONS, 12 * cuw + 4, TREE_LC, 16, 48, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 77, 124, 14, 16, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 7th 16x32/32x16 *************************************************
		// 
		// x=32,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 53, 13, 32, 32, node_info[5])
		.compare(CHANNEL_0, 36, 54, 13, 32, 32, node_info[3], { node_info[4],node_info[5] })
		// x=32,y=32,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 59, 13, 32, 32, node_info[11])
		.compare(CHANNEL_1, 40, 60, 13, 32, 32, node_info[9], { node_info[10],node_info[11] })
		// x=48,y=0,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 93, 13, 48, 0, node_info[29])
		.compare(CHANNEL_3, 60, 94, 13, 48, 0, node_info[26], { node_info[28], node_info[29] })
		// x=0,y=48,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 124, 13, 0, 48, node_info[41])
		.compare(CHANNEL_4, 76, 125, 13, 0, 48, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 15th 16x16 *************************************************
		// 
		// x=48,y=32,w=16,h=32,node=3
		.load_kernel(CHANNEL_0, 54, 15, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[4],node_info[6],node_info[7],node_info[8] })
		// x=48,y=32,w=16,h=16,node=5
		.compare(CHANNEL_0, 38, 55, 15, 48, 32, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=32,y=48,w=32,h=16,node=9
		.load_kernel(CHANNEL_1, 60, 15, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[10], node_info[12],node_info[13],node_info[14] })
		// x=32,y=48,w=16,h=16,node=11
		.compare(CHANNEL_1, 42, 61, 15, 32, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=32,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 65, 15, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 46, 66, 15, 32, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=32,w=16,h=32,node=26
		.load_kernel(CHANNEL_3, 94, 15, NO_MODE_CONS, 8 * cuw + 12, TREE_LC, 48, 32, { node_info[28],node_info[30],node_info[31],node_info[32] })
		// x=48,y=32,w=16,h=16,node=29
		.compare(CHANNEL_3, 62, 95, 15, 48, 32, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=32,y=48,w=32,h=16,node=38
		.load_kernel(CHANNEL_4, 125, 15, NO_MODE_CONS, 12 * cuw + 8, TREE_LC, 32, 48, { node_info[40],node_info[42],node_info[43],node_info[44] })
		// x=32,y=48,w=16,h=16,node=41
		.compare(CHANNEL_4, 78, 126, 15, 32, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 16th 16x16 *************************************************
		// 
		// x=48,y=48,w=16,h=16,node=5
		.load_kernel(CHANNEL_0, 55, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[6],node_info[7],node_info[8] })
		.compare(CHANNEL_0, 39, 56, 16, 48, 48, node_info[5], { node_info[6],node_info[7],node_info[8] })
		// x=48,y=48,w=16,h=16,node=11
		.load_kernel(CHANNEL_1, 61, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[12],node_info[13],node_info[14] })
		.compare(CHANNEL_1, 43, 62, 16, 48, 48, node_info[11], { node_info[12],node_info[13],node_info[14] })
		// x=48,y=48,w=16,h=16,node=15
		.load_kernel(CHANNEL_2, 66, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[16],node_info[17],node_info[18],node_info[19] })
		.compare(CHANNEL_2, 47, 67, 16, 48, 48, node_info[15], { node_info[16],node_info[17],node_info[18],node_info[19] })
		// x=48,y=48,w=16,h=16,node=29
		.load_kernel(CHANNEL_3, 95, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[30],node_info[31],node_info[32] })
		.compare(CHANNEL_3, 63, 96, 16, 48, 48, node_info[29], { node_info[30],node_info[31],node_info[32] })
		// x=48,y=48,w=16,h=16,node=41
		.load_kernel(CHANNEL_4, 126, 16, NO_MODE_CONS, 12 * cuw + 12, TREE_LC, 48, 48, { node_info[42],node_info[43],node_info[44] })
		.compare(CHANNEL_4, 79, 127, 16, 48, 48, node_info[41], { node_info[42],node_info[43],node_info[44] })

		// ******************************************* 8th 16x32/32x16 *************************************************
		// 
		// x=48,y=32,w=16,h=32,node=3
		.bypass(CHANNEL_0, BYPASS_0, 56, 15, 48, 32, node_info[5])
		.compare(CHANNEL_0, 38, 57, 15, 48, 32, node_info[3], { node_info[4],node_info[5] })
		// x=32,y=48,w=32,h=16,node=9
		.bypass(CHANNEL_1, BYPASS_1, 62, 15, 32, 48, node_info[11])
		.compare(CHANNEL_1, 42, 63, 15, 32, 48, node_info[9], { node_info[10],node_info[11] })
		// x=48,y=32,w=16,h=32,node=26
		.bypass(CHANNEL_3, BYPASS_3, 96, 15, 48, 32, node_info[29])
		.compare(CHANNEL_3, 62, 97, 15, 48, 32, node_info[26], { node_info[28], node_info[29] })
		// x=32,y=48,w=32,h=16,node=38
		.bypass(CHANNEL_4, BYPASS_4, 127, 15, 32, 48, node_info[41])
		.compare(CHANNEL_4, 78, 128, 15, 32, 48, node_info[38], { node_info[40],node_info[41] })

		// ******************************************* 4th 32x32/16x64/64x16 *************************************************
		// 
		// x=32,y=32,w=32,h=32,node=1
		.bypass(CHANNEL_0, BYPASS_0, 57, 13, 32, 32, node_info[3])
		.bypass(CHANNEL_1, BYPASS_1, 63, 13, 32, 32, node_info[9])
		.bypass(CHANNEL_2, BYPASS_2, 67, 13, 32, 32, node_info[15])
		.compare(CHANNEL_0, 36, 68, 13, 32, 32, node_info[1], { node_info[2],node_info[3],node_info[9],node_info[15] })
		// x=48,y=0,w=16,h=64,node=24
		.bypass(CHANNEL_3, BYPASS_3, 97, 12, 48, 0, node_info[26])
		.compare(CHANNEL_3, 60, 98, 12, 48, 0, node_info[24], { node_info[25], node_info[26], node_info[27] })
		// x=0,y=48,w=64,h=16,node=36
		.bypass(CHANNEL_4, BYPASS_4, 128, 13, 0, 48, node_info[38])
		.compare(CHANNEL_4, 76, 129, 13, 0, 48, node_info[36], { node_info[37],node_info[38],node_info[39] })

		// ******************************************* 2nd 32x64/64x32 *************************************************
		// 
		// x=32,y=0,w=32,h=64,node=21
		.bypass(CHANNEL_3, BYPASS_3, 98, 9, 32, 0, node_info[24])
		.compare(CHANNEL_3, 56, 99, 9, 32, 0, node_info[21], { node_info[23],node_info[24] })
		// x=0,y=32,w=64,h=32,node=33
		.bypass(CHANNEL_4, BYPASS_4, 129, 9, 0, 32, node_info[36])
		.compare(CHANNEL_4, 72, 130, 9, 0, 32, node_info[33], { node_info[35],node_info[36] })

		// ******************************************* 1st 64x64 *************************************************
		// 
		// x=0,y=0,w=64,h=64,node=0
		.bypass(CHANNEL_0, BYPASS_0, 68, 1, 0, 0, node_info[1])
		.bypass(CHANNEL_3, BYPASS_3, 99, 1, 0, 0, node_info[21])
		.bypass(CHANNEL_4, BYPASS_4, 130, 1, 0, 0, node_info[33])
		.compare(CHANNEL_0, 0, 131, 1, 0, 0, node_info[0], { node_info[1],node_info[20],node_info[21],node_info[33] })

		.output(CHANNEL_0, 131, x, y)
		.done()
		.compile(load, pu, store);
}

void md_run_64_reg(int x, int y, Register& reg)
{
	reg.boot() = (x == 0 && y == 0) ? 1 : 0;
	reg.x() = x;
	reg.y() = y;
	reg.load_addr() = 0;
	reg.pu_addr() = 0;
	reg.store_addr() = 0;
}