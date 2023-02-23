

#if PCNL_RMD
#if (!defined(H)) || (!defined(W)) 
#error H or W not define
#endif

#if (H >= 4 * W) || (W >= 4 * H)
#define RMD_LIST_NUM 4
#else
#define RMD_LIST_NUM 5
#endif

#define RMD_CU_NUM (sizeof(RMD_SIZE_NAME(g_cu_parent_idx, W, H)) / sizeof(RMD_SIZE_NAME(g_cu_parent_idx, W, H)[0]))


#if ((H == 4) && (W == 8)) || ((H == 8) && (W == 4))
static void RMD_SIZE_NAME(calc, W, H)(const u32 scu_satd[(MAX_CU_SIZE2 >> MIN_CU_LOG2)][(MAX_CU_SIZE2 >> MIN_CU_LOG2)][IPD_CNT],
	u32 cu_satd0[RMD_CU_NUM][IPD_CNT], u32 cu_satd1[RMD_CU_NUM][IPD_CNT]
#if (W == 4) && (H == 8)
	, u32 cu_satd2[RMD_CU_NUM][IPD_CNT]
#endif                
	)
{
#pragma HLS INLINE OFF
	for (u32 i = 0; i < RMD_CU_NUM; i++) {
		HLS_DIRECTIVE(#pragma HLS PIPELINE)
			int x = RMD_SIZE_NAME(g_cu_parent_idx, W, H)[i][0];
		int y = RMD_SIZE_NAME(g_cu_parent_idx, W, H)[i][1];
		for (int ipm = 0; ipm < IPD_CNT; ipm++) {
			u32 cost_satd = scu_satd[x][y][ipm];
#if (W == 4) && (H == 8)
			cost_satd += scu_satd[x][y + 1][ipm];
#else
			cost_satd += scu_satd[x + 1][y][ipm];
#endif
			cu_satd0[i][ipm] = cost_satd;
			cu_satd1[i][ipm] = cost_satd;
#if (W == 4) && (H == 8)
			cu_satd2[i][ipm] = cost_satd;
#endif
		}
	}
}
#else

#define _RMD_2_OUTPUT_CONDITION()  (((W == 4) && (H == 32)) || ((W == 32) && (H == 4)) || ((W == 8) && (H == 16)) || ((W == 8) && (H == 64)) \
                                 || ((W == 64) && (H == 8)) || ((W == 16) && (H == 32)) || ((W == 16) && (H == 64))  || ((W == 64) && (H == 16)) || ((W == 32) && (H == 64)))
#define RMD_2_OUTPUT_CONDITION() _RMD_2_OUTPUT_CONDITION()

#define _RMD_3_OUTPUT_CONDITION()  (((W == 4) && (H == 16)) || ((W == 16) && (H == 4)) || ((W == 8) && (H == 32)) || ((W == 32) && (H == 8)))
#define RMD_3_OUTPUT_CONDITION() _RMD_3_OUTPUT_CONDITION()


static void RMD_SIZE_NAME(calc, W, H)(const u32 parent_satd[][IPD_CNT],
	u32 cu_satd0[RMD_CU_NUM][IPD_CNT]
#if RMD_3_OUTPUT_CONDITION()
	, u32 cu_satd1[RMD_CU_NUM][IPD_CNT]
	, u32 cu_satd2[RMD_CU_NUM][IPD_CNT]
#elif RMD_2_OUTPUT_CONDITION()
	, u32 cu_satd1[RMD_CU_NUM][IPD_CNT]
#endif              
	)
{
#pragma HLS INLINE OFF
	for (u32 i = 0; i < RMD_CU_NUM; i++) {
		HLS_DIRECTIVE(#pragma HLS PIPELINE)
			//       int x = RMD_SIZE_NAME(g_cu_scu_x0, W, H)[i];
		   //        int y = RMD_SIZE_NAME(g_cu_scu_y0, W, H)[i];
			u8 parent_idx0;
		u8 parent_idx1;
		parent_idx0 = RMD_SIZE_NAME(g_cu_parent_idx, W, H)[i][0];
		parent_idx1 = RMD_SIZE_NAME(g_cu_parent_idx, W, H)[i][1];
		for (int ipm = 0; ipm < IPD_CNT; ipm++) {
			//HLS_DIRECTIVE(#pragma HLS UNROLL  factor=8)                 
			u32 cost_satd = parent_satd[parent_idx0][ipm];
			cost_satd += parent_satd[parent_idx1][ipm];
			cu_satd0[i][ipm] = cost_satd;
#if RMD_3_OUTPUT_CONDITION()
			cu_satd1[i][ipm] = cost_satd;
			cu_satd2[i][ipm] = cost_satd;
#elif RMD_2_OUTPUT_CONDITION()
			cu_satd1[i][ipm] = cost_satd;
#endif
		}
	}
}
#endif // ((H == 4) && (W == 8)) || ((H == 8) && (W == 4))

static void RMD_SIZE_NAME(sort, W, H)(u32 cu_satd[RMD_CU_NUM][IPD_CNT], u8 rmd_output_mode[RMD_CU_NUM][IPD_RDO_CNT])
{
#pragma HLS INLINE
	for (u32 i = 0; i < RMD_CU_NUM; i++) {
		HLS_DIRECTIVE(#pragma HLS PIPELINE II = 1)
			u32 satd[IPD_CNT];
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = satd complete dim = 0)
			int ipred_list[SORT_NUM];
		u32 cand_cost[SORT_NUM];
		HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = ipred_list complete dim = 0)
			HLS_DIRECTIVE(#pragma HLS ARRAY_PARTITION variable = cand_cost complete dim = 0)
			for (int ipm = 0; ipm < IPD_CNT; ipm++) {
				satd[ipm] = cu_satd[i][ipm];
			}
		select_sort_mode(satd, ipred_list, cand_cost);
		insert_sort_mode(32, satd[32], ipred_list, cand_cost);
		for (int j = 0; j < IPD_RDO_CNT; j++) {
			rmd_output_mode[i][j] = ipred_list[j];


		}
	}
	
}


#undef H
#undef W
#undef RMD_LIST_NUM
#undef RMD_CU_NUM
#undef _RMD_2_OUTPUT_CONDITION
#undef RMD_2_OUTPUT_CONDITION
#undef _RMD_3_OUTPUT_CONDITION
#undef RMD_3_OUTPUT_CONDITION



#endif