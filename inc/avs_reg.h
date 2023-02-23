/*
 *                                                                          
 * Copyright (c)  :2022-03-14  AIIT Design Systems Inc.   /
 * Permission is hereby granted, free of charge, to any person obtaining  /
 * a copy of this software and associated documentation files (the   /
 * Software), to deal in the Software without restriction, including   /
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to   /
 * permit persons to whom the Software is furnished to do so, subject to   /
 * the following conditions:  /
 * The above copyright notice and this permission notice shall be included   /
 * in all copies or substantial portions of the Software.  /
 *                                                                          
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND,  /
 * EXPRESS OR IPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF  /
 * ERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  /
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   /
 * CLAI, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  /
 * TORT OR OTHERWISE, ARISING FRO, OUT OF OR IN CONNECTION WITH THE   /
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  /
 *  
 *
 */

#ifndef _AVS_REG__H
#define _AVS_REG__H

#include <string.h>

/*! @brief  [AIIT]:	avs register base space    */
#define AVS_REG_BASE										(0x00200000)
#define AVS_IRQ_OFFSET                                      (0)
#define AVS_REG_FETCH1_BASE									(AVS_REG_BASE + 0x1000)
#define AVS_REG_FETCH2_BASE									(AVS_REG_BASE + 0x2000)
#define AVS_REG_MD_BASE										(AVS_REG_BASE + 0x3000)
#define AVS_REG_AEC_BASE									(AVS_REG_BASE + 0x4000)
#define DDR_BASE1											(0x80000000)
#define DDR_BASE2											(0x20000000)

#define AVS_REG_CAP	(4096)

/*! @brief  [AIIT]:	avs FETCH ddr register base space    */
// #define FETCH_CTX_IO_CTX_PINTER_REFP_PTR_OFFSET 			4
// #define FETCH_CTX_IO_REF_PTR_DBK_OFFSET						6
// #define FETCH_CTX_IO_REFP_PTR_TEST_OFFSET					8	
// #define FETCH_CTX_IO_IS_LIBRARY_PICTURE_OFFSET				10			
// #define FETCH_CTX_IO_SLICE_SAO_ENABLE_OFFSET				12
// #define FETCH_CTX_IO_REFP_PTR_OFFSET						14
// #define FETCH_CTX_IO_UMVEOFFSETPICCOUNT_OFFSET				16
// #define FETCH_CTX_IO_CU_DELTA_QP_LCU_MAP_OFFSET				18
// #define MAP_MV_OFFSET										20
// #define MAP_REFI_OFFSET										22
// #define MAP_SCU_OFFSET										24
// #define MAP_CU_MODE_OFFSET									26
// #define CTU_CTRL_INPUT_LAMBDA_OFFSET						28
// #define CTU_CTRL_INPUT_DIST_CHROMA_WEIGHT_OFFSET			30
// #define CTU_CTRL_INPUT_CUQPMAP_OFFSET						32
// #define CTU_CTRL_INPUT_CUQPMAP_FRAME_OFFSET					34
#define IMGY_ORG_OFFSET										4
#define IMGU_ORG_OFFSET										6
#define IMGV_ORG_OFFSET										8
// #define CTX_PINTER_REFP_MAP_MV_0_OFFSET						42
// #define CTX_PINTER_REFP_MAP_MV_1_OFFSET						44
// #define CTX_PINTER_REFP_MAP_REFI_0_OFFSET					46
// #define CTX_PINTER_REFP_MAP_REFI_1_OFFSET					48
// #define LIST_PTR_0_OFFSET									50
// #define LIST_PTR_1_OFFSET									52

