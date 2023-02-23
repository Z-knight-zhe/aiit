#include "com_def.h"
#include "enc_bsw.h"
#include "md_cmod.h"
#include "iime.h"
#include "ap_int.h"
#include <hls_stream.h>
#ifndef __MERLIN__FETCH_H__

#define ADD_by_YHY 1
#define DEBUG_CHECK 0

#define     FETCH_PRAGMA_ON         1		// 
#define     FETCH_INTERFACE_ON      1
#define     FETCH_COM_REF_DEBUG     1

#define __MERLIN__FETCH_H__
typedef u32 SBAC_CTX_MODEL;
typedef s16 pel;
typedef int CPMV;
typedef long long COM_MTIME;
typedef struct _COM_IMGB COM_IMGB;
typedef struct _PICBUF_ALLOCATOR PICBUF_ALLOCATOR;
typedef struct _ENC_PINTER ENC_PINTER;
typedef struct SAOstatdata SAOStatData;
typedef int BOOL;
typedef struct _SKIP_MERGE_INPUT SKIP_MERGE_INPUT;

#endif

#define NUM_ELEMENTS 8
#define FETCH_REF_BLOCK 0
typedef ap_uint<128> uint128_t;


typedef struct _FETCH_CTX
{
    u8  ctx_info_pic_header_slice_type;
    int ctx_core_x_pel;
    int ctx_core_y_pel;
    int ctx_info_pic_width;
    int ctx_info_pic_height;
    int ctx_info_pic_width_in_scu;
    int ctx_info_pic_height_in_scu;
    int ctx_pinter_refp_pic_stride_luma;
    int ctx_pinter_refp_pic_stride_chroma;
    int ctx_pinter_refp_pic_width_luma;
    int ctx_pinter_refp_pic_height_luma;
    int ctx_pinter_refp_pic_width_chroma;
    int ctx_pinter_refp_pic_height_chroma;
}FETCH_CTX;

static void init_neb_info(ENC_ME_NEB_INFO* neb)
{
#if FETCH_PRAGMA_ON
//#pragma HLS ARRAY_PARTITION variable=neb->mv complete dim=0
//#pragma HLS ARRAY_PARTITION variable=neb->refi complete dim=0
#endif  
    int i = 0;
    //neb->scu = scu;

    {
#if FETCH_PRAGMA_ON
#pragma HLS unroll
#pragma HLS loop_tripcount max = 2
#endif
        neb->mv[i][0] = ((s16)0);
        neb->mv[i][1] = ((s16)0);
        neb->refi[i] = ((s8)(-1));
    }
    //neb->is_valid = ((u8 )0);
}
//extern IP_MAP_SCU *MAP_SCU_IP_ptr;