/*! @brief  [AIIT]:	avs FETCH register base space    */
#define MULTISLICE_MODE 												4
#define FETCH_CTX_IO_CTX_PTR 											6
#define FETCH_CTX_IO_CTX_PINTER_REFP_PIC_HEIGHT_CHROMA 					8
#define FETCH_CTX_IO_CTX_PINTER_REFP_PIC_HEIGHT_LUMA 					10
#define FETCH_CTX_IO_CTX_PINTER_REFP_PIC_STRIDE_CHROMA 					12
#define FETCH_CTX_IO_CTX_PINTER_REFP_PIC_STRIDE_LUMA 					14
#define FETCH_CTX_IO_CTX_PINTER_REFP_PIC_WIDTH_CHROMA 					16
#define FETCH_CTX_IO_CTX_PINTER_REFP_PIC_WIDTH_LUMA 					18
#define FETCH_CTX_IO_CTX_INFO_PIC_WIDTH 								20
#define FETCH_CTX_IO_CTX_INFO_PIC_HEIGHT 								22
#define FETCH_CTX_IO_CTX_INFO_PIC_WIDTH_IN_LCU 							24
#define FETCH_CTX_IO_CTX_INFO_PIC_HEIGHT_IN_LCU 						26
#define FETCH_CTX_IO_CTX_INFO_PIC_WIDTH_IN_SCU 							28
#define FETCH_CTX_IO_CTX_INFO_PIC_HEIGHT_IN_SCU 						30
#define FETCH_CTX_IO_CTX_INFO_PIC_HEADER_CHROMA_QUANT_PARAM_DELTA_CB 	32
#define FETCH_CTX_IO_CTX_INFO_PIC_HEADER_CHROMA_QUANT_PARAM_DELTA_CR 	34
#define FETCH_CTX_IO_CTX_INFO_PIC_HEADER_SLICE_TYPE 					36
#define FETCH_CTX_IO_SAMPLE_ADAPTIVE_OFFSET_ENABLE_FLAG 				38
#define FETCH_CTX_IO_STRIDE_LUMA 										40
#define FETCH_CTX_IO_STRIDE_CHROMA 										42
#define FETCH_CTX_IO_LCU_CNT 											44
#define FETCH_CTX_IO_FLAG_VALID_MAP_MV 									46
#define FETCH_CTX_IO_FLAG_VALID_MAP_REFI 								48
#define FETCH_CTX_IO_FLAG_VALID_LIST_PTR 								50
#define FETCH_CTX_IO_FLAG_VALID_MAP_MV_1 								52
#define FETCH_CTX_IO_FLAG_VALID_MAP_REFI_1 								54
#define FETCH_CTX_IO_FLAG_VALID_LIST_PTR_1 								56
//#define FETCH_CTX_IO_USE_DEBLOCK 										58
#define FETCH_CTX_IO_LOG2_CULINE 										58
#define FETCH_CTX_IO_MAX_CUWH 											60
#define FETCH_CTX_IO_AFFINE_ENABLE_FLAG 								62
#define FETCH_CTX_IO_UMVE_ENABLE_FLAG 									64
#define FETCH_CTX_IO_NUM_OF_HMVP_CAND 									66
#define FETCH_CTX_IO_AMVR_ENABLE_FLAG 									68
#define FETCH_CTX_IO_EMVR_ENABLE_FLAG 									70
#define FETCH_CTX_IO_DT_INTRA_ENABLE_FLAG 								72
#define FETCH_CTX_IO_POSITION_BASED_TRANSFORM_ENABLE_FLAG 				74
#define FETCH_CTX_IO_TSCPM_ENABLE_FLAG 									76
#define FETCH_CTX_IO_IPF_ENABLE_FLAG 									78
#define FETCH_CTX_IO_FIXED_SLICE_QP_FLAG 								80
#define FETCH_CTX_IO_CU_DELTA_QP_FLAG 									82
#define FETCH_CTX_IO_PH_ISTS_ENABLE_FLAG 								84
#define FETCH_CTX_IO_PH_EPMC_MODEL_FLAG 								86
#define FETCH_CTX_IO_IBC_FLAG 											88
#define FETCH_CTX_IO_SP_PIC_FLAG 										90
#define FETCH_CTX_IO_EVS_UBVS_PIC_FLAG 									92
#define FETCH_CTX_IO_UMVE_SET_FLAG 										94
#define FETCH_CTX_IO_PH_IPC_FLAG 										96
#define FETCH_CTX_IO_CU_QP_GROUP_AREA_SIZE 								98
#define FETCH_CTX_IO_SLICE_QP 											100
#define FETCH_CTX_IO_MIN_CU_SIZE 										102
#define FETCH_CTX_IO_MAX_PART_RATIO 									104
#define FETCH_CTX_IO_MAX_SPLIT_TIMES 									106
#define FETCH_CTX_IO_MIN_QT_SIZE 										108
#define FETCH_CTX_IO_MAX_BT_SIZE 										110
#define FETCH_CTX_IO_MAX_EQT_SIZE 										112
#define FETCH_CTX_IO_MAX_DT_SIZE 										114
#define FETCH_CTX_IO_ABVR_ENABLE_FLAG 									116
#define FETCH_CTX_IO_UMVE_ENH_ENABLE_FLAG 								118
#define FETCH_CTX_IO_AWP_ENABLE_FLAG 									120
#define FETCH_CTX_IO_AWP_MVR_ENABLE_FLAG 								122
#define FETCH_CTX_IO_ETMVP_ENABLE_FLAG 									124
#define FETCH_CTX_IO_AFFINE_UMVE_ENABLE_FLAG 							126
#define FETCH_CTX_IO_SMVD_ENABLE_FLAG 									128
#define FETCH_CTX_IO_BGC_ENABLE_FLAG 									130
//#define FETCH_CTX_IO_INTERPF_ENABLE_FLAG 								132
//																		134
#define FETCH_CTX_IO_IPC_ENABLE_FLAG 									132
#define FETCH_CTX_IO_NUM_OF_MVAP_CAND 									134
#define FETCH_CTX_IO_NUM_OF_HBVP_CAND 									136
#define FETCH_CTX_IO_ENHANCE_TSCPM_ENABLE_FLAG							138
#define FETCH_CTX_IO_PMC_ENABLE_FLAG 									140
#define FETCH_CTX_IO_IIP_ENABLE_FLAG 									142
#define FETCH_CTX_IO_SAWP_ENABLE_FLAG 									144
#define FETCH_CTX_IO_SBT_ENABLE_FLAG 									146
#define FETCH_CTX_IO_ISTS_ENABLE_FLAG 									148
#define FETCH_CTX_IO_SRCC_ENABLE_FLAG 									150
#define FETCH_CTX_IO_EIPM_ENABLE_FLAG 									152
#define FETCH_CTX_IO_ST_CHROMA_ENABLE_FLAG 								154
#define FETCH_CTX_IO_EST_ENABLE_FLAG 									156
#define FETCH_CTX_IO_LOG2_MAX_CUWH 										158
#define FETCH_CTX_IO_BIT_DEPTH_INPUT 									160
#define FETCH_CTX_IO_QP_OFFSET_BIT_DEPTH 								162
#define FETCH_CTX_IO_TEMPORAL_ID 										164
#define FETCH_CTX_IO_DATACOL 											166
#define FETCH_CTX_IO_CONS_PRED_MODE 									168
#define FETCH_CTX_IO_TREE_STATUS 										170
#define FETCH_CTX_IO_NUM_DELTA_QP 										172
#define FETCH_CTX_IO_PRED_QP 											174
#define FETCH_CTX_IO_CU_QP_GROUP_X 										176
#define FETCH_CTX_IO_CU_QP_GROUP_Y 										178
#define CTX_RPM_NUM_REFP_0 												180
#define CTX_RPM_NUM_REFP_1 												182
#define CTX_LCU_CNT 													184
#define CTX_LCU_CNT_CTRL 												185
//																		186
#define CTU_CTRL_INPUT_F_LCU 											186
#define CTU_CTRL_INPUT_SLICE_ENABLE 									188
#define CTU_CTRL_INPUT_QP_Y 											190
#define CTU_CTRL_INPUT_QP_U 											192
#define CTU_CTRL_INPUT_QP_V 											194
#define CTU_CTRL_INPUT_LAMBDA_MV 										196
#define CTU_CTRL_INPUT_MIPF_ENABLE_FLAG 								198
#define CTU_CTRL_INPUT_BIT_DEPTH_INTERNAL 								200
#define CTU_CTRL_INPUT_DEPTH 											202
#define CTU_CTRL_INPUT_CU_DQP_ENABLE 									204
#define CTU_CTRL_INPUT_USECUQPMAP 										206
#define CODE_STRU 														208
#define MDREG_SYS_V_1 													210
#define MDREG_SYS_V_2 													211
#define MDREG_SYS_V_3 													212
#define MDREG_SYS_V_4 													213
#define MDREG_COND0_V_1 												214
#define MDREG_COND0_V_2 												215
#define MDREG_COND0_V_3 												216
#define MDREG_COND0_V_4													217
#define MDREG_COND0_V_5 												219
#define MDREG_COND0_V_6 												220
#define MDREG_COND0_V_7 												221
#define MDREG_COND0_V_8 												222
#define MDREG_COND0_V_9 												223
#define MDREG_COND0_V_10 												224
#define MDREG_COND0_V_11 												225
#define MDREG_COND0_V_12 												226
#define MDREG_COND0_V_13 												227
#define MDREG_COND0_V_14 												228
#define MDREG_COND0_V_15 												229
#define MDREG_COND0_V_16 												230
#define MDREG_COND0_V_17 												231
#define MDREG_COND0_V_18 												232
#define MDREG_COND0_V_19 												233
#define MDREG_COND0_V_20 												234
#define MDREG_COND0_V_21 												235
#define MDREG_COND0_V_22 												236
#define MDREG_COND0_V_23 												237
#define MDREG_COND0_V_24 												238
#define MDREG_COND0_V_25 												239
#define MDREG_COND0_V_26 												240
#define MDREG_COND0_V_27 												241
#define MDREG_COND0_V_28 												242
#define MDREG_COND0_V_29 												243
#define MDREG_COND0_V_30 												244
#define MDREG_COND0_V_31 												245
#define MDREG_COND0_V_32 												246
#define MDREG_COND1_V_1 												247
#define MDREG_COND1_V_2 												248
#define MDREG_COND1_V_3 												249
#define MDREG_COND1_V_4 												250
#define MDREG_COND1_V_5 												252
#define MDREG_COND1_V_6 												253
#define MDREG_COND1_V_7 												254
#define MDREG_COND1_V_8 												255
#define MDREG_COND1_V_9 												256
#define MDREG_COND1_V_10 												257
#define MDREG_COND1_V_11 												258
#define MDREG_COND1_V_12 												259
#define MDREG_COND1_V_13 												260
#define MDREG_COND1_V_14 												261
#define MDREG_COND1_V_15 												262
#define MDREG_COND1_V_16 												263
#define MDREG_COND1_V_17 												264
#define MDREG_COND1_V_18 												265
#define MDREG_COND1_V_19 												266
#define MDREG_COND1_V_20 												267
#define MDREG_COND1_V_21 												268
#define MDREG_COND1_V_22 												269
#define MDREG_COND1_V_23 												270
#define MDREG_COND1_V_24 												271
#define MDREG_COND1_V_25 												272
#define MDREG_COND1_V_26 												273
#define MDREG_COND1_V_27 												274
#define MDREG_COND1_V_28 												275
#define MDREG_COND1_V_29 												276
#define MDREG_COND1_V_30 												277
#define MDREG_COND1_V_31 												278
#define MDREG_COND1_V_32 												279
#define MDREG_CFG0_V_1 													280
#define MDREG_CFG0_V_2 													281
#define MDREG_CFG0_V_3 													282
#define MDREG_CFG0_V_4 													283
#define MDREG_CFG0_V_5 													285
#define MDREG_CFG0_V_6 													286
#define MDREG_CFG0_V_7 													287
#define MDREG_CFG0_V_8 													288
#define MDREG_CFG0_V_9 													289
#define MDREG_CFG0_V_10 												290
#define MDREG_CFG0_V_11 												291
#define MDREG_CFG0_V_12 												292
#define MDREG_CFG0_V_13 												293
#define MDREG_CFG0_V_14 												294
#define MDREG_CFG0_V_15 												295
#define MDREG_CFG0_V_16 												296
#define MDREG_CFG0_V_17 												297
#define MDREG_CFG0_V_18 												298
#define MDREG_CFG0_V_19 												299
#define MDREG_CFG0_V_20 												300
#define MDREG_CFG0_V_21 												301
#define MDREG_CFG0_V_22 												302
#define MDREG_CFG0_V_23 												303
#define MDREG_CFG0_V_24 												304
#define MDREG_CFG0_V_25 												305
#define MDREG_CFG0_V_26 												306
#define MDREG_CFG0_V_27 												307
#define MDREG_CFG0_V_28 												308
#define MDREG_CFG0_V_29 												309
#define MDREG_CFG0_V_30 												310
#define MDREG_CFG0_V_31 												311
#define MDREG_CFG0_V_32 												312
#define MDREG_CFG1_V_1 													313
#define MDREG_CFG1_V_2 													314
#define MDREG_CFG1_V_3 													315
#define MDREG_CFG1_V_4 													316
#define MDREG_CFG1_V_5 													318
#define MDREG_CFG1_V_6 													319
#define MDREG_CFG1_V_7 													320
#define MDREG_CFG1_V_8 													321
#define MDREG_CFG1_V_9 													322
#define MDREG_CFG1_V_10 												323
#define MDREG_CFG1_V_11 												324
#define MDREG_CFG1_V_12 												325
#define MDREG_CFG1_V_13 												326
#define MDREG_CFG1_V_14 												327
#define MDREG_CFG1_V_15 												328
#define MDREG_CFG1_V_16 												329
#define MDREG_CFG1_V_17 												330
#define MDREG_CFG1_V_18 												331
#define MDREG_CFG1_V_19 												332
#define MDREG_CFG1_V_20 												333
#define MDREG_CFG1_V_21 												334
#define MDREG_CFG1_V_22 												335
#define MDREG_CFG1_V_23 												336
#define MDREG_CFG1_V_24 												337
#define MDREG_CFG1_V_25 												338
#define MDREG_CFG1_V_26 												339
#define MDREG_CFG1_V_27 												340
#define MDREG_CFG1_V_28 												341
#define MDREG_CFG1_V_29 												342
#define MDREG_CFG1_V_30 												343
#define MDREG_CFG1_V_31 												344
#define MDREG_CFG1_V_32 												345
//#define FRAME_LEVEL_FW_CHROMA_QUANT_PARAM_DELTA_CB 						
#define FRAME_LEVEL_FW_QP_OFFSET_BIT_DEPTH 								351
//#define FRAME_LEVEL_FW_CHROMA_QUANT_PARAM_DELTA_CR 						
//#define FRAME_LEVEL_FW_FIXED_SLICE_QP_FLAG 								
#define FRAME_LEVEL_FW_PIC_WIDTH 										347
#define FRAME_LEVEL_FW_PIC_HEIGHT                                       349 