static void init_ime_line(int* core_x_pel, int* core_y_pel, CTX_FETCH* fetch_ctx_io,
    ENC_ME_LINE_MAP* me_line_ptr, ENC_ME_NEB_INFO inner_nebs[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN])
{
    int x = *core_x_pel;
    int y = *core_y_pel;
    int i = 0;
    int j = 0;
    int above_scu;
    int left_scu;
    int x_scu = x >> 2;
    int y_scu = y >> 2;
    int lcu_width_in_scu = 1 << 7 >> 1 >> 2;
    int pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
    int scu = y_scu * pic_width_in_scu + x_scu;
    int temp_scu;
    //me_line_ptr->min_scu = scu;
    me_line_ptr->min_scu_x = x_scu;
    me_line_ptr->min_scu_y = y_scu;
    me_line_ptr->stride_in_map = pic_width_in_scu;
    me_line_ptr->stride_in_line = lcu_width_in_scu;
    me_line_ptr->pic_width_in_scu = fetch_ctx_io->ctx_info_pic_width_in_scu;
    me_line_ptr->pic_height_in_scu = fetch_ctx_io->ctx_info_pic_height_in_scu;
    me_line_ptr->curr_ctu_idx = 0;
    temp_scu = scu;
    above_scu = scu - pic_width_in_scu - 1;
    left_scu = scu - 1;
    //me_line_ptr->if_init = 1;
    /*MAP_SCU_IP_ptr->min_scu_x = x_scu;
    MAP_SCU_IP_ptr->min_scu_y = y_scu;
    MAP_SCU_IP_ptr->curr_ctu_idx = 0;*/

    //    for (i = 0; i < 5; i++) {
    //
    //#pragma HLS pipeline
    //        //#pragma HLS loop_tripcount max = 5
    //        me_line_ptr->refi_nebs[i] = ((s8)0);
    //       // me_line_ptr->scu_nebs[i] = ((u32)0);
    //        me_line_ptr->mv_nebs[i][0] = ((s16)0);
    //        me_line_ptr->mv_nebs[i][1] = ((s16)0);
    //    }
    for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {
#pragma HLS pipeline
        //#pragma HLS loop_tripcount max = 8
        for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

            //#pragma HLS loop_tripcount max = 2
            me_line_ptr->map_scu_inner[0][i][j] = ((u32)0);
            me_line_ptr->map_cu_mode_inner[0][i][j] = ((u32)0);
            init_neb_info(&inner_nebs[0][i][j]);
        }
    }
    for (i = 0; i < 1 << 7 >> 1 >> 2; i++) {

#pragma HLS pipeline
        //#pragma HLS loop_tripcount max = 8
        for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

            //#pragma HLS loop_tripcount max = 2
            me_line_ptr->map_scu_inner[1][i][j] = ((u32)0);
            me_line_ptr->map_cu_mode_inner[1][i][j] = ((u32)0);
            init_neb_info(&inner_nebs[1][i][j]);
        }
    }
    /*for (i = 0; i < 1 << 7 >> 1 >> 2;  i++) {

#pragma HLS loop_tripcount max = 8
        for (j = 0; j < 1 << 7 >> 1 >> 2; j++) {

#pragma HLS loop_tripcount max = 2
            MAP_SCU_IP_ptr->map_scu_inner[0][i][j] = ((u32 )0);
            MAP_SCU_IP_ptr->map_scu_inner[1][i][j] = ((u32 )0);
        }
    }*/
}