/*! @brief  [AIIT]:	avs md register base space    */
#define MEM_LOAD_BUF_V_OFFSET								4
#define MEM_PU_BUF_V_OFFSET									6
#define MEM_STORE_BUF_V_OFFSET								8

/*! @brief [AIIT]:  avs aec register base space */
/*
#define BS_DATA_OFFSET										4
#define BYTE_CNT_FRAME								        6
#define BYTE_CNT_FRAME_AP_VLD                               7
#define BS_STATUS								            8
#define BS_CLEAR										    10
*/

#define AEC_START		0
#define AEC_GLOBAL_INTR_ENABLE	1
#define AEC_IP_INTR_ENABLE	2
#define AEC_IP_INTR_STATUS	3
#define BYTE_CNT_FRAME		4
#define BS_DATA_OFFSET		6
#define LCU_TOTAL		7


/*! @brief  [AIIT]:	AVS yuv data base address    */
#define DRAM_BASE   (0x80000000)
#define DRAM_CAP    (100*SZ_1M)

#define SLICE_I                         	(1)
#define SLICE_P                         	(2)
#define SLICE_B								(3)

/* Max. and avs3_min. Quantization parameter */
#define MIN_QUANT                          0
#define MAX_QUANT_BASE                     63
#define FIXED_LAMBDA_NUM                   1000//ludd
#define FIXED_SQRT_LAMBDA_NUM              65535//ludd

#define COM_MAX(a,b)                   (((a) > (b)) ? (a) : (b))
#define COM_MIN(a,b)                   (((a) < (b)) ? (a) : (b))
#define COM_CLIP(n,avs3_min,avs3_max)            (((n)>(avs3_max))? (avs3_max) : (((n)<(avs3_min))? (avs3_min) : (n)))
#define COM_CLIP3(min_x, max_x, value)   COM_MAX((min_x), COM_MIN((max_x), (value)))

inline int wait_irq_pos(int val)
{
#ifdef RISCV_REGISTER_RW
	uint32_t reg = 0;
    uint32_t i = 0;

    while(1){
        reg = read_mreg32(sync_start);
        if(val == 0){
            if(((uint16_t)(reg >> 16)) == 0x0001)
                break;
        }
		else{
            if(i++ > 100000)
                break;
            else if (i % 20000 == 0)
            {
                //pr_log(DEBUG, "Delay time,wait process finish ......\n");
                printf("Delay time,wait process finish ......\n");
            }
        }
    }
#endif
	return 0;
}

#endif /* _AVS_REG__H */