#if ADD_by_YHY
void fetch_run(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
#if PATCH_DEBUG
    PATCH_INFO* patch,
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
    U32 map_cu_mode_inner_to_store[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
    AEC_FW* fetch2dbk_firmware,
    AEC_FW* fetch2aec_firmware,
///////////////////////////////////////////////////////////////////////////////////////////////////////
    CTX_FETCH* fetch_ctx_io,
    U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1,
    //COM_REFP* ctx_pinter_refp,
    COM_MAP* ctx_map,
    U11* ctx_lcu_cnt,
    CTU_Level_Parameters* fetch_output,
    FETCH2MD_FW* fetch2md_fw_ptr,
    FETCH2DBK_FW* fetch2dbk_fw_ptr,
    FETCH2SAO_FW* fetch2sao_fw_ptr,
    Slice_Level_Parameters* ctu_ctrl_input,
   // int* lcu_x,
   // int* lcu_y,
    FETCH_ORGI_LCU_IME* Fetch_Orig_Lcu_IME,
    strFetch_Orig_Lcu* Fetch_Orig_Lcu,
    strFetch_input_video_U8_ptr* Fetch_input_video_U8_ptr,
    FETCH_REF_WINDOW_IME* Fetch_Ref_Window_IME,
    strFetch_ref_window* Fetch_Ref_window,
    strFetch_ref_window* Fetch_Ref_window_md2chroma,
    SKIP_MERGE_INPUT* skip_input_ptr,
    ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME_1[ABOVE_LINE_LEN],
    ENC_ME_LINE_MAP* me_line_map_ptr,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME_1,
    U2& use_deblock,
    U7& real_lcu_width,
    U7& real_lcu_height
#if ENABLE_BFRAME
#if FETCH_COM_REF_DEBUG
    //, U1 flag_valid_map_mv
    //, U1 flag_valid_map_refi
    //, U1 flag_valid_list_ptr
    //, U1 flag_valid_map_mv_1
    //, U1 flag_valid_map_refi_1
    //, U1 flag_valid_list_ptr_1
    , COM_REFP* ctx_pinter_refp_0
    , COM_REFP* ctx_pinter_refp_1

    , u32* list_ptr_0
    , u32* list_ptr_1
    , Register* mdreg
	, Register* fetch2md_mdreg
    //, U32 refp_ptr_test[MAX_NUM_REF_PICS][REFP_NUM]
    //, U1 is_library_picture[MAX_NUM_REF_PICS][REFP_NUM]
#else
	, COM_REFP(*ctx_pinter_refp_all)[REFP_NUM]
#endif  // #if FETCH_COM_REF_DEBUG
#endif

#if Debug422
    , u8* chroma_format
#endif
);
#else
void fetch_run(

#if PATCH_DIVIDE_ENABLE
    int multislice_mode,

#endif
#if PATCH_DEBUG
    PATCH_INFO* patch,

#endif
    CTX_FETCH* fetch_ctx_io,
    COM_REFP* ctx_pinter_refp,
    COM_MAP* ctx_map,
    U11* ctx_lcu_cnt,
    CTU_Level_Parameters* fetch_output,
    FETCH2MD_FW* fetch2md_fw_ptr,
    FETCH2DBK_FW* fetch2dbk_fw_ptr,
    FETCH2SAO_FW* fetch2sao_fw_ptr,
    Slice_Level_Parameters* ctu_ctrl_input,
   // int* lcu_x,
   // int* lcu_y,
    strFetch_Orig_Lcu* Orig_Lcu_ptr,
    strFetch_input_video_U8_ptr* Fetch_input_video_U8_ptr,
    strFetch_ref_window* Fetch_Refwin_ptr,
    SKIP_MERGE_INPUT* skip_input_ptr,
    ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME[ABOVE_LINE_LEN],
    ENC_ME_LINE_MAP* me_line_map_ptr,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
	U6 &real_lcu_width,
	U6 &real_lcu_height

#if ENABLE_BFRAME
	, COM_REFP(*ctx_pinter_refp_all)[REFP_NUM]
        
#endif
#if Debug422
    , u8* chroma_format

#endif
);

#endif

void image_align16 (pel* dst, pel* src, U32 src_width, U32 src_height);
void align16_to_ap_data (ap_uint<128>* dst, pel* src, U32 src_width, U32 src_height);
void image_align8(pel* dst, pel* src, U32 src_width, U32 src_height);
void align16_to_ap_data_8bit(uint128_t* dst, pel* src, U32 src_width, U32 src_height);
void align8_to_ap_data_8bit(ap_uint<64>* dst, pel* src, U32 src_width, U32 src_height);
void align64_to_ap_data_10bit(uint128_t* dst, pel* src, U32 src_width, U32 src_height);
void align32_to_ap_data_10bit(ap_uint<64>* dst, pel* src, U32 src_width, U32 src_height);
void image_align64(pel* dst, pel* src, U32 src_width, U32 src_height);
void image_align32(pel* dst, pel* src, U32 src_width, U32 src_height);


#if ADD_by_YHY
void fetch_core(
#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
    U32 map_cu_mode_inner_to_store[2][SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN],
    AEC_FW* fetch2dbk_firmware,
    AEC_FW* fetch2aec_firmware,
///////////////////////////////////////////////////////////////////////////////////////////////////////
    CTX_FETCH* fetch_ctx_io,
    U8 ctx_rpm_num_refp_0,
    U8 ctx_rpm_num_refp_1,
    //s32(*map_mv_ref)[REFP_NUM][MV_D],
    //s32(*map_refi_ref)[REFP_NUM],
    //u32* list_ptr,
    s32 map_mv[6400][REFP_NUM][MV_D],
    s32 map_refi[6400][REFP_NUM],
    u32 map_scu[6400],
    u32 map_cu_mode[6400],
    U11* ctx_lcu_cnt,
    CTU_Level_Parameters* fetch_output,
    FETCH2MD_FW* fetch2md_fw_ptr,
    FETCH2DBK_FW* fetch2dbk_fw_ptr,
    FETCH2SAO_FW* fetch2sao_fw_ptr,
    Slice_Level_Parameters* ctu_ctrl_input,
    FETCH_ORGI_LCU_IME* Fetch_Orig_Lcu_IME,
    strFetch_Orig_Lcu* Fetch_Orig_Lcu,
    ap_uint<128>* imgY_org_ap,
    ap_uint<64>* imgU_org_ap,
    ap_uint<64>* imgV_org_ap,
#if DEBUG_CHECK
    ap_uint<128>* imgY_ref_ap,
    ap_uint<128>* imgU_ref_ap,
    ap_uint<128>* imgV_ref_ap,
#if ENABLE_BFRAME
    uint128_t* imgY_b_ap,
    uint128_t* imgU_b_ap,
    uint128_t* imgV_b_ap,
#endif
#endif

    pel refWinY0_ref[192][64],//0
    pel refWinY1_ref[192][64],//1
    pel refWinY2_ref[192][64],//2
    pel refWinY3_ref[192][64],//3
    pel refWinU0_ref[96][32],//0
    pel refWinU1_ref[96][32],//1
    pel refWinU2_ref[96][32],//2
    pel refWinU3_ref[96][32],//3
    pel refWinV0_ref[96][32],//0
    pel refWinV1_ref[96][32],//1
    pel refWinV2_ref[96][32],//2
    pel refWinV3_ref[96][32],//3

    pel refWinY0_B_ref[192][64],//0
    pel refWinY1_B_ref[192][64],//1
    pel refWinY2_B_ref[192][64],//2
    pel refWinY3_B_ref[192][64],//3
    pel refWinU0_B_ref[96][32],//0
    pel refWinU1_B_ref[96][32],//1
    pel refWinU2_B_ref[96][32],//2
    pel refWinU3_B_ref[96][32],//3
    pel refWinV0_B_ref[96][32],//0
    pel refWinV1_B_ref[96][32],//1
    pel refWinV2_B_ref[96][32],//2
    pel refWinV3_B_ref[96][32],//3

    int code_stru,
    FETCH_REF_WINDOW_IME* Fetch_Ref_Window_IME,
    strFetch_ref_window* Fetch_Ref_window,
    strFetch_ref_window* Fetch_Ref_window_md2chroma,
    SKIP_MERGE_INPUT* skip_input_ptr,
    ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME_1[ABOVE_LINE_LEN],
    ENC_ME_LINE_MAP* me_line_map_ptr,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME_1,
    U2& use_deblock,
    U7& real_lcu_width,
    U7& real_lcu_height
#if ENABLE_BFRAME
#if FETCH_COM_REF_DEBUG
    //, U1 flag_valid_map_mv
    //, U1 flag_valid_map_refi
    //, U1 flag_valid_list_ptr
    //, U1 flag_valid_map_mv_1
    //, U1 flag_valid_map_refi_1
    //, U1 flag_valid_list_ptr_1
    , s32 ctx_pinter_refp_map_mv_0[6400][REFP_NUM][MV_D]
    , s32 ctx_pinter_refp_map_mv_1[6400][REFP_NUM][MV_D]
    , s32 ctx_pinter_refp_map_refi_0[6400][REFP_NUM]
    , s32 ctx_pinter_refp_map_refi_1[6400][REFP_NUM]
    , u32 list_ptr_0[17 * 2]
    , u32 list_ptr_1[17 * 2]
    , Register* mdreg
	, Register* fetch2md_mdreg
    //, U32 refp_ptr_test[MAX_NUM_REF_PICS][REFP_NUM]
    //, U1 is_library_picture[MAX_NUM_REF_PICS][REFP_NUM]
#else
    , COM_REFP(*ctx_pinter_refp_all)[REFP_NUM]
#endif  // #if FETCH_COM_REF_DEBUG
#endif
#if Debug422
    , u8* chroma_format
#endif
);
#else
void fetch_core (     

#if PATCH_DIVIDE_ENABLE
    int multislice_mode,
#endif
    CTX_FETCH* fetch_ctx_io,
    S16 map_mv_ref[REFP_NUM][MV_D][2],


    s8 map_refi_ref[REFP_NUM][2],

    u32 list_ptr[MAX_NUM_REF_PICS],
    S16 map_mv[REFP_NUM][MV_D][2],


    s8 map_refi[REFP_NUM][2],
    U32 map_scu[CU_SIZE],
    U32 map_cu_mode[CU_SIZE],
    U11* ctx_lcu_cnt,


    CTU_Level_Parameters* fetch_output,
    FETCH2MD_FW* fetch2md_fw_ptr,
    FETCH2DBK_FW* fetch2dbk_fw_ptr,
    FETCH2SAO_FW* fetch2sao_fw_ptr,
    Slice_Level_Parameters* ctu_ctrl_input,
    strFetch_Orig_Lcu* Orig_Lcu_ptr,
    ap_uint<128>* imgY_org_ap,
    ap_uint<128>* imgU_org_ap,
    ap_uint<128>* imgV_org_ap,
    ap_uint<128>* imgY_ref_ap,
    ap_uint<128>* imgU_ref_ap,
    ap_uint<128>* imgV_ref_ap,
#if ENABLE_BFRAME
    uint128_t* imgY_b_ap,
    uint128_t* imgU_b_ap,
    uint128_t* imgV_b_ap,
#endif
    pel refWinY0[192][64],
    pel refWinY1[192][64],
    pel refWinY2[192][64],
    pel refWinY3[192][64],
    pel refWinU0[96][32],
    pel refWinU1[96][32],
    pel refWinU2[96][32],
    pel refWinU3[96][32],
    pel refWinV0[96][32],
    pel refWinV1[96][32],
    pel refWinV2[96][32],
    pel refWinV3[96][32],
#if ENABLE_BFRAME
    pel refWinY0_b[192][64],
    pel refWinY1_b[192][64],
    pel refWinY2_b[192][64],
    pel refWinY3_b[192][64],
    pel refWinU0_b[96][32],
    pel refWinU1_b[96][32],
    pel refWinU2_b[96][32],
    pel refWinU3_b[96][32],
    pel refWinV0_b[96][32],
    pel refWinV1_b[96][32],
    pel refWinV2_b[96][32],
    pel refWinV3_b[96][32],
#endif
    strFetch_input_video_U8_ptr* Fetch_input_video_U8_ptr,


    strFetch_ref_window* Fetch_Refwin_ptr,
    SKIP_MERGE_INPUT* skip_input_ptr,
    ENC_ME_NEB_INFO new_above_nebs_line[ABOVE_LINE_LEN],
    ENC_ME_NEB_INFO new_above_nebs_line_IME[ABOVE_LINE_LEN],
    ENC_ME_LINE_MAP* me_line_map_ptr,
    ENC_ME_LINE_MAP_IME* me_line_map_ptr_IME,
    U6& real_lcu_width,
    U6& real_lcu_height
#if ENABLE_BFRAME
    , COM_REFP(*ctx_pinter_refp_all)[REFP_NUM]
#endif
#if Debug422
    , u8* chroma_format
#endif
);
#endif

void lcu_fetch_org_y_bus_read(U32 start, U16 stride, uint128_t* img, hls::stream<uint128_t>& sfifo);
void lcu_fetch_org_y_write_back(hls::stream<uint128_t>& sfifo, pel pix[64][64]);
void lcu_fetch_org_y(U32 start, U16 stride, uint128_t* img, pel pix[64][64]);